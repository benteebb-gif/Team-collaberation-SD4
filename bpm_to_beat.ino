#include <Wire.h>

unsigned long lastBeat = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();
}

void loop() {
  unsigned char bpm = 0;

  Wire.requestFrom(0xA0 >> 1, 1);  // request 1 bytes from slave device
  while (Wire.available()) {       // slave may send less than requested
    bpm = Wire.read();             // receive heart rate value (a byte)
  }
  delay(100);

  unsigned long interval = 60000 / bpm;  // milliseconds / bpm calculates time between beats

  if (millis() - lastBeat >= interval) {  // if time has passed >= interval it gives a beat
    lastBeat = millis();
    Serial.println(1);  // sends 1 to touchdesigner to indicate beat
  } else {
    Serial.println(0);
  }
}
