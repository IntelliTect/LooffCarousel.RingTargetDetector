#include <Arduino.h>
#include <FastLED.h>  // fastLED 3.10.5 (esp32 arduino core 3.x)
#include <DFRobotDFPlayerMini.h>  // DFRobotDFPlayerMini 1.0.6

#include "./src/CelebrationPattern.h"
#include "./src/Patterns/Patterns.h"
#include "./src/Patterns/IdlePattern.h"
#include "./src/Patterns/BuildUpPattern.h"
#include "./src/ExamplePattern.h"
#include "./src/LedDisplayConf.h"

// patterns
CelebrationPattern **_CelebrationPatterns = new CelebrationPattern *[NUM_PATTERNS];
IdlePattern *_IdlePattern = new IdlePattern();
BuildUpPattern *_BuildUpPattern = new BuildUpPattern();

// leds
CRGB leds[NUM_STRIPS][NUM_LEDS_PER_STRIP];
#define NUM_MS_PER_FRAME 13

// sensors and servo
#define VIBRATION_SENSOR_0_PIN 16
#define VIBRATION_SENSOR_1_PIN 13
// gpio 15 is a strapping pin on the esp32, gpio 23 is a safe servo output
#define SERVO_ARDUINO_PIN 23

uint_fast16_t _vibSensorCheckFrequency = 2;
uint8_t _vibReading = 0;
uint8_t _vibReading2 = 0;
#define PERIOD_WAIT_BETWEEN_SCORES 1500
#define MAX_SCORE_IN_A_ROW 2

#define HOW_LONG_TO_WAIT_TO_IDLE_S 10

// dfplayer mini on uart 2. serial2 defaults to gpio 4 (led strip 0) for rx on esp32 core 3.x, so the pins are always passed
#define DFPLAYER_RX_PIN 26  // wired to the dfplayer tx
#define DFPLAYER_TX_PIN 25  // wired to the dfplayer rx
#define DFPLAYER_VOLUME 25  // 0-30
DFRobotDFPlayerMini _DfPlayer;
bool _AudioReady = false;

// sd card tracks (see sdcard/README.md): 0001-0004 are combo stages 1-4, 0005 is the jackpot
#define TRACK_NOT_SPINNING 6

// spin sensor: set the type once the hardware is chosen
#define SPIN_SENSOR_LEVEL 0  // pin is low while the carousel is spinning
#define SPIN_SENSOR_PULSE 1  // pin pulses low as the carousel turns
#define SPIN_SENSOR_TYPE SPIN_SENSOR_LEVEL
#define SPIN_SENSOR_PIN 32
#define SPIN_PULSE_TIMEOUT_MS 5000  // still spinning while the last pulse is newer than this
volatile unsigned long _LastSpinPulseMs = 0;

// combo: consecutive scores while spinning build up stages 1-4, and the 5th is the jackpot
#define COMBO_WINDOW_MS 2500
#define COMBO_JACKPOT_STAGE 5
#define JACKPOT_LENGTH_MS 7000  // about the length of track 0005
uint8_t _ComboStage = 0;        // 0 = no combo

void IRAM_ATTR onSpinPulse() {
  _LastSpinPulseMs = millis();
}

