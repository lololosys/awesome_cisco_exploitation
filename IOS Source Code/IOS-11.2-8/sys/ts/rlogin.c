/* $Id: rlogin.c,v 3.9.6.9 1996/08/13 21:08:08 che Exp $
 * $Source: /release/112/cvs/Xsys/ts/rlogin.c,v $
 *------------------------------------------------------------------
 * rlogin.c -- Unix remote login service
 *
 * April 1987, Greg Satz
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rlogin.c,v $
 * Revision 3.9.6.9  1996/08/13  21:08:08  che
 * CSCdi65824:  kerberos client mandatory broken for rsh
 * Branch: California_branch
 * Make sure non-Kerberized clients fail if "kerberos clients mandatory"
 * is configured.
 *
 * Revision 3.9.6.8  1996/07/30  01:47:32  billw
 * CSCdi49556:  rlogin does not support alternate tcp destination ports
 * Branch: California_branch
 *
 * Revision 3.9.6.7  1996/06/21  00:08:13  che
 * CSCdi60925:  exec cannot utilize kerberos
 * Branch: California_branch
 * Make sure Kerberos error messages are printed out only when
 * Kerberos is configured.
 *
 * Revision 3.9.6.6  1996/06/12  14:42:59  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.9.6.5  1996/06/06  21:42:21  che
 * CSCdi57954:  router crashes attempting rlogin using kerberos
 * Branch: California_branch
 * Print out error message if reg_invoke_kerberos_stackcheck() returns
 * FALSE.
 *
 * Revision 3.9.6.4  1996/06/04  08:37:42  che
 * CSCdi57424:  router crashes attempting kerberos telnet
 * Branch: California_branch
 * Added kerberos_stackcheck() registry to ensure correct stacksize of
 * process running kerberos clients.
 *
 * Revision 3.9.6.3  1996/05/04  02:08:32  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.9.6.2  1996/04/19  15:34:56  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.10  1996/04/16  03:57:55  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.1.6.9  1996/04/09  22:06:25  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.8  1996/03/30  03:33:15  carrel
 * cvs fixes
 *
 * Revision 2.1.6.7  1996/03/19  08:49:45  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.6  1996/03/12  03:11:24  che
 * Branch: IosSec_branch
 * Phase 2 Kerberos commit Part 2
 *
 * Revision 2.1.6.5  1996/03/05  04:45:09  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.1.6.4  1996/02/21  04:16:17  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.3  1996/01/23  06:55:54  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.2  1995/12/19  04:21:35  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.1  1995/11/17  18:39:12  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 * Revision 3.9.6.1  1996/03/18  22:20:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.6.3  1996/03/16  07:44:59  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.6.2  1996/03/07  10:58:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.6.1  1996/02/20  21:25:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/03/05  08:00:16  billw
 * CSCdi49552:  RADIUS support incomplete in rlogin trusted... commands
 *
 * Revision 3.8  1996/02/27  22:25:28  jenny
 * CSCdi44663:  ASM line stuck in Carrier Dropped state.
 * Don't block in TCP on write unless user spcified so to avoid possible
 * lost control of the tty during TCP close.
 *
 * Revision 3.7  1996/01/24  20:42:06  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.6  1996/01/12  02:41:24  billw
 * CSCdi44858:  use #define for AAA attribute values
 * Bring the AV pairs used up to spec level...
 *
 * Revision 3.5  1996/01/11  02:34:58  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * changed registry function ip_tuba_move_address to inline
 * ip_copy_address().
 *
 * Revision 3.4  1995/12/28  18:30:22  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreign addresses
 *   to tcp_open() for future portability.
 *
 * Revision 3.3  1995/12/13  02:59:46  billw
 * CSCdi42536:  aaa_acct not proper subsystem
 * Move some code around, change some calls to reg_invokes, etc.
 *
 * Revision 3.2  1995/11/17  18:55:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:33:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/10/14  01:10:14  billw
 * CSCdi40773:  AAA connection accounting missing AVpairs for rlogin
 *
 * Revision 2.7  1995/08/02  06:39:27  irfan
 * CSCdi38059:  resuming rlogin connections garbles async data on Copans
 * rlogin connections should start in cooked mode (flow sw). Avoid playing
 * with the flow control, if HW flow control is enabled on the line, for
 * both rlogin and telnet connections.
 *
 * Revision 2.6  1995/08/01  23:09:55  irfan
 * CSCdi37754:  rlogin should allow various sources of remote username
 * Allow a /user switch for rlogin command. Also let the current username
 * be passed as remote username, if rlogin trusted-remoteuser-source is
 * local. Besides, send the local username or "ciscoTS" if the current
 * username was authenticated via one of trusted methods.
 *
 * Revision 2.5  1995/07/14  07:03:20  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.4  1995/06/22 05:51:24  ahh
 * CSCdi36259:  RLOGIN: pending output not flushed correctly (regress)
 *
 * Revision 2.3  1995/06/20  22:02:24  ahh
 * CSCdi31398:  Rlogin sending window size packet twice
 * Eliminate race conditions in rlogin's urgent-data processing.
 *
 * Revision 2.2  1995/06/20  03:45:10  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.1  1995/06/07  23:10:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "packet.h"
#include "ttysrv.h"
#include "connect.h"
#include "config.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "../tcp/telnet.h"
#include "name.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../snmp/sr_old_ltsmib.h"
#include "rlogin.h"
#include "../ip/ip_registry.h"
#include "../parser/parser_defs_exec.h"
#include "aaa.h"
#include <ciscolib.h>
#include <string.h>

#include "../kerberos/include/csfc5/rfc1510.h"

#define	MAXBYTES 400			/* reschedule after this amount */

