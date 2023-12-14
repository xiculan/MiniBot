//Librerias
#include "SR04.h"
//Variables
long DistI;   //Distancia Ultrasonido izquierdo
long DistM;   //Distancia ultrasonido derecho
long DistD;   //Distancia ultrasonido medio
int Pmin = 100; //potencia minima para mover el motor
int Pmax = 255; //Potencia maxima de salida
int Ptotal = Pmax - Pmin; //Difrencia entre min y max
int Margen_US = 10;
int DistMin = 15;
int DistMed = 30;
int DistMax = 60;
int VMin = 30;
int VMed = 60;
int VMax = 100;
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
void Modulo_US1(){  
  int DistM_Anterior = DistM;
  DistM=SMedio.Distance();
  int Marjen = DistM_Anterior - DistM;
  if (Marjen < 0) {
    Marjen = 0 - Marjen;
  }
  if (Margen_US < Marjen ){
    Serial.print("Modulo_US// ");
    Serial.print("Frontal: ");
    Serial.print(DistM);
    Serial.println("cm  ");
  }
}
void Modulo_US3() {  
  int DistM_Anterior = DistM;
  DistM=SMedio.Distance();
  int Marjen = DistM_Anterior - DistM;
  if (Marjen < 0) {
    Marjen = 0 - Marjen;
  }
  if (Margen_US < Marjen ){
    Serial.print("Modulo_US// ");
    Serial.print("Frontal: ");
    Serial.print(DistM);
    Serial.println("cm  ");
  }
  if (DistM < 20) {
    DistI=SIzquierda.Distance();
    DistD=SDerecha.Distance();

    Serial.print("Izquierda: ");
    Serial.print(DistI);
    Serial.print("cm  ");
    Serial.print("Derecha: ");
    Serial.print(DistD);
    Serial.println("cm  ");
  }
}

void pruebas () {
  DistM=SMedio.Distance();
  DistI=SIzquierda.Distance();
  DistD=SDerecha.Distance();

  Serial.print("Sensores// ");
  Serial.print("Frontal: ");
  Serial.print(DistM);
  Serial.print("cm  ");
  Serial.print("Izquierda: ");
  Serial.print(DistI);
  Serial.print("cm  ");
  Serial.print("Derecha: ");
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
  Serial.println(VIzquierda);
  Serial.println(VDerecha);
  
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
  
  Serial.print("Velocidad: ");
  Serial.println(Velocidad);
  Serial.print("Giro: ");
  Serial.println(Giro);
  Serial.print("Velocidad motor Derecho: ");
  Serial.println(motorD);
  Serial.print("Velocidad motor Izquierdo: ");
  Serial.println(motorI); 
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

}

void loop() {
  Motores(100, 50);
  delay(3000);
  Motores(-100, 0);
  delay(3000);
  Motores(0, -50);
  delay(3000);
  
}




















