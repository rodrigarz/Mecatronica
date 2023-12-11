#include "WiFi.h"
#include <esp_now.h>

const int relayPin = 4;

typedef struct struct_message{
  bool emergencia;
}struct_message;

struct struct_message miMensaje;

static bool variable = false;

const unsigned long tiempoMaximo = 6000;
unsigned long tiempoAnterior = 0;

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

    esp_now_register_recv_cb(OnDataRecv);
    pinMode(relayPin, OUTPUT);
    //miMensaje.emergencia = false;

}

void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len){
  memcpy(&miMensaje, incomingData, sizeof(miMensaje));
  variable = miMensaje.emergencia;
  Serial.println("Recibido");
  tiempoAnterior = millis();
  if(miMensaje.emergencia == false)
  {
    digitalWrite(relayPin, HIGH);
  }
  else
  {
    digitalWrite(relayPin, LOW);
  }
  Serial.println(variable);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long tiempoActual = millis();
  if(tiempoActual - tiempoAnterior >= tiempoMaximo)
  {
    digitalWrite(relayPin, LOW);
    tiempoAnterior = tiempoActual;
  }
}
