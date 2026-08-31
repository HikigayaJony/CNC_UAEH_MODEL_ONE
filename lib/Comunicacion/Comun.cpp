#include <Arduino.h>

#include "Comun.h"
#include "Motores.h"
#include "Sensores.h"
#include "Seg.h"

// INICIALIZACIÓN
void comunicacion_init()
{
    Serial.begin(115200);

    Serial.println();
    Serial.println("=================================");
    Serial.println(" CNC UAEH - FIRMWARE");
    Serial.println(" Arduino Mega + A4988");
    Serial.println("=================================");
    Serial.println("Sistema iniciado.");
    Serial.println("Comandos:");
    Serial.println("X100   -> mover X 100 pasos");
    Serial.println("X-100  -> mover X -100 pasos");
    Serial.println("Y100   -> mover Y 100 pasos");
    Serial.println("Y-100  -> mover Y -100 pasos");
    Serial.println("Z100   -> mover Z 100 pasos");
    Serial.println("Z-100  -> mover Z -100 pasos");
    Serial.println("STOP   -> detener");
    Serial.println("STATUS -> estado del sistema");
    Serial.println();
}

// ESTADO
void mostrarEstado()
{
    Serial.println();
    Serial.println("----- ESTADO -----");

    if (emergenciaActiva())
    {
        Serial.println("EMERGENCIA: ACTIVA");
    }
    else
    {
        Serial.println("EMERGENCIA: OK");
    }

    Serial.print("LIMITE X: ");
    if (limiteXActivo())
        Serial.println("ACTIVO");
    else
        Serial.println("OK");

    Serial.print("LIMITE Y: ");
    if (limiteYActivo())
        Serial.println("ACTIVO");
    else
        Serial.println("OK");

    Serial.print("LIMITE Z: ");
    if (limiteZActivo())
        Serial.println("ACTIVO");
    else
        Serial.println("OK");

    Serial.println("------------------");
    Serial.println();
}

// Extrae el número asociado a un eje
long extraerPasos(String texto, char eje) {
    int idx = texto.indexOf(eje);
    if (idx == -1) return 0;

    int idxFin = idx + 1;
    while (idxFin < (int)texto.length() && (isDigit(texto[idxFin]) || texto[idxFin] == '-')) {
        idxFin++;
    }
    return texto.substring(idx + 1, idxFin).toInt();
}

// PROCESAMIENTO DE COMUNICACIÓN
void procesarComunicacion()
{
    if (Serial.available() == 0) return;

    String comando = Serial.readStringUntil('\n');
    comando.trim();
    comando.toUpperCase();

    // 1. Ignorar comentarios, líneas vacías y carácter %
    if (comando.length() == 0 || comando.startsWith("(") || comando.startsWith(";") || comando.startsWith("%"))
    {
        return;
    }

    // 2. Comandos de control estándar
    if (comando == "STOP") { detenerMotores(); Serial.println("OK: motores detenidos."); return; }
    if (comando == "STATUS") { mostrarEstado(); return; }

    // 3. Ignorar comandos M (Spindle / Fin de programa) enviando un "ok" de confirmación
    if (comando.startsWith("M"))
    {
        Serial.println("OK: comando M omitido.");
        return;
    }

    if (!sistemaSeguro())
    {
        Serial.println("ERROR: sistema en estado de emergencia.");
        return;
    }

    // 4. Limpiar prefijos de movimiento G (G0, G1, G00, G01, G02, G03)
    if (comando.startsWith("G0") || comando.startsWith("G1") || comando.startsWith("G2") || comando.startsWith("G3"))
    {
        int idxEspacio = comando.indexOf(' ');
        if (idxEspacio != -1) {
            comando = comando.substring(idxEspacio + 1);
        }
    }

    // 5. Verificar presencia de ejes (para diferenciar coordenadas X0 Y0 de un comando inválido)
    bool tieneX = (comando.indexOf('X') != -1);
    bool tieneY = (comando.indexOf('Y') != -1);
    bool tieneZ = (comando.indexOf('Z') != -1);

    if (!tieneX && !tieneY && !tieneZ) {
        Serial.println("ERROR: comando no valido.");
        return;
    }

    // 6. Extraer valores
    long pasosX = extraerPasos(comando, 'X');
    long pasosY = extraerPasos(comando, 'Y');
    long pasosZ = extraerPasos(comando, 'Z');

    bool dirX = pasosX >= 0;
    bool dirY = pasosY >= 0;
    bool dirZ = pasosZ >= 0;

    // Validación de límites
    if (tieneX && pasosX != 0 && dirX && limiteXActivo()) { Serial.println("ERROR: limite X activo."); return; }
    if (tieneY && pasosY != 0 && dirY && limiteYActivo()) { Serial.println("ERROR: limite Y activo."); return; }
    if (tieneZ && pasosZ != 0 && dirZ && limiteZActivo()) { Serial.println("ERROR: limite Z activo."); return; }

    // Ejecutar movimiento
    moverSimultaneo(labs(pasosX), dirX, labs(pasosY), dirY, labs(pasosZ), dirZ);

    Serial.println("OK: movimiento ejecutado.");
}