/* $Id: crypto_glue.c,v 3.2.58.13 1996/09/10 23:28:32 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_glue.c,v $
 *------------------------------------------------------------------
 * Cylink Source code: crypto_glue.c
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_glue.c,v $
 * Revision 3.2.58.13  1996/09/10  23:28:32  bew
 * CSCdi64360:  Bad and pending connection does not timeout
 * Branch: California_branch
 * Clear out the pending connection table entry as well
 * as connection table entry when manually clearing a connection.
 *
 * Revision 3.2.58.12  1996/08/30  23:15:20  bew
 * CSCdi67766:  crypto connection message timeout is too long
 * Branch: California_branch
 * Remove long timeout value setting -- use standard default.
 *
 * Revision 3.2.58.11  1996/08/28  19:29:03  bew
 * CSCdi66328:  crypto connection set too early on CRM side
 * Branch: California_branch
 * Use the pending connection id on the CRM side to tag crypto maps. The
 * pending conn id is replaced by connection id when connection
 * established as on initiating side.
 *
 * Revision 3.2.58.10  1996/08/16  05:56:02  bew
 * CSCdi64252:  4500 crashes randomly after a long time during connection
 * setup
 * Branch: California_branch
 * Re-implement manner of finding pending crypto connections using
 * existing SDU structures.
 *
 * Revision 3.2.58.9  1996/08/02  20:11:05  snijsure
 * CSCdi64981:  make crypto snmp modular so images can be built without
 * snmp.
 * Branch: California_branch
 * Invoke snmp cleaup function via registry.
 *
 * Revision 3.2.58.8  1996/07/15  10:05:04  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.2.58.7  1996/07/06  19:44:25  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.2.58.6  1996/07/01  09:05:51  xliu
 * CSCdi61744:  fail to select correct crypto maps when multiple are
 * created
 * Branch: California_branch
 *
 * Revision 3.2.58.5  1996/06/07  00:24:03  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.58.4  1996/06/05  23:11:18  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 3.2.58.3  1996/05/29  21:15:37  bew
 * CSCdi58311:  Could not clear crypto connection
 * Branch: California_branch
 * On clear of a crypto connection, print an error message if target
 * connection id is not found in the connection table.
 *
 * Revision 3.2.58.2  1996/05/12  23:12:15  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/04/19  14:55:41  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.32  1996/04/17  23:47:18  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 2.1.2.31  1996/04/16  21:52:03  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.2.30  1996/04/12  16:06:08  snijsure
 * Branch: IosSec_branch
 * Modified crypto_glue_clear_conn_id() to invoke
 * crypto_snmp_conn_update() which will send a trap
 * on completion of crypto ping, iff its initiated via
 * SNMP.
 *
 * Revision 2.1.2.29  1996/03/30  05:11:20  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 2.1.2.28  1996/03/25  18:43:38  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 2.1.2.27  1996/03/13  22:45:03  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.2.26  1996/03/01  01:38:55  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.2.25  1996/02/13  19:15:49  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.2.24  1996/01/18  23:23:12  bew
 * Branch: IosSec_branch
 * The crypto session manager no longer forks off connection messages but
 * handles them itself serially. This is due to SDU connection setup code
 * not handling the multitasking gracefully.
 *
 * Revision 2.1.2.23  1996/01/16  20:20:50  bew
 * Branch: IosSec_branch
 * Simplified the session mgmt debug msgs & changed the way that duplicate
 * connection messages are handled.
 *
 * Revision 2.1.2.22  1996/01/13  05:46:39  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 2.1.2.21  1995/12/22  00:23:28  bew
 * Branch: IosSec_branch
 * Connection messsages are now handled in their own processes. This is
 * necessary in order to maximize throughput of the H/W EPA.
 *
 * Revision 2.1.2.20  1995/12/20  23:30:43  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 2.1.2.19  1995/12/12  23:09:23  bew
 * Branch: IosSec_branch
 * Fixes for CS & better formatting of debug messages.
 *
 * Revision 2.1.2.18  1995/12/07  21:02:50  bew
 * Branch: IosSec_branch
 * Some fixes for CS to work on 68k machines.
 *
 * Revision 2.1.2.17  1995/11/30  05:57:20  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the new crypto engine code.
 *
 * Revision 2.1.2.16  1995/11/28  01:46:27  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.15  1995/10/17  01:19:34  bew
 * Branch: IosSec_branch
 * Add the simple unauthenticated public key exchange protocal. Cleaned
 * up the printing of pulic keys. Modified some show commands syntax to
 * match the functional spec. General cleanup.
 *
 * Revision 2.1.2.14  1995/10/16  04:04:16  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.2.12  1995/10/10  18:19:32  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.2.11  1995/10/03  18:51:22  xliu
 * Branch: IosSec_branch
 *
 * Moved epa dir under crypto dir. Renamed epa dir to crypto_engine dir.
 * Create crypto_lib under crypto_engine which include Cylink sw crypto
 * libs.
 *
 * Revision 2.1.2.10  1995/09/29  01:00:30  bew
 * Added session key timeouts. Also renamed epa_parser.c to
 * crypto_parser.c & changed the timestamp used in SDU code
 * to be the seconds from Day 0 rather than the contents of
 * msclock.
 *
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.9  1995/09/27  23:20:45  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1.2.8  1995/09/22  01:21:54  bew
 * Branch: IosSec_branch
 * Fixes for problems found during ACL code integration with connection
 * setup.
 *
 * Revision 2.1.2.7  1995/09/14  19:32:34  bew
 * Branch: IosSec_branch
 * 1) Implemented Cylink code to find a reflected conn message.
 * 2) Moved handling of a CIM message to be the same as a CRM (send
 * out through icmp code rather than directly to ipwrite().
 * 3) Replaced a malloc() in crypto_conn_entry_initiate_ip() with
 * a private chunk of memory so the routine can be used at
 * interrupt level.
 *
 * Revision 2.1.2.6  1995/09/12  19:46:08  bew
 * Branch: IosSec_branch
 * The network address in the crypto connection message is now used
 * correctly.
 *
 * Revision 2.1.2.5  1995/09/09  01:27:05  bew
 * Branch: IosSec_branch
 * General cleanup & bug fixing in crypto.
 *
 * Revision 2.1.2.4  1995/09/07  22:41:00  bew
 * Branch: IosSec_branch
 * Connection setup now sets up a shared DH number from which DES keys
 * can be derived.
 *
 * Revision 2.1.2.3  1995/09/02  01:48:31  bew
 * Branch: IosSec_branch
 * Inclusion of connection id in the CIB entry, and preparation
 * for calling the DH code.
 *
 * Revision 2.1.2.1  1995/08/30  20:41:36  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1  1995/08/28  22:59:08  bew
 * Placeholders for Cylink files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Glue between the Cisco IOS and the Cylink SDU code.
 */

