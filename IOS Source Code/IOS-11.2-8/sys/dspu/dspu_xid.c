/* $Id: dspu_xid.c,v 3.3.10.1 1996/03/18 19:33:35 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/dspu/dspu_xid.c,v $
 *------------------------------------------------------------------
 * DSPU XID Utilities.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This contains utilities to decode various XIDs.
 *------------------------------------------------------------------
 * $Log: dspu_xid.c,v $
 * Revision 3.3.10.1  1996/03/18  19:33:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.28.3  1996/03/16  06:40:41  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1.28.2  1996/03/07  08:45:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1.28.1  1996/02/20  13:47:57  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/26  17:33:03  ppearce
 * CSCdi49863:  DSPU reports info from CV 0x22 incorrectly
 *
 * Revision 3.2  1996/02/22  06:33:28  ppearce
 * CSCdi47803:  DSPU retries the connection sequence to the host too fast
 *   Detect CV 0x22 in XID rsp from host and delay reconnect retry
 *
 * Revision 3.1  1995/11/09  11:28:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:03:13  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "dspu.h"
#include "dspu_xid.h"
#include "bfrprim.h"
#include "dspu_util.h"




/*=====================================================================*
 * XidGetXidData: 
 *=====================================================================*
 *
 * Description:
 *   Get address of xid data (& its length) from ID.IND or ID_STN.IND.
 *
 **/

void
XidGetXidData (TCLSIMsg *clsiMsgP,
               byte **xidP,
               hword *xidSz)

{

   byte dataSize; 
   IdStnInd_t *idStnIndP;


   switch (CLSIMsgGetPrimitiveCommand (clsiMsgP)) {

    case CLS_ID_STN:
      idStnIndP = (IdStnInd_t *) clsiMsgP;
      *xidP = CLSIIdStnGetXID(idStnIndP, &dataSize);
      *xidSz = dataSize;
      break;

    case CLS_ID:
      if ((clsiMsgP->fPrimitiveCode & CLS_TYPE_MASK) == CLS_INDICATION) {
         IdInd_t * idIndP = (IdInd_t *) clsiMsgP;
         *xidSz = idIndP->fCLSIHdr.fDataSize - 
	    (offsetof(IdInd_t,fXID[0]) -  offsetof(IdInd_t,fVersion) +
	     idIndP->fReserveLength);
         *xidP = &(idIndP->fXID[idIndP->fReserveLength]);
      }
      else {			/* must be confirm, check before it comes here */
         IdCnf_t * idCnfP = (IdCnf_t *) clsiMsgP;
         *xidSz = idCnfP->fCLSIHdr.fDataSize - 
	    (offsetof(IdCnf_t,fXID[0]) - offsetof(IdCnf_t,fVersion) +
	     idCnfP->fReserveLength);
         *xidP = &(idCnfP->fXID[idCnfP->fReserveLength]);
      }
      break;

    default:
      *xidSz = 0;
      *xidP = 0;
      break;
   }

} /* XidGetXidData. */


/*=====================================================================*
 * XidGetFormat: 
 *=====================================================================*
 *
 * Description:
 *   Get xid format from ID.IND, or ID_STN.IND.
 *
 **/

XidFmtE
XidGetFormat (TCLSIMsg *clsiBfrP)

{

   byte *xidP;
   hword xidLn;
   byte xidType;
   XidFmtE xidFmt;


   XidGetXidData (clsiBfrP, &xidP, &xidLn);
   if (xidLn > 0) {
      xidType = xidP[0] >> 4;
      switch (xidType) {
       case 0:
         xidFmt = xidFmt_0;
         break;
       case 3:
         xidFmt = xidFmt_3;
         break;
       default:
         xidFmt = xidFmt_Bad;
         break;
      }
   }
   else
      xidFmt = xidFmt_Null;

   return (xidFmt);

} /* XidGetFormat. */


/*=====================================================================*
 * XidGetXid: 
 *=====================================================================*
 *
 * Description:
 *   Get xid (idblk.idnum) from xid data.
 *
 **/

