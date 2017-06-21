/* $Id: dlcllc.c,v 3.12.4.12 1996/08/30 00:42:50 kpatel Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlcllc.c,v $
 *------------------------------------------------------------------
 * dlcllc.c - Glue for converting from CLS to LLC
 *
 * March, 1994		Phil Morton
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Routines needed to map from CLS to existing LLC code base.
 * (Glue)
 *------------------------------------------------------------------
 * $Log: dlcllc.c,v $
 * Revision 3.12.4.12  1996/08/30  00:42:50  kpatel
 * CSCdi67085:  SEGV violation at 0x0 - Bad refcount in datagram_done
 *              Protect llc_xid_pak from datagram_done at interrupt level
 * Branch: California_branch
 *
 * Revision 3.12.4.11  1996/08/28  12:39:20  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.4.10  1996/08/26  15:04:27  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.12.4.9  1996/08/12  02:15:39  ppearce
 * CSCdi64450:  CSCdi59527 broke c4500-boot-m image
 * Branch: California_branch
 *   Add srb_core_registry that will definitely be included w/SRB subblock
 *
 * Revision 3.12.4.8  1996/08/10  01:03:17  athippes
 * CSCdi63699:  Adding a hidden knob - dlsw llc2 nornr
 * Branch: California_branch
 *
 * Revision 3.12.4.7  1996/07/25  14:12:30  fbordona
 * CSCdi64009:  %LNMC-3-BADCLSIRET: bogus Invalid ret code (0x7004)
 *              Adjust LNM for SRB sub-block changes.
 * Branch: California_branch
 *
 * Revision 3.12.4.6  1996/07/23  13:20:37  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.12.4.5  1996/07/09  05:48:17  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.12.4.4  1996/05/24  23:05:02  kchiu
 * CSCdi58704:  DLSW/FDDI enhancement breaks NCIA code
 * Branch: California_branch
 *
 * Revision 3.12.4.3  1996/05/17  10:42:29  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.6.12  1996/05/05  22:51:19  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.11.6.11  1996/05/02  22:46:54  kchiu
 * CSCdi56517:  Image build error for c1005 images in IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.6.10  1996/05/02  15:51:19  sbales
 * CSCdi51340:  Memory Leak DLSw on activation of PU2.1
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.6.9  1996/04/26  15:16:34  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.6.8  1996/04/15  00:08:26  kchiu
 * Branch: IbuMod_Calif_branch
 * Add dlsw+/fddi support
 *
 * Revision 3.11.6.7  1996/04/11  14:36:09  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.6.6  1996/04/05  06:05:33  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.11.6.5  1996/04/03  13:54:40  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.6.4  1996/03/28  08:19:37  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 * Revision 3.12.4.2  1996/04/25  23:03:33  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.12.4.1.2.1  1996/03/24  03:46:41  rzagst
 * ATM modularity: Remove AIP from all files that are not referring to the
 * AIP i/f
 * Branch: ATM_Cal_branch
 *
 * Revision 3.12.4.1  1996/03/18  19:10:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.4  1996/03/16  06:32:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.3  1996/03/13  01:14:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/03/07  08:41:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  00:22:02  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.12  1996/03/11  17:24:31  ppearce
 * CSCdi47301:  CLS/RSRB link activation may fail when LAK enabled
 *   Cannot depend on pak->peer_ptr to represent a valid peer entry
 *     use a different method to find valid peer entry
 *
 * Revision 3.11  1996/02/06  20:14:24  vrawat
 * CSCdi48218:  FRAS/BAN does not accept endstation Test-response
 *
 * Revision 3.10  1996/02/01  06:00:31  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.9  1996/01/26  16:52:13  dmccowan
 * CSCdi46491:  APPN & DSPU link act may fail when mult. link activated at
 * same time
 * Change SapSNAHandleTest to check address before sending ReqOpenStn(Cnf
 *
 * Revision 3.8  1996/01/23  18:07:51  dbilling
 * CSCdi46491:  APPN & DSPU link act may fail when mult. link activated
 * Change SapSNAHandleTest to check address before sending ReqOpenStn(Cnf)
 *
 * Revision 3.11.6.3  1996/03/17  17:36:54  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.11.6.2  1996/03/01  16:30:38  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.11.6.1  1996/02/28  21:23:44  ppearce
 * CSCdi50230:  IBU modularity - Remove unnecessary header file includes
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.2.1  1995/12/17  09:50:09  pmorton
 * Branch: Cal_Ibm_branch
 * VDLC Check-in.  This is the working base for all DLUs
 *
 * Revision 3.5  1995/12/04  22:25:28  jacobt
 * CSCdi42475:  FRAS memory leak when no rsp to XID. (code review)
 *
 * Revision 3.4  1995/12/04  03:52:22  jacobt
 * CSCdi42475:  FRAS memory leak when no rsp to XID.
 *
 * Revision 3.3  1995/11/17  08:55:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:14:18  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:11:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  20:53:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/09/21  23:59:36  dmccowan
 * CSCdi40832:  SNA XIDs may need additional encap bytes
 * use pak_dup_grow_realign() instead of pak_clip when processing XIDs
 *
 * Revision 2.10  1995/09/13  04:06:45  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.9  1995/08/26 01:29:33  lpereira
 * CSCdi39313:  router crashes at _datagram_out(0x607ac)+0x10 during clean
 * up
 *
 * Revision 2.8  1995/07/24  13:02:17  fbordona
 * CSCdi34580:  DLS show circuit displays HALT PENDING NOACK after
 *              successful disc
 *
 * Revision 2.7  1995/07/21  17:08:16  sberl
 * CSCdi37437:  Add new ModifyStn primitive to CLSI
 * Just set the right bits in fModifyFlags and put the new values in the
 * parameter structure.
 *
 * Revision 2.6  1995/07/06  21:46:19  sberl
 * CSCdi36777:  WFQ should count llc2 sessions as conversations
 * Add logic to classify LINK_LLC2 packets into LLC2 conversations.
 *
 * Revision 2.5  1995/07/02  05:15:35  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.4  1995/06/14  18:02:54  ravip
 * CSCdi34577:  dlsw session not established SAPs changed
 *              Handle XIDs to NULL SAP.
 *
 * Revision 2.3  1995/06/14  04:14:18  alam
 * CSCdi35813:  Enable CLS/LLC2 to use RIF over FDDI
 *
 * Revision 2.2  1995/06/09  19:27:06  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.1  1995/06/07  20:20:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../cls/msg_dlc.c"		/* Not a typo, see logger.h */
#include "cls_private.h"
#include "packet.h"
#include "config.h"
#include "../if/fddi.h"
#include "../if/rif.h"
#include "../if/network.h"
#include "../llc2/llc2.h"
#include "../llc2/lltimers.h"
#include "../llc2/llc2_inline.h"
#include "../llc2/llc2_sb.h"
#include "../llc2/parser_defs_llc2.h"
#include "address.h"
#include "../srt/rsrb.h"
#include "../srt/srb_core.h"
#include "../srt/srb.h"
#include "../srt/lack_externs.h"
#include "../srt/span.h"
#include "../if/priority.h"
#include "../srt/srb_core_registry.h"
#include "../srt/srt_registry.h"

