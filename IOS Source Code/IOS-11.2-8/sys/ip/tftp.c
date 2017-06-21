/* $Id: tftp.c,v 3.10.4.5 1996/09/06 21:00:57 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/tftp.c,v $
 *------------------------------------------------------------------
 * tftp.c -- Trivial File Transfer Protocol, RFC 783
 *
 * 22-October-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tftp.c,v $
 * Revision 3.10.4.5  1996/09/06  21:00:57  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.10.4.4  1996/08/07  09:00:22  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.10.4.3  1996/07/06  05:54:08  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.10.4.2  1996/05/21  09:51:38  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.4.1  1996/04/15  21:16:54  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.10  1996/03/11  19:26:00  ahh
 * CSCdi43020:  netbooting from not-directly connected tftp-server fails
 * Allow user to force a boot interface when spurious redirects cause
 * our automagic booting to fail.
 *
 * Revision 3.9  1996/01/24  20:15:14  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.8  1996/01/19  00:57:51  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1996/01/11  02:26:53  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . removed ip_tuba.c and clns_tuba.c.
 * . removed lots of tuba related registry stubs and code.
 *
 * Revision 3.6  1995/12/28  18:27:42  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreign addresses
 *   to tcp_open() for future portability.
 *
 * Revision 3.5  1995/12/01  01:37:39  bcole
 * CSCdi43512:  TFTP: Encapsulation problems when using bootstrap image
 *
 * Generate path table entries instead of ARP table entries for all serial
 * encapsulation types which don't make use of ARP, instead of only for
 * P2P interfaces.
 *
 * Revision 3.4  1995/11/22  17:47:22  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.3  1995/11/21  07:41:22  jenny
 * CSCdi43195:  TFTP: cannot specify source address
 *
 * Revision 3.2  1995/11/17  09:36:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/10/20  18:06:02  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.6  1995/09/11  23:49:39  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.5  1995/08/03 07:42:10  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.4  1995/07/16 18:04:12  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.3  1995/07/15  02:36:57  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.2  1995/06/09  13:05:41  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:01:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "packet.h"
#include "ip.h"
#include "ttysrv.h"
#include "config.h"
#include "sys_registry.h"
#include "../ip/ip_registry.h"
#include "file.h"
#include "access.h"
#include "../os/boot.h"
#include "../dev/monitor1.h"
#include "tftp.h"
#include "logger.h"
#include "../ui/debug.h"
#include "../ip/tftp_debug.h"
#include "ip_media.h"
#include "../iprouting/route.h"
#include "../clns/clns_externs.h"
#include "../os/free.h"
#include "../if/network.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../dev/flash_registry.h"
#include "subsys.h"
#include "../parser/parser_defs_tcpip.h"
#include "../parser/parser_defs_parser.h"

/*
 * Local Storage
 */
boolean tftp_serving;			/* do we support TFTP service */
uint successBlocksInRow;
boolean ip_tftp_ascii_mode;		/* Should we claim netascii? */
idbtype *tftp_source_idb;		/* idb used for tftp source address */
idbtype *ip_tftp_boot_idb;		/* Which interface to boot through */

#define TFTP_BLOCKS_PER_BANG	10

/*
 * tftp_RRQ
 * Send a TFTP Read Request for the specified filename
 * RRQ Header format:
 *   <2 opcode bytes><filename string><null byte><mode string><null byte>
 */

void tftp_RRQ (ipsocktype *soc, const char *filename, const char *mode)
{
    int len;
    uint offset;
    paktype *pak;
    uchar *data;

    /*
     * Length is filename string, a NULL byte, 2 byte opcode,
     * the mode string, and another NULL byte.
     */
    len = strlen(filename) + 1 + 2 + strlen(mode) + 1;
    pak = setup_ipsocket(soc, len, &offset);
    if (!pak) {
	return;
    }
    data = pak->data_area + offset;
    *data++ = 0; *data++ = RRQ;		/* set opcode to RRQ */
    while ((*data++ = *filename++));	/* filename */
    while ((*data++ = *mode++));	/* transfer mode */
    if (tftp_debug) {
	buginf("\nTFTP: Sending  read request");
    }
    ip_write_sock_multi(soc, pak, len);
}

/*
 * tftp_ACK
 * Send a TFTP ACK.  Returns TRUE/FALSE.
 * ACK Header format:
 *    <2 byte opcode><2 byte blocknumber>
 */

#define ACK_LENGTH 4

void tftp_ACK (ipsocktype *soc, short blocknumber)
{
    paktype *pak;
    uint offset;
    ushort *data;
 
    pak = setup_ipsocket(soc,ACK_LENGTH,&offset);
    if (!pak)
	return;
    data = (unsigned short *)(pak->data_area + offset);
    *data++ = h2n(ACK);
    *data++ = h2n(blocknumber);
    if (tftp_debug)
	buginf("\nTFTP: Sending  ACK for block %d,   socket_id %#x",
	       blocknumber, soc); 
    write_ipsocket(soc,pak,ACK_LENGTH);
}

/*
 * tftp_ERROR
 * Send a TFTP ERROR.
 * ACK Header format:
 *    <2 byte opcode><2 byte error code><string><null byte>
 */

#define ERROR_LENGTH 5

void tftp_ERROR (ipsocktype *soc, short code, char *errmsg)
{
    paktype *pak;
    uint offset, len;
    ushort *data;

    len = ERROR_LENGTH + strlen(errmsg);
    pak = setup_ipsocket(soc, len, &offset);
    if (!pak)
	return;
    data = (ushort *)(pak->data_area + offset);
    *data++ = h2n(ERROR);		/* make this an error packet */
    *data++ = h2n(code);
    strcpy((char *)data, errmsg);
    if (tftp_debug)
	buginf("\nTFTP: Sending error %d %s", code, errmsg);
    write_ipsocket(soc, pak, len);
}

