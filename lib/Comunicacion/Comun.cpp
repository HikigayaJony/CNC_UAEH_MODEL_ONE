#include "Comun.h"
#include "Configuracion.h"
#include <Arduino.h>

void comunicacion_init()
{
    Serial.begin(SERIAL_BAUDRATE);
}

void procesarComunicacion()
{
    if (Serial.available())
    {
        char comando = Serial.read();

        Serial.print("Comando recibido: ");
        Serial.println(comando);
    }
}