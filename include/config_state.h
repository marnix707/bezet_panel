#pragma once
#include <Arduino.h>

void initConfig();  // Call once in setup()

extern String getServerIP();
extern String getRoomID();
extern String getSSID();
extern String getPassword();
