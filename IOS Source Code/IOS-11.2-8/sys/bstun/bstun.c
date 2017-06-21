/* $Id: bstun.c,v 3.14.8.15 1996/09/10 08:48:29 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bstun.c,v $
 *------------------------------------------------------------------
 * bstun.c - BSTUN management/encapsulation routines
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun.c,v $
 * Revision 3.14.8.15  1996/09/10  08:48:29  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.14.8.14  1996/08/28  12:37:47  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.14.8.13  1996/08/23  06:04:44  jbalestr
 * CSCdi53010:  unconfigure all encaps bstun cause %SCHED-2-NOTWATCHTIMER
 * Branch: California_branch
 *
 * Revision 3.14.8.12  1996/08/07  08:56:52  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.14.8.11  1996/07/23  13:20:25  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.14.8.10  1996/07/17  00:00:28  snyder
 * CSCdi63174:  one blank == 116 bytes, another == 108 bytes
 * Branch: California_branch
 *              let the optimizer do its thing
 *
 * Revision 3.14.8.9  1996/07/06  05:51:58  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.14.8.8  1996/07/02  23:10:21  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.14.8.7  1996/06/27  09:23:27  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.14.8.6  1996/06/17  08:30:12  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.14.8.5  1996/05/22  17:17:15  fox
 * CSCdi57795:  Unable to change a BRI interface from ppp to hdlc.
 * Branch: California_branch
 * Don't clear idb->enctype in ppp_deencap(), slip_deencap() or
 * bstun_deencapsulate().  Move the setting of idb->encaptype to
 * the start of the encapsulation setup functions because calls to
 * other function use this value.
 *
 * Revision 3.14.8.4  1996/05/17  10:40:51  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.5  1996/05/05  22:49:37  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.11.2.4  1996/04/18  06:05:03  jbalestr
 * CSCdi54900:  router crashes when configure same bstun group on interface
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.3  1996/04/11  08:19:41  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.2  1996/04/03  13:50:29  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.2.1  1996/03/17  17:35:56  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.14.8.3  1996/04/03  05:25:32  arothwel
 * CSCdi48522:  change encap from bstun to hdlc does not delete bsc
 * control blocks
 * Branch: California_branch
 *
 * Revision 3.14.8.2  1996/03/23  01:29:30  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.14.8.1  1996/03/18  19:01:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.4  1996/03/16  06:29:01  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.3  1996/03/13  01:12:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.2  1996/03/07  08:39:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  00:19:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.14  1996/02/29  20:34:45  jbalestr
 * CSCdi48520:  BSTUN route command parses DIRECT
 * stops invalid configuration
 *
 * Revision 3.13  1996/02/28  23:04:27  jbalestr
 * CSCdi44604:  stuck timer without bstun keepalives
 *
 * Revision 3.12  1996/02/28  09:32:23  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.11  1996/02/15  09:01:57  smackie
 * Yet more comma operator wierdness thwarted. (CSCdi49069)
 *
 * Revision 3.10  1996/02/08  18:01:40  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.9  1996/02/07  16:10:18  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/02/01  06:00:14  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/16  03:37:12  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.6  1995/12/13  03:22:43  jbalestr
 * CSCdi45429:  bsc lack poller stops transmiting after shutdown/ no shut
 *
 * Revision 3.5  1995/12/07  05:27:16  jbalestr
 * CSCdi44604:  stuck timer without bstun keepalives
 *
 * Revision 3.4  1995/11/21  08:52:22  jbalestr
 * CSCdi42960:  bstun keepalive count
 * Added bstun keepalive interval and count
 *
 * Revision 3.3  1995/11/17  08:48:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:07:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:04:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.15  1995/10/04  05:16:15  jbalestr
 * CSCdi40970:  %ALIGN-3-CORRECT: Alignment correction made
 *
 * Revision 2.14  1995/09/09  00:51:04  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.13  1995/09/03  01:04:07  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.12  1995/08/25  11:41:03  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.11  1995/08/11 08:21:54  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.10  1995/08/08  16:45:49  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.9  1995/08/07 05:29:03  richl
 * CSCdi38315:  makefile rework
 *              move common peer code to libibm_util.a
 *
 * Revision 2.8  1995/08/01  06:28:42  arothwel
 * CSCdi38012:  Fix potential reference through null pointer in
 * valid_xxx().
 *
 * Revision 2.7  1995/07/28  23:15:00  ahh
 * CSCdi37876:  BSTUN: make sure args to TCP are initialized
 * Bug prevention -- bzero the block of args to tcpdriver_active_open
 * and tcpdriver_passive_open.
 *
 * Revision 2.6  1995/07/27  04:43:29  arothwel
 * CSCdi37743:  Make BSC and BSTUN debug output format similar.
 *
 * Revision 2.5  1995/07/26  06:17:49  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.4  1995/06/28  09:22:42  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/18  21:20:42  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/16  06:24:45  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "msg_bstun.c"			/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "subsys.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "mgd_timers.h"
#include "../os/signal.h"
#include "../ip/ip.h"
#include "../if/network.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "../if/priority.h"
#include "../util/random.h"
#include "parser_defs_bstun.h"
#include "../ui/debug.h"
#include "bstun.h"
#include "bstun_externs.h"
#include "bstun_debug.h"
#include "bstun_public.h"
#include "protocol_events.h"
#include "../ibm/libibm_externs.h"
#include "bsc_cmd_set.h"
#include "../os/free.h"
#include "../parser/parser_defs_parser.h"

static boolean bsttest_sender(bstunpeertype *inpeer, paktype *pak, hwidbtype *outidb,
		       boolean force, bstunpeertype **outpeer,
		       boolean usethispeer, boolean toallpp);
static void    bsttest_abort(bstunpeertype *stp);
static void    bsttest_close(bstunpeertype *stp, ulong param);
static bstunconnrtn bsttest_connect(bstunpeertype *stp);

/*---- Constants. */
#define NO_KILL_BSTUN_PROCS 0
#define KILL_BSTUN_PROCS 1

#define NO_CREATE_BSTUN_PROCS 0
#define CREATE_BSTUN_PROCS 1

/*
 * Global Storage
 */
queuetype  bstun_pinfoQ;
hwidbtype *bstun_first_idb;		/* first idb with bstun structs */
ipaddrtype bstun_name;			/* this peer's name */
watched_queue* bstconn_pakQ_p;
pid_t      bstun_pid;
static int        bstun_ifcnt = 0;
bstun_protocol_info *bstun_pinfo[BSTUN_MAX_GROUP + 1];
sys_timestamp bstun_start_time;

static ulong         bstun_keepalive_interval;  /* time to wait before timing out a peer */
static ulong         bstun_keepalive_count;     /* misses before death? */
static sys_timestamp bstun_next_keepalive;      /* wake up again for keepalive purposes */

/* 
 * Protocol state machine support. 
 */

watched_boolean* keepalive_cfg_request_p;
watched_queue*   bevent_pakQ_p;
pid_t      bevent_pid;
mgd_timer  bevent_timer;


mgd_timer*
bstun_get_bevent_timer (void)
{

   return(&bevent_timer);

}  /* bstun_get_bevent_timer. */



void
bstun_add_protocol (
    bstun_protocol_info *spi,
    char *name)
{
    bstun_protocol_list *spl;
    bstun_protocol_info *old;

    spl = (bstun_protocol_list *) find_namedthing(&bstun_pinfoQ, name);
    if (spl == NULL) {
	spl = malloc_named(sizeof(bstun_protocol_list), "bstun-protocol-list");
	if (spl && spi) {
	    sstrncpy(spl->name, name, NAMEDTHINGNAMELEN+1);
	    enqueue(&bstun_pinfoQ,  spl);
	    spl->q = spi;
            spl->next = NULL;
	    return;
	}
    } else {
	if (spi) {
	    old = (bstun_protocol_info *) spl->q;
	    spl->q = spi;
	    free( old);
	    return;
	}
    }
}


/*
 * bstun_deencapsulate
 * Take down the sub-ordinate protocol but don't change the encapsulation.
 * That will be done when the new encapsulation is set.
 */
static void
bstun_deencapsulate (hwidbtype* idb)
{

   bstun_shutinterface(idb, KILL_BSTUN_PROCS);

}

/*
 * bstun_parse_packet
 *
 * Set up a packet buffer so that the network level protocol fields
 * are initialized.
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 */

static iqueue_t
bstun_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    pak->enctype = idb->enctype;

    /*
     * If we're building a virgin frame, we have to set datagramstart
     */
    if (data) {
	pak->datagramstart = pak_center(pak);
    }
    pak->mac_start     = pak->datagramstart;
    pak->addr_start    = NULL;
    pak->rif_start     = NULL;
    pak->riflen	       = 0;
    pak->info_start    = NULL;
    pak->network_start = pak->datagramstart;

    /*
     * Um. I guess the encsize is zero. Or something.
     */
    pak->encsize = 0;
    return(bstun_input);
}

static boolean 
bstun_getlinestate (hwidbtype *idb)
{
    /*
     * ask the underlying protocol group if it is up or not
     */
    bstun_protocol_info *gpi;    

    if (idb && (idb->bstun_group != 0) 
            && ((gpi = bstun_pinfo[idb->bstun_group]) != NULL))
    {
        if (gpi->line_state) {
            return ((*gpi->line_state)(idb));
        }
    }
    return(FALSE);
}


/*
 * bstun_setup
 * Set up a serial interface to use BSTUN
 */
static boolean
bstun_setup (parseinfo *csb, hwidbtype *idb)
{
    int buffersize;
    idbtype *swidb;
    serialsb *ssb;

    idb->enctype = idb->enctype_def = idb->enctype_cfg = ET_BSTUN;
    idb->nokeepalive = TRUE;
    idb->vencap = (vencap_t)return_false;
    idb->deencap = bstun_deencapsulate;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    idb->getlink = hdlc_getlink;
    idb->oqueue = holdq_enqueue;
    idb->oqueue_dequeue = holdq_dequeue;
    idb->iqueue = bstun_input;
    idb->parse_packet = bstun_parse_packet;
    idb->show_support = NULL;
    idb->periodic = NULL;
    idb->extract_hwaddr = invalidate_hwaddr;
    idb->extract_snpa = invalidate_snpa;
    idb->broadcast = (broadcast_t) return_fuz_false;
    holdq_reset(idb);
    holdq_init(idb, HDLC_HOLDQ);
    idb->hdlc_lineup = TRUE;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    ssb->hdlc_myseq = 0;
    ssb->hdlc_mineseen = 0;
    idb->encsize = 0;
    idb->linestate = bstun_getlinestate;
    idb->lineaction = (lineaction_t)return_nothing;
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
        reg_invoke_media_enctype_setup(swidb, ET_NULL);
        swidb->cdp_enabled = FALSE;
	swidb->ip_enctype = ET_NULL;
    }
    idb->name = IFNAME_SERIAL;

    /*
     * Declare that the line is not in loopback.  HDLC is the only
     * protocol that can determine this.  Also prep the sent field so
     * that we don't compare 0 == 0 before we sent the first keepalive
     * packet.
     *
     * Moved this to here from setencap as setencap was not necessarily
     * called as HDLC is the default protocol so there may not be an
     * encapsulation hdlc command in the configuration.
     */
    ssb->hdlc_unique_sent = random_gen();
    idb->inloopback = FALSE;

    /*
     * Set up the maximum buffersize for this link (BSTUN gets a bit more...)
     */
    if (!idb->set_maxdgram) {
	printf("\nWarning: The maximum datagram size on interface %s will be %d bytes",
	       idb->hw_namestring, idb->maxmtu); 
    } else {
	buffersize = (idb->maxmtu > MAXDGRAM_BSTUN) ?
	  idb->maxmtu : MAXDGRAM_BSTUN;
	if (!(*idb->set_maxdgram)(idb, buffersize, (buffersize +
						    HDLC_HEADER_SIZE))) {
	    printf("\nWarning: The maximum datagram size on interface %s will be %d bytes",
			   idb->hw_namestring, idb->maxmtu);
	}
    }
    /* Clear out any info */
    bstun_shutinterface(idb, NO_KILL_BSTUN_PROCS);
    return(TRUE);
}

/*
 * bstun_show_priority_queue
 */
static boolean bstun_show_priority_queue (plisttype *plist,
                                          boolean custom,
                                          int i)
{
    char buff[BSTUN_MAX_PEER_NAME];
 
    if (plist->bstun_group) {
        printf("\n%5d  %6s protocol bstun %d ", i,
               queue2name(plist->priority, custom),
               plist->bstun_group);
        bstun_sprintf_addr(plist->bsc_addr, 0,
                          plist->bstun_group, buff);
        printf("%s", buff);
        return(TRUE);
    }
    return(FALSE);
}


/*
 * bstun_init
 * initialize BSTUN structures
 */

void
bstun_init (subsystype *subsys)
{
    GET_TIMESTAMP(bstun_start_time);
    queue_init(&bstun_pinfoQ, 0);
    memset(bstun_pinfo, 0, sizeof(bstun_protocol_info *[BSTUN_MAX_GROUP + 1]));
    bstconn_pakQ_p = create_watched_queue("Bstun-Queue", 0, 0);
    bevent_pakQ_p = create_watched_queue("Bevent-Queue", 0, 0);
    keepalive_cfg_request_p = create_watched_boolean("Bstun Keepalive cfg", 0);
    bstun_pid = 0;
    bstun_keepalive_interval = 0;
    bstun_keepalive_count = BSTUN_KEEPALIVE_DEFAULT_COUNT;

    if (system_loading)
	return;
    bstun_debug_init();

    /*
     * Register some functions
     */
    reg_add_raw_enqueue(LINK_BSTUN, bstif_input, "bstif_input");
    reg_add_onemin(bstun_periodic, "bstun_periodic");
    reg_add_media_serial_setup(ET_BSTUN, bstun_setup, "bstun_setup");
    reg_add_tcp_listen(BSTUN_P0_PORT, bsttcpd_listen, "bstun_tcp_listen");
    reg_add_tcp_listen(BSTUN_P1_PORT, bsttcpd_listen, "bstun_tcp_listen");
    reg_add_tcp_listen(BSTUN_P2_PORT, bsttcpd_listen, "bstun_tcp_listen");
    reg_add_tcp_listen(BSTUN_P3_PORT, bsttcpd_listen, "bstun_tcp_listen");
    reg_add_tcp_access_check(BSTUN_P0_PORT, bstun_active, "bstun_active");
    reg_add_tcp_access_check(BSTUN_P1_PORT, bstun_active, "bstun_active");
    reg_add_tcp_access_check(BSTUN_P2_PORT, bstun_active, "bstun_active");
    reg_add_tcp_access_check(BSTUN_P3_PORT, bstun_active, "bstun_active");
    reg_add_bstun_prioritization(bstun_prioritization, "bstun_prioritization");
    reg_add_priority_show(bstun_show_priority_queue,
                          "bstun_show_priority_queue");
    reg_add_priority_protocol_parse(LINK_BSTUN, prilist_protocol_bstun,
                                    "prilist_protocol_bstun");
    reg_add_priority_protocol_nv_add(LINK_BSTUN,
                                     priority_protocol_bstun_nv_add,
                                     "priority_protocol_bstun_nv_add");
    reg_add_priority_protocol_compare(LINK_BSTUN,
                                      priority_protocol_bstun_compare,
                                      "priority_protocol_bstun_compare");
}


