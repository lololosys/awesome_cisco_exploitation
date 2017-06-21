/* $Id: ttcp.c,v 3.10.4.5 1996/07/06 05:57:14 thille Exp $
 * $Source: /release/112/cvs/Xsys/tcp/ttcp.c,v $
 *------------------------------------------------------------------
 * ttcp.c -- TCP performance client/server
 *
 * June 1993, Andy Heffernan
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ttcp.c,v $
 * Revision 3.10.4.5  1996/07/06  05:57:14  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.10.4.4  1996/07/03  20:23:32  ahh
 * CSCdi62037:  TTCP: should allow display of connection info at end
 * Branch: California_branch
 *
 * Revision 3.10.4.3  1996/05/21  10:06:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.4.2  1996/05/17  21:26:22  ahh
 * CSCdi57857:  TCP: ttcp needs to estimate throughput
 * Estimate kB/sec.
 * Branch: California_branch
 *
 * Revision 3.10.4.1  1996/03/18  22:14:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/16  07:41:34  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.2  1996/03/07  10:53:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  21:21:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/10  07:43:34  ahh
 * CSCdi50817:  TCP: add ttcp to some images
 * (Patch 1 of 2)
 *
 * Revision 3.9  1996/03/02  03:03:32  ahh
 * CSCdi50550:  TCP: ttcp can leak memory
 *
 * Revision 3.8  1996/02/08  18:02:17  ahh
 * CSCdi48380:  TCP: TCP Driver close callback needs more context
 * Have some t with your cookie.
 *
 * Revision 3.7  1996/01/24  20:41:39  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.6  1996/01/22  19:11:45  jenny
 * CSCdi46967:  crash in tcp_putsetup
 * Make sure checking for blocking condition in both ESTAB and CLOSEWAIT
 * states. Allow writing to TCP in CLOSEWAIT state as well.
 *
 * Revision 3.5  1996/01/16  03:38:11  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.4  1995/12/01  19:38:55  ahh
 * CSCdi44831:  ttcp does not compile any more
 * Resolve symbol clash.
 *
 * Revision 3.3  1995/11/17  18:49:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:44:26  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:36:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:32:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/28  21:21:22  ahh
 * CSCdi37865:  TCP: ttcp can crash using the TCP Driver
 * Allocate the context rather than using a local.
 *
 * Revision 2.2  1995/06/20  07:19:31  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:06:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "packet.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "tcp.h"
#include "ttysrv.h"
#include "config.h"
#include "../ip/ip_registry.h"
#include "tcpdriver.h"
#include "../ui/service.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 * Yes, this is ugly.  Yes, this was ported.  No, that's not an excuse.
 */

#define DEF_BUFLEN	(8 * 1024)
#define DEF_NBUF	(2 * 1024)
#define DEF_BUFOFFSET	0
#define DEF_BUFALIGN	(16 * 1024)
#define DEF_PORT	5001
#define DEF_SINKMODE	TRUE
#define DEF_NODELAY	FALSE
#define DEF_B_FLAG	FALSE
#define DEF_TOUCHDATA	FALSE
#define DEF_RCVWNDSIZE	0
#define DEF_USE_DRIVER	FALSE
#define DEF_OQMAX	100
#define DEF_SHOW_TCP	FALSE

/* Prototypes for internal functions */
static void ttcp_receiver_read_data (tcp_encaps *, tcbtype *, int, void *);

enum { TTCP_INVALID = -1, TTCP_RECEIVE = 0, TTCP_TRANSMIT = 1 };

struct ttcp_context {
    ulong buflen;		/* length of buffer */
    ulong nbuf;			/* number of buffers to send in sinkmode */

    ulong bufoffset;		/* align buffer to this */
    ulong bufalign;		/* modulo this */

    ulong port;			/* TCP port number */
    tinybool sinkmode;		/* 0=normal I/O, !0=sink/source mode */
    tinybool nodelay;		/* set TCP_NODELAY socket option */
    tinybool b_flag;			/* use ttcp_mread() */
    tinybool touchdata;		/* access data after reading */
    ulong rcvwndsize;		/* socket buffer size to use */

    ulong nbytes;		/* bytes on net */
    unsigned long numCalls;	/* # of I/O system calls */

    tinybool use_tcpdriver;
    tinybool show_tcp;
    ulong outputqmax;
    sys_deltatime sleeptotal;
    ulong sleeps;
    ulong packets;
    enum { READ_TRAD = 0, READ_NEW = 1, READ_SERIOUSLY = 2 } readmode;

    enum { RS_INIT, RS_HEADER, RS_DATA } td_reader_state;

    tcbtype *ttcp_rcv_tcbbase, *ttcp_rcv_tcb;

