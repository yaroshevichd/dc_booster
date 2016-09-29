#include <TaskScheduler.h>

void measure_solar();
void measure_light();

int gSolarPin = A0;
int gLightPin = A1;
Scheduler gTaskMgr;
Task gTaskMeasureSolar(1000, TASK_FOREVER, &measure_solar, &gTaskMgr);
Task gTaskMeasureLight(1000, TASK_FOREVER, &measure_light, &gTaskMgr);

long readVcc()
{
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}

void setup()
{
  analogReference(INTERNAL);
  pinMode(gSolarPin, INPUT);
  pinMode(gLightPin, INPUT);
  Serial.begin(115200);
  Serial.println("Start");
}

void loop()
{
  gTaskMgr.execute();
}

void measure_solar()
{
  float ref_volts = readVcc() / 1000.0;
  int adc_val = analogRead(gSolarPin);
  float volts = float(adc_val) * ref_volts / 1024.0;
}

void measure_light()
{
  float ref_volts = readVcc() / 1000.0;
  int adc_val = analogRead(gLightPin);
  float volts = float(adc_val) * ref_volts / 1024.0;
}

