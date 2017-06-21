/* $Id: appxsnda.c,v 3.2.60.2 1996/06/28 23:20:09 hampton Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/libx/appxsnda.c,v $
 *------------------------------------------------------------------
 * appxsnda.c - Send application authenticator
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: appxsnda.c,v $
 * Revision 3.2.60.2  1996/06/28  23:20:09  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/19  15:11:59  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.4  1996/04/16  03:54:11  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.3  1996/04/02  08:05:01  carrel
 * cvs fixes
 *
 * Revision 3.1.2.2  1996/03/12  03:12:07  che
 * Branch: IosSec_branch
 * Phase 2 Kerberos commit Part 2
 *
 * Revision 3.1.2.1  1996/03/05  04:46:21  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:44:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original sendauth.c 3.1 12/29/93*/

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

csf_sts csfC5sendAppXAuth (
	csfNET_handle *neth,
	char *applver,
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
){
	csfC5_autht autht;
	csf_o result;
	csfC5_cksBlock cks;
	csf_sts rsts = CSF_STS_OK;
	csfC5_data inbuf;
	csfC5_data outbuf;
	csf_size_t len;
	char *nbuf = NULL;

	/* make sure no garbage in the error message */
	if (error) {
		csfC5clear_msgErr( error );
	}

    if (!do_telnet) {
	/*
	 * First, send over the length of the sendauth version string;
	 * then, we send over the sendauth version.  Next, we send
	 * over the length of the application version string followed
	 * by the string itself.  
	 */

	csfC5lit_data( csfC5appxauthver0,
		csfCstrlen( csfC5appxauthver0 ) + 1, &outbuf );
	rsts = csfC5netWriteMsg( neth, &outbuf );
	if (rsts) {
		return rsts;
	}

	csfC5lit_data( applver, csfCstrlen( applver ) + 1, &outbuf );
	rsts = csfC5netWriteMsg( neth, &outbuf );
	if (rsts) {
		return rsts;
	}

	/*
	 * Now, read back a byte: 0 means no error, 1 means bad sendauth
	 * version, 2 means bad application version
	 */

	rsts = csfNETread( neth, &result, 1, &len );
	if (rsts) {
		return rsts;
	}
	if (len != 1) {
		return CSFC5_STS_ENETFRAGREAD;
	}

	switch (result) {

	case CSFC5_APPAUTH_RESPONSE_OK:
		/* everything is fine */
		break;       

	case CSFC5_APPAUTH_RESPONSE_BADAUTHVERS:
		/* bad sendauth version */
		return CSFC5_STS_SENDAUTH_BADAUTHVERS;

	case CSFC5_APPAUTH_RESPONSE_BADAPPLVERS:
		/* bad application version */
		return CSFC5_STS_SENDAUTH_BADAPPLVERS;

	default:		/* something is screwed up here */

		return CSFC5_STS_SENDAUTH_BADRESPONSE;
	}


	/*
	 * We're finished with the initial negotiations; let's get and
	 * send over the authentication header (the AP_REQ message).
	 */
    }

	/*
	 * If no checksum was provided, supply a zero checksum structure
	 */
	if (!cksp) {
		csfC5clear_cksBlock( &cks );
		cksp = &cks;
	}

	/*
	 * Generate a random sequence number
	 */
	if (seqno) {
		/* caller wants a sequence number */
		rsts = csfC5crsGenSeqNo( &creds->seskey, seqno );
		if (rsts) {
			/* couldn't generate a sequence number */
			csfC5clear_autht( &autht );
			return rsts;
		}
	}


	/* Make the authentication header.
	 */
	rsts = csfC5make_msgApReq(
			apreq_opts,
			cksp,
			&creds->client,
			&creds->seskey,
			&creds->e_tkt,
			(seqno ? *seqno : 0),
			newkey,
			&autht,
			&outbuf
		);
	if (rsts) {
		csfC5clear_autht( &autht );
		return rsts;
	}

	if (do_telnet) {
	    telnet_suboption_pad_iac(outbuf.dp, outbuf.dl, &nbuf, buflen);

	    *buf = malloc_named(*buflen+SUBOPT_PAD,
					"Kerberos:csfC5sendAppXAuth()");
	    if (*buf == NULL) {
		free(nbuf);
		return CSFC5_STS_ENOMEM;
	    }
	    memcpy(&((*buf)[3]), nbuf, *buflen);

	    free(nbuf);
	    *buflen+=SUBOPT_PAD;
	    *sec = autht.ctime.secs;
	    *usec = autht.ctime.usecs;
	    return CSF_STS_OK;
	}

	/*
	 * Write the AP_REQ message.
	 */
	rsts = csfC5netWriteMsg( neth, &outbuf );
	csfC5free_data( &outbuf );
	if (rsts) {
		csfC5clear_autht( &autht );
		return rsts;
	}

	/*
	 * Now, read back a message.  If it was a null message (the
	 * length was zero) then there was no error.  If not, we the
	 * authentication was rejected, and we need to return the
	 * error structure.
	 */

	rsts = csfC5netReadMsg( neth, &inbuf );
	if (rsts) {
		csfC5clear_autht( &autht );
		return rsts;
	}

	if (!csfC5isNull_data( &inbuf )) {
		if (error) {
			/* the caller passed in an error pointer */
			rsts = csfC5rd_msgErr( &inbuf, error );
			if (rsts) {
				csfC5free_data( &inbuf );
				csfC5clear_autht( &autht );
				return rsts;
			}
		}
		csfC5free_data( &inbuf );
		csfC5clear_autht( &autht );
		return CSFC5_STS_SENDAUTH_REJECTED;
	}


	/*
	 * If we asked for mutual authentication, we should now get a
	 * length field, followed by a AP_REP message
	 */

	if (apreq_opts & CSFC5c_APOPTS_MUTUAL_REQUIRED_m) {
		csfC5_msgApRep aprep;
		csf_sts problem = CSF_STS_OK;

		rsts = csfC5netReadMsg( neth, &inbuf );
		if (rsts) {
			csfC5clear_autht( &autht );
			return rsts;
		}

		problem = csfC5rdEc_msgApRep( &inbuf, &creds->seskey, &aprep );
		if (
			problem ||
			aprep.epart.ctime.secs != autht.ctime.secs ||
			aprep.epart.ctime.usecs != autht.ctime.usecs
		) {
			problem = CSFC5_STS_SENDAUTH_MUTUAL_FAILED;
		}

		csfC5clear_autht( &autht );
		csfC5free_data( &inbuf );
		if (problem) {
			csfC5free_msgApRep( &aprep );
			return problem;
		}


		/*
		 * If the user wants to look at the AP_REP message,
		 * copy it for him
		 */

		if (rep_result) {
			*rep_result = aprep.epart;
			csfC5clear_msgApRepEPart( &aprep.epart );
		}
		csfC5free_msgApRep( &aprep );
	} 

	return CSF_STS_OK;
}
