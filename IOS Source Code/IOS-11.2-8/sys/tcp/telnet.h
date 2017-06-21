/* $Id: telnet.h,v 3.5.8.2 1996/08/28 23:13:16 asb Exp $
 * $Source: /release/112/cvs/Xsys/tcp/telnet.h,v $
 *------------------------------------------------------------------
 * telnet.h -- values used by the telnet protocol.
 *  
 * April, 1988  Bill Westfield
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: telnet.h,v $
 * Revision 3.5.8.2  1996/08/28  23:13:16  asb
 * CSCdi51882:  Telnet doesnt process RFC 1073 [NAWS] options for window
 * size
 * Branch: California_branch
 *
 * Revision 3.5.8.1  1996/04/19  15:33:53  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.6  1996/04/16  03:57:18  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.1.6.5  1996/03/30  03:19:56  carrel
 * cvs fixes
 *
 * Revision 2.1.6.4  1996/03/19  08:48:18  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.3  1996/03/05  04:44:47  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.1.6.2  1996/02/21  04:13:53  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.1  1995/12/17  02:32:30  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.5  1996/02/27  22:24:04  jenny
 * CSCdi44663:  ASM line stuck in Carrier Dropped state.
 * Don't block in TCP on write unless user spcified so to avoid possible
 * lost control of the tty during TCP close.
 *
 * Revision 3.4  1996/01/23  23:45:58  vandys
 * CSCdi45547:  Desire rotary behavior on PT TCP destinations
 * Add rotor option to randomize destination for TCP host lists
 *
 * Revision 3.3  1995/11/20  23:22:13  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  18:49:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:06:32  hampton
 * Bump version numbers from 1.x to 2.x.

 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TELNET_H__
#define __TELNET_H__
/*
 * Things we need
 */
#include "../ip/ip.h"

/*
 * Telnet option codes:
 * See RFC's 764, ...., for detailed explanations.
 */
#define TN_EOR 239		/* End of record */
#define TN_SE 240		/* end subnegotiation */
#define TN_NOP 241		/* no operation */
#define TN_DM 242		/* data mark */
#define TN_BREAK 243		/* send break or attention */
#define TN_IP 244		/* interrupt process */
#define TN_AO 245		/* abort output */
#define TN_AYT 246		/* are you there? */
#define TN_EC 247		/* erase character */
#define TN_EL 248		/* erase line */
#define TN_GA 249		/* go ahead */
#define TN_SB 250		/* subnegotiation follows */
#define TN_WILL 251		/* will do option */
#define TN_WONT 252		/* won't do option */
#define TN_DO  253		/* please do option */
#define TN_DONT 254 		/* don't do option */
#define TN_IAC 255		/* interpret as command */

/*
 * These are options for the WILL/WONT/DO/DONT commands
 */
#define TNO_BINARY 0		/* send binary data */
#define TNO_ECHO 1		/* remote echoing */
#define TNO_SUPRGA 3		/* suppress go aheads */
#define TNO_TIMEMARK 6		/* timing marks */
#define TNO_TTYLOC 23		/* send terminal location */
#define TNO_TTYTYPE 24		/* send terminal type */
#define TNO_SENDEOR 25		/* send End of Records */
#define TNO_TN3270REG 29	/* tn3270 regime */
#define TNO_NAWS 31		/* negotiate about window size */
#define TNO_TTYSPEED 32		/* send terminal speed */
#define TNO_LFLOW 33		/* local flow control */
#define TNO_XDISPLAY 35		/* send X Display location */
#define TNO_AUTH 37		/* send authentication information */

#define	TNQ_IS		0	/* option is... */
#define	TNQ_SEND	1	/* send option */

/*
 * Definitions for RFC1416 Authentication
 */
#define TNQ_REPLY	2	/* reply option */
#define TNQ_NAME	3	/* name option */
#define KERBEROS_V5	2

#define KRB5_AUTH		0
#define AUTH_REJECT		1
#define AUTH_ACCEPT		2
#define AUTH_RESPONSE		3
#define AUTH_FORWARD		4
#define AUTH_FORWARD_ACCEPT	5
#define AUTH_FORWARD_REJECT	6

/*
 * Modifiers for RFC1416 Authentication
 */
#define AUTH_WHO_MASK		1
#define AUTH_CLIENT_TO_SERVER	0
#define AUTH_SERVER_TO_CLIENT	1
#define AUTH_HOW_MASK		2
#define AUTH_HOW_ONE_WAY	0
#define AUTH_HOW_MUTUAL		2

/*
 * Flag bits used in defining the Telnet state machine
 * These are stored in conn->tn_state
 */

#define ST_CMD		0x1	/* received a data mark */
#define ST_SB		0x2	/* we are doing a subnegotiation */
#define	ST_SE		0x4	/* we are finished a subnegotiation */
#define ST_OPT	 	0x8	/* we are doing a simple option */
#define ST_DM	 	0x10	/* we are searching for a data mark */
#define ST_FLUSH	0x20	/* We are flushhing output (until TM) */
#define STATE_MASK (ST_DM+ST_CMD+ST_SB+ST_SE+ST_OPT+ST_FLUSH) /* mask of all states */

#define ST_LASTINCR   0x100	/* last char input was a RETURN */
#define ST_LASTOUTCR  0x200	/* last char output was a RETURN */
				/* Reserved */
#define ST_SENDTM     0x800	/* send TIMEMARK, start flushing output */
#define ST_IACHACK   0x1000	/* Don't allow IACs to be quoted */

