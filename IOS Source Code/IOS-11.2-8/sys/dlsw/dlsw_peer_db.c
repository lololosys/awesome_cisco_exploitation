/* $Id: dlsw_peer_db.c,v 3.23.4.14 1996/09/11 18:37:07 akhanna Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_peer_db.c,v $
 *------------------------------------------------------------------
 * DLSw Peer Data Base Service Routines
 *
 * July 1994, Frank Bordonaro 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Controls all access to the DLSw peer data base.
 *------------------------------------------------------------------
 * $Log: dlsw_peer_db.c,v $
 * Revision 3.23.4.14  1996/09/11  18:37:07  akhanna
 * CSCdi67926:  DLSW: ethernet direct encap causes crash
 * Branch: California_branch
 *
 * Revision 3.23.4.13  1996/09/09  19:08:54  akhanna
 * CSCdi65724:  Cannot stop keepalives on promiscous peer
 * Branch: California_branch
 *
 * Revision 3.23.4.12  1996/08/28  12:41:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.23.4.11  1996/08/21  22:19:23  akhanna
 * CSCdi48010:  dlsw flow control allow max/min flow control window sizes
 * Branch: California_branch
 *
 * Revision 3.23.4.10  1996/07/31  18:19:59  akhanna
 * CSCdi61887:  dlsw icanreach saps
 * Branch: California_branch
 *
 * Revision 3.23.4.9  1996/07/31  01:37:52  jlautman
 * CSCdi64415:  dlsw mib Active Circuits and Circut Creates
 * reversed/TConnStat wrong
 * Branch: California_branch
 *
 * Revision 3.23.4.8  1996/07/09  18:08:39  kmoberg
 * CSCdi46177:  access-expression output capabilities needs to be
 *              added to DLSw. Clean up remote peer config.
 * Branch: California_branch
 *
 * Revision 3.23.4.7  1996/06/28  23:05:45  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.23.4.6  1996/06/27  15:13:36  fbordona
 * CSCdi61278:  DLSw: System restarted by bus error at PC 0xD0D0D0D
 *              action_b(). Do not free peers if blocked on TCP open.
 *              Connect timer not set properly for priority peers.
 * Branch: California_branch
 *
 * Revision 3.23.4.5  1996/06/11  15:01:18  ravip
 * CSCdi58441:  xid3 flooding in csna/dlsw environment cause access
 *              router to reboot
 *                  - Carry xid command response bit in the message
 *                  - Use Capex to exchange information about this
 *                    support
 * Branch: California_branch
 *
 * Revision 3.23.4.4  1996/06/08  14:58:47  fbordona
 * CSCdi58842:  System restarted by bus error at PC 0xD0D0D0D, addr 0x0
 *              Do not clean up peer if blocked on TCP open.
 *              keepalive_count not initialized properly causing false
 *              keepalive failure.
 * Branch: California_branch
 *
 * Revision 3.23.4.3  1996/05/23  14:50:26  sbales
 * CSCdi58140:  DLSW+ - backup peer connects while primary still up
 * Branch: California_branch
 *
 * Revision 3.23.4.2  1996/05/17  10:46:14  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.19.2.13  1996/05/14  03:20:33  kpatel
 * CSCdi55075:  Cant define dlsw direct encapsulation for more that pvc
 *              dls frame-relay does not work over point to point
 *              sub-interface (CSCdi55085)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.12  1996/05/12  15:46:38  vkamat
 * CSCdi57297:  Invalid_peer traceback in DLSW local-switch configuration
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.11  1996/04/26  07:34:38  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.19.2.10  1996/04/25  19:37:48  ravip
 * CSCdi54131:  DLSw MIB ciscoDlswTConnOper Table returns 0 for stats
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.9  1996/04/25  16:20:13  ravip
 * CSCdi45773:  dlsw icanreach mac-exclusive command not working
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.8  1996/04/24  18:35:36  fbordona
 * CSCdi55588:  DLSw: Use managed timers for connecting dynamic peers
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.7  1996/04/23  20:13:57  fbordona
 * CSCdi55437:  DLSw peer may take several minutes to connect after reload
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.6  1996/04/20  02:08:20  fbordona
 * CSCdi55177:  DLSw phase I to convert peer to managed timers
 *              Required for scalability of large DLSw peer nets.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.5  1996/04/17  18:17:16  sbales
 * CSCdi53218:  DLSw+: backup peers cause loop with ethernet
 * Branch: IbuMod_Calif_branch
 *              Also implemented parser checks for redundant backups
 *              or backup peers of backup peers.
 *
 * Revision 3.19.2.4  1996/04/05  23:12:03  kpatel
 * CSCdi51479:  Memory leak when removing dlsw configuration.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.3  1996/04/03  14:01:50  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.19.2.2  1996/03/29  02:13:49  fbordona
 * CSCdi52957:  DLSW: Can not use lsap-output-list greater than 255
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.19.2.1  1996/03/17  17:38:15  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.23.4.1  1996/03/18  19:32:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.14.2.3  1996/03/16  06:39:25  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.14.2.2  1996/03/07  08:44:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.14.2.1  1996/02/20  00:46:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.23  1996/03/08  22:31:39  mzallocc
 * CSCdi49393:  Router crash on get for ciscoDlswTConn*
 *
 * Revision 3.22  1996/03/06  16:55:26  fbordona
 * CSCdi50687:  dlsw peer-on-demand-defaults does not allow the largest
 *              frame lf.
 *
 * Revision 3.21  1996/03/05  18:49:33  fbordona
 * CSCdi50574:  DLSw pod stuck in connect status.
 *              Use number of circuit count to disconnect pods.
 *
 * Revision 3.20  1996/02/27  21:36:30  fbordona
 * CSCdi49949:  DLSw peer-on-demand peers ignore tcp-queue-max.
 *              Improve "show dlsw peers" output.
 *
 * Revision 3.19  1996/02/26  20:56:28  ioakley
 * CSCdi49687: Correct problem with tcp-queue-max parameter for
 *             the global "dlsw remote-peer" config command.
 *
 * Revision 3.18  1996/02/20  22:08:18  fbordona
 * CSCdi47801:  SEgV exception, PC0x60637DFC tcpdriver_fhost.
 *              Use the tcpencaps handle now passed in the tcp
 *              closef vector (CSCdi48380) to determine if the
 *              DLSw read or write pipe has been closed.
 *
 * Revision 3.17  1996/02/07  19:09:24  fbordona
 * CSCdi46958:  Router crashes when DLSw peer is reloaded.
 *
 * Revision 3.16  1996/02/01  06:01:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.15  1996/02/01  02:57:08  fbordona
 * CSCdi47930:  DLSw session does not reconnect after peer powered off.
 *              Only attempt one peer connection at at time to avoid
 *              starving the dlsw background process.
 *
 * Revision 3.14  1996/01/18  22:24:54  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.13  1996/01/12  17:21:16  ravip
 * CSCdi46574:  cost from cap. exchange is ignored when choosing path
 *
 * Revision 3.12  1996/01/04  21:39:39  fbordona
 * CSCdi46273:  DLSw peers cycle CONNECT to DISC after shut no shut on
 *              serial.
 *
 * Revision 3.11  1995/12/19  00:39:54  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.10  1995/12/15  17:18:31  fbordona
 * CSCdi45661:  DLSw+ peers show incorrect circuit count.
 *              Added checking for enable and disable of ports.
 *              Consolidate fsm to disp *stn structures.
 *              Consolidate fsm_to_disp() and cstm_to_disp().
 *              Improve local switching debug.
 *
 * Revision 3.9  1995/12/14  15:28:09  ravip
 * CSCdi41809:  DLSW MIB ciruit counters and OIDs always set to 0
 *
 * Revision 3.8  1995/12/01  23:39:44  ravip
 * CSCdi41808:  Dlsw Mib does not respond to get-next correctly
 *
 * Revision 3.7  1995/11/30  21:22:56  fbordona
 * CSCdi44759:  DLSw peer-on-demand peers prematurely disconnect.
 *              Fix peer-on-demand crash introduced in 11.1
 *
 * Revision 3.6  1995/11/29  22:06:34  fbordona
 * CSCdi43961:  DLSw peer mysteriously shows up twice in write term.
 *              Simultaneous priority and border peers cause problems.
 *
 * Revision 3.5  1995/11/29  19:46:11  ravip
 * CSCdi40989:  crash: router tried to reference free memory in dlsw code
 *              use mem_lock() to lock memory, to avoid other routines
 *              from deleting the memory block in use.
 *              Use a flag to tag actions that would change the links.
 *
 * Revision 3.4  1995/11/20  23:25:54  fbordona
 * CSCdi42215:  dlsw backup peers broken
 *
 * Revision 3.3  1995/11/17  09:03:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:24:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  20:55:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/10/20  14:10:22  ravip
 * CSCdi40722: Exclusive reachability option does not filter frames
 *             correctly. Fixed problems with deleting reachability cache.
 *
 * Revision 2.9  1995/10/09  20:18:44  rnaderi
 * CSCdi41641:  Add sub-interface option to the parser for DLSw+/FR
 *
 * Revision 2.8  1995/10/03  15:07:33  fbordona
 * CSCdi40884:  crashdump netbios_acfcheck(0x301faf0+0x2c9844)+0x160)
 *              Add DLSw callback when NetBios access list removed
 *
 * Revision 2.7  1995/08/10  18:22:52  fbordona
 * CSCdi38575:  DLSw promiscuous peer doesnt learn remote config
 *
 * Revision 2.6  1995/08/07  05:31:35  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.5  1995/07/26  19:12:22  hampton
 * Convert DLSW to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37708]
 *
 * Revision 2.4  1995/07/24 13:05:50  fbordona
 * CSCdi34580:  DLS show circuit displays HALT PENDING NOACK after
 *              successful disc
 *
 * Revision 2.3  1995/06/28  18:51:13  fbordona
 * CSCdi36524:  CLS SDLC multidrop broken
 *              Clean up DLSw header files
 *
 * Revision 2.2  1995/06/18  21:21:27  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:27:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




#include "master.h"
#include "logger.h"
#include "../dlsw/msg_dlsw_peer.c"	/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "mgd_timers.h"
#include "../dlsw/parser_defs_dlsw.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../srt/srt_registry.h"
#include "../ui/debug.h"

#include "../ibm/netbios.h"
#include "../tcp/tcpdriver.h"
#include "../srt/rsrb.h"
#include "../dlsw/dlsw_dlx.h"
#include "../dlsw/dlsw_ssp.h"
#include "../dlsw/dlsw_peer_externs.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_peer.h"
#include "../dlsw/dlsw_cap_exchg.h"
#include "../dlsw/dlsw_debug.h"
#include "../h/mibconf.h"
#include "../if/rif_util.h"

/* 
 * The local peer information is maintained serarately from remote
 * peer information. Remote peer information is held on one of three
 * logical categories:
 *
 * 1. Remote Peers that are  Connection 
 *    This list is comprised of serarate lists for each known peer group
 *    and a list for peers not assigned to a peer group.
 *
 * 2. Remote Peers that are Disconnect
 *
 * 3. Remote Peers that are Awaiting Disconnect
 * 
 */