    sys_timestamp start_time;
    char *buffer;		/* read or write buffer */
};


/*
 * Simple application packet header for TCP Driver interface
 */

struct app_header {
    int packet_size;
};

/* TCP driver callbacks */

static
void ttcp_receiver_readf (tcp_encaps *handle, tcbtype *tcb, int inthresh,
			  void *cookie)
{
    struct ttcp_context *c = cookie;
    uchar *dummy;
    int bytes;

    if (tcp_getstring(tcb, &dummy, &bytes, ~0) == OK)
	c->nbytes += bytes;
    c->numCalls++;

    return;
}

static
void ttcp_receiver_init (struct ttcp_context *c)
{
    c->td_reader_state = RS_INIT;
}

static
void ttcp_receiver_new_readf (tcp_encaps *handle, tcbtype *tcb, int inthresh,
			      void *cookie)
{
    struct ttcp_context *c = cookie;
    static uchar *target = NULL;
    static ulong sizeleft = 0;
    static struct app_header hdr;
    static paktype *pak;
    uchar *junk;
    int bytes, numbytes;
    int error;

    if (c->td_reader_state == RS_INIT) {
	/* Start reading a header */
	c->td_reader_state = RS_HEADER;
	target = (uchar *) &hdr;
	sizeleft = sizeof(struct app_header);
    }

    while (1) {
	error = tcp_getstring(tcb, &junk, &bytes, ~0);
	c->numCalls++;
	if (error == NO_DATA) {
	    break;
	} else if (error != OK) {
	    buginf("\nreadf: error from tcp_getstring");
	    /* XXX abort */
	    return;
	}

	while (bytes) {
	    numbytes = min(bytes, sizeleft);

	    /*
	     * Copy the data if a target has been set up.
	     */
	    if (target) {
		bcopy(junk, target, numbytes);
		target += numbytes;
	    }
	    junk += numbytes;
	    bytes -= numbytes;
	    sizeleft -= numbytes;
	    if (c->td_reader_state == RS_DATA)
		c->nbytes += numbytes;

	    /*
	     * If the target is filled up, change state.
	     */
	    if (sizeleft == 0) {
		switch (c->td_reader_state) {
		case RS_INIT:
		    buginf("\ncan't happen");
		    break;

		case RS_HEADER:
		    c->packets++;
		    c->td_reader_state = RS_DATA;
		    pak = getbuffer(hdr.packet_size);
		    if (pak == NULL) {
			buginf("\nno memory for packet");
			/* No target, just drain until the next header. */
			target = NULL;
		    } else {
			target = pak->datagramstart;
		    }
		    sizeleft = hdr.packet_size;
		    break;

		case RS_DATA:
		    if (pak) {
			datagram_done(pak);
			pak = NULL;
		    }
		    c->td_reader_state = RS_HEADER;
		    target = (uchar *) &hdr;
		    sizeleft = sizeof(struct app_header);
		    break;
		}
	    }
	}
    }
}

static
void ttcp_receiver_read_header (tcp_encaps *handle, tcbtype *tcb, int inthresh,
				void *cookie)
{
    struct ttcp_context *c = cookie;
    uchar *junk, *dest;
    int bytes;
    struct app_header hdr;
    int error;

    c->packets++;

    dest = (char *) &hdr;
    do {
	error = tcp_getstring(tcb, &junk, &bytes, inthresh);
	c->numCalls++;
	if (error != OK) {
	    buginf("\nread_header: error from tcp_getstring");
	    /* XXX abort */
	    return;
	}

	/*
	 * Normally, an app would have a someplace to store the header
	 * and would now bcopy 'bytes' number of bytes from 'junk'
	 * into this place.
	 */

        bcopy(junk, dest, bytes);
	dest += bytes;
	inthresh -= bytes;
    } while (inthresh > 0);

    reg_invoke_ip_tcpdriver_update(handle, ttcp_receiver_read_data,
				   hdr.packet_size, cookie);
    return;
}

