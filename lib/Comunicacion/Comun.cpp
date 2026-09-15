#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> // Incluimos la librería del LCD I2C

#include "Comun.h"
#include "Motores.h"
#include "Sensores.h"
#include "Seg.h"

// Objeto global de la pantalla LCD (Dirección I2C 0x27, 16 columnas x 2 filas)
static LiquidCrystal_I2C lcd(0x27, 16, 2);

static float posAbsX = 0.0f;
static float posAbsY = 0.0f;
static float posAbsZ = 0.0f;

// true = G90 (absoluto), false = G91 (relativo). G90 es el modo por defecto.
static bool modoAbsoluto = true;

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

// INICIALIZACIÓN DE LA COMUNICACIÓN Y COMPONENTES
void comunicacion_init()
{
    Serial.begin(115200);

    // Inicializar pantalla LCD I2C y registrar el carácter personalizado
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, bloqueLleno);

    // Mensaje de Bienvenida al encender el sistema
    lcd.setCursor(0, 0);
    lcd.print("  CNC UAEH 2026 ");
    lcd.setCursor(0, 1);
    lcd.print("DESCONECTADO... ");

    Serial.println();
    Serial.println("=================================");
    Serial.println(" CNC UAEH - FIRMWARE");
    Serial.println(" Arduino Mega + A4988 + LCD");
    Serial.println("=================================");
    Serial.println("Sistema iniciado.");
    Serial.println("Comandos:");
    Serial.println("G90         -> modo absoluto (posicion destino)");
    Serial.println("G91         -> modo relativo (desplazamiento)");
    Serial.println("X100        -> mover/posicionar X 100");
    Serial.println("X-100       -> mover/posicionar X -100");
    Serial.println("Y100        -> mover/posicionar Y 100");
    Serial.println("Z100        -> mover/posicionar Z 100");
    Serial.println("G92 X0 Y0 Z0 -> fijar posicion actual como origen");
    Serial.println("STOP        -> detener");
    Serial.println("STATUS      -> estado del sistema");
    Serial.println();
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
    if (limiteXActivo()) Serial.println("ACTIVO");
    else Serial.println("OK");

    Serial.print("LIMITE Y: ");
    if (limiteYActivo()) Serial.println("ACTIVO");
    else Serial.println("OK");

    Serial.print("LIMITE Z: ");
    if (limiteZActivo()) Serial.println("ACTIVO");
    else Serial.println("OK");

    Serial.print("MODO: ");
    Serial.println(modoAbsoluto ? "G90 (ABSOLUTO)" : "G91 (RELATIVO)");

    Serial.print("POSICION ACTUAL: X=");
    Serial.print(posAbsX, 4);
    Serial.print(" Y=");
    Serial.print(posAbsY, 4);
    Serial.print(" Z=");
    Serial.println(posAbsZ, 4);

    Serial.println("------------------");
    Serial.println();
}

// Extrae el valor numérico (con decimales) asociado a un eje
float extraerValorEje(String texto, char eje, bool &presente)
{
    int idx = texto.indexOf(eje);
    if (idx == -1) { presente = false; return 0.0f; }

    int idxFin = idx + 1;
    while (idxFin < (int)texto.length() &&
           (isDigit(texto[idxFin]) || texto[idxFin] == '-' || texto[idxFin] == '.'))
    {
        idxFin++;
    }

    presente = true;
    return texto.substring(idx + 1, idxFin).toFloat();
}

