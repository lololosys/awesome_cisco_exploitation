/* $Id: bfrprim.h,v 3.3.54.1 1996/05/17 10:47:02 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/bfrprim.h,v $
 *------------------------------------------------------------------
 * DSPU Bfrprim Utilities.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * Contains exports for bfrprim utilities for handling CLSI buffers.
 *------------------------------------------------------------------
 * $Log: bfrprim.h,v $
 * Revision 3.3.54.1  1996/05/17  10:47:02  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.42.1  1996/03/28  21:07:23  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *   Missed files during VDLC commit
 *
 * Revision 3.3.2.1  1996/01/20  06:08:27  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.3  1995/11/21  00:25:19  rbatz
 * CSCdi40809:  Clean up DLC-level activation sequence for connect-ins.
 *
 * Revision 3.2  1995/11/17  09:04:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  17:02:33  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.1  1995/06/07 20:28:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




#ifndef __BFRPRIM_H                         
#define __BFRPRIM_H


#include "dtypes.h"       /* Defines types word, hword, byte, etc */

#include "../h/address.h"           /* Defines hwaddrtype for clsi_msg.h    */
#include "../cls/clsi_msg.h"
#include "../cls/clsi_queue.h"

#include "dlcparm.h"
#include "dspu_xid.h"


/*---------------------------------------------------------------------------*/
/* DSPU<->CLS LLC Parameter Structure.
 * (Canonical subset of ACTIVATE_SAP.REQ, REQ_OPNSTN.REQ & CONNECT.(REQ,RSP)
 * LLC parameter sub-structures. */

typedef struct LlcParmsT {              /* DSPU<->CLS LLC parameters. */
        byte dMaxOut;                   /* Window size. */
        byte dMaxIn;
        hword dMaxIField;               /* Max I frame. */
        word dT1;                       /* Response timer. */
        word dT2;                       /* ACK timer. */
        word dTi;                       /* Inactivity timer. */
        byte dMaxRetryCount;
        byte dXIDRetries;
        word dXIDTimer;
        byte dExplorerRetries;
        word dExplorerTimer;
        byte dExplorerOption;
        } LlcParmsT;


/*----------------------------------------------------------------------------*/
/* Exported DSPU<->CLS inline functions for speed access. */

/*
 * void * ReturnDataHandle(DataInd_t * bufferP);
 * void   SetDataLength(TCLSIMsg * bufferP,hword len);
 * hword  GetDataLength(TCLSIMsg * bufferP);
 */
 
#define DataOverHead   (offsetof(DataReq_t,fData) - offsetof(DataReq_t,fVersion))
#define ReturnDataHandle(bufferP)  ((bufferP)->fData+(bufferP)->fReserveLength)
#define SetDataLength(bufferP,len) ((bufferP)->fCLSIHdr.fDataSize=len+(DataOverHead+(bufferP)->fReserveLength))
#define GetDataLength(bufferP) ((bufferP)->fCLSIHdr.fDataSize-(DataOverHead+(bufferP)->fReserveLength))

/*
 * word  CLSIMsgGetIdentifier(TCLSIMsg *clsiBfrP);
 * hword CLSIMsgGetRetCode(TCLSIMsg *clsiMsgP);
 * hword CLSIMsgGetPrimitiveCommand(TCLSIMsg *clsiBfrP);
 * hword CLSIMsgGetPrimitiveType(TCLSIMsg *clsiMsgP);
 * hword CLSIMsgGetPrimitiveCode(TCLSIMsg *clsiMsgP);
 */
#define CLSIMsgGetRetCode(clsiMsgP) ((clsiMsgP)->fCLSIHdr.fStatus.fRetCode)
#define CLSIMsgGetIdentifier(clsiBfrP) (clsiBfrP->fIdentifier)
#define CLSIMsgGetPrimitiveCommand(clsiBfrP) \
                          (clsiBfrP->fPrimitiveCode & CLS_COMMAND_MASK) 
#define CLSIMsgGetPrimitiveType(clsiBfrP) \
                          (clsiBfrP->fPrimitiveCode & CLS_TYPE_MASK)
#define CLSIMsgGetPrimitiveCode(clsiBfrP) \
                          (clsiBfrP->fPrimitiveCode)
                                    
/*----------------------------------------------------------------------------*/
/* Exported DSPU<->CLS buffer primitive prototypes. */

TCLSIMsg *
DspuCLSIMsgCreate (BlockingType blkType,
                   size_t size);

