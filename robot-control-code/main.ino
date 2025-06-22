#include <mbed.h>
#include <ArduinoBLE.h>
#include "Arduino_BMI270_BMM150.h"

using namespace mbed;
using namespace rtos;
 
using namespace std::chrono_literals;

bool runCue = 0;

Queue<uint8_t,32> motorServiceQueue;
Queue<int,32> pumpServiceQueue;
Queue<bool,16> pressureServiceRXQueue;
Queue<int,128> pressureServiceTXQueue;
Queue<double,128> pressureLQueue;
Queue<double,128> pressureRQueue;

Semaphore motorServiceQueueSemaphore(1);
Semaphore pumpServiceQueueSemaphore(1);
Semaphore pressureServiceRXQueueSemaphore(1);
Semaphore pressureServiceTXQueueSemaphore(1);
Semaphore pressureLQueueSemaphore(1);
Semaphore pressureRQueueSemaphore(1);
Semaphore runCueSemaphore(1);

Thread t1;
Thread t2;
Thread t3;
Thread t4;
Thread t5;

void setup() {
  Serial.begin(9600);
  //while (!Serial);

  // set LED's pin to output mode
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(LED_BUILTIN, LOW);         // when the central disconnects, turn off the LED
  digitalWrite(LEDR, HIGH);               // will turn the LED off
  digitalWrite(LEDG, HIGH);               // will turn the LED off
  digitalWrite(LEDB, HIGH);                // will turn the LED off

  bluetooth_init();

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println("Hz");

  t1.start(TaskBluetooth);
  t2.start(TaskMotorControl);
  t3.start(TaskPumpControl);
  t4.start(TaskPressureSense);
  //t5.start(TaskPosition);
}
void loop() {
  
}