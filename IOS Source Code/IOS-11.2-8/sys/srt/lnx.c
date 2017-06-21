/* $Id: lnx.c,v 3.20.8.15 1996/09/15 02:55:54 rbatz Exp $
 * $Source: /release/112/cvs/Xsys/srt/lnx.c,v $
 *------------------------------------------------------------------
 * lnx.c -- Lan to X.25 Media Translation (LNX) Support
 *
 * January 2, 1993,      Percy P. Khabardar
 * September, 1993       Phil Morton
 * September, 1993       Ramin  Naderi
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lnx.c,v $
 * Revision 3.20.8.15  1996/09/15  02:55:54  rbatz
 * CSCdi60999:  QLLC: PS/2 using Link Station of Negotiable cannot connect.
 * Branch: California_branch
 *
 * Revision 3.20.8.14  1996/08/28  13:15:47  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.20.8.13  1996/08/26  15:12:17  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.20.8.12  1996/08/21  14:52:50  rbatz
 * CSCdi66340:  QLLC over PVCs displays infinite error messages.
 *              - Do not try to requue same qllc control block.
 *              - Do not respond to a Reset with a Reset
 *                because this can result in a protocol loop.
 * Branch: California_branch
 *
 * Revision 3.20.8.11  1996/08/21  13:15:07  rbatz
 * CSCdi62155:  LNX timers not working correctly. Result is TEST
 *              and NULL XID frames sent in the middle of
 *              XID(3) Negotiation.
 * Branch: California_branch
 *
 * Revision 3.20.8.10  1996/08/08  19:16:07  rbatz
 * CSCdi64913:  QLLC forwards XID(3) to RSRB with ABM bit set incorrectly.
 * Branch: California_branch
 *
 * Revision 3.20.8.9  1996/08/03  23:37:19  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.20.8.8  1996/07/23  13:29:07  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.20.8.7  1996/07/09  06:05:21  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.20.8.6  1996/05/23  06:42:16  pmorton
 * CSCdi58502:  Fix for CSCdi42369 broke FRAD image
 * Branch: California_branch
 * Use 'debug sdlc qllc' instead of 'debug qllc sdlc'
 *
 * Revision 3.20.8.5  1996/05/22  19:07:56  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * aleady -> already
 *
 * Revision 3.20.8.4  1996/05/22  05:00:27  pmorton
 * CSCdi42369:  sdlc-qllc conversion, no auto-reconnect
 * Branch: California_branch
 * 1. When qllc link state changes propagate to SDLC-QLLC conversion (SNX)
 * 2. Add 'debug qllc sdlc' to track this
 * 3. Correct SNX state names
 *
 * Revision 3.20.8.3  1996/05/18  00:31:29  pmorton
 * CSCdi56753:  DLSW over Qllc broken
 * Branch: California_branch
 * Use srb_get_or_create for the srb_sb.
 *
 * Revision 3.20.8.2  1996/05/17  12:12:57  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.18.2.8  1996/04/26  15:23:08  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.18.2.7  1996/04/26  07:58:13  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.18.2.6  1996/04/11  14:38:18  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.18.2.5  1996/04/06  01:05:56  pmorton
 * CSCdi45577:  QLLC w/PVCs regression with QLLC in CLS.
 * Build a map for managing PVCs, so that they can be claimed either by
 * CLS or by RSRB
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.18.2.4  1996/04/03  21:58:49  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.18.2.3  1996/03/28  08:20:59  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.18.2.2  1996/03/17  18:49:36  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.18.2.1  1996/03/01  16:31:28  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.20.8.1  1996/03/18  22:10:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.2.4  1996/03/16  07:37:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.10.2.3  1996/03/13  02:01:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.10.2.2  1996/03/07  10:50:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10.2.1  1996/02/20  18:47:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.20  1996/03/01  00:49:24  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up correctly
 * Restore CSCdi47275 w/fix to netbios_as.c
 *
 * Revision 3.19  1996/02/28  09:34:05  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.18  1996/02/21  02:43:43  hampton
 * Clean up platform specific SRB defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.17  1996/02/17  16:47:49  rbatz
 * CSCdi44435:  Outbound connection from FEP to QLLC device fails.
 *              Respond to XID Commands with XID Responses.
 *
 * Revision 3.16  1996/02/16  18:09:41  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Backout CSCdi47275 temporarily-- it breaks igs-c-l build
 *
 * Revision 3.15  1996/02/15  01:38:30  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Fix cleanup and change appl. cleanup from registry to vector
 *
 * Revision 3.14  1996/02/07  16:15:35  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.13  1996/02/01  06:10:12  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.12  1996/01/31  16:48:31  motto
 * Fixed a memory leak. Crash was caused by a pool_destroy bug
 * when buffers are leaked. Also reformatted the lnx_startup_seq
 * function.
 * CSCdi45231:  QLLC : Routers crash (Probably due to memory leak)
 *
 * Revision 3.11  1996/01/30  21:57:27  pmorton
 * CSCdi36695: QLLC disards XID packet (assuming that end station will
 * retransmit)
 * Hold the XID packet and send it to remote x.25 station after receiving
 * Call Accept
 *
 * Revision 3.10  1996/01/22  07:16:20  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.9  1996/01/16  21:53:37  pmorton
 * CSCdi45514: Router ignores null XID response - No XID T2 returned for
 * PU 2.0.
 * If the router is proxy for PU2.0, it sends XIDT2 whenever it receives
 * XID from the LAN.
 *
 * Revision 3.8  1996/01/05  10:25:05  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.7  1995/12/30  00:29:07  hou
 * CSCdi37413:  RTBR bridging fails for Ether->Token->Ether on vines and
 * ip.
 * - Set the default OUI for SNAP on token ring interface as 0x000000
 *   instead of 0x0000f8.
 * - Make low-end "ethernet-transit-oui" fully functional.
 *
 * Revision 3.6  1995/12/15  22:43:31  rbatz
 * CSCdi43378:  Explorers destined for SDLLC Virtual Ring forwarded to
 *              wrong ring.  Rework of vr_virtualp required.
 *
 * Revision 3.5  1995/12/05  14:40:32  motto
 * CSCdi43119:  Using QLLC/LLC2 conversion, memory leakage detected
 *
 * Revision 3.4  1995/11/21  00:04:13  rbatz
 * CSCdi40851:  SegV exception in qllc_x25_macaddr due to race
 *              condition and a NULL pointer.
 *
 * Revision 3.3  1995/11/17  18:54:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:41:57  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:31:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:30:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/19  08:10:29  bchan
 * CSCdi27730:  SNA Fragmentation break if no buffers available
 *
 * Revision 2.5  1995/08/07  05:43:45  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *              move dlsw_work to its own file, create libibm_util.a for
 *              utility routines used by dlsw and rsrb
 *
 * Revision 2.4  1995/07/19  21:06:27  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add services to reducce dependency on idb.
 * Install 'qllc dlsw' command.
 * Build QLLC Activate/Deactivate Ring CLSI message.
 * Consolidate qllc fields from swidb to a single data structure (subblock)
 *
 * Revision 2.3  1995/07/06  01:33:29  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.2  1995/06/28 09:31:21  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:00:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "media_registry.h"
#include "interface_generic.h"
#include "subsys.h"
#include "packet.h"
#include "ieee.h"
#include "../os/old_timer_callbacks.h"
#include "mgd_timers.h"
#include "logger.h"
#include "../srt/msg_qllc.c"		/* Not a typo, see logger.h */
#include "../h/config.h"
#include "parser.h"
#include "../x25/parser_defs_x25.h"
#include "../srt/parser_defs_qllc.h"
#include "../if/network.h"
#include "../llc2/llc2.h"
#include "../llc2/llc2_sb.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../if/rif.h"
#include "../if/rif_inline.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../ibm/sdlc.h"	/* ONLY TO LET lack_externs.h COMPILE */
#include "../ibm/sna_util.h"
#include "../srt/lack_externs.h"
#include "../cls/cls_private.h"
#include "../util/avl.h"
#include "../srt/qllc.h"
#include "../srt/qllc_private.h"
#include "../srt/lack2fsm.h"
#include "../srt/lack2_externs.h"
#include "../srt/lnx.h"
#include "../srt/srt_registry.h"
#include "../ibm/libibm_externs.h"
#include "../if/rif_util.h"
#include "srb_sb_private.h"

/*
 * Global Storage
 */
lnxtype		*lnx_hash_table[LNX_HASH_SIZE];

static watched_queue       *lnx_net_linkupQ;
static watched_queue       *lnx_dlc_linkupQ;
static watched_queue       *lnx_dataQ;

/*
 *  Forward declarations
 */

static	inline void lnx_to_lak_addr(lnxtype *lnx, lak_addr_t *lak_addr);

static void QllcSendThruQ (lnxtype* lnx, paktype *pak);
static boolean snx_add_lnxtype(uchar *vmac);
static boolean lnx_add_vring_interface 
    (hwidbtype *idb, ushort local_ring, uchar bridge, ushort target_ring);
static boolean lnx_remove_vring (hwidbtype *idb, int rsrb_et);
static void snx_start(void);
static	boolean lnx_active(hwidbtype *idb);
static	boolean lnx_vr_other_lnx(lnxtype *thislnx);
static 	void lnx_config_xid(parseinfo *csb, idbtype *idb);
static 	void lnx_config_partner(parseinfo *csb, idbtype *idb);
static 	void lnx_config_qllclf(parseinfo *csb, idbtype *idb);
static 	void lnx_config_sap(parseinfo *csb, idbtype *idb);
static 	void lnx_config_srb(parseinfo *csb, idbtype *idb);
static 	void lnx_config_poll(parseinfo *csb, idbtype *idb);
static  void sdlc_clean_snx (sdlc_data_block *sdb);
static	void show_lnxtype(lnxtype *lnx);
static	void lnx_contacted(lnxtype *lnx);
static  void lnx_net_ind_dm(lnxtype *lnx, paktype *pak);

static void lnx_unmap_qllc(uchar *vmac);
static lnxtype *lnxtype_init(uchar *vmac);
static process lnx_linkup_process(void);
static process lnx_data_process(void);

static dlc_rtn_t lnx_lak2_open  (dlc_db_t *dlc_cb, lak_t *lackie);
static dlc_rtn_t lnx_lak2_close (dlc_db_t *dlc_db, lak_t *lackie);
static dlc_rtn_t lnx_lak2_busy  (dlc_db_t *dlc_db, boolean busyit);
static dlc_rtn_t lnx_lak2_abort (dlc_db_t *dlc_db);
static dlc_handle_t *lnx_get_dlc_handle(void);

static boolean is_xid1(paktype *pak);
static paktype *lnx_get_xid1_pak(lnxtype *lnx);
static paktype *lnx_get_startup_pak(lnxtype *lnx);

static dlc_handle_t lnx_dlc_handle = {
    DLC_QLLC,    
    lnx_lak2_open,    lnx_lak2_close,    lnx_lak2_busy,	    lnx_lak2_abort
} ;



/*
 * Externs
 */
extern void tr_setencaps(parseinfo *);
extern char trprint_rifstr[];
extern int lack_passthru_rings[];


/* lnx_init()
 *
 * subsystem initialization
 */

#define QLLC_MAJVERSION 1
#define QLLC_MINVERSION 0
#define QLLC_EDITVERSION  1

