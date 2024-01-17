import socket
from pynput import keyboard
import time

mi_socket = socket.socket()
conectar = "si"
while conectar == "si":
    try:
        mi_socket.connect(("192.168.1.234", 8000))
        conectar = "echo"
    except:
        print("Error conectando al servidor")
        for i in range(5):
            print("intentando volver a conectar en: ", (5 - i))
            time.sleep(1)
print("conectado correctamente")
pulsada = 0

def send_message(message):
    message = message.encode('utf-8')
    mi_socket.send(message)

def on_press(key):
    global pulsada
    if (pulsada == 0):
        try:
            print("Alphanumeric key pressed: {0} ".format(key.char))
            send_message("{0}".format(key.char))
            pulsada = 1
        except:
            print("Tecla no reconocida")
    
def on_release(key):
    global pulsada
    pulsada = 0
    print("Key released: {0}".format(key))
    if key == keyboard.Key.esc:
        # Stop listener
        return False
    try:
        send_message("-{0}".format(key.char))
    except:
        print("Caracter no reconocido")
with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    print("Detecting keyboard")
    listener.join()

mi_socket.close()
