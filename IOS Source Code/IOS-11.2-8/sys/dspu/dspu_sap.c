/* $Id: dspu_sap.c,v 3.1.60.4 1996/08/28 12:42:10 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_sap.c,v $
 *------------------------------------------------------------------
 * DSPU SAP Support.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Includes support for all the SAP functions, including handling all
 * the CLSI commands which flow on the SAP.
 *------------------------------------------------------------------
 * $Log: dspu_sap.c,v $
 * Revision 3.1.60.4  1996/08/28  12:42:10  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.60.3  1996/06/21  02:21:52  slogie
 * CSCdi57722:  DPSU dont notify SSCP that secondary LU is available
 * Branch: California_branch
 * Fixes to ensure that we do not lose any buffers in the activation
 * window.  Plus debug support for the activation window.
 *
 * Revision 3.1.60.2  1996/05/27  06:01:40  jjohnson
 * CSCdi58758:  some errmsg invocations do not following coding conventions
 * Also fix a bunch of ByteToHexAscii invocations since they are mostly
 * colocated with errmsg invocations
 * Branch: California_branch
 *
 * Revision 3.1.60.1  1996/05/17  10:48:06  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.48.3  1996/05/03  17:18:23  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.48.2  1996/04/02  05:56:02  ppearce
 * CSCdi28462:  Crash in DSPU when disabling SAPs
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.48.1  1996/03/28  08:20:34  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.1.6.1  1996/01/20  06:08:49  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.1  1995/11/09  11:28:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:03:11  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* The template is used to defined proper CPS type 
 * and timer object type.
 */
#define SAP_TEMPLATE

#include "master.h"
#include "logger.h"
#include "msg_dspu.c"			/* Not a typo, see logger.h */

#include "dspu.h"
#include "dspu_sap.h"
#include "dmalloc.h"
#include "dspu_itimer.h"
#include "dspuqueu.h"
#include "bfrprim.h"
#include "dspu_xid.h"
#include "dspu_adaptor.h"
#include "dspu_ls.h"
#include "dspu_pu.h"
#include "dspu_puutil.h"
#include "dspu_error.h"
#include "dspu_util.h"


#define NoWorkBfrsWaitTime 60*SECONDS  /* Timeout period to wait for work 
                                          buffers to be made available. */

static hword actWindow = 0;
static hword numberOfSapsCreated = 0;
static sapWorkBufferUnderflowCount = 0;

/*---- Private function prototypes. */

static void
SapWorkBufferTimeout (Sap* sapP);

/*---- Sap functions. */

/*=====================================================================*
* SapWorkBufferUnderflow
*======================================================================*
*
* Description:
*   Increment the sap work buffer underflow counter.
*
**/
void
SapWorkBufferUnderflow (void)

{

   sapWorkBufferUnderflowCount++;

}  /* SapWorkBufferUnderflow. */


/*=====================================================================*
* SapSetActWindow
*======================================================================*
*
* Description:
*   Set the actWindow.
*
**/
void SapSetActWindow(hword actWindowValue)
{
   short diff = (actWindowValue - actWindow);
   
   if (diff != 0)
   {
      actWindow = actWindowValue;
      if (diff > 0)
         CreateGlobalItimer((hword) (diff * numberOfSapsCreated));
      else
         DeleteGlobalItimer((hword) ((-diff) * numberOfSapsCreated));
      
   }
}


/*=====================================================================*
* SapGetActWindow
*======================================================================*
*
* Description:
*   Return the actWindow.
*
**/
hword SapGetActWindow(void)
{
  return actWindow;
}


/*=====================================================================*
* SapCreate
*======================================================================*
*
* Description:
* This routine is called in response to the detection of any DSPU config
* command requiring that a sap be configured.
*
**/

Sap*                                    /* Ptr to sap cblk. . */
SapCreate (byte sapNumber,              /* Sap index. . */
           LinkType sapDirection,       /* Sap direction. . */
           Sibling* sapChildLocP,       /* Ptr to SAP child locator. . */
           Adaptor *sapParentP)         /* Ptr to SAP's parent. . */

