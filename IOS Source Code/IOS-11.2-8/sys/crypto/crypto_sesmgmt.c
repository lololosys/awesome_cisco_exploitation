/* $Id: crypto_sesmgmt.c,v 3.2.60.9 1996/09/10 23:28:39 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_sesmgmt.c,v $
 *------------------------------------------------------------------
 * cryto_sesmgmt.c - Crypto Session Management process
 *
 * May 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_sesmgmt.c,v $
 * Revision 3.2.60.9  1996/09/10  23:28:39  bew
 * CSCdi64360:  Bad and pending connection does not timeout
 * Branch: California_branch
 * Clear out the pending connection table entry as well
 * as connection table entry when manually clearing a connection.
 *
 * Revision 3.2.60.8  1996/08/28  12:39:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.7  1996/08/16  05:56:08  bew
 * CSCdi64252:  4500 crashes randomly after a long time during connection
 * setup
 * Branch: California_branch
 * Re-implement manner of finding pending crypto connections using
 * existing SDU structures.
 *
 * Revision 3.2.60.6  1996/07/15  10:05:18  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.2.60.5  1996/07/06  19:44:39  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.2.60.4  1996/07/01  09:06:00  xliu
 * CSCdi61744:  fail to select correct crypto maps when multiple are
 * created
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/06/07  00:24:21  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.60.2  1996/05/12  23:12:26  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/19  14:56:01  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.6.31  1996/04/17  23:47:34  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 2.1.6.30  1996/04/16  21:52:43  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.6.29  1996/04/12  23:07:40  carrel
 * Branch: IosSec_branch
 * Fix wildcarding in ACL for crypto map.  Also some cleanup.
 *
 * Revision 2.1.6.28  1996/03/01  01:39:14  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.6.27  1996/02/13  19:15:55  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.6.26  1996/01/25  18:19:56  bew
 * Branch: IosSec_branch
 * Implemented a config option for automatic pre-genereration of
 * Diffie-Hellman key pairs. Also changed the use of timers so
 * that the the STUCKTMR message doesn't show up any more.
 *
 * Revision 2.1.6.25  1996/01/18  23:23:17  bew
 * Branch: IosSec_branch
 * The crypto session manager no longer forks off connection messages but
 * handles them itself serially. This is due to SDU connection setup code
 * not handling the multitasking gracefully.
 *
 * Revision 2.1.6.24  1996/01/16  20:20:51  bew
 * Branch: IosSec_branch
 * Simplified the session mgmt debug msgs & changed the way that duplicate
 * connection messages are handled.
 *
 * Revision 2.1.6.23  1996/01/04  18:47:52  xliu
 * Branch: IosSec_branch
 * Crypto moduliztion.
 *
 * Revision 2.1.6.22  1995/12/22  00:23:26  bew
 * Branch: IosSec_branch
 * Connection messsages are now handled in their own processes. This is
 * necessary in order to maximize throughput of the H/W EPA.
 *
 * Revision 2.1.6.21  1995/12/20  23:30:50  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 2.1.6.20  1995/12/12  23:09:40  bew
 * Branch: IosSec_branch
 * Fixes for CS & better formatting of debug messages.
 *
 * Revision 2.1.6.19  1995/12/02  17:44:33  xliu
 * Branch: IosSec_branch
 *
 * Add parser command to precompute dh number.
 *
 * Revision 2.1.6.18  1995/11/28  01:46:39  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.17  1995/11/17  16:33:22  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.16  1995/10/14  04:53:17  xliu
 * Branch: IosSec_branch
 * Recheck in 2.1.6.14 version.
 *
 * Revision 2.1.6.14  1995/10/10  18:19:42  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.6.13  1995/10/03  18:51:28  xliu
 * Branch: IosSec_branch
 *
 * Moved epa dir under crypto dir. Renamed epa dir to crypto_engine dir.
 * Create crypto_lib under crypto_engine which include Cylink sw crypto
 * libs.
 *
 * Revision 2.1.6.12  1995/09/27  23:20:54  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1.6.11  1995/09/26  04:12:07  carrel
 * Branch: IosSec_branch
 * Initial commit of encryption ACL code.
 *
 * Revision 2.1.6.10  1995/09/22  01:22:01  bew
 * Branch: IosSec_branch
 * Fixes for problems found during ACL code integration with connection
 * setup.
 *
 * Revision 2.1.6.9  1995/09/14  19:32:37  bew
 * Branch: IosSec_branch
 * 1) Implemented Cylink code to find a reflected conn message.
 * 2) Moved handling of a CIM message to be the same as a CRM (send
 * out through icmp code rather than directly to ipwrite().
 * 3) Replaced a malloc() in crypto_conn_entry_initiate_ip() with
 * a private chunk of memory so the routine can be used at
 * interrupt level.
 *
 * Revision 2.1.6.8  1995/09/12  19:46:14  bew
 * Branch: IosSec_branch
 * The network address in the crypto connection message is now used
 * correctly.
 *
 * Revision 2.1.6.7  1995/09/09  01:27:10  bew
 * Branch: IosSec_branch
 * General cleanup & bug fixing in crypto.
 *
 * Revision 2.1.6.6  1995/09/07  22:41:02  bew
 * Branch: IosSec_branch
 * Connection setup now sets up a shared DH number from which DES keys
 * can be derived.
 *
 * Revision 2.1.6.5  1995/09/02  01:48:34  bew
 * Branch: IosSec_branch
 * Inclusion of connection id in the CIB entry, and preparation
 * for calling the DH code.
 *
 * Revision 2.1.6.4  1995/09/01  21:37:33  bew
 * Branch: IosSec_branch
 * Implementation of 11.0 scheduling, implementation of
 * pending connections timeout handling, addition of
 * routine to safely pick crypto connection ids, and
 * other cleanup.
 *
 * Revision 2.1.6.3  1995/08/30  20:41:42  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1.6.2  1995/08/02  19:24:35  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 2.1.6.1  1995/07/01  05:31:24  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 3.1  1995/11/09  11:13:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:21:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.3  1995/06/23  03:00:45  bew
 * Branch: IosSec_branch
 * Check the return from malloc().
 *
 * Revision 1.1.2.2  1995/06/06  21:33:55  bew
 * Branch: IosSec_branch
 * Cleanup of crypto subsystem debugging. Crypto debug is now accessed
 * through the standard parser "debug" command.
 *
 * Revision 1.1.2.1  1995/06/02  00:19:57  bew
 * Branch: IosSec_branch
 * New crypto files -- Session Management files and cleanup of the
 * subsystem.
 *
 * Revision 1.1  1995/06/01 23:16:24  bew
 * Placeholders for crypto/crypto* files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "packet.h"
#include "ttysrv.h"
#include "interface_private.h"
#include "sched.h"
#include "globs.h"
#include "string.h"
#include "../ipc/ipc_util.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "address.h"
#include "../ipc/ipc.h"
#include "crypto_public.h"
#include "crypto.h"
#include "crypto_engine_api.h"
#include "../os/chunk.h"
#include "registry.h"
#include "crypto_registry.h"
#include "crypto_conntable.h"
#include "crypto_map.h"
#include "crypto_sesmgmt.h"
#include "crypto_ip.h"
#include "../ui/debug.h"
#include "crypto_debug.h"
#include "crypto_glue.h"
#include "crypto_pubkey.h"
#include "../os/list.h"
#include "sched.h"
#include "crypto_util.h"
#include "mgd_timers.h"

#define CRYPTO_DH_PRECALC_TIME 10 * ONESEC

watched_queue *crypto_sesmgmtQ;

static mgd_timer conn_time;
static mgd_timer pconn_time;
static mgd_timer precalc_dh_time;

inline void crypto_stop_timers (void)
{
    mgd_timer_stop(&pconn_time);
    mgd_timer_stop(&conn_time);
    mgd_timer_stop(&precalc_dh_time);
}

inline void crypto_start_timers (void)
{
    mgd_timer_start(&conn_time, CRYPTO_EPC_WATCH_TIME);
    mgd_timer_start(&pconn_time, CRYPTO_PCONN_WATCH_TIME);
    mgd_timer_start(&precalc_dh_time, CRYPTO_DH_PRECALC_TIME);
}

ulong crypto_timeout_seconds;

forktype crypto_session_mgmt_proc (void);

void
crypto_sesmgmt_init (void)
{
    pid_t pid;

    /*
     * Create watched queue for Session Manager. The limit is for 
     * reliability in case of someone sending flooding us with crypto messages.
     */
    crypto_sesmgmtQ = create_watched_queue("Crypto SM", 20, 0);

    /*
     * Fork off the Session Manager
     */
    pid = process_create(crypto_session_mgmt_proc, "Crypto SM", LARGE_STACK,
			 PRIO_NORMAL);
    if (pid == NO_PROCESS) {
	/* BEW: Make this a real error message */
	printf("Error! Crypto SM process not started.\n");
    }
}

