#include "Modulino.h"

ModulinoKnob encoder;
ModulinoPixels leds;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Modulino.begin();
  encoder.begin();
  leds.begin();
}

void loop() {
  int value = encoder.get();
  if (value > 100 || value < 0) {
    encoder.set(0);
  }
  value = encoder.get();
  Serial.println(value);
  leds.set(1, RED, value);
  leds.set(2, GREEN, value);
  leds.set(3, BLUE, value);
  leds.show();
}
