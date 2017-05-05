#include <SPI.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10

const byte slaveAddress[5] = {'R','x','A','A','A'};
const byte masterAddress[5] = {'T','X','a','a','a'};


RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

char dataMoveForward[] = {2, 0};
char dataStop[] = {0,0};
char dataReceived[4];

void setup() {

  Serial.begin(38400);

  Serial.println("SimpleTx Starting");
    
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_MIN);
  
  radio.openWritingPipe(slaveAddress);
  radio.openReadingPipe(1, masterAddress);
}

void loop() {
  while(1) {
    send(dataMoveForward[0],dataMoveForward[1]);
    delay(200);
    readData();
    delay(200);
  }
}

void send(char out1,char out2) {
  char data[]={out1,out2};
  radio.stopListening();
  bool rslt;
  rslt = radio.write(&data, sizeof(data));
      // Always use sizeof() as it gives the size as the number of bytes.
      // For example if dataToSend was an int sizeof() would correctly return 2
  radio.startListening();

  Serial.print("Data Sent ");
  Serial.print(data);
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
      
    //show the read data
    showData();
  }
}

void showData() {
  Serial.print("Data received ");
  Serial.print(dataReceived);
  Serial.println();
}