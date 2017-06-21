/* $Id: dspu_ls.c,v 3.4.10.7 1996/08/28 12:42:00 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_ls.c,v $
 *------------------------------------------------------------------
 * DSPU Link Station Function.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This module contains all code/routines to support link stations: ie
 * constructors/destructors, etc., except finite state machine - see lsfsm.c.
 *------------------------------------------------------------------
 * $Log: dspu_ls.c,v $
 * Revision 3.4.10.7  1996/08/28  12:42:00  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.10.6  1996/08/28  03:58:20  bchan
 * CSCdi65090:  NSP stops sending xid when switched major nodes inactivated
 * Branch: California_branch
 *
 * Revision 3.4.10.5  1996/07/29  21:46:56  snyder
 * CSCdi64412:  declare some arrays const that should be sub_sna_pu
 *              moves 2376 out of data, saves 116 as well
 * Branch: California_branch
 *
 * Revision 3.4.10.4  1996/06/21  02:21:56  slogie
 * CSCdi57722:  DPSU dont notify SSCP that secondary LU is available
 * Branch: California_branch
 * Fixes to ensure that we do not lose any buffers in the activation
 * window.  Plus debug support for the activation window.
 *
 * Revision 3.4.10.3  1996/05/27  06:01:33  jjohnson
 * CSCdi58758:  some errmsg invocations do not following coding conventions
 * Also fix a bunch of ByteToHexAscii invocations since they are mostly
 * colocated with errmsg invocations
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/05/17  10:47:40  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.2  1996/04/03  14:02:40  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.2.1  1996/03/28  08:20:25  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.4.10.1  1996/03/18  19:33:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.18.3  1996/03/16  06:40:31  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.18.2  1996/03/07  08:45:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.18.1  1996/02/20  13:47:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/26  17:33:02  ppearce
 * CSCdi49863:  DSPU reports info from CV 0x22 incorrectly
 *
 * Revision 3.3  1996/02/22  09:08:07  ppearce
 * CSCdi47803:  DSPU retries the connection sequence to the host too fast
 *   Don't output LSXIDNegotiationError message unless debug enabled
 *
 * Revision 3.2.6.1  1996/01/20  06:08:40  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2  1995/11/30  19:08:57  ppearce
 * CSCdi44719:  sna rsrb start command does not appear in config
 *   Separate nv_write of "sna rsrb start" and "dspu rsrb start"
 *
 * Revision 3.1  1995/11/09  11:28:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/05  22:45:44  ppearce
 * CSCdi41646:  Unsolicited message from DSPU: RNR sent to host
 *
 * Revision 2.1  1995/09/20  17:02:57  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* The template is used to defined proper CPS type 
 * and timer object type.
 */
#define LS_TEMPLATE


#include "master.h"
#include "logger.h"
#include "msg_dspu.c"			/* Not a typo, see logger.h */
#include <string.h>

#include "dspu.h"
#include "dspu_ls.h"
#include "dspu_interr.h"
#include "dmalloc.h"
#include "bfrprim.h"
#include "snaprim.h"
#include "dspu_xid.h"
#include "dspu_itimer.h"
#include "wrkbblk.h"
#include "dspu_fsmstr.h"
#include "dspu_adaptor.h"
#include "dspu_pu.h"
#include "dspu_sap.h"
#include "dspu_puutil.h"
#include "dspu_error.h"
#include "dspu_util.h"
#include "dspu_xid.h"

static char* const lsEvent[];
extern char* linkStationStatus[];

/*----------------------------------------------------------------------------*/
/* Defines: Constants and inline functions. */
/*----------------------------------------------------------------------------*/


/* The magic number is used to checked data received is OK and also to
 * ensure that ucepid 0 is not passed
 */

#define LsMagicNumber           0x5a5a5000L
#define LsMagicMask             0xfffffe00L
#define DownStreamBit           0x100L

#define ID_MASK                 0xff

#define LsSetUCepID(lsDir,lsIdx) \
    (((lsDir == Downstream)? DownStreamBit: 0) + lsIdx + LsMagicNumber)

#define XidItime 30*SECONDS             /* Timeout period for XID reponse. */
#define CloseStnITime 30*SECONDS        /* Timeout period for CLOSE_STN.REQ. */

#define MAX_NULLXID_RETRIES 2           /* Max null XID's sent downstream. */


/*---- Private data structures; used only in this source module. */

typedef enum LsDownStreamWork {
        DnStrWk_SendNullXid,
        DnStrWk_SendConnect
        } LsDownStreamWork;


/*---- Private function prototypes; used only in this source module. */

static void
LsSendXidOrConnect (LsDownStreamWork lsDownStreamWork,
                    LinkStn *lsP,
                    TCLSIMsg *clsiMsgP,
                    boolean   xid3Negotiation);

static void 
LsItimer (LinkStn *lsP);

static void
LsXidNegErrMsg (NegErrE negErr,
                TCLSIMsg* clsiMsgP,
                hword maxIFrame,
                byte windowSize);

/*---- External references. */

extern FsmStruct LsFsmTable[LsS_MaxState][LsI_MaxInput];



/*----------------------------------------------------------------------------*
 * LsCreate:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Construct and initialize a link station control block.
 *
 **/

LinkStn *                               /* Addr of constructed LS. (Output). */
LsCreate (Sibling *lsChildLocP,         /* Ptr to child locator. (Input). */
          hwaddrtype *hwLsAddrP,        /* LS addressing information      */
          byte rSap,                    /* Remote sap. (Input). */
          Sap *sapP)                    /* Ptr to SAP ctl blk. (Input). */

{

   boolean inited;
   LinkStn *lsP;
   Parent_t lsParentP = GetDspuRoot();
          

   inited = FALSE;
   lsP = DspuMalloc (sizeof(LinkStn));
   if (lsP) {
      lsP->lsChildLocP = lsChildLocP;
      lsP->lsReqState = LsRS_Closed;
      lsP->fsmState = LsS_Reset;
      lsP->sapP = sapP;
      lsP->pCepId = 0;
  
      snpa_copy(hwLsAddrP, LsGetAddrP(lsP));
      lsP->rSap = rSap;
  
      /*---- Init PU parent (child locator) in this LS. */
      inited = InitChild (SINGLE, (Sibling*)&(lsP->puChildLoc));
      if (inited) {
    
	 /*---- Thread this LS (child) onto appropriate (up/down LS) root. */
	 lsP->lsIndex = AddChildFirstFree (lsChildLocP, lsP,
					   lsParentP, &(lsP->lsParentP));
	 inited = (lsP->lsIndex != BAD_SIBLING_INDEX);
	 if (inited) {
	    lsP->debugLevel = GetDebugLevel(); /* set to global debug */
	    lsP->lastErr = NoError;
      
	    InitialiseItimer (&(lsP->lsItimer), FALSE);
      
	    lsP->lastFlowRcvd = FlwCtl_None;
	    lsP->stopFlowCount = 0;
      
	    lsP->workType = 0x0;
	    lsP->workBufCnt = 0;
	    lsP->numNullXidSent = 0;
	    lsP->xidSent = FALSE;
	    lsP->rosrSent = FALSE;
	    InitWrkBBlk (&(lsP->workBBlk), lsP);
	 }
      }
      if (!inited) {
	 DspuFree (lsP);
	 lsP = (LinkStn *) NULL;
      }
   }

   return (lsP);

} /* LsCreate. */


/*----------------------------------------------------------------------------*
 * LsDelete:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Delete a link station control block.
 *
 **/

boolean                                 /* Result of LS deletion. (Input). */
LsDelete (LinkStn *lsP)                 /* LS root (up/down). (Input). */

