/* $Id: crypto_engine_vip_chain.c,v 1.1.50.9 1996/07/15 09:59:24 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/crypto_engine_vip_chain.c,v $
 *------------------------------------------------------------------
 * crypto engine vip parser command definition file.
 *
 * Jan 1996, xliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_vip_chain.c,v $
 * Revision 1.1.50.9  1996/07/15  09:59:24  xliu
 * CSCdi61651:  epa randome number seed is not initialized
 * Branch: California_branch
 *
 * Revision 1.1.50.8  1996/07/06  19:45:24  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.50.7  1996/06/21  22:32:50  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.50.6  1996/06/01  01:15:07  xliu
 * CSCdi59234:  crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 1.1.50.5  1996/05/28  16:00:10  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.50.4  1996/05/21  09:44:40  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.50.3  1996/05/12  23:13:35  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.50.2  1996/04/22  17:50:21  xliu
 * CSCdi55271:  remove including empty header file cfg_crypto_engine.h.
 * Branch: California_branch
 * Remove including empty file.
 *
 * Revision 1.1.50.1  1996/04/19  14:57:15  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.4  1996/04/09  20:03:26  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.2.3  1996/03/30  05:12:36  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.2  1996/03/25  23:19:29  xliu
 * Branch: IosSec_branch
 * Checkin VIP crypto engine code.
 *
 * Revision 1.1.2.1  1996/01/09  02:26:54  xliu
 * Branch: IosSec_branch
 * VIP software encryption.
 *
 * Revision 1.1  1996/01/09  02:11:58  xliu
 * Placeholder file.
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
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "sched.h"
#include "stacks.h"
#include "ttysrv.h"
#include "config.h"
#include "access.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../if/tring.h"
#include "../if/network.h"

#include "../src-vip/dfs_vip_idb.h"
#include "../src-vip/vip_debug.h"

#include "../ip/ipfast_index.h"
#include "../ip/ipfast_ipc.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_registry.h"

#include "../pas/if_pas.h"       /* includes pas_pci.h & segpak.h */
#include "../src-vip/vip_memd.h"

#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_engine/vip/crypto_engine_vip.h"
#include "../crypto/crypto_engine/rpsp/crypto_engine_rsp_ipc.h"

#include "crypto_hwepa.h"

uchar sha[20];   
uchar crypto_publics[64];
uchar dh_publics[64];
crypto_sig_t  sig;

extern ulong crypto_64_counter;
extern ulong crypto_128_counter;
extern ulong crypto_256_counter;
extern ulong crypto_512_counter;
extern ulong crypto_768_counter;
extern ulong crypto_1024_counter;
extern ulong crypto_1518_counter;

typedef enum parser_const_ {
    PARSER_SHOW_STATIS,
    PARSER_SHOW_CONN_TBL,
    PARSER_SHOW_CONFIG,
    PARSER_SHOW_DSS_KEYS,
    PARSER_SHOW_ACL,
    PARSER_SHOW_MAP,
    PARSER_SHOW_IDB,
} parser_const;


/*
 * Parse chains for config commands
 */
