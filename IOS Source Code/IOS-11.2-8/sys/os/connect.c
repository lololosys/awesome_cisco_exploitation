/* $Id: connect.c,v 3.7.18.8 1996/09/06 01:11:27 snyder Exp $
 * $Source: /release/112/cvs/Xsys/os/connect.c,v $
 *------------------------------------------------------------------
 * connect.c -- connection management routines
 *  
 * September 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: connect.c,v $
 * Revision 3.7.18.8  1996/09/06  01:11:27  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.7.18.7  1996/08/28  13:03:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.18.6  1996/08/08  22:42:51  swenson
 * CSCdi65178:  rlogin tried before telnet when making implicit connection
 * Branch: California_branch
 * Explicitly insert telnet before rlogin on the conn_vectorQ, rather than
 * relying on the order of subsystem initialization.
 *
 * Revision 3.7.18.5  1996/08/07  09:01:45  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.7.18.4  1996/07/10  22:21:55  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.7.18.3  1996/06/17  23:35:17  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.7.18.2  1996/03/27  10:44:22  irfan
 * CSCdi33144:  telnet should be configurable to refuse some options
 * Branch: California_branch
 * Add a bit in tty->capabilities to allow telnet/rlogin to silently
 * ignore remote host's flow control change requests.
 *
 * Revision 3.7.18.1  1996/03/18  21:29:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.2  1996/03/07  10:18:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.20.1  1996/02/20  16:38:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/07  16:14:16  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.6  1996/02/03  01:57:49  perryl
 * CSCdi47740:  Service hide-addr hiding is incomplete
 *
 * Revision 3.5  1996/01/24  20:39:37  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 *
 * Revision 3.4  1995/11/20  23:13:22  wmay
 * CSCdi43969:  Line Transport Preferred None not saved in config
 *
 * Revision 3.3  1995/11/17  18:46:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:44:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:18:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/08/02  06:39:45  irfan
 * CSCdi38059:  resuming rlogin connections garbles async data on Copans
 * rlogin connections should start in cooked mode (flow sw). Avoid playing
 * with the flow control, if HW flow control is enabled on the line, for
 * both rlogin and telnet connections.
 *
 * Revision 2.7  1995/08/01  23:09:41  irfan
 * CSCdi37754:  rlogin should allow various sources of remote username
 * Allow a /user switch for rlogin command. Also let the current username
 * be passed as remote username, if rlogin trusted-remoteuser-source is
 * local. Besides, send the local username or "ciscoTS" if the current
 * username was authenticated via one of trusted methods.
 *
 * Revision 2.6  1995/07/25  00:03:10  che
 * CSCdi28688:  Security tightening on product
 * connect.c:
 * Forces "transport input all" to be written out during a NVGEN if
 * tty->transport_allowedin = CONN_TRANSPORT_DEFMASK(default).
 *
 * cfg_line_transport.h:
 * Introduces new configuration command:
 * "transport { input | output } all"
 *
 * Revision 2.5  1995/07/14  07:02:45  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.4  1995/06/30 06:42:23  dstine
 * CSCdi36619:  IDB nits
 *         - remove #include of interface_private.h
 *
 * Revision 2.3  1995/06/21  08:56:11  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.2  1995/06/20  03:44:50  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.1  1995/06/07  21:54:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "packet.h"
#include "ttysrv.h"
#include "sys_registry.h"
#include "connect.h"
#include "name.h"
#include "config.h"
#include "../ui/debug.h"
#include "parser.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_misc.h"
#include "../ui/command.h"
#include "../snmp/sr_old_ltsmib.h"
#include "../os/os_debug_flags.h"
#include "../h/auth_registry.regh"
#include "../ui/service.h"
#include "../os/init.h"
#include "../h/cs_registry.regh"

/*
 * Local data
 */
queuetype conn_vectorQ;		/* Queue of defined connection vectors */
ulong connect_sysdef_transport; /* System default transport */
ulong connect_vectormask;	/* Mask of assigned vector bit numbers */

static boolean conn_vector_compare(connvector *, connvector *);
static boolean insertlist(queuetype *, void *, void *);

/*
 * getconnection
 * Assign a new connection block
 */

conntype *getconnection (register tt_soc *tty, connvector *vector)
{
    register conntype *conn;

    conn = malloc_named (sizeof (conntype), "Connection");
    if (conn) {
	conn->number = fetchnumber (tty);	/* assign number */
	conn->snmp_sesstype = vector->snmp_sesstype; /* set snmp status */
	sortenqueue (&tty->connhead, conn);	/* add to lines conns */
	tty->nconns++;				/* one more for line */
	if ((tty->capabilities & MODEMDTR) && (tty->nconns == 1)) {
	    dtrstate(tty,TRUE);
	    if (modem_debug)
		ttybug(tty,"Raising DTR");
	}
	conn->tty = tty;			/* remember tty pointer */
	GET_TIMESTAMP(conn->idletime);		/* init idle timer */
	conn->v = vector;			/* Set dispatch vector */
    }
    return (conn);
}

