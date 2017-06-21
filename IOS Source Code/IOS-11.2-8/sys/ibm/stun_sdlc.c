/* $Id: stun_sdlc.c,v 3.5.18.4 1996/08/28 12:47:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/ibm/stun_sdlc.c,v $
 *------------------------------------------------------------------
 * stun_sdlc.c -- SDLC protocol support within serial tunnel
 *
 * March 8, 1991 - Joel P. Bion
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: stun_sdlc.c,v $
 * Revision 3.5.18.4  1996/08/28  12:47:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.18.3  1996/07/23  13:23:28  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.5.18.2  1996/05/17  11:22:00  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.4.2  1996/04/26  07:54:54  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.4.1  1996/04/03  14:35:11  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.18.1  1996/03/18  20:12:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  01:23:55  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  09:42:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  00:55:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/07  16:12:13  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:03:39  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/11/17  09:24:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:36:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:46:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/08  16:47:29  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.5  1995/07/27 03:11:11  vrawat
 * CSCdi33572:  AS/400 sdlc multidrop enviromnment
 *
 * Revision 2.4  1995/07/06  01:33:00  hampton
 * Eliminate magic timestamps from SDLC and STUN code by converting them
 * to use the passive timer macros.  [CSCdi36750]
 *
 * Revision 2.3  1995/06/18 23:36:59  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:05:14  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:49:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "../ibm/msg_stun.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "parser.h"
#include "parser_defs_stun.h"
#include "../ip/ip.h"
#include "ibm.h"
#include "stun_sdlc.h"
#include "stun_sdlc_public.h"
#include "sdlc_public.h"
#include "ibm_externs.h"
#include "slack.h"
#include "../if/tring.h"
#include "../ibm/sna_la_pri.h"
#include "../if/network.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"

int sdlc_proxy_pid;
int sdlc_passthrough_pid;
ulong sdlc_proxy_poll_interval;		/* Proxy Poll interval */
ulong sdlc_primary_passthrough;		/* Primary Passthrough interval */
stun_protocol_info sdlc_protocol_info;
stun_protocol_info sdlc_tg_protocol_info;

extern queuetype stun_tg_infoQ;
extern boolean quick_response;

static void stun_sdlc_init (void)
{
    sdlc_proxy_pid = 0;
    sdlc_passthrough_pid = 0;
    sdlc_proxy_poll_interval = SDLC_PROXY_POLL_DEFAULT;
    sdlc_primary_passthrough = SDLC_PRIMARY_PASSTHROUGH_DEFAULT;
}
 
/*
 * Print out a SDLC frame.
 */
static void sdlc_print_frame (
    paktype *pak,
    int group,
    char *prefix,
    hwidbtype *idb)
{
    sdlc_header *sdlc;
    int modulo = 0;
    sdlc_proxy_interface *spi;
    char *the_type;
    char *the_control;
    char the_nr[10];
    char the_ns[10];
    char the_time[40];

    sprint_dhms(the_time, stun_lastframe);
    GET_TIMESTAMP(stun_lastframe);

    sdlc = (sdlc_header *) pak->datagramstart;

    if (!stun_print_check(group, (uchar *) (&(sdlc->address))))
        return;
    if ((spi = idb->stun_protocol_specific) != NULL)
        modulo = spi->proxy_info[sdlc->address].modulo;
    if (modulo == 0)
        modulo = 8;

    if (SDLC_IS_S_FRAME(sdlc)) {
	the_type = "S:";
	switch (sdlc->control1 & SDLC_S_FRAME_MASK) {
	  case SDLC_S_FRAME_RR:    the_control = "RR     "; break;
	  case SDLC_S_FRAME_RNR:   the_control = "RNR    "; break;
	  case SDLC_S_FRAME_REJ:   the_control = "REJ    "; break;
	  default:     	           the_control = "unknown"; break;
	}
    } else if (SDLC_IS_U_FRAME(sdlc)) {
	the_type = "U:";
	switch (sdlc->control1 & SDLC_U_FRAME_MASK) {
	  case SDLC_U_FRAME_UP:    the_control = "UP     "; break;
	  case SDLC_U_FRAME_UI:    the_control = "UI     "; break;
	  case SDLC_U_FRAME_SABM:  the_control = "SABM   "; break;
	  case SDLC_U_FRAME_SNRM:  the_control = "SNRM   "; break;
    	  case SDLC_U_FRAME_SNRME: the_control = "SNRME  "; break;
  	  case SDLC_U_FRAME_DISC:  the_control = "DISC/RD"; break;
  	  case SDLC_U_FRAME_SIM:   the_control = "SIM/RIM"; break;
	  case SDLC_U_FRAME_XID:   the_control = "XID    "; break;
	  case SDLC_U_FRAME_TEST:  the_control = "TEST   "; break;
	  case SDLC_U_FRAME_CFGR:  the_control = "CFGR   "; break;
	  case SDLC_U_FRAME_UA:    the_control = "UA     "; break;
	  case SDLC_U_FRAME_DM:	   the_control = "DM     "; break;
	  case SDLC_U_FRAME_FRMR:  the_control = "FRMR   "; break;
	  case SDLC_U_FRAME_BCN:   the_control = "BCN    "; break;
	  default:   	           the_control = "unknown"; break;
	}
    } else {			/* I Frame */
	the_type = "I:";
	the_control = "       ";
    }
    if (!SDLC_IS_U_FRAME(sdlc)) {
	sprintf(the_nr, " NR:%03d ", SDLC_NR(sdlc, modulo));
	if (SDLC_IS_I_FRAME(sdlc))
	    sprintf(the_ns, "NS:%03d", SDLC_NS(sdlc, modulo));
	else
	    sprintf(the_ns, "");
    } else {
	sprintf(the_nr, "");
    }

    buginf("\nSTUN sdlc: %s %15s %s (%03x/%03d) %s %s PF:%1d %s %s",
	   the_time, idb->hw_namestring, prefix, sdlc->address,  modulo,
	   the_type, the_control, (SDLC_PF_SET(sdlc, modulo) ? (1) : (0)),
	   the_nr, the_ns);

}


/*
 * Enable/disable proxy generation
 */
static void sdlc_enable_proxy_gen (
    sdlc_proxy_interface *spi,
    sdlc_proxy_address *spa,
    uchar *addr)
{
    spa->prev_in_proxy =
	spi->proxy_info[SDLC_PROXY_HEAD].prev_in_proxy;
    spi->proxy_info[spa->prev_in_proxy].next_in_proxy =
	spi->proxy_info[SDLC_PROXY_HEAD].prev_in_proxy =
	    (ulong) addr[0];
    spa->next_in_proxy = SDLC_PROXY_HEAD;
}

static void sdlc_disable_proxy_gen (
    sdlc_proxy_interface *spi,
    sdlc_proxy_address *spa)
{
    spi->proxy_info[spa->prev_in_proxy].next_in_proxy =
	spa->next_in_proxy;
    spi->proxy_info[spa->next_in_proxy].prev_in_proxy =
	spa->prev_in_proxy;
}


static void sdlc_proxy_clear_queue (
    uchar group,
    uchar *addr,
    sdlc_proxy_address *sta,
    boolean forward,
    boolean disable)
{
    paktype *pak;
    leveltype status = (leveltype) 0;

    /* Handle this very quick case... */
    if (stun_print_check(group, addr))
	buginf("\nSDLC: Dequeuing from Network for %s",
	       forward ? "Serial transmission" : "tossing");
    if (!forward) {
	if (disable)
	    status = raise_interrupt_level(NETS_DISABLE);
	while (dequeue(&sta->s_to_p_queue))
	    ;
	if (disable)
	    reset_interrupt_level(status);
	return;
    }
    while (TRUE) {
	if (disable)
	    status = raise_interrupt_level(NETS_DISABLE);
	if (!(pak = dequeue(&sta->s_to_p_queue))) {
	    if (disable)
		reset_interrupt_level(status);
	    return;
	}
	datagram_out(pak);
	if (disable)
	    reset_interrupt_level(status);
    }
}


