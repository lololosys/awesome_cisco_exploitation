/* $Id: v120.c,v 3.8.10.5 1996/08/07 09:04:40 snyder Exp $
 * $Source: /release/112/cvs/Xsys/wan/v120.c,v $
 *------------------------------------------------------------------
 * v120.c - routines for v.120 protocol over isdn
 *
 * July-1995, Bill May
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: v120.c,v $
 * Revision 3.8.10.5  1996/08/07  09:04:40  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.8.10.4  1996/07/28  07:33:16  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.8.10.3  1996/05/21  10:10:01  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.10.2  1996/04/27  06:38:19  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.6.8.3  1996/04/26  00:01:10  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.8.2  1996/04/17  00:02:50  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.8.1  1996/02/23  21:32:01  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.8.10.1  1996/03/18  22:49:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.3  1996/03/16  07:56:34  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.12.2  1996/03/13  02:12:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.12.1  1996/02/20  21:53:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/22  14:40:09  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/01  06:12:43  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1995/12/19  18:39:12  wmay
 * CSCdi45855:  V.120 UI frames have incorrect C/R - flip it.
 *
 * Revision 3.5  1995/12/17  18:41:29  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/07  18:23:47  wmay
 * CSCdi45025:  Spurious Accesses made in v120_line_state - restore line
 * state routine that was hooked and add extra protection to verify that
 * a V.120 connection is present.
 *
 * Revision 3.3  1995/11/17  18:05:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:51:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:37:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  23:30:24  wmay
 * Placeholder for CS_Arkansas_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "dialer_registry.h"
#include "ttysrv.h"
#include "packet.h"
#include "../if/network.h"
#include "../les/isdn_bri.h"
#include "v120.h"
#include "../ui/debug.h"
#include "../ui/debug_macros.h"
#include "dialer.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "parser_defs_ppp.h"
#include "v120_debug.h"

void v120_command (parseinfo *csb);
/*
 * V120 global variables.
 * fork variables are pids for processes
 * V120packetQ is what the incoming packets will be stored to
 * v120_head keeps a list of all the v120 interfaces
 */
static int v120_input_fork, v120_timer_fork;
static watched_queue *V120packetQ;
static v120_info_type *v120_head;

/*
 * Parser stuff
 */
#define ALTERNATE	NONE
#include "exec_debug_v120.h"
LINK_POINT(v120_debug_commands, ALTERNATE);
#undef ALTERNATE

#define ALTERNATE	NONE
#include "cfg_int_v120.h"
LINK_POINT(isdn_answer_command, ALTERNATE);
#undef ALTERNATE

static parser_extension_request v120_chain_init_table[] = {
    { PARSE_ADD_DEBUG_CMD, &pname(v120_debug_commands) },
    { PARSE_ADD_CFG_INT_ISDN_CMD, &pname(isdn_answer_command) },
    { PARSE_LIST_END, NULL }
};


/****************************************************************************
 * HWIDB hooked routines.
 * These are routines that are inserted into the hwidb in order to get the
 * input packets to v120_input.
 ****************************************************************************/

/*
 * v120_iqueue
 * replaces iqueue vector.  Will send the packet directly to the
 * v120_input routine via the V120packetQ
 */
static void v120_iqueue (hwidbtype *hwidb, paktype *pak)
{
    process_enqueue_pak(V120packetQ, pak);
}

/*
 * v120_parse_packet
 * replaces parse_packet vector.  Probably doesn't even need to do
 * anything.
 */
static iqueue_t v120_parse_packet (hwidbtype *hwidb, paktype *pak, uchar *data)
{
    pak->enctype = ET_V120;

    if (data) {
	pak->datagramstart = pak_center(pak) - V120_ENCAPBYTES;
    }
    pak->mac_start = pak->datagramstart;
    pak->network_start = pak->datagramstart + V120_ENCAPBYTES;
    pak->encsize = V120_ENCAPBYTES;

    return(v120_iqueue);
	     
}

/*
 * v120_vencapsulate, v120_encapsulate
 * replaces vencap, encap vectors.  Returning false makes sure
 * we don't send any packets
 */
static boolean v120_vencapsulate (paktype *pak, long address)
{
    if (v120_debug) {
	hwidbtype *hwidb;
	hwidb = pak->if_output->hwptr;
	if (hwidb)
	    buginf("\n%s: Attempt to use vencap vector while V120 active",
		   hwidb->hw_short_namestring);
    }
    return(FALSE);
}
/*
 * v120_line_state
 * return if the V.120 has contact with the other side
 */
static boolean v120_line_state (hwidbtype *hwidb)
{
    enum v120_states state;

    if (hwidb == NULL || 
        hwidb->isdn_info == NULL || 
        hwidb->isdn_info->v120info == NULL)
	return(FALSE);

    state = hwidb->isdn_info->v120info->state;
    return(state == V120_ESTABLISHED_STATE ||
	   state == V120_TIMER_RECOVERY_STATE);
}

/*
 * v120_command
 * This will handle the isdn all-incoming-calls-v120 command
 */
void v120_command (parseinfo *csb)
{
    hwidbtype *hwidb;

    hwidb = hwidb_or_null(csb->interface);
    if (hwidb == NULL || hwidb->isdn_info == NULL)
	return;
    if (hwidb->isdn_info->chantype != ISDN_Dchan)
	return;

    if (csb->nvgen) {
	nv_write(hwidb->isdn_info->answer_all_as_v120,
		 "%s", csb->nv_command);
    } else {
	if (csb->sense)
	    hwidb->isdn_info->answer_all_as_v120 = TRUE;
	else
	    hwidb->isdn_info->answer_all_as_v120 = FALSE;
    }
}

/****************************************************************************
 * v120_classify_packet - will check the length and will return the
 * type of the v120 packet.  Will return error code in the err variable
 * Note - this could probably be written as 2 tables, 1 translating a
 * uchar to v120_packet_type, another with the length.
 ****************************************************************************/
static enum v120_packet_type v120_classify_packet (paktype *pak, uchar *err)
{
    uchar control;

    *err = 0;
    /*
     * Skip past the LLI bytes, and read the 1st control byte
     */
    control = *(pak->datagramstart + V120_LLIBYTES);

    if ((control & V120_CONTROL_IFRAME_MASK) == 0) {
	if ((pak->datagramsize - V120_ENCAPBYTES) > V120_MAX_IFRAME_SIZE) {
	    *err = V120_FRMR_Y_MASK;
	    return(V120_BAD);
	}
	return(V120_IFRAME);
    }

