#include "Automatico.h"

//Input pulldown -> pulsado = low (conectar a pin y a tension)
//sensor ir -> No detecta = 1
void plantaAutomatica()
{
	int clasificacion[3] = { 1, 2, 3 };
	while (digitalRead(sensorIR) == 1)
	{
		Motor(200);
	}

	for (int i = 0; i <= 20; i++)
	{
		Motor(140);
	}

	//A�adir aqui el sistema de identificacion y clasificacion
	for (int j = 0; j < sizeof(clasificacion); j++)
	{
		posicionPasoPaso(clasificacion[j], true);
	}
}