#include "Arduino.h"
#include <Wire.h>
#include "GPS_Air530Z.h"
#include "HT_SSD1306Wire.h"

// Display setup: I2C address, frequency, SDA, SCL, display geometry, reset pin
SSD1306Wire display(0x3c, 500000, SDA, SCL, GEOMETRY_128_64, GPIO10);

// GPS module object
Air530ZClass GPS;

void VextON(void) {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW); // Vext ON
}

void VextOFF(void) {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH); // Vext OFF
}

void setup() {
  VextON();
  delay(10);

  display.init();
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_16);
  display.drawString(64, 32 - 8, "GPS initializing...");
  display.display();

  Serial.begin(921600);
  GPS.begin(115200);
}

void loop() {
  // Read GPS data for 1 second
  uint32_t starttime = millis();
  while ((millis() - starttime) < 1000) {
    while (GPS.available() > 0) {
      GPS.encode(GPS.read());
    }
  }

  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);

  char buffer[32];

  // Check if date and time are valid
  if (GPS.date.isValid() && GPS.time.isValid()) {
    sprintf(buffer, "%02d-%02d-%04d", GPS.date.day(), GPS.date.month(), GPS.date.year());
    display.drawString(0, 0, buffer);

    sprintf(buffer, "%02d:%02d:%02d", GPS.time.hour(), GPS.time.minute(), GPS.time.second());
    display.drawString(60, 0, buffer);
  } else {
    display.drawString(0, 0, "Date/Time Invalid");
  }

  // Fix status
  if (GPS.location.isValid()) {
    display.drawString(120, 0, "✔");  // Valid fix
  } else {
    display.drawString(120, 0, "✖");  // No fix
  }

  // Altitude
  if (GPS.altitude.isValid()) {
    dtostrf(GPS.altitude.meters(), 6, 2, buffer);
    display.drawString(0, 16, String("Alt: ") + buffer + " m");
  } else {
    display.drawString(0, 16, "Alt: --");
  }

  // HDOP
  if (GPS.hdop.isValid()) {
    dtostrf(GPS.hdop.hdop(), 4, 2, buffer);
    display.drawString(0, 32, String("HDOP: ") + buffer);
  } else {
    display.drawString(0, 32, "HDOP: --");
  }

  // Latitude
  if (GPS.location.isValid()) {
    dtostrf(GPS.location.lat(), 10, 6, buffer);
    display.drawString(60, 16, String("Lat: ") + buffer);

    dtostrf(GPS.location.lng(), 10, 6, buffer);
    display.drawString(60, 32, String("Lon: ") + buffer);
  } else {
    display.drawString(60, 16, "Lat: --");
    display.drawString(60, 32, "Lon: --");
  }

  // Speed
  if (GPS.speed.isValid()) {
    dtostrf(GPS.speed.kmph(), 6, 2, buffer);
    display.drawString(0, 48, String("Speed: ") + buffer + " km/h");
  } else {
    display.drawString(0, 48, "Speed: --");
  }

  display.display();

  // Optional: Print debug info to serial
  Serial.println("-------------");
  if (GPS.location.isValid()) {
    Serial.printf("Lat: %.6f, Lon: %.6f\n", GPS.location.lat(), GPS.location.lng());
  } else {
    Serial.println("Location invalid");
  }
  if (GPS.time.isValid()) {
    Serial.printf("Time: %02d:%02d:%02d\n", GPS.time.hour(), GPS.time.minute(), GPS.time.second());
  }
  if (GPS.date.isValid()) {
    Serial.printf("Date: %02d-%02d-%04d\n", GPS.date.day(), GPS.date.month(), GPS.date.year());
  }
  Serial.println("-------------");
}
