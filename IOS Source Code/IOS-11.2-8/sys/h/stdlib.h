/* $Id: stdlib.h,v 3.2.60.2 1996/04/29 05:30:25 mdb Exp $
 * $Source: /release/112/cvs/Xsys/h/stdlib.h,v $
 *------------------------------------------------------------------
 * ANSI C Standard include file
 *
 * May 1995, Steve Berl
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stdlib.h,v $
 * Revision 3.2.60.2  1996/04/29  05:30:25  mdb
 * CSCdi56007:  unix images have problems with multiple size_t definitions
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:37:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:37:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:17:36  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:13:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:36:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __STDLIB_H__
#define __STDLIB_H__

/*
 * ANSI C required macros:
 *   EXIT_FAILURE, EXIT_SUCCESS, MB_CHR_MAX, NULL, RAND_MAX
 */
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define MB_CUR_MAX 1
#define RAND_MAX 0x7fffffff
#if !defined(NULL)
#define NULL ((void *) 0)
#endif /* NULL */

/*
 * ANSI C required types:
 *   div_t, ldiv_t, size_t, wchar_t
 */

/*
 * The ANSI C standard requires we define the types size_t and whar_t
 * in this file, but it also requires that those types be defined in
 * stddef.h ... We therefore have to be bright enough to protect the
 * size_t and wchar_t types from appearing more than once in our
 * sources because multiple typedef statements for the same type cause
 * problems.
 *
 * Protect typedefs with an ifdef
 */
#if !defined(_SIZE_T) && !defined(_GCC_SIZE_T)

#define _SIZE_T

#if defined(__GNUC__)
#define _GCC_SIZE_T
typedef __typeof__ (sizeof (0)) size_t;
#else
typedef unsigned int size_t;
#endif /* __GNUC__ */

#endif /* _SIZE_T */

#if !defined(_WCHAR_T)
#define _WCHAR_T

#ifdef __WCHAR_TYPE__                   /* did the compiler have one */
typedef __WCHAR_TYPE__ wchar_t;
#else
typedef int wchar_t;
#endif /* __WCHAR_TYPE__ */

#endif /* _WCHAR_T */

/*
 * required types: div_t, ldiv_t
 */
typedef struct div_t_ {
    int quot;                           /* quotient */
    int rem;                            /* remainder */
} div_t;

typedef struct ldiv_t_ {
    long quot;                          /* quotient */
    long rem;                           /* remainder */
} ldiv_t;

/*
 * give hints to the compiler using the attribute mechanism
 */
#ifdef gnuc_attribute
#undef gnuc_attribute
#endif
#if defined(__GNUC__)
#define gnuc_attribute(attrs) __attribute__(attrs)
#else
#define gnuc_attribute(attrs)
#endif /* __GNUC__ */

/*
 * ANSI C required functions:
 *   abort, abs, atexit, atof, atoi, atol, bsearch, calloc, div, exit,
 *   free, getenv, labs, ldiv, malloc, mblen, mbstowcs, mbtowc, qsort,
 *   rand, realloc, srand, strtod, strtol, strtoul, system, wcstombs,
 *   wctomb 
 */
int               abs(int i);
int               atoi(const char *str);
void             *calloc(size_t nmemb, size_t size);
div_t             div(int numer, int denom);
void              free(void *ptr);
long int          labs(long int j);
ldiv_t            ldiv(long numer, long denom);
void             *malloc(size_t size);
void             *realloc(void *ptr, size_t size);
long int          strtol(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
int               wctomb(char *s, wchar_t wchar);

/*
 * presently unimplemented functions
 */
void              abort(void); /* gnuc_attribute((noreturn)) */
int               atexit(void (*func)(void));
double            atof(const char *nptr);
void             *bsearch(const void *key, const void *base, size_t nmemb,
		          size_t size,
			  int (*compar)(const void *, const void *));
void              exit(int status); /* gnuc_attribute((noreturn)) */
char             *getenv(const char *string);
int               mblen(const char *s, size_t n);
size_t            mbstowcs(wchar_t *pwcs, const char *s, size_t n);
int               mbtowc(wchar_t *pwc, const char *s, size_t n);
void              qsort(void *base, size_t nmemb, size_t size,
		        int (*compare)(const void *, const void *));
void              srand(unsigned int seed);
double            strtod(const char *nptr, char **endptr);
int               system(const char *string);
size_t            wcstombs(char *s, const wchar_t *pwcs, size_t n);


/*
 * Non-gcc versions will get the library versions, and will be
 * slightly slower. The old macros referenced the macro arg more than
 * once.
 */
#ifdef __GNUC__
#define abs(i)	\
	({ int __x = (i); (__x < 0) ? -__x : __x;})
#define labs(j)	\
	({ long int __x = (j); (__x < 0) ? -__x : __x;})
#endif

/*
 * reentrant versions of the standard routines
 */
int           _atoi_r(const char *s, int *err);
unsigned long _strtoul_r(const char *s, char **ptr, int base, int *err);
long          _strtol_r(const char *s, char **ptr, int base, int *err);
#endif /* __STDLIB_H__ */