#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "ttysrv.h"
#include "interface_private.h"
#include "globs.h"
#include "string.h"
#include "../ip/ip.h"
#include "address.h"
#include "../ip/ip_registry.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "crypto_public.h"
#include "crypto.h"
#include "crypto_engine_api.h"
#include "crypto_map.h"
#include "crypto_sesmgmt.h"
#include "crypto_ip.h"
#include "crypto_glue_private.h"
#include "crypto_glue_sdu.h"
#include "crypto_glue.h"
#include "crypto_pubkey.h"
#include "../ui/debug.h"
#include "crypto_debug.h"
#include "crypto_conntable.h"
#include "../os/clock.h"
#include "crypto_glue_structs.h"
#include "crypto_registry.h"

#define CRYPTO_MAX_RETRANSMISSIONS 3

/*****************************************************************************
 *  Cylink Functions called from Cisco code                                  *
 *****************************************************************************/

CIBptr get_cotptr (Pa2EpFrameControl *fc);

/*
 * Clear a connection id from the policy & EPA.
 * This can be called while interrupts are disabled, so no blocking allowed!
 */
void crypto_glue_clear_conn_id(int old_conn_id, int new_conn_id, ushort slot)
{
    if (crypto_sm_debug) {
	buginf("\nCRYPTO: Replacing %d in crypto maps with %d (slot %d)",
	        old_conn_id, new_conn_id, slot);
    }
  
    /*
     * Replace the old with new in crypto map UNLESS they are both 
     * CRYPTO_CONN_UNSET. There is no point to replacing the value with itself,
     * and furthermore causes nasty things to happen when map->hwidb is NULL,
     * as will be the case if we call it with both values being 
     * CRYPTO_CONN_UNSET.
     */
    if ((old_conn_id!= CRYPTO_CONN_UNSET)||(new_conn_id!= CRYPTO_CONN_UNSET)) {
    	crypto_map_replace_conn_id(old_conn_id, new_conn_id, slot);
    }


    /*
     * Inform the EPA that connection_id is being invalidated.
     */
    if (crypto_conntable_conn_is_valid(old_conn_id)) {
    	crypto_clear_connection_entry(old_conn_id, slot);
    }
    /* 
     * This routine updates status in snmp initiated crypto test
     * connections.
     * If some of the connections have transitioned into "final" state
     * and the user has requested a trap, this routine sends that trap.
     * NOTE: This is a non-blocking routine.
     */
    reg_invoke_crypto_snmp_conn_update();
}

