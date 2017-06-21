/* $Id: ntp_show.c,v 3.2.62.4 1996/09/04 23:04:20 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ntp/ntp_show.c,v $
 *------------------------------------------------------------------
 * "show" routines for NTP
 *
 * December 1992, Dave Katz
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ntp_show.c,v $
 * Revision 3.2.62.4  1996/09/04  23:04:20  snyder
 * CSCdi68106:  save some space in ntp
 *              80 from image, 104 from data section
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/08/19  18:55:20  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.2.62.2  1996/05/09  14:34:33  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.62.1.4.1  1996/04/27  07:16:14  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.62.1  1996/04/16  19:03:28  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2.84.1  1996/04/08  01:59:25  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.1  1996/04/05  03:18:39  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2  1995/11/17  17:51:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:41:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:28:48  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  21:45:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CISCO

#include "master.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../os/clock.h"
#include "ntp_public.h"
#include "ntpd.h"
#include "ntp_stdlib.h"
#include "ntp_refclock.h"
#include "../os/clock_private.h"
#include "ntp_registry_private.h"
#include "config.h"

static char *const modestring[] = {
    "unspec",
    "active",
    "passive",
    "client",
    "server",
    "bdcast",
    "control",
    "private",
    "bdcast client"
    };

/*
 * Form an ASCII string from a refclock ID of the form ".GOES."
 *
 */
static void form_refclock_ascii(char *refclock, char *string)
{
    int i;
    string[0] = '.';
    for (i=0; i<4; i++) {
	if (refclock[i] == '\0')
	    break;
	string[i+1] = refclock[i];
    }
    string[i+1] = '.';
    string[i+2] = '\0';
}
    
/*
 * show ntp associations [detail]
 *
 * OBJ(int,1) = TRUE if "detail" specified
 */

