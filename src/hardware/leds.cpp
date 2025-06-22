#include <Arduino.h>

#include "config_settings.h"

// RGB LED pin definitions
#define RED_PIN 14
#define GREEN_PIN 13
#define BLUE_PIN 21

// PWM setup
const int freq = 5000;
const int resolution = 8;

const int ledChannelRed = 0;
const int ledChannelGreen = 1;
const int ledChannelBlue = 2;

// === LED Setup ===
void init_leds() {
  // Setup PWM channels
  ledcSetup(ledChannelRed, freq, resolution);
  ledcSetup(ledChannelGreen, freq, resolution);
  ledcSetup(ledChannelBlue, freq, resolution);

  // Attach channels to pins
  ledcAttachPin(RED_PIN, ledChannelRed);
  ledcAttachPin(GREEN_PIN, ledChannelGreen);
  ledcAttachPin(BLUE_PIN, ledChannelBlue);

  // Default to off
  ledcWrite(ledChannelRed, 0);
  ledcWrite(ledChannelGreen, 0);
  ledcWrite(ledChannelBlue, 0);

  if (debug_mode)
  {
    Serial.println("LEDs initialized.");
  }
  
}

// === Direct color control with PWM (0–255) ===
void setColorPWM(uint8_t r, uint8_t g, uint8_t b) {
  ledcWrite(ledChannelRed, r);
  ledcWrite(ledChannelGreen, g);
  ledcWrite(ledChannelBlue, b);
}

// === Predefined Colors ===

void turnRed() {
  setColorPWM(255, 0, 0);
}

void turnGreen() {
  setColorPWM(0, 255, 0);
}

void turnBlue() {
  setColorPWM(0, 0, 255);
}

void turnOrange() {
  setColorPWM(255, 50, 0);  // Red + moderate Green
}

void turnOff() {
  setColorPWM(0, 0, 0);
}
