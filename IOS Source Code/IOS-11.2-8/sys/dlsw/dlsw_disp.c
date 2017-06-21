/* $Id: dlsw_disp.c,v 3.10.6.7 1996/09/11 04:45:42 kchiu Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_disp.c,v $
 *------------------------------------------------------------------
 * Interface between DLSw subsystem and CLSI
 * DLSw subsystem 
 *
 * Fri July 29, 1994 G. Pandian
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Converts all the DLSw messages to CLSI messages
 *------------------------------------------------------------------
 * $Log: dlsw_disp.c,v $
 * Revision 3.10.6.7  1996/09/11  04:45:42  kchiu
 * CSCdi68756:  DLSw+/fddi is not working
 * Branch: California_branch
 *
 * Revision 3.10.6.6  1996/08/26  15:05:11  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.10.6.5  1996/08/10  01:04:01  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.10.6.4  1996/07/09  05:49:20  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.10.6.3  1996/07/03  22:51:03  ioakley
 * CSCdi61929:  Revision 3.9.6.2 commit prevents large numbers
 *              of sessions from being established with TPNS.
 * Branch: California_branch
 *
 * Revision 3.10.6.2  1996/05/17  10:45:41  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.9.6.12  1996/05/02  22:47:17  kchiu
 * CSCdi56517:  Image build error for c1005 images in IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.6.11  1996/04/26  15:17:17  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.6.10  1996/04/26  07:34:19  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.9.6.9  1996/04/25  04:28:02  ioakley
 * CSCdi55673:  Reduce number of calls on DLSw LAN side and remove
 *              functions that are no longer useful or necessary.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.6.8  1996/04/24  06:30:18  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.9.6.7  1996/04/15  00:08:46  kchiu
 * Branch: IbuMod_Calif_branch
 * Add dlsw+/fddi support
 *
 * Revision 3.9.6.6  1996/04/05  23:11:59  kpatel
 * CSCdi51479:  Memory leak when removing dlsw configuration.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.6.5  1996/04/05  06:06:09  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.9.6.4  1996/04/05  01:21:20  ioakley
 * CSCdi53057:  CreateDiscRsp function using wrong clsi_msg offset.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.9.6.3  1996/04/03  14:01:32  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.9.6.2  1996/03/28  08:20:02  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.9.6.1  1996/03/17  17:38:01  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.10.6.1  1996/03/18  19:31:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.4  1996/03/16  06:39:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.3  1996/03/13  01:20:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/03/07  08:44:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  00:46:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/01  19:01:45  kmoberg
 * CSCdi44347:  SYS-2-SHAREDand tracebacks with DLSw ethernet. LLC2 over
 * tbridge causes a memory leak. Change retbuffer to datagram_done. Force
 * the linktype in DLSw. Don't try to retransmit a pak still in the box.
 *
 * Revision 3.9  1996/02/06  18:30:13  richl
 * CSCdi48241:  buffer leak in dlsw disp_to_lan function
 *
 * Revision 3.8  1996/02/02  22:02:04  pmorton
 * CSCdi47867:  DLSw : Over QLLC broken
 * Back out change for CSCdi44928 that was overwriting QLLC Sap structure.
 *
 * Revision 3.7  1996/01/18  22:24:44  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/15  23:12:47  ioakley
 * CSCdi44928:  Correct "user class" output from "show cls" command.
 *
 * Revision 3.5  1995/12/15  17:18:22  fbordona
 * CSCdi45661:  DLSw+ peers show incorrect circuit count.
 *              Added checking for enable and disable of ports.
 *              Consolidate fsm to disp *stn structures.
 *              Consolidate fsm_to_disp() and cstm_to_disp().
 *              Improve local switching debug.
 *
 * Revision 3.4  1995/12/09  16:49:05  fbordona
 * CSCdi45326:  DLSw+ ignores DISC.Ind in some states
 *              Make DLSw+ call CloseStn.Req with u_cep_id
 *              if ReqOpnStn.Req times out.
 *              Fix crash caused by "dlsw disable".
 *
 * Revision 3.3  1995/11/17  09:03:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:52  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:23:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  20:55:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/24  16:27:08  fbordona
 * CSCdi42216:  %DLSWP-3-PNOMEM: DLSw: No memory to send to group members
 *                Add check for getbuffer failure.
 *                Turn off RII in SSP dataframes.
 *
 * Revision 2.10  1995/10/21  01:24:08  kpatel
 * CSCdi40915:  ACTPU gets trashed in DLSW
 *
 * Revision 2.9  1995/10/11  04:20:36  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.8  1995/09/15  22:29:28  jacobt
 * CSCdi40481:  Memory leak: Last msg:%DLSWP-3-PNOMEM: DLSw: No memory to
 * send pak..
 *
 * Revision 2.7  1995/07/31  18:58:50  ppearce
 * CSCdi37939:  Eliminate use of CLS_P_TYPE_NUMBERS from DLSw+
 *
 * Revision 2.6  1995/07/25  17:40:16  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Incorporate Code reivew comment
 *
 * Revision 2.5  1995/07/19  21:00:52  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add registry service for callback to get QLLC's Activate Ring CLSI
 * message.
 * Move code around for default Activate ring, so that it doesn't fall in
 * the qllc path.
 *
 * Revision 2.4  1995/07/06  18:42:19  ccartee
 * CSCdi36735:  DLSw/EN 80d5 support completely broken
 * Isolate feature 'enable' check and packet transformations into seperate
 * functions.  Use these recycled functions for adding 80d5 functionality
 * to DLSw code.  Duplicate functionality in legacy code is removed and
 * use recycled code instead...
 *
 * Revision 2.3  1995/06/14  18:10:54  ravip
 * CSCdi34577:  dlsw session not established SAPs changed
 *              Handle XIDs to NULL SAP.
 *
 * Revision 2.2  1995/06/09  19:27:29  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.1  1995/06/07  20:26:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "../if/rif.h"
#include "../if/network.h"	/* access to net_bridge_enqueue() for TBRIDGE Tx... */
#include "../if/tring.h"
#include "../ui/debug.h"
#include "../cls/clsi_msg.h"
#include "../cls/dlc_registry.h"
#include "../cls/clsi_queue.h"
#include "../dlsw/dlsw_peer_externs.h" /* peek into peer definitions... */
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_dlu.h"
#include "../srt/srb_core.h"
#include "../llc2/llc2.h"   /* Remove once llc2 public interface is available*/

