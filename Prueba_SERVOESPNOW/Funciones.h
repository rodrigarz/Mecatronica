#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <Arduino.h>
#include <iostream>
#include "WiFi.h"
#include <esp_now.h>
#include <ESP32Encoder.h>
#include "PID_v1.h"

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
double  move(double xd, double vmax, double a, double dt);

#endif
