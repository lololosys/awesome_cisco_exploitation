/* $Id: crypto_parser.c,v 3.2.60.22 1996/09/04 17:25:38 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_parser.c,v $
 *------------------------------------------------------------------
 * crypto_parser.c - Routines called from the crypto parser code.
 *
 * September, 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_parser.c,v $
 * Revision 3.2.60.22  1996/09/04  17:25:38  xliu
 * CSCdi68014:  Zeroizing software generated DSS key on vip2 get wrong
 * error msg
 * Branch: California_branch
 *
 * Revision 3.2.60.21  1996/08/27  06:30:53  xliu
 * CSCdi67270:  crypto parser command clean up
 * Branch: California_branch
 * Incorporate comments from doc. review, as well as some syntax clean up.
 *
 * Revision 3.2.60.20  1996/08/07  18:16:03  xliu
 * CSCdi65394:  crypto zeroize delete watched boolean twice
 * Branch: California_branch
 *
 * Revision 3.2.60.19  1996/08/07  09:21:09  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 3.2.60.18  1996/08/06  02:26:49  bew
 * CSCdi64870:  The second PE is missing in a show crypto connection
 * Branch: California_branch
 * Fixed the display for 'show crypto connection' so that the PE is
 * displayed for every UPE. Got rid of an unnecessary function
 * call as a bonus.
 *
 * Revision 3.2.60.17  1996/08/05  22:50:30  sbelair
 * CSCdi64715:  Crypto zeroizing key on VIP2 crashed router
 * Branch: California_branch
 * crypto_zeroize() was freeing a key when the epa returned an error,
 * but the keys are already freed when crypto_pubkey_keypair_delete is
 * called.
 *
 * Revision 3.2.60.16  1996/07/31  03:11:55  bew
 * CSCdi64649:  Router complains crypto zeroize failed
 * Branch: California_branch
 * epa_zeroize() was erroneously returning an error in the RP/RSP case.
 *
 * Revision 3.2.60.15  1996/07/28  07:41:53  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * Branch: California_branch
 *
 * Revision 3.2.60.14  1996/07/26  07:40:13  bew
 * CSCdi64136:  show crypto connections shows unneeded status bits
 * Branch: California_branch
 * Only show some Cylink status bits. The others are not
 * meaningful to users.
 *
 * Revision 3.2.60.13  1996/07/22  18:53:52  bew
 * CSCdi62404:  password is not enabled when generating DSS key for EPA.
 * Branch: California_branch
 * Add password to gen-signature-keys when the target is an EPA.
 *
 * Revision 3.2.60.12  1996/07/17  09:04:23  xliu
 * CSCdi63107:  Show crypto conn eng stat crashes router
 * Branch: California_branch
 *
 * Revision 3.2.60.11  1996/07/16  01:54:11  xliu
 * CSCdi61961:  need to add crypto target to svip makefile
 * Branch: California_branch
 *
 * Revision 3.2.60.10  1996/07/15  10:05:10  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.2.60.9  1996/07/06  19:44:36  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.2.60.8  1996/07/03  05:54:33  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 3.2.60.7  1996/06/21  22:29:01  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 3.2.60.6  1996/06/07  00:24:15  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.60.5  1996/06/05  23:11:28  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 3.2.60.4  1996/05/31  06:53:15  bew
 * CSCdi51576:  no cr key-time...
 * Branch: California_branch
 * Fix no crypto key-timeout.
 *
 * Revision 3.2.60.3  1996/05/28  15:58:57  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/12  23:12:22  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/19  14:55:54  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.30  1996/04/17  23:47:25  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 2.1.2.29  1996/04/16  21:52:33  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.2.28  1996/04/12  23:07:36  carrel
 * Branch: IosSec_branch
 * Fix wildcarding in ACL for crypto map.  Also some cleanup.
 *
 * Revision 2.1.2.27  1996/04/09  20:02:41  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.2.26  1996/03/30  05:11:32  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 2.1.2.25  1996/03/26  22:45:07  xliu
 * Branch: IosSec_branch
 * Add testing command.
 *
 * Revision 2.1.2.24  1996/03/25  18:43:42  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 2.1.2.23  1996/03/21  10:07:06  xliu
 * Branch: IosSec_branch
 * Migrated to CA branch.
 *
 * Revision 2.1.2.22  1996/03/14  00:57:55  bew
 * Branch: IosSec_branch
 * Changed the crypto key-exchange protocol to be more intuitive.
 *
 * Revision 2.1.2.21  1996/03/13  22:45:11  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.2.20  1996/03/01  01:39:08  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 2.1.2.19  1996/02/29  19:03:32  bew
 * Branch: IosSec_branch
 * Commit of the private/encrypted NVRAM feature. This is in support
 * of the crypto project and allows portions of NVRAM to be
 * encrypted, and a portion to be private key storage for the IOS.
 *
 * Revision 2.1.2.18  1996/02/28  19:29:50  bew
 * Branch: IosSec_branch
 * Changes relating to the Sync.
 *
 * Revision 2.1.2.17  1996/02/13  19:15:52  carrel
 * Branch: IosSec_branch
 * Crypto Map checkin (finally)
 *
 * Revision 2.1.2.16  1996/01/25  18:19:54  bew
 * Branch: IosSec_branch
 * Implemented a config option for automatic pre-genereration of
 * Diffie-Hellman key pairs. Also changed the use of timers so
 * that the the STUCKTMR message doesn't show up any more.
 *
 * Revision 2.1.2.15  1996/01/19  07:00:38  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 2.1.2.14  1996/01/13  05:46:45  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 2.1.2.13  1996/01/04  18:47:49  xliu
 * Branch: IosSec_branch
 * Crypto moduliztion.
 *
 * Revision 2.1.2.12  1995/12/20  23:30:48  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 2.1.2.11  1995/12/12  23:09:34  bew
 * Branch: IosSec_branch
 * Fixes for CS & better formatting of debug messages.
 *
 * Revision 2.1.2.10  1995/12/10  09:33:15  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 2.1.2.9  1995/12/02  17:44:31  xliu
 * Branch: IosSec_branch
 *
 * Add parser command to precompute dh number.
 *
 * Revision 2.1.2.8  1995/11/30  05:57:24  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the new crypto engine code.
 *
 * Revision 2.1.2.7  1995/11/28  01:46:36  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.6  1995/10/17  01:19:36  bew
 * Branch: IosSec_branch
 * Add the simple unauthenticated public key exchange protocal. Cleaned
 * up the printing of pulic keys. Modified some show commands syntax to
 * match the functional spec. General cleanup.
 *
 * Revision 2.1.2.5  1995/10/16  04:04:20  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.2.3  1995/10/10  18:19:39  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.2.2  1995/10/03  18:51:26  xliu
 * Branch: IosSec_branch
 *
 * Moved epa dir under crypto dir. Renamed epa dir to crypto_engine dir.
 * Create crypto_lib under crypto_engine which include Cylink sw crypto
 * libs.
 *
 * Revision 2.1.2.1  1995/09/29  01:00:38  bew
 * Added session key timeouts. Also renamed epa_parser.c to
 * crypto_parser.c & changed the timestamp used in SDU code
 * to be the seconds from Day 0 rather than the contents of
 * msclock.
 *
 * Branch: IosSec_branch
 *
 * Revision 2.1  1995/09/29  00:46:10  bew
 * Placeholders for crypto files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
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
#include "parser.h"
#include "interface_private.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ip/ip.h" 
#include "crypto.h"
#include "crypto_public.h"
#include "crypto_engine_api.h"
#include "crypto_glue_private.h"
#include "../crypto/parser_defs_crypto.h"
#include "crypto_map.h"
#include "crypto_sesmgmt.h"
#include "crypto_public.h"
#include "crypto_ip.h"
#include "crypto_pubkey.h"
#include "../os/list.h"
#include "sched.h"
#include "crypto_conntable.h"
#include "crypto_glue.h"
#include "crypto_util.h"
#include "../parser/parser_actions.h"
#include "ttysrv.h"
#include "../if/network.h"
#include "../crypto/crypto_registry.h"
#include "../ui/command.h"

ushort crypto_sdu_connections = DEF_CNODES;
ushort crypto_sdu_entities = DEF_PNODES;

extern short pconn_used;
extern pconn_entry pconn_list[PCONN_ENTRIES];
extern ushort epCOTnodes;
extern PETnode *pet;

#define PCONN_ENTRIES           182
#define MAXLEN			 80  /* command buffer length */

