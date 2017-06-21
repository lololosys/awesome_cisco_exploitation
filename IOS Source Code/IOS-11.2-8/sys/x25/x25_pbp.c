/* $Id: x25_pbp.c,v 3.2.60.4 1996/08/28 13:23:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_pbp.c,v $
 *------------------------------------------------------------------
 * X25_PBP.C
 *
 * Feburary 11, 1994 Bill Thomas
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: x25_pbp.c,v $
 * Revision 3.2.60.4  1996/08/28  13:23:37  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.3  1996/07/23  13:32:39  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.2.60.2  1996/06/28  23:32:51  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/04/11  21:23:49  turadek
 * CSCdi51869:  Router hangs when hdlc stacker serial is dis/ena repeatedly
 * Branch: California_branch
 * Prevent decompression of zero-length packets.
 *
 * Revision 3.2  1995/11/17  18:07:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/20  07:38:25  hampton
 * Convert X.25 to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37392]
 *
 * Revision 2.2  1995/06/20 20:39:01  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:22:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../x25/msg_x25.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "lapb.h"
#include "x25.h"
#include "x25_address.h"
#include "x25_switch.h"
#include "x25_pbp.h"
#include "compress.h"

boolean x25_switching_pbp_compress = FALSE;

boolean x25_pbp_compress (hwidbtype * hwidb, lcitype * lci, paktype * pak,
			     paktype ** newpak_pp)
{
    ushort encsize;
    x25_pbp_hdr_t *x25_pbp_hdr_p;
    uchar *src_p, *dest_p;
    uchar *src_p_wk, *dest_p_wk;
    ulong src_len, src_len_wk;
    ulong max_dest_len = 0;
    ulong max_dest_len_wk = 0;
    paktype *newpak;
    x25paktype *p_new, *p_old;
    ulong status;

    if (hwidb->x25_modulo == X25_MOD8)
	encsize = X25HEADERBYTES + LAPB_OVERHEAD(hwidb);
    else
	encsize = X25EHEADERBYTES + LAPB_OVERHEAD(hwidb);

    if ((src_len = pak->datagramsize - encsize) <= 0) {
	return (FALSE);
    }
    src_p = pak->datagramstart + encsize;

    max_dest_len = src_len + src_len/8 + X25_PBP_STAC_FUDGE;

    *newpak_pp = newpak = getbuffer(max_dest_len + X25_PBP_HEADERBYTES);
    if (newpak == NULL) {
	return (TRUE);
    }
    pak_copy(pak, newpak, 0);/*in effect does datagramstart -= encsize*/

    /* x25_pbp_header is in the encap area. its lapb, x25, x25_pbp headers
     * followed by the line and payload (adjusted so that 1st byte of lapb
     * is on an even byte boundary).
     */
    newpak->datagramstart -= X25_PBP_HEADERBYTES;

    /* les drivers have problems with pak's on odd byte boundaries */
    newpak->datagramstart = (uchar *) (((ulong) newpak->datagramstart) &
				       BUFFER_WORD_ALIGN);

    p_old = (x25paktype *) (pak->datagramstart + LAPB_OVERHEAD(hwidb));
    p_new = (x25paktype *) (newpak->datagramstart + LAPB_OVERHEAD(hwidb));
    p_new->x25GfiLcg = p_old->x25GfiLcg;
    p_new->x25Lcn = p_old->x25Lcn;
    p_new->x25Pti = p_old->x25Pti & ~X25_MBIT_MASK;

    x25_pbp_hdr_p = (x25_pbp_hdr_t *) (newpak->datagramstart + encsize);

    dest_p = newpak->datagramstart + encsize + X25_PBP_HEADERBYTES;

    max_dest_len_wk = max_dest_len;
    /* LZS_Compress() modifies src_p_wk, dest_p_wk & src_len_wk */
    src_p_wk = src_p;		/* dont matter. be safe   */
    dest_p_wk = dest_p;		/* dont matter. be safe   */
    src_len_wk = src_len;	/* this one we can't loose */
    status = LZS_Compress(&src_p_wk, &dest_p_wk, &src_len_wk,
		 &max_dest_len_wk, lci->lci_pbp_comp_stac,
		 LZS_SOURCE_FLUSH | LZS_DEST_FLUSH |
		 LZS_SAVE_HISTORY | LZS_PERFORMANCE_MODE_1, 3);

    newpak->datagramsize = X25_PBP_HEADERBYTES + (ushort) (max_dest_len - 
							   max_dest_len_wk);
    if ((status != LZS_INVALID) && !(status & LZS_DEST_EXHAUSTED)) {
      newpak->datagramsize = X25_PBP_HEADERBYTES + (ushort) (max_dest_len - 
							     max_dest_len_wk);
      
      lci->lci_pbp_xmt_in_bytes += src_len;
      
      newpak->datagramsize += encsize;
      
      x25_pbp_hdr_p->version = X25_PBP_STAC;
      PUTSHORT(&x25_pbp_hdr_p->org_size, (ushort) src_len);
      x25_pbp_hdr_p->xmt_paks = lci->lci_pbp_xmt_sync;
      
      if (++lci->lci_pbp_xmt_sync == 0)
	lci->lci_pbp_xmt_sync = 1;
      
      return (TRUE);
    } else {
      x25_sendcause(lci->lci_idb->hwptr, lci, X25_RESET,
		X25_CLEAR_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
      errmsg(&msgsym(COMPERR, X25), lci->lci_idb->namestring, lci->lci_lci);
      return (FALSE);
    }
}				/* end x25_pbp_compress() */