static
void ttcp_receiver_read_data (tcp_encaps *handle, tcbtype *tcb, int inthresh,
			      void *cookie)
{
    struct ttcp_context *c = cookie;
    uchar *junk, *p;
    int bytes;
    paktype *pak;
    int error;

    pak = getbuffer(inthresh);
    if (pak == NULL) {
	buginf("\nno memory for packet");
	do {
	    error = tcp_getstring(tcb, &junk, &bytes, inthresh);
	    c->numCalls++;
	    if (error != OK) {
		buginf("\nread_data: error from tcp_getstring");
		/* XXX abort */
		return;
	    }
	    inthresh -= bytes;
	} while (inthresh > 0);
    } else {
	p = pak->datagramstart;
	do {
	    error = tcp_getstring(tcb, &junk, &bytes, inthresh);
	    c->numCalls++;
	    if (error != OK) {
		buginf("\nread_data: error from tcp_getstring");
		/* XXX abort */
		return;
	    }
	    c->nbytes += bytes;

	    /*
	     * Normally, an app would have allocated a packet structure,
	     * and would now bcopy 'bytes' number of bytes from 'junk'
	     * into the new packet.
	     */
	    bcopy(junk, p, bytes);
	    p += bytes;
	    inthresh -= bytes;
	} while (inthresh > 0);
	datagram_done(pak);
    }

    reg_invoke_ip_tcpdriver_update(handle, ttcp_receiver_read_header,
				   sizeof(struct app_header), cookie);

    return;
}

static
void ttcp_receiver_closef (tcp_encaps *t, void *cookie)
{
    struct ttcp_context *c = cookie;
    ulong time_elapsed;
    sys_deltatime avg_sleep;

    time_elapsed = ELAPSED_TIME(c->start_time);
    if (time_elapsed == 0)
	time_elapsed = 1;

    printf("\nttcp-r: %u bytes in %d ms (%d.%03d real seconds) (~%d kB/sec) +++",
	   c->nbytes, time_elapsed, time_elapsed / 1000, time_elapsed % 1000,
	   ((c->nbytes / time_elapsed) * 1000) / 1024);
    printf("\nttcp-r: %d I/O calls", c->numCalls);
    avg_sleep = c->sleeps ? c->sleeptotal / c->sleeps : 0;
    printf("\nttcp-r: %d sleeps (%Td ms total) (%Td ms average)",
	   c->sleeps, c->sleeptotal, avg_sleep);
    printf("\nttcp-r: %d packets", c->packets);
    flush();

    free(c->buffer);		/* This is the read buffer */

    /* Kill the listening handle */
    tcp_close(c->ttcp_rcv_tcbbase);

    free(c);			/* Delete the context now. */
}

static
void ttcp_sender_closef (tcp_encaps *t, void *cookie)
{
    struct ttcp_context *c = cookie;

    free(c->buffer);		/* This is the write buffer */
    free(c);
}

/*
 * ttcp_getpak
 */
static paktype *ttcp_getpak (int paklen)
{
    paktype *pak;

    pak = getbuffer(paklen);
    if (pak == NULL) {
	printf("\nno memory for packet in ttcp_xmit");
	return(NULL);
    }
    pak->datagramsize = paklen + sizeof(struct app_header);
    return(pak);
}


/*
 * Main transmit routine and its little helpers.
 */

#define iswhite(ch)	((ch) == ' ' || (ch) == '\t' || (ch) ==  '\n' || \
			 (ch) == '\r')
#define ttcp_isprint(ch)	(iswhite(ch) || ((ch) >= ' ' && (ch) <= '~'))

static
void pattern (char *cp, int cnt)
{
    char c;

    c = 0;
    while (cnt-- > 0) {
	while (!ttcp_isprint((c & 0x7f)))
	    c++;
	*cp++ = (c++ & 0x7f);
    }
}

static
boolean ttcp_write_td_BLOCK (tcp_encaps *h)
{
    if (h->state != TCPD_ALIVE || h->tcb->state != ESTAB)
	return(FALSE);
    if (h->tcb->tty && (psiset(h->tcb->tty)))
	return(FALSE);
    return(reg_invoke_ip_tcpdriver_oqlen(h) ==
	   reg_invoke_ip_tcpdriver_get_oqmax(h));
}

static
int ttcp_write_td (tcp_encaps *h, paktype *pak, struct ttcp_context *c)
{
    int error;

    /*
     * Could use tcpdriver_send here as well, but why
     * wait for a reschedule to get the data out?
     */
    error = reg_invoke_ip_tcpdriver_send_now(h, pak, FALSE);
    if (error == TCPDSEND_QFULL) {
	sys_timestamp then;

	GET_TIMESTAMP(then);
	edisms((blockproc *) ttcp_write_td_BLOCK, (ulong) h);
	c->sleeptotal += ELAPSED_TIME64(then);
	c->sleeps++;
	error = reg_invoke_ip_tcpdriver_send_now(h, pak, FALSE);
	if (error == TCPDSEND_QFULL)
	    printf("\nAck -- still blocked, error %d!!", error);
	else if (error == TCPDSEND_CLOSED)
	    printf("\nConnection died while retransmitting");
    } else if (error == TCPDSEND_CLOSED)
	printf("\nConnection closed?");

    return(TCPDSEND_OK(error) ? 0 : -1);
}

/*
 *			N W R I T E
 */
