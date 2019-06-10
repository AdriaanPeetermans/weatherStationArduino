// ESP8266 WiFi <-> UART Bridge
// by RoboRemo
// www.roboremo.com

// Disclaimer: Don't use RoboRemo for life support systems
// or any other situations where system failure may affect
// user or environmental safety.

#include <ESP8266WiFi.h>


// config: ////////////////////////////////////////////////////////////

#define UART_BAUD 9600
#define packTimeout 5 // ms (if nothing more on UART, then send packet)
#define bufferSize 256

//#define MODE_AP // phone connects directly to ESP
#define MODE_STA // ESP connects to WiFi router

#define PROTOCOL_TCP
//#define PROTOCOL_UDP


#ifdef MODE_STA
// For STATION mode:
const char *ssid = "WiFi-P2";  // Your ROUTER SSID
const char *pw = "Minoutje0987"; // and WiFi PASSWORD
const int port = 9876;
// You must connect the phone to the same router,
// Then somehow find the IP that the ESP got from router, then:
// menu -> connect -> Internet(TCP) -> [ESP_IP]:9876
#endif

//////////////////////////////////////////////////////////////////////////




#ifdef PROTOCOL_TCP
#include <WiFiClient.h>
WiFiServer server(port);
WiFiClient client;
#endif

#ifdef PROTOCOL_UDP
#include <WiFiUdp.h>
WiFiUDP udp;
IPAddress remoteIp;
#endif


uint8_t buf1[bufferSize];
uint16_t i1=0;

uint8_t buf2[bufferSize];
uint16_t i2=0;



void setup() {

  delay(500);
  
  Serial.begin(UART_BAUD);
  delay(1000);

  
  #ifdef MODE_STA
  // STATION mode (ESP connects to router and gets an IP)
  // Assuming phone is also connected to that router
  // from RoboRemo you must connect to the IP of the ESP
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  delay(1000);
  Serial.print("000 ");
  Serial.println(WiFi.localIP());
  #endif

  #ifdef PROTOCOL_TCP
  server.begin(); // start TCP server 
  #endif
}


void loop() {

  if(!client.connected()) { // if client not connected
    client = server.available(); // wait for it to connect
    return;
  }

  // here we have a connected client

  if(client.available()) {
    while(client.available()) {
      uint8_t recv = (uint8_t) client.read();
      buf1[i1] = recv;
      yield();
      if(i1 == bufferSize-1) {
        Serial.write(buf1, bufferSize);
        i1 = 0;
      }
      else {
        i1 ++;
      }
    }
    // now send rest to UART:
    Serial.write(buf1, i1);
    i1 = 0;
  }

  if(Serial.available()) {

    // read the data until pause:
    
    while(1) {
      if(Serial.available()) {
        buf2[i2] = (char) Serial.read(); // read char from UART
        if(i2<bufferSize-1) {
          i2++;
        }
        else {
          client.write((char*)buf2, i2+1);
          i2 = 0;
        }
      }
      else {
        //delayMicroseconds(packTimeoutMicros);
        int i = 0;
        while (i < 200) {
          delay(packTimeout);
          if(Serial.available()) {
            break;
          }
          i++;
        }
        if (i == 200) {
          break;
        }
      }
    }
    
    // now send to WiFi:
    client.write((char*)buf2, i2);
    i2 = 0;
    client.stop();
  }
}
