#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgTime.hpp>
#include <DW1000NgConstants.hpp>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <SparkFunLSM9DS1.h>

// connection pins
const uint8_t PIN_RST = 1; // reset pin
const uint8_t PIN_IRQ = 7; // irq pin
const uint8_t PIN_SS = 2; // spi select pin

// messages used in the ranging protocol
#define POLL 0
#define POLL_ACK 1
#define RANGE 2
#define RANGE_REPORT 3
#define RANGE_FAILED 255
// message flow state
volatile byte expectedMsgId = POLL_ACK;
// message sent/received state
volatile boolean sentAck = false;
volatile boolean receivedAck = false;
// timestamps to remember
uint64_t timePollSent;
uint64_t timePollAckReceived;
uint64_t timeRangeSent;
// data buffer
#define LEN_DATA 16
byte data[LEN_DATA];
// watchdog and reset period
uint32_t lastActivity;
uint32_t resetPeriod = 100;
// reply times (same on both sides for symm. ranging)
uint16_t replyDelayTimeUS = 1000;

device_configuration_t DEFAULT_CONFIG = {
    false,
    true,
    true,
    true,
    false,
    SFDMode::DECAWAVE_SFD,
    Channel::CHANNEL_5,
    DataRate::RATE_6800KBPS,
    PulseFrequency::FREQ_64MHZ,
    PreambleLength::LEN_256,
    PreambleCode::CODE_3
};

interrupt_configuration_t DEFAULT_INTERRUPT_CONFIG = {
    true,
    true,
    true,
    false,
    true
};

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
  
    // Setup required modules
    setupIMUHelper();
    setupRadioHelper();


    
    // DEBUG monitoring
    Serial.println(F("### DW1000Ng-arduino-ranging-tag ###"));
    // initialize the driver
    DW1000Ng::initialize(PIN_SS, PIN_IRQ, PIN_RST);
    Serial.println("DW1000Ng initialized ...");
    // general configuration
    DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
  DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);

    DW1000Ng::setNetworkId(10);
    
    DW1000Ng::setAntennaDelay(16436);
    
    Serial.println(F("Committed configuration ..."));
    // DEBUG chip info and registers pretty printed
    char msg[128];
    DW1000Ng::getPrintableDeviceIdentifier(msg);
    Serial.print("Device ID: "); Serial.println(msg);
    DW1000Ng::getPrintableExtendedUniqueIdentifier(msg);
    Serial.print("Unique ID: "); Serial.println(msg);
    DW1000Ng::getPrintableNetworkIdAndShortAddress(msg);
    Serial.print("Network ID & Device Address: "); Serial.println(msg);
    DW1000Ng::getPrintableDeviceMode(msg);
    Serial.print("Device mode: "); Serial.println(msg);
    // attach callback for (successfully) sent and received messages
    DW1000Ng::attachSentHandler(handleSent);
    DW1000Ng::attachReceivedHandler(handleReceived);
    // anchor starts by transmitting a POLL message
    transmitPoll();
    noteActivity();
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



void noteActivity() {
    // update activity timestamp, so that we do not reach "resetPeriod"
    lastActivity = millis();
}

void resetInactive() {
    // tag sends POLL and listens for POLL_ACK
    expectedMsgId = POLL_ACK;
    DW1000Ng::forceTRxOff();
    transmitPoll();
    noteActivity();
}

void handleSent() {
    // status change on sent success
    sentAck = true;
}

void handleReceived() {
    // status change on received success
    receivedAck = true;
}

void transmitPoll() {
    data[0] = POLL;
    DW1000Ng::setTransmitData(data, LEN_DATA);
    DW1000Ng::startTransmit();
}

