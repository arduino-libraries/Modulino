/*
 * Modulino Knob - Encoder Setter
 *
 * This example code is in the public domain. 
 * Copyright (c) 2025 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#include "Modulino.h"

ModulinoKnob encoder;
ModulinoPixels leds;
ModulinoButtons buttons;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Modulino.begin();
  encoder.begin();
  leds.begin();
  buttons.begin();
}

void loop() {
  int value = encoder.get();
  //Reset the position of the encoder with the set function
  if (value > 100 || value < 0) {
    encoder.set(0);
  }
  value = encoder.get();
  Serial.println(value);
  leds.set(1, RED, value);
  leds.set(2, GREEN, value);
  leds.set(3, BLUE, value);
  leds.show();
  buttons.update();
  buttons.setLeds(buttons.isPressed(0), buttons.isPressed(1), buttons.isPressed(2));
}
