#include "Modulino.h"

ModulinoDistance distance;

void setup() {
  Serial.begin(9600);
  Modulino.begin();
  distance.begin();
}

void loop() {
  if (distance.available()) {
    int measure = distance.get();
    Serial.println(measure);
  }
  delay(10);
}