/* local function declarations */
static clsLlcMapFnT llc2InitSystem(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2ActivateSapReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2ReqOpnStnReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2IdReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2ConnectReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2ConnectRsp(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2SignalStnReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2DataReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2FlowReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2DisconnectReq(llctype *llc,  ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2IdRsp (llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2TestReq (llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2TestRsp (llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llc2UDataReq (llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llcIdStnReq (llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llcIdStnRsp (llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llcTestStnReq (llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT llcTestStnRsp (llctype *llc, ClsDlcParamBlockT *clsBlock);

static boolean llcTestStnTxPak(ClsDlcParamBlockT *clsBlock, 
				      boolean cmd_resp_flag);
inline static boolean llcIdStnTxPak(ClsDlcParamBlockT *clsBlock,
                                      boolean cmd_resp_flag);
static void llcStnEncap(paktype *pak, uchar *src, uchar *dst,
				   sap_hdr *llc1, 
				   uchar *rif, ushort payload_len, 
				   uchar *payload_ptr);
static clsLlcMapFnT llcUDataStnReq (llctype *llc, ClsDlcParamBlockT *clsBlock);

static void SapSNAHandleTest(idbtype *idb, paktype *pak, boolean usesa);

static void SapSNAHandleIeeeXid(idbtype *idb, paktype *pak, boolean usesa);

static void clsLlcStoreRifROSReq(llctype *llc, ClsDlcParamBlockT *clsBlock);

static void llcReqOpenStnConf(llctype *llc, ClsUCepIdT uCepId, ClsErrorCodeT);
static boolean DlcFindRemote(llctype *llc, byte explorerOption);
static llctype *LlcNewLlctype(idbtype *idb, LlcReqOpenStnT* llcReqOpnParm);
static void DlcTxXidFail(llctype *llc, ClsErrorCodeT error);
static void DlcCancelXidInProgress(llctype *llc);

/* local inline function declarations */

static inline boolean llc2ClsNotify(llctype *llc, ClsOpcodeT opcode,
				    ClsErrorCodeT ClsDlcErrorCode);

/************************************************************************
 *			     CLSI Primitive: (none)
 ************************************************************************
 *
 * DLC Message Generated/Received
 * ------------------------------
 * llc_wput(*cep, DLC_INIT_SYSTEM, &parm_block)
 *
 * Code needs a mechanism to initialize global LLC2 structures, such as
 * hash tables and queues.  Parameter block will contain initialization
 * information such as:
 * 	llc2_hash size
 *
 *
 * Old Code Function
 * -----------------
 * llc2_init()
 *
 * Code initializes assiciated llc data structures such as:
 * 	llc hash table
 * 	internal queues
 * 	parser
 * 	further modular system initializations
 *
 *
 * Enhancements
 * ------------
 * For current code no change will be made to subsystem initialization.
 ************************************************************************
 */

static inline clsLlcMapFnT llc2InitSystem (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    /* llc2_init(); - currently called in system startup stuf */
    llc2_start();
    return;
}


/************************************************************************
 * 		   CLSI Primitive: Activate_sap.req
 *************************************************************************
 * Controls which SAP values we will respond.
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC2_ACTIVATE_SAP_REQ, &param_block)
 *
 *
 * Old Code Function
 * -----------------
 * Call llc2_register() to register passed SAP.
 *
 * Enhancements
 * ------------
 *
 * Accept options for responding back to different RIF exploration models.
 * Initially code will work as follows:
 * 	ARE or STE -->>
 * 		   <<--  SRF
 *
 * Store u_sap_id in llc2_registration[] array.
 **********************************************************************
 */
static inline  clsLlcMapFnT llc2ActivateSapReq (
	llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    idbtype		*idb;
    DlcActSapT		*dlcActSap;
    LlcActSapT		*llcActSap;
    sap_registry_t	*sapRegistry;
    sap_entry_t		*sapEntry;

    dlcActSap = &clsBlock->fParameters.fDlcActSap;
    llcActSap = &dlcActSap->fDlcSap.fLlcActSap;

    idb = (idbtype *)(dlcActSap->fPPortId);
    if (idb == NULL) {
	errmsg(&msgsym(OPTNULL, DLC));
	dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    sapRegistry = sap_get_registry(idb);
    if (sapRegistry == NULL) {
	dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_NO_SAP_REGISTRY);
	return;
    }

    sapEntry = sap_get_entry(idb, llcActSap->fLSapValue);
    if (sapEntry != NULL) {
	/* 
	 * Sap already registered. Check if it is preregistered for
	 * legacy code, or if it is in use by another DLU.
	 */
	if (sapEntry->sap_flags & SAP_PRE_REG_SAP) {
	    sap_deregister(sapRegistry, llcActSap->fLSapValue);
	    /* 
	     * sapEntries for preregistered saps are statically
	     * allocated! Do not dispose of the memory!!!
	     */
	} else {
	    dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_SAP_BUSY);
	    return;
	}
    }
	   
    sapEntry = malloc(sizeof(sap_entry_t));
    if (sapEntry != NULL) {
	int rc;
	
	sapEntry->sap_flags = llcActSap->fSapFlags | SAP_CLS_ACT_SAP;
	sapEntry->usapid = dlcActSap->fUSapId;
	sapEntry->llc_list = NULL;
	sapEntry->sap_handle_test = SapSNAHandleTest;
	sapEntry->sap_handle_ieee_xid = SapSNAHandleIeeeXid;
	sapEntry->sap_handle_sna_xid = SapSNAHandleSnaXid;
	sapEntry->xid_class = XID_CLASS_TYPE2;
	sapEntry->xid_window = XID_WIN_TYPE2;
	rc = sap_register(sapRegistry, llcActSap->fLSapValue, sapEntry);
	
	if (rc != 0) {            /* fails if SAP already in use */
	    dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_SAP_BUSY);
	    return;
	}
    } else {
	dlc_rput(clsBlock, CLS_ACTIVATE_SAP_CNF, CLS_OUT_OF_MEMORY);
	return;
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
 * llc_wput(*cep, LLC2_MODIFY_SAP_REQ, &param_block)
 *
 *
 * Old Code Function
 * -----------------
 * none/New function
 *
 * Enhancements
 * ------------
 * Deregister and re-register in the Sap registry
 *
 ***********************************************************************
 */

static inline  clsLlcMapFnT llc2ModifySapReq (
    	llctype *llc, ClsDlcParamBlockT *clsBlock)
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
 * llc_wput(*cep, LLC2_DEACTIVATE_SAP_REQ, &param_block)
 *
 *
 * Old Code Function
 * -----------------
 * None/New function
 *
 * Enhancements
 * ------------
 *
 * Removes Sap from the registry
 *
 ***********************************************************************
 */

static inline  clsLlcMapFnT llc2DeactivateSapReq (
    llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    idbtype		*idb;
    DlcActSapT		*dlcActSap;
    LlcActSapT		*llcActSap;
    sap_registry_t	*sapRegistry;
    sap_entry_t		*sapEntry;

    dlcActSap = &clsBlock->fParameters.fDlcActSap;
    llcActSap = &dlcActSap->fDlcSap.fLlcActSap;

    idb = (idbtype *)(dlcActSap->fPPortId);
    if (idb == NULL) {
	errmsg(&msgsym(OPTNULL, DLC));
	dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    sapRegistry = sap_get_registry(idb);
    if (sapRegistry == NULL) {
	dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_NO_SAP_REGISTRY);
	return;
    }

    sapEntry = sap_deregister(sapRegistry, llcActSap->fLSapValue);

    /* 
     * fails if SAP wasn't there 
     */
    if (sapEntry == NULL) {			
	dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_UNKNOWN_SAP);
	return;
    }

    /* 
     * Dispose of the memory.
     */
    free(sapEntry);			
    dlc_rput(clsBlock, CLS_DEACTIVATE_SAP_CNF, CLS_OK);

    return;
}


/************************************************************************
 * 	      CLSI Primitive: Req_opnstn.req (outgoing)
 *************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC2_REQ_OPNSTN_REQ, &parm_block)
 *
 *
 * Old Code Function
 * -----------------
 * llc2_open1()
 *
 *
 * Enhancements
 * ------------
 * Rename llc2_open1() to llc2_constructor().  Function allocates data
 * structure and populates with information from paramter block.
 * Structure members (llctype *) which require initialization include:
 * 	idb
 * 	loc_wind
 * 	u_cep_id
 *
 * Do not set llc->txaction to CONN_R_BIT, or SABME will be generated.
 * Based upon parameter block, need to do one of the following:
 * 	1. Send TEST with ARE
 * 	2. Send TEST with STE
 * 	3. Get RIF from RIF cache.
 * 	4. Send directed TEST @ 3, on failure, do #1 or #2
 *
 * Initial code will perform #2.
 *
 * Consider optional timeout for RIF determination process.  If no
 * response to TEST in certain amount of time, return error in
 * confirmation message to CLS.
 *
 * Also want to consider optional retry counter for TEST frames.
 *
 * Error Conditions
 * ----------------
 * Connection is already open between existing endpoints.
 *
 * Could not allocate llctype memory structure.
 *
 **********************************************************************
 */

static inline  clsLlcMapFnT llc2ReqOpnStnReq (
    llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    DlcReqOpenStnT	*dlcReqOpenStn;
    LlcReqOpenStnT	*llcReqOpenStn;
    ClsUCepIdT		uCepId;
    idbtype		*idb;
    llc_sb_t            *llc_sb;
    byte                explorerOption;
    

    CLSASSERT(llc == (llctype*) CLS_NO_CEP);	/* trap to detect older applications
				 * and verify that no LLC's are created
				 * except for here!
				 */
    dlcReqOpenStn = &clsBlock->fParameters.fDlcReqOpenStn;
    llcReqOpenStn = &dlcReqOpenStn->fDlcStn.fLlcReqOpenStn;
    explorerOption = llcReqOpenStn->fExplorerOption;
    
    idb = (idbtype *)dlcReqOpenStn->fPPortId;

    uCepId = dlcReqOpenStn->fClsUCepId;

    if (!interface_up_simple(idb)) {
	llcReqOpenStnConf(NULL, uCepId, CLS_INTERFACE_DOWN);
	return;
    }

    if (llcReqOpenStn->fLMacAddr.type == STATION_ILLEGAL)
	(*idb->hwptr->extract_hwaddr)(idb->hwptr, &llcReqOpenStn->fLMacAddr);

    llc = LlcNewLlctype(idb, llcReqOpenStn);

    /*
     * return code of LlcNewLlctype() has been overloaded to
     * return status as well as a valid pointer.  while not
     * as 'pure' as some solutions, it prevents extra overhead
     * by extraneous check and search functions which will
     * be called anyway.  Return code is cast to minimize impact
     * to legacy inside LlcNewLLctype() code which expects
     * simple NULL/non-NULL return codes...
     */
    /*
     * check for possible error conditions.  if an error occurred,
     * signal DLU and return, no need to continue processing.
     */
    switch ((llc2OpenFailureCode)llc)
    {

	/* Confirm error conditions with NULL llc.  Semantically, is
	   consistent because no llc was created.  Also has side
	   effect that the NULL condition does the right thing within
	   llcReqOpenStnConf(), and the error condition doesn't ;)
	 */
    case LLC2_LlcInUse:
	llcReqOpenStnConf(NULL, uCepId, CLS_CONNECTION_BUSY);
	break;

    case LLC2_LlcNoMem:
	llcReqOpenStnConf(NULL, uCepId, CLS_OUT_OF_MEMORY);
	break;

    case LLC2_LlcBadIdb:
	llcReqOpenStnConf(NULL, uCepId, CLS_UNKNOWN_PORT);
	break;

    default:		/* since nothing bad happened, llc must be ok! ;) */

        llc_sb = llc_get_or_create_sb(idb);
        if (!llc_sb) {
	  llcReqOpenStnConf(NULL, uCepId, CLS_OUT_OF_MEMORY);
          return;
        }

        dlcReqOpenStn->fDlcPCepId = (DlcPCepIdT)llc;
	llc->u_cep_id = uCepId;

	llc->llc1_timer = (llcReqOpenStn->fExplorerTimer == 0) ?
	    llc_sb->llc2_explorer_time :
	    llcReqOpenStn->fExplorerTimer;

	llc->llc1_max_retries = (llcReqOpenStn->fExplorerRetries == 0) ?
	    llc_sb->llc2_explorer_retry : 
	    llcReqOpenStn->fExplorerRetries;

	llc->retry_count = 0;

	llc->xid_timer = (llcReqOpenStn->fXIDTimer == 0) ?
	    llc_sb->llc2_xid_time :
	    llcReqOpenStn->fXIDTimer;

	llc->xid_max_retries = (llcReqOpenStn->fXIDRetries == 0) ?
	    llc_sb->llc2_xid_retry :
	    llcReqOpenStn->fXIDRetries;

	llc->loc_wind = (llcReqOpenStn->fMaxout == 0) ?
	    llc_sb->llc2_wind<<1 :
	    llcReqOpenStn->fMaxout<<1;

	llc->llc2_akmax = (llcReqOpenStn->fMaxin == 0) ?
	    llc_sb->llc2_akmax:
	    llcReqOpenStn->fMaxin;

	if (llc2_get_addrtype(llc) == STATION_FR10) {
            if (!llc->fDlci) { 
	        llc->llc2_dte[FR_L3_PID_DTE] = (llcReqOpenStn->fL3Pid == 0) ?
		    llc_sb->llc2_l3_pid :
		    llcReqOpenStn->fL3Pid;
	    }
	} else if (llc2_get_addrtype(llc) == STATION_ATMVC) {
	    llc->llc2_dte[ATM_L3_PID_DTE] = (llcReqOpenStn->fL3Pid == 0) ?
		llc_sb->llc2_l3_pid :
		llcReqOpenStn->fL3Pid;
	}

        /* 
	 * Defaults for these have already been set up 
	 * when the llctype was created.
	 */
        if ((int)llcReqOpenStn->fMaxTries>0)
                llc->llc2_n2 = llcReqOpenStn->fMaxTries;
        if (llcReqOpenStn->fTimerT1>0)
                llc->llc2_T1_time = llcReqOpenStn->fTimerT1;
        if (llcReqOpenStn->fTimerT2>0)
                llc->llc2_akdelay_time = llcReqOpenStn->fTimerT2;
        if (llcReqOpenStn->fTimerTi>0)
                llc->llc2_idle_time = llcReqOpenStn->fTimerTi;


	/*
	 * If specified, set the for this llc session
	 */
	llc->llc2_queuing_type = dlcReqOpenStn->fQueuingType;
	llc->llc2_queuing_parameter = dlcReqOpenStn->fQueuingParameter;
	if (llc->llc2_queuing_type == CLS_QUEUING_FAIR)
	    llc->llc2_fair_sequence = llc->llc2_queuing_parameter;
	else   
	    llc->llc2_fair_sequence = LLC2_FAIR_SEQ_DEFAULT;
	
	/* go and look for the remote station - either send a TEST, or look
	 * in the rif cache,  not important here.
	 *
	 * If it's already there, put the req open station out of its misery
	 */

	llc->llc_explorer_options = explorerOption;

        llc->fFlags = llcReqOpenStn->fFlags;
        if (llc->fFlags & ENC_DLSW) {
            llc->enc_flags |= ENC_DLSW;
        }
	if (explorerOption & EO_RIF_SPECIFIED) {
	    clsLlcStoreRifROSReq (llc, clsBlock);
	}

	if (explorerOption & EO_VALIDATE_RIF)
	{
	    if (DlcFindRemote(llc, explorerOption)) {
		llcReqOpenStnConf(llc, uCepId, CLS_OK);
	    } else {
		/*
		 * We sent a TEST and are waiting for a response or a
		 * timeout.
		 */
		llc->llc_req_opn_stn_sent = TRUE;
	    }
	} else {
	    llcReqOpenStnConf(llc, uCepId, CLS_OK);
	}

    } /* end switch */

    return;
}

/************************************************************************
 * 	      CLSI Primitive: closestn.req (outgoing)
 *************************************************************************
 **********************************************************************
 */

static inline  clsLlcMapFnT llcCloseStnReq (
    llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    if (llc == (llctype*) CLS_NO_CEP) {
      /* 
       * Trying to cancel a pending ReqOpnStn. CLS did not know the 
       * p_cep_id (llc), but it does know the u_cep_id it sent us in 
       * the ReqOpnStn.req. We need to scan all llc structures to 
       * find the correct one to close.
       */

      ClsUCepIdT uCepId;

      uCepId = clsBlock->fParameters.fDlcCloseStn.fClsUCepId;

      llc = llc2_by_ucepid(uCepId);   /* scan for llc->u_cep_id */
    }

    /* 
     * I've got an llc either because it was the pcepid passed in by
     * CLS, or because the code above found it from the ucepid. In either
     * case I do the same thing.
     */
    if (llc != (llctype*) CLS_NO_CEP && llc != NO_CLS) {
	llc2_goto_adm(llc, LC_USER_CLOSE);
	llc2ClsNotify(llc, CLS_CLOSE_STN_CNF, CLS_OK);
    } else 
	errmsg(&msgsym(INVPCEP, DLC));

    return;
}

/************************************************************************
 * 			CLSI Primitive: ID.req
 *************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC2_ID_REQ, &param_block)
 *
 *
 * Old Code Function
 * -----------------
 * tr_setup_xid()
 *
 *
 * Enhancements
 * ------------
 * Need new tx_xid() to allow user-configurable xid to be passed in.
 * (see phil) Need to make routine generic enough to handle all of the
 * following frame types:
 * 	1. UI
 * 	2. MAC
 * 	3. XID
 *
 * Who transmits on XID time-out?  DLU?  Or CLS?  How does APPN code
 * handle it?  Make it a configuration parameter Req_opnstn.req.
 * Potentially return timeout condition in ID.cnf.
 *
 * Error Conditions
 * ----------------
 * No memory for packet
 *
 * Output interface is down.
 **********************************************************************
 */

static inline  clsLlcMapFnT llc2IdReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    DlcIdStnT	*dlcIdStn;
    sap_hdr	*sapHdr;
    paktype	*pak;
    leveltype   level;

    if (!llc) {
	clsBlock->fResultError = CLS_UNKNOWN_CEP;
	return;
    }

    dlcIdStn = &clsBlock->fParameters.fDlcIdStn;
    pak	= clsBlock->fPakPtr;

    pak->datagramstart -= SAP_HDRBYTES;	/* adjust for SAP */
    pak->datagramsize  += SAP_HDRBYTES;

    sapHdr = (sap_hdr *)(pak->datagramstart);
    sapHdr->dsap = llc2_get_rsap(llc);
    sapHdr->ssap = llc2_get_lsap(llc);
    if (dlcIdStn->fXidT == XID_RESPONSE) {

/* Turn this off for APPN. Think about the function -SHB
 *	llc->llc_dlu_xid_rsp_pending = FALSE;
 */
	sapHdr->ssap |= SAP_RESPONSE;
    }

    sapHdr->control = LLC1_XID | P_BIT;	/* always set the P bit */

    DlcCancelXidInProgress(llc);
    level = raise_interrupt_level(NETS_DISABLE);
    pak_lock(pak);
    llc->llc_xid_pak = pak;
    reset_interrupt_level(level);

    llc->retry_count = 1;

    pak->llc2_cb = llc;

    /*
     * store if_input and if_output so that packet travels down same "river"
     * transmit routines will modify these fields, so store them to insure
     * re-transmitted packets take same path through the idb chaining...
     *
     * store other fields which are modified by virtual idb driver.
     * Fields are restored on re-transmission...
     */
    pak->vidb_input = pak->if_input; 
    pak->vidb_output = pak->if_output;
    
    pak->vidb_flags = pak->flags; 
    pak->vidb_datagramsize = pak->datagramsize;
    pak->vidb_datagramstart = pak->datagramstart;
    pak->vidb_linktype = pak->linktype; 
    pak->vidb_enctype = pak->enctype; 
    
    llc2_starttimer(llc, LLC1_TIMER, llc->xid_timer);
    llc2_transmit(llc, pak, pak->datagramsize - SAP_HDRBYTES,
		      LLC_UNNUMBERED);
    return;
}


/************************************************************************
 * 		     CLSI Primitive: Connect.req
 *************************************************************************
 * (Cause SABME to flow...)
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC2_CONNECT_REQ, &param_block)
 *
 * Old Code Function
 * -----------------
 * Setting llc->txaction = CONN_R_BIT (connection request), which initiates a
 *
 *
 * Enhancements
 * ------------
 *
 * DLC Message Received
 ************************************************************************
 */

static void llc2ProcessModifyFlags (llctype *llc,
				    ushort modFlags,
				    DlcConnectT* newDLCParms)
{
    /*
     * It seems this is the only flag we need to modify (for now). If
     * the need arises, we can add more here.
     */
    if (modFlags & CLS_MOD_MAXOUT) {
	/*
	 * Only allow XID exchange to negotiate downward from the
	 * configured value. We will never allow our send window to be
	 * greater than what is configured in the router.
	 */
	if (newDLCParms->fDlcMod.fLlcReqOpenStn.fMaxout < llc->orig_loc_wind) {
	    llc->orig_loc_wind = newDLCParms->fDlcMod.fLlcReqOpenStn.fMaxout;
	    llc->loc_wind = newDLCParms->fDlcMod.fLlcReqOpenStn.fMaxout << 1;
	}
    }
    if (modFlags & CLS_MOD_QUEUING) {
	llc->llc2_queuing_type = newDLCParms->fQueuingType;
	llc->llc2_queuing_parameter = newDLCParms->fQueuingParameter;
	if (llc->llc2_queuing_type == CLS_QUEUING_FAIR)
	    llc->llc2_fair_sequence = llc->llc2_queuing_parameter;
	else   
	    llc->llc2_fair_sequence = LLC2_FAIR_SEQ_DEFAULT;
    }
}

static clsLlcMapFnT llc2ConnectReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    DlcCancelXidInProgress(llc);
    llc2ProcessModifyFlags(llc, 
			   clsBlock->fParameters.fDlcConnect.fModifyFlags,
			   &clsBlock->fParameters.fDlcConnect);

    /*
     * FB 7/25/96
     * Can't do this, clsBlock->fPakPtr is NULL.
     * See CLSCepConnectReq.
     *
     * llc->enc_flags = clsBlock->fPakPtr->enc_flags; 
     */
    llc->llcRole = LLC2_PRIMARY;
    llc->txaction = CONN_R_BIT;	/* magic action flag to initiate connection */
    llc2call(llc);		/* examine magic flag and send SABME */
    return;
}


/************************************************************************
 *		     CLSI Primitive: Connect.rsp
 *************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_CONNECT_RSP, &param_block)
 * 	cookie should be LLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * (none)/New Function
 * 	LLC2 sends a UA
 *
 * Enhancements
 * ------------
 * Function carries out the transition from Contact_Cnf_Pending to
 * another new state (Contacted_Ind_Pending).
 ***********************************************************************
 */

static clsLlcMapFnT llc2ConnectRsp (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
   DlcCancelXidInProgress(llc);
   llc2ProcessModifyFlags(llc, 
			   clsBlock->fParameters.fDlcConnect.fModifyFlags,
			   &clsBlock->fParameters.fDlcConnect);

    /*
     * receipt of SABME causes transition from admstate to normalstate in
     * IEEE 802.2-1985 and in legacy code.  Seperation of atomic SABME/UA
     * (hear the atomic boom;), means we have absorbed the incoming SABME
     * in the admstate and we therefore must make the trasition into
     * normalstate here!
     * llc2_release_UA() sends frame and updates appropriate state variables
     */
    llc2_release_UA(llc);

/*  Option added to , To RNR or not , Enable when needed - rn 
    if(clsBlock->fParameters.fDlcConnect.fFlag  == TRUE)
     llc->state = LLS_BUSY;
*/

    return;
}



/************************************************************************
 * 	      CLSI Primitive: modifystn.req 
 *************************************************************************
 *
 **********************************************************************
 */

static inline  clsLlcMapFnT llcModifyStnReq (
    llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    llc2ProcessModifyFlags(llc, 
			   clsBlock->fParameters.fDlcConnect.fModifyFlags,
			   &clsBlock->fParameters.fDlcConnect);
    llc2ClsNotify(llc, CLS_MODIFY_STN_CNF, CLS_OK);
    return;
}


/************************************************************************
 * 		    CLSI Primitive: Signal_stn.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_SIGNAL_STN_REQ, &param_block)
 * 	cookie should be LLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * (none)/New Function
 * 	llc2_stateswitch(llc, CLEAR_LOCAL_BUSY)
 *
 * Enhancements
 * ------------
 * We should put a wrapper round this. Just to keep the layering
 * a bit cleaner.  Doing the fsm will enforce this anyway.
 *
 **********************************************************************
 */

static inline  clsLlcMapFnT llc2SignalStnReq (
    llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    llc2_stateswitch(llc, CLEAR_LOCAL_BUSY);

    reset_interrupt_level(level);

    return;
}


/************************************************************************
 *		       CLSI Primitive: Data.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_DATA_REQ, &param_block)
 * 	cookie should be DLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * llc2_send()
 * 	callout when the _first_ I- or S- frame is received.
 **********************************************************************
 */

static inline  clsLlcMapFnT llc2DataReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
     /* Temporary fix for now.  LLCs use pak->flags == PAK_LACK_OUT as
      * a signal to the tring encapsulate routines to use either
      * hardware address or passed in address.  LNM uses bia.  RSRB/LA
      * (etc.) use the PAK_LAK_OUT flag.  * Use some other mechanism
      * within CLS to turn on this falg and insure correct SA is taken.
      */
#ifdef CLAUDE_CARTEE
    clsBlock->fPakPtr->flags |= PAK_LACK_OUT; /* S/U frames use diff snd/rcv buffer */
#endif
    clsBlock->fPakPtr->flags |= PAK_LACK_OUT; /* S/U frames use diff snd/rcv buffer */

    /*
     *rc = llc2_send(llc, clsBlock->fPakPtr, clsBlock->fPakPtr->datagramsize);
     */

    llc2_send(llc, clsBlock->fPakPtr, clsBlock->fPakPtr->datagramsize);
   /*
    *if (rc != 0)
    *{
    *	buginf("\nCLI: Error %d in llc2_send, data not sent.", rc);
    *}
    */
    return;
}


/************************************************************************
 * 		       CLSI Primitive: Flow.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_FLOW_REQ, &param_block)
 * 	cookie should be DLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * llc2_setstate()
 * 	SET/CLEAR_LOCAL_BUSY
 *
 * After we set local busy  current LLC2 code just pitches any packets it
 * get till CLEAR_LOCAL_BUSY.  So we don't need to hold any packets.
 *
 *
 * CLSI Primitive
 * --------------
 * Flow.ind
 *
 * DLC Message Generated
 * ---------------------
 * cls_rput(cookie, CLS_FLOW_IND, &param_block)
 * 	cookie should be CLS's u_cep_id
 *
 * Old Code Function
 * -----------------
 * (none)/New Function		- llc2_input().
 * 	Need to call out from llc2 code.
 *
 * ?? Should we queue any packets received after making the call
 *    to CLS
 *    I say yes.
 * ??
 * ***********************************************************************/
static inline  clsLlcMapFnT llc2FlowReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    if (clsBlock->fParameters.fResumeFlow)
	llc2_stateswitch(llc, CLEAR_LOCAL_BUSY);
    else
	llc2_stateswitch(llc, SET_LOCAL_BUSY);

    reset_interrupt_level(level);
    return;
}


/************************************************************************
 *		    CLSI Primitive: Disconnect.req
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_DISC_REQ, &param_block)
 * 	cookie should be LLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * llc2_close()
 * 	Sends DISC.
 * ?? Should we allow for immediate disconnection?
 * ??
 *
 * Error Conditions
 * ----------------
 * Invalid cookie.
 **********************************************************************
*/

static inline  clsLlcMapFnT llc2DisconnectReq (llctype *llc,
				       ClsDlcParamBlockT *clsBlock)
{
    llc2_close(llc);
    return;
}

/************************************************************************
 *		    CLSI Primitive: Disconnect.rsp
 ************************************************************************
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(cookie, DLC_DISC_REQ, &param_block)
 * 	cookie should be LLC's p_cep_id
 *
 * Old Code Function
 * -----------------
 * llc2_close()
 * 	Sends DISC.
 * ?? Should we allow for immediate disconnection?
 * ??
 *
 * Error Conditions
 * ----------------
 * Invalid cookie.
 **********************************************************************
*/

static inline  clsLlcMapFnT llc2DisconnectRsp (llctype *llc,
				       ClsDlcParamBlockT *clsBlock)
{
    switch (llc->llcDiscAckRspType)
    {
    case LLC2_DISC_RSP_DM:		/* respond with a DM frame */
	send_control(llc, DM_IND);
	break;

    case LLC2_DISC_RSP_UA:     		/* respond with a UA frame */
	send_control(llc, UA_IND);
	break;

    case LLC2_DISC_RSP_NONE:	/* DISC _generated_ by this LLC station
				 * (i.e. after n2 timeout), so when DLU
				 * responds, no ackowledgement is required...
				 */
	break;

    default:
	if (llc2_debug_err)
	    buginf("\nLLC2: Unknown response type to DISC command");
    }

    llc->llcDiscAckRspType = LLC2_DISC_RSP_SENT;

    return;
}

static clsLlcMapFnT llc2PongReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    CepPongRequest(llc->u_cep_id, &llc->fCep);
    return;
}


/*
 * SapSNAHandler, vectors to the functions:
 *
 *     SapSNAHandleTest,
 *     SapSNAHandleSnaXid,
 *     SapSNAHandleIeeeXid,
 *
 *
 * standard way to handle XIDs and TESTS for SNA SAPs
 *
 * If a SAP receives a TEST Command for an existing llctype (dlcPCepId)
 * 	or a TEST Response
 *	    and there's a valid Request Open Station Requset outstanding which
 *          matches the address of the outstanding request,
 * it will confirm the request
 *
 * If the SAP receives an IEEE_XID, it will handle this the same way as a TEST
 *
 * If the SAP receives an XID, it passes it up to the CLS layer.  It never
 * responds on its own to the XID.  (If SDLLC is being used, the XID never
 * comes through this path, but always ends up in sdllc_input (through the
 * virtual ring, and in any case the SNA SAP won't be registered if the TR
 * is being used for SDLLC.
 *
 */

static void SapSNAHandleTest (idbtype *idb, paktype *pak, boolean usesa)
{
    sap_hdr		*llc1;
    llctype 		*llc2;
    sap_entry_t*	sap_entry;
    hwaddrtype          pakAddr;
    hwaddrtype          llcAddr;
    trrif_t             *trh;
	
    llc1 = (sap_hdr *) pak->info_start;
    (*idb->hwptr->extract_snpa)(pak, &pakAddr, SOURCE);
    
    sap_entry = sap_get_entry(idb, llc1->dsap);
    if (sap_entry == NULL) {
	return;
    }

    llc2 = sap_entry->llc_list;
    while (llc2 != NULL) {
        if (llc2->llc_req_opn_stn_sent) {
            
            if ((llc2->fDlci) && 
                (snpa_get_type(&pakAddr) == STATION_FR10)) {
                 /* fras ban */
                 trh = (trrif_t *) pak->mac_start; /* points to the start of TR header in 
                                                      frame-relay bridged-1490 frame */
                 if ((ieee_equal_sans_rii(trh->saddr, llc2_get_rmac(llc2))) &&
                     (llc2->fDlci == GETSHORT(pakAddr.addr)))

                     llcReqOpenStnConf(llc2, llc2->u_cep_id, CLS_OK);

            } else { 
                llc2_build_hwaddr_rmac(&llcAddr, llc2);
	        if (snpa_equ(&llcAddr, &pakAddr)) {
	            if (is_ieee802(idb->hwptr)) {
	                llc2_store_rif(llc2, pak->rif_start, CHANGE_DIRECTION);
		    }
		    llcReqOpenStnConf(llc2, llc2->u_cep_id, CLS_OK);
		    if (llc2->state == LLS_ADM && llc2->llc2_adm_timer_value != 0) {
			llc2_stoptimer(llc2, ADM_TIMER_M);
			llc2_starttimer(llc2, ADM_TIMER, llc2->llc2_adm_timer_value);
		    }
	        }
	    }
	}
	llc2 = llc2->llc_sap_link;
    }
}


void SapSNAHandleSnaXid (idbtype *swidb,
 			 paktype *xidPak,
			 void *uSapId,
			 boolean isResponse)
{
    llctype 		*llc;
    ClsDlcParamBlockT	clsBlock;
    DlcIdStnT		*dlcIdStn;
    LlcXidIdStnT	*llcIdStn;
    paktype		*pak;
    void                *u_cep_id;
    hwidbtype           *hwidb = hwidb_or_null(swidb);
    uchar               dtestring[LLC_MAX_DTE_SIZE];
    uchar               addrtype;
    sap_hdr		*llc1;
    lak_t               *lak_ptr;
    uchar               address[14];    
    ushort              Dlci;

    CLSASSERT_FATAL(hwidb != NULL);

    /* 
     * Since the caller releases the packet, we need a local copy.
     * Make sure the copy has room for the additional IdStnInd_t bytes.
     */
    pak = pak_dup_grow_realign(xidPak, xidPak->datagramsize, 
                               (sizeof(IdStnInd_t) + sizeof(long)));

    if (pak == NULL)
	return;

    /*
     * Determine if there is an existing context for this llc2 session
     * in the CLS world. If there is, then we need to do some
     * housekeeping related to that session. If there is no existing
     * context, make sure that u_cep_id is equal to CLS_NO_CEP so that CLS
     * will do the right thing.
     */
    llc = llc2_by_pak(pak, dtestring, &addrtype, &Dlci);
    if (llc != NULL) {

        if (llc->llc_req_opn_stn_sent) {
            /*
             * llc->llc_req_opn_stn_sent == TRUE means that TEST was sent
             *   so any response satisfies
             *
             */
            if (is_ieee802(swidb->hwptr)) {
                llc2_store_rif(llc, pak->rif_start, CHANGE_DIRECTION);
            }
            llcReqOpenStnConf(llc, llc->u_cep_id, CLS_OK);
        }

	if (llc->u_cep_id == (void*) CLS_NO_CEP) {
	    datagram_done(pak);		/* Not a CLS owned session */
	    return;
	}

	DlcCancelXidInProgress(llc);

	u_cep_id = llc->u_cep_id;

        /* 
	 * Initialize "rsrb_lack" in LLC as disabled          
	 * If peer exists and local-ack enabled for peer      
	 *   Create LAK structure if it doesn't already exist 
	 *   Set "rsrb_lack" in LLC to indicate LAK enabled   
	 */
        llc->rsrb_lack = FALSE;
        if (hwidb->status & IDB_VIRTUAL) {
            if (reg_invoke_rsrb_is_pak_from_lak_peer(pak)) {
                lak_ptr = reg_invoke_trmac2lak(pak, address, TRMAC2LAK_FROM_NET);
                if (!lak_ptr)
		    reg_invoke_cls_CreateLak(llc, pak);
		llc->rsrb_lack = TRUE;
	    }
	}
    } else {
        /* 
         *  Once the uSapId is NULL it will goto the 
         *  caller 'llc_process_xid' to free the old packet, because
         *  no DLU is existing, here let us free the new packet. 
         */
        if (uSapId == NULL) {
            datagram_done(pak);            
            return;
        }
	u_cep_id = (void*) CLS_NO_CEP;		/* No existing context */
    }
	
    dlcIdStn = &clsBlock.fParameters.fDlcIdStn;
    clsBlock.fPortId.type = CLS_P_TYPE_SWIDB;
    clsBlock.fPortId.fTypeInfo.swidb = swidb;


	llcIdStn = &dlcIdStn->fDlcXid.fLlcXidStn;

    if (is_ieee802(hwidb)) {
	    /* IEEE 802 LAN MAC header (TR, EN, FDDI) */

	    llcIdStn->fMacType = STATION_IEEE48;
	    llcIdStn->fLMacLength = IEEEBYTES;
	    llcIdStn->fLMacPtr = pak->addr_start;
	    llcIdStn->fRMacLength = IEEEBYTES;
	    llcIdStn->fRMacPtr = pak->addr_start + IEEEBYTES;
	    llcIdStn->fRoutingPtr = pak->rif_start;
            if (hwidb->make_snpa_native) {
               (hwidb->make_snpa_native)(hwidb, llcIdStn->fLMacPtr);
               (hwidb->make_snpa_native)(hwidb, llcIdStn->fRMacPtr);
	    } 
	if (pak->rif_start != NULL) {
	    llcIdStn->fRoutingLength = pak->info_start - pak->rif_start;
	} else {
	    llcIdStn->fRoutingLength = 0;
	}
    } else if (is_frame_relay(hwidb)) { /* Frame Relay 10 bit DLCI */
	llcIdStn->fMacType = STATION_FR10;
	llcIdStn->fLMacLength = STATIONLEN_FR10; /* Source and */
						     /* destination */
	/*
	 * This next bit is to deal with the fact that the dlci is
	 * mixed up with a bunch of other bits in the frame relay
	 * header. We overwrite the frame relay header with the 
	 * dlci number so the DLU can find it in the form it needs.
	 *
	 */
	PUTSHORT(pak->datagramstart, GETSHORT(&dtestring[FR_DLCI_DTE]));
	llcIdStn->fLMacPtr = pak->datagramstart; /* are the same! */
	llcIdStn->fRMacLength = STATIONLEN_FR10;
	llcIdStn->fRMacPtr = pak->datagramstart;
	llcIdStn->fRoutingLength = 0;
	llcIdStn->fRoutingPtr = NULL;
        llcIdStn->fDlci = Dlci;
    } else if (is_smds(hwidb)) { 
	llcIdStn->fMacType = STATION_SMDS64;
	llcIdStn->fLMacLength = STATIONLEN_SMDS64;
	llcIdStn->fLMacPtr = pak->addr_start;
	llcIdStn->fRMacLength = STATIONLEN_SMDS64;
	llcIdStn->fRMacPtr = pak->addr_start + STATIONLEN_SMDS64;
	llcIdStn->fRoutingLength = 0;
	llcIdStn->fRoutingPtr = NULL;
    } else if (is_atm(hwidb)) { 
	llcIdStn->fMacType = STATION_ATMVC;
	llcIdStn->fLMacLength = STATIONLEN_ATMVC; /* Source and */
	/* destination */
	/*
	 * This next bit is to deal with the fact that the dlci is
	 * mixed up with a bunch of other bits in the frame relay
	 * header. We overwrite the frame relay header with the 
	 * dlci number so the DLU can find it in the form it needs.
	 *
	 */
	PUTSHORT(pak->datagramstart, GETSHORT(&dtestring[ATM_VCD_DTE]));
	llcIdStn->fLMacPtr = pak->datagramstart; /* are the same! */
	llcIdStn->fRMacLength = STATIONLEN_ATMVC;
	llcIdStn->fRMacPtr = pak->datagramstart;
	llcIdStn->fRoutingLength = 0;
	llcIdStn->fRoutingPtr = NULL;
    } else if (is_ppp(hwidb)) {
	llcIdStn->fMacType = STATION_PPP;
	llcIdStn->fLMacLength = STATIONLEN_PPP;
	llcIdStn->fLMacPtr = pak->datagramstart;
	llcIdStn->fRMacLength = STATIONLEN_PPP;
	llcIdStn->fRMacPtr = pak->datagramstart;
	llcIdStn->fRoutingLength = 0;
	llcIdStn->fRoutingPtr = NULL;
    } else {
	buginf("\nSapSNAHandleSnaXid: Unknown address type %d",
	       addrtype);
	datagram_done(pak);
	return;
    }    
	
    llc1 = (sap_hdr *) pak->info_start;
    llcIdStn->fRSap = llc1->ssap & ~SAP_RESPONSE;
    llcIdStn->fLSap = llc1->dsap;
	
    /* Set network_start to point at start of xid data */
    pak->network_start = pak->info_start + SAP_HDRBYTES;

    dlcIdStn->fXIDLength = pak->datagramsize -
	(pak->network_start - pak->datagramstart);
    dlcIdStn->fXIDPtr = pak->network_start;

    dlcIdStn->fXidT          = isResponse ? XID_RESPONSE: XID_COMMAND;
    dlcIdStn->fDlcPCepId     = (DlcPCepIdT)llc;
    dlcIdStn->fClsUSapId = (ClsUSapIdT)uSapId;
    dlcIdStn->fClsUCepId = u_cep_id;
    
    clsBlock.fResultError    = CLS_OK;
    clsBlock.fPakPtr         = pak;
    
    reg_invoke_cls_rput(u_cep_id, CLS_ID_IND, &clsBlock);
    return;
}


static void SapSNAHandleIeeeXid(idbtype *idb, paktype *pak, boolean usesa)
{
    return;
}



ClsDlcParamBlockT*
llcBuildParmBlock (llctype *llc,
		   ClsOpcodeT opcode,
		   ClsDlcParamBlockT *paramBlock,
		   ClsErrorCodeT ClsDlcErrorCode)
{
    paktype *pak = NULL;	/* remove the unitialized warning msg */
    uint encapsize = 0;
    uint riflen;

    paramBlock->fResultError = ClsDlcErrorCode;
    /*
     * If fPakPtr is not equal to NULL, then CLS will assume that it
     * contains a paktype that it can use to build a CLSI message.
     *
     * In the future, other opcodes may provide a paktype for CLS to
     * optionally use. If CLS finds a non-NULL value here, it will
     * assume responsibility for releasing the paktype when it is
     * finished using it.
     *
     * For now, the only opcodes that should have a non-NULL value for
     * this field are CLS_DATA_IND, and CLS_ID_IND. The cases for
     * these opcodes handle setting the field.
     */
    paramBlock->fPakPtr = NULL;

    switch (opcode) {

    case CLS_NO_OP:
    case CLS_ENABLE_CNF:
    case CLS_DISABLE_CNF:
	break;				/* not an llc2 message */

	/* SAP management operations always run right through, so we can
	 * reuse the parameter block that was sent down (AND it has the
	 * correct ClsUSapId in it)
	 */
    case CLS_ACTIVATE_SAP_CNF:
    case CLS_MODIFY_SAP_CNF:
	break;				/* not an llc2 message */

    case CLS_DEACTIVATE_SAP_CNF:
	break;				/* not an llc2 message */

    case CLS_REQ_OPNSTN_CNF:
	break;				/* handled elsewhere */

    case CLS_CLOSE_STN_CNF:
	break;				/* handled elsewhere */

    case CLS_TEST_IND:
	break;				/* handled elsewhere */

    case CLS_ID_IND:
	break;				/* handled in SapSNAHandleSnaXid */

    case CLS_CONNECT_IND:
    case CLS_CONNECT_CNF:	/* claude???, what are the parameters to put here??? */
    case CLS_MODIFY_STN_CNF:
	paramBlock->fParameters.fDlcConnect.fPPortId = llc->idb;
	paramBlock->fParameters.fDlcConnect.fDlcPCepId = (DlcPCepIdT)llc;
	paramBlock->fParameters.fDlcConnect.fClsUCepId = llc->u_cep_id;
	/* check this later, adm_reason probably isn't the right variable */

	paramBlock->fParameters.fDlcConnect.fLlcRetCode = llc->adm_reason;
	
        /* Pass the RIF up to the DLU.  APPN needs this for HPR. */
        riflen = rif_length(&llc->llc2_dte[RIF_DTE]);
        paramBlock->fParameters.fDlcConnect.fDlcMod.fLlcReqOpenStn.fRifLength =riflen;
        if (riflen) {
           memcpy (&paramBlock->fParameters.fDlcConnect.fDlcMod.fLlcReqOpenStn.fRif[0], &llc->llc2_dte[RIF_DTE], riflen);
        }
        break;

    case CLS_DISC_IND:
    case CLS_DISC_CNF:
	paramBlock->fParameters.fDlcDisconnect.fPPortId = llc->idb;
	paramBlock->fParameters.fDlcDisconnect.fDlcPCepId = (DlcPCepIdT)llc;
	paramBlock->fParameters.fDlcDisconnect.fClsUCepId = llc->u_cep_id;
	paramBlock->fParameters.fDlcDisconnect.fLlcRetCode = llc->adm_reason;
	break;


    case CLS_DATA_IND:
	pak = llc->inbuf;	/* save indirection below */
	/*
	 * Remove all headers, and adjust so passed packet only includes
	 * information payload. Dereferenced ptr math removes:
	 *     AC,FC,DA,SA and variable length RIF
	 * Constant removes: DS,SS,Nr and Ns
	 */
	encapsize = pak->info_start + SAP_LONGHDRBYTES - pak->datagramstart;
	pak->datagramstart += encapsize; /* move ptr beyond hdr to payload */
	pak->datagramsize -= encapsize;	 /* decrease size accordingly */

        paramBlock->fLoadLength =  pak->datagramsize;

	paramBlock->fPakPtr = pak;
	break;

    case CLS_TEST_STN_IND:
    case CLS_CONNECT_STN_IND:
    case CLS_ID_STN_IND:
    case CLS_ACTIVATE_RING_CNF:
    case CLS_DEACTIVATE_RING_CNF:
        break;                          /* handled elsewhere */

    case CLS_UDATA_STN_IND:
    case CLS_UDATA_IND:
        pak = llc->inbuf;       /* save indirection below */
        /*
         * Remove all headers, and adjust so passed packet only includes
         * information payload. Dereferenced ptr math removes:
         *     AC,FC,DA,SA and variable length RIF
         * Constant removes: DS,SS,Nr and Ns
         */
        encapsize = pak->info_start + SAP_LONGHDRBYTES - pak->datagramstart;
        pak->datagramstart += encapsize; /* move ptr beyond hdr to payload */
        pak->datagramsize -= encapsize;  /* decrease size accordingly */

        paramBlock->fPakPtr = pak;
        break;

    case CLS_DATA_CNF:
	/* need something here for error reply to DLC_DATA_REQ */
	break;

    case CLS_FLOW_IND:
	CLSASSERT(!"Unexpected request to build CLS_FLOW_IND");
	break;
    case CLS_CONNECTED_IND:
	break;
    case CLS_PONG_CNF:
	CLSASSERT(!"Unexpected request to build CLS Pong Confirm");
	break;
    }

    return (paramBlock);
}

/*
 *static void llcReqOpenStnConf (
 *
 * Got some kind of reply for CLS about Request Open Station Request
 */

static void llcReqOpenStnConf (
    llctype 		*llc,
    ClsUCepIdT		uCepId,
    ClsErrorCodeT 	error)
{
    ClsDlcParamBlockT 	clsBlock;

    if (llc != NULL) {
	llc->llc_req_opn_stn_sent = FALSE;
	/* buginf("\n Req OpenStn Timer stopped"); */
	llc2_stoptimer(llc, LLC1_TIMER_M);
	uCepId = llc->u_cep_id;
	if (error != CLS_OK)
	{
	    llc2_goto_adm(llc, LC_USER_CLOSE);
	    llc = (llctype*) CLS_NO_CEP;
	}
    }
    clsBlock.fResultError = error;
    clsBlock.fParameters.fDlcReqOpenStn.fClsUCepId = uCepId;
    clsBlock.fParameters.fDlcReqOpenStn.fDlcPCepId = (DlcPCepIdT)llc;
    clsBlock.fPakPtr = NULL;
    reg_invoke_cls_rput(uCepId, CLS_REQ_OPNSTN_CNF, &clsBlock);
    return;
}

/*
 * May want to add a data field to the TEST in the future.
 */
static paktype* DlcBuildDirectedTest(llctype* llc)
{
    sap_hdr* llcheader;
    paktype* pak;
    hwidbtype	*hwidb = hwidb_or_null(llc->idb);
    
    pak = getbuffer(sizeof(sap_hdr));
    if (pak == NULL) {
	return(NULL);
    }
    
    pak->if_input = NULL;
    pak->if_output = llc->idb;
    
    /* When we get a pak from getbuffer() the assumption is that */
    /* only network_start and datagramstart are initialized. They */
    /* point at the end of the encapsulation area (as determined */
    /* by pak_center(). We like to work off of info_start which is */
    /* defined to point at the start of the DLC layer header. */
    
    pak->info_start = pak->network_start;
    
    llcheader  = (sap_hdr*) pak->info_start;
    llcheader->dsap = SAP_NULL;
    llcheader->ssap = llc2_get_lsap(llc);
    llcheader->control = LLC1_TEST | LLC1_P;
    pak->datagramsize = SAP_HDRBYTES;
    pak->linktype = llc->linktype;

    if (llc->fDlci) { 
        pak->llc2_enctype = ET_BRIDGE;
        llc->llc2_dte[RIF_DTE] = RC_DEF_ALLRINGS; 
        llc->llc2_dte[RIF_DTE+1] = RC_TR_LFDEF;
    }
    else
        pak->llc2_enctype = ET_LLC2;

    pak->llc2_cb = llc;
    
    if ((hwidb != NULL) && (hwidb->vencap != NULL)) {
        if (pak->llc2_enctype == ET_LLC2)
            (*hwidb->vencap)(pak, (long) llc->llc2_dte);
        else
            (*hwidb->vencap)(pak, (long) llc);
    } else {
	datagram_done(pak);
	pak = NULL;
    }
    return(pak);
}

/*
 * DlcFindRemote
 *
 * Look up the entry in the RIF cache.
 * If it's there return TRUE;
 * Else generate a TEST and send it out.  Return FALSE
 *
 * Return values is used to decide whether to to a reqopenstn confirm
 */
static boolean DlcFindRemote (llctype *llc, byte explorerOption)
{
    paktype	*pak = NULL;
    hwidbtype	*hwidb = hwidb_or_null(llc->idb);
    
    if (llc2_get_addrtype(llc) == STATION_IEEE48) {
	llc1_test_t *llc1;
	
	if ((explorerOption & EO_RIF_SPECIFIED) == 0) {
	    /* 
	     * No rif was specified. We need to try to find the route ourselves
	     */
	    if ((explorerOption & EO_TRANSPARENT_FIRST) || (is_ethernet(hwidb))) {
		/*
		 * We want to try a local directed TEST first. If this
		 * works, we are done, either the guy is on our local
		 * ring, or there is a ttransparently bridged path to
		 * him. If this times out, then we want to try an
		 * explorer so clear the bit in the llc. This way
		 * when we come back here if the timer expires, we
		 * will fall into the explorer code.
		 */
		llc->llc_explorer_options &= ~EO_TRANSPARENT_FIRST;
		pak = DlcBuildDirectedTest(llc);
	    } else {
		/*
		 * Want to add options here to control type of
		 * explorer [sberl] 
		 */
		pak = llc1_gen_explr(llc2_get_rmac(llc), llc2_get_lmac(llc), 
				     SAP_NULL, llc2_get_lsap(llc),
				     hwidb);
		if (pak == NULL) {
		    buginf("\nDlcFindRemote: Couldn't allocate an explorer");
		    return(FALSE);
		}
		llc1 = (llc1_test_t *)pak->info_start;
		
		pak->flags |= PAK_SRB_IN;		/* Dunno why */
		pak->if_input = NULL;
		pak->llc2_cb = llc;
		pak->linktype = llc->linktype;
		
	    }
	}
    } else {
	pak = DlcBuildDirectedTest(llc);
    }
    
    /* 
     * Important to start timer before send the packet, to avoid
     * race conditions
     */
    llc2_starttimer(llc, LLC1_TIMER, llc->llc1_timer);

    if (pak) {
	llc->retry_count++;
	datagram_out(pak);
    }
    
    return(FALSE);
}

/*
 * DlcTxXidFail
 *
 * Report Timeout error to Cls if XID retry fails
 */

static void DlcTxXidFail(llctype *llc, ClsErrorCodeT error)
{
    ClsDlcParamBlockT	clsBlock;

    clsBlock.fResultError = error;
    clsBlock.fPakPtr = NULL;
    clsBlock.fPortId.type = CLS_P_TYPE_SWIDB;
    clsBlock.fPortId.fTypeInfo.swidb = llc->idb;
    reg_invoke_cls_rput(llc->u_cep_id, CLS_ID_IND, &clsBlock);

    if (llc->llc_xid_pak) {
	datagram_done(llc->llc_xid_pak);
	llc->llc_xid_pak = NULL;
    }

    return;
}

/*
 * DlcCancelXidInProgress
 *
 * Perform the operations required to
 * cancel any XID in progress and free
 * the associated packet.
 */

static void DlcCancelXidInProgress (llctype *llc)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    if (llc->llc_xid_pak) {
        datagram_done(llc->llc_xid_pak);
	llc->llc_xid_pak = NULL;
	llc2_stoptimer(llc, LLC1_TIMER_M);
    }
    reset_interrupt_level(level);
}

static llctype *LlcNewLlctype(idbtype* idb, LlcReqOpenStnT* llcOpenParms)
{
    llctype*	llc;
    ushort	linktype;
    
    /*
     * Initialize linktype stored in llc control structure.  
     * CLS/RSRB support requires overloading of (*idb->oqueue)().  The
     * virtual ring code uses (*idb->oqueue)() to send packets to
     * CLS/LLC code and it is also used to get packets from CLS into
     * virtual ring code.  The linktype is used as an indicator to
     * flag which direction a frame is going.  See coments in
     * cls_add_vring_interface() and cls_vidb_oqueue() for more
     * details.
     */
    if (idb->hwptr->status & IDB_VIRTUAL)
    {
	linktype = LINK_CLS;
    } else
    {
	linktype = LINK_LLC2;
    }

    /* set up to call llc2_open1 */
    /*
     * return code of llc2_open1() has been overloaded to
     * return status as well as a valid pointer.  while not
     * as 'pure' as some solutions, it prevents extra overhead
     * by extraneous check and search functions which will
     * be called anyway.  Return code is cast to minimize impact
     * to legacy code which expects simple NULL/non-NULL
     * return codes...
     */
    llc = llc2_open1(llcOpenParms->fRSapValue,
		     llcOpenParms->fLSapValue,
		     &llcOpenParms->fRMacAddr,
		     &llcOpenParms->fLMacAddr,
		     idb, 
		     linktype,
		     llcOpenParms->fMaxout,
		     NULL,		       /* no status proc */
		     FALSE,		       /* non blocking */
		     FALSE,		       /* local-ack */
		     TRUE,		       /* called from CLSI */
		     llcOpenParms->fL3Pid,     /* RFC1490 NLPID */
                     llcOpenParms->fDlci);     /* for FR BAN    */

    switch ((llc2OpenFailureCode)llc)
    {
    case LLC2_LlcInUse:
	break;

    case LLC2_LlcNoMem:
	break;

    case LLC2_LlcBadIdb:
	break;

    default:			/* since nothing bad happened, llc must be ok! ;) */
    	llc->u_cep_id = (void*) CLS_NO_CEP;
	break;
    }

    return(llc);
}

/*
 * llc1_timer
 *
 * Timeout on llc1 events
 *
 * retransmit unless max retries has been reached in which case
 * "It's dead, Jim"
 */

void llc1_timer (llctype *llc)
{
    if (llc->llc_req_opn_stn_sent) {
	if (llc->llc1_max_retries &&
	    	llc->retry_count >= llc->llc1_max_retries)
	    llcReqOpenStnConf(llc, llc->u_cep_id, CLS_NO_REM_STN);

	else {				/* look again */
	    if (DlcFindRemote(llc, llc->llc_explorer_options))
		      llcReqOpenStnConf(llc, llc->u_cep_id, CLS_OK);
        }
    } else if (llc->llc_xid_pak) {
	if (llc->xid_max_retries && llc->retry_count >= llc->xid_max_retries) {
	    llc->retry_count = 0;
	    DlcTxXidFail(llc, CLS_NO_XID_RSP);
	}
	else {
	    paktype *pak;

	    pak = llc->llc_xid_pak;

	    pak->if_input = pak->vidb_input; 
	    pak->if_output = llc->idb;
	    
	    pak->flags = pak->vidb_flags; 
	    pak->datagramsize = pak->vidb_datagramsize; 
	    pak->datagramstart = pak->vidb_datagramstart;
	    pak->linktype = pak->vidb_linktype; 
	    pak->enctype = pak->vidb_enctype; 

	    llc->retry_count++;
	    pak->llc2_cb = llc;
	    
	    pak_lock(pak);

	    /* start timer before sending, to avoid race conditions */
	    llc2_starttimer(llc, LLC1_TIMER, llc->xid_timer);
	    /*
	     * Used to just call llc_sendframe(), but now need to
	     * rebuild the encapsulation because the vidb driver wants
	     * to muck around with vidb_datagram???, etc.
	     */
	    llc2_transmit(llc, pak, pak->datagramsize - SAP_HDRBYTES,
			   LLC_UNNUMBERED);
	}
    }
    return;
}

/*
 * adm_timer
 *
 * Timeout if no i/o in adm 
 *
 */

void adm_timer (llctype *llc)
{
    /* We just notify the DLU. We don't take any specific action now.
     * Its up the DLU to close the connection if it wants to.
     */

    llc2ClsNotify(llc, CLS_DISC_IND, CLS_INACTIVITY_TIMEOUT);
    llc->llc2_adm_timer_value = 0;  /* Only a single chance... */
}

/*
 * c l s L L c S t o r e R i f R O S {Req, Cnf} ( )
 *
 * Store rif for CLS-controlled LLC session.
 *
 * The function is intended to be an evolutionary function as
 * the route storage strategy changes within the router.  Ideally,
 * the rif should be stored with the LLC control block.  The advantages
 * are:
 *        1. RIF and LLC will not get out of sync,
 *           when LLC is destroyed, RIF is destroyed as well
 *
 *        2. Fewer instructions executed per frame
 *           no extra calls to locate RIF in common RIF table
 *
 * However to support this, the Token-Ring llc2 encapsulation routines
 * need to be modified in such a way as to be able to accepts a RIF
 * which is passed in with a packet.  Since this work will be
 * necessary for the media-independent LLC work, the 'ideal' RIF
 * support will be postponed until then.
 *
 * Current operation (06/07/84;) of this code and RIF storage is as follows:
 *   Summary:
 *       Primary Link stations lock RIF on RequestOpenStation.cnf
 *       Secondary Link stations lock RIF on RequestOpenStation.req
 *        (initial support will lock all RIF's on ROS.cnf
 *
 *   1. Primary stations will send an explorer TEST-cmd frame,
 *   2. The first TEST-rsp frame will be stored in the common RIF cache
 *      via rif_update() in SapSNAHandleTest().  The RIF will be stored
 *      as a RIF_AGED by the source address and destination addresses,
 *      just as the RequestOpnStn.cnf goes back to CLS.
 *
 *   1. Secondary stations will respond to TEST-cmd without any RIF
 *      storage or state change.
 *   2. Incoming XID from primary will trigger DLU to send RequestOpnStn.Req
 *
 *   3. Change RIF in common rif cache to RIF NO_AGE on RequestOpnStn.Req.
 *      (Eventual code enhancement is to take RIF from actual RequestOpnStn.Req
 *       message block...)
 *
 */

/*
 * c l s L L c S t o r e R i f R O S R e q ( )
 *
 * Store Rif in Request Open Station Request processing
 */
void clsLlcStoreRifROSReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    byte rifLength = clsBlock->fParameters.fDlcReqOpenStn.fDlcStn.fLlcReqOpenStn.fRifLength;
    byte *rifPtr = &clsBlock->fParameters.fDlcReqOpenStn.fDlcStn.fLlcReqOpenStn.fRif[0];

    if (rifLength != 0) {
  	/*
  	 * do length in rif's RC and discrete length match?
  	 */
  	CLSASSERT(rifLength == RIF_LENGTH(*rifPtr));
  	
  	bcopy(rifPtr, &llc->llc2_dte[RIF_DTE], rifLength);
    } else {
 	/*
 	 * KLUDGE! Legal rif always has an even length. Setting it to
 	 * an odd value fakes llc2_token_macaddr() into thinking that
 	 * this is a rif (zero means legacy code to it). tr_vencap()
 	 * will then notice that the rif is an illegal length and set
 	 * the length to zero and send the packet with no rif at all.
 	 * [sberl]
 	 */
   	 llc->llc2_dte[RIF_DTE] = 0x01;
    }
}

/**/
/*
 * C L S  R S R B   support starts here
 *
 */


/*
 * 		   CLSI Primitive: Test.req
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC2_TEST_REQ, &param_block) */
static inline clsLlcMapFnT llc2TestReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    DlcTestStnT *dlcTestStn;
    sap_hdr    *sapHdr;
    paktype    *pak;
    
    if (!llc)	{
	clsBlock->fResultError = CLS_UNKNOWN_CEP;
	return;
    }
    
    dlcTestStn = &clsBlock->fParameters.fDlcTestStn;
    pak    = clsBlock->fPakPtr;
    
    pak->datagramstart -= SAP_HDRBYTES;    /* adjust for SAP */
    pak->datagramsize  += SAP_HDRBYTES;
    
    sapHdr = (sap_hdr *)(pak->datagramstart);
    sapHdr->dsap = llc2_get_rsap(llc);
    sapHdr->ssap = llc2_get_lsap(llc);
    if (dlcTestStn->fTestT == TEST_RESPONSE)
	sapHdr->ssap |= SAP_RESPONSE;


    sapHdr->control = LLC1_TEST | LLC1_P;  /* always set the P bit */
    
    pak->llc2_cb = llc;
    
    llc2_transmit(llc, pak, pak->datagramsize - SAP_HDRBYTES, LLC_UNNUMBERED);
    
    return;
}


/*
 * 		   CLSI Primitive: Test.rsp
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC2_TEST_RSP, &param_block)
 */

static inline clsLlcMapFnT llc2TestRsp (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    DlcTestStnT *dlcTestStn;
    sap_hdr    *sapHdr;
    paktype    *pak;

    if (!llc)  {
	clsBlock->fResultError = CLS_UNKNOWN_CEP;
	return;
    }
    
    dlcTestStn = &clsBlock->fParameters.fDlcTestStn;
    pak    = clsBlock->fPakPtr;
    
    pak->datagramstart -= SAP_HDRBYTES;    /* adjust for SAP */
    pak->datagramsize  += SAP_HDRBYTES;
    
    sapHdr = (sap_hdr *)(pak->datagramstart);
    sapHdr->dsap = llc2_get_rsap(llc);
    sapHdr->ssap = llc2_get_lsap(llc);
    if (dlcTestStn->fTestT == TEST_RESPONSE)
	sapHdr->ssap |= SAP_RESPONSE;

    sapHdr->ssap |= SAP_RESPONSE;
    sapHdr->control = LLC1_TEST | LLC1_P;  /* always set the P bit */
    
    pak->llc2_cb = llc;
    
    llc2_transmit(llc, pak, pak->datagramsize - SAP_HDRBYTES, LLC_UNNUMBERED);
    return;
}

/*
 * 		   CLSI Primitive: Id.rsp
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC2_ID_RSP, &param_block)
 */

static inline clsLlcMapFnT llc2IdRsp (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    DlcIdStnT   *dlcIdStn;
    sap_hdr     *sapHdr;
    paktype     *pak;

    if (!llc) {
        clsBlock->fResultError = CLS_UNKNOWN_CEP;
        return;
    }

    dlcIdStn = &clsBlock->fParameters.fDlcIdStn;
    pak = clsBlock->fPakPtr;

    pak->datagramstart -= SAP_HDRBYTES; /* adjust for SAP */
    pak->datagramsize  += SAP_HDRBYTES;

    sapHdr = (sap_hdr *)(pak->datagramstart);
    sapHdr->dsap = llc2_get_rsap(llc);
    sapHdr->ssap = llc2_get_lsap(llc);

    if (dlcIdStn->fXidT == XID_RESPONSE)
        sapHdr->ssap |= SAP_RESPONSE;

    DlcCancelXidInProgress(llc);

    sapHdr->control = LLC1_XID | P_BIT; /* always set the P bit */

    llc->retry_count = 1;

    /*
     * store if_input and if_output so that packet travels down same "river"
     * transmit routines will modify these fields, so store them to insure
     * re-transmitted packets take same path through the idb chaining...
     *
     * store other fields which are modified by virtual idb driver.
     * Fields are restored on re-transmission...
     */
    pak->vidb_input = pak->if_input;
    pak->vidb_output = pak->if_output;

    pak->vidb_flags = pak->flags;
    pak->vidb_datagramsize = pak->datagramsize;
    pak->vidb_datagramstart = pak->datagramstart;
    pak->vidb_linktype = pak->linktype;
    pak->vidb_enctype = pak->enctype;

    llc2_transmit(llc, pak, pak->datagramsize - SAP_HDRBYTES,LLC_UNNUMBERED);
    return;
}


/*
 * 		   CLSI Primitive: udata.req
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC2_UDATA_REQ, &param_block)
 */

static inline clsLlcMapFnT llc2UDataReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    sap_hdr    *sapHdr;
    paktype    *pak;

    if (!llc) 	{
	clsBlock->fResultError = CLS_UNKNOWN_CEP;
	return;
    }
    
    pak    = clsBlock->fPakPtr;
    pak->datagramstart -= SAP_HDRBYTES;    /* adjust for SAP */
    pak->datagramsize  += SAP_HDRBYTES;

    sapHdr = (sap_hdr *)(pak->datagramstart);
    sapHdr->dsap = llc2_get_rsap(llc);
    sapHdr->ssap = llc2_get_lsap(llc);
    
    sapHdr->control = LLC1_UI;
    
    pak->llc2_cb = llc;
    
    llc2_transmit(llc, pak, pak->datagramsize - SAP_HDRBYTES, LLC_UNNUMBERED);
    
    return;
}

