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



// MOTOR X


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



// DETENER MOTORES

void detenerMotores()
{
    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);
}