/*
 * freeconnection
 * Unqueue and release a connection block.  Note that this may be a
 * terminal driver connection block and hence not in the queue.
 */

void freeconnection (register conntype *conn)
{
    register tt_soc *tty;

    if (!conn)
	return;
    tty = conn->tty;
    if (tty) {
	if (conn == tty->conntop)
	    tty->conntop = NULL;	/* remove from tty driver */
	else
	    unqueue (&tty->connhead, conn);

	/*
	 * if this was the current connection, or if it is the last
	 * connection on the tty, reset dispatch defaults.
	 */
	tty->nconns--;
	if ((conn->status & CONN_CURRENT) || (tty->nconns <= 0)) {
	    if (tty->defaults) {
	        bcopy(&tty->defaults->u_editinfo, &tty->tty_editinfo,
		      sizeof(tty_editdata));
		tty->tty_dispatchtimeout = tty->defaults->u_dispatchtimeout;
	    }
	}
	if ((tty->capabilities & MODEMDTR) && tty->nconns <= 0) {
	    dtrstate(tty,FALSE);
	    if (modem_debug)
		ttybug(tty,"Dropping DTR");
	}
    }
    free (conn);			/* release connection block memory */
}

/* 
 * fetchnumber
 * Find the best connection number.
 */

int fetchnumber (register tt_soc *tty)
{
    register int i = 0;
    register conntype * c;

    c = (conntype *) tty->connhead.qhead;	/* start at top of list */
    if (!c || !c->number)
	return (1);				/* no connections, return 1 */
    while (c) {
	if (++i < c->number)
	    return (i);				/* found a hole, return i+1 */
	c = c->link;
    }
    return (i + 1);				/* use next number */
}

/*
 * findconn
 * Find connection number n for a given tty line
 */

conntype *findconn (register tt_soc *tty, register int n)
{
    register conntype *conn;

    conn = (conntype *) tty->connhead.qhead;
    while (conn)
	if (conn->number == n)
	    break;
	else
	    conn = conn->link;
    return (conn);
}

/*
 * findlogicalname
 * Find logical name and return connection pointer
 */

conntype *findlogicalname (
    register tt_soc *tty,
    register char *name,
    ulong *cnt)
{
    register conntype *conn, *d, *e;
    register int partial = 0, exact = 0;

    *cnt = 0;				/* assume count is zero */
    if (!*name)
	return (NULL);			/* NULL names illegal */
    conn = (conntype *) tty->connhead.qhead;
    e = NULL;
    d = NULL;
    while (conn) {
	if (partialmatch (conn->logicalname, name)) {
	    ++partial;
	    d = conn;
	    if (!strcasecmp(conn->logicalname, name)) {
		++exact;
		e = conn;
	    }
	}
	conn = conn->link;
    }
    /* may have n>1 partial matches and n=1 exact matches */
    *cnt = (exact << 16) | partial;	/* exact,,partial. Each 16 bits */
    if (exact == 1)
	return (e);			/* 1 exact/ ? partial */
    if (partial == 1)
	return (d);			/* 1 partial/exact */
    return (NULL);			/* no unique matches */
}

/*
 * clearnotices
 * Clear the notified bit on all connections of a given line
 */

void clearnotices (register tt_soc *tty)
{
    register conntype *conn = (conntype *) tty->connhead.qhead;
    while (conn) {
	conn->status &= ~CONN_NOTIFIED;
	conn = conn->link;
    }
}

/*
 * sortenqueue
 * Put connection block into queue sorted by increasing connection number
 */

void sortenqueue (register queuetype *q, register conntype *e)
{
    register conntype *c, *prev;
    register int number;

    q->count++;				/* count one more element in queue */
    c = (conntype *) q->qhead;		/* first element or NULL */
    number = e->number;			/* its connection number */
    prev = (conntype *) q;		/* qhead pointer */
    while (c)
	if (number < c->number) {
	    e->link = c;		/* e points to c */
	    prev->link = e;		/* prev points to e */
	    return;
	}
	else {
	    prev = c;
	    c = c->link;
	}
    prev->link = e;	    		/* have a new tail or no head */
    q->qtail = e;
    e->link = NULL;
}

/*
 * assignlogicalname
 * Given a string, set a connection logical name.
 */

void assignlogicalname (register conntype *conn, register char *str)
{
    ulong i;
    register boolean exact;

    if ((str == NULL) || (*str == '\0')) {
 	(void) findlogicalname(conn->tty,conn->hostname,&i);
 	exact = (i >> 16) & 0xFFFF;	/* n Exact matches */
	if (exact)
	    return;			/* EXACT match. Can't use. */
        str = conn->hostname;		/* current default */
    }
    sstrncpy(conn->logicalname, str, CONNLEN+1);
}

/*
 * notification
 * Notify a user of new data on quiescent connections.
 */

