/* $Id: dspu_pu.h,v 3.3.6.1 1996/05/17 10:47:58 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_pu.h,v $
 *------------------------------------------------------------------
 * DSPU PU Support.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This defines a common PU structure used by both upstream and downstream
 * PU's.
 *------------------------------------------------------------------
 * $Log: dspu_pu.h,v $
 * Revision 3.3.6.1  1996/05/17  10:47:58  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.14.5  1996/04/24  00:50:10  lcox
 * Branch: IbuMod_Calif_branch
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 3.2.14.4  1996/04/22  00:06:36  lcox
 * CSCdi55112:  mem corruption on ACTPU when NMVT rsp owed downstream
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.14.3  1996/04/09  22:36:36  lmetzger
 * Update of Initial Version of Response Time Reporter
 *
 * Revision 3.2.14.2  1996/03/22  18:50:27  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.2.14.1  1996/03/17  17:38:39  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.3  1996/03/05  03:25:34  bchan
 * CSCdi49448:  DSPU only allows 256 downstream PUs
 *
 * Revision 3.2  1996/01/18  22:32:38  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.1  1995/11/09  11:28:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:03:07  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __PU_H                         
#define __PU_H

#include "../cls/clsi_msg.h"
#include "dtypes.h"
#include "dspuqueu.h"
#include "dspu_cps.h"
#include "dspu_xid.h"


/*----- PU Work Bits -----*/   
#define WORK_LU_UP      0x01 
#define WORK_LU_DOWN    0x08 
#define WORK_DACTPU     0x02 
#define WORK_ACTPU      0x04 
#define WORK_NMVTRSP    0x10

/* ----- PU flags, required state and action */
#define RqdActive     1
#define RqdSendNmvtPsid   2
#define NmvtPsidSent  4      /* nmvt psid had been sent     */
#define RqdPendDelete 8     /* Rqd state = inactive, pending deletion of PU */
#define ConstructedBySna  0x10   /* a pu can be constructed by sna od dspu */
#define PuStartRqsted     0x20   /* pu is inactive, retry later */
#define DddluSupported    0x40   /* actpu recorded DDDLU support */

 struct ConnParmsT {         /* Connection parameters. */
        byte windowSize;            /* DLC frames to buffer. */
        hword maxIFrame;            /* Maximum I frame size. */
        };

 struct RetryInfoT {         /* Connection retry information. */
        byte maxCount;              /* Number of retries. */
        hword maxTimeout;           /* Timeout period before next retry. */
        } ;

typedef byte PuWorkType;

typedef enum { 
          PuActFailRsn_None,
          PuActFailRsn_Other,
          PuActFailRsn_InternalError,
          PuActFailRsn_ConfigError,  
          PuActFailRsn_NegativeRsp, 
          PuActFailRsn_AlreadyActive
        } PuActFailRsnE;  
      
/* activation status for undefined upstream LUs */
typedef struct LuMap {
        word active[8];  /* lu active status bit map */
                         /* 8 x 32 bits to cover 1 ..255 */
                 /* Note: when LUs are defined (that is, configured for
                  * DSPU or LU API functions) then LU child structs and
                  * fsms take over this function
                  */
        } LuMap;

/*=====================================================================*
* Pu structure
*======================================================================*/

struct Pu {                 /* PU structure. */
    
