#include <Arduino.h>
#include <FastLED.h> // fastLED 3.4.0
#include <Servo.h>

#include "./src/CelebrationPattern.h"
#include "./src/Patterns/Patterns.h"
#include "./src/ExamplePattern.h"
#include "./src/LedDisplayConf.h"

// patterns
CelebrationPattern **_CelebrationPatterns = new CelebrationPattern *[NUM_PATTERNS];

//leds
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];
#define LED_BRIGHTNESS 150
#define NUM_MS_PER_FRAME 13

// sensors and servo
// must use gpio pin numbers for when not dealing with fastled
#define VIBRATION_SENSOR_0_PIN 16 //d0
#define VIBRATION_SENSOR_1_PIN 13 //d7
#define SERVO_ARDUINO_PIN 15      //d8

uint_fast16_t _vibSensorCheckFrequency = 3;
uint8_t _vibReading = 0;
#define PERIOD_WAIT_BETWEEN_SCORES 800
#define MAX_SCORE_IN_A_ROW 5

void setup()
{

  // set up sensors
  Serial.begin(115200);
  delay(20);
  Serial.println("setup start");

  pinMode(VIBRATION_SENSOR_0_PIN, INPUT);
  pinMode(VIBRATION_SENSOR_1_PIN, INPUT);

  //add patterns
  _CelebrationPatterns[0] = new RainbowComet();
  _CelebrationPatterns[1] = new StarBurst();
  _CelebrationPatterns[2] = new RandomColorCircle();
  _CelebrationPatterns[3] = new Seahawks();

  // add led strips
  FastLED.addLeds<WS2812, 1, STRIP_RGB_ORDER>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 2, STRIP_RGB_ORDER>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 3, STRIP_RGB_ORDER>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 4, STRIP_RGB_ORDER>(leds[3], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 5, STRIP_RGB_ORDER>(leds[4], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 6, STRIP_RGB_ORDER>(leds[5], NUM_LEDS_PER_STRIP);

  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.setCorrection(TypicalPixelString);

  // indicate startup!
  FastLED.showColor(CRGB::Red);
  delay(1000);
  FastLED.showColor(CRGB::Green);
  delay(1000);
  FastLED.showColor(CRGB::Blue);
  delay(1000);
  FastLED.clear(); // clear all pixel data
  FastLED.show();
  delay(10);

  pinMode(SERVO_ARDUINO_PIN, OUTPUT);
  delay(10);
}

// loop() props
bool _NewScore = false;                       // same as vibration detected
uint8_t _Speed = 3;                           // frequency in ms to draw a pattern
bool _CurrentPatternAnimationFinished = true; // dont play at start up!
CelebrationPattern *_SelectedPattern;
uint8_t _ScoresInARow = 0;
const int _VibrationSensorRefreshCheckPeriodMilliS = PERIOD_WAIT_BETWEEN_SCORES * MAX_SCORE_IN_A_ROW + 100;
bool _SensorsAreFalsePositive = false;

void loop()
{

  if (!_SensorsAreFalsePositive)
  {
    _NewScore = checkIfSomeoneScored();
    // if its been n seconds and no vibration detected at the time n then reset the 8 bit counter
    // the sensors should be in a good state
    EVERY_N_MILLISECONDS(_VibrationSensorRefreshCheckPeriodMilliS)
    {
      if (!_NewScore)
      {
        _ScoresInARow = 0;
      }
    }
  }

  if (_NewScore)
  {
    _ScoresInARow++;
  }

  // the cheap vibration sensors get stuck in a false positive state:
  // prevent starting a new pattern or ring bell if in a false positve state
  // essentially only "celebrate" if the frequency of scoring is greater than the PERIOD_WAIT_BETWEEN_SCORES
  // if frequency is greater then that indicates the sensors are false positive
  // the sensors will go to low once anothor score occurs (knocking them causes them to get unstuck). So wait until low, and then immediately "celebrate".
  if (_ScoresInARow >= MAX_SCORE_IN_A_ROW)
  {
    _SensorsAreFalsePositive = true;
    //Serial.println("bad state!");
    _NewScore = checkVibrationSensorsForLow();
    // reset if the sensors go back to a good state
    if (_NewScore)
    {
      _SensorsAreFalsePositive = false;
      _ScoresInARow = 0;
      //Serial.println("good state");
    }
  }

  ringBell(_NewScore);

  if (!_CurrentPatternAnimationFinished || _NewScore)
  {
    _CurrentPatternAnimationFinished = drawPattern(_NewScore);
    showFrame();
    // once the pattern completes clear the LEDs
    if (_CurrentPatternAnimationFinished)
    {
      delay(10);       // wait a moment so that the last frame of the pattern is shown (not really important though)
      FastLED.clear(); // clear all pixel data
      FastLED.show();
    }
  }
  else
  {
    // just draw black occasionally
    EVERY_N_MILLISECONDS(300)
    {
      FastLED.show();
    }
  }
}

bool checkIfSomeoneScored()
{

  _NewScore = false;
  EVERY_N_MILLISECONDS_I(thisvibrationCheckTimer, _vibSensorCheckFrequency)
  {
    _vibSensorCheckFrequency = 1;

    _vibReading = digitalRead(VIBRATION_SENSOR_0_PIN);
    yield();
    if (_vibReading == 1)
    {
      _vibReading = digitalRead(VIBRATION_SENSOR_1_PIN);
      if (_vibReading == 1)
      {
        //dont check the sensors for a bit
        // (presume that no one will score for a bit/ dont allow another score to interfere)
        _vibSensorCheckFrequency = PERIOD_WAIT_BETWEEN_SCORES;

        _NewScore = true;
      }
    }
    thisvibrationCheckTimer.setPeriod(_vibSensorCheckFrequency);
  }
  return _NewScore;
}


bool drawPattern(bool newScore)
{

  if (newScore)
  {
    // prepare to draw a pattern
    FastLED.clear(); // clear all pixel data
    FastLED.show();
    _CurrentPatternAnimationFinished = false;
    uint8_t index = random8(NUM_PATTERNS);
    _SelectedPattern = _CelebrationPatterns[index];
  }

  // draw the pattern
  EVERY_N_MILLISECONDS_I(thisAnimationTimer, _Speed)
  {
    // speed can be controlled dynamically
    _Speed = _SelectedPattern->m_speed;
    thisAnimationTimer.setPeriod(_Speed);

    _CurrentPatternAnimationFinished = _SelectedPattern->draw(leds, newScore);
  }
  
  return _CurrentPatternAnimationFinished;
}

uint8_t _msCounter = 0;

void showFrame(){
    EVERY_N_MILLISECONDS(1)
  {
    //adjust for displays actual framerate.
    _msCounter++;
    if (_msCounter % NUM_MS_PER_FRAME == 0)
    {
      // draw the pattern
      FastLED.show();
    }
  }
}

uint8_t _BellSwingLoopCount = 0;
bool _SwingBell = false;

void ringBell(bool newScore)
{
  if (newScore)
  {
    _SwingBell = true;
  }
  if (_SwingBell)
  {
    digitalWrite(SERVO_ARDUINO_PIN, HIGH);
    _BellSwingLoopCount++;
    if (_BellSwingLoopCount > 20)
    {
      // reset
      _BellSwingLoopCount = 0;
      _SwingBell = false;
      digitalWrite(SERVO_ARDUINO_PIN, LOW);
      yield();
    }
  }
}

bool checkVibrationSensorsForLow()
{
  _NewScore = false;

  _vibReading = digitalRead(VIBRATION_SENSOR_0_PIN);
  yield();
  if (_vibReading == 0)
  {
    _vibReading = digitalRead(VIBRATION_SENSOR_1_PIN);
    if (_vibReading == 0)
    {
      _NewScore = true;
    }
  }

  return _NewScore;
}
