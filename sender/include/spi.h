#include <avr/io.h>

#define SPI_CS PB2 // CSN
#define SPI_SCK PB5
#define SPI_MISO PB4
#define SPI_MOSI PB3

void set_csn_low(void);
void set_csn_high(void);


void init_spi(void);

uint8_t spi_transfer(uint8_t data);