#define	ALTERNATE	NONE
LINK_POINT(crypto_engine_config_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for show commands
 */
#define	ALTERNATE	NONE
#include "../crypto/crypto_engine/vip/exec_show_crypto_engine.h"
LINK_POINT(crypto_engine_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for clear commands
 */
#define	ALTERNATE	NONE
#include "../crypto/crypto_engine/vip/exec_clear_crypto_engine.h"
LINK_POINT(crypto_engine_clear_commands, ALTERNATE);
#undef	ALTERNATE

/*
 *  Parse chain registration array for static map commands
 */
parser_extension_request static_crypto_engine_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(crypto_engine_config_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(crypto_engine_show_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(crypto_engine_clear_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * crypto_parser_init - Initialize crypto parser support
 */
void crypto_engine_vip_parser_init (void)
{
    parser_add_command_list(static_crypto_engine_chain_init_table, "crypto_engine");
}

void crypto_hma_alloc_cmd (parseinfo *csb)
{
    int size = GETOBJ(int, 1);
    uchar attribute = EPA_HMA_READ_ALLOWED | EPA_HMA_WRITE_ALLOWED;
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;

    if (crypto_engine_vip_debug) {
        buginf("\nCRYPTO_ENGINE: send hma alloc command.\n");
    }

    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;
    blk->header.command_id = EPA_TEST_ONLY;
    blk->next = NULL;

    hMemAlloc (size, attribute, blk);
}

void crypto_hma_free_cmd (parseinfo *csb)
{

    int handle = GETOBJ(int, 1);
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;


    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: send hma alloc command.\n");
    }

    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;
    blk->header.command_id = EPA_TEST_ONLY;

    hMemFree (handle, blk);
}

void crypto_hma_read_cmd (parseinfo *csb)
{

    int handle=  GETOBJ(int, 1);
    int size = GETOBJ(int, 2);
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;


    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: send hma read command.\n");
    }

    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;
    blk->header.command_id = EPA_TEST_ONLY;

    hMemRead (handle, size, blk);
}

void crypto_hma_write_cmd (parseinfo *csb)
{

    int handle = GETOBJ(int, 1);
    uchar data[] = "this is a test string";
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;


    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: send hma write command.\n");
    }

    blk->header.status = CRYPTO_ENGINE_REGIST_KEY_REQ;
    blk->header.command_id = EPA_TEST_ONLY;

    hMemWrite (handle, sizeof("this is a test string"), data, blk);
}

void crypto_set_passwd_cmd (parseinfo *csb)
{
    uchar passwd[] = "CISCO-EPA";
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;


    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: set epa password.\n");
    }

    blk->header.status = EPA_SET_EPA_PASSWD;
    blk->header.command_id = EPA_TEST_ONLY;

    hSetEPAPasswd (passwd, sizeof("CISCO-EPA"), blk);
}

void crypto_sha_cmd (parseinfo *csb)
{
    uchar message[200];
    int size =  GETOBJ(int, 1);
    ushort i;
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;


    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: SHA test.\n");
    }

    blk->header.status = EPA_SHA;
    blk->header.command_id = EPA_TEST_ONLY;

    for (i=0; i<size; i++) message[i] = i;
    hSHA (message, size, blk);
}
    
void crypto_gen_dsskey_cmd (parseinfo *csb)
{
    short p_handle = GETOBJ(int, 1);
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;


    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: Generate DSS Key.\n");
    }

    blk->header.status = EPA_GEN_DSS_KEY;
    blk->header.command_id = EPA_TEST_ONLY;

    hGenDSSKey(p_handle, 64, blk);
}

void crypto_gen_dsssig_cmd (parseinfo *csb)
{
    short p_handle = GETOBJ(int, 1);
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;


    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: Generate DSS Signature.\n");
    }

    blk->header.status = EPA_GEN_DSS_SIGNATURE;
    blk->header.command_id = EPA_TEST_ONLY;

    hGenDSSSignature (p_handle, sha, 64, blk); 

}

void crypto_ver_dsssig_cmd (parseinfo *csb)
{
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;


    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: Verify DSS Signature.\n");
    }

    blk->header.status = EPA_VER_DSS_SIGNATURE;
    blk->header.command_id = EPA_TEST_ONLY;

    hVerDSSSignature(sha, 64, &sig, crypto_publics, blk);

}

void crypto_gen_ran_number_cmd (parseinfo *csb)
{
    ushort ran_size = GETOBJ(int, 1);
    short  ran_handle = GETOBJ(int, 2);
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;


    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: Generate Random Number.\n");
    }

    blk->header.status = EPA_GEN_RANDOM;
    blk->header.command_id = EPA_TEST_ONLY;

    hGenRandom (ran_handle, ran_size, blk);
}

void crypto_read_ran_number_cmd (parseinfo *csb)
{

}

void crypto_set_ran_seed_cmd (parseinfo *csb)
{
    uchar seed[] = "This is the seed";
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;


    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: Set Random Number Seed.\n");
    }

    blk->header.status = EPA_SET_RN_SEED;
    blk->header.command_id = EPA_TEST_ONLY;

    hSetRNSeed (seed, sizeof("This is the seed"), blk);
}

