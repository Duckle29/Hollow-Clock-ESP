#include "main.hpp"

void setup() 
{
  Serial.begin(115200);

  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.autoConnect("HollowClock_setup");

  configTime(USER_TZ, "pool.ntp.org");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  Serial.print(ctime(&now));

  stepper.setTotalSteps(steps_per_rotation);
  stepper.setRpm(6); // Lowest allowed is 6
  Serial.println(stepper.getDelay());

}

bool step_triggered, sync_triggered;
void loop() 
{
  if(millis() % step_interval == 0)
  {
    if(!step_triggered)
    {
      step_triggered = true;
      do_step();
    }
  }
  else
  {
    step_triggered = false;
  }

  if((millis()) % sync_interval == 0 && !sync_triggered)
  {
    if(!sync_triggered)
    {
      sync_triggered = true;
      // Sync time with NTP server
      configTime(USER_TZ, "pool.ntp.org");
    }
  }
  else
  {
    sync_triggered = false;
  }
  stepper.run();
}


void do_step()
{
  time_t now = time(nullptr);
  struct tm * ptm = gmtime(&now);
  uint8_t deg = (ptm->tm_min - current_minute) * 6;  // minute to degrees
  current_minute = ptm->tm_min;

  Serial.println(ctime(&now));
  

  stepper.newMoveDegreesCCW(deg * rotations_per_hour);
}