/*
 * Enqueue function to the Session Manager.
 */
void
crypto_sesmgmt_enqueue(crypto_sesmgmt_block *blk)
{
    process_enqueue(crypto_sesmgmtQ, blk);
}

/*
 * Handle a single connection message.
 */
static void
crypto_sesmgmt_input (crypto_sesmgmt_block *blk)
{
    boolean ok;

    if (crypto_sm_debug) {
        buginf("\nCRYPTO: Dequeued a message: %s",
	       crypto_xlate_message_name(blk->message_type));
    }

    /*
     * Verify the transport-level checksum.
     */
    switch (blk->message_type) {

	case CIM:
	case CRM:
	case CCM:
	    crypto_ip_verify_icmp_checksum(blk->pak, &ok);
	    if (!ok) {
		/*
		 * The pak has already been returned to the pool.
		 */
		free(blk);
		return;
	    }
	    break;

	default:
	    break;
    }

    /* 
     * Process the message. In the case that we received a message we
     * return the pak since we don't need it any more. Note that if a new 
     * message was generated it was put in a new pak, so we really don't
     * need this one any more.
     */
    switch (blk->message_type) {

        case Inititate_Connection:
	    crypto_glue_send_nnc_req(blk);
	    chunk_free(crypto_blk_pool, blk);
	    break;

	case CIM:
	    crypto_glue_recv_nnc_req(blk);
	    datagram_done(blk->pak);
	    free(blk);
	    break;

        case CRM:
	    crypto_glue_recv_nnc_rpy(blk);
	    datagram_done(blk->pak);
	    free(blk);
	    break;

        case CCM:
	    crypto_glue_recv_nnc_cfm(blk);
	    datagram_done(blk->pak);
	    free(blk);
	    break;

        default:
	    if (crypto_sm_debug) {
	        buginf("\nCRYPTO: PID %d Invalid message_type.");
	    }
	    free(blk);
	    break;
    }
}

