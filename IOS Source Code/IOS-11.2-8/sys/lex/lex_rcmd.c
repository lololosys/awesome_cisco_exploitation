/* $Id: lex_rcmd.c,v 3.5.20.8 1996/08/28 13:00:04 thille Exp $
 * $Source: /release/112/cvs/Xsys/lex/lex_rcmd.c,v $
 *------------------------------------------------------------------
 * lex_rcmd.c - LEX remote command handling
 *
 * Feb. 1994, David Coli
 *
 * Copyright (c) 1994-1997 by Cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lex_rcmd.c,v $
 * Revision 3.5.20.8  1996/08/28  13:00:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.20.7  1996/08/14  02:36:22  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.5.20.6  1996/08/07  09:01:32  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.20.5  1996/08/03  23:33:21  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.5.20.4  1996/06/24  09:21:33  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.5.20.3  1996/05/17  11:26:55  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.6.3  1996/05/05  23:32:33  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.5.6.2  1996/04/26  07:56:30  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.6.1  1996/04/03  20:01:13  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.20.2  1996/04/29  09:39:35  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.5.20.1  1996/03/18  20:43:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.3  1996/03/07  10:06:11  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.2  1996/02/20  01:09:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/01  06:07:06  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4.2.1  1996/01/24  22:35:43  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/20  00:19:57  dcoli
 * CSCdi32142:  LEX rcmd length check incorrect in some cases
 * Fix length check for LEX_RCMD_NAK and LEX_RCMD_REJ packets.
 *
 * Revision 3.3  1996/01/05  10:17:57  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.2  1995/11/17  17:43:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:41:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef toupper
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "../dev/flash_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"     /* needed for PARSEBUF and other definitions */
#include "../if/network.h"
#include "access.h"
#include "../if/priority_private.h"
#include "ieee.h"
#include "../os/old_timer_callbacks.h"
#include "../wan/ppp.h" /* needed for PPP_ENCAPBYTES definition */
#include "file.h"       /* needed for FILE_ACCESS_TFTP */
#include "ttysrv.h"     /* needed for psion, psioff prototype */
#include "../ui/parse_util.h" /* needed for get_host_address, etc */
#include "../parser/parser_defs_priority.h"
#include "../parser/parser_defs_flash.h"
#include "lex_rcmd.h"
#include "lex.h"
#include "parser_defs_lex.h"
#include "../wan/ncp.h"
#include "../if/msg_lineproto.c"	/* Not a typo, see logger.h */
#include "../lex/msg_lex.c"		/* Not a typo, see logger.h */
#include "../wan/ppp_registry.h"

/* 
 * Forward references
 */
static boolean lex_check_rcmd_length(paktype *, lex_command *, lex_rcmdst_type *);
static void lex_retrans_hndlr(lex_rcmdst_type *);
static void lex_setup_and_send_rcmd(uchar, idbtype *);
static void lex_rcmd_throttle_send(void);
static void lex_update_serial_stats(lex_serial_counters_t *, 
 				    lex_serial_counters_t *);
static void lex_update_ether_stats(lex_ether_counters_t *, 
				    lex_ether_counters_t *);
static void lex_process_ack(paktype *, lex_rcmdst_type *);
static void lex_process_nak_rej(uchar, paktype *, lex_rcmdst_type *);
static void lex_find_addr_filter_low_high(int *, int *);
static void lex_find_type_filter_low_high(int *, int *);
static void lex_check_addr_filter_changed(int, int, int *, int *, idbtype *);
static void lex_check_type_filter_changed(int, int, int *, int *, idbtype *);
static void lex_priority_list_changed(ulong, ushort);
static void lex_check_priority_list_changed(ulong list);
static boolean lex_encap_rcmd_packet(hwidbtype *, paktype *pak, int length);


/*
 * Globals
 */
static addrtype lex_hostip;      /* TFTP server address */
boolean lex_rcmd_debug;
static uchar LEX_cmd_id = 0;     /* Used to identify commands with responses */
queuetype LEXrcmdQ;              /* Contains lex_rcmdst_type structures */
queuetype LEXinputQ;             /* contains packet pointers to remote command */
                                 /* responses from the FLEX device.         */
static queuetype LEX_rcmd_waitQ; /* Used to throttle sending of config cmds */

static int lex_type_filter_low = MAXTYPEACC;
static int lex_type_filter_high = MINTYPEACC;
static int lex_addr_filter_low = MAXMACADDRACC;
static int lex_addr_filter_high = MINMACADDRACC;
static boolean priority_callback_exists = FALSE;


static char *const lex_commands[] = {
    "\"lex input-type-list\"",
    "\"lex input-address-list\"",
    "\"lex priority-group\"",
    "\"shutdown\"",
    "\"no shutdown\"",
    "\"clear controller lex\"",
    "\"show interface lex\"",
    "\"copy (wrq)\"",
    "\"copy (data)\"",
    "\"copy (status)\"",
    "\"inventory\"",
    "\"status\"",
};

const char lex_not_bound[] = "is not bound to a serial interface";

/*
 * remote_ether_lineprotocol(pak)
 * process the remote ethernet's line protocol status
 * and send acknowledgement to the remote lex.
 * if the status has changed since last known,
 * send the usual cisco LINEPROTO message to the console & logger.
 */
static void get_remote_lex_status (paktype *pak)
{
    boolean lineproto_is_up;   /* current lineprotocol value received from the 1000 */
    lex_unsolicited_t *lex_unsolicited_packet;
    lex_idb_info_t *info = pak->if_input->lex_swidb->lex_info;
    lex_command *lex_cmd = NULL;
    idbtype   *serial_swidb;
    hwidbtype *serial_hwidb;
    char interface_name[MAX_LEX_IFDESCR_LENGTH];
    
    lex_unsolicited_packet = (lex_unsolicited_t *)(sizeof(struct lex_hdr) + 
			       sizeof(struct lex_option) +
			       pak->network_start);
    serial_swidb = pak->if_input;       /* idb of the serial line */
    serial_hwidb = serial_swidb->hwptr;

    lineproto_is_up = (lex_unsolicited_packet->status & 
		       LEXSTATUS_ETHER_LINEPROTOCOL);

    if ((lineproto_is_up && 
	(info->last_remote_ether_lineproto != LEXETHER_LINEPROTOCOL_UP)) ||
	(!lineproto_is_up && 
	(info->last_remote_ether_lineproto != LEXETHER_LINEPROTOCOL_DOWN))) {
	sprintf(interface_name,"%s-ethernet",
		pak->if_input->lex_swidb->namestring);
	errmsg(&msgsym(UPDOWN, LINEPROTO), interface_name,
	       (lineproto_is_up) ? "up" : "down");
    }
    if ( lineproto_is_up ) 
      info->last_remote_ether_lineproto = LEXETHER_LINEPROTOCOL_UP;
    else
      info->last_remote_ether_lineproto = LEXETHER_LINEPROTOCOL_DOWN;

    pak->if_output = pak->if_input;
    pak->linktype = LINK_LEX_RCMD;

    lex_cmd = (lex_command *)lex_rcmd_headstart(pak);
    lex_cmd->hdr.code = LEX_RCMD_UNSOLICITED;

    if (lex_encap_rcmd_packet(serial_hwidb, pak, lex_cmd->hdr.plen))
      datagram_out(pak); 
    else
      datagram_done(pak);
}



/*
 * lex_rcmd_enqueue
 * TYPE_PPP_LEXCP is used to carry both the LEX ncp messages and
 * LEX remote command messages.  However, we have already stripped
 * off the LEX NCP packets in ppp_getlink().
 */

static void lex_rcmd_enqueue (paktype *pak)
{
    p_enqueue(&LEXinputQ, pak);
}

void lex_rcmd_init (void)
{
    queue_init(&LEXrcmdQ, 0);
    queue_init(&LEXinputQ, 0);
    queue_init(&LEX_rcmd_waitQ, 0);
    lex_hostip.type = ADDR_IP;
    lex_hostip.length = ADDRLEN_IP;
    lex_hostip.ip_addr = -1L;
    reg_add_raw_enqueue (LINK_LEX_RCMD, lex_rcmd_enqueue, "lex_rcmd_enqueue");
}

/*
 * lex_rcmd_cleanup
 * Once an rcmd structure (lex_rcmdst_type) has been successfully created
 * in lex_setup_rcmd, this is the only routine that frees up the resources
 * associated with the rcmd.  Following are the scenarios in which 
 * lex_rcmd_cleanup is called:
 * 1) Commands which block from the parser ("show interface" and "copy"),
 *    call lex_rcmd_cleanup themselves.  The rcmd SHOULD NEVER BE DELETED
 *    BY ANYONE BUT THE WAITING PROCESS, OR THAT PROCESS WILL HANG!
 * 2) In lex_retrans_hndlr, if the retry count has been exceeded, and no
 *    process is waiting on the rcmd,.
 * 3) In lex_retrans_hndlr, if the rcmd has been marked for deletion by
 *    lex_unbind (rcmd->delete_rcmd == TRUE).  This is only set for
 *    commands which do not have a process waiting on the rcmd status.
 * 4) In lex_process_ack, if a match is found on command-id and sw-idb,
 *    and the rcmd does not have a process waiting on it.
 * 5) In lex_setup_and_send_rcmd, if no access list or priority group
 *    exists, or an invalid command code is passed in.
 * 6) In lex_send_rcmd, if the lex interface is unbound, and the rcmd
 *    has been marked for deletion (rcmd->delete_rcmd == TRUE) by lex_unbind.
 */

void lex_rcmd_cleanup (lex_rcmdst_type *rcmd)
{
    destroy_timer(rcmd->retrans_tmr);
    datagram_done(rcmd->pak);
    p_unqueue(&LEXrcmdQ, rcmd);
    free(rcmd);
    lex_rcmd_throttle_send();

}

/*
 * lex_rcmd_check_id
 * Search LEXrcmdQ for an entry whose command-id field matches that of the
 * incoming packet.  The serial interface must still be bound to the same
 * lex interface that originated the command in the first place.
 * If we can't match a response to an outstanding command, NULL is returned
 * and the caller should ditch the packet.
 */