{

   boolean deleted;
   Pu* puP;

   puP = GetSingleChild (&(lsP->puChildLoc));
   if (lsP->workBufCnt > 0) {     /* make sure all buffers are acknowledge */
      dspuActTraceType = TraceTypeFree;
      DspuActivationWindowTrace(lsP->debugLevel, LsActWindowTrace,
                   puP, 0, (SapQueryBuffersUsed(lsP->sapP) - lsP->workBufCnt));
      SapFreeGrantedWorkBuffer (lsP->sapP, lsP->workBufCnt); 
    }

   StopItimer (&(lsP->lsItimer)); /* Halt any outstanding ls timer. */
   SapDeregisterForWork(lsP->sapP, lsP);
   deleted = DelChild (lsP->lsChildLocP, lsP->lsIndex, &(lsP->lsParentP));
   if (puP  != 0)
        ParentLinkStationClosed(puP);
   if (deleted) {
      memset (lsP, 0, sizeof(LinkStn));
      DspuFree (lsP);
   }

   return (deleted);

} /* LsDelete. */


/*----------------------------------------------------------------------------*
 * LsFind:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Given a link station uCepId, find the address of the link station
 * control block associated with this uCepId.
 *
 **/

LinkStn *
LsFind (word lsId)                      /* LS identifier.  */

{

   if ((lsId & LsMagicMask) != LsMagicNumber)
      return 0;
   
   return (LinkStn *)GetChild((lsId & DownStreamBit)? 
               GetDnlsRoot (): GetUplsRoot (),  ((hword)lsId) & ID_MASK);

}  /* LsFind. */


/*----------------------------------------------------------------------------*
 * LsReceive:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * This procedure is called direct by DSPUReceive to process all link
 * station signals/commands. For all commands except flow control it sets
 * up the appropriate input for the link station finite-state-machine (fsm),
 * and then calls the fsm to process the command. All flow control is
 * intercepted and handled here.
 *
 **/

void
LsReceive (TCLSIMsg *clsiMsgP)          /* Ptr to CLSI buffer. (Input). */

{

   LinkStn *lsP;
   LsInput lsInput = LsI_ResetLs;
   boolean confirm, indication;
   boolean processThruFsm;
   hword command;
   FlowControlE flowCtrl;
   XidNegErrT xidNegErr;
   word identifier;
   boolean error;
   byte* xidP;
   hword xidLn;
   word retCode = CLS_OK;


   
   error = FALSE;
   processThruFsm = TRUE;

   /*---- Find link station. */

   identifier = CLSIMsgGetIdentifier (clsiMsgP);
   confirm = CLSIMsgIsConfirm (clsiMsgP);
   indication = CLSIMsgIsIndication (clsiMsgP);
   command = CLSIMsgGetPrimitiveCommand (clsiMsgP);

   lsP = LsFind (identifier);
   if (lsP == 0) {

       /* "lsP" will not be found if ReqOpnStn.Req never completed       */
       /*    but clsiMsg might be CloseStn.Cfm (which is "not" an error) */
       /* -------------------------------------------------------------- */
       if ( (command == CLS_CLOSE_STN) && (confirm) ) {
           processThruFsm = FALSE;
       } else {
           errmsg(&msgsym(CLSIBadIdentifier, DSPU),
	          ByteToHexAscii((byte*) clsiMsgP, sizeof(TCLSIMsg)));
           error = TRUE;
       }
   }
   else {

      /*---- Have link station. Setup signal/command for LS FSM to handle.
	Note: For confirms, we check the command completion code.
	If non-zero, we'll reset the link-station, PU, and LU's. */

      switch (command) {

       case  CLS_CLOSE_STN:
         if (confirm)
            lsInput = ((retCode = CLSIMsgGetRetCode ((CloseStnCnf_t*)clsiMsgP)) == CLS_OK) ?
	       LsI_CloseCnf : LsI_ResetLs;
         else {
            errmsg(&msgsym(CLSIBadPrimitiveType, DSPU),
                    ByteToHexAscii((byte*) clsiMsgP,
                                    sizeof(TCLSIMsg)));
            error = TRUE;
	 }
         break;
        
       case  CLS_CONNECT:
         if (confirm)
            lsInput = ((retCode = CLSIMsgGetRetCode ((ConnectCnf_t*)clsiMsgP)) ==CLS_OK) ?
	       LsI_ConnectCnf : LsI_ResetLs;
         else
            lsInput = LsI_ConnectInd;
         break;
           
       case  CLS_CONNECTED:
         if (indication)
            lsInput = LsI_ConnectedInd;
         else {
            errmsg(&msgsym(CLSIBadPrimitiveType, DSPU),
                    ByteToHexAscii((byte*) clsiMsgP,
                                    sizeof(TCLSIMsg)));
            error = TRUE;
	 }
         break;

       case  CLS_DATA:
         lsInput = confirm ? LsI_ResetLs : LsI_DataInd;
         break;

       case  CLS_DISCONNECT:
         if (confirm)
            /* disconnect confirm does not have a return code */
            lsInput = LsI_DiscCnf;
         else
            if (indication)
               lsInput = LsI_DiscInd;
         break;
        
       case  CLS_FLOW:		/* Handle FLOW.IND here, not in FSM. */
         if (indication) {
            flowCtrl = FlowInd_GetFlowAction ((FlowInd_t *) clsiMsgP);
            LsFlowControlFromClsi (lsP, flowCtrl);
            processThruFsm = FALSE;
	 }
         else {
            errmsg(&msgsym(CLSIBadPrimitiveType, DSPU),
                    ByteToHexAscii((byte*) clsiMsgP,
                                    sizeof(TCLSIMsg)));
            error = TRUE;
	 }
         break;
        
       case  CLS_ID:
         if ((confirm) &&
             ((retCode = CLSIMsgGetRetCode ((IdCnf_t *) clsiMsgP)) != CLS_OK))
            lsInput = LsI_ResetLs;
         else {
            switch (XidGetFormat (clsiMsgP)) {

	     case xidFmt_Null:
	       lsInput = LsI_IdIndCnfNull;
	       break;

	     case xidFmt_0:
	       lsInput = LsI_IdIndCnf0;
	       break;

	     case xidFmt_3:

	       /*---- First check for XID negotiation errors. */
	       if (XidQueryNegError (clsiMsgP, &xidNegErr)) {
                  if (lsP->debugLevel) {
		      errmsg(&msgsym(LSXIDNegotiationError, DSPU),
			      xidNegErr.byteOffset, 
                              xidNegErr.bitOffset,
			      xidNegErr.senseData);
                  } 
		  lsInput = LsI_Xid3Err;
	       }

	       /*---- Next check for ACTPU capability. */
	       else 
		  lsInput = LsI_IdIndCnf3;
	       break;
   
	     default:
	       XidGetXidData (clsiMsgP, &xidP, &xidLn);
	       errmsg(&msgsym(UnknownXid, DSPU), ByteToHexAscii(xidP, 4));
	       error = TRUE;
	       break;
	    }
	 }
         break;
           
       case  CLS_OPEN_STN:
         errmsg(&msgsym(CLSIBadPrimitiveCmd, DSPU),
                  ByteToHexAscii((byte*) clsiMsgP, sizeof(TCLSIMsg)));
         error = TRUE;
         break;

       case  CLS_REQ_OPNSTN:
         if (confirm) {
            if ((retCode = CLSIMsgGetRetCode ((ReqOpnStnCnf_t*)clsiMsgP)) != CLS_OK)
               lsInput = LsI_ResetLs;
            else
               if (lsP->pCepId == 0 ||
                   lsP->pCepId == GetPCepID((ReqOpnStnCnf_t *) clsiMsgP))
                  lsInput = LsI_ReqOpnCnf;
               else
                  lsInput = LsI_ReqOpnCnfX;
	 }
         else {
            errmsg(&msgsym(CLSIBadPrimitiveType, DSPU),
                    ByteToHexAscii((byte*) clsiMsgP,
                                    sizeof(TCLSIMsg)));
            error = TRUE;
	 }
         break;

       case  CLS_SIGNAL_STN:
         if (confirm)
            /* signal station does not have a return code */
            lsInput =  LsI_SigStnCnf;
         else {
            errmsg(&msgsym(CLSIBadPrimitiveType, DSPU),
                    ByteToHexAscii((byte*) clsiMsgP,
                                    sizeof(TCLSIMsg)));
            error = TRUE;
	 }
         break;
           
       default:
         errmsg(&msgsym(CLSIBadPrimitiveCmd, DSPU),
                 ByteToHexAscii((byte*) clsiMsgP, sizeof(TCLSIMsg)));
         error = TRUE;
      }
   }

   /*---- Now process incoming signal through LS FSM. */
   if (retCode != CLS_OK) {
       if (lsP) {
         DspuCLSIErrorMsg(lsP->debugLevel, DspuCLSIBadReturnCode, retCode);
       } else {
         DspuCLSIErrorMsg(GetDebugLevel(), DspuCLSIBadReturnCode, retCode);
       }
   }

   if (!error && processThruFsm) /* Detected signal to process? */
      LsFsm (lsP, lsInput, clsiMsgP);
   else
      CLSIMsgDestroy (clsiMsgP); /* Either error or handleed already. */

} /* LsReceive. */


