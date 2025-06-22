#include "config_settings.h"

// === Version Info ===
const char* version = "1.2";

// === WiFi Defaults ===
const char* default_ssid = "Connecting..";
const char* default_password = "DikkeDekbeer";
const bool auto_AP_when_disconnected = true;

// === Server Defaults ===
const char* default_serverIP = "192.168.0.130";
const char* default_roomID = "room2";

// === App Settings ===
const int refresh_interval_seconds = 2;
bool debug_mode = true;
const int ending_soon_threshold_minutes = 5;
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

// Constants used in array sizes should be macros
#define MAX_TITLE_LEN 30

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = refresh_interval_seconds * 1000; // 2 seconds

// Scheudule calculation


const int minutes_in_day = (end_hour - start_hour) * 60;

