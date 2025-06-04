// web_config.cpp
#include <web_config.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <Arduino.h>
#include <storage.h>
#include <config_settings.h>
#include "GUI_builder.h"

Preferences prefs;
WebServer server(80);

// Webform here
String htmlForm()
{
    String ssidVal = prefs.getString("ssid", "");
    String passVal = prefs.getString("password", "");
    String serverVal = prefs.getString("server", "");
    String roomVal = prefs.getString("room", "");

    String html = "<!DOCTYPE html><html><head><title>Panel Config</title>";
    html += "<style>";
    html += "body { background-color: #e6f2ff; font-family: Arial, sans-serif; padding: 30px; }";
    html += ".panel { max-width: 400px; margin: auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 15px rgba(0,0,0,0.2); }";
    html += "h2 { text-align: center; color: #333; }";
    html += "label { display: block; margin-top: 15px; font-weight: bold; color: #555; }";
    html += "input[type='text'], input[type='password'] { width: 100%; padding: 10px; margin-top: 5px; border: 1px solid #ccc; border-radius: 5px; }";
    html += "input[type='submit'] { margin-top: 20px; width: 100%; padding: 10px; background-color: #4da6ff; border: none; color: white; font-size: 16px; border-radius: 5px; cursor: pointer; }";
    html += "input[type='submit']:hover { background-color: #3399ff; }";
    html += "</style></head><body>";

    html += "<div class='panel'><h2>Panel Config</h2><form method='POST' action='/save'>";
    html += "<label for='ssid'>SSID:</label><input type='text' name='ssid' value='" + ssidVal + "'>";
    html += "<label for='password'>Password:</label><input type='password' name='password' value='" + passVal + "'>";
    html += "<label for='server'>Server IP:</label><input type='text' name='server' value='" + serverVal + "'>";
    html += "<label for='room'>Room ID:</label><input type='text' name='room' value='" + roomVal + "'>";
    html += "<input type='submit' value='Save'>";
    html += "</form></div></body></html>";

    return html;
}

void handleRoot()
{
    server.send(200, "text/html", htmlForm());
}

void handleSave()
{
    saveSSID(server.arg("ssid"));
    savePassword(server.arg("password"));
    saveServerIP(server.arg("server"));
    saveRoomID(server.arg("room"));

    server.send(200, "text/html", "<h2>Saved. Rebooting...</h2>");
    delay(1000);
    ESP.restart();
}

void startConfigPortal()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP("BezetPanel-Setup");
    Serial.println("⚙️ Config portal running on 192.168.4.1");
    UI_draw_AP_mode();

    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.begin();

    while (true)
    {
        server.handleClient();
        delay(10);
    }
}

bool tryConnectWiFi()
{
    String ssid = loadSSID();
    String pass = loadPassword();
    if (debug_mode)
    {
        Serial.println("[DEBUG] Attempting to connect to WiFi..." + ssid + " with password: " + pass);
    }

    WiFi.begin(ssid.c_str(), pass.c_str());
    for (int i = 0; i < 10; i++)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.print("✅ Connected to ");
            Serial.println(ssid);
            Serial.print("IP: ");
            Serial.println(WiFi.localIP());
            return true;
        }
        delay(1000);
        Serial.print(".");
    }
    return false;
}

void loadPreferences()
{
    prefs.begin("panel", false);
}

String getStoredServerIP()
{
    return prefs.getString("server", "192.168.0.130");
}

String getStoredRoomID()
{
    return prefs.getString("room", "room1");
}
