/* $Id: bfrprim.c,v 3.4.10.3 1996/08/28 12:41:49 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/bfrprim.c,v $
 *------------------------------------------------------------------
 * DSPU Buffer primitives.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This module contains the methods for processing CLSI Buffers.
 *------------------------------------------------------------------
 * $Log: bfrprim.c,v $
 * Revision 3.4.10.3  1996/08/28  12:41:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.10.2  1996/05/17  10:46:59  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.3  1996/05/03  17:18:06  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.2.2  1996/04/03  14:02:21  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.2.1  1996/03/28  08:20:12  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.4.10.1  1996/03/18  19:33:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.3  1996/03/16  06:40:16  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.20.2  1996/03/07  08:45:16  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/01/24  21:42:45  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/02/26  06:11:51  dmccowan
 * CSCdi49560:  APPN Connection Network over FDDI fails to activate
 * Fill in EnableTRParms_t structure for FDDI.
 * Remove Obsolete EnableFDDIParms_t structure.
 *
 * Revision 3.3.2.1  1996/01/20  06:08:25  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.3  1995/11/21  00:25:17  rbatz
 * CSCdi40809:  Clean up DLC-level activation sequence for connect-ins.
 *
 * Revision 3.2  1995/11/17  09:04:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:27:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:56:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/11  04:20:44  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.4  1995/09/14  06:15:02  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 * Revision 2.3  1995/06/18  21:45:10  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  19:27:50  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.1  1995/06/07  20:28:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#include "master.h"
#include "logger.h"
#include "msg_dspu.c"			/* Not a typo, see logger.h */
#include <string.h>
#include "interface_private.h"

#include "dspu.h"
#include "bfrprim.h"
#include "dmalloc.h"
#include "dspu_util.h"
#include "../if/tring.h"
#include "../if/rif.h"

#define OneSecond 1000


/*=====================================================================*
 * GetClsParmsSize()
 *======================================================================*
 *
 * Description:
 *   Returns size of CLSI parms structures based on DLC type
 *
 **/
static hword GetClsParmsSize(hword clsPrimitiveCode, DspuDLCType_e dlcType) 
{

    hword clsParmSize = 0;

    switch(dlcType) {

        case DSPU_DLC_SDLC:
          if (clsPrimitiveCode == CLS_ENABLE)
            clsParmSize = sizeof(EnableSDLCParms_t);
          else if (clsPrimitiveCode == CLS_ACTIVATE_SAP)
            clsParmSize = sizeof(ActSapSDLCParms_t);
          break;

        case DSPU_DLC_QLLC:
          if (clsPrimitiveCode == CLS_ENABLE)
            clsParmSize = sizeof(EnableQLLCParms_t);
          else if (clsPrimitiveCode == CLS_ACTIVATE_SAP)
            clsParmSize = sizeof(ActSapQLLCParms_t);
          break;

        case DSPU_DLC_FDDI:
        case DSPU_DLC_ETHERNET:
        case DSPU_DLC_TOKEN_RING:
        case DSPU_DLC_CHANNEL:
          if (clsPrimitiveCode == CLS_ENABLE)
            clsParmSize = sizeof(EnableTRParms_t);
          else if (clsPrimitiveCode == CLS_ACTIVATE_SAP)
            clsParmSize = sizeof(ActSapLLCParms_t);
          break;

        case DSPU_DLC_RSRB:
          if (clsPrimitiveCode == CLS_ENABLE)
            clsParmSize = sizeof(EnableRSRBParms_t);
          else if (clsPrimitiveCode == CLS_ACTIVATE_SAP)
            clsParmSize = sizeof(ActSapLLCParms_t);
          break;

        case DSPU_DLC_VDLC:
          if (clsPrimitiveCode == CLS_ENABLE)
            clsParmSize = sizeof(EnableVDLCParms_t);
          else if (clsPrimitiveCode == CLS_ACTIVATE_SAP)
            clsParmSize = sizeof(ActSapVDLCParms_t);
          break;

        case DSPU_DLC_NCIA:
          if (clsPrimitiveCode == CLS_ENABLE)
            clsParmSize = sizeof(EnableNCIAParms_t);
          else if (clsPrimitiveCode == CLS_ACTIVATE_SAP)
            clsParmSize = sizeof(ActSapNCIAParms_t);
          break;

        case DSPU_DLC_FRAME_RELAY:
          if (clsPrimitiveCode == CLS_ENABLE)
            clsParmSize = sizeof(EnableFRParms_t);
          else if (clsPrimitiveCode == CLS_ACTIVATE_SAP)
            clsParmSize = sizeof(ActSapLLCParms_t);
          break;

        case DSPU_DLC_UNDEFINED:
        default:
          break;

    } /* End switch(dlcType) */

    return(clsParmSize);

} /* End GetClsParmSize() */ 


/*=====================================================================*
 * GetClsDlcType()
 *======================================================================*
 *
 * Description:
 *   Returns CLSDLCType_e corresponding to given DspuDLCType_e
 *
 **/
static CLSDLCType_e GetClsDlcType(DspuDLCType_e dlcType) 
{

    switch(dlcType) {

        case DSPU_DLC_SDLC:
            return(CLS_SDLC);

        case DSPU_DLC_QLLC:
            return(CLS_QLLC);

        case DSPU_DLC_FDDI:
        case DSPU_DLC_ETHERNET:
        case DSPU_DLC_TOKEN_RING:
        case DSPU_DLC_CHANNEL:
            return(CLS_LLC);

        case DSPU_DLC_RSRB:
            return(CLS_RSRB);

        case DSPU_DLC_VDLC:
            return(CLS_VDLC);

        case DSPU_DLC_NCIA:
            return(CLS_NCIA);

        case DSPU_DLC_FRAME_RELAY:
            return(CLS_FRAME_RELAY);

        case DSPU_DLC_UNDEFINED:
        default:
            return(DSPU_DLC_UNDEFINED);

    } /* End switch(dlcType) */

} /* End GetClsDlcType() */ 


/*=====================================================================*
 * DspuCLSIMsgCreate.
 *======================================================================*
 *
 * Description:
 *   Interlude function to CLSIMsgCreate to obtain a CLSI message buffer.
 * It performs either a "non-blocking" create buffer (which returns
 * immediately on failure), or a "blocking" create buffer (which waits
 * until a buffer can be created).
 *
 **/

