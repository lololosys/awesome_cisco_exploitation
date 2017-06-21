/* $Id: fr_l2.c,v 3.1.68.16 1996/09/11 16:49:37 liqin Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_l2.c,v $
 *------------------------------------------------------------------
 * fr_l2.c -- Frame Relay layer 2 supporting SVC (LAPF)
 *
 * July 1995, Chifei Cheng
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_l2.c,v $
 * Revision 3.1.68.16  1996/09/11  16:49:37  liqin
 * CSCdi68280:  replace malloc with malloc_named in frame relay code
 * Branch: California_branch
 *
 * Revision 3.1.68.15  1996/08/28  13:21:22  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.68.14  1996/08/15  01:28:20  chifei
 * CSCdi66087:  LAPF T203 default should be 30 seconds
 * Branch: California_branch
 *
 * Revision 3.1.68.13  1996/08/07  09:04:31  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.68.12  1996/07/29  22:11:28  snyder
 * CSCdi64472:  declare some arrays const in shr_frame, shr_frame_svc
 *              2380 out of data, 40 image
 * Branch: California_branch
 *
 * Revision 3.1.68.11  1996/07/23  13:31:16  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.68.10  1996/07/04  17:05:38  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Moved command handling out of frame_relay_command()
 *
 * Revision 3.1.68.9  1996/07/03  01:57:44  chifei
 * CSCdi58333:  Some frame relay SVC commands are available in
 * non-enterprise images
 * Branch: California_branch
 * Modularized "frame svc" and "frame lapf" commands to go with FR SVC
 * subsystem; fixed/clean-up LAPF command format; consolidated DL
 * primitives define's.
 *
 * Revision 3.1.68.8  1996/06/28  23:31:58  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.68.7  1996/06/07  06:00:42  fowler
 * CSCdi59356:  All frame relay subinterface tests are failing - could
 *              be a broadcast
 *              Ensure LAPF packets aren't treated as
 *              broadcasts
 * Branch: California_branch
 *
 * Revision 3.1.68.6  1996/06/04  00:26:41  chifei
 * CSCdi57895:  show lapf display needs some clean-up
 * Branch: California_branch
 *
 * Revision 3.1.68.5  1996/05/23  14:52:39  chifei
 * CSCdi57895:  show lapf display needs some clean-up
 * Branch: California_branch
 *
 * Revision 3.1.68.4  1996/05/23  14:32:22  chifei
 * CSCdi57332:  FR SVC initialization point needs to be re-examined
 * Branch: California_branch
 * This is layer 2 specific change.
 *
 * Revision 3.1.68.3  1996/05/05  07:53:57  chifei
 * CSCdi56042:  modular-fr-svc image does not build
 * Branch: California_branch
 *
 * Revision 3.1.68.2  1996/04/30  21:38:59  chifei
 * CSCdi56098:  sh int shows svc info when the subsystem doesnt exist
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/04/25  23:23:21  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.72.1  1996/04/24  04:11:19  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1  1995/12/04  19:46:57  shankar
 * Placeholder for SVC development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "types.h"
#include "sched.h"
#include "../os/signal.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "passive_timers.h"
#include "mgd_timers.h"
#include "../if/network.h"
#include "serial.h"
#include "../if/priority.h"
#include "../ui/debug.h"
#include "../wan/parser_defs_frame.h"
#include "fr_l2.h"
#include "frame_relay.h"
#include "fr_lmi.h"
#include "fr_registry.h"
#include "fr_svc_registry.h"
#include "../wan/dialer_registry.h"
#include "fr_debug.h"


static boolean lapf_started;
static watched_queue	*Lapf_iQ,
			*Lapf_ackQ,
			*Lapf_inPakQ;
static mgd_timer *lapf_timer_master;
static pid_t	lapf_in_pid,
		lapf_outI_pid,
		lapf_bg_pid;
static Lapfcb_t *Lapf_requeue_head;
static int lapfcb_cnt;


/*
 * show lapf state and statistics
 */
static const char *const Lapf_states[LAPF_MAX_STATES] = {
    "TEI_ASSIGNED",
    "AWAITING_ESTABLISHMENT",
    "AWAITING_RELEASE",
    "MULTIPLE_FRAME_ESTABLISHED",
    "TIMER_RECOVERY"
};

/* This table matches enum lapf_err_code */
static const char *const lapf_showCause[LAPF_ERR_MAX] = {
			/* error code per spec: */
    "unsolic. S 1",
    "unsolic. DM 1",
    "unsolic. UA 1",
    "unsolic. UA 0",
    "unsolic. DM 0",
    "peer re-estab",
    "rexmt SABME err",
    "rexmt DISC err",
    "rexmt RR err",
    "N(R) err",
    "rcv FRMR",
    "unknown frame",
    "info too long",
    "length err",
    "N200 err",
			/* our own error code: */
    "memory out",
    "peer disconnect",
    "LMI down",
    "line down",
    "SVC disabled",
    "initial state",
    "L3 err",
    "setRexmt err",
    "updateVA err",
    "unAckQ err",
    "I queue err"
};


static const char *const Lapf_frames[LAPF_MAX_EVENTS] = {
	"I frame",
	"SABME",
	"UA",
	"DISC",
	"DM",
	"FRMR",
	"RR",
	"RNR",
	"REJ"
};


/* 
 * Internal utilities:
 */

/*
 * lapf_printPak() - print out the packet content for debugging purpsoes
 *	- given packet should have datagramstart set at beginning DLCI (0)
 */
static void lapf_printPak (
    Lapfcb_t *lapfcb,
    paktype *pak,
    boolean input,
    int index,
    uchar pf)
{
    Lapf_hdr_t *ptr;
    boolean rsp;
    ushort dlci, seq_num;

    ptr = (Lapf_hdr_t *)pak->datagramstart;
    dlci = GETSHORT(&ptr->dlci);
    seq_num = GETSHORT(&ptr->hdr);
    rsp = dlci & DLCI_CR_MASK ? TRUE : FALSE;
    if (lapfcb->lapf_flags2.simulate && input)
	rsp = (rsp) ? FALSE : TRUE;
    buginf("\n*LAPF %s %s %s %s p/f=%1d",
	lapfcb->hwidb->hw_namestring, 
	input ? "RX <-" : "TX ->",
	Lapf_frames[index], rsp ? "Rsp" : "Cmd", pf);
    switch (index) {
    case LAPF_I_INDEX:
	{
	int i, size;
	short *temp1;

	buginf(" N(S)=%d N(R)=%d pak=%x", seq_num >> 9, (seq_num & 0xff) >> 1,
		pak);
	buginf("\n");
	size = pak->datagramsize;
	temp1 = (short *)pak->datagramstart;
	/* convert short to char */
	if (size > 128)
	    size = 128;
	size = size >> 1;
	for (i = 0; i < size; i++, temp1++) {
	    buginf(" 0x%04x", GETSHORT(temp1));
	    if ( (i>0) && !((i+1) % 8))
		buginf("\n");
	}
	}
	break;

    case LAPF_RR_INDEX:
    case LAPF_RNR_INDEX:
    case LAPF_REJ_INDEX:
	/* S frame */
	buginf(", N(R)= %d", (seq_num & 0xff) >> 1);
	break;
    default:	/* U frame */
	break;
    }
}

/*
 * lapf_o_enqueue() - utility to do enqueue for the unAck queue
 *	(add to the end of the circular double linked list)
 */
static boolean lapf_o_enqueue (
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    Lapf_o_list_t *node;

    node = malloc_named(sizeof(Lapf_o_list_t), "FR LAPF Unack Queue Node");
    if (!node)
        return(FALSE);
    node->pak = pak;
    lapfcb->lapf_unAckQ.blink->flink = node;
    node->blink = lapfcb->lapf_unAckQ.blink;
    lapfcb->lapf_unAckQ.blink = node;
    node->flink = &lapfcb->lapf_unAckQ;
    return(TRUE);
}


/*
 * lapf_o_unqueue ()- unqueue from unAck queue which is a double linked list
 *	- i.e. take the latest out
 *	- return NULL if queue depleted
 */
static paktype *lapf_o_unqueue (Lapfcb_t *lapfcb)
{
    Lapf_o_list_t *node;
    paktype *pak;

    if ((node = lapfcb->lapf_unAckQ.blink) == &lapfcb->lapf_unAckQ)
	return(NULL);
    node->blink->flink = node->flink;
    node->flink->blink = node->blink;
    pak = node->pak;
    free(node);
    return(pak);
}

/*
 * lapf_o_dequeue ()- dequeue from unAck queue which is a double linked list
 *	- i.e. take the oldest out
 *	- return NULL if queue depleted
 */
static paktype *lapf_o_dequeue (Lapfcb_t *lapfcb)
{
    Lapf_o_list_t *node;
    paktype *pak;

    if ((node = lapfcb->lapf_unAckQ.flink) == &lapfcb->lapf_unAckQ)
	return(NULL);
    node->flink->blink = node->blink;
    node->blink->flink = node->flink;
    pak = node->pak;
    free(node);
    return(pak);
}

/*
 * lapf_o_list_init() - utility to initialize the unAck queue which is a
 *	circular double linked list
 */
static inline void lapf_o_list_init (Lapfcb_t *lapfcb)
{
    lapfcb->lapf_unAckQ.flink = lapfcb->lapf_unAckQ.blink = &lapfcb->lapf_unAckQ;
    lapfcb->lapf_unAckQ.pak = NULL;
}


/*
 * lapf_purge_unAckQ() - clean up unAck queue, return all buffers
 */
static void lapf_purge_unAckQ (Lapfcb_t *lapfcb) 
{
    Lapf_o_list_t *node, *nextnode;
    uint pakcnt;

    nextnode = lapfcb->lapf_unAckQ.flink;
    pakcnt = 0;
    while ((node = nextnode) != &lapfcb->lapf_unAckQ) {
	datagram_done(node->pak);
	nextnode = node->flink;
	free(node);
	pakcnt++;
    }
    if (lapf_debug & (pakcnt > 0)) {
	buginf("\n*LAPF %s purged %d pak's from unAckQ",
		lapfcb->hwidb->hw_namestring, pakcnt);
    }
    lapf_o_list_init(lapfcb);
}


/*
 * lapf_purgeQs() - utility to purge all queues for this lapf cb
 */
static void lapf_purgeQs (Lapfcb_t *lapfcb)
{
    leveltype level;
    paktype *pak;

    level = raise_interrupt_level(NETS_DISABLE);
    lapf_purge_unAckQ(lapfcb);
    while ((pak = p_dequeue(&lapfcb->lapf_iQ)))	{  /* purge I frame queue */
	lapfcb->lapf_stats.lapf_i_xmt_drop_cnt++;
	datagram_done(pak);
	if (lapf_debug)
	    buginf("\n*LAPF %s Dropped pak %x from I queue",
		lapfcb->hwidb->hw_namestring, pak);
    }
    lapfcb->lapf_k_cnt = 0;
    lapfcb->lapf_flags2.i_queued = \
	lapfcb->lapf_flags2.i_requeued = LAPF_BIT_CLEAR;
    reset_interrupt_level(level);
}


static inline void lapf_clearSeqCnts (Lapfcb_t *lapfcb)
{
    lapfcb->lapf_vs = lapfcb->lapf_va = lapfcb->lapf_vr = \
	lapfcb->lapf_ns = lapfcb->lapf_nr = 0;
    lapfcb->lapf_flags.flag.vs_wrap = LAPF_BIT_CLEAR;
}


/*
 * lapf_reset() - clean up control block, go to initial state
 */
static void lapf_reset (
	Lapfcb_t *lapfcb,
	fr_idb_struct_type *fr_idb,
	boolean tellL3)
{
    uchar rpt_type;

    if (lapfcb->lapf_state == TEI_ASSIGNED)
	return;		/* we're down already */
    if (lapf_debug)
	buginf("\n*LAPF %s Link reset", fr_idb->hwidb->hw_namestring);
    mgd_timer_stop(&lapfcb->lapf_timers);	/* stop all timers */
    lapfcb->lapf_n200_cnt = 0;
    fr_idb->lapfup = FALSE;
    lapfcb->lapf_state = TEI_ASSIGNED;
    if (lapfcb->lapf_flags2.clrStats)
	memset(&lapfcb->lapf_stats, 0, sizeof(struct Lapf_stats)); 
    else
	lapfcb->lapf_stats.lapf_reset_cnt++;
    if (tellL3) {
	if (lapf_debug)
	    buginf(" and clean up");
	lapf_purgeQs(lapfcb);
	lapf_clearSeqCnts(lapfcb);
	rpt_type = (lapfcb->lapf_flags.flag.rcvRel) ?
		   FR_DL_REL_CNF : FR_DL_REL_IND;
	if (!lapfcb->lapf_flags2.testL2)	/* normal function */
	    reg_invoke_fr_svc_l2_to_l3(rpt_type, fr_idb, NULL); 
	else	/* in L2 test mode */
	    lapf_l3_to_l2(rpt_type, fr_idb, NULL);
    }
    lapfcb->lapf_flags.allflags &= LAPF_FLAGS_RESET;
    lapfcb->echoI_cnt = 0;	/* test only */
}


/*
 * lapf_setPak() - utility to get pak ready for datagram_out
 */
static void lapf_setPak (
	paktype *pak,
	hwidbtype *idb)
{
    pak->if_output = idb->firstsw;
    pak->linktype = LINK_ADDRESS;
    pak->enctype = ET_FRAME_RELAY;
    pak->fr_flags &= ~PAK_FR_BROADCAST;
    pak->flags |= PAK_PRIORITY;
    if (idb->board_encap)
	(*idb->board_encap)(pak, idb);
    GET_TIMESTAMP(idb->lastoutput);
    pak->outputtime = idb->lastoutput;
}


/*
 *  lapf_send_i() -- send an I frame out
 *	caller should take care of the pak->refcnt before coming here
 */
static void lapf_send_i (
    paktype *pak,
    Lapfcb_t *lapfcb,
    ushort dlci,
    uchar pf)
{
    Lapf_hdr_t *i_ptr;
    leveltype level;
    ushort seq_num;

    if (!pak || !lapfcb)
	return;
    i_ptr = (Lapf_hdr_t *)pak->datagramstart;
    PUTSHORT(&i_ptr->dlci, dlci);   
    level = raise_interrupt_level(NETS_DISABLE);
    seq_num = ((ushort)lapfcb->lapf_vs) << 9;		/* N(S) */ 
    seq_num |= (ushort)((lapfcb->lapf_vr << 1) | pf);	/* N(R) */
    reset_interrupt_level(level);
    PUTSHORT(&i_ptr->hdr, seq_num);
    lapf_setPak(pak, lapfcb->hwidb);
    if (lapf_debug)
	lapf_printPak(lapfcb, pak, FALSE, LAPF_I_INDEX, pf);
    datagram_out(pak);
    return;
}

/*
 *  lapf_send_s() -- send an S frame out
 *	Return TRUE if packet sent, FALSE if out of memory or hw down
 */
static boolean lapf_send_s (
    uchar id,
    ushort dlci,
    Lapfcb_t *lapfcb,
    uchar pf)
{
    Lapf_hdr_t *s_ptr;
    paktype *pak;
    ushort hdr;

    if (!lapfcb || lapfcb->hwidb->state != IDBS_UP)
	return(FALSE);
    if (!(pak = getbuffer(sizeof(Lapf_hdr_t))))
	return(FALSE);
    s_ptr = (Lapf_hdr_t *)pak->network_start;
    PUTSHORT(&s_ptr->dlci, dlci);   
    hdr = (ushort)id << 8;
    hdr |= (ushort)((lapfcb->lapf_vr << 1) | pf);	/* N(R) */
    PUTSHORT(&s_ptr->hdr, hdr);
    pak->datagramsize = LAPF_S_SIZE;
    lapf_setPak(pak, lapfcb->hwidb);

    if (lapf_debug) {
	int index = 0;

	switch (id) {
	case LAPF_RR:
	    index = LAPF_RR_INDEX;
	    break;
	case LAPF_RNR:
	    index = LAPF_RNR_INDEX;
	    break;
	case LAPF_REJ:
	    index = LAPF_REJ_INDEX;
	    break;
	}
	if (index != 0)	/* just safety */
	    lapf_printPak(lapfcb, pak, FALSE, index, pf);
	else
	    buginf("\n*LAPF %s unknow S frame sent - ALERT!!",
		lapfcb->hwidb->hw_namestring);
    }
    datagram_out(pak);
    return(TRUE);
}

/*
 *  lapf_send_u() -- send an U frame to my peer
 *	Return TRUE if packet sent, FALSE if out of memory or hw down
 */
