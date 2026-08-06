#include <avr/io.h>

#define BAUD 9600
#define MYUBRR ((F_CPU / (BAUD * 16UL)) - 1)

void init_uart(void);

void uart_transmit(uint8_t data);
void uart_print(const char* text);
void uart_print_hex(uint8_t data);