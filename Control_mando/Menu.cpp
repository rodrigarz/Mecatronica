
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
//uint8_t placaServo[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t placaServo[] = {0xB4, 0xE6, 0x2D, 0xFB, 0x1B, 0xBD};
uint8_t placaControl2[] = {0x10, 0x91, 0xA8, 0x19, 0xC8, 0xF4};


struct struct_message data = {};
struct mensaje_control mensaje;

//Caracteres para los menus
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

//Funcion para leer el encoder
int16_t leeEncoder()
{
	return(rotaryEncoder.readEncoder());
}

//Funcion para leer el incremento del encoder
int8_t deltaEncoder()
{
	int8_t encoderDelta = rotaryEncoder.encoderChanged();
	return encoderDelta;
}

//Funcion para inicializar el encoder
void inicializaRotaryEncoder()
{
	rotaryEncoder.begin();
	rotaryEncoder.setup([] {rotaryEncoder.readEncoder_ISR(); });
	rotaryEncoder.setBoundaries(-10000, 10000, true);
}

//Funcion para inicializar los LCD
void inicializacion()
{
  inicializaLcd(lcd1);
  inicializaLcd(lcd2);
  lcd1.backlight();
  lcd2.backlight();
  data.estado = 0;
}

//Funcion para inicializar uno de los LCD
void inicializaLcd(LiquidCrystal_I2C &display)
{
	display.init();
	display.backlight();
	display.createChar(0, menuCursor);
	display.createChar(1, upArrow);
	display.createChar(2, downArrow);
}

//Escribe en LCD 1
void escribeLcd(String mensaje1, String mensaje2)
{
	lcd1.setCursor(0, 0);
	lcd1.print(mensaje1);
	for (int i = mensaje1.length(); i < lcdColumns; i++) lcd1.print(" ");
	lcd1.setCursor(0, 1);
	lcd1.print(mensaje2);
	for (int i = mensaje2.length(); i < lcdColumns; i++) lcd1.print(" ");
}

//Escribe en el LCD indicado como parametro
void escribeLcd1(String mensaje, LiquidCrystal_I2C &display)
{
	display.setCursor(0, 0);
	display.print(mensaje);
	for (int i = mensaje.length(); i < lcdColumns; i++) display.print(" ");
}

//Funcion que indica el estado del boton del rotary
bool botonEncoderPulsado()
{
	if (rotaryEncoder.currentButtonState() == BUT_RELEASED)
		return false;
	else
		return true;
}

//Funcion para gestionar menu
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

//Funcion para mostrar menu
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


//Funcion para variar el valor de un double con el encoder
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

//Funcion para variar el valor de un int con el encoder
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


//Funcion con los datos del menu principal
void menuPrincipal()
{
	String menu[] = {"Manual", "Automatico", "Ajustes" };
	String opDefecto[3];
	int index = 0;
	lcd2.clear();

	do
	{
		if (data.control == AUTOMATICO)
		{
			opDefecto[0] = "";
			opDefecto[1] = "*";
		}
		else
		{
			opDefecto[0] = "*";
			opDefecto[1] = "";
		}
		index = miMenu(menu, 3, opDefecto, index, lcd1);

		switch (index)
		{
		case 0:
			data.control = MANUAL;
			menuManual();
			break;
		case 1:
			data.control = AUTOMATICO;
			mandarDatos();
			break;
		case 2:
			menuAjustes();
			break;
		}
	} while (index == 1);
}

//Funcion con los datos del menu de ajustes
void menuAjustes()
{
	String menu[] = { "Volver", "Control", "Parametros" };
	int index = 0;
	String opDefecto[3];
	lcd2.clear();

	do
	{
		opDefecto[1] = (data.controlCinta == VELOCIDAD ? "Velocidad" : "Posicion");
		index = miMenu(menu, 3, opDefecto, index, lcd1);
		
		switch (index)
		{
		case 1:
			data.controlCinta = (data.controlCinta == VELOCIDAD ? POSICION : VELOCIDAD);
			break;
		case 2:
			menuParametros();
			break;
		}
	} while (index != 0);
}

//Funcion con los datos y funcionalidad del menu de parametros, permite grabar o borrar en eeprom
void menuParametros()
{
	String menu[] = { "Volver", "Param. pos", "Grabar param", "Borrar param" };
	int index = 0;
	String opDefecto[4];
	lcd1.clear();
	lcd1.setCursor(1, 0);
	lcd1.print("Parametros");
	static long int tiempo = millis();

	lcd1.setCursor(0, 1);
	do
	{
		tiempo = millis();

		index = miMenu(menu, 4, opDefecto, index, lcd1);

		switch (index)
		{
		case 1:
			menuKpos();
			break;
		case 2:
			settingsSaveToEEprom();
			escribeLcd1("Grabando", lcd2);
			do
			{

			} while (millis() - tiempo < 2000);
			break;
		case 3:
			settingsWipe();
			escribeLcd1("Borrando param", lcd2);
			do
			{
			
			} while (millis() - tiempo < 2000);
			break;
		}
	} while (index != 0);

}

