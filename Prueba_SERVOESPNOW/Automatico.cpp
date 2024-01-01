#include "Automatico.h"

//Input pulldown -> pulsado = low (conectar a pin y a tension)
//sensor ir -> No detecta = 1
void plantaAutomatica()
{
  unsigned long int t;
	int clasificacion[3] = { 1, 2, 3 };
	while (digitalRead(sensorIR) == 0)
	{
		Motor(100);
    vTaskDelay(1);
	}

	unsigned long int tactual = millis();
  t = millis();
  while(t-tactual < 1000)
  {
		Motor(70);
    t = millis();
    vTaskDelay(1);
	}
  // if(t - tactual > 1000)
  // {
  //   Motor(0);
  // }
	//A�adir aqui el sistema de identificacion y clasificacion
	for (int j = 0; j < sizeof(clasificacion); j++)
	{
		posicionPasoPaso(clasificacion[j], true);
	}
}