#ifndef MOTORES_H
#define MOTORES_H

#include <Arduino.h>

// Inicializa los pines de los motores.
void motores_init();


// Movimiento absoluto
bool moverA(float x, float y, float z, float feedrate);

// Movimiento relativo
bool moverRelativo(float dx, float dy, float dz, float feedrate);

// Detener
void detenerMotores();

// Posición
float obtenerX();
float obtenerY();
float obtenerZ();

// Establecer posición
void establecerPosicion(float x, float y, float z);


// Estado
bool motoresOcupados();


// Actualización
void motores_update();



#endif