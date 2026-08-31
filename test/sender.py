import serial
import time

# Sustituye 'COM3' por el puerto real de tu Arduino Mega (ej. COM4, COM5)
PUERTO_SERIAL = 'COM3' 
BAUDRATE = 115200
ARCHIVO_GCODE = 'golfito.txt'

try:
    # Abrir conexión Serial con el Arduino
    arduino = serial.Serial(PUERTO_SERIAL, BAUDRATE, timeout=2)
    time.sleep(2) # Esperar a que el Arduino Mega se reinicie al conectar
    print(f"Conectado exitosamente a {PUERTO_SERIAL}")

    # Leer y enviar el archivo línea por línea
    with open(ARCHIVO_GCODE, 'r') as archivo:
        for linea in archivo:
            linea = linea.strip()
            
            # Omitir líneas vacías o comentarios
            if not linea or linea.startswith('(') or linea.startswith(';'):
                continue
            
            print(f"Enviando: {linea}")
            arduino.write(f"{linea}\n".encode('utf-8'))
            
            # Esperar confirmación del Arduino (ej. "OK: movimiento ejecutado.")
            respuesta = arduino.readline().decode('utf-8').strip()
            print(f"Arduino dice: {respuesta}")

    arduino.close()
    print("Envío de G-code completado.")

except Exception as e:
    print(f"Error: {e}")