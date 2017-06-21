/* $Id: appx.h,v 3.2.60.1 1996/04/19 15:12:37 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/include/csfc5/appx.h,v $
 *------------------------------------------------------------------
 * appx.h - Application authenticator definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appx.h,v $
 * Revision 3.2.60.1  1996/04/19  15:12:37  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.2  1996/04/02  08:37:57  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:47:28  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:47:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_APPX_H_
#define _CSFC5_APPX_H_   "%W% %G%"
/*.original func-proto.h 3.8 5/22/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */


#define CSFC5_APPAUTH_VERSION				"KRB5_SENDAUTH_V1.0"
#define CSFC5_APPAUTH_RESPONSE_OK				((csf_o)0)
#define CSFC5_APPAUTH_RESPONSE_BADAUTHVERS		((csf_o)1)
#define CSFC5_APPAUTH_RESPONSE_BADAPPLVERS		((csf_o)2)

csf_sts csfC5sendAppXAuth (
	csfNET_handle *neth,
	char *appl_version,
	csfC5_prnId *client,
	csfC5_tktFlags apreq_opts,
	csfC5_cksBlock *cksp,
	csfC5_creds *creds, /*in-out*/
	csfC5_seqNo *seqno, /*out*/
	csfC5_crsKBlock *newkey, /*out*/
	csfC5_msgErr *error, /*out*/
	csfC5_msgApRepEPart *rep_result, /*out*/
	int do_telnet, /* in */
	char **buf, /* out */
	int *buflen, /* out */
	long *sec, /* out */
	long *usec /* out */
);


csf_sts csfC5recvAppXAuth (
	csfNET_handle *fd,
	char *appl_version,
	csfC5_prnId *server,
	csfC5_netHAddr *s_haddr,
	csfC5readKeyProc_msgApReq keyproc,
	void *keyprocarg,
	csfC5_rc *rc,
	csfC5_seqNo *seqno, /*out*/
	csfC5_prnId *client, /*out*/
	csfC5_tkt *tkt, /*out*/
	csfC5_autht *autht, /*out*/
	char *authdata, /* in */
	int authdata_len, /* in */
	char **mutual_data, /* out */
	int *mutual_data_len /* out */
);


#define CSFC5_APPX_TIMESTAMP	0x01
#define CSFC5_APPX_SEQUENCE		0x02
#define CSFC5_APPX_REPLAY		0x04 /*TBD/JNK--unused*/

csf_sts csfC5makeAppXPrivMsg (
	const csfC5_data *,
	const csfC5_crsEType,
	const csfC5_crsKBlock *,
	const csfC5_netHAddr *,
	const csfC5_netHAddr *,
	csf_ui32,
	csf_ui32,
	void *,
	csfC5_data *
);

csf_sts csfC5readAppXPrivMsg (
	const csfC5_data *,
	const csfC5_crsKBlock *,
	const csfC5_netHAddr *,
	const csfC5_netHAddr *,
	csf_ui32,
	csf_ui32,
	void *,
	csfC5_rc *,
	csfC5_data *
);

csf_sts csfC5makeAppXSafeMsg (
	const csfC5_data *userdata,
	const csfC5_cksType ckstype,
	const csfC5_crsKBlock *key,
	const csfC5_netHAddr *s_haddr,
	const csfC5_netHAddr *r_haddr,
	csfC5_seqNo seqno,
	csf_flags appxflags,
	csfC5_data *outbuf
);

csf_sts csfC5readAppXSafeMsg (
	const csfC5_data *inbuf,
	const csfC5_crsKBlock *key,
	const csfC5_netHAddr *s_haddr,
	const csfC5_netHAddr *r_haddr,
	csfC5_seqNo seqno,
	csf_flags appxflags,
	csfC5_rc *rc,
	csfC5_data *outbuf
);

#endif