/*
 * tftp_get_laddr
 * Return the source address to use for a TFTP connection.  The ip address
 * for global tftp_source_idb is returned if user configured an alternative
 * source address.  Otherwise, return NULL to let UDP choose the "best" local
 * address.
 */
addrtype *tftp_get_laddr (addrtype *laddr)
{
    if (tftp_source_idb && tftp_source_idb->ip_address &&
	interface_up(tftp_source_idb)) {
	laddr->type = ADDR_IP;
	laddr->ip_addr = tftp_source_idb->ip_address;
	return(laddr);
    } else {
	return(NULL);
    }
}

/*
 * Open a socket to a TFTP server
 */

static ipsocktype *tftp_open_socket (addrtype *fhost, ushort sockflags)
{
    ipsocktype *soc;
    addrtype laddr, *lhost;

    lhost = tftp_get_laddr(&laddr);
    soc = open_ipsocket(UDP_PROT, lhost, fhost, 0, TFTP_PORT, sockflags);
    return(soc);
}

/*
 * tftp_sendERROR
 * Open a UDP socket to the given host and send a TFTP ERROR.  Be sure to
 * use exactly the numbers he used, in case he cares.
 */

void tftp_sendERROR (paktype *pak, uint lport, uint fport, int code, char
		     *errmsg)
{
    ipsocktype *soc;
    addrtype laddr, faddr;

    ip_extract_addr(pak, &faddr, SOURCE);
    ip_extract_addr(pak, &laddr, DESTINATION);
    soc = open_ipsocket(UDP_PROT, &laddr, &faddr, lport, fport, 0);
    if (!soc) {
	return;
    }
    tftp_ERROR(soc, code, errmsg);
    reg_invoke_ip_close_socket(soc);
}

/*
 * tftp_read
 * Use TFTP to read a file from a host into a buffer.
 * Returns error codes defined in tftp.h
 */