SUBSYS_HEADER(qllc, QLLC_MAJVERSION, QLLC_MINVERSION, QLLC_EDITVERSION,
	      lnx_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: srb, x25", NULL);

void lnx_init (subsystype *subsys)
{
    register int i;

    for (i = 0; i < LNX_HASH_SIZE; i++) {
	lnx_hash_table[i] = NULL;
    }
    lnx_running = FALSE;
    qllc_init();
    qllc_ui_init();

    reg_add_lack_get_dlc_handle(DLC_QLLC, lnx_get_dlc_handle, 
				"lnx_get_dlc_handle");
    reg_add_snx_start(snx_start, "snx_start");
    reg_add_snx_output(snx_output, "snx_output");
    reg_add_sdlc_clean_snx(sdlc_clean_snx, "sdlc_clean_snx");
    reg_add_snx_add_lnxtype(snx_add_lnxtype, "snx_add_lnx_type");
    reg_add_rsrb_add_vring_interface(lnx_add_vring_interface,
				     "lnx_add_vring_interface");
    reg_add_rsrb_remove_vring(lnx_remove_vring, "lnx_remove_vring");
    reg_add_srt_other_media(lnx_active, "lnx_active");
}

static dlc_handle_t *lnx_get_dlc_handle(void)
{
    return(&lnx_dlc_handle);
}

/* lnx_start ()
 *
 * Get lnx processes running.
 * lnx_linkup_process - This process is used to open up qllc and llc2 sessions.
 * lnx_data_process - This process is used when output packet fragmentation 
 * fails, and the packet is bumped up to process level.
 * Seperate processs are reqd. because linkup is not a high priority 
 * event, but moving data to the output is.
 */
void lnx_start (void)
{
    static pid_t lnx_pid = NO_PROCESS;
    static pid_t lnx_data_pid = NO_PROCESS;
    
    if (lnx_running) {
        return;
    }

    /*
     * Create the watched queues and the process.
     */
    if (!lnx_net_linkupQ) {
        lnx_net_linkupQ = create_watched_queue("LNX/QLLC NET LinkupQ", 0,
                                                LNX_NET_LINKUP_EVENT);
        if (!lnx_net_linkupQ) {
            return;
        }
    }

    if (!lnx_dlc_linkupQ) {
        lnx_dlc_linkupQ = create_watched_queue("LNX/QLLC DLC LinkupQ", 0,
                                                LNX_DLC_LINKUP_EVENT);
        if (!lnx_dlc_linkupQ) {
            delete_watched_queue(&lnx_net_linkupQ);
            return;
        }
    }

    if (lnx_pid == NO_PROCESS) {
        lnx_pid = process_create(lnx_linkup_process, "LNX/QLLC Linkup",
                                NORMAL_STACK, PRIO_NORMAL);
        if (lnx_pid == NO_PROCESS) {
            delete_watched_queue(&lnx_net_linkupQ);
            delete_watched_queue(&lnx_dlc_linkupQ);
            return;
        }
    }

    if (!lnx_dataQ) {
	lnx_dataQ = create_watched_queue("QLLC DataQueue", 0, 0);
    }
    if (!lnx_data_pid) {
	lnx_data_pid = process_create(lnx_data_process, "LNX Data",
				      NORMAL_STACK, PRIO_HIGH);
        if (lnx_data_pid == NO_PROCESS) {
	    printf("\n%%Can't create LNX data proc ");
            delete_watched_queue(&lnx_dataQ);
            return;
        }
    }

   lnx_running = TRUE;
   reg_add_media_llc_create(lnx_fix_llc, "lnx_fix_llc");

   qllc_start();
}




/*
 * lnx_findfree
 *
 *
 * Allocates a new lnxtype struct.
 * Returns NULL if no memory.
 */
lnxtype *lnx_findfree (void)
{

    lnxtype *lnx;


    if ( (lnx = malloc(sizeof(lnxtype) ) ) == NULL)
       return ( NULL );
      
    return (lnx);

}



/*
 * lnx_freeup
 */
void lnx_freeup (lnxtype *lnx)
{

   if (lnx) {
       lnx_unmap_qllc(lnx->vmac_addr);
       lnx_remove_macaddr_hash(lnx);
       datagram_done(lnx_get_startup_pak(lnx));
       datagram_done(lnx_get_xid1_pak(lnx));
       if (lnx->lnx_xid1_pak) {
 	   datagram_done(lnx->lnx_xid1_pak);
       }
       free(lnx);
   }
   else if (lnx_error_debug) {
       buginf("\nQLLC: lnx_freeup - NULL lnx pointer!!");
   }
}


/* lnx_x25_map_add
 * 
 * put an entry in the qllc x25 map,
 * if the map already exists and has an lnxtype there,
 * then update the idb field in the lnx
 */
boolean lnx_x25_map_add(uchar *vmac, x25map *x25map)
{
    qllcmap_t	*qllcmap;
    lnxtype	*lnx;

    qllcmap = qllcmap_add(vmac, x25map);
    if (qllcmap == NULL) {
	printf("QLLC: couldn't add map entry for %e", vmac);
	return(FALSE);
    }
    
    lnx = qllcmap->lnx;

    if (lnx != NULL) {
	lnx->idb = x25map->map_idb;
	qllc_idb_set_lnx_active(lnx->idb, TRUE);
    }
    return(TRUE);
}

static boolean snx_add_lnxtype(uchar *vmac)
{
    qllcmap_add(vmac, NULL);	/* add the map entry, for a 
					 * placeholder
					 */
    lnxtype_init(vmac);
    return(TRUE);	
}

/*
 * void lnx_unmap_qllc
 *
 * Remove lnx from a qllc map entry
 */
static void lnx_unmap_qllc(uchar *vmac)
{
    qllcmap_t	*qllcmap;

    qllcmap = qllcmap_lookup(vmac);
    if (qllcmap != NULL) {
	qllcmap->lnx = NULL;
    }
}

/* lnx_x25_map_delete
 *
 * execute when no x25 map/no x25 pvc command is issued
 *
 * Prepares the lnx for disconnecting, but doesn't free the lnxtype
 * This happens when the disconnect is completed.
 */

void lnx_x25_map_delete(lnxtype *lnx)
{
    if (lnx == NULL) {
	return;
    }
    qllc_detach(lnx->qllc);
    lnx_disconnect_net(lnx);
    lnx_remove_macaddr_hash(lnx);
    lnx_freeup(lnx);
    return;
}

/*
 * initialize variables used by lnx - this function is called when
 * qllc source route bridging is configured with the 
 *	qllc srb command.
 * It is also called when sdlc-qllc conversion is enabled through
 *
 * A lnxtype control block is malloc'd and entered in the lnx hash
 * table.  Also all lnxtype entries are set to default values.
 */  
static lnxtype *lnxtype_init (uchar *vmac)
{
    lnxtype	*lnx;
    qllcmap_t	*qllcmap;
    x25map	*x25map;

    qllcmap = qllcmap_lookup(vmac);

    if (qllcmap == NULL) {
	printf("%%QLLC: no virtual mac address for %e. Ignored", vmac);
	return(NULL);
    }
    if (lnx_by_macaddr(vmac) != NULL) {
	printf("\n%%Duplicate virtual MAC addr %e.  Ignored", vmac);
	return (NULL);
    }

    if ( !(lnx   = lnx_findfree()) ) {
	return(NULL);
    }
 
    /* Initialize lnxtype data structures */
    memset(lnx, 0, sizeof(lnxtype) );

    lnx->lnx_state         = LNX_DISCONNECT;
    lnx->configured_prtnr    = FALSE;
    lnx->dsap          = SAP_IBM_SNA;
    lnx->ssap          = SAP_IBM_SNA;
    lnx->llc           = NULL;
    lnx->qllc          = NULL;
    lnx->lnx_lak_enable    = FALSE;
    lnx->xid_valid     = FALSE;
    lnx->qllc_set_largest = FALSE;
    lnx->lnx_dlc_maxbtu  = 0;
    lnx->lnx_xid_pak = NULL;
    lnx->lnx_xid_rsp_pending  = FALSE;
    lnx->lnx_test_rsp_pending = FALSE;

    lnx->lnx_llc_abm = XID3_ABM;    /* Default to ON, until we learn it */

    /* fill in the virtual MAC address, from the x25 map command */
    ieee_copy(vmac, lnx->vmac_addr);
    lnx_enter_macaddr_hash(lnx);

    qllcmap->lnx = lnx;
    x25map = qllcmap->map;
    if (x25map != NULL) {
	lnx->idb = x25map->map_idb;
	qllc_idb_set_lnx_active(lnx->idb, TRUE);
    }
    return (lnx);
}

/* tests whether an LNX is being used before the x.25 configurator
 * attaches or removes a PVC from the virtual MAC address in the
 * X.25 map
 */

boolean lnx_busy(addrtype *addr)
{
    lnxtype	*lnx;
    uchar	*macaddr;

    macaddr = &addr->qllc_addr[0];
   
    if ( !(lnx = lnx_by_macaddr(macaddr)) )
	return (FALSE);
	
    if (lnx->lnx_state != LNX_DISCONNECT)
    	return(TRUE);
    return(FALSE);
}


static void lnx_pvc_setup (lnxtype *lnx, boolean sense)
{
    uchar	*vmac;
    qllctype	*qllc;
    qllcmap_t	*map;

    vmac = lnx->vmac_addr;
    map = qllcmap_lookup(vmac);
    if (map == NULL) {
	if (lnx_error_debug) {
	    buginf("\nNo qllc map entry for %e", vmac);
	}
	return;
    }
    if (map->qpsap) {			/* claimed by CLS */
	if (lnx_error_debug) {
	    buginf("\nVirtual Mac address already claimed by CLS");
	}
	return;
    }
    qllc = map->qllc;
    if (qllc == NULL) {
	/*
	 * Wasn't a PVC after all
	 */
	return;
    }
    qllc_detach (lnx->qllc);	/* remove what's there */
    lnx_disconnect_net(lnx);	/* just to be sure that we're closed down */
	
    if (sense) {			/* adding or changing */
	QPvcRemoveUnclaimed(qllc);	/* Remove from the orphanage */
	qllc->lnx = lnx;		/* binds lnx to qllc */
	lnx_fix_qllc(qllc, vmac);	/* binds qllc to lnx - and more  */
    } else {
	QPvcAddUnclaimed(qllc);
	lnx_unmap_qllc(lnx->vmac_addr);
	lnx_remove_macaddr_hash(lnx);
    }
}

/*
 * lnx_get_startup_pak
 * lnx_get_xid1_pak
 *
 * pull the packet from the lnxtype, clear the lnxtype,
 * and return the packet
 *
 * Since these routines can be called at process and interrupt level
 * it opens a window for race conditions.
 * Use raise_interrupt_level to shut this window tight
 */

static paktype *
lnx_get_startup_pak (lnxtype *lnx)
{
    leveltype level;
    paktype *pak;
    
    level = raise_interrupt_level(NETS_DISABLE);
    pak = lnx->lnx_startup_pak;
    lnx->lnx_startup_pak = NULL;
    reset_interrupt_level(level);
    return(pak);
}

static paktype *
lnx_get_xid1_pak (lnxtype *lnx)
{
    leveltype level;
    paktype *pak;
    
    level = raise_interrupt_level(NETS_DISABLE);
    pak = lnx->lnx_xid1_pak;
    lnx->lnx_xid1_pak = NULL;
    reset_interrupt_level(level);
    return(pak);
}

 
/*
 * lnx_net_linkup
 * This routine is useful when qllc is the secondary side of an qllc session.
 * It is used to establish an llc2 session with the Token Ring
 *station.
 *
 * Come here, when we received a QSM
 */
 
static void lnx_net_linkup (void)
{
    paktype	*pak;
    leveltype	level;
    llctype	*llc2 = NULL;
    lnxtype	*lnx;
    qllctype	*qllc;
 
 
    while ((pak = process_dequeue(lnx_net_linkupQ )) != NULL ) {
	lnx = pak->lnx_cb;
	qllc = lnx->qllc;
	datagram_done(pak);
	
	if (!qllc) {
	    if (lnx_error_debug){
		buginf("\nQLLC lnx_net_linkup_proc: no qllc");
	    }
	    continue;
	}
	if (lnx->lnx_state == LNX_NET_CONTACT_PENDING) {
	    llc2 = llc2_open2(lnx->llc, TRUE);
	    level = raise_interrupt_level(NETS_DISABLE);
	    if (llc2 && lnx->lnx_state == LNX_NET_CONTACT_PENDING) {
		qllc_contact_accept(qllc);
		qllc_quench(qllc, FALSE); /* unthrottle X.25 input */
		llc2_stateswitch(lnx->llc, CLEAR_LOCAL_BUSY);
		lnx_setstate(lnx, LNX_CONNECT);
		lnx->llc->lnx_active =TRUE;
                lnx->llc->media_llc_cleanup = lnx_cleanup_llc; 
	    } else {
		if (llc2)
		    llc2_close(llc2);
		qllc_contact_reject (qllc);
		lnx_setstate(lnx, LNX_DISCONNECT);
	    }
	    reset_interrupt_level(level);
	    continue;
	}
	
      if(lnx->lnx_state == LNX_AW_NET && qllc_idb_test_lnx_active(lnx->idb))
       {
        llc2 = llc2_open2(lnx->llc, TRUE);
        if(llc2)
         {
          lnx_net_ind_connect(lnx);
          lnx_setstate(lnx, LNX_CONNECT);
          /*lnx->llc = llc2; */
          lnx->llc->lnx_active = TRUE;
          lnx->llc->media_llc_cleanup = lnx_cleanup_llc;

          qllc_sendcontrol(qllc, QUA, FALSE);
          qllc_setstate (qllc,  QST_NORMAL);
	  qllc_quench(qllc, FALSE);

          /*llc2_stateswitch(lnx->llc, SET_LOCAL_BUSY); */

         }
        else
         {
          /* ???clean up the QLLC side */
          lnx_net_ind_disconnect(lnx); 
          lnx_setstate(lnx, LNX_DISCONNECT);
         }

       }
      else
       {
        if(lnx_error_debug){
         buginf("\nQLLC: unable to open llc2 session.\n");
        }
        continue;
       }

     }
  }


/*
 * lnx_dlc_linkup
 * This routine is useful when qllc is the primary side of an qllc session.
 * It is used to establish an qllc session with the X.25 station.
 */
static void lnx_dlc_linkup (void)
{

    paktype  *pak;
    qllctype *qllc;
    trrif_t  *trh;
    lnxtype  *lnx;
    leveltype level;


	while ( (pak = process_dequeue(lnx_dlc_linkupQ)) != NULL ) {

	    trh = (trrif_t *)pak->datagramstart;
	    lnx = lnx_by_macaddr(trh->daddr);
		
	    if (lnx == NULL) {
		if (lnx_error_debug) {
		    buginf("\nQLLC: no addrblock, pak dropped, da %e, sa %e, rif %s",
			    trh->daddr, trh->saddr,
			    get_rif_str( (uchar *)&(trh->rc_blth), trprint_rifstr) );
		}
		datagram_done(pak);
		continue;
	    }    
            	    
	    /* remove the rif pak from the lnx structure */  
	    datagram_done(lnx_get_startup_pak(lnx));
	    
	    level = raise_interrupt_level(NETS_DISABLE);
	    /* 
	     * always run at this level to avoid race conditions
	     */

            if (lnx->lnx_state == LNX_AW_QLLC_SEC) {
		reset_interrupt_level(level);
	    	lnx_upstream(lnx);
		datagram_done(pak);
		continue;
	    }

	    if (lnx->lnx_state != LNX_AW_QLLC_PRI) {
		reset_interrupt_level(level);
		datagram_done(pak);
                continue;               /* state changed, do not pass go  */
	    }

	    reset_interrupt_level(level);

            /* non-blocking call to qllc_open */
            qllc = qllc_open(trh->daddr, lnx->idb, FALSE);

            /* 
 	     * We already have the sabme pak to send to the llc2 state
 	     * machine once we receive the QUA. Feeding the pak for
 	     * the sabme to the llc2 state machine will send a ua to
 	     * the lan partner and set the llc2 connection to the
 	     * proper state
 	     */
  
	    datagram_done(lnx_get_xid1_pak(lnx));
  
	    level = raise_interrupt_level(NETS_DISABLE);
	    /* 
	     * always run at this level to
	     * avoid race conditions
            */

            if (!qllc) {
		datagram_done(pak);	/* clean up after ourselves */

                /* what's this? we  got a message that the virtual  circuit
                 * is up, intiated QSM/QUA, but it never completed
                 *
                 * Ah well, that could happen (and not even because of an
                 * inconsistency in the router)
                 */
                if (lnx_error_debug){
                    buginf("\nQLLC: unable to open qllc session, state %s",
                        lnx_prstate(lnx->lnx_state) );
                } 
                if (lnx->lnx_lak_enable) {    /* local_ack - RSRB connection */
                    lnx_lak2_dlc_event(lnx, l2mi_dlc_ind_closed);
                } else {
                    /* shouldn't be an llc2 connection, but just in case... */
                    llc2_close(lnx->llc);
                }
                lnx_setstate(lnx, LNX_DISCONNECT);
 
            } else {    /* real qllc - open succeeded */
		/* can set lnx_startup_pak here, since
		 * a) we cleared it out with lnx_get_startup_pak 
		 *    at the top of the while loop
		 * b) we're running with NETS_DISABLE
		 */
                lnx->lnx_startup_pak = pak;
		lnx->lnx_net_data_seen = FALSE;
                if (lnx->lnx_state == LNX_AW_QLLC_PRI) {
                    lnx->qllc            = qllc;
                }
            }
	    reset_interrupt_level(level);
        }
    }
 

/*
 * lnx_dlc_ind_connect
 *
 * upcall from qllc_ind_connect
 *
 * Useful if we're waiting for a network connection, and aren't running
 * in a blocking mode
 *
 * For now, do nothing
 */
 
void lnx_dlc_ind_connect(lnxtype *lnx)
{
    return;
}

 
 
/* inline functions for quick clean access to XID
 * - too trivial to be real
*/
 
/*
 * i s _ x i d 3
 *
 * quick test to make sure that we're interpreting a packet correctly
 * before reading it
 */
 
static inline boolean lnx_is_xid3(paktype *pak)
{
    xid3type *xid;
 
    if (pak->datagramsize < sizeof(xid3type))
        return(FALSE);
 
    xid = (xid3type *)pak->datagramstart;
 
    if ( (xid->xid_fmt & XID_FMT_MASK) != XID_XID3)
        return(FALSE);
 
    return (TRUE);
}
 
/*
 * i s _ x i d 1
 *
 * quick test to make sure that we're interpreting a packet correctly
 * before reading it
 */
 
static boolean is_xid1 (paktype *pak)
{ 
    if (pak->datagramsize < XID1_MIN_SIZE +1 )	/* allow for 0xBF */
        return(FALSE);
 
    if ( *(pak->datagramstart+1) != XID_XID1_PU2)
        return(FALSE);
 
    return (TRUE);
}

/* xid1_flip_role
 *
 * primary received, transmit secondary,  OR vice versa.
 */
static
inline void xid1_flip_role (paktype *pak)
{
	*(pak->datagramstart+1+XID1_ROLE_TWS_OFF) ^= XID1_PRIMARY_MASK;
}

/* is_xidnull
 *
 * test for NULL xid
 */
 
static inline boolean is_xidnull(paktype *pak, int xidoffset)
{
     if (pak->datagramsize == (xidoffset + sizeof(xidnulltype)) )
        return(TRUE);
 
    return (FALSE);
}
 
/*
 * x i d _ s p o o f _ a b m
 *
 * note the abm bit we received, and drop in the ABM bit the partner
 * wants
 */
static inline void lnx_xid_spoof_abm(paktype *pak, uchar *from_abm, uchar *to_abm)
{
    xid3type *xid = (xid3type *)pak->datagramstart;
 
    /* pick up the input value */
 
    *from_abm = xid->xid_lsflags & XID3_ABM;
 
    /* plop in the new one */
    if (*to_abm)
        xid->xid_lsflags |= XID3_ABM;
    else
        xid->xid_lsflags &= ~XID3_ABM;
}
 
/*
 * x i d _ g l e a n _ m a x b t u
 *
 * extract the max btu from the XID-3, without having to worry about
* odd/even byte boundaries
*/
 
static inline void lnx_xid_glean_maxbtu( xid3type *xid, uchar *stash)
{
    PUTSHORT (stash, GETSHORT(&xid->xid_maxbtu[0]));
}
 
/*
 * l n x _ s t a r t u p _ s e q
 *
 * Before LNX opens up llc2 and qllc sessions on either end there are a few
 * things which LNX needs to do before sessions can open up.  Among these
 * are sending out explorers, exchanging XID's etc.
 */
void lnx_startup_seq (paktype *pak, lnxtype *lnx, 
                      hwidbtype *inidb, tr_ventrytype *vre)
{
    uchar       conch;
    boolean     is_response;
    int         xidoffset;
    trrif_t     *trh;
    int         riflen;
    uchar       *frameptr;
    paktype     *xid_pak;
    sap_hdr  	*llc1;
 
    /* hold a packet so that we can build a lak2 entry, complete with
     * piggy_pak
     */

/* FIX THIS TO USE THE PACKET PARSING ROUTINES - PHIL */

    trh         = (trrif_t *)pak->datagramstart;
 
    riflen      = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;
 
    pak->llc2_sapoffset = TRING_ENCAPBYTES + riflen;
    frameptr            = pak->datagramstart + pak->llc2_sapoffset;
 
    llc1                = (sap_hdr *)frameptr;
    xidoffset           = (frameptr +2) - pak->datagramstart;
    conch               =  *(frameptr + 2) & ~P_BIT;    /* snuff out p/f bit */
    is_response         = (llc1->ssap & SAP_RESPONSE);
 
    /* stash the pak type so that we can create a lackie with the suitable
     * info later on.
     * Make sure this is not for NULL SAPs
     *
     * We can just scribble on the lnxtype since we're already in
     * interrupt context.
     */
    if (!lnx->lnx_startup_pak && 
	(llc1->dsap != 0 || (llc1->ssap & ~SAP_RESPONSE) != 0) ) {
	llc1->ssap &= ~SAP_RESPONSE;	/* don't need this any more */

	lnx->lnx_startup_pak = pak_duplicate(pak);
    }
    lnx->lnx_vre = vre;

    if (qllc_event_debug || qllc_test_debug) {
        buginf("\nQLLC: rx %s %s, da %e, sa %e, pak 0x%x rif %s ",
               conch == LLC_TEST ? "explorer" : "XID" ,
               is_response ? "response" : "command",
               trh->daddr, trh->saddr, pak,
               get_rif_str(&trh->rc_blth, trprint_rifstr));
        if (conch == XID)
	 pr_xid ( pak, xidoffset+1 );	/* Skip 0xBF */
    }
 
    /* make sure that the partner agrees with the configuration */
    if (!lnx->actual_prtnr) {
        lnx->actual_prtnr = TRUE;
        ieee_copy_sans_rii(trh->saddr, lnx->prtnr);
    } else {
        if (!ieee_equal_sans_rii(trh->saddr, lnx->prtnr)) {
            if (lnx_error_debug) {
                buginf("\nQLLC %e: different partner Originally %e, now %e shows up",
                       lnx->vmac_addr, lnx->prtnr, trh->saddr);
            }
            vr_nukepak(pak);
            return;
        }
    }
 
    if (conch == LLC_TEST) {
        if (is_response) {      /* send a NULL XID */
 
	    if (!lnx->lnx_test_rsp_pending) {
		/* repeated TEST response - ignore it */
		vr_nukepak(pak);
		return;
	    }
	    lnx->lnx_test_rsp_pending = FALSE;

	    rif_update(NULL, 0, trh->saddr, trh->daddr,
		       (srbroute_t *)(riflen ? &trh->rc_blth : NULL),
		       RIF_AGED);

	    trh->rc_dlf ^= RC_D;  /* toggle direction bit */
            xid_pak = tr_setup_lnx_xid(trh->saddr, trh->daddr,
				       lnx, FALSE, /* Command */
				       inidb, &trh->rc_blth, FALSE);
 
            if (xid_pak) {
                if (qllc_event_debug) {
                    buginf("\nQLLC: gen NULL XID, da %e, sa %e, rif %s, dsap %x, ssap %x ",
                           trh->saddr, trh->daddr,
                           get_rif_str((uchar *)&trh->rc_blth, trprint_rifstr),
                           lnx->dsap, lnx->ssap);
		}
                datagram_out(xid_pak);
            } else {
                if (lnx_error_debug) {
                    buginf("\nQLLC: couldn't make NULL XID. %e(%02x)->%e(%02x)",
                            lnx->vmac_addr, lnx->prtnr, lnx->ssap, lnx->dsap);
	        }
	    }
            vr_nukepak(pak);   /* free TEST RSP PAK */
        } else {           /* down-stream LLC2 indication */
	    if (qllc_hwidb_test_lnx_active(inidb)) {
		if (lnx->lnx_state == LNX_DISCONNECT) {
                    /* We have the LNX structure at this point.
                     * Store the pak in the linkupQ for calling out.
                     */
                    if (process_enqueue(lnx_dlc_linkupQ, pak)) {
			pak_lock(pak);	    /* Since we need to nuke it below */
			lnx_setstate(lnx, LNX_AW_QLLC_SEC);
		    }
		}
	    }
	    rif_update(NULL, 0, trh->saddr, trh->daddr,
			(srbroute_t *)(riflen ? &trh->rc_blth : NULL),
                        RIF_AGED);
	    trh->rc_dlf ^= RC_D;       

	    tr_process_test(inidb, pak, TRUE);
	    vr_nukepak(pak);
	    return;
        }
    } else if (conch == XID) {

	lnx->lnx_xid_seen = TRUE;		/* avoid more polling XIDs */

        if (lnx->xid_valid) {       /* yup, we're proxying */
	    trh->rc_dlf ^= RC_D;  /* toggle direction bit */
 
	    /* build XID 0T2 */
 
	    xid_pak = tr_setup_lnx_xid(trh->saddr, trh->daddr,
			  	       lnx, !is_response,
				       inidb, &trh->rc_blth, TRUE);
 
	    if (xid_pak) {
	        if (qllc_event_debug) {
		    buginf("\nQLLC: gen XID 0T2, da %e, sa %e, rif %s, "
                           "dsap %x, ssap %x ",
			   trh->saddr, trh->daddr,
			   get_rif_str((uchar *)&trh->rc_blth, trprint_rifstr),
			   lnx->dsap, lnx->ssap);
	        }
	        datagram_out(xid_pak);
 
	    } else {                     /* no pak error message */
	        if (lnx_error_debug) {
		    buginf("\nQLLC: couldn't make XID packet. %e(%02x)->%e(%02x)",
			   lnx->vmac_addr, lnx->prtnr, lnx->ssap, lnx->dsap);
	        }
	    }
	    vr_nukepak(pak);
        } else {    /* not proxying ; pass on to QLLC */
	    /*
	     *	If QLLC does not exist, we either
	     *  store the XID, and call out, or we
	     *	may drop it on the ground.
	     */
	    if (lnx->qllc == NULL) {
		datagram_done(lnx->lnx_xid_pak);
		lnx->lnx_xid_pak = NULL;
		lnx->lnx_xid_rsp_pending = FALSE;
		if (is_xidnull(pak, xidoffset)) {
		    if (lnx->xid_poll == FALSE) {
			lnx->lnx_xid_pak = pak_duplicate(pak);
			lnx->lnx_xid_rsp_pending = !is_response;
		    }
		    if ((lnx->lnx_state == LNX_DISCONNECT) &&
			(process_enqueue(lnx_dlc_linkupQ, pak))) {
			pak_lock(pak);
			lnx_setstate(lnx, LNX_AW_QLLC_SEC);
			if (qllc_event_debug) {
			    buginf("\nQLLC %e: NO X.25 connection - calling out.",
				    lnx->vmac_addr);
			}
		    } 
		} 
		vr_nukepak(pak);	
		return;
	    }

            if (!is_response) {
               /* 
                * It's an XID CMD.
                */
                lnx->lnx_xid_rsp_pending = TRUE;
            }

 	    /* 
	     * If xid-poll is set, and it's a NULL XID command,
 	     * drop the NULL XID on the ground; NPSI will send
	     * the NULL QXID to solicit an XID(0).
 	     */	   
 	    if (lnx->xid_poll && is_xidnull(pak, xidoffset)) {
		lnx->lnx_xid_rsp_pending = FALSE;
 	        vr_nukepak(pak);
 	        return;
 	    }
  
	    pak->datagramstart += xidoffset;
	    pak->datagramsize  -= xidoffset;
 
	    if (lnx->lnx_xid1) {
		if (lnx_is_xid3(pak)) {
		    errmsg(&msgsym(BAD_XID, QLLC), lnx->vmac_addr);
		    qllc_close(lnx->qllc);
		    return;
		} else {
		    /* 
		     * ditch the packet; use the saved XID FMt 1 packet we
		     * we saved on the way in
		     */
		    datagram_done(pak);
		    pak = lnx->lnx_xid1_pak;
		    lnx->lnx_xid1_pak = NULL;
		    if (pak == NULL) {
			return;
		    }
		    xid1_flip_role(pak); /* and out it goes */
			
		    if (qllc_state_debug) {
			int i;
			    
			buginf("\nXID1 %d bytes from LAN to X.25 ", 
				pak->datagramsize);
			for (i=0; i < pak->datagramsize; i++) {
			    buginf("%02x ", pak->datagramstart[i]);
			}
		    }	
		}
	    } else if (lnx_is_xid3(pak)) {
		/* save & spoof the ABM bit */
	        lnx_xid_spoof_abm(pak, &lnx->lnx_llc_abm, &lnx->lnx_qllc_abm);
	        lnx_xid_glean_maxbtu((xid3type *)pak->datagramstart,
		                     (uchar *)&lnx->lnx_net_maxbtu);
                /*
                 * Need to look like NPSI and retransmit an XID when
                 * dealing with Comms Manager Negotiable.
                 */
                if (lnx->qllc->xid32_from_remote) {
                    datagram_done(lnx->qllc->saved_xid3pak);
                    lnx->qllc->saved_xid3pak = pak_duplicate(pak);
                    mgd_timer_start(&lnx->qllc->fTimer, QT1_DEFAULT);
                }
	    }
	    qllc_fwd_xid(lnx->qllc, pak);
	}
    } /* XID */
    return;
}
   
void lnx_forward_held_xid (lnxtype *lnx, paktype *pak)
{
    /* Finally, forward whatever the packet is out the X.25 interface */
 
    /* Adjust the packet to start of the XID field */
    pak->datagramstart += pak->llc2_sapoffset + 2;
    pak->datagramsize  -= pak->llc2_sapoffset + 2;
     
    if (qllc_event_debug) {
 	buginf("\nXID %d bytes from LAN to X.25 ", 
 	       pak->datagramsize);
    }
  
    qllc_fwd_xid(lnx->qllc, pak);
    return;
}
   
/* l n x _ q l l c _ n o t i f y
 *
 * indication from the x.25 side that something's going on.
 * This is currently very like sdlc_ind_disconnect.
 */
 
void lnx_qllc_notify(lnxtype *lnx, lnx_dlc_event_t event)
{
    if (!lnx) {
	if (lnx_error_debug){
	    buginf("\nQLLC: Invalid lnx in lnx_qllc_notify");
	  }
	return;
    }
    
    if (qllc_state_debug) {
	buginf("\nQLLC %e lnx_qllc_notify state %s event %d", lnx->vmac_addr,
		lnx_prstate(lnx->lnx_state), event);
    }

    switch(event) {

    case LNX_DLC_LINKUP_IND:	
	lnx->lnx_xid_seen = FALSE;
	switch (lnx->lnx_state) {
	    
	case LNX_AW_NET:
	case LNX_AW_QLLC_SEC:
	case LNX_CONNECT:
	case LNX_NET_CONTACT_PENDING:
	    lnx_disconnect_net(lnx);
	    break;
	    
	case LNX_DW_NET:
	    /*
	     * When in DW_NET there is something actually happening.  
	     * Leave it alone.
	     */
	    break;
	    
	case LNX_DISCONNECT:
	case LNX_DW_QLLC:
	case LNX_AW_QLLC_PRI:
	    lnx_setstate(lnx, LNX_DISCONNECT);
	    break;

	case LNX_AW_NET_QLLC_SEC:
	    break;
	}
	break;
	    
    case LNX_DLC_LINKDOWN_IND:	/* virtual circuit lost			*/
	lnx->lnx_xid_seen = FALSE;
	switch (lnx->lnx_state) {

	case LNX_DW_QLLC:	/* what we were waiting for
				 * to be safe we make sure the network
				 * side comes down
				 */
	    lnx_disconnect_net(lnx);	/* bring it down */
	    break;
		
	case LNX_AW_NET:
	case LNX_AW_QLLC_SEC:
	case LNX_AW_QLLC_PRI:
	case LNX_CONNECT:
	case LNX_NET_CONTACT_PENDING:
	case LNX_AW_NET_QLLC_SEC:
	    lnx_disconnect_net(lnx);	/* bring it down */
	    break;
	    
	case LNX_DW_NET:		/* wait right here */
	    break;			/* don't change state */
	    
	case LNX_DISCONNECT:		/* we're already here */
	    break;
	}
	break;
	
    case LNX_DLC_CONTACT_IND:	/* received QSM				*/
	switch (lnx->lnx_state) {
	    
	case LNX_DISCONNECT:		/* DLC is idle */
	case LNX_AW_NET_QLLC_SEC:	/* Called out - call accepted */
	    lnx_contact_net(lnx);	
	    /* start the contact operation on the net side */
	    break;
	    
	case LNX_DW_QLLC:	/* waiting for qllc to disconnect - not used */
	    break;
	    
	case LNX_AW_QLLC_PRI:	/* awaiting for primary sdlc to come up */
	    lnx_disconnect_net(lnx);
	    break;
	    
	case LNX_AW_NET:	/* waiting for net media, TR or NET */
	case LNX_AW_QLLC_SEC:	/* waiting for secondary qllc to come up */
	case LNX_NET_CONTACT_PENDING: 
	    /* already received QSM on DLC side, sent SABME/LUR */
	    break;		/* ignore */

	case LNX_DW_NET:	/* waiting for network side to get torn down */
	    qllc_contact_reject(lnx->qllc);
	    break;		/* refuse the contact request */

	case LNX_CONNECT:	/* fully connected, data flows */
	    qllc_contact_accept(lnx->qllc);
	    qllc_quench(lnx->qllc, FALSE); /* unthrottle X.25 input */
	}

        break;
	
    case LNX_DLC_CONTACTED_IND:	/* received QUA				*/
       switch (lnx->lnx_state) {
	    
	case LNX_AW_QLLC_PRI:	/* waiting for qllc side to come up */
            /* start the contact operation on the net side */
            lnx_contacted(lnx);
	    break;

	  default:
            break;

	}
    }
    return;
    
}

/* l n x _ c o n t a c t _ n e t
 *
 * start the net contact process,
 * at the moment we don't handle call-in from a FEP on a PVC that just
 * wants to connect to a Secondary, and not even bother with XID exchange
 *
 * Build a packet that can be put on the lnx_net_linkupQ
 *
 * Call llc2_open1/ or l2mi_dlc_ind_ext_conn
 directly rather than ask lnx_net_linkup_proc
 * to do it for us
 * PHIL 12/11/93
 */

 
void lnx_contact_net(lnxtype *lnx)
{
    llctype		*llc;
    paktype		*pak;
    idbtype             *swidb;
    hwaddrtype          srcMac, destMac;

    if (lnx_local_ack_this(lnx) ) {		/* handle local ack first */
	lnx->lnx_lak_enable = TRUE;

	if (lnx_create_lackie(lnx)) {
		lnx_lak2_dlc_event(lnx, l2mi_dlc_ind_ext_conn);
		lnx_setstate(lnx, LNX_NET_CONTACT_PENDING);
	}
	return;
    }
    
    lnx->lnx_lak_enable = FALSE;
    datagram_done(lnx_get_xid1_pak(lnx));
    datagram_done(lnx_get_startup_pak(lnx));

    snpa_init(&destMac, STATION_IEEE48, TARGET_UNICAST, lnx->prtnr);
    snpa_init(&srcMac, STATION_IEEE48, TARGET_UNICAST, lnx->vmac_addr);
    srcMac.addr[0] |= TR_RII;

    swidb = lnx->idb->hwptr->vidb_link->firstsw;
    llc = llc2_open1(lnx->dsap, lnx->ssap, &destMac, &srcMac,
					 swidb,
					 LINK_LLC2, 
                                         llc_sb_get_wind(swidb),
					 NULL, TRUE, TRUE,
					 FALSE, NULL_L3_PID,NULL_DLCI); /* Not a clsi_call */
			
    if (!llc) {
	if (lnx_error_debug){
	    buginf("\nQLLC: %e llc2_open1 failed");
	}
	return;
    }
    
	/* Queue up a packet for the lnx_net_linkupQ */
	pak = getbuffer(0);
	if (!pak) {
	    llc2_close(llc);
	    return;
	}
	pak->lnx_cb = lnx;
	if (!process_enqueue(lnx_net_linkupQ, pak) ) {
	  if (lnx_error_debug)
	    buginf("\nQLLC: pak enqueue lnx_net_linkupQ failed");
	  datagram_done(pak);
	  llc2_close(llc);
	}
	lnx->llc = llc;
	lnx_setstate(lnx, LNX_NET_CONTACT_PENDING);

}	

/* xid1_glean_maxbtu
 *
 * Get the info from the XID1 packet
 * and stash in the lnx_dlc
 * Allow one extra byte for the 0xBF field - normalization bug 
 */
 
static inline void xid1_glean_maxbtu (paktype *pak, uchar *stash)
{
    PUTSHORT (stash, GETSHORT(pak->datagramstart+1+ XID1_MAXBTU_OFF));
}

/* is_xid1_primary
 *
 * tell whether a XID1 packet is Primary or Secondary
 */
static boolean is_xid1_primary (paktype *pak)
{
	return( *(pak->datagramstart+1+XID1_ROLE_TWS_OFF) & XID1_PRIMARY_MASK);
}

/* xid1_set_null
 *
 * convert XID Fmt 1 packet to XID_NULL
 * incoming call was Primary
 */

static inline void  xid1_set_null (paktype *pak)
{
	pak->datagramsize = 1;	/* chop off the rest of the XID continuation */
}

/* xid1_set_xid0T2
 *
 * Convert a secondary XID Fmt 1 packet to Format 0 Type 2
 * Input packet is 'normalized' (pak->datagram_start points to 0xBF)
 */
 
static inline void  xid1_set_xid0T2 (paktype *pak)
{
	pak->datagramsize = 1+ XID0_SIZE; /* chop off the rest of the XID continuation */
	pak->datagramstart[1] = XID_TYPE0_FMT2;
	pak->datagramstart[2] = 0;	/* Fixed format XID - no length field */
}

/*
 * lnx_contacted
 *
 * qllc side has resonded to the QSM with a QUA
 * send the pak for the SABME to the llc2 state machine
 * so a UA is sent to the lan partner and the llc and
 * lnx connection are in the proper states
 */
 
static void lnx_contacted(lnxtype *lnx)
{
  paktype  *pak;

  /* get the pak for the sabme that we saved in lnx_dlc_linkup_proc */
  pak = lnx_get_startup_pak(lnx);
  
  if (lnx->qllc) {
   if (lnx->lnx_state == LNX_AW_QLLC_PRI) {

     if (lnx->lnx_lak_enable) { 
		    
      /* set state to LNX_CONNECT, and wait for
       * lack2 FSM to complete by
       * sending Connect Poll Request
       * (state l2st_connectpending)
       * receiving Connect Poll Ack Request,
       * clearing busy.
       * sending Connect Poll Ack (state l2st_connect)
       */
			 
       lnx_setstate(lnx, LNX_CONNECT);
       lnx_lak2_dlc_event (lnx, l2mi_dlc_ind_opened);
       datagram_done(pak);
     } else {  
	 if (pak == NULL) {		/* error */
	     if (lnx_error_debug) {
		 buginf("\nLNX: startup pak is NULL");
	     }
	     qllc_close(lnx->qllc);
	     return;
	 }
        /* regular LLC 2 connection */
        lnx_setstate(lnx, LNX_AW_NET);
        pak->flags |= PAK_LACK_IN;
        llc2_input(pak);		/* This will consume the packet */
        /* let llc2 send out the UA */
        /* this calls lnx_fix_llc which calls
         * lnx_setstate(lnx, LNX_CONNECT);
	 * and sets llc->lnx_active = TRUE;
         */

        if (lnx->llc == NULL) {
         if (lnx_error_debug)
 	  buginf("\nQLLC: no llc after receiving SABME");
	 qllc_close(lnx->qllc);
        }
      } 
     } else {
        /* state changed for some reason */
        qllc_close(lnx->qllc);
        lnx->qllc = NULL;
	datagram_done(pak);
     } 

  }
}


/* lnx_local_ack_this
 *
 * Should this lnx start a local ack session
 */

boolean lnx_local_ack_this(lnxtype *lnx)
{
    boolean	result;
    leveltype	level;

    if (!lnx_lak_enable)
	return (FALSE);

    result = FALSE;
    level = raise_interrupt_level(NETS_DISABLE);

    if (lnx->lnx_startup_pak)
	result = dlc_local_ack_this(lnx->lnx_startup_pak, lnx->idb->hwptr, 
			    (tr_ventrytype *)lnx->lnx_vre);
    reset_interrupt_level(level);
    return(result);
}
 
/* l n x _ n e t _ d i s c _ a c k
 *
 * we received something (DISC - only used in SRB/RSRB - not LOCAL ACK
 *
 * acknowldge it to make the remote station or peer happy
 */
 
void lnx_net_disc_ack(lnxtype *lnx, paktype *pak)
{
    pak->flags |= PAK_LACK_IN;
    llc2_input(pak); 
}


/* l n x _ d i s c o n n e c t _ n e t
 *
 * For what ever reason it's necessary, we disconnect the net
 * (LLC2/LACK) interface
 *
 * The routine condenses the functionality to just one place
 *
 */
 
void lnx_disconnect_net(lnxtype *lnx)
{
    /* we're waiting for LAN side to come up, abort */
    if (!lnx) {
	if (lnx_error_debug)
	    buginf("\nQLLC : lnx_disconnect_net invalid lnx");
	return;
    }

    lnx_setstate(lnx, LNX_DW_NET);

    if (lnx->lnx_lak_enable) {
        /* LAK2: it's dlc_ind_closed */
        lnx_lak2_dlc_event( lnx, l2mi_dlc_ind_closed);
	unset_rif(NULL, 0, lnx->prtnr, lnx->vmac_addr, FALSE);
    } else  if (lnx->llc) {
	/*
	 * qllc side is going down.  If there is an LLC open close it.
	 * When this finishes (can take some time) the lnx_state
	 * will get properly set.
	 *
	 * If there is no LLC connection simply go right to DISCONNECT.
	 */
	llc2_close(lnx->llc);
    } else
    	/* neither local-ack nor llc2 connection is in place
	 * can just go to disconnect
	 */
	lnx_setstate(lnx, LNX_DISCONNECT);
}

/* l n x _ n e t _ i n d _ d i s c o n n e c t
 *
 * a disconnection indication from the lnx network side is being
 * signaled to lnx.
 *
 * This forces a disconnection from any state.  lnx goes to disconnect
 * and we force SDLC into DISCSENT if it was not disconnected or DISCSENT.
 */

void lnx_net_ind_disconnect (lnxtype *lnx)
{
    lnx_setstate(lnx, LNX_DISCONNECT);
/*
 * QLLC TREATS qllc_close AS AN ATOMIC OPERATION; SDLLC DOESN'T
 *
 * PHIL 8/23/9
 *
 * QLLC SHOULDN'T EITHER
 *
 * compare SDLLC code
 */
    qllc_close(lnx->qllc);

    unset_rif(NULL, 0, lnx->prtnr, lnx->vmac_addr, FALSE);
}


/* l n x _ n e t _ i n d _ d m
 *
 * a disconnect mode indication from the lnx network side is being
 * signaled to lnx.
 *
 * This forces a disconnection from any state.  lnx goes to disconnect
 * and we forward the DM frame to the qllc side(as a QDM rsp).
 */

void lnx_net_ind_dm (lnxtype *lnx, paktype *pak)
{
    if (lnx->qllc) {
        qllc_sendcontrol(lnx->qllc, QDM, FALSE);
        qllc_goto_adm(lnx->qllc, QLS_DISCONNECT, TRUE);
    }
    pak = NULL;
}

/*
 * l n x _ n e t _ i n d _ c o n n e c t
 *
 * a connection indication from the lnx network side (llc2/lack) is being
 * signalled to lnx.
 */

void lnx_net_ind_connect (lnxtype *lnx)
{
    if (lnx->lnx_state != LNX_AW_NET && lnx_error_debug){
         buginf("\nQLLC: net_ind_connect wrong state: %s for %e",
	       lnx_prstate(lnx->lnx_state), lnx->vmac_addr);
    }
/* NOTE sdllc_ind_connect provides support for downstream startup;
 *
 * Can't use their code till we have more intellignece about
 * primary/secondary
 * PHIL 9/6/93
 */

    lnx_setstate(lnx, LNX_CONNECT);
    lnx_quench(lnx, FALSE);
}

/* lnx_map_net_event
 *
 * analyse input and categtorize the event
 */
 
lnx_net_event_t lnx_map_net_event(lnxtype *lnx, int opcode, uchar conch)
{
    switch(opcode)
        {
        case RSRB_OP_NONE:
        case RSRB_OP_FORWARD:
        case RSRB_OP_FORWARDCRC:
        case RSRB_OP_QLLC:
        case RSRB_OP_LLC2DATA:
        case RSRB_OP_FORWARDFST:
        case RSRB_OP_FORWARDFSTCRC:

#ifdef notdef
/* For now we make no distinction between I and S frames
 * This is the code that would do that 
 *
 * IN FACT WE NEED TO DISTINGUISH RNR FROM RR AND REJ FOR QUENCH TOWARDS
 * X.25
 *
 * PHIL 1/12/94
 */
	    /* I Frames */
	    if ((conch & LLC_IMASK) == LLC_IFRAME)
		return (LNX_DATA);

	    /* S Frames */
	    if ((conch & LLC_SMASK) == LLC_SUPERVISORY)
		return (LNX_SFRAME);
#else
	    if ((conch & LLC_UNNUMBERED) != LLC_UNNUMBERED)
		return (LNX_DATA);
#endif
	    /* U Frames */ 
            switch (conch)
                {
                case SABME:
                    return (LNX_CONN_REQ);
 
                case DISC:		/* can also be RD */
                    return (LNX_DISC_REQ);
 
                case UA:
                    if (lnx->lnx_lak_enable) {
                     if (lnx_error_debug)
                      buginf("\nQLLC: Local Ack, but we got UA. Discarding");
                     return (LNX_INVALID);
                    }
                    return (LNX_UA);
 
                case DM:
                    if (lnx->lnx_lak_enable) {
                     if (lnx_error_debug)
                      buginf("\nQLLC: Local Ack, but we got DM. Discarding");
                     return (LNX_INVALID);
                    }
                    return (LNX_DISC_REQ);
 
                }
            break;
 
        }
 
    return (LNX_INVALID);
}

/*
 * lnx_input - input to qllc from llc2.  This routine processes all llc2
 * frames. LLC2 frames which change link states (SABME, DISC/DM, FRMR) have
 * their corresponding QLLC counterparts sent to the X.25 stations.  All S/U
 * frames are processed and freed by llc2.  I-frames are forwarded to QLLC 
 * and freed when they receive a +ve response from the remote X.25 station. 
 */

void lnx_input (hwidbtype *inidb, paktype *pak, int opcode, tr_ventrytype *vre)
{
 
   uchar                conch;
   trrif_t              *trh;
   int                  riflen = 0;
   leveltype            level;
   sap_hdr		*llc1;
   lnxtype              *lnx = NULL;
   boolean              lnx_error = FALSE;
   lnx_net_event_t      lnx_net_event;
   boolean              qllc_quenchon = FALSE;
 
   trh                 = (trrif_t *)pak->datagramstart;
   riflen              = trh->rc_blth & RC_LTHMASK;
   pak->llc2_sapoffset = TRING_ENCAPBYTES + riflen;
   llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);
 
   conch = llc1->control & ~P_BIT;     /* snuff out p/f bit */
 
   /*
    * Scan all the x.25 interfaces to see if da matches (virtual)
    * lnx token ring address.  If no match, exit the funtion.
    */
   lnx = lnx_by_macaddr(trh->daddr);
   if (!lnx) {
       if (lnx_error_debug)
           buginf("\nQLLC: lnx_input - entry for %e not found", trh->daddr);
           vr_nukepak(pak);
           return;
   }
 
   /* handle TEST/XID */
   if (conch == XID || conch == LLC_TEST) {
       lnx_startup_seq(pak, lnx, inidb, vre);
        return;
   }
 
   switch (opcode) {	/* handle LACK2 and LACK OP CODES RIGHT AWAY */

       /* OLD LACK OP CODES - SHOULDN"T EVER HAPPEN */

      case RSRB_OP_LINKUP_REQ:
      case RSRB_OP_LINKDOWN_REQ:
      case RSRB_OP_LINKUP_PRSP:
      case RSRB_OP_LINKDOWN_PRSP:
      case RSRB_OP_QUENCH_ON:
      case RSRB_OP_QUENCH_OFF:

       if (lnx_error_debug)
	   buginf("\nQLLC: Opcode %s Invalid", vr_op2str(opcode));
       vr_nukepak(pak);
       return;

       /* LACK2 OP CODES - THEY'RE DONE NOW */

      case RSRB_OP_ConnectRequest:
      	if (lnx_lak_enable)
		lnx->lnx_lak_enable = TRUE;
	
	/* fall through */
	
      case RSRB_OP_ConnectPollReq:
      case RSRB_OP_ConnectPollAck:
      case RSRB_OP_ConnectPollAckAndReq:
      case RSRB_OP_ConnectWaitAck:
      case RSRB_OP_DisconnectRequest:
      case RSRB_OP_DisconnectAck:
      case RSRB_OP_LA2_QON:
      case RSRB_OP_LA2_QON_ACK:
      case RSRB_OP_LA2_QOFF:
      case RSRB_OP_LA2_QOFF_ACK:
       if ( lnx_debug || qllc_event_debug)
	   buginf("\nQLLC %e %s net <-%s",
		  lnx->vmac_addr, lnx_prstate(lnx->lnx_state), vr_op2str(opcode));

       lak2_process_netmsg(pak, opcode, vre, DLC_QLLC);
       return;

     default:
       break;
   }

   /* now the only packets we have left are 'real' SRB control packets
    * and data packets
    */

   lnx_net_event = lnx_map_net_event(lnx, opcode, conch);
 
   if (lnx_net_event == LNX_INVALID) {
       if (lnx_error_debug)
        buginf("\nQLLC: Invalid input op %s conch %02x",
                  vr_op2str(opcode), conch);
       vr_nukepak(pak);
       return;
   }
 
   if ( lnx_debug || (qllc_event_debug && 
	      (lnx_net_event != LNX_DATA || lnx->lnx_state != LNX_CONNECT)) ) {

       buginf("\nQLLC: %e %s net <-%s (%s)%02x",
	      lnx->vmac_addr, lnx_prstate(lnx->lnx_state),
	      lnx_pr_event(lnx_net_event),
	      vr_op2str(opcode), conch);

       if (lnx_net_event == LNX_DATA && (conch & LLC_IMASK) == LLC_IFRAME)
        if (qllc_event_debug)
	 buginf("\nQLLC: %d bytes", pak->datagramsize);
   }
 
   /* input processing finished: drive it through the LNX state machine
    *
    * state processing doesn't nuke the pak - we do that at the bottom
    * if we consume the pak clear the variable
    */
 
   switch (lnx->lnx_state) {
 
   case LNX_DISCONNECT:
   case LNX_AW_NET_QLLC_SEC:

       switch (lnx_net_event) {
 
       default:
           lnx_error = TRUE;
           break;
 
       case LNX_UA:
           break;		/* ignore, it's only weird, not an error */

       case LNX_DM:             /* lan rsp states that llc is disconnected */
           lnx_net_ind_dm(lnx,pak);
           break;

       case LNX_DISC_REQ:
           /* some one is still connected - huh
            * make them happy and get out
            */
           lnx_net_disc_ack(lnx, pak);
           pak = NULL;
           break;
 
      case LNX_CONN_REQ:
           /* received SABME
	    * infer that QLLC is downstream
            * Start the downstream side
            *
            * Note that we DON'T acknowldge the conn req, since
            * the downstream side isn't up yet
            */
 
	   lnx->lnx_lak_enable = FALSE;
 
           /* Add this frame's source address to the RIF cache */
           rif_update(NULL, 0, trh->saddr, trh->daddr,
               (srbroute_t *) (riflen ? &trh->rc_blth : NULL), RIF_NOAGE);

           lnx_setstate(lnx, LNX_AW_QLLC_PRI);	/* note changed state */
           /* queue the pak for the asychronous qllc_open operation */
           if (process_enqueue(lnx_dlc_linkupQ, pak)) {
	       pak = NULL;				/* mark as consumed */
	   }
           else
            lnx_setstate(lnx, LNX_DISCONNECT);
           break;
       }
       break;
 
   case LNX_DW_QLLC:
       lnx_error = TRUE;
       break;           /* not used so far */
 
   case LNX_AW_QLLC_PRI:
       switch (lnx_net_event) {
 
       default:         /* actually invalid, but we just ignore it */
           lnx_error = TRUE;
           break;
 
       case LNX_CONN_REQ:       /* valid - it's a retry, ignore - hey,
                                 * we're trying to bring up the qllc
                                 * side anyway
                                 */
           break;
 
       case LNX_DISC_REQ:       /* no net connection up, ack this and
                                 * close qllc
                                 */
           lnx_net_disc_ack(lnx, pak);
           pak = NULL;
	   qllc_close(lnx->qllc);
           break;

       case LNX_DM:             /* lan rsp states that llc is disconnected */
           lnx_net_ind_dm(lnx,pak);
           break;

       }
       break;
 
   case LNX_DW_NET:
 
       switch (lnx_net_event) {
 
       default:
           lnx_error = TRUE;
           break;
 
       case LNX_UA:		/* pass UA into LLC2 machine to complete */
       case LNX_DISC_REQ:	/* pass DISC into LLC2 machine to get UA out */
           lnx_net_disc_ack(lnx, pak);
           pak = NULL;
           lnx_setstate (lnx, LNX_DISCONNECT);
           break;

         case LNX_DM:           /* lan rsp states that llc is disconnected */
           lnx_net_ind_dm(lnx,pak);
           break;
        }
       break;
 
   case LNX_AW_NET:
 
       /* this case is tricky; it can mean that we're doing reverse
        * startup and waiting for a net connection accept,
        * OR
        * it can mean that the downstream side is up and we're waiting for
        * the network to try to connect again
        *
        */
 
 
       switch (lnx_net_event) {
 
       default:
           lnx_error = TRUE;
           break;
 
       case LNX_UA:
           level = raise_interrupt_level(NETS_DISABLE);
           pak->flags |= PAK_LACK_IN;
           llc2_input(pak);  /* this goes round robin hood's barn and finally
			      * we get to state LNX_CONNECT
			      */

           reset_interrupt_level (level);
           pak = NULL; 
           break;
       
       case LNX_DM:             /* lan rsp states that llc is disconnected */
           lnx_net_ind_dm(lnx,pak);
           break;
 
       case LNX_DISC_REQ:
           lnx_net_disc_ack(lnx, pak);
           pak = NULL;
	   /* fall through to the next case */

       case LNX_CONN_REQ:       /* we've been waiting for this, now we
                                 * can ack, and go to CONNECTED
                                 */
 
           /* it has to be SABME, can be passed to llc2_input */
           level = raise_interrupt_level(NETS_DISABLE);
           pak->flags |= PAK_LACK_IN;
           llc2_input(pak);     /* this calls lnx_fix_llc which calls
                                 * lnx_setstate(lnx, LNX_CONNECT);
                                 */
           reset_interrupt_level (level);
           pak = NULL;          /* consumed */
           if (lnx->lnx_state == LNX_CONNECT)
                /* forward any pent-up packets to the remote */
                qllc_quench (lnx->qllc, FALSE );
           break;
 
       }
       break;
 
   case LNX_NET_CONTACT_PENDING:	/* sent SABME/LINKUP_REQ */

       switch (lnx_net_event) {

       default:
	   lnx_error = TRUE;
	   break;

       case LNX_CONN_REQ:
	   lnx_error = TRUE;
	   break;

       case LNX_DISC_REQ:
	   lnx_net_ind_disconnect(lnx);
	   break;

       case LNX_DM:             /* lan rsp states that llc is disconnected */
           lnx_net_ind_dm(lnx,pak);
           break;

       case LNX_UA:
	   if (lnx->lnx_lak_enable) {
	       if (lnx_error_debug) {
		   buginf("\nQLLC Rx UA, and LACK linkup REQ sent");
	       }
	       lnx_contact_net(lnx);
	       break;
	   }
	   pak->flags |= PAK_LACK_IN;
	   llc2_input(pak);		/* this will wake up the llc2_open2 in
					 * lnx_net_linkup_proc
					 */
            pak = NULL;          /* bchan: pak nu twice */
           /* Add this frame's source address to the RIF cache */
           rif_update(NULL, 0, trh->saddr, trh->daddr,
               (srbroute_t *) (riflen ? &trh->rc_blth : NULL), RIF_NOAGE);

	   break;

       case LNX_QUENCH_ON:
       case LNX_QUENCH_OFF:
       case LNX_DATA:
	   lnx_error = TRUE;
	   break;
       }
       
       break;

   case LNX_AW_QLLC_SEC:
       lnx_error = TRUE;
       break;
 
   case LNX_CONNECT:
 
       switch (lnx_net_event) {
 
       default:
           lnx_error = TRUE;
           break;
 
       case LNX_CONN_REQ:       /* we've been waiting for this, now we
                                 * can ack, and go to CONNECTED
                                 */
           /* if we've already seen data on this connection, the net
            * side has no business sending us another SABME
            * Make it not so and GET OUT
            */
           if (lnx->lnx_net_data_seen) {
               lnx_error = TRUE;
               break;
           }
 
           /* it has to be SABME, can be passed to llc2_input */
           level = raise_interrupt_level(NETS_DISABLE);
           pak->flags |= PAK_LACK_IN;
           llc2_input(pak);     /* this calls lnx_fix_llc which calls
                                 * lnx_setstate(lnx, LNX_CONNECT);
                                 */
           reset_interrupt_level (level);
           pak = NULL;          /* consumed */
           break;
 
       case LNX_DATA:
           lnx->lnx_net_data_seen = TRUE;
           if (!lnx->lnx_lak_enable) {          /* good ole llc2 */
               level = raise_interrupt_level(NETS_DISABLE);
               pak->flags |= PAK_LACK_IN;
               llc2_input(pak);
               reset_interrupt_level(level);
 
               if (!lnx->llc) {
                   /*
                    * incoming frame tore the llc connection down completely
                    * don't do anything
                    */
                   pak = NULL;
                   break;
               }
 
               if ((conch & LLC_IMASK) != LLC_IFRAME) {
                   /*
                    * llc2_input consumes everything except I-Frames
                    */
                   pak = NULL;
                   break;
               }
 
           }
 
           /* Data frames: pass them out to QLLC (fragmentation optional) */
 
           if ((conch & LLC_IMASK) == LLC_IFRAME ||
                opcode == RSRB_OP_LLC2DATA) {
 
               if (lnx->lnx_lak_enable) {
                  pak->datagramstart += TRING_ENCAPBYTES + riflen + 4;
                  pak->datagramsize  -= (TRING_ENCAPBYTES + riflen + 4);
                  pak->flags         |= PAK_LACK_OUT;
               }
               if (pak->flags & PAK_LACK_OUT) {
                   /*
                    * LACK_OUT gets set immediately above or by llc2_input if
                    * llc2 accepted this I frame.
                    */
 
                   /*
                    * SO WHAT KIND OF DATA PACKET GETS HERE THAT DOESN'T HAVE
                    * PAK_LAK_OUT - maybe an retransmitted LLC2 packet ?
                    *
                    * PHIL
                    */
 
                   QllcSendThruQ(lnx, pak);
                   pak = NULL;          /* forwarded it to QLLC; mark consumed */
 
               } else {
                   if (lnx_error_debug)
                    buginf("\nQLLC: PAK_LACK_OUT flag is not set");
               }
 
           }
 	   pak = NULL;	/* mark consumed */
           break;
 
       case LNX_DISC_REQ:
           lnx_net_disc_ack(lnx, pak);
	   if ( lnx->qllc )
	       qllc_close(lnx->qllc);
	   lnx_setstate (lnx, LNX_DISCONNECT);
           pak = NULL;
           break;
   
      case LNX_DM:             /* lan rsp states that llc is disconnected */
           lnx_net_ind_dm(lnx,pak);
           break;

       case LNX_QUENCH_ON:
	   qllc_quenchon = TRUE;	/* fall through */
       case LNX_QUENCH_OFF:
           qllc_quench(lnx->qllc, qllc_quenchon);
           break;
       }
       break;
   }		/* end of switch(lnx->lnx_state)  */
 
   if (lnx_error) {
       if (lnx_error_debug)
        buginf("\nQLLC: %e, State %s invalid input %s. Bailing.",
		  lnx->vmac_addr, lnx_prstate(lnx->lnx_state),
		  lnx_pr_event(lnx_net_event));
 
   }
   if (pak)
       vr_nukepak(pak);
 
   return;
}



