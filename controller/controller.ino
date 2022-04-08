#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(6, 7);                // CE, CSN
const byte address[6] = "00001"; // Byte of array representing the address. This is the address where we will send the data. This should be same on the receiving side.
uint8_t SentMessage[1] = {0};
#define LED 2

void setup()
{
    pinMode(LED, OUTPUT);
    if (radio.begin(6, 7))
    { // Starting the Wireless communication
        Serial.println("success");
    }
    else
    {
        Serial.println("failed");
        digitalWrite(LED, HIGH);
    }
    radio.openWritingPipe(address); // Setting the address where we will send the data
    radio.setPALevel(RF24_PA_MIN);  // You can set it as minimum or maximum depending on the distance between the transmitter and receiver.
    radio.stopListening();          // This sets the module as transmitter
}
void loop()
{
    SentMessage[0]++;
    Serial.println(SentMessage[0]);
    digitalWrite(LED, SentMessage[0] % 2 == 0 ? HIGH : LOW);
    if (radio.write(SentMessage, sizeof(SentMessage)))
    { // Sending the message to receiver
        digitalWrite(LED, SentMessage[0] % 2 == 0 ? HIGH : LOW);
    }
    else
    {
        SentMessage[0]--;
    }
    delay(1000);
}