#include <GyverOLED.h>
#include <GyverEncoder.h>

#include "Config.h"
#include "Settings.h"
#include "MenuSystem.h"
#include "Motion.h"
#include "Primitives.h"
#include "Tool.hpp"

// GLOBAL OBJECTS

GyverOLED<SSH1106_128x64, OLED_NO_BUFFER> oled;
Encoder enc(ENC_A, ENC_B, ENC_SW);
Tool tool{};

// DRAWING MODES



// SETUP

void setup() {
  initMotion();
  
  pinMode(BTN_X, INPUT);
  pinMode(BTN_Y, INPUT);
  
  pinMode(13, OUTPUT); // LED_BUILTIN??

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

// LOOP

void loop() {
  enc.tick();
  if (enc.isRight()) moveCursor(1);
  if (enc.isLeft())  moveCursor(-1);
  if (enc.isClick()) selectItem();
}
