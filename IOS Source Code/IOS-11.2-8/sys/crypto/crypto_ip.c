/* $Id: crypto_ip.c,v 3.2.58.14 1996/09/11 23:25:07 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_ip.c,v $
 *------------------------------------------------------------------
 * crypto_ip.c - IP-level routines calling the EPA API
 *
 * April, 1995 Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_ip.c,v $
 * Revision 3.2.58.14  1996/09/11  23:25:07  bew
 * CSCdi68438:  crypto determination of network address should be done
 * earlier
 * Branch: California_branch
 * Run the routing tables during connection message setup to determine
 * which interface the packet will go out.
 *
 * Revision 3.2.58.13  1996/08/30  01:27:34  bew
 * CSCdi67651:  crypto connection doesnt setup if first CRM is lost
 * Branch: California_branch
 * Don't reset the pending connection id if it is already pending.
 *
 * Revision 3.2.58.12  1996/08/28  19:29:08  bew
 * CSCdi66328:  crypto connection set too early on CRM side
 * Branch: California_branch
 * Use the pending connection id on the CRM side to tag crypto maps. The
 * pending conn id is replaced by connection id when connection
 * established as on initiating side.
 *
 * Revision 3.2.58.11  1996/08/16  05:56:06  bew
 * CSCdi64252:  4500 crashes randomly after a long time during connection
 * setup
 * Branch: California_branch
 * Re-implement manner of finding pending crypto connections using
 * existing SDU structures.
 *
 * Revision 3.2.58.10  1996/08/16  01:58:56  bew
 * CSCdi65450:  First crypto connection msg gets lost and has to be resent
 * Branch: California_branch
 * Initialize ttl when sending the first connection message.
 *
 * Revision 3.2.58.9  1996/08/07  21:58:40  bew
 * CSCdi65215:  crypto connection msgs not handled properly if router is
 *              destination
 * Branch: California_branch
 * If the connection message is a CIM, check each crypto map statement
 * until one matches. If a CRM or CCM which is for us, queue it up for
 * session mgr without checking the policy.
 *
 * Revision 3.2.58.8  1996/07/06  19:44:30  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.2.58.7  1996/07/01  09:05:55  xliu
 * CSCdi61744:  fail to select correct crypto maps when multiple are
 * created
 * Branch: California_branch
 *
 * Revision 3.2.58.6  1996/06/21  22:28:50  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 3.2.58.5  1996/06/18  01:43:23  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.2.58.4  1996/06/07  00:24:09  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.58.3  1996/05/12  23:12:19  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/04/27  05:46:09  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.18.1  1996/04/26  00:12:25  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.2.58.1  1996/04/19  14:55:50  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.6.34  1996/04/17  23:47:20  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 2.1.6.33  1996/04/16  21:52:21  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.6.32  1996/04/12  23:07:27  carrel
 * Branch: IosSec_branch
 * Fix wildcarding in ACL for crypto map.  Also some cleanup.
 *
 * Revision 2.1.6.31  1996/04/09  20:02:35  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.30  1996/03/28  21:02:38  bew
 * Branch: IosSec_branch
 * Change the name from send_echo_continued() to the more standard
 * send_echo_internal().
 *
 * Revision 2.1.6.29  1996/03/01  01:39:01  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.6.28  1996/02/28  19:29:48  bew
 * Branch: IosSec_branch
 * Changes relating to the Sync.
 *
 * Revision 2.1.6.27  1996/02/13  19:15:51  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.6.26  1996/01/24  22:55:50  bew
 * Branch: IosSec_branch
 * Sync-related changes to sys/crypto.
 *
 * Revision 2.1.6.25  1995/12/20  23:30:45  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 2.1.6.24  1995/12/12  23:09:31  bew
 * Branch: IosSec_branch
 * Fixes for CS & better formatting of debug messages.
 *
 * Revision 2.1.6.23  1995/11/30  05:57:22  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the new crypto engine code.
 *
 * Revision 2.1.6.22  1995/11/28  01:46:34  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.21  1995/11/17  16:33:18  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.20  1995/10/16  04:04:19  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.6.18  1995/10/10  18:19:36  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.6.17  1995/10/03  18:51:24  xliu
 * Branch: IosSec_branch
 *
 * Moved epa dir under crypto dir. Renamed epa dir to crypto_engine dir.
 * Create crypto_lib under crypto_engine which include Cylink sw crypto
 * libs.
 *
 * Revision 2.1.6.16  1995/09/29  20:49:36  xliu
 * Branch: IosSec_branch
 *
 * Set encrypt/decrypt offset to be 8 byte off transport header.
 *
 * Revision 2.1.6.15  1995/09/27  23:20:51  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1.6.14  1995/09/27  06:09:11  carrel
 * Branch: IosSec_branch
 * Allow both Encryption and Decryption to occur to the same packet.
 *
 * Revision 2.1.6.13  1995/09/26  04:12:04  carrel
 * Branch: IosSec_branch
 * Initial commit of encryption ACL code.
 *
 * Revision 2.1.6.12  1995/09/22  01:21:58  bew
 * Branch: IosSec_branch
 * Fixes for problems found during ACL code integration with connection
 * setup.
 *
 * Revision 2.1.6.11  1995/09/14  19:32:36  bew
 * Branch: IosSec_branch
 * 1) Implemented Cylink code to find a reflected conn message.
 * 2) Moved handling of a CIM message to be the same as a CRM (send
 * out through icmp code rather than directly to ipwrite().
 * 3) Replaced a malloc() in crypto_conn_entry_initiate_ip() with
 * a private chunk of memory so the routine can be used at
 * interrupt level.
 *
 * Revision 2.1.6.10  1995/09/12  19:46:12  bew
 * Branch: IosSec_branch
 * The network address in the crypto connection message is now used
 * correctly.
 *
 * Revision 2.1.6.9  1995/09/08  08:03:06  xliu
 * Branch: IosSec_branch
 * Eliminate CRYPTO_FAST_ENCRYPT/DECRYPT cryptoflags. Replace it with the
 * switch_mode input parameter to epa encrypt/decrypt api.
 * Replace CRYPTO_DONE cryptoflag with CRYPTO_ENCRYPT_DONE and
 * CRYPTO_DECRYPT_DONE flag to fix the bug caused by reused pak in ping
 * application.
 *
 * Revision 2.1.6.8  1995/09/02  01:48:33  bew
 * Branch: IosSec_branch
 * Inclusion of connection id in the CIB entry, and preparation
 * for calling the DH code.
 *
 * Revision 2.1.6.7  1995/08/30  20:41:39  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1.6.6  1995/08/16  23:43:52  bew
 * Branch: IosSec_branch
 * Fixed a couple of small bugs.
 *
 * Revision 2.1.6.5  1995/08/11  00:59:17  xliu
 * Branch: IosSec_branch
 *
 * Modified epa_parser.c with the updated epa interface.
 *
 * Revision 2.1.6.4  1995/08/02  19:24:32  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 2.1.6.3  1995/07/21  17:38:28  xliu
 * Branch: IosSec_branch
 *
 * Taken out the code to malloc the context at the encryption and
 * decryption call.
 *
 * Revision 2.1.6.2  1995/07/21  04:36:35  xliu
 * Branch: IosSec_branch
 * Update epa_parser.c with new epa api. Add CRYPTO_DECRYPT flag.
 *
 * Revision 2.1.6.1  1995/07/01  05:31:22  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 3.1  1995/11/09  11:13:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:21:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.6  1995/06/23  03:00:45  bew
 * Branch: IosSec_branch
 * Check the return from malloc().
 *
 * Revision 1.1.2.5  1995/06/06  21:33:54  bew
 * Branch: IosSec_branch
 * Cleanup of crypto subsystem debugging. Crypto debug is now accessed
 * through the standard parser "debug" command.
 *
 * Revision 1.1.2.4  1995/06/01  22:56:13  bew
 * Branch: IosSec_branch
 * Addition of new crypto_ip_* functions which contain IP-specific
 * functions (duh) some of which have been moved from crypto.c. Also, some
 * macros have been renamed to better indicate they are IP specific. Many
 * of the functions have been developed with an eye to placing them under
 * a registry in the future.
 *
 * Revision 1.1.2.3  1995/05/27  16:37:54  xliu
 * Branch: IosSec_branch
 * Modify epa_parser to support new api.
 * Modify crypto_ip_callback for encryption case.
 *
 * Revision 1.1.2.2  1995/04/28  01:37:42  bew
 * Branch: IosSec_branch
 * Change debugging default to be off & fix a bug where encryption isn't
 * done when debug is off.
 *
 * Revision 1.1.2.1  1995/04/27  19:23:38  bew
 * Branch: IosSec_branch
 * Adding IP level files which communicate with the EPA API, and Utility
 * files used by both the parser EPA support and the IP level files.
 *
 * Revision 1.1  1995/04/27 19:11:58  bew
 * Placeholder files for crypto IP and Utility files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "config.h"
#include "packet.h"
#include "sched.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../util/random.h"
#include "crypto.h"
#include "crypto_engine_api.h"
#include "crypto_map.h"
#include "crypto_sesmgmt.h"
#include "crypto_public.h"
#include "crypto_util.h"
#include "crypto_ip.h"
#include "../ui/debug.h"
#include "crypto_debug.h"
#include "../crypto/parser_defs_crypto.h"
#include "crypto_pubkey.h"
#include "crypto_glue.h"
#include "crypto_conntable.h"
#include "address.h"

#define PDU_INIT_CONNID_OFFSET 24
#define PDU_RESP_CONNID_OFFSET 28
#define PDU_CFM_RP_CID_OFFSET  4

/*
 * crypto_ip_send_echo
 * Send a special connecction message ICMP Echo to a host and block for a reply.
 * Returns TRUE if Echo Reply received.  FALSE under all other circumstances.
 */

