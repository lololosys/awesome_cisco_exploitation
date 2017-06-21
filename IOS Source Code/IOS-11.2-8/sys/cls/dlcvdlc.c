/* $Id: dlcvdlc.c,v 3.3.26.10 1996/09/03 22:27:22 rbatz Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlcvdlc.c,v $
 *------------------------------------------------------------------
 * Internal CLS connections between two CLS-DLUS so that one DLU can
 * act as a DLC for the other DLU
 *
 * August, 1995         Phil Morton
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 *
 * CLEARCASE SUPERCEDES THE LOG
 *
 * CSCdi71922 is a major design shift
 *
 * Here are some notes from the DDTs.
 *
 * To explain it briefly, I did vdlc as a stateless piece
 * of code.  To fix the ddts I need to define states so
 * that I can tell one DLU if the other has closed down
 * the connection.  You can't just pass DISC.IND, because
 * the CEPs may be in the process of XID negotiation,
 * or some other state other than connected.
 * So I added states, and hence a rather large diff.
 * 
 * Let me explain the way Close Stn Req works.
 *
 * If a Vcep (say VCep A) receives Close Stn. Req it does everything
 * it can to get its partner (say VCep B) to close too.  (It would be
 * so much easier with a CLS_CLOSE_STN_IND primitive.)  This is done
 * by calling VCepEventInput for VCep B with VDLC_CLOSE_STN_IND.  This
 * puts VCep B into CLOSING state, and then VCep A's Close Stn. Req
 * always completes with VCepDestroy, and it's no longer there.
 *
 * Note A.
 *
 * Since we don't have a Close Stn. Ind, Cep B needs to send different
 * messages to its DLU depending on what it's doing.  For instance if
 * it's in the middle of connecting, it wants to do a Connect Confirm
 * (error), if it's waiting for an XID response it needs to time that
 * out with NO_REM_STN, etc.
 *
 * This is why I needed to add states.  So there has to be a way to
 * add the states.  There are a numbe of things that make a VCep
 * change state.  Amongs opther things there are some messages that
 * come from the DLU that make that happen.  The code to do this is in
 * VCepSetStateFromClsOpCode.
 *
 * Another thing that makes a VCep change state is when its partner
 * gets a message.  In this case the state is different, to reflect
 * the it is sending a message to the DLU.  It uses the routine
 * VCepSetStateFromDlcOpcode.
 *
 * I suppose I could have made them both adjust states with one
 * routine but it would have tied them together more than I want to.
 * One of the fun things about VDLC is that theVCep's are largely
 * independent of each other.  I think that makes the code more
 * stable.
 * ****************************************************************
 * $Log: dlcvdlc.c,v $
 * Revision 3.3.26.10  1996/09/03  22:27:22  rbatz
 * CSCdi67189:  Multiple connections for reverse QLLC (DLSW+) crashes.
 * Branch: California_branch
 *
 * Revision 3.3.26.9  1996/08/30  20:31:24  pmorton
 * CSCdi67720:  LNM over DLSw broken in 11.2
 * Branch: California_branch
 * Merge in changes from 11.1 version.
 *
 * Revision 3.3.26.8  1996/08/28  12:39:35  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.26.7  1996/08/26  15:04:32  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.3.26.6  1996/08/16  07:05:32  ioakley
 * CSCdi63385:  Prevent router crash caused by CLS Connect_Req failure
 *              and resolve LNM conn pending/reporting link issues.
 * Branch: California_branch
 *
 * Revision 3.3.26.5  1996/08/07  08:57:40  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.26.4  1996/08/03  23:21:10  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.3.26.3  1996/07/09  05:48:24  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.3.26.2  1996/05/17  10:42:52  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.14.8  1996/05/04  02:08:01  pleung
 * CSCdi56227:  appn stop link doesnt function properly appn over dlsw
 * Branch: IbuMod_Calif_branch
 * To ensure VDLC inform its partner when only CloseStnRequest is used w/o
 * DISC
 *
 * Revision 3.3.14.7  1996/04/26  15:16:39  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.14.6  1996/04/26  01:52:56  doner
 * CSCdi55831:  IBU Modularity - CLSI and VDLC converted to new IOS
 * scheduler
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.14.5  1996/04/11  14:36:18  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.14.4  1996/04/10  17:40:51  ppearce
 * CSCdi52877:  VDLC merge into IbuMod_Calif
 * Branch: IbuMod_Calif_branch
 *   Temporary fix for crash when VDLC sends ReqOpnStn.Cfm(NO_REMOTE_STN)
 *
 * Revision 3.3.14.3  1996/04/03  13:54:57  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.14.2  1996/03/29  19:37:33  ioakley
 * CSCdi52877:  VDLC merge into IbuMod_Calif - Resolve various LNM/VDLC
 *              inconsistencies & include LNM/VDLC disable port updates.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.14.1  1996/03/28  08:19:40  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.3.26.1  1996/03/18  19:10:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/03/13  01:14:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.2.1  1996/03/07  08:41:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/18  20:38:02  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2.4.3  1996/03/03  00:26:17  ioakley
 * CSCdi50585:  Pick up Phil Morton's Vdlc_Rework and
 *              Mark Regan's Appn over DLSw updates.
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2.4.2  1996/01/20  06:08:15  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.2.4.1  1995/12/17  09:50:13  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.2  1995/11/17  08:56:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:53:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/25  21:19:20  pmorton
 * Add placeholder for new feature
 *
 *------------------------------------------------------------------
 * $Endlog$ 
 */



#include "master.h"
#include "logger.h"
#include "../cls/msg_dlc.c"		/* Not a typo, see logger.h */
#include "cls_private.h"
#include <stdarg.h>
#include "../h/subsys.h"
#include "mgd_timers.h"
#include "../if/network.h"
#include "../if/if_vidb.h"
#include "../util/avl.h"
#include "../llc2/llc2.h"
#include "../srt/srb.h"
#include "../srt/srb_core.h"
#include "../srt/rsrb.h"
#include "../srt/srt_registry.h"

#define INLINE

#include "dlcvdlc_private.h"

/* local function declarations */

static boolean VCepClsNotify(VCepT *vdlc, ClsOpcodeT opcode,
			     ClsErrorCodeT ClsDlcErrorCode, paktype *pak);

/* VdlcInit
 *
 * Initialize the subsystem, do not allocate any resources
 */

static void VdlcInit (subsystype *subsys)
{
    reg_add_dlc_wput(CLS_VDLC, VdlcWput, "VdlcWput");
    reg_add_dlsw_get_ring_req(CLS_VDLC, VPortGetRingReq,
			     "VPortGetRingReq");
    reg_add_dlsw_get_sap(CLS_VDLC, VdlcGetBridgingSapId, 
			 "VdlcGetBridgingSapId");
    reg_add_vdlc_default_port(VdlcDefaultPort, "VdlcDefaultPort");
    return;
}

/*
 * CLS subsystem header
 */

#define VDLC_MAJVERSION 1
#define VDLC_MINVERSION 0
#define VDLC_EDITVERSION  1

SUBSYS_HEADER(dlc_vdlc, VDLC_MAJVERSION, 
	      VDLC_MINVERSION, VDLC_EDITVERSION,
	      VdlcInit, SUBSYS_CLASS_PROTOCOL,
	      "seq: dlc",
	      "req: dlc");

/* VdlcStart
 *
 * This is where resources are committed
 */

static INLINE void VdlcStart (VCepT *vdlc, ClsDlcParamBlockT *clsBlock)
{
    static int started = FALSE;
    static int vdlc_back_pid = 0;

    if (started)
	return;

    if (!vdlc_back_pid) {
	vdlc_back_pid = process_create(vdlc_background,
				       "VDLC Background",
				       LARGE_STACK,PRIO_NORMAL);
	/* Using default stack size will give stack low warning */
	if (vdlc_back_pid == NO_PROCESS) {
	    vdlc_back_pid = 0;
	    return;
	}
	mgd_timer_init_parent(&VdlcMasterTimer, NULL);
    }
    
    started = TRUE;
    list_create(&VdlcBgList, 0, NULL, 0);
    VdlcBgList_look = create_watched_boolean(VDLCBGLIST,VDLCBGLIST_EVENT);
    
    return;
}


/************************************************************************
 *			   CLSI Primitive: Enable.req
 ************************************************************************
 *
 * If the port (vmac) is already there, we're done,
 * Else create the VPortT and add it the the vrg,
 *
 * Input paramters:
 *	ring-group number,
 *	virtual mac address (optional)
 * 	 if vmac is not specified, then the default port is used.
 *	 The call still succeeds even if there is no default port and no vmac
 *	 That is necessary because DLSw doesn't need either one.
 *	 It's up to the other DLU's (APPN,DSPU) to make sure that there is a 
 *	 vmac.
 *
 *
 * Output parameters:
 *
 *	Vport
 *	vmac address for the port (NULL, if none was supplied and DLSw+
 *	 is using an unnumbered port for the bridging SAP)
 *
 * DLC Message Generated
 * ---------------------
 * vdlc_wput(*cep, DLC_ENABLE_REQ, &param_block)
 *
 *
 ************************************************************************
 */

static void 
VdlcEnableReq (void *dummy, ClsDlcParamBlockT *clsBlock)
{
    DlcEnableT		*dlcEnable;
    VDLCEnableT		*vdlcEnable;
    ushort		vrn;
    tr_vgrouptype	*vrg;
    VrgT		*vdlc;
    idbtype		*idb;
    hwidbtype		*hwidb;
    VPortT		*port;
    uchar		*vmac;

    /* Check if the ring group exists */

    dlcEnable = &clsBlock->fParameters.fDlcEnable;
    vdlcEnable = &dlcEnable->fMediaEnable.fVDLCEnable;

    idb = DlcToIdb(&dlcEnable->fPortId);
    if (idb != NULL) {			/* idb and port already exist */
	hwidb = hwidb_or_null(idb);
	if (hwidb == NULL) {
	    dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_UNKNOWN_RING);
	    return;
	}
	vrn = srbV.srb_sb_get_targetring(idb);
    } else {
	vrn = vdlcEnable->fRingGroup;
    }

    /* Check if the ring group exists */
    vrg =  vrfind_ringgroup(vrn);

    if (vrg == NULL) {
	dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_UNKNOWN_RING);
	return;
    }

    /*
     * If the Vrg doesn't exist for vmac yet, create it.
     */
     
    if (vrg->vr_vdlc == NULL) {
    	vrg->vr_vdlc = VrgCreate(vrg);
	if (vrg->vr_vdlc == NULL) {
            dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_OUT_OF_MEMORY);
            return;
        }
    }
    
    vdlc = vrg->vr_vdlc;
    
    /* If the Vport doesn't exist yet, create it */
    
    if (snpa_get_type(&vdlcEnable->fVMacAddr) != STATION_IEEE48) {
	dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_UNKNOWN_PORT);
	return;
    }
    
    vmac = vdlcEnable->fVMacAddr.addr;

    port = VrgLookupPortByVmac(vdlc, vmac);
    if (port == NULL) {
	port = VPortCreate(vmac, vdlc);
	if (port == NULL) {
	    dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_OUT_OF_MEMORY);
	    return;
	}
	VrgAddPort(vdlc, port);		/* add port to the list */
	if (cls_debug_vdlc) {
	    buginf("\nVDLC: Added Port %e at 0x%x", vmac, port);
	}
    }
    ieee_copy(port->fVmac, vmac);	/* return vmac to the caller */
    
    reg_invoke_dlsw_add_cache(vmac);	/* tell DLSw that vmac is known */

    dlcEnable->fPPortId = (DlcPPortIdT) port->fIdb; /* for future reference	*/
    vdlcEnable->fMaxIFieldLength = llc_mtu(port->fIdb->hwptr);
    dlcEnable->fMediaEnable.fVDLCEnable.fVersion = CLSI_VERSION;
    dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_OK);
    VdlcStart(NULL, NULL);			/* kick off the background process */
}


/************************************************************************
 *			   CLSI Primitive: Disable.req
 ************************************************************************
 *
 * Disable port 
 *
 * DLC Message Generated
 * ---------------------
 * vdlc_wput(*cep, DLC_DISABLE_REQ, &param_block)
 *
 *
 ************************************************************************
 */

