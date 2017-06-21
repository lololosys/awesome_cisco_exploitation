/* $Id: crypto.c,v 3.2.60.10 1996/09/11 23:24:57 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto.c,v $
 *------------------------------------------------------------------
 * crypto.c - Protocol-independent crypto function
 *
 * May 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto.c,v $
 * Revision 3.2.60.10  1996/09/11  23:24:57  bew
 * CSCdi68438:  crypto determination of network address should be done
 * earlier
 * Branch: California_branch
 * Run the routing tables during connection message setup to determine
 * which interface the packet will go out.
 *
 * Revision 3.2.60.9  1996/08/28  19:29:01  bew
 * CSCdi66328:  crypto connection set too early on CRM side
 * Branch: California_branch
 * Use the pending connection id on the CRM side to tag crypto maps. The
 * pending conn id is replaced by connection id when connection
 * established as on initiating side.
 *
 * Revision 3.2.60.8  1996/07/20  01:02:29  bew
 * CSCdi62017:  %SCHED-3-THRASHING: Process thrashing on watched queue
 * Crypto SM
 * Branch: California_branch
 * Push the existing event list before waiting on watched booleans,
 * awaiting a crypto engine response, and pop it back when done
 * waiting. There was confusion in dealing with the other events
 * (timers, queue) which this process waits on when not scheduled.
 *
 * Revision 3.2.60.7  1996/07/15  10:04:53  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.2.60.6  1996/07/06  19:44:20  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.2.60.5  1996/07/01  09:05:46  xliu
 * CSCdi61744:  fail to select correct crypto maps when multiple are
 * created
 * Branch: California_branch
 *
 * Revision 3.2.60.4  1996/06/07  00:23:56  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.60.3  1996/05/30  20:23:55  bew
 * CSCdi58950:  Need to make exportable crypto images
 * Branch: California_branch
 * Added the exportable crypto subsystem. The guts of crypto_init() are
 * moved into crypto.c, and crypto_exportable_init.c is added. The
 * strong_encryption_required boolean added and set appropriately
 * by both the crypto and exportable crypto subsystems.
 *
 * Revision 3.2.60.2  1996/05/12  23:12:10  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/19  14:55:28  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.6.37  1996/04/17  23:47:13  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 2.1.6.36  1996/04/16  21:51:46  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.6.35  1996/04/09  20:02:24  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.34  1996/03/30  05:11:12  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 2.1.6.33  1996/03/25  18:43:35  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 2.1.6.32  1996/03/21  19:56:35  bew
 * Branch: IosSec_branch
 * Move crypto export message to be printed from crypto_init().
 *
 * Revision 2.1.6.31  1996/03/13  22:44:51  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.6.30  1996/03/13  00:51:28  bew
 * Branch: IosSec_branch
 * Add "Compliance with Export Laws" banner when an image with crypto
 * boots.
 *
 * Revision 2.1.6.29  1996/03/01  01:38:44  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.6.28  1996/01/25  18:19:44  bew
 * Branch: IosSec_branch
 * Implemented a config option for automatic pre-genereration of
 * Diffie-Hellman key pairs. Also changed the use of timers so
 * that the the STUCKTMR message doesn't show up any more.
 *
 * Revision 2.1.6.27  1996/01/18  23:23:07  bew
 * Branch: IosSec_branch
 * The crypto session manager no longer forks off connection messages but
 * handles them itself serially. This is due to SDU connection setup code
 * not handling the multitasking gracefully.
 *
 * Revision 2.1.6.26  1996/01/16  20:20:49  bew
 * Branch: IosSec_branch
 * Simplified the session mgmt debug msgs & changed the way that duplicate
 * connection messages are handled.
 *
 * Revision 2.1.6.25  1996/01/13  05:46:34  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 2.1.6.24  1996/01/04  18:47:44  xliu
 * Branch: IosSec_branch
 * Crypto moduliztion.
 *
 * Revision 2.1.6.23  1995/12/20  23:30:40  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 2.1.6.22  1995/12/02  17:44:28  xliu
 * Branch: IosSec_branch
 *
 * Add parser command to precompute dh number.
 *
 * Revision 2.1.6.21  1995/11/30  05:52:13  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the new crypto engine code.
 *
 * Revision 2.1.6.20  1995/11/28  01:46:19  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.19  1995/11/17  16:33:11  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.18  1995/10/17  01:19:31  bew
 * Branch: IosSec_branch
 * Add the simple unauthenticated public key exchange protocal. Cleaned
 * up the printing of pulic keys. Modified some show commands syntax to
 * match the functional spec. General cleanup.
 *
 * Revision 2.1.6.17  1995/10/16  04:04:12  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.6.15  1995/10/10  18:19:27  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.6.14  1995/10/03  18:51:20  xliu
 * Branch: IosSec_branch
 *
 * Moved epa dir under crypto dir. Renamed epa dir to crypto_engine dir.
 * Create crypto_lib under crypto_engine which include Cylink sw crypto
 * libs.
 *
 * Revision 2.1.6.13  1995/09/29  01:00:29  bew
 * Added session key timeouts. Also renamed epa_parser.c to
 * crypto_parser.c & changed the timestamp used in SDU code
 * to be the seconds from Day 0 rather than the contents of
 * msclock.
 *
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.12  1995/09/27  23:20:38  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1.6.11  1995/09/12  19:46:06  bew
 * Branch: IosSec_branch
 * The network address in the crypto connection message is now used
 * correctly.
 *
 * Revision 2.1.6.10  1995/09/09  01:27:03  bew
 * Branch: IosSec_branch
 * General cleanup & bug fixing in crypto.
 *
 * Revision 2.1.6.9  1995/09/07  22:40:59  bew
 * Branch: IosSec_branch
 * Connection setup now sets up a shared DH number from which DES keys
 * can be derived.
 *
 * Revision 2.1.6.8  1995/09/01  21:37:28  bew
 * Branch: IosSec_branch
 * Implementation of 11.0 scheduling, implementation of
 * pending connections timeout handling, addition of
 * routine to safely pick crypto connection ids, and
 * other cleanup.
 *
 * Revision 2.1.6.7  1995/09/01  00:16:53  bew
 * Branch: IosSec_branch
 * Fix bug with verify.
 *
 * Revision 2.1.6.6  1995/08/30  20:41:33  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1.6.5  1995/08/17  00:43:41  xliu
 * Branch: IosSec_branch
 * Incorporate the bug fix on sign and verify.
 *
 * Revision 2.1.6.4  1995/08/16  23:43:51  bew
 * Branch: IosSec_branch
 * Fixed a couple of small bugs.
 *
 * Revision 2.1.6.3  1995/08/02  19:24:27  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 2.1.6.2  1995/07/21  04:36:31  xliu
 * Branch: IosSec_branch
 * Update epa_parser.c with new epa api. Add CRYPTO_DECRYPT flag.
 *
 * Revision 2.1.6.1  1995/07/01  05:31:16  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 3.1  1995/11/09  11:12:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:20:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.4  1995/06/23  03:00:46  bew
 * Branch: IosSec_branch
 * Check the return from malloc().
 *
 * Revision 1.1.2.3  1995/06/21  20:37:23  xliu
 * Branch: IosSec_branch
 *
 * Incorporate the changes made by epa api.
 *
 * Revision 1.1.2.2  1995/06/06  21:33:51  bew
 * Branch: IosSec_branch
 * Cleanup of crypto subsystem debugging. Crypto debug is now accessed
 * through the standard parser "debug" command.
 *
 * Revision 1.1.2.1  1995/06/02  00:19:52  bew
 * Branch: IosSec_branch
 * New crypto files -- Session Management files and cleanup of the
 * subsystem.
 *
 * Revision 1.1  1995/06/01 23:16:21  bew
 * Placeholders for crypto/crypto* files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "globs.h"
#include "interface_private.h"
#include "../os/sched_private.h"
#include "../if/network.h"
#include "../ipc/ipc_util.h"
#include "../ip/ip.h"
#include "../../boot/cpu.h"
#include "crypto_public.h"
#include "crypto.h"
#include "crypto_engine_api.h"
#include "crypto_map.h"
#include "crypto_sesmgmt.h"
#include "address.h"
#include "interface.h"
#include "crypto_ip.h"
#include "../ui/debug.h"
#include "crypto_debug.h"
#include "string.h"
#include "../os/list.h"
#include "sched.h"
#include "crypto_glue.h"
#include "crypto_util.h"
#include "../crypto/crypto_registry.h"

ulong crypto_processor_serial;

ushort crypto_number_of_slots;

uint crypto_supported_algorithms;

typedef struct crypto_callback_block_ {
    watched_boolean *done;
    ushort status;
    union {
    	epa_rtn_alg_param param;
    	epa_rtn_syndrome syndrome;
    	epa_rtn_sig signature;
    } returned_data;
} crypto_callback_block;

/*
 * Callback for the generic crypto facility.
 */
