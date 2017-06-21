/* $Id: dspu_error.h,v 3.3.22.6 1996/09/10 06:08:37 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_error.h,v $
 *------------------------------------------------------------------
 * DSPU Error Handling.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This includes the calls and enumerations for configuration and
 * trace errors.
 *------------------------------------------------------------------
 * $Log: dspu_error.h,v $
 * Revision 3.3.22.6  1996/09/10  06:08:37  lcox
 * CSCdi68591:  dspu pu matching algorithm behaviour is not clearly defined
 * Branch: California_branch
 *
 * Revision 3.3.22.5  1996/09/10  01:02:49  lcox
 * CSCdi61971:  dspu debug message should give readable string
 * Branch: California_branch
 *
 * Revision 3.3.22.4  1996/08/29  02:43:49  lcox
 * CSCdi67526:  remove old edism logic from luapi and style cleanup
 * Branch: California_branch
 *
 * Revision 3.3.22.3  1996/07/29  21:46:55  snyder
 * CSCdi64412:  declare some arrays const that should be sub_sna_pu
 *              moves 2376 out of data, saves 116 as well
 * Branch: California_branch
 *
 * Revision 3.3.22.2  1996/06/21  02:21:48  slogie
 * CSCdi57722:  DPSU dont notify SSCP that secondary LU is available
 * Branch: California_branch
 * Fixes to ensure that we do not lose any buffers in the activation
 * window.  Plus debug support for the activation window.
 *
 * Revision 3.3.22.1  1996/05/17  10:47:31  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.8.5  1996/04/15  18:25:50  ppearce
 * CSCdi54277:  4500/7500 crashes when debug dspu is enabled
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.8.4  1996/04/10  00:04:49  ppearce
 * CSCdi54114:  DSPU crashes router during deactivation of downstream PUs
 * Branch: IbuMod_Calif_branch
 *   Fix incorrect display in error message
 *
 * Revision 3.3.8.3  1996/04/09  18:37:09  ppearce
 * CSCdi54114:  DSPU crashes router during deactivation of downstream PUs
 * Branch: IbuMod_Calif_branch
 *   Check for null luluTimer before invoking StopItimer()
 *
 * Revision 3.3.8.2  1996/03/28  08:20:23  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.3.8.1  1996/03/22  18:50:18  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.2.4.1  1996/01/20  06:08:39  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2  1995/11/20  16:37:31  ppearce
 * CSCdi43278:  dspu start and sna start commands disappear from config
 *
 * Revision 3.1  1995/11/09  11:28:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/14  06:15:12  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __ERROR_H
#define __ERROR_H

#include "dspu.h"

#define SNA_header  "SNA"
#define DSPU_header "DSPU"

/*---- Dspu error codes, for error handling. */