static int tftp_read (char *filename, addrtype *host, boolean persistence,
	       uchar *buffer, ulong *buffersize, int maxretries, 
	       addrtype *answerhost, boolean verbose, 
	       boolean (*func)(uchar **buf, long *bytesleft, long len,
			       void *opaque_args),
	       void *func_arg)
{
    ipsocktype *soc;
    udptype *udp;
    ushort *data;
    paktype *pak;
    ushort blocknumber;
    boolean firsttime, done;
    long len, bytesleft, retries;
    uchar *offset;
    int retval;
    ushort timeout;
    short curtimeout;
    sys_timestamp ack_timer;
    addrtype srcaddr;
    idbtype *idb, *out_idb;
    ipaddrtype dummy, nexthop;
    iphdrtype *ip;
    int getfileretries;

    successBlocksInRow = 0;
    retries = maxretries;
    getfileretries = maxretries;
    firsttime = TRUE;
    done = FALSE;
    blocknumber = 1;
    bytesleft = *buffersize;
    *buffersize = 0;
    soc = tftp_open_socket(host, SOCK_ANYSRC);
    if (!soc) {
	return(ERROR_NOMEM);
    }

    /*
     * If there is a preferred boot interface for TFTP, set
     * it up with the socket now.
     */
    if (system_loading && ip_tftp_boot_idb)
	soc->if_output = ip_tftp_boot_idb;

    ip_arp_announcement();

    /*
     * A dummy arp entry is created to help in TFTP but it could prevent
     * successful TFTP operation over interfaces that require IP address
     * to link layer mappings.
     *
     * This is because the dummy arp entry is created with the IP address
     * of the TFTP server, for which we might not have a map table entry.
     * If a default-gateway is present (with the proper mapping),
     * this can result in the initial TFTP request being encapsulated
     * successfully, but subsequent TFTP packets failing in encapsulation.
     *
     * If we need to create a dummy entry later and the request is
     * unicast, find the outbound interface and the nexthop.
     * Later, if the reply comes back via the same interface, create a 
     * dummy path table entry with the precalculated nexthop info.
     *
     * Avoid creating path table entry if the outbound interface is 
     * unnumbered because we need to rely upon the gleaned arp table
     * entry to reach the server.
     */
    nexthop = 0;
    out_idb = NULL;
    dummy = host->ip_addr;
    if (!tftp_ip_router_active() && 
	(host->type == ADDR_IP) &&
	(dummy != -1) && !IPMULTICAST(dummy)) {
	out_idb = find_path(&dummy, IP_PATH_USE_IF_GUESS);
	/*
	 * Only create path table entries for interfaces which do not use
	 * ARP table entries at encapsulation time.
	 */
	if (out_idb && (out_idb->hwptr->status & IDB_SERIAL) &&
	    !out_idb->ip_unnumbered && !out_idb->arp_smds && 
	    !(BRIDGING_IF(out_idb, LINK_IP)))
	    nexthop = dummy;
    }

    tftp_RRQ(soc,filename, "octet");
    if (verbose) {
	printf("\nLoading %s ",filename);
	flush();
    }
    timeout = TFTP_MIN_TIMEOUT;
    TIMER_STOP(ack_timer);
    while (!done) {
	len = 0;
	if (verbose) {			/* psi code stubbed in bootstraps */
	    char c;

	    while ((c = getc(stdio)) != -1) {
		if (c == stdio->tty_escapechar) {
		    tftp_ERROR(soc, ERROR_UNDEF, "User abort");
		    printf(" [Interrupted]\n");
		    flush();
		    retval = ERROR_ABORT;
		    goto xit;
		}
	    }
	}
	if (TIMER_RUNNING(ack_timer)) {
	    curtimeout = timeout - ELAPSED_TIME(ack_timer);
	    /*
	     * Make sure we retransmit our ACK packets regardless of whether
	     * or not read_ipsocket() returns something.
	     */
	    if (curtimeout <= 0) {
		tftp_ACK(soc, blocknumber);
		GET_TIMESTAMP(ack_timer);
		curtimeout = timeout;
	    }
	} else {
	    curtimeout = timeout;
	}
	pak = read_ipsocket(soc, TRUE, curtimeout, &udp, &offset);
	if (!pak) {
	    if (retries-- > 0) {
		if (firsttime == TRUE)
		    tftp_RRQ(soc,filename, "octet");
		else {
		    tftp_ACK(soc,blocknumber-1);
		    GET_TIMESTAMP(ack_timer);
		}
		if (timeout < TFTP_MAX_TIMEOUT)
		    timeout += TFTP_BACKOFF_DELAY;
		if (verbose) {
		    printc('.');
                    successBlocksInRow = 0;
		    flush();
		}
	        continue;
	    } else {
		if (verbose) {
		    printf(" [timed out]\n");
		    flush();
		}
		retval = ERROR_TIMEOUT;
		goto xit;
	    }
	}
	
	timeout = TFTP_MIN_TIMEOUT;
	data = (unsigned short *) offset;
	switch (n2h(*data++)) {
	    case DATA:
		if (tftp_debug)
		    buginf("\nTFTP: Received block %d, socket_id %#x",
			   n2h(*data),soc);
		if (firsttime == TRUE) {
		    ip_extract_addr(pak, &soc->laddress, DESTINATION);
		    ip_extract_addr(pak, &soc->faddress, SOURCE);
		    ip_extract_addr(pak, answerhost, SOURCE);
		    idb = NULL;
		    if (pak->linktype == LINK_IP) {
			soc->laddress.ip_addr = (ipaddrtype)
			                        ip_reply_srcadr(pak);
			if (!soc->laddress.ip_addr) {
			    break;
			}
			
			/*
			 * Dont override routing, assume a second proxy-arp 
			 * will come in and change our path to the server.
			 * 
			 * If the reply is via the same interface as request
			 * and the nexthop has been precalculated, create
			 * a path table entry else create an arp table entry.
			 */
			if (!tftp_ip_router_active()) {
			    if (((pak->if_input == out_idb) && nexthop)
				&& (soc->faddress.ip_addr != nexthop))
				path_update(soc->faddress.ip_addr, nexthop,
					    pak->if_input);
			    else
				ip_arp_glean(pak);
			}
			soc->if_output = NULL;
			if (verbose) {
			    dummy = soc->faddress.ip_addr;
			    idb = reg_invoke_ip_first_hop(&dummy, dummy,
							  IP_PATH_DEFFLAGS);
			}
		    }
		    if (verbose) {
			printf("from %s", address_string(&soc->faddress));
			if (idb)
			    printf(" (via %s): ", idb->namestring);
			else
			    printf(": ");
			flush();
		    }
		    soc->fport = n2h(udp->sport);
		    soc->status &= ~SOCK_ANYSRC;
		    firsttime = FALSE;
		    retries = maxretries;
		    while (!process_is_queue_empty(soc->inq)) {	/* flush other replies */
			retbuffer(process_dequeue(soc->inq));
		    }
		} else {
		  ip_extract_addr(pak, &srcaddr, SOURCE);
		  if ((soc->fport != udp->sport) ||
		      !ip_match_address(&srcaddr, &soc->faddress)) {

			/* 
			 * Make sure it goes back to the offending server. 
			 * unknown transfer ID 
			 */
			tftp_sendERROR(pak, n2h(udp->dport), n2h(udp->sport),
				       ERROR_UNKID, "unknown transfer"
				       " connection" );
			break;
		    }
		}
		if (n2h(*data++) == blocknumber) {
		    ip = iphdrtype_start(pak);
		    tftp_ACK(soc,blocknumber);
		    GET_TIMESTAMP(ack_timer);
		    if (verbose) {
			/* only printf success every 10 good blocks */
			file_xfer_progress(++successBlocksInRow,
					   TFTP_BLOCKS_PER_BANG);
		    }
		    retries = maxretries;
		    blocknumber++;
		    len = n2h(ip->tl) - (ip->ihl << 2) - UDPHEADERBYTES - 4;
		    bytesleft -= len;
		    if (bytesleft < 0) {
		        tftp_ERROR(soc, ERROR_NOSPACE, 
                                   "Buffer overflow");
			if (verbose) {
			    printf(" [buffer overflow - %d/%d bytes]\n",
				   *buffersize, *buffersize + bytesleft);
			    flush();
			}
			retbuffer(pak);
			retval = ERROR_TOOBIG;
			goto xit;
		    }
		    bcopy(data, adjustpointer(buffer), len);
		    buffer += len;
		    *buffersize += len;
		    if (len < TFTP_MAXDATA) {
			if (verbose) {
			    printf("\n[OK - %d/%d bytes]\n", *buffersize,
				   *buffersize + bytesleft);
			    flush();
			}
			done = TRUE;
		    }
		} else {
		    if (verbose) {
			printc('O');
			flush();
		    }
		}
		break;
	    case ERROR:				/* ignore errors */
		if ((!ip_broadcast(host->ip_addr, pak->if_input)) &&
		      n2h(*data) == ERROR_NOFILE) {
		    /*
		     * Remote TFTP server could be a PC based system
		     * that does not respond correctly if received a read 
		     * request immediately after terminated from the 1st 
		     * file probing connection.  The file does exist
		     * (persistence flag set) so wait and retry.
		     */
		    if (persistence && firsttime && (getfileretries-- > 0)) {
                	tftp_ERROR(soc, ERROR_UNDEF, "Session terminated");
			reg_invoke_ip_close_socket(soc);
    			soc = tftp_open_socket(host, SOCK_ANYSRC);
    			if (!soc) {
			    return(ERROR_NOMEM);
    			}
			process_sleep_for(TFTP_MIN_TIMEOUT);
			tftp_RRQ(soc,filename, "octet");
		    } else {
			if (verbose) {
			    printf(" [File not found]\n");
			    flush();
		        }
			retbuffer(pak);
			retval = ERROR_NOFILE;
			goto xit;
		    }
		}
		break;

	    default:
		if (verbose) {
		    printc('E');
		    flush();
		}
		/*
		 * Make sure it goes back to the offending server. 
		 * Illegal TFTP operation
		 */
		tftp_sendERROR(pak, n2h(udp->dport), n2h(udp->sport),
			       ERROR_ILLOP, "Illegal operation");
		break;
	} /* switch */
	retbuffer(pak);		/* release buffer */
	if (func) {
	    if ((*func)(&buffer, &bytesleft, len, func_arg) == FALSE) {
                tftp_ERROR(soc, ERROR_UNDEF, "Session terminated");
		retval = ERROR_ABORT;
		goto xit;
	    }
	}
    } /* while (!done) */
    retval = ERROR_OK;
  xit:
    reg_invoke_ip_close_socket(soc);
    return(retval);
}

