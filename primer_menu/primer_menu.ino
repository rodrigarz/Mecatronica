#include <LiquidCrystal_I2C.h>
#include <ESP32Encoder.h>

#define CLK 22;
#define DT 23;

ESP32Encoder encoder;
unsigned long lecturaA;
int direccion = 5;

LiquidCrystal_I2C lcd1(0x27, 16, 2);
LiquidCrystal_I2C lcd2(0x3F, 16, 2);

void setup() {
  // put your setup code here, to run once:
  encoder.attachHalfQuad(23, 22);
  encoder.setCount(0);
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  long posA = encoder.getCount();
  lecturaA = millis();
  long posB = encoder.getCount();
  if(posA - posB <10)
  {
    direccion = 1; //sentido horario
  }else if(posA - posB >10)
  {
    direccion = 0; //sentido antihorario
  }



}