{

   boolean sapAdded;


   Sap* this = DspuMalloc (sizeof(Sap));
   if (this) {
      this->sapChildLocP = sapChildLocP;
      this->sapDirection = sapDirection;
      this->sapNumber = sapNumber;
      this->sapState = SapClosed;
      this->buffersUsed = 0;
      sapAdded = AddChild (this->sapChildLocP, sapNumber, this,
                           sapParentP, &(this->sapParentP));
      if (sapAdded) {
         InitialiseItimer (&(this->sapItimer), TRUE);  
         DspuQueueInit (&(this->WrkBBlkHeader));
         SapRequestOpenSap (this);  
         CreateGlobalItimer (actWindow);
         numberOfSapsCreated++;
         this->snaOrDspu = GetSnaMode();
      }
      else {
         DspuFree (this);
         this = (Sap*) NULL;
      }
   }

   return this;

} /* SapCreate. */


/*=====================================================================*
 * SapGetOrCreate
 *======================================================================*
 *
 * Description:
 * This routine create a sap under the adaptor if sap does not exist
 *
 **/
Sap*
QuerySapExisted (Adaptor* adaptorP,
                byte sap)
{
   return GetChild(GetSapChildren(adaptorP), sap);
}




/*======================================================================*
* SapDelete: *
*======================================================================*
*
* Description:
* Delete SAP, iff there is no link station activity on this sap.
*
**/

boolean
SapDelete (Sap *sapP)

{

   Adaptor* adaptorP = sapP->sapParentP;    /* save my parent */
   boolean deleted = DelChild (sapP->sapChildLocP, sapP->sapNumber,
			       &(sapP->sapParentP));
   DeleteGlobalItimer(actWindow);
   numberOfSapsCreated--;
   DspuFree (sapP);		/* And return it to heap. */
   AdaptorSapClosed(adaptorP);  /* tell adaptor that sap had died */
   return (deleted);

} /* SapDelete. */



/*=====================================================================*
* SapFind.
*======================================================================*
*
* Description:
*   DSPU system architecture defines SAP a child of adaptor while CLSI
* defines SAP as global sap_id. To allow DSPU architecture, the sap_id is
* defined as adaptorNumber+sapNumber. 
* static hword GetAdaptorNum(word uSapId);
**/

#define GetAdaptorNum(uSapId) (((hword)uSapId) >> 8)

Sap *
SapFind (word uSapId)                   /* sap_id used by CLSI. */

{
   Adaptor *adaptorP = GetChild(GetAdaptorRoot(), GetAdaptorNum(uSapId));

   return adaptorP? 
      GetChild(GetSapChildren(adaptorP), (hword)uSapId & 0xff): 0;          

} /* SapFind. */


/*======================================================================*
* SapRequestOpenSap: *
*======================================================================*
*
* Description:
* Iff sap not opened, Construct ACTIVATE_SAP.REQ.
*
**/

void
SapRequestOpenSap (Sap* this)

{

   word uSapId;
   if (this->sapState == SapClosed) { 
      uSapId = (word)((GetAdaptorIndex(this->sapParentP) << 8) +
                        this->sapNumber);

      if (AdaptorSendActivateSap (this->sapParentP,uSapId, this->sapNumber))
         this->sapState = SapOpening;
   }

}  /* SapRequestOpenSap. */

/*=====================================================================*
* SapRequestCloseSap
*======================================================================*
*
* Description:
*  Delete all link station and request to close the sap
*
**/

void SapRequestCloseSap (Sap* this)

{
   DoToAllChildrenParm2((this->sapDirection == Upstream)? GetUplsRoot():GetDnlsRoot(), 
                          (ProcChild2)LsSapClosed, this);
   
   if (this->sapState == SapOpened) {
      word uSapId =(word)((GetAdaptorIndex(this->sapParentP) << 8) + this->sapNumber); 
      DeactSapReq_t* deactSapP = Cnstrct_DeactSapReq(this->pSapId, uSapId);
      if (deactSapP){
         this->sapState = SapClosing;  
         DspuQueuePut ((TCLSIMsg *)deactSapP);
      }
   }
}

/*=====================================================================*
* SapRequestOpenStation
*======================================================================*
*
* Description:
*   This is an outbound request such that pCepId is not defined yet. 
* There is a corresponding link station RequestOpenStation which is an
* inbound call and the pCepId is defined
*
**/

boolean
SapRequestOpenStation (const Sap* this,
                       ReqOpnStnReq_t *rOSRP)

