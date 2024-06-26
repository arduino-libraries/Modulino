#include <Modulino.h>

// Create object instance
ModulinoThermo thermo;

void setup(){
  Serial.begin(9600);

  // Call all necessary .begin() function
  Modulino.begin();
  thermo.begin();
}

void loop(){

  float celsius = thermo.getTemperature();

  float fahrenheit = (celsius * 9 / 5) + 32;

  float humidity = thermo.getHumidity();

  Serial.print("Temperature (C) is: ");
  Serial.println(celsius);

  Serial.print("Temperature (F) is: ");
  Serial.println(fahrenheit);

  Serial.print("Humidity (rH) is: ");
  Serial.println(humidity);

}