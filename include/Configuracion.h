#ifndef CONFIGURACION_H
#define CONFIGURACION_H

// COMUNICACIÓN

#define BAUDRATE 115200

// MOTOR

// NEMA 17 típico de 1.8 grados.
// 360 / 1.8 = 200 pasos por revolución.
#define MOTOR_STEPS_PER_REV 200

// Microstepping configurado físicamente en el A4988.
// 1  = paso completo
// 2  = 1/2
// 4  = 1/4
// 8  = 1/8
// 16 = 1/16

#define MOTOR_MICROSTEPS 1

// Transmicion mecanica


//sustituir en las pruebas

#define X_STEPS_PER_MM 80.0
#define Y_STEPS_PER_MM 80.0
#define Z_STEPS_PER_MM 400.0


// Velocidad

#define DEFAULT_FEEDRATE 300.0

#define MAX_FEEDRATE 1000.0

// pantalla

#define SERIAL_BAUDRATE 115200

// Pulso step

#define STEP_PULSE_US 5


// Logica


#define LIMIT_ACTIVE LOW

#define EMERGENCY_ACTIVE LOW

#define DRIVER_ENABLE LOW

#define DRIVER_DISABLE HIGH

// Posicion inicial

#define INITIAL_X 0.0
#define INITIAL_Y 0.0
#define INITIAL_Z 0.0


// MOVIMIENTO

// Dirección considerada positiva.
// Puede invertirse posteriormente.
#define X_DIR_POSITIVE HIGH
#define Y_DIR_POSITIVE HIGH
#define Z_DIR_POSITIVE HIGH



// SEGURIDAD

// Los finales de carrera utilizan INPUT_PULLUP.
// Por tanto:
// LOW = activo
// HIGH = inactivo
#define LIMIT_ACTIVE LOW


// Paro de emergencia.
// LOW = emergencia activa.
#define EMERGENCY_ACTIVE LOW


#endif