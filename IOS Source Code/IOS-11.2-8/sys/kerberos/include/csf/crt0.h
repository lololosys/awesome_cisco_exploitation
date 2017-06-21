/* $Id: crt0.h,v 3.2 1995/11/17 17:47:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csf/crt0.h,v $
 *------------------------------------------------------------------
 * crt0.h - Crypto subsystem definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crt0.h,v $
 * Revision 3.2  1995/11/17  17:47:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_CRT0_H_
#define _CSF_CRT0_H_  "%W% %G%"
/*.original los-proto.h 3.4 6/29/94*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All Rights Reserved.
 */

/*
 * Standard C run-time functions.  Default is the
 * host-defined functions.
 */

#ifndef csfCfree
#define csfCfree free
#endif

#ifndef csfCmalloc
#define csfCmalloc malloc
#endif

#ifndef csfCcalloc
#define csfCcalloc calloc
#endif

#ifndef csfCrealloc
#define csfCrealloc realloc
#endif

#ifndef csfCmemcpy
#define csfCmemcpy memcpy
#endif

#ifndef csfCmemcmp
#define csfCmemcmp memcmp
#endif

#ifndef csfCmemset
#define csfCmemset memset
#endif

#ifndef csfCstrlen
#define csfCstrlen strlen
#endif

#ifndef csfCstrcpy
#define csfCstrcpy strcpy
#endif

#ifndef csfCstrncpy
#define csfCstrncpy strncpy
#endif

#ifndef csfCstrcmp
#define csfCstrcmp strcmp
#endif

#ifndef csfCstrncmp
#define csfCstrncmp strncmp
#endif

#ifndef csfCstrcat
#define csfCstrcat strcat
#endif

#ifndef csfCstrncat
#define csfCstrncat strncat
#endif

#ifndef csfCstrchr
#define csfCstrchr strchr
#endif

#ifndef csfCstrrchr
#define csfCstrrchr strrchr
#endif

#ifndef csfCtime
#define csfCtime cisco_kerb_time
#endif

#ifndef csfCrand
#define csfCrand cisco_kerb_rand
#endif

#ifndef csfCsrand
#define csfCsrand cisco_kerb_srand
#endif

#ifndef csfCabs
#define csfCabs(x) ((x > 0) ? x : -x)
#endif

#ifndef csfCassert
#define csfCassert assert
#endif

#ifndef csfCabort
#define csfCabort abort
#endif

#endif /* _CSF_CRT0_H_ */
