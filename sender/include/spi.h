#include <avr/io.h>

#define SPI_SS PB2 // Chip Select of SPI


void set_csn_low(void);
void set_csn_high(void);


void init_spi(void);

uint8_t spi_transfer(uint8_t data);