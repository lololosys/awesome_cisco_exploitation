/* $Id: asm_c3000.h,v 3.2 1995/11/17 08:39:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c3000.h,v $
 *------------------------------------------------------------------
 * Assembly definitions for c3000 assembly wrappers
 *
 * August 1993, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: asm_c3000.h,v $
 * Revision 3.2  1995/11/17  08:39:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:54:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:05:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../boot/src-68-c3000/pcmap.h"

/*
 * Registers to save on interrupt
 */
#define REGISTERS	d0-d3/a0-a3
