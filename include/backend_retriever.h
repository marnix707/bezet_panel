#pragma once

#include <ctime>

bool isCurrentLectureActive(time_t now, const char* startStr, const char* endStr);
