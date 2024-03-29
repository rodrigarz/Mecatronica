#include "Librerias.h"

//Declaramos las variables necesarias
String stringEstado[] = {"Automatico", "Manual"};
String stringControl[] = {"Vel", "Pos"};

//uint8_t placaServo2[] = {0x80, 0x7D, 0x3A, 0xFD, 0x0D, 0x50}; //Direccion placa rodrigo (la de mas pines)
//
uint8_t placaServo2[] = {0xB4, 0xE6, 0x2D, 0xFB, 0x1B, 0xBD};
uint8_t placaControl[] = {0x10, 0x91, 0xA8, 0x19, 0xC8, 0xF4};

esp_now_peer_info_t peerInfo;
esp_now_peer_info_t peerInfo2;
int posServo = 0;
int posExpulsor = 0;
int pasosPap = 0;
int posPap = 1;

hw_timer_t * timer = NULL;

const int pinParo = 12;
const int pinMarcha = 14;

struct mensaje_control miMensaje;

// Callback al enviar, muestra si ha sido correcto o no
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//Interrupcion periodica para manadar ok a la placa control
void IRAM_ATTR onTimer()
{
  esp_now_send(placaControl, (uint8_t *) &miMensaje, sizeof(miMensaje));
}

//Interrupcion si se activa el paro de emergencia
void IRAM_ATTR paroEmergencia()
{
  miMensaje.emergencia = true;
  esp_now_send(placaControl, (uint8_t *) &miMensaje, sizeof(miMensaje));
}

//Interrupcion si se activa el marcha
void IRAM_ATTR marcha()
{
  miMensaje.emergencia = false;
  esp_now_send(placaControl, (uint8_t *) &miMensaje, sizeof(miMensaje));
}

void setup() {
  // put your setup code here, to run once:

  //Inicializamos
  Serial.begin(115200);
  pinMode(pinParo, INPUT_PULLUP);
  pinMode(pinMarcha, INPUT_PULLUP);
  inicializacion();                  // Incialización del display
  settingsLoadFromEEprom();         // Carga valores de Eeprom
  inicializaRotaryEncoder();         //Incialización encoder rotativo HW-040 

  //Vinculamos las interrupciones
  attachInterrupt(pinParo, paroEmergencia, RISING);
  attachInterrupt(pinMarcha, marcha, RISING);

  ESP32Encoder::useInternalWeakPullResistors=UP;
  escribeLcd(stringEstado[data.estado], stringControl[data.control]);

  //Iniciamos ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Init ESP-NOW
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, placaServo2, 6);
  memcpy(peerInfo2.peer_addr, placaControl, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  peerInfo2.channel = 1;
  peerInfo2.encrypt = false;

  //Si hay errores al hacer link con algun esclavo, lo indicamos por puerto serie
   if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer servo");
    return;
  }

  if (esp_now_add_peer(&peerInfo2) != ESP_OK){
    Serial.println("Failed to add peer control");
    return;
  }

  //Configuramos el temporizador para la interrupcion periodica
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 2000000, true);
  timerAlarmEnable(timer);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(botonEncoderPulsado())
  {
    menuPrincipal();
  }
}


