#ifndef COMUN_H
#define COMUN_H

#include <Arduino.h>

void comunicacion_init();
void procesarComunicacion();
void mostrarEstado();
long extraerPasos(String texto, char eje);

#endif