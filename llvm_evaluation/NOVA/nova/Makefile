#
# Makefile for the linux NOVA filesystem routines.
#
obj-m += squint_module.o
obj-m += nova.o

nova-y := balloc.o bbuild.o checksum.o dax.o dir.o file.o gc.o inode.o ioctl.o \
	journal.o log.o mprotect.o namei.o parity.o rebuild.o snapshot.o stats.o \
	super.o symlink.o sysfs.o perf.o print.o

KDIR = /data/storage/xieyuxi/clean_start/provisioning/roles/kernel_install/files/kernel/lib/modules/5.1.0+/build


CFLAGS_balloc.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_bbuild.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_checksum.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_dir.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_dax.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_gc.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_file.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_inode.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_ioctl.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_journal.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_log.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_mprotect.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_namei.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_parity.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_rebuild.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_snapshot.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_super.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_stats.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_symlink.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_sysfs.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so
CFLAGS_perf.o += -O1 -fexperimental-new-pass-manager -fpass-plugin=/home/xieyuxi/shared/llvm-instrumentation/build/fencePass/LLVMFENCE.so

kbuild:
	make -C $(KDIR) M=`pwd` modules

clean:
	make -C $(KDIR) M=`pwd` clean