TCLSIMsg*
DspuCLSIMsgCreate(BlockingType blkType,		/* Block waiting for free buffer? */
		  size_t size)
{

  TCLSIMsg* clsiMsgP = CLSIMsgCreate(size);

  if ((blkType == BLOCKING) && (!clsiMsgP))
    return BlockForBuffer(size);

  return clsiMsgP;

}  /* DspuCLSIMsgCreate. */


/*=====================================================================*
 * CLSIMsgGetCorrelator: 
 *======================================================================*
 *
 * Description:
 *   Retrieve the correlator field from the signal structure.
 *
 **/ 

word
CLSIMsgGetCorrelator(TCLSIMsg * clsiMsgP)
{

   word correlator;


   switch (CLSIMsgGetPrimitiveCode(clsiMsgP)) {

    case CLS_ENABLE | CLS_CONFIRM:
      correlator = ((EnableCnf_t *) clsiMsgP)->fCorrelator;
      break;

    case CLS_DISABLE | CLS_CONFIRM:
      correlator = ((DisableCnf_t *) clsiMsgP)->fCorrelator;
      break;

    default:
      correlator = -1;
      break;
   }
   return (correlator);

} /* CLSIMsgGetCorrelator. */


/*=====================================================================*
 * Cnstrct_EnableReq: 
 *======================================================================*
 *
 * Description:
 *   Construct ENABLE.REQ packet.
 *
 **/

EnableReq_t *
Cnstrct_EnableReq(AdaptorId *adaptorIdP, hword correlator)
{

  hword enableReqSize;
  EnableReq_t *enableReqP;


  enableReqSize = offsetof(EnableReq_t, fParms) 
                    + GetClsParmsSize(CLS_ENABLE, adaptorIdP->dlcType);

  enableReqP = (EnableReq_t *) DspuCLSIMsgCreate(BLOCKING, enableReqSize);
  if (enableReqP) {
    enableReqP->fCLSIHdr.fHeaderLength = CLS_ENABLE_REQ_HDR_SZ;
    enableReqP->fCLSIHdr.fPrimitiveCode = CLS_ENABLE | CLS_REQUEST;
    enableReqP->fCLSIHdr.fDataSize = enableReqSize -  CLS_ENABLE_REQ_HDR_SZ ;
    enableReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    enableReqP->fCLSIHdr.fIdentifierType = CLS_ENTITY_MGR;
    enableReqP->fCLSIHdr.fIdentifier = 0;
   
    enableReqP->fCorrelator = correlator;

    enableReqP->fPortId.fTypeInfo.swidb = adaptorIdP->dlcIdb;
    enableReqP->fPortId.type = CLS_P_TYPE_SWIDB;

    enableReqP->fDLCType = GetClsDlcType(adaptorIdP->dlcType);
    switch (adaptorIdP->dlcType) {

    case DSPU_DLC_SDLC:
    {
      EnableSDLCParms_t *sdlcParmP;
      sdlcParmP = (EnableSDLCParms_t *) & (enableReqP->fParms[0]);
      sdlcParmP->fVersion = CLSI_VERSION;
      break;
    }
    case DSPU_DLC_QLLC:
    {
      EnableQLLCParms_t *qllcParmP;
      qllcParmP = (EnableQLLCParms_t *) & (enableReqP->fParms[0]);
      qllcParmP->fVersion = CLSI_VERSION;
      break;
    }
    case DSPU_DLC_ETHERNET:
    case DSPU_DLC_TOKEN_RING:
    case DSPU_DLC_CHANNEL:
    case DSPU_DLC_FDDI:
    {
      EnableTRParms_t   *llcParmP;
      llcParmP = (EnableTRParms_t*) &(enableReqP->fParms[0]);
      llcParmP->fVersion = CLSI_VERSION;
      break;
    }
    case DSPU_DLC_RSRB:
    {
      EnableRSRBParms_t *rsrbParmP;
      rsrbParmP = (EnableRSRBParms_t *) & (enableReqP->fParms[0]);
      rsrbParmP->fVersion = CLSI_VERSION;
      rsrbParmP->fLocalRing    = GetRsrbLocalRing();
      rsrbParmP->fBridgeNumber = GetRsrbBridgeNumber();
      rsrbParmP->fTargetRing   = GetRsrbTargetRing();
      snpa_init(&rsrbParmP->fVMacAddr, STATION_IEEE48, TARGET_UNICAST, 
        (uchar *) GetRsrbVirtualMacAddr());
      rsrbParmP->fMaxIFieldLength = 0;
      rsrbParmP->fRsrbOptionFlags = 0;
      break;
    }
    case DSPU_DLC_VDLC:
    {
      EnableVDLCParms_t *vdlcParmP;
      vdlcParmP = (EnableVDLCParms_t *) & (enableReqP->fParms[0]);
      vdlcParmP->fVersion = CLSI_VERSION;
      vdlcParmP->fRingGroup = GetVdlcRingNum();
      snpa_init(&vdlcParmP->fVMacAddr, STATION_IEEE48, TARGET_UNICAST, 
        (uchar *) GetVdlcVirtualMacAddr());
      vdlcParmP->fMaxIFieldLength = 0;
      vdlcParmP->fVdlcOptionFlags = 0;
      break;
    }

    case DSPU_DLC_NCIA:
    {
      EnableNCIAParms_t *nciaParmP;
      nciaParmP = (EnableNCIAParms_t *) & (enableReqP->fParms[0]);
      bzero(nciaParmP, sizeof (EnableNCIAParms_t));
      nciaParmP->fVersion = CLSI_VERSION;
      nciaParmP->fServerNum = GetNciaServerNum();
      break;
    }

    case DSPU_DLC_FRAME_RELAY:
    {
      EnableFRParms_t   *frParmP;
      frParmP = (EnableFRParms_t*) &(enableReqP->fParms[0]);
      frParmP->fVersion = CLSI_VERSION;
      break;
    }
    default:                    /* Here => programmer error. */
      break;
    }
  }
  return (enableReqP);

} /* Cnstrct_EnableReq() */