/*
 * 		   CLSI Primitive: Teststn.req
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC_TEST_STN_REQ, &param_block)
 */
static inline clsLlcMapFnT llcTestStnReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    boolean ret_code;	 

    ret_code = llcTestStnTxPak(clsBlock, TRUE);
    if (LLC2_DEBUG_PACKET)
	buginf("\n Send TestStn.Request - %s; len = %d ",
	       ret_code ? "success" : "failed", clsBlock->fLoadLength); 
}

/*
 * 		   CLSI Primitive: Teststn.rsp
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC_TEST_STN_RSP, &param_block)
 */
static inline clsLlcMapFnT llcTestStnRsp (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    boolean ret_code;

    ret_code = llcTestStnTxPak(clsBlock, FALSE);
    if (LLC2_DEBUG_PACKET)
	buginf("\n Send TestStn.Response - %s; len = %d ",
	       ret_code ? "success" : "failed", clsBlock->fLoadLength); 
}


/*
 * Utility routine that gets used for llcTestStnReq 
 * and llcTestStnResp
 */

static boolean llcTestStnTxPak(ClsDlcParamBlockT *clsBlock, 
				      boolean cmd_resp_flag)
{

    DlcTestStnT *dlcTestStn;
    sap_hdr 	llc1;
    hwidbtype	*idb;
    idbtype	*swidb;
    paktype     *pak;
    trrif_t     *trh;
    ushort      riflen;


    dlcTestStn = &clsBlock->fParameters.fDlcTestStn;

    swidb = DlcToIdb(&clsBlock->fPortId);
    if(swidb == NULL) 	{
	if (llc2_debug_err)
	    buginf("\ndlcllc: Invalid IDB in TestStn.%s request, ignored.",
		   cmd_resp_flag ? "Request" : "Response");
    	return FALSE;
    }

    idb = swidb->hwptr;
    pak    = clsBlock->fPakPtr;
    if (!pak) 	{
	buginf("\nTR: No buffer for TestStn.%s to %e", 
	       cmd_resp_flag ? "Request" : "Response", dlcTestStn->fRMac);
	return FALSE;
    }

    llc1.dsap = dlcTestStn->fRSap;
    llc1.ssap = dlcTestStn->fLSap;
    if (!cmd_resp_flag)
	llc1.ssap |= SAP_RESPONSE;
    llc1.control = LLC1_TEST | LLC1_P;  /* assume cmd/poll */

    /*
     * pass in if_output to llcStnEncap via pak structure.  idb is
     * required to allow fn to make SR or TB encapsulation decision...
     */
    pak->if_output     = idb->firstsw;
    pak->enctype       = idb->enctype;

    llcStnEncap(pak, dlcTestStn->fLMac, dlcTestStn->fRMac,
		 (sap_hdr *) &llc1, dlcTestStn->fRif, clsBlock->fLoadLength,
		 clsBlock->fRawData);

    /*
     * pak->linktype must be set properly for out-going packets.
     * Normally set from llc->linktype, but we don't have llc2 control
     * structure yet!  So if we're Tx'ing out vidb, force to LINK_CLS... */
    if ((idb->status == (IDB_VIRTUAL | IDB_TR)) || 
       (idb->status == (IDB_VIRTUAL | IDB_FDDI)))
	pak->linktype = LINK_CLS;

    if ((pak->if_output) && (pak->enc_flags & ENC_DLSW) &&
       (is_fddi(pak->if_output->hwptr))) {

        fddi_hdr *fddi;

        pak->datagramsize = pak->datagramsize - TRING_ENCAPBYTES + FDDI_ENCAPBYTES;
        pak->datagramstart = pak->datagramstart + TRING_ENCAPBYTES-FDDI_ENCAPBYTES;
        fddi = (fddi_hdr *)pak->datagramstart;
        if (idb->type == IDBTYPE_FDDIT) {
            ieee_swap(fddi->saddr, fddi->saddr);
            ieee_swap(fddi->daddr, fddi->daddr);
        }
        fddi->fc = FDDI_LLC_FC_BYTE;
    }
    /* 
      tr_print_packet(idb,"TestStn Sender : ", 
      			(tring_hdr *)pak->datagramstart, 
		        (ulong *) pak->network_start,
		        0, pak->datagramsize); */

    trh = (trrif_t *) pak->datagramstart;
    riflen = trh->rc_blth & RC_LTHMASK;

    if (!reg_invoke_srb_out_check((uchar *) pak->datagramstart,
               pak->datagramsize, idb, riflen)) {
        datagram_done(pak);
        return FALSE;
    }

    datagram_out(pak);
    return TRUE;
}


