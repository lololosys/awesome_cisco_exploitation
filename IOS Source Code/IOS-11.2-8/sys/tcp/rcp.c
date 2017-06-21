/* $Id: rcp.c,v 3.10.10.17 1996/09/06 21:01:10 snyder Exp $
 * $Source: /release/112/cvs/Xsys/tcp/rcp.c,v $
 *------------------------------------------------------------------
 * rcp.c -- Remote Copy over TCP
 *
 * March 1993, Paul Traina/Andy Heffernan
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rcp.c,v $
 * Revision 3.10.10.17  1996/09/06  21:01:10  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.10.10.16  1996/08/28  13:17:10  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.10.15  1996/08/13  21:07:34  che
 * CSCdi65824:  kerberos client mandatory broken for rsh
 * Branch: California_branch
 * Make sure non-Kerberized clients fail if "kerberos clients mandatory"
 * is configured.
 *
 * Revision 3.10.10.14  1996/08/07  09:02:51  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.10.10.13  1996/07/23  00:06:55  ahh
 * CSCdi63728:  RCP: simplify code to remove duplication
 * Saves 216 bytes! (Robert Snyder would be proud)
 * Branch: California_branch
 *
 * Revision 3.10.10.12  1996/07/06  05:57:03  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.10.10.11  1996/06/21  00:07:59  che
 * CSCdi60925:  exec cannot utilize kerberos
 * Branch: California_branch
 * Make sure Kerberos error messages are printed out only when
 * Kerberos is configured.
 *
 * Revision 3.10.10.10  1996/06/06  21:42:11  che
 * CSCdi57954:  router crashes attempting rlogin using kerberos
 * Branch: California_branch
 * Print out error message if reg_invoke_kerberos_stackcheck() returns
 * FALSE.
 *
 * Revision 3.10.10.9  1996/06/04  23:27:49  ahh
 * CSCdi30184:  RCP: debugging is inadequate
 * Branch: California_branch
 *
 * Revision 3.10.10.8  1996/06/04  08:36:28  che
 * CSCdi57424:  router crashes attempting kerberos telnet
 * Branch: California_branch
 * Added kerberos_stackcheck() registry to ensure correct stacksize of
 * process running kerberos clients.
 *
 * Revision 3.10.10.7  1996/06/01  12:29:04  che
 * CSCdi54857:  krb5-instance problem with none auth list
 * Branch: California_branch
 * Print out Kerberos error message only if failed via Kerberos.
 *
 * Revision 3.10.10.6  1996/05/21  10:06:21  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.10.5  1996/05/18  02:14:16  ahh
 * CSCdi58040:  Copying empty NVRAM causes reload
 * nv_review_buffer() can return NULL, so check for it.  Also swap
 * sstrncpy() args so incoming rcps work.
 * Branch: California_branch
 *
 * Revision 3.10.10.4  1996/04/19  15:33:28  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.10.10.3  1996/04/15  21:23:31  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.10.10.2  1996/04/15  15:00:11  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.10.10.1  1996/03/18  22:13:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/16  07:40:48  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.2  1996/03/07  10:52:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  21:20:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/22  14:38:58  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.9  1996/02/07  16:16:05  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/02/01  20:50:48  jenny
 * CSCdi47575:  crash when rsh command from host has wildcard
 *
 * Revision 3.7  1996/01/24  20:41:02  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.6  1996/01/16  03:37:52  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.5  1995/12/28  18:29:04  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreight addresses
 *   to tcp_open() for feature portability.
 *
 * Revision 3.4  1995/12/17  18:39:17  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:48:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:44:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:35:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/09/25  08:35:22  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.12  1995/09/11  16:45:30  pst
 * CSCdi39650:  config overwrite is braindead
 *
 * Revision 2.11  1995/08/17  00:29:04  ahh
 * CSCdi38764:  filesystem full not noticed by rcp
 * Sync up with the receiver when the transfer is all done.
 *
 * Revision 2.10  1995/08/06  01:28:33  ahh
 * CSCdi38304:  RCP: copying running-config out remotely should be quiet
 * Do nv_current_to_buffer() quietly.
 *
 * Revision 2.9  1995/07/22  06:05:23  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Fix over-zealousness in limiting output of "Building Configuration..."
 *
 * Revision 2.8  1995/07/16  18:04:26  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.7  1995/07/15  02:37:17  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.6  1995/07/06  21:46:12  ahh
 * CSCdi36734:  RCP: cannot stop file transfer on a VTY
 * Consume VTY input during file transfer so the TTY code will
 * notice when someone hits the escpae char.
 *
 * Revision 2.5  1995/06/20  07:19:27  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/18  06:19:22  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.3  1995/06/17 06:26:24  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.2  1995/06/09  13:21:43  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:05:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "ttysrv.h"
#include "connect.h"
#include "logger.h"
#include "login.h"
#include "aaa.h"
#include "config.h"
#include "parser.h"
#include "file.h"
#include "subsys.h"
#include "../os/boot.h"
#include "../ui/command.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ip/tftp.h"
#include "../h/config.h"
#include "tcp.h"
#include "tcpinternal.h"
#include "../os/nv.h"
#include "../h/file.h"
#include "../ip/ip_registry.h"
#include "../dev/flash_registry.h"
#include "rcp.h"
#include "rsh.h"
#include "tcp_debug.h"
#include "../dev/monitor1.h"
#include "exception.h"
#include "../os/free.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_tcpip.h"
#include "../if/network.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../tcp/msg_rcmd.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include <string.h>
#include "subsys.h"
#include "../h/auth_registry.regh"
#include "../kerberos/include/csfc5/rfc1510.h"

/* Imported globals */
extern boolean tcprcmd_debug;

/* Exported globals */
boolean rcp_enable, rsh_enable;
static char *rcmd_remote_username = NULL;

/* Private globals */
static char *const rsh_invalid_commands[] = {
                                  "telnet", "connect", "config", NULL};
static queuetype rhostQ;
static queuetype rshdiscmdQ;
static boolean rcmd_dns_lookup;
static idbtype *rcmd_source_idb;

#define RCP_BLOCKS_PER_BANG	10
#define RCP_WINDOW		(8 * 1024)


/*
 * rcmd_debug_string
 *
 * Return a formatted string with all the normal stuff
 * put into an RCMD debug message
 */

static char *
rcmd_debug_string (char *who, tcbtype *tcb, int dir)
{
    char *buf;

    buf = string_getnext();
    sprintf(buf, "%s: [%d %s %s:%d] %s", who, tcb->localport,
	    (dir == IN) ? "<-" : "->", address_string(&tcb->foreignhost),
	    tcb->foreignport, (dir == IN) ? "recv" : "send");
    return(buf);
}

/*
 * rcmd_response()
 *	Read and return response code from connection.
 *	Prints any error string sent.
 *	Returns -1 if code couldn't be read at all.
 */

static int
rcmd_response (tcbtype *tcb, boolean verbose)
{
    int status;
    uchar *buffer;
    int bytes;
    char c;

    c = tcp_getbyteblock(tcb, &status);
    if (status != OK) {
	if (verbose) {
	    char *error;

	    error = tcperror2string(status);
	    printf("%%rcp: Protocol error, %s\n", error);
	} 
	return(-1);
    }

    if (c == 0) {
	if (tcprcmd_debug)
	    buginf("\n%s <OK>", rcmd_debug_string("RCMD", tcb, IN));
	return(0);       /* OK, return */
    }

    /*
     * Collect the error message and print it if in verbose mode.
     */
    status = tcp_getstringblock(tcb, &buffer, &bytes, MAXPACKETDATALENGTH);
    if (status == OK) {
	char *p = strchr(buffer, '\n');
	if (p)
	    *p = '\0';
	else
	    buffer[bytes] = '\0';
	if (verbose)
	    printf("%%%s\n", buffer);
	if (tcprcmd_debug)
	    buginf("\n%s <BAD, %s>", rcmd_debug_string("RCMD", tcb, IN),
		   buffer);
    }

    return(c);
}

/*
 * rcp_put_string()
 *	Write a character string to the connection, not including
 *	the ASCII nul.
 *	Does not force the data out.
 */

static void
rcp_put_string (tcbtype *tcb, char *buf, boolean rcmderror)
{
    char *p;

    for (p = buf; *p; p++)
	(void) tcp_putbyte(tcb, *p, TRUE);
    if (tcprcmd_debug) {
	int ssize;

	ssize = strlen(buf);
	buginf("\n%s %s%*s%s\\n",
	       rcmd_debug_string(rcmderror ? "RCMD" : "RCP", tcb, OUT),
	       rcmderror ? "<BAD," : "", ssize - 1, buf, rcmderror ? ">" : "");
    }
}

/*
 * rcmd_ok()
 *	Send OK code to connection.
 */

