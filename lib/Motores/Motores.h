#ifndef MOTORES_H
#define MOTORES_H

void motores_init();

void moverX(long pasos, bool direccion);
void moverY(long pasos, bool direccion);
void moverZ(long pasos, bool direccion);

#endif