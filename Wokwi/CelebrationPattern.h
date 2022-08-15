// These statements are optional. However, I recommend that you add them because
// they'll prevent the library code from getting imported multiple times.
#ifndef CelebrationPattern_h
#define CelebrationPattern_h

// You need to import Arduino.h to write a custom library
#include "Arduino.h"
#include <FastLED.h>

class CelebrationPattern
{
public:
    // pure virtual function
    virtual bool draw(CRGB **leds, uint8_t numStrips, uint8_t numLedsPerStrip, bool someoneJustScored = false) = 0;
    double speed; // how often you want draw() called in ms
};

#endif