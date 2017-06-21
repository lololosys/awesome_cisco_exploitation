/* $Id: crt1.c,v 3.2 1995/11/17 17:46:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/csflibs/util/crt1.c,v $
 *------------------------------------------------------------------
 * crt1.c - Misc standard functions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crt1.c,v $
 * Revision 3.2  1995/11/17  17:46:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:39  shaker
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


/*
 * Alternates for standard functions when the standard
 * functions don't work correctly.
 */

#ifndef csfCfree1
void csfCfree1( void *p )
{
	if (p != NULL) {
		free( p );
	}
}
#endif

#ifndef csfCmalloc1
void * csfCmalloc1( csf_size_t n )
{
	if (n == 0) {
		n++;
	}
	return malloc( n );
}
#endif

#ifndef csfCcalloc1
void * csfCcalloc1( csf_size_t ne, csf_size_t es )
{
	if ((ne == 0) || (es == 0)) {
		return malloc(1);
	}
	else {
		return malloc(ne*es);
	}
}
#endif

#ifndef csfCrealloc1
void * csfCrealloc1( void *p, csf_size_t n )
{
	if (p == NULL) {
		return csfCmalloc( n );
	}
	else if (n == 0) {
		csfCfree( p );
		return NULL;
	}
	else {
		return realloc( p, n );
	}
}
#endif

/*
 * Non-standard (but useful) functions.
 */

#ifndef csfCcmalloc
void * csfCcmalloc (
	csf_size_t n
){
	return csfCcalloc1( 1, n );
}
#endif

#ifndef csfCmemdup
void * csfCmemdup (
	const void *p,
	csf_size_t n
){
	void *p1;

	p1 = csfCmalloc( n );
	if (p1) {
		csfCmemcpy( p1, p, n );
	}
	return p1;
}
#endif

#ifndef csfCfreez
void csfCfreez (
	void *p,
	csf_size_t n
){
	if (p) {
		csfCmemzero( p, n );
		csfCfree( p );
	}
}
#endif

#ifndef csfCstrdup
char * csfCstrdup (
	const char *s
){
	char *ds = NULL;	/*TBD/JNK--strdup(NULL) returns NULL?*/

	if (s) {
		ds = (char *) csfCmalloc( csfCstrlen(s)+1 );
		if (ds) {
			csfCstrcpy( ds, s );
		}
	}

	return ds;
}
#endif

#ifndef csfCstrupper
char * csfCstrupper (
	char *s
){
	char *c;

	for (c = s; *c; c++) {
		if (islower( *c )) {
			*c = toupper( *c );
		}
	}

	return s;
}
#endif

#ifndef csfCstrlower
char * csfCstrlower (
	char *s
){
	char *c;

	for (c = s; *c; c++) {
		if (isupper( *c )) {
			*c = tolower( *c );
		}
	}

	return s;
}
#endif

#ifndef csfCstrcmpi
int csfCstrcmpi (
	const char *s1,
	const char *s2
){
	char c1, c2;

	for (c1 = *s1, c2 = *s2; c1 && c2; c1 = *s1++, c2 = *s2++) {
		if (isupper( c1 )) {
			c1 = tolower( c1 );
		}
		if (isupper( c2 )) {
			c2 = tolower( c2 );
		}
		if (c1 - c2) {
			break;
		}
	}

	return c1 - c2;
}
#endif