static lex_rcmdst_type *lex_rcmd_check_id (idbtype *swidb, uchar id)
{
    lex_rcmdst_type *rcmd;

    for (rcmd = (lex_rcmdst_type *)LEXrcmdQ.qhead; rcmd; rcmd = rcmd->next) {
        if ((id == rcmd->cmd_id) && (rcmd->swidb == swidb))
            return(rcmd);
    }
    return(NULL);
}

/*
 * lex_rcmd_input
 * This process is forked at Lex subsystem initialization time if at least
 * one Lex interface was parsed in NVRAM (num_lex_interfaces > 0).  If none 
 * exist at this time, the lex_rcmd_input process will be forked at the time 
 * that the first Lex interface is configured.
 *
 * This process dequeues incoming packets from LEXinputQ.  If the serial
 * interface from which the packet arrived is not bound to a Lex interface
 * (pak->if_input->lex_swidb == NULL) the packet should be disposed of since
 * the intended recipient cannot receive the packet anyway because it is in
 * an incorrect state.  Calls lex_rcmd_check_id to match the response
 * with the original command and calls lex_check_rcmd_length to check the
 * command length for correctness. Calls lex_process_ack to handle the
 * response and remove the corresponding lex_rcmdst_type element from the 
 * LEXrcmdQ. Finally, datagram_done is called to return the packet to the 
 * buffer pool.
 */

void lex_rcmd_input (void)
{
    paktype *pak;
    lex_command *lex_cmd_rcvd = NULL;
    int pak_count;
    lex_rcmdst_type *lex_orig_rcmd = NULL;

    while (TRUE) {
        edisms(queueBLOCK, (ulong)&LEXinputQ);
        pak_count = PAK_SWITCHCOUNT;
        while (TRUE) {
            /*
             * Only process a limited number per pass.
             */
            if (pak_count-- == 0) {
                break;
            }

            pak = p_dequeue(&LEXinputQ);
            if ( !pak )
                break;

            if (pak->if_input->lex_swidb == NULL) {
                if (lex_rcmd_debug)
                    buginf("\nLEX-RCMD: %s command received on unbound serial interface- %s", 
			   lex_print_rcmd(lex_cmd_rcvd->opt.type),
			   pak->if_input->namestring);
                datagram_done(pak);
                continue;
            }

            lex_cmd_rcvd = (lex_command *)lex_rcmd_headstart(pak);

	    if (lex_rcmd_debug) {
		buginf("\nLEX-RCMD: %s : %s command received",
		       pak->if_input->lex_swidb->namestring,
		       lex_print_rcmd(lex_cmd_rcvd->opt.type));

		if ((lex_cmd_rcvd->opt.type != LEX_DLOAD_DATA) &&
		    (lex_cmd_rcvd->opt.type != LEX_STATS))
		    lexdump(pak->datagramstart, pak->datagramsize, "\nRcvd rcmd:");
		else
		    lexdump(pak->datagramstart, 
			    pak->if_input->hwptr->encsize + LEX_RCMD_HDR_LEN +
			    LEX_BLOCK_NUM_SIZE, "\nRcvd rcmd:");
	    }

	    if ( lex_cmd_rcvd->hdr.code == LEX_RCMD_UNSOLICITED ) 
	      get_remote_lex_status(pak);
	    else {
		lex_orig_rcmd = lex_rcmd_check_id(pak->if_input->lex_swidb,
						  lex_cmd_rcvd->hdr.id);
		if (lex_orig_rcmd == NULL) {
		    if (lex_rcmd_debug)
		      buginf("\nLEX-RCMD: ACK or response received on %s without a corresponding ID",
			     pak->if_input->namestring);
		    datagram_done(pak);
		    continue;
		}

		stop_timer(lex_orig_rcmd->retrans_tmr);

		if (!lex_check_rcmd_length(pak, lex_cmd_rcvd, lex_orig_rcmd)) {
		    datagram_done(pak);
		    continue;
		}

		switch (lex_cmd_rcvd->hdr.code) {
		  case LEX_RCMD_ACK:
		    lex_process_ack(pak, lex_orig_rcmd);
                    break;
		  case LEX_RCMD_NAK: /* Handle NAK and REJ the same way */
		  case LEX_RCMD_REJ:
		    lex_process_nak_rej(lex_cmd_rcvd->hdr.code, pak, lex_orig_rcmd);
                    break;
		  default:
                    /*
                     * This should not happen.
                     */
		    buginf("\nLEX-RCMD: illegal CODE field received in header: %d",
			   lex_cmd_rcvd->hdr.code);
                    break;
		}

		datagram_done(pak);
		lex_rcmd_throttle_send();
	    }
	}
    }
}

/*
 * lex_check_rcmd_length
 * Check for correct header length and option length fields on an incoming
 * Lex rcmd.  Returns FALSE if the length is incorrect.
 */

static boolean lex_check_rcmd_length (paktype *pak, lex_command *lex_cmd_rcvd,
				      lex_rcmdst_type *rcmd)
{
    boolean length_ok = TRUE;
    lex_command *lex_sent_cmd;

    /*
     * If LEX_RCMD_NAK or LEX_RCMD_REJ returned, then header and option
     * lengths of received packets should be the same as the original
     * packet being NAK'd or REJected.  The only exception is the case of 
     * LEX_DLOAD_DATA, where the flex box never returns the actual download 
     * data.
     */
    if (((lex_cmd_rcvd->hdr.code == LEX_RCMD_NAK) ||
        (lex_cmd_rcvd->hdr.code == LEX_RCMD_REJ)) &&
        (lex_cmd_rcvd->opt.type != LEX_DLOAD_DATA)) {
	lex_sent_cmd = (lex_command *)lex_rcmd_headstart(rcmd->pak);
	if ((lex_cmd_rcvd->hdr.plen != lex_sent_cmd->hdr.plen) ||
	    (lex_cmd_rcvd->opt.length != lex_sent_cmd->opt.length))
            length_ok = FALSE;
    }
    else /* we received a LEX_RCMD_ACK or a LEX_DLOAD_DATA */
    switch (lex_cmd_rcvd->opt.type) {
        case LEX_STATS:
	    if ((lex_cmd_rcvd->opt.flags & LEX_STATS_BOTH_INTS) == LEX_STATS_BOTH_INTS) {
		/*
		 * here we do some arithmetic on pointers to things
		 * inside the lex counters structures.  is there a
		 * way to do this at runtime without actually declaring
		 * some physical space for the structures? 
		 */
                if ((lex_cmd_rcvd->hdr.plen != LEX_RCMD_HDR_LEN + 
		     sizeof(lex_serial_counters_t) +
		     sizeof(lex_ether_counters_t)) ||
		    (lex_cmd_rcvd->opt.length != LEX_RCMD_OPTHDR_LEN + 
		     sizeof(lex_serial_counters_t) +
		     sizeof(lex_ether_counters_t)) )
                    length_ok = FALSE;
	    }
	    else if (lex_cmd_rcvd->opt.flags & LEX_SERIAL_STATS) {
                if ((lex_cmd_rcvd->hdr.plen != 
		     LEX_RCMD_HDR_LEN + sizeof(lex_serial_counters_t)) ||
		    (lex_cmd_rcvd->opt.length != 
		     LEX_RCMD_OPTHDR_LEN + sizeof(lex_serial_counters_t)))
                    length_ok = FALSE;
	    }
	    else if (lex_cmd_rcvd->opt.flags & LEX_ETHER_STATS) {
                if ((lex_cmd_rcvd->hdr.plen != 
		     LEX_RCMD_HDR_LEN + sizeof(lex_ether_counters_t)) ||
		    (lex_cmd_rcvd->opt.length != 
		     LEX_RCMD_OPTHDR_LEN + sizeof(lex_ether_counters_t)))
                    length_ok = FALSE;
	    }
	    else {
		lex_sent_cmd = (lex_command *)lex_rcmd_headstart(rcmd->pak);
		if ((lex_cmd_rcvd->hdr.plen != lex_sent_cmd->hdr.plen) ||
		    (lex_cmd_rcvd->opt.length != lex_sent_cmd->opt.length))
		    length_ok = FALSE;
	    }
 	    break;

        case LEX_INVENTORY:
            if ((lex_cmd_rcvd->hdr.plen != LEX_RCMD_HDR_LEN + LEX_INVENTORY_LEN) ||
		(lex_cmd_rcvd->opt.length != LEX_RCMD_OPTHDR_LEN + LEX_INVENTORY_LEN))
		length_ok = FALSE;
	    break;

        case LEX_DLOAD_STATUS:
            if ((lex_cmd_rcvd->hdr.plen != LEX_RCMD_HDR_LEN + LEX_DLOAD_STATUS_LEN) ||
	        (lex_cmd_rcvd->opt.length != LEX_RCMD_OPTHDR_LEN + LEX_DLOAD_STATUS_LEN))
                length_ok = FALSE;
 	    break;

	case LEX_DLOAD_DATA:
	    if ((lex_cmd_rcvd->hdr.plen != (LEX_RCMD_HDR_LEN + sizeof(ushort)))
		|| (lex_cmd_rcvd->opt.length != LEX_RCMD_OPTHDR_LEN + sizeof(ushort)))
		length_ok = FALSE;
	    break;

        default:
	    lex_sent_cmd = (lex_command *)lex_rcmd_headstart(rcmd->pak);
	    if ((lex_cmd_rcvd->hdr.plen != lex_sent_cmd->hdr.plen) ||
		(lex_cmd_rcvd->opt.length != lex_sent_cmd->opt.length))
                length_ok = FALSE;
	    break;
    }
    if (length_ok == FALSE)
        buginf("\nLEX-RCMD: illegal length for %s : %s",
                pak->if_input->lex_swidb->namestring,
                lex_print_rcmd(lex_cmd_rcvd->opt.type));
    return(length_ok);
}