extern boolean dlsw_core_debug_msgs;

extern boolean dlsw_nornr;

/*
 * Support for IBM 80d5 Ethernet frames...
 */
boolean is_80d5_enabled(paktype *pak, hwidbtype *hwidb);
void xform_802_to_80d5(paktype *pak, ulong transit_oui);
void xform_80d5_to_802(paktype *pak);

/*
 * forward reference
 */
static boolean cls_dlsw_oqueue (hwidbtype *idb, paktype *pak, enum HEADTAIL which);

void fsm_to_disp (union D_primitives *dp)
{
    struct strbuf ctlbuf;
    paktype *pak;

    struct str_core_sndctl sndctlbuf;
    TCLSIMsg *themsg;
    char *str;

    sndctlbuf.msg_type = dp->type;
    ctlbuf.len = sizeof(struct str_core_sndctl);
    ctlbuf.buf = (char *) &sndctlbuf;
    str = string_getnext();

    switch(dp->type){
      case D_ENABLE_REQ:
        themsg = CreateEnableReq((struct D_enable_req *) dp);
        break;

      case D_DISABLE_REQ:
        themsg = CreateDisableReq((struct D_disable_req *) dp);
        break;

      case D_ACTIVATESAP_REQ:
        themsg = CreateActSapReq((struct D_activatesap_req *) dp);
        break;

      case D_ACTIVATERING_REQ:
        themsg = CreateActRingReq((struct D_activatering_req *) dp);
        break;

      case D_DEACTIVATESAP_REQ:
        themsg = CreateDeActSapReq((struct D_deactivatesap_req *) dp);
        break;

      case D_DEACTIVATERING_REQ:
        themsg = CreateDeActRingReq((struct D_deactivatering_req *) dp);
        break;

      case D_MODIFYSAP_REQ:
        /*
           themsg = CreateModifySapReq((struct D_modifysap_req *) dp);
           */
        themsg = NULL;
        break;

      case D_REQ_OPNSTN_REQ:
        themsg = CreateReqOpnStnReq((struct D_reqopenstn_req *) dp);
        break;

      case D_CLOSESTN_REQ:
        themsg = CreateCloseStnReq((struct D_closestn_req *) dp);
        break;

      case D_ID_REQ:
        themsg = CreateIDReq((struct D_id_req *) dp);
        break;

      case D_ID_RSP:
        themsg = CreateIDRsp((struct D_id_rsp *) dp);
        break;
      case D_IDSTN_REQ:
        themsg = CreateIDStnReq((struct D_generic_stn_msg *) dp);
        break;
      case D_IDSTN_RSP:
        themsg = CreateIDStnRsp((struct D_generic_stn_msg *) dp);
        break;
      case D_TESTSTN_REQ:
        themsg = CreateTestStnReq((struct D_generic_stn_msg *) dp);
        break;
      case D_TESTSTN_RSP:
        themsg = CreateTestStnRsp((struct D_generic_stn_msg *) dp);
        break;
      case D_UDATASTN_REQ:
        themsg = CreateUDataStnReq((struct D_generic_stn_msg *) dp);
        break;

      case D_TEST_REQ:
        themsg = CreateTestReq((struct D_test_req *) dp);
        break;

      case D_TEST_RSP:
        themsg = CreateTestRsp((struct D_test_rsp *) dp);
        break;

      case D_CONNECT_REQ:
        themsg = CreateConnectReq((struct D_connect_req *) dp);
        break;

      case D_CONNECT_RSP:
        themsg = CreateConnectRsp((struct D_connect_rsp *) dp);
        break;

      case D_SIGNALSTN_REQ:
        themsg = CreateSignalStnReq((struct D_signalstn_req *) dp);
        break;

      case D_DATA_REQ:
        themsg = CreateDataReq((struct D_data_req *) dp);
        break;

      case D_UDATA_REQ:
        themsg = CreateUDataReq((struct D_udata_req *) dp);
        break;

      case D_FLOW_REQ:
        themsg = CreateFlowReq((struct D_flow_req *) dp);
        break;

      case D_DISCONNECT_REQ:
        themsg = CreateDiscReq((struct D_disconnect_req *) dp);
        break;

      case D_DISCONNECT_RSP:
        themsg = CreateDiscRsp((struct D_disconnect_rsp *) dp);
        break;

      case D_REQ_OPN_REQ:
        /*themsg = CreateReqOpnReq((struct D_reqopen_req *)dp);
         */
        themsg = NULL;
        break;

      default:
        buginf("can not recognize the message\n");
        themsg = NULL;
        break;
    }

    if (themsg == NULL)
        return;

    if (dlsw_core_debug_msgs)
        buginf("\n DISP Sent : %s ",
               CLSIMsgSprintfShort((TCLSIMsg *)themsg, str));
    pak = CLSIMsgGetPak(themsg);
    if (pak) {
        llctype *llc;

        pak->enc_flags |= ENC_DLSW;
        llc =  pak->llc2_cb;
        if (llc)
            llc->enc_flags = pak->enc_flags;
    }
    CLSIQueuePut(gDLSW2CLSWriteQ, (TCLSIMsg*)themsg);

}