/*
 * Utility routine that gets used for llcTestStnReq 
 * llcTestStnResp and llcUdataStn
 * Ideally one should use idb->encap routines.
 * This needs to change
 *
 * Limitation!!!  in the current implementation, these CLS extensions
 * using this utility will _NOT_ work for CLS_RSRB port types, as
 * these are vidbs which _DO_ contain RIFs.  As there are no current
 * plans to implement DLSW over RSRB DLC, this isn't of great concern.
 * However, this should be taken into account next time these routines
 * are re-written 
 */
static void llcStnEncap(paktype *pak, uchar *src, uchar *dst,
				   sap_hdr *llc1, 
				   uchar *rif, ushort payload_len, 
				   uchar *payload_ptr)
{
    ushort 	  riflen;
    sap_hdr	  *temp_llc1_ptr;
    trrif_t 	  *trframe;
    boolean	  norif;
    hwidbtype	  *hwvidb  = NULL;
    idbtype       *swvidb  = NULL;
    srb_vector_table_t *srbV_p;
    srb_triplet_t srb_triplet;
    
    if (pak && pak->if_output) {
        hwvidb = pak->if_output->hwptr;
    }
   
    swvidb = firstsw_or_null(hwvidb);
    srbV_p = reg_invoke_srb_get_vector_table();
    if (swvidb && srbV_p) {
        srbV_p->srb_sb_get_triplet(swvidb, &srb_triplet);
    } else {
        srb_triplet.localRing  = SRB_INVALID_RING_NUM;
        srb_triplet.bridgeNum  = SRB_INVALID_BRIDGE_NUM;
        srb_triplet.remoteRing = SRB_INVALID_RING_NUM;
    }

    if ((hwvidb != NULL) &&
       (hwvidb->status == (IDB_VIRTUAL | IDB_TR)) &&
       (hwvidb->vidb_link == RSRB_INVALID_ADDRESS) &&
       (hwvidb->unit == 0) &&
       (srb_triplet.localRing  == SRB_INVALID_RING_NUM) &&
       (srb_triplet.remoteRing == SRB_INVALID_RING_NUM))
          norif = TRUE;
    else
    /*
     * Do not use rifs when transmitting to CLS TBRIDGE virtual ports!
     */
       norif = FALSE;
    
    /*
     * fill in the rifs
     */
    riflen = (norif) ? 0 : RIF_LENGTH(rif[0]);
    trframe = (trrif_t *)(pak->network_start - TRING_ENCAPBYTES - riflen);
    if (norif == FALSE)
	bcopy(rif, &trframe->rc_blth ,riflen);

    pak->datagramstart = (uchar *) trframe;

    /*
     * Mac headers
     */
    trframe->ac = AC_PRI0;
    trframe->fc = FC_TYPE_LLC;
    ieee_copy(dst, trframe->daddr);
    ieee_copy(src, trframe->saddr);
    if (norif == FALSE)
	trframe->saddr[0] |= TR_RII; /* always contains rif */
				/* ...well, now I wouldn't say that... */


    /*
     * LLC headers
     */
    temp_llc1_ptr = (sap_hdr *)pak->network_start;
    temp_llc1_ptr->dsap = llc1->dsap;
    temp_llc1_ptr->ssap = llc1->ssap;
    temp_llc1_ptr->control = llc1->control;

    /* 
     * copy any data 
     */
    if(payload_len)
	bcopy(payload_ptr, (uchar *) &(temp_llc1_ptr->data[0]), 
	      payload_len);

    pak->datagramsize  = TRING_ENCAPBYTES + riflen + sizeof(sap_hdr) + 
			 payload_len;

}

