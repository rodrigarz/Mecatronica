#ifndef CINTA_H
#define CINTA_H

#include "Globales.h"
#include "Librerias.h"

void inicializaCinta();
void controlVelocidad();
void controlPosicion();
double move(double xd, double vmax, double a, double dt);

#endif