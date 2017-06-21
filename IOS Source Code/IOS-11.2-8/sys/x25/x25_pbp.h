/* $Id: x25_pbp.h,v 3.2 1995/11/17 18:07:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/x25_pbp.h,v $
 *------------------------------------------------------------------
 * X25_PBP.H
 *
 * Feburary 11, 1994 Bill Thomas
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: x25_pbp.h,v $
 * Revision 3.2  1995/11/17  18:07:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:22:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __X25_PBP_H__
#define __X25_PBP_H__
#include "../wan/stac_comp.h"
#include "x25.h"

typedef struct {
    uchar version;	/*version*/
#define X25_PBP      0x40       /*differentate from compress predictor*/
#define X25_PBP_STAC (1 | X25_PBP)

    uchar xmt_paks;	/*sequence number*/
    ushort org_size;	/*packet size before compression*/
} x25_pbp_hdr_t;

#define X25_PBP_PROTOCOL_ID 0xD9

#define X25_PBP_HEADERBYTES  4 /* sizeof(x25_pbp_hdr_t) */ 
/*decompression stac can't stop on a dime.  need X25_PBP_STAC_FUDGE extra
 *space on a getbuffer
 */
#define X25_PBP_STAC_FUDGE    (LZS_DEST_MIN + 1)
/*used to determine far empty slots in lci_holdQ*/
#define X25_PBP_MODULO 8
#define X25_PBP_WINDOW 7

#define X25_PBP_MAX_RNR_TICKS 5 /*send X25 RESET after 5 ticks*/

extern boolean x25_pbp_compress (hwidbtype *, lcitype *, paktype *, paktype **);
extern boolean x25_pbp_decompress (paktype *, paktype *, uchar *, lcitype *,
					ulong *);
extern void x25_pbp_reset (lcitype *);
extern void no_x25_pbp_compression (lcitype *);
extern void x25_pbp_clearcounters (lcitype *);
extern boolean x25_pbp_init (lcitype *, boolean );
extern void x25_pbp_show_stats (lcitype *);
extern void x25_pbp_timer (void);

extern ushort x25_pbp_switch(boolean , lcitype *);

/* x25_pbp_switch return values */
#define X25_PBP_SWITCH_FAILED 0
#define X25_PBP_SWITCH_OK 1
#define X25_PBP_SWITCH_ER 2

extern boolean x25_pbp_unacked (hwidbtype *, lcitype *);
extern void x25_pbp_update_lci_ack (hwidbtype *, lcitype *);
extern boolean x25_pbp_inner_node_receive (lcitype *, packettype *);
extern boolean x25_pbp_check_pr (lcitype *, int );
extern void x25_pbp_flush_call (lcitype *);
extern void x25_pbp_extract_pr_ps (lcitype *, packettype *, int *, int *);
extern void x25_pbp_stuff_pr_ps (lcitype *, packettype *);
extern void x25_pbp_finish_ack (lcitype *, paktype *);
extern paktype *x25_pbp_begin_ack (hwidbtype *, lcitype *);
extern void x25_pbp_common_reset (lcitype *);
extern boolean x25_pbp_restriction_checks (lcitype *, hwidbtype *);

extern boolean x25_switching_pbp_compress;
extern ulong x25_pbp_stac_history_size;

#endif __X25_PBP_H__
