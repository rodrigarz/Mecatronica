#include "Librerias.h"

    const int dirRA = 25;
    const int dirRB = 27;
    const int enR = 26;

void setupMovement(){
  pinMode(dirRA,   OUTPUT);
  pinMode(dirRB,   OUTPUT);
  pinMode(enR,     OUTPUT);   
}

void Motor(int speedR){
  //Serial.println(speedR);
  if(speedR>0){   //DELANTE
    digitalWrite(dirRA , HIGH );
    digitalWrite(dirRB ,  LOW  );
  }
  else{   //ATRAS
    digitalWrite(dirRA , LOW );
    digitalWrite(dirRB , HIGH );
  }
    analogWrite(enR, abs(speedR)); 
}
