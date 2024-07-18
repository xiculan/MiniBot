#include "SR04.h"
#include "Wire.h"
#include <Servo.h>

String inputString = "";
bool stringComplete = false;

//------------------------------------------------------
//      CONFIGURACIONES
//------------------------------------------------------

//Margen de cm entre mediciones para mostrar por pantalla
int Margen_US = 5;

//Distancia minima, media y maxima a tener en cuenta para la navegacion
int DistMin = 20;
int DistMed = 60;
int DistMax = 100;

//Velocidad minima, media y maxima para el movimiento
int VMin = 17;
int VMed = 20;
int VMax = 23;

//Configuracion de recorrido util de los motores (controlados como servo)
int PMotoresMin = 0;
int PMotoresMax = 160;
int PVelCal = 3; //Calibra la salida por que el eje esta descentrado
int PGirCal = 13; //Calibrar la salida de giro
int ejeMotor = 85;

//direcciones i2c
int Ardu = 8; //Primer esclavo, encargado de los ultrasonidos
int Master = 31;

//------------------------------------------------------
//      OTRAS VARIABLES
//------------------------------------------------------

//Valores por defecto para variables de la función Ultrasonido()
int DistI_Anterior = 0;
int DistM_Anterior = 0;
int DistD_Anterior = 0;

//Variables SOLO ENTRADA de datos de ultrasonido (no modificar valores en programa)
int DistI;   //Distancia Ultrasonido izquierdo
int DistM;   //Distancia ultrasonido derecho
int DistD;   //Distancia ultrasonido medio

//inicializacion de variables para funcion Motores()
int VelocidadAnterior = 0;
int GiroAnterior = 0;

int numeroRecibido = 0;
int RC = 0;

//------------------------------------------------------
//      SALIDAS DIGITALES
//------------------------------------------------------

// Definimos los pines de los motores
Servo VelocidadMotores;
Servo GiroMotores;

/*
Modulo de control de los motores:
Este controla la velocidad de los motores a izquierda y derecha.
esta pensado para llevar 2 motores
Acepta variables Velocidad y giro en % (de 0 a 100)
Se encarga del calculo para conseguir que los motores funcionen correctamente
Cambiando Pmin y Pmax se ajusta el recorrido util de la potencia del motor
*/

void Motores(int Velocidad, int Giro){
  int SalidaVel = map(Velocidad, -100, 100, PMotoresMin, PMotoresMax);
  SalidaVel = SalidaVel + PVelCal; //Correccion sobre el eje de la velocidad
  int SalidaGir = map(Giro, -100, 100, PMotoresMin, PMotoresMax);
  SalidaGir = SalidaGir + PGirCal; //Correccion sobre el eje del giro
  if (Velocidad == 0){
    VelocidadMotores.write(ejeMotor);
    SalidaVel = ejeMotor;
  }
  else{
    VelocidadMotores.write(SalidaVel);
  }
  GiroMotores.write(SalidaGir);

  //SALIDA POR PANTALLA
  if (Velocidad != VelocidadAnterior || Giro != GiroAnterior || true){
    VelocidadAnterior = Velocidad;
    GiroAnterior = Giro;   
    Serial.println("--------------------------------------------");
    Serial.print("MODULO MOTORES//  ");
    Serial.print(" Velocidad: ");
    Serial.print(Velocidad);
    Serial.print("%/");
    Serial.print(SalidaVel);
    Serial.print("  Giro: ");
    Serial.print(Giro);
    Serial.print("%/");
    Serial.println(SalidaGir);

    Serial.println("****************************************");
    Serial.print("Modulo_US// ");
    Serial.print("   US izc: "); 
    Serial.print(DistI);
    Serial.print("cm");
    Serial.print(" ---- US Med: ");
    Serial.print(DistM);
    Serial.print("cm");
    Serial.print(" ---- US Der: ");
    Serial.print(DistD);
    Serial.println("cm  ");
  } 
}

void Ultrasonido () {
  Wire.requestFrom(Ardu, 3);

  if (Wire.available() >= 3) {
    //Aqui se reciben los datos de las mediciones
    DistI = Wire.read();
    DistM = Wire.read();
    DistD = Wire.read();

    int Marjen1 = abs(DistI_Anterior - DistI);
    int Marjen2 = abs(DistM_Anterior - DistM);
    int Marjen3 = abs(DistD_Anterior - DistD);
    
    if ((Margen_US < Marjen1) || (Margen_US < Marjen2) || (Margen_US < Marjen3)){
      // Realiza alguna acción si el margen es superado
    }
    DistI_Anterior = DistI;
    DistM_Anterior = DistM;
    DistD_Anterior = DistD;
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin(Master);
  VelocidadMotores.attach(10);
  GiroMotores.attach(11);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(EnvioDatos);
  delay(5000);
}

void EnvioDatos() {
  Wire.write(DistI);
  Wire.write(DistM);
  Wire.write(DistD);
}

int EnRango(int valor, int min, int max){
  if (valor > max){
    valor = max;
  }
  if (valor < min){
    valor = min;
  }
  return valor;
}

// Aquí se reciben datos desde la Raspberry
void receiveEvent(int howMany) {
  inputString = "";
  while(Wire.available() > 0) { // Mientras tengamos caracteres en el buffer
    char inChar = (char)Wire.read();
    if (inChar == '\n') {
      stringComplete = true;
    }
    else {
      inputString += inChar;
    }
  }
  if (stringComplete) {
    inputString.remove(0,1);
    Serial.println(inputString);
    if (inputString == "w") {
      Motores(VMax, 0);
      RC = 1;
    }
    else if (inputString == "s") {
      Motores(-VMax, 0);
      RC = 1;
    }
    else if (inputString == "d") {
      Motores(0, VMax);
      RC = 1;
    }
    else if (inputString == "a") {
      Motores(0, -VMax);
      RC = 1;
    }
    else if (inputString == "0") {
      Motores(0, 0);
      RC = 1;
    }
    else if (inputString == "fin") {
      Motores(0, 0);
      RC = 0;
    }
  }
}

void loop() {
  Ultrasonido();
  if (RC == 0){
    Serial.print("\n\n\n\n");
    if (DistM <= DistMin) {
      Motores(0, 0);
    }
    else if (DistI < (DistMin / 2)){ 
      Motores(0, -30);
    }
    else if (DistD < (DistMin / 2)){
      Motores(0, 30);
    }
    else { //Ajustamos la velocidad según la distancia
      if (DistM <= DistMin) {
        Motores(0, 0);
      }
      else if (DistM < DistMed) {
        Motores(VMin, 0);
      }
      else if (DistM < DistMax) {
        Motores(VMed, 0);
      }
      else {
        Motores(VMax, 0);
      }
    }
  }
  else { 
    Serial.println("entro");
    if (DistI <= DistMin || DistM <= DistMin || DistD <= DistMin){
      Motores(0, 0);
    }
  }
  delay(300);
}
