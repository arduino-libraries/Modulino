#include <Modulino.h>

ModulinoRelay relay;

void setup() {
  Serial.begin(115200);
  Modulino.begin();
  relay.begin();

}
void loop() {
  relay.ON();
  if(relay.getStatus())
  {
    Serial.println("Relay state ON!");
  }
  delay(1000);
  relay.OFF();
  if(!(relay.getStatus()))
  {
    Serial.println("Relay state OFF!");
  }
  Serial.println();
  delay(1000);
}