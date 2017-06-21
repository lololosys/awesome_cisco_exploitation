/* $Id: sdlc.c,v 3.13.12.21 1996/08/28 12:47:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/sdlc.c,v $
 *------------------------------------------------------------------
 * sdlc.c -- Serial Data Link Control (Alternate transmission, no REJ, FRMR
 *           off by default.)
 * 
 * May & August, 1991, Joel P. Bion
 * from: lapb.c, 1986 Greg Satz
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sdlc.c,v $
 * Revision 3.13.12.21  1996/08/28  12:47:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.12.20  1996/08/26  15:08:27  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.13.12.19  1996/08/07  08:59:37  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.13.12.18  1996/08/02  17:56:46  dbilling
 * CSCdi63154:  CLS(SDLC) trips assert when dlc sends data ind w/CLS_NO_CEP
 * Discard DATA_IND destined for CLS if u_cep_id = CLS_NO_CEP
 * Branch: California_branch
 *
 * Revision 3.13.12.17  1996/07/23  13:23:23  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.13.12.16  1996/07/18  04:25:40  ppearce
 * CSCdi56398:  Router (sdlc-secondary) fails to respond to SNRM inputs
 * Branch: California_branch
 *   Ensure SDLLC process is re-started when SDLLC is configured
 *
 * Revision 3.13.12.15  1996/07/17  21:27:59  vkamat
 * CSCdi60439:  SDLC-primary interface dies if serial line is looped
 * Branch: California_branch
 *
 * Revision 3.13.12.14  1996/07/10  23:08:35  pleung
 * CSCdi51258:  aping transfer stops sending data
 * Branch: California_branch
 *
 * Revision 3.13.12.13  1996/07/09  05:58:21  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.13.12.12  1996/06/27  09:30:51  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.13.12.11  1996/06/25  15:27:58  fbordona
 * CSCdi60090:  2500 SDLC connection stuck in USBUSY state after DLSW
 *              session closed. Improper use of TIMER_STOP and SLEEPING
 *              caused CLS to stop sending XID NULL.
 * Branch: California_branch
 *
 * Revision 3.13.12.10  1996/06/18  01:26:11  athippes
 * CSCdi60080:  7000 router crash during SDLC configuration
 * Branch: California_branch
 *
 * Revision 3.13.12.9  1996/06/17  23:30:56  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.13.12.8  1996/06/11  09:15:23  dignacio
 * CSCdi51539:  SDLC test output not sent to terminal mon telnet session
 * Branch: California_branch
 *
 * Revision 3.13.12.7  1996/06/11  06:49:32  vkamat
 * CSCdi48414:  SDLC full datamode crashes under CLS/SDLC
 * Branch: California_branch
 *
 * Revision 3.13.12.6  1996/06/04  00:20:56  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * secondry -> secondary
 *
 * Revision 3.13.12.5  1996/05/31  15:50:35  vkamat
 * CSCdi58956:  UA frame leaks while SDLC Primary port is recovering the
 * line
 * Branch: California_branch
 *
 * Revision 3.13.12.4  1996/05/23  06:42:37  pmorton
 * CSCdi58502:  Fix for CSCdi42369 broke FRAD image
 * Branch: California_branch
 * Use 'debug sdlc qllc' instead of 'debug qllc sdlc'
 *
 * Revision 3.13.12.3  1996/05/22  04:59:57  pmorton
 * CSCdi42369:  sdlc-qllc conversion, no auto-reconnect
 * Branch: California_branch
 * 1. When qllc link state changes propagate to SDLC-QLLC conversion (SNX)
 * 2. Add 'debug qllc sdlc' to track this
 * 3. Correct SNX state names
 *
 * Revision 3.13.12.2  1996/05/17  11:21:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.13.2.6  1996/05/03  05:42:14  athippes
 * CSCdi47090:  Non-Activation XID sent to SDLC device with address as ff
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.13.2.5  1996/05/02  20:24:21  pmorton
 * CSCdi42678:  Router crashed after removing SDLC address
 * Branch: IbuMod_Calif_branch
 * Mark sdlc data block for deletion (zombie).  Don't remove
 * till CLS issues CLOSE Station request.
 *
 * Revision 3.13.2.4  1996/04/26  15:19:58  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.13.2.3  1996/04/26  07:54:29  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.13.2.2  1996/04/11  14:37:53  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.13.2.1  1996/04/03  14:34:30  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.13.12.1  1996/03/18  20:11:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.3  1996/03/13  01:23:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8.2.2  1996/03/07  09:42:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.1  1996/02/20  00:54:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/02/13  21:27:16  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.12  1996/02/13  02:25:11  fred
 * CSCdi46765:  SDLC interface show fair-queue enabled
 *         turn it off
 *
 * Revision 3.11  1996/02/07  16:12:00  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.10  1996/02/01  06:03:22  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.9  1996/01/25  11:18:20  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.8  1996/01/18  22:51:48  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1995/12/31  18:17:25  fbordona
 * CSCdi46267:  DLSw sdlc secondary multidrop PUs do not connect.
 *
 * Revision 3.6  1995/12/15  22:46:56  rbatz
 * CSCdi43378:  Explorers destined for SDLLC Virtual Ring forwarded
 *              to wrong ring.  Rework of vr_virtualp required.
 *
 * Revision 3.5  1995/12/08  22:40:59  vrawat
 * CSCdi43686:  After reload SDLC packets identified as HDLC by serial
 * driver
 *
 * Revision 3.4  1995/11/20  22:24:54  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  09:23:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:35:48  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:45:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.18  1995/10/19  08:09:26  bchan
 * CSCdi27730:  SNA Fragmentation break if no buffers available
 *
 * Revision 2.17  1995/10/04  17:17:33  dignacio
 * CSCdi36250:  SDLC Test Frame responds with twice as many sent.
 *
 * Revision 2.16  1995/09/09  00:51:45  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.15  1995/08/12  07:09:06  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.14  1995/08/08  16:47:13  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.13  1995/08/07 05:35:14  richl
 * CSCdi38315:  makefile rework
 *              move utility routines to libibm_util.a
 *
 * Revision 2.12  1995/08/01  02:32:05  vrawat
 * CSCdi37993:  fix stun problems found at customer site
 *
 * Revision 2.11  1995/07/31  04:12:45  szdravko
 * CSCdi37829:  Two sdlc mib fields not maintained
 *
 * Revision 2.10  1995/07/28  22:05:22  vrawat
 * CSCdi36957:  Aborted SDLC STUN frames transmitted across local-ack
 * connection
 *
 * Revision 2.9  1995/07/19  21:03:16  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add service to reducce dependency on idb.
 *
 * Revision 2.8  1995/07/13  20:33:10  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.7  1995/07/06  01:32:28  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.6  1995/06/28 18:44:18  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *
 * Revision 2.5  1995/06/28  09:25:46  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.4  1995/06/18  23:36:48  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/15  02:27:01  lpereira
 * CSCdi35871:  Cannot remove sdlc address without shutting the interface
 *
 * Revision 2.2  1995/06/09  13:05:10  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:47:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define DISABLE_RESETS

#include "master.h"
#include "types.h"
#include <string.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "../cls/dlc_registry.h"
#include "interface_private.h"  /* -> generic when subblock work done */
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_sdlc.h"
#include "../parser/parser_defs_half_duplex.h"
#include "../wan/serial_parser.h"
#include "../ui/debug.h"
#include "address.h"
#include "../llc2/llc2.h"
#include "../srt/rsrb.h"
#include "../srt/lnx.h"
#include "sdlc.h"
#include "sdlc_public.h"
#include "sdlc_registry.h"
#include "../ibm/sdlc_registry.regc"
#include "logger.h"
#include "../ibm/msg_sdlc.c"		/* Not a typo, see logger.h */
#include "../ibm/msg_stun.c"		/* Not a typo, see logger.h */
#include "../os/free.h"
#include "../wan/serial.h"
#include "../wan/serial_parser.h"
#include "../ip/ip.h"
#include "../if/network.h"
#include "../srt/srb.h"
#include "../srt/srb_core.h"
#include "../srt/rsrb.h"
#include "../hes/if_mci.h"
#include "../ibm/ibm.h"
#include "../ibm/ibm_public.h"
#include "../ibm/ibm_externs.h"
#include "../ibm/libibm_externs.h"
#include "../ibm/sna_util.h"
#include "../ibm/sdlc_debug.h"
#include "../srt/lack_externs.h"
#include "../srt/lack2fsm.h"
#include "../srt/lack2_externs.h"
#include "../ibm/sna_la_pri.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "../srt/srt_registry.h"
#include "../ibm/sdlc_registry.h"
#include "../srt/lnx.h"
#include "../cls/dlc_public.h"
#include "../cls/dlc.h"
#include "sdlc_event.h" 
#include "subsys.h"
#include "../if/rif_util.h"
#include "../os/old_timer_callbacks.h"
#include "../if/priority.h"

extern void sdlc_wput (DlcPCepIdT pCepId, DlcOpcodeT opcode,
	       ClsDlcParamBlockT *clsParamBlock);

static const char sdlc_cb[] = "SDLC CB";
/** Forward Declaration 
 **/
static void SdlcFrmrCondition(hwidbtype *idb, 
                              paktype *pak, 
                              sdlc_data_block *sdb,
                              uchar frmr_reason, 
                              int poll, 
                              uchar secondary);
static void sdlc_frmr(hwidbtype *idb,
                       paktype *epak,
                       uchar frmr_reason, 
                       uchar poll, 
                       uchar cmd,
                       sdlc_data_block *sdb);

static timer_t *sdlc_tmr;         /* timer handle */

long sdlc_regcode;		/* sdlc service registration id */

static boolean IsOutbound(sdlc_data_block *sdb);
static boolean IsInbound(sdlc_data_block *sdb);
static boolean IsPartnerCfgd(sdlc_data_block *sdb);

/*
 * #define FULL_I_FRAMES for full I frame printing on output (helpful for
 *  debugging in house)
 */


static boolean sdlc_debug_filter(
    boolean *pflag,
    sdlc_data_block *sdb,
    paktype *pak)
{
    debug_list_t *list;
    int offset = IEEEBYTES * 2;
    uchar *data = NULL;
    int size = 0;
    mac_address_type mac_null_addr;

    list = debug_listq_find(pflag);
    if ((list == NULL) || (sdb == NULL)) {
	return (TRUE);
    }
    
    if (list->idb != NULL && sdb->sdlc_idb->firstsw != NULL && 
	list->idb != sdb->sdlc_idb->firstsw) {
	return (FALSE);
    }
    if (pak) {
	data = pak->datagramstart;
	size = pak->datagramsize;
    } else {
	data = &sdb->sdlc_address;
	size = 1;
    }
    memset(&mac_null_addr, 0, sizeof(mac_address_type));
    return(reg_invoke_xmacaccesscheck_gen(list->access_list,
			       &mac_null_addr, &mac_null_addr,
			       data, offset, size));
}

static void sdlc_packet_dump(sdlc_data_block *sdb, paktype *pak,
		      char *namestring, char *label)
{
    /*
     * now accepts sdb = NULL for group poll address and unconfigured addresses
     */
    if (pak && sdlc_packet_debug)
        if (!sdb || sdlc_debug_filter(&sdlc_packet_debug, sdb, pak)) {
	    int size, i;
	    
	    if (sdlc_packet_debug < pak->datagramsize) 
		size = sdlc_packet_debug -1;
	    else 
		size = pak->datagramsize -1;

	    buginf ("\n%s %s", namestring, label);

	    if (pak->datagramsize <= 4) {
		buginf("\t");
		for (i=0; i < pak->datagramsize; i++)
		    buginf("%02x", *(pak->datagramstart + i));
	    }
	    else 
		memory_dump(pak->datagramstart,
			    pak->datagramstart + size,
			    FALSE, MEMORY_DUMP_DATA_TYPE_EBCDIC, buginf);
	}
}
#define SDLC_DEBUG(sdb, pak) (sdlc_debug && \
		sdlc_debug_filter(&sdlc_debug, sdb, pak))
			   
static int sdlc_tf;				/* timer process' pid */

/*
 *  number of bytes sent before a Poll from this router.
 */
ulong serial_bcount;
ulong adjusted_t1;
boolean snx_running; /* qllc-sdlc */

/*
 * Externs
 */
extern queuetype stun_tg_infoQ;
extern char *pr_cls_sdlc_state(sdlc_data_block *sdb);
extern void sdlc_cls_reset(sdlc_data_block *sdb);
extern paktype* GetProxyXid(sdlc_data_block *, boolean);

static void tws_transmit(hwidbtype *);
static boolean tws_send(hwidbtype *, sdlc_data_block *, boolean), 
	tws_send_xmtleg(hwidbtype *, sdlc_data_block *, boolean);
#ifdef SDLC_VERBOSE_DEBUG
static void retbuffer2 (paktype *pak)
{
    if (pak->refcount != 1) {
	if (sdlc_debug) {
	    printf("\nSDLC: Retbuffer with sharecount = %d (pak %x)",
		   pak->refcount, pak);
	}
    }
    datagram_done(pak);
} 
#else
#define retbuffer2 datagram_done
#endif



/*
 * Set/Clear busy state of OUR end (we generate RNR on busy)
 */
void sdlc_set_clear_busy (hwidbtype *idb, unsigned addr, boolean setit)
{
    sdlc_data_block *sdb;

    sdb =  idb->sdlc_data[addr];
    if (!sdb)
	return;
    if (setit) {
	switch (sdb->sdlc_state) {
	case SDLC_CONNECT:
            sdlc_setstate(idb, SDLC_USBUSY, sdb);
	    break;
	case SDLC_THEMBUSY:
            sdlc_setstate(idb, SDLC_BOTHBUSY, sdb);
	    break;
	default:
	    break;
	}
    } else {
	switch (sdb->sdlc_state) {
	case SDLC_USBUSY:
            sdlc_setstate(idb, SDLC_CONNECT, sdb);
	    break;
	case SDLC_BOTHBUSY:
            sdlc_setstate(idb, SDLC_THEMBUSY, sdb);
	    break;
	default:
	    break;
	}
    }
}


/*
 * STUN Transmission Group reroute for sdlc link failure.
 */




static void stun_tg_sdlc_fail (
    hwidbtype *idb,
    sdlc_data_block *sdb)
{
    int i;
    paktype *pak;
    elementtype *currel;

    struct sdlc_tg_reroute_info {
      hwidbtype       *idb;
      sdlc_data_block *sdb;
    } tg_reroute[TG_MAX_LINKS];

    boolean         queues_full = FALSE;
    int             num_tg_idb, next_route;
    stunpeertype    *stp, *next_stp;
    hwidbtype       *next_idb;
    sdlc_data_block *next_sdb;
    hwidbtype       *cos_reseq_idb = NULL;
    stun_tg_info    *tgi;

    if (stun_tg_debug)
        buginf("\nSDLC: link failure on interface %s",idb->hw_namestring);

    /*
     *  If this sdlc link is part of a transmission group then reroute
     *  the I-frames in the retransmit queue and the sdlc hold queue to
     *  another active sdlc link in the transmission group.
     */
    stp = stun_addr_to_peer(idb, ((char *) &(sdb->sdlc_address)));
    if((!stp) || (!stp->tg_active))
       return;

    for (tgi = (stun_tg_info *) stun_tg_infoQ.qhead; tgi; tgi = tgi->next) {
        if (tgi->group == stp->group)
            break;
    }

    if (!tgi) {
        errmsg(&msgsym(NOTGI, STUN), stp->group);
        return;
    }

    num_tg_idb = 0;
    if (stun_tg_debug)
        buginf("\nSDLC: idb = %s, part of tg : %d", idb->hw_namestring,
                stp->group);

    /* search for available tg sdb's that have avail. holdq space */
    for (next_idb=stun_first_idb; next_idb; next_idb=IDB_NEXT(next_idb)) {
        if ((next_idb == idb) ||
            (next_idb->stun_group != idb->stun_group) ||
            (idb->maxmtu > next_idb->maxmtu) ||
            (!(next_stp = (stunpeertype *) next_idb->stun_Q.qhead)))
            continue;

        cos_reseq_idb = next_idb;
        next_sdb=(sdlc_data_block *)next_idb->sdlc_data[next_stp->addr[0]];
        if (next_sdb->sdlc_state != SDLC_CONNECT)
            continue;

        if (!QUEUEFULL(&next_sdb->sdlc_holdq)) {
            if (stun_tg_debug)
                buginf("\nSDLC: alt route idb = %s, part of tg: %d",
                        next_idb->hw_namestring, idb->stun_group);
             tg_reroute[num_tg_idb].idb = next_idb;
             tg_reroute[num_tg_idb].sdb = next_sdb;
             num_tg_idb++;
             cos_reseq_idb = next_idb;
        }
    }

    /* perform reroute if any holdq space found above */
    if (num_tg_idb > 0) {
        if (stun_tg_debug)
            buginf("\nSDLC: one or more interfaces to reroute frames...");
        /*
         * Reroute frames on the interface retransmit queue.
         */
        next_route = 0;
        for (i = 0; i < SDLC_MODULO; i++) {
             if (sdb->sdlc_paks[i]) {
                 if (stun_tg_debug)
                     buginf("\nSDLC: frame on %s retransmit queue",
                             idb->hw_namestring);
                 /* find an idb that can take a packet */
                 while(TRUE) {
                     if (!QUEUEFULL(&tg_reroute[next_route].sdb->sdlc_holdq))
                         break;
                     else if (++next_route >= num_tg_idb) {
                         queues_full = TRUE;
                          break;
                     }
                 }
                 if (queues_full)
                     break;
                 if (stun_tg_debug)
                     buginf("\nSDLC: moving frame to %s holdq",
                             tg_reroute[next_route].idb->hw_namestring);
                 sdlc_holdq (tg_reroute[next_route].idb,
                             tg_reroute[next_route].sdb, sdb->sdlc_paks[i]);
                 sdb->sdlc_paks[i] = NULL;
             }
        }

        /*
         * Reroute frames on the interface holdq.
         */
        while ((pak=pak_dequeue(&sdb->sdlc_holdq)) && (!queues_full)) {
            if (stun_tg_debug)
                buginf("\nSDLC: frame on %s holdq",idb->hw_namestring);

            while(TRUE) {
                if (!QUEUEFULL(&tg_reroute[next_route].sdb->sdlc_holdq))
                    break;
                else if (++next_route >= num_tg_idb) {
                    queues_full = TRUE;
                    break;
                }
            }
            if (queues_full)
                break;
            if (stun_tg_debug)
                buginf("\nSDLC: moving frame to %s holdq",
                        tg_reroute[next_route].idb->hw_namestring);
            sdlc_holdq (tg_reroute[next_route].idb,
                        tg_reroute[next_route].sdb, pak);
        }

        /*
         * Reroute frames from tg's resequence queue.
         * These frames do not need to be transmitted, just
         * marked to be transmitted on an interface that is UP.
         */
        if ((currel = (elementtype *)(tgi->tg_reseqQ.qhead))) {
            while (currel) {
                if (currel->pakptr->if_output->hwptr == idb) {
                    if (stun_tg_debug)
                        buginf("\nSDLC: Q'd PIU - changing idb");
                    currel->pakptr->if_output = cos_reseq_idb->firstsw;
                    currel->pakptr->peer_ptr = cos_reseq_idb->stun_Q.qhead;
                }
                currel = currel->next;
            }
        }
    } else {
        if (stun_tg_debug)
            buginf("\nSDLC: no place to reroute frames...");
        /*
         * Discard PIUs in reseqQ, no place to reroute them.
         */
        while ((pak = pak_dequeue(&tgi->tg_reseqQ))) {
            if (stun_tg_debug)
                buginf("\nSDLC: discarding frame in tg_reseqQ");
            datagram_done(pak);
        }
    }
    return;
}


/* Handle a "FRMR condition".  Save the FRMR ifield for the snmp trap.  If the
 * FRMR is not allowed on the line, then we still save the ifield because this
 * gives valuable information about the reason for the reject of the frame.
 */ 
static void SdlcFrmrCondition(hwidbtype *idb, 
                       paktype *pak, 
                       sdlc_data_block *sdb,
                       uchar frmr_reason, 
                       int poll, 
                       uchar secondary)
{
    sdlc_nm_event_hook_t event = -1;
    sdlc_frametype *frame = (sdlc_frametype *)pak->datagramstart;
    uchar control = frame->control;

    if (!(idb->sdlc_flags & SDLC_FLAG_NOFRMR)) {
        /* FRMR allowed. Send the FRMR. 
         */
        sdlc_frmr(idb, pak, frmr_reason, poll, secondary, sdb);
    }
    else {
        /* FRMR not allowed. 
         */
        switch (frmr_reason){
            case SDLC_FRMR_Y:
                sdlc_reset(idb, "Frame larger than N1 bits seen", sdb);
                break;

            case SDLC_FRMR_X|SDLC_FRMR_W:
                if (sdb->sdlc_state != SDLC_ERROR) {
                    if (SDLC_ISUFRAME(frame))
                        sdlc_reset(idb, "Unknown FRMR received", sdb);
                    else /* sframe */
                        sdlc_reset(idb, "S frame received neq. 2 bytes in length", sdb);
		}
                break;

            case SDLC_FRMR_W:
                sdlc_reset(idb, "Unknown U frame received", sdb);
                break;

            case SDLC_FRMR_Z:
                sdlc_reset(idb, "Invalid Nr", sdb);
                break;
        }
    }

    /* Setup the event for the SNA NETWORK MANAGEMENT or SNMP TRAP.
     */
    switch (frmr_reason){
        case SDLC_FRMR_Y:
            event = SDLC_MaxIFieldExceeded;                   /*NmEvent Hook 1*/
            break;

        case SDLC_FRMR_X|SDLC_FRMR_W:
            if (sdb->sdlc_state == SDLC_ERROR) 
                event = SDLC_RxUFrameWithIFieldInErrorState;/*NmEvent Hook 14*/
            else if (SDLC_ISUFRAME(frame))
                event = SDLC_RxUFrameWithIField;            /*NmEvent Hook 3*/
            else /* sframe */
                event = SDLC_RxSFrameWithIField;            /*NmEvent Hook 11*/
            break;

        case SDLC_FRMR_W:
            event = SDLC_RxInvalidUFrame;                   /*NmEvent Hook 10*/
            break;

        case SDLC_FRMR_Z:
            event = SDLC_RxInvalidNr;                       /*NmEvent Hook 17*/
            break;
        
    }
  
    /* Save the FRMR ifield regardless if FRMR is allowed or not 
     */
    sdb->sdlc_lastFailFRMRInfo[0] = control;
    if (poll)
	sdb->sdlc_lastFailFRMRInfo[0] |= SDLC_PF;
    sdb->sdlc_lastFailFRMRInfo[1] = (sdb->sdlc_vs << 1) | ((secondary ? 0 : 1) << 4) | (sdb->sdlc_vr << 5);
    sdb->sdlc_lastFailFRMRInfo[2] = frmr_reason;

    if (event != -1)
        SdlcNmEvent(event, idb, sdb); 
}

inline static void sdlc_setstate_force_polled(
     hwidbtype *idb,
     sdlc_data_block *sdb)
{
     if (ISPSDLC(idb)) {
         if (idb->sdlc_current_polled == 0) {
             idb->sdlc_current_polled = sdb->sdlc_address;
             if (!TIMER_RUNNING(idb->sdlc_top_timer)) {
                 GET_TIMESTAMP(idb->sdlc_top_timer);
             } else {
		 if (SDLC_DEBUG(sdb, NULL))
		     buginf("\nSDLC: current polled 0 but timer running.");
	     }
         } else if (!TIMER_RUNNING(idb->sdlc_top_timer)) {
	     if (SDLC_DEBUG(sdb, NULL)) 
		 buginf("\nSDLC: current polled set but timer off.");
	 }
     } else
         TIMER_START(sdb->sdlc_poll_starve_timer, idb->sdlc_poll_wait);
}


/*
 * set the SDLC state of a link
 */
void sdlc_setstate (hwidbtype *idb, int newstate, sdlc_data_block *sdb)
{
    leveltype level;
    int i, prevstate;
    paktype *pak;

    if (!sdb)
        return;

    level = raise_interrupt_level(NETS_DISABLE);
    if (idb->state != IDBS_UP && newstate != SDLC_DISCONNECT) {
        if (sdllc_debug || sdlc_qllc_debug)
            buginf("\nSDLC_STATE: interface %s %02x down, changing %s to %s",
                   idb->hw_namestring, sdb->sdlc_address, 
                   sdlc_prstate(NULL, newstate), sdlc_prstate(NULL, SDLC_DISCONNECT));
        newstate = SDLC_DISCONNECT;
    }

    if (newstate == SDLC_XIDSENT) {
        if ((idb->sdlc_current_polled == sdb->sdlc_address) ||
            (idb->sdlc_current_polled == 0)) { /* If it's our turn, or no one else */
                idb->sdlc_current_polled = sdb->sdlc_address;
        }
    }
    else if (newstate == SDLC_XIDSTOP) {
	/*
	 * Return the idb->sdlc_top_timer to it's regular 1 second interval.
	 * This will allow detection of poll starvation if the router
	 * winds up acting as a secondary station.
	 */
	TIMER_START(idb->sdlc_top_timer, ONESEC);
    }

    if ((newstate == SDLC_DISCONNECT) || (newstate == SDLC_DISCSENT))
        sdb->sdlc_xid_count = 0;

    if (sdb->sdlc_state != newstate || newstate == SDLC_DISCONNECT) {
        prevstate = sdb->sdlc_state;
        sdb->sdlc_ready_for_poll = FALSE;
        /*
         * Do any special actions dependent on previous state.
         */
        if (prevstate == SDLC_SNRMSENT) {
            /*
             * If being taken out of SNRMSENT we might have owned
             * the slow poll timer.  Give it up if so.
             */
            if (idb->sdlc_slow_polled == sdb->sdlc_address) {
                idb->sdlc_slow_polled = 0;
                TIMER_STOP(idb->sdlc_slow_poll_timer);
            }
        }

        /*
         * Do special newstate dependent actions.
         */
        if (newstate == SDLC_DISCONNECT) {
           /*
            * There should be a settimer(SDLC_SET_PAUSE_TIMER_RFP) here but
            * we can't because something is screwy with the data structures
            * and it causes a crash.  This needs to be fixed.
            */
           sdb->sdlc_vs = 0;
           sdb->sdlc_vr = 0;
           TIMER_STOP(sdb->time_linkup_sent);
           sdb->sdlc_rvr = 0;
           sdb->sdlc_seqmap = 0;
           sdb->stun_peer_busy = FALSE;
           sdb->stun_local_busy = FALSE;
           sdb->stun_init_busy = FALSE;

           if (sdb->sdlc_frmrpak) {
              datagram_done(sdb->sdlc_frmrpak);
              sdb->sdlc_frmrpak = NULL;
           }

           /*
            * Special STUN TG logic for attemping to deliver I-frames
            * on sdlc link queues.
            */
           if (prevstate != SDLC_DISCONNECT)
               stun_tg_sdlc_fail(idb, sdb);
           for (i = 0; i < SDLC_MODULO; i++) {
              if (sdb->sdlc_paks[i]) {
                  datagram_done(sdb->sdlc_paks[i]);
                  sdb->sdlc_paks[i] = NULL;
              }
           }
           while ((pak = pak_dequeue(&sdb->sdlc_holdq)))
               datagram_done(pak);

        } else
            sdlc_setstate_force_polled(idb, sdb);
        sdb->sdlc_state = newstate;
        sdb->sdlc_retcnt = 0;
        sdb->sdlc_iframeRetCnt = 0;
        if (newstate >= SDLC_CONNECT && prevstate < SDLC_CONNECT) {
	    if (!idb->sdlc_grp_start_poll)
		idb->sdlc_grp_start_poll = sdb->sdlc_address;
	    if (sdb->qllc_have_prtnr)
		sdlc_ind_connect_snx(idb, sdb);
	    else
		sdllc_ind_connect(idb, sdb);
	}
        else if (newstate < SDLC_SNRMSENT) {
            /*
             * disconnect is special in that we don't care what the previous
	     * state was.  Anytime we are disconnected we give the indication -
	     * except if the station isn't really opened
	     */
	    if (ISCLSSDLC(idb) && sdb->u_sap_id) {
		sdlc_cls_reset(sdb);
	    } else {
		if (sdb->qllc_have_prtnr)
		    sdlc_ind_disconnect_snx(idb, sdb);
		else
		    sdllc_ind_disconnect(idb, sdb);
	    }
        }
    }
    reset_interrupt_level(level);
}


