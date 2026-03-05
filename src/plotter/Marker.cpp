#include <Arduino.h>

#include "Marker.h"
#include "Config.h"
#include "Settings.h"

// SERVO

Servo servo;

// FUNCTIONS

void initMarker() {
  servo.attach(SERVO_PIN);
}

void markerUp() {
  servo.write(settings.servoAngleUp);
  delay(settings.servoWaitMs);
}

void markerDown() {
  servo.write(settings.servoAngleDown);
  delay(settings.servoWaitMs);
}

void makeDot() {
  markerDown();
  markerUp();
}