{

   if (this->sapState == SapOpened) {
      rOSRP->fCLSIHdr.fIdentifier = this->pSapId;
      DspuQueuePut ((TCLSIMsg *) rOSRP);
      return TRUE;
   }
   else{                                /* Sap not activated. */
      CLSIMsgDestroy ((TCLSIMsg *) rOSRP);
      return FALSE;
   }

}  /* SapRequestOpenStation. */



/*============================================================================*
* SapReceive: *
*============================================================================*
*
* Description:
* This is called by ProcessSap, and handles sap level signals.
*
**/

void
SapReceive (Sap*       this,            /* The sap we're interested in. */
            SapInput   sapInput,        /* Sap input to handle. */
            TCLSIMsg*  clsiMsgP)        /* CLSI message buffer. */

{

   hword rCode;			/* Return code from confirm. */
   Sibling* lsRootP;		/* Ptr to LS root (in DSPU. */
   Sibling* puRootP;		/* Ptr to inactive PU root. */
   LinkStn *lsP;		/* New link station ptr. */
   Pu *puP;			/* PU ptr (found or constructed). */
   XidAddrSapT rXidAddrSap;	/* xid, addr, & sap triple        */
   XidFmtE xidFmt;
   CloseStnReq_t *closeStnReqP;
   IdStnInd_t *idStnIndP;
   word pCepId;

   boolean freeBuf = TRUE;


   switch (sapInput) {
      
    case SapI_ActivateSapCnf:
      if ((rCode = CLSIMsgGetRetCode ((ActSapCnf_t*)clsiMsgP)) == CLS_OK) {
	 if (this->sapState == SapOpening) {
	    this->sapState = SapOpened;
	    this->pSapId = ActSapCnf_GetPSapID ((ActSapCnf_t *) clsiMsgP);

	    /*---- Attempt to start all link stations associated with this
	      sap. */

	    DoToAllChildrenParm2 (GetUplsRoot(),
				  (ProcChild2)LsQueryLsOnSapAndStart, this);
	    DoToAllChildrenParm2 (GetDnlsRoot(),
				  (ProcChild2)LsQueryLsOnSapAndStart, this);
	 }
	 else
	    errmsg(&msgsym(CLSIUnexpctActSapCnf, DSPU),
		    ByteToHexAscii((byte*) clsiMsgP,
				    sizeof(TCLSIMsg)));
      }
      else{
         DspuCLSIErrorMsg(GetDebugLevel(), DspuCLSIBadReturnCode, rCode);
         if (this->sapState == SapOpening) {
 	    this->sapState = SapClosed;
            SapDelete(this);
	 }
      }
      break;
      
    case SapI_DeactiveSapCnf:
      if ((rCode = CLSIMsgGetRetCode ((DeactSapCnf_t*)clsiMsgP)) == CLS_OK) {
	 if (this->sapState == SapClosing) {
	    this->sapState = SapClosed;
            SapDelete(this);
	 }
	 else
	    errmsg(&msgsym(CLSIUnexpctDeactSapCnf, DSPU),
		    ByteToHexAscii((byte*) clsiMsgP,
				    sizeof(TCLSIMsg)));
      }
      else {
        DspuCLSIErrorMsg(GetDebugLevel(), DspuCLSIBadReturnCode, rCode);
      }
      break;
      
      /*---- This is the beginning of handling connect in's. If everything
             proceeds normally, we'll pass this signal off to the link
             station for further handling. */

    case SapI_IdStnInd:
      if (this->sapState == SapOpened) {

	 /*---- Determine direction this signal is coming from. */

	 switch (this->sapDirection) {
	  case Upstream:
	    lsRootP = GetUplsRoot();
	    puRootP = GetInactUpuRoot();
	    break;
	  case Downstream:
	    lsRootP = GetDnlsRoot();
	    puRootP = GetInactDpuRoot();
	    break;
	  default:
	    lsRootP = NULL;	/* Here => Programmer error only. */
	    puRootP = NULL;
	    break;
	 }

	 /*---- DO PU LOOKUP. */

	 /*---- Get XID format; & remote MAC address and SAP. */

	 xidFmt = XidGetFormat (clsiMsgP);
	 IdStnInd_GetAddrSap((IdStnInd_t *) clsiMsgP, &rXidAddrSap);
         rXidAddrSap.lsapP = this;
	 rXidAddrSap.searchMode = XMS_AddrSap;
	 rXidAddrSap.u_xid.xid = NoXid;

	 /*---- If PU in active list: something odd happened. Ie we received
	   an ID_STN.IND during connection or connected stages. */

	 if ((puP = LsFindActivePu (lsRootP, &rXidAddrSap)) != 0) {
	    freeBuf = FALSE;
	    lsP = PuGetParentLs(puP);
	    LsFsm (lsP, LsI_IdStnInd, clsiMsgP);
	 }

	 /*---- Otherwise PU is inactive or not configured. */

	 else {

	    /*---- If PU is inactive create LS, attach PU & do connect in. */
	    puP = PuFindInactive (puRootP, &rXidAddrSap);
	    if (puP) {
	       lsP = LsCreate (lsRootP, &rXidAddrSap.addr,
                                  rXidAddrSap.rsap, this);
	       if (lsP) {
		  freeBuf = FALSE;
		  PuChangeToActive (puP, lsP);
		  LsFsm (lsP, LsI_IdStnIndPU, clsiMsgP);
	       }
	       else {		/* Link station creation failure. */
                  uchar  addr[STATIONLEN_MAX];
                  ushort addrLen = snpa_get_length(&rXidAddrSap.addr);
                  snpa_copy_to_buffer(&rXidAddrSap.addr, (uchar *) &addr);
		  errmsg(&msgsym(LSConnInFailedNoMem, DSPU),
			  ByteToHexAscii((byte *) &addr, addrLen));
	       }
	    }

	    /*---- PU not found with MAC & SAP. Try MAC & XID, MAC or XID,
	      in that order. */

	    else {
	       switch (xidFmt) {
		case xidFmt_Null:
		  break;
		case xidFmt_0:
		case xidFmt_3:
		  rXidAddrSap.searchMode = XMS_AddrXid;
		  rXidAddrSap.u_xid.xid = XidGetXid (clsiMsgP);
		  puP = PuFindInactive (puRootP, &rXidAddrSap);
		  if (puP) {
		     lsP = LsCreate (lsRootP, &rXidAddrSap.addr,
				     rXidAddrSap.rsap, this);
		     if (lsP) {
			freeBuf = FALSE;
			PuChangeToActive (puP, lsP);
			LsFsm (lsP, LsI_IdStnIndPU, clsiMsgP);
		     }
		     else {	/* Link station creation failure. */
                        uchar  addr[STATIONLEN_MAX];
                        ushort addrLen = snpa_get_length(&rXidAddrSap.addr);
                        snpa_copy_to_buffer(&rXidAddrSap.addr, (uchar *)&addr);
		        errmsg(&msgsym(LSConnInFailedNoMem, DSPU),
		     	        ByteToHexAscii((byte *) &addr, addrLen));
		     }
		  }
		  break;
		default:	/* Programmer error! */
		  break;
	       }
	    }
	 }

	 /*---- PU LOOKUP FAILED. */

	 if (!puP) {

	    /*---- If a default PU has been cfg'd AND we're downstream;
	      then create a link station, If the XID contained in
	      the ID_STN.IND was null do a connect in (with no PU).
	      Otherise if it was a format 0 or 3 XID then construct
	      a PU from the default and do a connect in. */

	    if (GetDefaultPu() && (this->sapDirection == Downstream)) {
	       lsP = LsCreate (lsRootP, &rXidAddrSap.addr,
			       rXidAddrSap.rsap, this);
	       if (lsP) {
		  freeBuf = FALSE;
		  switch (xidFmt) {
		   case xidFmt_Null:
		     LsFsm (lsP, LsI_IdStnInd, clsiMsgP);
		     break;
		   case xidFmt_0:
		   case xidFmt_3:
		     rXidAddrSap.u_xid.xid = XidGetXid (clsiMsgP);
		     puP = DefaultPu_nw (lsP, &rXidAddrSap.addr,
					 SapGetSapNum(rXidAddrSap.lsapP), 
                                         rXidAddrSap.rsap,
					 rXidAddrSap.u_xid.xid);
		     if (puP)
			LsFsm (lsP, LsI_IdStnIndPU, clsiMsgP);
		     else {
                        uchar  addr[STATIONLEN_MAX];
                        ushort addrLen = snpa_get_length(&rXidAddrSap.addr);
                        snpa_copy_to_buffer(&rXidAddrSap.addr, (uchar *)&addr);
			errmsg(&msgsym(LSConnInFailedNoMem, DSPU),
				ByteToHexAscii((byte *) &addr, addrLen));
			LsDelete (lsP);
		     }
		     break;
		   default:	/* Programmer error. */
		     break;
		  }
	       }
	       else {		/* Link station creation failure. */
                  uchar  addr[STATIONLEN_MAX];
                  ushort addrLen = snpa_get_length(&rXidAddrSap.addr);
                  snpa_copy_to_buffer(&rXidAddrSap.addr, (uchar *)&addr);
		  errmsg(&msgsym(LSConnInFailedNoMem, DSPU),
		 	  ByteToHexAscii((byte *) &addr, addrLen));
	       }
	    }
	    else {

	       /*---- We're not downstream or there was no default pu
		 configured. So, if the XID was null, we'll construct
		 a link station with no attached PU, and attempt a
		 connect in. If however the XID is format 0 or 3,
		 we'll reject the attempt to connect in, & ship a
		 CLOSE_STN.REQ out. */

	       switch (xidFmt) {
		case xidFmt_Null:
		  lsP = LsCreate (lsRootP, &rXidAddrSap.addr,
				  rXidAddrSap.rsap, this);
		  if (lsP) {
		     freeBuf = FALSE;
		     LsFsm (lsP, LsI_IdStnInd, clsiMsgP);
		  }
		  else {
                     uchar  addr[STATIONLEN_MAX];
                     ushort addrLen = snpa_get_length(&rXidAddrSap.addr);
                     snpa_copy_to_buffer(&rXidAddrSap.addr, (uchar *)&addr);
		     errmsg(&msgsym(LSConnInFailedNoMem, DSPU),
			     ByteToHexAscii((byte *) &addr, addrLen));
		  }
		  break;
		case xidFmt_0:
		case xidFmt_3:
		  idStnIndP = (IdStnInd_t *) clsiMsgP;
		  pCepId = GetPCepID (idStnIndP);
                  if (pCepId != CLS_NO_CEP) {                  
    		      closeStnReqP = Cnstrct_CloseStnReq (pCepId, 0L);
		      DspuQueuePut ((TCLSIMsg *) closeStnReqP);
                  }                                           
		  break;
		default:	/* Programmer error. */
		  break;
	       }
	    }
	 }

      }
      else			/* State of sap incorrect. */
	 errmsg(&msgsym(CLSIUnexpctIdStnInd, DSPU),
		 ByteToHexAscii((byte*) clsiMsgP,
				 sizeof(TCLSIMsg)));
      break;

    default:			/* We don't handle any other signals. */
      errmsg(&msgsym(CLSIBadPrimitiveCmd, DSPU),
	      ByteToHexAscii((byte*) clsiMsgP, sizeof(TCLSIMsg)));
      break;

   } /* switch (sapInput). */

   if (freeBuf)
      CLSIMsgDestroy (clsiMsgP);

} /* SapReceive. */