/*
 * tftp_file_read
 * A jacket routine for tftp_read() that conforms to the SERVICE_FILE_READ
 * calling conventions.  Index is FILE_ACCESS_TFTP.
 */

static int tftp_file_read (filetype *confg, uchar *buffer,
		           ulong *buffersize, boolean verbose,
			   boolean (*func)(uchar **buf, long *bytesleft,
			            long len, void *opaque_args),
			   void *func_arg)
{
    int code;

    code = tftp_read(confg->filename, &confg->ps.tuba.boothost, 
		     confg->persistence, buffer, buffersize, 
		     MAXRETRIES, &confg->ps.tuba.respondent,
		     verbose, func, func_arg);
    return (code);
}

/*
 * tftp_opcode_string
 * Return a string with the name of the opcode.
 */

char *tftp_opcode_string (int opcode)
{
    switch(opcode) {
    case RRQ:
	return("read request");
    case WRQ:
	return("write request");
    case DATA:
	return("data packet");
    case ACK:
	return("ack");
    case ERROR:
	return("error packet");
    default:
	return("packet with unknown opcode");
    }
}

/*
 * tftp_getack(soc, expect, server, retry_num)
 * Try to get an ACK for the current packet (block number expected).
 * Return TRUE if we get the expected ACK.
 * Return FALSE if we time out or receive an earlier ACK (retransmit request).
 * Return the TFTP error code added to ERROR_OFFSET on error conditions.
 */
 
#define ERROR_OFFSET  1000
#define TFTP_UNEXPECTED 2000   /* was 2*ERROR_OFFSET */

static const char tftp_err_rx[] = "\nTFTP: error code %d received - %s\n";
static const char tftp_unexpected[] = "\nTFTP: unexpected %s";

int tftp_getack (
    ipsocktype *soc,
    int expect,			/* block we are expecting to be acked */
    boolean server,
    int retry_num)
{
    paktype *pak;
    ushort *data;
    udptype *udp;
    int errorcode, opcode;
    ushort timeout;

    timeout = min(TFTP_MAX_TIMEOUT,
		  TFTP_MIN_TIMEOUT + retry_num*TFTP_BACKOFF_DELAY);
    pak = read_ipsocket(soc,TRUE,timeout,&udp,&data);
    if (!pak) {
	return(FALSE);
    }
    opcode = n2h(*data++);
    switch (opcode) {
	case ACK:
	    if (tftp_debug)
		buginf("\nTFTP: Received ACK for block %d, socket_id %#x",
		       n2h(*data),soc);
	    if (n2h(*data) == expect) {
		if (expect == 0) {
		    ip_extract_addr(pak, &soc->faddress, SOURCE);
		    ip_extract_addr(pak, &soc->laddress, DESTINATION);
		    soc->if_output = pak->if_input;
		    if (pak->linktype == LINK_IP) {
			soc->laddress.ip_addr = (ipaddrtype) 
			    ip_reply_srcadr(pak);

			/*
			 * Dont override routing, assume a second proxy-arp 
			 * will come in and change our path to the server.
			 */
			if (!tftp_ip_router_active()) {
			    ip_arp_glean(pak);
			}
			soc->if_output = NULL;
		    }
		    soc->fport = udp->sport;
		    soc->status &= ~SOCK_ANYSRC;
		}	
		errorcode = TRUE;
	    } else
		errorcode = FALSE;
	    break;
	case ERROR:
	    errorcode = n2h(*data++);
	    if (!server)
		printf(tftp_err_rx, errorcode,
		       data);
	    else if (tftp_debug)
		buginf(tftp_err_rx, errorcode, data);
	    errorcode += ERROR_OFFSET;
	    break;
	default:
	    if (!server)
		printf(tftp_unexpected, tftp_opcode_string(opcode));
	    else if (tftp_debug)
		buginf(tftp_unexpected, tftp_opcode_string(opcode));
	    errorcode = TFTP_UNEXPECTED; /* 2*ERROR_OFFSET */
	    break;
    }
    retbuffer(pak);
    return(errorcode);
}

/*
 * tftp_WRQ(soc,filename,mode,server)
 * Transmits a TFTP write request for the given file name in the
 *  specified mode, and then waits for the ACK to come back.
 * Returns TRUE if everything is OK, FALSE on fatal errors.
 */

boolean tftp_WRQ (ipsocktype *soc, const char *filename, const char *mode, boolean server)
{
    int i, len;
    uint offset;
    boolean result;
    char *data;
    const char *p;
    paktype *pak;

    for (i = 0; i < TFTP_WRITE_RETRIES; i++) {
	/*
	 * Length is filename string, a NULL byte, 2 byte opcode,
	 * mode string, and another NULL byte.
	 */
	len = strlen(filename) + 1 + 2 + strlen(mode) + 1;
	pak = setup_ipsocket(soc,len,&offset);
	if (!pak) {
	    return(FALSE);
	}
	data = (char *)(pak->data_area + offset);
	*data++ = 0; *data++ = WRQ;		/* set opcode to WRQ */
	for (p = filename; (*data++ = *p++); );	/* filename */
	for (p = mode; (*data++ = *p++); );	/* transfer mode */
	if (tftp_debug) {
	    buginf("\nTFTP: Sending write request "
		   "(retry %d), socket_id %#x", i,soc);
	}
	write_ipsocket(soc,pak,len);
	result = tftp_getack(soc, 0, server, i);
	switch (result) {
	  case TRUE:				/* received ACK */
	    if (!server)
	        file_xfer_progress(1, TFTP_BLOCKS_PER_BANG);
	    return(TRUE);
	  case FALSE:				/* timeout, try again */
	  case TFTP_UNEXPECTED:
	    if (!server) {
	        printc('.');
	        flush();
	    }
	    break;
	  default:				/* error */
	    return(FALSE);
	}
    }
    return(FALSE);
}

