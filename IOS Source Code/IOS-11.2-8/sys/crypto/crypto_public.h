/* $Id: crypto_public.h,v 3.1.58.12 1996/09/15 15:47:45 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_public.h,v $
 *------------------------------------------------------------------
 * crypto definitions public to IOS.
 *
 * Jan, 1996, xliu 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_public.h,v $
 * Revision 3.1.58.12  1996/09/15  15:47:45  xliu
 * CSCdi69200:  RSP crypto image  crashed when dram buffer allocation
 * return NULL
 * Branch: California_branch
 * Add macro definition.
 *
 * Revision 3.1.58.11  1996/09/10  14:00:40  sbelair
 * CSCdi67685:  Unable to fast-switch packets through encrypted tunnels
 * Branch: California_branch
 *
 * Define new switch mode to be used ater decryption for tunnel
 * fast-switching,
 * and define a new crypto vector to be invoked for decryption in tunnel
 * fast-
 * switching which will use the new switch mode.
 *
 * Revision 3.1.58.10  1996/08/28  19:29:13  bew
 * CSCdi66328:  crypto connection set too early on CRM side
 * Branch: California_branch
 * Use the pending connection id on the CRM side to tag crypto maps. The
 * pending conn id is replaced by connection id when connection
 * established as on initiating side.
 *
 * Revision 3.1.58.9  1996/08/17  01:34:20  xliu
 * CSCdi64300:  Need to investigate possible RSP memd buffer corruption
 * Branch: California_branch
 *
 * Revision 3.1.58.8  1996/07/15  10:05:13  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.1.58.7  1996/07/06  19:44:37  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.1.58.6  1996/06/21  22:29:23  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 3.1.58.5  1996/06/07  00:24:19  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.58.4  1996/06/05  23:11:34  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 3.1.58.3  1996/05/28  15:58:58  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 3.1.58.2  1996/05/12  23:12:24  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.1.58.1  1996/04/19  14:55:57  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.8  1996/04/17  23:47:30  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 3.1.2.7  1996/03/30  05:11:36  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 3.1.2.6  1996/03/26  09:20:00  xliu
 * Branch: IosSec_branch
 * Add more constants.
 *
 * Revision 3.1.2.5  1996/03/21  10:07:07  xliu
 * Branch: IosSec_branch
 * Migrated to CA branch.
 *
 * Revision 3.1.2.4  1996/03/01  01:39:10  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 3.1.2.3  1996/01/30  22:35:45  xliu
 * Branch: IosSec_branch
 * Checking in encryption RSP/VIP support code.
 *
 * Revision 3.1.2.2  1996/01/19  07:00:38  xliu
 * Branch: IosSec_branch
 * Commit before the second sync.
 *
 * Revision 3.1.2.1  1996/01/04  19:16:29  xliu
 * Branch: IosSec_branch
 * Crypto moudlization.
 *
 * Revision 3.1  1996/01/04  19:03:21  xliu
 * Place holder file.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef CRYPTO_PUBLIC_H_
#define CRYPTO_PUBLIC_H_

/*
 * Flags for pak->cryptoflags.
 */
#define CRYPTO_DONE             0x0001
#define CRYPTO_SIGN_ME          0x0002
#define CRYPTO_VERIFY_DONE      0x0004
#define CRYPTO_VERIFY_FAILED    0x0008
#define CRYPTO_DECRYPT          0x0010
#define CRYPTO_ENCRYPT          0x0020
#define CRYPTO_ENCRYPT_DONE     0x0040
#define CRYPTO_DECRYPT_DONE     0x0080
#define CRYPTO_ACL_LOG  	0x0100
#define CRYPTO_DENY		0x0200
#define CRYPTO_CM		0x0400
#define CRYPTO_CONN_NON_EXIST   0x0800

#define CRYPTO_CONN_MAXIMUM 299
#define CRYPTO_CONN_UNSET   0
#define CRYPTO_CONN_MINIMUM -100000
#define CRYPTO_CONN_PENDING CRYPTO_CONN_MINIMUM - 2
#define CRYPTO_CONN_BAD     CRYPTO_CONN_MINIMUM - 3
#define CRYPTO_CONN_BLOCKED CRYPTO_CONN_MINIMUM - 4
#define CRYPTO_CONN_ERROR   CRYPTO_CONN_MINIMUM - 5

#define CRYPTO_ICMP_CODE 74

#define CRYPTO_VIP_DECRYPT         0x1001
#define CRYPTO_VIP_DECRYPT_PENDING 0x1002
#define CRYPTO_VIP_NO_DECRYPT      0x1004
#define CRYPTO_VIP_CONN_SETUP      0x1008
#define CRYPTO_VIP_CONTINUE_FLOW   0x1010
#define CRYPTO_VIP_FRAGMENT        0x1020

#define CRYPTO_OPTIMUM_FS          0x01
#define CRYPTO_FLOW_FS             0x02

#define CRYPTO_WRITETHROUGH_CACHE_LINES 64

/*
 * vector function declaration for crypto vectors.
 */
typedef void (* crypto_rsp_flow_vec_t)(void *, iphdrtype*, ipcache_rntype*,
                                       ulong, hwidbtype*, hwidbtype*, void *);
typedef void (* crypto_vip_flow_vec_t)(hwidbtype *, paktype *, ushort *);
typedef void (* crypto_ps_vec_t)(ushort, paktype *, boolean, boolean);
typedef void (* fast_decrypt_t)(paktype *);
typedef boolean (* media_pak_ddfs_t )(hwidbtype *, paktype *);

/*
 * crypto engine idb vector
 *
 */
typedef struct {
    void*                 media_ip_turbo_fs;
    fastsend_t            media_fastsend;  /* only used by vip */
    void*                 media_pak_dfs;  
    crypto_ps_vec_t       ps_vec;          /* vector to encrypt at proc level */
    fastsend_t            fs_vec;          /* fastsend vector */
    fastsend_t            tunnel_fs_vec;   /* tunnel fastswitch vector */
    crypto_rsp_flow_vec_t crypto_rsp_flow; /* rsp flow encrypt/decrypt vector */
    crypto_vip_flow_vec_t vip_flow_decrypt;/* vector to decrypt for vip flow
                                              * switch */
} crypto_vector_t;

#define IS_ENCRYPTION(x)((x)->status & IDB_ENCRYPT)

#define crypto_rsp_host_encrypt(input, output, cptr)(\
                 !(is_vip(input) && IS_ENCRYPTION(input)) && \
                 !(is_vip(output) && IS_ENCRYPTION(output)) && \
                 (cptr)->crypto_encap)

#define crypto_deny_packet(x)(((x)->cryptoflags & CRYPTO_DENY) || ((x)->cryptoflags & CRYPTO_CONN_NON_EXIST))

#endif


