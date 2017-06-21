/* $Id: x_fpc.h,v 3.2 1995/11/17 17:46:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/des/x_fpc.h,v $
 *------------------------------------------------------------------
 * x_fpc.h - DES definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x_fpc.h,v $
 * Revision 3.2  1995/11/17  17:46:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/* Generated 9/23/94 4:18:36 */
/* This file is for 16 & 32 bit LSB & MSB systems. */

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#if		defined( CSF_CFG0_M_LSBF ) && CSF_CFG0_M_LSBF

	L2 = 0; R2 = 0;
/* FP operations */
/* first left to left */
	 if (L1 & (((csf_ui32)1)<< 7))	 L2 |= ((csf_ui32)1)<< 6;
	 if (L1 & (((csf_ui32)1)<<15))	 L2 |= ((csf_ui32)1)<< 4;
	 if (L1 & (((csf_ui32)1)<<23))	 L2 |= ((csf_ui32)1)<< 2;
	 if (L1 & (((csf_ui32)1)<<31))	 L2 |= ((csf_ui32)1)<< 0;
	 if (L1 & (((csf_ui32)1)<< 6))	 L2 |= ((csf_ui32)1)<<14;
	 if (L1 & (((csf_ui32)1)<<14))	 L2 |= ((csf_ui32)1)<<12;
	 if (L1 & (((csf_ui32)1)<<22))	 L2 |= ((csf_ui32)1)<<10;
	 if (L1 & (((csf_ui32)1)<<30))	 L2 |= ((csf_ui32)1)<< 8;
	 if (L1 & (((csf_ui32)1)<< 5))	 L2 |= ((csf_ui32)1)<<22;
	 if (L1 & (((csf_ui32)1)<<13))	 L2 |= ((csf_ui32)1)<<20;
	 if (L1 & (((csf_ui32)1)<<21))	 L2 |= ((csf_ui32)1)<<18;
	 if (L1 & (((csf_ui32)1)<<29))	 L2 |= ((csf_ui32)1)<<16;
	 if (L1 & (((csf_ui32)1)<< 4))	 L2 |= ((csf_ui32)1)<<30;
	 if (L1 & (((csf_ui32)1)<<12))	 L2 |= ((csf_ui32)1)<<28;
	 if (L1 & (((csf_ui32)1)<<20))	 L2 |= ((csf_ui32)1)<<26;
	 if (L1 & (((csf_ui32)1)<<28))	 L2 |= ((csf_ui32)1)<<24;


/* now from right to left */
	 if (R1 & (((csf_ui32)1)<< 7))	 L2 |= ((csf_ui32)1)<< 7;
	 if (R1 & (((csf_ui32)1)<<15))	 L2 |= ((csf_ui32)1)<< 5;
	 if (R1 & (((csf_ui32)1)<<23))	 L2 |= ((csf_ui32)1)<< 3;
	 if (R1 & (((csf_ui32)1)<<31))	 L2 |= ((csf_ui32)1)<< 1;
	 if (R1 & (((csf_ui32)1)<< 6))	 L2 |= ((csf_ui32)1)<<15;
	 if (R1 & (((csf_ui32)1)<<14))	 L2 |= ((csf_ui32)1)<<13;
	 if (R1 & (((csf_ui32)1)<<22))	 L2 |= ((csf_ui32)1)<<11;
	 if (R1 & (((csf_ui32)1)<<30))	 L2 |= ((csf_ui32)1)<< 9;
	 if (R1 & (((csf_ui32)1)<< 5))	 L2 |= ((csf_ui32)1)<<23;
	 if (R1 & (((csf_ui32)1)<<13))	 L2 |= ((csf_ui32)1)<<21;
	 if (R1 & (((csf_ui32)1)<<21))	 L2 |= ((csf_ui32)1)<<19;
	 if (R1 & (((csf_ui32)1)<<29))	 L2 |= ((csf_ui32)1)<<17;
	 if (R1 & (((csf_ui32)1)<< 4))	 L2 |= ((csf_ui32)1)<<31;
	 if (R1 & (((csf_ui32)1)<<12))	 L2 |= ((csf_ui32)1)<<29;
	 if (R1 & (((csf_ui32)1)<<20))	 L2 |= ((csf_ui32)1)<<27;
	 if (R1 & (((csf_ui32)1)<<28))	 L2 |= ((csf_ui32)1)<<25;