    LinkStn* lsP;                   /* link station pointer */       
    Itimer*  timer;                 /* timer id                 */
    Sibling* puChildLocP;           /* Ptr to pu child locator. */
    Sibling  luChildren;            /* all lu defined in this PU  */ 
    LinkType puType;                /* Upstream, downstream or inactive. */
    PuNameT  name;                  /* Name of PU          */
    hwaddrtype hwPuAddr;            /* PU address info     */
    idbtype  *configuredIdb;        /* PU interface specification        */
    DspuQueueHeader poolRangeHeader;    /* lu defined in this class */
    ConnParmsT cfgConnParms;        /* Configured connection parameters. */
    ConnParmsT negConnParms;        /* Negotiated connection parameters. */
    RetryInfoT retryInfo;           /* Configured connection retry info. */
    hword    sscpPuFsmState;
    hword    puFlag;                /* pu required state and action flag */
    Xid      xid;                   /* Configured Xid               */
    Xid      rcvdXid;               /* Received Xid                 */
    XidFmtE  rcvdXidFmt;            /* Xid format from received Xid */
    word     byteSents;
    word     frameSents;
    word     byteReceives;
    word     frameReceives;
    word     negativeRspSents;      /* Count of -rsp frames sent by Pu      */
    word     negativeRspReceives;   /* Count of -rsp frames received by Pu  */
    word     actFailCnt;            /* Count of Pu activation failures      */
    PuActFailRsnE lastActFailRsn;   /* Reason code for last act failure     */
    boolean  isDefault;             /* is this PU generate from default */
    byte     remoteSap;             /* for matching or connect out, */
    byte     lSap;                  /* for finding path for connect out */
    AdaptorId adaptorId;            /* adaptor that this pu is connected */
    PuWorkType   puWorkBits;
    hword    lastSscpPuSnf;         /* SNF in last req rcvd on SSCP-PU sess */
    byte     debugLevel;
    hword    puIndex;               /* inactive chain */  
    ulong      dspuMibPuIdx;        /* Index into dspuPuTable       */
    struct Pu* dspuMibNextPuP;      /* Next entry in dspuPuTable    */
    sys_timestamp startTime;        /* TimeStamp at Pu activation   */
    sys_timestamp stateChgTime;     /* TimeStamp at Pu state change */
    LuMap*   luMap;                 /* act state of undef LUs (up PU only) */
    byte     luPendingNmvtPsidRsp;  /* LU wanting NMVTPsid rsp(up PU only) */
    
};

/*=====================================================================*
* Downstream Pu specific
*======================================================================*
*/


typedef enum PuCommand{PuResetCmd, PuStopCmd, PuStartCmd}PuCommand;

typedef enum T5State {T5Reset, T5PendingActive, T5Active, T5Busy, 
                      T5PendingInact, T5Max} T5State;
typedef enum T5Input {
   StartPu,      /* Singal from Link station, PuStartCmd */
   ActpuPosRsp,  /* ACTPU positive response from downstream PU */
   ActpuNegRsp,  /* ACTPU negative response from downstream PU */
   DactpuRsp,    /* DACTPU positive or negative response from downstr PU */
   SupRequestFromPu,  /* supported SNA request from downstream PU */
   UnsupRequestFromPu,/* unsupported SNA request from downstream PU */
   RequestToPu,  /* SNA request to downstream PU  */
   ResponseFromPu, /* SNA response from downstrea PU */
   StopPu,      /* Singal from Link station, PuStopCmd */
   ReqDiscon,
   ResetPu,      /* Singal from Link station, PuResetCmd */
   T5Timeout,    /* SSCP_PU timer has timed out */
   MaxT5Input
} T5Input;
        
     


/*=====================================================================*
* Upstream Pu specific
*======================================================================*
*/

typedef enum T2State {T2Reset, T2Active, T2Busy, T2Max} T2State;


typedef enum T2Input {
    T2StartPu,        /* Signal from link station */
    Actpu,            /* ACTPU received from host */
    ActpuDddlu,       /* ACTPU received from host, with the PU Capabilities
                       * Control Vector indicating that the host supports
                       * unsolitited NMVT fro PSID
                       */
    Dactpu,           /* DACTPU received from host */ 
    RequestFromHost,  /* SNA reqeust received from host on the SSCP_PU sess*/
    RequestToHost,    /* An SNA request(probably NMVT) to host */
    ResponseFromHost, /* An SNA response from host */
    T2ResetPu,        /* Hierachical reset from DSPU Control Point */
    ChildrenResetSignal, /* Signal from children inidcating the last child
                         * SSCP_LU session is now reset.
                         */
    SendRequestToHost,/* Signal internal driven when request is sent */
    MaxT2Input
} T2Input;
                     

