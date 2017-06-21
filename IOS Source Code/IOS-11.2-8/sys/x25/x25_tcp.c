/* $Id: x25_tcp.c,v 3.5.18.2 1996/08/28 13:23:49 thille Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_tcp.c,v $
 *------------------------------------------------------------------
 * x25_tcp.c -- X25 TCP support
 *
 * July 1989, Greg Satz
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_tcp.c,v $
 * Revision 3.5.18.2  1996/08/28  13:23:49  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.18.1  1996/03/18  22:50:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.2  1996/03/07  11:20:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  21:56:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/08  18:02:19  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.4  1996/01/16  03:38:16  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.3  1995/11/21  08:27:40  ahh
 * CSCdi42413:  XOT: cannot tune TCP keepalive behavior
 * Allow keepalive rate to be controlled for XOT connections.
 *
 * Revision 3.2  1995/11/17  18:08:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "../tcp/tcpdriver.h"
#include "../tcp/tcpinternal.h"
#include "x25.h"
#include "x25_switch.h"
#include "x25_tcp.h"
#include "logger.h"
#include "../x25/msg_x25.c"		/* Not a typo, see logger.h */


/*
 * x25_switch_tcp_open
 * Return connection info for active open.
 */
tcp_encaps *x25_switch_tcp_open (
    ipaddrtype ipaddr,
    ipaddrtype sipaddr,
    void *cookie,
    uchar *diag)
{
    int status;
    tcp_encaps *t;
    leveltype level = 0;
    tcp_encaps tmp;

    /*
     *	we must not open a TCP connection with interrupts disabled,
     *	since it will invoke a task-level wait until the connection is
     *	established
     */
    if (get_interrupt_level()) {
	level = set_interrupt_level(ALL_ENABLE);
    }
    memset(&tmp, 0, sizeof(tcp_encaps));
    tmp.readf = x25_switch_tcp_readtrans;
    tmp.closef = x25_switch_tcp_netclose;
    tmp.flushf = NULL;
    tmp.flow_ctrl = NULL;
    tmp.inthresh = sizeof(transtype);
    tmp.cookie = cookie;
    tmp.flags = 0;
    t = reg_invoke_ip_tcpdriver_active_open(ipaddr, sipaddr, X25_SVC_PORT,
					    X25_TCP_WINDOWSIZE, &tmp, &status);
    if (level) {
	reset_interrupt_level(level);
    }

    if (t) {
	reg_invoke_ip_tcpdriver_change_timeout(t, TRUE);
    } else {
	switch (status) {
	case REJECTED:
	    *diag = X25_DIAG_INCOMING_CALL_BARRED;
	    break;
	case TIMEDOUT:
	    *diag = X25_DIAG_TIMER_EXPIRED;
	    break;
	case UNREACHABLE:
	    *diag = X25_DIAG_TEMP_ROUTING_ERROR;
	    break;
	}
    }
    return(t);
}

/*
 * x25_switch_tcp_listen
 * Handle building of incoming TCP connection on server port.
 * Hand to the TCP driver interface.
 */
void x25_switch_tcp_listen (tcbtype *tcb)
{
    tcp_encaps tmp;
    tcp_encaps *t;

    memset(&tmp, 0, sizeof(tcp_encaps));
    tmp.readf = x25_switch_tcp_readtrans;
    tmp.closef = x25_switch_tcp_netclose;
    tmp.flushf = NULL;
    tmp.flow_ctrl = NULL;
    tmp.inthresh = sizeof(transtype);
    tmp.cookie = NULL;
    tmp.flags = 0;
    if ((t = reg_invoke_ip_tcpdriver_passive_open(tcb,
						  X25_TCP_WINDOWSIZE, &tmp))) {
	reg_invoke_ip_tcpdriver_change_timeout(t, TRUE);
    } else {
	tcp_close(tcb);
    }
    return;
}

/*
 * x25_switch_tcp_slam
 * Slam a TCP connection due to an error.
 */
