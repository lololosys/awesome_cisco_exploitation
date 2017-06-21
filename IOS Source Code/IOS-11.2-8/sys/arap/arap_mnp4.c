/* $Id: arap_mnp4.c,v 3.2.58.4 1996/06/28 23:04:39 hampton Exp $
 * $Source: /release/112/cvs/Xsys/arap/arap_mnp4.c,v $
 *------------------------------------------------------------------
 * MNP routines
 *
 * January 1997, Bill Thomas & Arun Tirumalai 
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: arap_mnp4.c,v $
 * Revision 3.2.58.4  1996/06/28  23:04:39  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/06/17  22:57:20  tkolar
 * CSCdi57713:  AppleTalk:  FORCE QUIT and BAD EXIT occuring at a high rate
 * Branch: California_branch
 *
 * Revision 3.2.58.2  1996/05/18  03:40:28  tkolar
 * CSCdi57923:  Small packet leak in ARAP
 * Branch: California_branch
 * Repair MNP4 Link Request resend code.
 *
 * Revision 3.2.58.1  1996/03/18  18:50:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  08:27:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  13:31:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:38:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/18  06:45:36  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.3  1995/06/18 19:54:33  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  12:58:18  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:04:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../arap/msg_arap.c"           /* Not a typo, see logger.h */
#include "../h/config.h"
#include "parser.h"
#include "../if/network.h"
#include "arap_crc16.h"
#include "arap_mnp4.h"
#include "arap_public.h"
#include "../os/async.h"
#include "ttysrv.h"
#include "globs.h"

boolean mnp4_logging;

static inline void mnp4_mem_dump (uchar *p, int size)
{
    int i;

    buginf("\n %04x ", 0);
    for (i=0; i < size; ) {
	buginf("%02x", p[i++]);
	if (i%4 == 0)
	    buginf(" ");

	if (i%32 == 0)
	    buginf("\n %04x ", i);
    }
}

static inline mod_cntr mnp4_new_vk (mnp4type *mnp4)
{
    short new_vk;

    new_vk = QUEUESIZE(&mnp4->bufferQ) - MNP4_RCV_RESERVE;
    if (new_vk < 0) {
	new_vk = 0;
    } 
    if (new_vk > MNP4_MOD_VK) {
	new_vk = MNP4_MOD_VK;
    }
    return ((mod_cntr) new_vk);
}

static inline void mnp4_rcv_cashe_topoff (mnp4type * mnp4)
{
    int count;
    paktype *pak;

    /*
     * Place a new packet buffer on the receive cashe, to replace this one
     */
    for (count = QUEUESIZE(&(mnp4->bufferQ));
	 count < MNP4_RVC_CASHE_SIZE; count++) {
	pak = arap_getbuffer(MEDDATA, mnp4->tty->ttynum);
	if (pak == NULL)
	    return;
	p_enqueue(&(mnp4->bufferQ), pak);
    }
}

static inline void mnp4_crc_gen (paktype * pak)
{
    int count;
    ushort crcout = 0;

    for (count = 0; count < pak->datagramsize; count++) {
	crcout = MNP4_CRC16(pak->datagramstart[count], crcout);
    }
    crcout = MNP4_CRC16(MNP4_ETX, crcout);
    pak->mnp4_checksum = crcout;
}

static inline boolean mnp4_crc_check (paktype * pak, mnp4type * mnp4)
{
    int count;
    ushort crcin = 0;

    crcin = 0;
    for (count = 0; count < pak->datagramsize; count++) {
	crcin = MNP4_CRC16(pak->datagramstart[count], crcin);
    }

    crcin = MNP4_CRC16(MNP4_ETX, crcin);
    if (crcin != pak->mnp4_checksum) {
	mnp4->stats.garbled_received++;
	if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, mnp4->tty->ttynum))
	    buginf("\nMNP4 TTY %t:********Rcv Checksum failure", 
		mnp4->tty->ttynum);
	arap_datagram_done(pak, mnp4->tty->ttynum);
	return (FALSE);
    }
    return (TRUE);
}

static inline void mnp4_do_resends (mnp4type *mnp4)
{
    mnp4->resend_r = mnp4->rvr;
    mnp4->do_resend = TRUE;
    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, mnp4->tty->ttynum))
        buginf("\nMNP4 TTY %t: Queued for output RESENDS", 
		mnp4->tty->ttynum);
    mnp4_startoutput(mnp4);
}

static void mnp4_log_put (mnp4type *mnp4, boolean received, uchar *p)
{
    leveltype level;
    ulong flag;
    short i;
    long time;

    if (mnp4->log_ptr == NULL)
	return;

    if (received) {
	flag = MNP4_LOG_REC_FLAG;
    } else {
	flag = MNP4_LOG_XMT_FLAG;
    }

    level = raise_interrupt_level(NETS_DISABLE);
    GET_TIMESTAMP32(time);
    mnp4->log_ptr->item[mnp4->log_index].time = flag | 
						(time & MNP4_LOG_TIME_MSK);
    for (i=0; i < MNP4_LOG_ITEM_SIZE; i++) {
	mnp4->log_ptr->item[mnp4->log_index].data[i] = p[i];
    }
    if (++mnp4->log_index >= MNP4_LOG_SIZE)
	mnp4->log_index = 0;
    reset_interrupt_level(level);
}

static void mnp4_log_dump (mnp4type * mnp4, boolean protect)
{
    short i, index = mnp4->log_index;
    short suspend_cnt = 0;
    boolean first_time_flag = FALSE;
    long tmp , time_1st = 0;
    leveltype level = 0;		/*keep gcc happy*/

    if (protect)
	level = raise_interrupt_level(NETS_DISABLE);
    buginf("\n%t:\tBEGIN MNP4 LOG DUMP", mnp4->tty->ttynum);
    buginf("\n t401 %d t401_base %d (times modulo 1073741824 mils ie, 2**30)", 
		mnp4->t401, mnp4->t401_base);
    buginf("\n sizes MNP4InQ %d recv_ackQ %d send_ltQ %d",
		QUEUESIZE(&mnp4->MNP4InQ), QUEUESIZE(&mnp4->recv_ackQ),
		QUEUESIZE(&mnp4->send_ltQ) ); 
    buginf("\n sizes send_lr_ldQ %d bufferQ %d",
		QUEUESIZE(&mnp4->send_lr_ldQ), QUEUESIZE(&mnp4->bufferQ)); 
    buginf("\n mnp4_outpak %x do_resend %x send_ack %x free_pak_on_send %x lt_ack_cntr %d",
		mnp4->mnp4_outpak, mnp4->do_resend, mnp4->send_ack,
		mnp4->free_pak_on_send, mnp4->lt_ack_cntr);
    buginf("\n resend_r %02x rvs %02x ack_vs %02x rvr %02x",
		mnp4->resend_r, mnp4->rvs, mnp4->ack_vs, mnp4->rvr);
    buginf("\n rvk %02x vs %02x vk %02x vs_queued %02x", 
		mnp4->rvk, mnp4->vs, mnp4->vk, mnp4->vs_queued);


    buginf("\nRECV\t\t\t\tSEND");

    while (mnp4->log_ptr != NULL) {
	if (mnp4->log_ptr->item[index].time & MNP4_LOG_MSK) {
	    buginf("\n");
	    if (mnp4->log_ptr->item[index].time & MNP4_LOG_XMT_FLAG) {
		buginf("\t\t\t\t");
	    }
	    if (!first_time_flag) {
		time_1st = mnp4->log_ptr->item[index].time & MNP4_LOG_TIME_MSK; 
		first_time_flag = TRUE;
	    }

	    switch (mnp4->log_ptr->item[index].data[1]) {
	      case MNP4_LT:
		buginf(" LT   Ns[%02x]", mnp4->log_ptr->item[index].data[2]);
		break;
	      case MNP4_LT_H:
		buginf(" LT H Ns[%02x]", mnp4->log_ptr->item[index].data[2]);
		break;
	      case MNP4_LA:
		buginf(" LA          Nr[%02x] Nk[%02x]",
		       mnp4->log_ptr->item[index].data[2],
		       mnp4->log_ptr->item[index].data[3]);
		break;
	      case MNP4_LD:
		buginf(" LD ");
		break;
	      case MNP4_LR:
		buginf(" LR ");
		break;
	      default:
		buginf(" -- ");
		for (i = 0; i < MNP4_LOG_ITEM_SIZE; i++) {
		    buginf(" %02x");
		}
		break;
	    }

	    tmp = mnp4->log_ptr->item[index].time & MNP4_LOG_TIME_MSK;
	    buginf(" t %d ", (tmp - time_1st) & MNP4_LOG_TIME_MSK);
	}
	if (++index >= MNP4_LOG_SIZE)
	    index = 0;
	if (index == mnp4->log_index)
	    break;
	if (suspend_cnt++ > 99 && !protect) {
	    suspend_cnt = 0;
	    process_suspend();		/* don't be a hog */
	}
    }
    buginf("\nEND MNP4 LOG DUMP");
    if (protect)
	reset_interrupt_level(level);
}

