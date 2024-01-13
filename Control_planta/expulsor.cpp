#include "expulsor.h"

Servo servo1;

//Inicalizamos el servo
void inicializaServo()
{
  servo1.attach(servoPin);
  servo1.write(0);
}

//Para mover por grados el servo, limitado al maximo recorrido del expulsor
void posicionServo()
{
  int servoPosAnt = 0;
  int servoPosition = map(myData.posServo, 0, 75, 0, 75);
  servoPosition = constrain(servoPosition, 0, 75);
  servoPosAnt = servoPosition;
  servo1.write(servoPosition);
  if (servoPosAnt != servoPosition)
  {
    Serial.println(servoPosition);
  }
  delay(200);
}

//Movimientos progresivos para activar el expulsor
void posicionExpulsor(int modoActiva)
{   
  if (modoActiva == 1)
  {
      servo1.write(0);
      delay(50);
      for (int i = 0; i <= 75; i++)
      {
          servo1.write(i);
          delay(50);
      }
  }
  else if (modoActiva == 0)
  {
      servo1.write(75);
      delay(50);
      for (int i = 75; i >= 0; i--)
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
      for (int i = 0; i <= 75; i++)
      {
          servo1.write(i);
          delay(50);
      }

      delay(1000);

      for (int i = 75; i >= 0; i--)
      {
          servo1.write(i);
          delay(100);
      }
  }
}