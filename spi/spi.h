#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>

#define SPI_HAS_TRANSACTION
#define MSBFIRST BCM2835_SPI_BIT_ORDER_MSBFIRST
#define SPI_MODE0 BCM2835_SPI_MODE0
#define RF24_SPI_SPEED BCM2835_SPI_SPEED_8MHZ


struct spi {
	uint8_t border;
	uint8_t bmode;
	uint8_t clk;
};

void spi_begin(void);

void spi_begin_transaction(uint8_t border, uint8_t dmode, uint8_t clk);

void spi_end_transaction(void);

uint8_t spi_transfer(uint8_t data);

void spi_transfernb(char *tbuf, char *rbuf, uint32_t len);

void spi_transfern(char *buf, uint32_t len);

void spi_chip_select(int csn_pin);


#endif // _SPI_H_

