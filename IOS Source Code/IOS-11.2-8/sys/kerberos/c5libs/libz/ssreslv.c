/* $Id: ssreslv.c,v 3.2 1995/11/17 17:45:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libz/ssreslv.c,v $
 *------------------------------------------------------------------
 * ssreslv.c - Resolver to define how cache is stored
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ssreslv.c,v $
 * Revision 3.2  1995/11/17  17:45:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

/*
Generic resolver used by credentials cache (CC), replay cache (RC)
and the key table (KT) subsystems.  The names of these entities is of
the form "type:residual".  The type is specific to the subsystem; the
user calls subsystem-specific resolver, which calls us.  The subsystem
then uses the parsed type to determine it's internal type, and is
expected to pass the residual to a type-specific resolver (within the
subsystem).

Names may have any component empty:
	Input			Type	Residual
	abc:def		abc	def
	:def			NULL	def
	abc:			abc	NULL
	:				NULL	NULL
	NULL			NULL	NULL

All results are optional and specified as a NULL in the return argument.
*/

csf_sts csfC5ssresolve (
	const char *name,		/* Name to resolve. */
	char **typnam,			/* First position of type; NULL if none. */
	csf_size_t *typlen,	/* Length of type string. */
	char **resnam,			/* First position of residual; NULL if none. */
	csf_size_t *reslen		/* Length of residual string. */
){
	char *cp;
	char * tp;			/* Type start position. */
	csf_size_t tl;		/* Type length. */
	char * rp;			/* Residual start position. */
	csf_size_t rl;		/* Residual length. */

	tp = NULL;			/* Assume no type. */
	tl = 0;
	rp = NULL;			/* Assume no residual. */
	rl = 0;

	/*TBD/JNK -- make separator character (":") configuration option. */
	if (name) {
		cp = csfCstrchr( name, ':' );
		if (cp == NULL) {
			/* Type not specified. */
			rp = (char *) name;		/* No type; everything is.. */
			rl = csfCstrlen( name );	/* ..residual. */
		}
		else {
			/* Type specified. */
			tp = (char *) name;	/* Starts at beginning, up to.. */
			tl = cp - name;		/* ..but not including separator. */
			rp = cp + 1;			/* Residual is everything AFTER the.. */
			rl = csfCstrlen( rp );	/* ..separator. */
		}
	}

	if (tl == 0) {			/* Case of leading ":". */
		tp = NULL;
	}
	if (rl == 0) {			/* Case of trailing ":". */
		rp = NULL;
	}

	if (typnam) {
		*typnam = tp;
	}
	if (typlen) {
		*typlen = tl;
	}
	if (resnam) {
		*resnam = rp;
	}
	if (reslen) {
		*reslen = rl;
	}

	return CSF_STS_OK;

}

