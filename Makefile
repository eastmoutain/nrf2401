

CC = gcc

ROOT = .
BCM = bcm
NRF2401 = nrf2401
SPI = spi
COMMON = common

CFLAGS += -Wall -I$(ROOT) -I$(BCM) -I$(NRF2401) -I$(SPI) -I$(COMMON)

LFLAGS += -lpthread

obj = $(patsubst %.c, %.o, $(wildcard *.c))
obj += $(patsubst %.c, %.o, $(wildcard $(BCM)/*.c))
obj += $(patsubst %.c, %.o, $(wildcard $(NRF2401)/*.c))
obj += $(patsubst %.c, %.o, $(wildcard $(SPI)/*.c))
obj += $(patsubst %.c, %.o, $(wildcard $(COMMON)/*.c))

main: $(obj)
	@echo =============================================
	@echo  generating binary file...
	@echo [CC] $@
	@$(CC) $(LFLAGS) -o $@ $^

%.o:%.c
	@echo [CC] $@
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean debug

clean:
	@rm -rf $(obj) main