TCLSIMsg* CreateEnableReq (struct D_enable_req *dp)
{
    EnableReq_t* enableReq;
    uint  parmSize;
    
    switch (dp->port_type)
    {
    case CLS_TBRIDGE:
	parmSize = sizeof(EnableTBRIDGEParms_t); 
	break;

    case CLS_LLC:		/* Code is broken, CLS_LLC != TKR port!!! */
	parmSize = sizeof(EnableTRParms_t); 
	break;

    case CLS_SDLC: 
	parmSize = sizeof(EnableSDLCParms_t); 
	break;

    case CLS_QLLC:
	parmSize = sizeof(EnableQLLCParms_t);
	break;

    case CLS_VDLC:
	parmSize = sizeof(EnableVDLCParms_t);
	break;

    case CLS_RSRB:
        parmSize = sizeof(EnableRSRBParms_t);
        break;

    case CLS_NCIA:
        parmSize = sizeof(EnableNCIAParms_t);
        break;

    default:
	buginf("\nDLSw: CreateEnableReq, invalid port type (%d)", 
                        dp->port_type);
	return (NULL);
	break;
    }
    
    enableReq = (EnableReq_t*) CLSIMsgCreateHeader(CLS_ENABLE | CLS_REQUEST,
						   offsetof(EnableReq_t,fParms)-
						   CLS_ENABLE_REQ_HDR_SZ +
						   parmSize,
						   CLS_LLC_LAYER,
						   CLS_ENTITY_MGR,
						   0);
    if (enableReq == NULL)
	return NULL;

    enableReq->fDLCType = dp->port_type;  
    enableReq->fPortId.type = CLS_P_TYPE_SWIDB;
    enableReq->fPortId.fTypeInfo.swidb = dp->swidb;
    enableReq->fCorrelator = 0x00;     /* 0xdead; */

    switch (dp->port_type) {
      case CLS_TBRIDGE:
      {
	  static uchar dlsw_tb_vmacaddr[IEEEBYTES] = {0x00,0x00,0x0,0x0,0x0,0x0};
	  EnableTBRIDGEParms_t* tbridgeParms = (EnableTBRIDGEParms_t*) &(enableReq->fParms);


	  tbridgeParms->fVersion = 1;

	  snpa_init(&tbridgeParms->fVMacAddr, STATION_IEEE48, TARGET_UNICAST,
		    dlsw_tb_vmacaddr);

	  tbridgeParms->fTbridgeOptionFlags = 0; /* LACK on/of, etc... */
                                              /* setting is NO-OP for now. */
	  tbridgeParms->fMaxIFieldLength = 0; /* size of payload, not including any headers */
	                                      /* setting is NO-OP for now. */
	  tbridgeParms->fOqueueFn = (void *) &cls_dlsw_oqueue;
	  tbridgeParms->fSpanIndex = dp->span_index;
      }
	  break;
	  
      case CLS_SDLC:
      {				/* Broken, code falsely assumes CLS_LLC == TKR! */
	  EnableSDLCParms_t* sdlcParms = (EnableSDLCParms_t*) &(enableReq->fParms);

	  sdlcParms->fVersion = 1;
      }
	  break;
	  
      case CLS_QLLC:
      {				/* Broken, should use a simpler way to describe port **/
	  EnableQLLCParms_t* qllcParms = (EnableQLLCParms_t*) &(enableReq->fParms);

	  qllcParms->fVersion = 1;
      }
          break;

      case CLS_VDLC:
      {
	  EnableVDLCParms_t 	*vdlcParms;
	  hwidbtype		*hwptr;

	  vdlcParms = (EnableVDLCParms_t*) &(enableReq->fParms);
	  hwptr = hwidb_or_null(dp->swidb);
	  if (hwptr != NULL) {
	      vdlcParms->fRingGroup = srbV.srb_sb_get_targetring(dp->swidb);
	      (*hwptr->extract_hwaddr)(hwptr, &vdlcParms->fVMacAddr);
	      vdlcParms->fVdlcOptionFlags = 0; /* no options set */
	      vdlcParms->fMaxIFieldLength = 0; /* no limits on max packet size */
	  }
	  vdlcParms->fVersion = 1;
      }
          break;

      case CLS_RSRB:
          reg_invoke_dlsw_set_enable_req(dp->port_type, enableReq);
          dp->swidb = NULL;
          break;

      case CLS_NCIA:
          reg_invoke_dlsw_set_enable_req(dp->port_type, enableReq);
          break;

      case CLS_LLC:
      {				/* Broken, code falsely assumes CLS_LLC == TKR! */
	  EnableTRParms_t* trParms = (EnableTRParms_t*) &(enableReq->fParms);

	  trParms->fVersion = 1;

	  /* Use defaults for everything, for now. */
	  snpa_clear(&trParms->fLocalMac);
	  trParms->fTrSpeed = 0;
	  trParms->fMaxIFieldLength = 0;
	  break;
      }
	  
      default:
	  buginf("\nDLSw: Enabling unknown port type %d", 
                        dp->port_type);
      } /* endSwitch Port type */

    return((TCLSIMsg*) enableReq);
}

