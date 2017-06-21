/* $Id: arap.c,v 3.11.18.12 1996/09/10 18:51:22 asb Exp $
 * $Source: /release/112/cvs/Xsys/arap/arap.c,v $
 *------------------------------------------------------------------
 * Dummy file for Atalk_branch.
 *
 * September 1993, Steven Lin
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: arap.c,v $
 * Revision 3.11.18.12  1996/09/10  18:51:22  asb
 * CSCdi68276:  ARAP:  arap logging command requires debugging extensions
 * Branch: California_branch
 *
 * Revision 3.11.18.11  1996/08/28  12:35:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.18.10  1996/08/26  23:02:50  bjthomas
 * CSCdi63635:  %SYS-2-LINKED: Bad enqueue of, %SYS-2-INPUTQ: INPUTQ set,
 * but no idb
 * Branch: California_branch
 *
 * Revision 3.11.18.9  1996/08/03  23:18:17  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.11.18.8  1996/07/23  13:19:13  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.11.18.7  1996/06/28  23:04:35  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.11.18.6  1996/06/25  16:11:09  tkolar
 * CSCdi61155:  ARAP keepalive timeout will cause stack corruption
 * Branch: California_branch
 * Add more space for the tickle notification string, and
 * fix another small issue before it happens.
 *
 * Revision 3.11.18.5  1996/06/17  22:57:17  tkolar
 * CSCdi57713:  AppleTalk:  FORCE QUIT and BAD EXIT occuring at a high rate
 * Branch: California_branch
 *
 * Revision 3.11.18.4  1996/05/21  09:41:57  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.18.3  1996/05/08  04:09:48  tkolar
 * CSCdi57035:  ARAP error messages are too obscure
 * Branch: California_branch
 * Unobfuscate ARAP.
 *
 * Revision 3.11.18.2  1996/04/19  14:54:17  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.3.8.7  1996/04/09  19:44:03  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.3.8.6  1996/02/28  06:14:22  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.3.8.5  1996/02/21  02:52:30  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.3.8.4  1996/01/23  04:44:30  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.3.8.3  1995/12/19  03:31:35  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke
 *
 * Revision 2.3.8.2  1995/12/17  00:56:40  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.3.8.1  1995/11/17  16:10:53  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.11.18.1  1996/03/18  18:50:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.2  1996/03/07  08:27:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.9.2.1  1996/02/20  00:15:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/02/09  09:02:48  billw
 * CSCdi43680:  RADIUS doesnt support magic number ip address values
 * also add authstruct->auth_idb, and change auth->tty to auth->auth_tty
 *
 * Revision 3.10  1996/02/01  05:58:44  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.9  1996/01/18  19:47:23  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1996/01/16  08:27:32  dwong
 * CSCdi44145:  ATP packets sent to multicast address
 *
 * Revision 3.7  1995/12/18  04:16:53  tkolar
 * CSCdi42125:  Crash in _mnp4_overflowed
 * Reverse some unfortunate changes to the ARAP shutdown process
 *
 * Revision 3.6  1995/12/18  04:08:05  tkolar
 * CSCdi26866:  ARAP clients with local networks dont show up in chooser
 * Change incoming NBP LkUps to have the zone '*' before passing
 * to client.
 *
 * Revision 3.5  1995/12/17  18:23:59  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/29  00:16:07  lol
 * CSCdi42385:  TACACS+ crashes router in aaa_split_AV
 *
 * Revision 3.3  1995/11/18  08:05:41  tkolar
 * CSCdi43510:  ARAP xtacacs routines should look for local password
 *
 * Revision 3.2  1995/11/17  08:38:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.15  1995/11/08  20:48:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.14  1995/10/21  22:00:54  tkolar
 * CSCdi42197:  ARAP crashes in des_setkey and arapsec_old and arapsec
 * Remember to NULL out des_state before using it again.
 *
 * Revision 2.13  1995/10/02  20:42:40  lol
 * CSCdi36244:  Console line failing authorization should be more forgiving
 *              Authenticated console users now bypass authorization.
 *
 * Revision 2.12  1995/09/10  23:35:32  carrel
 * CSCdi40013:  User information needs to be present even when no login
 * occurs
 * aaa_create_user() now initializes the acc_method to be
 * AAA_ACC_METH_NONE.
 *
 * Revision 2.11  1995/08/09  16:18:31  jjohnson
 * CSCdi38326:  quicksort should be a utility function
 * delete arap_qsort from the arap subsystem
 * add new utility function qsort to libutil.a
 *
 * Revision 2.10  1995/08/08  20:50:17  lol
 * CSCdi37361:  AAA should not require auth for originating PPP connections
 *
 * Revision 2.9  1995/07/21  01:47:10  billw
 * CSCdi36156:  No AAA accounting messages for ARAP or Xremote
 *
 * Revision 2.8  1995/06/30  05:20:43  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.7  1995/06/27 17:07:04  tkolar
 * CSCdi35462:  DES code needs to be its own subsystem
 * Make it easier to export the source base.
 *
 * Revision 2.6  1995/06/18  19:54:27  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.5  1995/06/15  22:58:28  syiu
 * CSCdi35686:  Async callback AAA authorization and accounting phase not
 * working
 *
 * Revision 2.4  1995/06/15  03:26:32  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.3  1995/06/09  23:39:28  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.2  1995/06/09  12:58:16  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:04:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../arap/msg_arap.c"		/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include <ctype.h>
#include <stdlib.h>		/* for atoi */
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../h/config.h"
#include "../parser/parser_defs_config.h"
#include "parser_defs_arap.h"
#include "../parser/parser_actions.h"
#include "../h/sys_registry.h"
#include "../h/auth_registry.regh"
#include "aaa.h"
#include "../servers/tacacs_plus.h"
#include "../servers/tacacs.h"
#include "ttysrv.h"
#include "../h/login.h"
#include "connect.h"
#include "access.h"
#include "../atalk/macip_util.h"
#include "arap_mnp4.h"
#include "arap_v42bis.h"
#include "arap_smartbuf.h"
#include "../os/async.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_registry.h"
#include "../atalk/atalk_nbp.h"
#include "../atalk/at_fsm.h"
#include "../arap/ara_srvr_fsm.h"
#include "../arap/ara_conn_fsm.h"
#include "../arap/ara_input.h"
#include "../arap/ara_db.h"
#include "../arap/ara_server.h"
#include "../arap/ara_stats.h"
#include "arap.h"
#include "arap_public.h"
#include "arap_strdiacasecmp.h"
#include "../h/cs_registry.regh"
#include "../ts/callback.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../util/random.h"
#include "../util/qsort.h"
#include "arap_crc16.h"

/* ---------- Globals -----------------*/

int arap_authen_once = 1;
int arap_logging;
int arap_callback;
ushort arap_net;
uchar arap_zone[ZONELEN];
int arap_zonelen;

boolean bad_exit_errors;	/* useful in debugging images */


long arap_tot_conn;
long arap_tot_packin;
long arap_tot_packout;
long arap_getbuffer_failed;
/* ---------- Public Functions -----------*/

static inline char *ARAP_USER_P(araptype *ptr) {
    if (old_access_control) {
	return(ptr->tty->user);
    } else {
	char *user;
	if (!aaa_get_user_p(ptr->tty->ustruct, &user)) {
	    user = NULL;
	}
	return(user);
    }
}

static inline void arap_tickle_err (araptype *arapptr) {
    long curtime;

    if (bad_exit_errors) {
	GET_TIMESTAMP32(curtime);
	errmsg(&msgsym(XTICKLE, ARAP), arapptr->tty->ttynum,
		(ARAP_USER_P(arapptr)), arapptr->mnp4ptr->stats.packets_resent,
		arapptr->mnp4ptr->stats.packets_sent, 
		arapptr->last_data, curtime);
    }
    arapptr->force_quit = TRUE;
}

boolean ara_send_message (tt_soc *tty, paktype *pak)
{
    int count;
    if (tty->arapptr && (tty->arap_version == ARAP_VERSION2)) {
	tty->arapptr->text[0] = pak->atalk_datalen -1;
	for (count = 1; count < pak->atalk_datalen -1; count++) {
	    if (pak->transport_start[count-1] < 26)
		tty->arapptr->text[count] = 32;
	      else
		tty->arapptr->text[count] = pak->transport_start[count-1];
	}
	arap_send_msg(tty->arapptr, tty->arapptr->arap_seqnum++, MSG_TEXT_ALERT);
	return(TRUE);
    } else
    if (tty->arapptr && (tty->arap_version == ARAP_VERSION1)) {
	return(TRUE);
    }
    return(FALSE);
}


void arap_intercept(paktype *pak, boolean *intercepted)
{

    tt_soc *tty;
    uchar dnode;
    ushort dnet;
    ushort source_net, source_node = 0;
    int count;
    ddpTypes at_paktype;
    paktype *pak2;

    dnet = atalk_pak_dnet(pak);

    if (( dnet == arap_net) && arap_zonelen) {
	at_paktype = atalk_ValidPacket(pak,1,FALSE);
	if ((at_paktype == DDP_TYPE_Invalid) && (pak->if_input == NULL)) {
	    /* we generated the packet internally from the NBP stack */
	    at_paktype = DDP_TYPE_NBP;
	}
	dnode = atalk_pak_dnode(pak);

	/*
	 * If this is a NBP_FwdReq, convert to NBP_LkUp
	 * for processing.
	 */
	if ((pak->datagramstart[DDP_TYPE] == DDP_TYPE_NBP)
	  && ((pak->datagramstart[NBP_TYPE] >> 4) == NBP_FwdReq)
	  && (dnode == ATALK_ANY_ROUTER)) {
	    pak->datagramstart[NBP_TYPE] &= ~0xF0;
	    pak->datagramstart[NBP_TYPE] |= ((NBP_LkUp << 4) & 0xF0);
	    pak->datagramstart[DDP_DESTNODE] = 255;
	}

	/*  
	 * If this is an NBP lookup, change the zone field to '*' .
	 * That way the client will answer queries for all of the zones
	 * it is in.
	 */
	if ((pak->datagramstart[DDP_TYPE] == DDP_TYPE_NBP)
	    && ((pak->datagramstart[NBP_TYPE] >> 4) == NBP_LkUp)) {
	    NBPUntweak(pak);
        }

	if( (dnode != 255) && (dnode != 0) ) {
	    if (dnode > ALLlines) {
		/* if we get a bogus packet for something beyond the MODEMS
			array then bail.  */
		datagram_done(pak);
		*intercepted = TRUE;
		return;
	    }
	    tty = MODEMS[dnode];
	    if (tty && !modemcheck(tty) && tty->arapptr && 
                !arap_overflowed(tty) && tty->arapptr->packet_ready) {
		/* put the SRP data header on  -- specifically the flag byte
		    and the LLAP header.

		    Note:  The ARAP spec says that the LLAP destination and
			    source are to be set to 1 when a checksum has been
			    set in the packet.  It lies.  Look for a 1 in the
			    DDP checksum field instead.
		*/
		pak->datagramsize += ARAPAK_HDRLEN;
		pak->datagramstart -= ARAPAK_HDRLEN;

		pak->datagramstart[SRP_FLAG] = ARAP_DATA;
		pak->datagramstart[SRP_LLAP_DEST] = 0;  /* always 0 */
		pak->datagramstart[SRP_LLAP_SRC] = 0;
		pak->datagramstart[SRP_LLAP_TYPE] = ALAP_DDP_LONG;
		enqueue(&tty->arapptr->ttyOUT, pak);
	    } else {
		datagram_done(pak);
		if (tty->arapptr) tty->arapptr->overflow_pckts++;
	    }
	    *intercepted = TRUE;
	    return;
	} else {
	    /* this is a broadcast */
	    if ((at_paktype == DDP_TYPE_NBP) &&
		(pak->datagramstart[NBP_TYPE] >> 4 == NBP_LkUp)) {
		source_net = pak->datagramstart[NBP_NETHIGH];
		source_net <<= 8;
		source_net |= pak->datagramstart[NBP_NETLOW];
		if (source_net == arap_net) 
			source_node = pak->datagramstart[NBP_NODE];
		/* record the source_node so that we don't send the 
			broadcast to it */
	    }
	    pak->datagramsize += ARAPAK_HDRLEN;
	    pak->datagramstart -= ARAPAK_HDRLEN;

	    pak->datagramstart[SRP_FLAG] = ARAP_DATA;
	    pak->datagramstart[SRP_LLAP_DEST] = 0;  /* always 0 */
	    pak->datagramstart[SRP_LLAP_SRC] = 0;
	    pak->datagramstart[SRP_LLAP_TYPE] = ALAP_DDP_LONG;

	    if ((at_paktype != DDP_TYPE_RTMPReq) && 
		(at_paktype != DDP_TYPE_RTMPRsp)) {

		for (count = 1; count < ALLlines; count++) {
		    tty = MODEMS[count];
		    if (tty->arapptr && !modemcheck(tty) && 
                        !arap_overflowed(tty) && (count != source_node) && 
                        tty->arapptr->packet_ready){
			pak2 = arap_getbuffer(BIGDATA, tty->ttynum);
			if (pak2 != NULL) {
			    bcopy(pak->datagramstart, pak2->datagramstart,
					    pak->datagramsize);
			    pak2->datagramsize = pak->datagramsize;
			    enqueue(&tty->arapptr->ttyOUT, pak2);
			}
		    }
		}
	    }
	    datagram_done(pak);
	    *intercepted = TRUE;
	    return;
	}
    }
    return;
}

