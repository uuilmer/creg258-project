#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <SparkFunLSM9DS1.h>

/* Constants for use with transmitter */
// Pins that we connected to CE and CSN of the transmitter
RF24 radio(6, 7);
// Address that must be matching with receivere for a connection
const byte address[6] = "00001";
// Buffer containing messages we wish to send
String GyroscopeReading[1] = {""};

// LED that we will use to indicate proper connection to the transmitter
#define LED 2

/* Constants for IMU readings */
// The imu object that will provide us with gyroscope readings
LSM9DS1 imu;
// Speed at which wee will send gyroscope updates
#define UPDATE_SPEED 50
// Keep track of print time
static unsigned long lastPrint = 0;

/**
 * We must set up the transmitter, and the IMU
 */
void setup()
{
    // Setup LED indicator
    pinMode(LED, OUTPUT);

    // Setup required modules
    setupIMUHelper();
    setupRadioHelper();
}

/**
 * Attempt to establish a radio connection
 */
void setupRadioHelper()
{
    if (radio.begin(6, 7))
    {
        Serial.println("Connected to transmitter...");
        // Indicate a physical successful transmitter connection
        digitalWrite(LED, HIGH);
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

    if (imu.begin() == false)
    {
        Serial.println("Failed to communicate with LSM9DS1.");
        Serial.println("Double-check wiring.");
        while (1)
            ;
    }
}

/**
 * At each iteration, we must read the gyroscope and transmit through transmitter
 */
void loop()
{
    // Wait until we can measure again
    if ((lastPrint + UPDATE_SPEED) >= millis())
    {
        return;
    }

    GyroscopeReading[0] = readGyroscope();

    // Send reading to receiver
    for (auto c : GyroscopeReading[0])
    {
        radio.write(&c, sizeof(c));
    }

    // Update lastPrint time
    lastPrint = millis();
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
