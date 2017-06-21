/* $Id: cisco.h,v 3.4.28.2 1996/04/19 15:12:33 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/include/csf/config/cisco.h,v $
 *------------------------------------------------------------------
 * cisco.h - Cisco-specific CSF definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cisco.h,v $
 * Revision 3.4.28.2  1996/04/19  15:12:33  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.5  1996/04/16  03:55:00  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.4  1996/04/09  20:51:40  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.3  1996/04/02  08:26:30  carrel
 * cvs fixes
 *
 * Revision 3.1.2.2  1996/03/05  04:47:21  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.1.2.1  1996/01/23  05:57:54  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.4.28.1  1996/03/18  20:31:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.4.1  1996/03/07  10:02:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/13  01:34:15  che
 * CSCdi46854:  Kerberos KDC ports should be definable for each KDC server
 *
 * Revision 3.3  1996/01/04  22:52:53  che
 * CSCdi46133:  Kerberos KDC port should be configurable
 * Added "kerberos kdc-port <port number>" command.
 *
 * Revision 3.2  1995/11/17  17:47:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSF_CONFIGX_H_
#define _CSF_CONFIGX_H_ "%W% %G%"
#endif

/*---------------------------------------------*/
#ifdef CSF_BUILD_BEGIN

#define KERBEROS_KDC_PORT 88

#endif /* CSF_BUILD_BEGIN */
/*---------------------------------------------*/

/*---------------------------------------------*/
#ifdef CSF_BUILD_HDRI

#include "master.h"
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <ciscolib.h>

struct tm {
	int tm_year, tm_mon, tm_mday, tm_hour, tm_min, tm_sec,
		tm_wday, tm_yday, tm_isdst;
};
extern struct tm *gmtime(void *), *localtime(void *);

typedef unsigned long csf_time_t;
#define csf_time_t csf_time_t
#define csf_realloc(x, y) realloc(x, y)
#define csf_calloc(x, y) csfCcalloc1(x, y)
#define csfCcalloc(x, y) csfCcalloc1(x, y)
#define csf_malloc(x) malloc(x)
#define csf_free(x) free(x)
#define csfCmemcpy(dest, src, cnt) memcpy(dest, src, cnt)
#define csfCmemcmp bcmp
#define csf_strncmp strncmp
#define csf_strdup strdup
#define csf_strchr strchr
#define csf_strcmpi csfCstrcmpi
#define csf_strupper csfCstrupper
extern unsigned int cisco_kerb_rand(void);
extern void cisco_kerb_srand(unsigned int);

extern csf_time_t cisco_kerb_time(csf_time_t *);
extern struct tm *csf_gmtime1(ulong *);

#define csfNET_handle tcbtype

#ifdef CSF_HDRI_STDIO
#define BUFSIZ (128)
#define EOF (-1)
typedef struct file_ FILE;
extern char *getenv( const char * );
#endif /* CSF_HDRI_STDIO */

#ifdef CSF_HDRI_ERRNO
/* #define _csfSTSerrno(x) (???) */
#endif /* CSF_HDRI_ERRNO */

#ifdef CSF_HDRI_FILEIO
#include <types.h>
#define O_WRONLY (1)		/* Args to open(0) */
#define O_RDONLY (2)
#define O_RDWR (3)
#define O_CREAT (0x100)
#define O_EXCL (0x200)
#define O_TRUNC (0x400)
#define S_IFMT (0xFF)		/* Values to massage stat() */
#define S_IFREG (0x1)
#define SEEK_CUR (1)		/* Args "whence" to lseek() */
#define SEEK_SET (0)
struct stat {
	int st_uid;
	int st_mode;
	int st_size;
};
typedef int uid_t;
#endif /* CSF_HDRI_FILEIO */

#ifdef CSF_HDRI_NET
#include <types.h>
#include "../tcp/tcp.h"
struct in_addr {
	ulong s_addr;
};
struct sockaddr_in {
	enum { AF_INET, AF_MAX } sin_family;
	ushort sin_port;
	struct in_addr sin_addr;
};
enum _socktype { SOCK_DGRAM, SOCK_STREAM };
struct sockaddr {
	uchar sa_len;
	 uchar sa_family;
	uchar sa_data[14];
};
int getdomainname (
  	char * name,
  	size_t maxlen
);
#endif /* CSF_HDRI_NET */

#endif /* CSF_BUILD_HDRI */
/*---------------------------------------------*/

/*---------------------------------------------*/
#ifdef CSF_BUILD_CFG

/* Motorola 68020+ -- 32-bit, MSB, no alignment requirements. */
#define CSF_CFG0_M_WORDSIZE		32
#define CSF_CFG0_M_MSBF			1
#define CSF_CFG0_M_RALIGN		1

/* Base value for status codes--ALL csf status codes. */
#ifndef CSF_CFG_STSBASE
#define CSF_CFG_STSBASE			4096
#endif

/* Turn off module source identification by default. */
#ifndef csfSRCIDGEN_
#define csfSRCIDGEN_ 0
#endif

#define CSF_CFG0_M_WORDSIZE             32
#define CSF_CFG0_M_MSBF                 1
#define CSF_CFG0_M_RALIGN               1

#define CSFC5_CFG_NET_MAXHOSTNLEN	64

#endif /* CSF_BUILD_CFG */
/*---------------------------------------------*/

/*---------------------------------------------*/
#ifdef CSF_BUILD_END

extern csf_sts csfC5_net_getfullhostname(const char *, char *, csf_size_t);
extern csf_sts csfC5_get_default_realm(char **);
extern csf_sts csf_free_krbhst(char **);

#endif /* CSF_BUILD_END */
/*---------------------------------------------*/
