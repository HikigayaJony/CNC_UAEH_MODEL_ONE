#include "Sensores.h"
#include "Pines.h"
#include "Configuracion.h"
#include <Arduino.h>


// Inicio 

void sensores_init()
{
    pinMode(X_MIN_PIN, INPUT_PULLUP);
    pinMode(Y_MIN_PIN, INPUT_PULLUP);
    pinMode(Z_MIN_PIN, INPUT_PULLUP);

    pinMode(X_MAX_PIN, INPUT_PULLUP);
    pinMode(Y_MAX_PIN, INPUT_PULLUP);
    pinMode(Z_MAX_PIN, INPUT_PULLUP);

}

// LIMITES MÍNIMOS

bool xMinActivo()
{
    return digitalRead(X_MIN_PIN) == LIMIT_ACTIVE;
}


bool yMinActivo()
{
    return digitalRead(Y_MIN_PIN) == LIMIT_ACTIVE;
}


bool zMinActivo()
{
    return digitalRead(Z_MIN_PIN) == LIMIT_ACTIVE;
}


// Limite maximo


bool xMaxActivo()
{
    return digitalRead(X_MAX_PIN) == LIMIT_ACTIVE;
}


bool yMaxActivo()
{
    return digitalRead(Y_MAX_PIN) == LIMIT_ACTIVE;
}


bool zMaxActivo()
{
    return digitalRead(Z_MAX_PIN) == LIMIT_ACTIVE;
}


