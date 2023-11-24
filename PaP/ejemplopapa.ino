const int dirPin = 5;
const int stepPin = 15;
const int EN = 18; //8
const int steps = 200*16;
int stepDelay;
 
void setup() {
   // Marcar los pines como salida
   pinMode(dirPin, OUTPUT);
   pinMode(stepPin, OUTPUT);
   pinMode(EN, OUTPUT);
   //digitalWrite(EN, HIGH); //Anodo comun ENA+, DIR+ y PUL+ a VCC (5volt) y se maneja con ENA-, DI+- y PUL- conectados al Mc
    digitalWrite(EN, LOW); //Catodo comun ENA-, DIR- y PUL- a GND y se maneja con ENA+, DIR+ y PUL+ conectado al Mc
}
 
void loop() {
   //Activar una direccion y fijar la velocidad con stepDelay
   digitalWrite(dirPin, HIGH);
  
   stepDelay = 350; 
   // Giramos 200 pulsos para hacer una vuelta completa
   for (int x = 0;x< steps; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(stepDelay);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(stepDelay);
   }
   delay(1000);
 
// Cambiamos la direccion y disminuimos la velocidad
  digitalWrite(dirPin, LOW);
   stepDelay = 350;
   // Giramos 400 pulsos para hacer dos vueltas completas
   for (int x = 0; x < steps/4; x++) {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(stepDelay);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(stepDelay);
   }
   delay(1000);
}