    switch (control & V120_CONTROL_MASK) {
    case V120_CONTROL_S_FORMAT:
	if (pak->datagramsize != V120_S_FORMAT_SIZE) {
	    *err = V120_FRMR_X_MASK | V120_FRMR_W_MASK;
	    return(V120_BAD);
	}
	if (control == V120_RR_VALUE)	return(V120_RR);
	if (control == V120_RNR_VALUE)	return(V120_RNR);
	if (control == V120_REJ_VALUE)	return(V120_REJ);
	break;
    case V120_CONTROL_U_FORMAT:
	control &= ~V120_UNNUMBERED_PF_MASK;
	if (control == V120_UI_VALUE) {
	    if ((pak->datagramsize - V120_UNNUMBERED_ENCAPBYTES) >
		V120_MAX_IFRAME_SIZE) {
		*err = V120_FRMR_Y_MASK;
		return(V120_BAD);
	    }
	    return(V120_UI);
	} else if (control == V120_FRMR_VALUE) {
	    return(V120_FRMR);
	} else {
	    if (pak->datagramsize != V120_U_FORMAT_SIZE) {
		*err = V120_FRMR_X_MASK | V120_FRMR_W_MASK;
		return(V120_BAD);
	    }
	    switch (control) {
	    case V120_SABME_VALUE:	return(V120_SABME);
	    case V120_DM_VALUE:		return(V120_DM);
	    case V120_DISC_VALUE:	return(V120_DISC);
	    case V120_UA_VALUE:		return(V120_UA);
	    }
	}
	break;
    }
    *err = V120_FRMR_W_MASK;
    return(V120_BAD);
}

/****************************************************************************
 * Display v120 debug packets
 ****************************************************************************/
static const char *const v120_types[] = {
    "IFRAME",
    "RR",
    "RNR",
    "REJ",
    "SABME",
    "DM",
    "UI",
    "DISC",
    "UA",
    "FRMR",
    "BAD",
};

/*
 * v120_dump_bytes
 * Dump everything into a buffer (length is about 16 * 5 + 2 + ~8)
 * then buginf the buffer
 * Note: we only dump the first 16 bytes of the packet
 */
static void v120_dump_bytes (uchar *buf, ushort length)
{
    int ix;
    uchar tempbuff[100], *ptr;

    ptr = tempbuff;
    ptr += sprintf(ptr," len %d\n", length);

    ix = 0;
    if (length > 16) length = 16;
    while (length > 0) {
	ix++;
	length--;
	if (ix > 8 && length > 0) {
	    ptr += sprintf(ptr,"\n");
	    ix = 0;
	}
	ptr += sprintf(ptr, "0x%x ", *buf++);
    }
    buginf(tempbuff);
}

/*
 * v120_display_packet
 * Display packet type and relevant information
 */
static void v120_display_packet (hwidbtype *hwidb,
				 paktype *pak,
				 enum v120_packet_type type,
				 char inout)
{
    buginf("\n%s: %c %s:lli %d C/R %d ",
	   hwidb->hw_short_namestring,
	   inout,
	   v120_types[type],
	   v120_lli_to_num(pak->datagramstart),
	   (pak->datagramstart[0] >> 1) & 0x1);

    switch (type) {
    case V120_IFRAME:
	buginf("N(R)=%d N(S)=%d P/F=%d",
	       v120_get_nr(pak), v120_get_ns(pak),
	       (pak->datagramstart[3] & V120_SUPERVISORY_PF_MASK));
	v120_dump_bytes(pak->datagramstart + V120_ENCAPBYTES,
			pak->datagramsize - V120_ENCAPBYTES);
	break;
    case V120_UI:
	v120_dump_bytes(pak->datagramstart + V120_UNNUMBERED_ENCAPBYTES,
			pak->datagramsize - V120_UNNUMBERED_ENCAPBYTES);
	break;
    case V120_BAD:
	v120_dump_bytes(pak->datagramstart, pak->datagramsize);
	break;
    case V120_RR:
    case V120_RNR:
    case V120_REJ:
	buginf("N(R)=%d P/F=%d", pak->datagramstart[3] >> 1,
	       pak->datagramstart[3] & 0x01);
	break;
    case V120_FRMR:
	buginf("P/F=%d",
	       (pak->datagramstart[2] & V120_UNNUMBERED_PF_MASK) ? 1 : 0);
	v120_dump_bytes(pak->datagramstart + V120_UNNUMBERED_ENCAPBYTES,
			pak->datagramsize - V120_UNNUMBERED_ENCAPBYTES);
	break;
    case V120_SABME:
    case V120_DM:
    case V120_DISC:
    case V120_UA:
	buginf("P/F=%d",
	       (pak->datagramstart[2] & V120_UNNUMBERED_PF_MASK) ? 1 : 0);
	break;
    }
}

static void v120_display_input_packet (paktype *pak,
				       enum v120_packet_type type,
				       v120_info_type *v120)
{
    v120_display_packet(v120->hwptr, pak, type, 'I');
}

static void v120_display_output_packet (paktype *pak,
					enum v120_packet_type type,
					v120_info_type *v120)
{
    uchar temp;
    if (type == V120_BAD)
	type = v120_classify_packet(pak, &temp);
    v120_display_packet(v120->hwptr, pak, type, 'O');
}

/*
 * v120_start_idle_timer
 * Start idle timer on given interface.
 */
static void v120_start_idle_timer (hwidbtype *hwidb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(hwidb);
    if (ddb != NULL) {
	TIMER_START(ddb->dialer_idle_timer, ddb->dialer_idle_ticks);
    }
}

/****************************************************************************
 * Create and send V.120 informational packets
 ****************************************************************************/
static void v120_create_unnumbered_packet (hwidbtype *hwidb,
					   v120_info_type *v120,
					   uchar type,
					   boolean response)
{
    paktype *pak;

    pak = getbuffer(V120_U_FORMAT_SIZE);
    if (pak == NULL) {
	return;
    }

    pak->if_output = hwidb->firstsw;
    v120_write_unnumbered_header(pak, v120, type, response);

    if (v120_debug_packet) 
	v120_display_output_packet(pak, V120_BAD, v120);

    if (hwidb->board_encap) {
	if (!(*hwidb->board_encap)(pak, hwidb)) {
	    retbuffer(pak);
	    return;
	}
    }
    datagram_out(pak);
    v120_start_idle_timer(hwidb);
}

/*
 * v120_create_frmr_packet
 * this will create a FRMR packet - it needs the packet that came
 * in, and an error code, which it gets from v120_classify_packet
 */
static void v120_create_frmr_packet (hwidbtype *hwidb, v120_info_type *v120,
				     paktype *frompak, uchar errcode)

{
    paktype *pak;
    
    pak = getbuffer(V120_U_FORMAT_SIZE + 5);
    if (pak == NULL) {
	return;
    }

    pak->if_output = hwidb->firstsw;
    v120_write_unnumbered_header(pak, v120, V120_FRMR_F_VALUE, TRUE);
    PUTSHORT(pak->datagramstart + V120_U_FORMAT_SIZE,
	     GETSHORT(frompak->datagramstart));
    pak->datagramstart[V120_U_FORMAT_SIZE + 2] = v120->vs << 1;
    pak->datagramstart[V120_U_FORMAT_SIZE + 3] =
	(v120->vr << 1) | (pak->datagramstart[1] & V120_CR_MASK);
    pak->datagramstart[V120_U_FORMAT_SIZE + 4] = errcode;

    pak->datagramsize = V120_U_FORMAT_SIZE + 5;
    if (v120_debug_packet) 
	v120_display_output_packet(pak, V120_FRMR, v120);

    if (hwidb->board_encap) {
	if (!(*hwidb->board_encap)(pak, hwidb)) {
	    retbuffer(pak);
	    return;
	}
    }
    datagram_out(pak);
}