/*
 * lnx_quench
 *
 * qllc told us that it just filled or emptied its hold queue. 
 * Send the necesssary message to LLC2 or RSRB
 */

void lnx_quench(lnxtype *lnx, boolean quench_on)
{
 
    if (lnx) {
	if (qllc_event_debug)
	    buginf("\nQLLC: NET QUENCH %s", quench_on ? "ON": "OFF");

        if (quench_on) {
            lnx->lnx_quench_on_count++;
 
            if (lnx->lnx_lak_enable) {
                /* generate a QUENCH_ON cmd to remote llc2 session */
                lnx_lak2_dlc_event(lnx, l2mi_dlc_ind_txqhigh);
            } else if (lnx->llc) {
                 llc2_stateswitch(lnx->llc, SET_LOCAL_BUSY);
            }
        } else {
            lnx->lnx_quench_off_count++;
 
            if (lnx->lnx_lak_enable) {
                /* generate a QUENCH_OFF cmd to remote llc2 session */
                lnx_lak2_dlc_event(lnx, l2mi_dlc_ind_txqlow);
            } else if (lnx->llc) {
                 llc2_stateswitch(lnx->llc, CLEAR_LOCAL_BUSY);
            }
        }
    }
    else {
      if (lnx_error_debug)
        buginf("\nQLLC: QUENCH - data structure missing");
    }
}