/*=====================================================================*
* SapWorkBufferTimeout
*======================================================================*
*
* Description:
*   When no buffers are available a timer is started and this is the
* routine that gets called when the timer times out. It calls
* SapFreeGrantedWorkBuffer but it frees no buffers (no buffers were used).
* It scans through the work buffer block queue allocating buffers (if 
* available).
* 
**/
static void                              
SapWorkBufferTimeout(Sap*  sapP)     
{
  SapFreeGrantedWorkBuffer(sapP, 0);  /* Timeout routine */  
}  

/*=====================================================================*
* SapQueryGrantWorkBuffer
*======================================================================*
*
* Description:
*   A buffer is granted based on the following algorithm :
*          if (LsWorkBufferCount < (actWindow - buffersUsed))
*   Each Link Station is allowed to have up to half of the buffers left,
* out of the original SAP number of buffers configured in the "actWindow" 
* parameter.
* 
**/
boolean                              
SapQueryGrantWorkBuffer(Sap*  sapP,            
                        hword lsWorkBufferCount)     
{
  return (lsWorkBufferCount < (actWindow - sapP->buffersUsed));  
}  

/*=====================================================================*
* SapRequestWork
*======================================================================*
*
* Description:
*   This is called by the link station to request to do work. If it 
* passes the test for being granted a work buffer, then it is allowed
* to do work with the given buffer. If the work has been done, then
* it is automatically registered and enqueued for more more work.
* If it was not eligible for a buffer in the first place, then it gets
* registered and enqueued for work at a later stage.
* 
**/
void                              
SapRequestWork(Sap*     sapP
              ,WrkBBlk* wrkBBlkP                 
              )     
{
  WorkResult retFromDoWork;

  if (SapQueryGrantWorkBuffer (sapP, LsQueryBufferCount(wrkBBlkP->lsP)))
  {
    if ((retFromDoWork=LsDoWork (wrkBBlkP->lsP)) != WORK_NOT_DONE)
    { 
      if (!wrkBBlkP->bufReg)
      {
        /* Automatic registration 
         */
        wrkBBlkP->bufReg = TRUE;  
        DspuEnqueue (&sapP->WrkBBlkHeader, wrkBBlkP);    
      }
      if (retFromDoWork == WORK_DONE)
        sapP->buffersUsed++; 
      else 
      {
        /* LsDoWork return code must have been NO_WORK_BUFFER_AVAILABLE.
         * So start the timer and we'll try
         * again when the timer times out.
         */     
        StartItimer (&(sapP->sapItimer), NoWorkBfrsWaitTime, SapWorkBufferTimeout, sapP);
      }
    }
    /* else
     * LsDoWork return code was WORK_NOT_DONE.
     * This should never occur!  But if it does we simply
     * ignore the request to do work, and do not register
     * the station.
     */
  }
  else
  {
    /* Did not qualify to receive a buffer, so check if registered.
     * If not registered then, register it and enqueue it.         
     */
    if (!wrkBBlkP->bufReg)        
    {
      /* Automatic registration 
       */
      wrkBBlkP->bufReg = TRUE;    
      DspuEnqueue(&sapP->WrkBBlkHeader, wrkBBlkP);
    }
  }  
}  

