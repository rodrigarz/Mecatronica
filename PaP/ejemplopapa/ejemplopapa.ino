const int dirPin = 5;
const int stepPin = 15;
const int EN = 18;
const int steps = 200*16;  // Ajusta esto según la especificación de tu motor
int stepDelay;

void setup() {
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(EN, OUTPUT);
  digitalWrite(EN, LOW);  // Habilitar el motor al inicio
  Serial.begin(9600);     // Inicializar el puerto serie con una velocidad de baudios específica
}

void loop() {
  int command;
  // Solicitar un valor hasta que se reciba 1, 2 o 3
  while (true) {
    Serial.println("Ingrese 1 para 90 grados, 2 para 180 grados, 3 para 270 grados:");
    while (!Serial.available()) {
      // Esperar a que estén disponibles datos en el puerto serial
    }
    command = Serial.parseInt();
    Serial.read(); // Limpiar el buffer del serial después de leer el número
    if (command >= 1 && command <= 3) {
      break; // Salir del bucle si se recibió 1, 2 o 3
    } else {
      Serial.println("Valor no válido. Intente de nuevo.");
    }
  }

//Si entra 1 90º, si entra 2 180º, si entra 3 270º
  if (command == 1) {
    mueveMotor(steps/4);
    delay(1000);
    mueveMotorB(steps/4);
  } else if (command == 2) {
    mueveMotor(steps/2);
    delay(1000);
    mueveMotorB(steps/2);
  } else if (command == 3) {
    mueveMotor(steps*3/4);
    delay(1000);
    mueveMotorB(steps*3/4);
  }
}


void mueveMotor(int stepsToMove) {
  digitalWrite(dirPin, HIGH); //Definimos sentido
  stepDelay=50;///definidmos velocidad

  for (int x = 0; x < stepsToMove; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
}

void mueveMotorB(int stepsToMove) {
  digitalWrite(dirPin, LOW);
  stepDelay=50;

  for (int x = 0; x < stepsToMove; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
}