/*
 * v120_create_rr
 * This will create an RR with the correct v120 information in it
 * start_idle will indicate if we should reset the dialer idle timer
 */
static void v120_create_rr (hwidbtype *hwidb, v120_info_type *v120,
			    boolean response, uchar pf,
			    boolean start_idle)
{
    paktype *pak;

    pak = getbuffer(V120_S_FORMAT_SIZE);
    if (pak == NULL) {
	return;
    }

    pak->if_output = hwidb->firstsw;
    v120_write_supervisory_header(pak, v120, V120_RR_VALUE, response, pf);

    if (v120_debug_packet) 
	v120_display_output_packet(pak, V120_RR, v120);

    if (hwidb->board_encap) {
	if (!(*hwidb->board_encap)(pak, hwidb)) {
	    retbuffer(pak);
	    return;
	}
    }
    datagram_out(pak);
    v120->ack_pending = FALSE;
    if (start_idle)
	v120_start_idle_timer(hwidb);
}

/*
 * v120_create_rej
 * create a REJ packet
 */
static void v120_create_rej (hwidbtype *hwidb, v120_info_type *v120,
			     boolean response, uchar pf)
{
    paktype *pak;

    pak = getbuffer(V120_S_FORMAT_SIZE);
    if (pak == NULL) {
	return;
    }

    pak->if_output = hwidb->firstsw;
    v120_write_supervisory_header(pak, v120, V120_REJ_VALUE,
				  response, pf);
    if (v120_debug_packet) 
	v120_display_output_packet(pak, V120_REJ, v120);

    if (hwidb->board_encap) {
	if (!(*hwidb->board_encap)(pak, hwidb)) {
	    retbuffer(pak);
	    return;
	}
    }
    datagram_out(pak);
    v120_start_idle_timer(hwidb);
}

/****************************************************************************
 * Routines to change V120 states
 ****************************************************************************/

static void v120_dump_buffers (v120_info_type *v120)
{
    paktype *pak;

    while ((pak = pak_dequeue(&v120->unackQ))) {
	datagram_done(pak);
    }
}

static void v120_change_to_tei_assigned (v120_info_type *v120)
{
    v120_stop_t200(v120);
    v120_stop_t203(v120);
    v120->state = V120_TEI_ASSIGNED_STATE;
    v120->remote_busy = FALSE;
    v120_dump_buffers(v120);
}

static void v120_change_to_await_establish (v120_info_type *v120)
{
    v120->state = V120_AWAITING_ESTABLISHMENT_STATE;
    v120->remote_busy = FALSE;
    v120_create_unnumbered_packet(v120->hwptr, v120,
				  (V120_SABME_VALUE | V120_UNNUMBERED_PF_MASK),
				  FALSE);
    v120_start_t200(v120);
    v120->rc = 0;
    v120_dump_buffers(v120);
}

static void v120_change_to_establish (v120_info_type *v120)
{
    v120->state = V120_ESTABLISHED_STATE;
    v120->vs = 0;
    v120->va = 0;
    v120->vr = 0;
    v120->remote_busy = FALSE;
    v120->reject_exception = FALSE;
    v120->ack_pending = FALSE;
    v120_dump_buffers(v120);
    v120_stop_t200(v120);
    v120_start_t203(v120);
    v120_check_vty_txq(v120);
}

static void v120_change_to_timer_recovery (v120_info_type *v120)
{
    v120->state = V120_TIMER_RECOVERY_STATE;
    v120->rc = 0;
    v120_create_rr(v120->hwptr, v120, FALSE, V120_SUPERVISORY_PF_MASK, FALSE);
    v120_start_t200(v120);
}

#if 0
/*
 * Note:  The v120_change_to_await_release routine is commented out
 * intentionally.  This is what would be required if we ever needed
 * or desired to change to this state.  Right now (9/95), we will just
 * disconnect the isdn session.
 */
void v120_change_to_await_release (v120_info_type *v120)
{
    v120->state = V120_AWAITING_RELEASE_STATE;
    v120_create_unnumbered_packet(v120->hwptr, v120,
				  (V120_DISC_VALUE | V120_UNNUMBERED_PF_MASK),
				  FALSE);
    v120_start_t200(v120);
    v120->rc = 0;
}
#endif

/****************************************************************************
 * Create and send V.120 data packets
 *
 * One flaw in the way we do things now is that we will let the transmit
 * queue fill up to the window size.  We have a little problem then with
 * acknowledgements - we will have to wait to send one until the tx queue
 * is drained.  This is something that might have to worried about in the
 * future, though
 ****************************************************************************/
/*
 * v120_send_data
 * send a data packet of either multiframe or UI format
 * assumes that v120 header has already been added
 * Returns TRUE if packet is accepted for transmittal.
 * Returns FALSE if the upper layer should keep the packet
 */
boolean v120_send_data (paktype *pak, v120_info_type *v120)
{
    hwidbtype *hwidb;

    hwidb = v120->hwptr;

    if (v120->state != V120_ESTABLISHED_STATE) {
	/* Tell the upper level to save the packet */
	return(FALSE);
    }

    if (v120->remote_busy) {
	return(FALSE);
    }

    pak->if_output = hwidb->firstsw;
    
    if (v120->multi_frame) {
	uchar vk;

	/*
	 * send I frame - verify that we don't have too many outstanding
	 * buffers, then add the Iframe header
	 */
	vk = v120->va + V120_K_VALUE;
	if (vk >= V120_MAX_SEQ)
	    vk -= V120_MAX_SEQ;
	if (v120->vs == vk) {
	    return(FALSE);
	}

	v120->ack_pending = FALSE;
	pak->datagramstart -= V120_ENCAPBYTES;
	pak->datagramsize += V120_ENCAPBYTES;

	v120_write_lli(pak, v120, FALSE);
	pak->datagramstart[2] = (v120->vs << 1); /* control field is 0 */
	pak->datagramstart[3] = (v120->vr << 1); /* P/F is 0 */
	v120->vs = v120_increment_sequence(v120->vs);
	
	if (!(v120_t200_running(v120))) {
	    v120_stop_t203(v120);
	    v120_start_t200(v120);
	}
	/*
	 * set info_start field of pak to datagramstart so that
	 * adjusting at retransmission is nice and easy
	 */
	pak->info_start = pak->datagramstart;
	/*
	 * Lock the packet, and save it on the unackQ
	 */
	pak_lock(pak);
	if (pak_enqueue(&v120->unackQ, pak) == NULL) {
	    if (v120_debug) 
		buginf("\nCouldn't enqueue pak");
	    pak_unlock(pak);
	    retbuffer(pak);
	    v120_change_to_await_establish(v120);
	    return(TRUE);
	}
	if (v120_debug_packet) 
	    v120_display_output_packet(pak, V120_IFRAME, v120);
    } else {
	/* send UI frame */
	pak->datagramstart -= V120_U_FORMAT_SIZE;
	pak->datagramsize += V120_U_FORMAT_SIZE;
	v120_write_lli(pak, v120, FALSE);
	pak->datagramstart[2] = V120_UI_VALUE;
	if (v120_debug_packet) 
	    v120_display_output_packet(pak, V120_UI, v120);
    }
    if (hwidb->board_encap) {
	if (!(*hwidb->board_encap)(pak, hwidb)) {
	    v120_change_to_await_establish(v120);
	    return(TRUE);
	}
    }
    v120_start_idle_timer(hwidb);
    datagram_out(pak);
    return(TRUE);
}

