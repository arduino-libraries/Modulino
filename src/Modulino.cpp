// Copyright (c) 2024 Arduino SA
// SPDX-License-Identifier: MPL-2.0

#include "Modulino.h"

// The only singleton that needs to exist
// Build before other objects to fix the Wire object
ModulinoClass Modulino __attribute__ ((init_priority (101)));

ModulinoColor RED(255, 0, 0);
ModulinoColor BLUE(0, 0, 255);
ModulinoColor GREEN(0, 255, 0);
ModulinoColor VIOLET(255, 0, 255);
ModulinoColor WHITE(255, 255, 255);