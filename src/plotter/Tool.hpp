#include <Arduino.h>
#include <Servo.h>

#include "Config.h"
#include "Settings.h"

struct Tool {
  void enable() { _servo.attach(SERVO_PIN); }

  void disable() { _servo.detach(); }

  void up() { set(settings.servoAngleUp); }

  void down() { set(settings.servoAngleDown); }

  void dot() {
    down();
    up();
  }

private:
  void set(int degrees) {
    _servo.write(degrees);
    delay(settings.servoWaitMs);
  }

  Servo _servo{};
};

extern Tool tool;
