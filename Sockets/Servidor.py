import socket
import time
from smbus2 import SMBus

#########################
## Variables
#########################
Arduino = 31  # Conexión con Arduino por I2C

# Definimos el socket
mi_socket = socket.socket()
# Definimos la conexion (host, puerto)
mi_socket.bind(('192.168.1.200', 8000))
# Número de conexiones máximas (max)
mi_socket.listen(1)

print("Esperando conexión...")

conexion, addr = mi_socket.accept()  # Aquí se aceptan las conexiones
print("Nueva conexión establecida")
print(addr)

def enviar_mensaje(mensaje):
    try:
        mensaje = mensaje.encode('utf-8')
        conexion.send(mensaje)  # Usar 'conexion' en lugar de 'mi_socket'
    except Exception as e:
        print(e)

def envio_arduino(dato):
    try:
        with SMBus(1) as bus:
            bus.write_i2c_block_data(Arduino, 0, list(dato))
    except Exception as e:
        print(e)
        enviar_mensaje(str(e))  # Enviar el error como mensaje
    return

def leer_arduino():
    try:
        with SMBus(1) as bus:
            data = bus.read_i2c_block_data(Arduino, 0, 3)  # Lee 3 bytes de datos
            return data
    except Exception as e:
        print(e)
        return None

data = ""
# Empezamos un bucle infinito
while True:
    # Recibe datos del cliente
    try:
        datos_recibidos = conexion.recv(1024)
    except:
        print("Conexión perdida. Esperando nueva conexión")
        conexion, addr = mi_socket.accept()  # Aquí se aceptan las conexiones
        print("Reconectando a: ")
        print(addr)
        datos_recibidos = conexion.recv(1024)

    # Muestra los datos recibidos por pantalla
    data = datos_recibidos.decode('utf-8')
    comando = None
    mensaje = None

    # Aquí podemos añadir acciones
    if data in ["w", "Key.up"]:
        mensaje = "Adelante"
        comando = "w\n".encode()
    elif data in ["s", "Key.down"]:
        mensaje = "Atrás"
        comando = "s\n".encode()
    elif data in ["d", "Key.left"]:
        mensaje = "Derecha"
        comando = "d\n".encode()
    elif data in ["a", "Key.right"]:
        mensaje = "Izquierda"
        comando = "a\n".encode()
    elif data in ["-w", "-s", "-a", "-d", "-Key.up", "-Key.down", "-Key.left", "-Key.right"]:
        mensaje = "Fin movimiento"
        comando = "0\n".encode()
    else:
        mensaje = data

    # Comprobamos si se recibió un comando
    if comando is not None:
        envio_arduino(comando)

    # Comprobamos si tenemos un mensaje por mostrar
    if mensaje is not None:
        enviar_mensaje(mensaje)
        print(mensaje + "\n")
    
    # Leer datos de Arduino y mostrarlos por pantalla
    arduino_data = leer_arduino()
    if arduino_data:
        dist_i, dist_m, dist_d = arduino_data
        print(f"Distancias desde Arduino: Izquierda={dist_i} cm, Medio={dist_m} cm, Derecha={dist_d} cm")
        enviar_mensaje(f"Distancias: Izquierda={dist_i} cm, Medio={dist_m} cm, Derecha={dist_d} cm")

conexion.close()  # Cerramos la conexión
