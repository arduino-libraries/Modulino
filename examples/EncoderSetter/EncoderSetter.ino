#include "Modulino.h"

ModulinoKnob encoder;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Modulino.begin();
  encoder.begin();
}

void loop() {
  int value = encoder.get();
  if (value > 7 || value < 0) {
    encoder.set(0);
  }
  Serial.println(value);
}