typedef
struct epa_parser_block_ {
    watched_boolean *done_bool;
    ushort  op_status;
    union {
  	epa_key_obj       keyinfo;
	epa_rtn_sig_key   authkeyinfo;
        epa_rtn_get_status card_status_info;
    } reply_data;
} epa_parser_block;

/*
 * Populate a buffer with the results a parser call and mark it as being 
 * populated. Assume that the argument (block) is the address passed to the API
 * as proc_data_buffer.
 */
void
epa_parser_callback (
    epa_callback_context *context,
    ushort event_name,
    ushort epa_op_status,
    void *epa_rsp_buffer,
    ushort epa_rsp_buffer_size,
    struct epa_pak_info_ *epa_pak_info)
{
    epa_parser_block *block;

    block = context->proc_data_buffer;

    switch (event_name) {

    case CRYPTO_INIT:
	process_set_boolean(block->done_bool, TRUE);
	break;

    case CRYPTO_GEN_DSS_KEY_PAIR:
    case CRYPTO_GET_ENGINE_INFO:
    case CRYPTO_CONFIG_EPA:
    case CRYPTO_ZEROIZE:
	/*
	 * Copy a structure from the callback response buffer to the block 
	 * structure.
	 */
	memcpy((char *)&(block->reply_data), 
	       (uchar *)epa_rsp_buffer, epa_rsp_buffer_size);
	process_set_boolean(block->done_bool, TRUE);
	break;

    default:
	/*
	 * Nothing specific to do for these events.
	 */
	break;


    }

    block->op_status = epa_op_status;

}

void
crypto_show_pconn(parseinfo *csb)
{
    short i = pconn_used;

    if (i == NULL_INDEX) {
	return;
    } 

    printf("Pending Connection Table\n");
    printf("PE             UPE            Timestamp\tConn_id\n");
    while (i != NULL_INDEX) {
	printf("%15i%15i%d\t%d\n", pconn_list[i].pe.adr,
		pconn_list[i].upe.adr, pconn_list[i].timestamp,
		pconn_list[i].cib->crypto.cisco.connection_id);
	i = pconn_list[i].next;
    }
    printf("\n");
}

void
crypto_show_cib_entry(CIBdef *cib)
{
    uint bit;
    char *ptr;

    if (cib) {
	printf("%9d%8d%5d%u",
		cib->crypto.cisco.connection_id, 
		cib->crypto.cisco.new_connection_id,
		cib->crypto.method,
		cib->crypto.time);
    	if (NO_EXTENDED_SLOTS_ON_CHASSIS) {
	    printf("\n");
    	} else {
	    printf("  %4d\n", cib->crypto.cisco.slot);
    	}
	printf("               flags:");
	if (cib->status == 0) {
	    printf("<EMPTY>\n");
	} else {
	    for (bit=0; bit<32; bit++) {
		if (cib->status & (1<<bit)) {
		    ptr = crypto_xlate_cib_status(1<<bit);
		    if (ptr[0] != 0x0) {
		        printf("%s ", ptr);
		    }
		}
	    }
	    printf("\n");
	}
    } else {
	printf("cib pointer is NULL (not yet updated).\n");
    }
}