void
bstun_sprintf_addr (uchar *addr, boolean all_addr, uchar group, char *buff)
{
    ulong naddr;
    char *buffp;
    int i, shift;

    if (all_addr) {
	sprintf(buff, "all");
	return;
    }
    if (bstun_pinfo[group]->addrfmt != 'x') {
	for (naddr = 0, i = bstun_pinfo[group]->addrlength - 1, shift = 0;
	     i >= 0; i--, shift += 8)
	  naddr |= addr[i] << shift;
	sprintf(buff, ((bstun_pinfo[group]->addrfmt == 'd') ? "%u" : "%o"),
		naddr);
	return;
    }
    for (i = 0, buffp = buff; i < bstun_pinfo[group]->addrlength;
	 i++, buffp += 2)
      sprintf(buffp, ((i == 0) ? "%x" : "%02x "), addr[i]);
}

char *
bstpeer_id (bstunpeertype *stp, char *buff)
{
    char addrbuff[80];
    char addrbuff2[80];

    if (!stp) {
	buff[0] = '\0';
	return(buff);
    }
    bstun_sprintf_addr(stp->addr, stp->all_addr, stp->group, addrbuff2);
    sprintf(addrbuff, "%s[%u]", addrbuff2, stp->group);
    switch (stp->type) {
      case BSTUN_PT_NONE:
      case BSTUN_PT_TCPD:
	sprintf(buff, "(%s)%i/%d", addrbuff, stp->ipaddr, stp->port);
	break;
      case BSTUN_PT_IF:
	sprintf(buff, "(%s)%s", addrbuff, stp->outidb->hw_namestring);
	break;
      default:
	break;
    }
    return(buff);
}

char *
bstpeer_state (bstunpeertype *stp)
{
    switch (stp->state) {
      case BSTUN_PS_DEAD:	return("dead");
      case BSTUN_PS_CLOSED:	return("closed");
      case BSTUN_PS_OPENING:	return("opening");
      case BSTUN_PS_OPENWAIT:	return("open wait");
      case BSTUN_PS_CONNECTED:	return("open");
      default:			return("unknown");
    }
}

static char *
bst_state (
    bstunstatetype state)
{
    switch (state) {
      case BSTUN_PS_DEAD:	return("dead");
      case BSTUN_PS_CLOSED:	return("closed");
      case BSTUN_PS_OPENING:	return("opening");
      case BSTUN_PS_OPENWAIT:	return("open wait");
      case BSTUN_PS_CONNECTED:	return("open");
      default:			return("unknown");
    }
}

/*
 * bstif_findpeer
 * Given various parameters find the first peer structure
 * we can for a given address.  If not found on the last
 * stif idb pointer's queue try on any stif idb.  If found
 * then update this idb's last stif pointer.
 */

bstunpeertype *
bstif_findpeer (uchar group, uchar *addrp, hwidbtype *in_idb)
{
    bstunpeertype *stp;
    hwidbtype *idb;
    bstun_protocol_info *gpi;    

    idb = in_idb ? in_idb->bstun_if_last : in_idb;

    if ((idb) && ((gpi = bstun_pinfo[idb->bstun_group]) != NULL))
      for (stp = (bstunpeertype *) idb->bstun_Q.qhead; stp; stp =(bstunpeertype *) stp->next) {
	  if (stp->type != BSTUN_PT_IF || stp->outidb != in_idb)
	    continue;
	  if (stp->group == group) {
	    if (stp->all_addr) {
	      return(stp);
	    }
	    if (stp->input_addr_inuse) {
	      if (!bcmp(addrp, stp->input_addr, gpi->addrlength))
		return(stp);
	    } else {
	      if (!bcmp(addrp, stp->addr, gpi->addrlength))
		return(stp);
	    }
	  }
      }

    /*
     * not found on last recorded structure.  Search the whole list
     * of idbs.  Only match on one that points at this interface.
     */
    for (idb = bstun_first_idb; idb; idb = IDB_NEXT(idb)) {
	if (!idb->bstun_Q.qhead)
	    continue;
	if ((gpi = bstun_pinfo[idb->bstun_group]) == NULL)
	  continue;
	for (stp = (bstunpeertype *) idb->bstun_Q.qhead; stp; stp = (bstunpeertype *)stp->next) {
	    if (stp->type != BSTUN_PT_IF || stp->outidb != in_idb)
		continue;
	    if (stp->group == group && (stp->all_addr ||
					!bcmp(addrp, stp->addr,
					      gpi->addrlength))) {
		in_idb->bstun_if_last = idb;
		return(stp);
	    }
	}
    }
    return(NULL);
}

/*
 * bsttcp_findpeer
 * Find a tcp peer.  Either TCP driver or TCP Packet will do.
 */

bstunpeertype *
bsttcp_findpeer (uchar group, uchar *addrp, ipaddrtype ipaddr,
			      int lport, int fport)
{
  bstunpeertype *stp;
  hwidbtype *idb;
  bstun_protocol_info *gpi;
  boolean found = FALSE;

  for (idb = bstun_first_idb; idb; idb = IDB_NEXT(idb)) {
    if (!idb->bstun_Q.qhead)
      continue;
    if ((gpi = bstun_pinfo[idb->bstun_group]) == NULL)
      continue;
    for (stp = (bstunpeertype *) idb->bstun_Q.qhead; stp; stp = (bstunpeertype *)stp->next) {
      if ((stp->type != BSTUN_PT_TCPD) || (stp->ipaddr != ipaddr))
	continue;
      if (stp->group == group) {
	if (stp->all_addr) {
	  found = TRUE;
	}
	if (stp->input_addr_inuse) {
	  if (!bcmp(addrp, stp->input_addr, gpi->addrlength))
	    found = TRUE;
	} else {
	  if (!bcmp(addrp, stp->addr, gpi->addrlength))
	    found = TRUE;
	}
      }
      if (!found)
	continue;
      switch (lport) {
      case 0:
      case BSTUN_P0_PORT:
	return(stp);
      case BSTUN_P1_PORT:
        return(stp->priority_peers[1]);
      case BSTUN_P2_PORT:
        return(stp->priority_peers[2]);
      case BSTUN_P3_PORT:
        return(stp->priority_peers[3]);
      default:
	switch (fport) {
	case 0:
	case BSTUN_P0_PORT:
	  return(stp);
        case BSTUN_P1_PORT:
          return(stp->priority_peers[1]);
        case BSTUN_P2_PORT:
          return(stp->priority_peers[2]);
        case BSTUN_P3_PORT:
          return(stp->priority_peers[3]);
	default:
	  return(NULL);
	}
      }
    }
  }
  return(NULL);
}

/*
 * bstpak
 * get a packet for sending bstun protocol information
 */

paktype *
bstpak (ushort op, uchar group, uchar *addrp, ushort len)
{
    paktype *pak;
    bstunhdrtype *sth;
    bstun_protocol_info *gpi;
    int addrlen;

    if ((op != BSTUN_OP_VREP) && (op != BSTUN_OP_VREQ)) {
	if ((gpi = bstun_pinfo[group]) == NULL)
	    return(NULL);
	addrlen = gpi->addrlength;
    } else {
	addrlen = 0;
    }

    pak = getbuffer(sizeof(bstunhdrtype) + len + addrlen - 1);
    if (!pak)
	return(NULL);
    sth = (bstunhdrtype *)pak->network_start;
    sth->magic = BSTUN_MAGIC;
    sth->op = op;
    sth->group = group;
    if (addrlen > 0)
	bcopy(addrp, &(sth->addr[0]), addrlen);
    sth->len = len;
    pak->datagramstart = (uchar *) sth;

     if ((op == BSTUN_OP_VREP) || (op == BSTUN_OP_VREQ))
       pak->datagramsize = sizeof(bstunhdrtype);
    else
       pak->datagramsize = sizeof(bstunhdrtype) + len + addrlen - 1;
 
    return(pak);
}

/*
 * bst_op2str
 * convert a bstun op code into humanspeak
 */

char *
bst_op2str (ushort op)
{
    switch (op) {
      case BSTUN_OP_PASS:          return("Pass");
      case BSTUN_OP_NOP:           return("NOp");
      case BSTUN_OP_IGNORE:        return("Ignore");
      case BSTUN_OP_EMPTY:         return("Empty");
      case BSTUN_OP_VREQ:          return("Version Request");
      case BSTUN_OP_VREP:          return("Version Reply");

      case BSTUN_OP_BADOP:         return("Bad Op");
      case BSTUN_OP_UNKPEER:       return("Unknown Peer");
      case BSTUN_OP_UNKADDR:       return("Unknown Address");
      default:                    return("Unknown");
    }
}

/*
 * bstnail_peer
 * Actually remove a peer and associated structures
 */

static void
bstnail_peer (
    bstunpeertype *stp)
{
    leveltype status;
    char peerid[BSTUN_MAX_PEER_NAME];
    bstunpeertype *stptop;
    int i;

    if (bstun_debug)
	errmsg(&msgsym(PEERSHUTDOWN, BSTUN), bstpeer_id(stp, peerid),
	       stp->bstun_idb->hw_namestring);
    status = raise_interrupt_level(NETS_DISABLE);
    /* Be clean and mark the entry closed before deleting it */
    bstun_newstate(stp, BSTUN_PS_CLOSED);
    if (stp->type == BSTUN_PT_IF)
	stp->outidb->bstun_if_last = NULL;
    if (stp->abort)
	(*stp->abort)(stp);
    if (stp->priority_top) {
        stptop = stp->priority_top;
        for (i = 1; i <= BSTUN_PRIORITY_COUNT; i++) {
            if (stptop->priority_peers[i] == stp) {
                stptop->priority_peers[i] = NULL;
                break;
            }
        }
    }
    if (stp->all_addr)
       free(stp->arsl_p);
    free(stp);
    reset_interrupt_level(status);

}  /* bstnail_peer. */


static inline void
bstun_print_frame (
    paktype *pak,
    int group,
    char *prefix,
    hwidbtype *idb,
    void (*routine)(paktype *, int, char *, hwidbtype *))
{
    if (bstun_debug_packet)
      (*routine)(pak, group, prefix, idb);
}

/*
 * bstun_findpeer
 * Returns a ptr to the bstunpeertype structure, based on the address
 * supplied in the frame. Assumes gpi is valid
 */
bstunpeertype *
bstun_findpeer (hwidbtype* idb, paktype* pak)
{
  bstunpeertype *stp ;
  uchar *addrstart;
  bstun_protocol_info *gpi = bstun_pinfo[idb->bstun_group];

  addrstart = (uchar *) pak->datagramstart + gpi->addroffset;
  for (stp = (bstunpeertype *) idb->bstun_Q.qhead; stp;
       (bstunpeertype *)stp = stp->next) {
      if (stp->all_addr)
         break;
      if (stp->input_addr_inuse &&
         (!bcmp(addrstart, stp->input_addr, gpi->addrlength)))
         break;
      else
         if (!bcmp(addrstart, stp->addr, gpi->addrlength))
            break;    
  }
  return stp;

}  /* bstun_findpeer. */

/*
 * bstun_input
 * Handle the raw serial side of a serial tunnel connection.
 * This is the bstun side.
 */

void
bstun_input (hwidbtype *idb, paktype *pak)
{
    bstun_protocol_info *gpi;

    idb_increment_rx_proto_counts(pak, ACCT_PROTO_BSTUN);
    pak->acct_proto = ACCT_PROTO_BSTUN;

    if ((idb->state != IDBS_UP) ||
	((gpi = bstun_pinfo[idb->bstun_group]) == NULL)) {
	datagram_done(pak);
	return;
    }

    (void) (*gpi->hardware_forward_hook)(idb, pak);

}

void
bstun_to_network (hwidbtype *idb,
                  paktype *pak,
		  int pak_opcode)
{
    bstun_protocol_info *gpi;
    bstunhdrtype *sth;
    bstunpeertype *stp;
    char* addrstart;

    gpi = bstun_pinfo[idb->bstun_group];
    stp = bstun_findpeer(idb, pak);
    if (stp == NULL) {
       if (gpi)
           gpi->groupUnroutableRx++;
       datagram_done(pak);
       return;
    }

    bstun_print_frame(pak, stp->group, "SDI:", idb, gpi->print_frame);

    addrstart = (uchar *) pak->datagramstart + gpi->addroffset;
    pak->datagramstart -= sizeof(bstunhdrtype) + gpi->addrlength - 1;
    sth = (bstunhdrtype *) pak->network_start = pak->datagramstart;
    PUTSHORT(&sth->magic,BSTUN_MAGIC);
    PUTSHORT(&sth->op,pak_opcode);
    sth->group = stp->group;
    bcopy(addrstart, &(sth->addr[0]), gpi->addrlength);
    PUTSHORT(&sth->len,pak->datagramsize);
    pak->datagramsize += sizeof(bstunhdrtype) + gpi->addrlength - 1;
    (void) (*stp->sender)(stp, pak, NULL, FALSE, NULL, FALSE, FALSE);

}  /* bstun_to_network. */


/********************************************************************
 *
 * Bstunnel Background
 *
 ********************************************************************/

static boolean
bstun_tcp_correct_port (
    bstunpeertype *stp)
{
    if (!stp->priority_top) {
        return(stp->port == BSTUN_P0_PORT);
    } else {
        if (stp->priority_top->priority_peers[1] == stp)
            return(stp->port == BSTUN_P1_PORT);
        else if (stp->priority_top->priority_peers[2] == stp)
            return(stp->port == BSTUN_P2_PORT);
        else if (stp->priority_top->priority_peers[3] == stp)
            return(stp->port == BSTUN_P3_PORT);
    }
    return(FALSE);
}

boolean
bstcheck_conn_state (bstunpeertype *stp)
{
    boolean bad;
    char peerid[BSTUN_MAX_PEER_NAME];

    bad = FALSE;
    switch (stp->type) {
      case BSTUN_PT_TCPD:
	if (!stp->ipaddr || !(bstun_tcp_correct_port(stp)))
	    bad = TRUE;
	break;

      case BSTUN_PT_IF:
	if (!stp->outidb)
	    bad = TRUE;
	break;

      case BSTUN_PT_TEST:
        break;

      default:
	bad = TRUE;
	break;
    }
    if (bad || !stp->sender || !stp->connect || !stp->bstun_idb) {
	if (bstun_debug)
	    errmsg(&msgsym(BADCONN, BSTUN), 1, bstpeer_id(stp, peerid));
	return(FALSE);
    }
    return(TRUE);
}

/*
 * bstun_send_keepalive
 *
 * send a keepalive down to a remote peer.
 */
