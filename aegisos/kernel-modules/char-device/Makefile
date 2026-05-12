obj-m += char_device.o

KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

load:
	sudo insmod char_device.ko

unload:
	sudo rmmod char_device

test:
	@echo "Testing aegis_char device..."
	@echo "hello aegis" | sudo tee /dev/aegis_char
	@sudo cat /dev/aegis_char