/*
 * Initiate a COT entry. Also find out if a connection has previously been
 * established or if there is an error initiating an entry.
 */
int crypto_glue_initiate_connection (crypto_sesmgmt_block *blk, int method, 
				     char **peer, ushort slot, 
				     boolean *enqueue_it)
{
   CIBptr cib;

   /* 
    * Default is to tell the caller to enqueue a pending connection to the
    * Session Manager.
    */
   *enqueue_it = TRUE;

    /*
     * Look for a CIB for this PE/UPE pair.
     */
    cib = crypto_glue_get_cot(blk, TRUE);
    
    if (cib == 0) {
	/*
	 * If we got a NULL pointer back, then we may have the case where a 
	 * connection is pending. If so, we'd like to find the cib anyway and 
	 * return the pending identifier to the caller. To find the cib we try 
	 * again but pretending to be in connection msg mode.
	 */
        cib = crypto_glue_get_cot(blk, FALSE);
        if (cib) {
	    /*
	     * We may have found a pre-existing connection which was pending.
	     * If there is not a pending value in that field then we are in 
	     * an unknown state and we need to return an error.
	     */
	    if (crypto_conntable_conn_is_pending(cib->crypto.cisco.connection_id)) {
		*enqueue_it = FALSE;
		/*
		 * Do the normal return.
		 */
	    } 
	} else {
	    /*
	     * Oops. There must really not be a CIB.
	     */
	    *enqueue_it = FALSE;
	    return(CRYPTO_CONN_BAD);
	}
    }

    /* 
     * Verify that the connection id is for this slot. If it isn't
     * we need to tear down this connection and start up a new one.
     */
    if ((cib->crypto.cisco.connection_id != CRYPTO_CONN_UNSET) &&
        (cib->crypto.cisco.slot != slot)) {
	crypto_glue_clear_conn_id(cib->crypto.cisco.connection_id,
				  CRYPTO_CONN_UNSET,
				  cib->crypto.cisco.slot);
        cib->crypto.cisco.connection_id = CRYPTO_CONN_UNSET;
    }

    /*
     * The connection id will be unset if we just created the CIB so we need
     * to create a pending indicator & create some context for a new connection.
     */
    if (cib->crypto.cisco.connection_id == CRYPTO_CONN_UNSET) {
	cib->crypto.cisco.connection_id = crypto_conn_find_uniq_pending_value();
    	cib->crypto.cisco.new_connection_id = CRYPTO_CONN_UNSET;
    	cib->crypto.cisco.slot = slot;
  	cib->crypto.cisco.methods_allowed = 
				(int) (method & crypto_get_alg_vector());
	/*
	 * Use the crypto map pointer. We must always verify that the pointer
	 * is still valid before we use it!
	 */
	cib->crypto.cisco.peers_allowed = peer;
	cib->crypto.cisco.mode_flags = blk->mode_flags;
    } 

    return (cib->crypto.cisco.connection_id);
}

/*
 * Prepare an Echo Request (CIM).
 */
void
crypto_glue_send_nnc_req (crypto_sesmgmt_block *blk)
{
    Pa2EpFrameControl fc;

    fc.descriptorLink = NULL;
    /* 
     * We indicate the PE initiated the connection regardless of which side 
     * really did. We don't really care -- the Cylink PA code does need to
     * know it, but we don't use that code.
     */
    fc.receivedPath   = PE_SIDE; 
    fc.epEntry.pe.pid  = fc.epEntry.upe.pid = INTERNET_IP;
    fc.epEntry.pe.hash = fc.epEntry.upe.hash= 0;
    fc.epEntry.pe.adr  = blk->pe.ip_addr;
    fc.epEntry.upe.adr = blk->upe.ip_addr;
    /*
     * Store the original src and dst as given to us by the policy layer. 
     * They are used to address the ICMP connection message .
     */
    fc.epEntry.cisco.src = blk->src.ip_addr;
    fc.epEntry.cisco.dst = blk->dst.ip_addr;
    fc.epEntry.cisco.ptr = blk;

    /* 
     * Find the COT entry.
     * The only possible return by get_cot() as called here is a pointer
     */
    fc.epEntry.info.cs.cib = (CIBptr) crypto_glue_get_cot(blk, FALSE);
	
    /*
     * Initialize the cib to a known state. In particular, crypto.time must
     * be zero or the crypto_glue_long_pending_chk() might time it out in
     * error.
     */
    fc.epEntry.info.cs.cib->crypto.method = CRYPTO_UNDEFINED_METHOD;
    fc.epEntry.info.cs.cib->crypto.time = 0;

    send_nnc_req(&fc);
}

/*
 * Process a received NNC Echo (CIM) & prepare an NNC Reply (CRM).
 */
