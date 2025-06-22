#include <WiFi.h>
#include <PubSubClient.h> 

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

WiFiClient espClient;
PubSubClient client(espClient);

String roomID;
String serverIP;

StaticJsonDocument<1536> latestSchedule;

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  payload[length] = 0;
  String json = String((char *)payload);

  DeserializationError error = deserializeJson(latestSchedule, json);
  if (error)
  {
    Serial.print("[ERROR] MQTT JSON parse failed: ");
    Serial.println(error.f_str());
    return;
  }

  Serial.println("[MQTT] Schedule updated.");
}

void connectMQTT()
{
  while (!client.connected())
  {
    Serial.print("[MQTT] Connecting...");
    if (client.connect("BezetPanelClient"))
    {
      Serial.println("connected.");
      String topic = "bezetpanel/schedule/" + roomID;
      client.subscribe(topic.c_str());
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

  Serial.println("Trying default WiFi...");
  if (!tryConnectWiFi())
  {
    Serial.println("❌ Could not connect to default WiFi. Starting Config Portal.");
    startConfigPortal(); // Captive portal
  }

  Serial.println("✅ WiFi Connected: " + WiFi.localIP().toString());

  serverIP = getStoredServerIP();
  roomID = getStoredRoomID();

  // MQTT setup
  client.setServer(serverIP.c_str(), 1883);
  client.setCallback(mqttCallback);

  // Power on screen
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  // Init display and LEDs
  EPD_GPIOInit();
  UI_clear_all();
  syncTimeFromServer();
  init_leds();
}

void loop()
{
  client.loop();
  if (!client.connected()) connectMQTT();

  // Periodiek status publiceren
  if ((millis() - lastTime) > timerDelay)
  {
    lastTime = millis();

    if (WiFi.status() != WL_CONNECTED && auto_AP_when_disconnected)
    {
      Serial.println("[ERROR] WiFi disconnected.");
      WiFi.disconnect(true);
      delay(1000);
      startConfigPortal();
      return;
    }

    // Publiceer status met RSSI
    String status = "{\"room_id\":\"" + roomID + "\",\"rssi\":" + String(WiFi.RSSI()) + "}";
    client.publish("bezetpanel/status", status.c_str());

    // === UI rendering ===
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

  delay(10000);
}
