/* $Id: tacacs.h,v 3.4.48.2 1996/04/19 15:29:31 che Exp $
 * $Source: /release/112/cvs/Xsys/servers/tacacs.h,v $
 *------------------------------------------------------------------
 * tacacs.h - TAC access protocol support
 *
 * May 1988, Greg Satz
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tacacs.h,v $
 * Revision 3.4.48.2  1996/04/19  15:29:31  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.4  1996/04/10  23:08:24  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 2.1.6.3  1996/02/28  06:15:05  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.6.2  1995/12/17  02:16:20  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  17:39:31  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.4.48.1  1996/04/15  21:22:44  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.4  1995/12/08  00:54:44  irfan
 * CSCdi45224:  tacacs could use a specified source address
 * allow tacacs (TA, XTA, TA+) to use a specific (up) interface's IP
 * address.
 *
 * Revision 3.3  1995/11/22  22:38:28  che
 * CSCdi42049:  TACACS failures when using enable password privledge levels
 * Added new parameter to xtacacs_enable() to specify whether we're
 * doing an "ask" or a "notify".
 *
 * Revision 3.2  1995/11/17  18:51:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/08  16:48:41  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.2  1995/06/20 16:11:30  lol
 * CSCdi34401: Tacacs notif.type 6-ena and 10-slipoff not sent to
 *             nondefault server
 *
 * Revision 2.1  1995/06/07  22:44:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TACACS_H__
#define __TACACS_H__

#define	TA_VERSION		0
#define XTA_VERSION		0x80
#define XTA_MAJOR_VERSION_MASK	0xf0
#define XTA_MINOR_VERSION_MASK	0x0f

#define XTA_MINOR_VER_0		0x00
#define XTA_MINOR_VER_1		0x01
#define XTA_MINOR_VER_CUR	XTA_MINOR_VER_1

/*
 * global flags in tacacs_extended
 */
#define TACACS_EXTEND		0x01	/* extended tacacs is enabled */
#define TACACS_ASKCONNECT	0x02	/* ask about connection */
#define TACACS_NOTECONNECT	0x04	/* notifiy about connections */
#define TACACS_ASKSLIP		0x08	/* ask about SLIP command */
#define TACACS_NOTESLIP		0x10	/* notify about SLIP command */
#define TACACS_ASKENA		0x20	/* ask about "enable" */
#define TACACS_NOTEENA		0x40	/* notify about enable */
#define TACACS_NOTELOGOUT	0x80	/* notify about logout/disconnect */
#define TACACS_GETSLIPACL	0x100	/* Request&Install slip access lists */

/*
 * Operations
 */

#define	TA_QUERY		1
#define	TA_ANSWER		2
#define	TA_CHANGE		3
#define	TA_FOLLOW		4

/*
 * ANSWER responses
 */

#define	TA_A_ACCEPTED	1
#define	TA_A_REJECTED	2

/*
 * Reasons included in ANSWER
 */

#define TA_A_NONE	0
#define	TA_A_EXPIRING	1
#define	TA_A_PASSWORD	2
#define	TA_A_DENIED	3
#define	TA_A_NOROUTE	8	/* Dialup routing not permitted */
#define	TA_A_LOGINREQ	9       /* Login required for requested action */


/*
 * TACACS packet structure and sizes
 */

typedef struct tacacstype_ {
    uchar version;
    uchar type;
    ushort trans;
    uchar namelen;
    uchar pwlen;
} tacacstype;

#define	TACACS_SIZE	6

/*
 * eXtended TACACS
 * Try to keep longwords longword aligned.
 */
typedef struct xtacacstype_ {
    uchar version;		/* version of protocol */
    uchar type;			/* Type of query/response */
    ushort trans;		/* transaction ID */
    /*---------------*/
    uchar namelen;		/* length of name */
    uchar pwlen;		/* length of password */
    uchar response;		/* response code */
    uchar reason;		/* reason for response */
    /*---------------*/
    ulong uuid;			/* user id code assigned. */
    /*---------------*/
    ipaddrtype dhost;		/* destination host */
    /*---------------*/
    ushort dport;		/* destination port */
    ushort lport;		/* local line number */
    /*---------------*/
    ulong flags;		/* misc flags */
    /*---------------*/
    ushort accesslist;		/* access list for user */
				/* user name */
				/* password */
} xtacacstype;