void
crypto_glue_recv_nnc_req (crypto_sesmgmt_block *blk)
{
    Pa2EpFrameControl fc;

    fc.descriptorLink = NULL;
    fc.receivedPath   = UPE_SIDE;
    fc.frameLink       = blk->pak->datagramstart;
    fc.frameLength     = blk->pak->datagramsize;
    fc.epEntry.pe.pid  = fc.epEntry.upe.pid = INTERNET_IP;
    fc.epEntry.pe.hash = fc.epEntry.upe.hash= 0;
    fc.epEntry.pe.adr  = blk->pe.ip_addr;
    fc.epEntry.upe.adr = blk->upe.ip_addr;
    fc.epEntry.cisco.ptr = blk;
    crypto_ip_find_echo(blk->pak, &(fc.transportLink));

    /* 
     * Find the COT entry.
     * The only possible return by get_cot() as called here is a pointer,
     * but because I'm superstitious we check it.
     */
    fc.epEntry.info.cs.cib = (CIBptr) crypto_glue_get_cot(blk, TRUE);
    if (fc.epEntry.info.cs.cib == 0 ) {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: Null cib found (crypto_glue_recv_nnc_req)");
	}
	return;
    }

    /*
     * Adjust the status bits. Then check if there is a socket strucutre
     * attached. If not, go thorugh crypto_glue_get_cot() again to get one
     * attached. It will as the result of the status bits just adjusted.
     */
    fc.epEntry.info.cs.cib->status |= XCHG_KEYS;
    fc.epEntry.info.cs.cib->status &= ~TIME_KEYS;
    fc.epEntry.info.cs.cib->status &= ~PEND_CONN;
    
   if (!fc.epEntry.info.cs.cib->socket) {
 	fc.epEntry.info.cs.cib = (CIBptr) crypto_glue_get_cot(blk, TRUE);
	if (fc.epEntry.info.cs.cib == 0 ) {
 	    if (crypto_sm_debug) {
     		buginf("\nCRYPTO: Null cib found (crypto_glue_recv_nnc_req)");
 	    }
 	    return;
     	}
      }

    recv_nnc(&fc);
}
/*
 * Process a received NNC Echo Reply (CRM) & prepare an NNC Echo Confirm (CCM)
 */
void
crypto_glue_recv_nnc_rpy (crypto_sesmgmt_block *blk)
{
    Pa2EpFrameControl fc;
    ushort slot;

    fc.descriptorLink  = NULL;
    fc.receivedPath    = UPE_SIDE;
    fc.frameLink       = blk->pak->datagramstart;
    fc.frameLength     = blk->pak->datagramsize;
    fc.epEntry.pe.pid  = fc.epEntry.upe.pid = INTERNET_IP;
    fc.epEntry.pe.hash = fc.epEntry.upe.hash= 0;
    fc.epEntry.pe.adr  = blk->pe.ip_addr;
    fc.epEntry.upe.adr = blk->upe.ip_addr;
    crypto_ip_find_echo(blk->pak, &(fc.transportLink));

    /* 
     * Find the COT entry.
     * The only possible return by get_cot() as called here is a pointer
     */
    fc.epEntry.info.cs.cib = (CIBptr) crypto_glue_get_cot(blk, FALSE);

    /*
     * Verify slot.
     */
    slot = crypto_get_engine_id(blk->slot);
    if (fc.epEntry.info.cs.cib->crypto.cisco.slot != slot) {
	if (crypto_sm_debug) {
            buginf("\nCRYPTO: Warning: CRM associated with the wrong slot!");
            buginf("\n        Expected: %d, Actual: %d, Normalized:%d\n",
		   fc.epEntry.info.cs.cib->crypto.cisco.slot, blk->slot, slot);
	}
    }

    recv_nnc(&fc);
}

/*
 * Process a received NNC Echo Confirm (CCM)
 */
void
crypto_glue_recv_nnc_cfm(crypto_sesmgmt_block *blk)
{
    Pa2EpFrameControl fc;

    fc.descriptorLink = NULL;
    fc.receivedPath   = UPE_SIDE;
    fc.frameLink       = blk->pak->datagramstart;
    fc.frameLength     = blk->pak->datagramsize;
    fc.epEntry.pe.pid  = fc.epEntry.upe.pid = INTERNET_IP;
    fc.epEntry.pe.hash = fc.epEntry.upe.hash= 0;
    fc.epEntry.pe.adr  = blk->pe.ip_addr;
    fc.epEntry.upe.adr = blk->upe.ip_addr;
    crypto_ip_find_echo(blk->pak, &(fc.transportLink));

    /* 
     * Find the COT entry.
     * The only possible return by get_cot() as called here is a pointer
     */
    fc.epEntry.info.cs.cib = (CIBptr) crypto_glue_get_cot(blk, FALSE);

    /*
     * Verify slot.
     */
    if (fc.epEntry.info.cs.cib->crypto.cisco.slot != blk->slot) {
       if (crypto_sm_debug) {
            buginf("\nCRYPTO: CRM associated with the wrong slot!");
       }
       return;
    }

    recv_nnc(&fc);

}

