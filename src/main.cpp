#include "main.hpp"

void setup()
{
  Serial.begin(115200);

  // Configure pins
  for (uint8_t i = 0; i < sizeof(pins) / sizeof(pins[0]); i++)
  {
    // Set first 4 pins as outputs, remaining pins as input_pullup
    if (pins[i] >= 0)
    {
      pinMode(pins[i], i < 4 ? OUTPUT : INPUT_PULLUP);
    }
  }

  // Calculate delays
  for (int i = 0; i < sizeof(delays) / sizeof(delays[0]); i++)
  {
    delays[i] = get_delay(rpms[i]);
  }

  // Set up wifi
  AsyncWiFiManager wifiManager(&server, &dns);
  wifiManager.autoConnect("HollowClock_setup");

  time_sync();
  Serial.print("Getting NTP sync");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.print("\nGot sync. Time is: ");

  char buff[30];
  get_iso8601_stamp(buff, sizeof(buff));
  Serial.println(buff);

  struct tm *ptm = localtime(&now);
  is_dst = ptm->tm_isdst;

  calculate_move(true);
}

void loop()
{
  time(&now);
  calculate_move();
  step(delays[0]);
  handle_ui();
}

void calculate_move(bool init)
{
  // Exit  if it's not time to update
  if (millis() % update_interval && !init)
  {
    return;
  }

  // Get current time
  time_t now = time(nullptr);
  struct tm *ptm = localtime(&now);

  if ((bool)ptm->tm_isdst != is_dst)
  {
    is_dst = ptm->tm_isdst;

    is_dst ? steps_left += steps_total : steps_left -= steps_total;
  }

  // Calculate new indicator position
  int new_position_steps = (ptm->tm_min * 60 + ptm->tm_sec) * seconds_to_steps;

  int step_diff = new_position_steps - current_position;

  // If the position has wrapped to a new hour
  if (step_diff < 0)
  {
    step_diff += steps_total;
  }

  // Apply the movement if it's time to move.

  if ((now - last_movement) > move_interval || init)
  {
    last_movement = now;
    current_position += step_diff;

    // Constrain current_position to be within [0,steps_total[
    if (current_position >= steps_total)
    {
      current_position -= steps_total;
    }
    else if (current_position < 0)
    {
      current_position += steps_total;
    }
    
    // If we're not initializing the variables, move.
    if (!init)
    {
      steps_left += step_diff;
      pstrbuff += snprintf(pstrbuff, sizeof(strbuff) - (pstrbuff - strbuff) - 1, "Moving %d steps ", step_diff);
    }
    else
    {
      pstrbuff += snprintf(pstrbuff, sizeof(strbuff) - (pstrbuff - strbuff) - 1, "Initialization. Would've moved %d steps ", step_diff);
    }
  }

  // info print
  if (millis() - last_print > print_interval && print_interval != 0)
  {
    last_print = millis();
    char buff[30];
    get_iso8601_stamp(buff, sizeof(buff));

    Serial.printf("%s |  target: %d/%d\tcurrent: %d/%d\t%s\n",
                  buff,
                  new_position_steps, steps_total,
                  current_position, steps_total, strbuff);

    strbuff[0] = '\0';
    pstrbuff = strbuff;
  }
}

// This will handle 4 buttons if their pin is set to anything but -1. Any combination can be used.
// See more in config
void handle_ui()
{
  uint8_t state = 0;
  for (uint8_t i = 4; i < sizeof(pins) / sizeof(pins[0]); i++)
  {
    if (pins[i] >= 0)
    {
      state |= !digitalRead(pins[i]) << (i - 4);
    }
  }

  switch (state)
  {
  case 0b1000: // Forward fast
    steps_left++;
    step(delays[2]);
    break;

  case 0b0100: // Forward slow
    steps_left++;
    step(delays[1]);
    break;

  case 0b0010: // Reverse slow
    steps_left--;
    step(delays[1]);
    break;

  case 0b0001: // Reverse fast
    steps_left--;
    step(delays[2]);
    break;

  case 0b0000:
    // Do nothing
    break;

  default:
    pstrbuff += snprintf(pstrbuff, sizeof(strbuff) - (pstrbuff - strbuff) - 1, "multiple buttons held");
  }
}

// Get an ISO8601 timestapt string
size_t get_iso8601_stamp(char *pbuff, size_t maxlen)
{
  time_t now = time(nullptr);
  struct tm *ptm = localtime(&now);

  return strftime(pbuff, maxlen, "%FT%T%z", ptm);
}

uint16_t get_delay(int rpm)
{
  return (60000000 / (steps_total * (unsigned long)rpm));
}

void time_sync()
{
#ifdef ESP8266
  configTime(USER_TZ, ntp_server);
#else
  configTzTime(USER_TZ, ntp_server);
#endif
}

void step(uint16_t _delay)
{
  if (steps_left > 0)
  {
    stepCW(_delay);
    steps_left--;
  }
  else if (steps_left < 0)
  {
    stepCCW(_delay);
    steps_left++;
  }
  else // If we're not stepping, turn off the motor
  {
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(pins[i], LOW);
    }
  }
}

// This gets groups of 4 bits from the sequence, left-to-right, and writes it to the output
void stepCCW(uint16_t _delay)
{
  for (int i = sequence_steps; i >= 0; i--)
  {
    for (int k = 3; k >= 0; k--)
    {
      bool b = (1 << (4 * i + k)) & sequence;
      digitalWrite(pins[3 - k], b);
    }
    delayMicroseconds(_delay);
  }
}

// This gets the groups of 4 right-to-left, but still gets the bits left-to-right in the group of 4
void stepCW(uint16_t _delay)
{
  for (int i = 0; i < sequence_steps; i++)
  {
    for (int k = 3; k >= 0; k--)
    {
      bool b = (1 << (4 * i + k)) & sequence;
      digitalWrite(pins[3 - k], b);
    }
    delayMicroseconds(_delay);
  }
}
