/* $Id: preauth.c,v 3.2.60.1 1996/04/19 15:11:55 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/liba/preauth.c,v $
 *------------------------------------------------------------------
 * preauth.c - Preauth functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: preauth.c,v $
 * Revision 3.2.60.1  1996/04/19  15:11:55  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.3  1996/04/16  03:54:03  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.2  1996/04/02  07:59:54  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:46:13  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:44:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original preauth.c 3.2 3/11/94*/
/*.original preauth.h 3.5 3/24/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1992 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Sandia National Laboratories also makes no representations about the
 * suitability of the modifications, or additions to this software for 
 * any purpose.  It is provided "as is" without express or implied warranty.
 */

#include <csfc5/header.h>

/*
 * Preauthentication property flags
 */

#define CSFC5_PREAUTH_FLAGS_ENCRYPT      0x00000001
#define CSFC5_PREAUTH_FLAGS_HARDWARE     0x00000002


typedef csf_sts (csfC5_preauthGetProc) ( csfC5_pauth *pa_data );

typedef csf_sts (csfC5preauthChkProc) ( csfC5_data *data );

typedef struct _csfC5_preauthOps {
	csfC5_pauthType type;				/* Preauthentication type */
	csf_flags flags;					/* Property flags (see above) */
	csfC5_preauthGetProc *get;			/* Get preauth data */
	csfC5preauthChkProc *chk;			/* Check preauth data */
} csfC5_preauthOps;


static csf_sts find_preauthOp
	( int type, csfC5_preauthOps **preauth );

static csf_sts get_tStamp
	( csfC5_pauth * );

static csf_sts chk_tStamp
	( csfC5_data * );

static csfC5_preauthOps preauthOpsTab0[] = {
	{
		CSFC5cx_PADATA_T_ENC_UNIX_TIMESTAMP,
		CSFC5_PREAUTH_FLAGS_ENCRYPT,
		get_tStamp,
		chk_tStamp,
	},
	{ -1,}
};


static csf_sts find_preauthOp (
	int type,
	csfC5_preauthOps **preauth
){
	csfC5_preauthOps *ap = preauthOpsTab0;

	while ((ap->type != -1) && (ap->type != type)) {
		ap++;
	}
	if (ap->type == -1) {
		return CSFC5_STS_PREAUTH_BAD_TYPE;
	}
	*preauth = ap;
	return CSF_STS_OK;
}


/* csfC5obtain_pauth  is a glue routine which when passed in
 * a preauthentication type, client principal, and src_addr, returns
 * preauthentication data contained in data to be passed onto the KDC.
 */
csf_sts csfC5obtain_pauth (
	csfC5_pauthType type,
	const csfC5_crsKBlock *key,
	csfC5_pauth *pauth
){
	csf_sts rsts;
	csfC5_preauthOps *p_system;
	csfC5_crsEBlock eblock;
	csfC5_data scratch;
	csfC5_pauth pauth0;

	csfC5clear_pauth( &pauth0 );
	csfC5clear_pauth( pauth );
	csfC5clear_data( &scratch );

	if (!pauth) {
		return CSFC5_STS_EINVAL;
	}

	if (type == CSFC5c_PADATA_T_NONE) {
		return CSF_STS_OK;
	}

	pauth0.pa_type = type;

	/* Find appropriate preauthenticator */
	rsts = find_preauthOp( type, &p_system );
	if (rsts) {
		goto clean_exit;
	}

	rsts = (*p_system->get)( &pauth0 );
	if (rsts) {
		goto clean_exit;
	}

	/* Check to see if we need to encrypt padata */
	if (p_system->flags & CSFC5_PREAUTH_FLAGS_ENCRYPT) {
		/* If we dont have a encryption key we are out of luck */
		if (!key) {
			rsts = CSFC5_STS_PREAUTH_NO_KEY;
			goto clean_exit;
		}
		csfC5setKType_crsEBlock( &eblock, key->key_type );

		/* do any necessay key pre-processing */
		rsts = csfC5crsProcessKey( &eblock, key );
		if (rsts) {
			goto clean_exit;
		}

		/*
		 * Set up scratch data and length for encryption 
		 * Must allocate more space for checksum and confounder
		 * We also leave space for an uncrypted size field.
		 */
		scratch.dl = csfC5crsEncryptSize( pauth0.pa_data.dl+1,
			&eblock ) + 4; /*TBD/JNK--shouldn't be "4", "sizeof(?)"*/
		scratch.dp = csfCmalloc( scratch.dl );
		if (!scratch.dp) {
			(void) csfC5crsFinishKey(&eblock);
			rsts = CSF_STS_ENOMEM;
			goto clean_exit;
		}

		*((csf_ui32 *) scratch.dp) = pauth0.pa_data.dl;
		((char *)scratch.dp)[pauth0.pa_data.dl] = '\0';

		/* Encrypt preauth data in encryption key */
		rsts = csfC5crsEncrypt( pauth0.pa_data.dp, (char *) scratch.dp + 4,
			pauth0.pa_data.dl, &eblock, 0 );
		if (rsts) {
			(void) csfC5crsFinishKey(&eblock);
			csfCfree(scratch.dp);
			goto clean_exit;
		}
		(void) csfC5crsFinishKey(&eblock);

		csfC5free_data( &pauth0.pa_data );
		pauth0.pa_data = scratch;
	}

 clean_exit:
	if (rsts) {
		csfC5free_pauth( &pauth0 );
	}
	*pauth = pauth0;
	return rsts;
}


/* csfC5verify_pauth  is a glue routine which when passed in
 * the client, src_addr and padata verifies it with the appropriate 
 * verify function.
 */