void init_arap_globals (void)
{
    make_crctab();
    arap_zonelen = 0;
    arap_net = 65535;
}


void nv_arap_global(void)
{
    nv_write(arap_logging, "arap logging");
}



/* 
 * show_arap  --   Display arap connections (summarized or one in detail)
 *
 */
void show_arap(parseinfo *csb)
{
    tt_soc *tty;
    int target;
    mnp4type *mnp4;
    araptype *arapptr;

    target = GETOBJ(int, 1);

    if (target == -1) {
	printf("Statistics are cumulative since last reboot\n");
	printf("Total ARAP connections: %d\n", arap_tot_conn);
	printf("Total Appletalk packets output: %d\n", arap_tot_packout);
	printf("Total Appletalk packets input: %d\n", arap_tot_packin);
	printf("Total failures to obtain buffers: %d\n", arap_getbuffer_failed);
	return;
    }
    tty = MODEMS[target];
    arapptr = tty->arapptr;

    if (!arapptr) {
	printf("Appletalk Remote Access is not running on line %d\n", target);
	return;
    }
    printf("Active for %d minutes, ", 
		ELAPSED_TIME(arapptr->timestarted)/ONEMIN);
    if (!TIMER_RUNNING(arapptr->disconnecttime))
	printf("Unlimited time left\n");
    else
	printf("%d minutes left\n", 
		TIME_LEFT_SLEEPING(arapptr->disconnecttime)/ONEMIN);

    if (arapptr->smartbuffer)
	printf("Doing smartbuffering\n");
    else
	printf("Smartbuffering disabled\n");

    printf("Appletalk packets output: %d\n", arapptr->at_out_pckts);
    printf("Appletalk packets input: %d\n", arapptr->at_in_pckts);
    printf("Appletalk packets overflowed: %d\n",  arapptr->overflow_pckts);

    printf("V42bis compression efficiency (incoming/outgoing):  %d%% / %d%%\n",
	   (arapptr->v42ptr->input_outchar * 100) /
	   (arapptr->v42ptr->input_inchar),
	   (arapptr->v42ptr->output_inchar * 100) /
	   (arapptr->v42ptr->output_outchar));

    mnp4 = arapptr->mnp4ptr;

    printf("MNP4 packets received: %d\n", mnp4->stats.packets_received);
    printf("MNP4 packets sent: %d\n", mnp4->stats.packets_sent);
    printf("MNP4 garbled packets received: %d\n", mnp4->stats.garbled_received);
    printf("MNP4 out of order packets received: %d\n", mnp4->stats.out_of_order);
    printf("MNP4 packets resent: %d\n", mnp4->stats.packets_resent);
    printf("MNP4 nobuffers: %d\n", mnp4->stats.nobuffers);
}

/*
 * arap_command --  line configuration command for ARAP
 *
 */
void arap_command(parseinfo * csb)
{
    tt_soc *tty;
    int ttycount;
    char *list;

    if (csb->nvgen) 
    {
	tty = MODEMS[(int) csb->line1];
	switch(csb->which) {
	case PARSER_ARAP_AUTHEN:
	    list = tty->arap_acc_list_name;
	    /* The list "arap authen default" is the default, so we never
	     * nvgen it unless it has the keyword one-time at the end.
	     */
	    if (list[0] == '\0') {
		if (tty->arap_actype & AAC_SINGLELINE) {
		    nv_write(TRUE, "%s one-time", csb->nv_command);
		}
	    } else {
		nv_write(TRUE, "%s %s", csb->nv_command, list);
		nv_add(tty->arap_actype & AAC_SINGLELINE, " one-time");
	    }
	    break;
	case PARSER_ARAP_TACACS:
	    nv_write(tty->arap_actype & AAC_TACACS, "%s", csb->nv_command);
	    nv_add(tty->arap_actype & AAC_SINGLELINE, " single-line");
	    break;
	case PARSER_ARAP_NETWORK:
	    nv_write(arap_zonelen, "%s %d %s", csb->nv_command, arap_net,
							arap_zone);
	    break;
	case PARSER_ARAP_LOGGING:
	    nv_write(arap_logging, "%s", csb->nv_command);
	    break;
	case PARSER_ARAP_CALLBACK:
	    nv_write(arap_callback, "%s", csb->nv_command);
	    break;
	case PARSER_ARAP_LOGGING_DEBUGEXTENSIONS:
	    nv_write(bad_exit_errors, "%s", csb->nv_command);
	    break;
	case PARSER_MNP4_LOGGING:
	    nv_write(mnp4_logging, "%s", csb->nv_command);
	    break;
	case PARSER_ARAP_MANUALPASSWORD:
	    nv_write(tty->arap_mask & ARAP_MANUALPASSWORD, "%s", 
				csb->nv_command);
	    break;
	case PARSER_ARAP_DEDICATED:
	    nv_write(tty->def_capabilities & MUSTARAP, "%s", csb->nv_command);
	    break;
	case PARSER_ARAP_ENABLED:
	    nv_write((tty->def_capabilities & MAYARAP), "%s",
		     csb->nv_command);
	    break;
	case PARSER_ARAP_NOGUEST:
	    nv_write(tty->arap_mask & ARAP_NOGUEST, "%s", csb->nv_command);
	    nv_add((tty->arap_mask & (ARAP_NOGUEST | ARAP_NOGUEST_IFNEEDED)) ==
	    	(ARAP_NOGUEST | ARAP_NOGUEST_IFNEEDED), " if-needed");
	    break;
	case PARSER_ARAP_NETACCESSLIST:
	    nv_write(tty->arap_netlist != 0, "%s %d", csb->nv_command,
			tty->arap_netlist);
	    break;
	case PARSER_ARAP_TIMELIMIT:
	    nv_write(tty->arap_timelimit != 0, "%s %d",
		     csb->nv_command, tty->arap_timelimit/ONEMIN);
	    break;
	case PARSER_ARAP_NOSMARTBUFFER:
	    nv_write(tty->arap_mask & ARAP_NOSMARTBUFFER, "%s", 
			csb->nv_command);
	    break;
	case PARSER_ARAP_WARNINGTIME:
	    nv_write(tty->arap_warningtime != 0, "%s %d", 
			csb->nv_command, tty->arap_warningtime/ONEMIN);
	    break;
	case PARSER_ARAP_ZONELIST:
	    nv_write(tty->arap_zonelist != 0, "%s %d", csb->nv_command,
			tty->arap_zonelist);
	    break;
	default:
	    break;
	}

	return;
    }

    if (csb->which == PARSER_ARAP_LOGGING) {
	arap_logging = csb->sense;
	return;
    }

    if (csb->which == PARSER_ARAP_LOGGING_DEBUGEXTENSIONS) {
	bad_exit_errors = csb->sense;
	return;
    }

    if (csb->which == PARSER_MNP4_LOGGING) {
	mnp4_logging = csb->sense;
	return;
    }

    if (csb->which == PARSER_ARAP_CALLBACK) {
	arap_callback = csb->sense;
	return;
    }

    if (csb->which == PARSER_ARAP_NETWORK) {
	arap_net = (ushort)GETOBJ(int,1);
	if (csb->sense) {
	    arap_zonelen = atalk_hex_filter(GETOBJ(string,1), 
			    arap_zone, ZONELEN);
	    if (arap_zonelen <= 0) {
		printf("% bad ARAP command:""zone not specified");
		return;
	    }
	} else {
	    arap_zonelen = 0;
	}
	if (csb->sense)
	    atroute_AddProxyPath(arap_net,arap_zonelen,arap_zone, TRUE);
	else
	    atroute_DeleteProxyPath(arap_net);
	return;
    }

    for (ttycount = csb->line1; ttycount < csb->line2; ttycount++)
    {

    	tty = MODEMS[ttycount];

	switch (csb->which) {
	case PARSER_ARAP_AUTHEN:
	    if (csb->sense) {
		char *list_name;
		switch (GETOBJ(int,2)) {
		case 1:
		    list_name = "default";
		    tty->arap_acc_list_name[0] = '\0';
		break;
		case 2:
		    list_name = GETOBJ(string,1);
		    sstrncpy(tty->arap_acc_list_name, list_name,
			     sizeof(tty->arap_acc_list_name));
		    break;
		default:
		    buginf("\n AAA: ARAP: bad value in authentication list.");
		    list_name = NULL;
		    break;
		}
		if ((csb->resolvemethod == RES_MANUAL) && list_name &&
		    !aaa_is_list_defined(list_name, AAA_SVC_ARAP)) {
		    printf("AAA: Warning authentication list \"%s\" is not "
			   "defined for ARAP.\n", list_name);
		}
		if (GETOBJ(int,4)) {
		  tty->arap_actype |= AAC_SINGLELINE;
		}
	    } else {
		tty->arap_acc_list_name[0] = '\0';
		tty->arap_actype &= ~AAC_SINGLELINE;
	    }
	    break;
	case PARSER_ARAP_TACACS:
	    if (csb->sense) {
		tty->arap_actype = AAC_TACACS;
		if (GETOBJ(int,1)) {
		    tty->arap_actype |= AAC_SINGLELINE;
		}
	    } else {
		tty->arap_actype &= ~(AAC_TACACS | AAC_SINGLELINE);
	    }
	    break;

	case PARSER_ARAP_ZONELIST:
	    if (csb->sense) tty->arap_zonelist = GETOBJ(int, 1);
		    else
			    tty->arap_zonelist = 0;
	    break;
	case PARSER_ARAP_NETACCESSLIST:
	    if (csb->sense) tty->arap_netlist = GETOBJ(int, 1);
		    else
			    tty->arap_netlist = 0;
	    break;
	case PARSER_ARAP_MANUALPASSWORD:
	    if (csb->sense) 
		tty->arap_mask |= ARAP_MANUALPASSWORD;
	      else
		tty->arap_mask &= ~ARAP_MANUALPASSWORD;
	    break;
	case PARSER_ARAP_ENABLED:
	    docaps(csb, MAYARAP, FIRST_CAP);
	    break;
	case PARSER_ARAP_TIMELIMIT:
	    if (csb->sense) tty->arap_timelimit = (GETOBJ(int, 1)) *ONEMIN;
		    else
			    tty->arap_timelimit = 0;
	    break;
	case PARSER_ARAP_NOSMARTBUFFER:
	    if (csb->sense) 
		tty->arap_mask |= ARAP_NOSMARTBUFFER;
	      else
		tty->arap_mask &= ~ARAP_NOSMARTBUFFER;
	    break;
	case PARSER_ARAP_DEDICATED:
	    docaps(csb, MUSTARAP, FIRST_CAP);
	    break;
	case PARSER_ARAP_NOGUEST:
	    if (GETOBJ(int,1) == PARSER_ARAP_NOGUEST_IFNEEDED) { 
		tty->arap_mask |= ARAP_NOGUEST_IFNEEDED;
	    } else {
		tty->arap_mask &= ~ARAP_NOGUEST_IFNEEDED;
	    }
	    if (csb->sense) {
		tty->arap_mask |= ARAP_NOGUEST;
	    } else {
		tty->arap_mask &= ~ARAP_NOGUEST;
	    }
	    break;
	case PARSER_ARAP_WARNINGTIME:
	    if (csb->sense) tty->arap_warningtime = (GETOBJ(int, 1)) *ONEMIN;
		    else
			    tty->arap_warningtime = 0;
	    break;
	default:
	    break;
	}
    }
}




/*
 * arap -- The ARAP command process
 * 
 * This gets called either from start_process or from the parser, depending on
 * how the line is configured.  It drives packets through the input and
 * output queues, and checks on various quit conditions.
 */

