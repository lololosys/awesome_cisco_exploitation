/* $Id: crypto_engine_vip_init.c,v 1.1.68.14 1996/09/10 11:13:38 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/crypto_engine_vip_init.c,v $
 *------------------------------------------------------------------
 * crypto_engine_vip_init.c
 * 
 * This file defines the crypto engine vip initialization. 
 *
 * 11, 1995, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_vip_init.c,v $
 * Revision 1.1.68.14  1996/09/10  11:13:38  xliu
 * CSCdi68600:  ESA should not be given the tralier bytes to encrypt
 * Pass the total length (encap len + ip->tl) to ESA driver.
 * Branch: California_branch
 *
 * Revision 1.1.68.13  1996/09/06  23:57:47  bew
 * CSCdi62313:  Sourced fragments dont encrypt properly
 * Branch: California_branch
 * Add support for fragmented packets: always reassemble fragments
 * before either encryption or decryption.
 *
 * Revision 1.1.68.12  1996/08/18  17:35:27  xliu
 * CSCdi66415:  Use hw_if_index instead of mic_index, following VIP2 code
 * change
 * Branch: California_branch
 * When sending IPC message from RSP to VIP2, use hw_if_index to identify
 * the
 * interface on vip2.
 *
 * Revision 1.1.68.11  1996/08/15  18:04:46  ptran
 * Remove workaround by using IPC to pass EPA's info up to RSP
 * CSCdi64021:  show diag doesnt display EPA info when the next PA is empty
 * Branch: California_branch
 *
 * Revision 1.1.68.10  1996/07/06  22:15:39  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.68.9  1996/07/06  19:45:26  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.68.8  1996/07/01  12:26:54  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.68.7  1996/06/30  18:11:10  ptran
 * CSCdi61257:  snmp and show diag support for encryption PA
 * Branch: California_branch
 *
 * Revision 1.1.68.6  1996/06/21  22:32:56  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.68.5  1996/06/01  01:15:09  xliu
 * CSCdi59234:  crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 1.1.68.4  1996/05/28  16:00:11  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.68.3  1996/05/21  09:44:41  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.68.2  1996/05/12  23:13:36  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.68.1  1996/04/19  14:57:17  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.9  1996/04/17  23:48:31  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.8  1996/03/30  05:12:39  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.7  1996/03/25  23:19:34  xliu
 * Branch: IosSec_branch
 * Checkin VIP crypto engine code.
 *
 * Revision 1.1.2.6  1996/01/30  22:36:19  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.5  1996/01/09  02:26:55  xliu
 * Branch: IosSec_branch
 * VIP software encryption.
 *
 * Revision 1.1.2.4  1995/12/10  09:30:09  xliu
 * Branch: IosSec_branch
 * Merge the code to replace the idb fastsend using ipc.
 *
 * Revision 1.1.2.3  1995/12/05  05:41:47  xliu
 * Branch: IosSec_branch
 * Add output vip packet analysis and debugging.
 *
 * Revision 1.1.2.2  1995/12/02  17:43:01  xliu
 * Branch: IosSec_branch
 *
 * Modified epa driver interface.
 *
 * Revision 1.1.2.1  1995/11/30  05:39:54  xliu
 * Branch: IosSec_branch
 *
 * Files created for crypto engine vip.
 *
 * Revision 1.1  1995/11/30  05:23:34  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "types.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "logger.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "config.h"
#include "../wan/serial.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../if/tring.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../hes/dgbus.h"
#include "../dev/monitor1.h"

#include "../src-vip/dfs_vip_idb.h"

#include "../ip/ipfast_index.h"
#include "../ip/ipfast_ipc.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"

#include "sys_registry.h"

#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ip/ip_registry.h"

#include "../os/chunk.h"

#include "../pas/if_pas.h"       /* includes pas_pci.h & segpak.h */
#include "../src-vip/vip_memd.h"
#include "../src-vip/vip_debug.h"

#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipc/ipc_message.h"
#include "../ipc/ipc_seats.h"
#include "../ipc/ipc_ports.h"
#include "../ipc/ipc_test.h"
#include "../ipc/ipc_cbus.h"
#include "../ipc/ipc_debug.h"

#include "../crypto/crypto_registry.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_sesmgmt.h"
#include "crypto_engine_vip.h"
#include "crypto_hwepa.h"
#include "../crypto/crypto_engine/rpsp/crypto_engine_rsp_ipc.h"
#include "ttysrv.h"


/* 
 * Global veriable 
 */
