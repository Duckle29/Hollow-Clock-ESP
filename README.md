# Hollow-Clock-ESP

This is my code for running the [Hollow clock V2](https://www.thingiverse.com/thing:4761858)
with an ESP using NTP for time synchronization.

[Video on the code](https://www.youtube.com/watch?v=ZQMcDnMxaQs)

# How to use it
This uses [platformIO](https://platformio.org/platformio-ide) to compile and upload the code. 
The best experience is through VSCode, but if you just want to upload this program you can use [PIO
core](https://docs.platformio.org/en/latest/core/installation.html#installer-script)
straight on the command-line. 
Just `pio run -t upload` if you're using a d1_mini like me, or add `-e esp32` if
using a esp32 development board.  
You can also use `pio run -t upload -e esp8266_ota` if you want to upload over OTA (after flashing over serial once)

## config
All user config is done in `include/config.hpp`. You really should only need to change the pin assignments,
but other options are there if you want to tinker.

## Buttons
The firmware supports adding buttons for moving the dial forward or backwards.
You configure one or more of the buttons by setting their pin number in config.hpp
The four buttons are:

- Fast reverse
- Slow reverse
- Slow forward
- Fast forward
