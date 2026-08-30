#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>

void sensores_init();

bool limiteXActivo();
bool limiteYActivo();
bool limiteZActivo();


#endif