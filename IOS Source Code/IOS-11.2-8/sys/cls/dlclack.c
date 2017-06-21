/* $Id: dlclack.c,v 3.13.8.8 1996/08/28 12:39:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlclack.c,v $
 *------------------------------------------------------------------
 * dlclack.c - Glue for converting from CLS to RSRB/LACK
 *
 * November 1994, Paul Pearce
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Routines needed to map from CLS to existing RSRB/LACK code base
 * Includes vring creation.
 *------------------------------------------------------------------
 * $Log: dlclack.c,v $
 * Revision 3.13.8.8  1996/08/28  12:39:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.8.7  1996/08/26  15:04:21  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.13.8.6  1996/08/17  21:43:51  ppearce
 * CSCdi66186:  75k crashes w/ALIGN-1-FATAL after multiple RSRB tests
 * Branch: California_branch
 *   - Check malloc return before initializing SRB subblock
 *   - Free SRB subblock when vRing unconfigured to prevent memory leak
 *
 * Revision 3.13.8.5  1996/08/03  23:21:06  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.13.8.4  1996/07/09  05:48:11  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.13.8.3  1996/06/01  01:30:43  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.13.8.2  1996/05/17  10:42:24  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.12.2.7  1996/04/26  15:16:30  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.6  1996/04/26  07:33:07  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.12.2.5  1996/04/24  06:28:17  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.12.2.4  1996/04/05  06:05:29  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.12.2.3  1996/04/03  13:54:35  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.12.2.2  1996/03/17  17:36:50  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.12.2.1  1996/03/16  12:08:44  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.13.8.1  1996/03/18  19:10:11  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.2  1996/03/16  06:32:18  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.2.1  1996/03/13  01:14:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.13  1996/03/01  00:48:19  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up correctly
 * Restore CSCdi47275 w/fix to netbios_as.c
 *
 * Revision 3.12  1996/02/21  02:43:11  hampton
 * Clean up platform specific SRB defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.11  1996/02/16  18:08:47  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Backout CSCdi47275 temporarily-- it breaks igs-c-l build
 *
 * Revision 3.10  1996/02/15  01:37:16  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Fix cleanup and change appl. cleanup from registry to vector
 *
 * Revision 3.9  1996/01/18  20:37:43  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1995/12/30  00:24:19  hou
 * CSCdi37413:  RTBR bridging fails for Ether->Token->Ether on vines and
 * ip.
 * - Set the default OUI for SNAP on token ring interface as 0x000000
 *   instead of 0x0000f8.
 * - Make low-end "ethernet-transit-oui" fully functional
 *
 * Revision 3.7  1995/12/15  22:45:16  rbatz
 * CSCdi43378:  Explorers destined for SDLLC Virtual Ring forwarded
 *              to wrong ring.  Rework of vr_virtualp required.
 *
 * Revision 3.6  1995/11/30  02:59:42  syiu
 * CSCdi44150:  Remote Node NetBEUI on Token Ring platform not working
 *
 * Revision 3.5  1995/11/20  18:37:10  rbadri
 * CSCdi38662:  Configuring bridging on multiple ATM sub-interfaces causes
 *              hardware reset
 * Make span_fixup() recognize sub-interfaces, so that for ATM interfaces,
 * we can initiate a config change rather than a hardware reset.
 *
 * Revision 3.4  1995/11/20  16:39:06  ppearce
 * CSCdi43369:  CLSI DeactivateRing failure
 *   llc2DeActivateRngReq() accesses SAP value from wrong parm structure
 *
 * Revision 3.3  1995/11/17  08:55:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:14:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:11:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/11/08  20:53:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.12  1995/10/11  04:20:18  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.11  1995/10/10  19:16:21  vrawat
 * CSCdi40403:  Enabling and disabling virtual interfaces crashes router
 *
 * Revision 2.10  1995/10/03  07:19:02  kmoberg
 * CSCdi37823:  Changing bridge protocol with DLSW enabled causes crash.
 * dlsw bridge-group command broken.
 *
 * Revision 2.9  1995/08/07  05:30:00  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.8  1995/07/20  21:13:22  kchiu
 * CSCdi37258:  FRAS Dial-Backup for multiple stations failed
 *
 * Revision 2.7  1995/07/19  20:58:29  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add support for Activate/Deactivate Ring
 *
 * Revision 2.6  1995/07/06  18:42:05  ccartee
 * CSCdi36735:  DLSw/EN 80d5 support completely broken
 * Isolate feature 'enable' check and packet transformations into seperate
 * functions.  Use these recycled functions for adding 80d5 functionality
 * to DLSw code.  Duplicate functionality in legacy code is removed and
 * use recycled code instead...
 *
 * Revision 2.5  1995/07/02  05:15:31  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.4  1995/07/01  19:14:43  richl
 * CSCdi34435:  %SYS-3-SUPNONE: Registry 6 doesnt exist
 *              o Changed above msg so it isn't rate based
 *              o Protected call to registry with conditional statement
 *              o Resolved conflict in DRIVER/PROTOCOL class between
 *                the cbus_bridge and tbridge subsystems.
 *
 * Revision 2.3  1995/06/18  19:40:01  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/15  23:00:01  ppearce
 * CSCdi35921:  CLS/RSRB virtual ring not removed after "appn stop"
 *
 * Revision 2.1  1995/06/07  20:20:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#include "master.h"
#include "logger.h"
#include "../cls/msg_dlc.c"		/* Not a typo, see logger.h */
#include "packet.h"
#include "address.h"
#include "subsys.h"
#include "sys_registry.h"

#include "interface_generic.h"
#include "../if/tring.h"
#include "../srt/trsrb.h"
#include "../srt/srb_core.h"
#include "../tbridge/tbridge_sde.h"          /* included for SDE_ENCAPBYTES */
#include "../if/tring_common.h"
#include "../if/rif_inline.h"
#include "../if/tring_inline.h"
#include "../if/if_vidb.h"	/* prototypes for vidb search fn's */
#include "../if/network.h"

#include "../cls/cls_private.h"
#include "../cls/cls_assert.h"

#include "../llc2/llc2.h"
#include "../llc2/lltimers.h"
#include "../llc2/llc2_inline.h"

#include "../ibm/netbios_sb.h"

#include "../srt/rsrb.h" 
#include "../srt/lack_externs.h" 
#include "../srt/lack2fsm.h"
#include "../srt/lack2_externs.h"
#include "../srt/srb.h"
#include "../srt/span.h"	/* access to span_* functions to allow
				   linkage to transparent bridge
				   code. */
#include "../srt/srt_registry.h"