#define FLUSHREAD	0x01
#define	FLUSHWRITE	0x02
#define	NOSTOP		0x10
#define	DOSTOP		0x20
#define	WINDOW		0x80

struct windowupdate {
    uchar magic[4];
    ushort rows;
    ushort cols;
    ushort xpixels;
    ushort ypixels;
};

/*
 * Local storage
 */

connvector rlogin_connvector;
static int rlogin_trusted_ruser_source;
static int rlogin_trusted_luser_source;

/*
 * External declarations
 */
extern int tcp_chunksize;
extern void telnet_connect_command(parseinfo *csb);
extern void rlogin_chain_init(void);

/*
 * Forward declarations
 */
static char *rlogin_get_local_username(void);
static char *rlogin_get_remote_username(conntype *, boolean *, char *, int);
static void rlogin_conn_acct(conntype *, acctinfo *);

/*
 * rlogin_supported
 * Determine if rlogin is supported in the image.
 * This is a stub function; on images without rlogin, the registry
 * code will return FALSE since this function would not be registered.
 */

static boolean rlogin_supported (void)
{
    return(TRUE);
}

/*
 *
 */
static void put_window (
    conntype *conn)
{
    struct windowupdate update;

    update.magic[0] = update.magic[1] = 0xFF;
    update.magic[2] = update.magic[3] = 's';
    update.rows = update.xpixels = conn->tty->tty_length;
    update.cols = update.ypixels = conn->tty->tty_width;
    tcp_putpush((tcbtype *)conn->tcb,TRUE,FALSE);
    (void) tcp_putstring((tcbtype *)conn->tcb,(uchar *)&update,sizeof update);
    tcp_putpush((tcbtype *)conn->tcb,TRUE,FALSE);
}

/*
 * rlogin_oob
 * Perform out of band services request by remote host
 */

static int rlogin_oob (conntype *conn, tcbtype *tcb, uchar *dataptr,
		       int datacount)
{
    register tt_soc *tty;
    register uchar ch;
    boolean forced = FALSE;
    int flush = 0;

    /*
     * A problem exists in that we may enter the OOB state before
     * receiving the OOB data. In this case we assume that the
     * OOB command will be a flush.
     */
    if (tcb->oobbyte == -1) {
	ch = FLUSHWRITE;
	forced = TRUE;
    } else {
	ch = tcb->oobbyte;
	tcb->oobbyte = -1;	/* Taken the byte */
    }
    tty = conn->tty;
    if (ch & NOSTOP) {
	conn->status &= ~CONN_FLOW;
	conn_setsflow(tty, FALSE);
    }
    if (ch & DOSTOP) {
	conn->status |= CONN_FLOW;
	conn_setsflow(tty, TRUE);
    }
    if (ch & FLUSHREAD)
	clearinput(tty);
    if (ch & FLUSHWRITE) {
 	clearoutput(tty);
	/*
	 * Figure out how much of the current segment to flush.
	 */
	if (tcb->oobptr && tcb->oobptr >= dataptr &&
	    tcb->oobptr < (dataptr + datacount))
	    flush = tcb->oobptr - dataptr;
	else
	    flush = datacount;
    }
    if (ch & WINDOW)
	put_window(conn);

    /*
     * Do debugging output here so flushes don't affect us
     */
    if (conn->debug)
        printf("\nRLOGIN: byte %x%s", ch, forced ? " (forced)" : "");
    if ((ch & WINDOW) && conn->debug)
	printf("\nRLOGIN: Window size request, sent %dx%d",
	       tty->tty_length, tty->tty_width);
    if ((ch & NOSTOP) && conn->debug)
	printf("\nRLOGIN: no stop");
    if (ch & DOSTOP && conn->debug)
	printf("\nRLOGIN: do stop");
    if ((ch & FLUSHREAD) && conn->debug)
	printf("\nRLOGIN: flush read");
    if ((ch & FLUSHWRITE) && conn->debug)
	printf("\nRLOGIN: flush write");
    return(flush);
}