static void bstun_send_keepalive (bstunpeertype *stp)
{
    paktype *pak;
    char peerid[BSTUN_MAX_PEER_NAME];
    bstunhdrtype *sth;
 
    if (stp == NULL) {
	errmsg(&msgsym(NOBUF, BSTUN), "BSTUN", "send peer keepalive");
        if (bstun_debug) {
            buginf("\nBSTUN: NULL peer pointer passed to bstun_send_keepalive");
        }
        return;
    }
    pak = bstpak(BSTUN_OP_VREQ, stp->group, &(stp->addr[0]), 0);
    if (pak == NULL) {
        if (bstun_debug) {
            buginf("\nBSTUN: Couldn't get keepalive packet to send to peer %s.",
                   bstpeer_id(stp, peerid));
        }
        return;
    }
    /* we want to mak use of the Version Request to check the bstun
       peer state.  To make sure the bstun peer states are in sync
       the address field in a VREQ or VREP contains the state.
    */
    if (bstun_pinfo[stp->group]->local_ack_capable) {
	sth = (bstunhdrtype *)pak->network_start;
	sth->addr[0] = (char)stp->state;
    }

    if (!((*stp->sender)(stp, pak, NULL, FALSE, NULL, TRUE, FALSE))) {
        if (bstun_debug) {
            buginf("\nBSTUN: Couldn't send keepalive packet to peer %s.",
                   bstpeer_id(stp, peerid));
        }
    }
}

/*
 * bstun_abort_tcp_connection
 *
 *  aborts all the connections for the stp
 */
static void
bstun_abort_tcp_connection (bstunpeertype *stp)
{
    /*
     * Abort the TCP connection. If priority is configured
     * then abort all the connections.
     */
     char peerid[BSTUN_MAX_PEER_NAME];
     if (stp->priority_top){
         stp = stp->priority_top;
     }
     if (stp->priority) {
         int i;
         for (i = 1; i <= BSTUN_PRIORITY_COUNT; i++) {
             if (bstun_debug){
                 buginf("\nBSTUN: Keepalive failed: tearing down peer %s at %d ",
                        bstpeer_id(stp->priority_peers[i], peerid),
                        ELAPSED_TIME(bstun_start_time)/ONESEC);
             }
             if (stp->priority_peers[i]->abort){
                 (*stp->priority_peers[i]->abort)(stp->priority_peers[i]);
             }
         }
     }
     if (bstun_debug){
         buginf("\nBSTUN: Keepalive failed: tearing down peer %s at %d ",
                bstpeer_id(stp, peerid),
                ELAPSED_TIME(bstun_start_time)/ONESEC);
     }
     if (stp->abort){
         (*stp->abort)(stp);
     }
}

static void
bstun_exit_handler (int signal,
                    int subcode,
                    void *info,
                    char *addr)
{
    paktype* pak;

    if (bstun_debug)
       errmsg(&msgsym(PROCEXIT, BSTUN), signal, subcode, addr);
    /*
     * stop the process timer for keepalives
     */
    if (TIMER_RUNNING(bstun_next_keepalive)) {
        process_watch_timer(&bstun_next_keepalive, DISABLE);
        TIMER_STOP(bstun_next_keepalive);
    }
    process_watch_queue(bstconn_pakQ_p, DISABLE, ONE_SHOT);
    process_watch_boolean(keepalive_cfg_request_p, DISABLE, ONE_SHOT);
    while ((pak = process_dequeue(bstconn_pakQ_p)) != NULL){
        datagram_done(pak);
    }

}  /* bstun_exit_handler. */


/*
 * bstun_background
 * background process for bstun functions.
 * Handle making connections to peers.  The packet that caused
 * the peer to be opened is what is enqueued.  The packet has
 * a pointer to the the bstun structure that needs the open.
 *
 * This routine will also send periodic keepalives to check for link
 * or remote router failure. This is very important to do especially
 * in a local ack environment. Also, check for keepalive failure and if
 * so, tear the peer down and any associated local ack sessions.
 */

