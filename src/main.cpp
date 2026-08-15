#include <Arduino.h>

#include "Motores.h"
#include "Sensores.h"
#include "Comun.h"
#include "Seg.h"

void setup()
{
    motores_init();
    sensores_init();
    comunicacion_init();
    seguridad_init();

    Serial.println("CNC Arduino Mega iniciada");
}

void loop()
{
    procesarComunicacion();

    if (emergenciaActiva())
    {
        Serial.println("EMERGENCIA");
    }
}