/* Define local LAK routines to handle CLS-to-LAK events:      */
/*                                                             */
/*   lakConnectReq()    - process Connect.Req from DLU         */
/*   lakConnectRsp()    - process Connect.Rsp from DLU         */
/*   lakSignalStnReq()  - process Signal_stn.Req from DLU      */
/*   lakDataReq()       - process Data.Req from DLU            */
/*   lakFlowReq()       - process Flow.Req from DLU            */
/*   lakDisconnectReq() - processs Disconnect.Req from DLU     */ 
/*                                                             */
/* ----------------------------------------------------------- */
static clsLlcMapFnT lakConnectReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT lakConnectRsp(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT lakSignalStnReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT lakDataReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT lakFlowReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
static clsLlcMapFnT lakDisconnectReq(llctype *llc, ClsDlcParamBlockT *clsBlock);

/* Define local LAK routines to handle LAK-to-CLS events:      */
/*                                                             */
/*   cls_lak2_open()   -> process LAK session establishment    */
/*   cls_lak2_close()  -> process LAK session termination      */
/*   cls_lak2_busy()   -> process LAK session quench (on/off)  */
/*   cls_lak2_abort()  -> process LAK session abort            */
/*                                                             */
/* ----------------------------------------------------------- */
static dlc_rtn_t cls_lak2_open(dlc_db_t *dlc_cb, lak_t *lackie);
static dlc_rtn_t cls_lak2_close(dlc_db_t *dlc_db, lak_t *lackie);
static dlc_rtn_t cls_lak2_busy(dlc_db_t *dlc_db, boolean busyit);
static dlc_rtn_t cls_lak2_abort(dlc_db_t *dlc_db);

static dlc_handle_t cls_dlc_handle = {
    DLC_CLS,
    cls_lak2_open,    cls_lak2_close,    cls_lak2_busy,     cls_lak2_abort
} ;

static idbtype *cls_add_vring_interface(ClsDlcParamBlockT *clsBlock);
void cls_remove_vring_interface(idbtype *idb);
static idbtype *cls_find_vidb(hwaddrtype *vMacaddr, int localRing, int targetRing);
idbtype *cls_find_virtual_port (int bridge_group, idbtype *idb);
boolean cls_is_tbridge_type (hwidbtype *idb);
static boolean cls_vidb_oqueue (hwidbtype *idb, paktype *pak, enum HEADTAIL which);
static idbtype *cls_add_tbridge_interface(ClsDlcParamBlockT *clsBlock);
static boolean DlcValidPortParams(idbtype *idb, ClsDlcParamBlockT *);
static void dlsw_rsrb_vidb_iqueue(hwidbtype *idb, paktype *pak);

void llc2EnableReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
void llc2DisableReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
void llc2DeActivateRngReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
void llc2ActivateRngReq(llctype *llc, ClsDlcParamBlockT *clsBlock);
void cls_CreateLak(llctype *llc, paktype *pak);
static void lak_wput (DlcPCepIdT pCepId, DlcOpcodeT opcode, 
	       ClsDlcParamBlockT *clsParamBlock);

static dlc_handle_t *cls_lak_get_handle (void)
{
    return (&cls_dlc_handle);
}

static void dlc_lak_init (subsystype *subsys)
{
    /*
     * Currently, nothing to do but register our functions.
     * WARNING, llc2EnableReq brings in all of RSRB
     */

    reg_add_llc2EnableReq(llc2EnableReq, "llc2EnableReq");
    reg_add_llc2DisableReq(llc2DisableReq, "llc2DisableReq");

    reg_add_llc2ActivateRngReq(llc2ActivateRngReq, "llc2ActivateRngReq");
    reg_add_llc2DeActivateRngReq(llc2DeActivateRngReq, "llc2DeActivateRngReq");


    reg_add_dlc_wput(CLS_RSRB, lak_wput, "lak_wput");
    reg_add_lack_get_dlc_handle(DLC_CLS, cls_lak_get_handle, 
				"cls_lak_get_handle");
    reg_add_cls_find_virtual_port(cls_find_virtual_port,
				  "cls_find_virtual_port");
    reg_add_cls_is_tbridge_type(cls_is_tbridge_type,
                                  "cls_is_tbridge_type");
}

/*
 * CLS subsystem header
 */

#define DLCLACK_MAJVERSION 1
#define DLCLACK_MINVERSION 0
#define DLCLACK_EDITVERSION  1

SUBSYS_HEADER(dlc_lack, DLCLACK_MAJVERSION, 
	      DLCLACK_MINVERSION, DLCLACK_EDITVERSION,
	      dlc_lak_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: srb, llc2",
	      "req: srb, llc2");

/* Fake it for now */

static boolean DlcValidPortParams(idbtype *idb, ClsDlcParamBlockT *clsBlock)
{
    return(TRUE);
}

/*
 *
 */
static idbtype *cls_find_vidb(hwaddrtype *vMacaddr, int localRing, int targetRing)
{
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;

    vrg = vrfind_ringgroup(targetRing);
    if (!vrg)
        return((idbtype*)NULL);
    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
        if (vre->rn == localRing && ieee_equal((uchar *)vMacaddr->addr, vre->macaddr)) 
            return(firstsw_or_null(vre->virtual_idb));
    }
    return((idbtype*)NULL);
}

/*
 * Globals for this module only.
 */
static boolean gLLCInitialized = FALSE;

/*
 * c l s _ a d d _ v r i n g _ i n t e r f a c e ( )
 *
 * Create a new CLS 'appendix' ring and tie it into an existing ring
 * group. Routine assumes RSRB supporting infrastructure in place, if
 * not, routine returns an error in clsBlock.
 *
 * Resulting configuration (with desired fields enclosed in [])

    [fTargetRing]            [fBridgeNumber]           [fLocalRing]
   *******************                              ******************
  * Source-Bridge     *  +----------------------+  *                  *
  * Ring-Group 'ring' +--| CLS 'Virtual' Bridge |--* CLS Virtual Ring *
  *                   *	 +----------------------+  *                  *
   ******************                               ******************
                                                            |
						       +**********+
						       * CLS Vidb *
						       +**********+
                                                       [fVMacaddr]
 */

static idbtype *cls_add_vring_interface(ClsDlcParamBlockT *clsBlock)
{
    RSRBEnableT* rsrbEnable = &clsBlock->fParameters.fDlcEnable.fMediaEnable.fRSRBEnable;

    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    hwidbtype 	*idb;

    /* Make sure no TBRIDGE'ers try and sneak in */
    CLSASSERT(clsBlock->fParameters.fDlcEnable.fDLCType == CLS_RSRB);

    /*
     * Verify that localRing number is not already in use
     *
     */
    vrg = vrfind_ringgroup(rsrbEnable->fLocalRing);
    if (vrg)
    {
      clsBlock->fResultError = CLS_INVALID_LOCAL_RING;
      return(NULL);
    }

    /*
     * Verify that targetRing is defined
     *
     */
    vrg = vrfind_ringgroup(rsrbEnable->fTargetRing);
    if (!vrg)
    {
	clsBlock->fResultError = CLS_INVALID_TARGET_RING;
	return(NULL);
    }

    vre = malloc_vre(NULL);
    if (!vre)
    {
	clsBlock->fResultError = CLS_OUT_OF_MEMORY;
	return(NULL);
    }
    
    /*
     * Default fBridgeNumber if not defined by DLU 
     *   fBridgeNumber will be zero if not explicitly set by DLU
     *
     * Some day when BridgeNumber == 0 is accepted
     *   this check to default the BridgeNumber to 1 can be removed
     *
     *
     */
    if (rsrbEnable->fBridgeNumber == 0x0)
        rsrbEnable->fBridgeNumber = CLS_RSRB_DEFAULT_BRIDGE_NUM;

    idb = dlc_new_vidb(NULL, "Buff Ring",
		       rsrbEnable->fLocalRing, 
		       rsrbEnable->fBridgeNumber,
		       rsrbEnable->fTargetRing);

    if (!idb) {
       free(vre);
       clsBlock->fResultError = CLS_OUT_OF_MEMORY;
       return(NULL);
    }

    vre->virtual_idb = idb;
    idb->iqueue = rsrb_vidb_iqueue;	/* end-station input handler */
    idb->oqueue = cls_vidb_oqueue;

    vre->bn      = rsrbEnable->fBridgeNumber;
    vre->rn      = rsrbEnable->fLocalRing;
    vre->type    = RSRB_ET_LOCAL_VIRTUAL;
    vre->outhole.idb = vre->virtual_idb;
    vre->virtualp = virtualp_default;

    vre->virtual_idb->tr_vring_blk = (tr_ventrytype *)&vrg->rings.qhead;
    snpa_copy_to_buffer(&rsrbEnable->fVMacAddr, vre->virtual_idb->bia);
    snpa_copy_to_buffer(&rsrbEnable->fVMacAddr, vre->virtual_idb->hardware);
    snpa_copy_to_buffer(&rsrbEnable->fVMacAddr, vre->macaddr);

    enqueue(&vrg->rings, vre);
    vrg->flags |= VRGF_GENUPDATE;
    update_peers();
    return(firstsw_or_null(vre->virtual_idb));
}