forktype arap(tt_soc *tty)
{
    boolean running = TRUE, failed = TRUE;	/* TRUE while we are wanted */
    araptype *arapptr;
    int v42;
    mnp4type *mnp4;

    tty->arapptr = init_arapptr(tty);	/* get our monster pointer with all
					 * of our mnp4, v42bis, and arap
					 * state conditions in it */
    if (tty->arapptr == NULL) {
	death(tty);
    }
    arapptr = tty->arapptr;
    mnp4 = arapptr->mnp4ptr;

    if (arap_zonelen) {
	tty->conncount++;	/* this counts as a line use */
        arap_tot_conn++;
	failed = FALSE;
    } else {
	errmsg(ARAP_ERR_CONFAILED, tty->ttynum, 
		"global command 'arap network' has not been configured");
	running = FALSE;
	failed = TRUE;
    }

    /*  
     * If this is an ARAP callback, resume ARAP where we left off 
     */
    if (tty->acflags & ACF_CALLBACK_ARAP) {
       if (!arap_restart(tty)) {
	    if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum)) {
		buginf("\nARAP TTY %t: arap_restart failed", tty->ttynum);
	    }
	    death(tty);
       }
    }

    /* 
     * For VTY lines, start the vty-async helper process and just
     * return for TTY lines.
     */
    reg_invoke_arap_start_pt_helper_process(tty->type, tty);

    while (running) {

	edisms((blockproc *)arapBLOCK, (int) arapptr);

        if (modemcheck(tty)) {
	    if (bad_exit_errors) {
		errmsg(ARAP_ERR_BADEXIT, tty->ttynum,
		       tty->user, "Modem CD dropped unexpectedly.");
	    }
            running = FALSE;
            break;
        }

	/* if we haven't heard from the other side for a while, 
		tickle them. */
	if (TIMER_RUNNING_AND_AWAKE(arapptr->tickletime)) {
	    TIMER_START(arapptr->tickletime, TICKLETIME);
	    arap_send_msg(arapptr, arapptr->arap_seqnum++, MSG_TICKLE);

	    /* While transferring data into mac clients, macs stop
	     * sending ARAP TICKLE and DATA unexpectedly. This causes router
	     * to disconnect on tickle timeout. When the last MNP4 ack
	     * is repeated, macs start sending MNP4 LTs.
	     */
	    mnp4_send_ack(arapptr->mnp4ptr);
	    ++arapptr->ticklecount;

	    if ((tty->arap_version == ARAP_VERSION2  &&
		arapptr->ticklecount == MAX_UNACKED_TICKLES2) || 
		(tty->arap_version != ARAP_VERSION2 &&
		arapptr->ticklecount == MAX_UNACKED_TICKLES1)) {
		arap_tickle_err(arapptr);
	    }
	}

	/* if they have a time limit, enforce it. */
	if (TIMER_RUNNING_AND_AWAKE(arapptr->disconnecttime)) {
	    arapptr->force_quit = TRUE;
	    if (bad_exit_errors) {
		errmsg(ARAP_ERR_BADEXIT, arapptr->tty->ttynum,
		       (ARAP_USER_P(arapptr)), "User exceeded timelimit");
	    }
	} else if (TIMER_RUNNING_AND_AWAKE(arapptr->warningtime)) {
	    arap_send_msg(arapptr, arapptr->arap_seqnum++, MSG_TIMELEFT);
	    TIMER_STOP(arapptr->warningtime);
	}

	if (arapptr->force_quit) {
	    if (bad_exit_errors) {
		errmsg(ARAP_ERR_BADEXIT, arapptr->tty->ttynum, 
		   (ARAP_USER_P(arapptr)), "ARAP connection was terminated.");
	    }
	    running = FALSE;
	    break;
	}

	if (!mnp4_timers(mnp4)) {
	    arapptr->force_quit = TRUE;
	}

	/* clean everything coming in */
	running = mnp4_input(mnp4, &arapptr->v42InQ);
	if (!running) {
	    if (bad_exit_errors) {
		errmsg(&msgsym(MNP4RCVDISC, ARAP), mnp4->tty->ttynum);
	    }
	}
	v42 =v42_input(tty, &arapptr->v42InQ, &arapptr->ARAPInQ);
	if (v42 < 0) {
	    running = FALSE;
	    if (bad_exit_errors) {
		errmsg(ARAP_ERR_BADEXIT, arapptr->tty->ttynum, 
		       (ARAP_USER_P(arapptr)), 
		       "v42_input running (may be low memory");
	    }
	}

	if (arap_input(arapptr) < 0) {
	    running = FALSE;
	    if (bad_exit_errors) {
		errmsg(&msgsym(REMOTEERROR, ARAP), mnp4->tty->ttynum);
	    }
	}

	process_suspend();   /* relax and be a good citizen */

	if (!mnp4_timers(mnp4)) {
	    arapptr->force_quit = TRUE;
	    if (bad_exit_errors) {
		errmsg(&msgsym(TIMERERROR, ARAP), mnp4->tty->ttynum);
	    }
	}

	/* clean everything going out */
	arap_output(arapptr);
	v42 =v42_output(tty, &arapptr->v42OutQ, &arapptr->MNP4OutQ);
	if (v42 < 0) {
	    running = FALSE;
	    if (bad_exit_errors) {
		errmsg(ARAP_ERR_BADEXIT, arapptr->tty->ttynum, 
		       (ARAP_USER_P(arapptr)), 
		       "v42_output running (may be low memory)");
	    }
	}

	mnp4_output(mnp4, &arapptr->MNP4OutQ);

    }				/* while (running) */

    if (!failed) {
	if (arap_logging)  {
	    errmsg(ARAP_ERR_ADDRFREE, arapptr->tty->ttynum,
		   (ARAP_USER_P(arapptr)),
		   atalk_address(arap_net, arapptr->tty->ttynum),
		   ELAPSED_TIME(arapptr->timestarted)/ONESEC);
	}
	/* No need to call aaa_acct_end. Death will do this for us. */
    }
    tty->arap_version = ARAP_NOVERSION;
    death(tty);			/* we die now */
}

/*
 * This is required to send a MNP4 LR and then ARAP internal packet 
 * sequences out during the callback to ARAP client. 
 * Returns TRUE if callback exchange went as expected.
 * Otherwise, return FALSE. 
 */
static boolean arap_restart(tt_soc *tty)
{
   int count = 0;
   int status;
   mnp4type *mnp4 = tty->arapptr->mnp4ptr;

   /*
    * Expect a LR before proceeding, resend up to MNP4__INITIATOR_RETRIES
    */
   while (TRUE) {
     
     /*
      * Transmit a MNP4 LR packet out since now we are the MNP4 initiator 
      * in the callback attempt.  
      */
     send_link_request_response(mnp4); 
     status = wait_for_mnp4_remote(mnp4, MNP4_LR);
     if (status == OK){
	mnp4_send_ack(mnp4);
        break;
     }
     else if (status == TIMEDOUT) {
        count++; 
        if (count > MNP4_INITIATOR_RETRIES){
           errmsg(ARAP_ERR_TIMEOUT, tty->ttynum, "MNP4_LR");
           return(FALSE);
	}
        if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
           buginf("\nARAP TTY %t: arap_restart TIMEDOUT, count=%d", tty->ttynum,
		count);
     } else if (status == ABORTED){
        if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
           buginf("\nARAP TTY %t: arap_restart ABORTED", tty->ttynum);
        errmsg(ARAP_ERR_ABORTED, tty->ttynum, "MNP4_LR");
        return(FALSE);
     }
   }

   /*
    * Resume ARAP protocol sequence where the original incoming ARAP 
    * session had left off.
    * ARAP protocol's callback is a detour from the regular ARAP login 
    * sequence. After callback is initiated successfully at MNP4 level, 
    * it will resume the track where it last detoured off to do callback
    */
   if (!arap_resume(tty->arapptr))
      return(FALSE);       

   tty->arapptr->arap_started = TRUE;
   tty->arapptr->packet_ready = TRUE;
   GET_TIMESTAMP(tty->arapptr->timestarted);  /* set real start time */
   return(TRUE);
}
/* 
 * arap_cleanup -- clean up arap context
 */
void arap_cleanup(tt_soc *tty)
{
    if (tty && tty->arapptr){
       mnp4_disable(tty);
       destroy_v42(tty);
       destroy_arapptr(tty->arapptr); 
       tty->arapptr = NULL;
       tty->mnp4ptr = NULL;
    }
}

/*
 * Return ARAP callback flag
 */
boolean arap_callback_enabled(void)
{
   return(arap_callback);
}


/*
 * exec_arap_command -- parser command for ARAP
 */
void exec_arap_command(parseinfo *csb)
{

    if (!(stdio->def_capabilities & MAYARAP)) {
	printf("ARAP is not enabled on this line.");
	return;
    }
    process_set_name("Exec ARAP");
    arap(stdio);		/* start up arap. */

    kfork(stdio);		/* Automatically kill off the exec 
					when arap exits */

}

/*
 * The following are junk functions covering packet allocation and release.
 * They don't do anything useful now, but they're very handy when finding
 * memory leaks
 */

paktype *arap_input_getbuffer(int size, hwidbtype * idb, int ttynum)
{
    paktype *ret;

    ret = input_getbuffer(size, idb);
    if (ret == NULL) {
        return (NULL);
    }
    ret->datagramstart = ((uchar *) ret->data_area + ENCAPBYTES);
    if (ARAP_BUGINF(arapmemory_debug, aramemtty_debug, ttynum)) 
	buginf("\nARAP MEM TTY %t: arap_input_getbuffer %x", ttynum, ret);
    return (ret);
}


paktype *arap_getbuffer(int size, int ttynum)
{
    paktype *ret;
    leveltype level;

    ret = getbuffer(size);
    if (ret == NULL) {
	if (!onintstack()) {
	    process_sleep_for(ONESEC);
	    ret = getbuffer(size);
	}
	if (ret == NULL) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    arap_getbuffer_failed++;
	    reset_interrupt_level(level);
	    return(NULL);
	}
    }
    ret->datagramstart = ((uchar *) ret->data_area + ENCAPBYTES);
    ret->datagramsize = 0;
    if (ARAP_BUGINF(arapmemory_debug, aramemtty_debug, ttynum))
	buginf("\nARAP MEM TTY %t: arap_getbuffer %x", ttynum, ret); 
    ret->dataptr = ret->datagramstart;
    return (ret);
}

void arap_datagram_done(paktype *pak, int ttynum)
{
    if (ARAP_BUGINF(arapmemory_debug, aramemtty_debug, ttynum))
	buginf("\nARAP MEM TTY %t: arap_datagram_done %x", ttynum, pak);  
    datagram_done(pak);
}

paktype *arap_pak_duplicate(paktype *pak, int ttynum)
{
    paktype *ret;

    ret = pak_duplicate(pak);
    while (ret == NULL) {
	process_sleep_for(ONESEC);
	ret = pak_duplicate(pak);
    }
    if (ARAP_BUGINF(arapmemory_debug, aramemtty_debug, ttynum))
	buginf("\nARAP MEM TTY %t: arap_pak_duplicate %x", ttynum, ret);  
    return (ret);
}

/*
 * arap_overflowed --  return TRUE if arap is clogged up 
 *
 */
boolean arap_overflowed(tt_soc *tty)
{
    return (mnp4_overflowed(tty->arapptr->mnp4ptr));
}

/*
 * get_tty_mnp4ptr  --  return the mnp4ptr for a tty.
 *
 *   This function borders on being a kludge.  It is here so that the mnp4
 *     module does not have to know what the arap structure looks like
 */
mnp4type *get_tty_mnp4ptr(tt_soc *tty)
{
    return(tty->arapptr->mnp4ptr);
}

/*
 * get_tty_v42ptr  --  return the v42ptr for a tty.
 *
 *   This function borders on being a kludge.  It is here so that the v42bis 
 *     module does not have to know what the arap structure looks like
 */
v42type *get_tty_v42ptr(tt_soc *tty)
{
    return(tty->arapptr->v42ptr);
}

/*
 * get_tty_martbufptr
 *
 *   This function borders on being a kludge.  It is here so that the 
 *   smartbuffer module does not have to know what the arap structure looks like
 */
ring_t *get_tty_smartbufptr(tt_soc *tty, int type)
{
    if (type == SMARTBUF_INPUT) {
	return(tty->arapptr->smartbuf_input);
    } else if (type == SMARTBUF_OUTPUT) {
	return(tty->arapptr->smartbuf_output);
    } else {	
	return NULL;
    }
}

/*  ------------  Private Functions ------------- */

/*
 * arap_input --  input queue dispatch loop for ARAP
 * 
 * strip packets off of ARAPInQ, take off the modem tool header and dispose of
 * them properly
 * 
 */
