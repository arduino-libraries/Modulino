/*
 * Modulino Buzzer - Simple melody
 *
 * This example code is in the public domain.
 * Copyright (c) 2025 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#include <Arduino_Modulino.h>

ModulinoBuzzer buzzer;

int melody[] = { 262, 196, 196, 220, 196, 0, 247, 262 };

void setup() {
  Modulino.begin();
  buzzer.begin();
}

void loop() {

  for (int i = 0; i < 8; i++) {
    int note = melody[i];

    buzzer.tone(note, 250);
    delay(250);

  }
}
