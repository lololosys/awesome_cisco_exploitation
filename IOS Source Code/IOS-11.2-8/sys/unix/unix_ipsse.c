/* $Id: unix_ipsse.c,v 3.4.20.2 1996/07/06 05:57:45 thille Exp $
 * $Source: /release/112/cvs/Xsys/unix/unix_ipsse.c,v $
 *------------------------------------------------------------------
 * unix_ipsse.o -- Unix IP specific support code.
 *
 * Oct 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_ipsse.c,v $
 * Revision 3.4.20.2  1996/07/06  05:57:45  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.4.20.1  1996/03/18  22:30:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:59:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  21:27:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/05  17:59:50  ahh
 * CSCdi48168:  UNIX SSE file does not compile
 * Replace usage of NULL with '\0'.
 *
 * Revision 3.3  1995/11/17  17:55:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:46:29  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:42:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/01  06:55:09  hampton
 * Make the unix images compile again.  [CSCdi39708]
 *
 * Revision 2.2  1995/06/09 13:22:25  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:12:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "subsys.h"
#include "config.h"
#include "unix.h"
#include "../ip/ip.h"
#include "../hes/if_mci.h"
#include "../ip/ipfast.h"

#include "../parser/parser_defs_sse.h"
#include "../hes/sse.h"
#include "../hes/sse_registry.h"

extern ipcache_rntype *ipcache_add(ipaddrtype, ipaddrtype,
				   idbtype *, hwidbtype *, 
				   uchar *, int, ushort,
				   dialergrouptype *);
extern boolean ipfast_sse_add_entry (ssetype *sse, ipcache_rntype *entry);
extern void ipfast_sse_delete_entry (ssetype *sse, ipcache_rntype *entry);

/*
 * ip_read_cachefile
 * Read in a file containing output from show ip cache for priming
 * cache for SSE debugging
 */

void ip_read_cachefile (uchar* buf, uchar* end)
{
    uchar macbuf[200], *macptr, c, *tail;
    int maclen, num;
    ipaddrtype address;

    while (buf < end) {
	while (*buf == '\n') buf++;
	if ((*buf == ' ') || (*buf == '*')) {
	    /*
	     * Old format cache files.
	     */
	    while (isspace(*buf)) buf++;	/* skip whitespace */
	    while (!isspace(*buf)) buf++;	/* skip hash value */
	    while (isspace(*buf)) buf++;	/* skip whitespace */
	    tail = buf;
	    while (!isspace(*tail)) tail++;
	    *tail = '\0';
	    if (!parse_ip_address((char *)buf, &address)) {
		buginf("\nSSE: parse IP address failed");
		return;
	    }
	    buf = ++tail;
	    while (isspace(*buf)) buf++;	/* skip whitespace */
	    while (!isspace(*buf)) buf++;	/* skip interface name */
	    while (isspace(*buf)) buf++;	/* skip whitespace */
	} else {
	    /*
	     * New format cache files.
	     */
	    tail = buf;
	    while (*tail != '/') tail++;
	    *tail = '\0';
	    if (!parse_ip_address((char *)buf, &address)) {
		buginf("\nSSE: parse IP address failed");
		return;
	    }
	    buf = ++tail;
	    while (!isspace(*buf)) buf++;	/* skip bitinmask */
	    while (isspace(*buf)) buf++;	/* skip whitespace */
	    while (!isspace(*buf)) buf++;	/* skip timestamp */
	    while (isspace(*buf)) buf++;	/* skip whitespace */
	    while (!isspace(*buf)) buf++;	/* skip interface name */
	    while (isspace(*buf)) buf++;	/* skip whitespace */
	}
	c = toupper(*buf++);
	macptr = macbuf;
	maclen = 0;
	while (isxdigit(c)) {
	    num = 0;
	    if (isdigit(c))
		num = (c - '0');
	    else
		num = (c - '7');
	    c = toupper(*buf++);
	    num <<= 4;
	    if (isdigit(c))
		num += (c - '0');
	    else if (isxdigit(c)) 
		num += (c - '7');
	    c = toupper(*buf++);
	    if (c == '\n') {
		/*
		 * Look to see if this is a continuation of the mac string.
		 */
		tail = buf;
		while (*tail && (*tail != '\n')) {
		    if (*tail == '.')
			break;
		    tail++;
		}
		if (*tail && (*tail != '.')) {
		    while (isspace(*buf)) buf++; /* skip whitespace */
		    c = toupper(*buf++);
		}
	    }   
	    *macptr++ = num;
	    maclen++;
	}

	if (!ipcache_add(address, 0, SWIDBQ_HEAD, HWIDBQ_HEAD,
			 macbuf, maclen, 0, NULL)) 
	    buginf("\nSSE: unable to add entry %i to ip cache", address);
    }
}