static void VdlcDisableReq (void *dummy, ClsDlcParamBlockT *clsBlock)
{
#ifdef PHIL_DO_ME_LATER
    DlcDisableT       	*dlcDisable;
    VPortT		*port;

    /* 
     * There is a race condition between removing the vrg and 
     * receiving the VdlcDisableReq (since Disable.Req is queued).
     * This causes the VrgLookupPortByPortId() to fail. For now, we
     * must return success.
     */
    dlcDisable = &clsBlock->fParameters.fDlcDisable;
    port = VrgLookupPortByPortId(&dlcDisable->fPortId);

    if (port == NULL) {
	dlc_rput(clsBlock, CLS_DISABLE_CNF, CLS_UNKNOWN_PORT);
	return;
    }
#endif

    dlc_rput(clsBlock, CLS_DISABLE_CNF, CLS_OK);
    return;
}


/************************************************************************
 * 		   CLSI Primitive: Activate_sap.req
 *************************************************************************
 * Controls which SAP values we will respond.
 *
 * DLC Message Generated
 * ---------------------
 * vdlc_wput(*cep, DLC_ACTIVATE_SAP_REQ, &param_block)
 *
 * Input parameters:
 * port as portId
 * Sap value
 *
 * Adds the specified sap value to the ports list of SAPs
 *
 **********************************************************************
 */
static INLINE void VPortActivateSapReq (VCepT *vdlc, ClsDlcParamBlockT *clsBlock)
{
    DlcActSapT		*dlcActSap;
    VdlcActSapT		*vdlcActSap;
    VPortT		*port;
    VSapT		*sap;

    dlcActSap = &clsBlock->fParameters.fDlcActSap;
    vdlcActSap = &dlcActSap->fDlcSap.fVdlcActSap;

    port = VrgLookupPortByPortId(&clsBlock->fPortId);
    if (port == NULL) {
	errmsg(&msgsym(OPTNULL, DLC));
	dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    /* CAN A VDLC SAP BE PRE-REGISTERED ??? PHIL */
    
    sap = VPortLookupSapByValue(port, vdlcActSap->fLSapValue);
    if (sap != NULL) {
	/* 
	 * Sap already registered. if it is in use by another DLU.
	 *
	 * Check if it is preregistered for
	 * legacy code, 			??? PHIL
	 */
	dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_SAP_BUSY);
	return;
    }
	   
    sap = VSapCreate(dlcActSap->fUSapId, vdlcActSap->fLSapValue, port,
    	VDLC_NORMAL_PRIORITY);
    if (sap == NULL) {
	dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_OUT_OF_MEMORY);
	return;
    }

    VPortAddSap(port, sap);
    dlcActSap->fDlcPSapId = (DlcPSapIdT)sap;
    if (cls_debug_vdlc) {
	buginf("\nVDLC: Port %e Act Sap %02x, Cls SapId 0x%x, Dlc SapId 0x%x",
	       VPortGetVmac(port), VSapGetSapValue(sap), 
	       VSapGetUSapId(sap), sap);
    }
    dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_OK);
    return;
}

/************************************************************************
 * 		   CLSI Primitive: Modify_sap.req
 *************************************************************************
 * Modifies operational parameters on the SAP
 *
 * DLC Message Generated
 * ---------------------
 * vdlc_wput(*cep, DLC_MODIFY_SAP_REQ, &param_block)
 ***********************************************************************
 */

static INLINE  void VPortModifySapReq (
    	VCepT *vdlc, ClsDlcParamBlockT *clsBlock)
{
    return;
}

/************************************************************************
 * 		   CLSI Primitive: Deactivate_sap.req
 *************************************************************************
 * We won't respnd to this SAP any more.
 *
 * DLC Message Generated
 * ---------------------
 * vdlc_wput(*cep, DLC_DEACTIVATE_SAP_REQ, &param_block)
 ***********************************************************************
 */

static INLINE  void VPortDeactivateSapReq (
    VCepT *vdlc, ClsDlcParamBlockT *clsBlock)
{
    DlcActSapT		*dlcActSap;
    VdlcActSapT		*vdlcActSap;
    VPortT		*port;
    VSapT		*sap;

    dlcActSap = &clsBlock->fParameters.fDlcActSap;
    vdlcActSap = &dlcActSap->fDlcSap.fVdlcActSap;

    port = VrgLookupPortByPortId(&clsBlock->fPortId);
    if (port == NULL) {
	errmsg(&msgsym(OPTNULL, DLC));
	dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    sap = VPortLookupSapByValue(port, vdlcActSap->fLSapValue);
    if (sap == NULL) {
	/* 
	 * Sap not there - fail
	 */
	dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_UNKNOWN_SAP);
	return;
    }
	   
    VPortRemoveSap(port, sap);
    if (cls_debug_vdlc) {
	buginf("\nVDLC: Port %e Deact Sap %02x",
	       VPortGetVmac(port), VSapGetSapValue(sap));
    }

    VSapDestroy(sap);
    dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_OK);

    return;
}


/************************************************************************
 * 		   CLSI Primitive: Activate_ring.req
 *************************************************************************
 * Controls which SAP values we will respond.
 *
 * DLC Message Generated
 * ---------------------
 * vdlc_wput(*cep, DLC_ACTIVATE_RING_REQ, &param_block)
 *
 * Input parameters:
 * port as portId
 *
 * Adds the bridging SAP to the Vrg
 *
 **********************************************************************
 */
static INLINE  void VPortActivateRngReq (
	VCepT *dummy, ClsDlcParamBlockT *clsBlock)
{
    DlcActRngT		*dlcActRing;
    VdlcActRngT		*vdlcActRing;
    VrgT		*vdlc;
    VPortT		*port;
    VSapT		*sap;

    dlcActRing = &clsBlock->fParameters.fDlcActRng;
    vdlcActRing = &dlcActRing->fDlcRng.fVdlcActRng;

    port = VrgLookupPortByPortId(&clsBlock->fPortId);
    if (cls_debug_vdlc) {
	buginf("\nVDLC: ActRing port 0x%x", port);
    }
    if (port == NULL) {
	errmsg(&msgsym(OPTNULL, DLC));
	dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    vdlc = VPortGetVrg(port);

    if (vdlc->fBridgingSap != NULL) {
	/* Ring already activated for VDLC - Die! */
	if (cls_debug_vdlc) {
	    buginf(" - Already activated");
	}
	dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_SAP_BUSY);
	return;
    }
	   
    sap = VSapCreate(dlcActRing->fClsUSapId, 0, port,
    	VDLC_NORMAL_PRIORITY);
    if (sap == NULL) {
	dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_OUT_OF_MEMORY);
	return;
    }

    vdlc->fBridgingSap = sap;
    dlcActRing->fDlcPSapId = (DlcPSapIdT)sap;
    if (cls_debug_vdlc) {
	buginf(" vsap 0x%x", sap);
    }
    dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_OK);
    return;
}

/*
 * 		   CLSI Primitive: DeActivate_Ring.req
 *
 * DLC Message Generated
 * ---------------------
 * vdlc_wput(*cep, VDLC_DEACTIVATE_RING_REQ, &param_block)
 */

static INLINE void VPortDeActivateRngReq (void *dummy, 
					  ClsDlcParamBlockT *clsBlock)
{
    DlcActRngT		*dlcActRng;
    VPortT		*port;
    VrgT		*vdlc;

    dlcActRng = &clsBlock->fParameters.fDlcActRng;

    port = VrgLookupPortByPortId(&clsBlock->fPortId);

    if (cls_debug_vdlc) {
	buginf("\nVDLC: DeactRing port 0x%x", port);
    }
    if (port == NULL) {
	errmsg(&msgsym(OPTNULL, DLC));
	dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    vdlc = VPortGetVrg(port);
    if (vdlc->fBridgingSap == NULL) {
	if (cls_debug_vdlc) {
	    buginf(" - no active ring");
	}
	/* No Active Ring for VDLC - Die! */
	dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_UNKNOWN_SAP);
	return;
    }
    VSapDestroy(vdlc->fBridgingSap);
    vdlc->fBridgingSap = NULL;
	   
    dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_OK);
    return;
}


/************************************************************************
 * 	      CLSI Primitive: Req_opnstn.req (outgoing)
 *************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * vdlc_wput(*cep, DLC_REQ_OPNSTN_REQ, &parm_block)
 *
 * Inputs:
 *
 * PSapId
 * Dst, Src Mac
 * Dst, Src Sap
 *
 * Algorithm:
 *
 *  Extract info to build dtestring (Dst Mac, Sap, (Src Mac, Sap)
 *  Create VCep - fail if you can't.
 *  Does the Dst mac and sap, sap match a VDLC mac and and SAP
 *	If yes, is there a request open request pending that matches
 * 	this one?
 *	 If yes, do a request open Stn Confirm on both stations, and 
 *	 connect them to each other.
 *	 If no, and Dsp Sap is activated, send Id Stn Ind to Dst mac, Sap.
 *	 If no, and no Dst Sap is activated, fail
 *	Else,
 *	 Send test station ind to Bridging Sap, if present,
 *	 Copy test packet for srb_background
 *	 Start timer
 *
 *
 * Simplification 2/5/96 - PHIL
 * 
 *
 * Several changes made to original code:
 *
 * 1. We never build Id Stn Ind.  We could on End stations to issue Id
 * Req and use VCepSendToPartnerSap to convert message into messages
 * sent to the station component.
 *
 * 2. The bridging SAP is not really assicaoted with any particular
 * port.  This means that we can categorzie all VDLC connections more
 * economically:
 * Either VPort,SAP<---->Bringing SAP,
 * Or     VPort,SAP<---->VPort,SAP
 *
 * 3. This code doesn't try to build explorers to send to SRB.  It will
 * let everything go through DLSw.
 *
 * Code Path
 *
 *  Extract info to build dtestring (Dst Mac, Sap, (Src Mac, Sap)
 *  Create VCep - fail if you can't.
 *
 * If Dest Mac is a VDLC port
 *	Look up Dest Sap
 * Else
 *	If Source SAP is Bridging SAP
 *		Request Open Station Confirm (CLS_UNKNOWN_PORT)
 *	Else
 *		Dest Sap is Bridging Sap
 *
 * If Dest Sap exists
 *	If Dest Cep exists
 *		Rendezvous (state is VDLC_OPEN)
 *	Else
 *		If Dest SAP is bridging SAP
 *			Build TEST.STN.IND and send to Bridging SAP
 *			state = VDLC_OPENING
 *			
 *		Else
 *			Request Open Station Confirm (CLS_OK)
 *			state = VDLC_HALF_OPEN
 *			Start Timer
 * Else
 *	Request Open Station Confirm (CLS_UNKNOWN_SAP)
 *
 ***********************************************************************/

