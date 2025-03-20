#include "Modulino.h"

ModulinoMovement movement;

float x;
float y;
float z;
float gx;
float gy;
float gz;

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

  gx = movement.getGyroX();
  gy = movement.getGyroY();
  gz = movement.getGyroZ();

  Serial.print("Movement data: ");

  Serial.print("x ");
  Serial.print(x,3);
  Serial.print("	y ");
  Serial.print(y,3);
  Serial.print("	z ");
  Serial.print(z,3);

  Serial.print("\t\t gx ");
  Serial.print(gx,1);
  Serial.print("	|gy ");
  Serial.print(gy,1);
  Serial.print("	|gz ");
  Serial.println(gz,1);
}