#findef Pantalla_h
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

#define DIRLCD1 0x3F
#define DIRLCD2	0X27

int16_t leeEncoder();
int8_t deltaEncoder();
void inicializaRotarEncoder();

void inicializaLcd(LiquidCrystal_I2C display);

void escribeLcd(String mensaje1, String mensaje2, LiquidCrystal_I2C display);
void escribeLcd1(String mensaje, LiquidCrystal_I2C display);

int leeEstadoRotativo();
bool botonEncoderPulsado();

int miMenu(String menu[],  int maxMenuItems, String opDefecto[], int nMenuOpDef, LiquidCrystal_I2C display);
void muestraMenu(String menu[], int maxMenuitems, String opDefecto[], int opcionMenu, LiquidCrystal_I2C display);
double dameValor(Strig cadena, double valor, double inc, double min, double max);

void menuPrincipal();
void menuManual();
void menuSerie();
void menuAutomatico();
void menuAjustes();
void menuParametros();

#endif