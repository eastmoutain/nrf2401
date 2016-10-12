

CC = gcc

BCM = bcm
NRF2401 = nrf2401
SPI = spi

obj = $(patsubst %.c, %.o, $(wildcard *.c))
obj += $(patsubst %.c, %.o, $(wildcard $(BCM)/*.c))
obj += $(patsubst %.c, %.o, $(wildcard $(NRF2401)/*.c))
obj += $(patsubst %.c, %.o, $(wildcard $(SPI)/*.c))

main: $(obj)
	@echo CC $@
	@$(CC) - o $@ $^

%.o:%.c
	@echo CC $@
	@$(CC) -c $< -o $@

.PHONY: clean debug

clean:
	@rm -rf $(obj) main