/*
 * Global Data 
 */
localpeer_t *lpeer = NULL;
/* should be changed to dynamically create peer group lists */
queuetype conn_dlx_peerQ[DLSW_MAX_PGROUP];
queuetype conn_dlsw_peerQ;
queuetype wait_conn_peerQ;
queuetype direct_inputQ;
queuetype disc_peerQ;
queuetype port_listQ;
queuetype dlsw_peer_llc2Q;

mgd_timer dlsw_peer_master_timer;
mgd_timer dlsw_peer_conn_timer;
mgd_timer dlsw_peer_keep_timer;

/*
 * Global data for DLSw MIB
 */
long    conn_active_connection = 0;
long    conn_close_idle = 0;
long    conn_close_busy = 0;

/*
 * File Scope Function Prototypes
 */
static void adjust_remote_keepalives (ulong new_int);
static void reset_peer (peerentry *peer);
static void clear_non_conf_info (peerentry *peer);
static void dlsw_netbios_delete_list (netbios_acclisttype *);
static void peer_del_cache (peerentry *);

extern boolean csm_filter_check;

/*
 *********************************************************************
 * External Peer Data Base Interface
 *********************************************************************
 */

/*
 * dlsw_peer_init()
 *
 * Initialize the Peer Data Base before using it.
 */

void dlsw_peer_init ()
{
    ulong ii;
     static int dlsw_peer_pid = 0;

    if (!dlsw_peer_pid) {
        dlsw_peer_pid = cfork((forkproc *) dlsw_peer_process, 0L,
                             1500, "DLSw Peer Process", 0);
        if (dlsw_peer_pid == NO_PROCESS) {
            dlsw_peer_pid = 0;
            errmsg(PEER_GENERAL, "DLSw: Can't cfork dlsw_peer_process");
        }
    }

    queue_init(&conn_dlsw_peerQ, 0);
    queue_init(&disc_peerQ, 0);
    queue_init(&wait_conn_peerQ, 0);
    queue_init(&direct_inputQ, 0);
    queue_init(&port_listQ, 0);
    queue_init(&dlsw_peer_llc2Q, 0);

    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++)
        queue_init(&conn_dlx_peerQ[ii], 0);

    /*
     * Initialize DLSw Transports
     */
    peer_tcp_init();
    peer_fst_init();
    peer_direct_init();

    csm_filter_check = FALSE;

    reg_add_dlsw_remove_netbios_wan_filter(dlsw_netbios_delete_list,
                                           "dlsw_netbios_delete_list");

    /*
     * Initialize Peer Timer Hierarchy
     */
    mgd_timer_init_parent(&dlsw_peer_master_timer, NULL);
    mgd_timer_init_parent(&dlsw_peer_conn_timer, &dlsw_peer_master_timer);
    mgd_timer_init_parent(&dlsw_peer_keep_timer, &dlsw_peer_master_timer);
}


boolean dlsw_sna_spoof_and_seg ()
{
    return(lpeer->cap.spoof_and_seg);
}


/*
 * peer_ip_to_handle()
 *
 * Find a remote peer entry based on ip address.
 * Finds remote peers regardless of peer state.
 */

peerhandle_t peer_ip_to_handle (ipaddrtype ipaddr)
{
    ulong i;
    peerentry *peer;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);
    /*
     * Is this peer a connected DLX peer?
     */
    for (i = 0; i < DLSW_MAX_PGROUP; i++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[i].qhead)) {
            for (; peer; peer = peer->next) {
                if (peer->ipaddr == ipaddr) {
                    reset_interrupt_level(status);
                    return((peerhandle_t) peer);
                }
            }
        }
    }
    /*
     * Is this peer a connected dlsw peer?
     */
    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->ipaddr == ipaddr) {
                reset_interrupt_level(status);
                return((peerhandle_t) peer);
            }
        }
    }

    /*
     * Is this peer waiting to have it's capabilities acted upon?
     */
    if ((peer = (peerentry *) wait_conn_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->ipaddr == ipaddr) {
                reset_interrupt_level(status);
                return((peerhandle_t) peer);
            }
        }
    }

    /*
     * Is this a disconnected?
     */
    if ((peer = (peerentry *) disc_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if ((!peer->remove) && (peer->ipaddr == ipaddr)) {
                reset_interrupt_level(status);
                return((peerhandle_t) peer);
            }
        }
    }

    reset_interrupt_level(status);
    return((peerhandle_t) NULL);
}


/*
 * peer_hwidb_to_handle()
 *
 * Find a remote peer entry based on its direct encapsulation interface.
 */

peerhandle_t peer_hwidb_to_handle (hwidbtype *hwidb)
{
    ulong i;
    peerentry *peer;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);
    /*
     * Is this peer a connected DLX peer?
     */
    for (i = 0; i < DLSW_MAX_PGROUP; i++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[i].qhead)) {
            for (; peer; peer = peer->next) {
                if (peer->outidb == hwidb) {
                    reset_interrupt_level(status);
                    return((peerhandle_t) peer);
                }
            }
        }
    }
    /*
     * Is this peer a connected dlsw peer?
     */
    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->outidb == hwidb) {
                reset_interrupt_level(status);
                return((peerhandle_t) peer);
            }
        }
    }
    /*
     * Is this peer disconnected?
     */
    if ((peer = (peerentry *) disc_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if ((!peer->remove) && (peer->outidb == hwidb)) {
                reset_interrupt_level(status);
                return((peerhandle_t) peer);
            }
        }
    }
    reset_interrupt_level(status);
    return((peerhandle_t) NULL);
}


/*
 * peer_swidb_dlci_to_handle()
 *
 * Find a remote peer entry based on direct encapsulation interface and
 * frame relay dlci
 * 
 */

peerhandle_t peer_swidb_dlci_to_handle (idbtype *swidb, ushort fr_dlci)
{
    ulong i;
    peerentry *peer;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);
    /*
     * Is this peer a connected DLX peer?
     */
    for (i = 0; i < DLSW_MAX_PGROUP; i++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[i].qhead)) {
            for (; peer; peer = peer->next) {
                if ((peer->swidb == swidb)&&(peer->fr_dlci == fr_dlci)) {
                    reset_interrupt_level(status);
                    return((peerhandle_t) peer);
                }
            }
        }
    }
    /*
     * Is this peer a connected dlsw peer?
     */
    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if ((peer->swidb == swidb)&&(peer->fr_dlci == fr_dlci)) {
                reset_interrupt_level(status);
                return((peerhandle_t) peer);
            }
        }
    }
    /*
     * Is this peer disconnected?
     */
    if ((peer = (peerentry *) disc_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if ((!peer->remove) && ((peer->swidb == swidb)&&
                                    (peer->fr_dlci == fr_dlci))) {
                reset_interrupt_level(status);
                return((peerhandle_t) peer);
            }
        }
    }
    reset_interrupt_level(status);
    return((peerhandle_t) NULL);
}

mgd_timer dlsw_peer_master_timer;
mgd_timer dlsw_peer_conn_timer;
mgd_timer dlsw_peer_keep_timer;


/*
 * peer_handle_to_int()
 */
 
inline char *peer_handle_to_int (peerhandle_t peer_handle)
{
    return(((peerentry *)peer_handle)->outidb->hw_namestring);
}

/*
 * peer_handle_to_xport()
 */
 
inline uchar peer_handle_to_xport (peerhandle_t peer_handle)
{
    return(((peerentry *)peer_handle)->xport_type);
}

/*
 * peer_handle_to_ip()
 *
 * Map a remote peer's peer_handle to an ip address.
 * The peer manager notifies the DLSw Core when a peer_handle is no
 * longer valid, so for performance, assume the ccore is passing in a
 * valid peer_handle.
 */
 
inline ipaddrtype peer_handle_to_ip (peerhandle_t peer_handle)
{
    return(((peerentry *)peer_handle)->ipaddr);
}


/*
 * peer_handle_to_str()
 */

inline char *peer_handle_to_str (peerhandle_t peer_handle, char *buff)
{
    if (!buff) {
        errmsg(PEER_GENERAL, "NULL buffer provided to peer_handle_to_str()");
        return(NULL);
    } else if (!peer_handle) {
        errmsg(PEER_GENERAL, "NULL handle provided to peer_handle_to_str()");
        buff[0] = ' ';
        buff[1] = '\0';
        return(buff);
    } else if (valid_peer((peerentry *) peer_handle)) {
        return(peer_str((peerentry *) peer_handle, buff));
    } else {
        if (peer_debug_all)
            buginf("\nDLSw: peer_handle_to_str() invalid");
        buff[0] = ' ';
        buff[1] = '\0';
        return(buff);
    }
}

/*
 * peer_get_my_ipaddr()
 *
 * Returns 0 if no ipaddress defined.
 */
inline ipaddrtype peer_get_my_ipaddr ()
{
    return(lpeer ? lpeer->ipaddr : (ipaddrtype) 0);
}


/*
 * get the mac local reachability qhead
 */
inline macentry_t *get_local_mac_reach_qhead()
{
    return(lpeer ? (macentry_t *) lpeer->cap.macQ.qhead : (macentry_t *) NULL);
}


/*
 * get the name local reachability qhead
 */
inline nbentry_t *get_local_name_reach_qhead()
{
    return(lpeer ? (nbentry_t *) lpeer->cap.nbnameQ.qhead : (nbentry_t *) NULL);
}


/*
 * get the peer mac reachability qhead
 */
inline macentry_t *peer_get_mac_reach_qhead(peerhandle_t peer_id)
{
    if (peer_id) {
        return ((macentry_t *) (((peerentry *) peer_id)->cap.macQ.qhead));
    } else {
        return NULL;
    }
}


/*
 * get the peer name reachability qhead
 */
inline nbentry_t *peer_get_name_reach_qhead(peerhandle_t peer_id)
{
    if (peer_id) {
        return ((nbentry_t *) (((peerentry *) peer_id)->cap.nbnameQ.qhead));
    } else {
        return NULL;
    }
}