/*
 * x25_pbp_decompress()
 * INPUT: *pak pointer to packet to be decompressed.
 *	  *newpak pointer to packet to decompress into.
 *	  *dest_p pointer where in newpak to start decompressing into.
 *	  *lci pointer to lci structure for this LCN. 
 * RETURNS: ret_dest_len pointer which returns the number of
 *		bytes decompressed into newpak. 
 *	    boolean:
 *		TRUE if number of decompressed bytes is not zero and
 *		     all of the source compressed bytes are decompressed,
 *		else FALSE. 
 */ 
boolean x25_pbp_decompress (paktype * pak, paktype *newpak, uchar *dest_p, 
			  lcitype * lci, ulong *ret_dest_len)
{
    uchar *src_p;
    ulong src_len, max_dest_len, max_dest_len_wk;
    ulong dest_len;

    src_p = pak->datagramstart;
    src_len = pak->datagramsize;

    if ( (long)src_len<=0 ) {		/* 0-length incoming pak? */
	pak->flags |= PAK_ABORT;
	return (FALSE);
    }

    /*max_dest_len is the max length available for the decompression
     *of this packet and following packets of the segment chain.
     *(ulong) (dest_p - newpak->datagramstart) represent the
     *size of previously decompressed packets in this segmented
     *chain.
     */
    max_dest_len = X25_PBP_STAC_FUDGE + (ulong)(newpak->datagramsize) - 
			(ulong)(dest_p - newpak->datagramstart);

    max_dest_len_wk = max_dest_len;

    LZS_Decompress(&src_p, &dest_p, &src_len, &max_dest_len_wk,
		   lci->lci_pbp_decomp_stac, LZS_SAVE_HISTORY);

    *ret_dest_len = dest_len = max_dest_len - max_dest_len_wk;
    /*special case occurs when the last packet has only stac control
     *bytes, which will result in ret_dest_len == 0.  these bytes
     *must be run through the decompressor.
     */
    if (src_len != 0)  
	return (FALSE);

    lci->lci_pbp_rcv_out_bytes += dest_len;

    return (TRUE);
}				/* end x25_pbp_decompress() */

void x25_pbp_common_reset (lcitype *lci)
{
    paktype *pak;

    if (lci->lci_pbp_depak != NULL) {
        datagram_done(lci->lci_pbp_depak);
        lci->lci_pbp_depak = NULL;
    }
    if (lci->lci_pbp_ackpak) {
        p_unqueue(&x25LocalAckQ, lci->lci_pbp_ackpak);
        datagram_done(lci->lci_pbp_ackpak);
        lci->lci_pbp_ackpak = NULL;
    }
    /*flush packets from RNR hold queue*/
    while ((pak = p_dequeue(&lci->lci_pbp_rnr_deferredQ)))
        datagram_done(pak);

    if (lci->lci_pbp_rnr && TIMER_RUNNING(lci->lci_timer)) {
        p_unqueue(&x25tQ, lci);
        TIMER_STOP(lci->lci_timer);
    }
    lci->lci_thc_applied = FALSE;
    lci->lci_pbp_xmt_sync = 0;
    lci->lci_pbp_rcv_sync = 0;
    lci->lci_pbp_rnr_ticks = 0;
    lci->lci_pbp_rnr = FALSE;
    lci->lci_pbp_pr = 0;
    lci->lci_pbp_ps = 0;
    lci->lci_pbp_ack = 0;
    lci->lci_pbp_rpr = 0;
    lci->lci_pbp_holdQ_count = 0;
    lci->lci_pbp_need_pak_size = 0;
} /* end x25_pbp_common_reset() */