void notification (
    register conntype *conn)		/* current connection */
{
    register tt_soc *tty = (tt_soc *) conn->tty; /* controlling terminal */
    register conntype *cp;

    if (!(tty->capabilities & NOTIFY))	/* do only if we want notification */
	return;
    if (tty->statbits & (CARDROP | HANGUP))
	return;				/* line is dead */
    cp = (conntype *) tty->connhead.qhead;
    while (cp) {
	if (cp != conn)			/* ignore current connection */
	    if (!(cp->status & CONN_NOTIFIED))	/* if not notified yet */
		if (conn_unread(cp)) {		/* if we have unread data */
		    cp->status |= CONN_NOTIFIED;/* notify once */
		    printf("\n\07[New data from connection %d, ",cp->number);
		    (void) print_host(cp);
		    printf("]\n");
		}
	cp = cp->link;
    }
}

/*
 * conn_leastidle
 * Return connection pointer to least idle connection
 */

conntype *conn_leastidle (tt_soc *tty)
{
    register conntype *list, *conn;

    list = (conntype *) tty->connhead.qhead;
    conn = list;
    while (list) {
	if (ELAPSED_TIME(list->idletime) < ELAPSED_TIME(conn->idletime))
	    conn = list;
	list = list->link;
    }
    return(conn);
}

/*
 * transport_nvlist - Write a transport list to nvram
 */

static void transport_nvlist (
     parseinfo *csb,
     ulong mask,
     tt_soc *tty)
{
    connvector *connv;

    if (((tty->type == TTY_LINE) || (tty->type == AUX_LINE)) 
	&& (csb->val10 == TELNET_TRANSPORT_IN)) {
		if (mask == 0) {
			return;		/* Nothing to do if default */
		}

		if (mask == CONN_TRANSPORT_DEFMASK) {
			nv_write(TRUE, "%s %s", csb->nv_command, "all");
			return;
		}
    }

    if (mask == CONN_TRANSPORT_DEFMASK)
      	return;			/* Nothing to do if default */

    nv_write(TRUE, "%s", csb->nv_command); /* Write out command prefix */

    if (mask) {
	for (connv = (connvector *)conn_vectorQ.qhead;
	     connv;
	     connv = connv->next) {
	    if (connv->type & mask)
	      nv_add(TRUE, " %s", connv->name);
	}
    } else {
	nv_add(TRUE, " none");
    }
}

/*
 * transport_cmd
 * parse the top level "transport" command.  For interactive use (terminal
 * command) only "preferred" is valid, so do not require this keyword.
 */
void transport_cmd (parseinfo *csb)
{
    connvector *connv;
    tt_soc *tty;
    int i;

    if (csb->nvgen) {
	tt_soc *tty;
	ulong pref;

	tty = MODEMS[csb->line1];
	switch (GETOBJ(int,10)) {
	  case TELNET_TRANSPORT_PREFERRED:

	    pref = tty->defaults ? tty->defaults->u_transport_pref :
		tty->tty_transport_pref;

	    /* 
	     *  If the preference is "lat", set pref to 
	     *    CONN_TRANSPORT_DEFMASK so nvgen will pass it by.
	     */
	    for (connv = (connvector *)conn_vectorQ.qhead; connv;
	    		connv = connv->next) {
		if (strcmp("lat", connv->name) == 0) {
		    if (connv->type == pref) {
			pref = CONN_TRANSPORT_DEFMASK;
		    }
		    break;
		}
	    }

	    transport_nvlist(csb, pref, tty);
	    break;

	  case TELNET_TRANSPORT_IN:
	    transport_nvlist(csb, tty->transport_allowin, tty);
	    break;

	  case TELNET_TRANSPORT_OUT:
	    transport_nvlist(csb, tty->transport_allowout, tty);
	    break;
	}
	return;
    }

    switch (GETOBJ(int,10)) {
      case TELNET_TRANSPORT_PREFERRED:
	if (csb->set_to_default)
	    GETOBJ(int,2) = CONN_TRANSPORT_DEFMASK;
	setuser_integer(csb, GETOBJ(int,2),
			&MODEMS[0]->tty_transport_pref);
	break;
      case TELNET_TRANSPORT_OUT:
	if (csb->set_to_default)
	    GETOBJ(int,2) = CONN_TRANSPORT_DEFMASK;
	setinteger(csb, GETOBJ(int,2),
		   &MODEMS[0]->transport_allowout);
	break;
      case TELNET_TRANSPORT_IN:
	if (csb->set_to_default) {
	    /*
	     * Must iterate locally.  The defaults may be different if
	     * the range encompasses more than one type of line.
	     */
	    for (i = csb->line1; i < csb->line2; i++) {
		tty = MODEMS[i];
		if ((tty->type == TTY_LINE) || (tty->type == AUX_LINE)) {
		    tty->transport_allowin = 0;
		} else {
		    tty->transport_allowin = CONN_TRANSPORT_DEFMASK;
		}
	    }
	} else {
	    setinteger(csb, GETOBJ(int,2),
		       &MODEMS[0]->transport_allowin);
	}
	break;
      default:
	bad_parser_subcommand(csb, GETOBJ(int,10));
	break;
    }
}

/*
 * show_transport_list - Display a list of transports
 *
 * This routine displays a list of transports, passed a mask.
 */

