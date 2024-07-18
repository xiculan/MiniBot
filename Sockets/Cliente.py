import socket
from pynput import keyboard
import time
import threading

# Crear un socket
mi_socket = socket.socket()

# Variable para controlar la conexión
conectar = "si"

# Intentar conectar al servidor repetidamente si falla
while conectar == "si":
    try:
        mi_socket.connect(("192.168.1.200", 8000))  # Intentar conectarse al servidor
        conectar = "echo"  # Si se conecta correctamente, cambiar el valor para salir del bucle
    except:
        print("Error conectando al servidor")
        for i in range(5):
            print("Intentando volver a conectar en: ", (5 - i))  # Esperar 5 segundos antes de intentar de nuevo
            time.sleep(1)

print("Conectado correctamente")

# Variable para controlar el estado de la tecla
pulsada = 0

# Función que se ejecuta en segundo plano para recibir mensajes del servidor
def recive_messages():
    while True:
        try:
            mensaje = mi_socket.recv(1024).decode('utf-8')  # Recibir mensajes del servidor
            if mensaje:
                print(f"{mensaje}")  # Mostrar mensaje recibido
        except:
            print("Error recibiendo mensaje")
            break  # Salir del bucle si hay un error

# Crear e iniciar el hilo para recibir mensajes
thread_recive = threading.Thread(target=recive_messages)
thread_recive.daemon = True  # Asegura que el hilo se cierra cuando el programa principal termine
thread_recive.start()

# Función para enviar mensajes al servidor
def send_message(message):
    try:
        message = message.encode('utf-8')  # Codificar el mensaje en UTF-8
        mi_socket.send(message)  # Enviar el mensaje
    except BrokenPipeError:
        print("Error: Broken pipe. No se pudo enviar el mensaje.")
    except Exception as e:
        print(f"Error enviando mensaje: {e}")

# Función que se ejecuta cuando se presiona una tecla
def on_press(key):
    global pulsada
    if pulsada == 0:  # Verifica si ninguna otra tecla está actualmente registrada como presionada
        try:
            if hasattr(key, 'char'):  # Verifica si la tecla tiene un carácter (es alfanumérica)
                send_message("{0}".format(key.char))  # Enviar el carácter presionado
            else:
                send_message("{0}".format(key))  # Enviar la tecla especial presionada
            pulsada = 1  # Marcar la tecla como presionada
        except Exception as e:
            print(f"Error procesando tecla presionada: {e}")

# Función que se ejecuta cuando se libera una tecla
def on_release(key):
    global pulsada
    pulsada = 0  # Restablecer el estado de la tecla
    if key == keyboard.Key.esc:  # Si la tecla liberada es ESC, detener el listener
        return False
    try:
        if hasattr(key, 'char'):  # Verifica si la tecla tiene un carácter (es alfanumérica)
            send_message("-{0}".format(key.char))  # Enviar el carácter liberado con un prefijo "-"
        else:
            send_message("-{0}".format(key))  # Enviar la tecla especial liberada con un prefijo "-"
    except Exception as e:
        print(f"Error procesando tecla liberada: {e}")

# Iniciar el listener del teclado para detectar eventos de teclas presionadas y liberadas
with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    print("Detecting keyboard")
    listener.join()  # Mantener el listener activo hasta que se detenga manualmente

# Cerrar el socket al terminar el programa
mi_socket.close()
