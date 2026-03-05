#include <EEPROM.h>
#include <GyverOLED.h>

#include "Settings.h"

extern GyverOLED<SSH1106_128x64, OLED_NO_BUFFER> oled;

struct SettingsWithMarker {
  byte marker;
  Settings settings;
};

// GLOBALS

Settings settings;

const Settings defaults = {
  300,
  80,
  50,
  700,
  80
};

// FUNCTIONS

void loadSettings() {
  SettingsWithMarker s;
  EEPROM.get(EEPROM_ADDR, s);
  
  if (s.marker != EEPROM_MARKER) {
    s.settings = defaults;
    EEPROM.put(EEPROM_ADDR, s);
  }

  settings = s.settings;
}

void saveSettings() {
  // put использует побайтовый update, а он уже сам заботится о минимизации износа
  EEPROM.put(EEPROM_ADDR, SettingsWithMarker{
    .marker = EEPROM_MARKER,
    .settings = settings, 
  });
}

void resetSettings() {
  settings = defaults;
  saveSettings();

  // todo Вынести от сюда это всё ниже:
  oled.clear();
  oled.home();
  oled.setScale(2);
  oled.println("Сброс!");
  oled.setScale(1);
  oled.println();
  oled.println("Настройки по");
  oled.println("умолчанию");
  delay(1500);
}