boolean
crypto_ip_send_echo (ipaddrtype destination, echotype *data, 
		  	     int bytes, ipaddrtype source, uchar code, 
			     char *optional_data, int optional_data_length)
{
    paktype *pak;
    iphdrtype *ip;
    icmptype *icmp;
    int databytes, headsize;
    ushort *dataptr;

    headsize = IPHEADERBYTES(data->options) + sizeof(icmptype);
    if (bytes < headsize)
	bytes = headsize;
    pak = getbuffer(bytes);
    if (!pak)
	return(FALSE);
    pak->datagramsize = bytes;

    ip = (iphdrtype *)ipheadstart(pak);
    
    /*
     * Set the flag indicating the packet needs to be signed. The signing
     * will take place in datagram_out.
     */
    pak->cryptoflags |= CRYPTO_SIGN_ME;
    /*
     * Set the flag indicating that we've sourced this connection message.
     * This keeps the crypto map check from accidentally discovering it as a 
     * connection message and queueing it up for us.
     */
    pak->cryptoflags |= CRYPTO_CM;
   
    /*
     * We initialize the packet buffer by zeroing the IP and ICMP header
     * and putting the connection message in the rest of the packet.
     * Note that the first word of the data is reserved for a msclock
     * value inserted later.
     */
    databytes = bytes - headsize;
    icmp = (icmptype *) ((char *)ip + IPHEADERBYTES(data->options));
    dataptr = (ushort *)(icmp + START_OF_ICMP_DATA);

    /*
     * Copy the connection message into the optional_data portion of the ICMP
     * packet.
     */
    if (optional_data != NULL) {
	bcopy(optional_data, dataptr, optional_data_length);
    }

    memset(pak->network_start, 0, bytes-databytes);

    /*
     * Assume no fragmentation of ICMP message is allowed, and that
     * we are not broadcasting.
     */ 
     return(send_echo_internal(destination, data, bytes, source, FALSE,
			       NULL, pak, code, TRUE));
}

