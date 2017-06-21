/* $Id: telnet.c,v 3.14.8.17 1996/09/11 11:49:18 che Exp $
 * $Source: /release/112/cvs/Xsys/tcp/telnet.c,v $
 *------------------------------------------------------------------
 * telnet.c -- TCP based Telnet, RFC 764, et al.
 *
 * 29-November-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: telnet.c,v $
 * Revision 3.14.8.17  1996/09/11  11:49:18  che
 * CSCdi68766:  Router always returns WILL_AUTHEN if it receives DO_AUTHEN
 * Branch: California_branch
 * Add a "break" for the TNO_AUTH case in telnetuser_optioncheck().
 *
 * Revision 3.14.8.16  1996/09/05  18:34:55  che
 * CSCdi68169:  Kerb telnet into router will fail if kerb clients mandatory
 * Branch: California_branch
 * Check for "kerberos clients mandatory" only if we are the telnet client.
 *
 * Revision 3.14.8.15  1996/08/28  23:13:12  asb
 * CSCdi51882:  Telnet doesnt process RFC 1073 [NAWS] options for window
 * size
 * Branch: California_branch
 *
 * Revision 3.14.8.14  1996/08/28  13:17:27  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.14.8.13  1996/08/07  09:03:01  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.14.8.12  1996/08/05  16:16:32  swenson
 * CSCdi63963:  telnet suboption debugging uses printf not buginf
 * Branch: California_branch
 *
 * Revision 3.14.8.11  1996/08/02  06:00:09  irfan
 * CSCdi64901:  telnet has some data arrays, could be constantized
 * Branch: California_branch
 * change some data arrays  to const arrays.
 *
 * Revision 3.14.8.10  1996/08/01  21:56:14  ahh
 * CSCdi64871:  TELNET: fence-post error with multiple destination
 * addresses
 * Branch: California_branch
 *
 * Revision 3.14.8.9  1996/07/17  00:44:14  che
 * CSCdi61914:  kerberos mandatory broken
 * Branch: California_branch
 * Force telnet client to fail and exit if "kerberos clients mandatory"
 * is configured and we can't successfully authenticate via Kerberized
 * Telnet.
 *
 * Revision 3.14.8.8  1996/07/12  20:09:32  widmer
 * CSCdi62775:  Bogus externs in telnet.c cause 4k builds to fail
 * Branch: California_branch
 * Remove externs and move externs to header files
 *
 * Revision 3.14.8.7  1996/06/29  00:11:39  snyder
 * CSCdi61694:  change some char * to char [] in ip
 * Branch: California_branch
 *     save 116 bytes
 *
 * Revision 3.14.8.6  1996/06/28  23:30:28  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.14.8.5  1996/06/12  14:41:48  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.14.8.4  1996/06/04  08:36:34  che
 * CSCdi57424:  router crashes attempting kerberos telnet
 * Branch: California_branch
 * Added kerberos_stackcheck() registry to ensure correct stacksize of
 * process running kerberos clients.
 *
 * Revision 3.14.8.3  1996/05/04  02:07:56  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.14.8.2  1996/04/19  15:33:49  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.2.6.14  1996/04/16  03:57:12  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 2.2.6.13  1996/04/09  22:04:09  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.2.6.12  1996/03/28  08:44:51  che
 * Branch: IosSec_branch
 * Introduce function to release memory in conn->proto.tn.kerberos_info
 *
 * Revision 2.2.6.11  1996/03/27  23:32:36  che
 * Branch: IosSec_branch
 * More Kerberos authentication cleanup
 *
 * Revision 2.2.6.10  1996/03/26  04:29:45  che
 * Branch: IosSec_branch
 * Fix typo in telnet.c
 *
 * Revision 2.2.6.9  1996/03/25  20:55:48  che
 * Branch: IosSec_branch
 * Clean up how authentication negotiation works
 *
 * Revision 2.2.6.8  1996/03/19  08:48:15  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.2.6.7  1996/03/12  04:36:09  che
 * CSCdi51296:  7000 crashes attempting kerberos telnet
 * Branch: IosSec_branch
 * Negotiate Telnet authentication suboption(for Kerberos V5) only if we
 * have Kerberos credentials and a valid ustruct.
 *
 * Revision 2.2.6.6  1996/03/05  04:44:45  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 2.2.6.5  1996/02/21  04:13:50  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.2.6.4  1996/01/23  06:51:13  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.2.6.3  1995/12/19  04:20:25  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.2.6.2  1995/12/17  02:32:27  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.2.6.1  1995/11/17  18:36:06  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.14.8.1  1996/03/18  22:14:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.2.3  1996/03/16  07:41:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.2.2  1996/03/07  10:53:26  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.9.2.1  1996/02/20  21:21:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.14  1996/02/27  22:24:00  jenny
 * CSCdi44663:  ASM line stuck in Carrier Dropped state.
 * Don't block in TCP on write unless user spcified so to avoid possible
 * lost control of the tty during TCP close.
 *
 * Revision 3.13  1996/02/23  23:38:59  ahh
 * CSCdi47841:  Running some telnet/pad sessions over serial causes rout...
 * Allow tcp_uniqueport() to fail, and clean up some debug messages.
 *
 * Revision 3.12  1996/02/03  01:57:43  perryl
 * CSCdi47740:  Service hide-addr hiding is incomplete
 *
 * Revision 3.11  1996/01/24  20:41:35  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.10  1996/01/23  23:45:50  vandys
 * CSCdi45547:  Desire rotary behavior on PT TCP destinations
 * Add rotor option to randomize destination for TCP host lists
 *
 * Revision 3.9  1996/01/22  07:29:32  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.8  1996/01/16  03:38:08  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.7  1996/01/12  02:41:09  billw
 * CSCdi44858:  use #define for AAA attribute values
 * Bring the AV pairs used up to spec level...
 *
 * Revision 3.6  1996/01/11  02:34:07  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.5  1995/12/28  18:29:38  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreign addresses
 *   to tcp_open() for future portability.
 *
 * Revision 3.4  1995/12/13  02:59:43  billw
 * CSCdi42536:  aaa_acct not proper subsystem
 * Move some code around, change some calls to reg_invokes, etc.
 *
 * Revision 3.3  1995/11/20  23:22:12  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  18:49:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  21:32:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/10/14  01:09:04  billw
 * CSCdi40774:  AAA accounting for connections contains no port info
 * Put this the the cmdarg AVpair.
 *
 * Revision 2.10  1995/09/28  22:39:31  billw
 * CSCdi41159:  2509 does not answer DO TIMING-MARK from client
 *
 * Revision 2.9  1995/09/11  21:43:09  tkolar
 * CSCdi40022:  Telnet speed is retained even after the session is closed
 * Move passspeed and maxpassspeed into the userdata struct.
 *
 * Revision 2.8  1995/08/02  06:39:34  irfan
 * CSCdi38059:  resuming rlogin connections garbles async data on Copans
 * rlogin connections should start in cooked mode (flow sw). Avoid playing
 * with the flow control, if HW flow control is enabled on the line, for
 * both rlogin and telnet connections.
 *
 * Revision 2.7  1995/07/12  23:45:56  hampton
 * Convert TCP to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37015]
 *
 * Revision 2.6  1995/07/11 23:45:49  billw
 * CSCdi35800:  Need knob to supress user view of IP addresses
 *
 * Revision 2.5  1995/06/20  03:45:04  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.4  1995/06/17  06:26:54  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.3  1995/06/14  19:22:59  vandys
 * CSCdi35265:  Should use intfc description if no line description
 * Don't reference tty->location directly; pass tt_soc to a
 * function, let it take from either tt_soc or IDB.
 *
 * Revision 2.2  1995/06/09  23:40:23  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.1  1995/06/07  23:06:29  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "../h/auth_registry.regh"
#include "packet.h"
#include "ttysrv.h"
#include "../ui/debug.h"
#include "../os/os_debug.h"
#include "../ip/ip.h"
#include "tcp.h"
#include "tcpinternal.h"
#include "connect.h"
#include "config.h"
#include "address.h"
#include "name.h"
#include "telnet.h"
#include "telnet_debug.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/ipoptions.h"
#include "logger.h"
#include "../ts/ttystatem.h"
#include "../snmp/sr_old_ltsmib.h"
#include "../clns/clns_externs.h"
#include "../parser/parser_defs_exec.h"
#include "../ip/ip_registry.h"
#include "../h/cs_registry.regh"
#include "aaa.h"
#include "../dspu/snanm.h"
#include "../dspu/snanm_registry.regh"
#include "../ui/service.h"
#include "../if/network.h"
#include "../util/random.h"
#include "msg_telnet.c"			/* Not a typo, see logger.h */
#include "../os/async.h"


/*
 * local storage
 */

#define MAXCHRS 50
#define MAXBYTES 400		/* resched after this many characters */

static const uchar telnet_chars[32] = {	/* mapping between ascii*/
                                        /* and telnet chars */
	0,			/* ^@	, etc */
	0,			/* A */
	TN_BREAK,		/* ^B, B, b : BREAK */
	TN_IP,			/* ^C, C, c : Interrupt Process */
	0,			/* D */
	0,			/* E */
	0,			/* F */
	0,			/* G */
	TN_EC,			/* H :	Erase Character*/
	0,			/* I */

	0,			/* J */
	0,			/* K */
	0,			/* L */
	0,			/* M */
	0,			/* N */
	TN_AO,			/* O	: Abort output */
	0,			/* P */
	0,			/* Q */
	0,			/* R */
	0,			/* S */

	TN_AYT,			/* T	: Are you there? */
	TN_EL,			/* U	: Erase Line */
	0,			/* V */
	0,			/* W */
	0,			/* X */
	0,			/* Y */
	0,			/* Z */
	0,			/* */
	0,			/* */
	0,			/* */

	0,			/* */
	0			/* ? */
};

static const char *telnet_names[32] = {	/* mapping between ascii and */
                                        /*    telnet chars */
	NULL,			/* ^@	, etc */
	NULL,			/* A */
	"BREAK",		/* ^B, B, b : BREAK */
	"IP",			/* ^C, C, c : Interrupt Process */
	NULL,			/* D */
	NULL,			/* E */
	NULL,			/* F */
	NULL,			/* G */
	"EC",			/* H : Erase Character */
	NULL,			/* I */
	NULL,			/* J */
	NULL,			/* K */
	NULL,			/* L */
	NULL,			/* M */
	NULL,			/* N */
	"AO",			/* O	: Abort output */
	NULL,			/* P */
	NULL,			/* Q */
	NULL,			/* R */
	NULL,			/* S */
	"AYT",			/* T	: Are you there? */
	"EL",			/* U	: Erase Line */
	NULL,			/* V */
	NULL,			/* W */
	NULL,			/* X */
	NULL,			/* Y */
	NULL,			/* Z */
	NULL,			/* */
	NULL,			/* */
	NULL,			/* */
	NULL,			/* */
	NULL			/* */
};
/*
 * Possible tn3270 states (bits)
 */
/* initial states */
/* CMDNOTSENTSTATE indicates user did not type tn3270 command */
/* CMDSENTSTATE indicates user did type tn3270 command        */
#define CMDNOTSENTSTATE 0
#define CMDSENTSTATE 0x01
/* TERMINALTYPESENTSTATE indicates remote side negotiated terminal type option*/
#define TERMINALTYPESENTSTATE 0x02
/* HEWILLBINARYSTATE indicates remote side negotiated binary */
#define HEWILLBINARYSTATE 0x04
/* WEWILLBINARYSTATE indicates local side negotiated binary */
#define WEWILLBINARYSTATE 0x08
/*
 * If the tn3270cmd member of a connection block contains the value
 * TN3270STATE then we have statisfied the criteria for a tn3270 connection.
 * Kind of scary, but it is the standard convention 
 */
#define TN3270STATE       0x0F

idbtype *telnet_source_idb;		/* IDB to use for source address */

/*
 * telnet_init
 * Set up global stuff for the TELNET module
 */
void telnet_init(void)
{
    telnet_debug_init();
}

/*
 * open_connection
 * Open a TCP connection to a foreign host.
 * Returns pointer to a connection block on success, NULL otherwise.
 *
 * Parameters:
 * ptr     pointer to name cache entry, or NULL
 * foreignhost foreign host address (ptr is NULL)
 * foreignport foreign port
 * streamflag TRUE if stream connection
 * errptr  error code.  If NULL, print messages
 * ipopts  IP options to be used (source route)
 * debugflag TRUE if want debugging output
 * linemode TRUE if want Telnet line mode
 * tn3270cmd TRUE if tn3270 command got me here
 * abortflag TRUE if connection attempt aborted
 */
