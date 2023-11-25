
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "AiEsp32RotaryEncoder.h"
#include "Menu.h"
#include "Globales.h"

extern AiEsp32RotaryEncoder myEnc;

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

void incializaRotaryEncoder()
{
	rotaryEncoder.begin();
	rotaryEncoder.setup([] {rotaryEncoder.readEncoder_ISR(); });
	rotaryEncoder.setBoundaries(-10000, 10000, true);
}

void inicializaLcd(LiquidCrystal_I2C display)
{
	display.init();
	display.backlight();
	display.createChar(0, menuCursor);
	dìsplay.createChar(1, upArrow);
	display.createChar(2, downArrow);
}

void escribeLcd(String mensaje1, String mensaje2, LiquidCrystal_I2C display)
{
	display.setCursor(0, 0);
	display.print(mensaje1);
	for (int i = mensaje1.length(); i < lcdColumns; i++) display.print(" ");
	display.setCursor(0, 1);
	display.print(mensaje2);
	for (int i = mensaje2.length(); i < lcdColumns; i++) display.print(" ");
}

void escribeLcd1(String mensaje, LiquidCrystal_I2C display)
{
	display.setCursor(0, 0);
	diplay.print(mensaje);
	for (int i = mensaje.length(); i < lcdColumns; i++) display.print(" ");
}

bool botonEncoderPulsado()
{
	if (rotaryEncoder.currentButtonState() == BUT_RELEASED)
		return false;
	else
		return true;
}

int miMenu(String menu[], int maxMenuItems, String opDefecto[], int nMenuOpDef, LiquidCrystal_I2C display)
{
	int valEncoder, valEncoderAnt;
	int opcionMenu = nMenuOpDef + 1;
	static long int tiempo = millis();

	valEncoderAnt = leeEncoder();
	muestraMenu(menu, maxMenuItems, opDefecto, opcionMenu, display);

	while (rotaryEncoder.currentButtonState() != BUT_RELEASED)
	{
		valEncoder = leeEncoder();
		if (valEncoder > valEncoderAnt)
		{
			opcionMenu++;
			opcionMenu = constrain(opcionMenu, 1, maxMenuItems);
			muestraMenu(menu, maxMenuItems, opDefecto, opcionMenu, display);
			valEncoderAnt = valEncoder;
		}
		else if (valEncoder < valEncoderAnt)
		{
			opcionMenu--;
			opcionMenu = constrain(opcionMenu, 1, maxMenuItems);
			muestraMenu(menu, maxMenuItems, opDefecto, opcionMenu, display);
			valEncoderAnt = valEncoder;
		}
		else
		{
			if (millis() - tiempo > 400)
			{
				if (menu[opcionMenu - 1].length() + opDefecto[opcionMenu - 1].length() >= lcdColumns - 1)
					muestraMenu(menu, maxMenuItems, opDefecto, opcionMenu, display);
				tiempo = millis();
			}
		}
	}
	do
	{

	} while (millis() - tiempo < 100);
	return opcionMenu - 1;
}

void muestraMenu(String menu[], int maxMenuItems, String opDefecto[], int opcionMenu, LiquidCrysyal_I2C display)
{
	int numPags, pag;
	String linea;
	static int k = 0;
	int menuMaxLineas = 2;

	numPags = round((float)maxMenuItems / menuMaxLineas + .5);
	pag = (opcionMenu - 1) / menuMaxLineas;

	display.setCursor(0, 0);
	display.clear();
	if (pag == 0)
	{
		for (int i = 0; i < (maxMenuItems < menuMaxLineas ? maxMenuItems : menuMaxLineas); i++) // Mientras que i sea menor que el menor de maxMenuItems y menuMaxLineas
		{
			display.setCursor(0, i);
			linea = menu[i] + " " + opDefecto[i];
			if (opcionMenu == i + 1)
			{
				menuDesplazamiento(linea);
			}
			else
			{
				display.print(" ");
				if (linea.length() <= lcdColumns - 1)
					display.print(linea);
				else
					display.print(linea.substring(0, lcdColumns - 1));
			}

		}
	}
	else if (pag + 1 == numPags)
	{
		for (int i = 0; i < menuMaxLineas; i++)
		{
			display.setCursor(0, i);
			linea = menu[maxMenuItems - menuMaxLineas + 1] + " " + opDefecto[maxMenuItems - menuMaxLienas + i];
			if (opcionMenu == maxMenuItems - menuMaxLineas + i + 1)
			{
				menuDesplazamiento(linea);
			}
			else
			{
				display.print(" ");
				if (linea.length() <= lcdColumns - 1)
					dispay.print(linea);
				else
					display.print(linea.substring(0, lcdColumns - 1));
			}
		}
	}
	else
	{
		for (int = 0; i < menuMaxLineas; i++)
		{
			display.setCursor(0, i);
			linea = menu[pag * menumaxLineas + i] + " " + opDefecto[pag * menuMaxLineas + i];
			if (opcionMenu == pag * menuMaxLineas + i + )
			{
				menuDesplazamiento(linea);
			}
			else
			{
				display.print(" ");
				if (linea.length() <= lcdColumns - 1)
					display.print(linea);
				else
					display.print(linea.substring(0, lcdColumns - 1));
			}
		}
	}
}

void menuDesplazamiento(String linea)
{
	display.write(byte(0));
	if (linea.length() <= lcdColumns - 1)
		display.print(linea);
	else
	{
		if (linea.length() > lcdColumns - 1 - k)
			k++;
		else
			k = 0;
		display.print(linea.substring(k, lcdColumns - 1 + k));
		if (k + lcdColumns - 1 > linea.length())
			display.print(" " + linea.substring(0, k + lcdColumns - 1 - linea.length() - 1));
	}
}

//Por dame valor