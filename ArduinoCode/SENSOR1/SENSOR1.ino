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

#define LightPin 7
#define VCCPin 9
#define ESPReset 7
#define BattNeg A0
#define BattPos A1
#define PanelNeg A2
#define PanelPos A3

// using I2C
Adafruit_BMP280 bme;
DHT dht(DHTPIN, DHTTYPE);

volatile int f_wdt=1;

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
  if (!bme.begin())
  {  
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
  dht.begin();
  pinMode(VCCPin, OUTPUT); 
  digitalWrite(VCCPin, LOW);
  pinMode(ESPReset, OUTPUT);
  digitalWrite(ESPReset, HIGH);
  pinMode(BattNeg, INPUT);
  pinMode(BattPos, INPUT);
  pinMode(PanelNeg, INPUT);
  pinMode(PanelPos, INPUT);

  /* For the deep sleep*/
  MCUSR &= ~(1<<WDRF);
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR = 1<<WDP0 | 1<<WDP3;
  WDTCSR |= _BV(WDIE);
}
  
void loop()
{
  if(f_wdt == 1) {
    digitalWrite(VCCPin, HIGH); // Power sensors.
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

    // Send data.
//    String s = String(analogRead(LightPin));
//    Serial.print("SENSOR1 LI" + String(s.length()) + " " + s);
//    s = String(dht.readTemperature());
//    Serial.print(" T1" + String(s.length()) + " " + s);
//    s = String(dht.readHumidity());
//    Serial.print(" HU" + String(s.length()) + " " + s);
//    s = String(bme.readTemperature());
//    Serial.print(" T2" + String(s.length()) + " " + s);
//    s = String(bme.readPressure());
//    Serial.print(" PR" + String(s.length()) + " " + s);
//    s = String(analogRead(BattNeg));
//    Serial.print(" BN" + String(s.length()) + " " + s);
//    s = String(analogRead(BattPos));
//    Serial.print(" BP" + String(s.length()) + " " + s);
//    s = String(analogRead(PanelNeg));
//    Serial.print(" PN" + String(s.length()) + " " + s);
//    s = String(analogRead(PanelPos));
//    Serial.println(" PP" + String(s.length()) + " " + s);

    
    String sendMes = "100 ";
    String s = String(analogRead(LightPin));
    if (s.length() < 2) {
      sendMes = sendMes + "0";
    }
    if (s.length() < 3) {
      sendMes = sendMes + "0";
    }
    if (s.length() < 4) {
      sendMes = sendMes + "0";
    }
    sendMes = sendMes + s + ",";
    float t = dht.readTemperature();
    s = String(t).substring(0,4);
    if (t < 10) {
      s =  "0" + s;
    }
    else {
      s = s + "0";
    }
    sendMes = sendMes + s + ",";
    s = String(dht.readHumidity()).substring(0,4);
    sendMes = sendMes + s + "0,";
    t = bme.readTemperature();
    s = String(t);
    if (t < 10) {
      s = "0" + s;
    }
    sendMes = sendMes + s + ",";
    s = String(bme.readPressure()).substring(0,9);
    sendMes = sendMes + s.substring(0,4) + "." + s.substring(4,6) + s.substring(7,9) + ",";
    s = String(analogRead(PanelPos)/1024.0*5).substring(0,4);
    sendMes = sendMes + s + ",";
    s = String(analogRead(BattPos)/1024.0*5).substring(0,4);
    sendMes = sendMes + s;
    Serial.println(sendMes);

    // Wait for answer.
    mess = "";
    timeout = millis();
    while (mess.length() <= 1) {
      if (millis() - timeout > 10000) {
        return;
      }
      mess = Serial.readStringUntil('\r');
      delay(500);
    }

    // Rerun if failed.
    if (mess.indexOf("B00") < 0) {
      return;
    }

    //Parse sleep length:
    mess = mess.substring(4);
    int minutes = mess.toInt();
    int sleepCycles = int(minutes*60/8.0);
    
//    Serial.print("----   Light    ----------------\n");
//    Serial.print("Measurement = ");
//    Serial.println(analogRead(LightPin));
//    Serial.print("--------------------------------\n\n");
//    
//    Serial.print("----   DHT 22   ----------------\n");
//    Serial.print("Temperature = ");
//    Serial.print(dht.readTemperature());
//    Serial.println(" *C");
//    Serial.print("Humidity = ");
//    Serial.print(dht.readHumidity());
//    Serial.println(" %");
//    Serial.print("--------------------------------\n\n");
//    
//    Serial.print("---- GY BMP 280 ----------------\n");
//    Serial.print("Temperature = ");
//    Serial.print(bme.readTemperature());
//    Serial.println(" *C");
//    Serial.print("Pressure = ");
//    Serial.print(bme.readPressure() / 100); // 100 Pa = 1 millibar
//    Serial.println(" mb");
//    Serial.print("Approx altitude = ");
//    Serial.print(bme.readAltitude(1013.25));
//    Serial.println(" m");
//    Serial.print("--------------------------------\n\n");
    digitalWrite(VCCPin, LOW);
    delay(100);

    /* Enter sleep*/
    for (int i = 0; i < sleepCycles; i++) {
      f_wdt = 0;
      enterSleep();
    }
  }
}