void x25_pbp_reset (lcitype * lci)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    if (lci->lci_pbp_comp_stac == NULL || 
	lci->lci_pbp_decomp_stac == NULL ||
	lci->lci_pbp_enabled == FALSE) {
	reset_interrupt_level(level);
	return;
    }
    x25_pbp_common_reset(lci);
    lci->lci_pbp_resets++;
    x25_pbp_clearcounters(lci);
    LZS_InitCompressionHistory(lci->lci_pbp_comp_stac);
    LZS_InitDecompressionHistory(lci->lci_pbp_decomp_stac);
    reset_interrupt_level(level);
}				/* end x25_pbp_reset() */

void no_x25_pbp_compression (lcitype * lci)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    lci->lci_pbp_enabled = FALSE;	/* this 1st */
    if (lci->lci_pbp_comp_stac != NULL) {
	free(lci->lci_pbp_comp_stac);
	lci->lci_pbp_comp_stac = NULL;
    }
    if (lci->lci_pbp_decomp_stac !=  NULL)
      free(lci->lci_pbp_decomp_stac);
    x25_pbp_common_reset(lci);
    lci->lci_pbp_compress = FALSE;
    reset_interrupt_level(level);
}				/* end no_x25_pbp_compression() */

void x25_pbp_clearcounters (lcitype * lci)
{
ushort i;

	lci->lci_pbp_xmt_in_bytes = 0;
	lci->lci_pbp_rcv_out_bytes = 0;
	for (i=0; i < COMP_STAT_PERIODS_MAX; i++) {
	    lci->lci_pbp_next_clk[i] = comp_stat_clks[i];
	    lci->lci_pbp_xmt_in_interv[i] = 0;
	    lci->lci_pbp_bytessent_interv[i] = 0;
	    lci->lci_pbp_rcv_out_interv[i] = 0;
	    lci->lci_pbp_bytesrcvd_interv[i] = 0;

	    lci->lci_pbp_xmt_in_interv_l[i] = 0;
	    lci->lci_pbp_bytessent_interv_l[i] = 0;
	    lci->lci_pbp_rcv_out_interv_l[i] = 0;
	    lci->lci_pbp_bytesrcvd_interv_l[i] = 0;
	}
}				/* end  x25_pbp_clearcounters() */

/*
 * x25 PBP local ack's explained (if (x25_switching_pbp_compress == TRUE)):
 *
 *PBP compression can decompress multiple segmented frames (M bit's) into
 *one larger non-segmented frame.  this means x25 end to end ack's won't
 *work.  so we have PBP compression "local ack's" as follows (need to invent
 *some names to describe a conceptial model):
 *
 *  ---> s0 [near  "outer node" decompress/compress near "inner node"] ---->S
 * S--->    [far  "inner node" --- far "outer node"] s1 <------
 *
 * where the path S can be remote or local switching.
 *
 *the lci's node associated with the idb
 *is the outer node and uses the standard
 *flow control variables: lci_pr, lci_ack, etc.
 *the  inner node uses these flow control
 *variables: lci_pbp_ps, lci_pbp_ack, lci_pbp_rpr,
 *lci_pbp_pr & lci_pbp_holdQ_count (available slots modulo 8,
 *window * fixed at 7.  it's the backpressure variable
 *used by the near node ack's).
 *
 *ANOTHER COMPLICATION:
 *   of the pair of local ack interface:
 *         one doing PBP decompression/compression
 *         the other NOT doing decompression/compression (the NOT interface).
 *   the NOT interface on input packets MUST DO packet re-assemply
 *   of fragmented frames (M bits). 
 *
 *local ack variable usage:
 *	lci_pbp_pr		next expected received ps
 *	lci_pbp_rpr		last          received pr 
 *	lci_pbp_ack		next pr to be sent
 *	lci_pbp_ps		next ps to be sent
 *	lci_pbp_holdQ_count     computed far inner nodes holdQ count
 *				     via lci_pbp_ps & lci_pbp_rpr.
 *	lci_pbp_ackpak		queued RR to be forwarded over TCP.
 */

