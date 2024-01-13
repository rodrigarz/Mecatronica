/*
Globales.h

Autor: Grupo 5 mecatrónica
Año: 2023/2024

Descripción:
Contiene los pines, variables globales y estructuras necesarias para el funcionamiento general

*/


#ifndef GLOBALES_H
#define GLOBALES_H

#include <Arduino.h>


//Declaramos los pines
const int dirPin = 4;
const int stepPin = 15;
const int EN = 18;
const int servoPin = 12;
const int encoderA = 23; //Morado
const int encoderB = 19; //Azul
const int pinFinalCarrera = 13;
const int stepInicial = 2;
const int sensorIR = 33;
const int ledColores = 14;

//Variables que se usan en varios puntos del programa
extern int gradosActualMesa;
extern double Setpoint;
extern bool entraMovimiento;
extern bool entraAutomatico;

//Estructura de datos por ESP-NOW
typedef struct struct_message {
	int indicacion; //1 = posServo, 2 = posExpulsor, 3 = pasosPap, 4 = posPap, 5 = velCinta, 6 = posCinta
	int posServo; //Ajusta por grados el servo
	int posExpulsor; //1 = activar, 0 = desactivar, 2 = movimiento completo
	int gradosPaP; //Indica grados movimiento mesa
	double velPap; //Indica velocidad movmiento mesa
	int velCinta; //Inidca velocidad para control cinta
	int incrCinta; //Indica incremento de distancia para cinta
	int posPap; //Indica posicion automatica de la mesa
	bool estado; // TRUE = todo ok, FALSE = emergencia activada
	bool control; //TRUE = modos manuales, FALSE = modo automatico
	bool controlCinta; //TRUE = posicion, FALSE = velocidad
	int estadoManual;
	double setPoint;
	double kPZMPos;
	double kDZMPos;
	double kIZMPos;
	double kPPos;
	double kDPos;
	double kIPos;
	byte eepromValidData;
}struct_message;

extern struct struct_message myData;


#endif