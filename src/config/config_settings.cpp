#include "config_settings.h"

// === Version Info ===
const char* version = "1.1";

// === WiFi Defaults ===
const char* default_ssid = "DefaultSSID";
const char* default_password = "DefaultPassword";

// === Server Defaults ===
const char* default_serverIP = "192.168.0.100";
const char* default_roomID = "room1";

// === App Settings ===
const int refresh_interval_seconds = 2;
bool debug_mode = true;
bool draw_current_timeline = false;

// === Schedule Time Settings ===
const int start_hour = 8;
const int end_hour = 18;

// === GUI Constants ===
const int x_end = 400;
const int rect_offset_left = 5;
const int rect_offset_right = 2;
const int vertical_offset = 8;
const int time_label_start = 150;
const int booking_start_offset = 50;