TCLSIMsg* CreateDisableReq (struct D_disable_req *dp)
{
 DisableReq_t* disableReq;
   
 disableReq = (DisableReq_t*) CLSIMsgCreateHeader(CLS_DISABLE | CLS_REQUEST,
                                           0,
                                           CLS_LLC_LAYER,
                                           CLS_ENTITY_MGR,
                                           0);
 if (disableReq == NULL)
  return NULL;
 
 disableReq->fPortId.type = CLS_P_TYPE_SWIDB;
 disableReq->fPortId.fTypeInfo.swidb = dp->swidb;
 disableReq->fDLCType = 
                    dp->port_type; 
 disableReq->fCorrelator = 0x00;     /* 0xdead; */
 
 /* Use defaults for everything, for now. */
 
 return((TCLSIMsg*) disableReq);
 
}

TCLSIMsg* CreateActSapReq (struct D_activatesap_req *dp)
{
 ActSapReq_t* actSapReq;
 ActSapLLCParms_t* trParms;
    
 actSapReq = (ActSapReq_t*) CLSIMsgCreateHeader(CLS_ACTIVATE_SAP | CLS_REQUEST,
					   offsetof(ActSapReq_t, fParms) -
					   CLS_ACT_SAP_REQ_HDR_SZ +
					   sizeof(ActSapLLCParms_t),
					   CLS_LLC_LAYER,
					   CLS_ENTITY_MGR,
					   0);
  if (actSapReq == NULL)
   return NULL;

  actSapReq->fCLSIHdr.fUSapId = 0x1234;
    
  actSapReq->fVersion = 1;
  actSapReq->fCorrelator = 0x00;      /*0xdead */
  actSapReq->fPortId.type = CLS_P_TYPE_SWIDB;
  actSapReq->fPortId.fTypeInfo.swidb = dp->swidb;
  actSapReq->fDLCType = dp->port_type;    

  trParms = (ActSapLLCParms_t*) &(actSapReq->fParms);
  trParms->fUserClass = 0x02;
  trParms->fSapValue = dp->sap_value;  
  trParms->fT1 = 0;
  trParms->fT2 = 0;
  trParms->fTi = 0;
  trParms->fMaxOut = 0;
  trParms->fMaxRetryCount = 0;		/* no limit */
  trParms->fMaxIn = 0;
  trParms->fMaxIField = 0;
  trParms->fXIDRetries = 1;             /* no XID retry */
  trParms->fXIDTimer = ONESEC;
  trParms->fExplorerRetries = 1;        /* no Explorer retry */
  trParms->fExplorerTimer = ONESEC;
  return((TCLSIMsg*) actSapReq);
}

TCLSIMsg* CreateDeActSapReq (struct D_deactivatesap_req *dp)
{
 DeactSapReq_t* deactSapReq;
   
 deactSapReq = (DeactSapReq_t*) CLSIMsgCreateHeader(CLS_DEACTIVATE_SAP | CLS_REQUEST,
                                           0,
                                           CLS_LLC_LAYER,
                                           CLS_P_SAP_ID,
                                           dp->p_sap_id);
  if (deactSapReq == NULL)
   return NULL;
 
  deactSapReq->fCLSIHdr.fUSapId = 0x1234;
   
  deactSapReq->fVersion = 1;
  deactSapReq->fCorrelator = 0x00;      /*0xdead */
 
  return((TCLSIMsg*) deactSapReq);
}

TCLSIMsg* CreateReqOpnStnReq (struct D_reqopenstn_req *dp)
{
 ReqOpnStnReq_t* reqOpnStnReq;
 ReqOpnStnLLCParms_t* trParms;
    
 reqOpnStnReq = 
	(ReqOpnStnReq_t*) CLSIMsgCreateHeader(CLS_REQ_OPNSTN | CLS_REQUEST,
				      offsetof(ReqOpnStnReq_t, fParms) -
				      CLS_REQ_OPNSTN_REQ_HDR_SZ +
				      sizeof(ReqOpnStnLLCParms_t),
				      CLS_LLC_LAYER,
				      CLS_P_SAP_ID,
                                      dp->p_sap_id);

 if (reqOpnStnReq == NULL)
  return NULL;

 reqOpnStnReq->fCLSIHdr.fUCepId = dp->uCepId;
 reqOpnStnReq->fVersion = 1;
 reqOpnStnReq->fCorrelator = 0xdead;
 reqOpnStnReq->fQueuingType = CLS_QUEUING_DEFAULT;

 trParms = (ReqOpnStnLLCParms_t*) &(reqOpnStnReq->fParms);

 /*
  * If this is an incoming call, these came from the idStn.ind
  */
 trParms->fDestAddr.type = STATION_IEEE48 ;
 trParms->fDestAddr.length = STATIONLEN_IEEE48;
 ieee_copy(dp->remoteAddr, trParms->fDestAddr.addr);
 trParms->fDestSap = dp->rSap;

 trParms->fSrcAddr.type = STATION_IEEE48 ;
 trParms->fSrcAddr.length = STATIONLEN_IEEE48;
 ieee_copy(dp->localAddr, trParms->fSrcAddr.addr); 
 trParms->fSrcSap = dp->lSap;

 trParms->fRifLength = dp->rifLength;
 if (dp->rifLength > 0)
  bcopy(dp->rif, trParms->fRif, dp->rifLength);
 
 trParms->fExplorerRetries = 3;
 trParms->fExplorerTimer = 1000;
 
 /*
  * These are other configuration parameters. Mostly use defaults.
  */
 trParms->fT1 = 0;
 trParms->fT2 = 0;
 trParms->fTi = 0;
 trParms->fMaxOut = 0;
 trParms->fMaxRetryCount = 0;
 trParms->fMaxIn = 0;
/*
 trParms->fWindowOption = 0;
*/

 trParms->fMaxOutIncr = 0;
 trParms->fMaxIField = 0;
 trParms->fXIDRetries = 1; /* do not retry */
 trParms->fXIDTimer = ONESEC;
 trParms->fExplorerRetries = 1;  /* do not retry */
 trParms->fExplorerTimer = ONESEC;
 trParms->fExplorerOption = dp->options;
 trParms->fDlci = 0; 
 trParms->fFlags |= ENC_DLSW;
  
 if (dlsw_nornr)
     trParms->fFlags |= LLC_NO_RNR;

 return((TCLSIMsg*) reqOpnStnReq);
}