/*
 * option state flags.
 * These are now divorced from the fsm states.
 *
 * Note that the options are independently negotiated in each direction.
 * STH_ECHO and STO_ECHO, for example, mean very different things.
 * If we are acting as a terminal server, STH_ECHO means that the host
 * is going to echo.  If it is off, we have to echo.  STO_ECHO would
 * mean that we sent back to the host every character that it sent us
 * over the net, a very bad thing to do.  (The canonical way to tell
 * a 4.2 telnet user implementation is that it negotiates exactly this
 * absurd behavior.  Fortunately, it doesn't actually carry it out.)
 */

/* our options, right halfword */

#define STO_ECHO	   0x1	/* we are echoing */
#define STO_SUPRGA	   0x2	/* we are suppressing go aheads */
#define STO_BINARY	   0x4	/* we are sending binary */
#define	STO_TTYTYPE	   0x8	/* we are doing tty type */
#define	STO_TTYLOC	  0x10	/* we are doing tty location */
#define STO_EDIT	  0x20	/* we are doing local editing */
#define STO_LINE	  0x40	/* user wants local editing */
#define STO_TTYSPEED	  0x80	/* we are doing tty speed */
#define STO_LFLOW	 0x100	/* we will respond to flow control */
#define STO_SENDEOR	 0x200	/* we will send "end of records" */
#define STO_XDISPLAY	 0x400	/* we will send X display location */
#define STO_AUTH	 0x800	/* we will handle RFC1416 authentication 
				   protocol */
#define	STO_NAWS	0x1000  /* we will send window size */

/* his options, left halfword */

#define STH_ECHO	   0x10000	/* he are echoing */
#define STH_SUPRGA	   0x20000	/* he are suppressing go aheads */
#define STH_BINARY	   0x40000	/* he are sending binary */
#define	STH_TTYTYPE	   0x80000	/* he are doing tty type */
#define	STH_TTYLOC	  0x100000	/* he are doing tty location */
#define STH_EDIT	  0x200000	/* he are doing local editing */
#define STH_LINE	  0x400000	/* unused */
#define STH_TTYSPEED	  0x800000	/* he are doing tty speed */
#define STH_LFLOW	 0x1000000	/* he will respond to flow control */
#define STH_SENDEOR	 0x2000000	/* he will send EORs */
#define STH_XDISPLAY	 0x4000000	/* he will send X display location */
#define STH_AUTH	 0x8000000	/* he will handle RFC1416 authentication
					   protocol */
#define	STH_NAWS	0x10000000	/* he will send window size */
#define ST_TN_UNIMPLEMENTED 0		/* Unimplemented option */

/* local editing support */

#define READLINE_BLOCK 0
#define READLINE_DONE -1

#define SERVERTELNET(conn) (((tcbtype *)(conn->tcb))->flags & TCB_PASSIVEOPEN)

/*
 * Options for open_connection() "flags" field.  Do NOT use more than the
 * low 16 bits unless you read and understand pt/translate.h's use of
 * "tcpflags"!
 */
#define OPENFL_STREAM (0x01)
#define OPENFL_DEBUG (0x02)
#define OPENFL_LINE (0x04)
#define OPENFL_TN3270 (0X08)
#define OPENFL_ROTOR (0x10)


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * telnet.c
 */
extern void telnet_init(void);
extern boolean magic_fingers(conntype *);
extern boolean send_option(conntype *, ulong, ulong, uchar, uchar);
extern boolean telnetBLOCK(conntype *);
extern boolean telnetserver_optioncheck(conntype *, uchar);
extern boolean telnetuser_optioncheck(conntype *, uchar);
extern int tn_cmdstate(conntype *, uchar);
extern boolean tnof(conntype *, int, boolean, boolean);
extern char *option_string(int, uint, char *);
extern conntype *open_connection(nametype *, addrtype *, ushort, addrtype *,
				 int *, ipoptiontype *, uint, boolean *);
extern int linemode_getline(tty_editdata *, paktype *, int (*mygetc)(void),
			    ulong, int *);
extern int outguess(tt_soc *);
extern int pak_rembyte(paktype *);
extern int tcp(conntype *, int);
extern int telnet(conntype *, int);
extern int telnet_getline(conntype *);
extern int telnet_inputfsm(conntype *, uchar);
extern int tnif(conntype *, uchar);
extern void close_connection(register conntype *);
extern void option_emit(conntype *, int, int);
extern void receive_option(conntype *, ulong, ulong, ulong);
extern void tcp_clearoutput(tt_soc *);
extern void tcp_parse_resume(conntype *, parseinfo *);
extern void tcp_setsflow(tt_soc *);
extern void telnet_defaults(conntype *, boolean);
extern int telnet_input(conntype *, uchar *, int);
extern void telnet_putc(conntype *, uchar);
extern int telnet_puts(conntype *, uchar *, int);
extern void telnetuser_optiondone(conntype *, ulong, ulong, ulong);
extern void tn_optionstate(conntype *, uchar);
extern void tn_sendsub(conntype *, int, int, char *);
extern void tn_suboption(conntype *, int);
extern boolean telnet_alive(conntype *);
extern int telnet_unread(conntype *);
extern conntype *telnet_multiproto_open(parseinfo *, boolean *);
extern void telnet_conn_acct(conntype *, acctinfo *);
extern addrtype *telnet_source_address(idbtype *, addrtype *);
extern void tn_sendsubcnt(conntype *, int, int, char *, int);
extern void telnet_suboption_pad_iac(char *, int, char **, int *);
extern boolean tty_status_ok(conntype *, tcbtype *, tt_soc *, long, int *); 

#endif __TELNET_H__
