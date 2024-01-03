#include "expulsor.h"

Servo servo1;

void inicializaServo()
{
  servo1.attach(servoPin);
}

void posicionServo()
{
  Serial.println("Entro");
    int servoPosAnt = 0;
    int servoPosition = map(myData.posServo, 0, 180, 0, 180);
    servoPosAnt = servoPosition;
    servo1.write(myData.posServo);
    if (servoPosAnt != servoPosition)
    {
        Serial.println(servoPosition);
    }
    delay(200);
}

void posicionExpulsor(int modoActiva)
{   
    if (modoActiva == 1)
    {
        servo1.write(0);
        delay(50);
        for (int i = 0; i <= 180; i++)
        {
            servo1.write(i);
            delay(50);
        }
    }
    else if (modoActiva == 0)
    {
        servo1.write(180);
        delay(50);
        for (int i = 180; i >= 0; i--)
        {
            servo1.write(i);
            delay(50);
        }
    }
    else if (modoActiva == 2)
    {
        //Para movimiento progresivo
        servo1.write(0);
        delay(50);
        for (int i = 0; i <= 180; i++)
        {
            servo1.write(i);
            delay(50);
        }

        delay(1000);

        for (int i = 180; i >= 0; i--)
        {
            servo1.write(i);
            delay(100);
        }
    }
}