# LooffCarousel.RingTargetDetector
An Arduino project for detecting successful tosses of carousel rings and rewarding the throwers with LED and sound feedback.

## Versions

Two firmware versions are provided, select the one that matches your hardware:

| Folder | Target board | Notes |
| ------ | ------------ | ----- |
| [sketch_esp32](./sketch_esp32) | ESP32 Dev Module (`esp32:esp32:esp32`) | Supported target going forward |
| [sketch_esp8266](./sketch_esp8266) | NodeMCU ESP8266 (`esp8266:esp8266:nodemcuv2`) | Legacy version, kept for existing hardware |

## Hardware (ESP32)

Firmware lives in [./sketch_esp32](./sketch_esp32).

| Signal | GPIO |
| ------ | ---- |
| LED strip 0-5 | 4, 5, 18, 19, 21, 22 |
| Vibration sensor 0 | 16 |
| Vibration sensor 1 | 13 |
| Bell servo | 23 |
| DFPlayer Mini RX (ESP32 TX) | 25 |
| DFPlayer Mini TX (ESP32 RX) | 26 |
| Spin sensor (active low, internal pull-up) | 32 |

Libraries: FastLED 3.10.5 and DFRobotDFPlayerMini 1.0.6. The SD card layout is in [./sdcard](./sdcard).

`SPIN_SENSOR_TYPE` in `sketch_esp32.ino` picks how the spin sensor is read: `SPIN_SENSOR_LEVEL` treats a low pin as spinning, `SPIN_SENSOR_PULSE` treats a falling edge within the last `SPIN_PULSE_TIMEOUT_MS` as spinning. With nothing wired to GPIO 32 the carousel reads as not spinning.

## Wokwi

Develop patterns with the [Wokwi project](./Wokwi)  

### Intro video:
https://www.loom.com/share/6214540d130b4ce48f3a302fa1ada0d5

## Fastled

see https://github.com/FastLED/FastLED/wiki/Basic-usage

![](./readmeAssets/targetRendering.jpg)

![](./readmeAssets/demo.gif)
