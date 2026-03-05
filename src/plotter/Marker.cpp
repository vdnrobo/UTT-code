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
  servo.write(settings.servoUp);
  delay(settings.servoWait);
}

void markerDown() {
  servo.write(settings.servoDown);
  delay(settings.servoWait);
}

void makeDot() {
  markerDown();
  markerUp();
}