TCLSIMsg* CreateIDReq (struct D_id_req *dp)
{
 IdReq_t* idReq;
    
 idReq = (IdReq_t*) CLSIMsgCreateHeader(CLS_ID|CLS_REQUEST,
				   offsetof(IdReq_t, fXID) -
				   CLS_ID_REQ_HDR_SZ +
				   dp->xidLength,
				   CLS_LLC_LAYER,
				   CLS_P_CEP_ID,
				   dp->pCepId);
 if (idReq == NULL)
  return NULL;

 idReq->fVersion = CLSI_VERSION;
 idReq->fCorrelator = 0xdead;
 idReq->fReserveLength = 0;
 bcopy(dp->xidData, &idReq->fXID[0], dp->xidLength);
 return((TCLSIMsg*) idReq);
}

TCLSIMsg* CreateIDRsp (struct D_id_rsp *dp)
{
 IdRsp_t* idRsp;

 idRsp = (IdRsp_t*) CLSIMsgCreateHeader(CLS_ID | CLS_RESPONSE,
				   offsetof(IdRsp_t, fXID) -
				   CLS_ID_RSP_HDR_SZ +
				   dp->xidLength,
				   CLS_LLC_LAYER,
				   CLS_P_CEP_ID,
				   dp->pCepId);
 if (idRsp == NULL)
  return NULL;

 idRsp->fVersion = CLSI_VERSION;
 idRsp->fCorrelator = 0xdead;
 idRsp->fReserveLength = 0;
 bcopy(dp->xidData, &idRsp->fXID[0], dp->xidLength);
 return((TCLSIMsg*) idRsp);
    
}

TCLSIMsg* CreateConnectReq (struct D_connect_req *dp)
{
 ConnectReq_t* connectReq;
 size_t parmSize = 0;		/* For now. Do parameters later */
    
 connectReq = (ConnectReq_t*) CLSIMsgCreateHeader(CLS_CONNECT | CLS_REQUEST,
					     offsetof(ConnectReq_t, fParms) -
					     CLS_CONNECT_REQ_HDR_SZ +
					     parmSize,
					     CLS_LLC_LAYER,
					     CLS_P_CEP_ID,
					     dp->pCepId);
 if (connectReq == NULL)
  return NULL;

 /* we might want to lock the Rif here, this will send out a SABME */
 connectReq->fCLSIHdr.fModifyFlags = 0;
 connectReq->fVersion = CLSI_VERSION;
 connectReq->fCorrelator = 0xdead;

 return((TCLSIMsg*) connectReq);
}

TCLSIMsg* CreateConnectRsp (struct D_connect_rsp *dp)
{
 ConnectRsp_t* connectRsp;
 size_t parmSize = 0;		/* For now. Do parameters later */
    
 connectRsp = (ConnectRsp_t*) CLSIMsgCreateHeader(CLS_CONNECT | CLS_RESPONSE,
					     offsetof(ConnectRsp_t, fParms) -
					     CLS_CONNECT_RSP_HDR_SZ +
					     parmSize,
					     CLS_LLC_LAYER,
					     CLS_P_CEP_ID,
					     dp->pCepId);
 if (connectRsp == NULL)
  return NULL;

 connectRsp->fCLSIHdr.fActionCode = 0;
 connectRsp->fCLSIHdr.fModifyFlags = 0;
 connectRsp->fVersion = CLSI_VERSION;
 connectRsp->fCorrelator = 0xdead;
 connectRsp->fFlag = dp->flag;

 return((TCLSIMsg*) connectRsp);
}

TCLSIMsg* CreateSignalStnReq (struct D_signalstn_req *dp)
{
 SignalStnReq_t* sigStnReq;
    
 sigStnReq = (SignalStnReq_t*)
 CLSIMsgCreateHeader(CLS_SIGNAL_STN | CLS_REQUEST,
		    sizeof(SignalStnReq_t) -
		    CLS_SIGNAL_STN_REQ_HDR_SZ,
		    CLS_LLC_LAYER,
		    CLS_P_CEP_ID,
		    dp->pCepId);
 if (sigStnReq == NULL)
  return NULL;
    
 sigStnReq->fCLSIHdr.fActionCode = 0; /* start data (exit local */
				 /* busy state) */
 sigStnReq->fVersion = CLSI_VERSION;
 sigStnReq->fCorrelator = 0xdead;

 return((TCLSIMsg*) sigStnReq);
}

TCLSIMsg* CreateDataReq (struct D_data_req *dp)
{
 DataReq_t* dataReq;
 hword reservedSize;
    
 dataReq = (DataReq_t*) CLSIMsgCreateFromPak(dp->pak,
                                       CLS_DATA | CLS_REQUEST,
				       CLS_LLC_LAYER,
				       CLS_P_CEP_ID,
				       dp->pCepId,
                                       0,
                                       &reservedSize);
 if (dataReq == NULL)
  return NULL;

 dataReq->fVersion = CLSI_VERSION;
 dataReq->fCorrelator = 0xdead;
 dataReq->fReserveLength = reservedSize;
 return((TCLSIMsg*) dataReq);
}