static void x25_switch_tcp_slam (
    tcp_encaps *t,
    lcitype *lci,
    uchar diag,
    boolean normal)
{
    reg_invoke_ip_tcpdriver_update(t, x25_switch_tcp_read_flush, 1, lci);
    reg_invoke_ip_tcpdriver_close(t);

    if (!lci) {
	errmsg(&msgsym(VCLOSTSYNC, X25), "TCP/PVC", 0,
		normal ? "closed unexpectedly" : "corrupted");
    } else if (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
	lci->lci_pvc_status = X25_PVC_TCP_SHUT;
	t = ((lci->lci_swdata == t) ? lci->lci_pvc_swdata : lci->lci_swdata);
	if (t) {
	    reg_invoke_ip_tcpdriver_update(t, x25_switch_tcp_read_flush, 
					   1, lci);
	    reg_invoke_ip_tcpdriver_close(t);
	}
	lci->lci_swdata = NULL;
	lci->lci_pvc_swdata = NULL;

	x25_sendcause(lci->lci_idb->hwptr, lci, X25_RESET,
		      X25_RESET_NETWORK_OUT_OF_ORDER, diag, NULL);
    } else if (lci->lci_swdata) {
	lci->lci_swdata = NULL;

	errmsg(&msgsym(VCLOSTSYNC, X25), lci->lci_idb->namestring,
	       lci->lci_lci, normal ? "closed unexpectedly" : "corrupted");
	x25_sendcause(lci->lci_idb->hwptr, lci, X25_CLEAR,
		      X25_CLEAR_REMOTE_PROC_ERR, diag, NULL);
    }
}

/*
 * x25_switch_tcp_read_flush
 * read and ignore any data we get from TCP
 * used after closing the connection
 */
void x25_switch_tcp_read_flush (
    tcp_encaps *t,
    tcbtype *tcb,
    int count,
    void *cookie)
{
    int num;
    uchar *p;
    /*
     * flush the TCP driver's buffer
     */
    do {
	if (tcp_getstring(tcb, &p, &num, 0x1000) != OK)
	    break;
	} while (num != 0);
}

/*
 * x25_switch_tcp_readtrans
 * Perform TCP read for transaction header.
 */
void x25_switch_tcp_readtrans (
    tcp_encaps *t,
    tcbtype *tcb,
    int count,
    void *cookie)
{
    int num, status;
    transtype trans;
    uchar *p, *transp;
    lcitype *lci;

    lci = (lcitype *)cookie;

    /*
     *	sanity check of the requested count
     */
    if (count != sizeof(transtype)) {
	x25_switch_tcp_slam(t, lci, X25_DIAG_NO_INFO, FALSE);
	return;
    }

    num = tcp_unread(tcb);

    /*
     *	get a packet header, if one is available
     */
    if (num >= sizeof(transtype)) {
	transp = (uchar *)&trans;
	do {
	    status = tcp_getstring(tcb, &p, &num, count);
	    if (status != OK) {
		x25_switch_tcp_slam(t, lci, X25_DIAG_NO_INFO, TRUE);
		return;
	    }
	    bcopy(p, transp, num);
	    transp += num;
	    count -= num;
	} while (count > 0);

	/*
	 * sanity check of packet header version and packet size
	 */
	if (trans.version != X25_SWITCH_VERSION)
	    x25_switch_tcp_slam(t, lci, X25_DIAG_NO_INFO, FALSE);
	else if (trans.length < X25HEADERBYTES)
	    x25_switch_tcp_slam(t, lci, X25_DIAG_PACKET_TOO_SHORT, FALSE);
	else if (trans.length > (X25MAXDATA + X25EHEADERBYTES))
	    x25_switch_tcp_slam(t, lci, X25_DIAG_PACKET_TOO_LONG, FALSE);
	else
	    reg_invoke_ip_tcpdriver_update(t, x25_switch_tcp_read,
					   trans.length, lci);
    } else {
	reg_invoke_ip_tcpdriver_update(t, NULL, sizeof(transtype), lci);
    }
}

/*
 * x25_switch_tcp_read
 * Perform TCP read for X.25 packet and process it.
 */