/*
 * 		   CLSI Primitive: udatastn.req
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC_UDATA_STN_REQ, &param_block)
 */
static inline clsLlcMapFnT llcUDataStnReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    DlcUdataStnT *dlcUdataStn;
    sap_hdr 	llc1;
    hwidbtype	*idb;
    idbtype	*swidb;
    paktype     *pak;
    trrif_t     *trh;
    ushort      riflen;


    dlcUdataStn = &clsBlock->fParameters.fDlcUdataStn;

    swidb = DlcToIdb(&clsBlock->fPortId);
    if(swidb == NULL) 	{
	if (LLC2_DEBUG_PACKET || LLC2_DEBUG_STATE || LLC2_DEBUG_ERR)
	    buginf("\ndlcllc: Invalid IDB in UdataStn request, ignored.");
    	return;
    }

    idb = swidb->hwptr;
    pak    = clsBlock->fPakPtr;
    if (!pak) 	{
	buginf("\nTR: No buffer for UdataStn.%s to %e", dlcUdataStn->fRMac); 
	return;
    }

    llc1.dsap = dlcUdataStn->fRSap;
    llc1.ssap = dlcUdataStn->fLSap;
    llc1.control = LLC1_UI;  /* assume cmd/poll */

    /*
     * pass in if_output to llcStnEncap via pak structure.  idb is
     * required to allow fn to make SR or TB encapsulation decision...
     */
    pak->if_output     = idb->firstsw;
    pak->enctype       = idb->enctype;

    llcStnEncap(pak, dlcUdataStn->fLMac, dlcUdataStn->fRMac,
		(sap_hdr *) &llc1, dlcUdataStn->fRif, clsBlock->fLoadLength,
		clsBlock->fRawData);

    pak->if_output     = idb->firstsw;
    pak->enctype       = idb->enctype;
    /*
     * pak->linktype must be set properly for out-going packets.
     * Normally set from llc->linktype, but we don't have llc2 control
     * structure yet!  So if we're Tx'ing out vidb, force to LINK_CLS...
     */
    if ((idb->status == (IDB_VIRTUAL | IDB_TR)) || 
       (idb->status == (IDB_VIRTUAL | IDB_FDDI)))
	pak->linktype = LINK_CLS;

    if ((pak->if_output) && (pak->enc_flags & ENC_DLSW) &&
       (is_fddi(pak->if_output->hwptr))) {

        fddi_hdr *fddi;

        pak->datagramsize = pak->datagramsize - TRING_ENCAPBYTES + FDDI_ENCAPBYTES;
        pak->datagramstart = pak->datagramstart + TRING_ENCAPBYTES-FDDI_ENCAPBYTES;
        fddi = (fddi_hdr *)pak->datagramstart;
        if (idb->type == IDBTYPE_FDDIT) {
            ieee_swap(fddi->saddr, fddi->saddr);
            ieee_swap(fddi->daddr, fddi->daddr);
        }
        fddi->fc = FDDI_LLC_FC_BYTE;
    }

    trh = (trrif_t *) pak->datagramstart;
    riflen = trh->rc_blth & RC_LTHMASK;

    if (!reg_invoke_srb_out_check((uchar *) pak->datagramstart,
               pak->datagramsize, idb, riflen)) {
        datagram_done(pak);
        return;
    }

    datagram_out(pak);
    return ;
}