static INLINE  void VSapReqOpnStnReq (
    VCepT *vdlc, ClsDlcParamBlockT *clsBlock)
{
    DlcReqOpenStnT	*dlcReqOpenStn;
    VdlcReqOpenStnT	*vdlcReqOpenStn;
    ClsUCepIdT		uCepId;
    VrgT		*vrg;
    VPortT		*port,		*partnerPort;
    VSapT		*sap,		*partnerSap;
    VCepT		*cep,		*partnerCep;
    uchar  dtestring[LLC_MAX_DTE_SIZE], partnerString[LLC_MAX_DTE_SIZE];
 
/*
 *  Extract info to build dtestring (Dst Mac, Sap, (Src Mac, Sap)
 */

    dlcReqOpenStn = &clsBlock->fParameters.fDlcReqOpenStn;
    vdlcReqOpenStn = &dlcReqOpenStn->fDlcStn.fVdlcReqOpenStn;
    
    sap = (VSapT *)dlcReqOpenStn->fDlcPSapId;
    port = VSapGetPort(sap);
    vrg = VPortGetVrg(port);
    uCepId = dlcReqOpenStn->fClsUCepId;

    if (cls_debug_vdlc) {
	buginf("\nVDLC: Req Open Stn Req PSap 0x%x, port 0x%x", sap, port);
    }

    if (port == NULL) {
    	CLSASSERT(!"Vdlc: Null Port");
 	DlcReqOpenStnConf((DlcPCepIdT)CLS_NO_CEP, uCepId, CLS_UNKNOWN_PORT);
	return;
    }

    if (vdlcReqOpenStn->fLMacAddr.type == STATION_ILLEGAL) {
	snpa_init(&vdlcReqOpenStn->fLMacAddr, 
		STATION_IEEE48, TARGET_UNICAST, port->fVmac);
    }
    
    llc2_build_dte_string(dtestring,
	&(vdlcReqOpenStn->fRMacAddr),
	&(vdlcReqOpenStn->fLMacAddr),
	(uchar)(vdlcReqOpenStn->fRSapValue),
	(uchar)(vdlcReqOpenStn->fLSapValue), 0);

    if (cls_debug_vdlc) {
	buginf("\n    ");
	VPrintDteString(dtestring);
    }
    if(cls_debug_test) {
	buginf("\nVDLC: ReqOpenStn ");
	VPrintDteString(dtestring);
    }
/*
 *  Create VCep - fail if you can't.
 */
    cep = VCepCreate(uCepId, sap, dtestring, VDLC_NORMAL_PRIORITY, 0);
    if (cep == NULL) {
	DlcReqOpenStnConf((DlcPCepIdT)CLS_NO_CEP, uCepId, CLS_OUT_OF_MEMORY);
	return;
    }
    dlcReqOpenStn->fDlcPCepId = (DlcPCepIdT)cep;

    /*
     * Set timeout parameters
     */
    if (vdlcReqOpenStn->fTimerT1 != 0) {
	cep->fTimeout = vdlcReqOpenStn->fTimerT1;
    } else {
	cep->fTimeout = VDLC_DEFAULT_OPEN_TIMER;
    }
    if (vdlcReqOpenStn->fRetries != 0) {
	cep->fMaxTries = vdlcReqOpenStn->fRetries;
    } else {
	cep->fMaxTries = VDLC_DEFAULT_MAX_RETRIES;
    }
    cep->fRetryCount = 0;
    
    /* now look for a partner CEP */
    VCepBuildPartnerDteString(dtestring, partnerString);

/*
 * Looking for a perfect mate
 *
 * Wanna know what's goin' down
 * Read the description of the AlGoreIthm
 */    
    partnerPort = VrgLookupPortByVmac(VPortGetVrg(port), 
				      &partnerString[LMAC_DTE]);

#ifdef PHIL_NOT_QUITE
    if (partnerPort == NULL) {
    	partnerPort = VrgGetDefaultPort(VPortGetVrg(port));
	/* get the default port if it exists */
    }
#endif

    if (partnerPort != NULL) {
    	partnerSap = VPortLookupSapByValue(partnerPort, 
					   partnerString[LSAP_DTE]);

    } else {				/* partner port is null */
	if (sap == vrg->fBridgingSap) {
	    DlcReqOpenStnConf((DlcPCepIdT)CLS_NO_CEP, uCepId, CLS_UNKNOWN_PORT);
	    VCepDestroy(cep);
	    return;
	} else {
	    partnerSap = vrg->fBridgingSap;
	}
    }

    if (partnerSap != NULL) {
	partnerCep = VSapLookupCep(partnerSap, partnerString);
	if (partnerCep != NULL) {
	    if (VCepRendezvous(cep, partnerCep)) {
		VSapAddCep(sap, cep);
	    } else {			/* We blew it -state inconsistency */
		VCepDestroy(cep);
	    }
	} else {			/* no partner yet */
	    VSapAddCep(sap, cep);
	    if (partnerSap == vrg->fBridgingSap) {
		VCepSendTest(cep);	/* Sets State - Starts Timer */
	    } else {			/* non-bridging sap */
		VCepSetState(cep, VDLC_HALF_OPEN);
		VCepTimerStart(cep, cep->fTimeout);
		DlcReqOpenStnConf((DlcPCepIdT)cep, uCepId, CLS_OK);
	    }
	}
    } else {			/* partner SAP is NULL */
	DlcReqOpenStnConf((DlcPCepIdT)CLS_NO_CEP, uCepId, CLS_UNKNOWN_SAP);
	VCepDestroy(cep);
    }
}


static boolean VCepRendezvous (VCepT *cep, VCepT *partnerCep)
{
    if ( (VCepGetState(partnerCep) == VDLC_HALF_OPEN ||
	  VCepGetState(partnerCep) == VDLC_XID_RSP_PENDING ||
	  VCepGetState(partnerCep) == VDLC_RX_CONNECT_REQ)  &&
	VCepGetPartner(partnerCep) == NULL) {	/* RENDEZVOUS */
	
	if (cls_debug_vdlc) {
	    buginf("\nVDLC: two-way connection established");
	}
	/*
	 * Handle this cep
	 */
	VCepSetPartner(cep, partnerCep);
	VCepTimerStop(cep);
	if (VCepGetState(partnerCep) != VDLC_RX_CONNECT_REQ) {
	    VCepSetState(cep, VDLC_OPEN);
	}
	DlcReqOpenStnConf((DlcPCepIdT)cep, VCepGetUCepId(cep), CLS_OK);
	/*
	 * Now its bosom buddy
	 */
	VCepSetPartner(partnerCep, cep);
	if (VCepGetState(partnerCep) != VDLC_XID_RSP_PENDING) {
	    VCepTimerStop(partnerCep);
	    VCepSetState(partnerCep, VDLC_OPEN);
	}
	return (TRUE);
    } else {
	if (cls_debug_vdlc) {
	    buginf("Partner already taken with 0x%x or in invalid state %s",
		   VCepGetPartner(partnerCep), 
		   VCepPrState(VCepGetState(partnerCep)));
	}
	DlcReqOpenStnConf((DlcPCepIdT)CLS_NO_CEP, VCepGetUCepId(cep), 
		          CLS_VDLC_STATE_ERROR);
	return(FALSE);
    }
}


/************************************************************************
 * 	      CLSI Primitive: closestn.req (outgoing)
 *************************************************************************
 **********************************************************************
 */

static INLINE  void VCepCloseStnReq (
    VCepT *cep, ClsDlcParamBlockT *clsBlock)
{
    DlcCloseStnT	*dlcCloseStn;
    VSapT		*sap;
    ClsUCepIdT 		uCep;

    dlcCloseStn = &clsBlock->fParameters.fDlcCloseStn;
    if (cls_debug_vdlc) {
	buginf("\nVDLC: Close Station Request: ");
	VPrintDteString(VCepGetDteString(cep));
    }
    
    if (cep == (VCepT*) CLS_NO_CEP) {
      /* 
       * Trying to cancel a pending ReqOpnStn. CLS did not know the 
       * p_cep_id (vdlc), but it does know the u_cep_id it sent us in 
       * the ReqOpnStn.req. We need to scan all vdlc structures to 
       * find the correct one to close.
       */
	sap = (VSapT *)dlcCloseStn->fDlcPSapId;
	if (sap != NULL) {
	    uCep = dlcCloseStn->fClsUCepId;
	    cep = VSapLookupCepByUCepId(sap, uCep);
	}
	if (cep == (VCepT *)CLS_NO_CEP) {
	    /* we should be able to find something */
	    errmsg(&msgsym(INVPCEP, DLC));
	    VCepClsNotify(cep, CLS_CLOSE_STN_CNF, CLS_UNKNOWN_CEP, NULL);
	    return;
	}
    }
    /* Close Station Indication for Partner - as good as we can get */
    VCepEventInput(cep->fPartner, VDLC_CLOSE_STN_IND);
    VCepClsNotify(cep, CLS_CLOSE_STN_CNF, CLS_OK, NULL);
    VCepDestroy(cep);
    return;
}

/* VCepEventInput
 *
 * If we ever get around to rewriting VDLC as a state machine this 
 * is the place to collect the inputs.
 */

static void 
VCepEventInput (VCepT *cep, VCepEventT event)
{
    if (cep == NULL) {
	return;
    }
    switch (event) {

    case VDLC_CLOSE_STN_IND:
	VCepProcessCloseStnInd(cep);
	break;
    case VCEP_TIMER:
	VCepProcessTimer(cep);
	break;

    default:
	errmsg(&msgsym(BADPARAM, DLC), "VCepEventInput", event, "event");
	break;
    }
}

static void
VCepProcessTimer (VCepT *cep)
{
    switch (VCepGetState(cep)) {
    default:
	break;
    case VDLC_IDLE:
    case VDLC_OPEN:
	break;
    case VDLC_NO_TEST:
	if (!VCepSendTest(cep)) {
	    VCepDestroy(cep);
	}
	break;
    case VDLC_OPENING:

	{
	    TCLSCep *uCepId;
	    /*
	     * Source SAP is never bridging SAP
	     * We haven't got a response from Bridging SAP
	     * Retry - unless max retries exceeded
	     */
	    if (cep->fRetryCount < cep->fMaxTries) {
		if (cls_debug_test) {
		    buginf("T");
		}
		cep->fRetryCount++;
		if (!VCepSendTest(cep)) {
		    VCepDestroy(cep);
		}
	    } else {
		/*
		 * Don't exactly know what's going on here
		 *   but prevent the crash for now 
		 *
		 * Check uCepId before posting ReqOpenStn confirm
		 *                                      --ppearce
		 */
		uCepId = (TCLSCep *) VCepGetUCepId(cep);
		if (CLSCepCheck(uCepId) && uCepId->fRequest) {
		    DlcReqOpenStnConf((DlcPCepIdT)cep, 
				      (ClsUCepIdT) uCepId,
				      CLS_NO_REM_STN);
		}
		if(cls_debug_test) {
		    buginf("\nVDLC: NO REM STN ");
		    VPrintDteString(VCepGetDteString(cep));
		}
		VCepDestroy(cep);
	    }
	    break;
	}
    case VDLC_HALF_OPEN:
	/* 
	 * Source SAP is (almost) always a bridging SAP
	 * Send it a Disconnect Ind - we'll need to
	 * find a better way to destroy it
	 */
	VCepClsNotify(cep, CLS_DISC_IND, CLS_DISC_ERROR, NULL);
	break;
    case VDLC_XID_RSP_PENDING:
	if (cep->fRetryCount < cep->fMaxTries) {
	    cep->fRetryCount++;
	    VCepTimerStart(cep, cep->fTimeout);
	    break;
	}
	VCepClsNotify(cep, CLS_ID_IND, CLS_NO_XID_RSP, NULL);
	if (cls_debug_test) {
	    buginf("\nVDLC: No XID RSP");
	    VPrintDteString(VCepGetDteString(cep));
	}
        VCepSetState(cep, VDLC_OPEN);
        break;
    case VDLC_RX_CONNECT_REQ:
	VCepClsNotify(cep, CLS_CONNECT_CNF, CLS_NO_REM_STN, NULL);
	VCepSetState(cep, VDLC_HALF_OPEN);
	break;		    
    }
}

static void
VCepProcessCloseStnInd (VCepT *cep)
{
    switch (VCepGetState(cep)) {
	    
    case VDLC_IDLE:
    case VDLC_NO_TEST:
    case VDLC_OPENING:
    case VDLC_HALF_OPEN:
    case VDLC_CLOSING:
	/* 
	 * these states don't make any sense, because we only get 
	 * CLOSE_STN_IND if the partner existed and its DLU sent a
	 * CLOSE_STN_REQ
	 */
	if (cls_debug_err) {
	    buginf("\nVDLC: inconsistency on CLOSE STN IND Cep ");
	    VPrintDteString(VCepGetDteString(cep));
	    buginf(" %s", VCepPrState(VCepGetState(cep)));
	}
	break;
    
    case VDLC_XID_RSP_PENDING:
	if (cls_debug_test) {
	    buginf("	\nVDLC: NO XID RSP ");
	    VPrintDteString(VCepGetDteString(cep->fPartner));
	}
	VCepClsNotify(cep, CLS_ID_IND, CLS_NO_XID_RSP, NULL);
	break;
	    
    case VDLC_OPEN:
	/*
	 *  Both partners present - no Logical connection yet
	 */
        if (cls_debug_vdlc) {
	    buginf("\nVDLC: Disc ind timer pops ");
        }
        VCepClsNotify(cep, CLS_DISC_IND, CLS_INACTIVITY_TIMEOUT, NULL);
	break;

	/*
	 *transitional states for connect/disconnect
	 */

    case VDLC_RX_CONNECT_REQ:	/* from DLU */
	VCepClsNotify(cep, CLS_CONNECT_CNF, CLS_NO_REM_STN, NULL);
	break;

    case VDLC_TX_CONNECT_IND:	/*   to DLU */
	VCepClsNotify(cep, CLS_DISC_IND, CLS_NO_REM_STN, NULL);
	break;

    case VDLC_RX_DISC_REQ:		/* from DLU */
	VCepClsNotify(cep, CLS_DISC_CNF, CLS_OK, NULL);
	break;

    case VDLC_TX_DISC_IND:		/*   to DLU */
	break;			/* have to wait - we already told DLU */
	
    case VDLC_CONNECTED:		/* Connected */
	VCepClsNotify(cep, CLS_DISC_IND, CLS_NO_REM_STN, NULL);
	break;

    default:
	break;
    }
    VCepSetState(cep, VDLC_CLOSING);
    return;
}