conntype *open_connection (nametype *ptr, addrtype *foreignhost,
			   ushort foreignport, addrtype *localhost,
			   int *errptr, ipoptiontype *ipopts,
			   uint flags, boolean *abortflag)
{
    int nhosts, i, start, next, status;
    addrtype fhost;
    ipaddrtype fhosts[MAXIPADDRESSES];
    short lport, fport;
    tcbtype *tcb;
    conntype *conn;
    ipoptiontype *myopts = NULL;

    if (abortflag != NULL)
	*abortflag = TRUE;
    /*
     * Set up the connection block.
     */ 
    conn = getconnection(stdio, &tcp_connvector);
    if (!conn) {
	if (!errptr)
	    printf(nomemory);
	else
	    *errptr = NO_MEMORY;
	return(NULL);
    }
    conn->debug = ((flags & OPENFL_DEBUG) != 0);
    conn->proto.tn.statechange_check = telnetuser_optioncheck;
    conn->proto.tn.statechange_done = telnetuser_optiondone;
    conn->proto.tn.tn3270cmd = ((flags & OPENFL_TN3270) ?
	CMDSENTSTATE : CMDNOTSENTSTATE);
    conn->proto.tn.in3270mode = FALSE;
    conn->proto.tn.tn3270inited = FALSE;
    conn->proto.tn.do_kerb_auth = FALSE;
    conn->proto.tn.kerberos_info = NULL;

    /*
     * Generate list of destinations, with count
     */
    if (!ptr) {
	fhosts[0] = foreignhost->addr.ip_address;
	nhosts = 1;
    } else {
	/*
	 * Count them up
	 */
	for (i = 0; i < MAXIPADDRESSES; ++i) {
	    fhosts[i] = ptr->v.ip.adr[i].addr.ip_address;
	    if (fhosts[i] == 0) {
		break;
	    }
	}

	/*
	 * No hosts--can't do the connection ("Can't happen")
	 */
	if (i == 0) {
	    return(NULL);
	}

	/*
	 * There they are
	 */
	nhosts = i;
    }

    /*
     * Pick a starting point; 0, unless they asked for a random
     * rotor
     */
    if (flags & OPENFL_ROTOR) {
	start = (random_gen() % nhosts);
    } else {
	start = 0;
    }

    /*
     * Step through addresses until we succeed or run
     * out of alternatives.
     */
    lport = tcp_uniqueport(stdio->ttynum);
    if (lport == 0) {
	printf("\n%% Out of local ports");
	if (conn->proto.tn.kerberos_info) {
	    reg_invoke_kerberos_free_kerberos_info( conn->proto.tn.kerberos_info);
	    conn->proto.tn.kerberos_info = NULL;
	}
	freeconnection(conn);
	return(NULL);
    }
    fport = TELNET_PORT;
    i = start;
    tcb = NULL;
    do {
	/*
	 * Calculate next step forward with wrap at end of fhosts[] list
	 */
	next = i+1;
	if (next >= nhosts) {
	    next = 0;
	}

	/*
	 * Get next address to attempt
	 */
	ip_copy_address(fhosts[i], &fhost);

	/*
	 * Print out message indicating what we're doing, at least as
	 * much as we are supposed to.  This is made complex by
	 * "service hide_telnet_addresses", which is a special hack
	 * to meet needs of some service provider scripts.
	 * If busy-str is set, do not display Trying... message.
	 *
	 *    errptr	hideaddr     Start	Intermediate	OK	Fail
	 *   (busy-str) (service)
	 *      0	  0	     "try..."   <addr, errs>   "Open"  <reason>
	 *	0	  1	     "try..."    #n, errs      "Open"  <reason>
	 *	1	  0          <nul>      <nul>          <nul>   <busy-str>
	 *      1	  1          <nul>      <nul>          <nul>   <busy-str>
	 */
	if (!errptr) {
	    if (service_hideaddr) {
		if (ptr)
		    printf("Trying %s address #%d ... ", ptr->name, i+1);
		else
		    printf("Trying ... ");
		flush();
	    } else {
		if (ptr) {
		    printf("Trying %s (%s", ptr->name, address_string(&fhost));
		    if (foreignport != TELNET_PORT) {
			printf(", %d",foreignport);
		    }
		    printf(")... ");
		} else {
		    printf("Trying %s", address_string(&fhost));
		    if (foreignport != TELNET_PORT) {
			printf(", %d", foreignport);
		    }
		    printf(" ... ");
		}
		flush();
	    }
	}

	/*
	 * Perform access checking on the current address. 
	 */
	if (!check_access(stdio, fhost.ip_addr, lport, foreignport, TRUE) ||
	    !reg_invoke_login_connect(fhost.ip_addr, foreignport)) {
	    if (!errptr)
		printf("\n%% Connections to that host not permitted from this terminal");
	    if (ptr) {
		i = next;
		continue;
	    }
	    if (conn->proto.tn.kerberos_info) {
		reg_invoke_kerberos_free_kerberos_info(
						conn->proto.tn.kerberos_info);
		conn->proto.tn.kerberos_info = NULL;
	    }
	    freeconnection(conn);
	    return(NULL);
	}

	/*
	 * Try making the connection.  Check for martians.
	 */
	if (martian(fhost.ip_addr, TRUE)) {
	    printf("%%invalid host address");
	    if (ptr) {
		i = next;
		continue;
	    }
	    if (conn->proto.tn.kerberos_info) {
		reg_invoke_kerberos_free_kerberos_info(
						conn->proto.tn.kerberos_info);
		conn->proto.tn.kerberos_info = NULL;
	    }
	    freeconnection(conn);
	    return(NULL);
	}

	/*
	 * Create a copy of any IP header option list
	 * We need to copy since the TCB's option list is discarded
	 * when the TCB is deallocated. 
	 */
	if (ipopts != NULL) {
	    myopts = malloc(sizeof(ipoptiontype));
	    if (myopts)
		bcopy(ipopts, myopts, sizeof(ipoptiontype));
	}

	fport = foreignport ? foreignport : TELNET_PORT;
        if (conn->proto.tn.tn3270cmd) {
            if (!reg_invoke_tn3270_conn_init1(conn)) {
		printf("%%Trouble allocating tn3270 structures\n");
		if (conn->proto.tn.kerberos_info) {
		    reg_invoke_kerberos_free_kerberos_info(
						conn->proto.tn.kerberos_info);
		    conn->proto.tn.kerberos_info = NULL;
		}
		freeconnection(conn);
		return(NULL);
            }
	}
	tcb = tcp_open(stdio, fport, &fhost, lport, localhost, &status, myopts);
	if (!tcb) {
	    myopts = NULL;		/* options deallocated by tcp_open */
	    if (!errptr)
		printf("\n%% %s\n", tcperror2string(status));
	    else
	        *errptr = status;
	    if (service_hideaddr && errptr)
		printc('\n');
	    if (status == LOCAL_RESET) {
		if (abortflag)
		  *abortflag = TRUE; /* Mark as user abort */
		break;		/* Don't loop if user did a reset */
	    }
	}

 	if (status == OPTSTOOBIG || tcb || !ptr)
	    break;

	/*
	 * Advance to next step
	 */
	i = next;

    } while (i != start);

    /*
     * If we failed to make a connection, flush connection block
     * and (possibly) report error.
     */
    if (!tcb) {
        reg_invoke_tn3270_conn_terminate(conn, FALSE);
	if (conn->proto.tn.kerberos_info) {
	    reg_invoke_kerberos_free_kerberos_info(conn->proto.tn.kerberos_info);
	    conn->proto.tn.kerberos_info = NULL;
	}
	freeconnection(conn);
	return(NULL);
    }

    /*
     * We succeeded.  Finish connection setup.
     * If busy-message is set, turn on the quiet flag, CONN_QUIET
     */
    if (!errptr)
	printf("Open\n");
    else
	conn->status |= CONN_QUIET;

    if (flags & OPENFL_STREAM) {
	conn->status |= CONN_STREAM;
	conn->snmp_sesstype = D_tslineSesType_stream;
    }

    if (flags & OPENFL_LINE)
	conn->proto.tn.tn_option |= STO_LINE|STO_EDIT;
    conn->hostname[0] = conn->logicalname[0] = '\0';
    conn->tcb = tcb;
    address_copy(&conn->address, &fhost);
    if (!(flags & OPENFL_STREAM) &&
	    (fport == TELNET_PORT) && (!conn->proto.tn.tn3270cmd)) {
	/*
	 * Tell the other side a bunch about ourselves and what
	 * options we can support.
	 */
	send_option(conn, STH_SUPRGA, STH_SUPRGA, TN_DO, TNO_SUPRGA);
	if (stdio->termtype)
	    send_option(conn, STO_TTYTYPE, STO_TTYTYPE, TN_WILL, TNO_TTYTYPE);
	if (get_tty_location(stdio) || stdio->conntop)
	    send_option(conn, STO_TTYLOC, STO_TTYLOC, TN_WILL, TNO_TTYLOC);
	if (stdio->tty_txspeed && stdio->tty_rxspeed)
	    send_option(conn, STO_TTYSPEED, STO_TTYSPEED, TN_WILL, TNO_TTYSPEED);
	if (stdio->tty_width || stdio->tty_length) {
	    send_option(conn, STO_NAWS, STO_NAWS, TN_WILL, TNO_NAWS);
	}
	    
	send_option(conn, STO_LFLOW, STO_LFLOW, TN_WILL, TNO_LFLOW);

	tcp_putpush(conn->tcb,TRUE,FALSE);
    }
    if (conn->proto.tn.tn3270cmd && conn->proto.tn.tn3270inited) {
	tcb->flags |= TCB_ALWAYSPUSH; /* Make sure we always push these paks */
        reg_invoke_tn3270_conn_init2(conn);
        conn->proto.tn.tn3270eorin = 1;
    }
    if (abortflag != NULL)
	*abortflag = FALSE;
    return(conn);
}

/*
 * close_connection
 * Close the specified TCP connection.
 */

void close_connection (conntype *conn)
{
    tcbtype *tcb;

    if (!conn)
	return;
    reg_invoke_tcp_tty_close(conn);

    /* NmEvent Hook 1 */
    reg_invoke_Tcp_Alert(conn);

    tcb = conn->tcb;
    reg_invoke_aaa_acct_end(conn->tty->ustruct, OK, conn->acct_info,
			    tcb->bytesincount, tcb->bytesoutcount,
			    tcb->pakincount, tcb->pakoutcount);
    if (tcb) {
	if (tcb->state < ESTAB)
	    tcp_abort(tcb);
	else
	    tcp_close(tcb);
    }
    if (conn->proto.tn.tn3270inited) {
        reg_invoke_tn3270_conn_terminate(conn, TRUE);
    }
    if (conn->proto.tn.kerberos_info) {
	reg_invoke_kerberos_free_kerberos_info(conn->proto.tn.kerberos_info);
	conn->proto.tn.kerberos_info = NULL;
    }
    freeconnection(conn);	/* release connection block */
}

/*
 * Outguess
 * Return our best guess as to how many bytes we will send to the net from the
 * tty input buffer.  If no terminal, assume a medium sized buffer.
 * If we are a virtual terminal, also assume medium size, since we will
 * usually guess wrong about how much data is pending.  Note that the code
 * is carefully arranged so that the guess will be at LEAST the guess amount,
 * even when we have special cases to provide a "bigger" (MEDDATA) amount.
 */

int outguess (tt_soc *tty)
{
    int guess;

    guess = 10 + (*(tty->ovector->vinputpending))(tty);
    /*
     * add in data based on dispatch timeout.  Note that 9600bps is
     * approximately one byte per millisecond, so this ends up being
     * milliseconds times expected bytes per millisecond.
     */
    if (tty->tty_rxspeed)
	guess += (tty->tty_dispatchtimeout * tty->tty_rxspeed)/9600;
    else			    /* Assume 9600 if no speed specified */
	guess += tty->tty_dispatchtimeout;

    if (guess > (MEDDATA-(TCPHEADERBYTES + MINIPHEADERBYTES)))
	return(BIGDATA-(TCPHEADERBYTES + MINIPHEADERBYTES));

    if (tty->type == VTY_LINE)
	return(MEDDATA-(TCPHEADERBYTES + MINIPHEADERBYTES));

    /*
     * Try to guess if a line is in milking machine mode, and if so,
     * allocate a small buffer if we are just echoing, and a medium
     * buffer if it looks like there might be more.
     */
    if ((guess > 11) && (tty->capabilities & NOEXEC))
	return(MEDDATA-(TCPHEADERBYTES + MINIPHEADERBYTES));
	
    if (guess > SMALLDATA-(TCPHEADERBYTES + MINIPHEADERBYTES))
	return(MEDDATA-(TCPHEADERBYTES + MINIPHEADERBYTES));

    return(SMALLDATA-(TCPHEADERBYTES + MINIPHEADERBYTES));
}

/*
 * telnetBLOCK
 * Event dismiss for the telnet routine.  Check for I/O after checking
 * all the special conditions.  This prevents large amounts of output
 * from locking out recognition of the interrupt sequence, carrier drop, etc.
 */

boolean telnetBLOCK (register conntype *conn)
{
    register tt_soc *tty = conn->tty;

    conn->status |= CONN_CHECKALL;
    if (TIMER_RUNNING_AND_AWAKE(conn->dispatchtimer))
	return(FALSE);
    if (psiset (tty) || modemcheck (tty))
	return(FALSE);
    if (!SLEEPING(conn->notetime))
	return(FALSE);

    /*
     * exec-timeout is used for incoming telnet connections only, and is 
     * ignored in outgoing connections, even if the outgoing connection 
     * was made from the incoming telnet.
     */ 
    if (!(tty->statbits & CONNECTED)) {
	if (tty->tty_exectimeout && TIMER_RUNNING_AND_AWAKE(tty->dallytimer)) {
	    tty->statbits |= IDLETIMEOUT;
	    return(FALSE);
	}
    } 

    /*
     * session-timeout is used for outgoing telnet connections only.
     */
    if (tty->tty_sessiontimeout && conn_session(conn, TRUE) == FALSE)
	return(FALSE);

    if (TIMER_RUNNING_AND_AWAKE(conn->logintimer))
	return(FALSE);
    conn->status &= ~CONN_CHECKALL;
    if (inputpending(tty))
	return(FALSE);

    /*
     * Unblock if there is data from TCP and we can send data to the tty.
     */
    if ((tcp_unread(conn->tcb) != 0) && (!outputblock(tty)))
	return(FALSE);

    /*
     * Unblock if we blocked here because write to TCP would've blocked
     * previously, and write to TCP now won't block.
     */
    if ((conn->status & CONN_WRITEBLOCK) && (!outputblock(tty))) {
	conn->status &= ~CONN_WRITEBLOCK;
	return(FALSE);
    }

    /*
     * Unblock if there is state change in TCP.
     */
    if (tcp_state_activity(conn->tcb)) 
	return(FALSE);

    return(TRUE);
}

/*
 * tcp_parse_resume
 * Parse the resume switches for a tcp connection.
 */
void tcp_parse_resume (conntype *conn, parseinfo *csb)
{
    nametype *ptr;

    ptr = name_cache_lookup (conn->logicalname, NULL, NAM_IP);
    if (ptr)
	conn->loginstring = ptr->resumestring;

    if (GETOBJ(int,2)) {		/*  resume /debug  */
	conn->debug = TRUE;
    }
    if (GETOBJ(int,3)) {		/*  resume /nodebug  */
	conn->debug = FALSE;
    }
    if (GETOBJ(int,4)) {		/*  resume /echo  */
	conn->proto.tn.tn_option &= ~STH_ECHO;
    }
    if (GETOBJ(int,5)) {		/* resume /noecho  */
	conn->proto.tn.tn_option |= STH_ECHO;
    }
    if (GETOBJ(int,6)) {		/*  resume /line  */
	conn->proto.tn.tn_option |= STO_EDIT;
    }
    if (GETOBJ(int,7)) {		/*  resume /noline  */
	conn->proto.tn.tn_option &= ~STO_EDIT;
    }
    if (GETOBJ(int,8)) {		/*  resume /stream  */
	conn->status |= CONN_STREAM;
	conn->snmp_sesstype = D_tslineSesType_stream;
    }
    if (GETOBJ(int,9)) {		/*  resume /nostream  */
	conn->status &= ~CONN_STREAM;
	conn->snmp_sesstype = D_tslineSesType_telnet;
    }
}	

/* 
 * send_byte_block
 *
 * Try to send one byte to TCP, block in telnetBLOCK if write to TCP
 * failed.
 * 
 */
static boolean send_byte_block (conntype *conn, int ch)
{ 
    tcbtype * tcb;
    tt_soc *tty;
    int code;
    long oldcaps;

    tty = conn->tty;
    tcb = conn->tcb;
    oldcaps = conn->tty->capabilities;
    
    while (tcp_putbyte(tcb, ch, FALSE) == FALSE) {
	/*
	 * Block in telnetBLOCK if blockflag is FALSE so that we can
	 * be in control of the tty. If user specifically passed 
	 * blockflag as TRUE, that process will be blocked in TCP when 
	 * tcp_putbyte fails. tty_dallytimer() will start either the 
	 * exec_timeout or session_timeout for this connection.
	 */
	conn->status |= CONN_WRITEBLOCK; 
	tty_dallytimer(tty);
	edisms((blockproc *)telnetBLOCK, (ulong)conn);
	/*
	 * Something happened to this tty while we blocked in 
	 * telnetBLOCK, stop trying to send the data and return. 
	 */
	if (!tty_status_ok(conn, tcb, tty, oldcaps, &code)) {
	    process_may_suspend();
	    return(FALSE);
	}
    }
    /*
     * tcp_putbyte sent the one byte data ok.
     */
    return(TRUE);
}

