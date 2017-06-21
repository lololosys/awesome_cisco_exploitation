/* $Id: statreg.h,v 3.2 1995/11/17 07:41:45 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/statreg.h,v $
 *------------------------------------------------------------------
 * statreg.h -- definitions for MC68000 processor status register
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: statreg.h,v $
 * Revision 3.2  1995/11/17  07:41:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	SR_C		0x1	/* C (carry) bit */
#define	SR_V		0x2	/* V (overflow) bit */
#define	SR_Z		0x4	/* Z (zero) bit */
#define	SR_N		0x8	/* N (negative) bit */
#define	SR_X		0x10	/* X (extend) bit */

#define	SR_CCODES	(SR_C|SR_V|SR_Z|SR_N|SR_X)	/* condition codes */

#define	SR_INTMASK	0x700	/* interrupt level */

#define	SR_SUPERVISOR	0x2000	/* supervisor state bit */

#define	SR_TRACE	0x8000	/* trace trap bit */
