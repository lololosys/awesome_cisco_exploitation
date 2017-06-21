/* $Id: vspace_4k.h,v 3.2.58.1 1996/03/21 23:46:41 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-4k/vspace_4k.h,v $
 *------------------------------------------------------------------
 * vspace_4k.h -- Header file for R4K virtual address space
 *                definition
 * 
 * Michael Beesley, November 1993
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vspace_4k.h,v $
 * Revision 3.2.58.1  1996/03/21  23:46:41  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/11/22  21:42:54  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:40:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:21:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:52  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * typedef for virtual space defintion array
 */
typedef struct vspace_tbl_t_ {
    ulong vaddr, paddr, paddr_a35_a32;
    ushort page_mask;
    unsigned pshift : 8;
    unsigned cache  : 3;
    unsigned dirty  : 1;
    unsigned valid  : 1;
    unsigned global : 1;
} vspace_tbl_t;

/*
 * Definitions for controlling physical address bits 35..32 which
 * are used when a level 2 cache is installed.
 */
#define ADDR_BIT32_ZERO		0x00
#define ADDR_BIT32_ONE		0x01
#define ADDR_BIT33_ZERO		0x00
#define ADDR_BIT33_ONE		0x02
#define ADDR_BIT35_32_SHIFT	20

/*
 * Definitions to control main DRAM physical addresses
 */
#define DRAM_ADDR33	(ADDR_BIT33_ZERO << ADDR_BIT35_32_SHIFT)

/*
 * prototypes
 */
extern vspace_tbl_t *vspace_table_addr(long *);

