obj-m += lsm_stub.o

KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

# Load only on a development VM — see source comments
load:
	sudo insmod lsm_stub.ko

unload:
	sudo rmmod lsm_stub
