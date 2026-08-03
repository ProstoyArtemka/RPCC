#include <avr/io.h>

#define NRF_CE PB1 // Chip Enable for NRF
#define NRF_CHANNEL 34
#define NRF_CHANNEL_SIZE 5

#define NRF_REGISTER_READ               0x00000000
#define NRF_REGISTER_WRITE              0x00100000
#define NRF_READ_RX_PAYLOAD             0x10100000
#define NRF_WRITE_TX_PAYLOAD            0x10100000
#define NRF_FLUSH_TX                    0x11100001
#define NRF_FLUSH_RX                    0x11100010
#define NRF_ACTIVATE                    0x01010000
#define NRF_NOP                         0x11111111

#define NRF_CONFIG_REGISTER             0x00
#define NRF_ENABLE_AUTO_ACK_REGISTER    0x01
#define NRF_SETUP_RETR_REGISTER         0x04
#define NRF_CHANNEL_REGISTER            0x05
#define NRF_RF_SETUP_REGISTER           0x06
#define NRF_STATUS_REGISTER             0x07
#define NRF_TX_ADDR_REGISTER            0x10
#define NRF_RX_ADDR_P0                  0x0A
#define NRF_RX_PW_P0                    0x11

#define NRF_CONFIG_PRIMARY_RX_POS       0x00
#define NRF_CONFIG_PWR_UP_POS           0x10

#define NRF_STATUS_RX_DATA_READY_POS    0x06
#define NRF_STATUS_TX_DATA_SENT_POS     0x05
#define NRF_STATUS_MAX_RT_POS           0x04

void set_ce_low(void);
void set_ce_high(void);

void nrf_write_reg(uint8_t reg, uint8_t value);
void nrf_clear_irq(void);

void init_nrf(void);