/*
 * called by the inner node to stuff a DATA packet destined for
 * the far inner node with ps/pr using the near inner ps/or variables.
 * ps++ occurs here.
 */
void x25_pbp_stuff_pr_ps (lcitype *lci, packettype *p)
{
    uchar *c;

    if ((p->x25_sn & 0x3) == X25_MODULO_8) {
	p->x25_pti = (p->x25_pti & 0x1f) | (lci->lci_pbp_ack << 5);
	if ((p->x25_pti & X25_DATA_MASK) == X25_DATA) {
	    p->x25_pti = (p->x25_pti & 0xf1) | (lci->lci_pbp_ps << 1);
	    lci->lci_pbp_ps = ++lci->lci_pbp_ps % X25_PBP_MODULO;
	}
    } else {
	c = (uchar *)p + X25HEADERBYTES;
	*c = (*c & 0x1) | lci->lci_pbp_ack << 1;
	if ((p->x25_pti & X25_DATA_MASK) == X25_DATA) {
	    p->x25_pti = (p->x25_pti & 0x1) | (lci->lci_pbp_ps << 1);
	    lci->lci_pbp_ps = ++lci->lci_pbp_ps % X25_PBP_MODULO;
	}
    }
    /*update holdQ count to reflect packet we are sending*/
    lci->lci_pbp_holdQ_count = (lci->lci_pbp_ps - lci->lci_pbp_rpr +
				X25_PBP_MODULO) % X25_PBP_MODULO;

} /*end x25_pbp_stuff_pr_ps()*/

/*
 *extract ps/pr from a DATA or ACK packet
 */
void x25_pbp_extract_pr_ps (lcitype *lci, packettype *p, int *pr, int *ps)
{
    uchar *c;

    if ((p->x25_sn & 0x3) == X25_MODULO_8) {
	*pr = (p->x25_pti >> 5) & 0x7;
	if ((p->x25_pti & X25_DATA_MASK) == X25_DATA) {
	    *ps = (p->x25_pti >> 1) & 0x7;
	}
    } else {
	c = (uchar *)p + X25HEADERBYTES;
	*pr = (*c >> 1) & 0x7f;
	if ((p->x25_pti & X25_DATA_MASK) == X25_DATA) {
	    *ps = (p->x25_pti >> 1) & 0x7f;
	}
    }
} /*end x25_pbp_extract_pr_ps()*/

void x25_pbp_flush_call (lcitype *lci)
{
    if (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
	x25_flush_call(lci,
		X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, TRUE);
	x25_sendcause(lci->lci_idb->hwptr, lci, X25_RESET,
		X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_NO_INFO, NULL);
    } else {
	x25_sendcause(lci->lci_idb->hwptr, lci, X25_CLEAR,
		X25_CLEAR_NETWORK_CONGESTION, X25_DIAG_NO_INFO, NULL);
    }
} /*end x25_pbp_flush_call()*/

/*
 *validate pr from the far inner node. called by the near inner node
 */
boolean x25_pbp_check_pr (lcitype *lci, int pr)
{
    boolean good;

    if (pr != lci->lci_pbp_ps && pr != lci->lci_pbp_rpr) {
	if (lci->lci_pbp_ps > lci->lci_pbp_rpr)
	    good = (pr >= lci->lci_pbp_rpr && pr <= lci->lci_pbp_ps);
	else if (lci->lci_pbp_rpr == lci->lci_pbp_ps)
	    good = (pr == lci->lci_pbp_rpr);
	else
	    good = (pr >= lci->lci_pbp_rpr || pr <= lci->lci_pbp_ps);
	    
    } else
	good = TRUE;
    if (good) {
	lci->lci_pbp_rpr = pr;
    }
    return (good);
} /*end x25_pbp_pr_check()*/

/*
 *process far inner node DATA & ACK packet.  called by the near inner
 *node to updating & validate the near inner node ps/pr variables
 */