void crypto_gen_dh_pair_cmd (parseinfo *csb)
{
    short p_handle = GETOBJ(int, 1);
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;

 
    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: Generate DH Pair.\n");
    }

    blk->header.status = EPA_GEN_DH_PAIR;
    blk->header.command_id = EPA_TEST_ONLY;

    hGenDHPair (p_handle, DH_LENGTH_MIN, blk);

}

void crypto_get_dh_shared_cmd (parseinfo *csb)
{
    short p_handle = GETOBJ(int, 1);
    short s_handle = GETOBJ(int, 2);
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;

 
    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: Get shared DH number.\n");
    }

    blk->header.status = EPA_GET_DH_SHARED;
    blk->header.command_id = EPA_TEST_ONLY;

    hGetDHShared(p_handle, s_handle, DH_LENGTH_MIN, dh_publics, blk);
}

void crypto_set_session_cmd (parseinfo *csb)
{
    short encrypt_handle = GETOBJ(int, 1);
    short iv_handle  = GETOBJ(int, 2);
    ushort encrypt_method = 0x0B;
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;


 
    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: Set DES key.\n");
    }

    blk->header.status = EPA_SET_SESSION;
    blk->header.command_id = EPA_TEST_ONLY;

    hSetSession(iv_handle, encrypt_handle,  encrypt_method, blk);

}

void crypto_free_session_cmd (parseinfo *csb)
{
    short session_handle = GETOBJ(int, 1);
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;

 
    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: Free session %d\n", session_handle);
    }

    blk->header.status = EPA_FREE_SESSION;
    blk->header.command_id = EPA_TEST_ONLY;

    hFreeSession(session_handle, blk);

}

void crypto_zeroize_cmd (parseinfo *csb)
{
    epa_crypto_blk   *blk;


    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;

    if (crypto_engine_vip_debug) {
        buginf("\nCRYPTO_ENGINE: send zerozie command");
    }

    blk->header.status = EPA_ZEROIZE_EPA;
    blk->header.command_id = EPA_TEST_ONLY;

    hZeroize(blk);

}

void crypto_set_epa_status_cmd (parseinfo *csb)
{
    ulong  mask = GETOBJ(int, 2);
    ulong  flag = GETOBJ(int, 3);
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;

 
    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: Set EPA status %04x\n", mask);
    }

    blk->header.status = EPA_SET_EPA_STATUS;
    blk->header.command_id = EPA_TEST_ONLY;

    hSetEPAStatus(sizeof("CISCO-EPA"), mask, flag, "CISCO-EPA", blk);

}

void crypto_get_epa_status_cmd (parseinfo *csb)
{
    epa_crypto_blk   *blk;

    blk = malloc(sizeof(epa_crypto_blk));
    blk->next = NULL;

 
    if (crypto_engine_vip_debug) {
        buginf(  "\nCRYPTO_ENGINE: Get Status\n");
    }

    blk->header.status = EPA_GET_EPA_STATUS;
    blk->header.seq_number = CRYPTO_REQ_SEQ_END; 
    blk->header.command_id = EPA_TEST_ONLY;

    hGetEPAStatus(blk);

}