/**************************************************************************
 * Retransmission routines
 *
 * Right now, we just take all the packets off the unackQ and retransmit
 * them immediately.  If we ever change this code so that we can have
 * a larger window size for outstanding packets, we might overflow the
 * transmitter.  In that case, something will have to be done.
 **************************************************************************/
/*
 * v120_retransmit_adjust
 * pak is taken from the unackQ.  Put the latest v(r) on the packet
 */
static void v120_retransmit_adjust (paktype *pak, v120_info_type *v120)
{
    pak->info_start[3] = (v120->vr << 1);
}

/*
 * v120_retransmit
 * Retransmit the entire unackQ.  We always take ack'ed packets off the
 * unackQ before we call this routine
 */
static void v120_retransmit (v120_info_type *v120)
{
    int ix;
    paktype *pak, *pakcopy;
    elementtype *element;
    leveltype status;

    ix = v120->unackQ.count;
    element = v120->unackQ.qhead;
    if (v120_debug && ix > 0)
	buginf("\n%s:retransmit %d packets",
	       v120->hwptr->hw_short_namestring, ix);
    /*
     * Question - is this right, or should we be ALL_DISABLE ?
     */
    status = raise_interrupt_level(NETS_DISABLE);
    while (ix > 0) {
	/*
	 * If pak->refcount is > 1, that means that the pak is still
	 * on the output queue.  That's okay, but we must copy
	 * the pak so we can adjust the v(r). (Thanks, Gil)
	 */
	pak = element->pakptr;
	if (pak->refcount > 1) {
	    pakcopy = pak_duplicate(pak);
	    if (pakcopy == NULL) {
		/*
		 * In this case, we just don't transmit the rest.
		 * q.921 will pick it up eventually
		 */
		reset_interrupt_level(status);
		return;
	    }
	    pak = pakcopy;
	}  else {
	    pak_lock(pak);	/* Just relock the packet */
	}

	v120_retransmit_adjust(pak, v120);

	if (v120_debug_packet) 
	    v120_display_output_packet(pak, V120_IFRAME, v120);
	pak->if_output = v120->hwptr->firstsw;
	datagram_out(pak);
	element = element->next;
	ix--;
    }
    reset_interrupt_level(status);
    v120_start_idle_timer(v120->hwptr);
}

/****************************************************************************
 * V120 TEI ASSIGNED state machine
 ****************************************************************************/
static void v120_tei_assigned_state (hwidbtype *hwidb,
				     v120_info_type *v120,
				     paktype *pak,
				     enum v120_packet_type type)
{
    uchar pf;

    switch (type) {
    case V120_SABME:
	pf = v120_unnumbered_pf(pak);
	v120_create_unnumbered_packet(hwidb, v120,
				      (V120_UA_VALUE | pf),
				      TRUE);
	v120_change_to_establish(v120);
	break;
    case V120_DISC:
	pf = v120_unnumbered_pf(pak);
	v120_create_unnumbered_packet(hwidb, v120,
				      (V120_DM_VALUE | pf),
				      TRUE);
	break;
    case V120_DM:
	pf = v120_unnumbered_pf(pak);
	if (!(pf)) {
	    v120_change_to_await_establish(v120);
	}
	break;
    case V120_UA: /* fall through */
    default:
	if (v120_debug) {
	    buginf("\n%s:Illegal packet in tei assigned state-c,d",
		   hwidb->hw_short_namestring);
	}
	break;
    }
    datagram_done(pak);
}

/****************************************************************************
 * V120 AWAITING ESTABLISHMENT state machine
 ****************************************************************************/
static void v120_awaiting_establishment_state (hwidbtype *hwidb,
					       v120_info_type *v120,
					       paktype *pak,
					       enum v120_packet_type type)
{
    uchar pf;
    
    pf = v120_unnumbered_pf(pak);
    
    switch (type) {
    case V120_SABME:
	v120_create_unnumbered_packet(hwidb, v120,
				      (V120_UA_VALUE | pf),
				      TRUE);
	break;
    case V120_DISC:
	v120_create_unnumbered_packet(hwidb, v120, (V120_DM_VALUE | pf),
				      FALSE);
	break;
    case V120_UA:
	if (pf) {
	    v120_change_to_establish(v120);
	} else {
	    if (v120_debug)
		buginf("\n%s:UA with F=0 in awaiting establish",
		       hwidb->hw_short_namestring);
	}
	break;
    case V120_DM:
	if (pf) {
	    v120_change_to_tei_assigned(v120);
	}
	break;
    default:
	if (v120_debug) {
	    buginf("\n%s:Illegal packet in awaiting establishment state",
		   hwidb->hw_short_namestring);
	}
	break;
    }
    datagram_done(pak);
}

/***************************************************************************
 * V120 established state routines
 ***************************************************************************/
/*
 * v120_check_nr
 * Verify that a received N(R) is in a valid range, and
 * then remove acknowledged packets from the unackQ
 * Returns true if we can go ahead - we've received a valid sequence
 * number.  It will return FALSE on an invalid sequence number.  In that
 * case, we've already sent the FRAMR packet, and changed states.  So
 * nothing further needs to be done.
 */
static boolean v120_check_nr (v120_info_type *v120, uchar nr, paktype *pak)
{
    boolean nr_okay = FALSE, had_acks;

    if (v120->va > v120->vs) {
	/* have the modula in the middle */
	if ((nr >= v120->va) || (nr <= v120->vs)) {
	    nr_okay = TRUE;
	} 
    } else {
	if (nr >= v120->va && nr <= v120->vs) {
	    nr_okay = TRUE;
	} 
    }
    if (nr_okay == FALSE) {
	/*
	 * Send a FRMR packet, and change to the await establish
	 * state
	 */
	v120_create_frmr_packet(v120->hwptr, v120, pak, V120_FRMR_Z_MASK);
	v120_change_to_await_establish(v120);
	return(FALSE);
    }
    /*
     * Now, remove any packets off the unackQ.  If we did, see if the
     * vty code has any packets to send
     */
    had_acks = FALSE;
    while (v120->va != nr) {
	had_acks = TRUE;
	pak = pak_dequeue(&v120->unackQ);
	if (pak != NULL) {
	    datagram_done(pak);
	}
	v120->va = v120_increment_sequence(v120->va);
    }
    /*
     * If we had some acknowledgements, check the transmit queue
     */
    if (had_acks && v120->remote_busy == FALSE)
	v120_check_vty_txq(v120);
    return(TRUE);
}

/*
 * v120_established_state
 * This is the state machine for the established state
 */
static void v120_established_state (hwidbtype *hwidb,
				    v120_info_type *v120,
				    paktype *pak,
				    enum v120_packet_type type)
{
    uchar pf;
    boolean toss_dgram;
    uchar nr, ns, old_va;

