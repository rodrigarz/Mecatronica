/*
Cinta.h

Autor: Grupo 5 mecatrónica
Año: 2023/2024

Descripción:
Contiene las funcionalidades necesarias para implementar el funcionamiento de la cinta transportadora

*/


#ifndef CINTA_H
#define CINTA_H

#include "Globales.h"
#include "Librerias.h"

void inicializaCinta();
void controlVelocidad();
void controlPosicion();
double move(double xd, double vmax, double a, double dt);

#endif