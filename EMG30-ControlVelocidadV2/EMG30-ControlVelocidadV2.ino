//Genera una señal senoidal (-360,360) de periodo definido por el usuario que sirve
//de Setpoint para el controlador PID. El encoder del motor es el Imput del PID
//Para ajustar el rango del PWM "PID_v1->PID::SetOutputLimits" al rededor de 160 se empieza a mover

#include "Librerias.h"



/* Conexiones
LCD       ESP32
Vcc       3.3 v
GND       GND
SDA        GPIO21
SCL       GPIO22
*/
/*
// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);  
*/
/*
connecting Rotary encoder. Se definen en Menu.h
CLK (A pin) - to any microcontroler intput pin with interrupt -> in this example pin 35
DT (B pin) - to any microcontroler intput pin with interrupt -> in this example pin 36
SW (button pin) - to any microcontroler intput pin -> in this example pin 32
VCC - to microcontroler VCC (then set ROTARY_ENCODER_VCC_PIN -1) or in this example pin 25
GND - to microcontroler GND
Conexions definidas en Menu.h

Movimiento de motor Pines definidos en Movement.cpp

*/




system_t sys;   // Estructura definida en Globales.h que contiene los variables del programa que se graba en la Eeprom 

String stringEstado[]={"Manual","Automatico", "PuertoSerial"};
String stringControl[]={"VEL","POS"};
String stringEntrada[]={"ESCALON","SENO","REFERENCIA","RAMPA"};




// Motor

int encoderA = 23; //MORADO
int encoderB = 19; //AZUL
                   //VERDE-GND
                   //MARRON-3.3V
const int fCarrera =4;


//AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN);
//Encoder myEnc(encoderA, encoderB);

ESP32Encoder myEnc;

//AiEsp32RotaryEncoder myEnc = AiEsp32RotaryEncoder(encoderA, encoderB, -1, -1);

int test_limits = 2; // De rotary Encoder
double Kp, Ki, Kd;
double Setpoint, Input, Output; 
double velMotor;



//float tiempo;
//unsigned long int t_ant, t_ant2,t;

int boton_pulsado=0;
//int estado=PARO;


PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

int buscaFC();
void controlVelocidad();
void controlPosicion();
double  move(double xd, double vmax, double a, double dt);

void setup() {
  Serial.begin(115200);
 
  inicializaLcd();                  // Incialización del display
  settingsLoadFromEEprom();         // Carga valores de Eeprom
  incializaRotaryEncoder();         //Incialización encoder rotativo HW-040 

  ESP32Encoder::useInternalWeakPullResistors=UP;
  myEnc.attachHalfQuad(23, 19);
  myEnc.setCount(0);
  myEnc.clearCount();

  
  pinMode(fCarrera, INPUT_PULLUP); // Final de carrera
  setupMovement();                // Incializacion Pines Motor
  //myPID.SetMode(MANUAL);
  myPID.SetSampleTime(100);
  
  //myPID.SetTunings(Kp, Ki, Kd);
  //buscaFC();

  myPID.SetOutputLimits(-255, 255);
  if(sys.estado==Manual) myPID.SetMode(MANUAL);
  else myPID.SetMode(AUTOMATIC);
  
  //t_ant=millis();
  //Pos_ant = myEnc.read();
  escribeLcd(stringEstado[sys.estado],stringControl[sys.control]+": "+stringEntrada[sys.entrada]);
  if(sys.entrada==REFERENCIA){
      Setpoint=0;
       
       myEnc.clearCount();
  }
  else{
    Setpoint=sys.setPoint;
     
      myEnc.clearCount();
  }

}



void loop() {
  

  if(botonEncoderPulsado()) menuPrincipal();

  //Serial.println("Estado: "+String(sys.estado));
  //Serial.println("Control: "+String(sys.control));

  if(sys.estado==Manual){
    if(sys.control==VELOCIDAD)  controlVelocidad();
    else if (sys.control==POSICION) controlPosicion();
  }
  else if(sys.estado==Manual){
    myPID.SetMode(MANUAL);
    Motor(0);
    //Serial.println("Estado: "+String(sys.estado));
    
  }
  //Serial.println("Pos: "+String(myEnc.readEncoder()));
  //Serial.println("Pos: "+String((int32_t)myEnc.getCount()));
  
}


