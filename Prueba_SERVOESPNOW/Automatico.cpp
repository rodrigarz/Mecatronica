#include "Automatico.h"

//Input pulldown -> pulsado = low (conectar a pin y a tension)
//sensor ir -> No detecta = 1
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void inicializaColores()
{
  tcs.begin();
}

int obtenerColor()
{
  uint16_t clear, red, green, blue;

  tcs.setInterrupt(false);
  digitalWrite(ledColores, HIGH);
  delay(500);
  tcs.getRawData(&red, &green, &blue, &clear);
  tcs.setInterrupt(true);
  digitalWrite(ledColores, LOW);

  float total = red + green + blue;
  float redRatio = red / total;
  float blueRatio = blue / total;

  float colorRange = 0.4;

  if (redRatio > colorRange) {
    Serial.println("Rojo");
    return 1;
  } 
  else if (blueRatio > colorRange) {
    Serial.println("Azul");
    return 2;
  }
   else {
    Serial.println("Otro");
    return 3;
  }

}

void plantaAutomatica()
{
  int color = 0; //1 = rojo, 2 = azul, 3 = otro
  unsigned long int t;
	while (digitalRead(sensorIR) == 1)
	{
		Motor(0);
    //vTaskDelay(1);
	}

  if(digitalRead(sensorIR) == 0)
  {
    color = obtenerColor();
    Motor(140);
   // vTaskDelay(1);
  }
  
	unsigned long int tactual = millis();
  while(millis()-tactual < 5000)
  {
   // vTaskDelay(1);
	}

  tactual = millis();
  Motor(0);

  posicionPasoPaso(color, true);

  delay(50);

}