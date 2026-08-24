#ifndef CONFIG_H
#define CONFIG_H


// CONFIGURACIÓN GENERAL


#define SERIAL_BAUDRATE 115200


// CONFIGURACIÓN DE MOTORES

#define MOTOR_STEPS_PER_REV 200

// Microstepping del A4988.
// Para esta primera prueba:
// 1 = paso completo
// 2 = medio paso
// 4 = 1/4
// 8 = 1/8
// 16 = 1/16
#define MOTOR_MICROSTEPS 1


// Tiempo entre pulsos STEP.
// Por ahora trabajaremos con velocidad fija.
#define MOTOR_STEP_DELAY_US 1000

#define X_STEPS_PER_MM 80.0
#define Y_STEPS_PER_MM 80.0
#define Z_STEPS_PER_MM 400.0


// LÍMITES DE VELOCIDAD


#define MAX_X_SPEED 1000.0
#define MAX_Y_SPEED 1000.0
#define MAX_Z_SPEED 500.0

#endif