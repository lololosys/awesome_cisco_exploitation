/* $Id: mv_prodid.c,v 3.2.58.2 1996/08/07 06:20:55 szdravko Exp $
 * $Source: /release/112/cvs/Xsys/dspu/mv_prodid.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Product Set ID major vector
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mv_prodid.c,v $
 * Revision 3.2.58.2  1996/08/07  06:20:55  szdravko
 * CSCdi62268:  Wrong output in NetMaster console coming from NSP
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/18  19:33:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  08:45:56  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  13:48:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:05:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "mv_prodid.h"
#include "snautil.h"

ProductId productSetId =
{
   sizeof(ProductId), 0x10, 0,
   offsetof(ProductId, swLen) - offsetof(ProductId, hwLen),
   0x11,   /* product identifier */
   0x9,    /* non IBM hardware */

   16,
   0,      /* hardware product id */
   0x10,   /* machine type + plant + seq */
   {0xf0,0xf0,0xf0,0xf0},  /* machine type */

   {0x40,0x40},       /* plant number */
   {0xf0,0xf0,0xf0,0xf0,0xf0,0xf0,0xf0},
   

   sizeof(ProductId) - offsetof(ProductId, swLen),
   0x11,
   0xc,   /* non IBM software */
   8,
   4,             /* common level */
   {0xf0,0xf0},     /* version */
   {0xf0,0xf0},     /* release */
   {0xf0,0xf0},      /* modification */
   9,
   8,             /* software product */
   {0x40,0x40,0x40,0x40,0x40,0x40,0x40},
  0x12,0x06,     
  {0x83,0x89,0xA2,0x83,0x96,0x40,0xE2,0xD5,0xC1,0x40,0xD9,0x96,0xA4,0xA3,0x85,0x99}
};
  




/* The software version is in ascii string, we extract the software
 * product as () and the version is numeric digit
 */


/* Get the least significant 2 digits of the version
 * string
 */
static byte* GetEbcdic(byte* ascii, byte* ebcdic)
{
    byte c;

    ebcdic[0] = 0xf0;
    ebcdic[1] = 0xf0;

    while ((c = *ascii++) != 0 && (c < '0' || c > '9'))
         ;

    while (c >= '0' && c <= '9'){
       ebcdic[0] = ebcdic[1];
       ebcdic[1] = c - '0' + 0xf0;
       c = *ascii++;
    }
    return c? ascii : 0;
}

/* Extract the software and version numbers.  The string "version" contains the
 * first two lines of output when "show version" is entered.  For example,
 * "Cisco Internetwork Operating System Software 
 *  IOS (tm) 3000 Software (IGS-J-L), Experimental Version 11.2(14345) [szdravko 107]"
 * The definition for this version string is contained in the cisco*.c files (found
 * in the object directory).
 */
static void ExtractVersion(char* buf, byte* ver, byte* release, byte* modification)
{
   char* s = version;
   byte c;
   int maxChar;
   while ((c = *s++) != 0){
      /* Extract the software type defined in the parentheses.
       * i.e. in the above example its "IGS-J-L"
       */
      if (c == '(' && (c = *s++) >= 'A' && c <= 'Z'){
         *buf++ = c;
         maxChar = 1;
         while ((c = *s++) != 0 && c != ')' && maxChar++ < 8)
            *buf++ = c;
         if (c == 0)
            return;

         /* Extract the version number and converts to ebcdic.  
          * i.e. in the above example, version = 11, release = 02
          * and modification = 45.  Note that in the case of modification  
          * its the last two significant digits.
          */
         *buf = 0;
         if ((s = GetEbcdic(s, ver)) != 0)
             if ((s = GetEbcdic(s, release)) != 0)
                  GetEbcdic(s, modification);
         return;
      }
  }
  *buf = 0;
}


/* The last seven non-blank characters of the host name padded by left with 0xf0
 * is used for sequence
 */

static int useSerial = FALSE;

void SetSequenceNumber(const char* hostName)
{

   int len;
   const char* src;
   char* dest;
   if (!useSerial) {
      if ((len = strlen(hostName)) > 7){
          src = hostName + 1;
          dest = productSetId.sequence;
           len = 7;
      }
      else {
         src = hostName;
         dest = &productSetId.sequence[7 - len];
      }
      memset(productSetId.sequence, 0xf0, 7 - len);   /* set top byte to ebcdic 0 */
      ConvertAsciiToEbcdic(src, dest,  len);
   }


}

void ProductIdInit (void)
{
   byte buffer[16];
   int  len;
   char* prodP;   
   
   char* serial = platform_get_string(PLATFORM_STRING_HARDWARE_SERIAL);
   if (serial && *serial){
      useSerial = TRUE;
      ConvertAsciiToEbcdic(serial, productSetId.sequence,  7);
   }

   ExtractVersion(buffer, productSetId.version, 
                   productSetId.release, productSetId.modification);

   if ((len = strlen(buffer)) > 7)
      len = 7;
   ConvertAsciiToEbcdic(buffer, productSetId.softProductId, len);

   prodP = platform_get_string(PLATFORM_STRING_NOM_DU_JOUR);
   if (prodP[0] < '0' || prodP[1] > '9') {
     /*
       Product string is non-numeric.
       If it's "RP1" or "RSP7000" it's a 7000, "RSPx" is a 7500.
       If string starts with "CPA", router is a CiscoPro, and
       uses the string format "CPAxxxx", where xxxx is the router type.
       If none of the above, model is unknown, use 0000.
     */
 
     if ((strcmp(prodP,"RP1") == 0) ||
	 (strcmp(prodP,"RSP7000")== 0)) {
       productSetId.machineType[0] = 0xf7;
       productSetId.machineType[1] = 0xf0;
     } else if (strncmp(prodP,"RSP",3) == 0) {
       productSetId.machineType[0] = 0xf7;
       productSetId.machineType[1] = 0xf5;
     } else if (strncmp(prodP,"CPA",3) == 0) {
       ConvertAsciiToEbcdic(prodP+3,productSetId.machineType, 2);  
     } else {
       productSetId.machineType[0] = 0xf0;
       productSetId.machineType[1] = 0xf0;
     }
   } else {      
     ConvertAsciiToEbcdic(prodP,productSetId.machineType, 2);
   }
}

  