/* VSapHandleMsgs
 *
 * This is the framework for handling messages that have to be sent to 
 * a partner SAP.  In this case we have to set various fields in the 
 * CLS - DLC Parameter block, and pass the info to the SAP.
 * Messages are DLC_TEST_STN_REQ/RSP, and DLC_ID_STN_REQ/RSP.
 *
 * These messages only come from the Bridging SAP, and only need to
 * be passed to SAPs on the Vport corresponding to the destination
 * mac addresses.
 *
 * Unfortunately the structures for these are different;
 * but ... the corresponding fields are located at exactly 
 * the same locations for TESTs and XIDs
 *
 * NOTE - THIS ASSUMES THAT ALL THE MESSAGES ORIGINATE ON THE BRIDGING SAP
 * THis may not be true.
 * Apparently LNM will occasionally send to the Active Ring Monitor
 *
 * Handling:
 *
 *	If not match on dmac			- discard the packet
 *	If dsap not zero and no match		- discard the packet
 *
 *	TEST_STN_REQ:
 *		Bounce if dsap zero or match	- or discard the packet
 *	TEST_STN_RSP:
 *		If it's a response to an opening Cep
 *		   REQUEST OPEN STATION CONFIRM
 *	ID_STN_REQ:
 *		ID_STN_IND on dsap match	- or discard the packet
 *	ID_STN_RSP:
 *		Discard the packet
 *	UDATA_STN_REQ:
 *		UDATA_STN_ID on dsap match	- or discard the packet
 * 	Anything else:
 *		Invalid: will never get here from VDLCWput
 */
 
static INLINE void VSapHandleMsgs (VCepT *pCepId, DlcOpcodeT opcode,
			       ClsDlcParamBlockT *clsBlock)
{
    DlcIdStnT		*dlcIdStn;
    LlcXidIdStnT	*llcIdStn;
    VPortT		*port;
    VSapT		*sap;
    VrgT		*vrg;
    
    uchar		*macSwap;
    uchar		sapSwap;

    DlcUdataStnT        *dlcUdataStn;
    LlcUdataStnT        *llcUdataStn;

    dlcUdataStn =       &clsBlock->fParameters.fDlcUdataStn;
    llcUdataStn =       (LlcUdataStnT *) &(clsBlock->fParameters.fDlcUdataStn.fDlcUdata.fLlcUdataStn);

    dlcIdStn = &clsBlock->fParameters.fDlcIdStn;
    llcIdStn = &dlcIdStn->fDlcXid.fLlcXidStn;

   if (cls_debug_vdlc) {
	buginf("\nVDLC: Sap to Sap ");
	PrintDlcOpCode(opcode);
	buginf("VSap 0x%x %e(%02x)->%e(%02x)", 
	       dlcIdStn->fDlcPSapId, 
	       dlcIdStn->fLMac,  dlcIdStn->fLSap,
	       dlcIdStn->fRMac,  dlcIdStn->fRSap);
    }

    switch (opcode) {
      case DLC_ID_STN_RSP:
      	datagram_done(clsBlock->fPakPtr);
	return;
      case DLC_TEST_STN_RSP:
      case DLC_TEST_STN_REQ:
      case DLC_ID_STN_REQ:
      case DLC_UDATA_STN_REQ:
	break;
      default:
	CLSASSERT(!"VSapHandleMsgs - Illegal op code");
	return;
    }

    if (is_ieee_zero(dlcIdStn->fRMac)) {
        datagram_done(clsBlock->fPakPtr);
        return;
    }

    vrg = VPortGetVrg(VSapGetPort((VSapT *)dlcIdStn->fDlcPSapId));
    if (vrg == NULL) {
	errmsg(&msgsym(OPTNULL, DLC));
      	datagram_done(clsBlock->fPakPtr);
	return;
    }

    port = VrgLookupPortByVmac(vrg, dlcIdStn->fRMac);

    if (port == NULL) {		/* no destination port enabled */
	if (cls_debug_vdlc) {
	    buginf(" - no dest port");
	}
	sap = vrg->fBridgingSap;

	/*
	 * Make sure there really is a bridging Sap
	 */
	if (sap == NULL || 

	    /* 
	     * Make sure that we don't forward a message to the
	     * bridging sap if it already came from there. 
	     */
	    dlcIdStn->fClsUSapId == VSapGetUSapId(sap)) {
      	datagram_done(clsBlock->fPakPtr);
	return;
    }
    } else {
	sap = VPortLookupSapByValue(port, dlcIdStn->fRSap);
    }
    if (sap == NULL && cls_debug_vdlc) {
	buginf(" - no dest SAP");
    }
    
    if (opcode == DLC_TEST_STN_REQ) {
	if (dlcIdStn->fRSap == 0 || sap != NULL) {
	    DlcBounceTestStnInd((DlcPCepIdT)CLS_NO_CEP, clsBlock);
	} else {
      	    datagram_done(clsBlock->fPakPtr);
    	}
	return;
    }
    
    if (dlcIdStn->fRSap == 0 || sap == NULL) {
      	datagram_done(clsBlock->fPakPtr);
	return;
    }
    
    /* 
     * handle TEST.STN.RSP
     *
     * Look at the partner Sap and see if there's a partner Cep,
     * If there is, do REQUEST OPEN STATION CONFIRM
     */
    if (opcode == DLC_TEST_STN_RSP) {
	avl_walk(sap->fCepList, (avl_walker_type)VCepHandleTestResponse,
		 dlcIdStn->fLMac);
      	datagram_done(clsBlock->fPakPtr);
	return;
    }
    
    /* 
     * we're down to the final shoot-out - have to handle
     * ID_STN_REQ, and UDATA_STN_REQ.
     * In both cases we need to provide enough information
     * for the SAP to know how to issue a Request Open Station
     * Request later on.
     *
     * This means swapping Destination and Source
     * Mac and Sap pointers.
     *
     * At this point continue to exploit the fact that LlcUDataStnT
     * and LlcXidStnT exactly over lap each other
     *
     * THIS IS WRONG BUT WHAT'S BROKEN IS THE DUPLICATION OF STRUCTRES
     * PHIL 9/15/95
     */


    dlcIdStn->fClsUCepId = (ClsUCepIdT)CLS_NO_CEP;
    dlcIdStn->fDlcPCepId = CLS_NO_CEP;
    dlcIdStn->fClsUSapId = VSapGetUSapId(sap);
    dlcIdStn->fDlcPSapId = (DlcPSapIdT)sap;

    sapSwap = llcIdStn->fLSap;
    llcIdStn->fLSap = llcIdStn->fRSap;
    llcIdStn->fRSap = sapSwap;
    
    macSwap = llcIdStn->fLMacPtr;
    llcIdStn->fLMacPtr = llcIdStn->fRMacPtr;
    llcIdStn->fRMacPtr = macSwap;

    if (opcode == DLC_UDATA_STN_REQ)
	vdlcUDataStnReq(clsBlock, dlcUdataStn, llcUdataStn);
    
    VdlcTellPartner((VCepT *)CLS_NO_CEP, opcode, clsBlock);
    return;
}

/* VCepHandleMsgs
 *
 * This is the U conector at work
 *
 * Messages that correspond to Unnumbered frames for which a cep
 * exists should have either an llc (for Local connections), or 
 * a partner.
 *
 * If we're dealing with a Local connection, just pass the info on to
 * llc_wput.
 *
 * Otherwise tell the partner.
 */
 
static INLINE void VCepHandleMsgs (VCepT *cep, DlcOpcodeT opcode,
				 ClsDlcParamBlockT *clsBlock)
{
    CLSASSERT(cep != (VCepT *)CLS_NO_CEP);

    if (VCepGetLlc(cep)) {		/* local case */
    	llc_wput((DlcPCepIdT)(VCepGetLlc(cep)), opcode, clsBlock);
	return;
    }
    if (VCepGetState(cep) == VDLC_CLOSING) {
    	/*	
	 * Partner issued CLOSE STN REQ
	 * and went away.
	 * Handle what's left
	 */
    	VCepHandleClosingMsgs(cep, opcode, clsBlock);
	return;
    }
	    
    if (cep->fPartner) {		/* tell your partner */
	if (cls_debug_vdlc) {
	    buginf("\nVDLC: CEP to CEP ");
	    PrintDlcOpCode(opcode);
	    VPrintDteString(VCepGetDteString(cep));
	}
	if (VCepGetState(cep->fPartner) == VDLC_XID_RSP_PENDING) {
	    switch(opcode) {
	    case DLC_ID_REQ:
	    case DLC_ID_RSP:
		VCepSetState(cep->fPartner, VDLC_OPEN);
                VCepTimerStop(cep->fPartner);
		break;
	    default:
	    }
	} else {
	    VCepSetStateFromDlcOpcode(cep, opcode);	/* adjust the state */
	}
	VdlcTellPartner(cep->fPartner, opcode, clsBlock);
    } else {
	if (VCepSendToPartnerSap(cep, opcode, clsBlock)) {
	    /* 
	     * Send the message to the partner SAP.
	     * See above for partner SAP definition.
	     */
	    VCepSetStateFromDlcOpcode(cep, opcode);
	} else {
	    VCepNoPartner(cep);
	}
    }
    return;
}

/* VCepSendToPartnerSap
 *
 * Some requests from a Cep can be sent before we have a partner Cep.
 * If there's no context, then we can send the to a partner SAP
 *
 * Partner SAP is defined by the following algorithm:
 * If the destination port is NULL we have to use the bridging SAP
 *
 * Otherwise there's a real port, the must be a real Sap
 *
 * If the source SAP is the bridging SAP, there must be a real destination
 * SAP.
 * 
 *
 * Fail if the partner SAP can't be found, or if the op code requires
 * a partner CEP
 *
 * All frames reach here in canonical form - i.e. pak->datagramstart
 * points to the beginning of the payload.  We have to add in some
 * encapsulation similar to llcStnEncap
 */