static boolean lapf_send_u (
    uchar hdr,
    ushort dlci,
    Lapfcb_t *lapfcb)
{
    hwidbtype *idb;
    paktype *pak;
    Lapf_hdr_t *u_ptr;

    if (!(pak = getbuffer(sizeof(Lapf_hdr_t))))
	return(FALSE);
    if (!lapfcb || !(idb = lapfcb->hwidb) || (idb->state != IDBS_UP))
	return(FALSE);
    u_ptr = (Lapf_hdr_t *)pak->network_start;
    PUTSHORT(&u_ptr->dlci, dlci);   
    PUTSHORT(&u_ptr->hdr, ((ushort)hdr) << 8);
    pak->datagramsize = LAPF_U_SIZE;	/* pak->datagramstart should be fine */
    lapf_setPak(pak, idb);

    if (lapf_debug) {
	int index = 0;
	uchar pf;

	pf = (hdr & ~LAPF_U_MASK) ? LAPF_BIT_SET : LAPF_BIT_CLEAR;
	switch (hdr & LAPF_U_MASK) {
	case LAPF_SABME:
	    index = LAPF_SABME_INDEX;
	    break;
	case LAPF_UA:
	    index = LAPF_UA_INDEX;
	    break;
	case LAPF_DISC:
	    index = LAPF_DISC_INDEX;
	    break;
	case LAPF_DM:
	    index = LAPF_DM_INDEX;
	    break;
	case LAPF_FRMR:
	    index = LAPF_FRMR_INDEX;
	    break;
	}
	if (index != 0)	/* just safety */
	    lapf_printPak((Lapfcb_t *)(idb->frame_relay_stuff->lapf_cb),
		pak, FALSE, index, pf);
	else
	    buginf("\n*LAPF %s unknow U frame sent - ALERT!!",
		lapfcb->hwidb->hw_namestring);
    }
    datagram_out(pak);
    return(TRUE);
}


/*
 *  lapf_kickoff() -- begin to establish multi-frame mode on DLCI 0
 */
static boolean lapf_kickoff (
	Lapfcb_t *lapfcb,
	fr_idb_struct_type *fr_idb)
{
    if (lapf_send_u((uchar)(LAPF_SABME | ~LAPF_U_MASK), LAPF_DLCI0_CMD,
	lapfcb)) {
	lapfcb->lapf_stats.lapf_sabme_xmt_cnt++;
	lapfcb->lapf_state = AWAITING_ESTABLISHMENT;
	fr_idb->lapfup = FALSE;
	mgd_timer_stop(&lapfcb->lapf_t203);
	mgd_timer_stop(&lapfcb->lapf_retry_timer);
	lapfcb->lapf_n200_cnt = 0;
	mgd_timer_start(&lapfcb->lapf_t200, lapfcb->lapf_t200_delay * (ONESEC/10));
	return (TRUE);
    } else {	/* failed to send SABME */
	mgd_timer_stop(&lapfcb->lapf_timers);
	lapfcb->lapf_n200_cnt = 0;
	mgd_timer_start(&lapfcb->lapf_retry_timer,
			lapfcb->lapf_retry_delay * ONESEC);
	if (lapf_debug)
	    buginf("\n*LAPF %s Failed to kick off, retry timer on",
		fr_idb->hwidb->hw_namestring);
	return (FALSE);
    }
}


static boolean LAPF_should_be_up (
    fr_idb_struct_type *fr_idb)
{
    if (lapf_debug && fr_svc_detd_debug) {
	boolean status;

	status = fr_idb && fr_idb->fr_svc_enabled && fr_idb->fr_lineup && \
		fr_idb->hwidb->state == IDBS_UP;
	buginf("\n*LAPF %s Lapf should be %s", fr_idb->hwidb->hw_namestring, 
		status ? "up" : "down");
	return (status);
    } else {
	return ((fr_idb) &&
		(fr_idb->fr_lineup) &&
		(fr_idb->fr_svc_enabled) && 
		(fr_idb->hwidb->state == IDBS_UP));
    }
}


/*
 * lapf_internalErr() - Lapf's internal error detected
 */
static void lapf_internalErr (
	Lapfcb_t *lapfcb,
	fr_idb_struct_type *fr_idb,
	uchar err_code)
{
	lapfcb->lapf_cause = err_code;
	if (lapf_debug)
	    buginf("\n*LAPF %s Internal error- %s",
		lapfcb->hwidb->hw_namestring,
		lapf_showCause[err_code - LAPF_ERR_SUP_F1]);
	if (lapf_send_u((uchar)LAPF_DISC, LAPF_DLCI0_CMD, lapfcb)) /* p=0 */
	    lapfcb->lapf_stats.lapf_disc_xmt_cnt++;
	lapf_reset(lapfcb, fr_idb, TRUE);
	if (LAPF_should_be_up(fr_idb)) {
	    lapf_kickoff(lapfcb, fr_idb);
	}
}


/*
 * Lapf_128* -  2 untilities below to do modulus 128 sequencing
 */

static inline uchar Lapf_128_decrement (uchar *seqNo)
{
    return((*seqNo = (*seqNo == 0) ? 127 : --(*seqNo)));
}

static inline uchar Lapf_128_increment (uchar *seqNo)
{
    return((*seqNo = (*seqNo == 127) ? 0 : ++(*seqNo)));
}


/*
 *  lapf_set_rexmt () -- set up the queue for re-transmission
 *	- move packets from unAck queue to  I frame queue
 *	- for any internal error, send DM(f=0) to re-extablish the link
 *	When done, V(S) = N(R)
 */
static boolean lapf_set_rexmt (
	Lapfcb_t *lapfcb,
	fr_idb_struct_type *fr_idb)
{
    paktype *pak;

    if (lapfcb->lapf_vs != lapfcb->lapf_nr) {
	if (lapf_debug)
	    buginf("\n*LAPF %s Rexmt unAck, V(S)= %d, N(R)= %d",
		lapfcb->hwidb->hw_namestring, lapfcb->lapf_vs, lapfcb->lapf_nr);
	while ((pak = lapf_o_unqueue(lapfcb))) {
	    pak->datagramsize -= sizeof(Lapf_hdr_t);
	    pak->datagramstart += sizeof(Lapf_hdr_t);
	    p_requeue(&lapfcb->lapf_iQ, pak);
	    if (lapfcb->lapf_flags2.i_queued == LAPF_BIT_CLEAR) {
		process_enqueue(Lapf_iQ, lapfcb);
		lapfcb->lapf_flags2.i_queued = LAPF_BIT_SET;
	    }
	    lapfcb->lapf_stats.lapf_rexmt_cnt++;
	    if ((Lapf_128_decrement(&lapfcb->lapf_vs)) == 127) {
		lapfcb->lapf_flags.flag.vs_wrap = LAPF_BIT_CLEAR;
	    }
	    if (lapfcb->lapf_vs == lapfcb->lapf_nr)
		break;
	}
	if (!pak) {
	    /* serious error! :-( reset the link */
	    lapf_internalErr(lapfcb, fr_idb, LAPF_ERR_SET_REXMT); 
	    return(FALSE);
	}
    }
    return(TRUE);
}


/*
 * lapf_in_sync() - checking on ( v(a) <= n(r) <= v(s) )
 */
static boolean lapf_in_sync (
    Lapfcb_t *lapfcb)
{
    uchar vs;

    if (lapf_debug && fr_svc_detd_debug) {
	buginf("\n*LAPF %sCheck sync, %sV(S)=%d V(A)=%d V(R)=%d N(R)=%d N(S)=%d",
	lapfcb->hwidb->hw_namestring,
	(lapfcb->lapf_flags.flag.vs_wrap == LAPF_BIT_SET) ? "VS wrapped ":"",
	lapfcb->lapf_vs, lapfcb->lapf_va, lapfcb->lapf_vr,
	lapfcb->lapf_nr, lapfcb->lapf_ns);
    }

    if (lapfcb->lapf_flags.flag.vs_wrap == LAPF_BIT_CLEAR)
	return((lapfcb->lapf_va <= lapfcb->lapf_nr) &&
		(lapfcb->lapf_nr <= lapfcb->lapf_vs));
    /* V(S) has wrapped */
    if (lapfcb->lapf_nr <= lapfcb->lapf_vs) {	/* n(r) also wrapped */
	if (lapfcb->lapf_nr == lapfcb->lapf_va) {
	    lapfcb->lapf_flags.flag.vs_wrap = LAPF_BIT_CLEAR;
	    return(TRUE);
	} else if (lapfcb->lapf_nr < lapfcb->lapf_va)
	    return(TRUE);
	else
	    return(FALSE);

    } else {	/* n(r) hasn't wrapped */
	vs = lapfcb->lapf_vs | 0x80;
	if (((vs - lapfcb->lapf_nr) <= lapfcb->lapf_k) &&
	    (lapfcb->lapf_va <= lapfcb->lapf_nr))
	    return(TRUE);
	else
	    return(FALSE);
    }
}


/*
 *  lapf_updateVA() -- updating my V(A) with received N(R).
 *	Upon invocation, must be V(A) != N(R) --normally should be v(a)<n(r).
 *	Clean up unAck queue along with V(A)'s change.
 *	Send DM(f=0) to re-extablish the link if we're corrupted.
 *	When done, V(A) = N(R)
 */
static boolean lapf_updateVA (
	Lapfcb_t *lapfcb,
	fr_idb_struct_type *fr_idb)
{
    paktype *pak;

    if (lapf_debug)
	buginf("\n*LAPF %s update V(A), nr = %d, va = %d",
	    fr_idb->hwidb->hw_namestring, lapfcb->lapf_nr, lapfcb->lapf_va);
    while ((pak = lapf_o_dequeue(lapfcb))) {
	datagram_done(pak);
	if ((Lapf_128_increment(&lapfcb->lapf_va)) == 0) { /* v(a) wrapped */
	    lapfcb->lapf_flags.flag.vs_wrap = LAPF_BIT_CLEAR;
	}
	if (lapfcb->lapf_va == lapfcb->lapf_nr)
	    break;
    }
    if (!pak) {
	/* serious error! :-( reset the link */
	lapf_internalErr(lapfcb, fr_idb, LAPF_ERR_NR_VA);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * lapf_outmem_down() -- out of memory, can't go on, bring the link down
 *	and tell L3 about it.
 */
static void lapf_outmem_down (
    Lapfcb_t *lapfcb,
    fr_idb_struct_type *fr_idb)
{
			/* typically failed to send U or S frame */
    if (lapf_debug)
	buginf("\n*LAPF No memory to xmt on %s in state %d",
		fr_idb->hwidb->hw_namestring, lapfcb->lapf_state + 4);
    lapfcb->lapf_flags.flag.rcvRel = LAPF_BIT_CLEAR;
    lapfcb->lapf_cause = LAPF_ERR_MEMORY;
    lapf_reset(lapfcb, fr_idb, TRUE);
    if (LAPF_should_be_up(fr_idb)) {
	mgd_timer_start(&lapfcb->lapf_retry_timer,
			lapfcb->lapf_retry_delay * ONESEC);
    } else {
	if (lapf_debug)
	    buginf("\n*LAPF %s Link down", fr_idb->hwidb->hw_namestring);
    }
}


/*
 * lapf_nr_recovery() - NR Recover procedure
 */
static void lapf_nr_recovery (
    Lapfcb_t *lapfcb,
    fr_idb_struct_type *fr_idb)
{
    lapfcb->lapf_cause = LAPF_ERR_NR;
    if (lapf_debug) {
	buginf("\n*LAPF %s NR_recovery, %sV(S)=%d V(A)=%d V(R)=%d N(R)=%d N(S)=%d",
	fr_idb->hwidb->hw_namestring,
	(lapfcb->lapf_flags.flag.vs_wrap == LAPF_BIT_SET) ? "VS wrapped ":"",
	lapfcb->lapf_vs, lapfcb->lapf_va, lapfcb->lapf_vr,
	lapfcb->lapf_nr, lapfcb->lapf_ns);
    }
    mgd_timer_stop(&lapfcb->lapf_timers);
    lapfcb->lapf_n200_cnt = 0;
    if (lapfcb->lapf_flags2.no_frmr == LAPF_BIT_CLEAR) {
	if (lapf_send_u((uchar)(LAPF_FRMR | ~LAPF_U_MASK), LAPF_DLCI0_RSP,
			lapfcb)) {
	    lapfcb->lapf_stats.lapf_frmr_xmt_cnt++;
	} else {
	    lapf_outmem_down(lapfcb, fr_idb);
	    return;
	}
    }
    lapfcb->lapf_flags.flag.rcvRel = LAPF_BIT_CLEAR;
    if (LAPF_should_be_up(fr_idb)) {
        lapf_reset(lapfcb, fr_idb, FALSE);	/* don't tell L3 yet */
        lapf_kickoff(lapfcb, fr_idb);
    } else {
        lapf_reset(lapfcb, fr_idb, TRUE);	/* tell L3 */
        if (lapf_debug)
    	buginf("\n*LAPF %s Link down", lapfcb->hwidb->hw_namestring);
    }
}


/*
 * External event handling begins:
 */

static void lapf_null (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s Wrong state (%d), frame (%x) ignored",
		fr_idb->hwidb->hw_namestring,
		lapfcb->lapf_state + 4, lapfcb->rcvFrame);
    lapfcb->lapf_stats.lapf_rcv_drop_cnt++;
    if (lapfcb->rcvFrame == LAPF_RCV_I)
	lapfcb->lapf_stats.lapf_i_rcv_drop_cnt++;
    lapfcb->lapf_stats.lapf_ignore_cnt++;
}

/*
 * State machine main routines follows.
 *	-- function naming convention is lapf_<frame>_<state#> 
 *	where <frame> = i, sabme, ua, dm, disc, rr, rnr, rej, or frmr
 *	      <state> = 4, 5, 6, or 7, or null if the case doesn't apply 
 *	See lapf_event_table below.
 */

/*
 * received I frame:
 */

static void lapf_i_7 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_i_7", fr_idb->hwidb->hw_namestring);
    if (lapfcb->lapf_flags.flag.rcvRsp) {	/* I response */
	if (lapfcb->lapf_ns == lapfcb->lapf_vr) {
	    Lapf_128_increment(&lapfcb->lapf_vr);
	    lapfcb->lapf_flags.flag.rejExcpt = LAPF_BIT_CLEAR;
	    pak->datagramsize -= sizeof(Lapf_hdr_t);
	    pak->datagramstart += sizeof(Lapf_hdr_t);
	    if (!lapfcb->lapf_flags2.testL2) { /* normal function */
		reg_invoke_fr_svc_l2_to_l3(FR_DL_DATA_IND, fr_idb, pak); 
		if (lapf_debug) {
		    buginf("\n*LAPF %s DL_DATA_IND", lapfcb->hwidb->hw_namestring);
		}
	    } else	/* in L2 test mode */
		lapf_l3_to_l2(FR_DL_DATA_IND, fr_idb, pak);
	    if (!lapfcb->lapf_flags.flag.ackPending) {
		lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_SET;
		process_enqueue(Lapf_ackQ, lapfcb);
	    }
	} else {
	    datagram_done(pak);	/* discard the frame */
	    if (!lapfcb->lapf_flags.flag.rejExcpt) {
		lapfcb->lapf_flags.flag.rejExcpt = LAPF_BIT_SET;
		if (lapf_send_s(LAPF_REJ, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_CLEAR))
		    lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		else {
		    lapf_outmem_down(lapfcb, fr_idb);
		    return;
		}
	    }
	}
	/*
	 * Per Q.922 SDL, I response should then mtake sure F=0, but the spec
	 * gives wrong error code. So we're simply not checking that which
	 * does not affect the state machine but just not reporting it.
	 *
	 * I response in state 7 continues later
	 */

    } else {	/* I command in state 7 */
	if (lapfcb->lapf_ns == lapfcb->lapf_vr) {
	    Lapf_128_increment(&lapfcb->lapf_vr);
	    lapfcb->lapf_flags.flag.rejExcpt = LAPF_BIT_CLEAR;
	    pak->datagramsize -= sizeof(Lapf_hdr_t);
	    pak->datagramstart += sizeof(Lapf_hdr_t);
	    if (!lapfcb->lapf_flags2.testL2) {	/* normal function */
		reg_invoke_fr_svc_l2_to_l3(FR_DL_DATA_IND, fr_idb, pak); 
		if (lapf_debug) {
		    buginf("\n*LAPF %s DL_DATA_IND",
			lapfcb->hwidb->hw_namestring);
		}
	    } else	/* in L2 test mode */
		lapf_l3_to_l2(FR_DL_DATA_IND, fr_idb, pak);
	    if (lapfcb->lapf_flags.flag.rcvPF) {
		if (lapf_send_s(LAPF_RR, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_SET)){
		    lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
		    lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		} else {
		    lapf_outmem_down(lapfcb, fr_idb);
		    return;
		}
	    } else {	/* p=0 */
		if (lapfcb->lapf_flags.flag.ackPending == LAPF_BIT_CLEAR) {
		    lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_SET;
		    process_enqueue(Lapf_ackQ, lapfcb);
		}
	    }
	} else {	/* n(s) <> v(r) */
	    if (lapf_debug)
		buginf("\n*LAPF %s I frame N(S) != V(R)",
			lapfcb->hwidb->hw_namestring);
	    datagram_done(pak);	/* discard this frame */
	    if (!lapfcb->lapf_flags.flag.rejExcpt) {
		lapfcb->lapf_flags.flag.rejExcpt = LAPF_BIT_SET;
		if (lapf_send_s(LAPF_REJ, LAPF_DLCI0_RSP,
				lapfcb, lapfcb->lapf_flags.flag.rcvPF))
		    lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		else {
		    lapf_outmem_down(lapfcb, fr_idb);
		    return;
		}
	    } else {
		if (lapfcb->lapf_flags.flag.rcvPF) {
		    if (lapf_send_s(LAPF_RR, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_SET)) {
			lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
			lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		    } else {
			lapf_outmem_down(lapfcb, fr_idb);
			return;
		    }
		}
	    }
	}
    }
    /* for both I command and I response */
    if (lapf_in_sync(lapfcb)) {
	if (lapfcb->lapf_flags.flag.peerRNR) {
	    if (lapfcb->lapf_va != lapfcb->lapf_nr)
		lapf_updateVA(lapfcb, fr_idb);
	} else if (lapfcb->lapf_nr == lapfcb->lapf_vs) { 
	    /* he's got all I've sent */
	    if (lapfcb->lapf_va != lapfcb->lapf_nr) {
		if (!lapf_updateVA(lapfcb, fr_idb))
	    	    return;
	    }
	    mgd_timer_stop(&lapfcb->lapf_t200);
	    mgd_timer_start(&lapfcb->lapf_t203, lapfcb->lapf_t203_delay * ONESEC);
	} else if (lapfcb->lapf_va != lapfcb->lapf_nr) {
	    if (!lapf_updateVA(lapfcb, fr_idb))
		    return;
	    mgd_timer_stop(&lapfcb->lapf_t203);
	    mgd_timer_start(&lapfcb->lapf_t200, lapfcb->lapf_t200_delay * (ONESEC/10));
	}
    } else	/* out of sync */
	lapf_nr_recovery(lapfcb, fr_idb);
}


