#include <Arduino.h>

#define RED_PIN 14
#define GREEN_PIN 13
#define BLUE_PIN 21

const int freq = 5000;
const int resolution = 8;

const int ledChannelRed = 0;
const int ledChannelGreen = 1;
const int ledChannelBlue = 2;

void init_leds() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
}

void setColor(bool r, bool g, bool b) {
  digitalWrite(RED_PIN, r ? HIGH : LOW);
  digitalWrite(GREEN_PIN, g ? HIGH : LOW);
  digitalWrite(BLUE_PIN, b ? HIGH : LOW);
}

void turnRed()
{
  ledcWrite(ledChannelRed, 255);
  ledcWrite(ledChannelGreen, 0);
  ledcWrite(ledChannelBlue, 0);
}

void turnBlue()
{
  ledcWrite(ledChannelRed, 0);
  ledcWrite(ledChannelGreen, 0);
  ledcWrite(ledChannelBlue, 255);
}