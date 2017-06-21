/* $Id: crypto_engine.h,v 1.1.68.11 1996/09/06 23:57:33 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/core/crypto_engine.h,v $
 *------------------------------------------------------------------
 * crypto_engine.h
 * 
 * This file defines the types and constants used by crypto engine. 
 *
 * 11, 1995, xliu 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine.h,v $
 * Revision 1.1.68.11  1996/09/06  23:57:33  bew
 * CSCdi62313:  Sourced fragments dont encrypt properly
 * Branch: California_branch
 * Add support for fragmented packets: always reassemble fragments
 * before either encryption or decryption.
 *
 * Revision 1.1.68.10  1996/08/07  09:21:35  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 1.1.68.9  1996/07/17  09:32:09  xliu
 * CSCdi63107:  Show crypto conn eng stat crashes router
 * Branch: California_branch
 *
 * Revision 1.1.68.8  1996/07/15  10:05:49  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 1.1.68.7  1996/07/03  05:54:48  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 1.1.68.6  1996/07/01  12:26:14  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.68.5  1996/06/21  22:29:42  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.68.4  1996/06/05  23:11:46  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 1.1.68.3  1996/05/28  15:59:13  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.68.2  1996/05/12  23:12:43  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.68.1  1996/04/19  14:56:31  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.14  1996/04/17  23:47:56  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.13  1996/03/30  05:12:00  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.12  1996/03/21  21:16:58  xliu
 * Branch: IosSec_branch
 * Add registry call to download crypto map.
 *
 * Revision 1.1.2.11  1996/03/21  10:07:17  xliu
 * Branch: IosSec_branch
 * Migrated to CA branch.
 *
 * Revision 1.1.2.9  1996/02/05  03:22:22  xliu
 * Branch: IosSec_branch
 * Add DH code; Add crypto_engine_encrypt/decrypt vectors.
 *
 * Revision 1.1.2.8  1996/01/30  22:35:57  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.7  1996/01/20  16:51:26  xliu
 * Branch: IosSec_branch
 * Solve the 7000 problem with ip les fastswitch code.
 *
 * Revision 1.1.2.6  1996/01/09  19:13:42  xliu
 * Branch: IosSec_branch
 * Delete obsolete prototyping functions.
 *
 * Revision 1.1.2.5  1996/01/04  18:35:43  xliu
 * Branch: IosSec_branch
 * After initial clean up.
 *
 * Revision 1.1.2.4  1995/12/17  06:22:59  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 1.1.2.3  1995/12/10  09:27:14  xliu
 * Branch: IosSec_branch
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 1.1.2.2  1995/12/02  17:42:44  xliu
 * Branch: IosSec_branch
 *
 * Modified epa driver interface.
 *
 * Revision 1.1.2.1  1995/11/30  04:59:44  xliu
 * Branch: IosSec_branch
 *
 * Moved the crypto engine core files into dir CORE.
 *
 * Revision 1.1  1995/11/30  04:20:49  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef EPA_ENGINE_H
#define EPA_ENGINE_H
/* Constants defined for the resource tables          */
#define    CRYPTO_CONNTBL_SIZE          300   /* the size of the conn table */
#define    CRYPTO_ENGINE_MAX_NUM        15    /* for RSP/VIP platform */ 
#define    CRYPTO_ENGINE_REQPOOL_SIZE   50    /* the max num. of holding req*/
#define    CRYPTO_ENGINE_REQ_ENTRY_UNUSED  0  /* the unused entry in req pool */
#define    CRYPTO_REQ_DATA_MAX_SIZE     512   /* the req data buf size */ 
#define    CRYPTO_DH_PAIR_SET           1
#define    CRYPTO_DH_PAIR_UNSET         0

/* Constants for crypto request sequence end mark */
#define    CRYPTO_REQ_SEQ_END  100
#define    CRYPTO_ENGINE_VIP_SEND_TO_RSP   0xFFFF

/* Constants for crypto engine timer control flag */
#define    CRYPTO_REQ_TIMEOUT_SET     1
#define    CRYPTO_REQ_TIMEOUT_UNSET   0
#define    CRYPTO_REQ_TIMER_UPDATE_INTERVAL  2000000L

/* Constants for crypto request block data buffer */
#define CRYPTO_ENGINE_RSP_BUFFER_LENGTH  100

