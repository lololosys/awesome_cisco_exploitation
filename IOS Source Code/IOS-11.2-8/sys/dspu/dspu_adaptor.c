/* $Id: dspu_adaptor.c,v 3.1.62.4 1996/08/28 12:41:55 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_adaptor.c,v $
 *------------------------------------------------------------------
 * DSPU Adaptor Procedures.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This module contains the methods for the DSPU Adaptor control
 * block, and the code to process the Adaptor CLSI commands.
 *------------------------------------------------------------------
 * $Log: dspu_adaptor.c,v $
 * Revision 3.1.62.4  1996/08/28  12:41:55  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.62.3  1996/05/27  06:01:30  jjohnson
 * CSCdi58758:  some errmsg invocations do not following coding conventions
 * Also fix a bunch of ByteToHexAscii invocations since they are mostly
 * colocated with errmsg invocations
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/05/17  10:47:22  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.50.3  1996/05/03  17:18:13  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.50.2  1996/04/03  14:02:27  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1.50.1  1996/03/28  08:20:20  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.1.62.1  1996/03/18  19:33:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.28.2  1996/03/07  08:45:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1.28.1  1996/02/20  13:47:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.1  1995/11/09  11:27:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/19  21:12:53  ppearce
 * CSCdi42171:  SNANM/DSPU cannot be config'd on Frame Relay sub-interfaces
 *   Allow SNANM/DSPU config commands on sub-interfaces
 *
 * Revision 2.1  1995/09/20  17:02:40  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */





/* This template defines the appropriate parent/child typedefs 
 * for adaptors. 
 */
#define ADAPTOR_TEMPLATE

#include "master.h"
#include "logger.h"
#include "msg_dspu.c"			/* Not a typo, see logger.h */
#include <string.h>

#include "dspu.h"
#include "dspu_adaptor.h"
#include "bfrprim.h"
#include "dspu_cps.h"
#include "dspu_sap.h"
#include "dspu_ls.h"
#include "dspu_pu.h"
#include "dspu_error.h"
#include "dmalloc.h"
#include "dspu_util.h"


/*============================================================================*
* AdaptorCreate: *
*============================================================================*
*
* Description:
* Constructor for an adpator. Create ctrl blk. Send enable.req.
*
**/

Adaptor* 
AdaptorCreate (Parent_t adParentP,
               Sibling*    adChildLocP,
               AdaptorId*  adaptorIdP)

{

   Adaptor* this = DspuMalloc(sizeof(Adaptor));
   if (this) {
   
      /*---- Add this adaptor cblk into adaptor locator structure. */
      this->adChildLocP = adChildLocP;
      InitChild (MULTIPLE, &(this->sapChildren));
      this->id.dlcType = adaptorIdP->dlcType;
      this->id.u.adaptornumber = adaptorIdP->u.adaptornumber;
      this->id.adaptorSubIfNumber = adaptorIdP->adaptorSubIfNumber;
      this->id.dlcIdb = adaptorIdP->dlcIdb;
      this->state = AdaptorReset;
      this->index = AddChildFirstFree (adChildLocP,this, 
                                       adParentP, &(this->adParentP));
      if (this->index != BAD_SIBLING_INDEX) {

         /*---- Send ENABLE.REQ out across CLSI to interface. */
         if (!AdaptorEnable (this)) {
            DspuFree (this);
            this = NULL;
         }
      }
      else {			/* Failed adding adaptor. */
         DspuFree (this);
         this = (Adaptor*) NULL;
      }
   } 

   return this;

} /* AdaptorCreate. */


/*============================================================================*
 * AdaptorDelete:
 *============================================================================*
 *
 * Description:
 * Destructor for an adpator. Undo linkage & destroy ctrl blk.
 *
 **/

void
AdaptorDelete (Adaptor *adaptorP)

{

   DelChild (adaptorP->adChildLocP, adaptorP->index, &(adaptorP->adParentP));
   memset (adaptorP, 0, sizeof(Adaptor));
   DspuFree (adaptorP);

} /* AdaptorDelete. */

/*============================================================================*
 * AdaptorGetOrCreate
 *============================================================================*
 *
 * Description:
 * Create an adaptor if adaptor does not exists
 *
 **/
Adaptor*
AdaptorGetOrCreate (AdaptorId* idP)
{
   Adaptor* adaptorP;
   if ((adaptorP = QueryAllChildrenParm(&dspu.adaptorRoot,
                                        (ProcQChild1)AdaptorFind, idP)) == 0)
      adaptorP = AdaptorCreate (&dspu, &dspu.adaptorRoot, idP);
 
   if (adaptorP == 0)
      DspuError0(ConfigNoMemory);
   return adaptorP;
}

