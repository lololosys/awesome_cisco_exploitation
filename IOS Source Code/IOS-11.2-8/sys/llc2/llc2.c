/* $Id: llc2.c,v 3.19.4.23 1996/09/12 20:00:42 ggyurek Exp $
 * $Source: /release/112/cvs/Xsys/llc2/llc2.c,v $
 *------------------------------------------------------------------
 * llc2.c -- LLC2 user and system interfaces
 * 
 * January 1991, Rick Watson
 * 
 * llc2.c    v4.4   Copyright EUCS 1986,1987,1988 
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: llc2.c,v $
 * Revision 3.19.4.23  1996/09/12  20:00:42  ggyurek
 * CSCdi44511:  LLC2 timer process may overrun its stack
 * Increased stack size for LLC2 Timer process to 1500 words
 * Branch: California_branch
 *
 * Revision 3.19.4.22  1996/08/26  15:10:41  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.19.4.21  1996/08/13  22:36:29  kpatel
 * CSCdi63658:  IBM: Unknown L3 PID, fr-doencap failed on router with
 *              TokenRing. Dont break making low end images.
 * Branch: California_branch
 *
 * Revision 3.19.4.20  1996/08/12  02:16:04  ppearce
 * CSCdi64450:  CSCdi59527 broke c4500-boot-m image
 * Branch: California_branch
 *   Add srb_core_registry that will definitely be included w/SRB subblock
 *
 * Revision 3.19.4.19  1996/08/11  22:15:40  bwu
 * CSCdi49831:  cls assert at kCepStateIdReqPending when activating appn
 * links
 * Branch: California_branch
 * Reset xid llc1 timer when UA to SAMBE is received
 *
 * Revision 3.19.4.18  1996/08/08  01:57:37  kpatel
 * CSCdi63658:  IBM: Unknown L3 PID, fr-doencap failed on router with
 *              TokenRing
 * Branch: California_branch
 *
 * Revision 3.19.4.17  1996/08/07  09:01:39  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.19.4.16  1996/08/03  23:33:38  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.19.4.15  1996/07/23  13:26:10  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.19.4.14  1996/07/13  11:39:34  fred
 * CSCdi62873:  NULL dereference in fair-queue routines when called by
 *         traffic shaping - make hash_mask independent of caller
 * Branch: California_branch
 *
 * Revision 3.19.4.13  1996/07/11  04:51:21  monical
 * CSCdi60994:  llc2 dynwind command not parsed correctly
 * Branch: California_branch
 *
 *
 * This ddts also includes a fix for the problem where the parser
 * accepts invalid values, even though an error message was displayed.
 *
 * Revision 3.19.4.12  1996/07/09  06:00:54  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.19.4.11  1996/06/28  00:03:45  kchiu
 * CSCdi61200:  llc2 dynwind (nec d2-11) problem back again
 * Branch: California_branch
 *
 * Revision 3.19.4.10  1996/06/24  09:22:04  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.19.4.9  1996/05/21  09:56:57  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.19.4.8  1996/05/18  13:24:15  ppearce
 * CSCdi58098:  Remove use of llc2_default[] table - use defaults directly
 * Branch: California_branch
 *
 * Revision 3.19.4.7  1996/05/17  11:27:04  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.15.2.12  1996/05/14  16:36:30  ppearce
 * CSCdi57596:  Spurious access from llc2_init_fair_sum()
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.11  1996/05/05  23:32:42  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.15.2.10  1996/05/02  15:51:56  sbales
 * CSCdi51340:  Memory Leak DLSw on activation of PU2.1
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.9  1996/04/26  15:22:25  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.15.2.8  1996/04/26  07:56:36  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.15.2.7  1996/04/24  06:30:34  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.15.2.6  1996/04/15  00:09:13  kchiu
 * Branch: IbuMod_Calif_branch
 * Add dlsw+/fddi support
 *
 * Revision 3.15.2.5  1996/04/03  20:03:52  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.15.2.4  1996/03/17  18:06:57  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.15.2.3  1996/03/15  05:55:48  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comments
 *
 * Revision 3.15.2.2  1996/03/01  16:31:11  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.15.2.1  1996/02/28  21:24:20  ppearce
 * CSCdi50230:  IBU modularity - Remove unnecessary header file includes
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.4.6  1996/05/10  01:34:47  wmay
 * CPP and ppp half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.19.4.5  1996/04/29  10:03:03  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.19.4.4  1996/04/29  09:39:57  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.19.4.3  1996/04/25  23:15:40  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.19.4.2  1996/04/17  13:44:49  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.19.4.1  1996/03/18  21:11:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.4  1996/03/16  07:13:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.2.3  1996/03/13  01:50:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.9.2.2  1996/03/07  10:06:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.9.2.1  1996/02/20  01:09:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.19  1996/03/11  17:45:45  ppearce
 * CSCdi49044:  Router does not reconnect after DM received via Frame-Relay
 *   Don't stop XID timer if DISC/DM received - allow XID timer to expire
 *
 * Revision 3.18  1996/03/06  16:46:12  pleung
 * CSCdi35377:  %LLC-2-UNEXPECT: LLC2 : validate_llc UNEXPECT problem.
 *
 * Revision 3.17  1996/03/01  00:48:59  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up correctly
 * Restore CSCdi47275 w/fix to netbios_as.c
 *
 * Revision 3.16  1996/02/28  01:02:47  kchiu
 * CSCdi48822:  llc2 windowing mechanism doesn't work properly under
 * congestion
 *
 * Revision 3.15  1996/02/16  18:09:21  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Backout CSCdi47275 temporarily-- it breaks igs-c-l build
 *
 * Revision 3.14  1996/02/15  01:37:59  dbilling
 * CSCdi47275:  CLS over RSRB(lack) does not clean up lakie
 * Fix cleanup and change appl. cleanup from registry to vector
 *
 * Revision 3.13  1996/02/07  16:14:00  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.12  1996/02/03  03:17:31  vrawat
 * CSCdi47917:  Problems with show llc2 command in FRAS serial DTE info
 * field
 *
 * Revision 3.11  1996/02/01  06:07:13  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.10  1996/01/29  23:28:45  vrawat
 * CSCdi47386:  FRAS should ignore l3pid in frames received from
 * frame-relay side
 *
 * Revision 3.9  1996/01/22  06:38:16  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1996/01/22  03:55:41  dmccowan
 * CSCdi45007:  LLC2 LCP negotiated over PPP when not wanted
 *
 * Revision 3.7  1996/01/17  03:00:49  wilber
 * CSCdi46932:  Transparent Bridging failed to bridge Lantastic LLC
 * LLC2 packet should be bridged if LLC2 stack is not running and bridging
 * is configured
 *
 * Revision 3.6  1996/01/11  17:59:35  dmccowan
 * CSCdi46549:  llc2 over fair-queuing broken
 * use new fair-queue hashing routine when sending frames not
 * associated with a specific llctype.
 *
 * Revision 3.5  1995/12/17  18:33:13  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/21  19:33:57  sberl
 * CSCdi42181:  More than 1000 LLC2 sessions causes excessive tracebacks
 * Remove the check for > 1000 llc sessions from llc2_tick().
 * Add a counter to keep track of how many llc2 sessions there really are.
 * Add this count to banner for "show llc2".
 * Make llc2_starttimer() a real function instead of an inline. It was way
 * too big.
 *
 * Revision 3.3  1995/11/17  17:43:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:01:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:35:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.16  1995/11/08  21:15:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.15  1995/10/25  00:11:21  lpereira
 * CSCdi37113:  appn stop link should send disconnect to end stations
 *
 * Revision 2.14  1995/10/18  05:33:40  pmorton
 * CSCdi40639:  CMNS uses wrong MAC address to open LLC2 session
 * Call llc_clear_rii so that the addresses passed in to llc1_open1 are
 * clean.
 *
 * Revision 2.13  1995/09/17  15:50:08  ppearce
 * CSCdi40438:  Cannot configure LLC parms for RSRB/LAK on CSNA virtual i/f
 *   LLC2 Channel interface commands lost after reload
 *
 * Revision 2.12  1995/09/15  05:57:15  ppearce
 * CSCdi40438:  Cannot config LLC parms for RSRB/LAK on CSNA virtual i/f
 *
 * Revision 2.11  1995/09/01  06:02:06  ppearce
 * CSCdi39705:  MEMD-3-BADVCN when using CSNA with RSRB/TCP local-ack
 *   Re-work how CSNA header is added before sending pak to CIP interface
 *
 * Revision 2.10  1995/08/24  23:43:28  utam
 * CSCdi38916:  parser does not accept : llc2 dynwind nw 3
 *
 * Revision 2.9  1995/08/12  07:09:13  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.8  1995/08/03  21:09:01  hampton
 * Convert LLC2 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi38203]
 *
 * Revision 2.7  1995/07/20 19:00:23  ppearce
 * CSCdi37404:  CIP-to-RSRB with TCP/lack not working
 *
 * Revision 2.6  1995/07/19  21:03:39  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add service to reducce dependency on idb.
 *
 * Revision 2.5  1995/07/06  21:46:41  sberl
 * CSCdi36777:  WFQ should count llc2 sessions as conversations
 * Add logic to classify LINK_LLC2 packets into LLC2 conversations.
 *
 * Revision 2.4  1995/06/21  08:55:47  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/19  06:48:15  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  19:28:24  sberl
 * CSCdi35593:  Clean up cls queuing option support
 * All dlus hard code the default behaviour until they get around to
 * adding configuration.
 *
 * Revision 2.1  1995/06/07  21:41:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "../cls/dlc_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "../srt/srt_registry.h"
#include "../srt/srb_core_registry.h"
#include "../partner/partner_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_llc2.h"
#include "../parser/parser_actions.h"
#include "../ui/debug.h"
#include "llc2.h"
#include "llc2_debug.h"
#include "lltimers.h"
#include "llc2_sb.h"
#include "ieee.h"
#include "../if/network.h"
#include "../os/free.h"

#include "../srt/rsrb.h"	/* caccac may be dispensable */
#include "../srt/srb_core.h"

#include "../srt/lack_externs.h"

#include "../if/tring.h"
#include "../cls/dlc.h"
#include "../cls/cls_assert.h"
#include "../x25/x25.h"
#include "llc2_inline.h"
#include "../wan/frame_relay.h"
#include "../wan/ppp.h"
#include "../wan/ppp_registry.h"
#include "../parser/parser_defs_priority.h"
#include "../if/priority_private.h"
#include "../if/rif_util.h"
#include "../if/rif_inline.h"

#define LLC2_MAX_ILEN    8150 

/*
 * Forward declarations
 */
static int find_in_event(uchar *, int);
static void eval_n_r(llctype *);
static void eval_n_s(llctype *);
static void llc_open_command(idbtype *idb, uchar *dst, boolean blocking);
static void llc_attach_command(idbtype *idb, uchar *dst);
static void llc_send_command(int cookie, uchar *data, int datalen);
static void llc_close_command(int cookie);
static void llc_stat_command(void);
static void llc_offset_command(int offset);
static void llc_enter_dte_hash(llctype *llc);
static boolean clsIsPakCls(paktype *pak);
static void llc2_init_fair_sum(idbtype *idb, llctype *llc);
static boolean llc2_on_idb_override;

/*
 * Global storage
 */
int		llc2_running;
sys_timestamp	llc2_nexttime;
queuetype	llc2_sabmeQ;	      /* hold SABME packets until llc2_timers*/
queuetype	llc2_cleanupQ;	      /* llcs awaiting final disposition */
queuetype	llctQ;		      /* llc timer queue */
static llctype	*llc2stationptr;
llctype		*llc_dte_hash_table[LLC_DTE_HASH_SIZE];
int		llc_in_hash_count;
queuetype       llc2_freelistQ;
llc2_register_type llc2_registration[128];

/*
 * Traffic and error counters.
 */
#ifdef NOBODY_READS_ME
int llc_stats_nomem;			/* no memory */
int llc_stats_unexpsabme;		/* unsolicited sabme */
int llc_stats_ignored;			/* ignored frames */
#endif

/*
 * c l s I s P a k C l s ( )
 * classify incoming packet to determine if it should be processed
 * by legacy code (return FALSE) or by CLS code (return TRUE).
 */
static boolean clsIsPakCls(paktype *pak)
{
    sap_entry_t *sap_entry;
    sap_hdr     *llcheader;
    hwaddrtype  destAddr;
    hwaddrtype  interfaceAddr;
    hwidbtype   *hwidb;
    idbtype     *swidb;
    trrif_t *trh = (trrif_t *)pak->datagramstart;

    swidb = pak->if_input;
    if (swidb == NULL) {
	buginf("\nclsIsPakCls: NULL pak->if_input");
	return(FALSE);
    }
    
    hwidb = swidb->hwptr;
    if (hwidb == NULL) {
	buginf("\nclsIsPakCls: bad hwidb");
	return(FALSE);
    }
    
    /* 
     * Check for DLSW 
     * If the bridging SAP has been registered on this interface, then
     * always return TRUE, otherwise, keep on checking other things.
     *
     *   -- SAP activated by CLS?
     *   -- Is RIF 'terminated' into DLSw?...
     *   --   ...or is it from a CLS_TBRIDGE vidb
     *
     */
    sap_entry = sap_get_bridging_entry(swidb);
    if ((sap_entry != NULL) && (sap_entry->sap_flags & SAP_CLS_ACT_SAP)) {
        srb_vector_table_t *srbV_p = reg_invoke_srb_get_vector_table();
        if (srbV_p) {
            ushort srb_targetring = srbV_p->srb_sb_get_targetring(swidb);
            if ((RIF_DEST_RING(trh) == srb_targetring) ||
	        (hwidb->status == (IDB_VIRTUAL | IDB_TR))) {
	        return(TRUE);
            }
        }
    }

    if (is_ieee802(hwidb)) {
        (*hwidb->extract_snpa)(pak, &destAddr, DESTINATION);
	(*hwidb->extract_hwaddr)(hwidb, &interfaceAddr);

	/*
	 * For now only accept packets pointed to this IDB's macaddress!
	 */
	if (!snpa_equ(&destAddr, &interfaceAddr))
	    return(FALSE);
    }

    llcheader = (sap_hdr*) pak->info_start;
    sap_entry = sap_get_entry(swidb, llcheader->dsap);

    /*
     * no SAP registration means -> LEGACY code!
     */
    if (sap_entry == NULL)
	return(FALSE);

    /*
     * return value of single flag.  Ignore other fields,
     * as anything which registered for CLS should receive
     * frame.  Can use other SAP registry types to restore
     * registry type on SAP 'deregistry'...
     */
    return((sap_entry->sap_flags) & SAP_CLS_ACT_SAP);
}

/* 
 * llc2_debug_filter
 */

boolean llc2_debug_filter (boolean *pflag, llctype *llc)
{
    debug_list_t *list;
    uchar data[2];
    int offset = IEEEBYTES * 2;
    
    list = debug_listq_find(pflag);
    if (list == NULL) {
	return (TRUE);
    }
    if (llc == NULL) {
	return (TRUE);
    }
    if (!validmem(llc)) {
	return (TRUE);
    }
    if (list->idb != NULL && list->idb != llc->idb ) {
	return (FALSE);
    }
    if (list->access_list == 0) {
	return (TRUE);
    }
    data[0] = llc2_get_rsap(llc);
    data[1] = llc2_get_lsap(llc);
    return(reg_invoke_xmacaccesscheck_gen(list->access_list,
			       (mac_address_type *)llc2_get_rmac(llc),
			       (mac_address_type*)llc2_get_lmac(llc),
			       (uchar *)&data, offset, 2));
}