/*----------------------------------------------------------------------------*
 * LsSendData:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Send DATA.REQ packet out across CLSI. (Contains user data.)
 *
 **/

void
LsSendData (LinkStn *lsP,
            DataReq_t *clsiMsgP)

{


   LsFsm (lsP, LsI_DataReq, (TCLSIMsg *) clsiMsgP); /* Simple, no? */

} /* LsSendData. */


/*----------------------------------------------------------------------*
 * LsSapClosed:
 *----------------------------------------------------------------------*
 *
 * Description:
 * Remove all link stations as the sap is to be closed.
 *
 **/

void
LsSapClosed (LinkStn *lsP, Sap* sapP)

{
   if (lsP->sapP == sapP){

       LsDelete(lsP);
   }
}


/*----------------------------------------------------------------------*
 * LsQueryLsOnSapAndStart:
 *----------------------------------------------------------------------*
 *
 * Description:
 * Check that this ls is associated with the sap. If it is, then process
 * a start command on this ls. [A REQ_OPNSTN.REQ will be constructed and
 * sent (again).]
 *
 **/

void
LsQueryLsOnSapAndStart (LinkStn *lsP,
                        Sap *sapP)

{


   if (lsP->sapP == sapP) {
      LsFsm (lsP, LsI_StartLs, 0);
   }

} /* LsQueryLsOnSapAndStart. */


/*----------------------------------------------------------------------------*
 * LsLogForWork:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Remember the type of work to be done on this link station. Now call up
 * the sap (on which this link station is attached;) checking to see if
 * there are enough free buffers to get the 'type of work' done. You need
 * to the sap code for a better understanding.
 *
 **/

void
LsLogForWork (LinkStn *lsP,
              LsWorkType workType)

{

   lsP->workType |= workType;	/* Turn on work type bit. */
   SapRequestWork (lsP->sapP, &(lsP->workBBlk));

} /* LsLogForWork. */


/*----------------------------------------------------------------------------*
 * LsDoWork:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Do pu or link station work. This routine is called by the sap to process
 * work requests put on the work list by LsLogForWork.
 *
 **/

WorkResult
LsDoWork (LinkStn *lsP)

{

   Pu *puP;
   WorkResult workDone;
   IdRsp_t *idRspP;
   IdReq_t *idReqP;
   ReqOpnStnReq_t *rOSRP;
   word uCepId;


   workDone = WORK_NOT_DONE;	/* Assume couch potato position. */
   puP = GetSingleChild (&(lsP->puChildLoc));

   /*---- Is there work for the PU to do? */
   if (lsP->workType & WORK_PU) {
      workDone = DoPuWork (puP); /* Call in the PU workman. */
      switch (workDone) {
       case WORK_DONE:		/* PU working out. */
         lsP->workBufCnt++;
         break;
       case WORK_NOT_DONE:	/* PU not working out. */
         lsP->workType ^= WORK_PU;
         break;
       default:
         /*---- No work buffers available */
         break;
      }
   }

   /*---- Are we still in lazy mode? */
   if (workDone == WORK_NOT_DONE) {

      /*---- Yes. But maybe not for long... */
      if (lsP->workType & (WORK_XID3_REQ | WORK_XID3_REQ2 ))
	 { 
	    /*---- Handle transmission of format 3 XID, via ID.REQ. */
	    if ((idReqP=PuSendXidReq (puP, (lsP->workType & WORK_XID3_REQ2 ) != 0)) == 0)
	       workDone = NO_WORK_BUFFERS_AVAILABLE;
	    else
	       {   
		  SetPCepID (idReqP, lsP->pCepId);
		  lsP->xidSent = TRUE;
		  lsP->workBufCnt++;
		  DspuQueuePut ((TCLSIMsg *) idReqP);
		  lsP->workType &= ~(WORK_XID3_REQ | WORK_XID3_REQ2) ;
		  workDone = WORK_DONE;
                  dspuActTraceType = TraceTypeXid3Req;
	       }  
	 }
      else if (lsP->workType & WORK_XID3_RSP)
	 { 
	    /*---- Handle transmission of format 3 XID, via ID.RSP. */
	    if ((idRspP=PuSendXidRsp (puP)) == 0)
	       workDone = NO_WORK_BUFFERS_AVAILABLE;
	    else
	       {   
		  SetPCepID (idRspP, lsP->pCepId);
		  lsP->xidSent = TRUE;
		  lsP->workBufCnt++;
		  DspuQueuePut ((TCLSIMsg *) idRspP);
		  lsP->workType ^= WORK_XID3_RSP;
		  workDone = WORK_DONE;
                  dspuActTraceType = TraceTypeXid3Rsp;
	       }  
	 }
      else  if (lsP->workType & WORK_XIDNULL_REQ) {

	 /*---- Handle transmission of null XID, via ID.REQ. */
	 if ((idReqP = Cnstrct_IdReq (NON_BLOCKING, lsP->pCepId, 0, 0)) == 0)
	    workDone = NO_WORK_BUFFERS_AVAILABLE;
	 else {
	    lsP->workType ^= WORK_XIDNULL_REQ;
	    lsP->xidSent = TRUE;
	    lsP->workBufCnt++;
	    DspuQueuePut ((TCLSIMsg *) idReqP);
	    workDone = WORK_DONE;
            dspuActTraceType = TraceTypeXidNullReq;
	 }
      }
      else
	 if (lsP->workType & WORK_XIDNULL_RSP) {

	    /*---- Handle transmission of null XID, via ID.RSP. */
	    if ((idRspP = Cnstrct_IdRsp (NON_BLOCKING, lsP->pCepId, 0, 0)) == 0) 
	       workDone = NO_WORK_BUFFERS_AVAILABLE;
	    else {  
	       lsP->workType ^= WORK_XIDNULL_RSP;
	       lsP->xidSent = TRUE;
	       lsP->workBufCnt++;
	       DspuQueuePut ((TCLSIMsg *) idRspP);
	       workDone = WORK_DONE;
               dspuActTraceType = TraceTypeXidNullRsp;
	    }
         }
	 else

	    /*---- Handle REQ_OPNSTN.REQ, connect out. */
	    if (lsP->workType & WORK_ROSR_COUT) {
	       puP = GetSingleChild (&(lsP->puChildLoc));
	       rOSRP = PuSolicitReqOpnStnReq (puP, 
					      LsSetUCepID(SapQueryDirection(lsP->sapP),lsP->lsIndex));
	       if (rOSRP) {
		  if (SapRequestOpenStation (lsP->sapP, rOSRP)){
		     lsP->workBufCnt++;
		     workDone = WORK_DONE;
		     lsP->rosrSent = TRUE;
                     dspuActTraceType = TraceTypeConnOut;
		  }
		  else
		     workDone = WORK_NOT_DONE; /* wait for reactivation */
                  
		  lsP->workType ^= WORK_ROSR_COUT;
               }
	       else
		  workDone = NO_WORK_BUFFERS_AVAILABLE;
            }
	    else

	       /*---- Handle REQ_OPNSTN.REQ, connect in. */
	       if (lsP->workType & WORK_ROSR_CIN) {
		  byte rifLen;
		  byte* rifP;
		  XidAddrSapT xidAddrSap;
		  LlcParmsT* llcParmsP;
		  word pCepId;
		  Pu* puP = GetSingleChild (&(lsP->puChildLoc));

                  /* note that puP could be null, that is, we dont have a pu
                   * yet. PuSolicitLLCParms will fill in default value.
                   */
		  llcParmsP = PuSolicitLLCParms (LLC_Cfg_Parms, puP);

                  rifLen = IdStnInd_GetRif( (IdStnInd_t*) lsP->savClsiMsgP,
                                                   &rifP);

                  IdStnInd_GetAddrSap( (IdStnInd_t*) lsP->savClsiMsgP,
                                                       &xidAddrSap);

		  uCepId = LsSetUCepID( SapQueryDirection(lsP->sapP), 
                                          lsP->lsIndex);
		  pCepId = GetPCepID((IdStnInd_t *) lsP->savClsiMsgP);

		  rOSRP = Cnstrct_ReqOpnStnReq (NON_BLOCKING, uCepId, pCepId,
						&xidAddrSap.addr, 
                                                 xidAddrSap.rsap,
					 	 llcParmsP, rifLen, rifP);
		  if (rOSRP) {
		     if (SapRequestOpenStation (lsP->sapP, rOSRP)){
			lsP->workBufCnt++;
			workDone = WORK_DONE;
			lsP->rosrSent = TRUE;
                        dspuActTraceType = TraceTypeConnIn;
		     }
		     else
			workDone = WORK_NOT_DONE; /* wait for reactivation */
		     lsP->workType ^= WORK_ROSR_CIN;
                  }
		  else
		     workDone = NO_WORK_BUFFERS_AVAILABLE;
               }
   }

   /* Trace activation windows whenever work is done. */
   /* (Add 1 to SAP count because we are about to return WORK_DONE.) */
   if (workDone == WORK_DONE)
     DspuActivationWindowTrace(lsP->debugLevel, LsActWindowTrace,
                           puP, lsP->workBufCnt, (SapQueryBuffersUsed(lsP->sapP) +1));

   return (workDone);

} /* LsDoWork. */


