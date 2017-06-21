/* $Id: crypto_engine_vip.h,v 1.1.68.10 1996/09/10 11:13:35 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/crypto_engine_vip.h,v $
 *------------------------------------------------------------------
 * crypto_engine_vip.h
 * 
 * This file defines the types and constants specific to vip. 
 *
 * 11, 1995, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_vip.h,v $
 * Revision 1.1.68.10  1996/09/10  11:13:35  xliu
 * CSCdi68600:  ESA should not be given the tralier bytes to encrypt
 * Pass the total length (encap len + ip->tl) to ESA driver.
 * Branch: California_branch
 *
 * Revision 1.1.68.9  1996/07/28  11:17:14  xliu
 * CSCdi64306:  crypto engine memory aligment error on vip2
 * Branch: California_branch
 *
 * Revision 1.1.68.8  1996/07/22  07:38:56  xliu
 * CSCdi63492:  PPP does not work when switched from HDLC on EPA/VIP2
 * Branch: California_branch
 *
 * Revision 1.1.68.7  1996/07/15  09:59:17  xliu
 * CSCdi61651:  epa randome number seed is not initialized
 * Branch: California_branch
 *
 * Revision 1.1.68.6  1996/07/06  19:45:21  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.68.5  1996/06/21  22:32:39  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.68.4  1996/06/01  01:15:02  xliu
 * CSCdi59234:  crypto rsp flow vector does not work correct for vip
 * encryption
 * Branch: California_branch
 *
 * Revision 1.1.68.3  1996/05/28  16:00:07  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.68.2  1996/05/12  23:13:33  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.68.1  1996/04/19  14:57:13  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.8  1996/04/17  23:48:28  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.2.7  1996/03/30  05:12:30  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 1.1.2.6  1996/03/25  23:19:20  xliu
 * Branch: IosSec_branch
 * Checkin VIP crypto engine code.
 *
 * Revision 1.1.2.5  1996/01/30  22:36:17  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.2.4  1996/01/09  02:26:50  xliu
 * Branch: IosSec_branch
 * VIP software encryption.
 *
 * Revision 1.1.2.3  1995/12/10  09:30:06  xliu
 * Branch: IosSec_branch
 * Merge the code to replace the idb fastsend using ipc.
 *
 * Revision 1.1.2.2  1995/12/02  17:42:59  xliu
 * Branch: IosSec_branch
 *
 * Modified epa driver interface.
 *
 * Revision 1.1.2.1  1995/11/30  05:39:52  xliu
 * Branch: IosSec_branch
 *
 * Files created for crypto engine vip.
 *
 * Revision 1.1  1995/11/30  05:23:31  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef CRYPTO_ENGINE_VIP_H_
#define CRYPTO_ENGINE_VIP_H_

#define CRYPTO_HWEPA_NORMAL 0x0001
#define CRYPTO_SWEPA_PRESENT	 0x0002
#define CRYPTO_HWEPA_NOT_PRESENT 0x0004

#define CRYPTO_ENGINE_SELF  2

#define CRYPTO_ENGINE_VIP_IPC_PORT_NAME "crypto_engine_vip_ipc_port"

#define CRYPTO_CONN_UNSET     0
#define CRYPTO_CONN_SET       1

#define CRYPTO_VIP_KEY_CONTEXT_POOL_SIZE    100
#define CRYPTO_VIP_KEY_REQ_BLK_BUFFER_SIZE  100

#define CRYPTO_REG_KEY_REQ  1
#define CRYPTO_NON_REG_KEY_REQ 2

#define CRYPTO_VIP_KEY_REQ_ENTRY_SET  1
#define CRYPTO_VIP_KEY_REQ_ENTRY_UNSET 0

#define CRYPTO_EPA_KEY_RING 1
#define CRYPTO_EPA_ENCRYPT_RING 0

#define CRYPTO_ENGINE_REGIST_KEY_REQ 1
#define CRYPTO_ENGINE_NON_REGIST_KEY_REQ 0

#define CRYPTO_EPA_ENCRYPT_KEY_UNSET 0x00
#define CRYPTO_EPA_IV_KEY_UNSET      0x00
#define CRYPTO_EPA_ENCRYPT_KEY_SET   0x01
#define CRYPTO_EPA_IV_KEY_SET        0x02
#define CRYPTO_EPA_SESSION_SET       0x03
#define CRYPTO_EPA_SESSION_UNSET     0x00

#define CRYPTO_ENGINE_VIP_EPA_PASSWD  "CISO_EPA"

#define CRYPTO_CONN_UNSET     0
#define CRYPTO_CONN_SET       1

#define CRYPTO_VIP_SIGNATURE_BUFFER  350

/* 
 * crypto_encrypt_control_block
 * The control information needed by epa firmware.
 */