/*
 * rlogin
 * Perform rlogin protocol stuff on the given connection.
 */

static int rlogin (
    register conntype *conn, boolean dummy)
{
    register tcbtype *tcb = (tcbtype *)(conn->tcb);
    register tt_soc *tty = (tt_soc *)(conn->tty);
    register short n;
    long status, pushflag, oldcaps;
    int datacount;
    uchar *dataptr;
    char *p, buffer[80];
    long ch;
    int to_flush;
    boolean success;
    int code;

    tcb->flags |= TCB_EDITURG;		/* remove urgent data from stream */
    clearnotices (tty);			/* reset notification bits */
    TIMER_START(conn->notetime, 30*ONESEC);/* init notify timer */
    if (conn->loginstring) {		/* wake magic fingers */
	GET_TIMESTAMP(conn->logintimer);
    } else
	TIMER_STOP(conn->logintimer);
    GET_TIMESTAMP(conn->idletime);	/* no idle time yet */
    if (tty->tty_sessiontimeout) {
	start_sessiontimeout(conn, tty);
    } else
	TIMER_STOP(conn->sessiontimer);	/* no session time out yet */
    tty->statbits &= ~IDLETIMEOUT;	/* ... */
    tcp_idle(tcb,FALSE);		/* we're not idle any more */
    pushflag = FALSE;			/* not sending data yet */
    oldcaps = tty->capabilities;	/* save original caps word */    


    /*
     * Use Kerberos authentication if connection to a Kerberized rlogind
     */
    if (reg_invoke_kerberos_is_running() && reg_invoke_kerberos_stackcheck()
	&& stdio && stdio->ustruct && stdio->ustruct->cred
	&& (stdio->ustruct->cred->cred_type == KERBEROS) 
	&& (tcb->foreignport == KLOGIN_PORT)) {

	(void) tcp_putbyte(tcb, '\0', TRUE);    /* 1st NULL byte */

	p = tty->termtype ? tty->termtype : "network";
	ch = tty->tty_txspeed ? tty->tty_txspeed : ASYNC_DEFSPEED;
	n = sprintf(buffer, "%s/%d", p, ch);
	if (!reg_invoke_kerberos_client_auth(tcb, tty, &conn->address, 
	    (long) CSFC5c_APOPTS_MUTUAL_REQUIRED_m, 
	    rlogin_get_local_username(), rlogin_get_local_username(), 
	    buffer, NULL, 0)) {
	    status = tcp_lsterr(tcb);
	    if (status == OK)
		status = END_OF_FILE;
	    conn_restoreflow(tty, oldcaps);
	    return(status);
	}
	edisms((blockproc *)telnetBLOCK, (ulong)conn);
	conn->status |= CONN_SETUP;
	conn->status |= CONN_FLOW;   /* Client starts in Cooked mode */
    }

    if ((conn->status & CONN_SETUP) == 0) {
	/*
	 * Send local user, remote user and terminal type
	 */
	if (conn->loginstring) {
	    /*
	     * if there is a login-string, get the username from there.
	     * Otherwise, ask for it from the user
	     */
	    p = buffer;
	    for (datacount = sizeof(buffer) - 1; datacount; datacount--)
		if ((*p = *conn->loginstring++) == RETURN) {
		    *p = '\0';
		    if (*conn->loginstring == LINEFEED)
			conn->loginstring++; /* Skip the linefeed too! */
		    break;
		} else {
		    if (*p == '%') { /* check for escapes */
			/*
			 * Handle some of the % escapes here, so that the
			 * same loginstring can be used for both telnet and
			 * rlogin.  Don't actually pause for %p and %w, but
			 * filter them out.  Except for %% and %m, other
			 * escapes don't make any sense here, and get skipped
			 */
			do {
			    ch = *conn->loginstring++;	/* skip next char */
			} while (ch >= '0' && ch <= '9');

			if (ch == 'm' || ch == 'M') { /* Return? */
			    *p = '\0';
			    break;
			} else if (ch != '%')
			    continue; /* Overwrite % with next char. */
		    }
		    p++;
		}
	    printf("login: %s\n", buffer);
	} else {
	    rlogin_get_remote_username(conn, &success,
				       buffer, sizeof(buffer)-1);
	    if (!success) {
/*		return (REJECTED); */  /* allow NULL ruser */
	    }
	}
	if (! (*buffer)) {          /* No remote username ? */
/*	    return (REJECTED);*/
	}

	if (conn->debug) {
	    printf("\nRLOGIN: local username is: %s\n",
		   rlogin_get_local_username());
	    printf("\nRLOGIN: remote username is: %s\n",
		   buffer);
	}

	(void) tcp_putbyte(tcb, '\0', TRUE);    /* 1st NULL byte */

	for (p = rlogin_get_local_username();
	     ((p) && (*p)); p++)                /* Client username */
	    (void) tcp_putbyte(tcb, *p, TRUE);
	(void) tcp_putbyte(tcb, '\0', TRUE);	/* End NULL byte */

	for (p = buffer; *p; p++)               /* Remote username */
	    (void) tcp_putbyte(tcb, *p, TRUE);
	(void) tcp_putbyte(tcb, '\0', TRUE);    /* End NULL byte */

	p = tty->termtype ? tty->termtype : "network";
	ch = tty->tty_txspeed ? tty->tty_txspeed : ASYNC_DEFSPEED;
	n = sprintf(buffer, "%s/%d", p, ch);
	buffer[n] = '\0';
	for (p = buffer; *p; p++)
	    (void) tcp_putbyte(tcb, *p, TRUE);
	(void) tcp_putbyte(tcb, '\0', TRUE);
	tcp_putpush(tcb,TRUE,FALSE);	/* write data to the net */
	edisms((blockproc *)telnetBLOCK, (ulong)conn);
	conn->status |= CONN_SETUP;
	conn->status |= CONN_FLOW;   /* Client starts in Cooked mode */
    }
    if (conn->status & CONN_FLOW) {
	conn_setsflow(tty, TRUE);
    } else {
	conn_setsflow(tty, FALSE);
    }
    while (TRUE) {
	edisms((blockproc *)telnetBLOCK, (ulong)conn);
	if (!tty_status_ok(conn, tcb, tty, oldcaps, &code))
	    return (code);
	/*
	 * Handle input from the net (output to the tty)
 	 */
	n = MAXBYTES;
	if (tcp_unread(tcb) || tcp_closing(tcb)) {
	    while (n > 0) {
	        status = tcp_getstring(tcb, &dataptr, &datacount,
			tcp_chunksize ? tcp_chunksize : MAXINT);
		if (status == NO_DATA)
		    break;
	        switch (status) {
		    case URGENT_DATA:
			to_flush = rlogin_oob(conn, tcb, dataptr, datacount);
			datacount -= to_flush;
			dataptr += to_flush;
			if (datacount == 0)
			    break;
			/*FALLTHROUGH*/
		    case OK:
			if (tty->tty_sessiontimeout &&
			    (tty->capabilities & OUTIDLE))
			    start_sessiontimeout(conn, tty);
			/* 
			 * Before we start the rlogin I/O loop we need
			 * to get the NUL Ack from the remote server.
			 * See RFC 1282. Flush all other input till
			 * that time.
			 */
			if (!conn->proto.rlogin_info.ack_seen) {
			    while (datacount) {
				datacount--;
				if (*dataptr++ == '\0') {
				    conn->proto.rlogin_info.ack_seen = TRUE;
				    break;
				}
			    }
			    if (!datacount)
				break;
			}
			status = putpkt(tty, tcb->curinputpak, dataptr,
					datacount);
			datacount -= status; /* Get bytes not moved */
			if (datacount)
			    tcp_putback(tcb, datacount); /* Put back data */
			if (!status)
			    n = -1; /* Nothing output, don't loop */
			else
			    n -= status;
			break;
		    case NO_DATA:
			break;
		    default:
		        status = tcp_lsterr(tcb);
		        if (status == OK)
			    status = END_OF_FILE;
			conn_restoreflow(tty, oldcaps);
	    	        return(status);
		}
		if (inputpending(tty))
		    break;
	    }
	    (void)startoutput(tty);		/* ensure output is running */
	}

	/*
	 * handle and Macro magic here.
	 */
	if (conn->loginstring) {
	    if (magic_fingers(conn)) {
		tcp_putpush(tcb,TRUE,FALSE);
		continue;		/* continue without checking tty */
	    } else
		pushflag = TRUE;
	}

	/* 
	 * Now handle input from the tty (output to the net).
	 */
	if (inputpending(tty)) {
	    GET_TIMESTAMP(conn->idletime);
	    if (tty->tty_sessiontimeout) 
		start_sessiontimeout(conn, tty);
	    n = MAXBYTES;
	    if (tty->tty_dispatchtimeout) {
		if (!TIMER_RUNNING(conn->dispatchtimer))
		    TIMER_START(conn->dispatchtimer, tty->tty_dispatchtimeout);
	    } else
		pushflag = TRUE;
	    while (--n) {
		ch =  getc (tty);	/* get byte from tty */
		if (ch == -1)	/* break if no tty data */
		    break;
		if (tty->tty_editinfo.dispatch[ch & 0x7F] & TTY_DISP_DISP)
		    pushflag = TRUE;
		if (tcp_putbyte(tcb, ch, FALSE) == FALSE) {
		    ungetc(tty,ch);
		    break;
		}
	    }
	}
	if (pushflag || TIMER_RUNNING_AND_AWAKE(conn->dispatchtimer)) {
	    tcp_putpush(tcb,TRUE,FALSE);
	    pushflag = FALSE;
	    TIMER_STOP(conn->dispatchtimer);
	}
    }
}

