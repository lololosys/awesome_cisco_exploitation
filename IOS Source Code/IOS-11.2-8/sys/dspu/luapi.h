/* $Id: luapi.h,v 3.1.4.2 1996/08/29 02:43:44 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/luapi.h,v $
 *------------------------------------------------------------------
 * LU API - LU application interface.
 *
 * Sept 1995, Lesley Cox
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the function definitions for
 * internal router applications to access the LU interface.
 * The only application at this stage is the Response Time
 * Reporter and the LUAPI is tailored to this.
 *------------------------------------------------------------------
 * $Log: luapi.h,v $
 * Revision 3.1.4.2  1996/08/29  02:43:44  lcox
 * CSCdi67526:  remove old edism logic from luapi and style cleanup
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/05/17  10:48:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/22  18:50:43  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1996/03/20  17:54:33  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LUAPI_H__
#define __LUAPI_H__

#include "types.h"
#include "dtypes.h"


/*
 * LuApiReturnCode
 *   Return code values for lu api functions.
 *   
 */
typedef enum LuApiReturnCode {
  Success,               /* call completed successfully                */
  InsufficientMemory,    /* resources could not be allocated           */
  InvalidHandle,         /* invalid host name or API handle            */
  HostConnInactive,      /* host connection and/or PU inactive         */
  NoLuAvailable,         /* specified LU already open or if locaddr==0
                            then there was no active LU and one could
                            not be activated via DDDLU feature         */
  HostApplInvalid,       /* requested host application was not
                             available at the host                     */
  BindFailure,           /* BIND with host application failed          */
  FlowClosed,            /* session traffic flow reset (ie no SDT yet) */
  IRMViolation           /* immediate request mode violation by appl   */
  } LuApiReturnCode;


/*
 * TCloseIndication
 *
 *   Definition of type for luClose Indication and
 *   sscpLuClose Indication entry points.
 *   When the indication is called the LU session is no longer
 *   available to the application and the ownership of the buffer
 *   is returned.
 *   
 */
typedef void (*TCloseIndication) (
     void* applLuH, /* application LU Handle */
     byte* bufferP  /* outstanding rcv buffer, if any */
    );


/*
 * TReceiveIndication
 *
 *   Definition of type for luReceive Indication and
 *   sscpLuReceive Indication entry points.
 *   This indication returns control of the buffer to the application.
 *   No API request calls should be made in the indication code.
 *
 *   luReceive Indication:
 *     For dataLen != 0,
 *       the RU data (i.e. without TH or RH) is returned in the buffer
 *       and the dataLen is the length of data that was present in the PIU.
 *       If the destination buffer is too short, the data returned is
 *       truncated to the length provided on the luReceive call.
 *       The last flag when TRUE indicates that this piece represents the
 *       last segment in the last RU in a chain of RUs (- when segmentation
 *       and chaining are not in use on a session this flag will always
 *       return TRUE).
 *     For dataLen == 0,
 *       Note: only negative responses are passed to luReceive Indication.
 *       The buffer is not used, the system and user senses from the
 *       negative response are returned in sysSense and userSense.
 *
 *   sscpLuReceive Indication:
 *     the PIU (i.e. including TH and RH) is returned in the buffer
 *     and the dataLen is the length of the PIU.
 *     If the destination buffer is too short, the data returned is
 *     truncated to the length provided on the sscpLuReceive call.
 *     If the PIU is a negative response, then the system and user senses
 *     from a negative response are also returned in sysSense and
 *     userSense.  Otherwise the sense codes are zero.
 *     The last flag when TRUE indicates that this piece represents the
 *     last segment in the last RU in a chain of RUs (- when segmentation
 *     and chaining are not in use on a session this flag will always
 *     return TRUE).
 *     
 */
typedef void (*TReceiveIndication) (
     void *    applLuH,   /* application LU Handle          */
     byte*     bufferP,   /* rcv buffer                     */
     uint      dataLen,   /* length of received data        */
     boolean   lastFrame, /* EBIU and EC                    */
     hword     sysSense,  /* system sense in a -ve response */
     hword     userSense  /* user sense in a -ve response   */
    );


/*
 * luT0orT2QueryPu request
 *
 *   Check PU definition - check status of PU name that will be used
 *   on a subsequent luT0orT2Open call.
 *
 *   Returns:
 *     Success          = PU is defined and active
 *     HostConnInactive = PU is defined but inactive
 *     InvalidHandle    = PU is not defined  (i.e. not configured)
 */
LuApiReturnCode luT0orT2QueryPu(
     char*            hostName    /* host name on the "sna host" or
                                     "dspu host" command             */
    );