/*
 * sdlc_settimer(idb, value, addr)
 * - Set SDLC timer to a given value. Only primary sides have timers, the
 * secondary has one overall timer (for idle time since last poll).
 */
void sdlc_settimer (hwidbtype *idb, int delta_time, sdlc_data_block *sdb)
{
    leveltype status;

    if (ISPSDLC(idb)) {
	status = raise_interrupt_level(NETS_DISABLE);
	sdb->sdlc_ready_for_poll = FALSE;
	TIMER_START(idb->sdlc_top_timer, delta_time);
	reset_interrupt_level(status);
    } else {
        /*
         * If the sdlc role is "none" or "prim-xid-poll" and sending
         * Proxy XID's, then we need set the top timer so XIDs
         * can be re-tried. Some end stations must receive "n" XID's
         * before they respond. Comm Mgr 2 is one example.
         */
        if ((ISCLSSDLC(sdb->sdlc_idb)) && (sdb->sdlc_state == SDLC_XIDSENT)) {
            sdb->sdlc_ready_for_poll = FALSE;
            TIMER_START(idb->sdlc_top_timer, delta_time);
        }
    }
}

/*
 * sdlc_set_pause_timer(idb, addr)
 * - Set SDLC timer to a given value. Only primary sides have timers, the
 * secondary has one overall timer (for idle time since last poll).
 */
void sdlc_set_pause_timer (hwidbtype *idb, sdlc_data_block *sdb)
{
    leveltype status;

    if (!ISPSDLC(idb))
	return;
    status = raise_interrupt_level(NETS_DISABLE);
    sdb->sdlc_ready_for_poll = FALSE;
    TIMER_START(idb->sdlc_top_timer, idb->sdlc_poll_pause_timer);
    reset_interrupt_level(status);
}

/*
 * sdlc_set_pause_timer_rfp(idb, addr)
 * - Set SDLC timer to a given value. Only primary sides have timers, the
 * secondary has one overall timer (for idle time since last poll).
 */
void sdlc_set_pause_timer_rfp (hwidbtype *idb, sdlc_data_block *sdb)
{
    leveltype status;

    if (!ISPSDLC(idb))
	return;
    status = raise_interrupt_level(NETS_DISABLE);
    sdb->sdlc_ready_for_poll = TRUE;
    TIMER_START(idb->sdlc_top_timer, idb->sdlc_poll_pause_timer);
    reset_interrupt_level(status);
}



/*
 * sdlc_encapsulate
 * Wrap SDLC around some data
 */

boolean sdlc_encapsulate (paktype *pak, int length, sdlc_data_block *sdb)
{
    hwidbtype *idb;

    idb = pak->if_output->hwptr;
    if (idb->state != IDBS_UP)
	return(FALSE);
    if ((length*8) > idb->sdlc_n1)
      return(FALSE);
    pak->datagramsize = length + SDLCHEADERBYTES;
    if ( (sdb->sdllc_lak_enable) || (sdb->sdllc_llc) || (sdb->qllcp)
	|| ISCLSSDLC(sdb->sdlc_idb))
       pak->datagramstart -= sizeof(sdlc_frametype);
    else
       pak->datagramstart = pak->network_start - SDLCHEADERBYTES;
    pak->enctype = idb->enctype;
    pak->encsize    = SDLCHEADERBYTES;
    pak->mac_start  = pak->datagramstart;
    pak->addr_start = NULL;
    pak->rif_start  = NULL;
    pak->riflen     = 0;
    pak->info_start = NULL;
    return(TRUE);
}


/*
 * sdlc_deencap
 */
static void sdlc_deencap(hwidbtype *idb)
{
    int addr;
    sdlc_data_block *sdb;
    leveltype        level;
    hwidbtype *tmpidb;

    level = raise_interrupt_level(NETS_DISABLE);

    if (idb->sdllc_active)
	sdllc_remove_vring(idb, RSRB_ET_SDLLC);


    for (addr = SDLC_MIN_ADDRESS; (addr <= SDLC_MAX_ADDRESS); addr++) {
	sdb = (sdlc_data_block *) idb->sdlc_data[addr];
	if (!sdb) 
	    continue;

	if (ISCLSSDLC(idb)) {
	    SdlcDeleteClsSdb(sdb);
	} else {
	    sdlc_reset(idb, "SDLC deencap forcing cleanup", sdb);
	    free(sdb);
	}
        idb->sdlc_data[addr] = NULL;
    }
    reg_invoke_srt_set_srb_params(idb, FALSE);

    /*
     * These following fields all need to be removed from the idb.  This
     * routine would free the sdlc allocated control block rather than null
     * out the fields.
     */
    TIMER_STOP(idb->sdlc_top_timer);
    TIMER_STOP(idb->sdlc_slow_poll_timer);
    idb->sdlc_current_polled = 0;
    idb->sdlc_role = 0;
    idb->configured_role = 0;
    idb->sdlc_t1 = 0;
    idb->sdlc_n1 = 0;
    idb->sdlc_n2 = 0;
    idb->sdlc_poll_wait = 0;
    idb->sdlc_poll_pause_timer = 0;
    idb->sdlc_poll_limit_value = 0;
    idb->sdlc_line_speed = 0;
    idb->sdlc_uc_delay = 0;
    idb->sdlc_upstate = FALSE;
    idb->sdlc_slow_polled = 0;
    idb->sdlc_slow_poll_interval = 0;
    idb->sdlc_output_flag = 0;
    idb->sdlc_group_addr = 0;
    idb->sdlc_grp_start_poll = 0;
    idb->sdlc_default = NO_SDLC;
    idb->sdlc_have_default_partner = FALSE;
    idb->sdlc_default_inbound = FALSE;
    idb->sdlc_default_outbound = FALSE;
    idb->sdlc_dlsw_u_sap_id = 0;
    ieee_zero(idb->sdlc_default_partner);

    FOR_ALL_HWIDBS(tmpidb) {
	if (is_sdlc(tmpidb) && (tmpidb != idb)) {
	    reset_interrupt_level(level);
	    return;
	}
    }

    if (sdlc_tmr) {
	destroy_timer(sdlc_tmr);
	sdlc_tmr = 0;
    }
    reset_interrupt_level(level);
}


/*
 * sdlc_holdq
 *
 * Put a packet on the SDLC holdq
 */
void sdlc_holdq (
    register hwidbtype *idb,
    sdlc_data_block *sdb,
    register paktype *pak)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    GET_TIMESTAMP(pak->inputtime);
    if (!pak_enqueue(&sdb->sdlc_holdq, pak)) {
        errmsg(&msgsym(DROPPED, SDLC));
        pak->if_output->hwptr->counters.output_total_drops++;
        datagram_done(pak);
        sdlc_reset(idb, "SDLC: sdlc_holdq queue overflow", sdb);
    }
    if ((idb->sdlc_flags & SDLC_FLAG_TWS_FULL) || (
		 sdb->sdlc_address != idb->sdlc_current_polled))
	idb->sdlc_output_flag = 1;

    reset_interrupt_level(level);
}


/*
 * sdlc_dooutput
 * Output IFRAMEs either in queue or passed in
 * If this is an SDLLC session, check if the output queue is below the
 * the threshold value to relase the flow control on the llc2 side.
 */

void sdlc_dooutput (register hwidbtype *idb, sdlc_data_block *sdb)
{
    register paktype *newpak = (paktype *)NULL;
    lak_t *lackie;
    boolean poll = FALSE;

    while ((((sdb->sdlc_vs + SDLC_MODULO) - sdb->sdlc_rvr) % SDLC_MODULO) < idb->sdlc_k) {
	newpak = pak_dequeue(&sdb->sdlc_holdq);
	if (newpak == NULL)
	    break;
	pak_lock(newpak);
	if ((((sdb->sdlc_vs + SDLC_MODULO - sdb->sdlc_rvr) % SDLC_MODULO)
				== idb->sdlc_k - 1) || QUEUEEMPTY(&sdb->sdlc_holdq)) {
	    poll = TRUE;
	}
        if(idb->sdlc_line_speed)
           serial_bcount += newpak->datagramsize;
	sdlc_sendframe(SDLC_IFRAME, poll, idb, newpak, sdb);
	sdb->sdlc_seqmap |= (1 << sdb->sdlc_vs);
	sdb->sdlc_paks[sdb->sdlc_vs] = newpak;
	sdb->sdlc_vs = ++sdb->sdlc_vs % SDLC_MODULO;

	if (sdb->sdlc_poll)
	    break;
    }

    if (ISCLSSDLC(idb)) {
	sdlc_flow_ctrl_ind(sdb);
	return;
    }


    /*
     * legacy SDLLC code -
     * check if the output queue is below the threshold value to release
     * flow control on the llc2 side. This is used by both local ack sdllc
     * and non local-ack sdllc.
     */
    if ((sdb->sdllc_local_busy == TRUE ) &&
         (((100 * sdb->sdlc_holdq.count)/sdb->sdlc_holdq.maximum) 
	  <= SDLC_TXQ_LOWATER ) ) {
       if (sdb->sdllc_lak_enable) {
            lackie = sdb2lak(idb, sdb);
            if (lackie)
                lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_txqlow);
       } else
          llc2_stateswitch( sdb->sdllc_llc, CLEAR_LOCAL_BUSY );

       sdb->sdllc_local_busy = FALSE;
    }
}


/*
 * getsdlc
 * Build a SDLC packet
 */

paktype *getsdlc (register hwidbtype *idb, char *why)
{
    register paktype *pak;

    pak = getbuffer(0);
    if (pak) {
	pak->if_output = idb->firstsw;
	pak->datagramsize = SDLCHEADERBYTES;
	pak->datagramstart = pak->network_start - SDLCHEADERBYTES;
    }
    return pak;
}

 /* When qllc or llc2 sends data to sdlc, segmentation may be required and extra
  * buffer may be needed. Buffer is available at process but not at interrupt. So
  * we need to switch to process level.
  *  The sdlc main_timer_prococess is shared with the segmentation. This is quite
  * dirty but we expected this code is not necessary once CLS is used.
  */
 
 static void SdlcSendCheck(void);
 
 /* sdlc has fragment data */
 static boolean sdlcFragmentData = FALSE;

/*
 * sdlc_prstate
 * Return pointer to string containing SDLC state.
 */

char *sdlc_prstate (sdlc_data_block *sdb, int state)
{
    if (sdb)
        state = sdb->sdlc_state;
    switch (state) {
      case SDLC_DISCONNECT:     return("DISCONNECT");
      case SDLC_DISCPEND:       return("DISCPEND");
      case SDLC_SNRMSEEN:       return("SNRMSEEN");
      case SDLC_DISCSENT:       return("DISCSENT");
      case SDLC_ERROR:          return("ERROR");
      case SDLC_XIDSTOP:	return("XIDSTOP");
      case SDLC_XIDSENT:	return("XIDSENT");
      case SDLC_SNRMSENT:       return("SNRMSENT");
      case SDLC_CONNECT:        return("CONNECT");
      case SDLC_THEMBUSY:       return("THEMBUSY");
      case SDLC_USBUSY:         return("USBUSY");
      case SDLC_BOTHBUSY:       return("BOTHBUSY");
      default:                  return("sdlc state bogus");
    }
}

/*
 * sdlc_prframe
 * Return pointer to a string describing SDLC frame.
 */

char *sdlc_prframe (
    register hwidbtype *idb,
    register paktype *pak,
    sdlc_data_block *sdb,
    uchar type, uchar poll)
{
    register sdlc_frametype *frame = (sdlc_frametype *)pak->datagramstart;
    uchar ntype;
    uchar *p = (uchar *)(frame + 1);
    int len; 

    ntype = 0;
    if (type == SDLC_NOFRAME) {
	if (SDLC_ISIFRAME(frame))
	    ntype = SDLC_IFRAME;
	else if (SDLC_ISUFRAME(frame))
	    ntype = frame->control & ~SDLC_PF;
	else if (SDLC_ISSFRAME(frame))
	    ntype = frame->control & 0xf;
    } else
	ntype = type;
    switch (ntype) {
    case SDLC_SNRM:
	len = sprintf(sdb->sdlc_buffer, "(%d) SNRM%s", pak->datagramsize,
	    poll ? " P" : "");
	break;
    case SDLC_DISC:
	len = sprintf(sdb->sdlc_buffer, "(%d) DISC%s", pak->datagramsize,
	    poll ? " P" : "");
	break;
    case SDLC_DM:
	len = sprintf(sdb->sdlc_buffer, "(%d) DM%s", pak->datagramsize,
	    poll ? " F" : "");
	break;
    case SDLC_UPOLL:
        len = sprintf(sdb->sdlc_buffer, "(%d) UP%s", pak->datagramsize,
            poll ? " F" : "");
        break;
    case SDLC_UA:
	len = sprintf(sdb->sdlc_buffer, "(%d) UA%s", pak->datagramsize,
	    poll ? " F" : "");
	break;
    case SDLC_FRMR:
	len = sprintf(sdb->sdlc_buffer, "(%d) FRMR%s %#x %#x %#x",
	    pak->datagramsize,
	    poll ? " F" : "", p[0], p[1], p[2]);
	break;
    case SDLC_SIM:
	len = sprintf(sdb->sdlc_buffer, "(%d) SIM%s", pak->datagramsize,
            poll ? " P" : "");
	break;
    case SDLC_XID:
	len = sprintf(sdb->sdlc_buffer, "(%d) XID%s", pak->datagramsize,
            poll ? " P" : "");
	break;
    case SDLC_TEST:
	len = sprintf(sdb->sdlc_buffer, "(%d) TEST%s", pak->datagramsize,
            poll ? " P" : "");
	break;
    case SDLC_CFGR:
	len = sprintf(sdb->sdlc_buffer, "(%d) CFGR%s", pak->datagramsize,
            poll ? " P" : "");
	break;
    case SDLC_BCN:
	len = sprintf(sdb->sdlc_buffer, "(%d) BCN%s", pak->datagramsize,
            poll ? " P" : "");
	break;
    case SDLC_IFRAME:
	if (type == SDLC_NOFRAME)
	    len = sprintf(sdb->sdlc_buffer, "(%d) IFRAME%s %d %d",
		pak->datagramsize,
		poll ? " P" : "",
		(frame->control >> 1) & 0x7, (frame->control >> 5) & 0x7);
	else
	    len = sprintf(sdb->sdlc_buffer, "(%d) IFRAME%s %d %d",
		pak->datagramsize,
		poll ? " P" : "", sdb->sdlc_vs, sdb->sdlc_vr);
	break;
    case SDLC_RR:
	if (type == SDLC_NOFRAME)
	    len = sprintf(sdb->sdlc_buffer, "(%d) RR%s %d", pak->datagramsize,
		poll ? " P/F" : "", (frame->control >> 5) & 0x7);
	else
	    len = sprintf(sdb->sdlc_buffer, "(%d) RR%s %d", pak->datagramsize,
		poll ? " P/F" : "", sdb->sdlc_vr);
	break;
    case SDLC_RNR:
	if (type == SDLC_NOFRAME)
	    len = sprintf(sdb->sdlc_buffer, "(%d) RNR%s %d",
		pak->datagramsize,
		poll ? " P/F" : "", (frame->control >> 5) & 0x7);
	else
	    len = sprintf(sdb->sdlc_buffer, "(%d) RNR%s %d",
		pak->datagramsize,
		poll ? " P/F" : "", sdb->sdlc_vr);
	break;
    case SDLC_REJ:
	if (type == SDLC_NOFRAME)
	    len = sprintf(sdb->sdlc_buffer, "(%d) REJ%s %d",
		pak->datagramsize,
		poll ? " P/F" : "", (frame->control >> 5) & 0x7);
	else
	    len = sprintf(sdb->sdlc_buffer, "(%d) REJ%s %d",
		pak->datagramsize,
		poll ? " P/F" : "", sdb->sdlc_vr);
	break;
    default:
	len = sprintf(sdb->sdlc_buffer, "(%d) ILLEGAL %#x %#x",
	    pak->datagramsize,
	    frame->address, poll ? frame->control | SDLC_PF : frame->control);
    }
    sdb->sdlc_buffer[len] = '\0';
    return sdb->sdlc_buffer;
}

/*
 * sdlc_sendframe
 * Send a SDLC frame
 * Must be called from interrupt level 5
 */

void sdlc_sendframe (int type, boolean poll, register hwidbtype *idb, 
		     paktype *pak, sdlc_data_block *sdb)
{
    register sdlc_frametype *frame = (sdlc_frametype *)pak->datagramstart;
    boolean set_control;
    boolean non_activation_xid;
    uchar *xid;
    stunpeertype *stp;

    pak->if_output = idb->firstsw;

    set_control = TRUE;
    non_activation_xid = FALSE;
    switch (type) {
      case SDLC_SNRM:   sdb->sdlc_snrmssent++;  break;
      case SDLC_FRMR:   sdb->sdlc_frmrssent++;  break;
      case SDLC_DISC:   sdb->sdlc_discssent++;  break;
      case SDLC_DM:     sdb->sdlc_dmssent++;    break;
      case SDLC_UA:     sdb->sdlc_uassent++;    break;
      case SDLC_XID:
        xid = (((uchar *)pak->datagramstart) + SDLCHEADERBYTES);
        non_activation_xid = xid_is_nonactivation((uchar *)xid);
        sdb->sdlc_xidssent++;
        break;
      case SDLC_SIM:    ISSSDLC(idb) ? sdb->sdlc_rimssent++ : 
	  sdb->sdlc_simssent++; break; 
      case SDLC_UI:     sdb->sdlc_uiframessent++; break;
      case SDLC_REJ:    sdb->sdlc_rejssent++; break;
      case SDLC_UPOLL:  break;
      case SDLC_TEST:   
	sdb->sdlc_testssent++;
	sdb->sdlctest_sendcnt++;
	break;
      case SDLC_IFRAME:
        sdb->sdlc_iframessent++;

        /* Octets covers the address, control, and 
         * information field of iframes only.
         */
        sdb->sdlc_octetssent += pak->datagramsize; 

        if (poll)  {
            if (ISSSDLC(idb))
                sdb->sdlc_pollRspsOut++;
            else
                sdb->sdlc_pollsOut++;
        }
        frame->control = type | (sdb->sdlc_vs << 1) | (sdb->sdlc_vr << 5);
        set_control = FALSE;
        break;

      /*
       * Control field is built for S-Frames by caller
       */
      case SDLC_RNR:
        if (poll) {
            if (ISSSDLC(idb))
                sdb->sdlc_pollRspsOut++;
            else
                sdb->sdlc_pollsOut++;
        }
        sdb->sdlc_rnrssent++;
        set_control  = FALSE;
        break;

      case SDLC_RR:
        if (idb->sdlc_simultaneous && poll)
            sdb->poll_seqmap = sdb->sdlc_seqmap;

        if (poll) {
            if (ISSSDLC(idb))
                sdb->sdlc_pollRspsOut++;
            else
                sdb->sdlc_pollsOut++;
        }
        set_control = FALSE;
        break;

      case SDLC_NOFRAME:
        /* Octets covers the address, control, and 
         * information field of iframes only.
         * Note : octestssent counts retransmission of iframes
         * as well.
         */
        sdb->sdlc_octetssent += pak->datagramsize; 
        /* fall through */
      default:
        set_control = FALSE;
        break;
    }

    if (set_control)
        frame->control = type;
    if (poll)
        frame->control |= SDLC_PF;

    sdb->sdlc_lastCtrlOut = frame->control; 

    /* deal with sdlc address violations for stun TG */
    stp = stun_addr_to_peer(idb, ((char *) &(sdb->sdlc_address)));
    if ((stp) && (stp->tg_active)) {
       if (type == SDLC_SNRM) {
          /* set local SNRM address to the same as remote SNRM address */
          frame->address = sdb->tg_snrm_addr;
       } else if (sdb->stun_addr_echo) {
          /* tg echo bit logic - sec NCPs set high order bit in sdlc addr */
          if (ISPSDLC(idb))
             frame->address = sdb->sdlc_address & SDLC_ECHO_OFF;
          else
             frame->address = sdb->sdlc_address | SDLC_ECHO_ON;
       } else {
          /* frame is not snrm and echo oiption is not used */
          frame->address = sdb->sdlc_address;
       }
    } else if ((!(ISCLSSDLC(idb) && (frame->address == SDLC_BCAST_ADDR)
		 && (type == SDLC_XID))) || non_activation_xid)
	frame->address = sdb->sdlc_address;  /* address may be FF */

    if (SDLC_DEBUG(sdb, pak)) {
        buginf("\n%s: SDLC O %02x %s %s  s: %d  r: %d  rr: %d", idb->hw_namestring,
               frame->address, sdlc_prstate(sdb, 0),
               sdlc_prframe(idb, pak, sdb, type, poll), sdb->sdlc_vs, sdb->sdlc_vr, sdb->sdlc_rvr);
    }
    sdlc_packet_dump(sdb, pak, pak->if_output->namestring, "SDLC output");
    (void) holdq_enqueue(idb, pak, TAIL);
    sdb->sdlc_poll = (poll || sdb->sdlc_poll); 
    (*(idb->soutput))(idb);
}

/*
 * sdlc_validcmdframe
 * Returns TRUE iff we have a valid command frame type
 */

boolean sdlc_validcmdframe (register sdlc_frametype *frame)
{
    if (SDLC_ISIFRAME(frame))
	return TRUE;
    else if (SDLC_ISUFRAME(frame)) {
	switch (frame->control) {
	case SDLC_SNRM:
	case SDLC_DISC:
	    return TRUE;
	}
    } else if (SDLC_ISSFRAME(frame)) {
	switch (frame->control & 0xf) {
	case SDLC_RNR:
	case SDLC_RR:
        case SDLC_REJ:
	    return TRUE;
	}
    }
    return FALSE;
}

/*
 * sdlc_start
 * Start SDLC connect and ack if necessary.
 * Called when entering CONNECT state.
 * Should be called with "ack" = FALSE if enctype is SDLCP.
 */

void sdlc_start (
    register hwidbtype *idb,
    register paktype *pak,
    boolean ack,
    boolean poll,
    sdlc_data_block *sdb)
{

    sdb->sdlc_vs = 0;
    sdb->sdlc_vr = 0;
    TIMER_STOP(sdb->time_linkup_sent);
    sdb->sdlc_rvr = 0;
    sdb->sdlc_seqmap = 0;
    sdb->sdlc_retcnt = 0;
    sdb->sdlc_iframeRetCnt = 0;
    sdb->sdlc_poll = FALSE; 

    if((ISPSDLC(idb) && (sdb->stun_init_busy == TRUE)) ||
       (ISCLSSDLC(idb) && (idb->sdlc_role == SDLC_PRIMARY)))
       sdlc_setstate(idb, SDLC_USBUSY, sdb);
    else
       sdlc_setstate(idb, SDLC_CONNECT, sdb);

    if (ack) {
	pak->datagramsize = SDLCHEADERBYTES;
	sdlc_sendframe(SDLC_UA, poll, idb, pak, sdb);
    }
    sdlc_set_pause_timer_rfp(idb, sdb);

    /*
     * Notify SNMP managers that this link is up.
     */
    idb->reason = "SDLC connected";
    reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
}

/*
 * sdlc_frmr
 * Send a SDLC frame error. A FRMR may *always* be sent.
 */

static void sdlc_frmr (
    register hwidbtype *idb,
    paktype *epak,
    uchar frmr_reason, 
    uchar poll, 
    uchar cmd,
    sdlc_data_block *sdb){

    register uchar *p;
    register paktype *pak;
    register sdlc_frametype *frame = (sdlc_frametype *)epak->datagramstart;

    if ((idb->sdlc_flags & SDLC_FLAG_NOFRMR)) {
	sdlc_reset(idb, "Code error! FRMR sent when FRMR not allowed on link.",
		   sdb);
	if (sdlc_debug)
	  buginf("Code error! FMR sent when FRMR not allowed on link.");
    }
    if (!(pak = getbuffer(3))) {
	return;
    }
    pak->if_output = idb->firstsw;
    pak->datagramsize = SDLCHEADERBYTES + 3;
    p = pak->datagramstart = pak->network_start - SDLCHEADERBYTES;
    p[2] = frame->control;
    if (poll)
	p[2] |= SDLC_PF;
    p[3] = (sdb->sdlc_vs << 1) | ((cmd ? 0 : 1) << 4) | (sdb->sdlc_vr << 5);
    p[4] = frmr_reason;
    if (sdb->sdlc_frmrpak) {
	datagram_done(sdb->sdlc_frmrpak);
    }
    sdb->sdlc_frmrpak = pak;
    pak_lock(pak);
    if (SDLC_DEBUG(sdb, pak))
	buginf("\n%s: SDLC I %02x %s %s (%c)", idb->hw_namestring,
	       sdb->sdlc_address,
	       sdlc_prstate(sdb, 0), sdlc_prframe(idb, epak, sdb,
						    SDLC_NOFRAME, poll),
	       cmd ? 'C' : 'R');
    sdlc_setstate(idb, SDLC_ERROR, sdb);
    if ((ISPSDLC(idb)) || (poll)) {
	sdlc_sendframe(SDLC_FRMR, cmd ? poll : FALSE, idb, pak, sdb);
        sdlc_settimer(idb, idb->sdlc_t1, sdb);
    } else
	errmsg(&msgsym(SDLC_ERR, SDLC));
}

/* Support for the SNMP MIB.  
 *   1. Retransmits frame count
 *   2. Kill the link if iframe(s) resent > N2 
 */
