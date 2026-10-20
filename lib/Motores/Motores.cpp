#include <Arduino.h>
#include "Motores.h"
#include "Pines.h"
#include "Configuracion.h"
#include "Sensores.h"
#include "Seg.h"

// Parámetros de aceleración
#define MIN_FEEDRATE 30.0        
#define ACCELERATION 50.0

long errX = 0;
long errY = 0;
long errZ = 0;

// Posición actual
float posicionX = INITIAL_X;
float posicionY = INITIAL_Y;
float posicionZ = INITIAL_Z;

// Movimiento activo
bool movimientoActivo = false;

// Destino en pasos
long destinoX = 0;
long destinoY = 0;
long destinoZ = 0;

// Pasos actuales 
long pasosX = 0;
long pasosY = 0;
long pasosZ = 0;

// Variables para Bresenham 3D
long deltaX = 0, deltaY = 0, deltaZ = 0;
int dirX = 1, dirY = 1, dirZ = 1;
long maxPasos = 0;
long pasoActual = 0;

// Variables para la rampa de aceleración
static unsigned long delayActualUS = 0;
static unsigned long delayMinimoUS = 0;   
static unsigned long delayStartUS = 0;    

float acelerapas = 0.0;         // pasos/s^2
long pasosdeaceleracion = 0;    // pasos para acelerar

static unsigned long ultimoPasoMicros = 0;

// Conversión de mm a pasos
long mmAStepsX(float mm) { return (long)(mm * X_STEPS_PER_MM); }
long mmAStepsY(float mm) { return (long)(mm * Y_STEPS_PER_MM); }
long mmAStepsZ(float mm) { return (long)(mm * Z_STEPS_PER_MM); }

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

void habilitarDrivers()
{
    digitalWrite(STEPPER_ENABLE_PIN, DRIVER_ENABLE);
}

void deshabilitarDrivers()
{
    digitalWrite(STEPPER_ENABLE_PIN, DRIVER_DISABLE);
}

bool moverA(float x, float y, float z, float feedrate)
{
    if (!sistemaSeguro() || movimientoActivo) {
        return false;
    }

    if (feedrate <= 0) {
        feedrate = DEFAULT_FEEDRATE;
    }
    if (feedrate > MAX_FEEDRATE) {
        feedrate = MAX_FEEDRATE;
    }

    destinoX = mmAStepsX(x);
    destinoY = mmAStepsY(y);
    destinoZ = mmAStepsZ(z);

    // Cálculo de diferencias absolutas
    deltaX = abs(destinoX - pasosX);
    deltaY = abs(destinoY - pasosY);
    deltaZ = abs(destinoZ - pasosZ);

    dirX = (destinoX >= pasosX) ? 1 : -1;
    dirY = (destinoY >= pasosY) ? 1 : -1;
    dirZ = (destinoZ >= pasosZ) ? 1 : -1;

    if (deltaX > 0) digitalWrite(X_DIR_PIN, dirX > 0 ? HIGH : LOW);
    if (deltaY > 0) digitalWrite(Y_DIR_PIN, dirY > 0 ? HIGH : LOW);
    if (deltaZ > 0) digitalWrite(Z_DIR_PIN, dirZ > 0 ? HIGH : LOW);

    // Eje dominante (el que dará más pasos)
    maxPasos = max(deltaX, max(deltaY, deltaZ));

    if (maxPasos == 0) {
        return true; 
    }

    // Inicializar acumuladores de error Bresenham 3D
    errX = maxPasos / 2;
    errY = maxPasos / 2;
    errZ = maxPasos / 2;

    pasoActual = 0;

    // Cálculo de velocidades y rampa
    float pasosPorSegTarget = (feedrate / 60.0) * X_STEPS_PER_MM;
    float pasosPorSegStart = (MIN_FEEDRATE / 60.0) * X_STEPS_PER_MM;

    delayMinimoUS = (unsigned long)(1000000.0 / pasosPorSegTarget);
    delayStartUS = (unsigned long)(1000000.0 / pasosPorSegStart);
    delayActualUS = delayStartUS;

    // Convertir aceleración a pasos/s^2
    acelerapas = ACCELERATION * X_STEPS_PER_MM;

    // Pasos necesarios para acelerar
    pasosdeaceleracion = (long)((pow(pasosPorSegTarget, 2) - pow(pasosPorSegStart, 2)) / (2.0 * acelerapas));

    if (pasosdeaceleracion > maxPasos / 2) {
        pasosdeaceleracion = maxPasos / 2;
    }

    habilitarDrivers();
    movimientoActivo = true;
    ultimoPasoMicros = micros();
    return true;
}