/*
 * tftp_sendblock
 * Transmit LEN bytes from BUFFER using block number blknum, and then
 * wait for the ACK to come back.  Do retransmissions if needed.
 * Returns TRUE if everything OK, FALSE on fatal errors.
 */

boolean tftp_sendblock (
    ipsocktype *soc,
    int blknum,
    uchar *buffer,
    int len,		/* either 512, or last packet */
    boolean server)	/* output status if not server */
{
    int i;
    uint offset;
    boolean result;
    ushort *data;
    paktype *pak;
    static int retried = 0; /* May get inconsistent !'s with concurrency */

    for (i = 0; i < TFTP_WRITE_RETRIES; i++) {
	/*
	 * Length is 2 byte opcode, 2 byte block number, plus data
	 */
	pak = setup_ipsocket(soc,len+4,&offset);

	/* If we couldn't get a buffer, wait a sec and try again. */

	if (!pak) {
	    if (!server) {
		printc('M');
	    	flush();
	    }
	    process_sleep_for(ONESEC);
	    continue;
	}

	data = (ushort *)(pak->data_area + offset);
	*data++ = h2n(DATA);
	*data++ = h2n(blknum);
	bcopy(buffer, data, len);
	if (tftp_debug)
	    buginf("\nTFTP: Sending block %d (retry %d), socket_id %#x",
		   blknum,i,soc);
	write_ipsocket(soc,pak,len+4);
	result = tftp_getack(soc, blknum, server, i);
	switch (result) {
	  case TRUE:			/* received ACK */
	    if (!server) {
		file_xfer_progress(retried ? 1 : blknum,
				   TFTP_BLOCKS_PER_BANG);
		retried = 0;
	    }
	    return(TRUE);
	  case FALSE:			/* timeout or retransmit request */
	  case TFTP_UNEXPECTED:
	    if (!server) {
		retried++;
		printc('.');
		flush();
	    }
	    break;
	  default:			/* error */
	    return(FALSE);
	}
    }
    return(FALSE);
}

/*
 * tftp_write_socket(soc,filename,buffer,mode,buffersize)
 * TFTP a buffer to an open socket.
 * Returns TRUE if file successfully xferred, FALSE if some error.
 */

boolean tftp_write_socket (ipsocktype *soc, const char *filename,
			   uchar *buffer, const char *mode, uint buffersize)
{
    int blknum, nblks;
    boolean result;

    printf("\nWriting %s ", filename);
    flush();
    result = reg_invoke_ip_tftp_WRQ(soc,filename,mode, FALSE);
    if (result == TRUE) {

	nblks = (buffersize/TFTP_MAXDATA) + 1;	/* total number of blocks */
	for (blknum=1; blknum < nblks; blknum++) {
	    result = reg_invoke_ip_tftp_sendblock(soc,blknum,buffer,
						  TFTP_MAXDATA,FALSE);
	    if (result != TRUE) {
	        break;
	    }
	    flush();
	    buffer += TFTP_MAXDATA;
	    buffersize -= TFTP_MAXDATA;
	}
	/*
	 * Send last (partial) block of data.
	 */
	if (result == TRUE) {
	    result = reg_invoke_ip_tftp_sendblock(soc, blknum, buffer,
						  buffersize, FALSE);
	}
    }
    return(result);
}

/*
 * tftp_dump_core_file - Write out a core file using TFTP
 */
static
boolean tftp_dump_core_file (uchar *mem, ulong size,
			     const char *filename, ipaddrtype host,
			     ipsocktype *soc)
{
    boolean result;
    addrtype *lhost;
    addrtype laddr;
    addrtype faddr;

    /*
     * Kludge.  If memory size is 16M, reduce by one TFTP block to
     * avoid garbage dumps due to tftpd bugs.
     * If we are dealing with memory more than 16M, atleast dump
     * 16M properly.
     */

    if (size >= TFTPD_FAILURE_SIZE)
 	size = TFTPD_FAILURE_SIZE - TFTP_MAXDATA;

    if (soc) {
	/*
	 * Socket was opened when user configured exception dump. 
	 * If user has specified a different source address for all 
	 * tftp connections, update the socket local address field
	 * with the new address.
	 */
	lhost = tftp_get_laddr(&laddr);
	if (lhost && (soc->laddress.ip_addr != lhost->ip_addr))
	    soc->laddress.ip_addr = lhost->ip_addr;
    } else {
	faddr.type = ADDR_IP;
	faddr.ip_addr = host;
	soc = tftp_open_socket(&faddr, SOCK_ANYSRC);
    }
    if (!soc) {
	printf("\nCan't open socket to dump host %i", host);
	return(FALSE);
    }

    result = tftp_write_socket(soc, filename, mem, "octet", size);
    close_ipsocket(soc);
    return(result);
}

/*
 * tftp_dump_slave_core_file - Write out a slave core file using TFTP
 * Assumes caller only writes one slave core at a time.
 */
