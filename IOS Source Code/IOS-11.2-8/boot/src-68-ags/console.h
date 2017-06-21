/* $Id: console.h,v 3.1 1995/11/09 09:08:27 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-ags/console.h,v $
 *------------------------------------------------------------------
 * console.h - ROM monitor console for the S68562 DUART
 *
 * September 1987, Greg Satz
 *
 * Copyright (c) 1987-1993 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: console.h,v $
 * Revision 3.1  1995/11/09  09:08:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:14:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:32:58  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.2  1993/07/01  23:10:41  hampton
 * Bring modular system to Aries 9.21(0.12) patch level.
 *
 * Revision 1.1  1992/03/07  23:56:18  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../sys/hes/s68562.h"

/*
 * Basic IO primitives needed when initializing (since you can't
 * yet trust memory).
 */

#define PUTSTRING(str,arg) \
	str = arg; \
	while (*str) { \
		while ((uartaddr->a.gsr & GSR_A_TXRDY) == 0) \
			; \
		uartaddr->a.txfifo[0] = *str++; \
	}

#define PUTDIGIT(digit,shift) \
	while ((uartaddr->a.gsr & GSR_A_TXRDY) == 0) \
		; \
	uartaddr->a.txfifo[0] = digits[((digit >> shift) & 0xF)];

#define PUTHEXNUM(hexnum) \
	PUTDIGIT(hexnum,28); \
	PUTDIGIT(hexnum,24); \
	PUTDIGIT(hexnum,20); \
	PUTDIGIT(hexnum,16); \
	PUTDIGIT(hexnum,12); \
	PUTDIGIT(hexnum,8); \
	PUTDIGIT(hexnum,4); \
	PUTDIGIT(hexnum,0);

/*
 * Initial Register definitions
 */

extern u_char UARTspeeds[];
