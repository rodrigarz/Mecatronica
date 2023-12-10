#include "Funciones.h"

struct struct_message myData;
Servo servo1;
String success;
esp_now_peer_info_t peerInfo;

ESP32Encoder myEnc;

int test_limits = 2; // De rotary Encoder
double Kp, Ki, Kd;
double Setpoint, Input, Output;
double velMotor;
int gradosActualMesa = 0;

int stepDelay = 0;
int posServo = 0;

PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);


void movimiento()
{
    switch (myData.indicacion)
    {
    case 1:
        posicionServo();
        myData.indicacion = 0;
        break;

    case 2:
        posicionExpulsor();
        myData.indicacion = 0;
        break;

    case 3:
        pasosPasoPaso();
        myData.indicacion = 0;
        break;

    case 4:
        posicionPasoPaso();
        myData.indicacion = 0;
        break;

    case 5:
        controlVelocidad();
        myData.indicacion = 0;
        break;

    case 6: 
        controlPosicion();
        myData.indicacion = 0;
        break;

    default:
        break;
    }
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

void posicionExpulsor()
{   
    if (myData.posExpulsor == 1)
    {
        //servo1.write(180);
        for (int i = 0; i <= 180; i++)
        {
            servo1.write(i);
            delay(30);
        }
    }
    else if (myData.posExpulsor == 0)
    {
        //servo1.write(0);
        for (int i = 180; i >= 0; i--)
        {
            servo1.write(i);
            delay(30);
        }
    }
    else if (myData.posExpulsor == 2)
    {
        //Para movimiento progresivo
        for (int i = 0; i <= 180; i++)
        {
            servo1.write(i);
            delay(30);
        }

        delay(1000);

        for (int i = 180; i >= 0; i--)
        {
            servo1.write(i);
            delay(30);
        }
    }
}

void pasosPasoPaso()
{
    if (gradosActualMesa + myData.gradosPaP > 300 || gradosActualMesa + myData.gradosPaP < -300)
    {
        Serial.println("No se puede mover la mesa mas distancia");
        return;
    }

    gradosActualMesa = gradosActualMesa + myData.gradosPaP;

    if (myData.gradosPaP > 0)
    {
        mueveMotor(myData.gradosPaP * steps / 360);
    }
    else
    {
        myData.gradosPaP = myData.gradosPaP * -1;
        mueveMotorB(myData.gradosPaP * steps / 360);
    } 
}

void posicionPasoPaso()
{
    if (myData.posPap == 1) {

        if (gradosActualMesa + (steps / 4)/8.89 > 300)
        {
            Serial.println("No se puede mover la mesa mas distancia");
            return;
        }
        mueveMotor(steps / 4);
        delay(1000);
        mueveMotorB(steps / 4);
    }
    else if (myData.posPap == 2) {
        
        if (gradosActualMesa + (steps / 2)/8.89 > 300)
        {
            Serial.println("No se puede mover la mesa mas distancia");
            return;
        }
        mueveMotor(steps / 2);
        delay(1000);
        mueveMotorB(steps / 2);
    }
    else if (myData.posPap == 3) {

        if (gradosActualMesa + (steps * (3 / 4))/8.89 > 300)
        {
            Serial.println("No se puede mover la mesa mas distancia");
            return;
        }
        mueveMotor(steps * 3 / 4);
        delay(1000);
        mueveMotorB(steps * 3 / 4);
    }
}


void mueveMotor(int stepsToMove) {
    digitalWrite(dirPin, HIGH); //Definimos sentido
    stepDelay = round((0.1125 * 1000000) / (2 * myData.velPap));///definidmos velocidad

    for (int x = 0; x < stepsToMove; x++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(stepDelay);
    }
}

void mueveMotorB(int stepsToMove) {
    digitalWrite(dirPin, LOW);
    stepDelay = round((0.1125 * 1000000) / (2 * myData.velPap));

    for (int x = 0; x < stepsToMove; x++) {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(stepDelay);
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

    ESP32Encoder::useInternalWeakPullResistors = UP;
    myEnc.attachHalfQuad(23, 19);
    myEnc.setCount(0);
    myEnc.clearCount();

    setupMovement();                // Incializacion Pines Motor
    myPID.SetSampleTime(100);

    myPID.SetOutputLimits(-255, 255);
    myPID.SetMode(AUTOMATIC);
    Setpoint = 0;
    gradosActualMesa = 0;
}


void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    //Serial.print("Bytes received: ");
    //Serial.println(len);
    Serial.print("Char: ");
    Serial.println(myData.posServo);
    Serial.println();
    if (myData.control == true)
    {
      movimiento();
    }
    else if (myData.control == false)
    {
      //plantaAutomatica();
    }
}

void controlVelocidad() {
    unsigned long int t, dt = 100; // intervalo para el calculo de velocidad
    float tiempo;
    int8_t inc;
    double Pos;

    static double Pos_ant = myEnc.getCount();

    //float periodo;
    static unsigned long int tinicio = millis();
    static unsigned long int t_ant = millis();  // Temporizador para calculo velocidad
    static unsigned long int t_ant2 = millis();// Temporizador para calculo Periodo

    // Calculo velocidad


    t = millis();
    if (t - t_ant > dt) {
        //Pos = myEnc.read();
        //Pos= myEnc.readEncoder();
        Pos = myEnc.getCount();
        Input = (double)1000 * (Pos - Pos_ant) / (t - t_ant);  // Calculo la velocidad
        t_ant = t;
        Pos_ant = Pos;
    }

    //Serial.println("Pos: "+String((int32_t)myEnc.getCount()))
    Setpoint = myData.velCinta;
    //if(Output<140 && Output>-140) {  // Las constantes son diferentes en la zona muerta
    if (Setpoint - Input > 100 || Setpoint - Input < -100) {
        Kp = myData.kPZMVel * 0.1; //0.1
        Ki = myData.kIZMVel * 0.1; //0.9
        Kd = myData.kDZMVel * 0.1; //0
    }
    else {
        Kp = myData.kPVel * 0.1;// 0.01
        Ki = myData.kIVel * 0.1; //0.2
        Kd = myData.kDVel * 0.1; //0.2
    }
    myPID.SetTunings(Kp, Ki, Kd);
    myPID.Compute();
    velMotor = Output; //Cambiar signo si el motor gira solo en un sentido
    Motor(velMotor);
}

void controlPosicion() {
    float tiempo;
    unsigned long int t, dt = 100; // intervalo para la visualiaci�n valores
    static unsigned long int tinicio = millis();  // Para calculo  funcion sinoidal
    static unsigned long int t_ant = millis();  // Temporizador para calculo velocidad
    static unsigned long int t_ant2 = millis();// Temporizador para calculo Periodo
    //static int dir=1;

	double incr = (myData.incrCinta / 0.3);
    t = millis();
    Setpoint = move(incr, 1000, 800, 0.1); // cada pulso se multiplica *10 para cambios m�s r�pidos

    //Input = myEnc.read(); // Lee encoder del motor
    //Input = myEnc.readEncoder();;
    Input = myEnc.getCount();
    if (Output<140 || Output>-140) {
        Kp = myData.kPZMPos * 0.1; //3
        Ki = myData.kIZMPos * 0.1; //0.05
        Kd = myData.kDZMPos * 0.1; //0.0
    }
    else {
        Kp = myData.kPPos * 0.1; //0.76
        Ki = myData.kIPos * 0.1; //0.05
        Kd = myData.kDPos * 0.1; //0.05
    }
    myPID.SetTunings(Kp, Ki, Kd);
    myPID.Compute();

    t_ant = t;

    velMotor = Output; //Cambiar signo si el motor gira solo en un sentido 
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
