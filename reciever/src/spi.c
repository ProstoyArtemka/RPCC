#include <spi.h>
#include <avr/delay.h>

void set_csn_low(void) { 
    PORTD &= ~(1 << SPI_CS);
    
    _delay_us(2);
}

void set_csn_high(void) { 
    PORTD |= (1 << SPI_CS); 
    
    _delay_us(2);
}


void spi_init(void) {

    DDRB |= (1 << PB2); // SS for master-mode

    DDRD |= (1 << SPI_CS); // SPI CS
    PORTD |= (1 << SPI_CS); // Enable CS

    DDRB |= (1 << SPI_SCK) | (1 << SPI_MOSI); // Set outputs SCK, MOSI and Chip Select for SPI 

    DDRB &= ~(1 << SPI_MISO); // Set MISO as Input
    PORTB &= ~(1 << SPI_MISO); // No pull-up resistor for MISO

    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
    SPCR &= ~(1 << SPR1); 
    SPSR &= ~(1 << SPI2X);

    SPCR &= ~((1 << CPOL) | (1 << CPHA)); // Spi mode 0
}

uint8_t spi_transfer(uint8_t data) {
    SPDR = data; // Send data.

    while (!(SPSR & (1 << SPIF))); // Wait until SPIF in SPSR becomes 1.

    return SPDR; // Get recieved data.
}
