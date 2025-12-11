#include <Wire.h>

// -------------------------------------------
// HEARTBEAT
// -------------------------------------------
unsigned long lastBeat = 0;

// -------------------------------------------
// EMG SETTINGS
// -------------------------------------------
int baseline = 0;              // rustwaarde van EMG 
const int EMG_PIN = A0;
const int NOISE_THRESHOLD = 10;   // onder deze waarde → ruis wordt genegeerd
const int MAX_EMG = 200;          // maximale nuttige amplitude

float smoothLevel = 0;            // smooth filter 


// -------------------------------------------
// Gemiddelde van 32 samples lezen
// -------------------------------------------
int readEMG() {
  long sum = 0;
  for (int i = 0; i < 32; i++) {
    sum += analogRead(EMG_PIN);
  }
  return sum >> 5;
}


// -------------------------------------------
// Calibratie: EMG baseline tijdens rust meten
// -------------------------------------------
void calibrateEMG() {
  long sum = 0;

  Serial.println("Calibrating EMG... Stay relaxed (2 sec)");

  for (int i = 0; i < 200; i++) {
    sum += analogRead(EMG_PIN);
    delay(10);
  }

  baseline = sum / 200;

  Serial.print("Baseline EMG = ");
  Serial.println(baseline);
}


// -------------------------------------------
// SETUP
// -------------------------------------------
void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(EMG_PIN, INPUT);

  calibrateEMG();
}


// -------------------------------------------
// LOOP
// -------------------------------------------
void loop() {

  // ---------------- HEARTBEAT ----------------
  unsigned char bpm = 0;

  Wire.requestFrom(0xA0 >> 1, 1);  
  while (Wire.available()) {
    bpm = Wire.read();
  }

  delay(10);

  unsigned long interval = (bpm > 0) ? (60000UL / bpm) : 1000;

  int heartbeatFlag = 0;

  if (millis() - lastBeat >= interval) {
    lastBeat = millis();
    heartbeatFlag = 1;   // beat!
  }

  // --------------- EMG -----------------------

  int raw = readEMG();
  int diff = raw - baseline;

  // ruis eruit
  if (diff < 0) diff = 0;
  if (diff < NOISE_THRESHOLD) diff = 0;
  if (diff > MAX_EMG) diff = MAX_EMG;

  // naar 0-10 schaal
  int level = map(diff, 0, MAX_EMG, 0, 10);

  // smoothing (low-pass)
  smoothLevel = (0.8 * smoothLevel) + (0.2 * level);

  int emgLevel = (int)smoothLevel;


  // ------------- SERIAL OUTPUT ---------------
  // structuur:  heartbeat  emgLevel
  Serial.print(heartbeatFlag);
  Serial.print(" ");
  Serial.println(emgLevel);

  delay(10);
}
