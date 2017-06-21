/* $Id: pgen_snif.c,v 3.4.22.1 1996/03/18 21:35:25 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/pgen/pgen_snif.c,v $
 *------------------------------------------------------------------
 * P G E N _ S N I F . C
 *
 * Code to convert a Sniffer file in a RAM buffer into packets in
 * the packet generator workspace.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pgen_snif.c,v $
 * Revision 3.4.22.1  1996/03/18  21:35:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/03/07  10:41:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/02/20  16:58:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/01/23  11:55:44  mdb
 * CSCdi47339:  apply code review comments for CSCdi47065
 *
 * Revision 3.3  1996/01/22  06:58:41  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:56:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:59:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/14  01:52:14  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.2  1995/06/09  13:18:10  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:34:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <stdlib.h>		/* for abs() */
#include <ciscolib.h>
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "pgen_private.h"
#include "sniffer.h"		/* Sniffer data structure declarations */
#include "mpilib.h"		/* Multiple-precision arithmetic (for times) */

/* Conversion tables from various Network General code values into useful
 * data.
 */
static char *sn_netnames[] = SN_NETNAMES; /* Names for network types */
static byte sn_defunits[] = SN_DEFUNITS;  /* Default time unit codes by type */
static int sn_t_divisors[] = SN_T_DIVISORS; /* Divisors for unit conversion */
static int sn_t_multipliers[] = SN_T_MULTIPLIERS; /* Multipliers ditto */

/* pgen_sniftows appends the files in a Sniffer data file to the packet
 * generator workspace. The new packets are added at the end of the
 * workspace. No cleanup is done after errors, so the workspace will be
 * modified if the error is encountered after the first frame.
 */
