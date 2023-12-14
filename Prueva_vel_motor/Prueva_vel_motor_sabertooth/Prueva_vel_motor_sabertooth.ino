#include <Servo.h>

int data = 0;
int vel = 0;


//------------------------------------------------------
//      SALIDAS DIGITALES
//------------------------------------------------------
Servo myservo;


int Motores(int Velocidad, int Giro) {
  
  myservo.write(map(Velocidad, -100, 100, 0, 180));
  

  Serial.print("\n\n\n\n");
  Serial.println("--------------------------------------------");
  Serial.print("MODULO MOTORES//  ");
  Serial.print(" Velocidad: ");
  Serial.print(Velocidad);
  Serial.print("/");
  Serial.print(map(Velocidad, -100, 100, 0, 180));
  Serial.print("  Giro: ");
  Serial.print(Giro);
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  myservo.attach(10);
}

void loop() {
  if (Serial.available()) {
    data = Serial.parseInt();
  }
  if (data != 0) {
    vel = data;
  }
  Motores(vel, 0);
  delay(500);
}
