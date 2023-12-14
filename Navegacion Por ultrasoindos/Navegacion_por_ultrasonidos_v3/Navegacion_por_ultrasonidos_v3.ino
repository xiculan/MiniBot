//Librerias
#include "SR04.h"
//Variables
long DistI;   //Distancia Ultrasonido izquierdo
long DistM;   //Distancia ultrasonido derecho
long DistD;   //Distancia ultrasonido medio
int Pmin = 130; //potencia minima para mover el motor
int Pmax = 255; //Potencia maxima de salida
int Ptotal = Pmax - Pmin; //Difrencia entre min y max
int Margen_US = 3;
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

#define MotorI1 10  //Motor Izquierdo Adelante
#define MotorI2 11  //Motor Izquierdo Atras
#define MotorD1 12  //Motor Derecho Adelante
#define MotorD2 13  //Motor Derecho atras

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
  /*
  Serial.print("Velocidad: ");
  Serial.println(Velocidad);
  Serial.print("Giro: ");
  Serial.println(Giro);
  Serial.print("Velocidad motor Derecho: ");
  Serial.println(motorD);
  Serial.print("Velocidad motor Izquierdo: ");
  Serial.println(motorI); */
  //----------------------------------------------------------------------------

  if (Velocidad == 0) { //Si la velocidad es 0% ajusta la potencia a 0
    motorI = 0;
    motorD = 0;
  }
  else { //si no la ajusta segun el recorrido util del motor
    motorI = Pmin + (Ptotal * motorI / 100);
    motorD = Pmin + (Ptotal * motorD / 100);
  }
  //Ajustamos la potencia. Si es negativa sale por el pin de marxa atras, si no por el pin de marxa alante
  //Ajuste salida motor izquierdo
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
  //Ajuste salida Motor derecho
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
  Serial.print("Difrencia de potencia: ");
  Serial.println(Ptotal); //Difrencia entre Vmin y Vmax para calcular el voltaje util del motor
  pinMode(MotorI1, OUTPUT);
  pinMode(MotorD1, OUTPUT);
  pinMode(MotorI2, OUTPUT);
  pinMode(MotorD2, OUTPUT);

}

void loop() {
  
  Modulo_US1(); //Actualizamos las distancias: DistI, DistM, DistD
  pruebas();
  if (DistM <= 15) { //si se acerca a menos de 15 cm para completamente
    Motores(0, 0);
  }
  else if(DistM < 100) { //Hasta los 100 cm regula la velocidad segun la distancia
    Motores(DistM, 0);
  }
  else { //A partir de 100 cm velocidad maxima
    Motores (100, 0);
  }
  //Serial.println("-----------------------------------------------------------------------");
  delay(10 *  DistM);
}




















