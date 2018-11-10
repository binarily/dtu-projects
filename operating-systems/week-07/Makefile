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
          -Werror -fno-strict-aliasing -fno-common \
          -std=gnu99 -m32 -march=i386 -fno-stack-protector \
          $(OPTIMIZATION_CFLAGS)

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

# This variable holds object files which hold user level executables
EXECUTABLES = \
 objects/program_0/executable.o \
 objects/program_1/executable.o \
 objects/program_2/executable.o

# This variable holds object files which are to be linked into the main
# kernel image.
KERNEL_OBJECTS = \
 objects/kernel/kernel.o \
 objects/kernel/kernel_customization.o \
 $(EXECUTABLES) \
 objects/kernel/video.o

KERNEL_SOURCES = \
 src/kernel/kernel.c \
 src/kernel/kernel_customization.c \
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

objects/program_startup_code:
	-mkdir -p objects/program_startup_code

objects/program_startup_code/startup_32.o: src/program_startup_code/startup_32.s | objects/program_startup_code
	$(AS) --32 -o objects/program_startup_code/startup_32.o src/program_startup_code/startup_32.s

objects/program_0:
	-mkdir -p objects/program_0

objects/program_0/main.o: src/program_0/main.c src/program_include/scwrapper.h | objects/program_0
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(USER_INCLUDE_DIRS) $(OPTIMIZATIONFLAGS) -m32 -c -o objects/program_0/main.o src/program_0/main.c

objects/program_0/executable: objects/program_startup_code/startup_32.o objects/program_0/main.o src/program_startup_code/program_link.ld | objects/program_0
	$(LD) -m elf_i386 -z max-page-size=4096 -static -Tsrc/program_startup_code/program_link.ld --defsym __executable__=0 -o objects/program_0/executable objects/program_startup_code/startup_32.o objects/program_0/main.o

objects/program_0/executable.o: objects/program_0/executable.stripped | objects/program_0
	$(OBJCOPY) -O binary objects/program_0/executable.stripped objects/program_0/executable.bin
	$(OBJCOPY) -I binary -O elf32-i386 -B i386 --rename-section .data=.exec,alloc,contents,load,readonly,data objects/program_0/executable.bin objects/program_0/executable.o

objects/program_0/executable.stripped: objects/program_0/executable | objects/program_0
	$(STRIP) -o objects/program_0/executable.stripped objects/program_0/executable

objects/program_1:
	-mkdir -p objects/program_1

objects/program_1/main.o: src/program_1/main.c src/program_include/scwrapper.h | objects/program_1
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(USER_INCLUDE_DIRS) $(OPTIMIZATIONFLAGS) -m32 -c -o objects/program_1/main.o src/program_1/main.c

objects/program_1/executable: objects/program_startup_code/startup_32.o objects/program_1/main.o src/program_startup_code/program_link.ld | objects/program_1
	$(LD) -m elf_i386 -z max-page-size=4096 -static -Tsrc/program_startup_code/program_link.ld --defsym __executable__=1 -o objects/program_1/executable objects/program_startup_code/startup_32.o objects/program_1/main.o

objects/program_1/executable.o: objects/program_1/executable.stripped | objects/program_1
	$(OBJCOPY) -O binary objects/program_1/executable.stripped objects/program_1/executable.bin
	$(OBJCOPY) -I binary -O elf32-i386 -B i386 --rename-section .data=.exec,alloc,contents,load,readonly,data objects/program_1/executable.bin objects/program_1/executable.o

objects/program_1/executable.stripped: objects/program_1/executable | objects/program_1
	$(STRIP) -o objects/program_1/executable.stripped objects/program_1/executable


objects/program_2:
	-mkdir -p objects/program_2

objects/program_2/main.o: src/program_2/main.c src/program_include/scwrapper.h | objects/program_2
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(USER_INCLUDE_DIRS) $(OPTIMIZATIONFLAGS) -m32 -c -o objects/program_2/main.o src/program_2/main.c

objects/program_2/executable: objects/program_startup_code/startup_32.o objects/program_2/main.o src/program_startup_code/program_link.ld | objects/program_2
	$(LD) -m elf_i386 -z max-page-size=4096 -static -Tsrc/program_startup_code/program_link.ld --defsym __executable__=2 -o objects/program_2/executable objects/program_startup_code/startup_32.o objects/program_2/main.o

objects/program_2/executable.o: objects/program_2/executable.stripped | objects/program_2
	$(OBJCOPY) -O binary objects/program_2/executable.stripped objects/program_2/executable.bin
	$(OBJCOPY) -I binary -O elf32-i386 -B i386 --rename-section .data=.exec,alloc,contents,load,readonly,data objects/program_2/executable.bin objects/program_2/executable.o

objects/program_2/executable.stripped: objects/program_2/executable | objects/program_2
	$(STRIP) -o objects/program_2/executable.stripped objects/program_2/executable

# Misc rules
clean:
	-rm -rf objects bochs/iso bochs/boot.iso

objects:
	-mkdir -p objects

objects/kernel:
	-mkdir -p objects/kernel

compile: objects/kernel/kernel

all: boot
