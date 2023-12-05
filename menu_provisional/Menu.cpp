
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

//uint8_t placaServo[] = {0x80, 0x7D, 0x3A, 0xFD, 0x0D, 0x50}; //Direccion placa rodrigo (la de mas pines)
uint8_t placaServo[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


struct struct_message data;

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

void inicializaLcd(LiquidCrystal_I2C &display)
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

void escribeLcd1(String mensaje, LiquidCrystal_I2C &display)
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
    if(valEncoderAnt != valEncoder)
    {
      Serial.println(valEncoder);
    }
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

void muestraMenu(String menu[], int maxMenuItems, String opDefecto[], int opcionMenu, LiquidCrystal_I2C &display)
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
      } else {
        display.print(" ");
      }

      if (linea.length() <= lcdColumns) {
        display.print(linea);
        // Rellena con espacios si la línea es más corta que el ancho de la pantalla
        for (int j = linea.length(); j < lcdColumns; j++) {
          display.print(" ");
        }
      } else {
        // Muestra solo la parte visible en una línea
        display.print(linea.substring(0, lcdColumns));
      }
    }
  }
}



double dameValor(String cadena, double valor, double inc, double min, double max)
{
	int valEncoder, valEncoderAnt;
  int cont = 0;
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
    cont = cont + 1;
    if (cont == 10)
    {
      lcd2.clear();
      cont = 0;
    }
    lcd2.setCursor(0, 0);
	  lcd2.print(cadena + String(":"));
		lcd2.setCursor(0, 1);
		lcd2.print(valor);
	}
	delay(100);
  lcd2.clear();
	return valor;
}


int dameValorInt(String cadena, int valor, int inc, int min, int max)
{
	int valEncoder, valEncoderAnt;
  int cont = 0;
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
    cont = cont + 1;
    if (cont == 10)
    {
      lcd2.clear();
      cont = 0;
    }
    lcd2.setCursor(0, 0);
	  lcd2.print(cadena + String(":"));
		lcd2.setCursor(0, 1);
		lcd2.print(valor);
	}
	delay(100);
  lcd2.clear();
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
		if (data.estado == AUTOMATICO)
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
			data.estado = MANUAL;
      menuManual();
			break;
		case 2:
			data.estado = AUTOMATICO;
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
		opDefecto[1] = (data.control == VELOCIDAD ? "Velocidad" : "Posicion");
		index = miMenu(menu, 3, opDefecto, index, lcd1);
		
		switch (index)
		{
		case 1:
			data.control = (data.control == VELOCIDAD ? POSICION : VELOCIDAD);
			break;
		case 2:
			menuParametros();
			break;
		}
	} while (index != 0);
}