void *crypto_glue_get_cot (crypto_sesmgmt_block *blk, boolean init_conn)
{
    Pa2EpFrameControl fc;
    CIBptr cib_ptr;

    fc.descriptorLink = NULL;
    fc.receivedPath   = PE_SIDE;
    fc.epEntry.pe.pid  = fc.epEntry.upe.pid = INTERNET_IP;
    fc.epEntry.pe.hash = fc.epEntry.upe.hash= 0;
    fc.epEntry.pe.adr  = blk->pe.ip_addr;
    fc.epEntry.upe.adr = blk->upe.ip_addr;

    /*
     * If this is called because a packet was found requiring a connection,
     * mark it as a ConnectInit call.
     */
    if (init_conn) {
    	fc.frameMode = ConnectInit;
    } else {
    	fc.frameMode = NncFrame;
    }

    cib_ptr = ep_get_cotptr(&fc);

    return((void *)cib_ptr);
}

void
crypto_glue_cs_init(void)
{
    /* 
     * In looking ahead to when we support both the "unnamaged" mode and
     * the Secure Manager SDU mode, we set:
     * a. the availability of the various maps to be correct, that is
     *     the protocol vector (PV). Probably this should never equal
     *     CS_CONFIG_TB_AVAIL in unmanaged mode.
     */
    csAvail.bitMap = CS_PV_TB_AVAIL;

    /*
     * Set the number of retransmissions higher than the SDU code. This allows 
     * both sides to timeout due to a missing ARP cache entry, plus one more 
     * to succeed.
     */
    cs_max_retransmissions = CRYPTO_MAX_RETRANSMISSIONS; 

    switch (cs_init()) {
	case CS_NO_ERROR:
	   /* 
	    * Set up the Cisco certificate & protocol vector
	    */
	    crypto_glue_initialize_cisco_nnc();
    	    crypto_glue_init_protocol_vector();
	   break;
	case INIT_CHLG_FAILED:
	    sduState = SDU_OFFLINE;
	    buginf("ERROR: cs_init() failed.\n");
	    break;
    }

    /*
     * Initialize the syndrome.
     */
    (void) memset(syndrome, '\0', CRYPTO_CHALLENGE_SIZE);
}

/*
 * What do do with errors? 
 */
void
crypto_glue_ep_init(void)
{
    switch (ep_init()) {
	case EP_OKAY:
	    /*
	     * Hardwire an open SDU for this release & declare the SDU to be
	     * now operational.
	     */
    	    ep_ctrl = OPEN_SDU | PASS_INTRANET;
	    sduState = SDU_OPERATION;
	    break;
	default:
	    sduState = SDU_OFFLINE;
	    buginf("ERROR: ep_init() failed.\n");
	    break;
    }
}

boolean crypto_glue_pconn_mgmt (void)
{
    pconn_mgmt();
    if (pconn_used == NULL_INDEX)
	return(TRUE);
    else
	return(FALSE);
}

void crypto_glue_epc_task (void)
{
    epc_task();
}

/*
 * Initialize the set of IP protocols (i.e., IP, TCP/IP, UDP/IP) which are
 * the first 3 bits of the protocol vector (0xe). The protocal action table is
 * a 2 bit value split into two tables, the first bit (most significant) in
 * sdu_pact1_tb and the second in sdu_pact2_tb. The pairing "01" means CRYPT,
 * so for the IP protocols the first bit is zero (0x0) , and the second is
 * 1 (0xe).
 */
void crypto_glue_init_protocol_vector (void)
{
    sdu_pv_tb[0]    = 0xe0000000L;
    sdu_pact1_tb[0] = 0x00000000L;
    sdu_pact2_tb[0] = 0xe0000000L;
    return;
}

void crypto_glue_set_session_key_timeout (ushort timeout)
{
    epMaxKeyAge = timeout;
    return;
}

ushort crypto_glue_get_session_key_timeout (void)
{
    return(epMaxKeyAge);
}