/*
 * LLC2 subsystem header
 */

#define LLC2_MAJVERSION 1
#define LLC2_MINVERSION 0
#define LLC2_EDITVERSION  1

SUBSYS_HEADER(llc2, LLC2_MAJVERSION, LLC2_MINVERSION, LLC2_EDITVERSION,
	      llc2_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: dlc, ppp",
	      "req: dlc");

/* 
 * llc2_findcirc.
 * Returns a pointer to the llctype structure for the connection.
 */
llctype *llc2_findcirc (uchar *srcdteptr, idbtype *idb)
{
    register llctype *llc;

    if (is_smds(idb->hwptr))
       llc = llc_by_dte(&srcdteptr[SMDS_RMAC_DTE], 
                        (int) srcdteptr[SMDS_LSAP_DTE], 
	   	        (int) srcdteptr[SMDS_RSAP_DTE], idb);
    else if (is_ppp(idb->hwptr))
       llc = llc_by_dte(srcdteptr, 
                        (int) srcdteptr[PPP_LSAP_DTE], 
	   	        (int) srcdteptr[PPP_RSAP_DTE], idb);
    else
       llc = llc_by_dte(&srcdteptr[RMAC_DTE], (int) srcdteptr[LSAP_DTE], 
	   	        (int) srcdteptr[RSAP_DTE], idb);

    return llc;
}

llctype* llc2_by_ucepid(void* uCepId)
{
    int     n;
    llctype *llc;

    for (n = 0; n < LLC_DTE_HASH_SIZE; n++) {
	llc = llc_dte_hash_table[n];
	while (llc != NULL) {
	    if (llc->u_cep_id == uCepId)
		return llc;
	    else
		llc = llc->next_hash;
	}
    }
    return NULL;
}

/* 
 * set the llc2 values 
 */

static void llc2_set_llc2_values(llctype *llc, llc_sb_t *llc_sb)
{
        llc->llc2_akmax            = llc_sb->llc2_akmax ;
        llc->llc2_akdelay_time     = llc_sb->llc2_akdelay_time;  
        llc->llc2_wind		   = llc_sb->llc2_wind;  
        llc->llc2_T1_time 	   = llc_sb->llc2_T1_time; 
        llc->llc2_n2 		   = llc_sb->llc2_n2;     
        llc->llc2_trej_time 	   = llc_sb->llc2_trej_time;
        llc->llc2_tpf_time 	   = llc_sb->llc2_tpf_time;
        llc->llc2_tbusy_time 	   = llc_sb->llc2_tbusy_time;
        llc->llc2_idle_time 	   = llc_sb->llc2_idle_time;
        llc->llc2_xid_rtry_time    = llc_sb->llc2_xid_rtry_time;
        llc->llc2_xid_neg_val_time = llc_sb->llc2_xid_neg_val_time;
        llc->llc2_txQ_max 	   = llc_sb->llc2_txQ_max;    
	llc->llc2_adm_timer_value  = llc_sb->llc2_adm_timer_value;
}

/* 
 * llc2_findfree.
 * Allocates a new llctype struct.
 * Returns NULL if no memory.
 */
llctype *llc2_findfree (int locallsap, idbtype *idb)
{
    llctype   *llc;
    llc_sb_t *llc_sb;

    llc_sb = llc_get_or_create_sb(idb);
    if (!llc_sb) {
      return(NULL);
    }

    /* disallow station component LSAP - i.e. 00  */
    if (locallsap == 0)	
	return(NULL);

    if ((llc = get_llctype()) == NULL) 
	return(NULL);

    /* Initialize llctype data structures */
    memset(llc, 0, sizeof(llctype)); 

    llc->q_elem.q_context = llc;

    /* Initialize llc2 values */
    llc2_set_llc2_values(llc, llc_sb);

    /* local window is established here, won't follow value in the sb */
    llc->loc_wind = llc_sb->llc2_wind<<1;
    llc->rem_wind = REM_WIND_DEFAULT<<1;

    /* Set prioritization to off for now. */
    llc->llc2_queuing_type = CLS_QUEUING_DEFAULT;

   /* initialize dynamic window related parameters */
    llc->nw = llc_sb->llc2_nw;
    llc->fr_dwc = llc_sb->llc2_fr_dwc;
    llc->ww = llc_sb->llc2_wind;
    llc->good_frame_count = 0;
    llc->orig_loc_wind = llc_sb->llc2_wind;

   /*
    * At this point we must assume the DLU is a legacy user of LLC2.
    * CLS LLC's will adjust hi and lo water marks later in llc2_open1().
    */
    CepInit(&llc->fCep, &llc->txQ, llc_sb->llc2_txQ_max, 
	    LLC_OQUEUE_LOW_WATER_MARK, LLC_OQUEUE_HIGH_WATER_MARK, 0);
    return(llc);
}

/*
 * Handle a SABME for a new connection, at interrupt level
 */

static paktype *llc2_handle_sabme (paktype *pak, llctype **llcp, uchar* srcdtestring)
{
    llctype *llc;
    uchar *frameptr;
    idbtype *idb = pak->if_input;
    hwaddrtype tempaddr;
    
    if (pak->if_input == NULL) {
	protocol_discard(pak, TRUE);
	return(NULL);
    }

    frameptr = pak->info_start;

    /*
     * Currently we don't listen for connects except in
     * the case of the debugging code.  CONS will likely
     * have to add code here to check for a listening
     * process
     */
    if (((!llc2_debug_test) && 
	 (!idb->hwptr->cmns_intfc_enabled) &&
	 (!(pak->flags & PAK_LACK_IN)) &&
	 (!llc2_registration[*frameptr / 2].proc))) {
	llc = NULL;
    } else 
	llc = llc2_findfree((int)*frameptr, pak->if_input);
    
    if (llc == NULL) {
#ifdef NOBODY_READS_ME
	if (llc2_debug_test)
	    llc_stats_nomem++;
	else
	    llc_stats_unexpsabme++;
#endif
	/* 
	 * Use station component to send DM RSP 
	 * llc->p_f_val will be set up prior to call of 
	 * llc2_stateswitch 
	 */
	llc = llc2stationptr;

	bcopy(srcdtestring, llc->llc2_dte, LLC_MAX_DTE_SIZE);

	/* Store the rif for DLSw... */
	llc2_store_rif(llc, pak->rif_start, CHANGE_DIRECTION);

	(*idb->hwptr->extract_snpa)(pak, &tempaddr, SOURCE);
	llc->llc2_addrtype = snpa_get_type(&tempaddr);

	if (pak->flags & (PAK_LACK_IN | PAK_LACK_OUT)) {
	    llc->rsrb_lack = TRUE;
	} else {
	    llc->rsrb_lack = FALSE;
	}
	
	llc->idb = pak->if_input;
	send_control(llc, DM_IND);
	
	protocol_discard(pak, TRUE);
	return(NULL);
    }

    bcopy(srcdtestring, llc->llc2_dte, LLC_MAX_DTE_SIZE);
    (*idb->hwptr->extract_snpa)(pak, &tempaddr, SOURCE);
    llc->llc2_addrtype = snpa_get_type(&tempaddr);
    
    if (pak->flags & (PAK_LACK_IN | PAK_LACK_OUT)) {
	llc->rsrb_lack = TRUE;
    } else {
	llc->rsrb_lack = FALSE;
    }
    
    llc->u_cep_id = NO_CLS;	/* LLC is under control of legacy code
				   (RSRB/LA, SDLLC/LA, LNM) */
    llc->idb = pak->if_input;
    llc_enter_dte_hash(llc);
    llc->state = LLS_ADM;
    
    /*
     * Init the linktype to LINK_LLC2 so that we can send
     * controls correctly.
     * When we figure out who this is for, reset it to the
     * correct link type.
     */
    if (pak->flags & PAK_LACK_IN)
	llc->linktype = LINK_LLC2;
    else
	llc->linktype = llc2_registration[*frameptr / 2].link;

    if (!llc->linktype)
	llc->linktype = LINK_LLC2;
    
    if ((!(pak->flags & PAK_LACK_IN)) && llc2_registration[*frameptr / 2].proc)
	(*llc2_registration[*frameptr / 2].proc)(llc);
    *llcp = llc;

    /*
     * Find possible lack or sdllc associated structures and fix up pointers to them.
     */

    /*
     * Invoke application specific (lack, sdllc) code to complete
     * the initialization.
     * pak is used to signal error condition:
     *   NULL - error in sdllc_fix_llc()
     *   non-NULL - status
     *
     * also modified lack_fix_llc() to handle additional
     * pak indirection
     */
    reg_invoke_media_llc_create(llc, &pak);

    /*
     * Initialize WFQ parameters for this llc
     *   (provided WFQ has been configured for the interface)
     */
    if (idb->hwptr->fair_queue_head) {
        llc2_init_fair_sum(idb, llc);
        llc->llc2_fair_sequence = LLC2_FAIR_SEQ_DEFAULT;
    }

    return(pak);
}

/*
 * These next two routines are responsible for building the dte string
 * based on various input formats. 
 * 
 * The first one llc2_build_dte_string() hs the components of the dte
 * string listed as explicit arguments to the function. It is used to
 * initialize the llctype based on input parameters to llc2open1().
 * 
 * The second llc_by_pak() builds a dte string from the contents of a
 * received packet. It is used to build a dte string in llc2_input() to
 * match the incoming packet against existing llctypes and if running the
 * legacy code, to initialize a new one for us.
 * 
 * The important thing here is that both of these functions generate the
 * same string.
 */

void llc2_build_dte_string(uchar* dtestring,
			   hwaddrtype* remoteMac,
			   hwaddrtype* localMac,
			   uchar remoteSap,
			   uchar localSap,
			   uchar l3pid)
{
    memset(dtestring, 0, LLC_MAX_DTE_SIZE);

    switch (snpa_get_type(remoteMac))
    {
    case STATION_IEEE48:
	ieee_copy(remoteMac->addr, &dtestring[RMAC_DTE]);
	dtestring[LSAP_DTE] = localSap;
	dtestring[RSAP_DTE] = remoteSap;
	if (snpa_get_type(localMac) == STATION_IEEE48)
	    ieee_copy(localMac->addr, &dtestring[LMAC_DTE]);
	else
	    ieee_zero(&dtestring[LMAC_DTE]);
	break;
    case STATION_FR10:
	/* build a FR DTE string */
	snpa_copy_to_buffer(remoteMac, &dtestring[FR_DLCI_DTE]);
	dtestring[FR_LSAP_DTE] = localSap;
	dtestring[FR_RSAP_DTE] = remoteSap;
	break;
    case STATION_PPP:
	ieee_copy_sans_rii(remoteMac->addr, &dtestring[RMAC_DTE]);
	dtestring[LSAP_DTE] = localSap;
	dtestring[RSAP_DTE] = remoteSap;
	if (snpa_get_type(localMac) == STATION_PPP)
	    ieee_copy_sans_rii(localMac->addr, &dtestring[LMAC_DTE]);
	else
	    ieee_zero(&dtestring[LMAC_DTE]);
	break;
    case STATION_SMDS64:
	/* build an SMDS DTE string */
	snpa_copy_to_buffer(remoteMac, &dtestring[SMDS_RMAC_DTE]);
	snpa_copy_to_buffer(localMac, &dtestring[SMDS_LMAC_DTE]);
	dtestring[SMDS_LSAP_DTE] = localSap;
	dtestring[SMDS_RSAP_DTE] = remoteSap;
	break;
    case STATION_ATMVC:
	/* build a ATM 1483 DTE string */
	snpa_copy_to_buffer(remoteMac, &dtestring[ATM_VCD_DTE]);
	dtestring[ATM_LSAP_DTE] = localSap;
	dtestring[ATM_RSAP_DTE] = remoteSap;
	dtestring[ATM_L3_PID_DTE] = l3pid;
	break;
    default:
	if (llc2_debug_err)
	    buginf("\nllc2_build_dte_string: Unsupported address type %d",
		   snpa_get_type(remoteMac));
	break;
    }
    return;
}

/*
 * llc2_by_pak
 *
 * Given a parsed packet find an llc to go with it
 * Return NULL if there isn't one.
 *
 * Also generates and returns the dte string used for the search. This
 * can be used later to initialize a new llctype structure.
 *
 * Also returns the addrtype value of the mac layer addressing format.
 * See address.h for the values.
 *
 * Currently handles IEEE 48 bit and Frame Relay 10 bit addresses. 
 */
                                                  
llctype *llc2_by_pak (paktype *pak, uchar *dtestring, uchar *addrtype, ushort *Dlci)
{
    sap_hdr	*llc1;
    hwaddrtype  localAddr;
    hwaddrtype  remoteAddr;
    idbtype     *idb;
    trrif_t     *trh;
    ushort      temp_dlci; 
    fr_idb_struct_type *fr_idb;

    idb = pak->if_input;
    if (idb == NULL)
    {
	buginf("\nllc2_by_pak: pak->if_input is NULL");
	return(NULL);
    }
    
    (*idb->hwptr->extract_snpa)(pak, &remoteAddr, SOURCE);
    *addrtype = snpa_get_type(&remoteAddr);
    llc1 = (sap_hdr *)pak->info_start;
    
    memset(dtestring, 0, LLC_MAX_DTE_SIZE);
    *Dlci = 0 ; /* for everything except FRAS BAN */ 

    switch (*addrtype)
    {
    case STATION_IEEE48:
	(*idb->hwptr->extract_snpa)(pak, &localAddr, DESTINATION);
	ieee_copy(remoteAddr.addr, &dtestring[RMAC_DTE]);
	dtestring[LSAP_DTE] = llc1->dsap;
	dtestring[RSAP_DTE] = llc1->ssap & ~SAP_RESPONSE;
	if ((pak->flags & (PAK_LACK_IN | PAK_LACK_OUT)) || clsIsPakCls(pak)) {
	    ieee_copy(localAddr.addr, &dtestring[LMAC_DTE]);
	}
	break;
	
    case STATION_FR10:
	snpa_copy_to_buffer(&remoteAddr, &dtestring[FR_DLCI_DTE]);
	/*
	 * The incoming packet is addressed to us, so...
	 * it's dsap is our local (source) sap.
	 * it's ssap is our remote (destination) sap
	 */
	dtestring[FR_LSAP_DTE] = llc1->dsap;
	dtestring[FR_RSAP_DTE] = llc1->ssap & ~SAP_RESPONSE;

        if (pak->llc2_enctype != ET_BRIDGE) { 
            /* handling fras BNN */ 
            snpa_copy_to_buffer(&remoteAddr, &dtestring[FR_DLCI_DTE]);
	     /*
	     * The incoming packet is addressed to us, so...
	     * it's dsap is our local (source) sap.
	     * it's ssap is our remote (destination) sap
	     */
	    dtestring[FR_LSAP_DTE] = llc1->dsap;
	    dtestring[FR_RSAP_DTE] = llc1->ssap & ~SAP_RESPONSE;
        } else {
            /* handling fras BAN */ 
            trh = (trrif_t *) pak->mac_start; /* pak->datagramstart points to frame_relay
                                                 header and pak->macstart points to start
                                                 of token-ring header*/ 
            ieee_copy_sans_rii(trh->saddr, &dtestring[RMAC_DTE]);
            ieee_copy(trh->daddr, &dtestring[LMAC_DTE]);
            dtestring[LSAP_DTE] = llc1->dsap;
	    dtestring[RSAP_DTE] = llc1->ssap & ~SAP_RESPONSE;
            snpa_copy_to_buffer(&remoteAddr, (uchar *)&temp_dlci);
            fr_idb =  idb->hwptr->frame_relay_stuff; /* this is used in DLCI_TO_NUM */
            *Dlci = DLCI_TO_NUM(temp_dlci); 
	}
	break;

      case STATION_PPP:
	(*idb->hwptr->extract_snpa)(pak, &localAddr, DESTINATION);
	ieee_copy(remoteAddr.addr, &dtestring[RMAC_DTE]);
	dtestring[LSAP_DTE] = llc1->dsap;
	dtestring[RSAP_DTE] = llc1->ssap & ~SAP_RESPONSE;
	break;
	
    case STATION_SMDS64:
	snpa_copy_to_buffer(&remoteAddr, &dtestring[SMDS_RMAC_DTE]);
	(*idb->hwptr->extract_snpa)(pak, &localAddr, DESTINATION);
	snpa_copy_to_buffer(&localAddr, &dtestring[SMDS_LMAC_DTE]);
	dtestring[SMDS_LSAP_DTE] = llc1->dsap;
	dtestring[SMDS_RSAP_DTE] = llc1->ssap & ~SAP_RESPONSE;
	break;

    case STATION_ATMVC:
	snpa_copy_to_buffer(&remoteAddr, &dtestring[ATM_VCD_DTE]);
	/*
	 * The incoming packet is addressed to us, so...
	 * it's dsap is our local (source) sap.
	 * it's ssap is our remote (destination) sap
	 */
	dtestring[ATM_LSAP_DTE] = llc1->dsap;
	dtestring[ATM_RSAP_DTE] = llc1->ssap & ~SAP_RESPONSE;
	/* 
	 * This next line should probably be replaced by some sort of
	 * accessor method call. 8/30/94 [sberl]
	 */
	dtestring[ATM_L3_PID_DTE] = *(pak->info_start - 1);
	break;

    default:
	buginf("\nllc2_by_pak: Address type %d Not Handled!",
	       *addrtype);
	return(NULL);
    }
    
    return(llc_by_dte(dtestring, 
		      llc1->dsap, 
		      llc1->ssap & ~SAP_RESPONSE, 
		      idb));
}

