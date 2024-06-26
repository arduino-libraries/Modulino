#include <Modulino.h>

ModulinoButtons buttons;

bool button_a = false;
bool button_b = false;
bool button_c = false;

void setup() {
  Serial.begin(9600);
  Modulino.begin();
  buttons.begin();
  //function to control the LEDs on top of each button
  buttons.setLeds(true, true, true);
}
void loop() {
  //request new data from the Modulino buttons
  if (buttons.update()) {
    //Check if the buttons has been pressed
    if (buttons.isPressed(0)) {
      Serial.println("Button A pressed!");
    } else if (buttons.isPressed(1)) {
      Serial.println("Button B pressed!");
    } else if (buttons.isPressed(2)) {
      Serial.println("Button C pressed!");
    }
  }
}