void controlVelocidad(){
  unsigned long int t, dt=100; // intervalo para el calculo de velocidad
  float tiempo;
  int8_t inc;
  double Pos; 
  
  static double Pos_ant=myEnc.getCount();
  
  //float periodo;
  static unsigned long int tinicio=millis();
  static unsigned long int t_ant=millis();  // Temporizador para calculo velocidad
  static unsigned long int t_ant2=millis();// Temporizador para calculo Periodo
  
// Calculo velocidad

  
  t=millis();
  if(t-t_ant> dt){
    //Pos = myEnc.read();
    //Pos= myEnc.readEncoder();
    Pos= myEnc.getCount();
    Input=(double)1000*(Pos-Pos_ant)/(t-t_ant);  // Calculo la velocidad
    t_ant=t;
    Pos_ant=Pos;
    Serial.print("+SetPoint:"+String(Setpoint));
    Serial.print(", ");
    if(sys.entrada==ESCALON){
      Serial.print("SetPoint:"+String(-Setpoint));
      Serial.print(", "); 
    }
    Serial.print("Input:"+String(Input));
    Serial.print(", ");
    Serial.print("Output:"+String(Output));
    escribeLcd1(stringEstado[sys.estado]+": "+String(Input));
    Serial.print(", ");
    Serial.println("Error:"+String(Setpoint - Input));
    
  }

  //Serial.println("Pos: "+String((int32_t)myEnc.getCount()));


  if(sys.entrada==ESCALON){
     inc=deltaEncoder();
     if(inc!=0) Setpoint=(Setpoint>0?Setpoint+inc:Setpoint-inc); // Puedo cambiara la referencia con el rotary encoder
     if(t-t_ant2>sys.periodo*1000){        // Cambio sentido
        Setpoint=-Setpoint;
        t_ant2=t; 
     }
  }else if(sys.entrada==REFERENCIA){
       inc=deltaEncoder();
       if(inc!=0) Setpoint+=inc;
  }else if(sys.entrada==SENO){
        tiempo=(float)(millis()-tinicio);
        Setpoint = sys.setPoint*sin(2*PI*tiempo/(2*sys.periodo*1000)+PI);
        //Serial.println("SetPoint: "+String(Setpoint)+"--"+String(2*PI*tiempo/(2*sys.periodo*1000)+PI));
    
  }
  //if(Output<140 && Output>-140) {  // Las constantes son diferentes en la zona muerta
  if(Setpoint - Input >100 || Setpoint - Input <-100) {
      Kp=sys.kPZMVel*0.1; //0.1
      Ki=sys.kIZMVel*0.1; //0.9
      Kd=sys.kDZMVel*0.1; //0
  }
  else {
      Kp=sys.kPVel*0.1;// 0.01
      Ki=sys.kIVel*0.1; //0.2
      Kd=sys.kDVel*0.1; //0.2
  }
  myPID.SetTunings(Kp,Ki,Kd);
  myPID.Compute();
  velMotor=Output; //Cambiar signo si el motor gira solo en un sentido
  Motor(velMotor); 
}