void cls_remove_vring_interface(idbtype *idb)
{

    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    srb_triplet_t srb_triplet;
    leveltype     status;

    srbV.srb_sb_get_triplet(idb, &srb_triplet);
    vrg = vrfind_ringgroup(srb_triplet.remoteRing);
    if (vrg) {
 
       vre = vrfind_rn2entry(srb_triplet.localRing, RSRB_BN_DONTCARE,
                 (tr_ventrytype *) &vrg->rings.qhead, RSRB_ET_LOCAL_VIRTUAL);
 
       if (vre) {
 
          status = raise_interrupt_level(NETS_DISABLE);
          unqueue(&vrg->rings, vre);
          if (vre->virtual_idb) {
              srbV.srb_destroy_sb(vre->virtual_idb->firstsw);
              vidb_free(vre->virtual_idb->firstsw);
          }
          free(vre);
          reset_interrupt_level(status);
 
        } /* vre found */
 
        vrg->flags |= VRGF_GENUPDATE;
        update_peers();
 
    } else if (cls_debug_err) {

        buginf("\nCouldn't find vrg for Ring: %d", srb_triplet.remoteRing);

    } /* End else - vrg not found */

} /* End cls_remove_vring_interface() */


static boolean cls_vidb_oqueue (hwidbtype *idb, paktype *pak,
				enum HEADTAIL which)
{

    tring_hdr *trh;             /* Pointer to TR MAC header                */


    switch (pak->linktype)	/* linktype defines _source_ of packet */
    {
    case LINK_CLS:		/* from cls virtual ring -> rsrb transport */
    {
	trrif_t *trh = (trrif_t *)pak->datagramstart;

	clear_if_input(pak);
	pak->if_input = idb->firstsw;
	pak->if_output = NULL;

	if ((trh->rc_blth & RC_BROADBIT)) {
	      pak->flags |= PAK_SRB_IN;
	      srb_enq(srb_explorerQ, pak);
	      break;
	} else 
	{
	    /*
	     * Explicitly set type parameter as RSRB_OP_FORWARDCRC to prevent
	     * superfluous lack processing
	     */
	    vrforward_pak(pak, TRUE, RSRB_OP_FORWARDCRC, RSRB_RIFLF_LEAVEALONE);
	}
	break;
	
    } /* endCase LINK_CLS */


    case LINK_RSRB:		/* from rsrb transport -> cls virtual ring */
	clear_if_input(pak);
	pak->if_input = pak->if_output;

       /*
        * Verify Destination MAC address in packet matches 
        *   the virtual MAC address in the idb before 
        *   calling idb->iqueue vector
        *
        * idb->iqueue vector is set to rsrb_vidb_iqueue()
        *   for processing of TEST/XID/LLC2 packets
        */
        trh = (tring_hdr *) pak->datagramstart;
        if (ieee_equal(trh->daddr, idb->hardware)) {
   	    (*idb->iqueue)(idb, pak);
        } else {
            dlsw_rsrb_vidb_iqueue(idb, pak);
        }
	break;

    default:
        if (cls_debug_err) {
	    buginf("\n Incorrect linktype in cls_vidb_iqueue: %d, %s",
	        pak->linktype, link2name(pak->linktype));
        }
        datagram_done(pak);
        break;

    } /* endSwitch(pak->linktype) */

	return(FALSE);
}



/*
 * Create a new virtual interface and tie it into an existing bridge
 * group. Routine does not assume that existing bridge structure is in
 * place.
 *
 * Resulting configuration (with desired fields enclosed in [])

                 ******************
                 *                *
                 *    D  L  U     *
                 *                *
                 ******************
                         #
                         #
                   #############
	           # DLU  Vidb # <-- [tbridgeEnable->fVMacAddr]
                   #############
                         #  
                         #  <--- [tbridgeEnable->fOqueueFn]
			 #       (queuing fn for i/o to/from bgroup)
                         #  
      --------------*#####   <--- [tbridgeEnable->fSpanIndex]
                    |
                    |
       (Ethernet)   |
                    |
                    |
*/

static idbtype *cls_add_tbridge_interface(ClsDlcParamBlockT *clsBlock)
{
    TBRIDGEEnableT* tbridgeEnable =
	&clsBlock->fParameters.fDlcEnable.fMediaEnable. fTBRIDGEEnable;

    hwidbtype 	*idb;
    idbtype 	*swidb;
    spantype    *span;
    
    /*
     * Note 'illegal' NULL values for ring and bridge numbers passed
     * into dlc_new_vidb().  These are not characteristic of vidbs
     * used for SR bridging, and this is used to identify these as
     * TBRIDGE idbs.  If you change this be sure to fix
     * cls_find_virtual_port() below, so these vidbs can be
     * properly identified...
     */
    idb = dlc_new_vidb(NULL, "DLSw Port", 0, 0, 0);
    if (!idb) {
	  clsBlock->fResultError = CLS_OUT_OF_MEMORY;
	  return(NULL);
      }
    swidb = idb->firstsw;

    /*
     * As per example in rsrb_create_virtual_bridge(), tidy up behind
     * dlc_new_vidb() so bridging fields are set properly. */

    /*
     * TBRIDGE ports may cover Ethernet, Token-Ring or FDDI
     * interfaces, since we're not sure what will be underneath us (or
     * out on the network between us and end-node), assume the worst
     * and limit maxmtu to 1500 minus some space for the header.
     */
    idb->maxmtu = RC_LF1500_VAL - LLC_DATA_OFFSET;
    srbV.srb_sb_set_rsrb_transit_oui(idb->firstsw, DEFAULT_OUI);
    idb->tr_transit_oui = DEFAULT_OUI;

    /*
     * following is GETOBJ(int, 6);  what is this in sr/tlb command?
     */

    netbios_sb_disable_name_caching(swidb);

    idb->iqueue = (iqueue_t) (-1);
    idb->oqueue = (oqueue_t) tbridgeEnable->fOqueueFn;
    
    /*
     * Initialize Transparent bridging fields
     *
     * For now disable transmission of BPDU's onto DLSw ports.  In the
     * future, DLSw will need to accept and forward BPDU's so that we
     * can support multiple DLSw connections between LANs.
     * span_disabled == TRUE, causes BPDU's to be dropeed in ieee_out().
     */
    swidb->span_index = tbridgeEnable->fSpanIndex;
    swidb->port_id = swidb->if_number;
    swidb->number_bit_mask =  1; /* Real interfaces start at bit 1 */
    swidb->path_cost = 10;

    swidb->span_disabled = TRUE;
    
    snpa_copy_to_buffer(&tbridgeEnable->fVMacAddr, idb->bia);
    snpa_copy_to_buffer(&tbridgeEnable->fVMacAddr, idb->hardware);
    /*
     * span_lookup() has side effect of creating spanning structure if
     * it isn't present already.  Because current TBRIDGE client (DLSw
     * ;) creates TBRIDGE as a global command ahead of interface and
     * 'bridge <n> protocol' command, we need to create bridging
     * foundation here.  Otherwise, TBRIDGE won't be configured on
     * boot-up...
     */
    span = span_lookup(tbridgeEnable->fSpanIndex, TRUE, 0);
    if (!span)
    {
	clsBlock->fResultError = CLS_OUT_OF_MEMORY;
	vidb_free(swidb);
	return(NULL);
    }
    /*
     * Link TBRIDGE vidb into span structure associated with this index...
     */
    span_fixup(tbridgeEnable->fSpanIndex, idb, swidb);

    return(firstsw_or_null(idb));
}


/*
 * Given an idb, check if it is CLS_TBRIDGE CLS type.
 */
 
boolean cls_is_tbridge_type (hwidbtype *hwidb)
{
    idbtype *swidb = firstsw_or_null(hwidb);
    srb_triplet_t srb_triplet;
    srbV.srb_sb_get_triplet(swidb, &srb_triplet);
    if ((hwidb != NULL) &&
        (hwidb->status == (IDB_VIRTUAL | IDB_TR)) &&
        (hwidb->vidb_link == RSRB_INVALID_ADDRESS) &&
        (hwidb->unit == 0) &&
        (srb_triplet.localRing  == SRB_INVALID_RING_NUM) &&
        (srb_triplet.remoteRing == SRB_INVALID_RING_NUM))
        return(TRUE);
 
    return(FALSE);
}
 


