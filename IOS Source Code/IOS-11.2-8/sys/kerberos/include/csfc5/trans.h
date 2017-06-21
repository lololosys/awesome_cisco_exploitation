/* $Id: trans.h,v 3.2 1995/11/17 17:49:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/trans.h,v $
 *------------------------------------------------------------------
 * trans.h - Kerberos trans definition
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: trans.h,v $
 * Revision 3.2  1995/11/17  17:49:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_TRANS_H_
#define _CSFC5_TRANS_H_  "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1989,1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#ifndef csfC5_transType
typedef csf_ui8 csfC5_transType;
#endif

typedef struct _csfC5_trans {
	csfC5_transType tr_type;
	csfC5_data tr_data;
} csfC5_trans;

void csfC5clear_trans( csfC5_trans * );
void csfC5free_trans( csfC5_trans * );
csf_sts csfC5copy_trans( const csfC5_trans *, csfC5_trans * );

#endif
