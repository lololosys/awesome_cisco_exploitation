/* $Id: ident.c,v 3.9.10.3 1996/07/06 05:57:01 thille Exp $
 * $Source: /release/112/cvs/Xsys/tcp/ident.c,v $
 *------------------------------------------------------------------
 * ident.c  --  Support for RFC1413, the Ident protocol
 *
 * September 1995, Tim Kolar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ident.c,v $
 * Revision 3.9.10.3  1996/07/06  05:57:01  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.9.10.2  1996/05/21  10:06:19  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.9.10.1  1996/03/18  22:13:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.3  1996/03/16  07:40:45  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.2  1996/03/07  10:52:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  21:20:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/22  14:38:56  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.8  1996/01/31  23:34:37  tkolar
 * CSCdi47936:  Customers want additional Ident functionality
 * Add an option to prevent username polling attacks.
 *
 * Revision 3.7  1996/01/29  22:08:57  tkolar
 * CSCdi46928:  Identd does not fully match the RFC
 * Change OS to "IOS", remove whitespace, and fix bounds checking
 *
 * Revision 3.6  1996/01/29  07:30:21  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/18  15:53:33  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.4  1996/01/16  03:37:50  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.3  1995/12/17  18:39:15  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:47:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:32:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/10  17:46:27  tkolar
 * Add placeholder files to main branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ctype.h>
#include <ciscolib.h>
#include "ttysrv.h"
#include "ident.h"
#include "../tcp/tcp.h"
#include "../tcp/tcpinternal.h"
#include "../h/interface_private.h"
#include "aaa.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "../ip/ip_registry.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_config.h"


#define IDENT_IDLE_TIMEOUT (3 * ONEMIN)
#define IDENT_DALLYSECS     2            /* wait 2 seconds for closing */

static char *ident_alwaysbe;     /* fixed string to return */

static parser_extension_request ip_ident_chain_init_table[];

static uchar ident_enable;


/*
 * ip_ident_command --  IP configuration command for Ident
 *
 */
void ip_ident_command (parseinfo *csb)
{
    if (csb->nvgen) {
       nv_write(ident_enable, "%s", csb->nv_command);
       nv_add(ident_alwaysbe != NULL, " always-be %s", ident_alwaysbe);
       return;
    }

    ident_enable = csb->sense;

    /*
     *  Always free up ident_alwaysbe.  In the no case it's going away
     *   anyways, and in the non-no case we'll be resetting it below.
     */
    if (ident_alwaysbe) {
	free(ident_alwaysbe);
	ident_alwaysbe = NULL;
    }

    if ((csb->sense) && (csb->which == IP_IDENT_ALWAYSBE)) {
	setstring(&ident_alwaysbe, GETOBJ(string,1));
    }
}


/*
 * tcp_listen_ident_port
 * Performs processing for a ident port request
 */
void tcp_listen_ident_port (tcbtype *tcb)
{
    register int line;
    line = get_vtyline(TELNET_PORT, &tcp_connvector);
    if (line <= 0) {
        tcp_close(tcb);
	return;
    } 

    tcb->pid = process_create(tcpident_daemon, "TCP Ident", NORMAL_STACK,
                 PRIO_LOW);

    if (tcb->pid != NO_PROCESS) {
	process_set_arg_ptr(tcb->pid, tcb);
    } else {
	tcp_abort(tcb);
    }

}

/*
 * tcp_access_check_ident
 * routine to validate access to ident port(113).
 */
boolean tcp_access_check_ident (ulong sport, ulong dport, ipaddrtype fhost)
{
    if (ident_enable)
        return(TRUE);
    /* Else fall through and do default processing */
    return(tcp_access_check_default(sport, dport, fhost));
}

/*
 * parse_query
 * Parse ident queries from the client
 */

