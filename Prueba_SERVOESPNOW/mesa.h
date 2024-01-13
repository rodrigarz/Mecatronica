/*
Mesa.h

Autor: Grupo 5 mecatrónica
Año: 2023/2024

Descripción:
Contiene las funciones necesarias para implementar funcionalidad de la mesa

*/


#ifndef MESA_H
#define MESA_H

#include "Globales.h"
#include "Librerias.h"

void inicializaMesa();
void pasosPasoPaso();
void posicionPasoPaso(int posicion, bool modoAuto);
void mueveMotor(int stepsToMove);
void mueveMotorB(int stepsToMove);
void buscaInicio();

#endif