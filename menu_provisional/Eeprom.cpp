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
  data.periodo = 15;
  data.kPZMVel = 0.2;
  data.kDZMVel = 0;
  data.kIZMVel = 0.9;
  data.kPVel = 0.2;
  data.kDVel = 0;
  data.kIVel = 0.15;
  data.kPZMPos = 2;
  data.kDZMPos = 0;
  data.kIZMPos = 0.15;
  data.kPPos = 0.3;
  data.kDPos = 0;
  data.kIPos = 0.05;
  //data.eepromValidData=54; //Un número definido cualquiera
}

void settingsWipe() {
  settingsReset();
  preferences.begin("settings", false);
  preferences.clear();  // Limpia la eeprom
  preferences.end();
  //ESP.restart();
}