/* 
 * lex_rcmd_stub_request_send
 * All lex interface configuration commands (and also LEX_INVENTORY)
 * should call lex_rcmd_stub_request_send instead of calling 
 * lex_setup_and_send_rcmd directly.  This is because if there are many 
 * lex interfaces configured with the maximum number of configuration 
 * commands, too many packets would be consumed at startup and would not be
 * released until the acknowledgements came back.  Instead of sending
 * the packets immediately, lex_rcmd_stub_request_send checks how many
 * rcmds are outstanding, and if at the maximum, enqueues them
 * onto LEX_rcmd_waitQ until a slot is vacated.
 * Called in lex_bind for all of the possible commands:
 * LEX_INVENTORY, LEX_SHUTDOWN, LEX_PROTO_FILTER, LEX_ADDR_FILTER, and
 * LEX_SET_PRIORITY.  Also called from the lex interface's idb->shutdown 
 * vector.
 */

void lex_rcmd_stub_request_send (uchar cmd, idbtype *swidb)
{
    lex_rcmd_stub_type *rcmd_stub;

    if (LEXrcmdQ.count >= LEX_MAX_OUTSTANDING_RCMDS) {
        rcmd_stub = malloc(sizeof(lex_rcmd_stub_type));
        if (!rcmd_stub) {
            printf(nomemory);
            return;
        }
        rcmd_stub->cmd_code = cmd;
        rcmd_stub->swidb = swidb;
        p_enqueue(&LEX_rcmd_waitQ, rcmd_stub);
    }
    else
        lex_setup_and_send_rcmd(cmd, swidb);
}


/*
 * lex_rcmd_throttle_send
 * If remote command requests have been queued on LEX_rcmd_waitQ and
 * there is room in the LEXrcmdQ, call lex_setup_and_send_rcmd to
 * send the remote command to the FLEX device.
 */

static void lex_rcmd_throttle_send (void)
{
    lex_rcmd_stub_type *rcmd_stub;
    int send_count;
    int i;

    send_count = MIN(LEX_rcmd_waitQ.count,
                     LEX_MAX_OUTSTANDING_RCMDS - LEXrcmdQ.count);
    if (send_count <= 0)
        return;
    for (i = 0; i < send_count; i++) {
        rcmd_stub = p_dequeue(&LEX_rcmd_waitQ);
        if (rcmd_stub == NULL)
            break;
        lex_setup_and_send_rcmd(rcmd_stub->cmd_code, rcmd_stub->swidb);
        free(rcmd_stub);
    }
}

/*
 * lex_setup_rcmd
 * Allocate the resources, and initialize the structures necessary to
 * send a remote command to the FLEX device:
 * - malloc the memory for a lex_rcmdst_type structure and initialize
 *   its fields
 * - create a timer
 * - get a packet buffer and initialize if_input, if_output, and
 *   linktype fields.
 * - initialize the lex_hdr and lex_option sections of the remote
 *   command.
 * - enqueue it onto LEXrcmdQ
 */

lex_rcmdst_type *lex_setup_rcmd (uchar cmd, idbtype *swidb,
                                        boolean process_waiting)
{
    paktype *pak = NULL;
    lex_rcmdst_type *rcmd = NULL;
    lex_command *lex_cmd = NULL;

    if (system_loading || (swidb == NULL) || (swidb->lex_swidb == NULL))
        return(NULL);

    rcmd = malloc(sizeof(lex_rcmdst_type));
    if (!rcmd) {
        printf(nomemory);
        return(NULL);
    }
    rcmd->cmd_code = cmd;
    rcmd->swidb = swidb;

    if (process_waiting)
        rcmd->process_waiting = TRUE;
    else
        rcmd->process_waiting = FALSE;

    /* 
     * Reserve command ID of 0 for un-solicited messages such as error 
     * conditions.
     */
    LEX_cmd_id++;
    if (LEX_cmd_id == 0)
	LEX_cmd_id++;
    rcmd->cmd_id = LEX_cmd_id;

    rcmd->retry_count = 0;
    rcmd->retrans_tmr = create_timer(lex_retrans_hndlr, rcmd, 0, 0,
                                     "lex_retrans_timer", TMT_ONCE);
    if (rcmd->retrans_tmr == NULL) {
        free(rcmd);
        return(NULL);
    }

    rcmd->status = 0;
    rcmd->status_posted = FALSE;
    rcmd->delete_rcmd = FALSE;

    /* 
     * Store the tty where the remote command is issued.
     */
    rcmd->tty = stdio; 

    /* 
     * Get a buffer for the remote command and initialize the control fields.
     */
    pak = getbuffer(sizeof(lex_command) + PPP_ENCAPBYTES);
    if (!pak) {
        destroy_timer(rcmd->retrans_tmr);
        free(rcmd);
        return(NULL);
    }

    rcmd->pak = pak;
    pak->if_input = NULL;
    pak->if_output = swidb->lex_swidb;	/* if_output is serial idb */
    pak->linktype = LINK_LEX_RCMD;

    lex_cmd = (lex_command *)lex_rcmd_headstart(pak);

    /*
     * Build PPP/LCP header.
     */
    lex_cmd->hdr.code = LEX_RCMD_REQ;
    lex_cmd->hdr.id = rcmd->cmd_id;
    lex_cmd->hdr.plen = LEX_RCMD_HDR_LEN;

    /*
     * Build option header.
     */
    lex_cmd->opt.type = rcmd->cmd_code;
    lex_cmd->opt.flags = 0;
    lex_cmd->opt.length = LEX_RCMD_OPTHDR_LEN;

    p_enqueue(&LEXrcmdQ, rcmd);

    return(rcmd);
}


/*
 * lex_send_rcmd
 * Does everything necessary to send an rcmd that has been previously
 * set up with all the correct information:
 * - set the retransmission timer
 * - encapsulate the datagram (using the plen field from the rcmd header)
 * - call pak_lock in case the packet needs to be re-transmitted
 *   (datagram_done will be called in lex_rcmd_cleanup to return the buffer
 *    back to the buffer pool)
 * - call datagram_out
 */

void lex_send_rcmd (lex_rcmdst_type *rcmd)
{
    idbtype *serial_swidb;
    hwidbtype *serial_hwidb;
    lex_command *lex_cmd = NULL;

    if (rcmd->swidb->lex_swidb == NULL) {
        if (lex_rcmd_debug)
            buginf("\nLEX-RCMD: %s %s", rcmd->swidb->namestring, lex_not_bound);
        if (rcmd->delete_rcmd)
            lex_rcmd_cleanup(rcmd);
        else {
            rcmd->status = LEX_LINK_DOWN;
            rcmd->status_posted = TRUE;
        }
        return;
    }

    serial_swidb = rcmd->swidb->lex_swidb;
    serial_hwidb = serial_swidb->hwptr;

    lex_cmd = (lex_command *)lex_rcmd_headstart(rcmd->pak);
    /*
     * The FLEX box takes a long time to burn flash, so on the FIRST
     * LEX_DLOAD_STATUS message, use a long timeout value.
     */
    if ((rcmd->cmd_code == LEX_DLOAD_STATUS) && (rcmd->retry_count == 0))
	set_timer(rcmd->retrans_tmr, LEX_FLASH_WRITE_TIME);
    else
	set_timer(rcmd->retrans_tmr, rcmd->swidb->lex_info->rcmd_timeout);

    if (lex_encap_rcmd_packet(serial_hwidb, rcmd->pak, lex_cmd->hdr.plen)) {
        pak_lock(rcmd->pak);
        datagram_out(rcmd->pak);
        if (lex_rcmd_debug)
	    if (lex_cmd->opt.type == LEX_DLOAD_DATA)
	        lexdump(rcmd->pak->datagramstart, 
		        rcmd->pak->if_output->hwptr->encsize + 
			LEX_RCMD_HDR_LEN + LEX_BLOCK_NUM_SIZE, "\nSend rcmd:");
	    else
	        lexdump(rcmd->pak->datagramstart, rcmd->pak->datagramsize, "\nSend rcmd:");
    }
    else {
        if (lex_rcmd_debug)
            buginf("\nLEX-RCMD: encapsulation failure");
    }
}


static void lex_process_ack (paktype *rcvd_pak, lex_rcmdst_type *rcmd)
{
    lex_command *sent_cmd = NULL;
    lex_command *rcvd_cmd = NULL;
    uchar *stats_offset = NULL;

    rcvd_cmd = (lex_command *)lex_rcmd_headstart(rcvd_pak);

    switch (rcvd_cmd->opt.type) {

	case  LEX_NOSHUTDOWN:

	    net_cstate(rcmd->swidb->hwptr, IDBS_UP);
	    lex_rcmd_cleanup(rcmd);
	    break;

	case  LEX_SHUTDOWN:

	    net_cstate(rcmd->swidb->hwptr, IDBS_ADMINDOWN);
	    lex_rcmd_cleanup(rcmd);
	    break;

	case LEX_DLOAD_DATA:

	    sent_cmd = (lex_command *)lex_rcmd_headstart(rcmd->pak);
 	    if (sent_cmd->cmd.dload_data.block_number ==
	        rcvd_cmd->cmd.dload_data.block_number) {
		    rcmd->status = LEX_BLOCK_ACK;
		    /*
		     * If this is the last block of the file, don't
		     * print '!' because of the following cosmetic bug
		     * that occurs: !!![OK - 73490/131072 bytes]!
		     */
		    if (sent_cmd->opt.length == LEX_RCMD_OPTHDR_LEN +
						 LEX_BLOCK_NUM_SIZE +
						 + TFTP_MAXDATA) {
			/*
			 * Print on the tty which issued the command.
			 */
		        ttyprintc(rcmd->tty, '!');
		        ttyflush(rcmd->tty);
		    }
	    }
 	    else
	        rcmd->status = LEX_SEQ_ERROR;

	    rcmd->status_posted = TRUE;
	    break;

	case LEX_DLOAD_WRQ:

  	    if (rcvd_cmd->cmd.dload_wrq.block_number == 0)
	        rcmd->status = LEX_BLOCK_ACK;
	    else
	        rcmd->status = LEX_SEQ_ERROR;
	    rcmd->status_posted = TRUE;
	    break;

	case LEX_DLOAD_STATUS:

	    rcmd->status = rcvd_cmd->cmd.dload_status.status_code;
	    rcmd->status_posted = TRUE;
	    break;

	case LEX_STATS:

	    stats_offset = (uchar *)&rcvd_cmd->cmd.data;
	    if (rcvd_cmd->opt.flags & LEX_SERIAL_STATS) {
    		lex_update_serial_stats((lex_serial_counters_t *)stats_offset,
			         &(rcmd->swidb->lex_info->lex_stats->serial));

		if (rcvd_cmd->opt.flags & LEX_ETHER_STATS) {
		    stats_offset += sizeof(lex_serial_counters_t);
		    lex_update_ether_stats((lex_ether_counters_t *)stats_offset,
			             &(rcmd->swidb->lex_info->lex_stats->ether));
		}
	    }
	    else if (rcvd_cmd->opt.flags & LEX_ETHER_STATS)
    		lex_update_ether_stats((lex_ether_counters_t *)stats_offset,
			         &(rcmd->swidb->lex_info->lex_stats->ether));
	    if (rcmd->process_waiting) {
	        rcmd->status = LEX_BLOCK_ACK;
	        rcmd->status_posted = TRUE;
	    }
	    else
		lex_rcmd_cleanup(rcmd);
	    break;

	case LEX_INVENTORY:
	    bcopy(&rcvd_cmd->cmd.inventory, 
	          rcmd->swidb->lex_info->inventory, LEX_INVENTORY_LEN);

	    lex_rcmd_cleanup(rcmd);
	    break;

	default:
	    lex_rcmd_cleanup(rcmd);
	    break;
    }
}

