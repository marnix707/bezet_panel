#pragma once
#include <Arduino.h>

void startConfigPortal();
bool tryConnectWiFi();
void loadPreferences();
String getStoredServerIP();
String getStoredRoomID();
