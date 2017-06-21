/* $Id: string.h,v 3.2.60.2 1996/04/29 05:30:26 mdb Exp $
 * $Source: /release/112/cvs/Xsys/h/string.h,v $
 *------------------------------------------------------------------
 * Declarations for  ANSI C standard library functions
 *
 * February 1995, Steve Berl (sberl)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: string.h,v $
 * Revision 3.2.60.2  1996/04/29  05:30:26  mdb
 * CSCdi56007:  unix images have problems with multiple size_t definitions
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:37:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:37:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:17:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:13:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/20  20:24:44  lol
 * CSCdi42253:  callback causes IPCP authorization failures
 *              Only return callback info when doing local authorization of
 *              ppp/lcp, slip, arap or exec
 *
 * Revision 2.1  1995/06/07  20:36:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __STRING_H__
#define __STRING_H__
#ifndef GROVEL

#if defined(NULL)
#undef NULL
#endif /* NULL */
#define NULL ((void *) 0)

/*
 * The ANSI C standard requires we define the type size_t in this
 * file, but it also requires that those types be defined in stddef.h
 * ... We therefore have to be bright enough to protect the size_t and
 * wchar_t types from appearing more than once in our sources because
 * multiple typedef statements for the same type cause problems.
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

void   *memcpy(void *, const void *, size_t);
void   *memmove(void *, const void *, size_t);
int     memcmp(const void *, const void *, size_t);
void   *memchr(const void *, int, size_t);
void   *memset(void *, int, size_t);

char   *strcat(char *dest, const char *src);
int     strcmp(const char *, const char *);
int     strncmp(const char *, const char *, size_t);
char   *strchr(const char *, int);
char   *strrchr(const char *s, int c);
int     strcoll(const char *s1, const char *s2);
char   *strerror(int errnum);
size_t  strlen(const char *);
char   *strcpy(char *dest, const char *src);
char   *strncpy(char *dest, const char *src, size_t n);
char   *strtok(char *str, const char *set);
char   *strncat(char *dest, const char *src, size_t n);
size_t  strspn(const char *s, const char *set);
char   *strpbrk(const char *s1, const char *s2);

/* not quite the ANSI definition */
size_t  strcspn(char *, char *);

#define STREQ(s1,s2)    (strcmp(s1,s2) == 0)

/*
 * not yet implemented
 */
char *strstr(const char *s1, const char *s2);
size_t strxfrm(char *s1, const char *s2, size_t n);

#endif /* GROVEL */
#endif /* __STRING_H__ */