/*---------------------------------------------------------------------------*/
/* PU Structure. */


Pu* Pu_nw(
     LinkType puType,
     const char*  puName,  /* Host name */
     hword dlci,           /* DLCI number         */
     byte  sdlc,           /* SDLC address        */
     const char*  x25,     /* X.25 address        */
     const char*  rMAC,    /* Remote MAC address  */
     byte  rSap,
     byte  lSap,
     idbtype *idb,
     byte  windowSize,
     hword maxIframe,
     word  xid,             
     byte maxRetryCount,
     hword maxRetryTimeout);
     

Pu* DefaultPu_nw(
     LinkStn* ls,          /* link station parent */
     hwaddrtype* hwPuAddrP,   
     byte  lSap,
     byte  rSap,
     word  xid
     );

/* destructor */
void Pu_dl(Pu* this);

/*=====================================================================*
* ReceivePu
*======================================================================*
*
* Description:
*   This is called by the link station to pass data packet from CLSI
* interface.
*
**/

void ReceivePu(
       Pu*        this,
       DataInd_t* clsiBfrP       /* clsi buffer for data          */
       );
     
boolean QueryMatchPuName(Pu* this, const char* name);
             
IdRsp_t* PuSendXidRsp (Pu *this);
IdReq_t* PuSendXidReq (Pu *this, boolean xid3negotiation);


void T2sscpPuFsm(
             Pu* this, 
             T2Input  fsmInput, 
             DataReq_t* clsiBfrP);
             
void T5sscpPuFsm(
             Pu* this, 
             T5Input  fsmInput, 
             DataReq_t* clsiBfrP);
             
             
void SendPu( 
       Pu*        this,
       DataReq_t* clsiBfrP       /* clsi buffer for data          */
       );
       
void ProcessPuCommand( 
       Pu*        this,
       PuCommand    command
       );


Pu *
ConnectDefaultPu (LinkStn * linkStnP, 
                  XidAddrSapT *xidAddrSapP);

boolean QueryPuSendAlert(Pu* this);

void PuSendNmvt(Pu* this, DataReq_t *clsiBfrP, byte luNumber);

void PuRequestSentNmvt(Pu* this);

void PuReconnectCheck (void);

/* inline function */

#define GetLuChildren(this)  (&this->luChildren)
/* Sibling* GetLuChildren(Pu* this) {return &this->luChildren;} */

#define PuGetPoolRangeHeader(puP) (&puP->poolRangeHeader)

/* DspuQueueHeader* PuGetPoolRangeHeader(Pu* puP) {
 *        return &puP->poolRangeHeader;}
 */

/* in pu or lu where clsi buffer is of type DataIndication but the message
 * to be free is TCLSIMSG type. We typecast here.
 */
#define FreeDataIndBuffer(a)  CLSIMsgDestroy((TCLSIMsg*)a)


#define PuQueryActive(puP) (puP->sscpPuFsmState == T5Active)
/* boolean PuQueryActive (Pu* puP); */

#define PuQueryUpPuActive(puP) (puP->sscpPuFsmState == T2Active)
/* boolean PuQueryUpPuActive (Pu* puP); */

#define PuQueryUpPuInActive(puP) (puP->sscpPuFsmState == T2Reset)
/* boolean PuQueryUpPuInActive (Pu* puP); */

#define PuQueryUpPuBusy(puP)(puP->sscpPuFsmState == T2Busy)
/* boolean PuQueryUpPuBusy (Pu* puP); */

#define PuQueryDirection(puP)      (puP->puType)
/* LinkType PuQueryDirection (Pu* puP); */
       
#define GetPuName(puP)            (puP? (char*)(puP->name.name):"")
/* const char* GetPuName (Pu *puP); */

