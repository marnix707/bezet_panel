#pragma once
#include <Arduino.h>

constexpr size_t SCREEN_BUFFER_SIZE = 15000;  // Adjust to your actual buffer size
constexpr size_t LABEL_BUFFER_SIZE = 256;

extern uint8_t ImageBW[];
extern uint8_t lastImageBW[];
extern char label[LABEL_BUFFER_SIZE];
