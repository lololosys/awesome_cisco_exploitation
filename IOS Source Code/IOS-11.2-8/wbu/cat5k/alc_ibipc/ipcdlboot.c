/* $Id: ipcdlboot.c,v 1.1.4.1 1996/05/09 14:55:22 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_ibipc/ipcdlboot.c,v $
 *------------------------------------------------------------------
 * ipcdlboot.c
 *
 * March 1995, jjchen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipcdlboot.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:22  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:50  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:42  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:59  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:50  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:18  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.6  1995/10/04  01:07:44  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.5  1995/06/15  09:39:35  rlowe
 * Complete change-over from ATM driver to BIGA driver.  Remove
 * Rx packet upcall to ATM driver.
 *
 * Revision 1.4  1995/06/14  22:49:41  rlowe
 * Change ADVR_RxSarPollSvc, ADVR_TxSarPollSvc to have "fOnce" flag.
 *
 * Revision 1.3  1995/06/13  04:55:22  rlowe
 * Changes to get BIGA driver running in the Boots for download use.
 *
 * Revision 1.2  1995/05/05  16:33:26  jjchen
 * Modified to support ALC boot download.
 *
 * Revision 1.1  1995/05/01  23:51:55  jjchen
 * Moved from syn_core/nmp/nmpflash/ibipc.
 *
 * Revision 1.1  1995/03/29  05:44:21  jjchen
 * Created.
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "utypes.h"

#ifndef NO_VLAN
#include "synpacket.h"
#endif

#include "ipcboot.h"

#include "ibframe.h"

#include "ibipc.h"
#include "ib_externs.h"

extern IBIPC_DL_CB_T ibdlcb;

/* local variables */

int system_timer;

int ipc_dl_system_timer;

paktype packet[NUM_OF_PAKS];

queuetype ipc_q;

/* routines */
void ipc_pkt_rx(void *, uInt32);
void datagram_done(paktype *);

/*
 * IPC download timer
 */
void ipc_dl_timer()
{
	int delta_time;

	if (!ibdlcb.timer_index || ipc_dl_system_timer == system_timer)
	    return;

	delta_time = system_timer - ipc_dl_system_timer;

	if (ibdlcb.timer_index <= delta_time) {
	    ibdlcb.timer_index = 0;
	    ipc_dl_timeout();
	}
	else
	    ibdlcb.timer_index -= delta_time;
}

#ifdef COMMENT_OUT

int cfork(void (* func)(), int arg1, int arg2, uChar * str, int arg3)
{

	func();
	return(1);
}

/*
 * IPC donwload daemon.
 */

int systeminitBLOCK()
{
	return(0);
}

int ipcdlBLOCK()
{
	int rc = 1;

	/* 
	 * Check if received a packet (set rc = 0).
	 */
	
	/*
	 * Serve the download timer.
	 */
	ipc_dl_timer();

	return(rc);
}

void edisms(int (* func)(), int arg)
{
	while(func()) {
	     /* kick watch dog timer and something else ... */
	}
}

#endif /* COMMENT_OUT */

void queue_init(queuetype * q, int flag)
{
	int i;

	/* Initialize queue.
	 */
	q->mp = 0;

	/* Initialize ipc_q and packet structures.
	 */
	ipc_q.mp = 0;

	for (i = 0; i < NUM_OF_PAKS; i++) {
	    packet[i].linktype = 0;
	    packet[i].datagramstart = packet[i].datagram;
	}

#if	0
	ADVR_RegInPktUpCall(ipc_pkt_rx);
#endif
}

paktype * pak_dequeue(queuetype * pq)
{
	paktype * pak;
	
	if (pak = (paktype *)pq->mp) {
	    pq->mp = 0;
	}
	return(pak);
}

void pak_enqueue(queuetype * qp, void * mp)
{
	/* It is always one packet element in the queue,
	 * since this is singe thread which polls received packet one at a time.
	 */
	if (!qp->mp)
	    qp->mp = mp;
	else
	    datagram_done(mp);
}

#ifndef MENFUNC_SUPPORT

memblk mb[NUM_OF_MB];
static memfunc_call_once = 0;

uChar * malloc(uInt32 size)
{
	int i;
	uChar * mp = 0;

	if (!memfunc_call_once) { /* initialize once */
	   memfunc_call_once = 1;
	   for (i = 0; i < NUM_OF_MB; i++)
		mb[i].used = 0;
	}

	if (size > MB_SIZE) return(mp);
	
	for (i = 0; i < NUM_OF_MB; i++) {
	    if (mb[i].used) continue;
	    mb[i].used = 1;
	    mp = mb[i].mp;
	    break;
	}
	return(mp);
}

void free(uChar * mp)
{
	int i;

	for (i = 0; i < NUM_OF_MB; i++) {
	    if (!mb[i].used || mb[i].mp != mp) continue;
	    mb[i].used = 0;
	    break;
	}
}

#endif /* MENFUNC_SUPPORT */

void datagram_done(paktype * mptr)
{
	mptr->linktype = 0;
}

void datagram_out(paktype *mptr)
{

}

paktype * getbuffer(int mlen)
{
	int i;

	if (mlen > DATAGRAM_SIZE)
	    return(0);

	for (i = 0; i < NUM_OF_PAKS; i++)
	    if (!packet[i].linktype) {
		packet[i].linktype = 1;
		return(&packet[i]);
	    }

	return(0);
}

void ipc_pkt_rx(void * dp, uInt32 len)
{
	paktype * pak;

	if (!(pak = getbuffer(len)))
	    return;

	bcopy(dp, pak->datagramstart, len);
	pak->datagramsize = len;

	pak_enqueue(&ipc_q, pak);
}

int ipc_boot_dl()
{
	paktype * pak;
	int       rc = 1;
	
#ifdef 0
	while(ibdlcb.in_progress != IBIPC_DL_STATE_WRITE_FLASH &&
	      ibdlcb.in_progress != IBIPC_DL_STATE_ABORT)
#endif
	{

	    /* 
	     * Check any received packet.
	     */

	    if (pak = pak_dequeue(&ipc_q)) {

		if (is_ipc_pkt(pak)) {
		    Ipc_rx((uChar *)pak);
		    ipc_dl_rx();
	        }
	    	else {
		    datagram_done(pak); /* free it */
	    	}
	    }
	}

#if	0
		/* Poll the SARs. */
	ADVR_RxSarPollSvc (TRUE) ;
	ADVR_TxSarPollSvc (TRUE) ;
#else
		/* Poll the BIGA. */
	BT_biga_rx_pkt (TRUE) ;
#endif

	if (ibdlcb.in_progress == IBIPC_DL_STATE_WRITE_FLASH)
	    rc = 0;
	else {
	    if (ibdlcb.in_progress == IBIPC_DL_STATE_ABORT) {
		ipc_dl_exit(0, 0);
		rc = -1;
	    }
	}

	return(rc);
}