/* LinkType GetPuType(Pu* puP) */
#define GetPuType(puP)  (puP->puType)

/* Xid GetPuCfgXid(Pu* puP) */
#define GetPuCfgXid(puP)  (puP->xid.xid)

/* idbtype *GetPuConfiguredIdb(Pu* puP) */
#define GetPuConfiguredIdb(puP)  (puP->configuredIdb)

/* byte GetPuCfgWindowSize(Pu* puP) */
#define GetPuCfgWindowSize(puP)  (puP->cfgConnParms.windowSize)

/* hword GetPuCfgMaxIFrame(Pu* puP) */
#define GetPuCfgMaxIFrame(puP)  (puP->cfgConnParms.maxIFrame)

/* XidFmtE GetPuRcvdXidFmt(Pu* puP) */
#define GetPuRcvdXidFmt(puP)  (puP->rcvdXidFmt)

/* void SetPuRcvdXidFmt(Pu* puP, XidFmtE xidFmt) */
#define SetPuRcvdXidFmt(puP, xidFmt)  (puP->rcvdXidFmt = xidFmt)

/* Xid GetPuRcvdXid(Pu* puP) */
#define GetPuRcvdXid(puP)  (puP->rcvdXid.xid)

/* void SetPuRcvdXid(Pu* puP, word xid) */
#define SetPuRcvdXid(puP, xidId)  (puP->rcvdXid.xid = xidId)

static inline void PuLuMapSetLuActive(Pu* puP, byte luNo) {
  puP->luMap->active[luNo>>5] |= (1L << (luNo & 0x1F));
}

static inline void PuLuMapSetLuInactive(Pu* puP, byte luNo) {
  puP->luMap->active[luNo>>5] &= ~(1L << (luNo & 0x1F));
}
   
static inline boolean PuLuMapIsLuActive(Pu* puP, byte luNo) {
  return puP->luMap->active[luNo>>5] & (1L << (luNo & 0x1F));
}

/* boolean PuIsDddluSupported(Pu* puP) */
#define PuIsDddluSupported(puP) ((puP->puFlag & DddluSupported) != 0)

/*  byte GetPuNegWindowSize(Pu* puP) */
#define GetPuNegWindowSize(puP)  (puP->negConnParms.windowSize)

/* hword GetPuNegMaxIFrame(Pu* puP) */
#define GetPuNegMaxIFrame(puP)  (puP->negConnParms.maxIFrame)

/* byte GetPuLocalSapAddr(Pu* puP) */
#define GetPuLocalSapAddr(puP)  (puP->lSap)

/* byte GetPuRemoteSapAddr(Pu* puP) */
#define GetPuRemoteSapAddr(puP)  (puP->remoteSap)

/* hwaddrtype* GetPuAddrP(Pu* puP) */
#define GetPuAddrP(puP)  (&puP->hwPuAddr)

/* AdaptorId* GetPuAdaptorIdP(Pu* puP) */
#define GetPuAdaptorIdP(puP)  (&puP->adaptorId)

/* DlcType_e GetPuDlcType(Pu* puP) */
#define GetPuDlcType(puP)  (GetAdaptorDlcType((&puP->adaptorId)))

/* byte GetPuDlcUnit(Pu* puP) */
#define GetPuDlcUnit(puP)  (GetAdaptorDlcUnit((&puP->adaptorId)))

/* byte GetPuDlcPort(Pu* puP) */
#define GetPuDlcPort(puP)  (GetAdaptorDlcPort((&puP->adaptorId)))

/* byte GetPuMaxRetryCount(Pu* puP) */
#define GetPuMaxRetryCount(puP)  (puP->retryInfo.maxCount)

/* hword GetPuMaxRetryTimeout(Pu* puP) */
#define GetPuMaxRetryTimeout(puP)  (puP->retryInfo.maxTimeout)

/* word GetPuByteSents(Pu* puP) */
#define GetPuByteSents(puP)      (puP->byteSents)