void crypto_engine_show_cmd (parseinfo *csb)
{
    ushort i = 0;
    epa_swcrypto_connection *conn;

    switch(GETOBJ(int, 1)) {
    case PARSER_SHOW_CONN_TBL:

        printf("\n%10s %10s  %5s %8s %8s %8s\n","Connection", 
               "Interface", "State","Algorithm", "Encrypt", "Decrypt");

        conn = &epa_swcrypto_conn_tbl[0];

        for (i = 0; i<= 5; i++, conn++) 
            if (i == CRYPTO_CONNTBL_SIZE) {
                buginf ("Error: maximum connection number exceeded!\n");
                return;
            } else {
            if (conn->state == EPA_SWCRYPTO_CONN_SET) {
                if (conn->swidb) {
                    printf("%10d %10s  %5d %8d %8d %8d\n",
                           conn->connection_id,
                           is_subinterface(conn->swidb)? 
                               conn->swidb->short_namestring:
                               conn->swidb->namestring,
                           conn->state, conn->alg_type, conn->encrypt_count, 
                           conn->decrypt_count);
                } else {
                    printf("%10d %10s  %5d %8d %8d %8d\n",
                           conn->connection_id,
                           "unknown",  
                           conn->state, conn->alg_type, conn->encrypt_count, 
                           conn->decrypt_count);
                }
            } else {
	        printf("%10s %10s  %5s %8s %8s %8s\n", "unset",
                       "unset", "unset", "unset", "unset", "unset");
            }
        }
    break;

    case PARSER_SHOW_DSS_KEYS:

        if (crypto_engine_vip_config.epa_type == CRYPTO_HWEPA) {
            printf("\nDSS private handle: %d", crypto_engine_vip_dss_p_handle);
        } else {

        printf("\nDSS private key: \n");
        for (i=0; i<20; i++)
            printf("%02x ", crypto_pri_key[i]);
        printf("\n");
        printf("\nDSS public key: \n");
        for (i=0; i<64; i++)
            printf("%02x ", crypto_pub_key[i]);
        printf("\n");
        }
    break;

    case PARSER_SHOW_ACL:
    {
        ulong list = GETOBJ(int, 2);
        slowtype *item = NULL, *next=NULL;

        printf("\n Access Control List: %ld", list);

        item = (slowtype *)accessarray[list];
        for (; item; item = next) {
            next = item->next;
            printf("\ngrant: %d, prot: %d", 
                    item->grant, item->protocol);
        }
     }
     break;
  
     case PARSER_SHOW_MAP:

        crypto_vip_show_map();
        break;

     case PARSER_SHOW_IDB:
         crypto_show_idb();
   
    default:
    break;
    }

}

void crypto_engine_test_cmd (parseinfo *csb)
{

}

void crypto_engine_clear_cmd (parseinfo *csb)
{

}

void crypto_show_conn_cmd (parseinfo *csb)
{
    ushort i;
    crypto_vip_conn_entry *conn;

    conn = &crypto_vip_conntbl[0];

    printf("\n%10s %10s %10s %10s %10s %10s",
           "conn id", "dh-s-h", "encrypt-h", "iv-h", "method", "state");

    for (i=0; i< 5; i++) {

        printf("\n%10d %10d %10d %10d %10d %10d",
               conn->connection_id,
               conn->hma_dh_shared,
               conn->hma_encrypt_key,
               conn->hma_iv_key,
               conn->encrypt_method,
               conn->state);
        conn++;
    }
}

void crypto_show_idb (void)
{
    hwidbtype *idb;
    crypto_vector_t *crypto_vec;

    printf("\nsys particle size: %d", vsys_vec.vip_particle_size);

    printf("\nprivate dss handle: %d", crypto_engine_vip_dss_p_handle);

    FOR_ALL_HWIDBS(idb) {
        printf("\nidb = %x", idb);
        printf("\ncrypto vector: %x", idb->crypto_vectors);
        if (idb->crypto_vectors) {
            crypto_vec = (crypto_vector_t *)idb->crypto_vectors;
            printf("\nmedia fastsend: %x", crypto_vec->media_fastsend);
            printf("\nmedia pak dfs: %x", crypto_vec->media_pak_dfs);
        }
        if (idb->firstsw && idb->firstsw->crypto_map) {
            printf("\ncrypto map: %s", idb->firstsw->crypto_map);
        }
    }
} 


void crypto_set_keys_cmd (parseinfo *csb)
{
   uchar key[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};
   ushort i;
                        
    buginf(  "\n");

    for (i=0; i<128; i++) 
        buginf(  "%02x ", epa_swcrypto_conn_tbl[1].expanded_encrypt_key[i]);

   if (epa_des_initkey(DES_CFB64_IV1,
                   1,
                   CRYPTO_DES_KEY_VALUE_SIZE,
                   key)) {
       buginf(  "\nCRYPTO_ENGINE: encrypt key successed.");
    } else {
       buginf(  "\nCRYPTO_ENGINE: encrypt key failed.");
    } 

    buginf(  "\n");

    for (i=0; i<128; i++) 
        buginf("%02x ", epa_swcrypto_conn_tbl[1].expanded_encrypt_key[i]);

    bcopy(key, epa_swcrypto_conn_tbl[1].iv_key, 8);

}

