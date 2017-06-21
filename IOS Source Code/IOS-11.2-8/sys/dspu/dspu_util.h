/* $Id: dspu_util.h,v 3.1 1995/11/09 11:28:47 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dspu_util.h,v $
 *------------------------------------------------------------------
 * DSPU Utilities.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * DSPU utility programs
 *------------------------------------------------------------------
 * $Log: dspu_util.h,v $
 * Revision 3.1  1995/11/09  11:28:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/14  06:15:15  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DSPU_UTIL_H__
#define __DSPU_UTIL_H__

#include "dtypes.h"


void
HexAsciiToByte (const char* ascii,      /* ascii data to be converted */      
                byte* saveArea,         /* converted hex data save area*/
                hword len);             /* length of digits           */

char*                                   /* The converted ascii buffer. */
ByteToHexAscii (const byte* hex,        /* The hex buffer. */
                hword len);             /* Length of hex buffer. */

void
GetDataBufferInHexAscii (byte* inBufP,     /* Input  byte buffer          */
                         char* outBufP,    /* Output char buffer          */
                         hword inBufLen);  /* Length of input byte buffer */

char* MacToAscii(const byte* mac);

void QllcSubAddrToAscii(byte subAddr, char* qllcAddr);
byte AsciiToQllcSubAddr(const char* subAddr);

/* same as memcmp */

int Memcmp(const void* s1, const void* s2, size_t n);

void
strcopyup (char* src, char* dest, int stringLen);

#define swab(a,b) { byte c = b; b = a; a = c; }

#endif  /* __DSPU_UTIL_H__ */
/*---- End include file: dspu_util.h. */
