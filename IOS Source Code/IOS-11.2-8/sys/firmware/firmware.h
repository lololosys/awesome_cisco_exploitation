/* $Id: firmware.h,v 3.2 1995/11/17 09:10:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/firmware/firmware.h,v $
 *------------------------------------------------------------------
 * firmware.h - common definitions.
 *
 * Aug 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: firmware.h,v $
 * Revision 3.2  1995/11/17  09:10:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:34:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Define a macro to put under_scores for M68k stuff
 * and remove them for mips.
 */

#ifdef OBJECT_4K

#define DATA16		.half
#define DATA32(a)	.word	(a)
#define UNDER_BAR(a)		a

#else /* OBJECT_4K */

#define DATA16		.word
#define DATA32(a)	.long	(a)
#define UNDER_BAR(a)		_ ## a

#endif /* OBJECT_4K */