/*
 * peer_get_state()
 *
 * return the connection state for a remote peer.
 */
 
static inline peerstatetype peer_get_state (peerhandle_t peer_handle)
{
    return(((peerentry *)peer_handle)->conn_state);
}

 

/*
 * peer_get_cost
 * Return the cost of the peer
 *
 */
inline uchar peer_get_cost (peerhandle_t peer_handle)
{
    if((((peerentry *)peer_handle)->cost) != DLSW_NORM_PCOST) {
        return(((peerentry *)peer_handle)->cost);
    } else {
        return(((peerentry *)peer_handle)->cap.cost);
    }
}


/*
 * dlsw_peer_active 
 * Return an indication if the dlsw peer manager is enabled.
 * Primarily for TCP usage.
 */

boolean dlsw_peer_active (ulong sport, ulong dport, ipaddrtype fhost)
{
    return(lpeer != NULL);
}


boolean peer_get_xid_op_support (peerhandle_t peer_handle)
{
    if (peer_handle) {
        return (((peerentry *) peer_handle)->cap.xid_op_support);
    } else {
        return FALSE;
    }
}

/*
 * peer_get_vendor_type()
 */

uchar peer_get_vendor_type (peerhandle_t peer_handle)
{
    peerentry *peer;

    peer = (peerentry *)peer_handle;
    if (!peer->cap.cap_known)
        return(PEER_UNKNOWN);

    if ((lpeer) && 
	(!bcmp(lpeer->cap.vendor_id, peer->cap.vendor_id, SNAP_OUIBYTES)))
        return(PEER_CISCO);

    return(PEER_NOT_CISCO);
}


inline boolean peer_ciscos ()
{
    int ii;

    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
        if (conn_dlx_peerQ[ii].qhead)
            return(TRUE);
    }
    return(FALSE);
}


inline boolean peer_non_ciscos ()
{
    return(conn_dlsw_peerQ.qhead != NULL);
}

 
/*
 * peer_get_box_largest_frame()
 */

inline uchar peer_get_box_largest_frame ()
{

    if (lpeer) {
        return((lpeer->largest_frame == RC_LFINITIAL) ?
                       RC_LF17800 : lpeer->largest_frame);
    } else {
        return(RC_LF17800);
    }

}


/*
 * peer_get_largest_frame()
 */

inline uchar peer_get_largest_frame (peerhandle_t peer_handle)
{
    int media_lf;
    peerentry *peer = (peerentry *) peer_handle;

    if ((peer->xport_type == DLSW_IF) ||
        (peer->xport_type == DLSW_IF_LAN)) {
        /*
         * Re-calc lf since users could have changed it since
         * the peer was created.
         */
         media_lf = bytes_to_rc_code(peer->outidb->maxmtu);
         /*
          * If media lf is not greater than the user configured lf
          * on the remote peer, then use it.
          */
         if (media_lf <= peer->largest_frame_def)
             peer->largest_frame = media_lf;
    }
    return(((peerentry *)peer_handle)->largest_frame);
}

/*
 * peer_get_init_pac_window()
 */

inline ulong peer_get_init_pac_window (peerhandle_t peer_handle)
{
    return(((peerentry *)peer_handle)->cap.initial_pacing_window);
}


/*
 * peer_get_port_list()
 */

static inline portlistentry *peer_get_port_list (peerhandle_t peer_handle)
{
    return(((peerentry *)peer_handle)->ple);
}

/*
 * supported_sap_check()
 *
 * Returns non-zero if the sap is supported.
 */

boolean supported_sap_check (peerhandle_t peer_handle, uchar ssap, uchar dsap)
{
    uchar byte_index;
    uchar bit_index;
    uchar bit_mask;
    uchar *sap_array;

    if (peer_handle)
        sap_array = ((peerentry *)peer_handle)->cap.supp_saps;
    else
        sap_array = lpeer->cap.supp_saps;

    /*
     * Check source sap.
     */
    byte_index = ssap >> 4;
    bit_index = (ssap >> 1) % 8;
    bit_mask = MIN_SAP_IN_BYTE >> bit_index;
    if ((sap_array[byte_index] & bit_mask) == 0)
        return(FALSE);

    /*
     * Check destination sap.
     */
    byte_index = dsap >> 4;
    bit_index = (dsap >> 1) % 8;
    bit_mask = MIN_SAP_IN_BYTE >> bit_index;
    if ((sap_array[byte_index] & bit_mask) == 0)
        return(FALSE);
 
    return(TRUE);
}

/*
 *********************************************************************
 * Internal Peer Data Base Interface
 *********************************************************************
 */


/*
 * dlsw_netbios_delete_list
 *
 * Go through all peers, removing references to the access list being
 * deleted.
 */

static void dlsw_netbios_delete_list (netbios_acclisttype *netbios_list)
{
    ulong i;
    peerentry *peer;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);
    /*
     * Is this peer a connected DLX peer?
     */
    for (i = 0; i < DLSW_MAX_PGROUP; i++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[i].qhead)) {
            for (; peer; peer = peer->next) {
                if (peer->netbios_h_oacf == netbios_list)
                    peer->netbios_h_oacf = NULL;
                if (peer->netbios_b_oacf == netbios_list)
                    peer->netbios_b_oacf = NULL;
            }
        }
    }
    /*
     * Is this peer a connected dlsw peer?
     */
    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->netbios_h_oacf == netbios_list)
                peer->netbios_h_oacf = NULL;
            if (peer->netbios_b_oacf == netbios_list)
                peer->netbios_b_oacf = NULL;
        }
    }

    /*
     * Is this peer waiting to have it's capabilities acted upon?
     */
    if ((peer = (peerentry *) wait_conn_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->netbios_h_oacf == netbios_list)
                peer->netbios_h_oacf = NULL;
            if (peer->netbios_b_oacf == netbios_list)
                peer->netbios_b_oacf = NULL;
        }
    }

    /*
     * Is this a disconnected?
     */
    if ((peer = (peerentry *) disc_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->netbios_h_oacf == netbios_list)
                peer->netbios_h_oacf = NULL;
            if (peer->netbios_b_oacf == netbios_list)
                peer->netbios_b_oacf = NULL;
        }
    }

    reset_interrupt_level(status);
}


/*
 * get_pod_peer_by_ip()
 */

peerentry *get_pod_peer_by_ip (ipaddrtype ipaddr)
{
    peerentry *peer;

    peer = get_conn_peer_by_ip(ipaddr);
    if (peer)
        return(peer);

    if ((peer = (peerentry *) wait_conn_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->ipaddr == ipaddr)
                return(peer);
        }
    }

    if ((peer = (peerentry *) disc_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if ((!peer->remove) && (peer->ipaddr == ipaddr))
                return(peer);
        }
    }

    return(NULL);
}


/*
 * get_conn_peer_by_ip()
 *
 * Find a remote peer entry based on ip address.
 * Only returns peers that are connected (capability exchange complete).
 */

peerentry *get_conn_peer_by_ip (ipaddrtype ipaddr)
{
    ulong i;
    peerentry *peer;
    leveltype status;

    status = raise_interrupt_level(ALL_DISABLE);
    /*
     * Is this peer a connected DLX peer?
     */
    for (i = 0; i < DLSW_MAX_PGROUP; i++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[i].qhead)) {
            for (; peer; peer = peer->next) {
                if (peer->ipaddr == ipaddr) {
                    reset_interrupt_level(status);
                    return(peer);
                }
            }
        }
    }
    /*
     * Is this peer a connected dlsw peer?
     */
    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->ipaddr == ipaddr) {
                reset_interrupt_level(status);
                return(peer);
            }
        }
    }

    reset_interrupt_level(status);
    return((peerentry *) NULL);
}



/*
 * peer_ready_to_send()
 */

boolean peer_ready_to_send (peerentry *peer)
{
    if (!peer || peer->remove)
        return(FALSE);

    if (peer->peer_status == PEER_READY)
        return(TRUE);

    if (peer->peer_status == PEER_NOT_READY) {
        if ((peer->peer_type == DEMAND_PEER) ||
	    (peer->peer_type == DYNAMIC_PEER))
            return(TRUE);
    }

    return(FALSE);
}


/*
 * valid_peer()
 */

boolean valid_peer (peerentry *peer)
{
    if ((peer) && (!peer->remove) &&
        ((peer->peer_status == PEER_READY) ||
         (peer->peer_status == PEER_NOT_READY))) {
        return(TRUE);
    }

    if (peer_debug_all) {
        if (peer == 0)
            buginf("\nDLSw: Invalid_peer(): peer:%x", peer);
        else
            buginf("\nDLSw: Invalid_peer(): peer:%x rem:%d status:%x type:%x",
                   peer, peer->remove, peer->peer_status, peer->peer_type);
        errmsg(PEER_GENERAL, "Invalid_peer");
    }
    return(FALSE);
}


/*
 * find_priority_peer()
 *
 * Find a remote peer entry based on ip address and local port number.
 */

peerentry *find_priority_peer (ipaddrtype ipaddr, ushort port)
{
    peerentry *peer;
    uchar i;

    if ((peer = (peerentry *)peer_ip_to_handle(ipaddr))) {
        if ((peer->port == port) || (!peer->priority))
            return(peer);

        for (i = 1; i <= DLSW_PRIORITY_COUNT; i++) {
            if (peer->priority_peers[i]) {
                if (peer->priority_peers[i]->port == port)
                    return(peer->priority_peers[i]);
            }
        }
    }
    return(peer);
}


/*
 * find_port_list()
 */

portlistentry *find_port_list (uchar list_num)
{
    portlistentry *entry = NULL;
  
    if (port_listQ.qhead) {
        for (entry = port_listQ.qhead; entry; entry = entry->next) {
            if (entry->list_num == list_num)
                break; 
        }
    }
    return(entry);
}



/*
 * remove_port_list()
 */
void remove_port_list (portlistentry *entry, uchar list_num)
{
    if (!entry) {
        entry = find_port_list(list_num);
    }
    if (entry) {
        entry->portlist_len = 0;
        if ((entry->portlist_len == 0) && 
                     (entry->ringlist_len == 0) &&
                     (entry->bgrouplist_len == 0)) {
            p_unqueue(&port_listQ, entry);
            free(entry);
        }
    }
}



/*
 * remove_ring_list()
 */
