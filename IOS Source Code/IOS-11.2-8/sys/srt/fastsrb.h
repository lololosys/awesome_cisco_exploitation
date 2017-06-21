/* $Id: fastsrb.h,v 3.5.10.3 1996/08/26 15:12:08 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/fastsrb.h,v $
 *------------------------------------------------------------------
 * Fast switching declarations, etc for srb and rsrb - platform independent
 *
 * February 1995, Bob O'Grady
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fastsrb.h,v $
 * Revision 3.5.10.3  1996/08/26  15:12:08  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.5.10.2  1996/07/18  20:29:26  ppearce
 * CSCdi63366:  Move SRB counters from hwidb into SRB subblock
 * Branch: California_branch
 *   - Decreases size of hwidb for non-SRB'd interfaces
 *   - Provides additional IOS-independence for SRB
 *   - Moves SRB a little closer to being completely swidb-based
 *
 * Revision 3.5.10.1  1996/03/18  22:10:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/03/16  07:37:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.1  1996/01/24  23:27:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/02/21  02:43:40  hampton
 * Clean up platform specific SRB defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.4  1995/12/13  04:24:02  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.3  1995/11/17  18:54:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:41:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:31:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:30:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/08  08:08:16  ogrady
 * CSCdi38378:  Srb common packets double counted.  Remove input
 * accounting from srb_common_setup.
 *
 * Revision 2.4  1995/07/11  00:52:37  ogrady
 * CSCdi34908:  Process level R/SRB hangs interface (orig AT over SRB
 * fails).  Don't make fastout vector return_false to disable srb fast
 * switching since its called from srb_common() path.  Set AC byte to
 * AC_PRI4 for all srb output paths.  Don't count ignored MAC packets as
 * drops.  Ongoing SRB counter clean up.
 *
 * Revision 2.3  1995/06/26  20:45:13  rbatz
 * CSCdi30326:  Bytes and Packet information INVALID(sho int token ring)
 * When fast srb explorers were transmitted, outputs_fast[ACCT_PROTO_SRB]
 * and tx_cumbytes_fast[ACCT_PROTO_SRB] were not being incremented.  This
 * resulted in incorrect reporting of output stats for "show int tokX".
 *
 * Revision 2.2  1995/06/17  06:26:12  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  23:00:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __FASTSRB_H__
#define __FASTSRB_H__

#include "srb_core.h"

#define	ENTER_RSRB(a)

/*
 * srb fast switching return values
 * NOTE the idb->tr_srb_fastout() vector should in general consume the
 *  packet (with datagram_done()) and note return FS_SRB_SLOWSWITCH.
 *  By time it is called the input pak may
 *  have been modified - so saving the pak on error and passing it up
 *  to process level may not be an option.
 *  Also if packets are sent via both fastswitch
 *  and process level on a per pak basis there is a risk of out of order
 *  packets - worse than a drop.
 */

#define	FS_SRB_SENT	1	/* pak sent, datagram_done() called */
#define	FS_SRB_SLOWSWITCH 0	/* pak needs process level processing */
#define	FS_SRB_DROPPED	-1	/* pak not sent, datagram_done() called */


/*
 * Packet check sum type for pak->fcs_type field
 * indicates type of check sum appended to packet in memory
 *   (check sum is not included in datagramsize).
 * currently used by fast srb
 */

#define	FCS_TYPE_NONE	0
#define	FCS_TYPE_TR	1
#define	FCS_TYPE_FDDI	2


/*
 * Set up SRB packet to call reg_invoke_srt_srb_forward()
 */
static inline boolean
srb_common_setup (hwidbtype *idb, paktype *pak, uint buf_flags)
{
    static char srb_rif_string[(SRB_MAXRIF * 2) + (SRB_MAXRIF / 2)];

    if (tokenring_debug) {

	tring_hdr *trh;
	sap_hdr *llc1;

        trh = (tring_hdr *) pak->datagramstart;
        llc1 = (sap_hdr *)
	       &((tring_hdr *)pak->datagramstart)->data[pak->riflen];

	tr_print_packet(idb, " br:", trh, (ulong *)llc1,
			buf_flags, pak->datagramsize);
    }
    
    if (idb->tr_bridge_idb == NULL && idb->tr_vring_blk == NULL) {

	if (srb_debug || srbevent_debug) {

	    tring_hdr *trh;
            trh = (tring_hdr *) pak->datagramstart;

	    buginf("\nSRB%d: no path, s: %e d: %e rif: %s",
		   idb->unit, trh->saddr, trh->daddr,
		   ((trh->saddr[0] & TR_RII) ?
		    get_rif_str(trh->data, srb_rif_string) : ""));
	}
        srbV.srb_sb_incr_rx_drops(idb->firstsw);
	datagram_done(pak);
	return(TRUE);
    }
    
    /*
     * According to the spec we should also check to see if
     * FR_LF < BR_LF and discard.  But this is a bozo check.  So
     * why burn the cycles.  If the frame was truely too large
     * the interface would have blown it off.
     */
    
    pak->enctype = ET_NULL;
    pak->flags |= PAK_SRB_IN;
    GET_TIMESTAMP(idb->lastinput);	/* remember time of last input */

    reg_invoke_srt_srb_forward(pak, idb, pak->riflen);
    return(TRUE);
}

extern if_srb_fastout_type get_srb_fastout(hwidbtype *);
extern boolean fs_explorer_out(paktype *, fs_hdrtype *, hwidbtype *);
extern void fs_expl_vectors(hwidbtype *);
extern int no_srb_fastout(hwidbtype *, paktype *);

extern fs_explorer_cb fs_explorer;
extern ipaddrtype	rsrb_fst_name;
extern queuetype	netbios_bytes_accQ;

#endif __FASTSRB_H__

