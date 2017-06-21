/* $Id: at_trace.c,v 3.3.18.3 1996/07/10 22:19:05 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_trace.c,v $
 *------------------------------------------------------------------
 * AppleTalk Trace command implementation.
 *
 * February 1993, David S.A. Stine, on a whim.
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_trace.c,v $
 * Revision 3.3.18.3  1996/07/10  22:19:05  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.18.2  1996/03/23  01:27:46  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3.18.1  1996/03/18  18:53:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:29:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:17:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/08  20:09:37  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.2  1995/11/17  08:43:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:50:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/13  06:50:11  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.1  1995/06/07 20:09:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "packet.h"
#include "interface.h"
#include "logger.h"
#include "atalk_private.h"

/*
 * command buffer length.
 */
#define MAXLEN (80)

/*
 * atalk_trace
 *
 * Send Atalk echo packets with echo function 3 to ping, 4 to echo
 * (ie, we send function TRACE, expect TREPLY) to routers along the path to
 * the destination.
 *
 * We timestamp packets on the way out and they send the packet
 * back to us.
 */
void
atalk_trace (char *args)
{
    printf("\n%%Not yet implemented.");
#if 0
    idbtype	*idb;
    char 	buff[MAXLEN];		/* input buffer */
    ulong	seconds;		/* time to wait for response */
    boolean	numericonly;		/* don't do NBP names */
    boolean	parsed;			/* state of various parses */
    boolean	verbose;		/* print extra output */
    ushort	src_net;		/* source Atalk network */
    uchar	src_node;		/* source appletalk node */


    if (!atalk_parse_address(args, &net, &node)) {
	printf("\nTarget AppleTalk address: ");
	if (!rdtty(buff, MAXLEN) || !atalk_parse_address(buff, &net, &node)) {
	    printf("\n%% Bad AppleTalk address or host name");
	    return;
	}
	printf("\nSource address: ");
	parsed = rdtty(buff, MAXLEN);
	if (parsed) {
	    if (!null(buff)) {
		/*
		 * Confirm that the address is one of ours by scanning
		 * all interfaces.
		 */
		parsed = FALSE;
		if (atalk_parse_address(buff, &src_net, &src_node)) {
		    FOR_ALL_SWIDBS(idb){
			parsed = (idb, source);
			if (parsed)
			    break;
		    }
		}
	    }
	}
	if (!parsed) {
	    printf("\n%% Invalid source address");
	    return;
	}
	printf("\nNumeric display [n]: ");
	parsed = rdtty(buff, MAXLEN);
	if (parsed) {
	    if (!null(buff)) {
		if (toupper(*buff) == 'Y')
		    numericonly = TRUE;
	    }
	}
	if (!parsed) {
	    printf("\n%% Y or N");
	    return;
	}
    } else {
	seconds = 3;
	ntries  = 3;
	numericonly = TRUE;
    }

    /*
     * At this point, we have all the info we need to actually start the
     * trace.
     */
    psion(stdio);
    printf("%s%s", type_escape_seq_abort_msg, tracing_the_route_to_msg);

    if (numericonly)
        printf("%a\n", destination);
    else if (namep = at_address_lookup(destination))  /* yes, "=", not "==" */
	printf("%s (%i)\n", namep->name, namep->v.ip.adr[0].addr.ip_address);
    else
	printf("%a\n", destination);
    flush();
    gotthere = FALSE;
    current_router = 0;

    /*
     * Look familiar? Scarfed from atalk_sendecho()...
     */
    tree = atroute_GetTree();
    r = atroute_FindRoute(tree,net,FALSE);
    if (r == NULL) {
	errmsg(AT_ERR_NOROUTE, atalk_address(net, node));
	return (FALSE);
    }
    pak = atalk_pak_create(r->rpath.idb, host, ATALK_SOCK_PING,
			   ATALK_SOCK_ECHO,
			   DDP_TYPE_ECHO,bytes-DDP_OVERHEADBYTES);
    atroute_ReleaseRoute(r);
    if (!pak) {
	errmsg(AT_ERR_NOMEM, "trace packet", __LINE__, __FILE__);
	atalk_stat[ATALK_NO_BUFFERS]++;
	return (FALSE);
    }
    memset(pak->transport_start, 0, pak->atalk_datalen);
    atalk_pak_hearself(pak);
    echo = (ddpecho *)pak->transport_start;
    echo->echofunc = DDP_ECHO_TRACE;
    echo->echoseq = 0;
    if (bytes >= ATALK_MINECHOSIZE) {
	GET_TIMESTAMP(clock);
	clock = n2hl(clock);
	bcopy((uchar*)&clock, echo->data, sizeof(clock));
    }
    atalk_DDPcksum(pak); 
    data->how='.';
    if (!atalk_send(pak)) {
	/*
	 * Don't return here since we fail to wait for AARP
	 * to have an oppurtunity to populate.
	 */
	atalk_stat[ATALK_ECHO_FAILED]++;
	data->how = 'E';
    } else {
	atalk_stat[ATALK_ECHO_SENT]++;
    }
    while (!gotthere) {
	printf("\n %-2u ", ttl);
	for (probe = 0; probe < nprobes; ++probe) {
	    pak = atalk_pak_create
	    data = malloc(sizeof (tracetype));
	    if (!data)
		return;
#endif
}