/*============================================================================*
* AdaptorFind: *
*============================================================================*
*
* Description:
* Find adaptor using type of dlc, and adaptor number.
*
**/

boolean
AdaptorFind (Adaptor*   this,
             AdaptorId* adaptorIdP)

{
   return( this->id.dlcType == adaptorIdP->dlcType && 
           this->id.u.adaptornumber == adaptorIdP->u.adaptornumber &&
           this->id.adaptorSubIfNumber == adaptorIdP->adaptorSubIfNumber);
}

/*============================================================================*
* FindNextAdaptor:  
*============================================================================*
*
* Description:
*   Find next adaptor according to lexicographical ordering criteria 
*     DlcType.unitNumber.portNumber
* 
*     E.g. Assume adapter List contains:
*            1.0.1
*            3.0.1
*            1.1.0
*            1.0.0
*            2.0.1
*
*          Given 1.0.0 -> return 1.0.1
*          Given 1.0.1 -> return 1.1.0
*          Given 2.0.1 -> return 3.0.0
*
**/

Adaptor*
FindNextAdaptor (AdaptorId* adaptorIdP)

{

  Adaptor* adaptorP;
  Adaptor* nextAdaptorP = NULL;

  DspuDLCType_e inType,    dlcType,    nextType    = 0;
  byte          inUnitNum, dlcUnitNum, nextUnitNum = 0;
  byte          inPortNum, dlcPortNum, nextPortNum = 0;


  /* Initialize given input type, unitNum, & portNum                   */
  /*   must fine next lexicographical adaptor after this given adaptor */
  /* ----------------------------------------------------------------- */
  inType    = GetAdaptorDlcType(adaptorIdP);
  inUnitNum = GetAdaptorDlcUnit(adaptorIdP);
  inPortNum = GetAdaptorDlcPort(adaptorIdP);

  /* For each adaptor in the AdaptorList:                                   */
  /*                                                                        */
  /*   Determine if the adapter identifiers (dlcType.dlcUnitNum.dlcPortNum) */
  /*     meet the lexicographical criteria of being after the given adaptor */
  /*     but before the current nextAdaptor                                 */
  /*                                                                        */
  /*   nextAdaptor is initialized the first adaptor is found that is        */
  /*      lexicographically after the give adapter                          */
  /*                                                                        */
  /*   nextAdaptor is the next-adaptor-so-far                               */
  /*     search of the AdaptorList continues to find the "real" nextAdaptor */
  /* ---------------------------------------------------------------------- */
  adaptorP = GetFirstChild(GetAdaptorRoot());
  while (adaptorP) {

      dlcType    = GetAdaptorDlcType(AdaptorGetId(adaptorP));
      dlcUnitNum = GetAdaptorDlcUnit(AdaptorGetId(adaptorP));
      dlcPortNum = GetAdaptorDlcPort(AdaptorGetId(adaptorP));

     
      if (dlcType >= inType) {
          if (dlcType == inType) {
            
              if (dlcUnitNum >= inUnitNum) {
                  if (dlcUnitNum == inUnitNum) {

                      if ( (dlcPortNum > inPortNum) && 
                           (!nextAdaptorP || (dlcPortNum < nextPortNum)) ) {
                          nextAdaptorP = adaptorP;
                      }

                  } else if (!nextAdaptorP || (dlcUnitNum < nextUnitNum)) {
                      nextAdaptorP = adaptorP;
                  }
              }

          } else if (!nextAdaptorP || (dlcType < nextType)) {
              nextAdaptorP = adaptorP;
          } 
      } 

      /* If "new" nextAdaptor was found,                     */
      /*   reset the local vars representing the nextAdaptor */
      /* --------------------------------------------------- */ 
      if (nextAdaptorP == adaptorP) {
          nextType    = GetAdaptorDlcType(AdaptorGetId(nextAdaptorP));
          nextUnitNum = GetAdaptorDlcUnit(AdaptorGetId(nextAdaptorP));
          nextPortNum = GetAdaptorDlcPort(AdaptorGetId(nextAdaptorP));
      } 
      adaptorP = GetNextChild(GetAdaptorRoot(), adaptorP->index);  

  } /* End while(adaptorP) */

  if (nextAdaptorP) {
    adaptorIdP->dlcType = GetAdaptorDlcType(AdaptorGetId(nextAdaptorP));
    adaptorIdP->u.adaptornumber = GetAdaptorDlcNumber(AdaptorGetId(nextAdaptorP));
  }
  return(nextAdaptorP);

} /* End FindNextAdaptor() */