static void mnp4_crashdump (mnp4type *mnp4)
{
    mnp4_log_dump(mnp4, TRUE);
    crashdump(200);
}

static void mnp4_default_crashdump (mnp4type *mnp4)
{
    errmsg(&msgsym(DEFAULT, ARAP), mnp4->tty->ttynum);
    mnp4_crashdump(mnp4);
}

/*
 * see comments in mnp4_packets_pending()
 */

static paktype *prime_mnp4_outpak (mnp4type * mnp4)
{
    paktype *pak = NULL;
    uchar *dataptr;
    leveltype level;
    mod_cntr vs;

    level = raise_interrupt_level(NETS_DISABLE);
    if (mnp4->mnp4_outpak == NULL) {
	/* priority is LR's & LD's, LA's, resends, and lastly LT's */
	if ((pak = dequeue(&mnp4->send_lr_ldQ))) {
	    /* sending LR or LD */
	    ;			/* a null statement */
	} else if (mnp4->send_ack) {
	    mnp4->send_ack = FALSE;

	    pak = mnp4->pak_ack;
	    /* pak_ack was inited by mnp4_setup() */

	    pak->datagramstart[MNP4_LA_RECVCRED] = mnp4->vk;
	    switch (mnp4->link_state) {
	      case MNP4_CONNECTING:
		pak->datagramstart[MNP4_LA_RECVSEQ] = 0;
		break;
	      case MNP4_DATA_PHASE:
		mnp4->ack_vs = MNP4_MOD_SUB(mnp4->rvs, 1);
		pak->datagramstart[MNP4_LA_RECVSEQ] = mnp4->ack_vs;
		break;
	      case MNP4_DISCONNECTING:
		break;
	      default:
		mnp4_default_crashdump(mnp4);
	    }
	    /*could be at interrupt level but LA's are small*/
	    mnp4_crc_gen(pak);
	} else if (mnp4->do_resend && mnp4->link_state == MNP4_DATA_PHASE) {
	    /* nothing needs to be resent */
	    if (mnp4->vs == mnp4->resend_r) {
		mnp4->do_resend = FALSE;
	    } else if (mnp4->rvk && (MNP4_MOD_SUB(mnp4->resend_r, mnp4->rvr) < 
			mnp4->rvk)) {
		/*window is open. do resends*/
		pak = mnp4->resend[mnp4->resend_r & MNP4_MOD_RESEND].pak;
		mnp4->resend[mnp4->resend_r & MNP4_MOD_RESEND].resending = TRUE;
		if (pak == NULL) {
		    errmsg(&msgsym(NULLRESEND, ARAP), mnp4->tty->ttynum); 
		    mnp4_crashdump(mnp4);
		}
		mnp4->stats.packets_resent++;
		mnp4->resend_r = MNP4_MOD_ADD(mnp4->resend_r, 1);
		/* will do another TIMER_START() after mnp4_sndchar()
		 * finishes sending this frame.  that way the resend counter
		 * can detect a hung send condition.
		 */
		TIMER_START(mnp4->timer_resend, mnp4->t401);
		/* no more to resend */
		if (mnp4->vs == mnp4->resend_r) {
		    mnp4->do_resend = FALSE;
		}
	    }
	/* if MNP4_MOD_VK ever were 256, then a term:
	 *   "(MNP4_MOD_ADD(mnp4->vs, 1) != mnp4->rvr)" 
	 * would be required in the following "if"
	 */
	} else if (mnp4->rvk && (MNP4_MOD_SUB(mnp4->vs, mnp4->rvr) < 
		   mnp4->rvk) && mnp4->link_state == MNP4_DATA_PHASE) {
	    /* window is open */
	    if ((pak = dequeue(&mnp4->send_ltQ))) {
		/*
		 * every thing else setup by mnp4_output() only LR's are
		 * numbered and can be resent
		 */
		vs = pak->datagramstart[MNP4_LT_SENDSEQ];
		if (vs != mnp4->vs) {
		    errmsg(&msgsym(VSERROR, ARAP), mnp4->tty->ttynum);
		    mnp4_crashdump(mnp4);
		}
		mnp4->vs = MNP4_MOD_ADD(vs, 1);
		if (mnp4->resend[vs & MNP4_MOD_RESEND].pak != NULL) {
		    errmsg(&msgsym(NOTNULLRESEND, ARAP), mnp4->tty->ttynum);
		    mnp4_crashdump(mnp4);
		}
		mnp4->resend[vs & MNP4_MOD_RESEND].pak = pak;
		mnp4->resend[vs & MNP4_MOD_RESEND].resending = FALSE;
		mnp4->resend_cntr = 0;
		/* will do another TIMER_START() after mnp4_sndchar()
		 * finishes sending this frame.  that way the resend counter
		 * can detect a hung send condition.
		 */
		TIMER_START(mnp4->timer_resend, mnp4->t401);
	    }
	}
	if (pak) {
	    /* this logic is needed by mnp4_sndchar().  
	     * see mnp4_packets_pending()
	     */
	    mnp4->mnp4_outpak = pak;
	    mnp4->free_pak_on_send = FALSE;
	    pak->dataptr = pak->datagramstart;
	    pak->length = pak->datagramsize;
	    mnp4->ostate = MNP4_START_STAGE0;
	    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
		    mnp4->tty->ttynum)){
		dataptr = pak->datagramstart;
		buginf("\nMNP4 TTY %t:\t\tStart sending %02x %02x %02x %02x",
			mnp4->tty->ttynum, dataptr[0], dataptr[1], dataptr[2],
			dataptr[3]);
	    }
	}
    }
    reset_interrupt_level(level);
    return (pak);
}

void mnp4_startoutput (mnp4type * mnp4)
{
    if (prime_mnp4_outpak(mnp4)) {
	startoutput(mnp4->tty);
    }
}

void mnp4_send_ack (mnp4type * mnp4)
{

    mnp4->vk = mnp4_new_vk(mnp4);
    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, mnp4->tty->ttynum)) {
	buginf("\nMNP4 TTY %t: Queued for output LA", mnp4->tty->ttynum);
        if (TIMER_RUNNING(mnp4->timer_ack_delay)) {
	    if (AWAKE(mnp4->timer_ack_delay)) {
		buginf("\nMNP4 TTY %t: Ack delay timer running and awake", 
			mnp4->tty->ttynum);
	    } else {
		buginf("\nMNP4 TTY %t: Ack delay timer running and sleeping", 
			mnp4->tty->ttynum);
	    }
	}
    }
    TIMER_STOP(mnp4->timer_ack_delay);
    mnp4->send_ack = TRUE;
    mnp4_startoutput(mnp4);
}