/*
 * Given a bridge group and an idb, find the 'next' CLS TBRIDGE idb in
 * the vidb queue.  A NULL idb on input will return the first idb in
 * the chain.  Return a valid *swidb or NULL of no subsequent CLS
 * TBRIDGE idb found.
 */

idbtype *cls_find_virtual_port (int index, idbtype *idb)
{
    idbtype *vidb;
    
    /*
     * Check all of the characteristics of a CLS-created Transparent
     * Bridge port.  Look in cls_add_tbridge() at the parameters to
     * dlc_new_vidb() to see the 'illegal' dummy parameters used to
     * fill out the vidb.
     *
     * If all of the 'signs' match, return this idbtype as the one to
     * add to the bridge group.  Otherwise, return NULL as we may have
     * a collision with some other vidb user (i.e. SR/TLB, AST,
     * etc...) */
    while ((vidb = get_next_vidb(idb)) != NULL)
    {
        hwidbtype *hwvidb = hwidb_or_null(vidb);
        srb_triplet_t srb_triplet;
        srbV.srb_sb_get_triplet(vidb, &srb_triplet);
	if ((hwvidb != NULL) &&
	    (hwvidb->status == (IDB_VIRTUAL | IDB_TR)) &&
	    (idb_sw_get_spanindex(vidb) == index) &&
	    (hwvidb->vidb_link == RSRB_INVALID_ADDRESS) &&
	    (hwvidb->unit == 0) &&
	    (srb_triplet.localRing  == SRB_INVALID_RING_NUM) &&
	    (srb_triplet.remoteRing == SRB_INVALID_RING_NUM))
	    break;

	idb = vidb;
    }
    return(vidb);    
}


/************************************************************************
 *			   CLSI Primitive: Enable.req
 ************************************************************************
 *
 * Initialize port with hardware specific paramters.
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC2_ENABLE_REQ, &param_block)
 *
 *
 * Old Code Function
 * -----------------
 * (none)/New Function
 *
 *
 * Enhancements
 * ------------
 * New function will be required to configure a modified
 * tr_process_test() and tr_process_xid() to respond to NULL SAP.
 * Potentially envision array of SAP length, each entry has a flag to
 * indicate how to respond to incoming frame.
 *
 * Modifications to tr_process_test();
 * 	1. Remove hard coded references to SAPs.
 *
 * 	2. Create initializer (static or within llc2_init() to add
 * 	entries for those removed in #1
 *
 * 	3. Routine autonomously responds to NULL SAP frames, pass
 * 	others up.
 *
 * Error Conditions
 * ----------------
 * If llc2 subsystem hasn't been initialized, return error.  System may
 * not be initialized because of memory shortage or not configured in box
 *  (i.e. modular image)
 *
 ************************************************************************ */

void llc2EnableReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    idbtype		*idb;
    sap_registry_t 	*sapRegistry;
    DlcEnableT		*dlcEnable;
    hwidbtype           *hwidb;    

    /* Check if the port exists and is capable of running LLC2 */
    
    dlcEnable = &clsBlock->fParameters.fDlcEnable;
    idb = DlcToIdb(&dlcEnable->fPortId);

    switch (dlcEnable->fDLCType)
    {
    case CLS_RSRB:
    {
	/*
	 * Since this is a virtual interface, and it is one that we
	 * haven't seen before, we want to try to create it.
	 */
	RSRBEnableT* rsrbEnable;
	      
	rsrbEnable = &clsBlock->fParameters.fDlcEnable.fMediaEnable.fRSRBEnable;
	      
	CLSASSERT(snpa_is_valid(&rsrbEnable->fVMacAddr));
	      
	if (cls_find_vidb(&rsrbEnable->fVMacAddr, rsrbEnable->fLocalRing,
			  rsrbEnable->fTargetRing)) {
	    dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_ALREADY_ENABLED);
	    return;
	}
	/*
	 * Create a new virtual ring and a new idb to hang onto it.
	 * All rings and MAC address configuration can be obtained
	 * from clsBlock.  Pass in ptr to entire structure, so we can
	 * use result field and prevent parameter creep.
	 */
	idb = cls_add_vring_interface(clsBlock);
        if (idb == NULL) {
    	    dlc_rput(clsBlock, CLS_ENABLE_CNF, clsBlock->fResultError);
	    return;
        }
	/*
	 * Initialize SRB subsystem until modularization bugs
	 * get worked out...
	 */
	start_srb_background();
    } /* endCase CLS_RSRB */
	break;
	      
    case CLS_TBRIDGE:
	/*
	 * NULL return code on idb checked on fall-through...
	 */
	idb = cls_add_tbridge_interface(clsBlock);

	break;

	/*
	 * All other CLS port types currently (1/17/95) should be
	 * passing in non-NULL idb pointers.  Check them on fall
	 * through
	 */

    default:
	break;

    } /* endSwitch fDLCType */
	  

    if (idb == NULL) {
	dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    /*
     * If this is the first call to an interface using LLC2, then we
     * need to start up the llc2 subsystem. It is OK to call this
     * routine multiple times so don't bother to check if the
     * subsystem is running or not.
     */

    if (gLLCInitialized == FALSE) {
	  llc2_start();
	  gLLCInitialized = TRUE;
      }
    
    /* 
     * If the sap registry doesn't exist for this port yet, create it.
     */
    
    sapRegistry = sap_get_registry(idb);
    if (sapRegistry == NULL) {
	sapRegistry = sap_create_registry(idb);
	if (sapRegistry == NULL) {
	    dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_OUT_OF_MEMORY);
	    return;
	}
    }
	
    /* 
     * If this port requires/allows input parameters, then check that
     * they make sense. 
     */

    if (!DlcValidPortParams(idb, clsBlock)) {
	dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_CONFIG_CONFLICT);
	return;
      }

    dlcEnable->fPPortId = (DlcPPortIdT) idb; /* save the port ID for future use */
    hwidb = idb->hwptr;

    switch (dlcEnable->fDLCType) {
      case CLS_LLC:
      {	/* code is broken, falsely assumes LLC==TKR */
	  TREnableT* trEnable =
	      &clsBlock->fParameters.fDlcEnable.fMediaEnable.fTREnable;

	  trEnable->fVersion = CLSI_VERSION;
	  (*hwidb->extract_hwaddr)(hwidb, &trEnable->fLocalMac);
	  trEnable->fTrSpeed = hwidb->tr_ring_speed;
	  trEnable->fMaxIFieldLength = llc_mtu(hwidb);
      }
	  break;

      case CLS_TBRIDGE:
      {
	  TBRIDGEEnableT* tbridgeEnable =
	      &clsBlock->fParameters.fDlcEnable.fMediaEnable.fTBRIDGEEnable;

	  tbridgeEnable->fVersion = CLSI_VERSION;
	  (*hwidb->extract_hwaddr)(hwidb, &tbridgeEnable->fVMacAddr);
	  tbridgeEnable->fMaxIFieldLength = llc_mtu(hwidb);
      }
	  break;

      case CLS_SDLC:
      case CLS_VDLC:
      case CLS_QLLC:
	  /*
	   * Should never see SDLC or QLLC port types in the code,
	   * handled in dlcsdlc.c/dlcqllc.c
	   */
	  CLSASSERT(!"Invalid port type");
	  break;
	  

      case CLS_RSRB:
      {
	  RSRBEnableT* rsrbEnable=
	      &clsBlock->fParameters.fDlcEnable.fMediaEnable.fRSRBEnable;
	      
	  rsrbEnable->fVersion = CLSI_VERSION;
	  (*hwidb->extract_hwaddr)(hwidb, &rsrbEnable->fVMacAddr);    
	  rsrbEnable->fMaxIFieldLength = llc_mtu(hwidb);
      }
	  
	  
      case CLS_FRAME_RELAY:
      case CLS_ATM_1483:
      default:
	  break;
      }	/* endSwitch (dlcEnable->fDLCType) */

    dlc_rput(clsBlock, CLS_ENABLE_CNF, CLS_OK);

    return;
}


