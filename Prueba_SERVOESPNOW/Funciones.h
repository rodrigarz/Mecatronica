#ifndef FUNCIONES_H
#define FUNCIONES_H
#include "Librerias.h"
#include "Globales.h"

void inicializa();
void movimiento();
void posicionServo();
void posicionExpulsor(int modoActiva);
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len);
void controlVelocidad();
void controlPosicion();
double move(double xd, double vmax, double a, double dt);
//void plantaAutomatica();
extern int movMotorDC;
extern bool entraMovimiento;

#endif