/*----------------------------------------------------------------------------*
 * LsQueryBufferCount:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Query buffers tied up by this link station.
 *
 **/

hword
LsQueryBufferCount (LinkStn *lsP)

{

   return (lsP->workBufCnt);

} /* LsQueryBufferCount. */


/*----------------------------------------------------------------------------*
 * LsFreeBuffer:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Call sap to remove buffer from worklist.
 *
 **/

void
LsFreeBuffer (LinkStn *lsP)

{
   Pu *puP;


   if (lsP->workBufCnt > 0)
      lsP->workBufCnt--;
   else
      SapWorkBufferUnderflow();

   /* Trace the Free before doing the SapFreeGrantedWorkBuffer, because it may do 
      more work.                                                                 */
   puP = GetSingleChild (&(lsP->puChildLoc));
   dspuActTraceType = TraceTypeFree;
   DspuActivationWindowTrace(lsP->debugLevel, LsActWindowTrace,
                         puP, lsP->workBufCnt, (SapQueryBuffersUsed(lsP->sapP) - 1));

   SapFreeGrantedWorkBuffer (lsP->sapP, 1);

} /* LsFreeBuffer. */


/*----------------------------------------------------------------------------*
 * LsFlowControlFromClsi. *
 *----------------------------------------------------------------------------*
 *
 * Description:
 * If this flwctl action != last flwctl action, doit.
 *
 **/

void
LsFlowControlFromClsi (LinkStn *lsP,
                       FlowControlE flowCtrl)

{

   Pu *puP;


   if (flowCtrl != lsP->lastFlowRcvd) {
      lsP->lastFlowRcvd = flowCtrl;
      puP = GetSingleChild (&(lsP->puChildLoc));
      PuFlowControlFromLs (puP, flowCtrl);
   }

} /* LsFlowControlFromClsi. */


/*----------------------------------------------------------------------------*
 * LsFlowControlFromPu:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * The PU has requested us (the link station) to perform flow control.
 * We test and set flow counters with interrupts inhibited to protect
 * against concurrency (ie DSPU re-entrancy).
 *
 **/

void
LsFlowControlFromPu (LinkStn *lsP,
                     FlowControlE flowCtrl)

{

   switch (flowCtrl) {

    case FlwCtl_Stop:
      DISABLE_INTERRUPT
	 if (lsP->stopFlowCount == 0) {
	    lsP->stopFlowCount++;
	    ENABLE_INTERRUPT
	       LsFsm (lsP, LsI_FlowReqStop, 0);
         }
	 else {
	    lsP->stopFlowCount++;
	    ENABLE_INTERRUPT
	    }
      break;

    case FlwCtl_Resume:
      DISABLE_INTERRUPT
      
	 /* If an LU connection is established while flow control is off,
	  * then an extra FlwCtl_Resume will be received.  This extra
	  * Flw_Ctl_Resume should be ignored.
	  */
	 if (lsP->stopFlowCount > 0) {
	    lsP->stopFlowCount--;
	    if (lsP->stopFlowCount == 0) {
	       ENABLE_INTERRUPT
		  LsFsm (lsP, LsI_FlowReqResume, 0);
	       break;
	    }
	 }
      ENABLE_INTERRUPT;
        
      /* Ignore extra FlwCtl_Resumes
       */
      break;

    default:
      /*---- We will never get here. */
      break;
   }


} /* LsFlowControlFromPu. */


/*----------------------------------------------------------------------------*
 * LsProcessCmd:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Process Ls command.
 *
 **/

void
LsProcessCmd (LinkStn *lsP,
              LsInput lsInput,
              TCLSIMsg *clsiMsgP)

{


   LsFsm (lsP, lsInput, clsiMsgP);

} /* LsProcessCmd. */


/*----------------------------------------------------------------------------*
 * LsFsm:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * This fsm does the bulk of link station command/signal processing.
 *
 **/

void
LsFsm (LinkStn *lsP,                    /* Addr of LS block. (Input). */
       LsInput lsInput,                 /* LS input. (Input). */
       TCLSIMsg *clsiMsgP)              /* Addr of CLSI buffer. (Input). */

