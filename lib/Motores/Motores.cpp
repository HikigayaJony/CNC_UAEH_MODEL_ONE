#include <Arduino.h>

#include "Motores.h"
#include "Pines.h"
#include "Configuracion.h"
#include "Sensores.h"
#include "Seg.h"

//rampa de aceleracionv1 mm/min

#define MIN_FEEDRATE 30.0        
#define ACCELERATION 50.0

long errX = 0;
long errY = 0;
long errZ = 0;



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


// para el algoritmo bresenham

long deltaX = 0, deltaY = 0, deltaZ = 0;
int dirX = 1, dirY = 1, dirZ = 1;
long maxPasos = 0;
long pasoActual = 0;



// rampa de aceleracion

static unsigned long delayActualUS = 0;
static unsigned long delayMinimoUS = 0;   // Retardo para la velocidad objetivo 
static unsigned long delayStartUS = 0;    // Retardo inicial 

float acelerapas =0.0; //pasos/s^2
long pasosdeaceleracion = 0; //pasos para acelerar

static unsigned long ultimoPasoMicros = 0;



// Tiempo en microsegundos entre cada paso
unsigned long pasoDelayUS = 1000;
unsigned long UltimoPasoMicros = 0;

// Conversion de mm a pasos

long mmAStepsX(float mm){ return (long)(mm * X_STEPS_PER_MM);}


long mmAStepsY(float mm){ return (long)(mm * Y_STEPS_PER_MM);}


long mmAStepsZ(float mm){return (long)(mm * Z_STEPS_PER_MM);}


// Inicio de control


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

    
    

    // calculo de diferencias por bresenham
    deltaX = abs(destinoX - pasosX);
    deltaY = abs(destinoY - pasosY);
    deltaZ = abs(destinoZ - pasosZ);

    dirX = (destinoX >= pasosX) ? 1 : -1;
    dirY = (destinoY >= pasosY) ? 1 : -1;
    dirZ = (destinoZ >= pasosZ) ? 1 : -1;

    if (deltaX > 0) digitalWrite(X_DIR_PIN, dirX > 0 ? HIGH : LOW);
    if (deltaY > 0) digitalWrite(Y_DIR_PIN, dirY > 0 ? HIGH : LOW);
    if (deltaZ > 0) digitalWrite(Z_DIR_PIN, dirZ > 0 ? HIGH : LOW);

    //eje dominante (el que dará más pasos)
    maxPasos = max(deltaX, max(deltaY, deltaZ));

    // se detiene cuando estemos en el lugar de destino

    if (maxPasos == 0) {
        return true; 
    }

    // Inicializar errores de interpolación respecto al eje principal bresenham 3D
    // Inicializar acumuladores de error
    errX = maxPasos / 2;
    errY = maxPasos / 2;
    errZ = maxPasos / 2;

    pasoActual = 0;
    movimientoActivo = true;

    // calculo para las aceleraciones que deben tomar los ejes
    float pasosPorSegTarget = (feedrate / 60.0) * X_STEPS_PER_MM;
    float pasosPorSegStart = (MIN_FEEDRATE / 60.0) * X_STEPS_PER_MM;

    delayMinimoUS = (unsigned long)(1000000.0 / pasosPorSegTarget);
    delayStartUS = (unsigned long)(1000000.0 / pasosPorSegStart);
    delayActualUS = delayStartUS;

    //convertimos aceleracion apasos
    acelerapas= ACCELERATION * X_STEPS_PER_MM;

    // definimos cuantos pasos necesitamos para acelerar hasta la velocidad objetivo
    acelerapas = (long)((pow(pasosPorSegTarget, 2) - pow(pasosPorSegStart, 2)) / (2.0 * acelerapas));

    // en trayectos cortos, usamos
    if (acelerapas > maxPasos / 2) {
        acelerapas = maxPasos / 2;
    }

    habilitarDrivers();
    movimientoActivo = true;
    ultimoPasoMicros = micros();
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

    // Si hemos llegado al destino, detenemos el movimiento y actualizamos la posición final
if (pasoActual >= maxPasos) {
    movimientoActivo = false;

    pasosX = destinoX;
    pasosY = destinoY;
    pasosZ = destinoZ;

    posicionX = (float)pasosX / X_STEPS_PER_MM;
    posicionY = (float)pasosY / Y_STEPS_PER_MM;
    posicionZ = (float)pasosZ / Z_STEPS_PER_MM;

    // Opcional: apagar pulsos por seguridad
    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);
}
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

    bool darPasoX = false;
    bool darPasoY = false;
    bool darPasoZ = false;

    // Acumuladores de error para Bresenham 3D
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

    if (darPasoX) digitalWrite(X_STEP_PIN, HIGH);
    if (darPasoY) digitalWrite(Y_STEP_PIN, HIGH);
    if (darPasoZ) digitalWrite(Z_STEP_PIN, HIGH);

    

    // pasos para X
    if (deltaX == maxPasos || (2 * errY >= 0)) {
        digitalWrite(X_STEP_PIN, HIGH);
    }
    


    // eje Y
    if (deltaY == maxPasos || (2 * errY >= 0 && deltaX == maxPasos)) {
        digitalWrite(Y_STEP_PIN, HIGH);
    }

    // Z

    if (deltaZ == maxPasos || (2 * errZ >= 0 && deltaX == maxPasos)) {
        digitalWrite(Z_STEP_PIN, HIGH);
    }

    delayMicroseconds(STEP_PULSE_US);

    // Bajar los impulsos
    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);


    //contadores de posicion
    
    if (deltaX == maxPasos || (2 * errY >= 0)) {
        pasosX += dirX;
        if (deltaX != maxPasos) errY -= 2 * maxPasos;
    }
    if (deltaY == maxPasos || (2 * errY >= 0)) {
        pasosY += dirY;
        errY += 2 * deltaY;
    }
    if (deltaZ == maxPasos || (2 * errZ >= 0)) {
        pasosZ += dirZ;
        errZ += 2 * deltaZ;
    }

    if (darPasoX) pasosX += dirX;
    if (darPasoY) pasosY += dirY;
    if (darPasoZ) pasosZ += dirZ;

    pasoActual++;

    //actualizar coordenadas

    posicionX = (float)pasosX / X_STEPS_PER_MM;
    posicionY = (float)pasosY / Y_STEPS_PER_MM;
    posicionZ = (float)pasosZ / Z_STEPS_PER_MM;

    // calculo de rampa de aceleracion

    if (pasoActual < acelerapas) {
        // Fase 1: Aceleración (Disminuir delayActualUS)
        float progreso = (float)pasoActual / acelerapas;
        delayActualUS = delayStartUS - (progreso * (delayStartUS - delayMinimoUS));
    }
    else if (pasoActual > (maxPasos - acelerapas)) {
        // Fase 3: Desaceleración (Aumentar delayActualUS)
        float progreso = (float)(maxPasos - pasoActual) / acelerapas;
        delayActualUS = delayStartUS - (progreso * (delayStartUS - delayMinimoUS));
    }
    else {
        // Fase 2: Velocidad Crucero
        delayActualUS = delayMinimoUS;
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

    pasosX = mmAStepsX(x);
    pasosY = mmAStepsY(y);
    pasosZ = mmAStepsZ(z);
}



// Estado de los motores


bool motoresOcupados()
{
    return movimientoActivo;
}