/*
 * Session Management parent process.
 *
 * This process formats & sends connection messages based on requests received 
 * in the queue. When a full connection sequence is completed it updates the 
 * connection table in the EPA and updated the crypto maps.
 *
 * Currently this process handles each connection message in a serial fashion.
 * It might be more desirable for each message to before forked off into
 * its own process so that we can fill the crypto engine queue a bit faster
 * when there are simultaneous connection attempts. However it has been
 * found that the SDU connection setup code hasn't been engineered for
 * multitasking and reliability goes down drastically when this is tried.
 * If suitable locking is implemented in that code then we could try multiple
 * processes again.
 *
 * Using timers this process also kicks off the Cylink functions to do
 * housekeeping in the pending connection list & the connection table (e.g.,
 * noticing session key timeouts). The pending connection timer is watched by 
 * this process ONLY if there are entries on the list since it is expected 
 * that there won't be pending connections most of the time.
 */
forktype 
crypto_session_mgmt_proc (void)
{
    ulong major, minor;
    static mgd_timer master;     /* static declaration sets struct to 0s */
    mgd_timer *expired_timer;
    enum {PCONN, CONN, PRECALC_DH};
    crypto_sesmgmt_block *blk;

    /* 
     * Some of the following initialization (e.g., protocol vector init)
     * depends on the configuration having been fully read.
     */
    process_wait_on_system_init(); 
    process_wait_on_system_config();

    /* 
     *
     * SDU initialization
     * Moved from crypto_init_common to get more than 10 connections working
     *
     */
    crypto_glue_ep_init();
    crypto_glue_cs_init();

#if 0
    /* Initialize the crypto_engine. */
    crypto_engine_init(crypto_engine_rpsp_slot, CRYPTO_SWEPA);
#endif
    crypto_engine_init_dss_keys();

    crypto_init_alg_vector();
    crypto_timeout_seconds = 20; /* BEW: Wild Guess */

    mgd_timer_init_parent(&master, NULL);
    mgd_timer_init_leaf(&pconn_time, &master, PCONN, NULL, FALSE);
    mgd_timer_init_leaf(&conn_time, &master, CONN, NULL, FALSE);
    mgd_timer_init_leaf(&precalc_dh_time, &master, PRECALC_DH, NULL, FALSE);

    process_watch_queue(crypto_sesmgmtQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&master, ENABLE);
    crypto_start_timers();

    while (TRUE) {

	process_wait_for_event();
	
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case QUEUE_EVENT:
		/*
		 * Dequeue a connection message.
		 */
		blk= (crypto_sesmgmt_block *) process_dequeue(crypto_sesmgmtQ);
		if (!blk) {
		    if (crypto_sm_debug) {
			buginf("\nCRYPTO: crypto_session_mgmt_proc(): "
			       "blk is NULL");
		    }
		    break;
		}
		/*
		 * crypto_sesmgmt_input() will block so we have to turn off 
		 * the timers to avoid nasty watchdog messages.
		 */
		crypto_stop_timers();
		crypto_sesmgmt_input(blk);
		crypto_start_timers();
		break;

	    case TIMER_EVENT:
		while (mgd_timer_expired(&master)) {
		    expired_timer = mgd_timer_first_expired(&master);
		    switch (mgd_timer_type(expired_timer)) {
		    case PCONN:
			(void) crypto_glue_pconn_mgmt();
			mgd_timer_update(expired_timer,
					 CRYPTO_PCONN_WATCH_TIME);
			break;
		    case CONN:
			crypto_glue_epc_task();
			crypto_glue_long_pending_chk();
			mgd_timer_update(expired_timer, CRYPTO_EPC_WATCH_TIME);
			break;

		    case PRECALC_DH:
			/*
			 * crypto_glue_pregen_dh() will block so we
			 * have to turn off the timers to avoid nasty
			 * watchdog messages.
			 */
			crypto_stop_timers();
			crypto_glue_pregen_dh();
			crypto_start_timers();
			break;

		    default:
			mgd_timer_stop(expired_timer);
			buginf("\nCRYPTO: Unexpected managed timer!");
			break;
		    }
		}
		break;

	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
}