/*
 * 		   CLSI Primitive: Idstn.req
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC_ID_STN_REQ, &param_block)
 */
static inline clsLlcMapFnT llcIdStnReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    boolean ret_code;

    ret_code = llcIdStnTxPak(clsBlock, TRUE);
    if (LLC2_DEBUG_PACKET)
	buginf("\n Send IdStn.Request - %s; len = %d ",
	       ret_code ? "success" : "failed", clsBlock->fLoadLength); 
}


/*
 * 		   CLSI Primitive: Idstn.rsp
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC_ID_STN_RSP, &param_block)
 */
static inline clsLlcMapFnT llcIdStnRsp (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    boolean ret_code;

    ret_code = llcIdStnTxPak(clsBlock, FALSE);
    if (LLC2_DEBUG_PACKET)
	buginf("\n Send IdStn.Response - %s; len = %d ",
	       ret_code ? "success" : "failed", clsBlock->fLoadLength); 
}




/*
 * Utility routine that gets used for llcIdStnRsp
 */
static inline boolean llcIdStnTxPak(ClsDlcParamBlockT *clsBlock, 
				      boolean cmd_resp_flag)
{

    DlcIdStnT *dlcIdStn;
    sap_hdr 	llc1;
    hwidbtype	*idb;
    idbtype	*swidb;
    paktype     *pak;

    dlcIdStn = &clsBlock->fParameters.fDlcIdStn;

    swidb = DlcToIdb(&clsBlock->fPortId);
    if(swidb == NULL) 	{
	if (llc2_debug_err)
	    buginf("\ndlcllc: Invalid IDB in IdStn.%s request, ignored.",
		   cmd_resp_flag ? "Request" : "Response");
    	return FALSE;
    }

    idb = swidb->hwptr;
    pak = clsBlock->fPakPtr;
    if (!pak) 	{
	buginf("\nTR: No buffer for IdStn.%s to %e", 
	       cmd_resp_flag ? "Request" : "Response", dlcIdStn->fRMac);
	return FALSE;
    }

    llc1.dsap = dlcIdStn->fRSap;
    llc1.ssap = dlcIdStn->fLSap;
    if (!cmd_resp_flag)
	llc1.ssap |= SAP_RESPONSE;
    llc1.control = LLC1_XID | LLC1_P;  /* assume cmd/poll */

    /*
     * pass in if_output to llcStnEncap via pak structure.  idb is
     * required to allow fn to make SR or TB encapsulation decision...
     */
    pak->if_output = idb->firstsw;
    pak->enctype = idb->enctype;

    llcStnEncap(pak, dlcIdStn->fLMac, dlcIdStn->fRMac,
		 (sap_hdr *) &llc1, dlcIdStn->fRif, clsBlock->fLoadLength,
		 clsBlock->fRawData);

    /*
     * pak->linktype must be set properly for out-going packets.
     * Normally set from llc->linktype, but we don't have llc2 control
     * structure yet!  So if we're Tx'ing out vidb, force to LINK_CLS... 
     */
    if ((idb->status == (IDB_VIRTUAL | IDB_TR)) || 
       (idb->status == (IDB_VIRTUAL | IDB_FDDI)))
	pak->linktype = LINK_CLS;

    if ((pak->if_output) && (pak->enc_flags & ENC_DLSW) &&
       (is_fddi(pak->if_output->hwptr))) {

        fddi_hdr *fddi;

        pak->datagramsize = pak->datagramsize - TRING_ENCAPBYTES + FDDI_ENCAPBYTES;
        pak->datagramstart = pak->datagramstart + TRING_ENCAPBYTES-FDDI_ENCAPBYTES;
        fddi = (fddi_hdr *)pak->datagramstart;
        if (idb->type == IDBTYPE_FDDIT) {
            ieee_swap(fddi->saddr, fddi->saddr);
            ieee_swap(fddi->daddr, fddi->daddr);
        }
        fddi->fc = FDDI_LLC_FC_BYTE;
    }
    datagram_out(pak);
    return TRUE;
}