/* lnx_initiate
 *
 * handle incoming call from X.25.
 * Once incoming call is received and accepted by X.25, and
 * come here
 * and find the source route to the host.
 * Send out an explorer, and let lnx_startup_seq 
 * send out the XIDs needed to activate the host.
 */

void lnx_initiate (lnxtype *lnx)
{
    hwidbtype	*vidb;
    riftype   	*rifp;
    paktype  	*test_pak;
   
   vidb  = lnx->idb->hwptr->vidb_link;
    if (!lnx->actual_prtnr || lnx->lnx_state != LNX_DISCONNECT) {
        return;
    }
 
    rifp = rif_lookup(lnx->prtnr, NULL, &lnx->vmac_addr[0],0);
    test_pak = llc1_gen_explr(lnx->prtnr, &lnx->vmac_addr[0], SAP_NULL, lnx->ssap, vidb);
    lnx->lnx_test_rsp_pending = TRUE;
    if (test_pak) {
        test_pak->flags |= PAK_SRB_IN;
        test_pak->if_input =  vidb->firstsw;
        if (qllc_event_debug) {
            buginf("\nQLLC: tx POLLING TEST, da %e, sa %e",
                    lnx->prtnr, &lnx->vmac_addr[0]);
        }
        srb_enq(srb_explorerQ, test_pak);    
    }
    mgd_timer_start(&lnx->qllc->fTimer, QT1_DEFAULT);   
}

