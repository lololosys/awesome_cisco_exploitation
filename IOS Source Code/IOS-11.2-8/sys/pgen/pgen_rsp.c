/* $Id: pgen_rsp.c,v 3.5.12.1 1996/03/18 21:35:22 gstovall Exp $ 
 * $Source: /release/111/cvs/Xsys/pgen/pgen_rsp.c,v $ 
 *------------------------------------------------------------------
 * Pagent RSP specific routines
 *
 * July 1995, Brian Jackson
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pgen_rsp.c,v $
 * Revision 3.5.12.1  1996/03/18  21:35:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.3  1996/03/13  01:52:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.12.2  1996/03/07  10:41:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  16:58:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/13  05:51:50  ogrady
 * CSCdi45887:  Router crash in rsp_fastsend
 *              Allocate memd buffers with extra padding for
 *              memdcopy() overrun.
 *
 * Revision 3.4  1996/01/23  11:55:41  mdb
 * CSCdi47339:  apply code review comments for CSCdi47065
 *
 * Revision 3.3  1995/12/22  21:58:49  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.2  1995/11/17  00:21:57  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:59:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/14  01:52:11  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 * 
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "ttysrv.h"
#include "parser.h"
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "../if/network.h"
#include "stacks.h"
#include "pgen_registry.h"
#include "pgen_private.h"

#include "../machine/cpu_4k.h"
#include "../src-rsp/rsp_bufhdr.h"
#include "../src-rsp/rsp_if.h"
#include "../src-rsp/rsp_qasic.h"
#include "../src-rsp/rsp_pcmap.h"
#include "../src-rsp/qa_hw.h"
#include "../src-rsp/rsp_fast.h"
#include "../src-rsp/rsp_cache.h"
#include "../hes/if_cmd.h"

#include "pgen_rsp.h"

/*
 * =====================================================================
 * Pagent subsystem and registry initialization
 * =====================================================================
 */

/* Prototypes */
static void pgen_rsp_init(subsystype *);

/* Pagent platform specific subsystem header */
#define MAJVERSION  2
#define MINVERSION  2
#define EDITVERSION 0

SUBSYS_HEADER(pgen_rsp, MAJVERSION, MINVERSION, EDITVERSION,
              pgen_rsp_init, SUBSYS_CLASS_PROTOCOL,
              "seq: rsp, pgen",
              "req: rsp, pgen");

void pgen_rsp_init (
    subsystype *subsys)          /* Subsystem header pointer */
{

    /*
     * Allocate a Pagent private buffer return pool.  We point our
     * preallocated packets to this return pool so that they aren't
     * returned to the system pool and reallocated.
     */
    int qindex = get_hwq(QT_GFREE, 1);
    if (qindex == QA_ALLOC_ERROR) {
	printf("\n%%Error allocating pgen fast send return queue");
    } else {
	pgen_qptr = queue_ptr(qindex);

	/* Register the RSP platform fast send service */
	reg_add_pgen_send(pgen_fastsend, "pgen_fastsend");

        /*
	 * Allocate a Pagent private hardware queue.  We use this
	 * queue to forward packets for interfaces not in fast count
         * mode to the standard input processing routines by pointing
         * the RSP global RawQ to this queue while any interface is in
         * fast count mode.
         */
	qindex = get_hwq(QT_GFREE, 1);
	if (qindex == QA_ALLOC_ERROR) {
	    printf("\n%%Error allocating pgen fast count queue");
	} else {
	    pgen_RawQ = queue_ptr(qindex);

	    /*
	     * Enable interrupts for the Pagent input raw queue and
	     * steal the input interrupt.  We'll pass non-fast count
	     * interface input to the standard input processor.
	     */
	    set_qa_rp_attention(qindex, QA_ATTN_NET);
	    changelevel(LEVEL_NETWORK, pgen_process_rawq, "Network Interrupt");

	    /* Register the RSP platform fast count service */
	    reg_add_pgen_count(pgen_fastcount, "pgen_fastcount");
	}
    }

} /* pgen_rsp_init */

/*
 * =====================================================================
 * Pagent RSP Fast Send routines
 *
 * These routine preallocate MEMD buffer for each packet to be sent
 * and enqueues those buffers directly on the hardware output queue for
 * the interface
 * =====================================================================
 */

