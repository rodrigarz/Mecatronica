#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Menu.h"
#include <ESP32Encoder.h>
#include <AiEsp32RotaryEncoder.h>


int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd1(0x27, lcdColumns, lcdRows);
LiquidCrystal_I2C lcd2(0x3F, lcdColumns, lcdRows);

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
//LiquidCrystal_I2C lcd1(0x27, lcdColumns, lcdRows);
//LiquidCrystal_I2C lcd2(0x30, lcdColumns, lcdRows);

//String menuItems[] = {"ITEM 1", "ITEM 2", "ITEM 3", "ITEM 4", "ITEM 5", "ITEM 6"};
//int readKey;

// Menu control variables
/*
int menuPage = 0;
int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
int maxMenuItems = round(sizeof(menuItems) / sizeof(String));
int cursorPosition = 0;

*/

// Creates 3 custom characters for the menu display
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

//Incializaci�n encoder rotativo HW-040 


AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN);

int16_t leeEncoder() {
    return(rotaryEncoder.readEncoder());
}

int8_t deltaEncoder() {
    int8_t encoderDelta = rotaryEncoder.encoderChanged();
    return encoderDelta;

}

void incializaRotaryEncoder() {
    rotaryEncoder.begin();
    rotaryEncoder.setup([] {rotaryEncoder.readEncoder_ISR(); });
    //optionally we can set boundaries and if values should cycle or not
    rotaryEncoder.setBoundaries(-10000, 10000, true); //minValue, maxValue, cycle values (when max go to min and vice versa)
}

void inicializaLcd() {
    lcd1.init();// initialize LCD                  
    lcd1.backlight(); // turn on LCD backlight 
    // Creates the byte for the 3 custom characters
    lcd1.createChar(0, menuCursor);
    lcd1.createChar(1, upArrow);
    lcd1.createChar(2, downArrow);
    lcd2.init();// initialize LCD                  
    lcd2.backlight(); // turn on LCD backlight 
    // Creates the byte for the 3 custom characters
    lcd2.createChar(0, menuCursor);
    lcd2.createChar(1, upArrow);
    lcd2.createChar(2, downArrow);
}

void escribeLcd(String mensaje1, String mensaje2, String mensaje3, String mensaje4) {
    //lcd.clear();
    lcd1.setCursor(0, 0);
    lcd2.setCursor(0, 0);
    lcd1.print(mensaje1);
    for (int i = mensaje1.length(); i < lcdColumns; i++) lcd1.print(" ");
    lcd2.print(mensaje3);
    for (int i = mensaje3.length(); i < lcdColumns; i++) lcd2.print(" ");
    lcd1.setCursor(0, 1);
    lcd2.setCursor(0, 1);
    lcd1.print(mensaje2);
    for (int i = mensaje2.length(); i < lcdColumns; i++) lcd1.print(" ");
    lcd2.print(mensaje4);
    for (int i = mensaje4.length(); i < lcdColumns; i++) lcd2.print(" ");
}
void escribeLcd1(String mensaje, String mensaje2) {
    //lcd.clear();
    lcd1.setCursor(0, 0);
    lcd2.setCursor(0, 0);
    lcd1.print(mensaje);
    for (int i = mensaje.length(); i < lcdColumns; i++) lcd1.print(" ");
    lcd2.print(mensaje2);
    for (int i = mensaje2.length(); i < lcdColumns; i++) lcd2.print(" ");
}



bool botonEncoderPulsado() {
    if (rotaryEncoder.currentButtonState() == BUT_RELEASED)
        return true;
    else
        return false;
}


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////*******Menu Generico ******///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