static void mnp4_process_ack (paktype * pak, mnp4type * mnp4)
{
    mod_cntr sa, r_1, nr_1, nr, rvk;
    paktype *pak_acked;
    uchar *dataptr;
    long ack_delay, temp, head_room;
    leveltype level;

    dataptr = pak->datagramstart;
    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, mnp4->tty->ttynum)) {
	buginf("\nMNP4 TTY %t:Rcv LA  Nr[%02x] Nk[%02x]",
	       mnp4->tty->ttynum, dataptr[MNP4_LA_RECVSEQ],
	       dataptr[MNP4_LA_RECVCRED]);
    }
    level = raise_interrupt_level(NETS_DISABLE);

    rvk = pak->datagramstart[MNP4_LA_RECVCRED];
    /*
     * clamp rvk to MNP4_MOD_VK so as to limit the size of our resend[] which
     * allows us some nice sanity checks
     */
    if (rvk > MNP4_MOD_VK)
	rvk = MNP4_MOD_VK;
    mnp4->rvk = rvk;

    switch (mnp4->link_state) {
      case MNP4_CONNECTING:
	break;

      case MNP4_DATA_PHASE:
	/* range of sent frames unacked */
	sa = MNP4_MOD_SUB(mnp4->vs, mnp4->rvr);
	if (sa == 0)
	    break;

	nr = (mod_cntr) pak->datagramstart[MNP4_LA_RECVSEQ];
	nr_1 = MNP4_MOD_ADD(nr, 1);
	r_1 = mnp4->rvr;

	/* Nr is not within the range of sent frame which are unacked? */
	if (nr_1 == r_1 || MNP4_MOD_SUB(nr_1, r_1) > sa) {
	    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
		    mnp4->tty->ttynum)){
		buginf("\nMNP4 TTY %t:\t\t\tRcv LA Nr %x out of range",
		       mnp4->tty->ttynum, nr);
	    }
	    break;
	}
	/* free pak's from resend array up to and including this acked pak */
	while (nr_1 != r_1) {
	    if ((pak_acked = mnp4->resend[r_1 & MNP4_MOD_RESEND].pak)) {

		/*
		 * can't free a pak which is being sent. so we signal
		 * mnp4_sndchar() to free it after its sent
		 */
		if (pak_acked == mnp4->mnp4_outpak) {
		    mnp4->free_pak_on_send = TRUE;
		} else {
		    arap_datagram_done(pak_acked, mnp4->tty->ttynum);
		}
		mnp4->resend[r_1 & MNP4_MOD_RESEND].pak = NULL;

		/*compute ack delay as long as t401 has not topped out*/
		if (mnp4->t401 < mnp4->t401_max) {
		    /* get mac's ack delay */
		    ack_delay = (long)
			ELAPSED_TIME(mnp4->resend[r_1 & MNP4_MOD_RESEND].time);

		    /*
		     * compute average delay in mil seconds  avrg_delay =
		     * .8125*(avrg_delay - ack_delay) + ack_delay; term =
		     * (avrg_delay - ack_delay) .8125 = 1/2 + 1/4 + 1/16 p =
		     * term/2 + term/4 + term/16 avrg_delay = p + ack_delay
		     */
		    mnp4->avrg_delay = MNP4_AVRG_FACT1(mnp4->avrg_delay - 
							ack_delay);
		    temp = MNP4_AVRG_FACT1(mnp4->avrg_delay);
		    mnp4->avrg_delay += temp + MNP4_AVRG_FACT2(temp) + 
								ack_delay;

		    /* don't use average till valid */
		    if (mnp4->avrg_delay_cntr < MNP4_AVRG_VALID) {
			mnp4->avrg_delay_cntr++;
		    } else {
			head_room = mnp4->avrg_delay + (mnp4->avrg_delay >> 1);
			if (head_room > mnp4->t401_max) {
			    head_room = mnp4->t401_max;
			}
			if (head_room > mnp4->t401) {
			    mnp4->t401 = head_room;
			    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
				    mnp4->tty->ttynum)) {
				buginf("\nMNP4 TTY %t: increased t401 to %d", 
					mnp4->tty->ttynum, mnp4->t401);
			    }
			}
		    }
		}
	    } else {
		errmsg(&msgsym(NULLRESEND, ARAP), mnp4->tty->ttynum);
		mnp4_crashdump(mnp4);
	    }
	    r_1 = MNP4_MOD_ADD(r_1, 1);
	}
	mnp4->rvr = nr_1;

	/* all sent frames acked */
	if (r_1 == mnp4->vs) {
	    mnp4->do_resend = FALSE;
	    TIMER_STOP(mnp4->timer_resend);
	    mnp4->resend_cntr = 0;
	} else {
	    mnp4->resend_r = nr_1;	/* in case were are doing resends */
	}
	/* try and start output - we may have been flow controlled */
	mnp4_startoutput(mnp4);
	break;

      case MNP4_DISCONNECTING:
	break;
      default:
	mnp4_default_crashdump(mnp4);
    }
    reset_interrupt_level(level);
    arap_datagram_done(pak, mnp4->tty->ttynum);
}

/*
 * mnp4_output  --  dispatch the MNP outgoing queue
 * 
 * Takes packets off of MNP4OutQ, tag on the header and footer information, and
 * send them off to the line queue procedure.
 */

void mnp4_output (mnp4type * mnp4, queuetype * MNP4OutQptr)
{
    paktype *pak;

    if (mnp4 == NULL)
	return;

    while ((pak = p_dequeue(MNP4OutQptr))) {

	pak->datagramsize += 3;
	pak->datagramstart -= 3;

	pak->datagramstart[MNP4_LENGTH] = 2;
	if ((mnp4->tty->arap_version == ARAP_VERSION2) &&
	    (mnp4->tty->acflags & ACF_CALLBACK_ARAP))
	    pak->datagramstart[MNP4_TYPE] = MNP4_LT_H;
	else
	    pak->datagramstart[MNP4_TYPE] = MNP4_LT;

	pak->datagramstart[MNP4_LT_SENDSEQ] = mnp4->vs_queued;
	mnp4->vs_queued = MNP4_MOD_ADD(mnp4->vs_queued, 1);
	pak->length = pak->datagramsize;
 	if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, mnp4->tty->ttynum))	
	    buginf("\nMNP4 TTY %t: Queued for output LT (%d) Ns[%x]",
		   mnp4->tty->ttynum, pak->datagramsize, 
		   pak->datagramstart[MNP4_LT_SENDSEQ]);
	/* don't do the crc at interrupt level*/
	mnp4_crc_gen(pak);
	p_enqueue(&mnp4->send_ltQ, pak);
	mnp4_startoutput(mnp4);
    }
}

boolean mnp4_overflowed (mnp4type * mnp4)
{
    if (QUEUESIZE(&mnp4->send_ltQ) > MNP4_MAXSENTQSIZE) {
	return (TRUE);
    }
    return (FALSE);
}

/*
 * mnp4_internal_output --  output line enqueuer procedure
 * This just puts the packet on the interrupt level queue.
 */
void mnp4_internal_output (mnp4type * mnp4, paktype * pak)
{
    int count;

    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, mnp4->tty->ttynum)) {
	buginf("\nMNP4 TTY %t: Queued for output LR or LD (%d): ",
		mnp4->tty->ttynum, pak->datagramsize);
	for (count = 0; count < 5; count++) {
	    buginf(" %x", pak->datagramstart[count]);
	}
    }
    mnp4_crc_gen(pak);
    p_enqueue(&mnp4->send_lr_ldQ, pak);
    mnp4_startoutput(mnp4);
}

