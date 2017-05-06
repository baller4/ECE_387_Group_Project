#include <SPI.h>
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

void setup() {
  
  Serial.begin(250000);
  
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
  delay(50);
  sendD();
  delay(50); 
}

//read the data from the glove
void readData() {
  if(radio.available()) {
    
    //read the data and store it in dataReceived
    radio.read(&dataReceived, sizeof(dataReceived));
    
    //move the car based off of the data received
    move(dataReceived);
    
    //show the read data
    showData();
  }
  
}

//moves the car given data from the glove
void move(char * data) {
  Serial.println("move");
  int speed0,speed1; //speed1 represents the left motor and vise versa
  switch(data[0]){ // this switch statement determin if the car is moving forward, backwards, or not at all
    case 0:
    speed0=0;
    speed1=0;
    break;        
    case 1:
    speed0=100;
    speed1=100;
    break;
    case -1:
    speed0=-100;
    speed1=-100;
    break;
    default:
    Serial.println("error");
    speed0=0;
    speed1=0;
  }
  switch(data[1]){// this switch statement modifies the speed of one of the motors if the car need to turn
    case -3:
    if(speed1>0){
      speed1-=45;
    }else if(speed1<0){
      speed1+=45;
    }
    break;
    case -2:
    if(speed1>0){
      speed1-=30;
    }else if(speed1<0){
      speed1+=30;
    }
    break;
    case -1:
    if(speed1>0){
      speed1-=15;
    }else if(speed1<0){
      speed1+=15;
    }
    break;
    case 0:
    speed0 = speed0;
    speed1 = speed1;
    break;      
    case 1:
    if(speed0>0){
      speed0-=15;
    }else if(speed0<0){
      speed0+=15;
    }
    break;
    case 2:
    if(speed0>0){
      speed0-=30;
    }else if(speed0<0){
      speed0+=30;
    }
    break;
    case 3:
    if(speed0>0){
      speed0-=45;
    }else if(speed0<0){
      speed0+=45;
    }
    break;
    default:
    Serial.println("error");
    speed0=0;
    speed1=0;
  }
  Serial.println(speed0);
  Serial.println(speed1);
  
  //if speed is 0, break, otherwise set the speed of the motor to speed#
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

//for debugging
void showData() {
  Serial.print("Data received ");
  Serial.print(dataReceived[0],DEC);
  Serial.print(dataReceived[1],DEC);
  Serial.println(); 
}

//sends a 0 or 1 to the glove to tell the glove if the car is close to an object
void sendD() {
  char dataToSend[1];
  dataToSend[0] = ultraSonicData();
  
  radio.stopListening();
  
  bool rslt;
  rslt = radio.write(&dataToSend, sizeof(dataToSend));
        // Always use sizeof() as it gives the size as the number of bytes.
  radio.startListening();
  
  Serial.print("Data Sent ");
  Serial.print(dataToSend[0],DEC);
  if (rslt) {
    Serial.println("  Acknowledge received");
  }
  else {
    delay(15); //short to delay if transcievers fall out of sync
    Serial.println("  Tx failed");
  }
}

//determines if the car is close to an object
char ultraSonicData() {
  if(Ultrasonic.distanceRead()<50)
  {
    return 1; //if an object is close
  }
  else
  {
    return 0;
  }
}
