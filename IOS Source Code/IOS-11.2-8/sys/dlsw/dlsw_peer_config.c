/* $Id: dlsw_peer_config.c,v 3.22.6.12 1996/09/09 19:08:41 akhanna Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer_config.c,v $
 *------------------------------------------------------------------
 * Configuration routines for DLSw Peer Module
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Called by DLSw parser macros for DLSw peer configuration commands
 *------------------------------------------------------------------
 * $Log: dlsw_peer_config.c,v $
 * Revision 3.22.6.12  1996/09/09  19:08:41  akhanna
 * CSCdi65724:  Cannot stop keepalives on promiscous peer
 * Branch: California_branch
 *
 * Revision 3.22.6.11  1996/09/05  21:03:42  sbales
 * CSCdi67272:  dlsw remote-peer definitions order & backup peers
 * Branch: California_branch
 *              Ensure that backup peer definitions follow the
 *              definition of the peer being backed up.
 *
 * Revision 3.22.6.10  1996/08/28  12:41:01  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.22.6.9  1996/08/21  22:19:12  akhanna
 * CSCdi48010:  dlsw flow control allow max/min flow control window sizes
 * Branch: California_branch
 *
 * Revision 3.22.6.8  1996/08/09  03:33:33  fbordona
 * CSCdi65588:  DLSw does not show TCP queue depth when state is WAN_BUSY
 * Branch: California_branch
 *
 * Revision 3.22.6.7  1996/08/07  08:58:15  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.22.6.6  1996/07/31  18:19:54  akhanna
 * CSCdi61887:  dlsw icanreach saps
 * Branch: California_branch
 *
 * Revision 3.22.6.5  1996/07/09  18:08:34  kmoberg
 * CSCdi46177:  access-expression output capabilities needs to be
 *              added to DLSw. Clean up remote peer config.
 * Branch: California_branch
 *
 * Revision 3.22.6.4  1996/05/30  14:43:42  sbales
 * CSCdi59064:  DLSw+: show commands need to be more usable
 *              modify options for "sh dls reach", "sh dls cir", "sh dls
 *              loc", modify formatting for "show dls peer"
 * Branch: California_branch
 *
 * Revision 3.22.6.3  1996/05/21  09:45:39  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.22.6.2  1996/05/17  10:46:06  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.19.2.9  1996/05/14  03:20:27  kpatel
 * CSCdi55075:  Cant define dlsw direct encapsulation for more that pvc
 *              dls frame-relay does not work over point to point
 *              sub-interface (CSCdi55085)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.8  1996/04/26  07:34:31  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.19.2.7  1996/04/24  18:35:26  fbordona
 * CSCdi55588:  DLSw: Use managed timers for connecting dynamic peers
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.6  1996/04/23  20:13:52  fbordona
 * CSCdi55437:  DLSw peer may take several minutes to connect after reload
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.5  1996/04/20  02:08:13  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.4  1996/04/17  18:17:12  sbales
 * CSCdi53218:  DLSw+: backup peers cause loop with ethernet
 * Branch: IbuMod_Calif_branch
 *              Also implemented parser checks for redundant backups
 *              or backup peers of backup peers.
 *
 * Revision 3.19.2.3  1996/04/03  14:01:44  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.19.2.2  1996/03/29  02:13:43  fbordona
 * CSCdi52957:  DLSW: Can not use lsap-output-list greater than 255
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.22.6.1  1996/03/18  19:32:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.19.2.1  1996/03/17  17:38:10  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.13.2.3  1996/03/16  06:39:16  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.13.2.2  1996/03/07  08:44:43  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.13.2.1  1996/02/20  00:46:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.22  1996/03/06  16:55:22  fbordona
 * CSCdi50687:  dlsw peer-on-demand-defaults does not allow the largest
 *              frame lf.
 *
 * Revision 3.21  1996/03/05  18:49:27  fbordona
 * CSCdi50574:  DLSw pod stuck in connect status.
 *              Use number of circuit count to disconnect pods.
 *
 * Revision 3.20  1996/02/27  21:36:24  fbordona
 * CSCdi49949:  DLSw peer-on-demand peers ignore tcp-queue-max.
 *              Improve "show dlsw peers" output.
 *
 * Revision 3.19  1996/02/26  20:56:25  ioakley
 * CSCdi49687: Correct problem with tcp-queue-max parameter for
 *             the global "dlsw remote-peer" config command.
 *
 * Revision 3.18  1996/02/20  22:08:12  fbordona
 * CSCdi47801:  SEgV exception, PC0x60637DFC tcpdriver_fhost.
 *              Use the tcpencaps handle now passed in the tcp
 *              closef vector (CSCdi48380) to determine if the
 *              DLSw read or write pipe has been closed.
 *
 * Revision 3.17  1996/02/07  16:11:02  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.16  1996/02/01  06:01:29  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.15  1996/02/01  02:57:03  fbordona
 * CSCdi47930:  DLSw session does not reconnect after peer powered off.
 *              Only attempt one peer connection at at time to avoid
 *              starving the dlsw background process.
 *
 * Revision 3.14  1996/01/29  07:27:22  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.13  1996/01/18  22:24:50  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.12  1995/12/30  00:53:50  kpatel
 * CSCdi46237:  Spurious memory access on 4500 on configuring
 *              no remote peer
 *
 * Revision 3.11  1995/12/19  00:39:50  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.10  1995/12/17  18:26:07  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.9  1995/12/15  04:56:34  kpatel
 * CSCdi45411:  DLSw: peer connection doesnt go thru for Direct
 *              Bridge encap
 *
 * Revision 3.8  1995/12/14  15:27:42  ravip
 * CSCdi41809:  DLSW MIB ciruit counters and OIDs always set to 0
 *
 * Revision 3.7  1995/11/30  21:22:51  fbordona
 * CSCdi44759:  DLSw peer-on-demand peers prematurely disconnect.
 *              Fix peer-on-demand crash introduced in 11.1
 *
 * Revision 3.6  1995/11/29  22:06:29  fbordona
 * CSCdi43961:  DLSw peer mysteriously shows up twice in write term.
 *              Simultaneous priority and border peers cause problems.
 *
 * Revision 3.5  1995/11/29  19:45:43  ravip
 * CSCdi40989:  crash: router tried to reference free memory in dlsw code
 *              use mem_lock() to lock memory, to avoid other routines
 *              from deleting the memory block in use.
 *              Use a flag to tag actions that would change the links.
 *
 * Revision 3.4  1995/11/20  23:25:48  fbordona
 * CSCdi42215:  dlsw backup peers broken
 *
 * Revision 3.3  1995/11/17  09:03:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:24:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  20:55:47  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/10/09  20:18:41  rnaderi
 * CSCdi41641:  Add sub-interface option to the parser for DLSw+/FR
 *
 * Revision 2.6  1995/08/07  05:31:31  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.5  1995/07/26  19:12:16  hampton
 * Convert DLSW to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37708]
 *
 * Revision 2.4  1995/06/30 18:52:26  widmer
 * CSCdi36635:  nv_add string should not contain newline
 * Fix dlsw NV generation
 *
 * Revision 2.3  1995/06/28  18:50:39  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *              Clean up DLSw header files
 *
 * Revision 2.2  1995/06/18  21:21:23  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:27:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "mgd_timers.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_priority.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../tcp/tcpdriver.h"
#include "../if/network.h"
#include "../ui/debug.h"
#include "../ibm/libibm_externs.h"
#include "../if/rif_util.h"
#include "../ibm/netbios.h"
#include "../srt/rsrb.h"
#include "../srt/srt_registry.h"
#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_core_externs.h"
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_cap_exchg.h"
#include "../dlsw/dlsw_debug.h"
#include "../ibm/libibm_externs.h"
#include "../llc2/llc2_sb.h"

/*
 * File Scope Function Prototypes
 */
static void dlsw_nvgen_port_list (parseinfo *);
static void dlsw_nvgen_ring_list (parseinfo *);
static void dlsw_nvgen_bgroup_list (parseinfo *);
static void dlsw_nvgen_local (parseinfo *);
static void dlsw_nvgen_remote (parseinfo *, peerentry *);
static void dlsw_nvgen_icanreach (parseinfo *);
static void dlsw_nvgen_icnr (parseinfo *);
static void dlsw_nvgen_pod_defs (parseinfo *);
static void dlsw_nvgen_prompeer_defs (parseinfo *);
static void dlsw_nvgen_bgroup_assign(parseinfo *csb);
static void parse_local_peer (parseinfo *);
static void parse_remote_peer (parseinfo *);
static void parse_port_list (parseinfo *);
static void parse_ring_list (parseinfo *);
static void parse_bgroup_list (parseinfo *);
static void parse_icanreach (parseinfo *);
static void parse_icnr (parseinfo *csb);
static void parse_pod_defs (parseinfo *csb);
static void parse_prompeer_defs (parseinfo *csb);
static void print_peer_info (peerentry  *, int);
static void peer_update_sup_sap (boolean, uchar *, uchar);
static void parse_bgroup_assign (parseinfo *csb);
static uchar cvsap_to_sap (uchar, uchar);

void dlsw_assign_bgroup_llc2_params(idbtype *sw_idb,dlsw_bgroup_t *dlsw_bgroup);

/*
 * File Scope Data
 */
static char *const peer_type_str[PEER_MAX_TYPE] = 
                                     {"conf", "prom", "pod", "dynamic"};

/*
 * Global Data
 */
boolean peer_debug_all = FALSE;
boolean dlsw_force_ssp = TRUE;
sys_timestamp dlsw_start_time;

/**********************************************************************
 *
 * Peer configuration commands.
 *
 *********************************************************************/

/*
 * dlsw_peer_disable()
 */

void dlsw_peer_disable (boolean sense)
{
    ulong ii;
    peerentry *peer;
    peerentry *next;
    leveltype status;


    if (!sense) {
        /*
         * If "no dlsw disable", schedule peers to connect.
         */
        peer = (peerentry *) disc_peerQ.qhead;
        for (; peer; peer = peer->next) {
            if ((peer->peer_type == CONF_PEER) && (!peer->remove) &&
                (lpeer->conn_mode != PASSIVE_MODE) && 
                (peer->conn_mode != PASSIVE_MODE) && (!peer->primary_peer)) {
                peer->conn_retries = 0;
                mgd_timer_start(&peer->conn_timer, 0);
            }
        }
        return;
    }

    status = raise_interrupt_level(ALL_DISABLE);

    /*
     * Call dlsw_peer_fsm() with CLOSE_CONNECTION for all
     * peers whether connected or not to allow peer fsm/db 
     * to manipulate timers.
     */
    peer = (peerentry *) disc_peerQ.qhead;
    while (peer) {
        next = peer->next;
        /*
         * Don't let pods live through a disable.
         */
        if (peer->peer_type == DEMAND_PEER)
            peer->remove = TRUE;
        dlsw_peer_fsm(peer, CLOSE_CONNECTION);
        peer = next;
    }

    peer = (peerentry *) wait_conn_peerQ.qhead;
    while (peer) {
        next = peer->next;
        dlsw_peer_fsm(peer, CLOSE_CONNECTION);
        peer = next;
    }

    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
        peer = (peerentry *) conn_dlx_peerQ[ii].qhead;
        while (peer) {
            next = peer->next;
            dlsw_peer_fsm(peer, CLOSE_CONNECTION);
            peer = next;
        }
    }

    peer = (peerentry *) conn_dlsw_peerQ.qhead;
    while (peer) {
        next = peer->next;
        dlsw_peer_fsm(peer, CLOSE_CONNECTION);
        peer = next;
    }

    reset_interrupt_level(status);
}

