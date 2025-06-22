#include <mbed.h>
#include <string>
 
using namespace mbed;
using namespace rtos;
 
using namespace std::chrono_literals;

extern Queue<bool,16> pressureServiceRXQueue;
extern Queue<int,128> pressureServiceTXQueue;
extern Queue<double,128> pressureLQueue;
extern Queue<double,128> pressureRQueue;
extern Semaphore pressureServiceRXQueueSemaphore;
extern Semaphore pressureServiceTXQueueSemaphore;
extern Semaphore pressureLQueueSemaphore;
extern Semaphore pressureRQueueSemaphore;

#define pressureL_pin A0
#define pressureR_pin A1
#define idle_state 0
#define log_state 1
#define lock_state 2

double pressureL()
{
  analogReadResolution(12);
  double temp;
  temp = analogRead(pressureL_pin);
  temp = temp/4095;
  temp = (20.0/2.64)*(3.3*temp)-12.5; //gives pressure in kPa relative to atm
  double offset = 6.68;
  return ((temp));
}

double pressureR()
{
  analogReadResolution(12);
  double temp;
  temp = analogRead(pressureR_pin);
  temp = temp/4095;
  temp = 5*(5*temp - 2.5); //gives pressure in kPa relative to atm
  double offset = 6.64;
  return ((temp - offset)*1000);
}

void TaskPressureSense()
{
  pinMode(pressureL_pin,INPUT);
  pinMode(pressureR_pin,INPUT);

  uint8_t sensor_state = idle_state;
  bool run = 0;
  bool lock_monitor = 0;

  while(1)
  {
    runCueSemaphore.try_acquire();
    run = runCue;
    runCueSemaphore.release();

    switch(sensor_state)
    {
      case idle_state:
        if(pressureServiceTXQueueSemaphore.try_acquire())
        {
          Serial.println(pressureL(),8);
          int *message = new int(pressureL()*1000);
          pressureServiceTXQueue.try_put(message);
          pressureServiceTXQueueSemaphore.release();
        }

        if(pressureServiceTXQueueSemaphore.try_acquire())
        {
          //Serial.println(pressureR());
          int *message = new int(pressureR()*1000);
          pressureServiceTXQueue.try_put(message);
          pressureServiceTXQueueSemaphore.release();
        }

        if(pressureLQueueSemaphore.try_acquire())
        {
          double *message = new double(pressureL());
          pressureLQueue.try_put(message);
          pressureLQueueSemaphore.release();
        }

        if(pressureRQueueSemaphore.try_acquire())
        {
          double *message = new double(pressureR());
          pressureRQueue.try_put(message);
          pressureRQueueSemaphore.release();
        }

        if(run)
        {
          sensor_state = log_state;
        }

        break;

      case log_state:
        if(pressureServiceTXQueueSemaphore.try_acquire())
        {
          int *message = new int(pressureL());
          pressureServiceTXQueue.try_put(message);
          pressureServiceTXQueueSemaphore.release();
        }

        if(pressureServiceTXQueueSemaphore.try_acquire())
        {
          int *message = new int(pressureR());
          pressureServiceTXQueue.try_put(message);
          pressureServiceTXQueueSemaphore.release();
        }

        if(!run)
        {
          sensor_state = lock_state;
        }
      
        break;

      case lock_state:
        bool* pressureServiceRX;
        //Serial.println("lock_state");
        if(pressureServiceRXQueueSemaphore.try_acquire()){
          if(pressureServiceRXQueue.try_get(&pressureServiceRX)){
            Serial.println(*pressureServiceRX);
            lock_monitor = *pressureServiceRX;
            delete pressureServiceRX;
        }
        pressureServiceRXQueueSemaphore.release();
        }
        if(lock_monitor)
        {
          lock_monitor = 0;
          sensor_state = idle_state;
        }

        break;

    }
    ThisThread::sleep_for(40ms);
  }
}