/* $Id: pgen_4500.c,v 3.3.44.2 1996/04/17 13:47:02 fred Exp $ 
 * $Source: /release/112/cvs/Xsys/pgen/pgen_4500.c,v $ 
 *------------------------------------------------------------------
 * Pagent 4500 specific routines
 *
 * July 1995, Brian Jackson
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pgen_4500.c,v $
 * Revision 3.3.44.2  1996/04/17  13:47:02  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.3.44.1  1996/03/18  21:35:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.2  1996/03/13  01:52:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.12.1  1996/02/20  16:58:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/22  21:58:48  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.2  1995/11/17  00:21:54  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:59:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/14  01:52:04  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 * 
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "ttysrv.h"
#include "parser.h"
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "../if/network.h"
#include "../machine/cpu_4k.h"
#include "pgen_registry.h"
#include "pgen_private.h"

/*
 * =====================================================================
 * Subsystem and registry initialization
 * =====================================================================
 */

/* Prototypes */
static void pgen_4500_init(subsystype *);

/* Pagent platform specific subsystem header */
#define MAJVERSION  2
#define MINVERSION  2
#define EDITVERSION 0

SUBSYS_HEADER(pgen_4500, MAJVERSION, MINVERSION, EDITVERSION,
              pgen_4500_init, SUBSYS_CLASS_PROTOCOL,
              "seq: pgen",
              "req: pgen");

void pgen_4500_init (
    subsystype *subsys)          /* Subsystem header pointer */
{

    /* Register the 4500 platform specific services */
    reg_add_pgen_send(pgen_fastsend, "pgen_fastsend");

} /* pgen_rsp_init */

/*
 *
 =====================================================================
 * Pagent 4500 Fast Send routines
 *
 * This routine acheives higher transmission throughput by calling
 * the interface fastsend routine in preference to the soutput routine.
 * Additionally, fine granularity packet intervals are provided
 * through calls to the 20 nsec R4000 clock.
 *
 =====================================================================
 */

/* Clock tick counter used to control packet transmission rates */
static ulong pgen_delayuntil;

/*
 * Byte swap packets destined for an FDDI interface.
 * Initialize the delay until time to now.
 */
static void pgen_init_paks (
    paktype *firstpak,          /* First packet to send */
    paktype *lastpak)           /* Last packet to send */
{
    paktype *pak;               /* Current packet being sent */
    hwidbtype *hwidb;           /* Output interface information */

    /* Byte unswap the mac addresses for FDDI interfaces */
    for (pak = firstpak; pak != lastpak; pak = pak->next) {
        hwidb = hwidb_or_null(pak->pgen_output);
        if (hwidb != NULL && hwidb->type == IDBTYPE_BSI) {
            capture_swap_fddi_hdr(pak);
	}
    }

    /* Initialize the delay until time to now */
    pgen_delayuntil = getcp0_count_inline();

    return;
}

/*
 * Restore byte swapped FDDI packets.
 */
static void pgen_restore_paks (
    paktype *firstpak,          /* First packet to send */
    paktype *lastpak)           /* Last packet to send */
{
    paktype *pak;               /* Current packet being sent */
    hwidbtype *hwidb;           /* Output interface information */

    /* Byte unswap the mac addresses for FDDI interfaces */
    for (pak = firstpak; pak != lastpak; pak = pak->next) {
        hwidb = hwidb_or_null(pak->pgen_output);
        if (hwidb != NULL && hwidb->type == IDBTYPE_BSI) {
            capture_swap_fddi_hdr(pak);
	}
    }

    return;
}

/* 
 * Transmit packets using the `fastsend' routine for the outgoing interface.
 * Implement repeat loops honoring all packet rate configuration requests.
 * No attempt is made to resend the packet if transmission fails.
 */
void pgen_fastsend (
    paktype *firstpak,          /* First packet to send */
    paktype *lastpak,           /* Last packet to send */
    int repeat,                 /* Times to repeat the packet sequence */
    int delay)                  /* Time to delay between each repeat loop */
{
    paktype *pak;               /* Current packet being sent */
    queuetype *qptr;            /* Pointer to the output hold queue */
    ulong qmaximum;             /* Save original maximum hold queue size */
    sys_timestamp starttime;    /* Time at which we start sending */
    sys_timestamp wakeuptime;   /* Constructed sleep timer */
    hwidbtype *hwidb;           /* Output interface information */
    int copies;                 /* Number of times we've sent this packet */
    int incr, rptnum;           /* Variables used in repeat loop */

    /* Initialize the transmission environment */
    pgen_init_paks(firstpak, lastpak);

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

	    /* Per packet repeat loop */
	    for (copies = 0; copies < pak->pgen_rptcount; copies++) {

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

		pak_lock(pak);      /* Hold onto this sucker... */

		/*
		 * Transmit the packet using the fastsend routine.
		 * Set the holdq size to 0 to insure that the packet
		 * is dropped rather than placed on the holdq since
		 * hold queue processing corrupts packets.
		 */
		qptr = &(hwidb->outputq[PRIORITY_NORMAL]);
		qmaximum = qptr->maximum;
		qptr->maximum = 0;

		pak->if_output = pak->pgen_output;
		(*hwidb->fastsend) (hwidb, pak);

		qptr->maximum = qmaximum;
		
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

    /* Restore router environment */
    pgen_restore_paks(firstpak, lastpak);

    return;
}

