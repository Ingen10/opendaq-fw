#ifndef BBSPI_H
#define BBSPI_H

class BbspiClass {
  private:
    // Use PIO pins to implement a "bitbang" SPI port:
    int bb_clk;     // clock pin
    int bb_mosi;    // master out, slave in
    int bb_miso;    // master in, slave out
    
    int bb_cpol;    // clock polarity: clk value when inactive
    int bb_cpha;    // clock phase: data valid on clock leading (0) or trailing (1) edges
  
  public:
    // methods
    //! class constructor
    BbspiClass();
    
    //General transfer function
    int setup();
    int setup(uint8_t my_clk, uint8_t my_mosi, uint8_t my_miso);
    int configure(uint8_t my_cpol, uint8_t my_cpha);
    uint8_t transfer(uint8_t data);
};

extern BbspiClass spisw;

#endif
