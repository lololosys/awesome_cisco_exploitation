/* $Id: msgerr.h,v 3.2 1995/11/17 17:48:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/msgerr.h,v $
 *------------------------------------------------------------------
 * msgerr.h - Error message definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgerr.h,v $
 * Revision 3.2  1995/11/17  17:48:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_MSGERR_H_
#define _CSFC5_MSGERR_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#ifndef csfC5_msgErrCode
typedef csf_ui32 csfC5_msgErrCode;
#endif

typedef struct _csfC5_msgErr {
	csfC5_pVNo pvno;			/* Protocol version number. */
	csfC5_msgType msg_type;	/* Message type == ERR. */
	/* some of these may be meaningless in certain contexts */
	csfC5_usTStamp ctime;		/* Client time. */
	csfC5_usTStamp stime;		/* Server time. */
	csfC5_msgErrCode errcode;	/* error code (protocol error #'s) */
	csfC5_prnId client;			/* client's principal id; optional */
	csfC5_prnId server;			/* server's principal identifier */
	csfC5_data text;			/* descriptive text */
	csfC5_data e_data;			/* additional error-describing data */
} csfC5_msgErr;

void csfC5clear_msgErr( csfC5_msgErr * );
void csfC5free_msgErr( csfC5_msgErr * );


csf_sts csfC5mk_msgErr (
	const csfC5_msgErr *,
	csfC5_data *
);

csf_sts csfC5rd_msgErr (
	const csfC5_data *,
	csfC5_msgErr *
);

#endif