void setup() {

  // set up sensors
  Serial.begin(115200);
  delay(20);
  Serial.println("setup start");

  pinMode(VIBRATION_SENSOR_0_PIN, INPUT);
  pinMode(VIBRATION_SENSOR_1_PIN, INPUT);

  pinMode(SPIN_SENSOR_PIN, INPUT_PULLUP);
#if SPIN_SENSOR_TYPE == SPIN_SENSOR_PULSE
  attachInterrupt(digitalPinToInterrupt(SPIN_SENSOR_PIN), onSpinPulse, FALLING);
#endif

  // add patterns
  _CelebrationPatterns[0] = new RainbowComet();
  _CelebrationPatterns[1] = new StarBurst();
  _CelebrationPatterns[2] = new Seahawks();
  _CelebrationPatterns[3] = new ReverseRainbowStarBurst();
  //_CelebrationPatterns[4] = new RandomColorCircle();


  // add led strips
  // the esp8266 pins 1-6 don't carry over to the esp32 (1 and 3 are serial, 6-11 are flash), so use free gpio outputs
  FastLED.addLeds<WS2812, 4, STRIP_RGB_ORDER>(leds[0], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 5, STRIP_RGB_ORDER>(leds[1], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 18, STRIP_RGB_ORDER>(leds[2], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 19, STRIP_RGB_ORDER>(leds[3], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 21, STRIP_RGB_ORDER>(leds[4], NUM_LEDS_PER_STRIP);
  FastLED.addLeds<WS2812, 22, STRIP_RGB_ORDER>(leds[5], NUM_LEDS_PER_STRIP);

  FastLED.setBrightness(LED_BRIGHTNESS);
  FastLED.setCorrection(TypicalPixelString);

  // indicate startup!
  FastLED.showColor(CRGB::Red);
  delay(1000);
  FastLED.showColor(CRGB::Green);
  delay(1000);
  FastLED.showColor(CRGB::Blue);
  delay(1000);
  FastLED.clear();  // clear all pixel data
  FastLED.show();
  delay(10);

  // ack mode lets begin() report whether the dfplayer answered; without one the leds still run, just silently
  Serial2.begin(9600, SERIAL_8N1, DFPLAYER_RX_PIN, DFPLAYER_TX_PIN);
  _AudioReady = _DfPlayer.begin(Serial2, /* isACK */ true, /* doReset */ true);
  if (_AudioReady) {
    _DfPlayer.volume(DFPLAYER_VOLUME);
    Serial.println("dfplayer ready");
  } else {
    Serial.println("dfplayer not found, continuing without audio");
  }

  pinMode(SERVO_ARDUINO_PIN, OUTPUT);
  delay(10);
}

// loop() props
bool _NewScore = false;
bool _PossibleNewScore = false;                // same as vibration detected
bool _CurrentPatternAnimationFinished = true;  // dont play at start up!
CelebrationPattern *_SelectedPattern;
uint8_t _VibrationsDetectedInARow = 0;
const int _VibrationSensorRefreshCheckPeriodMilliS = PERIOD_WAIT_BETWEEN_SCORES * MAX_SCORE_IN_A_ROW + 100;
bool _SensorsAreFalsePositive = false;
unsigned long _TimeOfLastScore;
bool _StartIdling = false;
bool _CheckForIdle = true;

void loop() {
  _PossibleNewScore = false;

  if (!_SensorsAreFalsePositive) {
    _PossibleNewScore = checkForVibration();
    // if its been n seconds and no vibration detected at the time n then reset the 8 bit counter
    // the sensors should be in a good state
    EVERY_N_MILLISECONDS(_VibrationSensorRefreshCheckPeriodMilliS) {
      if (!_PossibleNewScore) {
        _VibrationsDetectedInARow = 0;
      }
    }
  }

  if (_PossibleNewScore) {
    _VibrationsDetectedInARow++;
  }

  // confirm the score
  // the cheap vibration sensors get stuck in a false positive state:
  // prevent starting a new pattern or ring bell if in a false positive state
  // essentially only "celebrate" if the frequency of scoring is greater than the PERIOD_WAIT_BETWEEN_SCORES
  // if frequency is greater then that indicates the sensors are false positive
  // the sensors will go to low once another score occurs (knocking them causes them to get unstuck). So wait until low, and then immediately "celebrate".
  if ((_VibrationsDetectedInARow >= MAX_SCORE_IN_A_ROW) || _SensorsAreFalsePositive) {
    _SensorsAreFalsePositive = true;
    Serial.println("bad state!");
    _NewScore = doubleCheckSensors();
    // reset if the sensors go back to a good state
    if (_NewScore) {
      _SensorsAreFalsePositive = false;
      _VibrationsDetectedInARow = 0;
      Serial.println("good state");
    }
  } else {
    // now confirmed that the vibration was a score
    _NewScore = _PossibleNewScore;
  }

  // read dfplayer acks as they arrive, otherwise the next play command waits up to 500 ms for the previous one's ack
  if (_AudioReady && _DfPlayer.available()) {
    Serial.print("dfplayer message ");
    Serial.print(_DfPlayer.readType());
    Serial.print(" ");
    Serial.println(_DfPlayer.read());
  }

  bool startPattern = false;
  if (_NewScore) {
    respondToScore();
    // turn on Idling checks
    _CheckForIdle = true;
    startPattern = true;
  } else {
    startPattern = updateCombo();
  }

  ringBell(_NewScore);

  if (_CheckForIdle && !_NewScore) {
    EVERY_N_SECONDS(5) {
      _StartIdling = checkIfIdle();
      Serial.println("check idle");
      if (_StartIdling) {
        // turn off idling check
        Serial.println("start idle");
        _CheckForIdle = false;
        _SelectedPattern = _IdlePattern;
        _ComboStage = 0;
      }
    }
  }

  bool draw = !_CurrentPatternAnimationFinished || startPattern || _StartIdling;

  if (draw) {
    bool startFromBeginning = _StartIdling || startPattern;

    if (startFromBeginning) {
      Serial.println("start from beginning");
    }

    _CurrentPatternAnimationFinished = drawPattern(startFromBeginning);

    showFrame();
    // once the pattern completes clear the LEDs
    if (_CurrentPatternAnimationFinished) {
      delay(10);        // wait a moment so that the last frame of the pattern is shown (not really important though)
      FastLED.clear();  // clear all pixel data
      FastLED.show();
    }
  } else {
    // just draw black occasionally
    EVERY_N_MILLISECONDS(300) {
      FastLED.show();
    }
  }
  // reset the start idle
  _StartIdling = false;
}

bool checkForVibration() {
  bool vibrationDetected = false;
  EVERY_N_MILLISECONDS_I(thisvibrationCheckTimer, _vibSensorCheckFrequency) {
    _vibSensorCheckFrequency = 1;

    _vibReading = digitalRead(VIBRATION_SENSOR_0_PIN);
    yield();
    if (_vibReading == 1) {
      _vibReading = digitalRead(VIBRATION_SENSOR_1_PIN);
      if (_vibReading == 1) {
        // dont check the sensors for a bit
        //  (presume that no one will score for a bit/ dont allow another score to interfere)
        _vibSensorCheckFrequency = PERIOD_WAIT_BETWEEN_SCORES;

        vibrationDetected = true;
      }
    }
    thisvibrationCheckTimer.setPeriod(_vibSensorCheckFrequency);
  }
  return vibrationDetected;
}

uint8_t _Speed = 3;  // frequency in ms to draw a pattern
bool _PatternFinished = true;
bool drawPattern(bool newScore) {
  if (newScore) {
    // prepare to draw a pattern
    FastLED.clear();  // clear all pixel data
    FastLED.show();
    // draw the first frame with newScore as true
    _PatternFinished = _SelectedPattern->draw(leds, newScore);
    return _PatternFinished;
  }

  // draw the pattern
  EVERY_N_MILLISECONDS_I(thisAnimationTimer, _Speed) {
    // speed can be controlled dynamically
    _Speed = _SelectedPattern->m_speed;
    thisAnimationTimer.setPeriod(_Speed);

    _PatternFinished = _SelectedPattern->draw(leds, newScore);
  }
  return _PatternFinished;
}

uint8_t _msCounter = 0;

void showFrame() {
  EVERY_N_MILLISECONDS(1) {
    // adjust for displays actual framerate.
    _msCounter++;
    if (_msCounter % NUM_MS_PER_FRAME == 0) {
      // draw the pattern
      FastLED.show();
    }
  }
}

uint8_t _BellSwingLoopCount = 0;
bool _SwingBell = false;

void ringBell(bool newScore) {
  if (newScore) {
    _SwingBell = true;
  }
  if (_SwingBell) {
    digitalWrite(SERVO_ARDUINO_PIN, HIGH);
    _BellSwingLoopCount++;
    if (_BellSwingLoopCount > 20) {
      // reset
      _BellSwingLoopCount = 0;
      _SwingBell = false;
      digitalWrite(SERVO_ARDUINO_PIN, LOW);
      yield();
    }
  }
}

// the behavior observed is that both of the sensors can be stuck in a high or low after a vibration, and upon hitting the sensor again one will change states.
// and so the idea is to wait for that change, signifying that it was struck. This logic is working the best.
bool doubleCheckSensors() {
  return checkVibrationSensorsAreDifferent();
}

bool checkVibrationSensorsAreDifferent() {
  _vibReading = digitalRead(VIBRATION_SENSOR_1_PIN);
  _vibReading2 = digitalRead(VIBRATION_SENSOR_0_PIN);
  yield();

  if (_vibReading != _vibReading2) {
    return true;
  }

  return false;
}

bool checkIfIdle() {
  if (millis() - _TimeOfLastScore > (HOW_LONG_TO_WAIT_TO_IDLE_S * 1000)) {
    return true;
  }
  return false;
}

// picks this score's combo stage, track and pattern. a new score always cuts off the current track and pattern
void respondToScore() {
  unsigned long now = millis();

  if (isSpinning()) {
    bool comboContinues = _ComboStage > 0 && _ComboStage < COMBO_JACKPOT_STAGE && now - _TimeOfLastScore <= COMBO_WINDOW_MS;
    _ComboStage = comboContinues ? _ComboStage + 1 : 1;

    if (_ComboStage == COMBO_JACKPOT_STAGE) {
      selectRandomCelebration(false);
    } else {
      _BuildUpPattern->setStage(_ComboStage);
      _SelectedPattern = _BuildUpPattern;
    }
    // tracks 0001-0005 line up with the combo stages
    playTrack(_ComboStage);
  } else {
    _ComboStage = 0;
    selectRandomCelebration(false);
    playTrack(TRACK_NOT_SPINNING);
  }

  _TimeOfLastScore = now;
  Serial.print("score, combo stage ");
  Serial.println(_ComboStage);
}

// ends a combo once its window runs out and chains the jackpot patterns. returns true when it picked a pattern to start
bool updateCombo() {
  if (_ComboStage == 0) {
    return false;
  }

  if (_ComboStage < COMBO_JACKPOT_STAGE) {
    if (millis() - _TimeOfLastScore > COMBO_WINDOW_MS) {
      Serial.println("combo ended");
      _ComboStage = 0;
      selectRandomCelebration(false);
      return true;
    }
    return false;
  }

  // jackpot: the celebration patterns are short, so keep starting new ones until track 0005 has had time to play out
  if (_CurrentPatternAnimationFinished) {
    if (millis() - _TimeOfLastScore < JACKPOT_LENGTH_MS) {
      Serial.println("jackpot next pattern");
      selectRandomCelebration(true);
      return true;
    }
    _ComboStage = 0;
  }
  return false;
}

void selectRandomCelebration(bool differentFromCurrent) {
  uint8_t index = random8(NUM_PATTERNS);
  if (differentFromCurrent && NUM_PATTERNS > 1 && _CelebrationPatterns[index] == _SelectedPattern) {
    index = (index + 1 + random8(NUM_PATTERNS - 1)) % NUM_PATTERNS;
  }
  _SelectedPattern = _CelebrationPatterns[index];
  Serial.print("pattern ");
  Serial.println(index);
}

bool isSpinning() {
#if SPIN_SENSOR_TYPE == SPIN_SENSOR_PULSE
  return _LastSpinPulseMs != 0 && millis() - _LastSpinPulseMs < SPIN_PULSE_TIMEOUT_MS;
#else
  return digitalRead(SPIN_SENSOR_PIN) == LOW;
#endif
}

void playTrack(uint8_t track) {
  if (_AudioReady) {
    // a play command replaces whatever is playing, so the new hit's track starts right away
    _DfPlayer.playMp3Folder(track);
  }
}