static int arap_input(araptype *arapptr)
{
    paktype *pak;
    ushort net;

    while ((pak = dequeue(&arapptr->ARAPInQ))) {
	/* strip the modem tool header */
	pak->datagramstart += MT_HEADERLEN;
	pak->datagramsize -= MT_HEADERLEN;

	/* if we haven't initialized the session yet, start it */
	if (!arapptr->arap_started) {
	    arapptr->arap_started = TRUE;
	    if (arap_startup(arapptr, pak) < 0)
		return (-1);
	    arapptr->packet_ready = TRUE;
	    GET_TIMESTAMP(arapptr->timestarted);  /* set real start time */
	} else { 

	    /* mark that a packet has been received */
	    TIMER_START(arapptr->tickletime, TICKLETIME);
	    GET_TIMESTAMP32(arapptr->last_data); 
	    arapptr->ticklecount = 0;	/* reset tickle count */

	    /* ack tickles */
	    if ((pak->datagramstart[ARAPAK_CMD] == MSG_TICKLE) &&
		(pak->datagramstart[ARAPAK_DATAFLAG] == ARAP_INTERNAL)) {

		if (pak->datagramstart[ARAPAK_SEQNUM] == arapptr->arap_acked) {
		    if (ARAP_BUGINF(arapinternal_debug, arainttty_debug,
			    arapptr->tty->ttynum)) 
			buginf("\nARAP TTY %t: Discarded TICKLE",
			    arapptr->tty->ttynum);
		    arap_datagram_done(pak, arapptr->tty->ttynum);
		    continue;
		} else {
		    if (ARAP_BUGINF(arapinternal_debug, arainttty_debug,
			    arapptr->tty->ttynum)) 
			buginf("\nARAP TTY %t: Received TICKLE",
			    arapptr->tty->ttynum);
		    arap_send_msg(arapptr, pak->datagramstart[ARAPAK_SEQNUM], 
							MSG_ACK);
		    arapptr->arap_acked = pak->datagramstart[ARAPAK_SEQNUM];
		    arap_datagram_done(pak, arapptr->tty->ttynum);
		    continue;
		}
	    }
	    /* discard acks */
	    if ((pak->datagramstart[ARAPAK_CMD] == MSG_ACK) &&
		(pak->datagramstart[ARAPAK_DATAFLAG] == ARAP_INTERNAL)) {
		arap_datagram_done(pak, arapptr->tty->ttynum);
		continue;
	    }

	    if (arapptr->smartbuffer &&
		(pak->datagramstart[ARAPAK_DATAFLAG] != ARAP_DATA)) {
		paktype *pak2;
		int temp;

		pak2 = arap_getbuffer(BIGDATA, arapptr->tty->ttynum);
		if (pak2 == NULL) {
		    /* This box is in trouble, toss the ARAP client. */
		    arap_datagram_done(pak, arapptr->tty->ttynum);
		    return(-1);
		}

		smartbuffer_decode(&(arapptr->smartbuf_conf), 
			arapptr->tty, pak->datagramstart, pak->datagramsize, 
			pak2->datagramstart, &temp);
		pak2->datagramsize = temp;
		arap_datagram_done(pak, arapptr->tty->ttynum);
		pak = pak2;
		pak->datagramstart--;
		pak->datagramsize++;
	    }

	    /* discard anything that isn't data */
	    if (!(arapptr->smartbuffer) && 
		(pak->datagramstart[ARAPAK_DATAFLAG] != ARAP_DATA))
	    {
		errmsg(ARAP_ERR_PAKINVALID, arapptr->tty->ttynum);
	    }

	    if (arapptr->accesslist) {
		/* make net equal DDP destination net */
		net = pak->datagramstart[ARAPAK_HDRLEN + DDP_DESTNETHIGH];
		net <<= 8;
		net |= pak->datagramstart[ARAPAK_HDRLEN + DDP_DESTNETLOW];
		if (!atalk_accesscheck (net, arapptr->accesslist, FALSE)) {
		    arap_datagram_done(pak, arapptr->tty->ttynum);
		    continue;
		}
	    }

	    /*
	     *  Modify the packet so atalk_send will accept it, and hand it
	     *   to atalk_send...
	     */
	    arap_make_atalk(arapptr, pak);


	    arapptr->at_in_pckts++;
	    arap_tot_packin++;

	    pak->line_input = TRUE;
	    if ((pak->datagramstart[DDP_TYPE] == DDP_TYPE_NBP)
		&& ((pak->datagramstart[NBP_TYPE] >> 4) == NBP_BrRq)) {
		atalk_enqueue(pak);
	    } else {
	        /* Always set dstmcast to unicast before calling atalk_send */
	        pak->atalk_dstmcast = ATALK_UNICAST;
		atalk_send(pak);
	    }
	}
    }
    return (0);
}

/*
 * arap_make_atalk -- make an ARAP packet look like it originated in the CS
 * 
 * Fill in the fields of the packet so that it will ship out of our appletalk
 * interface correctly
 */
static void arap_make_atalk(araptype *arapptr, paktype *pak)
{
    uchar *newstart;
    int count;


    newstart = ((uchar *) pak->data_area + ENCAPBYTES);
    /* if the llap source equal 1, do the checksum */
    pak->datagramstart += ARAPAK_HDRLEN; 
    pak->datagramsize -= ARAPAK_HDRLEN;

    for (count = 0; count < pak->datagramsize; count++)
    {
	newstart[count] = pak->datagramstart[count];
    }
    pak->datagramstart = newstart;
    pak->linktype = LINK_APPLETALK;
    pak->datagramstart[DDP_LENGTHHIGH] = pak->datagramsize >> 8;
    pak->datagramstart[DDP_LENGTHLOW] = pak->datagramsize;
    pak->enctype = ET_ETHERTALK;


    /* if this is an NBP broadcast lookup, tweak it */
    if ((pak->datagramstart[DDP_TYPE] == DDP_TYPE_NBP)
	&& ((pak->datagramstart[NBP_TYPE] >> 4) == NBP_BrRq)) {
	NBPTweak(arapptr, pak);
    }


    pak->datagramstart[DDP_LENGTHHIGH] = pak->datagramsize >> 8;
    pak->datagramstart[DDP_LENGTHLOW] = pak->datagramsize;
    pak->if_input =  NULL;  
    atalk_pak_initdataptr(pak);
    if (pak->datagramstart[DDP_CKSUMHIGH] | pak->datagramstart[DDP_CKSUMLOW]) {
    /* if any bits are set in the checksum, recalculate it */
	atalk_DDPcksum(pak);
    }
}

/*
 * NBPUntweak
 *
 * Given an NBP LkUp packet, replace the existing zone with '*'.
 */

static void NBPUntweak(paktype *pak)
{
    uchar *zoneptr, *packet;
    int offset;

    /* set ptr to point to the zone name field of an NBP LkUp packet. */
    packet = pak->datagramstart;

    offset = 20;   /* This is the object field of the first NBP tuple in a
			long DDP NBP packet */

    offset += packet[offset] + 1;  /*  Now we point at the type field */
    offset += packet[offset] +1; /*  And finally we point at the zone field */
    zoneptr = &packet[offset];

    /* 
     * replace zone with * 
     *
     * Remember we're dealing with Pascal strings here.  The first byte is
     *  the length.
     */

    pak->datagramsize -= (*zoneptr -1);
    *zoneptr = 1;
    zoneptr++;
    *zoneptr = '*';
}

/*
 * NBPTweak
 *
 *  Replace the "zone" on an outgoing NBP BrRq with the zone associated
 *    with the ARAP user.
 */
static void NBPTweak(araptype *arapptr, paktype *pak)
{
    uchar *zoneptr, *packet;
    int offset;

    /* set zoneptr to point to the zone name field of an NBP BrRq packet. */
    packet = pak->datagramstart;

    offset = 20;   /* This is the object field of the first NBP tuple in a
			long DDP NBP packet */
    offset += packet[offset] + 1;
    offset += packet[offset] +1;
    zoneptr = &packet[offset];

    /* 
     * if zone field is a *, replace with our zone 
     * Remember this is a Pascal string; first byte is the length.
     */
    if ((*zoneptr == 1) && (*(++zoneptr) == '*')) {
	bcopy(arap_zone, zoneptr, arap_zonelen);
	*(zoneptr - 1) = arap_zonelen;
	pak->datagramsize += arap_zonelen - 1;

    }
    pak->datagramstart[DDP_DESTNETHIGH] = 0;
    pak->datagramstart[DDP_DESTNETLOW] = 0;
    pak->datagramstart[DDP_DESTNODE] = 0;
}

/*
 * arap_startup --  initialize an arap session
 * 
 * Perform the startup dance of the rare and exotic ARAP beast.  This is all
 * done by lock-step:  send a message, wait for ACK, get a message, ACK it,
 * etc.
 */

static int arap_startup(araptype *arapptr, paktype *pak)
{
    paktype *pak2;
    boolean quit = FALSE;


    /*
     * first packet start...this is the link arbitration packet.  We take it
     * off the wire (passed as the argument), change the pktType code from 1
     * to 2, and send it back.  I would imagine that this will have to change
     * when we get the telebit code
     */
    pak2 = arap_getbuffer(MEDDATA, arapptr->tty->ttynum);
    if (pak2 == NULL) {
	/* we don't have enough memory to be starting this session. */
        arap_datagram_done(pak, arapptr->tty->ttynum);
	return(-1);
    }
    bcopy(pak->datagramstart, pak2->datagramstart, pak->datagramsize);
    pak2->datagramsize = pak->datagramsize;
    arap_datagram_done(pak, arapptr->tty->ttynum);
    pak2->datagramstart[ARA_LNKARB_TYPELOW] = ARA_LNKARB_CRR;
    enqueue(&arapptr->ttyOUT, pak2);

    /* this is the rest of the dialogue */
    arap_send_msg(arapptr, arapptr->arap_seqnum++, MSG_SRVRVERSION);
    if (wait_for_remote(arapptr, MSG_ACK) < 0)
	return (-1);
    if (wait_for_remote(arapptr, MSG_RMTVERSION) < 0)
	return (-1);
    arap_send_msg(arapptr, arapptr->arap_acked++, MSG_ACK);
    do {
	if (arapptr->auth.resultcode == ERR_VLD8_MANUALPASSWORDREQUIRED) {
	   if (wait_for_remote(arapptr, MSG_AUTH_AGAIN) < 0)
		return(-1);
	   arap_send_msg(arapptr, arapptr->arap_acked++, MSG_ACK);
	}
	arap_send_msg(arapptr, arapptr->arap_seqnum++, MSG_AUTH_CHALLENGE);
	if (wait_for_remote(arapptr, MSG_ACK) < 0)
	    return (-1);
	if (wait_for_remote(arapptr, MSG_AUTH_REQUEST) < 0)
	    quit = TRUE;
	arap_send_msg(arapptr, arapptr->arap_acked++, MSG_ACK);
        if ((arapptr->info) && arapptr->tty && 
            (arapptr->tty->arap_version == ARAP_VERSION1) &&
            reg_invoke_callback_enabled(arapptr->info,arapptr->tty,
                                        ACF_CALLBACK_ARAP)){
           arapptr->auth.resultcode = ERR_VLD8_CALLBACK;
	}
	arap_send_msg(arapptr, arapptr->arap_seqnum++, MSG_AUTH_RESPONSE);
	if (wait_for_remote(arapptr, MSG_ACK) < 0)
	    return (-1);

        /*
         * Trigger ARAP 1.0 callback if required
         */ 
        if ((arapptr->info) && arapptr->tty && 
            (arapptr->tty->arap_version == ARAP_VERSION1)){
           if (ARAP_BUGINF(arapinternal_debug, arainttty_debug,
		arapptr->tty->ttynum))
             buginf("\nARAP TTY %t: arap_startup initiating callback ARAP 1.0",
		arapptr->tty->ttynum);
           if (reg_invoke_callback_initiate(arapptr->info, arapptr->tty, 
                                        ACF_CALLBACK_ARAP, TRUE)){
              /*
               * If we get here, then there was a problem with callback. 
               * Kill the line
               */
              death(arapptr->tty);
	   }
        }  
    } while (arapptr->auth.resultcode == ERR_VLD8_MANUALPASSWORDREQUIRED);
    if (quit)
	return (-1);
    if (arapptr->force_quit) return(-1);

     /*
     * Trigger ARAP 2.0 callback if required
     */ 
    if (arapptr->info && arapptr->tty && 
        (arapptr->tty->arap_version == ARAP_VERSION2) &&
        reg_invoke_callback_enabled(arapptr->info,arapptr->tty,
                                        ACF_CALLBACK_ARAP)){
        if (ARAP_BUGINF(arapinternal_debug, arainttty_debug,
		arapptr->tty->ttynum))
           buginf("\nARAP TTY %t: arap_startup initiating callback ARAP 2.0",
		arapptr->tty->ttynum);
        arap_send_msg(arapptr, arapptr->arap_seqnum++, MSG_CALLBACK);
        if (wait_for_remote(arapptr, MSG_ACK) < 0)
	    return (-1);
        if (reg_invoke_callback_initiate(arapptr->info, arapptr->tty, 
                                         ACF_CALLBACK_ARAP, TRUE)){
            /*
             * If we get here, then either the registry is a stub or
             * there was a problem with callback. Kill the line
             */
            death(arapptr->tty);           
	}
    }

    arap_send_msg(arapptr, arapptr->arap_seqnum++, MSG_STARTINFOFROMSERVER);
    if (wait_for_remote(arapptr, MSG_ACK) < 0)
	return (-1);
    if (wait_for_remote(arapptr, MSG_STARTINFOFROMREMOTE) < 0)
	return (-1);
    arap_send_msg(arapptr, arapptr->arap_acked, MSG_ACK);
    while(arap_send_msg(arapptr, arapptr->arap_seqnum++, MSG_ZONELISTINFO))
    {
	if (wait_for_remote(arapptr, MSG_ACK) < 0)
	    return (-1);
    }
    if (arap_logging)
        errmsg(ARAP_ERR_ADDRUSED, arapptr->tty->ttynum, (ARAP_USER_P(arapptr)),
			arap_net, arapptr->tty->ttynum); 

    return (0);

}


/*
 * arap_resume --  resume an arap session
 * 
 * Resume ARAP protocol sequence where the original incoming ARAP 
 * session had left off.
 * ARAP protocol's callback is a detour from the regular ARAP login 
 * sequence. After callback is initiated successfully at MNP4 level, 
 * it will resume the track where it last detoured off to do callback
 */