static void ReTxFrameCntAndIFrameRetCnt(hwidbtype *idb, sdlc_data_block *sdb)
{
     /* Count any iframes are to be retransmitted. The number of iframes 
      * retransmited is the difference between the last transmitted
      * frame count and the frame count requested by the remote.
      *
      * Note : The following code using sign arithmetic causing the result to be -ve, it
      *        is changed to use the mask instead.
      *
      *        sdb->sdlc_retransmitsFrameCount += (sdb->sdlc_vs - sdb->sdlc_rvr)% SDLC_MODULO; 
      */
    sdb->sdlc_retransmitsFrameCount += ((sdb->sdlc_vs - sdb->sdlc_rvr)& (SDLC_MODULO -1));  


    /* If the iframes have been retransmitted more than N2 times then kill
     * the link.
     */
    if (sdb->sdlc_iframeRetCnt > idb->sdlc_n2) {
        sdb->sdlc_retriesexps++;
        SdlcNmEvent(SDLC_PollRetriesExhausted_IFrameReSent, idb, sdb);/*NmEvent Hook 20*/
    }
}

/*
 * sdlc_give_pending_iframes
 *
 * This routine "dumps" any pending I-Frames out to the remote side. usually,
 * this is done immediately before we would do a "P/F" RR. Call this *only*
 * when the interface is in the "CONNECT" state. The code is written this way
 * so multipoint is easier to write.
 */
static void sdlc_give_pending_iframes (
    hwidbtype *idb,
    sdlc_data_block *sdb)
{
    int ns;
    sdlc_frametype *newframe;
    boolean poll = FALSE;

    /* if remote peer is receiver-busy, don't send any I frames */
    if ((sdb->sdlc_state == SDLC_THEMBUSY) ||
              (sdb->sdlc_state == SDLC_BOTHBUSY))
         return;
 
    ns = sdb->sdlc_rvr;

    if (ns != sdb->sdlc_vs)
        ReTxFrameCntAndIFrameRetCnt(idb, sdb);

    if ((1 << ns) & sdb->sdlc_seqmap) {

	/*
	 * Catch special case where last few IFRAMEs
	 * in window could be lost
	 */
	while (ns != sdb->sdlc_vs) {
	    newframe = (sdlc_frametype *)
		sdb->sdlc_paks[ns]->datagramstart;
	    newframe->control = SDLC_IFRAME | (ns << 1) |
		(sdb->sdlc_vr << 5);
	    pak_lock(sdb->sdlc_paks[ns]);
           /*
            * serial_bcount is used to calculated a adjusted t1.
            * this is necessary because of the delay between
            * when the frame is put on the idb output queue and
            * when the frame is physically sent out on the line.
            */
            if(idb->sdlc_line_speed)
              serial_bcount += sdb->sdlc_paks[ns]->datagramsize;
	    if ((ns == (sdb->sdlc_vs - 1)) && QUEUEEMPTY(&sdb->sdlc_holdq))
		poll = TRUE;

	    sdlc_sendframe(SDLC_NOFRAME, poll, idb,
			   sdb->sdlc_paks[ns], sdb);

	    ns = ++ns % SDLC_MODULO;
	}
    }    
    /*
     * If hold queue non-empty, and we are not waiting for a
     * response from a poll to the other side, and we are
     * connected, then output some I-Frames to the other side.
     * 
     */
    if (!sdb->sdlc_poll && !QUEUEEMPTY(&sdb->sdlc_holdq)) {
	sdlc_dooutput(idb, sdb);
    }
}

/*
 * sdlc_secondary_response
 *
 * Handle responses as SDLC secondary side. Assumes that it is called only
 * as a response to a frame with a POLL bit set.
 */
static void sdlc_secondary_response (
    hwidbtype *idb,
    sdlc_data_block *sdb,
    paktype *pak,
    boolean rejin)
{
    paktype *ourpak = pak;

    /*
     * First, send any pending output, if the other side is not busy
     */
    if ((sdb->sdlc_state == SDLC_CONNECT) ||
	(sdb->sdlc_state == SDLC_USBUSY)) {
	sdlc_give_pending_iframes(idb, sdb);
    }
    /* Final bit has been sent, either pigyback on an I frame or
     * Send in an RNR frame.
     */
    if (sdb->sdlc_poll) {
        /* If following poll starve timer pops, we reset the link */
        TIMER_START(sdb->sdlc_poll_starve_timer, idb->sdlc_poll_wait);
        retbuffer2(pak);
	return;
    }

    /*
     * Now, send a final RR or RNR depending on *our* busy state.
     */
    if (ourpak == NULL) {
        ourpak = getsdlc(idb, "RR or RNR, sec rsp");
	if (!ourpak) {
	    return;
        }
    } else 
        ourpak->datagramsize = SDLCHEADERBYTES;
    ourpak->datagramstart[0] = sdb->sdlc_address;
    ourpak->datagramstart[1] = ((sdb->sdlc_state == SDLC_CONNECT) ||
				(sdb->sdlc_state == SDLC_THEMBUSY)) ?
				    SDLC_RR : SDLC_RNR;
    ourpak->datagramstart[1] |= SDLC_PF | (sdb->sdlc_vr << 5);
    sdlc_sendframe(((sdb->sdlc_state == SDLC_CONNECT) ||
		    (sdb->sdlc_state == SDLC_THEMBUSY)) ? SDLC_RR :
		   SDLC_RNR, TRUE, idb, ourpak, sdb);

    /* If following poll starve timer pops, we reset the link */
    TIMER_START(sdb->sdlc_poll_starve_timer, idb->sdlc_poll_wait);
}



#define RNRLIMIT_TIMER_SET(idb, sdb) ((idb->sdlc_rnrLimit != SDLC_DEFAULT_RNRLIMIT) && \
                               TIMER_RUNNING(sdb->sdlc_rnrLimitTimer))

#define RNRLIMIT_TIMER_NOT_SET(idb, sdb) ((idb->sdlc_rnrLimit != SDLC_DEFAULT_RNRLIMIT) && \
                               !TIMER_RUNNING(sdb->sdlc_rnrLimitTimer))


/* Times out the remote busy condition. When the rnr-limit timer 
 * expires, the station is reset and the rnr-limit expiration counter
 * is incremented.
 */
void SdlcRNRTimerLimitCheck(hwidbtype *idb, sdlc_data_block *sdb) 
{
    if (RNRLIMIT_TIMER_SET(idb, sdb) && AWAKE(sdb->sdlc_rnrLimitTimer)){
        sdlc_reset(idb, "SDLC RNR Limit Timer Expired", sdb);
        SdlcNmEvent(SDLC_RNRLimit, idb, sdb);   /* NmEvent Hook 22*/ 
        sdb->sdlc_rnrLimitCount++;
        TIMER_STOP(sdb->sdlc_rnrLimitTimer);
    }
}



/*
 * sdlc_input
 * Handle incoming SDLC frames.
 * Called from device interrupt.
 */

void sdlc_input (register hwidbtype *idb, paktype *pak)
{
    register sdlc_frametype *frame = (sdlc_frametype *)pak->datagramstart;
    int poll;
    uchar control;
#ifdef FULL_I_FRAMES
    uchar *place;
    int count;
    int addr;
#endif
    int nr, ns;
    uchar *p = (uchar *)(frame + 1);
    boolean secondary;
    char *why;
    int addr = frame->address;
    sdlc_data_block *sdb;
    stunpeertype *stp;
    sdlc_data_block *first_sdb;
    int disc_addr;
    paktype *dumppak;

    /*
     * If CLS or STUN, packet has already been counted.
     */
    if (!ISCLSSDLC(idb) && !(idb->enctype == ET_STUN))
	sdlc_count_input(idb, pak);

    /*
     * If secondary, it is not valid for anything to still be on the interface
     * output queue unless we are runnign TWS.  But this can happen if we had 
     * frames to send, but we had an rts-timeout.  Diagnose the condition and 
     * correct it.
     */

    if (!idb->sdlc_simultaneous) {
	if (!QUEUEEMPTY(&idb->outputq[PRIORITY_NORMAL])) {
	    while ((dumppak = holdq_dequeue(idb)))
		datagram_done(dumppak);
	}
    }

    /* If group-poll is supported on the interface check for group-poll */
    if ((frame->control & ~SDLC_PF) == SDLC_UPOLL) {
	if (!idb->sdlc_group_addr) {
	    errmsg(&msgsym(INVLDGRPPOLL, SDLC), idb->hw_namestring, addr);
	    retbuffer2(pak);
	    return;
	}
	if (addr != idb->sdlc_group_addr) {
	    errmsg(&msgsym(INVLDGRPCFG, SDLC), idb->hw_namestring, addr);
	    retbuffer2(pak);
	    return;
	}
	if (idb->sdlc_grp_start_poll) {
	    sdb = first_sdb = idb->sdlc_data[idb->sdlc_grp_start_poll];
	    addr = disc_addr = 0;
	    /* 
	     * We have to loop through all the active link stations and reset
	     * the poll starvation timers. Also we have to find the first   
	     * link station that is either disconnected or has data to send.
	     */
	    do {
		if (sdb->sdlc_state != SDLC_DISCONNECT) {
		    /* 
		     *  Reset the poll starvation timer and see
		     *  if any link station has gone down or 
		     *  if any link station has data to send 
		     */
		    TIMER_START(sdb->sdlc_poll_starve_timer, idb->sdlc_poll_wait);
		    if (!disc_addr) {
			if (sdb->sdlc_state == SDLC_DISCSENT)
			    disc_addr = sdb->sdlc_address;
			if (!addr)
			    if (!QUEUEEMPTY(&sdb->sdlc_holdq))
				addr = sdb->sdlc_address;
		    }
		    if (sdb->sdlc_state == SDLC_CONNECT ||
			sdb->sdlc_state == SDLC_USBUSY) {
			idb->sdlc_grp_start_poll = sdb->sdlc_address;
		    }
		}
		sdb = idb->sdlc_data[sdb->next];
	    } while (sdb != first_sdb);
	    if (disc_addr || addr) {
		if (disc_addr) {
		    /* 
		     * If any link station is down send a RD to the FEP 
		     * The FEP takes a while to clean up, so let's update
		     * the start_poll so that we round-robin on the RD as
		     * well as the UP
		     */
		    sdb = idb->sdlc_data[disc_addr];
		    pak->datagramsize = SDLCHEADERBYTES;
		    sdlc_sendframe(SDLC_RD, TRUE, idb, pak, sdb);
		    sdb->sdlc_state = SDLC_DISCPEND;
		    idb->sdlc_grp_start_poll = sdb->sdlc_address;
		}
		else {
		    sdb = idb->sdlc_data[addr];
		    sdb->sdlc_poll = FALSE;
		    if ((sdb->sdlc_state == SDLC_CONNECT) ||
			(sdb->sdlc_state == SDLC_USBUSY)) {
			sdlc_dooutput(idb, sdb);
			if (sdb->sdlc_poll)
			    retbuffer2(pak);
		    }
		    if (!sdb->sdlc_poll) {
			pak->datagramsize = SDLCHEADERBYTES;
			sdlc_sendframe(((sdb->sdlc_state == SDLC_CONNECT) ||
					(sdb->sdlc_state == SDLC_THEMBUSY)) ? 
				       SDLC_RR : SDLC_RNR, TRUE, idb, pak, sdb);
		    }
		}
	    } else {
		/* 
		 * If there is nothing to send then respond with an UP 
		 */
		sdb = idb->sdlc_data[idb->sdlc_grp_start_poll];
		pak->datagramsize = SDLCHEADERBYTES;
		sdlc_sendframe(SDLC_UPOLL, TRUE, idb, pak, sdb);
	    }
	    return;
	}
    }
    /*
     * If the idb uses_tg and tg_active for the stp, then treat the frame
     * like it contains the configured sdlc address. The default for " addr"
     * is the frame address (set above).
     */
    if ((idb->stun_group) && (stun_pinfo[idb->stun_group]->uses_tg) &&
        (stp = (stunpeertype *) idb->stun_Q.qhead)) {
        addr = stp->addr[0];
    }

    sdb = idb->sdlc_data[addr];
    if (!sdb) {
	/*
	 * This is not an error, because maybe the router is only a
	 * subset of the stations.  However, it might be a config error,
	 * so supply a debug message.
	 */
	if (sdlc_packet_debug)
	    buginf("SDLC: %s, Received input for unknown address %02x. Tossing frame.",
		   idb->hw_namestring, frame->address);
	retbuffer2(pak);
	return;
    }
    if (frame->control & SDLC_PF)
        poll = TRUE;
    else
        poll = FALSE;

    secondary = ISSSDLC(idb);

    sdb->stun_frame_ok = FALSE;

    if ((ISPSDLC(idb)) && (addr != idb->sdlc_current_polled)) {
	if (SDLC_DEBUG(sdb, pak))
            buginf("\n%s R R %02x data from wrong address!  got address %02x",
               idb->hw_namestring, idb->sdlc_current_polled, addr);
	retbuffer2(pak);
	return;
    }
    sdb->sdlc_lastCtrlIn = frame->control;
    frame->control &= ~SDLC_PF;

    /*
     * If router is secondary, respond to TEST P regardless of sdlc state.
     */
    if (secondary && (frame->control == SDLC_TEST)) {
        sdlc_sendframe(SDLC_TEST, TRUE, idb, pak, sdb);
        return;
    }

    if (poll && IS_SDLC_CONNECT(sdb)) {
            /*
             * While we are waiting something could have changed our
             * state.  Then the incoming happened that is now using
             * the timer (like a state change).  Only set the timer
             * back to polling types of things if we are in a connect
             * state.
             */
	     sdb->sdlc_poll = FALSE; 
             sdlc_set_pause_timer_rfp(idb, sdb);
     }
    
    if (idb->state != IDBS_UP) {
	retbuffer2(pak);
	return;
    }

    /* 
     * Check if the SDLC Test command is running 
     */
    if (!secondary && (sdb->test_cnt) && (frame->control == SDLC_TEST)) {
	sdb->sdlctest_rcvcnt ++;
	retbuffer2(pak);
	sdlc_set_pause_timer_rfp(idb, sdb);
        return;
    }
    
    switch (sdb->sdlc_state) {
      case SDLC_DISCONNECT:
      case SDLC_SNRMSEEN:
      case SDLC_XIDSENT:
      case SDLC_XIDSTOP:
	/*
	 * DISCONNECT:
	 *    Output UA to incoming SNRM, enter CONNECT
	 *    Output DM to incoming IFRAME/P, DISC, or any command/P
	 *    Otherwise, ignore frame.
	 *
	 * DISCSENT code jumps here if SNRM received while we are a secondary
	 */
      snrmrcvd:
	if (sdllc_debug)
	    buginf("\nsnrmrcvd:");
        if (secondary) {
            if (pak->datagramsize == SDLCHEADERBYTES && frame->control == SDLC_SNRM) {
		if (sdllc_pid && idb->sdllc_active) {
		    GET_TIMESTAMP(sdb->sdlc_sec_linkup_time);
		    /*
		     * keep resetting the timer so long as we keep receiving SNRMs
		     * to bring up the connection  on the network side
		     */
		    if (sdb->sdllc_state < SDLLC_AW_SDLC_SEC) {
			if (sdb->sdllc_state == SDLLC_DISCONNECT) {
			    sdllc_setstate(idb, sdb, SDLLC_AW_NET, "SDLC Sec connect (net wait)");
			    sdllc_initiate_pu(idb, sdb);
			} 
			if (idb->sdlc_group_addr)
			    sdlc_sendframe(SDLC_DM, poll, idb, pak, sdb);
			retbuffer2(pak);
			break;
		    }
		}
		if (sdllc_debug)
		    buginf("\n calling sdlc start");
                sdlc_start(idb, pak, TRUE, poll, sdb);
            } else if ((poll && SDLC_ISIFRAME(frame)) ||
                       (pak->datagramsize == SDLCHEADERBYTES && frame->control == SDLC_DISC) ||
                       (poll && sdlc_validcmdframe(frame))) {
                pak->datagramsize = SDLCHEADERBYTES;
                sdlc_sendframe(SDLC_DM, poll, idb, pak, sdb);
            } else {
                retbuffer2(pak);
                break;
            }
        } else {
            /*
             * Primary
             */

	  /* 
	     Code to handle FRMR was added for cash dispensers that
	     reply FRMR when a line error occurs. This was needed to
	     compete with Motorola Tpads, which already handle this
	     behavior. 
	     Because the station will respond to our DISC with DM,
	     it is possible that a debug message will be generated
	     if the current polled address doesn't match the one
	     in the response. This is benign and can be ignored.
          */
	  if ((frame->control & ~SDLC_PF) == SDLC_FRMR) {
	    if (sdlc_debug)
	      buginf("\nSDLC Station %x sent FRMR.",frame->address);
	    pak->datagramsize = SDLCHEADERBYTES;
	    sdlc_sendframe(SDLC_DISC, TRUE, idb, pak, sdb);
	    sdlc_setstate(idb, SDLC_DISCONNECT, sdb);
          } else if (pak->datagramsize == SDLCHEADERBYTES && 
		      (frame->control & ~SDLC_PF) == SDLC_DM)
	    retbuffer2(pak);
	  else if ((poll && SDLC_ISIFRAME(frame)) ||
                     (pak->datagramsize == SDLCHEADERBYTES && frame->control == SDLC_DISC) ||
                     (poll && sdlc_validcmdframe(frame))) {
                pak->datagramsize = SDLCHEADERBYTES;
                sdlc_sendframe(SDLC_DM, poll, idb, pak, sdb);
            } else {
		if (sdllc_debug)
		    buginf("\ninvalid control byte");
                retbuffer2(pak);
                break;
            }
        }
	break;

    case SDLC_SNRMSENT:
	/*
	 * SNRMSENT:
	 *    Enter CONNECT if UA seen
	 *    Otherwise ignore frame.
	 */
	switch (frame->control & ~SDLC_PF) {
	  case SDLC_UA:
	    if (pak->datagramsize == SDLCHEADERBYTES) 
		sdlc_start(idb, NULL, FALSE, poll, sdb);
	    break;
	  case SDLC_SNRM:
	    errmsg(&msgsym(CONFIGERR, SDLC), idb->hw_namestring,
		   sdb->sdlc_address, frame->control);
	    break;
	  case SDLC_DM:
	    if (SDLC_DEBUG(sdb, NULL))
		buginf("\n%s SDLC addr %02x received DM in response to SNRM");
	    break;
	  default:
	    /*
	     * Something other than UA or DM in response to our SNRM
	     */
	    errmsg(&msgsym(NOUA, SDLC), idb->hw_namestring, 
		   sdb->sdlc_address, frame->control);
	}		    
	retbuffer2(pak);
	break;
    case SDLC_CONNECT:
    case SDLC_THEMBUSY:
    case SDLC_USBUSY:
    case SDLC_BOTHBUSY:
        if (poll) {
           if (ISPSDLC(idb))
	      sdlc_set_pause_timer_rfp(idb, sdb);
           else
              sdlc_settimer(idb, idb->sdlc_t1, sdb);
        }
        else
           /* Didn't get poll bit, wait. Use T1 for both PRIM and SEC */
           sdlc_settimer(idb, idb->sdlc_t1, sdb);

	if (SDLC_ISIFRAME(frame)) {

	    if ((pak->datagramsize*8) > idb->sdlc_n1) {
                SdlcFrmrCondition(idb, pak, sdb, SDLC_FRMR_Y, poll, secondary);
		retbuffer2(pak);
		break;
	    }
            if(poll) {
                /*
                 * If a station is still busy it will send an I frame not
                 * Final followed by an RNR Final.  Receiving an I Final
                 * indicates it is no longer busy.
                 */
                if(sdb->sdlc_state == SDLC_THEMBUSY)
                    sdlc_setstate(idb, SDLC_CONNECT, sdb);
                else if(sdb->sdlc_state == SDLC_BOTHBUSY)
                    sdlc_setstate(idb, SDLC_USBUSY, sdb);
            }
	    ns = (frame->control >> 1) & 07;
	    nr = (frame->control >> 5) & 07;
	    if (!sdlc_ack(idb, pak, poll, secondary, nr, sdb))
		break;
	    if (ns == sdb->sdlc_vr) {
                /*
                 * Correct sequenced IFRAME received and we are in a state
                 * where it is acceptable to receive an I-FRAME.  Increment
                 * receive state variable and pass up if FRAME contains data.
                 */
		sdb->sdlc_vr = ++sdb->sdlc_vr % SDLC_MODULO;
                /* set the stun_frame_ok flag for STUN local-ack so that the
                 * I-frame can be sent to the the other peer
                 */
                sdb->stun_frame_ok = TRUE;

		pak->datagramstart += SDLCHEADERBYTES;
		pak->datagramsize -= SDLCHEADERBYTES;
		if (pak->datagramsize > 0) {
                  if (sdllc_pid && idb->sdllc_active) {
                    pak->datagramstart -= SDLCHEADERBYTES;
                    pak->datagramsize += SDLCHEADERBYTES;
                    {
		      if (secondary && (poll)) {
			paktype *newpak;
			if ((newpak = pak_duplicate(pak))) {
			  /* Pass up to LLC2 code */
			  sdllc_output(idb, newpak);
			}
			sdlc_secondary_response(idb, sdb, pak, FALSE);
                      } else {
                        sdllc_output(idb, pak);
                      }
                    }
                  }  else if (snx_running && idb->snx_active) {
                        pak->datagramstart -= SDLCHEADERBYTES;
                        pak->datagramsize += SDLCHEADERBYTES;
                        {
                            paktype *newpak;
 
                            if ((newpak = pak_duplicate(pak)))
                                reg_invoke_snx_output(idb, newpak);
                            if (secondary && poll)
                                sdlc_secondary_response(idb, sdb, pak, FALSE);
                            else
                                retbuffer2(pak);
                        }
		    } else if (ISCLSSDLC(idb)) 
		      if (sdb->u_cep_id == (void*)CLS_NO_CEP) {
			if (SDLC_DEBUG(sdb, NULL))
			  buginf("\nSDLC: discarding DATA_IND for CLS, u_cep_id = CLS_NO_CEP");
                        datagram_done(pak);
		      } else {
			pak->datagramstart -= SDLCHEADERBYTES;
                        pak->datagramsize += SDLCHEADERBYTES;
		        {
			    paktype *newpak;
			    if ((newpak = pak_duplicate(pak))) {
				sdb->inbuf = newpak;
                                sdlcClsNotify(sdb, CLS_DATA_IND, CLS_OK);
			    }
                            if (secondary && poll)
                                sdlc_secondary_response(idb, sdb, pak, FALSE);
                            else
                                retbuffer2(pak);
			}
		  } else {
                    /* Quietly toss the packet if in test mode */
			if (sdb->sdlc_testmode) {
                      retbuffer2(pak);
                    } else {
                      raw_enqueue(idb, pak);
                    }
                    if (secondary && (poll)) {
                      sdlc_secondary_response(idb, sdb, NULL, FALSE);
                    }
                  }
                } else {
                  if (secondary && (poll)) {
                    sdlc_secondary_response(idb, sdb, pak, FALSE);
                  } else {
                    retbuffer2(pak);
                  }
                }
	    } else {
                if (secondary && (poll)) {
                    sdlc_secondary_response(idb, sdb, pak, FALSE);
		} else {
		    retbuffer2(pak);
		}
	    }
	} else if (SDLC_ISUFRAME(frame)) {
	    why = NULL;
	    if ((frame->control == SDLC_FRMR && pak->datagramsize !=
		SDLCHEADERBYTES + SDLC_FRMR_DATA) ||
		(frame->control != SDLC_FRMR && pak->datagramsize != 
		SDLCHEADERBYTES)) {
                  SdlcFrmrCondition(idb, pak, sdb, SDLC_FRMR_X|SDLC_FRMR_W, poll, secondary);
		  retbuffer2(pak);
		  break;
	    }
	    switch (frame->control) {
	    case SDLC_SNRM:
                /*
                 * SNRM when connected resets the connection.  This will tear
                 * down both the sdlc side and the network side.  We then let
                 * everything rebuild again.
                 *
                 * Alternatively we could just ignore it.
                 */
                sdlc_reset(idb, "SDLC SNRM received", sdb);
                sdb->sdlc_protocolErrs++;
                SdlcNmEvent(SDLC_RxSnrmInNrm, idb, sdb);/*NmEvent Hook 4*/
                retbuffer2(pak);
                break;
	    case SDLC_FRMR:
		/*
		 * Do we think that FRMRs cannot be supported on this link?
		 */
		if ((idb->sdlc_flags & SDLC_FLAG_NOFRMR)) {
		    why = "FRMR read, but not allowed on this link";
		} else {
		    errmsg(&msgsym(FRAMEERR, SDLC),
			   idb->hw_namestring , p[0], (p[1] >> 1) & 0x7,
			   p[1] & (1 << 4) ? 'R' : 'C', (p[1] >> 5) & 0x7,
			   p[2]);

                    /* Save the FRMR ifield into the sdb */
                    bcopy(p, sdb->sdlc_lastFailFRMRInfo, 3);

		    if (p[2] & SDLC_FRMR_W) {
			if (p[2] & SDLC_FRMR_X) {
			    errmsg(&msgsym(INFOBAD, SDLC), idb->hw_namestring);
                            SdlcNmEvent(SDLC_RxFrmrWandX, 
                                        idb, sdb);/*NmEvent Hook 5*/
			  }
			else {
			    errmsg(&msgsym(CTRLBAD, SDLC), idb->hw_namestring);
                            SdlcNmEvent(SDLC_RxFrmrW, 
                                        idb, sdb);/*NmEvent Hook 6*/
			  }
		    }
		    if (p[2] & SDLC_FRMR_Y) {
			errmsg(&msgsym(N1TOOBIG, SDLC), idb->hw_namestring);
                        SdlcNmEvent(SDLC_RxFrmrY, 
                                    idb, sdb);/*NmEvent Hook 7*/
		    }
		    if (p[2] & SDLC_FRMR_Z) {
			errmsg(&msgsym(INVNR, SDLC), idb->hw_namestring);
                        SdlcNmEvent(SDLC_RxFrmrZ, idb, sdb);/*NmEvent Hook 8*/
		    }
                    if ((p[2] & (SDLC_FRMR_W | SDLC_FRMR_X | SDLC_FRMR_Y | SDLC_FRMR_Z)) == 0)
                        SdlcNmEvent(SDLC_RxFrmrNoReason, idb, sdb); /*NmEvent Hook 2*/
		}

		/* Fall through */
	    case SDLC_DM:
	    case SDLC_UA:
		/*
		 * Enter DISCONNECT when incoming FRMR or DM or UA seen
		 */
		if (why == NULL) {
		    if (frame->control == SDLC_FRMR)
                      why = "SDLC FRMR received";
		    else if (frame->control == SDLC_DM) {
		      why = "SDLC DM received";
                      SdlcNmEvent(SDLC_RxDm, idb, sdb);/*NmEvent Hook 9*/
                    }
		    else {
		      why = "SDLC UA received";
		    }
		}   
		sdlc_reset(idb, why, sdb);
                retbuffer2(pak);
		break;
	    case SDLC_DISC:
                /*
                 * Secondary side:
                 *    Enter DISCONNECT when DISC seen and output UA.
		 * Primary side:
		 *    This is an RD. Enter DISCSENT state, eventually send
		 *    a disconnect out.
                 */
                if (secondary) {
		    sdlc_sendframe(SDLC_UA, poll, idb, pak, sdb);
		    sdlc_reset(idb, "SDLC DISC received", sdb);
                } else {
		    sdlc_setstate(idb, SDLC_DISCSENT, sdb);
                    sdlc_set_pause_timer(idb, sdb);
		    retbuffer2(pak);
                }
                break;
	    default:
		/*
		 * Otherwise, received illegal frame so respond with FRMR
		 * and enter ERROR
		 */
                SdlcFrmrCondition(idb, pak, sdb, SDLC_FRMR_W, poll, secondary);
		retbuffer2(pak);
	    }
	} else if (SDLC_ISSFRAME(frame)) {
	    if (pak->datagramsize != 2) {
                SdlcFrmrCondition(idb, pak, sdb, SDLC_FRMR_X|SDLC_FRMR_W, poll, secondary);
		retbuffer2(pak);
		break;
	    }
	    nr = (frame->control >> 5) & 0x7;
	    control = frame->control & 0xf;
            if ((control != SDLC_RNR) && (control != SDLC_RR) &&
                (control != SDLC_REJ)) {
		/*
		 * Received illegal frame so reset.
		 */
		if (poll)
		    frame->control |= SDLC_PF;
                sdb->sdlc_protocolErrs++;
                SdlcNmEvent(SDLC_RxInvalidSFrame, 
                            idb, sdb); /*NmEvent Hook 12 */
		sdlc_reset(idb, "Received S frame that is not RR/RNR", sdb);
		retbuffer2(pak);
		break;
	    }
	    if (!sdlc_ack(idb, pak, poll, secondary, nr, sdb)) 
		break;
	    /*
	     *  If sdllc, and not sdllc local ack, release busy state if LLC tx queue
	     *  has drained.  If local ack, there is no LLC or LLC txq.
	     */
	    if (sdllc_pid && idb->sdllc_active &&
		(sdb->sdlc_state == SDLC_USBUSY || sdb->sdlc_state == SDLC_BOTHBUSY))
		 if ((sdb->sdllc_llc != NULL) &&
		     (llc_get_oqueue_status(sdb->sdllc_llc) <
		      DLC_OQUEUE_NORMAL)) {
		     sdlc_set_clear_busy(idb, sdb->sdlc_address, FALSE);
		     if (SDLC_DEBUG(sdb, NULL))
			 buginf("\nSDLLC: releasing flow control,txQ drained");
		 }
	    switch (control) {
	    case SDLC_RNR:
		/*
		 * Enter BUSY and start T1 timer for command or response.
		 */
		switch (sdb->sdlc_state) {
		case SDLC_THEMBUSY:
		case SDLC_CONNECT:
		    sdlc_setstate(idb, SDLC_THEMBUSY, sdb);
		    break;
		default:
		    sdlc_setstate(idb, SDLC_BOTHBUSY, sdb);
		    break;
		}

                if (poll)
                    sdb->sdlc_retcnt = 0;
                break;

                /* Start the RNR Limit Timer if it has not  
                 * already been started. rnrLimit is in minutes
                 * so convert to msec.
                 */
		if (RNRLIMIT_TIMER_NOT_SET(idb, sdb))
		   TIMER_START(sdb->sdlc_rnrLimitTimer, idb->sdlc_rnrLimit * ONEMIN);

		break;

	    case SDLC_RR:
		/*
		 * When RR/F seen,
		 *    clear timers and ret. count.
		 *    reset state
		 *    perform any retransmissions
		 */
                switch (sdb->sdlc_state) {
                  case SDLC_CONNECT:
                    break;
                  case SDLC_THEMBUSY:
                    sdlc_setstate(idb, SDLC_CONNECT, sdb);
                    break;
                  default:
                    sdlc_setstate(idb, SDLC_USBUSY, sdb);
                    break;
                }
 
                if (poll)
                    sdb->sdlc_retcnt = 0;

    
                /* We have received an RR. This stops a remote busy condition. 
                 * So, stop the RNR Limit Timer if it is started.
                 */
                if (RNRLIMIT_TIMER_SET(idb, sdb))
                    TIMER_STOP(sdb->sdlc_rnrLimitTimer); 

                break;

            case SDLC_REJ:

                /*
                 * Return to CONNECT and stop timer for
                 * command or response if BUSY.
                 */
                switch (sdb->sdlc_state) {
                case SDLC_THEMBUSY:
                case SDLC_CONNECT:
                  sdlc_setstate(idb, SDLC_CONNECT, sdb);
                  break;
                default:
                  sdlc_setstate(idb, SDLC_USBUSY, sdb);
                  break;
                }
                sdb->sdlc_iframeRetCnt = 0;

                sdlc_set_pause_timer_rfp(idb, sdb);
                break;

            }
	    /* Handle any required responses as secondary */
            if (secondary && poll) {
                sdlc_secondary_response(idb, sdb, pak, (control == SDLC_REJ));
            } else {
                retbuffer2(pak);
            }
	}
	break;
      case SDLC_DISCSENT:
      case SDLC_DISCPEND:
	/*
	 * DISCSENT:
	 *
	 * If this is the primary side:
	 *    Output DM if SNRM seen
	 *    Enter DISCONNECT if UA or DM seen.
	 *    Otherwise, discard frame.
	 *
	 * If this is the secondary side:
	 *    Jump to 'snrmseen' code if SNRM received.
	 *    Output UA if DISC seen and enter DISCONNECT.
	 *    Otherwise, if any other frame with a poll bit is received, 
	 *    		 send RD frame and stay in this state.
	 */
        if (secondary) {
            if (pak->datagramsize == SDLCHEADERBYTES && frame->control == SDLC_SNRM) {
                sdlc_reset(idb, "SDLC SNRM received (secondary)", sdb);
                goto snrmrcvd;
            } else if (pak->datagramsize == SDLCHEADERBYTES && frame->control == SDLC_DISC) {
                sdlc_sendframe(SDLC_UA, poll, idb, pak, sdb);
                sdlc_reset(idb, "SDLC DISC received (secondary)", sdb);
                break;
            } else if (poll) {
                pak->datagramsize = SDLCHEADERBYTES;
                sdlc_sendframe(SDLC_RD, TRUE, idb, pak, sdb);
                break;
            }
        } else {
            /*
             * Primary
             */
            if (pak->datagramsize == SDLCHEADERBYTES) {
                if (frame->control == SDLC_SNRM) {
                    sdlc_sendframe(SDLC_DM, poll, idb, pak, sdb);
                    sdlc_set_pause_timer_rfp(idb, sdb);
                    break;
                } else if (frame->control == SDLC_UA || frame->control == SDLC_DM) {
                    sdlc_set_pause_timer_rfp(idb, sdb);
                    sdlc_reset(idb, "disconnect successful", sdb);
                    retbuffer2(pak);
                    break;
                }
            }
        }
        if (SDLC_DEBUG(sdb, NULL))
            buginf("\nSDLC: DISCSENT and received %02x", frame->control);
        retbuffer2(pak);
        break;
    case SDLC_ERROR:
	/*
	 * If the code gets to here, then FRMRs *must* have been allowed on
	 * the link.
	 */
	if (SDLC_ISUFRAME(frame)) {
	    if ((frame->control == SDLC_FRMR && pak->datagramsize !=
		SDLCHEADERBYTES + SDLC_FRMR_DATA) ||
	        (frame->control != SDLC_FRMR && pak->datagramsize !=
		SDLCHEADERBYTES)) {

                SdlcFrmrCondition(idb, pak, sdb, SDLC_FRMR_X|SDLC_FRMR_W, poll, secondary);
		retbuffer2(pak);
		break;
	    }
	    switch (frame->control) {
	    case SDLC_SNRM:
	    case SDLC_DISC:
		/*
		 * Output UA when SNRM seen, enter CONNECT.
		 * Output UA when DISC seen, enter DISCONNECT
		 */
		if (!secondary) {
		    if (sdllc_debug)
			buginf("\nreceived SNRM and not secondary");
                    /*
                     * Primary ignores
                     */
		    retbuffer2(pak);
		    break;
		}
		sdlc_reset(idb, "SDLC SNRM or DISC received", sdb);
		if (frame->control == SDLC_SNRM) 
		    sdlc_start(idb, pak, TRUE, poll, sdb);
		else {
		    sdlc_sendframe(SDLC_UA, poll, idb, pak, sdb);
		    sdlc_setstate(idb, SDLC_DISCONNECT, sdb);
		    sdlc_set_pause_timer(idb, sdb);
		}
		break;
	    case SDLC_FRMR:
	    case SDLC_DM:
		/*
		 * If input is response, retransmit original FRMR.
                 * If in loopback, respond with SNRM and enter SNRMSENT.
                 * Otherwise reset.  SNRM will be sent when restarted by SDLLC.
		 */
		if (secondary) {
		    pak_lock(sdb->sdlc_frmrpak);
                    /* Note that no SNA Network Management Alert will be sent on
                     * retransmition of FRMRs since one FRMR alert is sufficient.
                     */
		    sdlc_sendframe(SDLC_FRMR, sdlc_validcmdframe(frame) ?
			poll : FALSE, idb, sdb->sdlc_frmrpak, sdb);
		    retbuffer2(pak);
		} else {
		    if (!idb->loopback) {
                        if (frame->control == SDLC_FRMR) {
			    /* Save the FRMR ifield into the sdb */
			    bcopy(p, sdb->sdlc_lastFailFRMRInfo, 3);
			    SdlcNmEvent(SDLC_RxFrmrInErrorState,
					idb, sdb);/*NmEvent Hook 15*/
                        } else 
			    SdlcNmEvent(SDLC_RxDmInErrorState,
					idb, sdb);/*NmEvent Hook 16*/
			sdlc_reset(idb, "SDLC DM or FRMR received", sdb);
		    }
		    pak->datagramsize = SDLCHEADERBYTES;
                    sdlc_setstate(idb, SDLC_DISCONNECT, sdb);
                    retbuffer2(pak);
		}
		break;
	    case SDLC_UA:
		/*
		 * Retransmit FRMR if UA seen.
		 */
		pak_lock(sdb->sdlc_frmrpak);
                /* 
		 * Note that no SNA Network Management Alert will be sent on
                 * retransmition of FRMRs since one FRMR alert is sufficient.
                 */
		sdlc_sendframe(SDLC_FRMR, FALSE, idb, sdb->sdlc_frmrpak, sdb);
		retbuffer2(pak);
		break;
	    default:
		/*
		 * Respond with original FRMR
		 */
		pak_lock(sdb->sdlc_frmrpak);
                /* Note that no SNA Network Management Alert will be sent on
                 * retransmition of FRMRs since one FRMR alert is sufficient.
                 */
		sdlc_sendframe(SDLC_FRMR, FALSE, idb, sdb->sdlc_frmrpak, sdb);
		retbuffer2(pak);

		break;
	    }
	} else if (SDLC_ISSFRAME(frame)) {
	    switch (frame->control) {
	    case SDLC_RR:
	    case SDLC_RNR:
	    case SDLC_REJ:
		/*
		 * Ignore unsolicited finals.
		 */
		if (!secondary && poll)
		    break;
		/* FALL THROUGH */
	    default:
                if ((ISPSDLC(idb)) || (poll)) {
		    pak_lock(sdb->sdlc_frmrpak);
                    /* Note that no SNA Network Management Alert will be sent on
                     * retransmition of FRMRs since one FRMR alert is sufficient.
                     */
		    sdlc_sendframe(SDLC_FRMR, (secondary &&
					       sdlc_validcmdframe(frame)) ?
				   poll : FALSE, idb, sdb->sdlc_frmrpak, sdb);
		}
		break;
	    }
	    retbuffer2(pak);
	} else {
	    /*
	     * Retransmit FRMR if input command with P.
	     */
            if ((ISPSDLC(idb)) || (poll)) {
		pak_lock(sdb->sdlc_frmrpak);
                /* Note that no SNA Network Management Alert will be sent on
                 * retransmition of FRMRs since one FRMR alert is sufficient.
                 */
		sdlc_sendframe(SDLC_FRMR, (secondary && sdlc_validcmdframe(frame)) ?
			       poll : FALSE, idb, sdb->sdlc_frmrpak, sdb);
	    }
	    retbuffer2(pak);
	}
	break;
    }
}