{

   FsmStruct lsFsm;
   LinkStn *ls2P;
   Pu *puP;
   hword maxIFrame;
   byte windowSize;
   CloseStnReq_t *closeStnReqP;
   DisconnectReq_t *disConnReqP;
   DisconnectRsp_t *disConnRspP;
   FlowReq_t *flowReqP;
   TCLSIMsg *idSigP;
   XidAddrSapT rXidAddrSap;
   boolean freeBuf;
   LinkType linkDirection;
   LsWorkType workType;
   NegErrE xidNegParms;
   boolean lsDeleted;
   word pCepId;


   lsFsm = LsFsmTable[lsP->fsmState][lsInput];
   puP = GetSingleChild (&(lsP->puChildLoc));
   if (lsP->fsmState != lsFsm.state) {
       DspuFsmChgTrace(lsP->debugLevel, LsFsmChgTrace, puP, 
                         lsEvent[lsInput], 
                         linkStationStatus[lsP->fsmState],
                         linkStationStatus[lsFsm.state] );
   }
   lsP->fsmState = lsFsm.state;
   DspuFsmTrace (lsP->debugLevel, LsFsmTrace, puP, 
                   lsInput, lsFsm.state, lsFsm.action);
   freeBuf = TRUE;

   /*---- SPLIT ON LINK STATION ACTION. */

   switch (lsFsm.action) {

      /*---- NOP. */
    case a0:
      break;

      /*---- Initiate connect out sequence. */
    case a1:
      lsP->lsReqState = LsRS_Connected;
      if (puP){
	 LsLogForWork (lsP, WORK_ROSR_COUT);
         lsP->retry = GetPuMaxRetryCount(puP);   /* save the retry count */
      }
      break;

      /*--- Disconnect station before open Confirm received */
    case a26:
      closeStnReqP = Cnstrct_CloseStnReq (0, 
                       LsSetUCepID(SapQueryDirection(lsP->sapP), lsP->lsIndex));
      DspuQueuePut ((TCLSIMsg *) closeStnReqP);
      /* fall down to a2 to clean up state */

      /*---- Error on REQ_OPNSTN.REQ. Destroy ls-pu pair. */
    case a2:
      lsP->lsReqState = LsRS_Closed;
      lsP->pCepId = 0;
      LsFreeBuffer (lsP);
      if (puP) {
         PuConnectOutErrorPrint (puP);
       }
       LsDelete(lsP);

      break;

      /*---- Exec'd to terminate ls-pu pair. We ship a CLOSE_STN.REQ. The
	CLOSE_STN.CNF will cause destruction. */
    case a3:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      lsP->lsReqState = LsRS_Closed;
      closeStnReqP = Cnstrct_CloseStnReq (lsP->pCepId, 0L);
      DspuQueuePut ((TCLSIMsg *) closeStnReqP);
      StartItimer (&(lsP->lsItimer), CloseStnITime, LsItimer, lsP);
      break;

      /*---- Exec'd to stop pu. This is not currently executed by anyone. It's
	really network management code; for the future. */
    case a4:
      lsP->lsReqState = LsRS_Closed;
      if (puP)
	 ProcessPuCommand (puP, PuStopCmd);
      break;

      /*---- Connect in <-> connect out crossover.
	At this point we've accepted an ID_STN.IND (connect in) during
	connect out. Remember this fact by saving the incoming p_cep_id
	from the ID_STN.IND. The FSM changes state to "PendClose". Later
	we detect contention if the REQ_OPNSTN.CNF p_cep_id is different
	from the ID_STN.IND p_cep_id. See comments at a7 & a25 for more. */
    case a5:
      lsP->savClsiMsgP = clsiMsgP;
      lsP->pCepId = GetPCepID ((IdStnInd_t *) clsiMsgP);
      freeBuf = FALSE;
      break;

      /*---- REQ_OPNSTN.CNF on connect out. Ship out a null XID. */
    case a6:
      LsFreeBuffer(lsP);
      lsP->rosrSent = FALSE;
      lsP->pCepId = GetPCepID ((ReqOpnStnCnf_t *) clsiMsgP);
      if (lsP->lsReqState == LsRS_Connected) {
	 LsLogForWork (lsP, WORK_XIDNULL_REQ);
      }
      else
	 LsFsm (lsP, LsI_ResetLs, 0);
      break;

      /*---- REQ_OPNSTN.CNF on connect in. Process the XID buried in the
	ID_STN.IND previously received.
	Note: This action may well have been executed during
	connect in <-> connect out contention resolution. However,
	the dlc must have been clever enough to resolve the contention.
	We say great, no more resolution work for us to do, and carry
	on as if nothing special happened. */
    case a7:
      LsFreeBuffer(lsP);
      lsP->rosrSent = FALSE;
      lsP->pCepId = GetPCepID ((ReqOpnStnCnf_t *) clsiMsgP);
      if (lsP->lsReqState == LsRS_Connected) {
 	 switch (XidGetFormat (lsP->savClsiMsgP)) {
	  case xidFmt_Null:
	    LsFsm (lsP, LsI_IdIndCnfNull, lsP->savClsiMsgP);
	    break;
	  case xidFmt_0:
	    LsFsm (lsP, LsI_IdIndCnf0, lsP->savClsiMsgP);
	    break;
	  case xidFmt_3:
	    LsFsm (lsP, LsI_IdIndCnf3, lsP->savClsiMsgP);
	    break;
	  default:
	    /* (xidFmt_Bad). */
	    break;
	 }
	 lsP->savClsiMsgP = NULL;
      }
      else
	 LsFsm (lsP, LsI_ResetLs, 0);
      break;


      /*---- Received null XID (on connect in) with no pu; return same to
	other end. */
    case a8:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      workType = (CLSIMsgIsIndication (clsiMsgP)) ?
	 WORK_XIDNULL_RSP : WORK_XIDNULL_REQ;
      LsLogForWork (lsP, workType);
      break;

      /*---- Received null XID on connect in or out with pu. If upstream,
	ship a format 3 XID. If downstream, ship a null XID. */
    case a9:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      if (puP){
	 LsSendXidOrConnect (DnStrWk_SendNullXid, lsP, clsiMsgP, TRUE);
      }
      break;

      /*---- Received format 0 XID with pu. For upstream, ship a format 3
	XID. For downstream, find (or create a default) pu, and ship
	out a CONNECT.REQ. */
    case a10:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      rXidAddrSap.u_xid.xid = XidGetXid (clsiMsgP);
      snpa_copy(LsGetAddrP(lsP), &rXidAddrSap.addr);
      rXidAddrSap.rsap = lsP->rSap;
      rXidAddrSap.lsapP = lsP->sapP;
      rXidAddrSap.searchMode = XMS_AddrXid;
      if ((puP = LsFindInactivePu (lsP, &rXidAddrSap)) != 0){
         lsP->retry = GetPuMaxRetryCount(puP);   /* save the retry count */
	 LsSendXidOrConnect (DnStrWk_SendConnect, lsP, clsiMsgP, TRUE);
         SetPuRcvdXidFmt(puP, XidGetFormat(clsiMsgP));
         SetPuRcvdXid(puP, XidGetXid(clsiMsgP));
      }
      else			/* Pu search/creation failure. :-(. */
	 LsFsm (lsP, LsI_ResetLs, 0);
      break;

      /*---- Received format 0 XID with pu. Ship out a CONNECT.REQ. */
    case a11:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      if (puP) {
         lsP->retry = GetPuMaxRetryCount(puP);   /* save the retry count */
	 LsSendXidOrConnect (DnStrWk_SendConnect, lsP, clsiMsgP, TRUE);
         SetPuRcvdXidFmt(puP, XidGetFormat(clsiMsgP));
         SetPuRcvdXid(puP, XidGetXid(clsiMsgP));
      }
      break;

      /*---- Solicit pu for negotiation parameters, and ship them,
	encapsulated in a CONNECT.REQ signal. */
    case a12:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      StopItimer (&(lsP->lsItimer)); /* Halt any outstanding ls timer. */
      if (puP) {
	 ConnectReq_t *connReqP;
	 LlcParmsT* llcParmsP;
	 llcParmsP = PuSolicitLLCParms (LLC_Neg_Parms, puP);
	 connReqP = Cnstrct_ConnectReq (lsP->pCepId, llcParmsP);
	 DspuQueuePut ((TCLSIMsg *) connReqP);
      }
      break;

      /*---- Connect succeeded: ship a SIGNAL_STN.REQ to the other end,
	and start the pu. */
    case a13:
      if (puP) {
	 SignalStnReq_t *sigStnReqP;
	 sigStnReqP = Cnstrct_SigStnReq (lsP->pCepId);
	 DspuQueuePut ((TCLSIMsg *) sigStnReqP);
	 ProcessPuCommand (puP, PuStartCmd);
	 linkDirection = PuQueryDirection (puP);
         if (!IsPuDefault(puP)) {
             DspuActivationTrace(lsP->debugLevel, LsActivatedTrace, puP,
                                   GetPuName(puP), linkDirection);
         } else {
             DspuActivationTrace(lsP->debugLevel, LsActivatedTrace, NULL,
                                   "DFLT-PU", linkDirection);
         }
      }
      break;

      /*---- Same as a12, but ship resonse not request. */
    case a14:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      if (puP) {
	 ConnectRsp_t *connRspP;
         LlcParmsT* llcParmsP;
	 llcParmsP = PuSolicitLLCParms (LLC_Neg_Parms, puP);
	 connRspP = Cnstrct_ConnectRsp (lsP->pCepId, llcParmsP);
	 DspuQueuePut ((TCLSIMsg *) connRspP);
      }
      break;

      /*---- Took a reset ls on connect in with no pu. We will simply
	self destruct. */
    case a15:
      LsFreeBuffer (lsP);
      PuConnectInErrorPrint (puP, LsGetAddrP(lsP));
      lsDeleted = LsDelete (lsP);
      break;

      /*---- DISCONNECT.IND received. Ship response & CLOSE_STN.REQ. This
	will ultimately cause destruction to this ls-pu pair. */
    case a16:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      if (lsP->pCepId == 0){    /* OpnStnRqst not confirmed yet */
 
         /* we cannot sent a disconnect response because we did not
          * have a handle.
          */
         closeStnReqP = Cnstrct_CloseStnReq (0, 
                       LsSetUCepID(SapQueryDirection(lsP->sapP), lsP->lsIndex));
         DspuQueuePut ((TCLSIMsg *) closeStnReqP);
     }
     else{
	 disConnRspP = Cnstrct_DisConnectRsp (lsP->pCepId);
	 DspuQueuePut ((TCLSIMsg *) disConnRspP);
	 closeStnReqP = Cnstrct_CloseStnReq (lsP->pCepId, 0L);
	 DspuQueuePut ((TCLSIMsg *) closeStnReqP);
         if (puP)
	    ProcessPuCommand (puP, PuResetCmd);
     }
     StartItimer (&(lsP->lsItimer), CloseStnITime, LsItimer, lsP);
     break;

      /*---- DATA.IND from other end. Go directly to pu. */
    case a17:
      if (puP) {
	 ReceivePu (puP, (DataInd_t *) clsiMsgP);
	 freeBuf = FALSE;
      }
      break;

      /*---- NOP. */
    case a18:
      break;

      /*---- This action is executed if we receive an ID_STN.IND signal when
	we're not expecting one. */
    case a19:
      pCepId = GetPCepID((IdStnInd_t*) clsiMsgP);
      if (pCepId != CLS_NO_CEP) {
          closeStnReqP = Cnstrct_CloseStnReq (pCepId, 0L);
          DspuQueuePut ((TCLSIMsg *) closeStnReqP);
      }
      break;

      /*---- DATA.REQ coming from pu. Truck on thru pu! */
    case a20:
      SetPCepID ((DataReq_t *) clsiMsgP, lsP->pCepId);
      DspuQueuePut (clsiMsgP);
      freeBuf = FALSE;
      break;

      /*---- CLOSE_STN.CNF arrived. Time to shut this ls-pu pair down. */
    case a21:
      StopItimer (&(lsP->lsItimer)); /* Halt any outstanding ls timer. */
      if (lsP->rosrSent || lsP->xidSent)
	 LsFreeBuffer(lsP);
      if (puP) {
	 linkDirection = PuQueryDirection (puP);
           
	 DspuActivationTrace(lsP->debugLevel, LsDeactivatedTrace, puP,
                               GetPuName(puP), linkDirection);
           
      }
      LsDelete (lsP);
      break;

      /*---- Received CONNECTED.IND signal. Print connected message. */
    case a22:
      if (puP) {
	 linkDirection = PuQueryDirection (puP);
         if (!IsPuDefault(puP)) {
             DspuActivationTrace(lsP->debugLevel, LsActivatedTrace, puP,
                                   GetPuName(puP), linkDirection);
         } else {
             DspuActivationTrace(lsP->debugLevel, LsActivatedTrace, NULL,
                                   "DFLT-PU", linkDirection);
         }
      }
      break;

      /*---- Received ID_STN.IND, and there's no crossover. Send
	REQ_OPNSTN.REQ connect in to other end. */
    case a23:
      lsP->lsReqState = LsRS_Connected;
      lsP->savClsiMsgP = clsiMsgP;
      freeBuf = FALSE;
      LsLogForWork (lsP, WORK_ROSR_CIN);
      break;

      /*---- Received stopped signal from pu. Ship DISCONNECT.REQ to
	other end. */
    case a24:
      disConnReqP = Cnstrct_DisConnectReq (lsP->pCepId);
      DspuQueuePut ((TCLSIMsg *) disConnReqP);
      break;

      /*---- Connect in <-> connect out crossover.
	We previously received an ID_STN.IND, and the dlc did not
	resolve the contention. We'll resolve the contention by
	terminating the connect out session, creating a new link
	station, connecting the pu to the new ls, and issuing a
	(REQ_OPNSTN.REQ) connect in. */
    case a25:
      LsFreeBuffer(lsP);
      lsP->rosrSent = FALSE;
      if (puP) {
	 pCepId = GetPCepID((ReqOpnStnCnf_t*) clsiMsgP);
         if (pCepId != CLS_NO_CEP) {
	     closeStnReqP = Cnstrct_CloseStnReq (pCepId, 0L);
	     DspuQueuePut ((TCLSIMsg *) closeStnReqP);
         }
	 ls2P = LsCreate (lsP->lsChildLocP, LsGetAddrP(lsP),
                          lsP->rSap, lsP->sapP);
	 if (ls2P) {
	    PuChangeToActive (puP, ls2P);
	    LsFsm (ls2P, LsI_IdStnIndPU, lsP->savClsiMsgP);
	    lsP->savClsiMsgP = NULL;
	 }
	 else {
            uchar  addr[STATIONLEN_MAX];
            ushort addrLen = snpa_get_length(LsGetAddrP(lsP));
            snpa_copy_to_buffer(LsGetAddrP(lsP), (uchar *) &addr);
	    errmsg(&msgsym(LSConnInFailedNoMem, DSPU),
		    ByteToHexAscii((byte *) &addr, addrLen));
	 }
      }
      break;


      /*---- Received format 3 XID. Same as a10, except look for matching
	inactive (or create default) pu. Update the pu's negotiation
	parameters. Now, if upstream send format 3 XID, else if downstream
	ship out a CONNECT.REQ. */
    case a27:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      rXidAddrSap.u_xid.xid = XidGetXid (clsiMsgP);
      snpa_copy(LsGetAddrP(lsP), &rXidAddrSap.addr);
      rXidAddrSap.rsap = lsP->rSap;
      rXidAddrSap.lsapP = lsP->sapP;
      rXidAddrSap.searchMode = XMS_AddrXid;
      if ((puP = LsFindInactivePu (lsP, &rXidAddrSap)) != 0) {
	 PuChangeToActive (puP, lsP);
	 windowSize = XidGetWindowSize (clsiMsgP);
         maxIFrame = XidGetMaxIFrame (clsiMsgP);
	 xidNegParms = PuQueryUpdtNegConnParms (puP, windowSize, maxIFrame);
	 if (xidNegParms == Xid_NP_NoError) {
            lsP->retry = GetPuMaxRetryCount(puP);   /* save the retry count */
	    LsSendXidOrConnect (DnStrWk_SendConnect, lsP, clsiMsgP, FALSE);
            SetPuRcvdXidFmt(puP, XidGetFormat(clsiMsgP));
            SetPuRcvdXid(puP, XidGetXid(clsiMsgP));
         }
	 else {
	    LsXidNegErrMsg (xidNegParms, clsiMsgP, maxIFrame, windowSize);
	    idSigP = (TCLSIMsg *) Cnstrct_IdSigWithNegError (xidNegParms,
							     lsP->pCepId,
							     clsiMsgP);
            if (idSigP) 
              DspuQueuePut (idSigP);
	 }
      }
      else
	 LsFsm (lsP, LsI_ResetLs, 0);
      break;

      /*---- (Hopefully) The pu already exists. Do same as a27, except without
	the pu matching or creation. */
    case a28:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      if (puP) {
	 windowSize = XidGetWindowSize (clsiMsgP);
         maxIFrame = XidGetMaxIFrame (clsiMsgP);
	 xidNegParms = PuQueryUpdtNegConnParms (puP, windowSize, maxIFrame);
	 if (xidNegParms == Xid_NP_NoError) {
            lsP->retry = GetPuMaxRetryCount(puP);   /* save the retry count */
	    LsSendXidOrConnect (DnStrWk_SendConnect, lsP, clsiMsgP, FALSE);
            SetPuRcvdXidFmt(puP, XidGetFormat(clsiMsgP));
            SetPuRcvdXid(puP, XidGetXid(clsiMsgP));
         }
	 else {
	    LsXidNegErrMsg (xidNegParms, clsiMsgP, maxIFrame, windowSize);
	    idSigP = (TCLSIMsg *) Cnstrct_IdSigWithNegError (xidNegParms,
							     lsP->pCepId,
							     clsiMsgP);
            if (idSigP)
              DspuQueuePut (idSigP);
	 }
      }
      else
	 LsFsm (lsP, LsI_ResetLs, 0);
      break;

      /*---- Send a flow control stop signal. (Called from pu code only). */
    case a29:
      flowReqP = Cnstrct_FlowReq (BLOCKING, lsP->pCepId, FlwCtl_Stop);
      DspuQueuePut ((TCLSIMsg *) flowReqP);
      break;

      /*---- Send a flow control resume signal. (Called from pu code only). */
    case a30:
      flowReqP = Cnstrct_FlowReq (BLOCKING, lsP->pCepId, FlwCtl_Resume);
      DspuQueuePut ((TCLSIMsg *) flowReqP);
      break;

      /*---- Exec'd when REQ_OPNSTN.CNF received in incorrect state. */
    case a31:
      LsFreeBuffer(lsP);
      break;

      /*---- Took a reset ls during connect in with a pu. Destroy ls-pu pair,
	and print message. */
    case a32:
      lsP->lsReqState = LsRS_Closed;
      lsP->pCepId = 0;
      LsFreeBuffer (lsP);
      PuConnectInErrorPrint (puP, LsGetAddrP(lsP));
      LsDelete(lsP);
      break;

      /*--- Xid3 was rejected. now set up the timer to resend it. */

    case a33:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      StartItimer (&(lsP->lsItimer), GetPuMaxRetryTimeout(puP), LsItimer, lsP);
      break;
    
    case a34:
      if (lsP->xidSent){
	 lsP->xidSent = FALSE;
	 LsFreeBuffer(lsP);
      }
      if (lsP->retry == 0) {           /* exhaust all retries */
 	  LsFsm(lsP, LsI_ResetLs, 0);   /* reset the retry */
      }
      else {
         if (lsP->retry != INFINITE_MAX_RETRY_COUNT){
              lsP->retry--;
         }
         LsLogForWork(lsP, WORK_XID3_REQ2);   /* send xid 3 again */
         StartItimer (&(lsP->lsItimer), GetPuMaxRetryTimeout(puP), LsItimer, lsP);
      }
      break;

      /*---- End of the current LSFSM instruction set! */
    default:
      break;
   } /* switch (lsFsm.action). */

   if ((freeBuf) && (clsiMsgP))
      CLSIMsgDestroy (clsiMsgP);

   /* To avoid any slow "leakage" in the work buffers ensure
      that all buffers are freed when in the reset state. 
      At this time there should be no activation being done by
      either the link station or the resources above it (PU and LU).  */

   if (lsP->fsmState == LsS_Reset)
     if (lsP->workBufCnt > 0) {
       SapFreeGrantedWorkBuffer (lsP->sapP, lsP->workBufCnt); 
       lsP->workBufCnt = 0;
       dspuActTraceType = TraceTypeFree;
       DspuActivationWindowTrace(lsP->debugLevel, LsActWindowTrace,
                             puP, lsP->workBufCnt, SapQueryBuffersUsed(lsP->sapP));

     }

} /* LsFsm. */


