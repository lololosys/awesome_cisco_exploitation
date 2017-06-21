/* $Id: stun.c,v 3.12.8.8 1996/08/28 12:47:09 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/stun.c,v $
 *------------------------------------------------------------------
 * stun.c -- Serial Tunnel
 * 
 * July 24, 1990 - Eric B. Decker
 * 1991 - Joel P. Bion
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stun.c,v $
 * Revision 3.12.8.8  1996/08/28  12:47:09  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.12.8.7  1996/08/26  19:37:39  athippes
 * CSCdi62480:  AGS+ Crash
 * Branch: California_branch
 *
 * Revision 3.12.8.6  1996/07/17  00:00:13  snyder
 * CSCdi63174:  one blank == 116 bytes, another == 108 bytes
 * Branch: California_branch
 *              let the optimizer do its thing
 *
 * Revision 3.12.8.5  1996/07/02  23:11:00  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.12.8.4  1996/06/27  09:31:14  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.12.8.3  1996/05/17  11:21:41  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.4.4  1996/04/26  07:54:41  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.11.4.3  1996/04/11  14:38:01  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.4.2  1996/04/03  14:34:58  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.4.1  1996/03/17  17:50:33  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.12.8.2  1996/03/23  01:31:33  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.12.8.1  1996/03/18  20:12:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.4  1996/03/16  06:53:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.3  1996/03/13  01:23:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/03/07  09:42:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  00:55:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.12  1996/02/29  00:42:42  vrawat
 * CSCdi49966:  tcp-queue-max xxx, under stun route, does not show up in
 * the config
 *
 * Revision 3.11  1996/02/08  18:02:00  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.10  1996/02/07  16:12:06  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.9  1996/02/07  00:46:49  irfan
 * CSCdi48054:  async interfaces should default to no keepalive
 * Allow hwidb's to have default other than KEEPALIVE_DEFAULT.
 *
 * Revision 3.8  1996/02/01  06:03:30  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/29  07:28:02  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.6  1996/01/18  22:51:55  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/16  03:37:20  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.4  1995/12/08  22:40:52  vrawat
 * CSCdi43686:  After reload SDLC packets identified as HDLC by serial
 * driver
 *
 * Revision 3.3  1995/11/17  09:24:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:36:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:46:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/09/13  21:14:09  vrawat
 * CSCdi36577:  clsdriver code does CLSOpen(), when not configured
 *
 * Revision 2.10  1995/09/09  00:51:54  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.9  1995/08/08  16:47:21  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.8  1995/08/07 05:35:34  richl
 * CSCdi38315:  makefile rework
 *              move utility routines to libibm_util.a
 *
 * Revision 2.7  1995/08/01  02:32:15  vrawat
 * CSCdi37993:  fix stun problems found at customer site
 *
 * Revision 2.6  1995/07/28  22:05:29  vrawat
 * CSCdi36957:  Aborted SDLC STUN frames transmitted across local-ack
 * connection
 *
 * Revision 2.5  1995/07/27  03:11:09  vrawat
 * CSCdi33572:  AS/400 sdlc multidrop enviromnment
 *
 * Revision 2.4  1995/07/06  01:32:48  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.3  1995/06/28 09:25:59  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/18  23:36:55  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:48:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ctype.h>
#undef   toupper                        /* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"  /* -> _generic when frame_relay mess sorted out */
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "logger.h"
#include "../ibm/msg_stun.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "parser_defs_stun.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "ibm.h"
#include "stun.h"
#include "ibm_externs.h"
#include "../tcp/tcpdriver.h"
#include "../wan/frame_relay.h"  /* only for DLCI_TO_NUM ! */
#include "../if/network.h"
#include "../if/tring.h"
#include "sna_la_pri.h"
#include "sdlc.h"
#include "sdlc_debug.h"
#include "stun_sdlc_public.h"
#include "slack.h"
#include "ibm_public.h"
#include "../srt/rsrb.h"
#include "../if/priority.h"
#include "../parser/parser_defs_priority.h"
#include "subsys.h"
#include "stun_clslack.h"
#include "sr_stunmib.h"
#include "../util/random.h"
#include "../ibm/sna_util.h"
#include "../ibm/libibm_externs.h"
#include "../parser/parser_defs_parser.h"

extern boolean IsClsdriverRunning;
extern byte UserConfiguredLisnSap;

/*
 * Global Storage
 */
queuetype stun_pinfoQ;
hwidbtype *stun_first_idb;		/* first idb with stun structs */
ipaddrtype stun_name;			/* this peer's name */
queuetype stconn_pakQ;
int stun_pid;
sys_timestamp stun_lastframe;
stun_protocol_info *stun_pinfo[STUN_MAX_GROUP + 1];
uchar stun_debug_group;
uchar stun_debug_addr[STUN_MAX_ADDR_LENGTH];
boolean stun_debug_addr_significant;
queuetype stun_tg_infoQ;
ulong stun_keepalive_interval;          /* How long to wait before timing out a peer */
ulong stun_keepalive_count;             /* How many misses before death? */
sys_timestamp stun_next_keepalive;      /* when to wake up again for keepalive purposes */
ulong stun_slack_fsm_timeout;          /* FSM wait timeout   */
sys_timestamp stun_start_time;
boolean quick_response = FALSE;

/*********************************************************************
 *
 * Low level routines.
 *
 *********************************************************************/

static boolean stun_add_tg (ulong group)
{
    stun_tg_info *tgi;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);
    /*
     * Search stun_tg_infoQ for the definition of this group.
     */
    for (tgi = (stun_tg_info *) stun_tg_infoQ.qhead; tgi; tgi = tgi->next) {
         if (tgi->group == group) {
             reset_interrupt_level(status);
             return (TRUE);
         }
    }

    /*
     * TG protocol group is not yet defined.
     */
    tgi = malloc(sizeof(stun_tg_info));
    if (tgi) {
        tgi->next = NULL;
        tgi->group = group;
        tgi->tg_wait_for_zero = FALSE;
        tgi->tg_stp = NULL;
        queue_init(&tgi->tg_reseqQ, TG_MAX_RESEQ_QSIZE);
        enqueue(&stun_tg_infoQ, tgi);
        reset_interrupt_level(status);
        return (TRUE);
    }
    reset_interrupt_level(status);
    return (FALSE);
}

static void stun_delete_tg (ulong group)
{
    stun_tg_info *tgi;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);
    /*
     * Search stun_tg_infoQ for the definition of this group.
     */
    for (tgi = (stun_tg_info *) stun_tg_infoQ.qhead; tgi; tgi = tgi->next) {
         if (tgi->group == group) {
             unqueue(&stun_tg_infoQ, tgi);
             stun_cos_flush_reseq(&tgi->tg_reseqQ);
             free(tgi);
             break;
         }
    }
    reset_interrupt_level(status);
}

static void stun_add_protocol (
    stun_protocol_info *spi,
    char *name)
{
    stun_protocol_list *spl;
    stun_protocol_info *old;

    spl = find_namedthing(&stun_pinfoQ, name);
    if (spl == NULL) {
	spl = malloc(sizeof(stun_protocol_list));
	if (spl && spi) {
	    sstrncpy(spl->name, name, NAMEDTHINGNAMELEN+1);
	    enqueue(&stun_pinfoQ, spl);
	    spl->q = spi;
            spl->next = NULL;
	    return;
	}
    } else {
	if (spi) {
	    old = (stun_protocol_info *) spl->q;
	    spl->q = spi;
	    free(old);
	    return;
	}
    }
}

#ifdef NOBODY_CALLS_ME
void stun_delete_protocol (
    char *name)
{
    stun_protocol_list *spl;
    
    spl = find_namedthing(&stun_pinfoQ, name);
    if (spl != NULL) {
	unqueue(&stun_pinfoQ, spl);
	free(spl->q);
	free(spl);
    }
}
#endif /* NOBODY_CALLS_ME */


/*
 * stun_parse_packet
 *
 * Set up a packet buffer so that the network level protocol fields
 * are initialized.
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 */

static iqueue_t stun_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    if (!pak->if_input) {
	errmsg(&msgsym(NOINPIDB, STUN));
	return(NULL);
    }

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
    pak->riflen        = 0;
    pak->info_start    = NULL;
    pak->network_start = pak->datagramstart;

    /*
     * Um. I guess the encsize is zero. Or something.
     */
    pak->encsize = 0;
    return(stun_input);
}

/*
 * stun_setup
 * Set up a serial interface to use STUN
 */
static boolean stun_setup (parseinfo *csb, hwidbtype *idb)
{
    int buffersize;
    idbtype *swidb;
    serialsb *ssb;

    if (!idb->sdlc_data) {
	idb->sdlc_data = malloc(sizeof(sdlc_data_block *) * 256);
	if (!idb->sdlc_data) {
	    printf(nomemory);
	    return (FALSE);
	}
    }
    idb->nokeepalive = TRUE;
    idb->vencap = (vencap_t)return_false;
    idb->deencap = NULL;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    idb->getlink = hdlc_getlink;
    idb->oqueue = holdq_enqueue;
    idb->oqueue_dequeue = holdq_dequeue;
    idb->iqueue = stun_input;
    idb->parse_packet = stun_parse_packet;
    idb->show_support = sdlc_interface;
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
    idb->enctype = idb->enctype_def = idb->enctype_cfg = ET_STUN;
    idb->encsize = 0;
    idb->lineaction = sdlc_cstate;
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
     * Set up the maximum buffersize for this link (STUN gets a bit more...)
     */
    if (!idb->set_maxdgram) {
	printf("\nWarning: The maximum datagram size on interface %s will be %d bytes",
	       idb->hw_namestring, idb->maxmtu); 
    } else {
	buffersize = (idb->maxmtu > MAXDGRAM_STUN) ?
	  idb->maxmtu : MAXDGRAM_STUN;
	if (!(*idb->set_maxdgram)(idb, buffersize, (buffersize +
						    HDLC_HEADER_SIZE))) {
	    printf("\nWarning: The maximum datagram size on interface %s will be %d bytes",
			   idb->hw_namestring, idb->maxmtu);
	}
    }
    /* Clear out any info */
    stun_shutinterface(idb);
    (*(idb->reset))(idb);
    return(TRUE);
}

/*
 * stun_show_priority_queue
 */
static boolean stun_show_priority_queue (plisttype *plist,
					 boolean custom,
					 int i)
{
    char buff[MAX_PEER_NAME];

    if (plist->stun_group) {
	printf("\n%5d  %6s stun %d ", i,
	       queue2name(plist->priority, custom),
	       plist->stun_group);
	stun_sprintf_addr(plist->sdlc_addr, 0,
			  plist->stun_group, buff);
	printf("%s", buff);
	return(TRUE);
    }
    return(FALSE);
}

/*
 * stun_frame_macaddr
 *
 * Set braodcast flag to FALSE for now, and always return TRUE.
 * Called by MEDIA_FRAME_MACADDR, indexed by LINK_STUN.
 */
static boolean stun_frame_macaddr (
    paktype *pak,
    long address,
    addrtype *proto_addr,
    boolean *broadcast)
{
    proto_addr->type = 0;
    proto_addr->length = 0;
    proto_addr->addr.fr_address = (ushort)address;
    *broadcast = FALSE;
    return(TRUE);
}

static void stun_media_registry (void)
{
    reg_add_media_frame_macaddr(LINK_STUN, stun_frame_macaddr,
		"stun_frame_macaddr");
}

/*
 * stun_default_keepperiod
 * STUN encapsulation dependant keep alive default
 */
static boolean stun_default_keepperiod (hwidbtype *hwidb, short *keepperiod)
{
    if (hwidb && (ET_STUN == hwidb->enctype)) {
	*keepperiod = 0;
	return (TRUE);   /* say we identified it as our own */
    }
    return (FALSE);      /* let the loop continue */
}

/*
 * stun_init
 * initilize stunnel structures
 */

void stun_init (subsystype *subsys)
{
    stun_protocol_info *current;
    char *name;
    
    stun_media_registry();
    GET_TIMESTAMP(stun_lastframe);
    GET_TIMESTAMP(stun_start_time);
    queue_init(&stconn_pakQ, 0);
    queue_init(&stun_pinfoQ, 0);
    queue_init(&stun_tg_infoQ, TG_MAX_RESEQ_QSIZE);
    stun_pid = 0;
    stun_keepalive_interval = 0;
    stun_keepalive_count = STUN_KEEPALIVE_DEFAULT_COUNT;
    TIMER_START(stun_next_keepalive, STUN_KEEPALIVE_TX_PERIOD);
    stun_slack_fsm_timeout = STUN_SLACK_FSM_TIMEOUT_DEFAULT;
    if (system_loading)
	return;
    current = stun_sdlc_predefined_init(&name);
    stun_add_protocol(current, name);
    current = stun_sdlc_tg_predefined_init(&name);
    stun_add_protocol(current, name);
    current = stun_noaddress_predefined_init(&name);
    stun_add_protocol(current, name);

    /*
     * Register some functions
     */
    reg_add_raw_enqueue(LINK_STUN, stif_input, "stif_input");
    reg_add_onemin(stun_periodic, "stun_periodic");
    reg_add_media_serial_setup(ET_STUN, stun_setup, "stun_setup");
    reg_add_tcp_listen(STUN_P0_PORT, sttcpd_listen, "stun_tcp_listen");
    reg_add_tcp_listen(STUN_P1_PORT, sttcpd_listen, "stun_tcp_listen");
    reg_add_tcp_listen(STUN_P2_PORT, sttcpd_listen, "stun_tcp_listen");
    reg_add_tcp_listen(STUN_P3_PORT, sttcpd_listen, "stun_tcp_listen");
    reg_add_tcp_access_check(STUN_P0_PORT, stun_active, "stun_active");
    reg_add_tcp_access_check(STUN_P1_PORT, stun_active, "stun_active");
    reg_add_tcp_access_check(STUN_P2_PORT, stun_active, "stun_active");
    reg_add_tcp_access_check(STUN_P3_PORT, stun_active, "stun_active");
    reg_add_stun_prioritization(stun_prioritization, "stun_prioritization");
    reg_add_priority_show(stun_show_priority_queue,
			  "stun_show_priority_queue");
    reg_add_priority_protocol_parse(LINK_STUN, prilist_protocol_stun,
				    "prilist_protocol_stun");
    reg_add_priority_protocol_nv_add(LINK_STUN,
				     priority_protocol_stun_nv_add,
				     "priority_protocol_stun_nv_add");
    reg_add_priority_protocol_compare(LINK_STUN,
				      priority_protocol_stun_compare,
				      "priority_protocol_stun_compare");
    reg_add_idb_get_keep_default(stun_default_keepperiod,
				 "stun_default_keepperiod");
}

boolean stun_parse_addr_core (uchar group, char *buff, uchar *addr)
{
    char *buffs, *buffe, c;
    int shift, i;
    uchar thisone;
    ulong naddr, mask;

    if ((buff == FALSE) || (*buff == '\0'))
      return(FALSE);
    
    switch (stun_pinfo[group]->addrfmt) {
      case 'd':
      case 'o':
	if (!parse_var_unsigned(buff, stun_pinfo[group]->addrfmt,
				&naddr))
	    return(FALSE);
	for (i = stun_pinfo[group]->addrlength - 1, shift = 0, mask = 0xff;
	     i >= 0; i--, shift += 8, mask = mask << 8)
	  addr[i] = (uchar) ((naddr & mask) >> shift);
	break;
      case 'x':
	i = stun_pinfo[group]->addrlength - 1;
	buffs = buff;
	/* Skip any stupid leading 0x or 0X */
	if (*buffs == '0')
	  if ((*(buffs + 1) == 'x') || (*(buffs + 1) == 'X'))
	    buffs = buffs + 2;
	for (buffe = buffs; *(buffe + 1); buffe++)
	  ;
	/* Read in a byte per loop... */
	while (buffe >= buffs) {
	    /* First half... */
	    c = toupper(*buffe--);
	    if (!(((c >= '0') && (c <= '9')) ||
		  ((c >= 'A') && (c <= 'F'))))
	      return(FALSE);
	    thisone = ((uchar) c -
		       (((c >= '0') && (c <= '9')) ? '0' : '7'));
	    /* Odd number of digits given? */
	    if (buffe >= buffs) {
		c = toupper(*buffe--);
		if (!(((c >= '0') && (c <= '9')) ||
		      ((c >= 'A') && (c <= 'F'))))
		  return(FALSE);
		thisone |= (((uchar) c -
			     (((c >= '0') && (c <= '9')) ?
			      '0' : '7')) << 4);
	    }
	    addr[i--] = thisone;
	    if ((i < 0) && (buffe >= buffs))
	      return(FALSE);
	}
    }
    return(TRUE);
}


void stun_sprintf_addr (uchar *addr, boolean all_addr, uchar group, char *buff)
{
    ulong naddr;
    char *buffp;
    int i, shift;

    if (all_addr) {
	sprintf(buff, "all");
	return;
    }
    if (stun_pinfo[group]->addrfmt != 'x') {
	for (naddr = 0, i = stun_pinfo[group]->addrlength - 1, shift = 0;
	     i >= 0; i--, shift += 8)
	  naddr |= addr[i] << shift;
	sprintf(buff, ((stun_pinfo[group]->addrfmt == 'd') ? "%u" : "%o"),
		naddr);
	return;
    }
    for (i = 0, buffp = buff; i < stun_pinfo[group]->addrlength;
	 i++, buffp += 2)
      sprintf(buffp, ((i == 0) ? "%x" : "%02x"), addr[i]);
}

