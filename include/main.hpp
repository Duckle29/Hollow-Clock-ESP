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
#include <CheapStepper.h>

AsyncWebServer server(80);
DNSServer dns;

CheapStepper stepper(motor_pins[0], motor_pins[1], motor_pins[2], motor_pins[3]);

time_t last_movement;
uint16_t last_position_deg;
uint32_t last_print;
char strbuff[200];
char * pstrbuff = strbuff;

// Prototypes
void move(bool init=false);
void time_sync(void);
int deg_rollover(int);
void handle_ui(void);
size_t get_iso8601_stamp(char*, size_t);