/*
 * sdlc_ack
 * Acknowledge some SDLC frames.
 *
 * THIS PROCEDURE SHOULD ONLY BE CALLED FROM INTERRUPT LEVEL
 * OR WITH INTERRUPTS DISABLED.
 */

boolean sdlc_ack (
    register hwidbtype *idb,
    register paktype *pak,
    uchar poll,
    uchar secondary,
    register int nr,
    sdlc_data_block *sdb)
{
    register int savet1;

#ifdef SDLC_VERBOSE_DEBUG
    if (SDLC_DEBUG(sdb, NULL))
	buginf("\n%s: SDLC nr %d rvr %d ret %d vr %d vs %d seq %#x",
	    idb->hw_namestring, nr,
	    sdb->sdlc_rvr, sdb->sdlc_retcnt, sdb->sdlc_vr, sdb->sdlc_vs,
	    sdb->sdlc_seqmap);
#endif
    if (nr != sdb->sdlc_vs && nr != sdb->sdlc_rvr &&
	!(sdb->sdlc_seqmap & (1 << ((nr - 1 + SDLC_MODULO) % SDLC_MODULO)))) {

        SdlcFrmrCondition(idb, pak, sdb, SDLC_FRMR_Z, poll, secondary);

	retbuffer2(pak);
	return FALSE;
    }
    /*
     * Disable T1 timer.
     * If this nr acknowledges some frames, reset retransmission
     * counter and free each acked frame.
     */
    savet1 = 0;
    if ((sdb->sdlc_state == SDLC_CONNECT) ||
        (sdb->sdlc_state == SDLC_USBUSY)) {
        savet1 = TIME_LEFT_SLEEPING(idb->sdlc_top_timer);
        sdb->sdlc_ready_for_poll = FALSE;
    }
    if (sdb->sdlc_rvr != nr)
	sdb->sdlc_retcnt = 0;

    /* 
     * Check if the same iframe(s) are going to be retransmitted.
     * i.e. no iframes acknowledged  nr == sdb->sdlc_rvr
     * &&   retransmit iframes since nr != sdb->sdlc_vs
     */
    if ((nr == sdb->sdlc_rvr) && (nr != sdb->sdlc_vs))
        sdb->sdlc_iframeRetCnt++;
    
    if (nr != sdb->sdlc_rvr)
        sdb->sdlc_iframeRetCnt = 0;  

    while (sdb->sdlc_rvr != nr) {
	if (sdb->sdlc_paks[sdb->sdlc_rvr]) {
#ifdef SDLC_VERBOSE_DEBUG
	    if (SDLC_DEBUG(sdb, NULL)) {
		buginf("\nSDLC6: Packet at %x refcount of %d before datagram_done",
		       sdb->sdlc_paks[sdb->sdlc_rvr],
		       sdb->sdlc_paks[sdb->sdlc_rvr]->refcount);
		buginf("\nSDLC: Finishing SDLC_PAKS[%d] (%x, %d)",
		       sdb->sdlc_rvr, sdb->sdlc_paks[sdb->sdlc_rvr],
		       sdb->sdlc_paks[sdb->sdlc_rvr]->refcount);
	    }
#endif
	    datagram_done(sdb->sdlc_paks[sdb->sdlc_rvr]);
	    sdb->sdlc_paks[sdb->sdlc_rvr] = NULL;
	    sdb->sdlc_seqmap &= ~(1 << sdb->sdlc_rvr);
            sdb->poll_seqmap &= ~(1 << sdb->sdlc_rvr);
	    sdb->sdlc_rvr = ++sdb->sdlc_rvr % SDLC_MODULO;
	} else if (SDLC_DEBUG(sdb, NULL))
	    buginf("\n%s: SDLC NULL saved packet ptr, rvr %d, nr %d, vs %d, vr %d",
		idb->hw_namestring,sdb->sdlc_rvr,nr,sdb->sdlc_vs,sdb->sdlc_vr);
    }

    /*
     * If some frames still unacknowledged, continue or restart T1.
     */
    if (ISPSDLC(idb)) {
        if (poll)
            sdlc_set_pause_timer_rfp(idb, sdb);
        else
            sdlc_settimer(idb, idb->sdlc_t1, sdb);
    } else {
	/* Secondary side */
        sdlc_settimer(idb, idb->sdlc_t1, sdb);
    }

    return TRUE;
}

/*
 * sdlcBLOCK
 * Scheduler test for the sdlc timer process
 * Returns FALSE if it is time to wake up SDLC to send data, or poll again.
 * Returns TRUE if the SDLC timer should remain blocked.
 * Kills the sdlc timer process if no interfaces have sdlc encaps.
 */

boolean sdlcBLOCK (void)
{
    register hwidbtype *idb;
    boolean sdlc_in_use;

    sdlc_in_use = FALSE;
    FOR_ALL_HWIDBS(idb) {
 	if (sdlcFragmentData)
	    return FALSE;        /* force system to retransmit fragment data */
        if (!is_sdlc(idb))
	    continue;
        sdlc_in_use = TRUE;

	if (idb->sdlc_simultaneous && idb->sdlc_output_flag)
	    return FALSE;

	if (TIMER_RUNNING(idb->sdlc_top_timer)) {
	    if ((!SLEEPING(idb->sdlc_top_timer)) &&
		(idb->sdlc_current_polled != 0))
		return FALSE;
	}
    }
    if (!sdlc_in_use) {
	process_kill(sdlc_tf);
	sdlc_tf = 0; 
    }
    return TRUE;
}


/* Macro to make the following code more legible */
#define SDLC_SEND_TIMER_RR_OR_RNR(place) \
    mod8_pak = getbuffer(2); \
    if (mod8_pak) { \
        mod8_pak->datagramsize = SDLCHEADERBYTES; \
        mod8_pak->datagramstart = mod8_pak->network_start - SDLCHEADERBYTES; \
        mod8_pak->enctype = ET_SDLCP; \
        mod8_pak->datagramstart[0] = sdb->sdlc_address; \
        mod8_pak->datagramstart[1] = (((sdb->sdlc_state == SDLC_CONNECT) || \
        			       (sdb->sdlc_state == SDLC_THEMBUSY)) ? \
				        SDLC_RR : SDLC_RNR); \
        mod8_pak->datagramstart[1] |= SDLC_PF | (sdb->sdlc_vr << 5); \
        if (SDLC_DEBUG(sdb, mod8_pak)) \
            buginf("\nSDLC: Sending RR/RNR at location %u, %s %0x", (place), \
                   idb->hw_namestring, sdb->sdlc_address); \
        sdlc_sendframe((((sdb->sdlc_state == SDLC_CONNECT) || \
		        (sdb->sdlc_state == SDLC_THEMBUSY)) ? \
		         SDLC_RR : SDLC_RNR), \
		         TRUE, idb, mod8_pak, sdb); \
    }


/*
 * sdlc_main_timer
 * Handle retransmission among various other protocol oddities
 */
static forktype sdlc_main_timer (void)
{
    register hwidbtype *idb;
    register paktype *pak, *pakc;
    paktype *mod8_pak, *test_pak;
    leveltype level = 0;
    int addr;
    int oldaddr = 0;
    sdlc_data_block *sdb, *oldsdb;
    boolean done, norm_activity, slow_polling, sdlc_test_print;
    int string_len;

    edisms(systeminitBLOCK, 0);
 
    for (;;) {
	edisms((blockproc *)sdlcBLOCK, 0L);
        if (sdlcFragmentData){
            sdlcFragmentData = FALSE;
            SdlcSendCheck();
            continue;
        }

	FOR_ALL_HWIDBS(idb) {
	    /* Not SDLC or not up? */
            if (!is_sdlc(idb))
		continue;

	    /* Timer not set for this IDB? */
	    if ((!(idb->sdlc_simultaneous && idb->sdlc_output_flag)) && 
		!TIMER_RUNNING_AND_AWAKE(idb->sdlc_top_timer))
		continue;

	    /* No connections currently in use on this link? */
	    if (idb->sdlc_current_polled == 0)
	      continue;

	    /*
	     *
             * We are the primary side!
	     *
	     */
            if (ISPSDLC(idb)) {
		oldaddr = idb->sdlc_current_polled;
		sdb = (sdlc_data_block *) idb->sdlc_data[oldaddr];
		done = FALSE;
                serial_bcount = 0;

		/* Print out what is happening */
		if (SDLC_DEBUG(sdb, NULL)) {
		    if (sdb) {
			if (sdb->sdlc_state != SDLC_DISCONNECT) {
                            buginf("\n%s: SDLC T %02x (%#Tn) %s %#Ta %c %d",
                                   idb->hw_namestring, idb->sdlc_current_polled,
                                   sdlc_prstate(sdb, 0), idb->sdlc_top_timer,
                                   (sdb->sdlc_ready_for_poll ? 'P' : 'T'), sdb->sdlc_retcnt);
			}
		    }
		}

		/* TWS mode, we have data for other station, start
		 * to send out I frame
		 */
		if (idb->sdlc_simultaneous && 
			idb->sdlc_output_flag &&
			!TIMER_RUNNING_AND_AWAKE(idb->sdlc_top_timer)) {
		    level = raise_interrupt_level(NETS_DISABLE);
		    tws_transmit(idb);
		    reset_interrupt_level(level);
		    idb->sdlc_output_flag = 0;
		    continue;
		}

		/*
                 * BLOCK A
                 *
		 * Does the current guy get to receive more output (all prev.
		 * acked and more to send...) or will we poll it for more input
		 * if it has given ("maxout" = k) frames?
		 */
		 /* Execute this code only if Two Way Simultaneous is not configured *
		  * AND more than one PU is configured on the interface.             */
		if ((!idb->sdlc_simultaneous) && 
		    (oldaddr != (idb->sdlc_data[oldaddr])->next)) {
		    if (sdb && (sdb->sdlc_ready_for_poll) &&
			(((!((1 << sdb->sdlc_rvr) & sdb->sdlc_seqmap)) &&
			  (!QUEUEEMPTY(&sdb->sdlc_holdq))) ||
			 (sdb->sdlc_iframes_since_poll >= idb->sdlc_k))) {
			if (sdb->sdlc_poll_count < idb->sdlc_poll_limit_value) {
			    level = raise_interrupt_level(NETS_DISABLE);
			    sdlc_give_pending_iframes(idb, sdb);
			    if (serial_bcount)
			      adjusted_t1 = (((serial_bcount * idb->sdlc_uc_delay)  >> 10))
                                + 10 + idb->sdlc_t1;
			    else
			      adjusted_t1 = idb->sdlc_t1;
			    if (!sdb->sdlc_poll) {
				SDLC_SEND_TIMER_RR_OR_RNR(1);
			    }
			    sdlc_settimer(idb, adjusted_t1, sdb);
			    serial_bcount = 0;
			    done = TRUE;
			    sdb->sdlc_poll_count++;
			    reset_interrupt_level(level);
			} else {
			    sdb->sdlc_poll_count = 0;
			}
		    }
	        }

		if (done)
		    continue;

		addr = oldaddr;
                norm_activity = slow_polling = sdlc_test_print = FALSE;
		do {
		    addr = ( idb->sdlc_data[addr])->next;
		    sdb =  idb->sdlc_data[addr];
		    if (sdb) {
			sdb->sdlc_poll = FALSE;  
			level = raise_interrupt_level(NETS_DISABLE);
			/* 
			 * If the SDLC Test command is running send a TEST frame
			 * out and start the timer.
			 */
			if (sdb->test_cnt) {
			    /*
			     * If the Test command has completed set the flag for
			     * printing the results further down after the interrupt
			     * is enabled.
			     */
			    if ((sdb->test_cnt != 0xFF) && 
				(sdb->sdlctest_sendcnt == sdb->test_cnt)) {
				sdlc_test_print = TRUE;
				sdb->test_cnt = 0;
			    }
			    else {
				string_len = strlen(sdb->test_string);
				test_pak = getbuffer(string_len + SDLCHEADERBYTES);

/*
 * BOGOSITY ALERT!!!  The below code mungs out test_pak without ever
 * having checked that getbuffer returned a valid packet...
 */

				test_pak->if_output = idb->firstsw;
				test_pak->datagramsize = string_len + SDLCHEADERBYTES;
				bcopy (sdb->test_string, 
				       test_pak->datagramstart + SDLCHEADERBYTES,
				       string_len);
				sdlc_sendframe(SDLC_TEST, TRUE, idb, test_pak, sdb);
				sdlc_settimer(idb, idb->sdlc_t1, sdb);
				norm_activity = TRUE;
			    }
			}
                        else if (sdb->sdlc_ready_for_poll) {
                            /*
                             * BLOCK B
                             *
                             * Time to send next poll.
                             */
                            switch(sdb->sdlc_state) {
                              case SDLC_DISCONNECT:
                                break;
                              case SDLC_DISCSENT:
                              case SDLC_ERROR:
                              case SDLC_SNRMSENT:
                              case SDLC_XIDSENT:
                              case SDLC_XIDSTOP:
                                /*
                                 * wrong state to have ready_for_poll set.  Bitch and
                                 * reset the timer but make sure that ready for poll is
                                 * off.  This makes sure that we run around the loop one
                                 * more time avoiding turning the timer off.  At worst this
                                 * wastes one poll_pause interval.
                                 */
                                if (sdllc_debug)
                                    buginf("\nSDLC: wrong state and ready for poll, %0x %s",
                                           sdb->sdlc_address, sdlc_prstate(sdb, 0));
                                sdlc_set_pause_timer(idb, sdb);
                                norm_activity = TRUE;
                                break;
                              case SDLC_CONNECT:
                              case SDLC_THEMBUSY:
                              case SDLC_USBUSY:
                              case SDLC_BOTHBUSY:
				if (idb->sdlc_simultaneous) {
				    (void)tws_send(idb, sdb, TRUE);
                                    if (serial_bcount)
                                        adjusted_t1 = (((serial_bcount * 
					  idb->sdlc_uc_delay) >> 10))+ 10 + idb->sdlc_t1;
                                    else
                                        adjusted_t1 = idb->sdlc_t1;
                                    serial_bcount = 0;
                                    sdlc_settimer(idb, adjusted_t1, sdb);
                                    (void)tws_transmit(idb);
                                    norm_activity = TRUE;
				    break;
				}
                                sdlc_give_pending_iframes(idb, sdb);
                                if (serial_bcount)
                                    adjusted_t1 = (((serial_bcount * idb->sdlc_uc_delay)  >> 10))                                        + 10 + idb->sdlc_t1;
                                else
                                    adjusted_t1 = idb->sdlc_t1;
                                serial_bcount = 0;
				if (!sdb->sdlc_poll) {
                                    SDLC_SEND_TIMER_RR_OR_RNR(4);
				}
                                sdlc_settimer(idb, adjusted_t1, sdb);
                                norm_activity = TRUE;
                                break;
                              default:
				errmsg(&msgsym(ILLEGSTATE, SDLC),
				       idb->hw_namestring,
				       sdb->sdlc_address, 
				       sdlc_prstate((sdb), 0),
				       sdb->sdlc_state);
                                break;
                            }
                        } else if (((sdb->sdlc_state == SDLC_SNRMSENT) ||
                                    (sdb->sdlc_state == SDLC_XIDSENT)) &&
                                   (sdb->sdlc_retcnt >= 2) &&
                                   idb->sdlc_slow_poll_interval) {
                            /*
                             * BLOCK C
                             *
                             * Slow Polling is enabled on this line, we are
                             * SNRMing or retrying XIDs, and we have already 
                             * tried 3 or more times.  This PU is slow polling.
                             */
                            if (idb->sdlc_slow_polled == 0) {
                                /*
                                 * Slow polling and no one owns the timer.  Grab it.
                                 */
                                idb->sdlc_slow_polled = addr;
                                TIMER_START(idb->sdlc_slow_poll_timer,
					    idb->sdlc_slow_poll_interval);
                                slow_polling = TRUE;
                            } else if (idb->sdlc_slow_polled == addr) {
                                /*
                                 * This PU is slow polling and it currently is waiting for
                                 * the slow poll timer to go off.  When it does we will send
                                 * another SNRM and release the timer.  Otherwise just wait
                                 * until it goes off.
                                 */
                                if (!SLEEPING(idb->sdlc_slow_poll_timer)) {
                                    sdlc_settimer(idb, idb->sdlc_t1, sdb);
                                    if (sdb->sdlc_state == SDLC_XIDSENT) {
					if (sdb->outbuf) {
					    pakc = pak_duplicate(sdb->outbuf);
					    if (!pakc)
						sdlc_setstate(idb, SDLC_XIDSTOP, sdb);
					    else
						sdlc_sendframe(SDLC_XID, TRUE, idb,
							       pakc, sdb);
					}
                                    } else {
                                        pak = getsdlc(idb, "SNRM slowpoll");
                                        if (pak)
                                            sdlc_sendframe(SDLC_SNRM, TRUE, idb,
                                                           pak, sdb);
                                    }

                                    sdb->sdlc_retcnt++;
                                    sdb->sdlc_lastReplyTO++;/* No reply to poll */
                                    norm_activity = TRUE;
                                    idb->sdlc_slow_polled = 0;
                                    TIMER_STOP(idb->sdlc_slow_poll_timer);
                                } else
                                    slow_polling = TRUE;
                            } else {
                                /*
                                 * This PU is slow polling but we are still waiting for
                                 * the slow poll timer to come to us.  Just wait.
                                 */
                                slow_polling = TRUE;
				if (sdlc_debug)
				  buginf("\nWaiting for slow-poll timer, addr = %x",addr);
                            }
                        } else if (sdb->sdlc_retcnt >= idb->sdlc_n2) {
                            /*
                             * BLOCK D
                             * Ran off the end of N2.  Nuke em.
                             *
                             * We may be slow polling when we exceeded the retry count.  Note that we
                             * don't need to check to see if this creature has the slow poll timer.
                             * When it launched its last SNRM/XID and retcnt got popped to n2 it went out
                             * of slow poll and released the timer.  T1 expired and we went around
                             * the loop again.  Now we are are looking at the n2 expiry.  Either
                             * someone else picked up the slow poll timer or it is still 0.
                             */
                            sdlc_reset(idb, "N2 (Retry Count) Expired", sdb);
                            norm_activity = TRUE;
                            SdlcNmEvent(SDLC_PollRetriesExhausted_NoResponse, 
                                        idb, sdb);/*NmEvent Hook 18*/
                            sdb->sdlc_retriesexps++; 
                        } else {
                            /*
                             * BLOCK E
                             * Normal first time or retry logic.
                             */
                            sdb->sdlc_retcnt++;
                            sdb->sdlc_lastReplyTO++;/*No reply to poll */
                            switch (sdb->sdlc_state) {
                              case SDLC_DISCONNECT:
                                /*
                                 * disconnect and not ready for poll.  set
                                 * ready for poll so we don't waste anymore time
                                 * here doing nothing.
                                 */
                                sdb->sdlc_ready_for_poll = TRUE;
                                break;
                              case SDLC_DISCSENT: /* resend DISC */
                                sdlc_settimer(idb, idb->sdlc_t1, sdb);
                                pak = getsdlc(idb, "DISC");
                                if (pak)
                                    sdlc_sendframe(SDLC_DISC, TRUE, idb, pak, sdb);
                                norm_activity = TRUE;
                                break;
                              case SDLC_ERROR:
                                sdlc_reset(idb, "FRMR timed out", sdb);
                                norm_activity= TRUE;
                                break;
                              case SDLC_XIDSENT: /* resend XID */
				if (sdb->outbuf) {
				    pakc = pak_duplicate(sdb->outbuf);
				    if (!pakc) {
					sdlc_setstate(idb, SDLC_XIDSTOP, sdb);
					datagram_done(sdb->outbuf);
					sdb->outbuf = NULL;
				    } else {
					sdlc_sendframe(SDLC_XID, TRUE,
						       idb, pakc, sdb);
					sdlc_settimer(idb, idb->sdlc_t1, sdb);
					norm_activity = TRUE;
                                    }
				}
                                break;
			      case SDLC_XIDSTOP:
                                sdlc_settimer(idb, idb->sdlc_t1, sdb);
                                break;

                              case SDLC_SNRMSENT: /* resend SNRM */
                                sdlc_settimer(idb, idb->sdlc_t1, sdb);
                                pak = getsdlc(idb, "SNRM");
                                if (pak)
                                    sdlc_sendframe(SDLC_SNRM, TRUE, idb, pak, sdb);
                                norm_activity = TRUE;
                                break;
                              case SDLC_CONNECT: /* send RR/P */
                              case SDLC_THEMBUSY:
                              case SDLC_USBUSY:
                              case SDLC_BOTHBUSY:
                                /*
                                 * If we have gotten here, then we have
                                 * sent an RR with *no* response
                                 */
                                SDLC_SEND_TIMER_RR_OR_RNR(2);
                                sdlc_settimer(idb, idb->sdlc_t1, sdb);
                                norm_activity = TRUE;
                                break;
			      default: 
                                break;
                            }

                        }
                        if (norm_activity) {
                            if (addr != idb->sdlc_current_polled) {
                                oldsdb = idb->sdlc_data[idb->sdlc_current_polled];
                                if (oldsdb) {
                                    oldsdb->sdlc_poll_count = 0;
                                    oldsdb->sdlc_iframes_since_poll = 0;
                                }
                            }
                            idb->sdlc_current_polled = addr;
                        }
                        SdlcRNRTimerLimitCheck(idb, sdb);
                        reset_interrupt_level(level);
			/*
			 * If the SDLC Test command has completed print the results
			 */
			if (sdlc_test_print) {
			    print_sdlc_test_results(sdb);
			    sdlc_test_print = FALSE;
			}
                    }
                } while ((addr != oldaddr) && (!norm_activity));

                if (!norm_activity) {
                    if (slow_polling)
                        COPY_TIMESTAMP(idb->sdlc_slow_poll_timer, idb->sdlc_top_timer);
                    else {
                        TIMER_STOP(idb->sdlc_top_timer);
                        idb->sdlc_current_polled = 0;
                    }
                }
            }
            else {
		/*
		 * We are the secondary side. 
		 */
		for (addr = SDLC_MIN_ADDRESS; addr <= SDLC_MAX_ADDRESS; addr++) {
		    sdb = idb->sdlc_data[addr];
		    if (sdb && sdb->sdlc_state > SDLC_SNRMSEEN) {
			if (AWAKE(sdb->sdlc_poll_starve_timer)) {
			    sdlc_reset(idb, "SDLC Secondary Poll Starvation", sdb);
			    SdlcNmEvent(SDLC_InactivityTimerExpired, 
					idb, sdb); /*NmEvent Hook 19*/
			    sdb->sdlc_activityTOs++;
			}
		    }
		    SdlcRNRTimerLimitCheck(idb, sdb);
		}
		TIMER_START(idb->sdlc_top_timer, ONESEC);
	    }
	}
    }
}


