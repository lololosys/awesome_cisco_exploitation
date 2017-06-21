/* $Id: prn1.c,v 3.2 1995/11/17 17:44:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/prn1.c,v $
 *------------------------------------------------------------------
 * prn1.c - Functions to manipulate Kerberos principles
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: prn1.c,v $
 * Revision 3.2  1995/11/17  17:44:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright 1993-1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include <csfc5/header.h>


static csfC5_data csfC5tgsname0 = {sizeof(CSFC5c_NAME_TGS)-1, CSFC5c_NAME_TGS, 0};

csf_sts csfC5toSalt_prnId (
	const csfC5_prnId *prnid,
	csfC5_data *ret
){
	csf_size_t size = 0;
	int nelem;
	int i;
	csf_sts rsts;

	csfC5clear_data( ret );

	if (prnid == NULL) {
		return CSF_STS_OK;
	}

	nelem = prnid->name.pcn;
	for (i = 0; i < nelem; i++) {
		size += prnid->name.pca[i].namec.dl;
	}
	size += prnid->realm.namec.dl;

	rsts = csfC5malloc_data( size, ret );
	if (rsts) {
		return rsts;
	}

	{
		void *dest;
		csf_size_t len;

		dest = ret->dp;
		len = prnid->realm.namec.dl;
		csfCmemcpy( dest, prnid->realm.namec.dp, len );

		for (i = 0; i < nelem; i++) {
			dest = _csfCptrInc( dest, len );
			len = prnid->name.pca[i].namec.dl;
			csfCmemcpy( dest, prnid->name.pca[i].namec.dp, len );
		}
	}

	return CSF_STS_OK;
}


csf_sts csfC5dataTo_prnId (
	csfC5_prnId *prnid,
	const csfC5_data *realm,
	... /* csfC5_data * */
){
	va_list ap;
	int i;
	int count = 0;
	csfC5_prnId pret;
	csf_sts rsts;

	csfC5clear_prnId( &pret );

	va_start( ap, realm );
	while (va_arg( ap, csfC5_data *) != NULL) {
		count++;
	}
	va_end( ap );

	/* get space for array */
	pret.name.pca = (csfC5_prnNC *) csfCmalloc( sizeof(csfC5_prnNC) * count );
	if (!pret.name.pca) {
		rsts = CSF_STS_ENOMEM;
		goto clean_exit;
	}
	pret.name.pcn = count;

	rsts = csfC5copy_data( realm, &pret.realm.namec );
	if (rsts) {
		goto clean_exit;
	}

	va_start( ap, realm );
	for (i = 0; i < count; i++) {
		csfC5_data *pnext;
		pnext = va_arg( ap, csfC5_data * );
		rsts = csfC5copy_data( pnext, &pret.name.pca[i].namec );
		if (rsts) {
			goto clean_exit;
		}
	}
	va_end( ap );

	pret.name.prn_type = CSFC5c_NT_UNKNOWN;

clean_exit:
	if (rsts) {
		csfC5free_prnId( &pret );
	}
	*prnid = pret;
	return rsts;
}


csf_sts csfC5tgsName_prnId (
	const csfC5_prnNC *realm,		/* Realm. */
	const csfC5_prnNC *instance,	/* Instance. Optional. Default is "realm". */
	const csfC5_data *name,		/* Name. Optional. Default is "krbtgt". */
	csfC5_prnId *tgsprnid
){
	csf_sts rsts;
	const csfC5_data *instance0;
	const csfC5_data *name0;

	if (instance == NULL) {
		instance0 = &realm->namec;
	}
	else {
		instance0 = &instance->namec;
	}

	if (name == NULL) {
		name0 = &csfC5tgsname0;
	}
	else {
		name0 = name;
	}

	rsts = csfC5dataTo_prnId(
			tgsprnid,
			&realm->namec,
			name0,
			instance0,
			NULL
		);
	return rsts;
}
