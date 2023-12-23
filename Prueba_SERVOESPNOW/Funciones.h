#ifndef FUNCIONES_H
#define FUNCIONES_H
#include "Librerias.h"
  #ifndef GLOBALES_H
    #include "Globales.h"
  #endif

void inicializa();
void movimiento();
void posicionServo();
void posicionExpulsor(int modoActiva);
void pasosPasoPaso();
void posicionPasoPaso(int posicion, bool modoAuto);
void mueveMotor(int stepsToMove);
void mueveMotorB(int stepsToMove);
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len);
void controlVelocidad();
void controlPosicion();
double move(double xd, double vmax, double a, double dt);
//void plantaAutomatica();
void buscaInicio();
extern int movMotorDC;
extern bool entraMovimiento;

#endif
