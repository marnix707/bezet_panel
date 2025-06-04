#include "EPD.h"     // Include the EPD library for controlling the electronic ink screen (E-Paper Display)
#include "EPD_GUI.h" // Include the EPD_GUI library for graphical user interface (GUI) operations
#include "info_bar.h"
#include <graphics/bezet_bitmap.h>
#include "hardware/leds.h"
#include <config_settings.h>
#include <constants.h>
#include <ArduinoJson.h>

// Define a black and white image array as the buffer for the e-paper display
uint8_t ImageBW[15000]; // Define the size based on the resolution of the e-paper display

char label[max_title_len + 1] = {0};

// Clear all content on the display
// void clear_all() {
//   EPD_Clear();                                      // Clear the display content
//   Paint_NewImage(ImageBW, EPD_W, EPD_H, 0, WHITE);  // Create a new image buffer, fill with white
//   EPD_Full(WHITE);                                  // Fill the entire screen with white
//   EPD_Display_Part(0, 0, EPD_W, EPD_H, ImageBW);    // Update the display content
// }

void UI_draw_time()
{
  time_t now = time(nullptr);
  struct tm *now_tm = localtime(&now);

  char buffer[16];
  snprintf(buffer, sizeof(buffer), "%02d:%02d", now_tm->tm_hour, now_tm->tm_min);
  EPD_ShowString(10, 195, buffer, 48, BLACK);
}

void UI_draw_roomID()
{
  // EPD_ShowString(150, 2, roomID.c_str(), 16, BLACK);
  EPD_ShowString(150, 2, String("D1.132 Vak TIS Melektrolab").c_str(), 16, BLACK);
}

void UI_draw_borders()
{
  // Draw the box around bezet
  EPD_DrawLine(0, 144, 144, 144, BLACK); // Draw horizontal line midden stuk van bezet en lokaal
  delay(1);
  EPD_DrawLine(144, 0, 144, 299, BLACK); // Draw vertical line bottom
  delay(1);
  EPD_DrawLine(time_label_start + 50, 24, time_label_start + 50, 299, BLACK); // Draw vertical line bottom (booking divide line)
  delay(1);
  EPD_DrawLine(144, 24, 399, 24, BLACK); // Draw horizontal line info bar
  delay(1);
}

void UI_draw_room_free(boolean room_is_free)
{
  // Make GUI
  if (room_is_free)
  {
    EPD_ShowPicture(0, 0, 144, 144, epd_bitmap_vrij, BLACK); // put in bezet logo
  }
  else
  {
    EPD_ShowPicture(0, 0, 144, 144, epd_bitmap_bezet, BLACK); // put in bezet logo
  }
}

void render_schedule_and_status(JsonArray schedule, int start_y, int total_height)
{
  const int x_start = time_label_start + booking_start_offset;
  const int rect_width = x_end - x_start;

  // clear the area
  EPD_DrawRectangle(x_start, start_y, x_end, start_y + total_height, WHITE, 1);
  yield();

  bool room_free = true;
  time_t now = time(nullptr);

  for (JsonObject item : schedule)
  {
    const char *startStr = item["start_time"];
    const char *endStr = item["end_time"];
    const char *titleStr = item["title"];

    if (!startStr || !endStr)
      continue;

    int sh, sm, eh, em;

    struct tm startTm = {}, endTm = {};
    if (sscanf(startStr, "%d-%d-%dT%d:%d:%d",
               &startTm.tm_year, &startTm.tm_mon, &startTm.tm_mday,
               &startTm.tm_hour, &startTm.tm_min, &startTm.tm_sec) == 6 &&
        sscanf(endStr, "%d-%d-%dT%d:%d:%d",
               &endTm.tm_year, &endTm.tm_mon, &endTm.tm_mday,
               &endTm.tm_hour, &endTm.tm_min, &endTm.tm_sec) == 6)
    {
      startTm.tm_year -= 1900;
      startTm.tm_mon -= 1;
      endTm.tm_year -= 1900;
      endTm.tm_mon -= 1;
      time_t start = mktime(&startTm);
      time_t end = mktime(&endTm);
      if (now >= start && now < end)
        room_free = false;
    }

    sh = String(startStr).substring(11, 13).toInt();
    sm = String(startStr).substring(14, 16).toInt();
    eh = String(endStr).substring(11, 13).toInt();
    em = String(endStr).substring(14, 16).toInt();

    if (sh < start_hour || eh > end_hour + 1 || sm < 0 || em < 0)
      continue;

    int start_minutes = (sh - start_hour) * 60 + sm;
    int end_minutes = (eh - start_hour) * 60 + em;
    if (end_minutes <= start_minutes)
      continue;

    int y_start = start_y + (start_minutes * total_height) / minutes_in_day + vertical_offset;
    int y_end = start_y + (end_minutes * total_height) / minutes_in_day + vertical_offset;

    if (y_start >= y_end || y_end > EPD_H)
      continue;

    // Draw booking block
    bool is_active = isCurrentLectureActive(now, startStr, endStr);

    EPD_DrawRectangle(x_start + rect_offset_left, y_start, x_end - rect_offset_right, y_end, BLACK, is_active ? 1 : 0);
    yield();

    if (titleStr && strlen(titleStr) > 0)
    {
      memset(label, 0, sizeof(label));
      String title = String(titleStr).substring(0, max_title_len);
      title.toCharArray(label, sizeof(label));

      int textY = y_start + (y_end - y_start) / 2 - 8;
      int textX = x_start + (rect_width / 2) - (title.length() * 8 / 2);

      EPD_ShowString(textX, textY, label, 16, is_active ? WHITE : BLACK);
      yield();
    }
  }

  // Draw room status indicator
  if (room_free)
  {
    setColor(0, 1, 0); // Green
    UI_draw_room_free(true);
    if (debug_mode)
    {
      Serial.println("🟢 Room is FREE");
    }
  }
  else
  {
    setColor(1, 0, 0); // Red
    UI_draw_room_free(false);
    if (debug_mode)
    {
      Serial.println("🔴 Room is OCCUPIED");
    }
  }
}

void UI_draw_time_labels_vertical(int start_y, int total_height)
{
  const int total_labels = end_hour - start_hour + 1;
  const int spacing = total_height / (total_labels - 1); // 9 gaps between 10 labels

  char buffer[8];

  for (int i = 0; i < total_labels; i++)
  {
    int hour = start_hour + i;
    snprintf(buffer, sizeof(buffer), "%02d:00", hour);

    int y = start_y + i * spacing;
    EPD_ShowString(time_label_start, y, buffer, 16, BLACK);
    delay(1);
    yield();
  }

  if (draw_current_timeline)
  {
    // Draw horizontal time indicator line
    int hour = 16, minute = 55;
    // get_current_time(&hour, &minute);
    if (hour >= start_hour && hour <= end_hour)
    {
      int total_minutes = (hour - start_hour) * 60 + minute;
      int y_offset = (total_minutes * total_height) / ((total_labels - 1) * 60); // 9 hours = 540 min
      int y_position = start_y + y_offset + vertical_offset;                     // To do an extra pixels to make the line of time in the center of the text

      // Draw the horizontal line across the screen width (customize as needed)
      EPD_DrawLine(time_label_start + booking_start_offset, y_position, time_label_start + 200, y_position, BLACK);
    }
  }
}

void UI_draw_info_bar()
{
  UI_draw_signal(376, 0);
  UI_draw_roomID();
  UI_draw_time();
}