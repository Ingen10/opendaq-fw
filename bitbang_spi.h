#ifndef BITBANG_SPI_H
#define BITBANG_SPI_H

#include <inttypes.h>
typedef uint8_t byte;

#define IODIRA 0x00
#define IODIRB 0x01
#define IODIR  IODIRA
#define GPPUA  0x0C
#define GPPUB  0x0D
#define GPPU   GPPUA
#define GPIOA  0x12
#define GPIOB  0x13
#define GPIO   GPIOA

#define SPI_SS    0 // set slave select pin
#define SPI_CLK   2 // set clock pin
#define SPI_MOUT  3 // set master out, slave in pin


void mcp23s17_write(byte addr, uint16_t data);

#endif