static
int ttcp_Nwrite (tcbtype *tcb, void *buf, unsigned int count,
		 struct ttcp_context *c)
{
    int cnt;
    int error;

    cnt = tcp_write(tcb, buf, count, &error);
    if (cnt == -1)
	printf("\n%%Write failed: %s\n", tcperror2string(error));
    c->numCalls++;

    return(cnt);
}

static
int ttcp_Nwrite_td (tcp_encaps *h, paktype *pak, struct ttcp_context *c)
{
    int error;
    paktype *newpak;
    int size;
    struct app_header *hdr;

    /*
     * Get a new packet.
     */
    size = pak->datagramsize + sizeof(struct app_header);
    newpak = getbuffer(size);
    if (newpak == NULL) {
	printf("\nno memory for packet copy");
	return(-1);
    }
    newpak->datagramsize = size;

    /*
     * Put the header in place and copy the contents.
     */
    hdr = (struct app_header *) newpak->datagramstart;
    hdr->packet_size = pak->datagramsize;
    bcopy(pak->datagramstart, newpak->datagramstart+sizeof(struct app_header),
	  pak->datagramsize);

    /*
     * Throw away the old packet and send the new one.
     */
    datagram_done(pak);
    error = ttcp_write_td(h, newpak, c);
    c->numCalls++;
    return(error);
}

static
int ttyread (char *s, int n)
{
    int ch;
    int count;

    count = 0;
    ch = 0;
    while (n-- && (ch = getcwait(stdio)) != '\r' && ch != 04 && ch != -1) {
	*s++ = ch;
	count++;
    }
    if (ch == '\r') {
	*s++ = ch;
	*s = '\n';
	count += 2;
    } else if (ch == -1)
	return(-1);

    return(count);
}

/*
 * ttcp_xmit
 *	Open a connection, send bytes
 */