crypto_sesmgmt_message_types crypto_ip_find_connection_msg_type (paktype *pak)
{
    iphdrtype *ip;
    icmptype *icmp;
    ushort *dataptr;
    crypto_sesmgmt_message_types msg_type;

    ip = (iphdrtype *)ipheadstart(pak);
    icmp = (icmptype *)((char *)ip + ltob(ip->ihl));
    dataptr = (ushort *)(icmp + START_OF_ICMP_DATA);

    (void) crypto_glue_find_message_type((uchar *)dataptr,&msg_type);
    return(msg_type);
}

void
crypto_ip_queue_conn_message (paktype *pak, crypto_maptype *map, int method)
{
    crypto_sesmgmt_block *blk;
    iphdrtype *ip;
    icmptype *icmp;
    ushort *dataptr;

    blk=malloc(sizeof(crypto_sesmgmt_block));
    if (blk == NULL) {
	retbuffer(pak);
	return;
    }

    blk->message_type = crypto_ip_find_connection_msg_type(pak);
    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * If the message type is a CIM, a map argument was given from which we
     * get the info to fill in blk. If the message type was CRM or CCM, we 
     * should have opened a session with the given peer, so use the Cylink 
     * data structures to find that session.
     */
    if (blk->message_type == CIM) {
	if (!map) {
	    if (crypto_sm_debug) {
		buginf("\nCRYPTO: map not provided for CIM!");
	    }
	    return;
	}

	/*
	 * Always choose a new pending id. If this is a new connection we
	 * ain't got one yet, and if this is a race condition we'll need it
	 * later on anyway to replace the old one. But if the crypto map
	 * already has a pending id we don't want to change it until we find
	 * out later what's really going on (e.g., hit a race condition, or
	 * if this is a re-transmission.)
	 */
	blk->pending_conn_id = crypto_conn_find_uniq_pending_value();
    	if (!crypto_conntable_conn_is_pending(map->conn_id)) {
	    map->conn_id =  blk->pending_conn_id;
    	}

        blk->pe.ip_addr  = map->pe.ip_addr;
        blk->upe.ip_addr = map->upe.ip_addr;
	blk->slot = map->crypto_engine_id;
    	blk->method = method;
    	blk->peer = map->peers;
    	blk->mode_flags = map->mode_flags;
    } else {
    	icmp = (icmptype *)((char *)ip + ltob(ip->ihl));
    	dataptr = (ushort *)(icmp + START_OF_ICMP_DATA);
	/*
	 * Find the pending connection entry & return the PE/UPE info in blk.
	 */
	if (!crypto_glue_find_pending_conn((uchar *)dataptr, blk)) {
	    retbuffer(pak);
	    free(blk);
	    return;
	}
    }

    /*
     * Fill in the common info. Some may be NULL
     */
    blk->src.ip_addr = ip->srcadr;
    blk->dst.ip_addr = ip->dstadr;
    blk->pe.type = blk->upe.type = ADDR_IP;
    blk->src.type = blk->dst.type = ADDR_IP;
    blk->pak = pak;

    crypto_sesmgmt_enqueue(blk);
}

