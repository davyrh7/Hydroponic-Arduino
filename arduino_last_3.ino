#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoJson.h>

#define TempData 10

int ECPin = A0;
int ECGround = A1;
int ECPower = A4;
int pHSensor = A2;
int pompa_nutrisi1 = 2;
int R1 = 510;
int Ra = 25;
int samples = 10;
int ppm = 0;
int pH = 0;

const int TempPower = 8;
const int TempGnd = 9;

float PPMconversion = 0.7;
float TemperatureCoef = 0.019;
float K = 2.4;
float adc_resolution = 1024.0;
float Temperature = 10;
float EC = 0;
float EC25 = 0;
float raw = 0;
float Vin = 5;
float Vdrop = 0;
float Rc = 0;
float buffer = 0;

String values;

OneWire oneWire(TempData);
DallasTemperature sensors(&oneWire);

 void setup() { 
  Serial.begin(9600);

  pinMode(TempGnd, OUTPUT );
  digitalWrite(TempGnd, LOW );
  pinMode(TempPower, OUTPUT );
  digitalWrite(TempPower, HIGH );
  pinMode(ECPin,INPUT);
  pinMode(ECPower, OUTPUT);
  pinMode(ECGround, OUTPUT);
  digitalWrite(ECGround, LOW);
  digitalWrite(ECPower, LOW);
  pinMode(pompa_nutrisi1, OUTPUT);
  delay(100);  

  sensors.begin();
  delay(100);

  R1 = (R1+Ra);
}

 
void loop() {
  values = (pH_value()+','+ ppm_value());
  delay(1000); 
  Serial.flush();
  delay(1000);
  Serial.print(values); 
  delay(1000);
}

int ph(int voltage){
  return 2.59 + ((2.5 - voltage)/0.18);
}

String pH_value(){
  
  int measurings=0;
  
  for (int i = 0; i < samples; i++)
  {
    measurings += analogRead(pHSensor);
    delay(10);
    }
    int voltage = 5/adc_resolution * measurings/samples;

    delay(5000);

    return String(ph(voltage));
}

String ppm_value(){
  sensors.requestTemperatures();
  Temperature = sensors.getTempCByIndex(0);

  digitalWrite(ECPower, HIGH);
  raw = analogRead(ECPin);
  raw = analogRead(ECPin);
  digitalWrite(ECPower, LOW);

  Vdrop = (Vin*raw)/1024.0;
  Rc = (Vdrop*R1)/(Vin-Vdrop);
  Rc = Rc-Ra;
  EC = 1000/(Rc*K);
  EC25 = EC/ (1+ TemperatureCoef*(Temperature-25.0));
  ppm = (EC25)*(PPMconversion*1000);

  delay(5000);

  if(ppm >= 1200){
    digitalWrite(pompa_nutrisi1, LOW);
  }
  if(ppm <= 900){
    digitalWrite(pompa_nutrisi1, HIGH);
  }
  
  return String(ppm);
}