csf_sts csfC5verify_pauth (
	csfC5_pauth *data,
	const csfC5_crsKBlock *key,
	int *req_id,
	int *flags
){
	csfC5_preauthOps *p_system;
	csfC5_crsEBlock eblock;
	csfC5_data scratch;
	int free_scratch = 0;
	csfC5_cksBlock cksum;
	csf_sts rsts;

	if (!data) {
		return CSFC5_STS_EINVAL;
	}

	/* Find appropriate preauthenticator */
	rsts = find_preauthOp( (int) data->pa_type, &p_system );
	if (rsts) {
		return rsts;
	}

	/* Check to see if we need to decrypt padata */
	if (p_system->flags & CSFC5_PREAUTH_FLAGS_ENCRYPT) {

		/* If we dont have a decryption key we are out of luck */
		if (!key) {
			return CSFC5_STS_EINVAL;
		}

		csfC5setKType_crsEBlock( &eblock, key->key_type );

		scratch.dl = data->pa_data.dl;
		scratch.dp = (char *) csfCmalloc( scratch.dl );
		if (!scratch.dp) {
			return CSF_STS_ENOMEM;
		}

		/* do any necessay key pre-processing */
		rsts = csfC5crsProcessKey( &eblock,key );
		if (rsts) {
			csfCfree( scratch.dp );
			return rsts;
		}

		/* Decrypt data */
		rsts = csfC5crsDecrypt(
			(char *) data->pa_data.dp + 4, /*TBD/JNK--4 again*/
			scratch.dp,
			scratch.dl - 4, &eblock,
			0
		);
		if (rsts) {
			(void) csfC5crsFinishKey( &eblock );
			csfCfree( scratch.dp );
			return rsts;
		}

		scratch.dl  =
			(((int) ((unsigned char *)data->pa_data.dp)[0] << 24) +
			((int) ((unsigned char *)data->pa_data.dp)[1] << 16) +
			((int) ((unsigned char *)data->pa_data.dp)[2] << 8) +
			(int) ((unsigned char *)data->pa_data.dp)[3]);
		free_scratch++;
	} 
	else {
		scratch.dp = data->pa_data.dp;
		scratch.dl = data->pa_data.dl;
	}

	rsts = (*p_system->chk)( &scratch );
	if (free_scratch) {
		csfCfree(scratch.dp);
	}
	if (rsts) {
		return rsts;
	}
	if (flags) {
		*flags = p_system->flags;
	}

	/* Generate a request id by crc32ing the (encrypted) preauth data.
	 * The idea behind req_id is that it is dependant upon
	 * the information in data. This could then be used for
	 * replay detection.
	 */
	/* MUST csfCmalloc cksum.contents */
	cksum.cks_data.dp = csfCcmalloc( csfC5size_cksType(CSFC5c_CKS_T_CRC32) );
	if (!cksum.cks_data.dp) {
		return(1);	/*TBD/JNK--should be real status*/
	}

	if (
		csfC5cksCalculate(
			CSFC5c_CKS_T_CRC32,
			data->pa_data.dp, data->pa_data.dl,
			0, /* No seed.. */
			0, /* ..or seed length */
			&cksum
		)
	) {
		*req_id = 0;
	} 
	else {
		/* Checksum length should be 32 bits, so truncation should never
		   take place */
		if (cksum.cks_data.dl > sizeof(*req_id)) {
			cksum.cks_data.dl = sizeof(*req_id);
		}
		/* Offset req_id for 64 bit systems */
		csfCmemcpy(
			(char *)req_id + (sizeof(*req_id) - cksum.cks_data.dl),
			cksum.cks_data.dp,
			cksum.cks_data.dl
		);
	} 
	csfCfree( cksum.cks_data.dp );
	return CSF_STS_OK;
}


/*
 * Format is:   8 bytes of random confounder,
 *              1 byte version number (currently 0),
 *              4 bytes: number of seconds since Jan 1, 1970, in MSB order.
 */
csf_sts get_tStamp (
	csfC5_pauth *pa_data
){
	csfC5_tStamp tnow;
	unsigned char *tmp;
	csf_sts rsts;

	rsts = csfC5getNow_tStamp( &tnow );
	if (rsts) {
		return rsts;
	}

	pa_data->pa_data.dl = 13;
	tmp = pa_data->pa_data.dp = csfCmalloc( pa_data->pa_data.dl );
	if (!tmp) {
		return CSF_STS_ENOMEM;
	}

	/* Was replaced with this... */
	/* Random confounder. */
	rsts = csfRANconfounder( 8, tmp );
	if (rsts) {
		return rsts;
	}
	tmp += 8;

	/* Version number. */
	*tmp++ = 0;

	/* Time. */
	*tmp++ = (tnow >> 24) & 255;
	*tmp++ = (tnow >> 16) & 255;
	*tmp++ = (tnow >> 8) & 255;
	*tmp++ = tnow & 255;

	return CSF_STS_OK;
}

csf_sts chk_tStamp (
	csfC5_data *data
){
	unsigned char *tmp;
	csfC5_tStamp patime;

	tmp = (unsigned char *) data->dp;
	if (tmp[8] != 0) {
		return CSFC5_STS_PREAUTH_FAILED;
	}
	patime = tmp[9] << 24;
	patime += tmp[10] << 16;
	patime += tmp[11] << 8;
	patime += tmp[12];

	if (csfC5cmpSkewedNow_tStamp( patime ) != 0) {
		return CSFC5_STS_PREAUTH_FAILED;
	}

	return CSF_STS_OK;
}
