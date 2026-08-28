#ifndef CONFIGURACION_H
#define CONFIGURACION_H



// COMUNICACIÓN


#define BAUDRATE 115200



// MOTOR


// NEMA 17 típico de 1.8 grados.
// 360 / 1.8 = 200 pasos por revolución.
#define MOTOR_STEPS_PER_REV 200


// Microstepping configurado físicamente en el A4988.
//
// 1  = paso completo
// 2  = 1/2
// 4  = 1/4
// 8  = 1/8
// 16 = 1/16
//
#define MOTOR_MICROSTEPS 1


// Tiempo entre cambios de STEP.
// Menor valor = mayor velocidad.
//
// Esta primera versión usa velocidad fija.
#define MOTOR_STEP_DELAY_US 1000



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