void
crypto_ip_find_sig_params (uchar *dataptr, crypto_sig_params *params)
{
    /* 
     * Set to NULL to indicate error. It is set to the clear data pointer
     * at the end which indicates success.
     */
    params->clear_data = NULL;

    params->clear_data_length = 
	    crypto_glue_get_msg_length((uchar *)dataptr, FALSE);
    if (params->clear_data_length == 0) {
	return;
    }

    params->signature = crypto_glue_find_sig_addr((uchar *)dataptr);
    if (params->signature == NULL) {
	return;
    }

    params->signature_length = CRYPTO_DEF_SIG_SIZE;
   
    /*
     * Add the data pointer last to indicate success.
     */
    params->clear_data = (char *)dataptr;

    return;
}

void 
crypto_ip_recompute_icmp_checksum (paktype *pak)
{
    iphdrtype *ip;
    icmptype *icmp;

    ip = (iphdrtype *)ipheadstart(pak);
    icmp = (icmptype *)((char *)ip + ltob(ip->ihl));

    icmp->checksum=0; /* Get rid of old checksum! */
    icmp->checksum=ipcrc((ushort *)icmp, (n2h(ip->tl) - ltob(ip->ihl)));
}

void 
crypto_ip_verify_icmp_checksum (paktype *pak, boolean *result)
{
    iphdrtype *ip;
    icmptype *icmp;

    ip = (iphdrtype *)ipheadstart(pak);
    icmp = (icmptype *)((char *)ip + ltob(ip->ihl));

#if 0
/* XL: need investigation. When ipcrc is enabled the packets are dropped.*/
/* This only happens when we migirage to vip2.                           */
    crypto_dump_data(pak->datagramstart, pak->datagramsize);
    if (ipcrc((ushort *)icmp, 
	      (n2h(ip->tl) - ltob(ip->ihl))
	      )) {
	ip_dispose(&icmp_traffic.checksumerr, pak);
	*result = FALSE;
    } else {
	*result = TRUE;
    }
#endif
    *result = TRUE;
}


