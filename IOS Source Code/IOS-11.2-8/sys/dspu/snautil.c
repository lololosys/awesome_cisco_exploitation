/* $Id: snautil.c,v 3.3.10.2 1996/05/17 10:49:46 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/snautil.c,v $
 *------------------------------------------------------------------
 * DSPU SNA Utilities.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Some SNA Conversion Utilities.
 *------------------------------------------------------------------
 * $Log: snautil.c,v $
 * Revision 3.3.10.2  1996/05/17  10:49:46  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.2.2  1996/04/25  23:57:14  bchan
 * CSCdi55817:  gs7-s-mz broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.2.1  1996/04/03  14:04:01  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.10.1  1996/03/18  19:34:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  06:41:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  08:46:17  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:48:58  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/26  19:04:21  ppearce
 * CSCdi49872:  DSPU cannot establish connection with VTAM through CIP
 *   Insert CV 0x0E and CV 0x10 into XID3
 *
 * Revision 3.2  1995/11/17  09:06:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:30:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>

#include "dspu.h"
#include "snautil.h"
#include "snaprim.h"

/*---- Static data. */

static hword defPuNameCount;

static const byte ebcdicToAscii[64] = 
  {' ','A','B','C','D','E','F','G','H','I',' ',' ',' ',' ',' ',' ',
   ' ','J','K','L','M','N','O','P','Q','R',' ',' ',' ',' ',' ',' ',
   ' ',' ','S','T','U','V','W','X','Y','Z',' ',' ',' ',' ',' ',' ',
   '0','1','2','3','4','5','6','7','8','9',' ',' ',' ',' ',' ',' '
  };

/*---- Forward declared prototype definitions. */

static boolean
EbcdicStringIsSpaces (byte *string, hword stringLen);


void SetResourceNameInEbcdic
       (const byte * ascii
       ,byte * ebcdic)
{

  /*---- If we have a null PU name, create a default one. */
  if (strlen(ascii) == 0) {
     byte  defaultName[10];
     defPuNameCount++;                  /* This is updated in SetPuName() too. */
     sprintf(defaultName, "%s%04d", "DFLT", defPuNameCount);
     ascii = defaultName;
  }
  memset(ebcdic, 0x40, 8);              /* Now set buffer to EBCDIC spaces. */
  ConvertAsciiToEbcdic(ascii, ebcdic,  strlen(ascii));
}

/*=====================================================================*
* ConvertEbcdicToAscii
*======================================================================*
*
* Description:
*   This routine converts a given ebcidic string to ascii. It is 
* rejected if it is not uppercase and numeric.   
*   
**/
boolean ConvertEbcdicToAscii
          (byte * ebcdic
          ,byte * ascii
          ,byte   len)
{
  boolean flag=TRUE;

  while ((len-- > 0) && flag)
  {
    if (*ebcdic > 0xC0)
      *ascii++ = ebcdicToAscii[*ebcdic++ - 0xC0];
    else
      flag=FALSE;
  }  

  if (flag) 
    *ascii='\0';

  return flag;
}

/*============================================================================*
* EbcdicStringIsSpaces:
*=============================================================================*
*
* Description:
*
**/

static boolean
EbcdicStringIsSpaces (byte *string,
                      hword stringLen)

{

  boolean isSpaces = TRUE;
  hword i;

  i = 0;
  while ((i < stringLen) && (isSpaces)) {
    isSpaces = (string[i] == 0x40);
    i++;
  }
  return (isSpaces);

}  /* EbcdicStringIsSpaces. */


/*=====================================================================*
* SetPuName
*======================================================================*
*
* Description:
*
**/
void SetPuName(DataReq_t* clsiBfrP, byte *name)
{
  SNA  * snaP;

  snaP = (SNA *)ReturnDataHandle(clsiBfrP);

  if (EbcdicStringIsSpaces(&snaP->ruData[2], 8) ||
      !ConvertEbcdicToAscii(&snaP->ruData[2], name, 8)) 
  {
    /* Detected invalid ebcdic data so set name as default */
    defPuNameCount++;                   /* This is updated in SetResourceNameInEbcDic(). */ 
    sprintf(name,"%s%04d","DFLT",defPuNameCount); 
  }  
}

/*=====================================================================*
* ConvertToEbcdicName
*======================================================================*
*
* Description:
*   This routine converts ascii to ebcdic(8 bytes) name with blank.
*
**/
void ConvertToEbcdicName(const byte* ascii, byte* ebcdic)
{
   int len = strlen(ascii);
   if (len > 8)
       len = 8;
   else
       memset(ebcdic, 0x40, 8);
   ConvertAsciiToEbcdic(ascii, ebcdic,  len);
}

/*=====================================================================*
* ConvertToEbcdicPuName
*======================================================================*
*
* Description:
*   This routine converts ascii to ebcdic name (8 chars).
*   Name was already validated in parser, using DspuIsValidPuName().
**/
void ConvertToEbcdicPuName(const byte* ascii, byte* ebcdic)
{

    byte i,j; 
    byte c;
    char puName[8];
   
    memset(ebcdic, 0x40, 8);
    ConvertToEbcdicName(ascii, puName);
    for (i=0, j=0; i<8; i++) {
      c = puName[i];
      ebcdic[j++] = puName[i];
    }
}