boolean x25_pbp_inner_node_receive (lcitype *lci, packettype *p)
{
    int pr, ps;
    
    x25_pbp_extract_pr_ps(lci, p, &pr, &ps); 
    if (!x25_pbp_check_pr(lci, pr)) {
	return (FALSE);
    }

    if ((p->x25_pti & X25_DATA_MASK) == X25_DATA) {
	if ((ps != lci->lci_pbp_pr) || 
	   (ps == (lci->lci_pbp_ack + X25_PBP_WINDOW) % X25_PBP_MODULO)) {
	    return (FALSE);
	}
	lci->lci_pbp_pr = ++lci->lci_pbp_pr % X25_PBP_MODULO;
    }
    
    /*we only need to know the far sides lci_holdQ status - don't need to
     *know far sides wout.  X25_HOLDQ is default to 10. max value of
     *lci_pbp_holdQ_count is 7.  the assumed window size is 7. 
     *lci_pbp_holdQ_count is used as the back pressure variable for the outer 
     *node. PBP Compression without aggregation can decompress multiple M bit
     *frames into a single frame sent from the near pair to the
     *far node pair.  so don't need to worry about window wrap.  but
     *with aggregation it is a problem, which can be solved as follows:
     *  use X25_MODULO_128. then, for sn == 0x1 insert the extra byte
     *  in the x25 header (using the encap area) and set sn == 0x3 (illegal
     *  - but its just between cisco's) and the at the far node convert
     *  back to X25_MODULO_8 after extracting 6 bit pr & ps. 
     */
    /*update holdQ count to reflect the pr value received*/
    lci->lci_pbp_holdQ_count = (lci->lci_pbp_ps - lci->lci_pbp_rpr +
				X25_PBP_MODULO) % X25_PBP_MODULO;
    return (TRUE);
} /*end x25_pbp_inner_node_receive()*/

/*
 * update near outer node lci_ack taking into account the back pressure
 * of the far outer node (as computed by the near inner node).
 */
void x25_pbp_update_lci_ack (hwidbtype *hwidb, lcitype *lci)
{
    int unack;

    unack = (((lci->lci_pr + hwidb->x25_modulo) - lci->lci_ack) % 
		hwidb->x25_modulo) - lci->lci_pbp_holdQ_count;
    if (unack > 0) {
	lci->lci_ack = (lci->lci_ack + unack) % hwidb->x25_modulo;
    }
} /*end x25_pbp_update_lci_ack()*/

/*
 * return near outer node linearized lci_ack. range 0-7
 * i.e., the number of frames we can ack. 
 */
boolean x25_pbp_unacked (hwidbtype *hwidb, lcitype *lci)
{
    int unack;

    unack = (((lci->lci_pr + hwidb->x25_modulo) - lci->lci_ack) %
                hwidb->x25_modulo) - lci->lci_pbp_holdQ_count;
    return (unack == hwidb->x25_th || unack == lci->lci_win);
} /* end x25_pbp_unacked()*/

/*
 *start building near inner node ack packet
 */
paktype *x25_pbp_begin_ack (hwidbtype *idb, lcitype *lci)
{
    paktype *pak;
    packettype *p;

    pak = getx25(idb, lci, 0, TRUE);
    if (!pak)
	return (NULL);
    p = (packettype *)pak->datagramstart;
    p->x25_q = 0;
    p->x25_d = 0;
    p->x25_sn = (idb->x25_modulo == X25_MOD8) ? X25_MODULO_8 : X25_MODULO_128;
    p->x25_lcg = LCG(lci->lci_lci);
    p->x25_lcn = LCN(lci->lci_lci);
    p->x25_pti = X25_RR;
    return (pak);
} /*end x25_pbp_begin_ack()*/

/*
 *finish building inner node ack packet
 */
void x25_pbp_finish_ack (lcitype *lci, paktype *pak)
{
    packettype *p = (packettype *)pak->datagramstart;
    uchar *c;

    if ((p->x25_sn & 0x3) == X25_MODULO_8) {
	p->x25_pti |= (lci->lci_pbp_ack << 5);
    } else {
	c = (uchar *)p + X25HEADERBYTES;
	*c = lci->lci_pbp_ack << 1;
    }
} /*end x25_pbp_finish_ack()*/

