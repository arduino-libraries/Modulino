#include "Modulino.h"

ModulinoButtons buttons;
ModulinoBuzzer buzzer;
ModulinoPixels leds;
ModulinoKnob encoder;
ModulinoDistance distance;
ModulinoMovement imu;
ModulinoPressure barometer;

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
  barometer.begin();
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
    imu.update();
    Serial.print("IMU: x ");
    Serial.print(imu.getX(), 3);
    Serial.print("\ty ");
    Serial.print(imu.getY(), 3);
    Serial.print("\tz ");
    Serial.println(imu.getZ(), 3);

    Serial.print("Pressure: " + String(barometer.getPressure()));
    Serial.println("\tTemperature: " + String(barometer.getTemperature()));
  }

  if (color.available()) {
    int r;
    int g;
    int b;
    color.read(r, g, b);
    leds.set(4 + skip, r, g, b, 255);
    leds.show();
  }

  if (buttons.update()) {
    if (buttons.isPressed(0)) {
      leds.set(1 + skip, RED, 15);
      buzzer.tone(440 + pitch, 1000);
    } else {
      leds.clear(1 + skip);
    }
    if (buttons.isPressed(1)) {
      leds.set(2 + skip, BLUE, 15);
      buzzer.tone(880 + pitch, 1000);
    } else {
      leds.clear(2 + skip);
    }
    if (buttons.isPressed(2)) {
      leds.set(3 + skip, GREEN, 15);
      buzzer.tone(1240 + pitch, 1000);
    } else {
      leds.clear(3 + skip);
    }
    leds.show();
  }
}
