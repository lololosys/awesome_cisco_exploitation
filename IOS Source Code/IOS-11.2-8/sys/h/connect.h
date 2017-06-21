/* $Id: connect.h,v 3.3.20.3 1996/09/06 01:10:51 snyder Exp $
 * $Source: /release/112/cvs/Xsys/h/connect.h,v $
 *------------------------------------------------------------------
 * connect.h -- connection management definitions
 *  
 * September 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: connect.h,v $
 * Revision 3.3.20.3  1996/09/06  01:10:51  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/06/12  14:26:29  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/04/19  15:15:09  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 2.1.6.6  1996/04/16  03:53:45  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.1.6.5  1996/03/30  02:37:18  carrel
 * cvs fixes
 *
 * Revision 2.1.6.4  1996/03/25  20:55:24  che
 * Branch: IosSec_branch
 * Clean up how authentication negotiation works
 *
 * Revision 2.1.6.3  1996/03/05  04:43:15  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.1.6.2  1996/02/21  03:16:20  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.1  1995/11/17  16:41:53  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 * Revision 3.3  1996/02/03  01:57:54  perryl
 * CSCdi47740:  Service hide-addr hiding is incomplete
 *
 * Revision 3.2  1995/11/17  09:11:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:58:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/02  06:39:39  irfan
 * CSCdi38059:  resuming rlogin connections garbles async data on Copans
 * rlogin connections should start in cooked mode (flow sw). Avoid playing
 * with the flow control, if HW flow control is enabled on the line, for
 * both rlogin and telnet connections.
 *
 * Revision 2.4  1995/08/01  23:09:30  irfan
 * CSCdi37754:  rlogin should allow various sources of remote username
 * Allow a /user switch for rlogin command. Also let the current username
 * be passed as remote username, if rlogin trusted-remoteuser-source is
 * local. Besides, send the local username or "ciscoTS" if the current
 * username was authenticated via one of trusted methods.
 *
 * Revision 2.3  1995/06/20  22:01:36  ahh
 * CSCdi31398:  Rlogin sending window size packet twice
 * Eliminate race conditions in rlogin's urgent-data processing.
 *
 * Revision 2.2  1995/06/20  03:44:40  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.1  1995/06/07  20:34:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CONNECT_H__
#define __CONNECT_H__

#include "globs.h"
#include "address.h"

#define REMOTE_UNAME_LEN 32            /* Number of characters in */
                                       /* remote username */

struct connvector_ {

    struct connvector_ *next;		/* Link to next */
    ulong type;				/* Connection type - CONN_xxx value */
    const char *name;			/* Transport name */
    const char *help;			/* Help string to describe protocol */
    int snmp_sesstype;			/* Default type for SNMP */

    /*
     * The following vectors apply to inbound connections
     */

    boolean (*vsettty)(conntype *, tt_soc *); /* Set a new controlling tty */
    boolean (*vchanged)(conntype *);	/* Process connection changed state */
    boolean (*vreport)(conntype *);	/* Report connection status */

    /*
     * The following vectors apply to outbound sessions
     */

    int (*vrun)(conntype *, boolean); /* Run an outbound session */
    conntype *((*vopen)(parseinfo *, boolean *));
				       /* Open a session - implicit protocol */
    void (*vresume)(conntype *, parseinfo *); /* Resume a session */

    /*
     * The following vectors apply to both inbound and outbound sessions
     */

    boolean (*valive)(conntype *);	/* Is connection still alive? */
    void (*vclose)(conntype *);		/* Close a connection */
    int (*vunread)(conntype *);		/* Return unread input count */
    /*
     * Copy accounting information.  Put accounting info including
     * type of connection and destination/etc into acctinfo.
     * Called when acctstruct is first created.
     */
    void (*vaccounting)(conntype *, acctinfo *); /* Update accounting info */
};

#define CONNLEN 20			/* up to 20 chars for name strings */ 
#define	SUBBUFLEN 1024			/* up to 1024 characters */

