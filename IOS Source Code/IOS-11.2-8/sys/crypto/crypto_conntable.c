/* $Id: crypto_conntable.c,v 3.2.58.6 1996/08/02 20:11:02 snijsure Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_conntable.c,v $
 *------------------------------------------------------------------
 * crypto_conntable.c - Connection table update routines
 *
 * August, 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_conntable.c,v $
 * Revision 3.2.58.6  1996/08/02  20:11:02  snijsure
 * CSCdi64981:  make crypto snmp modular so images can be built without
 * snmp.
 * Branch: California_branch
 * Invoke snmp cleaup function via registry.
 *
 * Revision 3.2.58.5  1996/07/15  10:04:56  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.2.58.4  1996/07/06  19:44:22  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/07/01  09:05:49  xliu
 * CSCdi61744:  fail to select correct crypto maps when multiple are
 * created
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/05/12  23:12:11  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/04/19  14:55:32  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.22  1996/04/16  21:51:52  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.2.21  1996/04/12  23:07:23  carrel
 * Branch: IosSec_branch
 * Fix wildcarding in ACL for crypto map.  Also some cleanup.
 *
 * Revision 2.1.2.20  1996/03/13  22:44:58  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.2.19  1996/03/01  01:38:50  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.2.18  1996/01/18  23:23:10  bew
 * Branch: IosSec_branch
 * The crypto session manager no longer forks off connection messages but
 * handles them itself serially. This is due to SDU connection setup code
 * not handling the multitasking gracefully.
 *
 * Revision 2.1.2.17  1996/01/04  18:47:46  xliu
 * Branch: IosSec_branch
 * Crypto moduliztion.
 *
 * Revision 2.1.2.16  1995/12/12  23:09:20  bew
 * Branch: IosSec_branch
 * Fixes for CS & better formatting of debug messages.
 *
 * Revision 2.1.2.15  1995/12/10  09:33:12  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 2.1.2.14  1995/11/30  05:57:19  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the new crypto engine code.
 *
 * Revision 2.1.2.13  1995/11/17  16:33:14  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.2.12  1995/10/16  04:04:14  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.2.10  1995/10/10  18:19:30  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.2.9  1995/10/03  18:51:21  xliu
 * Branch: IosSec_branch
 *
 * Moved epa dir under crypto dir. Renamed epa dir to crypto_engine dir.
 * Create crypto_lib under crypto_engine which include Cylink sw crypto
 * libs.
 *
 * Revision 2.1.2.8  1995/09/27  23:20:42  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1.2.7  1995/09/26  04:12:01  carrel
 * Branch: IosSec_branch
 * Initial commit of encryption ACL code.
 *
 * Revision 2.1.2.6  1995/09/22  01:21:51  bew
 * Branch: IosSec_branch
 * Fixes for problems found during ACL code integration with connection
 * setup.
 *
 * Revision 2.1.2.5  1995/09/14  19:32:33  bew
 * Branch: IosSec_branch
 * 1) Implemented Cylink code to find a reflected conn message.
 * 2) Moved handling of a CIM message to be the same as a CRM (send
 * out through icmp code rather than directly to ipwrite().
 * 3) Replaced a malloc() in crypto_conn_entry_initiate_ip() with
 * a private chunk of memory so the routine can be used at
 * interrupt level.
 *
 * Revision 2.1.2.4  1995/09/06  01:17:04  bew
 * Branch: IosSec_branch
 * epa_init() is now called whenever new DSS keys are created or read in
 * from the configuration. Also added a crypto config command to initialize
 * the EPA to replace the exec epa command. The epa command was removed
 * altogether.
 *
 * Revision 2.1.2.3  1995/09/01  21:37:29  bew
 * Branch: IosSec_branch
 * Implementation of 11.0 scheduling, implementation of
 * pending connections timeout handling, addition of
 * routine to safely pick crypto connection ids, and
 * other cleanup.
 *
 * Revision 2.1.2.2  1995/08/30  20:41:34  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1.2.1  1995/08/02  19:24:29  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 2.1  1995/08/02  19:12:17  bew
 * Placeholder files for crypto.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "config.h"
#include "packet.h"
#include "globs.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "crypto_public.h"
#include "crypto.h"
#include "crypto_engine_api.h"
#include "sched.h"
#include "crypto_map.h"
#include "crypto_sesmgmt.h"
#include "crypto_ip.h"
#include "../os/chunk.h"
#include "crypto_conntable.h"
#include "crypto_glue.h"
#include "crypto_registry.h"

/*
 * This value starts at -1 and is decremented for each pending connection to 
 * provide a unique value for a pending connection. It is returned to the ACL 
 * code so we can find this particular ACL entry again.
 */
short crypto_conn_pending_value = -1;

chunk_type *crypto_blk_pool;

void
crypto_conn_init (void)
{
    /*
     * Allocate a chunck of blk's. This allows us to allocate memory from a
     * private pool at interrupt level.
     */
    crypto_blk_pool = chunk_create(sizeof(crypto_sesmgmt_block), 
				   CRYPTO_CONN_CHUNK_MAX, CHUNK_FLAGS_NONE,
				   NULL, 0, "Crypto Conn Blk Chunk");
    return;
}

void 
crypto_conn_entry_initiate_ip (int *conn_id, ipaddrtype pe, ipaddrtype upe, 
			       ipaddrtype phost, ipaddrtype uphost,
                               ushort slot, int method, char **peer,
			       uint mode_flags)
{
    crypto_sesmgmt_block *blk;
    int status;
    boolean enqueue_it;
    
    if ((blk = chunk_malloc(crypto_blk_pool)) == NULL) {
	*conn_id = CRYPTO_CONN_ERROR;
        return;
    }

    /*
     * Setup blk to pass this request on to Session Manager
     */
    blk->message_type = Inititate_Connection;
    blk->pak = NULL;

    /*
     * Policy code gave us PE and UPE 
     * We'll be initiating a message so set src=phost, dst=uphost
     */
    blk->pe.ip_addr=pe;
    blk->upe.ip_addr=upe;
    blk->src.ip_addr=phost;
    blk->dst.ip_addr=uphost;
    blk->pe.type = blk->upe.type = ADDR_IP;
    blk->src.type = blk->dst.type = ADDR_IP;
    blk->slot = slot;
    blk->peer = peer;
    blk->mode_flags = mode_flags;

    status = crypto_glue_initiate_connection(blk, method, peer, slot,
					     &enqueue_it);

    if (crypto_conntable_conn_is_pending(status)) {
	/*
	 * The connection is pending, and a unique pending value has been 
	 * assigned which needs to be passed on to the ACL code.
	 *
	 * Queue the request for the SM before returning, but only if
	 * this is a newly initiated connection.
	 */
	if (enqueue_it) {
	    crypto_sesmgmt_enqueue(blk);
	}
    } else {
	/*
	 * There was either an error or an existing connection.
	 * Free the block before returning.
	 */
	chunk_free(crypto_blk_pool, blk);
    }

    *conn_id = status;
    return;
}


/*
 * Return a unique value for storing in an ACL to uniquely identify this 
 * connection. We assume that it is unique because the range of values is so 
 * large (100000), and a  pending connection lasts such as relatively short 
 * amount of time (1.5 minutes max) that the value won't possibly be reused. 
 * That's the theory, anyway.
 */
long crypto_conn_find_uniq_pending_value (void)
{
    if (crypto_conn_pending_value < CRYPTO_CONN_MINIMUM) {
	crypto_conn_pending_value = -1;
    }
    return (crypto_conn_pending_value--);
}
