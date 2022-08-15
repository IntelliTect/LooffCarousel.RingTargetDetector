#ifndef ExamplePattern_h
#define ExamplePattern_h

#include "Arduino.h"
#include <FastLED.h>
#include "CelebrationPattern.h"
#include "LedDisplayConf.h"

class ExamplePattern : public CelebrationPattern
{
  public:
    ExamplePattern();
    bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false);

  private:
    uint8_t _i;
    uint8_t _x;

};

#endif