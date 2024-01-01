#include "Funciones.h"

struct struct_message myData = {};
Servo servo1;
String success;
esp_now_peer_info_t peerInfo;

ESP32Encoder myEnc;

int test_limits = 2; // De rotary Encoder
double Kp, Ki, Kd;
double Setpoint, Input, Output;
double velMotor;


int posServo = 0;
int salida;
int movMotorDC = 0;
extern bool entraMovimiento = false;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


void movimiento()
{
    switch (myData.indicacion)
    {
    case 1:
        movMotorDC = 0;
        posicionServo();
        myData.indicacion = 0;
        break;

    case 2:
        movMotorDC = 0;
        posicionExpulsor(myData.posExpulsor);
        myData.indicacion = 0;
        break;

    case 3:
        movMotorDC = 0;
        pasosPasoPaso();
        myData.indicacion = 0;
        break;

    case 4:
        movMotorDC = 0;
        posicionPasoPaso(myData.posPap, false);
        myData.indicacion = 0;
        break;

    case 5:
        movMotorDC = 2;
        do{
          controlVelocidad();
          vTaskDelay(1);
        }while(myData.indicacion == 5);
        myData.indicacion = 0;
        break;

    case 6: 
        movMotorDC = 1;
        do{
          controlPosicion();
          vTaskDelay(1);
        }while(myData.indicacion == 6);
        myData.indicacion = 0;
        break;

    default:
        movMotorDC = 0;
        Motor(0);
        break;
    }
    delay(50);
}

void posicionServo()
{
  Serial.println("Entro");
    int servoPosAnt = 0;
    int servoPosition = map(myData.posServo, 0, 180, 0, 180);
    servoPosAnt = servoPosition;
    servo1.write(myData.posServo);
    if (servoPosAnt != servoPosition)
    {
        Serial.println(servoPosition);
    }
    delay(200);
}

void posicionExpulsor(int modoActiva)
{   
    if (modoActiva == 1)
    {
        servo1.write(0);
        delay(50);
        for (int i = 0; i <= 180; i++)
        {
            servo1.write(i);
            delay(50);
        }
    }
    else if (modoActiva == 0)
    {
        servo1.write(180);
        delay(50);
        for (int i = 180; i >= 0; i--)
        {
            servo1.write(i);
            delay(50);
        }
    }
    else if (modoActiva == 2)
    {
        //Para movimiento progresivo
        servo1.write(0);
        delay(50);
        for (int i = 0; i <= 180; i++)
        {
            servo1.write(i);
            delay(50);
        }

        delay(1000);

        for (int i = 180; i >= 0; i--)
        {
            servo1.write(i);
            delay(100);
        }
    }
}




void inicializa()
{
    Serial.begin(115200);
    Serial.println(WiFi.macAddress());
    WiFi.mode(WIFI_STA);

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    else
    {
        Serial.print("Todo ok");
    }

    esp_now_register_recv_cb(OnDataRecv);
    pinMode(dirPin, OUTPUT);
    pinMode(stepPin, OUTPUT);
    pinMode(EN, OUTPUT);
    digitalWrite(EN, LOW);
    servo1.attach(servoPin);

    pinMode(pinFinalCarrera, INPUT_PULLDOWN);

    ESP32Encoder::useInternalWeakPullResistors = UP;
    myEnc.attachHalfQuad(encoderA, encoderB);
    myEnc.setCount(0);
    myEnc.clearCount();

    pinMode(sensorIR, INPUT);

    setupMovement();                // Incializacion Pines Motor
    myPID.SetSampleTime(100);

    myPID.SetOutputLimits(-255, 255);
    myPID.SetMode(AUTOMATIC);
    Setpoint = 0;
    gradosActualMesa = 0;
    Setpoint = myData.setPoint;
    //mueveMotorB(5);
    myData.velPap = 250;
    buscaInicio();
}


void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    //Serial.print("Bytes received: ");
    //Serial.println(len);
    Serial.print("Char: ");
    Serial.println(myData.posServo);
    Serial.println();
    Setpoint = myData.setPoint;
    if (myData.control == true)
    {
      entraMovimiento = true;
    }
    else if (myData.control == false)
    {
      plantaAutomatica();
    }
}

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
    
    if(t-tinicio > dt-15)
    {
      Motor(salida);
      tinicio = t;
    }
}

void controlPosicion() {
  float tiempo;
  unsigned long int t, dt = 100;                // intervalo para la visualiación valores
  static unsigned long int tinicio = millis();  // Para calculo  funcion sinoidal
  static unsigned long int t_ant = millis();    // Temporizador para calculo velocidad
  static unsigned long int t_ant2 = millis();   // Temporizador para calculo Periodo
  //static int dir=1;

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


  if (t - t_ant > dt) {  // Imprime valores cada periodo de muestreo
    Serial.print("+SetPoint:" + String(Setpoint));
    Serial.print(", ");
    Serial.print("Input:" + String(Input));
    Serial.print(", ");
    Serial.println("Output:" + String(Output));
    t_ant = t;
  }

  velMotor = Output;  //Cambiar signo si el motor gira solo en un sentido
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


