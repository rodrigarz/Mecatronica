#ifndef Globales_h
#define Globales_h

#include "Librerias.h"


#define MARCHA 0
#define PARO 1
#define VELOCIDAD 0
#define POSICION 1
#define ESCALON 0
#define SENO  1
#define REFERENCIA 2
#define RAMPA 3

typedef struct {
    int estado;
    int control;
    int entrada;
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
extern String stringEntrada[];


#endif
