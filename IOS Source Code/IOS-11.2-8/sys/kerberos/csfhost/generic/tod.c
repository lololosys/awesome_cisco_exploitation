/* $Id: tod.c,v 3.2 1995/11/17 17:46:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csfhost/generic/tod.c,v $
 *------------------------------------------------------------------
 * tod.c - Get time of day
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tod.c,v $
 * Revision 3.2  1995/11/17  17:46:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original timeofday.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#define CSF_HDRI_ERRNO
#include <csfc5/header.h>

csf_sts csfCsecTOD (
	csf_ui32 *timeret
){
	csf_time_t tval;
	int errno = 0;

	tval = csfCtime( NULL );
	if (tval == ((csf_time_t) -1)) {
		errno = CSFC5_STS_ENOTIM;
		return _csfSTSerrno( errno );
	}
	if (timeret) {
		*timeret = tval;
	}

	return CSF_STS_OK;
}
