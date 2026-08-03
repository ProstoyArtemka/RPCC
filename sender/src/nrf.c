#include <nrf.h>
#include <spi.h>

#include <avr/delay.h>

void set_ce_low() { PORTB &= ~(1 << NRF_CE); }
void set_ce_high() { PORTB |= (1 << NRF_CE); }



void nrf_write_reg(uint8_t reg, uint8_t value) {

    set_csn_low();

    spi_transfer(NRF_REGISTER_WRITE | reg);
    spi_transfer(value);

    set_csn_high();

}

uint8_t nrf_read_reg(uint8_t reg) {

    uint8_t value;

    set_csn_low();

    spi_transfer(NRF_REGISTER_READ | reg);
    value = spi_transfer(0x00); // Send nothing to get value

    set_csn_high();

}

void nrf_clear_irq(void) {

    // Reset status register
    nrf_write_reg(
        NRF_STATUS_REGISTER, 
        (1 << NRF_STATUS_MAX_RT_POS) | (1 << NRF_STATUS_RX_DATA_READY_POS) | (1 << NRF_STATUS_TX_DATA_SENT_POS)
    );

}

void init_nrf(void) {

    _delay_ms(100);

    nrf_write_reg(NRF_ENABLE_AUTO_ACK_REGISTER, 0x00);
    nrf_write_reg(NRF_CHANNEL_REGISTER, NRF_CHANNEL);
    nrf_write_reg(NRF_RF_SETUP_REGISTER, 0x06);
    nrf_write_reg(NRF_RX_PW_P0, NRF_CHANNEL_SIZE);

    nrf_write_reg(NRF_CONFIG_REGISTER, (1 << NRF_CONFIG_PWR_UP_POS)); // Enable power and recieve mode

    _delay_ms(5);

} 