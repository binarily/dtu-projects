/* Copyright (c) 1997-2017, FenixOS Developers
   All Rights Reserved.

   This file is subject to the terms and conditions defined in
   file 'LICENSE', which is part of this source code package.
 */

/**
 * \file program_link.ld This file contains the link script needed to link 
 * the application executables.
 */
OUTPUT_FORMAT("elf32-i386")

SECTIONS {
 . = 0x00300000 + (0x10000 * 2 * __executable__);
 .text : { *(.text) } 
 . = NEXT(0x10000);
 .data : { *(.data) *(.rodata) *(.bss) } 
}