/*
 * lnx_net_fwd_xid
 *
 * we got an XID from the data link layer; pass it in to the Net
 */
void lnx_net_fwd_xid (lnxtype *lnx, paktype *pak, boolean qllc_response)
{
    riftype     *rif;
    leveltype	level;

    clear_if_input(pak);        /* packet no longer charged to X.25 input */
 
    if (!lnx->qllc) {
        if (lnx_error_debug)
         buginf("\nQLLC: no qllc structure to forward xid");
        vr_nukepak(pak);
        return;
    }

    if (lnx->lnx_state == LNX_CONNECT) {
        if (lnx_error_debug)
	    buginf("\nQLLC: %e in CONNECTED State. Discarding XID from X.25 link",
		   lnx->vmac_addr);
	datagram_done(pak);
	return;
    }
    if (!(rif = rif_lookup(lnx->prtnr, NULL, &lnx->vmac_addr[0], 0) )) {
	if (lnx_error_debug)
	    buginf("\nQLLC: couldn't find rif for partner (%e)", lnx->prtnr);
        vr_nukepak(pak);
        return;
    }

    if ( lnx_is_xid3(pak)) {
        /* save & spoof the ABM bit */
        lnx_xid_spoof_abm(pak, &lnx->lnx_qllc_abm, &lnx->lnx_llc_abm);
        lnx_xid_glean_maxbtu( (xid3type *)pak->datagramstart,
                          (uchar *)&lnx->lnx_dlc_maxbtu);
        lnx->qllc->xid32_from_remote = (xid_get_node_type(pak->datagramstart + 1) == XID_TYPE_2);

    } else if (is_xid1(pak)) {

        if (qllc_state_debug) {
            int i;

            buginf("\nXID1: %d bytes from X.25 ", pak->datagramsize);
            for (i=0; i < pak->datagramsize; i++) {
                buginf("%02x ", pak->datagramstart[i]);
            }
        }
	/* spoof XID1 handling to the host */
	datagram_done (lnx_get_xid1_pak(lnx));

	level = raise_interrupt_level(NETS_DISABLE);
	lnx->lnx_xid1_pak = pak;
	xid1_glean_maxbtu(pak, (uchar *)&lnx->lnx_dlc_maxbtu);
	pak = pak_clip(pak, 1+XID0_SIZE);
	reset_interrupt_level(level);
	if (pak == NULL) {
	    datagram_done(lnx_get_xid1_pak(lnx));
	    return;
	}
	if (is_xid1_primary(pak)) {
	    xid1_set_null(pak);
	} else {
	    xid1_set_xid0T2(pak);
	}
        if (qllc_test_debug) {
            int i;

            buginf("\nXID1 %d bytes to LAN ", pak->datagramsize);
            for (i=0; i < pak->datagramsize; i++) {
                buginf("%02x ", pak->datagramstart[i]);
            }
	}
    } else if (is_xidnull(pak, 0)) { 
        /* 
         *  Forward NULL QXID Rsp if we owe a response.
         *  Forward NULL QXID Cmd if response not expected.
         */
	if (lnx->lnx_xid_rsp_pending ^ qllc_response) {
	    vr_nukepak(pak);
	    return;
	}
    }
 
    /* put the right headers on it. */
    pak = lnx_to_llc_xid(lnx, pak, (uchar *)(rif->rif),
                             lnx->idb->hwptr->vidb_link);
    if (pak != NULL) 
        datagram_out(pak);
   
}
 
/* prepend mac header, and llc1 header, to make the XID suitable for
 * Token Ring transport,
 *
 * Special case for a NULL XID
 */
 
 
paktype *lnx_to_llc_xid( lnxtype *lnx, paktype *pak,
                       uchar *rif, hwidbtype *vidb)
{
    llc1_hdr_t          *llc1;
    tring_hdr           *trframe;
    int                 riflen;
    trrif_t             *pakrif;
    int                 offset;
 
    if (!rif) {
        errmsg(&msgsym(NULLPTR, QLLC), "rif");
        datagram_done(pak);
        return (NULL);
    }

    riflen = rif[0] & RC_LTHMASK;
 
    /* compute offset */
 
    offset = TRING_ENCAPBYTES + riflen + sizeof(lnx->ssap) + sizeof(lnx->dsap);
 
    pak->datagramstart -= offset;
    pak->datagramsize  += offset;
 
    /* compute prepend offsets
     * working backward from the business end of the XID, which
     *  starts in the middle of the llc1 header.  Blech.
     */
 
    trframe = (tring_hdr *)  ( pak->datagramstart );
    pakrif  = (trrif_t *)   ( pak->datagramstart + TRING_ENCAPBYTES );
    llc1    = (llc1_hdr_t *)( pak->datagramstart + TRING_ENCAPBYTES + riflen );
 
    trframe->ac = AC_PRI0;
    trframe->fc = FC_TYPE_LLC;
 
 
    ieee_copy(lnx->prtnr, trframe->daddr);
    ieee_copy(lnx->vmac_addr, trframe->saddr);

    /* set the RII bits for SRB */
    trframe->saddr[0] |= TR_RII;
 
    /* fill in the rif */
    bcopy(rif, pakrif, riflen);
 
    /* fill in the llc1 header */
    llc1->dsap = lnx->dsap;
    llc1->ssap = lnx->ssap;
    if (lnx->lnx_xid_rsp_pending) {
	lnx->lnx_xid_rsp_pending = FALSE;
        llc1->ssap |= SAP_RESPONSE;
    }
 
    pak->if_output = vidb->firstsw;
    pak->enctype = vidb->enctype;
    return(pak);
}



/*
 * lnx_output - Data frames received from QLLC are now to be passed 
 * to LLC2, or to Local Ack.
 *
 * We come here from qllc_input, and can assume that qllc is valid.
 */

void lnx_output (paktype *pak, qllctype *qllc)
{
    lnxtype *lnx;
    leveltype level;
    
    if (! (lnx = qllc->lnx) ) {
	datagram_done(pak);
    }

    if (lnx->lnx_state == LNX_CONNECT) {
	clear_if_input(pak);      /* packet no longer charged to X.25 input */
	pak->if_output = lnx->idb->hwptr->vidb_link->firstsw;
 
	if (lnx->lnx_lak_enable) {
	    lnx_vrforward_data (lnx, pak);
	} else {
	    pak->flags    |= PAK_LACK_OUT;
	    level          = raise_interrupt_level(NETS_DISABLE);
	    if (llc_get_oqueue_status(lnx->llc) ==
		DLC_OQUEUE_FULL) {
		datagram_done(pak);
	    } else {
		llc2_send(lnx->llc, pak, pak->datagramsize);
	    }
	    reset_interrupt_level(level);
	}
    } else {
        if (lnx_error_debug)
	    buginf("\nQLLC: Data discarded from qllc, received in %s state",
		   lnx_prstate(lnx->lnx_state) );
        datagram_done(pak);
        /* reset the llc session */
    }
}



/*
 * lnx_vrforward_data
 * Used by LNX local ack sessions to send QLLC data to its llc2 counterpart
 * on the other side of the backbone.  
 */

void lnx_vrforward_data (lnxtype *lnx, paktype *pak)
{

   uchar       sa[IEEEBYTES];
   riftype   	*rifptr;
   int         	riflen;
   int         	dgsize;
   int         	trhdrsize;
   tring_hdr  	*trframe;
   sap_hdr 	*llc1;

   pak->if_input = pak->if_output;

   /* form the src address */
   ieee_copy( &(lnx->vmac_addr[0]), sa);
   sa[0] |= TR_RII;

   /* get the rif string */
   rifptr = rif_lookup(&(lnx->prtnr[0]), NULL, &(sa[0]), 0);
   if (rifptr)
      riflen = rifptr->length;
   else
      riflen = 0;   

   trhdrsize = TRING_ENCAPBYTES + riflen + 4;  /* 4 = llc i-frame hdr size */

   /*
    * The last 4 bytes are needed because vrforward_pak() does kludgy things
    * with the TR CRC bytes.  If left out the packet will be 4 bytes short
    * when it emerges at the other end!!
    */
   dgsize = trhdrsize + pak->datagramsize + TR_FCS_SIZE;

   pak->datagramstart -= trhdrsize; 
   pak->datagramsize   = dgsize;

   /* fill in the TR mac header */
   trframe = (tring_hdr *)pak->datagramstart;
   trframe->ac = AC_PRI0;
   trframe->fc = FC_TYPE_LLC;

   ieee_copy(&(lnx->prtnr[0]), &(trframe->daddr[0]));
   ieee_copy(&(sa[0]), &(trframe->saddr[0]));

   /* fill in the rif */
   bcopy(rifptr->rif, trframe->data, riflen);

   /* fill in the llc2 header */
   llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);
   llc1->dsap = lnx->dsap;
   llc1->ssap = lnx->ssap;

   /* send it!! */
   vrforward_pak(pak, TRUE, RSRB_OP_LLC2DATA, RSRB_RIFLF_LEAVEALONE);

}

 
 
/* Callback routine to close qllc session because llc told us to */
void lnx_cleanup_llc (llctype *llc)
{

    lnxtype  *lnx;

    if (llc == NULL) {
	if (lnx_error_debug)
	    buginf("\nQLLC: lnx_cleanup_qllc:  null llc pointer");
	return;
    }

    lnx = lnx_by_macaddr(&llc->llc2_dte[LMAC_DTE]);

    if (lnx == NULL) {
        if (lnx_error_debug)
	    buginf("\nQLLC: lnx_cleanup_qllc:  lnx not active on llc");
      return;                 
    }

    if (lnx_debug)
	buginf("\nQLLC: removing rif for %e %e",
	       &llc->llc2_dte[RMAC_DTE], &llc->llc2_dte[LMAC_DTE]);
    unset_rif(NULL, 0, &llc->llc2_dte[RMAC_DTE], &llc->llc2_dte[LMAC_DTE], FALSE);

    /*
     * shutting the llc, force the qllc to reset by calling
     * qllc_close.  Set qllc->qllc_llc to NULL to prevent
     * qllc from calling us back.  This is very important to do this
     * NULLing before calling qllc_close.
     *
     * Note it is also important to set lnx_state to DISCONNECT prior to
     * hitting the qllc side.  Since the network side is already down we
     * don't want to kill it again.
     */
    
    lnx->llc = NULL;
    llc->lnx_active = FALSE;
    
    switch (lnx->lnx_state) {
	
    case LNX_DISCONNECT:
	break;				/* nothing to do */

    case LNX_DW_QLLC:
	break;				/* already closing QLLC */

    case LNX_DW_NET:
    case LNX_AW_NET:
    case LNX_AW_QLLC_SEC:
    case LNX_AW_QLLC_PRI:
    case LNX_AW_NET_QLLC_SEC:
    case LNX_NET_CONTACT_PENDING:
    case LNX_CONNECT:
	/*
	 * If we are network disconnect phase then clean up for real.
	 * Or if we are asyncronously disconnecting (ie. llc or the network
	 * is yanking our chain).  Ie we were in a connect phase.
	 * 
	 * Otherwise we are handling the special disconnect case
	 * where we have handled an incoming LLC of somekind, have
	 * processed it, then tore the session back down.  Don't
	 * fuck with the lnx state.
	 */
	lnx_setstate(lnx, LNX_DISCONNECT);

	if (lnx->qllc) {
/*
 * QLLC TREATS close AS AN ATOMIC OPERATION; SDLLC DOESN'T
 * SHOULD DO THOUGH - PHIL 12/6/93
 * compare SDLLC code
 */
	    qllc_close(lnx->qllc);
	}
    }
}



/*
 * lnx_fix_llc
 *
 * 1. Finds matching X.25 address map entry
 * 2. Attaches llc to address map entry and llc to the lnx associated with
 *        address map entry
 *
 * This is the normal way for an llc to get hooked up with an lnx
 *
 */
void lnx_fix_llc (llctype *llc, paktype **ppak)
{
    paktype	*pak = *ppak;
    trrif_t     *trh; 
    lnxtype     *lnx;

    trh = (trrif_t *)pak->datagramstart;

    lnx = lnx_by_macaddr(trh->daddr);
    if (lnx == NULL) {
	if (lnx_error_debug)
	    buginf("\nQLLC: Doing a FIX LLC function when QLLC not defined for address %e",
		   trh->daddr);
	return;
    }
    
    lnx->llc = llc;
    llc->lnx_active = TRUE;
    llc->media_llc_cleanup = lnx_cleanup_llc;
    if (lnx->lnx_state == LNX_AW_NET) {
	/*
	 * Only do a real state transition if this PU is in the
	 * right state for a network connect.
	 *
	 * It is only a real network connect if we are in the right
	 * state.
	 */
	lnx_net_ind_connect(lnx);
    }
}

/*
 * lnx_fix_qllc
 *
 * received a QSM from the X.25 link - it's time to start to
 * build up the lnx data structure,
 *
 * To start we know the lci, qllc.
 * We can find the address map from lci, and lnx from the address map.
 *
 * At this point the llc is not known, we have to send out
 * explorers etc. Eventually there will be  a response from
 * the explorer, we'll reply, and in due course receive a SABME,
 * which will get passed to llc2.  llc2 will call lnx_fix_llc2,
 * to complete the association.
 */

void lnx_fix_qllc (qllctype *qllc, uchar *macaddr)
{
    lnxtype *lnx;

    lnx  = lnx_by_macaddr(macaddr);
    
    if (!lnx) {
	if (lnx_error_debug)
	 buginf("\nQLLC: lnx_fix_qllc: no lnx block");
	return;
    }
    lnx->qllc = qllc;

    if (!lnx->qllc_set_largest)
	lnx->lnx_dlc_maxbtu =  (1 << qllc->lci->lci_pout);

    lnx_setstate (lnx, LNX_DISCONNECT);

    return;
}

/*
 * LACK2 management
 *
 * This includes 
 *	lak_t management for lnx
 *	lnx dlc handler routines
 *	lnx dlc event input preprocessing
 *	...
 */

/*
 * lnx_to_lak_addr
 *
 * convert lnx fields to the ones needed to build a lak address for lookup
 */

static inline void lnx_to_lak_addr(lnxtype *lnx, lak_addr_t *lak_addr)
{
    ieee_copy(lnx->vmac_addr, lak_addr->lak_addr_fields.locaddr);
    ieee_copy(lnx->prtnr,     lak_addr->lak_addr_fields.remaddr);
    lak_addr->lak_addr_fields.locsap = lnx->ssap;
    lak_addr->lak_addr_fields.remsap = lnx->dsap;
}

