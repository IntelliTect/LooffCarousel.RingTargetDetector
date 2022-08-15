#include "Arduino.h"
#include <FastLED.h>
#include "CelebrationPattern.h"

class ExamplePattern : public CelebrationPattern
{

    uint8_t _i = 0;
    uint8_t _x = 0;

public:
    ExamplePattern::ExamplePattern()
    {
        _i = 0;
        _x = 0;
    };

    bool draw(CRGB **leds, uint8_t numStrips, uint8_t numLedsPerStrip, bool someoneJustScored = false)
    {
        // reset your pattern to play from the begining if someoneJustScored
        if (someoneJustScored)
        {
            //reset pattern
            _i = -1;
            _x = 0;
        }

        // restore state
        uint8_t i = _i;
        uint8_t x = _x;

        i++;

        if (i == numLedsPerStrip)
        {
            i = 0;
            x++;
        }

        // have a way to know when your pattern is complete.
        // and if pattern is completed do nothing
        // return true
        if (_x == numStrips)
        {
            return true;
        }
        // *turn off previous led;
        //leds[_x][_i] = CRGB::Black;
        // *turn on current led
        leds[x][i] = CRGB::Red;

        _i = i;
        _x = x;

        return false;
    }

    double speed = 2;
};