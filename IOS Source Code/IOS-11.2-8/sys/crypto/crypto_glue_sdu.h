/* $Id: crypto_glue_sdu.h,v 3.1.70.6 1996/09/11 23:25:03 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_glue_sdu.h,v $
 *------------------------------------------------------------------
 * 
 * crypto_glue_sdu.h - Externs for Cylink SDU->IOS glue functions.
 *
 * November 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_glue_sdu.h,v $
 * Revision 3.1.70.6  1996/09/11  23:25:03  bew
 * CSCdi68438:  crypto determination of network address should be done
 * earlier
 * Branch: California_branch
 * Run the routing tables during connection message setup to determine
 * which interface the packet will go out.
 *
 * Revision 3.1.70.5  1996/09/10  23:28:37  bew
 * CSCdi64360:  Bad and pending connection does not timeout
 * Branch: California_branch
 * Clear out the pending connection table entry as well
 * as connection table entry when manually clearing a connection.
 *
 * Revision 3.1.70.4  1996/08/28  19:29:06  bew
 * CSCdi66328:  crypto connection set too early on CRM side
 * Branch: California_branch
 * Use the pending connection id on the CRM side to tag crypto maps. The
 * pending conn id is replaced by connection id when connection
 * established as on initiating side.
 *
 * Revision 3.1.70.3  1996/07/06  19:44:29  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.1.70.2  1996/06/05  23:11:26  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 3.1.70.1  1996/04/19  14:55:46  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.5  1996/04/16  21:52:14  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 3.1.2.4  1996/03/13  22:45:07  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 3.1.2.3  1996/01/25  18:19:53  bew
 * Branch: IosSec_branch
 * Implemented a config option for automatic pre-genereration of
 * Diffie-Hellman key pairs. Also changed the use of timers so
 * that the the STUCKTMR message doesn't show up any more.
 *
 * Revision 3.1.2.2  1996/01/13  05:46:42  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 3.1.2.1  1995/11/28  01:46:31  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/22  23:17:41  bew
 * Placeholders for new locations of crypto parser files
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Externs
 */
extern uchar *crypto_glue_umSduGetDssPublic(uchar *, uchar *, CIBptr);

extern uchar crypto_glue_good_signature( uchar *, uchar *, short, uchar *);

extern int GetRTClock(ulong *);

extern ulong crypto_glue_set_cs_unique_conn_id(ushort);

extern boolean crypto_glue_generate_DH_public_number(uchar *, short, ushort);

extern boolean crypto_glue_generate_DH_shared_number(uchar *, short, ushort);

extern boolean crypto_glue_generate_syndrome(short,ushort,short,short,uchar *);

extern boolean crypto_glue_crypto_sign(ushort, uchar *, ushort, uchar *);

extern void crypto_glue_insert_node_id(uchar *, EntityID *);

extern void crypto_glue_timeout_keys(CIBptr);

extern void *taskAcquireBuffer(void);

extern void taskReleaseBuffer(Cy2TxFrameControl *);

extern boolean crypto_glue_set_crypto_method(CIBptr);

extern void crypto_glue_cleanup_cib(CIBptr);

extern void crypto_glue_precalc_dh(void);

extern void crypto_glue_setup_cib(CIBptr, void *);

extern void crypto_glue_clear_conn_id(int, int, ushort);

extern void crypto_glue_xchg_connection_ids(CIBptr, boolean, long);

extern void crypto_glue_clear_pending_index(CIBptr);

extern void crypto_glue_get_network_address(ulong *, long);

extern boolean crypto_glue_is_my_snum(ulong);
 
extern ulong crypto_glue_get_sn_by_slot(ushort);

extern ushort crypto_glue_get_sdu_connections(void);
 
extern ushort crypto_glue_get_sdu_entities(void);
 

