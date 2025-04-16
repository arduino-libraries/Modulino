/*
 * Modulino Buttons - Basic
 *
 * This example code is in the public domain.
 * Copyright (c) 2025 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#include <Arduino_Modulino.h>

// Create a ModulinoButtons object
ModulinoButtons buttons;

bool button_a = false;
bool button_b = false;
bool button_c = false;

void setup() {
  Serial.begin(9600);
  // Initialize Modulino I2C communication
  Modulino.begin();
  // Detect and connect to buttons module
  buttons.begin();
  // Turn on the LEDs above buttons A, B, and C
  buttons.setLeds(true, true, true);
}
void loop() {
  // Check for new button events, returns true when button state changes
  if (buttons.update()) {
    // Check which button was pressed (0=A, 1=B, 2=C)
    if (buttons.isPressed(0)) {
      Serial.println("Button A pressed!");
    } else if (buttons.isPressed(1)) {
      Serial.println("Button B pressed!");
    } else if (buttons.isPressed(2)) {
      Serial.println("Button C pressed!");
    }
  }
}