static void lex_process_nak_rej (uchar code, paktype *rcvd_pak, lex_rcmdst_type *rcmd)
{
    lex_command *rcvd_cmd = NULL;

    rcvd_cmd = (lex_command *)lex_rcmd_headstart(rcvd_pak);

    if (rcmd->process_waiting) {
	if (code == LEX_RCMD_NAK)
	    rcmd->status = LEX_NAK_ERROR;
	else if (code == LEX_RCMD_REJ)
	    rcmd->status = LEX_REJ_ERROR;
	rcmd->status_posted = TRUE;
    }
    else {
	if (code == LEX_RCMD_NAK)
	    errmsg(&msgsym(RCMDNAK, LEX), rcmd->swidb->namestring,
	           lex_print_rcmd(rcmd->cmd_code));
	else if (code == LEX_RCMD_REJ)
	    errmsg(&msgsym(RCMDREJ, LEX), rcmd->swidb->namestring,
	           lex_print_rcmd(rcmd->cmd_code));
	lex_rcmd_cleanup(rcmd);
    }
}

/*
 * lex_retrans_hndlr
 * If an rcmd times-out, lex_retrans_hndlr is invoked by the timer process.
 * The action taken depends on whether the "retry count" for the lex
 * interface has been exceeded or not.  
 *
 * If the retry count HAS NOT been exceeded, the retry_count field of the 
 * rcmd is incremented and lex_send_rcmd is called to send the rcmd again.  
 *
 * If the retry count HAS been exceeded, then the action depends on whether 
 * a process is waiting on the rcmd.  If a process is waiting on the rcmd,
 * a status of LEX_TIMEOUT_ERROR is posted to the rcmd (and that process is
 * responsible for calling lex_rcmd_cleanup).  If no process is
 * waiting on the rcmd, then an error message is issued and lex_rcmd_cleanup
 * is called to stop further processing on this rcmd and free up its
 * associated resources.
 */

static void lex_retrans_hndlr (lex_rcmdst_type *rcmd)
{
    if (lex_rcmd_debug)
        buginf("\nLEX-RCMD: timeout for %s: %s command", 
		rcmd->swidb->namestring, lex_print_rcmd(rcmd->cmd_code));

    if (rcmd->delete_rcmd) {
        errmsg(&msgsym(NOTSENT, LEX), rcmd->swidb->namestring,
		lex_print_rcmd(rcmd->cmd_code));
        lex_rcmd_cleanup(rcmd);
        return;
    }

    if (rcmd->retry_count >= rcmd->swidb->lex_info->retry_count) {

        if (rcmd->process_waiting) {
            rcmd->status = LEX_TIMEOUT_ERROR;
            rcmd->status_posted = TRUE;
        }
        else {
            /*
             * Issue "Retry count exceeded for <lexN> : <command>"
             * because these are config commands.  The other commands will use 
             * printf to issue the error message.
             */
            errmsg(&msgsym(TIMEOUT, LEX), rcmd->swidb->namestring,
	           lex_print_rcmd(rcmd->cmd_code));
            lex_rcmd_cleanup(rcmd);
        }
    }
    else {
        if (lex_rcmd_debug)
            buginf("\nLEX-RCMD: re-transmitting #%d %s : %s command", 
		   rcmd->cmd_code, rcmd->swidb->namestring,
                   lex_print_rcmd(rcmd->cmd_code));

        if (rcmd->process_waiting) {
	    /*
	     * Print on the tty which issued the command.
	     * this DOES NOT WORK.  it prints to the console
	     * because the rcmd->tty field is not initialized.
	     * i want to delete this printc completely.  eli
	     */         
	    ttyprintc(rcmd->tty, '.');
	    ttyflush(rcmd->tty);
        }
        rcmd->retry_count++;
        lex_send_rcmd(rcmd);
    }
}


static boolean lex_build_proto_filter_cmd (lex_rcmdst_type *rcmd, lex_command *lex_cmd)
{
    typetype *grp = NULL;
    typeitem *item;
    int flt_cnt = 0;

    if (rcmd->swidb->lex_info->lex_type_in != 0) {
        grp = type_get_acl(rcmd->swidb->lex_info->lex_type_in);
        if (!grp)
            return(FALSE);

        for (flt_cnt = 0, item = &grp->item[0];
	     (flt_cnt < grp->items) && (flt_cnt < LEX_MAX_PROTO_FILTERS);
	      item++, flt_cnt++) {
            lex_cmd->cmd.prot_fltr.filter[flt_cnt].value = item->value;
            lex_cmd->cmd.prot_fltr.filter[flt_cnt].mask  = item->mask;
            lex_cmd->cmd.prot_fltr.filter[flt_cnt].grant = item->grant;
        }
    }
    lex_cmd->hdr.plen += (flt_cnt * LEX_PFILTER_ENTRY_LEN);
    lex_cmd->opt.length += (flt_cnt * LEX_PFILTER_ENTRY_LEN);
    return(TRUE);
}


static boolean lex_build_addr_filter_cmd (lex_rcmdst_type *rcmd, lex_command *lex_cmd)
{
    int fltr_cnt = 0;
    macaddracctype *macitem;

    if (rcmd->swidb->lex_info->lex_address_in != 0) {
        macitem = mac_get_std_acl(rcmd->swidb->lex_info->lex_address_in);
        if (!macitem)
            return(FALSE);

        while (macitem && (fltr_cnt < LEX_MAX_ADDR_FILTERS)) {
            ieee_copy(macitem->addr.u.byte,
                      &lex_cmd->cmd.addr_fltr.flt[fltr_cnt].macaddr[0]);
            ieee_copy(macitem->mask.u.byte,
                      &lex_cmd->cmd.addr_fltr.flt[fltr_cnt].mask[0]);
	    lex_cmd->cmd.addr_fltr.flt[fltr_cnt].grant = macitem->grant;
            fltr_cnt++;
            macitem = macitem->next;
        }
    }
    lex_cmd->hdr.plen += (LEX_AFILTER_ENTRY_LEN * fltr_cnt);
    lex_cmd->opt.length += (LEX_AFILTER_ENTRY_LEN * fltr_cnt);
    return(TRUE);
}


static boolean lex_build_priority_cmd (lex_rcmdst_type *rcmd, lex_command *lex_cmd)
{
    int tcnt;		/* total count of "bridge list" elements - we only */
    			/* send the elements corresponding to "permit".    */
    int pcount = 0;	/* count of priority queue elements to send */
    typetype *grp = NULL;
    typeitem *item;
    plisttype *plist = NULL;
    ushort snaptype;
    int pindex;

    pindex = rcmd->swidb->lex_info->lex_priority_list;

    if (pindex != 0) {

	/*
	 * Walk a priority-list structure that was created via the
	 * "priority-list <list> protocol <protocol-name> <queue-keyword> [args]"
	 * command.
	 */

        if ((parray[pindex]) == NULL)
            return(FALSE);

        for (plist = parray[pindex]->plist;
	     (plist && (pcount < LEX_MAX_PRIORITY_ENTRIES)); plist = plist->next) {
            /*
             * If the "priority-list" command used the 
             * "protocol bridge list <list-number>" option, then walk
             * the data structure associated with the type-200 access list.
             * This command assigns a priority level to bridged
             * traffic according to an Ethernet-type code access list
             * number assigned by the "access-list" command.
             */
            if ((plist->linkcode == LINK_BRIDGE) && (plist->testlist)) {
                grp = type_get_acl(plist->testlist);
                if (!grp)
                    continue;

                for (tcnt = 0, item = &grp->item[0]; tcnt < grp->items;
                     item++, tcnt++) {
                    if (item->grant) {
                        lex_cmd->cmd.prot_prty.pentry[pcount].proto = item->value;
                        lex_cmd->cmd.prot_prty.pentry[pcount].priority = plist->priority;
                        pcount++;
                    }
                }
            }
            else if ((snaptype = link2snaptype(plist->linkcode)) != 0) {
                lex_cmd->cmd.prot_prty.pentry[pcount].proto = snaptype;
                lex_cmd->cmd.prot_prty.pentry[pcount].priority = plist->priority;
                pcount++;
            }
        }
    }
    lex_cmd->hdr.plen += (LEX_PRI_ENTRY_LEN * pcount);
    lex_cmd->opt.length += (LEX_PRI_ENTRY_LEN * pcount);
    return(TRUE);
}


/*
 * lex_setup_and_send_rcmd
 * Call the routines necessary to set up a lex rcmd and send it to the
 * cisco-1000.
 * IMPORTANT NOTE: This routine should only be called for rcmds that do
 * NOT have have a process waiting on them, since lex_setup_rcmd as
 * called below will set rcmd->process_waiting = FALSE.
 */