static void show_transport_list (
     ulong mask)
{
    connvector *connv;

    if (mask) {
	for (connv = (connvector *)conn_vectorQ.qhead;
	     connv;
	     connv = connv->next) {
	    if (connv->type & mask)
	      printf(" %s", connv->name);
	}
    } else {
	printf(" none");
    }
}

/*
 * show_transport
 * show output and preferred transports for the "show terminal" display.
 */
void show_transport (tt_soc *tty)
{
    ulong pref;

    printf("\nAllowed transports are");
    show_transport_list(tty->transport_allowout);

    pref = (tty->tty_transport_pref == CONN_TRANSPORT_DEFMASK) ?
      connect_sysdef_transport : tty->tty_transport_pref;

    printf(".  Preferred is");
    show_transport_list(pref);
    printf(".");
}

/*
 * connect_multiproto - Attempt to connect to a host using multiple protocols
 */

static conntype *connect_multiproto (
     parseinfo *csb,
     ulong mask,
     boolean *abortflag)
{
    connvector *connv;
    conntype *conn;

    /*
     * Loop through all of the protocol handlers and attempt to make
     * a connection with each of the allowed protocols.
     */

    for (connv = (connvector *)conn_vectorQ.qhead;
	 connv;
	 connv = connv->next) {
	if (connv->type & mask) {
	    conn = ((*(connv->vopen))(csb, abortflag));
	    if (conn)
	      return(conn);
	    if (*abortflag)
	      return(NULL);	/* Get out on aborts */
	}
    }
    return(NULL);		/* No match by now */
}

#define MAXCHRS  50

/*
 * set_exit_all - set/reset exit_all field in csb and all child csbs
 */

static void set_exit_all (parseinfo *csb, int val)
{
    if (csb->next)
	set_exit_all(csb->next, val);
    csb->exit_all = val;
}

static void do_hangup (tt_soc* tty, parseinfo *csb)
{
    if ((tty->capabilities & AUTOHANGUP) && !tty->currentcon && !tty->xrcb) {
	exit_command(FALSE);
	if (csb) {
	    set_exit_all(csb, TRUE);
	}
    }
    return;
}

/*
 * connect_allowed - Determine if a connection is allowed
 *
 * This routine is used to determine if a connection is allowed. We check to
 * see if the specified transport is allowed, and if the session limit is
 * exceeded.
 */

boolean connect_allowed (connvector *connv)
{
    if (stdio->sessionlimit && stdio->sessionlimit <= stdio->nconns) {
	printf("Session limit exceeded");
	return(FALSE);
    }
    
    if (connv && (!(connv->type & stdio->transport_allowout))) {
	printf("%% %s connections not permitted from this terminal",
	       connv->name);
	return(FALSE);
    }

    return(TRUE);
}

/*
 * connect_new_session - Start up a new session
 *
 * This routine is used to start up a new session on a terminal. It is
 * called by the various connection handlers after a new connection has
 * been created.
 */

void connect_new_session (conntype *conn, char *hostname, parseinfo *csb)
{
    conn->acct_info = reg_invoke_aaa_acct_conn(stdio->ustruct, csb, conn);
    if (strlen(hostname) > CONNLEN) {
	conn->hostname[0] = 0;
    } else {
	sstrncpy(conn->hostname, hostname, CONNLEN);
    }
    assignlogicalname(conn, NULL); /* Init name */
    ++stdio->conncount;	/* Count another use of this line */

    stdio->currentcon = conn;
    if (talk(stdio, stdio->currentcon, FALSE) == FALSE) {
	stdio->currentcon = conn_leastidle(stdio);
    }
    do_hangup(stdio, csb);
}

/*
 * connect_command
 * Connect to a foreign hostname/address.  the name can be any protocol
 */

const char connect_unknown[] = 
  "%% Unknown command or computer name, or unable to find computer address";

