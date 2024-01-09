#include "Automatico.h"

//Input pulldown -> pulsado = low (conectar a pin y a tension)
//sensor ir -> No detecta = 1
void plantaAutomatica()
{
  unsigned long int t;
	while (digitalRead(sensorIR) == 1)
	{
		Motor(0);
    //vTaskDelay(1);
	}

  if(digitalRead(sensorIR) == 0)
  {
    Motor(140);
   // vTaskDelay(1);
  }
  
	unsigned long int tactual = millis();
  while(millis()-tactual < 3000)
  {
   // vTaskDelay(1);
	}

  tactual = millis();
  Motor(0);

  posicionPasoPaso(2, true);

  delay(50);

}