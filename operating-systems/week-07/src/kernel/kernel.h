/* Copyright (c) 1997-2017, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file kernel.h This file holds declarations
  for the kernel that you should not touch. */

#ifndef _KERNEL_H_
#define _KERNEL_H_

/* First some declarations for data structures and functions found in the
   assembly code or the linker script. */

/*! Halts the machine. Will stop the machine and only reset will wake
    it up. */
extern void halt_the_machine(void);

/*! Go to user space. */
extern void go_to_user_space(void) __attribute__ ((noreturn));

/* Declarations for functions found in other C files. */

/*! Outputs a string to the VGA screen. */
extern void
kprints(const char* const string
        /*!< Points to a null terminated string */);

/*! Outputs an unsigned 32-bit value to the VGA screen. */
extern void
kprinthex(const register uint32_t value /*! value to be printed. */);

/*! Size of the array that holds starting addresses of applications */
#define EXECUTABLE_TABLE_SIZE (3)

/*! Array of starting addresses for all applications. The starting
    address is the address of the first instruction to execute in each
    program. */
extern
uintptr_t executable_table[EXECUTABLE_TABLE_SIZE];

/*! Maximum number of threads in the system. */
#define MAX_THREADS 256

/*! Maximum number of processes in the system. */
#define MAX_PROCESSES 16

/*! Finalizes initialization of the kernel. */
extern void kernel_late_init(void)
 __attribute__ ((noreturn));

/*! Handles one system call. */
extern void handle_system_call(void);

#include "kernel_customization.h"

/*! All threads in the system. */
extern struct thread threads[MAX_THREADS];

/*! The current thread running on the cpu. */
extern struct thread* current_thread;

/*! All processes in the system. */
extern struct process processes[MAX_PROCESSES];
#endif
