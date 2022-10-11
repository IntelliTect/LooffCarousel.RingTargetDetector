#include <Arduino.h>
#include <FastLED.h>  // fastLED 3.4.0
#include <Servo.h>

#include "./src/CelebrationPattern.h"
#include "./src/Patterns/Patterns.h"
#include "./src/ExamplePattern.h"
#include "./src/LedDisplayConf.h"


// patterns
CelebrationPattern **_CelebrationPatterns = new CelebrationPattern *[NUM_PATTERNS];

//leds
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];
#define LED_BRIGHTNESS 150
#define NUM_MS_PER_FRAME 13
uint8_t _msCounter = 0;

// sensors and servo
// must use gpio pin numbers for when not dealing with fastled
#define VIBRATION_SENSOR_0_PIN 16  //d0
#define VIBRATION_SENSOR_1_PIN 13  //d7
#define SERVO_ARDUINO_PIN 15  //d8 


uint_fast16_t _vibSensorCheckFrequency = 3;
uint8_t _vibReading = 0;

void setup() {

  // set up sensors
  Serial.begin(115200);
  delay(20);
  Serial.println("setup start");

  pinMode(VIBRATION_SENSOR_0_PIN, INPUT);
  pinMode(VIBRATION_SENSOR_1_PIN, INPUT);

  //add patterns
  _CelebrationPatterns[0] = new RainbowComet();
  _CelebrationPatterns[1] = new StarBurst();
  _CelebrationPatterns[2] = new RandomColorCircle();
  _CelebrationPatterns[3] = new Seahawks();

  // add led strips
  FastLED.addLeds<WS2812, 1, STRIP_RGB_ORDER>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 2, STRIP_RGB_ORDER>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 3, STRIP_RGB_ORDER>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 4, STRIP_RGB_ORDER>(leds[3], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 5, STRIP_RGB_ORDER>(leds[4], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 6, STRIP_RGB_ORDER>(leds[5], NUM_LEDS_PER_STRIP);

  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.setCorrection(TypicalPixelString);


  // indicate startup!
  FastLED.showColor(CRGB::Red);
  delay(1000);
  FastLED.showColor(CRGB::Green);
  delay(1000);
  FastLED.showColor(CRGB::Blue);
  delay(1000);
  FastLED.clear();  // clear all pixel data
  FastLED.show();
  delay(10);

  pinMode(SERVO_ARDUINO_PIN, OUTPUT);
  delay(10);


}

// loop() props
bool _SomeoneScored = false;
uint8_t _Speed = 3;                            // frequency in ms to draw a pattern
bool _CurrentPatternAnimationFinished = true;  // dont play at start up!
CelebrationPattern *_SelectedPattern;
bool _SwingBell = false;
uint8_t _BellSwingLoopCount = 0;

void loop() {

  _SomeoneScored = false;

  EVERY_N_MILLISECONDS_I(thisvibrationCheckTimer, _vibSensorCheckFrequency) {
    _vibSensorCheckFrequency = 1;

    _vibReading = digitalRead(VIBRATION_SENSOR_0_PIN);
    yield();
    if (_vibReading == 1) {
      _vibReading = digitalRead(VIBRATION_SENSOR_1_PIN);
      if (_vibReading == 1) {
        //dont check the sensors for a bit
        // (presume that no one will score for a bit/ dont allow another score to interfere)
        _vibSensorCheckFrequency = 800;

        // prepare to draw a pattern
        FastLED.clear();  // clear all pixel data
        FastLED.show();
        _SomeoneScored = true;
        _CurrentPatternAnimationFinished = false;
        uint8_t index = random8(NUM_PATTERNS);
        _SelectedPattern = _CelebrationPatterns[index];
        // prepare bell
        _SwingBell = true;
      }
    }
    thisvibrationCheckTimer.setPeriod(_vibSensorCheckFrequency);
  }

  if (!_CurrentPatternAnimationFinished) {
    // draw the pattern
    EVERY_N_MILLISECONDS_I(thisAnimationTimer, _Speed) {
      // speed can be controlled dynamically
      _Speed = _SelectedPattern->m_speed;
      thisAnimationTimer.setPeriod(_Speed);

      _CurrentPatternAnimationFinished = _SelectedPattern->draw(leds, _SomeoneScored);

      // once the pattern completes clear the LEDs
      if (_CurrentPatternAnimationFinished) {
        delay(10);        // wait a moment so that the last frame of the pattern is shown (not really important though)
        FastLED.clear();  // clear all pixel data
        FastLED.show();
      }
    }
     EVERY_N_MILLISECONDS(1)
  {
    //adjust for displays actual framerate.
    _msCounter++;
    if (_msCounter % NUM_MS_PER_FRAME == 0) {
      // draw the pattern
      FastLED.show();
    }
  }

  } else {
    // just draw black occasionally
    EVERY_N_MILLISECONDS(300) {
      // draw
      FastLED.show();
    }
  }

  if (_SwingBell) {
      digitalWrite(SERVO_ARDUINO_PIN,HIGH);
      _BellSwingLoopCount++;
   
    if (_BellSwingLoopCount > 20) {
      // reset
      _BellSwingLoopCount = 0;
      _SwingBell = false;
       digitalWrite(SERVO_ARDUINO_PIN,LOW);
       yield();
    }
  }
}