#include "Librerias.h"

    const int dirRA = 25;
    const int dirRB = 27;
    const int enR = 5;

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
    analogWrite(enR, abs(speedR));
  }
  else if(speedR<0) {//ATRAS
    digitalWrite(dirRA , LOW );
    digitalWrite(dirRB , HIGH );
    analogWrite(enR, abs(speedR));
  }
  else{
    digitalWrite(dirRA , LOW );
    digitalWrite(dirRB , LOW );
    analogWrite(enR, 0);
  } 
}
