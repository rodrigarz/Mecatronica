#include "Librerias.h"
#include <Preferences.h>

Preferences preferences;

void settingsLoadFromEEprom() {

  settingsReset();

  preferences.begin("settings", false);
  preferences.getBytes("sysSettings", &data, sizeof(data));
  if (data.eepromValidData != 54) {
    Serial.println("Error de Eeprom. Cargando valores por defecto");
    //escribeLcd2("Error de Eeprom","Val. defecto", lcd1);
    delay(3000);
    settingsWipe();
  }
  preferences.end();
}

void settingsSaveToEEprom() {

  preferences.begin("settings", false);
  data.eepromValidData = 54;  //Un número definido cualquiera
  preferences.putBytes("sysSettings", &data, sizeof(data));
  preferences.end();
}

void settingsReset() {
  data.estado = MANUAL;
  data.control = VELOCIDAD;
  data.estadoManual = MANUAL_ENC;
  data.setPoint = 500;
  data.kPZMPos = 2;
  data.kDZMPos = 0;
  data.kIZMPos = 1;
  data.kPPos = 0.5;
  data.kDPos = 0.05;
  data.kIPos = 0.2;
  //data.eepromValidData=54; //Un número definido cualquiera
}

void settingsWipe() {
  settingsReset();
  preferences.begin("settings", false);
  preferences.clear();  // Limpia la eeprom
  preferences.end();
  //ESP.restart();
}