/*
 * Exchange connection id values.  This function MUST keep the following in 
 * sync:
 * 1) COT table connection ids, 2) ACL entries connection id. 
 * Note that the EPA tables do not require update -- all that is important
 * there is that a conn id not be used until all the needed info in the 
 * connection has been setup. This is orthogonal to whether or not a conn id
 * has been installed in the policy. (We do the best we can to not set the 
 * policy to a not-ready connection id, but it is not always possible due to the
 * interactions between SDU and IOS code.)
 *
 * This function is called in the following situations:
 * a) A pending (negative) value being replaced by a connection id (positive) 
 *    value as the result of a session being formed.
 * b) A positive value being replaced by CRYPTO_CONN_UNSET indicating that
 *    this side has timed out the session and is forcing a rekey.
 */

void crypto_glue_xchg_connection_ids (CIBptr cib, 
				      boolean install_new_conn_id_first,
				      long new_conn_id)
{
    leveltype level;

    /*
     * We must keep all of the policy code, EPA tables, and SM tables 
     * consistent vis a via connection ids. The only way to ensure that
     * they all get cleaned out before anything else happens is to disable
     * interrupts. 
     */
    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Sometimes (e.g., when clearing a connection) we want to install the
     * a new connection id immediately before exchanging. To be safe we do it
     * here, while interrupts have been disabled.
     */
    if (install_new_conn_id_first) {
	cib->crypto.cisco.new_connection_id = new_conn_id;
    }

    crypto_glue_clear_conn_id(cib->crypto.cisco.connection_id,
			      cib->crypto.cisco.new_connection_id,
			      cib->crypto.cisco.slot);

    if (cib->crypto.cisco.new_connection_id == CRYPTO_CONN_UNSET) {
	/*
	 * Remove TIME_KEYS from status so that we won't try to time it out
	 * again before we exchange keys again. Without taking TIME_KEYS away,
	 * we would repeatedly try to walk the ACLs to exchange values, and
	 * worse yet ... due to the long delay in creating the connection
	 * messages, rekeys would happen twice per connection because they would
	 * get timed out during key negotiation. (The times would get 
	 * interrogated again before it was replaced by the rekey operation.)
	 */
	(cib->status) &= (~TIME_KEYS);
    }

    /*
     * Now that the policy code has been informed, reset the new connection id 
     * to be "unset" since it is now the current connection id.
     */
    cib->crypto.cisco.connection_id = cib->crypto.cisco.new_connection_id;
    cib->crypto.cisco.new_connection_id = CRYPTO_CONN_UNSET;
    
    reset_interrupt_level(level);

    return;
}

/*
 * Clear a connection id from both the current and new connection fields
 * within a CIB.
 */
void crypto_glue_clear_connection (int connection, ushort slot)
{
    ushort i,j,p;
    boolean found = FALSE;

    /*
     * Run the PET/COT tables to match the connection id.
     */
    for (i=pet_nodes-1; i; i--) {
	if ((p = pet[i].page) != 0) {
 	    for (j=cot_nodes-1; j; j--) {
		if ((cot[p].node[j].cib.crypto.cisco.connection_id 
		    	== connection) && (slot ==
			cot[p].node[j].cib.crypto.cisco.slot)) {
		    /*
		     * Set the new connection id to be "unset" before
		     * nuking the connection. This is actually done inside
		     * crypto_glue_xchg_connection_ids() so as to do it
		     * when locked.
		     */
		    crypto_glue_xchg_connection_ids(&cot[p].node[j].cib, TRUE,
						    CRYPTO_CONN_UNSET);
		    crypto_glue_cleanup_cib(&cot[p].node[j].cib);
		    crypto_glue_clear_pending_index(&cot[p].node[j].cib);
		    found = TRUE;
		}
		if ((cot[p].node[j].cib.crypto.cisco.new_connection_id 
		    	== connection) && (slot ==
			cot[p].node[j].cib.crypto.cisco.slot)) {
		    /*
		     * Just set the new connection id to be "unset".
		     * There is not need to tell the policy layer since
		     * they didn't know about it anyway.
		     */
		    cot[p].node[j].cib.crypto.cisco.new_connection_id =
			CRYPTO_CONN_UNSET;
		    /*
		     * Tell the EPA too.
		     */
    		    if (crypto_conntable_conn_is_valid(connection)) {
    	    		crypto_clear_connection_entry(connection, slot);
        	    }
		    found = TRUE;
		}
	    }
	}
    }
    if (!found) {
	printf("%% Connection %d not found in connection table", connection);
	if (NO_EXTENDED_SLOTS_ON_CHASSIS) {
	    printf(".\n");
	} else {
	    printf(" for slot %d.\n", slot);
	}
	/*
	 * If the conn id was found in the COT tables then we've already 
	 * cleared out the policy and crypto engine tables. In the case of
	 * an unexpected situation though  this code lets us be sure that 
	 * everyone has the connection id cleared out.
	 */
    	crypto_map_replace_conn_id(connection, CRYPTO_CONN_UNSET, slot);
    	/*
     	 * Inform the EPA that connection_id is being invalidated.
     	 */
    	if (crypto_conntable_conn_is_valid(connection)) {
    	    crypto_clear_connection_entry(connection, slot);
        }
    }
}

