#pragma once

#if !defined(ESP32) && !defined(ESP8266)
  #error This sketch works only for the ESP8266 and ESP32
#endif

#include <Arduino.h>
#include "config.hpp"

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
#else
    #include <WiFi.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>

#include <time.h>
#include <ESPNtpClient.h>
#include <CheapStepper.h>

AsyncWebServer server(80);
DNSServer dns;

CheapStepper stepper(pins[0], pins[1], pins[2], pins[3]);

time_t last_movement;
uint16_t last_position_deg;
uint32_t last_print;
char strbuff[50];

// Prototypes
void move(bool init=false);
void time_sync(void);
int deg_rollover(int);