/* $Id: m68vectors.h,v 3.2 1995/11/17 07:41:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/m68vectors.h,v $
 *------------------------------------------------------------------
 * m68vectors.h -- define MC68000 exception vectors
 *
 * August 1986-1992 Greg Satz
 *
 * Copyright (c) 1986-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: m68vectors.h,v $
 * Revision 3.2  1995/11/17  07:41:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	EVEC_RESETSSP	((long *)(ADRSPC_RAM+0x000))
#define EVEC_RESETPC	((long *)(ADRSPC_RAM+0x004))
#define EVEC_BUSERR	((long *)(ADRSPC_RAM+0x008))
#define EVEC_ADDERR	((long *)(ADRSPC_RAM+0x00C))
#define EVEC_ILLINST	((long *)(ADRSPC_RAM+0x010))
#define EVEC_DIVZERO	((long *)(ADRSPC_RAM+0x014))
#define EVEC_CHK	((long *)(ADRSPC_RAM+0x018))
#define EVEC_TRAPV	((long *)(ADRSPC_RAM+0x01C))
#define EVEC_PRIV	((long *)(ADRSPC_RAM+0x020))
#define EVEC_TRACE	((long *)(ADRSPC_RAM+0x024))
#define EVEC_LINE1010	((long *)(ADRSPC_RAM+0x028))
#define EVEC_LINE1111	((long *)(ADRSPC_RAM+0x02C))

/*
 * vectors 0x30 - 0x5C are "reserved to Motorola")
 */

#define EVEC_SPURINT	((long *)(ADRSPC_RAM+0x060))
#define EVEC_LEVEL1	((long *)(ADRSPC_RAM+0x064))
#define EVEC_LEVEL2	((long *)(ADRSPC_RAM+0x068))
#define EVEC_LEVEL3	((long *)(ADRSPC_RAM+0x06C))
#define EVEC_LEVEL4	((long *)(ADRSPC_RAM+0x070))
#define EVEC_LEVEL5	((long *)(ADRSPC_RAM+0x074))
#define EVEC_LEVEL6	((long *)(ADRSPC_RAM+0x078))
#define EVEC_LEVEL7	((long *)(ADRSPC_RAM+0x07C))

#define EVEC_TRAP0	((long *)(ADRSPC_RAM+0x080))
#define EVEC_TRAP1	((long *)(ADRSPC_RAM+0x084))
#define EVEC_TRAP2	((long *)(ADRSPC_RAM+0x088))
#define EVEC_TRAP3	((long *)(ADRSPC_RAM+0x08C))
#define EVEC_TRAP4	((long *)(ADRSPC_RAM+0x090))
#define EVEC_TRAP5	((long *)(ADRSPC_RAM+0x094))
#define EVEC_TRAP6	((long *)(ADRSPC_RAM+0x098))
#define EVEC_TRAP7	((long *)(ADRSPC_RAM+0x09C))
#define EVEC_TRAP8	((long *)(ADRSPC_RAM+0x0A0))
#define EVEC_TRAP9	((long *)(ADRSPC_RAM+0x0A4))
#define EVEC_TRAPA	((long *)(ADRSPC_RAM+0x0A8))
#define EVEC_TRAPB	((long *)(ADRSPC_RAM+0x0AC))
#define EVEC_TRAPC	((long *)(ADRSPC_RAM+0x0B0))
#define EVEC_TRAPD	((long *)(ADRSPC_RAM+0x0B4))
#define EVEC_TRAPE	((long *)(ADRSPC_RAM+0x0B8))
#define EVEC_TRAPF	((long *)(ADRSPC_RAM+0x0BC))

/*
 * vectors 0xC0 - 0xFC are "reserved to Motorola"
 */

/*
 * vectors 0x100 - 0x3FC are User Interrupt Vectors
 */
#define EVEC_USERINT(v)	((long *)(ADRSPC_RAM + 0x100 + ((v)<<2)))

#define EVEC_LASTVEC	((long *)(ADRSPC_RAM+0x3FC))
#define EVEC_AFTER	((long *)(ADRSPC_RAM+0x400))
#define NUM_EVECS	256	/* number of exception vectors */

