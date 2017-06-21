/* $Id: asm_c4000.h,v 3.2 1995/11/17 08:39:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_c4000.h,v $
 *------------------------------------------------------------------
 * Assembly definitions for 4000 platforms
 *
 * August 1993, Scott Mackie 
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: asm_c4000.h,v $
 * Revision 3.2  1995/11/17  08:39:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:54:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:05:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../boot/src-68-c4000/pcmap.h"

/*
 * Registers to save on interrupt
 */
#define REGISTERS	d0-d3/a0-a1

/*
 * ichain jump table definitions
 */
#define ICHAIN_JUMP_SLOT1A ICHAIN_TABLE_BASE
#define ICHAIN_JUMP_SLOT1B ICHAIN_JUMP_SLOT1A + ICHAIN_ENTRY_SIZE
#define ICHAIN_JUMP_SLOT2A ICHAIN_JUMP_SLOT1A + ICHAIN_BYTES_PER_SLOT
#define ICHAIN_JUMP_SLOT2B ICHAIN_JUMP_SLOT2A + ICHAIN_ENTRY_SIZE
#define ICHAIN_JUMP_SLOT3A ICHAIN_JUMP_SLOT2A + ICHAIN_BYTES_PER_SLOT
#define ICHAIN_JUMP_SLOT3B ICHAIN_JUMP_SLOT3A + ICHAIN_ENTRY_SIZE

/*
 * idb table definitions
 */
#define HWIDB_SLOT1A HWIDB_TABLE_BASE
#define HWIDB_SLOT1B HWIDB_SLOT1A + HWIDB_ENTRY_SIZE
#define HWIDB_SLOT1C HWIDB_SLOT1B + HWIDB_ENTRY_SIZE
#define HWIDB_SLOT1D HWIDB_SLOT1C + HWIDB_ENTRY_SIZE

#define HWIDB_SLOT2A HWIDB_SLOT1A + HWIDB_BYTES_PER_SLOT
#define HWIDB_SLOT2B HWIDB_SLOT2A + HWIDB_ENTRY_SIZE
#define HWIDB_SLOT2C HWIDB_SLOT2B + HWIDB_ENTRY_SIZE
#define HWIDB_SLOT2D HWIDB_SLOT2C + HWIDB_ENTRY_SIZE

#define HWIDB_SLOT3A HWIDB_SLOT2A + HWIDB_BYTES_PER_SLOT
#define HWIDB_SLOT3B HWIDB_SLOT3A + HWIDB_ENTRY_SIZE
#define HWIDB_SLOT3C HWIDB_SLOT3B + HWIDB_ENTRY_SIZE
#define HWIDB_SLOT3D HWIDB_SLOT3C + HWIDB_ENTRY_SIZE

/* 
 * NIM private scratch areas, to be used freely by each nim.
 */
#define NIM1_SCRATCH_AREA    NIM_SCRATCH_BASE
#define NIM2_SCRATCH_AREA    NIM1_SCRATCH_AREA + NIM_SCRATCH_BYTES_PER_SLOT
#define NIM3_SCRATCH_AREA    NIM2_SCRATCH_AREA + NIM_SCRATCH_BYTES_PER_SLOT

