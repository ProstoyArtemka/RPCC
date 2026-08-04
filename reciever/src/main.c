#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/delay.h>

#include <stdlib.h>

#include <spi.h>
#include <nrf.h>



void init_nrf_interrupt(void) {

    DDRD &= ~(1 << NRF_IRQ);
    PORTD |= (1 << NRF_IRQ);

    PCMSK2 |= (1 << PCINT20); // Enablle NRF_IRQ (PCINT20 == PD4) in mask
    PCICR |= (1 << PCIE2); // Enable pin change interruption 2
}

void init_sleep_mode(void) {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    sleep_enable(); // Enable sleep mode
    sleep_bod_disable(); // Disable low power detection
    
    PCIFR |= (1 << PCIF2);

    sei(); // Enable interrupts
    sleep_cpu(); // Sleeping

    sleep_disable(); // Disable sleep after woke up

}

void init_leds(void) {

    DDRD |= (1 << PD5);
    PORTD &= ~(1 << PD5);

}

int main(void) {

    init_spi();
    init_nrf();
    init_nrf_interrupt();
    init_leds();

    set_ce_high();
    _delay_ms(5);

    nrf_flush_rx();

    while (1) {
        
        init_sleep_mode(); // Enter sleep state

        if (!(PIND & (1 << NRF_IRQ))) {
            uint8_t status = nrf_read_reg(NRF_STATUS_REGISTER);

            if (status & (1 << NRF_STATUS_RX_DATA_READY_POS)) {

                set_csn_low();
                spi_transfer(NRF_READ_RX_PAYLOAD);

                uint8_t buffer[NRF_CHANNEL_SIZE];
                for (uint8_t i = 0; i < NRF_CHANNEL_SIZE; i++) {
                    buffer[i] = spi_transfer(0); // Transfer nothing to read value
                }

                set_csn_high();

                PORTD |= (1 << PD5);
                _delay_ms(500);

                PORTD &= ~(1 << PD5);
                _delay_ms(500);
            }

            nrf_flush_rx();
            nrf_clear_irq();

            _delay_us(50);

        }

    }

    return 0;
}

ISR(PCINT2_vect) {


    PCIFR |= (1 << PCIF2);

}