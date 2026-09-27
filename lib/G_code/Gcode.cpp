#include <Arduino.h>

#include "Gcode.h"
#include "Motores.h"
#include "Seg.h"
#include "Configuracion.h"
#include "Comun.h"


// Estado de g-code

bool modoAbsoluto = true;

float feedrateActual = DEFAULT_FEEDRATE;



// Extraer parametro


bool obtenerParametro(const String& linea,char letra,float& valor)
{
    int posicion = linea.indexOf(letra);

    if (posicion == -1)
    {
        return false;
    }


    int inicio = posicion + 1;

    int fin = linea.indexOf(' ', inicio);

    if (fin == -1)
    {
        fin = linea.length();
    }


    String numero = linea.substring(inicio, fin);

    valor = numero.toFloat();

    return true;
}

//Inicio

void gcode_init()
{
    modoAbsoluto = true;

    feedrateActual = DEFAULT_FEEDRATE;
}


// PROCESAMIENTO

void procesarGCode(const String& entrada)
{
    String linea = entrada;

    linea.trim();

    linea.toUpperCase();


    if (linea.length() == 0)
    {
        return;
    }


    // comentarios

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


    // Estado

    if (linea == "STATUS")
{
    mostrarEstado();
    return;
}
    // Emergencia

    if (emergenciaActiva())
    {
        Serial.println("error:EMERGENCY");

        return;
    }


    // comando G90

    if (linea.startsWith("G90"))
    {
        modoAbsoluto = true;

        Serial.println("ok");

        return;
    }


    // comando G91

    if (linea.startsWith("G91"))
    {
        modoAbsoluto = false;

        Serial.println("ok");

        return;
    }


    //comando G92

    if (linea.startsWith("G92"))
    {
        float x = obtenerX();
        float y = obtenerY();
        float z = obtenerZ();

        obtenerParametro(linea, 'X', x);
        obtenerParametro(linea, 'Y', y);
        obtenerParametro(linea, 'Z', z);

        establecerPosicion(x, y, z);

        Serial.println("ok");

        return;
    }


    // comando G0 / G1

    if (linea.startsWith("G0") ||
        linea.startsWith("G1"))
    {
        float x = obtenerX();
        float y = obtenerY();
        float z = obtenerZ();


        //feedrate

        float f;

        if (obtenerParametro(linea, 'F', f))
        {
            if (f > 0)
            {
                feedrateActual = f;
            }
        }


        //coordenadas

        float parametro;


        if (obtenerParametro(linea, 'X', parametro))
        {
            if (modoAbsoluto)
                x = parametro;
            else
                x += parametro;
        }


        if (obtenerParametro(linea, 'Y', parametro))
        {
            if (modoAbsoluto)
                y = parametro;
            else
                y += parametro;
        }


        if (obtenerParametro(linea, 'Z', parametro))
        {
            if (modoAbsoluto)
                z = parametro;
            else
                z += parametro;
        }


        // Ejecutar

        if (moverA(x, y, z, feedrateActual))
        {
            
        }
        else
        {
            Serial.println("error:MOVE");
        }

        return;
    }


    // Comando M0

    if (linea.startsWith("M0"))
    {
        detenerMotores();

        Serial.println("ok");

        return;
    }


    // Comandos M2 / M30

    if (linea.startsWith("M2") ||
        linea.startsWith("M30"))
    {
        detenerMotores();

        Serial.println("ok");

        return;
    }

    // comando de comandos(?)
    if (linea == "HELP")
{
    Serial.println("=== COMANDOS CNC UAEH ===");
    Serial.println("STATUS");
    Serial.println("HELP");
    Serial.println("G0 X.. Y.. Z..");
    Serial.println("G1 X.. Y.. Z.. F..");
    Serial.println("G90");
    Serial.println("G91");
    Serial.println("G92 X.. Y.. Z..");
    Serial.println("=========================");
    return;
}

    // Desconocido

    Serial.println("error:UNKNOWN_COMMAND");
}