void connect_command (parseinfo *csb)
{
    conntype *conn;
    ulong proto;
    boolean abortflag = FALSE;

    /*
     * If there's no address, prompt and gather one here
     */
    if (GETOBJ(string, 1)[0] == '\0') {
	printf("Host: ");
	if (!rdtty(GETOBJ(string, 1), PARSEBUF - 2)) {
	    printf("\n%% Bad host address");
	    return;
	}
    }

    if (GETOBJ(int,9) == CMD_CONNECT_RLOGIN) {
	/*
	 * Images without rlogin code should not allow the "rlogin"
	 * command to be parsed successfully, but in case someone
	 * goofs, check anyway.
	 */
	if (reg_invoke_rlogin_supported())
	    reg_invoke_rlogin_connect(csb);
	else
	    printf(noasm);
	return;
    }

    /* csb->multiple_funcs is overloaded as a return value which is
     * checked by exec() to determine if the connection request
     * was successful.  When connect_command() is called from the
     * parser, multiple_funcs is ignored upon return, so it is safe
     * to do this.
     */

    set_multiple_funcs(csb, 0);

    /*
     * Perform the general connection checks
     */

    if (!connect_allowed(NULL))
      return;

    /*
     * First, attempt to do a connect using the preferred transport.
     * Make sure that the preferred protocol is actually allowed.
     */

    proto = stdio->tty_transport_pref; /* Get preferred transport */

    /*
     * If preferred transport in tty is untouched, set preferred transport
     * to system-wide default protocol
     */

    if (proto == CONN_TRANSPORT_DEFMASK)
      proto = connect_sysdef_transport;	/* Set system-wide default protocol */

    conn = connect_multiproto(csb, proto & stdio->transport_allowout,
			      &abortflag);
    if (abortflag)
      return;			/* User aborted - get out */

    /*
     * If we failed to connect, try all of the allowed protocols on the
     * line, except for the prefered (since we just did that).
     */

    if (!conn)
      conn = connect_multiproto(csb,
				stdio->transport_allowout &
				~proto,
				&abortflag);
    if (abortflag)
      return;			/* User aborted - get out */

    /*
     * By now, if we don't have a connection, it means that we were not
     * able to recognise this name using any known protocols.
     */

    if (!conn) {
	printf(connect_unknown);
	do_hangup(stdio, csb);
	return;
    }

    /*
     * We are successful - indicate success and start the new connection
     */

    set_multiple_funcs(csb, 1);

    if (GETOBJ(int, 9) == CMD_CONNECT_TUNNEL) {
	/*
	 * For a tunnel connection, make the link very transparent
	 * by turning off the escape characters/etc.  note that the
	 * parser will have already set /stream.
	 */
	csb->line1 = stdio->ttynum;
	csb->line2 = stdio->ttynum+1;
	download_command(csb);
	stdio->tty_escapechar = -1;
    }
    connect_new_session(conn, GETOBJ(string, 1), csb);

    if (GETOBJ(int, 9) == CMD_CONNECT_TUNNEL) {
	/*
	 * Restore escape character, but don't bother trying to undo
	 * the rest of download_command.
	 */
	stdio->tty_escapechar = stdio->defaults->u_escapechar;
    }

}

/*
 * close_command
 * Close the specified connection.  Returns TRUE if connection deleted.
 */
boolean close_command (
    tt_soc *tty,
    int num,
    char *name,
    register conntype *conn)
{
    ulong j;

    if ((num == 0) && (name[0] == '\0')) {
	if (conn == NULL) {
	    printf("%% No current connection");
	    return(FALSE);
        }
    } else {
	if (num == 0) {
	    conn = findlogicalname(tty,name,&j);
	    if (conn == NULL) {
		lognamerr(j);
		return(FALSE);
	     }
	} else {
	    if (!(conn = findconn(tty,num))) {
		printf("?%d is not an open connection",num);
		return(FALSE);
	    }
	}
    }
    printf("Closing connection to ");
    (void) print_host(conn);
    if (!confirm(" "))
	return(FALSE);
    flush();
    conn_close(conn);
    return(TRUE);
}

/*
 * conn_check_idles
 *
 * Scan the connection list for timed-out sessions.
 * Closing them as they are found
 */
conntype *conn_check_idles (void)
{
    conntype *conn, *temp;

    conn = stdio->connhead.qhead;

    if (!stdio->tty_sessiontimeout) {
        return(conn);
    }

    while (conn) {
        if (TIMER_RUNNING_AND_AWAKE(conn->sessiontimer)) {
            temp = conn->link;
            conn_close(conn);
            conn = temp;
        } else {
            conn = conn->link;
        }
    }
    stdio->currentcon = conn_leastidle(stdio);
    return (stdio->currentcon);
}

/*
 * resume_command
 * Resume the current or named connection.
 * Returns NULL if could not resume, else returns pointer to connection.
 */
