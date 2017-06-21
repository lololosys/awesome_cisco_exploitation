/* $Id: refclock_trimble.c,v 3.1.2.2 1996/08/20 03:14:03 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/refclock_trimble.c,v $
 *------------------------------------------------------------------
 * NTP Driver for the Trimble TSIP protocol
 *
 * April 1996, Dave Katz
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: refclock_trimble.c,v $
 * Revision 3.1.2.2  1996/08/20  03:14:03  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Code review comments.
 *
 * Revision 3.1.2.1  1996/08/19  18:55:34  dkatz
 * CSCdi65483:  NTP needs modularity cleanup
 * Branch: California_branch
 * Break up NTP into separate subsystems.  Significantly reduce image size
 * and stack usage.  Clean up refclock interface.  Make all modules
 * compile cleanly with all error checking enabled (DUSTY is clean!)
 *
 * Revision 3.1  1996/06/22  21:14:48  dkatz
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#define CISCO

#include "master.h"
#include "subsys.h"
#include <string.h>
#include <ciscolib.h>
#include "ttysrv.h"
#include "config.h"
#include "../os/location.h"
#include "ntpd.h"
#include "ntp_refclock.h"
#include "ctype.h"
#include "ntp_stdlib.h"
#include "ntp_fp.h"
#include "../os/clock.h"

/*
 * The TSIP protocol consists of records starting with DLE and ending
 * with DLE/ETX.  Any DLE in the data is doubled.
 *
 * We tell the driver to embed a timestamp whenever it sees a DLE or
 * an ETX.  This means that we will have multiple timestamps in the stream,
 * but it means that the drivers can remain ignorant of this strange protocol.
 */

/*
 * Interface definitions
 */
#define	PRECISION	(-20)	/* precision assumed (about 1 us) */
#define	REFID		"GPS\0"	/*  reference ID */
#define	DESCRIPTION	"Trimble Navigation GPS Receiver"

/* States for receive machine */

typedef enum {SYNC_STATE, SYNC_DLE_STATE, RCV_STATE, RCV_DLE_STATE} rcv_state;

/* GPS Time message */

#define TRIMBLE_ID_GPS_TIME 0x41
typedef struct trimble_gps_time_ {
    uchar id;				/* ID code */
    uchar time_of_week[4];		/* Single-precision time-of-week */
    uchar week_number[2];		/* Integer week number */
    uchar gps_utc_offset[4];		/* Single-precision GPS/UTC offset */
} trimble_gps_time;
#define TRIMBLE_GPS_TIME_LENGTH 11


/* Single-precision XYZ ECEF Fix */

#define TRIMBLE_ID_XYZ_FIX 0x42
typedef struct trimble_xyz_fix_ {
    uchar id;				/* ID code */
    uchar X[4];				/* Single-precision X coordinate */
    uchar Y[4];				/* Single-precision Y coordinate */
    uchar Z[4];				/* Single-precision Z coordinate */
    uchar time_of_fix[4];		/* Single-precision time of fix */
} trimble_xyz_fix;
#define TRIMBLE_XYZ_FIX_LENGTH 17
#define RADIANS_TO_CENTISECS_MANT 1320094759 /* Rads to centisecs mantissa */
#define RADIANS_TO_CENTISECS_EXP  -6	/* Rads to centisecs exponent */


/* Velocity fix, XYZ ECEF */

#define TRIMBLE_ID_XYZ_VELOCITY 0x43
typedef struct trimble_xyz_velocity_ {
    uchar id;				/* ID code */
    uchar X[4];				/* Single-precision X velocity */
    uchar Y[4];				/* Single-precision Y velocity */
    uchar Z[4];				/* Single-precision Z velocity */
    uchar bias_rate[4];			/* Bias rate */
    uchar time_of_fix[4];		/* Single-precision time of fix */
} trimble_xyz_velocity;
#define TRIMBLE_XYZ_VELOCITY_LENGTH 21


/* Receiver Health */

#define TRIMBLE_ID_RECEIVER_HEALTH 0x46
typedef struct trimble_receiver_health_ {
    uchar id;				/* ID code */
    uchar status;			/* Current status */
    uchar error_code;			/* Error code */
} trimble_receiver_health;
#define TRIMBLE_RECEIVER_HEALTH_LENGTH 3

