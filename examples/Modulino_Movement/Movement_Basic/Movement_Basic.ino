/*
 * Modulino Movement - Simple Serial Output
 *
 * This example code is in the public domain. 
 * Copyright (c) 2025 Arduino
 * SPDX-License-Identifier: MPL-2.0
 */

#include "Modulino.h"

// Create a ModulinoMovement
ModulinoMovement movement;

float x, y, z;
float gx, gy, gz;

void setup() {
  Serial.begin(9600);
  // Initialize Modulino I2C communication
  Modulino.begin();
  // Detect and connect to movement sensor module
  movement.begin();
  
}

void loop() {
  // Read new movement data from the sensor
  movement.update();

  // Get acceleration and gyroscope values
  x = movement.getX();
  y = movement.getY();
  z = movement.getZ();
  gx = movement.getGyroX();
  gy = movement.getGyroY();
  gz = movement.getGyroZ();

  // Print acceleration values
  Serial.print("A: ");
  Serial.print(x, 3);
  Serial.print(", ");
  Serial.print(y, 3);
  Serial.print(", ");
  Serial.print(z, 3);
  
  // Print divider between acceleration and gyroscope
  Serial.print(" | G: ");
  
  // Print gyroscope values
  Serial.print(gx, 1);
  Serial.print(", ");
  Serial.print(gy, 1);
  Serial.print(", ");
  Serial.println(gz, 1);
  
  delay(200);
}