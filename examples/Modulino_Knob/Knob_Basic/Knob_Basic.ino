/*
 * Modulino Knob - Basic
 *
 * This example code is in the public domain.
 * Copyright (c) 2025 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#include <Arduino_Modulino.h>

// Create a ModulinoKnob object
ModulinoKnob knob;

void setup() {
  Serial.begin(9600);
  // Initialize Modulino I2C communication
  Modulino.begin();
  // Detect and connect to knob module
  knob.begin();
}

void loop(){
  // Get the current position value of the knob
  int position = knob.get();
  // Check if the knob has been pressed (clicked)
  bool click = knob.isPressed();

  Serial.print("Current position is: ");
  Serial.println(position);

  if(click){
    Serial.println("Clicked!");
  }

}