/*=====================================================================*
 * Cnstrct_DisableReq: 
 *======================================================================*
 *
 * Description:
 *   Construct DISABLE.REQ packet.
 *
 **/ 

DisableReq_t *
Cnstrct_DisableReq(AdaptorId *adaptorIdP, hword correlator)
{

  DisableReq_t *disableReqP;


  disableReqP = (DisableReq_t *) DspuCLSIMsgCreate(BLOCKING,
						   sizeof(DisableReq_t));
  if (disableReqP) {
    disableReqP->fCLSIHdr.fHeaderLength = CLS_DISABLE_REQ_HDR_SZ;
    disableReqP->fCLSIHdr.fPrimitiveCode = CLS_DISABLE | CLS_REQUEST;
    disableReqP->fCLSIHdr.fDataSize = sizeof(DisableReq_t) - CLS_DISABLE_REQ_HDR_SZ;
    disableReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    disableReqP->fCLSIHdr.fIdentifierType = CLS_ENTITY_MGR;
    disableReqP->fCLSIHdr.fIdentifier = 0;

    disableReqP->fCorrelator = correlator;

    disableReqP->fDLCType = GetClsDlcType(adaptorIdP->dlcType);
    disableReqP->fPortId.fTypeInfo.swidb = adaptorIdP->dlcIdb;
    disableReqP->fPortId.type = CLS_P_TYPE_SWIDB;
  }
  return (disableReqP);

}/* Cnstrct_DisableReq. */


/*=====================================================================*
 * Cnstrct_ActSapReq: 
 *======================================================================*
 *
 * Description:
 *   Construct ACTIVATE_SAP.REQ packet.
 *
 **/ 

ActSapReq_t *
Cnstrct_ActSapReq (word uSapId, AdaptorId *adaptorIdP, byte sapValue) 
{

#define LLC_TYPE_1_AND_2    0x02 

  ActSapReq_t *actSapReqP;
  hword msgSize;


  msgSize = offsetof(ActSapReq_t, fParms[0])
              + GetClsParmsSize(CLS_ACTIVATE_SAP, adaptorIdP->dlcType);

  actSapReqP = (ActSapReq_t *) DspuCLSIMsgCreate(BLOCKING, msgSize);

  if (actSapReqP) {
    actSapReqP->fCLSIHdr.fHeaderLength = CLS_ACT_SAP_REQ_HDR_SZ;
    actSapReqP->fCLSIHdr.fPrimitiveCode = CLS_ACTIVATE_SAP | CLS_REQUEST;
    actSapReqP->fCLSIHdr.fDataSize = msgSize - CLS_ACT_SAP_REQ_HDR_SZ;
    actSapReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    actSapReqP->fCLSIHdr.fIdentifierType = CLS_ENTITY_MGR;
    actSapReqP->fCLSIHdr.fIdentifier = 0;

    actSapReqP->fCLSIHdr.fUSapId = uSapId;

    actSapReqP->fVersion = CLSI_VERSION;
    actSapReqP->fCorrelator = 0;

    actSapReqP->fPortId.fTypeInfo.swidb = adaptorIdP->dlcIdb;
    actSapReqP->fPortId.type = CLS_P_TYPE_SWIDB;

    actSapReqP->fDLCType = GetClsDlcType(adaptorIdP->dlcType);
    switch (adaptorIdP->dlcType) {

    case DSPU_DLC_SDLC:
    {
       ActSapSDLCParms_t *sdlcParmsP;
       sdlcParmsP = (ActSapSDLCParms_t *) & (actSapReqP->fParms[0]);
       memset(sdlcParmsP, 0, sizeof(ActSapSDLCParms_t));
       sdlcParmsP->fSapValue = sapValue;
       sdlcParmsP->fAddrType = STATION_SDLC;
       break;
    }
    case DSPU_DLC_QLLC:
    {
       char asciiSapAddr[3];
       ActSapQLLCParms_t *qllcParmsP;
       qllcParmsP = (ActSapQLLCParms_t *) & (actSapReqP->fParms[0]);
       memset(qllcParmsP, 0, sizeof(ActSapQLLCParms_t));
       QllcSubAddrToAscii(sapValue, asciiSapAddr);
       memcpy(qllcParmsP->fQActSap.fDispatch.fQSapSubaddress, 
                asciiSapAddr, strlen(asciiSapAddr));
       break;
    }
    case DSPU_DLC_ETHERNET:
    case DSPU_DLC_TOKEN_RING:
    case DSPU_DLC_RSRB:
    case DSPU_DLC_FRAME_RELAY:
    case DSPU_DLC_FDDI:
    case DSPU_DLC_CHANNEL:
    {
       ActSapLLCParms_t *llcParmsP;
       llcParmsP = (ActSapLLCParms_t *) & (actSapReqP->fParms[0]);
       memset(llcParmsP, 0, sizeof(ActSapLLCParms_t));
       llcParmsP->fUserClass = LLC_TYPE_1_AND_2;
       llcParmsP->fSapValue  = sapValue;
       break;
    }
    case DSPU_DLC_VDLC:
    {
       ActSapVDLCParms_t *vdlcParmsP;
       vdlcParmsP = (ActSapVDLCParms_t *) & (actSapReqP->fParms[0]);
       memset(vdlcParmsP, 0, sizeof(ActSapVDLCParms_t));
       vdlcParmsP->fUserClass = LLC_TYPE_1_AND_2;
       vdlcParmsP->fSapValue  = sapValue;
       break;
    }

    case DSPU_DLC_NCIA:
    {
       ActSapNCIAParms_t *nciaParmsP;
       nciaParmsP = (ActSapNCIAParms_t *) & (actSapReqP->fParms[0]);
       memset(nciaParmsP, 0, sizeof(ActSapNCIAParms_t));
       nciaParmsP->fUserClass = LLC_TYPE_1_AND_2;
       nciaParmsP->fSapValue  = sapValue;
       break;
    }

    default:                    /* Here => programmer error. */
      break;
     }
  }
  return (actSapReqP);

}/* Cnstrct_ActSapReq() */


/*=====================================================================*
 * Cnstrct_DeactSapReq: 
 *======================================================================*
 *
 * Description:
 *   Construct DEACTIVATE_SAP.REQ packet.
 *
 **/ 