static
int ttcp_xmit (addrtype *addr, struct ttcp_context *c)
{
    tcbtype *tcb = NULL;
    tcp_encaps *handle = NULL;
    int error;
    char *realbuf, *buf;
    int bufsize;
    int packetlen = 0;
    ulong time_elapsed;

    /*
     * If using TCP driver add in some bytes for a fake application
     * packet header.
     */
    if (c->use_tcpdriver)
	packetlen = c->buflen;

    /* Allocate a buffer and align if requested. */
    bufsize = c->buflen + c->bufalign;

    if ((realbuf = malloc(bufsize)) == NULL) {
	printf(nomemory);
	return(-1);
    }

    if (c->bufalign)
	buf = realbuf + (c->bufalign - ((int) realbuf % c->bufalign) +
			 c->bufoffset) % c->bufalign;
    else
	buf = realbuf;

    c->buffer = realbuf;

    printf("\nttcp-t: buflen=%u, nbuf=%u, align=%d/%d, port=%d",
	   c->use_tcpdriver ? packetlen : c->buflen, c->nbuf,
	   c->bufalign, c->bufoffset, c->port);
    if (c->rcvwndsize)
	printf(", rcvwndsize=%d", c->rcvwndsize);
    printf("  tcp  -> %s", address_string(addr));
    flush();

    if (!c->use_tcpdriver) {
	tcb = tcp_create(NULL, &error, NULL);
	if (tcb == NULL) {
	    printf("\n%%Open failed: %s\n", tcperror2string(error));
	    free(realbuf);
	    return(-1);
	}

	error = tcp_connect(tcb, addr, c->port);
	if (error != OK) {
	    printf("\n%%Connect failed: %s\n", tcperror2string(error));
	    tcp_close(tcb);
	    free(realbuf);
	    return(-1);
	}
	printf("\nttcp-t: connect (mss %d, sndwnd %d, rcvwnd %d)",
	       tcb->maxsegmentsize, tcb->sndwnd, tcb->rcvwnd);

	/* Disable Nagle if requested. */
	if (c->nodelay) {
	    int yes = 1;
	    tcp_set_property(tcb, TCP_NO_DELAY, &yes, sizeof(yes));
	}
    } else {
	tcp_encaps tmp;
	tcbtype *tcb;

	memset(&tmp, 0, sizeof(tcp_encaps));
	tmp.readf = NULL;
	tmp.closef = ttcp_sender_closef;
	tmp.flushf = NULL;
        tmp.flow_ctrl = NULL;
	tmp.inthresh = 1;
	tmp.cookie = c;
	handle = reg_invoke_ip_tcpdriver_active_open(addr->ip_addr, 0, c->port,
						     c->rcvwndsize ?
						     c->rcvwndsize :
						     tcp_defrcvwnd,
						     &tmp, &error);
	if (handle == NULL) {
	    printf("\n%%Open failed: %s\n", tcperror2string(error));
	    free(realbuf);
	    return(-1);
	}

	reg_invoke_ip_tcpdriver_set_oqmax(handle, c->outputqmax);
	reg_invoke_ip_tcpdriver_change_timeout(handle, TRUE);

	tcb = reg_invoke_ip_tcpdriver_tcb(handle);
	printf("\nttcp-t: connect (mss %d, sndwnd %d, rcvwnd %d)",
	       tcb->maxsegmentsize, tcb->sndwnd, tcb->rcvwnd);
	/* Disable Nagle if requested. */
	if (c->nodelay) {
	    int yes = 1;
	    tcp_set_property(tcb, TCP_NO_DELAY, &yes, sizeof(yes));
	}
    }
    flush();

    psion(stdio);

    GET_TIMESTAMP(c->start_time);

    if (!c->use_tcpdriver) {
	if (c->sinkmode) {
	    pattern(buf, c->buflen);
	    while (c->nbuf-- &&
		   ttcp_Nwrite(tcb, buf, c->buflen, c) == c->buflen &&
		   psipending() == FALSE)
		c->nbytes += c->buflen;
	} else {
	    int cnt;

	    while ((cnt = ttyread(buf, c->buflen)) > 0 &&
		   ttcp_Nwrite(tcb, buf, cnt, c) == cnt)
		c->nbytes += cnt;
	}
    } else {
	paktype *pak;

	if (c->sinkmode) {
	    while (c->nbuf-- && psipending() == FALSE) {
		pak = ttcp_getpak(packetlen);
		if (pak == NULL)
		    break;
		pattern(pak->datagramstart, packetlen);
		if (ttcp_Nwrite_td(handle, pak, c) != 0)
		    break;
		c->nbytes += packetlen;
	    }
	} else {
	    int cnt;

	    while (c->nbuf-- && psipending() == FALSE) {
		pak = ttcp_getpak(packetlen);
		if (pak == NULL)
		    break;
		cnt = ttyread(pak->datagramstart, packetlen);
		if (cnt <= 0)
		    break;
		if (ttcp_Nwrite_td(handle, pak, c) != 0)
		    break;
		c->nbytes += cnt;
	    }
	}
    }
    psioff(stdio);

    time_elapsed = ELAPSED_TIME(c->start_time);

    if (time_elapsed == 0)
	time_elapsed = 1;

    {
	sys_deltatime avg_sleep;
	tcbtype *tcb;
	if (!c->use_tcpdriver)
	    tcb = tcb;
	else
	    tcb = reg_invoke_ip_tcpdriver_tcb(handle);

	printf("\nttcp-t: %d bytes in %d ms (%d.%03d real seconds) (~%d kB/sec) +++",
	       c->nbytes, time_elapsed, time_elapsed / 1000,
	       time_elapsed % 1000,
	       ((c->nbytes / time_elapsed) * 1000) / 1024);
	printf("\nttcp-t: %d I/O calls", c->numCalls);
	avg_sleep = c->sleeps ? c->sleeptotal / c->sleeps : 0;
	printf("\nttcp-t: %d sleeps (%Td ms total) (%Td ms average)",
	       c->sleeps, c->sleeptotal, avg_sleep);	       
    }

    if (tcb) {
	if (c->show_tcp)
	    tcp_status(tcb);
	tcp_close(tcb);
    }
    if (handle)
	reg_invoke_ip_tcpdriver_close(handle);
    if (!c->use_tcpdriver)
	free(realbuf);
    /* When using TCP Driver, the close callback will free the buffer. */

    return(0);
}

/*
 * Main receive routine and its little helpers.
 */

/*
 *			M R E A D
 *
 * This function performs the function of a read(II) but will
 * call read(II) multiple times in order to get the requested
 * number of characters.  This can be necessary because
 * network connections don't deliver data with the same
 * grouping as it is written with.  Written by Robert S. Miles, BRL.
 */
static
int ttcp_mread (tcbtype *tcb, char *bufp, unsigned int n,
		struct ttcp_context *c)
{
    unsigned count = 0;
    int nread, error;

    do {
	nread = tcp_read(tcb, bufp, n-count, &error);
	c->numCalls++;
	if (nread < 0) {
	    printf("\n%%Read failed: %s\n", tcperror2string(error));
	    return(-1);
	}
	if (nread == 0)
	    return((int) count);
	count += (unsigned) nread;
	bufp += nread;
    } while (count < n);

    return((int) count);
}

/*
 *			N R E A D
 */
static
int ttcp_Nread (tcbtype *tcb, void *buf, unsigned int count,
		struct ttcp_context *c)
{
    int cnt;
    int error;

