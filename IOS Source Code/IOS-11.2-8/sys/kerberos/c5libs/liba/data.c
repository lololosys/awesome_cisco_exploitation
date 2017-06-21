/* $Id: data.c,v 3.2 1995/11/17 17:44:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/data.c,v $
 *------------------------------------------------------------------
 * data.c - Functions to clear data block
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: data.c,v $
 * Revision 3.2  1995/11/17  17:44:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

void csfC5clear_data (
	csfC5_data *p
){
	if (p) {
		csfCmemzero( p, sizeof(*p) );
	}
}

void csfC5free_data (
	csfC5_data *p
){
	if (p) {
		if (p->dp) {
			csfCmemzero( p->dp, p->dl );
			csfCfree( p->dp );
			csfC5clear_data( p );
		}
	}
}

csf_sts csfC5copy_data (
	const csfC5_data *in,
	csfC5_data *out
){
	csfC5_data tmp;

	csfC5clear_data( out );

	tmp = *in;
	tmp.dp = csfCmalloc( tmp.dl );
	if (tmp.dp == NULL) {
		return CSF_STS_ENOMEM;
	}
	csfCmemcpy( tmp.dp, in->dp, tmp.dl );
	*out = tmp;
	return CSF_STS_OK;
}

/*
 * Copy a data structure, with fresh allocation.
 */

csf_sts csfC5copyC_data (
	const csfC5_data *indata,
	csfC5_data **outdata
){
	csfC5_data *tempdata;

	*outdata = NULL;

	tempdata = (csfC5_data *) csfCmalloc( sizeof(*tempdata) );
	if (!tempdata) {
		return CSF_STS_ENOMEM;
	}

	tempdata->dl = indata->dl;

	tempdata->dp = csfCmalloc( tempdata->dl );
	if (!tempdata->dp) {
		csfCfree( tempdata );
		return CSF_STS_ENOMEM;
	}

	csfCmemcpy( tempdata->dp, indata->dp, tempdata->dl );
	*outdata = tempdata;

	return CSF_STS_OK;
}


void csfC5move_data (
	csfC5_data *in,
	csfC5_data *out
){
	*out = *in;
	csfC5clear_data( in );
}

int csfC5cmp_data (
	const csfC5_data *d1,
	const csfC5_data *d2
){
	int v;

	if (d1 && !d2) {
		return -1;
	}

	if (!d1 && d2) {
		return 1;
	}

	v = d1->dl - d2->dl;
	if (v == 0) {
		v = csfCmemcmp( d1->dp, d2->dp, d1->dl );
	}

	return v;
}


csf_boolean csfC5isNull_data (
	const csfC5_data *d
){
	if (d && (d->dl != 0) && (d->dp != NULL)) {
		return FALSE;
	}
	return TRUE;
}

csf_size_t csfC5getLen_data (
	const csfC5_data *d
){
	return d->dl;
}

void csfC5setLen_data (
	csf_size_t dl,
	csfC5_data *d
){
	d->dl = dl;
}

void * csfC5getPtr_data (
	const csfC5_data *d
){
	return d->dp;
}

void csfC5setPtr_data (
	void *dp,
	csfC5_data *d
){
	d->dp = dp;
}

csfC5_data * csfC5lit_data (
	const void *dp,
	csf_size_t dl,
	csfC5_data *d
){
	d->dp = (void *) dp; /*TBD/JNK--Should I worry?*/
	d->dl = dl;
	return d;
}

csfC5_data * csfC5litStr_data (
	const char *s,
	csfC5_data *d
){
	return csfC5lit_data( (void *) s, csfCstrlen( s ), d );
}

csf_sts csfC5malloc_data (
	csf_size_t size,
	csfC5_data *d
){
	d->dp = csfCmalloc( size );
	if (!d->dp) {
		d->dl = 0;
		return CSF_STS_ENOMEM;
	}
	else {
		d->dl = size;
		return CSF_STS_OK;
	}
}


csf_sts csfC5import_data (
	void *p,
	csf_size_t size,
	csfC5_data *d
){
	d->dp = csfCmalloc( size );
	if (!d->dp) {
		d->dl = 0;
		return CSF_STS_ENOMEM;
	}
	d->dl = size;
	(void) csfCmemcpy( d->dp, p, size );
	return CSF_STS_OK;
}


csf_sts csfC5realloc_data (
	csf_size_t size,
	csfC5_data *d
){
	void *p;

	p = csfCrealloc( d->dp, size );
	if (!p) {
		return CSF_STS_ENOMEM;
	}
	else {
		d->dp = p;
		return CSF_STS_OK;
	}
}
