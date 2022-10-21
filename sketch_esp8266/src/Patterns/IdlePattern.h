#ifndef IdlePattern_h
#define IdlePattern_h

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

// patterns to be played when no one has scored for a a while
class IdlePattern : public CelebrationPattern
{
  private:
    const uint8_t _GradientStep = 1;
    const uint16_t howManyFramesToPlay = 9600; //(approx 8 mins)
    uint8_t _PalleteStartingIndex = 0;
    uint8_t _NumberOfLedsPerPaletteColor = 2;

  public:
    ExamplePattern()
    {
      m_speed = 50;
    };

    bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false)
    {

      static int8_t iPos = NUM_LEDS_PER_STRIP;
      static uint8_t sequence = 0;
      static bool startNextSequence = true;
      static uint16_t framesPlayed = 0;


      // reset your pattern to play from the begining if someoneJustScored
      if (someoneJustScored)
      {
        sequence = 0;
        startNextSequence = true;
        framesPlayed = 0;
      }

      switch (sequence) {
        case 0:

          flow(leds);
          DuplicateToOtherStrips(leds);
          framesPlayed++;

          if (framesPlayed > howManyFramesToPlay) {
            return true;
          }
          break;

        default:
          return true;
      }


      return false;
    }


    void flow(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP])
    {
      fillLEDsFromPaletteColors(_PalleteStartingIndex, leds);
      _PalleteStartingIndex += _GradientStep;
    }

    void fillLEDsFromPaletteColors(uint8_t startingIndex, CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP])
    {
      uint8_t index = startingIndex;

      uint8_t interval = NUM_LEDS_PER_STRIP / _NumberOfLedsPerPaletteColor;

      for (uint8_t i = 0; i < interval; i++)
      {
        uint8_t startingLed = (i * _NumberOfLedsPerPaletteColor);
        uint8_t endLed = startingLed + _NumberOfLedsPerPaletteColor;
        //if this is the last iteration then set it cover the rest of the _Leds
        if (((i + 1) - interval) == 0)
        {
          endLed = NUM_LEDS_PER_STRIP;
        }
        for (uint8_t led = startingLed; led < endLed; ++led)
        {
          leds[0][led] = ColorFromPalette(RainbowColors_p, index, 255, LINEARBLEND);
        }
        index += _GradientStep;
      }
    }


};

#endif