static
boolean tftp_dump_slave_core_file (uchar *mem, ulong size,
                             const char *filename, ipaddrtype host,
                             char flags)
{
    boolean result;
    static ipsocktype *soc;
    static blknum=1;
    addrtype faddr;
 
    switch (flags) {
    case TFTP_OPEN_SLAVECORE_SOCKET:
        if (soc)
            close_ipsocket(soc);
 
	faddr.type = ADDR_IP;
	faddr.ip_addr = host;
	soc = tftp_open_socket(&faddr, SOCK_ANYSRC);
        if (!soc) {
            printf("\nCan't open socket to dump host %i", host);
            return(FALSE);
        } else {
            return(reg_invoke_ip_tftp_WRQ(soc,filename,"octet",FALSE));
        }
        break;
 
    case TFTP_WRITE_SLAVECORE_SOCKET:
        if (!soc) {
            printf("\nCan't write to  socket; dump host %i", host);
            return(FALSE);
        } else {
            printf("!");
            result =  reg_invoke_ip_tftp_sendblock(soc,blknum++,mem,
                                                  TFTP_MAXDATA,FALSE);
            flush();
            return (result);
        }
        break;
 
    case TFTP_CLOSE_SLAVECORE_SOCKET:
 
        printf("\n");
        blknum = 1;
        if (!soc) {
            printf("\nCan't close NULL socket to dump host %i", host);
            return(FALSE);
        } else {
            close_ipsocket(soc);
            soc = NULL;
            return (TRUE);
        }
    }
    printf("\nNo slave_core_dump flag specified.\n");
    return(FALSE);
}

/*
 * tftp_write (filename,host,buffer,mode,buffersize)
 * The high level access for writing a file to some host using TFTP.
 *
 * this is really painful!  would really like to call tftp_file_write,
 * but the way the parser chain calls this, we have no concept of a
 * a filetype!  this sucks.  beavis
 */

static void tftp_write (char *filename, addrtype *host, uchar *buffer,
                      char *mode, uint buffersize)
{
    ipsocktype *soc;
    boolean result;
    addrtype *lhost;
    addrtype laddr;

    lhost = tftp_get_laddr(&laddr);
    soc = open_ipsocket(UDP_PROT, lhost, host, 0, TFTP_PORT, SOCK_ANYSRC);
    if (!soc)
        return;
    result = tftp_write_socket(soc,filename,buffer,mode,buffersize);
    close_ipsocket(soc);
    return;
}
/*
 * tftp_file_write
 * The high level access for writing a file to some host using TFTP.
 * Returns TRUE if file successfully transferred, FALSE if some error.
 */

static boolean tftp_file_write (filetype *file, char *buffer,
				uint buffersize)
{
    ipsocktype *soc;
    boolean result;
    const char *mode;

    if (file->proto != FILE_ACCESS_TFTP) {
	return(FALSE);
    }
    soc = tftp_open_socket(&file->ps.tuba.boothost, SOCK_ANYSRC);
    if (!soc) {
	return(FALSE);
    }
    if (ip_tftp_ascii_mode)
	mode = "netascii";
    else
	mode = "octet";
    result = tftp_write_socket(soc, file->filename, buffer, mode, buffersize);
    reg_invoke_ip_close_socket(soc);
    return(result);
}

/*
 * tftp_ip_router_active
 * Determine if TFTP should make use of ARP table
 */

boolean tftp_ip_router_active (void)
{
    /*
     * If we are not running up and running as an IP router,
     * we should make use of the arp table.
     */
    if (!systeminit_flag || !ip_router_running) {
 	return(FALSE);
    }
    return(TRUE);
}

/*
 * tftp_file_match
 * Does the protocol dependent information in two file descriptors match?
 * Called by SERVICE_FILE_MATCH with index FILE_ACCESS_TFTP
 */

static boolean tftp_file_match (filetype *cfg1, filetype *cfg2)
{
    return(ip_match_address(&cfg1->ps.tuba.boothost, &cfg2->ps.tuba.boothost));
}

/*
 * tftp_bootinfo_get 
 * Record protocol specific boot information.
 * Called by SERVICE_BOOTINFO_GET with index of FILE_ACCESS_TFTP
 */

static void tftp_bootinfo_get (bootinfo_t *bootinfo, filetype *sysconfg)
{
    sysconfg->ps.tuba.respondent.type = ADDR_IP;
    sysconfg->ps.tuba.respondent.ip_addr = bootinfo->ipaddr;
}

/*
 * tftp_bootinfo_set
 * After loading in the image, tell bootstrap where it came from.
 * Used by SERVICE_BOOTINFO_SET with index FILE_ACCESS_TFTP
 */

static void tftp_bootinfo_set (bootinfo_t *bootinfo, filetype *sysconfg)
{
    bootinfo->ipaddr = sysconfg->ps.tuba.respondent.ip_addr;
}

/*
 * tftp_file_source
 * Return string indicating the source of a file
 * Called by SERVICE_FILE_SOURCE, index is FILE_ACCESS_TFTP
 */

static void tftp_file_source (filetype *file, uchar *buffer)
{
    sprintf(buffer, "tftp from %s",
	    address_string(&file->ps.tuba.respondent));
}

/*
 *
 * tftp_bootfile_parse
 * Given a protocol name and parts of a source address, figure out if
 * TFTP should be used to access the file.  Set up fields in file
 * descriptor as appropriate.  Returns TRUE if there was a match.
 * Called by SERVICE_BOOTFILE_PARSE, index is FILE_ACCESS_TFTP
 */

static boolean tftp_bootfile_parse (filetype *file, char *protocol,
				    char *host, char *arg)
{
    ipaddrtype boothost;

    if (!strmatch(protocol, "tftp", FALSE))
	return(FALSE);
    if (!null(host)) {
	if (!parse_hostname(host, &boothost)) {
	    printf("%% Bad IP address: %s", host);
	    return(FALSE);
	}
    } else
	boothost = -1L;
    file->proto = FILE_ACCESS_TFTP;
    file->ps.tuba.boothost.type = ADDR_IP;
    file->ps.tuba.boothost.ip_addr = boothost;
    return(TRUE);
}

/*
 * tftp_file_parse
 * Fill in protocol specific information when parsing a file specification
 * Called by SERVICEFILE_PARSE, index is FILE_ACCESS_TFTP
 */