//Funcion con los valores de los parametros del PID en posicion
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
			data.kPZMPos = dameValor(menu[index], data.kPZMPos, 0.1, 0, 1000);

			break;
		case 2:                     ////--Kd Zona Muerta--////
			data.kDZMPos = dameValor(menu[index], data.kDZMPos, 0.01, 0, 1000);

			break;
		case 3:                     ////--Ki Zona Muerta--////
			data.kIZMPos = dameValor(menu[index], data.kIZMPos, 0.01, 0, 1000);
			break;
		case 4:                     ////--Kp--////
			data.kPPos = dameValor(menu[index], data.kPPos, 0.1, 0, 1000);

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


//Menu manual, permite elegir entre manual con encoder o por puerto serie
void menuManual()
{
	data.control = true;
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


//Manual con enconder, permite fijar el valor de los actuadores mediante el rotary encoder
void menuManualEncoder()
{
  String menu[] = {"Volver", "Grados Servo", "Expulsor", "Grados mesa", "Pos PaP", "Mov Cinta"};
  int index = 0;
  String opDefecto[6];
  lcd2.clear();
  do
  {
   opDefecto[1] = String(data.posServo);
   //opDefecto[2] = String(data.posExpulsor);
   opDefecto[3] = String(data.gradosPaP);
   //opDefecto[4] = String(data.posPap);
   index = miMenu(menu, 6, opDefecto, index, lcd2);
   switch(index)
   {
    case 1:
      data.posServo = dameValorInt(menu[index], data.posServo, 5, 0, 75);
      data.indicacion = 1;
      mandarDatos();
		  break;
    
    case 2:
      data.posExpulsor = dameValorInt(menu[index], data.posExpulsor, 1, 0, 2);
      data.indicacion = 2;
      mandarDatos();
      break;
    
    case 3:
      data.gradosPaP = dameValorInt(menu[index], data.gradosPaP, 5, -1000, 1000);
      data.indicacion = 3;
      data.velPap = 281.25;
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

//Menu para el movimiento de la cinta, cambia entre control posicion y velocidad, y permite fijar el setpoint
void menuMoverCinta()
{
  String menu[] = {"Volver", "Tipo control", "SetPoint"};
  int index = 0;
  String opDefecto[3];
  lcd2.clear();
  do
  {
    opDefecto[1] = (data.controlCinta == VELOCIDAD ? "Velocidad":"Posicion");
    index = miMenu(menu, 3, opDefecto, index, lcd2);
    switch(index)
    {
      case 0:
        data.indicacion = 0;
        mandarDatos();
      case 1:
        data.controlCinta=(data.controlCinta==VELOCIDAD?POSICION:VELOCIDAD);
        break;
      case 2:
        data.setPoint = dameValor(menu[index], data.setPoint, 10, -1000, 1000);
        if(data.controlCinta == false)
        {
          data.indicacion=5;
        }
        else
        {
          data.indicacion=6;
        }
        mandarDatos();
        break;
    }
  } while(index!=0);
  data.indicacion = 0;
  mandarDatos();
}

//Menu para dar instrucciones por el puerto serial, si no existe la instruccion indicada, sale
void menuPuertoSerial()
{
  String comando, entrada;
  String mmesa = "mmesa";
  String mexpulsor = "mexpulsor";
  String mcintav = "mcintav";
  String mcintap = "mcintap";
  double primero, segundo;
  int salir = 0;
  bool mandar = true;

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
      String partes[3];
      int numPartes = splitString(entrada, ' ', partes);
      if(numPartes == 3)
      {
        lcd2.setCursor(0, 0);
        lcd2.print("Recibido:");
        lcd2.setCursor(0, 1);
        comando = partes[0];
        primero = partes[1].toInt();
        segundo = partes[2].toDouble();
        lcd2.print(comando);
        mandar = true;
      } else if(numPartes == 2)
      {
        lcd2.setCursor(0, 0);
        lcd2.print("Recibido:");
        lcd2.setCursor(0, 1);
        comando = partes[0];
        primero = partes[1].toInt();
        lcd2.print(String(comando));
        mandar = true;
        
      } else{
        salir = 1;
        lcd2.setCursor(0, 0);
        lcd2.print("Error al leer");
        mandar = false;
      }
      
    }
    if(comando == mmesa)
    {
		data.indicacion = 3;
		data.gradosPaP = primero;
		data.velPap = segundo;

    } else if(comando == mcintav)
    {
		  data.indicacion = 5;
		  data.velCinta = primero;
      data.setPoint = data.velCinta*19;
    } else if(comando == mcintap)
    {
		  data.indicacion = 6;
		  data.incrCinta = primero;
      data.setPoint = data.incrCinta*19;
    } else if (comando == mexpulsor)
    {
		data.indicacion = 2;
		data.posExpulsor = primero;
    }

    if(mandar == true)
    {
      mandarDatos();
    }
    delay(2000);
    lcd2.clear();
  } while (salir != 1);
  data.indicacion = 0;
  mandarDatos();
}

//Funcion para mandar los datos al esclavo de la planta
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


//Funcion para separar la cadena introducida por el puerto serie
int splitString(String input, char delimiter, String parts[])
 {
  int partIndex = 0;
  int startIndex = 0;
  int endIndex = 0;

  while (endIndex >= 0) {
    endIndex = input.indexOf(delimiter, startIndex);

    if (endIndex >= 0) {
      parts[partIndex] = input.substring(startIndex, endIndex);
    } else {
      parts[partIndex] = input.substring(startIndex);
    }

    startIndex = endIndex + 1;
    partIndex++;
  }

  return partIndex;
}

