#pragma once

#include <Arduino.h>

// EEPROM

#define EEPROM_ADDR   0
#define EEPROM_MARKER 0xAB

// SETTINGS

struct Settings {
  int  stepDelay;
  int  servoUp;
  int  servoDown;
  int  servoWait;
  int  circleSegs; // todo Что делать с неиспользуемым полем?
};

// GLOBALS

extern Settings settings;

// FUNCTIONS

void loadSettings();
void saveSettings();
void resetSettings();