/*
 * Create the cisco NNC.
 * For now we use the nnc_sys_oc NNC
 */
void crypto_glue_initialize_cisco_nnc (void)
{
    uchar *nnc_ptr;

    /*
     * Per Cylink, Cisco is assigned manufacturer id of '001', and we use
     * product id '0' for SecureDomain.
     *
     * Per Chuck Williams' memo dated 950621:
     *
     * Manufacture ID Code Points
     *  Value		Manufacturer
     *	000		Cylink
     *	001		Cisco
     *	002		reserved
     *  ...		...
     *	FFE		reserved
     *	FFF		unknwn
     *
     * Product type ID Code Points
     *	Value		Manufacturer
     *	0		SecureDomain
     *	1		SecureNode
     *	2		SecureGate
     *	3		reserved
     *	4		reserved
     *	5		reserved
     *	6		SecureCell
     *	7		SecureFrame
     *	8		manufacturer-specific
     *	...		...
     *	F		manufacturer-specific
     */

    /*
     * Use the system "open/close" flag which is used if there are no
     * NNCs downloaded from the Security Manager.
     */
    nnc_ptr= nnc_sys_oc; 

    *nnc_ptr=0x00;	/* product type: 2 bytes */
    nnc_ptr++;
    *nnc_ptr=0x10;
    nnc_ptr++;

    nnc_ptr += 4;	/* customer id: 4 bytes of zeros */

    *nnc_ptr=0x00;	/* software version: 2 bytes */
    nnc_ptr++;
    *nnc_ptr=0x01;
    nnc_ptr++;
    
    nnc_ptr += 6;	/* node id: 6 bytes filled in during CS */

    *nnc_ptr=0x00;	/* revision number: 2 bytes */
    nnc_ptr++;
    *nnc_ptr=0x01;
    nnc_ptr++;

    nnc_ptr += 4;	/* Method vector: 4 bytes filled in during CS */

    nnc_ptr += 64;	/* SU DSS public number: 64 bytes of zeros */

    *nnc_ptr=0x00;	/* number of groups (1 group): 2 bytes */
    nnc_ptr++;
    *nnc_ptr=0x01;
    nnc_ptr++;

    nnc_ptr += 2;	/* openness/closeness: 2 bytes of zeros (open) */

    *nnc_ptr=0x80;	/* group membership: 1 group */
    nnc_ptr += 4;

    *nnc_ptr=0x80;	/* root/leaf role: 1 group */
    nnc_ptr += 4;

    *nnc_ptr=0x80;	/* meshiness: 1 group */
    nnc_ptr += 4;

			/* SM Group DSS signature: 40 zeros */
}

/*
 * Find the end of a connection message. This is used by several other
 * glue functions.
 */
uchar *
crypto_glue_find_msg_end_addr (uchar *pdu)
{
    uchar frm_type;
    ushort recv_nnc_size;
    uchar   *nnc = NULL;
    uchar *computed_end;

    frm_type = *(pdu + PDU_OP_OFFSET);

    switch (frm_type) {

        case ECHO_REQUEST:
	case ECHO_REPLY:
	    /* nnc = pdu + PDU_FIX_NNC_OFFSET + dhPublicLen; */
	    nnc = pdu + PDU_FIX_NNC_OFFSET + 64; 
	    recv_nnc_size = get_nnc_size(nnc);
	    computed_end = nnc + (recv_nnc_size) + DEF_SIG_SIZE;
	    break;

	case ECHO_CONFIRM:
	    computed_end = pdu + PDU_CFM_SIG_OFFSET + DEF_SIG_SIZE;
	    break;

	default:
	    if (crypto_sm_debug) {
		buginf("\nCRYPTO: crypto_glue_find_msg_end_addr(): "
			"Invalid msg type=%d", frm_type);
	    }
	    computed_end = 0;
	    break;
    }
    return(computed_end);
}

/*
 * Calculate the length of a connection message.
 */
int
crypto_glue_get_msg_length (uchar *pdu, boolean include_sig_in_length)
{
    uchar *end = crypto_glue_find_msg_end_addr(pdu);

    if ((end != 0) && (!include_sig_in_length)) {
	end -= DEF_SIG_SIZE;
    }

    return (end - pdu);
}

/*
 * Find the address of the connection message signature (not NNC signature).
 */
uchar *
crypto_glue_find_sig_addr (uchar *pdu)
{
    uchar *end = crypto_glue_find_msg_end_addr(pdu);

    if (end != 0) {
	end -= DEF_SIG_SIZE;
    }

    return (end);
}

