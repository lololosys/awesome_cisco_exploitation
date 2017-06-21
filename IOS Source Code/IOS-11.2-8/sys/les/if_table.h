/* $Id: if_table.h,v 3.1 1995/11/09 12:33:21 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/if_table.h,v $
 *------------------------------------------------------------------
 * if_table.h -- mapping between interface value and interface bit field
 *
 * September 1995, Eric Fronberg
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_table.h,v $
 * Revision 3.1  1995/11/09  12:33:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/23  05:57:26  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.1  1995/09/23  05:35:52  enf
 * placeholder for CSCdi39189
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_TABLE_H__
#define __IF_TABLE_H__

/* bit masks for interface types */
#define ENET_BIT_MASK 		0x00000001
#define SERIAL_BIT_MASK		0x00000002
#define TRING_BIT_MASK		0x00000004
#define BRI_BIT_MASK		0x00000008
#define ASYNC_BIT_MASK		0x00000010



const unsigned short interface_bit_table[] =
{
    /* as specified in .../boot/src-68-c1000/cookie.h */
    0,							/* 0 = undefined */
    ENET_BIT_MASK | SERIAL_BIT_MASK, 			/* 1 = 1E1T */
    ENET_BIT_MASK,					/* 2 = 2E */
    TRING_BIT_MASK | SERIAL_BIT_MASK,			/* 3 = 1R1T */
    TRING_BIT_MASK | ENET_BIT_MASK,			/* 4 = 1R1E */
    TRING_BIT_MASK,					/* 5 = 2R */
    ENET_BIT_MASK | BRI_BIT_MASK,			/* 6 = 1E1B */
    ENET_BIT_MASK | SERIAL_BIT_MASK,			/* 7 = 1E2T */
    TRING_BIT_MASK | SERIAL_BIT_MASK,			/* 8 = 1R2T */
    ENET_BIT_MASK | SERIAL_BIT_MASK | BRI_BIT_MASK,	/* 9 = 1E1T1B */
    TRING_BIT_MASK | SERIAL_BIT_MASK | BRI_BIT_MASK,	/* 10 = 1R1T1B */
    ENET_BIT_MASK | SERIAL_BIT_MASK | BRI_BIT_MASK,	/* 11 = 1E2T1B */
    TRING_BIT_MASK | SERIAL_BIT_MASK | BRI_BIT_MASK,	/* 12 = 1R2T1B */
    TRING_BIT_MASK | BRI_BIT_MASK,			/* 13 = 1R1B */
    ENET_BIT_MASK | SERIAL_BIT_MASK | ASYNC_BIT_MASK,	/* 14 = 1E2T8A */
    TRING_BIT_MASK | SERIAL_BIT_MASK |ASYNC_BIT_MASK,	/* 15 = 1R2T8A */
    ENET_BIT_MASK | SERIAL_BIT_MASK,			/* 16 = 1E1T1I */
    TRING_BIT_MASK | SERIAL_BIT_MASK,			/* 17 = 1R1T1I */
    TRING_BIT_MASK | SERIAL_BIT_MASK | ENET_BIT_MASK,	/* 18 = 1R2T1E */
    TRING_BIT_MASK | SERIAL_BIT_MASK,			/* 19 = 2R2T */
    ENET_BIT_MASK | SERIAL_BIT_MASK,			/* 20 = 2E2T */
    ENET_BIT_MASK | SERIAL_BIT_MASK,			/* 21 = 1E2Thub */
    0,							/* 22 = undefined */
    ENET_BIT_MASK | SERIAL_BIT_MASK | BRI_BIT_MASK,	/* 23 = 1E2T1Bhub */
    ENET_BIT_MASK | BRI_BIT_MASK,			/* 24 = 1E1N */
    ENET_BIT_MASK | SERIAL_BIT_MASK | BRI_BIT_MASK,	/* 25 = 1E2T1B2AS */
    TRING_BIT_MASK | SERIAL_BIT_MASK | BRI_BIT_MASK	/* 26 = 1R2T1B2AS */
};

#endif /*  ifndef(__IF_TABLE_H__) */