/* dlsw_peer_nvwrite()
 *
 * Check if peer is a backup peer
 * 
 * If so, take no action
 * If not, write the peer definition to NVRAM, as well as any
 *  backups to that peer
 */

static void dlsw_peer_nvwrite (parseinfo *csb, peerentry *peer)
{
    /* Only write if this is not a backup peer */
    if (!peer->primary_peer) {
        dlsw_nvgen_remote(csb, peer);

        /* If this peer has a backup, write it now */
        if (peer->backup_peer) {
            dlsw_nvgen_remote(csb, peer->backup_peer);
	}
    }
}

/*
 * dlsw_peer_command()
 *
 * global dlsw peer commands.
 */

void dlsw_peer_command (parseinfo *csb)
{
    ulong   i;
    peerentry  *peer;

    if (system_loading)
        return;

    if (!bridge_enable) {
        printf("\n%%DLSw not supported in this release");
        return;
    }

    if (csb->nvgen) {
        switch (csb->which) {
          case PEER_LOCAL:
            dlsw_nvgen_local(csb);
            break;

          case PEER_REMOTE:
            /* Connected DLX peers. */
            for (i = 0; i < DLSW_MAX_PGROUP; i++) {
                if ((peer = (peerentry *) conn_dlx_peerQ[i].qhead)) {
                    for (; peer; peer = peer->next) 
                        dlsw_peer_nvwrite(csb, peer);
                }
            }

            /* Connected DLSw peers. */
            if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
                for (; peer; peer = peer->next) 
		    dlsw_peer_nvwrite(csb, peer);
            }

            /* Peers waiting to act on capabilities */
            if ((peer = (peerentry *) wait_conn_peerQ.qhead)) {
                for (; peer; peer = peer->next) 
		    dlsw_peer_nvwrite(csb, peer);
            }

            /* Disconnected peers. */
            if ((peer = (peerentry *) disc_peerQ.qhead)) {
                for (; peer; peer = peer->next) 
		    dlsw_peer_nvwrite(csb, peer);
            }
            break;

          case PEER_PORT_LIST:
            dlsw_nvgen_port_list(csb);
            break;

          case PEER_RING_LIST:
            dlsw_nvgen_ring_list(csb);
            break;

          case PEER_BGROUP_LIST:
            dlsw_nvgen_bgroup_list(csb);
            break;

          case PEER_ICANREACH:
            dlsw_nvgen_icanreach(csb);
            break;

          case PEER_ICNR:
            dlsw_nvgen_icnr(csb);
            break;

          case PEER_POD_DEFAULTS:
            dlsw_nvgen_pod_defs(csb);
            break;

          case PEER_PROM_DEFAULTS:
             dlsw_nvgen_prompeer_defs(csb);
             break;

	   case PEER_FORCE_SSP:
            /*
             * Hidden command
             * nv_write(dlsw_force_ssp, "%s", csb->nv_command);
             */
            break;

          case PEER_BGROUP_ASSIGN:
	    dlsw_nvgen_bgroup_assign(csb);
            break;

          default:
            bad_parser_subcommand(csb, csb->which);
            break;

        }
        return;
    }

    switch (csb->which) {
      case PEER_LOCAL:
        parse_local_peer(csb);
        break;

      case PEER_REMOTE:
        parse_remote_peer(csb);
        break;

      case PEER_PORT_LIST:
        parse_port_list(csb);
        break;

      case PEER_RING_LIST:
        parse_ring_list(csb);
        break;

      case PEER_BGROUP_LIST:
        parse_bgroup_list(csb);
        break;

      case PEER_ICANREACH:
        parse_icanreach(csb);
        break;

      case PEER_ICNR:
        parse_icnr(csb);
        break;

      case PEER_POD_DEFAULTS:
        parse_pod_defs(csb);
        break;

      case PEER_PROM_DEFAULTS:
         parse_prompeer_defs(csb);
         break;

      case PEER_FORCE_SSP:
        /*
         * Hidden command
         * dlsw_force_ssp = csb->sense;
         */
        break;

      case PEER_BGROUP_ASSIGN:
	parse_bgroup_assign(csb);
        break;

      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}


/*
 * dlsw_nvgen_local()
 */

static void dlsw_nvgen_local (parseinfo *csb)
{
    if (lpeer) {
        nv_write(TRUE, "%s", csb->nv_command);
        nv_add(lpeer->ipaddr, " peer-id %i", lpeer->ipaddr);
        nv_add(lpeer->cap.group, " group %d", lpeer->cap.group);
        nv_add(lpeer->cap.border, " border");
        nv_add(lpeer->cap.cost != DLSW_NORM_PCOST, " cost %d",
               lpeer->cap.cost);
        nv_add(lpeer->largest_frame != RC_LFINITIAL, " lf %d",
               rc_code_to_bytes(lpeer->largest_frame));
        nv_add(lpeer->keepalive_int != DLSW_DEF_KEEP_INT,
               " keepalive %d", (lpeer->keepalive_int / ONESEC));
        nv_add(lpeer->conn_mode == PASSIVE_MODE, " passive");
        nv_add(lpeer->cap.pod_enabled, " promiscuous");
        nv_add(lpeer->cap.initial_pacing_window != DLSW_FC_INIT_WINDOW,
              " init-pacing-window %d", lpeer->cap.initial_pacing_window);
        nv_add(lpeer->max_pacing_window != DLSW_FC_MAX_WINDOW,
              " max-pacing-window %d", lpeer->max_pacing_window);
        nv_add(lpeer->cap.spoof_and_seg, " biu-segment");
    }
}


/*
 * dlsw_nvgen_remote()
 */

static void dlsw_nvgen_remote (parseinfo *csb, peerentry *peer)
{
    if (!lpeer)
        return;

    if ((peer->remove) || (peer->peer_type == PROM_PEER) ||
	(peer->peer_type == DEMAND_PEER) || (peer->peer_type == PEER_MAX_TYPE))
        return;

    nv_write(TRUE, "%s %d", csb->nv_command, 
	     (peer->ple ? peer->ple->list_num : 0));
    switch (peer->xport_type) {
      case DLSW_TCP:
        /*
         * local-ack is not an option is version 1.
         * nv_add(TRUE, " tcp %i%s", peer->ipaddr,
         *        (peer->local_ack ? "" : "pass-thru"));
         */
        nv_add(TRUE, " tcp %i", peer->ipaddr);
        break;
      case DLSW_FST:
        nv_add(TRUE, " fst %i", peer->ipaddr);
        break;
      case DLSW_IF:
        nv_add(TRUE, " interface %s%s", peer->outidb->hw_namestring,
               (peer->local_ack ? "" : " pass-thru"));
        break;
      case DLSW_FR:
        nv_add(TRUE, " frame-relay interface %s %d%s",
	       peer->swidb->namestring,
	       peer->fr_dlci,
	       (peer->local_ack ? "" : " pass-thru"));
        break;
      case DLSW_IF_LAN:
        nv_add(TRUE, " interface %s %e",
               peer->outidb->hw_namestring, peer->macaddr);
        break;
    }

    nv_add(peer->priority, " priority");
    nv_add(peer->largest_frame_def != RC_LFINITIAL, " lf %d",
           rc_code_to_bytes(peer->largest_frame));
    nv_add(peer->cost != DLSW_NORM_PCOST, " cost %d", peer->cost);
    if (peer->peer_type != DYNAMIC_PEER)
	 nv_add(peer->keepalive_int != lpeer->keepalive_int, 
		" keepalive %d", (peer->keepalive_int / ONESEC));
    nv_add(peer->max_tcp_qlen != DLSW_MAX_TCP_QLEN, " tcp-queue-max %d",
           peer->max_tcp_qlen);
    nv_add(peer->dlsw_lsap_out, " lsap-output-list %d", peer->dlsw_lsap_out);
    nv_add((peer->netbios_h_oacf != NULL), " host-netbios-out %s",
           peer->netbios_h_oacf->name);
    nv_add((peer->netbios_b_oacf != NULL), " bytes-netbios-out %s",
           peer->netbios_b_oacf->name);
    if (peer->primary_peer)
        nv_add(TRUE, " backup-peer %i", peer->primary_peer->ipaddr);
    if (peer->tcp_timeout && peer->xport_type == DLSW_TCP)
	 nv_add(peer->tcp_timeout != DLSW_KEEP_CNT_THOLD *
		DLSW_DEF_KEEP_INT, " timeout %d", 
		(peer->tcp_timeout / ONESEC));
    if ((peer->backup_linger != -1) && peer->primary_peer)
	 nv_add(TRUE, " linger %d", (peer->backup_linger / ONEMIN));
    if (peer->peer_type == DYNAMIC_PEER) {
	 nv_add(TRUE, " dynamic");
	 if (peer->no_llc)
	      nv_add(peer->no_llc != 5 * ONEMIN, " no-llc %d", 
		     peer->no_llc / ONEMIN);
	 if (peer->dynamic_inactivity > 0)
	      nv_add(TRUE, " inactivity %d", (peer->dynamic_inactivity /
					      ONEMIN));
    }
    if (peer->dlsw_dmac_outlist)
	 nv_add(TRUE, " dmac-output-list %d",
		peer->dlsw_dmac_outlist);
    if (!(is_ieee_zero(peer->dlsw_dmac_out)))
	 nv_add(TRUE, " dest-mac %e", peer->dlsw_dmac_out);

    nv_add(peer->conn_mode == PASSIVE_MODE, " passive");
}


/*
 * dlsw_nvgen_port_list()
 *
 * [no] dlsw port-list <list_num> <interface> <interface> ....
 */

static void dlsw_nvgen_port_list (parseinfo *csb)
{
    ulong ii;
    portlistentry *ple;

    for (ple = port_listQ.qhead; ple; ple = ple->next) {
        if (ple->portlist_len) {
            nv_write(TRUE, "%s %d", csb->nv_command,  ple->list_num);
            for (ii = 0; ii < ple->portlist_len; ii++) {
                nv_add(TRUE, " %s", ple->sw_idb[ii]->hwptr->hw_namestring);
            }
        }
    }
}


/*
 * dlsw_nvgen_ring_list()
 * [no] dlsw ring-list <list_num> rings <ring_no> <ring_no> ....
 */
static void dlsw_nvgen_ring_list (parseinfo *csb)
{
    int              ii;
    portlistentry    *ple;

    for (ple = port_listQ.qhead; ple; ple = ple->next) {
        if (ple->ringlist_len) {
            nv_write(TRUE, "%s %d rings", csb->nv_command, ple->list_num);
            for (ii = 0; ii < ple->ringlist_len; ii++) {
                nv_add(TRUE, " %d", ple->ring_no[ii]);
            }
        }
    }
}



/*
 * dlsw_nvgen_bgroup_list()
 * [no] dlsw bgroup-list <list_num> bgroups <bgroup_no> <bgroup_no> ....
 */