/*
 * rlogin_open_connection - Open a connection for RLOGIN
 */

static conntype *rlogin_open_connection (
     nametype *nameptr,
     ipaddrtype foreignhost,
     ushort dportno,
     boolean debugflag,
     boolean *abortflag,
     int remote_uname_valid,
     char *remote_uname)

{
    conntype *conn;
    ushort lport;
    tcbtype *tcb;
    ipaddrtype fhost = ADDR_ILLEGAL;
    addrtype faddr;
    int i;
    int status;
    int use_kerb = 0;

    /*
     * First see if we want to use Kerberos
     */
    if (reg_invoke_kerberos_is_running() && reg_invoke_kerberos_stackcheck()
	&& stdio && stdio->ustruct && stdio->ustruct->user
	&& stdio->ustruct->cred
	&& (stdio->ustruct->cred->cred_type == KERBEROS)) {
	use_kerb = 1;
	if (dportno == RLOGIN_PORT) /* only change default port */
	    dportno = KLOGIN_PORT;
    } else {
	if (reg_invoke_kerberos_is_running() 
	    && !reg_invoke_kerberos_stackcheck()) {
	    printf("Kerberos was enabled after this 'exec' process was forked."
		   "\nLog off and log back in to utilize Kerberos.\n");
	}
    }

    /*
     * Next, check to see if we HAVE to use Kerberos.
     */
    if (reg_invoke_kerberos_use_kerb_clients() && !use_kerb) {
	printf("Only Kerberized login connections are allowed on this host.\n");
	return(NULL);
    }

    /*
     * First, try to get a connection block
     */

    conn = getconnection(stdio, &rlogin_connvector);
    if (!conn) {
	printf(nomemory);
	return(NULL);
    }

    conn->debug = debugflag;

    lport = IPPORT_RESERVED - 1;
    if (!tcp_reservedport(&lport)) {
	printf("%% No reserved ports");
	return(NULL);
    }

    do {
	if ((use_kerb) && (dportno == RLOGIN_PORT)) {
	    use_kerb = 0;
	}

	for (i = 0, tcb = NULL; i < MAXIPADDRESSES; i++) {
	    if (nameptr && !nameptr->v.ip.adr[i].addr.ip_address)
		break;
	    /*
	     * Print out message indicating what we're doing
	     */
	    if (nameptr) {
		if (nameptr->busystring == NULL) {
		    if (i != 0)
			printc('\n');
		    printf("Trying %s (%i)... ", nameptr->name, 
		   	nameptr->v.ip.adr[i].addr.ip_address);
		}
	    } else {
		printf("Trying %i...", foreignhost);
	    }
	    flush();
	
	    /*
	     * Perform access checking on the current address.
	     */
	    fhost = nameptr ? nameptr->v.ip.adr[i].addr.ip_address :foreignhost;
	    if (!check_access(stdio, fhost, lport, dportno, TRUE) ||
		!reg_invoke_login_connect(fhost, dportno)) {
		printf("\n%% Connections to that host not permitted from this "
			"terminal");
		if (nameptr)
		    continue;
		freeconnection(conn);
		return(NULL);
	    }

	    /*
	     * Try making the connection.  Check for martians.
	     */
	    if (martian(fhost, TRUE)) {
		printf("%%invalid host address");
		if (nameptr)
		    continue;
		freeconnection(conn);
		return(NULL);
	    }

	    faddr.type = ADDR_IP;
	    faddr.ip_addr = fhost;

	    tcb = tcp_open(stdio, dportno,&faddr,lport,NULL,&status,NULL);
	    if (!tcb) {
		if (status == FOREIGN_RESET || status == TIMEDOUT) {
		    while (tcb_findport((ushort)--lport) 
			&& lport > MAXPRIVEDPORT)
		      ;
		    if (lport > MAXPRIVEDPORT) {
			i--;
			continue;
		    }
		}
		if (nameptr && nameptr->busystring)
		    printf("%s", nameptr->busystring);
		else
		    printf("%s\n", tcperror2string(status));
		if (status == LOCAL_RESET) {
		    if (abortflag)
		      *abortflag = TRUE;
		    break;
		}
	    }
 	    if (tcb || !nameptr)
	      break;
	}

	if (use_kerb) {
	    if ((!tcb) && (!reg_invoke_kerberos_use_kerb_clients())) {
		if (dportno == KLOGIN_PORT)
		    dportno = RLOGIN_PORT; /* Only switch back if default */
		printf("Kerberos:\tConnection unreachable.  Using normal "
			"rlogin...\n");
	    } else {
		use_kerb = 0;
	    }
	}
    } while (use_kerb);

    /*
     * If we failed to make a connection, flush connection block
     * and report error.
     */
    if (!tcb) {
	freeconnection(conn);
	return(NULL);
    }

    /*
     * We succeeded.  Finish connection setup.
     */

    if (nameptr == NULL || nameptr->busystring == NULL)
	printf("Open\n");

    conn->hostname[0] = conn->logicalname[0] = '\0';
    conn->tcb = tcb;
    conn->proto.rlogin_info.ack_seen = FALSE;
    if (remote_uname_valid) {
	sstrncpy(conn->proto.rlogin_info.remote_uname,
		 remote_uname, REMOTE_UNAME_LEN);
    }
    ip_copy_address(fhost, &conn->address);

    return(conn);
}