#define DH_LENGTH_MIN 64
#define DSS_LENGTH_MIN 64

#define CRYPTO_ENGINE_IPC_MSG_SIZE    100
#define CRYPTO_ENGINE_MAX        15	
#define CRYPTO_NO_CANCEL         0

#define CRYPTO_ENGINE_REGISTER   0x00
#define CRYPTO_ENGINE_DEREGISTER 0x01

/*
 * Defines for crypto_release_counter values. See
 * crypto_engine_init() for more details.
 */
#define CRYPTO_RELEASE_CNT_HIGH		15
#define CRYPTO_RELEASE_CNT_MID		25
#define CRYPTO_RELEASE_CNT_LOW		50
#define CRYPTO_RELEASE_CNT_DEFAULT CRYPTO_RELEASE_CNT_MID

#define CRYPTO_KEY_COMM_TIMER    0x01

/* 
 * Type definition of the element of input queue to swepa encrypt process. 
 */
typedef struct {
    short            conn_id;
    ushort           data_size;
    uchar*           data;
    ushort           command;
    ushort           iv_seed_size;
    uchar            iv_seed[CRYPTO_MAX_IV_SIZE];
    ushort           status;
    ushort           switch_mode;
    crypto_pak_info  pak_info;
} crypto_encrypt_header;

/*
 * structure definition for crypto engine req registration pool
 */
typedef struct epa_engine_req_ {
    int                   req_id;
    epa_callback_context* proc_context;
    epa_callback_handle   proc_callback;
    mgd_timer             timer;
    ushort                command_id;
    ushort                crypto_unit_id;   
} epa_engine_req;

/*
 * Type definition of the crypto key request/response block.
 */
typedef struct epa_crypto_blk_header_ {
    ushort    req_id;
    ushort    seq_number;
    ushort    command_id;
    ushort    status;
    ushort    data_size;
    ushort    pad;
} epa_crypto_blk_header;

typedef struct epa_crypto_blk_ {
    struct epa_crypto_blk_ *next;
    struct epa_crypto_blk_header_ header;
    uchar  data[CRYPTO_REQ_DATA_MAX_SIZE];
} epa_crypto_blk;

/*
 * Type definition of the crypto key request block data format 
 * The data format for the response block is defined in epa_api.h.
 * ? performance
 */

typedef struct {  /* connection id, alg_type_id, */
    ushort    connection_id;
    ushort    encrypt_alg_type;
} epa_crypto_session_key_data_blk; 

typedef struct {
   ushort     connection_id;
   uchar      peer_public_number[DH_LENGTH_MIN];
} crypto_gen_dh_param_data_blk;

typedef struct {
   uchar   secret_number[64];
   uchar   public_number[64];
   uchar   shared_number[64];
   ushort  status;  /* set =1, unset = 0 */
} crypto_dh_pair;

typedef struct {
   uchar*     data;
   ushort     data_length;
   ushort     alg_type;
} crypto_sign_data_blk;

typedef struct {
   ushort     data_length;
   ushort     alg_type;
} crypto_vip_sign_data_blk;

typedef struct {
   uchar*     clear_data;
   uchar*     signature;
   ushort     clear_data_length;
   ushort     signature_length;
   ushort     alg_type;
   uchar      public_key[DSS_LENGTH_MIN];
} crypto_verify_data_blk;

typedef struct {
    ushort  clear_data_length;
    ushort  signature_length;
    ushort  alg_type;
    uchar   public_key[DSS_LENGTH_MIN];
} crypto_vip_verify_data_blk;

typedef struct {
    ushort    conn_id;
} crypto_clear_conn_entry_data_blk;

typedef struct {
    uchar passwd[CRYPTO_EPA_PASSWD_MAX_SIZE];
    ushort passwd_size;
}crypto_config_epa_data_blk;

typedef struct {
    uchar passwd[CRYPTO_EPA_PASSWD_MAX_SIZE];
    ushort passwd_size;
}crypto_gen_dss_key_data_blk;

typedef struct {  /* connection id, alg_type_id, */
    ushort    connection_id;
    ushort    challenge_length;
    ushort    offset;
    uchar*    challenge;
} crypto_gen_syndrome_data_blk; 

typedef struct {
    ushort   connection_id;
}crypto_clear_dh_number_data_blk;


