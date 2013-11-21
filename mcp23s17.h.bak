#ifndef MCP23S17_H
#define MCP23S17_H

#include <pins_arduino.h>

// Register addresses
#define IODIRA 0x00
#define IODIRB 0x01
#define IODIR  IODIRA
#define GPPUA  0x0C
#define GPPUB  0x0D
#define GPPU   GPPUA
#define GPIOA  0x12
#define GPIOB  0x13
#define GPIO   GPIOA

// Use PIO pins to implement a "bitbang" SPI port:
#define SPI_CLK   PIO1  // clock pin
#define SPI_MOSI  PIO2  // master out, slave in

void mcp23s17_write(int ss_pin, uint8_t addr, uint16_t data);

#endif