/*
 * rlogin_connect_command - Handle the connect command for rlogin
 *
 * This routine handles the connect command for the rlogin protocol.
 */

static void rlogin_connect_command (parseinfo *csb)
{
    int dummy;
    nametype *nameptr;
    boolean debug;
    conntype *newcon;
    addrtype address;
    ushort dport;

    /*
     * We need to chain into telnet if,
     * .... /line option is specified or
     * .... /stream option is specified or
     * .... tn3270 connect is desired
     * Note we don't want to mutually recurse if no port was
     *      specified and telnet chained into us bcaz of name
     *      struct recommending RLOGIN_PORT
     */

    if (GETOBJ(int,4) || GETOBJ(int,5) ||
	(CMD_CONNECT_TN3270 == GETOBJ(int,9))) {

	/*
	 * We can only be here, if the connect_command(), found
	 * CMD_CONNECT_RLOGIN. We should be able to safely chain
	 * into telnet, w/o fearing mutual recursion.
	 */
	telnet_connect_command(csb);
	return;
    }

    /*
     * See if this connect is allowed
     */

    if (!connect_allowed(&rlogin_connvector))
	return;

    /*
     * Attempt to look up the address in dotted format
     */

    if (numeric_lookup(ADDR_IP, GETOBJ(string, 1), &address)) {
	/* Did it look like an IP address? */
	if (address.ip_addr) {	
	    if (martian(address.ip_addr, TRUE)) { /* Yes, is it valid? */
		printf("%% Invalid IP address");
		return;
	    }
	}
	nameptr = NULL;
    } else {

	/*
	 * Attempt to look up the address in the name cache
	 */

	psion(stdio);		/* Enable interrupts */
	nameptr = name_lookup(GETOBJ(string, 1), &dummy, NAM_IP);
	if (!nameptr) {
	    if (!psipending())
		printf(connect_unknown);
	    psioff(stdio);
	    return;
	}
	psioff(stdio);
    }

    /*
     * At this point, we have either an IP address in address.ip_addr
     * or a name block pointer in nameptr.
     */

    debug = GETOBJ(int, 3);	/* Get the debugging flag */
    if (GETOBJ(int,1))
	dport = GETOBJ(int,1);
    else
	dport = RLOGIN_PORT;
    /*
     * The name ptr should be locked against being freed and filled
     * with poison. CSCdi23760 caused a crash where busy-string looked
     * good. Note since none of the above code blocked, so it should
     * still be OK to do the locking here.
     */
    if (nameptr)
	nameptr->sharecount++;

    /*
     * Attempt to open the connection
     */

    newcon = rlogin_open_connection(nameptr, address.ip_addr, dport, debug,
				    &dummy, GETOBJ(int,10),
				    GETOBJ(string,2));

    if (!newcon) {
	if (nameptr) {
	    nameptr->flags &= ~NAM_RLOGIN;	/* rlogin didn't work */
	    nameptr->sharecount--;
	}
	return;
    }

    if (nameptr) {
	name_valid(nameptr, TRUE);	/* Say cache entry is good */
	nameptr->flags |= NAM_RLOGIN;	/* rlogin did work */
	newcon->loginstring = nameptr->loginstring; /* remember any */
                                                    /* login string */
    }

    connect_new_session(newcon, GETOBJ(string, 1), csb);
					 /* Start this new session */
    if (nameptr)
	nameptr->sharecount--;
}