void
crypto_callback (
    epa_callback_context *context,
    ushort event_name,
    ushort epa_op_status,
    void *epa_rsp_buffer,
    ushort epa_rsp_buffer_size,
    struct epa_pak_info_ *epa_pak_info)
{
    crypto_callback_block *blk;
    ushort *pass_or_fail;

    blk=context->proc_data_buffer;

    /*
     * NOTE: The crypto engine returns events of type "epa_crypto_command",
     *           NOT of type "epa_event_name".
     */
    switch (event_name) {
        case CRYPTO_SIGN:
        case CRYPTO_VIP_SIGN:
	    if (epa_op_status == EPA_OP_OK) {
	        memcpy((uchar *) &(blk->returned_data.signature), 
	        	(uchar *)epa_rsp_buffer, epa_rsp_buffer_size);
	    }
	    blk->status = epa_op_status;
	    process_set_boolean(blk->done, TRUE);
            break;

        case CRYPTO_VERIFY:
        case CRYPTO_VIP_VERIFY:
	    if (epa_op_status == EPA_OP_OK) {
	        pass_or_fail = epa_rsp_buffer;
	        blk->status = *pass_or_fail;
	    } else {
	        blk->status = CRYPTO_INVALID_SIGNATURE;
	    }
	    process_set_boolean(blk->done, TRUE);
	    break;

        case CRYPTO_GEN_DH_ALG_PHASE1_PARAM:
	    if (epa_op_status == EPA_OP_OK) {
	        memcpy((uchar *) &(blk->returned_data.param), 
	        	(uchar *)epa_rsp_buffer, epa_rsp_buffer_size);
	    }
	    blk->status = epa_op_status;
	    process_set_boolean(blk->done, TRUE);
	    break;
        
	case CRYPTO_GEN_DH_ALG_PHASE2_PARAM:
	    blk->status = epa_op_status;
	    process_set_boolean(blk->done, TRUE);
	    break;
        
        case CRYPTO_CREATE_SESSION_KEY:
	    blk->status = epa_op_status;
	    process_set_boolean(blk->done, TRUE);
            break;

        case CRYPTO_GEN_SYNDROME:
	    if (epa_op_status == EPA_OP_OK) {
	        memcpy((uchar *) &(blk->returned_data.syndrome), 
	        	(uchar *)epa_rsp_buffer, epa_rsp_buffer_size);
	    }
	    blk->status = epa_op_status;
	    process_set_boolean(blk->done, TRUE);
            break;

        default:
	    if (crypto_sm_debug) {
       	    	buginf("\nCRYPTO: crypto_callback: Unknown event type %d",
                         event_name); 
	    }
	    if (blk) { /* attempt to reset a boolean */
	        process_set_boolean(blk->done, TRUE);
	    }
            break;
    }
}