TCLSIMsg* CreateDiscReq (struct D_disconnect_req *dp)
{
 DisconnectReq_t* disconnectReq;
 size_t parmSize = 0;		/* For now. Do parameters later */
    
 disconnectReq = (DisconnectReq_t*) CLSIMsgCreateHeader(CLS_DISCONNECT | CLS_REQUEST,
			offsetof(DisconnectReq_t, fCorrelator) -
			CLS_DISCONNECT_REQ_HDR_SZ +
			parmSize,
			CLS_LLC_LAYER,
			CLS_P_CEP_ID,
			dp->pCepId);
 if (disconnectReq == NULL)
  return NULL;

 disconnectReq->fVersion = CLSI_VERSION;
 disconnectReq->fCorrelator = 0xdead;

 return((TCLSIMsg*) disconnectReq);
}

TCLSIMsg* CreateDiscRsp (struct D_disconnect_rsp *dp)
{
 DisconnectRsp_t* disconnectRsp;
 size_t parmSize = 0;		/* For now. Do parameters later */
    
 disconnectRsp = (DisconnectRsp_t*) CLSIMsgCreateHeader(CLS_DISCONNECT | CLS_RESPONSE,
				    offsetof(DisconnectRsp_t, fCorrelator) -
				    CLS_DISCONNECT_RSP_HDR_SZ +
				    parmSize,
				    CLS_LLC_LAYER,
				    CLS_P_CEP_ID,
				    dp->pCepId);
 if (disconnectRsp == NULL)
  return NULL;

 disconnectRsp->fVersion = CLSI_VERSION;
 disconnectRsp->fCorrelator = 0xdead;

 return((TCLSIMsg*) disconnectRsp);
}

TCLSIMsg* CreateCloseStnReq (struct D_closestn_req *dp)
{
 CloseStnReq_t* closeStnReq;
    
 closeStnReq = (CloseStnReq_t*) CLSIMsgCreateHeader(CLS_CLOSE_STN | CLS_REQUEST,
				     offsetof(CloseStnReq_t, fCorrelator) -
				     CLS_CLOSE_STN_REQ_HDR_SZ,
				     CLS_LLC_LAYER,
				     dp->identifierType,
				     dp->identifier);
 if (closeStnReq == NULL)
  return NULL;

 closeStnReq->fCLSIHdr.fUCepId = 0x00abcdef;
 closeStnReq->fCLSIHdr.fActionCode = CLS_ACTION_FORCED;
    
 closeStnReq->fVersion = CLSI_VERSION;
 closeStnReq->fCorrelator = 0xdead;

 return((TCLSIMsg*) closeStnReq);
}


/*******************************************************/
/*                        New Messages.                */
/*******************************************************/

/*
 * Activate Ring Request.
 */

TCLSIMsg* CreateActRingReq (struct D_activatering_req *dp)
{
 ActRngReq_t* actringReq;
 
 actringReq = (ActRngReq_t *)
     reg_invoke_dlsw_get_ring_req(dp->port_type, dp->swidb);

 if (actringReq == NULL)
  return NULL;

 actringReq->fCLSIHdr.fUSapId = dp->u_sap_id;

 if (actringReq->fDLCType == CLS_NCIA)
     return((TCLSIMsg*) actringReq);

 actringReq->fLocalRing    = dp->source_ring;
 actringReq->fBridgeNumber = dp->bridge_number;
 actringReq->fTargetRing   = dp->target_ring;
 actringReq->fVersion    = 0;

 actringReq->fDLCType = dp->port_type;  
 actringReq->fPortId.type = CLS_P_TYPE_SWIDB;
 actringReq->fPortId.fTypeInfo.swidb = dp->swidb;

 return((TCLSIMsg*) actringReq);
}

/*
 * Deactivate Ring Request.
 */
TCLSIMsg* CreateDeActRingReq (struct D_deactivatering_req *dp)
{
 DeActRngReq_t* deactringReq;
    
 deactringReq = (DeActRngReq_t*) CLSIMsgCreateHeader(CLS_DEACTIVATE_RING | CLS_REQUEST,
						     0,
						     CLS_LLC_LAYER,
						     CLS_P_SAP_ID,
						     dp->p_sap_id);
 if (deactringReq == NULL)
  return NULL;

 deactringReq->fVersion  =  1;
 deactringReq->fCorrelator = 0xdead;
 deactringReq->fCLSIHdr.fUSapId = dp->u_sap_id;

 return((TCLSIMsg*) deactringReq);
    
}

/*
 * Test Request.
 */
TCLSIMsg* CreateTestReq (struct D_test_req *dp)
{
 TestReq_t* testReq;

 testReq = (TestReq_t*) CLSIMsgCreateHeader(CLS_TEST | CLS_REQUEST,
				   offsetof(TestReq_t, fTEST) -
				   CLS_TEST_REQ_HDR_SZ +
				   dp->testLength,
				   CLS_LLC_LAYER,
				   CLS_P_CEP_ID,
				   dp->pCepId);
 if (testReq == NULL)
  return NULL;

 testReq->fVersion = CLSI_VERSION;
 testReq->fCorrelator = 0xdead;
 testReq->fReserveLength = 0;
 bcopy(dp->testData, &testReq->fTEST[0], dp->testLength);
 return((TCLSIMsg*) testReq);
    
}

/*
 * Test response.
 */