void send_link_disconnect (mnp4type * mnp4)
{
    paktype *pak;

    if (mnp4 == NULL)
	return;

    pak = arap_getbuffer(MEDDATA, mnp4->tty->ttynum);
    if (pak == NULL) {
	return;
    }
    pak->datagramstart = ((uchar *) pak->network_start);
    mnp4->link_state = MNP4_DISCONNECTING;

    /*
     * If callback, disconnect is special
     */
    if (mnp4->tty->acflags & ACF_CALLBACK_ARAP) {
	pak->datagramstart[MNP4_LENGTH] = 7;
	pak->datagramstart[MNP4_TYPE] = MNP4_LD;
	pak->datagramstart[MNP4_LD_TYPE] = 1;
	pak->datagramstart[MNP4_LD_LEN] = 1;
	pak->datagramstart[MNP4_LD_REASON] = MNP4_LD_USER;

	pak->datagramstart[5] = 2;
	pak->datagramstart[6] = 1;
	pak->datagramstart[7] = 0;

	pak->datagramsize = MNP4_LD_PAKLEN + 3;
	pak->length = MNP4_LD_PAKLEN + 3;

	mnp4_internal_output(mnp4, pak);
	return;
    }
    pak->datagramstart[MNP4_LENGTH] = 4;
    pak->datagramstart[MNP4_TYPE] = MNP4_LD;
    pak->datagramstart[MNP4_LD_TYPE] = 1;
    pak->datagramstart[MNP4_LD_LEN] = 1;
    pak->datagramstart[MNP4_LD_REASON] = MNP4_LD_USER;


    pak->datagramsize = MNP4_LD_PAKLEN;
    pak->length = MNP4_LD_PAKLEN;

    mnp4_internal_output(mnp4, pak);

}

/*
 * send_link_request_response  --   pretty much what it sounds like
 */
void send_link_request_response (mnp4type * mnp4)
{
    paktype *pak;
    int count;
    uchar temp[40] =
    {29, 1, 2, 1, 6, 1, 0, 0, 0, 0, 255, 2, 1, 2, 3, 1, 8, 4, 2,
    64, 0, 8, 1, 3, 14, 4, 3, 0, 8, 250};
    /*
     * the trailing 3, 0, 8, 250 on this packet represent the v42bis info.
     * "3" means v42bis both ways, "0,8" is actually a byteswapped 2048
     * dictionary size, and "250" is the max string size
     */

    /* we'll probably need to change this for telebit */

    if (mnp4 == NULL)
	return;

    pak = arap_getbuffer(MEDDATA, mnp4->tty->ttynum);
    if (pak == NULL) {
	return;
    }
    pak->datagramsize = MNP4_LR_PAKLEN;
    pak->length = MNP4_LR_PAKLEN;
    pak->datagramstart = ((uchar *) pak->network_start);


    for (count = 0; count < MNP4_LR_PAKLEN; count++) {
	pak->datagramstart[count] = temp[count];
    }

    mnp4_internal_output(mnp4, pak);
}

/*
 * mnp4_input  --  dispatch the MNP4 input queue
 */
boolean mnp4_input (mnp4type * mnp4, queuetype * v42InQptr)
{
    paktype *pak;
    uchar *dataptr;
    int count;
    int status;
    mod_cntr va, ns;
    leveltype level;

    if (mnp4 == NULL)
	return (FALSE);

    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, mnp4->tty->ttynum)) 
	buginf("\nMNP4 TTY %t:mnp4_input()", mnp4->tty->ttynum);

    while ((pak = p_dequeue(&mnp4->recv_ackQ)) ||
	   (pak = p_dequeue(&mnp4->MNP4InQ))) {

	mnp4->stats.packets_received++;
	mnp4_rcv_cashe_topoff(mnp4);
	if (!mnp4_crc_check(pak, mnp4)) {
	    if (mnp4->link_state == MNP4_DATA_PHASE) {
		mnp4_send_ack(mnp4);
	    }
	    continue;
	}

	dataptr = pak->datagramstart;

	mnp4_log_put(mnp4, TRUE, dataptr);

	/* ignore the high bit of the type for ARAP 2.0 compatibility */

	switch (dataptr[MNP4_TYPE]) {
	  case MNP4_LR:	/* handle a link request */
	    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
		    mnp4->tty->ttynum)) 
		buginf("\nMNP4 TTY %t:Rcv LR", mnp4->tty->ttynum);

	    arap_datagram_done(pak, mnp4->tty->ttynum);

	    /*
	     * Send a MNP4 LR packet, and expect an MNP4 LA. On a timeout,
	     * resend up to MNP4_RESPONDER_RETRIES
	     */
	    count = 0;
	    while (TRUE) {
		send_link_request_response(mnp4);
		status = wait_for_mnp4_remote(mnp4, MNP4_LA);
		if (status == OK) {
		    break;
		} else if (status == ABORTED) {
		    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
			    mnp4->tty->ttynum))
			buginf("\nMNP4 TTY %t:********Wait rcv LA ABORTED",
				mnp4->tty->ttynum);
		    return (FALSE);
		}
		if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
			mnp4->tty->ttynum))
		    buginf("\nMNP4 TTY %t:********Wait LA timeout resending",
				mnp4->tty->ttynum);
		if (++count >= MNP4_RESPONDER_RETRIES) {
		    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
			    mnp4->tty->ttynum)) 
			buginf("\nMNP4 TTY %t:********Wait LR max resend - end",
				mnp4->tty->ttynum);
		    return (FALSE);
		}
	    }
	    break;
	  case MNP4_LD:	/* disconnect the link */
	    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
		    mnp4->tty->ttynum))
		buginf("\nMNP4 TTY %t:Rcv LD", mnp4->tty->ttynum);
	    arap_datagram_done(pak, mnp4->tty->ttynum);
	    mnp4->link_state = MNP4_DISCONNECTING;
	    return (FALSE);
	    break;
	  case MNP4_LT:		/* handle a link transmission */
	  case MNP4_LT_H:	/* handle a link transmission */
	    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
		    mnp4->tty->ttynum)){
		buginf("\nMNP4 TTY %t:Rcv LT (%d) Ns[%x]: ", mnp4->tty->ttynum,
			pak->datagramsize, pak->datagramstart[MNP4_LT_SENDSEQ]);

		for (count = 0; count < 5; count++) {
		    buginf(" %x", pak->datagramstart[count]);
		}
	    }
	    if (mnp4->link_state != MNP4_DATA_PHASE) {
		if (mnp4->link_state == MNP4_CONNECTING) {
		    errmsg(&msgsym(LRPHASERROR, ARAP), mnp4->tty->ttynum);
		}
		arap_datagram_done(pak, mnp4->tty->ttynum);
		return (TRUE);
	    }
	    /*
	     * don't allow out of order frames 
	     *
	     */
	    ns = pak->datagramstart[MNP4_LT_SENDSEQ];

	    level = raise_interrupt_level(NETS_DISABLE);

	    if (ns == mnp4->rvs) {
		mnp4->lt_ack_cntr = 0;
		mnp4->rvs = MNP4_MOD_ADD(mnp4->rvs, 1);

		/*
		 * this is what we want.  Send it up to v42bis
		 */
		pak->datagramstart += 3;
		pak->datagramsize -= 3;
		enqueue(v42InQptr, pak);

		va = mnp4->vk / 2;

		/* if va == 0 we still send an ack*/
		if (MNP4_MOD_SUB(mnp4->rvs, mnp4->ack_vs) > va) {
		    /* half the window or more is unacked by us, send an ack */
		    mnp4_send_ack(mnp4);
		} else {
		    if (!TIMER_RUNNING(mnp4->timer_ack_delay)) {
			TIMER_START(mnp4->timer_ack_delay, mnp4->t402);
			if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
				mnp4->tty->ttynum)) {
			    buginf("\nMNP4 TTY %t:Ack delay timer started ", 
					mnp4->tty->ttynum); 
			}
		    } else {
			if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
				mnp4->tty->ttynum))		
			    buginf("\nMNP4 TTY %t:Ack delay timer running", 
					mnp4->tty->ttynum);
		    }
		}
	    } else {
		/* not the previous LT - send an ack */
		if (ns != MNP4_MOD_SUB(mnp4->rvs, 1)) {
		    mnp4->stats.out_of_order++;
		    mnp4->lt_ack_cntr++;
		    mnp4_send_ack(mnp4);
		} else if (mnp4->lt_ack_cntr++ > 0) { 
		    /* don't ack on 1st previous LT */
		    mnp4_send_ack(mnp4);
		}
		arap_datagram_done(pak, mnp4->tty->ttynum);
	    }
	    reset_interrupt_level(level);
	    break;
	  case MNP4_LA:	/* handle a link ack */
	    mnp4_process_ack(pak, mnp4);
	    break;
	  case MNP4_LN:
	    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, 
		    mnp4->tty->ttynum))
		buginf("\nMNP4 TTY %t:Rcv LN/discarded", mnp4->tty->ttynum);
	    arap_datagram_done(pak, mnp4->tty->ttynum);
	    break;
	  case MNP4_LNA:
	    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
		    mnp4->tty->ttynum))
		buginf("\nMNP4 TTY %t:Rcv LNA/discarded", mnp4->tty->ttynum);
	    arap_datagram_done(pak, mnp4->tty->ttynum);
	    break;
	  default:
	    mnp4_send_ack(mnp4);
	    arap_datagram_done(pak, mnp4->tty->ttynum);
	    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
		    mnp4->tty->ttynum))
		buginf("\nMNP4 TTY %t:Rcv Unknown MNP4 packet/discarded",
			mnp4->tty->ttynum);
	    break;
	}
    }
    return (TRUE);
}

