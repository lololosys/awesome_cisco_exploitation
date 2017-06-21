/* $Id: ciscolib.h,v 3.1.10.1 1996/03/18 20:50:34 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/cisco/include/ciscolib.h,v $
 *------------------------------------------------------------------
 * ciscolib.h -- prototypes needed for libcisco.a functions
 *
 * December 1995, Mark D. Baushke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ciscolib.h,v $
 * Revision 3.1.10.1  1996/03/18  20:50:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.2  1996/03/09  05:28:53  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.1.2.1  1996/03/07  08:20:16  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  04:41:21  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CISCOLIB_H__
#define __CISCOLIB_H__

#define STRING_BUF_CHARS 80
extern  unsigned char const bit_swaps[];
extern  unsigned char const hexchars[];

long    atoo(const char *s);
int     atohex(const char *s);
int     badbcd(unsigned char *s, long len);
void    bcdtochar(unsigned char *from, unsigned char *to, long len);
int     bcmp(const void *s1, const void *s2, long bytes);
void    bcopy(const void *src, void *dest, long bytes);
void    bscopy(void const *src, void *dest, long bytes);
void    bzero(void *dummy, long count);
void    chartobcd(unsigned char *from, unsigned char *to, long len);
int     chartohex(unsigned char ch);
int     cmpid(const unsigned char *ptr1, const unsigned char *ptr2, long len);
void    concat(const char *s1, const char *s2, char *s3);
char   *deblank(const char *s);
int     firstbit(unsigned long word);
int     firstrbit(unsigned long word);
int     is_string(const char *s);
int     lcmp(unsigned long *p1, unsigned long *p2, long longs);
void    lowercase(char *str);
int     null(const char *s);
int     num_bits(unsigned long);
char   *remove_ws(char *s);
char   *sstrncat(char *s1, char const *s2, unsigned long max);
char   *sstrncpy(char *s1, char const *s2, unsigned long max);
int     strcasecmp(const char *s1, const char *s2);
int     strncasecmp(const char *s1, const char *s2, unsigned long length);
char   *string_getnext(void);
void    sys_ebcdic_to_ascii(unsigned char *string, long length);
int     term_char(char c);
char    tohexchar(unsigned char c);
void    uppercase(char *str);
void    wcopy(unsigned short* src, unsigned short* dst, long count);

#define tohexchar(c) (hexchars[(c)&0x0f])

#endif /* __CISCOLIB_H__ */
