#include <GyverEncoder.h>
#include <GyverOLED.h>

#include "Config.h"
#include "MenuSystem.h"
#include "Motion.h"
#include "Primitives.h"
#include "Settings.h"
#include "Tool.hpp"

// GLOBAL OBJECTS

GyverOLED<SSH1106_128x64, OLED_NO_BUFFER> oled{};
Encoder enc{ENC_A, ENC_B, ENC_SW};
Tool tool{};

// DRAWING MODES
// А что здесь будет? Перечисление? Процедуры?

void setup() {
  initMotion();

  pinMode(BTN_X, INPUT);
  pinMode(BTN_Y, INPUT);

  pinMode(13, OUTPUT);  // LED_BUILTIN??

  settings.load();

  tool.enable();
  tool.up();

  enc.setType(TYPE2);
  enc.setFastTimeout(30);
  enc.setBtnPinMode(HIGH_PULL);
  enc.setPinMode(HIGH_PULL);

  oled.init();

  // MENU CONFIGURATION

  initMenu("ГЛАВНАЯ");

  drawMenu();
}

void loop() {
  enc.tick();
  if (enc.isRight()) moveCursor(1);
  if (enc.isLeft()) moveCursor(-1);
  if (enc.isClick()) selectItem();
}
