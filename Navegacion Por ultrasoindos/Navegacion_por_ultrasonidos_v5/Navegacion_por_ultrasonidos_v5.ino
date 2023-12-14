//Librerias
#include "SR04.h"
#include <string.h>
using namespace std; 
//Variables
long DistI;   //Distancia Ultrasonido izquierdo
long DistM;   //Distancia ultrasonido derecho
long DistD;   //Distancia ultrasonido medio
int Pmin = 100; //potencia minima para mover el motor
int Pmax = 255; //Potencia maxima de salida
int Ptotal = Pmax - Pmin; //Difrencia entre min y max

int DistM_Anterior = 0;
int Margen_US = 5;

int DistMin = 20;
int DistMed = 40;
int DistMax = 60;   

int VMin = 1;
int VMed = 20;
int VMax = 100;

int VelocidadAnterior = 0;
int GiroAnterior = 0;


//Pines
#define TRIG_PIN1 3       //Sensor izquierdo
#define ECHO_PIN1 2
SR04 SIzquierda = SR04(ECHO_PIN1,TRIG_PIN1);
#define TRIG_PIN2 5       //Sensor central
#define ECHO_PIN2 4
SR04 SMedio = SR04(ECHO_PIN2,TRIG_PIN2);
#define TRIG_PIN3 7       //Sensor derecho
#define ECHO_PIN3 6
SR04 SDerecha = SR04(ECHO_PIN3,TRIG_PIN3);


//#define Boton 1
#define MotorI1 8  //Motor Izquierdo Adelante
#define MotorI2 9  //Motor Izquierdo Atras
#define MotorIV 10
#define MotorD1 11  //Motor Derecho Adelante
#define MotorD2 12  //Motor Derecho atras
#define MotorDV 13
/*
Modulo de ultrasonidos. Este se encarga de actualizar las variables globales
Imprime por pantalla la distancia detectada por los 3 sensores
*/

void Modulo_US() {  
  int DistM_Anterior = DistM;
  DistM=SMedio.Distance();
  DistI=SIzquierda.Distance();
  DistD=SDerecha.Distance();
  
  int Marjen = DistM_Anterior - DistM;
  if (Marjen < 0) {
    Marjen = 0 - Marjen;
  }
  if (Margen_US < Marjen ){
    DistM_Anterior = DistM;
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

void pruebas () {
  //Serial.println("****************************************");
  //Serial.println("MODULO ULTRASONIDOS");
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
/*
Modulo de control de los motores:
Este controla la velocidad de los motores a izquierda y derecha.
esta pensado para llevar 2 motores
Acepta variables Velocidad y giro en % (de 0 a 100)
Se encarga del calculo para conseguir que los motores funcionen correctamente
Cambiando Pmin y Pmax se ajusta el recorrido util de la potencia
*/

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


int Motores(int Velocidad, int Giro){
  int motorD = Velocidad;
  int motorI = Velocidad;
  if (Giro > 0) { //si el giro es a la derecha quito velocidad al motor derecho
    motorD = motorD - Giro;
  }
  if (Giro < 0){ //Si el giro es a la izquierda quito velocidad al motor izquierdo
    motorI = motorI - Giro;
  }
  
  //----------------------------------------------------------------------------
  //SALIDA POR PANTALLA 

  
  if (Velocidad != VelocidadAnterior || Giro != GiroAnterior){
    VelocidadAnterior = Velocidad;
    GiroAnterior = Giro;
    
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
    //Serial.println("-----------------------------------------");
    pruebas ();
  }
  
  //----------------------------------------------------------------------------

  if (motorI == 0) { //Si la velocidad es 0% ajusta la potencia a 0
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

  if (motorD == 0) { //Si la velocidad es 0% ajusta la potencia a 0
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


void setup() {
  Serial.begin(9600);
  Serial.print("Difrencia de potencia: ");
  Serial.println(Ptotal); //Difrencia entre Vmin y Vmax para calcular el voltaje util del motor
  pinMode(MotorI1, OUTPUT);
  pinMode(MotorD1, OUTPUT);
  pinMode(MotorI2, OUTPUT);
  pinMode(MotorD2, OUTPUT);
  //pinMode(Boton, INPUT_PULLUP);
}

void loop() {
  Modulo_US(); //Actualizamos las distancias: DistI, DistM, DistD
  //pruebas();
  
  //Posivilidad de añadir boton de pause
  /*if (digitalRead(Boton) == HIGH) {
    delay(2000);
    int esperar = 1;
    while (esperar == 1) {
      delay(1000);
      if (digitalRead(Boton) == HIGH) {
        esperar = 0; //fin de la espera
      }
    }
  }*/

  if (DistI < DistMin){
    Motores(0, 30);
  }
  else if (DistD < DistMin){
    Motores(0, -30);
  }
  else {
    if (DistM <= DistMin) {
      if (DistI >= DistD) {
        Motores(0, 0);
      }
      if (DistI < DistD) {
        Motores(0, 0);
      }
    }
    else if(DistM < DistMed) {
      Motores(VMin, 0);
    }
    else if(DistM < DistMax) {
      Motores(VMed, 0);
    }
    else {
      Motores (VMax, 0);
    }
  }
  delay(300);
}




















