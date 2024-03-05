#include "Modulino.h"

// The only singleton that needs to exist
// Build before other objects to fix the Wire object
ModulinoClass Modulino __attribute__ ((init_priority (101)));

APDS9999 color(Wire1);  // TODO: need to change to APDS9999 https://docs.broadcom.com/doc/APDS-9999-DS

ModulinoColor RED(255, 0, 0);
ModulinoColor BLUE(0, 0, 255);
ModulinoColor GREEN(0, 255, 0);
ModulinoColor VIOLET(255, 0, 255);
ModulinoColor WHITE(255, 255, 255);