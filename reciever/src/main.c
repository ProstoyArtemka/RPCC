#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/delay.h>
#include "string.h"

#include <stdlib.h>

#include <spi.h>
#include <nrf.h>
#include <uart.h>

uint8_t connected_to_driver = 0;

void nrf_init_interrupt(void) {

    DDRD &= ~(1 << NRF_IRQ); // Enable input on IRQ
    PORTD |= (1 << NRF_IRQ); // Enable pull-up on IRQ

    EICRA &= ~((1 << ISC01) | (1 << ISC00));
    EIMSK |= (1 << INT0);

    sei();

    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void leds_init(void) {

    DDRD |= (1 << PD5);
    PORTD &= ~(1 << PD5);

}

void wait_till_driver_connects() {

    if (connected_to_driver) return;

    static char handshsake_message[] = "rpcc_handshake";
    uint8_t handshake_length = sizeof(handshsake_message) - 1;

    char recieved_buffer[15];
    uint8_t recieved_bytes = 0;

    while (recieved_bytes < handshake_length) recieved_buffer[recieved_bytes++] = uart_recieve();
    recieved_buffer[recieved_bytes] = '\0';

    if (strcmp(handshsake_message, recieved_buffer) != 0) return;

    for (uint8_t i = 0; i < handshake_length; i++) uart_transmit(handshsake_message[i]);

    PORTD |= (1 << PD5);
    _delay_ms(1000);
    PORTD &= ~(1 << PD5);

    connected_to_driver = 1;
}

int main(void) {

    uart_init();
    spi_init();
    nrf_init();
    nrf_init_interrupt();
    leds_init();

    nrf_flush_rx();

    set_ce_high();
    _delay_ms(5);

    uint8_t status = nrf_read_reg(NRF_STATUS_REGISTER);
    if (status == 0x27) uart_print("SPI is ok!");

    while (1) {
        
        // Wait till driver recognizes device

        wait_till_driver_connects();

        // Now we can sleep

        sleep_enable(); // Enable sleep mode
        sleep_bod_disable(); // Disable low power detection
        sleep_cpu(); // Sleeping

        sleep_disable();

        EIMSK &= ~(1 << INT0); // Disable interrupt while reading registers

        uint8_t status = nrf_read_reg(NRF_STATUS_REGISTER);

        if (status & (1 << NRF_STATUS_RX_DATA_READY_POS)) {

            set_csn_low();
            spi_transfer(NRF_READ_RX_PAYLOAD);

            uint8_t buffer[NRF_CHANNEL_SIZE];
            for (uint8_t i = 0; i < NRF_CHANNEL_SIZE; i++) {
                buffer[i] = spi_transfer(0); // Transfer nothing to read value
            }

            set_csn_high();
        }

        nrf_flush_rx();
        nrf_clear_irq();

        EIMSK |= (1 << INT0); // Enable interrupt after IRQ was cleared.

        _delay_us(50);

    }

    return 0;
}

ISR(INT0_vect) {}