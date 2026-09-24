#include <Arduino.h>

#include "Seg.h"
#include "Pines.h"
#include "Configuracion.h"
#include "Sensores.h"
#include "Motores.h"

bool emergenciaSoftware = false;


// Inicio

void seguridad_init()
{
    pinMode(EMERGENCY_STOP_PIN, INPUT_PULLUP);

    emergenciaSoftware = false;
}

// Emergencia

bool emergenciaActiva()
{
    return digitalRead(EMERGENCY_STOP_PIN) == EMERGENCY_ACTIVE;
}

// Emergencia software

void activarEmergencia()
{
    emergenciaSoftware = true;

    detenerMotores();
}


void limpiarEmergencia()
{
    if (!emergenciaActiva())
    {
        emergenciaSoftware = false;
    }
}

// Estado del sistema

bool sistemaSeguro()
{
    if (emergenciaActiva())
    {
        return false;
    }

    if (emergenciaSoftware)
    {
        return false;
    }

    return true;
}