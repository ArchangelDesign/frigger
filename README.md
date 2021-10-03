# Frigger

Frigger monitors opening/closing of the fridge (and freezer),
sounds alarm if the door has been open for too long and summarizes
the total open time storing it in EPROM (up to 256 hours).
Connect pin `PIN_TEST` to the ground to put it in test mode
which will continou to beep when either door is open.

Hardware:
- Arduino (any)
- Active buzzer
- 2 magnetic switches (or any NC contact switches)

Required libraries:
- adafruit/Adafruit SSD1306@^2.4.6
- adafruit/Adafruit GFX Library@^1.10.12
- adafruit/Adafruit BusIO@^1.9.3
- contrem/arduino-timer@^2.3.0