void x25_switch_tcp_read (
    tcp_encaps *t,
    tcbtype *tcb,
    int count,
    void *cookie)
{
    int num, status, len, gfi;
    uchar *p;
    paktype *pak;
    register leveltype level;
    lcitype *lci;

    lci = (lcitype *)cookie;

    num = tcp_unread(tcb);

    if (num < count) {
	reg_invoke_ip_tcpdriver_update(t, NULL, count, lci);
	return;
    }

    /*
     *	get a buffer for our packet
     */
    if ((pak = getbuffer(count)) == NULL) {
	x25_switch_tcp_slam(t, lci, X25_DIAG_NO_INFO, TRUE);
	return;
    }

    pak->linktype = LINK_X25TCP;

    /*
     *	get and validate the General Format Identifier
     */

    gfi = tcp_getbyte(tcb, &status);

    if (gfi == -1) {
	reg_invoke_ip_tcpdriver_update(t, NULL, count, lci);
	datagram_done(pak);
	return;
    }

    if (status != OK) {
	datagram_done(pak);
	x25_switch_tcp_slam(t, lci, X25_DIAG_NO_INFO, TRUE);
	return;
    }

    if (((gfi & 0x30) >> 4) == X25_MODULO_8)
	pak->datagramstart = pak->network_start - X25HEADERBYTES;
    else if (((gfi & 0x30) >> 4) == X25_MODULO_128)
	pak->datagramstart = pak->network_start - X25EHEADERBYTES;
    else {
	datagram_done(pak);
	x25_switch_tcp_slam(t, lci, X25_DIAG_INVALID_GFI, FALSE);
	return;
    }

    /*
     *	get the packet data into our buffer
     */

    *pak->datagramstart++ = gfi;
    pak->datagramsize = count;
    count--;

    do {
	status = tcp_getstring(tcb, &p, &len, count);
	if (status != OK) {
	    datagram_done(pak);
	    x25_switch_tcp_slam(t, lci, X25_DIAG_NO_INFO, TRUE);
	    return;
	}
	bcopy(p, pak->datagramstart, len);
	pak->datagramstart += len;
	count -= len;
    } while (count > 0);

    if (((gfi & 0x30) >> 4) == X25_MODULO_8)
	pak->datagramstart = pak->network_start - X25HEADERBYTES;
    else
	pak->datagramstart = pak->network_start - X25EHEADERBYTES;

    reg_invoke_ip_tcpdriver_update(t, x25_switch_tcp_readtrans,
				   sizeof(transtype), lci);
    pak->x25_flags = 0;
    level = raise_interrupt_level(NETS_DISABLE);
    x25_switch_packet(t, pak, lci);
    reset_interrupt_level(level);
}

/*
 *  x25_switch_tcp_close
 *  Handle connection when X.25 closes it.
 */
void x25_switch_tcp_close (
    lcitype *lci,
    tcp_encaps *t,
    paktype *pak)
{
    transtype *trans;
    int status;

    /*
     *  we may or may not have an LCI associated with a connection
     */
    if (lci) {
	t = lci->lci_swdata;
	lci->lci_swdata = NULL;
    }

    /*
     *  a final message may be sent before closing a connection
     */
    if (pak) {
	pak->datagramstart -= sizeof(transtype);
	trans = (transtype *)pak->datagramstart;
	PUTSHORT(&trans->version, X25_SWITCH_VERSION);
	PUTSHORT(&trans->length, pak->datagramsize);
	pak->datagramsize += sizeof(transtype);

	status = reg_invoke_ip_tcpdriver_send(t, pak, FALSE);
	if (TCPDSEND_ERROR(status))
	    datagram_done(pak);
    }

    reg_invoke_ip_tcpdriver_update(t, x25_switch_tcp_read_flush, 1, lci);
    reg_invoke_ip_tcpdriver_close(t);

    /*
     *  if we have a remote tunneled PVC, we avoid a connection collision
     *  by allowing an incoming connection to occur while making an outgoing
     *  connection and simply keeping both connections; if this takes place
     *  we'll have two connections associated with the LCI and will need to
     *  close the second connection also
     */
    if (lci && lci->lci_pvc_swdata) {
	t = lci->lci_pvc_swdata;
	reg_invoke_ip_tcpdriver_update(t, x25_switch_tcp_read_flush, 1, lci);
	reg_invoke_ip_tcpdriver_close(t);
	lci->lci_pvc_swdata = NULL;
    }
}

