#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "MotorDriver.h"
#include <Ultrasonic.h>

Ultrasonic Ultrasonic(A4, A5);
#define CE_PIN 9
#define CSN_PIN 10

const byte thisSlaveAddress[5] = {'R','x','A','A','A'};
const byte masterAddress[5] = {'T','X','a','a','a'};

RF24 radio(CE_PIN, CSN_PIN);
MotorDriver motor;

char dataReceived[2]; // this must match dataToSend in the TX
//char dataToSend[4] = "BBB";


void setup() {
  //int a = -1;
  Serial.begin(250000);
  //Serial.println(a,HEX);
  
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
  delay(100);
  sendD();
  delay(100);
  
    
}

void readData() {
  Serial.println("read");
//  while
//  startRadio();
  if(radio.available()) {
    
    //read the data and store it in dataReceived
    radio.read(&dataReceived, sizeof(dataReceived));
    move(dataReceived);
      //show the read data
    showData();
  }else{
    Serial.println("nothing recieved");
  }
  
}

void move(char * data) {
  Serial.println("move");
  int speed0,speed1;
  switch(data[0]){
    case 0:
    speed0=0;
    speed1=0;
    break;        
    case 1:
    speed0=80;
    speed1=80;
    break;
    case 2:
    speed0=90;
    speed1=90;
    break;
    case 3:
    speed0=100;
    speed1=100;
    break;
    default:
    Serial.println("error");
    speed0=0;
    speed1=0;
  }
  switch(data[1]){
    case -3:
    if(speed1!=0){
      speed1-=45;
    }
    break;
    case -2:
    if(speed1!=0){
      speed1-=30;//=map(speed1,0,100,0,50);
    }
    break;
    case -1:
    if(speed1!=0){
      speed1-=15;//map(speed1,0,100,0,75);
    }
    break;
    case 0:
    speed0 = speed0;
    speed1 = speed1;
    break;      
    case 1:
    if(speed0!=0){
      speed0-=15;//map(speed0,0,100,0,75);
    }
    break;
    case 2:
    if(speed0!=0){
      speed0-=30;//map(speed0,0,100,0,50);
    }
    break;
    case 3:
    if(speed0!=0){
    speed0-=45;//map(speed0,0,100,0,25);
    }
    break;
    default:
    Serial.println("error");
    speed0=0;
    speed1=0;
  }
  digitalWrite(MOTORSHIELD_IN4, LOW);
  Serial.println(speed0);
  Serial.println(speed1);
  if(speed0==0){
    motor.brake(0);
  }else{
    motor.speed(0, speed0);
  }
  if(speed1==0){
    motor.brake(1);
  }else{
    motor.speed(1, speed1);
  }
}

void showData() {
  Serial.print("Data received ");
  Serial.print(dataReceived[0],DEC);
  Serial.print(dataReceived[1],DEC);
  //Serial.print(dataReceived[1]);
  //Serial.print(dataReceived[2]);
  Serial.println(); 
  //delay(500);
}

void sendD() {
  Serial.println("send");
//  startRadio();
  char dataToSend[1];
  if(Ultrasonic.distanceRead()<50)
  {
    dataToSend[0]=1;
  }else{
    dataToSend[0]=0;
  }
  
  radio.stopListening();
  
  bool rslt;
  rslt = radio.write(&dataToSend, sizeof(dataToSend));
  Serial.println("send1");
        // Always use sizeof() as it gives the size as the number of bytes.
        // For example if dataToSend was an int sizeof() would correctly return 2
  radio.startListening();
  Serial.println("send2");
  Serial.print("Data Sent ");
  Serial.print(dataToSend[0],DEC);
  if (rslt) {
    Serial.println("  Acknowledge received");
  }
  else {
    delay(15);
    Serial.println("  Tx failed");
  }
}
