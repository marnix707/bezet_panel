#include "backend_retriever.h"
#include <cstdio> // for sscanf
#include <ctime>  // for time_t and struct tm
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "Arduino.h"
#include "config_settings.h"
#include "config_state.h"

// === Time Sync ===
void syncTimeFromServer()
{
  HTTPClient http;
  WiFiClient client;
  String url = "http://" + getServerIP() + ":5000/now";
  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode == 200)
  {
    String payload = http.getString();

    if (debug_mode)
    {
      Serial.print("[DEBUG] Payload: ");
      Serial.println(payload);
    }
    int timeIndex = payload.indexOf("\"time\":\"");
    if (timeIndex != -1)
    {
      int quote1 = payload.indexOf("\"", timeIndex + 7);
      int quote2 = payload.indexOf("\"", quote1 + 1);
      String timeStr = payload.substring(quote1 + 1, quote2);

      struct tm tm;
      if (sscanf(timeStr.c_str(), "%d-%d-%dT%d:%d:%d",
                 &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
                 &tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 6)
      {
        tm.tm_year -= 1900;
        tm.tm_mon -= 1;
        time_t t = mktime(&tm);
        struct timeval now = {.tv_sec = t};
        settimeofday(&now, nullptr);
        if (debug_mode)
        {
          Serial.print("[DEBUG] Parsed time: ");
          Serial.println(ctime(&t)); // Print human-readable time
        }
      }
      else
      {
        if (debug_mode)
          Serial.print("❌ Failed to parse time string.\n");
      }
    }
  }
  else
  {
    if (debug_mode)
    {
      Serial.print("❌ Time sync HTTP error: ");
      Serial.println(httpCode);
    }
  }

  http.end();
}

// === Fetch and parse schedule ===
bool fetchSchedule(JsonDocument &doc)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    WiFiClient client;
    String url = "http://" + getServerIP() + ":5000/schedule/" + getRoomID() + "?rssi=" + String(WiFi.RSSI());

    http.begin(client, url);

    int httpResponseCode = http.GET();
    if (httpResponseCode == 200)
    {
      DeserializationError error = deserializeJson(doc, http.getStream());

      if (error)
      {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        http.end();
        return false;
      }

      http.end();
      return true;
    }
    else
    {
      Serial.print("HTTP error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  }
  else
  {
    Serial.println("WiFi not connected");
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

// // DEPRECATED
// void printNextUpcomingBooking(const JSONVar& schedule) {
//   time_t now = time(nullptr);
//   JSONVar nextEvent;
//   time_t soonest = 0;

//   for (int i = 0; i < schedule.length(); i++) {
//     if (JSON.typeof(schedule[i]) != "object") {
//       Serial.print("Skipping non-object at index ");
//       Serial.println(i);
//       continue;
//     }

//     JSONVar entry = schedule[i];

//     // Check if "start_time" exists and is a string
//     if (!entry.hasOwnProperty("start_time") || JSON.typeof(entry["start_time"]) != "string") {
//       Serial.print("Missing or invalid 'start_time' at index ");
//       Serial.println(i);
//       continue;
//     }

//     const char* rawStart = (const char*)entry["start_time"];
//     if (rawStart == nullptr) {
//       Serial.print("Null start_time pointer at index ");
//       Serial.println(i);
//       continue;
//     }

//     String startStr = String(rawStart);
//     if (startStr.length() < 19) {
//       Serial.print("start_time too short at index ");
//       Serial.println(i);
//       continue;
//     }

//     // Parse safely
//     int year = startStr.substring(0, 4).toInt();
//     int month = startStr.substring(5, 7).toInt();
//     int day = startStr.substring(8, 10).toInt();
//     int hour = startStr.substring(11, 13).toInt();
//     int minute = startStr.substring(14, 16).toInt();
//     int second = startStr.substring(17, 19).toInt();

//     if (year < 2020 || month < 1 || month > 12 || day < 1 || day > 31 || hour > 23 || minute > 59 || second > 59) {
//       Serial.print("Invalid timestamp at index ");
//       Serial.println(i);
//       continue;
//     }

//     struct tm tmStart = {};
//     tmStart.tm_year = year - 1900;
//     tmStart.tm_mon = month - 1;
//     tmStart.tm_mday = day;
//     tmStart.tm_hour = hour;
//     tmStart.tm_min = minute;
//     tmStart.tm_sec = second;

//     time_t eventTime = mktime(&tmStart);

//     if (eventTime == -1) {
//       Serial.print("mktime failed at index ");
//       Serial.println(i);
//       continue;
//     }

//     if (eventTime > now && (soonest == 0 || eventTime < soonest)) {
//       soonest = eventTime;
//       nextEvent = entry;
//     }
//   }

//   if (soonest != 0 && JSON.typeof(nextEvent) == "object") {
//     Serial.println("Next upcoming booking:");
//     Serial.print("Title: ");
//     Serial.println((const char*)nextEvent["title"]);
//     Serial.print("Start Time: ");
//     Serial.println((const char*)nextEvent["start_time"]);
//     Serial.print("Type: ");
//     Serial.println((const char*)nextEvent["type"]);
//   } else {
//     Serial.println("No upcoming bookings found for today.");
//   }
// }