/* $Id: dnld.h,v 3.2 1995/11/17 07:41:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/dnld.h,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Author: clev
 *------------------------------------------------------------------
 * $Log: dnld.h,v $
 * Revision 3.2  1995/11/17  07:41:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:11:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Defines for the download mechanism
*/

/*
** The following are the target handshake characters.
** The target sends the appropriate char to the host to confirm
** reciept of a packet or to inform of other status.
*/
#define DNLD_HNDSHK_START    '~'  /* target start character */
#define DNLD_HNDSHK_ACK      '+'  /* packet recieved ok */
#define DNLD_HNDSHK_RESEND   '-'  /* resend a packet */
#define DNLD_HNDSHK_ABORT    '!'  /* abort the transmission */

/* general defines */
#define DNLD_MAXRETRIES      5    /* packet retry count */
#define DNLD_MAXPACKET       256  /* maximum packet size */

/* flag for datacount field to indicate 32 bit address field */
#define DNLD_32BITFLG        0x8000

/* host sends to target in address field of termination record to abort */
#define DNLD_ABORT_ADDR      -1

/*
** defines for the host side state machine
*/
typedef enum {
    DNLD_IDLE,         /* not doing a download */
    DNLD_STARTWAIT,    /* waiting for a start from the target */
    DNLD_ACKWAIT,      /* waiting for an ack from the target */
    DNLD_SENDPKT,      /* send the packet in the filebuf */
    DNLD_SENDNEXTPKT,  /* read and send the next packet */
    DNLD_SENDLASTPKT,  /* send this packet and quit */
    DNLD_LASTACKWAIT,  /* wait for last ack */
} DNLD_STATE;

#ifndef UNIX
/* dnld.c */
extern void dnld(char *c);
extern void download(char *progname, int verbose);
#endif
