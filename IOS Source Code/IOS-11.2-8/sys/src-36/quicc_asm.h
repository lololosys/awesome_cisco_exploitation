/* $Id: quicc_asm.h,v 3.2 1995/11/17 18:39:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-36/quicc_asm.h,v $
 *------------------------------------------------------------------
 * quicc_asm.h -- QUICC Assembler parameters
 *
 * November 1994, Andrew G. Harvey
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: quicc_asm.h,v $
 * Revision 3.2  1995/11/17  18:39:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:51:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __QUICC_ASM_H__
#define __QUICC_ASM_H__

#include "../src-36-c1000/pcmap.h"

#define SCCE1  	(0x1000 + 0x610)
#define SCCE2	(0x1000 + 0x630)
#define SCCE3  	(0x1000 + 0x650)
#define SCCE4	(0x1000 + 0x670)
#define CISR	(0x1000 + 0x54C)

/*
 * This size MUST be the size of the quicc_interrupt_t structure.
 */
#define SCC1OFF		0   /* (0 * sizeof(quicc_interrupt_t)) */
#define SCC2OFF		16  /* (1 * sizeof(quicc_interrupt_t)) */
#define SCC3OFF		32  /* (2 * sizeof(quicc_interrupt_t)) */
#define SCC4OFF		48  /* (3 * sizeof(quicc_interrupt_t)) */

#endif
