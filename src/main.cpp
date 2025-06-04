#include <WiFi.h>
#include <HTTPClient.h>

/* Include all project files */
#include "network/backend_debug.h"
#include "network/backend_retriever.h"

#include "display/GUI_builder.h"

// === Fetch and parse schedule ===
bool fetchSchedule(JsonDocument &doc)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    WiFiClient client;
    String url = "http://" + serverIP + ":5000/schedule/" + roomID;
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

// === Time Sync ===
void syncTimeFromServer()
{
  HTTPClient http;
  WiFiClient client;
  String url = "http://" + serverIP + ":5000/now";
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
        Serial.print("✅ Time synced: ");
        Serial.println(timeStr);
      }
      else
      {
        Serial.print("❌ Failed to parse time string.\n");
      }
    }
  }
  else
  {
    Serial.print("❌ Time sync HTTP error: ");
    Serial.println(httpCode);
  }

  http.end();
}

// === LED Helpers ===
void turnRed()
{
  ledcWrite(ledChannelRed, 255);
  ledcWrite(ledChannelGreen, 0);
  ledcWrite(ledChannelBlue, 0);
}

void turnBlue()
{
  ledcWrite(ledChannelRed, 0);
  ledcWrite(ledChannelGreen, 0);
  ledcWrite(ledChannelBlue, 255);
}

// === Screen Change Detection ===
bool screenHasChanged(const uint8_t *current, const uint8_t *previous, size_t size)
{
  for (size_t i = 0; i < size; i++)
  {
    if (current[i] != previous[i])
    {
      return true;
    }
  }
  return false;
}

// === Setup ===
void setup()
{
  Serial.begin(115200);

  // Welcome Message
  Serial.print("---- | Starting up BezetPanel V" + version + " | ----\n");

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("[WIFI] Connecting to " + String(ssid));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\n[WIFI] Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Power on screen
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

  // Init display and clear it
  EPD_GPIOInit();
  // EPD_Init_Fast(Fast_Seconds_1_5s);
  EPD_Clear();
  Paint_NewImage(ImageBW, EPD_W, EPD_H, 0, WHITE);
  EPD_Full(WHITE);
  Paint_NewImage(ImageBW, EPD_W, EPD_H, 0, WHITE);
  EPD_Display_Part(0, 0, EPD_W, EPD_H, ImageBW);
  memcpy(lastImageBW, ImageBW, sizeof(ImageBW));

  // // Fill with black
  // memset(ImageBW, 0x00, sizeof(ImageBW)); // Black = 0x00
  // EPD_Display_Part(0, 0, EPD_W, EPD_H, ImageBW);
  // delay(1000);

  // // Fill with white
  // memset(ImageBW, 0xFF, sizeof(ImageBW)); // White = 0xFF
  // EPD_Display_Part(0, 0, EPD_W, EPD_H, ImageBW);
  // delay(1000);

  // // Repeat one more time (some screens need 2 full cycles)
  // memset(ImageBW, 0x00, sizeof(ImageBW));
  // EPD_Display_Part(0, 0, EPD_W, EPD_H, ImageBW);
  // delay(1000);

  // memset(ImageBW, 0xFF, sizeof(ImageBW));
  // EPD_Display_Part(0, 0, EPD_W, EPD_H, ImageBW);
  // delay(1000);

  syncTimeFromServer();
  init_leds();
}

// === Main loop ===
void loop()
{
  if ((millis() - lastTime) > timerDelay)
  {
    lastTime = millis();

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
      delay(10);
      yield();

      if (debug_mode)
      {
        Serial.print("[DEBUG] Free heap before: ");
        Serial.println(ESP.getFreeHeap());
      }

      UI_draw_info_bar();
      yield();
      delay(5);

      UI_draw_time_labels_vertical(32, 250);
      yield();
      delay(5);

      // UI_draw_time();
      // yield();
      // delay(5);

      // UI_draw_roomID();
      // yield();
      // delay(5);

      render_schedule_and_status(schedule, 32, 250);
      yield();
      delay(10);

      UI_draw_borders();
      yield();
      delay(5);

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

      yield();
      delay(10);
    }
    else
    {
      Serial.println("[ERROR] Failed to fetch schedule.");
    }

    memset(label, 0, sizeof(label)); // Clear UI text data if needed
  }

  delay(10000); // loop throttle
}