char *stpeer_id (stunpeertype *stp, char *buff)
{
    char addrbuff[80];
    char addrbuff2[80];

    if (!stp) {
	buff[0] = '\0';
	return(buff);
    }
    stun_sprintf_addr(stp->addr, stp->all_addr, stp->group, addrbuff2);
    sprintf(addrbuff, "%s[%u]", addrbuff2, stp->group);
    switch (stp->type) {
      case STUN_PT_NONE:
      case STUN_PT_TCPD:
	sprintf(buff, "(%s)%i/%d", addrbuff, stp->ipaddr, stp->port);
	break;
      case STUN_PT_IF:
	sprintf(buff, "(%s)%s", addrbuff, stp->outidb->hw_namestring);
	break;
      case STUN_PT_FR:
	sprintf(buff, "(%s)%s", addrbuff, stp->outidb->hw_namestring);
	break;
      case STUN_PT_CLS:
	sprintf(buff, "(%s)%s", addrbuff, stp->outidb->hw_namestring);
	break;
      default:
	break;
    }
    return(buff);
}

char *stpeer_state (stunpeertype *stp)
{
    switch (stp->state) {
      case STUN_PS_DEAD:	return("dead");
      case STUN_PS_CLOSED:	return("closed");
      case STUN_PS_OPENING:	return("opening");
      case STUN_PS_OPENWAIT:	return("open wait");
      case STUN_PS_CONNECTED:	return("open");
      case STUN_PS_DIRECT:	return("direct");
      default:			return("unknown");
    }
}

static char *st_state (
    stunstatetype state)
{
    switch (state) {
      case STUN_PS_DEAD:	return("dead");
      case STUN_PS_CLOSED:	return("closed");
      case STUN_PS_OPENING:	return("opening");
      case STUN_PS_OPENWAIT:	return("open wait");
      case STUN_PS_CONNECTED:	return("open");
      case STUN_PS_DIRECT:	return("direct");
      default:			return("unknown");
    }
}

static char *stun_priority_peername (
    int priority_num)
{
    switch (priority_num) {
      case 0:
        return("   High");
	break;
      case 1:
        return(" Medium");
	break;
      case 2:
	return(" Normal");
	break;
      case 3:
	return("    Low");
	break;
      default:
	return("Unknown");
	break;
    }
}


static stunpeertype *stfr_findpeer (uchar group, uchar *addrp, idbtype *swidb, paktype *pak)
{
    stunpeertype *stp;
    hwidbtype *in_idb = swidb->hwptr;
    hwidbtype *idb = NULL;
    stun_protocol_info *gpi;    
    fr_idb_struct_type *fr_idb;
    ushort dlci;

    fr_idb = in_idb->frame_relay_stuff;
    dlci = DLCI_TO_NUM(*((ushort *)pak->datagramstart));

    idb = in_idb ? in_idb->stun_if_last : in_idb;

    if ((idb) && ((gpi = stun_pinfo[idb->stun_group]) != NULL))
      for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
	  if ((stp->type != STUN_PT_FR) || 
			(stp->outswidb != swidb) || (stp->dlci != dlci))
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
    for (idb = stun_first_idb; idb; idb = IDB_NEXT(idb)) {
	if (!idb->stun_Q.qhead)
	    continue;
	if ((gpi = stun_pinfo[idb->stun_group]) == NULL)
	  continue;
	for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
	    if ((stp->type != STUN_PT_FR) || 
			(stp->outswidb != swidb) || (stp->dlci != dlci))
		continue;
	    if (stp->group == group && (stp->all_addr ||
					!bcmp(addrp, stp->addr,
					      gpi->addrlength))) {
		in_idb->stun_if_last = idb;
		return(stp);
	    }
	}
    }
     return NULL;
}


/*
 * stif_findpeer
 * Given various parameters find the first peer structure
 * we can for a given address.  If not found on the last
 * stif idb pointer's queue try on any stif idb.  If found
 * then update this idb's last stif pointer.
 */

stunpeertype *stif_findpeer (uchar group, uchar *addrp, hwidbtype *in_idb)
{
    stunpeertype *stp;
    hwidbtype *idb;
    stun_protocol_info *gpi;    

    idb = in_idb ? in_idb->stun_if_last : in_idb;

    if ((idb) && ((gpi = stun_pinfo[idb->stun_group]) != NULL))
      for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
	  if (stp->type != STUN_PT_IF || stp->outidb != in_idb)
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
    for (idb = stun_first_idb; idb; idb = IDB_NEXT(idb)) {
	if (!idb->stun_Q.qhead)
	    continue;
	if ((gpi = stun_pinfo[idb->stun_group]) == NULL)
	  continue;
	for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
	    if (stp->type != STUN_PT_IF || stp->outidb != in_idb)
		continue;
	    if (stp->group == group && (stp->all_addr ||
					!bcmp(addrp, stp->addr,
					      gpi->addrlength))) {
		in_idb->stun_if_last = idb;
		return(stp);
	    }
	}
    }
    return(NULL);
}

/*
 * sttcp_findpeer
 * Find a tcp peer.  Either TCP driver or TCP Packet will do.
 */

stunpeertype *sttcp_findpeer (uchar group, uchar *addrp, ipaddrtype ipaddr,
			      int lport, int fport)
{
  stunpeertype *stp;
  hwidbtype *idb;
  stun_protocol_info *gpi;
  boolean found = FALSE;

  for (idb = stun_first_idb; idb; idb = IDB_NEXT(idb)) {
    if (!idb->stun_Q.qhead)
      continue;
    if ((gpi = stun_pinfo[idb->stun_group]) == NULL)
      continue;
    for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
      if ((stp->type != STUN_PT_TCPD) || (stp->ipaddr != ipaddr))
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
      case STUN_P0_PORT:
	return(stp);
      case STUN_P1_PORT:
	return(stp->priority_peers[1]);
      case STUN_P2_PORT:
	return(stp->priority_peers[2]);
      case STUN_P3_PORT:
	return(stp->priority_peers[3]);
      default:
	switch (fport) {
	case 0:
	case STUN_P0_PORT:
	  return(stp);
	case STUN_P1_PORT:
	  return(stp->priority_peers[1]);
	case STUN_P2_PORT:
	  return(stp->priority_peers[2]);
	case STUN_P3_PORT:
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
 * stpak
 * get a packet for sending stun protocol information
 */

paktype *stpak (ushort op, uchar group, uchar *addrp, ushort len)
{
    paktype *pak;
    stunhdrtype *sth;
    stun_protocol_info *gpi;
    int addrlen;

    if ((op != STUN_OP_VREP) && (op != STUN_OP_VREQ)) {
	if ((gpi = stun_pinfo[group]) == NULL)
	    return(NULL);
	addrlen = gpi->addrlength;
    } else {
	addrlen = 0;
    }

    pak = getbuffer(sizeof(stunhdrtype) + len + addrlen - 1);
    if (!pak)
	return(NULL);
    sth = (stunhdrtype *)pak->network_start;
    sth->magic = STUN_MAGIC;
    sth->op = op;
    sth->group = group;
    if (addrlen > 0)
	bcopy(addrp, &(sth->addr[0]), addrlen);
    sth->len = len;
    pak->datagramstart = (uchar *) sth;

     if ((op == STUN_OP_VREP) || (op == STUN_OP_VREQ))
       pak->datagramsize = sizeof(stunhdrtype);
    else
       pak->datagramsize = sizeof(stunhdrtype) + len + addrlen - 1;
 
    return(pak);
}

/*
 * st_op2str
 * convert a stun op code into humanspeak
 */

char *st_op2str (ushort op)
{
    switch (op) {
      case STUN_OP_PASS:          return("Pass");
      case STUN_OP_NOP:           return("NOp");
      case STUN_OP_IGNORE:        return("Ignore");
      case STUN_OP_EMPTY:         return("Empty");
      case STUN_OP_VREQ:          return("Version Request");
      case STUN_OP_VREP:          return("Version Reply");
      case STUN_OP_LINKUP_REQ:    return("Local Ack Linkup Request");
      case STUN_OP_LINKUP_RSP:    return("Local Ack Linkup Response");
      case STUN_OP_LINKDOWN_REQ:  return("Local Ack Link Down Request");
      case STUN_OP_LINKDOWN_RSP:  return("Local Ack Link Down Response");
      case STUN_OP_ABORT_REQ:     return("Abort Request");
      case STUN_OP_ABORT_RSP:     return("Abort Response");
      case STUN_OP_RD_REQ:        return("RD Request");
      case STUN_OP_DATA :         return("SDLC Data");
      case STUN_OP_QUENCH_ON:     return("Quench On");
      case STUN_OP_QUENCH_OFF:    return("Quench Off");
      case STUN_OP_BADOP:         return("Bad Op");
      case STUN_OP_UNKPEER:       return("Unknown Peer");
      case STUN_OP_UNKADDR:       return("Unknown Address");
      case STUN_OP_SIM:           return("SIM Request");
      case STUN_OP_LINK_ESTAB:    return("Link Established");
      default:                    return("Unknown");
    }
}

/*
 * stnail_peer
 * Actually remove a peer and associated structures
 */

static void stnail_peer (
    stunpeertype *stp)
{
    leveltype status;
    char peerid[MAX_PEER_NAME];
    stunpeertype *stptop;
    int i;

    if (stun_debug)
	errmsg(&msgsym(PEERSHUTDOWN, STUN), stpeer_id(stp, peerid),
	       stp->stun_idb->hw_namestring);
    status = raise_interrupt_level(NETS_DISABLE);
    /* Be clean and mark the entry closed before deleting it */
    stun_newstate(stp, STUN_PS_CLOSED);
    if ((stp->type == STUN_PT_IF) || 
        (stp->type == STUN_PT_FR))
	stp->outidb->stun_if_last = NULL;
    if (stp->abort)
	(*stp->abort)(stp);
    if (stp->priority_top) {
	stptop = stp->priority_top;
	for (i = 1; i <= STUN_PRIORITY_COUNT; i++) {
	    if (stptop->priority_peers[i] == stp) {
		stptop->priority_peers[i] = NULL;
		break;
	    }
	}
    }
    reset_interrupt_level(status);
}

boolean stun_print_check (uchar group, uchar *address)
{
    if ((stun_packet_debug == FALSE) ||
	(stun_pinfo[group] == NULL)) {
	return(FALSE);
    }
    if ((stun_debug_group &&
	 stun_debug_group != group) ||
	(stun_debug_addr_significant &&
	 bcmp(stun_debug_addr,
	      address, stun_pinfo[group]->addrlength))) {
	return(FALSE);
    }
    return(TRUE);
}

void stun_print_frame (
    paktype *pak,
    int group,
    char *prefix,
    hwidbtype *idb,
    void (*routine)(paktype *, int, char *, hwidbtype *))
{
    if (!stun_packet_debug)
        return;
    (*routine)(pak, group, prefix, idb);
}

/*
 * stun_addr_to_peer
 */
stunpeertype *stun_addr_to_peer (hwidbtype *idb, char *addr)
{
  stun_protocol_info *gpi;
  stunpeertype *stp;

  gpi = stun_pinfo[idb->stun_group];
  if (!gpi)
    return(NULL);

  for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next)
    if (stp->all_addr || (!bcmp(addr, stp->addr, gpi->addrlength)))
      break;
  return(stp);
  
}

/*
 * stun_input
 * Handle the raw serial side of a serial tunnel connection.
 * This is the stun side.
 */

void stun_input (hwidbtype *idb, paktype *pak)
{
    stunpeertype *stp;
    stunhdrtype *sth;
    ushort pak_opcode;
    stun_protocol_info *gpi;
    uchar *addrstart;
    uchar tg_bcast=0xff;
    sdlc_frametype *sdlcfrm;
    paktype *newpak;
    sdlc_data_block *sdb;

    /*
     * sdlc_count_input can provide useful debugs, even if not running 
     * local-ack.
     */
    sdlc_count_input(idb, pak);

    idb_increment_rx_proto_counts(pak, ACCT_PROTO_STUN);

    pak->acct_proto = ACCT_PROTO_STUN;
    pak_opcode = STUN_OP_PASS;

    if ((idb->state != IDBS_UP) ||
	((gpi = stun_pinfo[idb->stun_group]) == NULL)) {
	stif_nukepak(pak);
	return;
    }
    addrstart = (char *) pak->datagramstart + gpi->addroffset;
    
    /* if tg is active for this interface, special stp search required */
    if ((idb->stun_group) && (stun_pinfo[idb->stun_group]->uses_tg)) {
        /*
         * For a tg active interface, assume only one stp, no multidrop.
         */
        if ((stp = (stunpeertype *) idb->stun_Q.qhead)) {
            if (!stp->tg_active) {
                stif_nukepak(pak);
                return;
            }

            /*
             * Due to NCPs using unpredictable sdlc addresses and sending
             * numbered I-frames before a SNRM during a remote NCP load, STUN
             * TG reverts to STUN Pass-through during the load interval. This
             * interval begins when a SIM/RIM is received and ends when a SNRM
             * is received. Sincec NCPs do not know their sdlc roles during the
             * NCP load, RIM is treated as if it were a SIM.
             */
            sdlcfrm = (sdlc_frametype *) pak->datagramstart;
            if ((sdlcfrm->control & ~SDLC_PF) == SDLC_SIM)
                stp->local_ack_tg_load = TRUE;

            /*
             * Check sdlc frame address to match configured/implied addresses.
             * Check for non-echo, echo, and broadcast SDLC addresses. Note that
             * the overhead of bcmp doe not need to be incurred, since TG sdlc
             * addresses are always 1 byte.
             */
            if (stp->local_ack_tg_load == FALSE) {
                if ((stp->addr[0] != addrstart[0]) &&
                    (stp->addr[0] != (addrstart[0] & SDLC_ECHO_OFF)) &&
                    (addrstart[0] != tg_bcast)) {
                    /* frame address is NOT a valid TG address */
                    if ((stun_debug) || (stun_packet_debug) || (stun_tg_debug))
                        buginf("\nSTUN: interface %s, Rec addr %02x, expect %02x, %02x, or %02x",
                               idb->hw_namestring, (uint)addrstart[0], 
                               *stp->addr, (uint)(*stp->addr | SDLC_ECHO_ON),
                               (uint)tg_bcast);
                    stif_nukepak(pak);
                    return;
                }
            }
        } else {
            stif_nukepak(pak);
            return;
        }
    } else {
       /* Tg not active on this interface.
        * scan the structure looking for an address that matches
        * or a wildcard.  Send this packet to that peer.  Only
        * the structure attached to this given interface is searched.
        * We are only allowed to switch those packets with an address
        * associated with this STUN line.
        */
       for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
           if (stp->all_addr)
               break;
           if (stp->input_addr_inuse && (!bcmp(addrstart, stp->input_addr,
                                            gpi->addrlength)))
               break;
           else if (!bcmp(addrstart, stp->addr, gpi->addrlength))
               break;

       }
       if (stp == NULL) {
          /*
           * If we are configured for group poll and receive a group poll then
           * we send the packet directly to sdlc_input for processing.
           */
          if ((idb->sdlc_group_addr) && (*addrstart == idb->sdlc_group_addr)) {
              sdlc_input(idb, pak);
              return;
          }
          else {
             stif_nukepak(pak);
             return;
          }	 
       }
    }
    stun_print_frame(pak, stp->group, "SDI:", idb, gpi->print_frame);
    if (stp->state == STUN_PS_DIRECT) {
	stp->pkts_rx++;
        stp->bytes_rx += pak->datagramsize;
    } else {
        pak_opcode = (*gpi->serial_forward_hook)(idb, pak, stp);
        if ((pak_opcode & STUN_OP_ERR) || (pak_opcode == STUN_OP_IGNORE)) {
            return;
        } else {
            if (pak_opcode == STUN_OP_DATA) {
                sdb = SDB_FROM_PEER(stp);
                if (!sdb->stun_frame_ok) {
                    stif_nukepak(pak);
                    return;
		  }
                else
                    sdb->stun_frame_ok = FALSE;
	      }
            pak->datagramstart -= sizeof(stunhdrtype) + gpi->addrlength - 1;
            sth = (stunhdrtype *) pak->datagramstart;
	    PUTSHORT(&sth->magic,  STUN_MAGIC);
            PUTSHORT(&sth->op,  pak_opcode);
            sth->group = stp->group;
            PUTSHORT(&sth->len,  pak->datagramsize);
            pak->datagramsize += sizeof(stunhdrtype) + gpi->addrlength - 1;

            /* if tg is active for this peer, insert config'd addr in sth */
            if (stp->tg_active)
               bcopy(stp->addr, &(sth->addr[0]), gpi->addrlength);
            else
               bcopy(addrstart, &(sth->addr[0]), gpi->addrlength);
        }
    }
    if (pak_opcode != STUN_OP_IGNORE) {
        /*
         * If we have a virtual multidrop, then send the broadcast frame to all the 
         * STUN peers in the multidrop.
         */
        if((idb->sdlc_flags & SDLC_FLAG_VIRTUAL_MULTIDROP) && (*addrstart == SDLC_BCAST_ADDR)) {
            for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
                  if(stp->addr[0] != SDLC_BCAST_ADDR)
                     continue;
                  newpak = pak_duplicate(pak);
                  if(!newpak) {
                     buginf("\nSTUN: stun_input pak_dup failed, Insufficient internal buffer");
                     break;
                  }
                  (void) (*stp->sender)(stp, newpak, NULL, FALSE, NULL, FALSE, FALSE);
            }
            stif_nukepak(pak);
        }
        else
	    (void) (*stp->sender)(stp, pak, NULL, FALSE, NULL, FALSE,
			      (pak_opcode == STUN_OP_LINKUP_RSP));
    }
}