DeactSapReq_t *
Cnstrct_DeactSapReq(word pSapId,
		    word uSapId)
{

  DeactSapReq_t *deactSapReqP;


  deactSapReqP = (DeactSapReq_t *) DspuCLSIMsgCreate(BLOCKING,
						     sizeof(DeactSapReq_t));
  if (deactSapReqP) {
    deactSapReqP->fCLSIHdr.fHeaderLength = CLS_DEACT_SAP_REQ_HDR_SZ;
    deactSapReqP->fCLSIHdr.fPrimitiveCode = CLS_DEACTIVATE_SAP | CLS_REQUEST;
    deactSapReqP->fCLSIHdr.fDataSize = sizeof(DeactSapReq_t) -
      CLS_DEACT_SAP_REQ_HDR_SZ;
    deactSapReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    deactSapReqP->fCLSIHdr.fIdentifierType = CLS_P_SAP_ID;
    deactSapReqP->fCLSIHdr.fIdentifier = pSapId;

    deactSapReqP->fCLSIHdr.fUSapId = uSapId;

    deactSapReqP->fVersion = CLSI_VERSION;
    deactSapReqP->fCorrelator = 0;
  }
  return (deactSapReqP);

}/* Cnstrct_DeactSapReq. */


/*=====================================================================*
 * Cnstrct_IdReq: 
 *======================================================================*
 *
 * Description:
 *   Construct ID.REQ packet.
 *
 **/ 

IdReq_t *
Cnstrct_IdReq(BlockingType blkType,
	      word pCepId,
	      byte * xidP,
	      hword xidLen)
{

  size_t bufSize;
  IdReq_t *idReqP;


  bufSize = offsetof(IdReq_t, fXID[0]) + xidLen;
  idReqP = (IdReq_t *) DspuCLSIMsgCreate(blkType, bufSize);
  if (idReqP) {
    idReqP->fCLSIHdr.fHeaderLength = CLS_ID_REQ_HDR_SZ;
    idReqP->fCLSIHdr.fPrimitiveCode = CLS_ID | CLS_REQUEST;
    idReqP->fCLSIHdr.fDataSize = bufSize - CLS_ID_REQ_HDR_SZ;
    idReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    idReqP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;
    idReqP->fCLSIHdr.fIdentifier = pCepId;

    idReqP->fVersion = CLSI_VERSION;
    idReqP->fCorrelator = 0;
    idReqP->fReserveLength = 0;

    memcpy(&(idReqP->fXID[0]), xidP, xidLen);
  }
  return (idReqP);

}/* Cnstrct_IdReq. */


/*=====================================================================*
 * Cnstrct_IdRsp: 
 *======================================================================*
 *
 * Description:
 *   Construct ID.RSP packet.
 *
 **/ 


IdRsp_t *
Cnstrct_IdRsp(BlockingType blkType,
	      word pCepId,
	      byte * xidP,
	      hword xidLen)
{

  size_t bufSize;
  IdRsp_t *idRspP;


  bufSize = offsetof(IdRsp_t, fXID[0]) + xidLen;
  idRspP = (IdRsp_t *) DspuCLSIMsgCreate(blkType, bufSize);
  if (idRspP) {
    idRspP->fCLSIHdr.fHeaderLength = CLS_ID_RSP_HDR_SZ;
    idRspP->fCLSIHdr.fPrimitiveCode = CLS_ID | CLS_RESPONSE;
    idRspP->fCLSIHdr.fDataSize = bufSize - CLS_ID_RSP_HDR_SZ;
    idRspP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    idRspP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;
    idRspP->fCLSIHdr.fIdentifier = pCepId;

    idRspP->fVersion = CLSI_VERSION;
    idRspP->fCorrelator = 0;
    idRspP->fReserveLength = 0;

    memcpy(&(idRspP->fXID[0]), xidP, xidLen);
  }
  return (idRspP);

} /* Cnstrct_IdRsp. */ 


TCLSIMsg *
Cnstrct_IdSigWithNegError(NegErrE negError,
			  word pCepId,
			  TCLSIMsg * clsiMsgP)
{

   byte *xidP;
   hword xidLn;
   byte *newXidP;
   hword newXidLn;
   TCLSIMsg *idSigP;


   idSigP = 0;
   XidGetXidData(clsiMsgP, &xidP, &xidLn);
   newXidLn = xidLn + sizeof(XidCtlVec22T);
   newXidP = DspuMalloc(newXidLn);
   if (newXidP) {
      memcpy(newXidP, xidP, xidLn); /* Copy in existing XID. */
      newXidP[1] += sizeof(XidCtlVec22T); /* Update XID length. */
      switch (negError) {	/* Insert appropriate neg error. */
       case Xid_NP_NoActpuCap:
	 XidCtlVec22_Updt((XidCtlVec22T *) & newXidP[xidLn],
			  XID_ACTPU_BYTE, XID_ACTPU_BIT);
	 break;
       case Xid_NP_BadWindowSize:
	 XidCtlVec22_Updt((XidCtlVec22T *) & newXidP[xidLn],
			  XID_WINDOWSIZE_BYTE, XID_WINDOWSIZE_BIT);
	 break;
       case Xid_NP_BadMaxIFrame:
	 XidCtlVec22_Updt((XidCtlVec22T *) & newXidP[xidLn],
			  XID_MAXIFRAME_BYTE, XID_MAXIFRAME_BIT);
	 break;
       default:
	 break;
      } 
      if (CLSIMsgIsIndication(clsiMsgP)) { /* Reply with Response. */
	 idSigP = (TCLSIMsg *) Cnstrct_IdRsp(BLOCKING, pCepId,
					     newXidP, newXidLn);
      }
      else {			/* Confirm: reply with request. */
	 idSigP = (TCLSIMsg *) Cnstrct_IdReq(BLOCKING, pCepId,
                                             newXidP, newXidLn);
      } DspuFree(newXidP);
   }
   return (idSigP);

} /* Cnstrct_IdSigWithNegError. */


/*=====================================================================*
 * Cnstrct_DataReq: 
 *======================================================================*
 *
 * Description:
 *   Construct DATA.REQ packet.
 *
 **/ 