static void dlsw_nvgen_bgroup_list (parseinfo *csb)
{
    int              ii;
    portlistentry    *ple;

    for (ple = port_listQ.qhead; ple; ple = ple->next) {
        if (ple->bgrouplist_len) {
            nv_write(TRUE, "%s %d bgroups", csb->nv_command, ple->list_num);
            for (ii = 0; ii < ple->bgrouplist_len; ii++) {
                nv_add(TRUE, " %d", ple->bgroup_no[ii]);
            }
        }
    }
}




/*
 * dlsw_nvgen_icanreach()
 */

static void dlsw_nvgen_icanreach (parseinfo *csb)
{
    macentry_t *macentry;
    nbentry_t *nbentry;
    uchar cvsap_mask;
    ulong ii, jj;
    boolean got_one = FALSE;
    uchar counter = 0;


    if (!lpeer)
        return;

    nv_write(lpeer->cap.mac_exclusive, "%s mac-exclusive", csb->nv_command);
    nv_add(lpeer->icr_mac_excl_rem, " remote");
    nv_write(lpeer->cap.nb_exclusive, "%s netbios-exclusive", csb->nv_command);
    nv_add(lpeer->icr_nb_excl_rem, " remote");
    macentry = (macentry_t *) lpeer->cap.macQ.qhead;
    if (macentry) {
        for (; macentry; macentry = macentry->next)
            nv_write(TRUE, "%s mac-address %e mask %e", csb->nv_command, 
		     macentry->macaddr, macentry->macmask);
    }
    nbentry = (nbentry_t *) lpeer->cap.nbnameQ.qhead;
    if (nbentry) {
        for (; nbentry; nbentry = nbentry->next)
            nv_write(TRUE, "%s netbios-name %s", csb->nv_command, 
		     nbentry->nbname);
    }
    for (ii = 0; ii < SAP_CV_BYTES; ii++) {
        cvsap_mask = MIN_SAP_IN_BYTE;
        for (jj = 0; jj < 8; jj++) {
            if (lpeer->cap.supp_saps[ii] & cvsap_mask) {
                ++counter;
            }
        }
    }
    if ( (counter >= 128) || (lpeer->reach_flag != REACHABLE))
        return;

    for (ii = 0; ii < SAP_CV_BYTES; ii++) {
        cvsap_mask = MIN_SAP_IN_BYTE;
        for (jj = 0; jj < 8; jj++) {
            if (lpeer->cap.supp_saps[ii] & cvsap_mask) {
                if (!got_one) {
                    nv_write(TRUE, "%s sap", csb->nv_command);
                    got_one = TRUE;
                }
                nv_add(TRUE, " %2x", cvsap_to_sap(ii, cvsap_mask));
            }
            cvsap_mask = cvsap_mask >> 1;
        }
    }

}


/*
 * dlsw_nvgen_icnr()
 */

static void dlsw_nvgen_icnr (parseinfo *csb)
{
    uchar cvsap_mask;
    ulong ii, jj;
    boolean got_one = FALSE;

    if (!lpeer)
        return;

    if (lpeer->reach_flag != NOT_REACHABLE)
        return;

    for (ii = 0; ii < SAP_CV_BYTES; ii++) {
        cvsap_mask = MIN_SAP_IN_BYTE;
        for (jj = 0; jj < 8; jj++) {
            if (!(lpeer->cap.supp_saps[ii] & cvsap_mask)) {
                if (!got_one) {
                    nv_write(TRUE, "%s sap", csb->nv_command);
                    got_one = TRUE;
                }
                nv_add(TRUE, " %2x", cvsap_to_sap(ii, cvsap_mask));
            }
            cvsap_mask = cvsap_mask >> 1;
        }
    }
}


/*
 * dlsw_nvgen_pod_defs()
 */

static void dlsw_nvgen_pod_defs (parseinfo *csb)
{
    if (!lpeer)
        return;

    /*
     * If the defaults have not been changed, do nothing.
     */
    if ((lpeer->pod_defaults.xport_type == DLSW_TCP) &&
        (lpeer->pod_defaults.local_ack == TRUE) &&
        (lpeer->pod_defaults.priority == FALSE) &&
        (lpeer->pod_defaults.max_tcp_qlen == DLSW_MAX_TCP_QLEN) &&
        (lpeer->pod_defaults.cost == DLSW_NORM_PCOST) &&
        (lpeer->pod_defaults.keepalive_int == DLSW_DEF_KEEP_INT) &&
        (lpeer->pod_defaults.netbios_h_oacf == NULL) &&
        (lpeer->pod_defaults.netbios_b_oacf == NULL) &&
        (lpeer->pod_defaults.dlsw_lsap_out == 0) &&
        (lpeer->pod_defaults.pod_inactivity ==
                (DLSW_POD_DEF_DISC_DELAY * ONEMIN)) &&
        (lpeer->pod_defaults.largest_frame == RC_LFINITIAL) &&
	(lpeer->pod_defaults.dlsw_dmac_outlist == 0) &&
        (is_ieee_zero(lpeer->pod_defaults.dlsw_dmac_out))) {
        return;
    }

    nv_write(TRUE, "%s", csb->nv_command); 
    if (lpeer->pod_defaults.ple)
        nv_add(TRUE, " port-list %d", lpeer->pod_defaults.ple->list_num);
    if (lpeer->pod_defaults.xport_type == DLSW_TCP) {
        nv_add(lpeer->pod_defaults.priority, " priority");
    } else {
        nv_add(TRUE, " fst");
    }
    nv_add(lpeer->pod_defaults.cost != DLSW_NORM_PCOST, " cost %d",
	   lpeer->pod_defaults.cost);
    nv_add(lpeer->pod_defaults.keepalive_int != lpeer->keepalive_int,
           " keepalive %d", (lpeer->pod_defaults.keepalive_int / ONESEC));
    nv_add(lpeer->pod_defaults.max_tcp_qlen != DLSW_MAX_TCP_QLEN,
           " tcp-queue-max %d", lpeer->pod_defaults.max_tcp_qlen);
    nv_add(lpeer->pod_defaults.dlsw_lsap_out, " lsap-output-list %d",
	   lpeer->pod_defaults.dlsw_lsap_out);
    nv_add((lpeer->pod_defaults.netbios_h_oacf != NULL), 
	   " host-netbios-out %s", lpeer->pod_defaults.netbios_h_oacf->name);
    nv_add((lpeer->pod_defaults.netbios_b_oacf != NULL), 
	   " bytes-netbios-out %s", lpeer->pod_defaults.netbios_b_oacf->name);
    nv_add((lpeer->pod_defaults.pod_inactivity !=
            (DLSW_POD_DEF_DISC_DELAY * ONEMIN)),
           " inactivity %d", lpeer->pod_defaults.pod_inactivity / ONEMIN);
    nv_add(lpeer->pod_defaults.largest_frame != RC_LFINITIAL, " lf %d",
           rc_code_to_bytes(lpeer->pod_defaults.largest_frame));
    nv_add(lpeer->pod_defaults.dlsw_dmac_outlist, " dmac-output-list %d",
            lpeer->pod_defaults.dlsw_dmac_outlist);
    nv_add(!(is_ieee_zero(lpeer->pod_defaults.dlsw_dmac_out)), " dest-mac %e",
              lpeer->pod_defaults.dlsw_dmac_out);
}

/*
 * dlsw_nvgen_prompeer_defs()
 */

static void dlsw_nvgen_prompeer_defs (parseinfo *csb)
{
    if (!lpeer)
        return;

    /*
     * If the defaults have not been changed, do nothing.
     */
     if ((lpeer->prompeer_defaults.max_tcp_qlen == DLSW_MAX_TCP_QLEN) &&
         (lpeer->prompeer_defaults.cost == DLSW_NORM_PCOST) &&
         (lpeer->prompeer_defaults.keepalive_int == -1) &&
         (lpeer->prompeer_defaults.netbios_h_oacf == NULL) &&
         (lpeer->prompeer_defaults.netbios_b_oacf == NULL) &&
         (lpeer->prompeer_defaults.dlsw_lsap_out == 0) &&
         (lpeer->prompeer_defaults.largest_frame == RC_LFINITIAL) &&
         (lpeer->prompeer_defaults.dlsw_dmac_outlist == 0) &&
         (is_ieee_zero(lpeer->prompeer_defaults.dlsw_dmac_out))) {
         return;
      }

     nv_write(TRUE, "%s", csb->nv_command);
     nv_add(lpeer->prompeer_defaults.cost != DLSW_NORM_PCOST, " cost %d",
            lpeer->prompeer_defaults.cost);
     nv_add(lpeer->prompeer_defaults.keepalive_int != -1,
            " keepalive %d", (lpeer->prompeer_defaults.keepalive_int / ONESEC));
     nv_add(lpeer->prompeer_defaults.max_tcp_qlen != DLSW_MAX_TCP_QLEN,
            " tcp-queue-max %d", lpeer->prompeer_defaults.max_tcp_qlen);
     nv_add(lpeer->prompeer_defaults.dlsw_lsap_out, " lsap-output-list %d",
            lpeer->prompeer_defaults.dlsw_lsap_out);
     nv_add((lpeer->prompeer_defaults.netbios_h_oacf != NULL),
            " host-netbios-out %s", lpeer->prompeer_defaults.netbios_h_oacf->name);
     nv_add((lpeer->prompeer_defaults.netbios_b_oacf != NULL),
            " bytes-netbios-out %s", lpeer->prompeer_defaults.netbios_b_oacf->name);
     nv_add(lpeer->prompeer_defaults.largest_frame != RC_LFINITIAL, " lf %d",
            rc_code_to_bytes(lpeer->prompeer_defaults.largest_frame));
     nv_add(lpeer->prompeer_defaults.dlsw_dmac_outlist, " dmac-output-list %d",
            lpeer->prompeer_defaults.dlsw_dmac_outlist);
     nv_add(!(is_ieee_zero(lpeer->prompeer_defaults.dlsw_dmac_out)), " dest-mac %e",
         lpeer->prompeer_defaults.dlsw_dmac_out);
}


/*
 * d l s w _ n v g e n _ b g r o u p _ a s s i g n
 */

static boolean dlsw_bgroup_llc2_configured (dlsw_bgroup_t *dlsw_bgroup)
{
 
   if ( (dlsw_bgroup->llc2_akmax != AKMAX_DEFAULT) ||
        (dlsw_bgroup->llc2_idle_time != IDLE_DEFAULT) ||
        (dlsw_bgroup->llc2_wind != LOC_WIND_DEFAULT) ||
        (dlsw_bgroup->llc2_n2 != N2_DEFAULT) ||
        (dlsw_bgroup->llc2_T1_time != T1_DEFAULT) ||
        (dlsw_bgroup->llc2_tbusy_time != TBUSY_DEFAULT) ||
        (dlsw_bgroup->llc2_trej_time != TREJ_DEFAULT) ||
        (dlsw_bgroup->llc2_tpf_time != TPF_DEFAULT) ||
        (dlsw_bgroup->llc2_txQ_max != TXQ_MAX_DEFAULT) ||
        (dlsw_bgroup->llc2_akdelay_time != AKDELAY_DEFAULT) ||
        (dlsw_bgroup->llc2_xid_neg_val_time != XID_NEG_TIME) ||
        (dlsw_bgroup->llc2_xid_rtry_time != XID_RTRY_TIME) ) {
        return (TRUE);
   } else {
        return (FALSE);
   }
}

