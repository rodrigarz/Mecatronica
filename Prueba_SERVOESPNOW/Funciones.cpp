#include "Funciones.h"
#include <Arduino.h>
#include "Globales.h"
#include "WiFi.h"
#include "Servo.h"
#include <esp_now.h>
#include <iostream>


struct struct_message myData;
Servo servo1;
String success;
esp_now_peer_info_t peerInfo;

ESP32Encoder myEnc;

int test_limits = 2; // De rotary Encoder
double Kp, Ki, Kd;
double Setpoint, Input, Output;
double velMotor;

PID myPID(&Input, &Output, &Setpoint, myData.Kp, Ki, Kd, DIRECT);


void movimiento()
{
    switch (myData.indicacion)
    {
    case 1:
        posicionServo();
        myData.indicacion = 0;
        break;

    case 2:
        posicionExpulsor();
        myData.indicacion = 0;
        break;

    case 3:
        pasosPasoPaso();
        myData.indicacion = 0;
        break;

    case 4:
        posicionPasoPaso();
        myData.indicacion = 0;
        break;

    default:
        break;
    }
}

void posicionServo()
{
    int servoPosAnt = 0;
    int servoPosition = map(myData.posServo, 0, 180, 0, 180);
    servoPosAnt = servoPosition;
    servo1.write(servoPosition);
    if (servoPosAnt != servoPosition)
    {
        Serial.println(servoPosition);
    }
    delay(100);
}

void posicionExpulsor()
{
    if (myData.posExpulsor == 1)
    {
        servo1.write(180);
        delay(200);
        servo1.write(0);
        myData.posExpulsor = 0;
    }
}

void pasosPasoPaso()
{
    if (myData.gradosPaP > 0)
    {
        mueveMotor(myData.gradosPaP * steps / 360);
    }
    else
    {
        myData.gradosPaP = myData.gradosPaP * -1;
        mueveMotorB(myData.gradosPaP * steps / 360);
    }
}

void posicionPasoPaso()
{
    if (myData.posPap == 1) {
        mueveMotor(steps / 4);
        delay(1000);
        mueveMotorB(steps / 4);
    }
    else if (myData.posPap == 2) {
        mueveMotor(steps / 2);
        delay(1000);
        mueveMotorB(steps / 2);
    }
    else if (myData.posPap == 3) {
        mueveMotor(steps * 3 / 4);
        delay(1000);
        mueveMotorB(steps * 3 / 4);
    }
}


void mueveMotor(int stepsToMove) {
    digitalWrite(dirPin, HIGH); //Definimos sentido
    stepDelay = round((0.1125 * 1000000) / (2 * myData.velPap));///definidmos velocidad

    for (int x = 0; x < stepsToMove; x++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(stepDelay);
    }
}

void mueveMotorB(int stepsToMove) {
    digitalWrite(dirPin, LOW);
    stepDelay = round((0.1125 * 1000000) / (2 * myData.velPap));

    for (int x = 0; x < stepsToMove; x++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(stepDelay);
    }
}


void inicializa()
{
    Serial.begin(115200);
    Serial.println(WiFi.macAddress());
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    else
    {
        Serial.print("Todo ok");
    }

    esp_now_register_recv_cb(OnDataRecv);
    pinMode(dirPin, OUTPUT);
    pinMode(stepPin, OUTPUT);
    pinMode(EN, OUTPUT);
    digitalWrite(EN, LOW);
    servo1.attach(servoPin);

    ESP32Encoder::useInternalWeakPullResistors = UP;
    myEnc.attachHalfQuad(23, 19);
    myEnc.setCount(0);
    myEnc.clearCount();

    setupMovement();                // Incializacion Pines Motor
    myPID.SetSampleTime(100);

    myPID.SetOutputLimits(-255, 255);
    myPID.SetMode(AUTOMATIC);

    escribeLcd(stringEstado[sys.estado], stringControl[sys.control] + ": " + stringEntrada[sys.entrada]);
    if (sys.entrada == REFERENCIA) {
        Setpoint = 0;

        myEnc.clearCount();
    }
    else {
        Setpoint = myData.setPoint;

        myEnc.clearCount();
    }
}


void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    //Serial.print("Bytes received: ");
    //Serial.println(len);
    Serial.print("Char: ");
    Serial.println(myData.posServo);
    Serial.println();
    movimiento();
}