void resume_command (parseinfo *csb)
{
    register conntype *newcon;
    tt_soc *tty;
    conntype *conn;

    conn_check_idles();    /* Scan all sessions for Time-outs */

    tty = stdio;
    conn = tty->currentcon;

    /*
     * Implement stepping to the next connection, if requested.
     */
    if (conn && GETOBJ(int,13)) {
	conn = conn->link;
	if (!conn)
	    conn = (conntype *)tty->connhead.qhead;
    }

#define	DO_RESUME_HANGUP(return_conn)\
    if (return_conn == NULL) {\
	if (tty->connhead.qhead != NULL){\
	    return;\
	}\
	if ((tty->capabilities & AUTOHANGUP)){\
	    exit_command(FALSE);\
	    if (csb)\
		csb->exit_all= TRUE;\
	    return;\
	}\
    }\
    tty->currentcon = return_conn;\
    return

    if (GETOBJ(string,1)[0] == '/') {
	strcat(GETOBJ(string,1), GETOBJ(string,2));
	strcpy(GETOBJ(string,2), GETOBJ(string,1));
	GETOBJ(string,1)[0] = '\0';
    }
    if (GETOBJ(int,1) || (GETOBJ(string,1)[0] != '\0')) {	/* is there some kind of name */
	
	if (GETOBJ(int,1)) {
	    if (!(newcon = findconn(tty,GETOBJ(int,1)))) {
		printf("%% %d is not an open connection",GETOBJ(int,1));
		DO_RESUME_HANGUP(NULL);
		return;
	    }
	} else {
	    if (!(newcon =
		  (conntype *)findlogicalname(tty, GETOBJ(string,1),
					      (ulong *)&GETOBJ(int,1)))) {
		/*
		 * See if user wants to establish the connection when
		 * it doesn't already exist.
		 */
		if (GETOBJ(int,14)) {
		    if (!null(GETOBJ(string,3))) /* command specified? */
			sprintf(csb->line, "%s", GETOBJ(string,3));
		    else
			sprintf(csb->line, "connect %s", GETOBJ(string,1));
		    if (0 != parse_cmd(csb))
			printf("%% Can't connect with \"%s\" command",
			       csb->line);
		    return;
		}
		lognamerr(GETOBJ(int,1));
		DO_RESUME_HANGUP(NULL);
		return;
	    }
	}
	if (conn != NULL){
	    conn->status &= ~CONN_CURRENT;
	}
	conn = newcon;

    } else {

	/*
	 * If no connection specified and none is current, select the one
	 * that has been idle for the least amount of time.
	 */
	if (conn == NULL) {
	    conn = conn_leastidle(tty);
	    if (conn == NULL) {
		printf("%% No open connections");
		DO_RESUME_HANGUP(NULL);
	    }
	}
    }
    conn_resume(conn, csb);	/* Call the protocol handler for resume */

    /*
     * Resume the connection.  A FALSE return indicates
     * that the connection has died.  We then pick a new default.
     */
    if (talk(tty,conn,TRUE) == FALSE){
	conn = conn_leastidle(tty);
    }
    DO_RESUME_HANGUP(conn);
}

/*
 * name_command
 * Give a logical name to a connection
 */
void name_command (parseinfo *csb)
{
    register tt_soc *tty = stdio;
    register conntype *conn;
    int i, status;
    char inbuff[PARSEBUF], *buff;
    boolean parsed;

    buff = inbuff;
    printf("Connection number: ");
    status = rdtty(buff, MAXCHRS-2);
    i = parse_unsigned(deblank(buff), &parsed);
    if (!status || (!parsed)) {
	printf("%% Connection number required");
	return;
    }
    if (!(conn = findconn(tty,i))) {
	printf("%% %s is not an open connection", buff);
	return;
    }
    printf("Enter logical name: ");
    status = rdtty(buff,MAXCHRS-2);
    buff = deblank(buff);
    if (!status || null(buff)) {
	printf("%% Bad logical name");
	return;
    }
    uppercase(buff);
    if (!validlogicalname(buff, tty, conn))
	return;
    printf("Connection %d to ",conn->number);
    (void) print_host(conn);
    printf(" will be named \"%s\" ", buff);
    if (!confirm(NULL))
	return;
    sstrncpy(conn->logicalname,buff,CONNLEN+1);
}

/*
 * talk -- talk to a remote host
 * Returns  TRUE: connection was escaped from
 *	    FALSE: connection is dead and closed.
 */

boolean talk (register tt_soc *tty, register conntype *conn, int resumeflag)
{
    register int result;
    conntype *tcon;

    if (conn == NULL)
	return(TRUE);
    if (resumeflag) {
	printf("[Resuming connection %d to ",conn->number);
	(void) print_host(conn);
	printf(" ... ]\n");
    }
    flush();				/* e.g., FINGER from VTY */
    psion(tty);				/* psi enabled */
    for (tcon = (conntype *)conn->tty->connhead.qhead; tcon != NULL;
	tcon = tcon->link)
	tcon->status &= ~CONN_CURRENT;
    conn->status |= CONN_CURRENT;	/* active one  */ 
    tty->statbits |= CONNECTED;		/* Mark tty as connected */
    result = conn_run(conn, resumeflag); /* Run the session */
    psioff(tty);		        /* disable psi stuff */
    tty->statbits &= ~CONNECTED;	/* Not connected anymore */
    if (result != OK) {
	if (result == IDLE_TIMEDOUT)
	    clearoutput(stdio);
	/*
	 * If the quiet bit for this connection is set
	 * do not print the Connection close message
	 */
	if (!(conn->status & CONN_QUIET)) {
	    /*
	     * do not print the host name or address, if
	     * service hide telnet address is set
	     */
	    if (service_hideaddr) {
		printf("\n[Connection");
	    }
	    else {
		printf("\n[Connection to ");
		(void) print_host(conn);
	    }
	}
    }

    if (!(conn->status & CONN_QUIET)) {
	switch (result) {
	case OK:
	    break;
	case END_OF_FILE:
	case FOREIGN_RESET:
	    printf(" closed by foreign host]"); break;
	case REJECTED:
	    printf(" reset: username missing or invalid]"); break;
	case CARRIER_DROP:
	case LOCAL_RESET:
	    printf(" closed by local host]"); break;
	case TIMEDOUT:
	    printf(" timed out]"); break;
	case IDLE_TIMEDOUT:
	    printf(" idle too long; timed out]");
	    break;
	default:
	    if (conn->errorstring)
		printf(" aborted: %s]", conn->errorstring);
	    else
		printf(" aborted: error status %d]",result);
	    break;
	}	
    }

    switch (result) {
	case OK:
	    GET_TIMESTAMP(conn->idletime);
	    if (tty->capabilities & NOTIFY)
		notification(conn);
	    return(TRUE);
	case IDLE_TIMEDOUT:
	    if (tty->nconns <= 1)	/* force exec timeout? */
		tty->statbits |= IDLETIMEOUT;
	    break;
	default:
	    break;
    }	

    conn->status &= ~CONN_QUIET;
    conn_close(conn);
    return(FALSE);
}

