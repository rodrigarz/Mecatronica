
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "AiEsp32RotaryEncoder.h"
#include "Menu.h"
#include "Globales.h"
#include "Eeprom.h"

extern PID myPID;
extern double Setpoint;
//extern Encoder myEnc;
extern AiEsp32RotaryEncoder myEnc;
// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(DIRLCD, lcdColumns, lcdRows); 


//String menuItems[] = {"ITEM 1", "ITEM 2", "ITEM 3", "ITEM 4", "ITEM 5", "ITEM 6"};
//int readKey;

// Menu control variables
/*
int menuPage = 0;<
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

//Incialización encoder rotativo HW-040 


AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN);

int16_t leeEncoder(){
  return(rotaryEncoder.readEncoder());
}

int8_t deltaEncoder(){
    int8_t encoderDelta = rotaryEncoder.encoderChanged();
    return encoderDelta;
  
}

void incializaRotaryEncoder(){
  rotaryEncoder.begin();
  rotaryEncoder.setup([]{rotaryEncoder.readEncoder_ISR();});
  //optionally we can set boundaries and if values should cycle or not
  rotaryEncoder.setBoundaries(-10000, 10000, true); //minValue, maxValue, cycle values (when max go to min and vice versa)
}

void inicializaLcd(){
  lcd.init();// initialize LCD                  
  lcd.backlight(); // turn on LCD backlight 
    // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
} 

void escribeLcd(String mensaje1, String mensaje2){
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(mensaje1);
  for(int i=mensaje1.length();i<lcdColumns;i++) lcd.print(" ");
  lcd.setCursor(0, 1);
  lcd.print(mensaje2);
  for(int i=mensaje2.length();i<lcdColumns;i++) lcd.print(" ");
}
void escribeLcd1(String mensaje){
  //lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(mensaje);
  for(int i=mensaje.length();i<lcdColumns;i++) lcd.print(" ");
}



bool botonEncoderPulsado(){
    if(rotaryEncoder.currentButtonState()==BUT_RELEASED)
        return true;
    else
        return false;
}


////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////*******Menu Generico ******///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