/*=====================================================================*
 * SapFreeGrantedWorkBuffer
 *======================================================================*
 *
 * Description:
 *   This decrements the buffersUsed by the number passed. It then scans  
 * all the Link Stations registered for work, looking for ones that can be 
 * granted buffers to do work; this is done as follows:
 *
 *   The first Link Station is dequeued from the work list, and 
 * SapQueryGrantWorkBuffer is called; if it fails the test for being granted a 
 * buffer, it is requeued on the end of the list. If it passes the test for 
 * being granted a buffer then LsDoWork is called; if this returns 
 * WORK_DONE then the Link Station is requeued at the bottom of the list,
 * the last queue element is updated and the BufferLeft is decremented.  
 * If WORK_NOT_DONE is returned then the Link Station is automatically 
 * deregistered by not requeuing it on the registered list and resetting
 * the "registered" flag.
 * If NO_WORK_BUFFER_AVAILABLE is returned the link station is requeued
 * and a timer is started to try again later.
 * 
 *   This algorithm continues with all the Link Stations until either the 
 * buffersUsed is less than the actWindow, or all Link Stations have been 
 * scanned and none have done the work they were eligible for. 
 *
 **/
void                                 
SapFreeGrantedWorkBuffer(Sap*  sapP,        
                         hword numBuffersToFree)    
{
   WrkBBlk*   wrkBBlkP;
   WrkBBlk*   lastWrkBBlkP;
   WorkResult retFromDoWork;

   if (sapP->buffersUsed < numBuffersToFree) {
      SapWorkBufferUnderflow();
      sapP->buffersUsed = 0;
    }
   else
      sapP->buffersUsed -= numBuffersToFree;  

   /* Ensure that the timer does not get started a second time 
    */
   StopItimer (&(sapP->sapItimer));  

   /* Save the last queue element for checking if all Link Stations   
    * have been scanned.                                              
    */
   lastWrkBBlkP = DspuGetLastQelement(&sapP->WrkBBlkHeader);
  
   while (sapP->buffersUsed <= actWindow)  {
      wrkBBlkP = DspuDequeue(&sapP->WrkBBlkHeader);
    
      if (wrkBBlkP == 0)
	 break;			/* no work need to be done */
        
      retFromDoWork = WORK_NOT_DONE;
      if (SapQueryGrantWorkBuffer (sapP, LsQueryBufferCount(wrkBBlkP->lsP))) {
	 if ((retFromDoWork = LsDoWork(wrkBBlkP->lsP)) == WORK_DONE) {
	    /* The link station has done the work it was granted, so it  
	     * is requeued so that it can again be given the opportunity 
	     * to do more work when it is at the head of the queue.      
	     * When it is requeued, the last queue element is updated,   
	     * forcing us to scan the queue until it is reached.         
	     *                                                                    
	     * (We are basically keeping a ptr to the last queue element 
	     *  which has WORK DONE.)                                    
	     */

	    lastWrkBBlkP = wrkBBlkP; /* Update the last queue element */
	    DspuEnqueue(&sapP->WrkBBlkHeader, wrkBBlkP);
	    sapP->buffersUsed++;    
	 }
	 else if (retFromDoWork == NO_WORK_BUFFERS_AVAILABLE) {
	    /* No buffers were available, so enqueue the work block
	     * to give the work block another chance to do work when
	     * it's at the head of the list. Also start the timer so that
	     * we can try again when the timer times out.
	     */  
	    DspuEnqueue(&sapP->WrkBBlkHeader, wrkBBlkP);
	    StartItimer (&(sapP->sapItimer), NoWorkBfrsWaitTime, 
			 SapWorkBufferTimeout, sapP);
	    break;             
	 }
	 else
	    /* It failed to do work, so automatically deregister it. 
	     */ 
	    wrkBBlkP->bufReg = FALSE;
      }
      else
	 /* It is not eligible for a work buffer now, so it is enqueued   
	  * and tested again later when it comes to the head of the queue. 
	  */
	 DspuEnqueue(&sapP->WrkBBlkHeader, wrkBBlkP);
       
      /* Check if all link stations have been scanned and none have 
       * returned WORK_DONE 
       */
      if ((wrkBBlkP == lastWrkBBlkP) && (retFromDoWork != WORK_DONE))
	 break;
   }  

} /* SapFreeGrantedWorkBuffer. */


     