/*
 * rlogin_multiproto_open - Handle session open for connect command
 *
 * This routine is used to handle rlogin connections when the user has not
 * specified an explicit protocol.
 */

static conntype *rlogin_multiproto_open (
     parseinfo *csb,
     boolean *abortflag)
{
    nametype *nameptr;
    int dummy;
    conntype *newcon;
    addrtype address;
    boolean debug;

    /*
     * Attempt to look up the address in dotted format
     */

    if (numeric_lookup(ADDR_IP, GETOBJ(string, 1), &address)) {
	/* Did it look like an IP address? */
	if (address.ip_addr) {	
	    if (martian(address.ip_addr, TRUE)) { /* Yes, is it valid? */
		return(NULL);	/* No, indicate failure */
	    }
	}
	nameptr = NULL;

    } else {

	/*
	 * Attempt to look up the address in the name cache
	 */

	psion(stdio);		/* Enable interrupts */
	nameptr = name_lookup(GETOBJ(string, 1), &dummy, NAM_IP);
	if (!nameptr) {
	    if (psipending())
	      *abortflag = TRUE;
	    psioff(stdio);
	    return(NULL);
	}
	psioff(stdio);
	/*
	 * Check if rlogin has been specificially disallowed for this name
	 * by setting up the hostname with a different tcp port (eg telnet)
	 */
	if (nameptr->v.ip.port && (nameptr->v.ip.port != RLOGIN_PORT))
	    return(NULL);

	if (!(nameptr->flags & NAM_RLOGIN))
	    return(NULL);	/* RLOGIN has failed before */
    }

    debug = GETOBJ(int,3);

    /*
     * The name ptr should be locked against being freed and filled
     * with poison. CSCdi23760 caused a crash where busy-string looked
     * good. Note since none of the above code blocked, so it should
     * still be OK to do the locking here.
     */

    if (nameptr)
	nameptr->sharecount++;

    newcon = rlogin_open_connection(nameptr, address.ip_addr, RLOGIN_PORT,
				    debug, abortflag, FALSE, NULL);

    if (!newcon) {
	if (nameptr)
	    nameptr->flags &= ~NAM_RLOGIN; /* Don't try RLOGIN again */
    } else {
	/* we succeeded here..remember to set the login-string pointer */
	if(nameptr){
	    name_valid(nameptr,TRUE);      /* cache entry is definitely */
                                           /*  valid */
	    newcon->loginstring = nameptr->loginstring; /* remember any */
                                                        /* login string */
	}
    }
    if (nameptr)
	nameptr->sharecount--;
    return(newcon);
}


