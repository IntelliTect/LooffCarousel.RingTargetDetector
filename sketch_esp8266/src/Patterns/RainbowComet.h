#ifndef RainbowComet_h
#define RainbowComet_h

#include "Arduino.h"
#include <FastLED.h>
// for arduino
#include "../CelebrationPattern.h"
#include "../LedDisplayConf.h"
#include "../UtilityFunctions.h"

// for Wokwi (doesn't have folder structure)
/* #include "CelebrationPattern.h"
#include "LedDisplayConf.h"
#include "UtilityFunctions.h" */


class RainbowComet : public CelebrationPattern
{

  public:
    RainbowComet()
    {
      m_speed = 20;
    };

    bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false)
    {
      const byte fadeAmt = 128;
      const int cometSize = 10;
      const int deltaHue  = 4;

      static byte hue = HUE_RED;
      static int iDirection = 1;
      static int iPos = 0;

      if (someoneJustScored) {
        iPos = 0;
        iDirection = 1;
        hue = HUE_RED;
      }

      hue += deltaHue;

      iPos += iDirection;
      if (iPos == (NUM_LEDS_PER_STRIP - cometSize) || iPos == 0)
        iDirection *= -1;

      for (int i = 0; i < cometSize; i++)
        leds[0][iPos + i].setHue(hue);

      // Randomly fade the LEDs
      for (int j = 0; j < NUM_LEDS_PER_STRIP; j++)
        if (random(10) > 5)
          leds[0][j] = leds[0][j].fadeToBlackBy(fadeAmt);

      DuplicateToOtherStrips(leds);

      if (iPos == (NUM_LEDS_PER_STRIP - cometSize)) {
        return true;
      }

      return false;
    }

};

#endif