static void dlsw_nvgen_bgroup_assign (parseinfo *csb)
{
    dlsw_bgroup_t *dlsw_bgroup;

    if (!lpeer)			/* cannot configure a dlsw bridge-group */
        return;			/* if there isn't a dlsw lpeer command */

    for (dlsw_bgroup = (dlsw_bgroup_t *)dlsw_bgroupQ.qhead;
         dlsw_bgroup;
         dlsw_bgroup = dlsw_bgroup->next) {
       nv_write(TRUE, "%s %d", csb->nv_command, dlsw_bgroup->span_index);
       nv_add(dlsw_bgroup->sap_prilist_num, " sap-priority %d",
              dlsw_bgroup->sap_prilist_num);
       nv_add(dlsw_bgroup->sna_la_prilist_num, " locaddr-priority %d",
              dlsw_bgroup->sna_la_prilist_num);
       if (dlsw_bgroup_llc2_configured(dlsw_bgroup)) { 
           nv_add(TRUE, " llc2");
           nv_add(dlsw_bgroup->llc2_akmax != AKMAX_DEFAULT, " ack-max %d",
                  dlsw_bgroup->llc2_akmax);
           nv_add(dlsw_bgroup->llc2_idle_time != IDLE_DEFAULT, " idle-time %d",
                  dlsw_bgroup->llc2_idle_time);
           nv_add(dlsw_bgroup->llc2_n2 != N2_DEFAULT, " N2 %d",
                  dlsw_bgroup->llc2_n2);
           nv_add(dlsw_bgroup->llc2_T1_time != T1_DEFAULT, " t1-time %d",
                  dlsw_bgroup->llc2_T1_time);
           nv_add(dlsw_bgroup->llc2_trej_time != TREJ_DEFAULT, " trej-time %d",
                  dlsw_bgroup->llc2_trej_time);
           nv_add(dlsw_bgroup->llc2_tpf_time != TPF_DEFAULT, " tpf-time %d",
                  dlsw_bgroup->llc2_tpf_time);
           nv_add(dlsw_bgroup->llc2_txQ_max != TXQ_MAX_DEFAULT, " txq-max %d",
                  dlsw_bgroup->llc2_txQ_max);
           nv_add(dlsw_bgroup->llc2_wind != LOC_WIND_DEFAULT, 
                  " local-window %d", dlsw_bgroup->llc2_wind);
           nv_add(dlsw_bgroup->llc2_tbusy_time != TBUSY_DEFAULT, 
                  " tbusy-time %d", dlsw_bgroup->llc2_tbusy_time);
           nv_add(dlsw_bgroup->llc2_akdelay_time != AKDELAY_DEFAULT, 
                  " ack-delay-time %d", dlsw_bgroup->llc2_akdelay_time);
           nv_add(dlsw_bgroup->llc2_xid_neg_val_time != XID_NEG_TIME, 
                  " xid-neg-val-time %d", dlsw_bgroup->llc2_xid_neg_val_time);
           nv_add(dlsw_bgroup->llc2_xid_rtry_time != XID_RTRY_TIME, 
                  " xid-retry-time %d", dlsw_bgroup->llc2_xid_rtry_time);
       }
    }
}

/*
 * parse_local_peer()
 */

static void parse_local_peer (parseinfo *csb)
{
    ulong   lf_size;
    ushort  group_num;
    ulong   keep_int;
    uchar   conn_mode;
    ushort  cost;
    ipaddrtype ipaddr;
    dlsw_bgroup_t *dlsw_bgroup;

    /*
     * DLSw Local Peer for this box.
     */
    if (!csb->sense) {
        peer_remove_local();
        dlsw_local_disable();
        dlsw_ports_disable();
        if (dlsw_bgroupQ.qhead) {
            while ((dlsw_bgroup = p_dequeue(&dlsw_bgroupQ)))
                free(dlsw_bgroup);
	}
        TIMER_STOP(dlsw_start_time);
        return;
    }

    if (GETOBJ(int,1)) {
        ipaddr = GETOBJ(paddr,1)->ip_addr;
    } else {
        ipaddr = 0;
    }

    if (GETOBJ(int,2)) {
        if (!ipaddr) {
            printf("\n%%Must specify peer-id to belong to a group");
            return;
        }
        group_num = GETOBJ(int,3);
    } else if (GETOBJ(int,4)) {
        printf("\n%%Local peer must belong to a group to be a border peer");
        return;
    } else {
        group_num = 0;
    }

    if (GETOBJ(int,9))
        keep_int = GETOBJ(int,10) * ONESEC;
    else
        keep_int = DLSW_DEF_KEEP_INT;

    if (GETOBJ(int,11))
        conn_mode = PASSIVE_MODE;
    else
        conn_mode = NORMAL_MODE;

    if (GETOBJ(int,7))
        lf_size = GETOBJ(int,8);
    else
        lf_size = RC_LFINITIAL;

    if (GETOBJ(int,5))
        cost = GETOBJ(int,6);
    else
        cost = DLSW_NORM_PCOST;

    peer_create_local(ipaddr, lf_size, group_num, GETOBJ(int,4), 
                      cost, keep_int, conn_mode, GETOBJ(int,12),
                      GETOBJ(int,13), GETOBJ(int,14), GETOBJ(int,15));
    GET_TIMESTAMP(dlsw_start_time);
    return;
}


/*
 * parse_remote_peer()
 */

static void parse_remote_peer (parseinfo *csb)
{
    ulong   lf_code;
    ulong   keep_int;
    ushort  cost;
    ushort  fr_dlci;
    ulong   list_num;
    ushort  tcp_qlen;
    uchar   macaddr[IEEEBYTES];
    uchar   xport_type;
    ipaddrtype ipaddr;
    hwidbtype  *hwidb;
    idbtype    *swidb;
    peerentry  *peer;
    peerentry  *primary_peer = NULL;
    portlistentry *ple = NULL;
    netbios_acclisttype *netbios_bytes_list = NULL;
    netbios_acclisttype *netbios_host_list = NULL;
    ulong tcp_timeout;
    int backup_linger = -1;
    peer_t ptype;
    int dynamic_inactivity = -1;
    int no_llc = 0;
    char buff[DLSW_MAX_PEER_ID];
    uchar dlsw_dmac_out[IEEEBYTES];
    uchar   conn_mode;

    ieee_copy(zeromac, macaddr);
    ieee_copy(zeromac, dlsw_dmac_out);
    switch (GETOBJ(int,2)) {
      case DLSW_TCP:
        ipaddr = GETOBJ(paddr,1)->ip_addr;
        hwidb = NULL;
        swidb = NULL;
        xport_type = DLSW_TCP;
        fr_dlci = 0;
        break;
      case DLSW_IF:
        ipaddr = 0;
        hwidb = GETOBJ(idb,1)->hwptr;
        swidb = NULL;
        fr_dlci = 0;
        if (GETOBJ(idb,1)->hwptr->status & IDB_SERIAL) {
            xport_type = DLSW_IF;
        } else {
            if (GETOBJ(idb,1)->hwptr->status & IDB_IEEEMASK) {
                ieee_copy(GETOBJ(hwaddr,1)->addr, macaddr);
                xport_type = DLSW_IF_LAN;
            } else {
                printf("\n%%Invalid transport type, interface, or address");
                return;
            }
        }

        /* All direct encapsulation must specify pass-thru. The default is */
        /* local-ack.                                                      */
        if (GETOBJ(int,3)) {
          printf("\n%%For direct encapsulation, must specify pass-thru");
          return;
        }
        break;
      case DLSW_FST:
        ipaddr = GETOBJ(paddr,1)->ip_addr;
        hwidb = NULL;
        swidb = NULL;
        xport_type = DLSW_FST;
        fr_dlci = 0;
        break;
      case DLSW_FR:
        ipaddr = 0;
        swidb = GETOBJ(idb,1);
        hwidb = swidb->hwptr;
        fr_dlci = (ushort) GETOBJ(int, 14);
        xport_type = DLSW_FR;
        break;
      default:
        printf("\n%%Invalid transport type, interface, or address");
        return;
    }

    /*
     * "no remote-peer..." case 
     */
    if (!csb->sense) {

        switch(xport_type) {
          case DLSW_TCP:
          case DLSW_FST:
            peer = (peerentry *) peer_ip_to_handle(ipaddr);
            break;

          case DLSW_IF:
          case DLSW_IF_LAN:
            peer = (peerentry *) peer_hwidb_to_handle(hwidb);
            break;

          case DLSW_FR:
            peer = (peerentry *) peer_swidb_dlci_to_handle(swidb,fr_dlci);
            break;

          default:
            printf("\n%%Invalid transport type, interface, or address");
            return;           
        }

        if (!peer)
          return;

        if (peer->backup_peer) {
            printf("\n%%Must remove backup %s before removing primary peer.",
                   peer_str(peer->backup_peer, buff));
            return;
        }

        /*
         * TCP may block when a connection attempt is made.
         * If we are blocking on the connect, we can not remove
         * the peer, otherwise, when TCP unblocks, we will access
         * freed memory.
         */
        if (peer->active_open_wait) {
            printf("\nDLSw: %s is connecting - can not remove peer.",
                   peer_str(peer->backup_peer, buff));
            printf("\nIt may take up to 1 minute for this condition to clear.");
            return;
        }

        /*
         * If removing a backup peer, remove primary peer's
         * link to the backup.
         */
        if (peer->primary_peer)
            peer->primary_peer->backup_peer = NULL;

        peer_remove_remote(peer);
        return;
    }

    if (!lpeer) {
        printf("\n%%DLSw local-peer required before remote-peer.");
        return;
    }

    /* validate port list */
    if (GETOBJ(int,1)) {
        ple = find_port_list(GETOBJ(int,1));
        if (!ple) {
            printf("\n%%port-list %d does not exist.", GETOBJ(int,1));
            return;
        }
    }

    if ((xport_type != DLSW_TCP) && (GETOBJ(int,4))) {
        printf("\n%%Can only set priority for TCP connected peers");
        return;
    }

    tcp_timeout = GETOBJ(int,15) * ONESEC; 
    cost = GETOBJ(int,5);
    lf_code = GETOBJ(int,6);
    tcp_qlen = GETOBJ(int,9);
    list_num = GETOBJ(int,10);

    if (GETOBJ(int,7))
        keep_int = GETOBJ(int,8) * ONESEC;
    else
        keep_int = -1;
    
    if (GETOBJ(int,11)) {
        netbios_host_list = find_namedthing(&netbios_host_accQ, GETOBJ(string,1));
        if (netbios_host_list == NULL) {
            printf("\n%%Netbios host list %s not found", GETOBJ(string,1));
            return;
        }
    }

    if (GETOBJ(int,12)) {
        netbios_bytes_list = find_namedthing(&netbios_bytes_accQ, GETOBJ(string,2));
        if (netbios_bytes_list == NULL) {
            printf("\n%%Netbios bytes list %s not found", GETOBJ(string,2));
            return;
        }
    }

    if (GETOBJ(int,13)) {
        primary_peer = (peerentry *)
                       peer_ip_to_handle(GETOBJ(paddr,2)->ip_addr);
        if (!primary_peer) {
            printf("\n%%Primary peer by ip address %i does not exist", 
                   GETOBJ(paddr,2)->ip_addr);
            return;
        } else if ((primary_peer->xport_type != DLSW_TCP) && 
                    (primary_peer->xport_type != DLSW_FST)){
            printf("\n%%Primary peer must use TCP encapsulation");
            return;
        } else if ((primary_peer->backup_peer != NULL) &&
                   (primary_peer->backup_peer != 
                                   (peerentry *) peer_ip_to_handle(ipaddr))) {
            printf("\n%%Primary peer already has backup defined");
            return;
        } else if (primary_peer->primary_peer != NULL) {
            printf("\n%%Cannot configure backup peer for another backup peer");
            return;
        } else if (GETOBJ(int,18) != -1) 
	     backup_linger = GETOBJ(int,18) * ONEMIN;
	else
	     backup_linger = -1;
    }

    if (GETOBJ(int,16)) {
	 if (xport_type != DLSW_TCP){
	      printf("\n%% Can only configure dynamic for TCP remote peers");
	      return;
	 } else if (primary_peer) {
	      printf("\n%% Cannot configure a backup as dynamic");  
	      return;
	 } else {
              if (GETOBJ(int,17) != -1)
		  dynamic_inactivity = GETOBJ(int,17) * ONEMIN;
              else 
		  dynamic_inactivity = GETOBJ(int,17);

	      if (GETOBJ(int, 19)) {
		  if (dynamic_inactivity != -1) {
		       printf("\n%% Cannot specify both inactivity and no-llc.");
		       return;
		  } else 
		       no_llc = GETOBJ(int, 19) * ONEMIN;
	      }
	 }
	 ptype = DYNAMIC_PEER;
    } else
	 ptype = CONF_PEER;

    if (GETOBJ(hwaddr,2))
	 ieee_copy(GETOBJ(hwaddr,2)->addr, dlsw_dmac_out);

    if (GETOBJ(int,21))
        conn_mode = PASSIVE_MODE;
    else
        conn_mode = NORMAL_MODE;
    
    peer_create_remote(fr_dlci, ipaddr, ptype, ple, xport_type,
              GETOBJ(int,3), GETOBJ(int,4), cost, lf_code, keep_int, 
              list_num, netbios_host_list, netbios_bytes_list,
              tcp_qlen, hwidb, macaddr, primary_peer, tcp_timeout,
	      backup_linger, dynamic_inactivity, no_llc,
	      GETOBJ(int, 20), dlsw_dmac_out, swidb, conn_mode);
    return;
}


