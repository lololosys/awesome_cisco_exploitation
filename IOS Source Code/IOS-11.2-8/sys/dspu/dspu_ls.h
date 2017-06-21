/* $Id: dspu_ls.h,v 3.1.62.2 1996/08/28 03:58:22 bchan Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_ls.h,v $
 *------------------------------------------------------------------
 * DSPU Link Station Structure.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This contains the structure definition of the link station control block.
 *------------------------------------------------------------------
 * $Log: dspu_ls.h,v $
 * Revision 3.1.62.2  1996/08/28  03:58:22  bchan
 * CSCdi65090:  NSP stops sending xid when switched major nodes inactivated
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/05/17  10:47:42  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.50.1  1996/03/28  08:20:26  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.1.6.1  1996/01/20  06:08:42  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.1  1995/11/09  11:28:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:58  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __LS_H                         
#define __LS_H

#include "dtypes.h"
#include "dspu_cps.h"
#include "dspu_itimer.h"
#include "wrkbblk.h"


/*---- The following defines and enumerated types are exported. */

/*---- Link station work types. */

#define WORK_PU          0x1            /* PU work. */
#define WORK_XIDNULL_REQ 0x2            /* ID.REQ w/ null xid. */
#define WORK_XIDNULL_RSP 0x4            /* ID.RSP w/ null xid. */
#define WORK_ROSR_COUT   0x8            /* REQ_OPNSTN.REQ connect out. */
#define WORK_ROSR_CIN    0x10           /* REQ_OPNSTN.REQ connect in. */
#define WORK_XID3_REQ    0x20 
#define WORK_XID3_RSP   0x40
/* fix vtam 4.1 bug, to pass XID3_REQ with prenegotiation */
#define WORK_XID3_REQ2   0x80


typedef hword LsWorkType;

typedef enum LsReqState {
        LsRS_Connected, LsRS_Closed} LsReqState;

typedef enum LsState {                  /* State conditions for LS FSM. */
        LsS_Reset, LsS_PendOut, LsS_PendIn, LsS_PendInPu, LsS_PendXid, LsS_Xid,
        LsS_ConnOut, LsS_ConnIn, LsS_Conn, LsS_PendDisc, LsS_PendClose,
        LsS_MaxState
        } LsState;

typedef enum LsInput {                  /* Inputs for LS FSM. */
        LsI_StartLs, LsI_StopLs, LsI_ResetLs, LsI_IdStnInd, LsI_IdStnIndPU,
        LsI_ReqOpnCnf, LsI_ReqOpnCnfX, LsI_IdIndCnfNull, LsI_IdIndCnf0,
        LsI_IdIndCnf3, LsI_ConnectLs, LsI_ConnectCnf, LsI_ConnectInd,
        LsI_ConnectedInd, LsI_SigStnCnf, LsI_DiscCnf, LsI_DiscInd,
        LsI_CloseCnf, LsI_DataInd, LsI_DataReq, LsI_FlowReqStop,
        LsI_FlowReqResume, LsI_Timeout, LsI_PuStopped, LsI_Xid3Err, 
        LsI_MaxInput
        } LsInput;

/*---- The following structure(s) are exported. */

  struct LinkStn {                      /* Link station structure. */
        Parent_t lsParentP;             /* Ptr to parent of this structure. */
        Sibling *lsChildLocP;           /* Ptr to ls child locato. */
        Sap *sapP;                      /* Pointer to SAP control block. */
        TCLSIMsg* savClsiMsgP;          /* Ptr to saved clsi buffer w/ XID. */
        SingleChild puChildLoc;         /* PU child locator structure. */
        LsState fsmState;               /* State of fsm for this ls. */
        LsReqState lsReqState;          /* Required state for this ls. */
        Itimer lsItimer;                /* Interval timer structure. */
        hword lsIndex;                  /* Used for u_cep_id construction. */
        word pCepId;                    /* LS id, for connect-(in/out). */
        hwaddrtype hwLsAddr;            /* LS addressing information     */
        byte rSap;                      /* Remote Sap. Actual used. */
        byte xidSent;                   /* a flag to indicate xid sent */
        byte rosrSent;                  /* Ditto for req_opnstn.req. */
        byte retry;                     /* xid 3 retry cound */
        hword debugLevel;               /* Debug level. */
        hword lastErr;                  /* Last error on this conxn. */
        FlowControlE lastFlowRcvd;      /* Ignore next flow-states, if same. */
        hword stopFlowCount;            /* # of 'flow-stops' outstanding, */
                                        /* for partner link stations. */
        LsWorkType workType;            /* Work type bits. */
        byte     numNullXidSent;
        hword workBufCnt;               /* Work buffers used by this LS. */
        WrkBBlk workBBlk;               /* For Sap: work buffer block. */
        } ;

/*---- The following interfaces are exported. */

