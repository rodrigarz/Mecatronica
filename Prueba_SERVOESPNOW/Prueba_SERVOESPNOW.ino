#include "WiFi.h"
#include <esp_now.h>
#include "Servo.h"


int posServo = 0;
String success;
esp_now_peer_info_t peerInfo;

typedef struct struct_message {
  int posServo;
}struct_message;

struct struct_message myData;
struct struct_message datain;

static const int servoPin = 19;

Servo servo1;

 
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  //Serial.print("Bytes received: ");
  //Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.posServo);
  Serial.println();
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  servo1.attach(servoPin);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}
void loop() {
  // put your main code here, to run repeatedly:
  int servoPosition = map(posServo, 0, 180, 0, 180);
  servo1.write(servoPosition);
  Serial.println(servoPosition);
  delay(100);

}
