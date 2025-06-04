#include "storage.h"
#include <Preferences.h>
#include "config_settings.h"

namespace
{
    Preferences prefs;
    const char *ns = "config"; // Preferences namespace
}

// === Load functions ===
String loadSSID()
{
    prefs.begin(ns, true);
    String value = prefs.getString("ssid", String(default_ssid));
    prefs.end();
    return value;
}

String loadPassword()
{
    prefs.begin(ns, true);
    String value = prefs.getString("password", String(default_password));
    prefs.end();
    return value;
}

String loadServerIP()
{
    prefs.begin(ns, true);
    String value = prefs.getString("serverIP", String(default_serverIP));
    prefs.end();
    return value;
}

String loadRoomID()
{
    prefs.begin(ns, true);
    String value = prefs.getString("roomID", String(default_roomID));
    prefs.end();
    return value;
}

// === Save functions ===
void saveSSID(const String &ssid)
{
    prefs.begin(ns, false);
    prefs.putString("ssid", ssid);
    prefs.end();
}

void savePassword(const String &password)
{
    prefs.begin(ns, false);
    prefs.putString("password", password);
    prefs.end();
}

void saveServerIP(const String &ip)
{
    prefs.begin(ns, false);
    prefs.putString("serverIP", ip);
    prefs.end();
}

void saveRoomID(const String &room)
{
    prefs.begin(ns, false);
    prefs.putString("roomID", room);
    prefs.end();
}

// === Clear all ===
void clearPreferences()
{
    prefs.begin(ns, false);
    prefs.clear();
    prefs.end();

    Serial.print("[DEUB] All preferences cleared.");
}
