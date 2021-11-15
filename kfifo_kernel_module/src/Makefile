obj-m += squint_module.o
obj-m += test_module.o
KDIR = /lib/modules/$(shell uname -r)/build

kbuild:
	make -C $(KDIR) M=`pwd` modules

test:
	sudo dmesg -C
	sudo insmod squint_module.ko
	sudo insmod test_module.ko
	cat /proc/squint
	sudo rmmod test_module.ko
	sudo rmmod squint_module.ko
	dmesg

clean:
	make -C $(KDIR) M=`pwd` clean

