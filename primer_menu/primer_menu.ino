#include <LiquidCrystal_I2C.h>
#include <ESP32Encoder.h>
#include "Menu.h"
#include <AiEsp32RotaryEncoder.h>
#include "WiFi.h"
#include <esp_now.h>

int encoderA = 23;
int encoderB = 19;
int boton_pulsado = 0;

uint8_t placaServo[] = {0x80, 0x7D, 0x3A, 0xFD, 0x0D, 0x50}; //Direccion placa rodrigo (la de mas pines)

ESP32Encoder encoder;
unsigned long lecturaA;
int direccion = 5;
String success;
esp_now_peer_info_t peerInfo;

//LiquidCrystal_I2C disp1(0x27, 16, 2);
//LiquidCrystal_I2C disp2(0x3F, 16, 2);


void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // put your setup code here, to run once:
  inicializaLcd();
  //inicializaLcd(disp2);
  ESP32Encoder::useInternalWeakPullResistors=UP;
  encoder.attachHalfQuad(23, 19);
  encoder.setCount(0);
  encoder.clearCount();
  Serial.begin(115200);
  escribeLcd("Mensaje 1", "Mensaje 2", "Mensaje 3", "Mensaje 4");

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Init ESP-NOW


  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, placaServo, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

   if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

}
void loop() {
  // put your main code here, to run repeatedly:
  menuPrincipal();
  delay(1000);

}
