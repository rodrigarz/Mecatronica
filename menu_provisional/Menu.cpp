
#include <Arduino.h>
#include "AiEsp32RotaryEncoder.h"
#include "Menu.h"
#include "Globales.h"
#include <LiquidCrystal_I2C.h>
#include "Eeprom.h"

int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd1(DIRLCD1, lcdColumns, lcdRows);
LiquidCrystal_I2C lcd2(DIRLCD2, lcdColumns, lcdRows);

byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};

byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};

byte menuCursor[8] = {
  B01000, //  *
  B00100, //   *
  B00010, //    *
  B00001, //     *
  B00010, //    *
  B00100, //   *
  B01000, //  *
  B00000  //
};


AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_DT, ROTARY_ENCODER_CLK, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN);

int16_t leeEncoder()
{
	return(rotaryEncoder.readEncoder());
}

int8_t deltaEncoder()
{
	int8_t encoderDelta = rotaryEncoder.encoderChanged();
	return encoderDelta;
}

void inicializaRotaryEncoder()
{
	rotaryEncoder.begin();
	rotaryEncoder.setup([] {rotaryEncoder.readEncoder_ISR(); });
	rotaryEncoder.setBoundaries(-10000, 10000, true);
}

void inicializacion()
{
  inicializaLcd(lcd1);
  inicializaLcd(lcd2);
  lcd1.backlight();
  lcd2.backlight();
}

void inicializaLcd(LiquidCrystal_I2C display)
{
	display.init();
	display.backlight();
	display.createChar(0, menuCursor);
	display.createChar(1, upArrow);
	display.createChar(2, downArrow);
}

void escribeLcd(String mensaje1, String mensaje2)
{
	lcd1.setCursor(0, 0);
	lcd1.print(mensaje1);
	for (int i = mensaje1.length(); i < lcdColumns; i++) lcd1.print(" ");
	lcd1.setCursor(0, 1);
	lcd1.print(mensaje2);
	for (int i = mensaje2.length(); i < lcdColumns; i++) lcd1.print(" ");
}

void escribeLcd1(String mensaje, LiquidCrystal_I2C display)
{
	display.setCursor(0, 0);
	display.print(mensaje);
	for (int i = mensaje.length(); i < lcdColumns; i++) display.print(" ");
}

bool botonEncoderPulsado()
{
	if (rotaryEncoder.currentButtonState() == BUT_RELEASED)
		return false;
	else
		return true;
}

int miMenu(String menu[],int maxMenuItems,   String opDefecto[], int nMenuOpDef, LiquidCrystal_I2C display){
  int valEncoder,valEncoderAnt;
  int opcionMenu=nMenuOpDef+1;
  static long int tiempo=millis();
  
 
  valEncoderAnt = rotaryEncoder.readEncoder();
  muestraMenu(menu, maxMenuItems,  opDefecto, opcionMenu, display);
  while (rotaryEncoder.currentButtonState() != BUT_RELEASED) {
    valEncoder = rotaryEncoder.readEncoder();
    Serial.println(valEncoder);
    if(valEncoder>valEncoderAnt){     
            opcionMenu++;
            opcionMenu = constrain(opcionMenu, 1, maxMenuItems);
            muestraMenu(menu, maxMenuItems,   opDefecto, opcionMenu, display);
            valEncoderAnt=valEncoder;
    }else if(valEncoder<valEncoderAnt){
            opcionMenu--;
            opcionMenu = constrain(opcionMenu, 1, maxMenuItems);
            muestraMenu(menu, maxMenuItems,   opDefecto, opcionMenu, display);
            valEncoderAnt=valEncoder;
    }else{      // Entra cada cierto tiempo
            if(millis()-tiempo>400) {
                if(menu[opcionMenu-1].length()+opDefecto[opcionMenu-1].length()>=lcdColumns-1)                 // Solo se llama a la funcion si el texto es largo
                    muestraMenu(menu, maxMenuItems, opDefecto,opcionMenu, display );
                tiempo=millis();
            }
    }
  }
  delay(100);
  return opcionMenu-1;
}

