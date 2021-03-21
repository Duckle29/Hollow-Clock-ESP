#pragma once

#if !defined(ESP32) && !defined(ESP8266)
#error This sketch works only for the ESP8266 and ESP32
#endif

#include <Arduino.h>
#include "config.hpp"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <TZ.h>
#else
#include <WiFi.h>
#include "TZdef.h"
#endif

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>

#include <time.h>

AsyncWebServer server(80);
DNSServer dns;

const uint8_t sequence_steps = 8;
const uint32_t sequence = 0b10001100010001100010001100011001;                            // 8 sections of 4 bits. 32bit constant to store the sequence
const uint16_t steps_total = (steps_per_rotation * rotations_per_hour) / sequence_steps; // We'll always do a full sequence of steps, so reduce total stu
const float seconds_to_steps = steps_total / 3600.0;
uint16_t delays[] = {0, 0, 0}; // This will be calculated in setup

int32_t steps_left = 0;

time_t now;
time_t last_movement;
time_t last_ntp_sync;

uint32_t last_step;
uint32_t last_print;
uint32_t current_position;

bool is_dst;

char strbuff[200];
char *pstrbuff = strbuff;

// Prototypes
void handle_ui(void);
void calculate_move(bool init = false);
void time_sync(void);

size_t get_iso8601_stamp(char *, size_t);
uint16_t get_delay(int);

void step(uint16_t);
void stepCW(uint16_t);
void stepCCW(uint16_t);