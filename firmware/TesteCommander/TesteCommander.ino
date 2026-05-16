#include <WiFi.h>
#include <WiFiUdp.h>
WiFiUDP udp;
unsigned char packetBuffer[255];
unsigned int localPort = 8089;
const char* ssid = "TP-Link_8D26";
const char* pass = "58426470";
//const char* ssid = "HUAWEI-1E4HF05A";
//const char* pass = "";
//const char* ssid = "TP-LINK_ECDB";
//const char* pass = "03367896";

int id = 0;
int pinMotorEsqA = 32;
int pinMotorEsqB = 33;
int pinMotorDirA = 25;
int pinMotorDirB = 26;
char connected = 0;
//int led1 = 33;
//int led2 = 32;
void motorValue(){
  unsigned char reverse = packetBuffer[1];
  unsigned int motorEsq =packetBuffer[id*2+2];
  unsigned int motorDir =packetBuffer[id*2+3];
  unsigned leftMask = 1<<(2*id+3);
  unsigned char rightMask = 1<<(2*id+2);
  unsigned char backLeft = reverse & leftMask;
  unsigned char backRight = reverse & rightMask;
  Serial.printf("%02X %02X %02X %02X %02X\n",reverse,backLeft, backRight, leftMask, rightMask);

  /*if(motorEsq <= 16 && motorEsq > 0){
    //digitalWrite(led2, HIGH);
     //delay(100);
     //digitalWrite(led2, LOW);
     //delay(100);    
    analogWrite(pinMotorEsqA, 180);
    analogWrite(pinMotorEsqB, 0);
  }
 

  else{
     //digitalWrite(led2, HIGH);
     //delay(100);
     //digitalWrite(led2, LOW);
     //delay(100);    
    analogWrite(pinMotorEsqA, 0);
    analogWrite(pinMotorEsqB, motorEsq);
  }
  
  if(motorDir <= 16 && motorDir > 0){
     //digitalWrite(led1, HIGH);
     //delay(100);
     //digitalWrite(led1, LOW);
     //delay(100);   
    analogWrite(pinMotorDirA, 180);
    analogWrite(pinMotorDirB, 0);
  }
  else{
     //digitalWrite(led1, HIGH);
     //delay(100);
     //digitalWrite(led1, LOW);
     //delay(100);   
    analogWrite(pinMotorDirA, 0);
    analogWrite(pinMotorDirB, motorDir);  
  }
  */
  if (backLeft != 0){
    printf("inverte Esq");
    analogWrite(pinMotorEsqA,  motorEsq);
    analogWrite(pinMotorEsqB, 0);
  }
  else{
    analogWrite(pinMotorEsqA, 0);
    analogWrite(pinMotorEsqB, motorEsq);
  }

  if(backRight != 0){
    printf("inverte Dir \n");
    analogWrite(pinMotorDirA, motorDir);
    analogWrite(pinMotorDirB, 0);
  }
  else{
    analogWrite(pinMotorDirA, 0);
    analogWrite(pinMotorDirB, motorDir);
  }
}

/*void task(void *pvParameter){
  WiFi.begin(ssid, pass);
  while(1){
    //WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED){
      connected = 0;
      delay(1); //Serial.print(F("."));
     
      analogWrite(pinMotorDirA,100);
      analogWrite(pinMotorDirB,0);
      analogWrite(pinMotorEsqB,0);
      analogWrite(pinMotorEsqA,0);
    }
    if(!connected){
      udp.begin(localPort);
      connected = 1; 
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}*/

void setup() {
   //pinMode(led1, OUTPUT);
   //pinMode(led2, OUTPUT);
  Serial.begin(115200);
  // Connect to Wifi network.
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED){
    delay(500); Serial.print(F("."));
  }
  udp.begin(localPort);
  Serial.printf("UDP server : %s:%i \n", WiFi.localIP().toString(), localPort);
  //xTaskCreatePinnedToCore(task, "task1", 2048, NULL, 1 , NULL, 1);
}


void loop() {
 // if(connected){
    int packetSize = udp.parsePacket();
    if (packetSize) {
      Serial.print(" Received packet from : "); Serial.println(udp.remoteIP());
      Serial.print(" Size : "); Serial.println(packetSize);
      int len = udp.read(packetBuffer, 255);
      if (len > 0) packetBuffer[len - 1] = 0;
      //Serial.printf("Data : %x\n", packetBuffer);
      for(int i = 0; i < 9; i++)
        Serial.printf("%02X ", packetBuffer[i]);
        //digitalWrite(led1, HIGH);
        //digitalWrite(led2, HIGH);
        //delay(1000);
        //digitalWrite(led1, LOW);
        //digitalWrite(led2, LOW);
        //delay(1000);    
        Serial.println();
      //udp.beginPacket(udp.remoteIP(), udp.remotePort());
      //udp.printf("UDP packet was received OK\r\n");
      //udp.endPacket();
        motorValue();
    }
    //Serial.println("\n");
    //delay(100);
    //Serial.print("[Server Connected] ");
    //Serial.println (WiFi.localIP());
    //motorValue()
  //}
  
}