DataReq_t *
Cnstrct_DataReq(BlockingType blkType,
		hword dataLen)
{

  size_t bufSize;
  DataReq_t *dataReqP;


  bufSize = offsetof(DataReq_t, fData[0]) + dataLen;
  dataReqP = (DataReq_t *) DspuCLSIMsgCreate(blkType, bufSize);
  if (dataReqP) {
    dataReqP->fCLSIHdr.fHeaderLength = CLS_DATA_REQ_HDR_SZ;
    dataReqP->fCLSIHdr.fPrimitiveCode = CLS_DATA | CLS_REQUEST;
    dataReqP->fCLSIHdr.fDataSize = bufSize - CLS_DATA_REQ_HDR_SZ;
    dataReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    dataReqP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;
    dataReqP->fCLSIHdr.fIdentifier = 0L;

    dataReqP->fVersion = CLSI_VERSION;
    dataReqP->fCorrelator = 0;
    dataReqP->fReserveLength = 0;
  }
  return (dataReqP);

}/* Cnstrct_DataReq. */


/*=====================================================================*
 * Cnstrct_FlowReq: 
 *======================================================================*
 *
 * Description:
 *   Construct FLOW.REQ packet.
 *
 **/ 


FlowReq_t *
Cnstrct_FlowReq(BlockingType blkType,
		word pCepId,
		FlowControlE flowControl)
{

   FlowReq_t *flowReqP;


   flowReqP = (FlowReq_t *) DspuCLSIMsgCreate(blkType, sizeof(FlowReq_t));
   if (flowReqP) {
      flowReqP->fCLSIHdr.fHeaderLength = CLS_FLOW_REQ_HDR_SZ;
      flowReqP->fCLSIHdr.fPrimitiveCode = CLS_FLOW | CLS_REQUEST;
      flowReqP->fCLSIHdr.fDataSize = sizeof(FlowReq_t) - CLS_FLOW_REQ_HDR_SZ;
      flowReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
      flowReqP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;
      flowReqP->fCLSIHdr.fIdentifier = pCepId;

      switch (flowControl) {
       case FlwCtl_Stop:
	 flowReqP->fCLSIHdr.fFlowAction = CLS_ACTION_STOP_FLOW;
	 break;
       case FlwCtl_Resume:
	 flowReqP->fCLSIHdr.fFlowAction = CLS_ACTION_RESUME_FLOW;
	 break;
       default:
	 break;
      } 
      flowReqP->fVersion = CLSI_VERSION;
      flowReqP->fCorrelator = 0;
   }
   return (flowReqP);

} /* Cnstrct_FlowReq. */


/*=====================================================================*
 * Cnstrct_SigStnReq: 
 *======================================================================*
 *
 * Description:
 *   Construct SIG_STN.REQ packet.
 *
 **/ 


SignalStnReq_t *
Cnstrct_SigStnReq(word pCepId)
{

  SignalStnReq_t *sigStnReqP;


  sigStnReqP = (SignalStnReq_t *) DspuCLSIMsgCreate(BLOCKING,
						    sizeof(SignalStnReq_t));
  if (sigStnReqP) {
    sigStnReqP->fCLSIHdr.fHeaderLength = CLS_SIGNAL_STN_REQ_HDR_SZ;
    sigStnReqP->fCLSIHdr.fPrimitiveCode = CLS_SIGNAL_STN | CLS_REQUEST;
    sigStnReqP->fCLSIHdr.fDataSize = sizeof(SignalStnReq_t) -
      CLS_SIGNAL_STN_REQ_HDR_SZ;
    sigStnReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    sigStnReqP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;
    sigStnReqP->fCLSIHdr.fIdentifier = pCepId;

    sigStnReqP->fCLSIHdr.fActionCode = CLS_ACTION_START;

    sigStnReqP->fVersion = CLSI_VERSION;
    sigStnReqP->fCorrelator = 0;
  }
  return (sigStnReqP);

}/* Cnstrct_SigStnReq. */


/*=====================================================================*
 * Cnstrct_ConnectReq: 
 *======================================================================*
 *
 * Description:
 *   Construct CONNECT.REQ packet.
 *
 **/ 


ConnectReq_t *
Cnstrct_ConnectReq(word pCepId,
                   LlcParmsT* llcParmsP)
{

  ConnectReq_t *connReqP;
  ReqOpnStnLLCParms_t *ROSR_ParmsP;

  size_t connSize = offsetof(ConnectReq_t, fParms[0]) + sizeof(ReqOpnStnLLCParms_t);


  connReqP = (ConnectReq_t *) DspuCLSIMsgCreate(BLOCKING,connSize);

  if (connReqP) {
    connReqP->fCLSIHdr.fHeaderLength = CLS_CONNECT_REQ_HDR_SZ;
    connReqP->fCLSIHdr.fPrimitiveCode = CLS_CONNECT | CLS_REQUEST;
    connReqP->fCLSIHdr.fDataSize = connSize - CLS_CONNECT_REQ_HDR_SZ;
    connReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    connReqP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;
    connReqP->fCLSIHdr.fIdentifier = pCepId;

    connReqP->fCLSIHdr.fModifyFlags = 0;

    if (llcParmsP->dMaxOut > 0)
      connReqP->fCLSIHdr.fModifyFlags |= CLS_MOD_MAXOUT;

    if (llcParmsP->dMaxRetryCount > 0)
      connReqP->fCLSIHdr.fModifyFlags |= CLS_MOD_MAXRETRY;

    if (llcParmsP->dT1 > 0)
      connReqP->fCLSIHdr.fModifyFlags |= CLS_MOD_T1;

    if (llcParmsP->dMaxIField > 0)
      connReqP->fCLSIHdr.fModifyFlags |= CLS_MOD_MAXIFIELD;

    connReqP->fVersion = CLSI_VERSION;
    connReqP->fCorrelator = 0;

    ROSR_ParmsP = (ReqOpnStnLLCParms_t *) & (connReqP->fParms[0]);
    memset (ROSR_ParmsP, 0, sizeof(ReqOpnStnLLCParms_t));
    ROSR_ParmsP->fMaxOut = llcParmsP->dMaxOut;
    ROSR_ParmsP->fMaxIField = llcParmsP->dMaxIField;
    ROSR_ParmsP->fT1 = llcParmsP->dT1;
    ROSR_ParmsP->fMaxRetryCount = llcParmsP->dMaxRetryCount;
  }
  return (connReqP);

}/* Cnstrct_ConnectReq. */


