#pragma once
#include <Arduino.h>

void initConfig();  // Call once in setup()

String getServerIP();
String getRoomID();
String getSSID();
String getPassword();
