#pragma once

#include <ArduinoJson.h>


void UI_clear_all();

void UI_draw_time();
void UI_draw_roomID();
void UI_draw_info_bar();
void UI_draw_signal(int x, int y);

void UI_draw_borders();

void UI_draw_room_free(boolean room_is_free);

void render_schedule_and_status(JsonArray schedule, int start_y, int total_height, char* label);
void UI_draw_time_labels_vertical(int start_y, int total_height);

void UI_draw_AP_mode();

bool screenHasChanged(const uint8_t *current, const uint8_t *previous, size_t size);