/*=====================================================================*
 * Cnstrct_ConnectRsp: 
 *======================================================================*
 *
 * Description:
 *   Construct CONNECT.RSP packet.
 *
 **/ 


ConnectRsp_t *
Cnstrct_ConnectRsp(word pCepId,
                   LlcParmsT* llcParmsP)

{

  ConnectRsp_t *connRspP;
  ReqOpnStnLLCParms_t *ROSR_ParmsP;

  size_t connSize = offsetof(ConnectRsp_t, fParms[0]) + sizeof(ReqOpnStnLLCParms_t);


  connRspP = (ConnectRsp_t *) DspuCLSIMsgCreate(BLOCKING, connSize);
  if (connRspP) {
    connRspP->fCLSIHdr.fHeaderLength = CLS_CONNECT_RSP_HDR_SZ;
    connRspP->fCLSIHdr.fPrimitiveCode = CLS_CONNECT | CLS_RESPONSE;
    connRspP->fCLSIHdr.fDataSize = connSize - CLS_CONNECT_RSP_HDR_SZ;
    connRspP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    connRspP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;
    connRspP->fCLSIHdr.fIdentifier = pCepId;

    connRspP->fCLSIHdr.fActionCode = CLS_ACTION_CONNECT;

    connRspP->fCLSIHdr.fModifyFlags = 0;

    if (llcParmsP->dMaxOut > 0)
      connRspP->fCLSIHdr.fModifyFlags |= CLS_MOD_MAXOUT;

    if (llcParmsP->dMaxRetryCount > 0)
      connRspP->fCLSIHdr.fModifyFlags |= CLS_MOD_MAXRETRY;

    if (llcParmsP->dT1 > 0)
      connRspP->fCLSIHdr.fModifyFlags |= CLS_MOD_T1;

    if (llcParmsP->dMaxIField > 0)
      connRspP->fCLSIHdr.fModifyFlags |= CLS_MOD_MAXIFIELD;

    connRspP->fVersion = CLSI_VERSION;
    connRspP->fCorrelator = 0;

    ROSR_ParmsP = (ReqOpnStnLLCParms_t *) & (connRspP->fParms[0]);
    memset (ROSR_ParmsP, 0, sizeof(ReqOpnStnLLCParms_t));
    ROSR_ParmsP->fMaxOut = llcParmsP->dMaxOut;
    ROSR_ParmsP->fMaxIField = llcParmsP->dMaxIField;
    ROSR_ParmsP->fT1 = llcParmsP->dT1;
    ROSR_ParmsP->fMaxRetryCount = llcParmsP->dMaxRetryCount;
  }
  return (connRspP);

}/* Cnstrct_ConnectRsp. */


/*=====================================================================*
 * Cnstrct_DisConnectReq: 
 *======================================================================*
 *
 * Description:
 *   Construct DISCONNECT.REQ packet.
 *
 **/ 


DisconnectReq_t *
Cnstrct_DisConnectReq(word pCepId)
{

  DisconnectReq_t *disconnReqP;


  disconnReqP = (DisconnectReq_t *) DspuCLSIMsgCreate(BLOCKING,
						   sizeof(DisconnectReq_t));
  if (disconnReqP) {
    disconnReqP->fCLSIHdr.fHeaderLength = CLS_DISCONNECT_REQ_HDR_SZ;
    disconnReqP->fCLSIHdr.fPrimitiveCode = CLS_DISCONNECT | CLS_REQUEST;
    disconnReqP->fCLSIHdr.fDataSize = sizeof(DisconnectReq_t) -
      CLS_DISCONNECT_REQ_HDR_SZ;
    disconnReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    disconnReqP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;
    disconnReqP->fCLSIHdr.fIdentifier = pCepId;

    disconnReqP->fVersion = CLSI_VERSION;
    disconnReqP->fCorrelator = 0;
  }
  return (disconnReqP);

}/* Cnstrct_DisConnectReq. */


/*=====================================================================*
 * Cnstrct_DisConnectRsp: 
 *======================================================================*
 *
 * Description:
 *   Construct DISCONNECT.RSP packet.
 *
 **/ 

DisconnectRsp_t *
Cnstrct_DisConnectRsp(word pCepId)
{

  DisconnectRsp_t *disConnRspP;


  disConnRspP = (DisconnectRsp_t *) DspuCLSIMsgCreate(BLOCKING,
						   sizeof(DisconnectRsp_t));
  if (disConnRspP) {
    disConnRspP->fCLSIHdr.fHeaderLength = CLS_DISCONNECT_RSP_HDR_SZ;
    disConnRspP->fCLSIHdr.fPrimitiveCode = CLS_DISCONNECT | CLS_RESPONSE;
    disConnRspP->fCLSIHdr.fDataSize = sizeof(DisconnectRsp_t) -
      CLS_DISCONNECT_RSP_HDR_SZ;
    disConnRspP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    disConnRspP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;
    disConnRspP->fCLSIHdr.fIdentifier = pCepId;

    disConnRspP->fCLSIHdr.fActionCode = CLS_ACTION_ACCEPT;

    disConnRspP->fVersion = CLSI_VERSION;
    disConnRspP->fCorrelator = 0;
  } 
  return (disConnRspP);

}/* Cnstrct_DisConnectRsp. */


/*=====================================================================*
 * Cnstrct_CloseStnReq: 
 *======================================================================*
 *
 * Description:
 *   Construct CLOSE_STN.REQ packet.
 *
 **/ 