typedef enum DspuErrorNum {
    NoError,

/*---- Begin configuration error messages. */

    DSPU_TOKEN_Twice,
    ActWindowDefinedTwice,
    MultipleFOCALPOINT,
    DSPU_POOL_HostNameInvalid,
    DSPU_POOL_ConflictHostLuDef,
    DSPU_POOL_InvalidPoolName,
    DSPU_POOL_PoolActiveMember,
    DSPU_LU_PuNameInvalid,
    DSPU_PU_Duplicated,
    DSPU_PU_MacDuplicated,
    DSPU_PU_NoRemoteAddressOrXid,
    DSPU_PU_RemoteAddressAndXid,
    DSPU_PU_InterfaceConflict,
    DSPU_PU_NotInactive,
    DSPU_PU_NameNotFound,
    DSPU_POOL_NameNotFound, 
    DSPU_HOST_InvalidName,
    DSPU_LU_PoolNameInvalid,
    DSPU_LU_HostNameInvalid,
    DSPU_LU_ConflictHostLuDef,
    DSPU_LU_ConflictPuLuDef,
    DSPU_LU_ConflictPoolLuDef,
    DSPU_LU_CommNotFollowsPUComm,
    DSPU_LU_InvalidPOOL_HOSTPara,
    DSPU_LU_PoolActiveMember,
    DSPU_DEFAULT_twice,
    DSPU_DEFAULT_NotConfigured,
    DSPU_LuEndBeforeLuStart,
    DSPU_noAdaptor,
    DSPU_AdaptorNotSupported,
    DSPU_RSRBnotDefined,
    DSPU_RSRBalreadyDefined,
    DSPU_RSRBinvalidLocalRing,
    DSPU_RSRBinvalidTargetRing,
    DSPU_RSRBinvalidMacAddr,
    DSPU_RSRBActivePuLs,
    DSPU_VDLCnotDefined,
    DSPU_VDLCalreadyDefined,
    DSPU_VDLCinvalidRingNum,
    DSPU_VDLCinvalidMacAddr,
    DSPU_VDLCActivePuLs,
    ActWindowNotDefined,
    DSPU_LU_NoName,
    DSPU_LU_PoolNotMatched,
    DSPU_HOST_notInactive,
    ConfigNoMemory,
    ConfigNoMemoryForLu,
    ConfigNoMemoryForDwLu,
    ConfigNoMemToConnAllLus,
    InsufficientLusInPool,
    LuNotConfigured,
    
    EnableDefinedTwice,
    RsrbEnableNoTargetRing,
    RsrbEnableBadRsrbDef,
    RsrbStartFailed,
    RsrbNotEnabled,
    RsrbNoTargetRing,
    VdlcEnableBadVdlcDef,
    VdlcStartFailed,
    VdlcNotEnabled,
    PuNoRemoteAddr,
    ActNotStarted,
    StartBadName,
    StartBadInterface,
    StartAlreadyStarted,
    StartNoSapsEnabled,
    ActivateBadName,
    ActivateAlreadyActivated,
    NoActivateFailed,
    InvalidQllcSubAddr,
    InvalidXID,
    
    SapDefinedTwice,
    SapNotDefined,
    SapActivePuLs,

/*---- Begin Debug/Trace error messages. */

    PuFsmTrace,
    PuFsmChgTrace,
    LuFsmTrace,
    LuFsmChgTrace,
    LsFsmTrace,
    LsFsmChgTrace, 
    IframeTraceNum,
    PuActivatedTrace,
    PuDeactivatedTrace,
    LuActivatedTrace,
    LuDeactivatedTrace,
    LsActivatedTrace,
    LsDeactivatedTrace,
    LsActWindowTrace,

    DspuStartTimerFailed,
    DspuStopTimerFailed,
    DspuCLSIBadReturnCode,

/*---- Begin "dspu debug" display messages. */

    DspuDbgHeader,
    DspuDbgAllOn,
    DspuDbgPuOn1,
    DspuDbgAllOff,
    DspuDbgPuOff,
    DspuDbgPuHeader,
    DspuDbgPuOn2,
    DspuDbgNotReset

} DspuErrorNum;


/* Debug Trace data */

#define TraceActivationTraffic 1
#define TraceFsmChange         2
#define TraceFsm               4
#define TraceIframeTraffic     8

extern const char* const errorMessage[];    /* error message table. */

extern const char* const dspuActTraceMsg[]; /* activation trace message table*/
typedef enum DspuActTraceEnum {
    TraceTypeNone,
    TraceTypeDactlu,
    TraceTypeActlu,
    TraceTypeNotifyAvail,
    TraceTypeNotifyNotAvail,
    TraceTypeUnbind,
    TraceTypeDactpu,
    TraceTypeActpu,
    TraceTypeXidNullReq,
    TraceTypeXidNullRsp,
    TraceTypeConnOut,
    TraceTypeConnIn,
    TraceTypeXid3Req,
    TraceTypeXid3Rsp,
    TraceTypeFree
} DspuActTraceEnum;
extern DspuActTraceEnum dspuActTraceType;

/*---- The following entrypoints are exported. */

static inline void DspuError0 (DspuErrorNum error)
{
   char *dbgHeader = (GetSnaMode() ? SNA_header : DSPU_header);
   printf ("\n%s: %s\n", dbgHeader, errorMessage[error]);
}
 
static inline void DspuError1 (DspuErrorNum error, int param1)
{
   char *dbgHeader = (GetSnaMode() ? SNA_header : DSPU_header);
   printf ("%s: ", dbgHeader);
   printf (errorMessage[error], param1);
   printc('\n');
}

static inline void DspuError2 (DspuErrorNum error, char* p1, char* p2)
{
   char *dbgHeader = (GetSnaMode() ? SNA_header : DSPU_header);
   printf ("%s: ", dbgHeader);
   printf (errorMessage[error], p1, p2);
   printc('\n');
}
 
static inline void DspuErrors (DspuErrorNum error, const unsigned char* param1)
{
   char *dbgHeader = (GetSnaMode() ? SNA_header : DSPU_header);
   printf ("%s: ", dbgHeader);
   printf (errorMessage[error], param1);
   printc('\n');
}


#define DspuDisplayMsg0(msg)           printf(errorMessage[msg])
#define DspuDisplayMsg1(msg,p1)        printf(errorMessage[msg],p1)
#define DspuDisplayMsg2(msg,p1,p2)     printf(errorMessage[msg],p1,p2)
#define DspuDisplayMsg3(msg,p1,p2,p3)  printf(errorMessage[msg],p1,p2,p3)

