/* $Id: crc.h,v 3.2 1995/11/17 17:47:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/crc.h,v $
 *------------------------------------------------------------------
 * crc.h - CRC32 definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crc.h,v $
 * Revision 3.2  1995/11/17  17:47:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_CRC_H_
#define _CSF_CRC_H_  "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

/*
 * CRC -- Cyclic-redundancy-check functions.
 */


extern csf_ui32 const csfCRC32AD2Tab0[];

csf_sts csfCRC32sum (
	const void *crctab,
	const void *in,
	csf_size_t len,
	csf_ui32 *cks
);

csf_sts csfCRC32sumAD2 (
	const void *in,
	csf_size_t len,
	csf_ui32 *cks
);

#endif
