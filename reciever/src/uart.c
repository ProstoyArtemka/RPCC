#include <uart.h>
#include <avr/io.h>

void uart_init(void) {

    UBRR0H = (unsigned char) (MYUBRR >> 8);
    UBRR0L = (unsigned char) (MYUBRR);

    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

}

void uart_transmit(uint8_t data) {

    while (!(UCSR0A & (1 << UDRE0)));

    UDR0 = data;
}

uint8_t uart_recieve(void) {

    while (!(UCSR0A & (1 << RXC0)));
    
    return UDR0;
}

void uart_print(const char *str) {

    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void uart_print_hex(uint8_t num) {

    uart_transmit(num);

}