static char *rlogin_get_lname_local (void)
{

    if (old_access_control) {
	return (stdio->user);
    } else {
	if (stdio->ustruct) {
	    return (stdio->ustruct->user);
	}
    }
    return (NULL);
}

static char *rlogin_get_lname_tacacs (void)
{

    if (old_access_control) {
	/*
	 * this is pretty lame. We think that this
	 * user must have passed TACACS authentication,
	 * since he is logged on this line and the line
	 * has "login tacacs" set.  This assumption may
	 * not be true in some rare cases.
	 */
	if (AC_TACACS == stdio->actype) {
	    return stdio->user;
	}

    } else {

	if (stdio->ustruct &&
	    (AAA_ACC_METH_TACACSPLUS == stdio->ustruct->acc_method)) {

	    return (stdio->ustruct->user);
	}
    }

    return (NULL);
}

static char *rlogin_get_lname_radius (void)
{
    if (stdio->ustruct &&
	(AAA_ACC_METH_RADIUS == stdio->ustruct->acc_method)) {
	    return (stdio->ustruct->user);
	}
    return (NULL);
}

/*
 * rlogin_get_remote_name_local
 *
 * fills up the buffer with the  current logged in username
 *    in absence of which it fails.
 *    Allow the user to specify a different usename using /user
 */
static void rlogin_get_remote_name_local (conntype *conn, boolean *success,
				   char *buffer, int buffsize,
				   boolean signon_name)

{
    char *remote_name = conn->proto.rlogin_info.remote_uname;

    *success = FALSE;

    /*
     * We try to guess what remote username the use wants. 
     * The order is:  /user
     *                local sign-on name
     *                query the user
     */

    if (buffer) {

	buffer[0] = '\0';

	/*
	 * No   /user switch ?  Try the local sign on name
	 */
	if (signon_name && ((!remote_name) || (!*remote_name))) {
	    remote_name = rlogin_get_lname_local();
	}
	if (remote_name && (*remote_name) &&
	    ((strlen(remote_name)+1) < buffsize)) {
	    
	    strcpy(buffer, remote_name);

	} else {
	    printf("login: ");                    /* Get it from user */
	    (void) rdtty(buffer, buffsize);
	    flush();
	}
	if (*buffer) {
	    *success = TRUE;
	}
    }
}


static void rlogin_trusted_host_missing (conntype *conn, boolean *success,
				  char *buffer, int buffsize)

{
    *success = FALSE;
    if (buffer) {
	buffer[0] = '\0';
    }
    return;
}

static char *rlogin_get_remote_username (conntype *conn,
					 boolean *success,
					 char *buffer, int buffsize)
{
    switch (rlogin_trusted_ruser_source) {

    case RLOGIN_TRUSTEDHOST_TACACS:
    case RLOGIN_TRUSTEDHOST_RADIUS:
	rlogin_trusted_host_missing(conn, success, buffer, buffsize);
	break;

    case RLOGIN_TRUSTEDHOST_LOCAL:
	rlogin_get_remote_name_local(conn, success, buffer,
				     buffsize, TRUE);
	break;

    case RLOGIN_TRUSTEDHOST_DEFAULT:
    default:
	rlogin_get_remote_name_local(conn, success, buffer,
				     buffsize, FALSE);
	break;
    }
    return buffer;
}