/*
 * telnet
 * Perform telnet protocol action on the specified connection.
 * If connection was created as a TCP stream, don't do telnet protocol.
 * Returns I/O status code.
 */

int telnet (conntype *conn, int resumeflag)
{
    tcbtype * tcb;
    tt_soc *tty;
    int ch;			        /* char temporary */
    boolean streamflag;           	/* TRUE if a pure TCP stream */
    int n;			        /* fairness counter */
    boolean pushflag;	 	        /* non-zero if must send to net */
    long oldcaps;			/* terminal capabilities on entry */
    int status;				/* status codes */
    int datacount;
    uchar *dataptr;
    int code;

    tty = conn->tty;			/* set up tty pointer */
    tcb = conn->tcb;			/* set up tcb pointer */
    streamflag = conn->status & CONN_STREAM; /* data stream or telnet */
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
    conn->proto.tn.tn_state &= ~ST_FLUSH;		/* Stop flushing */
    tcp_idle(tcb,FALSE);		/* we're not idle any more */
    pushflag = FALSE;			/* not pushing data yet */
    oldcaps = tty->capabilities;	/* save original caps word */    
    if (conn->proto.tn.tn_option & STO_LFLOW) { 
	/*
	 * If we can negotiate flow control, restore state of terminal
	 * flow control to that negotiated for this connection.
	 */
	if (conn->status & CONN_FLOW)
	    conn_setsflow(tty, TRUE);
	else
	    conn_setsflow(tty, FALSE);
    }
    if (resumeflag && conn->proto.tn.in3270mode) {
	reg_invoke_tn3270_conn_resume(conn);
    }
    while (TRUE) {
	edisms ((blockproc *)telnetBLOCK, (ulong)conn);
	if (!tty_status_ok(conn, tcb, tty, oldcaps, &code))
	    return(code);
	/*
	 * Handle input from the net (output to the tty)
 	 */
	n = MAXBYTES;
	if (tcp_unread(tcb) || tcp_closing(tcb)) {
	    while (n > 0) {
	        status = tcp_getstring(tcb, &dataptr, &datacount,
			tcp_chunksize ? tcp_chunksize :MAXINT);
		if (status == NO_DATA)
		    break;
	        switch (status) {
		    case URGENT_DATA:
			if (conn->debug) {
			    printf("\n[TN: urgent rcvd]");
			    flush();
			}
			if (telnet_debug && (conn == conn->tty->conntop))
			    buginf("\nTCP%t: Telnet: urgent rcvd",
				conn->tty->ttynum);
		        if (!streamflag) {
			    clearoutput(tty);
			    conn->proto.tn.tn_state |= ST_DM;
			}
		    case OK:
		        if (!streamflag)
			  status = telnet_input(conn, dataptr, datacount);
			else
			  status = putpkt(tty, tcb->curinputpak,
					    dataptr, datacount);
			datacount -= status; /* Get bytes not moved */
			tcp_putback(tcb, datacount);
			if (!status)
			  n = -1; /* Nothing output, don't loop */
			else
			  n -= status;
			if (tty->tty_sessiontimeout &&
			    (tty->capabilities & OUTIDLE))
			    start_sessiontimeout(conn, tty);
			break;
		    case NO_DATA:
		        break;
		    default:
		        status = tcp_lsterr(tcb);
		        if (status == OK)
			    status = END_OF_FILE;
			conn_restoreflow(tty, oldcaps);
	    	        return(status);
	        } /* switch */
		if (inputpending(tty))
		    break;
	    } /* while */
	    (void)startoutput(tty);		/* ensure output is running */
	} /* if */

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
	    if (streamflag) {	/* doing TCP byte stream */
		n = MAXBYTES;
		if (tty->tty_dispatchtimeout) {
		    if (!TIMER_RUNNING(conn->dispatchtimer))
			TIMER_START(conn->dispatchtimer,
				    tty->tty_dispatchtimeout);
		} else
		    pushflag = TRUE;
		while (--n) {
		    ch = getc(tty);	/* get byte from tty */
		    if (ch == -1)	/* break if no tty data */
		        break;
		    if (tty->tty_editinfo.dispatch[ch & 0x7F] & TTY_DISP_DISP)
			pushflag = TRUE;
		    if (tty->dispatch_machine)
			pushflag = tty_runstates(conn, ch);
		    if (tcp_putbyte(tcb, ch, FALSE) == FALSE) {
			ungetc(tty,ch);
		        break;
		        } /* if */
		} /* while */
	    } /* if */
	    else {			/* doing telnet protocol */
		register int echoflag;
		int result;
		if (conn->proto.tn.tn_option & STO_EDIT) { /* local editing */
		    result = telnet_getline(conn);
		    switch (result) {
			case READLINE_DONE:
			    pushflag = TRUE;
			    break;
			case READLINE_BLOCK:
			    break;
			default:
			    errmsg(&msgsym(READLINE, TN), result);
		    }
		} else {
		    
		echoflag = (conn->proto.tn.tn_option & STH_ECHO);
		n = MAXBYTES;
		if (tty->tty_dispatchtimeout) {
		    if (!TIMER_RUNNING(conn->dispatchtimer))
			TIMER_START(conn->dispatchtimer,
				    tty->tty_dispatchtimeout);
		}
		else
		    pushflag = TRUE;
		while (--n) {
		    ch = getc (tty);	/* get byte from tty */
		    if ((ch == -1) || (tty->statbits & ESCAPED))
			break;		/* quit if no tty input data */
                    if (conn->proto.tn.in3270mode) {
                        reg_invoke_tn3270_user_keypress(ch, conn);
                    } else {
		        if (tty->tty_editinfo.dispatch[ch & 0x7F]
                                                               & TTY_DISP_DISP)
			    pushflag = TRUE;
		        if (tty->dispatch_machine)
			    pushflag = tty_runstates(conn, ch);
		        if (tnof(conn,ch,FALSE,FALSE) == FALSE)
			    break;		/* quit if can't do output */
		        if (echoflag == FALSE) {
			    if (((conn->proto.tn.tn_option & STO_BINARY) == 0)&&
				    ((tty->capabilities & TRANSPARENT) == 0))
			        printc(ch);
			    else
			        putc(tty,ch);
                        }
		    }
		} /* while */
		if (echoflag == FALSE)
		    startoutput(tty);
		} /* telnet protocol */
	    }
	}   /* if inputpending(tty) */
	if (pushflag || TIMER_RUNNING_AND_AWAKE(conn->dispatchtimer)) {
	    tcp_putpush(tcb,TRUE,FALSE);
	    pushflag = FALSE;
	    TIMER_STOP(conn->dispatchtimer);
	    tty->dispatch_state = 0;
	}
    }   /* while (TRUE) */
}

/*
 * tnof
 * telnet protocol output filter.
 * Returns TRUE if output possible, FALSE otherwise.
 */

boolean tnof (conntype *conn, int ch, boolean blockflag, boolean serverflag)
{
    tcbtype *tcb;
    char sentbuf[6];

    tcb = conn->tcb;
    /*
     * If we are in ASCII mode (i.e., we haven't negotiated the Binary
     * option or are forcing binary), we must clear bit 8 and do
     * CR -> CRLF hacking.
     */
    if (((conn->proto.tn.tn_option & STO_BINARY) == 0) && 
			((conn->tty->capabilities & TRANSPARENT) == 0)) {
	/*
	 * Ensure that we're sending only 7-bit ASCII.
	 * NOTE: Although the spec says we should send only 7-bit data, we
	 * actually send 8-bit data.  This aids programs that use the
	 * parity bit, but that don't know to negotiate binary mode. 
 	 * However we do make sure that CRLF's have the parity bit off,
 	 * since otherwise we don't have a legal end of line.
	 */
#ifdef notdef
	ch &= 0177;
#endif
	/*
	 * If the last character sent was a <CR>, and this is a client
	 * connection, send a <LF>. Note that the last character was <CR>
	 * flag is only set on client connections if we failed to send
	 * the <LF> at the time the <CR> was sent. If this is a server
	 * connection, and the character we are about to send is not
	 * a linefeed, we will send a <NUL> first to indicate that the
	 * previous <CR> was intended to return to the current line.
	 * If the character is a <LF>, we will pass it through and clear
	 * the last was <CR> flag.
	 */
	if (conn->proto.tn.tn_state & ST_LASTOUTCR) {
	    if (serverflag) {
		if ((ch & conn->tty->tty_specialmask) != LINEFEED) {
		    if (send_byte_block(conn, '\0') == FALSE) {
			ungetc(conn->tty, ch);
			return(FALSE);
		    }
		}
	    } else {
		if (send_byte_block(conn, LINEFEED) == FALSE) {
		    ungetc(conn->tty, ch);
		    return(FALSE);
		}
	    }
            conn->proto.tn.tn_state &= ~ST_LASTOUTCR;
	}

	/*
	 * Handle the BREAK "character".  Send IAC BREAK.
         * If ip-on-break option set , send IAC IP
	 */
	if (ch >= BREAK_CHAR) {
	    if (ch == BREAK_CHAR) {
                if (send_byte_block(conn, TN_IAC) == FALSE) {
                    ungetc(conn->tty, ch);
                    return(FALSE);
                }
                if ((conn->tty->capabilities2 & IPONBREAK_2) == 0) {
                    ch = TN_BREAK;
                    strcpy(sentbuf,"BREAK");
                } else {
                    ch = TN_IP;
                    strcpy(sentbuf,"IP");
                    conn->proto.tn.tn_state |= ST_SENDTM;
                }
                if (telnet_debug && (conn == conn->tty->conntop))
                    buginf("\nTCP%t: %s sent on receiving BREAK char",
                       conn->tty->ttynum,sentbuf);
                if (conn->debug) {
                    printf("\n[TN: %s sent]",sentbuf);
                    flush();
                }
	    } else {
		int chtmp = telnet_chars[ch & 0x1F];
		if (chtmp) {
		    if (send_byte_block(conn, TN_IAC) == FALSE) {
	                ungetc(conn->tty, ch);
	                return(FALSE);
		    }
		    if (conn->debug) {
			printf("\n[TN: Sent %s]",telnet_names[ch&0x1F]);
			flush();
		    }
		    ch = chtmp;
		    if (ch == TN_AO || ch == TN_IP)
			conn->proto.tn.tn_state |= ST_SENDTM;
		} else if ((ch & 0x1F) == ('?' & 0x1F)) {
		    printf("\n[Special telnet escape help]");
		    for (chtmp = 0; chtmp <= 0x1F; chtmp++)
			if (telnet_chars[chtmp])
			    printf("\n  %c%c  sends telnet %s",
				   conn->tty->tty_escapechar, chtmp+'@',
				   telnet_names[chtmp]);
		    printf("\n");
		    flush();
		    return(TRUE);
		} else
		    return(TRUE);	/* discard illegal commands */
	    }
	}

	/*
	 * Handle IAC doubling.  Note that we always block for the first IAC.
	 */
	if (ch == TN_IAC) {
	    if (send_byte_block(conn, ch) == FALSE)
		return(FALSE); 
	}

	/*
	 * If we are sending a <CR>, and we are the client side of this
	 * connection, try to send a <LF> as well. This is done because
	 * the definition of a line terminator character is <CR><LF> and
	 * we want to be sure that a server will take action on our input
	 * lines immedietly. Note that if insufficient space exists for
	 * the <LF>, we will mark to send one before the next character.
	 * For a server connection, we wait until the next character to
	 * see if we need to insert a <LF> or <NUL>. This allows us to
	 * send the beginning of current line sequence through a telnet
	 * connection.
	 */
	if (!conn->proto.tn.in3270mode &&
	    ((ch & conn->tty->tty_specialmask) == RETURN)) {
 	    if (send_byte_block(conn, RETURN) == FALSE) {
 	        ungetc(conn->tty,ch);
 	        return(FALSE);
 	    }
	    if (!serverflag) {
		if (send_byte_block(conn, LINEFEED) == FALSE) {
		    conn->proto.tn.tn_state |= ST_LASTOUTCR;
		    return(FALSE);
		}
	    } else {
		conn->proto.tn.tn_state |= ST_LASTOUTCR;
	    }
	    return(TRUE);
	}

	/*
	 * Not sending a CR or anything else very funky.
	 * Fall through to end of function to send normal data.
	 */
    } else {

	/*
 	 * We are in binary mode.  Don't do fancy CRLF hacking.
	 */

	/*
	 * Handle the BREAK "character".  Send IAC BREAK.
         * If ip-on-break option set , send IAC IP
	 */
	if (ch >= BREAK_CHAR) {
	    if (ch == BREAK_CHAR) {
                if (send_byte_block(conn, TN_IAC) == FALSE) {
                    ungetc(conn->tty, ch);
                    return(FALSE);
                }
                if ((conn->tty->capabilities2 & IPONBREAK_2) == 0) {
                    ch = TN_BREAK;
                    strcpy(sentbuf,"BREAK");
                } else {
                    ch = TN_IP;
                    strcpy(sentbuf,"IP");
                    conn->proto.tn.tn_state |= ST_SENDTM;
                }
                if (telnet_debug && (conn == conn->tty->conntop))
                    buginf("\nTCP%t: %s sent on receiving BREAK char",
                       conn->tty->ttynum,sentbuf);
                if (conn->debug) {
                    printf("\n[TN: %s sent]",sentbuf);
                    flush();
                }
	    } else {
		int chtmp = telnet_chars[ch & 0x1F];
		if (chtmp) {
		    if (send_byte_block(conn, TN_IAC) == FALSE) {
	                ungetc(conn->tty, ch);
	                return(FALSE);
		    }
		    if (conn->debug) {
			printf("\n[TN: Sent %s]",telnet_names[ch&0x1F]);
			flush();
		    }
		    ch = chtmp;
		    if (ch == TN_AO || ch == TN_IP)
			conn->proto.tn.tn_state |= ST_SENDTM;
		} else
		    return(TRUE);	/* discard illegal commands */
	    }
	}

	/*
	 * Handle IAC doubling.  Note that we always block for the first IAC.
	 */
	if (ch == TN_IAC) {
	    if (send_byte_block(conn, ch) == FALSE)
		return(FALSE); 
	}

	/*
	 * If it's a RETURN and we're FAKING telnet binary, (ie TRANSPARENT)
	 * turn CR into CR, NUL.  We always block for the CR.
	 * Note that we try to send 0215 as itself.
	 */
	if (ch == RETURN && (conn->proto.tn.tn_option & STO_BINARY) == 0) {
	    if (send_byte_block(conn, ch) == FALSE) 
		return(FALSE); 
	    ch = '\0';
	}
    }

    /*
     * Here in both binary and non-binary cases to send a byte to
     * the network.  All other fanciness has already been taken care of.
     */
    if (send_byte_block(conn, ch) == FALSE)  {
	ungetc(conn->tty,ch);
	return(FALSE);
    }

    /*
     * Certain telnet commands (IP and AO, for example) can start to
     * flush local output as soon as they are typed.  We follow them
     * with a Timing mark, so that we can tell when the remote host
     * has responded, at which point we can start output again...
     */
    if (conn->proto.tn.tn_state & ST_SENDTM) {
	clearoutput(conn->tty);
	conn->proto.tn.tn_state &= ~ST_SENDTM;
	conn->proto.tn.tn_state |= ST_FLUSH;
	option_emit(conn, TN_DO, TNO_TIMEMARK);
    }
    return(TRUE);
}