static void
rcmd_ok (tcbtype *tcb)
{
    tcp_putbyte(tcb, 0, TRUE);
    tcp_putpush(tcb, TRUE, FALSE);
    if (tcprcmd_debug)
	buginf("\n%s <OK>", rcmd_debug_string("RCMD", tcb, OUT));
}

/*
 * rcmd_error()
 *	Send error code and string to connection.
 */

void
rcmd_error (tcbtype *tcb, char *s)
{
    tcp_putbyte(tcb, 1, TRUE);
    rcp_put_string(tcb, s, TRUE);
    tcp_putpush(tcb, TRUE, FALSE);
}

/*
 * rcp_receive()
 *	Fill a buffer with data using rcp protocol on tcb.
 *	Called after the connection has been established and the other
 *	side has authenticated us (or we've authenticated the other side
 *	if called by rshd_server).
 *
 *      a return of ERROR_ABORT means something happened that caused
 *      the transfer to fail.  it does not mean that it was aborted
 *      by the user.
 */

static int
rcp_receive (tcbtype *tcb, uchar *buffer, ulong *buffsize,
	     boolean verbose, rcp_callback func, void *func_arg)
{
    uchar *cmdbuffer, *cp, *data;
    int mode, size;
    long bytesleft;
    int bytes_read, c, status, reads, bytes_to_read, chunk_size, chunk_read;
    int bytes;

    size = 0;
    while (TRUE) {
	rcmd_ok(tcb);
	status = tcp_getstringblock(tcb, &cmdbuffer, &bytes,
				    MAXPACKETDATALENGTH);
	if (status == END_OF_FILE) {
	    if (tcprcmd_debug)
		buginf("\n%s <EOF>", rcmd_debug_string("RCP", tcb, IN));
	    if (size == 0) {
		/*
		 * Remote host closed too early (probably misconfigured).
		 * Caller will generate an error message.
		 */
		return(ERROR_ABORT);
	    } else {
		if (verbose)
		    printf(" [OK]\n");
		return(ERROR_OK);
	    }
	} else if (status == LOCAL_RESET) {
	    if (verbose) {
		printf(" [Interrupted]\n");
		flush();
	    }
	    return(ERROR_UNDEF);
	} else if (status != OK)
	    goto punt_tcp;

	if (bytes == 0) {
	    rcmd_ok(tcb);
	    return(ERROR_NOFILE);
	}

	cmdbuffer[bytes] = '\0';	/* make sure we're null terminated */

	for (cp = cmdbuffer; cp < cmdbuffer + bytes; cp++)
	    if (*cp == '\n')
		*cp = '\0';

	cp = cmdbuffer;

	switch (*cp) {
	case '\01':			/* soft error (file not found) */
	case '\02':			/* hard error */
	    if (tcprcmd_debug)
		buginf("\n%s \\%o", rcmd_debug_string("RCP", tcb, IN), *cp);
	    if (verbose)
		printf("\n%%%s", ++cp);
	    return(ERROR_NOFILE);

	case 'T':
	case 'D':
	    if (tcprcmd_debug)
		buginf("\n%s %c", rcmd_debug_string("RCP", tcb, IN), *cp);
	    continue;

	case 'E':
	    if (tcprcmd_debug)
		buginf("\n%s %c", rcmd_debug_string("RCP", tcb, IN), *cp);
	    rcmd_ok(tcb);
	    if (verbose)
		printf(" [OK]\n");
	    return(ERROR_OK);

	case 'C':
	    if (tcprcmd_debug)
		buginf("\n%s %s", rcmd_debug_string("RCP", tcb, IN), cp);
	    mode = 0;
	    for (cp++; cp < cmdbuffer + 5; cp++) {
		if (*cp < '0' || *cp > '7') {
		    rcmd_error(tcb, "rcp: protocol screwup: bad mode\n");
		    return(ERROR_PROTOCOL);
		}
		mode = (mode << 3) | (*cp - '0');
	    }

	    if (*cp++ != ' ') {		/* Mode not delimited? */
		rcmd_error(tcb, "rcp: protocol screwup: mode not delimited\n");
		return(ERROR_PROTOCOL);
	    }

	    size = 0;
	    while ((*cp >= '0') && (*cp <= '9'))
		size = size * 10 + (*cp++ - '0');

	    if (*cp++ != ' ') {		/* Size not delimited? */
		rcmd_error(tcb, "rcp: protocol screwup: size not delimited\n");
		return(ERROR_PROTOCOL);
	    }

	    if (size > *buffsize && !func) {
		if (verbose)
		    printf("\nFile %s is too big for receive buffer %u/%u",
			   cp, *buffsize, size);
		return(ERROR_TOOBIG);
	    }

	    if (verbose) {
		printf("\nLoading %u byte file %s: ", size, cp);
		flush();
	    }

	    /* Open file here if desired */

	    rcmd_ok(tcb);			/* We've "opened" our buffer */

	    reads = bytes = bytes_read = 0;

	    if (func) {
		while (bytes_read < size) {
                    bytesleft = size - bytes_read; /* bytes left to read */
		    bytes_to_read = min(CHUNK_SIZE, bytesleft);
		    chunk_size = bytes_to_read;
		    bytes = 0;

		    /*
		     * Read and copy CHUNK_SIZE bytes (except for
		     * the final chunk) before calling the flash
		     * callback function.  This ensures that the
		     * flash code won't have problems calculating
		     * the checksum on the data given to it; the
		     * value of CHUNK_SIZE must be even.
		     */
		    while (bytes < chunk_size) {
			status = tcp_getstringblock(tcb, &data, &chunk_read,
						    bytes_to_read);
			/*
			 * Some flash code will detect the interrupt
			 * sequence and arrange for the file to be
			 * marked as bad, so break out of the loop
			 * now so we can call the callback.
			 */
			if (status == LOCAL_RESET)
			    break;
			else if (status != OK)
			    goto punt_tcp;

			bcopy(data, adjustpointer(buffer), chunk_read);
			buffer += chunk_read;
			bytes += chunk_read;
			bytes_to_read -= chunk_read;
		    }

		    if (!(*func)(&buffer, &bytesleft, bytes, func_arg)) {
			rcmd_error(tcb, "Write failed\n");
			return(ERROR_WRITE);
		    } else if (status == LOCAL_RESET) {
			/*
			 * There was an interrupt, but the flash
			 * didn't handle it and return an error
			 * from the callback, so handle it now.
			 */
			if (verbose) {
			    printf(" [Interrupted]\n");
			    flush();
			}
			return(ERROR_UNDEF);
		    }

		    if (verbose)
			file_xfer_progress(++reads, RCP_BLOCKS_PER_BANG);

		    bytes_read += bytes;

		    /*
		     * In the case where we are an rcp server, if there
		     * is console input, give it a chance to run.  exec()
		     * runs at a lower priority than IP Input.
		     */
		    if ((inputpending(stdio)) && !verbose)
			process_suspend();
		}
	    } else {
		ulong bytes_left;

		bytes_left = size;
		while (bytes_left) {
		    status = tcp_getstringblock(tcb, &data, &bytes_read,
						bytes_left);
		    if (status == LOCAL_RESET) {
			if (verbose) {
			    printf(" [Interrupted]\n");
			    flush();
			}
			return(ERROR_UNDEF);
		    } else if (status != OK)
			goto punt_tcp;

		    if (verbose)
			file_xfer_progress(++reads, RCP_BLOCKS_PER_BANG);

		    bcopy(data, buffer, bytes_read);
		    buffer += bytes_read;
		    bytes_left -= bytes_read;

		    /*
		     * In the case where we are an rcp server, if there
		     * is console input, give it a chance to run.  exec()
		     * runs at a lower priority than IP Input.
		     */
		    if ((inputpending(stdio)) && !verbose)
			process_suspend();
		}
	    }
		
	    if (tcprcmd_debug)
		buginf("\n%s %d bytes", rcmd_debug_string("RCP", tcb, IN),
		       size);

	    *buffsize = size;

	    c = tcp_getbyteblock(tcb, &status);	/* get response sync */
	    break;

	default:
	    if (tcprcmd_debug)
		buginf("\n%s \\%o", rcmd_debug_string("RCP", tcb, IN), *cp);
	    return(ERROR_UNDEF);
	}
    }

punt_tcp:
    if (tcprcmd_debug)
	buginf("\n%s, TCP read failed -- %s",
	       rcmd_debug_string("RCP", tcb, OUT), tcperror2string(status));
    if (verbose)
	printf(" [Abort: %s]", tcperror2string(status));

    return(ERROR_ABORT);
}

/*
 * rcmd_get_local_username
 *
 */

char *
rcmd_get_local_username (void)
{
    char *luser;

    if (old_access_control) {
	luser = stdio->user ? stdio->user : hostname;
    } else if (!aaa_get_user_p(stdio->ustruct, &luser)) {
	luser = hostname;
    }
    return(luser);
}

