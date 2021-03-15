#include <Arduino.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> 

#include <TZ.h>
#include <time.h>

#include <CheapStepper.h>

#define USER_TZ TZ_Europe_Copenhagen

const uint16_t steps_per_rotation = 4096;
const uint32_t seconds_per_rotation = 15*60;
const uint8_t rotations_per_hour = 4;

const uint32_t step_interval = 5 * 1000; // Perform steps every 5 seconds
const uint32_t sync_interval = 24 * 60 * 60 * 1000; // NTP sync every day

uint8_t current_minute = 0;

AsyncWebServer server(80);
DNSServer dns;

CheapStepper stepper(D1, D2, D3, D4);

// Prototypes
void do_step(void);
void time_sync(void);