void remove_ring_list (portlistentry *entry, uchar list_num)
{
    if (!entry) {
        entry = find_port_list(list_num);
    }
    if (entry) {
        entry->ringlist_len = 0;
        if ((entry->portlist_len == 0) && 
                     (entry->ringlist_len == 0) &&
                     (entry->bgrouplist_len == 0)) {
            p_unqueue(&port_listQ, entry);
            free(entry);
        }
    }
}




/*
 * remove_bgroup_list()
 */
void remove_bgroup_list (portlistentry *entry, uchar list_num)
{
    if (!entry) {
        entry = find_port_list(list_num);
    }
    if (entry) {
        entry->bgrouplist_len = 0;
        if ((entry->portlist_len == 0) && 
                     (entry->ringlist_len == 0) &&
                     (entry->bgrouplist_len == 0)) {
            p_unqueue(&port_listQ, entry);
            free(entry);
        }
    }
}




/*
 * peer_using_port_list()
 */

peerentry *peer_using_port_list (uchar list_num)
{
    ulong i;
    peerentry *peer;

    for (i = 0; i < DLSW_MAX_PGROUP; i++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[i].qhead)) {
            for (; peer; peer = peer->next) {
		if (peer->ple->list_num == list_num)
		    return(peer);
	    }
        }
    }

    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->ple->list_num == list_num)
                return(peer);
        }
    }

    if ((peer = (peerentry *) wait_conn_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->ple->list_num == list_num)
                return(peer);
        }
    }

    if ((peer = (peerentry *) disc_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->ple->list_num == list_num)
		return(peer);
        }
    }

    return(NULL);
}


/*
 * peer_pass_port_filter()
 *
 * Returns TRUE of peer has access to "port", FALSE otherwise.
 */

boolean peer_pass_port_filter (peerhandle_t peer_handle, dlsw_ring_t *port)
{
    portlistentry *ple;
    uchar i;

    if ((peer_handle == 0) || (port == NULL)) {
         return (TRUE);
    }

    if (!valid_peer((peerentry *) peer_handle)) {
         return (TRUE);
    }

    ple = ((peerentry *) peer_handle)->ple;
    if (!ple) {
        return(TRUE);
    }

    for (i = 0; i < ple->portlist_len; i++) {
        if (ple->sw_idb[i] == port->swidb) {
            return(TRUE);
        }
    }
    for (i = 0; i < ple->ringlist_len; i++) {
        if (ple->ring_no[i] == port->ring_number) {
            return(TRUE);
        }
    }
    for (i = 0; i < ple->bgrouplist_len; i++) {
        if (ple->bgroup_no[i] == port->trans_bridge_group) {
            return(TRUE);
        }
    }
    return(FALSE);
}

static unsigned int PeerIndexInitValue (void) 
{
  /* this routine returns an initial value for peer_index as defined
     in the dlsw mib for objects ciscoDlswTConnOperConfigIndex and
     ciscoDlswTConnConfigIndex
  */
  static dlsw_peer_index_initializer = 0;
  if (++dlsw_peer_index_initializer > 65000) {
    dlsw_peer_index_initializer = 1;
  };
  return dlsw_peer_index_initializer;
}

/*
 * create_peer()
 */

peerentry *create_peer (boolean top_peer, ushort fr_dlci, 
                        ipaddrtype ipaddr, peer_t ptype,
                        portlistentry *ple, uchar xport_type,
                        boolean lack, ushort cost, ushort lf_code,
                        ushort tcp_qlen, hwidbtype *outidb, uchar *macaddr,
                        idbtype *swidb, boolean is_backup, uchar conn_mode)
{
    int lf_size;
    int ii;
    peerentry *peer;

    lf_size = rc_code_to_bytes(lf_code);
    if (lf_size == -1) {
        printf("Valid largest frame sizes are %d, %d, %d, %d, %d, %d, %d, %d and %d.",
               RC_LF516_VAL, RC_LF1470_VAL, RC_LF1500_VAL,
               RC_LF2052_VAL, RC_LF4472_VAL, RC_LF8144_VAL,
               RC_LF11407_VAL, RC_LF11454_VAL, RC_LF17800_VAL);
        return(NULL);
    }

    peer = malloc(sizeof(peerentry));
    if (!peer) {
        return(NULL);
    }

    /*
     * Initialize general fields of remote peer entry.
     */
    peer->next = NULL;
    peer->peer_status = PEER_NOT_READY;
    peer->ple = ple;
    peer->qptr = NULL;
    peer->ipaddr = ipaddr;
    peer->remove = FALSE;
    peer->peer_type = ptype;
    peer->conn_state = PEER_DISC;
    peer->debug = peer_debug_all;
    peer->debug_fst_seq = FALSE;
    peer->debug_fast_pak = FALSE;
    peer->debug_fast_err = FALSE;
    peer->cost = cost;
    peer->xport_type = xport_type;

    bcopy(zerosnap, peer->cap.vendor_id, SNAP_OUIBYTES);
    peer->cap.version_number = 0;
    if (peer->cap.version_string) {
        free(peer->cap.version_string);
        peer->cap.version_string = NULL;
    }
    peer->cap.cap_known = FALSE;
    peer->cap.cap_ack = FALSE;
    peer->cap.cap_sent = FALSE;
    peer->cap.initial_pacing_window = 0;
    for (ii = 0; ii < SAP_CV_BYTES; ii++)
        peer->cap.supp_saps[ii] = ~0;
    peer->cap.num_tcp_sessions = DLSW_DEF_NUM_TCP_PIPES;
    peer->cap.loop_prevent_support = FALSE;
    peer->cap.mac_exclusive = FALSE;
    peer->cap.nb_exclusive = FALSE;
    queue_init(&peer->cap.macQ, 0);
    queue_init(&peer->cap.nbnameQ, 0);
    peer->cap.local_ack = FALSE;
    peer->cap.priority = FALSE;
    peer->cap.group = DLSW_UNKNOWN_PGROUP;
    peer->cap.cost = DLSW_UNKNOWN_PCOST;
    peer->cap.border = FALSE;
    peer->cap.pod_enabled = FALSE;
    peer->cap.spoof_and_seg = FALSE;
    peer->cap.xid_op_support = FALSE;
    peer->cap.peer_type = CONF_PEER;

    /*
     * Only start timers for high priority peer.
     */
    if (top_peer) {
        mgd_timer_init_leaf(&peer->conn_timer, &dlsw_peer_conn_timer,
                            PEER_CONN_TIMER, peer, FALSE);
        mgd_timer_init_leaf(&peer->keep_timer, &dlsw_peer_keep_timer,
                            PEER_KEEP_TIMER, peer, FALSE);
    }

    if (lpeer)
        PUTLONG(&peer->keepalive_int, lpeer->keepalive_int);
    else
        PUTLONG(&peer->keepalive_int, DLSW_DEF_KEEP_INT);

    peer->keepalive_count = 0;
    GET_TIMESTAMP(peer->last_non_peer_traffic);
    GET_TIMESTAMP(peer->zero_ckt_time);
    TIMER_STOP(peer->last_passive_open);
    peer->took_def_keep = TRUE;
    peer->conn_retries = 0;
    peer->conn_mode = conn_mode;

    if ((peer->peer_type == CONF_PEER) && (top_peer) &&
        (!dlsw_disable) && (lpeer->conn_mode != PASSIVE_MODE) &&
        (!is_backup) && (peer->conn_mode != PASSIVE_MODE)) {
        mgd_timer_start(&peer->conn_timer, DLSW_DEF_CONNECT_INT);
    }

    peer->tcp_rd_t = NULL;
    peer->tcp_wr_t = NULL;
    peer->active_open_wait = FALSE;
    peer->tcp_fini_pending = FALSE;
    peer->close_write_pending = FALSE;
    peer->connect_pod = FALSE;
    peer->connect_dynam = FALSE;
    peer->port = 0;
    if (tcp_qlen)
        peer->max_tcp_qlen = tcp_qlen;
    else
        peer->max_tcp_qlen = DLSW_MAX_TCP_QLEN;
    peer->tcp_explorer_hiwater = (peer->max_tcp_qlen *
                                  DLSW_TCP_LO_WATER_PERCENT) / 100;
    peer->local_ack = lack;
    peer->priority = FALSE;
    peer->wait_pri_connect = FALSE;
    for (ii = 0; ii <= DLSW_PRIORITY_COUNT; ii++)
        peer->priority_peers[ii] = NULL;
    peer->priority_top = NULL;
    peer->primary_peer = NULL;
    peer->backup_peer = NULL;
    peer->backup_connect = FALSE;
    GET_TIMESTAMP(peer->conn_start_time);
    peer->num_ckts = 0;

    peer->disc_active_circuits = 0;
    peer->in_data_packets = 0;
    peer->out_data_packets = 0;
    peer->in_data_octets = 0;
    peer->out_data_octets = 0;
    peer->in_cntl_packets = 0;
    peer->out_cntl_packets = 0;
    peer->cur_ex_sents = 0;
    peer->icr_ex_sents = 0;
    peer->cur_ex_rcvds = 0;
    peer->icr_ex_rcvds = 0;
    peer->nq_ex_sents = 0;
    peer->nr_ex_sents = 0;
    peer->nq_ex_rcvds = 0;
    peer->nr_ex_rcvds = 0;
    peer->circuit_creates = 0;
    peer->config_opens = 0;

    peer->rd_state = RD_INIT;
    peer->target = NULL;
    peer->sizeleft = 0;
    peer->pak = NULL;
    peer->out_of_window_cnt = 0;
    peer->ip_int_seqnumin = 0;
    peer->ip_int_seqnumout = 0;

    peer->outidb = outidb;
    peer->swidb = swidb;
    if (macaddr)
        ieee_copy(macaddr, peer->macaddr);
    else
        ieee_zero(peer->macaddr);
    peer->idbnext = NULL;
    peer->fr_dlci = fr_dlci;
    peer->fr_state = FR_DISCONN;

    peer->dlsw_lsap_out = 0;
    peer->netbios_h_oacf = NULL;
    peer->netbios_b_oacf = NULL;
    peer->dlsw_dmac_outlist = 0;
    bzero(&peer->dlsw_dmac_out, IEEEBYTES);

    peer->drops = 0;
    peer->pkts_rx = 0;
    peer->pkts_tx = 0;

    peer->peer_index = PeerIndexInitValue();
    
    return(peer);
}


