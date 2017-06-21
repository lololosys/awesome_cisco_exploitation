/* $Id: crypto_engine_rsp.h,v 1.1.70.9 1996/08/12 22:59:22 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/rpsp/crypto_engine_rsp.h,v $
 *------------------------------------------------------------------
 * crypto_engine_rsp.h 
 * 
 * This file defines the types and constants specific to RSP.     
 *
 * 11, 1995, xliu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_engine_rsp.h,v $
 * Revision 1.1.70.9  1996/08/12  22:59:22  xliu
 * CSCdi63854:  Traffic is pass in clear text when distributed switching
 * is off
 * Branch: California_branch
 *
 * Revision 1.1.70.8  1996/08/07  08:26:32  snijsure
 * CSCdi65248:  encryption mib doesnt show correct status of ESA.
 * Branch: California_branch
 * Retrieve ESA status using crypto_show_card_status,
 * Also assign proper value to cieEngineCardIndex, using the chassis mib
 * interface.
 *
 * Revision 1.1.70.7  1996/07/28  07:44:15  xliu
 * CSCdi62947:  Add parser command to query epa status from RSP
 * This fix also solve the problems reported by CSCdi63488, CSCdi63852.
 * Branch: California_branch
 *
 * Revision 1.1.70.6  1996/07/15  10:06:42  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 1.1.70.5  1996/07/03  05:54:57  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 1.1.70.4  1996/06/21  22:31:55  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 1.1.70.3  1996/06/17  22:46:48  xliu
 * CSCdi60490:  Crypto RSP router does not encrypt/decrypt packets
 * The switch mode should be int otherwise the decrypted packets are not
 * send
 * out correctly.
 * Branch: California_branch
 *
 * Revision 1.1.70.2  1996/05/12  23:13:12  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.70.1  1996/04/19  14:57:04  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.4.8  1996/04/17  23:48:10  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.4.7  1996/03/22  17:40:30  xliu
 * Branch: IosSec_branch
 * Add ipcache update to crypto map.
 *
 * Revision 1.1.4.6  1996/03/21  10:08:40  xliu
 * Branch: IosSec_branch
 * Integrated with RSP changes.
 *
 * Revision 1.1.4.5  1996/01/30  22:36:07  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 1.1.4.4  1996/01/19  07:01:02  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 1.1.4.3  1996/01/09  01:12:11  xliu
 * Branch: IosSec_branch
 * RSP flow switch support.
 *
 * Moved encryption from datagram_out to ipsendnet, after fragmentation.
 *
 * Revision 1.1.4.1  1995/11/30  05:42:30  xliu
 * Branch: IosSec_branch
 *
 * Files created for crypto engine rsp.
 *
 * Revision 1.1  1995/11/30  05:13:08  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef CRYPTO_ENGINE_RSP_H_
#define CRYPTO_ENGINE_RSP_H_

#include "ttysrv.h"
#include "../if/ether.h"

/* Constants for rsp flow switching */
#define CRYPTO_FLOW_DECRYPT  1
#define CRYPTO_FLOW_DECRYPT_PENDING  2
#define CRYPTO_FLOW_NON_DECRYPT  3

#define CRYPTO_CONN_IDB_CHUNK_SIZE   3000


extern paktype *crypto_fs_pak;
paktype* crypto_rsp_create_pak(bufhdr_t*, void *, ipcache_rntype *);

/* 
 * crypto_rsp_flow_info
 * Most of information here is requried to interface with
 */
typedef struct crypto_rsp_flow_info_ {
    int        switch_mode;
    iphdrtype  *ip;
    hwidbtype  *hwidb;
    paktype    *pak;
    ipflow_t   *flow;
}crypto_rsp_flow_info;

/* Function protocotyping */
int rsp_ipflowswitch(bufhdr_t *, iphdrtype*, hwidbtype*);
int rsp_ipfastswitch(bufhdr_t *, iphdrtype*, hwidbtype*);
void crypto_engine_ps_vec(ushort, paktype *, boolean, boolean);
void crypto_engine_fs_decrypt (paktype *);
void crypto_engine_fastsend (hwidbtype *, paktype *);
void crypto_engine_req_remote_dspt (ushort, void*);
void rsp_eip_fs (bufhdr_t*, hwidbtype *, paktype*);
void rsp_serial_fs (bufhdr_t*, hwidbtype *, paktype*);
int rsp_serial_ip_turbo_fs (bufhdr_t*, hwidbtype *);
int crypto_rsp_ipflow_fs (bufhdr_t *, iphdrtype *, hwidbtype *);
int crypto_rsp_ip_fs(bufhdr_t *, iphdrtype *, hwidbtype *);
void crypto_engine_rsp_upd_vip_fastsend(hwidbtype *);
void crypto_engine_rsp_ipc_init(void);
boolean rsp_holdq_enqueue(bufhdr_t*, hwidbtype *, void *, ushort);
void rsp_fs_free_memd_pak(paktype *);
void crypto_vip_assign_map_to_int (hwidbtype *idb, void *head);
void crypto_vip_remove_map_from_int (hwidbtype *idb, void *head);
void crypto_engine_rsp_ipc_callback(void *);
void crypto_rsp_flow_encrypt_decrypt (void *bp,iphdrtype*, ipcache_rntype*,
                                      ulong, void*); 
void crypto_ipcache_add_callback (ipcache_rntype *entry);
void crypto_initiate_vip_get_conn_tbl (int slot);
void crypto_config_epa (ushort, uchar*, ushort);
void crypto_check_epa_passwd (ushort, uchar*, ushort);
void crypto_engine_show_card_status(ushort, boolean);
void crypto_card_clear_latch(ushort);
boolean crypto_remove_dfs(idbtype*);
void crypto_rsp_restore_media_vectors(hwidbtype *, void *head);
void crypto_add_connection_idb(hwidbtype *, ushort);


paktype *rsp_replicate_pak(paktype *);
#endif