    if (c->b_flag)
	cnt = ttcp_mread(tcb, buf, count, c);	/* fill buf */
    else {
	cnt = tcp_read(tcb, c->sinkmode ? NULL : buf, count, &error);
	if (cnt == -1)
	    printf("\n%%Read failed: %s\n", tcperror2string(error));
	c->numCalls++;
    }
    if (c->touchdata && cnt > 0) {
	int c = cnt, sum;
	char *b = buf;

	while (c--)
	    sum += *b++;
    }

    return(cnt);
}

static
int putbytes (tt_soc *tty, char *s, int n)
{
    int len = n;

    while (n--) {
	putc(tty, *s);
	s++;
    }

    return(len);
}

/*
 * ttcp_rcv
 *	Wait for a connection, receive bytes.
 */

static
int ttcp_rcv (struct ttcp_context *c)
{
    tcbtype *tcb = NULL;
    tcp_encaps *handle = NULL;
    char *realbuf, *buf;
    int error;
    ulong time_elapsed;
    sys_deltatime avg_sleep;

    c->ttcp_rcv_tcbbase = c->ttcp_rcv_tcb = NULL;

    /* Allocate a buffer and align if requested. */
    if ((realbuf = malloc(c->buflen + c->bufalign)) == NULL) {
	printf(nomemory);
	return(-1);
    }

    if (c->bufalign)
	buf = realbuf + (c->bufalign - ((int) realbuf % c->bufalign) +
			 c->bufoffset) % c->bufalign;
    else
	buf = realbuf;

    c->buffer = realbuf;

    printf("\nttcp-r: buflen=%u, nbuf=%u, align=%d/%d, port=%d",
	   c->buflen, c->nbuf, c->bufalign, c->bufoffset, c->port);
    if (c->rcvwndsize)
	printf(", rcvwndsize=%d", c->rcvwndsize);
    printf("  tcp");
    flush();

    if ((c->ttcp_rcv_tcbbase = tcp_create(stdio, &error, NULL)) == NULL) {
	printf("\n%%Open failed: %s\n", tcperror2string(error));
	free(realbuf);
	return(-1);
    }

    error = tcp_bind(c->ttcp_rcv_tcbbase, NULL, c->port);
    if (error != OK) {
	printf("\n%%Bind failed: %s\n", tcperror2string(error));
	tcp_close(c->ttcp_rcv_tcbbase);
	free(realbuf);
	return(-1);
    }

    if (c->rcvwndsize)
	tcp_set_property(c->ttcp_rcv_tcbbase, TCP_WINDOW_SIZE, &c->rcvwndsize,
			 sizeof(c->rcvwndsize));

    error = tcp_listen(c->ttcp_rcv_tcbbase, 1, NULL);
    if (error != OK) {
	printf("\n%%Listen failed: %s\n", tcperror2string(error));
	tcp_close(c->ttcp_rcv_tcbbase);
	free(realbuf);
	return(-1);
    }

    tcb = c->ttcp_rcv_tcb = tcp_accept(c->ttcp_rcv_tcbbase, &error);
    if (tcb == NULL) {
	printf("\n%%Accept failed: %s\n", tcperror2string(error));
	tcp_close(c->ttcp_rcv_tcbbase);
	free(realbuf);
	return(-1);
    }

    printf("\nttcp-r: accept from %s (mss %d, sndwnd %d, rcvwnd %d)",
	   address_string(&tcb->foreignhost), tcb->maxsegmentsize,
	   tcb->sndwnd, tcb->rcvwnd);
    flush();

    GET_TIMESTAMP(c->start_time);

    if (c->use_tcpdriver) {
	tcp_encaps tmp;

	memset(&tmp, 0, sizeof(tcp_encaps));
	ttcp_receiver_init(c);
	if (c->readmode == READ_TRAD) {
	    tmp.readf = ttcp_receiver_read_header;
	    tmp.inthresh = sizeof(struct app_header);
	} else if (c->readmode == READ_NEW) {
	    tmp.readf = ttcp_receiver_new_readf;
	    tmp.inthresh = 1;
	} else {
	    tmp.readf = ttcp_receiver_readf;
	    tmp.inthresh = 1;
	}
	tmp.closef = ttcp_receiver_closef;
	tmp.flushf = NULL;
        tmp.flow_ctrl = NULL;
	tmp.cookie = c;
	handle = reg_invoke_ip_tcpdriver_passive_open(tcb,
						      c->rcvwndsize ?
						      c->rcvwndsize :
						      TCP_WINDOW_SIZE,
						      &tmp);
	if (handle == NULL) {
	    printf("\n%%Open failed: %s\n", "but don't know why");
	    free(realbuf);
	    return(-1);
	}

	reg_invoke_ip_tcpdriver_change_timeout(handle, TRUE);
	/* Rest of execution will be handled via callbacks */
	return(0);
    }

    if (c->sinkmode) {
	int cnt;

	while ((cnt = ttcp_Nread(tcb, buf, c->buflen, c)) > 0)
	    c->nbytes += cnt;
    } else {
	int cnt;

	while ((cnt = ttcp_Nread(tcb, buf, c->buflen, c)) > 0 &&
	       putbytes(stdio, buf, cnt) == cnt)
	    c->nbytes += cnt;
    }

    time_elapsed = ELAPSED_TIME(c->start_time);
    if (time_elapsed == 0)
	time_elapsed = 1;

    printf("\nttcp-r: %u bytes in %d ms (%d.%03d real seconds) (~%d kB/sec) +++",
	   c->nbytes, time_elapsed, time_elapsed / 1000, time_elapsed % 1000,
	   ((c->nbytes / time_elapsed) * 1000) / 1024);
    printf("\nttcp-r: %d I/O calls", c->numCalls);
    avg_sleep = c->sleeps ? c->sleeptotal / c->sleeps : 0;
    printf("\nttcp-r: %d sleeps (%Td ms total) (%Td ms average)",
	   c->sleeps, c->sleeptotal, avg_sleep);

    if (tcb) {
	if (c->show_tcp)
	    tcp_status(tcb);
	tcp_close(tcb);
    }
    if (c->ttcp_rcv_tcbbase)
	tcp_close(c->ttcp_rcv_tcbbase);
    if (handle)
	reg_invoke_ip_tcpdriver_close(handle);
    free(realbuf);

    return(0);
}