ushort x25_pbp_switch (boolean enabled, lcitype *lci)
{
    lcitype *lci1;
    x25routeentrytype *pt;

    /*x25_pbp_switch() is an initialization routine called by
     *x25_pbp_init().
     *if x25 switching pbp compression is disabled (!x25_switching_pbp_compress)
     *  and doing x25 switching then don't allow compression or local ack's .
     *else
     *  if there is sw_data then, the call came in on lci1 and has been
     *  set up by a prior call from x25_direct_call() to this routine
     *  lci_swdata covers remote switching - we compress/decompress
     *  !lci_swdata covers encapsulations.
     *  setup so as to not do decompress on lci and compress on lci1
     */ 
    if (lci->lci_swtype == X25_ROUTE_IF && 
                       ((lci1 = lci->lci_swdata) != NULL)) {
	/*switched case local*/
	if (enabled) {/*lci moves compressed data*/	
	    if (lci1->lci_pbp_enabled) {/*lci1 moves compressed data*/
		lci->lci_pbp_compress = FALSE;
		lci1->lci_pbp_compress = FALSE;
		no_x25_pbp_compression(lci1);
		lci->lci_pbp_enabled = FALSE;
		lci->lci_pbp_send_id = TRUE;
		lci1->lci_pbp_send_id = FALSE;
		lci->lci_pbp_local_ack = FALSE;
		lci1->lci_pbp_local_ack = FALSE;
		return (X25_PBP_SWITCH_FAILED);
	    } else {/*lci1 moves no compressed data*/
		if (!x25_switching_pbp_compress) {
		    lci->lci_pbp_send_id = FALSE;
		    errmsg(&msgsym(PBPNOEND, X25), lci->lci_idb->namestring,
			   lci->lci_lci);
		    lci->lci_pbp_compress = FALSE;
		    lci1->lci_pbp_compress = FALSE;
		    no_x25_pbp_compression(lci1);
		    lci->lci_pbp_enabled = FALSE;
		    lci1->lci_pbp_send_id = FALSE;
		    lci->lci_pbp_local_ack = FALSE;
		    lci1->lci_pbp_local_ack = FALSE;
		    return(X25_PBP_SWITCH_ER);
		} else {
		    /*following done later because initialization routines
		     * called later test these before performing tasks
		     *  lci->lci_pbp_compress = TRUE;
		     *  lci->lci_pbp_send_id = TRUE;
		     */

		    lci->lci_pbp_local_ack = TRUE;
		    lci1->lci_pbp_local_ack = TRUE;
		    return (X25_PBP_SWITCH_OK);
		}
	    }
	} else {/*lci moves no compressed data*/
	    if (lci1->lci_pbp_enabled) {/*lci1 moves compressed data*/
		if (!x25_switching_pbp_compress) {
		    /*assume its PBP on both lci & lci1. but issue warning*/
		    lci->lci_pbp_send_id = TRUE;
		    lci->lci_pbp_compress = FALSE;
		    lci1->lci_pbp_compress = FALSE;
		    no_x25_pbp_compression(lci1);
		    lci->lci_pbp_enabled = FALSE;
		    lci1->lci_pbp_send_id = FALSE;
		    lci->lci_pbp_local_ack = FALSE;
		    lci1->lci_pbp_local_ack = FALSE;
		    return(X25_PBP_SWITCH_FAILED);
		} else {
		    lci->lci_pbp_compress = FALSE;
		    lci->lci_pbp_send_id = FALSE;
		    lci->lci_pbp_local_ack = TRUE;
		    lci1->lci_pbp_local_ack = TRUE;
		    return (X25_PBP_SWITCH_FAILED);
		}
	    } else {/*lci1 moves no compressed data*/
		lci->lci_pbp_compress = FALSE;
		lci->lci_pbp_send_id = FALSE;
		lci->lci_pbp_local_ack = FALSE;
		return (X25_PBP_SWITCH_FAILED);
	    }
	}
    } else if (enabled) {
	/*following done later because initialization routines called
	 * later test these before performing tasks
	 *  lci->lci_pbp_compress = TRUE;
	 *  lci->lci_pbp_send_id = TRUE;
	 */

	pt = lci->lci_rtp;
	if ((lci->lci_swtype == X25_ROUTE_IPADDR) || (pt != NULL && 
	   pt->type == X25_ROUTE_IPADDR)) { /* remote switched call */
	    if (!x25_switching_pbp_compress) {
		lci->lci_pbp_compress = FALSE;
		lci->lci_pbp_enabled = FALSE;
		lci->lci_pbp_send_id = FALSE;
		lci->lci_pbp_local_ack = FALSE;
		return (X25_PBP_SWITCH_FAILED);
	    } else {
		lci->lci_pbp_local_ack = TRUE;
	    }
	}
	return (X25_PBP_SWITCH_OK);
    } else {
	lci->lci_pbp_compress = FALSE;
	lci->lci_pbp_send_id = FALSE;
	return (X25_PBP_SWITCH_FAILED);
    }
}/*end x25_pbp_switch() */

