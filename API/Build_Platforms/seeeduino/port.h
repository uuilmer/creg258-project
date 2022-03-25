#include <Arduino.h>

typedef byte uint8_t;
typedef word uint16_t;
typedef unsigned long uint32_t;
#define NULL 0
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define DW3000_IRQn_Pin 0
#define DW3000_RESET_Pin 1
#define DW3000_WAKEUP_Pin 2

// SPI defs
#define DW3000_CS_Pin 12
#define DW3000_CLK_Pin 8 // DWM3000 shield SPIM1 sck connected to DW3000
#define DW3000_MOSI_Pin 10  // DWM3000 shield SPIM1 mosi connected to DW3000
#define DW3000_MISO_Pin 9  // DWM3000 shield SPIM1 miso connected to DW3000

#define SET_WAKEUP_PIN_IO_LOW digitalWrite(DW3000_WAKEUP_Pin, LOW)
#define SET_WAKEUP_PIN_IO_HIGH digitalWrite(DW3000_WAKEUP_Pin, HIGH)

#define WAIT_500uSEC delay(500)
#define WAIT_200uSEC delay(200)

void reset_DWIC(void);
void Sleep(uint32_t);
void make_very_short_wakeup_io(void);
void wakeup_device_with_io(void);
void port_set_dw_ic_spi_slowrate(void);
void port_set_dw_ic_spi_fastrate(void);
int writetospiwithcrc(uint16_t headerLength, const uint8_t *headerBuffer,
                      uint16_t bodyLength, const uint8_t *bodyBuffer,
                      uint8_t crc8);
int writetospi(uint16_t headerLength, const uint8_t *headerBuffer,
               uint16_t bodyLength, const uint8_t *bodyBuffer);
int readfromspi(uint16_t headerLength, uint8_t *headerBuffer,
                uint16_t readLength, uint8_t *readBuffer);

extern const struct dwt_probe_s dwt_probe;