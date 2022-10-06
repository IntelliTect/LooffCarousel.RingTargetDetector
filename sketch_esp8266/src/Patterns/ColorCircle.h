#ifndef RandomColorCircle_h
#define RandomColorCircle_h

#include "Arduino.h"
#include <FastLED.h>
#include "../CelebrationPattern.h"
#include "../LedDisplayConf.h"

class RandomColorCircle : public CelebrationPattern
{
  private:
    uint8_t _i;
    uint8_t _x;

  public:
    RandomColorCircle()
    {
      _i = 0;
      _x = 0;
      m_speed = 0.25;
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
      
      leds[x][i] = CRGB(random8(), random8(), random8());

      _i = i;
      _x = x;

      return false;
    }

};

#endif