crypto_vip_conn_entry       crypto_vip_conntbl[CRYPTO_CONNTBL_SIZE];
ulong                       crypto_engine_vip_key_req_id;
ushort                      crypto_engine_vip_blk_index = 0;
ushort                      crypto_engine_vip_dss_p_handle = 0;
forktype                    crypto_engine_vip_key_rsp_dspt(void);
crypto_engine_vip_dh_entry  crypto_engine_vip_dh_tbl[CRYPTO_CONNTBL_SIZE];
crypto_vip_key_req_context
                            crypto_vip_key_req_context_pool[
                                         CRYPTO_VIP_KEY_CONTEXT_POOL_SIZE];
epa_crypto_blk              crypto_engine_vip_blk_buffer[
                                         CRYPTO_VIP_KEY_REQ_BLK_BUFFER_SIZE];
chunk_type                 *crypto_map_head_chunk;
chunk_type                 *crypto_map_chunk;
chunk_type                 *crypto_key_blk_chunk;
chunk_type                 *crypto_map_tag_chunk;
chunk_type                 *crypto_vector_chunk;
ulong                      crypto_engine_vip_mem_in_use = 0;
ulong			   crypto_processor_serial;

/* To be entered through parser command */
uchar                       crypto_engine_vip_epa_passwd[10] = 
                                         CRYPTO_ENGINE_VIP_EPA_PASSWD;
void crypto_engine_vip_parser_init (void);

/*
 * crypto_engine_epa_random_init
 *
 * Send random seed number to EPA. This is called only when EPA status
 * indicate that the seed number is not set yet.
 */
void crypto_engine_epa_random_init (void)
{
    epa_crypto_blk *blk;

    blk = malloc (sizeof(epa_crypto_blk));
    if (!blk) {
        buginf("\nCRYPTO_ENGINE: can not start to init epa random number.");
        return;
    }

    blk->next = NULL;
    blk->header.command_id = CRYPTO_INIT_RANDOM;
    crypto_send_epa_key_request (blk);
}

void crypto_engine_vip_get_maps (void)
{
    ipc_message *message;
    epa_crypto_blk_header  crypto_blk_hdr;

    message = ipc_get_message(sizeof(epa_crypto_blk_header), 0, 0);
    if (!message) {
        return;
    }
    crypto_blk_hdr.command_id = CRYPTO_ENGINE_VIP_GET_CFG;
    crypto_blk_hdr.status = crypto_engine_vip_config.epa_type;
    crypto_blk_hdr.req_id = vip_logical_slot();

    bcopy(&crypto_blk_hdr, message->header->data, 
          sizeof(epa_crypto_blk_header));

    ipc_send_message_permanent_port(message, IPC_TYPE_CRYPTO);

}

void crypto_engine_vip_register_evec (void *ptr)
{
}

void crypto_engine_vip_init_chunks(void)
{
    crypto_map_head_chunk = chunk_create(sizeof(crypto_map_headtype),
                                         CRYPTO_VIP_MAP_HEAD_MAX,
                                         CHUNK_FLAGS_NONE, NULL,
                                         0, "Crypto maphead");
    crypto_engine_vip_mem_in_use += sizeof(crypto_map_headtype) *
                                    CRYPTO_VIP_MAP_HEAD_MAX;

    crypto_map_chunk = chunk_create(sizeof(crypto_maptype),
                                         CRYPTO_VIP_MAP_MAX,
                                         CHUNK_FLAGS_NONE, NULL,
                                         0, "Crypto map");
    crypto_engine_vip_mem_in_use += sizeof(crypto_maptype) *
                                    CRYPTO_VIP_MAP_MAX;

    crypto_key_blk_chunk = chunk_create(sizeof(epa_crypto_blk),
                                         CRYPTO_VIP_KEY_BLK_MAX,
                                         CHUNK_FLAGS_NONE, NULL,
                                         0, "Crypto keyblk");
    crypto_engine_vip_mem_in_use += sizeof(epa_crypto_blk) *
                                    CRYPTO_VIP_KEY_BLK_MAX;

    crypto_vector_chunk = chunk_create(sizeof(crypto_vector_t),
                                         CRYPTO_VIP_VECTOR_MAX,
                                         CHUNK_FLAGS_NONE, NULL,
                                         0, "Crypto vector");
    crypto_engine_vip_mem_in_use += sizeof(crypto_vector_t) *
                                    CRYPTO_VIP_VECTOR_MAX;

    crypto_map_tag_chunk = chunk_create(CRYPTO_MAP_TAG_MAX_SIZE,
                                         CRYPTO_VIP_MAP_TAG_MAX,
                                         CHUNK_FLAGS_NONE, NULL,
                                         0, "Crypto maptag");
    crypto_engine_vip_mem_in_use += CRYPTO_MAP_TAG_MAX_SIZE * 
                                    CRYPTO_VIP_MAP_TAG_MAX;
}

/*
 * Initialize the crypto engine subsystem
 *
 * The epa driver, if presented, will be initialized before crypto_engine.
 */