boolean crypto_pregen_dh_pair(ushort conn_id, ushort slot)
{
    epa_callback_context context;
    crypto_callback_block block;
    epa_alg_obj algorithm;
    boolean status;

    context.req_timeout= CRYPTO_KEY_COMM_TIMEOUT; 
    context.proc_data_buffer=&block;
    algorithm.algorithm_type = DH_ALG_PHASE1;
    block.done = create_watched_boolean("Crypto DH Precalc", 0);
    process_push_event_list(NULL); /* don't be bothered by q events & timers */
    process_watch_boolean(block.done, ENABLE, ONE_SHOT);
    status=FALSE;

    if (epa_generate_alg_parameter(crypto_get_engine_id(slot), 
                                   &algorithm, conn_id,
            &context, crypto_callback) == EPA_OP_OK) {
       
        process_wait_for_event();

        if (crypto_sm_debug) {
            buginf("\nCRYPTO: DH phase 1 status for conn_id %d slot %d:%s",
                        conn_id, slot, crypto_xlate_status(block.status));
        }
	if (block.status == EPA_OP_OK) {
	    status = TRUE;
	}
    } else {
        buginf("\nCRYPTO: failed to generate DH pair.\n");
    }

    delete_watched_boolean(&block.done);
    process_pop_event_list(NULL); /* restore queue events & timers */
    return(status);
}

