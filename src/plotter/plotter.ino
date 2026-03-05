#include <GyverEncoder.h>

#include "Config.h"
#include "MenuSystem.h"
#include "Motion.h"
#include "Oled.hpp"
#include "Primitives.h"
#include "Settings.h"
#include "Tool.hpp"

// GLOBAL OBJECTS

Oled oled{};
Encoder enc{ENC_A, ENC_B, ENC_SW};
Tool tool{};
UI ui{};

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

  ui.initMenu("ГЛАВНАЯ");
  ui.draw();
}

void loop() {
  enc.tick();
  if (enc.isRight()) ui.onValue(1);
  if (enc.isLeft()) ui.onValue(-1);
  if (enc.isClick()) ui.onClick();
}