/*=====================================================================*
 * SapDeregisterForWork
 *======================================================================*
 *
 * Description:
 *   This is included to be used by a Link Station which is being destructed
 * while its LsWorkBufferCount is non-zero (which should never happen!).
 * (In the normal course of events, Link Stations are automatically 
 * deregistered.) It scans down all the registered Link Stations looking
 * for a pointer match; when found it removes the Link Station from the 
 * list, and resets the "registered" flag.
 *
 **/
void                                  
SapDeregisterForWork(Sap     *sapP,           
                     LinkStn *lsP)                 
{
   WrkBBlk *wrkBBlkP;
  
   wrkBBlkP = DspuGetFirstQelement(&sapP->WrkBBlkHeader); 
   while (wrkBBlkP) {
      if (wrkBBlkP->lsP == lsP) {
	 wrkBBlkP->bufReg = FALSE;
	 DspuDeleteQueueElement(&sapP->WrkBBlkHeader, wrkBBlkP);
         break;
      }
      else
         wrkBBlkP = wrkBBlkP->next;
   } 
}  

/*============================================================================*
 * SapQueryBuffersUsed:
 *============================================================================*
 *
 * Description:
 * Query buffers used by this SAP.
 *
 **/

hword
SapQueryBuffersUsed (Sap *sapP)

