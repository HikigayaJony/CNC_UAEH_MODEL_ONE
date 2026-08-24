#include <Arduino.h>

#include "Motores.h"
#include "Pines.h"
#include "Configuracion.h"


// =====================================================
// Inicialización de motores
// =====================================================

void motores_init()
{
    // -------------------------
    // Motor X
    // -------------------------

    pinMode(X_STEP_PIN, OUTPUT);
    pinMode(X_DIR_PIN, OUTPUT);


    // -------------------------
    // Motor Y
    // -------------------------

    pinMode(Y_STEP_PIN, OUTPUT);
    pinMode(Y_DIR_PIN, OUTPUT);


    // -------------------------
    // Motor Z
    // -------------------------

    pinMode(Z_STEP_PIN, OUTPUT);
    pinMode(Z_DIR_PIN, OUTPUT);


    // Estado inicial de STEP
    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);
}


// =====================================================
// Motor X
// =====================================================

void moverX(long pasos, bool direccion)
{
    digitalWrite(X_DIR_PIN, direccion);

    for (long i = 0; i < pasos; i++)
    {
        digitalWrite(X_STEP_PIN, HIGH);

        delayMicroseconds(MOTOR_STEP_DELAY_US);

        digitalWrite(X_STEP_PIN, LOW);

        delayMicroseconds(MOTOR_STEP_DELAY_US);
    }
}


// =====================================================
// Motor Y
// =====================================================

void moverY(long pasos, bool direccion)
{
    digitalWrite(Y_DIR_PIN, direccion);

    for (long i = 0; i < pasos; i++)
    {
        digitalWrite(Y_STEP_PIN, HIGH);

        delayMicroseconds(MOTOR_STEP_DELAY_US);

        digitalWrite(Y_STEP_PIN, LOW);

        delayMicroseconds(MOTOR_STEP_DELAY_US);
    }
}


// =====================================================
// Motor Z
// =====================================================

void moverZ(long pasos, bool direccion)
{
    digitalWrite(Z_DIR_PIN, direccion);

    for (long i = 0; i < pasos; i++)
    {
        digitalWrite(Z_STEP_PIN, HIGH);

        delayMicroseconds(MOTOR_STEP_DELAY_US);

        digitalWrite(Z_STEP_PIN, LOW);

        delayMicroseconds(MOTOR_STEP_DELAY_US);
    }
}