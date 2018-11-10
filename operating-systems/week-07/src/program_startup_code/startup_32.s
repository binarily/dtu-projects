# Copyright (c) 1997-2017, FenixOS Developers
# All Rights Reserved.
#
# This file is subject to the terms and conditions defined in
# file 'LICENSE', which is part of this source code package.

# Startup code for programs

.text
.global _start

_start:
 # Setup stack pointer
 mov $stack, %esp

 # main expects two arguments: argc and argv
 push $argv
 push 1

 # Call main
 call main

 # When main returns, perform the terminate system call
 # so the kernel can clean up
 mov $6, %eax
 sysenter
 # NOTE: we never return!
 hlt
	
.section .data
.align 4
name:
 .asciz "dummy"
argv:
 .int  name
 .int  0

.section .bss
.align 4
.skip 8 * 1024
stack:
