//Librerias
#include "SR04.h"
//Variables
long DistI;
long DistM;
long DistD;
int Vmin = 130;
int Vmax = 255;
int Vtotal = Vmax - Vmin;
//Pines
#define TRIG_PIN1 3
#define ECHO_PIN1 2
SR04 SIzquierda = SR04(ECHO_PIN1,TRIG_PIN1);
#define TRIG_PIN2 5
#define ECHO_PIN2 4
SR04 SMedio = SR04(ECHO_PIN2,TRIG_PIN2);
#define TRIG_PIN3 7
#define ECHO_PIN3 6
SR04 SDerecha = SR04(ECHO_PIN3,TRIG_PIN3);

#define MotorI1 10
#define MotorI2 11
#define MotorD1 12
#define MotorD2 13



void Modulo_US(){
  DistI=SIzquierda.Distance();
  DistM=SMedio.Distance();
  DistD=SDerecha.Distance();
  Serial.print("Modulo_US// Frontal: ");
  Serial.print(DistM);
  Serial.print("cm    Izquierda: ");
  Serial.print(DistI);
  Serial.print("cm    Derecha: ");
  Serial.print(DistD);
  Serial.println("cm");
}

int Motores(int Velocidad, int Giro){
  int motorD = Velocidad;
  int motorI = Velocidad;
  Serial.println(Velocidad);
  if (Giro > 0) { //si el giro es a la derecha quito velocidad al motor derecho
    motorD = motorD - Giro;
  }
  if (Giro < 0){ //Si el giro es a la izquierda quito velocidad al motor izquierdo
    motorI = motorI - Giro;
  }
  Serial.print("Velocidad: ");
  Serial.println(Velocidad);
  Serial.print("Giro: ");
  Serial.println(Giro);
  Serial.print("Velocidad motor Derecho: ");
  Serial.println(motorD);
  Serial.print("Velocidad motor Izquierdo: ");
  Serial.println(motorI);

  if (Velocidad == 0) {
    motorI = 0;
    motorD = 0;
  }
  else {
    motorI = Vmin + (Vtotal * motorI / 100);
    motorD = Vmin + (Vtotal * motorD / 100);
  }

  if (motorI > 0){
    analogWrite(MotorI1, motorI);
    analogWrite(MotorI2, 0);
  }
  else if (motorI < 0){
    motorI = 0 - motorI;
    analogWrite(MotorI1, 0);
    analogWrite(MotorI2, motorI);
  }
  else if (motorI == 0){
    analogWrite(MotorI1, 0);
    analogWrite(MotorI2, 0);
  }
  if (motorD > 0){
    analogWrite(MotorD1, motorD);
    analogWrite(MotorD2, 0);
  }
  else if (motorD < 0){
    motorD = 0 - motorI;
    analogWrite(MotorD1, 0);
    analogWrite(MotorD2, motorI);
  }
  else if (motorD == 0){
    analogWrite(MotorD1, 0);
    analogWrite(MotorD2, 0);
  }
}


void setup() {
  Serial.begin(9600);
  Serial.print("Difrencia de potencia");
  Serial.println(Vtotal); //Difrencia entre Vmin y Vmax para calcular el voltaje util del motor
  pinMode(MotorI1, OUTPUT);
  pinMode(MotorD1, OUTPUT);
  pinMode(MotorI2, OUTPUT);
  pinMode(MotorD2, OUTPUT);

}

void loop() {
  
  Modulo_US();
  
  if (DistM <= 15) {
    Motores(0, 0);
    while (DistM < 15) {
      Serial.println("-----------------------------------------------------------------------");
      Modulo_US();
      delay(500);
    }
  }
  else if(DistM < 100) {
    Motores(DistM, 0);
  }
  else {
    Motores (100, 0);
    while (DistM > 100) {
      Serial.println("-----------------------------------------------------------------------");
      Modulo_US();
      delay(500);
    }
  }
  Serial.println("-----------------------------------------------------------------------");
  delay(200);
}




