static tcbtype *parse_query (tcbtype *tcb, char *query,
			     int query_size, char *error)
{
    ulong lport = 0, fport = 0;
    tcbtype *query_tcb;
    int state = IPARSE_STARTING;
    int count;

    for (count = 0; count < query_size; count++) {
	switch(state) {
	  case IPARSE_STARTING:
	    if (isdigit(query[count])) {
		lport = query[count] - '0';
		state = IPARSE_LOCALPORT;
	    }
	    break;
	  case IPARSE_LOCALPORT:
	    if (!isdigit(query[count])) {
		state = IPARSE_FIND_NEXTDIGIT;
	    } else {
		lport = (query[count] - '0') + 10*lport;
	    }
	    break;
	  case IPARSE_FIND_NEXTDIGIT:
	    if (isdigit(query[count])) {
		fport = query[count] - '0';
		state = IPARSE_FOREIGNPORT;
	    }
	    break;
	  case IPARSE_FOREIGNPORT:
	    if (!isdigit(query[count])) {
		state = IPARSE_DONE;
	    } else {
		fport = (query[count] - '0') + 10*fport;
	    }
	    break;
	  case IPARSE_DONE:
	    /* skip everything after we've got what we want */
	    break;
	  default:
	      if (tcprcmd_debug) {
		  buginf("\nIdent: UNKNOWN STATE (%d) IN parse_query", state);
	      }
	      state = IPARSE_FOREIGNPORT;
	      break;
	}
    }

    if ((state != IPARSE_DONE) && (state != IPARSE_FOREIGNPORT)) {
        if (tcprcmd_debug) {
	    buginf("\nIdent:  Client %i bad query",
		   tcb->foreignhost.ip_addr);
        }
	strcpy(error,"INVALID-PORT");
	return(NULL);
    }

    /*
     * If the ports identified from the ident query are not in valid port
     * range, return INVALID-PORT error to the ident client
     */
    if (lport < 1 || lport > 65535 || fport < 1 || fport > 65535) {
        if (tcprcmd_debug) {
	    buginf("\nIdent:  Client %i querying unknown connection "
		   "local port - %d, remote port - %d",
		   tcb->foreignhost.ip_addr, fport, lport);
        }
	strcpy(error,"INVALID-PORT");
	return(NULL);
    }

    /*
     * Identify the TCP connection corresponding to the local/foreign port pair
     * If none found return NO-USER error string tothe client
     */

    if (tcprcmd_debug) {
        buginf("\nIdent: Client %i  has TCP host port - %d, router "
	       "port - %d", tcb->foreignhost.ip_addr, fport, lport);
    }

    query_tcb = find_tcb((ushort) fport, (ushort) lport,
			 &tcb->foreignhost, FALSE, FALSE);
    if (NULL == query_tcb) {
        strcpy(error,"NO-USER");
	if (tcprcmd_debug) {
	    buginf("\nIdent:  Client %i has no valid TCB on queried ports"
		   ".. NO-USER error");
	}
    }
    return(query_tcb);
}

/*
 * format_ident_reply
 * Fill in the Ident reply buffer to be sent to the client
 */

static int format_ident_reply (char *reply_buf, char *reply_str,
			       int index, int user)
{
    int count = index;
    int length = strlen(reply_str);

    /* if this is going to end up too large, just return */
    if ((index + strlen(USER_IDENT_STR) + length + 1) > IDENT_MAXREPLY) {
	return(count);
    }
	
    if (user) {
	bcopy(USER_IDENT_STR, (reply_buf + count), USER_IDENT_LEN);
	count += USER_IDENT_LEN;
    } else {
	bcopy(ERROR_IDENT_STR, (reply_buf + count), ERROR_IDENT_LEN);
	count += ERROR_IDENT_LEN;
    }
    bcopy(reply_str, &reply_buf[count], length);
    count += length;
    reply_buf[count++] = '\n';
    return (count);

}

/*
 * tcpident_daemon
 * Perform TCP ident services
 */

