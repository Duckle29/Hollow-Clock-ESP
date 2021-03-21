#pragma once
#include <Arduino.h>

// --- Probably the only things you want to change --- //
const uint8_t pins[] = {
    D5, D6, D7, D8, // Motor pins. IN1 ... IN4
    // Button pins: fast reverse, tune reverse, tune forward, fast forward
    D1, D2, D3, D4 // Set to -1 if one isn't used
};

#define USER_TZ TZ_Europe_Copenhagen // You can look for your timezone here https://github.com/gmag11/ESPNtpClient/blob/main/src/TZdef.h
// ------------------------------------------------ //

// --- These are here if you want to tinker --- //
const char *ntp_server = "pool.ntp.org";

const int32_t move_interval = 10; //[s] Move the indicator ever 10 seconds

const uint16_t steps_per_rotation = 4096;
const uint32_t seconds_per_rotation = 15 * 60;
const uint8_t rotations_per_hour = 3600 / seconds_per_rotation;

// 12V motor can go to ~40 rpm
const uint8_t rpms[] = {1, 3, 10, 5}; // [rpm] Speeds for Clock movement, slow tune, fast tune, dst adjust
const uint32_t print_interval = 5000; //[ms] How often to print debug info. Set to 0 to disable

const int32_t update_interval = 250; //[ms] check the time every 250 ms
