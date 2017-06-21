/* $Id: i8254.h,v 3.2 1995/11/17 07:41:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/i8254.h,v $
 *------------------------------------------------------------------
 * i8254.h - Intel 8254 Timer definitions
 *
 * September 1987, Greg Satz
 *
 * Copyright (c) 1987-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: i8254.h,v $
 * Revision 3.2  1995/11/17  07:41:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	ctaddr		((struct timerdevice *)CTDEVBASEADDR)

struct timerdevice {
	volatile u_char counter0;
	volatile u_char counter1;
	volatile u_char counter2;
	volatile u_char control;
};

/*
 * Control word definitions
 */

#define	SEL_COUNTER0		0x00	/* select counter 0 */
#define	SEL_COUNTER1		0x40	/* select counter 1 */
#define	SEL_COUNTER2		0x80	/* select counter 2 */
#define	SEL_READBACK		0xC0	/* Read back command */

#define	RW_LATCH		0x00	/* Counter latch command */
#define	RW_LSB			0x10	/* R/W least significant byte */
#define	RW_MSB			0x20	/* R/W most significant byte */
#define	RW_LSBMSB		0x30	/* R/W least then most sig. byte */

#define	CT_MODE0		0x00	/* Mode 0 */
#define	CT_MODE1		0x02	/* Mode 1 */
#define	CT_MODE2		0x04	/* Mode 2 */
#define	CT_MODE3		0x06	/* Mode 3 */
#define	CT_MODE4		0x08	/* Mode 4 */
#define	CT_MODE5		0x0A	/* Mode 5 */

#define	CT_BCD			0x01	/* BCD counter */
