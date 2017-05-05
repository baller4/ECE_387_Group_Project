#include <RF24.h>
#include <MPU9250.h>
#include <quaternionFilters.h>
#include <Wire.h>
#include <SPI.h>
#include <Sparkfun_DRV2605L.h>
#include <Wire.h>

#define CE_PIN 9
#define CSN_PIN 10

const byte slaveAddress[5] = {'R','x','A','A','A'};
const byte masterAddress[5] = {'T','X','a','a','a'};

char carDirac;
char carSpeed;

RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

char dataMoveForward = 65;
char dataStop = 66;
char dataReceived[4];

MPU9250 myIMU;
const int SENSOR_PIN = 0;      // Analog input pin

int average=0;

const int out = 6;
const int intensity=5;

SFE_HMD_DRV2605L HMD;

void setup() {

  Serial.begin(38400);

  Serial.println("SimpleTx Starting");
  myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);
    
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_MIN);
  
  radio.openWritingPipe(slaveAddress);
  radio.openReadingPipe(1, masterAddress);
  pinMode(out,OUTPUT);
  pinMode(intensity,OUTPUT);
  HMD.Mode(0x00); //PWM INPUT 
  HMD.MotorSelect(0x0A);
  HMD.Library(6); //change to 7 for ERM motors 
  HMD.begin();
}

void loop() {
   myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
   myIMU.getAres();
   myIMU.ay = (float)myIMU.accelCount[1] * myIMU.aRes - myIMU.accelBias[1];
   Serial.print(myIMU.ay);
  sensorData();
  send(carSpeed, carDirac);
  delay(50);
  readData();
  delay(50);
}


void sensorData(void){
  int sensorValue;
  sensorValue = analogRead(SENSOR_PIN);
  if (sensorValue<=100&&sensorValue>=0){
    carSpeed=0;
  }
  else if(sensorValue>100&&sensorValue<=400){
    carSpeed=-1;
  }
  else {
    carSpeed=1;
  }
    if(myIMU.ay>-0.200&&myIMU.ay<0.200){
      carDirac=0;
    }
     if(myIMU.ay>=0.200&&myIMU.ay<0.350){
    carDirac=-1;
  }
  else if (myIMU.ay>=0.350&&myIMU.ay<0.550){
    carDirac=-2;
  }
  else if(myIMU.ay>=0.550&&myIMU.ay<0.800){
    carDirac=-3;
  }
  else if(myIMU.ay<=-0.200&&myIMU.ay>-0.350){
     carDirac=1;
  }
  else if(myIMU.ay<=-0.350&&myIMU.ay>-0.550){
     carDirac=2;
  }
  else if (myIMU.ay<=-0.550&&myIMU.ay>=-0.800){
      carDirac=3;
  }
}
void send(char out1, char out2) {
  char data[]={out1, out2};
  radio.stopListening();
  bool rslt;
  rslt = radio.write(&data, sizeof(data));
      // Always use sizeof() as it gives the size as the number of bytes.
      // For example if dataToSend was an int sizeof() would correctly return 2
  radio.startListening();

  Serial.print("Data Sent ");
  Serial.print(data[0],DEC);
  Serial.print(data[1],DEC);
  if (rslt) {
    Serial.println("  Acknowledge received");
  }
  else {
    Serial.println("  Tx failed");
  }
}

void readData() {
  
  if (radio.available()) {
    
    //read the data and store it in dataReceived
    radio.read(&dataReceived, sizeof(dataReceived));

    haptics(dataReceived[0]);
    
    //show the read data
    showData();
  }
}

void showData() {
  Serial.print("Data received ");
  Serial.print(dataReceived[0],DEC);
  Serial.println();
}


void haptics(char input) {
boolean haptic = false;
Serial.print("Input: ");
Serial.println(input, DEC);
if(input == 1) {
  haptic = true;
}
else {
  haptic = false;
}

 if(haptic){
  HMD.Waveform(40,100);
  HMD.go();
 }
 else{
  HMD.stop();
 }
 Serial.println(".");
}