static boolean arap_resume(araptype *arapptr)
{
    arap_send_msg(arapptr, arapptr->arap_seqnum++, MSG_STARTINFOFROMSERVER);
    if (wait_for_remote(arapptr, MSG_ACK) < 0)
	return (FALSE);
    if (wait_for_remote(arapptr, MSG_STARTINFOFROMREMOTE) < 0)
	return (FALSE);
    arap_send_msg(arapptr, arapptr->arap_acked, MSG_ACK);
    while(arap_send_msg(arapptr, arapptr->arap_seqnum++, MSG_ZONELISTINFO))
    {
	if (wait_for_remote(arapptr, MSG_ACK) < 0)
	    return (FALSE);
    }
    return (TRUE);
}

/*
 * arap_send_msg -- send the various ARAP internal messages properly
 * 
 * This is the real nitty gritty of the ARAP packet types.  A lot of things are
 * hard coded here that probably shouldn't be.  Note that data packets skip
 * this section.
 */
static int arap_send_msg(araptype *arapptr, int seqnum, int type)
{
    paktype *pak;
    ushort count;
    ziptabletype *zone;
    extern char *hostname;
    int numzones;
    int i;
    ziptabletype **my_table;
    ulong time, temp;
    int res;
    int v42;
    arap_packet *arapak;
    uchar *data;
    tt_soc *tty;

    tty = arapptr->tty;
    res = 0;
    pak = arap_getbuffer(MEDDATA, tty->ttynum);	/* get a buffer */
    if (pak == NULL) return(0);
    pak->datagramstart = ((uchar *) pak->data_area + ENCAPBYTES);
    pak->datagramstart += MT_HEADERLEN;	/* offset so the modem tool length has
				 * someplace to fit */

    arapak = (arap_packet *) pak->datagramstart;

    /* check the spec for the definitions of these packets */
    switch (type) {
    case MSG_ACK:
	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
	    buginf("\nARAP TTY %t: --------- ACKing %d ---------", tty->ttynum,
		seqnum); 
	arapak->ack.dataflag = ARAP_INTERNAL;
	arapak->ack.seqnum = seqnum;
	arapak->ack.cmdhighbyte = 0;
	arapak->ack.cmd = MSG_ACK;
	pak->datagramsize = 4;
	break;
    case MSG_SRVRVERSION:
	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
	    buginf("\nARAP TTY %t: --------- SRVRVERSION ---------",
		tty->ttynum); 
	arapak->srvrversion.dataflag = ARAP_INTERNAL;
	arapak->srvrversion.seqnum = seqnum;
	arapak->srvrversion.cmdhighbyte = 0;
	arapak->srvrversion.cmd = MSG_SRVRVERSION;
	arapak->srvrversion.lowVersion[0] = SRVRVERSION1_LOW0;
	arapak->srvrversion.lowVersion[1] = SRVRVERSION1_LOW1;
	arapak->srvrversion.lowVersion[2] = SRVRVERSION1_LOW2;
	arapak->srvrversion.lowVersion[3] = SRVRVERSION1_LOW3;
	if (tty->arap_version != ARAP_VERSION2) {
	   /* for arap version 1.0, lowVersion and highVersion are the same */
	    arapak->srvrversion.highVersion[0] = SRVRVERSION1_LOW0;
	    arapak->srvrversion.highVersion[1] = SRVRVERSION1_LOW1;
	    arapak->srvrversion.highVersion[2] = SRVRVERSION1_LOW2;
	    arapak->srvrversion.highVersion[3] = SRVRVERSION1_LOW3;
	} else {
	    arapak->srvrversion.highVersion[0] = SRVRVERSION2_HIGH0;
	    arapak->srvrversion.highVersion[1] = SRVRVERSION2_HIGH1;
	    arapak->srvrversion.highVersion[2] = SRVRVERSION2_HIGH2;
	    arapak->srvrversion.highVersion[3] = SRVRVERSION2_HIGH3;
	}
	pak->datagramsize = 12;
	break;
    case MSG_AUTH_CHALLENGE:
	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
	    buginf("\nARAP TTY %t: --------- AUTH_CHALLENGE ---------",
		tty->ttynum); 
	arapak->auth_challenge.dataflag = ARAP_INTERNAL;
	arapak->auth_challenge.seqnum = seqnum;
	arapak->auth_challenge.cmdhighbyte = 0;
	arapak->auth_challenge.cmd = MSG_AUTH_CHALLENGE;
	arapak->auth_challenge.authType[0] = 0;
	arapak->auth_challenge.authType[1] = 0;
	arapak->auth_challenge.authType[2] = 0;
	arapak->auth_challenge.authType[3] = KAUTH_TWOWAYDES;
	bcopy(&arapptr->auth.myrandom.high, 
		arapak->auth_challenge.randomNumber1, ARAP_LONG_SIZE);
	bcopy(&arapptr->auth.myrandom.low, 
		arapak->auth_challenge.randomNumber2, ARAP_LONG_SIZE);
	if (tty->arap_version != ARAP_VERSION2) {
	    pak->datagramsize = 16;
	} else {
	    arapak->auth_challenge.accessFlags[0] = 0;
	    arapak->auth_challenge.accessFlags[1] = 0;
	    arapak->auth_challenge.accessFlags[2] = 0;
	    arapak->auth_challenge.accessFlags[3] = kMultiPortSrvr;
	    if (arapptr->tty->arap_mask & ARAP_MANUALPASSWORD)
	        arapak->auth_challenge.accessFlags[3] |= kManualPasswordEntry;
	    pak->datagramsize = 20;
	}
	break;
    case MSG_AUTH_RESPONSE:
	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
	    buginf("\nARAP TTY %t: --------- AUTH_RESPONSE ---------",
		tty->ttynum); 
	arapak->auth_response.dataflag = ARAP_INTERNAL;
	arapak->auth_response.seqnum = seqnum;
	arapak->auth_response.cmdhighbyte = 0;
	arapak->auth_response.cmd = MSG_AUTH_RESPONSE;
	bcopy(&arapptr->auth.resultcode,
		arapak->auth_response.result, ARAP_SHORT_SIZE);
	bcopy(&arapptr->auth.myresult.high, 
		arapak->auth_response.resultnumber1, ARAP_LONG_SIZE);
	bcopy(&arapptr->auth.myresult.low, 
		arapak->auth_response.resultnumber2, ARAP_LONG_SIZE);
	if (tty->arap_version != ARAP_VERSION2) {
	    pak->datagramsize = 14;
	} else {
	    arapak->auth_response.nextAction[0] = 0;
	    if (!arapptr->auth.resultcode) {
		arapak->auth_response.nextAction[1] = kContinueConnection;
	    } else 
	    if (arapptr->auth.resultcode == ERR_VLD8_MANUALPASSWORDREQUIRED) {
		arapak->auth_response.nextAction[1] = kRetryDES;
	    } else {
		arapak->auth_response.nextAction[1] = kAbortConnection;
	    }
	    pak->datagramsize = 16;
	}
	break;
    case MSG_CALLBACK:
 	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
	    buginf("\nARAP TTY %t: --------- CALLBACK ---------", tty->ttynum); 
	arapak->startinfofromserver.dataflag = ARAP_INTERNAL;
	arapak->startinfofromserver.seqnum = seqnum;
	arapak->startinfofromserver.cmdhighbyte = 0;
	arapak->startinfofromserver.cmd = MSG_CALLBACK;
        pak->datagramsize = 4;
	break;       
    case MSG_STARTINFOFROMSERVER:
	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
	    buginf("\nARAP TTY %t: --------- STARTINFOFROMSERVER----------",
		tty->ttynum); 
	arapak->startinfofromserver.dataflag = ARAP_INTERNAL;
	arapak->startinfofromserver.seqnum = seqnum;
	arapak->startinfofromserver.cmdhighbyte = 0;
	arapak->startinfofromserver.cmd = MSG_STARTINFOFROMSERVER;
	if (arapptr->smartbuffer) {
	    temp = OUR_ARAP_SENDBUFFERSIZE;
	    bcopy(&temp, arapak->startinfofromserver.SB_SendBufSize,
		    ARAP_LONG_SIZE);

	    temp = OUR_ARAP_RECVBUFFERSIZE;
	    bcopy(&temp, arapak->startinfofromserver.SB_RcvBufSize,
		    ARAP_LONG_SIZE);
	} else {
		/* we don't want to do smartbuffering. */
	    temp = 0;
	    bcopy(&temp, arapak->startinfofromserver.SB_SendBufSize,
		    ARAP_LONG_SIZE);
	    bcopy(&temp, arapak->startinfofromserver.SB_RcvBufSize,
		    ARAP_LONG_SIZE);
	}

	bcopy(&arap_net, arapak->startinfofromserver.RealNet, ARAP_SHORT_SIZE);
	arapak->startinfofromserver.RealNode[0] = 0;
	arapak->startinfofromserver.RealNode[1] = 254; /* arbitrarily high
							  node number */

	if (TIMER_RUNNING(arapptr->disconnecttime))
	    time = TIME_LEFT_SLEEPING(arapptr->disconnecttime) / ONESEC;
	else
	    time = UNLIMITED;
	bcopy(&time, arapak->startinfofromserver.maxConnectTimer, 
		    ARAP_LONG_SIZE);

	bcopy(&arap_net, arapak->startinfofromserver.AppearAsNet, 
			ARAP_SHORT_SIZE);
	arapak->startinfofromserver.AppearAsNode[0] = 0;
	arapak->startinfofromserver.AppearAsNode[1] = arapptr->tty->ttynum;

	/* the following are pascal strings */

	arapak->startinfofromserver.serversZone[0] = arap_zonelen;
	bcopy(arap_zone, &(arapak->startinfofromserver.serversZone[1]), 
	      arap_zonelen);
	arapak->startinfofromserver.serversName[0] = strlen(hostname) + 1;
	sstrncpy((char *) &(arapak->startinfofromserver.serversName[1]), 
		hostname, strlen(hostname) +1);
	if (tty->arap_version != ARAP_VERSION2) {
	    pak->datagramsize = 92;
	} else {
	    arapak->startinfofromserver.smartBufferVersionMin[0] = 0;
	    arapak->startinfofromserver.smartBufferVersionMin[1] = 0;
	    arapak->startinfofromserver.smartBufferVersionMin[2] = 0;
	    arapak->startinfofromserver.smartBufferVersionMin[3] = SMARTBUFFER_VERSION;
	    arapak->startinfofromserver.smartBufferVersionMax[0] = 0;
	    arapak->startinfofromserver.smartBufferVersionMax[1] = 0;
	    arapak->startinfofromserver.smartBufferVersionMax[2] = 0;
	    arapak->startinfofromserver.smartBufferVersionMax[3] = SMARTBUFFER_VERSION;
	    pak->datagramsize = 98;
	}
	break;
    case MSG_ZONELISTINFO:

	my_table = malloc(ZONETABLESIZE*sizeof(ziptabletype *));

	if ( my_table == NULL) {
	    return(0);
	}


	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
	    buginf("\nARAP TTY %t: --------- ZONELISTINFO ---------", 
		tty->ttynum); 
	arapak->zonelistinfo.dataflag = ARAP_INTERNAL;
	arapak->zonelistinfo.seqnum = seqnum;
	arapak->zonelistinfo.cmdhighbyte = 0;
	arapak->zonelistinfo.cmd = MSG_ZONELISTINFO;

	count = 0;
	data = &arapak->zonelistinfo.data;

	numzones = make_zone_array(my_table, arapptr->zonelist);
	for (i = arapptr->zonecount; i < numzones; i++) {
	    zone = my_table[i];
	    data[count] = zone->length;
	    bcopy(zone->name, &(data[count + 1]), zone->length);
	    count += zone->length + 1;
	    if (count > ARAP_MAXZONEPACKETSIZE) {
		res = 1;
		break;
	    }
	}
	arapptr->zonecount = i+1;


	if (res == 1) arapak->zonelistinfo.lastflag = FALSE;
	   else
		      arapak->zonelistinfo.lastflag = TRUE;

	pak->datagramsize = count + 5;

	free(my_table);

	break;
    case MSG_TICKLE:
	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
	    buginf("\nARAP TTY %t:  -------sending MSG_TICKLE-------", 
		tty->ttynum); 
	arapak->tickle.dataflag = ARAP_INTERNAL;
	arapak->tickle.seqnum = seqnum;
	arapak->tickle.cmdhighbyte = 0;
	arapak->tickle.cmd = MSG_TICKLE;
	bcopy(&arap_net, arapak->tickle.theNet, ARAP_SHORT_SIZE);
	if (TIMER_RUNNING(arapptr->disconnecttime))
 	    temp = TIME_LEFT_SLEEPING(arapptr->disconnecttime) / ONESEC;
	  else
  	    temp = UNLIMITED;
	bcopy(&temp, arapak->tickle.timeleft, ARAP_LONG_SIZE);
	pak->datagramsize = 10;
	break;
    case MSG_TIMELEFT:
	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
	    buginf("\nARAP TTY %t:  -----sending MSG_TIMELEFT-------",
		tty->ttynum); 
	arapak->timeleft.dataflag = ARAP_INTERNAL;
	arapak->timeleft.seqnum = seqnum;
	arapak->timeleft.cmdhighbyte = 0;
	arapak->timeleft.cmd = MSG_TIMELEFT;
	if (TIMER_RUNNING(arapptr->disconnecttime))
 	    temp = TIME_LEFT_SLEEPING(arapptr->disconnecttime) / ONESEC;
	  else
  	    temp = UNLIMITED;
	bcopy(&temp, arapak->timeleft.timeleft, ARAP_LONG_SIZE);
	pak->datagramsize = 8;
	break;
    case MSG_TEXT_ALERT:
	if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, tty->ttynum))
		buginf("\nARAP TTY %t:  -------sending MSG_TEXT_ALERT-------",
		    tty->ttynum); 
	arapak->text_alert.dataflag = ARAP_INTERNAL;
	arapak->text_alert.seqnum = seqnum;
	arapak->text_alert.cmdhighbyte = 0;
	arapak->text_alert.cmd = MSG_TEXT_ALERT;
	bcopy(arapptr->text, arapak->text_alert.string, arapptr->text[0]+1);
	pak->datagramsize = 4+arapptr->text[0]+1;
	break;
    default:
	errmsg(ARAP_ERR_BADPAK, arapptr->tty->ttynum);
	break;
    }

    /* send the packet and flush it out of the system */
    enqueue(&arapptr->ttyOUT, pak);
    arap_output(arapptr);
    v42 = v42_output(tty, &arapptr->v42OutQ, &arapptr->MNP4OutQ);
    if (v42 < 0) {
	arapptr->force_quit = TRUE;
	if (bad_exit_errors) {
	    errmsg(ARAP_ERR_BADEXIT, arapptr->tty->ttynum, 
		(ARAP_USER_P(arapptr)),
		"Force Quit pak v42bisflush");
	}
    }
    mnp4_output(arapptr->mnp4ptr, &arapptr->MNP4OutQ);

    return(res);
}

