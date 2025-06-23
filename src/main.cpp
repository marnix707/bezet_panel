#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // ✅ Nodig voor JSON-documenten

/* Project files */
#include "backend_debug.h"
#include "backend_retriever.h"
#include "GUI_builder.h"
#include "EPD_SPI.h"
#include "EPD_GUI.h"
#include "EPD.h"
#include "web_config.h"
#include "storage.h"
#include "config_state.h"
#include "leds.h"
#include "constants.h"
#include "config_settings.h"
#include "info_bar.h"

/* Globals */
extern unsigned long lastTime;
extern unsigned long timerDelay;

extern uint8_t ImageBW[15000];
extern char label[];
extern uint8_t lastImageBW[];

extern JsonDocument latestSchedule;
extern JsonDocument schedulePayload;

WiFiClient espClient;
PubSubClient client(espClient);

String roomID;
String serverIP;

bool scheduleReceived = false;

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  String topicStr = String(topic);
  roomID = getRoomID();

  if (debug_mode)
  {
    Serial.println("[DEBUG] MQTT callback triggered.");
    Serial.print("[DEBUG] Received topic: ");
    Serial.println(topicStr);
  }

  String scheduleTopic = "bezetpanel/schedule/response/" + roomID;
  String timeTopic = "bezetpanel/time/response/" + roomID;

  if (topicStr == scheduleTopic)
  {
    if (debug_mode)
    {
      Serial.println("[DEBUG] ✅ Schedule topic matched.");
    }
    onScheduleMessage(topic, payload, length);
  }
  else if (topicStr == timeTopic)
  {
    if (debug_mode)
    {
      Serial.println("[DEBUG] ⏱️ Time sync topic matched.");
    }
    onTimeMessage(topic, payload, length);
  }
  else
  {
    Serial.println("⚠️ MQTT callback received unknown topic:");
    Serial.println("→ " + topicStr);
  }
}

void connectMQTT()
{
  while (!client.connected())
  {
    if (debug_mode)
      Serial.print("[MQTT] Connecting to " + serverIP + " for room " + roomID + "...\n");
    if (client.connect("BezetPanelClient"))
    {
      Serial.println("✅ MQTT connected.");

      roomID = getStoredRoomID(); // of getRoomID();

      // 📌 Hier abonneren op response topics
      String scheduleTopic = "bezetpanel/schedule/response/" + roomID;
      String timeTopic = "bezetpanel/time/response/" + roomID;

      client.subscribe(scheduleTopic.c_str());
      client.subscribe(timeTopic.c_str());

      if (debug_mode)
      {
        Serial.println("[DEBUG] Schedule subscribed to: " + scheduleTopic);
        Serial.println("[DEBUG] Time subscribed to: " + timeTopic);

        Serial.println("[MQTT] Subscribed to schedule and time response topics. with room ID: " + roomID);
      }
    }
    else
    {
      Serial.print(".");
      delay(1000);
    }
  }
}

void printStoredWiFi()
{
  Serial.println("Stored WiFi credentials:");
  Serial.println("SSID: " + getSSID());
  Serial.println("Password: " + getPassword());
}

void setup()
{
  Serial.begin(115200);
  Serial.print("----   |   Starting up BezetPanel V" + String(version) + "   |   ----\n");

  initConfig();
  printStoredWiFi();
  delay(1);

  Serial.println("🔄 Trying default WiFi...");
  if (!tryConnectWiFi())
  {
    Serial.println("❌ Could not connect to default WiFi. Starting Config Portal.");
    startConfigPortal();
  }

  Serial.println("✅ WiFi Connected: " + WiFi.localIP().toString());

  serverIP = getStoredServerIP();
  roomID = getStoredRoomID();

  client.setServer(serverIP.c_str(), 1883);
  client.setCallback(mqttCallback);

  connectMQTT();
  syncTimeFromServer();

  // 🔁 Eerste keer planning ophalen
  JsonDocument tempDoc;
  if (fetchSchedule(tempDoc))
  {
    latestSchedule.clear();
    latestSchedule.set(tempDoc);
    Serial.println("[MQTT] ✅ Schedule fetched and stored.");
  }
  else
  {
    Serial.println("[MQTT] ⚠️ Failed to fetch schedule during setup.");
  }

  // Init scherm en leds
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  EPD_GPIOInit();
  UI_clear_all();
  init_leds();

  Serial.println("✅ Setup complete.");
}

void loop()
{
  client.loop();
  if (!client.connected())
    connectMQTT();

  static unsigned long lastRun = 0;
  static unsigned long lastScheduleRefresh = 0;

  unsigned long now = millis();

  // 🔁 Elke 10 seconden uitvoeren
  if (now - lastRun >= 10000UL)
  {
    lastRun = now;

    // 🛜 WiFi check
    if (WiFi.status() != WL_CONNECTED && auto_AP_when_disconnected)
    {
      Serial.println("[ERROR] WiFi disconnected.");
      WiFi.disconnect(true);
      delay(1000);
      startConfigPortal();
      return;
    }

    // 📡 Status publiceren
    String status = "{\"room_id\":\"" + roomID + "\",\"rssi\":" + String(WiFi.RSSI()) + "}";
    client.publish("bezetpanel/status", status.c_str());

    // 🕒 Elke 30 seconden: planning ophalen
    if (now - lastScheduleRefresh >= 30000UL)
    {
      if (debug_mode)
        Serial.println("[DEBUG] Refreshing schedule...");

      lastScheduleRefresh = now;
      JsonDocument newDoc;
      
      if (fetchSchedule(newDoc))
      {
        latestSchedule.set(newDoc);
        Serial.println("[MQTT] 🔁 Schedule refreshed.");
      }
      else
      {
        Serial.println("[MQTT] ⚠️ Failed to refresh schedule.");
      }
    }

    // 🖥️ UI tekenen als er planning is
    JsonArray schedule = latestSchedule["schedule"];
    if (schedule.isNull())
    {
      Serial.println("[MQTT] No schedule yet.");
      return;
    }

    Paint_NewImage(ImageBW, EPD_W, EPD_H, 0, WHITE);
    if (debug_mode)
    {
      Serial.print("[DEBUG] Free heap before: ");
      Serial.println(ESP.getFreeHeap());
    }

    UI_draw_info_bar();
    UI_draw_time_labels_vertical(32, 250);
    render_schedule_and_status(schedule, 32, 250, label);
    UI_draw_borders();

    if (debug_mode)
    {
      Serial.print("[DEBUG] Free heap after: ");
      Serial.println(ESP.getFreeHeap());
    }

    if (screenHasChanged(ImageBW, lastImageBW, sizeof(ImageBW)))
    {
      Serial.println("[DEBUG] Screen has changed");
      EPD_Display_Part(0, 0, EPD_W, EPD_H, ImageBW);
      memcpy(lastImageBW, ImageBW, sizeof(ImageBW));
    }

    memset(label, 0, sizeof(label));
  }

  // Kleine delay om CPU te ontlasten (optioneel)
  delay(10);
}