/*
 * mnp4_setup --   initialize MNP4
 */
mnp4type *mnp4_setup (tt_soc * tty)
{
    mnp4type *mnp4;
    paktype *pak;

    mnp4 = malloc(sizeof(mnp4type));

    if (mnp4 == NULL)
	return (NULL);

    mnp4->log_ptr = NULL;
    mnp4->log_index = 0;
    mnp4->log_ptr = malloc(sizeof(mnp4_log));
    if (mnp4->log_ptr == NULL) {
	errmsg(&msgsym(NOLOG, ARAP), tty->ttynum);
    }

    /* get the one and only fixed pak_ack - don't free this pak */
    pak = arap_getbuffer(MEDDATA, tty->ttynum);
    if (pak == NULL) {
	if (mnp4->log_ptr)
	    free(mnp4->log_ptr);
	free(mnp4);
	return (NULL);
    }

    /* fill in the fields just once per session - except for ack_vs & vk */
    pak->datagramstart = pak->network_start;

    pak->datagramstart[MNP4_LENGTH] = 3;
    pak->datagramstart[MNP4_TYPE] = MNP4_LA;
    pak->datagramstart[MNP4_LA_RECVSEQ] = 0;	/* = >ack_vs by
						 * mnp4_sndchar() */
    pak->datagramstart[MNP4_LA_RECVCRED] = 0;	/* = ->vk by mnp4_sndchar() */

    pak->datagramsize = MNP4_LA_PAKLEN;
    pak->length = MNP4_LA_PAKLEN;
    mnp4->pak_ack = pak;

    /* initialize queues */
    queue_init(&mnp4->MNP4InQ, 0);
    queue_init(&mnp4->recv_ackQ, 0);
    queue_init(&mnp4->send_ltQ, 0);
    queue_init(&mnp4->send_lr_ldQ, 0);

    /* initialize tty MNP4 state machine */
    mnp4->istate = MNP4_RXIDLE;
    mnp4->ostate = MNP4_TXIDLE;

    /* initial MNP4 packet pointers */
    mnp4->mnp4_outpak = NULL;
    mnp4->mnp4_inpak = NULL;

    /* initialize MNP4 struct states */
    mnp4->tty = tty;

    mnp4->rvs = 1;
    mnp4->vs = 1;
    mnp4->vs_queued = 1;
    mnp4->rvk = MNP4_MOD_VK;  /* assume for startup */

    queue_init(&mnp4->bufferQ, 0);
    mnp4_rcv_cashe_topoff(mnp4);
    mnp4->vk = mnp4_new_vk(mnp4);
    if (mnp4->vk < MNP4_MOD_VK/2) {
	arap_datagram_done(mnp4->pak_ack, tty->ttynum);
	while ((pak = p_dequeue(&mnp4->bufferQ))) {
	    arap_datagram_done(pak, tty->ttynum);
	}
	if (mnp4->log_ptr)
	    free(mnp4->log_ptr);
	free(mnp4);
	return (NULL);
    }

    mnp4->send_ack = FALSE;
    mnp4->do_resend = FALSE;
    mnp4->free_pak_on_send = FALSE;
    mnp4->link_state = MNP4_CONNECTING;
    mnp4->lt_ack_cntr = 0;

    mnp4->resend_cntr = 0;
    mnp4->resend_r = 0;
    mnp4->rvr = 1;
    mnp4->ack_vs = 0;

    TIMER_STOP(mnp4->timer_resend);
    TIMER_STOP(mnp4->timer_ack_delay);
    TIMER_STOP(mnp4->wait_timer);
    TIMER_STOP(mnp4->timer_disconnect);

    mnp4->t401_base = MNP4_DEFAULT_RESEND_TIMEOUT;
    mnp4->t401_max = MNP4_DEFAULT_RESEND_TIMEOUT * MNP4_MAX_BAUD_FACT;
    mnp4->t401 = MNP4_DEFAULT_RESEND_TIMEOUT;
    mnp4->t402 = MNP4_DEFAULT_ACK_DELAY;
    mnp4->avrg_delay = 0;
    mnp4->avrg_delay_cntr = 0;

    mnp4->stats.packets_received = 0;
    mnp4->stats.packets_sent = 0;
    mnp4->stats.garbled_received = 0;
    mnp4->stats.out_of_order = 0;
    mnp4->stats.packets_resent = 0;
    mnp4->stats.nobuffers = 0;

    /* if an arap_version has already been selected, set up mnp4 correctly */
    /*
     * NOTE: this depends on autoselect using the first three characters to
     * select
     */
    if (tty->arap_version == ARAP_VERSION1) {
	mnp4->mnp4_dle = ARAP1_MNP4_DLE;
	mnp4->mnp4_syn = ARAP1_MNP4_SYN;
	mnp4->istate = MNP4_RECEIVE;
    } else if (tty->arap_version == ARAP_VERSION2) {
	mnp4->mnp4_dle = ARAP2_MNP4_DLE;
	mnp4->mnp4_syn = ARAP2_MNP4_SYN;
	mnp4->istate = MNP4_RECEIVE;
    }
    return (mnp4);
}

/*
 * disableBLOCK 
 */
boolean disableBLOCK (mnp4type * mnp4)
{
    if (modemcheck(mnp4->tty))
	return (FALSE);

    if (!mnp4_packets_pending(mnp4))
	return (FALSE);

    if (TIMER_RUNNING_AND_AWAKE(mnp4->timer_disconnect))
	return (FALSE);

    return (TRUE);
}

/*
 * mnp4_disable -- Remove MNP4 interrupt handler
 */
void mnp4_disable (tt_soc * tty)
{
    mnp4type *mnp4;
    paktype *pak;
    int i;

    mnp4 = tty->mnp4ptr;

    /* If NULL, unhook yourself and bail out.. */
    if (mnp4 == NULL) {
	tty_noservicehook(tty);
	return;
    }
    send_link_disconnect(mnp4);
    TIMER_START(mnp4->timer_disconnect, mnp4->t401_base * 
			MNP4_LR_LD_WAIT_FACTOR);
    edisms((blockproc *) disableBLOCK, (int) mnp4);

    /* LD was sent, we timed out or the tty circuit's gone.  
     * there is nothing else we can do
     * rip out the hooks and then free everything
     */

    tty_noservicehook(tty);

    if (mnp4_logging) {
	mnp4_log_dump(mnp4, FALSE);
    }

    /* Clean queues */
    while ((pak = p_dequeue(&mnp4->send_lr_ldQ))) {
	arap_datagram_done(pak, tty->ttynum);
    }
    while ((pak = p_dequeue(&mnp4->send_ltQ))) {
	arap_datagram_done(pak, tty->ttynum);
    }
    while ((pak = p_dequeue(&mnp4->MNP4InQ))) {
	arap_datagram_done(pak, tty->ttynum);
    }
    while ((pak = p_dequeue(&mnp4->recv_ackQ))) {
	arap_datagram_done(pak, tty->ttynum);
    }

    if (mnp4->mnp4_inpak) {
	arap_datagram_done(mnp4->mnp4_inpak, tty->ttynum);
    }

    /* clean packets in resend array */
    for ( i=0; i<MNP4_RESEND_SIZE; i++) {
	if (mnp4->resend[i].pak != NULL) {
	    arap_datagram_done(mnp4->resend[i].pak, tty->ttynum);
	}
    }

    if (mnp4->mnp4_outpak && mnp4->mnp4_outpak->refcount) {
	arap_datagram_done(mnp4->mnp4_outpak, tty->ttynum);
    }

    mnp4_destroy_setup(mnp4);
}