void pgen_sniftows (
    pgen_context *context,	/* Packet generation context */
    char *buffer,		/* Sniffer data to load */
    unsigned size)		/* Length of Sniffer data file */
{
    struct sn_rec *currec;	/* Pointer to record being processed */
    int recno;

    byte timeunit = TIMEUNIT_PC;	/* Time unit code */
    byte nettype = NETWORK_UNKNOWN;     /* Type code for network */

    paktype *pak;		/* Packet being added to workspace */
    unsigned paksize;		/* Size of the packet */
    unsigned inpsize;		/* Size of packet prefix actually in file */
    uchar baseyear;		/* Year of capture start */
    uchar basemonth;		/* Month of capture start */
    uchar baseday;		/* Day of capture start */
    uchar basehour;		/* Hour of capture start */
    uchar baseminute;		/* Minute of capture start */
    uchar basesecond;		/* Second of capture start */
    unit basetime[4];		/* Multiple-precision: Sniffer start time */
    unit rectime[4];		/* Multiple-precision: Time for this record */
    unit mptemp1[4];		/* Multiple-precision: general temporary */
    unit mptemp2[4];		/* Multiple-precision: general temporary */
    unit mptemp3[4];		/* Multiple-precision: general temporary */

    /* Set up multiple-precision math library */
    set_precision (4);		/* 16-bit units */

    /* Check the magic number to make sure this is a Sniffer file */
    if ((size < SN_MAGIC_LEN) ||
	(bcmp (buffer, SN_MAGIC_DATA, SN_MAGIC_LEN) != 0)) {
	printf ("\nFile doesn't look like a Sniffer file- wrong magic number");
	return;
    }

    /* Now, loop over all the data records */
    currec = (struct sn_rec *) (buffer + SN_MAGIC_LEN);
    recno = 1;
    while (currec < ((struct sn_rec *) (buffer + size))) {

	switch (LTLEWTOINT(currec->hdr.type)) {

        /* Version record. Tells us what kind of LAN and what time units
	 * are. 
	 */
	case REC_VERS:

	    /* Find out what type of net the data came from */
	    nettype = currec->u.vers.network;
	    if (nettype >= SN_MAXNETWORK) 
		nettype = NETWORK_UNKNOWN;

	    /* Find out how long the time tick is */
	    timeunit = currec->u.vers.timeunit;
	    if (timeunit == TIMEUNIT_UNSPEC)
		timeunit = sn_defunits[nettype];

	    /* Extract the capture start time */
	    baseyear =
		(LTLEWTOINT(currec->u.vers.date.pct_date) >> YEARSHIFT) &
		    YEARMASK;
	    basemonth =
		(LTLEWTOINT(currec->u.vers.date.pct_date) >> MONSHIFT) &
		    MONMASK;
	    baseday =
		(LTLEWTOINT(currec->u.vers.date.pct_date) >> DAYSHIFT) &
		    DAYMASK;
	    basehour =
		(LTLEWTOINT(currec->u.vers.date.pct_time) >> HOURSHIFT) &
		    HOURMASK;
	    baseminute =
		(LTLEWTOINT(currec->u.vers.date.pct_time) >> MINSHIFT) &
		    MINMASK;
	    basesecond =
		(LTLEWTOINT(currec->u.vers.date.pct_time) >> SECSHIFT) &
		    SECMASK;

	    /* Tell the user stuff */
	    printf ("\nVersion record at record %d; Sniffer version %d.%02d."
		    "\nFile captured from type %d (%s) network "
		    "on %d/%d/%d at %d:%02d:%02d",
		    recno, LTLEWTOINT(currec->u.vers.maj_vers),
		    LTLEWTOINT(currec->u.vers.min_vers),
		    currec->u.vers.network, sn_netnames[nettype],
		    basemonth, baseday, baseyear + 80,
		    basehour, baseminute, basesecond);

	    /* Create the multiple-precision quantity for the time at
	     * which the trace started, relative to the beginning of the
	     * current day, in this file's time units. Start by computing
	     * the number of seconds, then multiply by 1000000 to get
	     * microseconds.
	     */
	    mp_initl(basetime,
		     (((basehour * 60) + baseminute) * 60) + basesecond);
	    mp_initl(mptemp1, 1000000);
	    mp_mult(mptemp2, basetime, mptemp1);
	    bcopy (mptemp2, basetime, sizeof(basetime));

	    /* Now convert from microseconds to media-dependent units.
	     * This is the inverse of converting from media units to
	     * microseconds, so we multiply by the "divisor" and divide
	     * by the "multiplier".
	     */
	    mp_initl(mptemp1, sn_t_divisors[timeunit]);
	    mp_mult(mptemp2, basetime, mptemp1);
	    mp_initl(mptemp1, sn_t_multipliers[timeunit]);
	    mp_div(mptemp3, basetime, mptemp2, mptemp1);

	    break;

        /* Frame record. The actual data */
	case  REC_FRAME2:

/* The following code doesn't work because, despite what their documents
 * say, Network General leaves nonzero garbage data in the true_size fields
 * of untruncated frames. We just assume all frames are untruncated and
 * use the length in the file.
 */
#ifdef NOTDEF
	    /* Figure out how large the packet will ultimately be, and how
	     * much of it the Sniffer file actually contains.
	     */
	    paksize = LTLEWTOINT(currec->u.frame.true_size);
	    inpsize = LTLEWTOINT(currec->u.frame.size);
	    if (paksize == 0) {
		paksize = inpsize;
	    }
#endif
	    inpsize = LTLEWTOINT(currec->u.frame.size);
	    paksize = inpsize;

	    /* Start by getting a buffer in the workspace for the frame */
	    pak = pgen_wsnew (&(context->workspace),
			      pgen_wscount (&(context->workspace)) + 1,
			      paksize);
	    if (pak == NULL) {
		printf ("Can't get packet buffer at record %d; aborting load",
			recno);
		return;
	    }

	    /* Fill in all the fields */
	    pak->datagramsize = paksize;
	    pak->if_output = context->interface;
	    pak->pgen_output = context->interface;
	    pak->pgen_rptcount = 1;
	    pak->pgen_nameptr = NULL;

	    /* The time to send is a little complicated. We have to do
	     * ugly things with the 5-byte value from the Sniffer,
	     * plus there's an offset to add. Note that the initialization
	     * here depends on multiprecision units being 16 bits.
	     */
	    mp_init0(rectime);
	    rectime[sizeof(rectime)/sizeof(unit) - 1] =
		LTLEWTOINT(currec->u.frame.time_low);
	    rectime[sizeof(rectime)/sizeof(unit) - 2] =
		LTLEWTOINT(currec->u.frame.time_mid);
	    rectime[sizeof(rectime)/sizeof(unit) - 3] =
		currec->u.frame.time_high;

	    /* Subtract the base time to get the time relative to beginning
	     * of capture.
	     */
	    mp_sub (rectime, basetime);

	    /* Convert to microseconds by multiplying then dividing */
	    mp_initl (mptemp1, sn_t_multipliers[timeunit]);
	    mp_mult (mptemp2, rectime, mptemp1);
	    mp_initl (mptemp1, sn_t_divisors[timeunit]);
	    mp_div (mptemp3, rectime, mptemp2, mptemp1);

	    /* Apply the offset */
	    mp_initl (mptemp1, abs(context->timeoffs));
	    if (context->timeoffs < 0) mp_neg(mptemp1);
	    mp_add (rectime, mptemp1);

	    /* Check for overflow. If no overflow, extract value. */
	    if (significance(rectime) >
		(sizeof(pak->pgen_ttosend)/sizeof(unit))) {
		printf ("\nTime-to-send overflow at record %d", recno);
		pak->pgen_ttosend = -1;
	    } else {
		bcopy (((uchar *) rectime) +
		       (sizeof(rectime) - sizeof (pak->pgen_ttosend)),
		       &(pak->pgen_ttosend),
		       sizeof (pak->pgen_ttosend));
	    }

	    /* Copy the data into the packet. If the data are truncated
	     * in the file, only copy what we have.
	     */
	    bcopy (&(currec->u.frame.framedata[0]), pak->datagramstart,
		   min(paksize, inpsize));
	    /* If the packet was truncated, fill the rest with zeros */
	    if (inpsize < paksize)
		memset(pak->datagramstart + paksize, 0, paksize - inpsize);

	    break;

	/* EOF record. We'll support catenated files, so this isn't really
	 * necessarily EOF, but we can't trust the stuff we learned from
	 * the last version record after this.
	 */
	case REC_EOF:

	    printf ("\nSniffer EOF record encountered at record %d", recno);
	    nettype = NETWORK_UNKNOWN;
	    timeunit = TIMEUNIT_PC;
	    break;

	/* Some kind of proprietary Network General garbage. Always appears
	 * at record 2 of the file. We just ignore them.
	 */
	case REC_BIGSECRET:

	    break;

	/* Something strange */
	default:

	    printf ("\nFound unknown Sniffer record type %d at record %d",
		    LTLEWTOINT(currec->hdr.type), recno);
	    break;
	}

	/* Find the next record */
	currec = (struct sn_rec *) ((uchar *) currec +
				    LTLEWTOINT(currec->hdr.length) +
				    sizeof (struct f_rec_struct));

	recno++;		/* Bump the counter */

	process_may_suspend();	/* Let other things run */
    }

    /* Check to make sure that the buffer really ended at the end of the
     * last record.
     */
    if (currec > ((struct sn_rec *) (buffer + size))) {
	printf ("\nSniffer record extended beyond EOF");
	return;
    }

    return;
}