/*
 * luT0orT2Open request
 *
 *   Open function for LU type 0 or type 2 session.
 *   The BIND determines the LU type - either is accepted.
 *
 *   Returns:
 *     Success            = session established and ready for send/rcv
 *     InsufficientMemory = failed to create luapi control blocks
 *     InvalidHandle      = PU is not defined  (i.e. not configured)
 *     HostConnInactive   = PU is defined but inactive
 *     NoLuAvailable      = LU is inactive and cannot be activated or
 *                          LU is in use by another luapi appl or dspu
 *                          or (locaddr==0 case) no free active LU
 *     HostApplInvalid    = rcvd -rsp to INITSELF or NSPE req
 *     BindFailure        = BIND was rejected or unexpected UNBIND
 *
 **/
LuApiReturnCode luT0orT2Open(
     void*            applLuH,    /* application LU Handle           */
     void**           luApiLuH,   /* luapi LU Handle (returned)      */
     char*            hostName,   /* host name on the "sna host" or
                                     "dspu host" command             */
     byte*            locaddr,    /* LU address on host name PU
                                     if passed 0, value returned     */
     uint             applNameLen,/* Length of Host application name */
     byte*            applName,   /* Host application name (EBCDIC)  */
     byte*            modeName,   /* Mode name (EBCDIC, 8 chars)     */
     uint             userDataLen,/* Length of user data             */
     byte*            userData,   /* User data for INITSELF RU       */
     TCloseIndication luCloseInd, /* close indication entry point    */
     hword*           sysSense,   /* system sense returned on failure*/
     hword*           userSense,  /* user sense returned on failure  */
     uint             bindDataLen,/* length of bindData buffer       */
     byte*            bindData,   /* buffer for return of BIND User
                                     data on successful open         */
     uint*            bindDataCnt /* length of BIND User data        */
    );
                          

/*
 * luSend request
 *
 *   Send request on session.
 *   On return the luapi has finished looking at *ruData.
 *
 *   Returns:
 *     Success            = built and forwarded frame
 *     InvalidHandle      = luApiLuH invalid
 *     InsufficientMemory = failed to get buffer to build frame
 *     FlowClosed         = data traffic flow closed by SNA CLEAR req
 *
 */
LuApiReturnCode luSend(
     void*    luApiLuH,  /* luapi LU Handle     */
     uint     ruDataLen, /* Length of RU data   */
     byte*    ruData     /* RU data             */
    );
   

/*
 * luReceive request
 *
 *   receive request on session.
 *   This call passes a receive buffer to the API, which is passed back
 *   on luReceiveIndication, luCloseIndication or return of luClose.
 *   The api will queue a maximum of one receive frame per LU, and then
 *   negatively respond any subsequent frames if an luReceive request
 *   has not been issued.  Therefore, since a receive frame may be
 *   queued when luReceive request is called, the luReceive Indication
 *   may be driven before luReceive returns.
 *
 *   Returns:
 *     Success            = buffer accepted (- to be returned by ind)
 *     InvalidHandle      = luApiLuH invalid
 *
 */
LuApiReturnCode luReceive(
     void*              luApiLuH,  /* luapi LU Handle            */
     uint               rcvBufLen, /* Length of receive buffer   */
     byte*              rcvBuf,    /* receive buffer             */
     TReceiveIndication luRcvInd   /* rcv indication entry point */
    );


/*
 * luReceiveContinue request
 *
 *   continue receive request on session.
 *   This function should only be called from within an luReceive
 *   Indication call for which the lastFrame value passed was FALSE.
 *   Like luReceive Request, this call passes a receive buffer to the
 *   API, which is passed back on luReceiveIndication, luCloseIndication
 *   or luClose return.
 *   Unlike luReceive Request, luReceiveContinue will never call
 *   luReceive Indication.
 *
 *   Returns:
 *     Success            = buffer accepted (- to be returned by ind)
 *     InvalidHandle      = luApiLuH invalid
 *
 */
LuApiReturnCode luReceiveContinue(
     void*              luApiLuH,  /* luapi LU Handle            */
     uint               rcvBufLen, /* Length of receive buffer   */
     byte*              rcvBuf     /* receive buffer             */
    );


/*
 * luClose request
 *
 *   Request session close down.
 *   On return the LU session is no longer available.
 *   This function may block while it completes session take down.
 *   Ownership of any outstanding luReceive buffer is returned.
 *
 *   Returns:
 *     Success            = LU session closed and cleaned up
 *     InvalidHandle      = luApiLuH invalid
 *     InsufficientMemory = currently insufficient memory to do clean up
 */
LuApiReturnCode luClose(
     void*  luApiLuH,   /* luapi LU Handle                          */
     byte** bufferP     /* outstanding rcv buffer, if any (returned)*/
    );


/*
 * sscpLuQueryPu request
 *
 *   Check PU definition - check status of PU name that will be used
 *   on a subsequent sscpLuOpen.  Actually equivalent to luT0orT2QueryPu.
 */