static void frame_relay_congested_count(paktype *pak, llctype *llc, int event)
{
    /*
     * Decrease the window size during congestion or
     * increase the window size after certain number of consecutive
     * good frames received.
     */
    if ((pak->fr_flags & PAK_FR_BECN) == PAK_FR_BECN) {
        if (llc->fr_dwc == FRAME_RELAY_DWC_DEFAULT)
            llc->ww = FRAME_RELAY_DWC_DEFAULT;
        else if (llc->ww != FRAME_RELAY_DWC_DEFAULT) {
            llc->ww = llc->ww / llc->fr_dwc;
            if (!llc->ww)
                llc->ww = FRAME_RELAY_DWC_DEFAULT;
        }
        if (llc2_debug_dynwind)
            buginf("\nLLC2/DW: BECN received! event %s, Window size reduced to %d", llc2_prevent(event), llc->ww);
        llc->good_frame_count = 0;
        llc->loc_wind = llc->ww<<1;
    }
    else if (event <= REC_I_RSP) { /* I frame only */
        if (llc->ww == llc->orig_loc_wind)
            return;
        llc->good_frame_count++;
        if (llc2_debug_dynwind)
            buginf("\nLLC2/DW: %d consecutive I-frame(s) received without BECN",
                llc->good_frame_count);
        if (llc->good_frame_count == llc->nw) {
            llc->ww++;
            llc->good_frame_count = 0;
            if (llc->ww > llc->orig_loc_wind)
                llc->ww = llc->orig_loc_wind;
            llc->loc_wind = llc->ww<<1;
            if (llc2_debug_dynwind)
                buginf("\nLLC2/DW: Current working window size is %d", llc->ww);
        }
    }
}

/*
 * llc2_input.
 * Come here with llc2 datagram to process at interrupt level.
 * pak->datagramstart points to the beginning of the mac frame,
 * pak->datagramsize is the size of the entire frame.
 */
void llc2_input (paktype *pak)
{
    int       newevent;
    int       responsebit, poll_final_bit;
    uchar     *frameptr;
    uchar     srcdtestring[LLC_MAX_DTE_SIZE];
    llctype   *llc;
    idbtype   *idb;
    uchar     conch;
    paktype   *temppak;
    tr_vpeertype *vrp;
    boolean   isClsSAP;		/* is currect DSAP registered in CLSI?? */
    uchar     addrtype;
    ushort    Dlci;    

    if (!llc2_running) {
        if (pak->if_input && pak->if_input->tbridge_on_swidb) {
            reg_invoke_bridge_enqueue(pak, FALSE, TRUE);
        } else {
	    if (llc2_debug_packet)
		hexout(pak->datagramstart, pak->datagramsize, "llc2_input");
	    protocol_discard(pak, TRUE);
	}
	return;				/* and return */
    }

    idb = pak->if_input;
 
    frameptr = pak->info_start;		/* Should always point at dsap */
					/* of llc header */

    isClsSAP = clsIsPakCls(pak);

    newevent = find_in_event(frameptr, 
			     pak->datagramsize - (pak->info_start - pak->datagramstart));

    llc = llc2_by_pak(pak, srcdtestring, &addrtype, &Dlci);
    /*
     * Happens in two cases:
     *    1. Old style llc code
     *    2. CLSI registered SAP, but before Request Open Station
     */

    if (llc == NULL) {
	/*
	 * on receiving SABME, need to use station component
	 * to send indication up to CLS
	 * but don't allocate structure here!!!
	 * indication will be made from admstate()
	 */
	if (newevent == REC_SABME_CMD) {
	    if (isClsSAP) {
		if (pak->flags & (PAK_LACK_IN | PAK_LACK_OUT)) {
		    /* 
		     * (defensive code)
		     * clearing these flags prevents handle_sabme from treating
		     * incoming (CLSI) connection as Local-Ack session
		     */
		    buginf("\n CLI: Warning!!  PAK_LACK_* set on incoming SABME!");
		    pak->flags &= ~(PAK_LACK_IN | PAK_LACK_OUT);
		}
		/*
		 * Use statically defined station component to send
		 * message to CLS layer.  CLS will explicitly create LLC
		 * later with a Request Open Station...
		 */
		llc = llc2stationptr;
		llc->u_cep_id = (void*) CLS_NO_CEP;
		llc->llcRole = LLC2_SECONDARY;
	    } else {	/* SAP is under control of legacy code */
		/*
		 * If the connection block isn't found and we have a SABME, we're
		 * likely running at interrupt level so we can't create the
		 * connection block.  Queue it for llc2_timers().
		 */

                /*
                 * Frame relay is not supported for non CLS registered SAPs. 
                 * Add SMDS, ATM, etc here as DLUs support other medias.    
                 * (Handles SABME received prior to DLUs ACTIVATE SAP)         
                 */
                if (is_frame_relay(idb->hwptr)) {
                  protocol_discard(pak, TRUE);
                  return;
                }

		temppak = pak;
		if ((pak = llc2_handle_sabme(temppak, &llc, srcdtestring)) == NULL) {
		    if (llc2_debug_packet)
			hexout(pak->datagramstart, pak->datagramsize, "llc2_input");
		    return;
	}
		if (llc == NULL) {
		    buginf("\nLLC ptr not returned from llc2_handle_sabme");
		    return;
		}
	    }	/* endIf Cls/legacy code SAP */
	} else {     /* newevent is something other than SABME */
	    /* 
	     * (input frame is something other than SABME)
	     * Don't know about this connection, use station component in
	     * ADM state to send DM RSP if needed.
	     */
	    llc = llc2stationptr;
	    
            if ((pak->flags & PAK_LACK_IN) || (isClsSAP))
		llc->linktype = LINK_LLC2;
	    else
		llc->linktype = llc2_registration[*frameptr / 2].link;

            if (!llc->linktype)
	        llc->linktype = LINK_LLC2;

	    llc->idb = pak->if_input;
	    llc->state = LLS_ADM;
	}

	/* 
	 * copy the dte string into the link structure. 
	 */
	bcopy(srcdtestring, llc->llc2_dte, LLC_MAX_DTE_SIZE);
	
	/* We need to remember the rif for later, maybe */
	llc2_store_rif(llc, pak->rif_start, CHANGE_DIRECTION);

	llc->fDlci = Dlci;           /* for FR BAN implementation */ 

        llc->llc2_addrtype = addrtype;
        if (pak->flags & (PAK_LACK_OUT | PAK_LACK_IN))
	    llc->rsrb_lack = TRUE;
	else if (isClsSAP)
	    llc->rsrb_lack = FALSE;
	else 
	    llc->rsrb_lack = FALSE;

	/*
	 * Set rnr_disable state only if llc is running for RSRB and
	 * if the peer pointer is valid.  Peer pointer should normally
	 * be defined for RSRB.
	 *  Second check is paranoid corner check...
	 */
	if (llc->rsrb_lack && pak->peer_ptr) {
	    vrp = pak->peer_ptr;
	    llc->rnr_disable = vrp->rnr_disable;
	}
    }	/* endIf for llc == NULL check */

    if (LLC2_DEBUG_PACKET)
	hexout(pak->datagramstart, pak->datagramsize, "llc2_input");
    /*  
     * We have now identified the source of the incoming frame 
     * OR are using the station component should a response be needed
     */

    if ((addrtype == STATION_FR10) && (llc_sb_get_dynwind(idb)))
        frame_relay_congested_count(pak, llc, newevent);

    if (llc->inbuf != NULL) {
	/* Hey, if inbuf is set, then we are being called reentrantly.
	 * What we should do is queue the input. However, that has
	 * implications for lack. So, rather than that, we mask the
	 * bug, and hope it doesn't mess something else up.
	 */

	if (LLC2_DEBUG_ERR) {
	    buginf("\nLLC2: Reentrant call into llc2_input()");
	    bugtrace();
	}
	protocol_discard(llc->inbuf, TRUE);
	llc->inbuf = NULL;
    }

    llc->inbuf = pak;

#if 0
    /* This is now done in dlcllc.c, so it doesn't need to be
     * done here.
     */

    /*
     * SHB says turn off XID retry timer logic here!!!
     * Turn off the XID timer only if we receive a XID or SABME
     */

    if (llc->llc_xid_pak) {
	if ((newevent == REC_XID_CMD) ||
	    (newevent == REC_XID_RSP) ||
	    (newevent == REC_SABME_CMD) ||
            (newevent == REC_UA_RSP)) {

	    datagram_done(llc->llc_xid_pak);
	    llc->llc_xid_pak = NULL;
	    llc2_stoptimer(llc, LLC1_TIMER_M);
	}
    }
#endif

	if (llc != llc2stationptr)
	    llc2_starttimer(llc, IDLE_TIMER, llc->llc2_idle_time);
	responsebit = *(frameptr+1) & 1; 	/* c/r from SSAP */
	if (newevent <= REC_REJ_RSP) {		/* if I or supervisory */
	    poll_final_bit = *(frameptr+3) & 1;
	    llc->new_n_r = *(frameptr+3) & 0XFE;
	    eval_n_r(llc);
	    if (newevent <= REC_I_RSP) { 	/* if I frame */
		llc->new_n_s = *(frameptr+2) & 0XFE;
		eval_n_s(llc);
            }
    } else {
	    poll_final_bit = (*(frameptr+2) & 0X10) >> 4;
	}

	llc->p_f_val = poll_final_bit;
	llc->cmdrspbit = responsebit;

	/*
	 * Display the packet if debugging
	 */
	if (LLC2_DEBUG_PACKET && newevent <= REC_FRMR_RSP) {
	    if (newevent <= REC_I_RSP) { 	/* if I frame */
		buginf("\nLLC: i %s N(R)=%d N(S)=%d V(R)=%d p/f=%d",
		       llc2_prevent(newevent), llc->new_n_r>>1,
		       llc->new_n_s>>1, llc->v_r>>1, poll_final_bit);
	    } else if (newevent <= REC_REJ_RSP) {	/* if supervisory */
		buginf("\nLLC: i %s N(R)=%d p/f=%d", llc2_prevent(newevent),
		       llc->new_n_r>>1, poll_final_bit);
	} else {			 /* must be unnumbered */
		buginf("\nLLC: i %s", llc2_prevent(newevent));
	}
    }

	/* 
	 * The fact that the C language definition states that a true 
	 * result to a test evaluates to 1 is relied upon in llstate.c
	 * to set the F bit in RESPONSE frames
	 */
	llc->need_f_bit = (responsebit == 0) && (poll_final_bit != 0);
	llc->f_bit_set  = (responsebit != 0) && (poll_final_bit != 0);

	/*
	 * Clear retry count as described in IEEE802.2-1985, Section 7.5.9
	 * Condition is:
	 *               Valid I or S-format PDU
	 *               Final bit (in Response-type frame)
	 *               Received N(r) is valid
	 * Note that 1st comparison depends on ordering of events
	 * in llc2.h...
  	 */
	if ((newevent >= REC_I_CMD && newevent <= REC_REJ_RSP) &&
	    (llc->f_bit_set) &&	/* computed above */
	    (!llc->inval_n_r)	/* computed in eval_n_r() */
	    )
	{
	    llc->retry_count = 0;
	}
	llc2_stateswitch(llc, newevent);

    /*
     * Busy the LLC2 connection if the SDLC half of the 
     * SDLLC link is not up yet. (or off ;)
     */
    if ((!isClsSAP) &&
	(newevent == REC_SABME_CMD && 
	 (pak->flags & PAK_LACK_IN) &&
	 llc->state >= LLS_NORMAL && 
	 llc->rnr_disable == FALSE)) {
	if (llc->sdllc_active) {
	    if (llc->sdllc_idb->enctype == ET_SDLCS)
		llc2_stateswitch(llc, SET_LOCAL_BUSY);
	} else if (llc->lnx_active) {
	    ;				/* don't have to set local busy */
	} else {
	    llc2_stateswitch(llc, SET_LOCAL_BUSY);
	}
    }

    conch = *(frameptr + 2) & 0xEF;   /* snuff out p/f bit */
    pak->llc2_cb = llc;
    
    if (llc->inbuf) {
	protocol_discard(llc->inbuf, TRUE);
	llc->inbuf = NULL;
    }

    /* now check if anything cropped up which needs doing */

    if (llc->txaction != 0) {
	llc->event = LOCK_TX_ACTION;
	llc2_examaction(llc);
    }
    
}

/*
 * fast_llc2_input
 * Just like llc2_input but faster, man.
 * Assume l2st_connect state and the packet is an I-frame.
 * The caller dispenses with the packet.
 * Returns TRUE if the caller should forward the frame.
 */
