/* $Id: crypto_swepa_ini.c,v 1.1.66.6 1996/08/28 12:39:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/core/crypto_swepa_ini.c,v $
 *------------------------------------------------------------------
 * crypto_swepa_ini.c
 * Initialize sw encryption and key mgt process.
 *
 * 11, 1995, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_swepa_ini.c,v $
 * Revision 1.1.66.6  1996/08/28  12:39:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.66.5  1996/07/17  09:32:20  xliu
 * CSCdi63107:  Show crypto conn eng stat crashes router
 * Branch: California_branch
 *
 * Revision 1.1.66.4  1996/07/15  10:06:05  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 1.1.66.3  1996/07/06  19:45:02  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.66.2  1996/05/12  23:12:49  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.66.1  1996/04/19  14:56:38  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.6  1996/04/17  23:48:02  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.5  1996/03/01  01:39:32  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 1.1.2.4  1996/01/04  18:35:46  xliu
 * Branch: IosSec_branch
 * After initial clean up.
 *
 * Revision 1.1.2.3  1995/12/10  09:27:18  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 1.1.2.2  1995/12/02  17:42:47  xliu
 * Branch: IosSec_branch
 *
 * Modified epa driver interface.
 *
 * Revision 1.1.2.1  1995/11/30  09:30:13  xliu
 * Branch: IosSec_branch
 *
 * Modified to fit the requirement to run on different platforms.
 *
 * Revision 1.1  1995/11/30  09:25:42  xliu
 * Placeholder file.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "crypto_swepa_msg.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "subsys.h"
#include "packet.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_sesmgmt.h"
#include "../crypto/crypto_util.h"
#include "../crypto/crypto_ip.h"
#include "../crypto/crypto_conntable.h"
#include "../crypto/crypto_pubkey.h"
#include "crypto_engine.h"
#include "crypto_swepa.h"
#include "../os/chunk.h"

forktype          crypto_encrypt_proc (void), crypto_key_proc (void);
crypto_engine_idb_t          crypto_engine_idb;
crypto_engine_config_t       crypto_engine_conf;

/*
 * A single extern is placed here rather than putting it in crypto_swepa.h
 * which would require every caller of crypto_swepa.h to include chunk.h too.
 */
extern chunk_type *crypto_dropped_packets_chunks;

/*
 * crypto_engine_idb_init
 *
 * Initialize the structures required for software encryption/key requests.
 * If the hw epa is present, the driver provides the information to init.
 * the interface required to interface epa driver, which is done through 
 * callback. 
 */
void crypto_engine_idb_init (void)
{
    crypto_engine_idb.idb_type = CRYPTO_SWEPA;

    /* Create a watched queue for exp process. */
    crypto_engine_idb.expq = create_watched_queue("EXPQ", 0, 0);

    crypto_engine_idb.encrypt_que = malloc (sizeof(crypto_encrypt_header) * 
                                         EPA_ENCRYPT_QUE_SIZE);

    if (!crypto_engine_idb.encrypt_que) {
        errmsg(&msgsym(FAILED_TO_ALLOCATE_ENCRYPT_QUE, SWEPA));
    }
    crypto_engine_idb.encrypt_que_top = 0;
    crypto_engine_idb.encrypt_que_bot = 0;
    crypto_engine_idb.encrypt_que_count = 0;
}

/*
 * Initialize the dropped packet chunk list. The CHUNK_FLAGS_DYNAMIC flag
 * indicates that if we run out of preallocated chunks, it is allowed to try 
 * to create an additional chunks area.
 */
void crypto_engine_dropped_packet_init (void)
{
    crypto_dropped_packets_chunks = chunk_create(sizeof(crypto_drop_pak_t),
					 CRYPTO_DROPPED_PACKETS_CHUNK_MAX,
					 CHUNK_FLAGS_DYNAMIC, NULL, 0,
					 "Crypto Dropped Packets Chunk");
    return;
}

/*
 * epa_swcrypto_init
 */

void crypto_swepa_initialization (void)
{
    ushort i;
    char *processor;
    ushort key_stacksize;

    /*
     * Initialize the epa_crypto_idb 
     * This initialization routine should be registed as a callback
     * such that when the hw epa is removed, the epa idb will be updated
     * by the sw epa.
     */

     crypto_engine_idb_init();

     /*
      * Initialize the dropped_packet list
      */
    crypto_engine_dropped_packet_init();

    /* Initialize the connection and dh tables */
    for (i=0; i<CRYPTO_CONNTBL_SIZE; i++) {
        epa_swcrypto_conn_tbl[i].state = EPA_SWCRYPTO_CONN_UNSET;
        crypto_dh_pairs [i].status = CRYPTO_DH_PAIR_UNSET;
    }

    /* Initialize the pad buffer which could be used by CFB64 to encrupt
     * /decrpt the last block. 
     */
    memset(crypto_encrypt_pad, '0', EPA_DES_ENCDEC_BLOCK_LENGTH);

    /*
     * Fork encrypt process and key process.
     */
    processor = platform_get_string(PLATFORM_STRING_PROCESSOR);
    if ((strcmp(processor,"68000") == 0) || (strcmp(processor,"68360") == 0) ||
       (strcmp(processor,"68020") == 0) || (strcmp(processor, "68030") == 0) ||
       (strcmp(processor,"68ec030") == 0))
        key_stacksize = LARGE_STACK;
    else
        key_stacksize = NORMAL_STACK;

    process_create(crypto_encrypt_proc, "Encrypt Proc",LARGE_STACK,PRIO_HIGH); 
    process_create(crypto_key_proc, "Key Proc", key_stacksize, PRIO_NORMAL); 

}