void stun_to_network (hwidbtype *idb, paktype *pak, stunpeertype *stp,
		      char *addrstart, int pak_opcode)
{
  stun_protocol_info *gpi;
  stunhdrtype *sth;

  gpi = stun_pinfo[idb->stun_group];
  if (!gpi)
    return;
  pak->datagramstart -= sizeof(stunhdrtype) + gpi->addrlength - 1;
  sth = (stunhdrtype *) pak->datagramstart;
  sth->magic = STUN_MAGIC;
  sth->op = pak_opcode;
  sth->group = stp->group;
  bcopy(addrstart, &(sth->addr[0]), gpi->addrlength);
  sth->len = pak->datagramsize;
  pak->datagramsize += sizeof(stunhdrtype) + gpi->addrlength - 1;
  (void) (*stp->sender)(stp, pak, NULL, FALSE, NULL, FALSE, FALSE);
}

/********************************************************************
 *
 * Stunnel Background
 *
 ********************************************************************/

static boolean stun_tcp_correct_port (
    stunpeertype *stp)
{
    if (!stp->priority_top) {
	return(stp->port == STUN_P0_PORT);
    } else {
	if (stp->priority_top->priority_peers[1] == stp)
	    return(stp->port == STUN_P1_PORT);
	else if (stp->priority_top->priority_peers[2] == stp)
	    return(stp->port == STUN_P2_PORT);
	else if (stp->priority_top->priority_peers[3] == stp)
	    return(stp->port == STUN_P3_PORT);
    }
    return(FALSE);
}

boolean stcheck_conn_state (stunpeertype *stp)
{
    boolean bad;
    char peerid[MAX_PEER_NAME];

    bad = FALSE;
    switch (stp->type) {
      case STUN_PT_TCPD:
	if (!stp->ipaddr || !(stun_tcp_correct_port(stp)))
	    bad = TRUE;
	break;

      case STUN_PT_IF:
	if (!stp->outidb)
	    bad = TRUE;
	break;

      case STUN_PT_FR:
	if (!stp->outswidb || !stp->dlci)
	    bad = TRUE;
	break;

      case STUN_PT_CLS:
        if (!stp->outswidb || !stp->dlci || !stp->lsap)
            bad = TRUE;
        break;

      default:
	bad = TRUE;
	break;
    }
    if (bad || !stp->sender || !stp->connect || !stp->stun_idb) {
	if (stun_debug)
	    errmsg(&msgsym(BADCONN, STUN), 1, stpeer_id(stp, peerid));
	return(FALSE);
    }
    return(TRUE);
}

/*
 * stun_send_keepalive
 *
 * send a keepalive down to a remote peer.
 */
void stun_send_keepalive (stunpeertype *stp)
{
    paktype *pak;
    char peerid[MAX_PEER_NAME];
    stunhdrtype *sth;
 
    if (stp == NULL) {
	errmsg(&msgsym(NOBUF, STUN), "STUN", "send peer keepalive");
        if (stun_debug) {
            buginf("\nSTUN: NULL peer pointer passed to stun_send_keepalive");
        }
        return;
    }
    pak = stpak(STUN_OP_VREQ, stp->group, &(stp->addr[0]), 0);
    if (pak == NULL) {
        if (stun_debug) {
            buginf("\nSTUN: Couldn't get keepalive packet to send to peer %s.",
                   stpeer_id(stp, peerid));
        }
        return;
    }
    /* we want to mak use of the Version Request to check the stun
       peer state.  To make sure the stun peer states are in sync
       the address field in a VREQ or VREP contains the state.
    */
    if (stp->local_ack_active) {
	sth = (stunhdrtype *)pak->network_start;
	sth->addr[0] = (char)stp->slack_state;
    }
 
    if (!((*stp->sender)(stp, pak, NULL, FALSE, NULL, TRUE, FALSE))) {
        if (stun_debug) {
            buginf("\nSTUN: Couldn't send keepalive packet to peer %s.",
                   stpeer_id(stp, peerid));
        }
    }
}

/*
 * stun_background
 * background process for stun functions.
 * Handle making connections to peers.  The packet that caused
 * the peer to be opened is what is enqueued.  The packet has
 * a pointer to the the stun structure that needs the open.
 *
 * This routine will also send periodic keepalives to check for link
 * or remote router failure. This is very important to do especially
 * in a local ack environment. Also, check for keepalive failure and if
 * so, tear the peer down and any associated local ack sessions.
 */
boolean stun_backBLOCK (void)
{
    if (stun_keepalive_interval && !SLEEPING(stun_next_keepalive))
      return(FALSE);
    if (stconn_pakQ.qhead)
      return(FALSE);
    return(TRUE);
}
 

forktype stun_background (void)
{
    stunpeertype *stp;
    paktype *pak, *pumppak;
    int count;
    sys_timestamp time;
    char peerid[MAX_PEER_NAME];
    hwidbtype *idb;
    stun_protocol_info *gpi;
    int i;

    while (TRUE) {
        edisms((blockproc *)stun_backBLOCK, 0);
        if (stun_keepalive_interval && !SLEEPING(stun_next_keepalive)) {
            TIMER_START(stun_next_keepalive, stun_keepalive_interval);
            for(idb = stun_first_idb; idb; idb = IDB_NEXT(idb)) {
                if (!idb->stun_Q.qhead)
                  continue;
                if ((gpi = stun_pinfo[idb->stun_group]) == NULL)
                  continue;
                for (stp = (stunpeertype *) idb->stun_Q.qhead;
                     stp; stp = stp->next) {
                    if ((stp->state == STUN_PS_CONNECTED) &&
                        (stp->local_ack_active)) {
                        COPY_TIMESTAMP(stp->last_keepalive, time);
                        TIMER_UPDATE(time, stun_keepalive_interval);
                        if (!SLEEPING(time))
                            stun_send_keepalive(stp);
                        TIMER_UPDATE(time, (stun_keepalive_count - 1) *
				     stun_keepalive_interval);
                        if (!SLEEPING(time)) {
                            /*
                             * Local ACK currently is only allowed on TCP
                             * connections (because they are reliable).
                             * Abort will cause the connection to eventually
                             * close causing a state change causing a call
                             * to slack_fsm to drop the session.  This is a
                             * fairly long causal thread that is potentially
                             * easy to break, but the SLACK state machine
                             * cannot easily handle getting a network session
                             * loss sent to it TWICE, so, when the state of
                             * the connection goes to CLOSED will be the time
                             * when the connection is aborted.
                             */
                            /*
                             * Abort the TCP connection. If priority is configured
                             * then abort all the connections.
                             */
                            if (stp->priority_top)
                                stp = stp->priority_top;
                            if (stp->priority) {
                                for (i = 1; i <= STUN_PRIORITY_COUNT; i++) {
                                    if (stun_debug)
                                        buginf("\nSTUN: Keepalive failed: tearing down peer %s at %d ",
                                               stpeer_id(stp->priority_peers[i], peerid),
                                               ELAPSED_TIME(stun_start_time)/ONESEC);
                                    if (stp->priority_peers[i]->abort)
                                        (*stp->priority_peers[i]->abort)(stp->priority_peers[i]);
                                }

                            }
                            if (stun_debug)
                                buginf("\nSTUN: Keepalive failed: tearing down peer %s at %d ",
                                       stpeer_id(stp, peerid),
                                       ELAPSED_TIME(stun_start_time)/ONESEC);
                            if (stp->abort)
                              (*stp->abort)(stp);
                        }
                        if (stp->slack_retry_count >= 0) {
			    if (stp->slack_retry_count > stun_keepalive_count){
				/*
				 * turn off retransmission
				 * abort all tcp connections.
				 */
				stp->slack_retry_count = SLACK_RETRY_OFF;
                                if (stp->priority_top)
                                    stp = stp->priority_top;
                                if (stp->priority) {
                                    for (i = 1; i <= STUN_PRIORITY_COUNT; i++) {
                                        if (stp->priority_peers[i]->abort)
                                            (*stp->priority_peers[i]->abort)(stp->priority_peers[i]);
                                    }
                                }
				if (stp->abort)
				    (*stp->abort)(stp);
			    } else {
				/* retransmit  */
				slack_fsm(stp, SLACK_TIMER, SLACK_RETRY);
				stp->slack_retry_count++;
			    }
                        }
                    }
                }
            }
        }
	count = 10;			/* do up to 10 connections */
	while (TRUE) {
	    if (--count == 0)
		break;
	    pak = p_dequeue(&stconn_pakQ);
	    if (!pak)
		break;
	    /*
	     * sanity checks
	     */
	    stp = pak->peer_ptr;
	    if (!stcheck_conn_state(stp)) {
		stif_nukepak(pak);		/* you're history */
		if (stun_debug && stp)
		    errmsg(&msgsym(BADCONN, STUN), 2, stpeer_id(stp, peerid));
		continue;
	    }
            
            if (stp->type != STUN_PT_CLS) { 
            
	    switch (stp->state) {
	      case STUN_PS_OPENING:
		stun_newstate(stp, STUN_PS_OPENWAIT);
		switch ((*stp->connect)(stp)) {
		  case STUN_CONN_ABORTED:
		    stif_nukepak(pak);	/* you're history */
		    continue;
		  case STUN_CONN_FAILED:
		    stun_newstate(stp, STUN_PS_DEAD);
                    reg_invoke_stun_trap(stp);
		    stp->primary_t = NULL;
		    stp->secondary_t = NULL;
		    stif_nukepak(pak);		/* you're history */
		    continue;
		  case STUN_CONN_CONNECTED:
		    if (stun_debug)
			errmsg(&msgsym(OPENED, STUN), "CONN",
			       stpeer_id(stp, peerid), stpeer_state(stp));
		    stun_newstate(stp, STUN_PS_CONNECTED);
                    reg_invoke_stun_trap(stp);
		    /* Send a "pump packet" to get the other, passive end, open */
		    pumppak = stpak(STUN_OP_IGNORE, stp->group, &(stp->addr[0]), 30);
		    (void) (*stp->sender)(stp, pumppak, NULL, FALSE, NULL, FALSE, FALSE);
		    (void) (*stp->sender)(stp, pak, NULL, FALSE, NULL, TRUE, FALSE);
		    break;
                  case STUN_CONN_CONNECTING: 
                     /* this is only for cls, should never come here */ 
                     stun_newstate(stp,STUN_PS_DEAD);
                     stif_nukepak(pak);     /* you are history */ 
                     break; 
		}
		break;
		
	      default:
		errmsg(&msgsym(PEERSTATE, STUN),
		       stpeer_id(stp, peerid), stp->state, 1);
		stif_nukepak(pak);		/* you're history */
		continue;
	      } /* of switch */ 
              } /* of if */ 
              else  {    
              
                  switch(stp->state) { 
                      case STUN_PS_OPENING:
		          stcls_connect(stp); /* this is different nature for cls, this will 
						 send activatesap.req for local sap, connectRet
						 routine will set the state to STUN_PS_CONNECTED.
						*/
                      case STUN_PS_OPENWAIT: 
                          enqueue(&stconn_pakQ,pak); 
                          continue;    
		      case STUN_PS_DEAD:
                      case STUN_PS_CLOSED:
		          stif_nukepak(pak);	/* you're history */
		          continue;
		      case STUN_PS_CONNECTED:  /* this state change should be done
                                                  in connected function*/
                          /* Send a "pump packet" to get the other, passive end, open */
                          pumppak = stpak(STUN_OP_IGNORE, stp->group, &(stp->addr[0]), 30);
                           
                          (void) (*stp->sender)(stp, pumppak, NULL, FALSE, NULL, FALSE, FALSE);
                          (void) (*stp->sender)(stp, pak, NULL, FALSE, NULL, TRUE, FALSE);
                          break;
		      default:
                          errmsg(&msgsym(PEERSTATE, STUN),
                          stpeer_id(stp, peerid), stp->state, 1);
                          stif_nukepak(pak);              /* you're history */
                          continue;
		} /* of switch */
	     }  /* of else */ 

	 }
    }
 }

/********************************************************************
 *
 * Raw Interface: STUN Implementation Support
 *
 ********************************************************************/

void stif_nukepak (paktype *pak)
{
    datagram_done(pak);
}

static void stif_err (
    stunpeertype *stp,
    hwidbtype *idb,
    stunhdrtype *sth,
    char *label)
{
    char peerid[MAX_PEER_NAME];

    if (stp)
	idb = stp->outidb;
    if (stun_debug)
	errmsg(&msgsym(ERR, STUN), label, st_op2str(sth->op),
	       stpeer_id(stp, peerid), sth->op, sth->len);
    if (!stp)
	return;
    switch (sth->op) {
      case STUN_OP_UNKPEER:		/* about what we are sending */
      case STUN_OP_UNKADDR:
	stun_newstate(stp, STUN_PS_DEAD);
        reg_invoke_stun_trap(stp);
	break;
      default:
      case STUN_OP_BADOP:
	break;
    }
}

static void stif_send_punt (
    stunpeertype *stp,
    hwidbtype *idb,
    ushort op,
    stunhdrtype *sth,
    char *label)
{
    paktype *pak;
    char peerid[MAX_PEER_NAME];

    pak = stpak(op, sth->group, &(sth->addr[0]), 0);
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
	pak->linktype = LINK_STUN;
	pak->acct_proto = ACCT_PROTO_STUN;
	if (idb->vencap && (*idb->vencap)(pak, 1)) {
	    datagram_out(pak);
	    if (stun_debug)
		errmsg(&msgsym(SENDPUNT, STUN), label, st_op2str(op),
		       stpeer_id(stp, peerid));
	    return;
	}
	stif_nukepak(pak);
	return;
    }
    if (pak && stif_direct(stp, pak, NULL, TRUE, NULL, FALSE, FALSE)) {
	if (stun_debug)
	    errmsg(&msgsym(SENDPUNT, STUN), label, st_op2str(op),
		   stpeer_id(stp, peerid));
	return;
    }
    if (stun_debug)
	buginf("\nSTUN: %s: send punt, couldn't send %s %s to %s",
	       label, (pak ? " (no pak)" : ""), st_op2str(op),
	       stpeer_id(stp, peerid));
}

/*
 * stun_fromnet_forward
 * This routine handles the proxy state machine for network side input form
 * any type (across a serial link using the special serial transport, or TCP).
 */
void stun_fromnet_forward (
    paktype *pak,
    stunpeertype *stp,
    ushort opcode)
{
    hwidbtype *idb = hwidb_or_null(pak->if_output);
    stun_print_frame(pak, stp->group, "NDI:",
		     idb,
		     stun_pinfo[stp->group]->print_frame);

    pak->acct_proto = ACCT_PROTO_STUN;
    if (!(*stun_pinfo[stp->group]->network_forward_hook)(idb,
							 pak, stp, opcode)) {
       /*
        * Send the packet to the interface only if the interface is up.
        */
        if (pak->if_output->hwptr->state == IDBS_UP) {
            pak->linktype = LINK_STUN;
            datagram_out(pak);
        }
        else
            stif_nukepak(pak);
    }
}

/*
 * stif_input
 * process direct input from an interface.  Network side input.
 */

void stif_input (paktype *pak)
{
    stunpeertype *stp;
    stunhdrtype *sth;
    hwidbtype *idb;
    idbtype *swidb;
    ushort opcode;

    sth = (stunhdrtype *) pak->network_start;

    if (stun_debug) {
      if ((sth->op != STUN_OP_DATA) && (sth->op != STUN_OP_PASS)) {
        buginf("\nSTUN: Received %s opcode on %s",
               st_op2str(sth->op), pak->if_input->namestring);
      }
    }
 
    /* Check version number and reply with our own */
    if ((sth->op == STUN_OP_VREQ) ||
	(sth->op == STUN_OP_VREP)) {
	if (stun_debug)
  	    buginf("\nSTUN: version %x from remote on %s. Reply %x.",
		   sth->magic, pak->if_input->namestring, STUN_MAGIC);
	if (sth->op == STUN_OP_VREQ) {
	    sth->op = STUN_OP_VREP;
	    sth->magic = STUN_MAGIC;
	    stif_direct(NULL, pak, pak->if_input->hwptr, TRUE, NULL, FALSE, FALSE);
	}
        else
          stif_nukepak(pak);
	return;
    }
    if (sth->magic != STUN_MAGIC) {
	if (stun_debug)
	    errmsg(&msgsym(BADMAGIC, STUN), "IFin", STUN_MAGIC, sth->magic, 1);
	stif_nukepak(pak);
	return;
    }
    swidb = pak->if_input;
    idb = swidb->hwptr;

    if (is_frame_relay(idb))
        stp = stfr_findpeer(sth->group, &(sth->addr[0]), swidb, pak);
    else
        stp = stif_findpeer(sth->group, &(sth->addr[0]), idb);
 
    /* Any data at all is good enough for a keepalive */
    if (stp)
	GET_TIMESTAMP(stp->last_keepalive);
 
    opcode = sth->op;
    /*
     * We don't check for validity of stp until later.
     */	
    if (opcode & STUN_OP_ERR) {
	stif_err(stp, idb, sth, "IFin");
	stif_nukepak(pak);
	return;
    }
    if (opcode == STUN_OP_IGNORE) {
	stif_nukepak(pak);
	return;
    }
    switch (opcode) {
      case STUN_OP_LINKUP_REQ:
      case STUN_OP_LINKUP_RSP:
      case STUN_OP_LINKDOWN_REQ:
      case STUN_OP_LINKDOWN_RSP:
      case STUN_OP_ABORT_REQ:
      case STUN_OP_ABORT_RSP:
      case STUN_OP_RD_REQ:
      case STUN_OP_DATA:
      case STUN_OP_PASS:
      case STUN_OP_SIM:
        break;
      default:
	stif_send_punt(stp, idb, STUN_OP_BADOP, sth, "IFin");
	stif_nukepak(pak);
	return;
    }
    if (!sth->len) {
	if (stun_debug)
	    errmsg(&msgsym(BADLENOP, STUN), "IFin", sth->op, sth->len);
	if (stp)
	    stp->drops++;
	stif_nukepak(pak);
	return;
    }
    /*
     * find the first match in any of the address queues.
     */
    if (!stp) {
	stif_send_punt(NULL, idb, STUN_OP_UNKPEER, sth, "IFin");
	stif_nukepak(pak);
	return;
    }
    /*
     * process the incoming packet
     */
    pak->datagramstart = ((uchar *) sth + sizeof(stunhdrtype) +
			  stun_pinfo[stp->group]->addrlength - 1);
    pak->datagramsize = sth->len;
    pak->if_output = firstsw_or_null(stp->stun_idb);
    stp->pkts_rx++;
    stp->bytes_rx += pak->datagramsize;
    stun_fromnet_forward(pak, stp, opcode);
}