static void lex_setup_and_send_rcmd (uchar cmd, idbtype *swidb)
{
    lex_rcmdst_type *rcmd;
    lex_command *lex_cmd = NULL;

    rcmd = lex_setup_rcmd(cmd, swidb, FALSE);
    if (rcmd == NULL)
        return;

    lex_cmd = (lex_command *)lex_rcmd_headstart(rcmd->pak);

    switch (cmd) {
        case LEX_PROTO_FILTER:
            if (!lex_build_proto_filter_cmd(rcmd, lex_cmd)) {
                lex_rcmd_cleanup(rcmd);
                return;
            }
            else
                break;

        case LEX_ADDR_FILTER:
            if (!lex_build_addr_filter_cmd(rcmd, lex_cmd)) {
                lex_rcmd_cleanup(rcmd);
                return;
            }
            else
                break;

        case LEX_SET_PRIORITY:
            if (!lex_build_priority_cmd(rcmd, lex_cmd)) {
                lex_rcmd_cleanup(rcmd);
                return;
            }
            else
                break;

        case LEX_SHUTDOWN:
        case LEX_NOSHUTDOWN:
        case LEX_INVENTORY:
	    /*
	     * Nothing to do since plen has already been set in lex_setup_rcmd.
	     */
            break;

        default:
            if (lex_rcmd_debug)
                buginf("\nLEX-RCMD: lex_setup_and_send called with invalid parameter");
            lex_rcmd_cleanup(rcmd);
            return;
    }
    lex_send_rcmd(rcmd);
}


void lex_shutdown (hwidbtype *idb, int newstate)
{
    /*
     * If lex hardware idb is mark deleted, don't send a shutdown
     * message across to the flex box.
     */
    if (!(idb->status & IDB_DELETED)) {
	lex_rcmd_stub_request_send(LEX_SHUTDOWN, idb->firstsw);
    }
}

void lex_noshutdown (hwidbtype *idb)
{
    lex_setup_and_send_rcmd(LEX_NOSHUTDOWN, idb->firstsw);
}


boolean lex_rcmdBLOCK (lex_rcmdst_type *rcmd)
{
    if (rcmd->status_posted)
        return(FALSE);
    if (psipending()) {
        rcmd->status = LEX_RCMD_ABORTED;
        return(FALSE);
    }
    return(TRUE);
}


static boolean lex_check_version (uchar **buf, long *bytesleft, long len, void *opaque_args)
{
    struct lex_dload_version_args *dload_args;
    struct lex_image_header *lex_image_hdr;
    lex_inventory_msg *lex_inventory;
    char confirm_buffer[LEX_CONFIRM_BUF_SIZE];
    dload_args = (struct lex_dload_version_args *)opaque_args;

    dload_args->status = LEX_IMAGE_OK; /* assume OK */

    if (len == 0) {
	dload_args->status = LEX_IMAGE_NOT_CHECKED;
	/*
	 * Print on the tty which issued the command.
	 */         
	printc('.');
	flush();
	return(TRUE);
    }

    lex_image_hdr = (lex_image_header *)dload_args->dload_buf;
    lex_inventory = dload_args->idb->lex_info->inventory;

    if ((lex_image_hdr->file_length > lex_inventory->flash_size) ||
	(lex_image_hdr->file_length == 0)) {
	printf("\nIncorrect file length in image header");
	dload_args->status = LEX_IMAGE_BAD;
    }
    else
	/*
	* Return image size so that it can be provided to FLEX.
	*/
	dload_args->length = lex_image_hdr->file_length;

    if (LEX_FLASH_HAS_FILE(dload_args->idb)) {
        if ((lex_image_hdr->sw_major_ver < lex_inventory->flash_sw_ver_maj) ||
           ((lex_image_hdr->sw_major_ver == lex_inventory->flash_sw_ver_maj) &&
            (lex_image_hdr->sw_minor_ver <= lex_inventory->flash_sw_ver_min))) {
	    sprintf(confirm_buffer, "%s %u.%u with %u.%u ?",
		    "\nOK to overwrite Flash software version",
		    lex_inventory->flash_sw_ver_maj,
		    lex_inventory->flash_sw_ver_min,
		    lex_image_hdr->sw_major_ver,
		    lex_image_hdr->sw_minor_ver);
	    if (!confirm(confirm_buffer)) {
		printf("\nCopy aborted");
		dload_args->status = LEX_IMAGE_BAD;
	    }
	    return(FALSE);
        }
    }
    
    if ((lex_image_hdr->sw_major_ver < lex_inventory->prom_sw_ver_maj) ||
	((lex_image_hdr->sw_major_ver == lex_inventory->prom_sw_ver_maj) &&
	 (lex_image_hdr->sw_minor_ver < lex_inventory->prom_sw_ver_min))) {
	sprintf(confirm_buffer, "%s %u.%u with %u.%u ?",
                "\nOK to supercede PROM software version",
		lex_inventory->prom_sw_ver_maj,
		lex_inventory->prom_sw_ver_min,
		lex_image_hdr->sw_major_ver,
		lex_image_hdr->sw_minor_ver);
	if (!confirm(confirm_buffer)) {
	    printf("\nCopy aborted");
	    dload_args->status = LEX_IMAGE_BAD;
	}
	return(FALSE);
    }
    

    /*
     * Unless we were called with len == 0, we always want to return
     * FALSE so that we don't get called back twice for the version check.
     */
    return(FALSE);
}


/*
 * lex_check_tftp
 * Callback function for reg_invoke_file_read call.
 * - Returning "FALSE" from this function will stop the file_read,
 *   returning "TRUE" will cause the file_read to continue.
 * - If data was read then parameter "len" will be greater than 0.
 *   Note that if TFTP cannot find the file, this callback function is 
 *   invoked with len = 0.  TFTP will retry several times before timing out
 *   but ONLY if this function returns TRUE when (len == 0).
 * - In the file_read call, a buffersize parameter is passed which is the
 *   TOTAL available buffer space.  This variable is decreased by the
 *   size of each read and is passed to this callback in the "bytesleft"
 *   parameter. Since we pass in the size of the FLEX flash memory,
 *   we can detect the "buffer overflow" condition here.
 */

static boolean lex_check_tftp (uchar **buf, long *bytesleft, long len, void *opaque_args)
{
    struct lex_dload_args *dload_args;
    dload_args = (struct lex_dload_args *)opaque_args;

    if ((len > 0) || ((len == 0) && (dload_args->block_number > 1))) {
	/* lex_sendblock will issue any error messages */
        if (lex_sendblock(dload_args->dload_buf, len, 
	    dload_args->block_number++, dload_args->idb)) {
            *buf = dload_args->dload_buf;
            return(TRUE);
        }
        else
            return(FALSE);
    }
    return(TRUE); /* tftp read 0 bytes */
}

/*
 * lex_rcmd_send_dload
 * Calls lex_send_rcmd to send the various download commands that have
 * been previously set up (WRQ, SRQ and DATA) and handles all of the error 
 * cases.  This is the only routine that calls lex_rcmd_cleanup for all
 * of the download commands.
 */

static boolean lex_rcmd_send_dload (lex_rcmdst_type *rcmd)
{
    uchar status;
    int seq_err_retries = 0;

dload_retry:
    lex_send_rcmd(rcmd);
    edisms((blockproc *)lex_rcmdBLOCK, (ulong)rcmd);

    if (rcmd->status == LEX_SEQ_ERROR) {
        seq_err_retries++;
        if (seq_err_retries < LEX_SEQ_ERR_RETRIES)
            goto dload_retry;
        else {
            lex_rcmd_cleanup(rcmd);
            printf("\nSequence error");
            return(FALSE);
        }
    }
    else if (rcmd->status != LEX_BLOCK_ACK) {
        status = rcmd->status;
        lex_rcmd_cleanup(rcmd);
        if (status == LEX_RCMD_ABORTED)
	    printf("\nCommand aborted");
        else if (status == LEX_LINK_DOWN)
	    printf("\nSerial link down");
        else if (status ==  LEX_TIMEOUT_ERROR)
	    printf("\nTimeout error - LAN Extender not responding");
        else if (status == LEX_FLASH_WRERR)
	    printf("\nError writing to flash");
	else if (status == LEX_CHECKSUM_ERR)
	    printf("\nFile checksum error");
	else if (status == LEX_NAK_ERROR)
	    printf("\nReceived NAK from LAN Extender");
	else if (status == LEX_REJ_ERROR)
	    printf("\nReceived REJ from LAN Extender");
        else
	    printf("\nError in copy to LAN Extender");
        return(FALSE);
    }
    lex_rcmd_cleanup(rcmd);
    return(TRUE);
}


/*
 * Send the DOWNLOAD-WRITE-REQUEST
 */

static boolean lex_send_WRQ (char *filename, ulong filesize, idbtype *idb)
{
    int i;
    lex_rcmdst_type *rcmd;
    lex_command *lex_cmd;

    rcmd = lex_setup_rcmd(LEX_DLOAD_WRQ, idb, TRUE);
    if (!rcmd)
        return(FALSE);

    lex_cmd = (lex_command *)lex_rcmd_headstart(rcmd->pak);
    lex_cmd->cmd.dload_wrq.block_number = 0;
    lex_cmd->cmd.dload_wrq.filesize = filesize;
    i = 0;
    while (((lex_cmd->cmd.dload_wrq.filename[i++] = *filename++) !=
             '\0') && (i < LEX_MAX_FILENM))
        ;  /* do nothing */
    lex_cmd->hdr.plen += (i + sizeof(ushort) + sizeof(ulong));
    lex_cmd->opt.length += (i + sizeof(ushort) + sizeof(ulong));

    return(lex_rcmd_send_dload(rcmd));
}


/*
 * lex_send_SRQ
 * Send DOWNLOAD-STATUS-REQUEST message to the FLEX device.  During the
 * download, FLEX stores all the data buffers in RAM, and does not write
 * them to flash memory until the entire file has been received and
 * verified (checksum OK).  We need to send the DOWNLOAD-STATUS-REQUEST
 * message to find out if flash was burned successfully or if any errors
 * occurred.
 */

static boolean lex_send_SRQ (idbtype *idb)
{
    lex_rcmdst_type *rcmd = NULL;

    rcmd = lex_setup_rcmd(LEX_DLOAD_STATUS, idb, TRUE);
    if (!rcmd)
        return(FALSE);

    return(lex_rcmd_send_dload(rcmd));
}


