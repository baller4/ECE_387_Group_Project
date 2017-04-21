//  Demo function:The application method to drive the DC motor.
//  Author:Loovee (luweicong@seeed.cc)
//  2016-3-11

#include "MotorDriver.h"

MotorDriver motor;

void setup()
{
    // initialize
    motor.begin();
}

void loop()
{
    motor.speed(0, 100);            // set motor0 to speed 100
    motor.speed(1, 50);
    delay(1000);
    motor.brake(0);                 // brake
    motor.brake(1);   
    delay(1000);
    motor.speed(0, -100);           // set motor0 to speed -100
    motor.speed(1, -100);
    delay(1000);
    motor.stop(0);                  // stop
    motor.stop(1);
    delay(1000);
}
// END FILE