static boolean VCepSendToPartnerSap (VCepT *cep,
				  DlcOpcodeT dlcOpcode,
				  ClsDlcParamBlockT *clsBlock)
{
    VrgT	*vrg;
    VPortT	*port;
    VSapT	*sap;
    VSapT	*destSap;
    uchar	*dtestring;
    
    ClsOpcodeT	clsOpcode;
    boolean	isResponse;
    
    DlcIdStnT		*dlcIdStn;
    LlcXidIdStnT	*llcIdStn;
    
    paktype		*pak;
    sap_hdr		*llc1;
    uchar		control;

    dtestring = VCepGetDteString(cep);

    sap = VCepGetSap(cep);
    if (sap == NULL) {
	return (FALSE);
    }
    vrg = VPortGetVrg(VSapGetPort(sap));
    if (vrg == NULL) {
	return (FALSE);
    }

    if (cls_debug_vdlc) {
	buginf("\nVDLC: CEP to SAP ");
	PrintDlcOpCode(dlcOpcode);
	VPrintDteString(dtestring);
    }

    port = VrgLookupPortByVmac(vrg, &dtestring[RMAC_DTE]);
    if (port == NULL) {
	if (vrg->fBridgingSap == NULL) {
	    if (cls_debug_vdlc) {
		buginf(". No dest. failing");
	    }
	    return (FALSE);
	} else {
	    if (cls_debug_vdlc) {
		buginf(" via bridging SAP (DLSw)");
	    }
	    destSap = vrg->fBridgingSap;
	}
    } else {
    	destSap = VPortLookupSapByValue(port, dtestring[RSAP_DTE]);
    }
    
    if (destSap == NULL) {
	if (cls_debug_vdlc) {
	    buginf(".  Port enabled, but no active SAP");
	}
    	return (FALSE);
    }
    pak = clsBlock->fPakPtr;

    /*
     * Validate the opcode 
     * Only certain ones go to the partner SAP
     */
     
    isResponse = FALSE;
    
    switch (dlcOpcode) {
    	default:
	    return (FALSE);
	    
	    
	case DLC_TEST_RSP:
	    isResponse = TRUE;
	    /* Fall through to the next case */
	case DLC_TEST_REQ:
	    clsOpcode = CLS_TEST_STN_IND;
	    control = LLC1_TEST;
	    break;
	    
	case DLC_ID_RSP:
	    isResponse = TRUE;
	    /* Fall through to the next case */
	case DLC_ID_REQ:
	    clsOpcode = CLS_ID_STN_IND;
	    control = LLC1_XID;
	    break;
	    
	case DLC_CONNECT_REQ:
	    clsOpcode = CLS_CONNECT_STN_IND;
	    
	    /* What a drag; we have to invent or populate a packet,
	     * just to get the point across, since packets is the only
	     * way to convey a message in to the DLU
	     */
	    if (pak == NULL) {
	    	pak = getbuffer(0);
		if (pak == NULL) {
	    	    return (FALSE);
		}
		clsBlock->fPakPtr = pak;
	    }
	    control = SABME;
	    break;
	    
	case DLC_UDATA_REQ:
	    clsOpcode = CLS_UDATA_STN_IND;
	    control = LLC1_UI;
	    break;
	}
    
    if (pak== NULL) {
	return (FALSE);
    }

    /*
     * Build a decent encaps for the packet
     */

    pak->info_start = pak->datagramstart - SAP_HDRBYTES;
    pak->datagramsize += SAP_HDRBYTES;

    llc1 = (sap_hdr *)(pak->info_start);
    llc1->dsap = dtestring[RSAP_DTE];
    llc1->ssap = dtestring[LSAP_DTE];
    llc1->control = control;
    
    pak->addr_start = pak->info_start - (IEEEBYTES + IEEEBYTES);
    pak->datagramsize += (IEEEBYTES + IEEEBYTES);
    pak->datagramstart = pak->addr_start;
    pak->rif_start = NULL;
    pak->riflen = 0;

    ieee_copy(&dtestring[RMAC_DTE], &pak->addr_start[0]);
    ieee_copy(&dtestring[LMAC_DTE], &pak->addr_start[IEEEBYTES]);

    /*
     * This is it.
     * 
     * In all cases we need to provide enough information
     * for the SAP to know how to issue a Request Open Station
     * Request later on.
     *
     * This means making sure that the information is laid in the
     * packet accurately enough to set up correct pointers when
     * it's cep-building time
     *
     * At this point continue to exploit the fact that LlcUDataStnT,
     * LlcConnectStnT, LlcTestStn, and LlcXidStnT exactly 
     * overlap each other
     *
     * THIS IS WRONG BUT WHAT'S BROKEN IS THE DUPLICATION OF STRUCTRES
     * PHIL
     */

    dlcIdStn = &clsBlock->fParameters.fDlcIdStn;
    llcIdStn = &dlcIdStn->fDlcXid.fLlcXidStn;

    /* 
     * give CLS parameters for future req open stn 
     * Turning things around as we go
     */

    llcIdStn->fMacType = STATION_IEEE48;
    llcIdStn->fLMacLength = IEEEBYTES;
    llcIdStn->fLMacPtr = pak->addr_start;
    llcIdStn->fRMacLength = IEEEBYTES;
    llcIdStn->fRMacPtr = pak->addr_start + IEEEBYTES;
    
    llcIdStn->fRSap = llc1->ssap;
    llcIdStn->fLSap = llc1->dsap;
    
    llcIdStn->fRoutingLength = pak->riflen;
    llcIdStn->fRoutingPtr = pak->rif_start;

    dlcIdStn->fXIDLength =
      pak->datagramsize - (pak->network_start - pak->datagramstart);
    dlcIdStn->fXIDPtr 		= pak->network_start;
    dlcIdStn->fXidT     	= isResponse  ? TEST_RESPONSE: TEST_COMMAND;

    dlcIdStn->fClsUCepId = (ClsUCepIdT)CLS_NO_CEP;
    dlcIdStn->fDlcPCepId = CLS_NO_CEP;
    dlcIdStn->fClsUSapId = VSapGetUSapId(destSap);
    dlcIdStn->fDlcPSapId = (DlcPSapIdT)destSap;

    clsBlock->fResultError    	= CLS_OK;
    reg_invoke_cls_rput((ClsUCepIdT) CLS_NO_CEP, clsOpcode, clsBlock);
    return (TRUE);
}

/* VCepHandleTestResponse
 *
 * walker procedure to check if the cep is waiting for a Test Response
 * By the time the code gets here,  the SAPs have been verified, the local
 * mac address has been verified; it's only a matter of the Remote mac address
 * The walk never terminates
 */
 
static boolean VCepHandleTestResponse (VCepT *cep, va_list argptr)
{
    uchar	*remoteAddr;
    
    remoteAddr = va_arg(argptr, uchar *);
    
    if (VCepGetState(cep) == VDLC_OPENING &&
    	ieee_equal_sans_rii(remoteAddr, &cep->fDteString[RMAC_DTE])) {
	    VCepSetState(cep, VDLC_HALF_OPEN);
	    VCepTimerStart(cep, cep->fTimeout);
	    DlcReqOpenStnConf((DlcPCepIdT)cep, VCepGetUCepId(cep), CLS_OK);
	}
    return (TRUE);		/* continue the walk */
}

/* DlcToClsOpcode
 *
 * Convert dlc opcode (cls->dlc) to cls opcode (dlc->cls
 * the heart of vdlc
 */

static INLINE ClsOpcodeT DlcToClsOpcode (DlcOpcodeT dlcOpcode)
{
    switch (dlcOpcode) {
      default:			return(CLS_NO_OP);
      case DLC_ID_REQ:		return(CLS_ID_IND);
      case DLC_ID_RSP:		return(CLS_ID_IND);
      case DLC_ID_STN_REQ:	return(CLS_ID_IND);
      case DLC_TEST_REQ:	return(CLS_TEST_IND);
      case DLC_TEST_RSP:	return(CLS_TEST_IND);
      case DLC_TEST_STN_REQ:	return(CLS_NO_OP);	/* handled in VSapHandleMsgs */
      case DLC_UDATA_REQ:	return(CLS_UDATA_IND);
      case DLC_UDATA_STN_REQ:	return(CLS_UDATA_STN_IND);

      case DLC_CONNECT_REQ:	return(CLS_CONNECT_IND);
      case DLC_CONNECT_RSP:	return(CLS_CONNECT_CNF);
      case DLC_SIGNAL_STN_REQ:	return(CLS_NO_OP);	/* don't do anything for VDLC */

      case DLC_DISC_REQ:	return(CLS_DISC_IND);
      case DLC_DISC_RSP:	return(CLS_DISC_CNF);
    }
}

/* VdlcTellPartner
 *
 * Evaluates the opcode from CLS to DLC, and sets up the opposite
 * one.  Passes the message back to CLS.
 */
 
static void 
VdlcTellPartner (VCepT *cep,
		 DlcOpcodeT dlcOpcode,
		 ClsDlcParamBlockT *clsBlock)
{
    ClsOpcodeT		clsOpcode;
    ClsUCepIdT		uCep;
    ClsDlcParamBlockT	localClsBlock;

    clsOpcode = DlcToClsOpcode(dlcOpcode);
    
    if (cep != (VCepT *)CLS_NO_CEP) {
    	uCep = VCepGetUCepId(cep);
	VCepSetStateFromClsOpcode(cep, clsOpcode);
    } else {
    	uCep = (ClsUCepIdT)CLS_NO_CEP;
    }
    
    if (clsOpcode != CLS_NO_OP) {
	if (clsBlock == NULL) {		/* Just DISC-REQ, and DISC-RSP */
	    clsBlock = &localClsBlock;
	    clsBlock->fPakPtr = NULL;
	}
	clsBlock->fResultError = CLS_OK;
	reg_invoke_cls_rput(uCep, clsOpcode, clsBlock);
    }
    return;
}

/* VCepSetStateFromDlcOpcode and VCepSetStateFromClsOpcode
 *
 * Use opcodes to set new states in the CEP
 *
 * This is needed so that we can give a correct message
 * If the partner issues a CLOSE.STN.REQ
 * 
 * VCepSetStateFromDlcOpcode is for messages from CLS to VDLC
 * VCepSetStateFromClsOpcode is for messages from VDLC to CLS
 */
 
static void VCepSetStateFromDlcOpcode (VCepT *cep, DlcOpcodeT opcode)
{
    switch (opcode) {
    default:
        break;		/* no reason to set the opcode */
    case DLC_ID_REQ:
    	VCepSetState(cep, VDLC_XID_RSP_PENDING);
        VCepTimerStart(cep, cep->fTimeout);
        cep->fRetryCount = 0;
	break;
    case DLC_CONNECT_REQ:
    	VCepSetState(cep, VDLC_RX_CONNECT_REQ);
	break;
    case DLC_CONNECT_RSP:
    	VCepSetState(cep, VDLC_CONNECTED);
	break;
    case DLC_DISC_REQ:
    	VCepSetState(cep, VDLC_RX_DISC_REQ);
	break;
    case DLC_DISC_RSP:
    	VCepSetState(cep, VDLC_OPEN);
	break;     

    }
}

static void VCepSetStateFromClsOpcode (VCepT *cep, ClsOpcodeT opcode)
{
    switch (opcode) {
    default:
        break;		/* no reason to set the opcode */
    case CLS_CONNECT_IND:
    	VCepSetState(cep, VDLC_TX_CONNECT_IND);
	break;
    case CLS_CONNECT_CNF:
    	VCepSetState(cep, VDLC_CONNECTED);
	break;
    case CLS_DISC_IND:
    	VCepSetState(cep, VDLC_TX_DISC_IND);
	break;
    case CLS_DISC_CNF:
    	VCepSetState(cep, VDLC_OPEN);
	break;     
    }
}

/* VCepHandleClosingMsgs
 *
 * Partner has already issued a CLOSE STN REQ
 * This cep is in CLOSING state
 */
 
static INLINE void 
VCepHandleClosingMsgs (VCepT *cep, DlcOpcodeT opcode,
				 ClsDlcParamBlockT *clsBlock)
{
    switch (opcode) {
    default:
	break;
	
    case DLC_ID_REQ:
	datagram_done(clsBlock->fPakPtr);
	VCepClsNotify(cep, CLS_ID_IND, CLS_NO_XID_RSP, NULL);
	break;
	
    case DLC_ID_RSP:
    case DLC_UDATA_REQ:
    case DLC_TEST_REQ:
    case DLC_TEST_RSP:
	datagram_done(clsBlock->fPakPtr);
	break;
		
    case DLC_CONNECT_REQ:
	VCepClsNotify(cep, CLS_CONNECT_CNF, CLS_NO_REM_STN, NULL);
	break;
	
    case DLC_CONNECT_RSP:
	/* 
	 * If this happens we must have sent CONNECT_IND, so
	 * when the partner closes we already send DISC_IND,
	 * There's nothing left to do
	 */
    case DLC_SIGNAL_STN_REQ:
	/* 
	 * If this happens it means that we were in state CONNECTED
	 * So we already sent DISC IND to the DLU
	 */
	break;

    case DLC_DISC_REQ:
	/*
	 * Must have crossed with a DISC_IND
	 */
	VCepClsNotify(cep, CLS_DISC_CNF, CLS_OK, NULL);
	break;
		     	
    case DLC_DISC_RSP:
        datagram_done(clsBlock->fPakPtr);
	break;

    }
    return;
}