/*
 * lnx_create_lackie
 *
 * use configuration information and information gather from various
 * packets to build a lackie for lnx
 */

boolean lnx_create_lackie(lnxtype *lnx)
{
    paktype   		*pak;
    tr_ventrytype 	*vre;
    lak_t		*lackie;

    pak = lnx_get_startup_pak(lnx);

    if(!pak) {
	if (lnx_error_debug)
	    buginf("\nQLLC: no pak to create lackie");
	return(FALSE);
    }

    if (!(vre  = (tr_ventrytype *)lnx->lnx_vre) ) {
	if (lnx_error_debug)
	    buginf("\nQLLC: no vre to create lackie");
	vr_nukepak(pak);
	return(FALSE);
    }

    if (!(lackie = lack2_build_reverse_entry(pak, vre, lnx, DLC_QLLC)) ) {
	vr_nukepak(pak);
	return(FALSE);
    }

    return(TRUE);
}

/* LNX LACK2 ACTION ROUTINES */

/*
  * start the downstream open on the QLLC side
 */

static dlc_rtn_t lnx_lak2_open  (dlc_db_t *dlc_db, lak_t *lackie)
{
    lnxtype	*lnx;
    paktype	*pak;
    paktype	*newpak;
    trrif_t	*trh;
    
    if (!dlc_db || !lackie) {
        if (lnx_error_debug)
	 buginf("\nQLLC: lnx_lak2_open, null dlc_db or null lackie");
	return(DLC_RS_FAILED);
    }
    if (dlc_db->type != DLC_QLLC) {
        if (lnx_error_debug)
	 buginf("\nQLLC: lnx_lak2_open called with wrong type of dlc_db");
	return(DLC_RS_FAILED);
    }
    if (!lnx_lak_enable)
	return(DLC_RS_FAILED);

    if (!lackie->piggie_pak) {
        if (lnx_error_debug)
	 buginf("\nQLLC: lnx_lak2_open with no piggie pak");
	return(DLC_RS_FAILED);
    }
    pak = lackie->piggie_pak;

    trh = (trrif_t *)pak->datagramstart;
    if ( !(lnx = lnx_by_macaddr(trh->daddr)) )
	return(DLC_RS_FAILED);

    dlc_db->ptr.lnx = lnx;
    
    if (lnx->lnx_state != LNX_DISCONNECT)
	return(DLC_RS_FAILED);

    if ( !(newpak = pak_duplicate(pak)) )
	return(DLC_RS_FAILED);

    /* queue the pak for the asychronous qllc_open operation */
    if ( !(process_enqueue(lnx_dlc_linkupQ, newpak)) ) {
	datagram_done(newpak);
	return(DLC_RS_FAILED);
    }

    lnx_setstate(lnx, LNX_AW_QLLC_PRI);  /* note changed state */

    /* After missing on every way the routine could fail, it succeeds!
     * Note that, unlike the SDLC DLC OPEN, it must pend till qllc open
     * is complete
     */
    return(DLC_RS_PENDING);
}

static dlc_rtn_t lnx_lak2_close (dlc_db_t *dlc_db, lak_t *lackie)
{
    lnxtype	*lnx;

    if (!dlc_db || !lackie) {
        if (lnx_error_debug)
	 buginf("\nQLLC: lnx_lak2_close, null dlc_db or null lackie");
	return(DLC_RS_FAILED);
    }
    if (dlc_db->type != DLC_QLLC) {
        if (lnx_error_debug)
	 buginf("\nQLLC: lnx_lak2_close called with wrong dlc_db type.");
	return(DLC_RS_FAILED);
    }
    if ( !(lnx  = dlc_db->ptr.lnx) )
	return(DLC_RS_FAILED);

    lnx_net_ind_disconnect(lnx);
    return(DLC_RS_OK);
}

/* enable/disable x.25 input to QLLC/LNX */

static dlc_rtn_t lnx_lak2_busy  (dlc_db_t *dlc_db, boolean busyit)
{
    lnxtype	*lnx;

    if (!dlc_db) {
        if (lnx_error_debug)
	 buginf("\nQLLC: lnx_lak2_busy, null dlc_db");
	return(DLC_RS_FAILED);
    }
    if (dlc_db->type != DLC_QLLC) {
        if (lnx_error_debug)
	 buginf("\nQLLC: lnx_lak2_unbusy called with wrong dlc_db type.");
	return(DLC_RS_FAILED);
    }
    lnx = dlc_db->ptr.lnx;
    if (!lnx)
	return(DLC_RS_FAILED);

    if ( !busyit && lnx->lnx_state == LNX_NET_CONTACT_PENDING) {
	lnx_setstate (lnx, LNX_CONNECT); /* NOT PRETTY, BUT LACK2 FSM ... */
	qllc_contact_accept(lnx->qllc);
	qllc_quench(lnx->qllc, busyit);
    }

    return(DLC_RS_OK);
}

static dlc_rtn_t lnx_lak2_abort (dlc_db_t *dlc_db)
{
    lnxtype	*lnx;

    if (!dlc_db) {
        if (lnx_error_debug)
	 buginf("\nQLLC: lnx_lak2_busy, null dlc_db");
	return(DLC_RS_FAILED);
    }
    if (dlc_db->type != DLC_QLLC) {
        if (lnx_error_debug)
	 buginf("\nQLLC: lnx_lak2_unbusy called with wrong dlc_db type.");
	return(DLC_RS_FAILED);
    }
    lnx = dlc_db->ptr.lnx;
    if (!lnx)
	return(DLC_RS_FAILED);

    lnx_net_ind_disconnect(lnx);
    return(DLC_RS_OK);
}


/* lnx_lak2_dlc_event
 *
 * lnx got some event from the dlc layer
 * wrap it up nicely and pass it in to the LAK2 fsm
 *
 * The lackie (lackey) should be there.
 */

void lnx_lak2_dlc_event(lnxtype *lnx, int minor)
{
    lak_t	*lackie;
    lak_addr_t	lak_addr;
    leveltype	level;

    if (!lnx) {
	if (lnx_error_debug)
	    buginf("\nQLLC: lnx_lak2_event. No lnx");
	return;
    }

    if (qllc_event_debug)
	buginf("\nQLLC: lnx_lak2_dlc_event: %e->%e, event %s",
	       lnx->vmac_addr, lnx->prtnr, lak2_prinput(l2ma_dlc, minor));

    if (!lnx->lnx_lak_enable) {
	if (lnx_error_debug)
	    buginf("\nQLLC: lnx_lak2_dlc_event. No local ack on the lnx");
	return;
    }
    
    lnx_to_lak_addr(lnx, &lak_addr);
    lackie = lack_by_machdr(lak_addr.lak_addr);

    if (!lackie) {
	if (lnx_error_debug)
	    buginf("\nQLLC: %e->%e; lackie missing",
		   lnx->vmac_addr, lnx->prtnr);
	return;
    }
    level = raise_interrupt_level(NETS_DISABLE);
    lak2_fsm(lackie, l2ma_dlc, minor);
    reset_interrupt_level(level);
}


void lnx_setstate(lnxtype *lnx, lnx_state_t newstate)
{
    if ( lnx->lnx_state != newstate) {
        if (qllc_state_debug)
         buginf("\nQLLC: %e %s -> %s", lnx->vmac_addr,
                   lnx_prstate(lnx->lnx_state), lnx_prstate(newstate));
        lnx->lnx_state = newstate;
        if (newstate == LNX_DISCONNECT) {
	    if (lnx_validate(lnx) ) {
                lnx->lnx_xid_rsp_pending    = FALSE;
         	lnx->lnx_test_rsp_pending   = FALSE;
		lnx->lnx_lak_enable         = FALSE;
		/* lnx->time_linkup_sent = 0; */
	    }
        }
    }
}

/*
 * virtualp_qllc - the virtualp vector for qllc.
 * Returns TRUE if QLLC macaddr matches passed in 
 * macaddr for the vre.
 */
static boolean virtualp_qllc (tr_ventrytype *vre, uchar * macaddr)
{
    lnxtype *lnx;

    if ( (vre->type & RSRB_ET_QLLC) && lnx_running ) {
         lnx = lnx_by_macaddr(macaddr);
         if (lnx && (vre == qllc_get_vre(lnx->idb->hwptr)))
             return TRUE;
    }

    return FALSE;
}

/*
 * lnx_add_vring_interface - Add a "lnx virtual ring" to the ring-group. 
 * The lnx virtual ring is the x.25 PDN.  Once added to the ring-group
 * all the remote peers are notified of this new (QLLC) ring.
 * idb - idb of the serial x.25 link 
 * local_ring - x.25 virtual token ring
 * target_ring - ring-group number
 * vmacaddress - virtual MAC address for the X.25 device
 */
static boolean lnx_add_vring_interface 
    (hwidbtype *idb, ushort local_ring, uchar bridge, ushort target_ring)
{
    tr_vgrouptype 	*vrg;
    tr_ventrytype 	*vre;

    vrg = vrfind_ringgroup(target_ring);
    if (!vrg)
      return(FALSE);

    if (qllc_hwidb_get_data(idb, FALSE) == NULL) {
	return(FALSE);
    }

    vre = malloc_vre(NULL);
    if (!vre)
       return(FALSE);

    vre->virtual_idb = 
               lnx_new_vidb(idb, local_ring, LNX_BRIDGE_NUMBER, target_ring);

    if (!vre->virtual_idb) {
       free(vre);
       return(FALSE);
    }

    vre->bn      	= LNX_BRIDGE_NUMBER;
    vre->rn      	= local_ring;
    vre->type		= RSRB_ET_QLLC;
    vre->outhole.idb	= vre->virtual_idb;

    qllc_set_vre(idb, vre);

    vre->virtual_idb->tr_vring_blk = (tr_ventrytype *)&vrg->rings.qhead;
    vre->virtualp = virtualp_qllc;

    enqueue(&vrg->rings, vre);
    vrg->flags |= VRGF_GENUPDATE;
    update_peers();
    return(TRUE);

}

static boolean lnx_active (hwidbtype *idb)
{
    return(qllc_hwidb_test_lnx_active(idb));
}




void lnx_vidb_input 
    (hwidbtype *idb, paktype *pak, ushort opcode, tr_ventrytype *vre)
{

    trrif_t	*trh;		/* pointer to TR MAC header */
    int   	riflen = 0;	/* length of RIF in TR header */
    sap_hdr	*llc1;		/* pointer to llc-1 header */
    lnxtype    	*lnx;
    uchar       *frameptr;

    int         xidoffset;
    boolean     is_response;

    trh = (trrif_t *) pak->datagramstart;
    riflen = (trh->saddr[0] & TR_RII) ? trh->rc_blth & RC_LTHMASK : 0;

    pak->llc2_sapoffset = TRING_ENCAPBYTES + riflen;
    frameptr    = pak->datagramstart + pak->llc2_sapoffset;

    llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);
    xidoffset   = (frameptr +2) - pak->datagramstart;
    is_response = (llc1->ssap & SAP_RESPONSE);

    GET_TIMESTAMP(idb->lastinput);		/* remember time of last input */
    GET_TIMESTAMP(pak->inputtime);		/* save it in packet too       */

    idb->counters.rx_cumbytes += pak->datagramsize;
    idb->counters.inputs++;


    if (!rsrb_opcode_valid(opcode)) {
	buginf("\nQLLC: Bad opcode %04x from %e %e %02x %02x %s (%02x)",
	       opcode, trh->daddr, trh->saddr, llc1->dsap, llc1->ssap,
	       lack_prconch(llc1->control), llc1->control);
	vr_nukepak(pak);
	return;
    }

    /*
     * find which lnx this packet is pointed at.
     */
     
   if ( !(lnx = lnx_by_macaddr(trh->daddr)) ) {
       if (lnx_error_debug)
	   buginf("\nQLLC: lnx_vidb_input - no entry for %e",
		  trh->daddr);
	   vr_nukepak(pak);
	   return;
   }
    llc1 = (sap_hdr *)(pak->datagramstart + TRING_ENCAPBYTES + riflen);
 
    switch (llc1->control & ~LLC1_P) {

       /* packet contains local ack op code */
       case LLC1_UI:
          break;

       case LLC1_XID:
	    rif_update(NULL, 0, trh->saddr, trh->daddr,
                              (srbroute_t *)(riflen ? &trh->rc_blth : NULL),
                              RIF_AGED);
	    /*
	     * just respond to mimic older code.  Should we qualify this?
	     */
	    tr_process_xid(idb, pak, TRUE, FALSE);

	    /*
	     *	If this is an IEEE XID (which substitutes for a Test 
	     *	Poll), then don't forward to lnx_input.  tr_process_xid 
	     *	already responded to the test. 
	     */
	    if ( (pak->datagramsize > (xidoffset + sizeof(xidnulltype))) &&
		 (llc1->data[0] == XID_FMT_STD) ) {
		vr_nukepak(pak);
		return;
	    }
	    break;

       case LLC1_TEST:
            rif_update(NULL, 0, trh->saddr, trh->daddr,
                                 (srbroute_t *)(riflen ? &trh->rc_blth : NULL),
                                 RIF_AGED);

            tr_process_test(idb, pak, TRUE);

          break;

       /* LLC Type 2 drops in here... */
       default:
          break;
    }

    lnx_input(idb, pak, opcode, vre);
}



hwidbtype *lnx_new_vidb (hwidbtype *inidb, 
			 int thisring, int bridgenum, int targetring)
   /* inidb - idb of the serial x.25 link */
{
    hwidbtype 	*idb;
 
    idb = dlc_new_vidb(inidb, "QLLCVirtualRing",
		       thisring, bridgenum, targetring);
    if (idb == NULL)
	return(NULL);

    idb->oqueue            = rsrb_lnx_oqueue;
    idb->iqueue            = (iqueue_t)lnx_vidb_input;
    qllc_hwidb_set_lnx_active(idb, TRUE);

    return(idb);
}



/*
 * Raw enqueue for qllc rings
 */
boolean rsrb_lnx_oqueue (hwidbtype *idb, paktype *pak, enum HEADTAIL dummy)
{

   pak->if_input = idb->firstsw;
   pak->flags   &= ~PAK_LACK_OUT;
   vrforward_pak(pak, TRUE, RSRB_OP_QLLC, RSRB_RIFLF_LEAVEALONE); 
   return(TRUE);

}



/*
 * lnx_remove_vring
 * Destroy a qllc virtual ring associated with an IDB
 */
static boolean lnx_remove_vring (hwidbtype *idb, int rsrb_et)
{
    hwidbtype	  *vidb;
    srb_sb_t      *srb_sb;
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    leveltype     status;

    if (!idb)
	return(FALSE);

    srb_sb = srb_get_sb_inline(idb->firstsw); 
    if (!srb_sb)
	return(FALSE);

    if (!(rsrb_et == RSRB_ET_QLLC && is_x25(idb) && 
	  qllc_hwidb_test_lnx_active(idb)))
	return(FALSE);

    vrg = vrfind_ringgroup(srb_sb->srb_targetring);

    if (vrg) {
       vre = vrfind_rn2entry(srb_sb->srb_thisring, RSRB_BN_DONTCARE,
			     (tr_ventrytype *)&vrg->rings.qhead, RSRB_ET_QLLC);
       if (vre) {
          status = raise_interrupt_level(NETS_DISABLE);
          unqueue(&vrg->rings, vre);
          free(vre);
          reset_interrupt_level(status);
	}
	/* 
	 * this is the case where vre was freed without vre->virtual_idb
 	 * getting freed.	CSCdi12143
	 *
	 * TO MAKE THIS *REALLY* WORK WE NEED TO REMOVE THE llc2 BLOCKS
	 * FROM THE vidb_link, AND TO CLEAR IT IN THE IDB
	 * PHIL 1/24/94
	 */
       vidb = idb->vidb_link;
       if (vidb) {
	   vidb->state = IDBS_DOWN;
	   llc2_cstate(vidb);
	   free (vidb);
	   idb->vidb_link = NULL;
       }
       
	/* make sure this is done only when vrg is not NULL */
	vrg->flags        |= VRGF_GENUPDATE;
    }

    qllc_set_vre(idb, NULL);

    qllc_hwidb_set_lnx_active(idb, FALSE);

    srb_sb->srb_thisring   = SRB_INVALID_RING_NUM;
    srb_sb->srb_bridge_num = SRB_INVALID_BRIDGE_NUM;
    srb_sb->srb_targetring = SRB_INVALID_RING_NUM;

    srb_sb->srb_max_hops    = SRB_DEF_MAXHOPS;
    srb_sb->srb_maxin_hops  = SRB_DEF_MAXHOPS;
    srb_sb->srb_maxout_hops = SRB_DEF_MAXHOPS;

    srb_sb->srb_spanning_explorer  = FALSE;

    return (TRUE);
}


/*
 *  Hash routines        
 */

/*
 * l n x _ b y _ m a c a d d r
 *
 * Find entry in the lnx hash table given the virtual mac address.
 *
 * This routine is used when a packet from the llc2 side needs to go to
 * the x.25 side and the qllc block needs to be found.
 * A packet from the x.25 side going to the tr side picks up the qllc block
 * from the lci structure.  (For now - phil)
 */
lnxtype *lnx_by_macaddr (uchar *macaddr)
{
    int n;
    lnxtype *lnx;
    leveltype level;

    n = lnx_hash_macaddr(macaddr );

    level = raise_interrupt_level(NETS_DISABLE);
    lnx = lnx_hash_table[n];

    while (lnx) {
       if (ieee_equal(lnx->vmac_addr, macaddr)) {
          reset_interrupt_level(level);
          return (lnx);
       }
       lnx = lnx->next_hash;
    }

    reset_interrupt_level(level);
    return (NULL);

}



/*
 * l n x _ e n t e r _ m a c a d d r _ h a s h
 *
 * Enter an lnx into the virtual macaddr hash table.
 */
