/*
Funciones.h

Autor: Grupo 5 mecatrónica
Año: 2023/2024

Descripción:
Incluye funciones generales, como inicializacion, tipo de movimiento, o callback al recibir datos

*/




#ifndef FUNCIONES_H
#define FUNCIONES_H
#include "Librerias.h"
#include "Globales.h"

void inicializa();
void movimiento();
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len);

#endif
