#ifndef Globales_h
#define Globales_h

#include "librerias.h"

#define AUTOMATICO 0
#define MANUAL 1

#define VELOCIDAD 0
#define POSICION 1

#define MANUAL_ENC 0
#define PUERTO_SERIAL 1



typedef struct struct_message {
    int posServo;
} struct_message;

typedef struct
{
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
} system_t;

extern system_t sys;
extern String stringEstado[];
extern String stringControl[];



#endif // !Globales_h