/*
 * Preallocate MEMD buffers for the packets to be transmitted by the RSP.
 * Point the buffer return pool back to our private pgen buffer pool so we
 * can repeatedly dequeue and reenqueue the packets during transmission.
 * Before changing the contents of the pak structure, save the original
 * contents so that we can restore them after transmission.
 *
 * This routine is called once before the main transmission loop is entered.
 * Its sister routine is called once afterward to restore the original router
 * state.
 *
 * Return of 1 indicates success, else 0 is returned.
 */
static int pgen_init_paks (
    paktype *firstpak,          /* First packet to send */
    paktype *lastpak)           /* Last packet to send */
{
    paktype        *pak;        /* Current packet being processed */
    bufhdr_t	   *bufptr;     /* MEMD buffer header pointer */
    int		   size;        /* Size of the current packet */
    uchar	   *start;      /* Start of the current packet in DRAM */
    ushort         offset;      /* Local storage index */
    pgen_bufpool_t *bufpool;    /* The pgen MEMD buffer pool for the pak */

    /*
     * Preallocate MEMD buffers for each packt to be transmitted.  We'll
     * modify the pak struct to insure these buffers are not returned to the
     * system buffer pool so we can repeatedly retransmit them without having
     * to allocate new MEMD buffers.
     */
    for (pak = firstpak; pak != lastpak; pak = pak->next) {

        /* Allocate a local pgen MEMD buffer pool for each packet */
	pak->pgen_bufpool = malloc(sizeof(pgen_bufpool_t)*PGEN_NUMBUF);
	if (!pak->pgen_bufpool) {
	    printf("\n%%Unable to allocate pgen buffer pool");
	    return 0;
	}
	pak->pgen_bufcur = 0;

        /* Allocate a fixed number of MEMD buffers per packet */
	size = pak->datagramsize;
	bufpool = (pgen_bufpool_t *)pak->pgen_bufpool;
	for (offset = 0; offset < PGEN_NUMBUF; offset++, bufpool++) {

	    /* Allocate a MEMD buffer */
	    bufptr = get_memd_buffer(size + RSP_BUFSTART_PAD + MEMDCOPY_PAD);
	    if (!bufptr) {
		printf("\n%%Unable to preallocate pgen MEMD buffer");
		return 0;
	    }

	    /* Copy the packet to MEMD and initialize the buffer header */
	    start = MEMD_OFFSET_TO_UNCACHED_ADDR(bufptr->bf_buffer_ptr +
		RSP_BUFSTART_PAD);
	    memdcopy(pak->datagramstart, start, size);

	    bufptr->bf_tx_ptr = ADDR_TO_MEMD_OFFSET_24(start);
	    bufptr->bf_tx_bytes = size;

	    /* Save original packet values and point reinitialize */
	    bufpool->bf_queue_ptr = bufptr->bf_queue_ptr;
	    bufptr->bf_queue_ptr = 0;
	    bufpool->bufptr = bufptr;
	}
    }

    /* Initialize the delay until time to now */
    pgen_delayuntil = getcp0_count_inline();

    return 1;
    
} /* pgen_init_paks */

/*
 * Free the preallocated MEMD buffers and the pgen buffer pool.
 * Restore that pak struture to its original contents
 */