void menuParametros()
{
	String menu[] = { "Volver", "PosServo", "Periodo", "Param vel.", "Param. pos", "Grabar param", "Borrar param" };
	int index = 0;
	String opDefecto[7];
	lcd1.clear();
	lcd1.setCursor(1, 0);
	lcd1.print("Parametros");
	static long int tiempo = millis();

	lcd1.setCursor(0, 1);
	do
	{
		opDefecto[1] = String(data.setPoint);
		opDefecto[2] = String(data.periodo);
		tiempo = millis();

		index = miMenu(menu, 7, opDefecto, index, lcd1);

		switch (index)
		{
		case 1:
			data.setPoint = dameValorInt(menu[index], data.setPoint, 10, -1000, 1000);
			break;
		case 2:
			data.periodo = dameValor(menu[index], data.periodo, 0.05, 0, 1000);
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
		opDefecto[1] = String(data.kPZMVel);
		opDefecto[2] = String(data.kDZMVel);
		opDefecto[3] = String(data.kIZMVel);
		opDefecto[4] = String(data.kPVel);
		opDefecto[5] = String(data.kDVel);
		opDefecto[6] = String(data.kIVel);

		index = miMenu(menu, 7, opDefecto, index, lcd1);
		switch (index) {
		case 1:                     ////--Kp Zona muerta--////
			data.kPZMVel = dameValor(menu[index], data.kPZMVel, 0.01, 0, 1000);
			break;
		case 2:                     ////--Kd Zona Muerta--////
			data.kDZMVel = dameValor(menu[index], data.kDZMVel, 0.01, 0, 1000);
			break;
		case 3:                     ////--Ki Zona Muerta--////
			data.kIZMVel = dameValor(menu[index], data.kIZMVel, 0.01, 0, 1000);
			break;
		case 4:                     ////--Kp--////
			data.kPVel = dameValor(menu[index], data.kPVel, 0.01, 0, 1000);
			break;
		case 5:                     ////--Kd--////
			data.kDVel = dameValor(menu[index], data.kDVel, 0.01, 0, 1000);
			break;
		case 6:                     ////--Ki--////
			data.kIVel = dameValor(menu[index], data.kIVel, 0.01, 0, 1000);
			break;
		}
	} while (index != 0);
}

void menuKpos() {
	String menu[] = { "Volver", "KpZMPos","KdZMPos","KiZMPos","KpPos","KdPos","KiPos" };
	int index = 0;
	String opDefecto[7];

	do {
		opDefecto[1] = String(data.kPZMPos);
		opDefecto[2] = String(data.kDZMPos);
		opDefecto[3] = String(data.kIZMPos);
		opDefecto[4] = String(data.kPPos);
		opDefecto[5] = String(data.kDPos);
		opDefecto[6] = String(data.kIPos);

		index = miMenu(menu, 7, opDefecto, index, lcd1);
		switch (index) {
		case 1:                     ////--Kp Zona muerta--////
			data.kPZMPos = dameValor(menu[index], data.kPZMPos, 0.01, 0, 1000);

			break;
		case 2:                     ////--Kd Zona Muerta--////
			data.kDZMPos = dameValor(menu[index], data.kDZMPos, 0.01, 0, 1000);

			break;
		case 3:                     ////--Ki Zona Muerta--////
			data.kIZMPos = dameValor(menu[index], data.kIZMPos, 0.01, 0, 1000);
			break;
		case 4:                     ////--Kp--////
			data.kPPos = dameValor(menu[index], data.kPPos, 0.01, 0, 1000);

			break;
		case 5:                     ////--Kd--////
			data.kDPos = dameValor(menu[index], data.kDPos, 0.01, 0, 1000);

			break;
		case 6:                     ////--Ki--////
			data.kIPos = dameValor(menu[index], data.kIPos, 0.01, 0, 1000);
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
		if (data.estadoManual == MANUAL_ENC)
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
			data.estadoManual = MANUAL_ENC;
      menuManualEncoder();
			break;
		case 2:
			data.estadoManual = PUERTO_SERIAL;
      menuPuertoSerial();
			break;
		}
	} while (index != 0);
}

void menuManualEncoder()
{
  String menu[] = {"Volver", "Grados Servo", "Expulsor", "Grados mesa", "Pos PaP", "Mov Cinta"};
  int index = 0;
  String opDefecto[6];
  lcd2.clear();
  do
  {
   opDefecto[1] = String(data.posServo);
   opDefecto[2] = String(data.posExpulsor);
   opDefecto[3] = String(data.gradosPaP);
   opDefecto[4] = String(data.posPap);
   index = miMenu(menu, 5, opDefecto, index, lcd2);
   switch(index)
   {
    case 1:
    data.posServo = dameValorInt(menu[index], data.posServo, 5, 0, 180);
    data.indicacion = 1;
    mandarDatos();
		break;
    case 2:
    data.posExpulsor = dameValorInt(menu[index], data.posExpulsor, 1, 0, 1);
    data.indicacion = 2;
    mandarDatos();
    break;
    case 3:
    data.gradosPaP = dameValorInt(menu[index], data.gradosPaP, 5, -1000, 1000);
    data.indicacion = 3;
    mandarDatos();
    break;
    case 4:
    data.posPap = dameValorInt(menu[index], data.posPap, 1, 1, 3);
    data.indicacion = 4;
    mandarDatos();
    break;
    case 5:
    menuMoverCinta();
    break;
   }
  }while (index != 0);

}

void menuMoverCinta()
{
  String menu[] = {"Volver", "Tipo control", "SetPoint"};
  int index = 0;
  String opDefecto[3];
  lcd2.clear();
  do
  {
    opDefecto[1] = (data.control == VELOCIDAD ? "Velocidad":"Posicion");
    index = miMenu(menu, 3, opDefecto, index, lcd2);
    switch(index)
    {
      case 1:
        data.control=(data.control==VELOCIDAD?POSICION:VELOCIDAD);
        break;
      case 2:
        data.setPoint = dameValor(menu[index], data.setPoint, 10, -10000, 10000);
        mandarDatos();
        break;
    }
  } while(index!=0);
}

void menuPuertoSerial()
{
  String comando, entrada;
  double primero, segundo;
  int salir = 0;

  lcd1.clear();
  lcd2.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Escuchando");
  lcd1.setCursor(0, 1);
  lcd1.print("Puerto Serial");
  do
  {
    do
    {}while(!Serial.available());
    if(Serial.available() != 0)
    {
      String entrada = Serial.readStringUntil('\n');
      if(sscanf(entrada.c_str(), "%s %lf %lf", comando, &primero, &segundo) == 3)
      {
        lcd2.setCursor(0, 0);
        lcd2.print("Recibido:");
        lcd2.setCursor(0, 1);
        lcd2.print(comando);
      } else if(sscanf(entrada.c_str(), "%s %lf", comando, &primero) == 2)
      {
        lcd2.setCursor(0, 0);
        lcd2.print("Recibido:");
        lcd2.setCursor(0, 1);
        lcd2.print(String(comando));
        
      } else{
        salir = 1;
        lcd2.setCursor(0, 0);
        lcd2.print("Error al leer");
      }
      
    }
    if(comando == "mmesa")
    {
      data.gradosPaP = primero;
      data.velPap = segundo;
      lcd2.setCursor(0, 1);
      lcd2.print("mmesa");

    } else if(comando == "mcintav")
    {
      data.velCinta = primero;
      lcd2.setCursor(0, 1);
      lcd2.print("mcintav");
    } else if(comando == "mcintap")
    {
      data.incrCinta = primero;
      lcd2.setCursor(0, 1);
      lcd2.print("mcintap");
    } else if (comando == "mexpulsor")
    {
      data.posExpulsor = primero;
      lcd2.setCursor(0, 1);
      lcd2.print("mexpulsor");
    }
    mandarDatos();
    delay(2000);
    lcd2.clear();
  } while (salir != 1);
}

void mandarDatos()
{
  esp_err_t result = esp_now_send(placaServo, (uint8_t *) &data, sizeof(data));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
   Serial.println("Error sending the data");
  }
}