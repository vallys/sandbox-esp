#include <Arduino.h>
#include <TimeLib.h>

#define TIME_HEADER "T" // Header tag for serial time sync message
#define TIME_REQUEST 7  // ASCII bell character requests a time sync message

time_t startTime;

time_t requestSync();
void digitalClockDisplay();
void printDigits(int digits);
void processSyncMessage();

void setup()
{
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  setSyncProvider(requestSync); //set function to call when sync required
  Serial.println("Waiting for sync message");
}

void loop()
{
  if (Serial.available())
  {
    processSyncMessage();
  }
  if (timeStatus() != timeNotSet)
  {
    digitalClockDisplay();

    char c_dateTime[20];
    char c_startTime[20];

    sprintf(c_startTime, "%02d:%02d", hour(startTime), minute(startTime));
    uint8_t elapsedSeconds = now() - startTime;
    Serial.println(elapsedSeconds);
    uint8_t hours = elapsedSeconds / 3600;
    uint8_t minutes = (elapsedSeconds / 60) % 60;
    uint8_t seconds = elapsedSeconds % 60;
    sprintf(c_dateTime, "%02d:%02d:%02d", hours, minutes, seconds);

    Serial.println(c_startTime);
    Serial.println(c_dateTime);
  }

  if (timeStatus() == timeSet)
  {
    digitalWrite(2, HIGH); // LED on if synced
  }
  else
  {
    digitalWrite(2, LOW); // LED off if needs refresh
  }
  delay(1000);
}

void digitalClockDisplay()
{
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year());
  Serial.println();
}

void printDigits(int digits)
{
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void processSyncMessage()
{
  unsigned long pctime;
  const unsigned long DEFAULT_TIME = 1618594735; // Jan 1 2013

  if (Serial.find(TIME_HEADER))
  {
    pctime = Serial.parseInt();
    if (pctime >= DEFAULT_TIME)
    {                  // check the integer is a valid time (greater than Jan 1 2013)
      setTime(pctime); // Sync Arduino clock to the time received on the serial port
      startTime = now();
    }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}