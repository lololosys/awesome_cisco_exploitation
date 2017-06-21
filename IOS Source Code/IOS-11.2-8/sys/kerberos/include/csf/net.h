/* $Id: net.h,v 3.2.60.1 1996/04/19 15:12:31 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/include/csf/net.h,v $
 *------------------------------------------------------------------
 * net.h - Generic network definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: net.h,v $
 * Revision 3.2.60.1  1996/04/19  15:12:31  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.3  1996/04/16  03:54:55  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.2  1996/04/02  08:26:28  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:47:15  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:47:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_NET_H_
#define _CSF_NET_H_ "%W% %G%"
/*.original hostaddr.h 3.2 2/2/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#ifndef CSF_CFG_NET_MAXHOSTNLEN
#define CSF_CFG_NET_MAXHOSTNLEN			64
#endif

#ifndef CSF_CFG_NET_MAXDOMAINNLEN
#define CSF_CFG_NET_MAXDOMAINNLEN		100
#endif

#ifndef CSF_CFG_NET_MAX_DGRAM_SIZE
#define CSF_CFG_NET_MAX_DGRAM_SIZE		4096
#endif

#ifndef csfNET_handle

#define csfNET_handle tcbtype
#endif


csf_sts csfNETread (
	csfNET_handle *neth,
	const void *indata,
	const csf_size_t len,
	csf_size_t *rlen
);

csf_sts csfNETwrite (
	csfNET_handle *neth,
	void *outdata,
	const csf_size_t len,
	csf_size_t *rlen
);


csf_sts csfNETgetFullHostName (
	const char *name,
	char *rname,
	const csf_size_t maxlen
);

csf_sts csfNETgetMyHostName (
	char *rlen,
	const csf_size_t maxlen
);


#ifndef csfNETntoh16
csf_ui16 csfNETntoh16 ( csf_ui16 );
#endif

#ifndef csfNEThton16
csf_ui16 csfNEThton16 ( csf_ui16 );
#endif

#ifndef csfNEThton32
csf_ui32 csfNEThton32 ( csf_ui32 );
#endif

#ifndef csfNETntoh32
csf_ui32 csfNETntoh32 ( csf_ui32 );
#endif

#endif /* _CSF_NET_H_ */
