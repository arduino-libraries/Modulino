#include <Arduino_Modulino.h>

ModulinoBuzzer buzzer;

int frequency = 440;
int duration = 1000;

void setup(){
  Modulino.begin();
  buzzer.begin();
}

void loop(){

  buzzer.tone(frequency, duration);
  delay(1000);
  buzzer.tone(0, duration);
  delay(1000);

}
