#pragma once

#include <GyverOLED.h>

// Псевдоним для единой точки специализации класса, чтобы не дублировать определение и связывание
using Oled = GyverOLED<SSH1106_128x64, OLED_NO_BUFFER>; // Пока без буфера.. критичен ли 1 Киб?

extern Oled oled;