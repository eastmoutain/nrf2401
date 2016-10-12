
#include <stdint.h>
#include <unitstd.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "bcm2835.h"
#include "spi.h"


#define nrf24_max(a,b) (a>b?a:b)
#define nrf24_min(a,b) (a<b?a:b)


#define digital_write(pin, value) bcm2835_gpio_write(pin, value)
#define pin_mode(pin,value) bcm2835_gpio_fsel(pin,value);
#define OUTPUT BCM2835_GPIO_FSEL_OUTP

#define BV(n) (1 << n)

struct nrf24 rf24;

static void nrf24_begin_transaction(struct nrf24 *nrf24);
static void nrf24_end_transaction(struct nrf24 *nrf24);
static uint8_t nrf24_flush_rx(struct nrf24 *nrf24);
static uint8_t nrf24_flush_tx(struct nrf24 *nrf24);



void set_csn_pin_level(uint8_t pin, bool level)
{
	digital_write(pin, level);
	//delayMicroseconds(5);
	usleep(5);
}

void set_ce_pin_level(uint8_t pin, bool level)
{
	digital_write(pin, level);
}

static uint8_t nrf24_flush_rx(struct nrf24 *nrf24)
{
    uint8_t status;

    nrf24_begin_transaction(nrf24);
    status = spi_transfer(FLUSH_RX);
    nrf24_end_transaction(nrf24);

    return status;
}

static uint8_t nrf24_flush_tx(struct nrf24 *nrf24)
{
    uint8_t status;

    nrf24_begin_transaction(nrf24);

    status = spi_transfer(FLUSH_TX);

    nrf24_end_transaction(nrf24);

    return status;
}


static void nrf24_begin_transaction(struct nrf24 *nrf24)
{
	struct spi *spi = &nrf24->spi;
	
	spi_begin_transaction(spi->border, spi->bmode, spi->clk);

	set_csn_pin_level(nrf24->csn_pin, LOW);
}

static void nrf24_end_transaction(struct nrf24 *nrf24)
{
	set_csn_pin_level(nrf24->csn_pin, HIGH);
}

uint8_t nrf24_read_register(struct nrf24 *nrf24, uint8_t reg)
{
	uint8_t status;

	nrf24_begin_transaction(nrf24);

	status = spi_transfer(R_REGISTER | ( REGISTER_MASK & reg ));

	nrf24_end_transaction(nrf24);
	
	return status;
}

