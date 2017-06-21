/* $Id: timex.c,v 3.2 1995/11/17 17:45:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libz/timex.c,v $
 *------------------------------------------------------------------
 * timex.c - Misc. time functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: timex.c,v $
 * Revision 3.2  1995/11/17  17:45:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All rights reserved.
 */

#include <csfc5/header.h>

static csf_boolean inskew (
	csfC5_tDelta dtime
){

	if (csfCabs( dtime ) < (csfC5_tDelta) csfC5getcfgnum( CSFC5_CFG_CLOCKSKEW )) {
		return TRUE;
	}
	else {
		return FALSE;
	}

}

csf_sts csfC5getNow_tStamp (
	csfC5_tStamp *ts
){
	csfCmemzero( ts, sizeof(*ts) );
	return csfCsecTOD( ts );
}

int csfC5cmp_tStamp (
	csfC5_tStamp ts1,
	csfC5_tStamp ts2
){
	return (ts1 - ts2);
}

int csfC5cmpSkewed_tStamp (
	csfC5_tStamp ts1,
	csfC5_tStamp ts2
){
	csfC5_tDelta tdelta;

	tdelta = ts1 - ts2;

	if (inskew( tdelta )){
		tdelta = 0;
	}

	return tdelta;

}

int csfC5cmpSkewedNow_tStamp (
	csfC5_tStamp ts
){
	csfC5_tStamp tnow;
	csf_sts rsts;

	rsts = csfC5getNow_tStamp( &tnow );
	if (rsts) {
		return -1;
	}

	return csfC5cmpSkewed_tStamp( tnow, ts );

}

csf_sts csfC5get_usTStamp (
	csfC5_usTStamp *ust
){
	csfCmemzero( ust, sizeof(*ust) );
	return csfCusTOD( &ust->secs, &ust->usecs );
}

csf_sts csfC5getUnique_usTStamp (
	csfC5_usTStamp *ust
){
	csfCmemzero( ust, sizeof(*ust) );
	return csfCuusTOD( &ust->secs, &ust->usecs );
}