{

   return (sapP->buffersUsed);

 } /* SapQueryBuffersUsed. */


void SapPrintLLC(Sap* this, hword direction)
{

   /*
    *
    * Don't write SAP if SNA command (GetSnaMode() == TRUE), 
    *   but SAP is not SNA SAP (IsSapSna() == FALSE)
    *
    * Don't write SAP if DSPU command (GetSnaMode() == FALSE),
    *   but SAP is not DSPU SAP (IsSapSna() == TRUE)
    *
    */
   if ( (GetSnaMode() && !IsSapSna(this)) || (!GetSnaMode() && IsSapSna(this)) )
     return;

   nv_write(this->sapDirection == direction, "%s enable-%s lsap %d",
      (IsSapSna(this) ? "sna" : "dspu"),
      (direction == Downstream) ? "pu": "host", this->sapNumber);

} /* End SapPrintLLC() */

void SapPrintSDLC(Sap* this, hword direction)
{

   /*
    *
    * Don't write SAP if SNA command (GetSnaMode() == TRUE), 
    *   but SAP is not SNA SAP (IsSapSna() == FALSE)
    *
    * Don't write SAP if DSPU command (GetSnaMode() == FALSE),
    *   but SAP is not DSPU SAP (IsSapSna() == TRUE)
    *
    */
   if ( (GetSnaMode() && !IsSapSna(this)) || (!GetSnaMode() && IsSapSna(this)) )
     return;

   nv_write(this->sapDirection == direction, "%s enable-%s sdlc %s",
      (IsSapSna(this) ? "sna" : "dspu"),
      (direction == Downstream) ? "pu": "host",
      ByteToHexAscii(&this->sapNumber, 1));

} /* End SapPrintSDLC() */

