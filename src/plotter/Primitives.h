#pragma once

#include <Arduino.h>

// UI

void showMode(byte m, bool done);
void showMessage(const char* line1, const char* line2 = nullptr);

// primitive figure draw functions

void circleRel(float r, int seg);
void circleAt(float r, int seg);
void squareRel(float size);
void rectRel(float w, float h);
void lineRel(float dx, float dy);
void diagonals(float size);
