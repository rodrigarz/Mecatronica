#ifndef GLOBALES_H
#define GLOBALES_H

const int dirPin = 5;
const int stepPin = 15;
const int EN = 18;
const int steps = 200 * 16;
int stepDelay = 0;
int posServo = 0;
static const int servoPin = 19;
const int encoderA = 23; //Morado
const int encoderB = 22; //Azul

typedef struct struct_message {
	int indicacion; //1 = posServo, 2 = posExpulsor, 3 = pasosPap, 4 = posPap
	int posServo;
	int posExpulsor;
	int gradosPaP;
	double velPap;
	int velCinta;
	int incrCinta;
	int posPap;
	int estado;
	int control;
	int estadoManual;
	double setPoint;
	double periodo;
	double kPZMVel;
	double kDZMVel;
	double kIZMVel;
	double kPVel;
	double kDVel;
	double kIVel;
	double kPZMPos;
	double kDZMPos;
	double kIZMPos;
	double kPPos;
	double kDPos;
	double kIPos;
	byte eepromValidData;
}struct_message;


#endif