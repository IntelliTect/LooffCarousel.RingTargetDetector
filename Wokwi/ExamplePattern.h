#ifndef ExamplePattern_h
#define ExamplePattern_h

#include "Arduino.h"
#include <FastLED.h>
#include "CelebrationPattern.h"

class ExamplePattern : public CelebrationPattern
{

private:
    uint8_t _i;
    uint8_t _x;

public:
    ExamplePattern();
    bool draw(CRGB **leds, uint8_t numStrips, uint8_t numLedsPerStrip, bool someoneJustScored = false);
    double speed;
};

#endif