typedef struct _conn_telnet {
    ulong tn_state;			/* telnet fsm state */
    ulong tn_option;			/* Telnet option state flags */
    ulong tn_wantoption;		/* state that we want to be in */
    ulong tn_sentoption;		/* negotiation in progress */
    void (*statechange_done) (conntype *, ulong, ulong, ulong);	/* call when negot. complete */
    boolean (*statechange_check) (conntype *, uchar);	/* call to check if request OK */
    int lastcmd;			/* last Telnet command */
    tty_editdata *editinfo;		/* pointer to line mode edit data */
    addrtype hostaddr;		        /* IP or NSAP address of host */
    char *subpointer;			/* pointer into subbuffer */
    char subbuffer[SUBBUFLEN];		/* telnet suboptions */
    boolean tn3270cmd;                  /* true if tn3270 command issued */
    boolean in3270mode;                 /* true if tn3270 options negotiated */
    boolean tn3270inited;               /* true if tn3270 resources allocated */
    boolean tn3270eorin;                /* true if an eor has just arrived */
    screen_type *tn3270data;            /* points to a 3270 structure */
    void *cursesdata; /* unused */      /* points to a curses structure */
    termcap_globals_struct *termcapdata;/* points to a termcap structure */
    int sub_length;
    boolean do_kerb_auth;
    void *kerberos_info;
} conn_telnet;

typedef struct _conn_rlogin {
    boolean ack_seen;			/* true if received the OK ack byte */
    char remote_uname[REMOTE_UNAME_LEN];/* remote username to use */
} conn_rlogin;

#define LAT_MAXCREDITS 4		/* Maximum credits ever allowed */

typedef struct _conn_lat {
    void *hiptr;			/* pointer to HIC block, if HIC. */
    uchar stop_reason;			/* Stop slot reason code */
    uchar changes;			/* Pending changes (see latob.h) */
    uchar config;			/* Desired value for config flags */
    uchar char_parity;			/* Desired character size and parity */
    uchar char_mode;			/* Character transparency mode	*/
    uchar cur_char_mode;		/* Character mode currently set */
    ushort baud_xmit;			/* Desired transmit speed	*/
    ushort baud_recv;			/* Desired receive speed	*/
    uchar slot_empty;			/* Slot empty pointer		*/
    uchar slot_fill;			/* Slot fill pointer		*/
    paktype *input_pak[LAT_MAXCREDITS];	/* Pointers to packets with input */
    uchar *input_slot[LAT_MAXCREDITS];	/* Pointers into slots to read */
    uchar input_slot_left[LAT_MAXCREDITS]; /* Number of input bytes left */
    uchar *datahead;			/* Data buffer head pointer */
    uchar *datatail;			/* Data buffer tail pointer */
    uchar *databuf;			/* Pointer to data buffer itself */
    int datacount;			/* Byte count of pending data */
    ushort flags;			/* Flags */
    uchar max_credits;			/* Maximum credits for connection */
    ulong t_capabilities;		/* Saved capabilities */
    int t_parity;			/* Saved parity setting */
    int t_databits;			/* Saved databits setting */
    ulong t_icapabilities;		/* Current interactive capabilities */
} conn_lat;

typedef struct _conn_pad {
    boolean (*statechange_check) (conntype *, int, int);/* call to check if request OK */
} conn_pad;

struct conntype_ {
    struct conntype_ *link;		/* to the next guy or null */
    ulong status;			/* status of the connection */
    connvector *v;			/* pointer to dispatch vector */
    void *tcb;				/* pointer to TCB */
    void *spxcon;                       /* pointer to SPX connection */
    tt_soc *tty;			/* pointer to terminal data */
    int number;				/* connection number */
    boolean debug;			/* TRUE if debugging */
    sys_timestamp logintimer;		/* timer for magic fingers support */
    sys_timestamp sessiontimer;		/* idle session timeout */
    sys_timestamp sessiontimer_warning; /* idle session timeout warning*/
    sys_timestamp idletime;		/* time since last activity */
    sys_timestamp notetime;		/* notification time in ticks */
    sys_timestamp dispatchtimer;	/* timer for holding net-bound data */
    char hostname[CONNLEN+1];		/* hostname of connection */
    char logicalname[CONNLEN+1];	/* connection's logical name */
    char *loginstring;			/* pointer to init login string */
    char *errorstring;			/* Pointer to disconnect string */
    addrtype address;			/* Remote address, if any */
    int snmp_sesstype;			/* Session type for SNMP */
    acctinfo *acct_info;		/* Saved accounting info */
    union {				/* Protocol-dependent fields */
	conn_telnet tn;			/* Telnet-specific data */
	conn_rlogin rlogin_info;        /* Rlogin-specific date */
	conn_lat lat;			/* LAT specific data */
	conn_pad pad;			/* PAD specific data */
    } proto;
};

