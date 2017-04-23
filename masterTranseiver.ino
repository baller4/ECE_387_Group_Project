#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 9
#define CSN_PIN 10

const byte slaveAddress[5] = {'R','x','A','A','A'};
const byte masterAddress[5] = {'T','X','a','a','a'};


RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

char dataToSend[4] = "AAA";
char dataReceived[4];

void setup() {

  Serial.begin(9600);

  Serial.println("SimpleTx Starting");
    
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(108);
  radio.setPALevel(RF24_PA_MIN);
    
  radio.setRetries(1,15); // delay, count
  radio.openWritingPipe(slaveAddress);
  radio.openReadingPipe(1, masterAddress);
}

void loop() {
  send();
  delay(1000);
  readData();
}

void send() {
  
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
  delay(500);
}