/*============================================================================*
* AdaptorEnable: 
*============================================================================*
*
* Description:
* Send ENABLE.REQ out across CLSI.
*
**/

boolean
AdaptorEnable (Adaptor* this)

{

   EnableReq_t *enableReqP;

   if (this->state == AdaptorReset) {
      this->state = AdaptorOpening;
      enableReqP = Cnstrct_EnableReq(&this->id, this->index);
      if (enableReqP){
         DspuQueuePut ((TCLSIMsg *) enableReqP);
         return TRUE;
      }
   }

   return (FALSE);

} /* AdaptorEnable. */


/*============================================================================*
* AdaptorSapClosed: 
*============================================================================*
*
* Description:
* A sap under this adaptor is closed. If there is no sap under this adaptor,
* then send DISABLE.REQ out across CLSI.
*
**/

void AdaptorSapClosed (Adaptor* this)

{

   DisableReq_t *disableReqP;

   if (!QueryAnyChild(&this->sapChildren)) {
      disableReqP = Cnstrct_DisableReq (&this->id, this->index);
      if (disableReqP) {
          this->state = AdaptorClosing;
          DspuQueuePut ((TCLSIMsg *) disableReqP);
       }
   }
}


/*============================================================================*
* AdaptorSendActivateSap: *
*============================================================================*
*
* Description:
* Send ACTIVE_SAP.REQ only if adaptor is opened.
*
**/

boolean                                   /* TRUE if sap request sent */
AdaptorSendActivateSap (Adaptor *adaptorP,
                        word uSapId,
                        byte sapNumber)

{
   ActSapReq_t *actSapReqP;


   if (adaptorP->state == AdaptorOpened) {
      actSapReqP = Cnstrct_ActSapReq (uSapId, &adaptorP->id, sapNumber);
      if (actSapReqP) {
         DspuQueuePut ((TCLSIMsg *) actSapReqP);
         return TRUE;
      }
   }
   return FALSE;

} /* AdaptorSendActivateSap. */


/*============================================================================*
* AdaptorReceive: *
*============================================================================*
*
* Description:
* Process incoming adaptor signals from CLSI; ie ENABLE & DISABLE confirms.
*
**/

void
AdaptorReceive (Adaptor *this,
                TCLSIMsg* clsiMsgP)

{

   hword primCmd = CLSIMsgGetPrimitiveCommand (clsiMsgP);
   hword primType = CLSIMsgGetPrimitiveType (clsiMsgP);
   hword retCode = CLSIMsgGetRetCode ((EnableCnf_t*) clsiMsgP);

   switch (primCmd) {

    case CLS_ENABLE:
      if (primType == CLS_CONFIRM) {

         /*
          * SWIDB for RSRB/DLC is created during EnableReq
          *   and returned with EnableCnf
          *
          * Save SWIDB in adaptorId structure for future use
          *   with ActSapReq and DisableReq
          *
          */
         EnableCnf_t *enableCnfP = (EnableCnf_t *) clsiMsgP;
         if ( (enableCnfP->fDLCType == CLS_RSRB) ||
              (enableCnfP->fDLCType == CLS_VDLC) ||
              (enableCnfP->fDLCType == CLS_NCIA) )
         {
             this->id.dlcIdb = enableCnfP->fPortId.fTypeInfo.swidb;
         }
         
         switch (retCode) {

             case CLS_OK:
	         if (this->state == AdaptorOpening) {
	             this->state = AdaptorOpened;
	             DoToAllChildren(&this->sapChildren, SapRequestOpenSap);
	         } else {
	             errmsg(&msgsym(CLSIUnexpctEnableCnf, DSPU),
		              ByteToHexAscii((byte*) clsiMsgP,
                              sizeof(TCLSIMsg)));
                 }
                 break;

             case CLS_INVALID_TARGET_RING:
                 DspuError0(RsrbEnableNoTargetRing);
                 if (this->state == AdaptorOpening) {
                     this->state = AdaptorReset;
                     DoToAllChildren(&this->sapChildren, (ProcChild)SapDelete);
                 }
                 break; 

             default:
                 DspuCLSIErrorMsg(GetDebugLevel(), 
                                    DspuCLSIBadReturnCode, retCode);
                 if (this->state == AdaptorOpening) {
                     this->state = AdaptorReset;
                     DoToAllChildren(&this->sapChildren, (ProcChild)SapDelete);
                 }
                 break;

         } /* End switch (retCode) */

      } else {
          errmsg(&msgsym(CLSIBadPrimitiveType, DSPU),
	   	   ByteToHexAscii((byte*) clsiMsgP,
		   sizeof(TCLSIMsg)));
      }
      break;

    case CLS_DISABLE:
      if (primType == CLS_CONFIRM) {
	 if (retCode != CLS_OK) {
           DspuCLSIErrorMsg(GetDebugLevel(), DspuCLSIBadReturnCode, retCode);
         }
         AdaptorDelete(this);
	 break;
      }
      else
	 errmsg(&msgsym(CLSIBadPrimitiveType, DSPU),
		 ByteToHexAscii((byte*) clsiMsgP,
				 sizeof(TCLSIMsg)));
      break;

    default:
      errmsg(&msgsym(CLSIBadPrimitiveCmd, DSPU),
	      ByteToHexAscii((byte*) clsiMsgP,
			      sizeof(TCLSIMsg)));
      break;
   }

} /* AdaptorReceive. */


