#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(6, 7); // CE, CSN
const byte address[6] = "00001";
uint8_t ReceivedMessage[1] = {0};
#define LED 2

void setup()
{
    pinMode(LED, OUTPUT);
    Serial.begin(9600);
    radio.begin(6, 7);
    radio.openReadingPipe(0, address); // Setting the address at which we will receive the data
    radio.setPALevel(RF24_PA_MIN);     // You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
    radio.startListening();            // This sets the module as receiver
}
void loop()
{
    if (radio.available())
    {
        radio.read(ReceivedMessage, sizeof(ReceivedMessage)); // Reading the data
        Serial.println(ReceivedMessage[0]);
        digitalWrite(LED, ReceivedMessage[0] % 2 == 0 ? HIGH : LOW);
    }
    delay(10);
}