/*
  FDC1004 - basic

  This example reads data from Modulino Farad
  With this settings you can read channel 1.
  Offset and gain are set to Zero.


  This example code is in the public domain.
*/
#include    <Wire.h>
#include "Modulino.h"

uint16_t reg_read;
int measured_cap;
ModulinoFarad farad;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  Modulino.begin();
  farad.begin();
  //Offset settings - Value set to Zero
  farad.channelOffset(CHANNEL1,0x00);
  //Channel settings - Channel 1 will measure the capacitance on input CIN1sa and the offset is set to Zero 
  farad.channelSettings(CHANNEL1,CIN1,CAPDAC,0x00);
  //Measure settings - Channel 1 enabled, sample rate is 100 Samples/sec and measure is repeated
  farad.measurementSettings(MEAS_1_EN,RATE_100Ss,REPEAT_ENABLED);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (farad.measureAvailable(CHANNEL1)) 
  {     
    measured_cap = farad.getMeasure(CHANNEL1);
    Serial.print("The Value of the measured Capacitance is ");
    Serial.println(measured_cap);
  } 
  delay(200);
}