word
XidGetXid (TCLSIMsg *clsiMsgP)

{

   word xid;
   byte *xidP;
   hword xidLn;


   XidGetXidData (clsiMsgP, &xidP, &xidLn);
   if (xidLn >= 6)
      memcpy (&xid, xidP+2, sizeof(xid));
   else
      xid = 0;

   return (xid);

} /* XidGetXid. */


/*=====================================================================*
 * XidGetMaxIFrame: 
 *=====================================================================*
 *
 * Description:
 *   Get maxIframe size from ID.(IND,CNF) xid data.
 *
 **/

hword
XidGetMaxIFrame (TCLSIMsg *clsiMsgP)

{

   hword maxIFrame;
   byte *xidP;
   hword xidLn;


   XidGetXidData (clsiMsgP, &xidP, &xidLn);
   if (xidLn > 22) {
      maxIFrame = (hword) xidP[21] << 8;
      maxIFrame += (hword) xidP[22];
   }
   else
      maxIFrame = 0;

   return (maxIFrame);

} /* XidGetMaxIFrame. */


/*=====================================================================*
 * XidGetWindowSize: 
 *=====================================================================*
 *
 * Description:
 *   Get WindowSize from ID.(IND,CNF) xid data.
 *
 **/

byte
XidGetWindowSize (TCLSIMsg *clsiMsgP)

{

   byte *xidP;
   hword xidLn;
   byte windowSize;


   XidGetXidData (clsiMsgP, &xidP, &xidLn);
   if (xidLn > 27)
      windowSize = xidP[27];
   else
      windowSize = 0;

   return (windowSize);

} /* XidGetWindowSize. */



/*=====================================================================*
 * XidCtlVec22_Updt: 
 *=====================================================================*
 *
 * Description:
 *   Set XID negotiation error (0x22) information.
 *
 **/

void
XidCtlVec22_Updt (XidCtlVec22T *cv22P,
                  byte badByte,
                  byte badBit)

{

   cv22P->byte[0] = NEGOTIATION_ERROR;
   cv22P->byte[1] = 3;		/* 3 bytes followed */
   cv22P->byte[2] = 0;
   cv22P->byte[3] = badByte;
   cv22P->byte[4] = badBit;

} /* XidCtlVec22_Updt. */


/*=====================================================================*
 * XidQueryNegError: 
 *=====================================================================*
 *
 * Description:
 * Query the XID buffer for presence of an XID negotiation error
 * control vector. If one exists fill in the supplied logical control
 * vector indicating <byte/bit> offset of field in error in XID.
 *
 **/

boolean
XidQueryNegError (TCLSIMsg *clsiMsgP,
                  XidNegErrT *xidNegErrP)

{

   byte *xidDataP;
   hword xidDataLn;
   short lenLeft;

   XidGetXidData (clsiMsgP, &xidDataP, &xidDataLn);
   xidDataP += 18;		/* point to the variable length field */
   lenLeft = xidDataLn - 18 - *xidDataP; /* data length left after the first control vector */
   xidDataP += *xidDataP;	/* point to the first control vector */
   
   while (lenLeft > 1){		/* search for control vector 22 */
      if (*xidDataP == 0x22){   /* control vector found */
         memset(xidNegErrP, 0, sizeof(XidNegErrT));
         memcpy(&(xidNegErrP->byteOffset), &(xidDataP[2]), 2);
         memcpy(&(xidNegErrP->bitOffset),  &(xidDataP[4]), 1);
         if (xidDataP[1] > 3) {   /* Optional senseData present? */
             memcpy(&(xidNegErrP->senseData), 
                      ByteToHexAscii(&(xidDataP[5]), 4), 8);
         }
         return TRUE;
      }
      lenLeft -= (xidDataP[1] + 2);
      xidDataP += (xidDataP[1] + 2);
   }
   return FALSE;		/* no vector 22 found */
}


