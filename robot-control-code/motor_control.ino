#include <Servo.h>
#include <mbed.h>
 
using namespace mbed;
using namespace rtos;
 
using namespace std::chrono_literals;

extern bool runCue;
extern Queue<uint8_t,32> motorServiceQueue;
extern Semaphore motorServiceQueueSemaphore;
extern Semaphore runQue;

#define speed_maxP 0 //Clockwise rotation (Max speed)
#define speed_maxN 180 //Anticlockwise rotation (Max speed)
#define speed_stop 90 //Stop
double left_scaling_factor = 1.20;

Servo motorLeft;
Servo motorRight;

void TaskMotorControl()  // This is a Task.
{
  motorRight.attach(10);
  motorLeft.attach(9);
  uint8_t motorControlInput = 5;
  uint8_t motorControlInputOld = 5;
  uint8_t* motorControlInput_p;

  uint8_t leftadjust = 0;
  uint8_t rightadjust = 0;

  while(1)
  {
    if(motorServiceQueue.try_get(&motorControlInput_p)){
      motorControlInput = *motorControlInput_p;
      delete motorControlInput_p;
    }

    bool cue = 0;
    if(motorControlInput != 5)
      cue = 1;

    runCueSemaphore.try_acquire();
    runCue = cue;
    runCueSemaphore.release();

    if(motorControlInput == 10)
    {
      if(rightadjust == 0)
        leftadjust += 5;
      else
        rightadjust -= 5;
      motorControlInput = motorControlInputOld;
    }
    if(motorControlInput == 12)
    {
      if(leftadjust == 0)
        rightadjust +=5;
      else
        leftadjust -= 5;
      motorControlInput = motorControlInputOld;
    }
    if(motorControlInput == 13)
    {
      if(rightadjust == 0)
        leftadjust += 10;
      else
        rightadjust -= 10;
      motorControlInput = motorControlInputOld;
    }
    if(motorControlInput == 15)
    {
      if(leftadjust == 0)
        rightadjust += 10;
      else
        leftadjust -= 10;
      motorControlInput = motorControlInputOld;
    }
    if(motorControlInput == 11)
    {
      leftadjust = 0;
      rightadjust = 0;
      motorControlInput = motorControlInputOld;
    }

    switch(motorControlInput)
    {
      case 1:
        //Serial.println("Long Curve Back Left");
        motorLeft.write(speed_stop);
        motorRight.write(speed_maxN);
        break;
      case 2:
        //Serial.println("Back");
        motorLeft.write(speed_maxN*left_scaling_factor - leftadjust);
        motorRight.write(speed_maxN - rightadjust);
        break;
      case 3:
        //Serial.println("Long Curve Back Right");
        motorLeft.write(speed_maxN);
        motorRight.write(speed_stop);
        break;
      case 4:
        //Serial.println("Differential Left");
        motorLeft.write(speed_maxN);
        motorRight.write(speed_maxP);
        break;
      case 5:
        Serial.println("Stop");
        motorLeft.write(speed_stop);
        motorRight.write(speed_stop);
        break;
      case 6:
        //Serial.println("Differential Right");
        motorLeft.write(speed_maxP);
        motorRight.write(speed_maxN);
        break;
      case 7:
        //Serial.println("Long Curve Forward Left");
        motorLeft.write(speed_stop);
        motorRight.write(speed_maxP);
        break;
      case 8:
        Serial.println("Forward");
        motorLeft.write(speed_maxP*left_scaling_factor - leftadjust);
        motorRight.write(speed_maxP - rightadjust);
        break;
      case 9:
        //Serial.println("Long Curve Forward Right");
        motorLeft.write(speed_maxP);
        motorRight.write(speed_stop);  
        break;
    }

    motorControlInputOld = motorControlInput;
    
    ThisThread::sleep_for(250ms);  // wait 250 ms
  }
}