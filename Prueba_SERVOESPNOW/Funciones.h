#ifndef FUNCIONES_H
#define FUNCIONES_H
#include "Librerias.h"

void inicializa();
void movimiento();
void posicionServo();
void posicionExpulsor();
void pasosPasoPaso();
void posicionPasoPaso();
void mueveMotor(int stepsToMove);
void mueveMotorB(int stepsToMove);
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len);
void controlVelocidad();
void controlPosicion();
double move(double xd, double vmax, double a, double dt);

#endif