/*
 * mnp4_gotchar -- get characters from serial line and put them in MNP4
 * packet
 * 
 * This is the MNP4 input state machine.
 */
void mnp4_gotchar (tt_soc * tty, uchar * p, int len)
{
    paktype *pak;
    mnp4type *mnp4;
    uchar c;
    uchar *p_int = p;
    int len_int = len;
    boolean getbuffer_failed = FALSE;	/* no new pak's till we return
					 * don't waste the cycles on more
					 * getbuffer()'s
					 */
    mnp4 = tty->mnp4ptr;
    if (mnp4 == NULL)
	return;

    pak = mnp4->mnp4_inpak;

    while (len-- > 0) {
	if (pak == NULL && !getbuffer_failed) {
	    /*
	     * Try to get a packet, first from our private queue, then from
	     * the global pool.
	     */
	    pak = dequeue(&mnp4->bufferQ);
	    if (pak == NULL) {
		pak = arap_getbuffer(MEDDATA, tty->ttynum);
	    }
	    /*
	     * if no pak then use shadow states to keep our sync with the
	     * mnp4 frames - we don't store anything.  so till we get a
	     * pak we drop frames!
	     */
	    if (pak == NULL) {
		getbuffer_failed = TRUE;
		errmsg(&msgsym(RCVNOPAK, ARAP), tty->ttynum);
		mnp4->stats.nobuffers++;
	    }
	    /*
	     * Update MNP4 record of current packet
	     */
	    mnp4->mnp4_inpak = pak;
	}
	/*
	 * Get next character, process according to current MNP4 state
	 */
	c = *p++;
	switch (mnp4->istate) {

	    /*
	     * these first three cases check for the starting sequence (SYN,
	     * DLE, STX) of an MNP4 packet.  If at any time before or during
	     * the startup we get an unexpected character, we go back to the
	     * idle state
	     */
	  case MNP4_RXIDLE:
	    if (tty->arap_version == ARAP_NOVERSION) {
		switch (c) {
		  case ARAP1_MNP4_SYN:
		    mnp4->mnp4_syn = ARAP1_MNP4_SYN;
		    mnp4->istate = MNP4_START_STAGE1;
		    break;
		  case ARAP2_MNP4_SYN:
		    mnp4->mnp4_syn = ARAP2_MNP4_SYN;
		    mnp4->istate = MNP4_START_STAGE1;
		    break;
		  default:
		    break;
		}
	    } else {
		if (c == mnp4->mnp4_syn) {
		    mnp4->istate = MNP4_START_STAGE1;
		}
	    }
	    break;
	  case MNP4_START_STAGE1:
	    if (tty->arap_version == ARAP_NOVERSION) {
		/*try not to be fooled. check both syn & dle for the 
		 *right version
		 */
		switch (c) {
		  case ARAP1_MNP4_DLE:
		    if (mnp4->mnp4_syn == ARAP1_MNP4_SYN) {
			mnp4->mnp4_dle = ARAP1_MNP4_DLE;
			tty->arap_version = ARAP_VERSION1;
			mnp4->istate = MNP4_START_STAGE2;
			if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
				tty->ttynum)) {
			    buginf("\nMNP4 TTY %t:        Starting ARAP 1.0",
					tty->ttynum);
			}	    
		    } else {
			mnp4->istate = MNP4_RXIDLE;
		    }
		    break;
		  case ARAP2_MNP4_DLE:
		    if (mnp4->mnp4_syn == ARAP2_MNP4_SYN) {
			mnp4->mnp4_dle = ARAP2_MNP4_DLE;
			tty->arap_version = ARAP_VERSION2;
			mnp4->istate = MNP4_START_STAGE2;
			if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
				tty->ttynum)) {
			    buginf("\nMNP4 TTY %t:        Starting ARAP 2.0",
					tty->ttynum);
			}	    
		    } else {
			mnp4->istate = MNP4_RXIDLE;
		    }
		    break;
		  default:
		    mnp4->istate = MNP4_RXIDLE;
		    break;
		}
	    } else {
		if (c == mnp4->mnp4_dle) {
		    mnp4->istate = MNP4_START_STAGE2;
		} else {
		    mnp4->istate = MNP4_RXIDLE;
		}
	    }
	    break;
	  case MNP4_START_STAGE2:
	    if (c == MNP4_STX) {
		if (pak == NULL) {
		    /* don't loose frame sync */
		    mnp4->istate = MNP4_RECEIVE_SHADOW;
		} else {
		    mnp4->istate = MNP4_RECEIVE;
		}
	    } else {
		mnp4->istate = MNP4_RXIDLE;
	    }
	    break;

	  case MNP4_RECEIVE:
	    if (c == mnp4->mnp4_dle) {
		mnp4->istate = MNP4_DLE_RECEIVED;
	    } else {
		*pak->dataptr++ = c;
		if (++pak->datagramsize > MNP4_MAXPACKETSIZE) {
		    /*
		     * Oversized packet received. Clear it and go back to
		     * idle.
		     */
		    mnp4->istate = MNP4_RXIDLE;
		    pak->datagramsize = 0;
		    pak->dataptr = pak->network_start;

		    mnp4->stats.garbled_received++;
		    errmsg(&msgsym(RCVGIANT, ARAP), tty->ttynum);
		}
	    }
	    break;

	  case MNP4_DLE_RECEIVED:
	    if (c == MNP4_ETX) {
		mnp4->istate = MNP4_CHECKSUM1;
	    } else if (c == mnp4->mnp4_dle) {
		*pak->dataptr++ = c;
		if (++pak->datagramsize > MNP4_MAXPACKETSIZE) {
		    /*
		     * Oversized packet received, clear it and go back to
		     * IDLE
		     */
		    mnp4->stats.garbled_received++;
		    mnp4->istate = MNP4_RXIDLE;
		    pak->datagramsize = 0;
		    pak->dataptr = pak->network_start;
		    errmsg(&msgsym(RCVGIANT, ARAP), tty->ttynum);
		} else {
		    mnp4->istate = MNP4_RECEIVE;
		}
	    } else {
		if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
			tty->ttynum)) {
		    buginf("\nMNP4 TTY %t:    ****Rcv bad esc", 
				tty->ttynum);
		    buginf("\n------------frame to point of error");
		    mnp4_mem_dump(pak->datagramstart, pak->datagramsize);

		    buginf("\n------------raw input error at %x",
				p - p_int - 1);
		    mnp4_mem_dump(p_int, len_int);
		}
		mnp4->stats.garbled_received++;
		mnp4->istate = MNP4_RXIDLE;
		pak->datagramsize = 0;
		pak->dataptr = pak->network_start;
	    }
	    break;
	  case MNP4_CHECKSUM1:
	    pak->mnp4_checksum = c;
	    mnp4->istate = MNP4_CHECKSUM2;
	    break;
	  case MNP4_CHECKSUM2:
	    pak->mnp4_checksum |= (c << 8);
	    pak->length = pak->datagramsize;
	    if (pak->datagramstart[MNP4_TYPE] == MNP4_LA) {
		enqueue(&mnp4->recv_ackQ, pak);
	    } else {
		enqueue(&mnp4->MNP4InQ, pak);
	    }
	    mnp4->mnp4_inpak = NULL;
	    pak = NULL;
	    mnp4->istate = MNP4_RXIDLE;
	    break;

	  case MNP4_RECEIVE_SHADOW:
	    if (c == mnp4->mnp4_dle) {
		mnp4->istate = MNP4_DLE_RECEIVED_SHADOW;
	    }
	    break;
 
	  case MNP4_DLE_RECEIVED_SHADOW:
	    if (c == MNP4_ETX) {
		mnp4->istate = MNP4_CHECKSUM1_SHADOW;
	    } else if (c == mnp4->mnp4_dle) {
		mnp4->istate = MNP4_RECEIVE_SHADOW;
	    } else {
		if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
			tty->ttynum)) {
		    buginf("\nMNP4 TTY %t:    ****Rcv bad esc", 
				tty->ttynum);
		    buginf("\n------------frame to point of error");
		    mnp4_mem_dump(pak->datagramstart, pak->datagramsize);

		    buginf("\n------------raw input error at %x",
				p - p_int - 1);
		    mnp4_mem_dump(p_int, len_int);
		}
		mnp4->stats.garbled_received++;
		mnp4->istate = MNP4_RXIDLE;
	    }
	    break;

	  case MNP4_CHECKSUM1_SHADOW:
	    mnp4->istate = MNP4_CHECKSUM2_SHADOW;
	    break;
	  case MNP4_CHECKSUM2_SHADOW:
	    /* mnp4->mnp4_inpak is already null */
	    mnp4->istate = MNP4_RXIDLE;
	    break;

	  default:
	    mnp4_default_crashdump(mnp4);
	}
    }
}