/*============================================================================*
* AdaptorPrintDLC() 
*=============================================================================*
*
* Description:
*   Print "dspu enable-[host|pu]" commands for specified adaptor interface
*
**/

void AdaptorPrintDLC (Adaptor* this, hword direction)
{

   switch (this->id.dlcType) {

     case DSPU_DLC_SDLC:
       DoToAllChildrenParm(&this->sapChildren, (ProcChild1)SapPrintSDLC, 
                               direction);
       break;

     case DSPU_DLC_QLLC:
       DoToAllChildrenParm(&this->sapChildren, (ProcChild1)SapPrintQLLC, 
                               direction);
       break;

     case DSPU_DLC_ETHERNET:
     case DSPU_DLC_TOKEN_RING:
     case DSPU_DLC_FDDI:
     case DSPU_DLC_FRAME_RELAY:
     case DSPU_DLC_CHANNEL:
       DoToAllChildrenParm(&this->sapChildren, (ProcChild1)SapPrintLLC, 
                               direction);
       break;

     default:
       break;
   }
} /* End AdaptorPrintDLC() */

/*===========================================================================*
* AdaptorPrintRsrb ()
*============================================================================*
*
* Description:
*   Print "dspu rsrb enable-[host|pu] lsap <localSap>"
*
**/
void AdaptorPrintRsrb (Adaptor* this)
{

   if (this->id.dlcType == DSPU_DLC_RSRB) {

       /* Write all "dspu rsrb enable-pu" config commands first */
       /*   then write the "dspu rsrb enable-host" commands     */
       /* ----------------------------------------------------- */
       DoToAllChildrenParm(&this->sapChildren, (ProcChild1)SapPrintRsrb,
                              Downstream);

       DoToAllChildrenParm(&this->sapChildren, (ProcChild1)SapPrintRsrb,
                               Upstream);

   }

} /* End AdaptorPrintRsrb() */


/*===========================================================================*
* AdaptorPrintVdlc ()
*============================================================================*
*
* Description:
*   Print "dspu vdlc enable-[host|pu] lsap <localSap>"
*
**/
void AdaptorPrintVdlc (Adaptor* this)
{

   if (this->id.dlcType == DSPU_DLC_VDLC) {

       /* Write all "dspu vdlc enable-pu" config commands first */
       /*   then write the "dspu vdlc enable-host" commands     */
       /* ----------------------------------------------------- */
       DoToAllChildrenParm(&this->sapChildren, (ProcChild1)SapPrintVdlc,
                              Downstream);

       DoToAllChildrenParm(&this->sapChildren, (ProcChild1)SapPrintVdlc,
                               Upstream);

   }

} /* End AdaptorPrintVdlc() */


/*===========================================================================*
* AdaptorPrintNcia ()
*============================================================================*
*
* Description:
*   Print "dspu ncia enable-pu lsap <localSap>"
*
**/
void AdaptorPrintNcia (Adaptor* this)
{

   if (this->id.dlcType == DSPU_DLC_NCIA) {

       DoToAllChildrenParm(&this->sapChildren, (ProcChild1)SapPrintNcia,
                              Downstream);

   }

} /* End AdaptorPrintNcia() */
