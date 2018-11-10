/* Copyright (c) 1997-2017, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file sysdefines.h
 *  This file defines constants and data types used by system calls.
 *
 */

#ifndef _SYSDEFINES_H_
#define _SYSDEFINES_H_

/* Constant declarations. */

/*! Return code when system call returns normally. */
#define ALL_OK                  (0)
/*! Return code when system call returns with an error. */
#define ERROR                   (-1)
/*! Return code when system call is unknown. */
#define ERROR_ILLEGAL_SYSCALL   (-2)

/*! System call that returns the version of the kernel. */
#define SYSCALL_VERSION         (0)
/*! System call that prints a string. */
#define SYSCALL_PRINTS          (1)
/*! System call that prints a hexadecimal value. */
#define SYSCALL_PRINTHEX        (2)

/*! System call that allocates a memory block. The length of the requested
    memory block is passed in edi. The system call returns the address or an
    error code. */
#define SYSCALL_ALLOCATE        (4)

/*! System call that frees a memory block allocated through the allocate
    system call. The address of the memory block is passed in edi. The
    system call returns  ALL_OK if successful or an error code if
    unsuccessful. */
#define SYSCALL_FREE            (5)

/*! System call that terminates the currently running
 *  thread. It takes no parameters. Terminates the process
 *  when there are no threads left. */
#define SYSCALL_TERMINATE       (6)

/*! System call that creates a new process with one single
 *  thread. The program used is the executable whose index is
 *  passed in edi. */
#define SYSCALL_CREATEPROCESS   (7)

/*! System call that will temporarily move the calling thread from the running
    to the ready state. The system call does not take any parameters and returns
    ALL_OK.*/
#define SYSCALL_YIELD           (8)

/*! Creates a new thread in the process of the calling thread. The value of 
    eip register of the new thread is passed in the edi registe. The value of 
    the esp register of the new thread is passed in esi. The other registers
    are undefinied.

    The system call returns in rax ALL_OK if successful or an error code if 
    unsuccessful.
   */
#define SYSCALL_CREATETHREAD    (16)

/*! Creates a new semaphore and returns if successful the handle of the
    semaphore in the eax register. The initial value of the semaphore count
    value is passed in the edi register.
   
    If unsuccessful the system call returns an error code in rax. */
#define SYSCALL_CREATESEMAPHORE (17)

/*! Performs a semaphore down operation. The handle of the semaphore is passed
    in the edi register.

    The system call returns in rax ALL_OK if successful or an error code if 
    unsuccessful.
   */
#define SYSCALL_SEMAPHOREDOWN   (18)

/*! Performs an semaphore up operation. The handle of the semaphore is passed
    in the edi register.

    The system call returns in rax ALL_OK if successful or an error code if 
    unsuccessful.
   */
#define SYSCALL_SEMAPHOREUP     (19)

#endif
