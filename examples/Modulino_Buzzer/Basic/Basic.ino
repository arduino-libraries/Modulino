/*
 * Modulino Buzzer - Basic
 *
 * This example code is in the public domain.
 * Copyright (c) 2025 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#include <Arduino_Modulino.h>

ModulinoBuzzer buzzer;

int frequency = 440;
int duration = 1000;

void setup(){
  Modulino.begin();
  buzzer.begin();
}

void loop(){

  buzzer.tone(frequency, duration);
  delay(1000);
  buzzer.tone(0, duration);
  delay(1000);

}
