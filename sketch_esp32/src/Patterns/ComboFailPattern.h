#ifndef ComboFailPattern_h
#define ComboFailPattern_h

#include "Arduino.h"
#include <FastLED.h>
// for arduino
#include "../CelebrationPattern.h"
#include "../LedDisplayConf.h"
#include "../UtilityFunctions.h"
#include "./BuildUpPattern.h"

// for Wokwi (doesn't have folder structure)
/* #include "CelebrationPattern.h"
#include "LedDisplayConf.h"
#include "UtilityFunctions.h"
#include "BuildUpPattern.h" */

#define COMBO_FAIL_HOLD_FRAMES 12  // the rings stutter this long before they drain
#define COMBO_FAIL_RUNNER_LEDS 4

// shown when a combo's window runs out before the jackpot. The rings that were lit don't just vanish: each one shrinks
// into a short runner that drains back to the center (led 0) and fades out. It stays dim and moves one way only, so a
// missed combo never looks like the jackpot. Kept out of the celebration pool like IdlePattern and BuildUpPattern
class ComboFailPattern : public CelebrationPattern
{
private:
  uint8_t _Stage = 1;
  uint8_t _Frame = 0;

public:
  ComboFailPattern()
  {
    m_speed = 40;
  };

  // the number of rings that were lit when the combo failed
  void setStage(uint8_t stage)
  {
    _Stage = constrain(stage, 1, BUILD_UP_NUM_STAGES);
  }

  bool draw(CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP], bool someoneJustScored = false)
  {
    const uint8_t ringSpacing = NUM_LEDS_PER_STRIP / BUILD_UP_NUM_STAGES;
    const uint8_t ringWidth = ringSpacing - 2 * BUILD_UP_RING_GAP;
    // the outermost ring's runner has the furthest to go
    const uint8_t runFrames = (_Stage - 1) * ringSpacing + BUILD_UP_RING_GAP + COMBO_FAIL_RUNNER_LEDS;

    if (someoneJustScored)
    {
      _Frame = 0;
    }

    fill_solid(leds[0], NUM_LEDS_PER_STRIP, CRGB::Black);

    if (_Frame < COMBO_FAIL_HOLD_FRAMES)
    {
      // the rings stutter for a moment
      uint8_t brightness = (_Frame / 3) % 2 == 0 ? 200 : 50;
      for (uint8_t ring = 0; ring < _Stage; ring++)
      {
        fill_solid(&leds[0][ring * ringSpacing + BUILD_UP_RING_GAP], ringWidth, CHSV(BUILD_UP_RING_HUES[ring], 255, brightness));
      }
    }
    else
    {
      uint8_t runFrame = _Frame - COMBO_FAIL_HOLD_FRAMES;
      if (runFrame >= runFrames)
      {
        return true;
      }

      // the runners get dimmer the further they drain
      uint8_t brightness = 230 - (uint16_t)180 * runFrame / runFrames;
      for (uint8_t ring = 0; ring < _Stage; ring++)
      {
        int16_t head = ring * ringSpacing + BUILD_UP_RING_GAP - runFrame;
        for (uint8_t i = 0; i < COMBO_FAIL_RUNNER_LEDS; i++)
        {
          int16_t led = head + i;
          if (led >= 0 && led < NUM_LEDS_PER_STRIP)
          {
            // the head leads toward the center and the tail fades behind it
            leds[0][led] = CHSV(BUILD_UP_RING_HUES[ring], 255, brightness >> i);
          }
        }
      }
    }

    DuplicateToOtherStrips(leds);
    _Frame++;
    return false;
  }
};

#endif