static char *
rcmd_get_remote_username (void)
{
    if (rcmd_remote_username)
	return(rcmd_remote_username);
    return(rcmd_get_local_username());
}

/*
 * rcp_file_read()
 *	Called to fill a buffer using the rcp protocol.
 *	This is like issuing "rcp remote: file" on the router.
 *	Returns codes a la tftp_read().
 */

static int
rcp_file_read (filetype *confg, uchar *buffer, ulong *buffersize,
	       boolean verbose, boolean (*func)(uchar **buf, long *bytesleft,
						long len, void *opaque_args),
	       void *func_arg)
{
    addrtype *foreignhost = &confg->ps.tuba.boothost;
    uchar *cmd;
    tcbtype *tcb;
    int error;
    char *ruser;

    cmd = malloc(MAXCMDLEN);
    if (cmd == NULL)
	return(ERROR_ILLOP);

    sprintf(cmd, "rcp -f %s", confg->filename);

    /* SNMP may explicitly set a remote user name. If so, use it. */
    ruser = confg->ps.tuba.username ? confg->ps.tuba.username :
	rcmd_get_remote_username();
    tcb = rcmd(stdio, foreignhost, rcmd_get_local_username(), ruser, cmd,
	       NULL, verbose, &error);
    free(cmd);

    if (!tcb)
	return(error);

    if (verbose) {
	printf("\nConnected to %s", address_string(foreignhost));
	flush();
    }

    error = rcp_receive(tcb, buffer, buffersize, verbose, func, func_arg);

    if ((error == ERROR_ABORT) && verbose)
	rcmd_response(tcb, TRUE);

    tcp_close(tcb);

    return(error);
}

/*
 * rcp_send
 *	Send a buffer of data using rcp protocol on tcb.
 *	Called after the connection has been established and the other
 *	side has authenticated us (or we've authenticated the other side
 *	if called by rshd_server).
 */

static int
rcp_send (tcbtype *tcb, const char *name, char *buf, ulong bufsize,
	  rcp_write_callback func, void *func_arg, boolean display)
{
    char *tmpbuf;
    ushort st_mode;
    long chunk, bytes_left;
    int writes;
    int error, tcp_error;

    tmpbuf = malloc(MAXCMDLEN);
    if (tmpbuf == NULL)
	return(ERROR_ILLOP);		/* failure */

#ifdef RCP_PFLAG
    /* If we ever implement the -p option, here's what you do: */
    if (pflag) {
	sprintf(tmpbuf, "T%ld 0 %ld 0\n", mtime, atime);
	rcp_put_string(tcb, tmpbuf, FALSE);
	tcp_putpush(tcb, TRUE, FALSE);
	if (rcmd_response(tcb, display)) {
	    free(tmpbuf);
	    return(ERROR_PROTOCOL);
	}
    }
#endif

    /* Send file stats. */
    st_mode = SENT_FILE_MODE;			/* XXX -- configurable? */
    sprintf(tmpbuf, "C%04o %ld %s\n", st_mode & 07777, bufsize, name);
    rcp_put_string(tcb, tmpbuf, FALSE);
    tcp_putpush(tcb, TRUE, FALSE);
    if (rcmd_response(tcb, display)) {
	free(tmpbuf);
	return(ERROR_PROTOCOL);
    }

    free(tmpbuf);

    /*
     * Enough of this witty banter.
     * Transfer the actual file data.
     */
    writes = 0;
    error = ERROR_OK;
    bytes_left = bufsize;

    while (bytes_left) {
	int bytes_written;

	chunk = min(bytes_left, CHUNK_SIZE);
	if (func) {
	    /*
	     * The callback function actually provides the data
	     * a bit at a time, and sets buf to point at that data.
	     */
	    chunk = (*func)((uchar **) &buf, chunk, func_arg);
	    if (!chunk) {
		error = ERROR_READ;
		break;
	    }
	}

	bytes_written = tcp_putstringblock(tcb, buf, chunk, &tcp_error);
	if (bytes_written == 0) {
	    if (tcp_error == LOCAL_RESET) {
		if (display) {
		    printf(" [Interrupted]\n");
		    flush();
		}
		error = ERROR_UNDEF;
	    } else
		error = ERROR_ABORT;
	    break;
	} else
	    error = ERROR_OK;

	if (display)
	    file_xfer_progress(++writes, RCP_BLOCKS_PER_BANG);

	bytes_left -= chunk;
	if (!func)
	    buf += chunk;
    }

    if (tcprcmd_debug)
	buginf("\n%s %d bytes", rcmd_debug_string("RCP", tcb, OUT),
	       bufsize - bytes_left);

    switch (error) {
    case ERROR_READ:
	rcmd_error(tcb, "Read failed\n");
	break;
    case ERROR_OK:
	tcp_putpush(tcb, TRUE, FALSE);
	rcmd_ok(tcb);
	break;
    case ERROR_ABORT:
	rcmd_error(tcb, "Aborted\n");
	break;
    }

    return(error);
}

/*
 * rcp_write()
 *	Called to send a buffer using the rcp protocol.
 *	This is like issuing "rcp file remote:" on the router.
 */

static boolean
rcp_write (const char *filename, addrtype *foreignhost,
	   char *buffer, ulong buffsize, filetype *rcp_params,
	   rcp_write_callback func, void *func_arg)
{
    uchar *cmd;
    tcbtype *tcb;
    int error;
    boolean display = TRUE;
    char *remote_username = NULL;

    if (rcp_params) {
	display = rcp_params->ps.tuba.display;
	/* SNMP may explicitly set a remote user name. If so, use it. */
	remote_username = rcp_params->ps.tuba.username;
    }
    if (!remote_username)
	remote_username = rcmd_get_remote_username();

    if (display) {
	printf("\nWriting %s ", filename);
	flush();
    }

    cmd = malloc(MAXCMDLEN);
    if (!cmd)
	return(FALSE);

    sprintf(cmd, "rcp -t %s", filename);
    tcb = rcmd(stdio, foreignhost, rcmd_get_local_username(),
	       remote_username, cmd, NULL, display, &error);
    free(cmd);

    if (!tcb)
	return(FALSE);

    /* Sync with the receiver. */
    if (rcmd_response(tcb, display)) {
	tcp_close(tcb);
	return(FALSE);
    }

    /* Send the data. */
    error = rcp_send(tcb, filename, buffer, buffsize, func, func_arg, display);

    if (rcmd_response(tcb, display))
	error = ERROR_ABORT;

    tcp_close(tcb);

    return(error == ERROR_OK);
}

/*
 * rcp_file_write()
 *	This is the RCP version of the file_write service point,
 *	reached via reg_invoke_file_write.
 */

static boolean
rcp_file_write (filetype *file, char *buffer, uint buffersize)
{
    return(rcp_write(file->filename, &file->ps.tuba.boothost, buffer,
		     buffersize, NULL, NULL, NULL));
}

/*
 * rcmd_put_string()
 *	Write a character string to the connection, including the ASCII nul.
 *	Does not force the data out.
 */
static void
rcmd_put_string (tcbtype *tcb, char *buf)
{
    char *p;

    for (p = buf; *p; p++)
	(void) tcp_putbyte(tcb, *p, TRUE);
    (void) tcp_putbyte(tcb, '\0', TRUE);
    if (tcprcmd_debug)
	buginf("\n%s %s\\0", rcmd_debug_string("RCMD", tcb, OUT), buf);
}

/*
 * rcmd
 *	Open a connection to the remote host's remote-shell port,
 *	send authentication information, return tcb and optionally
 *	return secondary tcb for stderr.
 *	Called by rcp_file_read() and rcp_write().
 */

