/* $Id: arap_public.h,v 3.2 1995/11/17 08:38:51 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/arap/arap_public.h,v $
 *------------------------------------------------------------------
 * Public function prototypes.
 *
 * October 1993, Steven Lin
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: arap_public.h,v $
 * Revision 3.2  1995/11/17  08:38:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:04:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef ARAPPUB_H
#define ARAPPUB_H


/* Message types, as defined in the ARAP spec */
#define MSG_ACK			0
#define MSG_SRVRVERSION		1
#define MSG_RMTVERSION		2
#define MSG_AUTH_CHALLENGE	3
#define MSG_AUTH_REQUEST	4
#define MSG_AUTH_RESPONSE	5
#define MSG_STARTINFOFROMSERVER	6
#define MSG_STARTINFOFROMREMOTE 7
#define MSG_ZONELISTINFO	8
#define MSG_TICKLE		9
#define MSG_TIMELEFT		10
#define MSG_TIMERCANCELLED	11
#define MSG_AUTH_AGAIN		12
#define MSG_CALLBACK            17
#define MSG_TEXT_ALERT		18

#define ARAP_INTERNAL		16
#define ARAP_DATA		80

/* packet offsets for ARA internal packets*/
#define ARAPAK_DATAFLAG         0
#define ARAPAK_SEQNUM           1
#define ARAPAK_CMDHIGHBYTE      2  /* never used */
#define ARAPAK_CMD              3

/* packet offsets for ARA data packets*/
#define SRP_FLAG	0
#define SRP_LLAP_DEST	1
#define SRP_LLAP_SRC	2
#define SRP_LLAP_TYPE	3

#define ARAPAK_HDRLEN           4

#define DDP_LENGTHHIGH          0
#define DDP_LENGTHLOW           1
#define DDP_CKSUMHIGH           2
#define DDP_CKSUMLOW            3
#define DDP_DESTNETHIGH         4
#define DDP_DESTNETLOW          5
#define DDP_SRCNETHIGH          6
#define DDP_SRCNETLOW           7
#define DDP_DESTNODE            8
#define DDP_TYPE                12

#define NBP_TYPE                13
#define NBP_NETHIGH		15
#define NBP_NETLOW		16
#define NBP_NODE		17

#define ARAPAK_SEQNUM           1
#define ARAPAK_CMDHIGHBYTE      2  /* never used */
#define ARAPAK_CMD              3

#define ARAPAK_HDRLEN           4

#define DDP_LENGTHHIGH          0
#define DDP_LENGTHLOW           1
#define DDP_CKSUMHIGH           2
#define DDP_CKSUMLOW            3
#define DDP_DESTNETHIGH         4
#define DDP_DESTNETLOW          5
#define DDP_SRCNETHIGH          6
#define DDP_SRCNETLOW           7
#define DDP_DESTNODE            8
#define DDP_TYPE                12

#define ARAP_NOVERSION		0
#define ARAP_VERSION1		1
#define ARAP_VERSION2		2

#define MAX_MTLENGTH		618

/* bits for tty->arap_mask */

#define ARAP_NOSMARTBUFFER	0x1     /* turn off smartbuffering ? */
#define ARAP_NOGUEST		0x2     /* disallow guest logins ? */
#define ARAP_NOGUEST_IFNEEDED	0x4     /* if noguest is set but authentication
					was done some other way, allow ARAP to 
					connect */
#define ARAP_MANUALPASSWORD	0x8     /* require manual entry of passwords */ 
#define ARAP_ABORTV42BIS	0x10    /* emergency bail out of v42bis. */

#define ARAP_SUCCESS  0
#define ARAP_FAILURE -1

#define NOTTY_DEBUG -1	/* Used to flag that line specific debugging 
			 * is not enabled
			 */

/* Need to print debug messages
 *    if debug all is turned on OR
 *    if debugging is enabled on all lines OR 
 *    if line specific debugging is enabled
 */
#define ARAP_BUGINF(debug, tty_debug, ttynum)\
    (((debug) && ((tty_debug == ttynum) ||\
    (tty_debug == NOTTY_DEBUG))) ? TRUE : FALSE)

extern int arainttty_debug;
extern int aramnp4tty_debug;
extern int aramemtty_debug;
extern int arav42tty_debug;
extern ushort arap_net;
extern uchar arap_zone[];
extern int arap_zonelen;

/* public function prototypes for arap.c */
void arap_intercept(paktype *, boolean *);
void exec_arap_command(parseinfo * );
void show_arap(parseinfo * );
void arap_command(parseinfo * );
forktype arap(tt_soc * );
paktype *arap_input_getbuffer(int , hwidbtype *, int);
paktype *arap_getbuffer(int, int);
void arap_datagram_done(paktype *, int);
paktype *arap_pak_duplicate(paktype * , int);
boolean arap_overflowed(tt_soc * );
void ara_init(subsystype *);
void ara_parser_init(void);
void init_arap_globals(void);
boolean ara_send_message (tt_soc *, paktype *);
void v42bis_exit_handler(void);
void arap_cleanup(tt_soc *);
boolean arap_callback_enabled(void);
void send_link_request_response(mnp4type *);
#endif	/* ARAPPUB_H */