/************************************************************************
 *			   CLSI Primitive: Disable.req
 ************************************************************************
 *
 * Disable port 
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, DLC_DISABLE_REQ, &param_block)
 *
 *
 * Old Code Function
 * -----------------
 * (none)/New Function
 *
 * Enhancements
 * ------------
 *
 * Error Conditions
 * ----------------
 *
 ************************************************************************ */

void llc2DisableReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{

    idbtype		*idb;
    DlcDisableT		*dlcDisable;


    dlcDisable = &clsBlock->fParameters.fDlcDisable;
    idb = DlcToIdb(&dlcDisable->fPortId);
    if (idb == NULL) {
	dlc_rput(clsBlock, CLS_DISABLE_CNF, CLS_UNKNOWN_PORT);
	return;
    }

    switch (dlcDisable->fDLCType) {

      case CLS_RSRB:
      case CLS_TBRIDGE:
          cls_remove_vring_interface(idb);
          break;

      default:
          break;

    }	/* End switch (dlcDisable->fDLCType) */

    dlc_rput(clsBlock, CLS_DISABLE_CNF, CLS_OK);
}

/*
 * 		   CLSI Primitive: Activate_Ring.req
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC2_ACTIVATERING_REQ, &param_block)
 */
void llc2ActivateRngReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    idbtype *idb;
    DlcActRngT          *dlcActRng;
    LlcActRngT          *llcActRng;

    sap_registry_t      *sapRegistry;
    sap_entry_t         *sapEntry;
   
    dlcActRng = &clsBlock->fParameters.fDlcActRng;
    llcActRng = &dlcActRng->fDlcRng.fLlcActRng;
   
    idb = (idbtype *)(dlcActRng->fPPortId);
    if (idb == NULL) {
	errmsg(&msgsym(OPTNULL, DLC));
	dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_UNKNOWN_PORT);
	return;
    }

 
    sapRegistry = sap_get_registry(idb);
    if (sapRegistry == NULL)  {
	dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_NO_SAP_REGISTRY);
	return; 
    }

    sapEntry = malloc(sizeof(sap_entry_t));
    if (sapEntry == NULL) {
	  dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_OUT_OF_MEMORY);
	  return;
      } else {
	    int rc;

	    sapEntry->sap_flags = llcActRng->fSapFlags | SAP_CLS_ACT_SAP;
	    sapEntry->usapid = dlcActRng->fClsUSapId;
	    sapEntry->llc_list = NULL;
	    sapEntry->sap_handle_test = NULL;
	    sapEntry->sap_handle_ieee_xid = NULL;
	    sapEntry->sap_handle_sna_xid = NULL;
	    sapEntry->xid_class = XID_CLASS_TYPE2;
	    sapEntry->xid_window = XID_WIN_TYPE2;
	    rc = sap_register_bridging(sapRegistry, sapEntry);
    
	    if (rc != 0) {	/* fails if SAP already in use */
		  /*reg_invoke_dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_SAP_BUSY); */
		  /* source-bridge command invoked with different ring-group, break; */
	      }
	    /*
	     * Now that CLS-related data structures have been
	     * successfully allocated and initialized, we must update
	     * 'lower layer' data structures to enable data flow.
	     * Enabling command will depend on underlying DLC media.
	     * If DLC-specific command succeeds, then fall-through to
	     * common 'okee-dokey' confirmation...
	     */
	    switch (idb->hwptr->status)
	    {
		/*
		 * Enable flow of frames from transparent bridging
		 * code by initializing the spanning tree port...
		 */
	    case (IDB_VIRTUAL | IDB_TR): /* CLS_TBRIDGE ports... */
	    {
		if (idb->span_ptr &&
		    (idb->span_ptr)->running)
		    span_port_enable(idb->span_ptr, idb);
	    }
		break;
		
	    case IDB_TR:
		/*
		 * A very rough example, still need to address
		 * different types of llc ports.  i.e. all CLS_LLC's
		 * are not token-rings!
		 *
		 * local_ring     = dlcActRng -> fLocalRing;
		 * bridge_number  = dlcActRng -> fBridgeNumber;
		 * target_ring    = dlcActRng -> fTargetRing;
		 *
		 *activate_ring(idb, target_ring, local_ring, bridge_num);
		 */
	    default:		/* other DLC enables go here... */
		break;
	    } /* EndSwitch (dlcDlcEnable->fDLCType) */

	    dlc_rput(clsBlock, CLS_ACTIVATE_RING_CNF, CLS_OK);
	    return;
	} /* endElse sapEntry != NULL */
}



/*
 * 		   CLSI Primitive: DeActivate_Ring.req
 *
 * DLC Message Generated
 * ---------------------
 * llc_wput(*cep, LLC2_DEACTIVATERING_REQ, &param_block)
 */
void llc2DeActivateRngReq (llctype *llc, ClsDlcParamBlockT *clsBlock)
{
    idbtype		*idb;
    DlcDeActRngT	*dlcDeActRng;
    LlcDeActRngT	*llcDeActRng;
    sap_registry_t	*sapRegistry;
    sap_entry_t		*sapEntry;

    dlcDeActRng = &clsBlock->fParameters.fDlcDeActRng;
    llcDeActRng = &dlcDeActRng->fDlcRng.fLlcDeActRng;

    idb = (idbtype *)(dlcDeActRng->fPPortId);
    if (idb == NULL) {
	  errmsg(&msgsym(OPTNULL, DLC));
	  dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_UNKNOWN_PORT);
	  return;
      }

    sapRegistry = sap_get_registry(idb);
    if (sapRegistry == NULL) {
	  dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_NO_SAP_REGISTRY);
	  return;
      }

    /*
     * Before deallocating CLS structures, disable frame flow into
     * this control point.  de-allocated , we must update 'lower
     * layer' data structures to disable data flow.  Disabling command
     * will depend on underlying DLC media.  If DLC-specific command
     * succeeds, then fall-through to common 'okee-dokey'
     * confirmation...
     */
    switch (idb->hwptr->status)
    {
	/*
	 * Enable flow of frames from transparent bridging
	 * code by initializing the spanning tree port...
	 */
    case (IDB_VIRTUAL | IDB_TR):
    {
	/*
	 * 1. Disable spanning tree port
	 * 2. De-link from bridge group
	 * 3. Free up vidb
	 *
	 * Future work should seperate #3 into CLS_DISABLE, should
	 * this primitive ever get written.
	 */
	if (idb->span_ptr)
	{
	    if ((idb->span_ptr)->running)
		span_port_disable(idb->span_ptr, idb);

	    span_delete_port(idb);
	}
	/*
	 * vidbs are created from dlc_new_vidb(), which uses
	 * init_idb_names() to setup name string.  init_idb_names()
	 * calls setstring() which will malloc a piece of memory to
	 * store the incoming string. (Which is kinda stupid, since
	 * we're passing in a 'const char *' which won't go away
	 * anyway, but I digress...)  Since we've malloc'ed something,
	 * let's free it up.
	 */

	free(idb->namestring);	/* emancipate the memory! */
	idb->namestring = NULL;

	free(idb->short_namestring); /* be free! */
	idb->short_namestring = NULL;

	vidb_free(idb);
    }
	break;
		
    case IDB_TR:
	/*
	 * A very rough example, still need to address
	 * different types of llc ports.  i.e. all CLS_LLC's
	 * are not token-rings!
	 *
	 * local_ring     = dlcDeActRng -> fLocalRing;
	 * bridge_number  = dlcDeActRng -> fBridgeNumber;
	 * target_ring    = dlcDeActRng -> fTargetRing;
	 *
	 * deactivate_ring(idb, target_ring, local_ring, bridge_num); */
    default:			/* other DLC enables go here... */
	break;
    } /* EndSwitch (dlcDlcEnable->fDLCType) */


    /*
     * Frame flow has presumably stopped, clean up CLS data
     * structures...
     */
    sapEntry = sap_deregister_bridging(sapRegistry);
    if (sapEntry == NULL) {	
	  dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_UNKNOWN_SAP);
	  return;
      }
    if (validmem(sapEntry))
	free(sapEntry);		
    dlc_rput(clsBlock, CLS_DEACTIVATE_RING_CNF, CLS_OK);
    return;
}


