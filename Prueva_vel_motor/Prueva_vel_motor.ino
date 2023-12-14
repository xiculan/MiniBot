//Librerias
#include "Wire.h"

//------------------------------------------------------
//      CONFIGURACIONES
//------------------------------------------------------

// Configuracion del recorrido util de potencia del motor
int Pmin = 0; //potencia minima para mover el motor
int Pmax = 255; //Potencia maxima de salida
int Ptotal = Pmax - Pmin; //Difrencia entre min y max (Calculos internos)


//------------------------------------------------------
//      OTRAS VARIABLES
//------------------------------------------------------

//inicializacion de variables para funcion Motores()
int VelocidadAnterior = 0;
int GiroAnterior = 0;

int data = 0;
int vel = 0;

//------------------------------------------------------
//      SALIDAS DIGITALES
//------------------------------------------------------

// Definimos los pines de los motores
#define MotorI1 8   //Motor Izquierdo Adelante
#define MotorI2 9   //Motor Izquierdo Atras
#define MotorIV 10  //Potencia motor
#define MotorD1 11  //Motor Derecho Adelante
#define MotorD2 12  //Motor Derecho atras
#define MotorDV 13  //Potencia motor

/*
Modulo de control de los motores:
Este controla la velocidad de los motores a izquierda y derecha.
esta pensado para llevar 2 motores
Acepta variables Velocidad y giro en % (de 0 a 100)
Se encarga del calculo para conseguir que los motores funcionen correctamente
Cambiando Pmin y Pmax se ajusta el recorrido util de la potencia del motor
*/

int Motores(int Velocidad, int Giro){
  //Una variable por motor (regula la velocidad por separado)
  int motorD = Velocidad;
  int motorI = Velocidad;
  if (Giro > 0) { //si el giro es a la derecha quito velocidad al motor derecho
    motorD = motorD - Giro;
  }
  if (Giro < 0){ //Si el giro es a la izquierda quito velocidad al motor izquierdo
    motorI = motorI - Giro;
  }
  
  //----------------------------------------------------------------------------
  //SALIDA POR PANTALLA /*
  if (Velocidad != VelocidadAnterior || Giro != GiroAnterior){
    VelocidadAnterior = Velocidad;
    GiroAnterior = Giro;   
  
    Serial.print("\n\n\n\n");
    Serial.println("--------------------------------------------");
    Serial.print("MODULO MOTORES//  ");
    Serial.print(" Velocidad: ");
    Serial.print(Velocidad);
    Serial.print("  Giro: ");
    Serial.print(Giro);
    Serial.print(" ---- mot izq: ");
    Serial.print(motorD);
    Serial.print(" ----  mot der: ");
    Serial.println(motorI);
  } //*/  
  //----------------------------------------------------------------------------

  if (motorI == 0) { //Si la velocidad es 0% ajusta la potencia a 0 en motor izquierdo
    motorI = 0;
  }
  else {
    if (motorI > 0){
      motorI = Pmin + (Ptotal * motorI / 100);
    }
    else{
      motorI = -Pmin + (Ptotal * motorI / 100);
    }
  }

  if (motorD == 0) { //Si la velocidad es 0% ajusta la potencia a 0 en motor derecho
    motorD = 0;
  }
  else {
     if (motorD > 0){
      motorD = Pmin + (Ptotal * motorD / 100);
    }
    else{
      motorD = -Pmin + (Ptotal * motorD / 100);
    }
  }
    
  Motores_Salida(motorI, motorD);
}

int Motores_Salida(int VIzquierda, int VDerecha) {
  
  if(VIzquierda > 0) {
    digitalWrite(MotorI1, HIGH);
    digitalWrite(MotorI2, LOW);
    analogWrite(MotorIV, VIzquierda);
  }
  else if(VIzquierda < 0) {
    VIzquierda = 0 - VIzquierda;
    digitalWrite(MotorI1, LOW);
    digitalWrite(MotorI2, HIGH);
    analogWrite(MotorIV, VIzquierda);
  }
  else {
    digitalWrite(MotorI1, LOW);
    digitalWrite(MotorI2, LOW);
    analogWrite(MotorIV, 0);
  }
  if(VDerecha > 0) {
    digitalWrite(MotorD1, HIGH);
    digitalWrite(MotorD2, LOW);
    analogWrite(MotorDV, VDerecha);
  }
  else if(VDerecha < 0) {
    VDerecha = 0 - VDerecha;
    digitalWrite(MotorD1, LOW);
    digitalWrite(MotorD2, HIGH);
    analogWrite(MotorDV, VDerecha);
    
  }
  else {
    digitalWrite(MotorD1, LOW);
    digitalWrite(MotorD2, LOW);
    analogWrite(MotorDV, 0);
  }
}


void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  Serial.print("Difrencia de potencia: ");
  Serial.println(Ptotal); //Difrencia entre Vmin y Vmax para calcular el voltaje util del motor
  pinMode(MotorI1, OUTPUT);
  pinMode(MotorD1, OUTPUT);
  pinMode(MotorI2, OUTPUT);
  pinMode(MotorD2, OUTPUT);
}

void loop() {
  if (Serial.available())
   {
      data = Serial.parseInt();
   }
   if (data != 0){
    vel =  data;
   }
  Motores(vel, 0);
  delay(500);
}