void
crypto_show_pet(parseinfo *csb)
{
    ushort i, j, cur;
    short page;

    printf("Connection Table\n");
    printf("PE             UPE            Conn_id  New_id  Alg  Time");
    if (NO_EXTENDED_SLOTS_ON_CHASSIS) {
	printf("\n");
    } else {
	printf("  Slot\n");
    }
    for (i = 0; i < epPEThash; i++) {
	if (pet_hash[i] != 0) { /* found a hash list */
	    cur = pet_hash[i];
	    while (cur != 0) {
		page = pet[cur].page;
		for (j = 1; j < epCOTnodes; j++) {
		    if (cot[page].node[j].id.adr != 0) {
		        printf("%15i%15i", pet[cur].id.adr,
					   cot[page].node[j].id.adr);
		        crypto_show_cib_entry(&cot[page].node[j].cib);
		    }
		}
    printf("\n");
		cur = pet[cur].hash.next;
	    }
	}
    }
}
void crypto_zeroize (parseinfo *csb)
{
    ushort slot;
    crypto_mykey *key;
    static epa_parser_block block;
    epa_callback_context context;
    ushort status;

    /*
     * Find the crypto engine specified by the user.
     */
    if (GETOBJ(int,2) == CRYPTO_INVALID_SLOT) {
	slot = crypto_engine_rpsp_slot;
    } else {
	slot = GETOBJ(int,2);
    }

    /* 
     * Find the DSS key pair & delete from the configuration. 
     */
    key = crypto_pubkey_get_keypair_by_slot(slot);
    if (!key) {
	printf("%% DSS Keys not found in configuration.\n");
        if (crypto_get_engine_id(slot) == crypto_engine_rpsp_slot) {
            return;
        }
    } else {
        /* 
         * Confirm:     
         */
        printf("Warning! Zeroize will remove your DSS signature keys."); 
        if (!yes_or_no("Do you want to continue", FALSE, FALSE)) {
	    return;
        }

        printf("%% Keys to be removed are named %s.", key->name);
        if (!yes_or_no("Do you really want to remove these keys", 
            FALSE, FALSE)) {
            return;
        }
        crypto_pubkey_keypair_delete(key);
    }

    /*
     * Clear crypto_engine_tbl
     */
    if (crypto_engine_tbl[slot].name) {
        free(crypto_engine_tbl[slot].name);
        crypto_engine_tbl[slot].name = NULL;
    }
    crypto_engine_tbl[slot].state = CRYPTO_ENGINE_INSTALLED;
    
    /* 
     * Call the crypto engine zeroize routine 
     * If this is the RP/RSP, then call epa_zeroize() without blocking, and
     * we're done. Otherwise, block on the VIP.
     */
    if (crypto_get_engine_id(slot) == crypto_engine_rpsp_slot) {
        if (epa_zeroize(crypto_engine_rpsp_slot, NULL, NULL) != EPA_OP_OK) {
            printf("%% Zeroize failed.\n");
        } else {
            printf("%% Zeroize done.\n");
        }
        return;
    } 

    /*
     * Need to send command to vip. Setup for blocking.
     */
    context.req_timeout = CRYPTO_KEY_COMM_TIMEOUT; 
    context.proc_data_buffer = &block; 
    block.done_bool = create_watched_boolean("Crypto Zeroize", 0);
    process_set_boolean(block.done_bool, FALSE);
    process_watch_boolean(block.done_bool, ENABLE, ONE_SHOT);
    
    if (epa_zeroize(crypto_get_engine_id(slot), &context,
 	            epa_parser_callback) != EPA_OP_OK) {
    	printf("%% Zeroize failed:%s\n", crypto_xlate_status(block.op_status));
    	delete_watched_boolean(&block.done_bool);
     	return;
     }

    /*
     * Wait for crypto engine.
     */
    process_wait_for_event();
    delete_watched_boolean(&block.done_bool);

    if (block.op_status != EPA_OP_OK) {
	printf("%% Error in zerozing:%s\n",
		   crypto_xlate_status(block.op_status));
	return;
    }

    /* 
     * Fix crypto engine status vector
     */
    if (crypto_engine_tbl[slot].crypto_unit_type == CRYPTO_HWEPA) {
        status = crypto_set_card_status(slot, 
      			     block.reply_data.card_status_info.status);
        if (status != EPA_OP_OK) {
            printf("%% Setting card status failed:%s\n",
	            crypto_xlate_status(block.op_status));
        }
    }

    /*
     * Update epa status 
     */
    printf("[OK]\n");
}

