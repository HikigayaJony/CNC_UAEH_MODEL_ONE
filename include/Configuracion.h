#ifndef CONFIGURACION_H
#define CONFIGURACION_H

// COMUNICACIÓN
#define BAUDRATE 115200
#define SERIAL_BAUDRATE 115200

// MOTOR
#define MOTOR_STEPS_PER_REV 200
#define MOTOR_MICROSTEPS 16     // Recomendado: 1/16 para mayor suavidad

// Transmisión mecánica (Basado en 1/16 micropasos)
#define X_STEPS_PER_MM 80.0
#define Y_STEPS_PER_MM 80.0
#define Z_STEPS_PER_MM 400.0

// Velocidad
#define DEFAULT_FEEDRATE 300.0
#define MAX_FEEDRATE 1000.0

// Aceleración
#define MIN_FEEDRATE 30.0        // mm/min para el arranque
#define ACCELERATION 50.0        // mm/s^2 en el eje

// Pulso step
#define STEP_PULSE_US 5

// Posición inicial
#define INITIAL_X 0.0
#define INITIAL_Y 0.0
#define INITIAL_Z 0.0

// MOVIMIENTO (Dirección considerada positiva)
#define X_DIR_POSITIVE HIGH
#define Y_DIR_POSITIVE HIGH
#define Z_DIR_POSITIVE HIGH

// SEGURIDAD Y LÓGICA
#define LIMIT_ACTIVE LOW         // LOW = final de carrera presionado (INPUT_PULLUP)
#define EMERGENCY_ACTIVE LOW     // LOW = paro de emergencia activo
#define DRIVER_ENABLE LOW        // LOW = drivers habilitados
#define DRIVER_DISABLE HIGH      // HIGH = drivers deshabilitados

#endif