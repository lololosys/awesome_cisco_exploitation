/* $Id: crypto_engine_rsp_ipc.h,v 1.1.50.7 1996/09/11 22:43:18 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/rpsp/crypto_engine_rsp_ipc.h,v $
 *------------------------------------------------------------------
 * Crypto Engine RSP ipc constant and type definitions.
 *
 * Jan 1996, xliu 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_rsp_ipc.h,v $
 * Revision 1.1.50.7  1996/09/11  22:43:18  xliu
 * CSCdi68828:  crypto map displayed on VIP doesnt match crypto map on RSP
 * Branch: California_branch
 * Extended the length of the names.
 *
 * Revision 1.1.50.6  1996/07/03  05:55:01  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 1.1.50.5  1996/07/01  12:26:36  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.50.4  1996/05/28  15:59:58  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.50.3  1996/05/12  23:13:14  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.50.2  1996/05/07  23:51:59  carrel
 * CSCdi56978:  Crypto Maps wont compile with named access lists
 * Branch: California_branch
 * Updated to use named access lists
 *
 * Revision 1.1.50.1  1996/04/19  14:57:06  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.6  1996/04/17  23:48:14  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.5  1996/03/30  05:12:16  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.4  1996/03/21  10:08:42  xliu
 * Branch: IosSec_branch
 * Integrated with RSP changes.
 *
 * Revision 1.1.2.3  1996/01/30  22:36:08  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.2  1996/01/19  07:01:03  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.2.1  1996/01/09  07:21:53  xliu
 * Branch: IosSec_branch
 * Crypto engine RSP.
 *
 * Revision 1.1  1996/01/09  05:32:09  xliu
 * Crypto engine RSP.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CRYPTO_ENGINE_VIP_SLOT0_NAME "crypto_engine_vip_slot0"
#define CRYPTO_ENGINE_VIP_SLOT1_NAME "crypto_engine_vip_slot1"
#define CRYPTO_ENGINE_VIP_SLOT2_NAME "crypto_engine_vip_slot2"
#define CRYPTO_ENGINE_VIP_SLOT3_NAME "crypto_engine_vip_slot3"
#define CRYPTO_ENGINE_VIP_SLOT4_NAME "crypto_engine_vip_slot4"
#define CRYPTO_ENGINE_VIP_SLOT5_NAME "crypto_engine_vip_slot5"
#define CRYPTO_ENGINE_VIP_SLOT6_NAME "crypto_engine_vip_slot6"
#define CRYPTO_ENGINE_VIP_SLOT7_NAME "crypto_engine_vip_slot7"
#define CRYPTO_ENGINE_VIP_SLOT8_NAME "crypto_engine_vip_slot8"
#define CRYPTO_ENGINE_VIP_SLOT9_NAME "crypto_engine_vip_slot9"
#define CRYPTO_ENGINE_VIP_SLOT10_NAME "crypto_engine_vip_slot10"

#define CRYPTO_ENGINE_RSP_CRNT_PORT "crypto_engine_rsp_crnt_port"

#define CRYPTO_VIP_MSG_TYPE_SIZE  4

#define CRYPTO_MAP_TAG_MAX_SIZE   50

/*
 * crypto engine vip ipc msg type defintion
 */

typedef struct {
    ipc_ipfast_msg_hdr  hdr;
    int			slot;
    uchar*              data;
} crypto_engine_vip_ipc_msg;

typedef struct {
    ipc_ipfast_msg_hdr hdr;
    ushort             command;
} crypto_engine_vip_map_msg;

#define CRYPTO_VIP_ADD_MAP_HEAD     0x01
#define CRYPTO_VIP_ADD_MAP          0x02
#define CRYPTO_VIP_DEL_MAP_HEAD     0x03
#define CRYPTO_VIP_DEL_MAP          0x04
#define CRYPTO_VIP_ASSIGN_MAP       0x05
#define CRYPTO_VIP_DEASSIGN_MAP     0x06
#define CRYPTO_VIP_MAP_SET_PEUPE    0x07
#define CRYPTO_VIP_UPD_CONN_ID      0x08
#define CRYPTO_VIP_REMOVE_MAP       0x09
#define CRYPTO_VIP_GET_CONN_TBL     0x0A
#define CRYPTO_VIP_MAP_SET_MODE     0x0B


typedef struct {
    ushort           command;
    ushort           pad;
    int              seq_no;
    int              conn_id;
    int              algorithms;
    ulong            pe_addr;
    ulong            upe_addr;
    int              key_timeout;
    int              acl_number;
    int              num_conn;
    uint	     mode_flags;
    uchar            tag[CRYPTO_MAP_TAG_MAX_SIZE];
    ushort           tag_size;
}crypto_vip_upd_map_msg;

typedef struct {
    ushort           command;
    ushort           tag_size;
    uchar            tag[CRYPTO_MAP_TAG_MAX_SIZE];
} crypto_vip_map_tag_msg;


typedef struct {
    ushort command;
    int    hw_if_index;
    uchar  map_tag[CRYPTO_MAP_TAG_MAX_SIZE];
    int    tag_size;
    int    seq_no;
}crypto_vip_assign_map_to_int_msg;

typedef struct {
    ushort command;
    int    new_conn_id;
    int    old_conn_id;
    int    seq_no;
    uchar  map_tag[CRYPTO_MAP_TAG_MAX_SIZE];
    int    tag_size;
} crypto_vip_map_upd_conn_msg;
   

typedef struct {
    ipc_ipfast_msg_hdr hdr;
    ushort    command;
    int       hw_if_index; 
} crypto_engine_vip_upd_idb_msg;

typedef struct {
    ushort           command;
    int              crypto_engine_id;
} crypto_vip_get_conn_tbl_msg;


#define CRYPTO_VIP_MAP_HEAD_MAX   50
#define CRYPTO_VIP_MAP_MAX        50 
#define CRYPTO_VIP_MAP_TAG_MAX    50 
#define CRYPTO_VIP_KEY_BLK_MAX    50 
#define CRYPTO_VIP_VECTOR_MAX     50
 
/*
 * Generate crypto engine vip ipc port name.
 *
 * The name is hard coded and this routine is used by both the servier and
 * the client to generate the ipc port name.
 */
static inline char* crypto_generate_ipc_port_name (ushort slot)
{
 switch(slot) {
      case 0:
        return CRYPTO_ENGINE_VIP_SLOT0_NAME;
      case 1:
        return CRYPTO_ENGINE_VIP_SLOT1_NAME;
      case 2:
        return CRYPTO_ENGINE_VIP_SLOT2_NAME;
      case 3:
        return CRYPTO_ENGINE_VIP_SLOT3_NAME;
      case 4:
        return CRYPTO_ENGINE_VIP_SLOT4_NAME;
      case 5:
        return CRYPTO_ENGINE_VIP_SLOT5_NAME;
      case 6:
        return CRYPTO_ENGINE_VIP_SLOT6_NAME;
      case 7:
        return CRYPTO_ENGINE_VIP_SLOT7_NAME;
      case 8:
        return CRYPTO_ENGINE_VIP_SLOT8_NAME;
      case 9:
        return CRYPTO_ENGINE_VIP_SLOT9_NAME;
      case 10:
        return CRYPTO_ENGINE_VIP_SLOT10_NAME;
      default:
        return NULL;
    }
}

/* Function protocol type info */
void crypto_engine_rsp_send_ipc_message(hwidbtype *, crypto_engine_vip_ipc_msg *);
void *ip_ipc_get_msg(int);
void ip_ipc_send_msg(int, ipc_message_type, boolean);
void crypto_vip_init_maps(epa_crypto_blk_header *);