void muestraMenu(String menu[], int maxMenuItems, String opDefecto[], int opcionMenu, LiquidCrystal_I2C display)
{
  int numPags, pag;
  String linea;
  int menuMaxLineas = 2;

  numPags = round((float)maxMenuItems / menuMaxLineas + 0.4999);
  pag = (opcionMenu - 1) / menuMaxLineas;

  display.clear();

  for (int i = 0; i < menuMaxLineas; i++) {
    int currentIndex = pag * menuMaxLineas + i;
    if (currentIndex < maxMenuItems) {
      display.setCursor(0, i);
      linea = menu[currentIndex] + " " + opDefecto[currentIndex];

      if (opcionMenu == currentIndex + 1) {
        display.write(byte(0));
        if (linea.length() <= lcdColumns - 1) {
          display.print(linea);
          for (int j = linea.length(); j < lcdColumns - 1; j++) {
            display.print(" ");
          }
        } else {
          display.print(linea.substring(0, lcdColumns - 1));
        }
      } else {
        display.print(" ");
        if (linea.length() <= lcdColumns) {
          display.print(linea);
          for (int j = linea.length(); j < lcdColumns; j++) {
            display.print(" ");
          }
        } else {
          display.print(linea.substring(0, lcdColumns));
        }
      }
    }
  }
}



double dameValor(String cadena, double valor, double inc, double min, double max)
{
	int valEncoder, valEncoderAnt;
	lcd2.clear();
	lcd2.setCursor(0, 0);
	lcd2.print(cadena + String(":"));
	lcd2.setCursor(0, 1);
	lcd2.print(valor);
	static long int tiempo = millis();

	valEncoderAnt = leeEncoder();
	while (rotaryEncoder.currentButtonState() != BUT_RELEASED)
	{
		valEncoder = leeEncoder();
		if (valEncoder > valEncoderAnt)
		{
			valor = valor + inc;
		}
		else if (valEncoder < valEncoderAnt)
		{
			valor = valor - inc;
		}
		valor = constrain(valor, min, max);
		valEncoderAnt = valEncoder;
		lcd2.clear();
		lcd2.setCursor(0, 0);
		lcd2.print(cadena + String(":"));
		lcd2.setCursor(0, 1);
		lcd2.print(valor);
	}
	delay(100);
	return valor;
}

void menuPrincipal()
{
	String menu[] = { "Volver", "Manual", "Automatico", "Ajustes" };
	String opDefecto[4];
	int index = 0;
	lcd2.clear();

	do
	{
		if (sys.estado == AUTOMATICO)
		{
			opDefecto[1] = "";
			opDefecto[2] = "*";
		}
		else
		{
			opDefecto[1] = "*";
			opDefecto[2] = "";
		}
		index = miMenu(menu, 4, opDefecto, index, lcd1);

		switch (index)
		{
		case 1:
			sys.estado = MANUAL;
      menuManual();
			break;
		case 2:
			sys.estado = AUTOMATICO;
			break;
		case 3:
			menuAjustes();
			break;
		}
	} while (index == 3);
}

void menuAjustes()
{
	String menu[] = { "Volver", "Control", "Parametros" };
	int index = 0;
	String opDefecto[3];
	lcd2.clear();

	do
	{
		opDefecto[1] = (sys.control == VELOCIDAD ? "Velocidad" : "Posicion");
		index = miMenu(menu, 3, opDefecto, index, lcd1);
		
		switch (index)
		{
		case 1:
			sys.control = (sys.control == VELOCIDAD ? POSICION : VELOCIDAD);
			break;
		case 2:
			menuParametros();
			break;
		}
	} while (index != 0);
}

void menuParametros()
{
	String menu[] = { "Volver", "SetPoint", "Periodo", "Param vel.", "Param. pos", "Grabar param", "Borrar param" };
	int index = 0;
	String opDefecto[7];
	lcd1.clear();
	lcd1.setCursor(1, 0);
	lcd1.print("Parametros");
	static long int tiempo = millis();

	lcd1.setCursor(0, 1);
	do
	{
		opDefecto[1] = String(sys.setPoint);
		opDefecto[2] = String(sys.periodo);
		tiempo = millis();

		index = miMenu(menu, 7, opDefecto, index, lcd1);

		switch (index)
		{
		case 1:
			sys.setPoint = dameValor(menu[index], sys.setPoint, 10, -10000, 10000);
			break;
		case 2:
			sys.periodo = dameValor(menu[index], sys.periodo, 0.05, 0, 1000);
			break;
		case 3:
			menuKvel();
			break;
		case 4:
			menuKpos();
			break;
		case 5:
			settingsSaveToEEprom();
			escribeLcd1("Grabando", lcd2);
			do
			{

			} while (millis() - tiempo < 2000);
			break;
		case 6:
			settingsWipe();
			escribeLcd1("Borrando param", lcd2);
			do
			{
			
			} while (millis() - tiempo < 2000);
			break;
		}
	} while (index != 0);
}

