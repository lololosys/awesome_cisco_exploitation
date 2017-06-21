/* $Id: decnet_echo.h,v 3.2.60.1 1996/03/18 19:11:34 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/decnet/decnet_echo.h,v $
 *------------------------------------------------------------------
 * D E C n e t _ e c h o . h
 *
 * July 1992
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module contains all the definitions and data structures for the ping
 * command.
 *------------------------------------------------------------------
 * $Log: decnet_echo.h,v $
 * Revision 3.2.60.1  1996/03/18  19:11:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/14  00:53:13  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.2.26.1  1996/02/20  13:44:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:58:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/10  20:37:22  asastry
 * CSCdi36667: Allow DECnet pings to make use of 'size' in extended mode.
 *
 * Revision 2.2  1995/07/07  22:46:36  hampton
 * Convert DECNET to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi36830]
 *
 * Revision 2.1  1995/06/07 20:22:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern uchar dn_e_baseaddr[IEEEBYTES];
extern queuetype DNechoQ;

/*
 * Echo Packet Format
 */

typedef struct dn_echo_cb_t_ {
    struct dn_echo_cb_t_ *next;
    tt_soc *tty;
    boolean active;		/* haven't seen reply yet if true */
    sys_timestamp timeout;
    sys_timestamp xmittime;	/* when it was sent */
    ulong deltatime;		/* how long it took to come back */
    uchar how;			/* what to print -- '!', '.', etc. */
    int flags;			/* option flags */
    boolean echo_flavor;        /* echo = TRUE, trace = FALSE */
    uchar echo_dest[2];		/* echo destination */
} dn_echo_cb_t;

typedef struct dnnsplinkpkt_ {
    unsigned char msgflg;	/* flags byte */
    unsigned char dstaddr[2];   /* logical link destination */
    unsigned char srcaddr[2];	/* logical link source */
    unsigned char acknum[2];	/* not used */
    unsigned char ackdat[2];    /* not used */
    unsigned char segnum[2];	/* number of this message */
    unsigned char lsflags;	/* link service flags */
    unsigned char fcval;	/* not used */
} dnnsplinkpkt;
#define SIZEOF_DNNSPLINKPKT 13

#define DN_MAXECHOSIZE DN_MINPACKET
#define DN_MINECHOSIZE (SIZEOF_DNDATAPACKET + SIZEOF_DNNSPLINKPKT)

#define NSP_LINK_SERVICE 0x10
#define MAX_BUFLEN 80

/*
 * PROTOTYPES
 */

/*
 * decnet_echo.c
 */
extern boolean dn_disconnect(paktype *, dndatapacket *, dnnet *);
extern boolean dn_echoBLOCK(ulong);  
extern boolean decnet_ping_parse(char *, int, int *, char **, boolean *);
extern void dn_ping(char *);
extern boolean dn_send_echo(int, int, dn_echo_cb_t *, long);
extern void register_snmp_decnet_echo(void);