/*
 * sdlc_reset
 * Reset SDLC protocol machine
 */

void sdlc_reset (
    register hwidbtype *idb,
    char *why,
    sdlc_data_block *sdb)
{
   if (!sdb)
        return;
   if (stun_debug || SDLC_DEBUG(sdb, NULL) || sdllc_debug || sdlc_qllc_debug)
       buginf("\nSDLC Reset, %s %02x %s: %s", idb->hw_namestring,
	      sdb->sdlc_address, sdlc_prstate(sdb, 0), why);

   idb->reason = why;
   if (sdb->sdlc_state != SDLC_DISCONNECT)
       reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);

   sdlc_setstate(idb, SDLC_DISCONNECT, sdb);

   if (idb->enctype == ET_STUN) {
       /* Report SDLC Session loss to slack FSM */
       slack_burn_and_die(idb, sdb);
   }
   /*
    * The initilization code is hosed (sdlc_encaps) so we can't do the
    * timer setting in sdlc_setstate when we go disconnect like we should.
    * But if we are primary we restart the link via SDLC_SNRMSENT and
    * start the timers.  If so don't turn it off.
    */
   sdb->sdlc_poll = FALSE;  
   if (sdb->sdllc_llc)
       llc2_goto_adm(sdb->sdllc_llc, LC_INTDOWN);
   if (sdb->sdlc_state == SDLC_DISCONNECT)
       sdlc_set_pause_timer_rfp(idb, sdb);
}

/*
 * sdlc_getlink
 * Determine the protocol family of the packet
 */

long sdlc_getlink (paktype *pak)
{
    return(LINK_ILLEGAL);
}

extern void SetBanDlciMacAddr(hwidbtype *, uchar *);

void sdlc_config_partner (parseinfo *csb, hwidbtype *idb)
{
    uchar newmac[IEEEBYTES];
    sdlc_data_block *sdb;
    int addr;
    uchar null_addr[] = { 0x00,0x00,0x00,0x00,0x00,0x00 };
    
    if (!idb->sdllc_active && !ISCLSSDLC(idb)) {
 	printf("\nThis command is only valid for SDLLC or SDLC encapsulation");
 	printf("\nConfigure SDLC encapsulation or SDLLC TRADDR first");
 	return;
    }
    
    if (!idb->sdllc_active) {
	if (ieee_equal(idb->sdlc_vr_macaddr, null_addr)) {
	    printf("Please configure sdlc vmac address first");
	    return;
	}
    }
    ieee_copy(GETOBJ(hwaddr,1)->addr, newmac);
    if (newmac[0] & 0x80) {
 	printf("\nmalformed hex mac address");
 	return;
    }
    
    addr = GETOBJ(int, 1);
    sdb  = (sdlc_data_block *)idb->sdlc_data[addr];
    
    if (!sdb) {
	printf("\nConfigure sdlc before issuing this command");
	return;
    }
    
    if (!csb->sense) {
	ieee_zero(sdb->sdlc_prtnr);
	sdb->sdlc_have_prtnr = FALSE;
 	sdb->sdlc_inbound = FALSE;
	sdb->sdlc_outbound = FALSE;
    } else {
	ieee_copy(newmac, sdb->sdlc_prtnr);
	sdb->sdlc_have_prtnr = TRUE;
 	sdb->sdlc_inbound = GETOBJ(int,2);
 	sdb->sdlc_outbound = GETOBJ(int,3);
	if (sdb->sdlc_inbound && 
	    (IsSecPu2(sdb) || (idb->enctype == ET_SDLCS))) {
	    printf("\nsecondary station is inbound by default");
	    sdb->sdlc_inbound = FALSE;
	}
	if (sdb->sdlc_outbound &&
	    (IsPrimPu2(sdb) || (idb->enctype == ET_SDLCP))) {
	    printf("\nprimary station is outbound by default");
	    sdb->sdlc_outbound = FALSE;
	}
        if (csb->interface->hwptr->sdlc_default == FRAS_SDLC) {
          printf("\n\nWARNING: BAN-SDLC sessions will not come up unless the");
          printf("\n         fras ban command is removed first, then re-applied.");
        }
    } 
}

 
void sdlc_config_xid (parseinfo *csb, hwidbtype *idb)
 {
     uchar		addr;
     sdlc_data_block  	*sdb;
     char		*buffs;
 
     if (!idb->sdllc_active && !ISCLSSDLC(idb)) {
 	printf("\nThis command is only valid for SDLLC or SDLC encapsulation");
 	printf("\nConfigure SDLC encapsulation or SDLLC TRADDR first");
 	return;
     }
 
    addr = GETOBJ(int, 1);
    sdb  = (sdlc_data_block *) idb->sdlc_data[addr];
 
     if (!sdb) {
	printf("\nSDLC not configured for address %x. Command ignored.", addr);
	return;
    }
 
     if (!csb->sense) {
 	sdb->sdlc_xid_configured = FALSE;
 	return;
     }
 
     buffs = (char *) &GETOBJ(string,1)[0];
     if (!valid_XID(buffs, sdb->sdlc_xid, SDLC_XID_SIZE))
 	return;

     if (GETOBJ(int,2))
         sdb->sdlc_xid0_n2 = GETOBJ(int,2);
     else
         sdb->sdlc_xid0_n2 = SDLC_DEFAULT_XID0_N2;

     sdb->sdlc_xid_configured = TRUE;
 }
 
 
void sdlc_config_saps (parseinfo *csb, hwidbtype *idb)
{
    int addr;
    sdlc_data_block *sdb;
    
    if ((!idb->sdllc_active) && (!ISCLSSDLC(idb))) {
 	printf("\nThis command is only valid for SDLLC or SDLC encapsulation");
 	printf("\nUse SDLC encapsulation or a TRADDR command first");
 	return;
    }
    addr = GETOBJ(int, 1);
    sdb  = (sdlc_data_block *)idb->sdlc_data[addr];
    if (!sdb) {
        printf("\nConfigure sdlc before issuing this command");
        return;
    }
    if (!csb->sense) {
	sdb->sdlc_ssap = SAP_IBM_SNA;
	sdb->sdlc_dsap = SAP_IBM_SNA;
    } else {
	sdb->sdlc_ssap = GETOBJ(int, 2);
	sdb->sdlc_dsap = GETOBJ(int, 3);
    }
}
   
 
void sdlc_config_sdlclf (parseinfo *csb, hwidbtype *idb)
{
    sdlc_data_block *sdb;
    int addr;
    int largest;
    
    if ((!idb->sdllc_active) && (idb->enctype != ET_SDLC)){
 	printf("\nSDLLC not enabled for this interface. ");
 	printf("\nUse SDLC encapsulation or a TRADDR command first");
 	return;
    }
    
    addr = GETOBJ(int,1);
    sdb  = (sdlc_data_block *) idb->sdlc_data[addr];
    
    if (!sdb) {
 	printf("\nSDLC not enabled for this address on this interface. Command ignored");
 	return;
    }
    
    if (!csb->sense) {
        largest = SDLC_DEFAULT_SLF;
    } else {
        if (GETOBJ(int, 2) == 0)
	    largest = SDLC_DEFAULT_SLF;
        else
	    largest = GETOBJ(int, 2);
    }
    
    /* We don't fragment sdlc frames going over to the token ring side. */
    if (!ISCLSSDLC(idb) && largest > rc_code_to_bytes(idb->sdllc_largest_token_frame)) {
 	printf("\nMax token ring frame size cannot be less than the max SDLC frame size");
 	return;
    }
 
    sdb->sdlc_largest_frame = largest;
}



static int sdlc_stn_count (hwidbtype *hwidb)
{
    int i;
    int count = 0;
    
    for (i = SDLC_MIN_ADDRESS; i <= SDLC_MAX_ADDRESS; i++) {
	if (hwidb->sdlc_data[i])
	    count++;
    }
    
    return(count);
}

void sdlc_flow_ctrl_ind (sdlc_data_block *sdb)
{
    CepFlowControlInd(sdb->u_cep_id, &sdb->fCep);
}

/*
 * sdlc_encaps
 * Configure other SDLC encapsulation parameters
 */