bool moverRelativo(float dx, float dy, float dz, float feedrate)
{
    return moverA(posicionX + dx, posicionY + dy, posicionZ + dz, feedrate);
}

void motores_update()
{
    if (!movimientoActivo) return;

    if (!sistemaSeguro()) {
        detenerMotores();
        return;
    }

    // Control de tiempo no bloqueante usando delayActualUS (Rampa)
    if (micros() - ultimoPasoMicros < delayActualUS) {
        return;
    }
    ultimoPasoMicros = micros();

    bool darPasoX = false;
    bool darPasoY = false;
    bool darPasoZ = false;

    // Algoritmo de Bresenham 3D simétrico
    errX -= deltaX;
    if (errX < 0) {
        errX += maxPasos;
        darPasoX = true;
    }

    errY -= deltaY;
    if (errY < 0) {
        errY += maxPasos;
        darPasoY = true;
    }

    errZ -= deltaZ;
    if (errZ < 0) {
        errZ += maxPasos;
        darPasoZ = true;
    }

    // Enviar pulsos únicamente a los ejes que les corresponda según Bresenham
    if (darPasoX) digitalWrite(X_STEP_PIN, HIGH);
    if (darPasoY) digitalWrite(Y_STEP_PIN, HIGH);
    if (darPasoZ) digitalWrite(Z_STEP_PIN, HIGH);

    delayMicroseconds(STEP_PULSE_US);

    // Bajar los impulsos
    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);

    // Actualizar contadores de pasos reales
    if (darPasoX) pasosX += dirX;
    if (darPasoY) pasosY += dirY;
    if (darPasoZ) pasosZ += dirZ;

    pasoActual++;

    // Actualizar coordenadas en milímetros
    posicionX = (float)pasosX / X_STEPS_PER_MM;
    posicionY = (float)pasosY / Y_STEPS_PER_MM;
    posicionZ = (float)pasosZ / Z_STEPS_PER_MM;

    // Cálculo de rampa de velocidad (Aceleración / Crucero / Desaceleración)
    if (pasoActual < pasosdeaceleracion) {
        float progreso = (float)pasoActual / pasosdeaceleracion;
        delayActualUS = delayStartUS - (progreso * (delayStartUS - delayMinimoUS));
    }
    else if (pasoActual > (maxPasos - pasosdeaceleracion)) {
        float progreso = (float)(maxPasos - pasoActual) / pasosdeaceleracion;
        delayActualUS = delayStartUS - (progreso * (delayStartUS - delayMinimoUS));
    }
    else {
        delayActualUS = delayMinimoUS;
    }

    // Comprobación de fin de movimiento
    if (pasoActual >= maxPasos) {
        movimientoActivo = false;

        pasosX = destinoX;
        pasosY = destinoY;
        pasosZ = destinoZ;

        posicionX = (float)pasosX / X_STEPS_PER_MM;
        posicionY = (float)pasosY / Y_STEPS_PER_MM;
        posicionZ = (float)pasosZ / Z_STEPS_PER_MM;

        digitalWrite(X_STEP_PIN, LOW);
        digitalWrite(Y_STEP_PIN, LOW);
        digitalWrite(Z_STEP_PIN, LOW);
    }
}

void detenerMotores()
{
    movimientoActivo = false;
    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);
    deshabilitarDrivers();
}

float obtenerX() { return posicionX; }
float obtenerY() { return posicionY; }
float obtenerZ() { return posicionZ; }

void establecerPosicion(float x, float y, float z)
{
    posicionX = x;
    posicionY = y;
    posicionZ = z;

    pasosX = mmAStepsX(x);
    pasosY = mmAStepsY(y);
    pasosZ = mmAStepsZ(z);
}

bool motoresOcupados()
{
    return movimientoActivo;
}