void show_ntp_assoc(parseinfo *csb)
{

    struct peer *peer;
    int hash;
    char local_string[20];
    int time_since_rcv;
    int i;
    ulong index;
    ntp_refclock *refclock;

    if (!ntp_running)
	return;

    if (!GETOBJ(int,1)) {	/* Not detailed */
	automore_enable(
"\n      address         ref clock     st  when  poll reach  delay  offset    disp");

	for (hash = 0; hash < HASH_SIZE; hash++) {
	    for (peer = peer_hash[hash]; peer; peer = peer->next) {

		/* Skip the broadcast peers. */

		if (peer->hmode == MODE_BROADCAST)
		    continue;
		
		/* First print a flag according to the peer status. */
		
		if (peer == sys_peer) {
		    if (sys_leap == LEAP_NOTINSYNC)
			printf("\n#");
		    else
			printf("\n*");
		} else if (peer->select) {
		    printf("\n+");
		} else if (peer->candidate) {
		    printf("\n-");
		} else {
		    printf("\n ");
		}
		
		/* Now flag whether or not the peer is configured or not. */

		if (peer->flags & FLAG_CONFIG)
		    printf("~");
		else
		    printf(" ");

		/* Now the IP address. */
		
		printf("%15s", ntoa(&peer->srcadr));
		
		/* Now the reference clock ID. */
		
		if (peer->stratum <= 1) { /* The refclock is ASCII */
		    form_refclock_ascii((char *) &peer->refid, local_string);
		    printf("  %15s", local_string);
		} else {
		    printf("  %15i", peer->refid);
		}
		
		/* Stratum. */
		
		printf("  %-2d", peer->stratum);
		
		/* Time since last update. */
		
		if (peer->timereceived - peer->timereset == 0) {
		    printf("     -");
		} else {
		    time_since_rcv = current_time - peer->timereceived;
		    if (time_since_rcv <= 2 * (1 << NTP_MAXPOLL)) {
			printf("  %-4d", time_since_rcv);
		    } else if (time_since_rcv < SECSPERHR) {
			printf("   %-2dm", time_since_rcv / SECSPERMIN);
		    } else if (time_since_rcv < 10 * SECSPERHR) {
			printf("  %dh%02d", time_since_rcv / SECSPERHR,
			       (time_since_rcv % SECSPERHR) / SECSPERMIN);
		    } else if (time_since_rcv < SECSPERDAY) {
			printf("   %-2dh", time_since_rcv / SECSPERHR);
		    } else {
			printf("  %-3dd", time_since_rcv / SECSPERDAY);
		    }
		}
		
		/* Poll interval. */
		
		printf("  %-4d", 1 << peer->hpoll);

		/* Reachability. */

		printf("  %-3o", peer->reach);

		/* Delay. */

		printf("  %-6s", fptoms(peer->delay, 1));

		/* Offset. */

		printf("  %-6s", lfptoms(&peer->offset, 2));

		/* Dispersion. */

		printf("  %-6s", ufptoms(peer->dispersion, 1));
	    }
	}
	printf("\n * master (synced), # master (unsynced), + selected, - candidate, ~ configured");
	automore_disable();

    } else {
	
	/* Detailed: */
	
	automore_enable(NULL);
	for (hash = 0; hash < HASH_SIZE; hash++) {
	    for (peer = peer_hash[hash]; peer; peer = peer->next) {
		
		/* Skip the broadcast peers. */
		
		if (peer->hmode == MODE_BROADCAST)
		    continue;

		/* IP address. */
		
		printf("\n%s", ntoa(&peer->srcadr));
		
		/* Peer status */

		if (peer->flags & FLAG_CONFIG)
		    printf(" configured");
		else
		    printf(" dynamic");

		if (peer->flags & FLAG_AUTHENTIC)
		    printf(", authenticated");

		if (peer == sys_peer) {
		    printf(", our_master");
		} else if (peer->select) {
		    printf(", selected");
		} else if (peer->candidate) {
		    printf(", candidate");
		}

		if (peer->was_sane)
		    printf(", sane");
		else
		    printf(", insane");

		if (peer->correct)
		    printf(", valid");
		else
		    printf(", invalid");

		switch (peer->leap) {
		case LEAP_ADDSECOND:
		    printf(", leap_add");
		    break;
		case LEAP_DELSECOND:
		    printf(", leap_sub");
		    break;
		case LEAP_NOTINSYNC:
		    printf(", unsynced");
		    break;
		default:
		    break;
		}

		printf(", stratum %d", peer->stratum);
		
		/* Now the reference clock info. */
		
		printf("\nref ID");
		if (peer->stratum <= 1) { /* The refclock is ASCII */
		    form_refclock_ascii((char *) &peer->refid, local_string);
		    printf(" %s", local_string);
		} else {
		    printf(" %i", peer->refid);
		}

		printf(", time %08x.%08x (%CC)", peer->reftime.l_ui,
		       peer->reftime.l_uf, "%B", &peer->reftime);

		printf("\nour mode %s, peer mode %s", modestring[peer->hmode],
		       modestring[peer->pmode]);
		printf(", our poll intvl %d, peer poll intvl %d",
		       1 << peer->hpoll, 1<< peer->ppoll);

		printf("\nroot delay %s msec, root disp %s", 
		       fptoms(peer->rootdelay, 2),
		       ufptoms(peer->rootdispersion, 2));
		printf(", reach %o, sync dist %s", peer->reach, 
		       fptoms(peer->synch, 3));

		printf("\ndelay %s msec, offset %s msec, dispersion %s", 
		       fptoms(peer->delay,2), lfptoms(&peer->offset, 4),
		       ufptoms(peer->dispersion, 2));
		printf("\nprecision 2**%d, version %d", -peer->precision, 
		       peer->version);
		printf("\n%s time %08x.%08x (%CC)", "org", peer->org.l_ui, 
		       peer->org.l_uf, "%B", &peer->org);
		printf("\n%s time %08x.%08x (%CC)", "rcv", peer->rec.l_ui, 
		       peer->rec.l_uf, "%B", &peer->rec);
		printf("\n%s time %08x.%08x (%CC)", "xmt", peer->xmt.l_ui, 
		       peer->xmt.l_uf, "%B", &peer->xmt);
		printf("\nfiltdelay = ");
		index = peer->filter_nextpt;
		for (i=0; i<NTP_SHIFT; i++) {
		    index = (index - 1) % NTP_SHIFT;
		    printf(" %-7s", fptoms(peer->filter_delay[index], 2));
		}
		printf("\nfiltoffset =");
		index = peer->filter_nextpt;
		for (i=0; i<NTP_SHIFT; i++) {
		    index = (index - 1) % NTP_SHIFT;
		    printf(" %-7s", lfptoms(&peer->filter_offset[index], 2));
		}
		printf("\nfilterror = ");
		index = peer->filter_nextpt;
		for (i=0; i<NTP_SHIFT; i++) {
		    index = (index - 1) % NTP_SHIFT;
		    printf(" %-7s", ufptoms(peer->filter_error[index], 2));
		}
		if (peer->flags & FLAG_REFCLOCK) {
		    refclock = peer->procptr;
		    if (refclock) {
			printf("\nReference clock status:  %s",
			       reg_invoke_ntp_refclock_status(refclock->currentstatus));
			printf("\nTimecode: %s", refclock->lastcode);
		    }
		}

		printf("\n");
		automore_conditional(0);
	    }
	}
	automore_disable();
    }		
}

