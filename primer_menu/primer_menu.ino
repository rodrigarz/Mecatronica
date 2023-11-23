#include <LiquidCrystal_I2C.h>
#include <ESP32Encoder.h>
#include "Menu.h"
#include <AiEsp32RotaryEncoder.h>

int encoderA = 23;
int encoderB = 19;
int boton_pulsado = 0;

ESP32Encoder encoder;
unsigned long lecturaA;
int direccion = 5;

LiquidCrystal_I2C disp1(0x27, 16, 2);
LiquidCrystal_I2C disp2(0x30, 16, 2);

void setup() {
  // put your setup code here, to run once:
  inicializaLcd(disp1);
  inicializaLcd(disp2);
  ESP32Encoder::useInternalWeakPullResistors=UP;
  encoder.attachHalfQuad(23, 19);
  encoder.setCount(0);
  encoder.clearCount();
  Serial.begin(115200);
  escribeLcd("Mensaje 1", "Mensaje 2", "Mensaje 3", "Mensaje 4");

}

void loop() {
  // put your main code here, to run repeatedly:
  if(botonEncoderPulsado()) menuPrincipal();

}
