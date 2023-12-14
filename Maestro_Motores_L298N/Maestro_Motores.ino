//Librerias
#include "SR04.h"
#include <string.h>
#include "Wire.h"

//------------------------------------------------------
//      CONFIGURACIONES
//------------------------------------------------------

//Margen de cm entre mediciones para mostrar por pantalla
int Margen_US = 5;

//Distancia minima, media y maxima a tener en cuenta para la navegacion
int DistMin = 30;
int DistMed = 50;
int DistMax = 70;

//Velocidad minima, media y maxima para el movimiento
int VMin = 10;
int VMed = 30;
int VMax = 50;

// Configuracion del recorrido util de potencia del motor
int Pmin = 50; //potencia minima para mover el motor
int Pmax = 255; //Potencia maxima de salida
int Ptotal = Pmax - Pmin; //Difrencia entre min y max (Calculos internos)


//------------------------------------------------------
//      OTRAS VARIABLES
//------------------------------------------------------
//variable para cambiar entre modo maestro y esclavo
volatile bool modoMaestro = true;

//Valores por defecto vara variables de la funcion Ultrasonido()
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
  if (Velocidad != VelocidadAnterior || Giro != GiroAnterior || true){
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


void Ultrasonido () {
  Wire.requestFrom(8, 3);

  while (Wire.available()) {
    //Aqui llace el recuerdo del antiguo comprovador de distancias, descanse en paz
    //Actualmente el validador de datos se encuentra en el esclavo1 (ultrasonidos)
    /*
    if (((DistIOriginal - DistI_Anterior) > MargenDistUS) && (contI < IntentosUS)) {
      DistI = DistI_Anterior;
      contI++;
    }
    else{
      contI = 0;
      DistI = DistIOriginal;
    }*/
    
    //Aqui se reciven los datos de las mediciones
    DistI = Wire.read();
    DistM = Wire.read();
    DistD = Wire.read();


    int Marjen1 = DistI_Anterior - DistI;
    int Marjen2 = DistM_Anterior - DistM;
    int Marjen3 = DistD_Anterior - DistD;
    if (Marjen1 < 0) {
      Marjen1 = 0 - Marjen1;
    }
    if (Marjen2 < 0) {
      Marjen2 = 0 - Marjen2;
    }
    if (Marjen3 < 0) {
      Marjen3 = 0 - Marjen3;
    }
    if ((Margen_US < Marjen1) || (Margen_US < Marjen2) || (Margen_US < Marjen3)){
      /*
      DistI_Anterior = DistI;
      DistM_Anterior = DistM;
      DistD_Anterior = DistD;
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
      Serial.println("cm  ");*/
    }
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin(10);
  Wire.onRequest(EnvioDatos); //Funcion encargada del I2C
  Wire.onReceive(CambiarModo)
  
  Serial.print("Difrencia de potencia: ");
  Serial.println(Ptotal); //Difrencia entre Vmin y Vmax para calcular el voltaje util del motor
  pinMode(MotorI1, OUTPUT);
  pinMode(MotorD1, OUTPUT);
  pinMode(MotorI2, OUTPUT);
  pinMode(MotorD2, OUTPUT);
}

void EnvioDatos () {
  Wire.write(DistI);
  Wire.write(DistM);
  Wire.write(DistD);
  modoMaestro = !modoMaestro;
}

void CambiarModo ()  {
  modoMaestro = !modoMaestro;
}

void loop() {
  Ultrasonido();

  ///*
  if (DistM <= DistMin) {
      Motores(0, 0);
    }
  else if (DistI < DistMin){ 
    Motores(VMin, 30);
  }
  else if (DistD < DistMin){
    Motores(-VMin, -30);
  } //*/
  else { //Ajustamos la velocidad segun la distancia
    if (DistM <= DistMin) {
      Motores(0, 0);
      Serial.println("entro");
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
  delay(500);
  Serial.print("\n\n\n\n");
}