void init_pod_defaults()
{
    /*
     * Initialize default peer on demand defaults.
     */
    lpeer->pod_defaults.next = NULL;
    lpeer->pod_defaults.ple = NULL;
    lpeer->pod_defaults.qptr = NULL;
    lpeer->pod_defaults.ipaddr = 0;
    lpeer->pod_defaults.peer_type = DEMAND_PEER;
    lpeer->pod_defaults.xport_type = DLSW_TCP;
    lpeer->pod_defaults.local_ack = TRUE;
    lpeer->pod_defaults.priority = FALSE;
    lpeer->pod_defaults.max_tcp_qlen = DLSW_MAX_TCP_QLEN;
    lpeer->pod_defaults.cost = DLSW_NORM_PCOST;
    lpeer->pod_defaults.keepalive_int = DLSW_DEF_KEEP_INT;
    lpeer->pod_defaults.netbios_h_oacf = NULL;
    lpeer->pod_defaults.netbios_b_oacf = NULL;
    lpeer->pod_defaults.dlsw_lsap_out = 0;
    lpeer->pod_defaults.dlsw_dmac_outlist = 0;
    ieee_copy(zeromac, lpeer->pod_defaults.dlsw_dmac_out);
    lpeer->pod_defaults.pod_inactivity = DLSW_POD_DEF_DISC_DELAY * ONEMIN;
    lpeer->pod_defaults.largest_frame_def = RC_LFINITIAL;
    lpeer->pod_defaults.largest_frame = RC_LFINITIAL;
}

void init_prompeer_defaults()
{
    /*
     * Initialize default prom peers defaults.
     */
    lpeer->prompeer_defaults.next = NULL;
    lpeer->prompeer_defaults.ple = NULL;
    lpeer->prompeer_defaults.qptr = NULL;
    lpeer->prompeer_defaults.ipaddr = 0;
    lpeer->prompeer_defaults.peer_type = PROM_PEER;
    lpeer->prompeer_defaults.xport_type = DLSW_TCP;
    lpeer->prompeer_defaults.local_ack = TRUE;
    lpeer->prompeer_defaults.priority = FALSE;
    lpeer->prompeer_defaults.max_tcp_qlen = DLSW_MAX_TCP_QLEN;
    lpeer->prompeer_defaults.cost = DLSW_NORM_PCOST;
    lpeer->prompeer_defaults.keepalive_int = -1;
    lpeer->prompeer_defaults.netbios_h_oacf = NULL;
    lpeer->prompeer_defaults.netbios_b_oacf = NULL;
    lpeer->prompeer_defaults.dlsw_lsap_out = 0;
    lpeer->prompeer_defaults.dlsw_dmac_outlist = 0;
    ieee_copy(zeromac, lpeer->prompeer_defaults.dlsw_dmac_out);
    lpeer->prompeer_defaults.dlsw_lsap_out = 0;
    lpeer->prompeer_defaults.pod_inactivity = DLSW_POD_DEF_DISC_DELAY * ONEMIN;
    lpeer->prompeer_defaults.largest_frame_def = RC_LFINITIAL;
    lpeer->prompeer_defaults.largest_frame = RC_LFINITIAL;
}


/*
 * peer_create_local()
 *
 * Creates and initializes the local peer for this box. Without the local peer 
 * definition, the Peer Connection Manager will not function.
 */

boolean peer_create_local (ipaddrtype ipaddr, uchar largest, ushort group, 
                         boolean border, ushort cost, ulong keep_int, 
                         uchar conn_mode, boolean pod_enabled,
                         boolean sna_segment, ushort init_pacing_win,
			 ushort max_pacing_win)
{
    int lf_size;
    ulong ii;
    peerentry *peer;

    if (lpeer) {
        /*
         * local peer already exists.
         */
        if (lpeer->ipaddr != ipaddr) {
            printf("\n%%Must remove the local-peer to change ip address");
            return(FALSE);
        }
        if (lpeer->cap.group != group) {
            printf("\n%%Must remove local-peer to change peer group");
            return(FALSE);
        }
        if (lpeer->cap.border != border) {
            printf("\n%%Must remove local-peer to change border capability");
            return(FALSE);
        }
        if (lpeer->cap.cost != cost) {
            printf("\n%%Must remove local-peer to change cost");
            return(FALSE);
        }
        if (lpeer->cap.initial_pacing_window != init_pacing_win) {
             printf("\n%%Must remove local-peer to change initial pacing window");
             return(FALSE);
        }
        if (lpeer->max_pacing_window != max_pacing_win) {
             printf("\n%%Must remove local-peer to change maximum pacing window");
             return(FALSE);
        }
        if (lpeer->cap.spoof_and_seg != sna_segment) {
            printf("\n%%Must remove local-peer to change biu-segment option");
            return(FALSE);
        }

        if ((largest != 0) && (lpeer->largest_frame > largest))
            printf("\n%%lf change will not affect existing circuits");

        if ((lpeer->conn_mode != conn_mode) && (conn_mode == PASSIVE_MODE))
            printf("\n%%Passive change will not affect existing connections");

        if (lpeer->cap.pod_enabled != pod_enabled)
            printf("\n%%promiscuous change will not affect existing peer connections");

        /*
         * Change local peer fields that are allowed to be changed without first
         * removing the local peer definition.
         */
        lpeer->largest_frame = largest;
        if (keep_int == -1)  
            keep_int = DLSW_DEF_KEEP_INT;
        if (lpeer->keepalive_int != keep_int) {
            lpeer->keepalive_int = keep_int;
            adjust_remote_keepalives(keep_int);
        }

        if ((lpeer->conn_mode != conn_mode) &&
            (conn_mode == NORMAL_MODE)) {
            /*
             * "passive" removed from local peer config.
             * Schedule remote peers to connect.
             */
            peer = (peerentry *) disc_peerQ.qhead;
            for (; peer; peer = peer->next) {
                if ((peer->peer_type == CONF_PEER) && (!peer->remove) &&
                    (!dlsw_disable) && (!peer->primary_peer))
                    mgd_timer_start(&peer->conn_timer, 0);
            }
        }

        lpeer->conn_mode = conn_mode;
        lpeer->cap.cost = cost;
        lpeer->cap.pod_enabled = pod_enabled;
        return(TRUE);

    }

    lf_size = rc_code_to_bytes(largest);
    if (lf_size == -1) {
        printf("Valid largest frame sizes are %d, %d, %d, %d, %d, %d, %d, %d and %d.",
               RC_LF516_VAL, RC_LF1470_VAL, RC_LF1500_VAL,
               RC_LF2052_VAL, RC_LF4472_VAL, RC_LF8144_VAL,
               RC_LF11407_VAL, RC_LF11454_VAL, RC_LF17800_VAL);
        return(FALSE);
    }

    lpeer = malloc(sizeof(localpeer_t));
    if (!lpeer) {
        return(FALSE);
    }
  
    lpeer->ipaddr = ipaddr;
    lpeer->largest_frame = largest;
    if (keep_int == -1)
        lpeer->keepalive_int = DLSW_DEF_KEEP_INT;
    else
        lpeer->keepalive_int = keep_int;
    lpeer->conn_mode = conn_mode;
    lpeer->max_pacing_window = max_pacing_win;

    bcopy(cisco_snap, lpeer->cap.vendor_id, SNAP_OUIBYTES);
    lpeer->cap.cap_known = TRUE;
    lpeer->cap.cap_ack = TRUE;
    lpeer->cap.cap_sent = TRUE;
    lpeer->cap.version_number = AIW_CP_VERS;
    lpeer->cap.release_number = AIW_CP_REL;
    lpeer->cap.version_string = version;
    lpeer->cap.initial_pacing_window = init_pacing_win;
    for (ii = 0; ii < SAP_CV_BYTES; ii++)
        lpeer->cap.supp_saps[ii] = ~0;
    lpeer->cap.num_tcp_sessions = CISCO_NUM_TCP_PIPES;
    lpeer->cap.loop_prevent_support = FALSE;
    lpeer->cap.mac_exclusive = FALSE;
    lpeer->cap.nb_exclusive = FALSE;
    queue_init(&lpeer->cap.macQ, 0);
    queue_init(&lpeer->cap.nbnameQ, 0);
    lpeer->cap.cisco_version = CISCO_VERSION_1;
    lpeer->cap.local_ack = TRUE;
    lpeer->cap.priority = TRUE;
    lpeer->cap.group = group;
    lpeer->cap.cost = cost;
    lpeer->cap.border = border;
    lpeer->cap.pod_enabled = pod_enabled;
    lpeer->cap.spoof_and_seg = sna_segment;
    lpeer->cap.xid_op_support = TRUE;
    lpeer->border_peer = NULL;
    lpeer->reach_flag = 0;
    init_pod_defaults();
    init_prompeer_defaults();
    dlsw_start();
    return(TRUE);
}
         


boolean dlsw_is_local_mac_excl ()
{
    if (lpeer) {
        if (lpeer->icr_mac_excl_rem) {
            return (FALSE);
        } else {
            return (lpeer->cap.mac_exclusive);
        }
    }
 
    return(FALSE);
}

boolean dlsw_is_local_nb_excl ()
{
    if (lpeer) {
        if (lpeer->icr_nb_excl_rem) {
            return (FALSE);
        } else {
            return (lpeer->cap.nb_exclusive);
        }
    }
 
    return(FALSE);
}

boolean dlsw_is_peer_mac_excl (peerhandle_t peer_handle)
{

    return(((peerentry *)peer_handle)->cap.mac_exclusive);

}

boolean dlsw_is_peer_nb_excl (peerhandle_t peer_handle)
{

    return(((peerentry *)peer_handle)->cap.nb_exclusive);

}

boolean dlsw_is_peer_being_removed (peerhandle_t peer_handle)
{

    return(((peerentry *)peer_handle)->remove);

}



/*
 * peer_remove_local()
 *
 * Removes the local peer definition for this box. Without the local peer
 * definition, the Peer Connection Manager will not function.
 */
 
void peer_remove_local ()
{
    macentry_t *macentry;
    nbentry_t *nbentry;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);
    /*
     * Remove all remote peer definitions.
     */
    peer_remove_all_remote();

    if (lpeer) {
        lpeer->border_peer = NULL; 
        if (lpeer->cap.macQ.qhead) {
            while ((macentry = p_dequeue(&lpeer->cap.macQ)))
                free(macentry);
        }
        if (lpeer->cap.nbnameQ.qhead) {
            while ((nbentry = p_dequeue(&lpeer->cap.nbnameQ)))
                free(nbentry);
        }
        lpeer = NULL; 
    }
    reset_interrupt_level(status);
}


/*
 * new_remote_peer()
 */

