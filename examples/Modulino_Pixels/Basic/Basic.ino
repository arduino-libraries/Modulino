#include <Modulino.h>

ModulinoPixels leds;

int brightness = 25;

void setup(){
  Modulino.begin();
  leds.begin();
}

void loop(){
  //Set all LEDs blue
  for (int i = 0; i < 8; i++) {
    leds.set(i, BLUE, brightness);
    leds.show();
  }
}