boolean lex_sendblock (uchar *buffer, int len, long blknum, idbtype *idb)
{
    lex_rcmdst_type *rcmd = NULL;
    lex_command *lex_cmd;

    rcmd = lex_setup_rcmd(LEX_DLOAD_DATA, idb, TRUE);
    if (!rcmd)
        return(FALSE);

    lex_cmd = (lex_command *)lex_rcmd_headstart(rcmd->pak);
    lex_cmd->hdr.plen += (LEX_BLOCK_NUM_SIZE + len);
    lex_cmd->opt.length += (LEX_BLOCK_NUM_SIZE + len);
    lex_cmd->cmd.dload_data.block_number = blknum;
    bcopy(buffer, lex_cmd->cmd.dload_data.data, len);

    return(lex_rcmd_send_dload(rcmd));
}


void lex_download_command (parseinfo *csb)
{
    filetype file;
    boolean tftp_okay;
    char *filename;
    ulong buffersize;
    ulong buffersize_oneblock;
    idbtype *idb;
    char *hostname;
    addrtype host;
    uchar *download_buffer;
    struct lex_dload_args dload_args;
    struct lex_dload_version_args dload_ver_args;

    idb = GETOBJ(idb,1);

    if (idb->lex_swidb == NULL) {
        printf("\n%s %s", idb->namestring, lex_not_bound);
        return;
    }

    if (idb->lex_info->inventory->flash_size == 0) {
        printf(lex_no_inventory);
        return;
    }

    filename = malloc(LEX_MAX_FILENM);
    if (!filename) {
        printf(nomemory);
        return;
    }

    hostname = malloc(LEX_MAX_HOSTNM);
    if (!hostname) {
        printf(nomemory);
        free(filename);
        return;
    }

    download_buffer = malloc(TFTP_MAXDATA);
    if (!download_buffer) {
        printf(nomemory);
        free(filename);
        free(hostname);
        return;
    }

    psion(stdio);

    /*
     * Setup parameters for the full file transfer.
     */
    dload_args.idb = idb;
    dload_args.dload_buf = download_buffer;
    dload_args.block_number = 1;
    buffersize = idb->lex_info->inventory->flash_size;

    /*
     * Setup parameters for the image version and length check.
     */
    dload_ver_args.idb = idb;
    dload_ver_args.dload_buf = download_buffer;
    buffersize_oneblock = TFTP_MAXDATA;

    if (csb->which == COPY_TFTP_LEX) {
        /*
         * Get IP address of tftp server.
         */
        address_copy(&host, &lex_hostip);

        if (!get_host_address(&host, hostname, LEX_MAX_HOSTNM))
    	    goto dload_exit;

        if (!get_source_file(filename, (LEX_MAX_FILENM - 1), "copy", TRUE))
       	    goto dload_exit;

        address_copy(&lex_hostip, &host);	/* save new default */

        file.filename = filename;
        file.proto = FILE_ACCESS_TFTP;
        bcopy(&host, &file.ps.tuba.boothost, sizeof(addrtype));

	printf("\nChecking software version");
	flush();
	
	dload_ver_args.status = LEX_IMAGE_NO_STATUS;

        tftp_okay = reg_invoke_file_read(FILE_ACCESS_TFTP, (filetype*)&file,
                                         (uchar *)download_buffer,
                                         &buffersize_oneblock, FALSE, 
					 lex_check_version, 
					 (void *)&dload_ver_args);

	if (dload_ver_args.status == LEX_IMAGE_BAD)
	    goto dload_exit;
	else if (dload_ver_args.status == LEX_IMAGE_NOT_CHECKED) {
	    printf("\nFile not found");
	    goto dload_exit;
	} else if (dload_ver_args.status == LEX_IMAGE_NO_STATUS) {
	    printf("\nConnection timed out");
	    goto dload_exit;
	}
        if (lex_send_WRQ(filename, dload_ver_args.length, idb) == FALSE)
	    goto dload_exit;

        tftp_okay = reg_invoke_file_read(FILE_ACCESS_TFTP, (filetype*)&file,
                                         (uchar *)download_buffer,
                                         &buffersize, TRUE, lex_check_tftp,
                                         (void *)&dload_args);

        if (!tftp_okay)
	    goto dload_exit;
    }
    else if (csb->which == COPY_FLASH_LEX) {

	if (reg_invoke_get_num_flash_part() <= 1) {
	    if (!get_source_file(filename, (LEX_MAX_FILENM - 1), "copy", TRUE))
		goto dload_exit;
	} else {
	    /* 
	     * multiple partions we need to display and let user choose
	     * partition number as well as filename
	     */
	    if (!reg_invoke_multi_part_get_file_name(filename)) {
		goto dload_exit;
	    }
	}
        if (!reg_invoke_file_exists(FILE_ACCESS_FLASH, filename)) {
            printf("\nFile not found");
            goto dload_exit;
        }

	printf("\nChecking software version");
	flush();

        tftp_okay = reg_invoke_flash_block_server(filename, download_buffer,
                                                  &buffersize_oneblock, FALSE,
                                                  lex_check_version,
                                                  (void *)&dload_ver_args);

	if (dload_ver_args.status == LEX_IMAGE_BAD)
	    goto dload_exit;

        if (lex_send_WRQ(filename, dload_ver_args.length, idb) == FALSE)
            goto dload_exit;

	buffersize = TFTP_MAXDATA;
        tftp_okay = reg_invoke_flash_block_server(filename, download_buffer,
                                                  &buffersize, TRUE,
                                                  lex_check_tftp,
                                                  (void *)&dload_args);

	if (tftp_okay)
	    printf("\n[OK - %d/%d bytes]\n", dload_ver_args.length, buffersize);
	else
	    goto dload_exit;
    }
        
    printf("\nWaiting for LAN Extender to write flash");
    flush();
    if (lex_send_SRQ(idb) == TRUE)
	printf("\nSuccessful download to LAN Extender");

    /*
     * Ask FLEX for new inventory info since we just changed flash.
     */
    lex_setup_and_send_rcmd(LEX_INVENTORY, idb);


dload_exit:
    psioff(stdio);
    free(filename);
    free(hostname);
    free(download_buffer);
}



char *lex_print_rcmd (uchar cmd_code)
{
    if ((cmd_code >= LEX_RCMD_START) && (cmd_code <= LEX_RCMD_END))
        return(lex_commands[cmd_code - LEX_RCMD_START]);
    else
        return("unknown");
}


/*
 * lex_reload_command
 * Called via the parser for the "clear controller lex <unit>" command.
 * This command causes the FLEX device to halt and perform a cold restart.
 */
void lex_reload_command (parseinfo *csb)
{
    idbtype *swidb;
    lex_rcmdst_type *rcmd;
    lex_command *lex_cmd;


    swidb = GETOBJ(idb,1);
    if (swidb->lex_swidb == NULL) {
	printf("\n%s %s", swidb->namestring, lex_not_bound);
	return;
    }
    printf("\nreload remote lex controller?");
    if (confirm(NULL)) {
	rcmd = lex_setup_rcmd(LEX_RELOAD, swidb, FALSE);
	if (rcmd == NULL) {
	    printf(nomemory);
	    return;
	}
	lex_cmd = (lex_command *)lex_rcmd_headstart(rcmd->pak);

	if (csb->which == LEX_RELOAD_DEFAULT)
	    lex_cmd->opt.flags = LEX_RELOAD_DEFAULT_IMAGE;
	else if (csb->which == LEX_RELOAD_PROM)
	    lex_cmd->opt.flags = LEX_RELOAD_PROM_IMAGE;

	lex_send_rcmd(rcmd);
    }
}


void lex_config_command (parseinfo *csb)
{
    idbtype *idb;
    int old_list;

    idb = csb->interface;
    if (csb->nvgen) {
        switch (csb->which) {
	case LEX_INADDRESS:
	    nv_write(idb->lex_info->lex_address_in, "%s %d", 
		     csb->nv_command, idb->lex_info->lex_address_in);
	    break;

	case LEX_INTYPE:
	    nv_write(idb->lex_info->lex_type_in, "%s %d", csb->nv_command,
		     idb->lex_info->lex_type_in);
	    break;

	case LEX_PRIORITY:
	    nv_write(idb->lex_info->lex_priority_list, 
		     "%s %d", csb->nv_command,
		     idb->lex_info->lex_priority_list);
	    break;

	case LEX_RETRY:
	    nv_write((idb->lex_info->retry_count != LEX_DEFAULT_CMD_RETRY),
		     "%s %d", csb->nv_command, idb->lex_info->retry_count);
	    break;

	case LEX_TIMEOUT:
	    nv_write((idb->lex_info->rcmd_timeout != LEX_DEFAULT_TIMEOUT),
		     "%s %d", csb->nv_command, idb->lex_info->rcmd_timeout);
	    break;

	default:
	    break;

        }
    }
    else {
        switch (csb->which) {
	case LEX_INADDRESS:

	    old_list = idb->lex_info->lex_address_in;

	    if (csb->sense)
		idb->lex_info->lex_address_in = GETOBJ(int,1);
	    else
		idb->lex_info->lex_address_in = 0;

	    lex_check_addr_filter_changed(old_list,
				          idb->lex_info->lex_address_in,
				          &lex_addr_filter_low,
				          &lex_addr_filter_high, idb);

	    lex_rcmd_stub_request_send(LEX_ADDR_FILTER, idb);
	    break;


	case LEX_INTYPE:
	    old_list = idb->lex_info->lex_type_in;

	    if (csb->sense)
		idb->lex_info->lex_type_in = GETOBJ(int,1);
	    else
		idb->lex_info->lex_type_in = 0;

	    lex_check_type_filter_changed(old_list,
				          idb->lex_info->lex_type_in,
				          &lex_type_filter_low,
				          &lex_type_filter_high, idb);

	    lex_rcmd_stub_request_send(LEX_PROTO_FILTER, idb);
	    break;

	case LEX_PRIORITY:
	    if (csb->sense)
		idb->lex_info->lex_priority_list = GETOBJ(int,1);
	    else
		idb->lex_info->lex_priority_list = 0;
	    lex_check_priority_list_changed(idb->lex_info->lex_priority_list);
	    lex_rcmd_stub_request_send(LEX_SET_PRIORITY, idb);
	    break;

	case LEX_RETRY:
	    if (csb->sense)
		idb->lex_info->retry_count = GETOBJ(int,1);
	    else
		idb->lex_info->retry_count = LEX_DEFAULT_CMD_RETRY;
	    break;

	case LEX_TIMEOUT:
	    if (csb->sense)
		idb->lex_info->rcmd_timeout = GETOBJ(int,1); 
	    else
		idb->lex_info->rcmd_timeout = LEX_DEFAULT_TIMEOUT; 
	    break;

	default:
	    break;
        }
    }
}


