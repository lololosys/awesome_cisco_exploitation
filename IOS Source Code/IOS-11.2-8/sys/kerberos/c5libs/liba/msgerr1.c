/* $Id: msgerr1.c,v 3.2 1995/11/17 17:44:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/msgerr1.c,v $
 *------------------------------------------------------------------
 * msgerr1.c - Functions to manipulate msgerr structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msgerr1.c,v $
 * Revision 3.2  1995/11/17  17:44:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */


#include <csfc5/header.h>


csf_sts csfC5rd_msgErr (
	const csfC5_data *enc_errbuf,
	csfC5_msgErr *dec_error
){
	csf_sts rsts;

	rsts = csfC5asnD_msgErr( enc_errbuf, dec_error );

	return rsts;
}


csf_sts csfC5mk_msgErr (
	const csfC5_msgErr *dec_err,
	csfC5_data *enc_err
){
	csf_sts rsts;

	rsts = csfC5asnE_msgErr( dec_err, enc_err );

	return rsts;
}
