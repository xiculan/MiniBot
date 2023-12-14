#define Motor 11
#define V 13
int Potencia = 0;

void setup() {
  pinMode(Motor, OUTPUT);
  pinMode(V, OUTPUT);
  digitalWrite(Motor, HIGH);
  Serial.begin(9600);
}
void loop (){
  while (Potencia < 250){
    Potencia += 1;
    analogWrite(V, Potencia);
    Serial.println(Potencia);
    delay(200);
  }
  Serial.println("Fin del programa");
  delay(2000);
  Potencia = 0;
  
}