void lex_input_type_list_changed (ulong list)
{
    lex_info_t *info = NULL;

    FOR_ALL_LEX(&lex_activeQ,info) {
        if (info->swidb->lex_info->lex_type_in == list)
	    lex_rcmd_stub_request_send(LEX_PROTO_FILTER, info->swidb);
    }
}

void lex_input_address_list_changed (ulong list)
{
    lex_info_t *info = NULL;

    FOR_ALL_LEX(&lex_activeQ,info) {
        if (info->swidb->lex_info->lex_address_in == list)
	    lex_rcmd_stub_request_send(LEX_ADDR_FILTER, info->swidb);
    }
}

/*
 * lex_priority_list_changed
 * This function gets "registered" as a priority list callback if any
 * lex interface gets configured with the "lex priority-group" command.
 * The callback will be destroyed when all "lex priority-group" commands
 * have been removed.  It will be created/destroyed as needed by
 * lex_check_priority_list_changed.
 */
static void lex_priority_list_changed (ulong list, ushort cmd_option)
{
    lex_info_t *info = NULL;

    /*
     * Lex only supports the "protocol" keyword option of the
     * "priority-list" command.  If any other option was entered, we
     * don't care.
     */
    if (cmd_option != PRICMD_PROTOCOL)
	return;

    /*
     * Send LEX_SET_PRIORITY remote command to any active lex interface
     * that is configured for the priority list which changed.
     */
    FOR_ALL_LEX(&lex_activeQ,info) {
        if (info->swidb->lex_info->lex_priority_list == list)
	    lex_rcmd_stub_request_send(LEX_SET_PRIORITY, info->swidb);
    }
}

/*
 * lex_show_interface_common
 * Given a pointer to a lex sw-idb and the type of interface for which
 * statistics are desired (currently only Ethernet and serial exist on
 * the FLEX device), set up the rcmd, send it to FLEX, and wait for the
 * response or some error condition.  In the command option flags, 
 * we request that the FLEX device clear its counters after sending
 * the interface stats.
 *
 * please note that the route iab_lex_get_stats() in sys/lex/lex_snmp.c
 * also sends rcmds.  possibly the common code between this routine
 * and iab_lex_get_stats() should be combined for ease of maintenance.
 */

boolean lex_show_interface_common (idbtype *idb, lex_interface_type if_type)
{
    lex_rcmdst_type *rcmd = NULL;
    lex_command *lex_cmd = NULL;
    boolean retval = TRUE;

    if (idb->lex_swidb == NULL) {
        printf("\n%s %s", idb->namestring, lex_not_bound);
        return(FALSE);
    }

    rcmd = lex_setup_rcmd(LEX_STATS, idb, TRUE);
    if (rcmd == NULL)
        return(FALSE);

    lex_cmd = (lex_command *)lex_rcmd_headstart(rcmd->pak);
    if (if_type == LEX_SERIAL_INTF)
	lex_cmd->opt.flags = LEX_RESET_SERIAL_STATS + LEX_SERIAL_STATS;
    else if (if_type == LEX_ETHER_INTF)
	lex_cmd->opt.flags = LEX_RESET_ETHER_STATS + LEX_ETHER_STATS;

    psion(stdio);
    lex_send_rcmd(rcmd);
    edisms((blockproc *)lex_rcmdBLOCK, (ulong)rcmd);
    psioff(stdio);

    if (rcmd->status == LEX_TIMEOUT_ERROR) {
        printf("\nTimeout error - LAN Extender not responding");
	retval = FALSE;
    }
    else if (rcmd->status == LEX_RCMD_ABORTED) {
        printf("\nCommand aborted");
	retval = FALSE;
    }
    else if (rcmd->status == LEX_LINK_DOWN) {
        printf("\nSerial link down");
	retval = FALSE;
    }
    else if (rcmd->status == LEX_NAK_ERROR) {
	printf("\nReceived NAK from LAN Extender");
	retval = FALSE;
    }
    else if (rcmd->status == LEX_REJ_ERROR) {
	printf("\nReceived REJ from LAN Extender");
	retval = FALSE;
    }
    lex_rcmd_cleanup(rcmd);
    return(retval);
}

/*
 * lex_update_stats
 * It was decided that the host router should accumulate the FLEX interface
 * statistics and that FLEX would always clear its counters after sending
 * them to the host router.  (Actually the protocol allows for more
 * flexibility than this, but this is how it is currently done.)  One of the
 * reasons for this decision is that if the FLEX were to accumulate the
 * statistics, then a "clear counters" remote command would have to be
 * implemented.  Then the generic "clear counters" on ALL interfaces could 
 * generate a fair amount of traffic.
 */

static void lex_update_serial_stats (lex_serial_counters_t *rcvd_ctrs,
			             lex_serial_counters_t *cached_ctrs)
{
    cached_ctrs->inputs         += rcvd_ctrs->inputs;
    cached_ctrs->input_crc      += rcvd_ctrs->input_crc;
    cached_ctrs->input_frame    += rcvd_ctrs->input_frame;
    cached_ctrs->input_overrun  += rcvd_ctrs->input_overrun;
    cached_ctrs->input_drops    += rcvd_ctrs->input_drops;
    cached_ctrs->input_abort    += rcvd_ctrs->input_abort;
    cached_ctrs->input_error    += rcvd_ctrs->input_error;
    cached_ctrs->input_resource += rcvd_ctrs->input_resource;
    cached_ctrs->rx_cumbytes    += rcvd_ctrs->rx_cumbytes;
    cached_ctrs->nobuffers      += rcvd_ctrs->nobuffers;

    cached_ctrs->lastinput_delta = rcvd_ctrs->lastinput_delta;
    cached_ctrs->rx_datarate     = rcvd_ctrs->rx_datarate;
    cached_ctrs->rx_packetrate   = rcvd_ctrs->rx_packetrate;

    cached_ctrs->outputs          += rcvd_ctrs->outputs;
    cached_ctrs->output_error     += rcvd_ctrs->output_error;
    cached_ctrs->output_underrun  += rcvd_ctrs->output_underrun;
    cached_ctrs->tx_cumbytes      += rcvd_ctrs->tx_cumbytes;

    cached_ctrs->lastoutput_delta  = rcvd_ctrs->lastoutput_delta;
    cached_ctrs->tx_datarate       = rcvd_ctrs->tx_datarate;
    cached_ctrs->tx_packetrate     = rcvd_ctrs->tx_packetrate;

    cached_ctrs->output_qcount_low   = rcvd_ctrs->output_qcount_low;
    cached_ctrs->output_qmaximum_low = rcvd_ctrs->output_qmaximum_low;
    cached_ctrs->output_qdrops_low  += rcvd_ctrs->output_qdrops_low;

    cached_ctrs->output_qcount_normal   = rcvd_ctrs->output_qcount_normal;
    cached_ctrs->output_qmaximum_normal = rcvd_ctrs->output_qmaximum_normal;
    cached_ctrs->output_qdrops_normal  += rcvd_ctrs->output_qdrops_normal;

    cached_ctrs->output_qcount_medium   = rcvd_ctrs->output_qcount_medium;
    cached_ctrs->output_qmaximum_medium = rcvd_ctrs->output_qmaximum_medium;
    cached_ctrs->output_qdrops_medium  += rcvd_ctrs->output_qdrops_medium;

    cached_ctrs->output_qcount_high   = rcvd_ctrs->output_qcount_high;
    cached_ctrs->output_qmaximum_high = rcvd_ctrs->output_qmaximum_high;
    cached_ctrs->output_qdrops_high  += rcvd_ctrs->output_qdrops_high;

    cached_ctrs->transitions      += rcvd_ctrs->transitions;

}


/*
 * lex_update_ether_stats
 * Take the interface statistics for the cisco-1000 ethernet interface
 * sent in a LEX_STATS remote command, and update the structure which is hung 
 * off the lex idb for keeping these stats.
 */

static void lex_update_ether_stats (lex_ether_counters_t *rcvd_ctrs,
			            lex_ether_counters_t *cached_ctrs)
{
    cached_ctrs->inputs         += rcvd_ctrs->inputs;
    cached_ctrs->input_crc      += rcvd_ctrs->input_crc;
    cached_ctrs->input_frame    += rcvd_ctrs->input_frame;
    cached_ctrs->input_overrun  += rcvd_ctrs->input_overrun;
    cached_ctrs->input_drops    += rcvd_ctrs->input_drops;
    cached_ctrs->input_abort    += rcvd_ctrs->input_abort;
    cached_ctrs->input_error    += rcvd_ctrs->input_error;
    cached_ctrs->input_resource += rcvd_ctrs->input_resource;
    cached_ctrs->rx_cumbytes    += rcvd_ctrs->rx_cumbytes;
    cached_ctrs->nobuffers      += rcvd_ctrs->nobuffers;
    cached_ctrs->input_broad    += rcvd_ctrs->input_broad;
    cached_ctrs->runts          += rcvd_ctrs->runts;
    cached_ctrs->giants         += rcvd_ctrs->giants;

    cached_ctrs->lastinput_delta = rcvd_ctrs->lastinput_delta;
    cached_ctrs->rx_datarate     = rcvd_ctrs->rx_datarate;
    cached_ctrs->rx_packetrate   = rcvd_ctrs->rx_packetrate;

    cached_ctrs->outputs          += rcvd_ctrs->outputs;
    cached_ctrs->output_error     += rcvd_ctrs->output_error;
    cached_ctrs->output_underrun  += rcvd_ctrs->output_underrun;
    cached_ctrs->output_collision += rcvd_ctrs->output_collision;
    cached_ctrs->tx_cumbytes      += rcvd_ctrs->tx_cumbytes;

    cached_ctrs->output_qcount   = rcvd_ctrs->output_qcount;
    cached_ctrs->output_qmaximum = rcvd_ctrs->output_qmaximum;
    cached_ctrs->output_qdrops  += rcvd_ctrs->output_qdrops;

    cached_ctrs->lastoutput_delta  = rcvd_ctrs->lastoutput_delta;
    cached_ctrs->tx_datarate       = rcvd_ctrs->tx_datarate;
    cached_ctrs->tx_packetrate     = rcvd_ctrs->tx_packetrate;
    cached_ctrs->resets           += rcvd_ctrs->resets;
}