/*
 * parse_port_list()
 *
 * [no] dlsw port-list <list-num> <interface> <interface> ...
 *
 * OBJ(int,1) = <list_num>
 * OBJ(int,2) = number of hwidb pointers in uiarray
 * uiarray = array of hwidb's
 */

static void parse_port_list (parseinfo *csb)
{
    ulong ii;
    peerentry *peer;
    portlistentry *ple;
    char buff[DLSW_MAX_PEER_ID];

    ple = find_port_list(GETOBJ(int,1));
    if (!csb->sense) {
        if (!ple)
            printf("\nport-list %d does not exist", GETOBJ(int,1));
        else if ((peer = peer_using_port_list(GETOBJ(int,1))))
            printf("\nlist %d is being used by %s",
                   GETOBJ(int,1), peer_str(peer, buff));
        else
            remove_port_list(ple, GETOBJ(int,1));
        return;
    }

    if (!ple) {
        /*
         * Create a new portlistentry.
         */
        ple = malloc(sizeof(portlistentry));
        if (!ple) {
            return;
        }
        ple->list_num = GETOBJ(int,1);
        p_enqueue(&port_listQ, ple);
    } 

    /*
     * Whether the ple previously existed or is new, fill in the info.
     */
    ple->portlist_len = GETOBJ(int,2);
    for (ii = 0; ii < GETOBJ(int,2); ii++) {
        ple->sw_idb[ii] = (idbtype *) csb->uiarray[ii];
    }
    return;
}




/*
 * parse_ring_list()
 *
 * [no] dlsw ring-list <list-num> rings <ring_number> <ring_number> ...
 */

static void parse_ring_list (parseinfo *csb)
{
    int              ii, list_number;
    peerentry        *peer;
    portlistentry    *ple;
    char             buff[DLSW_MAX_PEER_ID];

    list_number = GETOBJ(int, 1);
    ple = find_port_list(list_number);
    if (!csb->sense) {
        if (!ple) {
            printf("\nring-list %d does not exist", list_number);
        } else if ((peer = peer_using_port_list(list_number))) {
            printf("\nlist %d is being used by %s",
                   list_number, peer_str(peer, buff));
        } else {
            remove_ring_list(ple, list_number);
        }
        return;
    }

    if (!ple) {
        /*
         * Create a new portlistentry.
         */
        ple = malloc(sizeof(portlistentry));
        if (!ple) {
            return;
        }
        ple->list_num = list_number;
        p_enqueue(&port_listQ, ple);
    } 

    /*
     * Whether the ple previously existed or is new, fill in the info.
     */
    ple->ringlist_len = GETOBJ(int,2);
    for (ii = 0; ii < ple->ringlist_len; ii++) {
        ple->ring_no[ii] = csb->uiarray[ii];
    }

    return;
}




/*
 * parse_bgroup_list()
 *
 * [no] dlsw bgroup-list <list-num> bgroups <bgroup_number> <bgroup_number> ...
 */

static void parse_bgroup_list (parseinfo *csb)
{
    int              ii, list_number;
    peerentry        *peer;
    portlistentry    *ple;
    char             buff[DLSW_MAX_PEER_ID];

    list_number = GETOBJ(int, 1);
    ple = find_port_list(list_number);
    if (!csb->sense) {
        if (!ple) {
            printf("\nbgroup-list %d does not exist", list_number);
        } else if ((peer = peer_using_port_list(list_number))) {
            printf("\nlist %d is being used by %s",
                   list_number, peer_str(peer, buff));
        } else {
            remove_bgroup_list(ple, list_number);
        }
        return;
    }

    if (!ple) {
        /*
         * Create a new portlistentry.
         */
        ple = malloc(sizeof(portlistentry));
        if (!ple) {
            return;
        }
        ple->list_num = list_number;
        p_enqueue(&port_listQ, ple);
    } 

    /*
     * Whether the ple previously existed or is new, fill in the info.
     */
    ple->bgrouplist_len = GETOBJ(int,2);
    for (ii = 0; ii < ple->bgrouplist_len; ii++) {
        ple->bgroup_no[ii] = csb->uiarray[ii];
    }

    return;
}




/*
 * parse_icanreach()
 */

static void parse_icanreach (parseinfo *csb)
{
    macentry_t *macentry;
    nbentry_t *nbentry;
    ulong ii;
    uchar mask;

    if (!lpeer) {
        printf("\n%%DLSw local-peer required before icanreach.");
        return;
    }

    switch (GETOBJ(int, 1)) {
      case ICR_MAC_EXCL:
        lpeer->cap.mac_exclusive = csb->sense;
        if (csb->sense) {
            lpeer->icr_mac_excl_rem = GETOBJ(int,4);
        } else {
            lpeer->icr_mac_excl_rem = FALSE;
        }
        break;

      case ICR_NB_EXCL:
        lpeer->cap.nb_exclusive = csb->sense;
        if (csb->sense) {
            lpeer->icr_nb_excl_rem = GETOBJ(int,4);
        } else {
            lpeer->icr_nb_excl_rem = FALSE;
        }
        break;

      case ICR_MAC:
        macentry = (macentry_t *) lpeer->cap.macQ.qhead;
        for (; macentry; macentry = macentry->next) {
            if (ieee_equal(GETOBJ(hwaddr,1)->addr, macentry->macaddr))
                break;
        }
        if (csb->sense) {
            if (macentry) {
                printf("\nmac addr %e already exists", macentry->macaddr);
                return;
            }
            macentry = malloc(sizeof(macentry_t));
            if (!macentry) {
                return;
            }
            ieee_copy(GETOBJ(hwaddr,1)->addr, macentry->macaddr);
            if (GETOBJ(int,2))
                ieee_copy(GETOBJ(hwaddr,2)->addr, macentry->macmask);
            else
                ieee_copy(baddr, macentry->macmask);
            p_enqueue(&lpeer->cap.macQ, macentry);
        } else {
            if (macentry) {
                p_unqueue(&lpeer->cap.macQ, macentry);
                free(macentry);
            } else {
                printf("\nCan not deconfigure mac address %e - does not exist",
                       GETOBJ(hwaddr,1)->addr);
                return;
            }
        }
        break;

      case ICR_NBNAME:
        nbentry = (nbentry_t *) lpeer->cap.nbnameQ.qhead;
        for (; nbentry; nbentry = nbentry->next) {
            if (wc_str_cmp(nbentry->nbname, GETOBJ(string,1)) == 0)
                break;
        }
        if (csb->sense) {
            if (nbentry) {
                printf("\nnetbios name %s already exists", nbentry->nbname);
                return;
            }
            nbentry = malloc(sizeof(nbentry_t));
            if (!nbentry) {
                printf(nomemory);
                return;
            }
            nbentry->len = strlen(GETOBJ(string,1));
	    if (nbentry->len > NETBIOS_MAX_NAME_LEN) {
		printf("\nNetbios name is too long");
		free(nbentry);
		return;
	    }
            bcopy(GETOBJ(string,1), nbentry->nbname, nbentry->len);
            nbentry->nbname[nbentry->len] = '\0';
            peer_build_netbios_mask(nbentry);
            p_enqueue(&lpeer->cap.nbnameQ, nbentry);
        } else {
            if (nbentry) {
                p_unqueue(&lpeer->cap.nbnameQ, nbentry);
                free(nbentry);
            } else {
                printf("\nCan not deconfigure netBios name %s - does not exist", 
                       GETOBJ(string,1));
                return;
            }
        }
        break;
      case ICR_SAPS:
        lpeer->reach_flag = REACHABLE;
        csm_clear_sap_entries(USER_CONFIGURED);
        if (!csb->sense)
                mask = ~0;
        else
                mask = 0;

        for (ii = 0; ii < SAP_CV_BYTES; ii++) {
                lpeer->cap.supp_saps[ii] = mask;
        }

        for (ii = 0; ii < GETOBJ(int,3); ii++) {
            if (csb->uiarray[ii] & ODD_SAP_INDICATOR) {
                printf("\n%%Odd SAP %x ignored", csb->uiarray[ii]);
            } else {
                peer_update_sup_sap(1, lpeer->cap.supp_saps,
                                    csb->uiarray[ii]);
                csm_update_sap_entry(SET, USER_CONFIGURED,
                                       csb->uiarray[ii], REACHABLE, 0);
            }
        }
        break;
    }
    send_runtime_capabilities();
}


/*
 * peer_build_netbios_mask()
 */