static void pgen_restore_paks (
    paktype *firstpak,          /* First packet to send */
    paktype *lastpak)           /* Last packet to send */
{
    paktype     *pak;           /* Current packet being sent */
    bufhdr_t    *bufptr;        /* Buffer header of packet to transmit */
    bufhdr_t    *abufhdr;       /* Buffer header dequeued from private pool */
    ushort      offset;         /* Local storage index */
    pgen_bufpool_t *bufpool;    /* The pgen MEMD buffer pool for the pak */

    /*
     * Free MEMD buffers and restore values for each packet
     */
    for (pak = firstpak; pak != lastpak; pak = pak->next) {

        /* Clear the buffer header pointer, none is allocated */
	pak->buffer_header = NULL;

	bufpool = (pgen_bufpool_t *)pak->pgen_bufpool;
	if (!bufpool)
	   continue;

        /* For each MEMD buffer allocated per packet */
	for (offset = 0; offset < PGEN_NUMBUF; offset++, bufpool++) {

            /* Return each MEMD buffer to its original buffer pool */
	    bufptr = bufpool->bufptr;
	    if (bufptr) {

		/* Dequeue packet from the pgen queue */
		while (bufptr->bf_queue_ptr) {
		    abufhdr = hwq_dequeue_inline(pgen_qptr);
		    if (abufhdr != NULL) 
		        abufhdr->bf_queue_ptr = 0;
		}

		/* Return the MEMD buffer to the pool */
		bufptr->bf_queue_ptr = bufpool->bf_queue_ptr;
		hwq_enqueue(QASIC_OFFSET_TO_ADDR(bufptr->bf_queue_ptr),bufptr);
	    }
	}

        /* Free the Pagent buffer pool pointers for this packet */
	free(pak->pgen_bufpool);
    }

} /* pgen_restore_paks */

/*
 * Specialized RSP packet transmit routine that dequeues preallocated MEMD
 * buffers from a local buffer pool and enqueues them on a hardware output
 * queue.
 */
void pgen_fastsend (
    paktype *firstpak,          /* First packet to send */
    paktype *lastpak,           /* Last packet to send */
    int repeat,                 /* Times to repeat the packet sequence */
    int delay)                  /* Time to delay between each repeat loop */
{
    paktype *pak;               /* Current packet being sent */
    sys_timestamp starttime;    /* Time at which we start sending */
    sys_timestamp wakeuptime;   /* Constructed sleep timer */
    hwidbtype *hwidb;           /* Output interface information */
    int copies;                 /* Number of times we've sent this packet */
    bufhdr_t    *bufptr;        /* Buffer header of packet to transmit */
    bufhdr_t    *abufhdr;       /* Buffer header dequeued from private pool */
    pgen_bufpool_t *bufpool;    /* The pgen MEMD buffer pool for the pak */
    int incr, rptnum;           /* Variables used in repeat loop */

    /* Preallocate MEMD buffers for fast RSP transmission */
    if (!pgen_init_paks(firstpak, lastpak)) {
        pgen_restore_paks(firstpak, lastpak);
        return;
    }

    /*
     * Replay repeat loop.
     * Repeat count of 0 indicates loop forever.
     */
    incr = repeat ? 1 : 0;
    for (rptnum = incr; rptnum <= repeat; rptnum += incr) {

	/* This timing loop is relative to the start time */
	GET_TIMESTAMP(starttime);

        /* Traverse packets to be transmitted loop */
	for (pak = firstpak; pak != lastpak; pak = pak->next) {

	    /* If this packet's time has not yet come, wait for it. */
	    if (CLOCK_IN_INTERVAL(starttime, pak->pgen_tick)) {
		if (psiset(stdio))
		    return;
		wakeuptime = TIMER_ADD_DELTA(starttime, pak->pgen_tick);
                process_sleep_until(&wakeuptime);
	    }

	    /* Make sure the interface is up */
	    hwidb = hwidb_or_null(pak->pgen_output);
	    if ((!hwidb) || (hwidb->state != IDBS_UP))
	        continue;

	    /* Update the interface accounting information */
	    hwidb->counters.outputs += pak->pgen_rptcount;
	    hwidb->counters.tx_cumbytes += pak->datagramsize*pak->pgen_rptcount;

	    /* Point to this packets MEMD buffer pool */
	    bufpool = (pgen_bufpool_t *)pak->pgen_bufpool;

	    /* Per packet repeat loop */
	    for (copies = 0; copies < pak->pgen_rptcount; copies++) {

		/* Select the next MEMD buffer from the MEMD buffer pool */
		if (++pak->pgen_bufcur >= PGEN_NUMBUF)
		    pak->pgen_bufcur = 0;
		bufptr = (bufpool+pak->pgen_bufcur)->bufptr;

		/* Dequeue packets from the pgen queue til our packets free */
		while (bufptr->bf_queue_ptr) {
		    abufhdr = hwq_dequeue_inline(pgen_qptr);
		    if (abufhdr)
		        abufhdr->bf_queue_ptr = 0;
		}

		/* Delay until it is time to send the packet */
		if (pak->pgen_interval) {

		    while (getcp0_count_inline() < pgen_delayuntil) {
			if (pgen_delayuntil > getcp0_count_inline() +
			    (pak->pgen_interval / 20)) {
			    pgen_delayuntil = 0L;
			    break;
			}
			if (psiset(stdio))
			    return;
		    }

		    /* Convert nsecs to 20 nsec ticks for next delay */
		    pgen_delayuntil += pak->pgen_interval / 20;
		}

		/* Enqueue the MEMD packet on the outgoing hardware queue */
		bufptr->bf_queue_ptr = ADDR_TO_MEMD_OFFSET_16(pgen_qptr);
		hwq_enqueue_inline(hwidb->tx_q_ptr, bufptr);
	    }
	}

        /* Delay if we're gonna do it again */
        if ((delay != -1) && (rptnum < repeat)) {
            /* Check for user interrupt */
            if (psiset(stdio))
                break;
            process_sleep_for (delay/1000);
	}
    }

    /* Free memory and restore the pak struct */
    pgen_restore_paks(firstpak, lastpak);

} /* pgen_fastsend */