void llc_wput (DlcPCepIdT pCepId, DlcOpcodeT opcode,
	       ClsDlcParamBlockT *clsParamBlock)
{
    switch (opcode)
    {
    case DLC_INIT_SYSTEM_REQ:
	llc2InitSystem((llctype *)pCepId, clsParamBlock); break;
	
    case DLC_ENABLE_REQ:
	reg_invoke_llc2EnableReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_DISABLE_REQ:
	reg_invoke_llc2DisableReq((llctype *) pCepId, clsParamBlock); break;

    case DLC_ACTIVATE_SAP_REQ:
	llc2ActivateSapReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_MODIFY_SAP_REQ:
	llc2ModifySapReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_DEACTIVATE_SAP_REQ:
	llc2DeactivateSapReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_REQ_OPNSTN_REQ:
	llc2ReqOpnStnReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_CLOSE_STN_REQ:
	llcCloseStnReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_MODIFY_STN_REQ:
	llcModifyStnReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_ID_REQ:
	llc2IdReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_CONNECT_REQ:
	llc2ConnectReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_CONNECT_RSP:
	llc2ConnectRsp((llctype *)pCepId, clsParamBlock); break;

    case DLC_SIGNAL_STN_REQ:
	llc2SignalStnReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_DISC_REQ:
	llc2DisconnectReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_DISC_RSP:
	llc2DisconnectRsp((llctype *)pCepId, clsParamBlock); break;

    case DLC_DATA_REQ:
	llc2DataReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_FLOW_REQ:
	llc2FlowReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_ACTIVATE_RING_REQ:
	reg_invoke_llc2ActivateRngReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_DEACTIVATE_RING_REQ:
	reg_invoke_llc2DeActivateRngReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_ID_RSP:
        llc2IdRsp((llctype *)pCepId, clsParamBlock); break;

    case DLC_TEST_REQ:
        llc2TestReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_TEST_RSP:
        llc2TestRsp((llctype *)pCepId, clsParamBlock); break;

    case DLC_UDATA_REQ:
        llc2UDataReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_ID_STN_REQ:
        llcIdStnReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_ID_STN_RSP:
        llcIdStnRsp((llctype *)pCepId, clsParamBlock); break;

    case DLC_TEST_STN_REQ:
        llcTestStnReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_TEST_STN_RSP:
        llcTestStnRsp((llctype *)pCepId, clsParamBlock); break;

    case DLC_UDATA_STN_REQ:
        llcUDataStnReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_PONG_REQ:
	llc2PongReq((llctype*) pCepId, clsParamBlock); break;

    default:
	CLSASSERT(!"llc_wput: Invalid opcode\n");
    }
}