static void lapf_i_8 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_i_8", fr_idb->hwidb->hw_namestring);
    if (lapfcb->lapf_flags.flag.rcvRsp) {	/* I response */
	if (lapfcb->lapf_ns == lapfcb->lapf_vr) {
	    /* he's sending as expected */
	    Lapf_128_increment(&lapfcb->lapf_vr);
	    lapfcb->lapf_flags.flag.rejExcpt = LAPF_BIT_CLEAR;
	    pak->datagramsize -= sizeof(Lapf_hdr_t);
	    pak->datagramstart += sizeof(Lapf_hdr_t);
	    if (!lapfcb->lapf_flags2.testL2) {	/* normal function */
		reg_invoke_fr_svc_l2_to_l3(FR_DL_DATA_IND, fr_idb, pak); 
		if (lapf_debug) {
		    buginf("\n*LAPF %s DL_DATA_IND",
			lapfcb->hwidb->hw_namestring);
		}
	    } else	/* in L2 test mode */
		lapf_l3_to_l2(FR_DL_DATA_IND, fr_idb, pak);
	    if (lapfcb->lapf_flags.flag.ackPending == LAPF_BIT_CLEAR) {
		lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_SET;
		process_enqueue(Lapf_ackQ, lapfcb);
	    }
	} else {
	    datagram_done(pak);	/* discard this re-xmited frame */
	    if (lapfcb->lapf_flags.flag.rejExcpt == LAPF_BIT_CLEAR) {
		lapfcb->lapf_flags.flag.rejExcpt = LAPF_BIT_SET;
		if (lapf_send_s(LAPF_REJ, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_CLEAR))
		    lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		else {
		    lapf_outmem_down(lapfcb, fr_idb);
		    return;
		}
	    }
	}
	if (lapf_in_sync(lapfcb)) {
	    if (lapfcb->lapf_flags.flag.rcvPF) {
		lapfcb->lapf_flags.flag.peerRNR = LAPF_BIT_CLEAR;
		if (lapfcb->lapf_va != lapfcb->lapf_nr)
		    if (!lapf_updateVA(lapfcb, fr_idb))
			return;
		mgd_timer_stop(&lapfcb->lapf_t200);
		mgd_timer_start(&lapfcb->lapf_t203, lapfcb->lapf_t203_delay * ONESEC);
		if (!lapf_set_rexmt(lapfcb, fr_idb))
		    return;	/* internal error, link reset */
		lapfcb->lapf_state = MULTIPLE_FRAME_ESTABLISHED;
	    } else {
		if (lapfcb->lapf_va != lapfcb->lapf_nr)
		    if (!lapf_updateVA(lapfcb, fr_idb))
			return;
	    }
	} else
	    lapf_nr_recovery(lapfcb, fr_idb);
	/* done with I response in state 8 */

    } else {	/* I command in state 8 */
	if (lapfcb->lapf_ns == lapfcb->lapf_vr) {
	    Lapf_128_increment(&lapfcb->lapf_vr);
	    lapfcb->lapf_flags.flag.rejExcpt = LAPF_BIT_CLEAR;
	    pak->datagramsize -= sizeof(Lapf_hdr_t);
	    pak->datagramstart += sizeof(Lapf_hdr_t);
	    if (!lapfcb->lapf_flags2.testL2) {	/* normal function */
		reg_invoke_fr_svc_l2_to_l3(FR_DL_DATA_IND, fr_idb, pak); 
		if (lapf_debug) {
		    buginf("\n*LAPF %s DL_DATA_IND",
			lapfcb->hwidb->hw_namestring);
		}
	    } else	/* in L2 test mode */
		lapf_l3_to_l2(FR_DL_DATA_IND, fr_idb, pak);
	    if (lapfcb->lapf_flags.flag.rcvPF) {
		if (lapf_send_s(LAPF_RR, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_SET)){
		    lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
		    lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		} else {
		    lapf_outmem_down(lapfcb, fr_idb);
		    return;
		}
	    } else {
		lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_SET;
		process_enqueue(Lapf_ackQ, lapfcb);
	    }
	} else {	/* n(s) <> v(r) */
	    datagram_done(pak);	/* discard this frame */
	    if (!lapfcb->lapf_flags.flag.rejExcpt) {
		lapfcb->lapf_flags.flag.rejExcpt = LAPF_BIT_SET;
		if (lapf_send_s(LAPF_REJ, LAPF_DLCI0_RSP, lapfcb,
			lapfcb->lapf_flags.flag.rcvPF))
		    lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		else {
		    lapf_outmem_down(lapfcb, fr_idb);
		    return;
		}
	    } else if (lapfcb->lapf_flags.flag.rcvPF) {
		if (lapf_send_s(LAPF_RR, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_SET)){
		    lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
		    lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		} else {
		    lapf_outmem_down(lapfcb, fr_idb);
		    return;
		}
	    }
	}
	if (lapf_in_sync(lapfcb)) {
	    if (lapfcb->lapf_va != lapfcb->lapf_nr) {
		if (!lapf_updateVA(lapfcb, fr_idb))
		    return;
	    }
	} else
	    lapf_nr_recovery(lapfcb, fr_idb);
    }	/* done with I command in state 8 */
}

static inline void Lapf_notSendMsg (
	uchar msg,
	Lapfcb_t *lapfcb)
{
    if (lapf_debug)
	buginf("\n*LAPF Memory low, can't send Message %s in state %d on %s",
		Lapf_frames[msg], lapfcb->lapf_state + 4,
		lapfcb->hwidb->hw_namestring);
}


/*
 * Received SABME:
 */
static void lapf_sabme_4 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    uchar pf_mask;

    if (lapf_debug)
	buginf("\n*LAPF %s lapf_sabme_4", fr_idb->hwidb->hw_namestring);
    pf_mask = lapfcb->lapf_flags.allflags & LAPF_U_PF_SET;
    if (LAPF_should_be_up(fr_idb)) {
	if (lapf_send_u((uchar)(LAPF_UA | pf_mask), LAPF_DLCI0_RSP, lapfcb)) {
	    mgd_timer_stop(&lapfcb->lapf_timers);
	    lapfcb->lapf_n200_cnt = 0;
	    lapfcb->lapf_stats.lapf_ua_xmt_cnt++;
	    lapf_clearSeqCnts(lapfcb);
	    if (!lapfcb->lapf_flags2.testL2)	/* normal function */
		reg_invoke_fr_svc_l2_to_l3(FR_DL_EST_IND, fr_idb, NULL); 
	    else	/* in L2 test mode */
		lapf_l3_to_l2(FR_DL_EST_IND, fr_idb, NULL);
	    mgd_timer_start(&lapfcb->lapf_t203, lapfcb->lapf_t203_delay * ONESEC);
	    lapfcb->lapf_state = MULTIPLE_FRAME_ESTABLISHED;
	    fr_idb->lapfup = TRUE;
	    if (lapf_debug)
		buginf("\n*LAPF %s Link up!", fr_idb->hwidb->hw_namestring);
	} else
	    lapfcb->lapf_cause = LAPF_ERR_MEMORY;
    } else {	/* stay down */
	if (lapf_send_u((uchar)(LAPF_DM | pf_mask), LAPF_DLCI0_RSP, lapfcb))
	    lapfcb->lapf_stats.lapf_dm_xmt_cnt++;
	else {	/* most likely out of memory */
	    lapfcb->lapf_cause = LAPF_ERR_MEMORY;
	    Lapf_notSendMsg(4, lapfcb);
	}
    }
}


static void lapf_sabme_5 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_sabme_5", fr_idb->hwidb->hw_namestring);
    if (lapf_send_u((uchar)(LAPF_UA | (lapfcb->lapf_flags.allflags & LAPF_U_PF_SET)),
		LAPF_DLCI0_RSP, lapfcb))
	lapfcb->lapf_stats.lapf_ua_xmt_cnt++;
    else {
	lapf_outmem_down(lapfcb, fr_idb);
	return;
    }
}


static void lapf_sabme_6 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_sabme_6", fr_idb->hwidb->hw_namestring);
    if (lapf_send_u((uchar)(LAPF_DM | (lapfcb->lapf_flags.allflags & LAPF_U_PF_SET)),
		LAPF_DLCI0_RSP, lapfcb))
	lapfcb->lapf_stats.lapf_dm_xmt_cnt++;
    else {
	lapf_outmem_down(lapfcb, fr_idb);
	return;
    }
}


static void lapf_sabme_78 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_sabme_78", fr_idb->hwidb->hw_namestring);
    if (lapf_send_u((uchar)(LAPF_UA | (lapfcb->lapf_flags.allflags & LAPF_U_PF_SET)),
		LAPF_DLCI0_RSP, lapfcb))
	lapfcb->lapf_stats.lapf_ua_xmt_cnt++;
    else {
	lapf_outmem_down(lapfcb, fr_idb);
	return;
    }
    if (lapfcb->lapf_vs != lapfcb->lapf_va) {
	lapf_purgeQs(lapfcb);
	if (!lapfcb->lapf_flags2.testL2)	/* normal function */
	    reg_invoke_fr_svc_l2_to_l3(FR_DL_EST_IND, fr_idb, NULL); 
	else	/* in L2 test mode */
	    lapf_l3_to_l2(FR_DL_EST_IND, fr_idb, NULL);
    }
    lapf_clearSeqCnts(lapfcb);
    lapfcb->lapf_cause = LAPF_ERR_PEER_SABME;
    if (lapfcb->lapf_flags2.clrStats)
	memset(&lapfcb->lapf_stats, 0, sizeof(struct Lapf_stats)); 
    lapfcb->lapf_flags.allflags &= LAPF_FLAGS_RESET;
    mgd_timer_stop(&lapfcb->lapf_timers);
    lapfcb->lapf_n200_cnt = 0;
    mgd_timer_start(&lapfcb->lapf_t203, lapfcb->lapf_t203_delay * ONESEC);
    lapfcb->lapf_state = MULTIPLE_FRAME_ESTABLISHED;
}

/*
 * Received UA:
 */
static void lapf_ua_4 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_ua_4", fr_idb->hwidb->hw_namestring);
    lapfcb->lapf_cause = lapfcb->lapf_flags.flag.rcvPF ?
			 LAPF_ERR_UA_F1 : LAPF_ERR_UA_F0;
    lapf_null(fr_idb, lapfcb, pak);
}


static void lapf_ua_5 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_ua_5", fr_idb->hwidb->hw_namestring);
    if (lapfcb->lapf_flags.flag.rcvPF) {
	mgd_timer_stop(&lapfcb->lapf_timers);
	lapfcb->lapf_n200_cnt = 0;
	if (lapfcb->lapf_flags.flag.rcvEst) {
	    lapfcb->lapf_flags.flag.rcvEst = LAPF_BIT_CLEAR;
	    lapf_purgeQs(lapfcb);
	    if (!lapfcb->lapf_flags2.testL2)	/* normal function */
		reg_invoke_fr_svc_l2_to_l3(FR_DL_EST_CNF, fr_idb, NULL); 
	    else	/* in L2 test mode */
		lapf_l3_to_l2(FR_DL_EST_CNF, fr_idb, NULL);
	} else if (lapfcb->lapf_vs != lapfcb->lapf_va) {
	    lapf_purgeQs(lapfcb);
	    if (lapf_debug)
		buginf("\n*LAPF %s Link reset for sequence number error",
			fr_idb->hwidb->hw_namestring);
	    if (!lapfcb->lapf_flags2.testL2)	/* normal function */
		reg_invoke_fr_svc_l2_to_l3(FR_DL_EST_IND, fr_idb, NULL); 
	    else	/* in L2 test mode */
		lapf_l3_to_l2(FR_DL_EST_IND, fr_idb, NULL);
	}
	lapf_clearSeqCnts(lapfcb);
	mgd_timer_start(&lapfcb->lapf_t203, lapfcb->lapf_t203_delay * ONESEC);
	lapfcb->lapf_state = MULTIPLE_FRAME_ESTABLISHED;
	fr_idb->lapfup = TRUE;
	if (lapf_debug)
	    buginf("\n*LAPF %s Link up!", fr_idb->hwidb->hw_namestring);
    } else {
	lapfcb->lapf_cause = LAPF_ERR_UA_F0;
	if (lapf_debug)
	    buginf("\n*LAPF %s unsolicited UA/f=0",
		fr_idb->hwidb->hw_namestring);
    }
}


static void lapf_ua_6 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_ua_6", fr_idb->hwidb->hw_namestring);
    if (lapfcb->lapf_flags.flag.rcvPF) {
	lapf_reset(lapfcb, fr_idb, TRUE);
	if (LAPF_should_be_up(fr_idb)) {
	    lapf_kickoff(lapfcb, fr_idb);	/* try to resume */
	}
    } else {
	if (lapf_debug)
	    buginf("\n*LAPF %s unsolicited UA/f=0",
		fr_idb->hwidb->hw_namestring);
    }
}

static void lapf_ua_78 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_ua_78", fr_idb->hwidb->hw_namestring);
    if (lapfcb->lapf_flags.flag.rcvPF) {
	lapfcb->lapf_cause = LAPF_ERR_UA_F1;
	if (lapf_debug)
	    buginf("\n*LAPF %s unsolicited UA/f=1",
		fr_idb->hwidb->hw_namestring);
    } else {
	lapfcb->lapf_cause = LAPF_ERR_UA_F0;
	if (lapf_debug)
	    buginf("\n*LAPF %s unsolicited UA/f=0",
		fr_idb->hwidb->hw_namestring);
    }
}

/*
 * Received DISC:
 */
static void lapf_disc_45 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{

    if (lapf_debug)
	buginf("\n*LAPF %s lapf_disc_45", fr_idb->hwidb->hw_namestring);
    if (lapf_send_u((uchar)(LAPF_DM | (lapfcb->lapf_flags.allflags & LAPF_U_PF_SET)),
		LAPF_DLCI0_RSP, lapfcb)) {
	lapfcb->lapf_stats.lapf_dm_xmt_cnt++;
    } else {
	lapf_outmem_down(lapfcb, fr_idb);
    }
    /* no state change */
}


static void lapf_disc_6 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_disc_6", fr_idb->hwidb->hw_namestring);
    if (lapf_send_u((uchar)(LAPF_UA | (lapfcb->lapf_flags.allflags & LAPF_U_PF_SET)),
		LAPF_DLCI0_RSP, lapfcb))
	lapfcb->lapf_stats.lapf_ua_xmt_cnt++;
    else {
	if (lapf_debug)
	    Lapf_notSendMsg(2, lapfcb);
    }
}