void controlPosicion(){
  float tiempo;
  unsigned long int t, dt=100; // intervalo para la visualiación valores
  static unsigned long int tinicio=millis();  // Para calculo  funcion sinoidal
  static unsigned long int t_ant=millis();  // Temporizador para calculo velocidad
  static unsigned long int t_ant2=millis();// Temporizador para calculo Periodo
  //static int dir=1;

  t=millis();
  if(sys.entrada==ESCALON){
    int8_t inc=deltaEncoder();
    if(inc!=0) Setpoint=(Setpoint>0?Setpoint+inc:Setpoint-inc); // Puedo cambiara la referencia con el rotary encoder

    if(t-t_ant2>sys.periodo*1000){        // Cambio sentido
        Setpoint=-Setpoint;
        t_ant2=t; 
    }
    
  }else if(sys.entrada==REFERENCIA){
      int8_t inc=deltaEncoder();
      if(inc!=0) Setpoint+=inc*10; // cada pulso se multiplica *10 para cambios más rápidos
  }else if(sys.entrada==SENO){
      tiempo=(float)(millis()-tinicio);
      Setpoint = sys.setPoint*sin(2*PI*tiempo/(2*sys.periodo*1000)+PI)-200;
      
  }else if(sys.entrada==RAMPA){
      if((abs(Setpoint-sys.setPoint))==0){ // Si se alcanza el destino cambia la posicion
            sys.setPoint=-sys.setPoint;
      }
           
      Setpoint=move(sys.setPoint, 1000, 800, 0.1); //double xd, double vmax, double a, double dt);
      
    

  }
  //Input = myEnc.read(); // Lee encoder del motor
  //Input = myEnc.readEncoder();;
  Input = myEnc.getCount();
  if(Output<140 || Output>-140){
      Kp=sys.kPZMPos*0.1; //3
      Ki=sys.kIZMPos*0.1; //0.05
      Kd=sys.kDZMPos*0.1; //0.0
  }
  else{
    Kp=sys.kPPos*0.1; //0.76
    Ki=sys.kIPos*0.1; //0.05
    Kd=sys.kDPos*0.1; //0.05
  }
  myPID.SetTunings(Kp,Ki,Kd);
  myPID.Compute();

  
  if(t-t_ant> dt && sys.entrada!=RAMPA){                                      // Imprime valores cada periodo de muestreo
    Serial.print("+SetPoint:"+String(Setpoint));
    Serial.print(", ");
    if(sys.entrada==ESCALON){
      Serial.print("SetPoint:"+String(-Setpoint));
      Serial.print(", "); 
    }
    Serial.print("Input:"+String(Input));
    Serial.print(", ");
    Serial.println("Output:"+String(Output));
    escribeLcd1(stringEstado[sys.estado]+": "+String(Input));
    t_ant=t;
  }

  velMotor=Output; //Cambiar signo si el motor gira solo en un sentido 
  Motor(velMotor);
}





double  move(double xd, double vmax, double a, double dt){
  static unsigned long int t_ant=millis();
 
  static double v,x,x_ant=myEnc.getCount();
  unsigned long int t;
  int sentido_giro;
  
  
  
  //x=myEnc.read();
  //x=myEnc.readEncoder();
  x=myEnc.getCount();
  t=millis();
  if(t-t_ant>dt*1000){
      
      t_ant=t;
      sentido_giro=(xd-x>0?1:-1);
  
  
    if(abs(xd-x) >2*vmax*vmax*dt/a){
    //if(abs(xd-x) >500){
      if(abs(v) < vmax){      // Acelero
          x=x+v*dt+0.5*a*dt*dt*sentido_giro;
          v=v+a*dt*sentido_giro;
          Serial.println("Acelero");
      }
      else{
          v=vmax*sentido_giro;
          x=x+v*dt;        // Vel constante
          Serial.println("Vel constante");
      }
   }
   else{                            // Decelero
       x=x+v*dt-0.5*a*dt*dt*sentido_giro;
        v=v-a*dt*sentido_giro;
       if((xd-x)*sentido_giro<0){
          x=xd;    // Comprobamos no pasarnos  
          v=0;
       }
       Serial.println("Decelero");
   }  
    Serial.print("xd:"+String(xd));
    Serial.print(", ");
    Serial.print("x:"+String(x));
    Serial.print(", ");
    Serial.print("Input:"+String((int32_t)myEnc.getCount()));
    Serial.print(", ");
    Serial.println("v:"+String(v));
   
  }
 return x;
  
  
}

  
