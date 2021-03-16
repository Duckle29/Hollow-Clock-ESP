#include "main.hpp"

void setup() 
{
  Serial.println(USER_TZ);
  Serial.begin(115200);

  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.autoConnect("HollowClock_setup");

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

  move(true);

  stepper.setTotalSteps(steps_per_rotation);
  stepper.setRpm(6); // Lowest allowed is 6
}

void loop() 
{
  move();
  stepper.run();
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
    snprintf(strbuff, sizeof(strbuff), "Moving %d°", deg);
    stepper.newMoveDegreesCCW(deg * rotations_per_hour);
  }
  if (init)
  {
    last_movement = now;
    last_position_deg = new_position_deg;
    snprintf(strbuff, sizeof(strbuff), "Initialization. Would've moved %d°", deg);
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