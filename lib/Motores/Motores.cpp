#include <Arduino.h>

#include "Motores.h"
#include "Pines.h"
#include "Configuracion.h"
#include "Sensores.h"
#include "Seg.h"


// Posicion actual

float posicionX = INITIAL_X;
float posicionY = INITIAL_Y;
float posicionZ = INITIAL_Z;

// Movimiento

bool movimientoActivo = false;

// Destino

long destinoX = 0;
long destinoY = 0;
long destinoZ = 0;

// Pasos actuales 

long pasosX = 0;
long pasosY = 0;
long pasosZ = 0;

// Tiempo en microsegundos entre cada paso
unsigned long pasoDelayUS = 1000;
unsigned long ultimoPasoMicros = 0;

// Conversion de mm a pasos

long mmAStepsX(float mm)
{
    return (long)(mm * X_STEPS_PER_MM);
}


long mmAStepsY(float mm)
{
    return (long)(mm * Y_STEPS_PER_MM);
}


long mmAStepsZ(float mm)
{
    return (long)(mm * Z_STEPS_PER_MM);
}


// Inicio


void motores_init()
{
    pinMode(X_STEP_PIN, OUTPUT);
    pinMode(X_DIR_PIN, OUTPUT);

    pinMode(Y_STEP_PIN, OUTPUT);
    pinMode(Y_DIR_PIN, OUTPUT);

    pinMode(Z_STEP_PIN, OUTPUT);
    pinMode(Z_DIR_PIN, OUTPUT);

    pinMode(STEPPER_ENABLE_PIN, OUTPUT);

    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);

    digitalWrite(STEPPER_ENABLE_PIN, DRIVER_DISABLE);

    posicionX = INITIAL_X;
    posicionY = INITIAL_Y;
    posicionZ = INITIAL_Z;
}

// Habilitación de drivers


void habilitarDrivers()
{
    digitalWrite(STEPPER_ENABLE_PIN, DRIVER_ENABLE);
}


void deshabilitarDrivers()
{
    digitalWrite(STEPPER_ENABLE_PIN, DRIVER_DISABLE);
}


// Movimiento absoluto

bool moverA(float x, float y, float z, float feedrate)
{
    if (!sistemaSeguro() ||  movimientoActivo){
        return false;
    }
    

    if (feedrate <= 0){
        feedrate = DEFAULT_FEEDRATE;
    }


    if (feedrate > MAX_FEEDRATE){
        feedrate = MAX_FEEDRATE;
    }


    destinoX = mmAStepsX(x);
    destinoY = mmAStepsY(y);
    destinoZ = mmAStepsZ(z);


    pasosX = mmAStepsX(posicionX);
    pasosY = mmAStepsY(posicionY);
    pasosZ = mmAStepsZ(posicionZ);

    // Esto da la direccion

    digitalWrite(X_DIR_PIN,destinoX >= pasosX ? HIGH : LOW);

    digitalWrite(Y_DIR_PIN,destinoY >= pasosY ? HIGH : LOW);

    digitalWrite(Z_DIR_PIN, destinoZ >= pasosZ ? HIGH : LOW);

    // Nueva seccion, calcular el tiempo entre pasos

    float pasosPorSegundo = (feedrate / 60.0) * X_STEPS_PER_MM; //Para no saturar el motor en base al cpu

    if (pasosPorSegundo > 0) {
        // Tiempo por paso en microsegundos (1,000,000 us / pasos_por_segundo)
        pasoDelayUS = (unsigned long)(1000000.0 / pasosPorSegundo);
    } else {
        pasoDelayUS = 10000; // Valor seguro por defecto (10ms)
    }

    habilitarDrivers();

    movimientoActivo = true;

    return true;
}



// Movimiento relativo

bool moverRelativo(float dx, float dy, float dz, float feedrate)
{
    return moverA(
        posicionX + dx,
        posicionY + dy,
        posicionZ + dz,
        feedrate
    );
}

// Update

void motores_update()
{
    if (!movimientoActivo)
    {
        return;
    }


    if (!sistemaSeguro())
    {
        detenerMotores();
        return;
    }


    // Tiempo de seguridad

    if (micros() - ultimoPasoMicros < pasoDelayUS) {
        return;
        }
    ultimoPasoMicros = micros();

    // pasos para X
    
    if (pasosX != destinoX)
    {
        digitalWrite(X_STEP_PIN, HIGH);

        delayMicroseconds(STEP_PULSE_US);

        digitalWrite(X_STEP_PIN, LOW);

        pasosX += destinoX > pasosX ? 1 : -1;

        posicionX = pasosX / X_STEPS_PER_MM;
    }


    // eje Y

    if (pasosY != destinoY)
    {
        digitalWrite(Y_STEP_PIN, HIGH);

        delayMicroseconds(STEP_PULSE_US);

        digitalWrite(Y_STEP_PIN, LOW);

        pasosY += destinoY > pasosY ? 1 : -1;

        posicionY = pasosY / Y_STEPS_PER_MM;
    }


    // Z

    if (pasosZ != destinoZ)
    {
        digitalWrite(Z_STEP_PIN, HIGH);

        delayMicroseconds(STEP_PULSE_US);

        digitalWrite(Z_STEP_PIN, LOW);

        pasosZ += destinoZ > pasosZ ? 1 : -1;

        posicionZ = pasosZ / Z_STEPS_PER_MM;
    }


    
    // Fin del movimiento
    
    if (pasosX == destinoX &&
        pasosY == destinoY &&
        pasosZ == destinoZ)
    {
        movimientoActivo = false;
    }
}


// Stop

void detenerMotores()
{
    movimientoActivo = false;

    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);

    deshabilitarDrivers();
}



// Posición

float obtenerX()
{
    return posicionX;
}


float obtenerY()
{
    return posicionY;
}


float obtenerZ()
{
    return posicionZ;
}


void establecerPosicion(float x, float y, float z)
{
    posicionX = x;
    posicionY = y;
    posicionZ = z;
}



// Estado de los motores


bool motoresOcupados()
{
    return movimientoActivo;
}
