#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TCS34725.h>

// Inicializa el sensor de color TCS34725
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(115200);

  // Inicializa el sensor de color
  if (!tcs.begin()) {
    Serial.println("Error al encontrar el sensor TCS34725. Conecta el sensor correctamente.");
  }

  Serial.println("Sensor TCS34725 encontrado. Listo para detectar colores.");
}

void loop() {
  uint16_t clear, red, green, blue;

  // Lee los valores de color del sensor
  tcs.getRawData(&red, &green, &blue, &clear);

  // Calcula las proporciones de los canales de color
  float total = red + green + blue;
  float redRatio = red / total;
  float greenRatio = green / total;
  float blueRatio = blue / total;

  // Define umbrales para cada color
  float colorThreshold = 0.4;  // Ajusta según sea necesario

  // Comprueba el color y muestra el resultado en el monitor serial
  if (redRatio > colorThreshold) {
    Serial.println("Rojo");
  } else if (greenRatio > colorThreshold) {
    Serial.println("Verde");
  } else if (blueRatio > colorThreshold) {
    Serial.println("Azul");
  } else {
    Serial.println("Vacio");
  }

  delay(1000); // Espera 1 segundo antes de realizar la siguiente lectura
}