#include <pthread.h>
#include <stdio.h>
#include "spi.h"
#include "bcm2835.h"

static pthread_mutex_t spi_lock;

void spi_begin(void)
{
	if (!bcm2835_init()) {
		return;
	}

	bcm2835_spi_begin();

	// TODO: init mutex lock
}

void spi_begin_transaction(uint8_t border, uint8_t dmode, uint8_t clk)
{
	// TODO: Get lock
	bcm2835_spi_setBitOrder(border);
	bcm2835_spi_setDataMode(dmode);
	bcm2835_spi_setClockDivider(clk);
}

void spi_end_transaction(void)
{
	// TODO: Release lock
}

void spi_transfer(uint8_t data)
{
	uint8_t ret = bcm2835_spi_transfer(data);
	return ret;
}

void spi_transfernb(char *tbuf, char *rbuf, uint32_t len)
{
	bcm2835_spi_transfernb(tbuf, rbuf, len);
}

void spi_transfern(char *buf, uint32_t len)
{
	spi_transfernb(buf, buf, len);
}

void spi_chip_select(int csn_pin)
{
	bcm2835_spi_chipSelect(csn_pin);
	bcm2835_delayMicroseconds(5);
}