typedef struct {
    uchar     command;
    uchar     status;
    ushort    session;    /* hma handle */
    uchar     iv[8];
    uchar     reserved1[8];
    ushort    offset;
    ushort    total_length;
} crypto_encrypt_control_blk;

/*
 * crypto_callback_info
 * Callback struct used by epa driver to communicate with crypto_engine.
 *
 */

typedef struct {
    ushort     type;
    ushort     status;
} crypto_callback_info;

/*
 * crypto_init_callback_info
 * Specifies the info returned from epa driver during the initialization.
 */
typedef struct {
    ushort	status;		   /* hwepa status */
    hwidbtype*  epa_encrypt_idb;   /* idb to encrypt */
    hwidbtype*  epa_key_idb;       /* idb to key mgt */
} crypto_init_callback_info;

typedef struct {
    int         switch_mode;
    iphdrtype   *ip;
    hwidbtype   *hwidb;
    paktype     *pak;
    encapinfotype  *eptr;
    int         encap_len;
    ulong       iplen;
    void        *output;
    ipflow_t    *flow;
} crypto_vip_pak_info;

typedef void (*encrypt_t)(short, crypto_vip_pak_info*, ushort *);
typedef void (*decrypt_t)(short, crypto_vip_pak_info*, ushort*);

/*
 * crypto_engine_vip_conf
 * crypto engine vip configuration info
 */
typedef struct {
    ushort     epa_type;
    uint       epa_serial_number;
    ushort     hwepa_status;
    hwidbtype  *epa_key_idb;
    hwidbtype  *epa_encrypt_idb;
    encrypt_t  encrypt;
    decrypt_t  decrypt;
    ipc_port_id ipc_port;
    watched_queue *dspq; 
} crypto_engine_vip_conf;

/* crypto_engine_fastsend
 *
 * Function protocol type header/
 */
void crypto_engine_fastsend (paktype*, hwidbtype *);

#define CRYPTO_EPA_ENCRYPT	0x00
#define CRYPTO_EPA_DECRYPT      0x80 
#define CRYPTO_EPA_TEST2        0x20

/* The packet has been successfuly decrypted */
#define CRYPTO_EPA_DECRYPT_DONE    0x01     

/* The epa driver complete initialization    */
#define CRYPTO_EPA_INIT_DONE          0x02

/* EPA encryption/decryption error happens */
#define CRYPTO_EPA_ENCRYPTION_ERROR   0x03

#define CRYPTO_ENGINE_VIP_KEY_INPUT_MAX_SIZE   300

typedef struct {
    uchar  command;
    uchar  status;
    uchar  data[CRYPTO_ENGINE_VIP_KEY_INPUT_MAX_SIZE - 2];
} crypto_engine_key_comm_buffer_t;     

typedef struct {
    ushort connection_id;
    ushort hma_dh_shared;
    ushort hma_encrypt_key;
    ushort hma_iv_key;
    ushort hma_session_id;
    ushort encrypt_method;
    ushort state;
    uchar  hma_encrypt_key_state;
    uchar  hma_iv_key_state;
    uint   encrypt_count;
    uint   decrypt_count;
    hwidbtype *hwidb;
} crypto_vip_conn_entry;

typedef struct {
    ushort conn_id;
    ushort command;
    ushort req_id;
    ushort state;
    ushort hold_count;
    ulong  timeout;
    epa_crypto_blk *blk;
} crypto_vip_key_req_context;

typedef struct {
    ushort  dh_pri_handle;
    ushort  dh_shr_handle;
    ushort  state;
} crypto_engine_vip_dh_entry;

typedef struct {
    ipc_ipfast_msg_hdr hdr;
    int		target;
    ushort     	command;
} crypto_vip_fast_hdr;

