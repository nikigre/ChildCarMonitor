#include "DHT.h"

#define DEBUG true

#if DEBUG == true
  #define TimerDelay 1000 * 2 // seconds
  #define WarningTemp 25 //25 °C is too much
  #define TempertureAlarm 10 //how many times TimerDelay must pass that we signal alarm
  #define MaxWarningTempCounter 2 * TempertureAlarm //MAX temp counter
#else
  #define TimerDelay 1000 * 10 // seconds
  #define WarningTemp 35 //35 °C is too much
  #define TempertureAlarm 30 //how many times TimerDelay must pass that we signal alarm
  #define MaxWarningTempCounter 2 * TempertureAlarm //MAX temp counter
#endif


#define DHTPIN 6
#define DHTTYPE DHT11

#define BuzzerPin 7

#define ResetButton 2

DHT dht(DHTPIN, DHTTYPE);

char buff[100];
char temperatureString[6];

int timer;
int warningTempCounter = 0;


void setup() {
  Serial.begin(9600);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(ResetButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ResetButton), resetData, CHANGE);
  
  dht.begin();
}

void loop() {
  sprintf(buff, "%d\twarningTempCounter > TempertureAlarm \t %d > %d", timer, warningTempCounter, TempertureAlarm);
  Serial.println(buff);
  if(warningTempCounter > TempertureAlarm) {
    alarm();
  } else {
    digitalWrite(BuzzerPin, LOW);
  }

  float temperature = getTemperature();
  
  sprintf(buff, "%d\ttemperature >= WarningTemp \t %s >= %d", timer, temperatureString, WarningTemp);
  Serial.println(buff);
  if(temperature >= WarningTemp) {
    if(warningTempCounter != MaxWarningTempCounter)
      warningTempCounter++;
  } else {
    if(warningTempCounter > 0)
      warningTempCounter--;
  }
  
  delay(TimerDelay);
  timer++;
  Serial.println(" ");
}


void alarm() {
  sprintf(buff, "%d\tALARM!!!", timer);
  Serial.println(buff);
  digitalWrite(BuzzerPin, HIGH);
}

float getTemperature() {
  float temperature = dht.readTemperature();

  if(isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return -1;
  } else {
    dtostrf(temperature, 4, 2, temperatureString);
    sprintf(buff, "%d\t %s °C", timer, temperatureString);
    Serial.println(buff);
  }
  return temperature;
}

void resetData()
{
  warningTempCounter=0;
  digitalWrite(BuzzerPin, LOW);
}
