// These statements are optional. However, I recommend that you add them because
// they'll prevent the library code from getting imported multiple times.
#ifndef CelebrationPattern_h
#define CelebrationPattern_h

// You need to import Arduino.h to write a custom library
#include "Arduino.h"
#include <FastLED.h>
#include "LedDisplayConf.h"


class CelebrationPattern
{
  public:
    // called at around every n ms where n is defined by "speed".
    virtual bool draw(CRGB Leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false) = 0;
    // how often you want draw() called in ms
    uint8_t m_speed;
};

#endif