static void sdlc_proxy_enqueue (
    stunpeertype *stp,
    sdlc_proxy_address *sta,
    paktype *pak)
{
    if (QUEUESIZE(&(sta->s_to_p_queue)) > STUN_MAX_QLEN) {
        if (stun_print_check(stp->group, stp->addr))
            buginf("\nSDLC: STUN max q exceeded, dropping packet");
	retbuffer(pak);
	stp->drops++;
    } else {
	if (stun_print_check(stp->group, stp->addr))
	    buginf("\nSDLC: Enquing packet from Network for Serial Transmission");
	p_enqueue(&(sta->s_to_p_queue), pak);
    }
}

static void sdlc_newstate (
    uchar group,
    uchar *addr,
    sdlc_proxy_address *sta,
    int news)
{
    sta->state = news;
    if (stun_print_check(group, addr)) {
	buginf("\n  New state = %c%1u",
	       ((sta->modulo) ? (sta->primary ? ('p') : ('s')) : 'd'),
	       news);
    }
}


/*
 * sdlc_passthrough_bground
 *
 * Periodically reset the proxy state
 */
static forktype sdlc_passthrough_bground (void)
{
    sdlc_proxy_interface *sti;
    ulong i;
    hwidbtype *idb;
    
    while (TRUE) {
	for (idb = stun_first_idb; idb; idb = IDB_NEXT(idb)) {
	    if (!idb->stun_group)
	      continue;
	    if ((sti = idb->stun_protocol_specific) != NULL) {
		for (i = 0; i <= SDLC_MAX_ADDRESS; i++) {
		    if ((sti->proxy_info[i].state == SDLC_PROXY_STATE_4) &&
			(sti->proxy_info[i].primary)) {
			sdlc_newstate(idb->stun_group, (uchar *) &i,
				      (&(sti->proxy_info[i])),
				      SDLC_PROXY_STATE_0);
			if (stun_print_check(idb->stun_group, (uchar *) &i))
			  buginf("\nSTUN: Passthr move of %s, adr %x p4->p0",
				 idb->hw_namestring, i);
		    }
		}
	    }
	}
	process_sleep_for(ONESEC * sdlc_primary_passthrough);
    }
}
 

/*
 * proxy_background
 * send out SDLC "RR" proxies to various links that need them
 */
static forktype sdlc_proxy_bground (void)
{
    hwidbtype *idb;
    sdlc_proxy_address *spa;
    sdlc_proxy_interface *spi;
    int next;
    paktype *mod8_pak, *mod128_pak;
    sdlc_proxy_hdr *mod8_sdlc, *mod128_sdlc;

    mod8_pak = getbuffer(sizeof(sdlc_proxy_hdr));
    mod128_pak = getbuffer(sizeof(sdlc_proxy_hdr));

    if (!mod8_pak || !mod128_pak) {
	process_kill(THIS_PROCESS);
    }

    mod8_sdlc = (sdlc_proxy_hdr *)sdlc_proxy_headstart(mod8_pak);
    mod128_sdlc = (sdlc_proxy_hdr *)sdlc_proxy_headstart(mod128_pak);

    mod8_pak->datagramsize = SDLC_RR_8_LEN;
    mod8_pak->datagramstart = (uchar *)mod8_sdlc;
    mod8_pak->enctype = ET_HDLC;
    mod8_pak->linktype = LINK_STUN;

    mod128_pak->datagramsize = SDLC_RR_128_LEN;
    mod128_pak->datagramstart = (uchar *)mod128_pak->network_start;
    mod128_pak->enctype = ET_HDLC;
    mod128_pak->linktype = LINK_STUN;
    mod128_sdlc->sdlc_control1 = SDLC_IS_RR;

    while (TRUE) {
	for (idb = stun_first_idb; idb; idb = IDB_NEXT(idb)) {
	    if (idb->stun_protocol_specific) {
		spi = idb->stun_protocol_specific;

		if (spi->lock)
		  continue;
		
		if ((next = spi->proxy_info[SDLC_PROXY_HEAD].next_in_proxy) !=
		    SDLC_PROXY_HEAD) {
		    spa = &(spi->proxy_info[next]);
		    mod128_pak->if_output = idb->firstsw;
		    mod8_pak->if_output = idb->firstsw;
		} else
		    continue;

		for (; (next != SDLC_PROXY_HEAD);
		     next = spa->next_in_proxy,
		     spa = &(spi->proxy_info[next])) {
		    if (idb->state != IDBS_UP) {
			sdlc_disable_proxy_gen(spi, spa);
			sdlc_newstate(idb->stun_group, (uchar *)&next, spa,
				      SDLC_PROXY_STATE_0);
			continue;
		    }
		    spa->poll_gen_rep++;
		    if (spa->modulo == 128) {
			mod128_sdlc->sdlc_address = next;
			mod128_sdlc->sdlc_control2 =
			    (SDLC_128_POLL | (spa->pnr << SDLC_128_NR_SHIFT));
			stun_print_frame(mod128_pak,
					 idb->stun_group, "PRQ:", idb,
					 sdlc_print_frame);
			pak_lock(mod128_pak);
			datagram_out(mod128_pak);
		    } else if (spa->modulo == 8) {
			mod8_sdlc->sdlc_address = next;
			mod8_sdlc->sdlc_control1 = 
			    (SDLC_IS_RR | SDLC_8_POLL |
			     (spa->pnr << SDLC_8_NR_SHIFT));
			stun_print_frame(mod8_pak,
					 idb->stun_group, "PRQ:", idb,
					 sdlc_print_frame);
			pak_lock(mod8_pak);
			datagram_out(mod8_pak);
		    }
		}
	    }
	}
	process_sleep_for((ONESEC/1000) * sdlc_proxy_poll_interval);
    }
    datagram_done(mod128_pak);
    datagram_done(mod8_pak);
}

static void sdlc_proxy_long_periodic (
    hwidbtype *idb)
{
    return;
}


static void show_sdlc_proxy (void)
{
    hwidbtype *idb;
    ulong i;
    sdlc_proxy_interface *spi;
    int cnt = 0;
    boolean first;
    
    for (idb = stun_first_idb; idb; idb = IDB_NEXT(idb)) {
	if (idb->enctype != ET_STUN)
	    continue;
	if (stun_pinfo[idb->stun_group] != &sdlc_protocol_info)
  	    continue;
	if ((spi = idb->stun_protocol_specific) == NULL)
	    continue;
	first = TRUE;
	for (i = 0; i <= SDLC_MAX_ADDRESS; i++) {
	    if ((spi->proxy_info[i].modulo) ||
		(spi->proxy_info[i].discovery)) {
		if (first) {
		    printf("\n%s %d %s%s", idb->hw_namestring, idb->unit,
			   (idb->firstsw->description ? "-- " : ""),
			   (idb->firstsw->description ? idb->firstsw->description : ""));
		    first = FALSE;
		}
		if (!(cnt % 8))
		    printf("\n");
		printf("  %-3x: %c%1u", i, (spi->proxy_info[i].modulo ?
					    ((spi->proxy_info[i].primary) ?
					    'p' : 's') : 'd'),
		       spi->proxy_info[i].state);
	    }
	}
	if (cnt % 8)
	    printf("\n");
    }
}

