/*
Programa_control_con_rele.ino

Autor: Grupo 5 mecatrónica
Año: 2023/2024

Descripción:
Sistema de seguridad para cortar la corriente a la planta mediante un relé

*/


#include "WiFi.h"
#include <esp_now.h>

//Inicializamos todas las variables y estructuras necesarias
const int relayPin = 8;
const int pinLed = 3;

typedef struct struct_message{
  bool emergencia;
}struct_message;

struct struct_message miMensaje;

static bool variable = false;

const unsigned long tiempoMaximo = 3000;
unsigned long tiempoAnterior = 0;
unsigned long lastMillis = 0;

static int contador = 0;

//Funcion para encender y apagar alternativamente el led
void cambiarColorRojo() {
  if(miMensaje.emergencia == true)
  {
    digitalWrite(pinLed, !digitalRead(pinLed));
  }
  else
  {
    digitalWrite(pinLed, LOW);
  }
}

void setup() {
  // put your setup code here, to run once:
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

  //Bindeamos el callback e inicializamos los pines
  esp_now_register_recv_cb(OnDataRecv);
  pinMode(relayPin, OUTPUT);
  pinMode(pinLed, OUTPUT);

}


//Callback al recibir informacion, si se detecta emergencia, se apaga el relé
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len){
  memcpy(&miMensaje, incomingData, sizeof(miMensaje));
  variable = miMensaje.emergencia;
  Serial.println("Recibido");
  tiempoAnterior = millis();
  if(miMensaje.emergencia == false)
  {
    digitalWrite(relayPin, HIGH);
    contador = 0;
  }
  else
  {
    digitalWrite(relayPin, LOW);
  }
  Serial.println(variable);
}


//Si pasan dos periodos sin recibir ok, se apaga la planta
void loop() {
  // put your main code here, to run repeatedly
  unsigned long tiempoActual = millis();
  if(tiempoActual - tiempoAnterior >= tiempoMaximo)
  {
    contador = contador + 1;
    tiempoAnterior = tiempoActual;
  }

  if(contador >= 2)
  {
    digitalWrite(relayPin, LOW);
    miMensaje.emergencia = true;
  }

  //Si hay emergencia, el led parpadea rojo
  if(miMensaje.emergencia == true)
  {
    if (millis() - lastMillis >= 500) {
      cambiarColorRojo();
      lastMillis = millis();  // Actualiza el tiempo del último cambio
    }
  } else{
    digitalWrite(pinLed, HIGH);
  }
}
