/* $Id: snautil.h,v 3.3.10.1 1996/05/17 10:49:49 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/snautil.h,v $
 *------------------------------------------------------------------
 * DSPU SNA Utilities.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * Headers for some SNA utilities.
 *------------------------------------------------------------------
 * $Log: snautil.h,v $
 * Revision 3.3.10.1  1996/05/17  10:49:49  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.2.1  1996/04/25  23:57:16  bchan
 * CSCdi55817:  gs7-s-mz broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3  1996/02/26  19:04:23  ppearce
 * CSCdi49872:  DSPU cannot establish connection with VTAM through CIP
 *   Insert CV 0x0E and CV 0x10 into XID3
 *
 * Revision 3.2  1995/11/17  09:06:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:30:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../ibm/ibm_types.h"
#include "dtypes.h"
#include "../cls/clsi_msg.h"

/*=====================================================================*
* ConvertAsciiToEbcdic
*======================================================================*
*
* Description:
*   This routine converts a given ascii string to ebcdic.    
*   
**/
void ConvertAsciiToEbcdic
       (const byte * ascii
       ,byte       * ebcdic
       ,int         length);
       

void SetResourceNameInEbcdic
       (const byte * ascii
       ,byte       * ebcdic);
       
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
          ,byte   len);

/*=====================================================================*
* SetPuName
*======================================================================*
*
* Description:
*   This routine sets the pu name by converting it from ebcdic
* to ascii.
*
**/
void SetPuName
       (DataReq_t* clsiBfrP
       ,byte *name);


/*=====================================================================*
* ConvertToEbcdicName
*======================================================================*
*
* Description:
*   This routine converts ascci to ebcdic(8 bytes) name with blank.
*
**/
void ConvertToEbcdicName(const byte* ascii, byte* ebcdic);
void ConvertToEbcdicPuName(const byte* ascii, byte* ebcdic);

extern const byte asciiToEbcdic[];


