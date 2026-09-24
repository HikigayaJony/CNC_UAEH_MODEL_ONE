#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Incluimos la librería del LCD I2C

#include "Comun.h"
#include "Motores.h"
#include "Sensores.h"
#include "Seg.h"
#include "Gcode.h"
#include "Configuracion.h"

// Objeto global de la pantalla LCD (Dirección I2C 0x27, 16 columnas x 2 filas)
static LiquidCrystal_I2C lcd(0x27, 16, 2);

static String bufferSerial = "";

// Carácter personalizado para barra llena (Bloque sólido de 5x8 píxeles)
static byte bloqueLleno[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

// Función para actualizar la pantalla LCD 16x2 desde las tramas enviadas por C#
void actualizarPantallaLCD(String trama)
{
    // Trama esperada desde C#: "LCD|ESTADO|PORCENTAJE" (ejemplo: "LCD|DIBUJAN|G-CODE|45")
    int primerPipe = trama.indexOf('|');
    int segundoPipe = trama.indexOf('|', primerPipe + 1);

    if (primerPipe != -1 && segundoPipe != -1)
    {
        String estado = trama.substring(primerPipe + 1, segundoPipe);
        int porcentaje = trama.substring(segundoPipe + 1).toInt();

        if (porcentaje < 0) porcentaje = 0;
        if (porcentaje > 100) porcentaje = 100;

        // --- RENGLÓN 1: Estado del Sistema / Conexión (Rellenado a 16 espacios) ---
        lcd.setCursor(0, 0);
        String linea1 = estado;
        while (linea1.length() < 16) linea1 += " ";
        lcd.print(linea1.substring(0, 16));

        // --- RENGLÓN 2: Porcentaje y ProgressBar [████░░░░░] ---
        lcd.setCursor(0, 1);
        
        // Mapeo a 9 bloques asignados para la barra gráfica
        int bloques = map(porcentaje, 0, 100, 0, 9);

        // Formato de porcentaje a 4 caracteres exactos ("  0%", " 45%", "100%")
        String txtPct = String(porcentaje) + "%";
        if (porcentaje < 10) txtPct = "  " + txtPct;
        else if (porcentaje < 100) txtPct = " " + txtPct;

        lcd.print(txtPct); // Usa 4 columnas
        lcd.print(" [");    // Usa 2 columnas

        // Dibujar los 9 caracteres de la barra
        for (int i = 0; i < 9; i++)
        {
            if (i < bloques)
            {
                lcd.write(byte(0)); // Bloque sólido personalizado
            }
            else
            {
                lcd.print(" ");
            }
        }
        lcd.print("]"); // Usa 1 columna. Total: 4 + 2 + 9 + 1 = 16 caracteres
    }
}

// Inicio de la comunicación serie y pantalla LCD
void comunicacion_init()
{
    

    // Inicializar pantalla LCD I2C y registrar el carácter personalizado
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, bloqueLleno);

    // Mensaje de Bienvenida al encender el sistema
    lcd.setCursor(0, 0);
    lcd.print("  CNC UAEH 2026 ");
    lcd.setCursor(0, 1);
    lcd.print("DESCONECTADO... ");


    Serial.begin(SERIAL_BAUDRATE);

    bufferSerial = "";

    Serial.println("CNC UAEH READY");
    Serial.println("Firmware: 1.0");
}



// ESTADO GENERAL DE SENSORES Y EJES
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

    if (xMinActivo() || xMaxActivo())
    Serial.println("ACTIVO");
    else
    Serial.println("OK");

    Serial.print("LIMITE Y: ");

    if (yMinActivo() || yMaxActivo())
    Serial.println("ACTIVO");
    else
    Serial.println("OK");

    Serial.print("LIMITE Z: ");
    if (zMinActivo() || zMaxActivo())
    Serial.println("ACTIVO");
    else
    Serial.println("OK");

    Serial.print("POSICION ACTUAL: X=");
    Serial.print(obtenerX(), 4);

    Serial.print(" Y=");
    Serial.print(obtenerY(), 4);

    Serial.print(" Z=");
    Serial.println(obtenerZ(), 4);

    Serial.println("------------------");
    Serial.println();
}




void comunicacion_update()
{
    while (Serial.available() > 0)
    {
        char caracter = Serial.read();


        // fin de la linea

        if (caracter == '\n')
        {
            if (bufferSerial.length() > 0)
            {
                procesarGCode(bufferSerial);

                bufferSerial = "";
            }
        }


        // ignora el cr

        else if (caracter == '\r')
        {
            // No hacer nada
        }


       // acumular

        else
        {
            bufferSerial += caracter;


            // Protección contra líneas gigantes
            if (bufferSerial.length() > 120)
            {
                bufferSerial = "";

                Serial.println("error:LINE_TOO_LONG");
            }
        }
    }
}