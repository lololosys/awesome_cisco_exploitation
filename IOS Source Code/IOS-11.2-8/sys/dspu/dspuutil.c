/* $Id: dspuutil.c,v 3.2.60.1 1996/03/18 19:33:43 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/dspu/dspuutil.c,v $
 *------------------------------------------------------------------
 * DSPU Utility Programs.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * These are mainly conversion utilities.
 *------------------------------------------------------------------
 * $Log: dspuutil.c,v $
 * Revision 3.2.60.1  1996/03/18  19:33:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:45:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:48:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:05:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/14  06:15:25  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 * Revision 2.1  1995/06/07  20:29:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "dspu.h"
#include "ctype.h"
#include "dspu_util.h"

#define MAX_BUFFER_LN 64
#define MAX_SAP_ASCII_LN 3


/*=====================================================================*
* HexAsciiToByte
*======================================================================*
*
* Description:
*   Convert ascii hex char to int. This is used to convert ascii xid 
* into xid, or ascii mac to hex mac
*
**/
void
HexAsciiToByte (const char* ascii,      /* ascii data to be converted */      
                byte* saveArea,         /* converted hex data save area*/
                hword len)              /* length of digits           */

{

   byte c;
   hword odd;
   hword srcLen = strlen(ascii);  /* find the data length */
   
   if (srcLen > (len*2))
      srcLen = len*2;             /* prevent overrun */

   while (len > (srcLen+1)/2){
      *saveArea++ = 0;           /* fill up  leading zero */
      len--;
   }
   *saveArea = 0;
   
   odd = (srcLen & 1);
   while (srcLen-- > 0)
   {
      if ((c = *ascii++)>= 'a' && c <= 'f')
          c = c - 'a' + 10;
      else if (c >= 'A' && c <= 'F')
          c = c - 'A' + 10;
      else
          c -= '0';
      
      if ((odd ^= 1) == 0){
         *saveArea <<= 4;
         *saveArea++ |= c;
      }
      else
         *saveArea = c;
   }
   
}  /* HexAsciiToByte. */


/*=====================================================================*
* ByteToHexAscii
*======================================================================*
*
* Description:
* This is used to convert internal mac, xid, etc data into ascii hex
* character data.
*
**/
char*                                   /* The converted ascii buffer. */
ByteToHexAscii (const byte* hex,        /* The hex buffer. */
                hword len)              /* Length of hex buffer. */

{

   char* p;
   static char printBuf[MAX_BUFFER_LN];


   if (len > (MAX_BUFFER_LN/2 - 1))
      len = (MAX_BUFFER_LN/2 - 1);

   for (p = printBuf; len--; ) {
      *p++ = tohexchar(*hex >> 4);
      *p++ = tohexchar(*hex++);
   }
   *p = 0;

   return printBuf;
   
}  /* ByteToHexAscii. */


/*=====================================================================*
* MacToAscii
*======================================================================*
*
* Description:
*    Convert mac into ascii data and add '.' to give the format of
* xxxx.xxxx.xxxx
*
**/
char* MacToAscii(const byte* mac)
{
   int i, j;
   char* asciiP = ByteToHexAscii(mac, 6);
   
   char* s = &asciiP[11];     /* end of mac string */
   char* p = &asciiP[14];     /* null termination location of new string */
   *p-- = 0;                  /* null terminated */
   
   for (i = 2; i--;){
      for (j = 4; j--;)       /* copy 4 bytes backward */
         *p-- = *s--;
      *p-- = '.';             /* insert dot on the 4th byte */
   }
   return asciiP;
}

/*
* AsciiToQllcSubAddr:
*
* Description:
*    Convert and map qllc addresses (in Ascii) to a byte value. 
*    Single digits have a hex "F" padded to them (i.e an "8" --> F8).
*    Double digits are mapped one-to-one (i.e "59" --> 59 and "08" --> 8).
*    
*/

byte
AsciiToQllcSubAddr (const char* subAddr) 
{

    byte res;

    /* check the subAddr's range validity */
    if ((strlen(subAddr) > 2) || (!isdigit(subAddr[0]))) {
        return (0xFE);
    }

    /*                                                        
     *  perform the mapping according to the number of digits.
     *
     */
    if (strlen(subAddr) == 1)
        res = 0xF0 + (subAddr[0] - '0');        
    else  
        res = (subAddr[0] - '0')*10 + (subAddr[1] - '0');

    return (res);
}

/*
* QllcSubAddrToAscii:
*
* Description:
*    Convert and map the qllc address (in byte) into ASCII character.
*    It reverses the process of the "AsciiToSubAddr()" function defined
*    above.
*
*/

void
QllcSubAddrToAscii (byte subAddr, char* qllcAddr)
{
    ushort i = 0;

    if (subAddr < 0xF0) {
        qllcAddr[i++] = (subAddr/10) + '0';
	subAddr = subAddr % 10;
	qllcAddr[i++] = subAddr + '0';
    } else 
        qllcAddr[i++] = (subAddr - 0xF0) + '0'; 

    qllcAddr[i] = '\0';

}/* QllcSubAddrToAscii*/

/*=====================================================================*
* GetDataBufferInHexAscii:
*======================================================================*
*
* Description:
*   Convert byte data into ASCII HEX character data
*
**/

void
GetDataBufferInHexAscii (byte* inBufP,
                         char* outBufP,
                         hword inBufLen)

{

   hword i;


   if (inBufLen > MAX_IFRAME_TRC_DATA_LN)
      inBufLen = MAX_IFRAME_TRC_DATA_LN;

   for (i = 0; i < inBufLen; i++) {
      sprintf(outBufP, "%02x", *inBufP++);
      outBufP = outBufP+2;
      if ((i & 31) == 31) {
         *outBufP++ = '\n';
         *outBufP++ = ' ';
      } else if ((i & 7) == 7) {
         *outBufP++ = ' ';
      }
   }

}  /* GetDataBufferInHexAscii */


/*============================================================================
* Memcmp
*=============================================================================
*
* Description:
*    Perform ANSI memory compare (memcmp) function.
*
**/
int Memcmp(const void* s1, const void* s2, size_t n)
{
   const unsigned char *su1, *su2;
   
   for (su1 = s1, su2 = s2; 0 < n; su1++, su2++, n--)
   {
      if (*su1 != *su2)
         return ((*su1 < *su2)? -1: 1);
   }
   return 0;
}


/*============================================================================
* strcopyup
*=============================================================================
*
* Description:
*    Copy and uppercase a string from source to target. Source must be null
*    terminated. Use same calling convention as Cisco's character utility fn's.
*
**/
void
strcopyup (char* src, char* dest, int stringLen)
{

    while ((--stringLen > 0) && *dest)
	*dest++ = toupper(*src++);
    *dest = '\0';

}  /* strcopyup. */