typedef struct {
    ushort  size;
    uchar   passwd[CRYPTO_EPA_PASSWD_MAX_SIZE];
}epa_passwd_t;

static inline uint get_vip_serial_number (void)
{
    /* Curretnly we have not found a routine to return vip serila number.
     * Will be replaced by such routine.
     */
    return 0x1111;
}

/*
 * Function prototype
 */
boolean crypto_vip_epa_encrypt (void *, ushort *);
boolean crypto_vip_epa_decrypt (void *, ushort *);
boolean crypto_vip_sw_encrypt (void *, ushort *);
boolean crypto_vip_sw_decrypt (void *, ushort *);
void crypto_engine_send_key_response (epa_crypto_blk *);
void crypto_register_idb(ushort, hwidbtype*);
void crypto_engine_vip_sign (paktype *, hwidbtype *);
void crypto_engine_init (ushort, ushort);
crypto_engine_vip_conf crypto_engine_vip_config;
void crypto_swepa_vip_output(ushort, void*);
void crypto_engine_vip_parser_init (void);
void crypto_engine_vip_fastsend (hwidbtype *, paktype *);
void crypto_swepa_initialization(void);
void crypto_sw_vip_encrypt_decrypt (ushort, crypto_encrypt_header *);
void crypto_engine_vip_free_pak (paktype *);
void crypto_key_rsp_dspt(void);
void crypto_send_epa_key_request(epa_crypto_blk *);
void crypto_engine_vip_upd_map(uchar *);
void crypto_engine_vip_upd_idb(uchar *);
void crypto_engine_recv_key_request(uchar *);
void crypto_map_free(crypto_maptype *map);
void crypto_map_head_free(crypto_map_headtype *head);
crypto_map_headtype *crypto_map_head_add (char *map_tag, ushort);
crypto_maptype *crypto_map_add(crypto_map_headtype *head, int seq_no);
void crypto_map_head_delete(char *tag, ushort);
boolean crypto_map_delete(crypto_map_headtype *head,
                                 crypto_maptype *map);
inline crypto_map_headtype *vip_find_crypto_map_head(uchar *name,ushort size);
inline crypto_maptype *find_crypto_map(crypto_map_headtype *head,
                                              int seq);
uint crypto_engine_check_decryption_or_cs(paktype *, hwidbtype *);
void crypto_vip_show_map(void);
void crypto_show_idb(void);
uint vip_logical_slot(void);
void crypto_engine_vip_fastsend (hwidbtype *idb, paktype *pak);
void crypto_engine_vip_decrypt (hwidbtype *, paktype *, ushort *);
void vip_set_sig_params (paktype*, crypto_sig_params *);
boolean crypto_engine_vip_pak_dfs (hwidbtype *, paktype *);
uint crypto_engine_check_decryption_or_cs(paktype *, hwidbtype *);
boolean crypto_vip_ip_encaps_generic(paktype*, encapinfotype *, hwidbtype *,
                                     void *, int, ulong, void*);
void crypto_dump_particles (paktype *, ushort);
void pak_copy_scatter_to_contiguous(paktype*, paktype*, uchar*, uchar*, ushort);

/*
 * External
 */
extern ulong crypto_engine_vip_key_req_id;
extern crypto_engine_vip_conf crypto_engine_vip_config;
extern crypto_vip_conn_entry crypto_vip_conntbl[];
extern crypto_vip_key_req_context crypto_vip_key_req_context_pool[];
extern ipc_port_id crypto_engine_rsp_crnt_port; 
extern epa_crypto_blk crypto_engine_vip_blk_buffer[];
extern ushort crypto_engine_vip_blk_index;
extern uchar crypto_engine_vip_epa_passwd[];
extern ushort crypto_engine_vip_dss_p_handle;
extern crypto_engine_vip_dh_entry crypto_engine_vip_dh_tbl[];
extern chunk_type *crypto_map_head_chunk;
extern chunk_type *crypto_map_chunk;
extern chunk_type *crypto_map_tag_chunk;
extern chunk_type *crypto_vector_chunk;
extern chunk_type *crypto_key_blk_chunk;
extern crypto_engine_vip_conf crypto_engine_vip_config;
extern epa_passwd_t epa_passwd;
#endif