void peer_build_netbios_mask (nbentry_t *nbentry)
{
    char *n, *m;
    uchar i;

    /*
     * Build wildcard mask where don't cares are blanks.
     */
    for (i = 0, n = nbentry->nbname, m = nbentry->nbmask;
         i < NETBIOS_MAX_NAME_LEN; i++, n++, m++) {
        if ((*n == '?') || (*n == '\0'))
            *m = ' ';
        else
            *m = *n;
    }
    if (nbentry->nbname[nbentry->len -1] == '*') {
        nbentry->nbmask[i] = ' ';
        nbentry->wildterm = TRUE;
    } else 
        nbentry->wildterm = FALSE;
}


/*
 * parse_icnr()
 */

static void parse_icnr (parseinfo *csb)
{
    ulong ii;

    if (!lpeer) {
        printf("\n%%DLSw local-peer required before incannotreach.");
        return;
    }

    if (GETOBJ(int,1) == PEER_ICNR_SAP) {
        lpeer->reach_flag = NOT_REACHABLE;
        csm_clear_sap_entries(USER_CONFIGURED);
        for (ii = 0; ii < SAP_CV_BYTES; ii++)
            lpeer->cap.supp_saps[ii] = ~0;

        for (ii = 0; ii < GETOBJ(int,2); ii++) {
            if (csb->uiarray[ii] & ODD_SAP_INDICATOR) {
                printf("\n%%Odd SAP %x ignored", csb->uiarray[ii]);
            } else {
                peer_update_sup_sap(!csb->sense, lpeer->cap.supp_saps, 
                                    csb->uiarray[ii]);
                csm_update_sap_entry(SET, USER_CONFIGURED, 
                                       csb->uiarray[ii], NOT_REACHABLE, 0);
            }
        }
        send_runtime_capabilities();
    } 
}


/*
 * parse_pod_defs()
 */

static void parse_pod_defs (parseinfo *csb)
{
    portlistentry *ple = NULL;
    netbios_acclisttype *netbios_bytes_list = NULL;
    netbios_acclisttype *netbios_host_list = NULL;

    if (!lpeer) {
        printf("\n%%DLSw local-peer required before peer-on-demand defaults.");
        return;
    }

    if (!csb->sense) {
	init_pod_defaults();
        return;
    }

    if (GETOBJ(int,1)) {
        ple = find_port_list(GETOBJ(int,1));
        if (!ple) {
            printf("\n%%port-list %d does not exist.", GETOBJ(int,1));
            return;
        }
    }

    if (GETOBJ(int,9)) {
        netbios_host_list = find_namedthing(&netbios_host_accQ, GETOBJ(string,1));
        if (netbios_host_list == NULL) {
            printf("\n%%Netbios host list %s not found", GETOBJ(string,1));
            return;
        }
    }

    if (GETOBJ(int,10)) {
        netbios_bytes_list = find_namedthing(&netbios_bytes_accQ, GETOBJ(string,2));
        if (netbios_bytes_list == NULL) {
            printf("\n%%Netbios bytes list %s not found", GETOBJ(string,2));
            return;
        }
    }

    lpeer->pod_defaults.ple = ple;
    lpeer->pod_defaults.xport_type = GETOBJ(int,2);
    lpeer->pod_defaults.max_tcp_qlen = GETOBJ(int,3);
    lpeer->pod_defaults.priority = GETOBJ(int,4);
    lpeer->pod_defaults.cost = GETOBJ(int,5);
    if (GETOBJ(int,6))
        lpeer->pod_defaults.keepalive_int = GETOBJ(int,7) * ONESEC;
    else
        lpeer->pod_defaults.keepalive_int = DLSW_DEF_KEEP_INT;
    lpeer->pod_defaults.netbios_h_oacf = netbios_host_list;
    lpeer->pod_defaults.netbios_b_oacf = netbios_bytes_list;
    lpeer->pod_defaults.dlsw_lsap_out = GETOBJ(int,8);
    lpeer->pod_defaults.pod_inactivity = GETOBJ(int,11) * ONEMIN;
    lpeer->pod_defaults.largest_frame = GETOBJ(int,12);
    lpeer->pod_defaults.dlsw_dmac_outlist = GETOBJ(int,13);

    if (GETOBJ(hwaddr,1))
         ieee_copy(GETOBJ(hwaddr,1)->addr, lpeer->pod_defaults.dlsw_dmac_out);
}

/*
 * parse_prompeer_defs()
 */

static void parse_prompeer_defs (parseinfo *csb)
{
    netbios_acclisttype *netbios_bytes_list = NULL;
    netbios_acclisttype *netbios_host_list = NULL;

    if (!lpeer) {
        printf("\n%%DLSw local-peer required before prom-peer defaults.");
        return;
    }

    if (!csb->sense) {
        init_prompeer_defaults();
        return;
    }

    if (GETOBJ(int,6)) {
        netbios_host_list = find_namedthing(&netbios_host_accQ, GETOBJ(string,1));
        if (netbios_host_list == NULL) {
            printf("\n%%Netbios host list %s not found", GETOBJ(string,1));
            return;
        }
    }

    if (GETOBJ(int,7)) {
        netbios_bytes_list = find_namedthing(&netbios_bytes_accQ, GETOBJ(string,2));
        if (netbios_bytes_list == NULL) {
            printf("\n%%Netbios bytes list %s not found", GETOBJ(string,2));
            return;
        }
    }

    lpeer->prompeer_defaults.max_tcp_qlen = GETOBJ(int,1);
    lpeer->prompeer_defaults.cost = GETOBJ(int,2);
    if (GETOBJ(int,3))
        lpeer->prompeer_defaults.keepalive_int = GETOBJ(int,4) * ONESEC;
    else
        lpeer->prompeer_defaults.keepalive_int = -1;
    lpeer->prompeer_defaults.netbios_h_oacf = netbios_host_list;
    lpeer->prompeer_defaults.netbios_b_oacf = netbios_bytes_list;
    lpeer->prompeer_defaults.dlsw_lsap_out = GETOBJ(int,5);
    lpeer->prompeer_defaults.largest_frame = GETOBJ(int,8);
    lpeer->prompeer_defaults.dlsw_dmac_outlist = GETOBJ(int,9);

    if (GETOBJ(hwaddr,1))
         ieee_copy(GETOBJ(hwaddr,1)->addr, lpeer->prompeer_defaults.dlsw_dmac_out);

    return;
}


/*
 * p a r s e _ b g r o u p _ a s s i g n ( )
 *
 * Link DLSw switching entity to a transparent bridging bridge group.
 *
 * Create a new DLSw/Ethernet virtual interface and tie it into an
 * existing bridge group. Routine does not assume that existing bridge
 * structure is in place.
 *
 * Resulting configuration (with desired fields enclosed in [])

                 ******************
                 *                *
                 *   D  L  S  w   *
                 *                *
                 ******************
                         #
                         #
                   #############
	           # DLSw Vidb #
                   #############
                         #
                         #
      --------------*#####   <--- [lpeer->span_index]
                    |
                    |
       (Ethernet)   |
                    |
                    |

*/


static void parse_bgroup_assign (parseinfo *csb)
{
    ushort span_index;
    dlsw_bgroup_t *dlsw_bgroup;

    if (!lpeer) {
        printf("\n%%DLSw local-peer required before modification of bridge group number.");
        return;
    }

    span_index = GETOBJ(int,1);

    if (!csb->sense) {

        /* clear previously defined dlsw bridge group */
        
        dlsw_bgroup = dlsw_get_bgroup(span_index);
        if (dlsw_bgroup) {
            dlsw_set_bgroup_params(dlsw_bgroup->tbridge_idb->hwptr, 
                                                     csb->sense, span_index);
            unqueue(&dlsw_bgroupQ, dlsw_bgroup);
            free(dlsw_bgroup);
	}
        else
            printf("\n%%DLSw bridge group %d not configured", span_index);
    }
    else {		
        
        if (dlsw_get_bgroup(span_index)) {
	    printf("\n%%DLSw already attached to the transparent bridge group.");
	    return;
	}

        dlsw_bgroup = malloc_named(sizeof(dlsw_bgroup_t), "DLSW-BGROUP");
        if (!dlsw_bgroup) {
            printf(nomemory);
            return;
        }
        enqueue(&dlsw_bgroupQ, dlsw_bgroup);
   
        dlsw_bgroup->span_index = span_index;
        dlsw_bgroup->sap_prilist_num = GETOBJ(int,2);
        dlsw_bgroup->sna_la_prilist_num = GETOBJ(int,3);
        dlsw_bgroup->llc2_akmax            = (GETOBJ(int,4) ? 
                                              GETOBJ(int,4) : AKMAX_DEFAULT);
        dlsw_bgroup->llc2_akdelay_time     = (GETOBJ(int,5) ?
                                              GETOBJ(int,5) : AKDELAY_DEFAULT);
        dlsw_bgroup->llc2_idle_time        = (GETOBJ(int,6) ?
                                              GETOBJ(int,6) : IDLE_DEFAULT);
        dlsw_bgroup->llc2_wind             = (GETOBJ(int,7) ?
                                              GETOBJ(int,7) : LOC_WIND_DEFAULT);
        dlsw_bgroup->llc2_n2               = (GETOBJ(int,8) ? 
                                              GETOBJ(int,8) : N2_DEFAULT);
        dlsw_bgroup->llc2_T1_time          = (GETOBJ(int,9) ?
                                              GETOBJ(int,9) : T1_DEFAULT);
        dlsw_bgroup->llc2_tbusy_time       = (GETOBJ(int,10) ?
                                              GETOBJ(int,10) : TBUSY_DEFAULT);
        dlsw_bgroup->llc2_trej_time        = (GETOBJ(int,11) ?
                                              GETOBJ(int,11) : TREJ_DEFAULT);
        dlsw_bgroup->llc2_tpf_time         = (GETOBJ(int,12) ?
                                              GETOBJ(int,12) : TPF_DEFAULT);
        dlsw_bgroup->llc2_xid_neg_val_time = (GETOBJ(int,13) ?
                                              GETOBJ(int,13) : XID_NEG_TIME);
        dlsw_bgroup->llc2_xid_rtry_time    = (GETOBJ(int,14) ?
                                              GETOBJ(int,14) : XID_RTRY_TIME);
        dlsw_bgroup->llc2_txQ_max          = (GETOBJ(int,15) ?
                                              GETOBJ(int,15) : TXQ_MAX_DEFAULT);

        dlsw_set_bgroup_params(NULL, csb->sense, span_index);
    }
}


/*
 * cvsap_to_sap()
 */

static uchar cvsap_to_sap (uchar byte_index, uchar bit_mask) 
{
    uchar sap;
    ulong ii;

    sap = byte_index * 16;
    for (ii = 0; ii < 8; ii++) {
        if (bit_mask == MIN_SAP_IN_BYTE)
            break;
        bit_mask = bit_mask << 1;
    }
    return(sap + (ii * 2));
}


/*
 * peer_update_sup_sap()
 *
 * if 'supp' is TRUE, support 'sap'.
 */

static void peer_update_sup_sap (boolean supp, uchar *sap_array, uchar sap)
{
    uchar byte_index;
    uchar bit_index;
    uchar bit_mask = MIN_SAP_IN_BYTE;

    byte_index = sap >> 4;
    bit_index = (sap >> 1) % 8;
    bit_mask = bit_mask >> bit_index;
    if (supp)
        sap_array[byte_index] |= bit_mask; 
    else 
        sap_array[byte_index] &= ~bit_mask; 
}