static void lapf_disc_78 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_disc_78", fr_idb->hwidb->hw_namestring);
    lapfcb->lapf_cause = LAPF_ERR_PEER_DISC;
    if (lapf_send_u((uchar)(LAPF_UA | (lapfcb->lapf_flags.allflags & LAPF_U_PF_SET)),
		LAPF_DLCI0_RSP, lapfcb)) {
	lapfcb->lapf_flags.flag.rcvRel = LAPF_BIT_CLEAR;
	lapfcb->lapf_flags.flag.rcvEst = LAPF_BIT_CLEAR;
	lapf_reset(lapfcb, fr_idb, TRUE);
	if (lapf_debug)
    	    buginf("\n*LAPF %s Link down for DISC, waiting for peer",
		   fr_idb->hwidb->hw_namestring);
    } else
	lapf_outmem_down(lapfcb, fr_idb);
}

/*
 * Received DM:
 */
static void lapf_dm_4 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_dm_4", fr_idb->hwidb->hw_namestring);
    if (!lapfcb->lapf_flags.flag.rcvPF && LAPF_should_be_up(fr_idb)) {
	lapf_kickoff(lapfcb, fr_idb);	/* try to resume */
    }
    /* otherwise, no action */
}


static void lapf_dm_5 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_dm_5", fr_idb->hwidb->hw_namestring);
    if (lapfcb->lapf_flags.flag.rcvPF) {
	lapfcb->lapf_cause = LAPF_ERR_DM_F1;
	lapf_reset(lapfcb, fr_idb, TRUE);
	if (lapf_debug)
	    buginf("\n*LAPF %s Link down for DM, waiting for peer",
		fr_idb->hwidb->hw_namestring);
	/* stay down and wait for peer's initiation */
    }
    /* otherwise, no action */
}


static void lapf_dm_6 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    uchar rpt_type;

    if (lapf_debug)
	buginf("\n*LAPF %s lapf_dm_6", fr_idb->hwidb->hw_namestring);
    if (lapfcb->lapf_flags.flag.rcvPF) {
	rpt_type = (lapfcb->lapf_flags.flag.rcvRel) ?
		   FR_DL_REL_CNF : FR_DL_REL_IND;
	if (!lapfcb->lapf_flags2.testL2)	/* normal function */
	    reg_invoke_fr_svc_l2_to_l3(rpt_type, fr_idb, NULL); 
	else	/* in L2 test mode */
	    lapf_l3_to_l2(rpt_type, fr_idb, NULL);
	mgd_timer_stop(&lapfcb->lapf_timers);
	lapfcb->lapf_n200_cnt = 0;
	lapfcb->lapf_state = TEI_ASSIGNED;
	if (LAPF_should_be_up(fr_idb)) {
	    mgd_timer_start(&lapfcb->lapf_retry_timer,
			    lapfcb->lapf_retry_delay * ONESEC);
	}
    }
    /* otherwise, no action */
}


static void lapf_dm_7 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_dm_7", fr_idb->hwidb->hw_namestring);
    if (!lapfcb->lapf_flags.flag.rcvPF) {
	lapfcb->lapf_cause = LAPF_ERR_DM_F0;
	if (LAPF_should_be_up(fr_idb)) {
	    lapf_reset(lapfcb, fr_idb, FALSE);
	    lapf_kickoff(lapfcb, fr_idb);	/* try to resume */
	} else {	/* or else clean up and stay down */
	    lapf_reset(lapfcb, fr_idb, TRUE);
	    if (lapf_debug)
		buginf("\n*LAPF %s Link down", fr_idb->hwidb->hw_namestring);
	}

    } else {	/* f=1 */
	lapfcb->lapf_cause = LAPF_ERR_DM_F1;
	/* stay in state 7 */
    }
}


static void lapf_dm_8 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_dm_8", fr_idb->hwidb->hw_namestring);
    if (lapfcb->lapf_flags.flag.rcvPF) {
	lapfcb->lapf_cause = LAPF_ERR_DM_F1;
    } else {
	lapfcb->lapf_cause = LAPF_ERR_DM_F0;
    }
    if (LAPF_should_be_up(fr_idb)) {
	lapf_reset(lapfcb, fr_idb, FALSE);
	lapf_kickoff(lapfcb, fr_idb);	/* try to resume */
    } else {	/* or else clean up and stay down */
	lapf_reset(lapfcb, fr_idb, TRUE);
	if (lapf_debug)
    	    buginf("\n*LAPF %s Link down", lapfcb->hwidb->hw_namestring);
    }
}

/*
 * Received FRMR:
 */
static void lapf_frmr_78 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_frmr_78", fr_idb->hwidb->hw_namestring);
    lapfcb->lapf_cause = LAPF_ERR_FRMR;
    if (LAPF_should_be_up(fr_idb)) {
	lapf_reset(lapfcb, fr_idb, FALSE);
	lapf_kickoff(lapfcb, fr_idb);
    } else {
	lapf_reset(lapfcb, fr_idb, TRUE);
	if (lapf_debug)
	    buginf("\n*LAPF %s L2 link down",
		fr_idb->hwidb->hw_namestring);
    }
}


/*
 * Received RR:
 */
static void lapf_rr_7 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_rr_7", fr_idb->hwidb->hw_namestring);
    lapfcb->lapf_flags.flag.peerRNR = LAPF_BIT_CLEAR;
    if (lapfcb->lapf_flags.flag.rcvPF) {
	if (lapfcb->lapf_flags.flag.rcvRsp)
	    lapfcb->lapf_cause = LAPF_ERR_SUP_F1;
	else {	/* RR cmd p=1 */
	    if (lapf_send_s(LAPF_RR, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_SET)) {
		lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
		mgd_timer_stop(&lapfcb->lapf_timers);
		lapfcb->lapf_n200_cnt = 0;
		lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
	    } else {
		lapf_outmem_down(lapfcb, fr_idb);
		return;
	    }
	}
    }	/* if PF = 0, no action required thus far */

    if (lapf_in_sync(lapfcb)) {
	if (lapfcb->lapf_nr == lapfcb->lapf_vs) {
	    if (lapfcb->lapf_va != lapfcb->lapf_nr)
		if (!lapf_updateVA(lapfcb, fr_idb))
		    return;
	    mgd_timer_stop(&lapfcb->lapf_t200);
	    lapfcb->lapf_n200_cnt = 0;
	    mgd_timer_start(&lapfcb->lapf_t203, lapfcb->lapf_t203_delay * ONESEC);
	} else if (lapfcb->lapf_va != lapfcb->lapf_nr) {
	    if (!lapf_updateVA(lapfcb, fr_idb))
		return;
	    mgd_timer_stop(&lapfcb->lapf_t203);
	    lapfcb->lapf_n200_cnt = 0;
	    mgd_timer_start(&lapfcb->lapf_t200, lapfcb->lapf_t200_delay * (ONESEC/10));
	}
    } else
	lapf_nr_recovery(lapfcb, fr_idb);
}


static void lapf_rr_rej_rnr_8 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    Lapf_hdr_t *s_ptr;
    ushort hdr;

    if (lapf_debug)
	buginf("\n*LAPF %s lapf_rr_rej_rnr_8", fr_idb->hwidb->hw_namestring);
    s_ptr = (Lapf_hdr_t *)pak->datagramstart;
    hdr = GETSHORT(&s_ptr->hdr);
    hdr >>= 8;	/* get the 3rd byte */
    if (hdr == LAPF_RNR)
	lapfcb->lapf_flags.flag.peerRNR = LAPF_BIT_SET;
    else 
	lapfcb->lapf_flags.flag.peerRNR = LAPF_BIT_CLEAR;

    if (!lapfcb->lapf_flags.flag.rcvRsp) {	/* command */
	if (lapfcb->lapf_flags.flag.rcvPF) {	/* p=1 */
	    /* enquiry response */
	    if (lapf_send_s(LAPF_RR, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_SET)) {
		lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
		lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
	    } else {
		lapf_outmem_down(lapfcb, fr_idb);
		return;
	    }
	}
    /* case of command continues */

    } else {	/* a response */
	if (lapfcb->lapf_flags.flag.rcvPF) {	/* f=1, exactly what we need */
	    if (lapf_in_sync(lapfcb)) {
		if (lapfcb->lapf_va != lapfcb->lapf_nr) {
		    if (!lapf_updateVA(lapfcb, fr_idb))
			return;
		}
		if (hdr == LAPF_RNR) 
		    /* not sure if need to reset n200_cnt? */
		    mgd_timer_start(&lapfcb->lapf_t200,
				    lapfcb->lapf_t200_delay * (ONESEC/10));
		else {
		    mgd_timer_stop(&lapfcb->lapf_t200);
		    mgd_timer_start(&lapfcb->lapf_t203,
				    lapfcb->lapf_t203_delay * ONESEC);
		}
		if (!lapf_set_rexmt(lapfcb, fr_idb))
		    return;	/* internal error, link reset */
		lapfcb->lapf_state = MULTIPLE_FRAME_ESTABLISHED;
	    } else 
		lapf_nr_recovery(lapfcb, fr_idb);
	    return;
	}	/* case f=0 falls through */
    }

    if (lapf_in_sync(lapfcb)) {
	if (lapfcb->lapf_va != lapfcb->lapf_nr)
	    lapf_updateVA(lapfcb, fr_idb);
    } else
	lapf_nr_recovery(lapfcb, fr_idb);
}


/*
 * Received RNR:
 */

static void lapf_rnr_7 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_rnr_7", fr_idb->hwidb->hw_namestring);
    lapfcb->lapf_flags.flag.peerRNR = LAPF_BIT_SET;
    if (lapfcb->lapf_flags.flag.rcvPF) {
	if (lapfcb->lapf_flags.flag.rcvRsp)
	    lapfcb->lapf_cause = LAPF_ERR_SUP_F1;
	else
	    if (lapf_send_s(LAPF_RR, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_SET)) {
		lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
		lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
	    } else {
		lapf_outmem_down(lapfcb, fr_idb);
		return;
	    }
    }
    if (lapf_in_sync(lapfcb)) {
	if (lapfcb->lapf_va != lapfcb->lapf_nr)
	    if (!lapf_updateVA(lapfcb, fr_idb))
		return;
	mgd_timer_stop(&lapfcb->lapf_t203);
	lapfcb->lapf_n200_cnt = 0;
	mgd_timer_start(&lapfcb->lapf_t200, lapfcb->lapf_t200_delay * (ONESEC/10));
    } else
	lapf_nr_recovery(lapfcb, fr_idb);
}


/*
 * Received REJ:
 */

static void lapf_rej_7 (
    fr_idb_struct_type *fr_idb,
    Lapfcb_t *lapfcb,
    paktype *pak)
{
    if (lapf_debug)
	buginf("\n*LAPF %s lapf_rej_7", fr_idb->hwidb->hw_namestring);
    lapfcb->lapf_flags.flag.peerRNR = LAPF_BIT_CLEAR;
    if (lapfcb->lapf_flags.flag.rcvPF) {
	if (lapfcb->lapf_flags.flag.rcvRsp)
	    lapfcb->lapf_cause = LAPF_ERR_SUP_F1;
	else
	    if (lapf_send_s(LAPF_RR, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_SET)) {
		lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
		lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
	    } else {
		lapf_outmem_down(lapfcb, fr_idb);
		return;
	    }
    }
    if (lapf_in_sync(lapfcb)) {
	if (lapfcb->lapf_va != lapfcb->lapf_nr)
	    lapf_updateVA(lapfcb, fr_idb);
	mgd_timer_stop(&lapfcb->lapf_t200);
	mgd_timer_start(&lapfcb->lapf_t203, lapfcb->lapf_t203_delay * ONESEC);
	lapf_set_rexmt(lapfcb, fr_idb);
    } else
	lapf_nr_recovery(lapfcb, fr_idb);
}


    
/*
 * LAPF external event table -- indexed by event and current state
 *	- with rows in the order of enum Lapf_event_index
 *	  and columns in the order of enum Lapf_state_types
 */
static void (*const lapf_event_table[LAPF_MAX_EVENTS][LAPF_MAX_STATES])
			(fr_idb_struct_type *, Lapfcb_t *, paktype *) =
{
    {lapf_null,  lapf_null,  lapf_null,  lapf_i_7,  lapf_i_8},
    {lapf_sabme_4,  lapf_sabme_5,  lapf_sabme_6,  lapf_sabme_78,  lapf_sabme_78},
    {lapf_ua_4,  lapf_ua_5,  lapf_ua_6,  lapf_ua_78,  lapf_ua_78},
    {lapf_disc_45,  lapf_disc_45,  lapf_disc_6,  lapf_disc_78,  lapf_disc_78},
    {lapf_dm_4,  lapf_dm_5,  lapf_dm_6,  lapf_dm_7,  lapf_dm_8},
    {lapf_null,  lapf_null,  lapf_null,  lapf_frmr_78,  lapf_frmr_78},
    {lapf_null,  lapf_null,  lapf_null,  lapf_rr_7,  lapf_rr_rej_rnr_8},
    {lapf_null,  lapf_null,  lapf_null,  lapf_rnr_7,  lapf_rr_rej_rnr_8},
    {lapf_null,  lapf_null,  lapf_null,  lapf_rej_7,  lapf_rr_rej_rnr_8}
};

    


/*
 * lapf_frameRej - format error in received frame, do soft reset
 */
static void lapf_frameRej (
    Lapfcb_t *lapfcb,
    fr_idb_struct_type *fr_idb,
    uchar cause)
{
    lapfcb->lapf_cause = cause;
    if (lapf_debug)
	buginf("\n*LAPF %s error=%c, invalid frame rcv'd in state %d",
		lapfcb->hwidb->hw_namestring, cause, lapfcb->lapf_state + 4);
    lapfcb->lapf_stats.lapf_rcv_drop_cnt++;
    if (lapfcb->rcvFrame == LAPF_RCV_I)
	lapfcb->lapf_stats.lapf_i_rcv_drop_cnt++;
    if (fr_idb->lapfup) {	/* in state 7 or 8 */
	lapfcb->lapf_flags.flag.rcvRel = LAPF_BIT_CLEAR;
	if (LAPF_should_be_up(fr_idb)) {
	    lapf_reset(lapfcb, fr_idb, FALSE);
	    lapf_kickoff(lapfcb, fr_idb);	/* try to resume */
	} else
	    lapf_reset(lapfcb, fr_idb, TRUE);
	    if (lapf_debug)
		buginf("\n*LAPF %s Link down", fr_idb->hwidb->hw_namestring);
    }
}


/*
 * lapf_input_teardown() - teardown a process upon SIGEXIT signal
 */
static void lapf_input_teardown (
    int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_queue(Lapf_inPakQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(Lapf_inPakQ)) != NULL)
        retbuffer(pak);
    delete_watched_queue(&Lapf_inPakQ);
    lapf_in_pid = 0;
    Lapf_inPakQ = NULL;
}


/*
 * lapf_inPak_process () -- handle incoming LAPF packets, 
 *	excluding UI frames for LMI
 */