int miMenu(String menu[],int maxMenuItems,   String opDefecto[], int nMenuOpDef){
  int valEncoder,valEncoderAnt;
  int opcionMenu=nMenuOpDef+1;
  static long int tiempo=millis();
  
 
  valEncoderAnt = rotaryEncoder.readEncoder();
  muestraMenu(menu, maxMenuItems,  opDefecto, opcionMenu);
  while (rotaryEncoder.currentButtonState() != BUT_RELEASED) {
    valEncoder = rotaryEncoder.readEncoder();
    if(valEncoder>valEncoderAnt){     
            opcionMenu++;
            opcionMenu = constrain(opcionMenu, 1, maxMenuItems);
            muestraMenu(menu, maxMenuItems,   opDefecto, opcionMenu);
            valEncoderAnt=valEncoder;
    }else if(valEncoder<valEncoderAnt){
            opcionMenu--;
            opcionMenu = constrain(opcionMenu, 1, maxMenuItems);
            muestraMenu(menu, maxMenuItems,   opDefecto, opcionMenu);
            valEncoderAnt=valEncoder;
    }else{      // Entra cada cierto tiempo
            if(millis()-tiempo>400) {
                if(menu[opcionMenu-1].length()+opDefecto[opcionMenu-1].length()>=lcdColumns-1)                 // Solo se llama a la funcion si el texto es largo
                    muestraMenu(menu, maxMenuItems, opDefecto,opcionMenu );
                tiempo=millis();
            }
    }
  }
  lcd.clear();
  delay(100);
  return opcionMenu-1;
}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////*******Muestra Menu ******///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void muestraMenu(String menu[], int maxMenuItems,  String opDefecto[], int opcionMenu){
  int numPags,pag;
  String linea;
  //int maxTam=14;
  static int k=0;
  int menuMaxLineas=2;
  
  
  
  numPags=round((float)maxMenuItems/menuMaxLineas+0.4999);
  pag=(opcionMenu-1)/menuMaxLineas;
  lcd.setCursor(0, 0);
  lcd.clear();
  //Serial.println("NumPags: "+String(numPags));
  //Serial.println("Pag: "+String(pag));
  //Serial.println("opcionMenu: "+String(opcionMenu));
  if(pag==0){ // Primera página
    for(int i=0; i<(maxMenuItems<menuMaxLineas?maxMenuItems:menuMaxLineas); i++){
      lcd.setCursor(0, i);
      linea=menu[i]+" "+opDefecto[i];
      if(opcionMenu==i+1){
        lcd.write(byte(0));

        if(linea.length()<=lcdColumns-1)
          lcd.print(linea);
        else{
          if(linea.length()>lcdColumns-1-k ){
             k++;
          }
          else k=0;
          lcd.print(linea.substring(0+k,lcdColumns-1+k));
          if(k+lcdColumns-1>linea.length()) lcd.print(" "+linea.substring(0,k+lcdColumns-1-linea.length()-1));
        }
      }
      else{
        lcd.print(" ");
        if(linea.length()<=lcdColumns-1)
          lcd.print(linea);
        else
          lcd.print(linea.substring(0,lcdColumns-1));
      }
      
      
      //display.print(menu[i]+" "+opDefecto[i]);
      //display.startscrollleft(0x00, 0x0F);
    }
  }else if(pag+1==numPags){ // Ultima Pagina
    for(int i=0; i<menuMaxLineas; i++){
      lcd.setCursor(0, i);
      linea=menu[maxMenuItems-menuMaxLineas+i]+" "+opDefecto[maxMenuItems-menuMaxLineas+i];
      if(opcionMenu==maxMenuItems-menuMaxLineas+i+1){
        lcd.write(byte(0));
        
        if(linea.length()<=lcdColumns-1)
          lcd.print(linea);
        else{
         if(linea.length()>lcdColumns-1-k )k++;
         else k=0;
         lcd.print(linea.substring(0+k,lcdColumns-1+k));
         if(k+lcdColumns-1>linea.length()) lcd.print(" "+linea.substring(0,k+lcdColumns-1-linea.length()-1));
        }
      }
      else{
        lcd.print(" ");;
        if(linea.length()<=lcdColumns-1)
          lcd.print(linea);
        else
          lcd.print(linea.substring(0,lcdColumns-1));
      }
      
      
      //display.print(menu[maxMenuItems-menuMaxLineas+i]+" "+opDefecto[maxMenuItems-menuMaxLineas+i]);
      //display.startscrollleft(0x00, 0x0F);
    }
  }else{
    for(int i=0; i<menuMaxLineas; i++){
      lcd.setCursor(0, i);
      linea=menu[pag*menuMaxLineas+i]+" "+opDefecto[pag*menuMaxLineas+i];
      if(opcionMenu==pag*menuMaxLineas+i+1){
        lcd.write(byte(0));
        if(linea.length()<=lcdColumns-1)
          lcd.print(linea);
        else{
         if(linea.length()>lcdColumns-1-k )k++;
         else k=0;
         lcd.print(linea.substring(k,lcdColumns-1+k));
         if(k+lcdColumns-1>linea.length()) lcd.print(" "+linea.substring(0,k+lcdColumns-1-linea.length()-1));
        }
      }
      else{
        lcd.print(" ");
        if(linea.length()<=lcdColumns-1)
          lcd.print(linea);
        else
          lcd.print(linea.substring(0,lcdColumns-1));
      }
      
      
      //display.startscrollleft(0x00, 0x0F);
    }
  }
  
    
}

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////*******dameValor ******///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

