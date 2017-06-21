/* $Id: remif_proto.h,v 3.3 1996/02/13 08:13:43 dstine Exp $
 * $Source: /release/111/cvs/Xsys/ip/remif_proto.h,v $
 *------------------------------------------------------------------
 * Remote Interface protocol definition
 *
 * October 1993, Carl Schaefer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines how to interact with a remote interface over
 * TCP (control stream) & UDP (packet stream)
 *------------------------------------------------------------------
 * $Log: remif_proto.h,v $
 * Revision 3.3  1996/02/13  08:13:43  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.2  1995/11/17  09:36:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:00:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* duplicated from ip/ip.h */
#define REMIF_PORT	1984	/* remote interfaces over TCP & UDP */

/*
 * maximum size of argument string to a command issued over the
 * TCP stream.
 */

#define REMIF_MAXDATA	64		/* cntl msg max data area size */
#define REMIF_MAXADDR   10		/* Maximum physical address size */


/*
 * interaction over UDP
 * only used for packet data (in both directions)
 *
 * Format is:
 *   4 bytes of interface ID
 *   4 bytes of packet length
 *   packet data
 */

typedef struct remif_pakhdr_ {
    unsigned int if_id:32;		/* struct remif_t* */
    unsigned int bytes:32;		/* data[] byte count */
    unsigned char data[0];
} remif_pakhdr;

/*
 * there's a bunch of these (in a list) for every active remote interface
 */

typedef struct remif_listener_ {
    struct remif_listener_ *next;	/* Linked list next entry */
    int size;				/* Address size (might not be MAC) */
    unsigned char addr[REMIF_MAXADDR];	/* Address data */
} remif_listener;

typedef boolean (*tring_bridge_setup_t)(hwidbtype *);

/*
 * there's one of these for every active remote interface
 */
typedef struct remif_ {
    tcbtype* tcb;			/* control connection */
    ipsocktype* soc;			/* UDP socket for packet data */
    ipaddrtype ip_remote;		/* an IP addr of remote endpoint */
    uchar owner[32];			/* owner (user@host) of this remif */
    remif_listener *listeners;		/* Additional addresses we should
					   listen for. */

    int tunnelpid;			/* PID of raw input handler */
    queuetype pakQ;			/* queue of paks to encap and xmit */

    tring_bridge_setup_t tring_bridge_setup; /* tring setup vector */

    hwidbtype* hwidb;			/* "stolen" hwidb */
    oqueue_t oqueue;			/* saved output queue vector */
    iqueue_t iqueue;			/* saved input queue vector */
    periodic_t periodic;		/* saved periodic vector */
    int ip_routecache;			/* saved config of interface */
    enum IDB_STATES idb_state;		/* saved IDB state */
    boolean bridging_enabled;		/* bridging enabled */
} remif_t;

/*
 * interaction over TCP
 * remote interface creation, control, and destruction
 *
 * Format is:
 *   1 byte of command/response/info code
 *   1 byte of data section length (or zero of no addtional data...)
 *   optional data bytes (no more than REMIF_MAXDATA)
 *
 * CMD_* = Command from UNIX to remote interface
 * RESP_* = Command response from remote interface to UNIX
 * INFO_* = Async. information from remote interface to UNIX
 */

typedef enum REMIFCMD {

    REMIF_CMD_OPEN = 1,			/* data[] is
					   ASCIZ interface name
					   ASCIZ "user@host" (31 chars max) */

    REMIF_RESP_OPENED,			/* data[] is
					   4 bytes of interface ID
					   1 byte of address size in bytes
					   n bytes of interface phys addr. */

    REMIF_CMD_PAKPORT,			/* data[] is
					   2 bytes of UDP port */

    REMIF_CMD_NEW_MAC,			/* data[] is
					   1 byte of address size (in bytes)
					   n bytes of new mac address to
					     use. */
					   
    REMIF_CMD_LISTEN,			/* data[] is
					   1 byte of address size (in bytes)
					   n bytes of additional phys addr to
					   listen for. */
    /*
     * The following contain no additional data, and do not generate
     * responses
     */

    REMIF_CMD_CLOSE,			/* close this tunnel */
    REMIF_CMD_SHUTDOWN,			/* shut the real interface down */
    REMIF_CMD_NOSHUTDOWN,		/* bring it back up */
    REMIF_CMD_RESET,			/* force a hard reset. */
    REMIF_CMD_PROMISCUOUS,		/* enable promiscuous mode */
    REMIF_CMD_CHASTE,			/* disable promiscuous mode */

    REMIF_INFO_LINEUP,			/* line protocol transitioned up */
    REMIF_INFO_LINEDOWN,		/* line proto went down  */

    REMIF_RESP_NO_SUCH,			/* No such interface on this box */

    REMIF_RESP_IN_USE,			/* Interface is in use, data[] is 
					   ASCIZ, and tells who has control */

    REMIF_RESP_DOWN,			/* Can't attach to a down interface */

    REMIF_RESP_ERR,			/* generic error */

    REMIF_CMD_TBRIDGE                   /* Set up token ring bridging */

} remif_cmd;
