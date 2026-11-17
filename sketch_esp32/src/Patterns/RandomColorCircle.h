#ifndef RandomColorCircle_h
#define RandomColorCircle_h

#include "Arduino.h"
#include <FastLED.h>
#include "../CelebrationPattern.h"
#include "../LedDisplayConf.h"

class RandomColorCircle : public CelebrationPattern
{

  public:
    RandomColorCircle()
    {
      m_speed = 8;
    };

    bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false)
    {
      static uint8_t i = 0;
      static uint8_t x = 0;
      // reset your pattern to play from the begining if someoneJustScored
      if (someoneJustScored)
      {
        i = -1;
        x = 0;
      }

      i++;

      if (i == NUM_LEDS_PER_STRIP)
      {
        i = 0;
        x++;
      }

      // have a way to know when your pattern is complete.
      // and if pattern is completed do nothing
      // return true
      if (x == NUM_STRIPS)
      {
        return true;
      }
      
      leds[x][i] = CRGB(random8(), random8(), random8());

      return false;
    }

};

#endif