void crypto_engine_vip_init (subsystype *subsys)
{
    ushort i;
    dgbus_eeprom_t  eeprom_data;
    charlong        temp;
    int             sram_size;

    /*
     * Check this is vip2-40, according to the memory size. If not,
     * send message to RSP.
     */

    sram_size = mon_get_iosize();
    if (sram_size < TWO_MEG) {
        crypto_engine_vip_config.epa_type = CRYPTO_TYPE_UNKNOWN;
        /* We still make this registry since it report the failure to rsp */
        reg_add_crypto_engine_vip_get_maps(crypto_engine_vip_get_maps,
                                         "crypto_engine_vip_get_maps");
        return;
    }

    /*
     * Parser chain initialization
     */
    crypto_engine_vip_parser_init();

    /* Initialize the crypto engine swepa if hwepa is not
     * present.
     */
    if (crypto_engine_vip_config.epa_type == CRYPTO_TYPE_UNINIT) {

        crypto_engine_vip_config.epa_type = CRYPTO_SWEPA;
        crypto_engine_vip_config.hwepa_status = CRYPTO_HWEPA_NOT_PRESENT;
        crypto_engine_idb.vip_encrypt = crypto_vip_sw_encrypt;
        crypto_engine_idb.vip_decrypt = crypto_vip_sw_decrypt;

        /* Initialize the encryption process encryption routine. */
        crypto_engine_encrypt = crypto_sw_vip_encrypt_decrypt;
        crypto_engine_decrypt = crypto_sw_vip_encrypt_decrypt;
        crypto_engine_reassemble = NULL; /* Reassembly not yet supported */

	/*
	 * Set the count to relinquish the cpu during connection
	 * setup very high. The higher the number the less we give 
	 * up the cpu.
	 */
        crypto_release_counter = 2 * CRYPTO_RELEASE_CNT_LOW;
	crypto_yield_handle.yield_proc = crypto_engine_key_proc_yield;
	crypto_yield_handle.handle = &crypto_release_counter;

        /* Initialize the software output vector */
        crypto_engine_idb.swepa_output = crypto_swepa_vip_output;

        /* Start software encryption/key processes. */
        crypto_swepa_initialization();

        /* Initialize the crypto engine serial number using vip serial num. */
        crypto_engine_vip_config.epa_serial_number = get_vip_serial_number();
    } else {
        pas_get_eeprom_data(crypto_engine_vip_config.epa_encrypt_idb->slot,
                         (void *)&eeprom_data, sizeof(dgbus_eeprom_t)/2);
        temp.d.byte[0] = eeprom_data.serial_number_31_24;
        temp.d.byte[1] = eeprom_data.serial_number_23_16;
        temp.d.byte[2] = eeprom_data.serial_number_15_8;
        temp.d.byte[3] = eeprom_data.serial_number_7_0;
        crypto_engine_vip_config.epa_serial_number = temp.d.lword;

        /* Initialize the dss key private key handle */
        crypto_engine_vip_dss_p_handle = CRYPTO_ESA_DSS_PRIVATE_KEY_HANDLE; 
    }

    /* Initialize the connection table */
    for (i=0; i<CRYPTO_CONNTBL_SIZE; i++) {
       crypto_vip_conntbl[i].state = CRYPTO_CONN_UNSET;
    }

    crypto_engine_conf.platform = CRYPTO_PLATFORM_VIP;

    reg_add_crypto_engine_vip_upd_map(crypto_engine_vip_upd_map,
                                      "crypto_engine_vip_upd_map");
    reg_add_crypto_engine_recv_key_request(crypto_engine_recv_key_request,
                                      "crypto_engine_recv_key_request");

    reg_add_crypto_map_check(crypto_map_check, "crypto_map_check");

    reg_add_crypto_engine_vip_get_maps(crypto_engine_vip_get_maps,
                                      "crypto_engine_vip_get_maps");

    reg_add_crypto_engine_vip_register_evector(crypto_engine_vip_register_evec,
                                     "crypto_engine_vip_register_evec");

    /* Set up chunks for key and mgt ipc commands. */
    crypto_engine_vip_init_chunks();

    /* Initialize the serial number global */
    crypto_processor_serial=platform_get_value(PLATFORM_VALUE_HARDWARE_SERIAL);
}

/*
 * CRYPTO subsystem header
 */
#define CRYPTO_ENGINE_MAJVERSION 1
#define CRYPTO_ENGINE_MINVERSION 0
#define CRYPTO_ENGINE_EDITVERSION 1

SUBSYS_HEADER(crypto_engine, CRYPTO_ENGINE_MAJVERSION, CRYPTO_ENGINE_MINVERSION,
              CRYPTO_ENGINE_EDITVERSION,
	      crypto_engine_vip_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: vip_ip_fs",
	      "req: vip_ip_fs");


