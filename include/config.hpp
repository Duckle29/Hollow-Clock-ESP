#pragma once
#include <Arduino.h>

// Probably the only things you want to change:
    const uint8_t motor_pins[] = {D1, D2, D3, D4};  // The pins you've connected IN1...IN4 to

    // Pins used to rotate the motor for manual adjustments. Connect buttons between an pin and gnd (active low)
    // {rev fast, rev fine, fwd fine, fwd fast}
    const int ui_pins[] = {-1, -1, -1, -1};

    #define USER_TZ TZ_Europe_Copenhagen  // You can look for your timezone here https://github.com/gmag11/ESPNtpClient/blob/main/src/TZdef.h

    char* hostname = "HollowClock"; // Remember to change this in the OTA environments if you use ota
    char* OTA_pass = "ota_password";

// These are here if you want to tinker
    const char* ntp_server = "pool.ntp.org";
    

    const int32_t move_interval = 60; //[s] Perform steps every minute
    const uint16_t steps_per_rotation = 4096; 
    const uint32_t seconds_per_rotation = 15*60; 
    const uint8_t rotations_per_hour = 4;
    const uint32_t print_interval = 5000; //[ms] How often to print debug info. Set to 0 to disable

    const int32_t update_interval = 100; //[ms] check the time every 100 ms