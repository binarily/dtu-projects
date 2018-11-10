/* Copyright (c) 1997-2017, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/*! \file instruction_wrappers.h This file holds wrappers for various
   assembly language snippets. These wrappers allow access to low-level
   features such as machine specific registers and instructions that support
   synchronization primitives. */

#ifndef INSTRUCTION_WRAPPERS_H
#define INSTRUCTION_WRAPPERS_H

# include <stdint.h>

/*! Wrapper for the hlt instruction. */
static inline void
hlt(void)
{
 __asm volatile("hlt" : : : );
}


/*! Wrapper for the cpuid instruction. */
static inline void
cpuid(register const uint32_t functionNumber /*!< The number of the
                                                  requested function.
                                              */,
      register uint32_t*      EAXValue       /*!< The value returned
                                                  in the EAX register.
                                              */,
      register uint32_t*      EBXValue       /*!< The value returned
                                                  in the EBX register.
                                              */,
      register uint32_t*      ECXValue       /*!< The value returned
                                                  in the ECX register.
                                              */,
      register uint32_t*      EDXValue       /*!< The value returned
                                                  in the EDX register.
                                              */)
{
 __asm volatile("cpuid" :
                "=a" (*EAXValue),
                "=b" (*EBXValue),
                "=c" (*ECXValue),
                "=d" (*EDXValue) :
                "a" (functionNumber) : );
}

/*! Wrapper for the rdmsr instruction. */
static inline void
rdmsr(register const uint32_t MSR      /*!< The number of the MSR to read. */,
      register uint32_t*      EAXValue /*!< The low-order 32 bits of the MSR.
                                        */,
      register uint32_t*      EDXValue /*!< The high-order 32 bits of the MSR.
                                         */)
{
 __asm volatile("rdmsr" :
                "=a" (*EAXValue),
                "=d" (*EDXValue) :
                "c" (MSR) : );
}

/*! Wrapper for the wrmsr instruction. */
static inline void
wrmsr(register const uint32_t MSR      /*!< The number of the MSR to write.
                                        */,
      register const uint32_t EAXValue /*!< The low-order 32 bits of the
                                            value to write to the MSR. */,
      register const uint32_t EDXValue /*!< The high-order 32 bits of the
                                            value to write to the MSR. */)
{
 __asm volatile("wrmsr" : :
                "a" (EAXValue),
                "d" (EDXValue),
                "c" (MSR) : );
}

/*! Wrapper for the cli instruction. */
static inline void
cli(void)
{
 __asm volatile("cli" : : : );
}

/*! Wrapper for the lgdt instruction. */
static inline void
lgdt(register const uint16_t limit       /*!< The limit of the GDT. This is
                                              the size of the GDT minus one
                                              byte. */,
     register const uint32_t GDTAddress /*!< The starting address of the GDT.
                                         */)
{
 __asm volatile("sub $8,%%esp\n mov %%eax,4(%%esp)\n \
                 mov %%bx,2(%%esp)\n lgdt 2(%%esp)\n add $8,%%esp" :
                : "a" (GDTAddress), "b" (limit) : );
}

/*! Wrapper for the lldt instruction. */
static inline void
lldt(register const uint16_t selector /*!< A selector into the GDT for the LDT.
                                       */)
{
 __asm volatile("lldt %%ax" : : "a" (selector) : );
}

static inline uint64_t rdtsc(void)
{
 uint32_t low_value, high_value;

 __asm volatile("rdtsc" : "=a" (low_value), "=d" (high_value));

 return (uint64_t)high_value << 32 | low_value;
}

/*! Wrapper for the sti instruction. */
static inline void
sti(void)
{
 __asm volatile("sti" : : : );
}

/*! Wrapper for a 8-bit out instruction. */
static inline void
outb(register const int16_t portNumber  /*!< The number of the port to write
                                             to. */,
     register const int8_t  outputValue /*!< The value to be written to the
                                             port. */)
{
 __asm volatile("outb %%al,%%dx" : : "d" (portNumber), "a" (outputValue));
}

/*! Wrapper for a 16-bit out instruction. */
static inline void
outw(register const int16_t portNumber  /*!< The number of the port to write
                                             to. */,
     register const int16_t outputValue /*!< The value to be written to the
                                             port. */)
{
 __asm volatile("outw %%ax,%%dx" : : "d" (portNumber), "a" (outputValue));
}

/*! Wrapper for a 32-bit out instruction. */
static inline void
outl(register const int16_t portNumber    /*!< The number of the port to write
                                               to. */,
     register const int32_t   outputValue /*!< The value to be written to the
                                               port. */)
{
 __asm volatile("outl %%eax,%%dx" : : "d" (portNumber), "a" (outputValue));
}

/*! Wrapper for a 8-bit in instruction.
    \returns The value read. */
static inline int8_t
inInt8(register const int16_t  portNumber /*! The number of the port to read
                                               from. */ )
{
 register int8_t returnValue;
 __asm volatile("inb %%dx,%%al" : "=a" (returnValue) : "d" (portNumber));
 return returnValue;
}

/*! Wrapper for a 16-bit in instruction.
    \returns The value read. */
static inline int16_t
inInt16(register const int16_t portNumber /*! The number of the port to read
                                              from. */ )
{
 register short returnValue;
 __asm volatile("inw %%dx,%%ax" : "=a" (returnValue) : "d" (portNumber));
 return returnValue;
}

/*! Wrapper for a 32-bit in instruction.
    \returns The value read. */
static inline int
inInt32(register const int16_t  portNumber /*! The number of the port to read
                                               from. */ )
{
 register int returnValue;
 __asm volatile("inl %%dx,%%eax" : "=a" (returnValue) : "d" (portNumber));
 return returnValue;
}

/*! Wrapper for a 32-bit locked xchg instruction.
    \returns The value stored in the variable pointed to by
             pointer_to_variable before the operation is performed. */
static inline uint32_t
lock_xchg(register volatile uint32_t * const
                                         pointer_to_variable
                                         /*!< Pointer to the variable to
                                              operate on. */,
          register uint32_t              value
                                         /*!< The value to be written to
                                              memory. */)
{
 __asm volatile("lock xchgl %k0,%1"
                : "=r" (value)
                : "m" (*pointer_to_variable), "0" (value)
                : "memory");
 return value;
}

/*! Wrapper for a 32-bit locked xadd instruction.
    \returns The value stored in the variable pointed to by
             pointer_to_variable before the operation is performed.  */
static inline uint32_t
lock_xadd(register volatile uint32_t * const
                                          pointer_to_variable
                                           /*!< Pointer to the variable to
                                                operate on. */,
          register uint32_t               increment /*!< Value to add to the
                                                         variable. */)
{
 __asm volatile("lock xaddl %k0,%1"
                : "=r" (increment) 
                : "m" (*pointer_to_variable), "0" (increment)
                : "memory");
 return increment;
}

/*! Wrapper for a 32-bit locked cmpxchg instruction.
    \returns The value stored in the variable pointed to by
             pointer_to_variable before the operation is performed. */
static inline uint32_t
lock_cmpxchg(register volatile uint32_t * const
                                             pointer_to_variable
                                             /*!< Pointer to the variable to
                                                  operate on. */,
             register uint32_t               old_value
                                             /*!< The value assumed to be in
                                                  the variable. */,
             register const uint32_t         new_value
                                             /*!< The value to conditionally
                                                  write to the variable. */)
{
 __asm volatile("lock cmpxchgl %k1,%2"
                : "=a" (old_value)
                : "r"(new_value), "m" (*pointer_to_variable), "0" (old_value)
                : "memory");

 return old_value;
}

#endif
