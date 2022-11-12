#ifndef RainbowStarBurst_h
#define RainbowStarBurst_h

#include "Arduino.h"
#include <FastLED.h>
// for arduino
#include "../CelebrationPattern.h"
#include "../LedDisplayConf.h"
#include "../UtilityFunctions.h"

// for Wokwi (doesn't have folder structure)
// #include "CelebrationPattern.h"
// #include "LedDisplayConf.h"
// #include "UtilityFunctions.h"

class ReverseRainbowStarBurst : public CelebrationPattern
{

  public:
    ReverseRainbowStarBurst()
    {
      m_speed = .3;
    };

    bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false)
    {
      static uint8_t i = 0;
      static uint8_t iterations = 0;
      static CRGB colors[] = {CRGB::Green, CRGB::Crimson, 
      CRGB::Yellow, CRGB::DarkOrange, CRGB::Purple};
      static uint8_t c = 0;
      static uint8_t count = 0;

      // reset pattern to play from the beginning if someoneJustScored
      // only reset iterations to keep animation playing longer for
      // the new score but not have a hard color reset
      if (someoneJustScored)
      {
        iterations = 0;
      }

      i++;

      if (i == NUM_LEDS_PER_STRIP)
      {
        i = 0;
      }

      // Have a way to know when pattern is complete.
      // and if pattern is completed do nothing
      // return true.
      // 108 is so that if it is a full run, the orange doesn't just begin
      // to start again before ending
      if (iterations == 108)
      {
        return true;
      }
      if (count < 10)
      {
        count++;
      }
      else
      {
        if (c < (sizeof(colors) / sizeof(colors[0]) - 1))
        {
          c++;
        }
        else
        {
          c = 0;
          iterations++;
        }
        count = 0;
      }
      leds[0][i] = colors[c];
      DuplicateToOtherStrips(leds);

      return false;
    }

};

#endif