/* $Id: netwmsg.c,v 3.2.60.1 1996/04/19 15:12:06 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/net/netwmsg.c,v $
 *------------------------------------------------------------------
 * netwmsg.c - Writes a message to network
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netwmsg.c,v $
 * Revision 3.2.60.1  1996/04/19  15:12:06  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.3  1996/04/16  03:54:18  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.2  1996/04/02  08:13:39  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:46:34  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:45:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original write_msg.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

csf_sts csfC5netWriteMsg (
	csfNET_handle *fd,
	csfC5_data *outbuf
){
	csf_ui32 len;
	csf_size_t alen;
	csf_sts rsts;

	len = outbuf->dl;
	rsts = csfNETwrite( fd, &len, sizeof(len), &alen );
	if (rsts) {
		return rsts;
	}
	if (alen != sizeof(len)) {
		return CSFC5_STS_ENETFRAGWRITE;
	}

	if (len) {
		rsts = csfNETwrite( fd, outbuf->dp, outbuf->dl, &alen );
		if (rsts) {
			return rsts;
		}
		if (alen != outbuf->dl) {
			return CSFC5_STS_ENETFRAGWRITE;
		}
	}

	return CSF_STS_OK;
}
