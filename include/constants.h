#pragma once
#include <Arduino.h>

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = refresh_interval_seconds * 1000; // 2 seconds

// Scheudule calculation
const int minutes_in_day = (end_hour - start_hour) * 60;

// Display buffer
static uint8_t lastImageBW[15000];
