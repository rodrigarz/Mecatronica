#ifndef FUNCIONES_H
#define FUNCIONES_H
#include "Librerias.h"
#include "Globales.h"

void inicializa();
void movimiento();
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len);

#endif