int miMenu(String menu[], int maxMenuItems, String opDefecto[], int nMenuOpDef) {
    int valEncoder, valEncoderAnt;
    int opcionMenu = nMenuOpDef + 1;
    static long int tiempo = millis();


    valEncoderAnt = rotaryEncoder.readEncoder();
    muestraMenu(menu, maxMenuItems, opDefecto, opcionMenu);
    while (rotaryEncoder.currentButtonState() != BUT_RELEASED) {
        valEncoder = rotaryEncoder.readEncoder();
        if (valEncoder > valEncoderAnt) {
            opcionMenu++;
            opcionMenu = constrain(opcionMenu, 1, maxMenuItems);
            muestraMenu(menu, maxMenuItems, opDefecto, opcionMenu);
            valEncoderAnt = valEncoder;
        }
        else if (valEncoder < valEncoderAnt) {
            opcionMenu--;
            opcionMenu = constrain(opcionMenu, 1, maxMenuItems);
            muestraMenu(menu, maxMenuItems, opDefecto, opcionMenu);
            valEncoderAnt = valEncoder;
        }
        else {      // Entra cada cierto tiempo
            if (millis() - tiempo > 400) {
                if (menu[opcionMenu - 1].length() + opDefecto[opcionMenu - 1].length() >= lcdColumns - 1)                 // Solo se llama a la funcion si el texto es largo
                    muestraMenu(menu, maxMenuItems, opDefecto, opcionMenu);
                tiempo = millis();
            }
        }
    }
    lcd1.clear();
    lcd2.clear();
    delay(100);
    return opcionMenu - 1;
}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////*******Muestra Menu ******///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void muestraMenu(String menu[], int maxMenuItems, String opDefecto[], int opcionMenu) {
    int numPags, pag;
    String linea;
    //int maxTam=14;
    static int k = 0;
    int menuMaxLineas = 4;



    numPags = round((float)maxMenuItems / menuMaxLineas + 0.4999);
    pag = (opcionMenu - 1) / menuMaxLineas;
    lcd1.setCursor(0, 0);
    lcd1.clear();
    lcd2.setCursor(0, 0);
    lcd2.clear();
    //Serial.println("NumPags: "+String(numPags));
    //Serial.println("Pag: "+String(pag));
    //Serial.println("opcionMenu: "+String(opcionMenu));
    if (pag == 0) { // Primera p�gina
        for (int i = 0; i < (maxMenuItems < menuMaxLineas ? maxMenuItems : menuMaxLineas); i++) {
            lcd1.setCursor(0, i);
            lcd2.setCursor(0, i);
            linea = menu[i] + " " + opDefecto[i];
            if (opcionMenu == i + 1) {
                lcd1.write(byte(0));

                if (linea.length() <= lcdColumns - 1)
                    lcd1.print(linea);
                else {
                    lcd1.print(linea.substring(0, lcdColumns - 1));
					lcd2.print(linea.substring(lcdColumns - 1, lcdColumns));
                }
            }
            else {
                lcd1.print(" ");
                if (linea.length() <= lcdColumns - 1)
                    lcd1.print(linea);
                else {
                    lcd1.print(linea.substring(0, lcdColumns - 1));
                    lcd2.print(linea.substring(lcdColumns - 1, lcdColumns));
                }
            }


            //display.print(menu[i]+" "+opDefecto[i]);
            //display.startscrollleft(0x00, 0x0F);
        }
    }
    else if (pag + 1 == numPags) { // Ultima Pagina
        for (int i = 0; i < menuMaxLineas; i++) {
            lcd1.setCursor(0, i);
            lcd2.setCursor(0, i);
            linea = menu[maxMenuItems - menuMaxLineas + i] + " " + opDefecto[maxMenuItems - menuMaxLineas + i];
            if (opcionMenu == maxMenuItems - menuMaxLineas + i + 1) {
                lcd1.write(byte(0));

                if (linea.length() <= lcdColumns - 1)
                    lcd1.print(linea);
                else {
                    lcd1.print(linea.substring(0, lcdColumns - 1));
                    lcd2.print(linea.substring(lcdColumns - 1, lcdColumns));
                }
            }
            else {
                lcd1.print(" ");;
                if (linea.length() <= lcdColumns - 1)
                    lcd1.print(linea);
                else {
                    lcd1.print(linea.substring(0, lcdColumns - 1));
                    lcd2.print(linea.substring(lcdColumns - 1, lcdColumns));
                }
            }


            //display.print(menu[maxMenuItems-menuMaxLineas+i]+" "+opDefecto[maxMenuItems-menuMaxLineas+i]);
            //display.startscrollleft(0x00, 0x0F);
        }
    }
    else {
        for (int i = 0; i < menuMaxLineas; i++) {
            lcd1.setCursor(0, i);
            lcd2.setCursor(0, i);
            linea = menu[pag * menuMaxLineas + i] + " " + opDefecto[pag * menuMaxLineas + i];
            if (opcionMenu == pag * menuMaxLineas + i + 1) {
                lcd1.write(byte(0));
                if (linea.length() <= lcdColumns - 1)
                    lcd1.print(linea);
                else {
                    lcd1.print(linea.substring(0, lcdColumns - 1));
                    lcd2.print(linea.substring(lcdColumns - 1, lcdColumns));
                }
            }
            else {
                lcd1.print(" ");
                if (linea.length() <= lcdColumns - 1)
                    lcd1.print(linea);
                else {
                    lcd1.print(linea.substring(0, lcdColumns - 1));
                    lcd2.print(linea.substring(lcdColumns - 1, lcdColumns));
                }
            }


            //display.startscrollleft(0x00, 0x0F);
        }
    }


}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////*******dameValor ******///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

double dameValor(String cadena, double valor, double inc, double min, double max) {
    int valEncoder, valEncoderAnt;
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd2.clear();
    lcd2.setCursor(0, 0);
    lcd1.print(cadena + String(":"));
    lcd2.setCursor(0, 1);
    lcd2.print(valor);
    valEncoderAnt = rotaryEncoder.readEncoder();
    while (rotaryEncoder.currentButtonState() != BUT_RELEASED) {
        valEncoder = rotaryEncoder.readEncoder();
        if (valEncoder > valEncoderAnt) {
            valor = valor + inc;
            valor = constrain(valor, min, max);
            valEncoderAnt = valEncoder;
            lcd1.setCursor(0, 0);
            lcd1.clear();
            lcd2.clear();
            lcd2.setCursor(0, 0);
            lcd1.print(cadena + String(":"));
            lcd2.setCursor(0, 1);
            lcd2.print(valor);
            Serial.println("Valor: " + String(valor));
        }
        else if (valEncoder < valEncoderAnt) {
            valor = valor - inc;
            valor = constrain(valor, min, max);
            valEncoderAnt = valEncoder;
            lcd1.setCursor(0, 0);
            lcd1.clear();
            lcd2.clear();
            lcd2.setCursor(0, 0);
            lcd1.print(cadena + String(":"));
            lcd2.setCursor(0, 1);
            lcd2.print(valor);
            Serial.println("Valor: " + String(valor));
        }
    }
    delay(100);
    Serial.println("Valor: " + String(valor));
    return valor;
}


void menuPrincipal() {
    String menu[] = { "Volver", "Opcion A","Opcion B","Opcion C" };
    String opDefecto[4];
    int index = 0;
    miMenu(menu, 8, opDefecto, 0);
    escribeLcd1("Pantalla 1","Pantalla 2");
}