uint8_t nrf24_write_register(struct nrf24 *nrf24, uint8_t reg, uint8_t val)
{
	uint8_t status;
	
#if (DEBUG == ENABLE)
	printf("write reg [0x%x] with value [0x%x]\r\n", reg, val);
#endif

	nrf24_begin_transaction(nrf24);

	status = spi_transfer(W_REGISTER | ( REGISTER_MASK & reg );
	
	nrf24_end_transaction(nrf24);

	return status;
}


uint8_t nrf24_wirte_payload(struct nrf24 *nrf24, const void *buf, uint8_t len, uint8_t type)
{
    uint8_t status;
    uint8_t *p = buf;

    len = nrf24_min(len, nrf24->payload_size);
    uint8_t blank_len = nnnrf24->payload_size - len;
    
	nrf24_begin_transaction(rf24);

    status = spi_transfer(type);

    while (len--) {
        spi_transfer(*p++);
    }

    while (blank_len--) {
        spi_transfer(0);
    }

    nrf24_end_transaction(rf24);
}

uint8_t nrf24_read_payload(struct nrf24 *nrf24, const uint8_t buf, uint8_t len)
{
    uint8_t status;
    uint8_t *p = buf;

    len = nrf24_min(len, nrf24->payload_size);

    nrf24_begin_transaction(nrf24);

    status = spi_transfer(R_RX_PAYLOAD);
    while (len--) {
        *p++ = spi_transfer(0xff);
    }
    
    nrf24_end_transaction(nrf24);

    return status;
}


void nrf24_set_channel(struct nrf24 *nrf24, uint8_t channel)
{
    const uint8_t max_channel = 125;

    nrf24_write_register(nrf24, RF_CH, nrf24_min(channel, max_channel));
}

void nrf24_get_channel(struct nrf24 *nrf24)
{
    return nrf24_read_register(nrf24, RF_CH);
}

void nrf24_set_payload_size(struct nrf24 *nrf24, uint8_t size)
{
    nrf24->payload_size = size;
}

uint8_t nrf24_get_payload_size(struct nrf24 *nrf24)
{
    return nrf24->payload_size;
}

void nrf24_disable_crc(struct nrf24 *nrf24)
{
    uint8_t disable = nrf24_read_register(nrf24, NRF_CONFIG) & ~BV(EN_CRC);
    nrf24_write_register(nrf24, NRF_CONFIG, disable);
}

void nrf24_set_retries(struct nrf24 *nrf24,uint8_t delay, uint8_t count)
{
    nrf24_write_register(nrf24, SETUP_RETR, (delay&0xf)<<ARD | (count&0xf)<<ARC)
}

bool nrf24_set_data_rate(struct nrf24 *nrf24, rf24_datarate_e speed)
{
    bool result = false;
    uint8_t setup = nrf24_read_register(nrf24, RF_SETUP);

    setup &= ~(BV(RF_DR_LOW) | BV(RF_DR_HIGH));

    switch(speed) {
        default:
        case RF24_250KBPS: 
            {
            setup |= BV(RF_DR_LOW);
            nrf24->tx_rx_delay = 450;
            break;
        }
        case RF24_1MBPS:
        case RF24_2MBPS: {
            setup |= BV(RF_DR_HIGH);
            nrf24->tx_rx_delay = 190;
            break;
        }
    }

    nrf24_write_register(nrf24, RF_SETUP, setup);

    if (nrf24_read_register(nrf24, RF_SETUP) == setup) {
        result = true;
    }

    return result;
    
}

void nrf24_power_up(struct nrf24 *nrf24)
{
    uint8_t cfg = nrf24_read_register(nrf24, NRF_CONFIG);

    nrf24_write_register(nrf24, NRF_CONFIG, cfg | BV(PWR_UP));
    delay(5);
}

uint8_t nrf24_init(const uint8_t ce_pin, const uint8_t csn_pin)
{
    struct nrf24 *nrf = &nrf24;
    static const uint8_t rx_addr[] = {0x00, 0x01, 0x02, 0x03, 0x04};
    static const uint8_t tx_addr[] = {0x00, 0x01, 0x02, 0x03, 0x04}

    memset(nrf, 0, sizeof(struct nrf24));
    nrf->ce_pin = ce_pin;
    nrf->csn_pin = csn_pin;
    nrf->rx1_addr = rx_addr;
    nrf->tx1_addr = tx_addr;

	nrf->spi.clk = BCM2835_SPI_SPEED_8MHZ;
	nrf->spi.border = MSBFIRST;
	nrf->spi.bmode = SPI_MODE0;

    spi_begin();
    pin_mode(nrf->ce_pin,OUTPUT);
    set_csn_pin_level(nrf->ce_pin, LOW);
    delay(100);
    
    // reset NRF_CONFIG and enable 16 bit CRC
    nrf24_write_register(nrf24, NRF_CONFIG, 0b00001100);
	

    // set retries
    nrf24_set_retries(5, 15);

    if (false == nrf24_set_data_rate(nrf24,RF24_250KBPS))
    {
        // set data rate fail!!!
    }

    nrf24_set_data_rate(nrf24, RF24_1MBPS);
    // TODO: ommit toggle feature compared with RF24
    nrf24_write_register(nrf24, FEATURE, 0);
    nrf24_write_register(nrf24, DYNPD, 0);
    // Reset current status
    uint8_t status = BV(RX_DR) | BV(TX_DS) | BV(MAX_RT); 
    nrf24_write_register(nrf24, NRF_STATUS, status);

    // TODO: why it is 76
    nrf24_set_channel(76);

    nrf24_flush_rx();
    nrf24_flush_tx();

    nrf24_power_up();

    nrf24_write_register(nrf24, NRF_CONFIG, nrf24_read_register(nrf24,NRF_CONFIG) & ~BV(PRIM_RX));

    return (setup != 0 && setup != 0xff)
    
}


