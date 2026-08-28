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
}


// SETUP


void setup()
{
    // Inicializar motores
    motores_init();


    // Inicializar sensores
    sensores_init();


    // Inicializar seguridad
    seguridad_init();


    // Inicializar comunicación USB/Serial
    comunicacion_init();


    Serial.println("Inicializacion completa.");
    Serial.println("CNC lista.");
    Serial.println();
}



// LOOP


void loop()
{
    // Procesar comandos recibidos desde el PC
    procesarComunicacion();
}