static void stun_sdlc_nv_globals (stun_protocol_info *spi, parseinfo *csb)
{
    switch (csb->which) {
      case STUN_PRIMARY:
	nv_write(sdlc_primary_passthrough != SDLC_PRIMARY_PASSTHROUGH_DEFAULT,
		 "%s %u", csb->nv_command, sdlc_primary_passthrough);
	break;
      case STUN_POLL:
	nv_write(sdlc_proxy_poll_interval != SDLC_PROXY_POLL_DEFAULT,
		 "%s %u", csb->nv_command, sdlc_proxy_poll_interval);
	break;
      case STUN_SCHEMA:
	/* No NV generation when doing sdlc */
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

static void stun_sdlc_nv_1_interface (
    stun_protocol_info *spi,
    hwidbtype *idb)
{
}

static void stun_sdlc_nv_2_interface (
    stun_protocol_info *spi,
    hwidbtype *idb)
{
}

#ifdef NOBODY_CALLS_ME
void nv_stun_int_proxy (
    hwidbtype *idb,
    char *nv_command)
{
    sdlc_proxy_interface *sti;
    ulong i;

    if ((sti = idb->stun_protocol_specific) != NULL) {
	for (i = 0; i <= SDLC_MAX_ADDRESS; i++) {
	    if (sti->proxy_info[i].discovery) {
		nv_write(TRUE, "%s address %x discovery",
			 nv_command, i);
	    } else {
		nv_write((sti->proxy_info[i].permanent),
			 "%s address %x modulus %d %s", nv_command,
			 i, sti->proxy_info[i].modulo,
			 ((sti->proxy_info[i].primary) ?
			  "primary" : "secondary"));
	    }
	}
    }
}    
#endif /* NOBODY_CALLS_ME */

static void stun_sdlc_shutint (
    hwidbtype *idb)
{
    sdlc_proxy_interface *spi;
    ulong i;

    spi = idb->stun_protocol_specific;
    if (spi) {
	idb->stun_protocol_specific = NULL;
	for (i = 0; i <= SDLC_MAX_ADDRESS; i++) {
	    sdlc_proxy_clear_queue(idb->stun_group ,(uchar *) &i,
				   &(spi->proxy_info[i]), FALSE, FALSE);
	}
	free(spi);
    }
}



/*
 * sdlc_parse_proxy
 *
 * Parse the "proxy" interface subcommands for stun-tunnel.
 * 
 * The forms are:
 *    [no] stun proxy-poll [all | address <addr>]
 *         modulus [8 | 128] [primary | secondary]
 */   
static void sdlc_parse_proxy (parseinfo *csb)
{
    uchar addr[STUN_MAX_ADDR_LENGTH];
    hwidbtype *idb = csb->interface->hwptr;
    uchar modulo = 0;
    ulong i;
    boolean primary = FALSE;
    sdlc_proxy_interface *spi;
    boolean discovery = FALSE;
    stunpeertype *stp;
    
    if (idb->stun_group == 0) {
	printf("\nstun group for interface must be defined before proxies");
	return;
    }
    bcopy((char *)GETOBJ(paddr,1)->stun_addr, (char *)addr, ADDRLEN_STUN);
    if (GETOBJ(int,1) == STUNIF_PROXY_DISCOVERY) {
	discovery = TRUE;
	primary = FALSE;
	modulo = 0;
    } else {
	modulo = (uchar) GETOBJ(int,2);
	if (GETOBJ(int,1) == STUNIF_PROXY_PRIMARY) {
	    primary = TRUE;
	} else {
	    primary = FALSE;
	}
    }

    /* Everything parsed okay... create structure if none exists */
    for (stp = (stunpeertype *) idb->stun_Q.qhead; stp; stp = stp->next) {
	if (!bcmp(addr, stp->addr, stun_pinfo[idb->stun_group]->addrlength)) {
	    break;
	}
    }
    if (stp == NULL) {
	printf("\nA specific route to address %02x must be given to use proxy polling",
	       addr[0]);
	return;
    }
    if (stp->local_ack_active) {
	printf("\nBoth local acknowledgement and proxy polling cannot be used for the same");
	printf("\naddress (%02x)", addr[0]);
	return;
    }
    spi = idb->stun_protocol_specific;
    if (!spi) {
	if ((spi = malloc(sizeof(sdlc_proxy_interface))) == NULL) {
	    printf(nomemory);
	    return;
	}
	spi->lock = TRUE;

	/* Initialize the structure */
	for (i = 0; i <= SDLC_MAX_ADDRESS; i++) {
	    spi->proxy_info[i].pnr = spi->proxy_info[i].snr =
		SDLC_UNKNOWN_WINDOW;
	    spi->proxy_info[i].permanent = spi->proxy_info[i].primary =
		FALSE;
	    spi->proxy_info[i].modulo =
		spi->proxy_info[i].next_in_proxy =
		    spi->proxy_info[i].prev_in_proxy =
			spi->proxy_info[i].poll_gen_rep = 0;
	    sdlc_newstate(idb->stun_group, (uchar *) &i,
			  (&(spi->proxy_info[i])), SDLC_PROXY_STATE_0);
	    queue_init(&(spi->proxy_info[i].s_to_p_queue), 0);
	}
	spi->proxy_info[SDLC_PROXY_HEAD].next_in_proxy =
	    spi->proxy_info[SDLC_PROXY_HEAD].prev_in_proxy = SDLC_PROXY_HEAD;

	if (!sdlc_proxy_pid)
	    sdlc_proxy_pid = cfork(sdlc_proxy_bground, 0L, 0,
				   "SDLC Proxy Bkgd", 0);
 	if (!sdlc_passthrough_pid)
  	    sdlc_passthrough_pid = cfork(sdlc_passthrough_bground, 0L, 0,
 					 "SDLC Pass Bkgd", 0);
	idb->stun_protocol_specific = spi;
    }

    /*
     *  Issue a warning to all users of STUN proxy polling that this
     *  feature will disappear after this release (i.e. proxy poll
     *  will remain in effect through Release 9.1 but will be
     *  removed in Release 9.2).
     *
     *  Proxy polling is superceded by SDLC Local Termination.
     */
    if (csb->sense) {
	printf("\n*** WARNING ***");
	printf("\n  STUN Proxy Polling has been superceded by SDLC Local Ack.");
	printf("\n  STUN Proxy Polling will be removed in the next release of cisco router software");
    }

    /*
     * Special check the "config memory" people -- if all attributes identical
     * to chose already in place leave quietly and change nothing.
     */
    if (csb->sense) {
	if ((spi->proxy_info[addr[0]].discovery == discovery) &&
	    (discovery ||
	     ((spi->proxy_info[addr[0]].modulo == modulo) &&
	      (spi->proxy_info[addr[0]].primary == primary)))) {
	    spi->lock = FALSE;
	    return;
	}
    }

    /*
     * Turn on the lock. Polls are sent through by sdlc code if lock is true 
     * NONE OF THE CODE BELOW SHOULD EXECUTE A "RETURN"!!! IF THIS IS DONE, THE
     * LOCK WILL BE LEFT ON!!
     */
    spi->lock = TRUE;
    
    spi->proxy_info[addr[0]].pnr = spi->proxy_info[addr[0]].snr =
      SDLC_UNKNOWN_WINDOW;

    /* If was there, reset the state... */
    if (spi->proxy_info[addr[0]].permanent) {
	if ((spi->proxy_info[addr[0]].state == SDLC_PROXY_STATE_4) &&
	    (!(spi->proxy_info[addr[0]].primary)))
	  sdlc_disable_proxy_gen(spi, &(spi->proxy_info[addr[0]]));
    }
    sdlc_newstate(idb->stun_group, addr,
		  (&(spi->proxy_info[addr[0]])), (((csb->sense) && discovery) ?
						  SDLC_PROXY_STATE_6 :
						  SDLC_PROXY_STATE_0));
    spi->proxy_info[addr[0]].permanent = csb->sense;
    spi->proxy_info[addr[0]].modulo = ((csb->sense) ? (modulo) : (0));
    spi->proxy_info[addr[0]].discovery = ((csb->sense) ? (discovery) : (FALSE));
    spi->proxy_info[addr[0]].primary = primary;
    spi->proxy_info[addr[0]].pending_modulo = 0;
    
    /* Turn off lock */
    spi->lock = FALSE;
}

static void stun_sdlc_parse_pollint (parseinfo *csb)
{
    if (csb->sense) {
	sdlc_proxy_poll_interval = GETOBJ(int,1);
    } else {
	sdlc_proxy_poll_interval = SDLC_PROXY_POLL_DEFAULT;
    }
}

static void stun_sdlc_parse_primary_passthrough (parseinfo *csb)
{
    if (csb->sense) {
	sdlc_primary_passthrough = GETOBJ(int,1);
    } else {
	sdlc_primary_passthrough = SDLC_PRIMARY_PASSTHROUGH_DEFAULT;
    }
}

/*
 *
 * Incorporated herein is the "SDLC Proxy Polling" State Machine. This
 * is used to help reduce the number of proxy polls travelling through
 * the Internet during idle periods in SDLC communication. In its first
 * implementation, it attempts to be "safe"
 * on the error of sending a few more polls out, but allowing the data
 * through with minimal coding complexity. The objective is to detect the
 * idle state, defined as a period of identical RR transferals between 
 * stations, and simulate that idle condition. On the receipt of any traffic
 * from either end indicating the idle state is broken, full communication
 * is restored.
 * 
 * The idea is that the primary end of a link, when idle, has its polls
 * responded by the router and the secondary end of a link, when idle, has
 * polls periodically generated by the cisco router.
 * 
 * The "proxying" only takes effect after a few packets of idle time on
 * the connection, and will immediately shut-down with any activity.
 * 
 * It is defined in the following two state machines:
 *
 * *IN ALL STATES*:
 *                   IF receive SNRM/SNRME and discovery, set modulo to 0,
 *                   State to 6, role = Negotiable. 
 *
 * THEN, depending on role:
 *
 * Negotiable:
 *         State 6:  Data from serial
 *                    SNRM or    
 *                    SNRME    : Set pending_modulo = 8 if SNRM or 128 if SNRME
 *                               Set primary to TRUE. Goto 7.
 *                    Others   : Stay 6.
 *                   Data from network
 *                    SNRM or 
 *                    SNRME    : As above, but set primary to FALSE. Goto 7.
 *                    Others   : Stay 6.
 *         State 7:  Data from serial
 *                    pri=F, UA: Set modulo = pending_modulo. Goto 0.
 *                    Others   : Goto 6.
 *                   Data from network
 *                    pri=T, UA: Set modulo = pending_modulo. Goto 0.
 *                    Others   : Goto 6.
 *                    
 * Primary:
 *
 *         State 0:
 *                   Data from primary
 *                    RR, P/F=1: Save N(R) as "pnr". Forward RR to Sec. Goto 1.
 *                    Others   : Forward frame to Sec. Stay 0. 
 *                   Data from secondary
 *                    All      : Forward frame to Pri. Stay 0.
 *         State 1:
 *                   Data from primary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=pnr : Forward frame to Sec. Stay 1.
 *                    Others   : Forward frame to Sec. Goto 0.
 *                   Data from secondary
 *                    RR, P/F=1: Save N(R) as "snr". Forward RR to Pri. Goto 2.
 *                    Others   : Forward frame to Pri. Goto 0.
 *         State 2:
 *                   Data from primary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=pnr : Forward frame to Sec. Goto 3.
 *                    Others   : Forward frame to Sec. Goto 0.
 *                   Data from secondary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=snr : Forward frame to Pri. Stay 2.
 *                    Others   : Forward frame to Pri. Goto 0.
 *         State 3:
 *                   Data from primary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=pnr : Forward frame to Sec. Stay 3.
 *                    Others   : Forward frame to Sec. Goto 0.
 *                   Data from secondary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=snr : Forward frame to Pri. Stay 4.
 *                    Others   : Forward frame to Pri. Goto 0.
 *         State 4:
 *                   Periodically
 *                             : Goto 0.
 *                   Data from primary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=pnr : Reply to Pri. w/ RR, P/F=1, N(R)=snr. Stay 4.
 *                    Others   : Forward frame to Sec. Goto 0.
 *                   Data from secondary
 *                    S-Frame,
 *                    P/F=1,
 *                    N(R)=snr : Forward frame to Pri. Goto 2.
 *                    Other 
 *                    S-Frame,
 *                    or U-fr. : Forward frame to Pri. Goto 0.
 *                    Others   : Enqueue frame on s-to-p-queue. Goto 5.
 *
 *         State 5:
 *                   Data from primary
 *                    RR or
 *                    I-Frame,
 *                    P/F=1,
 *                    N(R)=pnr : Forward frame to Sec. Forward s-to-p-queue
 *                               to Pri. Goto 0.
 *                    Others   : Forward frame to Sec. Dump s-to-p-queue.
 *                               Goto 0.
 *                   Data from secondary
 *                    All      : Enqueue frame on s-to-p-queue. Stay 5.
 *
 *     Secondary Side:
 *         State 0:
 *            Event: Data from primary
 *                    RR, P/F=1: Save N(R) as "pnr". Forward RR to Sec. Goto 1.
 *                   Data from secondary
 *                    All      : Forward frame to Pri. Stay 0.
 *         State 1:
 *                   Data from primary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=pnr : Forward frame to Sec. Stay 1.
 *                    Others   : Goto 0.
 *                   Data from secondary
 *                    RR, P/F=1: Forward RR to Pri. Save N(R) as "snr". Goto 2.
 *                    Others   : Forward frame to Pri. Goto 0.
 *         State 2:
 *                   Data from primary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=pnr : Forward frame to Sec. Goto 3.
 *                    Others   : Goto 0.
 *                   Data from secondary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=snr : Forward frame to Pri. Stay 2. 
 *                    Others   : Forward frame to Pri. Goto 0. 
 *         State 3: 
 *                   Data from primary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=pnr : Forward frame to Sec. Stay 3.
 *                    Others   : Forward frame to Sec. Goto 0. 
 *                   Data from secondary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=snr : Forward frame to Pri. Goto 4. 
 *         State 4: 
 *                   Periodically
 *                             : Generate RR, P/F=1, N(R)=pnr to Sec. Stay 4.
 *                   Data from primary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=pnr : Forward frame to Sec. Goto 3.
 *                    Others   : Forward frame to Sec. Goto 0.
 *                   Data from secondary
 *                    RR,
 *                    P/F=1,
 *                    N(R)=snr : Stay 4.
 *                    Others   : Forward frame to Pri. Goto 0.
 *
 */

static ushort stun_sdlc_serial_input_proxy (
    hwidbtype *idb,
    paktype *pak,
    stunpeertype *stp)
{
    sdlc_proxy_interface *sti;
    sdlc_proxy_address *sta;
    sdlc_header *sdlc;
    boolean opcode_out = STUN_OP_PASS;
    uchar modulo;
    uchar addr;

    /*
     * Proxy polling work
     */
    sdlc = (sdlc_header *) pak->datagramstart;
    addr = (uchar) sdlc->address;
    sti = idb->stun_protocol_specific;
    modulo = sti->proxy_info[addr].modulo;
    if ((sti != NULL) &&
	((modulo) ||
	 (sti->proxy_info[addr].discovery))) {
	sta = &(sti->proxy_info[addr]);
	
	if (sta->discovery && SDLC_SNRM_SNRME(sdlc)) {
	    sti->lock = TRUE;
	    if ((sta->state == SDLC_PROXY_STATE_4) &&
		(!sta->primary)) {
		sdlc_disable_proxy_gen(sti, sta);
	    }
	    sta->modulo = 0;
	    sdlc_newstate(idb->stun_group, &addr,
			  sta, SDLC_PROXY_STATE_6);
	    sti->lock = FALSE;
	    modulo = 0;
	}
	if (modulo == 0) {
	    switch (sta->state) {
	    case SDLC_PROXY_STATE_6:
		if (SDLC_SNRM(sdlc)) {
		    sta->pending_modulo = 8;
		    sta->primary = TRUE;
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_7);
		} else if (SDLC_SNRME(sdlc)) {
		    sta->pending_modulo = 128;
		    sta->primary = TRUE;
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_7);
		}
		break;
	    case SDLC_PROXY_STATE_7:
		if ((sta->primary == FALSE) && SDLC_UA(sdlc)) {
		    sti->lock = TRUE;
		    sta->modulo = sta->pending_modulo;
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		    sti->lock = FALSE;
		} else {
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_6);
		}
		break;
	    }
	} else if (sta->primary) {
	    /* Data is from Primary */
	    switch (sta->state) {
		    
	    case SDLC_PROXY_STATE_0: /* Look for RR, P from Pri */
		if (SDLC_RR_AND_PF(sdlc, modulo)) {
		    sta->pnr = SDLC_NR(sdlc, modulo);
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_1);
		}
		break;

	    case SDLC_PROXY_STATE_1: /* Look for RR, F from Sec */
	    case SDLC_PROXY_STATE_3:
		if (!SDLC_RR_PF_AND_NR(sdlc, modulo, sta->pnr))
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		break;

	    case SDLC_PROXY_STATE_2: /* Look for RR, P from Pri */
		if (!SDLC_RR_PF_AND_NR(sdlc, modulo, sta->pnr))
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		else
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_3);
		break;

	    case SDLC_PROXY_STATE_4: /* Generate proxy replies */
		if (!SDLC_RR_PF_AND_NR(sdlc, modulo, sta->pnr))
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		else {
                    opcode_out = STUN_OP_IGNORE;
		    if (modulo == 128)
			sdlc->control2 = ((sdlc->control2 &
					   ~(SDLC_128_NR_MASK)) |
					  (sta->snr << SDLC_128_NR_SHIFT));
		    else
			sdlc->control1 = ((sdlc->control1 &
					   ~(SDLC_8_NR_MASK)) |
					  (sta->snr << SDLC_8_NR_SHIFT));
		    stun_print_frame(pak, stp->group, "PRP:", idb,
				     sdlc_print_frame);
		    sta->poll_gen_rep++;
		    stif_direct(stp, pak, idb, FALSE, NULL, FALSE, FALSE);
		}
		break;

	    case SDLC_PROXY_STATE_5:
		if ((SDLC_RR(sdlc) || SDLC_IS_I_FRAME(sdlc)) &&
		    SDLC_PF_SET(sdlc, modulo) &&
		    SDLC_NR(sdlc, modulo) == sta->pnr) {
		    sdlc_proxy_clear_queue(stp->group, stp->addr,
					   sta, TRUE, TRUE);
                    opcode_out = STUN_OP_IGNORE;
		    stif_nukepak(pak);
		} else
		    sdlc_proxy_clear_queue(stp->group, stp->addr,
					   sta, FALSE, TRUE);
		sdlc_newstate(idb->stun_group, &addr,
			      sta, SDLC_PROXY_STATE_0);
		break;

	    }			/* Switch */

	} else {		/* Secondary */

	    switch (sta->state) {

	    case SDLC_PROXY_STATE_0:
		break;

	    case SDLC_PROXY_STATE_1:
		if (SDLC_RR_AND_PF(sdlc, modulo)) {
		    sta->snr = SDLC_NR(sdlc, modulo);
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_2);
		} else
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		break;

	    case SDLC_PROXY_STATE_2:
		if (!SDLC_RR_PF_AND_NR(sdlc, modulo, sta->snr))
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		break;

	    case SDLC_PROXY_STATE_3:
		if (SDLC_RR_PF_AND_NR(sdlc, modulo, sta->snr)) {
		    sti->lock = TRUE; /* Set up proxy generator timer */
		    /* Insert item into double */
		    sdlc_enable_proxy_gen(sti, sta, &addr);
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_4);
		    sti->lock = FALSE;
		} else
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		break;

	    case SDLC_PROXY_STATE_4:
		if (!SDLC_RR(sdlc) || !SDLC_PF_SET(sdlc, modulo) ||
		    (sta->snr != SDLC_NR(sdlc, modulo))) {
		    /* Remove item from double */
		    sti->lock = TRUE;
		    sdlc_disable_proxy_gen(sti, sta);
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		    sti->lock = FALSE;
		} else {
                    opcode_out = STUN_OP_IGNORE;
		    stif_nukepak(pak);
		}
		break;
	    }
	}
    }
    return(opcode_out);
}