// PROCESAMIENTO DE COMUNICACIÓN SERIE Y G-CODE
void procesarComunicacion()
{
    if (Serial.available() == 0) return;

    String comando = Serial.readStringUntil('\n');
    comando.trim();

    // Captura prioritaria de comandos LCD enviados desde la aplicación en C#
    if (comando.startsWith("LCD|"))
    {
        actualizarPantallaLCD(comando);
        return;
    }

    comando.toUpperCase();

    // 1. Ignorar comentarios, líneas vacías y carácter %
    if (comando.length() == 0 || comando.startsWith("(") || comando.startsWith(";") || comando.startsWith("%"))
    {
        return;
    }

    // 2. Comandos de control estándar
    if (comando == "STOP") { detenerMotores(); Serial.println("OK: motores detenidos."); return; }
    if (comando == "STATUS") { mostrarEstado(); return; }

    // 3. Cambio de modo G90 / G91
    if (comando.startsWith("G90"))
    {
        modoAbsoluto = true;
        if (comando.indexOf('X') == -1 && comando.indexOf('Y') == -1 && comando.indexOf('Z') == -1)
        {
            Serial.println("OK: modo G90 (absoluto).");
            return;
        }
    }
    else if (comando.startsWith("G91"))
    {
        modoAbsoluto = false;
        if (comando.indexOf('X') == -1 && comando.indexOf('Y') == -1 && comando.indexOf('Z') == -1)
        {
            Serial.println("OK: modo G91 (relativo).");
            return;
        }
    }

    // 4. G92: fijar la posición actual como origen (o el valor indicado)
    if (comando.startsWith("G92"))
    {
        bool tieneXg92, tieneYg92, tieneZg92;
        float vx = extraerValorEje(comando, 'X', tieneXg92);
        float vy = extraerValorEje(comando, 'Y', tieneYg92);
        float vz = extraerValorEje(comando, 'Z', tieneZg92);

        if (tieneXg92) posAbsX = vx;
        if (tieneYg92) posAbsY = vy;
        if (tieneZg92) posAbsZ = vz;

        Serial.println("OK: origen fijado (G92).");
        return;
    }

    // 5. Ignorar comandos M (Spindle / Fin de programa) enviando un "ok"
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

    // 6. Limpiar prefijos de movimiento G
    if (comando.startsWith("G0") || comando.startsWith("G1") ||
        comando.startsWith("G2") || comando.startsWith("G3") ||
        comando.startsWith("G90") || comando.startsWith("G91"))
    {
        int idxEspacio = comando.indexOf(' ');
        if (idxEspacio != -1) {
            comando = comando.substring(idxEspacio + 1);
        }
    }

    // 7. Verificar presencia de ejes
    bool tieneX, tieneY, tieneZ;
    float valorX = extraerValorEje(comando, 'X', tieneX);
    float valorY = extraerValorEje(comando, 'Y', tieneY);
    float valorZ = extraerValorEje(comando, 'Z', tieneZ);

    if (!tieneX && !tieneY && !tieneZ) {
        Serial.println("ERROR: comando no valido.");
        return;
    }

    // 8. Calcular el DELTA real de movimiento por eje
    float deltaX = 0.0f, deltaY = 0.0f, deltaZ = 0.0f;

    if (modoAbsoluto)
    {
        if (tieneX) deltaX = valorX - posAbsX;
        if (tieneY) deltaY = valorY - posAbsY;
        if (tieneZ) deltaZ = valorZ - posAbsZ;
    }
    else
    {
        if (tieneX) deltaX = valorX;
        if (tieneY) deltaY = valorY;
        if (tieneZ) deltaZ = valorZ;
    }

    long pasosX = lround(fabs(deltaX));
    long pasosY = lround(fabs(deltaY));
    long pasosZ = lround(fabs(deltaZ));

    bool dirX = deltaX >= 0;
    bool dirY = deltaY >= 0;
    bool dirZ = deltaZ >= 0;

    // 9. Validación de límites
    if (tieneX && pasosX != 0 && dirX && limiteXActivo()) { Serial.println("ERROR: limite X activo."); return; }
    if (tieneY && pasosY != 0 && dirY && limiteYActivo()) { Serial.println("ERROR: limite Y activo."); return; }
    if (tieneZ && pasosZ != 0 && dirZ && limiteZActivo()) { Serial.println("ERROR: limite Z activo."); return; }

    // 10. Ejecutar movimiento simultáneo de motores
    moverSimultaneo(pasosX, dirX, pasosY, dirY, pasosZ, dirZ);

    // 11. Actualizar la posición absoluta DESPUÉS de mover
    if (modoAbsoluto)
    {
        if (tieneX) posAbsX = valorX;
        if (tieneY) posAbsY = valorY;
        if (tieneZ) posAbsZ = valorZ;
    }
    else
    {
        if (tieneX) posAbsX += valorX;
        if (tieneY) posAbsY += valorY;
        if (tieneZ) posAbsZ += valorZ;
    }

    Serial.println("OK: movimiento ejecutado.");
}