/*************************************************************************/
/* Define local LAK routines to handle CLS-to-LAK events:                */
/*************************************************************************/


/*************************************************************************
 *                   CLSI Primitive: Connect.req
 *************************************************************************
 *
 * Establish RSRB/LAK connection with remote peer
 *
 * DLC Message Generated
 * ---------------------
 * lak_wput(*cep, DLC_CONNECT_REQ, &param_block)
 *
 ************************************************************************
 */

static inline clsLlcMapFnT lakConnectReq (llctype *llc, 
                                          ClsDlcParamBlockT *clsBlock)
{

    lak_t *lackie = NULL;
    leveltype level;


   /* 
    * Check if LAK structure has been created 
    *   (should have been created during XID exchange)
    */
    lackie = llc2_get_lackie(llc);
    if (!lackie) {
        if (lack_debug_err)
          buginf("\nLOCACK: lakConnectReq, null LAK ptr");
        return;
    }


  /*
   * Set LLC2 LinkStn role as primary  
   * Set LLC2 state to SETUP
   *
   * "llc->llcRole" and "llc->state" are used by cls_lak2_busy()
   *   to determine if Connect.Cnf should be sent once LAK connection is up
   *
   */
   llc->llcRole = LLC2_PRIMARY;
   llc->state = LLS_SETUP;


   /* 
    * Send LAK ConnectReq to remote peer
    *   force LAK to create pak for ConnectReq with "piggie-pak = NULL"
    */
    lackie->piggie_pak = NULL;
    level = raise_interrupt_level(NETS_DISABLE);
    lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_ext_conn);
    reset_interrupt_level(level);


   /*
    * Since local-ack session is now established,
    *   stop LLC1 timer (if it has been started)
    */
    if (llc->timer_flags & LLC1_TIMER_M)
      llc2_stoptimer(llc, LLC1_TIMER_M);

    return;

} /* End lakConnectReq() */


/*************************************************************************
 *                   CLSI Primitive: Connect.rsp
 *************************************************************************
 *
 * If the state is LLS_SETUP, then the initial busy state of the
 * lack session has already been cleared by cls_lak2_busy, and
 * a Connected.ind is sent here.
 *
 * If not, change the state to LLS_SETUP and cls_lak2_busy
 * will send a CONNECTED.Ind when the initial "busy"
 * state clears.
 *
 ************************************************************************
 */

static inline clsLlcMapFnT lakConnectRsp (llctype *llc, 
                                          ClsDlcParamBlockT *clsBlock)
{
  if (llc->state == LLS_SETUP) {
    llc->state = LLS_NORMAL;
    llc2ClsNotify(llc, CLS_CONNECTED_IND, CLS_OK);
  } else {
    llc->state = LLS_SETUP;
  }
  
  return;
  
} /* End lakConnectRsp() */


/*************************************************************************
 *                   CLSI Primitive: Signal_stn.Req
 *************************************************************************
 *
 * Send Quench off to RSRB/LAK remote peer
 *
 * DLC Message Generated
 * ---------------------
 * lak_wput(*cep, DLC_SIGNAL_STN_REQ, &param_block)
 *
 ************************************************************************
 */

static inline clsLlcMapFnT lakSignalStnReq (llctype *llc, 
                                            ClsDlcParamBlockT *clsBlock)
{

    lak_t *lackie;
    leveltype level;


    lackie = llc2_get_lackie(llc);
    if (!lackie) {
        if (lack_debug_err)
          buginf("\nLOCACK: lakSignalStnReq, null LAK ptr");
        return;
    }

    llc->state = LLS_NORMAL;

    level = raise_interrupt_level(NETS_DISABLE);
    lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_txqlow);
    reset_interrupt_level(level);

    return;

} /* End lakSignalStnReq() */


/*************************************************************************
 *                   CLSI Primitive: Data.req
 *************************************************************************
 *
 * Forward data from DLU to RSRB/LAK remote peer
 *
 * DLC Message Generated
 * ---------------------
 * lak_wput(*cep, DLC_DATA_REQ, &param_block)
 *
 ************************************************************************
 */

static inline clsLlcMapFnT lakDataReq (llctype *llc, 
                                       ClsDlcParamBlockT *clsBlock)
{

    uchar *saveSapStartPtr;
    boolean llc_encaps_rc; 


    paktype *pak = clsBlock->fPakPtr;

    /* Setup pak fields for llc_encapsulate() */
    /* -------------------------------------- */
    pak->flags |= PAK_LACK_OUT;
    pak->if_output = llc->idb;

    /* Setup pak fields to mimic llc2_sendframe() */
    /*   (may not be necessary)                   */
    /* ------------------------------------------ */
    pak->linktype = llc->linktype;
    pak->llc2_cb = llc;


    /* For IFRAMES, pak->datagramstart ptr set to frame data    */
    /*                                                          */
    /*   (1) Move back pak->datagramstart to include SSAP/DSAP  */
    /*                                                          */
    /*   (2) Increment pak->datagramsize to reflect             */
    /*         movement of pak->datagramstart                   */
    /*                                                          */
    /*   (3) Save SapStart address for calculation              */
    /*         of pak->llc2_sapoffset after LLC encap of pak    */
    /* -------------------------------------------------------- */
    pak->datagramstart -= 4; 
    pak->datagramsize += 4;
    saveSapStartPtr = pak->datagramstart;
    
    /* Add LLC header info (DA,SA,RIF) */
    /* ------------------------------- */
    llc_encaps_rc = llc_encapsulate(pak, LLC_IFRAME, pak->datagramsize, 
				    llc);
    if (!llc_encaps_rc) {
        datagram_done(pak);
        return;
    }

    /* Save pak->llc2_sapoffset   */
    /*   and set DSAP/SSAP in pak */
    /* -------------------------- */
    pak->llc2_sapoffset  = saveSapStartPtr - pak->datagramstart;
    *(saveSapStartPtr)   = llc2_get_rsap(llc);
    *(saveSapStartPtr+1) = llc2_get_lsap(llc);

    /* Setup pak fields to mimic llc2_sendframe() */
    /*   (may not be necessary)                   */
    /* ------------------------------------------ */
    pak->flags &= ~PAK_SRB_OUT;
    pak->flags |= PAK_SRB_EXPLORER;

    
    /* Ensure this pak is not charged to any interface */
    /* ----------------------------------------------- */
    clear_if_input(pak);

    /* Setup pak fields for sending across Vring */
    /* ----------------------------------------- */
    pak->if_input = llc->idb;
    pak->if_output = NULL;

    vrforward_pak(pak, TRUE, RSRB_OP_LLC2DATA, RSRB_RIFLF_LEAVEALONE);

} /* End lakDataReq() */


/*************************************************************************
 *                   CLSI Primitive: Flow.req
 *************************************************************************
 *
 * Send Quench on/off to RSRB/LAK remote peer
 *
 * DLC Message Generated
 * ---------------------
 * lak_wput(*cep, DLC_FLOW_REQ, &param_block)
 *
 ************************************************************************
 */