/*----------------------------------------------------------------------------*
 * LsMatchPu:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Does the PU owned by this LS have a matching addr, sap, & xid combination?
 *
 **/

static boolean
LsMatchPu (LinkStn* lsP,
           XidAddrSapT *xmsP)

{

   boolean matched;
   Pu* puP;

   puP = GetSingleChild (&(lsP->puChildLoc));
   matched = (puP) ? PuMatched (puP, xmsP) : FALSE;
   return (matched);

} /* LsMatchPu. */


/*============================================================================*
 * LsMatchConnOutPuOnSap()
 *============================================================================*
 *
 * Description:
 *   Match PU if connectOut was initiated on the given SAP
 *
 **/
static boolean LsMatchConnOutPuOnSap (LinkStn* lsP, Sap *sapP)
{

   Pu       *puP;
   Adaptor  *puAdaptorP, *inputAdaptorP;
   boolean matched = FALSE;

   puP = GetSingleChild (&(lsP->puChildLoc));
   if (puP) {
     puAdaptorP = FindAdaptor(GetPuAdaptorIdP(puP));
     inputAdaptorP = FindAdaptor(SapGetAdaptorId(sapP));
     matched = ( (puAdaptorP == inputAdaptorP) && 
                 (GetPuLocalSapAddr(puP) == SapGetSapNum(sapP)) &&
                 (IsPuStarted(puP)) );
   }
   return(matched);

} /* LsMatchConnOutPuOnSap */


