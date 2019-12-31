/*
 * GY BMP 280 test sketch
 * http://satujamsaja.blogspot.co.id
 * 
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include "DHT.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#define DHTPIN 2
#define DHTTYPE DHT22

#define LightPin A3
#define VCCPin 9
#define ESPReset 7
#define SOILTEMPVCCPin 8
#define M1 3
#define M2 4
#define SOILTEMP1 A6
#define SOILTEMP2 A7
#define SOILMOIST1 A4
#define SOILMOIST2 A5

// using I2C
//Adafruit_BMP280 bme;
DHT dht(DHTPIN, DHTTYPE);

volatile int f_wdt=1;

int numberTries = 0;
int maxNumberTries = 5;

//Alternating current direction of the soil moisture sensors:
bool soilMoistDir = true;
float R;
float alpha = 2.73;
float Rt = 1.0;

//Debuging:
//String debugText = "";

ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    //Serial.println("WDT Overrun!!!");
  }
}

void enterSleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}

  
void setup()
{
  Serial.begin(4800);
  
  dht.begin();
  pinMode(VCCPin, OUTPUT);
  digitalWrite(VCCPin, LOW);
  pinMode(ESPReset, OUTPUT);
  digitalWrite(ESPReset, HIGH);
  pinMode(SOILTEMPVCCPin, OUTPUT);
  digitalWrite(SOILTEMPVCCPin, LOW);

  pinMode(SOILTEMP1, INPUT);
  pinMode(SOILTEMP2, INPUT);
  pinMode(SOILMOIST1, INPUT);
  pinMode(SOILMOIST2, INPUT);

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);

  /* For the deep sleep*/
  MCUSR &= ~(1<<WDRF);
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = 1<<WDP0 | 1<<WDP3;
  WDTCSR |= _BV(WDIE);
}
  
void loop() {
  if(f_wdt == 1) {
    if (numberTries >= maxNumberTries) {
      int sleepCycles = 100;
      pinMode(VCCPin, OUTPUT);
      digitalWrite(VCCPin, LOW);
      digitalWrite(SOILTEMPVCCPin, LOW);
      digitalWrite(M1, LOW);
      digitalWrite(M2, LOW);
      delay(100);
      /* Enter sleep*/
      for (int i = 0; i < sleepCycles; i++) {
        f_wdt = 0;
        enterSleep();
      }
      numberTries = 0;
      return;
    }
    else {
      numberTries ++;
    }
    pinMode(VCCPin, OUTPUT);
    digitalWrite(VCCPin, HIGH); // Power sensors.
    digitalWrite(SOILTEMPVCCPin, HIGH);
    if (soilMoistDir) {
      digitalWrite(M1, HIGH);
      digitalWrite(M2, LOW);
      soilMoistDir = false;
    }
    else {
      digitalWrite(M2, HIGH);
      digitalWrite(M1, LOW);
      soilMoistDir = true;
    }
    digitalWrite(ESPReset, LOW); // Reset ESP.
    delay(500);
    digitalWrite(ESPReset, HIGH);
    delay(500);

    // Wait for "Awake".
    String mess = "123";
    unsigned long timeout = millis();
    while (mess.indexOf("Awake") < 0) {
      if (millis() - timeout > 3000) {
        return;
      }
      mess = Serial.readStringUntil('\r');
    }
    
    String sendMes = "200 ";    //200 light#temp#hum#soilTemp1#soilTemp2#soilMoist1#soilMoist2#Polarity
    String s = String(analogRead(LightPin));
    sendMes = sendMes + s + "#";
    float t = dht.readTemperature();
    s = String(t);
    sendMes = sendMes + s + "#";
    s = String(dht.readHumidity());
    sendMes = sendMes + s + "#";
    
//    t = bme.readTemperature();
//    s = String(t);
//    sendMes = sendMes + s + "#";
//    
//    s = String(bme.readPressure()/100);
//    sendMes = sendMes + s + "#";
    
    s = String(analogRead(SOILTEMP1)/1024.0*5);
    sendMes = sendMes + s + "#";
    s = String(analogRead(SOILTEMP2)/1024.0*5);
    sendMes = sendMes + s + "#";
    float moist = analogRead(SOILMOIST1)/1024.0*5;
    if (soilMoistDir) {
      if (moist == 0) {
        R = 1000000;
      }
      else {
        R = alpha*Rt*(5-moist)/((1+alpha)*moist);
      }
    }
    else {
      R = alpha*Rt*moist/(alpha*(5-moist)-moist);
    }
    s = String(R);
    sendMes = sendMes + s + "#";
    moist = analogRead(SOILMOIST2)/1024.0*5;
    if (soilMoistDir) {
      if (moist == 0) {
        R = 1000000;
      }
      else {
        R = alpha*Rt*(5-moist)/((1+alpha)*moist);
      }
    }
    else {
      R = alpha*Rt*moist/(alpha*(5-moist)-moist);
    }
    s = String(R);
    sendMes = sendMes + s + "#";
    s = String(soilMoistDir);
    sendMes = sendMes + s;
    //sendMes = sendMes + "#" + debugText;
    Serial.println(sendMes);

    // Wait for answer.
    mess = "";
    timeout = millis();
    while (mess.length() <= 1) {
      if (millis() - timeout > 20000) {
        //debugText = "Timout, tries: " + String(numberTries);
        return;
      }
      mess = Serial.readStringUntil('\r');
      delay(500);
    }

    // Rerun if failed.
    if (mess.indexOf("B00") < 0) {
      //debugText = "Wrong answer: " + mess + ", tries: " + String(numberTries);
      return;
    }

    numberTries = 0;
    
    //Parse sleep length:
    mess = mess.substring(4);
    int minutes = mess.toInt();
    int sleepCycles = int(minutes*60/8.0);

    pinMode(VCCPin, OUTPUT);
    digitalWrite(VCCPin, LOW);
    digitalWrite(SOILTEMPVCCPin, LOW);
    digitalWrite(M1, LOW);
    digitalWrite(M2, LOW);
    delay(100);

    /* Enter sleep*/
    for (int i = 0; i < sleepCycles; i++) {
      f_wdt = 0;
      enterSleep();
    }
  }
}