CloseStnReq_t *
Cnstrct_CloseStnReq(word pCepId,
		    word uCepId)
{

  CloseStnReq_t *closeStnReqP;


  closeStnReqP = (CloseStnReq_t *) DspuCLSIMsgCreate(BLOCKING,
						     sizeof(CloseStnReq_t));
  if (closeStnReqP) {
    closeStnReqP->fCLSIHdr.fHeaderLength = CLS_CLOSE_STN_REQ_HDR_SZ;
    closeStnReqP->fCLSIHdr.fPrimitiveCode = CLS_CLOSE_STN | CLS_REQUEST;
    closeStnReqP->fCLSIHdr.fDataSize = sizeof(CloseStnReq_t) -
      CLS_CLOSE_STN_REQ_HDR_SZ;
    closeStnReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    /* During connect out and before a connect confirm, we dont have a pCep handle
     * ie pCepId == 0, we pass our cep (uCepId) to request to terminate the
     * connect out.
     */
    if (pCepId == 0){
       closeStnReqP->fCLSIHdr.fIdentifierType = CLS_U_CEP_ID;
       closeStnReqP->fCLSIHdr.fIdentifier = uCepId;
    }
    else{
       closeStnReqP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;
       closeStnReqP->fCLSIHdr.fIdentifier = pCepId;
    }
    closeStnReqP->fCLSIHdr.fUCepId = uCepId;
    closeStnReqP->fCLSIHdr.fActionCode = CLS_ACTION_NORMAL;

    closeStnReqP->fVersion = CLSI_VERSION;
    closeStnReqP->fCorrelator = 0;
  } 
  return (closeStnReqP);

}/* Cnstrct_CloseStnReq. */


/*=====================================================================*
 * Cnstrct_ReqOpnStnReq: 
 *======================================================================*
 *
 * Description:
 *   Construct REQ_OPENSTN.REQ packet.
 *
 **/ 

ReqOpnStnReq_t *
Cnstrct_ReqOpnStnReq(BlockingType blkType,
		     word srcId,
		     word destId,
		     hwaddrtype *remoteAddr,
                     byte rSap,
                     LlcParmsT* llcParmsP,
		     byte rifLen,
		     byte * rifP)
{

  size_t bufSize, dataSize;
  ReqOpnStnReq_t *reqOpnStnReqP;


  if ( snpa_get_type(remoteAddr) == STATION_SDLC) {
    bufSize = offsetof(ReqOpnStnReq_t,fParms) 
                + sizeof(ReqOpnStnSDLCParms_t);
  } else if ( snpa_get_type(remoteAddr) == STATION_X121) {
    bufSize = offsetof(ReqOpnStnReq_t,fParms) 
                + sizeof(ReqOpnStnQLLCParms_t);
  } else {
    bufSize = offsetof(ReqOpnStnReq_t,fParms) 
                + offsetof(ReqOpnStnLLCParms_t, fRif) + rifLen;
  }

  reqOpnStnReqP = (ReqOpnStnReq_t *) DspuCLSIMsgCreate(blkType, bufSize);
  if (reqOpnStnReqP) {
    reqOpnStnReqP->fCLSIHdr.fHeaderLength = CLS_REQ_OPNSTN_REQ_HDR_SZ;
    reqOpnStnReqP->fCLSIHdr.fPrimitiveCode = CLS_REQ_OPNSTN | CLS_REQUEST;
    dataSize = bufSize -  CLS_REQ_OPNSTN_REQ_HDR_SZ;
    reqOpnStnReqP->fCLSIHdr.fDataSize = dataSize;
    reqOpnStnReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
    reqOpnStnReqP->fCLSIHdr.fIdentifierType = CLS_P_SAP_ID;

    reqOpnStnReqP->fCLSIHdr.fUCepId = srcId;
    reqOpnStnReqP->fCLSIHdr.fPCepId = destId;
    reqOpnStnReqP->fQueuingType = CLS_QUEUING_DEFAULT;

    switch( snpa_get_type(remoteAddr) ) {

    case STATION_SDLC:
    {
        ReqOpnStnSDLCParms_t *ROSR_SDLCParmsP;
        ROSR_SDLCParmsP = (ReqOpnStnSDLCParms_t *) reqOpnStnReqP->fParms;
        memset(ROSR_SDLCParmsP, 0, sizeof(ReqOpnStnSDLCParms_t));

        snpa_copy(remoteAddr, &ROSR_SDLCParmsP->fDestAddr);

        ROSR_SDLCParmsP->fMaxOut = llcParmsP->dMaxOut;
        ROSR_SDLCParmsP->fMaxIn = llcParmsP->dMaxIn;
        ROSR_SDLCParmsP->fMaxIField = llcParmsP->dMaxIField;
        ROSR_SDLCParmsP->fT1 = llcParmsP->dT1;
        ROSR_SDLCParmsP->fT2 = llcParmsP->dT2;
        ROSR_SDLCParmsP->fTi = llcParmsP->dTi;
        ROSR_SDLCParmsP->fMaxRetryCount = llcParmsP->dMaxRetryCount;
        ROSR_SDLCParmsP->fXIDRetries = llcParmsP->dXIDRetries;
        ROSR_SDLCParmsP->fXIDTimer = llcParmsP->dXIDTimer;
        ROSR_SDLCParmsP->fExplorerOption = EO_VALIDATE_RIF;
        ROSR_SDLCParmsP->fExplorerRetries = llcParmsP->dExplorerRetries;
        ROSR_SDLCParmsP->fExplorerTimer = llcParmsP->dExplorerTimer;
        break;
    }
    case STATION_X121:
    {
        ReqOpnStnQLLCParms_t *ROSR_QLLCParmsP;
        ROSR_QLLCParmsP = (ReqOpnStnQLLCParms_t *) reqOpnStnReqP->fParms;
        memset(ROSR_QLLCParmsP, 0, sizeof(ReqOpnStnQLLCParms_t));

        snpa_copy(remoteAddr, &ROSR_QLLCParmsP->fDestAddr);

        ROSR_QLLCParmsP->fMaxOut = llcParmsP->dMaxOut;
        ROSR_QLLCParmsP->fMaxIn = llcParmsP->dMaxIn;
        ROSR_QLLCParmsP->fMaxIField = llcParmsP->dMaxIField;
        ROSR_QLLCParmsP->fT1 = llcParmsP->dT1;
        ROSR_QLLCParmsP->fT2 = llcParmsP->dT2;
        ROSR_QLLCParmsP->fTi = llcParmsP->dTi;
        ROSR_QLLCParmsP->fMaxRetryCount = llcParmsP->dMaxRetryCount;
        ROSR_QLLCParmsP->fXIDRetries = llcParmsP->dXIDRetries;
        ROSR_QLLCParmsP->fXIDTimer = llcParmsP->dXIDTimer;
        break;
    }
    case STATION_FR10:
    case STATION_IEEE48:
    {
        ReqOpnStnLLCParms_t  *ROSR_LLCParmsP;
        ROSR_LLCParmsP = (ReqOpnStnLLCParms_t *) reqOpnStnReqP->fParms;
        memset(ROSR_LLCParmsP, 0, sizeof(ReqOpnStnLLCParms_t));

        snpa_copy(remoteAddr, &ROSR_LLCParmsP->fDestAddr);

        ROSR_LLCParmsP->fDestSap = rSap;
        ROSR_LLCParmsP->fMaxOut = llcParmsP->dMaxOut;
        ROSR_LLCParmsP->fMaxIn = llcParmsP->dMaxIn;
        ROSR_LLCParmsP->fMaxIField = llcParmsP->dMaxIField;
        ROSR_LLCParmsP->fT1 = llcParmsP->dT1;
        ROSR_LLCParmsP->fT2 = llcParmsP->dT2;
        ROSR_LLCParmsP->fTi = llcParmsP->dTi;
        ROSR_LLCParmsP->fMaxRetryCount = llcParmsP->dMaxRetryCount;
        ROSR_LLCParmsP->fXIDRetries = llcParmsP->dXIDRetries;
        ROSR_LLCParmsP->fXIDTimer = llcParmsP->dXIDTimer;
        ROSR_LLCParmsP->fExplorerOption = llcParmsP->dExplorerOption;
        ROSR_LLCParmsP->fExplorerRetries = llcParmsP->dExplorerRetries;
        ROSR_LLCParmsP->fExplorerTimer = llcParmsP->dExplorerTimer;
        ROSR_LLCParmsP->fDlci = 0; 

        ROSR_LLCParmsP->fRifLength = rifLen;
        if (rifLen)
          memcpy(ROSR_LLCParmsP->fRif, rifP, rifLen);

        /*
         * Specific to Frame Relay/RFC1490 connections
         */
        if (snpa_get_type(remoteAddr) == STATION_FR10)
          ROSR_LLCParmsP->fL3Pid = CLS_L3PID_APPN;

        break;
    }
    default:
        break;

    } /* End switch ( snpa_get_type(remoteAddr) ) */
  }
  return (reqOpnStnReqP);

}  /* Cnstrct_ReqOpnStnReq. */