static boolean stun_sdlc_network_input_proxy (
    hwidbtype *idb,
    paktype *pak,
    stunpeertype *stp,
    ushort opcode)
{
    sdlc_proxy_interface *sti;
    sdlc_proxy_address *sta;
    sdlc_header *sdlc;
    uchar addr;
    boolean eat = FALSE;
    uchar modulo;

    sdlc = (sdlc_header *) pak->datagramstart;
    addr = (uchar) sdlc->address;
    sti = idb->stun_protocol_specific;
    if ((sti != NULL) &&
	((modulo = sti->proxy_info[addr].modulo) ||
	 (sti->proxy_info[addr].discovery))) {
	sta = &(sti->proxy_info[addr]);

	if (sta->discovery && SDLC_SNRM_SNRME(sdlc)) {
	    sti->lock = TRUE;
	    if ((sta->state == SDLC_PROXY_STATE_4) &&
		(!sta->primary)) {
		sdlc_disable_proxy_gen(sti, sta);
	    }
	    sta->modulo = 0;
	    sdlc_newstate(idb->stun_group, &addr,
			  sta, SDLC_PROXY_STATE_6);
	    sti->lock = FALSE;
	    modulo = 0;
	}
	if (modulo == 0) {
	    switch (sta->state) {
	    case SDLC_PROXY_STATE_6:
		if (SDLC_SNRM(sdlc)) {
		    sta->pending_modulo = 8;
		    sta->primary = FALSE;
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_7);
		} else if (SDLC_SNRME(sdlc)) {
		    sta->pending_modulo = 128;
		    sta->primary = FALSE;
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_7);
		}
		break;
	    case SDLC_PROXY_STATE_7:
		if ((sta->primary == TRUE) && SDLC_UA(sdlc)) {
		    sti->lock = TRUE;
		    sta->modulo = sta->pending_modulo;
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		    sti->lock = FALSE;
		} else {
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_6);
		}
		break;
	    }
	} else if (sta->primary) {
	    switch (sta->state) {

	    case SDLC_PROXY_STATE_0:
		break;

	    case SDLC_PROXY_STATE_1:
		if (SDLC_RR_AND_PF(sdlc, modulo)) {
		    sta->snr = SDLC_NR(sdlc, modulo);
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_2);
		} else
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		break;

	    case SDLC_PROXY_STATE_2:
		if (!SDLC_RR_PF_AND_NR(sdlc, modulo, sta->snr))
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		break;

	    case SDLC_PROXY_STATE_3:
		if (SDLC_RR_PF_AND_NR(sdlc, modulo, sta->snr))
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_4);
		else
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		break;

	    case SDLC_PROXY_STATE_4:
		if (SDLC_IS_S_FRAME(sdlc) || SDLC_IS_U_FRAME(sdlc)) {
		    if (!SDLC_RR_PF_AND_NR(sdlc, modulo, sta->snr))
			sdlc_newstate(idb->stun_group, &addr,
				      sta, SDLC_PROXY_STATE_0);
		    else
			/* The other side is behind us. Go back one. */
			sdlc_newstate(idb->stun_group, &addr,
				      sta, SDLC_PROXY_STATE_2);
		} else {
		    sdlc_proxy_enqueue(stp, sta, pak);
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_5);
		    eat = TRUE;
		}
		break;

	    case SDLC_PROXY_STATE_5:
		sdlc_proxy_enqueue(stp, sta, pak);
		eat = TRUE;
		break;
	    }

	} else			/* secondary */ {

	    switch (sta->state) {

	    case SDLC_PROXY_STATE_0:
		if (SDLC_RR_AND_PF(sdlc, modulo)) {
		    sta->pnr = SDLC_NR(sdlc, modulo);
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_1);
		}
		break;

	    case SDLC_PROXY_STATE_1:
	    case SDLC_PROXY_STATE_3:
		if (!SDLC_RR_PF_AND_NR(sdlc, modulo, sta->pnr))
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		break;

	    case SDLC_PROXY_STATE_2:
		if (SDLC_RR_PF_AND_NR(sdlc, modulo, sta->pnr))
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_3);
		else
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		break;

	    case SDLC_PROXY_STATE_4:
		sti->lock = TRUE;
		sdlc_disable_proxy_gen(sti, sta);
		if (!SDLC_RR(sdlc) || !SDLC_PF_SET(sdlc, modulo) ||
		    (sta->pnr != SDLC_NR(sdlc, modulo)))
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_0);
		else
		    sdlc_newstate(idb->stun_group, &addr,
				  sta, SDLC_PROXY_STATE_3);
		sti->lock = FALSE;
		break;
	    }
	}
    }
    return(eat);
}

