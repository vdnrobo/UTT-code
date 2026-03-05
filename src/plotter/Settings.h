#pragma once

struct Settings {
  int  stepDelay;
  int  servoAngleUp;
  int  servoAngleDown;
  int  servoWaitMs;
  int  circleSegs; // todo Что делать с неиспользуемым полем?

  void load();
  void save() const;
  void reset();
};

extern Settings settings;