static process lapf_inPak_process (void)
{
    int dismiss_count;
    hwidbtype *idb;
    paktype *pak;
    fr_idb_struct_type *fr_idb;
    Lapfcb_t *lapfcb;
    Lapf_hdr_t *ptr;
    ushort dlci, lapf_id;
    uchar pf_tmp, id_tmp, nr_tmp;
    
    process_wait_on_system_init();
    signal_permanent(SIGEXIT, lapf_input_teardown);
    Lapf_inPakQ = create_watched_queue("LAPF packets", 0, 0);
    process_watch_queue(Lapf_inPakQ, ENABLE, RECURRING);

    while (TRUE) {
	process_wait_for_event();	/* awaken by pak present */
	dismiss_count = PAK_SWITCHCOUNT;  /* only a limited number per pass */
	while (TRUE) {
	    if (--dismiss_count == 0)
		break;	
	    if ((pak = process_dequeue(Lapf_inPakQ)) == NULL)
		break;		/* queue empty */
	    if (!(idb = hwidb_or_null(pak->if_input))) {
		buginf("\nNET: source idb not set, PC %#x", current_pc());
		datagram_done(pak);
		continue;
	    }
	    if (!(fr_idb = idb->frame_relay_stuff) ||
		!(lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb))) {
		if (lapf_debug)
	    	    buginf("\n*LAPF %s Incoming frame lost control block, ignored",
			fr_idb->hwidb->hw_namestring);
		datagram_done(pak);
		continue;
	    }
	    GET_TIMESTAMP(idb->lastinput);  /* remember time of last input */
	    pak->inputtime = idb->lastinput;    /* save it in packet too */

	    ptr = (Lapf_hdr_t *)pak->datagramstart;
	    dlci = GETSHORT(&ptr->dlci);
	    lapfcb->lapf_flags.flag.rcvRsp = dlci & DLCI_CR_MASK ?
					LAPF_BIT_SET : LAPF_BIT_CLEAR;
	    if (lapfcb->lapf_flags2.simulate) {
		lapfcb->lapf_flags.flag.rcvRsp =
		    ~(lapfcb->lapf_flags.flag.rcvRsp);	/*flip for LAPD*/
	    }

	    lapf_id = GETSHORT(&ptr->hdr);
	    id_tmp = (uchar)(lapf_id >> 8);	/* get the 3rd byte */
	    nr_tmp = (uchar)(lapf_id & 0xff);	/* get the 4th byte */

	    if (!(id_tmp & LAPF_BIT_ONE)) { /* I frame */
		lapfcb->rcvFrame = LAPF_RCV_I;
		if (pak->datagramsize >
			(lapfcb->lapf_n201 + sizeof(Lapf_hdr_t))) {
		    lapf_frameRej(lapfcb, fr_idb, LAPF_ERR_I_FIELD);
		    datagram_done(pak);
		} else {
		    lapfcb->lapf_stats.lapf_Iframe_rcv_cnt++;
		    lapfcb->lapf_ns = id_tmp >> 1;
		    lapfcb->lapf_nr = nr_tmp >> 1;
		    pf_tmp = lapfcb->lapf_flags.flag.rcvPF =
			nr_tmp & LAPF_PF_MASK ?
			LAPF_BIT_SET : LAPF_BIT_CLEAR;
		    if (lapf_debug) {
			lapf_printPak(lapfcb, pak, TRUE, LAPF_I_INDEX, pf_tmp);
		    }
		    (*lapf_event_table[LAPF_I_INDEX][lapfcb->lapf_state])
			(fr_idb, lapfcb, pak);
		}

	    } else if (id_tmp & LAPF_BIT_TWO) {	/* U frame */
		lapfcb->rcvFrame = 0;
		if (pak->datagramsize > LAPF_U_SIZE) {
		    lapf_frameRej(lapfcb, fr_idb, LAPF_ERR_LENGTH);
		} else {
		    pf_tmp = lapfcb->lapf_flags.flag.rcvPF =
			id_tmp & LAPF_U_PF_SET ?
			LAPF_BIT_SET : LAPF_BIT_CLEAR;
		    switch (id_tmp & LAPF_U_MASK) {
		    case LAPF_SABME:
			lapfcb->rcvFrame = LAPF_RCV_SABME;
			if (lapfcb->lapf_flags.flag.rcvRsp) {
			    lapf_frameRej(lapfcb, fr_idb, LAPF_ERR_UNDEFINED);
			    break;
			}
		        lapfcb->lapf_stats.lapf_sabme_rcv_cnt++;
			if (lapf_debug)
			    lapf_printPak(lapfcb, pak, TRUE,
					LAPF_SABME_INDEX, pf_tmp);
		        (*lapf_event_table[LAPF_SABME_INDEX][lapfcb->lapf_state])
				(fr_idb, lapfcb, pak);
		        break;
		    case LAPF_DM:
			lapfcb->rcvFrame = LAPF_RCV_DSDM;
			if (!lapfcb->lapf_flags.flag.rcvRsp) {
			    lapf_frameRej(lapfcb, fr_idb, LAPF_ERR_UNDEFINED);
			    break;
			}
		        lapfcb->lapf_stats.lapf_dm_rcv_cnt++;
			if (lapf_debug)
			    lapf_printPak(lapfcb, pak, TRUE, LAPF_DM_INDEX,
				pf_tmp);
		        (*lapf_event_table[LAPF_DM_INDEX][lapfcb->lapf_state])
				(fr_idb, lapfcb, pak);
		        break;
		    case LAPF_DISC:
			lapfcb->rcvFrame = LAPF_RCV_DSDM;
			if (lapfcb->lapf_flags.flag.rcvRsp) {
			    lapf_frameRej(lapfcb, fr_idb, LAPF_ERR_UNDEFINED);
			    break;
			}
		        lapfcb->lapf_stats.lapf_disc_rcv_cnt++;
			if (lapf_debug)
			    lapf_printPak(lapfcb, pak, TRUE, LAPF_DISC_INDEX,
				pf_tmp);
		        (*lapf_event_table[LAPF_DISC_INDEX][lapfcb->lapf_state])
				(fr_idb, lapfcb, pak);
		        break;
		    case LAPF_UA:
			lapfcb->rcvFrame = LAPF_RCV_UA;
			if (!lapfcb->lapf_flags.flag.rcvRsp) {
			    lapf_frameRej(lapfcb, fr_idb, LAPF_ERR_UNDEFINED);
			    break;
			}
		        lapfcb->lapf_stats.lapf_ua_rcv_cnt++;
			if (lapf_debug)
			    lapf_printPak(lapfcb, pak, TRUE, LAPF_UA_INDEX,
				pf_tmp);
		        (*lapf_event_table[LAPF_UA_INDEX][lapfcb->lapf_state])
				(fr_idb, lapfcb, pak);
		        break;
		    case LAPF_FRMR:
			lapfcb->rcvFrame = LAPF_RCV_FRMR;
			if (!lapfcb->lapf_flags.flag.rcvRsp) {
			    lapf_frameRej(lapfcb, fr_idb, LAPF_ERR_UNDEFINED);
			    break;
			}
		        lapfcb->lapf_stats.lapf_frmr_rcv_cnt++;
			if (lapf_debug)
			    lapf_printPak(lapfcb, pak, TRUE, LAPF_FRMR_INDEX,
				pf_tmp);
		        (*lapf_event_table[LAPF_FRMR_INDEX][lapfcb->lapf_state])
				(fr_idb, lapfcb, pak);
		        break;
		    case LAPF_UI:
		        lapfcb->lapf_stats.lapf_ui_rcv_cnt++;
		        lapfcb->lapf_stats.lapf_ignore_cnt++;
		        break;
		    case LAPF_XID:
		        lapfcb->lapf_stats.lapf_xid_rcv_cnt++;
		        lapfcb->lapf_stats.lapf_ignore_cnt++;
		        break;
		    default:	/* undefined frame */
			lapfcb->rcvFrame = 0;
			lapf_frameRej(lapfcb, fr_idb, LAPF_ERR_UNDEFINED);
		        break;
		    }
		}
		datagram_done(pak);

	    } else {	/* S frame */
		if (pak->datagramsize > LAPF_S_SIZE) {	/* too big a frame */
		    lapf_frameRej(lapfcb, fr_idb, LAPF_ERR_LENGTH);
		} else {
		    pf_tmp = lapfcb->lapf_flags.flag.rcvPF = 
			nr_tmp & LAPF_PF_MASK ?
			LAPF_BIT_SET : LAPF_BIT_CLEAR;
		    lapfcb->lapf_nr = nr_tmp >> 1;
		    switch (id_tmp) {
		    case LAPF_RR:
			lapfcb->rcvFrame = LAPF_RCV_RR;
		        lapfcb->lapf_stats.lapf_rr_rcv_cnt++;
			if (lapf_debug)
			    lapf_printPak(lapfcb, pak, TRUE, LAPF_RR_INDEX,
				pf_tmp);
		        (*lapf_event_table[LAPF_RR_INDEX][lapfcb->lapf_state])
				(fr_idb, lapfcb, pak);
		        break;
		    case LAPF_RNR:
			lapfcb->rcvFrame = LAPF_RCV_RNR;
		        lapfcb->lapf_stats.lapf_rnr_rcv_cnt++;
			if (lapf_debug)
			    lapf_printPak(lapfcb, pak, TRUE, LAPF_RNR_INDEX,
				pf_tmp);
		        (*lapf_event_table[LAPF_RNR_INDEX][lapfcb->lapf_state])
				(fr_idb, lapfcb, pak);
		        break;
		    case LAPF_REJ:
			lapfcb->rcvFrame = LAPF_RCV_REJ;
		        lapfcb->lapf_stats.lapf_rej_rcv_cnt++;
			if (lapf_debug)
			    lapf_printPak(lapfcb, pak, TRUE, LAPF_REJ_INDEX,
				pf_tmp);
		        (*lapf_event_table[LAPF_REJ_INDEX][lapfcb->lapf_state])
				(fr_idb, lapfcb, pak);
		        break;
		    default:	/* undefined frame */
			lapf_frameRej(lapfcb, fr_idb, LAPF_ERR_UNDEFINED);
		    }
		}
		datagram_done(pak);
	    }
	}	/* next pak */
    }
}



/*
 * Internal event handling:
 */

static Lapfcb_t *lapf_setup(fr_idb_struct_type *fr_idb);

/*
 * lapf_input() -- check on input packet's header and queue up S/I/U frames
 *	Called from frame_relay_input() at the interrupt level
 *	frame must be prechecked to be for DLCI 0
 *	Should not get in here if running Cisco-typed LMI.
 *	Return TRUE if packet is queued up for further lapf parsing, or
 *		is dropped for error,
 *	Return FALSE if packet is not LAPF's interest.
 */
static boolean lapf_input (
	fr_idb_struct_type *fr_idb,
	paktype *pak)
{
    Lapfcb_t *lapfcb;
    Lapf_hdr_t *ptr;
    Lapf_id_t hdr;

    if (!fr_idb || !pak)
	return(FALSE);
    ptr = (Lapf_hdr_t *)(pak->datagramstart);
    /*
     * warning - be aware that lmi_idb is a hard define of fr_idb->fr_lmi
     */
    hdr.id = GETSHORT(&ptr->hdr);
    if ((fr_idb->fr_lmi) && (fr_idb->fr_lmi->fr_lmi_type != FR_CISCO_LMI)
	&& (hdr.oct.id1 == LAPF_UI && hdr.oct.id2 == ANSI_LMI_PROT_DISC)) {
	    return(FALSE);	/* LMI packet on DLCI 0 */
    }
    if (hdr.oct.id1 == LAPF_UI || hdr.oct.id1 == LAPF_XID) {
	return(FALSE);	/* see if other process wants it */
    }
    /* now likely to be mine, make sure lapf is running */
    lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb);
    if (!lapfcb) {
	if (LAPF_should_be_up(fr_idb))
	    lapfcb = lapf_setup(fr_idb);
    }
    if (lapfcb) {
	if (Lapf_inPakQ)
	    process_enqueue_pak(Lapf_inPakQ, pak);
	else
	    datagram_done(pak);	/* can't process this frame */
    } else {
	if (lapf_debug)
	    buginf("\n*LAPF %s input pak dropped for no ctrl block",
		fr_idb->hwidb->hw_namestring);
	datagram_done(pak);	/* can't process this frame */
    }
    return(TRUE);
}



/*
 * lapf_iq_teardown() - teardown a process upon SIGEXIT signal
 */
static void lapf_iq_teardown (
    int signal, int dummy1, void *dummy2, char *dummy3)
{
    process_watch_queue(Lapf_iQ, DISABLE, RECURRING);
    while (process_dequeue(Lapf_iQ) != NULL)
	;	/* should have been freed, if any */
    delete_watched_queue(&Lapf_iQ);
    lapf_outI_pid = 0;
    Lapf_iQ = NULL;
}

/*
 * lapf_put_requeueList() -
 *	add the CB that holds I frame in queue to a singler linked list 
 *	so that it gets enqueued for next round process
 */
static void
lapf_put_requeueList (Lapfcb_t *lapfcb)
{
    Lapfcb_t *node;

    if (Lapf_requeue_head == NULL)
	Lapf_requeue_head = lapfcb;
    else {
	for (node = Lapf_requeue_head; node->flink; node = node->flink)
	    ;
	node->flink = lapfcb;
    }
    lapfcb->flink = NULL;
}


/*
 * lapf_iQ_process () -- handle outgoing I frames
 */
static process lapf_iQ_process (void)
{
    int dismiss_count;
    paktype *pak = NULL;
    fr_idb_struct_type *fr_idb = NULL;
    Lapfcb_t *lapfcb = NULL;
    boolean error;
    
    process_wait_on_system_init();
    signal_permanent(SIGEXIT, lapf_iq_teardown);
    Lapf_iQ = create_watched_queue("LAPF I-frame output", 0, 0);
    process_watch_queue(Lapf_iQ, ENABLE, RECURRING);

    while (TRUE) {
	process_wait_for_event();	/* awaken by pak present */
	dismiss_count = PAK_SWITCHCOUNT;  /* only a limited number per pass */
	Lapf_requeue_head = NULL;
	/*
	 * serve all queued I frames as possible
	 */
	while (TRUE) {
	    if (dismiss_count <= 0) {
		break;	
	    }
	    if ((lapfcb = process_dequeue(Lapf_iQ)) == NULL)
		break;	/* queue empty */
	    /*
	     * validate lapfcb - necessary, to make sure it's not deleted
	     */
	    error = TRUE;
	    if (lapfcb->hwidb) {
		fr_idb = lapfcb->hwidb->frame_relay_stuff;
		if (fr_idb && (lapfcb == (Lapfcb_t *)fr_idb->lapf_cb))
		    error = FALSE;	/* normal path */
	    }
	    if (error) {
		if (lapf_debug)
		    buginf("\n*LAPF Bad lapfcb %x in Lapf_iQ", lapfcb);
		continue;	/* go serve next interface, if exists */
	    }

	    if (lapfcb->lapf_k_cnt == 0)
		break;	/* queue was drained */

	    pak = NULL;
	    /*
	     * serve this interface until queue empty or need re-queue
	     * (or internal error)
	     */
	    while ((pak = p_dequeue(&lapfcb->lapf_iQ)) != NULL) {
		if (dismiss_count-- == 0) {	/* have to dismiss */
		    p_requeue(&lapfcb->lapf_iQ, pak);
		    process_requeue(Lapf_iQ, lapfcb);
		    lapfcb = NULL;
		    break;	
		}
		if (lapfcb->lapf_flags.flag.peerRNR ||
		    (lapfcb->lapf_vs ==
		    ((lapfcb->lapf_va + lapfcb->lapf_k) & 0x7f)) ||
		    (lapfcb->lapf_state != MULTIPLE_FRAME_ESTABLISHED)) {
		    p_requeue(&lapfcb->lapf_iQ, pak);
		    lapf_put_requeueList(lapfcb);
		    lapfcb = NULL;
		    if (lapf_debug &&
			lapfcb->lapf_flags2.i_requeued == LAPF_BIT_CLEAR) {
			buginf("\n*LAPF %s pak %x REqueued",
			      lapfcb->hwidb->hw_namestring, pak);
			lapfcb->lapf_flags2.i_requeued = LAPF_BIT_SET;
		    }
		    break;	
		}
		/* now OK to send this I frame out */
		pak_lock(pak);	/* bumps up refcount */
		pak->datagramsize += sizeof(Lapf_hdr_t);
		pak->datagramstart -= sizeof(Lapf_hdr_t);
		lapf_send_i(pak, lapfcb, LAPF_DLCI0_CMD, LAPF_BIT_CLEAR);
		lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		lapfcb->lapf_flags2.i_requeued = LAPF_BIT_CLEAR;
		lapfcb->lapf_k_cnt--;
		lapfcb->lapf_stats.lapf_Iframe_xmt_cnt++;
		if ((Lapf_128_increment(&lapfcb->lapf_vs)) == 0) {
		    lapfcb->lapf_flags.flag.vs_wrap = LAPF_BIT_SET;
		}
		/* now do the un-ack queue list */
		if (!lapf_o_enqueue(lapfcb, pak)) {     /* out of memory */
		    datagram_done(pak);
		    lapf_outmem_down(lapfcb, fr_idb);
		    break;
		}
		if (!mgd_timer_running(&lapfcb->lapf_t200)) {
		    mgd_timer_stop(&lapfcb->lapf_t203);
		    mgd_timer_start(&lapfcb->lapf_t200,
				lapfcb->lapf_t200_delay * (ONESEC/10));
		}
	    }
	    /* done with one I queue */
	    if (lapfcb)
		lapfcb->lapf_flags2.i_queued = LAPF_BIT_CLEAR;
	}
	lapfcb = Lapf_requeue_head;
	while (lapfcb) {
	    process_enqueue(Lapf_iQ, lapfcb);
	    lapfcb = lapfcb->flink;
	}
    }
}



/*
 *  lapf_dataReq() -- Receive DL-Data-Reqest from layer 3
 *	state TEIassigned- error, something's wrong with layer 3
 *	state AwaitingEst- if not L3 initiated (link reset), queue it up
 *	state AwaitingRel- error, something's wrong with layer 3
 *	state MultiFrame- queue it up
 *	state TimerRecovery- queue it up
 */
static char lapf_dataReq (
    fr_idb_struct_type *fr_idb,
    paktype *pak)
{
    Lapfcb_t *lapfcb;

    if (!fr_idb || !(lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb)))
	return(FR_DL_ERROR);

    if (lapf_debug)
	buginf(" -in state %d", lapfcb->lapf_state + 4);
    switch (lapfcb->lapf_state) {
    case TEI_ASSIGNED:
    case AWAITING_ESTABLISHMENT:
    case AWAITING_RELEASE:
	lapfcb->lapf_stats.lapf_ignore_cnt++;
	return(FR_DL_ERROR);

    case MULTIPLE_FRAME_ESTABLISHED:
    case TIMER_RECOVERY:
	pak->if_output = lapfcb->hwidb->firstsw;
	lapfcb->lapf_k_cnt++;
	p_enqueue(&lapfcb->lapf_iQ, pak);
	if (lapfcb->lapf_flags2.i_queued == LAPF_BIT_CLEAR) {
	    process_enqueue(Lapf_iQ, lapfcb);
	    lapfcb->lapf_flags2.i_queued = LAPF_BIT_SET;
	}
	if (lapf_debug) {
	    buginf("\n*LAPF %s pak %x enqueued",
		lapfcb->hwidb->hw_namestring, pak);
	}
	return(FR_DL_PROCEED);
    }
    return(FR_DL_ERROR);
}