static ushort stun_sdlc_serial_input_local_ack (
     hwidbtype *idb,
     paktype *pak,
     stunpeertype *stp)
{
  int result;
  leveltype level;
  paktype *newpak;
  sdlc_header *frame;
  slackminor_sdlc slack_op;
  
  /*
   * If we are configured for group poll and receive a group poll then
   * we send the packet directly to sdlc_input for processing.
   */
  if (idb->sdlc_group_addr)
  {
      frame = (sdlc_header *)pak->datagramstart;
      if (frame->address == idb->sdlc_group_addr) {
          sdlc_input(idb, pak);
          return (STUN_OP_IGNORE);
      }
  }
  slack_op = slack_sdlc_categorize(pak,stp);

  /* 
   * If the SDLC Test command is running then Test frame should
   * be sent only to SDLC and not to the other STUN peer.
   */
  if (slack_op == SLACK_SDLC_TEST) {
      sdlc_input(idb, pak);
      return (STUN_OP_IGNORE);
  }
  result = slack_fsm(stp, SLACK_SDLC, slack_op);
  if (result & STUN_OP_ERR) {
    stif_nukepak(pak);
    return(STUN_OP_ERR);
  }
  if ((result & STUN_OP_OPCODE) == STUN_OP_LINKUP_REQ) {
    if ((idb->sdlc_group_addr) || (quick_response && 
                                  (!stp->now_xid_passthru)))
	return (result & STUN_OP_OPCODE);
    newpak = pak_duplicate(pak);
    if (!newpak) {
      stif_nukepak(pak);
      return(STUN_OP_IGNORE);
    }
    newpak->if_input = pak->if_input;
    newpak->if_output = pak->if_output;
    if (stp->slack_snrm_pak)
      datagram_done(stp->slack_snrm_pak);
    stp->slack_snrm_pak = newpak;
  }
  if (result & STUN_OP_TO_SDLC) {
    /*
     * Do not copy sdlc frame data - sdlc_input() only uses sdlc address and
     * control fields to update counts and local-ack the frame. The alternative
     * to this shortened copy is to change sdlc_input() so it does not consume
     * the pak. This requires major design changes.
     */
    newpak = pak_clip(pak, 2);
    if (!newpak) {
      errmsg(&msgsym(NOBUF, STUN), idb->hw_namestring, "local-ack sdlc frame");
      stif_nukepak(pak);
      return(STUN_OP_IGNORE);
    }
    newpak->if_input = pak->if_input;
    newpak->if_output = pak->if_output;
    level = raise_interrupt_level(NETS_DISABLE);
    sdlc_input(idb, newpak);
    reset_interrupt_level(level);
  }
  if ((result & STUN_OP_OPCODE) == STUN_OP_IGNORE) {
    stif_nukepak(pak);
  }
  return(result & STUN_OP_OPCODE);
}

