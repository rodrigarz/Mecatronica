#ifndef Globales_h
#define Globales_h

#include "librerias.h"

#define AUTOMATICO 0
#define MANUAL 1

#define VELOCIDAD 0
#define POSICION 1

#define MANUAL_ENC 0
#define PUERTO_SERIAL 1


//Tamaño actual: 145 bytes (max esp-now 250 byets)
typedef struct struct_message {
  int indicacion; //1 = posServo, 2 = posExpulsor, 3 = pasosPap, 4 = posPap
  int posServo;
  int posExpulsor;
  int gradosPaP;
  int velPap;
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
} struct_message;


extern struct_message data;

extern String stringEstado[];
extern String stringControl[];



#endif // !Globales_h

