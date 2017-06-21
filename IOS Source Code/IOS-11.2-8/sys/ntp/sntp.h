/* $Id: sntp.h,v 3.1.2.3 1996/06/25 22:55:29 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/ntp/sntp.h,v $
 *------------------------------------------------------------------
 * Definitions for Simple NTP (SNTP)
 *
 * June 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sntp.h,v $
 * Revision 3.1.2.3  1996/06/25  22:55:29  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add rudimentary clock selection logic.  Get rid of most static storage.
 * Keep track of all broadcast servers.
 *
 * Revision 3.1.2.2  1996/06/23  18:26:06  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Code review comments.
 *
 * Revision 3.1.2.1  1996/06/23  04:40:24  dkatz
 * CSCdi61119:  Commit SNTP Code
 * Branch: California_branch
 * Add an SNTP client.  This is not included in shipping IOS images.
 *
 * Revision 3.1  1996/06/22  21:14:50  dkatz
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _SNTP_H_
#define _SNTP_H_

/* Default NTP version number */

#define SNTP_DEFAULT_VERSION 1		/* Default version number */

/*
 * SNTP server structure.
 */
typedef struct sntp_server_ {
    struct sntp_server_ *next;		/* Next guy in the chain */
    ipaddrtype server_address;		/* Server's address */
    uchar server_version;		/* Version number to send */
    uchar server_stratum;		/* Server's stratum */
    tinybool server_configured;		/* TRUE if we configured this guy */
    ulong server_rapid_poll_count;	/* Count of remaining rapid polls */
    sys_timestamp server_reply_time;	/* Time of last reply */
    mgd_timer server_timer;		/* Timer to kick poll */
} sntp_server;

/*
 * SNTP process structure.
 */
typedef struct sntp_proc_ {
    boolean sntp_running;		/* TRUE if the process is running */
    pid_t sntp_pid;			/* PID of the process */
    mgd_timer sntp_master_timer;	/* Parent for all managed timers */
    boolean sntp_broadcast_client;	/* TRUE if we listen to broadcasts */
    queuetype sntp_server_queue;	/* Queue of server entries */
    sntp_server *sntp_current_server;	/* Currently synced to this guy */
    ipsocktype *sntp_sock;		/* Pointer to input socket */
} sntp_proc;


/* 
 * Maximum slew value.  The kernel slews at 500 ppm (500 usec per second).
 * Assuming a 64 second arrival rate, the most we can slew is 32 msec.
 * The value below is the fractional part of an NTP timestamp (in units
 * of 2^-32 seconds), calculated by 2^32 * .032.
 */
#define SNTP_MAX_SLEW 0x083126e9	/* 32 msec in binary form */


/* SNTP poll rate */

#define SNTP_POLL_RATE (64 * ONESEC)
#define SNTP_RAPID_POLL_RATE (ONESEC)
#define SNTP_RAPID_POLL_COUNT 10	/* Max number of quick polls */


/* SNTP client startup delay */

#define SNTP_STARTUP_DELAY (10 * ONESEC)  /* Wait time before first poll */


/* Server structure deletion timeout */

#define SNTP_SERVER_DELETION_TIMEOUT (10 * ONEMIN)


/* Server liveness timeout for selection purposes */

#define SNTP_SERVER_DEAD_TIMEOUT ((2 * SNTP_POLL_RATE) + (10 * ONESEC))


/* External declarations */

extern void sntp_poll(struct pkt *, int version);
extern void sntp_receive(sntp_proc *, struct pkt *, sntp_server *, ulong);
extern void sntp_send(sntp_proc *, sntp_server *);
extern void sntp_readpkt(sntp_proc *);
extern boolean sntp_start(sntp_proc **);
extern void sntp_stop(sntp_proc *);
extern boolean sntp_init_io(sntp_proc *);
extern void sntp_shutdown_io(sntp_proc *);
extern void sntp_delete_all_servers(sntp_proc *);
extern void sntp_chain_init(void);
extern sntp_server *sntp_find_server(sntp_proc *, ipaddrtype);
extern sntp_server *sntp_create_server(sntp_proc *, ipaddrtype);
extern void sntp_delete_server(sntp_proc *, sntp_server *);
extern sntp_proc *sntp_get_procptr(void);

#endif					/* _SNTP_H_ */