/*
 * print_host - print connection's name string or address
 * Returns string length
 */

int print_host (conntype *conn)
{
    int length;

    if (!validmem(conn) || !validmem(conn->tty))
	return(0);
    if (conn->hostname[0])
	return(printf("%s",conn->hostname));
    /*
     * prevent the conn data structure from being freed while we wait
     * for responses from names servers and such things.
     */
    mem_lock(conn);
    length =  printf("%s", name_addr2string(&conn->address));
    free(conn);
    return(length);
}

/*
 * tty_get_remote_addr - return a temporary buffer to a string containing the
 * remote location information (if available).  The return value points to
 * a temporary buffer returned by string_getnext().
 */

char *
tty_get_remote_addr (tt_soc *tty)
{
    char *rem = NULL;
    char *loc = NULL;
    hwidbtype *idb;

    switch (tty->type) {
    case CTY_LINE:
    case AUX_LINE:
    case TTY_LINE:
	if (tty->conntop == NULL) {
	/* Async side of a tty */
	    if (reg_invoke_modem_get_calling_number(tty, NULL, &rem) == FALSE)
		rem = "async"; /* caller ID unavailable, return default str */
	    /*
	     * check for "called number" infomation, and pass that as well,
	     * in the same string, separated by a slash.  Legal fomats:
	     *    caller
	     *    caller/called
	     *    async/called
	     */
	    if (reg_invoke_modem_get_called_number(tty, NULL, &loc) == TRUE) {
		char * tmp = string_getnext();
		concat(rem, "/", tmp);
		rem = strcat(tmp, loc);
	    }		    
	    break;
	}
	/* Network side of a tty.  Treat same vty. */
	/* FALLTHRU */
    case VTY_LINE:
	if (reg_invoke_channel_hwidb(tty, &idb)) {
	    if (reg_invoke_modem_get_calling_number(NULL, idb, &rem))
		break;
	}

	if (tty->conntop->address.type) { 	/* is address valid */
	    rem = name_addr2numeric(&tty->conntop->address);
	} else {
	    rem = tty->conntop->hostname;
	}
	break;
    default:
	rem = NULL;
	break;
    }

    return(rem);
}

/*
 * conn_session
 * Assumes that tty->sessiontimeout is has been checked by caller.
 * Scan through a list of connection blocks, checking for session timeouts.
 * If we're in the scheduler, just set flags.  If called from process
 * level, try gracefully closing non-current timed out connections.
 * If current one times out, set the tty idle timeout so we get booted back
 * to EXEC.  Return FALSE if more needs to be done, TRUE otherwise.
 */