/*
 * mnp4_sndchar() Process output characters to be sent to the asynch line
 */
uint mnp4_sndchar (tt_soc * tty, uchar * p, int max)
{
    uchar c;
    paktype *pak;
    mnp4type *mnp4;
    int len = 0;
    mod_cntr ns;

    mnp4 = tty->mnp4ptr;
    if (mnp4 == NULL)
	return (0);

    pak = mnp4->mnp4_outpak;

    while (TRUE) {
	if (pak == NULL) {
	    if ((pak = prime_mnp4_outpak(mnp4)) == NULL)
		return (len);
	}
	/* this may seem a bit strange.  but there is a method to .. see
	 * comments in mnp4_packets_pending() need to leave a pak in
	 * mnp4->mnp4_outpak if output pending and not flow controlled.
	 */
	if (len >= max) {
	    return (len);
	}
	switch (mnp4->ostate) {

	  /* prime_mnp4_outpak() init's ostate to MNP4_START_STAGE0 */ 
	  case MNP4_START_STAGE0:
	    mnp4->ostate = MNP4_START_STAGE1;
	    MNP4_PUT_CHAR(mnp4->mnp4_syn);
	    break;
	  case MNP4_START_STAGE1:
	    mnp4->ostate = MNP4_START_STAGE2;
	    MNP4_PUT_CHAR(mnp4->mnp4_dle);
	    break;
	  case MNP4_START_STAGE2:
	    mnp4->ostate = MNP4_SEND;
	    MNP4_PUT_CHAR(MNP4_STX);
	    break;
	  case MNP4_SEND:
	    /* prime_mnp4_outpak() init's length to datagramsize */
	    if (pak->length == 0) {
		mnp4->ostate = MNP4_END_STAGE;
		MNP4_PUT_CHAR(mnp4->mnp4_dle);
	    } else {
		/* prime_mnp4_outpak() init's dataptr to datagramstart */
		c = *pak->dataptr;
		pak->dataptr++;
		pak->length--;
		if (c == mnp4->mnp4_dle)
		    mnp4->ostate = MNP4_DLE_SENT;
		MNP4_PUT_CHAR(c);
	    }
	    break;
	  case MNP4_DLE_SENT:
	    mnp4->ostate = MNP4_SEND;
	    MNP4_PUT_CHAR(mnp4->mnp4_dle);
	    break;
	  case MNP4_END_STAGE:
	    mnp4->ostate = MNP4_CHECKSUM1;
	    MNP4_PUT_CHAR(MNP4_ETX);
	    break;
	  case MNP4_CHECKSUM1:
	    mnp4->ostate = MNP4_CHECKSUM2;
	    c = pak->mnp4_checksum & 255;
	    MNP4_PUT_CHAR(c);
	    break;
	  case MNP4_CHECKSUM2:
	    c = pak->mnp4_checksum >> 8;
	    MNP4_PUT_CHAR(c);
	    GET_TIMESTAMP(pak->outputtime);

	    mnp4_log_put(mnp4, FALSE, pak->datagramstart);

	    /* resend timer needs to be started again giving accurate
	     * resend timeouts for LT frames.
	     */
	    switch (pak->datagramstart[MNP4_TYPE]) {
	      case MNP4_LT:
	      case MNP4_LT_H:
		ns = pak->datagramstart[MNP4_LT_SENDSEQ];
		if (!mnp4->resend[ns & MNP4_MOD_RESEND].resending) {
		    GET_TIMESTAMP(mnp4->resend[ns & MNP4_MOD_RESEND].time);
		}
		/* this pak was acked while being resent */
		if (mnp4->free_pak_on_send) {
		    mnp4->free_pak_on_send = FALSE;
		    arap_datagram_done(pak, tty->ttynum);
		}
		TIMER_START(mnp4->timer_resend, mnp4->t401);
		break;
	      case MNP4_LA:
		/*don't free our one and only ack pak */
		break;
	      case MNP4_LR:
	      case MNP4_LD:
		arap_datagram_done(pak, tty->ttynum);
		break;
	      default:
		mnp4_default_crashdump(mnp4);
	    }

	    /* prepare to send next pak */
	    mnp4->ostate = MNP4_TXIDLE;
	    mnp4->mnp4_outpak = NULL;
	    mnp4->stats.packets_sent++;
	    pak = NULL;
	    break;
	  default:
	    mnp4_default_crashdump(mnp4);
	}
    }
}

/*
 * mnp4BLOCK 
 */
boolean mnp4BLOCK (mnp4type * mnp4)
{
    /* Wake on TTY disconnect */
    if (modemcheck(mnp4->tty))
	return (FALSE);

    /* wake on time out */
    if (TIMER_RUNNING_AND_AWAKE(mnp4->wait_timer))
	return (FALSE);

    /* Wake on non-empty input Queue's */
    if (QUEUESIZE(&mnp4->MNP4InQ) || QUEUESIZE(&mnp4->recv_ackQ))
	return (FALSE);

    return (TRUE);
}

/*
 * wait_for_mnp4_remote -wait for specified MNP packet from other end.
 * 
 * Use this only at initial MNP4 link setup. Do not use this for LT packets
 * since they have a resend mechanism on timeout already. Output: Return OK
 * if desired message arrived Return TIMEDOUT on timeout Return ABORTED on
 * unexpected events Implicit Output: in a Link Ack, the mnp4 context is
 * updated Note that on packet arrival that does not match the desired
 * message, the packet is discarded, and the wait continues.
 * 
 */