/*
 * crypto engine idb
 *
 * This structure defines the crypto engine interface, which consists 
 * the following:
 *
 * 0. The type of the encryption/key module: SW/HW.
 * 1. The input queue for sw encryption process, as well as its counters; 
 * 2. The input queue for sw key service process; 
 * 3. SW encryption output vector: rp, rsp. vip;
 * 4. VIP encryption vector: sw, hw
 * 5. VIP decryption vector: sw, hw
 */

typedef void (* crypto_swepa_output_t) (ushort, void *);
typedef boolean (* crypto_vip_encrypt_t) (void*, ushort *);
typedef boolean (* crypto_vip_decrypt_t) (void*, ushort *);
typedef void (* crypto_engine_encrypt_proc) (ushort, crypto_encrypt_header *);
typedef void (* crypto_engine_decrypt_proc) (ushort, crypto_encrypt_header *);
typedef paktype * (* crypto_engine_reassemble_proc) (paktype *);

typedef struct {
    ushort                	idb_type;
    ushort                	ipc_port_id;                 /* for ipc comm */
    watched_queue             	*expq;                       /* for swepa exp*/ 
    crypto_encrypt_header	*encrypt_que;	     /* for swepa */
    ushort                	encrypt_que_top;             /* for swepa */
    ushort                	encrypt_que_bot;             /* for swepa */
    ushort                	encrypt_que_count;           /* for swepa */
    crypto_swepa_output_t       swepa_output;                          
    crypto_vip_encrypt_t        vip_encrypt;
    crypto_vip_decrypt_t        vip_decrypt;
} crypto_engine_idb_t;

typedef struct crypto_drop_pak_ {
    ulong   count;
    hwidbtype *hwidb;
    struct crypto_drop_pak_ *next;
}crypto_drop_pak_t;

/*   
 * Function proto typing information
 */

void crypto_engine_proc_encrypt_enqueue(crypto_engine_idb_t*, void*); 
void crypto_engine_fast_encrypt_enqueue(crypto_engine_idb_t*, void*); 

epa_status 
epa_engine_req_dspt(ushort, ulong, ushort, epa_crypto_blk *, 
                    epa_callback_handle, epa_callback_context* );
void epa_engine_rsp_dspt(epa_crypto_blk *);

epa_status epa_add_crypto_unit_tbl(ushort, char*);

epa_status epa_del_crypto_unit_tbl(ushort);

epa_status crypto_engine_add_req_pool(ushort,ushort,ushort,ushort, 
		                  epa_callback_context*,
                                  epa_callback_handle );

epa_status crypto_engine_del_req_pool(ushort);
epa_engine_req* crypto_engine_find_req_info(ushort);
epa_crypto_blk* epa_get_blk_buffer(void);
void epa_free_blk_buffer(epa_crypto_blk *);
void crypto_engine_init(ushort, ushort);
void crypto_encrypt_enqueue(crypto_engine_idb_t*,crypto_encrypt_header *);
epa_status crypto_key_enqueue (crypto_engine_idb_t*, void *);
void* crypto_key_dequeue (crypto_engine_idb_t*);
void crypto_dump_data(uchar *, ushort);
void copy_sig_to_pak(uchar *, uchar*, paktype*, uchar*, ushort);
boolean copy_cert_to_buffer(paktype *, uchar *, uchar *, ushort);
void crypto_engine_key_timeout(mgd_timer *);


extern crypto_engine_config_t crypto_engine_tbl[];
extern crypto_engine_idb_t crypto_engine_idb;
extern crypto_engine_config_t crypto_engine_conf;
extern ulong epa_req_id; 
extern ushort epa_conn_crypto_pair_tbl[]; 
extern ulong *epa_des_time;
extern crypto_engine_config_t crypto_engine_conf;
extern void *ip_les_fastswitch;
extern crypto_engine_encrypt_proc crypto_engine_encrypt;
extern crypto_engine_decrypt_proc crypto_engine_decrypt;
extern crypto_engine_reassemble_proc crypto_engine_reassemble;
extern char*hostname;
extern ushort crypto_engine_platform;
extern ulong crypto_swepa_error_pak_count;
extern mgd_timer crypto_engine_master_timer;
extern epa_engine_req epa_engine_req_pool[];

typedef boolean (* ip_les_fast_t)(paktype *);

#endif