/*
 * telnet_putc and telnet_puts 
 * called from telnet_input 
 */

void telnet_putc (conntype *conn, uchar ch)
{
    if (conn->proto.tn.in3270mode) {
        reg_invoke_tn3270_network_data(&ch, 1, conn->proto.tn.tn3270eorin,
				       conn);
        conn->proto.tn.tn3270eorin = 0;
    } else
        putc(conn->tty, ch);  /* put rest of string */
}

int telnet_puts (conntype *conn, uchar *dataptr, int datacount)
{
    int status;

    if (conn->proto.tn.in3270mode) {
        reg_invoke_tn3270_network_data(dataptr, datacount,
				       conn->proto.tn.tn3270eorin, conn);
        conn->proto.tn.tn3270eorin = 0;
	return(datacount);	/* We ate the whole thing */
    }
    status = putpkt(conn->tty, ((tcbtype *)conn->tcb)->curinputpak,
		    dataptr, datacount);
    return(status);		/* Return count actually output */
}

/*
 * telnet_input
 * Apply Telnet Protocol processing on a TCP input buffer.
 * Write results to the connection's terminal device.
 * No special CRLF hacking at this point.
 */

int telnet_input (conntype *conn, uchar *dataptr, int datacount)
{
    uchar ch;
    uchar *dp;		/* pointer to last char we looked at */
    int result;
    int count = 0;		/* Total count absorbed */
    int status;
    
    dp = dataptr;
    while (datacount--) {
	ch = *dp++;
	if (conn->proto.tn.tn_state & STATE_MASK) {
	    result = telnet_inputfsm(conn, ch);
	    if (result != -1)
	      telnet_putc(conn, result);
	    count += dp - dataptr;	/* Account for bytes output */
	    dataptr = dp;		/* reset start */
	    continue;
	}
	if (ch == TN_IAC) {
	    result = (dp - dataptr) - 1;
	    if (result > 0) {
		status = telnet_puts(conn, dataptr, result);
					/* put string so far */
		count += status; /* Account for bytes output */
		if (status != result)
		  return(count); /* Partial output, get out now */
	    }
	    conn->proto.tn.tn_state |= ST_CMD;
	    count++;		/* Account for IAC */
	    dataptr = dp;
	    continue;
	}
	if (conn->proto.tn.tn_option & STH_BINARY)
	  continue;		/* binary doesn't have to watch EOL */

	if (conn->proto.tn.tn_state & ST_LASTINCR) {
	    conn->proto.tn.tn_state &= ~ST_LASTINCR;
	    if (ch == 0) {	/* CR any is ok, but we must discard null */
		count += dp - dataptr; /* Account for the null */
		dataptr = dp;
	 	continue;
	    }	
	}
	if (ch == RETURN) {
	    /*
	     * Except for CR-Null, which requires that we eat the null,
	     * the data in the packet is what we want anyway, so we don't
	     * have to set a special state of LASTINCR, or do output at
	     * this time, and can avoid excessive process wakeups.
	     */
	    if (datacount && *dp != 0)
		continue;
	    result = dp - dataptr; /* Get the count */
	    status = telnet_puts(conn, dataptr, result); /* Put string */
	    count += status;	/* Account for bytes output */
	    if (status != result)
	      return(count);	/* Get out on partial output */
	    dataptr = dp;	/* Set new start */
	    conn->proto.tn.tn_state |= ST_LASTINCR;
	}	    
    }
    result = dp - dataptr;
    if (result > 0) {
	status = telnet_puts(conn, dataptr, result);  /* put rest of string */
	count += status;	/* Account for the bytes */
    }
    return(count);		/* Return count actually output */
}

/*
 * telnet_inputfsm
 * Finite state processor for Telnet input.
 * Returns -1 if absorbed character, else a putc'able character.
 */

int telnet_inputfsm (conntype *conn, uchar ch)
{
    switch (conn->proto.tn.tn_state & (ST_CMD + ST_SB + ST_SE + ST_OPT)) {
         case 0:		/* probably because of ST_DM or ST_FLUSH */
 	    if (ch == TN_IAC)
 	        conn->proto.tn.tn_state |= ST_CMD;
 	    if ((conn->proto.tn.tn_option & STH_BINARY) == 0) {
 		if (conn->proto.tn.tn_state & ST_LASTINCR)
 		    conn->proto.tn.tn_state &= ~ST_LASTINCR;
		if (ch == RETURN)
 		    conn->proto.tn.tn_state |= ST_LASTINCR;
 	    }
            break;
	case ST_DM:
	   if (ch == TN_DM)
		conn->proto.tn.tn_state &= ~ST_DM;
	   break;
	case ST_CMD:
	    return(tn_cmdstate(conn,ch));
	    break;
	case ST_SB:
	    if (ch == TN_IAC) {
		conn->proto.tn.tn_state &= ~ST_SB;
		conn->proto.tn.tn_state |= ST_SE;
	    } else
		if (conn->proto.tn.subpointer < &conn->proto.tn.subbuffer[SUBBUFLEN]) {
		    *conn->proto.tn.subpointer++ = ch;
		    conn->proto.tn.sub_length++;
		}
	    break;
	case ST_SE:
	    if (ch != TN_SE) {
		if (conn->proto.tn.subpointer < &conn->proto.tn.subbuffer[SUBBUFLEN]) {
		    *conn->proto.tn.subpointer++ = ch;
		    conn->proto.tn.sub_length++;
		}
		conn->proto.tn.tn_state &= ~ST_SE;
		conn->proto.tn.tn_state |= ST_SB;
	    } else {
		conn->proto.tn.tn_state &= ~ST_SE;
		tn_suboption(conn, conn->proto.tn.sub_length);
		conn->proto.tn.sub_length = 0;
	    }
	    break;
	case ST_OPT:
	    tn_optionstate(conn,ch);
	    break;
	default:
	    errmsg(&msgsym(BADSTATE, TN), conn->proto.tn.tn_state);
	    break;
    }
    return(-1);
}

/*
 * tnif
 * telnet protocol input filter.
 * Given a character, perform appropriate filtering actions.
 * Returns character or, if input was a command component, -1
 */

int tnif (conntype *conn, uchar ch)
{
    if (conn->proto.tn.tn_state & STATE_MASK)
	return(telnet_inputfsm(conn,ch));
    if (ch == TN_IAC) {
	conn->proto.tn.tn_state |= ST_CMD;
	return(-1);
    }
    if ((conn->proto.tn.tn_option & (STH_BINARY|STO_BINARY)) == 0) {
	if (conn->proto.tn.tn_state & ST_LASTINCR) {
	    conn->proto.tn.tn_state &= ~ST_LASTINCR;
	    if ((ch == LINEFEED) || (ch == 0))
		return(-1);
	}
	if (ch == RETURN)
	    conn->proto.tn.tn_state |= ST_LASTINCR;
    }
    return((int)ch);
}

/*
 * tn_cmdstate
 * Come here for the next character after an IAC.
 * Returns a character or -1 if character was absorbed.
 */

int tn_cmdstate (conntype *conn, uchar ch)
{
    tcbtype *tcb;

    conn->proto.tn.tn_state &= ~ST_CMD;	       /* no longer in command state */
    switch (ch) {
        case TN_DM:
	    /*
	     * What if we have two adjacent urgent data segments that have
	     * gotten collapsed into one?  We could end up clear ST_DM at
	     * the end of the first one.  I'm not very confident I can do
	     * better at the moment.
	     */
	    conn->proto.tn.tn_state &= ~ST_DM;
	    break;
	case TN_IAC:
	    if (conn->proto.tn.tn_state & ST_IACHACK) {
		if (telnet_debug)
		    ttybug(conn->tty, "IAC un-quoted");
		conn->proto.tn.tn_state |= ST_CMD; /* Back to command mode */
		break;
	    }
	    return(TN_IAC);		/* we were IAC stuffing */
	    break;
	case TN_SB:
	    conn->proto.tn.subpointer = conn->proto.tn.subbuffer;
	    conn->proto.tn.tn_state |= ST_SB;
	    break;
	case TN_WILL:			/* option commands */
	case TN_WONT:
	case TN_DO:
	case TN_DONT:
	    conn->proto.tn.lastcmd = (int) ch;
	    conn->proto.tn.tn_state |= ST_OPT;
	    break;
	case TN_IP:			/* Interrupt Process */
	    if ((conn->tty->capabilities & BREAKONIP) == 0)
		goto badcmd;
	    /* Else Fall through into BREAK code */
	case TN_BREAK:			/* the BREAK command */
	    if (conn->debug) {
		if (ch == TN_IP)
		  printf("\n[TN: IP received, converted to BREAK]");
		else printf("\n[TN: BREAK received]");
		flush();
	    }
	    if (telnet_debug && (conn == conn->tty->conntop))
		if (ch == TN_IP)
		    buginf("\nTCP%t: IP received, converted to BREAK",
			conn->tty->ttynum);
		else
		    buginf("\nTCP%t: Telnet BREAK received",conn->tty->ttynum);
	    if (conn->tty->capabilities & BREAKSYNC) {
	        tcb = conn->tcb;
	        tcp_putbyte(tcb,TN_IAC,TRUE);
		tcp_sendurg(tcb);
		tcp_putbyte(tcb,TN_DM,TRUE);
		tcp_putpush(tcb, TRUE, FALSE);
	    }
	    return(BREAK_CHAR);
	case TN_AO:
	    clearoutput(conn->tty);
	    break;
	case TN_NOP:			/* the do nothing command */
	case TN_GA:			/* the Go Ahead command */
	    break;
        case TN_EOR:
            if (conn->debug) {
                printf("\n[TELNET: EOR received]\n");
		flush();
	    }
            if (conn->proto.tn.in3270mode) {
                reg_invoke_tn3270_end_of_packet(conn);
                conn->proto.tn.tn3270eorin = 1;
            }
            break;
	default:
badcmd:
	    if (conn->debug) {
		printf("\n[TN: command %u ignored]\n",ch);
		flush();
	    }
	   if (telnet_debug && (conn == conn->tty->conntop))
		buginf("\nTCP%t: Telnet command %u received",
			conn->tty->ttynum, ch);
	   break;
    }
    return(-1);				/* we ate the character */
}

/*
 * proto.tn.tn_optionstate
 * Come here after a WILL/WONT/DO/DONT option command
 */

void tn_optionstate (conntype *conn, uchar ch)
{
    char opts[80];

    if (conn->debug) {
	printf("%s[TN: received %s]\n",
		(conn->tty->statbits & LASTWASCR) ? "" : "\n",
		option_string(conn->proto.tn.lastcmd,ch,opts));
	flush();
    }
    if (telnet_debug && (conn == conn->tty->conntop))
	buginf("\nTCP%t: Telnet received %s", conn->tty->ttynum,
		option_string(conn->proto.tn.lastcmd,ch,opts));

    /* We only want to check "kerberos clients mandatory" if we are
     * the telnet client.  This is an ugly way of determining that we
     * are the client.  Is there a cleaner check?
     */
    if (conn && (conn->proto.tn.statechange_check == telnetuser_optioncheck)) {
	if (reg_invoke_kerberos_use_kerb_clients() && 
	    conn->tty && conn->tty->ustruct && !conn->tty->ustruct->cred) {

	    ((tcbtype *) conn->tcb)->state = ABORTED;
	    if (telnet_debug || kerberos_debug) {
		printf("Kerberos:\tKerberized telnet failed!\n");
	    }
	    return;
	}
    }

    conn->proto.tn.tn_state &= ~ST_OPT;		/* no longer in option state */
    switch (ch) {
	case TNO_BINARY:
	    receive_option(conn,TNO_BINARY,STO_BINARY,STH_BINARY);
            if (conn->proto.tn.tn3270cmd) {
                if (conn->proto.tn.tn_option & STO_BINARY)
                    conn->proto.tn.tn3270cmd |= WEWILLBINARYSTATE;
                else
                    conn->proto.tn.tn3270cmd &= ~WEWILLBINARYSTATE;
                if (conn->proto.tn.tn_option & STH_BINARY)
                    conn->proto.tn.tn3270cmd |= HEWILLBINARYSTATE;
                conn->proto.tn.in3270mode = 
                    (conn->proto.tn.tn3270cmd == TN3270STATE);
		if (conn->proto.tn.in3270mode) 
		    conn->proto.tn.tn_option &= ~STO_EDIT;
            }
	    break;
	/*
	 * If a user process, refuse to echo.  It would be bogus to do so
	 * in any case, but 4.3 telnetd's use this to detect 4.2 telnet's,
	 * and we don't want to be considered 4.2, because then they won't
	 * use SYNC's.
	 */
	case TNO_ECHO:
	    receive_option(conn, ch, STO_ECHO, STH_ECHO);
	    break;
	case TNO_SUPRGA:
	    receive_option(conn, ch, STO_SUPRGA, STH_SUPRGA);
	    break;

	case TNO_TTYSPEED:
	    receive_option(conn, ch, STO_TTYSPEED, STH_TTYSPEED);
	    if (SERVERTELNET(conn) && conn->proto.tn.lastcmd == TN_WILL &&
		conn->tty->tty_passspeed)
		tn_sendsub(conn, TNO_TTYSPEED, TNQ_SEND, "");
	    break;
	case TNO_TTYTYPE:
	    receive_option(conn, ch, STO_TTYTYPE, STH_TTYTYPE);
            if (conn->proto.tn.lastcmd == TN_WILL)
                tn_sendsub(conn, TNO_TTYTYPE, TNQ_SEND,"");
	    break;
	case TNO_TTYLOC:
	    receive_option(conn, ch, STO_TTYLOC, STH_TTYLOC);
	    break;
	case TNO_LFLOW:
	    receive_option(conn, ch, STO_LFLOW, STH_LFLOW);
	    /*
	     * If we are a server, and have agreed to negotiate about flow
	     * control, we may now need to turn it off.  This will show up
	     * because the tty databits will still say flow control is off
	     */
	    if ((conn->tty->capabilities & SFLOWIN) == 0 &&
		(conn->proto.tn.lastcmd == TN_WILL) &&
		(conn->proto.tn.tn_option & STH_LFLOW))
		tn_sendsub(conn, TNO_LFLOW, 0, "");
	    break;
	case TNO_XDISPLAY:
	    receive_option(conn, ch, STO_XDISPLAY, STH_XDISPLAY);
	    break;
	case TNO_TIMEMARK:
	    conn->proto.tn.tn_state &= ~ST_FLUSH;
	    if (conn->proto.tn.lastcmd == TN_DO) {
		startoutput(conn->tty);
		edisms((blockproc *)sputBLOCK, (ulong)conn->tty); /* Wait to finish */
		option_emit(conn, TN_WILL, TNO_TIMEMARK);
	    } else if (conn->proto.tn.lastcmd == TN_DONT)
		option_emit(conn, TN_WONT, TNO_TIMEMARK);
	    tcp_putpush(conn->tcb, TRUE, FALSE);
	    break;		
	case TNO_NAWS:
	    receive_option(conn, ch, STO_NAWS, STH_NAWS);
	    break;
        case TNO_SENDEOR:
            if (conn->proto.tn.tn3270cmd)
                receive_option(conn, ch, STO_SENDEOR, STH_SENDEOR);
            else
                receive_option(conn,ch,ST_TN_UNIMPLEMENTED,ST_TN_UNIMPLEMENTED);
            break;
	case TNO_AUTH:
	    receive_option(conn, ch, STO_AUTH, STH_AUTH);
	    if (reg_invoke_kerberos_is_running()
		&& reg_invoke_kerberos_stackcheck()) {
		if (conn->proto.tn.lastcmd == TN_WILL) {
		    char sendstring[4] = {KERBEROS_V5, AUTH_HOW_MUTUAL,
					  KERBEROS_V5, AUTH_HOW_ONE_WAY};

		    tn_sendsubcnt(conn, TNO_AUTH, TNQ_SEND, sendstring, 4);
		}
		if (conn->proto.tn.lastcmd == TN_WONT) {
		    conn->proto.tn.do_kerb_auth = FALSE;
		}
	    } else {
		if (reg_invoke_kerberos_is_running()) {
		    printf("Kerberos was enabled after this 'exec' process was "
			"forked.\nLog off and log back in to utilize Kerberos."
			"\n");
		} else {
		    printf("Kerberos:\tNo default realm defined for Kerberos!\n");
		}
	    }
	    break;
	default:
	    receive_option(conn, ch,ST_TN_UNIMPLEMENTED,ST_TN_UNIMPLEMENTED);
	    break;
    }
}