/*
 * Send connection message to the crypto engine for signing & delivery to the 
 * interface.
 */
boolean
crypto_sign_me (ushort slot, char *data_start, ushort data_length,
		char *signature_start)
{
    epa_callback_context context;
    epa_alg_obj alg;
    crypto_callback_block block;
    boolean status = FALSE;
    
    block.done = create_watched_boolean("Crypto Verify", 0);
    process_push_event_list(NULL);/* don't be bothered by q events & timers */
    process_watch_boolean(block.done, ENABLE, ONE_SHOT);

    context.req_timeout=60;	/* what is a good real value? */
    context.proc_data_buffer=&block; 
    context.req_timeout= CRYPTO_KEY_COMM_TIMEOUT;	

    alg.algorithm_type=DSS_SHA_SIGN;
    if (epa_sign(slot, data_start, data_length, &alg, 
		      &context, crypto_callback) == EPA_OP_OK) {
        process_wait_for_event();

        if (crypto_sm_debug) {
	     buginf("\nCRYPTO: Sign done. Status=%s", 
		    crypto_xlate_status(block.status));
        }

	if (block.status == EPA_OP_OK) {
	    status = TRUE;
	    /*
	     * Copy signature into connection message.
	     */
	    memcpy(signature_start, (char *) &(block.returned_data.signature), 
	           CRYPTO_DEF_SIG_SIZE);
	}

    } 

    delete_watched_boolean(&block.done);
    process_pop_event_list(NULL); /* restore queue events & timers */

    return(status);

}

/*
 * Send a message to the crypto engine for verification from the SDU code.
 */