/*
 * LLcByPak
 *
 * Given a parsed packet find an llc to go with it
 * Return NULL if there isn't one.
 *
 * Given a packet, find the llctype.
 *
 * If this packet has the transparent bridge in flag on, then it
 * came in encapsulated, so treat it as ethernet. Otherwise,
 * find the appropriate media-specific address.
 */

llctype *LLcByPak (idbtype *idb, paktype *pak)
{
    uchar     	dtestring[LLC_MAX_DTE_SIZE];
    sap_hdr	*llc1;
    hwaddrtype  tempAddr;

    memset(dtestring, 0, LLC_MAX_DTE_SIZE);
    llc1 = (sap_hdr *)pak->info_start;

    (*idb->hwptr->extract_snpa)(pak, &tempAddr, DESTINATION);
    if (tempAddr.type == STATION_IEEE48)
	ieee_copy(tempAddr.addr, &dtestring[LMAC_DTE]);
    else
	buginf("\nLLcByPak: non-IEEE address! type %d", tempAddr.type);

    (*idb->hwptr->extract_snpa)(pak, &tempAddr, SOURCE);
    if (tempAddr.type == STATION_IEEE48)
	ieee_copy(tempAddr.addr, &dtestring[RMAC_DTE]);
    else
	buginf("\nLLcByPak: non-IEEE address! type %d", tempAddr.type);

    /*
     * The incoming packet is addressed to us, so...
     * it's dsap is our local (source) sap.
     * it's ssap is our remote (destination) sap
     */
    dtestring[LSAP_DTE] = llc1->dsap;
    dtestring[RSAP_DTE] = llc1->ssap & ~SAP_RESPONSE;

    return(llc2_findcirc(dtestring, idb));
}

/*
 * llc1_gen_explr - Generate explorer packet.
 *
 * Formerly tr_gen_explr.
 *
 * Setup a test packet.  Mainly a matter of getting a buffer and
 * stuffing a lot of constants.
 *
 * daddr is where this packet will go.  Saddr is who to say it is
 * from.
 *
 * interface, which idb is sourcing the packet
 */

paktype *llc1_gen_explr (
    uchar *daddr,
    uchar *saddr,
    uchar dsap,
    uchar ssap,
    hwidbtype *interface)
{
    paktype	*pak;

    if (llc2_debug_packet)
        buginf("\nllc1: llc1_gen_explr: daddr = %e, saddr %e", daddr, saddr);
    pak = Llc1GenUFrame(daddr, saddr, dsap, ssap, LLC1_TEST, interface);
    if (pak == NULL && llc2_debug_err) {
	buginf("\nllc1: No buffer for TEST daddr = %e, saddr %e", 
	       daddr, saddr);
    }
    return (pak);
}




