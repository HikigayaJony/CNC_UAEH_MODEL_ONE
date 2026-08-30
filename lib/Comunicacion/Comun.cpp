#include <Arduino.h>

#include "Comun.h"
#include "Motores.h"
#include "Sensores.h"
#include "Seg.h"



// INICIALIZACIÓN


void comunicacion_init()
{
    Serial.begin(115200);

    Serial.println();
    Serial.println("=================================");
    Serial.println(" CNC UAEH - FIRMWARE");
    Serial.println(" Arduino Mega + A4988");
    Serial.println("=================================");
    Serial.println("Sistema iniciado.");
    Serial.println("Comandos:");
    Serial.println("X100   -> mover X 100 pasos");
    Serial.println("X-100  -> mover X -100 pasos");
    Serial.println("Y100   -> mover Y 100 pasos");
    Serial.println("Y-100  -> mover Y -100 pasos");
    Serial.println("Z100   -> mover Z 100 pasos");
    Serial.println("Z-100  -> mover Z -100 pasos");
    Serial.println("STOP   -> detener");
    Serial.println("STATUS -> estado del sistema");
    Serial.println();
}



// ESTADO


void mostrarEstado()
{
    Serial.println();
    Serial.println("----- ESTADO -----");

    if (emergenciaActiva())
    {
        Serial.println("EMERGENCIA: ACTIVA");
    }
    else
    {
        Serial.println("EMERGENCIA: OK");
    }


    Serial.print("LIMITE X: ");

    if (limiteXActivo())
        Serial.println("ACTIVO");
    else
        Serial.println("OK");


    Serial.print("LIMITE Y: ");

    if (limiteYActivo())
        Serial.println("ACTIVO");
    else
        Serial.println("OK");


    Serial.print("LIMITE Z: ");

    if (limiteZActivo())
        Serial.println("ACTIVO");
    else
        Serial.println("OK");

    Serial.println("------------------");
    Serial.println();
}

// Extrae el número asociado a un eje (ej. extrae 100 de "X100" o -50 de "Y-50")
long extraerPasos(String texto, char eje) {
    int idx = texto.indexOf(eje);
    if (idx == -1) return 0;

    int idxFin = idx + 1;
    while (idxFin < texto.length() && (isDigit(texto[idxFin]) || texto[idxFin] == '-')) {
        idxFin++;
    }
    return texto.substring(idx + 1, idxFin).toInt();
}


// PROCESAMIENTO DE COMUNICACIÓN



void procesarComunicacion()
{
    if (Serial.available() == 0)
    {
        return;
    }


    String comando = Serial.readStringUntil('\n');

    comando.trim();
    comando.toUpperCase();

    if (comando.length() == 0)
    {
        return;
    }


    
    // STOP
    

    if (comando == "STOP")
    {
        detenerMotores();

        Serial.println("OK: motores detenidos.");

        return;
    }


    
    // STATUS
    

    if (comando == "STATUS")
    {
        mostrarEstado();

        return;
    }


    
    // SEGURIDAD
 
    if (!sistemaSeguro())
    {
        Serial.println("ERROR: sistema en estado de emergencia.");

        return;
    }


    // COMANDO DE MOVIMIENTO COMBINADO (X, Y, Z)
    long pasosX = extraerPasos(comando, 'X');
    long pasosY = extraerPasos(comando, 'Y');
    long pasosZ = extraerPasos(comando, 'Z');

    // Validar si el comando contiene al menos un eje con movimiento
    if (pasosX == 0 && pasosY == 0 && pasosZ == 0) {
        Serial.println("ERROR: comando no valido.");
        return;
    }

    // Determinar dirección de cada eje
    bool dirX = pasosX > 0;
    bool dirY = pasosY > 0;
    bool dirZ = pasosZ > 0;

    // Validación de sensores de límite por eje
    if (pasosX != 0 && dirX && limiteXActivo()) {
        Serial.println("ERROR: limite X activo.");
        return;
    }
    if (pasosY != 0 && dirY && limiteYActivo()) {
        Serial.println("ERROR: limite Y activo.");
        return;
    }
    if (pasosZ != 0 && dirZ && limiteZActivo()) {
        Serial.println("ERROR: limite Z activo.");
        return;
    }

    // Ejecución simultánea de los tres motores
    moverSimultaneo(abs(pasosX), dirX, abs(pasosY), dirY, abs(pasosZ), dirZ);

    Serial.println("OK: movimiento ejecutado.");
}
/* COMANDO DE MOVIMIENTO
    

    char eje = comando.charAt(0);

    String valorTexto = comando.substring(1);

    long pasos = valorTexto.toInt();


    if (pasos == 0)
    {
        Serial.println("ERROR: comando no valido.");

        return;
    }


    bool direccion = pasos > 0;

    pasos = abs(pasos);


    
    // EJE X
    

    if (eje == 'X')
    {
        if (direccion && limiteXActivo())
        {
            Serial.println("ERROR: limite X activo.");

            return;
        }

        moverX(pasos, direccion);

        Serial.println("OK: movimiento X.");

        return;
    }


    
    // EJE Y
    
    if (eje == 'Y')
    {
        if (direccion && limiteYActivo())
        {
            Serial.println("ERROR: limite Y activo.");

            return;
        }

        moverY(pasos, direccion);

        Serial.println("OK: movimiento Y.");

        return;
    }


    
    // EJE Z
    

    if (eje == 'Z')
    {
        if (direccion && limiteZActivo())
        {
            Serial.println("ERROR: limite Z activo.");

            return;
        }

        moverZ(pasos, direccion);

        Serial.println("OK: movimiento Z.");

        return;
    }


    
    // COMANDO DESCONOCIDO
    
    Serial.println("ERROR: comando desconocido.");
}
    */