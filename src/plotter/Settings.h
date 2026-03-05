#pragma once

struct Settings {
  int stepDelay;
  int servoAngleUp;
  int servoAngleDown;
  int servoWaitMs;
  int circleSegs;  // Не удалено, потому что предполагаю, что на это поле что-то запланировано.

  void load();
  void save() const;
  void reset();
};

extern Settings settings;
