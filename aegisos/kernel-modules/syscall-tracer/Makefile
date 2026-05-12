obj-m += syscall_tracer.o

KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

load:
	sudo insmod syscall_tracer.ko

unload:
	sudo rmmod syscall_tracer

watch:
	sudo dmesg -w | grep aegis_trace
