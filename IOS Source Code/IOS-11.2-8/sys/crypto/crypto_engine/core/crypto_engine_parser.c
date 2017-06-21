/* $Id: crypto_engine_parser.c,v 1.1.56.18 1996/09/10 09:20:45 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/core/crypto_engine_parser.c,v $
 *------------------------------------------------------------------
 * This file defines crypto engine parser command.
 *
 * Jan 1996, xliu 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_parser.c,v $
 * Revision 1.1.56.18  1996/09/10  09:20:45  xliu
 * CSCdi68603:  crypto engine parser command clean up
 * Branch: California_branch
 *
 * Revision 1.1.56.17  1996/08/28  23:50:51  xliu
 * CSCdi67265:  Add parser command to show crypto lib version number
 * Branch: California_branch
 *
 * Revision 1.1.56.16  1996/08/27  03:34:36  xliu
 * CSCdi67265:  Add parser command to show crypto lib version number
 * Branch: California_branch
 * Modfied parser command to show crypto lib version number.
 *
 * Revision 1.1.56.15  1996/08/15  01:20:56  xliu
 * CSCdi65622:  Cannnot apply crypto map to frame-relay subinterface
 * Branch: California_branch
 *
 * Revision 1.1.56.14  1996/08/07  09:21:41  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 1.1.56.13  1996/08/07  08:26:26  snijsure
 * CSCdi65248:  encryption mib doesnt show correct status of ESA.
 * Branch: California_branch
 * Retrieve ESA status using crypto_show_card_status,
 * Also assign proper value to cieEngineCardIndex, using the chassis mib
 * interface.
 *
 * Revision 1.1.56.12  1996/07/28  07:44:05  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * This fix also solve the problems reported by CSCdi63488, CSCdi63852.
 * Branch: California_branch
 *
 * Revision 1.1.56.11  1996/07/17  09:32:13  xliu
 * CSCdi63107:  Show crypto conn eng stat crashes router
 * Branch: California_branch
 *
 * Revision 1.1.56.10  1996/07/15  10:05:58  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 1.1.56.9  1996/07/06  19:44:59  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.56.8  1996/07/01  12:26:21  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.56.7  1996/06/21  22:29:54  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.56.6  1996/06/18  00:24:29  bew
 * CSCdi60362:  Crypto router choose 40-des over 56-des
 * Branch: California_branch
 * Now printing the serial number correctly in
 * show crypto engine configuration.
 *
 * Revision 1.1.56.5  1996/06/10  07:09:45  xliu
 * CSCdi59904:  Crypto command show crypto engine stat is not yet
 * functional
 * Branch: California_branch
 *
 * Revision 1.1.56.4  1996/05/28  15:59:16  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.56.3  1996/05/21  09:44:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.56.2  1996/05/12  23:12:45  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.56.1  1996/04/19  14:56:35  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.8  1996/04/17  23:47:59  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.7  1996/04/09  20:03:01  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.6  1996/03/29  18:03:05  xliu
 * Branch: IosSec_branch
 * Commit again for the head fils changes.
 *
 * Revision 1.1.2.5  1996/03/26  22:44:23  xliu
 * Branch: IosSec_branch
 * Add testing parser command.
 *
 * Revision 1.1.2.4  1996/03/25  18:43:55  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 1.1.2.3  1996/03/16  01:44:09  xliu
 * Branch: IosSec_branch
 * Modfied show command.
 *
 * Revision 1.1.2.2  1996/01/19  07:00:49  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.2.1  1996/01/04  18:33:01  xliu
 * Branch: IosSec_branch
 * Define crypto engine parser commands.
 *
 * Revision 1.1  1996/01/04  18:30:16  xliu
 * Place holder file.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "config.h"
#include "logger.h"
#include "types.h"
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../ui/debug.h"
#include "../crypto/crypto_debug.h"
#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_pubkey.h"
#include "../crypto/crypto_util.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_engine_api.h"
#include "crypto_engine.h"
#include "crypto_swepa.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"
#include "../crypto/crypto_engine_rsp_registry.h"
#include "../crypto/parser_defs_crypto.h"

static uchar *get_type_string(ushort slot);
static uchar *get_platform_string(ushort);
static uchar *get_state_string(ushort);

/*
 * Show the connection table from start to end.
 */