boolean 
crypto_verify_me_sdu(uchar *start, uchar *sig, ushort slot, uchar *dss_pub_num)
{
    epa_callback_context context;
    epa_status ret;
    epa_alg_obj alg;
    static crypto_callback_block block;
    epa_key_obj key_obj;
    
    /*
     * Initialize a watched boolean. We want a unique one per process in the
     * case of multiple proccesses waiting on a verify simultaneously.
     */
    block.done = create_watched_boolean("Crypto Verify", 0);
    process_push_event_list(NULL); /* don't be bothered by q events & timers */
    process_watch_boolean(block.done, ENABLE, ONE_SHOT);
    
    /*
     * Make an epa_key_obj out of the public key.
     */
    key_obj.key_type = DSS_PUBLIC_KEY;
    key_obj.key_tag.dss_public_key.size = CRYPTO_DSS_PUBLIC_KEY_MAX_SIZE;
    key_obj.key_tag.dss_public_key.key_value=dss_pub_num;

    /*
     * Setup context for EPA
     */
    context.req_timeout= CRYPTO_KEY_COMM_TIMEOUT;	
    context.proc_data_buffer = &block;

    alg.algorithm_type=DSS_SHA_VERIFY;

    if ((ret=epa_verify(slot, start, (sig - start), sig, CRYPTO_DEF_SIG_SIZE, 
			 &alg, &key_obj, &context, &crypto_callback))
		== EPA_OP_OK) {

        process_wait_for_event();

        if (crypto_sm_debug) {
	     buginf("\nCRYPTO: Verify done. Status=%s",
		((block.status == CRYPTO_VALID_SIGNATURE) ? "OK" : "FAIL"));
        }
        delete_watched_boolean(&block.done);
        process_pop_event_list(NULL); /* restore queue events & timers */
        return(block.status == CRYPTO_VALID_SIGNATURE);
    } else {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: epa_verify() queueing failed, ret=%d", ret);
	}
    	delete_watched_boolean(&block.done);
        process_pop_event_list(NULL); /* restore queue events & timers */
	return(FALSE);
    } 
}

inline boolean crypto_ip_packet_needs_encryption (paktype *pak)
{
    iphdrtype *ip;

    ip = (iphdrtype *)ipheadstart(pak);

    if ((pak->cryptoflags & CRYPTO_ENCRYPT) &&
	!(pak->cryptoflags & CRYPTO_ENCRYPT_DONE))
	return(TRUE);
    else
	return(FALSE);
}

/*
 * Temporary call to IP level routine
 */
boolean 
crypto_packet_needs_encryption(paktype *pak)
{
    return(crypto_ip_packet_needs_encryption(pak));
}

/*
 * Generate the syndrome.
 * This function waits for the crypto engine to finish before returning.
 */
boolean
crypto_generate_syndrome (short conn_id, ushort slot, short offset,
			  short challenge_length, uchar *syndrome)
{
    epa_callback_context context;
    crypto_callback_block block;
    boolean status = FALSE;
    
    context.req_timeout=0;		/* what is a good real value? */
    context.proc_data_buffer=&block; 
    block.done = create_watched_boolean("Crypto Syndrome Generation", 0);
    process_push_event_list(NULL); /* don't be bothered by q events & timers */
    process_watch_boolean(block.done, ENABLE, ONE_SHOT);

    if (epa_generate_syndrome(slot, conn_id, challenge_length, offset,
	    &context, crypto_callback) == EPA_OP_OK) {

        process_wait_for_event();

	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: Syndrome gen status for conn_id %d slot %d:%s",
		   conn_id, slot, crypto_xlate_status(block.status));
	}
	if (block.status == EPA_OP_OK) {
	    status = TRUE;
	    /*
	     * Copy syndrome.
	     */
	    memcpy(syndrome, (char *) &(block.returned_data.syndrome), 
		   CRYPTO_CHALLENGE_SIZE);
	} 
    } else {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: Syndrome generation for conn_id %d slot %d "
		   "FAILED:%s", conn_id, slot, 
				crypto_xlate_status(block.status));
	}
    }

    delete_watched_boolean(&block.done);
    process_pop_event_list(NULL); /* restore queue events & timers */
    return(status);
}

/* 
 * Generate the DH public/private numbers, or generate the
 * shared DH number. If generating the key pair the public number
 * is returned in the space provided by the calling sequence
 * in arg1. 
 *
 * The 'phase' arg is passed as a short so as to not cause crypto.h to have
 * a dependancy on crypto_engine_api.h.
 *
 * This function waits for the crypto engine to finish before returning.
 */