boolean fast_llc2_input (paktype *pak, llctype *llc)
{
    int newevent;
    int responsebit, poll_final_bit;
    int framelen;
    idbtype *idb;
    sap_hdr *llc1;

    if (llc2_debug_packet)
	hexout(pak->datagramstart, pak->datagramsize, "fast_llc2_input");

    idb = pak->if_input;
    if (!llc2_running || !interface_up_simple(idb))
	return(FALSE);

    if (clsIsPakCls(pak))
	return(FALSE);		/* sorry, no CLS on this path */

    GET_TIMESTAMP(idb->hwptr->lastinput); /* remember time of last input */
    GET_TIMESTAMP(pak->inputtime);	/* save it in packet too */

    framelen = pak->datagramsize - pak->llc2_sapoffset;
    if (framelen < 4 || framelen > LLC2_MAX_ILEN) 
	return(FALSE);

    llc1 = (sap_hdr *) pak->info_start;
    
    responsebit = llc1->ssap & 1;
    newevent = REC_I_CMD + responsebit;	/* Either REC_I_CMD or REC_I_RSP */

    llc->inbuf = pak;

    if (llc != llc2stationptr)
	llc2_starttimer(llc, IDLE_TIMER, llc->llc2_idle_time);

    poll_final_bit = llc1->data[0] & 1;

    /* XXX -- this probably should be done a bit earlier. */
    llc->new_n_r = llc1->data[0] & 0xfe;
    eval_n_r(llc);
    if (llc->inval_n_r)
	return(FALSE);
    llc->new_n_s = llc1->control & 0xfe;
    if (llc->v_r != llc->new_n_s)
	return(FALSE);

    llc->p_f_val = poll_final_bit;
    llc->cmdrspbit = responsebit;

    /*
     * Display the packet if debugging
     */
    if (llc2_debug_packet) {
	buginf("\nFASTLLC: i %s N(R)=%d N(S)=%d V(R)=%d p/f=%d",
	       llc2_prevent(newevent), llc->new_n_r >> 1,
	       llc->new_n_s >> 1, llc->v_r >> 1, poll_final_bit);
    }

    llc->need_f_bit = !responsebit && poll_final_bit;
    llc->f_bit_set = responsebit && poll_final_bit;

    /*
     * Clear retry count as described in IEEE802.2-1985, Section 7.5.9
     * Condition is:
     *               Valid I or S-format PDU
     *               Final bit (in Response-type frame)
     *               Received N(r) is valid
     */
    if (llc->f_bit_set)
	llc->retry_count = 0;
    llc2_stateswitch(llc, newevent);
    if (LLC2_DEBUG_ERR && !(pak->flags & PAK_LACK_OUT))
	buginf("\nLLC: llc2_stateswitch rejected packet in fast-path");
    llc->need_f_bit = 0;

    pak->llc2_cb = llc;

    if (llc->inbuf) {
	/*
	 * LLC2 unhappy, assume it didn't set PAK_LACK_OUT
	 */
	llc->inbuf = NULL;
    }

    /* now check if anything cropped up which needs doing */

    if (llc->txaction != 0) {
	llc->event = LOCK_TX_ACTION;
	llc2_examaction(llc);
    }

    /* XXX What if the flag is not set -- the frame will wind up
     *     getting processed through the state machine twice
     *     (once here, and once when it hits the slow path)
     *     which seems a trifle brain-damaged...
     */
    return(pak->flags & PAK_LACK_OUT);
}


/* 
 * find_in_event
 * Returns the event caused by the incoming frame
 */
static int find_in_event (uchar *lsapptr, int framelen)
{
    uchar *frameptr;
    uchar conch;
    int responsebit;

    frameptr = lsapptr;
    conch = *(frameptr+2);		/* control */
    responsebit = *(frameptr+1) & 1;	/* c/r from SSAP */

    if ((conch & LLC_IMASK) == LLC_IFRAME) {		/* if I frame */
	if (framelen < 4) 
	    return(REC_TOO_SMALL);
	if (framelen > LLC2_MAX_ILEN) 
	    return(REC_INV_ILEN);
	return(REC_I_CMD + responsebit); /* REC_I_RSP = REC_I_CMD + 1 */
    }

    if ((conch & LLC_SMASK) == LLC_SUPERVISORY) {	/* if supervisory frame */
	  if (framelen != 4) {		                /* if invalid length */
	    if (framelen < 4) 
		return(REC_TOO_SMALL);
	    else 
		return(REC_INV_IFIELD);
        }
	if (conch == RR) 
	    return(REC_RR_CMD + responsebit);
	if (conch == RNR) 
	    return(REC_RNR_CMD + responsebit);
	if (conch == REJ) 
	    return(REC_REJ_CMD + responsebit);
	return(REC_BAD_CMD + responsebit);
    }

    /* Here we have an Unnumbered command */

    conch &= ~P_BIT;			/* remove poll/final bit */

    if (framelen < SAP_HDRBYTES) 
	return(REC_TOO_SMALL);

    if (responsebit == 0) {		/* if command */
	if (conch == SABME) {
	    if (framelen > SAP_HDRBYTES) 
		return(REC_INV_IFIELD);
	    return(REC_SABME_CMD);
        }
	if (conch == DISC) {
	    if (framelen > SAP_HDRBYTES) 
		return(REC_INV_IFIELD);
	    return(REC_DISC_CMD);
        }
	if (conch == XID) 
	{
	    
	    if (framelen < 6)
		return(REC_TOO_SMALL);
	    if ((*frameptr+3) != XID_FMT_STD)
		return(REC_BAD_CMD);
	    return(REC_XID_CMD);
	}
	if (conch == LLC_TEST) 
	    return(REC_TEST_CMD);
	return(REC_BAD_CMD);
    }

    /* Must be a response */

    if (conch == UA) {
	if (framelen > SAP_HDRBYTES) 
	    return(REC_INV_IFIELD);
	return(REC_UA_RSP);
    }
    if (conch == DM) {
	if (framelen > SAP_HDRBYTES) 
	    return(REC_INV_IFIELD);
	return(REC_DM_RSP);
    }
    if (conch == FRMR) {
	if (framelen > 8) 
	    return(REC_INV_ILEN);
	return(REC_FRMR_RSP);
    }
    if (conch == XID) 
	return(REC_XID_RSP);
    if (conch == LLC_TEST) 
	return(REC_TEST_RSP);
    return(REC_BAD_RSP);
}


/*
 * eval_n_r
 */
static void eval_n_r (llctype *llc)
{
    int n_r_copy;

    n_r_copy = llc->new_n_r;
    if (llc->v_s >= llc->last_n_r) {
	llc->inval_n_r = (n_r_copy < llc->last_n_r) || (n_r_copy > llc->v_s);
	return;
    }
    llc->inval_n_r = (n_r_copy < llc->last_n_r) && (n_r_copy > llc->v_s);
}


/*
 * eval_n_s
 */
static void eval_n_s (llctype *llc)
{
    int v_r_copy,n_s_copy;
#ifdef notdef
    register int windowedge;
#endif

    v_r_copy = llc->v_r;
    n_s_copy = llc->new_n_s;

    llc->inval_n_s = 0;
    llc->unexp_n_s = 0;
    if (v_r_copy == n_s_copy) 
	return;

#ifdef notdef
    /*
     ***** ? *****
     * This code assumes that the valid window is from N(R) to N(R) + window.
     * It really must include the previous windowsize frames too.
     * For now, don't ever set inval.  Think about this more later,
     * look at the book, etc.
     */
    /* an unexpected frame is one between v_r and v_r +window */
    windowedge = (v_r_copy + llc->loc_wind) & 255;
    if (windowedge > v_r_copy) {
	llc->unexp_n_s = (n_s_copy > v_r_copy) && (n_s_copy < windowedge);
    }
    else {
	llc->unexp_n_s = (n_s_copy > v_r_copy) || (n_s_copy < windowedge);
    }
    llc->inval_n_s = !llc->unexp_n_s; /* if not unexpected then invalid */
#else
    llc->unexp_n_s = 1;
#endif
}

/*
 * llc2call 
 */
void llc2call (llctype *llc)
{
    leveltype level;

    if (llc != NULL) 
    {
	level = raise_interrupt_level(NETS_DISABLE);
	llc2_examaction(llc);
	reset_interrupt_level(level);
    }
}


/*
 * llc2_init_idb
 */
void llc2_init_idb (idbtype *swidb, boolean setup_hwidb)
{
    llc_sb_t *llc_sb = llc_create_sb(swidb);
    if (!llc_sb) {
      printf(nomemory);
    }
}

/*
 * llc2_show_default
 *
 * "show llc2" default action.  Overridden by X25 CONS action when X25
 * is initialized.
 */

static void llc2_show_default (llctype *llc2)
{
}

/*
 * llc2_init.
 */
void llc2_init (subsystype *subsys)
{
    int       i;
    idbtype   *idb;
    llctype   *llc;

    llc2_debug_init();

    llc2_running = FALSE;
    llc2_debug_packet = FALSE;
    llc2_debug_dynwind = FALSE;
    llc2_debug_test = FALSE;
    GET_TIMESTAMP(llc2_nexttime);

    llc_in_hash_count = 0;
    for (i = 0; i < LLC_DTE_HASH_SIZE; i++)
	llc_dte_hash_table[i] = NULL;


    queue_init(&llc2_sabmeQ, 0);
    queue_init(&llctQ, 0);
    queue_init(&llc2_cleanupQ, 0);
    queue_init(&llc2_freelistQ, 0);

    init_test_command();    

    llc2_on_idb_override = (boolean)subsys_find_by_name("appn");

    /*
     * Allocate a static block used for responding to unconnected
     * circuits, etc.  Some things like the timer defaults won't
     * get initialized.  This shouldn't matter since we never
     * put this llc on in the hash table.
     */
    llc = malloc_named(sizeof(llctype), "LLC CB");
    if (llc == NULL) {
	return; 
    }
    llc->q_elem.q_context = llc;
    llc2stationptr = llc;
    llc->linktype = LINK_LLC2;
    llc->state = LLS_NONE;
    /*
     * Preset values in idbtypes
     */
    FOR_ALL_SWIDBS(idb)
	llc2_init_idb(idb, TRUE); /* CAC (TRUE is unused parameter) */

    /*
     * Preset values in LLC2 Registration queue
     */
    for (i = 0; i < 128; i++) {
	llc2_registration[i].proc = NULL;
	llc2_registration[i].link = 0;
    }

    /*
     * Register some functions.
     */
    llc2_media_registry();
    reg_add_sw_idb_init(llc2_init_idb, "llc2_init_idb");
    reg_add_hwif_state_change(llc2_cstate, "llc2_cstate");
    reg_add_llc2_input(llc2_input, "llc2_input");
    reg_add_SNA_Handle_SNA_XID(SapSNAHandleSnaXid, "SapSNAHandleSnaXid");
    reg_add_default_media_llc_show(llc2_show_default, "llc2_show_default");
    reg_add_dlc_wput(CLS_LLC, llc_wput, "llc_wput");
    reg_add_dlc_wput(CLS_FRAME_RELAY, llc_wput, "llc_wput");
    reg_add_dlc_wput(CLS_ATM_1483, llc_wput, "llc_wput");
    reg_add_dlc_wput(CLS_PPP, llc_wput, "llc_wput");
    reg_add_dlc_wput(CLS_TBRIDGE, llc_wput, "llc_wput"); /* Move to DLSw init */
    reg_add_determine_fair_queue_flow_id(LINK_LLC2, llc2_fair_queue_flow_id,
					 "llc2_fair_queue_flow_id");
    reg_add_proto_on_swidb(LINK_LLC2, llc2_on_idb,  "llc2_on_idb");

    /*
     * Register LLC2CP with PPP
     */
    reg_invoke_ncp_register(LINK_LLC2, TYPE_PPP_LLC2CP,
			    NULL, NULL, "LLC2");
}


/*
 * llc2BLOCK
 * Scheduler test for the llc2 timer process
 */
boolean llc2BLOCK (void)
{
    if ((llc2_sabmeQ.qhead != NULL)   || /* if a sabme to process */
	(llc2_cleanupQ.qhead != NULL) || /* if cleanup needed */
	(!SLEEPING(llc2_nexttime)))
	return FALSE;

    return TRUE;			/* still sleeping */
}

static void llc_freeup (llctype *llc)
{
    p_enqueue(&llc2_freelistQ, &llc->q_elem);
}

/*
 * llc2_timer
 * Handle llc2 timers and deferred processing.
 */
forktype llc2_timer (void)
{
    llctype *llc;

    edisms(systeminitBLOCK, 0);

    for (;;) {
	edisms((blockproc *)llc2BLOCK, 0L);

	/*
	 * Process the cleanup queue.
	 * We got put on the cleanup queue by goto_adm().
	 * We need to stay here if another task is blocked waiting
	 * on us.  Otherwise, the scheduler will be looking at 
	 * an invalid llc.  We shouldn't be here more than 1
	 * pass since llc2_openBLOCK() will notice that there 
	 * is an adm_reason set.
	 */
	/***** don't run forever *****/
	while (llc2_cleanupQ.qhead) {
	    llc = ((LlcQElement *) llc2_cleanupQ.qhead)->q_context;

	    /*
	     * Make sure adm_reason is set, otherwise we could 
	     * wait forever if open_block is set.
	     */
	    if (llc->adm_reason == 0) {
		llc->adm_reason = LC_TIMEOUT;
		if (LLC2_DEBUG_STATE || LLC2_DEBUG_ERR) {
		    buginf("\nLLC: reason was zero in cleanup queue");
		}
	    }

            if (llc->llc_xid_pak) {
                  datagram_done(llc->llc_xid_pak);
                  llc->llc_xid_pak = NULL;
                  llc2_stoptimer(llc, LLC1_TIMER_M);
	    } 

	    if (llc->open_block == LS_WAITING) {
		break;
	    }

	    (void) p_dequeue(&llc2_cleanupQ);

	    /*
	     * Tell someone that we're going away
	     */
	    if (LLC2_DEBUG_STATE || LLC2_DEBUG_ERR)
		buginf("\nLLC: cleanup %s, %s (%d)",
		       llc2_prid(llc), llc2_prreason(llc->adm_reason),
		       llc->adm_reason);
	    if (llc->statusproc)
		(*llc->statusproc)(llc, llc->adm_reason);
	    llc_freeup(llc);
	}

	/*
	 * Process timer functions
	 */
	llc2_tick();
    }
}


/*
 * get_llctype
 */
llctype *get_llctype (void)
{
    llctype *llc;
    LlcQElement * qelem_p = p_dequeue(&llc2_freelistQ);

    if (qelem_p == NULL) {
	if (llc2_debug_err) {
	    buginf("\nget_llctype = NULL");
	}
	llc = NULL;
    }
    else {
	llc = qelem_p->q_context;
    }

    if (llc2_freelistQ.count < (LLC_DTE_HASH_SIZE / 2)) {
	critical_background_wake();
    }

    return (llc);
}

/********************************
 *         Hash routines        *
 ********************************/

/*
 * llc_by_dte
 * Find entry in the dte hash table given the dte string and interface
 */
llctype *llc_by_dte (uchar   *srcdtestring, 
		     int     lsap,		/* -1 means wildcard */
		     int     rsap,		/* -1 means wildcard */
		     idbtype *idb)
{
    int       n;
    llctype   *llc;
    leveltype level;

    n = llc_hash_dte(srcdtestring);
    level = raise_interrupt_level(ALL_DISABLE);
    llc = llc_dte_hash_table[n];
    while (llc != NULL) {
	if ((bcmp(llc->llc2_dte, srcdtestring, LLC_MAX_DTE_SIZE) == 0) && 
	    (idb == llc->idb) &&
	    ((lsap == -1) || (lsap == llc2_get_lsap(llc))) &&
	    ((rsap == -1) || (rsap == llc2_get_rsap(llc)))) {
	    reset_interrupt_level(level);
	    return llc;
	}
	llc = llc->next_hash;
    }
    reset_interrupt_level(level);
    return (llctype *)NULL;
}


/*
 * llc_enter_dte_hash
 * Enter an llc into the dte hash table.
 */

