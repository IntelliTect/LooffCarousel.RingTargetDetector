#ifndef Seahawks_h
#define Seahawks_h

#include "Arduino.h"
#include <FastLED.h>
// for arduino
#include "../CelebrationPattern.h"
#include "../LedDisplayConf.h"

// for Wokwi (doesn't have folder structure)
/* #include "CelebrationPattern.h"
#include "LedDisplayConf.h"
#include "UtilityFunctions.h" */

class Seahawks : public CelebrationPattern
{
  private:
    uint8_t _i;
    uint8_t _x;
    bool _reversePattern;

  public:
    Seahawks()
    {
      _i = 0;
      _x = 0;
      m_speed = 20;
    };

    bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false)
    {
      // reset your pattern to play from the begining if someoneJustScored
      if (someoneJustScored)
      {
        _i = -1;
        _x = 0;
        _reversePattern = false;
      }

      // restore state (try to limit the amount of state you store... try to recover state algorithmically)
      uint8_t i = _i;
      uint8_t x = _x;
      bool reversePattern = _reversePattern;

      i++;
      
      if (i == NUM_LEDS_PER_STRIP)
      {
        if (!reversePattern)
        {
          x = 0;
          i = 0;
          reversePattern = true;
        } 
        else
        {
          return true;
        }
      }

      CRGB dodgerBlue = CRGB::DodgerBlue;
      CRGB lawnGreen = CRGB::LawnGreen;
      if (reversePattern)
      {
        leds[x][i] = dodgerBlue;
        leds[x + 1][i] = lawnGreen;
        leds[x + 2][i] = dodgerBlue;
        leds[x + 3][i] = lawnGreen;
        leds[x + 4][i] = dodgerBlue;
        leds[x + 5][i] = lawnGreen;
      }
      else
      {
        leds[x][i] = lawnGreen;
        leds[x + 1][i] = dodgerBlue;
        leds[x + 2][i] = lawnGreen;
        leds[x + 3][i] = dodgerBlue;
        leds[x + 4][i] = lawnGreen;
        leds[x + 5][i] = dodgerBlue;
      }
      if (m_speed > 8)
      {
        m_speed -= 1;
      }
      _i = i;
      _x = x;
      _reversePattern = reversePattern;
      
      return false;
    }
};

#endif