#define RECEIVER_STATUS_OK  0x00	/* All is well */
#define RECEIVER_STATUS_NO_GPS_TIME 0x01 /* No GPS time yet */
#define RECEIVER_STATUS_HIGH_PDOP 0x03	 /* PDOP is too high */
#define RECEIVER_STATUS_NO_SATELLITE 0x08 /* No usable satellites */
#define RECEIVER_STATUS_1_SATELLITE 0x09 /* One satellite in sight */
#define RECEIVER_STATUS_2_SATELLITE 0x0A /* Two satellites */
#define RECEIVER_STATUS_3_SATELLITE 0x0B /* Three satellites */


/* System Message */

#define TRIMBLE_ID_SYSTEM_MESSAGE 0x48


/* Lat/Long/Alt fix */

#define TRIMBLE_ID_LLA_FIX 0x4A
typedef struct trimble_lla_fix_ {
    uchar id;				/* ID code */
    uchar latitude[4];			/* Latitude, radians, +=N */
    uchar longitude[4];			/* Longitude, radians, +=E */
    uchar altitude[4];			/* Altitude, meters */
    uchar clock_bias[4];		/* Clock bias */
    uchar time_of_fix[4];		/* Time of fix */
} trimble_lla_fix;
#define TRIMBLE_LLA_FIX_LENGTH 21


/* Double precision Lat/Long/Alt fix */

#define TRIMBLE_ID_DOUBLE_LLA_FIX 0x84
typedef struct trimble_double_lla_fix_ {
    uchar id;				/* ID code */
    uchar latitude[8];			/* Latitude, radians, +=N */
    uchar longitude[8];			/* Longitude, radians, +=E */
    uchar altitude[8];			/* Altitude, meters */
    uchar clock_bias[8];		/* Clock bias */
    uchar time_of_fix[4];		/* Time of fix */
} trimble_double_lla_fix;
#define TRIMBLE_DOUBLE_LLA_FIX_LENGTH 37


/* UTC Parameters */

#define TRIMBLE_ID_UTC_PARAMS 0x4F
typedef struct trimble_utc_params_ {
    uchar id;				/* ID code */
    uchar A0[8];			/* Mystery parameters */
    uchar A1[4];
    uchar deltTls[2];
    uchar Tot[4];
    uchar WNt[2];
    uchar WNlsf[2];			/* Leap second flag? */
    uchar DN[2];
    uchar delTlsf[2];
} trimble_utc_params;
#define TRIMBLE_UTC_PARAMS_LENGTH 27



/*
 * Unit control structure
 */
struct trimbleunit {
    queuetype reass_queue;		/* Reassembly queue */
    l_fp frame_rcv_time;		/* Start-of-frame receive time */
    rcv_state curstate;			/* Receive state */
    l_fp start_of_week;			/* Epoch of the start of the week */
};

#define TRIMBLE_DLE	0x10
#define TRIMBLE_ETX	0x03

/*
 * trimble_float_to_int
 *
 * Convert a floating point number to an integer
 */
static int trimble_float_to_int (int mantissa, int exponent)
{
    if (exponent >= 0) {
	return(mantissa << exponent);
    } else {
	return(mantissa >> (-exponent));
    }
}


/*
 * trimble_float_to_l_fp
 *
 * Convert floating point to l_fp.  The binary decimal point is assumed
 * to be in the low-order bit of the mantissa.
 */
static void trimble_float_to_l_fp (int mantissa, int exponent, l_fp *result)
{
    result->l_i = trimble_float_to_int(mantissa, exponent);
    if (exponent >= 0) {
	result->l_f = 0;
    } else {
	result->l_f = mantissa << (32 + exponent);
    }
}


/*
 * trimble_float_multiply
 *
 * Multiply two floating point numbers;  returns the third.
 *
 * The numbers are expressed as mantissa/exponent pairs, with the binary
 * decimal point all the way to the *right* (so the mantissas are expressed
 * as integers).
 */
