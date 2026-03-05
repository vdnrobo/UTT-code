#include "Primitives.h"

#include "Motion.h"
#include "Tool.hpp"

void circleRel(float r, int seg) {
  float step = 2.0 * PI / seg;
  float x0 = r, y0 = 0;
  moveMMXY(x0, y0);
  tool.down();
  for (int i = 1; i <= seg; i++) {
    float a = step * i;
    float x = r * cos(a);
    float y = r * sin(a);
    moveMMXY(x - x0, y - y0);
    x0 = x;
    y0 = y;
  }
  tool.up();
}

void circleAt(float r, int seg) {
  moveMMXY(r, 0);
  float step = 2.0 * PI / seg;
  float x0 = 0, y0 = 0;
  tool.down();
  for (int i = 1; i <= seg; i++) {
    float a = step * i;
    float x = r * cos(a) - r;
    float y = r * sin(a);
    moveMMXY(x - x0, y - y0);
    x0 = x;
    y0 = y;
  }
  tool.up();
  moveMMXY(-r, 0);
}

void squareRel(float size) {
  tool.down();
  moveMMXY(size, 0);
  moveMMXY(0, size);
  moveMMXY(-size, 0);
  moveMMXY(0, -size);
  tool.up();
}

void rectRel(float w, float h) {
  tool.down();
  moveMMXY(w, 0);
  moveMMXY(0, h);
  moveMMXY(-w, 0);
  moveMMXY(0, -h);
  tool.up();
}

void lineRel(float dx, float dy) {
  tool.down();
  moveMMXY(dx, dy);
  tool.up();
}

void diagonals(float size) {
  tool.down();
  moveMMXY(size, size);
  tool.up();

  moveMMXY(-size, 0);

  tool.down();
  moveMMXY(size, -size);
  tool.up();
}
