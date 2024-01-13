#include "cinta.h"

ESP32Encoder myEnc;

//Variables necesarias
int test_limits = 2; // De rotary Encoder
double Kp, Ki, Kd;
double Input, Output;
double velMotor;
int salida;
double Setpoint = 0;

//Creamos PID
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

//Inicializamos la cinta
void inicializaCinta()
{
  ESP32Encoder::useInternalWeakPullResistors = UP;
  myEnc.attachHalfQuad(encoderA, encoderB);
  myEnc.setCount(0);
  myEnc.clearCount();

  setupMovement();                // Incializacion Pines Motor

  //Parametros iniciales del PID
  myPID.SetSampleTime(100);
  myPID.SetOutputLimits(-255, 255);
  myPID.SetMode(AUTOMATIC);

  Setpoint = 0;
  Setpoint = myData.setPoint;
}


//Funcion para hacer el control en velocidad
void controlVelocidad() {
    unsigned long int t, dt=50; // intervalo para el calculo de velocidad
    float tiempo;
    int8_t inc;
    double Pos;
    static double setpoin_ant;

    static double Pos_ant = myEnc.getCount();

    //float periodo;
    static unsigned long int tinicio = millis();
    static unsigned long int t_ant = millis();   // Temporizador para calculo velocidad
    static unsigned long int t_ant2 = millis();  // Temporizador para calculo Periodo

    // // Calculo velocidad


    t = millis();
    if (t - t_ant > dt) {
      Pos = myEnc.getCount();
      Input = (double)1000 * (Pos - Pos_ant) / (t - t_ant);  // Calculo la velocidad
      t_ant = t;
      Pos_ant = Pos;
      Serial.print("+SetPoint:" + String(myData.setPoint));
      Serial.print(", ");
      Serial.print("Input:" + String(Input));
      Serial.print(", ");
      Serial.print("Salida:" + String(salida));
      // escribeLcd1(stringEstado[myData.estado] + ": " + String(Input));
      Serial.print(", ");
      Serial.println("Error:" + String(myData.setPoint - Input));
    }

    //Si el setpoint ha cambiado, cambiamos el valor de salida
    if(setpoin_ant != myData.setPoint)
    {
      if (myData.setPoint > 0)
      {
        salida = map(myData.setPoint, 0, 1000, 140, 255);
      } else
      {
        salida = map(myData.setPoint, -1000, 0, -255, -140);
      }

      setpoin_ant = myData.setPoint;
    }

    //Cada periodo de muestreo, analizamos velocidad, si varia mas de 5 unidades, vamos cambiando la salida
    if(t - t_ant2 > dt)
    {
      if((myData.setPoint - Input) > 5)
      {
        salida = salida + 1;
      } else if( (myData.setPoint - Input) < -5)
      {
        salida = salida - 1;
      }
      t_ant2 = t;
    }
    salida = constrain(salida, -255, 255);
    
    //Cada periodo de muestreo -15 escribimos la señal en el motor
    if(t-tinicio > dt-15)
    {
      Motor(salida);
      tinicio = t;
    }
}

//Funcion para el control de posicion
void controlPosicion() {
  float tiempo;
  unsigned long int t, dt = 100;                // intervalo para la visualiación valores
  static unsigned long int tinicio = millis();  // Para calculo  funcion sinoidal
  static unsigned long int t_ant = millis();    // Temporizador para calculo velocidad
  static unsigned long int t_ant2 = millis();   // Temporizador para calculo Periodo

  //Fijamos valores para el PID, si esta en zona muerta o no
  t = millis();
  Input = myEnc.getCount();
  if (Output < 140 || Output > -140) {
    Kp = myData.kPZMPos * 0.1;  //3
    Ki = myData.kIZMPos;  //0.05
    Kd = myData.kDZMPos * 0.1;  //0.0
  } else {
    Kp = myData.kPPos;  //0.76
    Ki = myData.kIPos;  //0.05
    Kd = myData.kDPos;  //0.05
  }
  myPID.SetTunings(Kp, Ki, Kd);
  myPID.Compute();

  //Cada periodo marcado, escribimos los datos por el puerto serie
  if (t - t_ant > dt) {  
    Serial.print("+SetPoint:" + String(Setpoint));
    Serial.print(", ");
    Serial.print("Input:" + String(Input));
    Serial.print(", ");
    Serial.println("Output:" + String(Output));
    t_ant = t;
  }

  velMotor = Output;  
  Motor(velMotor);
}
  

//xd define la posicion final a alcanzar, y vmax la velocidad maxima que alcanzar�
double  move(double xd, double vmax, double a, double dt) {
    static unsigned long int t_ant = millis();

    static double v, x, x_ant = myEnc.getCount();
    unsigned long int t;
    int sentido_giro;

    x = myEnc.getCount();
    t = millis();
    if (t - t_ant > dt * 1000) {

        t_ant = t;
        sentido_giro = (xd - x > 0 ? 1 : -1);


        if (abs(xd - x) > 2 * vmax * vmax * dt / a) {
            //if(abs(xd-x) >500){
            if (abs(v) < vmax) {      // Acelero
                x = x + v * dt + 0.5 * a * dt * dt * sentido_giro;
                v = v + a * dt * sentido_giro;
                Serial.println("Acelero");
            }
            else {
                v = vmax * sentido_giro;
                x = x + v * dt;        // Vel constante
                Serial.println("Vel constante");
            }
        }
        else {                            // Decelero
            x = x + v * dt - 0.5 * a * dt * dt * sentido_giro;
            v = v - a * dt * sentido_giro;
            if ((xd - x) * sentido_giro < 0) {
                x = xd;    // Comprobamos no pasarnos  
                v = 0;
            }
            Serial.println("Decelero");
        }
        Serial.print("xd:" + String(xd));
        Serial.print(", ");
        Serial.print("x:" + String(x));
        Serial.print(", ");
        Serial.print("Input:" + String((int32_t)myEnc.getCount()));
        Serial.print(", ");
        Serial.println("v:" + String(v));

      }
      return x;
}