boolean x25_pbp_restriction_checks (lcitype *lci, hwidbtype *hwidb)
{
    /* X25_HOLDQ is now 10
     * idb->x25_holdmax = X25_HOLDQ 
     * idb->x25_holdmax should not be lowered below X25_HOLDQ
     */
    if (x25_switching_pbp_compress && (hwidb->x25_holdmax != 0 && 
       hwidb->x25_holdmax < X25_HOLDQ)) {
	errmsg(&msgsym(PBPHOLDQ, X25), lci->lci_idb->namestring,
		lci->lci_lci);
	return (FALSE);
    }
    if (x25_switching_pbp_compress && lci->lci_dbit) {
	errmsg(&msgsym(PBPDBIT, X25), lci->lci_idb->namestring,
		lci->lci_lci);
	return (FALSE);
    }
    return (TRUE);
}
/*
 * returns TRUE/FALSE - FALSE is an error condition
 * arg calling is sent TRUE when called from x25_sendcall() &
 * FALSE from x25_direct_call()
 * x25_pbp_switch() determines in the case of a IS (switching)
 * if the compressed input stream needs to be decompressed
 * & the non-compressed output stream needs to be compressed.
 * in the case of x25_sendcall() without error:
 *   lci_pbp_send_id determines if the PBP ID is sent &
 *   lci_pbp_compress determines if compression is done.
 *   lci_pbp_local_ack determines if an IS can't do
 *   end to end ack's (RR's).
 */
boolean x25_pbp_init (lcitype * lci, boolean calling)
{
    hwidbtype *hwidb;
    x25map *map = NULL;
    boolean compress = FALSE;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    if (lci == NULL) {
	reset_interrupt_level(level);
	return (FALSE);
    }
    if (lci->lci_idb == NULL) {
	reset_interrupt_level(level);
	return (FALSE);
    }
    if ((hwidb = lci->lci_idb->hwptr) == NULL) {
	reset_interrupt_level(level);
	return (FALSE);
    }

    map = lci->lci_map;
    if (calling) {
	if (hwidb->x25_flags & X25_COMPRESS)
	    compress = TRUE;
	if (map != NULL) {
	    if (map->map_flags & X25_MAPF_PBP)
		compress = TRUE;
	    if (map->map_flags & X25_MAPF_NOPBP)
		compress = FALSE;
	}
	if (!compress) {
	    /*we may have to change lci_swdata pbp compression params*/
	    (void) x25_pbp_switch(compress, lci);
	    /*issue X25 CALL without PBP ID*/
	    reset_interrupt_level(level);
	    return (TRUE);
	}
    } else {
	if (lci->lci_pbp_enabled) { /*call collision case*/
	    switch (x25_pbp_switch(TRUE, lci)) {
		case X25_PBP_SWITCH_ER:
		    reset_interrupt_level(level);
		    return (FALSE);
		case X25_PBP_SWITCH_FAILED:
		case X25_PBP_SWITCH_OK:
		    if (!x25_pbp_restriction_checks (lci, hwidb)) {
			reset_interrupt_level(level);
			return (FALSE);
		    }
		    break;
	    }	
	    reset_interrupt_level(level);
	    return (TRUE);
	}
    }
    /*lci_swdata pbp compression params may force us to change lci's
     *params + lci_swdata's may also be changed
     */
    switch (x25_pbp_switch(TRUE, lci)) {
	case X25_PBP_SWITCH_ER:
	    reset_interrupt_level(level);
	    return (FALSE);
	case X25_PBP_SWITCH_FAILED:
	    if (!x25_pbp_restriction_checks (lci, hwidb)) {
		reset_interrupt_level(level);
		return (FALSE);
	    }
	    reset_interrupt_level(level);
	    return (TRUE);
	case X25_PBP_SWITCH_OK:
	    if (!x25_pbp_restriction_checks (lci, hwidb)) {
		reset_interrupt_level(level);
		return (FALSE);
	    }
	    break;
    }	

    if (lci->lci_pbp_comp_stac == NULL) {
	lci->lci_pbp_comp_stac = 
	  malloc(LZS_SizeOfCompressionHistory());
	if (lci->lci_pbp_comp_stac == NULL) {
	    reset_interrupt_level(level);
	    return (FALSE);
	}
    }
    if (lci->lci_pbp_decomp_stac == NULL) {
	lci->lci_pbp_decomp_stac = 
	  malloc(LZS_SizeOfDecompressionHistory());
	if (lci->lci_pbp_comp_stac == NULL) {
	    reset_interrupt_level(level);
	    return (FALSE);
	}
    }
    x25_pbp_common_reset(lci);
    x25_pbp_clearcounters(lci);
    LZS_InitCompressionHistory(lci->lci_pbp_comp_stac);
    LZS_InitDecompressionHistory(lci->lci_pbp_decomp_stac);
    lci->lci_pbp_compress = TRUE;
    lci->lci_pbp_send_id = TRUE;
    lci->lci_pbp_enabled = TRUE;/* this last */
    reset_interrupt_level(level);
    return (TRUE);
}				/* end x25_pbp_init() */

