/*
Este es el programa del primer esclavo del protocolo i2c del minibot.
Sera el encargado del modulo de ultrasonidos.
Se encargara de Leer y trasmitir los datos al arduino maestro
Arduino maestro == Arduino uno
Arduino esclavo == Arduino nano
*/

//Aqui tenemos las librerias que vamos a utilizar
#include "SR04.h"
#include "Wire.h"

//------------------------------------------------------
//      CONFIGURACIONES
//------------------------------------------------------

//Referido al error maximo y los intentos a realizar en mediciones en la funcion  comprovador_error_ultrasons()
int errorMax = 100; //esta variable es %
int intentos = 3;

//------------------------------------------------------
//      OTRAS VARIABLES
//------------------------------------------------------

//Variables de control de distancia
int DistI = 0;    //Distancia Ultrasonido izquierdo
int DistI1 = 0;   //Ultimo valor valido
int DistI2 = 0;   //Penultimo valor valido

int DistM = 0;   //Distancia ultrasonido derecho
int DistM1 = 0;
int DistM2 = 0;

int DistD = 0;   //Distancia ultrasonido medio
int DistD1 = 0;
int DistD2 = 0;

//Contadores utilizados en la funcion comprovador_error_ultrasons()
int contadorIntentosI = 0;
int contadorIntentosM = 0;
int contadorIntentosD = 0;

//------------------------------------------------------
//      ENTRADAS DIGITALES
//------------------------------------------------------

//Pines de los sensores de ultrasonidos SR04
#define TRIG_PIN1 2       //Sensor izquierdo
#define ECHO_PIN1 3
SR04 SIzquierda = SR04(ECHO_PIN1,TRIG_PIN1);
#define TRIG_PIN2 4       //Sensor central
#define ECHO_PIN2 5
SR04 SMedio = SR04(ECHO_PIN2,TRIG_PIN2);
#define TRIG_PIN3 7       //Sensor derecho
#define ECHO_PIN3 8
SR04 SDerecha = SR04(ECHO_PIN3,TRIG_PIN3);

//------------------------------------------------------
//------------------------------------------------------

void setup() {
  Serial.begin(9600);   //puerto de serie (salida datos a ordenador)
  Wire.begin(8);        //Numero de esclavo (I2C)
  Wire.onRequest(EnvioDatos); //Funcion encargada del I2C

  //Damos los primeros valores para que tenga un arranque rapido el sistema (comentar si da errores)
  DistI1 = SDerecha.Distance();
  DistM1 = SDerecha.Distance();
  DistD1 = SDerecha.Distance();
  delay (300);
}

void EnvioDatos () {
  Wire.write(DistI1);
  Wire.write(DistM1);
  Wire.write(DistD1);
  //Serial.println("Mensaje saliente");
  
}

//int comprovador_error_ultrasons (int Dist1, int Dist2, int Dist3, int numero_sensor){
int comprovador_error_ultrasons (int Dist1, int Dist2, int Dist3, int sensor){
  int media = (Dist1 + Dist2 + Dist3) / 3;
  int calc = Dist1 - media;
  if (calc < 0){
    calc = 0 - calc;
  }
  int error = (errorMax * media) / 100;
  
  if (sensor == 1){
    if ((calc > error) && (contadorIntentosI < intentos) && (Dist1 > 30)){
      informe(Dist1, Dist2, Dist3, sensor, media, Dist1, error);
      contadorIntentosI = contadorIntentosI + 1;
      return Dist2;
    }
    else{
      informe(Dist1, Dist2, Dist3, sensor, media, 0 , error);
      contadorIntentosI = 0;
      return Dist1;
    }
  }

  else if (sensor == 2){
    if ((calc > error) && (contadorIntentosM < intentos) && (Dist1 > 30)){
      informe(Dist1, Dist2, Dist3, sensor, media, Dist1, error);
      contadorIntentosM = contadorIntentosM + 1;
      return Dist2;
    }
    else{
      informe(Dist1, Dist2, Dist3, sensor, media, 0, error);
      contadorIntentosM = 0;
      return Dist1;
    }
  }

  else if (sensor == 3){
    if ((calc > error) && (contadorIntentosD < intentos) && (Dist1 > 30)){
      informe(Dist1, Dist2, Dist3, sensor, media, Dist1, error);
      contadorIntentosD = contadorIntentosD + 1;
      return Dist2;
    }
    else{
      informe(Dist1, Dist2, Dist3, sensor, media, 0, error);
      contadorIntentosD = 0;
      return Dist1;
    }
  }
  
}
int informe (int Valor1, int Valor2, int Valor3, int sensor, int media, int eliminado, int margen){
  return; //Comentar aqui para desactivar informa <========================================================= INFORME AQUI
  if (sensor == 1){
    Serial.print("Ultrasonido Izc// ");
  }
  if (sensor == 2){
    Serial.print("Ultrasonido Med// ");
  }
  if (sensor == 3){
    Serial.print("Ultrasonido Der// ");
  }
  
  Serial.print(" Val 1: ");
  Serial.print(Valor1);
  Serial.print("cm");
  Serial.print(" || Val 2: ");
  Serial.print(Valor2);
  Serial.print("cm");
  Serial.print(" || Val 3: ");
  Serial.print(Valor3);
  Serial.print("cm  ");
  Serial.print(" || Margen: ");
  Serial.print(margen);
  Serial.print(" || Media: ");
  if (eliminado > 0) {
    Serial.print(media);
    Serial.print(" || Eliminado: ");
    Serial.println(eliminado);
  }
  else{
    Serial.println(media);
  }
  
}
void loop() {
  DistI2 = DistI1;

  //
  DistI1 = comprovador_error_ultrasons(DistI, DistI1, DistI2, 1);
  DistI = SIzquierda.Distance();

  //medicion_sensor_us_izquierdo=valor_sensor_us_izquierdo_corregido();
  //medicion_sensor_izquierdo=
  
  
  DistM2 = DistM1;
  DistM1 = comprovador_error_ultrasons(DistM, DistM1, DistM2, 2);
  DistM = SMedio.Distance();

  DistD2 = DistD1;
  DistD1 = comprovador_error_ultrasons(DistD, DistD1, DistD2, 3);
  DistD = SDerecha.Distance();
  /*
  inf marge_error_rel
  int valor_corretgit
  int contador
  //esquema algoritme

  //agafo tres valors
  valor1=valor distancia
  delay 100
  valor2=valor distancia
  delay 100
  valor3=valor distancia
  delay 100
  

  //calucul mitjana
  valor_mig=valor1+valor2+valor3/3

marge_error_abs=valor_mig*marge_error_rel

  dif1=valor1-valor_ig
si marge_erro_abs>dif1   
contador++
valor_corretgit=valor_corretgit+valor1

  dif2=valor1-valor_ig
si marge_erro_abs>dif1   
contador++
valor_corretgit=valor_corretgit+valor1

  dif3=valor1-valor_ig
si marge_erro_abs>dif1   
contador++
valor_corretgit=valor_corretgit+valor1


valor_corretgit=valor_corretgit/contador



  */


  delay(300);
  //Serial.println(); //Diferencia entre salidas

  /*
  Serial.println("=============================================================");
  Serial.print("Modulo_US// ");
  Serial.print("   US izc: ");
  Serial.print(DistI1);
  Serial.print("cm");
  Serial.print(" ---- US Med: ");
  Serial.print(DistM1);
  Serial.print("cm");
  Serial.print(" ---- US Der: ");
  Serial.print(DistD1);
  Serial.println("cm  ");
  //*/
}
