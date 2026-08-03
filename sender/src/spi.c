#include <spi.h>

void set_csn_low(void) { PORTB &= ~(1 << SPI_SS); }
void set_csn_high(void) { PORTB |= (1 << SPI_SS); }



void init_spi(void) {

    DDRB |= (1 << PB5) | (1 << PB3) | (1 << SPI_SS); // Enable SCK, MOSI and Chip Select for SPI 
    PORTB |= (1 << SPI_SS); // High on chip select, by default device is disabled

    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0); // Enabling SPI, Master mode and MHz freq for SPI. (1/16 of ATmega328P)

}

uint8_t spi_transfer(uint8_t data) {
    SPDR = data; // Send data.

    while (!(SPSR & (1 << SPIF))); // Wait until SPIF in SPSR becomes 1.

    return SPDR; // Get recieved data.
}