void crypto_engine_show_conntbl (ushort crypto_engine_id,
                                 ushort start,
                                 ushort end)
{
    ushort i = 0;
    epa_swcrypto_connection *conn;

    if (!end)  {
        end = CRYPTO_CONNTBL_SIZE;
    }

    printf("\n%5s %10s %13s %6s %10s %8s %8s\n","ID", "Interface",
           "IP-Address","State","Algorithm", "Encrypt","Decrypt");

    conn = &epa_swcrypto_conn_tbl[start];

    for (i = start; i<= end; i++, conn++) 
        if (i == CRYPTO_CONNTBL_SIZE) {
            if (crypto_engine_debug) {
                buginf ("\nCRYPTO_ENGINE: maximum connection number"
                        "  exceeded!\n");
            }
            return;
        } else {
        if (conn->state == EPA_SWCRYPTO_CONN_SET) {
            if (conn->swidb) {
                printf("%5d %10s %11i %6s %10s %8d %8d\n",conn->connection_id,
                  is_subinterface(conn->swidb)? conn->swidb->short_namestring:
                  conn->swidb->namestring,
                  conn->swidb->ip_address ? conn->swidb->ip_address:
                               conn->swidb->ip_unnumbered->ip_address,
                  conn->state == 1? "set":"unset", 
                  crypto_xlate_algorithm_name(conn->alg_type),
                  conn->encrypt_count, 
                  conn->decrypt_count);
            } else {
                printf("%5d %10s %11s %6s %10s %8d %8d\n",conn->connection_id,
                       "no traffic", "no traffic", "set",
                  crypto_xlate_algorithm_name(conn->alg_type),
                  conn->encrypt_count, 
                  conn->decrypt_count);
            }
        } else {
            printf("%5s %10s %11s %6s %8s %8s %8s\n", "unset",
                 "unset", "unset", "unset", "unset", "unset", "unset");
        }
    }
}

void crypto_engine_show_active_conn (ushort crypto_engine_id)
{
    ushort i;
    epa_swcrypto_connection *conn;

    printf("\n%5s %15s %11s %6s %15s %8s %8s\n","ID", "Interface",
           "IP-Address","State","Algorithm", "Encrypt","Decrypt");

    conn = &epa_swcrypto_conn_tbl[0];

    for (i = 0; i<= CRYPTO_CONNTBL_SIZE; i++, conn++) 
        if (conn->state == EPA_SWCRYPTO_CONN_SET) {
            if (conn->swidb) {
                printf(" %5d", conn->connection_id);
                if (conn->swidb->ip_address || conn->swidb->ip_unnumbered) {
                    printf("%15s %11i", conn->swidb->namestring,
                        conn->swidb->ip_address ? conn->swidb->ip_address:
                                    conn->swidb->ip_unnumbered->ip_address);
                } else { 
                    printf("%15s %11s", conn->swidb->namestring, "unassigned");
                }
                printf(" %6s %15s %8d %8d\n", "set",
                  crypto_xlate_algorithm_name(conn->alg_type),
                  conn->encrypt_count, 
                  conn->decrypt_count);
            } 
        }
}

void crypto_engine_show_dead_conn (ushort crypto_engine_id)
{
    ushort i;
    epa_swcrypto_connection *conn;

    printf("\n%10s %10s %13s %5s %8s %8s %8s\n","Connection", "Interface",
           "IP-Address","State","Algorithm", "Encrypt","Decrypt");

    conn = &epa_swcrypto_conn_tbl[0];

    for (i = 0; i<= CRYPTO_CONNTBL_SIZE; i++, conn++) 
        if (conn->state == EPA_SWCRYPTO_CONN_CLEAR) {
	    printf("%10d %10s %13is%5s %8s %8s %8s\n", 
                   conn->connection_id, "unset", "unset", "clear", "unset", 
                   "unset", "unset");
        }
}

void crypto_engine_show_config (void)
{
    ushort id;

    id = crypto_engine_rpsp_slot;

    printf("\n");
    printf("slot:               %d\n", id);
    if (!crypto_engine_tbl[id].name) {
        printf("engine name:        %s\n", "unknown");
    } else {
        printf("engine name:        %s\n", crypto_engine_tbl[id].name);
    }
    printf("engine type:        %s\n",   get_type_string(id));
    printf("serial number:      %08x\n", crypto_engine_tbl[id].serial);
    printf("platform:           %s\n",  get_platform_string(id));
    printf("crypto lib version: %s\n", crypto_get_tk_version());

    if (crypto_engine_tbl[id].crypto_unit_type == CRYPTO_SWEPA) {
        printf("\n");
        printf("Encryption Process Info: \n"); 
        printf("input queue top:    %d\n", crypto_engine_idb.encrypt_que_top);
        printf("input queue bot:    %d\n", crypto_engine_idb.encrypt_que_bot);
        printf("input queue count:  %d\n", 
                                  crypto_engine_idb.encrypt_que_count);
    }
}

