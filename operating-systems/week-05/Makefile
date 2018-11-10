# Copyright (c) 1997-2017, FenixOS Developers
# All Rights Reserved.
#
# This file is subject to the terms and conditions defined in
# file 'LICENSE', which is part of this source code package.

CC := gcc
AS := as
STRIP := strip
LD := ld
OBJCOPY := objcopy

OPTIMIZATION_CFLAGS ?= -O3

# This variable holds the compilation flags
CFLAGS ?= -msoft-float -mno-mmx -mno-sse -Wall -fno-builtin \
          -Werror -fno-strict-aliasing -fno-common -pedantic \
          -std=gnu99 -m32 -march=i386 $(OPTIMIZATION_CFLAGS)

INCLUDE_DIRS = -Iinclude/
USER_INCLUDE_DIRS = -Isrc/program_include/

# Rules for cd-image generation and booting
bochs/boot.iso : objects/kernel/kernel.stripped bochs/grub.cfg
	-rm -rf bochs/iso
	-mkdir -p bochs/iso/boot/grub
	cp bochs/grub.cfg bochs/iso/boot/grub/
	cp objects/kernel/kernel.stripped bochs/iso/kernel
	grub-mkrescue -o bochs/boot.iso bochs/iso

boot: bochs/boot.iso
	(cd bochs/; nice -20 bochs -q -f bochsrc)

boot-gdb: bochs/boot.iso
	(cd bochs/; nice -20 bochs-gdb -q -f bochsrc.gdb)

# This variable holds object files which are to be linked into the main
# kernel image.
KERNEL_OBJECTS = \
 objects/kernel/kernel.o \
 objects/kernel/video.o

KERNEL_SOURCES = \
 src/kernel/kernel.c \
 src/kernel/video.c

# Rules for the kernel
objects/kernel/kernel.stripped: objects/kernel/kernel | objects/kernel
	$(STRIP) -o objects/kernel/kernel.stripped objects/kernel/kernel

objects/kernel/kernel: objects/kernel/entry.o $(KERNEL_OBJECTS) src/kernel/kernel_link.ld | objects/kernel
	$(CC) $(CFLAGS) -static -nostdlib -Wl,-zmax-page-size=4096 -Tsrc/kernel/kernel_link.ld -o objects/kernel/kernel objects/kernel/entry.o $(KERNEL_OBJECTS)

objects/kernel/entry.o: src/kernel/entry.s | objects/kernel
	$(AS) --gstabs --32 -o objects/kernel/entry.o src/kernel/entry.s

objects/kernel/%.d: src/kernel/%.c | objects/kernel
	@set -e; rm -f $@; \
        $(CC) $(CFLAGS) $(INCLUDE_DIRS) -M $< > $@.$$$$; \
        sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
        rm -f $@.$$$$

-include $(KERNEL_SOURCES:src/kernel/%.c=objects/kernel/%.d)

objects/kernel/%.o: src/kernel/%.c objects/kernel/%.d | objects/kernel
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c -o $@ $<

# Misc rules
clean:
	-rm -rf objects bochs/iso bochs/boot.iso

objects:
	-mkdir -p objects

objects/kernel:
	-mkdir -p objects/kernel

compile: objects/kernel/kernel

all: boot