#define DspuCLSIMsgDestroy(x) CLSIMsgDestroy((TCLSIMsg*)x)


word
CLSIMsgGetCorrelator (TCLSIMsg *clsiMsgP);

/*============================================================================*/
/* The following procedures are constructors, used to build various signal    */
/* packets for the link station, PU and other layers. There are no equivalent */
/* destructors, since the CLSI layer will return all buffers to the heap.     */
/*============================================================================*/


EnableReq_t *
Cnstrct_EnableReq (AdaptorId *adaptorIdP, hword correlator);

DisableReq_t *
Cnstrct_DisableReq (AdaptorId *adaptorIdP, hword correlator);

ActSapReq_t *
Cnstrct_ActSapReq (word uSapId, AdaptorId *adaptorIdP, byte SapValue);

DeactSapReq_t *
Cnstrct_DeactSapReq (word pSapId,
                     word uSapId);

IdReq_t *
Cnstrct_IdReq (BlockingType blkType,
               word pCepId,
               byte *xidP,
               hword xidLen);

IdRsp_t *
Cnstrct_IdRsp (BlockingType blkType,
               word pCepId,
               byte *xidP,
               hword xidLen);

TCLSIMsg *
Cnstrct_IdSigWithNegError (NegErrE negError,
                           word pCepId,
                           TCLSIMsg *clsiMsgP);

DataReq_t *
Cnstrct_DataReq (BlockingType blkType,
                 hword dataLen);

FlowReq_t *
Cnstrct_FlowReq (BlockingType blkType,
                 word pCepId,
                 FlowControlE flowControl);

SignalStnReq_t *
Cnstrct_SigStnReq (word pCepId);

ConnectReq_t *
Cnstrct_ConnectReq (word pCepId,
                    LlcParmsT* llcParmsP);

ConnectRsp_t *
Cnstrct_ConnectRsp (word pCepId,
                    LlcParmsT* llcParmsP);

DisconnectReq_t *
Cnstrct_DisConnectReq (word pCepId);

DisconnectRsp_t *
Cnstrct_DisConnectRsp (word pCepId);

CloseStnReq_t *
Cnstrct_CloseStnReq (word pCepId,
                     word uCepId);

ReqOpnStnReq_t *
Cnstrct_ReqOpnStnReq (BlockingType blkType,
                      word srcId,
                      word destId,
                      hwaddrtype *remoteAddr,
                      byte rSap,
                      LlcParmsT* llcParmsP,
                      byte rifLen,
                      byte* rifP);

/*
ModCepReq_t *
Cnstrct_ModCepReq (word pCepId);
*/

FlowControlE
FlowInd_GetFlowAction (FlowInd_t *flowIndP);

void
IdStnInd_GetAddrSap (IdStnInd_t *idStnIndP, XidAddrSapT *xidAddrSapP);

byte
IdStnInd_GetRif (IdStnInd_t *idStnIndP, byte **rifP);

/* These 3 functions are replaced by the overload inline function SetPCepID
void
IdReq_SetPCepID (IdReq_t *idReqP,
                 word pCepId);
void
IdRsp_SetPCepID (IdRsp_t *idRspP,
                 word pCepId);
void
DataReq_SetPCepID (DataReq_t *dataReqP,
                   word pCepId);
*/

#define SetPCepID(p,w) ((p)->fCLSIHdr.fIdentifier = w)

/* These 2 routines are defined as inline overload function GetPCepID
word
IdStnInd_GetPCepID (IdStnInd_t *idStnIndP);
word
ReqOpnStnCnf_GetPCepID (ReqOpnStnCnf_t *reqOpnStnCnfP);
*/

#define GetPCepID(p) ((p)->fCLSIHdr.fPCepId)


void
ReqOpnStnReq_SetPCepID (ReqOpnStnReq_t *reqOpnStnReqP,
                        word pCepId);

/*
void
ModCepReq_SetPCepID (ModCepReq_t *modCepReqP,
                     word pCepId);
*/

/*
word
ActSapCnf_GetPSapID (ActSapCnf_t *actSapCnfP);
*/
#define ActSapCnf_GetPSapID(p) ((p)->fCLSIHdr.fPSapId)


/*=====================================================================*
* Indication2Request
*======================================================================*
*
* Description:
*   Used to convert the Indication data packet received into a Request
* data packet.
*
**/
DataReq_t * Indication2Request(DataInd_t * buffer);

#endif  /* __BFRPRIM_H. */
/*---- End include file: bfrprim.h. */