/*============================================================================*
 * LsMatchConnOutPuOnAdaptor()
 *============================================================================*
 *
 * Description:
 *   Match PU if connectOut was initiated on the given adaptorId
 *
 **/
static boolean LsMatchConnOutPuOnAdaptor (LinkStn* lsP, AdaptorId *inputIdP)
{

   Pu       *puP;
   Adaptor  *puAdaptorP, *inputAdaptorP;
   boolean matched = FALSE;

   puP = GetSingleChild (&(lsP->puChildLoc));
   if (puP) {
     puAdaptorP = FindAdaptor(GetPuAdaptorIdP(puP));
     inputAdaptorP = FindAdaptor(inputIdP);
     matched = ((puAdaptorP == inputAdaptorP) && IsPuStarted(puP));
   }
   return(matched);

} /* LsMatchConnOutPuOnAdaptor */


/*=====================================================================*
* QueryLsConnOutPuOnSap()
*======================================================================*
*
* Description:
*
*   Search for any linkStations on the given SAP 
*     where the PU initiated connectOut
*
**/
boolean QueryLsConnOutPuOnSap(Sap *sapP)
{
 
  LinkStn *lsP;

  /*
   * Search for active Pu linkStation on UplsRoot
   *
   */
  lsP = QueryAllChildrenParm( GetUplsRoot(),
                              (ProcQChild1) LsMatchConnOutPuOnSap, sapP);
  if (lsP) {
    return(TRUE);
  }
 
  /*
   * Search for active Pu linkStation on DnlsRoot
   *
   */
  lsP = QueryAllChildrenParm( GetDnlsRoot(),
                              (ProcQChild1) LsMatchConnOutPuOnSap, sapP);
  if (lsP) {
    return(TRUE);
  }
 
  /*
   * Else, no active PU linkStation found on the given Sap
   *
   */
  return(FALSE);
 
} /* End QueryLsConnOutPuOnSap() */


/*=====================================================================*
* QueryLsConnOutPuOnAdaptor()
*======================================================================*
*
* Description:
*
*   Search for any linkStations on the given Adaptor
*     where the PU initiated connectOut
*
**/
boolean QueryLsConnOutPuOnAdaptor(AdaptorId *adaptorIdP)
{
 
  LinkStn *lsP;

  /*
   * Search for active Pu linkStation on UplsRoot
   *
   */
  lsP = QueryAllChildrenParm( GetUplsRoot(),
                              (ProcQChild1) LsMatchConnOutPuOnAdaptor,
                              adaptorIdP);
  if (lsP) {
    return(TRUE);
  }
 
  /*
   * Search for active Pu linkStation on DnlsRoot
   *
   */
  lsP = QueryAllChildrenParm( GetDnlsRoot(),
                              (ProcQChild1) LsMatchConnOutPuOnAdaptor,
                              adaptorIdP);
  if (lsP) {
    return(TRUE);
  }
 
  /*
   * Else, no active PU linkStation found on the given Sap
   *
   */
  return(FALSE);
 
} /* End QueryLsConnOutPuOnAdaptor() */


/*----------------------------------------------------------------------------*
 * LsFindActivePu:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Find pu in the 'active-list' (of link stations) 
 *   which has a matching addr, sap & xid combination.
 *
 **/

Pu*
LsFindActivePu (Sibling* lsRootP,
                XidAddrSapT *xmsP)

{

   Pu* puP;
   LinkStn* lsP;


   lsP = QueryAllChildrenParm (lsRootP, (ProcQChild1)LsMatchPu, xmsP);
   puP = (lsP) ? GetSingleChild (&(lsP->puChildLoc)) : 0;
   return (puP);

} /* LsFindActivePu. */


