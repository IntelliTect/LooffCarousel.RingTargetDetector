#ifndef StarBurst_h
#define StarBurst_h

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


class StarBurst : public CelebrationPattern
{

  public:
    StarBurst()
    {
      m_speed = 10;
    };

    bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false)
    {

      static int8_t iPos = NUM_LEDS_PER_STRIP;
      static uint8_t sequence = 0;
      static bool startNextSequence = true;
      const uint8_t numLedsPerDraw = 8;
      static uint8_t numLedsThisDraw = numLedsPerDraw;
      static CRGB colours1[] = {CRGB::DodgerBlue, CRGB::Red};
      static CRGB colours2[] = {CRGB::Fuchsia, CRGB::Orange};
      const uint8_t numColors = 2;
      static uint8_t colorIndex = 0;



      // reset your pattern to play from the begining if someoneJustScored
      if (someoneJustScored)
      {
        sequence = 0;
        startNextSequence = true;
        colorIndex = random8(numColors);
      }

      switch (sequence) {
        case 0:

          //check if start
          if (startNextSequence) {
            iPos = NUM_LEDS_PER_STRIP - 1;
            startNextSequence = false;
            m_speed = 4;

          }

          numLedsThisDraw = numLedsPerDraw;
          if (iPos - numLedsThisDraw < 0) {
            numLedsThisDraw = iPos % numLedsPerDraw;
          }

          if (numLedsThisDraw == 0) {
            sequence = 1;
            startNextSequence = true;
            break;
          }

          // draw
          FastLED.clear();
          for (int8_t y = iPos; y >= iPos - numLedsThisDraw; y--) {
            leds[0][y] = colours1[colorIndex];
          }

          DuplicateToOtherStrips(leds);

          iPos--;
          break;
        case 1:
          //check if start
          if (startNextSequence) {
            iPos = 0;
            startNextSequence = false;
             m_speed = 13;
          }
          // draw
          fill_gradient_RGB(leds[0], 0, colours1[colorIndex], iPos, colours2[colorIndex]);
          DuplicateToOtherStrips(leds);
          iPos++;
          // check if done
          if (iPos == NUM_LEDS_PER_STRIP) {
            sequence++;
            startNextSequence = true;
          }
          break;
        default:
          return true;
      }


      return false;
    }

};

#endif