static INLINE  void 
VCepModifyStnReq (VCepT *cep, ClsDlcParamBlockT *clsBlock)
{
}

static INLINE  void VCepDataReq (VCepT *cep, ClsDlcParamBlockT *clsBlock)
{
    uchar       *dtestring;
    
    paktype		*pak;

    CLSASSERT(cep != (VCepT *)CLS_NO_CEP);

    if (VCepGetLlc(cep)) {		/* local case */
    	llc_wput((DlcPCepIdT)VCepGetLlc(cep), DLC_DATA_REQ, clsBlock);
	return;
    }
    if (VCepGetState(cep) != VDLC_CONNECTED) {
    	datagram_done(clsBlock->fPakPtr);
	return;
    }
    if (!cep->fPartner) {
	VCepNoPartner(cep);
	return;
    }
    if (cls_debug_vdlc) {
	buginf("\nVDLC: DATA ");
	VPrintDteString(VCepGetDteString(cep));
    }
    dtestring = VCepGetDteString(cep);
    if ((dtestring[LSAP_DTE] == SAP_IBMNM) ||
	(dtestring[RSAP_DTE] == SAP_IBMNM)) {
	clsBlock->fPakPtr->network_start = 
	clsBlock->fPakPtr->datagramstart;
    }
    pak = clsBlock->fPakPtr;
    p_enqueue(&cep->fTxQueue, pak);
    CepFlowControlInd(VCepGetUCepId(cep), VCepGetDlcCep(cep));
    VCepBgEnqueue(cep->fPartner);
    return;
}


static INLINE  void VCepFlowReq (VCepT *cep, ClsDlcParamBlockT *clsBlock)
{
    CLSASSERT(cep != (VCepT *)CLS_NO_CEP);

    if (VCepGetLlc(cep)) {		/* local case */
    	llc_wput((DlcPCepIdT)VCepGetLlc(cep), DLC_FLOW_REQ, clsBlock);
	return;
    }
    if (VCepGetState(cep) != VDLC_CONNECTED) {
	return;
    }
    if (cep->fPartner) {
	if (cls_debug_vdlc) {
	    buginf("\nVDLC: FlowReq QUENCH %s ", 
		   (clsBlock->fParameters.fResumeFlow) ? "OFF" : "ON");
	    VPrintDteString(VCepGetDteString(cep));
	}
	cep = cep->fPartner;
	if (VCepGetState(cep) != VDLC_CLOSING) {
	    CepFlowControlInd(VCepGetUCepId(cep), VCepGetDlcCep(cep));
	}
	return;
    }
    VCepNoPartner(cep);
    return;
}

static void VCepPongReq (VCepT *cep, ClsDlcParamBlockT *clsBlock)
{
    CLSASSERT(cep != (VCepT *)CLS_NO_CEP);

    if (VCepGetLlc(cep)) {		/* local case */
    	llc_wput((DlcPCepIdT)VCepGetLlc(cep), DLC_PONG_REQ, clsBlock);
	return;
    }
    if (VCepGetState(cep) != VDLC_CONNECTED) {
	return;
    }
    if (cep->fPartner) {
	if (cls_debug_vdlc) {
	    buginf("\nVDLC: Pong Request");
	    VPrintDteString(VCepGetDteString(cep));
	}
	CepPongRequest(VCepGetUCepId(cep), VCepGetDlcCep(cep));
	return;
    }
    VCepNoPartner(cep);
    return;
}

static void VdlcWput (DlcPCepIdT pCepId, DlcOpcodeT opcode,
	       ClsDlcParamBlockT *clsParamBlock)
{
    VCepT	*cep;

    cep = (VCepT *)pCepId;
    clsParamBlock->fResultError = CLS_OK;
    switch (opcode)
    {
    case DLC_INIT_SYSTEM_REQ:
	VdlcStart((VCepT *)pCepId, clsParamBlock); 
	break;
	
    case DLC_ENABLE_REQ:
	VdlcEnableReq((VCepT *)pCepId, clsParamBlock); 
	break;

    case DLC_DISABLE_REQ:
	VdlcDisableReq((VCepT *) pCepId, clsParamBlock); 
	break;

    case DLC_ACTIVATE_SAP_REQ:
	VPortActivateSapReq((VCepT *)pCepId, clsParamBlock); 
	break;

    case DLC_MODIFY_SAP_REQ:
	VPortModifySapReq((VCepT *)pCepId, clsParamBlock); 
	break;

    case DLC_DEACTIVATE_SAP_REQ:
	VPortDeactivateSapReq((VCepT *)pCepId, clsParamBlock); 
	break;

    case DLC_ACTIVATE_RING_REQ:
	VPortActivateRngReq((VCepT *)pCepId, clsParamBlock); 
	break;

    case DLC_DEACTIVATE_RING_REQ:
	VPortDeActivateRngReq((VCepT *)pCepId, clsParamBlock); 
	break;

    case DLC_REQ_OPNSTN_REQ:
	VSapReqOpnStnReq((VCepT *)pCepId, clsParamBlock); 
	break;

    case DLC_ID_STN_REQ:
    case DLC_ID_STN_RSP:
    case DLC_TEST_STN_REQ:
    case DLC_TEST_STN_RSP:
    case DLC_UDATA_STN_REQ:
	VSapHandleMsgs((VCepT *)pCepId, opcode, clsParamBlock);
	break;
	
    case DLC_CLOSE_STN_REQ:
	VCepCloseStnReq((VCepT *)pCepId, clsParamBlock); 
	break;

    case DLC_MODIFY_STN_REQ:
	VCepModifyStnReq((VCepT *)pCepId, clsParamBlock); 
	break;

    /*
     * These messages all have a cep context, so they don't need
     * to modify any information in the Cls-Dlc parameter block.
     */

    case DLC_ID_REQ:
    case DLC_ID_RSP:
    case DLC_TEST_REQ:
    case DLC_TEST_RSP:
	clsParamBlock->fParameters.fDlcId.fXIDPtr =
	    clsParamBlock->fPakPtr->datagramstart;

	clsParamBlock->fPakPtr->network_start =
	    clsParamBlock->fPakPtr->datagramstart;

	clsParamBlock->fParameters.fDlcId.fXIDLength =
	    clsParamBlock->fPakPtr->datagramsize;

	VCepHandleMsgs((VCepT *)pCepId, opcode, clsParamBlock);
	break;

    case DLC_CONNECT_RSP:
	VCepClsNotify((VCepT *)pCepId, CLS_CONNECTED_IND, CLS_OK, NULL);
	/* Fall through to the next case */

    case DLC_CONNECT_REQ:
       memset(clsParamBlock, 0, sizeof(ClsDlcParamBlockT));
	/* Fall through to the next case */

    case DLC_DISC_REQ:
    case DLC_DISC_RSP:
    case DLC_UDATA_REQ:
    case DLC_SIGNAL_STN_REQ:
	VCepHandleMsgs((VCepT *)pCepId, opcode, clsParamBlock); 
	break;

    case DLC_DATA_REQ:
	VCepDataReq((VCepT *)pCepId, clsParamBlock); 
	break;

    case DLC_PONG_REQ:
	VCepPongReq((VCepT*) pCepId, clsParamBlock); 
	break;

    case DLC_FLOW_REQ:
	VCepFlowReq((VCepT *)pCepId, clsParamBlock); 
	break;

    default:
	CLSASSERT(!"vdlc_wput: Invalid opcode\n");
    }
}

/* VCepClsNotify
 *
 * Tell CLS all about it
 */

static boolean VCepClsNotify (VCepT *cep, ClsOpcodeT opcode,
				     ClsErrorCodeT error, paktype *pak)
{
    ClsDlcParamBlockT 	clsDlcParamBlock;
    ClsUCepIdT		uCepId;
    
    uCepId = VCepGetUCepId(cep);
    if (uCepId == NO_CLS)
	return (FALSE);

    memset(&clsDlcParamBlock, 0, sizeof(ClsDlcParamBlockT));

    /* Handle special Op Code not handled by generic BuildParam Block */
    VCepBuildParamBlock(cep, opcode, &clsDlcParamBlock, error);
    DlcBuildParmBlock((DlcPCepIdT)cep, uCepId, opcode, &clsDlcParamBlock, 
		      error, pak);
    reg_invoke_cls_rput(uCepId, opcode, &clsDlcParamBlock);
    return (TRUE);
}

/* VCepBuildParamBlock
 * 
 * Helper routine to build up the parameter block prior 
 * to calling cls_rput.
 *
 * Most of the action codes are handled by 
 * VSapHandleMsgs and VCepHandleMsgs
 *
 * So there's very little to be handled here
 */

static ClsDlcParamBlockT* VCepBuildParamBlock (VCepT *cep,
					       ClsOpcodeT opcode,
					       ClsDlcParamBlockT *paramBlock,
					       ClsErrorCodeT errorCode)
{
    paramBlock->fResultError = errorCode;
    paramBlock->fPakPtr = NULL;

    switch (opcode) {

    case CLS_CLOSE_STN_CNF:
    case CLS_DATA_IND:
    case CLS_ID_IND:

	/* From VCepProcessTimerEvent */
    case CLS_CONNECT_CNF:	
	break;			/* the only ones with real code */

    case CLS_NO_OP:
    case CLS_ENABLE_CNF:
    case CLS_DISABLE_CNF:
    case CLS_ACTIVATE_SAP_CNF:
    case CLS_MODIFY_SAP_CNF:
    case CLS_DEACTIVATE_SAP_CNF:
    case CLS_REQ_OPNSTN_CNF:
    case CLS_TEST_IND:

    case CLS_CONNECT_IND:
    case CLS_TEST_STN_IND:
    case CLS_CONNECT_STN_IND:
    case CLS_ID_STN_IND:
    case CLS_ACTIVATE_RING_CNF:
    case CLS_DEACTIVATE_RING_CNF:
    case CLS_UDATA_STN_IND:
    case CLS_UDATA_IND:
    case CLS_FLOW_IND:
    case CLS_PONG_CNF:
	CLSASSERT(!"Illegal Cls OP Code");
	break;				/* handled elsewhere */

    case CLS_MODIFY_STN_CNF:
	paramBlock->fParameters.fDlcConnect.fPPortId = 
	    VPortGetIdb(VSapGetPort(VCepGetSap(cep)));
	paramBlock->fParameters.fDlcConnect.fDlcPCepId = (DlcPCepIdT)cep;
	paramBlock->fParameters.fDlcConnect.fClsUCepId = VCepGetUCepId(cep);

#ifdef PHIL_DO_ME_LATER
	paramBlock->fParameters.fDlcConnect.fLlcRetCode = cep->fAdmReason;
#endif
	break;

    case CLS_DISC_IND:
    case CLS_DISC_CNF:
	paramBlock->fParameters.fDlcDisconnect.fPPortId = 
	    (idbtype *)VPortGetIdb(VSapGetPort(VCepGetSap(cep)));
	paramBlock->fParameters.fDlcDisconnect.fDlcPCepId = (DlcPCepIdT)cep;
	paramBlock->fParameters.fDlcDisconnect.fClsUCepId = VCepGetUCepId(cep);
	
#ifdef PHIL_DO_ME_LATER
	paramBlock->fParameters.fDlcDisconnect.fLlcRetCode = cep->fAdmReason;
#endif
	break;

    case CLS_CONNECTED_IND:
        /* 
         * Not sure what it should do 
	 *   but receipt of CLS_CONNECTED_IND is legal 
	 *
	 * Prevent Illegal Cls OP Code traceback for now      --ppearce
         *  
         */
        break;

    case CLS_DATA_CNF:
	/* need something here for error reply to DLC_DATA_REQ */
	break;
    }
    return (paramBlock);
}

/*
 * Read from the RQ and send it up the appropriate DLU
 */