void crypto_sigkeys(parseinfo *csb)
{
    epa_alg_obj algorithm;
    static epa_parser_block block;
    epa_callback_context context;
    crypto_mykey *key;
    ushort slot;
    boolean password_status;
    epa_status status;
    uchar *password_buffer, *password_confirm_buffer;
    int name_len;

    /*
     * Check for 'no crypto gen-signature-keys name', which is invalid.
     */
    if (!csb->sense) {
	printf("%% Use 'crypto zeroize' to delete DSS keys.\n");
	return;
    }

    if (GETOBJ(int,2) == CRYPTO_INVALID_SLOT) {
	slot = crypto_engine_rpsp_slot;
    } else {
    	slot = GETOBJ(int,2);
	/*
	 * Verify that this slot has a VIP
	 */
	if ((slot != crypto_engine_rpsp_slot) &&
	    (reg_invoke_get_slot_serial_number(slot) == 0x0)) {
	    printf("%% Slot %d does not contain a crypto engine.\n",
		   slot);
	    return;
	}
    }

    /*
     * If there are keys already defined, print a warning message.
     */
    key = crypto_pubkey_get_keypair_by_slot(slot);
    if (key) {
        printf("%% Generating new DSS keys will require re-exchanging\n");
	printf("   public keys with peers who already have the public key\n");
	printf("   named %s!", key->name);
        if (!yes_or_no("Generate new DSS keys", FALSE, FALSE)) {
	    return;
	}
    }

    /*
     * Setup context for callbacks.
     */
    context.src_epa_id = 0;				/* local */
    context.dst_epa_id = 0;				/* local */
    context.dst_callback_handle_name = NULL;	/* local */
    context.req_timeout = CRYPTO_KEY_COMM_TIMEOUT;
    context.pak = NULL;		/* not used for these ops */
    context.proc_data_buffer = &block; 
    block.done_bool = create_watched_boolean("Crypto Config SigKeys", 0);
    process_set_boolean(block.done_bool, FALSE);
    process_watch_boolean(block.done_bool, ENABLE, RECURRING);

    /*
     * If crypto card h/w, check for existing password. Otherwise define one.
     * Ensure the status is correct first!
     */
    if (crypto_engine_tbl[slot].crypto_unit_type == CRYPTO_HWEPA) {
	password_buffer = malloc(MAXLEN);
	if (!password_buffer ) {
	    printf(nomemory);
	    delete_watched_boolean(&block.done_bool);
	    return;
	}
	if (!EPA_PASSWD_SET(crypto_engine_tbl[slot].epa_status)) {
	    printf("%% Initialize the crypto card password. You will need\n");
	    printf("   this password in order to generate new signature\n");
	    printf("   keys or clear the crypto card extraction latch.\n");
	} else {
	    printf("%% Enter the crypto card password.");
	}
	password_status = askstring(stdio, password_buffer, MAXLEN,"Password: ",
			   30 * ONESEC, 3, TRUE);
	if (!password_status) { /* error during entry */
	    printf("%% Aborted.\n");
	    free(password_buffer);
	    delete_watched_boolean(&block.done_bool);
	    return;
	}

	/*
	 * If the password is being set for the fist time, confirm the
	 * password to defeat typos. Also, config the crypto card for
	 * use.
	 */
	if (!EPA_PASSWD_SET(crypto_engine_tbl[slot].epa_status)) {
	    password_confirm_buffer = malloc(MAXLEN);
	    if (!password_confirm_buffer ) {
	        printf(nomemory);
		free(password_buffer);
	        delete_watched_boolean(&block.done_bool);
	        return;
	    }
	    password_status = askstring(stdio, password_confirm_buffer, MAXLEN, 
		    	       "Re-enter password: ", 30 * ONESEC, 3, TRUE);
	    printf("\n");
	    if (!password_status) { /* error during entry */
		printf("%% Aborted.\n");
		free(password_buffer);
		free(password_confirm_buffer);
	        delete_watched_boolean(&block.done_bool);
		return;
	    }
	    if (strcmp(password_buffer, password_confirm_buffer)) { /* ! 0 */
		printf("%% Passwords did not match. Aborted.\n");
		free(password_buffer);
		free(password_confirm_buffer);
	        delete_watched_boolean(&block.done_bool);
		return;
	    }
	    free(password_confirm_buffer); /* Done with confirmation. */ 

	    /*
	     * Config the crypto card for first use.
	     */
	    status = crypto_config_epa_card(crypto_get_engine_id(slot), 
				       password_buffer, strlen(password_buffer),
				       &context, epa_parser_callback);
	    if (status != EPA_OP_OK) {
		printf("%% Attempt to config crypto card failed:%s\n",
		   crypto_xlate_status(status));
	        free(password_buffer);
	    }
	    /*
	     * Wait for crypto engine.
	     */
	    process_wait_for_event();
	    if (block.op_status != EPA_OP_OK) {
		printf("%% Error in configuring crypto card :%s\n",
	   	    crypto_xlate_status(block.op_status));
		free(password_buffer);
	    	delete_watched_boolean(&block.done_bool);
		return;
	    }
    	    process_set_boolean(block.done_bool, FALSE);
	}
    } else {
	password_buffer = NULL; }

    printf("Generating DSS keys ....\n");
    flush();
    
    if ((key = malloc(sizeof(crypto_mykey))) == NULL) {
	printf(nomemory);
	delete_watched_boolean(&block.done_bool);
        return;
    }

    /*
     * Setup key generation paramaaters & do it.
     */
    algorithm.algorithm_type = DSS_KEY_GEN; /* Only support DSS for now */
    status = epa_generate_key_pair(crypto_get_engine_id(slot), &algorithm, 
			      password_buffer, strlen(password_buffer),
			      &context, epa_parser_callback);
    if (status != EPA_OP_OK) {
	printf("%% Attempt to generate keys failed:%s\n",
		   crypto_xlate_status(status));
	free(key);
	delete_watched_boolean(&block.done_bool);
	return;
    }

    /*
     * Wait for crypto engine.
     */
    process_wait_for_event();
    delete_watched_boolean(&block.done_bool);

    /*
     * Expect epa_rtn_public_key returned in block
     */
    if (block.op_status != EPA_OP_OK) {
	printf("%% Error in generating keys:%s\n",
		   crypto_xlate_status(block.op_status));
	free(key);
	return;
    }

    /* 
     * Fix crypto engine status vector
     */
    if (crypto_engine_tbl[slot].crypto_unit_type == CRYPTO_HWEPA) {
    	status = crypto_set_card_status(slot, 
					block.reply_data.authkeyinfo.status);
    	if (status != EPA_OP_OK) {
	    printf("%% Setting card status failed:%s\n",
		   crypto_xlate_status(block.op_status));
	}
    }

    /*
     * Store the public & private keys in the configuration.
     */
    key->slot = slot;
    name_len = strlen(GETOBJ(string,1))+1;
    key->name = malloc(name_len);
    if (key->name == NULL) {
    	printf(nomemory);
    	free(key);
    	return;
    }
    sstrncpy(key->name, GETOBJ(string,1), name_len);

    key->serial = 0x0; 
    if ( slot != crypto_engine_rpsp_slot ) {
        if (crypto_engine_tbl[slot].crypto_unit_type == CRYPTO_HWEPA) {
            key->serial = block.reply_data.authkeyinfo.serial_number;
	}
	else 
             key->serial = reg_invoke_get_slot_serial_number(slot); 
	/* 
	 * key->serial would be 0x0 if the specified slot is not a VIP
	 * in which case we default to using the RSP serial number.
	 */
	if ( key->serial == 0x0) {
    	    printf("%% Error: Slot %d doesn't contain a VIP or a RSP.\n",slot); 
    	    crypto_pubkey_keypair_freekey(key);
    	    return;
	}
    } else {
        key->serial = crypto_processor_serial;
    }

    /*
     * Update crypto_engine_tbl with the new name.
     */
    if (!crypto_engine_tbl[key->slot].name) {
        crypto_engine_tbl[key->slot].name = malloc(strlen(key->name)+1);
        if (crypto_engine_tbl[key->slot].name) {
            strncpy(crypto_engine_tbl[key->slot].name, key->name, 
                    strlen(key->name)+1);
        }
    }
    crypto_engine_tbl[key->slot].state = CRYPTO_ENGINE_DSS_KEY_GEN;

    /*
     * Get the keys returned from the crypto engine and add to the config.
     */
    if (block.reply_data.authkeyinfo.alg_type != CRYPTO_GEN_KEY_PAIR) {
	printf("%% Error: Wrong authentication key type returned\n");
    	crypto_pubkey_keypair_freekey(key);
    	return;
    }
    key->private_key.keylen =
    	block.reply_data.authkeyinfo.key_tag.dss_keys.pri_key_size;
    memcpy(&(key->private_key.key), 
    	   block.reply_data.authkeyinfo.key_tag.dss_keys.pri_key,
    	   key->private_key.keylen);
    key->public_key.keylen =
    	block.reply_data.authkeyinfo.key_tag.dss_keys.pub_key_size;
    memcpy(&(key->public_key.key), 
    	   block.reply_data.authkeyinfo.key_tag.dss_keys.pub_key,
    	   key->public_key.keylen);

    if (!crypto_pubkey_keypair_add(key)) {
    	printf("%% Error: Addition of keys to config failed.\n");
    	crypto_pubkey_keypair_freekey(key);
    }
    printf(" [OK]\n");

    free(password_buffer);

    return;
}

