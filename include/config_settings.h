#pragma once
#include <Arduino.h>

const String version = "1.0";

/* WiFi network settings */
const char *ssid = "Connecting..";
const char *password = "DikkeDekbeer";

/* Server settings */
const String serverIP = "192.168.0.130";
const String roomID = "room1";

/* Refresh interval in seconds */
const int refresh_interval = 2;

/* Activate Debugging mode */
bool debug_mode = true;

/* Draw current time in the schedule as a line [DEPRECATED]*/
bool draw_current_timeline = false;

/* Schedule time config */
const int start_hour = 8;
const int end_hour = 18;

/* Schedule GUI pixel specifications */
const int x_end = 400;
const int rect_offset = 2;
const int vertical_offset = 8;
const int max_title_len = 29;
const int time_label_start = 150;
const int booking_start_offset = 50;


