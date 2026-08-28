#ifndef MOTORES_H
#define MOTORES_H


// Inicializa los pines de los motores.
void motores_init();


// Movimiento de cada eje en pasos.
void moverX(long pasos, bool direccion);
void moverY(long pasos, bool direccion);
void moverZ(long pasos, bool direccion);


// Detiene todos los motores.
void detenerMotores();


#endif