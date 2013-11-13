#ifndef BBSPI_H
#define BBSPI_H

class BbspiClass {
  private:
    // Use PIO pins to implement a "bitbang" SPI port:
    int bb_clk;     // clock pin
    int bb_mosi;    // master out, slave in
    int bb_miso;    // master in, slave out
  
  public:
    // methods
    //! class constructor
    BbspiClass();
    
    //General transfer function
    int setup();
    int setup(uint8_t my_clk, uint8_t my_mosi, uint8_t my_miso);
    uint8_t transfer(uint8_t data);
};

extern BbspiClass spisw;

#endif