/*----------------------------------------------------------------------------*
 * LsFindInactivePu:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Locate PU. If found in inactive list, change to active, and connect to
 * current link station. If not found, create and connect up a default pu.
 * Note: This last action is not performed for upstream hosts! There is no
 * concept of a default host.
 *
 **/

Pu *
LsFindInactivePu (LinkStn *lsP,
                  XidAddrSapT *rXidAddrSapP)

{

   Pu *puP;
   Sibling *puInactRootP;
   LinkType sapDirection = SapQueryDirection (lsP->sapP);
   
   puInactRootP =  sapDirection == Upstream ? 
      GetInactUpuRoot() : GetInactDpuRoot();

   if ((puP = PuFindInactive (puInactRootP, rXidAddrSapP)) != 0)
      PuChangeToActive (puP, lsP);
   else  if (sapDirection == Downstream)
      puP = ConnectDefaultPu (lsP, rXidAddrSapP);

   return (puP);

} /* LsFindInactivePu. */


/*----------------------------------------------------------------------------*
 * LsSendXidOrConnect:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * If upstream send appropriate id.(req,rsp) signal, containing an XID fmt3
 * packet. If downstream: send an appropriate id.(req,rsp) signal,
 * containing a null XID, but limit null xid sending to 2; or send a
 * connect.req signal, depending on the type of downstream work requested.
 *
 **/

static void
LsSendXidOrConnect (LsDownStreamWork lsDownStreamWork,
                    LinkStn *lsP,
                    TCLSIMsg *clsiMsgP,
                    boolean   xid3Negotiation)   /* if true, this is first xid3 */

{

   LsWorkType workType;


   workType = (CLSIMsgGetPrimitiveType (clsiMsgP) == CLS_CONFIRM) ?
      WORK_XID3_REQ : WORK_XID3_RSP;

   if (SapQueryDirection(lsP->sapP) == Upstream) {
      
      if (workType == WORK_XID3_REQ && xid3Negotiation)
          workType = WORK_XID3_REQ2;    /* pass negotiation */
      LsLogForWork(lsP, workType);
   }
   else {
      if (lsDownStreamWork == DnStrWk_SendNullXid) {
	 if (lsP->numNullXidSent++ < MAX_NULLXID_RETRIES) {
	    workType = (workType == WORK_XID3_REQ) ?
	       WORK_XIDNULL_REQ : WORK_XIDNULL_RSP;
	    LsLogForWork (lsP, workType);              
	 }
	 else {
	    lsP->numNullXidSent = 0;
	    LsFsm (lsP, LsI_ResetLs, 0);
	 }
      }
      else
	 if (lsDownStreamWork == DnStrWk_SendConnect)
	    LsFsm (lsP, LsI_ConnectLs, 0);
   }

} /* LsSendXidOrConnect. */


/*----------------------------------------------------------------------------*
 * LsItimer:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Link Station Interval Timer.
 *
 **/

static void
LsItimer (LinkStn *lsP)

{

   LsFsm (lsP, LsI_Timeout, 0);	/* Process timeout. */

} /* LsItimer. */


/*----------------------------------------------------------------------------*
 * LsStartITimer:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * External entrypoint to start LS interval timer.
 *
 **/

void
LsStartITimer (LinkStn *lsP,
               hword timeInterval)

{


   StartItimer (&(lsP->lsItimer), timeInterval, LsItimer, lsP);

} /* LsStartITimer. */



/*----------------------------------------------------------------------------*
 * LsDebug()
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Turn on debugging in ls and sub-ordinate pu.
 *
 **/
void DebugLs (LinkStn *lsP, hword   debugLevel)
{

   Pu* puP;

   lsP->debugLevel |= debugLevel;
   puP = GetSingleChild (&(lsP->puChildLoc));
   if (puP)
      DebugPu (puP, debugLevel);

} /* DebugLs() */


/*----------------------------------------------------------------------------*
 * LsNoDebug:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Turn off debugging in ls and sub-ordinate pu.
 *
 **/

void NoDebugLs (LinkStn *lsP, hword debugLevel)
{

   Pu* puP;

   lsP->debugLevel &= ~debugLevel;
   puP = GetSingleChild (&(lsP->puChildLoc));
   if (puP)
      NoDebugPu (puP, debugLevel);

} /* NoDebugLs() */


/*----------------------------------------------------------------------------*
 * LsShow:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Show ls and sub-ordinate pu (name).
 *
 **/

void       
ShowLs (LinkStn *lsP, 
        hword all)

{

   Pu* puP;


   puP = GetSingleChild (&(lsP->puChildLoc));
   if (puP)
      ShowPu (puP, all);

} /* LsShow. */

/*----------------------------------------------------------------------------*
 * Pu interface routines
 *----------------------------------------------------------------------------*
 *
 * Description:
 *    The active PU iteration is done on LS where it has a root at dspu structure.
 * The following functions should be virtual function to work on PU but since we
 * dont have virtual funtions, we provide a funtion to call PU functions.
 *
 **/

void LsWrite (LinkStn *lsP)
{
   Pu* puP = GetSingleChild(&(lsP->puChildLoc));
   if (puP)
      PuWrite(puP);
}

void LsWriteRsrbStart (LinkStn *lsP)
{
   Pu* puP = GetSingleChild(&(lsP->puChildLoc));
   if (puP)
      DspuPuWriteRsrbStart(puP);
}

void LsWriteVdlcStart (LinkStn *lsP)
{
   Pu* puP = GetSingleChild(&(lsP->puChildLoc));
   if (puP)
      DspuPuWriteVdlcStart(puP);
}

void LsWriteOut (LinkStn *lsP, AdaptorId* idP)
{

   Pu* puP = GetSingleChild (&(lsP->puChildLoc));
   if (puP)
      PuWriteOut (puP, idP);
} 

void LsDeletePoolRangeIfMatch (LinkStn *lsP, const char* name)
{

   Pu* puP = GetSingleChild (&(lsP->puChildLoc));
   if (puP)
       DeletePoolRangeIfMatch (puP, name);
} 

/*----------------------------------------------------------------------------*
 * LsQueryFindChild:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Find pu with matching name.
 *
 **/

boolean                        /* TRUE if child pu with pu name existed */
QueryFindChild (LinkStn* lsP,
                const char* puName)

{
   Pu* puP = GetSingleChild (&(lsP->puChildLoc));
   return puP ? QueryMatchPuName (puP, puName) : FALSE;

} /* LsQueryFindChild. */


/*----------------------------------------------------------------------------*
 * LsXidNegErrMsg:
 *----------------------------------------------------------------------------*
 *
 * Description:
 * Print XID negotiation error message.
 *
 **/

void
LsXidNegErrMsg (NegErrE negErr,
                TCLSIMsg* clsiMsgP,
                hword maxIFrame,
                byte windowSize)

{

   byte* xidP;
   hword xidLn;


   XidGetXidData (clsiMsgP, &xidP, &xidLn);
   switch (negErr) {

    case Xid_NP_NoActpuCap:
      errmsg(&msgsym(LSNoACTPUCapability, DSPU), ByteToHexAscii(xidP, 4));
      break;

    case Xid_NP_BadWindowSize:
      errmsg(&msgsym(LSInvalidWindowSize, DSPU), ByteToHexAscii(xidP, 4),
	      windowSize);
      break;

    case Xid_NP_BadMaxIFrame:
      errmsg(&msgsym(LSInvalidMaxIFrame, DSPU), ByteToHexAscii(xidP, 4),
	      maxIFrame);
      break;

    default:
      break;

   }

} /* LsXidNegErrMsg. */


static char* const lsEvent[] = {
        "StartLs", "StopLs", "ResetLs", "IdStn.Ind", "IdStn.Ind.PU",
        "ReqOpn.Cnf", "ReqOpn.CnfX", "IdInd.Cnf.Null", "IdInd.Cnf0",
        "IdInd.Cnf3", "ConnectLs", "Connect.Cnf", "Connect.Ind",
        "Connected.Ind", "SigStn.Cnf", "Disc.Cnf", "Disc.Ind",
        "Close.Cnf", "Data.Ind", "Data.Req", "Flow.Req.Stop",
        "Flow.Req.Resume", "Timeout", "PuStopped"
};

