#include "backend_retriever.h"
#include <cstdio> // for sscanf
#include <ctime>  // for time_t and struct tm
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Arduino.h"
#include "config_settings.h"
#include "config_state.h"
#include <ArduinoJson.h> // ✅ Nodig voor JSON-documenten

bool timeReceived = false;
time_t receivedTime;

extern WiFiClient espClient;
extern PubSubClient client;

extern bool scheduleReceived;

extern String roomID;

JsonDocument latestSchedule;
JsonDocument schedulePayload;


void onTimeMessage(char *topic, byte *payload, unsigned int length)
{
  String timeStr;
  for (unsigned int i = 0; i < length; i++)
  {
    timeStr += (char)payload[i];
  }

  if (debug_mode)
  {
    Serial.print("[DEBUG] Raw time JSON payload: ");
    Serial.println(timeStr);
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, timeStr);
  if (err)
  {
    Serial.print("❌ Failed to parse time JSON: ");
    Serial.println(err.f_str());
    return;
  }

  const char *raw = doc["time"];
  if (!raw)
  {
    Serial.println("❌ No 'time' field in payload.");
    return;
  }

  struct tm tm;
  if (sscanf(raw, "%d-%d-%dT%d:%d:%d",
             &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
             &tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6)
  {
    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    time_t t = mktime(&tm);
    struct timeval now = {.tv_sec = t};
    settimeofday(&now, nullptr);
    timeReceived = true;
    receivedTime = t;

    if (debug_mode)
    {
      Serial.print("[MQTT] Time sync OK: ");
      Serial.println(ctime(&t));
    }
  }
  else
  {
    Serial.println("❌ Failed to parse timestamp string.");
  }
}

void syncTimeFromServer()
{
  timeReceived = false;

  String roomID = getRoomID();
  String responseTopic = "bezetpanel/time/response/" + roomID;
  String requestTopic = "bezetpanel/time/request/" + roomID;

  // client.subscribe(responseTopic.c_str());
  client.publish(requestTopic.c_str(), "get_time");

  unsigned long start = millis();
  while (!timeReceived && millis() - start < 3000)
  {
    client.loop(); // wait for message
    delay(10);
  }

  if (!timeReceived && debug_mode)
  {
    Serial.println("❌ Time sync via MQTT failed.");
  }

  // client.unsubscribe(responseTopic.c_str());
}

void onScheduleMessage(char *topic, byte *payload, unsigned int length) {
  if (debug_mode) {
    Serial.println("[DEBUG] Schedule payload received:");
    for (unsigned int i = 0; i < length; i++) Serial.print((char)payload[i]);
    Serial.println();
  }

  DeserializationError err = deserializeJson(schedulePayload, payload, length);
  if (err) {
    Serial.print("⚠️ Schedule JSON parse error: ");
    Serial.println(err.f_str());
    return;
  }

  // Kopieer inhoud naar latestSchedule
  latestSchedule.clear();
  latestSchedule.set(schedulePayload);

  scheduleReceived = true;

  if (debug_mode) {
    Serial.println("[DEBUG] Schedule payload stored and marked as received.");
  }
}



bool fetchSchedule(JsonDocument &target)
{
  scheduleReceived = false; // Reset status vóór verzending

  String topic = "bezetpanel/schedule/request/" + roomID;
  String payload = "{\"room_id\":\"" + roomID + "\",\"rssi\":" + String(WiFi.RSSI()) + "}";

  if (debug_mode)
  {
    Serial.println("[DEBUG] Fetching schedule from MQTT...");
    Serial.println("[DEBUG] Requesting schedule for room: " + roomID);
    Serial.println("[DEBUG] Request payload: " + payload);
  }

  client.publish(topic.c_str(), payload.c_str());

  if (debug_mode)
  {
    Serial.println("[DEBUG] Schedule request published to: " + topic);
  }

  // 🔁 Wacht maximaal 3000 ms op callback
  unsigned long start = millis();
  while (!scheduleReceived && millis() - start < 5000)
  {
    client.loop();
    delay(10);
  }

  if (scheduleReceived)
  {
    target.clear();
    target.set(latestSchedule);
    bool err=false;
    if (!err)
    {
      if (debug_mode)
        Serial.println("[DEBUG] ✅ Schedule copied to target document.");
      return true;
    }
    else
    {
      Serial.print("⚠️ Failed to parse schedule into target: ");
      //Serial.println(err.f_str());
    }
  }
  else
  {
    Serial.println("❌ No schedule received via MQTT.");
  }

  return false;
}

bool isCurrentLectureActive(time_t now, const char *startStr, const char *endStr)
{
  struct tm startTm = {}, endTm = {};

  if (sscanf(startStr, "%d-%d-%dT%d:%d:%d",
             &startTm.tm_year, &startTm.tm_mon, &startTm.tm_mday,
             &startTm.tm_hour, &startTm.tm_min, &startTm.tm_sec) != 6 ||
      sscanf(endStr, "%d-%d-%dT%d:%d:%d",
             &endTm.tm_year, &endTm.tm_mon, &endTm.tm_mday,
             &endTm.tm_hour, &endTm.tm_min, &endTm.tm_sec) != 6)
  {
    return false;
  }

  startTm.tm_year -= 1900;
  startTm.tm_mon -= 1;
  endTm.tm_year -= 1900;
  endTm.tm_mon -= 1;

  time_t start = mktime(&startTm);
  time_t end = mktime(&endTm);

  return now >= start && now < end;
}