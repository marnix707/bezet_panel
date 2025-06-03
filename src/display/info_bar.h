#include "EPD.h"      // Include the EPD library for controlling the electronic ink screen (E-Paper Display)
#include "EPD_GUI.h"  // Include the EPD_GUI library for graphical user interface (GUI) operations
#include "graphics/info_bar_bitmap.h"
#include <WiFi.h>


typedef struct {
  float threshold;
  const uint8_t* image;
} ThresholdIcon;

void UI_draw_signal(int x, int y) {
  const uint8_t* default_image = epd_bitmap_signal_strenght_no;

  ThresholdIcon signal_levels[] = {
    { -90, epd_bitmap_signal_strenght_0 },    // Very weak
    { -80, epd_bitmap_signal_strenght_10 },   // Weak
    { -67, epd_bitmap_signal_strenght_50 },   // Good
    { -50, epd_bitmap_signal_strenght_100 },  // Excellent
    {   0, epd_bitmap_signal_strenght_no }    // No signal (or invalid RSSI)
  };

  int32_t rssi = WiFi.RSSI();
  //Serial.printf("RSSI: %d dBm\n", rssi);

  const uint8_t* selected_image = default_image;

  // Special case: If not connected or RSSI == 0, use "no signal" icon
  if (WiFi.status() != WL_CONNECTED || rssi == 0) {
    selected_image = epd_bitmap_signal_strenght_no;
  } else {
    // Iterate thresholds from strongest to weakest
    for (int i = sizeof(signal_levels) / sizeof(signal_levels[0]) - 1; i >= 0; --i) {
      if (rssi >= signal_levels[i].threshold) {
        selected_image = signal_levels[i].image;
        break;
      }
    }
  }

  EPD_ShowPicture(x, y, 24, 24, selected_image, BLACK);
}

