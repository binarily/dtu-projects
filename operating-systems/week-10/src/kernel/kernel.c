/* Copyright (c) 1997-2017, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file kernel.c This file holds the code for the kernel. */

#include <stdint.h>
#include <instruction_wrappers.h>
#include <sysdefines.h>

#include "kernel.h"

/* First some declarations for data structures and functions found in the
   assembly code or the linker script. */

/*! Entry point into the kernel for system calls via sysenter. */
extern uint8_t sysenter_entry_point[];

/*! The kernel stack used when in the kernel. */
extern uint8_t kernel_stack[];

/*! Points to after the last byte used by the embedded
    executable applications. */
extern uint8_t end_of_applications[];

/*! Location of executable application 0. We put this into an array
    later */
extern uint8_t exec_0_start[];

/*! Location of executable application 1. We put this into an array
    later */
extern uint8_t exec_1_start[];

/*! Location of executable application 2. We put this into an array
    later */
extern uint8_t exec_2_start[];

/* Declarations for this file. */

/*! Initializes the kernel. */
extern void kernel_init(register uint32_t* const multiboot_information)
 __attribute__ ((noreturn));

/* These are declared in kernel.h */
uintptr_t executable_table[EXECUTABLE_TABLE_SIZE] = 
 {(uintptr_t)exec_0_start,
  (uintptr_t)exec_1_start,
  (uintptr_t)exec_2_start};

struct thread threads[MAX_THREADS];

struct thread* current_thread = &threads[0];

struct process processes[MAX_PROCESSES];

struct page_frame page_frame_table[MAX_NUMBER_OF_FRAMES];

unsigned long memory_pages;

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

 /* Set up support for sysenter. */
 /* The base code segment selector. This is used to set the other selectors. */
 wrmsr(0x174, 8, 0);
 /* Kernel stack pointer. We keep the top 4 bytes set to zero to keep GDB
    happy. */
 wrmsr(0x175, (uintptr_t)kernel_stack - 4, 0);
 /* The entry point for sysenter. We will end up there at system calls. */
 wrmsr(0x176, (uintptr_t)sysenter_entry_point, 0);

 /* Calculate the number of pages. */

 /* Extract information on how much memory is available in the machine. */
 memory_pages =  ((uintptr_t)(*(multiboot_information+2) + 1024)) / 4;

 if (memory_pages > MAX_NUMBER_OF_FRAMES)
  memory_pages = MAX_NUMBER_OF_FRAMES;

 {
  /* Calculate the number of frames occupied by the kernel and executable 
     images. */
  const register int k=((uintptr_t) end_of_applications)/(4*1024);
  register int i;

  /* Mark the pages that are used by the kernel or executable images as taken
    by the kernel (1 in the owner field). */
  for(i=0; i<k; i++)
  {
   page_frame_table[i].owner=(struct process*) 1;
   page_frame_table[i].free_is_allowed=0;
  }

  /* Loop over all the rest page frames and mark them as free (0 in owner
     field). */
  for(i=k; i<memory_pages; i++)
  {
   page_frame_table[i].owner=(struct process*) 0;
   page_frame_table[i].free_is_allowed=1;
  }

  /* Mark any unusable pages as taken by the kernel. */
  for(i=memory_pages; i<MAX_NUMBER_OF_FRAMES; i++)
  {
   page_frame_table[i].owner=(struct process*) 1;
   page_frame_table[i].free_is_allowed=0;
  }
 }

 kprints("The kernel has booted!\n");

 /* Perform any custom initializations you might want to do. */
 kernel_late_init();
}
