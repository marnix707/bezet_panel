#pragma once

#include <ctime>
#include <ArduinoJson.h>

bool isCurrentLectureActive(time_t now, const char* startStr, const char* endStr);
bool fetchSchedule(JsonDocument &doc);
void syncTimeFromServer(); 
