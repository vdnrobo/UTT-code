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

  void load();
  void save() const;
  void reset();
};

// GLOBALS

extern Settings settings;
