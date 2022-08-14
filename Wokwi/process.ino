#include <FastLED.h>

#define NUM_STRIPS 6
#define NUM_LEDS_PER_STRIP 60
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];
#define PUSH_BTN_PIN 2
//const int8_t DataPins[] = {8, 9, 10, 11,12,13};
uint8_t _ButtonState = 0;

void setup()
{

  // add led strips
  FastLED.addLeds<NEOPIXEL, 8>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 9>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 10>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 11>(leds[3], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 12>(leds[4], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, 13>(leds[5], NUM_LEDS_PER_STRIP);

  //add push button (simulate ring toss detected)
  pinMode(PUSH_BTN_PIN, INPUT_PULLUP);

  delay(50);
}

uint8_t _i = 0;
uint8_t _x = 0;

bool doPattern(bool someoneJustScored = false)
{
  // reset your pattern to play from the begining if someoneJustScored
  if (someoneJustScored)
  {
    //reset pattern
    _i = -1;
    _x = 0;
  }

  // restore state
  uint8_t i = _i;
  uint8_t x = _x;

  i++;

  if (i == NUM_LEDS_PER_STRIP)
  {
    i = 0;
    x++;
  }

  // have a way to know when your pattern is complete.
  // and if pattern is completed do nothing
  // return true
  if (_x == NUM_STRIPS)
  {
    return true;
  }
  // *turn off previous led;
  //leds[_x][_i] = CRGB::Black;
  // *turn on current led
  leds[x][i] = CRGB::Red;

  _i = i;
  _x = x;

  return false;
}

bool _SomeoneScored = false;
uint8_t _Speed = 3;
bool _CurrentPatternCompleted = false;

void loop()
{
  _ButtonState = digitalRead(PUSH_BTN_PIN);
  _SomeoneScored = false;

  if (_ButtonState == LOW)
  {
    FastLED.clear(); // clear all pixel data
    FastLED.show();

    _SomeoneScored = true;
    _CurrentPatternCompleted = false;
  }

  EVERY_N_MILLISECONDS_I(thistimer, _Speed)
  {
    thistimer.setPeriod(_Speed);

    if (_CurrentPatternCompleted == false)
    {
      _CurrentPatternCompleted = doPattern(_SomeoneScored);
    }
    FastLED.show();

    if (_CurrentPatternCompleted)
    {
      FastLED.clear(); // clear all pixel data
      FastLED.show();
    }
  }
}