void menuKvel()
{
	String menu[] = { "Volver", "KpZMVel","KdZMVel","KiZMVel","KpVel","KdVel","KiVel" };
	int index = 0;
	String opDefecto[7];

	do {
		opDefecto[1] = String(sys.kPZMVel);
		opDefecto[2] = String(sys.kDZMVel);
		opDefecto[3] = String(sys.kIZMVel);
		opDefecto[4] = String(sys.kPVel);
		opDefecto[5] = String(sys.kDVel);
		opDefecto[6] = String(sys.kIVel);

		index = miMenu(menu, 7, opDefecto, index, lcd1);
		switch (index) {
		case 1:                     ////--Kp Zona muerta--////
			sys.kPZMVel = dameValor(menu[index], sys.kPZMVel, 0.01, 0, 1000);
			break;
		case 2:                     ////--Kd Zona Muerta--////
			sys.kDZMVel = dameValor(menu[index], sys.kDZMVel, 0.01, 0, 1000);
			break;
		case 3:                     ////--Ki Zona Muerta--////
			sys.kIZMVel = dameValor(menu[index], sys.kIZMVel, 0.01, 0, 1000);
			break;
		case 4:                     ////--Kp--////
			sys.kPVel = dameValor(menu[index], sys.kPVel, 0.01, 0, 1000);
			break;
		case 5:                     ////--Kd--////
			sys.kDVel = dameValor(menu[index], sys.kDVel, 0.01, 0, 1000);
			break;
		case 6:                     ////--Ki--////
			sys.kIVel = dameValor(menu[index], sys.kIVel, 0.01, 0, 1000);
			break;
		}
	} while (index != 0);
}

void menuKpos() {
	String menu[] = { "Volver", "KpZMPos","KdZMPos","KiZMPos","KpPos","KdPos","KiPos" };
	int index = 0;
	String opDefecto[7];

	do {
		opDefecto[1] = String(sys.kPZMPos);
		opDefecto[2] = String(sys.kDZMPos);
		opDefecto[3] = String(sys.kIZMPos);
		opDefecto[4] = String(sys.kPPos);
		opDefecto[5] = String(sys.kDPos);
		opDefecto[6] = String(sys.kIPos);

		index = miMenu(menu, 7, opDefecto, index, lcd1);
		switch (index) {
		case 1:                     ////--Kp Zona muerta--////
			sys.kPZMPos = dameValor(menu[index], sys.kPZMPos, 0.01, 0, 1000);

			break;
		case 2:                     ////--Kd Zona Muerta--////
			sys.kDZMPos = dameValor(menu[index], sys.kDZMPos, 0.01, 0, 1000);

			break;
		case 3:                     ////--Ki Zona Muerta--////
			sys.kIZMPos = dameValor(menu[index], sys.kIZMPos, 0.01, 0, 1000);
			break;
		case 4:                     ////--Kp--////
			sys.kPPos = dameValor(menu[index], sys.kPPos, 0.01, 0, 1000);

			break;
		case 5:                     ////--Kd--////
			sys.kDPos = dameValor(menu[index], sys.kDPos, 0.01, 0, 1000);

			break;
		case 6:                     ////--Ki--////
			sys.kIPos = dameValor(menu[index], sys.kIPos, 0.01, 0, 1000);
			break;
		}
	} while (index != 0);

}

void menuManual()
{
	String menu[] = { "Volver", "Manual Enc.", "Puerto Serial" };
	int index = 0;
	String opDefecto[3];
  lcd2.clear();

	do
	{
		if (sys.estadoManual == MANUAL_ENC)
		{
			opDefecto[1] = "*";
			opDefecto[2] = "";
		}
		else
		{
			opDefecto[1] = "";
			opDefecto[2] = "*";
		}
		index = miMenu(menu, 3, opDefecto, index, lcd1);

		switch (index)
		{
		case 1:
			sys.estadoManual = MANUAL_ENC;
			break;
		case 2:
			sys.estadoManual = PUERTO_SERIAL;
			break;
		}
	} while (index != 0);
}