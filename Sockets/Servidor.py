##Librerias
import socket
import time
from smbus2 import SMBus
#########################
##Variables
#########################
Arduino = 31 #conexion con arduino por i2c


#definimos el socket
mi_socket = socket.socket()
#definimos la conexion (host, puerto)
mi_socket.bind(('192.168.1.234', 8000))
#numero de conexiones maximas (max)
mi_socket.listen(1)

print ("Esperando conexion...")

conexion, addr = mi_socket.accept() #Aqui se aceptan las conexiones
print ("Nueva conexion establecida")
print (addr)

mensaje = "Conexion establecida"
mensaje = mensaje.encode('utf-8')
conexion.send(mensaje) #envia un mensaje al usuario

def envio_arduino (dato):
    try:
        with SMBus(1) as bus:
            bus.write_i2c_block_data(Arduino, 0, dato)
    except:
        print("No se pudo conectar al arduino")
    return
data = ""
#emprezamos un bucle infinito
while data != "q":

    # Recibe datos del cliente
    datos_recibidos = conexion.recv(1024)

    # Verifica si se recibieron datos
    if not datos_recibidos:
        print("El cliente ha cerrado la conexión.")
        break

    # Muestra los datos recibidos por pantalla
    data = datos_recibidos.decode('utf-8')
    print (data)
    if (data == "w"):
        print("Adelante")
        comando = "w\n".encode()
        envio_arduino(comando)

    elif (data == "s"):
        print("Atras")
        comando = "s\n".encode()
        envio_arduino(comando)
    print()

conexion.close() #Cerramos la conexion


