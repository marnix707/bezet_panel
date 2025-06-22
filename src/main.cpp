#include <WiFi.h>
#include <HTTPClient.h>

/* Include all project files */
#include "backend_debug.h"
#include "backend_retriever.h"

#include "GUI_builder.h"

#include "EPD_SPI.h"
#include "EPD_GUI.h"
#include "EPD.h"

/* Insert all includes here (is this nessecary?) */
#include "web_config.h"
#include "storage.h"
#include "config_state.h"
#include "leds.h"
#include "constants.h"
#include "config_settings.h"

#include "info_bar.h"
#include "leds.h"


extern unsigned long lastTime;
extern unsigned long timerDelay;

extern uint8_t ImageBW[15000];
extern char label[];
extern uint8_t lastImageBW[];


void printStoredWiFi()
{
  Serial.println("Stored WiFi credentials:");
  Serial.println("SSID: " + getSSID());
  Serial.println("Password: " + getPassword());
}

// === Setup ===
void setup()
{
  Serial.begin(115200);

  Serial.print("----   |   Starting up BezetPanel V" + String(version) + "   |   ----\n");
  initConfig();

  // loadPreferences();
  printStoredWiFi();
  delay(1);

  Serial.println("Trying default WiFi...");
  if (!tryConnectWiFi())
  {
    Serial.println("❌ Could not connect to default WiFi. Starting Config Portal.");

    startConfigPortal(); // Captive portal if no saved WiFi
  }

  Serial.println("✅ WiFi Connected: " + WiFi.localIP().toString());

  String serverIP = getStoredServerIP();
  String roomID = getStoredRoomID();

  // Power on screen
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  // Init and clear display
  EPD_GPIOInit();
  UI_clear_all();

  // Time & LED init
  syncTimeFromServer();
  init_leds();

}

// === Main loop ===
void loop()
{
  
  if ((millis() - lastTime) > timerDelay)
  {
    lastTime = millis();

    // Check WiFi connection
    if (WiFi.status() != WL_CONNECTED && auto_AP_when_disconnected)
    {
      Serial.println("[ERROR] WiFi disconnected.");

      WiFi.disconnect(true); // Erase saved network (optional)
      delay(1000);
      startConfigPortal(); // Revert to AP
      return;
    }

    StaticJsonDocument<1536> doc;

    if (fetchSchedule(doc))
    {
      JsonArray schedule = doc["schedule"];
      if (schedule.isNull())
      {
        Serial.println("[ERROR] No 'schedule' key found.");
        return;
      }

      // === UI rendering ===
      Paint_NewImage(ImageBW, EPD_W, EPD_H, 0, WHITE);
      //yield();

      if (debug_mode)
      {
        Serial.print("[DEBUG] Free heap before: ");
        Serial.println(ESP.getFreeHeap());
      }

      UI_draw_info_bar();    //bring the info bar back later
      UI_draw_time_labels_vertical(32, 250);
      render_schedule_and_status(schedule, 32, 250, label);
      UI_draw_borders();

      if (debug_mode)
      {
        Serial.print("[DEBUG] Free heap after: ");
        Serial.println(ESP.getFreeHeap());
        Serial.println("[DEBUG] Drawing Screen.");
      }

      if (screenHasChanged(ImageBW, lastImageBW, sizeof(ImageBW)))
      {
        if (debug_mode)
        {
          Serial.println("[DEBUG] Screen has changed");
        }
        EPD_Display_Part(0, 0, EPD_W, EPD_H, ImageBW);
        memcpy(lastImageBW, ImageBW, sizeof(ImageBW));
      }
    }
    else
    {
      Serial.println("[ERROR] Failed to fetch schedule.");
    }

    memset(label, 0, sizeof(label)); // Clear UI text data if needed
  }
  delay(10000); // loop throttle
}
