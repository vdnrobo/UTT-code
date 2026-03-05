#include <EEPROM.h>

#include "Settings.h"

#define EEPROM_ADDR   0
#define EEPROM_MARKER 0xAB

struct SettingsWithMarker {
  uint8_t marker;
  Settings settings;
};

constexpr Settings default_settings {
  .stepDelay = 300,
  .servoAngleUp = 80,
  .servoAngleDown = 50,
  .servoWaitMs = 700,
  .circleSegs = 80,
};

void Settings::load() {
  SettingsWithMarker s;
  EEPROM.get(EEPROM_ADDR, s);

  if (s.marker != EEPROM_MARKER) {
    s.marker = EEPROM_MARKER;
    s.settings = default_settings;
    EEPROM.put(EEPROM_ADDR, s);
  }

  *this = s.settings;
}

void Settings::save() const {
  // put применяет побайтово update, а он уже сам заботится о минимизации износа
  EEPROM.put(EEPROM_ADDR, SettingsWithMarker{
    .marker = EEPROM_MARKER,
    .settings = *this, 
  });
}

void Settings::reset() {
  *this = default_settings;
  save();
}

Settings settings;
