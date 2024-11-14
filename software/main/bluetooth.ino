#include <ArduinoBLE.h>
#include <mbed.h>
 
using namespace mbed;
using namespace rtos;
 
using namespace std::chrono_literals;

extern Queue<uint8_t,32> motorServiceQueue;
extern Queue<uint8_t,32> pumpServiceQueue;
extern Queue<bool,16> pressureServiceRXQueue;
extern Queue<uint8_t,128> pressureServiceTXQueue;
extern Semaphore motorServiceQueueSemaphore;
extern Semaphore pumpServiceQueueSemaphore;
extern Semaphore pressureServiceRXQueueSemaphore;
extern Semaphore pressureServiceTXQueueSemaphore;

BLEService motorService("180A"); // BLE motor Service
BLEService pumpService("180B");
BLEService pressureService("181C");

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic motorCharacteristic("2A57", BLEWrite);
BLEByteCharacteristic pumpCharacteristic("2A57", BLEWrite);
BLEByteCharacteristic pressureCharacteristic("2A6D", BLENotify); //2A6D UUID for pressure
BLEBoolCharacteristic monitorCharacteristic("2A57", BLEWrite);


void bluetooth_init()
{

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("DAWN");
  BLE.setAdvertisedService(motorService);
  BLE.setAdvertisedService(pumpService);
  BLE.setAdvertisedService(pressureService);

  // add the characteristic to the service
  motorService.addCharacteristic(motorCharacteristic);
  pumpService.addCharacteristic(pumpCharacteristic);
  pressureService.addCharacteristic(pressureCharacteristic);
  pressureService.addCharacteristic(monitorCharacteristic);

  // add service
  BLE.addService(motorService);
  BLE.addService(pumpService);
  BLE.addService(pressureService);

  // set the initial value for the characteristic:
  motorCharacteristic.writeValue(0);
  pumpCharacteristic.writeValue(0);
  pressureCharacteristic.writeValue(0);
  monitorCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("DAWN Peripheral");
}

void TaskBluetooth()
{
  while(1)
  {
    // listen for BLE peripherals to connect:
    BLEDevice central = BLE.central();

    // if a central is connected to peripheral:
    if (central) {
      Serial.print("Connected to central: ");
      // print the central's MAC address:
      Serial.println(central.address());
      digitalWrite(LED_BUILTIN, HIGH);            // turn on the LED to indicate the connection

      // while the central is still connected to peripheral:
      while (central.connected()) {
        // if the remote device wrote to the characteristic,
        // use the value to control the LED:
        if (motorCharacteristic.written()) {
          motorServiceQueueSemaphore.acquire();
          uint8_t *message = new uint8_t(motorCharacteristic.value());
          motorServiceQueue.put(message);
          motorServiceQueueSemaphore.release();
        }
        if (pumpCharacteristic.written()) {
          pumpServiceQueueSemaphore.acquire();
          uint8_t *message = new uint8_t(pumpCharacteristic.value());
          pumpServiceQueue.put(message);
          pumpServiceQueueSemaphore.release();
        }
        if (monitorCharacteristic.written()) {
          Serial.println("hej");
          if(pressureServiceRXQueueSemaphore.try_acquire()){
            bool *message = new bool(monitorCharacteristic.value());
            Serial.println(*message);
            pressureServiceRXQueue.try_put(message);
            pressureServiceRXQueueSemaphore.release();
          }
        }
        if(pressureServiceTXQueueSemaphore.try_acquire()){
          uint8_t* message;
          if(pressureServiceTXQueue.try_get(&message)){
            pressureCharacteristic.writeValue(*message);
            delete message;
          }
          pressureServiceTXQueueSemaphore.release();
        }
      }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW);         // when the central disconnects, turn off the LED
    digitalWrite(LEDR, HIGH);          // will turn the LED off
    digitalWrite(LEDG, HIGH);        // will turn the LED off
    digitalWrite(LEDB, HIGH);         // will turn the LED off
    }
    ThisThread::sleep_for(10ms);
  }
}