/*
 * tn_suboption
 * Have collected complete sub option. Perform it here.
 */

void tn_suboption (conntype *conn, int sub_length)
{
    if ((conn->debug || telnet_debug) 
	&& conn->proto.tn.subbuffer[0] == TNO_NAWS) {
	unsigned char *cp;
	int length, width;

	cp = &conn->proto.tn.subbuffer[1];
	width = 256 * (*cp++);
	width += *cp++;
	length = 256 * (*cp++);
	length += *cp;

	if (conn->debug)
	    printf("\n[TN: recv SB NAWS %d %d]", 
		   width, length);
	if (telnet_debug)
	    buginf("\nTelnet%t: recv SB NAWS %d %d", conn->tty->ttynum,
		   width,
		   length);
    } else {
	if (conn->debug)
	    printf("\n[TN: recv SB %d %d %s]", conn->proto.tn.subbuffer[0],
		   conn->proto.tn.subbuffer[1], &conn->proto.tn.subbuffer[2]);
	if (telnet_debug)
	    buginf("\nTelnet%t: recv SB %d %d %s", conn->tty->ttynum,
		   conn->proto.tn.subbuffer[0], conn->proto.tn.subbuffer[1],
		   &conn->proto.tn.subbuffer[2]);
    }
    switch (conn->proto.tn.subbuffer[0] & 0xff) {
	case TNO_NAWS: 
	    /*
	     * process RFC1073 window [terminal] size option
	     */
	    {
		tt_soc *tty;
		unsigned char *cp;

		tty = conn->tty;
		cp = &conn->proto.tn.subbuffer[1];

		tty_savedefaults(tty);

		tty->tty_width = 256 * (*cp++);
		tty->tty_width += *cp++;
		tty->tty_length = 256 * (*cp++);
		tty->tty_length += *cp;

		/* sanity checking as the parser does it */
		if (tty->tty_width > 512) {
		    tty->tty_width = 512;
		}
		if (tty->tty_length > 512) {
		    tty->tty_length = 512;
		}
	    }
	    break;
	case TNO_LFLOW:
	    if (conn->proto.tn.subbuffer[1]&0xff) {
		conn->status |= CONN_FLOW;
		conn_setsflow(stdio, TRUE);
	    } else {
	        conn->status &= ~CONN_FLOW;
		conn_setsflow(stdio, FALSE);
	    }
	    break;
	case TNO_TTYTYPE:
            if (conn->proto.tn.tn3270cmd) {
                 tn_sendsub(conn,TNO_TTYTYPE,TNQ_IS,
                            reg_invoke_tn3270_terminal_type(conn->tty));
                 conn->proto.tn.tn3270cmd |= TERMINALTYPESENTSTATE;
                 conn->proto.tn.in3270mode =
                     (conn->proto.tn.tn3270cmd == TN3270STATE);
		 if (conn->proto.tn.in3270mode) 
		     conn->proto.tn.tn_option &= ~STO_EDIT;
	    } else if ((conn->proto.tn.subbuffer[1]&0xff) == TNQ_IS) {
		setstring(&stdio->termtype, &conn->proto.tn.subbuffer[2]);
	    } else if (((conn->proto.tn.subbuffer[1]&0xff) == TNQ_SEND) &&
                       (stdio->termtype))
		tn_sendsub(conn,TNO_TTYTYPE,TNQ_IS,
			    stdio->termtype);
	    break;
	case TNO_TTYSPEED:
	    if ((conn->proto.tn.subbuffer[1]&0xff) == TNQ_IS) {
	        int rxspeed = 0;
		int txspeed = 0;
		register char *cp = &conn->proto.tn.subbuffer[2];
		register tt_soc *tty = conn->tty;

		if (!tty->tty_passspeed)
		    break;

		/*
		 * Note that we reverse TX and RX, since our receive
		 * must be his send.  However split speed is unlikely
		 * to be useful.  Presumably we're doing this in
		 * the first place because we're milking a machine
		 * that does autobaud.  It's unlikely that autobaud
		 * can handle split speed.
		 */
		while (*cp >= '0' && *cp <= '9') {
		    rxspeed = (rxspeed * 10) + (*cp - '0');
		    cp++;
		}
		if (*cp != ',')
		    break;
		cp++;
 		while (*cp >= '0' && *cp <= '9') {
		    txspeed = (txspeed * 10) + (*cp - '0');
		    cp++;
		}
 		if (rxspeed && txspeed) {
		    if (rxspeed > tty->tty_maxpassspeed)
			rxspeed = tty->tty_maxpassspeed;
		    if (txspeed > tty->tty_maxpassspeed)
			txspeed = tty->tty_maxpassspeed;
		  setspeed(tty, txspeed, rxspeed);
		}
	    } else if ((conn->proto.tn.subbuffer[1]&0xff) == TNQ_SEND &&
		       stdio->tty_txspeed && stdio->tty_rxspeed) {
	        char speedbuf[20];
		sprintf(speedbuf, "%d,%d", stdio->tty_txspeed,
			stdio->tty_rxspeed);
		tn_sendsub(conn, TNO_TTYSPEED, TNQ_IS, speedbuf);
	    }
	    break;
	case TNO_XDISPLAY:
	    if ((conn->proto.tn.subbuffer[1] & 0xff) == TNQ_SEND) {
		char dispbuf[22];
		sprintf(dispbuf,"%s:%d",
			address_string(&((tcbtype *)conn->tcb)->localhost),
			stdio->ttynum + 2000);
		tn_sendsub(conn, TNO_XDISPLAY, TNQ_IS, dispbuf);
	    }
	    break;
	case TNO_AUTH:
	    if ((conn->proto.tn.tn_option & STO_AUTH) || 
		 (conn->proto.tn.tn_option & STH_AUTH)) {
		/* RFC1416 Autheintication is supported */
		int i = 2;
		boolean result = FALSE;

		if (conn->proto.tn.subbuffer[1] == TNQ_NAME) {
		    char tty_str[8];
		    boolean ret = FALSE;

		    /* 
		     * We are the server.
		     * We deal with the name packet the same way for all
		     *  authentication types.
		     */

		    if (conn->tty) {
			sprintf(&tty_str[0], "tty%d", conn->tty->ttynum);
			ret = aaa_create_user(&conn->tty->ustruct, NULL, NULL, 
				tty_str, tty_get_remote_addr(conn->tty),
				AAA_AUTHEN_TYPE_ASCII, AAA_SVC_LOGIN,
				conn->tty->csb ? conn->tty->csb->priv :
					      CUR_PRIV(conn->tty->privilege));
			if (!ret) {
			    if (telnet_debug || kerberos_debug)
				printf("Kerberos:\tFailed to create userstruct!");
			}
			conn->tty->ustruct->user = malloc(sub_length-1);
			if (conn->tty->ustruct->user == NULL) {
			    if (telnet_debug || kerberos_debug)
				printf("Kerberos:\tFailed to store username!");
			}
			for (i=0; i<sub_length-2; i++)
			    conn->tty->ustruct->user[i] = 
				conn->proto.tn.subbuffer[i+2];
			conn->tty->ustruct->user[i] = '\0';
		    } else {
			if (telnet_debug || kerberos_debug)
			    printf("Kerberos:\tFailed to store username!");
		    }
		    break;
		}

		if (conn->proto.tn.subbuffer[2] == '\0') {
		    /*
		     * Telnet client and server can't agree on an 
		     * authentication method
		     */
		    send_option(conn, STH_AUTH, STH_AUTH, TN_DONT, TNO_AUTH);
		    conn->proto.tn.do_kerb_auth = FALSE;
		}

		if (conn->proto.tn.subbuffer[1] == TNQ_SEND) {
		    while ((i<sub_length) && !result) {
			reg_invoke_telnet_auth(
			    (int) conn->proto.tn.subbuffer[i], &result, conn, 
			    sub_length, (int) conn->proto.tn.subbuffer[i+1]);
			i+=2;
		    }
		    if (!result && conn->tty && conn->tty->ustruct) {
			aaa_free_user(&conn->tty->ustruct);
		    }
		} else {
		    reg_invoke_telnet_auth(conn->proto.tn.subbuffer[2],
			&result, conn, sub_length, conn->proto.tn.subbuffer[3]);
		    if (!result && conn->tty && conn->tty->ustruct) {
			aaa_free_user(&conn->tty->ustruct);
		    }
		}
	    } else {
		option_emit(conn, TN_DONT, TNO_AUTH);
	    }
	    break;
	default:
	    if (conn->debug)
		printf("[Unrecognized]");
	    if (telnet_debug)
		buginf("[Unrecognized]");
	    break;
    }
}

/*
 * tn_sendsub
 * Send a Telnet suboption string
 */

void tn_sendsub (conntype *conn, int code, int request, char *cp)
{
    tcbtype *tcb = conn->tcb;
    if (telnet_debug)
	buginf("\nTelnet%t: Sent SB %d %d %s", tcb->ttynum, code, request, cp);
    if (conn->debug)
	printf("\n[TN: Sent SB %d %d %s]", code, request, cp);
    (void) tcp_putbyte(tcb, TN_IAC, TRUE);
    (void) tcp_putbyte(tcb, TN_SB, TRUE);
    (void) tcp_putbyte(tcb, code, TRUE);
    (void) tcp_putbyte(tcb, request, TRUE);
    for (; *cp; cp++)
	(void) tcp_putbyte(tcb, (uchar)*cp, TRUE);
    (void) tcp_putbyte(tcb, TN_IAC, TRUE);
    (void) tcp_putbyte(tcb, TN_SE, TRUE);
    tcp_putpush(tcb, TRUE, FALSE);
}
/*
 * tn_sendsubcnt
 * Send a Telnet suboption string 
 */

void tn_sendsubcnt (conntype *conn, int code, int request, char *cp, int len)
{
    int cnt =0;
    int length=0, width=0;		/* initializers to make gcc happy */

    tcbtype *tcb = conn->tcb;

    if (code == TNO_NAWS) {
	width = 256 * (unsigned)request;
	width += (unsigned)cp[0];
	length = 256 * (unsigned)cp[1];
	length += (unsigned)cp[2];
    }

    if (telnet_debug) {
	if (code == TNO_NAWS) {
	    buginf("\nTelnet%t: Sent SB NAWS %d %d", 
		   tcb->ttynum, width, length);
	} else {
	    buginf("\nTelnet%t: Sent SB %d %d %s", 
		   tcb->ttynum, code, request, cp);
	}
    }
    if (conn->debug) {
	if (code == TNO_NAWS) {
	    printf("\n[TN: Sent SB NAWS %d %d]", width, length);
	} else {
	    printf("\n[TN: Sent SB %d %d %s]", code, request, cp);
	}
    }
    (void) tcp_putbyte(tcb, TN_IAC, TRUE);
    (void) tcp_putbyte(tcb, TN_SB, TRUE);
    (void) tcp_putbyte(tcb, code, TRUE);
    (void) tcp_putbyte(tcb, request, TRUE);
    for (cnt = 0 ; cnt <len; cnt++) {
	(void) tcp_putbyte(tcb, (uchar)cp[cnt], TRUE);
    }
    (void) tcp_putbyte(tcb, TN_IAC, TRUE);
    (void) tcp_putbyte(tcb, TN_SE, TRUE);
    tcp_putpush(tcb, TRUE, FALSE);
}

/*
 * telnet_suboption_pad_iac
 * Pad TN_IAC bytes in telnet suboption buffer
 * Returns pointer to new memory and size of data in new memory
 */
void telnet_suboption_pad_iac(char *buf, int len, char **newbuf, int *newlen)
{
    int cnt, xtra = 0, padding = 10;
    char *tmpbuf, *tmpbuf2;
     
    tmpbuf = malloc(len+padding);
    if (tmpbuf == NULL)
	return;

    for (cnt=0;cnt<len;cnt++) {
	tmpbuf[cnt+xtra] = buf[cnt];
	if (((uchar)buf[cnt]) == ((uchar) TN_IAC)) {
	    xtra++;
	    if (xtra >= padding) {
		padding+=10;
		tmpbuf2 = realloc(tmpbuf, len+padding);
		if (tmpbuf2 == NULL) {
		    free(tmpbuf);
		    *newbuf = NULL;
		    *newlen = 0;
		    return;
		}
		tmpbuf = tmpbuf2;
	    }
	    tmpbuf[cnt+xtra] = buf[cnt];
	}
    }
    *newbuf = tmpbuf;
    *newlen = len+xtra;
}

/*
 * option_emit
 * Send a two byte option negotiation
 */

void option_emit (conntype *conn, register int code1, register int code2)
{
    register tcbtype *tcb;
    char opts[80];

    tcb = conn->tcb;
    (void) tcp_putbyte(tcb,TN_IAC,TRUE);
    (void) tcp_putbyte(tcb,code1,TRUE);
    (void) tcp_putbyte(tcb,code2,TRUE);
    if (conn->debug) {
	printf("\n[TN: sent %s]\n", option_string(code1, code2, opts));
	flush();
    }
    if (telnet_debug && (conn == conn->tty->conntop))
	buginf("\nTCP%t: Telnet sent %s", conn->tty->ttynum,
		 option_string(code1, code2, opts));
}

/*
 * telnet_defaults
 * Send the default telnet options, set local state flags.
 */
 