boolean
crypto_generate_DH_parameters (short phase, char *public_number, short conn_id,
			       ushort slot)
{
    epa_callback_context context;
    crypto_callback_block block;
    epa_alg_obj algorithm;
    boolean status = FALSE;
    
    context.req_timeout= CRYPTO_KEY_COMM_TIMEOUT;	
    context.proc_data_buffer=&block; 

    switch ((alg_types) phase) {
	case DH_ALG_PHASE1:
            algorithm.algorithm_type = DH_ALG_PHASE1;
	    break;
	case DH_ALG_PHASE2:
	    algorithm.algorithm_type = DH_ALG_PHASE2;
            /* 
	     * Give the crypto engine the UPE pub key 
	     */
    	    memcpy(algorithm.type_tag.dh_alg_phase2.peer_public_number,
	           (uchar *) public_number, DH_LENGTH_MIN);
	    break;
    	default:
	    if (crypto_sm_debug) {
	        buginf("\nCRYPTO: Invalid DH phase specified");
	    }
	    return(status);
    }
    block.done = create_watched_boolean("Crypto DH Generation", 0);
    process_push_event_list(NULL); /* don't be bothered by q events & timers */
    process_watch_boolean(block.done, ENABLE, ONE_SHOT);

    if (epa_generate_alg_parameter(slot, &algorithm, conn_id,
	    &context, crypto_callback) == EPA_OP_OK) {

        process_wait_for_event();

	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: DH gen phase %s status for conn_id %d slot %d:%s",
            	   (algorithm.algorithm_type == DH_ALG_PHASE1) ? "1": "2",
		   conn_id, 
		   slot,
		   crypto_xlate_status(block.status));
	}
	if (block.status == EPA_OP_OK) {
	    status = TRUE;
	    if (phase == DH_ALG_PHASE1) {
		/*
	     	 * Copy our public key into the space provided by the caller
	     	 */
	    	memcpy(public_number,
		       (char *) &(block.returned_data.param.alg_tag.dh_pub), 
		       sizeof(epa_rtn_DH_pub_number));
	    }
	} 
    } else {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: DH generation for conn_id %d slot %d FAILED:%s", 
				conn_id, 
				slot, 
				crypto_xlate_status(block.status));
	}
    }

    delete_watched_boolean(&block.done);
    process_pop_event_list(NULL); /* restore queue events & timers */
    return(status);
}

/*
 * Generate an encryption session key. 
 *
 * The 'encrypt_alg' arg is passed as a short so as to not cause crypto.h to 
 * have a dependancy on crypto_engine_api.h.
 */
boolean
crypto_create_session_key (short conn_id, ushort slot, short encrypt_alg)
{
    epa_callback_context context;
    crypto_callback_block block;
    epa_alg_obj alg;
    boolean status = FALSE;

    context.req_timeout= CRYPTO_KEY_COMM_TIMEOUT;
    context.proc_data_buffer=&block; 
    alg.algorithm_type = (alg_types) encrypt_alg;

    block.done = create_watched_boolean("Crypto Session Key Generation", 0);
    process_push_event_list(NULL); /* don't be bothered by q events & timers */
    process_watch_boolean(block.done, ENABLE, ONE_SHOT);

    if (epa_create_session_key(slot, &alg, conn_id, &context, crypto_callback) 
	    == EPA_OP_OK) {

        process_wait_for_event();

	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: Create encryption key for conn_id %d slot %d:%s",
			conn_id, slot, crypto_xlate_status(block.status));
	}
	if (block.status == EPA_OP_OK) {
	    status = TRUE;
	}; 
    } else {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: Create encryption key for conn_id %d slot %d "
		   "FAILED:%s", 
		   conn_id, 
		   slot, 
		   crypto_xlate_status(block.status));
	}
    }

    delete_watched_boolean(&block.done);
    process_pop_event_list(NULL); /* restore queue events & timers */
    return(status);
}

/*
 * Clear a connection entry out from a crypto engine.
 * This may be called while interrupts are disabled, so blocking not allowed!
 */