void crypto_engine_show_drop_pak (void)
{
    idbtype *idb = NULL;
    crypto_drop_pak_t *list_element;
    ipaddrtype ip_address;

    printf("\n%20s %13s %15s\n","Interface",
           "IP-Address","Drop Count");

    list_element = crypto_dropped_packets_list;
    while (list_element) {
       if (list_element->hwidb) {
           idb = list_element->hwidb->firstsw;
	   if (idb) {
		if (idb->ip_address) {
		    ip_address = idb->ip_address;
		} else if (idb->ip_unnumbered) {
		    ip_address = idb->ip_unnumbered->ip_address;
		} else {
		    ip_address = 0;
		}
           	printf("\n%20s %13i %ld", 
           	    is_subinterface(idb)? idb->short_namestring:idb->namestring,
		    ip_address, list_element->count);
	   }
       }
       list_element = list_element->next;
    }
}

static uchar *get_platform_string(ushort slot)
{
    if (crypto_engine_tbl[slot].platform == CRYPTO_PLATFORM_RP)
        return "rp crypto engine";
    if (crypto_engine_tbl[slot].platform == CRYPTO_PLATFORM_RSP)
        return "rsp crypto engine";
    if (crypto_engine_tbl[slot].platform == CRYPTO_PLATFORM_VIP)
        return "vip crypto engine";
    return NULL;
}

static uchar *get_type_string(ushort slot)
{
    if (crypto_engine_tbl[slot].id == CRYPTO_ENGINE_NOT_PRESENT)
        return "none";

    if (crypto_engine_tbl[slot].crypto_unit_type == CRYPTO_HWEPA)
        return "ESA";
    else
        return "software";
}

static uchar *get_state_string(ushort slot)
{
    switch (crypto_engine_tbl[slot].state) {
    case CRYPTO_ENGINE_INSTALLED:
        return "installed";
    case CRYPTO_ENGINE_DSS_KEY_GEN:
        return "dss key generated";
    default:
        return "not installed";
    }
}

void crypto_engine_show_all (void)
{
    ushort i;
    uchar *name;

    for (i=0; i<CRYPTO_MAX_SLOTS; i++) {

        /*
         * Only display the crypto engine info from the slot
         * where the crypto engine is installed.
         */ 
        if (crypto_engine_tbl[i].state != CRYPTO_ENGINE_NOT_INSTALLED &&
            crypto_engine_tbl[i].id == i) {

            printf("\n"); 
            name = crypto_get_key_name_by_slot(i);
            if (!name) {
                crypto_engine_tbl[i].name = malloc(strlen("unknown")+1);
                if (crypto_engine_tbl[i].name == NULL) {
                    printf("ERROR: can not malloc memory for name");
                    return;
                }
                strncpy(crypto_engine_tbl[i].name, "unknown", 
                        strlen("unknown") + 1);
            } else {
                crypto_engine_tbl[i].name = malloc(strlen(name)+1);
                if (crypto_engine_tbl[i].name == NULL) {
                    printf("ERROR: can not malloc memory for name");
                    return;
                }
                strncpy(crypto_engine_tbl[i].name, name, strlen(name)+1);
            }
            printf("crypto engine name:  %s\n", crypto_engine_tbl[i].name);
            printf("crypto engine type:  %s\n", get_type_string(i));
            printf("crypto engine state: %s\n", get_state_string(i));
            if (crypto_engine_tbl[i].crypto_unit_type == CRYPTO_SWEPA) {
                printf("crypto lib version: %s\n",
                       crypto_get_tk_version());
            }
        }
    }
} 

void crypto_init_vip_dss_keys (ushort slot) 
{
    epa_crypto_blk *blk;

    blk = malloc (sizeof(epa_crypto_blk));
    if (!blk) {
        return;
    }

    blk->header.command_id = CRYPTO_INIT;
    bcopy(crypto_pri_key, blk->data, SHA_LENGTH);
    bcopy(crypto_pub_key, blk->data+SHA_LENGTH,DSS_LENGTH_MIN);
    blk->header.data_size = DSS_LENGTH_MIN + SHA_LENGTH;
    
    reg_invoke_crypto_engine_req_remote_dspt (slot, (void *)blk);
}

void crypto_engine_command (parseinfo *csb)
{
/* Need to add config commnad here. */    

}

void crypto_show_card_status (ushort slot)
{
    reg_invoke_crypto_show_card_status(slot,TRUE);
}

#if 0
/*
 * Test DES implementation according to FIB[] doc.
 * Retained for historical purposes.
 */
