#ifndef SENSORES_H
#define SENSORES_H

#include <Arduino.h>

void sensores_init();

bool xMinActivo();
bool yMinActivo();
bool zMinActivo();

bool xMaxActivo();
bool yMaxActivo();
bool zMaxActivo();



#endif