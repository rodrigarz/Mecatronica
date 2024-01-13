#include "Librerias.h"
#ifndef GLOBALES_H
  #include "Globales.h"
#endif
void setup() {
  // put your setup code here, to run once:
  inicializa();

}
void loop() {
  // put your main code here, to run repeatedly:
  
  //Entrar a modo manual o automatico
  if(entraMovimiento)
  {
    movimiento();
    delay(50);
    entraMovimiento = false;
    
  } else if (entraAutomatico)
  {
    plantaAutomatica();
    delay(50);
  }
}
