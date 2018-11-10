# intro-cyber-systems-files

All programs created by me as a part of DTU's 02135 "Introduction to Cyber Systems" course. All other files (partially created by other students or lecturers) are not included in this folder.

# Files

All files have been used on Feather Huzzah ESP8266, a microPython board (unless specified otherwise). Multiple devices have been connected to this board, including: buttons, LED lights, neopixels, MCP9808 temperature sensor and MCP3008 ADC conversion chip (for potentiometer).

* Program 2.py - switch LEDs after each button press.

* Program 3.py - switch LEDs depending on temperature sensor input.

* Program 4.py - switch neopixels (RGB pixels with brightness control) depending on temperature sensor input.

* Program 5.py - change brightness of a neopixel depending on analog potentiometer input.

* REST.py - an API to read inputs (from temperature sensor, potentiometer, pins) and control whether a pin is on or off (for purposes of e.g. switching LEDs)

* Setter.py - a Python program that utilises API provided by REST.py, used on a computer rather than on a board

# What I have learned?

This course has provided me with experience of working with IoT-like devices and their logic. I have learned how to interact with hardware devices (including ones with not-so-obvious protocols, as in case of potentiometer, where data is written on first 10 bits) and how to relay that data so that it can be used for other purposes (by building an API).