static int make_zone_array(ziptabletype *my_table[], int access_list)
{
    ziptabletype *zone;
    extern ziptabletype *zip_table[];
    int count = 0;
    int i;


    for (i = 0; i < ZIPHASHLEN; i++) {
	zone = zip_table[i];
        while (zone) {
            if ((atalk_firstNet(zone) == NULL) && (zone->ifcount == 0)) {
                zone = zone->next;
                continue;
            }
    	    if ((zone) && (atalk_zonecheck(zone, access_list))) {
	        my_table[count] = zone;
	        count++;
	    }
	    zone = zone->next;
        }
    }
    qsort(my_table, count, sizeof(ziptabletype *), zonecmp);
    return (count);
}

static int zonecmp(const void *z1, const void *z2)
{
    ziptabletype *zone1, *zone2, **zo1, **zo2;
    uchar a[32], b[32];
    zo1 = (ziptabletype **) z1;
    zo2 = (ziptabletype **) z2;
    zone1 = *zo1;
    zone2 = *zo2;
    bcopy(zone1->name, a, 32);
    bcopy(zone2->name, b, 32);
    return (strdiacasecmp(zone1->name, zone2->name));
}

/*
 * wait_for_remote -- wait for an expected message from the other end
 * 
 * This is called as part of the starting dance.  Right now there's no provision
 * for exiting if the message is never received (it should time out).   If
 * the message came in with an error code set, we return the code (we should
 * probably return the whole packet).
 */
static int wait_for_remote(araptype *arapptr, int message)
{
    arap_packet *arapak;
    paktype *pak = NULL;
    short err;
    int v42;
    tt_soc *tty;
    int ret = ARAP_SUCCESS;

    tty = arapptr->tty;
    while (TRUE) {		/* loop while waiting for the message */
	while (!(QUEUESIZE(&arapptr->ARAPInQ))) {
	    edisms((blockproc *)arapstartupBLOCK, (int) arapptr);
	    if (arapptr->force_quit || modemcheck(tty)) {
		if (bad_exit_errors) {
		    errmsg(ARAP_ERR_BADEXIT, tty->ttynum,
		    	(ARAP_USER_P(arapptr)), 
			"Carrier dropped during startup");
		}
		return (ARAP_FAILURE);
	    }

	    if (!mnp4_timers(arapptr->mnp4ptr)) {
		return (ARAP_FAILURE);
	    }

	    if(mnp4_input(arapptr->mnp4ptr, &arapptr->v42InQ) == FALSE)
		return (ARAP_FAILURE);
	    v42 =v42_input(tty, &arapptr->v42InQ, &arapptr->ARAPInQ);
	    if (v42 < 0) {
		return (ARAP_FAILURE);
	    }	    
	}

	pak = dequeue(&arapptr->ARAPInQ);

	/* strip modem tool header */

	pak->datagramstart += 2;
	pak->datagramsize -= 2;

	arapak = (arap_packet *) pak->datagramstart;

	if (message == MSG_AUTH_REQUEST) {
	    if ((arapptr->tty->arap_mask & ARAP_MANUALPASSWORD)
			&& arapptr->password_firsttime) {
	        arapptr->auth.resultcode = ERR_VLD8_MANUALPASSWORDREQUIRED;
		arapptr->password_firsttime = FALSE;
		break;
	    }
	    arapptr->auth.resultcode = 0;
	    err = arapak->auth_request.result[0];
	    err <<= 8;
	    err |= arapak->auth_request.result[1];
	    if ((ARAP_BUGINF(arapinternal_debug, arainttty_debug,
		    tty->ttynum)) && err) {
		buginf("\nARAP TTY %t: Rcv %s in %s message, aborting session",
		   tty->ttynum,arap_err_strings(err),arap_msg_strings(message));
		ret = ARAP_FAILURE;
		break;
	    }
	    if (arapsec(arapptr, arapak) < 0) {
		ret = ARAP_FAILURE;
		break;
	    }
	    if (old_access_control) {
		if (!tty->user)
		    setstring(&tty->user, arapptr->auth.user);
	    } else {
		if (!aaa_get_cred(tty->ustruct, NULL, NULL, NULL,
				  NULL, NULL)) {
		    char *rem;
		    aaa_free_user(&tty->ustruct);
		    rem = tty_get_remote_addr(tty);
		    aaa_create_user(&tty->ustruct, arapptr->auth.user,
				    NULL, NULL, rem, AAA_AUTHEN_TYPE_ARAP,
				    AAA_SVC_ARAP, PRIV_USER);
		}
	    }
	}

	if (pak->datagramstart[3] == message) {
	    break;
	} else if (message != MSG_ACK) {
	    errmsg(ARAP_ERR_INITCONFUSED, tty->ttynum,
			arap_msg_strings(message));
	}

	arap_datagram_done(pak, tty->ttynum);
	pak = NULL;
    }
    if (pak) {
	arap_datagram_done(pak, tty->ttynum);
    }
    return (ret);
}

/*
 * arapsec --  Do ARAP security 
 * 
 */
static int arapsec(araptype *arapptr, arap_packet *pak)
{
    int retval;

    if (pak->auth_request.guest[1] == 1) {	/* if guest is set */
	strcpy(arapptr->auth.user, "Guest");
    }

    if (old_access_control) {
	retval = arapsec_old(arapptr, pak);
    } else {
	retval = arapsec_new(arapptr, pak);
    }
    if (retval == 0)		/* if Success, do accounting */
	reg_invoke_aaa_acct_net(arapptr->tty->ustruct, NULL, NULL);
    return(retval);
}