/*
 * ttcp getparms
 *	Do dialogue with user for parameters.
 */

#define TTY_BUFLEN	80

static
void ttcp_getnum (char *desc, ulong *num)
{
    char buf[TTY_BUFLEN];
    int parsed;
    int val = *num;

    do {
	printf("%s [%u]: ", desc, val);
	if (!rdtty(buf, TTY_BUFLEN))
	    return;
	if (null(buf))
	    break;
	val = parse_unsigned(buf, &parsed);
    } while (!parsed);

    *num = val;
}

static
int ttcp_getparms (addrtype *addr, struct ttcp_context *c)
{
    int mode;
    char buf[TTY_BUFLEN];

    /* Decide transmit or receive.*/
    mode = TTCP_INVALID;
    do {
	printf("transmit or receive [%s]: ", (0==0) ? "receive" : "transmit");
	if (!rdtty(buf, TTY_BUFLEN))
	    return(-1);
	if (null(buf) || strmatch(buf, "receive", FALSE))
	    mode = TTCP_RECEIVE;
	else if (strmatch(buf, "transmit", FALSE))
	    mode = TTCP_TRANSMIT;
	else
	    printf("\n%%must specify either receive or transmit\n");
    } while (mode == TTCP_INVALID);

    /* Get address to send to if transmitting. */
    if (mode == TTCP_TRANSMIT) {	/* xmit only */
	addr->type = ADDR_IP;
	addr->length = ADDRLEN_IP;
	do {
	    printf("Target IP address: ");
	    if (!rdtty(buf, TTY_BUFLEN))
		return(-1);
	    if (parse_hostname(buf, &addr->ip_addr))
		break;
	    printf("\n%% Bad IP address\n");
	} while (1);
    }

    /* Decide whether or not to use the TCP driver interface. */
    if (internal_cmd_enable) {
	printf("use tcp driver [%c]: ", c->use_tcpdriver ? 'y' : 'n');
	if (!rdtty(buf, TTY_BUFLEN))
	    return(-1);
	if (!null(buf))
	    c->use_tcpdriver = (toupper(buf[0]) == 'Y');
	if (c->use_tcpdriver) {
	    if (mode == TTCP_TRANSMIT)
		ttcp_getnum("\toutput queue", &c->outputqmax);
	    else
		ttcp_getnum("\treading mode (0=traditional, 1=new, 2=serious)",
			    (ulong *) &c->readmode);
	}
    }

    /* Get the size of memory buffer for reads and writes. */
    ttcp_getnum("buflen", &c->buflen);

    /* Get how many buffers to send if transmitting. */
    if (mode == TTCP_TRANSMIT)		/* xmit only */
	ttcp_getnum("nbuf", &c->nbuf);

    /* Get alignment of read/write buffer in memory. */
    ttcp_getnum("bufalign", &c->bufalign);

    /* Get offset from alignment at which to start the read/write buffer. */
    ttcp_getnum("bufoffset", &c->bufoffset);

    /* Get port number to listen on/connect to. */
    ttcp_getnum("port", &c->port);

    /* Decide whether or not to junk data. */
    printf("sinkmode [%c]: ", c->sinkmode ? 'y' : 'n');
    if (!rdtty(buf, TTY_BUFLEN))
	return(-1);
    if (!null(buf))
	c->sinkmode = (toupper(buf[0]) == 'Y');

    /*
     * If transmitting, decide whether or not to use Nagle (unimplemented).
     * If receiving, get receive window size.
     */
    if (mode == TTCP_TRANSMIT) {	/* xmit only */
	printf("buffering on writes [%c]: ", !c->nodelay ? 'y' : 'n');
	if (!rdtty(buf, TTY_BUFLEN))
	    return(-1);
	if (!null(buf))
	    c->nodelay = !(toupper(buf[0]) == 'Y');
    } else {		/* rcv only */
	while (1) {
	    if (c->rcvwndsize == 0)
		c->rcvwndsize = tcp_defrcvwnd;
	    ttcp_getnum("rcvwndsize", &c->rcvwndsize);
	    if (c->rcvwndsize <= MAXTCPWINDOW)
		break;
	    printf("\n%% size must be between 0 and %d\n", MAXTCPWINDOW);
	}
    }

    printf("show tcp information at end [%c]: ", c->show_tcp ? 'y' : 'n');
    if (!rdtty(buf, TTY_BUFLEN))
	return(-1);
    if (!null(buf))
	c->show_tcp = (toupper(buf[0]) == 'Y');

    return(mode);
}