double dameValor(String cadena, double valor, double inc, double min, double max){
  int valEncoder,valEncoderAnt;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(cadena+String(":"));
  lcd.setCursor(0, 1);  
  lcd.print(valor);
  valEncoderAnt = rotaryEncoder.readEncoder();
  while (rotaryEncoder.currentButtonState() != BUT_RELEASED) {
    valEncoder = rotaryEncoder.readEncoder();
    if(valEncoder>valEncoderAnt){           
            valor=valor+inc;
            valor = constrain(valor, min, max);
            valEncoderAnt=valEncoder;
            lcd.setCursor(0, 0);  
            lcd.clear();  
            lcd.print(cadena+String(":"));
            lcd.setCursor(0, 1);  
            lcd.print(valor);
            Serial.println("Valor: "+String(valor));
    }else if(valEncoder<valEncoderAnt){
            valor=valor-inc;
            valor = constrain(valor, min, max);
            valEncoderAnt=valEncoder;
            lcd.setCursor(0, 0);    
            lcd.clear();
            lcd.print(cadena+String(":"));
            lcd.setCursor(0, 1);
            lcd.print(valor);
            Serial.println("Valor: "+String(valor));
    }
  }
  delay(100);
  Serial.println("Valor: "+String(valor));
  return valor;
}


void menuPrincipal(){
  String menu[]={"Volver", "Paro","Marcha","Ajustes"};
  String opDefecto[4];
  int index=0;
 
  do{ 
      if(sys.estado==MARCHA) {
        opDefecto[1]="";
        opDefecto[2]="*";
      }else{
        opDefecto[2]="";
        opDefecto[1]="*";
      }
     index=miMenu(menu,4, opDefecto,index);
     switch (index) {
        case 1:                     ////--Estado Parado--////
            sys.estado=PARO; 
            break;
        case 2:                     ////--Estado Marcha--////
            sys.estado=MARCHA;
            break;
        case 3:                     ////--Ajuste Parametros--////
            menuAjustes(); 
            break;

     }
     //Serial.println("index: "+String(index));
  }while(index==3);
  if(sys.estado==PARO){
    myPID.SetMode(MANUAL);
    //myEnc.write(0);
    myEnc.reset(); 
  }
  else myPID.SetMode(AUTOMATIC);
  if(sys.entrada==ESCALON || sys.estado==SENO) Setpoint=sys.setPoint;
  else Setpoint=0;
  escribeLcd(stringEstado[sys.estado],stringControl[sys.control]+": "+stringEntrada[sys.entrada]);
  

}

void menuAjustes(){
  String menu[]={"Volver", "Control","Entrada","Parametros"};
  int index=0;
  String opDefecto[4];
 
  do{ 
     opDefecto[1]=(sys.control==VELOCIDAD?"Velocidad":"Posicion");
     //opDefecto[2]=(sys.entrada==ESCALON?"Escalon":"Seno");
     opDefecto[2]=stringEntrada[sys.entrada];
     index=miMenu(menu,4,opDefecto, index);
     switch (index) {
        case 1:                     ////--Control Posicion o Velocidad--////
            sys.control=(sys.control==VELOCIDAD?POSICION:VELOCIDAD);
            
            break;
        case 2:                     ////--Entrada Escalon o Senoide--////
            //sys.entrada=(sys.entrada==ESCALON?SENO:ESCALON);
            sys.entrada=(sys.entrada==3?0:sys.entrada+1);
            
            break;
        case 3:                     ////--Ajuste Parametros--////
            menuParametros(); 
            break;

     }
     //Serial.println("index: "+String(index));
  }while(index!=0);

}

