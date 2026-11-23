#ifndef BuildUpPattern_h
#define BuildUpPattern_h

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

#define BUILD_UP_NUM_STAGES 4
#define BUILD_UP_RING_GAP 3  // dark leds on each side of a ring so the rings read as separate
// rings go from the center (led 0) outward, and each ring is a step hotter in color
static const uint8_t BUILD_UP_RING_HUES[BUILD_UP_NUM_STAGES] = {HUE_BLUE, HUE_GREEN, HUE_YELLOW, HUE_RED};

// shown while a combo builds up, one ring per consecutive toss while spinning (stage 1 = 1 ring ... stage 4 = 4 rings).
// kept out of the celebration pool like IdlePattern
class BuildUpPattern : public CelebrationPattern
{
private:
  uint8_t _Stage = 1;
  uint8_t _NewRingLedsLit = 0;

public:
  BuildUpPattern()
  {
    m_speed = 15;
  };

  void setStage(uint8_t stage)
  {
    _Stage = constrain(stage, 1, BUILD_UP_NUM_STAGES);
  }

  bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false)
  {
    const uint8_t ringSpacing = NUM_LEDS_PER_STRIP / BUILD_UP_NUM_STAGES;
    const uint8_t ringWidth = ringSpacing - 2 * BUILD_UP_RING_GAP;

    if (someoneJustScored)
    {
      _NewRingLedsLit = 0;
      m_speed = 15;
    }

    uint8_t brightness = 255;
    if (_NewRingLedsLit < ringWidth)
    {
      // the rings from earlier tosses stay lit while the newest ring sweeps in
      _NewRingLedsLit++;
    }
    else
    {
      // then hold and pulse, faster each stage
      m_speed = 20;
      brightness = beatsin8(30 * _Stage, 120, 255);
    }

    fill_solid(leds[0], NUM_LEDS_PER_STRIP, CRGB::Black);
    for (uint8_t ring = 0; ring < _Stage; ring++)
    {
      uint8_t ringStart = ring * ringSpacing + BUILD_UP_RING_GAP;
      uint8_t ledsLit = (ring == _Stage - 1) ? _NewRingLedsLit : ringWidth;
      fill_solid(&leds[0][ringStart], ledsLit, CHSV(BUILD_UP_RING_HUES[ring], 255, brightness));
    }
    DuplicateToOtherStrips(leds);

    // never finishes on its own: the next toss or the end of the combo window replaces it
    return false;
  }
};

#endif
