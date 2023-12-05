#include "Librerias.h"


String stringEstado[] = {"Automatico", "Manual"};
String stringControl[] = {"Vel", "Pos"};

uint8_t placaServo2[] = {0x80, 0x7D, 0x3A, 0xFD, 0x0D, 0x50}; //Direccion placa rodrigo (la de mas pines)

esp_now_peer_info_t peerInfo;
int posServo = 0;
int posExpulsor = 0;
int pasosPap = 0;
int posPap = 1;



// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
 // data.posServo = 0;
 
  inicializacion();                  // Incialización del display
  settingsLoadFromEEprom();         // Carga valores de Eeprom
  inicializaRotaryEncoder();         //Incialización encoder rotativo HW-040 

  ESP32Encoder::useInternalWeakPullResistors=UP;
  escribeLcd(stringEstado[data.estado], stringControl[data.control]);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Init ESP-NOW


  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, placaServo2, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

   if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if(botonEncoderPulsado())
  {
    menuPrincipal();
  }
}