peerentry *new_remote_peer (ushort fr_dlci, ipaddrtype ipaddr, peer_t ptype,
          portlistentry *ple, uchar xport, boolean lack, boolean priority,
          ushort cost, ushort lf_code, long keep_int, ulong lsap_list,
          netbios_acclisttype *nb_hlist, netbios_acclisttype *nb_blist,
          ushort tcp_qlen, hwidbtype *outidb, uchar *macaddr, 
          peerentry *primary, ulong tcp_timeout, int backup_linger,
	  int dynamic_inactivity, int no_llc, int dlsw_dmac_outlist,
	  uchar *dlsw_dmac_out, idbtype *swidb, uchar conn_mode)
{
    ulong ii;
    peerentry *peer;
    boolean is_backup;
    char buff[DLSW_MAX_PEER_ID];

    if (primary)
        is_backup = TRUE;
    else
        is_backup = FALSE;

    peer = create_peer(TRUE, fr_dlci, ipaddr, ptype, ple, xport, lack,
                       cost, lf_code, tcp_qlen, outidb, macaddr, swidb, 
                       is_backup, conn_mode);
    if (!peer) {
        return(NULL);
    }

    peer->netbios_h_oacf = nb_hlist;
    peer->netbios_b_oacf = nb_blist;
    peer->dlsw_lsap_out = lsap_list;

    if (keep_int != -1) {
        peer->keepalive_int = keep_int;
        peer->took_def_keep = FALSE;
    }
    if (ptype == DYNAMIC_PEER) {
	 peer->keepalive_int = 0;
	 peer->dynamic_inactivity = dynamic_inactivity;
	 if (!no_llc)
	      peer->no_llc = 5 * ONEMIN;
	 else
	      peer->no_llc = no_llc;
	 queue_init(&peer->drpQ, 1);
    }
    if (tcp_timeout)
        peer->tcp_timeout = tcp_timeout;
    else
	 peer->keepalive_int ? (peer->tcp_timeout = DLSW_KEEP_CNT_THOLD
	 * peer->keepalive_int) : (peer->tcp_timeout =
	 DLSW_KEEP_CNT_THOLD * DLSW_DEF_KEEP_INT);
    peer->keepalive_count = 0;
    peer->primary_peer = primary;
    peer->backup_linger = backup_linger;
    TIMER_STOP(peer->linger_timer);
    GET_TIMESTAMP(peer->config_tstamp);
    peer->dlsw_dmac_outlist = dlsw_dmac_outlist;
    if (dlsw_dmac_out)
	 ieee_copy(dlsw_dmac_out, peer->dlsw_dmac_out);
    if (primary)
        primary->backup_peer = peer;
    peer->priority = priority;
    if (xport == DLSW_TCP)
        peer->port = DLSW_RD_PORT;

    if (!peer_init_remote_xport(peer, swidb, lf_code)) {
        PDEBUG("\nDLSw: peer_init_remote_xport failed for %s", 
               peer_str(peer, buff));
	mgd_timer_stop(&peer->conn_timer);
        peer_nuke(peer);
        return(NULL);
    }

    if (priority) {
        for (ii = 1; ii <= DLSW_PRIORITY_COUNT; ii++) {
            peer->priority_peers[ii] = create_peer(FALSE, fr_dlci,
                                ipaddr, ptype, ple, xport, lack, cost,
                                lf_code, tcp_qlen, outidb, macaddr, NULL,
                                FALSE, conn_mode);

            if (!peer->priority_peers[ii]) {
                peer_nuke(peer);
                return(NULL);
            }
            peer->priority_peers[ii]->keepalive_int = peer->keepalive_int;
            peer->priority_peers[ii]->keepalive_count = 0;
            peer->priority_peers[ii]->took_def_keep = peer->took_def_keep;
            peer->priority_peers[ii]->priority_top = peer;
            peer->priority_peers[ii]->priority = FALSE;
            peer->priority_peers[ii]->wait_pri_connect = FALSE;
            peer->priority_peers[ii]->netbios_h_oacf = nb_hlist;
            peer->priority_peers[ii]->netbios_b_oacf = nb_blist;
            peer->priority_peers[ii]->dlsw_lsap_out = lsap_list;
            peer->priority_peers[ii]->qptr = &disc_peerQ;
            if (!peer_init_remote_xport(peer->priority_peers[ii], swidb, lf_code)) {
                PDEBUG("\nDLSw: peer_init_remote_xport failed for %s", 
                       peer_str(peer, buff));
		mgd_timer_stop(&peer->conn_timer);
                peer_nuke(peer);
               return(NULL);
            }
        }
        peer->priority_peers[1]->port = DLSW_P1_PORT;
        peer->priority_peers[2]->port = DLSW_P2_PORT;
        peer->priority_peers[3]->port = DLSW_P3_PORT;
    } else {
        for (ii = 1; ii <= DLSW_PRIORITY_COUNT; ii++)
            peer->priority_peers[ii] = NULL;
    }

    /*
     * Queue peer entry in disconnect list. p_enqueue() checks if entry is
     * already in the list and locks interrupts during add to list.
     */
    peer->conn_state = PEER_DISC;
    peer->qptr = &disc_peerQ;
    p_enqueue(&disc_peerQ, peer);
    return(peer);
}


/*
 * peer_create_remote()
 *
 * If called beccause a remote peer was just configured, it is the
 *  responsibility of the caller to check the configuration commands syntax. 
 */
 
peerentry *peer_create_remote (ushort fr_dlci, ipaddrtype ipaddr, peer_t ptype, 
          portlistentry *ple, uchar xport, boolean lack, boolean priority, 
          ushort cost, ushort lf_code, long keep_int, ulong lsap_list, 
          netbios_acclisttype *nb_hlist, netbios_acclisttype *nb_blist, 
          ushort tcp_qlen, hwidbtype *outidb, uchar *macaddr, 
          peerentry *primary, ulong tcp_timeout, int backup_linger,
	  int dynamic_inactivity, int no_llc, int dlsw_dmac_outlist,
	  uchar *dlsw_dmac_out, idbtype *swidb, uchar conn_mode)
{
    peerentry *peer;

    if (!lpeer) {
        printf("\n%%DLSw local-peer must be configured first."); 
        return(NULL);
    } 

    if ((!lpeer->ipaddr) &&
        ((xport == DLSW_TCP) || (xport == DLSW_FST))) {
        printf("\n%%Remote peers can't use TCP or FST since the peer-id");
        printf("\n%%was not provided on the local-peer definition.");
        return(NULL);
    }

    /*
     * Does this remote peer already exist?
     */
    switch(xport) {
      case DLSW_TCP:
      case DLSW_FST:
        peer = (peerentry *) peer_ip_to_handle(ipaddr);
        break;

      case DLSW_IF:
      case DLSW_IF_LAN:
        peer = (peerentry *) peer_hwidb_to_handle(outidb);
        break;

      case DLSW_FR:
        peer = (peerentry *) peer_swidb_dlci_to_handle(swidb, fr_dlci);
        break;
     
      default:
        printf("\n%%Invalid transport type, interface, or address");
        return(NULL);           
    }

    if (peer) {
        /*
         * Remote peer already exists.
         */
        if (peer->xport_type != xport) {
            printf("\n%%error: peers with same ipaddr & different transport types");
            return(NULL);
        }
        if (peer->ple != ple) {
            printf("\n%%port-list change will not effect existing circuits");
        }
        /*
         * local-ack option not supported in release 1.
         *
         * if (peer->local_ack != lack) {
         *     printf("\n%%Must remove the remote-peer to change local-ack option");
         *     return(NULL);
         *  }
         */
        if (peer->priority != priority) {
            printf("\n%%Must remove the remote-peer to change priority option");
            return(NULL);
        }
        if (peer->max_tcp_qlen != tcp_qlen) {
            printf("\n%%Must remove the remote-peer to change tcp-queue-max");
            return(NULL);
        }
        if (!ieee_equal(peer->macaddr, macaddr)) {
            printf("\n%%Must remove the remote-peer to change the mac-address");
            return(NULL);
        }

        if ((lf_code != RC_LFINITIAL) && (peer->largest_frame != lf_code)) {
            printf("\n%%Must remove the remote-peer to change the lf");
            return(NULL);
        }

        if (peer->primary_peer != primary) {
            printf("\n%%Must remove the remote-peer to change backup option");
            return(NULL);
        }

        if ((peer->conn_mode != conn_mode) && (conn_mode == PASSIVE_MODE)) {
            printf("\n%%Passive change will not affect existing connection");
        }

        /*
         * Update remote-peer fields that can be changed without removing the 
         * remote peer.
         */
        peer->ple = ple;
        peer->netbios_h_oacf = nb_hlist;
        peer->netbios_b_oacf = nb_blist;
        peer->dlsw_lsap_out = lsap_list;
        peer->peer_type = ptype;

        if (keep_int == -1) {
            peer->keepalive_int = lpeer->keepalive_int;
            peer->took_def_keep = TRUE;
	    
        } else {
            peer->keepalive_int = keep_int;
            peer->took_def_keep = FALSE;
        }
	if (ptype == DYNAMIC_PEER) {
	     peer->keepalive_int = 0; /* we don't want keepalives */
	     peer->dynamic_inactivity = dynamic_inactivity;
	     if (!no_llc)
		  peer->no_llc = 5 * ONEMIN;
	     else
		  peer->no_llc = no_llc;
	}
	if (tcp_timeout)
	    peer->tcp_timeout = tcp_timeout;
	else
	      peer->keepalive_int ? (peer->tcp_timeout = DLSW_KEEP_CNT_THOLD
	      * peer->keepalive_int) : (peer->tcp_timeout =
	      DLSW_KEEP_CNT_THOLD * DLSW_DEF_KEEP_INT);
        peer->keepalive_count = 0;
	TIMER_STOP(peer->last_passive_open);
        peer->cost = cost;
        peer->primary_peer = primary;
	peer->backup_linger = backup_linger;
	TIMER_STOP(peer->linger_timer);
        GET_TIMESTAMP(peer->config_tstamp);
        TIMER_STOP(peer->connect_tstamp);
        TIMER_STOP(peer->disc_tstamp);
	peer->dlsw_dmac_outlist = dlsw_dmac_outlist;
	ieee_copy(dlsw_dmac_out, peer->dlsw_dmac_out);
	
        if ((peer->conn_mode != conn_mode) &&
            (conn_mode == NORMAL_MODE)) {
            /*
             * "passive" removed from remote peer config.
             * Schedule remote peer to connect, if not already.
             */
            if (peer->conn_state == PEER_DISC) {
	        if ((peer->peer_type == CONF_PEER) && (!peer->remove) &&
	            (!dlsw_disable) && (!peer->primary_peer)) {
		    mgd_timer_start(&peer->conn_timer, 0);
                }
            } 
        }
        peer->conn_mode = conn_mode;
        return(peer);
    } 

    /*
     * New remote-peer definition.
     */
    peer = new_remote_peer(fr_dlci, ipaddr, ptype, ple, xport, lack, priority,
                           cost, lf_code, keep_int, lsap_list, nb_hlist,
                           nb_blist, tcp_qlen, outidb, macaddr,
			   primary, tcp_timeout, backup_linger,
			   dynamic_inactivity, no_llc,
			   dlsw_dmac_outlist, dlsw_dmac_out, swidb, conn_mode);

    return(peer);
}
 
