#include <mbed.h>
 
using namespace mbed;
using namespace rtos;
 
using namespace std::chrono_literals;

extern Queue<bool,16> pressureServiceRXQueue;
extern Queue<uint8_t,128> pressureServiceTXQueue;
extern Queue<double,128> pressureLQueue;
extern Queue<double,128> pressureRQueue;
extern Semaphore pressureServiceRXQueueSemaphore;
extern Semaphore pressureServiceTXQueueSemaphore;
extern Semaphore pressureLQueueSemaphore;
extern Semaphore pressureRQueueSemaphore;

#define pressure_pin A1
#define idle_state 0
#define log_state 1
#define lock_state 2

double pressureL()
{
  double temp;
  temp = analogRead(pressure_pin);
  temp = temp/1023;
  temp = 5*(5*temp - 2.5); //gives pressure in kPa relative to atm
  return temp;
}

double pressureR()
{
  double temp;
  temp = analogRead(pressure_pin);
  temp = temp/1023;
  temp = 5*(5*temp - 2.5); //gives pressure in kPa relative to atm
  return temp;
}

void TaskPressureSense()
{
  pinMode(pressure_pin,INPUT);

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
          uint8_t *message = new uint8_t(10);
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
          Serial.println("\033c");
        }

        break;

      case log_state:
        if(pressureServiceTXQueueSemaphore.try_acquire())
        {
          uint8_t *message = new uint8_t(20);
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
          sensor_state = idle_state;
          Serial.println("\033c");
        }

        break;

    }
    ThisThread::sleep_for(40ms);
  }
}