static inline clsLlcMapFnT lakFlowReq (llctype *llc, 
                                       ClsDlcParamBlockT *clsBlock)
{

    lak_t *lackie;
    int minor;
    leveltype level;


    lackie = llc2_get_lackie(llc);
    if (!lackie) {
        if (lack_debug_err)
          buginf("\nLOCACK: lakFlowReq, null LAK ptr");
        return;
    }

    if (clsBlock->fParameters.fResumeFlow) {
        llc->state = LLS_NORMAL;
        minor = l2mi_dlc_ind_txqlow;
    } else {
        llc->state = LLS_BUSY;
        minor = l2mi_dlc_ind_txqhigh;
    }

    level = raise_interrupt_level(NETS_DISABLE);
    lak2_fsm(lackie, l2ma_dlc, minor);
    reset_interrupt_level(level);

    return;

} /* End lakFlowReq() */


/*************************************************************************
 *                   CLSI Primitive: Disconnect.req
 *************************************************************************
 *
 * Send Disconnect request to RSRB/LAK remote peer
 *
 * DLC Message Generated
 * ---------------------
 * lak_wput(*cep, DLC_DISC_REQ, &param_block)
 *
 ************************************************************************
 */

static inline clsLlcMapFnT lakDisconnectReq (llctype *llc, 
                                             ClsDlcParamBlockT *clsBlock)
{
    lak_t *lackie = NULL;
    leveltype level;


    llc->state = LLS_ADM;
    llc2ClsNotify(llc, CLS_DISC_CNF, CLS_OK);

    lackie = llc2_get_lackie(llc);
    if (!lackie) {
        if (lack_debug_err)
          buginf("\nLOCACK: lakDisconnectReq, null LAK ptr");
        return;
    }

    level = raise_interrupt_level(NETS_DISABLE);
    lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_closed);
    reset_interrupt_level(level);

    return;

} /* End lakDisconnectReq() */


/*
 * lak_wput ()
 *
 *   Provides interface between CLS and RSRB/LACK
 *
 */
static void lak_wput (DlcPCepIdT pCepId, DlcOpcodeT opcode,
                ClsDlcParamBlockT *clsParamBlock)
{

    llctype *llc = (llctype *) pCepId;


    /* If no pCepId assigned...                */
    /*   process through CLS/LLC glue routines */
    if (llc == (llctype*) CLS_NO_CEP) {
        llc_wput (pCepId, opcode, clsParamBlock);
        return;
    }


    /* If CLS/RSRB local-ack is NOT enabled... */
    /*   process through CLS/LLC glue routines */
    /* --------------------------------------- */
    if (!llc->rsrb_lack) {
        llc_wput (pCepId, opcode, clsParamBlock);
        return;
    }


    /* If CLS/RSRB local-ack "is" enabled...        */
    /*   process through CLS/RSRB LAK glue routines */
    /* -------------------------------------------- */
    switch (opcode) {

    case DLC_CONNECT_REQ:
        lakConnectReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_CONNECT_RSP:
        lakConnectRsp((llctype *)pCepId, clsParamBlock); break;

    case DLC_SIGNAL_STN_REQ:
        lakSignalStnReq((llctype *)pCepId, clsParamBlock); break;
        break;

    case DLC_DATA_REQ:
        lakDataReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_FLOW_REQ:
        lakFlowReq((llctype *)pCepId, clsParamBlock); break;

    case DLC_DISC_REQ:
        lakDisconnectReq((llctype *)pCepId, clsParamBlock); break;

    default:
        llc_wput (pCepId, opcode, clsParamBlock); break;

    } /* End switch (opcode) */

} /* End lak_wput() */



/*************************************************************************/
/* Define local LAK routines to handle LAK-to-CLS events:                */
/*************************************************************************/


/*************************************************************************
 *                   LAK Event: RSRB/LAK session activation 
 *************************************************************************
 *
 * Send CONNECT.Ind to DLU
 *
 ************************************************************************
 */

static dlc_rtn_t cls_lak2_open (dlc_db_t *dlc_db, lak_t *lackie)
{

    paktype *pak;
    llctype *llc2;


    if (!lackie || !dlc_db) {
        if (lack_debug_err) 
  	  buginf("\nLOCACK: cls_lak2_open, null dlc_db or null lackie");
	return(DLC_RS_FAILED);
    }
    if (dlc_db->type != DLC_CLS) {
        if (lack_debug_err) 
	  buginf("\nLOCACK: cls_lak2_open, wrong dlc_db type");
	return(DLC_RS_FAILED);
    }	
    pak = lackie->piggie_pak;
    if (!pak) {
        if (lack_debug_err) 
	  buginf("\nLOCACK: cls_lak2_open, no piggie pak");
	return(DLC_RS_FAILED);
    }
    llc2 = lackie->llc;
    if (!llc2) {
        if (lack_debug_err) 
	  buginf("\nLOCACK: cls_lak2_open, null LLC");
	return(DLC_RS_FAILED);
    } 
        
    /* Since local-ack session is now established, */
    /*   stop LLC1 timer (if it has been started   */
    /* ------------------------------------------- */
    if (llc2->timer_flags & LLC1_TIMER_M)
      llc2_stoptimer(llc2, LLC1_TIMER_M);

    dlc_db->ptr.llc = llc2;

    lackie->llc2_open_pak = pak;
    lackie->llc2_open_ok = TRUE;

    llc2->llcRole = LLC2_SECONDARY; 
    llc2ClsNotify(dlc_db->ptr.llc, CLS_CONNECT_IND, CLS_OK);
    return(DLC_RS_OK);

} /* End cls_lak2_open() */


/*************************************************************************
 *                   LAK Event: RSRB/LAK session termination
 *************************************************************************
 *
 * Send DISCONNECT.Ind to DLU
 *
 ************************************************************************
 */

static dlc_rtn_t cls_lak2_close (dlc_db_t *dlc_db, lak_t *lackie)
{

    if (!dlc_db || !lackie) {
        if (lack_debug_err) 
	  buginf("\nLOCACK: cls_lak2_close, null dlc_db or null lackie");
	return(DLC_RS_FAILED);
    }
    if (dlc_db->type != DLC_CLS) {
        if (lack_debug_err) 
          buginf("\nLOCACK: cls_lak2_close, wrong dlc_db type");
	return(DLC_RS_FAILED);
    }
    if (!dlc_db->ptr.llc) {
        if (lack_debug_err) 
	  buginf("\nLOCACK: cls_lak2_close, null LLC ptr in dlc_db");
 	return(DLC_RS_FAILED);
    }
    if (!dlc_db->ptr.llc->l2_lakptr) {
        if (lack_debug_err) 
	  buginf("\nLOCACK: cls_lak2_close, null LAK ptr in LLC");
	return(DLC_RS_FAILED);
    }
    
    /* If valid UCepId exists,                                */
    /*   DLU has not yet been notified of session termination */
    /* ------------------------------------------------------ */
    if (dlc_db->ptr.llc->u_cep_id != (void*) CLS_NO_CEP)
      llc2ClsNotify(dlc_db->ptr.llc, CLS_DISC_IND, CLS_OK);
    return(DLC_RS_OK);

} /* End cls_lak2_close() */


/*************************************************************************
 *                   LAK Event: RSRB/LAK session quench (on/off)
 *************************************************************************
 *
 * Send FLOW.Ind to DLU
 *
 ************************************************************************
 */

