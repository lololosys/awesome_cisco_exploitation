/* $Id: netrmsg.c,v 3.2.60.1 1996/04/19 15:12:05 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/net/netrmsg.c,v $
 *------------------------------------------------------------------
 * netrmsg.c - Reads a message from network
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netrmsg.c,v $
 * Revision 3.2.60.1  1996/04/19  15:12:05  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.3  1996/04/16  03:54:16  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.2  1996/04/02  08:13:38  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:46:32  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:45:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original read_msg.c 3.3 4/14/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

csf_sts csfC5netReadMsg (
	csfNET_handle *fd,
	csfC5_data *inbuf
){
	csf_ui32 len;
	csf_size_t len2;
	csf_o *buf = NULL;
	csf_sts rsts;

	_csfDBGassert( inbuf );

	rsts = csfNETread( fd, &len, sizeof(len), &len2 );
	if (rsts) {
		return rsts;
	}
	if (len2 != sizeof(len)) {
		return CSFC5_STS_ENETFRAGREAD;
	}

	if (len) {
		/*
		 * We may want to include a sanity check here someday....
		 */
		buf = csfCmalloc( len + 8 );	/*TBD/JNK "len + 8"? */
		if (!buf) {
			return CSF_STS_ENOMEM;
		}
		rsts = csfNETread( fd, buf, len, &len2 );
		if (rsts) {
			csfCfree( buf );
		}
		else if (len2 != len) {
			csfCfree( buf );
			rsts = CSFC5_STS_ENETFRAGREAD;
		}
	}
	/*TBD/JNK--set length & data even on an error?*/
	inbuf->dl = len;
	inbuf->dp = buf;

	return rsts;
}