static void trimble_float_multiply (int fact1man, int fact1exp,
				    int fact2man, int fact2exp,
				    int *prodman, int *prodexp)
{
    boolean fact1neg, fact2neg;
    ulong upper1man, lower1man, upper2man, lower2man;
    ulong upperprodman, middleprod1man, middleprod2man, middleprodman;
    ulong lowerprodman, lowermiddleprodman;;

    /* First, make everything positive. */

    fact1neg = FALSE;
    fact2neg = FALSE;
    if (fact1man < 0) {
	fact1man = -fact1man;
	fact1neg = TRUE;
    }
    if (fact2man < 0) {
	fact2man = -fact2man;
	fact2neg = TRUE;
    }

    /* Now break the mantissas up into 16 bit quantities. */

    upper1man = (fact1man >> 16) & 0xffff;
    lower1man = fact1man & 0xffff;
    upper2man = (fact2man >> 16) & 0xffff;
    lower2man = fact2man & 0xffff;

    /* Now we cross-multiply... (the multiplications cannot overflow) */

    upperprodman = upper1man * upper2man;
    lowerprodman = lower1man * lower2man;
    middleprod1man = upper1man * lower2man;
    middleprod2man = upper2man * lower1man;

    /* Now handle overflows. */

    *prodman = upperprodman;
    middleprodman = middleprod1man + middleprod2man;
    if (middleprodman < middleprod1man || middleprodman < middleprod2man)
	*prodman += 0x10000;		/* Overflowed the middle */
    lowermiddleprodman = middleprodman + ((lowerprodman >> 16) & 0xffff);
    if (lowermiddleprodman < middleprodman)
	*prodman += 0x10000;		/* Overflowed the middle again */
    *prodman += (lowermiddleprodman >> 16) & 0xffff;

    /* Change the sign if necessary. */

    if (fact1neg ^ fact2neg)
	*prodman = -(*prodman);

    /* Diddle the exponent and we're done. */

    *prodexp = fact1exp + fact2exp + 32;
}


/*
 * trimble_convert_single
 *
 * Convert a single_precision floating-point value to a mantissa and exponent
 * (with the binary decimal point in the low-order position).
 */
static void trimble_convert_single (uchar *bytes, int *mantissa, int *exponent)
{
    ulong value;
    ulong sign_bit;
    int local_exponent;

    /* Pick up the value. */

    value = GETLONG(bytes);
    local_exponent = ((value >> 23) & 0xFF) - 0x7F;
    sign_bit = value & 0x80000000;
    *mantissa = 0x40000000 | ((value & 0x7fffff) << 7);
    if (sign_bit)
	*mantissa = -(*mantissa);
    *exponent = local_exponent - 30;
}


/*
 * trimble_convert_double
 *
 * Convert a double_precision floating-point value to a mantissa and exponent
 * (with the binary decimal point in the low-order position).
 */
static void trimble_convert_double (uchar *bytes, int *mantissa, int *exponent)
{
    ulong value1, value2;
    ulong sign_bit;
    int local_exponent;

    /* Pick up the value. */

    value1 = GETLONG(bytes);
    value2 = GETLONG(bytes + 4);
    local_exponent = ((value1 >> 20) & 0x7FF) - 0x3FF;
    sign_bit = value1 & 0x80000000;
    *mantissa = 0x40000000 |
	((value1 & 0xfffff) << 10) |
	((value2 >> 22) & 0x3ff);
    if (sign_bit)
	*mantissa = -(*mantissa);
    *exponent = local_exponent - 30;
}


/*
 * trimble_flush_reass_queue
 *
 * Flush the reassembly queue.
 */
static void trimble_flush_reass_queue (struct trimbleunit *up)
{
    struct recvbuf *rbufp;

    while ((rbufp = dequeue(&up->reass_queue))) {
	free(rbufp);
    }
}


/*
 * trimble_shutdown - shut down the clock
 */
static void trimble_shutdown (int unit, struct peer *peer)
{
    struct trimbleunit *up;
    struct refclockproc *pp;
    
    pp = peer->procptr;
    up = pp->unitptr;
    trimble_flush_reass_queue(up);
    free(up);
}


/*
 * trimble_process_location
 *
 * Process a lat/long/alt message.
 */