static int arapsec_new(araptype *arapptr, arap_packet *pak)
{
    userstruct *ustruct;
    authenstruct astruct;
    boolean ret, new_ustruct;
    char tty_str[10], *rem, *tmpuser;
    tt_soc *atty;
    aaa_arap_auth_data auth_data;
    char *star_loc;

    atty = arapptr->tty;

    memset(&astruct, 0, sizeof(astruct));
    astruct.tty = atty;

    /*
     * First check to see if user is already authenticated (ie. to exec).
     * If so, then allow guest if "auth-guest" is set.
     * Otherwise only allow guest if "guest" is set.
     */
    if (aaa_get_cred(atty->ustruct, &tmpuser, NULL, NULL, NULL, NULL)) {
	if (pak->auth_request.guest[1] == 1) {	/* if guest login */
	    if (aaa_authen_first_method(AAA_SVC_ARAP, atty->arap_acc_list_name,
					AAA_ACC_METH_AUTHGUEST)) {
		strcpy(arapptr->auth.user, tmpuser);
		return(0);
	    } else {
		arapptr->auth.resultcode = ERR_VLD8_GUESTNOTALLOWED;
		return(-1);
	    }
	}
	ustruct = atty->ustruct;
	new_ustruct = FALSE;
    } else {
	sprintf(tty_str, "tty%d", atty->ttynum);
	if (pak->auth_request.guest[1] != 1) {	/* if guest is not set */
	    sstrncpy(arapptr->auth.user,
		     (char *)&pak->auth_request.userName[1],
		     pak->auth_request.userName[0] + 1);
	}
	rem = tty_get_remote_addr(atty);
	ret = aaa_create_user(&ustruct, arapptr->auth.user, NULL, tty_str, rem,
			      AAA_AUTHEN_TYPE_ARAP, AAA_SVC_ARAP, PRIV_USER);
	if (!ret) {
	    arapptr->auth.resultcode = ERR_CONNECTION_REFUSED;
	    return(-1);
	}
	if (pak->auth_request.guest[1] == 1) {	/* if guest is set */
	    if (aaa_authen_first_method(AAA_SVC_ARAP, atty->arap_acc_list_name,
					AAA_ACC_METH_GUEST)) {
		ustruct->acc_method = AAA_ACC_METH_GUEST;
		if (atty->ustruct)
		    aaa_free_user(&atty->ustruct);
		atty->ustruct = ustruct;
		return(0);
	    } else {
		arapptr->auth.resultcode = ERR_VLD8_GUESTNOTALLOWED;
		aaa_free_user(&ustruct);
		return(-1);
	    }
	}
	new_ustruct = TRUE;
    }

    ret = TRUE;

    /* If we are doing the "single-line" hack, do an ascii login for a
     * token card, and if it succeeds, fake the regular ARAP login
     * encryption.
     */

    star_loc = strchr(ustruct->user, TAC_SINGLELINE_CHAR);
    if ((atty->arap_actype & AAC_SINGLELINE) && star_loc) {
	*star_loc = '\0';
	ret = aaa_otp_login(NULL, atty, ustruct->user, star_loc+1, 
				    atty->arap_acc_list_name);

	if (ret) {
	    char fixed_pass[16];
	    des_state_type *des_state = NULL;

	    sstrncpy(fixed_pass, TAC_SINGLELINE_ARAP_PASS, sizeof(fixed_pass));
	    reg_invoke_des_pw_bitshift(fixed_pass);

	    reg_invoke_des_init(0, &des_state);
	    reg_invoke_des_setkey(des_state, fixed_pass);
	    reg_invoke_des_endes(des_state, (char *)&arapptr->auth.myrandom);
	    reg_invoke_des_done(des_state);
	    des_state = NULL;

	    /* Send this result back to the remote. */
	    astruct.server_data = malloc(8);
	    if (astruct.server_data) {
		bcopy(&arapptr->auth.myrandom, astruct.server_data, 8);
		astruct.server_data_len = 8;
	    } else {
		ret = FALSE;
	    }
	}
    } else {
	bcopy(&arapptr->auth.myrandom,         auth_data.myrandom,    8);
	bcopy(pak->auth_request.randomNumber1, auth_data.theirrandom, 8);
	bcopy(pak->auth_request.resultNumber1, auth_data.theirresult, 8);

	astruct.user_data = (char *)&auth_data;
	astruct.user_data_len = sizeof(auth_data);
	ret = aaa_start_login(ustruct, &astruct, atty->arap_acc_list_name,
			      AAA_AUTHEN_ACT_LOGIN, AAA_SVC_ARAP);
	if (ret && (astruct.status != AAA_AUTHEN_STATUS_PASS))
	    ret = FALSE;
    }
    
    astruct.user_data = NULL;

    if (ret && (!astruct.server_data || astruct.server_data_len != 8)) {
	if (aaa_authen_debug)
	    buginf("\nAAA/AUTHEN/ARAP: Received invalid response.");
	ret = FALSE;
    }

    if (!ret) {
	arapptr->auth.resultcode = ERR_VLD8_BADUSER;
	if (new_ustruct)
	    aaa_free_user(&ustruct);
	aaa_cleanup_login(ustruct, &astruct);
	return(-1);
    }

    /* Take the result returned by AAA and send it back to the remote. */
    bcopy(astruct.server_data, &arapptr->auth.myresult, 8);
    aaa_cleanup_login(ustruct, &astruct);

    if (new_ustruct) {
	if (atty->ustruct)
	    aaa_free_user(&atty->ustruct);
	atty->ustruct = ustruct;
    }

    /* Now we must do authorization */

    if (aaa_author_needed(AAA_AUTHOR_TYPE_NET, 0, NULL, atty)) {
	authorstruct author;
        int i, avail = 0;
        boolean ret = TRUE;
        char *attr, *val;
        enum AV_TYPE type;
        usernametype callback_info;

	ret = TRUE;
	memset(&author, 0, sizeof(authorstruct));
        memset(&callback_info, 0, sizeof(usernametype));
	author.auth_idb = atty->tty_idb;
	author.auth_tty = atty;

	/* First step.  Build outgoing A-V lists. */
	if (!aaa_add_AV(&author.out_args, &avail, AAA_ATTR_service,
			AAA_VAL_SVC_arap, AAA_AV_MAND) ||
	    !aaa_add_AV(&author.out_args, &avail, AAA_ATTR_protocol,
			AAA_VAL_PROT_atalk, AAA_AV_MAND)) 
	    ret = FALSE;

	if (ret) {
	    /* Second step.  Ask. */
	    ret = aaa_do_author(atty->ustruct, &author,
				AAA_AUTHOR_TYPE_NET, 0);

	    /* Third step.  Deal with response. */
	    switch(author.status) {
	    case AAA_AUTHOR_STATUS_ERROR:
	    case AAA_AUTHOR_STATUS_FAIL:
		ret = FALSE;
		break;
	    case AAA_AUTHOR_STATUS_PASS_ADD:
		if (!author.in_args)
		    break;
		aaa_merge_attrs(author.out_args, author.in_args,
				&author.in_args);
		/* FALLTHRU */
	    case AAA_AUTHOR_STATUS_PASS_REPL:
		if (!author.in_args)
		    break;
		for (i = 0; ret && author.in_args[i]; i++) {
		    if (aaa_author_debug) {
			buginf("\nAAA/AUTHOR/ARAP: Processing AV %s", 
			       author.in_args[i]);
		    }
		    if (aaa_split_AV(author.in_args[i], &attr, &val, &type)) {
			if (!strcmp(attr, AAA_ATTR_service)) {
			    if (strcmp(val, AAA_VAL_SVC_arap)) {
				if (type == AAA_AV_MAND) {
				    ret = FALSE;
				} else
				    if (aaa_author_debug)
					buginf("\nAAA/AUTHOR/ARAP: "
					       "tried to change \"service\".  "
					       "Ignoring.");
			    }
			    continue;
			}
			if (!strcmp(attr, AAA_ATTR_protocol)) {
			    if (strcmp(val, AAA_VAL_PROT_atalk)) {
				if (type == AAA_AV_MAND) {
				    ret = FALSE;
				} else
				    if (aaa_author_debug)
					buginf("\nAAA/AUTHOR/ARAP: "
					       "tried to change \"protocol\"."
					       "  Ignoring.");
			    }
			    continue;
			}
			if (!strcmp(attr, AAA_ATTR_acl)) {
			    int tmp_acl = atoi(val);
			    if ((tmp_acl < MINATALKACC) ||
				(tmp_acl > MAXATALKACC)) {
				if (aaa_author_debug)
				    buginf("\n%% AAA/AUTHOR/ARAP received "
					   "invalid access-list value %d. "
					   "(Should be %d - %d)", tmp_acl,
					   MINATALKACC, MAXATALKACC);
				if (type == AAA_AV_MAND)
				    ret = FALSE;
			    } else {
				arapptr->accesslist = tmp_acl;
			    }
			    continue;
			}
			if (!strcmp(attr, AAA_ATTR_zonelist)) {
			    int tmp_zone = atoi(val);
			    if ((tmp_zone < MINATALKACC) ||
				(tmp_zone > MAXATALKACC)) {
				if (aaa_author_debug)
				    buginf("\nAAA/AUTHOR/ARAP received "
					   "invalid zone-list value %d. "
					   "(Should be %d - %d)", tmp_zone,
					   MINATALKACC, MAXATALKACC);
				if (type == AAA_AV_MAND)
				    ret = FALSE;
			    } else {
				arapptr->zonelist = tmp_zone;
			    }
			    continue;
			}
			if (!strcmp(attr, AAA_ATTR_timeout)) {
			    int tmp_timeout = atoi(val);
			    if (tmp_timeout < 0) {
				if (aaa_author_debug)
				    buginf("\nAAA/AUTHOR/ARAP received "
					   "negative timeout value %d.", 
					   tmp_timeout);
				if (type == AAA_AV_MAND)
				    ret = FALSE;
			    } else {
				TIMER_START(arapptr->disconnecttime,
					    tmp_timeout*ONEMIN);
			    }
			    continue;
			}

 
                        if (!strcmp(attr, AAA_ATTR_callback)) {
 			    setstring(&callback_info.dial_string, val);
                            callback_info.flags |= USER_CALLBACK_DIALSTRING;
 			    continue;
 			}
                    
                        if (!strcmp(attr, AAA_ATTR_callback_noverify)) { 
                           if (atoi(val) == 1) 
 			     callback_info.flags |= USER_NOCALLBACK_VERIFY;
 			   continue;
 		        }
 
                        if (!strcmp(attr, AAA_ATTR_callback_rotary)) {
                             callback_info.callback_rotary_or_line = atoi(val);
 			    callback_info.flags |= USER_CALLBACK_ROTARY;
 			    continue;
 			}
 
                        if (!strcmp(attr, AAA_ATTR_callback_line)) {
                            callback_info.callback_rotary_or_line = atoi(val);
 			    callback_info.flags |= USER_CALLBACK_LINE;
 			    continue;
  			}

			/* if unknown and mandatory, then bail */
			if (type == AAA_AV_MAND) {
			    if (aaa_author_debug)
				buginf("\nAAA/AUTHOR/ARAP received unknown "
				       "mandatory attribute \"%s\"", attr);
			    ret = FALSE;
			}
		    }
		}
		break;
	    }
        }
        aaa_cleanup_author(&author);
        /* Fourth step.  Bail out if authorization failed. */
        if (!ret) {
	    if (aaa_author_debug) {
		buginf("\nAAA/AUTHOR/ARAP: denied");
	    }
	    arapptr->auth.resultcode = ERR_VLD8_BADUSER;
	    return(-1);
	}

        /*
         * Fifth and final step.
         * If the user is to do a callback, set up the resultcode to callback
         */ 
        if (arap_callback && 
            (callback_info.flags & USER_CALLBACK_DIALSTRING)) {
           if (ARAP_BUGINF(arapinternal_debug, arainttty_debug, atty->ttynum))
             buginf("\nARAP TTY %t: arapsec_new setting up callback", 
		atty->ttynum); 
           arapptr->info = duplicate_usernametype(&callback_info);
	}
    }
    return(0);
}

static int arapsec_old(araptype *arapptr, arap_packet *pak)
{
    uchar *ptr;
    int arap_did_tacacs = 0;
    des_state_type *des_state = NULL;
    tt_soc *tty = arapptr->tty;
    usernametype *local_user = NULL;

    /*
     * If it's a guest login then check the guest flags.
     * arapptr->auth.user has already been filled in with "Guest".
     */
    if (pak->auth_request.guest[1] == 1) {
	/*
	 * If the NOGUEST flag is set
	 *	If the IFNEED flag is not set then fail.
	 *	If the IFNEED FLAGS is set then
	 *		permit if user already logged in to exec
	 *		fail if they haven't logged in
	 * If NOGUEST is not set, then permit.
	 */
	if ((tty->arap_mask & ARAP_NOGUEST) &&
	    (!(tty->arap_mask & ARAP_NOGUEST_IFNEEDED) ||
	     !tty->user || !*tty->user)) {
	    arapptr->auth.resultcode = ERR_VLD8_GUESTNOTALLOWED;
	    return(-1);
	}
	return(0);
    }
    /* 
     * If user already authenticated at a login prompt and 
     * "arap_authen_once" is set, then don't authenticate again,
     * but fake it instead.
     */
    if (tty->user && *tty->user && arap_authen_once) {

	memset(arapptr->auth.password, 0, PASSWORDLEN);
	memset(arapptr->auth.user, 0, 20);

	ptr = &pak->auth_request.userName[1];

	pak->auth_request.userName[pak->auth_request.userName[0]+1] = 0;

	strcpy(arapptr->auth.user, (char *) ptr);
	
	sstrncpy(arapptr->auth.password, tty->saved_passwd,
		 sizeof(tty->saved_passwd));
	reg_invoke_des_pw_bitshift(arapptr->auth.password);
    } else {   
       /* user hasn't been authenticated.  Here we go.... */
	sstrncpy(arapptr->auth.user, (char *) &pak->auth_request.userName[1],
		 pak->auth_request.userName[0] + 1);

	bcopy(pak->auth_request.randomNumber1, &arapptr->auth.theirrandom, 8);

	bcopy(pak->auth_request.resultNumber1, &arapptr->auth.theirresult, 8);

        local_user = find_passwd(arapptr->auth.user, arapptr->auth.password);

	if ((tty->arap_actype & AAC_TACACS) 
		&& !(local_user && !(tty->arap_actype & AAC_SINGLELINE))) {
	    /*
	     * Use TACACS to DES crypt both randoms ... 
	     *  If it's a local user and single-line is being used then go
	     *  with this (the tacacs single-line code looks for local users.)
	     *  If it's a local user and single-line is *not* being used, 
	     *  then go with the local account routine.
	     *  
	     */
	    if (arapsec_tacacs_account(arapptr, pak) < 0)
		    return(-1);
	    arap_did_tacacs = 1;
	} else {			/* Using local account. */
	    if (arapsec_local_account(arapptr, local_user) < 0)
		    return(-1);
	}

	if ((arapptr->auth.myrandom.high != arapptr->auth.theirresult.high) ||
	    (arapptr->auth.myrandom.low !=  arapptr->auth.theirresult.low)) {
	    arapptr->auth.resultcode = ERR_VLD8_BADPASSWORD;
	    return (-1);
	}
    }

    if (!arap_did_tacacs) {
	reg_invoke_des_init(0, &des_state);
	reg_invoke_des_setkey(des_state, arapptr->auth.password);
	reg_invoke_des_endes(des_state, (char *) &arapptr->auth.theirrandom);
	reg_invoke_des_done(des_state);
	des_state = NULL;
    }

    bcopy(&arapptr->auth.theirrandom, &arapptr->auth.myresult, 8);
    return(0);
}

/*
 * arapsec_tacacs_account --  Do ARAP security using TACACS account
 * 
 */
static int arapsec_tacacs_account(araptype *arapptr, arap_packet *pak)
{
    int ok;
    char data[16], *pass, *user;
    bits64 *myrand    = (bits64 *)&data[0];
    bits64 *theirrand = (bits64 *)&data[8];
    tt_soc *tty;
    des_state_type *des_state = NULL;

    user = &(arapptr->auth.user[0]);
    tty = arapptr->tty;

    if (!tacacs_extended) {
	errmsg(ARAP_ERR_NOEXTTACACS, tty->ttynum);
	return(-1);
    }

    /*
     * Single-Line support takes the users password from the Username field.
     * It looks for a separator character and does a standard TACACS query.
     * Since ARAP does a two-way authentication, it then completes the
     * challenge-response by DES encrypting with a constant secret value.
     */
    if ((tty->arap_actype & AAC_SINGLELINE) &&
	((pass = strchr(user, TAC_SINGLELINE_CHAR)) != NULL)) {
	char fixed_pass[16];
	*pass = '\0';
	pass++;
	ok = reg_invoke_login_login(tty, user, pass, NULL);
	if (!ok) {
	    arapptr->auth.resultcode = ERR_VLD8_BADUSER;
	    return (-1);
	}

	sstrncpy(fixed_pass, TAC_SINGLELINE_ARAP_PASS, sizeof(fixed_pass));
	reg_invoke_des_pw_bitshift(fixed_pass);

	reg_invoke_des_init(0, &des_state);
	reg_invoke_des_setkey(des_state, fixed_pass);
	reg_invoke_des_endes(des_state, (char *)&arapptr->auth.myrandom);
	reg_invoke_des_done(des_state);
	des_state = NULL;

	reg_invoke_des_init(0, &des_state);
	reg_invoke_des_setkey(des_state, fixed_pass);
	reg_invoke_des_endes(des_state, (char *)&arapptr->auth.theirrandom);
	reg_invoke_des_done(des_state);
	des_state = NULL;
    } else {
	myrand->high	= arapptr->auth.myrandom.high;
	myrand->low	= arapptr->auth.myrandom.low;
	theirrand->high = arapptr->auth.theirrandom.high;
	theirrand->low  = arapptr->auth.theirrandom.low;

	ok = reg_invoke_login_authen(user, &data[0],
				     (sizeof(bits64) * 2),
				     (sizeof(bits64) * 2),
				     NULL, /* reason */
				     XTA_ARAP_AUTH, tty);
	if (!ok) {
	    /* Can I pass back the tacacs "reason"??????? */
	    arapptr->auth.resultcode = ERR_VLD8_BADUSER;
	    return (-1);
	}

	arapptr->auth.myrandom.high    = myrand->high;
	arapptr->auth.myrandom.low     = myrand->low;
	arapptr->auth.theirrandom.high = theirrand->high;
	arapptr->auth.theirrandom.low  = theirrand->low;
    }

    return(0);
}

/*
 * arapsec_local_account --  Do ARAP security using local account
 * 
 */
