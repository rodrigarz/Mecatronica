#include "Librerias.h"
#include <Preferences.h>

Preferences preferences;

void settingsLoadFromEEprom(){

  settingsReset();

  preferences.begin("settings", false);  
  preferences.getBytes("sysSettings",&sys,sizeof(sys));
  if (sys.eepromValidData!=54){
     Serial.println("Error de Eeprom. Cargando valores por defecto");
     escribeLcd("Error de Eeprom","Val. defecto");
     delay(3000);
     settingsWipe();
  }
   preferences.end();
}

void settingsSaveToEEprom(){

  preferences.begin("settings", false);  
  sys.eepromValidData=54; //Un número definido cualquiera
  preferences.putBytes("sysSettings",&sys,sizeof(sys));
  preferences.end();
  
}

void settingsReset() {
    sys.estado=PARO;
    sys.control=VELOCIDAD;
    sys.entrada=ESCALON;
    sys.setPoint=500;
    sys.periodo=15;
    sys.kPZMVel=0.2;
    sys.kDZMVel=0;
    sys.kIZMVel=0.9;
    sys.kPVel=0.2;
    sys.kDVel=0;
    sys.kIVel=0.15;
    sys.kPZMPos=2;
    sys.kDZMPos=0;
    sys.kIZMPos=0.15;
    sys.kPPos=0.3;
    sys.kDPos=0;
    sys.kIPos=0.05;
    //sys.eepromValidData=54; //Un número definido cualquiera
}

void settingsWipe(){
  settingsReset();
  preferences.begin("settings", false);
  preferences.clear();      // Limpia la eeprom 
  preferences.end();
  //ESP.restart();
}