/*
 * Define the size of the above structure
 */
#define XTACACS_SIZE		26

/*
 * "types"
 */

#define	XTA_LOGIN	1
#define	XTA_ANSWER	2
#define	XTA_CHANGE	3
#define	XTA_FOLLOW	4
#define XTA_CONNECT	5
#define XTA_ENABLE	6
#define XTA_LOGOUT	7
#define XTA_RELOAD	8
#define XTA_SLIPON	9
#define XTA_SLIPOFF    10
#define XTA_SLIPADDR   11
#define XTA_ARAP_AUTH  12
#define XTA_CHAP_AUTH  13

/*
 * ANSWER responses
 */

#define	XTA_A_ACCEPTED	1
#define	XTA_A_REJECTED	2

/*
 * reasons for rejection of request
 */

#define XTA_A_NONE	0
#define	XTA_A_EXPIRING	1
#define	XTA_A_PASSWORD	2
#define	XTA_A_DENIED	3
#define	XTA_A_NOROUTE	8	/* Dialup routing not permitted */
#define	XTA_A_LOGINREQ	9       /* Login required for requested action */

/*
 * The following are reasons for "logout" and "slipoff" xtacacs messages
 */
#define XTA_R_QUIT	4	/* user quit normally */
#define XTA_R_IDLE	5	/* idle timeout */
#define XTA_R_DROP	6	/* carrier dropped */
#define XTA_R_BAD	7	/* too many bad passwords */

/*
 * flags used
 */
     /* 
      * These flags defined for returns from XTACACS server.
      * NOTE: these flags are ignored.
      */
#define XTA_F_NOENABLE	   1	/* user may not enable */
#define XTA_F_NOENAPASS	   2	/* user may enable without password */
#define XTA_F_NOSLIP	   4	/* user may not run SLIP */
#define XTA_F_SLIPPASS	   8	/* user needs ok to run SLIP */
#define XTA_F_CONNPASS	0x10	/* user needs ok to make connections */
#define XTA_F_ACCESS	0x20	/* use the provided access list number */
     /* 
      * The following flag set in requests to XTACACS server.
      */
#define XTA_F_ROUTING	0x40	/* user wants to do dialup routing */

#define	TACACS_RETRANS	2
#define	TACACS_TIMEOUT	(5 * ONESEC)

/*
 * The following flags are used for single line password evaluation
 * (Currently used by ARAP & CHAP)
 */
#define TAC_SINGLELINE_CHAR		'*'
#define TAC_SINGLELINE_ARAP_PASS	"arap"



typedef struct ta_blocktype_ {
    struct ta_blocktype_ *next;	/* ptr to next entry */
    boolean done;		/* when complete */
    sys_timestamp timeout;	/* when to quit */
    ushort trans;		/* unique transaction number */
    uint response;		/* repsonse */
    paktype *pak;		/* packet to send */
    sys_timestamp nexttime;	/* When to try again (perhaps) */
} ta_blocktype;

/*
 * TACACS wait queue for responses
 */

typedef struct tacacsblock_ {
    struct tacacsblock_ *next;		/* ptr to next entry */
    boolean done;			/* when complete */
    sys_timestamp timeout;		/* when to quit */
    ushort trans;			/* unique transaction number */
    uchar response;			/* result */
    uchar reason;			/* result's why */
    ulong uuid;				/* unused for now */
} tacacsblock;

typedef struct servtype_ {
    struct servtype_ *next;		/* ptr to next entry */
    ipaddrtype addr;			/* internet address */
    ipsocktype *sock;			/* UDP socket */
    boolean single_connection;          /* T+ server type */
    uint timeout;			/* T+ per-server timeout */
    uint port;                          /* T+ per server port to use */
    tcbtype *handle;                    /* T+ connection handle */
    uint bytes_in;                      /* T+ # of bytes read */
    uint allocated;                     /* T+ space allocated */
    char *key;				/* T+ per-server key */
    tacacs_plus_pkt_hdr *in_data;       /* T+ input packet being assembled */
    uint closes;			/* handle closes */
    uint opens;				/* handle opens */
    uint errors;			/* error count */
    uint aborts;                        /* abort count */
    uint paks_in;                       /* # of incoming packets read */
    uint paks_out;                      /* # of outgoing packets sent */
} servtype;

