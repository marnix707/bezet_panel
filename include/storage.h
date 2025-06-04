#pragma once
#include <Arduino.h>

// Load saved values
String loadSSID();
String loadPassword();
String loadServerIP();
String loadRoomID();

// Save new values
void saveSSID(const String& ssid);
void savePassword(const String& password);
void saveServerIP(const String& ip);
void saveRoomID(const String& room);

// Erase all preferences
void clearPreferences();
