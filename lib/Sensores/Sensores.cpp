#include <Arduino.h>

#include "Sensores.h"
#include "Pines.h"
#include "Configuracion.h"



// INICIALIZACIÓN


void sensores_init()
{
    pinMode(X_LIMIT_PIN, INPUT_PULLUP);
    pinMode(Y_LIMIT_PIN, INPUT_PULLUP);
    pinMode(Z_LIMIT_PIN, INPUT_PULLUP);
}



// LIMITE X


bool limiteXActivo()
{
    return digitalRead(X_LIMIT_PIN) == LIMIT_ACTIVE;
}



// LIMITE Y


bool limiteYActivo()
{
    return digitalRead(Y_LIMIT_PIN) == LIMIT_ACTIVE;
}



// LIMITE Z


bool limiteZActivo()
{
    return digitalRead(Z_LIMIT_PIN) == LIMIT_ACTIVE;
}