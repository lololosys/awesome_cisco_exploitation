/* $Id: cisco.h,v 3.3 1996/01/04 22:52:57 che Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/kerberos/include/csfc5/config/cisco.h,v $
 *------------------------------------------------------------------
 * cisco.h - Cisco-specific CSFC5 definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco.h,v $
 * Revision 3.3  1996/01/04  22:52:57  che
 * CSCdi46133:  Kerberos KDC port should be configurable
 * Added "kerberos kdc-port <port number>" command.
 *
 * Revision 3.2  1995/11/17  17:49:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_CONFIG_ES_H_
#define _CSFC5_CONFIG_ES_H_ "%W% %G%"
#endif

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All rights reserved.
 */

/*---------------------------------------------*/
#ifdef CSFC5_BUILD_BEGIN
#endif /* CSFC5_BUILD_BEGIN */
/*---------------------------------------------*/

/*---------------------------------------------*/
#ifdef CSFC5_BUILD_CFG

#define CSFC5_CFG_CLOCKSKEW				(5 * 60)	/* Five minutes */
#define CSFC5_CFG_KDCREQ_DEFCKS_T		CSFC5c_CKS_T_RSA_MD4
#define CSFC5_CFG_KDCREQ_DEFOPT			CSFC5c_KDCOPT_RENEWABLE_OK_m

#define CSFC5_CFG_KT_MEM				1
#define CSFC5_CFG_KTDEF					"MEM:"
#define CSFC5_CFG_KTDEFTYPE				"MEM"

#define CSFC5_CFG_RC_MEM				1
#define CSFC5_CFG_RCDEF					"MEM:"
#define CSFC5_CFG_RCDEFTYPE				"MEM"

#define CSFC5_CFG_CC_MEM				1
#define CSFC5_CFG_CCDEF					"MEM:"
#define CSFC5_CFG_CCDEFTYPE				"MEM"

#define CSFC5_CFG_NET_SKDC_TIMEOUT		15000
#define CSFC5_CFG_NET_SKDC_MAX_RETRY	4
#define CSFC5_CFG_NET_SKDC_MAX_KDC		3

#define CSFC5_CFG_CRS_DES_CBC_CRC		1
#define CSFC5_CFG_CRS_DES_RAW_CBC		0

#define CSFC5_CFG_CKS_CRC32				1
#define CSFC5_CFG_CKS_DES_CBC_CKSUM		1
#define CSFC5_CFG_CKS_RSA_MD4			1
#define CSFC5_CFG_CKS_RSA_MD5			1

#endif /* CSFC5_BUILD_CFG */
/*---------------------------------------------*/

/*---------------------------------------------*/
#ifdef CSFC5_BUILD_END
#endif /* CSFC5_BUILD_END */
/*---------------------------------------------*/
