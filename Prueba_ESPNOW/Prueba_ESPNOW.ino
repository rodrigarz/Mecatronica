#include "WiFi.h"
#include <esp_now.h>
#include "AiEsp32RotaryEncoder.h"

#define ROTARY_ENCODER_DT 35
#define ROTARY_ENCODER_CLK 36
#define ROTARY_ENCODER_BUTTON_PIN 32
#define ROTARY_ENCODER_VCC_PIN -1

uint8_t placaServo[] = {0xB4, 0xE6, 0x2D, 0xFB, 0x1B, 0xBD};

int posServo = 0;
String success;
esp_now_peer_info_t peerInfo;

typedef struct struct_message {
    int posServo;
} struct_message;

struct struct_message data;

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_DT, ROTARY_ENCODER_CLK, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN);


// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


int dameValor(int valor, int inc, int min, int max)
{
	int valEncoder, valEncoderAnt;
	static long int tiempo = millis();

	valEncoderAnt = rotaryEncoder.readEncoder();
	while (rotaryEncoder.currentButtonState() != BUT_RELEASED)
	{
		valEncoder = rotaryEncoder.readEncoder();;
		if (valEncoder > valEncoderAnt)
		{
			valor = valor + inc;
		}
		else if (valEncoder < valEncoderAnt)
		{
			valor = valor - inc;
		}
		valor = constrain(valor, min, max);
		valEncoderAnt = valEncoder;
    Serial.println(valor);
    delay(500);
	}
	
	return valor;
}


bool botonEncoderPulsado()
{
	if (rotaryEncoder.currentButtonState() == BUT_RELEASED)
		return false;
	else
		return true;
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Ok");
  rotaryEncoder.begin();
	rotaryEncoder.setup([] {rotaryEncoder.readEncoder_ISR(); });
	rotaryEncoder.setBoundaries(-10000, 10000, true);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Init ESP-NOW
 if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, placaServo, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Entro a dame valor");
  data.posServo = dameValor(data.posServo, 1, 0, 180);
  Serial.println(data.posServo);
  delay(250);
  esp_err_t result = esp_now_send(placaServo, (uint8_t *) &data, sizeof(data));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
   Serial.println("Error sending the data");
  }
  delay(150);
  
  

}
