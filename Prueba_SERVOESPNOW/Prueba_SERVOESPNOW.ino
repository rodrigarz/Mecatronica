#include "WiFi.h"
#include <esp_now.h>
#include <Servo.h>

const int dirPin = 5;
const int stepPin = 15;
const int EN = 18;
const int steps = 200*16;
int stepDelay = 0;
int posServo = 0;
String success;
esp_now_peer_info_t peerInfo;

typedef struct struct_message {
  int indicacion; //1 = posServo, 2 = posExpulsor, 3 = pasosPap, 4 = posPap
  int posServo;
  int posExpulsor;
  int pasosPap;
  int posPap;
}struct_message;

struct struct_message myData;

static const int servoPin = 19;

Servo servo1;

 
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  //Serial.print("Bytes received: ");
  //Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.posServo);
  Serial.println();
}

void movimiento()
{
  switch(myData.indicacion)
  {
    case 1:
    posicionServo();
    break;
  
    case 2:
    posicionExpulsor();
    break;

    case 3:
    pasosPasoPaso();
    break;

    case 4:
    posicionPasoPaso();
    break;
  }
}

void posicionServo()
{
  int servoPosAnt = 0;
  int servoPosition = map(myData.posServo, 0, 180, 0, 180);
  servoPosAnt = servoPosition;
  servo1.write(servoPosition);
  if(servoPosAnt != servoPosition)
  {
    Serial.println(servoPosition);
  }
  delay(100);
}

void posicionExpulsor()
{
  if(myData.posExpulsor == 1)
  {
    servo1.write(180);
    delay(200);
    servo1.write(0);
    myData.posExpulsor = 0;
  }
}

void pasosPasoPaso()
{
  mueveMotor(myData.pasosPap);
}

void posicionPasoPaso()
{
  if (myData.posPap == 1) {
    mueveMotor(steps/4);
    delay(1000);
    mueveMotorB(steps/4);
  } else if (myData.posPap == 2) {
    mueveMotor(steps/2);
    delay(1000);
    mueveMotorB(steps/2);
  } else if (myData.posPap == 3) {
    mueveMotor(steps*3/4);
    delay(1000);
    mueveMotorB(steps*3/4);
  }
}


void mueveMotor(int stepsToMove) {
  digitalWrite(dirPin, HIGH); //Definimos sentido
  stepDelay=50;///definidmos velocidad

  for (int x = 0; x < stepsToMove; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
}

void mueveMotorB(int stepsToMove) {
  digitalWrite(dirPin, LOW);
  stepDelay=50;

  for (int x = 0; x < stepsToMove; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println(WiFi.macAddress());
  servo1.attach(servoPin);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else
  {
    Serial.print("Todo ok");
  }
  esp_now_register_recv_cb(OnDataRecv);
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);
}
void loop() {
  // put your main code here, to run repeatedly:
  movimiento();
  

}