/********************************************************************
 *
 * raw interface: stun implementation
 * peer vectors
 *
 ********************************************************************/

/*
 * stif_direct (acts as stif_sender)
 * just send a packet directly.  no state checks.  datagramsize and
 * datagramstart are asssumed to be set up correctly..
 */

boolean stif_direct (stunpeertype *peer, paktype *pak, hwidbtype *outidb,
		     boolean force, stunpeertype **outpeer,
		     boolean usethispeer, boolean toallpp)
{
    hwidbtype *idb;
    ushort dlci;

    if (outpeer)
	*outpeer = peer;
    idb = ((outidb == NULL) ? (peer->outidb) : (outidb));
    pak->if_output = idb->firstsw;
    pak->linktype = LINK_STUN;
    pak->acct_proto = ACCT_PROTO_STUN;
    dlci = ((peer && peer->type == STUN_PT_FR) ? peer->dlci : 1);

    if ((outidb != NULL) || ((peer) && (peer->state == STUN_PS_DIRECT)) ||
	(idb->vencap &&
	 (*idb->vencap)(pak, (((idb->status & IDB_IEEEMASK) && (peer)) ?
			      (ulong) peer->macaddr : (ulong) dlci)))) {
	datagram_out(pak);
	if (peer) {
	  peer->pkts_tx++;
          peer->bytes_tx += pak->datagramsize;
        }  
	return(TRUE);
    }
    stif_nukepak(pak);
    if (peer)
      peer->drops++;
    if (stun_debug)
        buginf("\nSTUN: vencap failed on %s", idb->hw_namestring);
    return(FALSE);
}

/*
 * stif_abort
 * abort a direct interface peer.  this doesn't actually do the
 * destruction of the data structure.  but it is responsible for tearing
 * down the communications link.
 */

void stif_abort (stunpeertype *peer)
{
    /*
     * nothing to abort
     */
}

/*
 * stif_close
 * close the transport connection.  accept no further input but
 * allow any output to be physically transmitted.
 */

void stif_close (stunpeertype *peer, ulong param)
{
    /*
     * no way to close the connection.  interfaces are always open.
     */
}

/*
 * stif_connect
 * establish a connection to a remote peer via a direct interface.
 * this routine isn't currently used but if it were it would
 * work just fine.  stif connections currently are always open.
 * with more configuration information being passed around this
 * will change.
 */

stunconnrtn stif_connect (stunpeertype *stp)
{
    paktype *pak;
    
    if ((stp->type != STUN_PT_IF) && (stp->type != STUN_PT_FR)) {
	if (stun_debug)
	    buginf("\nSTUN: conn: connect pointer invalid (if)");
	return(STUN_CONN_FAILED);
    }
    pak = stpak(STUN_OP_VREQ, 0, NULL, 0);
    if (!pak || !stif_direct(stp, pak, NULL, FALSE, NULL, FALSE, FALSE)) {
	return(STUN_CONN_FAILED);
    }
    return(STUN_CONN_CONNECTED);
}

/********************************************************************
 *
 * tcpdriver: Remote STUN implementation
 * Peer Vectors
 *
 ********************************************************************/

/*
 * Classify a packet for its priority. 
 *
 * See if SNA Local LU Address prioritization apply. (This may
 * be overriden by stun address priority.)  
 *
 * Then see if stun address priroitization for TCP/IP transport applies. 
 * (note, unlike stun serial transport, the pak->if_input is used in
 * finding the priority list.)  
 * 
 * If both test fails, then send out based on value of last two bits
 * of the first byte of the address.
 */
static int stun_packet_classify (
    paktype      *pak,
    stunpeertype *stp)
{

  stunhdrtype    *sth;
  hwidbtype        *idb;
  uchar          *fid2th;
  sdlc_frametype *sdlcfrm;
  int             priority, match=FALSE;
  sna_fid4_hdr   *sna_hdr;


  sth = (stunhdrtype *) (pak->datagramstart);
  if ((sth->op == STUN_OP_PASS) || (sth->op == STUN_OP_DATA)) {
    /* make sure this is an SNA fid2 packet */
    idb = pak->if_input->hwptr;
    /* first the logic for SNA Local LU address prioritization */
    /* make sure it is a sdlc frame */
    if (stun_pinfo[stp->group]->prototype == SDLC_PTYPE) {
      sdlcfrm = (sdlc_frametype *)((uchar *)sth + sizeof(stunhdrtype)
				   + stun_pinfo[stp->group]->addrlength -1);

      if (SDLC_ISIFRAME(sdlcfrm) ) {

        sna_hdr = (sna_fid4_hdr *)( (uchar *)sdlcfrm + sizeof(sdlc_frametype) );
        if (((sna_hdr->fid_tg & SNA_FID) == SNA_FID4) && (stun_tg_debug))
            buginf("\nSTUN_TG: frame:%x to network", (sna_hdr->snf & TG_SNF));

        fid2th = (uchar *)sdlcfrm + sizeof (sdlc_frametype);
        /* prioritize according to the lu address */
        if((*fid2th & SNA_TH_FID_MASK) == SNA_TH_FID2_VALUE) {
          priority = fid2_pri_classify(pak->if_input->hwptr, fid2th, &sdlcfrm->address);
          match=TRUE;
        }
      }
    }
    /*  now test to see stun address prioritization applies */
    if(idb->priority_list) {
      match = stun_addr_pri_tcp(idb, pak, &priority);
    }
    if(match == TRUE)
      return(priority);
    else
      return(PRIORITY_NORMAL);
  }
  else if (stp->tg_active)
    return(PRIORITY_NORMAL);
  else
    return (PRIORITY_HIGH);
}

static stunpeertype *stun_packet_classify_peer (
    paktype *pak,
    stunpeertype *rootpeer)
{
    int priority, i;
    stunpeertype *defaultone = rootpeer;

    if (!rootpeer->priority)
	return(rootpeer);
    priority = stun_packet_classify(pak, rootpeer);
    if (priority == 0) {
        return(rootpeer);
    } else {
        for (i = priority; i <= STUN_PRIORITY_COUNT; i++) {
            if (rootpeer->priority_peers[i]) {
                return(rootpeer->priority_peers[i]);
            }
        }
    }
    return(defaultone);
}


/*
 * sttcpd_sender
 * this is the sender for tcp using the tcpdriver.
 *
 * if the peer isn't open yet the appropriate state is entered.
 * if this is the first packet in the not open state it is queued
 * for the background process which will open.  if this isn't the
 * first packet it is squished into the ground.
 *
 * this routine can be called from anywhere including interrupt level.
 */

boolean sttcpd_sender (stunpeertype *inpeer, paktype *pak, hwidbtype *outidb,
		       boolean force, stunpeertype **outpeer,
		       boolean usethispeer, boolean toallpp)
{
    leveltype status;
    int res, i, j;
    paktype *newpak[STUN_PRIORITY_COUNT];
    boolean tmp_result;
    stunpeertype *peer, *toppeer;
    int current_len; 
    char debug_output[5];
    sdlc_data_block *sdb;
    stunhdrtype *sth;


    if ((toallpp) && (inpeer->priority)) {
	if (outpeer) {
	    *outpeer = inpeer;
	}
	/* Allocate ALL of the packets up front. This is all or nothing! */
	for (i = 0; i < STUN_PRIORITY_COUNT; i++) {
	    newpak[i] = pak_clip(pak, 2);
	    if (!newpak[i]) {
		for (j = (i - 1); j >= 0; j--) {
		    stif_nukepak(newpak[j]);
		}
		errmsg(&msgsym(NOBUF, STUN), "STUN", "open priority peers");
		if (stun_debug) {
		    buginf("\nSTUN: Cannot duplicate packet in sttcpd_sender!");
		    buginf("\nSTUN: Allocate more buffers...");
		}
		stif_nukepak(pak);
		return(FALSE);
	    }
	}
	for (i = 1; i <= STUN_PRIORITY_COUNT; i++) {
	    if (!inpeer->priority_peers[i]) {
		if (stun_debug) {
		    buginf("\nSTUN: Odd state; not all peers exist");
		}
		tmp_result = FALSE;
	    }  else {
		tmp_result = sttcpd_sender(inpeer->priority_peers[i],
					   newpak[i-1], outidb, force, NULL,
					   TRUE, FALSE);
	    }
	    if ((!inpeer->priority_peers[i]) || (!tmp_result)) {
		if (stun_debug) {
		    buginf("\nSTUN: Failed to send to %s priority peer",
			   stun_priority_peername(i));
		}
		for (j = i; j < STUN_PRIORITY_COUNT; j++) {
		    stif_nukepak(newpak[j]);
		}
		stif_nukepak(pak);
		return(FALSE);
	    }
	}
	usethispeer = TRUE;
	/* Fall through to the normal code now... */
    }

    if (usethispeer) {
	peer = inpeer;
    } else {
	peer = stun_packet_classify_peer(pak, inpeer);
    }
    if (outpeer)
	*outpeer = peer;
   /*
    **  SDLC Flow Control
    **  -----------------
    **
    **  If we are already in a flow control state and receive an I-Frame,
    **  compain about it and pass it on anyway.
    **
    **  If we are not in a flow control state, see if this I-Frame pushes
    **  us over the high water mark.
    **
    **  Regardless of the flow control state, pass this I-Frame along
    **  to the TCP/IP driver.
    **
    **  Flow control logic only makes sense when locak-ack is on.
    */
  
    if (peer->local_ack_active) {
       /* Top peer's slack_flow_control represents all priority peers */
        if ((!peer->priority) && (peer->priority_top))
          toppeer = peer->priority_top;
        else
          toppeer = peer;

        if (toppeer->slack_flow_control)
        {
          if (slack_debug & SLACK_DBG_FLOW_CONTROL)
	     buginf("\nSTUN (Serial%d): Received I-Frame while flow controlled",
		   peer->stun_idb->unit);
	}
	current_len = reg_invoke_ip_tcpdriver_oqlen(peer->primary_t);
	/* test if tcp queue high wather mark has been reached */
        if (current_len >= ((peer->user_def_max_tcp_qlen *
                            SLACK_HI_PERCENT) / 100)) {
            strcpy (debug_output, "RNR");
            sdlc_set_clear_busy (peer->stun_idb, (unsigned) peer->addr[0], TRUE);
            toppeer->slack_flow_control = TRUE;
	} else {
	    sprintf (debug_output, "%d", current_len);
	}
	if (slack_debug & SLACK_DBG_FLOW_CONTROL) {
	    buginf(".%s", debug_output);
	}
    }
 
    status = raise_interrupt_level(NETS_DISABLE);
    switch (peer->state) {
      case STUN_PS_CONNECTED:
	reset_interrupt_level(status);
        /*
         * If peer->primary_t is NULL, then we do not have a TCP session.
         * Simulate tcpdriver_send()'s return code for "no tcp session".
         * This situation occurs after a TCP session loss and sttcpd_abort()
         * has executed.
         */
        if (!peer->primary_t)
           res = TCPDSEND_CLOSED;
        else
           res = reg_invoke_ip_tcpdriver_send(peer->primary_t, pak, force);
	if (TCPDSEND_ERROR(res)) {
	    if (slack_debug & SLACK_DBG_FLOW_CONTROL) {
		buginf("\nSLACK (Serial%d): Dropped SDLC frame (TCP/IP full)",
		       peer->stun_idb->unit);
	    }
	    peer->drops++;
	    stif_nukepak(pak);
	    if (res == TCPDSEND_CLOSED) {
		stun_newstate(peer, STUN_PS_DEAD);
                reg_invoke_stun_trap(peer);
	    }
	    return(FALSE);
	}
	peer->pkts_tx++;
        peer->bytes_tx += pak->datagramsize;
	return(TRUE);
      case STUN_PS_DEAD:
      case STUN_PS_CLOSED:
        /*
         * Only do active open
         * if it is a secondary stun sdlc station,
         * or if stun local-ack is not active.
         * or if sdlc is in a disconnect state (allow XID to flow)
         * or if the op is a LINKUP_RSP (opens non root peers)
         *
         */
        sdb = SDB_FROM_PEER(peer);
        sth = (stunhdrtype *) pak->datagramstart;
        if ((!(peer->local_ack_active)) ||
	      ISSSDLC(peer->stun_idb) ||
              (sdb->sdlc_state == SDLC_DISCONNECT) ||
              (sth->op == STUN_OP_LINKUP_RSP)) {
	    stun_newstate(peer, STUN_PS_OPENING);
	    pak->peer_ptr = peer;
	    enqueue(&stconn_pakQ, pak);
	    reset_interrupt_level(status);
	    peer->pkts_tx++;
            peer->bytes_tx += pak->datagramsize;
	    return(TRUE);
        }
      case STUN_PS_OPENWAIT:
      case STUN_PS_OPENING:
      default:
	peer->drops++;
        if (stun_debug)
            buginf("\nSTUN: tcpd sender in wrong state, dropping packet");
	reset_interrupt_level(status);
	stif_nukepak(pak);
	return(FALSE);
    }
}
#ifdef NOBODY_CALLS_ME
/*
 * sttcpd_direct
 * just send it.  note that tcpdriver packet input assumes
 * that datagram_start and datagram_size are already set up.
 *
 * Direct is used for opening up connections and things like
 * that.  Don't pop the statistics.
 */

boolean sttcpd_direct (stunpeertype *stp, paktype *pak, hwidbtype *outidb,
		       boolean force)
{
    int res = TCPDSEND_QUEUED;

    if (stp->primary_t) {
	res = reg_invoke_ip_tcpdriver_send(stp->primary_t, pak, force);
	if (TCPDSEND_OK(res))
	    return(TRUE);
    }
    stif_nukepak(pak);
    if (res == TCPDSEND_CLOSED) {
	stun_newstate(stp, STUN_PS_DEAD);
        reg_invoke_stun_trap(stp);
    }
    return(FALSE);
}
#endif /* NOBODY_CALLS_ME */

/*
 * sttcpd_abort
 * nuke with extreme prejudice a tcpdriver peer.  this aborts the
 * connection and drains any input.
 */