static void tftp_file_parse (filetype *file, parseinfo *csb)
{
    char *bufptr;

    if (csb->nvgen) {
	nv_add(TRUE, "%s %s", file->filename, 
	       address_string(&file->ps.tuba.boothost));
        /*
         * Now add boot command to ROM monitor BOOT variable.
         */
        bufptr = reg_invoke_monvar_buff(ROMMON_BOOT_VAR);
        if (bufptr) {
            bufptr += strlen(bufptr);
            sprintf(bufptr, "tftp:%s %s,%d;", file->filename,
                    address_string(&file->ps.tuba.boothost),
                    BOOTRETRIES);
        }
	return;
    }
    file->proto = FILE_ACCESS_TFTP;
    file->ps.tuba.boothost.type = ADDR_IP;
    file->ps.tuba.boothost.ip_addr = -1;	/* default address */
    if (GETOBJ(paddr,1)->type == ADDR_IP) {	/* IP address */
	file->ps.tuba.boothost.type = ADDR_IP;
	file->ps.tuba.boothost.ip_addr = GETOBJ(paddr,1)->ip_addr;
    }
}

/*
 * ip_tftp_command
 * IP TFTP command processing
 */

static void ip_tftp_command (parseinfo *csb)
{
    switch (csb->which) {
    case IP_TFTPMODE:
	if (csb->nvgen)
	    nv_write(ip_tftp_ascii_mode, csb->nv_command);
	else
	    ip_tftp_ascii_mode = csb->sense;
	break;
    case IP_TFTP_SOURCE:
	if (csb->nvgen) {	
	    if (tftp_source_idb)
		nv_write(TRUE, "%s %s", csb->nv_command, 
			 tftp_source_idb->namestring);
	} else {
	    if (csb->sense)
		tftp_source_idb = GETOBJ(idb,1);
	    else
		tftp_source_idb = NULL;
	}
	break;
    case IP_TFTP_BOOT_INTERFACE:
	if (csb->nvgen) {
	    if (ip_tftp_boot_idb) {
		nv_write(TRUE, "%s %s", csb->nv_command,
			 ip_tftp_boot_idb->namestring);
	    }
	} else {
	    if (csb->sense)
		ip_tftp_boot_idb = GETOBJ(idb,1);
	    else
		ip_tftp_boot_idb = NULL;
	}
	break;
    }
}

/*
 * Access(get/set) routine tftp batchibg command mode.
 * set_flag:TRUE, set tftp_is_batching to <value>
 * set_flag:FALSE, return tftp_is_batching 
 * this function is useful for those sequence sensitive commands.
 * For example suppose user gives the command seq. as follows:
 * config>bridge irb
 * config>int bvi1
 *
 * It's mandatory to give the first command to carry out the 
 * second one.
 *
 * Then he copies cfg file to tftp server. The sequence in the tftp
 * file got reversed. (IOS always puts interface command first)
 * When he copies it back to the running configure, he fails
 * in the int bvi1 command since the order is reversed.
 * To resolve this, whenever we are doing batch command mode for cfg
 * file we set the <tftp_is_batching> to TRUE. At the checking side, 
 * we do a call back to get <tftp_is_batching> to see if batching is underway,
 * If so, we can loosen the order constraints.
 */
static boolean ip_tftp_state_access (access_batch_type access_flag, boolean value)
{
    static boolean tftp_is_batching = FALSE;

    if (access_flag == ACCESS_BATCH_SET) {
    	tftp_is_batching = value;
    } 
    return tftp_is_batching;
}

/*
 * Parse chains for "copy" command
 * This could be modularized better, but at least this is a start.
 */
#include "../parser/parser_defs_flash.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_config.h"
#include "../dev/flash_defines.h"

#define ALTERNATE	NONE
/* extend "copy tftp ..." from here */
LINK_TRANS(exec_copy_tftp_file_extend_here, no_alt);
LINK_TRANS(exec_copy_tftp_extend_here, exec_copy_tftp_file_extend_here);

/* copy tftp startup-config */
EOLS    (exec_copy_tftp_start_conf_eol, manual_configure,
	 PARSER_CONF_OVERWRITE_NET);
KEYWORD (exec_copy_tftp_startup_config, exec_copy_tftp_start_conf_eol,
	 exec_copy_tftp_extend_here,
         "startup-config",
	 "Copy to startup configuration", PRIV_ROOT);

/* copy tftp running-config */
EOLS    (exec_copy_tftp_run_conf_eol, manual_configure,
	 PARSER_CONF_NET);
KEYWORD (exec_copy_tftp_running_config, exec_copy_tftp_run_conf_eol,
	 exec_copy_tftp_startup_config, 
         "running-config",
	 "Update (merge with) current system configuration", PRIV_ROOT);

/* copy tftp flash */
EOLS    (exec_copy_tftp_flash_eol, flash_copy_command, COPY_TFTP_FLASH);
KEYWORD (exec_copy_tftp_flash, exec_copy_tftp_flash_eol, 
        exec_copy_tftp_running_config,
        SF, "Copy to system flash", PRIV_OPR);

/* copy tftp bootflash */
EOLS    (exec_copy_tftp_bflash_eol, flash_copy_command, COPY_TFTP_BOOTFLASH);
KEYWORD (exec_copy_tftp_bflash, exec_copy_tftp_bflash_eol, exec_copy_tftp_flash,
        BF, "Copy to boot flash", PRIV_OPR);
IFELSE  (ifelse_copy_tftp, exec_copy_tftp_bflash, exec_copy_tftp_flash,
         reg_invoke_flash_alias_match(BF));

/* copy tftp ... */
KEYWORD_ID (exec_copy_tftp, ifelse_copy_tftp, ALTERNATE,
	    OBJ(int,1), COPY_TFTP,
	    "tftp", "Copy from a TFTP server", PRIV_OPR);
LINK_POINT(tftp_copy_commands, exec_copy_tftp);
#undef ALTERNATE