void telnet_defaults (conntype *conn, boolean binaryflag)
{
    if ((conn->tty->capabilities & REFUSEECHO) == 0)
	send_option(conn, STO_ECHO, STO_ECHO, TN_WILL, TNO_ECHO);
    if ((conn->tty->capabilities & REFUSESGA) == 0)
	send_option(conn, STO_SUPRGA, STO_SUPRGA, TN_WILL, TNO_SUPRGA);
    if (conn->tty->tty_passspeed)
	send_option(conn, STH_TTYSPEED, STH_TTYSPEED, TN_DO,TNO_TTYSPEED);
    if (binaryflag) {
	send_option(conn, STO_BINARY, STO_BINARY, TN_WILL, TNO_BINARY);
	send_option(conn, STH_BINARY, STH_BINARY, TN_DO, TNO_BINARY);
	/*
	 * Go into binary mode immediately.  This will result in us speaking
	 * binary mode (which is probably good) if the remote system does not
	 * actually support the telnet protocol and never answers our repsonse.
	 * If the remote specifically rejects binary mode, we'll still turn it
	 * off.  This does not appear to affect the negotiation loop avoidance
	 * code, but watch for problems...
	 */
	conn->proto.tn.tn_option |=  STH_BINARY|STO_BINARY;
    } else
	send_option(conn, STH_TTYTYPE, STH_TTYTYPE, TN_DO, TNO_TTYTYPE);
    if (reg_invoke_kerberos_is_running()) {
	conn->proto.tn.do_kerb_auth = TRUE;
	send_option(conn, STH_AUTH, STH_AUTH, TN_DO, TNO_AUTH);
    } else {
	conn->proto.tn.do_kerb_auth = FALSE;
    }
    send_option(conn, STH_NAWS,  STH_NAWS, TN_DO, TNO_NAWS);
    tcp_putpush(conn->tcb,TRUE,FALSE);
}

/*
 * option_string
 * Return a string describing a telnet negotiation
 */

/* Telnet options as human-readable strings, indexed by option code */
static const char *const telnet_options[] = {
    "BINARY",			/* 0 */
    "ECHO",			/* 1 */
    "RECONNECTION",		/* 2 */
    "SUPPRESS-GA",		/* 3 */
    "APPROX-MSG-SIZE",		/* 4 */
    "STATUS",			/* 5 */
    "TIMING-MARK",		/* 6 */
    "REMOTE-TRANS-ECHO",	/* 7 */
    "LINE-WIDTH",		/* 8 */
    "PAGE-SIZE",		/* 9 */
    "CR-DISPOSITION",		/* 10 */
    "TAB-STOPS",		/* 11 */
    "TAB-DISPOSITION",		/* 12 */
    "FF-DISPOSITION",		/* 13 */
    "VTAB-STOPS",		/* 14 */
    "VTAB-DISPOSITION",		/* 15 */
    "LF-DISPOSITION",		/* 16 */
    "EXTENDED-ASCII",		/* 17 */
    "LOGOUT",			/* 18 */
    "BYTE-MACRO",		/* 19 */
    "DATA-ENTRY-TERM",		/* 20 */
    "SUPDUP",			/* 21 */
    "SUPDUP-OUTPUT",		/* 22 */
    "TTY-LOCATION",		/* 23 */
    "TTY-TYPE",			/* 24 */
    "SEND-EOR",			/* 25 */
    "TACACS-USER-ID",		/* 26 */
    "OUTPUT-MARKING",		/* 27 */
    "TERM-LOCATION-NUMBER",	/* 28 */
    "3270-REGIME",		/* 29 */
    "X3-PAD",			/* 30 */
    "WINDOW-SIZE",		/* 31 */
    "TTY-SPEED",		/* 32 */
    "LOCAL-FLOW",		/* 33 -- XX should be "REMOTE-FLOW" */
    "LINEMODE",			/* 34 */
    "X-DISPLAY",		/* 35 */
    "ENVIRONMENT",		/* 36 */
    "AUTHENTICATION",		/* 37 */
    "ENCRYPTION"		/* 38 */
};

char *option_string (register int option, register uint code, char *string)
{
    char *optstring;
    const char *codestring;

    switch (option) {
	case TN_WILL: optstring = "WILL"; break;
	case TN_WONT: optstring = "WONT"; break;
	case TN_DO: optstring = "DO"; break;
	case TN_DONT: optstring = "DONT"; break;
   	default: optstring = NULL; break;
    }

    if (code < sizeof(telnet_options) / sizeof(telnet_options[0]))
	codestring = telnet_options[code];
    else
	codestring = NULL;

    if (optstring == NULL) {
	if (codestring == NULL) sprintf(string, "%d %d", option, code);
	else sprintf(string, "%d %s (%d)", option, codestring, code);
    } else {
	if (codestring == NULL) sprintf(string, "%s %d", optstring, code);
	else sprintf(string, "%s %s (%d)", optstring, codestring, code);
    }
    return(string);
}

/*
 * magic_putc
 * write a character to a (tcp) connection, return FALSE if we need to BLOCK.
 */

static boolean magic_putc (conntype *conn, int c)
{
    tcbtype *tcb;

    /*
     * For a 3270 session, treat this like user input
     */
    if (conn->proto.tn.in3270mode) {
	reg_invoke_tn3270_user_keypress(c, conn);
	return(TRUE);
    }
    /*
     * For stream or RLOGIN sessions, just send the byte down the link
     */
    tcb = conn->tcb;
    if ((conn->status & CONN_STREAM) || 
	tcb->foreignport == RLOGIN_PORT)
      return(tcp_putbyte(tcb, c, FALSE));

    /*
     * call tnof with serverflag == TRUE, so that end-of-lines
     * will be handled properly (cr-lf gets converted to crlf).
     */
    return(tnof(conn, c, FALSE, TRUE));
}

/*
 * magic_fingers
 * Support the login-string command
 * send characters from login string directly to network connection,
 * bypassing TTY entirely.  If we want to ignore user typing, return TRUE.
 */

boolean magic_fingers (conntype *conn)
{
    char c;
    char *cp;
    int seconds;
   	
    cp = conn->loginstring;
    /*
     * DRH failsafe code
     */
    if (!validlink(conn->loginstring)) {
	errmsg(&msgsym(BADLOGIN, TN), cp);
	conn->status &= ~CONN_KBDLOCK;
	TIMER_STOP(conn->logintimer);
        conn->loginstring = NULL; /* Don't keep trying */
	return(FALSE);
    }

    if (cp == NULL) {
	conn->status &= ~CONN_KBDLOCK;
	TIMER_STOP(conn->logintimer);
	return(FALSE);
    }
    if (TIMER_RUNNING_AND_SLEEPING(conn->logintimer))
	return(conn->status & CONN_KBDLOCK ? TRUE : FALSE);

    conn->status &= ~CONN_KBDLOCK;
    TIMER_STOP(conn->logintimer);
    seconds = 0;
    while ((c = *cp++)) {
	if (c == RETURN && ((tcbtype *)conn->tcb)->foreignport == RLOGIN_PORT)
	    if (*cp == LINEFEED)
		cp++;		/* On rlogin connections, send only CR */
	if (c != '%') { 
	    if (!magic_putc(conn, c)) {
		cp--;				/* backup pointer.. */
		conn->tty->isavedchar = -1;	/* dont save in tty */
		conn->loginstring = cp;
		TIMER_START(conn->logintimer, ONESEC/5);
		return(conn->status & CONN_KBDLOCK ? TRUE : FALSE);
	    }
	}
	else {
 	    seconds = 0;
	    while ((*cp >= '0') && (*cp <= '9'))
		seconds = seconds*10 + (*cp++ - '0');
	    switch (*cp++) {
		case '%':
		    if (!magic_putc(conn, '%')) {
			cp--;				/* backup pointer.. */
			conn->tty->isavedchar = -1;	/* dont save in tty */
			TIMER_START(conn->logintimer, ONESEC/5);
			return(conn->status & CONN_KBDLOCK ? TRUE : FALSE);
		    }
		    break;
		case 'm':	/* Put in a raw carriage return */
		case 'M':	/* (tn3270 thang) */
		    if (!magic_putc(conn, RETURN)) {
			cp--;				/* backup pointer.. */
			conn->tty->isavedchar = -1;	/* dont save in tty */
			TIMER_START(conn->logintimer, ONESEC/5);
			return(conn->status & CONN_KBDLOCK ? TRUE : FALSE);
		    }
		    break;
		case 't':	/* Another way to put in tab since it */
		case 'T':	/* is completion char for parser. */
		    if (!magic_putc(conn, CH_HT)) {
			cp--;				/* backup pointer.. */
			conn->tty->isavedchar = -1;	/* dont save in tty */
			TIMER_START(conn->logintimer, ONESEC/5);
			return(conn->status & CONN_KBDLOCK ? TRUE : FALSE);
		    }
		    break;
		case 'p':
		case 'P':
		    if (seconds == 0)
			seconds = 1;
		    conn->status &= ~CONN_KBDLOCK;
		    conn->loginstring = cp;
		    TIMER_START(conn->logintimer, seconds*ONESEC);
		    return(FALSE);

		case 'w':
		case 'W':
		    if (seconds == 0)
			seconds = 1;
		    conn->status |= CONN_KBDLOCK;
		    conn->loginstring = cp;
		    TIMER_START(conn->logintimer, seconds*ONESEC);
		    return(TRUE);

		case 'b':
		case 'B':
		    if (!magic_putc(conn, BREAK_CHAR)) {
			cp--;				/* backup pointer.. */
			conn->tty->isavedchar = -1;	/* dont save in tty */
			TIMER_START(conn->logintimer, ONESEC/5);
			return(conn->status & CONN_KBDLOCK ? TRUE : FALSE);
		    }
		    break;
		    
		default:
		    break;
	    }
	}
    }
    conn->loginstring = NULL;
    return(FALSE);
}

/*
 * pak_rembyte
 * Remove a byte from the current packet.  This is used by local editing.
 * Return the character that was removed.
 */
int pak_rembyte (paktype *pak)
{
    if ((pak == NULL) || (pak->length >= pak->unspecified[0]))
	return(-1); 
    pak->length++;	
    return(*--pak->dataptr);
}

/*
 * telnet_getline(conn)
 * Read a line (with possible local editing) into the packet.
 * Return when we would block, or when the packet is ready to send.
 * Unfortunately, this routine knows more than it should about the
 * format of tcbs and packets.
 */

int telnet_getline (conntype *conn)
{
    tt_soc *tty = conn->tty;
    int c, stripped;
    boolean oflag = FALSE;		/* we generated some output */

    while ((c = getc(tty)) != -1) {
	stripped = c & 0177;
	/*
	 * first check for local editing characters.
	 */
	if (stripped == CH_BS || stripped == CH_DEL) { 	/* character delete */
	    c = pak_rembyte(((tcbtype *)conn->tcb)->curoutputpak);
	    if (((conn->proto.tn.tn_option & STH_ECHO) == 0) && (c != -1)) {
		erase_char(c);
		oflag++;
	    }	
	    continue;
	}
	if (stripped == CH_NAK) { /* line delete */
	    oflag++;
	    while ((c = pak_rembyte(((tcbtype *)conn->tcb)->
					       curoutputpak)) != -1) {
		if ((conn->proto.tn.tn_option & STH_ECHO) == 0) {
		    erase_char(c);
		    oflag++;
		}	
		if ((c & 0177) == RETURN) break;
	    }
	    continue;
	}
	if (stripped == CH_DC2) { /* line display */
	    paktype *pak = ((tcbtype *)conn->tcb)->curoutputpak;
	    if (pak == NULL)
		continue;
	    *pak->dataptr = '\0';
	    ttyprintf(tty,"\n%s", pak->dataptr -
		      (pak->unspecified[0] - pak->length));
	    oflag++;
	    continue;
	}
	/*
	 * Insert the character into the buffer.
	 */
	if (tnof(conn,c,FALSE,FALSE) == FALSE)
	    break;

	/*
	 * Possible local echoing.
	 */
	if ((conn->proto.tn.tn_option & STH_ECHO) == 0 && c < BREAK_CHAR) {
	    if (((conn->proto.tn.tn_option & STO_BINARY) == 0) && 
	        ((tty->capabilities & TRANSPARENT) == 0))
		printc(c);
	    else
	        putc(tty,c);
	    oflag++;
 	}

  	/*
  	 * Now check for dispatchness, etc.
  	 */
	if (stripped == RETURN || c >= BREAK_CHAR ||
		(tty->tty_editinfo.dispatch[stripped] & TTY_DISP_DISP)) {
  	    if (oflag)
		startoutput(tty);
  	    return(READLINE_DONE);
	}
    } /* while */
    if (oflag) startoutput(tty);
    return(READLINE_BLOCK);
}

/*
 * send_option
 * Send a telnet option negotiation, keep tack of enough data to prevent
 * negotiation loops.  Return FALSE if we can't send such an option
 * (because we already did), TRUE otherwise.  This is based on Dave
 * Borman's algorithm, execpt we only allow one outstanding negotiation
 * per option.
 */

boolean send_option (conntype *conn, ulong optionbit, ulong wantstate,
		     uchar code1, uchar code2)
{
    if (telnet_debug)
	buginf("\nTelnet%t: %x %x %d %d", conn->tty->ttynum, optionbit,
	       wantstate,code1,code2);
    if (((conn->proto.tn.tn_wantoption & optionbit) == wantstate) ||
	 (((conn->proto.tn.tn_option & optionbit) == wantstate) &&
	    (conn->proto.tn.tn_sentoption & optionbit) == 0)) {
	if (telnet_debug) buginf(" (current state)");
	return(TRUE);			/* already going to correct state */
    }

    if (conn->proto.tn.tn_sentoption & optionbit) {/* already in progress ? */
	if (telnet_debug) buginf(" (in progress)");
	return(FALSE);
    }

    conn->proto.tn.tn_wantoption &= ~optionbit;	/* clear bit */
    conn->proto.tn.tn_wantoption |= wantstate;	/* set or unset bit */
    conn->proto.tn.tn_sentoption |= optionbit;	/* say we sent negotiation */
    option_emit(conn, code1, code2);		/* send it */
    return(TRUE);
}

/*
 * receive_option
 * Here when we receive option negotiatiosn from a remote host.
 * This avoids loops use Dave Borman's algorithm.
 * We have to get passed two bits, since we don't know which one
 * will be used until we look at the connection state.
 */

