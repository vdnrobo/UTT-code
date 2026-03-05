#pragma once

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
