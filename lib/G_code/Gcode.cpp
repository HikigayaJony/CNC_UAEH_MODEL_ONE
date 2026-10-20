#include <Arduino.h>

#include "Gcode.h"
#include "Motores.h"
#include "Seg.h"
#include "Configuracion.h"
#include "Comun.h"

// Estado de g-code
bool modoAbsoluto = true;
float feedrateActual = DEFAULT_FEEDRATE;


bool obtenerParametro(const String& linea, char letra, float& valor)
{
    int posicion = linea.indexOf(letra);
    if (posicion == -1)
    {
        return false;
    }

    int inicio = posicion + 1;
    int fin = inicio;

    
    while (fin < (int)linea.length())
    {
        char c = linea.charAt(fin);
        if (isDigit(c) || c == '.' || c == '-' || c == '+')
        {
            fin++;
        }
        else
        {
            break; 
        }
    }

    if (inicio == fin)
    {
        return false;
    }

    String numero = linea.substring(inicio, fin);
    valor = numero.toFloat();
    return true;
}

// Inicio
void gcode_init()
{
    modoAbsoluto = true;
    feedrateActual = DEFAULT_FEEDRATE;
}


void procesarGCode(const String& entrada)
{
    String linea = entrada;
    linea.trim();
    linea.toUpperCase();

    if (linea.length() == 0)
    {
        return;
    }

    // Filtrar comentarios
    int comentario = linea.indexOf(';');
    if (comentario != -1)
    {
        linea = linea.substring(0, comentario);
        linea.trim();
    }

    if (linea.length() == 0)
    {
        return;
    }

    // Consulta de estado interno
    if (linea == "ESTADO")
    {
        mostrarEstado();
        return;
    }

    // Verificar interruptor de emergencia
    if (emergenciaActiva())
    {
        Serial.println("error:Emergencia activa");
        return;
    }

    // Comando G90 (Modo Absoluto)
    if (linea.startsWith("G90"))
    {
        modoAbsoluto = true;
        Serial.println("ok");
        return;
    }

    // Comando G91 (Modo Relativo)
    if (linea.startsWith("G91"))
    {
        modoAbsoluto = false;
        Serial.println("ok");
        return;
    }

    // Comando G92 (Fijar Posición)
    if (linea.startsWith("G92"))
    {
        float x = obtenerX();
        float y = obtenerY();
        float z = obtenerZ();
        float val = 0;

        if (obtenerParametro(linea, 'X', val)) x = val;
        if (obtenerParametro(linea, 'Y', val)) y = val;
        if (obtenerParametro(linea, 'Z', val)) z = val;

        establecerPosicion(x, y, z);
        Serial.println("ok");
        return;
    }

    // Comandos G0 / G1 (Movimiento)
    if (linea.startsWith("G0") || linea.startsWith("G1"))
    {
        
        float x = obtenerX();
        float y = obtenerY();
        float z = obtenerZ();

        bool esG0 = linea.startsWith("G0");
        float feedrateComando = esG0 ? MAX_FEEDRATE : feedrateActual;

        
        float f;
        if (obtenerParametro(linea, 'F', f))
        {
            if (f > 0)
            {
                feedrateActual = f;
                if (!esG0) feedrateComando = f;
            }
        }

        
        float parametro = 0;

        if (obtenerParametro(linea, 'X', parametro))
        {
            x = modoAbsoluto ? parametro : (obtenerX() + parametro);
        }

        if (obtenerParametro(linea, 'Y', parametro))
        {
            y = modoAbsoluto ? parametro : (obtenerY() + parametro);
        }

        if (obtenerParametro(linea, 'Z', parametro))
        {
            z = modoAbsoluto ? parametro : (obtenerZ() + parametro);
        }

        
        if (!moverA(x, y, z, feedrateComando))
        {
            Serial.println("error:MOVE_FAILED");
        }

        return;
    }

    // Comando M0 (Pausa)
    if (linea.startsWith("M0"))
    {
        detenerMotores();
        Serial.println("ok");
        return;
    }

    // Comandos M2 / M30 (Fin de Programa)
    if (linea.startsWith("M2") || linea.startsWith("M30"))
    {
        detenerMotores();
        deshabilitarDrivers();
        modoAbsoluto = true;
        Serial.println("ok");
        return;
    }

    // Comando HELP
    if (linea == "AYUDA" || linea == "HELP")
    {
        Serial.println("=== COMANDOS CNC UAEH ===");
        Serial.println("ESTADO: brinda información sobre el estado actual de la máquina");
        Serial.println("HELP/AYUDA: muestra esta lista de comandos");
        Serial.println("G0 X- Y- Z-   -> genera un movimiento rapido");
        Serial.println("G1 X- Y- Z- F- -> utiliza interpolacion lineal");
        Serial.println("G90 - modo absoluto");
        Serial.println("G91 - modo relativo");
        Serial.println("G92 X- Y- Z- -> establece la posición actual");
        Serial.println("=========================");
        return;
    }

    // Comando no reconocido
    Serial.println("error:COMANDO NO RECONOCIDO");
}