void menuParametros(){
  String menu[]={"Volver", "SetPoint","Periodo","Param Vel.","Param Pos.","Grabar Param.","Borrar Param."};
  int index=0;
  String opDefecto[7];
 
  do{ 
     opDefecto[1]=String(sys.setPoint);
     opDefecto[2]=String(sys.periodo);
     
     
     index=miMenu(menu,7,opDefecto, index);
     switch (index) {
        case 1:                     ////--SetPoint--////
            sys.setPoint=dameValor(menu[index], sys.setPoint,10, -10000, 10000);
            break;
        case 2:                     ////--Kp Zona muerta--////
            sys.periodo=dameValor(menu[index], sys.periodo,0.05, 0, 1000);
            
            break;
        case 3:                     ////--Parametros Velocidad--////
            menuKVel();
            
            break;
        case 4:                     ////--Parametrros Posicion--////
            menuKPos();
            break;
       
        case 5:
            settingsSaveToEEprom();
            escribeLcd("Grabando parametros","en Eeprom");
            delay(2000);
            break;
        case 6:
            settingsWipe();
            escribeLcd("Borrando parametros","de Eeprom");
            delay(2000);
            break;
            

     }
     //Serial.println("index: "+String(index));
  }while(index!=0);

}

void menuKVel(){
  String menu[]={"Volver", "KpZMVel","KdZMVel","KiZMVel","KpVel","KdVel","KiVel"};
  int index=0;
  String opDefecto[7];
 
  do{ 
     opDefecto[1]=String(sys.kPZMVel);
     opDefecto[2]=String(sys.kDZMVel);
     opDefecto[3]=String(sys.kIZMVel);
     opDefecto[4]=String(sys.kPVel);
     opDefecto[5]=String(sys.kDVel);
     opDefecto[6]=String(sys.kIVel);
     
     index=miMenu(menu,7,opDefecto, index);
     switch (index) {
        case 1:                     ////--Kp Zona muerta--////
            sys.kPZMVel=dameValor(menu[index], sys.kPZMVel,0.01, 0, 1000);
            
            break;
        case 2:                     ////--Kd Zona Muerta--////
            sys.kDZMVel=dameValor(menu[index], sys.kDZMVel,0.01, 0, 1000);
            
            break;
        case 3:                     ////--Ki Zona Muerta--////
            sys.kIZMVel=dameValor(menu[index], sys.kIZMVel,0.01, 0, 1000);
            break;
        case 4:                     ////--Kp--////
            sys.kPVel=dameValor(menu[index], sys.kPVel,0.01, 0, 1000);
            
            break;
        case 5:                     ////--Kd--////
            sys.kDVel=dameValor(menu[index], sys.kDVel,0.01, 0, 1000);
            
            break;
        case 6:                     ////--Ki--////
            sys.kIVel=dameValor(menu[index], sys.kIVel,0.01, 0, 1000);
            break;
             }
  }while(index!=0);

}


void menuKPos(){
  String menu[]={"Volver", "KpZMPos","KdZMPos","KiZMPos","KpPos","KdPos","KiPos"};
  int index=0;
  String opDefecto[7];
 
  do{ 
     opDefecto[1]=String(sys.kPZMPos);
     opDefecto[2]=String(sys.kDZMPos);
     opDefecto[3]=String(sys.kIZMPos);
     opDefecto[4]=String(sys.kPPos);
     opDefecto[5]=String(sys.kDPos);
     opDefecto[6]=String(sys.kIPos);
     
     index=miMenu(menu,7,opDefecto, index);
     switch (index) {
        case 1:                     ////--Kp Zona muerta--////
            sys.kPZMPos=dameValor(menu[index], sys.kPZMPos,0.01, 0, 1000);
            
            break;
        case 2:                     ////--Kd Zona Muerta--////
            sys.kDZMPos=dameValor(menu[index], sys.kDZMPos,0.01, 0, 1000);
            
            break;
        case 3:                     ////--Ki Zona Muerta--////
            sys.kIZMPos=dameValor(menu[index], sys.kIZMPos,0.01, 0, 1000);
            break;
        case 4:                     ////--Kp--////
            sys.kPPos=dameValor(menu[index], sys.kPPos,0.01, 0, 1000);
            
            break;
        case 5:                     ////--Kd--////
            sys.kDPos=dameValor(menu[index], sys.kDPos,0.01, 0, 1000);
            
            break;
        case 6:                     ////--Ki--////
            sys.kIPos=dameValor(menu[index], sys.kIPos,0.01, 0, 1000);
            break;
             }
  }while(index!=0);

}