/*
 * ttcp_command
 *	Invoked by Exec via the parser.
 */

void ttcp_command (parseinfo *csb)
{
    addrtype addr;
    int mode;
    struct ttcp_context *ttcp_context;

    ttcp_context = malloc(sizeof(struct ttcp_context));
    if (ttcp_context == NULL)
	return;

    ttcp_context->nbytes = ttcp_context->numCalls = 0;
    ttcp_context->buflen = DEF_BUFLEN;
    ttcp_context->nbuf = DEF_NBUF;
    ttcp_context->bufoffset = DEF_BUFOFFSET;
    ttcp_context->bufalign = DEF_BUFALIGN;
    ttcp_context->port = DEF_PORT;
    ttcp_context->sinkmode = DEF_SINKMODE;
    ttcp_context->nodelay = DEF_NODELAY;
    ttcp_context->b_flag = DEF_B_FLAG;
    ttcp_context->rcvwndsize = DEF_RCVWNDSIZE;
    ttcp_context->touchdata = DEF_TOUCHDATA;
    ttcp_context->use_tcpdriver = DEF_USE_DRIVER;
    ttcp_context->show_tcp = DEF_SHOW_TCP;
    ttcp_context->sleeptotal = ttcp_context->sleeps = 0;
    ttcp_context->outputqmax = DEF_OQMAX;
    ttcp_context->packets = 0;
    ttcp_context->readmode = READ_TRAD;

    if (GETOBJ(int, 1) == 0) {
	mode = ttcp_getparms(&addr, ttcp_context);
	if (mode == TTCP_INVALID)
	    return;
    } else {
	mode = GETOBJ(int, 1) - 1;	/* XXX Note the horrible dependency */
	address_copy(&addr, GETOBJ(paddr, 1));
    }

    if (ttcp_context->use_tcpdriver)
	reg_invoke_ip_tcpdriver_start();

    switch (mode) {
    case TTCP_RECEIVE:
	ttcp_rcv(ttcp_context);
	break;
    case TTCP_TRANSMIT:
	ttcp_xmit(&addr, ttcp_context);
	break;
    }

    if (!ttcp_context->use_tcpdriver)
	free(ttcp_context);	/* close callbacks will free context */
}

/*
 * Parse chains for TTCP exec commands
 */

#define ALTERNATE       NONE
#include "exec_ttcp.h"
LINK_POINT(ttcp_commands, ALTERNATE);
#undef	ALTERNATE

static parser_extension_request ttcp_chain_init_table[] = {
    { PARSE_ADD_EXEC_CMD, &pname(ttcp_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * ttcp_init
 * Initialize TTCP support
 */
static void ttcp_init (subsystype *dummy)
{
    parser_add_command_list(ttcp_chain_init_table, "ttcp");
}

/*
 * TTCP subsystem header
 * Note: This subsystem is not intended for (most) production images --
 * it is for performance testing purposes.
 */

#define TTCP_MAJVERSION 1
#define TTCP_MINVERSION 0
#define TTCP_EDITVERSION  1

SUBSYS_HEADER(ttcp, TTCP_MAJVERSION, TTCP_MINVERSION, TTCP_EDITVERSION,
	      ttcp_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
