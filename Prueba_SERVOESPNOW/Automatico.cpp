#include "Automatico.h"

void plantaAutomatica()
{
	int clasificacion[3] = { 1, 2, 3 };
	while (digitalRead(sensorIR) == LOW)
	{
		Motor(100);
	}

	for (int i = 0; i <= 20; i++)
	{
		Motor(20);
	}

	//Añadir aqui el sistema de identificacion y clasificacion
	for (int j = 0; j < sizeof(clasificacion); j++)
	{
		posicionPasoPaso(clasificacion[j], true);
	}
}