process tcpident_daemon (void)
{
    tt_soc *tty; 
    uchar *ptr;
    tcbtype *newtcb; 
    char *user, reply_buf[IDENT_MAXREPLY], error_str[IDENT_MAXERROR]; 
    char *replyptr;
    int reply_size, i, status, stringsize;
    boolean done, abort;
    tcbtype *tcb;

    if (!process_get_arg_ptr((void**)&tcb))
	    process_kill(THIS_PROCESS);

    process_set_crashblock(THIS_PROCESS, TRUE);

    if (tcprcmd_debug) {
        buginf("\nIdent:  server connecting to client %i",
	       tcb->foreignhost.ip_addr);
    }
    process_watch_timer(&tcb->dallytime, ENABLE);


    while(TRUE) {
	memset(reply_buf, 0, IDENT_MAXREPLY);
	memset(error_str, 0, IDENT_MAXERROR);
	replyptr = reply_buf;
	stringsize = 0;
	user = NULL;
	done = FALSE;
	abort = FALSE;

	while (!done) {

	    /* While there is TCP input, keep replying to ident queries */

	    TIMER_START(tcb->dallytime, IDENT_IDLE_TIMEOUT);
	    edisms((blockproc *)tcpmiscBLOCK, (ulong)tcb);

	    /* if the connection has closed or timed out, it's time to bail */
	    if (tcp_closing(tcb) || !SLEEPING(tcb->dallytime)) {
		abort = TRUE;
		if (tcprcmd_debug) {
		    buginf("\nIdent: TCP from %i timed out or closed",
			   tcb->foreignhost.ip_addr);
		}
		break;
	    }

	    while (tcp_unread(tcb)) {
		status = tcp_getstring(tcb, &ptr, &stringsize, MAXINT);
		if (status != OK)
		    break;

		/* Copy the querying string to be sent as a reply later */
		for(i=0; i < stringsize; i++) {

		    /*
		     *  Make sure the incoming string is not larger than
		     *	the buffer
		     */
		    if ((int) (replyptr - reply_buf) > IDENT_MAXREPLY) {
			done = TRUE;
			abort = TRUE;
			if (tcprcmd_debug) {
			    buginf("\nIdent: Input from %i too long,"
				   " Abort", tcb->foreignhost.ip_addr);
			}
			break;
		    }

		    *replyptr++ = ptr[i];

		    if ((ptr[i] == '\r') || (ptr[i] == '\n')){
			done = TRUE;
			/* strip off the last byte */
			replyptr--;
		    } 
		}

	    }
	}

	if (abort)
	    break;

	reply_size = (int) (replyptr - reply_buf);

	if (ident_alwaysbe) {
	    reply_size = format_ident_reply(reply_buf, ident_alwaysbe,
					    reply_size, 1);
	    if (tcprcmd_debug) {
	        buginf("\nIdent: Query from %i returning"
		       " ident_alwaysbe: %s",
		       tcb->foreignhost.ip_addr, ident_alwaysbe);
	    }
	} else {

	    /* Parse query and identify user of the TCP connection */
	    newtcb = parse_query(tcb, reply_buf, reply_size, error_str);

	    if (NULL != newtcb) {
	        tty = newtcb->tty;
		if (tty != NULL) {
		    if (tty->statbits & HASPROC) {
		        if (old_access_control) {
			    user = tty->user;
			} else {
			    /*
			     * the return val from aaa_get_user is
			     * a bool, but we will watch the change to
			     * the value of "user" pointer, instead
			     */
			    aaa_get_user_p(tty->ustruct, &user);

			}  /* old_access_control */
		    }      /* HASPROC */

		    /* 
		     * If no user could be identified, return <Guest>
		     */
		    if (user && ('\0' != user[0])) {
		        reply_size = format_ident_reply(reply_buf, user, 
							reply_size, 1);
		    } else {
		        reply_size = format_ident_reply(reply_buf,
							IDENT_GUEST, 
							reply_size, 1);
		    }
		    if (tcprcmd_debug) {
		        buginf("\nIdent:  Client %i, sending response %*s",
			       tcb->foreignhost.ip_addr,
			       reply_size, reply_buf);
		    }

		} else {   /* tty == NULL ?? */

		    strcpy(error_str,"UNKNOWN-ERROR");
		    if (tcprcmd_debug) {
		        buginf("\nIdent: %i No tty for tcb, ret: %s",
			       tcb->foreignhost.ip_addr, error_str);
		    }
		}  /* tty == NULL */
	    }      /* tcb != NULL */
	}          /* NULL == identalwaysbe */

	/* If any errors were encountered earlier return error string.
	     Unless, of course, we're always someone */
	if (strlen(error_str) && (ident_alwaysbe == NULL)) {
	    if (tcprcmd_debug) {
		buginf("\nIdent:  Client %i  unknown connection,"
			" returning %s",
		       tcb->foreignhost.ip_addr, error_str);
	    }
	    reply_size = format_ident_reply(reply_buf, error_str,
						reply_size, 0);
	}

	/* Send reply back to the ident client */
	for (i = 0; i < reply_size; i++) {
	    tcp_putbyte(tcb, reply_buf[i], TRUE);
	}
	tcp_putpush(tcb, TRUE, FALSE);
    }
    if (tcprcmd_debug) {
        buginf("\nIdent:  server disconnecting from client %i",
	       tcb->foreignhost.ip_addr);
    }

    tcp_close(tcb);

    process_kill(THIS_PROCESS);
}


/**************************************************************************
 *  Modularity below
 **************************************************************************/

static void ip_ident_init (subsystype *subsys) 
{
    ident_enable = FALSE;
    ident_alwaysbe = NULL;
    parser_add_command_list(ip_ident_chain_init_table, "IP Ident");
    reg_add_tcp_listen(IDENT_PORT, tcp_listen_ident_port,
		 "tcp_listen_ident_port");
    reg_add_tcp_access_check(IDENT_PORT, tcp_access_check_ident,
		"tcp_access_check_ident");

}

#define ALTERNATE	NONE
#include "cfg_ip_ident.h"
LINK_POINT(ip_ident_commands, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain registration array for Ident
 */

static parser_extension_request ip_ident_chain_init_table[] = {
    { PARSE_ADD_CFG_RTG_IP_CMD, &pname(ip_ident_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * Ident subsystem header
 */
#define IP_IDENT_MAJVERSION 1
#define IP_IDENT_MINVERSION 0
#define IP_IDENT_EDITVERSION 1

SUBSYS_HEADER(ip_ident_sys,
              IP_IDENT_MAJVERSION, IP_IDENT_MINVERSION,
              IP_IDENT_EDITVERSION,
              ip_ident_init, SUBSYS_CLASS_LIBRARY,
              subsys_req_iphost,
              NULL);