/* word GetPuByteReceives(Pu* puP) */
#define GetPuByteReceives(puP)   (puP->byteReceives)

/* word GetPuFrameSents(Pu* puP) */
#define GetPuFrameSents(puP)     (puP->frameSents)

/* word GetPuFrameReceives(Pu* puP) */
#define GetPuFrameReceives(puP)  (puP->frameReceives)

/* word GetPuNegativeRspSents(Pu* puP) */
#define GetPuNegativeRspSents(puP)  (puP->negativeRspSents)

/* void IncrPuNegativeRspSents(Pu* puP) */
#define IncrPuNegativeRspSents(puP) (puP->negativeRspSents++)

/* word GetPuNegativeRspReceives(Pu* puP) */
#define GetPuNegativeRspReceives(puP)  (puP->negativeRspReceives)

/* void IncrPuNegativeRspReceives(Pu* puP) */
#define IncrPuNegativeRspReceives(puP) (puP->negativeRspReceives++)

/* hword GetPuSscpState(Pu* puP) */
#define GetPuSscpState(puP)  (puP->sscpPuFsmState)

/* LinkStn* GetPuLsP(Pu* puP) */
#define GetPuLsP(puP)  (puP->lsP)

/* LsState GetPuLsState(Pu* puP) */
#define GetPuLsState(puP)  ((puP->lsP)->fsmState)

/* Sibling* GetPuChildLoc(Pu* puP) */
#define GetPuChildLocP(puP)  (puP->puChildLocP)

/* byte GetPuIndex(Pu* puP) */
#define GetPuIndex(puP)  (puP->puIndex)

/* DspuQueueHeader GetPuPoolRangeHdr(Pu* puP) */
#define GetPuPoolRangeHdr(puP)  (puP->poolRangeHeader);

/* boolean IsPuDefault(Pu* puP) */
#define IsPuDefault(puP)  (puP ? puP->isDefault : 0)

/* boolean IsPuSna(Pu* puP) */
#define IsPuSna(puP)  (puP ? (puP->puFlag & ConstructedBySna) : 0)

/* boolean IsPuStarted(Pu* puP) */
#define IsPuStarted(puP)  (GetPuDlcType(puP) != DSPU_DLC_UNDEFINED)

/* boolean IsPuRetryCountInfinite(Pu* puP) */
#define IsPuRetryCountInfinite(puP)  \
    (puP->retryInfo.maxCount == INFINITE_MAX_RETRY_COUNT)

/* sys_timestamp GetPuStartTime(Pu* puP) */
#define GetPuStartTime(puP)  (puP->startTime)

/* void SetPuStartTime(Pu* puP) */
#define SetPuStartTime(puP)  (GET_TIMESTAMP(puP->startTime))

/* sys_timestamp GetPuStateChgTime(Pu* puP) */
#define GetPuStateChgTime(puP)  (puP->stateChgTime)

/* void SetPuStateChgTime(Pu* puP) */
#define SetPuStateChgTime(puP)  (GET_TIMESTAMP(puP->stateChgTime))

/* word GetPuActFailCnt(Pu* puP) */
#define GetPuActFailCnt(puP)  (puP->actFailCnt)

/* void IncrPuActFailCnt(Pu* puP) */
#define IncrPuActFailCnt(puP)  (puP->actFailCnt++)

/* PuActFailRsnE GetPuLastActFailRsn(Pu* puP) */
#define GetPuLastActFailRsn(puP)  (puP->lastActFailRsn)

/* void SetPuLastActFailRsn(Pu* puP, PuActFailRsnE rsncode) */
#define SetPuLastActFailRsn(puP, rsncode) (puP->lastActFailRsn = rsncode)

/* Pu* GetPuDspuMibNextPuP(Pu* puP) */
#define GetPuDspuMibNextPuP(puP)  (puP ? puP->dspuMibNextPuP : 0)

