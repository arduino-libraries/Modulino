/*
 * Modulino Movement - Basic
 *
 * This example code is in the public domain. 
 * Copyright (c) 2025 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#include "Modulino.h"

ModulinoMovement movement;

float x;
float y;
float z;

void setup() {
  Serial.begin(9600);
  Modulino.begin();
  movement.begin();
}

void loop() {
  movement.update();

  x = movement.getX();
  y = movement.getY();
  z = movement.getZ();

  Serial.print("Movement data: ");
  Serial.print("x ");
  Serial.print(x, 3);
  Serial.print("	y ");
  Serial.print(y, 3);
  Serial.print("	z ");
  Serial.println(z, 3);
}