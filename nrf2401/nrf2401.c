
#include <stdint.h>
#include <unitstd.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "bcm2835.h"
#include "spi.h"


#define digitalWrite(pin, value) bcm2835_gpio_write(pin, value)
#define pinMode(pin,value) bcm2835_gpio_fsel(pin,value);
#define OUTPUT BCM2835_GPIO_FSEL_OUTP

struct nrf24 {
	uint8_t ce_pin;
	uint8_t csn_pin;
	uint16_t spi_speed;
	uint8_t spi_rxbuf[32 + 1];
	uint8_t spi_tx_buf[32 + 1];
	uint8_t addr_width;
	uint8_t rx1_addr[5];
	uint8_t tx1_addr[5];
};


void set_csn_pin_level(uint8_t pin, bool level)
{
	digitalWrite(pin, level);
	//delayMicroseconds(5);
	usleep(5);
}

void set_ce_pin_level(uint8_t pin, bool level)
{
	digitalWrite(pin, level);
}

void nrf24_begin_transaction()
{
	spi_begin_transaction();

	set_csn_pin_level(csn, LOW);
}

void nrf24_end_transaction(void)
{
	set_csn_pin_level(csn_pin, HIGH);
}

uint8_t nrf24_read_register(uint8_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t status;

	nrf24_begin_transaction();

	status = spi_transfer(R_REGISTER | ( REGISTER_MASK & reg ));

	nrf24_end_transaction();
	
	return status;
}

uint8_t nrf24_write_register(uint8_t reg, uint8_t val)
{
	uint8_t status;
	
#if (DEBUG == ENABLE)
	printf("write reg [0x%x] with value [0x%x]\r\n", reg, val);
#endif

	nrf24_begin_transaction();

	status = spi_transfer(W_REGISTER | ( REGISTER_MASK & reg );
	
	nrf24_end_transaction();

	return status;
}


