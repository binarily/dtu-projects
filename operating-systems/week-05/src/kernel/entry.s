# Copyright (c) 1997-2017, FenixOS Developers
# All Rights Reserved.
#
# This file is subject to the terms and conditions defined in
# file 'LICENSE', which is part of this source code package.

# This file holds all assembly code.
# WARNING: This code, like most assembly, code is far from easy to understand.
# Quick introductions to the AT&T syntax used by GNU Assembler can be found
# at http://sourceware.org/binutils/docs-2.17/as/index.html .
#  Take help from a teaching assistant!

 .global _start
 .global halt_the_machine
 .global kernel_stack
	
 .text
 # Here be dragons.
 #
 # The multiboot header which must be located early in the
 # boot image. More information on the multiboot standard can be found
 # at http://www.gnu.org/software/grub/manual/multiboot/

 .align 4
 .int    0x1BADB002             # magic word
 .int    1                      # flags
 .int    -(0x1BADB002+1)        # checksum
 .int    0
 .int    0
 .int    0
 .int    0
 .int    0
 .int    0
 .int    0
 .int    0
 .int    0

_start:
 # Disable interrupts in case they are enabled
 cli
	
 # Check that the boot loader put the expected magic number in eax
 cmpl   $0x2BADB002,%eax
 jne    halt_the_machine  # Ouch, no multiboot capable loader

 # Clear the kernel bss segment
 xor    %ecx,%ecx
 movl   $start_of_bss,%esi
bss_clear_loop:
 mov    %ecx,(%esi)
 add    $4,%esi
 cmp    $end_of_bss,%esi
 js     bss_clear_loop

 # Set a proper stack to use
 mov    $kernel_stack,%esp
	
 # May look strange but will keep GDB from parsing too far up the stack when
 # inspecting the stack
 pushl  %ecx

 # Breakpoint for the debugger
_debug_start:
	
 # Push the address to the Multiboot header
 pushl  %ebx

 # This is a little tricky. We can test if the CPUID instruction is available
 # by testing if bit 21 in EFLAGS can be changed. The kernel code needs the
 # CPUID instruction when initializing.
 pushf
 pop     %ecx
 btr     $21,%ecx
 push    %ecx
        
 popf
 pushf
 pop     %ecx
 btc     $21,%ecx
 jb      halt_the_machine
        
 push    %ecx
 popf
 pushf
 pop     %ecx
 bt      $21,%ecx
 jae     halt_the_machine
	
 # initialize the kernel. We will actually never return.
 call    kernel_init
 
 # We will end up here if something bad happens.
halt_the_machine:
 hlt    # halt the processor until the next unmasked external interrupt
 jmp    halt_the_machine

 .data
 .bss

 .align 4
 .space 8*1024,0
kernel_stack:
 
