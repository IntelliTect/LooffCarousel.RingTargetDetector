#ifndef IntelliTectPattern_h
#define IntelliTectPattern_h

#include "Arduino.h"
#include <FastLED.h>
#include "../CelebrationPattern.h"
#include "../LedDisplayConf.h"
#include "../UtilityFunctions.h"

class IntelliTectPattern : public CelebrationPattern
{
  private:
    static const CRGB _IntelliTectBlue;

  public:
    IntelliTectPattern()
    {
      m_speed = 20;
    };

    bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false)
    {
      static uint8_t sequence = 0;
      static uint8_t radius = 0;
      static uint8_t pulseFrame = 0;

      if (someoneJustScored)
      {
        sequence = 0;
        radius = 0;
        pulseFrame = 0;
      }

      switch (sequence)
      {
        case 0:
        {
          FastLED.clear();
          const uint8_t center = NUM_LEDS_PER_STRIP / 2;
          const uint8_t ringWidth = 4;

          for (uint8_t i = 0; i < NUM_LEDS_PER_STRIP; i++)
          {
            uint8_t dist = (i > center) ? (i - center) : (center - i);
            if (dist <= radius)
            {
              bool isBlue = (dist / ringWidth) % 2 == 0;
              leds[0][i] = isBlue ? _IntelliTectBlue : CRGB::White;
            }
          }

          DuplicateToOtherStrips(leds);
          radius++;

          if (radius > center)
          {
            sequence = 1;
            pulseFrame = 0;
          }
          break;
        }

        case 1:
        {
          for (uint8_t s = 0; s < NUM_STRIPS; s++)
            for (uint8_t i = 0; i < NUM_LEDS_PER_STRIP; i++)
              leds[s][i] = _IntelliTectBlue;

          pulseFrame++;
          if (pulseFrame > 12)
            return true;
          break;
        }
      }

      return false;
    }
};

const CRGB IntelliTectPattern::_IntelliTectBlue = CRGB(31, 112, 185);

#endif