/*
 * rlogin_trusted_auth_method_command
 * define which one of the methods are allowed to
 * discover a remote username for the rlogin command.
 *    Exclusively one at a time.
 */
void rlogin_trust_remoteuser_command (parseinfo *csb)
{

    if (csb->nvgen) {
	nv_write(((rlogin_trusted_ruser_source !=
		   RLOGIN_TRUSTEDHOST_DEFAULT) &&
		  (csb->which == rlogin_trusted_ruser_source)),
		 "%s", csb->nv_command);
	return;
    }

    rlogin_trusted_ruser_source = csb->which;

    if ((RLOGIN_TRUSTEDHOST_TACACS == rlogin_trusted_ruser_source) ||
	(RLOGIN_TRUSTEDHOST_RADIUS == rlogin_trusted_ruser_source)) {

	rlogin_trusted_ruser_source = RLOGIN_TRUSTEDHOST_DEFAULT;
	printf("%% Unsupported rlogin trusted-authentication-method");
    }
}

/*
 * rlogin_get_local_username
 *
 * Return the "local" username to be used for rlogin
 * connections. The default is "ciscoTS"
 * If configured for local we will try to use the current
 * sign-on name.
 * If configured for TACACS, we will try to use the current
 * sign-on name if authenticated via TACACS
 * If configured for RADIUS, we will try to use the current
 * sign-on name if authenticated via RADIUS.
 *
 * NOTE:  This routine MUST return a valid string.
 */
static char *rlogin_get_local_username (void)
{

    char *lname = NULL;
    static char RLOGIN_LOCAL_HOSTNAME[] = {"ciscoTS"};


    if (rlogin_trusted_luser_source & RLOGIN_TRUSTED_LNAME_LOCAL) {
	lname = rlogin_get_lname_local();
    }
    if ((!lname) &&
	(rlogin_trusted_luser_source & RLOGIN_TRUSTED_LNAME_TACACS)) {
	lname = rlogin_get_lname_tacacs();
    }
    if ((!lname) &&
	(rlogin_trusted_luser_source & RLOGIN_TRUSTED_LNAME_RADIUS)) {
	lname = rlogin_get_lname_radius();
    }
    if ((!lname) || !(*lname)) {
	lname = RLOGIN_LOCAL_HOSTNAME;
    }
    return lname;
}

/*
 * rlogin_trusted_localuser_source_command
 * define which one of the sources of local usernames
 * are valid.
 */
void rlogin_trusted_localuser_source_command (parseinfo *csb)
{

    if (csb->nvgen) {

	nv_write((rlogin_trusted_luser_source &
		  RLOGIN_TRUSTED_LNAME_LOCAL),
		 "%s %s", csb->nv_command, "local");
	nv_write((rlogin_trusted_luser_source &
		  RLOGIN_TRUSTED_LNAME_TACACS),
		 "%s %s", csb->nv_command, "tacacs");
	nv_write((rlogin_trusted_luser_source &
		  RLOGIN_TRUSTED_LNAME_RADIUS),
		 "%s %s", csb->nv_command, "radius");
	return;
    }

    if  (csb->sense) {
	rlogin_trusted_luser_source |= csb->which;
    } else {
	rlogin_trusted_luser_source &= ~csb->which;
    }
}


/*
 * rlogin_init - Initialization handling
 */

void rlogin_init (void)
{
    conn_vector_init(&rlogin_connvector, "rlogin", "Unix rlogin protocol");

    rlogin_trusted_ruser_source = RLOGIN_TRUSTEDHOST_DEFAULT;
    rlogin_trusted_luser_source = 0;

    rlogin_connvector.snmp_sesstype = D_tslineSesType_rlogin;
    rlogin_connvector.valive = telnet_alive;
    rlogin_connvector.vrun = rlogin;
    rlogin_connvector.vclose = close_connection;
    rlogin_connvector.vunread = telnet_unread;
    rlogin_connvector.vopen = rlogin_multiproto_open;
    rlogin_connvector.vaccounting = rlogin_conn_acct;

    reg_add_rlogin_supported(rlogin_supported, "rlogin_supported");
    reg_add_rlogin_connect(rlogin_connect_command, "rlogin_connect_command");
    rlogin_chain_init();
}

/*
 * Store AAA accounting info for rlogin connections.
 */
void rlogin_conn_acct (conntype *conn, acctinfo *acct)
{
    reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_PROTO, AAA_VAL_PROT_rlogin,
				 AAA_TYPE_STR, sizeof AAA_VAL_PROT_rlogin);
    reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_ADDR,
				 &conn->address.addr.ip_address,
				 AAA_TYPE_IP, 4);
    acct->flags |= ACCT_F_FIRSTDONE;
}


