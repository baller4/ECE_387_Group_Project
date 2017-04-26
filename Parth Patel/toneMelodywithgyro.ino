#include <quaternionFilters.h>
#include <MPU9250.h>
#include <Wire.h>
#include <SPI.h>
#include "pitches.h"


int val;
int prevVal;

MPU9250 myIMU;

void setup() {
  
  
  //--------------------------------------------------------------------//
  Wire.begin();
	Serial.begin(38400);   
	
    // Self testing the MPU9250 and reporting values
    myIMU.MPU9250SelfTest(myIMU.SelfTest);
    // Calibrate gyro and accelerometers, load biases in bias registers
    // void calibrateMPU9250(float * gyroBias, float * accelBias);
    myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);
  //--------------------------------------------------------------------//
}

void loop() {
  if (myIMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01)
  {  
    myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
    myIMU.getAres();
	myIMU.ax = myIMU.accelCount[0]; // using raw values w/o converting to g's 
	myIMU.ay = myIMU.accelCount[1];
	myIMU.az = myIMU.accelCount[2];
	  
	/*
	// This is the correct bias values..... values range from (-1 to 0 to 1).
	myIMU.ax = (float)myIMU.accelCount[0] * myIMU.aRes - myIMU.accelBias[0]; // using raw values w/o converting to g's 
	myIMU.ay = (float)myIMU.accelCount[1] * myIMU.aRes - myIMU.accelBias[1];
	myIMU.az = (float)myIMU.accelCount[2] * myIMU.aRes - myIMU.accelBias[2];
	*/
	//----------------------------------//
	myIMU.readGyroData(myIMU.gyroCount);  // Read the x/y/z adc values
    myIMU.getGres();
	myIMU.gx = myIMU.gyroCount[0]; // using raw values w/o converting to degrees per sec
	myIMU.gy = myIMU.gyroCount[1];
	myIMU.gz = myIMU.gyroCount[2];
	
	
	// Printing Values... Used for testing and confirming what range of values to use for their respective function
	Serial.print("X - Acceleration = "); Serial.print(myIMU.ax);
	Serial.print(" | Y - Acceleration = "); Serial.print(myIMU.ay);
	Serial.print(" | Z - Acceleration = "); Serial.print(myIMU.az);
	Serial.print("\t");
	Serial.print(" |  X - Gyroscope = "); Serial.print(myIMU.gx);
	Serial.print(" | Y - Gyroscope = "); Serial.print(myIMU.gy);
	Serial.print(" | Z - Gyroscope = "); Serial.println(myIMU.gz);
  }

//----------------------------------TONE----------------------------------------//

/* map(value, fromLow, fromHigh, toLow, toHigh)
*  The map() function re-maps a number from one range to another. That is, a value of fromLow 
*  would get mapped to toLow, a value of fromHigh to toHigh, values in-between to values in-between, etc.
*/ 
  val = map(myIMU.ax, -20000, 20000, 120, 1500);
  prevVal = map(myIMU.ay, -20000, 20000, 120, 1500);
  if (myIMU.gy <= -9000 || myIMU.gy >= 9000)
   {
     /* tone(pin, frequency, duration)
     * I am utilizing the tone() function to output a frequency range of 120-1500Hz that is mapped from 
     * the map() function.... map(lowAccelerometer, highAccelerometer, lowFrequency, highFrequency);
     */
     tone(8, val, 500);
   }
  else if(myIMU.gx <= -9000 || myIMU.gx >= 9000)
   {
     tone(8, prevVal, 500);
   }
}
