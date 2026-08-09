#include <avr/io.h>

#define BAUD 9600
#define MYUBRR ((F_CPU / (BAUD * 16UL)) - 1)

void uart_init(void);

void uart_transmit(uint8_t data);
uint8_t uart_recieve(void);
void uart_print(const char* text);
void uart_print_hex(uint8_t data);