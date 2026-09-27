#include <Arduino.h>

#include "Motores.h"
#include "Sensores.h"
#include "Seg.h"
#include "Comun.h"
#include "Gcode.h"

// SETUP
// Variable para rastrear el estado anterior del movimiento
bool estabaMoviendose = false;

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
    if (emergenciaActiva()) {
        detenerMotores();
    }

    //Procesar cualquier comando G-code entrante
    if (!motoresOcupados()) {
        comunicacion_update();
    }

    
    motores_update();

    //Detectar el momento exacto en que finaliza el movimiento para notificar al PC
    if (estabaMoviendose && !motoresOcupados()) {
        Serial.println("ok"); 
    }

    estabaMoviendose = motoresOcupados();
}