#define SLC_LEVELBITS	3		/* telnet SLC support level bits */
# define SLC_DEFAULT	3		/* Set to default, please */
# define SLC_VALUE	2		/* Set to a value */
# define SLC_CANTCHANGE 1		/* supported, but can't change */
# define SLC_NOSUPPORT	0		/* Not supported at all */
#define SLC_FLUSHOUT	32		/* Flush output */
#define SLC_FLUSHIN	64		/* flush input */
#define SLC_ACK		128		/* Ack */

/*
 * status bits
 */

#define CONN_CURRENT    0x1	/* current connection */
#define CONN_NOTIFIED   0x2	/* set if notified of data ready */
#define CONN_STREAM	0x8	/* TCP data stream (no telnet protocol) */
#define	CONN_SETUP	0x10	/* already initialized */
#define	CONN_FLOW	0x20	/* flow control set */
#define CONN_CHECKALL	0x40	/* check all scheduler predicates */
#define CONN_CHANGED	0x100	/* some parameter has changed */
#define CONN_KBDLOCK	0x200	/* "lock" the keyboard */
#define CONN_FLUSH	0x1000	/* Flush pending output on connection */
#define CONN_INBOUND	0x4000	/* Connection is inbound */
#define CONN_QUIET	0x8000	/* Quiet connect, no connect messages */
#define CONN_WRITEBLOCK	0x10000	/* Blocked on write to network*/ 

/*
 * Transport-related definitions
 */

#define CONN_TRANSPORT_DEFMASK (0xffffffff) /* Default transport mask */

/*
 * Flags for LAT protocol (conn_lat.flags)
 */

#define LAT_USER_INPUT	0x0001		/* User input should be checked by
					 * LAT sx_get_char routine */
#define LAT_SEND_BREAK	0x0002		/* Send a break slot soon */
#define LAT_CONN_HIC	0x0004		/* Connectio nwas host-initiated */

#define conn_settty(conn, tty) ((*((conn)->v->vsettty)) (conn,tty))
#define conn_changed(conn) ((*((conn)->v->vchanged)) (conn))
#define conn_report(conn) ((*((conn)->v->vreport)) (conn))
#define conn_alive(conn) ((conn) ? ((*((conn)->v->valive)) ((conn))) : FALSE)
#define conn_run(conn, resumeflag) ((*((conn)->v->vrun)) (conn, resumeflag))
#define conn_resume(conn, csb) ((*((conn)->v->vresume)) (conn, csb))
#define conn_close(conn) ((*((conn)->v->vclose)) (conn))
#define conn_unread(conn) ((*((conn)->v->vunread)) (conn))
#define conn_acctupdate(conn, acct) ((*((conn)->v->vaccounting)) (conn, acct))

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * connect.c
 */
extern boolean close_command(tt_soc *, int, char *, conntype *);
extern boolean conn_session(conntype *, boolean);
extern conntype *conn_check_idles(void);
extern boolean talk(tt_soc *, conntype *, int);
extern conntype *conn_leastidle(tt_soc *);
extern conntype *findconn(tt_soc *, int);
extern conntype *findlogicalname(tt_soc *, char *, ulong *);
extern conntype *getconnection(tt_soc *, connvector *);
extern int fetchnumber(tt_soc *);
extern int print_host(conntype *);
extern char *tty_get_remote_addr(tt_soc *);
extern void assignlogicalname(conntype *, char *);
extern void clearnotices(tt_soc *);
extern void freeconnection(conntype *);
extern void notification(conntype *);
extern void show_transport(tt_soc *);
extern void sortenqueue(queuetype *, conntype *);
extern void transport_cmd(parseinfo *);

extern queuetype conn_vectorQ;
extern ulong connect_sysdef_transport;
extern const char connect_unknown[];
extern boolean connect_allowed(connvector *);
extern void connect_new_session(conntype *, char *, parseinfo *);
extern void conn_vector_init(connvector *, const char *, const char *);
extern void conn_setsflow(tt_soc *, boolean);
extern void conn_restoreflow(tt_soc *, ulong);

extern void start_sessiontimeout (conntype *conn, tt_soc *tty);

#endif __CONNECT_H__