/*
 * unix_ip_trace_sse
 * Read a trace file and load it into the cache.  Entries have syntax:
 * add <prefix>
 * delete <prefix>
 * where <prefix> = <ipaddr>/<sigbits>
 */

void unix_ip_trace_sse (parseinfo *csb)
{
    char *name;
    uchar *buf, *end, *bufptr, *tail;
    int bufsize = 700000;
    long got;
    boolean ok;
    ipaddrtype address;
    enum {SSE_ADD, SSE_DELETE} opcode;
    int bits;
    ipcache_rntype entry;
    ssetype *sse;
    mcitype *cbus;
    
    cbus = (mcitype *) cbusQ.qhead;
    if (!cbus) {
	printf("\nwhat cbus?");
	return;
    }
    sse = cbus->sseptr;
    if (!sse) {
	printf("\nwhat sse?");
	return;
    }
    
    /*
     * Really just load a file to pre-prime the fast switching cache.
     */
    buf = malloc(bufsize);
    if (!buf) {
	buginf("\nSSE: malloc failed");
	return;
    }
    name = GETOBJ(string,1);
    if (!name) {
	buginf("\nSSE: no name");
	goto done;
    }
    if (sse_debug)
	buginf("\nSSE: parsing %s", name);
    if (!unix_read_file(name, buf, &got, (long) bufsize)) {
	buginf("\nSSE: read failed");
	goto done;
    }
    end = buf + got;

    /*
     * Preload the entry.  Make up a mac string, point to our first
     * interface.
     */
    entry.idb = SWIDBQ_HEAD;
    entry.hwidb = HWIDBQ_HEAD;
    entry.length = 10;			/* random garbage mac entry */

    bufptr = buf;
    while (bufptr < end) {
	tail = bufptr;
	while(isspace(*tail)) tail++;
	while(!isspace(*tail)) tail++;
	*tail = '\0';
	if (partialmatch ("add", bufptr))
	    opcode = SSE_ADD;
	else if (partialmatch("delete", bufptr))
	    opcode = SSE_DELETE;
	else {
	    printf("\nSyntax error: %s", bufptr);
	    goto done;
	}
	bufptr = ++tail;
	while(isspace(*bufptr)) bufptr++;
	tail = bufptr;
	while(!isspace(*tail) && (*tail != '/')) tail++;
	*tail = '\0';
	if (!parse_ip_address((char *)bufptr, &address)) {
	    printf("\nCan't parse IP address: %s", bufptr);
	    goto done;
	}
	entry.prefix = address;
	bufptr = ++tail;
	bits = parse_unsigned(bufptr, &ok);
	if (!ok) {
	    printf("\nCan't parse sig bits: %s", bufptr);
	    goto done;
	}
	entry.mask = ip_bitcount2mask(bits);
	if (opcode == SSE_ADD) {
	    if (!ipfast_sse_add_entry(sse, &entry)) {
		printf("\nAdd of %i/%d failed", address, bits);
		goto done;
	    }
	} else {
	    ipfast_sse_delete_entry(sse, &entry);
	}
	tail = bufptr;
	while (*tail != '\n') tail++;
	bufptr = ++tail;
	process_suspend();
    }
 done:
    free(buf);
}

/*
 * unix_ipsse_register_thyself
 */

void unix_ipsse_register_thyself (subsystype *subsys)
{
    reg_add_sse_preload_cache(LINK_IP, ip_read_cachefile,
			      "ip_read_cachefile"); 
}

/* 
 * unix_ipsse subsystem header
 */

#define SSE_MAJVERSION 1
#define SSE_MINVERSION 0
#define SSE_EDITVERSION 1

SUBSYS_HEADER(unix_ipsse, SSE_MAJVERSION, SSE_MINVERSION, SSE_EDITVERSION,
	      unix_ipsse_register_thyself, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      "req: sse, iphost");