/* now from left to right */
	 if (L1 & (((csf_ui32)1)<< 3))	 R2 |= ((csf_ui32)1)<< 6;
	 if (L1 & (((csf_ui32)1)<<11))	 R2 |= ((csf_ui32)1)<< 4;
	 if (L1 & (((csf_ui32)1)<<19))	 R2 |= ((csf_ui32)1)<< 2;
	 if (L1 & (((csf_ui32)1)<<27))	 R2 |= ((csf_ui32)1)<< 0;
	 if (L1 & (((csf_ui32)1)<< 2))	 R2 |= ((csf_ui32)1)<<14;
	 if (L1 & (((csf_ui32)1)<<10))	 R2 |= ((csf_ui32)1)<<12;
	 if (L1 & (((csf_ui32)1)<<18))	 R2 |= ((csf_ui32)1)<<10;
	 if (L1 & (((csf_ui32)1)<<26))	 R2 |= ((csf_ui32)1)<< 8;
	 if (L1 & (((csf_ui32)1)<< 1))	 R2 |= ((csf_ui32)1)<<22;
	 if (L1 & (((csf_ui32)1)<< 9))	 R2 |= ((csf_ui32)1)<<20;
	 if (L1 & (((csf_ui32)1)<<17))	 R2 |= ((csf_ui32)1)<<18;
	 if (L1 & (((csf_ui32)1)<<25))	 R2 |= ((csf_ui32)1)<<16;
	 if (L1 & (((csf_ui32)1)<< 0))	 R2 |= ((csf_ui32)1)<<30;
	 if (L1 & (((csf_ui32)1)<< 8))	 R2 |= ((csf_ui32)1)<<28;
	 if (L1 & (((csf_ui32)1)<<16))	 R2 |= ((csf_ui32)1)<<26;
	 if (L1 & (((csf_ui32)1)<<24))	 R2 |= ((csf_ui32)1)<<24;

/* last from right to right */
	 if (R1 & (((csf_ui32)1)<< 3))	 R2 |= ((csf_ui32)1)<< 7;
	 if (R1 & (((csf_ui32)1)<<11))	 R2 |= ((csf_ui32)1)<< 5;
	 if (R1 & (((csf_ui32)1)<<19))	 R2 |= ((csf_ui32)1)<< 3;
	 if (R1 & (((csf_ui32)1)<<27))	 R2 |= ((csf_ui32)1)<< 1;
	 if (R1 & (((csf_ui32)1)<< 2))	 R2 |= ((csf_ui32)1)<<15;
	 if (R1 & (((csf_ui32)1)<<10))	 R2 |= ((csf_ui32)1)<<13;
	 if (R1 & (((csf_ui32)1)<<18))	 R2 |= ((csf_ui32)1)<<11;
	 if (R1 & (((csf_ui32)1)<<26))	 R2 |= ((csf_ui32)1)<< 9;
	 if (R1 & (((csf_ui32)1)<< 1))	 R2 |= ((csf_ui32)1)<<23;
	 if (R1 & (((csf_ui32)1)<< 9))	 R2 |= ((csf_ui32)1)<<21;
	 if (R1 & (((csf_ui32)1)<<17))	 R2 |= ((csf_ui32)1)<<19;
	 if (R1 & (((csf_ui32)1)<<25))	 R2 |= ((csf_ui32)1)<<17;
	 if (R1 & (((csf_ui32)1)<< 0))	 R2 |= ((csf_ui32)1)<<31;
	 if (R1 & (((csf_ui32)1)<< 8))	 R2 |= ((csf_ui32)1)<<29;
	 if (R1 & (((csf_ui32)1)<<16))	 R2 |= ((csf_ui32)1)<<27;
	 if (R1 & (((csf_ui32)1)<<24))	 R2 |= ((csf_ui32)1)<<25;
