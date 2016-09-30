#include <TaskScheduler.h>

void measure_solar();
void measure_light();
void print_volts();
void prepare();

const int gSolarPin = A0;
const int gLightPin = A1;
float gVoltsRef = .0;
int gRawSolar = 0;
float gVoltsSolar = .0;
int gRawLight = 0;
float gVoltsLight = .0;
Scheduler gTaskMgr;
Task gTaskPrepare(100, TASK_ONCE, &prepare, &gTaskMgr, true);
Task gTaskMeasureSolar(1000, TASK_ONCE, &measure_solar, &gTaskMgr, false);
Task gTaskMeasureLight(1000, TASK_ONCE, &measure_light, &gTaskMgr, false);
Task gTaskPrint(1000, TASK_ONCE, &print_volts, &gTaskMgr, false);

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
  //analogReference(INTERNAL);
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
  gVoltsSolar = .0;
  for (int i = 0; i < 5; i++)
  {
    gRawSolar = analogRead(gSolarPin);
    gVoltsSolar += float(gRawSolar) * gVoltsRef / 1024.0;
  }
  gVoltsSolar /= 5.0;
  gVoltsSolar *= (80.5 + 54.0) / 54.0;
  gTaskMeasureLight.restartDelayed(100);
}

void measure_light()
{
  gRawLight = analogRead(gLightPin);
  gVoltsLight = float(gRawLight) * gVoltsRef / 1024.0;
  gTaskPrint.restartDelayed(100);
}

void print_volts()
{
  Serial.print("Solar: ");
  Serial.print(gRawSolar);
  Serial.print(" ADC ");
  Serial.print(gVoltsSolar);
  Serial.print(" V; ");
  Serial.print("Light: ");
  Serial.print(gRawLight);
  Serial.print(" ADC ");
  Serial.print(gVoltsLight);
  Serial.println(" V");
  gTaskMeasureSolar.restartDelayed(1000);
}

void prepare()
{
  //gVoltsRef = readVcc() / 1000.0;
  gVoltsRef = 1.1;
  Serial.print("Ref: ");
  Serial.print(gVoltsRef);
  Serial.println(" V");
  analogReference(INTERNAL);
  gTaskMeasureSolar.restartDelayed(100);
}