boolean conn_session (register conntype *curr, boolean schedtest)
{
    register conntype *con, *temp;

    /*
     * Scan list of all connections.  Close any timed out non-current ones.
     */
    if (!validmem(curr) || !validmem(curr->tty))
	return(TRUE);
    con = (conntype *)curr->tty->connhead.qhead;
    while (con) {
	if ((con != curr) && TIMER_RUNNING_AND_AWAKE(con->sessiontimer)){
	    if (schedtest)
		return(FALSE);
	    else {
		temp = con->link;
	        conn_close(con);
		con = temp;
	    }
	} else
	   con = con->link;
    }
    if (TIMER_RUNNING_AND_AWAKE(curr->sessiontimer)) {
	curr->tty->statbits |= IDLETIMEOUT;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * where_command
 * Print out local connection information.
 */

static const char amihead[] ="Conn Host                Address             Byte  Idle Conn Name\n";

void where_command (parseinfo *csb)
{
    conntype *currentcon;
    register conntype *conn;
    int len;

    conn = (conntype *) stdio->connhead.qhead;
    if (!conn) {
	printf("%% No connections open");
	return;
    }

    currentcon = conn_leastidle(stdio);
    automore_enable(amihead);
    while (conn != NULL) { 
	printf("%c%-3d ",((conn == currentcon) ? '*' : ' '), conn->number);
	len = CONNLEN - print_host(conn);
  	if (len < 0)
	    len = 0;
	printf("%*c", len, ' ');
	if (conn->address.ip_addr) {
	    printf("%20i", conn->address.ip_addr);
	} else {
	    printf("%20c", ' ');
	}
	printf("%-4d %-#5TE %s\n", conn_unread(conn), conn->idletime,
	       conn->logicalname);
	conn = conn->link;
    }
    automore_disable();
}

/*
 * conn_vector_init - Initialize a connection dispatch vector
 *
 * This routine is used to set up a connection vector. It is used as part
 * of protocol initialization.
 */
 
void conn_vector_init (
    connvector *vector,
    const char *name,
    const char *help)
{
    int i;
    ulong type;

    /*
     * Find a free bit to allocate for this connection vector
     */

    for (i = 0; i < 32; i++) {
	type = 1L << i;
	if (!(type & connect_vectormask))
	  break;
    }

    connect_vectormask |= type;		/* Mark this vector in use */

    vector->type = type;		/* Set up the type code */
    vector->name = name;		/* Set up the name */
    vector->help = help;		/* Set up the help text */
    vector->snmp_sesstype = D_tslineSesType_unknown;
					/* Set default SNMP session type */

    vector->vsettty = (boolean (*)(conntype *, tt_soc *)) return_false;	/* Default set new tty routine */
    vector->vchanged = (boolean (*)(conntype *)) return_false;	/* Default change handler */
    vector->vreport = (boolean (*)(conntype *)) return_false;	/* Default report state handler */
    vector->valive = (boolean (*)(conntype *)) return_false;	/* Default alive handler */
    vector->vrun = (int (*)(conntype *, boolean)) return_false;	/* Default run handler */
    vector->vclose = (void (*)(conntype *)) return_nothing;	/* Default close handler */
    vector->vunread = (int (*)(conntype *)) return_false;	/* Default unread handler */
    vector->vopen = (conntype *(*)(parseinfo *, boolean *))return_false; /* Default open handler */
    vector->vresume = (void (*)(conntype *, parseinfo *)) return_nothing; 	/* Default resume parse handler */
    vector->vaccounting = (void (*)(conntype *, acctinfo *)) return_nothing; /* default acct update */
    (void)insertlist(&conn_vectorQ, vector, conn_vector_compare);
}


static boolean conn_vector_compare (connvector *current, connvector *new)
{
    if (strcmp(new->name,"lat") == 0) {
	return(TRUE);
    }

    if ((strcmp(new->name, "pad") == 0) &&
	(strcmp(current->name, "lat") != 0)) {
	return(TRUE);
    }

    /* ensure telnet gets inserted before rlogin */
    if ((strcmp(new->name, "telnet") == 0) &&
	(strcmp(current->name, "rlogin") == 0)) {
	return(TRUE);
    }

    return(FALSE);
}


typedef boolean (*insertlist_t)(void *, void *);

static boolean insertlist (queuetype *queue, void *newitem,
		    void *insert_in_front)
{
    void **curr, **prev;

    prev = NULL;
    for (curr = (void **) queue->qhead; curr; curr = (void **) *curr) {
	if ((*((insertlist_t)insert_in_front))(curr, newitem)) {
	    if (prev) {
		*prev = newitem;
	    } else {
		queue->qhead = newitem;
	    }
	    *((void **)newitem) = curr;
	    queue->count++;
	    return(TRUE);
	}
	prev = curr;
    }
    enqueue(queue, newitem);
    return(TRUE);
}

/*
 * conn_setsflow
 * turn on and off SW flow control on the line. Called from
 * telnet and rlogin
 */
void conn_setsflow (tt_soc *tty, boolean sense)
{
    /*
     * don't do anything if we are doing HW flow control.
     * else things get messy as we can't currently have both
     * HW and SW flow control on the line.
     *
     * Also if FLOWPERMANENT, just ignore silently.
     */
    if ((tty->capabilities2 & FLOWPERMANENT_2) ||
	(tty->capabilities & (HFLOWIN | HFLOWOUT))) {
	if (modem_debug) {
	    ttybug(tty, "Ignoring Flow change request");
	}
	return;
    }
    if (sense && !(tty->capabilities & (SFLOWIN | SFLOWOUT))) {
	/*
	 * turn on SW flow control, if not on already
	 */
	tty->capabilities |= (SFLOWIN|SFLOWOUT);
	setsflow(tty);
    }
    if ((!sense) && (tty->capabilities & (SFLOWIN | SFLOWOUT))) {
	/*
	 * turn off SW flow control, if not off already
	 */
	tty->capabilities &= ~(SFLOWIN|SFLOWOUT);
	setsflow(tty);
    }
}

/*
 * conn_restoreflow
 * set SW flow control status of the line as in oldcaps
 * called from telnet and rlogin
 */
void conn_restoreflow (tt_soc *tty, ulong oldcaps)
{
    /*
     * don't do anything if we are doing HW flow control.
     * else things get messy as we can't currently have both
     * HW and SW flow control on the line.
     */
    if ((tty->capabilities2 & FLOWPERMANENT_2) ||
	(tty->capabilities & (HFLOWIN | HFLOWOUT))) {
	if (modem_debug) {
	    ttybug(tty, "Ignoring Flow restore request");
	}
	return;
    }
    oldcaps &= (SFLOWIN | SFLOWOUT);  /* the bits we want */
    if (oldcaps != (tty->capabilities & (SFLOWIN | SFLOWOUT))) {
	tty->capabilities &= ~(SFLOWIN | SFLOWOUT);
	tty->capabilities |= oldcaps;
	setsflow(tty);
    }
}