void
bstun_background (void)
{
    bstunpeertype *stp;
    paktype *pak, *pumppak;
    sys_timestamp time;
    char peerid[BSTUN_MAX_PEER_NAME];
    bstun_protocol_info *gpi;
    hwidbtype *idb;
    ulong major, minor;

    signal_oneshot(SIGEXIT, bstun_exit_handler);

    /*
     * watch for amy traffic on the connection
     */
    process_watch_queue(bstconn_pakQ_p, ENABLE, RECURRING);
    process_watch_boolean(keepalive_cfg_request_p, ENABLE, RECURRING);

    for(;;) {
       process_wait_for_event();
       while (process_get_wakeup(&major, &minor)) {
          switch (major) {
             case TIMER_EVENT :
                if (bstun_keepalive_interval && AWAKE(bstun_next_keepalive)) {
                   TIMER_START(bstun_next_keepalive,bstun_keepalive_interval+1);
                   for(idb = bstun_first_idb; idb; idb = idb->next) {
                      if (!idb->bstun_Q.qhead)
                         continue;
                      if ((gpi = bstun_pinfo[idb->bstun_group]) == NULL)
                         continue;
                      for (stp = idb->bstun_Q.qhead; stp; stp = stp->next) {
                         if ((stp->state == BSTUN_PS_CONNECTED) &&
                            (bstun_pinfo[stp->group]->local_ack_capable)) {
                            COPY_TIMESTAMP(stp->last_keepalive, time);
                            TIMER_UPDATE(time, bstun_keepalive_interval);
                            if (AWAKE(time)){
                               bstun_send_keepalive(stp);
                            }
                            TIMER_UPDATE(time, (bstun_keepalive_count - 1) *
		                         bstun_keepalive_interval);
                            if (AWAKE(time)) {
                               bstun_abort_tcp_connection(stp);
                            }
                         }
                      }
                   }
                }
                break;
	     case QUEUE_EVENT:
	        while ((pak = process_dequeue(bstconn_pakQ_p))) {
                   stp = (bstunpeertype *) pak->peer_ptr;
	           if (!bstcheck_conn_state(stp)) {
		      datagram_done(pak);		/* you're history */
		      if (bstun_debug && stp)
		         errmsg(&msgsym(BADCONN, BSTUN), 2, bstpeer_id(stp, peerid));
		      continue;
	           }
	           if(stp->state == BSTUN_PS_OPENING) {
		      bstun_newstate(stp, BSTUN_PS_OPENWAIT);
		      switch ((*stp->connect)(stp)) {
		        case BSTUN_CONN_ABORTED:
		           datagram_done(pak);	/* you're history */
		           continue;
		        case BSTUN_CONN_FAILED:
		           bstun_newstate(stp, BSTUN_PS_DEAD);
		           stp->primary_t = NULL;
    		           stp->secondary_t = NULL;
		           datagram_done(pak);		/* you're history */
		           continue;
		        case BSTUN_CONN_CONNECTED:
		           if (bstun_debug)
			      errmsg(&msgsym(OPENED, BSTUN), "CONN",
			              bstpeer_id(stp, peerid), bstpeer_state(stp));
		           bstun_newstate(stp, BSTUN_PS_CONNECTED);
		           /* Send a "pump packet" to get the other, passive end, open */
		           pumppak = bstpak(BSTUN_OP_IGNORE, stp->group, &(stp->addr[0]), 30);
		           (void) (*stp->sender)(stp, pumppak, NULL, FALSE, NULL, FALSE, FALSE);
		           (void) (*stp->sender)(stp, pak, NULL, FALSE, NULL, TRUE, FALSE);
		           break;
                      }
		   }
                   else{
		      errmsg(&msgsym(PEERSTATE, BSTUN),
		             bstpeer_id(stp, peerid), stp->state, 1);
		      datagram_done(pak);		/* you're history */
	           }
	        } /* while pak */
	        break;		
             case BOOLEAN_EVENT :
                /*---- Watch for keepalive configuration events 
                       if bstun_keepalive_interval is set then start timer */
                if (process_get_boolean(keepalive_cfg_request_p)) {
                    process_set_boolean(keepalive_cfg_request_p, FALSE);
                   if (bstun_keepalive_interval) {
                       process_watch_timer(&bstun_next_keepalive, ENABLE);
                       TIMER_START(bstun_next_keepalive, BSTUN_KEEPALIVE_TX_PERIOD);
                   } else {
                      if (TIMER_RUNNING(bstun_next_keepalive)) {
                          process_watch_timer(&bstun_next_keepalive, DISABLE);
                          TIMER_STOP(bstun_next_keepalive);
                      }
                   }
                }
                break;
	     default:
	        errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	} 
    } /* for */
}  /* bstun_background. */



/********************************************************************
 *
 * BSTUN Protocal State Machine Support
 *
 ********************************************************************/

/*
 * bevent_background
 *
 * Dequeue all packets on the event queue and forward to protocol fsm.
 * Check if master timer expired and pass timeout packet to fsm.
 * Always check if the sched needs to run.
 */

static inline boolean
bevent_valid_idb (hwidbtype* idb,
		  paktype* pak)
{
   boolean idb_ok = TRUE;

   if (!idb) {
      if (bstun_debug)
         buginf("\nBEVENT - pak has NULL idb");
      datagram_done(pak);
      idb_ok = FALSE;
   }
   else
   if (!(idb->poll_protocol_flags & PP_ACTIVE)) {
      datagram_done(pak);
      idb_ok = FALSE;
   }

   return(idb_ok);

}  /* valid_idb. */


static void
bevent_exec_protocol(paktype* pak,
		     hwidbtype* idb)
{
   bstun_protocol_info* gpi;
   ushort protocol_event;

   gpi = bstun_pinfo[idb->bstun_group];
   if (!gpi) {
      if (bstun_debug)
         errmsg(&msgsym(NOGRPDEF, BSTUN), idb->hw_namestring);
      datagram_done(pak);
   }
   else {
      protocol_event = get_major_protocol_event(pak);
      switch(protocol_event) {
      case PE_HARDWARE:                    /* Line shipped us something. */
         (*gpi->hardware_receive_hook)(pak, idb);
         break;
      case PE_NETWORK:                     /* Upper layer shipped us something. */
         (*gpi->network_receive_hook)(pak, idb);
         break;
      case PE_TIMER:                       /* A timer expired. */
         (*gpi->timer_receive_hook)(pak, idb);
         break;
      default:                             /* Ignore unknown events. */
         if (bstun_debug)
            errmsg(&msgsym(UNKPROTOEV, BSTUN), protocol_event, idb->hw_namestring);
         datagram_done(pak); 
         break;
      }
   }

}  /* bstun_exec_protocol. */


static void
bevent_exit_handler (int signal,
                     int subcode,
                     void *info,
                     char *addr)
{
    paktype* pak;

    if (bstun_debug)
       errmsg(&msgsym(PROCEXIT, BSTUN), signal, subcode, addr);
    process_watch_mgd_timer(&bevent_timer, DISABLE);
    process_watch_queue(bevent_pakQ_p, DISABLE, ONE_SHOT);
    while ((pak = process_dequeue(bevent_pakQ_p)) != NULL)
        datagram_done(pak);

}  /* bevent_exit_handler. */

void
bevent_background (void)
{
    hwidbtype* idb;
    paktype* pak;
    mgd_timer* expired_timer;

    signal_oneshot(SIGEXIT, bevent_exit_handler);
    process_watch_queue(bevent_pakQ_p, ENABLE, RECURRING);
    process_watch_mgd_timer(&bevent_timer, ENABLE);

    while (TRUE) {
        process_wait_for_event();

        /*---- Drain bstun event queue. */
        while((pak = process_dequeue(bevent_pakQ_p))) {

            /*---- Need to distinguish if_input and if_output. We're only
                   ever interested in the idb on which bstun and the
                   poll protocol are configured, _not_ the network side. */

            idb = (pak->protocol_event.major == PE_NETWORK) ?
                  pak->if_output->hwptr : pak->if_input->hwptr;
            if (!bevent_valid_idb(idb, pak))
               continue;
            bevent_exec_protocol(pak, idb);
        }

        /*---- Check for expired timers. */  
        while (mgd_timer_expired(&bevent_timer)) {
            expired_timer = mgd_timer_first_expired(&bevent_timer);

            /*---- Timer context is created w/ input idb. */
            pak = mgd_timer_context(expired_timer);
            idb = pak->if_input->hwptr;
            bevent_exec_protocol(pak, idb);
        }
    }

}  /* bevent_background. */


/*
 * bevent_network
 * 
 * Received stripped network packet.
 * Configure packet event header and put on the event queue.
 * The packet is consumed by the protocol fsm.
 */

boolean bevent_network (hwidbtype *idb, paktype *pak) 
{

    set_major_protocol_event(pak, PE_NETWORK);
    process_enqueue(bevent_pakQ_p, pak);
    return(TRUE);

}  /* bevent_network. */

/*
 * bevent_hardware
 * 
 * Received raw interface packet.
 * Configure packet event header and put on the event queue.
 * The packet is consumed by the protocol fsm.
 */

ushort bevent_hardware (hwidbtype *idb, paktype *pak)
{

    set_major_protocol_event(pak, PE_HARDWARE);
    process_enqueue(bevent_pakQ_p, pak);
    return(BSTUN_OP_IGNORE);

}  /* bevent_hardware. */


/********************************************************************
 *
 * Raw Interface: BSTUN Implementation Support
 *
 ********************************************************************/

static void
bstif_err (
    bstunpeertype *stp,
    hwidbtype *idb,
    bstunhdrtype *sth,
    char *label)
{
    char peerid[BSTUN_MAX_PEER_NAME];

    if (stp)
	idb = stp->outidb;
    if (bstun_debug)
	errmsg(&msgsym(ERR, BSTUN), label, bst_op2str(sth->op),
	       bstpeer_id(stp, peerid), sth->op, sth->len);
    if (!stp)
	return;
    switch (sth->op) {
      case BSTUN_OP_UNKPEER:		/* about what we are sending */
      case BSTUN_OP_UNKADDR:
	bstun_newstate(stp, BSTUN_PS_DEAD);
	break;
      default:
      case BSTUN_OP_BADOP:
	break;
    }
}

static void
bstif_send_punt (
    bstunpeertype *stp,
    hwidbtype *idb,
    ushort op,
    bstunhdrtype *sth,
    char *label)
{
    paktype *pak;
    char peerid[BSTUN_MAX_PEER_NAME];

    pak = bstpak(op, sth->group, &(sth->addr[0]), 0);
    if (stp) {
	idb = stp->outidb;
	stp->drops++;
    }
    if (pak && !stp) {
	/*
	 * go direct.  We don't know which peer it is.  But
	 * we do know the interface.
	 */
	pak->if_output = idb->firstsw;
	pak->linktype = LINK_BSTUN;
	pak->acct_proto = ACCT_PROTO_BSTUN;
	if (idb->vencap && (*idb->vencap)(pak, 1)) {
	    datagram_out(pak);
	    if (bstun_debug)
		errmsg(&msgsym(SENDPUNT, BSTUN), label, bst_op2str(op),
		       bstpeer_id(stp, peerid));
	    return;
	}
	datagram_done(pak);
	return;
    }
    if (pak && bstif_direct(stp, pak, NULL, TRUE, NULL, FALSE, FALSE)) {
	if (bstun_debug)
	    errmsg(&msgsym(SENDPUNT, BSTUN), label, bst_op2str(op),
		   bstpeer_id(stp, peerid));
	return;
    }
    if (bstun_debug)
	buginf("\nBSTUN: %s: send punt, couldn't send %s %s to %s",
	       label, (pak ? " (no pak)" : ""), bst_op2str(op),
	       bstpeer_id(stp, peerid));
}

/*
 * bstun_fromnet_forward
 * This routine handles the proxy state machine for network side input form
 * any type (across a serial link using the special serial transport, or TCP).
 */
static void
bstun_fromnet_forward (
    paktype *pak,
    bstunpeertype *stp,
    ushort opcode)
{
    boolean forwarded;

    hwidbtype *idb = hwidb_or_null(pak->if_output);
    bstun_print_frame(pak, stp->group, "NDI:",
		     idb,
		     bstun_pinfo[stp->group]->print_frame);

    /* Hand pak to protocol fsm if configured. */
    forwarded = (*bstun_pinfo[stp->group]->network_forward_hook)(idb, pak);
    if (!forwarded)
       datagram_done(pak);              /* The net_forward_hook *must* be configured! */

}  /* bstun_fromnet_forward. */

/*
 * bstif_input
 * process direct input from an interface.  Network side input.
 */

void
bstif_input (paktype *pak)
{
   bstunpeertype *stp;
    bstunhdrtype *sth;
    hwidbtype *idb;
    ushort opcode;

    sth = (bstunhdrtype *) pak->network_start;

    if (bstun_debug) {
      if (sth->op != BSTUN_OP_PASS) {
        buginf("\nBSTUN: Received %s opcode on %s",
               bst_op2str(sth->op), pak->if_input->namestring);
      }
    }
 
    /* Check version number and reply with our own */
    if ((sth->op == BSTUN_OP_VREQ) ||
	(sth->op == BSTUN_OP_VREP)) {
	if (bstun_debug)
  	    buginf("\nBSTUN: version %x from remote on %s. Reply %x.",
		   sth->magic, pak->if_input->namestring, BSTUN_MAGIC);
	if (sth->op == BSTUN_OP_VREQ) {
	    sth->op = BSTUN_OP_VREP;
	    sth->magic = BSTUN_MAGIC;
	    bstif_direct(NULL, pak, pak->if_input->hwptr, TRUE, NULL, FALSE, FALSE);
	}
        else
          datagram_done(pak);
	return;
    }
    if (sth->magic != BSTUN_MAGIC) {
	if (bstun_debug)
	    errmsg(&msgsym(BADMAGIC, BSTUN), "IFin", BSTUN_MAGIC, sth->magic, 1);
	datagram_done(pak);
	return;
    }
    idb = pak->if_input->hwptr;
    stp = bstif_findpeer(sth->group, &(sth->addr[0]), idb);
 
    /* Any data at all is good enough for a keepalive */
    if (stp)
	GET_TIMESTAMP(stp->last_keepalive);

    opcode = sth->op;
    /*
     * We don't check for validity of stp until later.
     */	
    if (opcode & BSTUN_OP_ERR) {
	bstif_err(stp, idb, sth, "IFin");
	datagram_done(pak);
	return;
    }
    if (opcode == BSTUN_OP_IGNORE) {
	datagram_done(pak);
	return;
    }
    switch (opcode) {
      case BSTUN_OP_PASS:
        break;
      default:
	bstif_send_punt(stp, idb, BSTUN_OP_BADOP, sth, "IFin");
	datagram_done(pak);
	return;
    }
    if (!sth->len) {
	if (bstun_debug)
	    errmsg(&msgsym(BADLENOP, BSTUN), "IFin", sth->op, sth->len);
	if (stp)
	    stp->drops++;
	datagram_done(pak);
	return;
    }
    /*
     * find the first match in any of the address queues.
     */
    if (!stp) {
	bstif_send_punt(NULL, idb, BSTUN_OP_UNKPEER, sth, "IFin");
        bstun_pinfo[sth->group]->groupUnroutableTx++;   
	datagram_done(pak);
	return;
    }
    /*
     * process the incoming packet
     */
    pak->datagramstart = ((uchar *) sth + sizeof(bstunhdrtype) +
			  bstun_pinfo[stp->group]->addrlength - 1);
    pak->datagramsize = sth->len;
    pak->if_output = firstsw_or_null(stp->bstun_idb);
    stp->pkts_rx++;
    if (pak)
        stp->bytes_rx += pak->datagramsize;
    bstun_fromnet_forward(pak, stp, opcode);
}


/********************************************************************
 *
 * raw interface: bstun implementation
 * peer vectors
 *
 ********************************************************************/

/*
 * bstif_direct (acts as stif_sender)
 * just send a packet directly.  no state checks.  datagramsize and
 * datagramstart are asssumed to be set up correctly..
 */

boolean
bstif_direct (bstunpeertype *peer, paktype *pak, hwidbtype *outidb,
		     boolean force, bstunpeertype **outpeer,
		     boolean usethispeer, boolean toallpp)
{
    hwidbtype *idb;

    if (outpeer)
	*outpeer = peer;
    idb = ((outidb == NULL) ? (peer->outidb) : (outidb));
    pak->if_output = idb->firstsw;
    pak->linktype = LINK_BSTUN;
    pak->acct_proto = ACCT_PROTO_BSTUN;
    if ((outidb != NULL) ||
	(idb->vencap &&
	 (*idb->vencap)(pak, (((idb->status & IDB_IEEEMASK) && (peer)) ?
			      (ulong) peer->macaddr : (ulong) 1)))) {
	datagram_out(pak);
	if (peer){
	  peer->pkts_tx++;
          if (pak)
              peer->bytes_tx += pak->datagramsize;
        }
	return(TRUE);
    }
    datagram_done(pak);
    if (peer)
      peer->drops++;
    if (bstun_debug)
        buginf("\nBSTUN: vencap failed on %s", idb->hw_namestring);
    return(FALSE);
}

/*
 * bstif_abort
 * abort a direct interface peer.  this doesn't actually do the
 * destruction of the data structure.  but it is responsible for tearing
 * down the communications link.
 */

void
bstif_abort (bstunpeertype *peer)
{
    /*
     * nothing to abort
     */
}

/*
 * bstif_close
 * close the transport connection.  accept no further input but
 * allow any output to be physically transmitted.
 */

void
bstif_close (bstunpeertype *peer, ulong param)
{
    /*
     * no way to close the connection.  interfaces are always open.
     */
}

/*
 * bstif_connect
 * establish a connection to a remote peer via a direct interface.
 * this routine isn't currently used but if it were it would
 * work just fine.  stif connections currently are always open.
 * with more configuration information being passed around this
 * will change.
 */

bstunconnrtn
bstif_connect (bstunpeertype *stp)
{
    paktype *pak;
    
    if (stp->type != BSTUN_PT_IF) {
	if (bstun_debug)
	    buginf("\nBSTUN: conn: connect pointer invalid (if)");
	return(BSTUN_CONN_FAILED);
    }
    pak = bstpak(BSTUN_OP_VREQ, 0, NULL, 0);
    if (!pak || !bstif_direct(stp, pak, NULL, FALSE, NULL, FALSE, FALSE)) {
	return(BSTUN_CONN_FAILED);
    }
    return(BSTUN_CONN_CONNECTED);
}

/********************************************************************
 *
 * tcpdriver: Remote BSTUN implementation
 * Peer Vectors
 *
 ********************************************************************/

/*
 * bstun_packet_classify() -> Classify a packet for its priority.
 *
 *   Test if BSTUN address prioritization for TCP/IP transport applies.
 *     (NOTE: Unlike stun serial transport,                            )
 *     (        the pak->if_input is used in finding the priority list )
 *
 *   If test fails, 
 *     send out based on value of last two bits of first byte of address
 *
 */
static int bstun_packet_classify (paktype *pak, bstunpeertype *stp)
{

  bstunhdrtype  *sth;
  hwidbtype     *idb;
  int            priority, match=FALSE;


  sth = (bstunhdrtype *) (pak->datagramstart);
  if (sth->op == BSTUN_OP_PASS) {
    idb = pak->if_input->hwptr;
    if(idb->priority_list) {
      match = bstun_addr_pri_tcp(idb, pak, &priority);
    }
    if(match == TRUE)
      return(priority);
    else
      return(PRIORITY_NORMAL);
  } else { 
    return (PRIORITY_HIGH);
  }

} /* End bstun_packet_classify() */

static bstunpeertype *
bstun_packet_classify_peer (
    paktype *pak,
    bstunpeertype *rootpeer)
{
    int priority, i;
    bstunpeertype *defaultone = rootpeer;

    if (!rootpeer->priority)
        return(rootpeer);
    priority = bstun_packet_classify(pak, rootpeer);
    if (priority == 0) {
        return(rootpeer);
    } else {
        for (i = priority; i <= BSTUN_PRIORITY_COUNT; i++) {
            if (rootpeer->priority_peers[i]) {
                return(rootpeer->priority_peers[i]);
            }
        }
    }
    return(defaultone);
}


/*
 * bsttcpd_sender
 * this is the sender for tcp using the tcpdriver.
 *
 * if the peer isn't open yet the appropriate state is entered.
 * if this is the first packet in the not open state it is queued
 * for the background process which will open.  if this isn't the
 * first packet it is squished into the ground.
 *
 * this routine can be called from anywhere including interrupt level.
 */

boolean
bsttcpd_sender (bstunpeertype *inpeer, paktype *pak, hwidbtype *outidb,
		       boolean force, bstunpeertype **outpeer,
		       boolean usethispeer, boolean toallpp)
{
    leveltype status;
    int res,i, j;
    paktype *newpak[BSTUN_PRIORITY_COUNT];
    boolean tmp_result;
    bstunpeertype *peer;

    if ((toallpp) && (inpeer->priority)) {
        if (outpeer) {
            *outpeer = inpeer;
        }
        /* Allocate ALL of the packets up front. This is all or nothing! */
        for (i = 0; i < BSTUN_PRIORITY_COUNT; i++) {
            newpak[i] = pak_duplicate(pak);
            if (!newpak[i]) {
                for (j = (i - 1); j >= 0; j--) {
                    datagram_done(newpak[j]);
                }
                datagram_done(pak);
                return(FALSE);
            }
        }
        for (i = 1; i <= BSTUN_PRIORITY_COUNT; i++) {
            if (!inpeer->priority_peers[i]) {
                if (bstun_debug) {
                    buginf("\nBSTUN: Odd state; not all peers exist");
                }
                tmp_result = FALSE;
            }  else {
                tmp_result = bsttcpd_sender(inpeer->priority_peers[i],
                                            newpak[i-1], outidb, force, NULL,
                                            TRUE, FALSE);
            }
            if ((!inpeer->priority_peers[i]) || (!tmp_result)) {
                if (bstun_debug) {
                    buginf("\nBSTUN: Failed to send to %s priority peer",
                           priority_peername(i));
                }
                for (j = i; j < BSTUN_PRIORITY_COUNT; j++) {
                    datagram_done(newpak[j]);
                }
                datagram_done(pak);
                return(FALSE);
            }
        }
        usethispeer = TRUE;
        /* Fall through to the normal code now... */
    }

    if (usethispeer) {
	peer = inpeer;
    } else {
	peer = bstun_packet_classify_peer(pak, inpeer);
    }
    if (outpeer)
	*outpeer = peer;
  
    status = raise_interrupt_level(NETS_DISABLE);
    switch (peer->state) {
      case BSTUN_PS_CONNECTED:
	reset_interrupt_level(status);
        /*
         * If peer->primary_t is NULL, then we do not have a TCP session.
         * Simulate tcpdriver_send()'s return code for "no tcp session".
         * This situation occurs after a TCP session loss and sttcpd_abort()
         * has executed.
         */
        if (!peer->primary_t)
           res = TCPDSEND_CLOSED;
        else {
           res = reg_invoke_ip_tcpdriver_send(peer->primary_t, pak, force);
        }
	if (TCPDSEND_ERROR(res)) {
	    peer->drops++;
	    datagram_done(pak);
	    if (res == TCPDSEND_CLOSED) {
		bstun_newstate(peer, BSTUN_PS_DEAD);
	    }
	    return(FALSE);
	}
	peer->pkts_tx++;
        if (pak)
            peer->bytes_tx += pak->datagramsize;
	return(TRUE);
      case BSTUN_PS_CLOSED:
        bstun_newstate(peer, BSTUN_PS_OPENING);
        pak->peer_ptr =  peer;
        process_enqueue(bstconn_pakQ_p, pak);
        reset_interrupt_level(status);
        peer->pkts_tx++;
        peer->bytes_tx += pak->datagramsize; 
        return(TRUE);
      case BSTUN_PS_DEAD:
      case BSTUN_PS_OPENWAIT:
      case BSTUN_PS_OPENING:
      default:
	peer->drops++;
        if (bstun_debug)
            buginf("\nBSTUN: tcpd sender in wrong state, dropping packet");
	reset_interrupt_level(status);
	datagram_done(pak);
	return(FALSE);
    }
}
/*
 * bsttcpd_abort
 * nuke with extreme prejudice a tcpdriver peer.  this aborts the
 * connection and drains any input.
 */

void
bsttcpd_abort (bstunpeertype *stp)
{
    char peerid[BSTUN_MAX_PEER_NAME];

    if (bstun_debug && stp->primary_t) {
	errmsg(&msgsym(TCPPEERSHUT, BSTUN), "aborting", bstpeer_id(stp, peerid),
	       reg_invoke_ip_tcpdriver_fhost(stp->primary_t),
	       reg_invoke_ip_tcpdriver_fport(stp->primary_t));
    }
    if (stp->primary_t) {
	reg_invoke_ip_tcpdriver_update(stp->primary_t, NULL, 0, NULL);
	reg_invoke_ip_tcpdriver_abort(stp->primary_t);
	stp->primary_t = NULL;
    }
    if (stp->secondary_t) {
	reg_invoke_ip_tcpdriver_update(stp->secondary_t, NULL, 0, NULL);
	reg_invoke_ip_tcpdriver_abort(stp->secondary_t);
	stp->secondary_t = NULL;
    }
}

void
bsttcpd_close (bstunpeertype *stp, ulong param)
{
    char peerid[BSTUN_MAX_PEER_NAME];

    if (bstun_debug) {
	errmsg(&msgsym(TCPPEERSHUT, BSTUN), "closing", bstpeer_id(stp, peerid),
	       reg_invoke_ip_tcpdriver_fhost(stp->primary_t),
	       reg_invoke_ip_tcpdriver_fport(stp->primary_t));
    }
    if (stp->primary_t) {
	reg_invoke_ip_tcpdriver_update(stp->primary_t, bsttcpd_drain, 1, 
				       (void *) param);
	reg_invoke_ip_tcpdriver_close(stp->primary_t);
    }
    if (stp->secondary_t) {
	reg_invoke_ip_tcpdriver_update(stp->secondary_t, bsttcpd_drain, 1,
				       (void *) param);
	reg_invoke_ip_tcpdriver_close(stp->secondary_t);
    }
    if (param == (ulong) 0) {
	stp->primary_t = stp->secondary_t = NULL;
    }
}

/* 
 * bsttcpd_flush
 *
 * Process a flags marked for "FLUSHING"
 */
static boolean
bsttcpd_flush (void *stpcookie)
{
    bstunpeertype *stp, *stpsub;
    bstun_protocol_info *gpi;
    hwidbtype *idb;

    stp = (bstunpeertype *) stpcookie;

    if ((!stp) || ((gpi = bstun_pinfo[stp->group]) == NULL)) {
	return(TRUE);
    }

    /*
     * Okay -- we found a legitimate group. Now let's make sure all of its
     * queues are empty.
     */
    for (idb = bstun_first_idb; idb; idb = IDB_NEXT(idb)) {
	if (!idb->bstun_Q.qhead) {
	    continue;
	}
	if (idb->bstun_group != stp->group) {
	    continue;
	}
	for (stpsub = (bstunpeertype *) idb->bstun_Q.qhead; stpsub;
	     stpsub = stpsub->next) {
	    if ((stpsub->type != BSTUN_PT_TCPD)) {
		if (stpsub->primary_t != NULL) {
		    if (reg_invoke_ip_tcpdriver_unacked_output(
						       stpsub->primary_t)) {
			return(FALSE);
		    }
		}
		if (stpsub->secondary_t != NULL) {
		    if (reg_invoke_ip_tcpdriver_unacked_output(
						       stpsub->secondary_t)) {
			return(FALSE);
		    }
		}
	    }
	}
    }
    return(TRUE);
}

/*
 * bsttcpd_connect
 * perform an active open to establish a connection to our peer.  it
 * handles basic opening and potential dual opens that might be occuring.
 */

bstunconnrtn
bsttcpd_connect (bstunpeertype *stp)
{
    int openstat;
    ipaddrtype lastip;
    tcp_encaps *t;
    char peerid[BSTUN_MAX_PEER_NAME];
    tcp_encaps tmp;

    if (stp->type != BSTUN_PT_TCPD) {
	if (bstun_debug)
	    buginf("\nBSTUN: CONN: connect pointer invalid (tcpd)");
	return(BSTUN_CONN_FAILED);
    }
    stp->retries = BSTUN_MAX_OPEN_TRIES + 1;
    while (--stp->retries) {
	if (bstun_debug)
	    errmsg(&msgsym(OPENING, BSTUN), bstpeer_id(stp, peerid),
		   stp->retries);
	openstat = -1;
	lastip = stp->ipaddr;
	/*
	 * tcpdriver_active_open will block waiting for the connection
	 * to open.
	 */
	memset(&tmp, 0, sizeof(tcp_encaps));
	tmp.inthresh = sizeof(bstunhdrtype) +
	    bstun_pinfo[stp->group]->addrlength - 1;
	tmp.cookie = stp;
	tmp.readf =  bsttcpd_rdhdr;
	tmp.closef = bsttcpd_fini;
	tmp.flushf = bsttcpd_flush;
        tmp.flow_ctrl = NULL;
        tmp.flags = 0;
	t = reg_invoke_ip_tcpdriver_active_open(stp->ipaddr, bstun_name,
						stp->port,
						10240, /* tcp window */	
						&tmp, &openstat);
	reg_invoke_ip_tcpdriver_set_oqmax(t, stp->user_def_max_tcp_qlen);
	reg_invoke_ip_tcpdriver_change_timeout(t,TRUE); /* reenable timeouts */
	/*
	 * while we were asleep it's possible that someone pulled
	 * the rug.  check out our pointers.
	 */
	if (stp->type != BSTUN_PT_TCPD || lastip != stp->ipaddr ||
	    stp->state < BSTUN_PS_OPENWAIT) {
	    reg_invoke_ip_tcpdriver_abort(t);
	    return(BSTUN_CONN_ABORTED);
	}
	switch (stp->state) {
	  case BSTUN_PS_OPENWAIT:
	    if (t) {			/* successful open? */
		if (stp->primary_t) {
		    /*
		     * remote side already got in.  we no longer
		     * need the connection just opened.
		     */
		    reg_invoke_ip_tcpdriver_update(t, bsttcpd_drain, 1, NULL);
		    reg_invoke_ip_tcpdriver_abort(t);
		} else {
		    stp->primary_t = t;
		}
		return(BSTUN_CONN_CONNECTED);
	    }
	    break;

	  default:
	    errmsg(&msgsym(CONNILLSTATE, BSTUN), bstpeer_id(stp, peerid),
		   stp->state);
	    reg_invoke_ip_tcpdriver_abort(t);
	    return(BSTUN_CONN_FAILED);
	}
	if (openstat == NO_MEMORY || openstat == UNREACHABLE ||
	      openstat == TIMEDOUT || openstat == OPTSTOOBIG) {
	    stp->retries = 1;
	}
    }
    if (stp->state == BSTUN_PS_OPENWAIT && openstat == -1) {
	openstat = TIMEDOUT;
    }
    if (bstun_debug && stp->state != BSTUN_PS_CLOSED) {
	errmsg(&msgsym(CONNOPENFAIL, BSTUN), bstpeer_id(stp, peerid),
	       tcpuser_open_fail_str(openstat), openstat);
    }
    return(BSTUN_CONN_FAILED);
}


/********************************************************************
 *
 * tcpdriver: support
 *
 ********************************************************************/

void
bsttcpd_send_close (tcp_encaps *t, ushort op, bstunhdrtype *sth,
			char *label, void *abort_cookie, boolean abort)
{
    paktype *pak;
    int result;

    if (bstun_debug) {
	errmsg(&msgsym(SENDPUNTTCP, BSTUN), label, bst_op2str(op),
	       sth->group, reg_invoke_ip_tcpdriver_fhost(t));
    }
    result = TCPDSEND_QFULL;
    pak = bstpak(op, sth->group, &(sth->addr[0]), 0);
    if (pak)
	result = reg_invoke_ip_tcpdriver_send(t, pak, TRUE);
    if (!pak || TCPDSEND_ERROR(result)) {
	if (!pak)
            errmsg(&msgsym(NOBUF, BSTUN), "BSTUN", "send tcp close");
	if (bstun_debug) {
	    buginf("\nBSTUN: %s: send close, no pak", label);
	}
	if (pak) {
	    datagram_done(pak);
	}
    }
    if (abort) {
	reg_invoke_ip_tcpdriver_update(t, bsttcpd_drain, 1, abort_cookie);
	reg_invoke_ip_tcpdriver_close(t);
    }
    return;
}

/*
 * bsttcpd_err
 * Simple error handler for dealing with errors coming back from a
 * peer.
 */

void
bsttcpd_err (bstunpeertype *stp, tcp_encaps *t, bstunhdrtype *sth,
		 char *label)
{
    char errbuff[BSTUN_MAX_PEER_NAME];

    if (bstun_debug)
	errmsg(&msgsym(ERR, BSTUN), label, bst_op2str(sth->op),
	       bstpeer_id(stp, errbuff), sth->op, sth->len);
    switch (sth->op) {
      case BSTUN_OP_BADOP:
      case BSTUN_OP_UNKPEER:
      case BSTUN_OP_UNKADDR:
      default:
	reg_invoke_ip_tcpdriver_abort(t);
	break;
    }
}

/*
 * bstget_hdr
 * obtain the bytes needed to build a header.
 */

boolean
bstget_hdr (tcp_encaps *t, tcbtype *tcb, int thresh, bstunpeertype *stp,
		   bstunhdrtype *sth, char *label, boolean *ignore)
{
    int addr_thresh;
    long bytes;
    int s;
    char* p;
    uchar * dest;
    char errbuf[BSTUN_MAX_PEER_NAME];
    paktype *pak;
    
    *ignore = FALSE;
    bytes = tcp_unread(tcb);
    if (bytes < thresh) {
	if (bstun_debug)
	    buginf("\nBSTUN: %s: %s bad state, thresh %d, bytes %d",
		   label, (stp ? bstpeer_id(stp, errbuf) : ""), thresh, bytes);
	return(FALSE);
    }
    dest =  (uchar*)sth;
    do {
	s = tcp_getstring(tcb, ((uchar **)&p), ((int *)&bytes), thresh);
	if (s != OK) {
	    if (bstun_debug)
		buginf("\nBSTUN: %s: getstring failed %d - %i(%d) -> %d",
		       label, s, tcb->foreignhost, tcb->foreignport,
		       tcb->localport);
	    return(FALSE);
	}
	bcopy(p, dest, bytes);
	dest += bytes;
	thresh -= bytes;
    } while (thresh > 0);

    /* Received data. Count as a keepalive */
    if (stp)
	GET_TIMESTAMP(stp->last_keepalive);
    if ((sth->op == BSTUN_OP_VREQ) ||
	(sth->op == BSTUN_OP_VREP)) {
	*ignore = TRUE;
	if (bstun_debug)
  	    buginf("\nBSTUN: version %x from remote on %i(%d) -> %d. Reply %x.",
		   sth->magic, tcb->foreignhost, tcb->foreignport,
		   tcb->localport, BSTUN_MAGIC);
	if (sth->op == BSTUN_OP_VREQ) {
	    int result;

	    result = TCPDSEND_QFULL;
	    pak = bstpak(BSTUN_OP_VREP, 0, NULL, 0);
	    if (pak)
		result = reg_invoke_ip_tcpdriver_send(t, pak, TRUE);
	    if (!pak || TCPDSEND_ERROR(result)) {
		if (!pak)
		    errmsg(&msgsym(NOBUF, BSTUN), "BSTUN", "send version reply");
		if (bstun_debug) {
                    buginf("\nBSTUN: %s: send version reply, nopak/fail",
                           bstpeer_id(stp, errbuf));
		}
		if (pak) {
		    datagram_done(pak);
		}
	    }
	}
	return(TRUE);
    }
    if (sth->magic != BSTUN_MAGIC) {
	if (bstun_debug)
	    errmsg(&msgsym(BADMAGIC, BSTUN), label,
		   BSTUN_MAGIC, sth->magic, 2);
	return(FALSE);
    }

    /*
     * In passive open, the header is read in two parts -- first, the core
     * portion, and then any additional "address bytes" as dictated by the
     * protocol information. This allows passive_opens to be general.
     */
    if (bstun_pinfo[sth->group] == NULL) {
	return(FALSE);
    }

    addr_thresh = bstun_pinfo[sth->group]->addrlength;

    if (addr_thresh) {
	dest =  &(sth->addr[0]);
	do {
	    s = tcp_getstring(tcb, ((uchar **)&p), ((int *)& bytes), addr_thresh);
	    if (s != OK) {
		if (bstun_debug)
		    buginf("\nBSTUN: %s: getstring failed %d - %i(%d) -> %d",
			   label, s, tcb->foreignhost, tcb->foreignport,
			   tcb->localport);
		return(FALSE);
	    }
	    bcopy(p, dest, bytes);
	    dest += bytes;
	    addr_thresh -= bytes;
	} while (addr_thresh > 0);
    }
    
    return(TRUE);
}

/*
 * bsttcpd_drain
 * Drain all input.  This makes sure we can't hang on input in a tcpdriver
 * close state while waiting for any output to finish up.
 */

void
bsttcpd_drain (tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie)
{
    long bytes;
    uchar* p;
    int s;
    bstunpeertype *stp;
    
    stp = (bstunpeertype *)cookie;

    bytes = tcp_unread(tcb);
    if (bstun_debug) {
	buginf("\nBSTUN: In sttcpd_drain, unget = %d", bytes);
    }
    while (bytes) {
	s = tcp_getstring(tcb, &p, ((int *)&bytes), bytes);
	if (s != OK && (bstun_debug)) {
	    buginf("\nBSTUN: DRAIN: getstring failed %d", s);
	}
	if (s != OK) {
	    reg_invoke_ip_tcpdriver_abort(t);
	    return;
	}
	if (bstun_debug) {
	    buginf("\nBSTUN: After getstring in sttcpd_drain");
	}
	bytes = tcp_unread(tcb);
    }
    if (bstun_debug) {
	buginf("\nBSTUN: Done with sttcpd_drain.");
    }
}

/*
 * bsttcpd_phdr
 * Deal with the first packet for a new connection.  This
 * has to obviously be on the passive side.  The active
 * side already knows which connection it has.
 */

void
bsttcpd_phdr (tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie)
{
    boolean ignore;
    bstunhdrtype lhdr, *sth;
    leveltype status;
    char peerid[BSTUN_MAX_PEER_NAME];
    bstunpeertype *stp;
    
    stp = (bstunpeertype *)cookie;

    if (stp) {
	if (bstun_debug) {
	    buginf("\nBSTUN: PHDR: stp set, t %i, %s",
		   reg_invoke_ip_tcpdriver_fhost(t), bstpeer_id(stp, peerid));
	}
	reg_invoke_ip_tcpdriver_abort(t);
	return;
    }
    sth = &lhdr;
    if (!bstget_hdr(t, tcb, (sizeof(bstunhdrtype) - 1), stp,
		   sth, "PHDR", &ignore)) {
	reg_invoke_ip_tcpdriver_abort(t);
	return;
    }
    if (ignore) {
        return;
    }
    stp = bsttcp_findpeer(sth->group, &(sth->addr[0]),
			 reg_invoke_ip_tcpdriver_fhost(t),
			 reg_invoke_ip_tcpdriver_lport(t),
			 reg_invoke_ip_tcpdriver_fport(t));
    if (sth->op & BSTUN_OP_ERR) {
	bsttcpd_err(stp, t, sth, "PHDR");
	reg_invoke_ip_tcpdriver_update(t, bsttcpd_drain, 1, stp);
	reg_invoke_ip_tcpdriver_close(t);
	return;
    }
    if (!stp) {
	bsttcpd_send_close(t, BSTUN_OP_UNKPEER, sth, "PHDR", NULL, TRUE);
	return;
    }
    if (bstun_debug) {
	errmsg(&msgsym(OPENED, BSTUN), "PHDR", bstpeer_id(stp, peerid),
	       bstpeer_state(stp));
    }
    status = raise_interrupt_level(NETS_DISABLE);
    switch (stp->state) {
      case BSTUN_PS_DEAD:
      case BSTUN_PS_CLOSED:
      case BSTUN_PS_OPENING:
      case BSTUN_PS_OPENWAIT:
	bstun_newstate(stp, BSTUN_PS_CONNECTED);
	if (stp->primary_t) {
	    stp->secondary_t = t;
	} else {
	    stp->primary_t = t;
	}

      case BSTUN_PS_CONNECTED:
	if (t != stp->primary_t && t != stp->secondary_t) {
	    /*
	     * Interesting.  We received an incoming packet but not
	     * on the connection we thought we were connected to.  Must
	     * be reestablishing contact.  Abort previous connection and
	     * then use this one as the new primary.  No longer need
	     * secondary.
	     */
	    if (bstun_debug) {
		errmsg(&msgsym(RECONNECT, BSTUN), bstpeer_id(stp, peerid));
	    }
	    bsttcpd_abort(stp);
	    stp->primary_t = t;
	}
	if (sth->len) {
	    bcopy((uchar *) sth, (uchar *) &stp->inhdr,
		  sizeof(bstunhdrtype) +
		  bstun_pinfo[stp->group]->addrlength - 1);
	    reg_invoke_ip_tcpdriver_update(t, bsttcpd_rddata, sth->len, stp);
	} else {
	    stp->inhdr.op = BSTUN_OP_EMPTY;
	    reg_invoke_ip_tcpdriver_update(t, bsttcpd_rdhdr,
					   sizeof(bstunhdrtype) +
					   bstun_pinfo[stp->group]->addrlength
					   - 1, stp);
	}

        /*
         * The TCP passive open side (sttcp_listen) does not have access
         * to the stp structure, so it could not properly set the max tcp 
         * queue size during the passive open. Set it properly here.
         */
        reg_invoke_ip_tcpdriver_set_oqmax(t, stp->user_def_max_tcp_qlen);

	stp->pkts_rx++;
	reset_interrupt_level(status);
	return;
		
      default:
	stp->drops++;
	reset_interrupt_level(status);
	if (bstun_debug) {
	    buginf("\nBSTUN: PHDR: bad connection state %d, %s",
		   stp->state, bstpeer_id(stp, peerid));
	}
	bsttcpd_abort(stp);
	return;
    }
}

/*
 * bsttcpd_rdhdr
 * tcpdriver implementation of bstun peer handler.  This
 * routine accepts the first part of an incoming packet from
 * a remote peer.  If there is data it resets the reader
 * function of the driver struct.
 */

void
bsttcpd_rdhdr (tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie)
{
    int bytes, s;
    uchar *p, *dest;
    paktype *pak=0;
    char peerid[BSTUN_MAX_PEER_NAME];
    bstunpeertype *stp;

    stp = (bstunpeertype *)cookie;

    if (!stp || stp->inhdr.op != BSTUN_OP_EMPTY) {
	buginf("\nBSTUN: HDR: state wrong");
	if (stp) {
	    stp->inhdr.op = BSTUN_OP_EMPTY;
	}
	reg_invoke_ip_tcpdriver_abort(t);
	return;
    }
    bytes = tcp_unread(tcb);
    if (stp->state != BSTUN_PS_CONNECTED ||
	thresh != (sizeof(bstunhdrtype) +
		   bstun_pinfo[stp->group]->addrlength - 1) ||
	bytes < thresh) {
	if (bstun_debug) {
	    buginf("\nBSTUN: HDR: %s bad state, thresh %d, bytes %d",
		   bstpeer_id(stp, peerid), thresh, bytes);
	}
	reg_invoke_ip_tcpdriver_abort(t);
	return;
    }	
    dest = (uchar *)&stp->inhdr;
    do {
	s = tcp_getstring(tcb, &((uchar *) p), &bytes, thresh);
	if (s != OK) {
	    if (bstun_debug) {
		buginf("\nBSTUN: HDR: getstring failed %d - %i(%d) -> %d",
		       s, tcb->foreignhost, tcb->foreignport,
		       tcb->localport);
	    }
	    reg_invoke_ip_tcpdriver_abort(t);
	    return;
	}
	bcopy(p, dest, bytes);
	dest += bytes;
	thresh -= bytes;
    } while (thresh > 0);

    if (stp->inhdr.magic != BSTUN_MAGIC) {
	if (bstun_debug) {
	    errmsg(&msgsym(BADMAGIC, BSTUN), "HDR", 
		   BSTUN_MAGIC, stp->inhdr.magic, 3);
	}
	reg_invoke_ip_tcpdriver_abort(t);
	stp->drops++;
	stp->inhdr.op = BSTUN_OP_EMPTY;
	return;
    }
    if (bstun_debug) {
	if (stp->inhdr.op != BSTUN_OP_PASS) {
	    buginf("\nBSTUN: Received %s opcode from %s at %d",
		   bst_op2str(stp->inhdr.op), bstpeer_id(stp, peerid),
		   (ELAPSED_TIME(bstun_start_time)/ONESEC));
	}
    }
    /* Any data at all is equivalent to a keepalive */
    GET_TIMESTAMP(stp->last_keepalive);

    if (stp->inhdr.op == BSTUN_OP_VREQ) {
	int result;

	result = TCPDSEND_QFULL;
	pak = bstpak(BSTUN_OP_VREP, 0, NULL, 0);
	if (pak)
	    result = reg_invoke_ip_tcpdriver_send(t, pak, TRUE);
	if (!pak || TCPDSEND_ERROR(result)) {
	    if (!pak)
		errmsg(&msgsym(NOBUF, BSTUN), "BSTUN", "send version reply");
	    if (bstun_debug) {
		buginf("\nBSTUN: %s: send version reply, nopak/fail",
		       bstpeer_id(stp, peerid));
	    }
	    if (pak) {
		datagram_done(pak);
	    }
	}
    }
    if (stp->inhdr.len) {
	reg_invoke_ip_tcpdriver_update(t, bsttcpd_rddata, stp->inhdr.len, stp);
     } else {
        stp->inhdr.op = BSTUN_OP_EMPTY;
    }
   stp->pkts_rx++;
   if (pak)
       stp->bytes_rx += pak->datagramsize;
}

void
bsttcpd_rddata (tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie)
{
    bstunhdrtype *sth;
    int bytes, s;
    paktype *pak;
    uchar* dest;
    uchar* p;
    char errbuff[BSTUN_MAX_PEER_NAME];
    ushort opcode;
    bstunpeertype *stp;
    
    stp = (bstunpeertype *)cookie;

    if (!stp || stp->inhdr.op == BSTUN_OP_EMPTY || !stp->inhdr.len) {
	if (bstun_debug) {
	    buginf("\nBSTUN: DATA: peer %s bad state, op %d len %d",
		   (stp ? bstpeer_id(stp, errbuff) : ""),
		   (stp ? stp->inhdr.op : -1), (stp ? stp->inhdr.len : -1));
	}
	reg_invoke_ip_tcpdriver_abort(t);
	return;
    }
    sth = &stp->inhdr;
    pak = getbuffer(sth->len);
    if (!pak) {
	if (bstun_debug)
	    buginf("\nBSTUN: DATA: no memory");
	do {
	    s = tcp_getstring(tcb, (uchar **) &p, &bytes,
			      thresh);
	    if (s != OK) {
		if (bstun_debug) {
		    buginf("\nBSTUN: DATA: drain getstring failed %d", s);
		}
		sth->op = BSTUN_OP_EMPTY;
		reg_invoke_ip_tcpdriver_abort(t);
		return;
	    }
	    thresh -= bytes;
	} while (thresh > 0);
	return;
    }
    dest = pak->datagramstart = pak->network_start;
    pak->datagramsize = sth->len;
    do {
	s = tcp_getstring(tcb, (uchar **) &p, &bytes, thresh);
	if (s != OK) {
	    if (bstun_debug) {
		buginf("\nBSTUN: DATA: getstring failed %d", s);
	    }
	    sth->op = BSTUN_OP_EMPTY;
	    stp->drops++;
	    reg_invoke_ip_tcpdriver_abort(t);	/* assume out of sync */
	    datagram_done(pak);
	    return;
	}
	bcopy(p, dest, bytes);
	dest += bytes;
	thresh -= bytes;
    } while (thresh > 0);
    opcode = sth->op;
    switch (opcode) {
      case BSTUN_OP_PASS:
	pak->enctype = ET_NULL;
	pak->if_output = firstsw_or_null(stp->bstun_idb);
	bstun_fromnet_forward(pak, stp, opcode);
	break;

      case BSTUN_OP_IGNORE:
	datagram_done(pak);
	break;

      default:
	if (bstun_debug)
	    buginf("\nBSTUN: DATA: bad op %d (%s) from %s",
		   opcode, bst_op2str(sth->op), bstpeer_id(stp, errbuff));
	stp->drops++;
	datagram_done(pak);
	break;
    }
    sth->op = BSTUN_OP_EMPTY;
    reg_invoke_ip_tcpdriver_update(t, bsttcpd_rdhdr,
				   sizeof(bstunhdrtype) +
				   bstun_pinfo[stp->group]->addrlength - 1,
				   stp);
}

/*
 * bsttcpd_listen
 * Given a tcb opened by tcp_listen upon an incoming connection, insert
 * it into the tcpdriver structures.  We don't have enough information
 * here to easily validate so we wait for an incoming packet.  If prior
 * to receiving that first packet this side tries to open it's open.
 */

void
bsttcpd_listen (tcbtype *tcb)
{
    tcp_encaps *t;
    tcp_encaps tmp;

    memset(&tmp, 0, sizeof(tcp_encaps));
    tmp.readf =  bsttcpd_phdr;
    tmp.closef = bsttcpd_fini;
    tmp.flushf = bsttcpd_flush;
    tmp.flow_ctrl = NULL;
    tmp.inthresh = sizeof(bstunhdrtype) + BSTUN_MAX_ADDR_LENGTH - 1;
    tmp.cookie = NULL;
    /* set the TCP window large enough to hold at least 2 of the largest frames */
    t = reg_invoke_ip_tcpdriver_passive_open(tcb, 10240, &tmp);
    if (!t) {
	if (bstun_debug) {
	    errmsg(&msgsym(BADPASSIVEOPEN, BSTUN),
		   tcb->foreignhost, tcb->foreignport, tcb->localport);
	}
	tcp_close(tcb);
    } else {
	/* set tcp queue size to default until peer information is available */
	reg_invoke_ip_tcpdriver_set_oqmax(t, BSTUN_MAX_QLEN);
	if (bstun_debug) {
	    errmsg(&msgsym(PASSIVEOPEN, BSTUN),
		   tcb->foreignhost, tcb->foreignport, tcb->localport);
	}
    }
    return;
}

/*
 * bsttcpd_fini
 * routine called when the tcpdriver detects a close on
 * the tcpdriver stream.
 */

void
bsttcpd_fini (tcp_encaps *t, void *cookie)
{
    leveltype status;
    char peerid[BSTUN_MAX_PEER_NAME];
    bstunpeertype *stp;

    stp = (bstunpeertype *)cookie;

    if (!stp) {				/* unknown peer abort */
	if (bstun_debug)
	    buginf("\nBSTUN: unknown peer closed");
	return;
    }
    status = raise_interrupt_level(NETS_DISABLE);
    stp->inhdr.op = BSTUN_OP_EMPTY;
    stp->primary_t = NULL;
    stp->secondary_t = NULL;
    if (bstun_debug)
	errmsg(&msgsym(TCPFINI, BSTUN), bstpeer_id(stp, peerid),
	       bstpeer_state(stp));
    bstun_newstate(stp, BSTUN_PS_CLOSED);
    reset_interrupt_level(status);
}

/*********************************************************************
 *
 * Misc routines.
 * 
 *********************************************************************/

/*
 * bstun_newstate
 *
 * Make the transition to a new state in bstunpeertype.
 */
void
bstun_newstate (bstunpeertype *stp, bstunstatetype newstate)
{
    char peerid[BSTUN_MAX_PEER_NAME];
    uint i;

    if (bstun_debug) {
     buginf("\nBSTUN: Change state for peer %s (%s->%s)",
             bstpeer_id(stp, peerid), bst_state(stp->state), bst_state(newstate));
    }
    stp->state = newstate;
    GET_TIMESTAMP(stp->last_keepalive);    

    if (stp->state != BSTUN_PS_CONNECTED) {
       if (stp->all_addr) {
          for (i = 0; i < BSTUN_MAX_ADDRS; i++)
              stp->arsl_p->addr_route_state_change[i] = FALSE;
       }
       else
          stp->addr_route_state_change = FALSE;
    }
}

/*
 * bstun_active
 * Return an indication that the Bstunnel is turned on.
 */

boolean
bstun_active (ulong sport, ulong dport, ipaddrtype fhost)
{
    return(bstun_first_idb != NULL);
}

/*
 * bstun_periodic
 * Do periodic things that need to be done.  Currently the only 
 * things we do are:
 *   (1) scan the remote peers for dead ones and reactivate, them, 
 *   (2) call protocol specific code, 
 */

static void
bstun_periodic_per_peer (
    bstunpeertype *stp)
{
    if (stp->state == BSTUN_PS_DEAD) {
	if (stp->type == BSTUN_PT_IF)
	    bstun_newstate(stp, BSTUN_PS_CONNECTED);
	else
	    bstun_newstate(stp, BSTUN_PS_CLOSED);
    }
}

void
bstun_periodic (void)
{
    hwidbtype *idb;
    bstunpeertype *stp;
    int i;
 
    for (idb = bstun_first_idb; idb; idb = IDB_NEXT(idb)) {
	for (stp = (bstunpeertype *) idb->bstun_Q.qhead; stp;
	       stp = stp->next) {
	    bstun_periodic_per_peer(stp);
            if (stp->priority) {
                for (i = 1; i <= BSTUN_PRIORITY_COUNT; i++)
                    bstun_periodic_per_peer(stp->priority_peers[i]);
            }
	}
    }
}


/*
 * return bstunpeertype if BSTUN has been configured for addrp
 */
bstunpeertype*
bstun_query_route_cfg (uchar* addrp,
                   hwidbtype *idb)
{
   bstunpeertype *stp = NULL;
   bstun_protocol_info *gpi;

   /*---- Locate the peer control block for this addr. */
   if ((idb) && ((gpi = bstun_pinfo[idb->bstun_group]) != NULL)) {
      for (stp = (bstunpeertype *) idb->bstun_Q.qhead; stp;
                 stp =(bstunpeertype *) stp->next) {
          if (stp->all_addr) {
	     break;
	  }
          if (stp->input_addr_inuse) {
	     if (!bcmp(addrp, stp->input_addr, gpi->addrlength))
	        break;
	     }
	  else {
	     if (!bcmp(addrp, stp->addr, gpi->addrlength))
	        break;
	     }
      }
   }

   return(stp);
   
}  /* bstun_query_route_cfg. */

/*
 * bstun_query_route
 *
 * This procedure is called by a protocol (e.g. BSC) to determine the
 * status of the route through the tunnel, for a given address on a
 * given port (idb).
 * The returned value is either ADDRESS_UNKNOWN, DOWN, UP or RESTARTED.
 * The RESTARTED value is returned if the route has come up since the
 * last call call to bstun_query_route for this address on this idb.
 */

bstunaddrroutestate_e
bstun_query_route (uchar* address,
                   hwidbtype *idb)
{
   bstunpeertype *stp = bstun_query_route_cfg(address, idb);
   bstunaddrroutestate_e ars = BSTUN_ROUTE_ADDR_UNKNOWN;

   if (stp) {
      if(stp->state == BSTUN_PS_CONNECTED) {
         if (stp->all_addr) {
            if (stp->arsl_p->addr_route_state_change[address[0]])
               ars =  BSTUN_ROUTE_UP;
            else {
               stp->arsl_p->addr_route_state_change[address[0]] = TRUE;
               ars = BSTUN_ROUTE_RESTARTED;
	    }
         }
         else {
            if (stp->addr_route_state_change)
               ars = BSTUN_ROUTE_UP;
            else {
               stp->addr_route_state_change = TRUE;
               ars = BSTUN_ROUTE_RESTARTED;
	    }
         }
      }
      else {
         ars = BSTUN_ROUTE_DOWN;
      }
   }
   return(ars);

}  /* bstun_query_route. */



static void
bstun_print_peer_info (
    hwidbtype *idb,
    bstunpeertype *stp,
    int i,
    uchar address )
{
    char buff[BSTUN_MAX_PEER_NAME];

    if ((address == 0) || (address == stp->addr[0])) {

        if (i == 0) {
            bstun_sprintf_addr(stp->addr, stp->all_addr, stp->group, buff);
            printf("\n%6s", buff);
        }
        switch (stp->type) {
        case BSTUN_PT_NONE:
        case BSTUN_PT_TCPD:
            if (!(stp->priority_top)) {
                printf("%10s%16i", "TCP", stp->ipaddr);
            }
            if ((stp->priority_top) || (stp->priority)) {
                printf("\n   %s Priority Peer    ", priority_peername(i));
            }
            break;
        case BSTUN_PT_IF:
            printf("%10s%16s", "IF", stp->outidb->hw_namestring);
            break;
        case BSTUN_PT_TEST:
            printf("%10s%16s", "Test", "");
            break;
        default:
            printf("%10s%16s", "", "");
            break;
        }
        printf("%10s%-8u%-8u%-8u", bstpeer_state(stp),
                stp->pkts_rx, stp->pkts_tx, stp->drops);
    }
}

static void
bstun_show_print (hwidbtype *idb, uint address)
{
    bstunpeertype *stp;

    printf("\n\n %c%s %s%s",
           ((idb == bstun_first_idb) ? '*' : ' '),
           idb->hw_namestring,
           (idb->firstsw->description ? "-- " : ""),
           (idb->firstsw->description ? idb->firstsw->description : ""));
    printf(" (group %u [%s])", idb->bstun_group, bstun_pinfo[idb->bstun_group]->name);

    if (idb->bstun_Q.qhead) {
        printf("\n%6s%10s%16s%10s%8s%8s%8s",
               "route", "transport", "address", "state", " rx_pkts", " tx_pkts", "   drops");
    }

    for (stp = idb->bstun_Q.qhead; stp; stp = stp->next) {
        bstun_print_peer_info(idb, stp, 0, address);
        if (stp->priority) {
            int i;
            for (i = 1; i <= BSTUN_PRIORITY_COUNT; i++) {
                if (stp->priority_peers[i]) {
                    bstun_print_peer_info(idb, stp->priority_peers[i], i, address);
                }
            }
        }
    }

    if (idb->bstun_Q.qhead) {
        printc('\n');
    }
}

/*
 * show_bstun
 * Display current serial tunnel configuration
 */

void
show_bstun (parseinfo *csb)
{
    hwidbtype *idb;
    uint group, address;
    boolean first;
 
    if (!bstun_name) {
	printf("%% BSTUN transport not enabled");
	return;
    }
    
    automore_enable(NULL);
    printf("\nThis peer: %i", bstun_name);
    group = GETOBJ(int,1);
    address = GETOBJ(int,2);

    first = TRUE;
    FOR_ALL_HWIDBS(idb) {
        if ((idb->enctype == ET_BSTUN) && (idb->bstun_group) 
          &&((group == 0) || (group == idb->bstun_group))) {
           if (first) {
                first = FALSE;
           } else {
	       automore_conditional(0);
	   }
           bstun_show_print(idb, address);
       }
    }
    automore_disable();
}

/*
 * Shut down BSTUN processing on an interface.
 */
static void bstun_nail_priority_peers (bstunpeertype *stp)
{
    int i;
    bstunpeertype *stphold;
 
    if (!stp)
        return;
    if (stp->priority) {
        for (i = 1; i <= BSTUN_PRIORITY_COUNT; i++) {
            stphold = stp->priority_peers[i];
            if (stphold)
                bstnail_peer(stphold);
        }
    }
    bstnail_peer(stp);

} /* End stun_nail_priority_peers() */

static void
bstun_shutintcore (hwidbtype *idb,
                   boolean kill_procs)
{
    leveltype status;
    bstunpeertype *stp;

    if(idb->bstun_group != 0) {
       status = raise_interrupt_level(ALL_DISABLE);
       while ((stp = dequeue(&idb->bstun_Q))) {
          bstun_nail_priority_peers(stp);
       }
       if (bstun_pinfo[idb->bstun_group]) {
          if (bstun_pinfo[idb->bstun_group]->shut_interface)
             (*bstun_pinfo[idb->bstun_group]->shut_interface)(idb);
       }
       idb->bstun_group = 0;
       if ((kill_procs) && (bstun_ifcnt > 0)) {
          bstun_ifcnt--;
          if (bstun_ifcnt == 0) {
             process_kill(bevent_pid);
             process_kill(bstun_pid);
          }
       }
       reset_interrupt_level(status);
    }
}

void
bstun_shutinterface (hwidbtype *idb,
		     boolean kill_procs)
{
    hwidbtype *tidb;

    bstun_shutintcore(idb, kill_procs);
    FOR_ALL_HWIDBS(tidb) {
	if (tidb->bstun_Q.qhead) {
	    break;
	}
    }
    bstun_first_idb = tidb;
    return;
}

/*
 * Shut down BSTUN processing on *all* interfaces.
 */
static void
bstun_shutallinterfaces (
    int group)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
	if ((group < 0) || (group == idb->bstun_group)) {
	    bstun_shutintcore(idb, KILL_BSTUN_PROCS);
	}
    }
    bstun_first_idb = NULL;
}