    toss_dgram = TRUE;
    switch (type) {
    case V120_SABME:
 	pf = v120_unnumbered_pf(pak);
	v120_create_unnumbered_packet(hwidb, v120,
				      (V120_UA_VALUE | pf),
				      TRUE);
	v120_change_to_establish(v120);
	break;
    case V120_DISC:
 	pf = v120_unnumbered_pf(pak);
	v120_create_unnumbered_packet(hwidb, v120,
				      (V120_UA_VALUE | pf),
				      TRUE);
	v120_change_to_tei_assigned(v120);
	break;
    case V120_DM:
 	pf = v120_unnumbered_pf(pak);
	if (pf) {
	    if (v120_debug)
		buginf("\n%s:DM with F=1, established state",
		       hwidb->hw_short_namestring);
	} else {
	    v120_change_to_await_establish(v120);
	}
	break;
    case V120_RR:
	old_va = v120->va;
	nr = v120_get_nr(pak);
	if (v120_check_nr(v120, nr, pak)) {
	    if (nr != v120->vs) {
		if (old_va != nr)
		    v120_start_t200(v120);
	    } else {
		v120_stop_t200(v120);
		v120_start_t203(v120);
	    }
	    if (v120_packet_is_command(pak)) {
		if (v120_supervisory_pf(pak))
		    /* asdf - might not want to restart idle timer */
		    v120_create_rr(hwidb, v120,
				   TRUE, V120_SUPERVISORY_PF_MASK, TRUE);
	    } else if (v120_supervisory_pf(pak)) {
		if (v120_debug)
		    buginf("\n%s:RR response w/f=1",
			   hwidb->hw_short_namestring);
	    }
	    if (v120->remote_busy) {
		v120->remote_busy = FALSE;
		v120_check_vty_txq(v120);
	    }
	}
	v120->remote_busy = FALSE;
	break;
    case V120_REJ:
	v120->remote_busy = FALSE;
	nr = v120_get_nr(pak);
	if (v120_check_nr(v120, nr, pak)) {
	    v120_stop_t200(v120);
	    v120_start_t203(v120);
	    if (v120_packet_is_command(pak)) {
		if (v120_supervisory_pf(pak))
		    v120_create_rr(hwidb, v120,
				   TRUE, V120_SUPERVISORY_PF_MASK, TRUE);
	    } else if (v120_supervisory_pf(pak)) {
		if (v120_debug)
		    buginf("\n%s:REJ response w/f=1",
			   hwidb->hw_short_namestring);
	    }
	    v120_retransmit(v120);
	}
	break;
    case V120_RNR:
	v120->remote_busy = TRUE;
	nr = v120_get_nr(pak);
	if (v120_check_nr(v120, nr, pak)) {
	    v120_start_t200(v120);
	    v120_stop_t203(v120);
	    if (v120_packet_is_command(pak)) {
		if (v120_supervisory_pf(pak))
		    v120_create_rr(hwidb, v120,
				   TRUE, V120_SUPERVISORY_PF_MASK, TRUE);
	    } else if (v120_supervisory_pf(pak)) {
		if (v120_debug)
		    buginf("\n%s:RNR response w/f=1",
			   hwidb->hw_short_namestring);
	    }
	}
	break;
    case V120_FRMR:
	v120_change_to_await_establish(v120);
	break;
    case V120_IFRAME:
	old_va = v120->va;
	nr = v120_get_nr(pak);
	if (v120_check_nr(v120, nr, pak)) {
	    if (v120->remote_busy == FALSE) {
		if (nr != v120->vs) {
		    if (nr != old_va)
			v120_start_t200(v120);
		} else {
		    v120_stop_t200(v120);
		    v120_start_t203(v120);
		}
	    }
	    pf = v120_iframe_pf(pak);
	    ns = v120_get_ns(pak);
	    if (ns == v120->vr) {
		/*
		 * We got the packet we expected.  See if we should send
		 * an RR.  We do so if P/F is set, or if we have 2 IFRAMES
		 * with no acks sent out
		 *
		 * It is here we might want to do flow control
		 */
		if (pf)
		    v120_create_rr(hwidb, v120,
				   /* Might need to send opposite of command */
				   TRUE, V120_SUPERVISORY_PF_MASK, TRUE);
		else {
		    /*
		     * If the ack_pending flag is set, send a RR - this
		     * will happen when we get 2 IFRAMES in, before we
		     * can transmit an acknowledgement
		     */
		    if (v120->ack_pending || v120->remote_busy)
			v120_create_rr(hwidb, v120, TRUE, 0, TRUE);
		    else
			v120->ack_pending = TRUE;
		}	
		toss_dgram = FALSE;
		pak->datagramstart += V120_ENCAPBYTES;
		pak->datagramsize -= V120_ENCAPBYTES;
		v120_data_to_vty(v120, pak);

		v120->vr = v120_increment_sequence(v120->vr);
		v120->reject_exception = FALSE;
	    } else {
		/*
		 * Not a good sequence number.  Handle it, Handle it...
		 * (Okay, to get that reference, you had to watch really
		 *  bad sitcoms in the 70's)
		 */
		if (v120->reject_exception) {
		    if (pf)
			v120_create_rr(hwidb, v120,
				       TRUE, V120_SUPERVISORY_PF_MASK, TRUE);
		} else {
		    v120->reject_exception = TRUE;
		    v120->ack_pending = FALSE;
		    v120_create_rej(hwidb, v120,
				    v120_packet_is_command(pak),
				    pf);
		}
	    }
	}
	break;
    case V120_UI:
	pak->datagramstart += V120_UNNUMBERED_ENCAPBYTES;
	pak->datagramsize -= V120_UNNUMBERED_ENCAPBYTES;
	v120_data_to_vty(v120, pak);
	toss_dgram = FALSE;
	break;
    default:
	break;
    }
    if (toss_dgram)
	datagram_done(pak);
}

/***************************************************************************
 * V120 timer recovery state routines
 ***************************************************************************/
static void v120_timer_recovery_state (hwidbtype *hwidb,
				       v120_info_type *v120,
				       paktype *pak,
				       enum v120_packet_type type)
{
    uchar pf;
    boolean toss_dgram;
    uchar nr, ns;