TCLSIMsg* CreateTestRsp (struct D_test_rsp *dp)
{
 TestRsp_t* testRsp;

 testRsp = (TestRsp_t*) CLSIMsgCreateHeader(CLS_TEST | CLS_RESPONSE,
				   offsetof(TestRsp_t, fTEST) -
				   CLS_TEST_RSP_HDR_SZ +
				   dp->testLength,
				   CLS_LLC_LAYER,
				   CLS_P_CEP_ID,
				   dp->pCepId);
 if (testRsp == NULL)
  return NULL;

 testRsp->fVersion = CLSI_VERSION;
 testRsp->fCorrelator = 0xdead;
 testRsp->fReserveLength = 0;
 bcopy(dp->testData, &testRsp->fTEST[0], dp->testLength);
 return((TCLSIMsg*) testRsp);
    
}

/*
 * udata request.
 */

TCLSIMsg* CreateUDataReq (struct D_udata_req *dp)
{
 UDataReq_t* udataReq;
    
 udataReq = (UDataReq_t*) CLSIMsgCreateHeader(CLS_UDATA | CLS_REQUEST,
                                       offsetof(UDataReq_t, fUData) -
				       CLS_UDATA_REQ_HDR_SZ +
				       dp -> udataLength,
				       CLS_LLC_LAYER,
				       CLS_P_CEP_ID,
				       dp->pCepId);
 if (udataReq == NULL)
  return NULL;

 udataReq->fVersion = CLSI_VERSION;
 udataReq->fCorrelator = 0xdead;
 udataReq->fReserveLength = 0;
 bcopy(dp->theUData, &udataReq->fUData[0], dp->udataLength);
 return((TCLSIMsg*) udataReq);
}

/*
 * Flow request.
 */

TCLSIMsg* CreateFlowReq (struct D_flow_req *dp)
{
 FlowReq_t* flowReq;
    
 flowReq = (FlowReq_t*) CLSIMsgCreateHeader(CLS_FLOW | CLS_REQUEST,
                                       0,
				       CLS_LLC_LAYER,
				       CLS_P_CEP_ID,
				       dp->pCepId);
 if (flowReq == NULL)
  return NULL;

 flowReq->fVersion = CLSI_VERSION;
 flowReq->fCLSIHdr.fFlowAction = dp -> action;
 return((TCLSIMsg*) flowReq);
}


TCLSIMsg* CreateIDStnReq (struct D_generic_stn_msg *dp)
{
    IdStnReq_t* idStnReq;

    idStnReq = (IdStnReq_t*) CLSIMsgCreateHeader(CLS_ID_STN | CLS_REQUEST,
                                                 offsetof(IdStnReq_t, fXID) -
                                                 CLS_ID_STN_REQ_HDR_SZ +
                                                 dp->frame_Length,
                                                 CLS_LLC_LAYER,
                                                 CLS_P_SAP_ID,
                                                 dp->p_sap_id);
    if (idStnReq == NULL)
        return NULL;

    idStnReq->fVersion = CLSI_VERSION;
    idStnReq->fCorrelator = 0xdead;
    idStnReq->fReserveLength = 0;
    idStnReq->fRSap = dp->rSap;
    idStnReq->fLSap = dp->lSap;
    idStnReq->fRifLength = 0;  /*rifLength*/
    ieee_copy(dp->localAddr, idStnReq->fLMac);
    ieee_copy(dp->remoteAddr, idStnReq->fRMac);

    bcopy(dp->frame_Data, &idStnReq->fXID[0], dp->frame_Length);
    return((TCLSIMsg*) idStnReq);
}


TCLSIMsg* CreateIDStnRsp (struct D_generic_stn_msg *dp)
{
    IdStnRsp_t* idStnRsp;

    idStnRsp = (IdStnRsp_t*) CLSIMsgCreateHeader(CLS_ID_STN | CLS_RESPONSE,
                                                 offsetof(IdStnRsp_t, fXID) -
                                                 CLS_ID_STN_RSP_HDR_SZ +
                                                 dp->frame_Length,
                                                 CLS_LLC_LAYER,
                                                 CLS_P_SAP_ID,
                                                 dp->p_sap_id);
    if (idStnRsp == NULL)
        return NULL;

    idStnRsp->fVersion = CLSI_VERSION;
    idStnRsp->fCorrelator = 0xdead;
    idStnRsp->fReserveLength = 0;
    idStnRsp->fRSap = dp->rSap;
    idStnRsp->fLSap = dp->lSap;
    idStnRsp->fRifLength = dp->rifLength;   /* rifLength */
    bcopy(dp->rif, idStnRsp->fRif, dp->rifLength);
    ieee_copy(dp->localAddr, idStnRsp->fLMac);
    ieee_copy(dp->remoteAddr, idStnRsp->fRMac);

    bcopy(dp->frame_Data, &idStnRsp->fXID[0], dp->frame_Length);
    return((TCLSIMsg*) idStnRsp);
}


/*
 * TestStn Request.
 */
TCLSIMsg* CreateTestStnReq (struct D_generic_stn_msg *dp)
{
    TestStnReq_t* testStnReq;

    testStnReq = (TestStnReq_t*) CLSIMsgCreateHeader(CLS_TEST_STN | CLS_REQUEST,
                                                     offsetof(TestStnReq_t, fTEST) -
                                                     CLS_TEST_STN_REQ_HDR_SZ +
                                                     dp->frame_Length,
                                                     CLS_LLC_LAYER,
                                                     CLS_P_SAP_ID,
                                                     dp->p_sap_id);

    if (testStnReq == NULL)
        return NULL;

    testStnReq->fVersion = CLSI_VERSION;
    testStnReq->fCorrelator = 0xdead;
    testStnReq->fReserveLength = 0;

    testStnReq->fRSap = dp->rSap;
    testStnReq->fLSap = dp->lSap;

    testStnReq->fRifLength = dp->rifLength;
    bcopy(dp->rif, testStnReq->fRif, dp->rifLength);
    ieee_copy(dp->localAddr, testStnReq->fLMac);
    ieee_copy(dp->remoteAddr, testStnReq->fRMac);

    bcopy(dp->frame_Data, &testStnReq->fTEST[0], dp->frame_Length);
    return((TCLSIMsg*) testStnReq);

}


