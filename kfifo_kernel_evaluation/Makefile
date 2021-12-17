obj-m += squint_module.o
obj-m += mockfs.o
KDIR = /lib/modules/$(shell uname -r)/build

kbuild:
	make -C $(KDIR) M=`pwd` modules

test:
	sudo dmesg -C
	sudo insmod squint_module.ko
	sudo insmod mockfs.ko
	cat /proc/MockFStrace
	cat /proc/squint
	sudo rmmod mockfs.ko
	sudo rmmod squint_module.ko
	dmesg

setup:
	sudo insmod squint_module.ko
	sudo insmod mockfs.ko

rmmods:
	sudo rmmod mockfs.ko
	sudo rmmod squint_module.ko

reset: rmmods setup

benchmark:
	g++ -std=c++11 -pthread benchmark.cpp

all: kbuild benchmark

clean:
	make -C $(KDIR) M=`pwd` clean