/*
 * Copying various things to tftp
 */

/* copy startup-config tftp */
EOLS    (exec_copy_start_conf_tftp_eol, write_network, WRITE_NETWORK);
KEYWORD_ID (exec_copy_start_conf_tftp, exec_copy_start_conf_tftp_eol, NONE,
	    OBJ(int,1), COPY_TFTP,
	    "tftp", 
	    "Copy to a TFTP server", PRIV_ROOT);
LINK_POINT(tftp_copy_start_commands, exec_copy_start_conf_tftp);

/* copy running-config tftp */
EOLS    (exec_copy_run_conf_tftp_eol, write_network, WRITE_NETWORK);
KEYWORD_ID (exec_copy_run_conf_tftp, exec_copy_run_conf_tftp_eol, NONE,
	    OBJ(int,1), COPY_TFTP, 
            "tftp",
	    "Copy to a TFTP server", PRIV_ROOT);
LINK_POINT(tftp_copy_run_commands, exec_copy_run_conf_tftp);

/* copy flash tftp */
EOLS    (exec_copy_flash_tftp_eol, flash_copy_command, COPY_FLASH_TFTP);
KEYWORD (exec_copy_flash_tftp, exec_copy_flash_tftp_eol, NONE,
        "tftp", "Copy to a TFTP server", PRIV_OPR);
LINK_POINT(tftp_copy_flash_commands, exec_copy_flash_tftp);

/* copy bootflash tftp */
EOLS    (exec_copy_bflash_tftp_eol, flash_copy_command, COPY_BOOTFLASH_TFTP);
KEYWORD (exec_copy_bflash_tftp, exec_copy_bflash_tftp_eol, NONE,
        "tftp", "Copy to a TFTP server", PRIV_OPR);
LINK_POINT(tftp_copy_bflash_commands, exec_copy_bflash_tftp);

#define ALTERNATE       NONE
#include "cfg_ip_tftp.h"
LINK_POINT(tftp_ip_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for tftp
 */
static parser_extension_request tftp_chain_table[] = {
    { PARSE_ADD_COPY_CMD, &pname(tftp_copy_commands) },
    { PARSE_ADD_COPY_STARTUP_CMD, &pname(tftp_copy_start_commands) },
    { PARSE_ADD_COPY_RUNNING_CMD, &pname(tftp_copy_run_commands) },
    { PARSE_ADD_COPY_FLASH_CMD, &pname(tftp_copy_flash_commands) },
    { PARSE_ADD_COPY_BFLASH_CMD, &pname(tftp_copy_bflash_commands) },
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(tftp_ip_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * tftp_init
 * Initialize TFTP support 
 */

static void tftp_init (subsystype *dummy)
{
    tftp_debug_init();
    tftp_serving = FALSE;
    tftpdata = NULL;

    if (!system_loading) {
	/* copy tftp ... */
	parser_add_link_point(PARSE_ADD_COPY_TFTP_CMD,
			      "copy tftp ...",
			      &pname(exec_copy_tftp_extend_here));
	parser_add_link_point(PARSE_ADD_COPY_TFTP_FILE_CMD,
			      "copy tftp ...",
			      &pname(exec_copy_tftp_file_extend_here));
    }
    parser_add_command_list(tftp_chain_table, "tftp");

    /*
     * Register some functions.
     */
    reg_add_file_match(FILE_ACCESS_TFTP, tftp_file_match, "tftp_file_match");
    reg_add_bootinfo_get(FILE_ACCESS_TFTP,
		      tftp_bootinfo_get, "tftp_bootinfo_get");
    reg_add_bootinfo_set(FILE_ACCESS_TFTP,
		      tftp_bootinfo_set, "tftp_bootinfo_set");
    reg_add_file_source(FILE_ACCESS_TFTP,
		      tftp_file_source, "tftp_file_source");
    reg_add_bootfile_parse(tftp_bootfile_parse, "tftp_bootfile_parse");
    reg_add_file_parse(FILE_ACCESS_TFTP, tftp_file_parse, "tftp_file_parse");
    reg_add_file_read(FILE_ACCESS_TFTP, tftp_file_read, "tftp_file_read");
    reg_add_file_write(FILE_ACCESS_TFTP, tftp_file_write, "tftp_file_write");
    reg_add_config_read(FILE_ACCESS_TFTP, ip_get_config_file,
			"ip_get_config_file");
    reg_add_config_write(FILE_ACCESS_TFTP, ip_put_config_file,
			 "ip_put_config_file");
    reg_add_noninteractive_config_read(FILE_ACCESS_TFTP,
			tftp_noninteractively_get_config_file, 
			"tftp_noninteractively_get_config_file");
    reg_add_core_dump(FILE_ACCESS_TFTP,
		      tftp_dump_core_file, "tftp_dump_core_file");
    reg_add_slave_core_dump(FILE_ACCESS_TFTP,
                      tftp_dump_slave_core_file, "tftp_dump_slave_core_file");
    reg_add_ip_tftp_open_socket(tftp_open_socket, "tftp_open_socket");
    reg_add_ip_tftp_WRQ(tftp_WRQ, "tftp_WRQ");
    reg_add_ip_tftp_sendblock(tftp_sendblock, "tftp_sendblock");
    reg_add_tftp_write(tftp_write, "tftp_write");
    reg_add_ip_tftp_ERROR(tftp_ERROR, "tftp_ERROR");
    reg_add_ip_tftp_server_init(ip_tftp_server_init, "ip_tftp_server_init");
    reg_add_access_batch_cmd(ip_tftp_state_access,"ip_tftp_state_access");

    tftp_parser_init();
}

/*
 * TFTP subsystem header
 */

#define TFTP_MAJVERSION 1
#define TFTP_MINVERSION 0
#define TFTP_EDITVERSION  1

SUBSYS_HEADER(tftp, TFTP_MAJVERSION, TFTP_MINVERSION, TFTP_EDITVERSION,
	      tftp_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