    toss_dgram = TRUE;
    switch (type) {
    case V120_SABME:
	pf = v120_unnumbered_pf(pak);
	v120_create_unnumbered_packet(hwidb, v120,
				      (V120_UA_VALUE | pf),
				      TRUE);
	v120_change_to_establish(v120);
	break;
    case V120_DISC:
	pf = v120_unnumbered_pf(pak);
	v120_create_unnumbered_packet(hwidb, v120,
				      (V120_UA_VALUE | pf),
				      TRUE);
	v120_change_to_tei_assigned(v120);
	break;
    case V120_UA:
	break;
    case V120_DM:
	/* record errors */
	v120_change_to_await_establish(v120);
	break;
    case V120_RR:
    case V120_REJ:
	v120->remote_busy = FALSE;
	nr = v120_get_nr(pak);
	if (v120_check_nr(v120, nr, pak)) {
	    pf = v120_supervisory_pf(pak);
	    if (v120_packet_is_command(pak)) {
		if (pf) {
		    v120_create_rr(hwidb, v120, TRUE, pf, FALSE);
		}
	    } else {
		if (pf) {
		    v120_stop_t200(v120);
		    v120_start_t203(v120);
		    v120_retransmit(v120);
		    v120->state = V120_ESTABLISHED_STATE;
		} 
	    }
	}
	break;
    case V120_RNR:
	v120->remote_busy = TRUE;
	nr = v120_get_nr(pak);
	if (v120_check_nr(v120, nr, pak)) {
	    pf = v120_supervisory_pf(pak);
	    if (v120_packet_is_command(pak)) {
		if (pf) {
		    v120_create_rr(hwidb, v120, TRUE, pf, TRUE);
		}
	    } else {
		/* response */
		if (pf) {
		    v120_start_t200(v120);
		    v120_retransmit(v120);
		    v120->state = V120_ESTABLISHED_STATE;
		} 
	    }
	}
	break;
    case V120_FRMR:
	v120_change_to_await_establish(v120);
	break;
    case V120_IFRAME:
	nr = v120_get_nr(pak);
	if (v120_check_nr(v120, nr, pak)) {
	    ns = v120_get_ns(pak);
	    pf = v120_iframe_pf(pak);
	    if (ns == v120->vr) {
		toss_dgram = FALSE;
		pak->datagramstart += V120_ENCAPBYTES;
		pak->datagramsize -= V120_ENCAPBYTES;
		v120_data_to_vty(v120, pak);

		v120->vr = v120_increment_sequence(v120->vr);
		if (pf)
		    v120_create_rr(hwidb, v120, TRUE, pf, TRUE);
		v120->reject_exception = FALSE;
	    } else {
		if (v120->reject_exception) {
		    if (pf)
			v120_create_rr(hwidb, v120, FALSE, pf, TRUE);
		} else {
		    v120->reject_exception = TRUE;
		    v120_create_rej(hwidb, v120,
				    v120_packet_is_command(pak),
				    pf);
		}
	    }
	}
	break;
    default:
	break;
    }
    if (toss_dgram) 
	datagram_done(pak);
}

/***************************************************************************
 * V120 awaiting release state routine
 ***************************************************************************/
static void v120_awaiting_release_state (hwidbtype *hwidb,
					 v120_info_type *v120,
					 paktype *pak,
					 enum v120_packet_type type)
{
    uchar pf;

    switch (type) {
    case V120_SABME:
	pf = v120_unnumbered_pf(pak);
	v120_create_unnumbered_packet(hwidb, v120, (V120_DM_VALUE | pf), TRUE);
	break;
    case V120_DISC:
	pf = v120_unnumbered_pf(pak);
	v120_create_unnumbered_packet(hwidb, v120, (V120_UA_VALUE | pf), TRUE);
	break;
    case V120_UA:
	pf = v120_unnumbered_pf(pak);
	if (pf) {
	    v120_change_to_tei_assigned(v120);
	} else {
	    if (v120_debug)
		buginf("\n%s:UA with F=0 in awaiting release",
		       hwidb->hw_short_namestring);
	}
	break;
    case V120_DM:
	pf = v120_unnumbered_pf(pak);
	if (pf) {
	    v120_change_to_tei_assigned(v120);
	}
	break;
    default:
	if (v120_debug) {
	    buginf("\n%s:Illegal packet in awaiting establishment state",
		   hwidb->hw_short_namestring);
	}
	break;
    }
    datagram_done(pak);
   
}


/***************************************************************************
 * V120 packet input routines
 ***************************************************************************/
static void v120_input (void)
{
    paktype *pak;
    hwidbtype *hwidb;
    ulong lli;
    enum v120_packet_type type;
    v120_info_type *v120;
    int pak_count;
    uchar classify_error;

    process_watch_queue(V120packetQ, ENABLE, RECURRING);
    
    while (TRUE) {
	process_wait_for_event();
	pak_count = PAK_SWITCHCOUNT;
	while (TRUE) {
	    if (--pak_count == 0)
		break;
	    
	    pak = process_dequeue(V120packetQ);
	    if (pak == NULL)
		break;
	    
	    hwidb = pak->if_input->hwptr;
	    if ((!(is_isdn(hwidb))) ||
		(hwidb->isdn_info->v120info == NULL)) {
		if (v120_debug) {
		    buginf("\n%s:not isdn or not v120",
			   hwidb->hw_short_namestring);
		}
		datagram_done(pak);
		continue;
	    }
	    v120 = hwidb->isdn_info->v120info;
	    if (v120 == NULL) {
		datagram_done(pak);
		continue;
	    }
	    lli = v120_lli_to_num(pak->datagramstart);
	    if (lli != v120->lli) {
		if (v120->params_from_call == FALSE) {
		    v120->lli = lli;
		    if (v120_debug) 
			buginf("\n%s:change in lli to %d",
			       hwidb->hw_short_namestring, lli);
		} else {
		    if (v120_debug) 
			buginf("\n%s:illegal lli %d - should be %d",
			       hwidb->hw_short_namestring, lli, v120->lli);
		    datagram_done(pak);
		    continue;
		}
	    }

	    /* we have a good hwidb */
	    type = v120_classify_packet(pak, &classify_error);
	    if (v120_debug_packet)
		v120_display_input_packet(pak, type, v120);
	    
	    if (type == V120_BAD) {
		if (v120_debug)
		    buginf("\n%s:illegal control field %x",
			   hwidb->hw_short_namestring,
			   GETSHORT(pak->datagramstart + V120_LLIBYTES));
		/* Transmit FRMR */
		v120_create_frmr_packet(hwidb, v120, pak, classify_error);
		datagram_done(pak);
		v120_change_to_tei_assigned(v120);
		continue;
	    }

	    switch (v120->state) {
	    case V120_TEI_ASSIGNED_STATE:
		v120_tei_assigned_state(hwidb, v120, pak, type);
		break;
	    case V120_AWAITING_ESTABLISHMENT_STATE:
		v120_awaiting_establishment_state(hwidb, v120, pak, type);
		break;
	    case V120_ESTABLISHED_STATE:
		v120_established_state(hwidb, v120, pak, type);
		break;
	    case V120_TIMER_RECOVERY_STATE:
		v120_timer_recovery_state(hwidb, v120, pak, type);
		break;
	    case V120_AWAITING_RELEASE_STATE:
		v120_awaiting_release_state(hwidb, v120, pak, type);
		break;
	    }
	} /* end reading packets */
    } /* end while (TRUE) */
}


/***************************************************************************
 * V120 timer routines
 * Timer routines for various states will just return the amount of time
 * until the next timer expires.  Then in the main routine, we dismiss for
 * the shortest amount.  This is where we could really improve.
 ***************************************************************************/
static inline ulong v120_tei_assigned_timer (hwidbtype *hwidb,
					     v120_info_type *v120)
{
    return(MAXULONG);
}