static boolean stun_sdlc_network_input_local_ack (
     hwidbtype *idb,
     paktype *pak,
     stunpeertype *stp,
     ushort opcode)
{
  int result;
  sna_fid4_hdr *sna_hdr; /* used to distinguish SNA FID4 packets */

  result = slack_fsm(stp, SLACK_NETWORK,
		     slack_network_categorize((int)opcode));

  if (result == STUN_OP_PASS) { 
      if (quick_response && (!stp->now_xid_passthru)) { 
 
          if (stp->slack_xid_rsp_received) /* throw away the old one */
              stif_nukepak(stp->stored_xid_response);
 
          stp->stored_xid_response = pak;   
          stp->slack_xid_rsp_received=TRUE;
          return(TRUE);
      }   
  }

  if (result & STUN_OP_ERR) {
    stif_nukepak(pak);
    return(TRUE);
  }
  pak->if_output = firstsw_or_null(stp->stun_idb);
  if (result & STUN_OP_TO_SDLC) {
     /*
      * There is no other processing after giving a frame to SDLC. This is
      * the normal processing for I-frames and avoids a pak_duplicate().
      */
     /* if cos prioritization is enabled, attempt to preserve seq */
     sna_hdr = (sna_fid4_hdr *) (pak->datagramstart + 2);
     if ((stp->tg_active) && ((sna_hdr->fid_tg & SNA_FID) == SNA_FID4)) {
        pak->peer_ptr = stp;
        stun_cos_dest_reseq(stp, pak);
     }
     else
        sdlc_holdq(pak->if_output->hwptr,stp->stun_idb->sdlc_data[stp->addr[0]],pak);
    return(TRUE);
  }
  if ((result & STUN_OP_OPCODE) == STUN_OP_IGNORE) {
    stif_nukepak(pak);
    return(TRUE);
  } else {
    return(FALSE);
  }
}

/*
 * stun_cos_flush_reseq
 *
 * Sends STUN frames on queue to the appropriate sdlc_holdq.
 */
void stun_cos_flush_reseq (
    queuetype *queue)
{
    paktype              *newpak;
    stunpeertype         *newstp;
    leveltype            status;
    sna_fid4_hdr *sna_hdr;

    status = raise_interrupt_level(ALL_DISABLE);
    while ((newpak = pak_dequeue(queue))) {
        newstp = newpak->peer_ptr;
        sna_hdr = (sna_fid4_hdr *) (newpak->datagramstart+SNA_HDR_OFFSET);
        if (stun_tg_debug)
            buginf("\nSTUN_RESEQ: frame:%x flushed from reseqQ",
                    (sna_hdr->snf & TG_SNF));
        sdlc_holdq(newpak->if_output->hwptr,
                   newstp->stun_idb->sdlc_data[newstp->addr[0]], newpak);
    }
    reset_interrupt_level(status);
    return;
}

/*
 * stun_tg_process_zero
 *
 * Send frame 0 and dequeue all frames in resequence queue and send
 * them on the same interface as frame 0 to ensure the order of delivery.
 */

static void stun_tg_process_zero (
    stun_tg_info *tgi,
    stunpeertype *stp,
    paktype      *pak)
{
    paktype      *newpak;
    sna_fid4_hdr *sna_hdr;

    tgi->tg_stp = stp;
    tgi->tg_wait_for_zero = FALSE;
    sdlc_holdq(pak->if_output->hwptr,
               stp->stun_idb->sdlc_data[stp->addr[0]], pak);

    if (stun_tg_debug)
        buginf("\nSTUN_RESEQ: frame:0 to %s", stp->stun_idb->hw_namestring);

    while ((newpak = pak_dequeue(&tgi->tg_reseqQ))) {
        sna_hdr = (sna_fid4_hdr *) (newpak->datagramstart+SNA_HDR_OFFSET);
        sdlc_holdq(pak->if_output->hwptr,
                   stp->stun_idb->sdlc_data[stp->addr[0]], pak);
        if (stun_tg_debug)
            buginf("\nSTUN_RESEQ: frame:%x dequeued to %s",
                    (sna_hdr->snf & TG_SNF), stp->stun_idb->hw_namestring);
    }
    return;
}