void sttcpd_abort (stunpeertype *stp)
{
    char peerid[MAX_PEER_NAME];

    if (stun_debug && stp->primary_t) {
	errmsg(&msgsym(TCPPEERSHUT, STUN), "aborting", stpeer_id(stp, peerid),
	       reg_invoke_ip_tcpdriver_fhost(stp->primary_t),
	       (ulong)reg_invoke_ip_tcpdriver_fport(stp->primary_t));
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
    stun_newstate(stp, STUN_PS_DEAD);
}

void sttcpd_close (stunpeertype *stp, ulong param)
{
    char peerid[MAX_PEER_NAME];

    if (stun_debug) {
	errmsg(&msgsym(TCPPEERSHUT, STUN), "closing", stpeer_id(stp, peerid),
	       reg_invoke_ip_tcpdriver_fhost(stp->primary_t),
	       reg_invoke_ip_tcpdriver_fport(stp->primary_t));
    }
    if (stp->primary_t) {
	reg_invoke_ip_tcpdriver_update(stp->primary_t, sttcpd_drain, 1, 
				       (void *) param);
	reg_invoke_ip_tcpdriver_close(stp->primary_t);
    }
    if (stp->secondary_t) {
	reg_invoke_ip_tcpdriver_update(stp->secondary_t, sttcpd_drain, 1,
				       (void *) param);
	reg_invoke_ip_tcpdriver_close(stp->secondary_t);
    }
    if (param == (ulong) 0) {
	stp->primary_t = stp->secondary_t = NULL;
    }
}

/* 
 * sttcpd_flush
 *
 * Process a flags marked for "FLUSHING"
 */
static boolean sttcpd_flush (void *stpcookie)
{
    stunpeertype *stp, *stpsub;
    stun_protocol_info *gpi;
    hwidbtype *idb;

    stp = (stunpeertype *) stpcookie;

    if ((!stp) || ((gpi = stun_pinfo[stp->group]) == NULL)) {
	return(TRUE);
    }

    /*
     * Okay -- we found a legitimate group. Now let's make sure all of its
     * queues are empty.
     */
    for (idb = stun_first_idb; idb; idb = IDB_NEXT(idb)) {
	if (!idb->stun_Q.qhead) {
	    continue;
	}
	if (idb->stun_group != stp->group) {
	    continue;
	}
	for (stpsub = (stunpeertype *) idb->stun_Q.qhead; stpsub;
	     stpsub = stpsub->next) {
	    if ((stpsub->type != STUN_PT_TCPD)) {
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
 * sttcpd_connect
 * perform an active open to establish a connection to our peer.  it
 * handles basic opening and potential dual opens that might be occuring.
 */

stunconnrtn sttcpd_connect (stunpeertype *stp)
{
    int openstat;
    ipaddrtype lastip;
    tcp_encaps *t;
    char peerid[MAX_PEER_NAME];
    tcp_encaps tmp;

    if (stp->type != STUN_PT_TCPD) {
	if (stun_debug)
	    buginf("\nSTUN: CONN: connect pointer invalid (tcpd)");
	return(STUN_CONN_FAILED);
    }
    stp->retries = STUN_MAX_OPEN_TRIES + 1;
    while (--stp->retries) {
	if (stun_debug)
	    errmsg(&msgsym(OPENING, STUN), stpeer_id(stp, peerid),
		   stp->retries);
	openstat = -1;
	lastip = stp->ipaddr;
	/*
	 * tcpdriver_active_open will block waiting for the connection
	 * to open.
	 */
	memset(&tmp, 0, sizeof(tcp_encaps));
	tmp.inthresh = sizeof(stunhdrtype) +
	    stun_pinfo[stp->group]->addrlength - 1;
	tmp.cookie = stp;
	tmp.readf = sttcpd_rdhdr;
	tmp.closef = sttcpd_fini;
	tmp.flushf = sttcpd_flush;
        tmp.flow_ctrl = NULL;
	t = reg_invoke_ip_tcpdriver_active_open(stp->ipaddr, stun_name,
						stp->port,
						10240, /* tcp window */	
						&tmp, &openstat);
	reg_invoke_ip_tcpdriver_set_oqmax(t, stp->user_def_max_tcp_qlen);
	reg_invoke_ip_tcpdriver_change_timeout(t,TRUE); /* reenable timeouts */
	/*
	 * while we were asleep it's possible that someone pulled
	 * the rug.  check out our pointers.
	 */
	if (stp->type != STUN_PT_TCPD || lastip != stp->ipaddr ||
	    stp->state < STUN_PS_OPENWAIT) {
	    reg_invoke_ip_tcpdriver_abort(t);
	    return(STUN_CONN_ABORTED);
	}
	switch (stp->state) {
	  case STUN_PS_OPENWAIT:
	    if (t) {			/* successful open? */
		if (stp->primary_t) {
		    /*
		     * remote side already got in.  we no longer
		     * need the connection just opened.
		     */
		    reg_invoke_ip_tcpdriver_update(t, sttcpd_drain, 1, NULL);
		    reg_invoke_ip_tcpdriver_abort(t);
		} else {
		    stp->primary_t = t;
		}
		return(STUN_CONN_CONNECTED);
	    }
	    break;

	  default:
	    errmsg(&msgsym(CONNILLSTATE, STUN), stpeer_id(stp, peerid),
		   stp->state);
	    reg_invoke_ip_tcpdriver_abort(t);
	    return(STUN_CONN_FAILED);
	}
	if (openstat == NO_MEMORY || openstat == UNREACHABLE ||
	      openstat == TIMEDOUT || openstat == OPTSTOOBIG) {
	    stp->retries = 1;
	}
    }
    if (stp->state == STUN_PS_OPENWAIT && openstat == -1) {
	openstat = TIMEDOUT;
    }
    if (stun_debug && stp->state != STUN_PS_CLOSED) {
	errmsg(&msgsym(CONNOPENFAIL, STUN), stpeer_id(stp, peerid),
	       tcpuser_open_fail_str(openstat), openstat);
    }
    return(STUN_CONN_FAILED);
}



/********************************************************************
 *
 * tcpdriver: support
 *
 ********************************************************************/

void sttcpd_send_close (tcp_encaps *t, ushort op, stunhdrtype *sth,
			char *label, void *abort_cookie, boolean abort)
{
    paktype *pak;
    int result;

    if (stun_debug) {
	errmsg(&msgsym(SENDPUNTTCP, STUN), label, st_op2str(op),
	       sth->group, reg_invoke_ip_tcpdriver_fhost(t));
    }
    result = TCPDSEND_QFULL;
    pak = stpak(op, sth->group, &(sth->addr[0]), 0);
    if (pak)
	result = reg_invoke_ip_tcpdriver_send(t, pak, TRUE);
    if (!pak || TCPDSEND_ERROR(result)) {
	if (!pak)
            errmsg(&msgsym(NOBUF, STUN), "STUN", "send tcp close");
	if (stun_debug) {
	    buginf("\nSTUN: %s: send close, no pak", label);
	}
	if (pak) {
	    datagram_done(pak);
	}
    }
    if (abort) {
	reg_invoke_ip_tcpdriver_update(t, sttcpd_drain, 1, abort_cookie);
	reg_invoke_ip_tcpdriver_close(t);
    }
    return;
}

/*
 * sttcpd_err
 * Simple error handler for dealing with errors coming back from a
 * peer.
 */

void sttcpd_err (stunpeertype *stp, tcp_encaps *t, stunhdrtype *sth,
		 char *label)
{
    char errbuff[MAX_PEER_NAME];

    if (stun_debug)
	errmsg(&msgsym(ERR, STUN), label, st_op2str(sth->op),
	       stpeer_id(stp, errbuff), sth->op, sth->len);
    switch (sth->op) {
      case STUN_OP_BADOP:
      case STUN_OP_UNKPEER:
      case STUN_OP_UNKADDR:
      default:
	reg_invoke_ip_tcpdriver_abort(t);
	break;
    }
}

/*
 * stget_hdr
 * obtain the bytes needed to build a header.
 */

boolean stget_hdr (tcp_encaps *t, tcbtype *tcb, int thresh, stunpeertype *stp,
		   stunhdrtype *sth, char *label, boolean *ignore)
{
    int addr_thresh;
    int bytes;
    int s;
    uchar *p, *dest;
    char errbuf[MAX_PEER_NAME];
    paktype *pak;
    
    *ignore = FALSE;
    bytes = tcp_unread(tcb);
    if (bytes < thresh) {
	if (stun_debug)
	    buginf("\nSTUN: %s: %s bad state, thresh %d, bytes %d",
		   label, (stp ? stpeer_id(stp, errbuf) : ""), thresh, bytes);
	return(FALSE);
    }
    dest = (uchar *)sth;
    do {
	s = tcp_getstring(tcb, &p, &bytes, thresh);
	if (s != OK) {
	    if (stun_debug)
		buginf("\nSTUN: %s: getstring failed %d - %i(%d) -> %d",
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
    if ((sth->op == STUN_OP_VREQ) ||
	(sth->op == STUN_OP_VREP)) {
	*ignore = TRUE;
	if (stun_debug)
  	    buginf("\nSTUN: version %x from remote on %i(%d) -> %d. Reply %x.",
		   sth->magic, tcb->foreignhost, tcb->foreignport,
		   tcb->localport, STUN_MAGIC);
	if (sth->op == STUN_OP_VREQ) {
	    int result;

	    result = TCPDSEND_QFULL;
	    pak = stpak(STUN_OP_VREP, 0, NULL, 0);
	    if (pak)
		result = reg_invoke_ip_tcpdriver_send(t, pak, TRUE);
	    if (!pak || TCPDSEND_ERROR(result)) {
		if (!pak)
		    errmsg(&msgsym(NOBUF, STUN), "STUN", "send version reply");
		if (stun_debug) {
                    buginf("\nSTUN: %s: send version reply, nopak/fail",
                           stpeer_id(stp, errbuf));
		}
		if (pak) {
		    datagram_done(pak);
		}
	    }
	}
	return(TRUE);
    }
    if (sth->magic != STUN_MAGIC) {
	if (stun_debug)
	    errmsg(&msgsym(BADMAGIC, STUN), label,
		   STUN_MAGIC, sth->magic, 2);
	return(FALSE);
    }

    /*
     * In passive open, the header is read in two parts -- first, the core
     * portion, and then any additional "address bytes" as dictated by the
     * protocol information. This allows passive_opens to be general.
     */
    if (stun_pinfo[sth->group] == NULL) {
	return(FALSE);
    }

    addr_thresh = stun_pinfo[sth->group]->addrlength;

    if (addr_thresh) {
	dest = (uchar *)(&(sth->addr[0]));
	do {
	    s = tcp_getstring(tcb, &p, &bytes, addr_thresh);
	    if (s != OK) {
		if (stun_debug)
		    buginf("\nSTUN: %s: getstring failed %d - %i(%d) -> %d",
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
 * sttcpd_drain
 * Drain all input.  This makes sure we can't hang on input in a tcpdriver
 * close state while waiting for any output to finish up.
 */

void sttcpd_drain (tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie)
{
    int bytes;
    uchar *p;
    int s;
    stunpeertype *stp;
    
    stp = (stunpeertype *)cookie;

    bytes = tcp_unread(tcb);
    if (stun_debug) {
	buginf("\nSTUN: In sttcpd_drain, unget = %d", bytes);
    }
    while (bytes) {
	s = tcp_getstring(tcb, &p, &bytes, bytes);
	if (s != OK && (stun_debug)) {
	    buginf("\nSTUN: DRAIN: getstring failed %d", s);
	}
	if (s != OK) {
	    reg_invoke_ip_tcpdriver_abort(t);
	    return;
	}
	if (stun_debug) {
	    buginf("\nSTUN: After getstring in sttcpd_drain");
	}
	bytes = tcp_unread(tcb);
    }
    if (stun_debug) {
	buginf("\nSTUN: Done with sttcpd_drain.");
    }
}

/*
 * sttcpd_phdr
 * Deal with the first packet for a new connection.  This
 * has to obviously be on the passive side.  The active
 * side already knows which connection it has.
 */

void sttcpd_phdr (tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie)
{
    boolean ignore;
    stunhdrtype lhdr, *sth;
    leveltype status;
    char peerid[MAX_PEER_NAME];
    stunpeertype *stp;
    
    stp = (stunpeertype *)cookie;

    if (stp) {
	if (stun_debug) {
	    buginf("\nSTUN: PHDR: stp set, t %i, %s",
		   reg_invoke_ip_tcpdriver_fhost(t), stpeer_id(stp, peerid));
	}
	reg_invoke_ip_tcpdriver_abort(t);
	return;
    }
    sth = &lhdr;
    if (!stget_hdr(t, tcb, (sizeof(stunhdrtype) - 1), stp,
		   sth, "PHDR", &ignore)) {
	reg_invoke_ip_tcpdriver_abort(t);
	return;
    }
    if (ignore) {
        return;
    }
    stp = sttcp_findpeer(sth->group, &(sth->addr[0]),
			 reg_invoke_ip_tcpdriver_fhost(t),
			 reg_invoke_ip_tcpdriver_lport(t),
			 reg_invoke_ip_tcpdriver_fport(t));
    if (sth->op & STUN_OP_ERR) {
	sttcpd_err(stp, t, sth, "PHDR");
	reg_invoke_ip_tcpdriver_update(t, sttcpd_drain, 1, stp);
	reg_invoke_ip_tcpdriver_close(t);
	return;
    }
    if (!stp) {
	sttcpd_send_close(t, STUN_OP_UNKPEER, sth, "PHDR", NULL, TRUE);
	return;
    }
    if (stun_debug) {
	errmsg(&msgsym(OPENED, STUN), "PHDR", stpeer_id(stp, peerid),
	       stpeer_state(stp));
    }
    status = raise_interrupt_level(NETS_DISABLE);
    switch (stp->state) {
      case STUN_PS_DEAD:
      case STUN_PS_CLOSED:
      case STUN_PS_OPENING:
      case STUN_PS_OPENWAIT:
	stun_newstate(stp, STUN_PS_CONNECTED);
        reg_invoke_stun_trap(stp);
	if (stp->primary_t) {
	    stp->secondary_t = t;
	} else {
	    stp->primary_t = t;
	}

      case STUN_PS_CONNECTED:
	if (t != stp->primary_t && t != stp->secondary_t) {
	    /*
	     * Interesting.  We received an incoming packet but not
	     * on the connection we thought we were connected to.  Must
	     * be reestablishing contact.  Abort previous connection and
	     * then use this one as the new primary.  No longer need
	     * secondary.
	     */
            if (stun_debug) {
                  buginf("\nSTUN: PHDR: bad connection state %d, %s",
                         stp->state, stpeer_id(stp, peerid));
            }
            reg_invoke_ip_tcpdriver_close(t);
	}
	if (sth->len) {
	    bcopy((uchar *) sth, (uchar *) &stp->inhdr,
		  sizeof(stunhdrtype) +
		  stun_pinfo[stp->group]->addrlength - 1);
	    reg_invoke_ip_tcpdriver_update(t, sttcpd_rddata, sth->len, stp);
	} else {
	    stp->inhdr.op = STUN_OP_EMPTY;
	    reg_invoke_ip_tcpdriver_update(t, sttcpd_rdhdr,
					   sizeof(stunhdrtype) +
					   stun_pinfo[stp->group]->addrlength
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
	if (stun_debug) {
	    buginf("\nSTUN: PHDR: bad connection state %d, %s",
		   stp->state, stpeer_id(stp, peerid));
	}
	sttcpd_abort(stp);
	return;
    }
}

/*
 * sttcpd_rdhdr
 * tcpdriver implementation of stun peer handler.  This
 * routine accepts the first part of an incoming packet from
 * a remote peer.  If there is data it resets the reader
 * function of the driver struct.
 */

void sttcpd_rdhdr (tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie)
{
    int bytes, s;
    uchar *p, *dest;
    paktype *pak=0;
    char peerid[MAX_PEER_NAME];
    stunpeertype *stp;

    stp = (stunpeertype *)cookie;

    if (!stp || stp->inhdr.op != STUN_OP_EMPTY) {
	buginf("\nSTUN: HDR: state wrong");
	if (stp) {
	    stp->inhdr.op = STUN_OP_EMPTY;
	}
	reg_invoke_ip_tcpdriver_abort(t);
	return;
    }
    bytes = tcp_unread(tcb);
    if (stp->state != STUN_PS_CONNECTED ||
	thresh != (sizeof(stunhdrtype) +
		   stun_pinfo[stp->group]->addrlength - 1) ||
	bytes < thresh) {
	if (stun_debug) {
	    buginf("\nSTUN: HDR: %s bad state, thresh %d, bytes %d",
		   stpeer_id(stp, peerid), thresh, bytes);
	}
	reg_invoke_ip_tcpdriver_abort(t);
	return;
    }	
    dest = (uchar *)&stp->inhdr;
    do {
	s = tcp_getstring(tcb, &((uchar *) p), &bytes, thresh);
	if (s != OK) {
	    if (stun_debug) {
		buginf("\nSTUN: HDR: getstring failed %d - %i(%d) -> %d",
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

    if (stp->inhdr.magic != STUN_MAGIC) {
	if (stun_debug) {
	    errmsg(&msgsym(BADMAGIC, STUN), "HDR", 
		   STUN_MAGIC, stp->inhdr.magic, 3);
	}
	reg_invoke_ip_tcpdriver_abort(t);
	stp->drops++;
	stp->inhdr.op = STUN_OP_EMPTY;
	return;
    }
    if (stun_debug) {
	if ((stp->inhdr.op != STUN_OP_DATA) &&
	    (stp->inhdr.op != STUN_OP_PASS)) {
	    buginf("\nSTUN: Received %s opcode from %s at %d",
		   st_op2str(stp->inhdr.op), stpeer_id(stp, peerid),
		   ELAPSED_TIME(stun_start_time)/ONESEC);
	}
    }
    /* Any data at all is equivalent to a keepalive */
    GET_TIMESTAMP(stp->last_keepalive);
    if (stp->inhdr.op == STUN_OP_VREQ) {
	int result;

	result = TCPDSEND_QFULL;
	pak = stpak(STUN_OP_VREP, 0, NULL, 0);
	if (pak)
	    result = reg_invoke_ip_tcpdriver_send(t, pak, TRUE);
	if (!pak || TCPDSEND_ERROR(result)) {
	    if (!pak)
		errmsg(&msgsym(NOBUF, STUN), "STUN", "send version reply");
	    if (stun_debug) {
		buginf("\nSTUN: %s: send version reply, nopak/fail",
		       stpeer_id(stp, peerid));
	    }
	    if (pak) {
		datagram_done(pak);
	    }
	}
    }
    if (stp->inhdr.len) {
	reg_invoke_ip_tcpdriver_update(t, sttcpd_rddata, stp->inhdr.len, stp);
     } else {
        stp->inhdr.op = STUN_OP_EMPTY;
    }
   if (pak) 
      stp->bytes_rx += pak->datagramsize;
   stp->pkts_rx++;
}

void sttcpd_rddata (tcp_encaps *t, tcbtype *tcb, int thresh, void *cookie)
{
    stunhdrtype *sth;
    int bytes, s;
    paktype *pak;
    uchar *dest, *p;
    char errbuff[MAX_PEER_NAME];
    ushort opcode;
    stunpeertype *stp;
    sdlc_data_block *sdb;
    
    stp = (stunpeertype *)cookie;

    if (!stp || stp->inhdr.op == STUN_OP_EMPTY || !stp->inhdr.len) {
	if (stun_debug) {
	    buginf("\nSTUN: DATA: peer %s bad state, op %d len %d",
		   (stp ? stpeer_id(stp, errbuff) : ""),
		   (stp ? stp->inhdr.op : -1), (stp ? stp->inhdr.len : -1));
	}
	reg_invoke_ip_tcpdriver_abort(t);
	return;
    }
    sth = &stp->inhdr;
    pak = getbuffer(sth->len);
    if (!pak) {
	if (stun_debug)
	    buginf("\nSTUN: DATA: no memory");
	do {
	    s = tcp_getstring(tcb, &p, &bytes, thresh);
	    if (s != OK) {
		if (stun_debug) {
		    buginf("\nSTUN: DATA: drain getstring failed %d", s);
		}
		sth->op = STUN_OP_EMPTY;
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
	s = tcp_getstring(tcb, &p, &bytes, thresh);
	if (s != OK) {
	    if (stun_debug) {
		buginf("\nSTUN: DATA: getstring failed %d", s);
	    }
	    sth->op = STUN_OP_EMPTY;
	    stp->drops++;
	    reg_invoke_ip_tcpdriver_abort(t);	/* assume out of sync */
	    stif_nukepak(pak);
	    return;
	}
	bcopy(p, dest, bytes);
	dest += bytes;
	thresh -= bytes;
    } while (thresh > 0);
    opcode = sth->op;
    switch (opcode) {
      case STUN_OP_LINKDOWN_REQ:
      case STUN_OP_PASS:
      case STUN_OP_LINKUP_REQ:
        /* if we are running stun tg, save SNRM address to send on sdlc SNRM */
        if (stp->tg_active) {
           sdb = SDB_FROM_PEER(stp);
           if (sdb)
              sdb->tg_snrm_addr = pak->datagramstart[0];
        }
      case STUN_OP_LINKUP_RSP:
      case STUN_OP_LINKDOWN_RSP:
      case STUN_OP_ABORT_REQ:
      case STUN_OP_ABORT_RSP:
      case STUN_OP_RD_REQ:
      case STUN_OP_DATA:
      case STUN_OP_SIM:
      case STUN_OP_QUENCH_ON:
      case STUN_OP_QUENCH_OFF:
      case STUN_OP_LINK_ESTAB:
	pak->enctype = ET_NULL;
	pak->if_output = firstsw_or_null(stp->stun_idb);
	stun_fromnet_forward(pak, stp, opcode);
	break;

      case STUN_OP_IGNORE:
	stif_nukepak(pak);
	break;

      default:
	if (stun_debug)
	    buginf("\nSTUN: DATA: bad op %d (%s) from %s",
		   opcode, st_op2str(sth->op), stpeer_id(stp, errbuff));
	stp->drops++;
	stif_nukepak(pak);
	break;
    }
    sth->op = STUN_OP_EMPTY;
    reg_invoke_ip_tcpdriver_update(t, sttcpd_rdhdr,
				   sizeof(stunhdrtype) +
				   stun_pinfo[stp->group]->addrlength - 1,
				   stp);
}

/*
 * sttcpd_listen
 * Given a tcb opened by tcp_listen upon an incoming connection, insert
 * it into the tcpdriver structures.  We don't have enough information
 * here to easily validate so we wait for an incoming packet.  If prior
 * to receiving that first packet this side tries to open it's open.
 */

void sttcpd_listen (tcbtype *tcb)
{
    tcp_encaps *t;
    tcp_encaps tmp;

    memset(&tmp, 0, sizeof(tcp_encaps));
    tmp.readf = sttcpd_phdr;
    tmp.closef = sttcpd_fini;
    tmp.flushf = sttcpd_flush;
    tmp.flow_ctrl = NULL;
    tmp.inthresh = sizeof(stunhdrtype) + STUN_MAX_ADDR_LENGTH - 1;
    tmp.cookie = NULL;
    /* set the TCP window large enough to hold at least 2 of the largest frames */
    t = reg_invoke_ip_tcpdriver_passive_open(tcb, 10240, &tmp);
    if (!t) {
	if (stun_debug) {
	    errmsg(&msgsym(BADPASSIVEOPEN, STUN),
                   tcb->foreignhost.ip_addr, (ulong)tcb->foreignport, (ulong)tcb->localport);
	}
	tcp_close(tcb);
    } else {
	/* set tcp queue size to default until peer information is available */
	reg_invoke_ip_tcpdriver_set_oqmax(t, STUN_MAX_QLEN);
	if (stun_debug) {
	    errmsg(&msgsym(PASSIVEOPEN, STUN),
                   reg_invoke_ip_tcpdriver_fhost(t),
                   (ulong)reg_invoke_ip_tcpdriver_fport(t),
                   (ulong)reg_invoke_ip_tcpdriver_lport(t));
	}
    }
    return;
}

/*
 * sttcpd_fini
 * routine called when the tcpdriver detects a close on
 * the tcpdriver stream.
 */

void sttcpd_fini (tcp_encaps *t, void *cookie)
{
    leveltype status;
    char peerid[MAX_PEER_NAME];
    stunpeertype *stp;
    stunpeertype *root_stp;
    int i;

    stp = root_stp = (stunpeertype *)cookie;

    if (!stp) {				/* unknown peer abort */
	if (stun_debug)
	    buginf("\nSTUN: unknown peer closed");
	return;
    }
    status = raise_interrupt_level(NETS_DISABLE);
    stp->inhdr.op = STUN_OP_EMPTY;
    stp->primary_t = NULL;
    stp->secondary_t = NULL;
    if (stun_debug)
	errmsg(&msgsym(TCPFINI, STUN), stpeer_id(stp, peerid),
	       stpeer_state(stp));
    stun_newstate(stp, STUN_PS_CLOSED);
    reg_invoke_stun_trap(stp);
    reset_interrupt_level(status);
    /*
     * Take down the rest of the priority peers if priority is configured.
     */
    if (stp->priority_top) {
        root_stp = stp->priority_top;
        if (root_stp->priority) {
            for (i = 0; i <= STUN_PRIORITY_COUNT; i++) {
                if ((root_stp->priority_peers[i] != stp) && (root_stp->priority_peers[i]->abort))
                    (*root_stp->priority_peers[i]->abort)(root_stp->priority_peers[i]);
            }
        }
    }
}


/*********************************************************************
 *
 * Misc routines.
 * 
 *********************************************************************/

/*
 * stun_newstate
 *
 * Make the transition to a new state in stunpeertype. Also, for SDLC
 * Local Ack'd connections, drive the SDLC Local Ack FSM if we are going
 * out of CONNECTED state.
 */
void stun_newstate (stunpeertype *stp, stunstatetype newstate)
{
    char peerid[MAX_PEER_NAME];
    stunpeertype *root_stp;

    if (stp->local_ack_active)
    {
	if ((stp->state == STUN_PS_CONNECTED) && 
	    (newstate   != STUN_PS_CONNECTED))
	{
            root_stp = stp;
            if (stp->priority_top)
                root_stp = stp->priority_top;
	    slack_fsm(stp,SLACK_NETWORK,SLACK_NETWORK_SESSION_LOSS);
        }
    }
    if (stun_debug) {
     buginf("\nSTUN: Change state for peer %s (%s->%s)",
             stpeer_id(stp, peerid), st_state(stp->state), st_state(newstate));
    }
    stp->state = newstate;
    GET_TIMESTAMP(stp->last_keepalive);    
}

/*
 * stun_active
 * Return an indication that the Stunnel is turned on.
 */

boolean stun_active (ulong sport, ulong dport, ipaddrtype fhost)
{
    return(stun_first_idb != NULL);
}

/*
 * stun_periodic
 * Do periodic things that need to be done.  Currently the only 
 * things we do are:
 *   (1) scan the remote peers for dead ones and reactivate, them, 
 *   (2) call protocol specific code, 
 *   (3) check SDLC Local Ack connections for hung waiting for disconnect.
 */

static void stun_periodic_per_peer (
    stunpeertype *stp)
{
    if (stp->state == STUN_PS_DEAD) {
	if ((stp->type == STUN_PT_IF) || (stp->type == STUN_PT_FR)) {
	    stun_newstate(stp, STUN_PS_CONNECTED);
            reg_invoke_stun_trap(stp);
	} else {
	    stun_newstate(stp, STUN_PS_CLOSED);
            reg_invoke_stun_trap(stp);
        }
    } else {
	if (((stp->slack_state == SLACK_AWAIT_LINKDOWN_UA) ||
	     (stp->slack_state == SLACK_AWAIT_ABORT_RSP)   ||
	     (stp->slack_state == SLACK_AWAIT_ABEND_DISC) ||
	     (stp->slack_state == SLACK_AWAIT_NORM_DISC)) &&
	    (AWAKE(stp->dead_timer))) {
	    slack_fsm(stp,SLACK_TIMER,SLACK_TIMER_DISC);
	}
    }

}

void stun_periodic (void)
{
    hwidbtype *idb;
    stunpeertype *stp;
    int i;

    for (idb = stun_first_idb; idb; idb = IDB_NEXT(idb)) {
	for (stp = (stunpeertype *) idb->stun_Q.qhead; stp;
	       stp = stp->next) {
	    stun_periodic_per_peer(stp);
	    if (stp->priority) {
		for (i = 1; i <= STUN_PRIORITY_COUNT; i++)
		    stun_periodic_per_peer(stp->priority_peers[i]);
	    }
	}
	if (idb->stun_group)
	  (*(stun_pinfo[idb->stun_group]->long_periodic_hook))(idb);
    }
}

/*
 * show_stun
 * Display current serial tunnel configuration
 */


static void stun_print_peer_info (
    hwidbtype *idb,
    stunpeertype *stp,
    int i)
{
    char buff[MAX_PEER_NAME];

    if (i == 0) {
	stun_sprintf_addr(stp->addr, stp->all_addr, stp->group, buff);
	printf("\n%s", buff);
	if (stp->input_addr_inuse) {
	    stun_sprintf_addr(stp->input_addr, stp->all_addr, stp->group,
			      buff);
	    printf("/%s  ", buff);
	} else {
	    printf("     ");
	}
    }
    switch (stp->type) {
    case STUN_PT_NONE:
    case STUN_PT_TCPD:
	if (!(stp->priority_top)) {
	    printf("TCP %17i", stp->ipaddr);
	    if (stp->priority)
		i = 0;
	}
	if ((stp->priority_top) || (stp->priority)) {
	    printf("\n   %s Priority Peer    ", stun_priority_peername(i));
	}
	break;
    case STUN_PT_IF:
	printf("IF  %17s", stp->outidb->hw_namestring);
	break;
    case STUN_PT_FR:
	printf("FR  %10s %6d", stp->outswidb->namestring, stp->dlci);
	break;
    case STUN_PT_CLS:
        printf("FR-L %9s %4d %2x",stp->outswidb->namestring, 
                stp->dlci, stp->lsap);
    default:
	break;
    }

    printf("%10s %1c %-8u  %-8u  %-8u ", stpeer_state(stp),
         (stp->local_ack_active ? '*' : ' '),
         stp->pkts_rx, stp->pkts_tx, stp->drops);
    (*stun_pinfo[idb->stun_group]->little_show_hook)(FALSE, idb,
						     stp);
}

void show_stun (parseinfo *csb)
{
    stunpeertype *stp;
    hwidbtype *idb;
    stun_protocol_list *spl;
    int i;
    boolean first, local_ack_found;
    
    if (!stun_name) {
	printf("%% STUN transport not enabled");
	return;
    }
    
    if (GETOBJ(int,1)) {
	for (spl = (stun_protocol_list *) stun_pinfoQ.qhead; spl;
	     spl = (stun_protocol_list *) spl->next) {
	    if (partialmatch(spl->name, GETOBJ(string,1))) {
		(*spl->q->show_hook)();
		return;
	    }
	}
    }

    automore_enable(NULL);
    printf("\nThis peer: %i", stun_name);

    first = TRUE;
    local_ack_found = FALSE;
    FOR_ALL_HWIDBS(idb) {
        if ((idb->enctype == ET_STUN) && (idb->stun_group)) {
           if (first) {
                first = FALSE;
           } else {
	       automore_conditional(0);
	   }
           printf("\n\n %c%s %s%s",
                  ((idb == stun_first_idb) ? '*' : ' '),
                  idb->hw_namestring,
                  (idb->firstsw->description ? "-- " : ""),
                  (idb->firstsw->description ? idb->firstsw->description : ""));
           printf(" (group %u [%s])", idb->stun_group,
                  stun_pinfo[idb->stun_group]->name);
           if (idb->stun_Q.qhead) {
               printf("\n                              state       rx_pkts   tx_pkts     drops ");
               (*stun_pinfo[idb->stun_group]->little_show_hook)(TRUE, idb,
                                                                NULL);
           }
           for (stp = (stunpeertype *) idb->stun_Q.qhead; stp;
                stp = stp->next) {
	       stun_print_peer_info(idb, stp, 0);
	       if (stp->priority) {
		   for (i = 1; i <= STUN_PRIORITY_COUNT; i++) {
		       if (stp->priority_peers[i]) {
			   stun_print_peer_info(idb, stp->priority_peers[i],
						i);
		       }
		   }
	       }
	       if (stp->local_ack_active)
		   local_ack_found = TRUE;     /* At least one local-ack */
	   }
           if (idb->stun_Q.qhead) {
               printc('\n');
	   }
       }
    }
    /*
     * If we found any STUN connections that are locally terminated, then
     * produce a locak-ack-specific portion of the report.
     */
    if (local_ack_found) {
	show_stun_local_ack();
    }
    automore_disable();
}

void show_stun_local_ack (void)
{
    hwidbtype		*idb;
    stunpeertype 	*stp;
    boolean 		first;
    boolean		first_local_ack, first_local_ack_this_idb;
    char 		buff[MAX_PEER_NAME];

    first = TRUE;
    first_local_ack = TRUE;
    /*
    **  Loop through all idb's, stopping to examine those that are STUN
    **  encapsulated and have a group definition.
    */
    FOR_ALL_HWIDBS(idb) {
        if ((idb->enctype == ET_STUN) && (idb->stun_group)) {
	    if (first) {
		first = FALSE;
	    } else {
		automore_conditional(0);
	    }
	    /*
	    **  Loop through all STUN peers for this idb, reporting on
	    **  those (if any) that have local acknowledgement active.
	    */
	    first_local_ack_this_idb = TRUE;
	    for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; 
	         stp = stp->next) {
		if (stp->local_ack_active) {
		    if (first_local_ack_this_idb) {
			if (first_local_ack) {
			    printf("\nSDLC Local Acknowledgement:");
			    first_local_ack = FALSE;
			}
			/*
			**  Print out interface-specific stuff.
			*/
			printf("\n\n %c%s %s%s",
			       ((idb == stun_first_idb) ? '*' : ' '),
			       idb->hw_namestring,
			       (idb->firstsw->description ? "-- " : ""),
			       (idb->firstsw->description ? idb->firstsw->description : ""));
			printf(" (group %u [%s])", idb->stun_group,
			       stun_pinfo[idb->stun_group]->name);
			/*
			**  Print out SDLC Local Ack title.
			*/
			printf("\n%-5s  %-3s %-17s %-15s %-4s %-4s %-8s %-8s",
			    " ", " ", " ",
			    "slack_state", "conn", "disc", 
			    "iframe_s", "iframe_r");
			first_local_ack_this_idb = FALSE;
		    }                     
		    /*
		    **  Print out a single line of data for this SDLC Local Ack.
		    */
		    stun_sprintf_addr(stp->addr, stp->all_addr, stp->group, 
				      buff);
		    printf("\n%s", buff);
		    if (stp->input_addr_inuse) {
			stun_sprintf_addr(stp->input_addr, stp->all_addr, 
					  stp->group, buff);
			printf("/%s  ", buff);
		    } else {
			printf("     ");
		    }
                    if (stp->type == STUN_PT_CLS) {
                       printf("%-4s %-6d %-4x ",
                               "FR", stp->dlci, stp->lsap);
		    }
		    else { 
                       printf("%-3s %17i", 
            		   "TCP", stp->ipaddr);
		    }
		    printf(" %-15s %-4d %-4d %-8d %-8d",
			slack_prstate(stp->slack_state), 
			stp->slack_stats.num_connects,
			stp->slack_stats.num_disconnects,
			stp->slack_stats.i_frames_sent,
			stp->slack_stats.i_frames_rcvd);
		}
	    }
	}
    }
}


/*
 * Shut down STUN processing on an interface.
 */ 
static void stun_nail_priority_peers (
    stunpeertype *stp)
{
    int i;
    stunpeertype *stphold;

    if (!stp)
	return;
    if (stp->priority) {
	for (i = 1; i <= STUN_PRIORITY_COUNT; i++) {
	    stphold = stp->priority_peers[i];
	    if (stphold) {
		stnail_peer(stphold);
		free(stphold);
	    }
	}
    }
    stnail_peer(stp);
    free(stp);
}

/*
 * sttrap_peers
 * send a trap for each peer before dequeueing and destroying it
 *
 */
static void sttrap_peers(hwidbtype *idb)
{
    stunpeertype *stp;
    stunstatetype savestate; 

    for (stp = (stunpeertype *) idb->stun_Q.qhead; stp;
         stp = stp->next) {
        savestate = stp->state;
        stp->state = STUN_PS_CLOSED;
        reg_invoke_stun_trap(stp);
        stp->state = savestate;
    }  
}

static void stun_shutintcore (
     hwidbtype *idb)
{
    leveltype status;
    stunpeertype *stp;
    int addr;

    if (idb->enctype != ET_STUN)
	return;
    status = raise_interrupt_level(ALL_DISABLE);
    sttrap_peers(idb);   
    while ((stp = dequeue(&idb->stun_Q))) {
	stun_nail_priority_peers(stp);
    }
    if (stun_pinfo[idb->stun_group]) {
	if (stun_pinfo[idb->stun_group]->shut_interface)
	    (*stun_pinfo[idb->stun_group]->shut_interface)(idb);
    }
 
    /*
     * Make sure sdbs are cleaned up.
     */
    for (addr = SDLC_MIN_ADDRESS; addr <= SDLC_MAX_ADDRESS; addr++) {
	if (idb->sdlc_data[addr] != NULL) {
	    free(idb->sdlc_data[addr]);
	    idb->sdlc_data[addr] = NULL;
	}
    }
    idb->sdlc_current_polled = 0;
    idb->stun_group = 0;
    idb->sdlc_t1 = SDLC_DEFAULT_T1;
    idb->sdlc_n1 = SDLC_DEFAULT_N1(idb);
    idb->sdlc_n2 = SDLC_DEFAULT_N2;
    idb->sdlc_k = SDLC_DEFAULT_K;
    idb->sdlc_line_speed = 0;
    idb->stun_sdlc_encap = ET_STUN;
    idb->sdlc_uc_delay = 0;
    idb->sdlc_slow_polled = 0;
    TIMER_STOP(idb->sdlc_slow_poll_timer);
    idb->sdlc_slow_poll_interval = SDLC_DEFAULT_SLOW_POLL;
    idb->sdlc_poll_wait = SDLC_DEFAULT_POLL_WAIT;
    idb->sdlc_poll_pause_timer = SDLC_DEFAULT_POLL_PAUSE_TIMER;
    idb->sdlc_poll_limit_value = SDLC_DEFAULT_POLL_LIMIT_VALUE;
    TIMER_STOP(idb->sdlc_top_timer);
    idb->sap_prilist = 0;
    idb->sna_la_prilist = 0;

    reset_interrupt_level(status);
}

void stun_shutinterface (hwidbtype *idb)
{
    hwidbtype *tidb;

    stun_shutintcore(idb);
    FOR_ALL_HWIDBS(tidb) {
	if (tidb->stun_Q.qhead) {
	    break;
	}
    }
    stun_first_idb = tidb;
    return;
}

/*
 * Shut down STUN processing on *all* interfaces.
 */
static void stun_shutallinterfaces (
    int group)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
	if ((group < 0) || (group == idb->stun_group)) {
	    stun_shutintcore(idb);
	}
    }
    stun_first_idb = NULL;
}


static void stun_parse_schema (parseinfo *csb)
{
    stun_protocol_list *spl;
    stun_protocol_info *spi;
    
    int i;
    int offset, len;
    char fmt;
    boolean onlyall;
    char name[NAMEDTHINGNAMELEN + 1];

    sstrncpy(name, GETOBJ(string,1), NAMEDTHINGNAMELEN+1);
    spl = find_namedthing(&stun_pinfoQ, name);
    if (spl) {
	/* You can't mess around with the predefined protocols */
	if (spl->q->permanent) {
	    printf("\nstun schema - the predefined \"%s\" protocol cannot be changed",
		   name);
	    return;
	}
	if (!csb->sense) {	/* Delete it */
	    for (i = STUN_MIN_GROUP; i < STUN_MAX_GROUP; i++)
	      if (stun_pinfo[i] == spl->q) {
		  stun_shutallinterfaces(i);
		  stun_pinfo[i] = NULL;
	      }
	    unqueue(&stun_pinfoQ, spl);
	    free(spl->q);
	    free(spl);
	    return;
	}
    }
    onlyall = FALSE;
    offset = GETOBJ(int,1);
    len = GETOBJ(int,2);
    fmt = GETOBJ(int,3);
    if ((len > ((fmt == 'x') ? (STUN_MAX_ADDR_LENGTH) :
		(STUN_MAX_D_OR_O_ADDR_LENGTH))) ||
	(len < STUN_MIN_ADDR_LENGTH)) {
	printf("\nlength illegal -- must be between %u and %u for the \'%c\' format",
	       STUN_MIN_ADDR_LENGTH, ((fmt == 'x') ?
				      (STUN_MAX_ADDR_LENGTH) :
				      (STUN_MAX_D_OR_O_ADDR_LENGTH)),
	       fmt);
	if (len == 0) {
	    printf("\nif you wish a length of zero, use the predefined \"noaddress\" STUN protocol");
	}
	return;
    }
    /* Old *must* match new... */
    if (spl) {
	if ((spl->q->addroffset != offset) ||
	    (spl->q->addrlength != len) ||
	    (spl->q->onlyall != onlyall)) {
	    printf("\nSchema %s already defined with different values.");
	    printf("\nUndefining all groups associated with it");
	    for (i = STUN_MIN_GROUP; i < STUN_MAX_GROUP; i++)
	      if (stun_pinfo[i] == spl->q) {
		  stun_shutallinterfaces(i);
		  stun_pinfo[i] = NULL;
	      }
	}
	spl->q->addroffset = offset;
	spl->q->addrlength = len;
	spl->q->addrfmt = fmt;
	spl->q->onlyall = onlyall;
	return;
    } else {
	spi = stun_new_generic(name, len, offset, fmt, onlyall,
			       FALSE, FALSE, FALSE);
	if (spi) {
	    stun_add_protocol(spi, name);
	}
    }
}

/*
 * stun_command
 * global stunnel commands.
 */

void stun_command (parseinfo *csb)
{
    ulong tstun_name;		/* Don't change name until interface shut! */
    stun_protocol_list *spl;
    int group;
    static char *no_stun =
	"\nSTUN not enabled -- give a stun peer-name first.";
    stunpeertype *stp;
    stun_protocol_info *gpi;
    hwidbtype *idb;
    uint i;

    if (csb->nvgen) {
	int i;

	switch (csb->which) {
	  case STUN_PEER:
	    nv_write(stun_name != (ipaddrtype) 0, "!\n%s %i",
		     csb->nv_command, stun_name);
            if (IsClsdriverRunning) 
                nv_add(TRUE,"  CLS  ");
            if (UserConfiguredLisnSap)
                nv_add(TRUE,"%x ", UserConfiguredLisnSap);
	    break;
	  case STUN_POLL:
	  case STUN_PRIMARY:
	    break;
          case STUN_QUICK_RESPONSE:
            nv_write(quick_response,"%s ",csb->nv_command);
            break;
	  case STUN_SCHEMA:
	    for (spl = (stun_protocol_list *) stun_pinfoQ.qhead; spl;
                 spl = (stun_protocol_list *) spl->next) {
                (*spl->q->global_nv_hook)(spl->q, csb);
            }
	    break;
	  case STUN_PROTOCOL:
	    for (i = 0; i <= STUN_MAX_GROUP; i++) {
		nv_write((stun_pinfo[i] != NULL), "%s %d %s",
			 csb->nv_command, i, stun_pinfo[i]->name);
	    }
	    break;
	  case STUN_KEEPALIVE:
            if (stun_keepalive_interval) {
               if (stun_keepalive_interval != STUN_KEEPALIVE_DEFAULT_INTERVAL)
                  nv_write(TRUE, "%s %d", csb->nv_command,
                           stun_keepalive_interval/ONESEC);
               else
                  nv_write(TRUE, "%s", csb->nv_command);
	    }
	    break;
	  case STUN_KEEPALIVE_COUNT:
            if (stun_keepalive_interval) {
               nv_write(stun_keepalive_count != STUN_KEEPALIVE_DEFAULT_COUNT,
	             "%s %d", csb->nv_command, stun_keepalive_count);
	    }
	    break;
          case STUN_SLACK_FSM_TIMEOUT:
            nv_write(stun_slack_fsm_timeout != STUN_SLACK_FSM_TIMEOUT_DEFAULT,
                     "%s %d", csb->nv_command, stun_slack_fsm_timeout);

            break;
	}
	return;
    }

    switch (csb->which) {
      case STUN_PEER:
	if (csb->sense) {
	    tstun_name = GETOBJ(paddr,1)->ip_addr;

            if (GETOBJ(int,1)) { /* its cls !! */
               if (GETOBJ(int,2))  /* user configured listening sap */
                   UserConfiguredLisnSap = GETOBJ(int,2);
               else
                   UserConfiguredLisnSap = 0;
               if (IsClsdriverRunning == FALSE)
                   stcls_start(); 
            } /* of if (GETOBJ(int,1)) */ 
	} else {
            /* Remove peer-name only if there are no priority-lists using stun */
            if (prilist_uses_stun_group(0)) {
               printf("\nFirst remove the priority-list containing a reference to a stun group");
               return;
            }
            tstun_name = (ipaddrtype) 0;
            IsClsdriverRunning = FALSE; /* this should kill the CLS-LACK server process */
            UserConfiguredLisnSap = 0;
	}
	if (tstun_name == (ipaddrtype) 0) {
            /*
             * When removing a peer-name, the following stun global parameters must
             * also be removed :
             *  --stun protocol-group          --stun keepalive-count
             *  --stun remote-peer-keepalive   --stun slack-fsm-timeout
             *
             * Calling stun_shutallinterfaces(-1) cleans up each stun interface.
             */
	    stun_shutallinterfaces(-1);
            stun_keepalive_count = STUN_KEEPALIVE_DEFAULT_COUNT;
            stun_keepalive_interval = 0;
            stun_slack_fsm_timeout = STUN_SLACK_FSM_TIMEOUT_DEFAULT;
            for (i = 0; i <= STUN_MAX_GROUP; i++)
               stun_pinfo[i] = NULL;
	}
	stun_name = tstun_name;
	break;
      case STUN_POLL:
      case STUN_PRIMARY:
	printf("\nProxy polling is obsolete, use STUN local-ack.");
	break;
      case STUN_QUICK_RESPONSE:
        if (csb->sense) 
            quick_response = TRUE;
        else
            quick_response = FALSE;    
        break;
      case STUN_SCHEMA:
	if (!stun_name && csb->sense) {
	    printf(no_stun);
	    break;
	}
	stun_parse_schema(csb);
	break;
      case STUN_PROTOCOL:
	if (!stun_name) {
	    printf(no_stun);
	    break;
	}
	group = GETOBJ(int,1);
	spl = find_namedthing(&stun_pinfoQ, GETOBJ(string,1));
	if (!spl) {
	    printf("\nunknown protocol - %s", GETOBJ(string,1));
	    return;
	}
	if (csb->sense) {
	    if ((stun_pinfo[group] != NULL) && (stun_pinfo[group] != spl->q)) {
		printf("\nold protocol-group for %d was %s.", group,
		       stun_pinfo[group]->name);
		printf("\nOld STUN information for interfaces in group %d being deleted",
		       group);
		stun_shutallinterfaces(group);
	    }
            if (spl->q->uses_tg) {
                if (stun_add_tg(group))
                    stun_pinfo[group] = spl->q;
            } else
                stun_pinfo[group] = spl->q;

	} else {
            /* if there is a stun priority-list using this group do not remove it. */
            if (prilist_uses_stun_group(group)) {
               printf("\nFirst remove the priority-list containing a reference to stun group %d",
                      group);
               return;
            }
	    stun_shutallinterfaces(group);
            if (stun_pinfo[group]->uses_tg)
                stun_delete_tg(group);
	    stun_pinfo[group] = NULL;
	}
	break;

      case STUN_KEEPALIVE:
        if (csb->sense) {
           stun_keepalive_interval = GETOBJ(int,1) * ONESEC;
           for(idb = stun_first_idb; idb; idb = IDB_NEXT(idb)) {
              if ((gpi = stun_pinfo[idb->stun_group]) == NULL)
                  continue;
              for (stp = (stunpeertype *) idb->stun_Q.qhead; stp;
                  stp = stp->next) {
		  GET_TIMESTAMP(stp->last_keepalive);
              }
           }
        }
        else {
            stun_keepalive_interval = 0;
        }
        break;
      case STUN_KEEPALIVE_COUNT:
        if (csb->sense) {
           stun_keepalive_count = GETOBJ(int,1);
        }
        else {
            stun_keepalive_count = STUN_KEEPALIVE_DEFAULT_COUNT;
        }
	break;
      case STUN_SLACK_FSM_TIMEOUT:
        if (csb->sense) {
           stun_slack_fsm_timeout = GETOBJ(int,1);
        }
        else {
           stun_slack_fsm_timeout = STUN_SLACK_FSM_TIMEOUT_DEFAULT;
        }
        break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


static stunpeertype *stun_create_peer (
    hwidbtype *idb,
    boolean allchosen,
    stuntypetype type,
    boolean local_ack_capable, 
    boolean priority, 
    boolean direct,
    uchar *addr,
    int port,
    ipaddrtype ipaddr,
    idbtype *outswidb,
    boolean input_addr_inuse,
    char *input_addr,
    ushort dlci,
    byte    lsap,
    byte  cls_priority)
{

    stunpeertype *stp;
    hwidbtype *outidb;
    
    stp = malloc(sizeof(stunpeertype));
    if (!stp) {
	printf(nomemory);
	return(NULL);
    }
    stp->group = idb->stun_group;
    stp->all_addr = allchosen;
    stp->slack_snrm_pak = NULL;
    stp->stored_xid_response=NULL;
    stp->slack_xid_rsp_received=FALSE;
    stp->now_xid_passthru = FALSE;
    stp->stun_idb = idb;
    stp->type = type;
    stp->local_ack_capable = local_ack_capable;
    stp->local_ack_active = FALSE;
    stp->local_ack_sim_sent = FALSE;
    stp->local_ack_tg_load = FALSE;
    stp->slack_flow_control = FALSE;
    stp->tg_active = stun_pinfo[idb->stun_group]->uses_tg;
    stp->priority = priority;
    GET_TIMESTAMP(stp->last_keepalive);
    stp->slack_retry_count = SLACK_RETRY_OFF;
    stp->slack_ua_rcvd = FALSE;
    bcopy(addr, stp->addr, stun_pinfo[stp->group]->addrlength);
    stp->input_addr_inuse = input_addr_inuse;
    if (input_addr_inuse) {
        bcopy(input_addr, stp->input_addr,
	      stun_pinfo[stp->group]->addrlength);
    }
    switch (type) {
    default:
    case STUN_PT_TCPD:
	stp->state = STUN_PS_CLOSED;
	stp->sender = sttcpd_sender;
	stp->abort = sttcpd_abort;
	stp->close = sttcpd_close;
	stp->connect = sttcpd_connect;
	stp->port = port;
	stp->ipaddr = ipaddr;
	stp->inhdr.op = STUN_OP_EMPTY;
	break;

    case STUN_PT_CLS:
        stp->outswidb = outswidb;
        stp->outidb = outswidb->hwptr;
        stp->dlci = dlci;
        stp->cls_priority = cls_priority;
        if (stp->local_ack_capable) 
            if (stcls_initpeer(stp, outswidb,lsap))
                break; 

        printf( "STUN: can not create peer \n");
        return(NULL);


    case STUN_PT_FR:
	stp->outswidb = outswidb;
	stp->dlci = dlci;

    case STUN_PT_IF:
        outidb = outswidb->hwptr;
	if (direct)
	    stp->state = STUN_PS_DIRECT;
	else
	    stp->state = STUN_PS_CONNECTED;
	stp->sender = stif_direct;
	stp->abort = stif_abort;
	stp->close = stif_close;
	stp->connect = stif_connect;
	stp->outidb = outidb;
	if (!supports_maxdgram(outidb)) {
	    printf("\nWarning: The maximum datagram size on interface %s will be %d bytes",
		   outidb->hw_namestring, outidb->maxmtu); 
	} else {
	    int buffersize;
	    buffersize = (outidb->maxmtu > MAXDGRAM_STUN) ?
		outidb->maxmtu : MAXDGRAM_STUN;
	    if (!(*outidb->set_maxdgram)(outidb, buffersize,
					 (buffersize +
					  HDLC_HEADER_SIZE))) {
		printf("\nWarning: The maximum datagram size on interface %s will be %d bytes",
		       outidb->hw_namestring, outidb->maxmtu);
	    }
	}
	break;
    }
    if (!(*stun_pinfo[idb->stun_group]->peer_change_hook)(idb, stp, TRUE)) {
	/* Problems with this stun peer definition.  Nuke it. */
	free(stp);
	stp = NULL;

    }
    return(stp);
}

/* stun_check_localack
 * returns FALSE if local-ack is invalid on the interface
 *         TRUE otherwise
 */
static boolean stun_check_localack(hwidbtype *idb, uchar addr)
{
    if (stun_pinfo[idb->stun_group]->local_ack_capable == FALSE) {
	printf("\nlocal acknowledgement not possible for stun \"%s\" routes",
	       stun_pinfo[idb->stun_group]->name);
	return(FALSE);
    } 

    if (!is_sdlc(idb)) {
	printf("\nlocal acknowledgement requires setting a sdlc-role to the interface");
	return(FALSE);
    } 

    if (!(idb->sdlc_data[addr])) {
	printf("\nAn sdlc address command must be given before local-ack can be used");
	return(FALSE);
    }
   
    if (idb->sdllc_active) {
	printf("\nSDLLC active on this interface. SDLLC and SDLC Local Ack are mutually exclusive");
	return(FALSE);
    }

    return(TRUE);

 } /* of function stun_check_localack */ 

/*
 * stun_parse_route
 *
 * [no] stun {route} [all | address <addr>]
 *                   [tcp <ipaddr] | interface <xfc>]
 * [no] stun {route} [all | address <addr>] interface <xfc> direct
 *
 * The "direct" keyword says that the outgoing interface is a direct
 * stun link rather than a connection to another peer.
 *
 */
static void stun_parse_route (parseinfo *csb, boolean gotfirst)
{
    leveltype status;
    int junk;
    stunpeertype *stp, *stpsub, *input_stp;
    stuntypetype type;
    hwidbtype *idb, *outidb;
    idbtype *outswidb = NULL;
    uchar addr[STUN_MAX_ADDR_LENGTH];
    uchar input_addr[STUN_MAX_ADDR_LENGTH];
    boolean direct;
    ipaddrtype ipaddr;
    boolean allchosen;
    boolean local_ack_active, priority, input_addr_inuse;
    boolean new_tcp_qlen;
    ulong user_def_tcp_qlen;
    int dlci = 0;
    byte lsap = 0 ;
    byte cls_priority = 0;

    stp = (stunpeertype *) NULL;
    outidb = NULL;
    ipaddr = 0;
    local_ack_active = priority = direct = input_addr_inuse = FALSE;
    idb = csb->interface->hwptr;
    user_def_tcp_qlen = STUN_MAX_QLEN; /* default if not user defined */
    new_tcp_qlen = FALSE;

    if (idb->stun_group == 0) {
	printf("\nA stun group must be defined for this interface before using this command.");
	return;
    }
    if (stun_pinfo[idb->stun_group] == NULL) {
	printf("\nNo group-protocol defined for group %d",
	       idb->stun_group);
	return;
    }
    if (GETOBJ(int,1) == STUN_RTALL) {
	allchosen = TRUE;
    } else {
	bcopy((char *)GETOBJ(paddr,1)->stun_addr, (char *)addr,
	      GETOBJ(paddr,1)->length);
	allchosen = FALSE;
    }

    /* make sure that "stun route XXX" is valid with stun protocol */
    if ((allchosen && stun_pinfo[idb->stun_group]->neverall) ||
        (!allchosen && stun_pinfo[idb->stun_group]->onlyall) )
      {
      printf("\nThis command not valid with stun protocol \"%s\".",
         stun_pinfo[idb->stun_group]->name);
      return;

      }

    junk = GETOBJ(int,2);
    if (junk == STUN_TCP) {
	type = STUN_PT_TCPD;
	ipaddr = GETOBJ(paddr,2)->ip_addr;
	if (GETOBJ(int,3)) {
	    if (!stun_check_localack(idb,addr[0]) )
                return;
	    local_ack_active = TRUE;
	}
	if (GETOBJ(int,4)) {
	    if (!local_ack_active) {
		printf("\n The \"local-ack\" option must be used with \"priority\"");
		return;
	    }
	    priority = TRUE;
	}
	if (GETOBJ(int,5)) {
	    bcopy((char *)GETOBJ(paddr,3)->stun_addr, (char *)input_addr,
		  GETOBJ(paddr,3)->length);
	    input_addr_inuse = TRUE;
	    for (input_stp = (stunpeertype *) idb->stun_Q.qhead; input_stp;
		input_stp = input_stp->next) {
		if (!bcmp(input_addr, input_stp->addr,
			  stun_pinfo[input_stp->group]->addrlength) ||
		    (stp->input_addr_inuse && 
		     !bcmp(input_addr, input_stp->input_addr,
			   stun_pinfo[input_stp->group]->addrlength)))
		    {
		        printf("\n stun route input-address already in use");
			return;
		    }
	    }
	}
        if (GETOBJ(int,6)) {
            user_def_tcp_qlen = GETOBJ(int,6);
            new_tcp_qlen = TRUE;
        }
	if ((stun_pinfo[idb->stun_group]->local_ack_required)  &&
	    (local_ack_active == FALSE)) {
	    printf ("\n\"local-ack\" required for this group.");
            return;
	} 
    }else{
	type = ((GETOBJ(int, 4)) ? STUN_PT_FR : STUN_PT_IF);
	if (type == STUN_PT_FR)
	   dlci = GETOBJ(int, 5);
        if (GETOBJ(int,7)) {
            if (!stun_check_localack(idb, addr[0]) )
                return;
            if (!IsClsdriverRunning) { 
               printf(" STUN: clsdriver not running, command not accepted \n");
               return;
	    }
            local_ack_active = TRUE;
            lsap = GETOBJ(int,6);
            type = STUN_PT_CLS; 
            if (GETOBJ(int,8))
                cls_priority = GETOBJ(int,9);
	}
	outswidb = GETOBJ(idb,1);
	outidb = hwidb_or_null(outswidb);
/*	csb->interface = idb->firstsw;*/
        if (idb->sdlc_flags & SDLC_FLAG_VIRTUAL_MULTIDROP) {
            printf("\nOnly TCP encapsulation is allowed for virtual multidrop.");
            return;
        }
	if (outidb == idb) {
	    printf("\nSpecified interface and current interface must be different");
	    return;
	}
	if ((outidb->status & IDB_SERIAL) == 0) {
	    printf("\nSpecified interface must be a serial interface");
	    return;
	}
	if (GETOBJ(int,3)) {
	    direct = TRUE;
	} else {
	    direct = FALSE;
	}
    }
    /*
     * see if the peer already exists.  If we are looking for the wild card
     * address then just check the last entry.
     */

    if (allchosen) {
	stp = (stunpeertype *) idb->stun_Q.qtail;
	if (stp && !stp->all_addr)
	    stp = NULL;
    } else {
	/*
	 * Sigh.  Not a wild card.  Must search.
	 */
	for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
	    if (stp->input_addr_inuse &&
		!bcmp(addr, stp->input_addr,
		      stun_pinfo[stp->group]->addrlength)) {
	        printf("\nstun route address already used as an input-address");
		return;
	    }
	}
	for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
	    if ((!bcmp(addr, stp->addr, stun_pinfo[stp->group]->addrlength)) &&
                (((stp->type == STUN_PT_IF) && (stp->outidb == outidb)) ||
		 ((stp->type == STUN_PT_FR) && (stp->outswidb == outswidb) && 
		  	(dlci == stp->dlci)) || 
                 ((stp->type == STUN_PT_CLS) && (stp->outswidb == outswidb) &&
                        (dlci == stp->dlci) && (stp->lsap == lsap)) ||
		 ((stp->type == STUN_PT_TCPD) && (stp->ipaddr == ipaddr))))

	      break;
	}
    }
    if (csb->sense) {
	/*
	 * creating or changing a stunnel peer.  If it already
	 * exists than check for identical (nothing to do then)
         * or something different (get rid of old first).
	 */
	if (stp) {
	    /* Same thing as before? If so, just return... */
	    if ((type == stp->type) && (stp->group == idb->stun_group) &&
		(stp->stun_idb == idb) &&
		(!bcmp(addr, stp->addr, stun_pinfo[stp->group]->addrlength)) &&
                (((stp->type == STUN_PT_IF) && (stp->outidb == outidb)) ||
                 ((stp->type == STUN_PT_FR) && (stp->outswidb == outswidb) &&
              		  	(dlci == stp->dlci)) || 
                 ((stp->type == STUN_PT_CLS) && (stp->outswidb == outswidb) &&
                                (dlci == stp->dlci) && (lsap == stp->lsap)) ||
		  ((stp->type == STUN_PT_TCPD) && (stp->ipaddr == ipaddr)))) {
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
			     stun_pinfo[stp->group]->addrlength)) {
		        printf("\nstun route input-address already set to a different value. Command ignored");
			return;
		    }
		}
		/*
		 *  If we are changing the SDLC Local Ack definition for this
		 *  stunpeertype, then make sure it won't disrupt things.
		 *    1. if SDLC Local Ack is active, make sure we are in
		 *       SLACK 'Disconnected' state and STUN 'closed' state,
		 *    2. if SDLC Local Ack is not active, make sure we are
		 *       in STUN 'closed' state.
		 */
		if (stp->local_ack_active != local_ack_active) {
		    if (stp->local_ack_active) {
			if ((stp->state       != STUN_PS_CLOSED)  ||
			    (stp->slack_state != SLACK_DISCONNECTED)) {
			    printf("\nBoth STUN and SLACK must be closed before removing local-ack definition.");
			    return;
			} else {
			    stp->local_ack_active = FALSE;
			    return;
			}
		    } else {
			if (stp->state != STUN_PS_CLOSED) {
			    printf("\nSTUN must be closed before adding local-ack definition.");
			    return;
			    
			} else {
			    stp->local_ack_active = TRUE;
			    return;
			}
		    }
		}

		/*
		 * For SDLC TGs, mark the stunpeertype active.
		 */
		if (stun_pinfo[stp->group]->uses_tg)
		  stp->tg_active = TRUE;

                (*stun_pinfo[idb->stun_group]->peer_change_hook)(idb, stp,
                                                                 TRUE);
		return;
	    }
	    /* nail the old peer */
	    status = raise_interrupt_level(NETS_DISABLE);
            sttrap_peers(idb);   
	    unqueue(&idb->stun_Q, stp); /* remove from queue */
	    stun_nail_priority_peers(stp);
	    reset_interrupt_level(status);
	}

	/* let's now create it. */
	stp = stun_create_peer(idb, allchosen, type, local_ack_active, priority,
			       direct, addr, STUN_P0_PORT, ipaddr, outswidb,
			       input_addr_inuse, input_addr, dlci,lsap,cls_priority);
	if (!stp)
	    return;
        stp->priority_top = NULL;
        stp->priority_peers[0] = stp;
        stp->user_def_max_tcp_qlen = user_def_tcp_qlen;
	if (priority) {
	    stpsub = stun_create_peer(idb, allchosen, type, local_ack_active,
				      FALSE, direct, addr, STUN_P1_PORT,
				      ipaddr, outswidb, input_addr_inuse,
				      input_addr, dlci,lsap,cls_priority);
	    if (!stpsub)
		return;
	    stp->priority_peers[1] = stpsub;
            stpsub->user_def_max_tcp_qlen = user_def_tcp_qlen;
	    stpsub->priority_top = stp;
	    stpsub = stun_create_peer(idb, allchosen, type, local_ack_active,
				      FALSE, direct, addr, STUN_P2_PORT,
				      ipaddr, outswidb, input_addr_inuse,
				      input_addr, dlci,lsap,cls_priority);
	    if (!stpsub)
		return;
	    stp->priority_peers[2] = stpsub;
            stpsub->user_def_max_tcp_qlen = user_def_tcp_qlen;
	    stpsub->priority_top = stp;
	    stpsub = stun_create_peer(idb, allchosen, type, local_ack_active,
				      FALSE, direct, addr, STUN_P3_PORT,
				      ipaddr, outswidb, input_addr_inuse,
				      input_addr, dlci,lsap,cls_priority);
	    if (!stpsub)
		return;
	    stp->priority_peers[3] = stpsub;
            stpsub->user_def_max_tcp_qlen = user_def_tcp_qlen;
	    stpsub->priority_top = stp;
	}
	if (!allchosen || idb->stun_Q.qhead == NULL) {
	    p_requeue(&idb->stun_Q, stp);
	} else {
	    /*
	     * addr is everything and there is already something on the
	     * queue.
	     */
	    ((stunpeertype *) (idb->stun_Q.qtail))->next = stp;
	    idb->stun_Q.qtail = stp;
	}
        if (stp->state == STUN_PS_DIRECT || stp->state == STUN_PS_CONNECTED)
           reg_invoke_stun_trap(stp);
	idb->nokeepalive = TRUE;
	if (!stun_pid) {
	    stun_pid = cfork(stun_background, 0L, 0, "STUN Background", 0);
	}

	if (stp->type == STUN_PT_TCPD) {
	    reg_invoke_ip_tcpdriver_start();
	}
    }
    else {
	/* delete entry if it exists */
	if (!stp)
	    return;				/* doesn't exist */
	status = raise_interrupt_level(NETS_DISABLE);
        (*stun_pinfo[idb->stun_group]->peer_change_hook)(idb, stp, FALSE);
        sttrap_peers(idb);   
	unqueue(&idb->stun_Q, stp); /* remove from queue */
	stun_nail_priority_peers(stp);
	reset_interrupt_level(status);
    }
    FOR_ALL_HWIDBS(idb) {
	if (idb->stun_Q.qhead) {
	    break;
	}
    }
    stun_first_idb = idb;
}


/*
 * stun_parse_group
 *
 * [no] stun group <n>
 *
 */
static void stun_parse_group (parseinfo *csb)
{

    hwidbtype *hwidb = hwidb_or_null(csb->interface);
    if (stun_pinfo[GETOBJ(int,1)] == NULL) {
	printf("\nNo group-protocol defined for group %d", GETOBJ(int,1));
	return;
    }
    if (!csb->sense) {
	stun_shutinterface(hwidb);
	hwidb->stun_group = 0;
	return;
    }

    if ((hwidb->stun_group) &&
	(GETOBJ(int,1) != hwidb->stun_group)) {
	printf("\nnew STUN group number for interface %s. Clearing STUN info.",
	       csb->interface->namestring);
	stun_shutinterface(hwidb);
    }
    hwidb->stun_group = GETOBJ(int,1);
    return;
}

/*
 * stunif_command
 * configure the serial tunnel.  Commands are as follows:
 *
 * Note that stun_command has already done the first call to get_word.
 */
void stunif_command (parseinfo *csb)
{
    hwidbtype *idb = hwidb_or_null(csb->interface);
    
    if (!stun_name) {
	if (!csb->nvgen) {
	    printf("\nSTUN not enabled -- give a stun peer-name first.");
	}
	return;
    }
    if ((idb->status & IDB_SERIAL) == 0) {
	if (!csb->nvgen) {
	    printf("\nstun only allowed on Serial interfaces");
	}
	return;
    }
    if (idb->enctype != ET_STUN && csb->sense) {
	if (!csb->nvgen) {
	    printf("\nstun requires STUN encapsulation on interface");
	}
	return;
    }
    if (!idb->nokeepalive) {
	if (!csb->nvgen) {
	    printf("\nstun requires keepalives to be disabled on interface");
	}
	return;
    }

    if (csb->nvgen) {
        stunpeertype *stp;
        char buff[MAX_PEER_NAME];

	if (idb->stun_group == 0) {
	    return;
	}

	switch (csb->which) {
	  case STUNIF_GROUP:
	    nv_write(TRUE, "%s %u", csb->nv_command, idb->stun_group);
	    break;
	  case STUNIF_PROXY:
	    break;
	  case STUNIF_SDLCROLE:
	    if (idb->stun_sdlc_encap == ET_SDLCP) {
		nv_write(TRUE, "%s primary", csb->nv_command);
	    } else {
		if (idb->stun_sdlc_encap == ET_SDLCS) {
		    nv_write(TRUE, "%s secondary", csb->nv_command);
		}
	    }
	    break;
	  case STUNIF_ROUTE:
	    for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
		nv_write(TRUE, "%s ", csb->nv_command);
		stun_sprintf_addr(stp->addr, stp->all_addr, stp->group, buff);
		if (stp->all_addr) {
		    nv_add(TRUE, "%s ", buff);
		} else {
		    nv_add(TRUE, "address %s ", buff);
		}
		switch (stp->type) {
		  case STUN_PT_TCPD:
		    nv_add(TRUE, "tcp %i", stp->ipaddr);
		    if (stp->local_ack_active) {
			nv_add(TRUE, " local-ack");
			if (stp->priority)
			    nv_add(TRUE, " priority");
			if (stp->input_addr_inuse) {
			    stun_sprintf_addr(stp->input_addr, FALSE, stp->group,
					      buff);
			    nv_add(TRUE, " input-address %s ", buff);
			}
		    }
                    if (stp->user_def_max_tcp_qlen != STUN_MAX_QLEN) {
                        nv_add(TRUE, " tcp-queue-max %d", stp->user_def_max_tcp_qlen);
                    }
		    break;
		  case STUN_PT_IF:
		    nv_add(TRUE, "interface %s", stp->outidb->hw_namestring);
		    nv_add(stp->state == STUN_PS_DIRECT, " direct");
		    break;
		  case STUN_PT_FR:
		    nv_add(TRUE, "interface %s", stp->outswidb->namestring);
		    nv_add(TRUE, " dlci %d", stp->dlci);
                    break; 
		  case STUN_PT_CLS:
		    nv_add(TRUE, "interface %s", stp->outswidb->namestring);
		    nv_add(TRUE, " dlci %d", stp->dlci);
                    nv_add(TRUE," %x local-ack", stp->lsap);
                    if (stp->cls_priority)
                        nv_add(TRUE, "  priority %d ",stp->cls_priority);
		    break;
		  default:
		    buginf("\nSTUN: illegal STUN peer");
		    break;
		}
	    }
	    break;
	  case NO_STUN:
	    /* No NV generation */
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case NO_STUN:
        stun_shutinterface(idb);
	idb->stun_group = 0;
	break;
      case STUNIF_GROUP:
	stun_parse_group(csb);
	break;
      case STUNIF_ROUTE:
	stun_parse_route(csb, FALSE);
	break;
      case STUNIF_PROXY:
	printf("\nProxy polling is obsolete, use STUN local-ack.");
	break;
      case STUNIF_SDLCROLE:
	/* Try commands specific for this stun protocol on this interface */
	if (stun_pinfo[idb->stun_group]) {
            if ((*stun_pinfo[idb->stun_group]->interface_parse_hook)(csb)) {
                return;
            }
        } else {
            printf("\nA stun group must be defined for this interface before using this command.");
            return;
        }
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * get_stun_name
 * return the peername used for stun to an external routine (sr_stunmib.c)
 *
 */
ipaddrtype get_stun_name(void)
{
  return (stun_name);
}
/*
 * get_stun_pname
 * return the protocol name used for stun to an external routine (sr_stunmib.c)
 *
 */
void get_stun_pname(uchar stungroup, char name[NAMEDTHINGNAMELEN+1])
{
   strcpy (name, stun_pinfo[stungroup]->name);
}


/*
 * STUN subsystem header
 */

#define STUN_MAJVERSION 1
#define STUN_MINVERSION 0
#define STUN_EDITVERSION  1

SUBSYS_HEADER(stun, STUN_MAJVERSION, STUN_MINVERSION, STUN_EDITVERSION,
	      stun_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: sdlc",
	      "req: sdlc, clsdriver");







