#include "Librerias.h"

volatile bool miVariable = false;

void setup() {
  // put your setup code here, to run once:
  inicializa();

}
void loop() {
  // put your main code here, to run repeatedly:
  miVariable = devuelveInter();
  while(miVariable)
  {
    miVariable = devuelveInter();
  }
}