/**********************************************************************
 *
 * Peer debug commands.
 *
 *********************************************************************/

/*
 * peer_debug_cmd()
 *
 * If peer is NULL, indicates debug scope is "all".
 */

void peer_debug_cmd (boolean prt_msg, peerhandle_t peer_handle,
                     parseinfo *csb, boolean sense)
{
    peerentry *peer = (peerentry *) peer_handle;
    ulong ii, jj;
    peerentry *p;
    boolean debug_fst_seq = FALSE;
    boolean debug_fast_pak = FALSE;
    boolean debug_fast_err = FALSE;
    char buff[DLSW_MAX_PEER_ID];

    dlsw_peer_debug = FALSE; 
    if (!peer) {
        peer_debug_all = sense;
        dlsw_peer_debug = sense;
    } else if (csb && sense) {
        debug_fst_seq = GETOBJ(int,3);
        debug_fast_pak = GETOBJ(int,4);
        debug_fast_err = GETOBJ(int,5);
    }

    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
        if ((p = (peerentry *) conn_dlx_peerQ[ii].qhead)) {
            for (; p; p = p->next) {
                if (peer) {
                    if (p == peer) {
                        p->debug = sense;
                        p->debug_fst_seq = debug_fst_seq;
                        p->debug_fast_pak = debug_fast_pak;
                        p->debug_fast_err = debug_fast_err;
                        if (p->priority) {
                            for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++)
                                peer->priority_peers[jj]->debug = sense;
                        }
                    }
                } else {
                    p->debug = sense;
                    p->debug_fst_seq = FALSE;
                    p->debug_fast_pak = FALSE;
                    p->debug_fast_err = FALSE;
                    if (p->priority) {
                        for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++)
                            p->priority_peers[jj]->debug = sense;
                    }
                }
                if (p->debug)
                    dlsw_peer_debug = TRUE; 
            }
        }
    }
    if ((p = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; p; p = p->next) {
            if (peer) {
                if (p == peer)
                    p->debug = sense;
            } else
                p->debug = sense;
            if (p->debug)
                dlsw_peer_debug = TRUE;
        }
    }

    if ((p = (peerentry *) wait_conn_peerQ.qhead)) {
        for (; p; p = p->next) {
            if (peer) {
                if (p == peer) {
                    p->debug = sense;
                    p->debug_fst_seq = debug_fst_seq;
                    p->debug_fast_pak = debug_fast_pak;
                    p->debug_fast_err = debug_fast_err;
                    if (p->priority) {
                        for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++)
                            p->priority_peers[jj]->debug = sense;
                    }
                }
            } else {
                p->debug = sense;
                p->debug_fst_seq = FALSE;
                p->debug_fast_pak = FALSE;
                p->debug_fast_err = FALSE;
                if (p->priority) {
                    for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++)
                        p->priority_peers[jj]->debug = sense;
                }
            }

            if (p->debug)
                dlsw_peer_debug = TRUE;
        }
    }

    if ((p = (peerentry *) disc_peerQ.qhead)) {
        for (; p; p = p->next) {
            if (peer) {
                if (p == peer) {
                    p->debug = sense;
                    p->debug_fst_seq = debug_fst_seq;
                    p->debug_fast_pak = debug_fast_pak;
                    p->debug_fast_err = debug_fast_err;
                    if (p->priority) {
                        for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++)
                            peer->priority_peers[jj]->debug = sense;
                    }
                }
            } else {
                p->debug = sense;
                p->debug_fst_seq = FALSE;
                p->debug_fast_pak = FALSE;
                p->debug_fast_err = FALSE;
                if (p->priority) {
                    for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++)
                        p->priority_peers[jj]->debug = sense;
                }
            }

            if (p->debug)
                dlsw_peer_debug = TRUE;
        }
    }
    if (peer) {
        if (peer->debug_fst_seq) {
            printf ("\nDLSw peer fst sequence debugging for %s is on",
                    peer_str(peer, buff));
        } 
        if (peer->debug_fast_pak) {
            printf ("\nDLSw peer fast switch packet debugging for %s is on",
                    peer_str(peer, buff));
        } 
        if (peer->debug_fast_err) {
            printf ("\nDLSw peer fast switch error debugging for %s is on",
                    peer_str(peer, buff));
        }

        printf ("\nDLSw basic debugging for %s is %s", 
                peer_str(peer, buff), peer->debug ? "on" : "off");
    } else if (prt_msg) {
        printf("\nDLSw peer debugging is %s", dlsw_peer_debug ? "on" : "off");
    }
}


void show_peer_debug ()
{
    ulong ii;
    peerentry *peer;
    char buff[DLSW_MAX_PEER_ID];

    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[ii].qhead)) {
            for (; peer; peer = peer->next) {
                if (peer->debug_fst_seq)
                    printf ("\n  DLSw peer fst sequence debugging for %s is on",
                            peer_str(peer, buff));
                if (peer->debug_fast_pak)
                    printf ("\n  DLSw peer fast switch packet debugging for %s is on",
                            peer_str(peer, buff));
                if (peer->debug_fast_err)
                    printf ("\n  DLSw peer fast switch error debugging for %s is on",
                            peer_str(peer, buff));
                if (peer->debug)
                    printf ("\n  DLSw basic debugging for %s is on", 
                            peer_str(peer, buff));
            }
        }
    }
    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->debug_fst_seq) 
                printf ("\n  DLSw peer fst sequence debugging for %s is on",
                        peer_str(peer, buff));
            if (peer->debug_fast_pak) 
                printf ("\n  DLSw peer fast switch packet debugging for %s is on",
                        peer_str(peer, buff));
            if (peer->debug_fast_err)
                printf ("\n  DLSw peer fast switch error debugging for %s is on",
                        peer_str(peer, buff));
            if (peer->debug)
                printf ("\n  DLSw basic debugging for %s is on",  
                        peer_str(peer, buff));
        }
    }
    if ((peer = (peerentry *) wait_conn_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->debug_fst_seq)
                printf ("\n  DLSw peer fst sequence debugging for %s is on",
                        peer_str(peer, buff));
            if (peer->debug_fast_pak)
                printf ("\n  DLSw peer fast switch packet debugging for %s is on",
                        peer_str(peer, buff));
            if (peer->debug_fast_err)
                printf ("\n  DLSw peer fast switch error debugging for %s is on",
                        peer_str(peer, buff));
            if (peer->debug)
                printf ("\n  DLSw basic debugging for %s is on",
                        peer_str(peer, buff));
        }
    }
    if ((peer = (peerentry *) disc_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->debug_fst_seq)     
                printf ("\n  DLSw peer fst sequence debugging for %s is on",
                        peer_str(peer, buff));
            if (peer->debug_fast_pak)     
                printf ("\n  DLSw peer fast switch packet debugging for %s is on",
                        peer_str(peer, buff));
            if (peer->debug_fast_err)
                printf ("\n  DLSw peer fast switch error debugging for %s is on",
                        peer_str(peer, buff));
            if (peer->debug)
                printf ("\n  DLSw basic debugging for %s is on",      
                        peer_str(peer, buff));
        }
    }
}


/**********************************************************************
 *
 * Display routines used to support "show" commands.
 *
 *********************************************************************/

/*
 * show_remote_stats()
 */
void show_remote_stats (peerhandle_t  peer_handle)
{
    peerentry  *peer = (peerentry  *) peer_handle;
    ulong ii;
    ulong jj;

    printf("\nPeers:                state     pkts_rx   pkts_tx  type  drops ckts TCP   uptime");

    if (peer) {
        print_peer_info(peer, 0);
        if (peer->priority) {
            for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++)
                print_peer_info(peer->priority_peers[jj], jj);
        }
        return;
    }

    automore_enable(NULL);
    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[ii].qhead)) {
            for (; peer; peer = peer->next) {
                mem_lock(peer);
                peer->change_flag = FALSE;
                if (peer->priority) {
                    for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++) {
                        mem_lock(peer->priority_peers[jj]);
                    }
                }
                print_peer_info(peer, 0);
                if (peer->priority) {
                    for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++) {
                        print_peer_info(peer->priority_peers[jj], jj);
                        free(peer->priority_peers[jj]);
                    }
                }
                automore_conditional(PEER_AUTOMORE_LINES);
                /*
                 * the change flag is set when peer moves to different
                 * queue or when the peer structure is deleted
                 */
                if (peer->change_flag) {
                    printf("\nPeers may have changed.... Retry command");
                    free(peer);
                    automore_disable();
                    return;
                }
                free(peer);
            }
        }
    }

    if ((peer = (peerentry *) wait_conn_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            mem_lock(peer);
            peer->change_flag = FALSE;
            if (peer->priority) {
                for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++) {
                    mem_lock(peer->priority_peers[jj]);
                }
            }
            print_peer_info(peer, 0);
            if (peer->priority) {
                for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++) {
                    print_peer_info(peer->priority_peers[jj], jj);
                    free(peer->priority_peers[jj]);
                }
            }
            automore_conditional(PEER_AUTOMORE_LINES);
            /*
             * the change flag is set when peer moves to different
             * queue or when the peer structure is deleted
             */
            if (peer->change_flag) {
                printf("\nPeers may have changed.... Retry command");
                free(peer);
                automore_disable();
                return;
            }
            free(peer);
        }
    }

    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            mem_lock(peer);
            peer->change_flag = FALSE;
            print_peer_info(peer, 0);
            automore_conditional(PEER_AUTOMORE_LINES);
            if (peer->change_flag) {
                printf("\nPeers may have changed.... Retry command");
                free(peer);
                automore_disable();
                return;
            }
            free(peer);
        }
    }

    if ((peer = (peerentry *) disc_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->peer_type == DEMAND_PEER)
                continue;
            mem_lock(peer);
            peer->change_flag = FALSE;
            if (peer->priority) {
                for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++) {
                    mem_lock(peer->priority_peers[jj]);
                }
            }
            print_peer_info(peer, 0);
            if (peer->priority) {
                for (jj = 1; jj <= DLSW_PRIORITY_COUNT; jj++) {
                    print_peer_info(peer->priority_peers[jj], jj);
                    free(peer->priority_peers[jj]);
                }
            }
            automore_conditional(PEER_AUTOMORE_LINES);
            if (peer->change_flag) {
                printf("\nPeers may have changed.... Retry command");
                free(peer);
                automore_disable();
                return;
            }
            free(peer);
        }
    }
    automore_disable();
}


/*
 * print_peer_info()
 */
