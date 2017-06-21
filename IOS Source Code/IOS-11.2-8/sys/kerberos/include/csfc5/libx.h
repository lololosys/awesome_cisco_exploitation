/* $Id: libx.h,v 3.2 1995/11/17 17:48:36 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/libx.h,v $
 *------------------------------------------------------------------
 * libx.h - Libx definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: libx.h,v $
 * Revision 3.2  1995/11/17  17:48:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_LIBX_H_
#define _CSFC5_LIBX_H_   "%W% %G%"
/*.original func-proto.h 3.8 5/22/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */


#define CSFC5_GC_USER_USER	1	/* want user-user ticket */
#define CSFC5_GC_CACHED		2	/* want cached ticket only */

csf_sts csfC5get_creds (
	csf_flags gcopts,
	csfC5_credsReqCtx *creq,
	csfC5_cc *cc,
	csfC5_creds *outcreds
);

csf_sts csfC5getStupid_creds (
	const csfC5_prnId *server,
	csfC5_cc *cc,
	csfC5_creds *creds
);

csf_sts csfC5getFromKdc_creds (
	csfC5_credsReqCtx *creq,
	csfC5_cc *cc,
	csfC5_creds *outcreds
);

csf_sts csfC5getViaTgt_creds (
	csfC5_credsReqCtx *creq,
	csfC5_creds *outcreds
);

csf_sts csfC5make_msgTgsReq (
	csfC5_credsReqCtx *creq,
	csfC5_data *req
);

csf_sts csfC5make_msgKdcReqBody (
	csfC5_credsReqCtx *creq,
	csfC5_data *req
);

typedef csf_sts (*csfC5pauthKeyProc) (
	const csfC5_crsKType,
	csfC5_crsKBlock *,
	void const *,
	csfC5_pauth **
);

csf_sts csfC5make_msgAsReq (
	csfC5_credsReqCtx *creq,
	csfC5_pauth *padata,
	csfC5_data *req
);

csf_sts csfC5read_msgAsRep (
	const csfC5_data *rep,
	csfC5_credsReqCtx *creq,
	csfC5pauthKeyProc key_proc,
	void const *keyseed,
	csfC5_msgKdcRep *asrep,
	csfC5_msgErr *errrep
);

csf_sts csfC5getITkt (
	const csfC5_creds *increds,
	const csfC5_pauthType pauth_type,
	const csfC5_crsEType etype,
	csfC5pauthKeyProc key_proc,
	void const *keyseed,
	csfC5_creds *outcreds,
	csfC5_cc *cc
);

csf_sts csfC5getITktPwd (
	const csfC5_creds *increds,
	const csfC5_pauthType pre_auth_type,
	const csfC5_crsEType etype,
	const char *password,
	csfC5_creds *outcreds,
	csfC5_cc *cc
);

csf_sts csfC5getITktSvcKey (
	const csfC5_creds *increds,
	const csfC5_pauthType pre_auth_type,
	const csfC5_crsEType etype,
	const csfC5_crsKBlock *key,
	csfC5_creds *outcreds,
	csfC5_cc *cc
);

csf_sts csfC5make_msgApReq (
	const csfC5_tktFlags apreq_opts,
	const csfC5_cksBlock *cks,
	const csfC5_prnId *client,
	const csfC5_crsKBlock *seskey,
	const csfC5_data *e_tkt,
	const csfC5_seqNo seqno,
	csfC5_crsKBlock *newkey,
	csfC5_autht *authtp,
	csfC5_data *e_apreq
);

typedef csf_sts (*csfC5readKeyProc_msgApReq) (
	void *,
	const csfC5_prnId *,
	csfC5_crsKVNo,
	csfC5_crsKBlock *
);

csf_sts csfC5readSvcKey_msgApReq (
	void *ktname,
	const csfC5_prnId *prn,
	csfC5_crsKVNo kvno,
	csfC5_crsKBlock *key
);

csf_sts csfC5read_msgApReq (
	const csfC5_data *,
	const csfC5_prnId *,
	const csfC5_netHAddr *,
	csfC5readKeyProc_msgApReq,
	void *,
	csfC5_rc *,
	csfC5_msgApReq *
);

csf_sts csfC5readD_msgApReq (
	csfC5_msgApReq *,
	const csfC5_prnId *,
	const csfC5_netHAddr *,
	csfC5readKeyProc_msgApReq,
	void *,
	csfC5_rc *
);


csf_sts csfC5makeSimple_msgErr (
	const csfC5_prnId *server,
	const csf_sts ests,
	csfC5_data *outbuf
);


csf_sts csfC5moveKdcRepTo_creds (
	csfC5_msgKdcRep *kdcrep,
	csfC5_creds *creds
);

csf_sts csfC5check_msgKdcRep (
	csfC5_credsReqCtx *creq,
	const csfC5_msgKdcRep *kdcrep
);

#endif
