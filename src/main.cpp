#include <Arduino.h>
#include <math.h>

int frequencyPin = PIN_A0;
int dutyCyclePin = PIN_A1;

int output1Pin = 2;
int output2Pin = 3;

long frequencyRange[2] = {50, 400}; // Hz-ben

double dutyCycle;
long frequency; // Hz-ben

unsigned long timeMax = 4294967295;
unsigned long periodStart; // microszekundumban
bool firstStart = true;

double inputToPercentage(double input, int decimalPlaces)
{
  double maxValue = 1023;
  double proportion = input / maxValue;
  double percent = proportion * 100;

  decimalPlaces = pow(10, decimalPlaces);

  int rounded = (double)round(percent * decimalPlaces) / decimalPlaces;

  return rounded;
}

long inputToValueInRange(double input, long range[])
{
  double proportion = inputToPercentage(input, 2) / 100;

  long diff = range[1] - range[0];

  long valueInRange = range[0] + diff * proportion;

  return round(valueInRange);
}

long getPeriodTime(long frequency)
{
  double periodTimeMicros = (double)1000000 / frequency;

  return round(periodTimeMicros);
}

unsigned long timeDifference(unsigned long startTime, unsigned long currentTime)
{
  if (startTime > currentTime)
  {
    // overflowolt
    unsigned long microsToMax = timeMax - startTime;
    return microsToMax + currentTime;
  }

  return currentTime - startTime;
}

void setup()
{
  pinMode(frequencyPin, INPUT);
  pinMode(dutyCyclePin, INPUT);
  pinMode(output1Pin, OUTPUT);
  pinMode(output2Pin, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  int dutyCycleInputValue = analogRead(dutyCyclePin);
  int frequencyInputValue = analogRead(frequencyPin);

  /* dutyCycle = inputToPercentage(dutyCycleInputValue, 2);
  frequency = inputToValueInRange(frequencyInputValue, frequencyRange); */
  dutyCycle = 50;
  frequency = 200;

  long periodTime = getPeriodTime(frequency);

  // új periódus kezdődik ha:
  // - ez az első periódus
  // - előző periódusnak vége van (több idő telt el azóta, mint a periódusidő)
  if (firstStart || timeDifference(periodStart, micros()) >= (unsigned long)periodTime)
  {
    firstStart = false;
    periodStart = micros();
  }

  unsigned long firstOutputThreshold = round(periodTime * (dutyCycle / 100));

  unsigned long timeSincePeriodStart = timeDifference(periodStart, micros());

  // annyi idő telt már el, hogy a második kimenet következik
  if (timeSincePeriodStart > firstOutputThreshold)
  {
    digitalWrite(output1Pin, LOW);
    digitalWrite(output2Pin, HIGH);
  }
  else
  {
    digitalWrite(output1Pin, HIGH);
    digitalWrite(output2Pin, LOW);
  }
}
