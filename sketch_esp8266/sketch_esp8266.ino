#include <Arduino.h>
#include <FastLED.h>  // fastLED 3.4.0
#include "./src/CelebrationPattern.h"
#include "./src/Patterns/Patterns.h"
#include "./src/ExamplePattern.h"
#include "./src/LedDisplayConf.h"


// patterns
#define NUM_CELEBRATIONS 2
CelebrationPattern **_CelebrationPatterns = new CelebrationPattern *[NUM_CELEBRATIONS];

//leds
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];
#define LED_BRIGHTNESS 150

// sensors
// must use gpio pin umbers for when not dealing with fastled
#define VIBRATION_SENSOR_0_PIN 16  //d0
#define VIBRATION_SENSOR_1_PIN 13  //d7
#define SERVO_PIN 15               //d8 should work not tested yet
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
  _CelebrationPatterns[0] = new ExamplePattern();
  _CelebrationPatterns[1] = new RainbowComet();
  // add led strips
  FastLED.addLeds<WS2812, 1, STRIP_RGB_ORDER>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 2, STRIP_RGB_ORDER>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 3, STRIP_RGB_ORDER>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 4, STRIP_RGB_ORDER>(leds[3], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 5, STRIP_RGB_ORDER>(leds[4], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 6, STRIP_RGB_ORDER>(leds[5], NUM_LEDS_PER_STRIP);

  FastLED.setBrightness(LED_BRIGHTNESS);

  delay(50);
}

// loop() props
bool _SomeoneScored = true;  // always play a start up for testing purposes
uint8_t _Speed = 3;          // frequency in ms to draw a pattern
bool _CurrentPatternCompleted = false;
CelebrationPattern *_SelectedPattern;

void loop() {

  // set your pattern to the default patter for testing
  _SelectedPattern = _CelebrationPatterns[0];
  _Speed = _SelectedPattern->m_speed;
  //------------
  _SomeoneScored = false;

  EVERY_N_MILLISECONDS_I(thisvibrationCheckTimer, _vibSensorCheckFrequency) {
    _vibSensorCheckFrequency = 1;

    _vibReading = digitalRead(VIBRATION_SENSOR_0_PIN);
    delay(1);
    if (_vibReading == 1) {
      _vibReading = digitalRead(VIBRATION_SENSOR_1_PIN);
      if (_vibReading == 1) {

        _vibSensorCheckFrequency = 800;

        // prepare to draw a pattern
        FastLED.clear();  // clear all pixel data
        FastLED.show();
        _SomeoneScored = true;
        _CurrentPatternCompleted = false;
        _SelectedPattern = _CelebrationPatterns[random(NUM_CELEBRATIONS - 1)];
      }
    }
    thisvibrationCheckTimer.setPeriod(_vibSensorCheckFrequency);
  }

  EVERY_N_MILLISECONDS_I(thisAnimationTimer, _Speed) {
    // speed can be controlled dynamically
    _Speed = _SelectedPattern->m_speed;

    thisAnimationTimer.setPeriod(_Speed);

    if (_CurrentPatternCompleted == false) {
      _CurrentPatternCompleted = _SelectedPattern->draw(leds, _SomeoneScored);
    }
    FastLED.show();

    if (_CurrentPatternCompleted) {
      FastLED.clear();  // clear all pixel data
      FastLED.show();
    }
  }
}