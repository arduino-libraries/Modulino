#include "Modulino.h"

ModulinoAmpere ampere;

void setup() {
  Serial.begin(9600);
  Modulino.begin();
  ampere.begin();
}

void loop() {
   if (ampere.available()) {
      Serial.print("Voltage: ");
      Serial.print(ampere.getVoltage(), 3);
      Serial.print(" V ");

      Serial.print(" Current: ");
      Serial.print(ampere.getCurrent()*1000, 3);
      Serial.print(" mA ");

      Serial.print("Power: ");
      Serial.print(ampere.getPower(), 8);
      Serial.print(" W\n");
  }
  delay(500);
}