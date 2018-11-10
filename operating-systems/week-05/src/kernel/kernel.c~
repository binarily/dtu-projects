/* Copyright (c) 1997-2017, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file kernel.c This file holds the code for the kernel. */

#include <stdint.h>
#include <instruction_wrappers.h>

/* First some declarations for data structures and functions found in the
   assembly code or the linker script. */

/*! Halts the machine. Will stop the machine and only reset will wake
    it up. */
extern void halt_the_machine(void);

/* Declarations for functions found in other C files. */

/*! Outputs a string to the VGA screen. */
extern void
kprints(const char* const string
        /*!< Points to a null terminated string */);

/*! Outputs an unsigned 32-bit value to the VGA screen. */
extern void
kprinthex(const register uint32_t value /*! value to be printed. */);

/* Declarations for this file. */

/*! Initializes the kernel. */
extern void kernel_init(register uint32_t* const multiboot_information)
 __attribute__ ((noreturn));

/* Definitions. */

void kernel_init(register uint32_t* const multiboot_information
                                          /*!< Points to a multiboot
                                               information structure. */)
{
 /* The Multiboot specification dictates that a boot loader will leave
    information in memory for the kernel to use, see
    http://www.gnu.org/software/grub/manual/multiboot . We go through the
    information and extract the information we need. We also sanity check. */

 /* Check if the boot loader left information on memory. */
 if (!(1 & *multiboot_information))
  halt_the_machine();

 /* Check if we can use sysenter/sysret. It is highly likely that sysenter
    is supported, it has been since Pentium 2, so this is really a sanity
    check. */
 {
  uint32_t eax, ebx, ecx, edx;

  cpuid(1, &eax, &ebx, &ecx, &edx);

  /* Check if the processor says it supports sysenter. */
  if (!(0x800 & edx))
   halt_the_machine();

  /* Pentium Pro has a bug in that it errouneously says it supports
     sysenter. Check if we are running on processor such as Pentium
     Pro which does not support sysenter. */
  if ((((eax & 0x00000F00) >> 8) == 6) &&
     (((eax & 0x000000F0) >> 4) < 3) &&
     ((eax & 0x0000000F) < 3))
   halt_the_machine();
 }

 /* Set up segment selectors. This is a x86 specific concept. Selectors
    are used to control execution modes of the processor and to control
    how data is accessed. Selectors are controlled through the GDT. The
    processor has to be given the size and pointer to the GDT. */
 {
  const static uint32_t gdt[10] = {0, 0, /* null */
                                   0xffff, 0x00cf9a00, /* kernel code */
                                   0xffff, 0x00cf9200, /* kernel data */
                                   0xffff, 0x00cffa00, /* user code */
                                   0xffff, 0x00cff200  /* user data */};

  /* Install it. */
  lgdt(sizeof(gdt) - 1, (uintptr_t) gdt);
  /* Install the local descriptor table. This is related to the GDT and
     installed as a safety/stability precaution. */
  lldt(0);

  /* Make the processor use the new GDT. */
  __asm volatile ("mov %%bx, %%ds\n \
                   mov %%bx, %%es\n \
                   mov %%bx, %%fs\n \
                   mov %%bx, %%gs\n \
                   mov %%bx, %%ss\n \
                   ljmp $8,$1f\n \
                   1:" : : "b" (16) :);
 }

 kprints("The kernel has booted!\n");

 /* Halt as we cannot do anything more right now. */ 
 halt_the_machine();

 while(1) ;
}