/*
 * x25_switch_tcp_netclose
 * Perform X.25 stuff when remote close happens.
 */
void x25_switch_tcp_netclose (tcp_encaps *t, void *cookie)
{
    lcitype *lci;

    lci = (lcitype *)cookie;

    if (lci == NULL)
	return;

    /*
     *  the other side unexpectedly closed our connection
     */
    reg_invoke_ip_tcpdriver_update(lci->lci_swdata,
				   x25_switch_tcp_read_flush, 1, lci);
    reg_invoke_ip_tcpdriver_close(lci->lci_swdata);
    lci->lci_swdata = NULL;

    if (IS_LCI_PVC(lci->lci_idb->hwptr, lci->lci_lci)) {
	if (lci->lci_pvc_swdata) {
	    reg_invoke_ip_tcpdriver_update(lci->lci_pvc_swdata,
					   x25_switch_tcp_read_flush, 1, lci);
	    reg_invoke_ip_tcpdriver_close(lci->lci_pvc_swdata);
	    lci->lci_pvc_swdata = NULL;
	}

	if (lci->lci_pvc_status == X25_PVC_TCP_CONNECTED ||
	    lci->lci_pvc_status == X25_PVC_UP)
	    lci->lci_pvc_status = X25_PVC_TCP_SHUT;

	/* no need to start the timer--we're entering state D2/D3 */
	x25_sendcause(lci->lci_idb->hwptr, lci, X25_RESET,
	    X25_RESET_NETWORK_OUT_OF_ORDER, X25_DIAG_LINK_OUT_OF_ORDER, NULL);
    } else {
	x25_sendcause(lci->lci_idb->hwptr, lci, X25_CLEAR,
		X25_CLEAR_REMOTE_PROC_ERR, X25_DIAG_ROUTING_ERROR, NULL);
    }
}

/*
 * x25_switch_tcp_send
 * Send an X.25 packet down a TCP pipe.
 * First add transaction header.
 */
void x25_switch_tcp_send (lcitype *lci, paktype *pak)
{
    transtype *t;
    int status;

    pak->datagramstart -= sizeof(transtype);
    t = (transtype *)pak->datagramstart;
    PUTSHORT(&t->version, X25_SWITCH_VERSION);
    PUTSHORT(&t->length, pak->datagramsize);
    pak->datagramsize += sizeof(transtype);

    status = reg_invoke_ip_tcpdriver_send(lci->lci_swdata, pak, FALSE);
    if (TCPDSEND_ERROR(status)) {
	x25_switch_tcp_slam(lci->lci_swdata, lci, X25_DIAG_NO_INFO, TRUE);
	datagram_done(pak);
    }
}

/*
 * x25_switch_tcp_nvparms
 * Do NV-generation for the TCP parameters in a route entry,
 * understanding what the defaults are.  A zero value for one
 * of the parameters is also taken for default
 */

void x25_switch_tcp_nvparms (x25routeentrytype *pt)
{
    nv_add(pt->tcp_keepalive_period &&
	   pt->tcp_keepalive_period != MAXALIVETICKS,
	   " xot-keepalive-period %d", pt->tcp_keepalive_period / ONESEC);
    nv_add(pt->tcp_keepalive_tries &&
	   pt->tcp_keepalive_tries != MAXALIVETRIES,
	   " xot-keepalive-tries %d", pt->tcp_keepalive_tries);
}

/*
 * x25_switch_tcp_setparms
 * A zero value means "don't change".
 */

void x25_switch_tcp_setparms (tcp_encaps *t, ulong maxaliveticks,
			      int maxalivetries)
{
    if (maxaliveticks)
	reg_invoke_ip_tcpdriver_keepalive(t, TCPD_KP_INTERVAL, maxaliveticks);
    if (maxalivetries)
	reg_invoke_ip_tcpdriver_keepalive(t, TCPD_KP_ATTEMPTS, maxalivetries);
}
