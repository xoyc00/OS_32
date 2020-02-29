DEFAULT_HOST!=../../../default-host.sh
HOST?=DEFAULT_HOST
HOSTARCH!=../../../target-triplet-to-arch.sh $(HOST)

CFLAGS?=-O2 -g
CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include

CFLAGS:=$(CFLAGS) -ffreestanding -Wall -Wextra -mno-red-zone -mcmodel=kernel -z max-page-size=0x1000
CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -D__is_x86_64 -I../../include
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS) -nostdlib -lk -lgcc

ARCHDIR=./
DRIVERDIR=../../driver/
SYSTEMDIR=../../system/
INTERPDIR=../../interp/

include make.config
include ../../interp/make.config
include ../../driver/make.config
include ../../system/make.config

KERNEL_OBJS=\
$(KERNEL_ARCH_OBJS) \
$(KERNEL_DRIVER_OBJS) \
$(KERNEL_SYSTEM_OBJS) \
$(KERNEL_INTERP_OBJS) \
../../kernel/kernel.o \
../../kernel/shell.o \
../../kernel/memory_mapper.o \
../../kernel/liballoc.o \

OBJS=\
crti.o \
crtbegin.o \
$(KERNEL_OBJS) \
crtend.o \
crtn.o \

LINK_LIST=\
$(LDFLAGS) \
crti.o \
crtbegin.o \
$(KERNEL_OBJS) \
$(LIBS) \
crtend.o \
crtn.o \
	

.phony: all install install-headers install-kernel
.SUFFIXES: .o .c .S

all: os_64.ker

os_64.ker: $(OBJS) ./linker.ld
	$(CC) -T ./linker.ld -o $@ $(CFLAGS) $(LINK_LIST) -Wl,--build-id=none
	grub-file --is-x86-multiboot os_64.ker

$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

.c.o:
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

.S.o:
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f os_32.kernel
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps ../../include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: os_64.ker
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp os_64.ker $(DESTDIR)$(BOOTDIR)

-include $(OBJS:.o=.d)