/* void SetPuDspuMibNextPuP(Pu* puP, Pu* nextPuP) */
#define SetPuDspuMibNextPuP(puP, nextPuP)  (puP->dspuMibNextPuP = nextPuP)

/* ulong GetPuDspuMibPuIdx(Pu* puP) */
#define GetPuDspuMibPuIdx(puP)  (puP ? puP->dspuMibPuIdx : 0L)

/* void SetPuDspuMibPuIdx(Pu* puP, ulong idx) */
#define SetPuDspuMibPuIdx(puP, idx)  (puP->dspuMibPuIdx = idx)


#define PuGetParentLs(puP)         (puP->lsP)
/* LinkStn* PuGetParentLs (Pu* puP); */

/* Send data to focalPoint host */
#define SscpPuSend(puP,buf)  T2sscpPuFsm(puP, RequestToHost, buf)

/* Request to send data to focalpoint host */
#define SscpPuRequestToSend(puP) T2sscpPuFsm(puP, SendRequestToHost, 0)

void PuConnectOut(Pu* this, AdaptorId *idP);
void PuNotConnectOut(Pu* this);
void PuActivateDLC(Pu* this);
void PuActivateSDLC(Pu* this);
void PuDeactivate(Pu* this);

/* void PuActivateLLC(Pu* puP) */
#define PuActivateLLC(puP)   (PuActivateDLC(puP))

/* void PuActivateQLLC(Pu* puP) */
#define PuActivateQLLC(puP)  (PuActivateDLC(puP))



/*=====================================================================*
* LogForPuWork
*======================================================================*
*
* Description:
*   This procedure is called whenever the PU wants to send DACTPU, ACTPU,  
* or when the LU wants to send something; because it must ask permission
* of the global buffer management to get a buffer.
*   It turns on the appropriate bit in the PU work bits, and calls 
* LogForLsWork(PUWORK).
*
**/
void LogForPuWork
       (Pu  *puP
       ,PuWorkType puWork
       );

/*=====================================================================*
* DoPuWork
*======================================================================*
*
* Description:
*   This checks the PU work bits in the following order: LU Work, 
* DACTPU, ACTPU. 
*   If the LU work bit is on it calls DoLuWork in all children LUs;
* if WORK_NOT_DONE is returned by all children, then it turns off the
* LU work bit; if WORK_DONE is returned it starts a timer and returns 
* WORK_DONE.
*   If no LU work is done, it then tries to do PU work; if the work 
* bit is on it sends the appropriate command, turns the bit off,
* starts a timer, and returns the enumeration WORK_DONE.
*   If there is no PU work to do this returns the enumeration
* WORK_NOT_DONE. 
*
**/
WorkResult DoPuWork
             (Pu *this
             );

/*=====================================================================*
* PuFreeBuffer
*======================================================================*
*
* Description:
*   This calls FreeLsWorkBuffer in the Link Station parent.
*
**/
void PuFreeBuffer
       (Pu *this
       );


/*=====================================================================*
* FindActivePu
*======================================================================*
*
* Description:
*    Scan through all ls to find a pu with a given name.
**/
Pu* FindActivePu(const char* name, Sibling* root);

#define FindActiveUpPu(name) FindActivePu(name, &dspu.upstrLsRoot)
#define FindActiveDownPu(name) FindActivePu(name, GetDnlsRoot())

/*=====================================================================*
* FindInactivePu
*======================================================================*
*
* Description:
*    Scan through list of inactive pus to find a pu with a given name.
**/
#define FindInactivePu(name,root)\
   QueryAllChildrenParm(root, (ProcQChild1)QueryMatchPuName, (void*)name)

#define FindInactiveUpPu(name) FindInactivePu(name, &dspu.upstrPuRoot)
#define FindInactiveDownPu(name) FindInactivePu(name, GetInactDpuRoot())

#endif  /* __PU_H. */
/*---- End include file: pu.h. */









