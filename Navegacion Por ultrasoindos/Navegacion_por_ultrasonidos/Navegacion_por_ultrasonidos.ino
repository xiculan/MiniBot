//Librerias
#include "SR04.h"
//Variables
long DistI;
long DistM;
long DistD;
int Vmin = 130;
int Vmed = 200;
int Vmax = 255;
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

void Modulo_Velocidad(){
  if (DistM < 25){
    analogWrite(MotorI1, Vmin);
    analogWrite(MotorD1, Vmin);
    Serial.println("Modulo Velocidad// Adelante min");
  }
  else if (DistM < 35){
    analogWrite(MotorI1, Vmed);
    analogWrite(MotorD1, Vmed);
    Serial.println("Modulo Velocidad// Adelante med");
  }
  else if (DistM >= 35){
    analogWrite(MotorI1, Vmax);
    analogWrite(MotorD1, Vmax);
    Serial.println("Modulo Velocidad// Adelante max");
  }
  analogWrite(MotorI1, 0);
  analogWrite(MotorD1, 0);
  
}

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

void Modulo_Giro(){
  if (DistM < 15){
    analogWrite(MotorI1, 0);
    analogWrite(MotorD1, 0);
    if (DistI > DistD){
      Serial.println("Modulo Giro// Girando Izquierda");
      analogWrite(MotorI2, Vmin);
      analogWrite(MotorD1, Vmin);
      delay(300);    // Provisional
      analogWrite(MotorD1, 0);
      analogWrite(MotorI2, 0);
    }
    else if (DistI <= DistD){
      Serial.println("Modulo Giro// Girando Derecha");
      analogWrite(MotorI1, Vmin);
      analogWrite(MotorD2, Vmin);
      delay(300);    // Provisional
      analogWrite(MotorD2, 0);
      analogWrite(MotorI1, 0);
    }
    else{
      Serial.print("Modulo Giro// ");
      Serial.println(DistM);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(MotorI1, OUTPUT);
  pinMode(MotorD1, OUTPUT);
  pinMode(MotorI2, OUTPUT);
  pinMode(MotorD2, OUTPUT);

}

void loop() {
  
  Modulo_US();
  if (DistM < 15){
    Modulo_Giro();
  }
  else{
  Modulo_Velocidad();
  }
  delay(200);
}




















