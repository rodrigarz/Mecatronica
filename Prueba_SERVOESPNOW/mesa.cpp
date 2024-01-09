#include "esp32-hal.h"
#include "mesa.h"

//volatile int stepDelay = 0;
int gradosActualMesa = 0;
const int steps = 200*16;

void inicializaMesa()
{
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);
  gradosActualMesa = 0;
}

void pasosPasoPaso()
{
    if (gradosActualMesa + myData.gradosPaP > 300 || gradosActualMesa + myData.gradosPaP < -300)
    {
        Serial.println("No se puede mover la mesa mas distancia");
        return;
    }

    gradosActualMesa = gradosActualMesa + myData.gradosPaP;

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

void posicionPasoPaso(int posicion, bool modoAuto)
{
    if (posicion == 1) {
        Serial.println("Hola");
        if (gradosActualMesa + (steps / 4)/8.89 > 300)
        {
            Serial.println("No se puede mover la mesa mas distancia");
            return;
        }
        Serial.println("Antes de mover");
        mueveMotor(steps / 4);
        if (modoAuto)
        {
            posicionExpulsor(2);
        }
        unsigned long int tactual = millis();
         while(millis()-tactual < 1000)
        {
          
	      }

        tactual = millis();
        mueveMotorB(steps / 4);
    }
    else if (posicion == 2) {
        
        if (gradosActualMesa + (steps / 2)/8.89 > 300)
        {
            Serial.println("No se puede mover la mesa mas distancia");
            return;
        }
        mueveMotor(steps / 2);
        if (modoAuto)
        {
            posicionExpulsor(2);
        }
        delay(1000);
        mueveMotorB(steps / 2);
    }
    else if (posicion == 3) {

        if (gradosActualMesa + (steps * (3 / 4))/8.89 > 300)
        {
            Serial.println("No se puede mover la mesa mas distancia");
            return;
        }
        mueveMotor(steps * 3 / 4);
        if (modoAuto)
        {
            posicionExpulsor(2);
        }
        delay(1000);
        mueveMotorB(steps * 3 / 4);
    }
}


void mueveMotor(int stepsToMove) {
    Serial.println("En mueve");
    digitalWrite(dirPin, HIGH); //Definimos sentido
    static int stepDelay = round((0.1125 * 1000000) / (2 * myData.velPap));///definidmos velocidad

    Serial.println("Antes de for");
    Serial.println(stepDelay);
    for (int x = 0; x < stepsToMove; x++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(stepDelay);
    }
}

void mueveMotorB(int stepsToMove) {
    digitalWrite(dirPin, LOW);
    static int stepDelay = round((0.1125 * 1000000) / (2 * myData.velPap));

    for (int x = 0; x < stepsToMove; x++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(stepDelay);
    }
}

void buscaInicio()
{
  while(digitalRead(pinFinalCarrera) == LOW)
  {
    mueveMotorB(stepInicial);
  }

  mueveMotor(75);
}