#elif	defined( CSF_CFG0_M_MSBF ) && CSF_CFG0_M_MSBF

	L2 = 0; R2 = 0;
/* FP operations */
/* first left to left */
	 if (L1 & (((csf_ui32)1)<< 7))	 L2 |= ((csf_ui32)1)<<30;
	 if (L1 & (((csf_ui32)1)<<15))	 L2 |= ((csf_ui32)1)<<28;
	 if (L1 & (((csf_ui32)1)<<23))	 L2 |= ((csf_ui32)1)<<26;
	 if (L1 & (((csf_ui32)1)<<31))	 L2 |= ((csf_ui32)1)<<24;
	 if (L1 & (((csf_ui32)1)<< 6))	 L2 |= ((csf_ui32)1)<<22;
	 if (L1 & (((csf_ui32)1)<<14))	 L2 |= ((csf_ui32)1)<<20;
	 if (L1 & (((csf_ui32)1)<<22))	 L2 |= ((csf_ui32)1)<<18;
	 if (L1 & (((csf_ui32)1)<<30))	 L2 |= ((csf_ui32)1)<<16;
	 if (L1 & (((csf_ui32)1)<< 5))	 L2 |= ((csf_ui32)1)<<14;
	 if (L1 & (((csf_ui32)1)<<13))	 L2 |= ((csf_ui32)1)<<12;
	 if (L1 & (((csf_ui32)1)<<21))	 L2 |= ((csf_ui32)1)<<10;
	 if (L1 & (((csf_ui32)1)<<29))	 L2 |= ((csf_ui32)1)<< 8;
	 if (L1 & (((csf_ui32)1)<< 4))	 L2 |= ((csf_ui32)1)<< 6;
	 if (L1 & (((csf_ui32)1)<<12))	 L2 |= ((csf_ui32)1)<< 4;
	 if (L1 & (((csf_ui32)1)<<20))	 L2 |= ((csf_ui32)1)<< 2;
	 if (L1 & (((csf_ui32)1)<<28))	 L2 |= ((csf_ui32)1)<< 0;


/* now from right to left */
	 if (R1 & (((csf_ui32)1)<< 7))	 L2 |= ((csf_ui32)1)<<31;
	 if (R1 & (((csf_ui32)1)<<15))	 L2 |= ((csf_ui32)1)<<29;
	 if (R1 & (((csf_ui32)1)<<23))	 L2 |= ((csf_ui32)1)<<27;
	 if (R1 & (((csf_ui32)1)<<31))	 L2 |= ((csf_ui32)1)<<25;
	 if (R1 & (((csf_ui32)1)<< 6))	 L2 |= ((csf_ui32)1)<<23;
	 if (R1 & (((csf_ui32)1)<<14))	 L2 |= ((csf_ui32)1)<<21;
	 if (R1 & (((csf_ui32)1)<<22))	 L2 |= ((csf_ui32)1)<<19;
	 if (R1 & (((csf_ui32)1)<<30))	 L2 |= ((csf_ui32)1)<<17;
	 if (R1 & (((csf_ui32)1)<< 5))	 L2 |= ((csf_ui32)1)<<15;
	 if (R1 & (((csf_ui32)1)<<13))	 L2 |= ((csf_ui32)1)<<13;
	 if (R1 & (((csf_ui32)1)<<21))	 L2 |= ((csf_ui32)1)<<11;
	 if (R1 & (((csf_ui32)1)<<29))	 L2 |= ((csf_ui32)1)<< 9;
	 if (R1 & (((csf_ui32)1)<< 4))	 L2 |= ((csf_ui32)1)<< 7;
	 if (R1 & (((csf_ui32)1)<<12))	 L2 |= ((csf_ui32)1)<< 5;
	 if (R1 & (((csf_ui32)1)<<20))	 L2 |= ((csf_ui32)1)<< 3;
	 if (R1 & (((csf_ui32)1)<<28))	 L2 |= ((csf_ui32)1)<< 1;

