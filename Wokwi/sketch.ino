#include <FastLED.h>
#include "CelebrationPattern.h"
#include "ExamplePattern.h"
#include "LedDisplayConf.h"

#define NUM_CELEBRATIONS 1
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];
#define PUSH_BTN_PIN 2
#define NUM_MS_PER_FRAME 13 //adjust for displays actual framerate aprox 80fps (12.5 ms = 1 ms) 
//const int8_t DataPins[] = {8, 9, 10, 11,12,13};
uint8_t _ButtonState = 0;

CelebrationPattern **_CelebrationPatterns = new CelebrationPattern *[NUM_CELEBRATIONS];

void setup()
{
  //add patterns
  _CelebrationPatterns[0] = new ExamplePattern();
  // add led strips
  FastLED.addLeds<NEOPIXEL, 8>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 9>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 10>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 11>(leds[3], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 12>(leds[4], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 13>(leds[5], NUM_LEDS_PER_STRIP);

  Serial.begin(115200);
  Serial.println("in setup");
  //add push button (simulate ring toss detected)
  pinMode(PUSH_BTN_PIN, INPUT_PULLUP);

  delay(50);
}


bool _SomeoneScored = true; // always play a start up for testing purposes
uint8_t _Speed = 3;
bool _CurrentPatternCompleted = false;
uint8_t d = 0;
uint8_t f = 0;

CelebrationPattern *_SelectedPattern;

void loop()
{

  // set your pattern to the default patter for testing
  _SelectedPattern = _CelebrationPatterns[0];
  _Speed = _SelectedPattern->m_speed;
  //------------
  _ButtonState = digitalRead(PUSH_BTN_PIN);
  _SomeoneScored = false;

  if (_ButtonState == LOW)
  {
    FastLED.clear(); // clear all pixel data
    FastLED.show();

    _SomeoneScored = true;
    _CurrentPatternCompleted = false;

    _SelectedPattern = _CelebrationPatterns[0];
  }

  EVERY_N_MILLISECONDS_I(thistimer, _Speed)
  {
    // speed can be controlled dynamically
    _Speed = _SelectedPattern->m_speed;

    thistimer.setPeriod(_Speed);

    if (_CurrentPatternCompleted == false)
    {
      _CurrentPatternCompleted = _SelectedPattern->draw(leds, _SomeoneScored);
    }

    if (_CurrentPatternCompleted)
    {
      FastLED.clear(); // clear all pixel data
      FastLED.show();
    }
  }

  EVERY_N_MILLISECONDS(1)
  {
    //adjust for displays actual framerate aprox 80fps (12.5 ms = 1 ms)
    d++;
    if (d % NUM_MS_PER_FRAME == 0) {
      FastLED.show();
    }

  }
}