static void trimble_process_location (struct peer *peer,
				      struct refclockproc *pp,
				      struct trimbleunit *up,
				      uchar *buffer, ulong length,
				      boolean double_prec)
{
    trimble_lla_fix *lla_fix;
    trimble_double_lla_fix *double_lla_fix;
    int latitude_radians_man, latitude_radians_exp;
    int longitude_radians_man, longitude_radians_exp;
    int latitude_man, latitude_exp;
    int longitude_man, longitude_exp;
    int altitude_man, altitude_exp;

    if (double_prec) {
	double_lla_fix = (trimble_double_lla_fix *) buffer;
	if (length != TRIMBLE_DOUBLE_LLA_FIX_LENGTH)
	    return;
	trimble_convert_double(double_lla_fix->latitude, &latitude_radians_man,
			       &latitude_radians_exp);
	trimble_convert_double(double_lla_fix->longitude,
			       &longitude_radians_man, &longitude_radians_exp);
	trimble_convert_double(double_lla_fix->altitude, &altitude_man,
			       &altitude_exp);
    } else {
	lla_fix = (trimble_lla_fix *) buffer;
	if (length != TRIMBLE_LLA_FIX_LENGTH)
	    return;
	trimble_convert_single(lla_fix->latitude, &latitude_radians_man,
			       &latitude_radians_exp);
	trimble_convert_single(lla_fix->longitude, &longitude_radians_man,
			       &longitude_radians_exp);
	trimble_convert_single(lla_fix->altitude, &altitude_man,
			       &altitude_exp);
    }

    /* We've got the lat/long values in radians.  Convert to centiseconds. */

    trimble_float_multiply(latitude_radians_man, latitude_radians_exp,
			   RADIANS_TO_CENTISECS_MANT, RADIANS_TO_CENTISECS_EXP,
			   &latitude_man, &latitude_exp);
    trimble_float_multiply(longitude_radians_man, longitude_radians_exp,
			   RADIANS_TO_CENTISECS_MANT, RADIANS_TO_CENTISECS_EXP,
			   &longitude_man, &longitude_exp);
			   
    /* Reduce them to integers. */

    pp->latitude = trimble_float_to_int(latitude_man, latitude_exp);
    pp->longitude = trimble_float_to_int(longitude_man, longitude_exp);
    pp->altitude = (trimble_float_to_int(altitude_man, altitude_exp) *
		    METERS_TO_FEET_NUMERATOR) / METERS_TO_FEET_DENOMINATOR;
    if (pp->leap != LEAP_NOTINSYNC) {
	location_set(pp->longitude, pp->latitude, pp->altitude);
    }
}


/*
 * trimble_process_gps_time
 *
 * Process a GPS time message.
 */
static void trimble_process_gps_time (struct peer *peer,
				      struct refclockproc *pp,
				      struct trimbleunit *up,
				      uchar *buffer, ulong length)
{
    ushort week_number;
    trimble_gps_time *gps_time;
    int mantissa, exponent;
    l_fp converted_time;

    buginf("\nGPS TIME:");
    if (length != TRIMBLE_GPS_TIME_LENGTH) {
	buginf("(bad length %d)", length);
    } else {
	gps_time = (trimble_gps_time *) buffer;
	week_number = GETSHORT(gps_time->week_number);
	up->start_of_week.l_ui = JAN_6_1980 +
	    (week_number * SECSPERWEEK);
	up->start_of_week.l_uf = 0;
	buginf(" week number %d", week_number);
	trimble_convert_single(gps_time->gps_utc_offset, &mantissa, &exponent);
	trimble_float_to_l_fp(mantissa, exponent, &converted_time);
	buginf(", GPS/UTC offset %08x.%08x",  converted_time.l_ui,
	       converted_time.l_uf);
	L_SUB(&up->start_of_week, &converted_time);
	trimble_convert_single(gps_time->time_of_week, &mantissa, &exponent);
	trimble_float_to_l_fp(mantissa, exponent, &converted_time);
	buginf(", time-of-week %08x.%08x", converted_time.l_ui,
	       converted_time.l_uf);
	L_ADD(&converted_time, &up->start_of_week);
	buginf(", rcvd time %C, local time %C", "%B", &converted_time,
	       "%B", &up->frame_rcv_time);
    }

    /* If we're not polled, bail. */
    
    if (pp->pollcnt == NOPOLLS)
	return;
    pp->pollcnt = NOPOLLS;		/* We just responded. */

    pp->leap = LEAP_NOWARNING;		/* %%% */
    pp->lastrec = up->frame_rcv_time;

    /*
     * Process the new sample in the median filter and determine the
     * reference clock offset and dispersion.  This will fill in lastref.
     */
    if (!refclock_new_sample(pp, NSAMPLES, NSAMPLES, &converted_time)) {
	refclock_report(peer, CEVNT_BADTIME);
	return;
    }
    
    if (ntp_refclock_debug) {
	buginf("\nOffset %s, disp %s, reftime %08x.%08x (%C), leap %d",
	       lfptoa(&pp->offset, 6), ufptoa(pp->dispersion, 6),
	       pp->lastref.l_ui, pp->lastref.l_uf, "%B", &pp->lastref,
	       pp->leap);
    }
    
    refclock_receive(peer, &pp->offset, 0, pp->dispersion, &pp->lastref,
		     &pp->lastrec, pp->leap);
    
    /* If the data is no good, whimper. */
    
    if (pp->leap == LEAP_NOTINSYNC) {
	refclock_report(peer, CEVNT_SYNC); /* It's out of sync */
    }
}


