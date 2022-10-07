#ifndef Seahawks_h
#define Seahawks_h

#include "Arduino.h"
#include <FastLED.h>
#include "CelebrationPattern.h"
#include "LedDisplayConf.h"

class Seahawks : public CelebrationPattern
{
  private:
    uint8_t _i;
    uint8_t _x;

  public:
    Seahawks()
    {
      _i = 0;
      _x = 0;
      m_speed = 30;
    };

    bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false)
    {
      // reset your pattern to play from the begining if someoneJustScored
      if (someoneJustScored)
      {
        _i = -1;
        _x = 0;
      }

      // restore state (try to limit the amount of state you store... try to recover state algorithmically)
      uint8_t i = _i;
      uint8_t x = _x;

      i++;
      if (i == 60)
      {
        return true;
      }

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
      CRGB dodgerBlue = CRGB::DodgerBlue;
      CRGB lawnGreen = CRGB::LawnGreen;
      leds[x][i] = lawnGreen;
      leds[x + 1][i] = dodgerBlue;
      leds[x + 2][i] = lawnGreen;
      leds[x + 3][i] = dodgerBlue;
      leds[x + 4][i] = lawnGreen;
      leds[x + 5][i] = dodgerBlue;
      if (m_speed > 8)
      {
        m_speed -= 1;
      }
      _i = i;
      _x = x;
      
      return false;
    }

};

#endif