void receive_option (conntype *conn, ulong code, ulong ourbit, ulong hisbit)
{
    ulong bit, bitstate, okcode, refusecode;
    boolean allowed, reply;

    switch (conn->proto.tn.lastcmd) {
	case TN_DO:
	    bit = ourbit;		/* our option */
	    bitstate = ourbit;	    	/* Turn it on */
	    okcode = TN_WILL;
	    refusecode = TN_WONT;
	    break;
	case TN_WILL:
	    bit = hisbit;		/* his option */
	    bitstate = hisbit;    	/* Turn it on */
	    okcode = TN_DO;
	    refusecode = TN_DONT;
	    break;
	case TN_DONT:
	    bit = ourbit;		/* our option */
	    bitstate = 0;	    	/* Turn it off */
	    okcode = TN_WONT;
	    refusecode = TN_WILL;
	    break;
	case TN_WONT:
	    bit = hisbit;		/* hist option */
	    bitstate = 0;	    	/* Turn it off */
	    okcode = TN_DONT;
	    refusecode = TN_DO;
	    break;
	default:
	    if (conn->debug) {
		printf("\n[TN: impossible option negotiation]\n");
		flush();
	    }
	    if (telnet_debug && (conn == conn->tty->conntop))
		buginf("\nTCP%t: impossible option negotiation",
			conn->tty->ttynum);
	    return;
    }    

    if (bit == ST_TN_UNIMPLEMENTED)
	if (refusecode == TN_DONT || refusecode == TN_WONT) {
	    option_emit(conn, refusecode, code);
	    if (conn->debug) {
		printf(" (unimplemented)");
		flush();
	    }
	    if (telnet_debug && (conn == conn->tty->conntop))
		buginf(" (unimplemented)");
	    /* make sure data gets sent */
	    tcp_putpush(conn->tcb,TRUE,FALSE);
	    return;
	}

    /*
     * Clear the bit saying we have outstanding negotiation.  Normally,
     * this would be a decrement, but not since we can only have one
     * outstanding negotiation.  The sentoption bit is still useful
     * since it prevents us from negotiating an option at the same
     * time the other side is changing it...
     */
    if (conn->proto.tn.tn_sentoption & bit) {
	if (conn->proto.tn.statechange_done) /* other side responds. React */
	    (*(conn->proto.tn.statechange_done))(conn, bit, bitstate, code);
	conn->proto.tn.tn_sentoption &= ~bit;		/* Clear bit */
	reply = TRUE;		/* note that this was a reply */
    } else
	reply = FALSE;		/* action initiated by other side. */

    if (bitstate != (conn->proto.tn.tn_wantoption & bit)) {/* What we want? */
	if (conn->proto.tn.statechange_check)
	    allowed = (*(conn->proto.tn.statechange_check)) (conn, code);
	else allowed = TRUE;			/* assume OK if no check */
	if (allowed) {				/* Ok to change ? */
	    conn->proto.tn.tn_wantoption &= ~bit;	/* clear bit */
	    conn->proto.tn.tn_wantoption |= bitstate;	/* set or unset bit */
	    option_emit(conn, okcode, code);	/* send it */
	    conn->proto.tn.tn_option &= ~bit;	/* Set our current state */
	    conn->proto.tn.tn_option |= bitstate;
	    if (!reply) {
		/*
		 * If the other side is initiating actions that we agree to,
		 * we may still need to do stuff after we send the OK.
		 */
		if (conn->proto.tn.statechange_done)
		    (*(conn->proto.tn.statechange_done))(conn, bit,
							 bitstate, code);
	    }

	} else {				/* Don't want to change ! */
	    if (telnet_debug && (conn == conn->tty->conntop))
		buginf(" (refused)");
	    conn->proto.tn.tn_sentoption |= bit;
	    option_emit(conn, refusecode, code);
	}
    } else {
	conn->proto.tn.tn_option &= ~bit;        /* Set our current state */
	conn->proto.tn.tn_option |= bitstate;
    }
    tcp_putpush(conn->tcb,TRUE,FALSE); /* make sure data gets sent*/
}

/*
 * telnetuser_optioncheck
 * Check validity an option state change request sent to the telnet client
 */

boolean telnetuser_optioncheck (conntype *conn, uchar code)
{
    switch (code) {
	/*
	 * If a user process, refuse to echo.  It would be bogus to do so
	 *  in any case, but 4.3 telnetd's use this to detect 4.2 telnet's,
	 *  and we don't want to be considered 4.2, because then they won't
	 *  use SYNC's.
	 * Also always do local echoing if USER requested Line mode.
	 */
	case TNO_ECHO:
	    if (conn->proto.tn.lastcmd == TN_DO)
		return(FALSE);
	    if ((conn->proto.tn.tn_option & STO_LINE) &&
		(conn->proto.tn.lastcmd == TN_WILL))
		return(FALSE);
	    return(TRUE);

	/*
	 * The system currently uses SUPRGA to indicate "local lin/edit mode".
	 * A host that refuses to Supress GAs, probably wants line at a time
	 * input, even if it never actually sends any GAs.  This is compatible
	 * with a number of hosts, such as Crays...
	 */
	case TNO_SUPRGA:
	    if ((conn->proto.tn.tn_option & STO_LINE) &&
		(conn->proto.tn.lastcmd == TN_WILL || 
		 conn->proto.tn.lastcmd == TN_DO))
		return(FALSE);		/* Stay in user requested line mode */

	    if ((conn->proto.tn.lastcmd == TN_WONT)) {
		if (((conn->proto.tn.tn_option & STO_EDIT) == 0) &&
		    (!conn->proto.tn.in3270mode)) {
		    conn->proto.tn.tn_option |= STO_EDIT;
		    if (conn->debug) {
			printf("[TN: (assuming local edit mode)]\n");
			flush();
		    }
		}
	    } else
		if ((conn->proto.tn.lastcmd != TN_DONT) &&
		    (conn->proto.tn.tn_option & STO_EDIT)) {
		    conn->proto.tn.tn_option &= ~STO_EDIT;
		    if (conn->debug) {
			printf("[TN: (local edit off)]\n");
			flush();
		    }
		}		
	    return(TRUE);

	case TNO_TTYSPEED:
	    if (stdio->tty_txspeed && stdio->tty_rxspeed)
		return(TRUE);
	    break;
	case TNO_TTYTYPE:
	    if (stdio->termtype || conn->proto.tn.tn3270cmd)
		return(TRUE);
	    break;
	case TNO_TTYLOC:
	    if (get_tty_location(stdio) || stdio->conntop)
		return(TRUE);
	    break;
	case TNO_LFLOW:
	    if (conn->proto.tn.lastcmd == TN_DO) {
	        conn->status |= CONN_FLOW;
		conn_setsflow(stdio, TRUE);
		return(TRUE);
	    }
	    if (conn->proto.tn.lastcmd == TN_DONT)
		return(TRUE);
	    break;	
	case TNO_BINARY:
	    return(TRUE);
	case TNO_TIMEMARK:
	    conn->proto.tn.tn_state &= ~ST_FLUSH;
	    if (conn->proto.tn.lastcmd == TN_WONT ||
		conn->proto.tn.lastcmd == TN_DONT)
		return(TRUE);
	    break;
        case TNO_SENDEOR:
            if (conn->proto.tn.tn3270cmd)
                return(TRUE);
            break;
	case TNO_XDISPLAY:
	    return(stdio->xrcb != NULL);
	case TNO_AUTH:
	    if (reg_invoke_kerberos_is_running()
		&& reg_invoke_kerberos_stackcheck() && conn && conn->tty
		&& conn->tty->ustruct && conn->tty->ustruct->cred
		&& conn->tty->ustruct->cred->cred
		&& conn->tty->ustruct->cred->cred_type == KERBEROS) {

		return(TRUE);
	    }
	    if (reg_invoke_kerberos_use_kerb_clients()) {
		((tcbtype *) conn->tcb)->state = ABORTED;
		if (telnet_debug || kerberos_debug) {
		    printf("Kerberos:\tKerberized telnet failed!\n");
		}
	    }
	    break;
	case TNO_NAWS:
	    return TRUE;
	default:
	    break;
    }
    return(FALSE);
}

/*
 * telnetuser_optiondone
 * perform side effects at option state change confirmation.  this is
 * needed for option changes we request at startup time.
 */

void telnetuser_optiondone (conntype *conn, ulong bit, ulong bitstate,
			    ulong code)
{
    char *p;

    switch (code) {
	case TNO_LFLOW:
	    if (conn->proto.tn.lastcmd == TN_DO) {
	        conn->status |= CONN_FLOW;
		conn_setsflow(stdio, TRUE);
		return;
	    }
	    if (conn->proto.tn.lastcmd == TN_DONT)
		return;
	    break;	
	case TNO_TTYLOC:
	    p = NULL;
	    if (conn->proto.tn.lastcmd == TN_DO) {
		p = get_tty_location(stdio);
		if (!p && stdio->conntop) {
		    p = conn->tty->conntop->hostname;
		}
	    }
	    if (p) {
		tn_sendsub(conn, TNO_TTYLOC, p[0], &p[1]);
	    }
	    break;
	case TNO_NAWS:
	    if (conn->proto.tn.lastcmd != TN_DO) {
		break;
	    }
	    if (conn->tty->tty_width || conn->tty->tty_length) {
		unsigned char ttysize[3], first;
		unsigned tmp_size;

		/*
		 * prepare to pass user's terminal size
		 * out to remote telnet server
		 *
		 * "first" holds the first byte of the size 
		 * because the NAWS suboption has no function
		 * code.
		 */
		tmp_size = conn->tty->tty_width;
		first = tmp_size / 256;
		ttysize[0] = tmp_size % 256;
		tmp_size = conn->tty->tty_length;
		ttysize[1] = tmp_size / 256;
		ttysize[2] = tmp_size % 256;

		tn_sendsubcnt(conn, TNO_NAWS, first, ttysize, sizeof ttysize);
	    }
	    break;
	default:
	    break;
    }
}

/*
 * telnetserver_optioncheck
 * Check validity an option state change request sent to the telnet client.
 * This applies to server exec and ttydaemons only, not to special things
 * like the protocol translator.
 */

boolean telnetserver_optioncheck (conntype *conn, uchar code)
{
    switch (code) {

	case TNO_NAWS:
	    return TRUE;
	case TNO_LFLOW:
	    if (nttylines > 0 || protocolconversion)
		if (conn->tty->type == VTY_LINE)
		    return(TRUE);
	    break;
	case TNO_BINARY:
	    return(TRUE);

	/*
	 * Line used in milking machine applications can be set up to
	 * refuse the Supress GA option, which will cause a good telnet
	 * to stay in line mode.
	 */
	case TNO_SUPRGA:
	    if ((conn->tty->capabilities & REFUSESGA) &&
		conn->proto.tn.lastcmd == TN_DO)
		return(FALSE);
	    return(TRUE);

	case TNO_ECHO:
	    if ((conn->tty->capabilities & REFUSEECHO) &&
		conn->proto.tn.lastcmd == TN_DO)
		return(FALSE);
	    return(TRUE);

	case TNO_TTYSPEED:
	    if (conn->tty->tty_passspeed)
		return(TRUE);
	    break;

	case TNO_TIMEMARK:
	    if (conn->proto.tn.lastcmd == TN_WILL ||
		conn->proto.tn.lastcmd == TN_WONT)
		conn->proto.tn.tn_state &= ~ST_FLUSH;
	    break;		

	case TNO_TTYTYPE:
            return(TRUE);
	case TNO_AUTH:
	    return(reg_invoke_kerberos_is_running()
		   && reg_invoke_kerberos_stackcheck());

    }
    return(FALSE);
}

#if 0
/*
 * linemode_getline
 * Make this general enough to call froma bunch of different places.
 *
 * Parameters:
 * editinfo editing data to use
 * pak     packet to update
 * mygetc  how to get characters
 * arg     argument for mygetc
 * why     more detail on why we terminated 
 */
int linemode_getline (tty_editdata *editinfo, paktype *pak,
		      int (*mygetc)(), ulong arg, int *why)
{
    int c;
    boolean oflag = FALSE;		/* we generated some output */

    while ((c = mygetc(arg)) != -1) {
	/*
	 * first check for local editing characters.
	 */
	if (editinfo->editflag &&
		(editinfo->dispatch[c & 0x7F] & TTY_DISP_EDIT)) {

	  if (c == editinfo->special[NVT_EC] ||
		c == editinfo->special[NVT_EC2]) {  	/* character delete */
		c = pak_rembyte(pak));
		if (stdio) {
		    erase_char(c);
		    oflag++;
		}
	    }
	    continue;
	  }
	  if (c == editinfo->special[NVT_EL]) {	/* line delete */
	    if (tty) {
		ttyprintf(tty,"XXX\n");
		oflag++;
	    }
	    while ((c = pak_rembyte(pak)) != -1)
		if (c == RETURN) break;
	    continue;
	  }
	  if (c == params[X3_LINEDISP]) {		/* line display */
	    *pak->dataptr = NULL;	    
	    if (tty) {
		ttyprintf(tty, "\n%s",pak->dataptr -
			  (pak->unspecified[0] - pak->length));
		oflag++;
	    }
	    continue;
	  }
	}
	if (c == BREAK_CHAR)
	    return(X3_BREAKLINE);
	/*
	 * Insert the character into the buffer.
	 */
	*pak->dataptr++ = c;
	if (--pak->length == 0)
	    return(X3_FULL);
	/*
	 * possible local echoing.
	 */
	if (params[X3_LOCALECHO] && tty) {
	    oflag++;
	    putc(tty,c);	/* do local echo if necessary */
	    if ((c == RETURN) && (params[X3_LFINSERT] & 4))
		putc(tty,LINEFEED);
	}
	/*
	 * now check for dispatchness, etc.
	 */
	if (c == RETURN) {
	    if (params[X3_LFINSERT] & 2) {
		*pak->dataptr++ = LINEFEED;
		if (--pak->length == 0) {
		    if (oflag) startoutput(tty);
		    return(X3_FULL);
		}
	    }
	    if (params[X3_DISPMASK] & 2)
		goto x3_done;
	}
	if (params[X3_DISPMASK]) {
	    if ((c == CH_DEL) && (params[X3_DISPMASK] & 8))
		goto x3_done;

	    if (c >= ' ') {
		if (params[X3_DISPMASK] & 1)
		    goto x3_done;
	    } else switch (c) {
		case CH_ESC:
		case BELL:
		case CH_ENQ:
		case CH_ACK:
		    if (params[X3_DISPMASK] & 4)
			goto x3_done;
		    break;
		case CH_CAN:
		case CH_DC2:
		    if (params[X3_DISPMASK] & 8)
			goto x3_done;
		    break;
		case CH_ETX:
		case CH_EOT:
		    if (params[X3_DISPMASK] & 16)
			goto x3_done;
		    break;
		case CH_HT:
		case LINEFEED:
		case CH_VT:
		case CH_FF:
		    if (params[X3_DISPMASK] & 32)
			goto x3_done;
		    break;
		default:
		    if (params[X3_DISPMASK] & 64)
			goto x3_done;
		    break;
	    } /* switch */
	} /* dispmask exists */
    } /* while */
    if (oflag) startoutput(tty);
    return(X3_BLOCK);
x3_done:
    if (oflag) startoutput(tty);
    return(X3_DONE);
}
#endif

/*
 * telnet_connect_command
 * Connect to a foreign tcp host:
 * e.g. <host> [<port>] [/debug] [/line] [/route] [/stream]
 * Where <host> may be a hostname or an IP address.
 */

#define PORTCHRS 20
#define MAXCHRS  50

