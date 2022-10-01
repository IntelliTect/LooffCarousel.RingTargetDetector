#include <Arduino.h>
#include <Servo.h>

#define SERVO_PIN 5    //d1
#define TRIGGER_PIN 4  //d2

Servo BellServo;

void setup() {
  Serial.begin(115200);
  delay(20);

  pinMode(TRIGGER_PIN, INPUT);

  BellServo.attach(SERVO_PIN);
  delay(10);
  BellServo.write(0);
  delay(50);
}

bool _SwingBell = false;
uint8_t _BellSwingLoopCount = 0;
uint8_t _trigger = 0;

void loop() {

  _trigger = digitalRead(TRIGGER_PIN);

  if (_trigger == 1) {
    _SwingBell = true;
  }

  if (_SwingBell) {

    if (_BellSwingLoopCount <= 100) {
      BellServo.write(60);
      _BellSwingLoopCount++;
      delay(1);
    }
    if (_BellSwingLoopCount >= 100) {
      BellServo.write(0);
      _BellSwingLoopCount++;
      delay(1);
    }

    if (_BellSwingLoopCount > 200) {
      // reset
      _BellSwingLoopCount = 0;
      _SwingBell = false;
    }
  }
}