/*
 * trimble_process_message
 *
 * Process a complete message.  The starting timestamp is in the
 * unit structure.  The message itself is spread across a series of
 * buffers enqueued on the reassembly queue.
 */
static void trimble_process_message (struct peer *peer,
				     struct refclockproc *pp,
				     struct trimbleunit *up)
{
    struct recvbuf *rbufp;
    ulong total_length;
    uchar *msg_buffer;
    uchar *cur_msg_ptr;
    trimble_receiver_health *receiver_health;

    /* First run through the queue to determine the total message length. */

    rbufp = up->reass_queue.qhead;
    total_length = 0;
    while (rbufp) {

	/* Delete the DLE from the end of the last buffer. */

	if (!rbufp->next && rbufp->recv_length)
	    rbufp->recv_length--;
	total_length += rbufp->recv_length;
	rbufp = rbufp->next;
    }

    /* Copy the pieces to a single buffer to make processing easier. */

    if (total_length) {
	msg_buffer = malloc(total_length);
	cur_msg_ptr = msg_buffer;
	while ((rbufp = dequeue(&up->reass_queue))) {
	    if (msg_buffer) {		/* Did we get memory? */
		memcpy(cur_msg_ptr, &rbufp->recv_buffer, rbufp->recv_length);
		cur_msg_ptr += rbufp->recv_length;
	    }
	    free(rbufp);
	}

	/* Process according to type. */

	switch (msg_buffer[0]) {
	  case TRIMBLE_ID_GPS_TIME:
	    trimble_process_gps_time(peer, pp, up, msg_buffer, total_length);
	    break;

	  case TRIMBLE_ID_LLA_FIX:
	    trimble_process_location(peer, pp, up, msg_buffer, total_length,
				     FALSE);
	    break;

	  case TRIMBLE_ID_DOUBLE_LLA_FIX:
	    trimble_process_location(peer, pp, up, msg_buffer, total_length,
				     TRUE);
	    break;

	  case TRIMBLE_ID_XYZ_FIX:
	    break;

	  case TRIMBLE_ID_XYZ_VELOCITY:
	    break;


	  case TRIMBLE_ID_RECEIVER_HEALTH:
	    buginf("\nRECEIVER_HEALTH: ");
	    if (total_length != TRIMBLE_RECEIVER_HEALTH_LENGTH) {
		buginf("(bad length %d)", total_length);
	    } else {
		receiver_health = (trimble_receiver_health *) msg_buffer;
		switch (receiver_health->status) {
		  case RECEIVER_STATUS_OK:
		    buginf("OK");
		    break;
		  case RECEIVER_STATUS_NO_GPS_TIME:
		    buginf("NO GPS TIME");
		    break;
		  case RECEIVER_STATUS_HIGH_PDOP:
		    buginf("HIGH PDOP");
		    break;
		  case RECEIVER_STATUS_NO_SATELLITE:
		    buginf("NO USABLE SATELLITES");
		    break;
		  case RECEIVER_STATUS_1_SATELLITE:
		    buginf("1 satellite");
		    break;
		  case RECEIVER_STATUS_2_SATELLITE:
		    buginf("2 satellites");
		    break;
		  case RECEIVER_STATUS_3_SATELLITE:
		    buginf("3 satellites");
		    break;
		}
	    }
	    break;

	  case TRIMBLE_ID_SYSTEM_MESSAGE:
	    buginf("\n System message: \"%s\"", &msg_buffer[1]);
	    break;


	  default:
	    buginf("\n");
	    cur_msg_ptr = msg_buffer;
	    while (total_length--) {
		buginf("%02x ", *cur_msg_ptr);
		cur_msg_ptr++;
	    }
	}
    }
}

