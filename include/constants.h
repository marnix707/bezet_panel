#pragma once
#include <Arduino.h>

constexpr size_t SCREEN_BUFFER_SIZE = 5000;  // Adjust to your actual buffer size
constexpr size_t LABEL_BUFFER_SIZE = 256;

extern uint8_t ImageBW[SCREEN_BUFFER_SIZE];
extern uint8_t lastImageBW[SCREEN_BUFFER_SIZE];
extern char label[LABEL_BUFFER_SIZE];