void SapPrintQLLC(Sap* this, hword direction)
{

   char asciiQllcAddr[3];
   /*
    *
    * Don't write SAP if SNA command (GetSnaMode() == TRUE), 
    *   but SAP is not SNA SAP (IsSapSna() == FALSE)
    *
    * Don't write SAP if DSPU command (GetSnaMode() == FALSE),
    *   but SAP is not DSPU SAP (IsSapSna() == TRUE)
    *
    */
   if ( (GetSnaMode() && !IsSapSna(this)) || (!GetSnaMode() && IsSapSna(this)) )
     return;

   QllcSubAddrToAscii(this->sapNumber, asciiQllcAddr);
   nv_write(this->sapDirection == direction, "%s enable-%s qllc %s",
      (IsSapSna(this) ? "sna" : "dspu"),
      (direction == Downstream) ? "pu": "host", asciiQllcAddr);

} /* End SapPrintQLLC() */

void SapPrintRsrb(Sap* this, hword direction)
{

   /*
    *
    * Don't write SAP if SNA command (GetSnaMode() == TRUE), 
    *   but SAP is not SNA SAP (IsSapSna() == FALSE)
    *
    * Don't write SAP if DSPU command (GetSnaMode() == FALSE),
    *   but SAP is not DSPU SAP (IsSapSna() == TRUE)
    *
    */
   if ( (GetSnaMode() && !IsSapSna(this)) || (!GetSnaMode() && IsSapSna(this)) )
     return;

   nv_write(this->sapDirection == direction, "%s rsrb enable-%s lsap %d",
      (IsSapSna(this) ? "sna" : "dspu"),
      (direction == Downstream) ? "pu": "host", this->sapNumber);

} /*End SapPrintRsrb() */

void SapPrintVdlc(Sap* this, hword direction)
{

   /*
    *
    * Don't write SAP if SNA command (GetSnaMode() == TRUE), 
    *   but SAP is not SNA SAP (IsSapSna() == FALSE)
    *
    * Don't write SAP if DSPU command (GetSnaMode() == FALSE),
    *   but SAP is not DSPU SAP (IsSapSna() == TRUE)
    *
    */
   if ( (GetSnaMode() && !IsSapSna(this)) || (!GetSnaMode() && IsSapSna(this)) )
     return;

   nv_write(this->sapDirection == direction, "%s vdlc enable-%s lsap %d",
      (IsSapSna(this) ? "sna" : "dspu"),
      (direction == Downstream) ? "pu": "host", this->sapNumber);

} /*End SapPrintVdlc() */

void SapPrintNcia(Sap* this, hword direction)
{

   /*
    *
    * Don't write SAP if SNA command (GetSnaMode() == TRUE), 
    *   but SAP is not SNA SAP (IsSapSna() == FALSE)
    *
    * Don't write SAP if DSPU command (GetSnaMode() == FALSE),
    *   but SAP is not DSPU SAP (IsSapSna() == TRUE)
    *
    */
   if ( (GetSnaMode() && !IsSapSna(this)) || (!GetSnaMode() && IsSapSna(this)) )
     return;

   nv_write(this->sapDirection == direction, "%s ncia enable-%s lsap %d",
      (IsSapSna(this) ? "sna" : "dspu"),
      (direction == Downstream) ? "pu": "host", this->sapNumber);

} /*End SapPrintNcia() */
