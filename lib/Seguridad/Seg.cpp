#include "Seg.h"
#include "Pines.h"
#include <Arduino.h>

void seguridad_init()
{
    pinMode(PARO_EMERGENCIA, INPUT_PULLUP);
}

bool emergenciaActiva()
{
    return digitalRead(PARO_EMERGENCIA) == LOW;
}