static inline void DspuFsmTrace(byte debugLevel, 
                                byte errorMsg, 
                                Pu*  puP,
                                byte fsmInput,
                                byte fsmState,
                                byte fsmAction)
                                
{

  char *trcHeader = (IsPuSna(puP) ? SNA_header : DSPU_header);

  if (debugLevel & TraceFsm) {
      buginf(errorMessage[errorMsg], trcHeader, 
               GetPuName(puP), fsmInput, fsmState, fsmAction);
  }

} /* End DspuFsmTrace() */

static inline void DspuFsmTrace2(byte debugLevel, 
                                 byte errorMsg, 
                                 Pu*  puP,
                                 byte luNumber,
                                 byte fsmInput,
                                 byte currentFsmState,
                                 byte newFsmState,
                                 byte fsmAction)
                                
{

  char *trcHeader = (IsPuSna(puP) ? SNA_header : DSPU_header);

  if (debugLevel & TraceFsm) {
      buginf(errorMessage[errorMsg], trcHeader, 
               GetPuName(puP), luNumber, 
               fsmInput, currentFsmState, newFsmState, fsmAction);
  }

} /* End DspuFsmTrace2() */

static inline void DspuFsmChgTrace(byte debugLevel, 
                                   byte errorMsg, 
                                   Pu*  puP,
                                   const char *fsmInput,
                                   const char *currentFsmState,
                                   const char *newFsmState)
                                
{

  char *trcHeader = (IsPuSna(puP) ? SNA_header : DSPU_header);

  if (debugLevel & TraceFsmChange) {
      buginf(errorMessage[errorMsg], trcHeader, GetPuName(puP),
               fsmInput, currentFsmState, newFsmState);
  }

} /* End DspuFsmChgTrace() */

static inline void DspuFsmChgTrace2(byte debugLevel, 
                                    byte errorMsg, 
                                    Pu*  puP,
                                    byte luNumber,
                                    const char *fsmInput,
                                    const char *currentFsmState,
                                    const char *newFsmState)
{

  char *trcHeader = (IsPuSna(puP) ? SNA_header : DSPU_header);

  if (debugLevel & TraceFsmChange) {
      buginf(errorMessage[errorMsg], trcHeader, 
               GetPuName(puP), luNumber, 
               fsmInput, currentFsmState, newFsmState);
  }

} /* End DspuFsmChgTrace2() */

static inline void DspuActivationTrace(byte debugLevel, 
                                       byte errorMsg, 
                                       Pu*  puP,
                                       const char *puName,
                                       byte parm)
                                
{

  char *trcHeader = (IsPuSna(puP) ? SNA_header : DSPU_header);

  if (debugLevel & TraceActivationTraffic) {
      buginf(errorMessage[errorMsg], trcHeader, puName, parm);
  }

} /* End DspuActivationTrace() */

static inline void DspuActivationWindowTrace(byte debugLevel,
                                             byte errorMsg,
                                             Pu*  puP,
                                             hword lsBufs,
                                             hword sapBufs)
{
  char *trcHeader = (IsPuSna(puP) ? SNA_header : DSPU_header);

  if (debugLevel & TraceActivationTraffic) {
      buginf(errorMessage[errorMsg],trcHeader,GetPuName(puP),
             dspuActTraceMsg[dspuActTraceType],lsBufs,sapBufs);
    }
} /* End DspuActivationWindowTrace() */

static inline void DspuIframeTrace(byte  errorMsg, 
                                   char  *dataDirection,
                                   Pu*   puP,
                                   hword dataLength,
                                   byte  *dataBuffer)
                                
{

  char *trcHeader = (IsPuSna(puP) ? SNA_header : DSPU_header);

      buginf(errorMessage[errorMsg], trcHeader, 
               dataDirection, GetPuName(puP), dataLength, dataBuffer);

} /* End DspuIframeTrace() */


/* 
 * DspuDebugErrorMsg()
 *   if debug enabled, output error msg
 *
 */
static inline void DspuDebugErrorMsg (boolean debug, byte errorMsg)
{
   if (debug) {
       char *dbgHeader = (GetSnaMode() ? SNA_header : DSPU_header);
       buginf(errorMessage[errorMsg], dbgHeader);
   }
}


/* 
 * DspuCLSIErrorMsg()
 *   if debug enabled, output CLSI error msg with bad return code
 *
 */
static inline void DspuCLSIErrorMsg (boolean debug, byte errorMsg, hword rc)
{
    if (debug) {
        char *dbgHeader = (GetSnaMode() ? SNA_header : DSPU_header);
        buginf(errorMessage[errorMsg], dbgHeader, CLSReturnCodeToString(rc));

    }
}

#endif  /* __ERROR_H */
/*---- End include file: error.h. */
