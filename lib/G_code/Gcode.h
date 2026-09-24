#ifndef GCODE_H
#define GCODE_H

#include <Arduino.h>

void gcode_init();

void procesarGCode(const String& linea);


#endif