static void print_peer_info (peerentry  *peer, int i)
{
    peerentry *hi_peer = peer;
    char uptime_str[16];
    char sbuf[90];

    if (peer->priority_top)
        hi_peer = peer->priority_top;
    if (hi_peer->remove)
        return;

    sprintf(sbuf, "");
    switch (peer->xport_type) {
      case DLSW_TCP:
        if (!peer->priority_top) {
            printf("\n TCP %15i", peer->ipaddr);
            if (peer->priority) {
                i = 0;
            }
        }
        if ((peer->priority_top) || (peer->priority)) {
            printf("\n  %s priority  ", priority_peername(i));
        }
        break;

      case DLSW_FST:
        printf("\n FST %15i", peer->ipaddr);
        sprintf(sbuf, "\n\tExpected: %u  Next Send: %u  Seq errors: %u",
                peer->ip_int_seqnumin, peer->ip_int_seqnumout,
                peer->out_of_window_cnt);
        break;

      case DLSW_IF:
        printf("\n IF  %15s", peer->outidb->hw_namestring);
        break;

      case DLSW_FR:
        printf("\n FR  %10s %-4d", peer->swidb->short_namestring, 
                 peer->fr_dlci);
        break;

      case DLSW_IF_LAN:
        printf("\n IF  %c%d %e", peer->outidb->name[0],
               peer->outidb->unit, peer->macaddr);
        break;

      default:
        printf("\n  Unknown Peer");
        return;
    }

    printf(" %8s %-9u %-9u %-5s %-6u",
           state_to_string(peer->conn_state),
           peer->pkts_rx, peer->pkts_tx,
           peer_type_str[peer->peer_type], peer->drops);

    /*
     * Display number of active circuits on a peer if circuits
     * a locally terminated.
     */
    if (((peer->xport_type == DLSW_TCP) && (!peer->priority_top)) ||
        ((peer->xport_type == DLSW_FR) && (peer->local_ack)))
        printf(" %-4d", peer->num_ckts);
    else
        printf(" %-4s", "-");

    if (((peer->conn_state == PEER_CONN) ||
         (peer->conn_state == PEER_XPORT_BUSY)) &&
        (peer->peer_status == PEER_READY))
        sprint_dhms(uptime_str, peer->uptime);
    else
        sprintf(uptime_str, "-");

    if ((peer->xport_type == DLSW_TCP) &&
        ((peer->conn_state == PEER_CONN) ||
         (peer->conn_state == PEER_XPORT_BUSY)) && 
        (peer->tcp_wr_t))
         printf(" %-3d %-8s", 
                reg_invoke_ip_tcpdriver_oqlen((tcp_encaps *) peer->tcp_wr_t),
                uptime_str);
    else
        printf(" %-3s %-8s%s", "-", uptime_str, sbuf);
}


/*
 * print_capabilities()
 */

void print_capabilities (peerhandle_t peer_handle)
{
    peerentry *peer = (peerentry *) peer_handle;
    peerentry *bp;
    macentry_t *macentry;
    nbentry_t *nbentry;
    uchar cvsap_mask;
    peercap_t *cap;
    ulong ii, jj;
    boolean got_one = FALSE;
    char buff[DLSW_MAX_PEER_ID];

    if (peer) {
        if (peer->remove)
            return;
        printf("\nDLSw: Capabilities for %s", peer_str(peer, buff));
        cap = &peer->cap;
    } else {
        cap = &lpeer->cap;
        printf("\nDLSw: Capabilities for local peer");
    }

    if (!cap->cap_known) {
        printf("\n  are not known at this time");
        return;
    }
    printf("\n  vendor id (OUI)         : '%x%x%x'", cap->vendor_id[0],
           cap->vendor_id[1], cap->vendor_id[2]);
    if (!bcmp(lpeer->cap.vendor_id, cap->vendor_id, SNAP_OUIBYTES))
        printf(" (cisco)");
    printf("\n  version number          : %d", cap->version_number);
    printf("\n  release number          : %d", cap->release_number);
    printf("\n  init pacing window      : %d", cap->initial_pacing_window);
    printf("\n  unsupported saps        : ");
    for (ii = 0; ii < SAP_CV_BYTES; ii++) {
        cvsap_mask = MIN_SAP_IN_BYTE;
        for (jj = 0; jj < 8; jj++) {
            if (!(cap->supp_saps[ii] & cvsap_mask)) {
                printf("%x ", cvsap_to_sap(ii, cvsap_mask));
                got_one = TRUE;
            }
            cvsap_mask = cvsap_mask >> 1;
        }
    }
    if (!got_one)
        printf("none");
    printf("\n  num of tcp sessions     : %d", cap->num_tcp_sessions);
    printf("\n  loop prevent support    : %s", 
           (cap->loop_prevent_support) ? "yes" : "no");
    printf("\n  icanreach mac-exclusive : %s", 
           (cap->mac_exclusive) ? "yes" : "no");
    printf("\n  icanreach netbios-excl. : %s", 
           (cap->nb_exclusive) ? "yes" : "no");
    printf("\n  reachable mac addresses : "); 
    macentry = (macentry_t *) cap->macQ.qhead;
    if (!macentry)
        printf("none");
    else {
        printf("%14e <mask %e>", macentry->macaddr, macentry->macmask);
        macentry = macentry->next;
        for (; macentry; macentry = macentry->next)
            printf("\n                            %14e <mask %e>",
                   macentry->macaddr, macentry->macmask); 
    }
    printf("\n  reachable netbios names : "); 
    nbentry = (nbentry_t *) cap->nbnameQ.qhead;
    if (!nbentry)
        printf("none");
    else {
       printf("%s", nbentry->nbname);
        nbentry = nbentry->next;
        for (; nbentry; nbentry = nbentry->next)
            printf("\n                            %s", nbentry->nbname); 
    }
            
    /* 
     * Only show cisco extended capabilities if we are displaying
     * the local peer's capabilities or the remote is a cisco.
     */
    if ((peer == NULL) ||
        (peer_get_vendor_type((peerhandle_t) peer) == PEER_CISCO)) {
        printf("\n  cisco version number    : %d", cap->cisco_version);
        printf("\n  peer group number       : %d", cap->group);
        printf("\n  border peer capable     : %s", 
               cap->border ? "yes" : "no");
        printf("\n  peer cost               : %d", cap->cost);
        printf("\n  biu-segment configured  : %s",
               cap->spoof_and_seg ? "yes" : "no");

        if (peer) {
            /*
             * Show remote peer specific info.
             */
            printf("\n  local-ack configured    : %s",
                   cap->local_ack ? "yes" : "no");
            printf("\n  priority configured     : %s",
                   cap->priority ? "yes" : "no");
        } else {
            /*
             * Show local peer specific info.
             */
            printf("\n  current border peer     : ");
            if (lpeer->cap.border)
                printf("local-peer");
            else if (!lpeer->border_peer)
                printf("none");
            else
                printf("%s cost %d", peer_str(lpeer->border_peer, buff),
                       lpeer->border_peer->cap.cost);
        }

        if (lpeer->cap.border) {
            for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
                if (((bp = (peerentry *) conn_dlx_peerQ[ii].qhead)) && 
                    (bp->cap.border))
                    printf("\n  border peer for group %d : %s cost %d", 
                           ii, peer_str(bp, buff), bp->cap.cost);
            }
        }
    }
    if (peer_get_vendor_type((peerhandle_t) peer) == PEER_CISCO)
        printf("\n  peer type               : %s", peer_type_str[cap->peer_type]);

    printf("\n  version string          : \n%s\n", cap->version_string);
}


/*
 * show_remote_capabilities()
 */

void show_remote_capabilities (peerhandle_t peer_handle)
{
    peerentry  *peer = (peerentry  *) peer_handle;
    ulong ii;
    boolean found = FALSE;

    if (peer)  {
        print_capabilities((peerhandle_t) peer);
        return;
    }

    /*
     * show capabilities for all *connected* peers.
     */
    automore_enable(NULL);
    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
        if (conn_dlx_peerQ[ii].qhead) {
            for (peer = (peerentry *) conn_dlx_peerQ[ii].qhead; peer;
                 peer = peer->next) {
                mem_lock(peer);
                peer->change_flag = FALSE;
                print_capabilities((peerhandle_t) peer);
                automore_conditional(0);
                if (peer->change_flag) {
                    printf("\nPeers may have changed.... Retry command");
                    free(peer);
                    automore_disable();
                    return;
                }
                free(peer);
                found = TRUE;
            }
        }
    }
    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            mem_lock(peer);
            peer->change_flag = FALSE;
            print_capabilities((peerhandle_t) peer);
            automore_conditional(0);
            if (peer->change_flag) {
                printf("\nPeers may have changed.... Retry command");
                free(peer);
                automore_disable();
                return;
            }
            free(peer);
            found = TRUE;
        }
    }
    automore_disable();
    if (!found)
        printf("\nDLSw: No remote peer capabilities known at this time");
}

void dlsw_show_fast_cache ()
{
    ulong ii;
    fast_cache_t *fct;
    peerentry *peer;
    uchar rif_str[64];
    uchar lmac_norii[IEEEBYTES];

    printf("\n    peer              local-mac      remote-mac   l/r sap rif");

    automore_enable(NULL);
    for (ii = 0; ii < DLSW_FCT_HASH_SIZE; ii++) {
        for (fct = fast_hash_table[ii].qhead; fct; fct = fct->next) {
           mem_lock(fct);
           /* peer handle is a pointer to the peer structure. lock it. */
           mem_lock((void *) fct->peer_handle);
           peer = (peerentry *) fct->peer_handle;
           get_rif_str(fct->rif, rif_str);
           ieee_copy(fct->lmac, lmac_norii);
           lmac_norii[0] &= ~TR_RII; /* don't show RII */

           if (peer->xport_type == DLSW_FST)
               printf("\nFST %15i ", peer->ipaddr);
           else
               printf("\nIF %16s ", peer->outidb->hw_namestring);

           printf("%e %e  %02x/%02x  %s",
                  lmac_norii, fct->rmac, fct->lsap, fct->rsap, rif_str);
           automore_conditional(PEER_AUTOMORE_LINES);
           free((void *) fct->peer_handle);
           /* change flag is set when fct is deleted */
           if (fct->change_flag) {
               printf("\nFast cache may have changed.... Retry command");
               free(fct);
               automore_disable();
               return;
           }
           free(fct);
        }
    }
    automore_disable();
}

void dlsw_assign_bgroup_llc2_params (idbtype *sw_idb, 
                                     dlsw_bgroup_t *dlsw_bgroup)
{
   llc_sb_t      *sb;

   sb = idb_get_swsb(sw_idb, SWIDB_SB_LLC2);
   if (sb) {
       sb->llc2_wind          = dlsw_bgroup->llc2_wind;
       sb->llc2_akmax         = dlsw_bgroup->llc2_akmax;
       sb->llc2_akdelay_time  = dlsw_bgroup->llc2_akdelay_time;
       sb->llc2_n2            = dlsw_bgroup->llc2_n2;
       sb->llc2_T1_time       = dlsw_bgroup->llc2_T1_time;
       sb->llc2_trej_time     = dlsw_bgroup->llc2_trej_time;
       sb->llc2_tpf_time      = dlsw_bgroup->llc2_tpf_time;
       sb->llc2_tbusy_time    = dlsw_bgroup->llc2_tbusy_time;
       sb->llc2_idle_time     = dlsw_bgroup->llc2_idle_time;
       sb->llc2_txQ_max       = dlsw_bgroup->llc2_txQ_max;
       sb->llc2_xid_rtry_time = dlsw_bgroup->llc2_xid_rtry_time;
       sb->llc2_xid_neg_val_time = dlsw_bgroup->llc2_xid_neg_val_time;
   } else {
       buginf("\nSub_block not available");
   }
}