static process vdlc_background (void)
{
    VCepT		*cep, *partner;
    paktype		*pak;

    process_wait_on_system_init();

    process_watch_mgd_timer(&VdlcMasterTimer, ENABLE);
    process_watch_boolean(VdlcBgList_look, ENABLE, RECURRING);
    
    while(1) {
	process_wait_for_event();                     /* Any event */

	while ((cep = list_dequeue(&VdlcBgList)) != NULL) {
	    pak = VCepGetRxPacket(cep);
	    if (pak == NULL) {
	    	continue;	/* shouldn't happen ... */
	    }
	    VCepClsNotify (cep, CLS_DATA_IND, CLS_OK, pak);
	    if (VCepRxQueueEmpty(cep)) {
	    	partner = VCepGetPartner(cep);
		if (partner != NULL) {
		    CepFlowControlInd(VCepGetUCepId(partner), 
				      VCepGetDlcCep(partner));
		    continue;
		}
	    }
	    /* cep's Rx queue not empty, put cep back on the list */
	    list_enqueue(&VdlcBgList, &cep->fLink, cep);
	}
        process_set_boolean(VdlcBgList_look, FALSE);
	/*
	 * Handle background timers
	 */
	while (mgd_timer_expired(&VdlcMasterTimer)) {
	    mgd_timer	*vTimer;

	    vTimer = mgd_timer_first_expired(&VdlcMasterTimer);
	    mgd_timer_stop(vTimer);  /* Make it go away! */
	    cep = mgd_timer_context(vTimer);

	    switch (mgd_timer_type(vTimer)) {
	    case VDLC_TIMER:
		VCepEventInput(cep, VCEP_TIMER);
		break;
	    }
	}
    }
}

/* VdlcGetBridgingSapId
 * 
 * Service provided to dlsw_work
 *
 * This is used to provide a Cls Core Level Sap ID
 * given a (virtual) interface with is of type VDLC
 */

ClsUSapIdT VdlcGetBridgingSapId (hwidbtype *hwidb) 
{ 
    idbtype             *swidb;
    tr_vgrouptype	*vrg;		/* the Ring Group */
    VrgT		*vdlc;		/* VDLC extensions */

    if (hwidb == NULL) {
	return(NULL);
    }
    swidb = hwidb->firstsw;
    vrg = vrfind_ringgroup(srbV.srb_sb_get_targetring(swidb));
    if (vrg == NULL) {
	return(NULL);
	}
    vdlc = vrg->vr_vdlc;
    if (vdlc == NULL) {
	return(NULL);
    }
    if (vdlc->fBridgingSap == NULL) {
	return(NULL);
    }
    return  (VSapGetUSapId(vdlc->fBridgingSap));
}

/* VCepPrState
 * 
 * Display for debugging
 */

static char *const VdlcStateNames[VDLC_NUM_STATES] = {
    "IDLE",
    "NO_TEST",
    "OPENING",
    "HALF_OPEN",
    "OPEN",
    "XID_RSP_PENDING",
    "RX_CONNECT_REQ",		/* from DLU */
    "TX_CONNECT_IND",		/*   to DLU */
    "RX_DISC_REQ",		/* from DLU */
    "TX_DISC_IND",		/*   to DLU */
    "CONNECTED",		/* Connected, CONN.REQ/CONN.IND - CONN.RSP/CONN.CNF completed */
    "CLOSING",			/* Partner is closed, awaiting closestn.req from dlu */
} ;

#define MAXSTRING 30

static char *VCepPrState (VdlcStateT state)
{
    static char output[MAXSTRING];

    if (state >=  VDLC_NUM_STATES) {
	sprintf(output,"?%d?", state);
	return(output);    
    }
    return(VdlcStateNames[state]);
}

/* VPrintDteString
 *
 * Display for debug/logging
 */

static void VPrintDteString (uchar *dtestring)
{
    buginf("%e(%02x)->%e(%02x) ", 
	   &dtestring[LMAC_DTE], dtestring[LSAP_DTE],
	   &dtestring[RMAC_DTE], dtestring[RSAP_DTE]);
}

/************************************************************************
 *                                                                      *
 * Vrg									*
 *									*
 * Extensions to the Ring Group for VDLC's purposes			*
 *                                                                      *
 ************************************************************************
 */


/* VdlcDefaultPort
 *
 * Initialize the default port - this is done so that there's an IDB
 * to pass up to DLSw+ via set_srb_params
 */

static void VdlcDefaultPort (tr_vgrouptype *vrg, uchar *vmac, boolean sense)
{
    VrgT		*vdlc;
    VPortT		*port;

    if (!reg_invoke_dlsw_running(0,0,0)) {
	return;				/* not needed - not dlsw */
    }
    if (vrg->vr_vdlc == NULL) {
	vrg->vr_vdlc = VrgCreate(vrg);
    }
    vdlc = vrg->vr_vdlc;
    if (vdlc == NULL) {
	return;				/* Need the ring group extension */
    }
    if (vdlc->fBridgingSap != NULL && sense) {
	return;				/* If the bridging SAP is already there */
    }
    port = VrgGetDefaultPort(vdlc);

    if (sense) {
        if (port == NULL) {
            port = VPortCreate(vmac, vdlc);
            vdlc->fDefaultPort = port;
            if (port != NULL) {
                VrgAddPort(vdlc, port);
            }
        }
    }

    if (port == NULL) {
        return;
    }
    if (vmac == NULL) {
	ieee_zero(vrg->vr_vmac);
    } else {
	ieee_copy(vmac, vrg->vr_vmac);
    }
    reg_invoke_srt_set_srb_params(hwidb_or_null(VPortGetIdb(port)), sense);
    return;
}

/* VrgCreate
 *
 * Create the VDLC Extension to the ring group
 *
 * This may go away if everything gets included in the ring group
 */
 
static VrgT *VrgCreate (tr_vgrouptype *rg)
{
    VrgT *vrg;
	
    vrg = malloc(sizeof(VrgT));
    if (vrg != NULL) {
	list_create(&vrg->fList, 0, NULL, 0);
	/*  create links bertween vrg extension and vrg */
	rg->vr_vdlc = vrg;
	vrg->fVrg = rg;
    }
    return (vrg);
}

/* VrgDestroy
 *
 * Destructor
 *
 * This requires some thought - if the ring group goes away it should
 * make the VrgT go away PHIL
 */
#ifdef NOT_NEEDED
 
static void VrgDestroy (VrgT *vdlc)
{
    tr_vgrouptype	*vrg;
    VPortT		*port;
	
    vrg = vdlc->fVrg;
	
    while ((port =  list_dequeue(&vdlc->fList)) != NULL) {
	VPortDestroy(port);
    }
    if (vdlc->fBridgingSap != NULL) {	/* bridging SAP */
	VSapDestroy(vdlc->fBridgingSap);
    }
    free (vdlc);
    vrg->vr_vdlc = NULL;
}
#endif

/* VrgAddPort
 *
 * Insert a port into the list.
 * Maintain sorted order
 */

static boolean VrgAddPort (VrgT *vrg, VPortT *port)
{	
    return (list_insert(&vrg->fList, (list_element *)port, port, VPortCompareMac));
}

static VrgT *VdlcLookupVrgByPortId (CLSPortID* portID)
{
    idbtype		*idb;
    tr_vgrouptype	*vrg;

    idb = DlcToIdb(portID);
    if (idb == NULL) {
	return(NULL);
    }

    vrg = vrfind_ringgroup(srbV.srb_sb_get_targetring(idb));
    if (vrg == NULL) {
	return(NULL);
    }
    return (vrg->vr_vdlc);
}

static VPortT *VrgLookupPortByPortId (CLSPortID* portID)
{
    VrgT		*vrg;
    idbtype		*idb;
    VPortT		testPort;

    idb = DlcToIdb(portID);
    if (idb == NULL) {
	return(NULL);
    }
   
    vrg = VdlcLookupVrgByPortId(portID);
    if (vrg == NULL) {
    	return(NULL);
    }
    testPort.fIdb = idb;

    return((VPortT *)list_lookup(&vrg->fList, (list_element *)&testPort, 
				 &testPort, VPortCompareIdb));
}

/************************************************************************
 *                                                                      *
 * Port                                                                 *
 *                                                                      *
 * Identified by Virtual Mac Address                                    *
 * Anchor for a list of SAPs                                            *
 *                                                                      *
 ************************************************************************
 */

/* Methods */

/* Constructor */

static VPortT *VPortCreate (uchar *vmac, VrgT *vrg)
{
    VPortT 		*port;
    hwidbtype 		*hwidb;

    port = malloc(sizeof(VPortT));
    if (port != NULL) {
	list_create(&port->fList, 0, NULL, 0);
	VPortSetVrg(port, vrg);
	VPortSetVmac(port,vmac);
	port->fIdb = vidb_malloc();
	if (port->fIdb == NULL) {
	    free(port);
	    return (NULL);
	}
	/*
	 * Do some things to make our virtual interface look
	 * sufficiently real
	 */
	hwidb = port->fIdb->hwptr;
	hwidb->parse_packet = tring_parse_packet;
	hwidb->extract_hwaddr = ieee_extract_hwaddr;
	hwidb->extract_snpa = tr_extract_addr;
	hwidb->maxmtu =  VDLC_DEFAULT_MAXMTU;
	ieee_copy(vmac, hwidb->hardware);
	hwidb->status = IDB_VIRTUAL;
        srbV.srb_sb_set_targetring(port->fIdb, vrg->fVrg->vrn);
	sprintf(port->fName, "VDLC%d", vrg->fVrg->vrn);
	hwidb->hw_namestring = hwidb->hw_short_namestring =
	    port->fIdb->namestring = port->fName;

    }
    return(port);
}

#ifdef NOT_NEEDED
/* destructor */

static void VPortDestroy (VPortT *port)
{
    VSapT *sap;
    while ((sap = list_dequeue(&port->fList)) != NULL) {
	VSapDestroy(sap);
    }
    free(port);
}

#endif

/* VPortGetRingReq
 * 
 * Provide a Activate Ring Request CLSI Message at Port Enable Confirm
 */

TCLSIMsg *VPortGetRingReq(idbtype *swidb)
{
    TCLSIMsg		*theMsg;

    theMsg =  CLSIMsgCreateHeader(CLS_ACTIVATE_RING | CLS_REQUEST,
				  offsetof(ActRngReq_t,fParms) -
				  CLS_ACT_RING_REQ_HDR_SZ +
				  sizeof(ActRngVDLCParms_t),
				  CLS_LLC_LAYER,
				  CLS_ENTITY_MGR,
				  0);
    if (theMsg == NULL) {
        printf(nomemory);
    }
    return (theMsg);
}


/* VPortCompareMac
 *
 * to maintain a sorted order
 */
 
static int VPortCompareMac (void *runner, void *port)
{
    return (ieee_compare(VPortGetVmac((VPortT *)runner),
			 VPortGetVmac((VPortT *)port)) );
}

/* VPortCompareIdb
 *
 * This isn't used for ordering, only for equality on searching
 */

static int VPortCompareIdb(void *portA, void *portB)
{
   if (VPortGetIdb((VPortT *)portA) == VPortGetIdb((VPortT *)portB)) {
   	return(0);
   }
   return(-1);
}

/************************************************************************
 *									*
 * SAP                                                                  *
 *                                                                      *
 * Service Access Point                                                 *
 *                                                                      *
 * VDLC SAPs are one byte quantities, just like LLC2 SAPs               *
 *									*
 ************************************************************************
 */

/* Methods */

static VSapT *VSapCreate (ClsUSapIdT uSapId, uchar sapValue, 
			  VPortT *port, VdlcPriorityT priority)
{
    VSapT	*sap;
	
    sap = malloc(sizeof(VSapT));
    if (sap != NULL) {
	VSapSetSapValue(sap, sapValue);
	VSapSetPort(sap, port);
	VSapSetUSapId(sap, uSapId);
	VSapSetPriority(sap, priority);
    }
    return(sap);
}

static void VSapDestroy (VSapT *sap)
{
    VCepT	*cep;
	
    while((cep = VSapGetFirstCep(sap)) != NULL) {
	VSapRemoveCep(sap, cep);
	VCepDestroy(cep);
    }
    free (sap);
    return;
}

/* VSapCompareUSapId
 *
 * This isn't used for ordering, only for equality on searching
 */

static int VSapCompareUSapId (void *sapA, void *sapB)
{
    if (VSapGetUSapId((VSapT *)sapA) == VSapGetUSapId((VSapT *)sapB)) {
    	return(0);
    }
    return (-1);
}