void lnx_enter_macaddr_hash (lnxtype *lnx)
{

    int n;
    leveltype level;

    n = lnx_hash_macaddr( &(lnx->vmac_addr[0]) );

    level = raise_interrupt_level(NETS_DISABLE);
    lnx->next_hash	= lnx_hash_table[n];
    lnx_hash_table[n]	= lnx;
    reset_interrupt_level(level);

}



/*
 * lnx_remove_macaddr_hash
 * Remove an entry from the virtual macaddr hash table.
 */
void lnx_remove_macaddr_hash (lnxtype *target)
{

    int n;
    lnxtype *lnx, **prev;
    leveltype level;
    
    n = lnx_hash_macaddr( &(target->vmac_addr[0]) );
    level = raise_interrupt_level(NETS_DISABLE);
    prev = &lnx_hash_table[n];
    lnx = *prev;

    while (lnx) {
	if (lnx == target) {
	    *prev  = lnx->next_hash;
	    lnx->next_hash = NULL;
	    reset_interrupt_level(level);
	    return;
	}
	prev = &lnx->next_hash;
	lnx = lnx->next_hash;
    }

    reset_interrupt_level(level);
    if (qllc_event_debug)
	buginf("\nQLLC: lnx_remove_macaddr_hash: lnx 0x%x not found", target);

}



/*
 * validate_lnx
 * Verify that an lnx exists
 */
boolean lnx_validate (lnxtype *target)
{

    int n;
    lnxtype *lnx;
    leveltype level;

    n = lnx_hash_macaddr( &(target->vmac_addr[0]) );
    level = raise_interrupt_level(NETS_DISABLE);
    lnx = lnx_hash_table[n];

    while (lnx) {
	if (lnx == target) {
           reset_interrupt_level(level);
	   return (TRUE);
	}
	lnx = lnx->next_hash;
    }

    reset_interrupt_level(level);
    return (FALSE);

}



/*
 * lnx_hash_macaddr
 *
 * Generate hash table index for a macaddr
 */
int lnx_hash_macaddr (uchar *macaddr)
{

    register int i, n = 0;

    for (i = 0; i < IEEEBYTES; i++)
	n += macaddr[i];

    n &= LNX_HASH_MASK;

    return (n);

}

/*
 * lnxif_command 
 * Configure LNX (media-translation) interface parameters.  All LNX 
 * configuration commands are interface sub-commands.  They are configured
 * for the serial line connected to the X.25 station which needs to
 * communicate with its token ring partner. 
 */