static void
bstun_startinterface (hwidbtype *idb,
		      boolean create_procs)
{

    if ((create_procs) && (bstun_ifcnt == 0)) {
       mgd_timer_init_parent(&bevent_timer, NULL);
       bevent_pid = process_create(bevent_background, "BSTUN Event-Process",
                                   LARGE_STACK, PRIO_NORMAL);
       bstun_pid = process_create(bstun_background, "BSTUN Background",
                                   LARGE_STACK, PRIO_NORMAL);
    }
    bstun_ifcnt++;
    if (bstun_pinfo[idb->bstun_group]) {
	if (bstun_pinfo[idb->bstun_group]->start_interface)
           (*bstun_pinfo[idb->bstun_group]->start_interface)(idb);
    }

}  /* bstun_startinterface. */


/*
 * bstun_command
 * global bstunnel commands.
 */

void
bstun_command (parseinfo *csb)
{
    ulong tbstun_name;		/* Don't change name until interface shut! */
    bstun_protocol_list *spl;
    int group;
    static char *no_bstun =
	"\nBSTUN not enabled -- give a bstun peer-name first.";
    uint i;

    if (csb->nvgen) {
	int i;

	switch (csb->which) {
	  case BSTUN_PEER:
	    nv_write(bstun_name != (ipaddrtype) 0, "!\n%s %i",
		     csb->nv_command, bstun_name);
	    break;
	  case BSTUN_PROTOCOL:
	    for (i = 0; i <= BSTUN_MAX_GROUP; i++) {
                if (bstun_pinfo[i] != NULL) {
        		   nv_write(TRUE, "%s %d %s",
			    csb->nv_command, i, bstun_pinfo[i]->name);
		}
	    }
	    break;
          case BSTUN_KEEPALIVE:
            if (bstun_keepalive_interval) {
               if (bstun_keepalive_interval != BSTUN_KEEPALIVE_DEFAULT_INTERVAL)
                  nv_write(TRUE, "%s %d", csb->nv_command,
                           bstun_keepalive_interval/ONESEC);
               else
                  nv_write(TRUE, "%s", csb->nv_command);
	    }
	    break;
	  case BSTUN_KEEPALIVE_COUNT:
            if (bstun_keepalive_interval) {
               nv_write(bstun_keepalive_count != BSTUN_KEEPALIVE_DEFAULT_COUNT,
	             "%s %d", csb->nv_command, bstun_keepalive_count);
	    }
	    break;
	}
	return;
    }

    switch (csb->which) {

      case BSTUN_PEER:
	if (csb->sense) {
	    tbstun_name = GETOBJ(paddr,1)->ip_addr;
	} else {
            /* Remove peer-name only if there no priority-lists using bstun */
            if (prilist_uses_bstun_group(0)) {
               printf("\nFirst remove the priority-list containing a reference t
o a bstun group");
               return;
            }
            tbstun_name = (ipaddrtype) 0;
	}
	if (tbstun_name == (ipaddrtype) 0) {
            /*
             * When removing a peer-name, the following bstun global
             * parameters must also be removed :
             *  --bstun protocol-group  --bstun keepalive-count
	     *
             * Calling bstun_shutallinterfaces(-1) cleans up each bstun
	     * interface.
             */
	    bstun_shutallinterfaces(-1);
            bstun_keepalive_count = BSTUN_KEEPALIVE_DEFAULT_COUNT;
            bstun_keepalive_interval = 0;
            for (i = 0; i <= BSTUN_MAX_GROUP; i++)
               bstun_pinfo[i] = NULL;
	}
	bstun_name = tbstun_name;
	break;

      case BSTUN_PROTOCOL:
	if (!bstun_name) {
	    printf(no_bstun);
	    break;
	}
	group = GETOBJ(int,1);
	spl = (bstun_protocol_list *) find_namedthing(&bstun_pinfoQ,
						     GETOBJ(string,1));
	if (!spl) {
	    printf("\nunknown protocol - %s", GETOBJ(string,1));
	    return;
	}
	if (csb->sense) {
	    if ((bstun_pinfo[group] != NULL) && (bstun_pinfo[group] != spl->q)) {
		printf("\nold protocol-group for %d was %s.", group,
		       bstun_pinfo[group]->name);
		printf("\nOld BSTUN information for interfaces in group %d being deleted",
		       group);
		bstun_shutallinterfaces(group);
	    }
            bstun_pinfo[group] = spl->q;

	} else {
            if (prilist_uses_bstun_group(group)) {
               printf("\nFirst remove the priority-list containing a reference t
o bstun group %d",
                      group);
               return;
            }
	    bstun_shutallinterfaces(group);
	    bstun_pinfo[group] = NULL;
	}
	break;

      case BSTUN_KEEPALIVE:
        if (csb->sense) {
           hwidbtype *idb;
           bstun_keepalive_interval = GETOBJ(int,1) * ONESEC;
           for(idb = bstun_first_idb; idb; idb = idb->next) {
              bstun_protocol_info *gpi;    
              bstunpeertype *stp;
              if ((gpi = bstun_pinfo[idb->bstun_group]) == NULL)
                  continue;
              for (stp = idb->bstun_Q.qhead; stp; stp = stp->next) {
		  GET_TIMESTAMP(stp->last_keepalive);
              }
           }
        }
        else {          
           bstun_keepalive_interval = 0;
        }
 
        /*---- Inform bstun background of configuration change. */
        process_set_boolean(keepalive_cfg_request_p, TRUE);
        break;

      case BSTUN_KEEPALIVE_COUNT:
        if (csb->sense) {
           bstun_keepalive_count = GETOBJ(int,1);
        }
        else {
           bstun_keepalive_count = BSTUN_KEEPALIVE_DEFAULT_COUNT;
        }
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}



static bstunpeertype *
bstun_create_peer (
    hwidbtype *idb,
    boolean allchosen,
    bstuntypetype type,
    boolean priority,
    uchar *addr,
    int port,
    ipaddrtype ipaddr,
    hwidbtype *outidb,
    boolean input_addr_inuse,
    uchar *input_addr)
{
    bstunpeertype *stp;
    uint i;

    stp = malloc_named(sizeof(bstunpeertype), "bstun-peer");
    if (!stp) {
	printf(nomemory);
	return(NULL);
    }
    stp->group = idb->bstun_group;
    stp->all_addr = allchosen;

    if (stp->all_addr) {
       stp->arsl_p = malloc_named(sizeof(bstunaddrroutestatelist_t),
				  "bstun-peer: addr-route-state-list");
       if (!stp->arsl_p) {
          printf(nomemory);
	  return(NULL);
       }
    }

    stp->bstun_idb = idb;
    stp->type = type;
    stp->priority = priority;
    GET_TIMESTAMP(stp->last_keepalive);
    bcopy(addr, stp->addr, bstun_pinfo[stp->group]->addrlength);
    stp->input_addr_inuse = input_addr_inuse;
    if (input_addr_inuse) {
        bcopy(input_addr, stp->input_addr,
	      bstun_pinfo[stp->group]->addrlength);
    }
    switch (type) {
    default:
    case BSTUN_PT_TCPD:
	stp->state = BSTUN_PS_CLOSED;
	stp->sender = bsttcpd_sender;
	stp->abort =  bsttcpd_abort;
	stp->close =  bsttcpd_close;
	stp->connect = bsttcpd_connect;
	stp->port = port;
	stp->ipaddr = ipaddr;
	stp->inhdr.op = BSTUN_OP_EMPTY;
	break;
    case BSTUN_PT_IF:
        stp->state = BSTUN_PS_CONNECTED;
	stp->sender = bstif_direct;
	stp->abort =  bstif_abort;
	stp->close =  bstif_close;
	stp->connect = bstif_connect;
	stp->outidb = outidb;
	if (!supports_maxdgram(outidb)) {
	    printf("\nWarning: The maximum datagram size on interface %s will be %d bytes",
		   outidb->hw_namestring, outidb->maxmtu); 
	} else {
	    int buffersize;
	    buffersize = (outidb->maxmtu > MAXDGRAM_BSTUN) ?
		outidb->maxmtu : MAXDGRAM_BSTUN;
	    if (!(*outidb->set_maxdgram)(outidb, buffersize,
					 (buffersize +
					  HDLC_HEADER_SIZE))) {
		printf("\nWarning: The maximum datagram size on interface %s will be %d bytes",
		       outidb->hw_namestring, outidb->maxmtu);
	    }
	}
	break;
      case BSTUN_PT_TEST:
	stp->state = BSTUN_PS_CONNECTED;
        if (stp->all_addr) {
            for (i = 0; i < BSTUN_MAX_ADDRS; i++) {
                stp->arsl_p->addr_route_state_change[i] = TRUE;
	    }
        }
        stp->addr_route_state_change = TRUE;
	stp->sender = bsttest_sender;
	stp->abort =  bsttest_abort;
	stp->close =  bsttest_close;
	stp->connect = bsttest_connect;
        break;
    }
    return(stp);
}

/*
 * bstun_parse_route
 *
 * [no] bstun {route} [all | address <addr>]
 *                   [tcp <ipaddr] | interface <xfc>]
 * [no] bstun {route} [all | address <addr>] interface <xfc>
 *
 */
static void
bstun_parse_route (parseinfo *csb, boolean gotfirst)
{
    leveltype status;
    int junk;
    bstunpeertype *stp, *stpsub, *input_stp;
    bstuntypetype type;
    hwidbtype *idb, *outidb;
    uchar addr[BSTUN_MAX_ADDR_LENGTH];
    uchar input_addr[BSTUN_MAX_ADDR_LENGTH];
    ipaddrtype ipaddr;
    boolean allchosen;
    boolean priority, input_addr_inuse;
    boolean new_tcp_qlen;
    ulong user_def_tcp_qlen;
    bstun_protocol_info* bpi;

    stp = (bstunpeertype *) NULL;
    outidb = NULL;
    ipaddr = 0;
    priority = input_addr_inuse = FALSE;
    idb = csb->interface->hwptr;
    user_def_tcp_qlen = BSTUN_MAX_QLEN; /* default if not user defined */
    new_tcp_qlen = FALSE;

    if (idb->bstun_group == 0) {
	printf("\nA bstun group must be defined for this interface before using this command.");
	return;
    }

    bpi = bstun_pinfo[idb->bstun_group];
    if (bpi == NULL) {
	printf("\nNo group-protocol defined for group %d",
	       idb->bstun_group);
	return;
    }

    if (GETOBJ(int,1) == BSTUN_RTALL) {
	allchosen = TRUE;
    } else {
	bcopy((char *)GETOBJ(paddr,1)->bstun_addr, (char *)addr,
	      GETOBJ(paddr,1)->length);
	allchosen = FALSE;
    }

    /* make sure that "bstun route XXX" is valid with bstun protocol */
    if ((allchosen && bpi->neverall) ||
        (!allchosen && bpi->onlyall) )
      {
      printf("\nThis command not valid with bstun protocol \"%s\".",
	     bpi->name);
      return;
      }

    /*---- Call protocol specific routine to validate bstun route address. */
    if ((!allchosen) && (bpi->validate_address)) {
       if (!(*bpi->validate_address)(idb, GETOBJ(paddr,1)->bstun_addr,
				     GETOBJ(paddr,1)->length)) {
          printf("\nInvalid address for bstun protocol %s.",
		 bpi->name);
	  return;
       }
    }

    junk = GETOBJ(int,2);
    if (junk == BSTUN_TCP) {
	type = BSTUN_PT_TCPD;
	ipaddr = GETOBJ(paddr,2)->ip_addr;
        if (GETOBJ(int,4)) {
            priority = TRUE;
        }
	if (GETOBJ(int,5)) {
	    bcopy((char *)GETOBJ(paddr,3)->bstun_addr, (char *)input_addr,
		  GETOBJ(paddr,3)->length);
	    input_addr_inuse = TRUE;
	    for (input_stp = (bstunpeertype *) idb->bstun_Q.qhead; input_stp;
		input_stp = input_stp->next) {
		if (!bcmp(input_addr, input_stp->addr,
			  bstun_pinfo[input_stp->group]->addrlength) ||
		    (stp->input_addr_inuse && 
		     !bcmp(input_addr, input_stp->input_addr,
			   bstun_pinfo[input_stp->group]->addrlength)))
		    {
		        printf("\n bstun route input-address already in use");
			return;
		    }
	    }
	}
        if (GETOBJ(int,6)) {
            user_def_tcp_qlen = GETOBJ(int,6);
            new_tcp_qlen = TRUE;
        }
    }else if (junk == BSTUN_TEST) {
	type = BSTUN_PT_TEST;
    }else{
        if (bstun_pinfo[idb->bstun_group]->local_ack_capable) {
	   printf("\nProtocol %s cannot be used on direct interfaces.",
		  bstun_pinfo[idb->bstun_group]->name);
	   return;
	}
	type = BSTUN_PT_IF;
	outidb = hwidb_or_null(GETOBJ(idb,1));
	csb->interface = idb->firstsw;
	if (outidb == idb) {
	    printf("\nSpecified interface and current interface must be different");
	    return;
	}
	if ((outidb->status & IDB_SERIAL) == 0) {
	    printf("\nSpecified interface must be a serial interface");
	    return;
	}
    }
    /*
     * see if the peer already exists.  If we are looking for the wild card
     * address then just check the last entry.
     */

    if (allchosen) {
	stp = (bstunpeertype *) idb->bstun_Q.qtail;
	if (stp && !stp->all_addr)
	    stp = NULL;
    } else {
	/*
	 * Sigh.  Not a wild card.  Must search.
	 */
	for (stp = (bstunpeertype *) idb->bstun_Q.qhead; stp; stp = stp->next) {
	    if (stp->input_addr_inuse &&
		!bcmp(addr, stp->input_addr,
		      bstun_pinfo[stp->group]->addrlength)) {
	        printf("\nbstun route address already used as an input-address");
		return;
	    }
	}
	for (stp = (bstunpeertype *) idb->bstun_Q.qhead; stp; stp = stp->next) {
	    if ((!bcmp(addr, stp->addr, bstun_pinfo[stp->group]->addrlength)) &&
                ((stp->type == BSTUN_PT_IF) ? (stp->outidb == outidb) :
                 (stp->ipaddr == ipaddr)))

	      break;
	}
    }
    if (csb->sense) {
	/*
	 * creating or changing a bstunnel peer.  If it already
	 * exists than check for identical (nothing to do then)
         * or something different (get rid of old first).
	 */
	if (stp) {
	    /* Same thing as before? If so, just return... */
	    if ((type == stp->type) && (stp->group == idb->bstun_group) &&
		(stp->bstun_idb == idb) &&
		(!bcmp(addr, stp->addr, bstun_pinfo[stp->group]->addrlength)) &&
		((stp->type == BSTUN_PT_IF) ? (stp->outidb == outidb) :
		 (stp->ipaddr == ipaddr))) {
		/* Yes, it is, but change certain parameters... */
                if (stp->priority != priority) {
                    printf("\nsorry, you cannot change priority usage without first removing the route");
                    return;
                }
		if (stp->input_addr_inuse != input_addr_inuse) {
		    printf("\nsorry, you cannot change the input address without first removing the route");
		    return;
		}
                if (new_tcp_qlen) {
                    printf("\nsorry, you cannot change the tcp-queue-max without first removing the route");
                    return;
                }
		if (input_addr_inuse) {
		    if (bcmp(input_addr, stp->input_addr,
			     bstun_pinfo[stp->group]->addrlength)) {
		        printf("\nbstun route input-address already set to a different value. Command ignored");
			return;
		    }
		}

		return;
	    }
	    /* nail the old peer */
	    status = raise_interrupt_level(NETS_DISABLE);
	    unqueue(&idb->bstun_Q,  stp); /* remove from queue */
	    bstun_nail_priority_peers(stp);
	    reset_interrupt_level(status);
	}

	/* let's now create it. */
	stp = bstun_create_peer(idb, allchosen, type, priority,
			       addr, BSTUN_P0_PORT, ipaddr, outidb,
			       input_addr_inuse, input_addr);
	if (!stp)
	    return;
        stp->priority_top = NULL;
        stp->priority_peers[0] = stp;
        stp->user_def_max_tcp_qlen = user_def_tcp_qlen;
        if (priority) {
            stpsub = bstun_create_peer(idb, allchosen, type, FALSE,
                                       addr, BSTUN_P1_PORT, ipaddr, 
                                       outidb, input_addr_inuse, input_addr);
            if (!stpsub)
                return;
            stp->priority_peers[1] = stpsub;
            stpsub->user_def_max_tcp_qlen = user_def_tcp_qlen;
            stpsub->priority_top = stp;
            stpsub = bstun_create_peer(idb, allchosen, type, FALSE,
                                       addr, BSTUN_P2_PORT, ipaddr, 
                                       outidb, input_addr_inuse, input_addr);
            if (!stpsub)
                return;
            stp->priority_peers[2] = stpsub;
            stpsub->user_def_max_tcp_qlen = user_def_tcp_qlen;
            stpsub->priority_top = stp;
            stpsub = bstun_create_peer(idb, allchosen, type, FALSE,
                                       addr, BSTUN_P3_PORT, ipaddr, 
                                       outidb, input_addr_inuse, input_addr);
            if (!stpsub)
                return;
            stp->priority_peers[3] = stpsub;
            stpsub->user_def_max_tcp_qlen = user_def_tcp_qlen;
            stpsub->priority_top = stp;
        }
	if (!allchosen || idb->bstun_Q.qhead == NULL) {
	    p_requeue(&idb->bstun_Q,  stp);
	} else {
	    /*
	     * addr is everything and there is already something on the
	     * queue.
	     */
	    ((bstunpeertype *) (idb->bstun_Q.qtail))->next = stp;
	    idb->bstun_Q.qtail =  stp;
	}
	idb->nokeepalive = TRUE;
	if (stp->type == BSTUN_PT_TCPD) {
	    reg_invoke_ip_tcpdriver_start();
	}
    } else {
	/* delete entry if it exists */
	if (!stp)
	    return;				/* doesn't exist */
	status = raise_interrupt_level(NETS_DISABLE);
	unqueue(&idb->bstun_Q,  stp); /* remove from queue */
	bstun_nail_priority_peers(stp);
	reset_interrupt_level(status);
    }
    FOR_ALL_HWIDBS(idb) {
	if (idb->bstun_Q.qhead) {
	    break;
	}
    }
    bstun_first_idb = idb;
}

/*
 * bstun_parse_group
 *
 * [no] bstun group <n>
 *
 */
static void
bstun_parse_group (parseinfo *csb)
{

    hwidbtype *hwidb = hwidb_or_null(csb->interface);
    if (bstun_pinfo[GETOBJ(int,1)] == NULL) {
	printf("\nNo group-protocol defined for group %d", GETOBJ(int,1));
	return;
    }
    if (!csb->sense) {
	bstun_shutinterface(hwidb, KILL_BSTUN_PROCS);
	hwidb->bstun_group = 0;
	return;
    }

    if (hwidb->bstun_group) {
        if(GETOBJ(int,1) != hwidb->bstun_group) {
            printf("\nnew BSTUN group number for interface %s. Clearing BSTUN info.",
                   csb->interface->namestring);
            bstun_shutinterface(hwidb, KILL_BSTUN_PROCS);
        }
        else {
            return ;
        }
    }

    hwidb->bstun_group = GETOBJ(int,1);
    bstun_startinterface(hwidb, CREATE_BSTUN_PROCS);
    return;
}

/*
 * bstunif_command
 * configure the serial tunnel.  Commands are as follows:
 *
 * Note that bstun_command has already done the first call to get_word.
 */
void
bstunif_command (parseinfo *csb)
{
    hwidbtype *idb = hwidb_or_null(csb->interface);
    
    if (!bstun_name) {
	if (!csb->nvgen) {
	    printf("\nBSTUN not enabled -- give a bstun peer-name first.");
	}
	return;
    }
    if ((idb->status & IDB_SERIAL) == 0) {
	if (!csb->nvgen) {
	    printf("\nbstun only allowed on Serial interfaces");
	}
	return;
    }
    if (idb->enctype != ET_BSTUN && csb->sense) {
	if (!csb->nvgen) {
	    printf("\nbstun requires BSTUN encapsulation on interface");
	}
	return;
    }
    if (!idb->nokeepalive) {
	if (!csb->nvgen) {
	    printf("\nbstun requires keepalives to be disabled on interface");
	}
	return;
    }

    if (csb->nvgen) {
        bstunpeertype *stp;
        char buff[BSTUN_MAX_PEER_NAME];

	if (idb->bstun_group == 0) {
	    return;
	}

	switch (csb->which) {
	  case BSTUNIF_GROUP:
	    nv_write(TRUE, "%s %u", csb->nv_command, idb->bstun_group);
	    break;
	  case BSTUNIF_ROUTE:
	    for (stp = (bstunpeertype *) idb->bstun_Q.qhead; stp; stp = stp->next) {
		nv_write(TRUE, "%s ", csb->nv_command);
		bstun_sprintf_addr(stp->addr, stp->all_addr, stp->group, buff);
		if (stp->all_addr) {
		    nv_add(TRUE, "%s ", buff);
		} else {
		    nv_add(TRUE, "address %s ", buff);
		}
		switch (stp->type) {
		  case BSTUN_PT_TCPD:
		    nv_add(TRUE, "tcp %i", stp->ipaddr);
                      if (stp->priority)
                          nv_add(TRUE, " priority");
                      if (stp->input_addr_inuse) {
                          bstun_sprintf_addr(stp->input_addr, FALSE, 
                                            stp->group, buff);
                          nv_add(TRUE, " input-address %s ", buff);
                      }
                      if (stp->user_def_max_tcp_qlen != BSTUN_MAX_QLEN) {
                          nv_add(TRUE, " tcp-queue-max %d", 
                                   stp->user_def_max_tcp_qlen);
                      }
		    break;
		  case BSTUN_PT_IF:
		    nv_add(TRUE, "interface %s", stp->outidb->hw_namestring);
		    break;
                  case BSTUN_PT_TEST:
		    nv_add(TRUE, "test");
                    break;
		  default:
		    buginf("\nBSTUN: illegal BSTUN peer");
		    break;
		}
	    }
	    break;
	  case NO_BSTUN:
	    /* No NV generation */
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case NO_BSTUN:
        bstun_shutinterface(idb, KILL_BSTUN_PROCS);
	idb->bstun_group = 0;
	break;
      case BSTUNIF_GROUP:
	bstun_parse_group(csb);
	break;
      case BSTUNIF_ROUTE:
	bstun_parse_route(csb, FALSE);
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}



/*********************************************************************
 *
 * BSC Test implementations
 * 
 *********************************************************************/

/*
 * bsttest_sender
 * this is the sender for test                    
 */

static boolean
bsttest_sender (bstunpeertype *inpeer, paktype *pak, hwidbtype *outidb,
		       boolean force, bstunpeertype **outpeer,
		       boolean usethispeer, boolean toallpp)
{
    uchar cmd;
    static uchar *const bsend_cmd_name[] = {
      "ADD",
      "DELETE",
      "ACTIVE",
      "INACTIVE",
      "POLL_DATA_ETB",
      "POLL_DATA_ETX",
      "SPLIT_POLL_DATA",
      "DEVICE_STATUS_DATA",
      "SELECT_DATA_ETB",
      "SELECT_DATA_ETX",
      "SPLIT_SELECT_DATA",
      "POLL_DATA_ABORT",
      "SEL_DATA_ABORT",
      "DEVICE_DO_WACK",
      "DEVICE_DONT_WACK" };

    cmd = pak->datagramstart[8+1];
    if ( (cmd == SELECT_DATA_ETX) || (cmd == POLL_DATA_ETX) ) {
      buginf( "\n%s, Bstun group %d   : Received BSTUN command %s",
              inpeer->bstun_idb->hw_namestring,
              inpeer->group, 
              bsend_cmd_name[pak->datagramstart[8+1]-POLL_ADDR_ADD]);
    } else {
      buginf( "\n%s, Bstun group %d   : Sending BSTUN command %s",
              inpeer->bstun_idb->hw_namestring,
              inpeer->group, 
              bsend_cmd_name[pak->datagramstart[8+1]-POLL_ADDR_ADD]);
    }
    buginf( "\nBSC Control unit address : %02x",
            pak->datagramstart[8+0] );
    switch(cmd) {
    case SELECT_DATA_ETX :
    case POLL_DATA_ETX : 
      buginf( "\nBSC Device address       : %02x",
              pak->datagramstart[8+2]);
      buginf( "\nBSC Text");
      memory_dump(&(pak->datagramstart[8+3]),
	          &(pak->datagramstart[8+3]) + pak->datagramsize-(8+3+1),
		  FALSE,
		  MEMORY_DUMP_DATA_TYPE_ASCII,
		  buginf);
      break;
    default:
      break;
    }
    datagram_done(pak);
    return(TRUE);
}

/*
 * bsttest_abort
 * 
 */

static void
bsttest_abort (bstunpeertype *stp)
{
    /*
     * nothing to abort
     */   
}

/*
 * bsttest_close
 * 
 */
static void
bsttest_close (bstunpeertype *stp, ulong param)
{
    /*
     * no way to close the connection 'cos it's not a real connection
     */    
}  

/*
 * bsttest_connect
 * 
 */
static bstunconnrtn
bsttest_connect (bstunpeertype *stp)
{
    return(BSTUN_CONN_CONNECTED);
}

/*
 * BSTUN subsystem header
 */

#define BSTUN_MAJVERSION 1
#define BSTUN_MINVERSION 0
#define BSTUN_EDITVERSION  1

SUBSYS_HEADER(bstun, BSTUN_MAJVERSION, BSTUN_MINVERSION, BSTUN_EDITVERSION,
	      bstun_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
              subsys_req_iphost);

