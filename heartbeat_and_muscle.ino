#include <Wire.h>

//heartbeat
unsigned long lastBeat = 0;


// muscle
int max_analog_dta = 300;   // max analog data
int min_analog_dta = 100;   // min analog data
int static_analog_dta = 0;  // static analog data


// get analog value
int getAnalog(int pin) {
  long sum = 0;

  for (int i = 0; i < 32; i++) {
    sum += analogRead(pin);
  }

  int dta = sum >> 5;

  max_analog_dta = dta > max_analog_dta ? dta : max_analog_dta;  // if max data
  min_analog_dta = min_analog_dta > dta ? dta : min_analog_dta;  // if min data

  return sum >> 5;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  //muscle
  pinMode(A0, INPUT);

  long sum = 0;

  for (int i = 0; i <= 10; i++) {
    for (int j = 0; j < 100; j++) {
      sum += getAnalog(A0);
      delay(1);
    }

    //bar.setLevel(10-i);
  }

  sum /= 1100;

  static_analog_dta = sum;

  Serial.print("static_analog_dta = ");
  Serial.println(static_analog_dta);
}

int level = 5;
int level_buf = 5;

void loop() {
  // heartbeat
  unsigned char bpm = 0;

  Wire.requestFrom(0xA0 >> 1, 1);  // request 1 bytes from slave device
  while (Wire.available()) {       // slave may send less than requested
    bpm = Wire.read();             // receive heart rate value (a byte)
  }
  delay(100);

  unsigned long interval = 60000 / bpm;  // milliseconds / bpm calculates time between beats

  if (millis() - lastBeat >= interval) {  // if time has passed >= interval it gives a beat
    lastBeat = millis();
    // Serial.print("heartbeat: ");
    Serial.print(1);  // sends 1 to touchdesigner to indicate beat
    Serial.print(" ");
  } else {
    // Serial.print("heartbeat: ");
    Serial.print(0);
    Serial.print(" ");
  }

  //muscle
  int val = getAnalog(A0);  // get Analog value

  int level2;

  if (val > static_analog_dta)  // larger than static_analog_dta
  {
    level2 = 5 + map(val, static_analog_dta, max_analog_dta, 0, 5);
  } else {
    level2 = 5 - map(val, min_analog_dta, static_analog_dta, 0, 5);
  }

  // to smooth the change of led bar
  if (level2 > level) {
    level++;
  } else if (level2 < level) {
    level--;
  }

  if (level != level_buf) {
    level_buf = level;
    //bar.setLevel(level);
    // Serial.print("muscle: ");
    Serial.print(level);
  }

  Serial.println(" ");

  delay(100);
}