void
crypto_ip_find_cm (paktype *pak, uchar **location)
{
    iphdrtype *ip;
    icmptype *icmp;
    ushort *dataptr;
    
    ip = (iphdrtype *)ipheadstart(pak);
    icmp = (icmptype *)((char *)ip + ltob(ip->ihl));
    dataptr = (ushort *)(icmp + START_OF_ICMP_DATA);
    *location = (uchar *) dataptr;
    return;
}

void
crypto_ip_find_echo (paktype *pak, uchar **location)
{
    iphdrtype *ip;
    icmptype *icmp;
    
    ip = (iphdrtype *)ipheadstart(pak);
    icmp = (icmptype *)((char *)ip + ltob(ip->ihl));
    *location = (uchar *) icmp;
    return;
}

/*
 * Send a connection message as an echo message out an interface, but don't 
 * wait for the reply. Returns the type of message which was sent, or
 * Unsuccessful_Message if there was an error.
 */
crypto_sesmgmt_message_types
crypto_ip_send_echo_based_msg (crypto_sesmgmt_block *blk)
{
    echotype edata;
    char *message;
    int message_length;
    int total_length;
    ipaddrtype source, destination;
    crypto_sesmgmt_message_types real_message_type;

    /*
     * Determine the message type.
     */
    if (crypto_glue_find_message_type((uchar *)blk->message, &real_message_type)
	     < 0) {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: Invalid message type found.");
	}
	return(Unsuccessful_Message);
    }

    /*
     * NOTE! Each case in this switch statement is assumed to have put valid
     *	     data in the following variables:
     *	source		- source IP address for this message (local IP address)
     *  destination	- dest IP address for this message (peer IP address)
     *  message		- beginning of the connection message
     *  message_length	- length of the connection message
     *	     If any of these variables cannot be determined the show cannot
     *	     go on thus the process must be killed.
     */
    switch (real_message_type) {

	case CIM:
	case CRM:
	case CCM: 
	    /*
	     * Find the pre-determined source & destination -- the original 
	     * PE and UPE for a CIM, the network addresses of the encrypting 
	     * routers fro CRM, CCM.
	     */
	    source=blk->src.ip_addr;
	    destination=blk->dst.ip_addr;
	    message= blk->message;
	    message_length = (int) blk->message_length;
	    break;

    	default:
	    if (crypto_sm_debug) {
	        buginf("\nCRYPTO: crypto_send_echo_based_msg(): "
			"Invalid message_type");
	    }
	    return(Unsuccessful_Message);
    }

    /*
     * Build echo data structure
     */
    edata.sequence = random_gen();
    edata.tos = TOS_PREC_ROUTINE;
    edata.options = NULL;
    edata.interactive = FALSE;
    edata.broadcast = FALSE;
    edata.id = 1;
    edata.ttl = TTL_DEFAULT;
    /*
     * Don't wait for the reply.
     */
    TIMER_START(edata.timeout, 0); 
	
    /*
     * Determine total length of packet. This includes IP header, ICMP
     * header, and ICMP data portion (i.e., connection message).
     */
    total_length = IPHEADERBYTES(edata.options) + sizeof(icmptype) +
	message_length;
    
    /*
     * Send the echo & block waiting for the return message.
     */
    if (crypto_ip_send_echo(destination,
     			    &edata, total_length, 
			    source,
			    CRYPTO_ICMP_CODE,
			    message, message_length)) {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: ICMP message failed: s=%i, d=%i - Link down?",
		    source, destination);
	}
    } else {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: ICMP message sent: s=%i, d=%i", source,
		   destination);
	}
    }

    return(real_message_type);
}
crypto_sesmgmt_message_types
crypto_get_cs_msg_type (paktype *pak)
{
    iphdrtype *ip;
    icmptype *icmp;
    ushort *dataptr;
    crypto_sesmgmt_message_types message_type;

    ip = (iphdrtype *)ipheadstart(pak);
    icmp = (icmptype *)((char *)ip + ltob(ip->ihl));
    dataptr = (ushort *)(icmp + START_OF_ICMP_DATA);

    /*
     * Determine message type
     */
    if (crypto_glue_find_message_type((uchar *)dataptr, &message_type)<0) {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: Invalid message type found");
	}
	return Undeclared_Message;
    }

    return message_type;
}

