# Frigger

Frigger monitors opening/closing of the fridge (and freezer),
sounds alarm if the door has been open for too long and summarizes
the total open time storing it in EPROM (up to 256 hours).
Connect pin `PIN_TEST` to the ground to put it in test mode
which will continue to beep when either door is open.
Connect `PIN_RESET` to ground to reset the counter.


![alt text](https://github.com/ArchangelDesign/frigger/blob/master/img/schematic.png)


### Configure:
// Main alarm trigger
#define MAX_OPEN_TIME 90 // 45 seconds
// no sound period
#define QUIET_TIME 30 // 15 seconds
// quiet alarm trigger
#define QUIET_ALARM_TIME 60 // 30 seconds
// turn off main alarm
#define MAX_ALARM_TIME 140 // 70 seconds

### Hardware:
- Arduino (any)
- Active buzzer
- 2 magnetic switches (or any NC contact switches)
- 1306 OLED Display

### Required libraries:
- adafruit/Adafruit SSD1306@^2.4.6
- adafruit/Adafruit GFX Library@^1.10.12
- adafruit/Adafruit BusIO@^1.9.3
- contrem/arduino-timer@^2.3.0
