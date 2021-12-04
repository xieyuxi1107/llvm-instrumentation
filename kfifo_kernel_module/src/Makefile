obj-m += squint_module.o

TARGET = my_test_instrument
obj-m += $(TARGET).o
CFLAGS_test_instrument.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so

$(TARGET)-objs := print.o test_instrument.o 
KDIR = /data/storage/xieyuxi/provisioning/roles/kernel_install/files/kernel/lib/modules/5.15.0-rc6+/build

kbuild:
	make -C $(KDIR) M=`pwd` modules

test:
	sudo dmesg -C
	sudo insmod squint_module.ko
	sudo insmod my_test_instrument.ko
	cat /proc/squint
	sudo rmmod my_test_instrument.ko
	sudo rmmod squint_module.ko
	dmesg

clean:
	make -C $(KDIR) M=`pwd` clean


