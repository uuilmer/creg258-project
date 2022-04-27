/**
 * 
 * @todo
 *  - move strings to flash (less RAM consumption)
 *  - fix deprecated convertation form string to char* startAsTag
 *  - give example description
 */
#include <SPI.h>
#include "DW1000Ranging.h"
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <SparkFunLSM9DS1.h>

// connection pins
const uint8_t PIN_RST = 1; // reset pin
const uint8_t PIN_IRQ = 7; // irq pin
const uint8_t PIN_SS = 2; // spi select pin

/* Constants for use with transmitter */
// Pins that we connected to CE and CSN of the transmitter
RF24 radio(0, 6);
// Address that must be matching with receivere for a connection
const byte address[6] = "00001";
// Buffer containing messages we wish to send
String GyroscopeReading[1] = {""};

/* Constants for IMU readings */
// The imu object that will provide us with gyroscope readings
LSM9DS1 imu;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Setup required modules
  setupIMUHelper();
  setupRadioHelper();

    
  //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
  //define the sketch as anchor. It will be great to dynamically change the type of module
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
  //Enable the filter to smooth the distance
  //DW1000Ranging.useRangeFilter(true);
  
  //we start the module as a tag
  DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY);
}

/**
 * Attempt to establish a radio connection
 */
void setupRadioHelper()
{
    if (radio.begin(0, 6))
    {
        Serial.println("Connected to transmitter...");
    }
    else
    {
        Serial.println("Failed to connect to transmitter...");
        return;
    }

    // Setup the transmitter
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN);
    radio.stopListening();
}

/**
 * Attempt to setup IMU for gyroscope readings
 */
void setupIMUHelper()
{
    Wire.begin();
    imu.begin();
}

void loop() {
  DW1000Ranging.loop();
}

void newRange() {
  GyroscopeReading[0] = readGyroscope();

  float x = DW1000Ranging.getDistantDevice()->getRange();
  float y = 0;
  float z = 0;

  String UWBReading = String(x) + " " + String(y) + " " + String(z) + " ";
  
  String reading = UWBReading + GyroscopeReading[0];

  Serial.println(reading);

  // Send reading to receiver
  for (auto c : reading)
  {
      radio.write(&c, sizeof(c));
  }
}

/**
 * Attempt to read a gyroscope reading from IMU
 */
String readGyroscope()
{
    // Update the sensor values whenever new data is available
    if (imu.gyroAvailable())
    {
        // To read from the gyroscope,  first call the
        // readGyro() function. When it exits, it'll update the
        // gx, gy, and gz variables with the most current data.
        imu.readGyro();
    }

    return String(imu.calcGyro(imu.gx)) + " " + String(imu.calcGyro(imu.gy)) + " " + String(imu.calcGyro(imu.gz)) + ",";
}

void newDevice(DW1000Device* device) {
  Serial.print("ranging init; 1 device added ! -> ");
  Serial.print(" short:");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device* device) {
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
}