void transmitRange() {
    data[0] = RANGE;

    /* Calculation of future time */
    byte futureTimeBytes[LENGTH_TIMESTAMP];

  timeRangeSent = DW1000Ng::getSystemTimestamp();
  timeRangeSent += DW1000NgTime::microsecondsToUWBTime(replyDelayTimeUS);
    DW1000NgUtils::writeValueToBytes(futureTimeBytes, timeRangeSent, LENGTH_TIMESTAMP);
    DW1000Ng::setDelayedTRX(futureTimeBytes);
    timeRangeSent += DW1000Ng::getTxAntennaDelay();

    DW1000NgUtils::writeValueToBytes(data + 1, timePollSent, LENGTH_TIMESTAMP);
    DW1000NgUtils::writeValueToBytes(data + 6, timePollAckReceived, LENGTH_TIMESTAMP);
    DW1000NgUtils::writeValueToBytes(data + 11, timeRangeSent, LENGTH_TIMESTAMP);
    DW1000Ng::setTransmitData(data, LEN_DATA);
    DW1000Ng::startTransmit(TransmitMode::DELAYED);
    //Serial.print("Expect RANGE to be sent @ "); Serial.println(timeRangeSent.getAsFloat());
}

void loop() {
    if (!sentAck && !receivedAck) {
        // check if inactive
        if (millis() - lastActivity > resetPeriod) {
            resetInactive();
        }
        return;
    }
    // continue on any success confirmation
    if (sentAck) {
        sentAck = false;
        DW1000Ng::startReceive();
    }
    if (receivedAck) {
        receivedAck = false;
        // get message and parse
        DW1000Ng::getReceivedData(data, LEN_DATA);
        byte msgId = data[0];
        if (msgId != expectedMsgId) {
            // unexpected message, start over again
            //Serial.print("Received wrong message # "); Serial.println(msgId);
            expectedMsgId = POLL_ACK;
            transmitPoll();
            return;
        }
        if (msgId == POLL_ACK) {
            timePollSent = DW1000Ng::getTransmitTimestamp();
            timePollAckReceived = DW1000Ng::getReceiveTimestamp();
            expectedMsgId = RANGE_REPORT;
            transmitRange();
            noteActivity();
        } else if (msgId == RANGE_REPORT) {
            expectedMsgId = POLL_ACK;
            float curRange;
            memcpy(&curRange, data + 1, 4);
            transmitPoll();
            noteActivity();

            float x = curRange;
            float y = 0;
            float z = 0;

            // Update the sensor values whenever new data is available
            if (imu.gyroAvailable())
            {
                // To read from the gyroscope,  first call the
                // readGyro() function. When it exits, it'll update the
                // gx, gy, and gz variables with the most current data.
                imu.readGyro();
            }

            char buff[8]; // Buffer big enough for 7-character float
            dtostrf(x, 0, 2, buff);
            for (auto c : buff) {
              radio.write(&c, sizeof(c));
            }
            radio.write(" ", sizeof(char));

            dtostrf(y, 0, 2, buff);
            for (auto c : buff) {
              radio.write(&c, sizeof(c));
            }
            radio.write(" ", sizeof(char));

            dtostrf(z, 0, 2, buff);
            for (auto c : buff) {
              radio.write(&c, sizeof(c));
            }
            radio.write(" ", sizeof(char));
            
            dtostrf(imu.calcGyro(imu.gx), 0, 2, buff);
            for (auto c : buff) {
              radio.write(&c, sizeof(c));
            }
            radio.write(" ", sizeof(char));

            dtostrf(imu.calcGyro(imu.gy), 0, 2, buff);
            for (auto c : buff) {
              radio.write(&c, sizeof(c));
            }
            radio.write(" ", sizeof(char));

            dtostrf(imu.calcGyro(imu.gz), 0, 2, buff);
            for (auto c : buff) {
              radio.write(&c, sizeof(c));
            }
            radio.write(",", sizeof(char));
        } else if (msgId == RANGE_FAILED) {
            expectedMsgId = POLL_ACK;
            transmitPoll();
            noteActivity();
        }
    }
}