void crypto_test_des (ushort mode) 
{
    uchar text[]="Now is the time for all ";
    uchar key[8] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
    uchar iv[8] = {0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef};
    uchar queue[8];
    ushort block_num;
    uchar *data = &text[0];
    ushort i=0;

    block_num = 3;

    printf("\nEncrypt using %d", mode);

    printf("\nCleartext: \n");
    for (i=0; i<24; i++) 
        printf("%02x ", text[i]);

    bcopy(key, epa_swcrypto_conn_tbl[1].encrypt_key, 8);

    /* Always using conn id 1 */
    epa_des_initkey(mode, 1, 8, epa_swcrypto_conn_tbl[1].encrypt_key);

    /* Set the encrption queue */
    bcopy(iv, queue, 8);
    for (i=0; i<block_num; i++) {
        epa_des_encrypt(mode, epa_swcrypto_conn_tbl[1].expanded_encrypt_key,
                        data, queue, 8);
        data += 8;
    }

    printf("\nCipher text\n");
    for (i=0; i<24; i++) 
        printf("%02x ", text[i]);

    /* Set decryption queue */
    bcopy(iv, queue, 8);
    data = &text[0];
    for (i=0; i<block_num; i++) {
        epa_des_decrypt(mode, epa_swcrypto_conn_tbl[1].expanded_encrypt_key,
                        data, queue, 8);
        data += 8;
    }

    printf("\nClear text\n");
    for (i=0; i<24; i++) 
        printf("%02x ", text[i]);

}
#endif

/*
 * NOTE: The following test code is included so that typical DES testing can be
 *       executed in the lab. It should NEVER be enabled in a customer image,
 *       particularly a 40-bit DES image as that could result in an export
 *       violation.
 *
 *       Enable the DES test code by uncommenting the following line:
 *       #define DES_TESTING_ENABLED 
 */

/*
 * Test DES implementation with user input
 */
void crypto_test_des_with_input (parseinfo *csb) 
{
#if DES_TESTING_ENABLED
    uchar text[8];
    uchar key[8];
    uchar iv[8];
    uchar queue[8];
    ushort block_num;
    uchar *data = &text[0];
    ushort i=0;
    ushort mode;
    int text_length;
    boolean doing_encryption;

    if (GETOBJ(int,1) == PARSER_TEST_DES_ENCRYPT) {
	doing_encryption = TRUE;
    } else {
	doing_encryption = FALSE;
    }

    /*
     * Validate mode.
     */
    switch (GETOBJ(int,5)) {
    case 8:
	mode = CFB8_MODE;
	break;
    case 64:
	mode = CFB64_MODE;
	break;
    default:
	printf("Illegal mode value - use 8 or 64 only\n");
	return;
    }
   
    /*
     * Validate  key & IV lengths.
     */
    if (GETOBJ(int,2) != 8) {
	printf("Bad length for key\n");
	return;
    }
    if (GETOBJ(int,3) != 8) {
	printf("Bad length for IV\n");
	return;
    }

    /*
     * For now, restrict the data to one 8-byte block.
     */
    text_length = GETOBJ(int,4);
    if (text_length != 8) {
	printf("Bad length for text\n");
	return;
    }

    bcopy(GETOBJ(string,2), key, 8);
    printf("\nK ");
    for (i=0; i<8; i++) {
        printf("%02x", key[i]);
    }

    bcopy(GETOBJ(string,3), iv, 8);
    printf(" IV ");
    for (i=0; i<8; i++) {
        printf("%02x", iv[i]);
    }

    block_num = text_length/8;
    bcopy(GETOBJ(string,4), text, text_length);
    if (doing_encryption) {
    	printf(" PT ");
    } else {
    	printf(" CT ");
    }
    for (i=0; i<text_length; i++) {
        printf("%02x", text[i]);
    }

    bcopy(key, epa_swcrypto_conn_tbl[1].encrypt_key, 8);

    /* Always using conn id 1 */
    epa_des_initkey(mode, 1, 8, epa_swcrypto_conn_tbl[1].encrypt_key);

    /* Set the encrption queue */
    bcopy(iv, queue, 8);
    for (i=0; i<block_num; i++) {
    	if (doing_encryption) {
            epa_des_encrypt(mode, epa_swcrypto_conn_tbl[1].expanded_encrypt_key,
                            data, queue, 8);
	} else {
	    epa_des_decrypt(mode, epa_swcrypto_conn_tbl[1].expanded_encrypt_key,
	    		    data, queue, 8);
	}
        data += 8;
    }

    if (doing_encryption) {
    	printf(" CT ");
    } else {
    	printf(" PT ");
    }
    for (i=0; i<text_length; i++) 
        printf("%02x", text[i]);
    printf("\n");
#else
    printf("DES testing is not enabled in this image\n");
#endif /* DES_TESTING_ENABLED */
}