static void lapf_reset2 (	/* real clean for link down */
    Lapfcb_t *lapfcb)
{
    lapf_purgeQs(lapfcb);
    lapf_clearSeqCnts(lapfcb);
    lapfcb->lapf_flags.flag.rcvEst = LAPF_BIT_CLEAR;
    if (lapf_debug)
	buginf("\n*LAPF %s clean up Q's and counts",
		lapfcb->hwidb->hw_namestring);
}


/*
 *  lapf_relReq() -- Receive DL-Release-Reqest from layer 3
 *	state TEIassigned- strange but confirm anyway
 *	state AwaitingEst- bring down the link
 *	state AwaitingRel- remember to confirm
 *	state MultiFrame- bring down the link
 *	state TimerRecovery- bring down the link
 */
static char lapf_relReq (fr_idb_struct_type *fr_idb)
{
    Lapfcb_t *lapfcb;

    if (!fr_idb || !(lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb)))
	return(FR_DL_ERROR);
    
    if (lapf_debug)
	buginf(" -in state %d", lapfcb->lapf_state + 4);
    switch (lapfcb->lapf_state) {
    case TEI_ASSIGNED:
	lapfcb->lapf_cause = LAPF_ERR_DISABLED;
	mgd_timer_stop(&lapfcb->lapf_timers);	/* stop all timers */
	lapfcb->lapf_n200_cnt = 0;
	lapf_reset2(lapfcb);
	return(FR_DL_CONFIRM);

    case AWAITING_RELEASE:
	lapfcb->lapf_flags.flag.rcvRel = LAPF_BIT_SET;
	return(FR_DL_PROCEED);

    case AWAITING_ESTABLISHMENT:
	if (LAPF_should_be_up(fr_idb)) {	/* sanity */
	    if (lapf_debug)
		buginf("\n*LAPF %s Rcv'd Rel-Req but SVC enabled",
			fr_idb->hwidb->hw_namestring); 
	    return(FR_DL_ERROR);
	}
	lapfcb->lapf_cause = LAPF_ERR_DISABLED;
	if (lapf_send_u((uchar)(LAPF_DM | ~LAPF_U_MASK), LAPF_DLCI0_CMD,
		lapfcb)) {
	    lapfcb->lapf_stats.lapf_dm_xmt_cnt++;
	} else {
	    Lapf_notSendMsg(4, lapfcb);
	}
	lapf_reset(lapfcb, fr_idb, FALSE);
	lapf_reset2(lapfcb);
	return(FR_DL_CONFIRM);	/* confirm we're down */

    case MULTIPLE_FRAME_ESTABLISHED:
    case TIMER_RECOVERY:
	lapfcb->lapf_cause = LAPF_ERR_DISABLED;
	if (lapf_send_u((uchar)(LAPF_DISC | ~LAPF_U_MASK), LAPF_DLCI0_CMD,
			lapfcb)) {	/* DISC sent */
	    lapfcb->lapf_stats.lapf_disc_xmt_cnt++;
	    lapf_reset(lapfcb, fr_idb, FALSE);
	    lapf_reset2(lapfcb);
	    lapfcb->lapf_flags.flag.rcvRel = LAPF_BIT_SET;
	    lapfcb->lapf_state = AWAITING_RELEASE;
	    return(FR_DL_PROCEED);
	} else {
	    Lapf_notSendMsg(3, lapfcb);		/* DISC not sent */
	    lapf_reset(lapfcb, fr_idb, FALSE);
	    lapf_reset2(lapfcb);
	    return(FR_DL_CONFIRM);	/* confirm we're down */
	}
    }
    return(FR_DL_ERROR);
}



/*
 *  lapf_estReq() -- Receive DL-Establish-Reqest from layer 3
 *	state TEIassigned- kick off the establish process
 *	state AwaitingEst- remember that we got the request
 *	state AwaitingRel- error, something's wrong with layer 3
 *	state MultiFrame- re-establish the link
 *	state TimerRecovery- re-establish the link
 */
static char lapf_estReq (fr_idb_struct_type *fr_idb)
{
    Lapfcb_t *lapfcb;

    if (!fr_idb || !fr_idb->fr_svc_enabled) /*sanity*/
	return(FR_DL_ERROR);

    if (!(lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb))) {
	if (!(lapfcb = lapf_setup(fr_idb)))
	    return(FR_DL_ERROR);	/* out of memory */
    }
    if (lapf_debug)
	buginf(" -in state %d", lapfcb->lapf_state + 4);
    switch (lapfcb->lapf_state) {
    case TEI_ASSIGNED:
	if (LAPF_should_be_up(fr_idb)) {	
	    lapfcb->lapf_flags.flag.rcvEst = LAPF_BIT_SET;
	    lapf_purgeQs(lapfcb);
	    lapf_clearSeqCnts(lapfcb);
	    mgd_timer_stop(&lapfcb->lapf_timers);
	    lapfcb->lapf_n200_cnt = 0;
	    lapf_kickoff(lapfcb, fr_idb);
	    return(FR_DL_PROCEED);
	} else {
	    lapfcb->lapf_stats.lapf_ignore_cnt++;
	    return(FR_DL_ERROR);
	}

    case AWAITING_ESTABLISHMENT:
	lapf_purgeQs(lapfcb);
	lapf_clearSeqCnts(lapfcb);
	lapfcb->lapf_flags.flag.rcvEst = LAPF_BIT_SET;
	return(FR_DL_PROCEED);

    case AWAITING_RELEASE:
	if (LAPF_should_be_up(fr_idb)) {	
	    lapfcb->lapf_flags.flag.rcvEst = LAPF_BIT_SET;
	    mgd_timer_stop(&lapfcb->lapf_timers);
	    lapfcb->lapf_n200_cnt = 0;
	    lapf_kickoff(lapfcb, fr_idb);
	    return(FR_DL_PROCEED);
	} else {
	    if (lapf_debug)
		buginf("\n*LAPF %s Rcv'd Est-Req but SVC disabled",
			fr_idb->hwidb->hw_namestring); 
	    lapfcb->lapf_stats.lapf_ignore_cnt++;
	    return(FR_DL_ERROR);
	}

    case MULTIPLE_FRAME_ESTABLISHED:
    case TIMER_RECOVERY:
	lapfcb->lapf_cause = LAPF_ERR_L3;
	lapf_reset(lapfcb, fr_idb, FALSE);	/* don't tell L3 there */
	lapf_reset2(lapfcb);	/* clear up everything */
	if (LAPF_should_be_up(fr_idb)) {
	    lapfcb->lapf_flags.flag.rcvEst = LAPF_BIT_SET;
	    lapf_kickoff(lapfcb, fr_idb);	/* try to resume */
	    return(FR_DL_PROCEED);
	} else {
	    if (lapf_debug)
		buginf("\n*LAPF %s Got Est-Req but L2 should be down",
			fr_idb->hwidb->hw_namestring); 
	    lapfcb->lapf_stats.lapf_ignore_cnt++;
	    return(FR_DL_ERROR);
	}
    }
    return(FR_DL_ERROR);	/* safety */
}



/*
 * lapf_l3_to_l2() -- layer 3 to layer 2 event dispatcher
 *	Handles DL- Request sent from layer 3 
 */
char lapf_l3_to_l2 (
    uchar event,
    fr_idb_struct_type *fr_idb,
    paktype *pak)
{
    int rtncode;	/* for test only */

    if (!fr_idb)
	return(FR_DL_ERROR);
    switch (event) {
    case FR_DL_DATA_REQ:
	if (!lapf_debug)
	    return(lapf_dataReq(fr_idb, pak));
	else {
	    buginf("\n*LAPF %s DL_DATA_REQ", fr_idb->hwidb->hw_namestring);
	    rtncode = lapf_dataReq(fr_idb, pak);
	    if (rtncode == FR_DL_ERROR)
		buginf(" -DL_Req error");
	    return(rtncode);
	}
    case FR_DL_EST_REQ:
	if (!lapf_debug)
	    return(lapf_estReq(fr_idb));
	else {
	    buginf("\n*LAPF %s DL_EST_REQ", fr_idb->hwidb->hw_namestring);
	    rtncode = lapf_estReq(fr_idb);
	    if (rtncode == FR_DL_ERROR)
		buginf(" -DL_Req error");
	    return(rtncode);
	}
    case FR_DL_REL_REQ:
	if (!lapf_debug)
	    return(lapf_relReq(fr_idb));
	else {
	    buginf("\n*LAPF %s DL_REL_REQ", fr_idb->hwidb->hw_namestring);
	    rtncode = lapf_relReq(fr_idb);
	    if (rtncode == FR_DL_ERROR)
		buginf(" -DL_Req error");
	    return(rtncode);
	}
    default:
	datagram_done(pak);
	if (lapf_debug)
	    buginf("\n*LAPF %s Event %x ignored by LAPF",
		fr_idb->hwidb->hw_namestring, event);
	return(FR_DL_ERROR);
    }
}




/*
 *  lapf_t203_process() -- process idle time-out (DL-control's keepalive)
 *	state TEIassigned- error, stop timer 
 *	state AwaitingEst- error, stop timer 
 *	state AwaitingRel- error, stop timer 
 *	state MultiFrame- send RR, enter TimerRecovery
 *	state TimerRecovery- error, stop timer
 */
static void lapf_t203_process (
	Lapfcb_t *lapfcb,
	fr_idb_struct_type *fr_idb)
{
    if (lapfcb->lapf_state == MULTIPLE_FRAME_ESTABLISHED) {
	if (lapf_send_s(LAPF_RR, LAPF_DLCI0_CMD, lapfcb, LAPF_BIT_SET)) {
	    lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
	    lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
	    lapfcb->lapf_n200_cnt = 0;
	    mgd_timer_stop(&lapfcb->lapf_t203);
	    mgd_timer_start(&lapfcb->lapf_t200, lapfcb->lapf_t200_delay * (ONESEC/10));
	    lapfcb->lapf_state = TIMER_RECOVERY;
	} else {	/* failed to send RR */
	    lapfcb->lapf_cause = LAPF_ERR_REXMT_RR;
	    Lapf_notSendMsg(6, lapfcb);
	    lapfcb->lapf_flags.flag.rcvRel = LAPF_BIT_CLEAR;
	    lapf_reset(lapfcb, fr_idb, TRUE);
	    if (LAPF_should_be_up(fr_idb))
		mgd_timer_start(&lapfcb->lapf_retry_timer,
			lapfcb->lapf_retry_delay * ONESEC);
	    else
		if (lapf_debug)
		    buginf("\n*LAPF %s Link down",
			fr_idb->hwidb->hw_namestring);
	}
    } else {	/* somehow in other state */
	mgd_timer_stop(&lapfcb->lapf_t203);
    }
}



/*
 *  lapf_t200_process() -- process retransmission time out 
 *	state TEIassigned- error, stop timer 
 *	state AwaitingEst- re-xmt SABME if not yet N200, or else retry
 *	state AwaitingRel- re-xmt DISC if not yet N200, enter TEIassigned state
 *	state MultiFrame- send I or RR, enter TimerRecovery
 *	state TimerRecovery- re-xmt I or RR if not yet N200, or else release
 */
static void lapf_t200_process (
	Lapfcb_t *lapfcb,
	fr_idb_struct_type *fr_idb)
{
    paktype *pak;

    switch (lapfcb->lapf_state) {
    case TEI_ASSIGNED:
	mgd_timer_stop(&lapfcb->lapf_timers);	/* stop all timers */
	lapfcb->lapf_n200_cnt = 0;
	return;

    case AWAITING_ESTABLISHMENT:
 	/*
	 * state AwaitingEst- re-xmt SABME if not yet N200, or else retry
	 */
	if (lapfcb->lapf_n200_cnt < lapfcb->lapf_n200) {
	    if (lapf_send_u((uchar)(LAPF_SABME | ~LAPF_U_MASK), 
			     LAPF_DLCI0_CMD, lapfcb)) {
		lapfcb->lapf_stats.lapf_sabme_xmt_cnt++;
		lapfcb->lapf_n200_cnt++;
		mgd_timer_start(&lapfcb->lapf_t200, lapfcb->lapf_t200_delay * (ONESEC/10));
		return;
	    } else {
		lapfcb->lapf_cause = LAPF_ERR_REXMT_SABME;
		Lapf_notSendMsg(1, lapfcb);
	    }
	} else {
            lapfcb->lapf_cause = LAPF_ERR_REXMT;
	    if (lapf_debug)
		buginf("\n*LAPF %s Reaching N200 in state %d",
			fr_idb->hwidb->hw_namestring, lapfcb->lapf_state + 4);
	}
	/* reached N200 or failed to send SABME */
	lapf_reset(lapfcb, fr_idb, TRUE);
	if (LAPF_should_be_up(fr_idb)) {	
	    mgd_timer_start(&lapfcb->lapf_retry_timer,
			    lapfcb->lapf_retry_delay * ONESEC);
	}
	return;

    case AWAITING_RELEASE: {
	int rpt_type;

 	/*
	 * state AwaitingRel- re-xmt DISC if not yet N200, enter TEIassigned 
	 */
	if (lapfcb->lapf_n200_cnt < lapfcb->lapf_n200) {
	    if (lapf_send_u((uchar)(LAPF_DISC | ~LAPF_U_MASK), 
				LAPF_DLCI0_CMD, lapfcb)) {
		lapfcb->lapf_stats.lapf_disc_xmt_cnt++;
		lapfcb->lapf_n200_cnt++;
		mgd_timer_start(&lapfcb->lapf_t200, lapfcb->lapf_t200_delay * (ONESEC/10));
		return;
	    } else {
		lapfcb->lapf_cause = LAPF_ERR_REXMT_DISC;
		Lapf_notSendMsg(3, lapfcb);
	    }
	} else {
	    if (lapf_debug)
		buginf("\n*LAPF %s Reaching N200 in state %d",
			fr_idb->hwidb->hw_namestring,
			lapfcb->lapf_state + 4);
	    lapfcb->lapf_cause = LAPF_ERR_REXMT;
	}

	/* reached N200 or failed to send DISC */
	rpt_type = (lapfcb->lapf_flags.flag.rcvRel) ?
		   FR_DL_REL_CNF : FR_DL_REL_IND;
	if (!lapfcb->lapf_flags2.testL2)	/* normal function */
	    reg_invoke_fr_svc_l2_to_l3(rpt_type, fr_idb, NULL); 
	else	/* in L2 test mode */
	    lapf_l3_to_l2(rpt_type, fr_idb, NULL);
	if (LAPF_should_be_up(fr_idb))
	    mgd_timer_start(&lapfcb->lapf_retry_timer,
			lapfcb->lapf_retry_delay * ONESEC);
	return;
	}

    case MULTIPLE_FRAME_ESTABLISHED:
 	/*
	 * state MultiFrame- re-xmt I or RR, enter TimerRecovery
	 */
	lapfcb->lapf_n200_cnt = 0;	/* no rsp to last sent I/RR command */
	if (lapfcb->lapf_flags.flag.peerRNR) {	/* peer rcv busy */
	    if (lapf_send_s(LAPF_RR, LAPF_DLCI0_CMD, lapfcb, LAPF_BIT_SET)) {
		lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
		lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		mgd_timer_stop(&lapfcb->lapf_t203);
		mgd_timer_start(&lapfcb->lapf_t200,
	    			lapfcb->lapf_t200_delay * (ONESEC/10));
	    } else {	/* failed to send RR */
		lapfcb->lapf_cause = LAPF_ERR_REXMT_RR;
		lapfcb->lapf_flags.flag.rcvRel = LAPF_BIT_CLEAR;
		lapf_reset(lapfcb, fr_idb, TRUE);
		mgd_timer_start(&lapfcb->lapf_retry_timer,
			lapfcb->lapf_retry_delay * ONESEC);
		return;
	    }
	} else {
t200rexmtI:	/* rexmt last sent pak */
	    pak = lapfcb->lapf_unAckQ.blink->pak;
	    if (pak) {
		if (lapf_debug)
		    buginf("\n*LAPF %s Re-xmt!", fr_idb->hwidb->hw_namestring);
		if ((Lapf_128_decrement(&lapfcb->lapf_vs)) == 127)
		    lapfcb->lapf_flags.flag.vs_wrap = LAPF_BIT_CLEAR;
		pak_lock(pak);	/* bumps up refcount */
		lapf_send_i(pak, lapfcb, LAPF_DLCI0_CMD, LAPF_BIT_SET);
		lapfcb->lapf_stats.lapf_rexmt_cnt++;
		if ((Lapf_128_increment(&lapfcb->lapf_vs)) == 0)
		    lapfcb->lapf_flags.flag.vs_wrap = LAPF_BIT_SET;
		lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		mgd_timer_stop(&lapfcb->lapf_t203);	/* safety*/
		mgd_timer_start(&lapfcb->lapf_t200,
				lapfcb->lapf_t200_delay * (ONESEC/10));
	    } else {	/* queue empty */
		lapf_internalErr(lapfcb, fr_idb, LAPF_ERR_UNACK_Q);
		return;
	    }
	}
	lapfcb->lapf_n200_cnt++;
	lapfcb->lapf_state = TIMER_RECOVERY;
	return;
	
    case TIMER_RECOVERY:
 	/*
	 * state TimerRecovery- re-xmt I or RR if not yet N200, or else reset
	 */
	if (lapfcb->lapf_n200_cnt != lapfcb->lapf_n200) {
	    if (lapfcb->lapf_flags.flag.peerRNR ||
		lapfcb->lapf_vs == lapfcb->lapf_va) {
		if (lapf_send_s(LAPF_RR, LAPF_DLCI0_CMD, lapfcb, LAPF_BIT_SET)) {
		    lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
		    lapfcb->lapf_n200_cnt++;
		    lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		    mgd_timer_start(&lapfcb->lapf_t200,
	    			lapfcb->lapf_t200_delay * (ONESEC/10));
		    return;
		} else {	/* RR not sent */
		    lapf_outmem_down(lapfcb, fr_idb);
		    return;
		}
	    } else	/* V(S) > V(A) */
		goto t200rexmtI;

	} else {   /* reached n200, peer too quiet */
	    lapfcb->lapf_cause = LAPF_ERR_REXMT;
	    if (lapf_debug)
		buginf("\n*LAPF %s Reaching N200 in state %d",
			fr_idb->hwidb->hw_namestring, lapfcb->lapf_state + 4);
	    if (LAPF_should_be_up(fr_idb)) {
		lapf_reset(lapfcb, fr_idb, FALSE);
		lapf_kickoff(lapfcb, fr_idb);
	    } else {
		lapf_reset(lapfcb, fr_idb, TRUE);
		if (lapf_debug)
		    buginf("\n*LAPF %s Link down",
			fr_idb->hwidb->hw_namestring);
	    }
	    return;
	}
    }
}


    
/*
 *  lapf_timer_process () -- check timers and react if any expired
 */
