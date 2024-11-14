#include <mbed.h>
 
using namespace mbed;
using namespace rtos;
 
using namespace std::chrono_literals;

extern Queue<uint8_t,32> pumpServiceQueue;
extern Semaphore pumpServiceQueueSemaphore;
extern Queue<double,128> pressureLQueue;
extern Queue<double,128> pressureRQueue;
extern Semaphore pressureLQueueSemaphore;
extern Semaphore pressureRQueueSemaphore;

#define PIN_pump 8
#define PIN_inlet_valve 18
#define PIN_outlet_valve 6
#define PIN_skin_valveL 7
#define PIN_skin_valveR 19

#define idle_state 0
#define operate_state 1

#define inflate_signal 14
#define deflate_signal 13 // placeholders at the moment
#define idle 0

void pump(PinStatus status) ///<Pump operation
{
 digitalWrite(PIN_pump,status);
}

void inflate(uint8_t side)
{
  digitalWrite(PIN_inlet_valve,HIGH);
  digitalWrite(PIN_outlet_valve,HIGH);
  digitalWrite(side,LOW);
  digitalWrite(PIN_pump,HIGH);
}

void deflate(uint8_t side)
{
  digitalWrite(PIN_inlet_valve,LOW);
  digitalWrite(PIN_outlet_valve,LOW);
  digitalWrite(side,HIGH);
  digitalWrite(PIN_pump,HIGH);
}

void stop(uint8_t side)
{
  digitalWrite(PIN_inlet_valve,LOW);
  digitalWrite(PIN_outlet_valve,LOW);
  digitalWrite(side,LOW);
  digitalWrite(PIN_pump,LOW);
}

class ThreeStateController {
public:
    // Constructor that sets the target and tolerance values
    ThreeStateController(uint8_t target, double tolerance)
        : target(target), tolerance(tolerance) {}

    void changeSetpoint(double nSetpoint) {
        setpoint = nSetpoint;
    }

    // Control function that returns "inflate", "deflate", or "stop"
    void control(double measured_value) {
        if (measured_value < setpoint - tolerance) {
            inflate(target);
        } else if (measured_value > setpoint + tolerance) {
            deflate(target);
        } else {
            stop(target);
        }
    }

private:
    uint8_t target;   // Skin to inflate
    double tolerance;  // Allowed tolerance around the setpoint
    double setpoint;
};

void TaskPumpControl()  // This is a Task.
{
  pinMode(PIN_pump, OUTPUT);
  pinMode(PIN_inlet_valve, OUTPUT);
  pinMode(PIN_outlet_valve, OUTPUT);
  pinMode(PIN_skin_valveL, OUTPUT);
  pinMode(PIN_skin_valveR, OUTPUT);

  uint8_t pumpControlInput = 0;
  uint8_t* pumpControlInput_p;
  double pressureL = 0;
  double* pressureL_p;
  double pressureR = 0;
  double* pressureR_p;

  double tolerance = 0.5;

  ThreeStateController Lcontroller(PIN_skin_valveL,tolerance);
  ThreeStateController Rcontroller(PIN_skin_valveR,tolerance);

  while(1)
  {
    if(pumpServiceQueueSemaphore.try_acquire()){
      if(pumpServiceQueue.try_get(&pumpControlInput_p)){
        pumpControlInput = *pumpControlInput_p;
        delete pumpControlInput_p;
      }
      pumpServiceQueueSemaphore.release();
    }

    if(pressureLQueueSemaphore.try_acquire()){
      if(pressureLQueue.try_get(&pressureL_p)){
        pressureL = *pressureL_p;
        delete pressureL_p;
      }
      pressureLQueueSemaphore.release();
    }

    if(pressureRQueueSemaphore.try_acquire()){
      if(pressureRQueue.try_get(&pressureR_p)){
        pressureR = *pressureR_p;
        delete pressureR_p;
      }
      pressureRQueueSemaphore.release();
    }
    //Serial.println(pumpControlInput);
    
    Lcontroller.changeSetpoint(0.5);
    Lcontroller.control(pressureL);

    ThisThread::sleep_for(50ms); //sleep for 50 ms
  }
}

/*
void TaskPumpControl()  // This is a Task.
{
  pinMode(PIN_pump, OUTPUT);
  pinMode(PIN_inlet_valve, OUTPUT);
  pinMode(PIN_outlet_valve, OUTPUT);
  pinMode(PIN_skin_valve, OUTPUT);

  uint8_t pump_state = idle_state;
  uint8_t pumpControlInput = 0;
  uint8_t* pumpControlInput_p;

  while(1)
  {
    if(pumpServiceQueueSemaphore.try_acquire()){
      if(pumpServiceQueue.try_get(&pumpControlInput_p)){
        pumpControlInput = *pumpControlInput_p;
        delete pumpControlInput_p;
      }
      pumpServiceQueueSemaphore.release();
    }
    //Serial.println(pumpControlInput);
    
    switch (pump_state)
    {
      case idle_state:

        if(pumpControlInput == inflate)
        {
          //Serial.println("Inflate");
          digitalWrite(PIN_inlet_valve,HIGH);
          digitalWrite(PIN_outlet_valve,HIGH);
          digitalWrite(PIN_skin_valve,LOW);
          digitalWrite(PIN_pump,HIGH);
          pump_state = operate_state;
        }
          
        if(pumpControlInput == deflate)
        {
          //Serial.println("Deflate");
          digitalWrite(PIN_inlet_valve,LOW);
          digitalWrite(PIN_outlet_valve,LOW);
          digitalWrite(PIN_skin_valve,HIGH);
          pump(HIGH);
          pump_state = operate_state;
        }
        
        break;

      case operate_state:

        if(pumpControlInput == idle)
        {
          //Serial.println("Idle");
          digitalWrite(PIN_inlet_valve,LOW);
          digitalWrite(PIN_outlet_valve,LOW);
          digitalWrite(PIN_skin_valve,LOW);
          pump(LOW);
          pump_state = idle_state;
        }

        break;
    }

    ThisThread::sleep_for(50ms); //sleep for 50 ms
  }
}
*/