tcbtype *
rcmd (tt_soc *tty, addrtype *rhost, char *luser, char *ruser,
      char *cmd, tcbtype **stderr, boolean display, int *error)
{
    ushort lport;
    tcbtype *tcb;
    int status;
    char portbuf[10];
    boolean msg_sent;
    addrtype laddr, *lhost;
    int timeout;
    long authopts = CSFC5c_APOPTS_MUTUAL_REQUIRED_m;
    int use_kerb = 0;
    ulong winsize;

    /*
     * First see if we want to use Kerberos
     */
    if (reg_invoke_kerberos_is_running() && reg_invoke_kerberos_stackcheck()
	&& tty && tty->ustruct && tty->ustruct->user && tty->ustruct->cred
	&& (tty->ustruct->cred->cred_type == KERBEROS)) {
	use_kerb = 1;
    } else {
	if (reg_invoke_kerberos_is_running() &&
	    !reg_invoke_kerberos_stackcheck()) {
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

    if (rcmd_source_idb && rcmd_source_idb->ip_address &&
	interface_up(rcmd_source_idb)) {
	laddr.type = ADDR_IP;
	laddr.ip_addr = rcmd_source_idb->ip_address;
	lhost = &laddr;
    } else
	lhost = NULL;

    lport = IPPORT_RESERVED - 1;
    timeout = 1;
    while (1) {
	if (!tcp_reservedport(&lport)) {
	    if (display)
		printf("%% No reserved ports\n");
	    *error = ERROR_NOSPACE; 	/* No more TCP ports available */
	    return(NULL);
	}
	tcb = tcp_open(tty, (use_kerb)? KSHELL_PORT:RSHELL_PORT, rhost, lport,
		       lhost, &status, NULL);
	if (tcb)
	    break;

	/*
	 * Connection failed, either try again or give up.
	 */
	if ((status == UNREACHABLE) || (status == REJECTED)) {
	    if (use_kerb && !reg_invoke_kerberos_use_kerb_clients()) {
		printf("Kerberos:\tConnection unreachable.  "
			"Trying regular rsh...\n");
		use_kerb = 0;
	    }
	}
	if (status == INUSE) {
	    lport--;
	} else if (status == REJECTED && timeout <= 16) {
	    /*
	     * BSD rcmd backs off and tries the same connection.
	     * Why not decrement the local port number and
	     * try again immediately like above?
	     */
	    process_sleep_for(timeout * ONESEC);
	    timeout *= 2;
	} else {
	    if (display)
		printf("%% %s\n", tcperror2string(status));
	    if (status == NO_MEMORY)		/* Out of memory */
		*error = ERROR_NOMEM;
	    else if (status == ABORTED)		/* TCP protocol not running */
		*error = ERROR_ACCESS;
	    else  
		*error = ERROR_EXISTS;		/* Local port is being used */ 
	    return(NULL);
	}
    }

    tcb->flags |= TCB_FULLSEGS;		/* big segments, please */
    winsize = RCP_WINDOW;
    tcp_set_property(tcb, TCP_WINDOW_SIZE, &winsize, sizeof(winsize));

    if (tcb->tty)
	psion(tcb->tty);                /* allow interrupts */

    if (stderr) {
	if (!tcp_reservedport(&lport)) {
	    if (display)
		printf("%% No reserved ports\n");
	    tcp_abort(tcb);
	    *error = ERROR_NOSPACE;	/* No more TCP ports available */ 
	    return(NULL);
	}
	sprintf(portbuf, "%d", lport);
	rcmd_put_string(tcb, portbuf);
	(void) tcp_putpush(tcb, TRUE, FALSE);	/* write data to net */
	*stderr = tcp_old_listen(NULL, lport);	/* wait for remote to
						   establish connection for
						   stderr traffic */
    } else {
	msg_sent = tcp_putbyte(tcb, 0, FALSE);  /* no stderr connection */
	if (tcprcmd_debug)
	    buginf("\n%s \\0", rcmd_debug_string("RCMD", tcb, OUT));
	if (msg_sent == FALSE) {
	    if (display)
		buginf("\n%% No resource for rcmd connection!");
	    tcp_close(tcb);
	    *error = ERROR_ABORT;
	    return(NULL);
	}
    }

    tcp_set_rcvwnd(tcb, tcp_defrcvwnd);

    /* Kerberos retrieves the luser/ruser/cmd parameters in a different
     * order than normal rlogin/rsh.  If you are using Kerberized r-commands
     * the luser/ruser/cmd parameters are retrieved in the
     * reg_invoke_kerberos_is_running() registry function.
     */
    if (reg_invoke_kerberos_is_running() && use_kerb) {
	if (!reg_invoke_kerberos_client_auth(tcb, tty, rhost, (long) authopts,
					     luser, ruser, cmd, NULL, 0)) {
	    tcp_close(tcb);
	    if (stderr)
		tcp_close(*stderr);
	    return(NULL);
	}
    } else {

	rcmd_put_string(tcb, luser);
	rcmd_put_string(tcb, ruser);
	rcmd_put_string(tcb, cmd);
	(void) tcp_putpush(tcb, TRUE, FALSE);	/* write data to net */
    }

    if (rcmd_response(tcb, display)) {
	tcp_close(tcb);
	if (stderr)
	    tcp_close(*stderr);
	*error = ERROR_PROTOCOL;
	return(NULL);
    } else
	return(tcb);
}	

/*
 * rshd_gets
 */

static char *
rshd_gets (tcbtype *tcb, char *buf, unsigned int buflen)
{
    char *sbuf = buf;
    int status, ch;

    while (buflen && (ch = tcp_getbyteblock(tcb, &status)) && status == OK) {
	*buf++ = ch;
	buflen--;
    }
    *buf = '\0';
    if (status != OK)
	return(NULL);
    return(sbuf);
}

/*
 * rcp_no_such_file()
 *	Send a UNIX-like error message down the pipe.
 */

#define MSG_ENOENT	"rcp: %s: No such file or directory\n"
#define MSG_ENOENT_SIMP	"rcp: No such file or directory\n"

static void
rcp_no_such_file (tcbtype *tcb, char *filename)
{
    char *msg;
    boolean alloced;

    msg = malloc(sizeof(MSG_ENOENT) + strlen(filename));
    alloced = (msg != NULL);
    if (alloced)
	sprintf(msg, MSG_ENOENT, filename);
    else
	msg = MSG_ENOENT_SIMP;
    rcmd_error(tcb, msg);
    if (alloced)
	free(msg);
}

/*
 * rcp2flash()
 *	Handle copying from remote to running and startup configuration.
 */

static boolean
rcp2flash (tcbtype *tcb, char *filename)
{
    uchar *buf;
    ulong buflen;

    /*
     * Even though we will only be transporting chunk_size chunks
     * we need to have extra space because the flash code assumes
     * extra space.
     */
    buflen = CHUNK_SIZE * 2;

    buf = malloc(buflen);
    if (buf == NULL) {
	rcmd_error(tcb, nomemory);
	return(FALSE);
    }

    if (reg_invoke_write_to_flash(tcb, filename, buf, buflen)) {
	free(buf);
	return(TRUE);
    }

    /*
     * Some versions of the flash write code sometimes give us
     * an error string in the buffer we handed to it.
    rcmd_error(tcb, buf);
     */
    free(buf);
    return(FALSE);
}

/*
 * rcp2config()
 *	Handle copying from remote to running and startup configuration.
 */

static int
rcp2config (tcbtype *tcb, char *filename, boolean direct_to_nv,
	    char *ruser, char *luser)
{
    uchar *buf;
    ulong buflen;
    int error;

    buflen = config_bytes;

    buf = malloc(buflen);
    if (buf == NULL)
	return(ERROR_ABORT);

    error = rcp_receive(tcb, buf, &buflen, FALSE, NULL, 0);
    if (error != ERROR_OK) {
	free(buf);
	return(error);
    }

    if (direct_to_nv) {
	if (!nv_buffer_to_nv(buf, buflen, TRUE)) {
	    errmsg(&msgsym(CONFIG_NV_ERR, SYS));
	    error = ERROR_ABORT;
	} else
	    errmsg(&msgsym(CONFIG_NV, SYS), filename, ruser,
		   address_string(&tcb->foreignhost));
    } else {
	if (buflen)
	    reset_host();
	parse_configure((char *)buf, TRUE, RES_CONFIG, PRIV_ROOT);
	errmsg(&msgsym(CONFIG, SYS), filename, ruser,
	       address_string(&tcb->foreignhost));
    }

    free(buf);
    return(error);
}

/*
 * rcp_command()
 *	Do the rcp command requested by the remote host.
 */

static int
rcp_command (tcbtype *tcb, tcbtype *tcb2, int argc, char *argv[],
	     char *ruser, char *luser, rhost_entry *entry)
{
    int fflag, tflag;
    int i;
    char *filename;
    int error;
    int pflag, iamremote, iamrecursive, targetshouldbedirectory;

    fflag = tflag = 0;
    pflag = iamremote = iamrecursive = targetshouldbedirectory = 0;
    for (i = 1; i < argc; i++) {
	if (argv[i][0] == '-') {
	    switch (argv[i][1]) {
	    case 'p':
		++pflag;
		break;
	    case 'r':
		++iamrecursive;
		break;
	    case 'd':
		targetshouldbedirectory = 1;
		break;
	    case 'f':
		iamremote = fflag = 1;
		break;
	    case 't':
		iamremote = tflag = 1;
		break;
	    default:
		break;
	    }
	} else
	    break;
    }

    if (pflag || iamrecursive || targetshouldbedirectory) {
	rcmd_error(tcb, "-p -r -d options not supported\n");
	return(-1);
    }

    if (!iamremote) {
	rcmd_error(tcb, "must specify either -f or -t\n");
	return(-1);
    }

    filename = argv[i];

    if (fflag || tflag) {
	if (fflag) {
	    /* Sync with the receiver. */
	    if (rcmd_response(tcb, FALSE))
		return(ERROR_ABORT);
	}

	/*
	 * If you must be enabled on the console to perform the
	 * analogs of the following remote copies, it only makes
	 * sense that we require the "enable" keyword on the
	 * remote-host configuration command for these.
	 */
	if (entry->priv_level != PRIV_ROOT) {
	    rcmd_error(tcb, "Permission denied.\n");
	    return(ERROR_ABORT);
	}
    }

    if (fflag) {	/* Send the "file" to the remote machine. */
	/* Send the data. */

	/*
	 * Try the two magic filenames first, then go to flash.
	 */
	if (strcmp(filename, "running-config") == 0) {
	    char *buf;
	    int buflen;

	    buf = malloc(config_bytes);
	    if (buf == NULL)
 		return(ERROR_ABORT);
	    buflen = nv_current_to_buffer(buf, config_bytes, FALSE,
					  PRIV_ROOT, FALSE);

	    error = rcp_send(tcb, filename, buf, buflen, NULL, NULL, FALSE);

	    free(buf);
	    return(error);
	} else if (strcmp(filename, "startup-config") == 0) {
	    nvtype *nv;
	    char *buf;
	    ulong buflen;

	    nv = nv_review_buffer();
	    if (nv == NULL) {
		rcp_no_such_file(tcb, filename);
		return(ERROR_ABORT);
	    }

	    buflen = nv->textsize;
	    if (!buflen) {
	        nv_done(nv);
	        return(ERROR_ABORT);
	    }
	    
	    buf = malloc(buflen);
	    if (buf == NULL) {
	        nv_done(nv);
	        return(ERROR_ABORT);
	    }
	    
	    /* don't use bcopy() to access NVRAM */
	    memcpy(buf, nv->textbase, buflen);

	    /* rcp_send() will eventually calls bcopy() to transfer
	     * data. We allocate a temp buffer so instead of directly
	     * accessing NVRAM, bcopy() will access this temp buffer.
	     */
	    error = rcp_send(tcb, filename, buf, buflen,
			     NULL, NULL, FALSE);
	    free(buf);
	    nv_done(nv);
	    return(error);
	} else if (reg_invoke_rcp_flash_server(tcb, filename)) {
	    return(ERROR_OK);
	} else {
	    /*
	     * XXX Flash code should provide enough information
	     * to produce a meaningful error message when it fails.
	     * It doesn't, so we do this:
	     */
	    rcp_no_such_file(tcb, filename);
	    return(ERROR_ABORT);
	}
    }

    if (tflag) {	/* Receive the "file" from the remote machine. */
	if (strcmp(filename, ".") == 0) {
	    rcmd_error(tcb, "please specify target filename\n");
	    return(ERROR_ABORT);
	}

	/*
	 * Try the two magic filenames first, then go to flash.
	 */
	if (strcmp(filename, "running-config") == 0) {
	    error = rcp2config(tcb, filename, FALSE, ruser, luser);
	    return(error);
	} else if (strcmp(filename, "startup-config") == 0) {
	    error = rcp2config(tcb, filename, TRUE, ruser, luser);
	    return(error);
	} else if (rcp2flash(tcb, filename)) {
	    return(ERROR_OK);
	} else {
	    /*
	     * XXX Flash code should provide enough information
	     * to produce a meaningful error message when it fails.
	     * It doesn't, so we do this:
	     */
	    rcp_no_such_file(tcb, filename);
	    return(ERROR_ABORT);
	}
    }

    return(0);
}

/*
 * find_rsh_disable_command
 */

static rsh_disabled_cmd_entry *
find_rsh_disable_command (queuetype *q, char *command)
{
    rsh_disabled_cmd_entry *t;

    for (t = (rsh_disabled_cmd_entry *)q->qhead; t; t = t->next) {
	if (partialmatch(t->command, command))
	    return(t);
    }

    return(NULL);
}

/*
 * Checks list of prohibited commands that are set to PRIV_USER
 * instead of PRIV_ROOT, thus allowing it to be eligible for
 * "auto-command" in the exec process.  A good example of this is
 * to prohibit "rsh host telnet".  we don't want to be able to
 * telnet via rsh.
 */

static inline boolean
invalid_rsh_command (char *command)
{
    return(find_rsh_disable_command(&rshdiscmdQ, command) != NULL);
}

/* rhosts stuff
 *
 * Enhancements:
 *	-hostname
 *		disallow access from hostname
 *	hostname -username
 *		disallow access by username on hostname
 * Notes:
 */

static rhost_entry *
find_rhost (queuetype *q, addrtype *addr, char *remote, char *local)
{
    rhost_entry *t;

    for (t = (rhost_entry *) q->qhead; t; t = t->next) {
	if (addr->ip_addr == t->addr.ip_addr &&
	    strcmp(local, t->local_user) == 0 &&
	    strcmp(remote, t->remote_user) == 0)
	    return(t);
    }

    return(NULL);
}

/*
 * rshd_authenticate()
 *	Determine whether or not to let the remote user on.
 */

static rhost_entry *
rshd_authenticate (tcbtype *tcb, char *ruser, char *luser)
{
    /*
     * There is only one "account" here, so there is only one
     * set of rhosts information, but a virtual "account" will
     * be enforced by checking for the "local user".
     */

    return(find_rhost(&rhostQ, &tcb->foreignhost, ruser, luser));
}

/*
 * rshd_server
 *	authenticate:
 *		check client's port is reserved (done already)
 *		read client username, server username, command strings
 *		find server username in passwd
 *		if server username is not root, read hosts.equiv and
 *			$HOME/.rhosts
 *		if server username is root, read /.rhosts
 *		if server username is not root and logins disabled, terminate
 *	execute the command
 */

forktype
rshd_server (kerberos_tcbtype *kerb_tcb)
{
    int status, ch;
    ushort second_port, new_errport;
    tcbtype *second_tcb = NULL;
    rhost_entry *entry;
    char ruser[USERNAMESIZE+1], luser[USERNAMESIZE+1], rcmd[RCMD_SIZE];
    char *command = NULL, *p;
    int argc;
    char **argv = NULL;
    boolean failed = FALSE;
    tcbtype *tcb;

    process_set_priority(PRIO_LOW);

    tcb = kerb_tcb->tcb;

    /*
     * Validate ip address to make sure we're not spoofed...
     */
    if (rcmd_dns_lookup) {
        if (!reg_invoke_name_validate_addr(&tcb->foreignhost)) {
	    errmsg(&msgsym(RCMDDNSFAIL, RCMD), tcb->foreignhost.ip_addr);
	    failed = TRUE;	/* Our problem, carry on for now. */
        }
    }

    command = malloc(MAXCMDLEN+1);
    if (command == NULL) {
	/* Ack! no memory! */
	failed = TRUE;		/* Our problem, carry on for now. */
    }

    tcb->flags |= TCB_FULLSEGS;		/* big segments, please */

    /* Read secondary port number. */
    second_port = 0;
    while ((ch = tcp_getbyteblock(tcb, &status)) && status == OK)
	second_port = second_port * 10 + ch - '0';

    if (status != OK) {
	if (tcprcmd_debug)
	    buginf("\n%s 2nd port number bogus",
		   rcmd_debug_string("RCMD", tcb, IN));
	goto done;		/* Their problem, stop. */
    }

    /*
     * Connect with the client's secondary port, if specified.
     * Not that we're going to do anything intelligent with it.
     */
    if (second_port) {
	if (tcprcmd_debug)
	    buginf("\n%s %d\\0", rcmd_debug_string("RCMD", tcb, IN),
		   second_port);
	if ((second_port < (IPPORT_RESERVED - 1)) || kerb_tcb->use_kerb) {
	    if (second_port < (IPPORT_RESERVED - 1)) {
		new_errport = IPPORT_RESERVED - 1;
		if (!tcp_reservedport(&new_errport)) {
		    failed = TRUE;	/* Our problem, carry on for now. */
		}
	    }
	    second_tcb = tcp_open(NULL, second_port, &tcb->foreignhost,
				  new_errport, NULL, &status, NULL);
	    if (!second_tcb) {
		if (tcprcmd_debug)
		    buginf("\nRCMD: [%d -> %s:%d] open failed -- %s",
			   new_errport, address_string(&tcb->foreignhost),
! 			   second_port, tcperror2string(status));
		goto done;	/* Their problem (most likely), stop. */
	    }
	} else {
	    if (tcprcmd_debug)
		buginf("\n%s Second port isn't reserved",
		       rcmd_debug_string("RCMD", tcb, IN));
	    goto done;		/* Their problem, stop. */
	}
    } else {
	if (tcprcmd_debug)
	    buginf("\n%s \\0", rcmd_debug_string("RCMD", tcb, IN));
    }

    /*
     * BSD rcmd() will have raced ahead and will be sitting
     * in accept.  Now that we've connected to the secondary port,
     * close the connection(s) and clean up if we're failing because
     * of the double-DNS authentication swipe or lack of memory
     * (i.e., things that are our fault).
     */
    if (failed)
	goto done;

    tcp_set_rcvwnd(tcb, tcp_defrcvwnd);

    if ((reg_invoke_kerberos_is_running()) && (kerb_tcb->use_kerb)) {
	if (!reg_invoke_kerberos_server_auth(tcb, NULL, 0, NULL)) {
	    rcmd_error(tcb, "Permission denied.\n");
	    if (tcprcmd_debug) {
		buginf("\nrshd: Kerberos server authentication failed with %i",
			tcb->foreignhost.ip_addr);
	    }
	    goto done;
	}

	rshd_gets(tcb, ruser, USERNAMESIZE); 
	rshd_gets(tcb, command, MAXCMDLEN);
	rshd_gets(tcb, luser, USERNAMESIZE); 
    } else {
	rshd_gets(tcb, ruser, USERNAMESIZE);
	rshd_gets(tcb, luser, USERNAMESIZE);
	rshd_gets(tcb, command, MAXCMDLEN);
    }

    if (tcprcmd_debug)
	buginf("\n%s %s\\0%s\\0%s\\0", rcmd_debug_string("RCMD", tcb, IN),
	       ruser, luser, command);

    /* Authenticate user. */
    entry = rshd_authenticate(tcb, ruser, luser);

    if (entry)
	rcmd_ok(tcb);
    else {
	if (tcprcmd_debug)
	    buginf("\n%s -- %s %i %s not in trusted hosts database",
		   rcmd_debug_string("RCMD", tcb, IN), luser,
		   tcb->foreignhost.ip_addr, ruser);
	rcmd_error(tcb, "Permission denied.\n");
	goto done;
    }

    sstrncpy(rcmd, command, 4);

    if (strcmp(rcmd, "rcp") == 0) {
	if (rcp_enable == FALSE) {
  	    rcmd_error(tcb, "Permission denied.\n");
	    errmsg(&msgsym(RCPATTEMPTED, RCMD), ruser,
		   address_string(&tcb->foreignhost));
	    goto done;
	}

        /* Argify the command string.  Note the conservative guesswork. */
        argc = strlen(command) / 2 + 1;
        if ((argv = malloc(sizeof(char *) * (argc + 1))) == NULL) {
	    /* Ack! no memory! */
	    goto done;
        }

        argc = 0;
        p = command;
        while (*p) {
	    argv[argc++] = p;
	    while (*p && !(*p == ' ' || *p == '\t' || *p == '\n'))
	        p++;
	    if (*p == '\0')
	        break;
	    *p = '\0';
	    p++;
	    while (*p == ' ' || *p == '\t' || *p == '\n')
	        p++;
        }
        argv[argc] = NULL;

        /* Run command. */
	rcp_command(tcb, second_tcb, argc, argv, ruser, luser, entry);
    } else {
	char first_token[FIRST_TOKEN];

	if (rsh_enable == FALSE) {
  	    rcmd_error(tcb, "Permission denied.\n");
	    errmsg(&msgsym(RSHATTEMPTED, RCMD), ruser,
		   address_string(&tcb->foreignhost));
	    goto done;
	}

	sstrncpy(first_token, command, FIRST_TOKEN);
	p = (char *)&first_token;
	while (*p && !(*p == ' ' || *p == '\t' || *p == '\n'))
	    p++;
	*p = '\0';
	
	if (invalid_rsh_command((char *)&first_token)) {
	    rcmd_error(tcb, "Permission denied.\n");
	    if (tcprcmd_debug)
		buginf("\nRSHD: %s not allowed\n", first_token);
	    goto done;
	}

       /*
        * If rsh_command succeeds in creating an EXEC process,
        * that new process is responsible for freeing both TCB's,
        * line, connection block, and anything else.
        */
	rsh_command(command, tcb, second_tcb, entry);
	goto done_save_tcbs;
    }

done:
    if (second_tcb)
	tcp_close(second_tcb);
    tcp_close(tcb);
    free(kerb_tcb);
    kerb_tcb = NULL;

done_save_tcbs:
    /*
     * free up the locally malloc'ed blocks of memory
     */
    free(argv);
    free(command);
    free(kerb_tcb);
    process_kill(THIS_PROCESS);
}

/*
 * rcmd_rhost_command()
 *	Called as part of configuration to add or remove a host/username
 *	pair from the rhost list.
 */

/*
 * [no] rcmd remote-host <local-user> { <host> | <ipaddr } <remote-user>
 *     [enable]
 */
static void
rcmd_rhost_command (parseinfo *csb)
{
    addrtype *addr;

    /*
     * This is from the perspective of the client requesting a rsh/rcp
     * request.
     * remote_user will be the requesting user name
     * local_user will be the target "account" in our router
     */
    char *local_user, *remote_user;
    rhost_entry *t, *entry;
    static int rhostQinit = 0;

    /*
     * Initialize the rhostQ if this is the first time in.
     */
    if (rhostQinit == 0) {
	queue_init(&rhostQ, 0);
	rhostQinit = 1;
    }

    if (csb->nvgen) {
	for (t = (rhost_entry *) rhostQ.qhead; t; t = t->next) {
	    nv_write(TRUE, "%s %s %i %s", csb->nv_command, t->local_user,
                     t->addr.ip_addr, t->remote_user);
	    if (t->priv_level > DEFAULT_PRIV) {
		nv_add(TRUE, " enable");
		nv_add(t->priv_level != PRIV_ROOT, " %d", t->priv_level);
	    }
	}
	return;
    }

    addr = GETOBJ(paddr,1);
    local_user = GETOBJ(string,1);
    remote_user = GETOBJ(string,2);

    /* Make a stab at finding the entry. */
    entry = find_rhost(&rhostQ, addr, remote_user, local_user);

    if (csb->sense) {		/* "rcmd remote-host ..." */
	if (entry) {
	    /*
	     * Let the user change this without deleting
	     * and reentering the configuration command.
	     */
	    entry->priv_level = GETOBJ(int,1);
	    return;
	}

	/* Create a new rhost_entry. */
	entry = malloc(sizeof(rhost_entry));
	if (entry == NULL) {
	    printf(nomemory);
	    return;
	}

	memset(entry, 0, sizeof(*entry));
	bcopy(addr, &entry->addr, sizeof(*addr));
	setstring(&entry->remote_user, remote_user);
	setstring(&entry->local_user, local_user);
	entry->priv_level = GETOBJ(int,1);
	enqueue(&rhostQ, entry);

    } else {			/* "no rcmd remote-host ..." */
	/* Delete the entry if there. */
	if (entry == NULL)
	    return;
	unqueue(&rhostQ, entry);
	free(entry->local_user);
	free(entry->remote_user);
	free(entry);
    }
}

/*
 * rsh_disable_command
 * make a command invalid to remote shells when this router acts as
 * a server
 */

static void
rsh_disable_command (parseinfo *csb)
{
    rsh_disabled_cmd_entry *entry;
    char *command;

    if (csb->nvgen) {
#ifdef HIDDEN
	for (t = (rsh_disabled_cmd_entry *) rshdiscmdQ.qhead; t; t = t->next) {
	    nv_write(TRUE, "%s %s", csb->nv_command, t->command);
	}
#endif
	return;
    }

    command = GETOBJ(string,1);

    entry = find_rsh_disable_command(&rshdiscmdQ, command);

    if (csb->sense) {
	if (entry)
	    return;

	/* Create a new rsh_disabled_cmd_entry. */
	entry = malloc(sizeof(rsh_disabled_cmd_entry));
	if (entry == NULL) {
	    printf(nomemory);
	    return;
	}

	memset(entry, 0, sizeof(rsh_disabled_cmd_entry));
	setstring(&entry->command, command);

	enqueue(&rshdiscmdQ, entry);
    } else {
	if (entry == NULL)
	    return;

	unqueue(&rshdiscmdQ, entry);
	free(entry->command);
	free(entry);
    }
}

/*
 * rcmd_command()
 *	Handle all the rcmd config commands
 */

void
rcmd_command (parseinfo *csb)
{
    switch (csb->which) {
    case RCMD_SOURCE_INTERFACE:
	if (csb->nvgen) {
	    if (rcmd_source_idb)
		nv_write(TRUE, "%s %s", csb->nv_command,
			 rcmd_source_idb->namestring);
	} else {
	    if (csb->sense)
		rcmd_source_idb = GETOBJ(idb,1);
	    else
		rcmd_source_idb = NULL;
	}
	break;

    case RCMD_REMOTE_USER:
	if (csb->nvgen) {
	    nv_write(rcmd_remote_username != NULL, "%s %s", csb->nv_command,
		     rcmd_remote_username);
	} else {
	    if (csb->sense) {
		setstring(&rcmd_remote_username, GETOBJ(string,1));
	    } else if (rcmd_remote_username) {
		free(rcmd_remote_username);
		rcmd_remote_username = NULL;
	    }
	}
	break;

    case RCMD_REMOTE_HOST:
	rcmd_rhost_command(csb);
	break;

    case RCMD_DOMAINLOOKUP:
	if (csb->nvgen) {
	    nv_write(domain_enable && !rcmd_dns_lookup,
		     "no %s", csb->nv_command);
	} else {
	    rcmd_dns_lookup = csb->sense;
	}
	break;

    case RCMD_RSH_ENABLE:
	if (csb->nvgen)
	    nv_write(rsh_enable, "%s", csb->nv_command);
	else
	    rsh_enable = csb->sense;
	break;

    case RCMD_RCP_ENABLE:
	if (csb->nvgen)
	    nv_write(rcp_enable, csb->nv_command);
	else
	    rcp_enable = csb->sense;
	break;

    case RCMD_RSH_DISABLED_CMD:
	rsh_disable_command(csb);
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * show rhost file
 */

void
show_rhosts (parseinfo *csb)
{
    rhost_entry *t;

    if (rhostQ.qhead == NULL)
	return;

    automore_enable(NULL);

    printf("Local user   Host            Remote user \n");

    for (t = (rhost_entry *) rhostQ.qhead; t; t = t->next)
	printf("%12s %15i %s\n", t->local_user, t->addr.ip_addr,
	       t->remote_user);

    automore_disable();
}

/*
 * show disabled commands for rsh
 */

void
show_rsh_disabled_commands (parseinfo *csb)
{
    rsh_disabled_cmd_entry *t;

    if (rshdiscmdQ.qhead == NULL)
	return;

    automore_enable(NULL);

    for (t = (rsh_disabled_cmd_entry *) rshdiscmdQ.qhead; t; t = t->next)
	printf("%s\n", t->command);

    automore_disable();
}

/*
 * rcp_file_match
 * Does the protocol dependent information in two file descriptors match?
 * Called by SERVICE_FILE_MATCH with index FILE_ACCESS_RCP
 */

static boolean
rcp_file_match (filetype *cfg1, filetype *cfg2)
{
    return(ip_match_address(&cfg1->ps.tuba.boothost, &cfg2->ps.tuba.boothost));
}

/*
 * rcp_file_parse
 * Fill in protocol specific information when parsing a file specification
 * Called by SERVICEFILE_PARSE, index is FILE_ACCESS_RCP
 */

static void
rcp_file_parse (filetype *file, parseinfo *csb)
{
    char *bufptr;

    if (csb->nvgen) {
	nv_add(TRUE, "rcp %s %s", file->filename, 
	       address_string(&file->ps.tuba.boothost));
        /*
         * Now add boot command to ROM monitor BOOT variable.
         */
        bufptr = reg_invoke_monvar_buff(ROMMON_BOOT_VAR);
        if (bufptr) {
            bufptr += strlen(bufptr);
            sprintf(bufptr, "rcp:%s %s,%d;", file->filename,
                    address_string(&file->ps.tuba.boothost),
                    BOOTRETRIES);
        }
	return;
    }
    file->proto = FILE_ACCESS_RCP;
    file->ps.tuba.boothost.type = ADDR_IP;
    file->ps.tuba.boothost.ip_addr = -1;	/* default address */
    if (GETOBJ(paddr,1)->type == ADDR_IP) {	/* IP address */
	file->ps.tuba.boothost.type = ADDR_IP;
	file->ps.tuba.boothost.ip_addr = GETOBJ(paddr,1)->ip_addr;
    }
}

/*
 *
 * rcp_bootfile_parse
 * Given a protocol name and parts of a source address, figure out if
 * RCP should be used to access the file.  Set up fields in file
 * descriptor as appropriate.  Returns TRUE if there was a match.
 * Called by SERVICE_BOOTFILE_PARSE, index is FILE_ACCESS_RCP
 */

static boolean
rcp_bootfile_parse (filetype *file, char *protocol, char *host, char *arg)
{
    ipaddrtype boothost;

    if (!strmatch(protocol, "rcp", FALSE))
	return(FALSE);
    if (!null(host)) {
	if (!parse_hostname(host, &boothost)) {
	    printf("%% Bad IP address: %s", host);
	    return(FALSE);
	}
    } else
	boothost = -1L;
    file->proto = FILE_ACCESS_RCP;
    file->ps.tuba.boothost.type = ADDR_IP;
    file->ps.tuba.boothost.ip_addr = boothost;
    return(TRUE);
}

/*
 * rcp_bootinfo_get 
 * Record protocol specific boot information.
 * Called by SERVICE_BOOTINFO_GET with index of FILE_ACCESS_RCP
 */

static void
rcp_bootinfo_get (bootinfo_t *bootinfo, filetype *sysconfg)
{
    sysconfg->ps.tuba.respondent.type = ADDR_IP;
    sysconfg->ps.tuba.respondent.ip_addr = bootinfo->ipaddr;
}

/*
 * rcp_bootinfo_set
 * After loading in the image, tell bootstrap where it came from.
 * Used by SERVICE_BOOTINFO_SET with index FILE_ACCESS_RCP
 */

static void
rcp_bootinfo_set (bootinfo_t *bootinfo, filetype *sysconfg)
{
    bootinfo->ipaddr = sysconfg->ps.tuba.respondent.ip_addr;
}

/*
 * rcp_file_source
 * Return string indicating the source of a file
 * Called by SERVICE_FILE_SOURCE, index is FILE_ACCESS_RCP
 */

static void
rcp_file_source (filetype *file, uchar *buffer)
{
    sprintf(buffer, "rcp from %s", address_string(&file->ps.tuba.boothost));
}

/*
 * rcp_dump_core_file - Write out a core file using RCP
 * This is a separate call (the caller could be doing
 * reg_invoke_file_write()) but TFTP needs the extra ipsocktype
 * parameter.
 */

static boolean
rcp_dump_core_file (uchar *mem, ulong size,
		    const char *filename, ipaddrtype iphost,
		    ipsocktype *soc_unused_have_a_nice_day)
{
    addrtype host;

    host.type = ADDR_IP;
    host.ip_addr = iphost;

    return(rcp_write((char *) filename, &host, mem, size, NULL, NULL, NULL));
}

/*
 * Parse chains for top-level IP configuration commands
 */
#define ALTERNATE	NONE
#include "cfg_ip_rcp.h"
LINK_POINT(rcp_cfg_ip, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for rcmd config commands (backwards compatibility)
 */
#define ALTERNATE       NONE
#include "cfg_rcmd.h"
LINK_POINT(rcmd_config_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for exception protocol configuration command
 */
#define ALTERNATE	NONE
LINK_TRANS(exception_proto_rcp_exit, no_alt);
KEYWORD_ID(exception_proto_rcp, exception_proto_rcp_exit, ALTERNATE,
	 OBJ(int,1), FILE_ACCESS_RCP,
	 "rcp", "RCP protocol", PRIV_CONF);
#undef ALTERNATE
#define ALTERNATE	exception_proto_rcp
LINK_POINT(rcp_exception_proto, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for "debug ip tcp" debug command
 */
#define ALTERNATE	NONE
EOLS  (debug_ip_tcp_rcmd_eol, tcp_debug_command, DEBUG_TCPRCMD);
KEYWORD (debug_ip_tcp_rcmd, debug_ip_tcp_rcmd_eol, ALTERNATE,
       "rcmd", "Rcmd transactions", PRIV_OPR);
#undef ALTERNATE
#define ALTERNATE	debug_ip_tcp_rcmd
LINK_POINT(rcp_debug, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for RCP/RSH "show" commands
 */
#define ALTERNATE       NONE
#include "exec_show_rsh_dis_cmnds.h"
#include "exec_show_rhosts.h"
LINK_POINT(rcmd_show_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain for exec commands
 */
#define ALTERNATE       NONE
#include "exec_rsh.h"
LINK_POINT(exec_rsh_commands, ALTERNATE);
#undef  ALTERNATE
/*
 * Parse chains for "boot system" command
 */
#define ALTERNATE	NONE
EOLS	(conf_boot_rcp_eol, boot_command, BOOT_FILE);
IPADDR	(conf_boot_rcp_ipaddr, conf_boot_rcp_eol, conf_boot_rcp_eol,
	 OBJ(paddr,1),
	 "Address from which to download the boot config file");
STRING	(conf_boot_rcp_file, conf_boot_rcp_ipaddr, no_alt,
	 OBJ(string,1), "Configuration filename");
KEYWORD_ID(conf_boot_rcp, conf_boot_rcp_file, ALTERNATE,
	 OBJ(int,2), FILE_ACCESS_RCP,
         "rcp", "Boot from a server via rcp", PRIV_CONF);
LINK_POINT(rcp_boot_stuff, conf_boot_rcp);
#undef ALTERNATE

/*
 * Parse chains for "copy" command
 * This could be modularized better, but at least this is a start.
 */
#include "../parser/parser_defs_flash.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_config.h"
#include "../dev/flash_defines.h"

#define ALTERNATE	NONE
/* extend "copy rcp ..." from here */
LINK_TRANS(exec_copy_rcp_file_extend_here, no_alt);
LINK_TRANS(exec_copy_rcp_extend_here, exec_copy_rcp_file_extend_here);

/* copy rcp startup-config */
EOLS    (exec_copy_rcp_start_conf_eol, manual_configure,
	 PARSER_CONF_OVERWRITE_NET);
KEYWORD (exec_copy_rcp_startup_config, exec_copy_rcp_start_conf_eol,
         exec_copy_rcp_extend_here,
         "startup-config",
	 "Copy to startup configuration", PRIV_ROOT);

/* copy rcp running-config */
EOLS    (exec_copy_rcp_run_conf_eol, manual_configure,
	 PARSER_CONF_NET);
KEYWORD (exec_copy_rcp_running_config, exec_copy_rcp_run_conf_eol,
	 exec_copy_rcp_startup_config, 
         "running-config",
	 "Update (merge with) current system configuration", PRIV_ROOT);

/* copy rcp flash */
EOLS    (exec_copy_rcp_flash_eol, flash_copy_command, COPY_RCP_FLASH);
KEYWORD (exec_copy_rcp_flash, exec_copy_rcp_flash_eol, 
         exec_copy_rcp_running_config,
	 SF, "Copy to system flash", PRIV_OPR);

/* copy rcp bootflash */
EOLS    (exec_copy_rcp_bflash_eol, flash_copy_command, COPY_RCP_BOOTFLASH);
KEYWORD (exec_copy_boot_rflash, exec_copy_rcp_bflash_eol, exec_copy_rcp_flash,
	 BF, "Copy to boot flash", PRIV_OPR);
IFELSE  (ifelse_copy_rcp, exec_copy_boot_rflash, exec_copy_rcp_flash,
         reg_invoke_flash_alias_match(BF));

/* copy rcp ... */
KEYWORD_ID (exec_copy_rcp, ifelse_copy_rcp, ALTERNATE,
            OBJ(int,1), COPY_RCP,
	    "rcp", "Copy from an rcp server", PRIV_OPR);
LINK_POINT(rcp_copy_commands, exec_copy_rcp);
#undef ALTERNATE

/*
 * Copying various things to rcp
 */

/* copy startup-config rcp */
EOLS    (exec_copy_start_conf_rcp_eol, write_network, WRITE_NETWORK);
KEYWORD_ID (exec_copy_start_conf_rcp, exec_copy_start_conf_rcp_eol, NONE,
	    OBJ(int,1), COPY_RCP,
	    "rcp", 
	    "Copy to an rcp server", PRIV_ROOT);
LINK_POINT(rcp_copy_start_commands, exec_copy_start_conf_rcp);

/* copy running-config rcp */
EOLS    (exec_copy_run_conf_rcp_eol, write_network, WRITE_NETWORK);
KEYWORD_ID (exec_copy_run_conf_rcp, exec_copy_run_conf_rcp_eol, NONE,
	    OBJ(int,1), COPY_RCP, 
            "rcp", 
	    "Copy to an rcp server", PRIV_ROOT);
LINK_POINT(rcp_copy_run_commands, exec_copy_run_conf_rcp);

/* copy flash rcp */
EOLS    (exec_copy_flash_rcp_eol, flash_copy_command, COPY_FLASH_RCP);
KEYWORD (exec_copy_flash_rcp, exec_copy_flash_rcp_eol, NONE,
        "rcp", "Copy to an rcp server", PRIV_OPR);
LINK_POINT(rcp_copy_flash_commands, exec_copy_flash_rcp);

/* copy bootflash rcp */
EOLS    (exec_copy_bflash_rcp_eol, flash_copy_command, COPY_BOOTFLASH_RCP);
KEYWORD (exec_copy_bflash_rcp, exec_copy_bflash_rcp_eol, NONE,
        "rcp", "Copy to an rcp server", PRIV_OPR);
LINK_POINT(rcp_copy_bflash_commands, exec_copy_bflash_rcp);
/*
 * Parse chain registration array for rcp/rsh
 */
static parser_extension_request rcp_chain_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(rcmd_show_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(exec_rsh_commands) },
    { PARSE_ADD_COPY_CMD, &pname(rcp_copy_commands) },
    { PARSE_ADD_COPY_STARTUP_CMD, &pname(rcp_copy_start_commands) },
    { PARSE_ADD_COPY_RUNNING_CMD, &pname(rcp_copy_run_commands) },
    { PARSE_ADD_COPY_FLASH_CMD, &pname(rcp_copy_flash_commands) },
    { PARSE_ADD_COPY_BFLASH_CMD, &pname(rcp_copy_bflash_commands) },
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(rcp_cfg_ip) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(rcmd_config_commands) },
    { PARSE_ADD_CFG_EXCEP_PROTO_CMD, &pname(rcp_exception_proto) },
    { PARSE_ADD_CFG_EXCEP_PROTO_EXIT,
	  (dynamic_transition *) &pname(exception_proto_rcp_exit) },
    { PARSE_ADD_CFG_BOOT_SYS_CMD, &pname(rcp_boot_stuff) },
    { PARSE_ADD_DEBUG_TCP_CMD, &pname(rcp_debug) },
    { PARSE_LIST_END, NULL }
};

/*
 * rcp_init
 * Initialize RCP support 
 */
static void rcp_init (subsystype *dummy)
{
    char *invalid;
    int i;
    rsh_disabled_cmd_entry *entry;

    rcp_enable = FALSE;
    rcmd_dns_lookup = TRUE;
    rcmd_source_idb = NULL;

    queue_init(&rshdiscmdQ, 0);
    i = 0;

    invalid = rsh_invalid_commands[i++];

    while (invalid) {
	/* Create a new rsh_disabled_cmd_entry */
	entry = malloc(sizeof(rsh_disabled_cmd_entry));
	if (entry == NULL) {
	    printf(nomemory);
	    return;
	}

	memset(entry, 0, sizeof(rsh_disabled_cmd_entry));
	setstring(&entry->command, invalid);

	enqueue(&rshdiscmdQ, entry);

	invalid = rsh_invalid_commands[i++];
    }
    
    if (!system_loading) {
	/* copy rcp ... */
	parser_add_link_point(PARSE_ADD_COPY_RCP_CMD,
			      "copy rcp ...",
			      &pname(exec_copy_rcp_extend_here));
	parser_add_link_point(PARSE_ADD_COPY_RCP_FILE_CMD,
			      "copy rcp ...",
			      &pname(exec_copy_rcp_file_extend_here));
    }
    parser_add_command_list(rcp_chain_table, "rcp");

    /*
     * Register some functions.
     */
    reg_add_file_match(FILE_ACCESS_RCP, rcp_file_match, "rcp_file_match");
    reg_add_bootinfo_get(FILE_ACCESS_RCP, rcp_bootinfo_get,
			 "rcp_bootinfo_get");
    reg_add_bootinfo_set(FILE_ACCESS_RCP, rcp_bootinfo_set,
			 "rcp_bootinfo_set");
    reg_add_file_parse(FILE_ACCESS_RCP, rcp_file_parse, "rcp_file_parse");
    reg_add_bootfile_parse(rcp_bootfile_parse, "rcp_bootfile_parse");
    reg_add_file_read(FILE_ACCESS_RCP, rcp_file_read, "rcp_file_read");
    reg_add_file_write(FILE_ACCESS_RCP, rcp_file_write, "rcp_file_write");

    reg_add_config_read(FILE_ACCESS_RCP, ip_get_config_file,
			"ip_get_config_file");
    reg_add_config_write(FILE_ACCESS_RCP, ip_put_config_file,
			 "ip_put_config_file");
     
    reg_add_file_source(FILE_ACCESS_RCP, rcp_file_source, "rcp_file_source");

    reg_add_get_remote_username(FILE_ACCESS_RCP, rcmd_get_remote_username,
			        "rcmd_get_remote_username");

    reg_add_rcp_write(rcp_write, "rcp_write");
    reg_add_rcp_send(rcp_send, "rcp_send");
    reg_add_rcp_receive(rcp_receive, "rcp_receive");

    reg_add_core_dump(FILE_ACCESS_RCP, rcp_dump_core_file,
		      "rcp_dump_core_file");

    rsh_init();
}

/*
 * RCP subsystem header
 */

#define RCP_MAJVERSION	1
#define RCP_MINVERSION	0
#define RCP_EDITVERSION	1

SUBSYS_HEADER(rcp, RCP_MAJVERSION, RCP_MINVERSION,
	      RCP_EDITVERSION, rcp_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      subsys_req_iphost);