static void lex_check_addr_filter_changed (int old_list, int new_list,
				           int *cur_low, int *cur_high,
				           idbtype *idb)
{
    if (old_list == new_list)
	return;

    if ((old_list == *cur_low) || (old_list == *cur_high) ||
	((new_list != 0) && ((new_list < *cur_low) || (new_list > *cur_high)))) {
	if (*cur_low <= *cur_high)
	    access_destroy_callback(*cur_low, *cur_high);

	lex_find_addr_filter_low_high(cur_low, cur_high);

	if (*cur_low <= *cur_high)
	    access_create_callback(lex_input_address_list_changed,
			           *cur_low, *cur_high);
    }
}
static void lex_check_type_filter_changed (int old_list, int new_list,
				           int *cur_low, int *cur_high,
				           idbtype *idb)
{
    if (old_list == new_list)
	return;

    if ((old_list == *cur_low) || (old_list == *cur_high) ||
	((new_list != 0) && ((new_list < *cur_low) || (new_list > *cur_high)))) {
	if (*cur_low <= *cur_high)
	    access_destroy_callback(*cur_low, *cur_high);

	lex_find_type_filter_low_high(cur_low, cur_high);

	if (*cur_low <= *cur_high)
	    access_create_callback(lex_input_type_list_changed,
			           *cur_low, *cur_high);
    }
}

static void lex_find_addr_filter_low_high (int *cur_low, int *cur_high)
{
    int temp_list;
    lex_info_t *info;
    *cur_low = MAXMACADDRACC;
    *cur_high = MINMACADDRACC;

    FOR_ALL_LEX(&lex_activeQ, info) {
	temp_list = info->swidb->lex_info->lex_address_in;
	if (temp_list != 0) {
	    if (temp_list < *cur_low)
		*cur_low = temp_list;
	    if (temp_list > *cur_high)
		*cur_high = temp_list;
	}
    }
    FOR_ALL_LEX(&lex_freeQ, info) {
	temp_list = info->swidb->lex_info->lex_address_in;
	if (temp_list != 0) {
	    if (temp_list < *cur_low)
		*cur_low = temp_list;
	    if (temp_list > *cur_high)
		*cur_high = temp_list;
	}
    }
}

static void lex_find_type_filter_low_high (int *cur_low, int *cur_high)
{
    int temp_list;
    lex_info_t *info;
    *cur_low = MAXTYPEACC;
    *cur_high = MINTYPEACC;

    FOR_ALL_LEX(&lex_activeQ, info) {
	temp_list = info->swidb->lex_info->lex_type_in;
	if (temp_list != 0) {
	    if (temp_list < *cur_low)
		*cur_low = temp_list;
	    if (temp_list > *cur_high)
		*cur_high = temp_list;
	}
    }
    FOR_ALL_LEX(&lex_freeQ, info) {
	temp_list = info->swidb->lex_info->lex_type_in;
	if (temp_list != 0) {
	    if (temp_list < *cur_low)
		*cur_low = temp_list;
	    if (temp_list > *cur_high)
		*cur_high = temp_list;
	}
    }
}


/*
 * lex_check_priority_list_changed
 * A lex interface has just been configured with the 
 * "[no] lex priority-group <group>" command, so see if we need to
 * create or destroy the priority list callback function.
 */
static void lex_check_priority_list_changed (ulong list)
{
    lex_info_t *info;
    boolean priority_list_configured = FALSE;

    FOR_ALL_LEX(&lex_activeQ, info) {
	if (info->swidb->lex_info->lex_priority_list != 0) {
	    priority_list_configured = TRUE;
	    break;
	}
    }

    if (priority_list_configured == FALSE) {
	FOR_ALL_LEX(&lex_freeQ, info) {
	    if (info->swidb->lex_info->lex_priority_list != 0) {
		priority_list_configured = TRUE;
		break;
	    }
	}
    }

    if (priority_list_configured) {
	if (priority_callback_exists == FALSE) {
	    priority_create_callback(lex_priority_list_changed,
				     1, PRIORITY_MAXLIST-1);
	    priority_callback_exists = TRUE;
	}
    }
    else { /* No priority list configured on any lex interface */
	if (priority_callback_exists) {
	    priority_destroy_callback(lex_priority_list_changed);
	    priority_callback_exists = FALSE;
	}
    }
}


/*
 * lex_send_clear_counters_rcmd
 * handled local.
 * Set up and send a LEX_STATS rcmd just requesting the cisco-1000 to
 * clear it's interface counters for either its ethernet or serial
 * interface.
 */

void lex_send_clear_counters_rcmd (idbtype *swidb, lex_interface_type intf_type)
{
    lex_rcmdst_type *rcmd;
    lex_command *lex_cmd = NULL;

    if (confirm("Clear \"show interface\" counters on this interface ")) {
        rcmd = lex_setup_rcmd(LEX_STATS, swidb, FALSE);
        if (rcmd == NULL)
            return;

        rcmd->process_waiting = FALSE;
        lex_cmd = (lex_command *)lex_rcmd_headstart(rcmd->pak);

	/* 
	 * set shadow values equal to counter values
	 * for the whichever interface
	 */
        if (intf_type == LEX_ETHER_INTF) {
	    lex_ether_counters_t *ether_counters, *si;
	    ether_counters = &(swidb->lex_info->lex_stats->ether);
	    si = &(swidb->lex_info->lex_stats->si_ether);
	    *si = *ether_counters;
            GET_TIMESTAMP(swidb->lex_info->last_ether_cclear);
            lex_cmd->opt.flags = LEX_RESET_ETHER_STATS;
        }
        else if (intf_type == LEX_SERIAL_INTF) {
	    lex_serial_counters_t *serial_counters, *si;
	    serial_counters = &(swidb->lex_info->lex_stats->serial);
	    si = &(swidb->lex_info->lex_stats->si_serial);
	    *si = *serial_counters;
            GET_TIMESTAMP(swidb->lex_info->last_serial_cclear);
            lex_cmd->opt.flags = LEX_RESET_SERIAL_STATS;
        }
        lex_send_rcmd(rcmd);
    }
}


/*
 * lex_clear_ether_counters
 * Called via the parser for the "clear counters lex <unit> ethernet"
 * command.  Tells the cisco-1000 to clear the counters for its ethernet
 * interface.
 */

void lex_clear_ether_counters (parseinfo *csb)
{
    idbtype *idb;

    idb = GETOBJ(idb,1);
    lex_send_clear_counters_rcmd(idb, LEX_ETHER_INTF);
}


/*
 * lex_clear_serial_counters
 * Called via the parser for the "clear counters lex <unit> serial"
 * command.  Tells the cisco-1000 to clear the counters for its serial
 * interface.
 */

void lex_clear_serial_counters (parseinfo *csb)
{
    idbtype *idb;

    idb = GETOBJ(idb,1);
    lex_send_clear_counters_rcmd(idb, LEX_SERIAL_INTF);
}

/*
 * Debugging routine used to dump the packets being sent to (received
 * from) the flex box.
 */
void lexdump(unsigned char *s, int l, char *t)
{
        register unsigned char *q;
        char line[4];
        int cnt;

        cnt = 0;
        q = s + l;
        while (s < q) {
                if ((cnt & 0xf) == 0) {
                        if (cnt > 0)

                        if (cnt > 0)
                                buginf("\n");
                                buginf("%s: ", t);
                }
                line[0] = toupper(tohexchar(*s >> 4));
                line[1] = toupper(tohexchar(*s));
                line[2] = ' ';
                line[3] = 0;
                buginf(line);
                cnt++;
                s++;
        }
        buginf("\n");
}

/*
 * Encapsulate a lex remote command packet.
 * We cannot call ppp encaps because when commpression is turned on
 * the remote command packet also gets commpressed, as compression is
 * done in ppp vencapsulate.
 */
static boolean lex_encap_rcmd_packet(hwidbtype *serial_hwidb, paktype *pak,
				     int length) 
{
    fsm *f;

    f = reg_invoke_ppp_link2fsm(LINK_LEX, serial_hwidb);

    if (!f || (f->state != OPEN)) {
	return FALSE;
    }

    pak->datagramstart = pak->network_start;
    pak->datagramsize = length;	/* Set datagram size */

    pak->encsize = 0;

    /* Write the type field in the header, which is type LEXCP */
    pak->datagramstart -= PPP_TYPE_FIELD_SIZE;
    PUTSHORT(pak->datagramstart, TYPE_PPP_LEXCP);
    pak->encsize += PPP_TYPE_FIELD_SIZE;
    pak->datagramsize += PPP_TYPE_FIELD_SIZE;

    /* Write the broadcast and control information of the ppp header */
    pak->datagramstart -= PPP_BRD_CNTL_SIZE;
    PUTSHORT(pak->datagramstart, PPP_STATION);
    pak->datagramsize += PPP_BRD_CNTL_SIZE;
    pak->encsize += PPP_BRD_CNTL_SIZE;


    pak->mac_start  = pak->datagramstart;
    pak->addr_start = NULL;
    pak->rif_start  = NULL;
    pak->riflen     = 0;
    pak->info_start = NULL;
    pak->enctype    = ET_PPP;
    
    /*
     * Check if it is a mips interface.
     */
    if (serial_hwidb->board_encap) {
	if (!(*serial_hwidb->board_encap)(pak, serial_hwidb)) {
	    return(FALSE);
	}
    }
    return(TRUE);
}
