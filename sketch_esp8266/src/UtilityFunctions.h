#ifndef UtilityFunctions_h
#define UtilityFunctions_h

#include "Arduino.h"
#include <FastLED.h>
#include <FastLED.h>
#include "LedDisplayConf.h"

// duplicates strip 0 to all other strips
static void DuplicateToOtherStrips(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP])
{
  for (uint8_t x = 1; x < NUM_STRIPS; x++) {
    for (uint8_t y = 0; y < NUM_LEDS_PER_STRIP; y++) {
      leds[x][y] = leds[x - 1][y];
    }
  }
}

#endif