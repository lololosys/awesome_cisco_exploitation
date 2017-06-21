/* $Id: appxrcva.c,v 3.2.60.2 1996/06/28 23:20:07 hampton Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/libx/appxrcva.c,v $
 *------------------------------------------------------------------
 * appxrcva.c - Receive application authenticator
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appxrcva.c,v $
 * Revision 3.2.60.2  1996/06/28  23:20:07  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/19  15:11:58  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.4  1996/04/16  03:54:09  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.3  1996/04/02  08:05:00  carrel
 * cvs fixes
 *
 * Revision 3.1.2.2  1996/03/12  03:12:05  che
 * Branch: IosSec_branch
 * Phase 2 Kerberos commit Part 2
 *
 * Revision 3.1.2.1  1996/03/05  04:46:19  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:44:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original recvauth.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>
#include "../ip/ip.h"
#include "../tcp/telnet.h"

static const char csfC5appxauthver0 [] = CSFC5_APPAUTH_VERSION;

#define SUBOPT_PAD 3

csf_sts csfC5recvAppXAuth (
	csfNET_handle *fd,
	char *applver,
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
){
	csf_sts rsts;
	csfC5_data inbuf;
	csfC5_msgApReq apreq;
	csfC5_data outbuf;
	csf_o response; 
	char *nbuf = NULL;

	/*
	 * First read the sendauth version string and check it.
	 */

    if (!authdata_len) {
	rsts = csfC5netReadMsg( fd, &inbuf );
	if (rsts) {
		return rsts;
	}

	if (csfCstrcmp( (char *)csfC5getPtr_data( &inbuf ), csfC5appxauthver0 )) {
		/* bad sendauth version -- notify client side of error */
		response = CSFC5_APPAUTH_RESPONSE_BADAUTHVERS;
		/* ignore errors from csfNETwrite(), we're bailing anyway */
		(void) csfNETwrite( fd, &response, 1, NULL );
		csfC5free_data( &inbuf );
		return CSFC5_STS_SENDAUTH_BADAUTHVERS;
	}

	csfC5free_data( &inbuf );


	/* Do the same thing for the application version string.
	 */
	rsts = csfC5netReadMsg( fd, &inbuf );
	if (rsts) {
		return rsts;
	}

	if (csfCstrcmp( (char *)csfC5getPtr_data( &inbuf ), applver )) {
		/* bad application version -- notify client side of error */
		response = CSFC5_APPAUTH_RESPONSE_BADAPPLVERS;
		/* ignore errors from csfNETwrite(), we're bailing anyway */
		(void) csfNETwrite( fd, &response, 1, NULL );
		csfC5free_data( &inbuf );
		return CSFC5_STS_SENDAUTH_BADAPPLVERS;
	}

	csfC5free_data( &inbuf );

	/* everything is fine so far, write ok response to client */
	response = CSFC5_APPAUTH_RESPONSE_OK;
	rsts = csfNETwrite( fd, &response, 1, NULL );
	if (rsts) {
		return rsts;
	}


	/* Read the AP_REQ message... */
	rsts = csfC5netReadMsg( fd, &inbuf );
	if (rsts) {
		return rsts;
	}
    } else {
	inbuf.dp = authdata;
	inbuf.dl = authdata_len;
    }

	rsts = csfC5read_msgApReq( &inbuf, server, s_haddr,
		keyproc, keyprocarg, rc, &apreq );

    if (!authdata_len) {
	csfC5free_data( &inbuf );
	csfC5clear_data( &outbuf );

	if (rsts) {
		rsts = csfC5makeSimple_msgErr( server, rsts, &outbuf );
		if (rsts) {
			csfC5free_msgApReq( &apreq );
			return rsts;
		}
	} 

	rsts = csfC5netWriteMsg( fd, &outbuf );

	csfC5free_data( &outbuf );
	if (rsts) {
		/*  if we sent back an error (or couldn't send!), we need to return */
		csfC5free_msgApReq( &apreq );
		return rsts;
	}
    } else {
	if (rsts) {
	    csfC5free_msgApReq( &apreq );
	    return(rsts);
	}
    }


	/*
	 * Here lies the mutual authentication stuff...
	 *
	 * We're going to compose and send a AP_REP message.
	 */

	if (apreq.ap_opts & CSFC5c_APOPTS_MUTUAL_REQUIRED_m) {
		csfC5_msgApRep repl;

		csfC5clear_msgApRep( &repl );

		if (seqno) {
			/* Generate a random sequence number */
			rsts = csfC5crsGenSeqNo( &apreq.tkt.epart.seskey, seqno );
			if (rsts) {
				csfC5free_msgApReq( &apreq );
				return rsts;
			}
		}

		repl.epart.ctime = apreq.autht.ctime;
		repl.epart.subkey = apreq.autht.subkey;
		if (seqno) {
			repl.epart.seqno = *seqno;
		}
		rsts = csfC5getEType_crsKType( apreq.tkt.epart.seskey.key_type,
			&repl.ec_epart.etype );
		if (rsts) {
			csfC5free_msgApReq( &apreq );
			return rsts;
		}

		rsts = csfC5mkEc_msgApRep( &repl, &apreq.tkt.epart.seskey, &outbuf );
		if (rsts) {
			csfC5free_msgApReq( &apreq );
			return rsts;
		}

		if (!authdata_len) {
		    rsts = csfC5netWriteMsg( fd, &outbuf );
		    csfC5free_data( &outbuf );
		    if (rsts) {
			csfC5free_msgApReq( &apreq );
			return rsts;
		    }
		}
	}

    if (apreq.ap_opts & CSFC5c_APOPTS_MUTUAL_REQUIRED_m) {
	if (authdata_len) {
	    telnet_suboption_pad_iac(outbuf.dp, outbuf.dl, &nbuf, 
				     mutual_data_len);

	    *mutual_data = malloc_named(*mutual_data_len+SUBOPT_PAD, 
						"Kerberos:csfC5recvAppXAuth()");
	    if (*mutual_data == NULL) {
		csfC5free_msgApReq( &apreq );
		free(nbuf);
		return CSFC5_STS_ENOMEM;
	    }
	    memcpy(&((*mutual_data)[3]), nbuf, *mutual_data_len);
		
	    *mutual_data_len+=SUBOPT_PAD;
	    free(nbuf);
	}
    } else {
	*mutual_data = NULL;
	*mutual_data_len = 0;
    }

	/*
	 * At this point, we've won.  We just need to copy whatever
	 * parts of the apreq structure which the user wants, clean
	 * up, and exit.
	 */

	if (client) {
		rsts = csfC5copy_prnId( &apreq.tkt.epart.client, client );
		if (rsts) {
			csfC5free_msgApReq( &apreq );
			return rsts;
		}
	}


	/*
	 * If the user wants that part of the apreq structure,
	 * return it; otherwise free it.
	 */

	if (tkt) {
		*tkt = apreq.tkt;
		csfC5clear_tkt( &apreq.tkt );
	}

	if (autht) {
		*autht = apreq.autht;
		csfC5clear_autht( &apreq.autht );
	}

	csfC5free_msgApReq( &apreq );

	return CSF_STS_OK;
}