/*
 * trimble_receive - receive data from the serial interface
 *
 * We implement a funny state machine here, since we need to detect the
 * start and end of each frame, and remove stuffing, and the like.
 *
 * The driver is set up to deliver a new buffer whenever a DLE or ETX
 * is hit;  these bytes are guaranteed to be the last characters in
 * the buffer.
 *
 * We save interesting buffers on the reass_queue as they come in, until
 * the frame has been built.  The initial DLE is always discarded, so the
 * frame ID is always the first byte of the first buffer on the queue.
 * The trailing DLE is left in the final buffer, but the ETX is removed.
 */
static struct recvbuf *trimble_receive (struct recvbuf *rbufp)
{
    struct trimbleunit *up;
    struct refclockproc *pp;
    struct peer *peer;
    uchar *next_byte;
    int byte_count;
    boolean save_buffer;
    
    /*
     * Initialize pointers and read the receiver response
     */
    peer = rbufp->recv_srcclock;
    pp = peer->procptr;
    up = pp->unitptr;
    byte_count = rbufp->recv_length;
    next_byte = rbufp->recv_buffer;
    save_buffer = FALSE;

    /* Process the contents. */

    while (byte_count > 0) {
	switch (up->curstate) {

	    /*
	     * Sync state.  If we see a DLE, save the timestamp on it,
	     * since it might be the start of a frame.  (We discard the
	     * buffer itself, however.)
	     */

	  case SYNC_STATE:
	    if (*next_byte == TRIMBLE_DLE) {
		up->curstate = SYNC_DLE_STATE;
		up->frame_rcv_time = rbufp->recv_time;
	    }
	    break;


	    /*
	     * Saw a DLE.  If the next byte is anything other than a
	     * DLE or ETX, we've hit the start of a frame.
	     */

	  case SYNC_DLE_STATE:
	    if (*next_byte != TRIMBLE_DLE && *next_byte != TRIMBLE_ETX) {
		up->curstate = RCV_STATE;
		save_buffer = TRUE;
	    }
	    break;


	    /*
	     * Receiving a frame.  If the next byte is a DLE, we may be
	     * at the end of the frame, or we may be unstuffing.  If so,
	     * shift states until we know.
	     */

	  case RCV_STATE:
	    if (*next_byte == TRIMBLE_DLE) {
		up->curstate = RCV_DLE_STATE;
	    }
	    save_buffer = TRUE;
	    break;


	    /*
	     * Received a DLE.  If the next byte is another DLE, delete
	     * it from the buffer.  We know that the DLE is the only byte
	     * in the buffer, since the NTP async driver guarantees this,
	     * so we simply discard the buffer.
	     *
	     * If the next byte is an ETX, we've hit the end of a frame.
	     * Anything else indicates a protocol violation.
	     */

	  case RCV_DLE_STATE:
	    switch (*next_byte) {

	      case TRIMBLE_DLE:
		break;

	      case TRIMBLE_ETX:
		trimble_process_message(peer, pp, up);
		up->curstate = SYNC_STATE;
		break;

	      default:
		up->curstate = SYNC_STATE;
		trimble_flush_reass_queue(up);
		break;
	    }
	}
	next_byte++;
	byte_count--;
    }

    /*
     * If we are saving the buffer, stick it in the queue and NULL out the
     * pointer so that the I/O driver doesn't free it for us.
     */

    if (save_buffer) {
	enqueue(&up->reass_queue, rbufp);
	rbufp = NULL;
    }
    return(rbufp);
}

/*
 * KLUDGE!!!
 *
 * clock_send_str_exec
 *
 * Send a string of characters to the clock.
 *
 * Assumes all characters are hex.
 */
void clock_send_str_exec (parseinfo *csb)
{
    char *curptr;
    int cur_ix;
    int length;
    uchar byte;
    tt_soc *tty;

    tty = MODEMS[AuxBase];		/* Yuck */
    length = strlen(GETOBJ(string,1));
    curptr = GETOBJ(string,1);
    for (cur_ix = 0; cur_ix < length; cur_ix += 2) {
	byte = chartohex(*curptr) << 4;
	curptr++;
	byte |= chartohex(*curptr);
	curptr++;
	if (outputblock(tty)) {
	    printf("\nOutput blocked");
	    return;
	}
	putc(tty, byte);
    }
    startoutput(tty);
}


/*
 * 
 * trimble_send
 *
 * Send a string of characters to the clock.
 *
 * Does not tolerate blocking.
 */
static void trimble_send(struct peer *peer, uchar *buffer, ulong length)
{
    int cur_ix;
    tt_soc *tty;

    tty = MODEMS[peer->refclkunit];
    for (cur_ix = 0; cur_ix < length; cur_ix ++) {
	if (outputblock(tty)) {
	    return;
	}
	putc(tty, *buffer++);
    }
    startoutput(tty);
}
    

