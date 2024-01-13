/*
Menu.h

Autor: Grupo 5 mecatrónica
Año: 2023/2024

Descripción:
Contiene toda la funcionalidad para usar el menu mediante rotary encoder en las dos pantallas del mando

*/


#ifndef Pantalla_h
#define Pantalla_h

#include <LiquidCrystal_I2C.h>

/* Conexiones Display
LCD       ESP32
Vcc       3.3 v
GND       GND
SDA        GPIO21
SCL       GPIO22
*/

#define ROTARY_ENCODER_DT 35
#define ROTARY_ENCODER_CLK 36
#define ROTARY_ENCODER_BUTTON_PIN 32
#define ROTARY_ENCODER_VCC_PIN -1

#define DIRLCD1 0x27
#define DIRLCD2	0X3F

int16_t leeEncoder();
int8_t deltaEncoder();
void inicializaRotaryEncoder();

void inicializaLcd(LiquidCrystal_I2C &display);
void inicializacion();

void escribeLcd(String mensaje1, String mensaje2);
void escribeLcd1(String mensaje, LiquidCrystal_I2C &display);

int leeEstadoRotativo();
bool botonEncoderPulsado();

int miMenu(String menu[],  int maxMenuItems, String opDefecto[], int nMenuOpDef, LiquidCrystal_I2C display);
void muestraMenu(String menu[], int maxMenuitems, String opDefecto[], int opcionMenu, LiquidCrystal_I2C &display);
double dameValor(String cadena, double valor, double inc, double min, double max);
int dameValorInt(String cadena, int valor, int inc, int min, int max);
void mandarDatos();

void menuPrincipal();
void menuManual();
void menuSerie();
void menuAutomatico();
void menuAjustes();
void menuParametros();
void menuKpos();
void menuManualEncoder();
void menuMoverCinta();
void menuPuertoSerial();

int splitString(String input, char delimiter, String parts[]);

void paroEmergencia();
void marcha();

#endif