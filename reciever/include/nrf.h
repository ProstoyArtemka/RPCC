#include <avr/io.h>

#define NRF_CE PD3 // Chip Enable for NRF
#define NRF_IRQ PD4
#define NRF_CHANNEL 80
#define NRF_CHANNEL_SIZE 8

#define NRF_REGISTER_READ               0b00000000
#define NRF_REGISTER_WRITE              0b00100000
#define NRF_READ_RX_PAYLOAD             0b01100001
#define NRF_WRITE_TX_PAYLOAD            0b10100000
#define NRF_FLUSH_TX                    0b11100001
#define NRF_FLUSH_RX                    0b11100010
#define NRF_ACTIVATE                    0b01010000
#define NRF_NOP                         0b11111111

#define NRF_CONFIG_REGISTER             0x00
#define NRF_ENABLE_AUTO_ACK_REGISTER    0x01
#define NRF_EN_RXADDR_REGISTER          0x02
#define NRF_SETUP_RETR_REGISTER         0x04
#define NRF_CHANNEL_REGISTER            0x05
#define NRF_RF_SETUP_REGISTER           0x06
#define NRF_STATUS_REGISTER             0x07
#define NRF_TX_ADDR_REGISTER            0x10
#define NRF_RX_ADDR_P0_REGISTER         0x0A
#define NRF_RX_PW_P0_REGISTER           0x11
#define NRF_FEATURE_REGISTER            0x1D


#define NRF_CONFIG_PRIMARY_RX_POS       0x0
#define NRF_CONFIG_PWR_UP_POS           0x1
#define NRF_CONFIG_EN_CRC_POS           0x3

#define NRF_STATUS_RX_DATA_READY_POS    0x06
#define NRF_STATUS_TX_DATA_SENT_POS     0x05
#define NRF_STATUS_MAX_RT_POS           0x04

void set_ce_low(void);
void set_ce_high(void);

void nrf_write_reg(uint8_t reg, uint8_t value);
uint8_t nrf_read_reg(uint8_t reg);
void nrf_send_packet(uint8_t* data, uint8_t size);
void nrf_clear_irq(void);
void nrf_flush_rx(void);

void init_nrf(void);