
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#define UTC_offset -7  // PDT
TinyGPSPlus tinyGPS;


String logFileName = "";
int networks = 0;

#define LOG_RATE 500
char currentTime[5];
        
SoftwareSerial ss(D1, D2); // RX, TX
StaticJsonDocument<1024> jsonBuffer; // create a JSON buffer

void lookForNetworks() {
  jsonBuffer.clear(); // clear the JSON buffer
  int n = WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("no networks found");
  }
  else {
    JsonArray networkArray = jsonBuffer.createNestedArray("networks"); // create a JSON array for network data

    for (int i = 0; i < n; ++i) {
      if ((WiFi.channel(i) > 0) && (WiFi.channel(i) < 15)) {
        networks++;

        JsonObject networkObject = networkArray.createNestedObject(); // create a JSON object for each network
        
        networkObject["bssid"] = WiFi.BSSIDstr(i);
        networkObject["ssid"] = WiFi.SSID(i);
        networkObject["ssid_short"] = WiFi.SSID(i).substring(0, 18);
        networkObject["rssi"] = WiFi.RSSI(i);
        networkObject["encryption"] = getEncryption(i, "");
        networkObject["channel"] = WiFi.channel(i);
        
        if (getEncryption(i,"") == "[WEP][ESS]") {  // flash if WEP detected
          networkObject["wep_detected"] = true;
        } else {
          networkObject["wep_detected"] = false;
        }
      }
    }
  }

  serializeJson(jsonBuffer, Serial); // serialize the JSON data and print it to the serial port
  Serial.println(); // add a new line after the JSON output
}
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available())
      tinyGPS.encode(ss.read());
  } while (millis() - start < ms);
}

String getEncryption(uint8_t network, String src) { // return encryption for WiGLE or print
  byte encryption = WiFi.encryptionType(network);
  switch (encryption) {
    case 2:
      if (src=="screen") { return "WPA"; }
      return "[WPA-PSK-CCMP+TKIP][ESS]";
    case 5:
      if (src=="screen") { return "WEP"; }
      return "[WEP][ESS]";
    case 4:
      if (src=="screen") { return "WPA2"; }
      return "[WPA2-PSK-CCMP+TKIP][ESS]";
    case 7:
      if (src=="screen") { return "NONE" ; }
      return "[ESS]";
  }
  if (src=="screen") { return "AUTO"; }
  return "[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][ESS]";      
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA); WiFi.disconnect();
  Serial.println("* ESP8266 WarDriver *\n");
  delay(500);
  Serial.println();
}

void loop() {
  lookForNetworks();
}


