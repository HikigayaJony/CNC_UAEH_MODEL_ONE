#include <Arduino.h>

#include "Motores.h"
#include "Sensores.h"
#include "Seg.h"
#include "Comun.h"
#include "Gcode.h"

// SETUP


//Hola
void setup()
{
    motores_init();

    sensores_init();

    seguridad_init();

    gcode_init();

    comunicacion_init();

    Serial.println("Inicializacion completa.");
    Serial.println("CNC lista.");
    Serial.println();
}

// LOOP

void loop()
{
    comunicacion_update();

    motores_update();

//Prueba de conección con el PC
// Seguridad

    if (emergenciaActiva())
    {
        detenerMotores();
    }
}