void telnet_connect_command (parseinfo *csb)
{
    register conntype *newcon;
    int dummy;
    addrtype address;
    short tcpport;
    boolean stream, debug, line, tn3270cmd;
    boolean noecho;
    int *errptr;
    ipoptiontype *ipopts;
    ipoptiontype opts, one_option;
    nametype *ptr;
    addrtype source_address, *saddr;
    uint flags;

    /*
     * See if this connect is allowed
     */

    if (!connect_allowed(&tcp_connvector)) {
	return;
    }

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

    /*
     * Attempt to look up the address in dotted format
     */
    if (numeric_lookup(ADDR_IP, GETOBJ(string, 1), &address)) {
	/* Did it look like an IP address? */
	if (address.ip_addr && martian(address.ip_addr, TRUE)) {
	    /* Yes, is it valid? */
	    printf("%% Invalid IP address");
	    return;
	}
	ptr = NULL;
    } else {

	/*
	 * Attempt to look up the address in the name cache
	 */

	psion(stdio);		/* Enable interrupts */
	ptr = name_lookup(GETOBJ(string, 1), &dummy, NAM_IP);
	if (!ptr) {
	    if (!psipending()) {
		printf(connect_unknown);
	    }
	    psioff(stdio);
	    return;
	}
	psioff(stdio);
    }

    /*
     * At this point, we have either an IP address in ipaddr or a name
     * block pointer in ptr.
     */

    tcpport = GETOBJ(int, 1); /* Pick up user specified */

    memset(&opts, 0, sizeof(ipoptiontype));
    ipopts = &opts;

    if (ip_secopt_bytes) {
	ip_add_bso_opt(ipopts);
	ip_add_eso_opt(ipopts, address.ip_addr);
    }

    debug = GETOBJ(int,3);
    line = GETOBJ(int,4);
    stream = GETOBJ(int,5);
    noecho = GETOBJ(int,7);
    tn3270cmd = (GETOBJ(int,9) == CMD_CONNECT_TN3270);

    if (GETOBJ(int,6)) {
	memset(&one_option, 0, sizeof(ipoptiontype));
	one_option.ipopts[0] = IPOPT_LSR;
	if (! parse_ipoptroute(&one_option, csb))
	  return;

	if (!add_ipoption(&opts, &one_option)) {
	    printf(noroom);
	    return;
	}
    }
    if (!ipopts->paddedlength){
	ipopts = NULL;
    }

    /*
     * Take the source address from the interface specified on the
     * command line, or from the configured telnet source interface.
     */
    saddr = telnet_source_address(GETOBJ(idb,1), &source_address);

    /*
     * Now we try to connect to the remote host.
     */

    if (ptr) {
	if (!tcpport) {
	    tcpport = ptr->v.ip.port ? ptr->v.ip.port : TELNET_PORT;
	}
	errptr = ptr->busystring ? &dummy : NULL;
    } else {   
	errptr = NULL;
	if (!tcpport) {
	    tcpport = TELNET_PORT;
	}
    }

    /*
     * If port is RLOGIN, do rlogin instead (if on a comm-server).
     * For tn3270 or /stream, /line switches we continue with telnet
     */
    if ((RLOGIN_PORT == tcpport) && (!(line || stream || tn3270cmd)) &&
	reg_invoke_rlogin_supported()) {
	reg_invoke_rlogin_connect(csb);
	return;
    }
    /*
     * The name ptr should be locked against being freed and filled
     * with poison. CSCdi23760 caused a crash where busy-string looked
     * good. Note since none of the above code blocked, so it should
     * still be OK to do the locking here.
     */
    if (ptr)
	ptr->sharecount++;

    /*
     * Calculate flags for open_connection()
     */
    flags = 0;
    if (stream)
	flags |= OPENFL_STREAM;
    if (debug)
	flags |= OPENFL_DEBUG;
    if (line)
	flags |= OPENFL_LINE;
    if (tn3270cmd)
	flags |= OPENFL_TN3270;

    /*
     * Attempt connection
     */
    newcon = open_connection(ptr, &address, tcpport, saddr, errptr,
	ipopts, flags, NULL);
    if (!newcon) {
	if (ptr) {
	    if (ptr->busystring) {		/* if busy message wanted */
		printf("%s",ptr->busystring);	/* print it now */
	    } else {
	        name_valid(ptr,FALSE);		/* recheck validity later */
	    }
	    ptr->sharecount--;
	}
	return;
    }

    if (ptr) {
	name_valid(ptr,TRUE);	/* cache entry is definitely valid */
	newcon->loginstring = ptr->loginstring;/* remember any login string */
    }
    if (noecho){
	newcon->proto.tn.tn_option |= STH_ECHO;
    }

    connect_new_session(newcon, GETOBJ(string, 1), csb);
					 /* Start this new session */
    if (ptr)
	ptr->sharecount--;
}

/*
 * tcp_setsflow
 * try to turn on flow control at the far end of a TCP connection.
 */

void tcp_setsflow (tt_soc *tty)
{
    conntype *conn = tty->conntop;

    if (conn && conn->tcb && (conn->proto.tn.tn_option & STH_LFLOW))
	tn_sendsub(conn, TNO_LFLOW, tty->capabilities & SFLOWIN ? 1 : 0, "");
}

/*
 * tcp_clearoutput
 * Try to clear output at the far end of a TCP connection
 */

void tcp_clearoutput (tt_soc *tty)
{
    conntype *conn = tty->conntop;
    tcbtype *tcb = tty2tcb(tty,FALSE);

    if (!tcb || !conn || (conn->status & CONN_STREAM))
	return;

    tcp_putbyte(tcb, TN_IAC, TRUE);
    tcp_sendurg(tcb);
    tcp_putbyte(tcb, TN_AO, TRUE);
    tcp_putbyte(tcb,TN_IAC,TRUE);
    tcp_putbyte(tcb,TN_DM,TRUE);
    tcp_putpush(tcb, TRUE, FALSE);
}

/*
 * telnet_alive
 * Determine whether a telnet connection is alive or not
 */

boolean telnet_alive (conntype *conn)
{
    tcbtype *tcb;

    tcb = conn->tcb;
    if (!tcb)
	return(FALSE);

    /*
     * telnet_alive() is a vector function accessed via conn_alive() 
     * macro by many TCP applications. In the past, FALSE is returned
     * to indicate modem drop when a FIN is received from remote TCP
     * regardless whether remote is half or fully closed. However, TCP
     * for rsh client on remote host can only receive data while being 
     * half-closed if it's invoked with "-n" option. Returning FALSE 
     * in this case will prematurely terminate the data transfer. 
     *
     * It's a bit hacky just checking for rsh port here for support of
     * data transfer while remote host being half-closed, but altering 
     * the behavior of telnet_alive() by always returning TRUE in 
     * CLOSEWAIT state could potentially cause problem in other applications 
     * since we don't know exactly how telnet_alive() is used by each 
     * application.
     */
    if (!tcp_closing(tcb))
	return(TRUE);
    else if (tcb->localport == RSHELL_PORT && tcb->state == CLOSEWAIT)
	return(TRUE);
    else 
	return(FALSE);
}
 
/* 
 * telnet_unread
 * Return the count of unread input characters
 */

int telnet_unread (conntype *conn)
{
    tcbtype *tcb;

    tcb = conn->tcb;
    if (!tcb)
	return(0);
    return(tcp_unread(tcb));
}

/*
 * telnet_multiproto_open - Handle session open for connect command
 *
 * This routine is used to handle telnet connections when the user has
 * not explicitly specified the protocol.
 *
 * Note that we should not print out any parsing errors, because an unknown
 * name may simply be something handled by another protocol.
 */

conntype *telnet_multiproto_open (parseinfo *csb, boolean *abortflag)
{
    addrtype address;
    nametype *nameptr;
    ushort tcpport;
    int dummy;
    int *errptr = NULL;
    conntype *newcon;
    boolean stream, debug, line, tn3270cmd;
    ipoptiontype *ipopts;
    ipoptiontype opts, one_option;
    addrtype source_address, *saddr;
    uint flags;

    tcpport = GETOBJ(int, 1);

    /*
     * Attempt to look up the address in dotted format
     */

    if (numeric_lookup(ADDR_IP, GETOBJ(string, 1), &address)) {
	/* Did it look like an IP address? */
	if (address.ip_addr && martian(address.ip_addr, TRUE)) {
	    /* Yes, is it valid? */
	    return(NULL);	/* No, indicate failure */
	}
	nameptr = NULL;
    } else {

	/*
	 * Attempt to look up the address in the name cache
	 */

	psion(stdio);		/* Enable interrupts */
	nameptr = name_lookup(GETOBJ(string, 1), &dummy, NAM_IP);
	if (!nameptr) {
	    if (psipending()) {
		*abortflag = TRUE;
	    }
	    psioff(stdio);
	    return(NULL);
	} else
	    if (nameptr->busystring)
		errptr = &dummy;
	psioff(stdio);
	if (!tcpport) {
	    tcpport = nameptr->v.ip.port;
	}
    }

    /*
     * Get options from csb
     */
    debug = GETOBJ(int,3);
    line = GETOBJ(int,4);
    stream = GETOBJ(int,5);
    tn3270cmd = (GETOBJ(int,9) == CMD_CONNECT_TN3270);

    /*
     * Take the source address from the interface specified on the
     * command line, or from the configured telnet source interface.
     */
    saddr = telnet_source_address(GETOBJ(idb,1), &source_address);

    /*
     * If we don't have a TCP port by now, default to telnet
     * Else if the port is RLOGIN, do rlogin (if supported).
     * For tn3270 or /stream, /line switches we continue with telnet
     */

    if (!tcpport) {
	tcpport = TELNET_PORT;
    } else if ((RLOGIN_PORT == tcpport) &&
	       (!(line || stream || tn3270cmd)) &&
	       reg_invoke_rlogin_supported()) {

	reg_invoke_rlogin_connect(csb);
	*abortflag = TRUE;
	return(NULL);
    }

    memset(&opts, 0, sizeof(ipoptiontype));
    ipopts = &opts;
    if (ip_secopt_bytes) {
	ip_add_bso_opt(ipopts);
    }
    if (GETOBJ(int,6)) {
	memset(&one_option, 0, sizeof(ipoptiontype));
	one_option.ipopts[0] = IPOPT_LSR;
	if (! parse_ipoptroute(&one_option, csb)) {
	    return(NULL);
	}
	if (!add_ipoption(&opts, &one_option)) {
	    printf(noroom);
	    return(NULL);
	}
    }
    if (!ipopts->paddedlength){
	ipopts = NULL;
    }

    /*
     * The name ptr should be locked against being freed and filled
     * with poison. CSCdi23760 caused a crash where busy-string looked
     * good. Note since none of the above code blocked, so it should
     * still be OK to do the locking here.
     */
    if (nameptr)
	nameptr->sharecount++;

    /*
     * Calculate flags for open_connection()
     */
    flags = 0;
    if (stream)
	flags |= OPENFL_STREAM;
    if (debug)
	flags |= OPENFL_DEBUG;
    if (line)
	flags |= OPENFL_LINE;
    if (tn3270cmd)
	flags |= OPENFL_TN3270;

    /*
     * Attempt to open a connection
     */
    newcon = open_connection(nameptr, &address, tcpport, saddr,
	errptr, ipopts, flags, abortflag);

    /* if we succeed here..remember to set the login-string pointer */
    if (newcon && nameptr) {
	name_valid(nameptr,TRUE);      /* cache entry is definitely valid */
	newcon->loginstring = nameptr->loginstring; /* remember any login */
                                                    /* string */
    }
    if (!newcon && errptr)
	printf("%s", nameptr->busystring); /* errptr implies this
					      exists */
    if (nameptr)
	nameptr->sharecount--;
    return(newcon);		/* Return a connection if we got one */
}

/*
 * telnet_conn_acct
 * add accounting info to acctinfo in a protocol dependent manner
 * look at the guts of the tcb and connection.
 */
void telnet_conn_acct (conntype *conn, acctinfo *acct)
{
    int port = ((tcbtype *)conn->tcb)->foreignport;

    if (conn->proto.tn.tn3270cmd)
	reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_PROTO, AAA_VAL_PROT_tn3270,
			AAA_TYPE_STR, AAA_STRSIZE(AAA_VAL_PROT_tn3270));
    else
	reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_PROTO, AAA_VAL_PROT_telnet,
			AAA_TYPE_STR, AAA_STRSIZE(AAA_VAL_PROT_telnet));
    reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_ADDR,
			 &conn->address.addr.ip_address, AAA_TYPE_IP, 4);
    if (port != TELNET_PORT)
	reg_invoke_aaa_acct_attr_add(acct, AAA_ATTR_CMDARG, &port,
				     AAA_TYPE_INT, sizeof(int));
    acct->flags |= ACCT_F_FIRSTDONE;
}

/*
 * telnet_source_address
 *
 * Return the source address to use for Telnet connections.  If the
 * source_idb is not suitable, try the global telnet_source_idb.
 * Returns NULL if we want TCP to choose the "best" local address for
 * us.
 */

addrtype *telnet_source_address (idbtype *source_idb, addrtype *source_address)
{
    /*
     * Take the source address from the configured interface for this
     * translation, or from the configured telnet source interface.
     */
    if (source_idb && source_idb->ip_address && interface_up(source_idb)) {
	source_address->type = ADDR_IP;
	source_address->ip_addr = source_idb->ip_address;
	return(source_address);
    } else if (telnet_source_idb && telnet_source_idb->ip_address &&
	       interface_up(telnet_source_idb)) {
	source_address->type = ADDR_IP;
	source_address->ip_addr = telnet_source_idb->ip_address;
	return(source_address);
    } else {
	return(NULL);
    }
}

/*
 * tty_status_ok
 *
 * Check tty condition for the connection and return FALSE if this
 * tty has an inactivity timeout, user typed escape char, or the 
 * modem has dropped carrier.
 */
boolean tty_status_ok (conntype *conn, tcbtype * tcb, tt_soc *tty, 
		    long oldcaps, int *code)
{
    if (conn->status & CONN_CHECKALL) {
	conn->status &= ~CONN_CHECKALL;
    	/*
         * Check tty driver events:
         *  - Session timeout occured.
         *  - return if escape character typed
         *  - return if carrier off
         *  - do notifications if timer has expired.
         */
	if (tty->tty_sessiontimeout)
	    (void) conn_session(conn, FALSE);
	if (tty->statbits & IDLETIMEOUT) {
	    conn_restoreflow(tty, oldcaps);
	    if (modem_debug)
		buginf("\nTTY%t: Inactivity timeout", tty->ttynum);
	    *code = IDLE_TIMEDOUT; 
	    return(FALSE);
	}
	if (modemcheck(tty)) {
 	    conn_restoreflow(tty, oldcaps);
	    *code = CARRIER_DROP;
	    return(FALSE);
	}
	if (psiset(tty)) {
 	    conn_restoreflow(tty, oldcaps);
	    tcp_idle(tcb,TRUE);
	    *code = OK;
	    return(FALSE);
	}
	if (!SLEEPING(conn->notetime)) {
	    notification(conn);
	    TIMER_START(conn->notetime, 30*ONESEC);
	}
    }
    return (TRUE);
}
