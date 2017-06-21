/* $Id: a2d2a.c,v 3.2 1995/11/17 17:46:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/util/a2d2a.c,v $
 *------------------------------------------------------------------
 * a2d2a.c - Misc. functions for time
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: a2d2a.c,v $
 * Revision 3.2  1995/11/17  17:46:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:38  shaker
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

#include <csf/header.h>

#ifndef csfCdtoa2
char * csfCdtoa2 (
	int n,
	char *s
){

	if ((n >= 0) && (n < 100)) {
		*s++ = (n / 10) + '0';
		*s++ = (n % 10) + '0';
	}
	return s;

}
#endif


#ifndef csfCdtoa4
char * csfCdtoa4 (
	int n,
	char *s
){
	int i;

	if ((n >= 0) && (n < 10000)) {
		for (i = 3; i >= 0; i--) {
			s[i] = (n % 10) + '0';
			n = n / 10;
		}
		s += 4;
	}

	return s;

}
#endif


#ifndef csfCatod2
int csfCatod2 (
	const char *s
){
	int retn = -1;

	if (isdigit( *s )) {
		retn = (*s - '0') * 10;
		s++;
		if (isdigit( *s )) {
			retn += *s - '0';
		}
	}

	return retn;

}
#endif


#ifndef csfCatod4
int csfCatod4 (
	const char *s
){
	int retn;
	int i;

	retn = 0;
	for (i = 0; i < 4; i++, s++) {
		if (!isdigit( *s )) {
			retn = -1;
			break;
		}
		retn *= 10;
		retn += (*s - '0');
	}

	return retn;

}
#endif