/*
 * peer_remove_remote()
 */

void peer_remove_remote (peerentry *peer)
{
    peer->remove = TRUE; 
    TIMER_STOP(peer->config_tstamp);
    dlsw_peer_fsm(peer, CLOSE_CONNECTION);
}

/*
 * dlsw_peer_newstate()
 *
 * Changes the state of the peer to 'newstate' and performs required
 * administrative tasks that accompany the state change.
 * Should only be called from the peer fsm so peer is guaranteed
 * to be the high priority peer.
 */
void dlsw_peer_newstate (peerentry *peer, peerstatetype newstate)
{
    leveltype status;
    queuetype *new_qptr;
    ulong ii;
    char buff[DLSW_MAX_PEER_ID];
    paktype *drp_pak; /* dynamic peer start pak */
    int media_lf;

    /*
     * Admin processing required when a peer is disconnected.
     */
    if ((newstate != PEER_DISC) && (peer->conn_state == newstate))
        return;

    PDEBUG("\nDLSw: %s, old state %s, new state %s", peer_str(peer, buff),
           state_to_string(peer->conn_state),
	   state_to_string(newstate));

    /* Processing DLSw MIB counters */
    if ((peer->conn_state != PEER_CONN) && (newstate == PEER_CONN)) {
            ++peer->config_opens;
    }
    if ((peer->conn_state == PEER_DISC) && (newstate != PEER_DISC)) {
        /* changing state from disc to something else ... */
        ++conn_active_connection;
    } else if ((peer->conn_state != PEER_DISC) && (newstate == PEER_DISC)) {
        peer->disc_active_circuits = peer->num_ckts;
        --conn_active_connection;
        if (peer->num_ckts) {
            ++conn_close_busy;
        } else {
            ++conn_close_idle;
        }
    }

    if (newstate == PEER_DISC) {
        new_qptr = &disc_peerQ;
	if (peer->peer_type == DYNAMIC_PEER) {
	    if ((drp_pak = pak_dequeue(&peer->drpQ)))
                datagram_done(drp_pak);
        }
        
        peer->peer_status = PEER_NOT_READY;
        if (peer->priority) {
            for (ii = 1; ii <= DLSW_PRIORITY_COUNT; ii++) {
                if (peer->priority_peers[ii])
                    peer->priority_peers[ii]->peer_status = PEER_NOT_READY;
            }
        }
        mgd_timer_stop(&peer->keep_timer);

        if ((peer->peer_type == CONF_PEER) && (!peer->remove) && 
            (!peer->active_open_wait) && (!dlsw_disable) &&
            ((peer->primary_peer == NULL) || (peer->backup_connect)) &&
            (lpeer->conn_mode != PASSIVE_MODE) && 
            (peer->conn_mode != PASSIVE_MODE)) {
            mgd_timer_start(&peer->conn_timer,peer_next_conn_interval(peer));

            /*
             * If peer has a backup, open the connection with it
             * depending on what this peer (the primary peer) is doing.
             * Can't open and close peers at interrupt level.
             */
            if ((peer->backup_peer) && (!peer->backup_peer->remove) &&
                (peer->backup_peer->conn_state == PEER_DISC) &&
                (peer->backup_peer->backup_connect == FALSE)) {
                 peer->backup_peer->backup_connect = TRUE;
                 mgd_timer_start(&peer->backup_peer->conn_timer, 0);
                 peer->backup_peer->conn_retries = 0;
            }
        } else {
            mgd_timer_stop(&peer->conn_timer);
        }
    } else if (newstate == PEER_CONN) {
        /*
         * If peer has a backup, close the connection with it
         * depending on what this peer (the primary peer) is doing.
         * Can't open and close peers at interrupt level.
         */
        if (peer->backup_peer) {
            peer->backup_peer->backup_connect = FALSE;
            PDEBUG("\nDLSw Clear cache for %i", peer->backup_peer->ipaddr);
            peer_del_cache(peer->backup_peer);
            if (peer->backup_peer->backup_linger == 0)
                dlsw_peer_fsm(peer->backup_peer, CLOSE_CONNECTION);
            else if (peer->backup_peer->backup_linger > 0)
                TIMER_START(peer->backup_peer->linger_timer,
                            peer->backup_peer->backup_linger);
        }
        if ((peer->xport_type == DLSW_IF) || (peer->xport_type == DLSW_FR)) {
	     media_lf = bytes_to_rc_code(peer->outidb->maxmtu);
	     if (peer->largest_frame > media_lf) {
	         errmsg(PEER_LFCHANGE,
			"WAN media mtu maximum is",
			rc_code_to_bytes(media_lf), "lowering peer lf");
		 peer->largest_frame = media_lf;
	     }
	}
        new_qptr = &wait_conn_peerQ;
    } else
        new_qptr = NULL;

    peer->conn_state = newstate;
    if ((new_qptr) && (peer->qptr != new_qptr)) {
        status = raise_interrupt_level(NETS_DISABLE);
        peer->change_flag = TRUE;
        peer->conn_retries = 0;

        unqueue(peer->qptr, peer);
        peer->qptr = new_qptr;
        enqueue(peer->qptr, peer);

        if (newstate == PEER_DISC) {
            if ((peer->cap.group) && (peer->cap.border))
                delete_border_peer_info(peer);
            reset_peer(peer);
        }
        reset_interrupt_level(status);

        if (newstate == PEER_CONN) {
            update_peer_cap(peer);
            peer->connect_pod = FALSE;
	    if (peer->peer_type == DYNAMIC_PEER) {
		 drp_pak = pak_dequeue(&peer->drpQ);
		 if (drp_pak)
		      (*peer->sender)(peer, drp_pak, TRUE, PEER_BCAST,
				      NULL);
	    }
            TIMER_STOP(peer->last_passive_open);
	    GET_TIMESTAMP(peer->connect_tstamp);
        } else if (newstate == PEER_DISC) {
            GET_TIMESTAMP(peer->disc_tstamp);
        }
    } else if (newstate == PEER_DISC) {
        /*
         * Handles the case where the peer never changes queues but
         * received a partner's CapExId message.
         */
        clear_non_conf_info(peer);

        /*
         * Remove promiscuous peers and pods that did not connect.
         */
        if ((peer->remove) || (peer->peer_type == PROM_PEER) || 
             (peer->peer_type == DEMAND_PEER))
            reset_peer(peer);
    } else {
        /*
         * If transport ready (indicated by a PEER_WAIT_CAP state),
         * send our capabilities.
         */
        if (newstate == PEER_WAIT_CAP) {
            GET_TIMESTAMP(peer->conn_start_time);
            /*
             * If peer is not promiscuous and cap msg not sent, send it.
             * If peer is promiscuous and we know partner's cap and
             * cap msg not sent, send it.
             */
            if (((peer->peer_type != PROM_PEER) || (peer->cap.cap_known)) &&
                (!peer->cap.cap_sent)) {
                if (send_capabilities(peer))
                    peer->cap.cap_sent = TRUE;
                else if (peer->peer_type == DEMAND_PEER)
                    peer_remove_remote(peer);
            }
        }
    }
}

/*
 * update_peer_cap()
 *
 * Notify CSM of all filter learned through capabilities exchange.
 */

void update_peer_cap (peerentry *peer)
{
    macentry_t *macentry;
    nbentry_t *nbentry;
    config_obj_u obj;

    if (!peer)
        return;

    csm_clear_cap_entries((peerhandle_t) peer);
    obj.peer_id = (peerhandle_t) peer;
    for (macentry = (macentry_t *) peer->cap.macQ.qhead;
	 macentry;
	 macentry = macentry->next) {
        csm_update_mac_entry(SET, CAP_XCHANGE, macentry->macaddr, 
                       macentry->macmask, REACHABLE, PEER_ID, obj);
    }
    for (nbentry = (nbentry_t *) peer->cap.nbnameQ.qhead;
	 nbentry;
	 nbentry = nbentry->next) {
	/*
	 * Do not enter wildcard entries into csm cache.
	 * CSM is not using them
	 */
	if (!str_has_wc(nbentry->nbname)) {
            csm_update_nbname_entry(SET, CAP_XCHANGE, nbentry->nbname,
                                    REACHABLE, PEER_ID, obj);
	}
    }
}


/*
 * peer_remove_all_remote()
 *
 * Remove all remote peer definitions.
 * Useful when the local peer is de-configured or for system shutdown. 
 */
 
void peer_remove_all_remote ()
{
    ulong ii;
    peerentry *peer;
    peerentry *next;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);
    /*
     * Disconnect connected DLX peers.
     */
    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
        while ((peer = (peerentry *) conn_dlx_peerQ[ii].qhead)) {
            peer_remove_remote(peer);
        }
    }
    /*
     * Disconnect connected DLSw peers.
     */
    while ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        peer_remove_remote(peer);
    }

    /*
     * Disconnect peers waiting to have their capoabilities acted upon.
     */
    while ((peer = (peerentry *) wait_conn_peerQ.qhead)) {
        peer_remove_remote(peer);
    }

    /*
     * Mark disconnected peer definitions to be removed.
     */
    peer = (peerentry *) disc_peerQ.qhead;
    while (peer) {
        next = peer->next;
        peer_remove_remote(peer);
        peer = next;
    }
    reset_interrupt_level(status);
}


/*
 * peer_nuke_one
 */

