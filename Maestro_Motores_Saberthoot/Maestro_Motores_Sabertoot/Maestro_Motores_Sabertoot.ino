//Librerias
#include "SR04.h"
#include "Wire.h"
#include <Servo.h>

//------------------------------------------------------
//      CONFIGURACIONES
//------------------------------------------------------

//Margen de cm entre mediciones para mostrar por pantalla
int Margen_US = 5;

//Distancia minima, media y maxima a tener en cuenta para la navegacion
int DistMin = 10;
int DistMed = 30;
int DistMax = 50;


//Velocidad minima, media y maxima para el movimiento
int VMin = 10;
int VMed = 30;
int VMax = 50;


//------------------------------------------------------
//      OTRAS VARIABLES
//------------------------------------------------------

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

//Configuracion de recorrido util de los motores (controlados como servo)
int PMotoresMin = 0;
int PMotoresMax = 160;
int PVelCal = 0; //Calibra la salida por que el eje esta descentrado
int PGirCal = 0; //Calibrar la salida de giro

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

int Motores(int Velocidad, int Giro){

  
  int SalidaVel = map(Velocidad, -100, 100, PMotoresMin, PMotoresMax);
  SalidaVel = SalidaVel + PVelCal; //Correccion sobre el eje de la velocidad
  int SalidaGir = map(Giro, -100, 100, PMotoresMin, PMotoresMax);
  SalidaGir = SalidaGir + PGirCal; //Correccion sobre el eje del giro

  if (Velocidad == 0){
    VelocidadMotores.write(60);
  }
  else{
    VelocidadMotores.write(SalidaVel);
  }
  
  GiroMotores.write(SalidaGir);

  //----------------------------------------------------------------------------
  //SALIDA POR PANTALLA /*
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
  } //*/  
  //----------------------------------------------------------------------------

}


void Ultrasonido () {
  Wire.requestFrom(8, 3);

  while (Wire.available()) {
    
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
      
    }
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin(10);
  Wire.onRequest(EnvioDatos); //Funcion encargada del I2C
  VelocidadMotores.attach(10);
  GiroMotores.attach(11); 
}

void EnvioDatos () {
  Wire.write(DistI);
  Wire.write(DistM);
  Wire.write(DistD);
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