void crypto_show_alg(parseinfo *csb)
{
    if (crypto_is_alg_set(CRYPTO_DES_56)) {
        printf("  %s %s\n", CRYPTO_DES_56_STR, CRYPTO_CFB64_STR);
    }
    if (crypto_is_alg_set(CRYPTO_DES_56_8)) {
        printf("  %s %s\n", CRYPTO_DES_56_STR, CRYPTO_CFB8_STR);
    }
    if (crypto_is_alg_set(CRYPTO_DES_40)) {
        printf("  %s %s\n", CRYPTO_DES_40_STR, CRYPTO_CFB64_STR);
    }
    if (crypto_is_alg_set(CRYPTO_DES_40_8)) {
        printf("  %s %s\n", CRYPTO_DES_40_STR, CRYPTO_CFB8_STR);
    }
}

void crypto_alg_set(parseinfo *csb)
{
    /*
     * Nvgen if the value is not the default. 
     */
    if (csb->nvgen) {
	if (crypto_get_alg_vector() != crypto_get_default_algorithm()) {
	    nv_write(TRUE, "!");
	    if (crypto_is_alg_set(CRYPTO_DES_56)) {
	        nv_write(TRUE, "%s %s", csb->nv_command, CRYPTO_DES_56_STR);
	    }
	    if (crypto_is_alg_set(CRYPTO_DES_56_8)) {
	        nv_write(TRUE, "%s %s %s", csb->nv_command, CRYPTO_DES_56_STR,
	    	     CRYPTO_CFB8_STR);
	    }
	    if (crypto_is_alg_set(CRYPTO_DES_40)) {
	        nv_write(TRUE, "%s %s", csb->nv_command, CRYPTO_DES_40_STR);
	    }
	    if (crypto_is_alg_set(CRYPTO_DES_40_8)) {
	        nv_write(TRUE, "%s %s %s", csb->nv_command, CRYPTO_DES_40_STR,
	    	     CRYPTO_CFB8_STR);
	    }
	}
	return;
    }

    if (!csb->sense) {
        crypto_set_alg_vector(GETOBJ(int,2), FALSE);
    } else {
	crypto_set_alg_vector(GETOBJ(int,2), TRUE);
    }

    return;
}

void crypto_set_session_key_timeout (parseinfo *csb)
{
    ushort timeout;

    /*
     * Nvgen if the value is not the default
     */
    if (csb->nvgen) {
	timeout = crypto_glue_get_session_key_timeout();
	if (timeout != CRYPTO_DEF_MAXKEYAGE) {
	    nv_write(TRUE, "!");
	    nv_write(TRUE, "crypto key-timeout %d", timeout);
	} 
	return;
    }

    if (csb->sense) {
    	crypto_glue_set_session_key_timeout((ushort)GETOBJ(int,2));
    } else {
    	crypto_glue_set_session_key_timeout(CRYPTO_DEF_MAXKEYAGE);
    }
}

void crypto_card_cfg_comm (ushort slot)
{
    reg_invoke_crypto_card_cfg_comm(slot);
}