void sdlc_encaps (parseinfo *csb)
{
    register hwidbtype *idb;
    int addr, i, temp_role;
    uchar addrprev;
    sdlc_data_block *sdb;
    leveltype level;
    sdlc_data_block *nsdb, *psdb;
    boolean test_mode, two_addr_mode, echo, ack_mode;
    uchar newmac[IEEEBYTES];

    idb = hwidb_or_null(csb->interface);
    if (csb->nvgen) {
	int i;
	boolean flag = TRUE;

	switch (csb->which) {
	  case SDLC_CFG_XID:
	  case SDLC_PRTNR:
	  case SDLC_SAPS:
	  case SDLC_SDLCLF:
	    break;
   	  case SDLC_VMAC:
	    if (ISCLSSDLC(idb)) {
		for (i = 0; i < IEEEBYTES; i++) 
		    if (idb->sdlc_vr_macaddr[i] != 0) 
			flag = FALSE;
		if (flag)
		    return;
		nv_write(TRUE, "%s %e", csb->nv_command, idb->sdlc_vr_macaddr);
	    }
	    break;

	  case SDLC_CMD_T1:
	    if (is_sdlc(idb) && ISPSDLC(idb)) {
		nv_write(idb->sdlc_t1 != SDLC_DEFAULT_T1,
			 "%s %d", csb->nv_command, idb->sdlc_t1);
	    }
	    break;
	  case SDLC_CMD_N1:
	    if (is_sdlc(idb)) {
		nv_write(idb->sdlc_n1 != SDLC_DEFAULT_N1(idb), "%s %d",
			 csb->nv_command, idb->sdlc_n1);
	    }
	    break;
	  case SDLC_CMD_N2:
	    if (is_sdlc(idb) && ISPSDLC(idb)) {
		nv_write(idb->sdlc_n2 != SDLC_DEFAULT_N2, "%s %d",
			 csb->nv_command, idb->sdlc_n2);
	    }
	    break;
	  case SDLC_CMD_K:
	    if (is_sdlc(idb)) {
		nv_write(idb->sdlc_k != SDLC_DEFAULT_K, "%s %d",
			 csb->nv_command,  idb->sdlc_k);
	    }
	    break;
	  case SDLC_CMD_HOLDQ:
	    for (i = SDLC_MIN_ADDRESS; i <= SDLC_MAX_ADDRESS; i++) {
		if ((sdb = idb->sdlc_data[i]) != NULL) {
		    sdb =  idb->sdlc_data[i];
		    nv_write(sdb->sdlc_holdq.maximum !=
			     SDLC_DEFAULT_HOLDQ, "%s %02x %d", csb->nv_command, i,
			     sdb->sdlc_holdq.maximum);
		}
	    }
	    break;
	  case SDLC_CMD_FRMR_DISABLE:
	    if (is_sdlc(idb)) {
		nv_write((idb->sdlc_flags & SDLC_FLAG_NOFRMR),
			 csb->nv_command);
	    }
	    break;
	  case SDLC_CMD_ADDRESS:
	    if (is_sdlc(idb)) {
		for (i = SDLC_MIN_ADDRESS; i <= SDLC_MAX_ADDRESS; i++) {
		    if ((sdb =  (idb->sdlc_data[i])) != NULL) {
			sdb =  idb->sdlc_data[i];
			nv_write(TRUE, "%s %02x", csb->nv_command, i);
			nv_add(sdb->sdlc_testmode, " test-mode");
			nv_add(sdb->sdlc_twoaddrmode, " two-addr-mode");
			nv_add(sdb->stun_addr_echo, " echo");
                        nv_add(sdb->sdlc_ackmode, " ack-mode");
                        nv_add(sdb->sdlc_xid_poll, " xid-poll");
			nv_add(sdb->sdlc_switched, " switched");
                        nv_add(sdb->sdlc_xid_passthru, " xid-passthru");
			if ((idb->sdllc_active) || ISCLSSDLC(idb)){
			    /* Have to put in command name because
			       nv_command is still "address" */
			       
			    nv_write((sdb->sdlc_largest_frame !=
				      SDLC_DEFAULT_SLF),
				     "%s sdlc-largest-frame %02x %u", 
				     ISCLSSDLC(idb) ? "sdlc":"sdllc",
				     i,
				     sdb->sdlc_largest_frame);
			    nv_write(sdb->sdlc_xid_configured,
				     "%s xid %02x %02x%02x%02x%02x",
				     ISCLSSDLC(idb) ? "sdlc":"sdllc",
				     i,
				     sdb->sdlc_xid[0],
				     sdb->sdlc_xid[1],
				     sdb->sdlc_xid[2],
				     sdb->sdlc_xid[3]);
			    nv_add(sdb->sdlc_xid_configured && 
                                   (sdb->sdlc_xid0_n2 != SDLC_DEFAULT_XID0_N2),
				     " xid0-n2 %d", sdb->sdlc_xid0_n2);
			    nv_write(sdb->sdlc_have_prtnr,
				     "%s partner %e %02x%s%s", 
				     ISCLSSDLC(idb) ? "sdlc":"sdllc",
				     sdb->sdlc_prtnr, i,
				     (sdb->sdlc_inbound ? " inbound" : ""),
				     (sdb->sdlc_outbound ? " outbound" : ""));
			    nv_write(((sdb->sdlc_ssap != SAP_IBM_SNA) ||
				      (sdb->sdlc_dsap != SAP_IBM_SNA)),
				     "%s saps %02x %02x %02x", 
				     ISCLSSDLC(idb) ? "sdlc":"sdllc",
				     i, 
				     sdb->sdlc_ssap, sdb->sdlc_dsap);
			}
		    }
		}
	    }
	    break;
	  case SDLC_CMD_POLL_PAUSE:
	    if (is_sdlc(idb) && ISPSDLC(idb)) {
		nv_write(idb->sdlc_poll_pause_timer !=
			 SDLC_DEFAULT_POLL_PAUSE_TIMER,
			 "%s %d", csb->nv_command,
			 idb->sdlc_poll_pause_timer);
	    }
	    break;
          case SDLC_CMD_RNR_LIMIT:
	    if (is_sdlc(idb)) {
		nv_write(idb->sdlc_rnrLimit !=
			 SDLC_DEFAULT_RNRLIMIT,
			 "%s %d", csb->nv_command,
			 idb->sdlc_rnrLimit);
	    }
            break;
	  case SDLC_CMD_POLL_LIMIT:
            if (is_sdlc(idb) && ISPSDLC(idb)) {
		nv_write(idb->sdlc_poll_limit_value !=
			 SDLC_DEFAULT_POLL_LIMIT_VALUE,
			 "%s %d", csb->nv_command,
			 idb->sdlc_poll_limit_value);
	    }
	    break;
	  case SDLC_CMD_POLL_WAIT:
	    if (is_sdlc(idb) && ISSSDLC(idb)) {
		nv_write(idb->sdlc_poll_wait !=
			 SDLC_DEFAULT_POLL_WAIT,
			 "%s %d", csb->nv_command,
			 idb->sdlc_poll_wait);
	    }
	    break;
	  case SDLC_CMD_SLOW_POLL:
	    if (is_sdlc(idb)) {
               if (idb->sdlc_slow_poll_interval == 0)
                  nv_write(TRUE, "no %s", csb->nv_command);
               else if (idb->sdlc_slow_poll_interval != SDLC_DEFAULT_SLOW_POLL)
                  nv_write(TRUE, "%s %d", csb->nv_command, idb->sdlc_slow_poll_interval/ONESEC);
	    }
	    break;
	  case SDLC_CMD_LINE_SPEED:
            if (is_sdlc(idb) && ISPSDLC(idb)) {
		nv_write(idb->sdlc_line_speed != 0,
			 "%s %d", csb->nv_command,
			 idb->sdlc_line_speed);
	    }
	    break;
	  case SDLC_CMD_HDX:
	  case SDLC_CMD_CTS_DELAY:
	  case SDLC_CMD_RTS_TIMEOUT:
	    break;
	  case SDLC_CMD_QLLC_PRTNR:
	    if (is_sdlc(idb) && idb->snx_active) {
		for (addr = SDLC_MIN_ADDRESS; addr <= SDLC_MAX_ADDRESS; 
		     addr++) {
		    sdb =  idb->sdlc_data[addr];
		    nv_write(sdb && sdb->qllc_have_prtnr, "%s %e %02x",
		             csb->nv_command, sdb->qllc_prtnr, addr);
		}			/* for all SDLC addresses */
	    }
	    break;
	  case SDLC_CMD_SIMULTANEOUS:
	    if (idb->sdlc_simultaneous) {
	        if (idb->sdlc_flags & SDLC_FLAG_TWS_FULL)
		    nv_write(TRUE, "%s full-datmode", csb->nv_command);
		else
		    nv_write(TRUE, "%s half-datmode", csb->nv_command);
	    }
	    break;
          case SDLC_CMD_VIRTUAL_MULTIDROP:
            if (ISSSDLC(idb)) 
                nv_write((idb->sdlc_flags & SDLC_FLAG_VIRTUAL_MULTIDROP), "%s",
                         csb->nv_command);
            break;

	  case SDLC_CMD_ROLE:
	    if (ISCLSSDLC(idb))
		if (idb->configured_role == SDLC_PRIMARY)
		    nv_write(TRUE, "%s primary", csb->nv_command);
		else if (idb->configured_role == SDLC_PRIM_XID_POLL)
		    nv_write(TRUE, "%s prim-xid-poll", csb->nv_command);
		else if (idb->configured_role == SDLC_SECONDARY)
		    nv_write(TRUE, "%s secondary", csb->nv_command);
	    break;

	  case SDLC_DLSW: {
	      boolean flag = TRUE;

              nv_write(idb->sdlc_have_default_partner, "%s partner %e %s%s",
                       csb->nv_command,
                       idb->sdlc_default_partner,
                       (idb->sdlc_default_inbound ? " inbound" : ""),
                       (idb->sdlc_default_outbound ? " outbound" : ""));
              if (idb->sdlc_default == DLSW_SDLC)
		   nv_write(TRUE, "%s default", csb->nv_command);
	      else
                for (i = 1; i<=SDLC_MAX_ADDRESS; i++)
                     if (idb->sdlc_data[i] &&
                         idb->sdlc_data[i]->sdlc_dlsw) {
                         nv_write(flag, "%s ", csb->nv_command);
                         flag = FALSE;
                         nv_add(TRUE, "%x ", i);
		     }
          }
	    break;
			
          case SDLC_CMD_GROUP_POLL:
            if (ISSSDLC(idb))
                nv_write((idb->sdlc_group_addr), "%s %02x",
                         csb->nv_command, idb->sdlc_group_addr);
            break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    if (!is_sdlc(idb)) {
        printf("\nThis interface has not been configured for SDLC.");
        return;
    }

    GET_TIMESTAMP(idb->sdlc_lastModifyTime);

    switch (csb->which) {
      case SDLC_VMAC:
	if (!ISCLSSDLC(idb)) {
	    printf("%%SDLC encapsulation is not configured on this interface.");
	    return;
	}
	ieee_copy((char *)GETOBJ(hwaddr,1)->addr, idb->sdlc_vr_macaddr);
	if (idb->sdlc_vr_macaddr[IEEEBYTES-1] != 0) {
	    printf("\n%%Last byte of the address should be 0x00.");
	    return;
	}
	break;

      case SDLC_PRTNR:
	if (!ISCLSSDLC(idb)) {
	    printf("%%SDLC encapsulation is not configured on this interface.");
	    return;
	}
	sdlc_config_partner(csb, idb);
	break;
      case SDLC_CFG_XID:
	if (!ISCLSSDLC(idb)) {
	    printf("%%SDLC encapsulation is not configured on this interface.");
	    return;
	}
	if (!ISCLSSDLC(idb)) {
	    printf("%%SDLC encapsulation is not configured on this interface.");
	    return;
	}
	sdlc_config_xid(csb, idb);
	break;
      case SDLC_SAPS:
	if (!ISCLSSDLC(idb)) {
	    printf("%%SDLC encapsulation is not configured on this interface.");
	    return;
	}
	sdlc_config_saps(csb, idb);
	break;
      case SDLC_SDLCLF:
	if (!ISCLSSDLC(idb)) {
	    printf("%%SDLC encapsulation is not configured on this interface.");
	    return;
	}
	sdlc_config_sdlclf(csb, idb);
	break;
      case SDLC_CMD_T1:
	if (ISSSDLC(idb)) {
	    printf("\nSDLC: T1 value applicable only to SDLC primary side");
	    return;
	}
	if (csb->sense) {
	    idb->sdlc_t1 = GETOBJ(int,1);
            if (idb->sdlc_t1 < ONESEC/10) {
                printf("\nSDLC: T1 value is less than 0.1 seconds, using default.");
		idb->sdlc_t1 = SDLC_DEFAULT_T1;
	    }
	} else {
	    idb->sdlc_t1 = SDLC_DEFAULT_T1;
	}
	break;

      case SDLC_CMD_N1:
	if (csb->sense) {
	    if (GETOBJ(int,1) > SDLC_DEFAULT_N1(idb)) {
                printf("\nN1 %d larger than max bit count (mtu * 8) %d",
                       GETOBJ(int,1), SDLC_DEFAULT_N1(idb));
	    }else {
		idb->sdlc_n1 = GETOBJ(int,1);
	    }
	}else {
	    idb->sdlc_n1 = SDLC_DEFAULT_N1(idb);
	}
	break;
      case SDLC_CMD_N2:
	if (ISSSDLC(idb)) {
	    printf("\nSDLC: N2 value applicable only to SDLC primary side");
	    return;
	}
	if (csb->sense) {
	    idb->sdlc_n2 = GETOBJ(int,1);
	} else {
	    idb->sdlc_n2 = SDLC_DEFAULT_N2;
	}
	break;
      case SDLC_CMD_K:
	if (csb->sense) {
	    idb->sdlc_k = GETOBJ(int,1);
	} else {
	    idb->sdlc_k = SDLC_DEFAULT_K;
	}
	break;
      case SDLC_CMD_HOLDQ:
	sdb =  idb->sdlc_data[GETOBJ(int,1)];
	if (!sdb) {
	    printf("\nSDLC is not enabled for this address. Command ignored.");
	} else if (csb->sense) {
	    sdb->sdlc_holdq.maximum = GETOBJ(int,2);
	} else 
	    sdb->sdlc_holdq.maximum = SDLC_DEFAULT_HOLDQ;

	CepUpdate(&sdb->fCep, NULL, sdb->sdlc_holdq.maximum,
		  0, 0, 0);
	break;
      case SDLC_CMD_FRMR_DISABLE:
	if (csb->sense) {
	    idb->sdlc_flags |= SDLC_FLAG_NOFRMR;
	} else {
	    idb->sdlc_flags &= ~SDLC_FLAG_NOFRMR;
	}
	break;
      case SDLC_CMD_ADDRESS:
	addr = GETOBJ(int,1);
	sdb =  idb->sdlc_data[GETOBJ(int,1)];
	test_mode = two_addr_mode = echo = ack_mode = FALSE;
	level = raise_interrupt_level(ALL_DISABLE);
	if (csb->sense) {
	    if (GETOBJ(int,4)) {
		test_mode = TRUE;
	    }
	    if (GETOBJ(int,5)) {
		two_addr_mode = TRUE;
	    }
	    if (GETOBJ(int,6)) {
		echo = TRUE;
	    }
            if (GETOBJ(int,7)) {
                if (addr != 0xFF) {
                    reset_interrupt_level(level);
                    printf("\nSDLC: ack-mode is only valid for SDLC address FF");
                    return;
                }
                if (idb->sdlc_flags & SDLC_FLAG_VIRTUAL_MULTIDROP) {
                    reset_interrupt_level(level);
                    printf("\nSDLC: ack-mode is not valid with virtual-multidrop.");
                    return;
                }
                ack_mode = TRUE;
            }
		
	    if (sdb) {
		/* Make sure parameters are consistent */
		if (test_mode != sdb->sdlc_testmode) {
		    reset_interrupt_level(level);
		    printf("\nSDLC: You cannot change test mode on the fly.");
		    return;
		}
		if (two_addr_mode != sdb->sdlc_twoaddrmode) {
		    reset_interrupt_level(level);
		    printf("\nSDLC: You cannot change two-addr-mode on the fly");
		    return;
		}
                if ((addr == 0xFF) && (ack_mode != sdb->sdlc_ackmode)) {
                    reset_interrupt_level(level);
                    printf("\nSDLC: Address FF is configured for local acknowledgement.");
                    return;
                }


                if (sdb->sdlc_xid_poll != GETOBJ(int,8) ||
		    sdb->sdlc_switched != GETOBJ(int,10) ||
		    sdb->sdlc_xid_passthru != GETOBJ(int,9)) {
                    if (idb->state == IDBS_UP) {
                        reset_interrupt_level(level);
                        printf("\ncannot change xid option when the interface is up.");
                        printf("\nadministratively shut down this interface first");
                        return;
                    } else {
                        sdb->sdlc_xid_poll = GETOBJ(int,8);
			sdb->sdlc_xid_passthru = GETOBJ(int,9);
			sdb->sdlc_switched = GETOBJ(int,10);

			/* Are we configuring switched mode? 
			 * Then we required sdlc  role primary,
			 * or sdlc role prim-xid-poll.
			 */

			/* If switched  is  being specified, validate co-requisites */
			if (GETOBJ(int,10)) {    
			    if ( (idb->configured_role == SDLC_PRIMARY) ||
				(idb->configured_role == SDLC_PRIM_XID_POLL) ) {
				sdb->sdlc_switched = TRUE;

			    /* 
			     * Update our switched_startup timer with the timer in use
			     * by sdlc_main_timer
			     */
				if (idb->sdlc_slow_poll_interval) {
				    COPY_TIMESTAMP(idb->sdlc_slow_poll_timer,
						   sdb->sdlc_switched_startup_timer);
				} else {
				    TIMER_START(sdb->sdlc_switched_startup_timer,
						idb->sdlc_t1);
				}
			    } else {
				reset_interrupt_level(level);
				printf("\nswitched mode requires sdlc role primary or prim-xid-poll");
				return;
			    }
			}
				
                    }
                }
	    } else {		/* !sdb */
                if ((addr == 0xFF) && (!ack_mode)) {
                    reset_interrupt_level(level);
                    printf("\nSDLC: illegal SDLC address - FF");
                    return;
                }
		
                if ((GETOBJ(int,8)) && 
		    ((idb->configured_role != SDLC_PRIMARY) &&
		     (idb->configured_role != SDLC_SECONDARY))) {
                    reset_interrupt_level(level);
                    printf("\nxid-poll option is only valid for sdlc role primary or secondary");
                    return;
                }

                if ((GETOBJ(int,9)) && 
		    ((idb->configured_role != SDLC_PRIMARY) &&
		     (idb->configured_role != SDLC_SECONDARY))) {
                    reset_interrupt_level(level);
                    printf("\nxid-passthru option is only valid for sdlc role primary or secondary");
                    return;
                }

		if ((GETOBJ(int,10) &&  /* if switched mode */
		     ( !(idb->configured_role == SDLC_PRIM_XID_POLL) &&
		     (  !(idb->configured_role == SDLC_PRIMARY))))) {
		    reset_interrupt_level(level);
		    printf("\nswitched mode requires sdlc role primary or prom-xid-poll.");
		    return;
		}

                if (ISCLSSDLC(idb) && (idb->configured_role == SDLC_NONE)) {
                    for (i = SDLC_MIN_ADDRESS; i <= SDLC_MAX_ADDRESS; i++) {
                        if (idb->sdlc_data[i])
                            if (i != addr) {
                                reset_interrupt_level(level);
                                printf("\nYou cannot configure more than one address if role is none.");
                                return;
                            }
                    }
                }

		if ((sdb = (malloc_named(sizeof(sdlc_data_block),
			    sdlc_cb))) == NULL) {
		    reset_interrupt_level(level);
		    printf(nomemory);
		    return;
		}

                /* Ensure statistic counters are initialised to zero */
                memset(sdb, 0,sizeof(sdlc_data_block)); 

		if ((sdb->sdlc_buffer =
		     malloc_named(120 * sizeof(char), sdlc_cb)) == NULL) {
		    reset_interrupt_level(level);
		    free(sdb);
		    printf(nomemory);
		    return;
		}
		idb->sdlc_data[addr] = sdb;
		sdb->sdlc_idb = idb;
		sdb->sdlc_address = addr;
                sdb->sdlc_xid_poll = GETOBJ(int,8);
                sdb->sdlc_xid_passthru = GETOBJ(int,9);
		sdb->sdlc_switched = GETOBJ(int,10);
		sdb->u_sap_id = 0;

		sdb->u_cep_id = (void*) CLS_NO_CEP;

		if (!sdb->sdlc_paks) {
		    sdb->sdlc_paks = 
			malloc_named(SDLC_MODULO * sizeof(paktype *), sdlc_cb);
		    if (sdb->sdlc_paks == NULL){
                        reset_interrupt_level(level);
			printf(nomemory);
                        level = raise_interrupt_level(ALL_DISABLE);
		    }
		} else
		    sdlc_reset(idb, "SDLC initialized", sdb);

 		sdb->sdlc_address = addr;
		sdb->sdlc_largest_frame = SDLC_DEFAULT_SLF;
		sdb->sdlc_ready_for_poll = FALSE;
                sdb->stun_peer_busy = FALSE;
                sdb->stun_local_busy = FALSE;
                sdb->stun_init_busy = FALSE;
                sdb->stun_addr_echo = echo;
                sdb->sdlc_ackmode = ack_mode;
		queue_init(&sdb->sdlc_holdq, SDLC_DEFAULT_HOLDQ);
		CepInit(&sdb->fCep, &sdb->sdlc_holdq, SDLC_DEFAULT_HOLDQ,
			SDLC_TXQ_LOWATER, SDLC_TXQ_HIWATER, 
			SDLC_CLS_FLOW_IND_DAMP);
		sdb->sdlc_testmode = test_mode;
		sdb->sdlc_twoaddrmode = two_addr_mode;
		sdb->sdlc_ssap = SAP_IBM_SNA;
		sdb->sdlc_dsap = SAP_IBM_SNA;
                sdb->lastFailCause = UNDEFINED;  
                TIMER_STOP(sdb->sdlc_lastFailTime);	
                TIMER_STOP(idb->sdlc_lastModifyTime);	
                sdb->sdlc_operName[0] = '\0';
                TIMER_STOP(sdb->sdlc_rnrLimitTimer);   
                sdb->sdlc_lastCtrlIn = SDLC_NOFRAME;  
                sdb->sdlc_lastCtrlOut = SDLC_NOFRAME; 

		/*
		 * Insert this item in the SDLC "poll" list.  
		 * This loop depends on uchar subtraction 1, 0, 255 ...
		 * Find the previous entry in the array.  Works for
		 * only ourself in the array, as well.
		 */
		sdb->next = sdb->prev = sdb->sdlc_address;

		for (addrprev = (addr - 1); 
		     !(idb->sdlc_data[addrprev]);
		     addrprev--); 

		psdb = idb->sdlc_data[addrprev];
		nsdb = idb->sdlc_data[psdb->next];
		psdb->next = sdb->sdlc_address;
		sdb->prev = psdb->sdlc_address;
		nsdb->prev = sdb->sdlc_address;
		sdb->next = nsdb->sdlc_address;

		/*
		 * If not primary, just set "current polled" to *us*
		 */
		if ((idb->sdlc_current_polled == 0) && (!ISPSDLC(idb)))
		    idb->sdlc_current_polled = addr;

                /*
                 * The configuration parsing runs prior to system_init during which the
                 * serial idb is up.  When CMD_ADDRESS executes it calles sdlc_setstate to
                 * set the interface to DISCONNECT which invokes autostart which sets the
                 * PU to SNRMSENT.  When system_init completes, sdlc_main_timer kicks in
                 * immediately and fires up a SNRM transmission.  This hits the serial card
                 * being used and the SNRM is sent.
                 */
                sdlc_setstate(idb, SDLC_DISCONNECT, sdb);
                if (sdb->sdlc_state == SDLC_DISCONNECT)
                    sdlc_set_pause_timer_rfp(idb, sdb);

                /*
                 * Set up sole interface timer if secondary.
                 */
                if (ISSSDLC(idb)) {
                    TIMER_START(sdb->sdlc_poll_starve_timer, idb->sdlc_poll_wait);
                    TIMER_START(idb->sdlc_top_timer, ONESEC);
                }
                /*
                 * Start timer fork
                 */
                if (sdlc_tf == 0) {
                    reset_interrupt_level(level);
                    sdlc_tf = cfork(sdlc_main_timer, 0L, 1500, "SDLC Timer", 0);
                    level = raise_interrupt_level(ALL_DISABLE);
                }
	    }
	} else {			/* no sdlc addresss <addr> */
	    if (sdb) {
		lak_t *lackie;

		if (sdb->sdllc_lak_enable) {
		    lackie = sdb2lak(idb, sdb);
		    if (lackie)
			lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_failure);
		}    

		/* 
		 * sdllc->llc2 is cleaned up in sdlc_reset
		 */
		sdlc_reset(idb, "User shut down SDLC address", sdb);
		idb->sdlc_data[sdb->prev]->next = sdb->next;
		idb->sdlc_data[sdb->next]->prev = sdb->prev;
		/*
		 * Handle special corner case -- deleting currently polled
		 * item? If so, change item to poll to be next one, and
		 * delete altogether if next is us.
		 */
                addr = GETOBJ(int,1);
                if (idb->sdlc_slow_polled == addr)
                    idb->sdlc_slow_polled = 0;
		if (idb->sdlc_current_polled == addr) {
		    if (idb->sdlc_current_polled == sdb->next) {
			idb->sdlc_current_polled = 0;
		    } else {
			idb->sdlc_current_polled = sdb->next;
		    }
		}
		if (idb->sdlc_grp_start_poll == addr) {
		    if (idb->sdlc_grp_start_poll == sdb->next) {
			idb->sdlc_grp_start_poll = 0;
		    } else {
			idb->sdlc_grp_start_poll = sdb->next;
		    }
		}
		idb->sdlc_data[addr] = NULL;
		if (ISCLSSDLC(idb)) {
		    SdlcDeleteClsSdb(sdb);
		} else {
		    free(sdb);
		}
	    }
	}
	reset_interrupt_level(level);
	break;
      case SDLC_CMD_POLL_PAUSE:
	if (ISSSDLC(idb)) {
	    printf("\nSDLC: Poll pause value applicable only to SDLC primary side");
	    return;
	}
	if (csb->sense) {
	    idb->sdlc_poll_pause_timer = GETOBJ(int,1);
	} else {
	    idb->sdlc_poll_pause_timer = SDLC_DEFAULT_POLL_PAUSE_TIMER;
	}
	break;
      case SDLC_CMD_RNR_LIMIT:
	if (csb->sense) 
            idb->sdlc_rnrLimit = GETOBJ(int,1);  
	else 
	    idb->sdlc_rnrLimit = SDLC_DEFAULT_RNRLIMIT;
	
	break;
      case SDLC_CMD_POLL_LIMIT:
	if (ISSSDLC(idb)) {
	    printf("\nSDLC: Poll limit value applicable only to SDLC primary side");
	    return;
	}
	if (csb->sense) {
	    idb->sdlc_poll_limit_value = GETOBJ(int,1);
	} else {
	    idb->sdlc_poll_limit_value = SDLC_DEFAULT_POLL_LIMIT_VALUE;
	}
	break;
      case SDLC_CMD_POLL_WAIT:
	if (ISPSDLC(idb)) {
	    printf("\nSDLC: Poll wait value applicable only to SDLC secondary side");
	    return;
	}
	if (csb->sense) {
	    idb->sdlc_poll_wait = GETOBJ(int,1);
	} else {
	    idb->sdlc_poll_wait = SDLC_DEFAULT_POLL_WAIT;
	}
        break;
      case SDLC_CMD_SLOW_POLL:
        if (ISSSDLC(idb)) {
            printf("\nSDLC: \"slow-poll\" is only applicable on primary SDLC interfaces");
            return;
        }
	if (csb->set_to_default)
	    idb->sdlc_slow_poll_interval = SDLC_DEFAULT_SLOW_POLL;
	else if (csb->sense) {
	    idb->sdlc_slow_poll_interval = GETOBJ(int,1) * ONESEC;
	} else {
            idb->sdlc_slow_poll_interval = 0;
	}
        idb->sdlc_slow_polled = 0;
        GET_TIMESTAMP(idb->sdlc_slow_poll_timer);
	break;
      case SDLC_CMD_LINE_SPEED:
        if (csb->sense) {
            if (ISSSDLC(idb)) {
	       printf("\nSDLC: Line speed applicable only to SDLC primary side");
	       return;
            }
            idb->sdlc_line_speed = GETOBJ(int,1);
           /*
            * calculate the delay in (milliseconds * 1024)
            * this is so the division to calcuate sdlc_uc_delay
            * will not result in 0.  1024 will allow divide
            * by shifting operations.
            */
            idb->sdlc_uc_delay = ((1024000 / idb->sdlc_line_speed) * 8);
        }
        else {
            idb->sdlc_line_speed = 0;
            idb->sdlc_uc_delay = 0;
	}
        break;

      case SDLC_CMD_HDX:
        printf("\nConverting 'sdlc hdx' to 'half-duplex'");
        csb->which = CFG_HDX_SET_HALF_DUPLEX;
        serial_half_duplex_translate_old_cmds(csb);
    	break;
  
      case SDLC_CMD_CTS_DELAY:
        printf("\nConverting 'sdlc cts-delay' to 'half-duplex timer cts-delay'");
        csb->which = CFG_HDX_CTS_DELAY;
        serial_half_duplex_translate_old_cmds(csb);
  	break;

      case SDLC_CMD_RTS_TIMEOUT:
        printf("\nConverting 'sdlc rts-timeout' to 'half-duplex timer rts-timeout'");
        csb->which = CFG_HDX_RTS_TIMEOUT;
        serial_half_duplex_translate_old_cmds(csb);
  	break;

      case SDLC_CMD_QLLC_PRTNR:
	
      /* Check for reasonable mac address.  Don't allow multicasts. */
	ieee_copy((char *)GETOBJ(hwaddr,1)->addr, newmac);
	if (newmac[0] & 0x80) {
	    printf("\nMalformed hex mac address - group bit set");
	    return;
	}
 
        addr = GETOBJ(int, 1);
 
	sdb  = (sdlc_data_block *)idb->sdlc_data[addr];
 
	if (!sdb) {
	    printf("\nConfigure sdlc before issuing this command");
	    return;
	}
 
	if (!csb->sense) {
	    ieee_zero(sdb->qllc_prtnr);
	    sdb->qllc_have_prtnr = FALSE;
	}
	else {
	    reg_invoke_snx_add_lnxtype(newmac);

	    ieee_copy(newmac, sdb->qllc_prtnr);
	    sdb->qllc_have_prtnr = TRUE;
	    idb->snx_active = TRUE;
	    reg_invoke_snx_start();  /* start background qllc work */
	}
 
      break;

      case SDLC_CMD_SIMULTANEOUS :
        if (ISSSDLC(idb)) {
	    printf("\nSDLC: Simultaneous Mode applicable only to SDLC primary side");
	    return;
	}
	idb->sdlc_flags &= ~SDLC_FLAG_TWS_FULL;
        idb->sdlc_simultaneous = csb->sense;
	if (GETOBJ(int, 1))
	    idb->sdlc_flags |= SDLC_FLAG_TWS_FULL;
	else
	    idb->sdlc_flags &= ~SDLC_FLAG_TWS_FULL;
        break;

      case SDLC_CMD_VIRTUAL_MULTIDROP:
        if (!ISSSDLC(idb)) {
            printf("\nSDLC: Broadcast propagation is only applicable to SDLC secondary");
            return;
	}
        if (!csb->sense)
            idb->sdlc_flags &= ~SDLC_FLAG_VIRTUAL_MULTIDROP;
        else
            idb->sdlc_flags |= SDLC_FLAG_VIRTUAL_MULTIDROP;
        break;

    case SDLC_CMD_ROLE: 
	/* Set configured role for this port.
	 * If configured role is SDLC_NONE, running role will 
	 * be different.
	 */
	if (!ISCLSSDLC(idb)) {
	    printf("%%SDLC encapsulation is not configured on this interface.");
	    return;
	}

	if (!csb->sense)
	    temp_role = SDLC_NONE;
	else {
	    temp_role = GETOBJ(int, 1);
	}

        if (temp_role == SDLC_NONE) {
	    if (sdlc_stn_count(idb) > 1) {
		printf("\n%%This interface can have only one sdlc address ");
		printf("\nconfigured if the role is none.");
		return;
	    }
	}

	idb->sdlc_current_polled = 0;
	idb->configured_role = temp_role;
	idb->sdlc_role = idb->configured_role;
	if (idb->configured_role == SDLC_PRIM_XID_POLL)
	    idb->sdlc_role = SDLC_PRIMARY;
	break;

    case SDLC_DLSW: {
        int i, j;
        uchar newmac[IEEEBYTES];
        boolean addr_to_be_detached, sdlc_dlsw_addr_conf;

        if (!ISCLSSDLC(idb)) {
            printf("%%SDLC encapsulation is not configured on this interface.");
            return;
        }

        if (csb->sense) {
            if (GETOBJ(int, 3)) {  /* Default dlsw partner command */
                ieee_copy(GETOBJ(hwaddr,1)->addr, newmac);
                if (newmac[0] & 0x80) {
                    printf("\nmalformed hex mac address");
                    break;
                }
                idb->sdlc_have_default_partner = TRUE;
                ieee_copy(newmac,idb->sdlc_default_partner);
                idb->sdlc_default_inbound = GETOBJ(int,4);
                idb->sdlc_default_outbound = GETOBJ(int,5);
                return;
            }

            /*
             * process "sdlc dlsw [default | addr]
             */
            if (GETOBJ(int, 1)) {
                /*
                 * "sdlc dlsw <addr> <addr> ..."
                 *
                 * This command may be replacing "sdlc dlsw deault",
                 * it could be adding one or more <addr>, it could
                 * be removing one or more <addr>, or all of the above.
                 * To deal with this, we must walk all sdb's and determine
                 * if this address is being added, removed, or is
                 * remaining in the list.
                 *
                 * This command overrides any pervious "sdlc dlsw default".
                 */
                idb->sdlc_default = NO_SDLC;
 
                /*
                 * First deal with addresses that are being detached from DLSw.
                 */
                for (i = SDLC_MIN_ADDRESS; i <= SDLC_MAX_ADDRESS; i++) {
                    if ((idb->sdlc_data[i]) && (idb->sdlc_data[i]->sdlc_dlsw)) {
                         /*
                         * This addr was previously attached to DLSw.
                         */
                        addr_to_be_detached = TRUE;
                        for (j = 0; j < GETOBJ(int, 1); j++) {
                            if (i == csb->uiarray[j]) {
                                addr_to_be_detached = FALSE;
                                break;
                            }
                        }
 
                        if (addr_to_be_detached) {
                           /*
                            * <addr> is being removed. Must cause the PU to
                            * disconnect.
                            */
                           if (idb->sdlc_data[i]->u_cep_id !=
                                                (sdlc_data_block*) CLS_NO_CEP) {
                               sdlcClsNotify(idb->sdlc_data[i],
                                             CLS_DISC_IND, CLS_OK);
                           }
 
                           idb->sdlc_data[i]->sdlc_dlsw = FALSE;
                           idb->sdlc_data[i]->u_sap_id = 0;
                        }
                    }
                }
 
                /*
                 * Now deal with addresses that are being attached to DLSw.
                 */
                for (i = 0; i < GETOBJ(int, 1); i++) {
                     if (!idb->sdlc_data[csb->uiarray[i]]) {
                          printf("\n%%SDLC Address %x is not configured.",
                                  csb->uiarray[i]);
                     } else {
                        idb->sdlc_data[csb->uiarray[i]]->sdlc_dlsw = TRUE;
                        idb->sdlc_data[csb->uiarray[i]]->addr_type =
                                                           STATION_IEEE48;
                        if (idb->sdlc_dlsw_u_sap_id)
                            idb->sdlc_data[csb->uiarray[i]]->u_sap_id =
                                                idb->sdlc_dlsw_u_sap_id;
                    }
                }
 
                /*
                 * Only request an activate ring if necessary.
                 * As long as DLSw has been attached to one addr or
                 * used as the default, subsequent calls are not needed.
                 */
                if (idb->sdlc_dlsw_u_sap_id == 0)
                    reg_invoke_srt_set_srb_params(idb, TRUE);
 
                return;
            }
 
            if (GETOBJ(int, 6) && !idb->sdlc_default != DLSW_SDLC) {
	      if (idb->sdlc_default == FRAS_SDLC) {
		printf("\nFRAS is configured on this interface.");
		return;
	      }
                /*
                 * New "sdlc dlsw default" command.
                 * If this is the first time DLSw is being attached to this
                 * port, than request an activate ring and let
                 * sdlcActivateRingReq() initialize the sdb->u_sap_id
                 * later. Otherwise, if the DLSw is already attached to this
                 * port, we must go through all sdb's and initialize their
                 * u_sap_id to the DLSw u_sap_id since DLSw will not issue
                 * a duplicate activate ring.
                 */
                idb->sdlc_default = DLSW_SDLC;
                if (idb->sdlc_dlsw_u_sap_id == 0) {
                     for (i = SDLC_MIN_ADDRESS; i <= SDLC_MAX_ADDRESS; i++) {
                        if (idb->sdlc_data[i])
                            idb->sdlc_data[i]->sdlc_dlsw = FALSE;
                    }
                    reg_invoke_srt_set_srb_params(idb, TRUE);
                } else {
                    for (i = SDLC_MIN_ADDRESS; i <= SDLC_MAX_ADDRESS; i++) {
                        if (idb->sdlc_data[i]) {
                            idb->sdlc_data[i]->u_sap_id = 
                                                  idb->sdlc_dlsw_u_sap_id;
                            idb->sdlc_data[i]->sdlc_dlsw = FALSE;
                            idb->sdlc_data[i]->addr_type = STATION_IEEE48;
                        }
                    }
                }
                return;
            }
            return;
        } else {
            /*
             * "no sdlc dlsw ...."
             */
            if (GETOBJ(int, 3)) {  /* Default dlsw partner command */
                idb->sdlc_have_default_partner = FALSE;
                ieee_zero(idb->sdlc_default_partner);
                idb->sdlc_default_inbound = FALSE;
                idb->sdlc_default_outbound = FALSE;
                return;
            }
 
            if (idb->state == IDBS_UP) {
                printf("\nCannot detach interface from DLSw when the interface is up.");
                printf("\nadministratively shut down this interface first");
                return;
            }
 
            if (GETOBJ(int, 1)) {
                /*
                 * no sdlc dlsw <addr> <addr>
                 *
                 * If "sdlc dlsw default" was previously configured,
                 * do nothing.
                 */
                if (idb->sdlc_default == DLSW_SDLC) {
                    printf("\n%%sdlc dlsw default is configured");
                    return;
                }
 
                /*
                 * Detach all addresses specified in the "no" command
                 * from DLSw. After this, if there aren't any other addresses
                 * attached to DLSw, then request a deactivate ring.
                 */
                for (i = 0; i < GETOBJ(int, 1); i++) {
                     if (!idb->sdlc_data[csb->uiarray[i]]) {
                          printf("\n%%SDLC Address %x is not configured.",
                                  csb->uiarray[i]);
                     } else {
                        idb->sdlc_data[csb->uiarray[i]]->sdlc_dlsw = FALSE;
                        idb->sdlc_data[csb->uiarray[i]]->u_sap_id = 0;
                     }
                }
 
                sdlc_dlsw_addr_conf = FALSE;
                for (i = SDLC_MIN_ADDRESS; i <= SDLC_MAX_ADDRESS; i++) {
                    if ((idb->sdlc_data[i]) && (idb->sdlc_data[i]->sdlc_dlsw)) {
                        sdlc_dlsw_addr_conf = TRUE;
                        break;
                     }
                }
 
                if (!sdlc_dlsw_addr_conf)
                    reg_invoke_srt_set_srb_params(idb, FALSE);
            }
 
            if (GETOBJ(int, 6)) {
                /*
                 * "no sdlc dlsw default" command.
                 */
                if (idb->sdlc_default == DLSW_SDLC) {
                    idb->sdlc_default = NO_SDLC;
                    reg_invoke_srt_set_srb_params(idb, FALSE);
                } else {
                    printf("\n%%sdlc dlsw default was not previously configured"
);
                }
            }
            return;
        }
      }
        break;
  
      case SDLC_CMD_GROUP_POLL:
        if (!ISSSDLC(idb)) {
            printf("\nSDLC: Group Poll is only applicable to SDLC secondary");
            return;
        }
        if (csb->sense)
            idb->sdlc_group_addr = GETOBJ(int,1);
        else
            idb->sdlc_group_addr = 0;
        break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


/*
 * Routines to manage deleting an active SDB
 */

void sdlc_mark_deleted (sdlc_data_block *sdb)
{
    sdb->sdlc_zombie = TRUE;
}

boolean is_sdlc_marked_deleted (sdlc_data_block *sdb)
{
    return (sdb->sdlc_zombie);
}


/*
 * sdlc_default_n1
 * Determine an N1 parameter given the multitude
 * layers we must deal with
 */

int sdlc_default_n1 (register hwidbtype *idb)
{
    int size;

    size = SDLCHEADERBYTES;
    size += idb->maxmtu;
    return (size * 8);
}

/*
 * sdlc_linestate
 * Return current line protocol status
 */

static boolean sdlc_linestate (hwidbtype *hwidb)
{
    return(hwidb->sdlc_upstate);    
}
/*
 * sdlc_count_input
 * Count the input frame for show interface & mibs.
 * Display the packet if debugging is enabled.
 */
void sdlc_count_input (hwidbtype *idb, paktype *pak)
{
    sdlc_frametype *frame;
    sdlc_data_block *sdb;

    /*
     * Note, the sdb can be null for group poll.  We'll print it anyway.
     */
    frame = (sdlc_frametype *) pak->datagramstart;
    sdb = idb->sdlc_data[frame->address];

    if (sdb && SDLC_DEBUG(sdb,pak))
	buginf("\n%s: SDLC I %02x %s ", idb->hw_namestring, 
	       frame->address, sdlc_prstate(sdb, 0));

    if (!sdb && sdlc_debug) 
	if ((frame->control & ~SDLC_PF) == SDLC_UPOLL) 
	    buginf("\n%s: SDLC I [%02x] UPOLL P",
		   idb->hw_namestring, 
		   frame->address);

    if (sdb && SDLC_DEBUG(sdb, pak)) {
	buginf("%s, [VR: %d VS: %d]", 
	       sdlc_prframe(idb, pak, sdb, SDLC_NOFRAME, 
			    (frame->control & SDLC_PF)),
	       sdb->sdlc_vr, sdb->sdlc_vs);
    }

    sdlc_packet_dump(sdb, pak, pak->if_input->namestring, "SDLC input");

    if (!sdb) return;	    

    /*
     * Now count the packet
     */
    if (SDLC_ISUFRAME(frame)) {
	switch (frame->control & ~SDLC_PF) {
	  case SDLC_SNRM:   sdb->sdlc_snrmsrcvd++;  break;
	  case SDLC_FRMR:   sdb->sdlc_frmrsrcvd++;  break;
	  case SDLC_DISC:   sdb->sdlc_discsrcvd++;  break;
	  case SDLC_DM:     sdb->sdlc_dmsrcvd++;    break;
	  case SDLC_UA:     sdb->sdlc_uasrcvd++;    break;
	  case SDLC_XID:    sdb->sdlc_xidsrcvd++;   break;
	  case SDLC_TEST:   sdb->sdlc_testsrcvd++;  break;
	  case SDLC_UI:     sdb->sdlc_uiframesrcvd++; break;
	  case SDLC_UPOLL:  break;
	  case SDLC_SIM:    
	    ISSSDLC(idb) ? sdb->sdlc_rimsrcvd++ : sdb->sdlc_simsrcvd++; 
	    break; 
	  default: 
	    errmsg(&msgsym(BADFRAME,SDLC), idb->hw_namestring, "U-",
		   frame->address, frame->control & ~SDLC_PF);
	}
	return;
    } 

    if (frame->control & SDLC_PF)
	ISSSDLC(idb) ? sdb->sdlc_pollsIn++ : sdb->sdlc_pollRspsIn++;

    if (SDLC_ISSFRAME(frame)) {
	switch(frame->control & 0xf) {
	  case SDLC_RR: break;
	  case SDLC_RNR: sdb->sdlc_rnrsrcvd++;  break;
	  case SDLC_REJ: sdb->sdlc_rejsrcvd++;  break;
	  default: 
	    errmsg(&msgsym(BADFRAME,SDLC), idb->hw_namestring, "S-",
		   frame->address, frame->control & ~SDLC_PF);
	}
    } else {
	if (SDLC_ISIFRAME(frame)) {
            /* Octets received covers the address, control,
             * and information field of iframes only.
             */
            sdb->sdlc_octetsrcvd += pak->datagramsize;
	    sdb->sdlc_iframes_since_poll++;
	    sdb->sdlc_iframesrcvd++;
	}
    	else
	    errmsg(&msgsym(BADFRAME,SDLC), idb->hw_namestring, "",
		   frame->address, frame->control);
    }
}

/*
 * sdlc_parse_packet
 *
 * Set up a packet buffer so that the network level protocol fields
 * are initialized.
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 */
static iqueue_t sdlc_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    if (!pak->if_input) {
	errmsg(&msgsym(NOINPIDB, STUN));
	return(NULL);
    }

    /*
     * If we're building a virgin frame, we have to set datagramstart
     */
    if (data) {
	pak->datagramstart = pak_center(pak) - SDLCHEADERBYTES;
    }

    pak->mac_start     = pak->datagramstart;
    pak->addr_start    = NULL;
    pak->rif_start     = NULL;
    pak->riflen	       = 0;
    pak->info_start    = NULL;
    pak->network_start = pak->mac_start + SDLCHEADERBYTES;
    /*
     * Record just how big this encapsulation was rather than every protocol
     * have it's own fun and wacky way to work it out.
     */
    pak->encsize = pak->network_start - pak->datagramstart;

    if (ISCLSSDLC(idb))
	return(sdlc_wrap_input);	/* SDLC wrapper */
    return(sdlc_input);
}

static boolean sdlc_retry_cls (sdlc_data_block *sdb)
{
    paktype *xid;

    /* Switched mode for PU2.x. If stn is closed, 
     * check the timer and restart if needed.
     */


    if ((sdb && 
 	(sdb->sdlc_switched) &&
 	((sdb->cls_state == CLS_STN_CLOSED)))) { 
 
	/* Has the sdlc switched-startup timer expired since
 	 * we last sent a switched mode initiated XID to the station?
 	 */

         if (!SLEEPING(sdb->sdlc_switched_startup_timer)) {
 	    if (sdlc_debug)
	      buginf("\nsdlc_retry_cls: switched_startup timer has expired,"
		     "addr=%02x",sdb->sdlc_address);
	    /* 
	     * Free whatever is in outbuf and get a NULL XID 
	     * for the main timer to send.
	     */
 	    if (sdb->outbuf)
 	        datagram_done(sdb->outbuf);                 
 	    sdb->outbuf =  GetProxyXid(sdb,FALSE);
 
 	    /* Use address 0xFF if only 1 station on link */
             if (!sdlc_is_multidrop(sdb)) {
		 sdb->outbuf->datagramstart[0] = SDLC_BCAST_ADDR;
	     }
 	      	      
 	    /* reset the timer to some point in the future.
 	     * Use user-specified slow_poll or t1 timer value.
 	     */
 	    if (sdb->sdlc_idb->sdlc_slow_poll_interval)  {
 	        TIMER_START(sdb->sdlc_switched_startup_timer,
 			    sdb->sdlc_idb->sdlc_slow_poll_interval);
 	    } else {
		TIMER_START(sdb->sdlc_switched_startup_timer,
			    sdb->sdlc_idb->sdlc_t1); 
	    }

	    /* cause main timer to send XID to stn */
 	    sdlc_setstate_force_polled(sdb->sdlc_idb, sdb);   
 
            /* set cls_state so sdlc_wrap_input knows the incoming
	     * response was for a  switched-startup  initiated XID.
	     * Update sdlc state to XID_SENT.
	     */  
 	    sdb->cls_state = CLS_STN_CLOSED;
 	    sdlc_setstate(sdb->sdlc_idb, SDLC_XIDSENT, sdb); 
 	}
	 return(TRUE);
     }
 	

    /*
     * Retry sending the XID for PU 2.0's if a number of conditions are met.
     * For Secondary, we send XID once we have seen SNRM from the host, but
     * only if the user configured us to explore out.
     * For Primary, we send XID0T2 as soon as the downstream device comes 
     * online, but not if user configures us for inbound.  We use the macro
     * IS_SDLC_CONNECT because it might be USBUSY or BOTHBUSY; either way 
     * we'll send XID's.
     * XID Proxy to host will occur if:
     * 1. Primary PU 2.0 and sdlc_state is CONNECT, but not in inbound mode.
     * 2. Secondary PU 2.0 and outbound is configured and SNRM was seen
     * 3. We are PU 2.1, not in inbound mode, and sdlc_state is DISCONNECT. 
     *    (Inbound option stops router from proxying host. CSCdi70552)
     */


    if ((IsPrimPu2(sdb) && !IsInbound(sdb) && IS_SDLC_CONNECT(sdb)) ||
        (IsSecPu2(sdb) && IsOutbound(sdb) &&
	 sdb->sdlc_state == SDLC_SNRMSEEN) ||
	(IsPrimXidPoll(sdb) && 
	 !IsInbound(sdb)    &&
	 (sdb->sdlc_state == SDLC_DISCONNECT))) {

        if (sdb->sdlc_xid_count >= sdb->sdlc_xid0_n2)
            sdb->sdlc_xid_count = 0;

        /*
         * If not 2.0 primary, always proxy with XID NULL.
         * For 2.0 primary; if this is the first XID sdlc is sending
         * to CLS or we are restarting the connect sequence (either way
         * sdlc_xid_count is zero), then send XID NULL. Otherwise,
         * send XID0. The reason that we restart the xid connect sequence
         * after n2 XID0's is that VTAM needs to see XID NULL while
         * the PU is ACTIVE or it will not respond to XID0.
         *
         * Note 1: CLS_CONNECT_REQ_PEND state is only used for
         * IsPrimPu2() (2.0) sdlc devices, therefore, checking it
         * is equivalent to checking IsPrimPu2().
         *
         * Note 2: The first XID0 is sent by sdlcIdReq() when the
         * XID NULL Response is received.
         */
        xid = GetProxyXid(sdb,
                          (sdb->cls_state == CLS_CONNECT_REQ_PEND) &&
                          sdb->sdlc_xid_count);

	if (xid) {
	    sdlcSendXidToCLS(sdb, xid);
	    sdb->sdlc_xid_count++;
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * s d l c _ i n i t i a t e
 *
 * Scan a given idb and generate on behalf of a given sdlc PU fire off the
 * required sequence to bring up a session.  This routine will generate
 * Explorers and XIDs as appropriate.  These can be very distructive to the
 * network so we limit how many we generate.  The allowed number is passed in
 * as num_explorers and num_xids.
 */

static void sdlc_initiate ( hwidbtype *idb, int *explorers_left, int *xids_left)
{
    sdlc_data_block *sdb;
    int addr;

    if (idb->sdlc_next_startup > SDLC_MAX_ADDRESS)
        idb->sdlc_next_startup = 0;
    for (addr = idb->sdlc_next_startup; (addr <= SDLC_MAX_ADDRESS); addr++) {
        sdb = idb->sdlc_data[addr];

	if (!sdb)
	    continue;

	/*
	 * We sent and RD but haven't gotten a DISC.  Go back to DISCSENT so we'll 
	 * send another one
	 */
	if (sdb->sdlc_state == SDLC_DISCPEND) {
            sdlc_setstate(idb, SDLC_DISCSENT, sdb);
            if ((idb->sdlc_current_polled == sdb->sdlc_address) ||
                (idb->sdlc_current_polled == 0)) { /* If it's our turn, or no one else */
                    sdlc_set_pause_timer(idb, sdb);
            }
	    continue;
	}

	if (ISCLSSDLC(idb)) {
	    if (sdb->u_sap_id == 0)
		continue;
            if (sdb->addr_type == STATION_IEEE48 && !(IsPartnerCfgd(sdb)))
		continue;

	    if (IsPrimPu2(sdb) && (sdb->sdlc_state == SDLC_DISCONNECT)) {
		if (sdlc_debug)
		    buginf("\n sdlc_initiate: restarting %02x, %s/%s",
			   sdb->sdlc_address,
			   pr_cls_sdlc_state(sdb), 
			   sdlc_prstate((sdb), 0));
                sdlc_setstate(sdb->sdlc_idb, SDLC_SNRMSENT, sdb);
                if ((idb->sdlc_current_polled == sdb->sdlc_address) ||
                    (idb->sdlc_current_polled == 0)) { /* If it's our turn, or no one else */
                        sdlc_set_pause_timer(sdb->sdlc_idb, sdb);
                }
	    }

	    if (TIMER_RUNNING_AND_SLEEPING(sdb->sdlc_initiation_throttle_time))
                continue;

	    if (sdb->cls_state <= CLS_FULL_XID_PEND) {
		if (sdlc_retry_cls(sdb))
		    (*xids_left)--;
	    }
	    TIMER_START(sdb->sdlc_initiation_throttle_time,
			SDLC_THROTTLE_INTERVAL);

	    /*
	     * check to see if we've generated enough noise for now
	     */
	    if (*xids_left <= 0)
		break;
	    
	} else {
            if (!IsPartnerCfgd(sdb))
		continue;

	    if (sdb->sdllc_state == SDLLC_DISCONNECT) {
		/*
		 * we missed a cstate transition and the i/f is up
		 * but sdllc is disconnected, force a disconnect so the 
		 * sdlc/sdllc comes back up.
		 * GE fix
		 */
		sdllc_ind_disconnect(idb, sdb);
		continue;
	    }
	    if (sdb->sdllc_state != SDLLC_AW_NET)
		continue;
	    
	    if (SLEEPING(sdb->sdlc_initiation_throttle_time))
		continue;

	    switch(sdllc_initiate_pu(idb, sdb)) {
	    case LLC1_XID:
		(*xids_left)--;
		break;
	    case LLC1_TEST:
		(*explorers_left)--;
		break;
	    default:
		break;
	    }
	    TIMER_START(sdb->sdlc_initiation_throttle_time,
			SDLC_THROTTLE_INTERVAL);

	    /*
	     * check to see if we've generated enough noise for now
	     */
	    if (*explorers_left <= 0 || *xids_left <= 0)
		break;
	}
    }
    if (addr > SDLC_MAX_ADDRESS)
        addr = 0;
    idb->sdlc_next_startup = addr;
}

/*
 * s d l c _ p e r i o d i c
 *
 * Periodic sdlc work.  Used for device initiated connections whereby the
 * Cisco router will generate either an XID or a TEST(explorer) packet
 * every minute trying to establish a connection with the token ring host. 
 *
 * We throttle what kind of noise we can generate by limiting how many in
 * a given interval of explorers and xids we will send.  If we run out of
 * either limit then we stop and pick up next time.  Note that when a given
 * PU has an explorer or xid generated on its behalf it will pop its thottle
 * timer.  This allows forward progress.
 */
static void sdlc_periodic (void)
{
    hwidbtype *idb;
    int explorers_left;
    int xids_left;

    explorers_left = xids_left = SDLC_INIT_COUNT;
    FOR_ALL_HWIDBS(idb) {
	if ((idb->state == IDBS_UP) && 
	    ((idb->sdllc_active) || ISCLSSDLC(idb)))
	{
	    if ((idb->enctype != ET_SDLCP) && (idb->enctype != ET_SDLCS)
		&& (!ISCLSSDLC(idb)))
		continue;
	    sdlc_initiate(idb, &explorers_left, &xids_left);
	    if (explorers_left <= 0 || xids_left <= 0)
		break;
	}
    }
    if (!ISCLSSDLC(idb))
	sdllc_sec_conn_tear_down();
}


/*
 * sdlc_setup
 * Set up a SDLC encapsulation
 */

void sdlc_setup1 (register hwidbtype *idb)
{
     idbtype *swidb;
    /*
     * Protocol handling vectors
     */
    idb->vencap = (vencap_t)return_false;;
    idb->deencap = sdlc_deencap;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    idb->getlink = sdlc_getlink;
    idb->show_support = sdlc_interface;
    idb->oqueue = holdq_enqueue;
    idb->oqueue_dequeue = holdq_dequeue;
    if (ISCLSSDLC(idb))
	idb->iqueue = sdlc_wrap_input;
    else
	idb->iqueue = sdlc_input;
    idb->parse_packet = sdlc_parse_packet;
    idb->extract_hwaddr = invalidate_hwaddr;
    idb->extract_snpa = invalidate_snpa;
    idb->broadcast = (broadcast_t) return_fuz_false;
    idb->periodic = NULL;
    idb->nokeepalive = TRUE;
    idb->lineaction = sdlc_cstate;
    idb->linestate = sdlc_linestate;
    /*
     * disable fair queuing if active on interface
     */
    delete_fair_queue(idb);
    /*
     * Use HDLC_HEADER_SIZE instead of SDLCHEADERBYTES since the microcode
     * requires the mci_rxoffset to be long word aligned, and mci_rxoffset
     * is calculated using encsize.
     */
    idb->encsize = HDLC_HEADER_SIZE;
    if (!sdlc_tmr) {
	/* 
	 * create timer, set period to 5 secs, and start timer 
	 */
 	sdlc_tmr = create_timer((timer_func)sdlc_periodic, 0,0,0, 
 				"SDLC TEST/XID proxy", 
 				TMT_CONTINUOUS);
 	set_timer(sdlc_tmr, ONESEC * 2); 
    }
    FOR_ALL_SWIDBS_ON_HW(idb, swidb)
        swidb->cdp_enabled = FALSE ;
}

void sdlc_setup2 (register hwidbtype *idb)
{
    /*
     * Protocol dependant fields
     */
    idb->sdlc_t1 = SDLC_DEFAULT_T1;	/* 3 seconds */
    idb->sdlc_n1 = SDLC_DEFAULT_N1(idb); /* bits */
    idb->sdlc_n2 = SDLC_DEFAULT_N2;	/* 20 times */
    idb->sdlc_k = SDLC_DEFAULT_K;	/* seven outstanding packets */
    if (idb->sdlc_poll_wait == 0)
        idb->sdlc_poll_wait = SDLC_DEFAULT_POLL_WAIT; /* 10 seconds */
    if (idb->sdlc_poll_pause_timer == 0)
	idb->sdlc_poll_pause_timer = SDLC_DEFAULT_POLL_PAUSE_TIMER;
    if (idb->sdlc_poll_limit_value == 0)
	idb->sdlc_poll_limit_value = SDLC_DEFAULT_POLL_LIMIT_VALUE;
    TIMER_START(idb->sdlc_top_timer, idb->sdlc_poll_pause_timer);
    idb->sdlc_line_speed = 0;
    idb->sdlc_uc_delay = 0;
    idb->sdlc_upstate = TRUE;
    idb->sdlc_slow_polled = 0;
    GET_TIMESTAMP(idb->sdlc_slow_poll_timer);
    /*
     * slow poll should be enabled by default
     * CSCdi13762
     */
    idb->sdlc_slow_poll_interval = SDLC_DEFAULT_SLOW_POLL;
    idb->sdlc_output_flag = 0;
    idb->sdlc_group_addr = 0;
}

static boolean sdlc_setup (parseinfo *csb, hwidbtype *hwidb)
{
    if (routed_protocols_on_idb(hwidb->firstsw)) {
	printf("\nSDLC encapsulation cannot be set on any interface running a routed protocol.");
	printf("\nThis interface's (%s) type will now revert to HDLC. Please",
	       hwidb->hw_namestring);
	printf("\nremove any references to routed protocols on this interface before");
	printf("\nenabling it for SDLC.");
	hwidb->enctype = ET_HDLC;
	hdlc_setup(NULL, hwidb);
	return(FALSE);
    }
    if (!hwidb->sdlc_data) {
	hwidb->sdlc_data = malloc_named(sizeof(sdlc_data_block *) * 256, sdlc_cb);
	if (!hwidb->sdlc_data) {
	    printf(nomemory);
	    return (FALSE);
	}
    }
    sdlc_setup1(hwidb);
    sdlc_setup2(hwidb);
    (*(hwidb->reset))(hwidb);
    return(TRUE);
}


/*
 * sdlc_cstate
 * Called by the cstat handler when the carrier state changes
 * This routine is *not* an interface vector routine.
 *
 * When a SDLC interface goes down this routine runs through all active PUs
 * and forces them to DISCONNECT.
 *
 * When a SDLC interface comes up and it is a SDLC Primary, we assume that
 * SDLLC is running and fire up the primary side by forcing to SNRMSENT.
 */

void sdlc_cstate (hwidbtype *idb)
{
    int addr;
    sdlc_data_block *sdb;

    /*
     * Right now, let's yank all the sessions.  When we fix the 
     * per-session cleanup, then we can go back to this line:
     * reg_invoke_srt_set_srb_params(idb, csb->sense);
     */
	if (idb->state != IDBS_UP) {
	    for (addr = SDLC_MIN_ADDRESS; (addr <= SDLC_MAX_ADDRESS); addr++) {
		sdb = idb->sdlc_data[addr];
		if (sdb && sdb->sdlc_state > SDLC_DISCONNECT) {
		    sdlc_reset(idb, "SDLC carrier changed to down",
			       idb->sdlc_data[addr]);

		    SdlcNmEvent(SDLC_CarrierDown, 
				idb, idb->sdlc_data[addr]);/*NmEvent Hook 21*/
		}
	    }
	}

    if (idb->state == IDBS_UP && (idb->sdllc_active || 
				  idb->snx_active || ISCLSSDLC(idb))
	&& (ISPSDLC(idb))) {
        /*
         * SDLC interface has just come up.  If running SDLLC then use
         * sdllc_ind_disconnect (we are already disconnect, just need the
         * indication) to restart things.
         */
        for (addr = SDLC_MIN_ADDRESS; (addr <= SDLC_MAX_ADDRESS); addr++) {
            sdb = idb->sdlc_data[addr];
            if (!sdb || ( !(IsPartnerCfgd(sdb)) &&
			 !sdb->qllc_have_prtnr &&
			 !ISCLSSDLC(idb)))
                continue;
            /*
             * make sure we really are down.  There are race conditions where
             * this may not be the case.  Like with configuration.
             */
            if (sdb->sdlc_state < SDLC_SNRMSENT) {
		if (sdb->qllc_have_prtnr)
		    sdlc_ind_disconnect_snx(idb, sdb);
		else if (!ISCLSSDLC(idb)) 
		    sdllc_ind_disconnect(idb, sdb);
	    }
	}   
    }
}

/*
 * Output correct information for the show interface command
 */
void sdlc_interface (register hwidbtype *idb)
{
    sdlc_data_block *sdb;
    int addr;
    char *s = "     ";

    if (!is_sdlc(idb))
	return;

    printf("\n    Router link station role: %s%s", 
	    ISPSDLC(idb) ? "PRIMARY" : ISSSDLC(idb) ? "SECONDARY" : "NONE",
            idb->dte_interface ? " (DTE)" : " (DCE)");
    printf("\n    Router link station metrics:");

    if (ISPSDLC(idb)) {
        if (idb->sdlc_slow_poll_interval)
	    printf("\n%s slow-poll %d seconds",
		   s,idb->sdlc_slow_poll_interval/ONESEC);
	else
	    printf("\n%s slow-poll not configured",s);

	if (idb->sdlc_flags & SDLC_FLAG_NOFRMR)
	    printf("\n%s frmr-disable configured",s);

	printf("\n%s T1 (reply time out) %d milliseconds",s,idb->sdlc_t1);
        printf("\n%s N1 (max frame size) %d bits",s,idb->sdlc_n1);
	printf("\n%s N2 (retry count) %d ",s,idb->sdlc_n2);
	printf("\n%s poll-pause-timer %u milliseconds",
	       s,idb->sdlc_poll_pause_timer);
	printf("\n%s poll-limit-value %u",s,idb->sdlc_poll_limit_value);
	printf("\n%s k (windowsize) %u",s,idb->sdlc_k);
    } else {
	if (idb->sdlc_group_addr)
	    printf("\n%s group poll address %x",s,idb->sdlc_group_addr);
	else 
	    printf("\n%s group poll not enabled",s);
	printf("\n%s poll-wait %u seconds",s,idb->sdlc_poll_wait);
        printf("\n%s N1 (max frame size)  %d bits",s,idb->sdlc_n1);
    }

    printf("\n%s modulo %u",s,SDLC_MODULO);

    if (idb->sdllc_active) {
        srb_triplet_t srb_triplet;
        srbV.srb_sb_get_triplet(idb->firstsw, &srb_triplet);
	printf("\n      SDLLC [ma: %02x%02x.%02x%02x.%02x--, ring: %d bridge: %d, target ring: %d",
               idb->sdlc_vr_macaddr[0], idb->sdlc_vr_macaddr[1], idb->sdlc_vr_macaddr[2],
               idb->sdlc_vr_macaddr[3], idb->sdlc_vr_macaddr[4], 
               srb_triplet.localRing, 
               srb_triplet.bridgeNum,
               srb_triplet.remoteRing);
	printf("\n             largest token ring frame %u]",
		   rc_code_to_bytes(idb->sdllc_largest_token_frame));
    } 

    if (ISCLSSDLC(idb))
	printf("\n      sdlc vmac: %02x%02x.%02x%02x.%02x--",
               idb->sdlc_vr_macaddr[0], idb->sdlc_vr_macaddr[1], 
	       idb->sdlc_vr_macaddr[2], idb->sdlc_vr_macaddr[3], 
	       idb->sdlc_vr_macaddr[4]);
    
    for (addr = SDLC_MIN_ADDRESS; (addr <= SDLC_MAX_ADDRESS); addr++) {
	sdb =  idb->sdlc_data[addr];
	if (!sdb)
	    continue;
	printf("\n  sdlc addr %02x state is %s", sdb->sdlc_address,
	       sdlc_prstate(sdb, 0));
	if (sdb->sdlc_testmode)
	    printf(" -- test mode!");
	if (sdb->sdlc_twoaddrmode)
	    printf(" -- two address mode!");
	if (sdb->sdlc_xid_poll)
	    printf(" (xid-poll)");
	if (sdb->sdlc_xid_passthru)
	    printf(" (xid-passthru)");
	if (sdb->sdlc_switched)
	    printf(" (switched)");
	if (ISCLSSDLC(idb))
	    printf("\n      cls_state is %s", pr_cls_sdlc_state(sdb)); 
	printf("\n      VS %d, VR %d, Remote VR %d, Current retransmit count %d",
	       sdb->sdlc_vs, sdb->sdlc_vr, sdb->sdlc_rvr,
	       sdb->sdlc_retcnt);
        printf("\n      Hold queue: %u/%u IFRAMEs %d/%d",
               sdb->sdlc_holdq.count, sdb->sdlc_holdq.maximum,
               sdb->sdlc_iframessent, sdb->sdlc_iframesrcvd);
        printf("\n      TESTs %d/%d XIDs %d/%d, DMs %d/%d FRMRs %d/%d", 
	       sdb->sdlc_testssent, sdb->sdlc_testsrcvd,
               sdb->sdlc_xidssent, sdb->sdlc_xidsrcvd,
	       sdb->sdlc_dmssent, sdb->sdlc_dmsrcvd,
	       sdb->sdlc_frmrssent, sdb->sdlc_frmrsrcvd);
        printf("\n      RNRs %d/%d SNRMs %d/%d DISC/RDs %d/%d REJs 0/%d",
               sdb->sdlc_rnrssent, sdb->sdlc_rnrsrcvd,
               sdb->sdlc_snrmssent, sdb->sdlc_snrmsrcvd,
               sdb->sdlc_discssent, sdb->sdlc_discsrcvd,
                                    sdb->sdlc_rejsrcvd);

        if (ISPSDLC(idb)) {
	    printf("\n      Poll: %s", (sdb->sdlc_poll == TRUE) ?
		   ("set") : ("clear"));
	    printf(", Poll count: %u,", sdb->sdlc_poll_count);
            if (sdb->sdlc_ready_for_poll)
                printf(" ready for poll,");
	}
	printf(" chain: %02x/%02x", sdb->prev, sdb->next);
	if (idb->sdllc_active) {
	    printf("\n      SDLLC [largest SDLC frame: %u, XID: ",
		   sdb->sdlc_largest_frame);
	    if (sdb->sdlc_xid_configured) {
		printf("%02x%02x%02x%02x", sdb->sdlc_xid[0],
		       sdb->sdlc_xid[1], sdb->sdlc_xid[2],
		       sdb->sdlc_xid[3]);
	    } else {
		printf("disabled");
	    }
	    printf(", Partner: ");
	    if (sdb->sdlc_have_prtnr) {
		printf("%e", sdb->sdlc_prtnr);
	    } else {
		printf("disabled");
	    }
	    printf("\n             state: %s",
		   sdllc_prstate(sdb, 0));
	    printf(", LLC pointer: 0x%x", (int) sdb->sdllc_llc);
            if (sdb->sdllc_lak_enable)
               printf(", local ack");
	    printf("]");
	}
    }
}

/*
 * sdlc_ind_disconnect_snx()
 *
 * sdlc is indicating that it has shut down a PU.
 * Or we detected a status change in the line condition.
 * Allow the upper layers to do what they need to do.
 */

void sdlc_ind_disconnect_snx (hwidbtype *idb, sdlc_data_block *sdb)
{

 if (sdlc_qllc_debug) {

     buginf("\nsdlc_ind_disconnect_snx %s - %02x, state %s", 
	    idb->hw_namestring, sdb->sdlc_address,
	    snx_prstate(sdb->snx_state));
 }

 if(sdb->snx_state == SNX_CONNECT && sdb->sdlc_state == SDLC_DISCONNECT)
  {
   /* covers the case where SDLC side is shut down and QLLC side 
    * is in connect state, i.e, we are the initiator. 
    */
   if(sdb->lnxp)
    {
     /* close it down only if we are the connect state.
      * This will prevent us from going in a loop if the other
      * side started this.
      */
     reg_invoke_sdlc_clean_snx(sdb);  
    }
   snx_set_state(sdb, SNX_DISCONNECT); 
  }
 /* This is the case of fresh start, or when QLLC was the initiator 
  * We will do our thing, which is setting up the SDLC side and    
  * and try the QLLC side periodically for connection establishment.
  */
 else if(sdb->sdlc_state == SDLC_DISCONNECT) 
  snx_set_state(sdb, SNX_DISCONNECT);   

 /*
  * Detect closing transition and do appropriate actions.
  * We check explicitly for ET_SDLCP rather than using ISPSDLC because
  * ISPSDLC also will return true if ET_STUN && stun_sdlc_encap is ET_SDLCP.
  * We only want to fire this side up if we are pure SDLC Primary.  This makes
  * the assumption that pure SDLC Primary is only turned on because we are
  * using SDLC-QLLC.
  */
 if (idb->enctype == ET_SDLCP && sdb->sdlc_state == SDLC_DISCONNECT &&
     sdb->snx_state == SNX_DISCONNECT && idb->state == IDBS_UP) 
  {
   /*
    * We are a Primary SDLC link and we are doing SDLLC.  We
    * are in DISCONNECT.  Go right back into SNRMSENT.
    */
    snx_set_state(sdb, SNX_AW_SDLC);
    sdlc_setstate(idb, SDLC_SNRMSENT, sdb);
    sdlc_set_pause_timer(idb, sdb);
   }
}


/*
 * sdlc_ind_connect_snx()
 *
 * sdlc is indicating that it has gone from a disconnected state
 * to a connected state.  Tell the upper layers and let them do what
 * they need to do.
 */
void sdlc_ind_connect_snx (hwidbtype *idb, sdlc_data_block *sdb)
{
 if (!sdb)
  return;

 if (snx_running && idb->snx_active) 
     {
	 /*
	  * This interface is running because of SDLLC.
	  */
	 switch(sdb->snx_state) {
	 case SNX_AW_SDLC:
	     snx_set_state(sdb,  SNX_AW_QLLC);
	     sdlc_set_clear_busy(idb, sdb->sdlc_address, TRUE);
	     break;
	     
	 case SNX_DISCONNECT:
	 case SNX_DW_SDLC:
	 case SNX_AW_QLLC:
	 case SNX_CONNECT:
	 default:
	     snx_set_state(sdb, SNX_DISCONNECT);
	     sdlc_setstate(idb, SDLC_DISCONNECT, sdb);
	     break;
	 }
     }
}

uchar *snx_prstate (snx_state_t snx_state)
{
    switch(snx_state) {
    case SNX_DISCONNECT:	return("SNX_DISCONNECT");
    case SNX_DW_SDLC:		return("SNX_DW_SDLC");
    case SNX_AW_QLLC:		return("SNX_AW_QLLC");
    case SNX_AW_SDLC:		return("SNX_AW_SDLC");
    case SNX_CONNECT:		return("SNX_CONNECT");
    default:			return ("Unknown");
    }
}

void snx_set_state (sdlc_data_block *sdb, snx_state_t newstate)
{
    if (sdb->snx_state != newstate) {
	if (sdlc_qllc_debug) {
	    buginf("\nSNX: %s - %02x, state %s->%s",
		   sdb->sdlc_idb->hw_namestring, sdb->sdlc_address,
		   snx_prstate(sdb->snx_state), snx_prstate(newstate));
	}
	sdb->snx_state = newstate;
    }
}

#ifdef NOBODY_CALLS_ME
/*
 * stun_sdlc_tg_reroute
 * Called from stun_sdlc_network_input_local_ack() in stun_sdlc.c.
 * The function is defined here in sdlc.c because it needs to understand
 * the sdlc_data_block which is declared in sdlc.h. Due to header file
 * conflicts sdlc.h and stun_sdlc.h can not be included by the same C file.
 */
void stun_sdlc_tg_reroute (paktype *pak, stunpeertype *stp)
{
    sdlc_data_block *next_sdb = NULL;
    stunpeertype *next_stp = NULL;
    hwidbtype *next_idb;
    hwidbtype *current_idb;
    sna_fid4_hdr *sna_hdr;

    current_idb = stp->stun_idb;
    for (next_idb = stun_first_idb; next_idb; next_idb = IDB_NEXT(next_idb)) {
        if ((next_idb == current_idb) ||
            (next_idb->stun_group != current_idb->stun_group) ||
            (current_idb->maxmtu > next_idb->maxmtu) ||
            (!(next_stp = (stunpeertype *) next_idb->stun_Q.qhead)))
            continue;

        next_sdb = (sdlc_data_block *) next_idb->sdlc_data[next_stp->addr[0]];
        if (next_sdb->sdlc_state != SDLC_CONNECT)
            continue;

        if (QUEUEFULL(&next_sdb->sdlc_holdq)) {
            if (stun_tg_debug)
                buginf("\nSDLC_REROUTE: can not reroute on %s - Q full",
                        next_idb->hw_namestring);
            continue;
        }
        if (stun_tg_debug)
            buginf("\nSDLC_REROUTE: alternate route idb = %s, part of tg : %d",
                    next_idb->hw_namestring, next_idb->stun_group);
        break;
    }

    if (next_idb) {
        pak->if_output = next_idb->firstsw;
        next_stp->slack_stats.i_frames_sent++;
        sna_hdr = (sna_fid4_hdr *) (pak->datagramstart + 2);

        if ((stp->tg_active) && ((sna_hdr->fid_tg & SNA_FID) == SNA_FID4) ) {
            pak->peer_ptr = next_stp;
            stun_cos_dest_reseq(next_stp, pak);
        } else {
            if (stun_tg_debug)
                buginf("\nSDLC_REROUTE: moving frame to %s holdq",
                        next_idb->hw_namestring);
            sdlc_holdq(next_idb, next_sdb, pak);
        }
    } else {
        if (stun_tg_debug)
            buginf("\nSDLC_REROUTE: dropping PIU - no alternate route");
        stp->drops++;
        stif_nukepak(pak);
   }
   return;
}
#endif /* NOBODY_CALLS_ME */


/*
 * Initialize SDLC stuff
 */

void sdlc_init (subsystype *subsys)
{
    sdlc_tf = 0;

    /*
     * Set up debugging flags
     */
    sdlc_debug_init();

    /*
     * Register some functions
     */
    reg_add_media_serial_setup(ET_SDLCP, sdlc_setup, "sdlc_setup");
    reg_add_media_serial_setup(ET_SDLCS, sdlc_setup, "sdlc_setup");
    reg_add_media_serial_setup(ET_SDLC, sdlc_setup, "sdlc_setup");
    reg_add_dlc_wput(CLS_SDLC, sdlc_wput, "sdlc_wput");
    reg_add_rsrb_add_vring_interface(sdllc_add_vring_interface,
				     "sdllc_add_vring_interface");
    reg_add_rsrb_remove_vring(sdllc_remove_vring, "sdllc_remove_vring");

    snx_running = FALSE;

    /*
     * Set up sdlc private function registry
     */
    sdlc_regcode = registry_create(REG_UNKNOWN, SERVICE_SDLC_MAX, "SDLC");
    if (sdlc_regcode != REG_ILLEGAL)
	create_registry_sdlc();

}

#ifdef NOT_YET
boolean sdlc_validate_xid(sdlc_data_block *sdb, char *xidptr, int xidlen)
{

    XID3type *xid3 = (XID3type *)xidptr;
    xid_format = xid_data->xid3_fmt & XID_FMT_MASK;
    if (xid_format != XID_FMT_0 && 
	xid_format != XID_FMT_1 &&
	xid_format != XID_FMT_3) {

      errmsg(&msgsym(XID_NOMATCH, SDLLC), sdlc_idb->hw_namestring, sdb->sdlc_address);
      errmsg(&msgsym(XID_FORMAT, SDLLC), xid_fmt >> 4);
      return (FALSE);
    }
    if (bcmp(&(sdb->sdlc_xid), &xid_data->xid3_blk_idnum), XID_SIZE)!=0){
      errmsg(&msgsym(XID_NOMATCH, SDLLC), sdlc_idb->hw_namestring, sdb->sdlc_address);
      errmsg(&msgsym(XID_DISPLAY, SDLLC), 
             xid_data->xid3_idblk_idnum[0], xid_data->xid3_idblk_idnum[1],
             xid_data->xid3_idblk_idnum[2], xid_data->xid3_idblk_idnum[3]);
      return (FALSE);
    }
    return (TRUE);
}
#endif

/* Return TRUE, if we did not finish all the transimission before
 * final bit came back.
 */
static boolean tws_send(register hwidbtype *idb, sdlc_data_block *sdb, boolean poll)
{
    register paktype *mod8_pak;

    /*
     * First, send any pending output, if the other side is not
     * busy
     */
    if ((sdb->sdlc_state == SDLC_CONNECT) ||
	(sdb->sdlc_state == SDLC_USBUSY)) {
	if (tws_send_xmtleg(idb, sdb, poll)) {
	    return TRUE;
	}
    }

    /* Final bit has been sent, either pigyback on an I frame or
     * Send in an RNR frame.
     */
    if (poll && sdb->sdlc_poll) {
	return FALSE;
    }

    if (poll && !sdb->sdlc_poll) {
	
        mod8_pak = getbuffer(2); 
        if (!mod8_pak) { 
            sdlc_tf = 0; 
            process_kill(THIS_PROCESS);
        } 
        mod8_pak->datagramstart = mod8_pak->network_start - SDLCHEADERBYTES;
        mod8_pak->enctype = ET_SDLCP; 
        mod8_pak->datagramstart[0] = sdb->sdlc_address; 
        mod8_pak->datagramsize = SDLCHEADERBYTES; 
        mod8_pak->datagramstart[1] = (((sdb->sdlc_state == SDLC_CONNECT) || 
				   (sdb->sdlc_state == SDLC_THEMBUSY)) ? 
				  SDLC_RR : SDLC_RNR); 
        mod8_pak->datagramstart[1] |= SDLC_PF | (sdb->sdlc_vr << 5); 
        if (SDLC_DEBUG(sdb, mod8_pak)) 
            buginf("\nSDLC: Sending RR/RNR at tws_send"); 
        sdlc_sendframe((((sdb->sdlc_state == SDLC_CONNECT) || 
		     (sdb->sdlc_state == SDLC_THEMBUSY)) ? 
		    SDLC_RR : SDLC_RNR), 
		   TRUE, idb, mod8_pak, sdb);
    }
    return FALSE;
}

static void tws_transmit(register hwidbtype *idb)
{
    int xmtaddr;

    if (!idb->sdlc_xmtptr || (idb->sdlc_data[idb->sdlc_xmtptr] == NULL))
	idb->sdlc_xmtptr = idb->sdlc_current_polled;

    if (idb->sdlc_xmtptr == idb->sdlc_current_polled) 
    	idb->sdlc_xmtptr = 
		 ((sdlc_data_block *)(idb->sdlc_data[idb->sdlc_current_polled]))->next;

    xmtaddr = idb->sdlc_xmtptr;

    while (1) {

	if ((xmtaddr == idb->sdlc_current_polled) && 
		    !(idb->sdlc_flags & SDLC_FLAG_TWS_FULL)) {
	    xmtaddr = 
		 ((sdlc_data_block *)(idb->sdlc_data[idb->sdlc_current_polled]))->next;
	    if ((xmtaddr == idb->sdlc_current_polled) || 
					(xmtaddr == idb->sdlc_xmtptr))
		break;
	}
        
	if (tws_send(idb, idb->sdlc_data[xmtaddr], FALSE))
	    break;

	xmtaddr = ((sdlc_data_block *) (idb->sdlc_data[xmtaddr]))->next;

	if (xmtaddr == idb->sdlc_xmtptr)
	    break;
    }

    idb->sdlc_xmtptr = xmtaddr;
}

/* Return TRUE, if we did not finish all the transimission before
 * final bit came back.
 */

static boolean tws_send_xmtleg(register hwidbtype *idb, sdlc_data_block *sdb, boolean setpoll)
{
    sdlc_data_block *oldsdb;
    int nr;
    sdlc_frametype *newframe;
    boolean poll = FALSE;
    register paktype *newpak = (paktype *)NULL;

    oldsdb = (sdlc_data_block *)idb->sdlc_data[idb->sdlc_current_polled];
    nr = sdb->sdlc_rvr;

    if (setpoll && ((1 << nr) & sdb->poll_seqmap)) {
	/*
	 * Catch special case where last few IFRAMEs
	 * in window could be lost
	 */
	if (nr != sdb->sdlc_vs)
	    ReTxFrameCntAndIFrameRetCnt(idb, sdb);

	while (nr != sdb->sdlc_vs) {
	    /* The polled station has returned the final bit, or timeout.
	     * Stop transmission and return.
	     */
	    newframe = (sdlc_frametype *)
		sdb->sdlc_paks[nr]->datagramstart;
	    newframe->control = SDLC_IFRAME | (nr << 1) |
		(sdb->sdlc_vr << 5);
	    pak_lock(sdb->sdlc_paks[nr]);

            if ((nr == (sdb->sdlc_vs - 1)) && QUEUEEMPTY(&sdb->sdlc_holdq))
                poll = TRUE;

            if(idb->sdlc_line_speed)
              serial_bcount += sdb->sdlc_paks[nr]->datagramsize;

	    sdlc_sendframe(SDLC_NOFRAME, (int)poll, idb,
			   sdb->sdlc_paks[nr], sdb);
	    nr = ++nr % SDLC_MODULO;
	}
    }    
    else {
        /*
         * If we are in full-dat mode then respond to the RR(F) with a RR(P),
         * so that the end station can send data if it has to, and we can
         * send the queued data after the poll.
         */
        if (setpoll && (idb->sdlc_flags & SDLC_FLAG_TWS_FULL)) {
            if (!QUEUEEMPTY(&sdb->sdlc_holdq))
                idb->sdlc_output_flag = 1;
            return FALSE;
        }
    }

    /* 
     * Return if we have done a poll here
     */
    if (setpoll && sdb->sdlc_poll) 
	return FALSE;
    
    while ((((sdb->sdlc_vs + SDLC_MODULO) - sdb->sdlc_rvr) % SDLC_MODULO) <
	idb->sdlc_k) {

	if (!setpoll && (!oldsdb->sdlc_poll || !SLEEPING(idb->sdlc_top_timer)))
	    return TRUE;

	if ((newpak = pak_dequeue(&sdb->sdlc_holdq)) == NULL)
		break;

 	/* if we need to do a poll, put the poll bit in the I
	 * frame when this I-frame is the last in the queue, or
	 * it is the last one in the window.
	 */
        if (setpoll && 
	   ((((sdb->sdlc_vs + SDLC_MODULO - sdb->sdlc_rvr) % SDLC_MODULO)
	      == idb->sdlc_k - 1) || QUEUEEMPTY(&sdb->sdlc_holdq))) {
	      poll = TRUE;
	}
	pak_lock(newpak);
    
        if(idb->sdlc_line_speed)
              serial_bcount += newpak->datagramsize;

	sdlc_sendframe(SDLC_IFRAME, (int)poll, idb, newpak, sdb);
	sdb->sdlc_seqmap |= (1 << sdb->sdlc_vs);
        if (poll)
            sdb->poll_seqmap = sdb->sdlc_seqmap;
	sdb->sdlc_paks[sdb->sdlc_vs] = newpak;
	sdb->sdlc_vs = ++sdb->sdlc_vs % SDLC_MODULO;
	if (setpoll && sdb->sdlc_poll)
	     break;
    }
    return FALSE;
}


/* 
 * SDLC subsystem header
 */

#define SDLC_MAJVERSION 1
#define SDLC_MINVERSION 0
#define SDLC_EDITVERSION  1

SUBSYS_HEADER(sdlc, SDLC_MAJVERSION, SDLC_MINVERSION, SDLC_EDITVERSION,
	      sdlc_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, netbios_acl, srb",
	      "req: netbios_acl, srb");


/*
 * SDLC Test Frame
 */


void sdlc_exec_test_command (parseinfo *csb)
{
    idbtype *swidb;
    hwidbtype *hwidb;
    sdlc_data_block *sdb;
    int sdlc_index;
    int str_len;

    swidb = GETOBJ(idb,1);
    hwidb = hwidb_or_null(swidb);
    if (hwidb->state != IDBS_UP) {
	printf("\nInterface is down");
	return;
    } else if (!is_sdlc(hwidb)) {
	printf("\nThe interface is not configured for SDLC");
        return;
    } else {
      sdlc_index = GETOBJ(int,1);
      if ((sdb = hwidb->sdlc_data[sdlc_index]) == NULL) {
	  printf("\nThe SDLC address %x has not been configured.", sdlc_index);
	  return;
      } 
    }	  
    switch (GETOBJ(int,2)) {
      case SDLC_TEST_ITERATIONS:
	  sdb->test_cnt = GETOBJ(int,3);
	  sdb->sdlctest_sendcnt = 0;
	  sdb->sdlctest_rcvcnt = 0;
	  break;
      case SDLC_TEST_CONTINUOUS:
	  sdb->test_cnt = 0xFF;
	  sdb->sdlctest_sendcnt = 0;
	  sdb->sdlctest_rcvcnt = 0;
	  break;
      case SDLC_TEST_STOP:
	  sdb->test_cnt = 0;
	  print_sdlc_test_results(sdb);
	  break;
    }
    if (GETOBJ(int,4)) {
	if ((str_len = strlen(GETOBJ(string,1))) > MAX_TEST_STRING) {
	    printf ("\n The text string will be truncated to 80 characters.");
	    str_len = MAX_TEST_STRING;
	}
	sstrncpy (sdb->test_string, GETOBJ(string, 1), str_len + 1);
    }
    else
	strcpy (sdb->test_string, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (sdb->test_cnt) {
	  if (!SLEEPING(hwidb->sdlc_top_timer)) {
	      hwidb->sdlc_current_polled = sdb->sdlc_address;
	      sdlc_settimer(hwidb, 0, sdb);
	  }
    }
}


void print_sdlc_test_results(sdlc_data_block *sdb)
{
    buginf ("\n SDLC Test for address %x completed", sdb->sdlc_address);
    buginf ("\n  Test frames sent      = %d", sdb->sdlctest_sendcnt);
    buginf ("\n  Test frames received  = %d \n", sdb->sdlctest_rcvcnt);
    flush ();
}

/*
 * sdlc_sendfrag()
 *
 * send a little fragment;
 * at this point the fragment is known to fit, and the sdb is also
 * known to be good
 */

void sdlc_sendfrag (sdlc_data_block *sdb, paktype *pak)
{
    sdlc_encapsulate(pak, pak->datagramsize, sdb);
    sdlc_holdq(pak->if_output->hwptr, sdb, pak);
}
 
 
/* This is called by qllc (lnx) or llc2 (sdllc) to send a packet to 
  * sdlc.
  */
 
static boolean sentBusy = FALSE;;
 
static boolean SdlcSend (sdlc_data_block *sdb)
{
    paktype *pak;
    hwidbtype* idb = sdb->sdlc_idb;
 
    while ((pak =  p_dequeue(&sdb->sdlcOutQ)) != NULL){
	int largest_frame;
 
        /* qllc and sdllc configured largest frame differently! */
        boolean qllc =  (pak->lnx_cb != NULL);
 
        largest_frame = qllc ? idb->sdlc_n1/8 : sdb->sdlc_largest_frame;
 
        /* Check if entire tr packet fits into an sdlc packet */
        if (pak->datagramsize <= largest_frame) {
	    sdlc_sendfrag(sdb, pak);
        } else {
	    /* No. We have to do some messy fragmentation */
            queuetype q;
	    if (sna_th_frag(pak, largest_frame, &q)){
		sendqueue(&q, (SendFragProc)sdlc_sendfrag, sdb);
	    }
	    else{
		p_requeue(&sdb->sdlcOutQ, pak);   /* put the queue back */
		sentBusy = FALSE;
		return FALSE;
	    }
        }
	/*
	 * Check if the high threshold value has been reached to quench ON 
	 * the llc2 side.
	 * Once sdlc starts draining the i-frames, it will quench OFF the
	 * llc2 side when the low water mark is reached in sdlc_dooutput().
	 */
	if (!qllc && !sdb->sdllc_local_busy &&
	    ((100 * sdb->sdlc_holdq.count)/sdb->sdlc_holdq.maximum >= SDLC_TXQ_HIWATER)) {
	    if (sdb->sdllc_lak_enable) {
		lak_t* lackie = sdb2lak(idb, sdb);
		if (lackie)
		    lak2_fsm(lackie, l2ma_dlc, l2mi_dlc_ind_txqhigh);
	    } else
		llc2_stateswitch(sdb->sdllc_llc, SET_LOCAL_BUSY);
	    sdb->sdllc_local_busy = TRUE;
        }
    }
    return TRUE;
}
 
void SdlcSendThruQ (sdlc_data_block *sdb, paktype *pak)
{
    p_enqueue(&sdb->sdlcOutQ, pak);
    if (sentBusy)
        sdlcFragmentData = TRUE;     /* wake up sdlc timer task to get buffer */
    else if (!SdlcSend(sdb))
        /* send data failure, this is caused by getbuffer during interrupt and we cannot get a
         * buffer. We force the timer process to wake up and process the getbuffer again
         */
        sdlcFragmentData = TRUE;     /* wake up sdlc timer task to get buffer */
}
 
static void SdlcSendCheck()
{
    hwidbtype* idb;
    sdlc_data_block* sdb;
    FOR_ALL_HWIDBS(idb) {
 	if (is_sdlc(idb)){
 	    int sdlcAddr;
 	    for (sdlcAddr = 0; sdlcAddr < 256; sdlcAddr++){
 		sdb = (sdlc_data_block *)idb->sdlc_data[sdlcAddr];
 		if (sdb && sdb->sdlcOutQ.qhead)
 		    SdlcSend(sdb);
 	    }
 	}
    }
}
 
 
/* Returns TRUE if sdb is inbound */
static boolean IsInbound(sdlc_data_block *sdb)
{
   if (!sdb)
       return(FALSE);
   if (sdb->sdlc_inbound)
       return(TRUE);
   if (sdb->sdlc_idb->sdlc_default_inbound)
       return(TRUE);
   return(FALSE);
}

/* Returns TRUE if sdb is outbound */
static boolean IsOutbound(sdlc_data_block *sdb)
{
   if (!sdb)
       return(FALSE);
   if (sdb->sdlc_outbound)
       return(TRUE);
   if (sdb->sdlc_idb->sdlc_default_outbound)
       return(TRUE);
   return(FALSE);
}

/* If Partner available : Returns TRUE */
static boolean IsPartnerCfgd(sdlc_data_block *sdb)
{
   if (!sdb)
        return(FALSE);
   if ((sdb->sdlc_have_prtnr) ||
       (sdb->sdlc_idb->sdlc_have_default_partner))
        return(TRUE);
   return(FALSE);
}

      
