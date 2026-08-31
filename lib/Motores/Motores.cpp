#include <Arduino.h>

#include "Motores.h"
#include "Pines.h"
#include "Configuracion.h"



// FUNCIONES INTERNAS


void generarPaso(int stepPin)
{
    digitalWrite(stepPin, HIGH);

    delayMicroseconds(MOTOR_STEP_DELAY_US);

    digitalWrite(stepPin, LOW);

    delayMicroseconds(MOTOR_STEP_DELAY_US);
}



// INICIALIZACIÓN


void motores_init()
{
    
    // X
    

    pinMode(X_STEP_PIN, OUTPUT);
    pinMode(X_DIR_PIN, OUTPUT);


   
    // Y
    

    pinMode(Y_STEP_PIN, OUTPUT);
    pinMode(Y_DIR_PIN, OUTPUT);


    
    // Z
    

    pinMode(Z_STEP_PIN, OUTPUT);
    pinMode(Z_DIR_PIN, OUTPUT);


    // Estado inicial

    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);

    digitalWrite(X_DIR_PIN, LOW);
    digitalWrite(Y_DIR_PIN, LOW);
    digitalWrite(Z_DIR_PIN, LOW);
}

void moverSimultaneo(long pasosX, bool dirX, long pasosY, bool dirY, long pasosZ, bool dirZ) 
{
    // 1. Establecer las direcciones en los pines correspondientes
    digitalWrite(X_DIR_PIN, dirX);
    digitalWrite(Y_DIR_PIN, dirY);
    digitalWrite(Z_DIR_PIN, dirZ);

    // 2. Determinar el número máximo de pasos a dar entre los tres ejes
    long maxPasos = max(pasosX, max(pasosY, pasosZ));
    if (maxPasos == 0) return;

    // Variables de error para el algoritmo de interpolación
    long errX = maxPasos / 2;
    long errY = maxPasos / 2;
    long errZ = maxPasos / 2;

    // 3. Bucle único para mover todos los motores en paralelo
    for (long i = 0; i < maxPasos; i++) 
    {
        // Evaluación del Eje X
        if (pasosX > 0) {
            errX -= pasosX;
            if (errX < 0) {
                errX += maxPasos;
                generarPaso(X_STEP_PIN);
            }
        }

        // Evaluación del Eje Y
        if (pasosY > 0) {
            errY -= pasosY;
            if (errY < 0) {
                errY += maxPasos;
                generarPaso(Y_STEP_PIN);
            }
        }

        // Evaluación del Eje Z
        if (pasosZ > 0) {
            errZ -= pasosZ;
            if (errZ < 0) {
                errZ += maxPasos;
                generarPaso(Z_STEP_PIN);
            }
        }
    }
}

/* MOTOR X 


void moverX(long pasos, bool direccion)
{
    digitalWrite(X_DIR_PIN, direccion);

    for (long i = 0; i < pasos; i++)
    {
        generarPaso(X_STEP_PIN);
    }
}



// MOTOR Y

void moverY(long pasos, bool direccion)
{
    digitalWrite(Y_DIR_PIN, direccion);

    for (long i = 0; i < pasos; i++)
    {
        generarPaso(Y_STEP_PIN);
    }
}



// MOTOR Z


void moverZ(long pasos, bool direccion)
{
    digitalWrite(Z_DIR_PIN, direccion);

    for (long i = 0; i < pasos; i++)
    {
        generarPaso(Z_STEP_PIN);
    }
}


*/
// DETENER MOTORES

void detenerMotores()
{
    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);
}