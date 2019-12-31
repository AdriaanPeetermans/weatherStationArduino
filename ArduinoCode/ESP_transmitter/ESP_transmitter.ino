#include <ESP8266WiFi.h>

const char* ssid     = "WiFi-P2";      // SSID
const char* password = "Minoutje0987";      // Password
const char* host = "192.168.1.59";  // IP serveur - Server IP
const int   port = 9876;            // Port serveur - Server Port

void setup() {
  // Initialize serial.
  Serial.begin(4800);
  Serial.setTimeout(5000);

  // Connect to WiFi.
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Wait for serial and notify data generator.
  while (!Serial) {}
  Serial.println("Awake");

  // Receive data from serial.
  while (Serial.available() <= 0) {
    delay(500);
  }
  String mess = Serial.readStringUntil('\r');

  // Connect to server.
  WiFiClient client;
  if (!client.connect(host, port)) {
      Serial.println("Connection Failed");
      return;
  }
  client.print(mess + "\r\n");

  // Wait for answer.
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 10000) {
      Serial.println("Client Timeout");
      client.stop();
      return;
    }
  }

  // Receive answer.
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  client.stop();
  delay(100);
  // Back to sleep.
  ESP.deepSleep(3600e6); //Max 1 hour of sleep.
}

void loop() {
}
