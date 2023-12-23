#ifndef Globales_h
#define Globales_h

#include "librerias.h"

#define AUTOMATICO false
#define MANUAL true

#define VELOCIDAD false
#define POSICION true

#define MANUAL_ENC 0
#define PUERTO_SERIAL 1


//Tamaño actual: 145 bytes (max esp-now 250 byets)
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
} struct_message;


typedef struct mensaje_control{
  bool emergencia;
}mensaje_control;

extern struct_message data;
extern mensaje_control mensaje;

extern String stringEstado[];
extern String stringControl[];



#endif // !Globales_h

