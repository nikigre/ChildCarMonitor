#include "DHT.h"

#define DEBUG true

//Variables for setting up the child car monitor
#if DEBUG == true
  #define TimerDelay 1000 * 2 // check sensors at X seconds
  #define WarningTemp 25 //25 °C is too much
  #define TempertureAlarm 10 //how many times TimerDelay must pass that we signal alarm
  #define MaxWarningTempCounter 2 * TempertureAlarm //MAX temp counter
#else
  #define TimerDelay 1000 * 10 // check sensors at X seconds
  #define WarningTemp 35 //35 °C is too much
  #define TempertureAlarm 30 //how many times TimerDelay must pass that we signal alarm
  #define MaxWarningTempCounter 2 * TempertureAlarm //MAX temp counter
#endif


//Temperature sensor parameters
#define DHTPIN 6
#define DHTTYPE DHT11

//Buzzer parameters
#define BuzzerPin 7

//Reset button parameters
#define ResetButton 2

DHT dht(DHTPIN, DHTTYPE);

char buff[100]; //For sprintf method
char temperatureString[6]; //For displaying temperature as string

int timer; //Timer that counts loop()
int warningTempCounter = 0; //How many times the temperature was above limit


void setup() {
  Serial.begin(9600);
  
  pinMode(BuzzerPin, OUTPUT);
  pinMode(ResetButton, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(ResetButton), resetData, CHANGE); //We make an interrupt on pin so we can restart all parameters on click
  
  dht.begin();
}

void loop() {
  sprintf(buff, "%d\twarningTempCounter > TempertureAlarm \t %d > %d", timer, warningTempCounter, TempertureAlarm);
  Serial.println(buff);
  if(warningTempCounter > TempertureAlarm) { //Check if we need to produse an alarm
    alarm();
  } else {
    digitalWrite(BuzzerPin, LOW);
  }

  float temperature = getTemperature();
  
  sprintf(buff, "%d\ttemperature >= WarningTemp \t %s >= %d", timer, temperatureString, WarningTemp);
  Serial.println(buff);
  if(temperature >= WarningTemp) { //Check if temperature is above alowed limit
    if(warningTempCounter != MaxWarningTempCounter)
      warningTempCounter++;
  } else {
    if(warningTempCounter > 0)
      warningTempCounter--;
  }

  //Delay checking
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