static int arapsec_local_account(araptype *arapptr, usernametype *info)
{
    des_state_type *des_state = NULL;

    if (info == NULL) {
	arapptr->auth.resultcode = ERR_VLD8_BADUSER;
	return (-1);
    }

    reg_invoke_des_pw_bitshift(arapptr->auth.password);

    reg_invoke_des_init(0, &des_state);
    reg_invoke_des_setkey(des_state, arapptr->auth.password);
    reg_invoke_des_endes(des_state, (char *) &arapptr->auth.myrandom);
    reg_invoke_des_done(des_state);
    des_state = NULL;

    /*
     * If the user is to do a callback (must be non-0 callback dialstring),
     * set up the resultcode to callback
     */ 
    if (arap_callback && info && info->dial_string && strlen(info->dial_string) > 0){
        if(ARAP_BUGINF(arapinternal_debug, arainttty_debug,
	    arapptr->tty->ttynum))
          buginf("\nARAP TTY %t: arapsec_local_account setting up callback",
		arapptr->tty->ttynum); 
        arapptr->info = duplicate_usernametype(info);
    }
    return(0);
}

/*
 * arap_output --  Handle outgoing ARAP packets
 * 
 * Take packets off of the ttyOUT, tack a modem tool length on the front, and
 * pass them to v42bis
 */
static void arap_output(araptype *arapptr)
{
    paktype *pak, *pak2;
    int temp;
    ushort net;

    while ((pak = dequeue(&arapptr->ttyOUT))) {
	arap_tot_packout++;
	arapptr->at_out_pckts++;
	if (arapptr->accesslist) {
	    /* set net to be the DDP source net */
	    net = pak->datagramstart[ARAPAK_HDRLEN + DDP_SRCNETHIGH];
	    net <<= 8;
	    net |= pak->datagramstart[ARAPAK_HDRLEN + DDP_SRCNETLOW];
	    if (!atalk_accesscheck (net, arapptr->accesslist, FALSE)) {
		    arap_datagram_done(pak, arapptr->tty->ttynum);
		    continue;
	    }
	}
	if (arapptr->smartbuffer && 
		(pak->datagramstart[ARAPAK_DATAFLAG] == ARAP_DATA)) {
	    pak2 = arap_getbuffer(BIGDATA, arapptr->tty->ttynum);
	    if (pak2 == NULL) {
		datagram_done(pak);
		break;
	    }
	    pak->datagramstart++;
	    pak->datagramsize--;

	    smartbuffer_encode(&(arapptr->smartbuf_conf), 
		    arapptr->tty, pak->datagramstart, pak->datagramsize, 
		    pak2->datagramstart, &temp);

	    pak2->datagramsize = temp;
	    arap_datagram_done(pak, arapptr->tty->ttynum);
	    pak = pak2;
	}

	pak->datagramstart -= MT_HEADERLEN;
	pak->datagramstart[0] = pak->datagramsize >> 8;
	pak->datagramstart[1] = pak->datagramsize;
	pak->datagramsize += MT_HEADERLEN;
        enqueue(&arapptr->v42OutQ, pak);
    }
}

/*
 * arapstartupBLOCK -- block on MNP4InQ, and a "clear line"
 */
static boolean arapstartupBLOCK(int *arg)
{
    araptype *arapptr = (araptype *) arg;

    if (QUEUESIZE(&arapptr->mnp4ptr->MNP4InQ) || 
       QUEUESIZE(&arapptr->mnp4ptr->recv_ackQ))
	return (FALSE);

    if (TIMER_RUNNING_AND_AWAKE(arapptr->mnp4ptr->timer_resend)) {
	return (FALSE);
    }
 
    if (TIMER_RUNNING_AND_AWAKE(arapptr->mnp4ptr->timer_ack_delay)) {
	return (FALSE);
    }

    if (arapptr->tty->statbits & CARDROP) {
	arapptr->force_quit = TRUE;
	if (bad_exit_errors) {
	    errmsg(ARAP_ERR_BADEXIT, arapptr->tty->ttynum, 
		(ARAP_USER_P(arapptr)),
	       "Carrier dropped during startup");
	}
	return (FALSE);
    }
    return (TRUE);
}

/*
 * arapBLOCK -- block on MNP4InQ, ttyOUT, and a "clear line"
 */

static boolean arapBLOCK(int *arg)
{
    araptype *arapptr = (araptype *) arg;
    mnp4type *mnp4 = arapptr->mnp4ptr;

    if (modemcheck(arapptr->tty)) 
        return(FALSE);

    if (QUEUESIZE(&arapptr->mnp4ptr->MNP4InQ) || 
       QUEUESIZE(&arapptr->mnp4ptr->recv_ackQ))
	return (FALSE);

    if (QUEUESIZE(&arapptr->ttyOUT))
	return (FALSE);

    if (arapptr->tty->statbits & CARDROP) {
        arapptr->force_quit = TRUE;
        return (FALSE);
    }

    if (TIMER_RUNNING_AND_AWAKE(mnp4->timer_resend)) {
        return (FALSE);
    }
 
    if (TIMER_RUNNING_AND_AWAKE(mnp4->timer_ack_delay)) {
        return (FALSE);
    }

    if (TIMER_RUNNING_AND_AWAKE(arapptr->tickletime))
        return(FALSE);

    return (TRUE);
}

/*
 *
 * destroy_arapptr
 *
 */

static void destroy_arapptr(araptype *arapptr)
{
    paktype *pak;

    /*
     * Free up queues
     */
    while ((pak = p_dequeue(&arapptr->ARAPInQ))) {
	arap_datagram_done(pak, arapptr->tty->ttynum);
    }
    while ((pak = p_dequeue(&arapptr->v42InQ))) {
	arap_datagram_done(pak, arapptr->tty->ttynum);
    }
    while ((pak = p_dequeue(&arapptr->v42OutQ))) {
	arap_datagram_done(pak, arapptr->tty->ttynum);
    }
    while ((pak = p_dequeue(&arapptr->MNP4OutQ))) {
	arap_datagram_done(pak, arapptr->tty->ttynum);
    }
    while ((pak = p_dequeue(&arapptr->ttyOUT))) {
	arap_datagram_done(pak, arapptr->tty->ttynum);
    }

    if (arapptr->smartbuffer) {
       smartbuffer_destroy_ring(arapptr->smartbuf_input);
       smartbuffer_destroy_ring(arapptr->smartbuf_output);
    }
    free(arapptr);
}

/*
 * init_arapptr -- initialize an ARAP top level pointer
 * 
 * This initializes all the queues, and sets the initial states for mnp4 and
 * v42bis.  This should also make a decision whether to come up or not (based
 * on memory or load), but it doesn't right now.
 */
static araptype *init_arapptr(tt_soc *tty)
{
    araptype *arapptr;

    arapptr = malloc(sizeof(araptype));

    if (arapptr == NULL) return(NULL);

    /*
     * init all of the queues (except for MNP4InQ, which is part of the
     * tt_soc data structure)
     */
    queue_init(&arapptr->ARAPInQ, 0);
    queue_init(&arapptr->v42InQ, 0);
    queue_init(&arapptr->v42OutQ, 0);
    queue_init(&arapptr->MNP4OutQ, 0);
    queue_init(&arapptr->ttyOUT, 0);

    /* set up mnp4 and v42bis */
    arapptr->mnp4ptr = mnp4_setup(tty);
    if (arapptr->mnp4ptr == NULL) {
	free(arapptr);
	return(NULL);
    }
    tty->mnp4ptr = arapptr->mnp4ptr;
    arapptr->v42ptr = init_v42();
    if (arapptr->v42ptr == NULL) {
	mnp4_destroy_setup(arapptr->mnp4ptr);
	free(arapptr);
	return(NULL);
    }

    /* various and sundry */
    arapptr->packet_ready = FALSE;
    arapptr->force_quit = FALSE;
    arapptr->arap_started = FALSE;
    arapptr->password_firsttime = TRUE;
    arapptr->arap_seqnum = 0;
    arapptr->arap_acked = 0;
    arapptr->tty = tty;
    arapptr->smartbuffer = !(tty->arap_mask & ARAP_NOSMARTBUFFER);

    /* set a pseudo-random number -- we're just trying to avoid playback
	attacks here */
    arapptr->auth.myrandom.high = random_gen() << 16;
    arapptr->auth.myrandom.high |= random_gen();
    arapptr->auth.myrandom.low = random_gen() << 16;
    arapptr->auth.myrandom.low |= random_gen();
    arapptr->auth.myresult.high = 0;
    arapptr->auth.myresult.low = 0;

    arapptr->zonecount = 0;

    if (tty->arap_timelimit) {
        TIMER_START(arapptr->disconnecttime, tty->arap_timelimit);
    } else {
	TIMER_STOP(arapptr->disconnecttime);
    }

    GET_TIMESTAMP(arapptr->timestarted);  /* set initially.  We reset this
                                             after ARAP session is fully
                                             started */
    if (TIMER_RUNNING(arapptr->disconnecttime)) {
          TIMER_START(arapptr->warningtime, 
			tty->arap_timelimit-tty->arap_warningtime);
    } else {
          TIMER_STOP(arapptr->warningtime);
    }

    TIMER_STOP(arapptr->tickletime);
    arapptr->ticklecount = 0;

    if (arapptr->smartbuffer) {
	if (smartbuffer_new_ring(&(arapptr->smartbuf_conf), 
		&(arapptr->smartbuf_input), OUR_ARAP_RECVBUFFERSIZE)) {
	    /* smartbuffer_new_ring failed */
	    destroy_v42(tty);
	    mnp4_destroy_setup(arapptr->mnp4ptr);
	    free(arapptr);
	    return(NULL);
	}

	if (smartbuffer_new_ring(&(arapptr->smartbuf_conf), 
		&(arapptr->smartbuf_output), OUR_ARAP_SENDBUFFERSIZE)) {
	    /* smartbuffer_new_ring failed */
	    smartbuffer_destroy_ring(arapptr->smartbuf_input);
	    destroy_v42(tty);
	    mnp4_destroy_setup(arapptr->mnp4ptr);
	    free(arapptr);
	    return(NULL);
	}
    }

    arapptr->accesslist = arapptr->tty->arap_netlist;
    arapptr->zonelist = arapptr->tty->arap_zonelist;

    /* change the line input and output routines to our own */
    if (!tty_servicehook(tty, mnp4_sndchar, mnp4_gotchar)) {
        if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, tty->ttynum))
            buginf("\nMNP4 TTY %t: Unable to set service hook", tty->ttynum);
        destroy_v42(tty);
        mnp4_destroy_setup(arapptr->mnp4ptr);
        free(arapptr);
        return (NULL);
    }

    return (arapptr);
}



/*
 * arap_msg_strings  --  convert message numbers to strings
 */
static char *arap_msg_strings(int msg)
{
    switch (msg) {
    case MSG_ACK:
	return ("MSG_ACK");
	break;
    case MSG_SRVRVERSION:
	return ("MSG_SRVRVERSION");
	break;
    case MSG_RMTVERSION:
	return ("MSG_RMTVERSION");
	break;
    case MSG_AUTH_CHALLENGE:
	return ("MSG_AUTH_CHALLENGE");
	break;
    case MSG_AUTH_REQUEST:
	return ("MSG_AUTH_REQUEST");
	break;
    case MSG_AUTH_RESPONSE:
	return ("MSG_AUTH_RESPONSE");
	break;
    case MSG_STARTINFOFROMSERVER:
	return ("MSG_STARTINFOFROMSERVER");
	break;
    case MSG_STARTINFOFROMREMOTE:
	return ("MSG_STARTINFOFROMREMOTE");
	break;
    case MSG_ZONELISTINFO:
	return ("MSG_ZONELISTINFO");
	break;
    case MSG_TICKLE:
	return ("MSG_TICKLE");
	break;
    case MSG_TIMELEFT:
	return ("MSG_TIMELEFT");
	break;
    case MSG_TIMERCANCELLED:
	return ("MSG_TIMERCANCELLED");
	break;
    default:
	return ("UNKNOWN");
	break;
    }
}

/*
 * arap_err_strings -- convert error numbers to strings
 */
static char *arap_err_strings(int err)
{
    switch (err) {
    case ERR_VLD8_CALLBACK:
	return ("ERR_VLD8_CALLBACK");
	break;
    case ERR_VLD8_BADVERSION:
	return ("ERR_VLD8_BADVERSION");
	break;
    case ERR_VLD8_BADUSER:
	return ("ERR_VLD8_BADUSER");
	break;
    case ERR_VLD8_BADPASSWORD:
	return ("ERR_VLD8_BADPASSWORD");
	break;
    case ERR_VLD8_BADLINK:
	return ("ERR_VLD8_BADLINK");
	break;
    case ERR_VLD8_NOCALLBACKALLOWED:
	return ("ERR_VLD8_NOCALLBACKALLOWED");
	break;
    case ERR_VLD8_ALLCBSERVERBUSY:
	return ("ERR_VLD8_ALLCBSERVERBUSY");
	break;
    case ERR_VLD8_GUESTNOTALLOWED:
	return ("ERR_VLD8_GUESTNOTALLOWED");
	break;
    case ERR_VLD8_SERVERISIMPOSTER:
	return ("ERR_VLD8_SERVERISIMPOSTER");
	break;
    case ERR_VLD8_LOGINNOTENABLED:
	return ("ERR_VLD8_LOGINNOTENABLED");
	break;
    default:
	return ("UNKNOWN");
	break;

    }
}