static ulong v120_awaiting_establishment_timer (hwidbtype *hwidb,
						v120_info_type *v120)
{
    if (TIMER_RUNNING_AND_AWAKE(v120->t200)) {
	v120->rc++;
	if (v120->rc >= V120_N200) {
	    v120_change_to_tei_assigned(v120);
	    return(MAXULONG);
	} else {
	    v120_create_unnumbered_packet(hwidb, v120,
					  (V120_SABME_VALUE |
					   V120_UNNUMBERED_PF_MASK),
					  FALSE);
	    v120_start_t200(v120);
	}
    } 
    return(TIME_LEFT_SLEEPING(v120->t200));
	
}

static ulong v120_established_timer (hwidbtype *hwidb,
				     v120_info_type *v120)
{
    if (TIMER_RUNNING_AND_AWAKE(v120->t200) ||
	TIMER_RUNNING_AND_AWAKE(v120->t203)) {
	if (v120_debug)
	    buginf("\n%s:EST:t20%c expired",
		   hwidb->hw_short_namestring,
		   TIMER_RUNNING_AND_AWAKE(v120->t200) ? '0' : '3');
	v120_change_to_timer_recovery(v120);
	return(TIME_LEFT_SLEEPING(v120->t200));
    } else {
	ulong left_t200, left_t203;
	/*
	 * If we have an ack pending, send it off
	 */
	if (v120->ack_pending)
	    v120_create_rr(hwidb, v120, TRUE, 0, TRUE);

	left_t200 = MAXULONG;
	left_t203 = MAXULONG;
	if (TIMER_RUNNING(v120->t200))
	    left_t200 = TIME_LEFT_SLEEPING(v120->t200);
	if (TIMER_RUNNING(v120->t203))
	    left_t203 = TIME_LEFT_SLEEPING(v120->t203);
	if (left_t200 < left_t203)
	    return(left_t200);
	return(left_t203);
    }
}

static ulong v120_timer_recovery_timer (hwidbtype *hwidb,
					v120_info_type *v120)
{
    if (TIMER_RUNNING_AND_AWAKE(v120->t200)) {
	if (v120_debug)
	    buginf("\n%s:TR:t200 expired",
		   hwidb->hw_short_namestring);
	if (v120->rc >= V120_N200) {
	    v120_change_to_await_establish(v120);
	    return(MAXULONG);
	} else {
	    v120->rc++;
	    v120_create_rr(hwidb, v120, FALSE, V120_SUPERVISORY_PF_MASK,
			   FALSE);
	    v120_start_t200(v120);
	}
    } 
    return(TIME_LEFT_SLEEPING(v120->t200));
}

static ulong v120_awaiting_release_timer (hwidbtype *hwidb,
					  v120_info_type *v120)
{
    if (TIMER_RUNNING_AND_AWAKE(v120->t200)) {
	if (v120->rc >= V120_N200) {
	    v120_change_to_await_establish(v120);
	    return(MAXULONG);
	} else {
	    v120->rc++;
	    v120_create_unnumbered_packet(hwidb, v120,
					  (V120_DISC_VALUE |
					   V120_UNNUMBERED_PF_MASK),
					  FALSE);
	    v120_start_t200(v120);
	}
    }
    return(TIME_LEFT_SLEEPING(v120->t200));
}

static void v120_timers (void)
{
    ulong time_to_next_run, ret_time;
    v120_info_type *v120;
    hwidbtype *hwidb;
    int fork, count;

    count = 0;
    while (v120_head != NULL && count < 10) {
	count++;
	v120 = v120_head;
	time_to_next_run = ONESEC;
	ret_time = MAXULONG;
        while (v120 != NULL) {
	    count = 0;
	    hwidb = v120->hwptr;
	    
	    switch (v120->state) {
	    case V120_TEI_ASSIGNED_STATE:
		ret_time = v120_tei_assigned_timer(hwidb, v120);
		break;
	    case V120_AWAITING_ESTABLISHMENT_STATE:
		ret_time = v120_awaiting_establishment_timer(hwidb, v120);
		break;
	    case V120_ESTABLISHED_STATE:
		ret_time = v120_established_timer(hwidb, v120);
		break;
	    case V120_TIMER_RECOVERY_STATE:
		ret_time = v120_timer_recovery_timer(hwidb, v120);
		break;
	    case V120_AWAITING_RELEASE_STATE:
		ret_time = v120_awaiting_release_timer(hwidb, v120);
		break;
	    }
	    if (ret_time < time_to_next_run) {
		time_to_next_run = ret_time;
	    }
	    v120 = v120->next;
	}
	process_sleep_for(time_to_next_run);
    }

    fork = v120_timer_fork;
    v120_timer_fork = 0;
    process_kill(fork);
}


/***************************************************************************
 * V120 start and stop routines.  Enables/disables V.120 for an interface
 ***************************************************************************/
static boolean v120_start_interface (hwidbtype *hwidb,
				     boolean have_llc,
				     void *p)
{
    v120_info_type *v120;
    v120_parameters_type *params;
    leveltype status;

    params = p;
    if (!(is_isdn(hwidb))) {
	return(FALSE);
    }
    /*
     * Check that we have valid V.120 parameters, if the call came in
     * with an LLC
     */
    if (have_llc && params != NULL) {
	if (params->sync) {
	    if (v120_debug)
		buginf("\n%s - V.120 doesn't support sync mode",
		       hwidb->hw_short_namestring);
	    return(FALSE);
	}
	if (params->lli_negotiation) {
	    if (v120_debug)
		buginf("\n%s - V.120 does not support LLI negotiation",
		       hwidb->hw_short_namestring);
	    return(FALSE);
	}
	if (params->mode == FALSE) {
	    if (v120_debug)
		buginf("\n%s - V.120 does not support Bit transparent mode",
		       hwidb->hw_short_namestring);
	    return(FALSE);
	}
    }
    /*
     * Make sure that V.120 processes are started and running 
     */
    if (v120_input_fork == 0) {
	if (V120packetQ == NULL)
	    V120packetQ = create_watched_queue("v120 input", 0, 0);
	v120_input_fork = process_create(v120_input, "V120 input",
					 NORMAL_STACK, PRIO_NORMAL);
	if (v120_input_fork == NO_PROCESS)
	    return(FALSE);
    }
    if (v120_timer_fork == 0) {
	v120_timer_fork = process_create(v120_timers, "V120 timers",
					 NORMAL_STACK, PRIO_NORMAL);
	/* If we can't create a timer process, exit */
	if (v120_timer_fork == NO_PROCESS)
	    return(FALSE);
    }
    /*
     * If for some reason the v120 is there, just do a restart
     */
    if (hwidb->isdn_info->v120info) {
	v120 = hwidb->isdn_info->v120info;
	if (v120_debug) {
	    buginf("\n%s:v120 interface request - already started",
		   hwidb->hw_short_namestring);
	}
	v120_change_to_tei_assigned(v120);
	return(TRUE);
    }
    /*
     * malloc the V.120
     */
    v120 = malloc_named(sizeof(v120_info_type), "V120 interface");
    if (v120 == NULL) {
	return(FALSE);
    }

    /*
     * Try to set up the vty here.  Might just want to get/reserve the
     * vty line, then open it when we are established.
     * Add the v120 to the list.
     */
    v120->hwptr = hwidb;
    if (v120_init_vty(v120, params) == FALSE) {
	free(v120);
	return(FALSE);
    }
    v120->next = v120_head;
    v120_head = v120;

    if (v120_debug)
	buginf("\n%s-v120 started", hwidb->hw_short_namestring);
    /*
     * Initialize the V.120 structure information
     */
    hwidb->isdn_info->v120info = v120;
    if (have_llc && params != NULL) {
	if (v120_debug)
	    buginf(" - Setting V.120 parameters from llc");
	v120->lli = V120_LLI_DEFAULT;
	v120->multi_frame = params->multiframe;
	v120->header_extension = params->header;
	v120->header_bytes = v120->header_extension ? 1 : 0;
	v120->params_from_call = TRUE;
	/* want to do stop, data parity and speed here */
    } else {
	if (v120_debug)
	    buginf(" - Setting default V.120 parameters");
	v120->lli = 256;
	v120->multi_frame = TRUE;
	v120->header_extension = TRUE;
	v120->header_bytes = 1;
	v120->params_from_call = FALSE;
    }
    queue_init(&v120->unackQ, 0);
    v120->I_called_death = FALSE;
    v120_change_to_tei_assigned(v120);
    /*
     * Save all the vectors and variables from the hwidb
     * put in the correct vectors for V.120.
     */
    status = raise_interrupt_level(ALL_DISABLE);
    v120->save.enctype = hwidb->enctype;
    hwidb->enctype = ET_V120;
    v120->save.iqueue = hwidb->iqueue;
    hwidb->iqueue = v120_iqueue;
    v120->save.parse_packet = hwidb->parse_packet;
    hwidb->parse_packet = v120_parse_packet;
    v120->save.vencap = hwidb->vencap;
    hwidb->vencap = v120_vencapsulate;
    v120->save.lineaction = hwidb->lineaction;
    hwidb->lineaction = NULL;
    v120->save.linestate = hwidb->linestate;
    hwidb->linestate = v120_line_state;
    v120->save.periodic = hwidb->periodic;
    hwidb->periodic = NULL;
    reset_interrupt_level(status);
    return(TRUE);
}

