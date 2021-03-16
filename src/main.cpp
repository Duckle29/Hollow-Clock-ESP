#include "main.hpp"

void setup() 
{
  Serial.println(USER_TZ);
  Serial.begin(115200);

  // IO setup
  for (int i=0; i<sizeof(ui_pins)/sizeof(ui_pins[0]); i++)
  {
    if (ui_pins[i] >= 0)
    {
      pinMode(ui_pins[i], INPUT_PULLUP);
    }
  }
  
  // WiFi setup
  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.autoConnect((String(hostname) + "_setup").c_str());

  // OTA setup
  ota.setup((char*)hostname, (char*)OTA_pass);
  ota.onStart([] () {
    Serial.println("OTA START");
  });
  ota.onEnd([] () {
    Serial.println(".");
    Serial.println("OTA END");
  });
  ota.onError([] () {
    Serial.println("OTA ERROR");
  });
   ota.onProgress([] () {
     int p = ota.getProgress();
     if (p % 5 == 0) {
       Serial.print(".");
     }
  });

  // NTP setup
  NTP.setTimeZone (USER_TZ);
  NTP.begin();
  Serial.print("Getting NTP sync");
  while(!NTP.getFirstSync())
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nGot sync. Time is: ");
  Serial.println(NTP.getTimeDateString(time(nullptr), "%FT%T%z\n"));

  // Stepper setup
  move(true);
  stepper.setTotalSteps(steps_per_rotation);
  stepper.setRpm(6); // Lowest allowed is 6
}

void loop() 
{
  handle_ui();
  move();
  ota.loop();
  stepper.run();
}

// This will handle 4 buttons if their pin is set to anything but -1. Any combination can be used.
// See more in config
void handle_ui()
{
  uint8_t state = 0;
  for (int i=0; i<sizeof(ui_pins)/sizeof(ui_pins[0]); i++)
  {
    if (ui_pins[i] >= 0)
    {
      state |= !digitalRead(ui_pins[i]) << i;
    }
  }

  int rpm = stepper.getRpm();
  switch (state)
  {
    case 0b1000: // Forward fast
      stepper.setRpm(20);
      stepper.stepCCW();
      break;

    case 0b0100:  // Forward slow
      stepper.setRpm(6);
      stepper.stepCCW();
      break;

    case 0b0010:  // Reverse slow
      stepper.setRpm(6);
      stepper.stepCW();
      break;
    
    case 0b0001:  // Reverse fast
      stepper.setRpm(20);
      stepper.stepCW();
      break;
    
    case 0b0000:
      // Do nothing
      break;
    
    default:
      pstrbuff += snprintf(pstrbuff, sizeof(strbuff)-(pstrbuff-strBuffer)-1, "multiple buttons held");

  }
  stepper.setRpm(rpm);
}

void move(bool init)
{
  if (millis() % update_interval && !init)
  {
    return;
  }
  time_t now = time(nullptr);
  struct tm * ptm = localtime(&now);

  int new_position_deg = (ptm->tm_min * 60 + ptm->tm_sec) / 10;
  int deg = deg_rollover(new_position_deg - last_position_deg);

  if (millis()-last_print > print_interval && print_interval != 0)
  {
    last_print = millis();
    Serial.print(NTP.getTimeDateString(now, "%FT%T%z"));
    Serial.printf("|  target: %d°\tcurrent: %d°\t%s\n", new_position_deg, last_position_deg, strbuff);
    strbuff[0] = '\0';
  }

  if((now - last_movement) > move_interval && !init)
  {
    last_movement = now;
    last_position_deg = new_position_deg;
    pstrbuff += snprintf(pstrbuff, sizeof(strbuff)-(pstrbuff-strBuffer)-1, "Moving %d°", deg);
    stepper.newMoveDegreesCCW(deg * rotations_per_hour);
  }
  if (init)
  {
    last_movement = now;
    last_position_deg = new_position_deg;
    pstrbuff += snprintf(pstrbuff, sizeof(strbuff)-(pstrbuff-strBuffer)-1, "Initialization. Would've moved %d°", deg);
  }
}

// Mostly taken from CheapStepper source.
int deg_rollover(int deg)
{
  // keep to 0-359 range
  if (deg >= 360 || deg < 0) 
  {
    deg %= 360; // returns negative if deg not multiple of 360
  }
  if (deg < 0)
  {
    deg += 360; // shift into 0-359 range
  }

  return deg;
}