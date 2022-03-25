#include "port.h"
#include <SPI.h>

static const struct dwt_spi_s dw3000_spi_fct = {
    .readfromspi = readfromspi,
    .writetospi = writetospi,
    .writetospiwithcrc = writetospiwithcrc,
    .setslowrate = port_set_dw_ic_spi_slowrate,
    .setfastrate = port_set_dw_ic_spi_fastrate
};

static const struct dwt_probe_s dw3000_probe_interf = 
{
    .dw = NULL,
    .spi = (void*)&dw3000_spi_fct,
    .wakeup_device_with_io = wakeup_device_with_io
};

void reset_DWIC(void)
{
  digitalWrite(DW3000_RESET_Pin, LOW);
  delay(2);
  digitalWrite(DW3000_RESET_Pin, HIGH);
  delay(2);
}

void Sleep(uint32_t ms)
{
  delay(ms);
}

void make_very_short_wakeup_io(void) {
    uint8_t cnt;

    SET_WAKEUP_PIN_IO_HIGH;
    for (cnt = 0; cnt < 10; cnt++) __NOP();
    SET_WAKEUP_PIN_IO_LOW;
}
void wakeup_device_with_io(void) {
    SET_WAKEUP_PIN_IO_HIGH;
    WAIT_200uSEC;
    SET_WAKEUP_PIN_IO_LOW;
}

void port_set_dw_ic_spi_slowrate(void){
    
}
void port_set_dw_ic_spi_fastrate(void){

}
int writetospiwithcrc(uint16_t headerLength, const uint8_t *headerBuffer,
                      uint16_t bodyLength, const uint8_t *bodyBuffer,
                      uint8_t crc8){

                      }
int writetospi(uint16_t headerLength, const uint8_t *headerBuffer,
               uint16_t bodyLength, const uint8_t *bodyBuffer){

               }
int readfromspi(uint16_t headerLength, uint8_t *headerBuffer,
                uint16_t readLength, uint8_t *readBuffer){
                    
                }