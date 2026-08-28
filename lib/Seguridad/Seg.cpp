#include <Arduino.h>

#include "Seg.h"
#include "Pines.h"
#include "Configuracion.h"
#include "Sensores.h"



// INICIALIZACIÓN


void seguridad_init()
{
    pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);
}



// EMERGENCIA


bool emergenciaActiva()
{
    return digitalRead(EMERGENCY_STOP_PIN) == EMERGENCY_ACTIVE;
}



// SISTEMA SEGURO


bool sistemaSeguro()
{
    if (emergenciaActiva())
    {
        return false;
    }

    return true;
}