#define NUM_FRAC_DIGITS 4	/* Number of digits after decimal point */
/*
 * Print an increment value as a frequency
 */
static void print_freq(u_int incr)
{
    u_int hz, hz_rmd;
    int i;

    hz = NTP_RESOLUTION / (incr >> NTP_SHIFT_FACTOR);
    hz_rmd = NTP_RESOLUTION % (incr >> NTP_SHIFT_FACTOR);
    printf("%d.", hz);
    for (i=0; i<NUM_FRAC_DIGITS; i++) {

	/* Round off the last digit... */

	hz = ((hz_rmd * 10) + (i == NUM_FRAC_DIGITS-1 ? 5 : 0)) / 
	      (incr >> NTP_SHIFT_FACTOR);
	hz_rmd = (hz_rmd * 10) % (incr >> NTP_SHIFT_FACTOR);
	printf("%d", hz);
    }
}

/*
 * show ntp status
 */
void show_ntp_status(parseinfo *csb)
{
    char local_string[7];
    ulong dummy, precision, incr, basic_incr;
    clock_epoch leap_epoch;
    int leap_offset;

    if (!ntp_running)
	return;

    printf("\nClock is ");
    switch(sys_leap) {
    case LEAP_NOWARNING:
	printf("synchronized");
	break;
    case LEAP_ADDSECOND:
	printf("synchronized (adding leap second)");
	break;
    case LEAP_DELSECOND:
	printf("synchronized (subtracting leap second)");
	break;
    default:
	printf("unsynchronized");
	break;
    }
    printf(", stratum %d, ", sys_stratum);
    if (sys_peer) {
	printf("reference is ");
	if (sys_stratum == 1) { /* The refclock is ASCII */
	    form_refclock_ascii((char *) &sys_refid, local_string);
	    printf("%s", local_string);
	} else {
	    printf("%i", sys_refid);
	}
    } else {
	printf("no reference clock");
    }
    get_clock_params(&dummy, &dummy, &precision, &incr, &basic_incr, 
			 &dummy);
    printf("\nnominal freq is ");
    print_freq(basic_incr);
    printf(" Hz, actual freq is ");
    print_freq(incr);
    printf(" Hz, precision is 2**%d", precision);
    printf("\nreference time is %08x.%08x (%CC)", sys_reftime.l_ui, 
	   sys_reftime.l_uf, "%B", &sys_reftime);
    printf("\nclock offset is %s msec, root delay is %s msec", 
	   lfptoms(&sys_offset, 4), fptoms(sys_rootdelay, 2));
    printf("\nroot dispersion is %s msec, peer dispersion is %s msec",
	   ufptoms(sys_rootdispersion, 2), ufptoms(sys_maxd, 2));
    leap_epoch.epoch_secs = clock_get_leap_second(&leap_offset);
    if (leap_epoch.epoch_secs) {
	leap_epoch.epoch_frac = 0;
	printf("\nLeap second occurs at %08x.%08x (%CC)", leap_epoch.epoch_secs,
	       leap_epoch.epoch_frac, "%B", &leap_epoch);
    }
}
