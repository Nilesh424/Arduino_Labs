#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <ArduinoJson.h>

// Choose two ardu pins to use for software serial
int RXPin = 3;
int TXPin = 2;
//choose two ardu pins for esp8266

int GPSBaud = 9600;
TinyGPSPlus gps;
int UTC_offset = 0;

SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
}

void loop() {
  StaticJsonDocument<256> doc;

  if (gpsSerial.available() > 0) {
    while (gpsSerial.available() > 0) {
      if (gps.encode(gpsSerial.read())) {
        setTime(gps.time.hour(), gps.time.minute(), gps.time.second(), gps.date.day(), gps.date.month(), gps.date.year());
        adjustTime(UTC_offset * SECS_PER_HOUR);

        if (gps.location.isValid()) {
          doc["latitude"] = gps.location.lat();
          doc["longitude"] = gps.location.lng();
        }

        if (gps.altitude.isValid()) {
          doc["altitude"] = gps.altitude.meters();
        }

        if (gps.speed.isValid()) {
          doc["speed"] = gps.speed.mph();
        }

        if (gps.satellites.isValid()) {
          doc["satellites"] = gps.satellites.value();
        }

        if (gps.hdop.isValid()) {
          doc["hdop"] = gps.hdop.value();
        }

        String json;
        serializeJson(doc, json);
        Serial.println(json);
      }
    }
  }
}
