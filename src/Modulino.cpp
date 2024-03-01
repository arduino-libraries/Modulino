#include "Modulino.h"

/*
Buttons buttons(62);
Tone buzzer(30);
LEDS leds(54);
Encoder encoder(58);
*/

// The only singleton that needs to exist
ModulinoClass Modulino;

BoschSensorClass imu(Wire1);
VL53L1X tof_sensor;
APDS9999 color(Wire1);  // TODO: need to change to APDS9999 https://docs.broadcom.com/doc/APDS-9999-DS
LPS22HBClass barometer(Wire1);
HS300xClass humidity(Wire1);

ModulinoColor RED(255, 0, 0);
ModulinoColor BLUE(0, 0, 255);
ModulinoColor GREEN(0, 255, 0);
ModulinoColor VIOLET(255, 0, 255);
ModulinoColor WHITE(255, 255, 255);