void lnxif_command(parseinfo *csb)
{
    hwidbtype	*hwptr;
 
    int n;
    lnxtype *lnx;
    idbtype *idb = csb->interface;
    hwptr = csb->interface->hwptr;

    if (csb->nvgen) {
	/* not parsing - just writing config parameters */   

	if (csb->which == QLLC_CFG_DLSW) {
	    QllcNvWriteDlsw(csb);
	    return;
	}

        for (n = 0; n < LNX_HASH_SIZE; n++)
	   for (lnx = lnx_hash_table[n]; lnx; lnx = lnx->next_hash) {
                if (lnx->idb == idb) {
                   if ( is_x25(hwptr) && qllc_idb_test_lnx_active(idb)) {
	                switch (csb->which) {

	                    case LNX_SRB:
                            {
                              srb_triplet_t srb_triplet;
                              srb_sb_get_triplet(lnx->idb, &srb_triplet);
                              nv_write(lnx->this_ring, 
				       "qllc srb     %e %d %d",
			       		lnx->vmac_addr,
					srb_triplet.localRing,
					srb_triplet.remoteRing);
                                  break;
                            }
	                    case LNX_XID:
                
                               nv_write(lnx->xid_valid,
					"qllc xid     %e %02x%02x%02x%02x",
					lnx->vmac_addr,
                                        lnx->xid[0], lnx->xid[1],
                                        lnx->xid[2], lnx->xid[3]);
                           	break;
	                    case LNX_PRTNR:
                
                               nv_write(lnx->configured_prtnr && 
					lnx->this_ring,
					"qllc partner %e %e",
					lnx->vmac_addr,
                                        lnx->prtnr);
		           	break;
	                    case LNX_SAP:
                
                               nv_write(((lnx->ssap != SAP_IBM_SNA || 
					  lnx->dsap != SAP_IBM_SNA) &&
					 lnx->this_ring) ,
  				        "qllc sap %e %02x %02x",
					lnx->vmac_addr,
                                        lnx->ssap, lnx->dsap);
                   		break;

	                    case LNX_QLLCLF:
                
                               nv_write(lnx->qllc_set_largest && 
					lnx->this_ring, 
					"qllc largest-packet %e %d",
					lnx->vmac_addr,
                                        lnx->lnx_dlc_maxbtu);
                               break;

			   case LNX_POLL: 
			       nv_write(lnx->xid_poll,  
					"qllc npsi-poll %e", lnx->vmac_addr);
			       break;

			   case LNX_PASS:
               
                               nv_write(lnx->passwd_valid &&
					lnx->this_ring,
					"qllc passwd     %e %s",
                                        lnx->vmac_addr,
                                        lnx->x25_passwd);
		 	       break;
	                    default:
		              bad_parser_subcommand(csb, csb->which);
                              break;
		       }
		    }
	    }
	}
	return;
    }					/* end of nvgen */

    if (!bridge_enable) {
       printf("\n%%Source Route Bridging not supported in this release");
       return;
    }


    if ( !is_x25(csb->interface->hwptr) ) {
       printf("\n%%qllc configuration commands only allowed on serial lines");
       printf("\nconfigured for X.25");
       return;
    }

    switch (csb->which) {
 
      case LNX_SRB:
        lnx_config_srb(csb, csb->interface);
        break;

      case LNX_PRTNR:
	lnx_config_partner(csb, csb->interface);
	break;

      case LNX_XID:
        lnx_config_xid(csb, csb->interface);
        break;

      case LNX_PASS:
        snx_config_pass(csb, csb->interface);
        break;
 
      case LNX_QLLCLF:
	lnx_config_qllclf(csb,  csb->interface);
	break;
	
      case LNX_SAP:
        lnx_config_sap(csb, csb->interface);
        break;

      case LNX_POLL:
 	lnx_config_poll(csb, csb->interface);
        break;
 
    case QLLC_CFG_DLSW:
	QllcConfigDlsw(csb, csb->interface);
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

}

/*
 * lnx_addr_lookup
 *
 * configuration routine to find the lnx block from the virtual
 * Token Ring address.
 */

static lnxtype *lnx_addr_lookup(parseinfo *csb, idbtype *idb)

{
    uchar macaddr[IEEEBYTES];
    lnxtype *lnx;

    memset(&macaddr, 0, sizeof(macaddr)); /* clean up before lookup */
    ieee_copy((char *)GETOBJ(hwaddr,1)->addr, macaddr);
    if (macaddr[0] & 0x80) {
	printf("\n%%Malformed hex mac address - group bit set");
	return(NULL);
    }
    
    if ( (lnx = lnx_by_macaddr(macaddr)) == NULL) {
	printf("\n%%QLLC: Virtual MAC address %e not found", &macaddr);	  
	printf("\nConfigure qllc with x.25 map command before issuing");
	printf("\nthis command");
	return (lnx);
    }

    if ( lnx->idb != idb) {
	printf("\n%%Virtual MAC address %e is not defined for the interface %s",
		macaddr, idb->namestring);
	return (NULL);
    }    

    return (lnx);

}
 


/*
 * lnx_config_srb
 *
 * Configure the x.25 network as a virtual token ring network.  This is
 * needed to make the x.25 device appear to its token ring partner as a
 * token ring device.  To accomplish this the serial device is given a
 * virtual token ring address called the lnx virtual tr address and the
 * x.25 network is made to appear as a virtual token ring by assigning a 
 * virtual ring number.  Both the lnx virtual tr address and the lnx virtual 
 * ring number need to be unique across the network.
 * Need one "qllc srb" command for each x.25 qllc copnnection.
 */
static void lnx_config_srb (parseinfo *csb, idbtype *idb)
{
    uchar	vmac[IEEEBYTES];
    hwidbtype 	*hwptr = idb->hwptr;
    srb_sb_t    *srb_sb;
    ushort lnx_vr, target_ring;
    lnxtype *lnx;
    
    ieee_copy(GETOBJ(hwaddr,1)->addr, vmac);
    if (csb->sense) {
  	lnx = lnxtype_init(vmac);
  	if (lnx == NULL) {
  	    printf("%%QLLC: Couldn't create a data structure for %e", vmac);
  	    return;
  	}     
    } else {
 	lnx = lnx_by_macaddr(vmac);
 	if (lnx == NULL) {
 	    printf("%%QLLC: No QLLC connection to %e", vmac);
 	    return; 
	}
    }

    /*
     * get the srb parameters (qllc virtual ring, target ring)
     */
    lnx_vr	= (ushort) GETOBJ(int,1);
    target_ring	= (ushort) GETOBJ(int,2);

    /* sanity check */
    if (target_ring == lnx_vr) {
       printf("\n%%local-ring and target-ring numbers must be different");
       return;
    }

    if ( vrfind_ringgroup(lnx_vr) ) {
       printf("\n%%lnx virtual ring cannot be a part of a ring group");
       return;
    }

    if ( !vrfind_ringgroup(target_ring) ) {
       printf("\n%%target ring %d must be the same as a ring-group",
	      target_ring);
       printf("\nas configured by the source-bridge ring-group command");
       return;
    }
    /* If we're adding to the configuration, we have to build a qllc v ring
     * for exactly the first qllc srb entry on this interface
     *
     * Subseqent qllc srb commands merely have to be sure that they are
     * correctly configured, i.e. lnx_vr ,and targetring match values in
     * the idb
     *
     * When we're removing qllc srb definitions, the last one to go
     * removes the virtual ring
     */

    srb_sb = srb_get_or_create_sb_inline(idb);
    if (srb_sb == NULL) {
	free(lnx);
	printf(nomemory);
	return;
    }

    if (csb->sense) {
        /* find out whether  X.25 link is already configured for qllc-llc2 */
        if ((srb_sb->srb_thisring == 0) && 
            (srb_sb->srb_targetring == 0)) {
            /* it's new! - add the virtual ring */
            srb_sb->srb_thisring   = lnx_vr;
            srb_sb->srb_bridge_num = LNX_BRIDGE_NUMBER;
            srb_sb->srb_targetring = (ushort) target_ring;
            hwptr->tr_transit_oui = DEFAULT_OUI;  /* for qllc-llc and sdlc-llc */


            lnx_add_vring_interface(hwptr, lnx_vr, LNX_BRIDGE_NUMBER, target_ring);

	    qllc_hwidb_set_lnx_active(hwptr, TRUE);
            lnx_start();
            llc2_start();

        } else  if ((srb_sb->srb_thisring != lnx_vr) ||
		    (srb_sb->srb_targetring != target_ring)) {
	    
            printf("\nInterface is already configured for source ring %d, targe\
t ring %d",
                   srb_sb->srb_thisring,
                   srb_sb->srb_targetring);
            printf("\n       Configuration parameter rejected");
            return;
        }
	lnx->this_ring = lnx_vr;
	lnx->target_ring = target_ring;
	lnx_pvc_setup(lnx, csb->sense);
    } else {				/* remove lnx connection */
        qllc_detach(lnx->qllc);
	lnx->this_ring = 0;
        lnx_disconnect_net(lnx);
	lnx_remove_macaddr_hash(lnx);
	lnx_unmap_qllc(vmac);
        if (!lnx_vr_other_lnx (lnx)) {
            lnx_remove_vring(hwptr, RSRB_ET_QLLC);
	    	/* no one's using it any more */
        }
	lnx_pvc_setup(lnx, csb->sense);
	lnx_freeup(lnx);
	return;
    }

    llc2_register(SAP_IBM_SNA, LINK_LLC2, lak_lnx_register);
    start_srb_background();
}

/*
 * lnx_vr_other_lnx
 *
 * walk the lnx hash table to find out if any other lnx is using the
 * virtual ring, for the given X.25 serial interface
 */
 
static boolean lnx_vr_other_lnx(lnxtype *thislnx)
{
    int		n;
    lnxtype	*lnx;
    idbtype	*thisidb;
    leveltype	level;
    
    thisidb = thislnx->idb;
    
    level = raise_interrupt_level(NETS_DISABLE);
    for (n = 0; n < LNX_HASH_SIZE; n++)
	for (lnx = lnx_hash_table[n]; lnx; lnx = lnx->next_hash) {
	    if (lnx == thislnx)
	    	continue;
	    if (lnx->idb == thisidb && lnx->this_ring != 0) {
		reset_interrupt_level(level);
	    	return (TRUE);
	    }
    	}
    reset_interrupt_level(level);
    return (FALSE);
}

/*
 * lnx_config_partner
 *
 * Configure the TR address of the partner it intends to talk to.  This is
 * needed for device initiated connections as the Cisco box will generate
 * an explorer packet on behalf of the x.25/qllc device to find the route
 * to its partner.
 */

static void lnx_config_partner (parseinfo *csb, idbtype *idb)
{
    lnxtype *lnx;
    uchar   newmac[IEEEBYTES];

    if ( !(lnx = lnx_addr_lookup(csb, idb)) ) {
	return;
    }
    if (lnx->this_ring == 0) {
	printf("\n%%QLLC Source Route Bridging not configured for this interface.");
	printf("\n      Use QLLC SRB command first");
	return;
    }

   if (!csb->sense) {
      ieee_zero(lnx->prtnr);
      lnx->configured_prtnr = FALSE;
      lnx->actual_prtnr = FALSE;
      return;
  }

    ieee_copy(GETOBJ(hwaddr,2)->addr, newmac);
    if (newmac[0] & 0x80) {
	printf("\n%%Broadcast partner address not allowed");
	return;
    }

    ieee_copy(newmac, lnx->prtnr);
    lnx->configured_prtnr = TRUE;
    lnx->actual_prtnr = TRUE;
}


/* configure the XID if the router is acting as a proxy for 
 * a IBM PU2.0 device.
 * With this option set the router generates an XID Fmt 0 Type 2
 * to send to the LAN-attached FEP
 */

static void lnx_config_xid (parseinfo *csb, idbtype *idb)
{
    lnxtype *lnx;
    char *buffs;

    if ( !(lnx = lnx_addr_lookup(csb, idb)) ) {
	return;
    }

    if (!csb->sense) {
	lnx->xid_valid = FALSE;
	memset(lnx->xid, 0, LNX_XID_SIZE);
	return;
    }

    buffs = (char *) &GETOBJ(string,1)[0];
    if (!valid_XID(buffs, lnx->xid, LNX_XID_SIZE) ) {
	return;
    }

    lnx->xid_valid = TRUE;

    return;

}

/*
 * lnx_config_qllclf
 *
 * Configure largest packet payload on the X.25 interface
 * This is used to fragment any mismatch in size between the LLC2
 * side and the X.25 side
*/

static void lnx_config_qllclf(parseinfo *csb, idbtype *idb)
{
    lnxtype *lnx;
    int		largest;

    if ( !(lnx = lnx_addr_lookup(csb, idb)) ) {
	return;
    }
    if (lnx->this_ring == 0) {
	printf("\n%%QLLC Source Route Bridging not configured for this interface.");
	printf("\n      Use QLLC SRB command first");
	return;
    }

    if (!csb->sense) {
	lnx->qllc_set_largest = FALSE;
	lnx->lnx_dlc_maxbtu = 0;
	return;
    }
    
    largest = GETOBJ(int, 1);
    
    switch(largest) {
      case 16:
      case 32:
      case 64:
      case 128:
      case 256:
      case 512:
      case 1024:
	lnx->lnx_dlc_maxbtu = largest;
        lnx->qllc_set_largest = TRUE;
	break;
	
      default:
	printf("\nQLLC: largest packet must be a power of two between 16 and 1024");
	break;
    }
    return;
}


/*
 * lnx_config_poll
 *
 * Configure poll option to support PU2.0 on LAN talking to FEP
 * It's illegal for a PU2.0 to send a NULL XID to a FEP over X.25
 * Must be XID 0T2
 */

static void lnx_config_poll(parseinfo *csb, idbtype *idb)
{
    lnxtype *lnx;

    if ( !(lnx = lnx_addr_lookup(csb, idb)) ) {
	return;
    }
    if (lnx->this_ring == 0) {
	printf("\n%%QLLC Source Route Bridging not configured for this interface.");
	printf("\n      Use QLLC SRB command first");
	return;
    }

    lnx->xid_poll = csb->sense;
    return;
}


static void lnx_config_sap (parseinfo *csb, idbtype *idb)
{
   lnxtype *lnx;
   uchar ssap, dsap;

   if ( !(lnx = lnx_addr_lookup(csb, idb)) ) {
       return;
   }
   if (lnx->this_ring == 0) {
       printf("\n%%QLLC Source Route Bridging not configured for this interface.");
       printf("\n       Use QLLC SRB command first");
   }

   /* If the no prefix is used then replace
      current sap values with the default 
      values
    */

   if (!csb->sense) {
        lnx->dsap  = SAP_IBM_SNA;
        lnx->ssap  = SAP_IBM_SNA;
	return;
   }

   ssap = GETOBJ(int,1);
   dsap = GETOBJ(int,2);

   if (ssap & SAP_RESPONSE) {
       printf("\n%%Illegal source sap. Response bit set");
       return;
   }
   
   if (dsap & SAP_RESPONSE) {
       printf("\n%%Illegal destination sap. Response bit set");
       return;
   }
   
   lnx->ssap = ssap;
   lnx->dsap = dsap;
   return;
}
   


void lak_lnx_register (llctype *cookie)
{

}


/*
 * l n x _ p r _ e v e n t
 *
 * render an event fit for humans to read
 */
 
char *lnx_pr_event(lnx_net_event_t event)
{
    switch(event) {
    default:
        return ("bogus");
 
    case LNX_INVALID:
        return ("Invalid");
 
    case LNX_CONN_REQ:
        return ("SABME");
 
    case LNX_DISC_REQ:
        return ("DISC");
 
    case LNX_UA:
        return ( "LLC2 UA" );
 
    case LNX_QUENCH_ON:
        return ( "RNR" );
 
    case LNX_QUENCH_OFF:
        return ( "RR" );
 
    case LNX_DATA:
        return ( "I/S Frame ");
    }
}



/*
 * l n x _ p r s t a t e
 *
 * Return a string describing the current state of an
 * lnx component or for a state value.
 *
 * input:       state   return the state string for this state value.
 * output:      char *  pointer to a string that is the state.
 */

char *lnx_prstate (lnx_state_t state)
{
    switch (state) {
      case LNX_DISCONNECT:    return("DISCONNECT");
      case LNX_DW_NET:        return("NET DISC WAIT");
      case LNX_DW_QLLC:       return("QLLC DISC WAIT");
      case LNX_AW_QLLC_PRI:   return("QLLC PRI WAIT");
      case LNX_NET_CONTACT_PENDING: return("NET CONTACT REPLY WAIT");
      case LNX_AW_NET_QLLC_SEC: return("NET LLC2_OPEN WAIT");
      case LNX_AW_NET:        return("NET UP WAIT");
      case LNX_AW_QLLC_SEC:   return("QLLC SEC WAIT");
      case LNX_CONNECT:       return("CONNECT");
      default:                return("qllc_bogus");
    }
}

/*
 * snx_start()
 *
 * It starts a timer that looks at the condition of the QLLC session and will
 * try to establish the underlying link between the two sidea, i.e, SDLC and
 * QLLC. Currnetly this check will be done every 10secs.
 * The current implemention is timer based, the hooks are provide to start
 * things up as a process rather than a timer base routine.
 * It calls the qllc_open to take care of the qllc start up work.
 */
static void snx_start (void)
{
    timer_t	*snx_tmr;
 
    if (snx_running)
	return;

    snx_tmr = create_timer(snx_periodic, 0,0,0, /* fn ptr + three args */
			  "SDLC_QLLC periodic", /* timer name */
			  TMT_CONTINUOUS /* continuous timer (bistable;) */);

    set_timer(snx_tmr, ONESEC * 10);
    qllc_start();
}


/*
 * snx_periodic()
 *
 * It will get called periodically to take care of any connection 
 * establishment needs. It will do this by calling snx_upstream.
 * Once every 10 secs.
 */
void snx_periodic (void)
{
 hwidbtype *idb;

/* Is there a better way to find the SNX, than to walk all the idbs
 */
 FOR_ALL_HWIDBS(idb)
  {
   if (idb->state == IDBS_UP && idb->snx_active)
    {
     snx_upstream(idb);
    }
  }
}


/*
 * snx_upstream()
 *
 * Will try to establish a connection with the Host using X.25/QLLC protocol.
 * Goes through the list of active SDLC connections and establishes connections
 * to the Host using x.25 for each SDLC connection as appropriate.
 */
void snx_upstream(hwidbtype *sdlc_idb)
{
 int addr;
 sdlc_data_block *sdb;
 qllctype *qllc;

 lnxtype  *lnx;
 uchar *pwp;
 

  for (addr = SDLC_MIN_ADDRESS; (addr <= SDLC_MAX_ADDRESS); addr++) 
   {
    sdb = (sdlc_data_block *) sdlc_idb->sdlc_data[addr];

    if(!sdb || !sdb->qllc_have_prtnr)
     continue;

    if (sdb->snx_state != SNX_CONNECT) {
	if (sdlc_qllc_debug)
	    buginf("\nSNX_Upstream: %s - %02x ~ %e State %s",
		   sdlc_idb->hw_namestring, addr,  sdb->qllc_prtnr,
		   snx_prstate(sdb->snx_state));
    }

    if ( sdb->snx_state != SNX_AW_QLLC)
     continue;

    if( !(lnx = lnx_by_macaddr( (uchar *)sdb->qllc_prtnr)) ) {
	if (lnx_error_debug) {
	    buginf("\nSNX: qllc not configured for sdlc partner %e",
		   sdb->qllc_prtnr);
	}
     continue;
    }

    if(lnx->passwd_valid)
     pwp = lnx->x25_passwd;
    else
     pwp = NULL;

    /* X.25 upstream start */
    qllc = qllc_open_upstream_snx(sdb->qllc_prtnr, lnx->idb, FALSE, sdb, pwp);

    if (!qllc)
     {
      if (lnx_error_debug)
       buginf("\nQLLC: qllc_open fails");
      return;
     }
   else
    {
     lnx->sdlcp  = sdb;
 
     snx_set_state(sdb, SNX_CONNECT);
     sdb->qllcp          = qllc;
     sdb->lnxp           = lnx;
     lnx_setstate(lnx, LNX_CONNECT);
     lnx->qllc          = qllc;

     if (!lnx->qllc_set_largest)
      lnx->lnx_dlc_maxbtu =  (1 << qllc->lci->lci_pout);

     qllc->sdlcp         = sdb;
     qllc->lnx           = lnx;
     qllc->idb           = lnx->idb;
     qllc->sdlc_idb      = sdlc_idb;
   }
 
  }

}

/*
 * snx_output()
 *
 * Take it from SDLC and give it to QLLC .
 * Called from interrupt level in sdlc_input so there is no need to mask
 * interrupts in this routine.
 * sdlc_input will nuke the packet! Don't do it here!!
 */
void snx_output(hwidbtype *idb, paktype *pak)
{
 sdlc_frametype *frame;
 sdlc_data_block *sdb;

 lnxtype *lnx = NULL;

  if (!idb || !pak)
   return;

  frame = (sdlc_frametype *)pak->datagramstart;
  sdb = (sdlc_data_block *)idb->sdlc_data[frame->address];
  if (!sdb)
   return;

  lnx = sdb->lnxp;
  if (!lnx)
   return;

  if (SDLC_ISIFRAME(frame)) 
   {
    if (sdb->snx_state == SNX_CONNECT)
     {
      pak->datagramstart += SDLCHEADERBYTES;
      pak->datagramsize  -= SDLCHEADERBYTES;
      if (sdb->qllcp)
       {
        QllcSendThruQ(lnx, pak); 
       }
      else
       datagram_done(pak);

      return;
     }
    else 
     {
      if (lnx_error_debug)
	buginf("\nQLLC: Iframe ignored in snx_output.\n");
      return;
     }
   }
}


/*
 * snx_input()
 *
 * Take it from QLLC and give it to SDLC.
 *
 */
void snx_input(paktype *pak, qllctype *qllc)
{
 lnxtype *lnx;

  lnx = qllc->lnx;

  if (lnx->lnx_state == LNX_CONNECT) 
   {
    clear_if_input(pak);	/* packet no longer charged to X.25 input */

    pak->lnx_cb = lnx;          /* ensure lnx is set to send it to sdlc */
    pak->if_output = qllc->sdlc_idb->firstsw;
 
    SdlcSendThruQ((sdlc_data_block *)qllc->sdlcp, pak);

   }
  else
   {
    if (lnx_error_debug)
     buginf("\nQLLC: Iframe ignored in snx_input.\n");

    datagram_done(pak);
   }

}

/*
 * snx_clean_sdlc()
 *
 * Will start the cleanup on the sdlc side of the things.
 */
void snx_clean_sdlc(qllctype *qllc)
{
 lnxtype *lnx;
 sdlc_data_block *sdb; 

 lnx = (lnxtype *)qllc->lnx;
 sdb = (sdlc_data_block *) lnx->sdlcp ;
 
 if (sdlc_qllc_debug && sdb != NULL) {
     buginf("%s - %02x snx_state %s", 
	    sdb->sdlc_idb->hw_namestring, sdb->sdlc_address, 
	    snx_prstate(sdb->snx_state));
 } 
 if(lnx != NULL)
  {
   if (sdb->snx_state == SNX_CONNECT) /* SNX_AW_SDLC if other side initiated */
    {
     /* clean the sdlc side. */
     snx_disconnect_sdlc (lnx);
    }
   else if (sdb->snx_state < SNX_DW_SDLC)
    {
	if (sdlc_qllc_debug) {
	    buginf("\n      no action - should end up at DISCONNECT");
	}
     /*
      * DW_SDLC isn't convered by this section.  When in DW_SDLC
      * there is something actually happening.  Leave it alone.
      *
      * Other states, DISCONNECT, DW_QLLC, just
      * reset back to DISCONNECT.
      */

      /* leave it alone, if SDLC is in disconnet and we are not connected */
      
    }
  }

}

/*
 * snx_disconnect_sdlc()
 *
 * Start the SDLC side by sending the DISC out.
 * Wait for the UA to arrive then finish the
 * the cleanup in the SDLC side.
 *
 */
void snx_disconnect_sdlc(lnxtype *lnx)
{
 qllctype *qllc;
 sdlc_data_block *sdb; 

    if (!lnx) {
    if (lnx_error_debug)
     buginf("\nQLLC: snx_disconnect_sdlc invalid lnx");
    return;
   }

  sdb = (sdlc_data_block *) lnx->sdlcp;
 
  qllc = lnx->qllc;

    if (sdb) {
 	if (sdlc_qllc_debug) {
 	    buginf("\nSNX_DISCONNECT_SDLC %s - %02x",
 		   sdb->sdlc_idb->hw_namestring, 
 		   sdb->sdlc_address);
 	}
    /*
    * qllc side is going down.  If there is an SDLC open close it.
    */
	snx_set_state(sdb, SNX_DW_SDLC);
     sdlc_setstate(qllc->sdlc_idb,SDLC_DISCSENT,sdb);
     sdlc_set_pause_timer(qllc->sdlc_idb, sdb);
    } else {
	snx_set_state(sdb, SNX_DISCONNECT);
   }
}


/*
 * sdlc_clean_snx()
 *
 * This routine is called from the sdlc side to clean up the QLLC/snx side.
 */
static void sdlc_clean_snx (sdlc_data_block *sdb)
{

 lnxtype *lnx;

  lnx = sdb->lnxp;
 
  if (!sdb)
   {
    if (lnx_error_debug)
     buginf("\nQLLC: snx_cleanup_sdlc:  null sdlc pointer");
    return;
   }

  /*
   * reset the qllc side by calling qllc_close.
   * qllc_close.  Set qllc->sdlcp to NULL to prevent
   * qllc from calling us back.  This is very important to do this
   * NULLing before calling qllc_close.
   */
 
 if (qllc_event_debug) {
     buginf("\nSDLC_CLEAN_SNX - %s - $02x", 
	    sdb->sdlc_idb->hw_namestring, sdb->sdlc_address);
 }
   lnx->sdlcp = NULL;
   /* per interface basis, forget it */
   /* lnx->lnx_active = FALSE; */

   if (lnx->qllc)
    {
     lnx->qllc->sdlcp = NULL;

     qllc_close(lnx->qllc);
     lnx->qllc->lnx = NULL;
     lnx->qllc = NULL;
    }
}

/*
 * snx_config_pass()
 *
 * In order to support the password field of the IBM X25 software.
 * This field will be set to the same value as configured by IBM
 * configuration. Not using this command results in no password.
 */
void snx_config_pass (parseinfo *csb, idbtype *idb)
{
 lnxtype *lnx;
 char pw[8];

  if ( !(lnx = lnx_addr_lookup(csb, idb)) )
   {
    return;
   }
   
  if (!csb->sense)
   {
    lnx->passwd_valid = FALSE;
    memset(lnx->x25_passwd, 0, SNX_X25_PASS_SIZE);
    return;
   }

  ieee_copy(GETOBJ(string,1), pw);
  ieee_copy(pw, lnx->x25_passwd);

  lnx->passwd_valid = TRUE;
  return;

}

/*
 * lnx_upstream()
 *
 * Will try to establish a connection with the Host using X.25/QLLC protocol.
 * We have and indication from the TEST frame that some system is alive 
 * down stream.
 *
 */
void lnx_upstream(lnxtype *lnx)
{
 qllctype *qllc;
 uchar *pwp;

  if(lnx->lnx_state != LNX_AW_QLLC_SEC)
   return;
   if(lnx->passwd_valid)
    pwp = lnx->x25_passwd;
   else
    pwp = NULL;

   /* X.25 upstream start */
   qllc = qllc_open_upstream_lnx(lnx->vmac_addr, lnx->idb, FALSE, pwp);
    if (!qllc) {
     lnx_setstate(lnx, LNX_DISCONNECT);
     if (lnx_error_debug)
      buginf("\nQLLC: qllc_open fails");
    } else {
      lnx->qllc          = qllc;
  
      if (!lnx->lnx_dlc_maxbtu)
       lnx->lnx_dlc_maxbtu =  (1 << qllc->lci->lci_pout);
  
      qllc->lnx           = lnx;
      qllc->idb           = lnx->idb;
     }
}

/*
 * p r _ x i d
 *
 * show the XID
 *
 * parameters:
 * pak		packet that contains an xid
 * xidoffset	offset from pak->datagramstart
 *
 */
void pr_xid( paktype *pak, int xidoffset )
{
    XID3type	*xid = (XID3type *)(pak->datagramstart + xidoffset);
    int		length = pak->datagramsize - xidoffset;
    uchar	*xidblk_num = &xid->xid3_idblk_idnum[0];

    buginf(" %d bytes", length);

    if (length >= offsetof(XID3type, xid3_fill1)) {
	buginf(" Fmt %dT%d: %02x%02x%02x%02x", (xid->xid3_fmt >> 4) & 0x0F,
	   (xid->xid3_fmt & 0x0F),
	    xidblk_num[0],  xidblk_num[1],  xidblk_num[2],  xidblk_num[3]);
    }
    return;
}
/*
 * show_lnx
 *
 * display the lnx connections
 */

void show_lnx(void)
{
    int		n;
    lnxtype	*lnx;

    automore_enable(NULL);
    for (n = 0; n < LNX_HASH_SIZE; n++) {
	for ( lnx = lnx_hash_table[n]; lnx; lnx = lnx->next_hash) {
	    show_lnxtype(lnx);
	}
    }
    QllcShowCeps();
    automore_disable();
}

static void show_lnxtype(lnxtype *lnx)
{
    qllctype	*qllc;
    idbtype	*idb = lnx->idb;
    lcitype	*lci;
    srb_triplet_t srb_triplet;

    srb_sb_get_triplet(idb, &srb_triplet);
    printf("\n%s: %e->%e. SAPs %x %x. Rings Src %d, Tgt %d.",
	   idb->namestring, &lnx->vmac_addr[0], &lnx->prtnr[0],
	   lnx->ssap, lnx->dsap,
           srb_triplet.localRing,
           srb_triplet.remoteRing);

    printf("\n  State %s ", lnx_prstate(lnx->lnx_state) );

    /* Setup pointers and lock them */
    if (!lnx->qllc) return;
    qllc = lnx->qllc;
    mem_lock(qllc);

    lci  = qllc->lci;
    if (lci)
        mem_lock(lci);

    if (lci)
	printf("\n  Remote DTE %s. QLLC Protocol State %s lci %d (%s)",
	       lci->lci_map->map_address[X25_MAP_PRIMARY_ADDR_INX].
			addr.x121_addr, qllc_prstate(qllc->qllc_state),
	       lci->lci_lci, qllc->qllc_ispvc ? "PVC": "SVC");
    if (qllc->qllc_holdQ.count)
	printf("\n  %d packets held ", qllc->qllc_holdQ.count);

    if (lnx->lnx_state == LNX_CONNECT && qllc->qllc_state == QST_NORMAL &&
	qllc->qllc_x25_busy)
	printf("X.25 busy");

    /* Call free to decrement the refcount */
    if (lci)
        free(lci);
    free(qllc);
}


/* Send data to qllc via lnx_dataQ. This routine can be called by
 * the background process to check if any data to be sent, or it
 * can be called by the interrupt.
 *
 * to protect against interrupt process enter this routine, we
 * set this flag so we dont have to worry about interrupts
 */

static boolean sentBusy = FALSE;;

static boolean QllcSend (void)

{
   paktype *pak;
   lnxtype* lnx;
   sentBusy = TRUE;    /* stop interrupt to enter */

   while ((pak =  process_dequeue(lnx_dataQ)) != NULL){

      lnx = pak->lnx_cb;
      pak->if_output = lnx->idb;

      if (lnx->qllc == NULL || lnx->qllc->lci == NULL){
          datagram_done(pak);  
	  if (lnx_error_debug) {
	      buginf("\nqllc disconnected");
	  }
          continue;           /* discard packet */
      }
      if ( pak->datagramsize <= lnx->lnx_dlc_maxbtu  ) {
          qllc_send(lnx->qllc, pak);
     }  else {
         /* No. We have to do some messy fragmentation */
         queuetype q;
         if (sna_th_frag(pak, lnx->lnx_dlc_maxbtu, &q)){
             sendqueue (&q, qllc_send, lnx->qllc);
         }
         else{
            process_requeue(lnx_dataQ, pak);   /* put the queue back */
            sentBusy = FALSE;    /* let interrupt routine to enter */
            return FALSE;
	 }
     }
  }
  sentBusy = FALSE;
  return TRUE;
}



static void QllcSendThruQ (lnxtype* lnx, paktype *pak)
{
   pak->lnx_cb = lnx;         /* ensure lnx is defined */
   process_enqueue(lnx_dataQ, pak);
   if (!sentBusy)
       QllcSend();
}

forktype lnx_linkup_process (void)
{
    boolean have_event;
    ulong major, minor;

    process_wait_on_system_init();
    process_watch_queue(lnx_dlc_linkupQ, ENABLE, RECURRING);
    process_watch_queue(lnx_net_linkupQ, ENABLE, RECURRING);
    
    for(;;) {
        process_wait_for_event();
        while ((have_event = process_get_wakeup(&major, &minor)) == TRUE) {
            switch (major) {
            case QUEUE_EVENT:
                switch (minor) {
                case LNX_NET_LINKUP_EVENT:                  
                    lnx_net_linkup();
                    break;
                case LNX_DLC_LINKUP_EVENT:
                    lnx_dlc_linkup();
                    break;
                default:
                    break;
                }
                break;
            default:
                break;
            }
        }
    }
}


static process lnx_data_process (void)
{
    process_wait_on_system_init();
    process_watch_queue(lnx_dataQ, ENABLE, RECURRING);

    for(;;) {
        process_wait_for_event();
	QllcSend();
    }
}