int wait_for_mnp4_remote (mnp4type * mnp4, int message)
{
    paktype *pak;
    uchar *dataptr;
    long baud;
    tt_soc *tty = mnp4->tty;

    if (mnp4 == NULL)
	return (ABORTED);

    /*
     * The only wait messages accepted are LR, LA and LD.
     */
    if ((message != MNP4_LR) && (message != MNP4_LD) && (message != MNP4_LA)) {
	errmsg(&msgsym(WAITBADMSG, ARAP), tty->ttynum, message);
	mnp4_crashdump(mnp4);
    }
    /* set baud to the slower speed */
    if (tty->tty_txspeed > tty->tty_rxspeed) {
	baud = tty->tty_rxspeed;
    } else {
	baud = tty->tty_txspeed;
    }
    if (baud > MNP4_MAX_BAUD) {
	baud = MNP4_MAX_BAUD;
    }
    mnp4->t401_base = MNP4_T401_BAUD_CONSTANT/baud + MNP4_T401_LATENCY;
    mnp4->t401 = mnp4->t401_base;
    mnp4->t401_max = mnp4->t401_base * MNP4_MAX_BAUD_FACT;
    mnp4->t402 = mnp4->t401 / 2;
    errmsg(&msgsym(MNP4T401, ARAP), tty->ttynum, mnp4->t401, baud);
    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, tty->ttynum))
	buginf("\nMNP4 TTY %t: T401 %d baud rate %d", tty->ttynum,
	    mnp4->t401, baud);

    /* start timer for LR - its much larger value than T401 for LT's */
    TIMER_START(mnp4->wait_timer, mnp4->t401 * MNP4_LR_LD_WAIT_FACTOR);

    /*
     * Loop waiting for incoming MNP packet to match a desired message
     */
    while (TRUE) {
	/*
	 * Block waiting for non-empty Queue or some unusual event
	 */
	edisms((blockproc *) mnp4BLOCK, (int) mnp4);

	/*
	 * On TTY disconnect or timeout problems, return with the appropriate
	 * error message
	 */
	if (modemcheck(tty)) {
	    return (ABORTED);
	} else if (TIMER_RUNNING_AND_AWAKE(mnp4->wait_timer)) {
	    return (TIMEDOUT);
	}
	/*
	 * Dequeue an MNP4 message and see if it it matches the desired
	 * message
	 */
	if ((pak = p_dequeue(&mnp4->recv_ackQ)) ||
	    (pak = p_dequeue(&mnp4->MNP4InQ))) {

	    /* see if other than an LA we got what we are waiting for */
	    mnp4->stats.packets_received++;
	    mnp4_rcv_cashe_topoff(mnp4);
	    if (!mnp4_crc_check(pak, mnp4)) {
		continue;
	    }

	    dataptr = pak->datagramstart;

	    mnp4_log_put(mnp4, TRUE, dataptr);

	    /*
	     * Check out what the message is and whether it matches the
	     * request message type waited upon. On a match, return TRUE.
	     * Otherwise, message is discarded, and anticipated message
	     * waited again
	     */
	    switch (dataptr[MNP4_TYPE]) {
	      case MNP4_LR:
		/*
		 * handle a link request
		 */
		if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, tty->ttynum))
		    buginf("\nMNP4 TTY %t:Wait Rcv LR", tty->ttynum);
		if (dataptr[MNP4_TYPE] == message) {
		    if(ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
			    tty->ttynum))
			buginf("\nMNP4 TTY %t:Wait Rcv match on LR",
			    tty->ttynum);
		    arap_datagram_done(pak, tty->ttynum);
		    mnp4->link_state = MNP4_DATA_PHASE;
		    return (OK);
		}
		arap_datagram_done(pak, tty->ttynum);
		break;
	      case MNP4_LD:
		/*
		 * disconnect the link
		 */
		if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, tty->ttynum))
		    buginf("\nMNP4 TTY %t:Wait Rcv LD", tty->ttynum);
		if (dataptr[MNP4_TYPE] == message) {
		    if(ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
			    tty->ttynum))
			buginf("\nMNP4 TTY %t:Wait Rcv match on LD",
			    tty->ttynum);
		    arap_datagram_done(pak, tty->ttynum);
		    return (OK);
		}
		arap_datagram_done(pak, tty->ttynum);
		mnp4->link_state = MNP4_DISCONNECTING;
		return (ABORTED);
	      case MNP4_LA:
		if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,	
			mnp4->tty->ttynum))
		    buginf("\nMNP4 TTY %t:Wait Rcv LA", tty->ttynum);
		if (dataptr[MNP4_TYPE] == message) {
		    mnp4_process_ack(pak, mnp4);
		    if(ARAP_BUGINF(arapmnp4_debug,aramnp4tty_debug,tty->ttynum))
			buginf("\nMNP4 TTY %t:Wait Rcv match on LA",
			    tty->ttynum);
		    mnp4->link_state = MNP4_DATA_PHASE;
		    return (OK);
		}
		mnp4_process_ack(pak, mnp4);
		break;
	      case MNP4_LT_H:
		if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, tty->ttynum))
		    buginf("\nMNP4 TTY %t:Wait Rcv LT_H/discarded",
			tty->ttynum);
		arap_datagram_done(pak, tty->ttynum);
		break;
	      case MNP4_LT:
		if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, tty->ttynum))
		    buginf("\nMNP4 TTY %t:Wait Rcv LT/discarded",
			tty->ttynum);
		arap_datagram_done(pak, tty->ttynum);
		break;
	      case MNP4_LN:
		if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, tty->ttynum))
		    buginf("\nMNP4 TTY %t:Wait Rcv LN/discarded",
			tty->ttynum);
		arap_datagram_done(pak, tty->ttynum);
		break;
	      case MNP4_LNA:
		if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, tty->ttynum))
		    buginf("\nMNP4 TTY %t:Wait Rcv LNA/discarded",
			tty->ttynum);
		arap_datagram_done(pak, tty->ttynum);
		break;
	      default:
		if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, tty->ttynum))
		    buginf("\nMNP4 TTY %t:Wait Rcv Unknown MNP4 pak/discarded",
				tty->ttynum);
		arap_datagram_done(pak, tty->ttynum);
		break;
	    }
	}			/* end if */
    }				/* end while */
}

boolean mnp4_timers (mnp4type * mnp4)
{
    leveltype level;

    if (mnp4 == NULL)
	return (FALSE);

    if (mnp4->link_state != MNP4_DATA_PHASE) {
	if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug, mnp4->tty->ttynum))
	    buginf("\nMNP4 TTY %t:Timeout but not in data phase",
		mnp4->tty->ttynum);
	TIMER_STOP(mnp4->timer_ack_delay);
	TIMER_STOP(mnp4->timer_resend);
    } else {

	if (TIMER_RUNNING_AND_AWAKE(mnp4->timer_ack_delay)) {
	    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
		    mnp4->tty->ttynum))
		buginf("\nMNP4 TTY %t:Ack timeout ", mnp4->tty->ttynum);
	    mnp4_send_ack(mnp4);
	}
	if (TIMER_RUNNING_AND_AWAKE(mnp4->timer_resend)) {
	    TIMER_STOP(mnp4->timer_resend);

	    if (ARAP_BUGINF(arapmnp4_debug, aramnp4tty_debug,
		    mnp4->tty->ttynum))
		buginf("\nMNP4 TTY %t:Resend timeout", mnp4->tty->ttynum);

	    level = raise_interrupt_level(NETS_DISABLE);
	    /* resend all unacked frames.  but 1st check resend counter */
	    if (mnp4->resend_cntr++ >= MNP4_MAX_RESENDS) {
		errmsg(&msgsym(MAXRESENDS, ARAP), mnp4->tty->ttynum);
		reset_interrupt_level(level);
		return (FALSE);
	    } else if (mnp4->rvk == 0) {	/* he turned us off - bitch */
		mnp4_send_ack(mnp4);
	    } else if (mnp4->vs != mnp4->rvr) {
		if (mnp4->do_resend) {
		    /*not good. resend_cntr will put us out of our misery!*/
		    errmsg(&msgsym(RESENDSLOW, ARAP), mnp4->tty->ttynum);
		    TIMER_START(mnp4->timer_resend, mnp4->t401);
		} else {
		    mnp4_do_resends(mnp4);
		}
	    }
	    reset_interrupt_level(level);
	}
    }
    return (TRUE);
}

/*
 * mnp4_destroy_setup
 */
void mnp4_destroy_setup (mnp4type *mnp4)
{
    paktype *pak;

    if (mnp4 == NULL) {
	return;
    }

    if (mnp4->log_ptr) {
	free(mnp4->log_ptr);
    }

    while ((pak = p_dequeue(&mnp4->bufferQ))) {
	arap_datagram_done(pak, mnp4->tty->ttynum);
    }

    if (mnp4->pak_ack) {
	arap_datagram_done(mnp4->pak_ack, mnp4->tty->ttynum);
    }

    free(mnp4);
}