/*
 * =====================================================================
 * Pagent RSP Fast Count routines
 *
 * These routine turn fast counting on or off for an interface and implement
 * the the input queue interrupt handler.
 * =====================================================================
 */

/*
 * Turn on/off interface fast counting by pointing the interface
 * packet input queue to Pagent queue or the standard queue.  Pagent
 * queue packets are counted and dropped.
 *
 * Display whether interfaces are in fast count or normal mode.
 */
void pgen_fastcount (
    hwidbtype *hwidb,           /* Hardware idb for counting interface */
    unsigned mode)              /* Mode: 0 == off, 1 == on, 2 == status */
{
    static char *mode_str[] = {"normal","count-only"};

    /* Process a single interface or all interfaces if hwidb is NULL */
    PGEN_FOR_HWIDBS(hwidb) {

	/* Enable or disable fast count mode */
	if (mode < 2) {
	    hwidb->raw_q_ptr = (mode) ? pgen_RawQ : RawQ;
	    send_raw_queue_ptr_cmd(hwidb,
				   BUFHDR_ADDR_TO_OFF(hwidb->raw_q_ptr));
	} 

	/* Display interface status */
	printf ("\n%14s %s", hwidb->hw_namestring, 
		mode_str[(hwidb->raw_q_ptr == pgen_RawQ) ? 1 : 0]);
    }

} /* pgen_fastcount */

/*
 * Pagent raw input queue interrupt handler.  Dequeue packets from the
 * fast count input queue, count them, and return them to the free
 * queue.  Call the standard input queue interrupt handler if there
 * is anything to process on the standard input queues.
 */
void pgen_process_rawq (void)
{
    hwidbtype   *hwidb;            /* Input interface hwidb pointer */
    bufhdr_t    *bufptr;           /* Buffer header pointer */
    ulong       level;             /* Saved interrupt level */

    /* Process the Pagent fast counting input raw queue */
    GLOBAL_INTERRUPT_BLOCK(level);
    while ((bufptr = hwq_dequeue_inline(pgen_RawQ))) {

	/* Flush the cache */
	hit_invalidate_cache_line_inline(bufptr);

	/* Restore network interrupts */
	GLOBAL_INTERRUPT_UNBLOCK(level);
    
	/* Update interface counters */
	hwidb = LOCAL_QUEUE_TO_HW_IDB(bufptr->bf_queue_ptr);
	hwidb->counters.inputs++;
	hwidb->counters.rx_cumbytes += bufptr->bf_rx_bytes;
	
	/* Free the packet */
	hwq_enqueue_inline(QASIC_OFFSET_TO_ADDR(bufptr->bf_queue_ptr), bufptr);

	/* Mask network interrupts while dequeuing */
	GLOBAL_INTERRUPT_BLOCK(level);
    }

    /* Restore network interrupts */
    GLOBAL_INTERRUPT_UNBLOCK(level);
    
    /* Process the standard input raw queues (only if needed) */
    if (hwq_peek_inline(RawQ) || hwq_peek_inline(ReturnQ))
	rsp_process_rawq();

} /* pgen_process_rawq */