/*
 * TestStn response.
 */
TCLSIMsg* CreateTestStnRsp (struct D_generic_stn_msg *dp)
{
    TestStnRsp_t* testStnRsp;

    testStnRsp = (TestStnRsp_t*) CLSIMsgCreateHeader(CLS_TEST_STN | CLS_RESPONSE,
                                                     offsetof(TestStnRsp_t, fTEST) -
                                                     CLS_TEST_STN_RSP_HDR_SZ +
                                                     dp->frame_Length,
                                                     CLS_LLC_LAYER,
                                                     CLS_P_SAP_ID,
                                                     dp->p_sap_id);
    if (testStnRsp == NULL)
        return NULL;

    testStnRsp->fVersion = CLSI_VERSION;
    testStnRsp->fCorrelator = 0xdead;
    testStnRsp->fReserveLength = 0;
    testStnRsp->fRSap = dp->rSap;
    testStnRsp->fLSap = dp->lSap;
    ieee_copy(dp->localAddr, testStnRsp->fLMac);
    ieee_copy(dp->remoteAddr, testStnRsp->fRMac);
   
    testStnRsp->fRifLength = dp->rifLength;
    bcopy(dp->rif, testStnRsp->fRif, dp->rifLength);
    bcopy(dp->frame_Data, &testStnRsp->fTEST[0], dp->frame_Length);
    return((TCLSIMsg*) testStnRsp);
}


/*
 * udatastn request.
 */

TCLSIMsg* CreateUDataStnReq (struct D_generic_stn_msg *dp)
{
    UDataStnReq_t* udataStnReq;

    udataStnReq = (UDataStnReq_t*) CLSIMsgCreateHeader(CLS_UDATA_STN | CLS_REQUEST,
                                                       offsetof(UDataStnReq_t, fUData) -
                                                       CLS_UDATA_STN_REQ_HDR_SZ +
                                                       dp->frame_Length,
                                                       CLS_LLC_LAYER,
                                                       CLS_P_SAP_ID,
                                                       dp->p_sap_id);
    if (udataStnReq == NULL)
        return NULL;

    udataStnReq->fVersion = CLSI_VERSION;
    udataStnReq->fCorrelator = 0xdead;
    udataStnReq->fReserveLength = 0;

    udataStnReq->fRSap = dp->rSap;
    udataStnReq->fLSap = dp->lSap;

    udataStnReq->fRifLength = dp->rifLength;
    bcopy(dp->rif, udataStnReq->fRif, udataStnReq->fRifLength);

    ieee_copy(dp->localAddr, udataStnReq->fLMac);
    ieee_copy(dp->remoteAddr, udataStnReq->fRMac);

    bcopy(dp->frame_Data, &udataStnReq->fUData[0], dp->frame_Length);
    return((TCLSIMsg*) udataStnReq);
}


static boolean cls_dlsw_oqueue (hwidbtype *idb, paktype *pak, enum HEADTAIL which)
{
    clear_if_input(pak);	/* don't care where you came from... */
    pak->if_input = idb->firstsw; /* ...now you came from 'me' */
    pak->if_output = NULL;

    switch (pak->linktype)	/* linktype defines _source_ of packet */
    {
	/*
	 * Packet came from transparent bridge...
	 * ...ship it to DLSw
	 */
    case LINK_BRIDGE:
        if (reg_invoke_dlsw_running(0,0,0)) {
          if (is_80d5_enabled(pak, idb))
              xform_80d5_to_802(pak);
  
          if (is_fddi(idb)) /* already in token ring format */
            tring_parse_packet(idb, pak, NULL);
          else
            (*idb->parse_packet)(idb, pak, NULL);
          dlsw_work(pak, idb, pak->riflen);
        }
        else
          datagram_done(pak);
        break;

    case LINK_CLS:
	/*
	 * Packet came from CLS...
	 * ...ship it into transparent bridge group
	 */
	/* check to make sure idb is CLS vidb */
	if (is_80d5_enabled(pak, idb))
	    xform_802_to_80d5(pak, idb->tr_transit_oui);
	
	/* Force the linktype */
	pak->linktype = LINK_BRIDGE;
	net_bridge_enqueue(pak);
	break;	

    default:
	datagram_done(pak);

    }
    return(TRUE);
}

/* dlsw_default_get_ring_request
 *
 * Default model to get Activate Ring Request;
 * This is based on Token Ring
 */

TCLSIMsg *dlsw_default_get_ring_req (idbtype *swidb)
{
    ActRngReq_t* 	actringReq;
    ActRngTRParms_t* 	trParms;

    actringReq = (ActRngReq_t *)CLSIMsgCreateHeader(CLS_ACTIVATE_RING | CLS_REQUEST,
						    offsetof(ActRngReq_t,fParms) -
						    CLS_ACT_RING_REQ_HDR_SZ +
						    sizeof(ActRngTRParms_t),
						    CLS_LLC_LAYER, CLS_ENTITY_MGR, 0);

    if (actringReq != NULL) {
	trParms = (ActRngTRParms_t*) &(actringReq->fParms);
	trParms->fT1 = 0;
	trParms->fT2 = 0;
	trParms->fTi = 0;
	trParms->fMaxOut = 0;
	trParms->fMaxRetryCount = 0;		/* no limit */
	trParms->fMaxIn = 0;
	trParms->fMaxIField = 0;
    }
    return ((TCLSIMsg *)actringReq);
}