/* now from left to right */
	 if (L1 & (((csf_ui32)1)<< 3))	 R2 |= ((csf_ui32)1)<<30;
	 if (L1 & (((csf_ui32)1)<<11))	 R2 |= ((csf_ui32)1)<<28;
	 if (L1 & (((csf_ui32)1)<<19))	 R2 |= ((csf_ui32)1)<<26;
	 if (L1 & (((csf_ui32)1)<<27))	 R2 |= ((csf_ui32)1)<<24;
	 if (L1 & (((csf_ui32)1)<< 2))	 R2 |= ((csf_ui32)1)<<22;
	 if (L1 & (((csf_ui32)1)<<10))	 R2 |= ((csf_ui32)1)<<20;
	 if (L1 & (((csf_ui32)1)<<18))	 R2 |= ((csf_ui32)1)<<18;
	 if (L1 & (((csf_ui32)1)<<26))	 R2 |= ((csf_ui32)1)<<16;
	 if (L1 & (((csf_ui32)1)<< 1))	 R2 |= ((csf_ui32)1)<<14;
	 if (L1 & (((csf_ui32)1)<< 9))	 R2 |= ((csf_ui32)1)<<12;
	 if (L1 & (((csf_ui32)1)<<17))	 R2 |= ((csf_ui32)1)<<10;
	 if (L1 & (((csf_ui32)1)<<25))	 R2 |= ((csf_ui32)1)<< 8;
	 if (L1 & (((csf_ui32)1)<< 0))	 R2 |= ((csf_ui32)1)<< 6;
	 if (L1 & (((csf_ui32)1)<< 8))	 R2 |= ((csf_ui32)1)<< 4;
	 if (L1 & (((csf_ui32)1)<<16))	 R2 |= ((csf_ui32)1)<< 2;
	 if (L1 & (((csf_ui32)1)<<24))	 R2 |= ((csf_ui32)1)<< 0;

/* last from right to right */
	 if (R1 & (((csf_ui32)1)<< 3))	 R2 |= ((csf_ui32)1)<<31;
	 if (R1 & (((csf_ui32)1)<<11))	 R2 |= ((csf_ui32)1)<<29;
	 if (R1 & (((csf_ui32)1)<<19))	 R2 |= ((csf_ui32)1)<<27;
	 if (R1 & (((csf_ui32)1)<<27))	 R2 |= ((csf_ui32)1)<<25;
	 if (R1 & (((csf_ui32)1)<< 2))	 R2 |= ((csf_ui32)1)<<23;
	 if (R1 & (((csf_ui32)1)<<10))	 R2 |= ((csf_ui32)1)<<21;
	 if (R1 & (((csf_ui32)1)<<18))	 R2 |= ((csf_ui32)1)<<19;
	 if (R1 & (((csf_ui32)1)<<26))	 R2 |= ((csf_ui32)1)<<17;
	 if (R1 & (((csf_ui32)1)<< 1))	 R2 |= ((csf_ui32)1)<<15;
	 if (R1 & (((csf_ui32)1)<< 9))	 R2 |= ((csf_ui32)1)<<13;
	 if (R1 & (((csf_ui32)1)<<17))	 R2 |= ((csf_ui32)1)<<11;
	 if (R1 & (((csf_ui32)1)<<25))	 R2 |= ((csf_ui32)1)<< 9;
	 if (R1 & (((csf_ui32)1)<< 0))	 R2 |= ((csf_ui32)1)<< 7;
	 if (R1 & (((csf_ui32)1)<< 8))	 R2 |= ((csf_ui32)1)<< 5;
	 if (R1 & (((csf_ui32)1)<<16))	 R2 |= ((csf_ui32)1)<< 3;
	 if (R1 & (((csf_ui32)1)<<24))	 R2 |= ((csf_ui32)1)<< 1;

#else
 error -- LSB-MSB not defined
#endif
