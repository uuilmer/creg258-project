#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SparkFunLSM9DS1.h>
#include "Adafruit_TinyUSB.h"

/* Constants for use with receiver */
// Pins that we connected to CE and CSN of the receiver
RF24 radio(6, 7);
// Address that must be matching with receivere for a connection
const byte address[6] = "00001";
// Buffer containing messages we wish to receive
char ReceivedGyroscopeReading[1];

// LED that we will use to indicate proper connection to the receiver
#define LED 2

/* Constants for TinyUSB */
// USB WebUSB object
Adafruit_USBD_WebUSB usb_web;
// Landing Page: scheme (0: http, 1: https), url
WEBUSB_URL_DEF(landingPage, 1 /*http*/, "compe-capstone.herokuapp.com/");

/**
 * We must set up the transmitter, tinyUSB, and the IMU
 */
void setup()
{
    Serial.begin(115200);
    // Setup LED indicator
    pinMode(LED, OUTPUT);

    // Setup required modules
    setupUSBHelper();
    setupRadioHelper();
}

/**
 * Attempt to establish a radio connection
 */
void setupRadioHelper()
{
    if (radio.begin(6, 7))
    {
        Serial.println("Connected to receiver...");
        // Indicate a physical successful receiver connection
        digitalWrite(LED, HIGH);
    }
    else
    {
        Serial.println("Failed to connect to receiver...");
        return;
    }

    // Setup the receiver
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();
}

/**
 * Attempt to setup USB transmission
 */
void setupUSBHelper()
{
#if defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040)
    // Manual begin() is required on core without built-in support for TinyUSB such as mbed rp2040
    TinyUSB_Device_Init(0);
#endif

    usb_web.setLandingPage(&landingPage);
    usb_web.begin();

    // Wait until device mounted
    while (!USBDevice.mounted())
        delay(1);
}

/**
 * At each iteration, we must get from transmitter and send to USB
 */
void loop()
{
    if (radio.available())
    {
        radio.read(ReceivedGyroscopeReading, sizeof(ReceivedGyroscopeReading));
        usb_web.write(ReceivedGyroscopeReading, sizeof(ReceivedGyroscopeReading));
    }
}
