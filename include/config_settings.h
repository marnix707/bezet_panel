#pragma once
#include <Arduino.h>

// Constants used in array sizes should be macros
#define MAX_TITLE_LEN 30

// === Version Info ===
extern const char* version;

// === WiFi Defaults ===
extern const char* default_ssid;
extern const char* default_password;

// === Server Defaults ===
extern const char* default_serverIP;
extern const char* default_roomID;

// === App Settings ===
extern const int refresh_interval_seconds;
extern bool debug_mode;
extern bool draw_current_timeline;

// === Schedule Time Settings ===
extern const int start_hour;
extern const int end_hour;

// === GUI Constants ===
extern const int x_end;
extern const int rect_offset_left;
extern const int rect_offset_right;
extern const int vertical_offset;
extern const int time_label_start;
extern const int booking_start_offset;
