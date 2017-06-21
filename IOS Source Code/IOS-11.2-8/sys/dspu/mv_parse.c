/* $Id: mv_parse.c,v 3.2.58.1 1996/07/29 07:04:06 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/mv_parse.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Major vector parser
 * This is part of focal point and is therefore part of dspu subsystem.
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mv_parse.c,v $
 * Revision 3.2.58.1  1996/07/29  07:04:06  lcox
 * CSCdi57699:  watchdog crash on invalid NMVT handling
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:05:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "mv_parse.h"
/*=====================================================================*
* MV iteration routines, MV_FindMV  MV_NextMV
*======================================================================*
*
* Description:
*
*   Iterate the buffer to find a specific MV or find next MV
**/

MV* MV_FindMV(MV* mv, hword mvId, hword len)
{
   hword  msLen;
   for (msLen = MV_Len(mv); msLen + sizeof(MV) < len;
                 (byte*) mv += msLen, len -= msLen){
      if (MV_Key(mv) == mvId)
         return mv;
      
   }
   return 0;
}

MV* MV_NextMV(MV* mv, hword len)
{
   hword  msLen;
   if ((msLen = MV_Len(mv)) + sizeof(MV) < len)
        return ((MV*) (byte*) mv + msLen);
   return 0;
}

/*=====================================================================*
* MV_FindSv
*======================================================================*
*
* Description:
*
*   Iterate the MV to find a specifiv SV
**/

SV* MV_FindSV(MV* msVector, byte svId)  /* finds spec'd subvector */
{
   int  lenLeft;
   SV*   svP = &msVector->sv;
   hword  msLen = MV_Len(msVector);
   for (lenLeft = msLen - 4; lenLeft > 0; 
              lenLeft -= svP->len, (byte*)svP += svP->len) {
      if (svP->key == svId)
         return svP;

      /* check for SV with length field 0 - this is not a valid value
      * but a remote device may make this error and if we do not make
      * this check then we will be in an infinite loop and crash in the
      * watchdog
      */  
      if (svP->len == 0)
         return 0; 
            
   }
   return 0;
}
/*=====================================================================*
* SV_FindSv
*======================================================================*
*
* Description:
*
*   Iterate the SV to find a specifiv SV
**/

SV* SV_FindSV(SV* subVector, byte sfId)  /* find specific subfiled */
{
   int lenLeft;
   SV* sfP = subVector + 1;   /* subfield format is the same as sub Vec */
   for (lenLeft = subVector->len -2;  lenLeft > 0; 
                lenLeft -= sfP->len, (byte*)sfP += sfP->len){
      if (sfP->key == sfId)
         return sfP;

      /* check for SF with length field 0 - this is not a valid value
      * but a remote device may make this error and if we do not make
      * this check then we will be in an infinite loop and crash in the
      * watchdog
      */  
      if (sfP->len == 0)
         return 0; 
            
   }
   return 0;
}