/*
 * Slip acl structure for return data from SLIPON request.
 */
typedef struct xta_slip_acl_ {
    long      in;
    long      out;
} xta_slip_acl;

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
extern queuetype tacacshostQ;
extern queuetype tacacsreqQ;
extern queuetype tacacslogQ;
extern uint tacacs_extended;
extern uint tacacs_lastresort;
extern uint tacacs_retrans;
extern ulong tacacs_timeout;
extern ulong tacacs_always;
extern boolean tacacs_directed_request;
extern boolean tacacs_directed_restricted;
extern idbtype *tacacs_source_idb;

/*
 * tacacs.c
 */ 
extern boolean tacacs_server(paktype *ip, udptype *udp);
extern void tacacs_service(parseinfo *csb);
extern boolean tacacs_send_query_addr(char *name, char *pw, tt_soc *tty,
				      boolean enabling, char **reason,
				      ipaddrtype addr);
extern boolean tacacs_send_query(char *name, char *pw, tt_soc *tty,
				 boolean enabling, char **reason);
extern boolean tacacs_fallback(int which, char **reason);
extern boolean tacacsBLOCK(tacacsblock *block);
extern int tacacs_reason(tt_soc *tty);
extern char *tacacs_whytext(xtacacstype *tac);
extern ipaddrtype tacacs_parse_server(char *user_str);
/*
 * xtacacs.c
 */ 
extern void xtacacs_enable_ext(void);
extern void xtacacs_setask(parseinfo *csb, int mask, boolean always,
			   boolean get_acl);
extern void xtacacs_setnote(parseinfo *csb, int mask, boolean always);
extern void xtacacs_server(paktype *ip);
extern void tacacs_resend(void);
extern void tacacs_acknowledgement(paktype *ip, xtacacstype *tac);
extern paktype *xtacacs_data_setup(char *name, void *data, int data_len,
				   int type, int ttynum, int minor_ver);
extern boolean xtacacs_data_ask_addr(paktype *pak, char **reason,
				     ipaddrtype addr, void *data,
				     int *recv_len);
extern boolean xtacacs_ask(paktype *ip, char **reason);
extern boolean xtacacs_note(paktype *ip, ipaddrtype addr);
extern boolean xtacacs_login(int ttynum, char *name, char *pw, char **reason,
			     ipaddrtype addr);
extern boolean xtacacs_connect(ipaddrtype dhost, ushort dport);
extern boolean xtacacs_enable(char *name, char *pw, char **reason,
			      uchar level, boolean sendask);
extern void xtacacs_set_enable(parseinfo *csb);
extern void xtacacs_enable_notify(tt_soc *tty, uchar level);
extern void xtacacs_logout(tt_soc *tty);
extern void xtacacs_reload(void);
extern boolean xtacacs_slipon(char *name, char *pw, tt_soc *tty,
			      ipaddrtype address, char **reason,
			      boolean routing, boolean from_exec_cmd);
extern void xtacacs_slipoff(tt_soc *tty, ipaddrtype address, hwidbtype *idb,
			    boolean from_ipcp);
extern boolean xtacacs_slipaddr(tt_soc *tty, char *name, char *pw, 
				ipaddrtype address,
				char **reason, boolean routing);
extern boolean xtacacs_authen(char *user, void *data, int data_len,
			      int recv_len, char **reason, int type,
			      tt_soc *tty);

/*
 * Inlines:  Used to be functions in xtacacs.c
 */
static inline boolean xtacacs_ask_addr(paktype *pak, char **reason,
				       ipaddrtype addr)
{
    return(xtacacs_data_ask_addr(pak, reason, addr, NULL, NULL));
}
static inline paktype *xtacacs_setup(char *name, char *pw, int type,
				     int ttynum)
{
    return(xtacacs_data_setup(name, pw, strlen(pw), type, ttynum,
			      XTA_MINOR_VER_0));
}
/*
 * tacacs_chain.c
 */
extern void tacacs_parser_init(void);

extern servtype *get_server (addrtype *);

#endif __TACACS_H__
