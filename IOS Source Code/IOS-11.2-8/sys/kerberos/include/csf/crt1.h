/* $Id: crt1.h,v 3.2 1995/11/17 17:47:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/crt1.h,v $
 *------------------------------------------------------------------
 * crt1.h - Crypto subsystem definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crt1.h,v $
 * Revision 3.2  1995/11/17  17:47:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_CRT1_H_
#define _CSF_CRT1_H_  "%W% %G%"

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */


/*
 * Alternates for standard functions.
 * (When the standard functions don't work right.)
 *
 * To use these, e.g.:
 *	#define csfCmalloc csfCmalloc1
 */

/* csfCmalloc1 -- Behave properly when size is zero. */
#ifndef csfCmalloc1
void * csfCmalloc1 ( csf_size_t );
#endif

/* csfCcalloc1 -- Behave properly when size(s) are zero. */
#ifndef csfCcalloc1
void * csfCcalloc1 ( csf_size_t, csf_size_t );
#endif

/* csfCrealloc1 -- Behave properly when pointer or size is zero. */
#ifndef csfCrealloc1
void * csfCrealloc1 ( void *, csf_size_t );
#endif

/* csfCfree1 -- Behave properly when pointer is zero. */
#ifndef csfCfree1
void csfCfree1 ( void * );
#endif


/*
 * Non-standard (but useful) functions.
 */

/* roundup -- Round up to nearest multiple. */
#ifndef _csfCroundUp
/* round x up to nearest multiple of y */
#define _csfCroundUp(x, y) ((((x) + (y) - 1)/(y))*(y))
#endif

/* min -- minimum */
#ifndef _csfCmin
#define _csfCmin( a, b ) (((a) < (b)) ? (a) : (b) )
#endif

/* max -- maximum. */
#ifndef _csfCmax
#define _csfCmax( a, b ) (((a) > (b)) ? (a) : (b) )
#endif

/* ptrInc -- Increment pointer by value. */
#ifndef _csfCptrInc
#define _csfCptrInc( p, i ) \
	((void *)((char *)p + i))
#endif


/* memzero -- Zero memory: memset( p, 0, n ) */
#ifndef csfCmemzero
#define csfCmemzero( p, n ) csfCmemset( (p), 0, (n) )
#endif

/* cmalloc -- Mallocs and zeros memory: calloc( 1, n ) */
#ifndef csfCcmalloc
#define csfCcmalloc( n ) csfCcalloc( 1, (n) )
#endif

/* memdup -- Malloc and duplicate memory */
#ifndef csfCmemdup
void * csfCmemdup ( const void *, csf_size_t );
#endif

/* freez -- Free and clear memory */
#ifndef csfCfreez
void csfCfreez ( void *, csf_size_t );
#endif

/* strdup -- Malloc and duplicate string. */
#ifndef csfCstrdup
char * csfCstrdup ( const char * );
#endif

/* strcmpi -- Case-insensitive string comparison. */
#ifndef csfCstrcmpi
int csfCstrcmpi ( const char * s1, const char * s2 );
#endif

/* strupper -- Upper-case string (in place). */
#ifndef csfCstrupper
char * csfCstrupper ( char * );
#endif

/* strlower -- Lower-case string (in place). */
#ifndef csfCstrlower
char * csfCstrlower ( char * );
#endif

/* strnputsz -- Concatenate multiple strings & null terminate. */
#ifndef csfCstrnputsz
char * csfCstrnputsz (	/*TBD/JNK*/
	char * dststr,
	csf_size_t dstmax,
	const char * srcstr,
	...
);
#endif


/* secTOD -- Time of day in seconds (1970 epoch). */
#ifndef csfCsecTOD
csf_sts csfCsecTOD
	( csf_ui32 * );
#endif

/* usTOD -- Time of day with microseconds. */
#ifndef csfCusTOD
csf_sts csfCusTOD
	( csf_ui32 *, csf_ui32 * );
#endif

/* uusTOD -- Time of day with unique microseconds. */
#ifndef csfCuusTOD
csf_sts csfCuusTOD
	( csf_ui32 *, csf_ui32 * );
#endif

/* gmtime -- like normal gmtime, except returs value as argument
 * and as value of function (to remain reentrant).
 */
/*
#ifndef csfCgmtime
csf_tm * csfCgmtime (
	const csf_time_t *it,
	csf_tm *out
);
#endif
*/

/* mkgmtime -- like mktime, only uses gmt. */
#ifndef csfCmkgmtime
csf_time_t csfCmkgmtime (
	const csf_tm *gmtm
);
#endif

/* atod4 -- decimal alhpha to number (4 digits). */
#ifndef csfCatod4
int csfCatod4 (
	const char *s
);
#endif

/* dtoa4 -- decimal to alpha decimal (4 digits). */
#ifndef csfCdtoa4
char * csfCdtoa4 (
	int n,
	char *s
);
#endif

/* atod4 -- decimal alhpha to number (2 digits). */
#ifndef csfCatod2
int csfCatod2 (
	const char *s
);
#endif

/* dtoa4 -- decimal to alpha decimal (2 digits). */
#ifndef csfCdtoa2
char * csfCdtoa2 (
	int n,
	char *s
);
#endif

#endif /* _CSF_CRT1_H_ */