static process lapf_timer_process (void)
{
    fr_idb_struct_type *fr_idb;
    Lapfcb_t *lapfcb;
    mgd_timer *lapf_exprdTimer;

    while (mgd_timer_expired(lapf_timer_master)) {
	lapf_exprdTimer = mgd_timer_first_expired(lapf_timer_master);
	fr_idb = mgd_timer_context(lapf_exprdTimer);
	lapfcb = (Lapfcb_t *)fr_idb->lapf_cb;
	if (!LAPF_should_be_up(fr_idb) || !lapfcb) {
	    mgd_timer_stop(lapf_exprdTimer);
	    lapfcb->lapf_n200_cnt = 0;
	    continue;
	}
	switch (mgd_timer_type(lapf_exprdTimer)) {
	  case LAPF_T200:
	    if (lapf_debug)
		buginf("\n*LAPF %s T200 expired, state = %d",
		    fr_idb->hwidb->hw_namestring, lapfcb->lapf_state + 4);
	    lapf_t200_process(lapfcb, fr_idb);
	    break;

	  case LAPF_T203:
	    if (lapf_debug)
		buginf("\n*LAPF %s T203 expired, state = %d",
		    fr_idb->hwidb->hw_namestring, lapfcb->lapf_state + 4);
	    lapf_t203_process(lapfcb, fr_idb);
	    break;

	  case LAPF_RETRY:
	    if (lapf_debug)
		buginf("\n*LAPF %s Retry timer expired, state = %d",
		    fr_idb->hwidb->hw_namestring,
		    lapfcb->lapf_state + 4);
	    if (!lapf_kickoff(lapfcb, fr_idb)) {  /* out of memory still */
		if ((lapfcb->lapf_vs != lapfcb->lapf_va) ||
		    (lapfcb->lapf_k_cnt > 0)) {
		    lapf_reset2(lapfcb);
		    /* do this only once during retry */
		    if (!lapfcb->lapf_flags2.testL2)  /* normal function */
		        reg_invoke_fr_svc_l2_to_l3(FR_DL_REL_IND, fr_idb, NULL); 
		    else	/* in L2 test mode */
		        lapf_l3_to_l2(FR_DL_REL_IND, fr_idb, NULL);
		}
	    }
	    break;

	  default:
	    mgd_timer_stop(lapf_exprdTimer);	/* stop this alien */
	}
    }
}


    
/*
 * lapf_ackPending_process() - handling ACK pending case for state 7/8
 */
static void lapf_ackPending_process (void)
{
    Lapfcb_t *lapfcb = NULL;
    fr_idb_struct_type *fr_idb = NULL;
    int loop_count;
    boolean error;
    
    if (lapf_debug)
	buginf("\n*LAPF AckPending");
    loop_count = PAK_SWITCHCOUNT;	/* safety */
    while (loop_count-- > 0) {
	if ((lapfcb = process_dequeue(Lapf_ackQ)) == NULL)
	    break;
	/*
	 * validate lapfcb - necessary, to make sure it's not deleted
	 */
	error = TRUE;
	if (lapfcb->hwidb) {
	    fr_idb = lapfcb->hwidb->frame_relay_stuff;
	    if (fr_idb && (lapfcb == (Lapfcb_t *)fr_idb->lapf_cb))
		error = FALSE;	/* normal path */
	}
	if (error) {
	    if (lapf_debug)
		buginf("\n*LAPF Bad lapfcb %x in Lapf_ackQ", lapfcb);
	    continue;	/* go serve next interface, if exists */
	}

	if ((lapfcb->lapf_flags.flag.ackPending) && fr_idb->lapfup) {
	    if (lapf_send_s(LAPF_RR, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_CLEAR)){
		lapfcb->lapf_flags.flag.ackPending = LAPF_BIT_CLEAR;
		lapfcb->lapf_stats.lapf_rr_xmt_cnt++;
	    } else {
		lapf_outmem_down(lapfcb, lapfcb->hwidb->frame_relay_stuff);
		break;
	    }
	}
    }
}
    
/*
 * lapf_bg_teardown() - teardown a process upon SIGEXIT signal
 */
static void lapf_bg_teardown (
    int signal, int dummy1, void *dummy2, char *dummy3)
{
    mgd_timer_stop(lapf_timer_master);
    process_watch_mgd_timer(lapf_timer_master, DISABLE);
    process_watch_queue(Lapf_ackQ, DISABLE, RECURRING);
    while (process_dequeue(Lapf_ackQ) != NULL)
	;	/* lapfcb should have been freed, if any */
    lapf_bg_pid = 0;
    Lapf_ackQ = NULL;
}


/*
 *  lapf_monitor() -- monitoring internal timer and flag events
 */
static process lapf_monitor (void)
{
    ulong major, minor;

    process_wait_on_system_init();
    signal_permanent(SIGEXIT, lapf_bg_teardown);
    Lapf_ackQ = create_watched_queue("LAPF Ack", 0, 0);
    process_watch_queue(Lapf_ackQ, ENABLE, RECURRING);
    process_watch_mgd_timer(lapf_timer_master, ENABLE);

    while (TRUE) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
		lapf_timer_process();
		break;

	      case QUEUE_EVENT:
		lapf_ackPending_process();
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}




/*
 *  lapf_lmidown() -- line protocol becomes down, determined by LMI
 *	state TEIassigned- no action
 *	state AwaitingEst- release process (xmt DISCp if hw up; else go down)
 *	state AwaitingRel- no action
 *	state MultiFrame- release process (xmt DISCp if hw up; else go down)
 *	state TimerRecovery- release process (xmt DISCp if hw up; else go down)
 */
static void lapf_lmidown (fr_idb_struct_type *fr_idb)
{
    Lapfcb_t *lapfcb;

    if (!fr_idb || !(lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb)))
	return;
    lapfcb->lapf_cause = LAPF_ERR_LMI;
    lapf_reset(lapfcb, fr_idb, TRUE); 
    if (lapf_debug)
	buginf("\n*LAPF %s link down for LMI down",
		fr_idb->hwidb->hw_namestring);
				/* and stay down until lmi comes up */
}

/*
 *  lapf_lineup() -- physical interface or line protocol (LMI) becomes up
 *	- called from fr_cstate(), i.e. hwidb->lineaction 
 *	- or from LMI changing its state from down to up
 */
static void lapf_lineup (fr_idb_struct_type *fr_idb)
{
    Lapfcb_t *lapfcb;

    if (!fr_idb)
	return;		/* stay down */
    if (!(lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb))) {
	if (!(lapfcb = lapf_setup(fr_idb)))
	    return;	/* out of memory */
    }
    if (fr_idb->hwidb->state != IDBS_UP)
	lapfcb->lapf_cause = LAPF_ERR_IFDOWN;
    else if (!fr_idb->fr_lineup)
	lapfcb->lapf_cause = LAPF_ERR_LMI;
    else if (!fr_idb->fr_svc_enabled)
	lapfcb->lapf_cause = LAPF_ERR_DISABLED;
    else if ((lapfcb->lapf_state != TEI_ASSIGNED) &&
	     (lapfcb->lapf_state != AWAITING_RELEASE))
	lapfcb->lapf_stats.lapf_ignore_cnt++;  /* we're in process already */
    else
	lapf_kickoff(lapfcb, fr_idb);
}

/*
 *  lapf_linedown() -- physical FR interface becomes down
 *	- called from fr_cstate(), i.e. hwidb->lineaction
 */
static void lapf_linedown (fr_idb_struct_type *fr_idb)
{
    Lapfcb_t *lapfcb;

    if (!fr_idb || !(lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb)))
	return;
    lapfcb->lapf_cause = LAPF_ERR_IFDOWN;
    lapf_reset(lapfcb, fr_idb, TRUE);
    if (lapf_debug)
	buginf("\n*LAPF %s Link down for line down",
		fr_idb->hwidb->hw_namestring);
}



/*
 *  lapf_teardown() -- tear down LAPF support for an FR interface
 *	- called when FR gets de-encapsulated
 *	- clean up before fr_idb is freed
 *	- even disable all processes if none interface do FR
 */
static void lapf_teardown (fr_idb_struct_type *fr_idb)
{
    Lapfcb_t *lapfcb;

    if (!(lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb)))
	return;
    mgd_timer_stop(&lapfcb->lapf_timers);	/* stop all timers */
    lapf_purgeQs(lapfcb);	/* purge all queues */
    free(lapfcb);		/* now, we're gone */
    fr_idb->lapf_cb = 0;
    fr_idb->lapfup = FALSE;
    if (--lapfcb_cnt <= 0) {
	if (lapf_in_pid)
	    process_kill(lapf_in_pid);
	if (lapf_outI_pid)
	    process_kill(lapf_outI_pid);
	if (lapf_bg_pid)
	    process_kill(lapf_bg_pid);
	lapfcb_cnt = 0;
	lapf_started = FALSE;
	if (lapf_debug)
	    buginf("\n*LAPF's gone!");
    }
}


/*
 * lapf_sys_event() -- framerelay layer 2 internal event dispatcher
 *	Handles PH-, MDL- primitives
 */
static void lapf_sys_event (
    uchar event,
    fr_idb_struct_type *fr_idb)
{
    if (!fr_idb)
	return;
    switch (event) {
    case LAPF_SHOW_IF:	/* support 'show int' */
	printf("\n  FR SVC %s, LAPF state %s",
		(fr_idb->fr_svc_enabled ? "enabled" : "disabled"),
		(fr_idb->lapfup? "up" : "down"));
	break;
    case FR_MDL_ASSIGN:
	lapf_lineup(fr_idb);
	break;
    case FR_MDL_REMOVE:
	lapf_lmidown(fr_idb);
	break;
    case FR_PH_DEACT:
	lapf_linedown(fr_idb);
	break;
    case FR_PH_ACT:
	lapf_lineup(fr_idb);
	break;
    case LAPF_TEARDOWN:
	lapf_teardown(fr_idb);
	break;
    case LAPF_CLR_COUNTERS: {
	Lapfcb_t *lapfcb;

	if ((lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb)))
	    memset(&lapfcb->lapf_stats, 0, sizeof(struct Lapf_stats)); 
	break;
      }
    default:
	if (lapf_debug) {
	    buginf("\n*LAPF %s Event %x ignored",
		fr_idb->hwidb->hw_namestring, event);
	    if (fr_idb->lapf_cb)
		((Lapfcb_t *)(fr_idb->lapf_cb))->lapf_stats.lapf_ignore_cnt++;
	}
    }
    return;
}



/*
 * lapf_showStats -  support the "show frame lapf" cmd
 */
static void lapf_showStats (
    Lapfcb_t *lapfcb,
    fr_idb_struct_type *fr_idb)
{
    uint t200;

    printf("\n\nInterface = %s (%s),  LAPF state = %s (%s)",
	   lapfcb->hwidb->hw_namestring,
	   (fr_idb->hwidb->state == IDBS_UP ? "up" : "down"),
	   Lapf_states[lapfcb->lapf_state],
	   fr_idb->lapfup ? "up" : "down");
    printf("\nSVC %s,  ", fr_idb->fr_svc_enabled ? "enabled" : "disabled");
    if (fr_idb->lapfup)
	printf("Last ");
    printf("link down cause = %s,  #link-reset = %d",
	lapf_showCause[lapfcb->lapf_cause - LAPF_ERR_SUP_F1],
	lapfcb->lapf_stats.lapf_reset_cnt);
    t200 = lapfcb->lapf_t200_delay / 10;
    printf("\nT200 = %d.%d sec.,  T203 = %d sec.,  N200 = %d,  k = %d,  N201 = %d",
	   t200, lapfcb->lapf_t200_delay - (t200 * 10), lapfcb->lapf_t203_delay,
	   lapfcb->lapf_n200, lapfcb->lapf_k, lapfcb->lapf_n201);
    printf("\nI-frame xmt = %d,  I-frame rcv = %d,  I-frame reXmt = %d",
	   lapfcb->lapf_stats.lapf_Iframe_xmt_cnt,
	   lapfcb->lapf_stats.lapf_Iframe_rcv_cnt,
	   lapfcb->lapf_stats.lapf_rexmt_cnt);
    printf("\nI xmt dropped = %d,  I rcv dropped = %d,  Rcv pak dropped = %d",
	   lapfcb->lapf_stats.lapf_i_xmt_drop_cnt,
	   lapfcb->lapf_stats.lapf_i_rcv_drop_cnt,
	   lapfcb->lapf_stats.lapf_rcv_drop_cnt);
    printf("\nRR xmt = %d,  RR rcv = %d,  RNR xmt = %d,  RNR rcv = %d",
	   lapfcb->lapf_stats.lapf_rr_xmt_cnt, lapfcb->lapf_stats.lapf_rr_rcv_cnt,
	   lapfcb->lapf_stats.lapf_rnr_xmt_cnt, lapfcb->lapf_stats.lapf_rnr_rcv_cnt);
    printf("\nREJ xmt = %d,  REJ rcv = %d,  FRMR xmt = %d,  FRMR rcv = %d",
	   lapfcb->lapf_stats.lapf_rej_xmt_cnt, lapfcb->lapf_stats.lapf_rej_rcv_cnt,
	   lapfcb->lapf_stats.lapf_frmr_xmt_cnt, lapfcb->lapf_stats.lapf_frmr_rcv_cnt);
    printf("\nDM xmt = %d,  DM rcv = %d,  DISC xmt = %d,  DISC rcv = %d",
	   lapfcb->lapf_stats.lapf_dm_xmt_cnt, lapfcb->lapf_stats.lapf_dm_rcv_cnt,
	   lapfcb->lapf_stats.lapf_disc_xmt_cnt, lapfcb->lapf_stats.lapf_disc_rcv_cnt);
    printf("\nSABME xmt = %d,  SABME rcv = %d,  UA xmt = %d,  UA rcv = %d",
	   lapfcb->lapf_stats.lapf_sabme_xmt_cnt, lapfcb->lapf_stats.lapf_sabme_rcv_cnt,
	   lapfcb->lapf_stats.lapf_ua_xmt_cnt, lapfcb->lapf_stats.lapf_ua_rcv_cnt);
    printf("\nV(S) = %d,  V(A) = %d,  V(R) = %d,  N(S) = %d,  N(R) = %d",
	   lapfcb->lapf_vs, lapfcb->lapf_va, lapfcb->lapf_vr,
	   lapfcb->lapf_ns, lapfcb->lapf_nr);
    printf("\n%smt FRMR at Frame Reject%s",
	   lapfcb->lapf_flags2.no_frmr ? "Not x" : "X",
	   lapf_debug ? ",  Lapf debug on" : "");
    if (lapfcb->lapf_flags2.testL2)
	printf("\nIn L2-only test mode,  %s L2 statistics,  Retry delay = %d sec",
		lapfcb->lapf_flags2.clrStats ? "Clear" : "Preserve",
		lapfcb->lapf_retry_delay);
    if (lapfcb->lapf_flags2.simulate)
	printf("\nIn LAPD simulation mode (user side)");
}



