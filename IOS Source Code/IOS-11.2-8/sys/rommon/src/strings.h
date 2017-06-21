/* $Id: strings.h,v 3.2 1995/11/17 18:43:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/strings.h,v $
 *------------------------------------------------------------------
 * strings.h
 *
 * September 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Prototypes for the standard string routines. 
 *------------------------------------------------------------------
 * $Log: strings.h,v $
 * Revision 3.2  1995/11/17  18:43:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __STRINGS_H__
#define __STRINGS_H__

/*
** strings prototypes
*/
extern char *strchr(char *sp, char c);
extern char *strcat(char *s1, char *s2);
extern int strcmp(char *s1, char *s2);
extern char *strcpy(char *s1, char *s2);
extern int strlen(char *s);
extern char *strncat(char *s1, char *s2, int n);
extern int strncmp(char *s1, char *s2, int n);
extern char *strncpy(char *s1, char *s2, int n);

#endif  /* __STRINGS_H__ */