void llc_enter_dte_hash (llctype *llc)
{
    int       n;
    leveltype level;
    sap_entry_t* sap_entry;

    if (llc->status_flags & LLCSF_IN_HASH) {
	if (LLC2_DEBUG_ERR) {
	    buginf("\nLLCTYPE enetered multiple times in hash table");
	}
	return;
    }

    sap_entry = sap_get_entry(llc->idb, llc2_get_lsap(llc));
    llc->state = LLS_ADM;
    n = llc_hash_dte(llc->llc2_dte);

    level = raise_interrupt_level(NETS_DISABLE);
    llc->next_hash = llc_dte_hash_table[n];
    llc_dte_hash_table[n] = llc;
    llc->status_flags |= LLCSF_IN_HASH;
    llc_in_hash_count++;
    if (sap_entry != NULL) {
	llc->llc_sap_link = sap_entry->llc_list;
	sap_entry->llc_list = llc;
    }
    reset_interrupt_level(level);
}


/*
 * llc_remove_dte_hash
 * Remove an entry from the dte hash table.
 */
void llc_remove_dte_hash (llctype *target)
{
    int       n;
    llctype   *llc, **prev;
    leveltype level;
    sap_entry_t* sap_entry;

    if ((target->status_flags & LLCSF_IN_HASH)) {

	level = raise_interrupt_level(ALL_DISABLE);
	n = llc_hash_dte(target->llc2_dte);
	sap_entry = sap_get_entry(target->idb, llc2_get_lsap(target));

	prev = &llc_dte_hash_table[n];
	llc = *prev;
	while (llc) {
	    if (llc == target) {
		*prev = llc->next_hash;
		llc->next_hash = (llctype *)NULL;
		llc_in_hash_count--;
		break;
	    }
	    prev = &llc->next_hash;
	    llc = llc->next_hash;
	}
	/* Its gone, or it ws never there... */
	target->status_flags &= ~LLCSF_IN_HASH;
	if (sap_entry != NULL) {
	    llc = sap_entry->llc_list;
	    if (llc == target) {
		sap_entry->llc_list = llc->llc_sap_link;
	    } else {
		/*
		 * This could potentially be a long walk down the list.
		 * Plan to change to a better data structure with fast deletes.
		 * [sberl]
		 */
		while (llc != NULL) {
		    if (llc->llc_sap_link == target) {
			llc->llc_sap_link = target->llc_sap_link;
			break;
		    } else {
			llc = llc->llc_sap_link;
		    }
		}
	    }
	    target->llc_sap_link = NULL;
	}
	reset_interrupt_level(level);
    }
}


/*
 * validate_llc
 * Verify that an llc exists
 */
boolean validate_llc (llctype *target)
{
    int       n;
    llctype   *llc;
    leveltype level;

    n = llc_hash_dte(target->llc2_dte);
    level = raise_interrupt_level(ALL_DISABLE);
    llc = llc_dte_hash_table[n];
    while (llc) {
	if (llc == target) {
	    reset_interrupt_level(level);
	    return TRUE;
	}
	llc = llc->next_hash;
    }
    reset_interrupt_level(level);

    return FALSE;
}


/********************************
 * External interface routines. *
 ********************************/

/*
 * llc2_register
 *
 * Called from a process to register for an llc2 SAP.  Any incoming calls
 * to this sap will cause an upcall to the specified routine with the
 * argument of the cookie.
 */
boolean llc2_register (int sap, int linktype, void (*notifproc)(llctype *))
{
    if (llc2_registration[sap/2].proc)
	return(FALSE);
    llc2_registration[sap/2].link = linktype;
    llc2_registration[sap/2].proc = notifproc;
    return(TRUE);
}

      
/*
 * llc2_open1
 *
 * don't do an actual open, just allocate resources.
 */
llctype *llc2_open1 (
		       int dsap, int ssap,
		       hwaddrtype *destMac,		/* destination address */
		       hwaddrtype *srcMac,		/* source address */
		       idbtype *idb,
		       int linktype,
		       int wsize,				/* window size */
		       void (*statusproc)(llctype *, int),	/* status notification proc */
		       boolean blocking,			/* true if blocking open */
		       boolean local_ack,			/* true if local ack for rsrb */
		       boolean clsi_call,			/* true if invoked from ReqOpenStn */
		       uchar l3pid,	                        /* Network layer protocol id */
				                         	/* for Frame Relay/RFC1490 only */
                       ushort Dlci)                            /* for BAN frame-relay encap ONLY*/

{
    llctype  *llc;
    uchar    dtestring[LLC_MAX_DTE_SIZE];
    hwaddrtype src_addr;
    hwaddrtype dest_addr;

    if (idb == NULL)
	return((clsi_call) ?	        /* signal error condition */
	       (llctype*)LLC2_LlcBadIdb	/* CLS signal */
	       : NULL);		/* don't break legacy code... ;) */
    
    
    /* Build the dte string for this set of addresses */
    memset(dtestring, 0, LLC_MAX_DTE_SIZE);
    llc_clear_rii(destMac, &dest_addr, idb);
    llc_clear_rii(srcMac, &src_addr, idb);
    llc2_build_dte_string(dtestring, &dest_addr, &src_addr, dsap, ssap, l3pid);
    
    /*
     * Check if llc is in use by someone else.  Legacy code looks to see if
     * found llc is in non-ADM state.  CLS code doesn't care, any LLC collision
     * in any state is an error...
     * Legacy code only supports NULL and non-NULL return codes for failure
     * and non-failure conditions.  Preserve this operation for legacy code,
     * but CLS requires information as to why LLC operation didn't ocmplete
     */
    llc = llc2_findcirc(dtestring, idb);
    if (llc != NULL) {
	if (llc->state != LLS_ADM || clsi_call)  {
  	    if (LLC2_DEBUG_ERR)
  		buginf("\nLLC: llc2_open1: dte %s %s already in use, aborting",
  		       llc2_prid(llc), llc2_prstate(llc, 0));
	    return((clsi_call) ?         /* signal already in use */
		   (llctype*)LLC2_LlcInUse /* CLS signal */
		   : NULL);	         /* don't break legacy code... ;) */
  	}
    } else {
	llc = llc2_findfree(ssap, idb);
  	if (llc == NULL) {
	    if (LLC2_DEBUG_ERR)
		buginf("\nLLC: llc2_open1: %e %e %02x %02x no memory for llc2 structure",
		       &dtestring[0], &dtestring[8],
		       dtestring[RSAP_DTE], dtestring[LSAP_DTE]);
	    return((clsi_call) ?         /* signal allocation failure */
		   (llctype*)LLC2_LlcNoMem /* CLS signal */
		   : NULL);	           /* don't break legacy code... ;) */
	}
	
	bcopy(dtestring, llc->llc2_dte, LLC_MAX_DTE_SIZE);
        if (snpa_get_type(&dest_addr) == STATION_FR10)
            llc->llc2_dte[FR_L3_PID_DTE] = l3pid;

        if (Dlci && (l3pid == CLS_BAN_FLAG)) {
            llc->fDlci = Dlci;                 /* for BAN implementation */
            llc->llc2_addrtype = STATION_FR10;
	} else { 
            llc->llc2_addrtype = snpa_get_type(destMac);
            llc->fDlci = 0;
	}
	
        if (local_ack) {
	    llc->lack_llc2_open_ok = TRUE;
	}
	llc->idb = idb;
	llc_enter_dte_hash(llc);
	llc->state = LLS_ADM;
    }
    
    llc->linktype = linktype;
    llc->idb = idb;
    llc->loc_wind = wsize<<1;		/* our window size */
    llc->llc1_timer = LLC1_T_DEFAULT;
    llc->statusproc = statusproc;
    llc->blocking = blocking;
    llc->rsrb_lack = local_ack;
    llc->l2_lakptr = NULL;

    /*
     * Initialize WFQ parameters for this llc
     *   (provided WFQ has been configured for the interface)
     */
    if (idb->hwptr->fair_queue_head) {
        llc2_init_fair_sum(idb, llc);
        llc->llc2_fair_sequence = LLC2_FAIR_SEQ_DEFAULT;
    }

    /*
     * Segregate CLS and older code initialization differences here!
     *
     * Other llc2 initialization also takes place in llc2_handle_sabme(),
     * so be sure to keep initializations there current...
     *
     * CLSI needs to allocate/initialize llc structure without
     * sending SABME (llc2ReqOpnStnReq()).
     * SABME will be sent later in llc2ConnectReq().
     * Clearing CONN_R_BIT prevents background (llc2_tick()) from
     * sending initial SABME.
     * However, set the bit so older code behaves as before...
     */
    if (clsi_call)
    {
	llc->txaction &= ~CONN_R_BIT;
	llc->u_cep_id = (void*) CLS_NO_CEP;
	/*
         * Disable tx of XIDs in the middle of a session.
	 */
	llc->llc2_xid_rtry_time = 0;
	

	/* Start the ADM timer. At this point, we will time out after
	 * a minute if we don't get anything else
	 */

	if (llc->llc2_adm_timer_value > 0) {
	    llc2_starttimer(llc, ADM_TIMER, llc->llc2_adm_timer_value);
	}

	CepUpdate(&llc->fCep, NULL, 0, 
		  LLC_OQUEUE_LOW_WATER_MARK, LLC_OQUEUE_HIGH_WATER_MARK, 
		  LLC_CLS_FLOW_IND_DAMP);
    }
    else
    {
	llc->txaction = CONN_R_BIT;
	llc->u_cep_id = NO_CLS;	
    }
    
    return(llc);
}
  
llctype *llc2_open2 (llctype *llc, boolean blocking)
{
    if (llc == NULL)
	return(NULL);
    llc2call(llc);

    /*
     * If non-blocking, return immediately, else wait for something
     * to happen.
     */
    if (!blocking)
	return(llc);

    if (!llc->open_block) {
	llc->open_block = LS_WAITING;	/* this llc is blocking another task */
	edisms((blockproc *)llc2_openBLOCK, (ulong) llc);
    }
    if (llc->open_block == LS_SUCCESS)
	return(llc);
    else
	return(NULL);
}

/*
 * llc2_open
 * Called from a process to open an llc2 circuit.
 * Blocks until circuit is open or fails.
 * Return NULL upon failure.
 */
llctype *llc2_open (
    int dsap, int ssap,
    hwaddrtype* destMac,		/* destination address */
    hwaddrtype* srcMac,			/* source address */
    idbtype *idb,
    int linktype,
    int wsize,				/* window size */
    void (*statusproc)(llctype *, int),/* status notification proc */
    boolean blocking,			/* true if blocking open */
    boolean local_ack)                 /* true if local ack for rsrb */
{
    llctype *temp;

    if (reg_invoke_local_lnm_llc2_open (idb, (llctype **)&temp))
      return (temp);

    return(llc2_open2
	   (llc2_open1
	    (dsap, ssap, destMac, srcMac, idb, linktype, wsize,
	     statusproc, blocking, local_ack, FALSE, NULL_L3_PID,NULL_DLCI), blocking));
}


/*
 * llc2_attach
 * Finds an existing llc2 circuit and set parameters for it.
 * Return FALSE upon failure.
 */
boolean llc2_attach (llctype *llc, int linktype, int wsize, /* windowsize */
    void (*statusproc)(llctype *, int))	/* status notification proc */
{
    if (!validate_llc(llc) || (llc->state == LLS_ADM))
	return FALSE;

    llc->linktype = linktype;
    llc->loc_wind = wsize<<1;		/* our window size */
    llc->statusproc = statusproc;

    return TRUE;
}


/*
 * llc2_close - close a connection.
 * Returns FALSE if error, else TRUE.
 */

int llc2_close (llctype *cookie)
{
    llctype *llc;
    
    /* Check for local lnm request */
    if (reg_invoke_local_lnm_llc2_close (cookie)) {
        return (DLC_RS_PENDING);
    }

    /*
     * Verify this is a valid llc
     */
    llc = cookie;
    if (!llc)
	return(DLC_RS_FAILED);
    if (!validate_llc(llc)) {
	  /*
	   * This can happen and not be an error because of Lack2
	   * Needs to be thought through.
	   */
	  if (LLC2_DEBUG_ERR)
	      buginf("\nLLC: attempt to close with invalid llc=0x%x", llc);
	  return(DLC_RS_FAILED);
      }
    
	  /*
     * LLC2 connections under CLS control are not deleted until explicitly informed
     * by the DLU...
     */
    if (!LLC2isCls(llc) && llc->state == LLS_ADM) {		/* if not connected */
	  /*
	   *  Invoke the special cleanup necessary for this llc.  If lack, lnx, or sdllc
           *  is involved in this llc, it will have set its special cleanup routine
           *  in the llc->media_llc_cleanup vector.  If there is no special cleanup
           *  routine set, none will be called.
	   */
          if (llc->media_llc_cleanup != NULL) {
             (*llc->media_llc_cleanup) (llc);
             llc->media_llc_cleanup = NULL;
	  }
	  llc_remove_dte_hash(llc);
	  if ((!llc->q_elem.q_next) &&
	      (llc2_cleanupQ.qtail != &llc->q_elem)) {
	      p_enqueue(&llc2_cleanupQ, &llc->q_elem);
	  }
	/*
	 * we always return PENDING.  This means that we always
	 * let the cleanup do the indicate above if needed.
	 */
	return(DLC_RS_PENDING);	/* "memory" cleanup, no network activity required for close */
      }
    
    llc->txaction |= DISC_R_BIT;
    llc2call(llc);
    
    return(DLC_RS_PENDING);
}

/*
 * llc_get_oqueue_status()
 */

dlc_oqueue_status_t llc_get_oqueue_status(llctype *llc)
{
    /* Check if it is a local lnm dlc2 oqueue status */
    if (reg_invoke_local_lnm_llc2_oqueue_status(llc)) {
        return (DLC_OQUEUE_NORMAL);
    }

    return (CepGetOqueueStatus(&llc->fCep));
}

/*
 * llc2_send
 * Convenience routine to send an I frame.
 * Entry: pak->datagramstart = beginning of data
 *        len = length of pak starting after encaps bytes
 * Returns: FALSE if failure, pak disposed of.
 */
void llc2_send (llctype *llc, paktype *pak,
		   int len /* length of data */)
{
    uchar    *savestart;
    
    /* Check if it is a local lnm llc2 send request */
    if (reg_invoke_local_lnm_llc2_send(llc, pak)) {
        return;
    }

    /*
     * Verify this is a valid llc
     */
    if (!validate_llc(llc)) {
        if (llc2_debug_err)
            buginf("LLC-2-UNEXPECT: LLC2: validate_llc UNEXPECTED EVENT");
        datagram_done(pak);
	return;
    }

    /*
     * For rsrb local ack, the packet header has already been formed.
     * llc2 needs to only adjust the seq. numbers in the llc2 header.
     * send_i_xxx does that.
     */
    pak->if_output = llc->idb;
    pak->linktype = llc->linktype;
    pak->llc2_cb = llc;

    if ((!llc->rsrb_lack) || (llc->sdllc_active || llc->lnx_active)) {
      /* Add 4 bytes for I frame */
      pak->datagramstart -= 4;
      savestart = pak->datagramstart;

      if (!llc_encapsulate(pak, LLC_IFRAME, len,llc)) {
           datagram_done(pak);
           return;
      }

      /*
       * Remember the offset to the first SAP.
       */
      pak->llc2_sapoffset = savestart - pak->datagramstart;
    }
    if (LLC2isCls(llc)) {
	pak->flags |= PAK_LACK_OUT;
    }

    if (llc->rsrb_lack || LLC2isCls(llc)) {
      pak->flags &= ~PAK_SRB_OUT;
      pak->flags |= PAK_SRB_EXPLORER; /* do the right thing!!! */
    }
    llc_output_inline(pak, LLC_IFRAME);
}


