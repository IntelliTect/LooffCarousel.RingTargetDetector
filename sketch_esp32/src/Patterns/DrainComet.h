#ifndef DrainComet_h
#define DrainComet_h

#include "Arduino.h"
#include <FastLED.h>
// for arduino
#include "../CelebrationPattern.h"
#include "../LedDisplayConf.h"

// for Wokwi (doesn't have folder structure)
/* #include "CelebrationPattern.h"
#include "LedDisplayConf.h" */


class DrainComet : public CelebrationPattern
{

  public:
    DrainComet()
    {
      m_speed = 200;
    };

    bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false)
    {
      const int cometSize = 4;
      const int deltaHue  = 4;
      const CRGB color = CRGB::CRGB::Purple;

      static byte hue = HUE_RED;
      static int iDirection = 1;
      static int iPos = 0;
      static int primaryStrip = 0;

      if (someoneJustScored) {
        iPos = 0;
        iDirection = 1;
        hue = HUE_RED;
      }

      hue += deltaHue;
      FastLED.clear();

      iPos += iDirection *5;
      // the comet moves 5 leds a frame, so stop it on the last position instead of running past the end of the strip
      if (iPos > NUM_LEDS_PER_STRIP - cometSize) {
        iPos = NUM_LEDS_PER_STRIP - cometSize;
      }
      if (iPos == (NUM_LEDS_PER_STRIP - cometSize) || iPos == 0) {
        iDirection *= -1;
      }

      uint8_t fadeAmount = 0;

      for (int i = 0; i < NUM_STRIPS; i++) {
        uint8_t strip = (primaryStrip + i) % NUM_STRIPS;
        //erase last frame
        //leds[strip].fill_solid(NUM_LEDS_PER_STRIP,CRGB::Black);
        for (int j = 0; j < cometSize ; j++) {
          leds[strip][iPos + j] = color;
          leds[strip][iPos + j].fadeToBlackBy(fadeAmount);
        }
        fadeAmount += 40;
        if (i == NUM_STRIPS - 1) {
          fadeAmount = 255;
        }
      }

      primaryStrip++;
      primaryStrip = primaryStrip % NUM_STRIPS;

      if (iPos == (NUM_LEDS_PER_STRIP - cometSize)) {
        return true;
      }

      return false;
    }

};

#endif