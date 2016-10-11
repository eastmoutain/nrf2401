#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>

void spi_begin(void);

void spi_begin_transaction(uint8_t border, uint8_t dmode, uint8_t clk);

void spi_end_transaction(void);

void spi_transfer(uint8_t data);

void spi_transfernb(char *tbuf, char *rbuf, uint32_t len);

void spi_transfern(char *buf, uint32_t len);

void spi_chip_select(int csn_pin);


#endif // _SPI_H_

