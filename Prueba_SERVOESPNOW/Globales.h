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
	int indicacion; //1 = posServo, 2 = posExpulsor, 3 = pasosPap, 4 = posPap, 5 = velCinta, 6 = posCinta
	int posServo; //Ajusta por grados el servo
	int posExpulsor; //1 = activar, 0 = desactivar, 2 = movimiento completo
	int gradosPaP; //Indica grados movimiento mesa
	double velPap; //Indica velocidad movmiento mesa
	int velCinta; //Inidca velocidad para control cinta
	int incrCinta; //Indica incremento de distancia para cinta
	int posPap; //Indica posicion automatica de la mesa
	int estado; // 0 = todo ok, 1 = emergencia activada
	int control; //0 = modos manuales, 1 = modo automatico
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