/*
 * trimble_poll - called by the transmit procedure
 */
static uchar poll_message[] = {0x10, 0x21, 0x10, 0x03};
static void trimble_poll(int unit, struct peer *peer)
{
    struct refclockproc *pp;

    /*
     * Decrement the poll counter to show that we have a poll
     * outstanding.
     */
    pp = peer->procptr;
    pp = pp->unitptr;
    if (pp->pollcnt == 0)
	refclock_report(peer, CEVNT_TIMEOUT);
    else
	pp->pollcnt--;
    pp->polls++;

    /* Transmit the poll message. */

    trimble_send(peer, poll_message, sizeof(poll_message));
}

/*
 * trimble_start - open the devices and initialize data for processing
 */
static int trimble_start (int unit, struct peer *peer)
{
    struct trimbleunit *up;
    struct refclockproc *pp;
    
    /*
     * Allocate and initialize unit structure
     */
    up = malloc(sizeof(struct trimbleunit));
    if (!up)
	return(0);
    pp = peer->procptr;
    pp->io.clock_recv = trimble_receive;
    pp->io.srcclock = peer;
    pp->unitptr = up;
    
    /*
     * Initialize miscellaneous variables
     */
    peer->precision = PRECISION;
    pp->clockdesc = DESCRIPTION;
    memcpy(&pp->refid, REFID, 4);
    
    pp->pollcnt = NOPOLLS;
    up->curstate = SYNC_STATE;
    return (1);
}


/*
 * trimble_setup
 *
 * Set up TTY parameters and such for this new reference clock.
 */
static void trimble_setup (struct peer *peer)
{
    tt_soc *tty;
    struct refclockproc *pp;
    struct trimbleunit *up;

    pp = peer->procptr;
    up = pp->unitptr;
    tty = MODEMS[peer->refclkunit];

    tty->capabilities2 |= NTP_CLOCK_2;
    tty->ts_char_1 = TRIMBLE_DLE;
    tty->ts_char_2 = TRIMBLE_ETX;

    /*
     * We set things up differently depending on whether we are looking for
     * a PPS signal or not.
     */
    if (peer->pps_flags) {		/* PPS requested */
	peer->ttysetupparm = NTP_SETUP_ENABLETIMEPPS;
	tty->capabilities2 |= PPS_CLOCK_2;
    } else {				/* No PPS */
	peer->ttysetupparm = NTP_SETUP_ENABLETIME;
    }
    tty->capabilities |= TRANSPARENT;
    tty->tty_holdchar = -1;
    tty->tty_escapechar = BREAK_CHAR;
    free(tty->current.padding_ptr);
    tty->current.padding_ptr = NULL;

    setspeed(tty, 9600, 9600);
    setparity(tty, PARITY_ODD);
    setstopbits(tty, 1);
    setdatabits(tty, 8);
}


/*
 * Transfer vector
 */
static struct refclock refclock_trimble = {
    trimble_start,			/* start up driver */
    trimble_shutdown,			/* shut down driver */
    trimble_poll,			/* transmit poll message */
    NULL,				/* not used */
    NULL,				/* initialize driver */
    NULL,				/* not used */
    NOFLAGS,				/* not used */
    trimble_setup			/* Set up TTY parameters */
};


/*
 * ntp_refclock_trimble_init
 *
 * Called at subsystem init time to link in.
 */
static void ntp_refclock_trimble_init (subsystype *subsys)
{
    ntp_refclock_register(&refclock_trimble, REFCLK_TRIMBLE);
}


/*
 * NTP trimble refclock subsystem header
 */

#define NTP_REFCLOCK_TRIMBLE_MAJVERSION 1
#define NTP_REFCLOCK_TRIMBLE_MINVERSION 0
#define NTP_REFCLOCK_TRIMBLE_EDITVERSION  1

SUBSYS_HEADER(ntp_refclock_trimble, NTP_REFCLOCK_TRIMBLE_MAJVERSION,
	      NTP_REFCLOCK_TRIMBLE_MINVERSION,
	      NTP_REFCLOCK_TRIMBLE_EDITVERSION, ntp_refclock_trimble_init,
	      SUBSYS_CLASS_PROTOCOL, "req: ntp_refclock", "seq: ntp_refclock");
