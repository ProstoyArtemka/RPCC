#include <nrf.h>
#include <spi.h>

#include <avr/delay.h>

void set_ce_low() { PORTD &= ~(1 << NRF_CE); }
void set_ce_high() { PORTD |= (1 << NRF_CE); }



void nrf_write_reg(uint8_t reg, uint8_t value) {

    set_csn_low();
    _delay_us(20);

    spi_transfer(NRF_REGISTER_WRITE | reg);
    spi_transfer(value);

    set_csn_high();
    _delay_us(5);

}

uint8_t nrf_read_reg(uint8_t reg) {

    uint8_t value;

    set_csn_low();
    _delay_us(20);

    spi_transfer(NRF_REGISTER_READ | reg);
    value = spi_transfer(0x00); // Send nothing to get value

    set_csn_high();
    _delay_us(5);

    return value;
}

void nrf_clear_irq(void) {

    // Reset status register
    nrf_write_reg(
        NRF_STATUS_REGISTER, 
        (1 << NRF_STATUS_MAX_RT_POS) | (1 << NRF_STATUS_RX_DATA_READY_POS) | (1 << NRF_STATUS_TX_DATA_SENT_POS)
    );

}

void nrf_send_packet(uint8_t* data, uint8_t size) {

    set_ce_low(); // Disable CE, ensurring there will be no data while nrf flushes tx



    set_csn_low();
    spi_transfer(NRF_FLUSH_TX);
    set_csn_high();



    set_csn_low();

    spi_transfer(NRF_WRITE_TX_PAYLOAD);
    for (uint8_t i = 0; i < size; i++) {
        spi_transfer(data[i]);
    }

    set_csn_high();



    set_ce_high();
    _delay_us(15); // Enabling CE for 15us, transfers packet and then disable it to stop transferring.
    set_ce_low();
}

void nrf_flush_rx(void) {

    set_csn_low();

    spi_transfer(NRF_FLUSH_RX);

    set_csn_high();

}

void nrf_init(void) {

    DDRD |= (1 << NRF_CE);

    _delay_ms(100);

    nrf_write_reg(NRF_ENABLE_AUTO_ACK_REGISTER, 0x00);
    nrf_write_reg(NRF_EN_RXADDR_REGISTER, 0x01);
    nrf_write_reg(NRF_CHANNEL_REGISTER, NRF_CHANNEL);
    nrf_write_reg(NRF_RF_SETUP_REGISTER, 0x027);
    nrf_write_reg(NRF_RX_PW_P0_REGISTER, NRF_CHANNEL_SIZE);
    nrf_write_reg(NRF_FEATURE_REGISTER, 0x01);

    set_csn_low();
    spi_transfer(NRF_REGISTER_WRITE | NRF_RX_ADDR_P0_REGISTER);

    uint8_t addr[5] = {'R', 'P', 'C', 'C', '<'};
    for (uint8_t i = 0; i < 5; i++) spi_transfer(addr[i]);

    set_csn_high();

    nrf_write_reg(NRF_CONFIG_REGISTER, (1 << NRF_CONFIG_PWR_UP_POS) | (1 << NRF_CONFIG_PRIMARY_RX_POS) | (1 << NRF_CONFIG_EN_CRC_POS)); // Enable power and reciever mode

    _delay_ms(5);

} 