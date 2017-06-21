/* $Id: unparse.c,v 3.2 1995/11/17 17:44:44 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/liba/unparse.c,v $
 *------------------------------------------------------------------
 * unparse.c - Functions to unparse Principle ID
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unparse.c,v $
 * Revision 3.2  1995/11/17  17:44:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:18:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original unparse.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Rewritten by Theodore Ts'o to propoerly unparse principal names
 * which have the component or realm separator as part of one of their
 * components.
 */

#include <csfc5/header.h>

/*
 * converts the multi-part principal format used in the protocols to a
 * single-string representation of the name. 
 *  
 * The name returned is in allocated storage and should be freed by
 * the caller when finished.
 *
 * Conventions: / is used to separate components; @ is used to
 * separate the realm from the rest of the name.  If '/', '@', or '\0'
 * appear in any the component, they will be representing using
 * backslash encoding.  ("\/", "\@", or '\0', respectively)
 *
 * returns error
 *      KRB_PARSE_MALFORMED     principal is invalid (does not contain
 *                              at least 2 components)
 * also returns system errors
 *      CSF_STS_ENOMEM                  unable to allocate memory for string
 */

csf_sts csfC5unparseX_prnId (
	const csfC5_prnId *prnid,
	char **name,
	int *size
){
	char *cp, *q;
	int i, j;
	int length, nelem;
	int totalsize;

	totalsize = prnid->realm.namec.dl + 1;

	for (cp = prnid->realm.namec.dp; *cp; cp++) {
		if (
			*cp == CSFC5c_NAMEC_REALM_SEP  ||
			*cp == CSFC5c_NAMEC_COMPONENT_SEP ||
			*cp == '\\' ||
			*cp == '\t'
		) {
			totalsize++;
		}
	}

	nelem = prnid->name.pcn;
	for (i = 0; i < nelem; i++) {
		cp = prnid->name.pca[i].namec.dp;
		length = prnid->name.pca[i].namec.dl;
		totalsize += length;
		for (j = 0; j < length; j++, cp++) {
			if (
				*cp == CSFC5c_NAMEC_REALM_SEP ||
				*cp == CSFC5c_NAMEC_COMPONENT_SEP ||
				*cp == '\0' ||
				*cp == '\\' ||
				*cp == '\t'
			) {
				totalsize++;
			}
		}
		totalsize++;	/* This is for the separator */
	}

	/* we need only n-1 seps for n components, but we need an
	 * extra byte for the NULL at the end
	 */
	if (*name) {
		if (*size < totalsize) {
			*size = totalsize;
			*name = csfCrealloc( *name, totalsize );
		}
	} 
	else {
		*name = csfCmalloc( totalsize );	/* room for null */
		if (size) {
			*size = totalsize;
		}
	}

	if (!*name) {
		return CSF_STS_ENOMEM;
	}

	q = *name;

	for (i = 0; i < nelem; i++) {
		cp = prnid->name.pca[i].namec.dp;
		length = prnid->name.pca[i].namec.dl;
		for (j = 0; j < length; j++, cp++) {
			switch (*cp) {
			case CSFC5c_NAMEC_COMPONENT_SEP:
			case CSFC5c_NAMEC_REALM_SEP:
			case '\t':
			case '\\':
				*q++ = '\\';
				*q++ = *cp;
				break;
			case '\0':
				*q++ = '\\';
				*q++ = '0';
				break;
			default:
				*q++ = *cp;
			}
		}
		*q++ = CSFC5c_NAMEC_COMPONENT_SEP;
	}

	q--;			/* Back up last component separator */
	*q++ = CSFC5c_NAMEC_REALM_SEP;

	cp = prnid->realm.namec.dp;
	length = prnid->realm.namec.dl;
	for (j = 0; j < length; j++, cp++) {
		switch (*cp) {
		case CSFC5c_NAMEC_COMPONENT_SEP:
		case CSFC5c_NAMEC_REALM_SEP:
		case '\t':
		case '\\':
			*q++ = '\\';
			*q++ = *cp;
			break;
		case '\0':
			*q++ = '\\';
			*q++ = '0';
			break;
		default:
			*q++ = *cp;
		}
	}
	*q++ = '\0';

	return CSF_STS_OK;
}

csf_sts csfC5toStr_prnId (
	const csfC5_prnId * prnid,
	char **name
){
	*name = NULL;
	return csfC5unparseX_prnId( prnid, name, NULL );
}
