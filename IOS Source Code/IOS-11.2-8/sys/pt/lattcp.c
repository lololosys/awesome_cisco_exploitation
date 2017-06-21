/* $Id: lattcp.c,v 3.6 1996/03/06 09:31:23 billw Exp $
 * $Source: /release/111/cvs/Xsys/pt/lattcp.c,v $
 *------------------------------------------------------------------
 * lattcp.c -- LAT -> TCP protocol conversion.
 *
 * May 1990,  Bill Westfield
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lattcp.c,v $
 * Revision 3.6  1996/03/06  09:31:23  billw
 * CSCdi44863:  AAA Accounting not done for one-step protocol translation
 * Also ensure tty->ustruct gets set for one-step PT sessions.
 *
 * Revision 3.5  1996/01/23  23:45:20  vandys
 * CSCdi45547:  Desire rotary behavior on PT TCP destinations
 * Add rotor option to randomize destination for TCP host lists
 *
 * Revision 3.4  1995/11/22  05:56:16  eschaffe
 * CSCdi44200:  !23 ports in the translate command should override ip host
 * port
 *
 * Revision 3.3  1995/11/20  23:22:02  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  18:59:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:22:19  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/13  21:48:05  eschaffe
 * CSCdi18102:  Require one translate statment to multiple hosts
 *         Add "host-name" Telnet/TCP outgoing option.
 *
 * Revision 2.2  1995/06/09  13:18:17  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:34:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../lat/mtcsys.h"
#include "../lat/latsys.h"
#include "../lat/latob.h"
#include "../lat/latsb.h"
#include "../lat/lathi.h"
#include "../lat/latslt.h"
#include "address.h"
#include "name.h"
#include "packet.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "ttysrv.h"
#include "connect.h"
#include "../x25/x25.h"
#include "../x25/x25_address.h"
#include "../x25/x25_switch.h"
#include "../x25/pad.h"
#include "config.h"
#include "../tcp/telnet.h"
#include "translate.h"
#include "../os/async.h"
#include "termqueue.h"
#include "../lat/sysfnc.h"
#include "../lat/latfnc.h"
#include "login.h"
#include "../lat/lat.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "stacks.h"

#include "tcplat.h"

/*
 * local storage
 */


/*
 * lattcp
 * translate from incoming LAT to outgoing TCP.
 *
 */