#define sscpLuQueryPu luT0orT2QueryPu


/*
 * sscpLuOpen request
 *
 *   Open function for application to access SSCP-LU session.
 *
 *   Returns:
 *     Success            = session established and ready for send/rcv
 *     InsufficientMemory = failed to create luapi control blocks
 *     InvalidHandle      = PU is not defined  (i.e. not configured)
 *     HostConnInactive   = PU is defined but inactive
 *     NoLuAvailable      = LU is inactive and cannot be activated or
 *                          LU is in use by another luapi appl or dspu
 *                          or (locaddr==0 case) no free active LU
 *   
 */
LuApiReturnCode sscpLuOpen(
     void*            applLuH,     /* application LU Handle           */
     void**           sscpLuApiH,  /* sscp-lu api LU Handle (returned)*/
     char*            hostName,    /* host name on the "sna host" or
                                      "dspu host" command             */
     byte*            locaddr,     /* LU address on host name PU
                                      if passed 0, value returned     */
     boolean          denyLuSession, /* disallow luOpen while
                                        sscpLu api is open            */
     TCloseIndication sscpLuCloseInd /* close indication entry point  */
    );


/*
 * sscpLuSend request
 *
 *   Send request on SSCP-LU session.
 *   On return the buffer is returned to the caller.  If a buffer was
 *   not available to copy the data then InsufficientMemory is returned.
 *   
 *   Returns:
 *     Success            = built and forwarded frame
 *     InvalidHandle      = luApiLuH invalid
 *     InsufficientMemory = failed to get buffer to build frame
 *     IRMViolation       = Immediate Request Mode violation
 *
 **/
LuApiReturnCode sscpLuSend(
     void*    sscpLuApiH,  /* sscp-lu api LU Handle    */
     uint     piuDataLen,  /* Length of PIU (TH,RH&RU) */
     byte*    piuData      /* PIU data                 */
    );


/*
 * sscpLuReceive request
 *
 *   receive request on SSCP-LU session.
 *   This call passes a receive buffer to the API, which is passed back
 *   on sscpLuReceive Indication or sscpLuClose Indication.
 *   The api will queue a maximum of one req frame + one rsp frame per LU,
 *   and negatively respond any subsequent frames if an sscpLuReceive
 *   request has not been issued.  Therefore, since a receive frame may be
 *   queued when sscpLuReceive request is called, the sscpLuReceive
 *   Indication may be driven before sscpLuReceive returns.
 *
 *   Returns:
 *     Success            = buffer accepted (- to be returned by ind)
 *     InvalidHandle      = sscpLuApiH invalid
 *
 */
LuApiReturnCode sscpLuReceive(
     void*              sscpLuApiH,  /* sscp-lu api LU Handle       */
     uint               rcvBufLen,   /* Length of receive buffer    */
     byte*              rcvBuf,      /* receive buffer              */
     TReceiveIndication sscpLuRcvInd /* receive indication entry pt */
    );


/*
 * sscpLuReceiveContinue request
 *
 *   receive request on SSCP-LU session.
 *   This call passes a receive buffer to the API, which is passed back
 *   on sscpLuReceive Indication or sscpLuClose Indication.
 *   The api will queue a maximum of one req frame + one rsp frame per LU,
 *   and negatively respond any subsequent frames if an sscpLuReceive
 *   request has not been issued.  Therefore, since a receive frame may be
 *   queued when sscpLuReceive request is called, the sscpLuReceive
 *   Indication may be driven before sscpLuReceive returns.
 *
 *   Returns:
 *     Success            = buffer accepted (- to be returned by ind)
 *     InvalidHandle      = sscpLuApiH invalid
 *
 */
LuApiReturnCode sscpLuReceiveContinue(
     void*              sscpLuApiH,  /* sscp-lu api LU Handle       */
     uint               rcvBufLen,   /* Length of receive buffer    */
     byte*              rcvBuf       /* receive buffer              */
    );


/*
 * sscpLuClose request
 *
 *   Request close down of application access to SSCP-LU session.
 *   On return the SSCP-LU session is no longer available.
 *   This function may block while it completes clean up.
 *   Ownership of any outstanding sscpLuReceive buffer is returned.
 *
 *   Returns:
 *     Success            = access to SSCP-LU session closed
 *     InvalidHandle      = sscpLuApiH invalid
 *     InsufficientMemory = currently insufficient memory to do clean up
 *
 */
LuApiReturnCode sscpLuClose(
     void*  sscpLuApiH,  /* sscp-lu api LU Handle                    */
     byte** bufferP      /* outstanding rcv buffer, if any (returned)*/
    );


#endif /* __LUAPI_H__ */


