#include "Modulino.h"

Buttons buttons;
Tone buzzer;
LEDS leds;
Encoder encoder;
Distance distance;

void setup() {

  Serial.begin(115200);
  Modulino.begin();

  color.begin();
  distance.begin();

  buttons.begin();
  encoder.begin();
  buzzer.begin();
  leds.begin();

  imu.begin();
  imu.setContinuousMode();
  barometer.begin();
  //humidity.begin();
}

int skip = 0;
int pitch = 0;
bool a = false;
bool b = false;
bool c = false;

void loop() {

  float x;
  float y;
  float z;

  if (encoder.pressed()) {
    skip = (skip + 1) % 5;
  }

  pitch = encoder.get() + distance.get();

  if (Serial.available() && Serial.read() == 's') {
    imu.readAcceleration(x, y, z);
    Serial.print("IMU: x ");
    Serial.print(x, 3);
    Serial.print("\ty ");
    Serial.print(y, 3);
    Serial.print("\tz ");
    Serial.println(z, 3);

    Serial.print("Pressure: " + String(barometer.readPressure()));
    Serial.println("\tTemperature: " + String(barometer.readTemperature()));
  }

  //Serial.print("Humidity: " + String(humidity.readHumidity()));
  //Serial.println("\tTemperature: " + String(humidity.readTemperature()));

  if (color.available()) {
    int r;
    int g;
    int b;
    color.read(r, g, b);
    leds.set(4 + skip, r, g, b, 255);
    leds.show();
  }

  if (buttons.get(a, b, c)) {
    if (a) {
      leds.set(1 + skip, RED, 15);
      buzzer.tone(440 + pitch, 1000);
    } else {
      leds.clear(1 + skip);
    }
    if (b) {
      leds.set(2 + skip, BLUE, 15);
      buzzer.tone(880 + pitch, 1000);
    } else {
      leds.clear(2 + skip);
    }
    if (c) {
      leds.set(3 + skip, GREEN, 15);
      buzzer.tone(1240 + pitch, 1000);
    } else {
      leds.clear(3 + skip);
    }
    leds.show();
  }
}