void x25_pbp_show_stats (lcitype *lci)
{
ulong ratio_i_xmt, ratio_i_rcv, int_f;
uchar ratio_f_xmt[COMP_RATIO_F + 1];
uchar ratio_f_rcv[COMP_RATIO_F + 1];
ulong numerator, divisor;
ushort i;

	if (lci == NULL || !lci->lci_pbp_compress) {
	    return;	     
	}

	printf("\n Bytes %d/%d PBP uncompressed tx/rx", 
	       lci->lci_pbp_xmt_in_bytes, lci->lci_pbp_rcv_out_bytes); 
	for (i=0; i < COMP_STAT_PERIODS_MAX; i++) {

	    numerator = lci->lci_pbp_xmt_in_bytes - 
				   lci->lci_pbp_xmt_in_interv[i];
	    divisor = lci->lci_bytessent - 
				lci->lci_pbp_bytessent_interv[i];
	    if ((long) divisor <= 0) {
		ratio_i_xmt = 0;
		int_f = 0;
	    } else {
		ratio_i_xmt = numerator/divisor;
		int_f = ((numerator % divisor)*COMP_RATIO_M)/divisor;
	    }
	    comp_bf_to_a(int_f, ratio_f_xmt);

	    numerator = lci->lci_pbp_rcv_out_bytes - 
			    lci->lci_pbp_rcv_out_interv[i];

	    divisor = lci->lci_bytesrcvd  - 
			    lci->lci_pbp_bytesrcvd_interv[i];
	    if ((long) divisor <= 0) {
		ratio_i_rcv = 0;
		int_f = 0;
	    } else {
		ratio_i_rcv = numerator/divisor;
		int_f = ((numerator % divisor)*COMP_RATIO_M)/divisor;
	    }
	    comp_bf_to_a(int_f, ratio_f_rcv);

	    printf("\n\tPBP %2d min avg ratio xmt/rcv %d.%s/%d.%s ", 
		comp_stat_clks[i], ratio_i_xmt, ratio_f_xmt, 
		ratio_i_rcv, ratio_f_rcv);

	}
}/* end x25_pbp_show_stats() */

/*
 *called every minute 
 */
void x25_pbp_timer (void)
{
int i, j, k;
lcitype *lci;

    for (k = 0; k < X25_LCI_HASH_SIZE; k++) {
	for (j = 0; (lci = lci_hashpick(x25_lci_hash_table[k], j)); j++) {
	    if (!lci->lci_pbp_compress)
		continue;

	    mem_lock(lci);
	    for (i=0; i < COMP_STAT_PERIODS_MAX; i++) {
		if (--lci->lci_pbp_next_clk[i] <= 0) {

		    lci->lci_pbp_next_clk[i] = comp_stat_clks[i];

		    lci->lci_pbp_xmt_in_interv[i] = 
				lci->lci_pbp_xmt_in_interv_l[i];

		    lci->lci_pbp_bytessent_interv[i] = 
				lci->lci_pbp_bytessent_interv_l[i];

		    lci->lci_pbp_rcv_out_interv[i] = 
				lci->lci_pbp_rcv_out_interv_l[i];

		    lci->lci_pbp_bytesrcvd_interv[i] = 
				lci->lci_pbp_bytesrcvd_interv_l[i];

		    lci->lci_pbp_xmt_in_interv_l[i] =
					lci->lci_pbp_xmt_in_bytes;

		    lci->lci_pbp_bytessent_interv_l[i] = lci->lci_bytessent; 

		    lci->lci_pbp_rcv_out_interv_l[i] =
					lci->lci_pbp_rcv_out_bytes;

		    lci->lci_pbp_bytesrcvd_interv_l[i] = lci->lci_bytesrcvd;

		}
	    }
	    free(lci);
	}
    }
}/* end x25_pbp__timer() */

/************************************/
/* End File: x25_pbp.c              */
/************************************/