/*
 * llc2_find_cb
 * Find an existing llc control block given destination mac
 * address, lsap, dsap, and idb.  lsap and/or dsap may
 * be ignored by setting them to -1.
 */
llctype *llc2_find_cb (uchar *mac,
		       int lsap,		/* -1 means wildcard */
		       int dsap,			/* -1 means wildcard */
		       idbtype *idb)
{
    uchar dtestring[LLC_MAX_DTE_SIZE];
    /* 
     * Place SAP values in mac array 
     * otherwise llc_hash_dte will fail
     * on dte with just MAC.
     */
    memset(dtestring, 0, LLC_MAX_DTE_SIZE);
    ieee_copy(mac, dtestring);
    dtestring[LSAP_DTE] = lsap;
    dtestring[RSAP_DTE] = dsap;
    return (llc_by_dte (dtestring, lsap, dsap, idb));
}


/*
 * llc2_openBLOCK
 * Wait for open event to occur
 */
boolean llc2_openBLOCK (llctype *llc)
{
    if ((llc->open_block != LS_WAITING)	|| (llc->adm_reason)) {
	if (llc->adm_reason)
	    llc->open_block = 0;	/* clear block */
	return(FALSE);
    }

    return(TRUE);			/* continue to block */
}


#ifdef CLAUDE_KILL_ME
replaced by a whole lotta function in 9.1(8) code...

/*
 * llc2_reason
 * Return text string for indicate reason
 */
char *llc2_reason (int reason)
{
    switch (reason) {
    case LC_CONNECT:   
    case LC_CONCNF:    
	return "connect";
    case LC_DATA:      
    case LC_DATACNF:   
	return "data";
    case LC_DISC:      
    case LC_DISCNF:    
	return "disconnect";
    case LC_RESET:     
    case LC_RSTCNF:    
	return "reset";
    case LC_REPORT:    
	return "report";
    case LC_FLOWCONTROL: 
	return "flowcontrol";
    case LC_TIMEOUT:
	return "timeout";
    case LC_INTDOWN:
	return "interface down";
    default:
	return "unknown";
    }
}
#endif


/*
 * llc2_add_llctype
 */
static void llc2_add_llctype (void)
{
    leveltype level;
    llctype   *llc;
    int i;

    level = raise_interrupt_level(ALL_DISABLE);
    if (llc2_freelistQ.count <= (LLC_DTE_HASH_SIZE / 2)) {
	for (i = llc2_freelistQ.count; i < LLC_DTE_HASH_SIZE; i++) {
	    llc =  malloc_named(sizeof(llctype), "LLC CB");
	    if (llc ==  NULL)
		break;
	    llc->q_elem.q_context = llc;
	    enqueue(&llc2_freelistQ, &llc->q_elem);
	}
    }
    reset_interrupt_level(level);
}

/*
 * llc2_start
 * Called by higher levels (IBMNM, CONS, etc) to make sure LLC2 is running.
 */
boolean llc2_start (void)
{
    static int started = FALSE;

    if (started)
	return TRUE;
    started = TRUE;
    
    (void) cfork (llc2_timer, 0L, 1500, "LLC2 Timer", 0);
    reg_add_critical_background(llc2_add_llctype, "llc2_add_llctype");
    critical_background_wake();

    llc2_running = TRUE;

    return TRUE;
}


/****************************
 * Packet transmit routines *
 ****************************/

/*
 * llc_encapsulate
 *
 * Return FALSE if encapsulation failure.  The 'length' parameter is the
 * number of bytes *after* the llc header.
 */
boolean llc_encapsulate (paktype *pak, int type, int length, llctype *llc)
{
    idbtype *idb;
    vencap_t vencap;

    if ((idb = pak->if_output) == NULL)
	return FALSE;

    switch (type) {
      case LLC_UNNUMBERED:
        pak->llc2_enctype = ET_LLC2;
	length += SAP_HDRBYTES;
	break;

      case LLC_SUPERVISORY:
      case LLC_IFRAME:
        pak->llc2_enctype = ET_LLC2;
	length += SAP_LONGHDRBYTES;
	break;

      default:
        pak->llc2_enctype = ET_SAP;
	length += SAP_HDRBYTES;
	break;

    }

    /*
     * pak->datagramstart should already be set to the beginning of the llc
     * data. The interface encaps routine will add the mac header.
     */
    pak->datagramsize = length;
    vencap = idb->hwptr->vencap;
    if (vencap) {
        if (llc->fDlci) { 
            /* fras ban encapsulation */ 
            pak->llc2_enctype = ET_BRIDGE;  
	    return ((*vencap)(pak, (long)llc));
	} else { 
            return ((*vencap)(pak, (long)llc->llc2_dte));
	}
    }
    return FALSE;
}

/********************************
 *  Member access functions     *
 ********************************/
inline lak_t *llc2_get_lackie(llctype *llc) {
    return(llc->l2_lakptr);
}

/*
 * llc_output
 *
 * Keep this around for x25_cons to call. When CONS starts to use
 * CLSI, then it can go away.
 */
void llc_output (paktype *pak, int type)
{
    llc_output_inline(pak, type);
}

/*
 * llc2_transmit
 *
 * Encapsulate then enqueue an llc2 packet for transmission.  The 'size'
 * parameter is the number of bytes *after* the llc header.
 */
void llc2_transmit (llctype *llc, paktype *pak,
		    int size,	/* size of data, not encaps */
		    int type)
{
    if (llc->idb == NULL) {
	  buginf("\nLLC: llc2_transmit: %s llc->idb NULL",
		 llc2_prid(llc));
	  datagram_done(pak);
	  return;
      }
    
    pak->if_output = llc->idb;
    pak->linktype = llc->linktype;
    pak->llc2_cb = llc;

    if (llc->rsrb_lack  || LLC2isCls(llc)) 
	pak->flags |= PAK_LACK_OUT; /* S/U frames use diff snd/rcv buffer */
    
    if (!llc_encapsulate(pak, type, size, llc)) {
        if (LLC2_DEBUG_ERR)
            buginf("\nllc2_transmit: %s encapsulation failed",
                   llc2_prid(llc));
        datagram_done(pak);
        return;
    }
    
    llc_sendframe(pak);
}

static inline void llc2_prioritize_pak(paktype* pak)
{
    llctype *llc;
    hwidbtype *hwidb;

    if (pak->linktype == LINK_LLC2) {
	llc = pak->llc2_cb;
	hwidb = pak->if_output->hwptr;
	if ((llc->llc2_queuing_type == CLS_QUEUING_PRIORITY) && 
	    (priority_type(hwidb) == QUEUE_PRIORITY)) {
	    pak->flags |= PAK_PRIORITIZED;
	    pak->oqnumber = llc->llc2_queuing_parameter;
	} else if ((llc->llc2_queuing_type == CLS_QUEUING_CUSTOM) && 
	    (priority_type(hwidb) == QUEUE_CUSTOM)) {
	    pak->flags |= PAK_PRIORITIZED;
	    pak->oqnumber = llc->llc2_queuing_parameter;
	}
    } 
}

/*
 * llc_sendframe
 */

void cls_test_and_set_linktype(paktype *pak);


int llc_sendframe (paktype *pak)
{
    llctype *llc;

    if (pak->if_output == NULL)
    {
	buginf("\nllc_sendframe: pak->if_output == NULL");
	datagram_done(pak);
	return(-1);
    }
    llc = pak->llc2_cb;
    pak->linktype = llc->linktype; /* force linktype for now... */
    if (llc == NULL) {
	buginf("\nllc_sendframe: bad llc");
	datagram_done(pak);
	return(-1);
    }
    if (pak->linktype != llc->linktype) {
	buginf("\nllc_sendframe: linktype %d %d mismatch",pak->linktype,llc->linktype);
	datagram_done(pak);
	return(-1);
    }


    llc2_prioritize_pak(pak);    /* Set up priority of packet */

    if (LLC2_DEBUG_PACKET)
	hexout(pak->datagramstart, pak->datagramsize, "llc_sendframe");

    datagram_out(pak);
 
    if (llc->state == LLS_ADM && llc->llc2_adm_timer_value != 0) {
	llc2_stoptimer(llc, ADM_TIMER_M);
	llc2_starttimer(llc, ADM_TIMER, llc->llc2_adm_timer_value);
    }

    return(0);
}


/********************************
 *      IDB cstate routines     *    
 ********************************/


/*
 * llc2_cstate
 * Come here when an interface changes state.  Drop all connections
 * if the interface went down.
 */
void llc2_cstate (hwidbtype *idb)
{
    int     n;
    llctype *llc, *next;

    for (n = 0; n < LLC_DTE_HASH_SIZE; n++) {
	llc = llc_dte_hash_table[n];
	while (llc) {
	    next = llc->next_hash;
	    if ((hwidb_or_null(llc->idb) == idb) && (idb->state != IDBS_UP)) 
		llc2_goto_adm(llc, LC_INTDOWN);
	    
	    llc = next;
	}
    }
}

/********************************
 *         Show routines        *    
 ********************************/

static char *llc2_prrif(llctype *llc, char * str)
{
    if (llc2_get_addrtype(llc) == STATION_IEEE48) {
	byte *rif_p = &llc->llc2_dte[RIF_DTE];
	byte riflen = *rif_p & 0x1f;	/* should use RIF_LENGTH, but */
					/* it doesn't handle odd lengths */
	if (riflen >= 2) {
	    get_rif_str(rif_p, str);
	    return str;
	}
    }
    return NULL;
}

/*
 * show_llc2
 * Show information about LLC circuits
 */
#define NLINES 7			/* number of lines we print */
void show_llc2_command (parseinfo *csb)
{
    int         n;
    llctype     *llc;
    uchar	banner[80];
    boolean	brief = GETOBJ(int,1);

    automore_enable(NULL);

    sprintf(banner, "LLC2 Connections: total of %d connections", llc_in_hash_count);

    automore_header(banner);
    if (llc_in_hash_count == 0) {
       	printf("\nNo LLC2 connections.");
    } else {
	for (n = 0; n < LLC_DTE_HASH_SIZE; n++) {
	    llc = llc_dte_hash_table[n];
	    while (llc) {
		automore_conditional(NLINES);
		show_llctype(llc, brief);

		if (llc->considb)
		    reg_invoke_media_llc_show(LINK_CMNS,llc);
		
		if (!validate_llc(llc))
		    break;
		llc = llc->next_hash;
	    }
	}
    }
    printf("\n");
    automore_disable();
}

/*
 * show_llctype
 */
void show_llctype (llctype *llc, boolean brief)
{
    char str[64];
    sys_timestamp m;			/* use fixed time base */
    char * p = llc2_prrif(llc, str);

    if (brief) {
	if (p != NULL) {
	    printf("\n%s %s %s", 
		   (llc->idb)? llc->idb->short_namestring : "no idb",
		   llc2_prid(llc), p);
	}
	else
	{
	    printf("\n%s %s",
		   (llc->idb)? llc->idb->short_namestring : "no idb",
		   llc2_prid(llc));
	}
	return;
    }
    GET_TIMESTAMP(m);
    printf("\n%s DTE: %s state %s", (llc->idb)? llc->idb->namestring : "no idb",
	   llc2_prid(llc), llc2_prstate(llc, 0));
    printf("\n   V(S)=%d, V(R)=%d, Last N(R)=%d, "
	   "Local window=%d, Remote Window=%d", 
	   llc->v_s>>1, llc->v_r>>1, llc->last_n_r>>1, 
	   llc->loc_wind>>1, llc->rem_wind>>1);
    if (!llc->idb)
	return;
    printf("\n   akmax=%d, n2=%d, ", llc->llc2_akmax, 
	   llc->llc2_n2);
    printf("Next timer in %d", CLOCK_DIFF_SIGNED(m, llc->next_timer));
    printf("\n   xid-retry timer %-6d/%6d  ack timer  %-6d/%6d",
	   (llc->timer_flags & XID_RTRY_TIMER_M) ? 
	   CLOCK_DIFF_SIGNED(m, llc->timers[XID_RTRY_TIMER]) : 0, 
	   llc->llc2_xid_rtry_time,
	   (llc->timer_flags & ACK_TIMER_M) ? 
	   CLOCK_DIFF_SIGNED(m, llc->timers[ACK_TIMER]) : 0, 
	   llc->llc2_T1_time);
    printf("\n   p timer         %-6d/%6d  idle timer %-6d/%6d",
	   (llc->timer_flags & P_TIMER_M) ? 
	   CLOCK_DIFF_SIGNED(m, llc->timers[P_TIMER]) : 0, 
	   llc->llc2_tpf_time,
	   (llc->timer_flags & IDLE_TIMER_M) ? 
	   CLOCK_DIFF_SIGNED(m, llc->timers[IDLE_TIMER]) : 0, 
	   llc->llc2_idle_time);
    printf("\n   rej timer       %-6d/%6d  busy timer %-6d/%6d",
	   (llc->timer_flags & REJ_TIMER_M) ? 
	   CLOCK_DIFF_SIGNED(m, llc->timers[REJ_TIMER]) : 0, 
	   llc->llc2_trej_time,
	   (llc->timer_flags & BUSY_TIMER_M) ? 
	   CLOCK_DIFF_SIGNED(m, llc->timers[BUSY_TIMER]) : 0, 
	   llc->llc2_tbusy_time);
    printf("\n   akdelay timer   %-6d/%6d  txQ count  %-6d/%6d",
  	   (llc->timer_flags & AKDELAY_TIMER_M) ? 
  	   CLOCK_DIFF_SIGNED(m, llc->timers[AKDELAY_TIMER]) : 0, 
	   llc->llc2_akdelay_time,
           llc->txQ.count,llc->llc2_txQ_max);
    if (p != NULL) {
	printf("\n   RIF: %s", p);
    }
}
#undef NLINES

/********************************
 *    Debug output routines     *
 ********************************/

/*
 *	hexout - hex output.
 */
void hexout (uchar *buf,   /* pointer to data */
	     int bc,	   /* data count */
	     char *leader)
{
	if (bc == 0) 			/* if none to dump */
	    return;

	buginf("\nLLC: %s", leader);
	memory_dump(buf, buf + bc,
		    FALSE, MEMORY_DUMP_DATA_TYPE_ASCII, buginf);
}

/********************************
 *   Debugging test commands    *
 ********************************/

#define LLC_OPEN 	0
#define LLC_ATTACH	(LLC_OPEN+1)
#define LLC_SEND	(LLC_ATTACH+1)
#define LLC_CLOSE	(LLC_SEND+1)
#define LLC_STAT	(LLC_CLOSE+1)
#define LLC_CHECKPAK    (LLC_STAT+1)
#define LLC_OFFSET      (LLC_CHECKPAK+1)
#define LLC_HELP	(LLC_OFFSET+1)
#define LLC_NTYPES	(LLC_HELP+1)

static char *const llc_cmds[LLC_NTYPES] = { "open", "attach", "send",
			       "close", "stat", "checkpak", "offset", "?" };
static const ushort llc_flags[LLC_NTYPES] = { 0, 0, 0,
				0, 0, 0, 0, CMD_INV };
static char *const llc_help[LLC_NTYPES] = {
    "open <interface> <dst-mac> [n]  open a circuit",
    "attach <dst-mac>                attach an existing connection",
    "send <cookie> <hex data>        send some data",
    "close <cookie>                  close the circuit",
    "stat                            status of circuits",
    "checkpak                        debug pakmacro",
    "offset                          set send command packet offset (hex)",
    NULL
};