/*
 * lapf_showCmd() - handles "show frame-relay lapf" command
 */
static void lapf_showCmd (parseinfo *csb)
{
    fr_idb_struct_type *fr_idb;
    Lapfcb_t *lapfcb;
    hwidbtype *idb;
    boolean screenfull, display;

    screenfull = display = FALSE;
    automore_enable(NULL);
    if (GETOBJ(idb,1)) {	/* interface is specified */
	idb = GETOBJ(idb,1)->hwptr;
	if (!is_frame_relay(idb) || !(fr_idb = idb->frame_relay_stuff)) {
	    printf("\n%% LAPF- Invalid interface");
	} else if (!(lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb))) {
	    printf("\n%% LAPF- No control block");
	} else {
	    display = TRUE;
	    lapf_showStats(lapfcb, fr_idb);
	}
    } else {
	FOR_ALL_HWIDBS(idb) {
	    if (is_frame_relay(idb) && (fr_idb = idb->frame_relay_stuff) &&
		(lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb))) {
		if (screenfull)
		    automore_conditional(0);
		else
		    screenfull = TRUE;
		lapf_showStats(lapfcb, fr_idb);
		display = TRUE;
	    }
	}
    }
    automore_disable();
    if (display)
	printf("\n");
}


static void lapf_test_sabme (
    Lapfcb_t *lapfcb)
{
    fr_idb_struct_type *fr_idb;

    if (lapfcb && lapfcb->hwidb &&		/* don't care about lmi */
	(fr_idb = lapfcb->hwidb->frame_relay_stuff)) {
	lapf_reset(lapfcb, fr_idb, TRUE);
	lapf_kickoff(lapfcb, fr_idb);       /* send sabme */
    }
}

static void lapf_test_ua (
    Lapfcb_t *lapfcb)
{
    lapf_send_u((uchar)(LAPF_UA | ~LAPF_U_MASK), LAPF_DLCI0_RSP, lapfcb);
}

static void lapf_test_disc (Lapfcb_t *lapfcb)
{
    lapf_send_u((uchar)(LAPF_DISC | ~LAPF_U_MASK), LAPF_DLCI0_CMD, lapfcb);
}

static void lapf_test_dm (Lapfcb_t *lapfcb)
{
    lapf_send_u((uchar)(LAPF_DM | ~LAPF_U_MASK), LAPF_DLCI0_RSP, lapfcb);
}

static void lapf_test_frmr (Lapfcb_t *lapfcb)
{
    lapf_send_u((uchar)(LAPF_FRMR | ~LAPF_U_MASK), LAPF_DLCI0_RSP, lapfcb);
}

static void lapf_test_rr (Lapfcb_t *lapfcb)
{
    lapf_send_s(LAPF_RR, LAPF_DLCI0_CMD, lapfcb, LAPF_BIT_SET);
}

static void lapf_test_rnr (Lapfcb_t *lapfcb)
{
    lapf_send_s(LAPF_RNR, LAPF_DLCI0_CMD, lapfcb, LAPF_BIT_SET);
}

static void lapf_test_rej (Lapfcb_t *lapfcb)
{
    lapf_send_s(LAPF_REJ, LAPF_DLCI0_RSP, lapfcb, LAPF_BIT_CLEAR);
}

static void lapf_test_i (Lapfcb_t *lapfcb)
{
    fr_idb_struct_type *fr_idb;
    paktype *pak;
    ushort *ptr;
    int i, j, rtncode;
#define Testpaksize 30
#define TestL2dataCnt 5		/* send that many pak in a row */

    for (j=0; j< TestL2dataCnt; j++) {
	if (!(pak = getbuffer(Testpaksize + sizeof(Lapf_hdr_t)))) {
	    return;
	}
	lapfcb->echoI_cnt = TestL2dataCnt;
	pak->datagramstart += sizeof(Lapf_hdr_t);
	pak->datagramsize = Testpaksize;
	ptr = (ushort *)pak->datagramstart;
	for (i=0; i < Testpaksize; i++)
	    PUTSHORT(ptr++, 0x7000 + j);
	if ((fr_idb = lapfcb->hwidb->frame_relay_stuff))
	    rtncode = lapf_dataReq(fr_idb, pak);
    }
}

/*
 * LAPF internal test support
 * - I, SABME, UA, DISC, DM, FRMR, RR, RNR, REJ, Est-Req, Rel-Req, Data-Req
 */

static void (*const Lapf_eventTest[LAPF_MAX_EVENTS]) (
    Lapfcb_t *lapfcb) =
{			/* table of functions for... */
    lapf_test_i,
    lapf_test_sabme,
    lapf_test_ua,
    lapf_test_disc,
    lapf_test_dm,
    lapf_test_frmr,
    lapf_test_rr,
    lapf_test_rnr,
    lapf_test_rej
};



/*
 * lapf_cfgCmd() -- dispatching handler of configuration/test command
 */
void lapf_cfgCmd (
    parseinfo *csb)
{
    Lapfcb_t *lapfcb = NULL;
    uint keyword;
    hwidbtype *hwidb = NULL;
    fr_idb_struct_type *fr_idb = NULL;

    if (!csb || !(csb->interface) || !(hwidb = csb->interface->hwptr))
	return;
    if ((hwidb->enctype != ET_FRAME_RELAY) ||
	!(fr_idb = hwidb->frame_relay_stuff)) {
	if (!csb->nvgen) {
	    printf("\n%%%s :Frame Relay not configured on interface", 
		   hwidb->hw_namestring);
	}
	return;
    }
    if (reg_invoke_dialer_huntgroup_member(hwidb)) {
	if (!csb->nvgen) {
	    printf("\n%%Cannot change frame relay configuration of hunt group"
		   " member");
	}
	return;
    }
    if (!(lapfcb = (Lapfcb_t *)(fr_idb->lapf_cb))) {
	if (!csb->nvgen)
	    printf("\n%% No LAPF control block - enable SVC first");
	return;
    }

    if (csb->nvgen) {
	nv_write(lapfcb->lapf_t200_delay != LAPF_DEF_T200, "%s t200 %d",
		csb->nv_command, lapfcb->lapf_t200_delay);
	nv_write(lapfcb->lapf_t203_delay != LAPF_DEF_T203, "%s t203 %d",
		csb->nv_command, lapfcb->lapf_t203_delay);
	nv_write(lapfcb->lapf_n200 != LAPF_DEF_N200, "%s n200 %d",
		csb->nv_command, lapfcb->lapf_n200);
	nv_write(lapfcb->lapf_n201 != LAPF_DEF_N201, "%s n201 %d",
		csb->nv_command, lapfcb->lapf_n201);
	nv_write(lapfcb->lapf_k != LAPF_DEF_K, "%s k %d",
		csb->nv_command, lapfcb->lapf_k);
	nv_write(lapfcb->lapf_flags2.no_frmr, "no %s frmr", csb->nv_command);
	return;
    }

    /* below not nvgen - don't change the order of examining cmd keyword */

    keyword = (uint)GETOBJ(int,1);

    if (keyword < LAPF_MAX_EVENTS) {	/* layer 2 frame test */
	if (lapfcb->lapf_flags2.testL2)	/* in test mode */
	    (*Lapf_eventTest[keyword])(lapfcb);
	else
	    printf("\n%% Not in LAPF test mode");

    } else if (keyword < LAPF_CFG_CMD) {	/* parameter change */
	switch (keyword) {
	case LAPF_T200_CMD:
	    lapfcb->lapf_t200_delay = csb->sense ?
		GETOBJ(int,2) : LAPF_DEF_T200;
	    break;
	case LAPF_T203_CMD:
	    lapfcb->lapf_t203_delay = csb->sense ?
		GETOBJ(int,2) : LAPF_DEF_T203;
	    break;
	case LAPF_N200_CMD:
	    lapfcb->lapf_n200 = csb->sense ?
		GETOBJ(int,2) : LAPF_DEF_N200;
	    break;
	case LAPF_N201_CMD:
	    lapfcb->lapf_n201 = csb->sense ?
		GETOBJ(int,2) : LAPF_DEF_N201;
	    break;
	case LAPF_K_CMD:
	    lapfcb->lapf_k = csb->sense ?
		GETOBJ(int,2) : LAPF_DEF_K;
	    break;
	case LAPF_FRMR_CMD:		/* not to send FRMR at frame reject */
	    lapfcb->lapf_flags2.no_frmr = csb->sense ?
		LAPF_BIT_CLEAR : LAPF_BIT_SET;
	    break;
	}

    } else {	/* other test related command */
	switch (keyword) {
	case FR_DL_EST_REQ:	/* simulate receiving establish-req */
	    if (csb->sense) {
		if ((lapfcb->lapf_flags2.testL2)) {
		    lapfcb->lapf_flags2.svc_enabled = fr_idb->fr_svc_enabled ?
			LAPF_BIT_SET : LAPF_BIT_CLEAR;
		    lapfcb->lapf_flags2.flg_changed = LAPF_BIT_SET;
		    fr_idb->fr_svc_enabled = TRUE;
		    lapf_estReq(fr_idb);
		} else {
		    printf("\n%% Not in LAPF test mode");
		}
	    }
	    break;

	case FR_DL_REL_REQ:	/* simulate receiving relese-req */
	    if (csb->sense) {
		if ((lapfcb->lapf_flags2.testL2)) {
		    lapfcb->lapf_flags2.svc_enabled = fr_idb->fr_svc_enabled ?
			LAPF_BIT_SET : LAPF_BIT_CLEAR;
		    lapfcb->lapf_flags2.flg_changed = LAPF_BIT_SET;
		    fr_idb->fr_svc_enabled = FALSE;
		    lapf_relReq(fr_idb);
		} else {
		    printf("\n%%Not in LAPF test mode; do test command first");
		}
	    }
	    break;

	case LAPF_TEST:		/* enter LAPF-only test mode */
	    if (csb->sense) {
		lapfcb->lapf_flags2.testL2 = LAPF_BIT_SET;
	    } else {
		lapfcb->lapf_flags2.testL2 = LAPF_BIT_CLEAR;
		if (lapfcb->lapf_flags2.flg_changed) {
		    fr_idb->fr_svc_enabled = lapfcb->lapf_flags2.svc_enabled ?
			TRUE : FALSE;
		    lapfcb->lapf_flags2.flg_changed = LAPF_BIT_CLEAR;
		}
		break;
	    }
	    break;

	case LAPF_LAPD:		/* simulating LAPD per Q.921 for testing */
	    if (csb->sense) {
		lapfcb->lapf_flags2.simulate = LAPF_BIT_SET;
		lapfcb->lapf_t203_delay = T203_Q921;		/* 10 sec. */
	    } else {
		lapfcb->lapf_flags2.simulate = LAPF_BIT_CLEAR;
		lapfcb->lapf_t203_delay = LAPF_DEF_T203;	/* 30 sec. */
		if (lapfcb->lapf_flags2.flg_changed) {
		    fr_idb->fr_svc_enabled = lapfcb->lapf_flags2.svc_enabled ?
			TRUE : FALSE;
		    lapfcb->lapf_flags2.flg_changed = LAPF_BIT_CLEAR;
		}
		break;
	    }
	    break;

	case LAPF_CLRCNT:		/* clearing statistics */
	    if (csb->sense) {
		lapfcb->lapf_flags2.clrStats = LAPF_BIT_SET;
		memset(&lapfcb->lapf_stats, 0, sizeof(struct Lapf_stats)); 
	    } else
		lapfcb->lapf_flags2.clrStats = LAPF_BIT_CLEAR;
	    break;

	case LAPF_RETRY_CMD:
	    lapfcb->lapf_retry_delay = csb->sense ?
		GETOBJ(int,2) : LAPF_RETRY_DELAY;
	    break;

	}	/* don't care others */
    }
}


/*
 *  lapf_init() -- initialize LAPF support
 *  called from frame_relay_svc_startup(), the init func for svc subsystem
 */
void lapf_init (void)
{
    lapf_started = FALSE;
    lapfcb_cnt = 0;
    lapf_in_pid = lapf_outI_pid = lapf_bg_pid = 0;
    Lapf_iQ = Lapf_ackQ = Lapf_inPakQ = NULL;
    reg_add_fr_svc_lapf_l3_to_l2(lapf_l3_to_l2, "lapf_l3_to_l2");
    reg_add_Lapf_sys_event(lapf_sys_event, "lapf_sys_event");
    reg_add_Lapf_input(lapf_input, "lapf_input");
    reg_add_Lapf_showCmd(lapf_showCmd, "lapf_showCmd");
}


/*
 *  lapf_setup() -- set up LAPF support for an FR interface
 *	- invoked from frame_relay_setup() when fr_idb becomes existant
 *	- initialize the control block but not yet activated
 */
static Lapfcb_t *lapf_setup (fr_idb_struct_type *fr_idb)
{
    Lapfcb_t *lapfcb;
    hwidbtype *idb;

    if (!fr_idb || !(idb = fr_idb->hwidb) || (fr_idb->lapf_cb))	/* sanity */
	return(NULL);
    fr_idb->lapfup = FALSE;
    if (!lapf_started) {
	if (!(lapf_timer_master = malloc_named(sizeof(mgd_timer), "FR LAPF Timer")))
	    return(NULL);	/* err msg was logged, that's it for now */
	lapf_in_pid = process_create(lapf_inPak_process,
			"LAPF Input", NORMAL_STACK, PRIO_HIGH);
	lapf_outI_pid = process_create(lapf_iQ_process,
			"LAPF Output-I", NORMAL_STACK, PRIO_HIGH);
	lapf_bg_pid = process_create(lapf_monitor,
			"LAPF timer-Ack", NORMAL_STACK, PRIO_HIGH);
        if ((lapf_in_pid == NO_PROCESS) || (lapf_outI_pid == NO_PROCESS) ||
	    (lapf_bg_pid == NO_PROCESS)) {
            if (lapf_debug)
                buginf("\n*LAPF setup failed on process creation");
            free(lapf_timer_master);
            return(NULL);
        }
	mgd_timer_init_parent(lapf_timer_master, NULL);
	lapf_started = TRUE;
    }
    if (!(lapfcb = malloc_named(sizeof(Lapfcb_t), "FR LAPF Control Block")))
	return(NULL);		/* err msg was logged, that's it for now */
    memset(lapfcb, 0, sizeof(Lapfcb_t));
    lapfcb->hwidb = idb;
    lapfcb->lapf_state = TEI_ASSIGNED;
    lapfcb->lapf_t200_delay = LAPF_DEF_T200;
    lapfcb->lapf_t203_delay = LAPF_DEF_T203;
    lapfcb->lapf_retry_delay = LAPF_RETRY_DELAY;
    lapfcb->lapf_n200 = LAPF_DEF_N200;
    lapfcb->lapf_n201 = LAPF_DEF_N201;
    lapfcb->lapf_k = LAPF_DEF_K;
    lapfcb->lapf_flags2.clrStats = LAPF_DEF_CLRSTATS;
    lapfcb->lapf_flags2.simulate = LAPF_BIT_CLEAR;
    lapfcb->lapf_flags2.testL2 = LAPF_BIT_CLEAR;
    if (fr_idb->hwidb->state != IDBS_UP)
	lapfcb->lapf_cause = LAPF_ERR_IFDOWN;
    else if (!fr_idb->fr_lineup)
	lapfcb->lapf_cause = LAPF_ERR_LMI;
    else if (!fr_idb->fr_svc_enabled)
	lapfcb->lapf_cause = LAPF_ERR_DISABLED;
    else
	lapfcb->lapf_cause = LAPF_ERR_INIT;	/* not really error, to be up*/
    mgd_timer_init_parent(&lapfcb->lapf_timers, lapf_timer_master);
    mgd_timer_init_leaf(&lapfcb->lapf_t200, &lapfcb->lapf_timers,
			LAPF_T200, fr_idb, FALSE);
    mgd_timer_init_leaf(&lapfcb->lapf_t203, &lapfcb->lapf_timers,
			LAPF_T203, fr_idb, FALSE);
    mgd_timer_init_leaf(&lapfcb->lapf_retry_timer, &lapfcb->lapf_timers,
			LAPF_RETRY, fr_idb, FALSE);
    lapf_o_list_init(lapfcb);	/* for unAck queue */
    queue_init(&lapfcb->lapf_iQ, 0);	/* for I frame output queue */
    fr_idb->lapf_cb = (ulong)lapfcb;	/* link to mother */
    lapfcb_cnt++;
    if (lapf_debug)
	buginf("\n*LAPF %s CB's set up", fr_idb->hwidb->hw_namestring);
    return(lapfcb);
}
    
