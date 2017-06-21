/* $Id: asm_4k.h,v 3.1.10.1 1996/03/18 20:50:05 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/asm-4k/asm_4k.h,v $
 *------------------------------------------------------------------
 * asm_4k.h -- macros used by mips 4k .S files
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k.h,v $
 * Revision 3.1.10.1  1996/03/18  20:50:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:20:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:24:44  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Define the assembler support macros for procedure framing
 * FRAME    : sets proc as global, generates debug info with .ent
 * ENDFRAME : generates debug into with .end
 */

#define FRAME(name, frame_register, offset, return_register)  \
    .globl name;                                              \
    .ent   name;                                              \
name:                                                         \
    .frame frame_register, offset, return_register

#define ENDFRAME(name)                                        \
    .end name