#define NTEST 10
llctype *test_llcs[NTEST];
static int llc2_send_offset = 0;

/*
 * init_test_command
 */
void init_test_command (void)
{
    int i;

    for (i = 0; i < NTEST; i++)
	test_llcs[i] = NULL;
}



/*
 * llc2_test_command
 */
void llc2_test_command (parseinfo *csb)
{
    if (!llc2_start()) {
	printf("LLC2 not started\n");
	return;
    }

    switch (csb->which) {
      case PARSER_LLC_OPEN:
	llc_open_command(GETOBJ(idb,1), (uchar *)GETOBJ(string,1),
			 !(boolean)(GETOBJ(int,2)));
	break;
      case PARSER_LLC_ATTACH:
	llc_attach_command(GETOBJ(idb,1), (uchar *)GETOBJ(string,1));
	break;
      case PARSER_LLC_SEND:
	llc_send_command(GETOBJ(int,2), (uchar *)GETOBJ(string,1),
			 GETOBJ(int,1));
	break;
      case PARSER_LLC_CLOSE:
	llc_close_command(GETOBJ(int,1));
	break;
      case PARSER_LLC_STAT:
	llc_stat_command();
	break;
      case PARSER_LLC_OFFSET:
	llc_offset_command(GETOBJ(int,1));
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

#ifdef CLAUDE_FIX_ME		/* old test command for comparison... */
/*
 * llc2_test_command
 */
void llc2_test_command (char *buff)
{
    int which, matches;
    char *args;

    if (!llc2_start()) {
	printf("LLC2 not started\n");
	return;
    }

    matches = nmatch(buff, llc_cmds, llc_flags, LLC_NTYPES, &which);
    if (matches > 1) {
	printf("%% Ambiguous - \"%s\"",buff);
	return;
    }
    if (matches == 0)
	which = -1;
    args = (char *) findarg(buff);

    switch (which) {
    case LLC_HELP:
	help_command(llc_help, llc_flags, LLC_NTYPES);
	break;
    case LLC_OPEN:
	llc_open_command(args);
	break;
    case LLC_ATTACH:
	llc_attach_command(args);
	break;
    case LLC_SEND:
	llc_send_command(args);
	break;
    case LLC_CLOSE:
	llc_close_command(args);
	break;
    case LLC_STAT:
	llc_stat_command();
	break;
    case LLC_CHECKPAK:
	llc_checkpak_command(args);
	break;
    case LLC_OFFSET:
	llc_offset_command(args);
	break;
    default:
	bad_subcommand(buff,"llc");
	break;
    }
}
#endif

/*
 * llc open <idb> <destination-mac> [N]
 */
static void llc_open_command (idbtype *idb, uchar *dst, boolean blocking)
{
    int i;
    llctype *llc;
    hwaddrtype destMac, srcMac;

    if (!idb) {
	printf("Bad interface specification\n");
	return;
    }

    /*
     * See if already open
     */
    for (i = 0; i < NTEST; i++) {
	llc = test_llcs[i];
	if (llc) {
	    if (ieee_equal(dst, &llc->llc2_dte[RMAC_DTE]) &&
		is_ieee_zero(&llc->llc2_dte[LMAC_DTE])) {
		printf("That destination is already open\n");
		return;
	    }
	}
    }

    snpa_init(&destMac, STATION_IEEE48, TARGET_UNICAST, dst);
    snpa_clear(&srcMac);		/* llc will use interface mac address */
    
    llc = llc2_open(SAP_IBMNM, SAP_IBMNM, &destMac, &srcMac, idb, LINK_IBMNM, 
		     3, (void(*)(llctype*,int))llc_statusproc, blocking, 
				FALSE);
    if (llc == NULL) {
	printf("Open failed\n");
	return;
    }

    /*
     * Find an empty slot
     */
    for (i = 0; i < NTEST; i++)
	if (test_llcs[i] == NULL) {
	    test_llcs[i] = llc;
	    printf("Your cookie is %02x\n", i);
	    llc->test_cookie = i;
	    return;
	}

    printf("Too many connections open\n");
}


/*
 * llc attach <interface> <address>
 */
static void llc_attach_command (idbtype *idb, uchar *dst)
{
    int     i;
    llctype *llc;

    /*
     * Get the interface
     */
    if (!idb) {
	printf("Bad interface specification\n");
	return;
    }

    /*
     * Find the connection
     */
    dst[IEEEBYTES] = SAP_IBMNM;
    dst[IEEEBYTES+1] = SAP_IBMNM;
    llc = llc2_findcirc(dst, idb);
    if (llc == NULL) {
	printf("Connection not found\n");
	return;
    }

    for (i = 0; i < NTEST; i++)
	if (llc == test_llcs[i]) {
	    printf("Already open or attached\n");
	    return;
	}
    for (i = 0; i < NTEST; i++)
	if (test_llcs[i] == NULL)
	    break;
    if (i == NTEST) {
	printf("Too many connections\n");
	return;
    }
    test_llcs[i] = llc;
    llc->statusproc = (void (*)(llctype*, int))llc_statusproc;

    printf("Your cookie is %02x\n", i);
    llc->test_cookie = i;
}


/*
 * llc_send_command
 */
static void llc_send_command (int cookie, uchar *data, int datalen)
{
    int     i;
    uchar   *cp;
    paktype *pak;
    llctype *llc = NULL;

    /*
     * Find our llc
     */
    for (i = 0; i < NTEST; i++) {
	llc = test_llcs[i];
	if (llc && llc->test_cookie == cookie)
		break;
    }
    if (i == NTEST) {
	printf("Can't find llc for cookie %02x\n", cookie);
	return;
    }


    /*
     * build and send the packet
     */
    pak = getbuffer(datalen);
    if (pak == NULL) {
	return;
    }

    cp = pak->datagramstart = pak->network_start - llc2_send_offset;
    for (i = 0; i < datalen; i++)
	*cp++ = data[i];

    pak->if_output = llc->idb;
    pak->linktype = LINK_LLC2;
    pak->llc2_cb = llc;

    llc2_send(llc, pak, datalen-llc2_send_offset);
}


/*
 * llc_close_command
 */
static void llc_close_command (int cookie)
{
    int     i, rc;
    llctype *llc;

    /*
     * Find our llc
     */
    for (i = 0; i < NTEST; i++) {
	llc = test_llcs[i];
	if (llc && llc->test_cookie == cookie)
		break;
    }
    if (i == NTEST) {
	printf("Can't find llc for cookie %02x\n", cookie);
	return;
    }

    test_llcs[i] = NULL;


    rc = llc2_close(llc);
    if (rc != DLC_RS_OK)
	printf("llc2_close returned error\n");
}


/*
 * llc_stat_command
 */
void llc_stat_command (void)
{
    int i;
    llctype *llc;

    for (i = 0; i < NTEST; i++)
	if ((llc = test_llcs[i]) != NULL)
	    show_llctype(llc, FALSE);
}


/*
 * llc_checkpak_command
 * added from 9.1(8), need to add parser support in test command section
 */
void llc_checkpak_command(void)
{
    paktype *pak = NULL;
    
    printf("\n&pak->next=%d\n", &pak->next);
}


/*
 * llc_offset_command
 */
void llc_offset_command (int offset)
{
    llc2_send_offset = offset;
    printf("\nOffset set to 0x%x", llc2_send_offset);
}

/*
 * llc_statusproc
 */
void llc_statusproc (llctype *llc, int status)
{
    int i;

    buginf("\nLLC: test statusproc called with status: %s (%d)", 
	   llc2_prreason(status), status);

    if (status == LC_CONNECT)
	return;

    /* remove from our list */
    for (i = 0; i < NTEST; i++)
	if (llc == test_llcs[i]) {
	    test_llcs[i] = NULL;
	    break;
	}
}





/****************************************
 *      LLC configuration commands      *
 ****************************************/

/*
 * llc2_ifcommand
 */
void llc2_ifcommand (parseinfo *csb)
{
    idbtype    *swidb;
    hwidbtype  *hwidb;
    llc_sb_t   *lsb  = NULL;
    ulong       llc2_fr_dwc_old_val;
    
    swidb = csb->interface;
    hwidb = swidb->hwptr;
    
    
    /*
     * llc2 parameters should be able to be configured even when no LAN media
     * is present.  SDLLC makes use of LLC2 even when LAN media is not present
     * and users of SDLLC need to configure LLC2 parameters for performance.
     * Also sdllc uses the virtual idb block which needs to be used when
     * configuring llc2.
     * Same goes for QLLC
     */
    if ((hwidb->vidb_link) && (hwidb->vidb_link->status & IDB_VIRTUAL)
	&& reg_invoke_srt_other_media(hwidb->vidb_link) ) {

	  hwidb = hwidb->vidb_link;
	  swidb = hwidb->firstsw;
      } else {
          if ((!is_ieee802(hwidb) && !is_frame_relay(hwidb) && 
               !is_cip_virtual(hwidb) && !is_async(hwidb) && 
               !is_isdn(hwidb)) ||
              (is_frame_relay(hwidb) &&
               (hwidb->frame_relay_stuff->fr_ietf_encaps == FALSE))) {
              if (!csb->nvgen) 
                  printf("\nLLC2 not supported on interface %s",
			     swidb->namestring);
              return;
	  }
      }
    
    lsb = llc_get_or_create_sb(swidb);
    if (!lsb) {
      printf(nomemory);
      return;
    }
    if (csb->nvgen) {
	  switch (csb->which) {
	    case LLC2_DYNWIND:
                if (is_frame_relay(hwidb) &&
                    (hwidb->frame_relay_stuff->fr_ietf_encaps == TRUE)) {
                    if (lsb->llc2_dynwind == TRUE) {
                        if ((lsb->llc2_nw == DYNWIND_NW_DEFAULT) &&
                            (lsb->llc2_fr_dwc == FRAME_RELAY_DWC_DEFAULT)) {
                            nv_write(TRUE, "%s", csb->nv_command);
                        } 
                        else if (lsb->llc2_nw == DYNWIND_NW_DEFAULT) {
                            nv_write(TRUE, "%s dwc %d", csb->nv_command,
                                lsb->llc2_fr_dwc);
                        }
                        else if (lsb->llc2_fr_dwc == FRAME_RELAY_DWC_DEFAULT) {
                            nv_write(TRUE, "%s nw %d", csb->nv_command,
                                lsb->llc2_nw);
                        }
                        else {
                            nv_write(TRUE, "%s nw %d dwc %d", csb->nv_command,
                                lsb->llc2_nw, lsb->llc2_fr_dwc);
                        }
                    }
                }
                break;
	    case LLC2_AKMAX:
		nv_write(lsb->llc2_akmax != AKMAX_DEFAULT,
                         "%s %d", csb->nv_command, lsb->llc2_akmax);
		break;
	    case LLC2_AKDELAY_T:
		nv_write(lsb->llc2_akdelay_time != AKDELAY_DEFAULT,
                         "%s %d", csb->nv_command, lsb->llc2_akdelay_time);
		break;
	    case LLC2_IDLE_T:
		nv_write(lsb->llc2_idle_time != IDLE_DEFAULT,
                         "%s %d", csb->nv_command, lsb->llc2_idle_time);
		break;
	    case LLC2_LOC_WIND:
		nv_write(lsb->llc2_wind != LOC_WIND_DEFAULT,
                         "%s %d", csb->nv_command, lsb->llc2_wind);
		break;
	    case LLC2_N2:
		nv_write(lsb->llc2_n2 != N2_DEFAULT,
                         "%s %d", csb->nv_command, lsb->llc2_n2);
		break;
	    case LLC2_T1_T:
		nv_write(lsb->llc2_T1_time != T1_DEFAULT,
                         "%s %d", csb->nv_command, lsb->llc2_T1_time);
		break;
	    case LLC2_TBUSY_T:
		nv_write(lsb->llc2_tbusy_time != TBUSY_DEFAULT,
                         "%s %d", csb->nv_command, lsb->llc2_tbusy_time);
		break;
	    case LLC2_TREJ_T:
		nv_write(lsb->llc2_trej_time != TREJ_DEFAULT,
                         "%s %d", csb->nv_command, lsb->llc2_trej_time);
		break;
	    case LLC2_TPF_T:
		nv_write(lsb->llc2_tpf_time != TPF_DEFAULT,
                         "%s %d", csb->nv_command, lsb->llc2_tpf_time);
		break;
	    case LLC2_NEG_VAL_T:
  		nv_write(lsb->llc2_xid_neg_val_time != XID_NEG_TIME,
                  "%s %d", csb->nv_command, lsb->llc2_xid_neg_val_time);
		break;
	    case LLC2_XID_RTRY_T:
		nv_write(lsb->llc2_xid_rtry_time != XID_RTRY_TIME,
                  "%s %d", csb->nv_command, lsb->llc2_xid_rtry_time);
		break;
	    case LLC2_TXQ_MAX:
		nv_write(lsb->llc2_txQ_max != TXQ_MAX_DEFAULT,
			 "llc2 txq-max %d", lsb->llc2_txQ_max);
		break;
	    case LLC2_ADM_T:
		nv_write(lsb->llc2_adm_timer_value != ADM_TIMER_DEFAULT,
			 "llc2 adm-timer-value %d", lsb->llc2_adm_timer_value);
		break;
	    default:
		bad_parser_subcommand(csb, csb->which);
		break;
	  }
	  return;
    }
    
    switch (csb->which) {
      case LLC2_DYNWIND:
          if (is_frame_relay(hwidb) &&
              (hwidb->frame_relay_stuff->fr_ietf_encaps == TRUE)) {
              lsb->llc2_dynwind = (csb->sense ? TRUE : FALSE);
              lsb->llc2_nw = (csb->sense ? GETOBJ(int,1) : DYNWIND_NW_DEFAULT);
              if (!lsb->llc2_nw)
                  lsb->llc2_nw = DYNWIND_NW_DEFAULT;
              llc2_fr_dwc_old_val = lsb->llc2_fr_dwc; /*  preserve old dwc  */
                                                      /*  value in case new */
                                                      /*  value is invalid. */
	      lsb->llc2_fr_dwc = (csb->sense ? GETOBJ(int,2) :
				  FRAME_RELAY_DWC_DEFAULT);
	      if (!lsb->llc2_fr_dwc)
		lsb->llc2_fr_dwc = FRAME_RELAY_DWC_DEFAULT;
	      else {
                  switch (GETOBJ(int,2)) {
                      case FRAME_RELAY_DWC_DEFAULT:
                      case FRAME_RELAY_DWC_0:
                      case FRAME_RELAY_DWC_1:
                      case FRAME_RELAY_DWC_2:
                      case FRAME_RELAY_DWC_3:
                      case FRAME_RELAY_DWC_4:
                          lsb->llc2_fr_dwc =
                              (csb->sense ? GETOBJ(int,2): FRAME_RELAY_DWC_DEFAULT);
                          break;
                      default:
                          /*  Acknowledge invalid dwc value, and restore  */
                          /*  old dwc value in idb.                       */
                          lsb->llc2_fr_dwc = llc2_fr_dwc_old_val;
                          printf("\nInvalid dwc value");
                          return;
                  }	     
              }
          }   
          else {
              printf("\nCommand not supported on interface %s",
                  swidb->namestring);
          }
          break;
      case LLC2_N2:
	  lsb->llc2_n2 = (csb->sense ? GETOBJ(int,1) : N2_DEFAULT);
	  break;
	  
      case LLC2_T1_T:
	  lsb->llc2_T1_time = (csb->sense ? GETOBJ(int,1) : T1_DEFAULT);
	  break;
	  
      case LLC2_TREJ_T:
	  lsb->llc2_trej_time = (csb->sense ? GETOBJ(int,1) : TREJ_DEFAULT);
	  break;
	  
      case LLC2_TPF_T:
	  lsb->llc2_tpf_time = (csb->sense ? GETOBJ(int,1) : TPF_DEFAULT);
	  break;
	  
      case LLC2_TBUSY_T:
	  lsb->llc2_tbusy_time = (csb->sense ? GETOBJ(int,1) : TBUSY_DEFAULT);
	  break;
	  
      case LLC2_IDLE_T:
	  lsb->llc2_idle_time = (csb->sense ? GETOBJ(int,1) : IDLE_DEFAULT);
	  break;
	  
      case LLC2_AKDELAY_T:
	  lsb->llc2_akdelay_time = (csb->sense ? GETOBJ(int,1) : AKDELAY_DEFAULT);
	  break;
	  
      case LLC2_XID_RTRY_T:
	  lsb->llc2_xid_rtry_time = (csb->sense ? GETOBJ(int,1) : XID_RTRY_TIME);
	  break;
	  
      case LLC2_AKMAX:
	  lsb->llc2_akmax = (csb->sense ? GETOBJ(int,1) : AKMAX_DEFAULT);
	  break;
	  
      case LLC2_LOC_WIND:
	  lsb->llc2_wind = (csb->sense ? GETOBJ(int,1) : LOC_WIND_DEFAULT);
	  break;
	  
      case LLC2_NEG_VAL_T:
	  lsb->llc2_xid_neg_val_time = (csb->sense ? GETOBJ(int,1) : XID_NEG_TIME);
	  break;
	  
      case LLC2_TXQ_MAX:
	  lsb->llc2_txQ_max = (csb->sense ? GETOBJ(int,1) : TXQ_MAX_DEFAULT);
	  break;
	  
      case LLC2_ADM_T:
	  lsb->llc2_adm_timer_value = (csb->sense ? GETOBJ(int,1) : ADM_TIMER_DEFAULT);
	  break;

      default:
	  bad_parser_subcommand(csb, csb->which);
	  break;
      }

      /* 
       * If vidb_link exists (i.e. a virtual hwidb exists)
       *   update vidb LLC subblock with latest config values
       *
       */
      if (hwidb->vidb_link) {
        llc_sb_copy(hwidb->vidb_link->firstsw, swidb);
      }
      reg_invoke_media_reset_llc2_parms(hwidb->type, hwidb);
}

/****************************************
 * Stub routines for IBM lanmgr testing *
 ****************************************/

/*
 * Stub routines for IBM Lanmgr support
 */
void test_lanmgr_enqueue ( paktype *pak)
{
    char string[20];
    
    sprintf(string, "Lanmgr rcvd %s:", (pak->llc2_cb)? "I" : "UI");
    hexout(pak->datagramstart, pak->datagramsize, string);

    datagram_done(pak);
}


#ifdef NOBODY_CALLS_ME
/*
 * ibmnm_enqueue
 * Remove in trees that have the real ibmnm
 */
void llc2_ibmnm_enqueue (paktype *pak)
{
    if (llc2_debug_test)
	test_lanmgr_enqueue(pak);
    else {
	protocol_discard(pak, TRUE);
    }
}
#endif /* NOBODY_CALLS_ME */

char *llc2_prid(llctype *llc)
{
    char *str = string_getnext();

    /*
     * Check implementation of string_getnext() before adding
     * more junk to the format strings below.  Current code
     * gives you a pointer to 80 bytes of space -- BEWARE.
     */

    if (llc == NULL)
	return("bogus");

    /* name not to exceed LLC2_PRINT_LENGTH chars */

    if ((llc2_get_addrtype(llc) == STATION_IEEE48) ||
        (llc2_get_addrtype(llc) == STATION_PPP)    ||
        ((llc2_get_addrtype(llc) == STATION_FR10) && llc->fDlci))
    {
	sprintf(str, "%e %e %02x %02x", 
		llc2_get_rmac(llc), llc2_get_lmac(llc),
		llc2_get_lsap(llc), llc2_get_rsap(llc));
    }
    else if (llc2_get_addrtype(llc) == STATION_FR10)
    {
	sprintf(str, "%d %02x %02x", 
		*((ushort*) llc2_get_rmac(llc)),
		llc2_get_lsap(llc), llc2_get_rsap(llc));
    }
    else if (llc2_get_addrtype(llc) == STATION_ATMVC)
    {
	sprintf(str, "%d %02x %02x", 
		*((ushort*) llc2_get_rmac(llc)),
		llc2_get_rsap(llc), llc2_get_lsap(llc));
    }
    return(str);
}


char *llc2_prstate(llctype *llc, int state)
{
    if (llc)
	state = llc->state;
    switch (state) {
      case LLS_NONE:		return("NONE");
      case LLS_ADM:		return("ADM");
      case LLS_SETUP:		return("SETUP");
      case LLS_RESET:		return("RESET");
      case LLS_D_CONN:		return("D_CONN");
      case LLS_ERROR:		return("ERROR");
      case LLS_NORMAL:		return("NORMAL");
      case LLS_BUSY:		return("BUSY");
      case LLS_REJECT:		return("REJECT");
      case LLS_AWAIT:		return("AWAIT");
      case LLS_AWAIT_BUSY:	return("AWAIT_BUSY");
      case LLS_AWAIT_REJ:	return("AWAIT_REJ");
      default:			return("UNKNOWN");
    }
}


char *llc2_prreason(int reason)
{
    switch(reason) {
      case 0:			return("LLC_LC_BOGUS");
      case LC_CONNECT:		return("CONNECT");
      case LC_CONCNF:		return("CONNECT CONFIRM");
      case LC_DATA:		return("DATA");
      case LC_DATACNF:		return("DATA CONFIRM");
      case LC_DISC:		return("DISC");
      case LC_DISC_LAN:		return("DISC_LAN");
      case LC_DISC_CNF:		return("DISC_REQ");
      case LC_DISCNF:		return("DISC CONFIRM");
      case LC_DM:		return("DM");
      case LC_RESET:		return("RESET");
      case LC_RSTCNF:		return("RESET CONFIRM");
      case LC_REPORT:		return("REPORT");
      case LC_FLOWCONTROL:	return("FLOWCONTROL");
      case LC_TIMEOUT:		return("TIMEOUT");
      case LC_INTDOWN:		return("INTERFACE DOWN");
      case LC_FRMR:		return("FRMR");
      case LC_ABORTED:		return("ABORTED");
      default:			return("UNKNOWN");
    }
}

char *llc2_prevent(int event)
{
    switch(event) {
      case 0:			return("ZERO EVENT");
      case REC_I_CMD:		return("REC_I_CMD");
      case REC_I_RSP:		return("REC_I_RSP");
      case REC_RR_CMD:		return("REC_RR_CMD");
      case REC_RR_RSP:		return("REC_RR_RSP");
      case REC_RNR_CMD:		return("REC_RNR_CMD");
      case REC_RNR_RSP:		return("REC_RNR_RSP");
      case REC_REJ_CMD:		return("REC_REJ_CMD");
      case REC_REJ_RSP:		return("REC_REJ_RSP");
      case REC_SABME_CMD:	return("REC_SABME_CMD");
      case REC_DISC_CMD:	return("REC_DISC_CMD");
      case REC_UA_RSP:		return("REC_UA_RSP");
      case REC_DM_RSP:		return("REC_DM_RSP");
      case REC_FRMR_RSP:	return("REC_FRMR_RSP");
      case REC_BAD_CMD:		return("REC_BAD_CMD");
      case REC_BAD_RSP:		return("REC_BAD_RSP");
      case TX_DM_RSP:		return("TX_DM_RSP");
      case REC_INV_LSAP:	return("REC_INV_LSAP");
      case REC_INV_ILEN:	return("REC_INV_ILEN");
      case REC_INV_IFIELD:	return("REC_INV_IFIELD");
      case REC_TOO_SMALL:	return("REC_TOO_SMALL");
      case CONN_REQ:		return("CONN_REQ");
      case DATA_REQ:		return("DATA_REQ");
      case RESET_REQ:		return("RESET_REQ");
      case DISC_REQ:		return("DISC_REQ");
      case P_TIMER_EXP:		return("P_TIMER_EXP");
      case ACK_TIMER_EXP:	return("ACK_TIMER_EXP");
      case REJ_TIMER_EXP:	return("REJ_TIMER_EXP");
      case BUSY_TIMER_EXP:	return("BUSY_TIMER_EXP");
      case XID_TIME_EXP:	return("XID_TIME_EXP");
      case IDLE_TIME_EXP:	return("IDLE_TIME_EXP");
      case CLOK_TX_ACTION:	return("CLOK_TX_ACTION");
      case LOCK_TX_ACTION:	return("LOCK_TX_ACTION");
      case SET_LOCAL_BUSY:	return("SET_LOCAL_BUSY");
      case CLEAR_LOCAL_BUSY:	return("CLEAR_LOCAL_BUSY");
      case INIT_PF_CYCLE:	return("INIT_PF_CYCLE");
      case REC_XID_CMD:		return("REC_XID_CMD");
      case REC_XID_RSP:		return("REC_XID_RSP");
      case REC_TEST_CMD:	return("REC_TEST_CMD");
      case REC_TEST_RSP:	return("REC_TEST_RSP");
      default:			return("UNKNOWN");
    }
}


char *llc2_prstatus(int status)
{
    switch(status) {
      case 0:			return("zero status");
      case LS_SUCCESS:		return("success");
      case LS_RESETTING:	return("resetting");
      case LS_RESET_DONE:	return("reset done");
      case LS_DISCONNECT:	return("disconnect");
      case LS_FAILED:		return("failed");
      case LS_CONFLICT:		return("conflict");
      case LS_RESET_FAILED:	return("reset failed");
      case LS_RESET_REFUSED:	return("reset refused");
      case LS_RESET_DECLINED:	return("reset declined");
      case LS_FRMR_RECEIVED:	return("frmr received");
      case LS_FRMR_SENT:	return("frmr sent");
      case LS_REM_BUSY:		return("rem busy");
      case LS_REM_NOT_BUSY:	return("rem not busy");
      case LS_WAITING:		return("waiting");
      default:			return("unknown");
    }
}


char	*llc2_prctlchr(int chr)
{
    switch(chr) {
      case SABME_IND:		return("SABME_IND");
      case DISC_IND:		return("DISC_IND");
      case XID_IND:		return("XID_IND");
      case UA_IND:		return("UA_IND");
      case DM_IND:		return("DM_IND");
      case XID_RSP_IND:		return("XID_RSP_IND");
      case RR_CMD_P0:		return("RR_CMD_P0");
      case RNR_CMD_P0:		return("RNR_CMD_P0");
      case REJ_CMD_P0:		return("REJ_CMD_P0");
      case RR_CMD_P1:		return("RR_CMD_P1");
      case RNR_CMD_P1:		return("RNR_CMD_P1");
      case REJ_CMD_P1:		return("REJ_CMD_P1");
      case RR_RSP:		return("RR_RSP");
      case RNR_RSP:		return("RNR_RSP");
      case REJ_RSP:		return("REJ_RSP");
      case FRMR_IND:		return("FRMR_IND");
      case I_CMD_IND:		return("I_CMD_IND");
      case I_RSP_IND:		return("I_RSP_IND");
      case TXTOTSIZE:		return("TXTOTSIZE");
      default:			return("unknown");
    }
}

/*
 * Set weight and return flow_id for WFQ logic
 */

short llc2_fair_queue_flow_id (paktype *pak)
{
    llctype *llc;

    llc = pak->llc2_cb;
    if (llc == NULL) {
	if (llc2_debug_err) {
	    /*
	     * Not sure what else will turn up here as we run
	     * regression tests, but so far I know about TEST and XID
	     * frames which are not associated with a specific
	     * llctype. [sberl 6/28/95]
	     */
	    buginf("\nllc2_fair_queue_flow_id: pak has NULL llc2_cb");
	}
        return(linktype_is_fq_conversation(pak));
    } else if (llc->llc2_queuing_type == CLS_QUEUING_FAIR) {
	pak->fair_weight = llc->llc2_fair_sequence;
    } else {
	pak->fair_weight = LLC2_FAIR_SEQ_DEFAULT;
    }
    return(llc->llc2_fair_sum);
}

static void llc2_init_fair_sum(idbtype *idb, llctype *llc)
{
    int sum;

    sum = xnethash(llc->llc2_dte, LLC_MAX_DTE_SIZE);
    llc->llc2_fair_sum = idb->hwptr->fair_queue_head->fq_hash_mask, sum;
}


/*
 * llc2_store_rif
 *
 * Store Rif in llctype
 */

void llc2_store_rif (llctype *llc, uchar *rif, boolean change_direction)
{
    if (rif == NULL) {

	/*
	 * KLUDGE! Legal rif always has an even length. Setting it to
	 * an odd value fakes llc2_token_macaddr() into thinking that
	 * this is a rif (zero means legacy code to it). tr_vencap()
	 * will then notice that the rif is an illegal length and set
	 * the length to zero and send the packet with no rif at all.
	 * [sberl]
	 */

	llc->llc2_dte[RIF_DTE] = 0x01;

    } else {

	/*
	 * We only change direction when the caller
	 * tells us to...
	 */

	if (change_direction == CHANGE_DIRECTION) {

	    srbroute_t *ptr = (srbroute_t *)rif;

	    ptr->rc_dlf ^= RC_D;
	}

	bcopy(rif, &llc->llc2_dte[RIF_DTE], RIF_LENGTH(*rif));
    }
}

/*
 * llc2_on_idb
 * returns boolean: is llc2 active on this swidb?
 */
boolean llc2_on_idb (idbtype *idb)
{
    sap_registry_t *sap_registry;
    int index;
  
    if (llc2_on_idb_override)
       return(TRUE);

    if (!llc2_running)
       return(FALSE);
  
    sap_registry = sap_get_registry(idb);
    if (sap_registry == NULL)
       return(FALSE);
  
    /* see if there is an active sap (other than NULL_SAP) */
    /* in this registry                                    */
    for (index = 1; index < MAX_SAP_ENTRIES; index ++) {
      if (  sap_registry->dlc_saps[index] &&
	   (sap_registry->dlc_saps[index]->sap_flags & SAP_CLS_ACT_SAP))
	 return(TRUE);
    }
  return (FALSE);
}

/*
 * llc2_respond_with_dm()
 *
 * this function takes a paktype, and responds to the
 * sender of the packet with a DM_IND packet. its used
 * by sdllc to rebuff a suitor.
 *
 * Use station component to send DM RSP 
 * llc->p_f_val will be set up prior to call of 
 * llc2_stateswitch 
 */

void llc2_respond_with_dm(paktype * pak)
{
    leveltype level = raise_interrupt_level(NETS_DISABLE);
    llctype llc = *llc2stationptr;

    llc2_by_pak(pak, llc.llc2_dte, &llc.llc2_addrtype, &llc.fDlci);

    if (pak->flags & (PAK_LACK_IN | PAK_LACK_OUT)) {
	llc.rsrb_lack = TRUE;
    } else {
	llc.rsrb_lack = FALSE;
    }

    llc.idb = pak->if_input;
    send_control(&llc, DM_IND);

    reset_interrupt_level(level);
}