static dlc_rtn_t cls_lak2_busy (dlc_db_t *dlc_db, boolean busyit)
{

    llctype *llc;


    if (!dlc_db) {
        if (lack_debug_err) 
	    buginf("\nLOCACK: cls_lak2_busy, null dlc_db");
	return(DLC_RS_FAILED);
    }

    if (dlc_db->type != DLC_CLS) {
        if (lack_debug_err) 
	    buginf("\nLOCACK: cls_lak2_busy, wrong dlc_db type");
	return(DLC_RS_FAILED);
    }

    if (!dlc_db->ptr.llc) {
        if (lack_debug_err) 
	    buginf("\nLOCACK: cls_lak2_busy, null LLC ptr in dlc_db");
	return(DLC_RS_FAILED);
    } else {
        llc = dlc_db->ptr.llc;
    } 


   /* 
    * When LAK connection is initially established, 
    *   LAK will call cls_lak2_busy() to clear "setup" state of connection 
    *
    * Use the "llc->llcRole" and "llc->state" values to determine
    *   if CLS needs to send Connect.Cnf or Connected.Ind to DLU
    *
    * NOTES: 
    *   If we are primary:
    *   (1) Flow.Ind is not necessary since llc2 is not active yet
    *   (2) llc->state becomes LLS_BUSY here... (will become LLS_NORMAL
    *       later when DLU issues SIGNAL_STN_REQ)
    *   (3) Connect.Cnf is sent to DLU
    *
    *   If we are secondary:
    *   (1) Flow.Ind is not necessary since llc2 is not active yet
    *   (2) If current state is LLS_SETUP, we have already received a
    *        Connect.rsp, so llc->state becomes LLS_NORMAL here
    *        and Connected.Ind is send to the DLU 
    *   (3) If current state is LLS_ADM, we haven't received a 
    *        Connect.rsp yet so llc->state becomes LLS_SETUP and 
    *        Connected.Ind will be send when Connect.rsp is received.
    */

    if (llc->state == LLS_SETUP) {
      if (llc->llcRole == LLC2_PRIMARY) {
	llc->state = LLS_BUSY;
	llc2ClsNotify(llc, CLS_CONNECT_CNF, CLS_OK);
	return(DLC_RS_OK);
      }
      
      if (llc->llcRole == LLC2_SECONDARY)  {
	llc->state = LLS_NORMAL;
	llc2ClsNotify(llc, CLS_CONNECTED_IND, CLS_OK);
	return(DLC_RS_OK);
      }
    }
	
    if ((llc->state == LLS_ADM) && (llc->llcRole == LLC2_SECONDARY)) {
      llc->state = LLS_SETUP;
      return(DLC_RS_OK);
    }

    /* 
     * LAK will call cls_lak2_busy() when QUENCH(on/off) is received
     *
     * If QUENCH(on) received from local-ack peer,                          
     *   Force DLU to stop sending data by indicating full LLC output queue 
     * Else if QUENCH(off) received,                                        
     *   reset curr_cong_level to "real" output queue level             
     */
    if (busyit) {
	CepSetCurrCongLevel(&llc->fCep, DLC_OQUEUE_FULL);
    } else {
	CepSetCurrCongLevel(&llc->fCep, llc_get_oqueue_status(llc)); 
	if (CepGetCurrCongLevel(&llc->fCep) == DLC_OQUEUE_EMPTY)
	    CepSetCurrCongLevel(&llc->fCep, DLC_OQUEUE_LOW);
    } 

    CepFlowNotify(llc->u_cep_id, &llc->fCep);
    CepSetPrevCongLevel(&llc->fCep, CepGetCurrCongLevel(&llc->fCep));

    return(DLC_RS_OK);

} /* End cls_lak2_busy() */


/*************************************************************************
 *                   LAK Event: RSRB/LAK session aborted
 *************************************************************************
 *
 * Send DISCONNECT.Ind to DLU
 *
 ************************************************************************
 */

static dlc_rtn_t cls_lak2_abort (dlc_db_t *dlc_db)
{

    if (!dlc_db) {
        if (lack_debug_err) 
	  buginf("\nLOCACK: cls_lak2_abort, null dlc_db");
	return(DLC_RS_OK);
    }
    if (dlc_db->type != DLC_CLS) {
        if (lack_debug_err) 
	  buginf("\nLOCACK: cls_lak2_abort, wrong dlc_db type");
	return(DLC_RS_FAILED);
    }
    if (!dlc_db->ptr.llc) {
        if (lack_debug_err) 
	  buginf("\nLOCACK: cls_lak2_abort, null LLC ptr in dlc_db");
	return(DLC_RS_FAILED);
    }

    /* If valid UCepId exists,                                */
    /*   DLU has not yet been notified of session termination */
    /* ------------------------------------------------------ */
    if (dlc_db->ptr.llc->u_cep_id != (void*) CLS_NO_CEP)
      llc2ClsNotify(dlc_db->ptr.llc, CLS_DISC_IND, CLS_OK);
    return(DLC_RS_OK);

} /* End cls_lak2_abort() */


/*************************************************************************
 *                   LAK Event: RSRB/LAK data received
 *************************************************************************
 *
 * Send DATA.Ind to DLU
 *
 ************************************************************************
 */

void cls_lak2_sendData (lak_t *lackie, paktype *pak)
{

    int info_size;
    int enctype;
    sap_hdr *llc1;


    clear_if_input(pak);

    llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + lackie->rif.len);
    token_get_enctype_infosize(llc1, &enctype, &info_size, NULL);
    token_set_data_pointers(pak, lackie->rif.len, info_size);
    lackie->llc->inbuf = pak;
    llc2ClsNotify(lackie->llc, CLS_DATA_IND, CLS_OK);

} /* End cls_lak2_sendData() */

/*
 * cls_CreateLak()
 *
 *   Called from SapSNAHandleSnaXid() to create CLS/RSRB LAK structure
 *     if local-ack has been enabled on the peer
 *   This is a registered routine.
 *
 */

void cls_CreateLak (llctype *llc, paktype *pak)
{
 
    tr_vpeertype  *vrp;
    tr_ventrytype *vre;
    trrif_t *trh;
    lak_t *lak_ptr;
    
    vrp = pak->peer_ptr;
    vre = vrfind_idb2entry(pak->if_input->hwptr, vrp->vrg);
    
    /* 
     * Reverse RIF so that lack2_build_reverse_entry() can find RIF okay 
     *   might be a problem in rif_match_offset() 
     */

    trh = (trrif_t *) pak->datagramstart;
    trh->rc_dlf ^= RC_D;
    lak_ptr = lack2_build_reverse_entry(pak, vre, 0, DLC_CLS);
    trh->rc_dlf ^= RC_D;
    
    if (lak_ptr) {
	llc->l2_lakptr = lak_ptr;
        llc->media_llc_cleanup = lack_cleanup_llc;
	lak_ptr->llc = lak_ptr->dlc_db.ptr.llc = llc;
        lak_ptr->llc2_open_ok = TRUE;
    }
    return;
    
} /* End cls_CreateLak() */



static void dlsw_rsrb_vidb_iqueue (
    hwidbtype *idb,
    paktype *pak)
{
    tring_hdr *trh;             /* pointer to TR MAC header */
    int riflen;                 /* length of RIF in TR header */
    sap_hdr *llc1;              /* pointer to llc-1 header */
    sap_entry_t         *sap_entry;
 
    GET_TIMESTAMP(idb->lastinput);      /* remember time of last input */
    pak->inputtime = idb->lastinput;    /* save it in packet too */
    idb->counters.rx_cumbytes += pak->datagramsize;
    idb->counters.inputs++;
 
    sap_entry = sap_get_bridging_entry(idb->firstsw);
    if (!sap_entry) {
        if (srb_debug || srbevent_debug)
            buginf("\ndlsw_rsrb_vidb_iqueue: DLSw sap entry is not valid");
        datagram_done(pak);
        return;
    }
    trh = (tring_hdr *) pak->datagramstart;
    if (trh->saddr[0] & TR_RII) {       /* have a rif get its length */
        riflen = trh->data[0] & RC_LTHMASK;
        llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);
    } else {
        riflen = 0;
        llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES);
    }
    if (tokenring_debug) {
        if (tr_print_packet(idb," in:", trh, (ulong *) llc1, 0, pak->datagramsize))
            buginf("\nVR%d:  in:      riflen %d, llc_offset %d",
                   idb->unit, riflen, (uchar *)llc1 - (uchar *)trh);
    }
 
    (*idb->parse_packet)(idb, pak, NULL);
    reg_invoke_dlsw_work(pak, idb, riflen);
}