void
crypto_init_epa (parseinfo *csb)
{

    /*
     * For now, epa_init is a synchronous call. It will probably become
     * asynchronous again when the viper EPA's are supported.
     */
    
    epa_init(crypto_engine_rpsp_slot, NULL, NULL);

#if ASYNC
    epa_callback_context *context;
    epa_parser_block *block;

    if ((context = malloc(sizeof(epa_callback_context))) == NULL) {
	return;
    }

    if ((block = malloc(sizeof(epa_parser_block))) == NULL) {
	free(context);
	return;
    }

    /*
     * Setup context.
     */
    context->req_timeout = CRYPTO_KEY_COMM_TIMEOUT;	
    context->proc_data_buffer = block; 
    
    /*
     * Initialize the watched boolean.
     */
    block->done_bool = create_watched_boolean("Crypto EPA Init", 0);
    process_set_boolean(block->done_bool, FALSE);

    /*
     * Do it.
     */
    if (epa_init(crypto_engine_rpsp_slot, context, epa_parser_callback) 
    	     == EPA_OP_OK) {
        process_watch_boolean(block->done_bool, ENABLE, ONE_SHOT);
	process_wait_for_event();
	printf("EPA status:%s\n", crypto_xlate_status(block->op_status));
    } else {
	printf("ERROR: EPA failed -- error returned:%s\n",
	       crypto_xlate_status(block->op_status));
    }

    /*
     * Cleanup
     */
    delete_watched_boolean(&block->done_bool);
    free(context);
    free(block);
#endif /* ASYNC */

    return;
}

/*
 * Process crypto configuration commands
 */
