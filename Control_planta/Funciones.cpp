#include "Funciones.h"

struct struct_message myData = {};
//esp_now_peer_info_t peerInfo;

bool entraMovimiento = false;
bool entraAutomatico = false;


//En función de la instrucción hacemos un movimiento u otro
void movimiento()
{
    switch (myData.indicacion)
    {
    case 1:
        posicionServo();
        myData.indicacion = 0;
        break;

    case 2:
        posicionExpulsor(myData.posExpulsor);
        myData.indicacion = 0;
        break;

    case 3:
        pasosPasoPaso();
        myData.indicacion = 0;
        break;

    case 4:
        posicionPasoPaso(myData.posPap, false);
        myData.indicacion = 0;
        break;

    case 5:
        do{
          controlVelocidad();
          vTaskDelay(1); //Para evitar que se reinicie el esp
        }while(myData.indicacion == 5);
        myData.indicacion = 0;
        break;

    case 6: 
        do{
          controlPosicion();
          vTaskDelay(1);
        }while(myData.indicacion == 6);
        myData.indicacion = 0;
        break;

    default:
        Motor(0);
        break;
    }
    delay(50);
}


//Inicializamos todo lo necesario para funcionar
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
    
    inicializaMesa();
    inicializaServo();
    inicializaCinta();
    inicializaColores();

    pinMode(pinFinalCarrera, INPUT_PULLDOWN);
    pinMode(sensorIR, INPUT);
    pinMode(ledColores, OUTPUT);

    myData.velPap = 250;
    buscaInicio();
}

//Callback cuando recibimos comunicacion por ESP-NOW
void OnDataRecv(const uint8_t* mac, const uint8_t* incomingData, int len) {
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.println();
    Setpoint = myData.setPoint;
    if (myData.control == true)
    {
      entraMovimiento = true;
      entraAutomatico = false;
    }
    else if (myData.control == false)
    {
      entraAutomatico = true;
      entraMovimiento = false;
      myData.indicacion = 0;
    }
}