static forktype lattcp (translatetype *trans)
{
    tt_soc *tty = stdio;
    conntype *latconn = tty->conntop;
    conntype *tcpconn;
    int n, quietflag;
    hi_ptr hi = latconn->proto.lat.hiptr;
    void *sb;
    addrtype address, source_address, *saddr;
    short tcpport;
    nametype *nameptr;

    if (translate_debug)
	buginf("\nlattcp%t: fork %d started", tty->ttynum, tty->tiptop);

    tty->ovector = &LATvector;
    tty->ivector = &LATvector;

    /*
     * Exit if its a HIC connection and requires password 
     * else proceed with password check 
     */
    if ((trans->flags & TRANSF_GLOB_LOGIN) &&
        (latconn->proto.lat.flags & LAT_CONN_HIC)) {
	    printf("\n%%Translate: Can't query for password on HIC connection");
	    trans->failures++;
	    death(tty);
    }

    trans->active++;
    if (trans->active > trans->maxusers)
	trans->maxusers = trans->active;

    /*
     * Perhaps check for a password.  Diddle the IO vectors and
     * use the EXEC code...
     */
    pt_do_login(trans, tty);

    n = 0;
    quietflag = (trans->flags & TRANSF_GLOB_QUIET) || 
	(latconn->proto.lat.flags & LAT_CONN_HIC);

    address.type = ADDR_IP;
    tcpport = trans->out.tcp.tcpport;

    saddr = telnet_source_address(trans->out.tcp.source_idb, &source_address);
    if (trans->out.tcp.tcpflags & TRANSF_TCP_HOST_NAME) {
        nameptr = trans->out.tcp.tcpnptr;
        /*
         * Attempt to open the next destination if the first one fails.
         * The IP host string contains a host name, port, and IP
         * addressess.
         * Where "IP address" can be one to MAXADDRESSES.
         * "port" is an optional 16 bit port number which will be used
         * instead of the default telnet port.
	 *
	 * If the port in the translate command is not a telnet port, then it 
	 * should override the host port.
	 */
	if (tcpport == TELNET_PORT && nameptr->v.ip.port)
	    tcpport = nameptr->v.ip.port;
        tcpconn = open_connection(nameptr, NULL, tcpport, saddr,
	    (quietflag ? &n : NULL), NULL, trans->out.tcp.tcpflags, NULL);
    } else {
        address.ip_addr = trans->out.tcp.ipaddress;
        tcpconn = open_connection(NULL, &address, tcpport, saddr,
	    (quietflag ? &n : NULL), NULL, trans->out.tcp.tcpflags, NULL);
    }

    if (tcpconn == NULL) {
	if (latconn->proto.lat.flags & LAT_CONN_HIC) {
	    /*
	     * try again later on the queue lat connection
	     */
hicqueue:
            /*
             * If Data Buffer is allocated, deallocate it
             */
            if (latconn->proto.lat.databuf) {		
		free(latconn->proto.lat.databuf);	
		latconn->proto.lat.databuf = NULL;	       
            }
	    freeconnection(latconn);
	    if (translate_debug)
		buginf("\nlattcp%t: connection remains queued", tty->ttynum);
	    trans->active--;
	    tty_reset(tty);
	    tty_deallocate(tty); /* line is no longer assigned */
	    kfork(tty);
	    return;
	} /* else */
	startoutput(tty);
	if (translate_debug) {
	    buginf("\nlattcp: can't get tcp connection to %i",
		   trans->out.tcp.ipaddress);
	    if (n > 0)
		buginf(tcperror2string(n));
	}
        trans->active--;
	trans->failures++;
	death(tty);
    } else if (latconn->proto.lat.flags & LAT_CONN_HIC) {
	/*
	 * CARDROP will get set as tcp_open tries to do stuff with the
	 * VTY conntop, which of course isn't set up yet.  Clear it.
	 */
	tty->statbits &= ~CARDROP;
	/*
	 * The outgoing conection worked.  Open up the incoming connection,
	 * and remove it from the terminal queue.
	 */
	termqueue_remove(hi);
	str_return((byte *)latconn->hostname, hi->hi_node, CONNLEN);
	/* Set up connection name */
	latconn->proto.lat.max_credits = LAT_INBOUND_CREDITS;
	sb = hi_open(hi, latconn, latconn->proto.lat.max_credits);
	if (sb) {
	    latconn->status |= CONN_INBOUND; /* Mark as inbound */
	    latconn->proto.lat.t_icapabilities = tty->capabilities;
	    latconn->proto.lat.t_capabilities = tty->capabilities;
	    latconn->proto.lat.t_parity = tty->tty_parity;
	    latconn->proto.lat.t_databits = tty->tty_databits;
	    latconn->tcb =  sb;
	    latconn->proto.lat.hiptr = NULL; /* no longer queued */
	    tty->tty_autocommand = NULL;
	} else {
	    hi_reject(hi, SLOT_STOP_NO_RESOURCE);	/* No resources */
	    goto hicqueue;
	}
	edisms((blockproc *)lat_openBLOCK, (ulong) latconn);
    }
    ((tcbtype *)tcpconn->tcb)->flags |= TCB_SYNCCLOSE;
    tty->statbits |= CONNECTED;		/* Mark as connected */
    psioff(tty);
    tcpconn->acct_info = reg_invoke_aaa_acct_conn(tty->ustruct, NULL,
						  tcpconn);
    telnet(tcpconn, FALSE);
    trans->active--;
    tty->statbits &= ~CONNECTED;	/* Not connected anymore */
    conn_close(tcpconn);
    death(tty);
    process_kill(THIS_PROCESS);
}

/*
 * lat_transto_tcp
 * Translate LAT input to TCP output
 * Returns PID of translation process or -1 if failure
 * Called by SERVICE_TRANS_LATINPUT, index is TRANS_TCP
 */

int lat_transto_tcp (translatetype *trans, tt_soc *tty)
{
    ulong stacksize = PT_DEFAULT_STACK_SIZE;

    if (reg_invoke_kerberos_is_running())
	stacksize = (ulong) EXEC_STACK_SIZE;
    return(cfork((forkproc *)lattcp, (long)trans, stacksize, "LAT to TCP",
		 tty->ttynum));
}