void crypto_command (parseinfo *csb)
{
    ushort slot, num;

    switch (GETOBJ(int,1)) {
    case PARSER_CRYPTO_SIGKEYS:
	crypto_sigkeys(csb);
	break;
    case PARSER_CRYPTO_PUBKEY:
	crypto_pubkey_cmd(csb);
	break;
    case PARSER_CRYPTO_DSS_KEYPAIR:
	crypto_pubkey_keypair_cmd(csb);
	break;
    case PARSER_CRYPTO_INIT_EPA:
	crypto_init_epa(csb);
	break;
    case PARSER_CRYPTO_ALG_SET:
	crypto_alg_set(csb);
	break;
    case PARSER_CRYPTO_KEY_TIMEOUT:
	crypto_set_session_key_timeout(csb);
	break;
    case PARSER_CRYPTO_KEY_EXCHANGE:
	crypto_exchange_ip(GETOBJ(paddr,1), GETOBJ(string,1),
			   GETOBJ(int,2));
	break;
    case PARSER_CRYPTO_KEY_EXCHANGE_PASSIVE:
	crypto_exchange_passive_ip(GETOBJ(int,2));
	break;
    case PARSER_CRYPTO_ZEROIZE:
	crypto_zeroize(csb);
	break;
    case PARSER_CRYPTO_PREGEN_DH:
	if (csb->nvgen) {
	    if (crypto_dh_pregen_count_array) {
	       for (slot=0; slot <crypto_number_of_slots; slot++) {
		   if (crypto_dh_pregen_count_array[slot]) {
			nv_write(TRUE, "!");
			nv_write(TRUE, "%s %d", csb->nv_command,
			 	 crypto_dh_pregen_count_array[slot]);
			if (crypto_number_of_slots > 1) {
			    nv_add(TRUE, " %d", slot);
			}
		    }
		}
	    }
	    break;
	} 
	if (csb->sense) {
	    /*
	     * Make sure the array has been initialized before using.
	     */
	    if (!crypto_dh_pregen_count_array) {
		num = crypto_get_number_of_slots();
		crypto_dh_pregen_count_array = 
	      	    malloc(num*sizeof(crypto_dh_pregen_count_array));
	        if (!crypto_dh_pregen_count_array) {
		    printf(nomemory);
		    break;
		}
	    }
	    /*
	     * Set the number to pregenerate for the given slot.
	     */
	    if (GETOBJ(int,3) == CRYPTO_INVALID_SLOT) {
		slot = crypto_engine_rpsp_slot;
	    } else {
		slot = GETOBJ(int,3);
	    }
	    crypto_dh_pregen_count_array[slot] = (ushort) GETOBJ(int, 2);
	} else {
	    /*
	     * Clear out all slots
	     */
	    free(crypto_dh_pregen_count_array);
	    crypto_dh_pregen_count_array = (ushort *) NULL;
	}
	break;

    case PARSER_CRYPTO_CARD_LATCH:
        crypto_card_cfg_comm(GETOBJ(int, 2));
        break;
    case PARSER_CRYPTO_SDU_PET_SIZE:
        if (csb->nvgen) {
            nv_write((crypto_sdu_entities != DEF_PNODES), 
                      "%s %d", csb->nv_command, crypto_sdu_entities);
            break;
            }
        if (!csb->sense) {
           crypto_sdu_entities = DEF_PNODES;
        } else {
           crypto_sdu_entities = GETOBJ(int, 2);
        } 
    	if (csb->resolvemethod == RES_MANUAL) {
	    /* Only print the message for a "conf terminal" */
            printf("%% Please reboot for the new table size to take effect.\n");
	}
        break;
    case PARSER_CRYPTO_SDU_COT_SIZE:
        if (csb->nvgen) {
            nv_write(TRUE, "!");
            nv_write((crypto_sdu_connections != DEF_CNODES), 
                      "%s %d", csb->nv_command, crypto_sdu_connections);
            break;
        }
        if (!csb->sense) {
            crypto_sdu_connections = DEF_CNODES;
        } else {
            crypto_sdu_connections = GETOBJ(int, 2);
        } 
        if (csb->resolvemethod == RES_MANUAL) {
            /* Only print the message for a "conf terminal" */
            printf("%% Please reboot for the new connection size to take effect.\n");
        }
        break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

void crypto_show_info(parseinfo *csb, ushort slot)
{
   /* Display crypto engine configuration info. */
}

/*
 * Process crypto show commands
 */
void crypto_show_command (parseinfo *csb)
{
    int timeout;
    ushort slot;
    int count;
    crypto_dh_pregen *item;

    switch (GETOBJ(int,1)) {
    case PARSER_CRYPTO_GETINFO:
	crypto_show_info(csb, 0);
	break;
    case PARSER_CRYPTO_GETINFO_ALL:
	for (count=0; count < crypto_number_of_slots; count++) {
	    crypto_show_info(csb, count);
	}
	break;
    case PARSER_CRYPTO_GETPUBKEY_NAME:
	crypto_show_pubkey_by_name(csb);
	break;
    case PARSER_CRYPTO_GETPUBKEY_SERIAL:
	crypto_show_pubkey_by_serial(csb);
	break;
    case PARSER_CRYPTO_GETPUBKEY_ALL:
	crypto_show_pubkey_all(csb);
	break;
    case PARSER_CRYPTO_GETPUBKEY_RP:
    case PARSER_CRYPTO_GETPUBKEY_RSP:
	crypto_show_mypubkey(crypto_processor_serial);
	break;
    case PARSER_CRYPTO_GETPUBKEY_ALLMY:
	crypto_show_mypubkey_all(csb);
	break;
    case PARSER_CRYPTO_GET_ALG:
	crypto_show_alg(csb);
	break;
    case PARSER_CRYPTO_ENGINE_ACT_CONN:
	slot = GETOBJ(int,2);
        if (slot == 0) {
	    crypto_engine_show_active_conn(crypto_engine_rpsp_slot);
        } else {
            reg_invoke_crypto_show_vip_conn_tbl (slot);
        }
	break;
    case PARSER_CRYPTO_GET_CONN:
	crypto_show_pconn(csb);
	crypto_show_pet(csb);
	break;
    case PARSER_CRYPTO_GET_KEY_TIMEOUT:
	timeout = crypto_glue_get_session_key_timeout();
	printf("Session keys will be re-negotiated every %d minute%s\n",
	       timeout, timeout == 1 ? "" : "s");
	break;
    case PARSER_CRYPTO_GET_PREGEN:
	printf("Number of pregenerated DH pairs");
        if (NO_EXTENDED_SLOTS_ON_CHASSIS) {
	    slot = crypto_engine_rpsp_slot;
    	} else {
	    if (GETOBJ(int,2) == CRYPTO_INVALID_SLOT) {
	    	slot = crypto_engine_rpsp_slot;
	    } else {
	    	slot = GETOBJ(int,2);
	    }
	    printf(" for slot %d", slot);
    	}
	count = 0;
	item = crypto_dh_pregen_head_array[slot];
	while (item) {
	    count++;
	    item=item->next;
	}
	printf(": %d\n", count);
	break;
    case PARSER_CRYPTO_ENGINE_CONNTBL:
	crypto_engine_show_conntbl(crypto_engine_rpsp_slot, GETOBJ(int,2),
				   GETOBJ(int, 3));
	break;

    case PARSER_CRYPTO_ENGINE_CONN_ALL:
        break;

    case PARSER_CRYPTO_ENGINE_DROP_PAK:
	crypto_engine_show_drop_pak();
	break;
    case PARSER_CRYPTO_ENGINE_RP_CONFIG:
	crypto_engine_show_config();
	break;
    case PARSER_CRYPTO_ENGINE_RSP_CONFIG:
	crypto_engine_show_config();
	break;
    case PARSER_CRYPTO_ENGINES:
        crypto_engine_show_all();
        break;
    case PARSER_CRYPTO_CARD:
        crypto_show_card_status(GETOBJ(int, 2));
        break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    } 
}
/*
 * Process crypto clear commands
 */
void crypto_clear_command (parseinfo *csb)
{
    ushort slot;

    switch (GETOBJ(int,1)) {
    case PARSER_CRYPTO_CLEAR_CONN:
    	if (GETOBJ(int,3) == CRYPTO_INVALID_SLOT) {
	    slot = crypto_engine_rpsp_slot;
    	} else {
	    slot = GETOBJ(int,3);
    	}
	crypto_glue_clear_connection(GETOBJ(int,2), slot);
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    } 
}

/*
 * Initiate a connection to a peer using an existing crypto map
 */
int crypto_test_initiate(addrtype *addr, addrtype *addr2, char *map, int num)
{
    int status;
    addrtype pe, upe;
    int alg;
    char **peers;
    boolean ok;

    if ((addr->type != ADDR_IP) || (addr2->type != ADDR_IP)) {
    	return(CRYPTO_CONN_ADDRERROR);
    }
    ok=find_crypto_map_policy(map, num, &pe, &upe, &alg, &peers);
    if (!ok) {
        buginf("\nCan not find crypto map. Abort.");
	return(CRYPTO_CONN_MAPERROR);
    }

    /* 
     * Initiate the connection 
     */
    printf("Sending Crypto Connection Message to: %i from: %i.\n", 
	    addr->ip_addr, addr2->ip_addr);
    crypto_conn_entry_initiate_ip(&status, pe.ip_addr, upe.ip_addr,
				  addr2->ip_addr, addr->ip_addr, 0, alg, 
				  peers, 0);
    return(status);
}

void crypto_test_parse (parseinfo *new_csb, char *cmd)
{
    strncpy(new_csb->line, cmd, PARSEBUF-1);
    (void) parse_cmd(new_csb);
    return;
}

void crypto_test_cleanup (parseinfo *csb, ushort conn_id, ushort slot, 
			  boolean clear_connection)
{
    free_csb(&csb);
    csb = 0;
    if (clear_connection) {
    	(void) crypto_clear_connection_entry(conn_id, slot);
    }
    return;
}

/*
 * Setup crypto policy on an interface so that the user can use ping (in all 
 * its flaming glory including sweeping pings) over an interface. This is 
 * vastly easier than trying to duplicate ping.
 */
void crypto_test_ping (parseinfo *csb)
{
    static int conn_id = 9;
    static int crypto_map_statement = 0;
    static int access_list = 149;
    uchar *dh_public_number, *their_dh_public_number;
    ushort slot;
    boolean ok;
    parseinfo *new_csb;
    char line[MAXLEN];  
    char buff[MAXLEN];
    idbtype *idb;
    ipaddrtype my_ipaddr, ipaddr;
    addrtype addr;

    if (!confirm("No other encryption should be active")) {
	return;
    }
    printf("\n");

    /*
     * Prompt for interface and remote (target) address.
     */
    printf("Interface: ");
    if (!rdtty(buff, MAXLEN)) {
	return;
    }
    parse_interface(buff, &idb);
    if (!idb) {
	printf("\n%% Bad interface");
	return;
    }
    printf("Peer IP address: ");
    if (!rdtty(buff, MAXLEN) || !parse_hostname(buff, &ipaddr)) {
        printf("\n%% Bad IP address");
        return;
    }

    if (NO_EXTENDED_SLOTS_ON_CHASSIS) {
	slot = crypto_engine_rpsp_slot;
    } else {
	slot = crypto_get_engine_id(idb->hwptr->slot);
    }
    if (idb->ip_address) {
	my_ipaddr = idb->ip_address;
    } else {
	if (idb->ip_unnumbered) {
	    my_ipaddr = idb->ip_unnumbered->ip_address;
	} else {
	    printf("\n%% No IP address found for interface.\n");
	    return;
	}
    }

    /*
     * Increment conn_id for this connection.
     */
    ++conn_id;
    
    /*
     * Initialize csb, marking it as being in configure mode.
     */
    new_csb = get_csb("Crypto ping command");
    if (!new_csb) {
    	crypto_test_cleanup(new_csb, conn_id, slot, TRUE);
	return;
    }
    new_csb->priv = PRIV_ROOT;
    new_csb->mode = config_mode;
    new_csb->lastchar = ' ';
    /*
     * Clear out any existing crypto map & use of this connection id.
     */
    (void) sprintf(line, "interface %s", idb->namestring);
    crypto_test_parse(new_csb, line);
    crypto_test_parse(new_csb, "no crypto map");
    crypto_test_parse(new_csb, "exit");


    /*
     * Generate DH phase 1 (public) number.
     */
    dh_public_number = malloc(64);
    if (!dh_public_number) {
	return;
    }
    their_dh_public_number = malloc(64);
    if (!their_dh_public_number) {
	free(dh_public_number);
	return;
    }
    ok=crypto_generate_DH_parameters((short) DH_ALG_PHASE1, dh_public_number,
				     conn_id, slot);
    if (!ok) {
	return; 
    }

    /*
     * Exchange DH phase 1 (public) numbers.
     */
    if (GETOBJ(int,1) == PARSER_TEST_CRYPTO_PING) {
	addr.ip_addr = ipaddr;
	addr.type = ADDR_IP;
	ok = crypto_exchange_sessionkey_ip(&addr, dh_public_number,
					   their_dh_public_number);
    } else {
	ok = crypto_exchange_sessionkey_passive_ip(dh_public_number,
		 				   their_dh_public_number);
    }
    if (!ok) {
	printf("%% Error in establishing TCP connection.\n");
	return;
    }

    /*
     * Create the session key.
     */
    ok=crypto_generate_DH_parameters((short) DH_ALG_PHASE2, 
				     their_dh_public_number, conn_id, slot);
    if (!ok) {
	return;
    }
    ok=crypto_create_session_key(conn_id, slot, DES_CFB64_IV1);
    if (!ok) {
	return;
    }

    /*
     * Set the pre-connection values for the this connection.
     */
    crypto_map_statement += 10;
    ++access_list;

    /*
     * Generate ACL and crypto map, assign to interface
     */
    (void) sprintf(line, "no access-list %d", access_list);
    crypto_test_parse(new_csb, line);
    (void) sprintf(line, "access-list %d permit icmp host %i host %i",
		   access_list, my_ipaddr, ipaddr);
    crypto_test_parse(new_csb, line);
    (void) sprintf(line, "access-list %d permit ip host %i host %i",
		   access_list, my_ipaddr, ipaddr);
    crypto_test_parse(new_csb, line);
    (void) sprintf(line, "crypto map loopback %d", crypto_map_statement);
    crypto_test_parse(new_csb, line);
    (void) sprintf(line, "match address %d", access_list);
    crypto_test_parse(new_csb, line);
    crypto_test_parse(new_csb, "set peer foo");
    crypto_test_parse(new_csb, "exit");
    (void) sprintf(line, "interface %s", idb->namestring);
    crypto_test_parse(new_csb, line);
    crypto_test_parse(new_csb, "crypto map loopback");
    crypto_test_parse(new_csb, "exit");

    /* 
     * Setup connection id 
     */
    crypto_glue_clear_conn_id(0, conn_id, slot);

    /*
     * Leave connection open ... should maybe have another hidden command
     * to clean up when the user is done testing.
     */
    crypto_test_cleanup(new_csb, conn_id, slot, FALSE);
}

void crypto_test_epa(ushort slot)
{
    printf("\nConfig epa in slot %d", slot);
    reg_invoke_crypto_config_epa(slot, "CISCO_EPA", sizeof("CISCO_EPA"));
}

void test_crypto (parseinfo *csb)
{
    int status;

    switch (GETOBJ(int,1)) {

    case PARSER_TEST_INITIATE_SESSION:
	status = crypto_test_initiate(GETOBJ(paddr,1), GETOBJ(paddr,2), 
			GETOBJ(string,1), GETOBJ(int,2));
	switch (status) {
	case CRYPTO_CONN_MAPERROR:
	    printf("Can't find specified crypto map\n");
	    break;
	case CRYPTO_CONN_ADDRERROR:
	    printf("Non-IP address type specified for an address\n");
	    break;
	default:
	    printf("Connection id: %d\n", status);
	    break;
	}
	break;

    case PARSER_TEST_CRYPTO_PING:
    case PARSER_TEST_CRYPTO_PING_PASSIVE:
	crypto_test_ping(csb);
	break;

    case PARSER_TEST_DES_ENCRYPT:
    case PARSER_TEST_DES_DECRYPT:
	crypto_test_des_with_input(csb);
	break;

    case PARSER_TEST_NIST_CFB8:
    case PARSER_TEST_NIST_CFB64:
	crypto_nist_runtests(csb);
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    } 
}
