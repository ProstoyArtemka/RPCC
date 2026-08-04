#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/delay.h>

#include <stdlib.h>

#include <spi.h>
#include <nrf.h>
#include <buttons.h>
#include <uart.h>

void init_buttons_pcint(void) {

    DDRD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4)); // Inputs PD2, PD3, PD4 for buttons
    PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD4); // Pull-up for this ports

    PCICR |= (1 << PCIE2); // Enable interrupts for PCIE2 (Port D)

    PCMSK2 |= (1 << PCINT18) | (1 << PCINT19) | (1 << PCINT20); // Enable this ports in PCINT mask

}

void init_sleep_mode(void) {

    cli(); // Disable interrupts

    sleep_enable(); // Enable sleep mode
    sleep_bod_disable(); // Disable low power detection
    
    sei(); // Enable interrupts
    
    sleep_cpu(); // Sleeping
    sleep_disable(); // Disable sleep after woke up

}

void init_leds(void) {

    DDRD |= (1 << PD5);
    PORTD &= ~(1 << PD5);

}

int main(void) {

    init_buttons_pcint();
    
    init_uart();
    init_spi();
    init_nrf();

    init_leds();

    set_ce_low();

    while (1) {

        init_sleep_mode(); // Enter sleep state

        if (get_buttons_flags() == 0) continue;

        if (is_button_pressed(0)) {

            uint8_t data[NRF_CHANNEL_SIZE];
            for (uint8_t i = 0; i < NRF_CHANNEL_SIZE; i++) data[i] = 0x55;

            nrf_send_packet(data, NRF_CHANNEL_SIZE);

            free(data);

            PORTD |= (1 << PD5);

            _delay_ms(500);

            PORTD &= ~(1 << PD5);
            
        };

        nrf_clear_irq();

        _delay_ms(100);
    
    }

    return 0;
}

ISR(PCINT2_vect) {
    // Will be called when one of button triggers

    update_buttons_flags();

}