static void peer_nuke_one (peerentry *peer)
{
    peerentry *temp;
    macentry_t *macentry;
    nbentry_t *nbentry;

    if (peer->xport_type == DLSW_FST) {
        free(peer->ip_header);
    } else if (peer->xport_type == DLSW_IF) {
        /*
         * If this peer was marked to be "removed", then another
         * direct peer could be configured on this interface.
         */
        if (peer == peer->outidb->dlsw_ifpeer)
            peer->outidb->dlsw_ifpeer = NULL;
    } else if (peer->xport_type == DLSW_FR) {
        if (peer == peer->outidb->dlsw_ifpeer)
         peer->outidb->dlsw_ifpeer = NULL;
    } else if (peer->xport_type == DLSW_IF_LAN) {
        /*
         * Unchain this entry.  Case 1: head of the queue.  Case 2:
         * somewhere else in the queue.
         */
        if (peer->outidb->dlsw_ifpeer == peer) {
            peer->outidb->dlsw_ifpeer = peer->idbnext;
        } else {
            temp = peer->outidb->dlsw_ifpeer;
            for ( ; temp; temp = temp->idbnext) {
                if (temp->idbnext == peer) {
                    temp->idbnext = temp->idbnext->idbnext;
                    break;
                }
            }
        }
    }

    if (peer->cap.macQ.qhead) {
        while ((macentry = p_dequeue(&peer->cap.macQ)))
            free(macentry);
    }
    if (peer->cap.nbnameQ.qhead) {
        while ((nbentry = p_dequeue(&peer->cap.nbnameQ)))
            free(nbentry);
    }
    peer->change_flag = TRUE;
    peer->next = NULL;
    peer->peer_status = PEER_DEAD;
    free(peer);
}


/*
 * clear_non_conf_info()
 */
static void clear_non_conf_info (peerentry *peer)
{
    macentry_t *macentry;
    nbentry_t *nbentry;
    ulong ii;

    peer->out_of_window_cnt = 0;
    peer->ip_int_seqnumin = 0;
    peer->ip_int_seqnumout = 0;
    bcopy(zerosnap, peer->cap.vendor_id, SNAP_OUIBYTES);
    peer->cap.version_number = 0;
    if (peer->cap.version_string) {
        free(peer->cap.version_string);
        peer->cap.version_string = NULL;
    }
    peer->cap.cap_known = FALSE;
    peer->cap.cap_ack = FALSE;
    peer->cap.cap_sent = FALSE;
    peer->cap.initial_pacing_window = 0;
    for (ii = 0; ii < SAP_CV_BYTES; ii++)
        peer->cap.supp_saps[ii] = ~0;
    peer->cap.num_tcp_sessions = DLSW_DEF_NUM_TCP_PIPES;
    peer->cap.loop_prevent_support = FALSE;
    peer->cap.mac_exclusive = FALSE;
    peer->cap.nb_exclusive = FALSE;
    if (peer->cap.macQ.qhead) {
        while ((macentry = p_dequeue(&peer->cap.macQ)))
            free(macentry); 
    }
    if (peer->cap.nbnameQ.qhead) {
        while ((nbentry = p_dequeue(&peer->cap.nbnameQ)))
            free(nbentry); 
    }
    peer->cap.local_ack = FALSE;
    peer->cap.priority = FALSE;
    peer->cap.group = DLSW_UNKNOWN_PGROUP;
    peer->cap.cost = DLSW_UNKNOWN_PCOST;
    peer->cap.border = FALSE;
    peer->cap.pod_enabled = FALSE;
    peer->cap.spoof_and_seg = FALSE;
    peer->cap.xid_op_support = FALSE;
    peer->cap.peer_type = CONF_PEER;

    peer->rd_state = RD_INIT;
    peer->target = NULL;
    peer->sizeleft = 0;
    peer->pak = NULL;
    peer->keepalive_count = 0;
    peer->wait_pri_connect = FALSE;
    peer->close_write_pending = FALSE;
    GET_TIMESTAMP(peer->last_non_peer_traffic);
    GET_TIMESTAMP(peer->zero_ckt_time);
}


/*
 * reset_peer()
 *
 * Clear non-configured information in remote peer structure.
 *  -- learned capabilities
 *  -- session counts
 */

static void reset_peer (peerentry *peer)
{
    peer_msg_u peer_msg;
    char buff[DLSW_MAX_PEER_ID];

    if (peer->active_open_wait) {
        errmsg(PEER_GENERAL, "Not removing blocked peer");
        PDEBUG("\nDLSw: action_d(): not removing blocked %s",
               peer_str(peer, buff));
        peer->tcp_fini_pending = TRUE;
        return;
    }

    if ((peer->peer_type == PROM_PEER) || (peer->peer_type == DEMAND_PEER)) {
        peer->remove = TRUE;
    }

    /*
     * Core would like to know when a peer connection is lost.
     */
    peer_msg.type = PEER_KILL_HANDLE;
    peer_msg.p_kill_handle.peer_handle = (peerhandle_t) peer;
    peer_to_core(&peer_msg);
    if (is_fast_peer((peerhandle_t) peer))
        dlsw_fct_clear_peer((peerhandle_t) peer);
    clear_non_conf_info(peer);
}

/*
 * peer_del_cache()
 *
 * Clear cache entries for a remote peer.
 *  
 */

static void peer_del_cache (peerentry *peer)
{
    peer_msg_u peer_msg;

    /*
     * Tell the core to delete the cache entries for this peer.
     */
    peer_msg.type = PEER_DEL_CACHE;
    peer_msg.p_del_cache.peer_handle = (peerhandle_t) peer;
    peer_to_core(&peer_msg);
}

/*
 * peer_nuke()
 */

void peer_nuke (peerentry *peer)
{
    uint i;
    leveltype status;

    status = raise_interrupt_level(NETS_DISABLE);
    if (peer->qptr)
        unqueue(peer->qptr, peer);

    if (peer->priority) {        
        for (i = 1; i <= DLSW_PRIORITY_COUNT; i++)
          peer_nuke_one(peer->priority_peers[i]);
          peer->priority_peers[i] = NULL;
    }
    peer_nuke_one(peer);
    reset_interrupt_level(status);
}


/*
 * adjust_remote_keepalives()
 */

static void adjust_remote_keepalives (ulong new_int)
{
    ulong ii;
    peerentry *peer;

    /*
     * Is this peer a connected DLX peer?
     */
    for (ii = 0; ii < DLSW_MAX_PGROUP; ii++) {
        if ((peer = (peerentry *) conn_dlx_peerQ[ii].qhead)) {
            for (; peer; peer = peer->next) {
                if (peer->took_def_keep) 
                    peer->keepalive_int = new_int;
            }
        }
    }
    /*
     * Is this peer a connected dlsw peer?
     */
    if ((peer = (peerentry *) conn_dlsw_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
                if (peer->took_def_keep)
                    peer->keepalive_int = new_int;
        }
    }
    /*
     * Is this a disconnected?
     */
    if ((peer = (peerentry *) disc_peerQ.qhead)) {
        for (; peer; peer = peer->next) {
            if (peer->took_def_keep)
                peer->keepalive_int = new_int;
        }
    }
}


/*
 * peer_str()
 */

char *peer_str (peerentry *peer, char *buff)
{
    if (!peer) {
        buff[0] = '\0';
	return(buff);
    } else if (peer->ipaddr)
        sprintf(buff,"peer %i(%d)", peer->ipaddr, peer->port);
    else if (peer->outidb)
        sprintf(buff,"peer on interface %s", peer->outidb->hw_namestring);
    else
        sprintf(buff,"UNKNOWN peer");
    return(buff);
}

boolean is_fast_peer (peerhandle_t peer_handle)
{
    peerentry *peer = (peerentry *) peer_handle;

    if (peer->xport_type == DLSW_TCP ||
        (peer->xport_type == DLSW_FR && peer->local_ack))
        return(FALSE);
    else
        return(TRUE);
}



/*
 * this function is used to update packets stats for use in DLSw MIB
 */
void peer_update_out_stat (peerentry *peer, paktype *pak)
{
    dlx_header_t *dlx_hdr;
    ssp_hdr_t    *ssp_hdr;
    int          msg_type;

    if (!peer)
        return;
    dlx_hdr = (dlx_header_t *) pak->datagramstart;
    if (dlx_hdr->proto_ver_id == DLX_PROTO_VERSION) {
        /*
         * right now we do not handle stats for dlx frames
         */
        return;
    }

    ssp_hdr = (ssp_hdr_t *) pak->datagramstart;
    msg_type = ssp_hdr->ssp_msg_type;
    if ((msg_type == SSP_OP_dataframe) 
             || (msg_type == SSP_OP_infoframe)
             || (msg_type == SSP_OP_dgmframe)) {
        ++peer->out_data_packets;
        peer->out_data_octets += GETSHORT(&ssp_hdr->ssp_msg_len);
    } else {
        ++peer->out_cntl_packets;
    }
    if (ssp_hdr->ssp_flags & SSP_FLAGS_EX) {
        switch (msg_type) {
          case SSP_OP_canureach:
            ++peer->cur_ex_sents;
            break;
          case SSP_OP_icanreach:
            ++peer->icr_ex_sents;
            break;
          case SSP_OP_netbios_nq:
            ++peer->nq_ex_sents;
            break;
          case SSP_OP_netbios_nr:
            ++peer->nr_ex_sents;
            break;
          default:
            break;
        }
    }
    return;
}



/*
 * this function is used to update packets stats for use in DLSw MIB
 */
void peer_update_in_stat (peerentry *peer, paktype *pak)
{
    dlx_header_t *dlx_hdr;
    ssp_hdr_t    *ssp_hdr;
    int          msg_type;

    if (!peer)
        return;
    dlx_hdr = (dlx_header_t *) pak->datagramstart;
    if (dlx_hdr->proto_ver_id == DLX_PROTO_VERSION) {
        /*
         * right now we do not handle stats for dlx frames
         */
        return;
    }

    ssp_hdr = (ssp_hdr_t *) pak->datagramstart;
    msg_type = ssp_hdr->ssp_msg_type;
    if ((msg_type == SSP_OP_dataframe) 
             || (msg_type == SSP_OP_infoframe)
             || (msg_type == SSP_OP_dgmframe)) {
        ++peer->in_data_packets;
        peer->in_data_octets += ssp_hdr->ssp_msg_len;
    } else {
        ++peer->in_cntl_packets;
    }
    if (ssp_hdr->ssp_flags & SSP_FLAGS_EX) {
        switch (msg_type) {
          case SSP_OP_canureach:
            ++peer->cur_ex_rcvds;
            break;
          case SSP_OP_icanreach:
            ++peer->icr_ex_rcvds;
            break;
          case SSP_OP_netbios_nq:
            ++peer->nq_ex_rcvds;
            break;
          case SSP_OP_netbios_nr:
            ++peer->nr_ex_rcvds;
            break;
          default:
            break;
        }
    }
    return;
}