/*
 * stun_cos_dest_reseq
 *
 * Performs FID4 TG sequence number resequencing for frames in the
 * range from 0 to TG_MAX_KEEP_ORDER. It is critical in TG environments
 * that frame 0 is the first frame delivered after the sequence wrap.
 */

void stun_cos_dest_reseq (stunpeertype *stp, paktype *pak)
{
    stun_tg_info *tgi;
    leveltype    status;
    sna_fid4_hdr *sna_hdr;
    ulong        pak_seq_num;

    status = raise_interrupt_level(ALL_DISABLE);
    sna_hdr = (sna_fid4_hdr *) (pak->datagramstart + SNA_HDR_OFFSET);
    pak_seq_num = (sna_hdr->snf & TG_SNF);

    for (tgi = (stun_tg_info *) stun_tg_infoQ.qhead; tgi; tgi = tgi->next) {
        if (tgi->group == stp->group)
            break;
    }
    if (!tgi) {
        errmsg(&msgsym(NOTGI, STUN), stp->group);

    } else if (pak_seq_num == TG_MAX_SEQ_NUM) {
        tgi->tg_wait_for_zero = TRUE;

    } else if ((tgi->tg_wait_for_zero) &&
               (pak_seq_num <= TG_MAX_KEEP_ORDER)) {

        if (pak_seq_num == 0) {
            stun_tg_process_zero(tgi, stp, pak);
            reset_interrupt_level(status);
            return;
        }

        if (data_enqueue(&tgi->tg_reseqQ, pak)) {

            if (stun_tg_debug)
                 buginf("\nSTUN_RESEQ: frame:%x queued, Q count is %d",
                         pak_seq_num, tgi->tg_reseqQ.count);

            if (QUEUEFULL(&tgi->tg_reseqQ))
                stun_cos_flush_reseq(&tgi->tg_reseqQ);

            reset_interrupt_level(status);
            return;
        }
    } else if ((pak_seq_num <= TG_MAX_KEEP_ORDER) && (tgi->tg_stp)) {

	/*
	  Due to an NCP bug, when a TG wraps, the next several
	  PIUs (9) must go out on the same link. This code causes
	  the same link to be used as the output link, and verifies that
	  the link is still active before using it. If it is not
	  active, then the wrap link is set to the current one.
	*/
      if (tgi->tg_stp->slack_state != SLACK_ACTIVE) {
	tgi->tg_stp = stp;
      } else {
	stp = tgi->tg_stp;
      }
    }

    sdlc_holdq(pak->if_output->hwptr,
               stp->stun_idb->sdlc_data[stp->addr[0]], pak);
    if (stun_tg_debug)
        buginf("\nSTUN_RESEQ: frame:%x to %s",
                pak_seq_num, stp->stun_idb->hw_namestring);
    reset_interrupt_level(status);
    return;
}

static int stun_sdlc_serial_input_hook (
     hwidbtype *idb,
     paktype *pak,
     stunpeertype *stp)
{
    sdlc_header *sdlc;
    sdlc_proxy_interface *sti;
    uchar addr;
    uchar modulo;

    sdlc = (sdlc_header *) pak->datagramstart;
    addr = (uchar) sdlc->address;

    sti = idb->stun_protocol_specific;
    if (stp->local_ack_active) {
      return(stun_sdlc_serial_input_local_ack(idb,
					      pak,
					      (stp->priority_top ?
					       stp->priority_top : stp)));
    } else if ((sti != NULL) &&
	       ((modulo = sti->proxy_info[addr].modulo) ||
		(sti->proxy_info[addr].discovery))) {
      return(stun_sdlc_serial_input_proxy(idb, pak, stp));
    } else
      return(STUN_OP_PASS);
}

static boolean stun_sdlc_network_input_hook (
     hwidbtype *idb,
     paktype *pak,
     stunpeertype *stp,
     ushort opcode)
{
    sdlc_header *sdlc;
    uchar addr;
    sdlc_proxy_interface *sti;
    uchar modulo;

    sdlc = (sdlc_header *) pak->datagramstart;
    addr = (uchar) sdlc->address;

    sti = idb->stun_protocol_specific;
    if (stp->local_ack_active) {
	stp = ((stp->priority_top) ? stp->priority_top : stp);
	return(stun_sdlc_network_input_local_ack(idb, pak, stp, opcode));
    } else if ((sti != NULL) &&
	       ((modulo = sti->proxy_info[addr].modulo) ||
		(sti->proxy_info[addr].discovery))) {
      return(stun_sdlc_network_input_proxy(idb, pak, stp, opcode));
    } else
      return(FALSE);
}

/*
 * Little show hook -- a few char. output for line. Header = TRUE for
 * header line. False for data.
 */
static void stun_sdlc_little_show_hook (
    boolean header,
    hwidbtype *idb,
    stunpeertype *stp)
{
    sdlc_proxy_interface *spi;
    if (header) {
	printf("   poll");
	return;
    }
    spi = idb->stun_protocol_specific;
    if ((spi) && (!stp->all_addr)) {
	if (spi->proxy_info[(int) stp->addr[0]].modulo) {
	    printf(" %-3u%c",
		   spi->proxy_info[(int) stp->addr[0]].modulo,
		   spi->proxy_info[(int) stp->addr[0]].primary ?
		   ('P') : ('S'));
	} else if (spi->proxy_info[(int) stp->addr[0]].discovery) {
	    printf(" NEG");
	}
    }
}

static boolean stun_sdlc_parse_global (parseinfo *csb)
{
    switch (csb->which) {
      case STUN_POLL:
	stun_sdlc_parse_pollint(csb);
	break;
      case STUN_PRIMARY:
	stun_sdlc_parse_primary_passthrough(csb);
	break;
      default:
	return(FALSE);
    }
    return(TRUE);
}

#ifndef WAYNES_WORLD
/*
**  This function and the call to this function are to be removed
**  prior to Release 9.1 FCS.
**					Wayne Clark, 08 May 92
*/
static void sdlc_parse_sdlc_role (parseinfo *csb)
{
    int encaptype;
    int addr;
    
    if (csb->sense) {
       if (GETOBJ(int,1)) {
           encaptype = ET_SDLCP;
       } else {
           encaptype = ET_SDLCS;
       }
       if (encaptype != csb->interface->hwptr->stun_sdlc_encap) {
           for (addr = SDLC_MIN_ADDRESS; (addr <= SDLC_MAX_ADDRESS); addr++) {
	       if (csb->interface->hwptr->sdlc_data[addr] != NULL) {
	           printf("\nSTUN/SDLC: You cannot change the sdlc-role while any SDLC addresses are");
		   printf("\n           defined for the interface (%s)", csb->interface->namestring);
		   return;
	       }
	   }
           csb->interface->hwptr->stun_sdlc_encap = encaptype;
           sdlc_setup2(csb->interface->hwptr);
       }
    } else {
       for (addr = SDLC_MIN_ADDRESS; (addr <= SDLC_MAX_ADDRESS); addr++) {
           if (csb->interface->hwptr->sdlc_data[addr] != NULL) {
	       printf("\nSTUN/SDLC: You cannot delete the sdlc-role while any SDLC addresses are");
	       printf("\n           defined for the interface (%s)", csb->interface->namestring);
	       return;
	   }
       }
       csb->interface->hwptr->stun_sdlc_encap = ET_STUN;
    }
}

