# Hollow-Clock-ESP

This is my code for running the [Hollow clock V2](https://www.thingiverse.com/thing:4761858)
with an ESP using NTP for time synchronization.

It even does DST correction if you set the right timezone :)


# How to use it
This uses [platformIO](https://platformio.org/platformio-ide) to compile and upload the code. 
The best experience is through VSCode, but if you just want to upload this program you can use [PIO
core](https://docs.platformio.org/en/latest/core/installation.html#installer-script)
straight on the command-line. 
Just `pio run -t upload` if you're using a d1_mini like me, or add `-e esp32` if
using a esp32 development board.

## Config
All user config is done in `include/config.hpp`.

## Buttons
The firmware supports adding buttons for moving the dial forward or backwards.
You configure one or more of the buttons by setting their pin number in config.hpp
The four buttons are:
- Fast reverse
- Slow reverse
- Slow forward
- Fast forward

On something like the D1 mini, the pins available are *very* limited.
Recommended pinout if using buttons would be:  
`{D5, D6, D7, D8,  // IN1..IN4 `  
`D1, D2, D3, D4} // Buttons`  

This is because some IO are pulled up or down during and after boot. This way, the pins for the driver won't be high on boot, and the pins for the buttons will have pull-ups available. 
