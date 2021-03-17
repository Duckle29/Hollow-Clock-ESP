#include "main.hpp"

void setup() 
{
  Serial.println(USER_TZ);
  Serial.begin(115200);

  for (int i=0; i<sizeof(ui_pins)/sizeof(ui_pins[0]); i++)
  {
    if (ui_pins[i] >= 0)
    {
      pinMode(ui_pins[i], INPUT_PULLUP);
    }
  }
  

  AsyncWiFiManager wifiManager(&server,&dns);
  wifiManager.autoConnect("HollowClock_setup");

  #ifdef ESP8266
    configTime(USER_TZ, ntp_server);
  #else
    configTzTime(USER_TZ, ntp_server);
  #endif

  Serial.print("Getting NTP sync");
  time_t now = time(nullptr);
  while(now < 8*3600*2)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }

  Serial.print("\nGot sync. Time is: ");

  char buff[30];
  get_iso8601_stamp(buff, sizeof(buff));
  Serial.println(buff);

  move(true);

  stepper.setTotalSteps(steps_per_rotation);
  stepper.setRpm(slow_rpm); // Lowest allowed is 6
}

void loop() 
{
  handle_ui();
  move();
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
      stepper.setRpm(fast_rpm);
      stepper.moveCCW(8);
      break;

    case 0b0100:  // Forward slow
      stepper.setRpm(slow_rpm);
      stepper.moveCCW(8);
      break;

    case 0b0010:  // Reverse slow
      stepper.setRpm(slow_rpm);
      stepper.moveCW(8);
      break;
    
    case 0b0001:  // Reverse fast
      stepper.setRpm(fast_rpm);
      stepper.moveCW(8);
      break;
    
    case 0b0000:
      // Do nothing
      break;
    
    default:
      pstrbuff += snprintf(pstrbuff, sizeof(strbuff)-(pstrbuff- strbuff)-1, "multiple buttons held");

  }
  stepper.setRpm(rpm);
}

size_t get_iso8601_stamp(char * pbuff, size_t maxlen)
{
  time_t now = time(nullptr);
  struct tm * ptm = localtime(&now);

  return strftime(pbuff, maxlen, "%FT%T%z", ptm);
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
    char buff[30];
    get_iso8601_stamp(buff, sizeof(buff));

    Serial.printf("%s|  target: %d°\tcurrent: %d°\t%s\n", buff, new_position_deg, last_position_deg, strbuff);

    strbuff[0] = '\0';
    pstrbuff = strbuff;
  }

  if((now - last_movement) > move_interval && !init)
  {
    last_movement = now;
    last_position_deg = new_position_deg;
    pstrbuff += snprintf(pstrbuff, sizeof(strbuff)-(pstrbuff-strbuff)-1, "Moving %d°", deg);
    stepper.newMoveDegreesCCW(deg * rotations_per_hour);
  }
  if (init)
  {
    last_movement = now;
    last_position_deg = new_position_deg;
    pstrbuff += snprintf(pstrbuff, sizeof(strbuff)-(pstrbuff-strbuff)-1, "Initialization. Would've moved %d°", deg);
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