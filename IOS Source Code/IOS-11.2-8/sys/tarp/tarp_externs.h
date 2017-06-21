/* $Id: tarp_externs.h,v 3.3.62.2 1996/05/17 21:03:30 asastry Exp $
 * $Source: /release/112/cvs/Xsys/tarp/tarp_externs.h,v $
 *------------------------------------------------------------------
 * TARP (Target ID address Resolution Protocol)
 *
 * October 1995, Arun Sastry
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tarp_externs.h,v $
 * Revision 3.3.62.2  1996/05/17  21:03:30  asastry
 * CSCdi57772:  Missed out a line in the log; add it.
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/05/17  20:55:41  asastry
 * CSCdi57772:  Fix misc. TARP bugs found during interoperability testing
 *
 *               - Don't change NSEL during TARP packet propagation
 *               - Implement loop detection as per amended TARP spec
 *               - Don't propagate TARP packet if TTL is zero (after
 *                 decrement)
 *
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/20  19:19:19  asastry
 * CSCdi43154:  TARP corrupted memory pointer. Fix pdb corruption bug. In
 * addition, incorporate these other misc. fixes:
 *
 *
 *         - Fix spurious TARP interface command during NVGEN
 *         - Reset sequence number to zero before sending TARP Type 4 PDU
 *         - Accept TARP PDU with sequence number of zero, regardless of
 *           whether there is an entry in Loop Detection Buffer
 *         - Zero out NSAP field in TARP TID cache if CLNS NET is deleted
 *           from underneath TARP
 *         - create TID entry with a zero NSAP if a TARP TID is entered
 *           before assigning a CLNS NET
 *
 * Revision 3.2  1995/11/17  18:44:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:33:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "tarp.h"

#ifndef __TARP_EXTERNS_H__
#define __TARP_EXTERNS_H__

/* tarp.c */

extern void tarp_init_idb(idbtype *, boolean);
extern watched_queue *tarpQ;
extern queuetype tarp_static_adjQ;
extern queuetype tarp_rqstQ; 
extern boolean tarp_running;
extern boolean tarp_propagate_flag;
extern boolean tarp_originate_flag;
extern boolean tarp_allow_cache_flag;
extern ulong tarp_traffic[];
extern char *tarp_my_tid;
extern ushort tarp_global_sequence_num;
extern uchar tarp_global_nsel;
extern uchar tarp_global_urc;
extern ushort tarp_lifetime;
extern uchar tarp_global_protocol_type;
extern uint tarp_ldb_flush_timer;
extern uint tarp_tid_cache_timer;
extern uint tarp_t1_resp_timer;  
extern uint tarp_t2_resp_timer;
extern uint tarp_zero_seq_timer;
extern uint tarp_arp_timer; 
extern uint tarp_post_t2_timer;
extern sys_timestamp tarp_type4_timer;

extern void tarp_start_process(void);
extern void tarp_init(subsystype *);
extern void tarp_enqueue(paktype *);
extern boolean tarp_running_status(void);
extern uchar tarp_zero_nsap[];

/* tarp2.c */
extern void tarp_originate_packet(uchar, uchar, uchar *, uchar *,
				  uchar *, uchar *);
extern void tarp_handle_type1_2_pdu(tarp_pdutype *, tarp_tid_cache_type *);
extern void tarp_handle_type3_pdu(tarp_pdutype *);
extern void tarp_handle_type4_pdu(tarp_pdutype *, clns_adjtype *, boolean);
extern void tarp_handle_type5_pdu(tarp_pdutype *);
extern void tarp_propagate(tarp_pdutype *, uchar, boolean, clns_adjtype *);
extern void tarp_propagate_new_info(uchar *, uchar *, uchar *);

/* tarp_chain.c */

extern void tarp_parser_init(void);

/* tarp_misc.c */

extern tarp_ldb_type *tarp_find_ldb_entry(uchar *);
extern void tarp_update_ldb_entry(tarp_ldb_type *, ushort, uint);
extern void tarp_update_tid_entry(uchar *, ushort, uchar, uchar *, ushort,
				  boolean, uchar);
extern tarp_ldb_type *tarp_insert_ldb_entry(ushort, uchar *);
extern void tarp_clean_up(void);
extern void tarp_ldb_ager(void);
extern void tarp_tid_ager(void);
extern tarp_tid_cache_type *tarp_walk_tid_table(int *, tarp_tid_cache_type *);
extern tarp_ldb_type *tarp_walk_ldb_table(int *, tarp_ldb_type *);
extern tarp_tid_cache_type *tarp_find_tid_entry(uchar *, int,
						enum TARP_TID_NET_KEY);
extern void tarp_delete_one_tid_entry(uchar *);
extern void tarp_clear_tid_table(boolean);
extern void tarp_build_address_string(tarp_pdutype *, uchar *);
extern uchar *tarp_addr_string(tarp_pdutype *, uchar *);
extern void tarp_build_tid_string(tarp_pdutype *, uchar *, enum TARP_SRC_DST);
extern void tarp_extract_area(uchar *, uchar *);
extern uchar *tarp_entry_type(tarp_tid_cache_type *);
extern void tarp_check_type4_timer(void);
extern void tarp_start_type4_timer(void);
extern enum TARP_HOST_TYPE tarp_get_host_status(clns_pdbtype **, uchar **);

/* tarp_parse.c */

extern tarp_static_adjQ_type *tarp_search_static_adjQ(uchar *,
						    enum TARP_ADJ_ENTRY_TYPE);

extern void tarp_update_static_adjQ(uchar *, enum TARP_ADJ_ENTRY_TYPE,
				    uchar *, boolean);
extern void tarp_modify_router_tid(uchar *, boolean);

#endif __TARP_EXTERNS_H__