LinkStn *                               /* Addr of LS created. */
LsCreate (Sibling *childLocP,           /* Ptr to LS child locator. */
          hwaddrtype *hwLsAddrP,        /* LS addressing information */
          byte rSap,                    /* Remote sap. */
          Sap *sapP);                   /* Ptr to SAP ctl block. */

boolean                                 /* Success of LS delete. */
LsDelete (LinkStn *lsP);                /* Addr of LS. */

LinkStn *
LsFind (word ls_id);                    /* LS identifier. */

void
LsReceive (TCLSIMsg* clsiBfrP);         /* Ptr to CLSI buffer. */

void
LsSendData (LinkStn *lsP,               /* Ptr to LS  structure. */ 
            DataReq_t *clsiBfrP);       /* Ptr to DATA.REQ buffer. */

void
LsSapClosed (LinkStn *lsP,          /* Ptr to LS. */
                 Sap* sapP);            /* Ptr to SAP. */

void
LsQueryLsOnSapAndStart (LinkStn *lsP,   /* Ptr to LS. */
                Sap *sapP);             /* Ptr to SAP. */

void
LsLogForWork (LinkStn *lsP,
              LsWorkType workType);

WorkResult
LsDoWork (LinkStn *lsP);                /* Ptr to LS structure. */

hword
LsQueryBufferCount (LinkStn *lsP);      /* Ptr to LS structure. */

void
LsFreeBuffer (LinkStn *lsP);            /* Ptr to LS structure. */

void
LsFlowControlFromClsi (LinkStn *lsP,    /* Ptr to LS structure. */
                       FlowControlE flowCtl); /* Flow control. */

void
LsFlowControlFromPu (LinkStn *lsP,      /* Ptr to LS structure. */
                     FlowControlE flowCtl); /* Flow control. */

void
LsProcessFlowControl (LinkStn *lsP,     /* Ptr to LS structure. */
                      FlowControlE flowCtl); /* Flow control. */

void
LsProcessCmd (LinkStn *lsP,             /* Ptr to ls at work. */
              LsInput lsInput,          /* Type of work ls will do. */
              TCLSIMsg* clsiBfrP);      /* Ptr to clsi buffer. */

void
LsFsm (LinkStn *lsP,                    /* Ptr to LinkStn structure. */
       LsInput lsInput,                 /* Input to LinkStn FSM. */
       TCLSIMsg* clsiBfrP);             /* Ptr to CLSI buffer. */
 

Pu *
LsFindInactivePu (LinkStn* lsP,         
                  XidAddrSapT *xidAddrSapP); /* XID, ADDR, SAP triple */

Pu *
LsFindActivePu (Sibling* lsRootP,      
                XidAddrSapT *xidAddrSapP);  /* XID, ADDR, SAP triple  */

void
LsStartITimer (LinkStn *lsP,
               hword timeInterval);

void DebugLs (LinkStn *lsP, hword   debugLevel);

void NoDebugLs (LinkStn *lsP, hword   debugLevel);

void ShowLs (LinkStn *lsP, hword all);

void LsWrite(LinkStn *lsP);
void LsWriteRsrbStart(LinkStn *lsP);
void LsWriteVdlcStart(LinkStn *lsP);

void LsWriteOut(LinkStn *lsP, AdaptorId* idP);

void LsLuWrite(LinkStn *lsP);


void LsDeletePoolRangeIfMatch (LinkStn *lsP, const char* name);

boolean QueryLsConnOutPuOnSap(Sap *sapP);
boolean QueryLsConnOutPuOnAdaptor(AdaptorId *adaptorIdP);

boolean                        /* TRUE if child pu with pu name existed */
QueryFindChild(LinkStn* lsP,
              const char* puName);
              
/* Sibling* GetPuChildren(LinkStn* lsP){return (Sibling*)&lsP->puChildLoc;}*/
#define GetPuChildren(lsP) ((Sibling*)(&lsP->puChildLoc))

/* Pu* LsChild(LinkStn* lsP) {return GetFirstChild(&lsP->puChildLoc);} */
#define LsChild(lsP) GetFirstChild((Sibling*)(&lsP->puChildLoc))


/* Sap* LsGetSap(LinkStn* lsP) {return lsP->sapP;} */
#define LsGetSap(lsP) (lsP->sapP)

/* hwaddrtype* LsGetAddrP(LinkStn* lsP) */
#define LsGetAddrP(lsP)  (&lsP->hwLsAddr)

/* byte LsGetRemoteSapAddr(LinkStn* lsP) */
#define LsGetRemoteSapAddr(lsP)  (lsP->rSap)

/* byte LsGetLocalSapAddr(LinkStn* lsP) */
#define LsGetLocalSapAddr(lsP)  (LsGetSap(lsP)->sapNumber)

/* boolean LsQueryFlowOff(LinkStn* lsP) */
#define LsQueryFlowOff(lsP) (lsP->lastFlowRcvd==FlwCtl_Stop)

#endif  /* __LS_H. */
/*---- End include file: ls.h. */