/*
 * Return the type of a message
 */
int
crypto_glue_find_message_type (uchar *pdu, crypto_sesmgmt_message_types *type)
{
    uchar frm_type = *(pdu + PDU_OP_OFFSET);
   
    switch (frm_type) {
        case ECHO_REQUEST:
	    *type=CIM;
	    break;
	case ECHO_REPLY:
	    *type=CRM;
	    break;
	case ECHO_CONFIRM:
	    *type=CCM;
	    break;
	default:
	    *type=Unsuccessful_Message;
	    return(-1);
    }
    return(0);
}

void
crypto_glue_insert_network_address (uchar *pdu, uchar *address, int size)
{
    memcpy((pdu+PDU_NW_ADDR_OFFSET), address, size);
    return;
}

#define CRYPTO_PENDING_TOO_LONG (((max_pconn_period*cs_max_retransmissions/60)+1) * 2)
/*
 * Check that no pending connection is pending for too long. If so, clear
 * it out -- something went wrong. Modeled after chk_key_age().
 */
void
crypto_glue_long_pending_chk (void)
{
    ushort    i,j,p;                        /* processing indeces */
    ushort    t,time;                        /* time computation variables */
    ushort  *s;				/* temp status ptr for faster code */
    long ccc_time;

    (void) GetRTClock(&ccc_time);
    time = (ushort) (ccc_time / 60L);     /* convert to mins % 2^16 */
    
    for (i=pet_nodes-1; i; i--) {
        if ((p = pet[i].page) != 0) {
            for (j=cot_nodes-1; j; j--) {
                if (crypto_conntable_conn_is_pending(
			cot[p].node[j].cib.crypto.cisco.connection_id) &&
		    	(cot[p].node[j].cib.crypto.time > 0)) 
		{
                    t = cot[p].node[j].cib.crypto.time;
		    /* get #minutes keys used */
                    if (t > time)                 
			/* time has wrapped, normalize */
                        t = ((ushort)0xffff - t) + time;  
                    else
                        t = time - t;
		    /*see if time has expired */
                    if (t > (int) CRYPTO_PENDING_TOO_LONG)
                    {
			s = &cot[p].node[j].cib.status;
                        *s |= XCHG_KEYS;
			if (crypto_sm_debug) {
			buginf("\nCRYPTO: Pending conn id %d pending too long.",
				cot[p].node[j].cib.crypto.cisco.connection_id);
			}
                        crypto_glue_timeout_keys(&cot[p].node[j].cib);
                    }
                }
	    } 
	}
    }
}

/*
 * Find the connection id in a CRM or CCM, and update the given structure.
 */
boolean crypto_glue_find_pending_conn (uchar *conn_msg, 
				       crypto_sesmgmt_block *blk)
{
    short pconn_index;
    ulong msg_conn_id;
    ushort saved_offset;

    /*
     * Get the pending connection table index based on message type.
     */
    switch (blk->message_type) {
    case CRM:
	pconn_index =  GETSHORT(conn_msg + PDU_INIT_PCONN_OFFSET);
	msg_conn_id =  GETLONG(conn_msg  + PDU_INIT_CONNID_OFFSET);
	break;
    case CCM:
	pconn_index = GETSHORT(conn_msg  + PDU_CFM_RPC_OFFSET);
	msg_conn_id =  GETLONG(conn_msg  + PDU_CFM_RP_CID_OFFSET);
	break;
    default:
	 return(FALSE);
    }
    if (pconn_index > PCONN_ENTRIES) {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: Corrupted index found for connection message: %d",
		    pconn_index);
	}
	return(FALSE);
    }

    /*
     * Verify that the index is valid -- that the connection is still pending.
     */
    switch (blk->message_type) {
    case CRM:
	saved_offset = PDU_INIT_CONNID_OFFSET;
	break;
    case CCM:
	saved_offset = PDU_RESP_CONNID_OFFSET;
	break;
    default:
	return(FALSE);
    }
    if (!pconn_exist(pconn_index, msg_conn_id, saved_offset)) {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: Invalid index found for connection message: %d",
		   pconn_index);
	}
	return(FALSE);
    }

    /*
     * Fill in the blk with connection info needed by the session mgr.
     */
    blk->pe.ip_addr = pconn_list[pconn_index].pe.adr;
    blk->upe.ip_addr = pconn_list[pconn_index].upe.adr;
    blk->slot = pconn_list[pconn_index].cib->crypto.cisco.slot;
    return(TRUE);
}

ushort crypto_glue_get_sdu_connections(void)
{
    return(crypto_sdu_connections);
}

ushort crypto_glue_get_sdu_entities(void)
{
    return(crypto_sdu_entities);
}