/*
 * v120_show
 * called from show_dialer
 * display the vty that we are hooked to
 */
static void v120_show (hwidbtype *hwidb)
{
    v120_info_type *v120;
    if (!(is_isdn(hwidb)) ||
	(hwidb->isdn_info->v120info == NULL))
	return;

    v120 = hwidb->isdn_info->v120info;
    printf("\nCall is V.120 - vty number %d", v120->v120_vty->ttynum);
}

/*
 * v120_stop_interface
 * remove the V.120 information from the b-channel interface
 */
static void v120_stop_interface (hwidbtype *hwidb)
{
    v120_info_type *v120, *p, *q;
    leveltype status;
    
    if (!(is_isdn(hwidb))) {
	return;
    }
    v120 = hwidb->isdn_info->v120info;
    if (v120 == NULL) {
	buginf("\nstop interface w/o v120");
	return;
    }
    if (v120_debug)
	buginf("\n%s:removing v120", hwidb->hw_short_namestring);
    /*
     * Get rid of all the output buffers
     */
    v120_dump_buffers(v120);

    /*
     * Stop the vty
     */
    v120_stop_vty(v120);
    /*
     * restore variables that have been hooked
     */
    status = raise_interrupt_level(ALL_DISABLE);
    hwidb->enctype = v120->save.enctype;
    hwidb->iqueue = v120->save.iqueue;
    hwidb->parse_packet = v120->save.parse_packet;
    hwidb->vencap = v120->save.vencap;
    hwidb->lineaction = v120->save.lineaction;
    hwidb->linestate = v120->save.linestate;
    hwidb->periodic = v120->save.periodic;

    hwidb->isdn_info->v120info = NULL;
    reset_interrupt_level(status);
    /*
     * remove from list of V120 interfaces
     */
    q = NULL;
    p = v120_head;
    while (p != v120 && p != NULL) {
	q = p;
	p = p->next;
    }
    if (p) {
	if (q == NULL) {
	    v120_head = p->next;
	} else {
	    q->next = p->next;
	}
    } else {
	if (v120_debug)
	    buginf("\nV120 not found on list");
    }

    free(v120);
}

/***************************************************************************
 * V120 autodetect
 ***************************************************************************/

/*
 * v120_autodetect_test
 *
 * tells us if we are seeing V120 packets on this B-channel
 */
static boolean v120_autodetect_test (hwidbtype *hwidb, paktype *pak)
{

    enum v120_packet_type type;
    uchar classify_error;

    /*
     * sanity check
     */
    if ((!pak) || (!hwidb)) {
	return (FALSE);
    }

    /*
     * Look for an SABME frame to detect V120 framing
     * This check may have to be extended to include
     * other packet types to make it more fool-proof
     */
    if (v120_debug || serial_debug) {
	uchar *data = pak->datagramstart;
	int ix;

	buginf("\nV120: Autodetect trying to detect V120 mode on %s",
	       (hwidb) ? hwidb->hw_short_namestring : "unknown");
	buginf("\nV120 sampled pkt:  %d bytes: ", pak->datagramsize);
	for (ix = 0;
	     ix < ((pak->datagramsize < 10) ? pak->datagramsize : 10);
	     ix++)
	    buginf(" %X", *(data+ix));
    }
    type = v120_classify_packet(pak, &classify_error);
    if (V120_SABME == type) {
	return (TRUE);
    }
    if (v120_debug || serial_debug) {
	buginf("..Failed");
    }
    return (FALSE);
}

/*
 * v120_autodetect_start_interface
 *
 * attempt to start v120 on this interface
 */
static boolean v120_autodetect_start_interface (hwidbtype *hwidb)
{

    /*
     * sanity check
     */
    if (!hwidb) {
	return (FALSE);
    }

    /*
     * attempt to slide the channel into V120 modem
     */
    return (v120_start_interface(hwidb, FALSE, NULL));
}

/*
 * v120_init
 * Initialize the V.120 subsystem.  Clear variables, add the
 * parser stuff, and register the functions
 */
static void v120_init (subsystype *subsys)
{
    v120_head = NULL;
    parser_add_command_list(v120_chain_init_table, "v120");
    v120_debug_init();
    v120_timer_fork = 0;
    v120_input_fork = 0;
    V120packetQ = NULL;
    v120_init_vty_vector();
    reg_add_v120_start_interface(v120_start_interface, "v120_start_interface");
    reg_add_v120_show(v120_show, "v120_show");
    reg_add_v120_stop_interface(v120_stop_interface, "v120_stop_interface");
    reg_invoke_serial_autodetect_register_encaps(ET_V120,
						 v120_autodetect_test,
						 v120_autodetect_start_interface);
}

#define V120_MAJVERSION 1
#define V120_MINVERSION 0
#define V120_EDITVERSION 1

SUBSYS_HEADER(v120, V120_MAJVERSION, V120_MINVERSION, V120_EDITVERSION,
	      v120_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      "req:dialer, isdn");

/* end file v120.c */
