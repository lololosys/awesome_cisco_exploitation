/* $Id: tcpdriver.h,v 3.4 1996/02/08 18:02:15 ahh Exp $
 * $Source: /release/111/cvs/Xsys/tcp/tcpdriver.h,v $
 *------------------------------------------------------------------
 * tcpdriver.h -- support for making TCP/IP an encapsulation
 *
 * February 1991, David Hampton
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcpdriver.h,v $
 * Revision 3.4  1996/02/08  18:02:15  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.3  1995/12/13  04:24:41  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.2  1995/11/17  18:48:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:32:09  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/17  06:26:37  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.1  1995/06/07  23:05:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TCPDRIVER_H__
#define __TCPDRIVER_H__


/* outputq congestion levels */
#define TCP_CONG_INCREASE    0x01       /* inc from last flow indication */
#define TCP_CONG_LOW         0x02
#define TCP_CONG_MEDIUM      0x04
#define TCP_CONG_HIGH        0x08
#define TCP_DAMP_CONG           5       /* throttles flow indications */

/* tcpdriver_readf is in ip/ip_registry.h */
typedef void (*tcpdriver_closef)(tcp_encaps *, void *);
typedef boolean (*tcpdriver_flushf)(void *);
typedef boolean (*tcpdriver_flow_ctrl)(tcp_encaps *, ulong, void *);
   
typedef ushort tcp_encaps_state;
typedef uchar tcp_encaps_events;
typedef uchar tcp_encaps_flags;

/* Encapsulation state */
enum { TCPD_ALIVE, TCPD_CLOSE, TCPD_ABORT };

struct tcp_encaps_ {
    struct tcp_encaps_ *next;
    tcbtype *tcb;
    queuetype outputq;
    ulong oqmaxlen;		        /* max length of outputq */
    ushort hiwater_lodamp;
    ushort hiwater_hidamp;
    ushort lowater_lodamp;
    ushort lowater_hidamp;
    ulong cong_level;                   /* current outputq congestion level */
    int inthresh;			/* when to call readf */
    void *cookie;			/* hand this back to readf */
    tcp_encaps_state state;		/* State of this thing */
    tcp_encaps_events events;		/* Events (or levels) */
    tcp_encaps_flags flags;		/* Flags */
    sys_timestamp closetime;		/* When did we start closing? */
    tcpdriver_readf readf;		/* Read callback */
    tcpdriver_closef closef;		/* Close callback */
    tcpdriver_flushf flushf;		/* Flush callback */
    tcpdriver_flow_ctrl flow_ctrl;      /* Flow Control callback */
    struct tcp_encaps_ *activate_next;
    struct tcp_encaps_ *activate_prev;
    timer_t *timer;
};

/*
 * Bit assignments for events
 */

#define TDEV_NOT_ALIVE	0x01	/* encaps not alive anymore */
#define TDEV_OUT_PEND	0x02	/* Output pending for encaps */
#define TDEV_NEW_CONN	0x04	/* This is a new passive connection */
#define TDEV_KILL_ME	0x08	/* Kill this encaps */
#define TDEV_ALL_EVENTS	0x0f

/*
 * Bit assignments for flags
 */

#define TDF_ACTIVATED	0x01	/* This encaps has been activated */

/* Events that will wake up the TCP Driver */
#define TCP_ACTIVATIONS	(TCPEV_FROM_ESTAB | TCPEV_IS_CLOSED | TCPEV_WINDOW | \
			 TCPEV_RTQUEUE)
#define TD_ACTIVATIONS	(TDEV_ALL_EVENTS)

/*
 * Return values from tcpdriver_send() and tcpdriver_send_now()
 * The actual values are historical, thus the defines for safety.
 */
#define TCPDSEND_CLOSED		-1	/* failure -- connection gone */
#define TCPDSEND_QUEUED		0	/* successful -- queued for output */
#define TCPDSEND_QFULL		1	/* failure -- output queue full */
#define TCPDSEND_SENT		2	/* successful -- packet was sent */

#define TCPDSEND_OK(x)	    ((x) == TCPDSEND_QUEUED || (x) == TCPDSEND_SENT)
#define TCPDSEND_ERROR(x)   !(TCPDSEND_OK(x))

/*
 * Bits used in tcpdriver_flags byte in the packet structure
 */

#define TCPD_PAK_INPIPE		0x01

/*
 * Op values for tcpdriver_keepalive()
 */

enum {
    TCPD_KP_ENABLE,
    TCPD_KP_INTERVAL,
    TCPD_KP_ATTEMPTS,
};


extern void tcpdriver_init(void);

#endif __TCPDRIVER_H__
