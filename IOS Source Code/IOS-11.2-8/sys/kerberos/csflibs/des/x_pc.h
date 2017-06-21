/* $Id: x_pc.h,v 3.2 1995/11/17 17:46:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/des/x_pc.h,v $
 *------------------------------------------------------------------
 * x_pc.h - DES definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x_pc.h,v $
 * Revision 3.2  1995/11/17  17:46:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/* Generated 9/23/94 4:32:21 */
/* This file is for 16 & 32 bit LSB & MSB systems. */

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#if		defined( CSF_CFG0_M_LSBF ) && CSF_CFG0_M_LSBF

	L2 = 0;
	P_temp = R1;
	P_temp_p = (unsigned char *) &P_temp;
	R2 = P_prime[0][*P_temp_p++];
	R2 |= P_prime[1][*P_temp_p++];
	R2 |= P_prime[2][*P_temp_p++];
	R2 |= P_prime[3][*P_temp_p];

#elif	defined( CSF_CFG0_M_MSBF ) && CSF_CFG0_M_MSBF

	L2 = 0;
	P_temp = R1;
	P_temp_p = (unsigned char *) &P_temp;
	R2 = P_prime[3][*P_temp_p++];
	R2 |= P_prime[2][*P_temp_p++];
	R2 |= P_prime[1][*P_temp_p++];
	R2 |= P_prime[0][*P_temp_p];

#else
 error -- LSB-MSB not defined
#endif