void crypto_clear_connection_entry(int conn_id, ushort slot)
{
    epa_status status;

    status = epa_clear_connection_entry(slot, (ushort)conn_id, NULL, NULL);
    if (crypto_sm_debug) {
        buginf("\nCRYPTO: Crypto Engine clear conn_id %d slot %d: %s", 
				conn_id, slot, crypto_xlate_status(status));
    }
}

/*
 * The following functions twiddle the system-wide list of algorithms.
 */

/*
 * Check to make sure there is at least 1 supported algorithm, and if 
 * not set one.
 */
void crypto_init_alg_vector (void)
{
    if (crypto_supported_algorithms == 0) {
	/*
	 * Set the default to either 56 bit DES (domestic) or 40 bit DES
	 * (international).
	 */
	if (crypto_sm_debug) {
		buginf("\nCRYPTO: Initializing crypto_supported_algorithms"
		       " to default value.\n");
	}
	crypto_set_alg_vector(crypto_get_default_algorithm(), TRUE);
    }
    return;
}

uint crypto_get_alg_vector (void)
{
    return (crypto_supported_algorithms);
}

/*
 * Set bit in crypto_supported_algorithms.
 * If set is TRUE then set the bit, if FALSE then clear the bit.
 */
void crypto_set_alg_vector (int algorithm, boolean set)
{
    if (set) {
    	crypto_supported_algorithms |= algorithm;
    } else {
	crypto_supported_algorithms &= ~algorithm;
	/*
	 * If there are NO algorithms left, then set the
	 * default one, to be consistent with the boot-time
	 * default rules. Yes, this uses recursion.
	 */
	if (crypto_supported_algorithms == 0) {
	   crypto_set_alg_vector(crypto_get_default_algorithm(), TRUE);
	}
    }
   return;
}

/*
 * Return TRUE if an algorithm is set in the Cisco group.
 */
boolean crypto_is_alg_set (int algorithm)
{
    if (crypto_supported_algorithms & algorithm) {
	return(TRUE);
    }
    return(FALSE);
}

/* BEW Can be replaced by a simple call to strong_crypto_allowed */
/*
 * crypto_is_exportable_system
 * this is the control knob for ALL export related restrictions
 * export_enable is defined in the os/main_*.c files based on the EXPORT_ENABLE
 * flag defined in makeimages.
 */
boolean crypto_is_exportable_system (void)
{
    return(!strong_crypto_allowed);
}

/*
 * Determine how many slots on this chasis. Make the best guess per platform
 * since there is no global variable telling us -- only a #define.
 * (The right thing to do would be to create a registry call to find out ....)
 */
ushort crypto_get_number_of_slots (void)
{
    ushort slots;

    if (crypto_chassis_has_slots()) {
	switch (cpu_type) {
	case CPU_RP1:
	    slots = 16;
	    break;
	case CPU_RSP:
	    slots = 13;
	    break;
	default:
	    slots = 16;
	}
    } else {
	slots = 1;
    }
    return(slots);
}

/*
 * Initialize the crypto subsystem
 */
void crypto_init_common (void)
{
    /*
     * Parser chain initialization
     */
    crypto_parser_init();

    /*
     * Find the processor serial number
     */
    crypto_processor_serial=platform_get_value(PLATFORM_VALUE_HARDWARE_SERIAL);
    if (crypto_processor_serial == 0x0) {
	crypto_processor_serial = reg_invoke_get_processor_serial_number();
    }
    if (crypto_processor_serial == 0x0) {
	printf("% Error! No serial number found for this platform.\n");
    }

    /*
     * Debug initialization
     */
    crypto_debug_init();

    /*
     * Chunk creation for crypto_conntable_* routines.
     */
    crypto_conn_init();
    crypto_glue_init_connection_ids();

    /*
     * Session Manager Initialization
     */
    crypto_sesmgmt_init();

    /*
     * Crypto map Initialization
     */
    crypto_map_init();

    /*
     * crypto engine init
     */
    crypto_engine_init(crypto_engine_rpsp_slot, CRYPTO_SWEPA);
}
