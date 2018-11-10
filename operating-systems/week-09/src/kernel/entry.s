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
 .global sysenter_entry_point
 .global go_to_user_space
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

sysenter_entry_point:
 push   %eax
 mov    $16,%eax
 mov    %ax,%ds
 mov    %ax,%es
 mov    %ax,%fs
 mov    %ax,%gs
 mov    current_thread,%eax

 # Save all necessary registers. Note that eax is on the stack, see above.
 # The system call wrappers used by user programs will place user space esp
 # in ecx and user space eip in edx

 popl   (%eax)
 mov    %ebx,4(%eax)
 mov    %esi,8(%eax)
 mov    %edi,12(%eax)
 mov    %ebp,16(%eax)
 mov    %ecx,20(%eax)
 mov    %edx,24(%eax)	

 jmp    handle_system_call

go_to_user_space:
 mov    current_thread,%eax
	
 pushl  (%eax)
 mov    4(%eax),%ebx
 mov    8(%eax),%esi
 mov    12(%eax),%edi
 mov    16(%eax),%ebp
 mov    20(%eax),%ecx
 mov    24(%eax),%edx	
		
 # Before executing this code, the value of esp in user space is placed
 # in ecx, the value of eip in user space is plaxed in edx. The value of eax
 # in user space is placed as the first element on the stack, see below!
 mov    $35,%eax
 mov    %ax,%ds
 mov    %ax,%es
 mov    %ax,%fs
 mov    %ax,%gs
 pop    %eax
 sysexit
	
 .data
 .bss

 .align 4
 .space 8*1024,0
kernel_stack:
 
