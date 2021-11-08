obj-m += lkm_example.o
KDIR = /lib/modules/$(shell uname -r)/build

kbuild:
	make -C $(KDIR) M=`pwd` modules

test:
	sudo dmesg -C
	sudo insmod lkm_example.ko
	sudo rmmod lkm_example.ko
	dmesg

clean:
	make -C $(KDIR) M=`pwd` clean
	sudo rmmod lkm_example.ko

