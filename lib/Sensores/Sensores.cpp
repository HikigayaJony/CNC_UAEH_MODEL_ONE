#include "Sensores.h"
#include "Pines.h"
#include <Arduino.h>
// ehehehehehehehhe xd xd vamos los pibes
void sensores_init()
{
    pinMode(X_LIMIT_PIN, INPUT_PULLUP);
    pinMode(Y_LIMIT_PIN, INPUT_PULLUP);
    pinMode(Z_LIMIT_PIN, INPUT_PULLUP);
}
bool limiteXActivo()
{
    return digitalRead(X_LIMIT_PIN) == LOW;
}

bool limiteYActivo()
{
    return digitalRead(Y_LIMIT_PIN) == LOW;
}

bool limiteZActivo()
{
    return digitalRead(Z_LIMIT_PIN) == LOW;
}