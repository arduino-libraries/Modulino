#include "Wire.h"

// Setting new_address to 0 means that the module will get back its original address
const uint8_t new_address = 0;

uint8_t address;

void setup() {
  Wire1.begin();
  Serial.begin(115200);
  delay(1000);
  if (new_address != 0 && (new_address < 8 || new_address > 0x77)) {
    Serial.println("Address outside valid range");
    while (1);
  }
  // Search for devices and wait for user confirmation
  for (int i = 8; i < 128; i++) {
    Wire1.beginTransmission(i);
    auto err = Wire1.endTransmission();
    if (err == 0) {
      Serial.print("Found device at ");
      Serial.println(i);
      address = i;
      Serial.println("Press 'c' to configure te new address");
    }
  }
}

String pinstrapToName(uint8_t pinstrap) {
  switch (pinstrap) {
    case 0x3C:
      return "BUZZER";
    case 0x58:
      return "JOYSTICK";
    case 0x7C:
      return "BUTTONS";
    case 0x28:
      return "RELAY";
    case 0x76:
    case 0x74:
      return "ENCODER";
    case 0x6C:
      return "SMARTLEDS";
    case 0x70:
      return "VIBRO";
  }
  return "UNKNOWN";
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    if (Serial.read() == 'c') {
      Serial.print("Assigning new address to ");
      Serial.println(address);
      uint8_t data[40] = { 'C', 'F', new_address * 2 };
      Wire1.beginTransmission(address);
      Wire1.write(data, 40);
      Wire1.endTransmission();
      delay(1000);
      Wire1.requestFrom(new_address, 1);
      Serial.println("Device type " + pinstrapToName(Wire1.read()) + " at new address " + String(new_address));
    }
  }
}
