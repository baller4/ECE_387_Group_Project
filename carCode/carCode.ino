#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "MotorDriver.h"
#define CE_PIN 9
#define CSN_PIN 10

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};
const byte masterAddress[5] = {'T','X','a','a','a'};

RF24 radio(CE_PIN, CSN_PIN);
MotorDriver motor;

char dataReceived[1]; // this must match dataToSend in the TX
char dataToSend[4] = "BBB";


void setup() {
  
  Serial.begin(38400);
  
  Serial.println("SimpleRx Starting");
  motor.begin();
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_MIN);
    
  radio.openWritingPipe(masterAddress);
  radio.openReadingPipe(1, thisSlaveAddress);
  radio.startListening();
}

void loop() {
  readData();
  delay(500);
  readData();
  delay(500);
  sendD();
}

void readData() {
  if(radio.available()) {
    
    //read the data and store it in dataReceived
    radio.read(&dataReceived, sizeof(dataReceived));
    if(dataReceived[0] == 65){
          motor.speed(0, 100);
          motor.speed(1, 100);
      }else{
        motor.brake(0);
        motor.brake(1);
      }
      //show the read data
    showData();
  }
}

void showData() {
  Serial.print("Data received ");
  Serial.print(dataReceived[0]);
  //Serial.print(dataReceived[1]);
  //Serial.print(dataReceived[2]);
  Serial.println();
  //delay(500);
}

void sendD() {
  
  radio.stopListening();
  bool rslt;
  rslt = radio.write(&dataToSend, sizeof(dataToSend));
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2
  radio.startListening();
  Serial.print("Data Sent ");
  Serial.print(dataToSend);
  if (rslt) {
    Serial.println("  Acknowledge received");
  }
  else {
    Serial.println("  Tx failed");
  }
}
