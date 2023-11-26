#include "Librerias.h"

system_t sys;

String stringEstado[] = {"Automatico", "Manual"};
String stringControl[] = {"Vel", "Pos"};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
 
  inicializacion();                  // Incialización del display
  settingsLoadFromEEprom();         // Carga valores de Eeprom
  inicializaRotaryEncoder();         //Incialización encoder rotativo HW-040 

  ESP32Encoder::useInternalWeakPullResistors=UP;
  escribeLcd(stringEstado[sys.estado], stringControl[sys.control]);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(botonEncoderPulsado())
  {
    menuPrincipal();
  }
}