#endif	WAYNES_WORLD

static boolean stun_sdlc_parse_interface (parseinfo *csb)
{
    switch (csb->which) {
      case STUNIF_PROXY:
	sdlc_parse_proxy(csb);
	return(TRUE);
#ifndef WAYNES_WORLD
      case STUNIF_SDLCROLE:
	sdlc_parse_sdlc_role(csb);
	return(TRUE);
#endif  WAYNES_WORLD
      default:
	break;
    }
    return(FALSE);
}

static boolean stun_sdlc_peer_change_hook (
     hwidbtype *idb,
     stunpeertype *stp,
     boolean exists)
{
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);
    if (exists) {
      /* Creating entry... */
      if (stp->local_ack_capable) {
	if (!stp->local_ack_active) {
	  stp->slack_state = SLACK_DISCONNECTED;
	  stp->local_ack_active = TRUE;
	  
	}
      } else {
	stp->slack_state = SLACK_DISCONNECTED;
	stp->local_ack_active = FALSE;
      }
    } else {
      /* Deleting entry... */
      stp->slack_state = SLACK_DISCONNECTED;
      stp->local_ack_active = FALSE;
      sdlc_reset(idb, "Administrative Shutdown", (idb->sdlc_data[stp->addr[0]]));
    }
    reset_interrupt_level(status);
    return(TRUE);
}

static boolean stun_sdlc_tg_peer_change_hook (
     hwidbtype *idb,
     stunpeertype *stp,
     boolean exists)
{
    hwidbtype *local_idb;
    stunpeertype *local_peer;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);
    if (exists) {
      /* Creating entry... */
      if (stp->local_ack_capable) {
	if (!stp->local_ack_active) {
	  stp->slack_state = SLACK_DISCONNECTED;
	  stp->local_ack_active = TRUE;
	  
	}
      } else {
	stp->slack_state = SLACK_DISCONNECTED;
	stp->local_ack_active = FALSE;
      }
      /*
       *  For Transmission Groups, make sure the IP address of this new
       *  stun peer matches the IP address of first stun peer in the
       *  same group.  
       *
       *  (Only the first stun peer needs to be checked since it is 
       *  impossible to add stunpeertypes to the stun_Q that have different
       *  IP addresses.
       */
      for (local_idb = stun_first_idb; local_idb; local_idb = IDB_NEXT(local_idb)) {
	  if ((local_idb->enctype    == ET_STUN)  &&
	      (local_idb->stun_group == stp->group)) {
              local_peer = (stunpeertype *)local_idb->stun_Q.qhead;
              if (local_peer) {
                 if (stp->ipaddr != local_peer->ipaddr) {
                    /* IP address of this new STUN peer doesn't match. */
                     printf
                     ("\nAll STUN peers for TGs must use the same IP address.");
                     reset_interrupt_level(status);
                     return(FALSE);
                 } else {
                     /* IP address of this new STUN peer matches. */
                     reset_interrupt_level(status);
                     return(TRUE);
                 }
              }
          }
      }
      /* Must be first stun peer for this TG.  Let's add it. */
      reset_interrupt_level(status);
      return(TRUE);
    } else {
      /* Deleting entry... */
      stp->slack_state = SLACK_DISCONNECTED;
      stp->local_ack_active = FALSE;
      stp->tg_active = FALSE;
      sdlc_reset(idb, "Administrative Shutdown",
		 (idb->sdlc_data[stp->addr[0]]));
    }
    reset_interrupt_level(status);
    return(TRUE);
}

stun_protocol_info *stun_sdlc_predefined_init (char **namep)
{
    stun_sdlc_init();
    sstrncpy(sdlc_protocol_info.name, "sdlc", NAMEDTHINGNAMELEN+1);
    sdlc_protocol_info.addrlength = 1;
    sdlc_protocol_info.addroffset = 0;
    sdlc_protocol_info.addrfmt = 'x';
    sdlc_protocol_info.prototype = SDLC_PTYPE;
    sdlc_protocol_info.onlyall = FALSE;
    sdlc_protocol_info.neverall = TRUE;
    sdlc_protocol_info.local_ack_capable = TRUE;
    sdlc_protocol_info.local_ack_required = FALSE;
    sdlc_protocol_info.permanent = TRUE;
    sdlc_protocol_info.uses_tg = FALSE;
    sdlc_protocol_info.serial_forward_hook = stun_sdlc_serial_input_hook;
    sdlc_protocol_info.network_forward_hook = stun_sdlc_network_input_hook;
    sdlc_protocol_info.shut_interface = stun_sdlc_shutint;
    sdlc_protocol_info.long_periodic_hook = sdlc_proxy_long_periodic;
    sdlc_protocol_info.show_hook = show_sdlc_proxy;
    sdlc_protocol_info.little_show_hook = stun_sdlc_little_show_hook;
    sdlc_protocol_info.global_parse_hook = stun_sdlc_parse_global;
    sdlc_protocol_info.interface_parse_hook = stun_sdlc_parse_interface;
    sdlc_protocol_info.global_nv_hook = stun_sdlc_nv_globals;
    sdlc_protocol_info.interface_nv_hook_1 = stun_sdlc_nv_1_interface;
    sdlc_protocol_info.interface_nv_hook_2 = stun_sdlc_nv_2_interface;
    sdlc_protocol_info.print_frame = sdlc_print_frame;
    sdlc_protocol_info.peer_change_hook = stun_sdlc_peer_change_hook;
    *namep = &(sdlc_protocol_info.name[0]);
    return(&sdlc_protocol_info);
}

stun_protocol_info *stun_sdlc_tg_predefined_init (char **namep)
{
    sstrncpy(sdlc_tg_protocol_info.name, "sdlc-tg", NAMEDTHINGNAMELEN+1);
    sdlc_tg_protocol_info.addrlength = 1;
    sdlc_tg_protocol_info.addroffset = 0;
    sdlc_tg_protocol_info.addrfmt = 'x';
    sdlc_tg_protocol_info.prototype = SDLC_PTYPE;
    sdlc_tg_protocol_info.onlyall = FALSE;
    sdlc_tg_protocol_info.neverall = TRUE;
    sdlc_tg_protocol_info.local_ack_capable = TRUE;
    sdlc_tg_protocol_info.local_ack_required = TRUE;
    sdlc_tg_protocol_info.permanent = TRUE;
    sdlc_tg_protocol_info.uses_tg = TRUE;
    sdlc_tg_protocol_info.serial_forward_hook = stun_sdlc_serial_input_hook;
    sdlc_tg_protocol_info.network_forward_hook = stun_sdlc_network_input_hook;
    sdlc_tg_protocol_info.shut_interface = stun_sdlc_shutint;
    sdlc_tg_protocol_info.long_periodic_hook = sdlc_proxy_long_periodic;
    sdlc_tg_protocol_info.show_hook = show_sdlc_proxy;
    sdlc_tg_protocol_info.little_show_hook = stun_sdlc_little_show_hook;
    sdlc_tg_protocol_info.global_parse_hook = stun_sdlc_parse_global;
    sdlc_tg_protocol_info.interface_parse_hook = stun_sdlc_parse_interface;
    sdlc_tg_protocol_info.global_nv_hook = stun_sdlc_nv_globals;
    sdlc_tg_protocol_info.interface_nv_hook_1 = stun_sdlc_nv_1_interface;
    sdlc_tg_protocol_info.interface_nv_hook_2 = stun_sdlc_nv_2_interface;
    sdlc_tg_protocol_info.print_frame = sdlc_print_frame;
    sdlc_tg_protocol_info.peer_change_hook = stun_sdlc_tg_peer_change_hook;
    *namep = &(sdlc_tg_protocol_info.name[0]);
    return(&sdlc_tg_protocol_info);
}