/*=====================================================================*
 * Cnstrct_ModCepReq: 
 *======================================================================*
 *
 * Description:
 * Construct MODIFY_CEP.REQ packet.
 * **/


/* <<<=== NOTE WELL. <<<=== NOTE WELL.
   ModCepReq_t *
   Cnstrct_ModCepReq (word pCepId)

   {

   ModCepReq_t *modCepReqP;


   modCepReqP = (ModCepReq_t *) DspuCLSIMsgCreate (sizeof(ModCepReq_t));
   if (modCepReqP) {
   modCepReqP->fCLSIHdr.fHeaderLength = CLS_MOD_CEP_REQ_HDR_SZ;
   modCepReqP->fCLSIHdr.fPrimitiveCode = CLS_MODIFY_CEP | CLS_REQUEST;
   modCepReqP->fCLSIHdr.fDataSize = sizeof(ModCepReq_t) - CLS_MOD_CEP_REQ_HDR_SZ;
   modCepReqP->fCLSIHdr.fSelector = CLS_LLC_LAYER;
   modCepReqP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;
   modCepReqP->fCLSIHdr.fIdentifier = pCepId;

   modCepReqP->fVersion = CLSI_VERSION;
   modCepReqP->fCorrelator = 0;
   }

   return (modCepReqP);
   } */

/* Cnstrct_ModCepReq. */


/*=====================================================================*
 * FlowInd_GetFlowAction: 
 *======================================================================*
 *
 * Description:
 *   Get flow action from LSA header in FLOW.IND.
 *
 **/ 

FlowControlE
FlowInd_GetFlowAction(FlowInd_t * flowIndP)
{

  return (flowIndP->fCLSIHdr.fFlowLevel >= DLC_CONGESTION_HIGH) ?
    FlwCtl_Stop : FlwCtl_Resume;

}  /* FlowInd_GetFlowAction. */


/*=====================================================================*
 * IdStnInd_GetAddrSap: 
 *======================================================================*
 *
 * Description:
 *   Returns addr/rSap from ID_STN.IND signal.
 *
 **/ 

void
IdStnInd_GetAddrSap(IdStnInd_t *idStnIndP, XidAddrSapT *xidAddrSapP)
{

  hwaddrtype idStnIndAddr;

  CLSIIdStnGetRemoteMac(idStnIndP, &idStnIndAddr);

  /*
   * Copy addr/rSap from idStnIndP to xidAddrSapP
   */
  snpa_copy(&idStnIndAddr, &xidAddrSapP->addr);
  if (snpa_get_type(&idStnIndAddr) == STATION_X121)
    xidAddrSapP->rsap = NoSap;
  else 
    xidAddrSapP->rsap = idStnIndP->fRSap;

} /* IdStnInd_GetAddrSap() */


/*=====================================================================*

 * IdStnInd_GetRif: 
 *======================================================================*
 *
 * Description:
 *   Return the rif and rif length from the ID_STN.IND signal.
 *
 **/ 

byte
IdStnInd_GetRif(IdStnInd_t * idStnIndP,
		byte ** rifP)
{
  byte rifLength;

  *rifP = CLSIIdStnGetRoute(idStnIndP, &rifLength);
  if (*rifP) { 
      /* 
       * Need to toggle the RIF direction bit like the rest of
       * the CLSI DLUs.
       */    
      srbroute_t *srb_rif = (srbroute_t *) *rifP;
      srb_rif->rc_dlf ^= RC_D;
  }

  return(rifLength);

}/* IdStnInd_GetRif. */


/*=====================================================================*
 * Indication2Request
 *======================================================================*
 *
 * Description:
 *   Used to convert the Indication data packet received into a Request
 * data packet.
 *
 **/ 

DataReq_t *
Indication2Request(DataInd_t * dataIndP)
{
  dataIndP->fCLSIHdr.fPrimitiveCode = CLS_DATA | CLS_REQUEST;
  dataIndP->fCLSIHdr.fIdentifierType = CLS_P_CEP_ID;

  return (DataReq_t *) dataIndP;
}
