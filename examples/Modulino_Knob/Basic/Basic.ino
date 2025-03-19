/*
 * Modulino Knob - Basic
 *
 * This example code is in the public domain. 
 * Copyright (c) 2025 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#include <Modulino.h>

ModulinoKnob knob;

void setup() {
  Serial.begin(9600);
  Modulino.begin();
  knob.begin();
}

void loop(){
  int position = knob.get();
  bool click = knob.isPressed();

  Serial.print("Current position is: ");
  Serial.println(position);

  if(click){
    Serial.println("Clicked!");
  }

}