/* VSapCompareValue
 *
 * Howver, this is used for ordering
 */

static int VSapCompareValue (void *sapA, void *sapB)
{
    return (VSapGetSapValue((VSapT *)sapA) - 
	    VSapGetSapValue((VSapT *)sapB));
}

/* VCepMatchUCepId
 *
 * test procedure for VSapLookupCepByUCepId
 */

static boolean VCepMatchUCepId (VCepT *cep, va_list argptr)
{
    ClsUCepIdT		uCepId;
    VCepT 		**testCep;

    uCepId = va_arg(argptr, ClsUCepIdT);

    if (VCepGetUCepId(cep) == uCepId) {
	testCep = va_arg(argptr, VCepT **);
	*testCep = cep;
	return (FALSE);			/* end the walk */
    }
    return (TRUE);			/* continue the walk */
}

/* VSapLookupCepByUCepId
 *
 * Walk the Cep list till we match, or return NULL
 */
  
static VCepT *VSapLookupCepByUCepId(VSapT *sap, ClsUCepIdT uCep)
{
	VCepT *cep;
	
	cep = (VCepT *)CLS_NO_CEP;
	avl_walk(sap->fCepList, (avl_walker_type)VCepMatchUCepId,
		 uCep, &cep);
	return(cep);
}


/*
 * Build a Test or XID Station Indication and pass it into the 
 * sap.
 *
 * Input parameters
 * Dest. Sap
 * Src Cep
 * Control
 *
 */

static boolean VSapSendUControlStnInd (VSapT *sap, VCepT *cep, uchar control)
{
    paktype 		*pak;
    ClsDlcParamBlockT	pb;
    hwidbtype		*hwidb;
    uchar		*dtestring;
    uchar		dsap;

    hwidb = hwidb_or_null(VPortGetIdb(VSapGetPort(VCepGetSap(cep))));
    dtestring = VCepGetDteString(cep);

    CLSASSERT(hwidb);
    if (hwidb == NULL) {
	return(FALSE);
    }

    if (control == LLC1_TEST) {
	dsap = SAP_NULL;
    } else {
	dsap = dtestring[RSAP_DTE];
    }
    if (cls_debug_vdlc) {
	buginf("\nVDLC: Send UFrame %02x: %e(%02x)->%e(%02x)", control, 
	       &dtestring[LMAC_DTE],  dtestring[LSAP_DTE], 
	       &dtestring[RMAC_DTE], dsap);
    }
    pak = Llc1GenUFrame(&dtestring[RMAC_DTE], &dtestring[LMAC_DTE], 
			dsap, dtestring[LSAP_DTE],
			 control, hwidb); 
    if (pak == NULL) {
	return(FALSE);
    }
    /* set up offsets */
    (*hwidb->parse_packet)(hwidb, pak,NULL);

    /* build and the paramblock to CLS */
    DlcSendUControlStnInd(&pb, pak, control, FALSE, 
    			  VSapGetUSapId(sap), (DlcPCepIdT)CLS_NO_CEP);
    return(TRUE);
}

/*
 * Perform UDataStnReq Encapsulation
 */
static void vdlcUDataStnReq(ClsDlcParamBlockT *clsBlock,
		            DlcUdataStnT *dlcUdataStn,
		            LlcUdataStnT *llcUdataStn)
{                     
    sap_hdr             *llc1;
    paktype             *pak;
    trrif_t 	        *trframe;

    pak = clsBlock->fPakPtr;
    trframe = (trrif_t *)(pak->network_start - TRING_ENCAPBYTES -
                          SAP_HDRBYTES - dlcUdataStn->fRifLength);
    pak->info_start = (uchar *) trframe;
    pak->datagramstart = (uchar *) trframe;
    pak->addr_start = (uchar *) (pak->network_start -
			        (TRING_ENCAPBYTES - 2) - SAP_HDRBYTES
			         - dlcUdataStn->fRifLength);
    pak->rif_start = (uchar *)(pak->network_start - SAP_HDRBYTES -
			       dlcUdataStn->fRifLength);
    pak->riflen = dlcUdataStn->fRifLength;
    bcopy(dlcUdataStn->fRif, pak->rif_start, dlcUdataStn->fRifLength);
    trframe->ac = AC_PRI0;
    trframe->fc = FC_TYPE_LLC;
    ieee_copy(dlcUdataStn->fRMac, trframe->daddr);
    ieee_copy(dlcUdataStn->fLMac, trframe->saddr);
    llc1 = (sap_hdr *)(pak->network_start - SAP_HDRBYTES);
    llc1->dsap = dlcUdataStn->fRSap;
    llc1->ssap = dlcUdataStn->fLSap;
    llc1->control = LLC1_UI; /* assume cmd/poll */
    bcopy(clsBlock->fRawData, (uchar *)pak->network_start,
	  clsBlock->fLoadLength);
    pak->datagramsize = TRING_ENCAPBYTES + dlcUdataStn->fRifLength +
                        SAP_HDRBYTES + clsBlock->fLoadLength;
    dlcUdataStn->fUDataLength = clsBlock->fLoadLength;
    dlcUdataStn->fUDataPtr = pak->network_start;
    llcUdataStn->fMacType = STATION_IEEE48;
    llcUdataStn->fLMacLength = IEEEBYTES;
    llcUdataStn->fLMacPtr = pak->addr_start;
    llcUdataStn->fRMacLength = IEEEBYTES;
    llcUdataStn->fRMacPtr = pak->addr_start + IEEEBYTES;
    
    llcUdataStn->fRSap = llc1->dsap;
    llcUdataStn->fLSap = llc1->ssap;
    
    llcUdataStn->fRoutingLength = pak->riflen;
    llcUdataStn->fRoutingPtr = pak->rif_start;
}

/************************************************************************
 *									*
 * Cep                                                                  *
 *                                                                      *
 * Connection End Point                                                 *
 *									*
 ************************************************************************
 */

/* Methods */

static VCepT *VCepCreate (ClsUCepIdT uCepId, VSapT *sap, 
			  uchar *dtestring, VdlcPriorityT priority, ulong flags)
{
    VCepT	*cep;
	
    cep = malloc(sizeof(VCepT));
    if (cep != NULL) {
	VCepSetUCepId(cep, uCepId);
	queue_init(&cep->fTxQueue, 0);
	CepInit(VCepGetDlcCep(cep), &cep->fTxQueue, VDLC_DEFAULT_HOLDQ,
		VDLC_TXQ_LOW_WATER, VDLC_TXQ_HIGH_WATER,
		VDLC_CLS_FLOW_IND_DAMP);
	VCepSetSap(cep, sap);
	VCepSetState(cep, VDLC_IDLE);
	VCepSetPriority(cep, priority);
	VCepSetDteString(cep, dtestring);
	mgd_timer_init_leaf(&cep->fTimer, &VdlcMasterTimer, 
			    VDLC_TIMER, cep, FALSE);
	if (cls_debug_vdlc) {
	    buginf("\n: VDLC Create cep 0x%x ", cep);
	    VPrintDteString(dtestring);
	}
    }
    return(cep);
}

static void  VCepDestroy (VCepT *cep)
{
    VSapT		*sap;
    paktype 		*pak;
    VCepT		*partner;
    
    sap = VCepGetSap(cep);
    VCepTimerStop(cep);
    CLSASSERT(sap);
    VSapRemoveCep(sap,cep);

    if (cls_debug_vdlc) {
	buginf("\nVDLC: Destroy Cep 0x%x ", cep);
	VPrintDteString((VCepGetDteString(cep)));
    }

    partner = cep->fPartner;

    if (partner) {
	partner->fPartner = NULL;
	partner->fRxQueue = NULL;
    }
		
    while ((pak = p_dequeue(&cep->fTxQueue)) != NULL) {
	datagram_done(pak);
    }
    VCepBgRemove(cep);

    if (VCepGetLlc(cep)) {
	llc2_close(VCepGetLlc(cep));
    }
    free(cep);
    return;
}

/* VCepSendTest
 *
 * build and transmit a TEST - this only goes to the bridging
 * Start a timer
 * sap
 */
static boolean
VCepSendTest (VCepT *cep)
{
    VrgT	*vrg;

    vrg = VPortGetVrg(VSapGetPort(VCepGetSap(cep)));
    if (vrg == NULL) {
	return(FALSE);
    }
    if (vrg->fBridgingSap == NULL) {
	return(FALSE);
    }
    if (VSapSendUControlStnInd(vrg->fBridgingSap, 
			       cep, LLC1_TEST)) {
	VCepSetState(cep, VDLC_OPENING);
	VCepTimerStart(cep, cep->fTimeout);
    } else {
	VCepSetState(cep, VDLC_NO_TEST);
	VCepTimerStart(cep, VDLC_BUFFER_WAIT);
    }
    return(TRUE);
}

#ifdef NOT_NEEDED
/* VCepExploreLocal
 *
 * Propagate an explorer to all the local Token Ring interfaces
 */

static void VCepExploreLocal(VCepT *cep)
{
    paktype *pak;
    idbtype		*idb;
    hwidbtype		*hwidb;
    uchar		*dtestring;

    idb = VPortGetIdb(VSapGetPort(VCepGetSap(cep)));
    hwidb = hwidb_or_null(idb);
    dtestring = VCepGetDteString(cep);

    CLSASSERT(hwidb);
    if (hwidb == NULL) {
	return;
    }

    pak = llc1_gen_explr(&dtestring[RMAC_DTE], &dtestring[LMAC_DTE], 
			 dtestring[RSAP_DTE], dtestring[LSAP_DTE],
			 hwidb); 
    if (pak == NULL) {			/* wait for background process to*/
	VCepSetState(cep, VDLC_NO_TEST);
	return;
    }
    pak->if_input = idb;

    /* 
     * If this works what it does is to send a packet to both the 
     * dslw path, and to call vr_forward_explorer
     * I'll have to debug it to be sure --- PHIL
     */

    VCepSetState(cep, VDLC_OPENING);
    srb_enq(srb_explorerQ, pak);
    return;
}

#endif

static void VCepNoPartner (VCepT *cep)
{
    if (cls_debug_err) {
	buginf("VDLC: Partner missing ");
	VPrintDteString(VCepGetDteString(cep));
    }
}


/* reverse the direction to build a partner string */

static void VCepBuildPartnerDteString (uchar *dtestring, uchar *partnerString)
{
    bzero(partnerString, LLC_MAX_DTE_SIZE);
    ieee_copy(&dtestring[RMAC_DTE], &partnerString[LMAC_DTE]);
    ieee_copy(&dtestring[LMAC_DTE], &partnerString[RMAC_DTE]);
    partnerString[RSAP_DTE] = dtestring[LSAP_DTE];
    partnerString[LSAP_DTE] = dtestring[RSAP_DTE];
}


/* VCepCompare
 *
 * For maintaining sorted order
 */
static enum avl_compare
VCepCompare (avl_node_type *cepA, avl_node_type *cepB)
{
    int comp;

    comp = memcmp(VCepGetDteString((VCepT *)cepA), 
		  VCepGetDteString((VCepT *)cepB),
		  LLC_MAX_DTE_SIZE);
    if (comp < 0)
	return(AVL_LT);
    if (comp > 0)
	return(AVL_GT);
    return(AVL_EQ);

}

/* VCepSetState
 *
 * Enhanced: display the states and transitions
 */	

static void VCepSetState (VCepT *cep, VdlcStateT state)
{
    if (cep == NULL) {
	return;
    }
    if (VCepGetState(cep) != state) {
	if (cls_debug_vdlc) {
	    /* 
	     * Two assignements because bad things happen if
	     * VCepPrState returns unknown values in a static variable
	     * This would get overwritten if both state values are invalid
	     */

	    buginf("\nVDLC: ");
	    VPrintDteString(VCepGetDteString(cep));
	    buginf(": %s->%s", 
		   VCepPrState(VCepGetState(cep)),
		   VCepPrState(state));
	}
	cep->fState = state;
    }
}

static void VCepTimerStart (VCepT *cep, int interval)
{
   mgd_timer_start(&cep->fTimer, interval);
   return;
}

static void VCepTimerStop (VCepT *cep)
{
   mgd_timer_stop(&cep->fTimer);
   return;
}
