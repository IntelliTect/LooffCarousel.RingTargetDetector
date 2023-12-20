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
uint16_t _BellSwingLoopCount = 0;
uint8_t _trigger = 0;

#define SWING_ANGLE 110
#define RESTING_ANGLE 40
const uint16_t SWING_INTERVAL = 15; // dont know why but 15 is like 300ms

void loop() {

  _trigger = digitalRead(TRIGGER_PIN);

  if (_trigger == 1) {
    _SwingBell = true;
  }

  if (_SwingBell) {

   Serial.println(_BellSwingLoopCount);
    if (_BellSwingLoopCount <= SWING_INTERVAL) {
      BellServo.write(SWING_ANGLE);
    } else {
      BellServo.write(RESTING_ANGLE);
    }

    if (_BellSwingLoopCount > (2 * SWING_INTERVAL)) {
      // reset
      _BellSwingLoopCount = 0;
      _SwingBell = false;
         Serial.println("reset");
    } else {
      _BellSwingLoopCount++;
      delay(1);
    }
  }
}