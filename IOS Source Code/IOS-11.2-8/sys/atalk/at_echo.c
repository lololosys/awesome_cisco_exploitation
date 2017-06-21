/* $Id: at_echo.c,v 3.3.18.5 1996/07/23 22:52:31 slin Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_echo.c,v $
 *------------------------------------------------------------------
 * at_echo.c -- Appletalk Echo support
 *
 * September 1988, Bill Palmer
 * October 1991, Russ Tremain, Added responder support.
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_echo.c,v $
 * Revision 3.3.18.5  1996/07/23  22:52:31  slin
 * CSCdi63836:  Rework obsolete os call in atalk
 * Branch: California_branch
 * - Remove edisms in send_atalkecho
 *
 * Revision 3.3.18.4  1996/07/10  22:18:00  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.3.18.3  1996/05/19  01:46:35  dwong
 * CSCdi57975:  Bus Error, Router Crashes when switching apple over Serial
 * Branch: California_branch
 * Another idb nightmare
 *
 * Revision 3.3.18.2  1996/03/23  01:26:09  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.3.18.1  1996/03/18  18:51:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:28:33  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:17:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/08  20:09:33  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.2  1995/11/17  08:42:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:49:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/13  06:49:33  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.2  1995/06/09 12:58:27  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:07:56  hampton
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
#include "../dev/monitor1.h"		/* for at_responder version info */
#include <interface.h>
#include "packet.h"
#include "logger.h"
#include "ttysrv.h"
#include "config.h"
#include "atalk_private.h"
#include "macip_util.h"
#include "../ui/command.h"
#include "../snmp/sr_pingmibdefs.h"
#include "../snmp/sr_pingmib.h"
#include "registry.h"
#include "../snmp/snmp_registry.h"
#include "../ui/common_strings.h"


static boolean send_atalkecho(ataddrtype hst,atalkechotype* data,int size);
static int snmp_atalk_echo_validate (ciscoPingEntry_t *entry);
static void snmp_atalk_echo_activate (ciscoPingEntry_t *entry);

#define MAXLEN 80

boolean
atalk_get_sweep (ulong *minn, ulong *maxx, ulong *intvl)
{
    char buff[MAXLEN];
    boolean parsed;
    ulong sysmin, sysmax;
    
    sysmin = *minn;
    sysmax = *maxx;
    printf("\nSweep min size [%d]: ", *minn);
    parsed = rdtty(buff, MAXLEN);
    if (parsed) {
	if (!null(buff))
	    *minn = parse_unsigned(buff, &parsed);
	parsed = parsed && (*minn > 0);
    }
    if (!parsed || (*minn < sysmin)) {
	printf("\n%% Bad minimum size");
	return (FALSE);
    }
    printf("\nSweep max size [%d]: ", *maxx);
    parsed = rdtty(buff, MAXLEN);
    if (parsed) {
	if (!null(buff))
	    *maxx = parse_unsigned(buff, &parsed);
	parsed = parsed && (*maxx > 0);
    }
    if (!parsed || (*maxx > sysmax) || (*maxx <= *minn)) {
	printf("\n%% Bad maximum size");
	return (FALSE);
    }
    printf("\nSweep interval [%d]: ", *intvl);
    parsed = rdtty(buff, MAXLEN);
    if (parsed) {
	if (!null(buff))
	    *intvl = parse_unsigned(buff, &parsed);
	parsed = parsed && (*intvl > 0);
    }
    if (!parsed || (*intvl == 0)) {
	printf("\n%% Bad interval size");
	return (FALSE);
    }
    return (TRUE);
}

/*
 * atalk_ping_parse
 * Parse a ping command line.  Called by SERVICE_PING_PARSE.
 * Return TRUE if positive identification of an Appletalk address.
 */

boolean atalk_ping_parse (char *args, int hint, int *index,
		          char **cp, boolean *running)
{
    ushort address;
    char tmpbuff[MAXLEN];

    *running = atalk_running;
    *cp = "Appletalk";
    *index = LINK_APPLETALK;

    if (!atalk_running)
	return (FALSE);

    if (hint < 0)
	return(strmatch(args, "appletalk", FALSE));
    else if (hint == LINK_ILLEGAL) {
	if (atalk_parse_address(args, &address, tmpbuff, NULL))
	    return(TRUE);
    } else if ((hint == LINK_APPLETALK) &&
	       (!args || !*args || atalk_parse_address(args, &address,
						       tmpbuff, NULL)))
	return(TRUE);
    return(FALSE);
}

/*
 * atalk_ping
 * Appletalk Echo Protocol ping command
 */
void
atalk_ping (char *args)
{
    long size, repeat, seconds, hits;
    ulong sweepmin, sweepmax, sweepint, sweeprepeat;
    ushort net;
    char buff[MAXLEN];
    boolean verbose, sweep;
    int i;
    uchar host;
    ataddrtype node;
    ulong tmin = 999999999, tmax = 0, tsum = 0;
    atalkechotype data;
    
    size = 100;			/* default datagram size */
    repeat = 5;			/* default repeat count */
    net = 0L;			/* no default network */
    seconds = 2;		/* default timeout */
    verbose = FALSE;		/* not verbose by default */
    sweep = FALSE;		/* not sweeping by default */
    sweepmin = ATALK_MINECHOSIZE+DDP_OVERHEADBYTES; /* run the gamut */
    sweepmax = ATALK_MAXECHOSIZE+DDP_OVERHEADBYTES; /* of sizes if sweeping */
    sweepint = 1;		/* use each size */
    sweeprepeat = 1;
    /*
     * get Appletalk address
     */
    if (!atalk_parse_address(args, &net, &host, NULL)) {
	printf("Target AppleTalk address: ");
	if (!rdtty(buff, MAXLEN)) {
	    printf("\n%% Bad Appletalk address");
	    return;
	}

	if (!atalk_parse_address(buff, &net, &host, NULL)) {
	    printf("\n%% Bad Appletalk address");
	    return;
	}

	/*
	 * get repeat count, size, timeout
	 */
	if (!ping_common(&repeat, &size, sweepmin,
			 sweepmax, &seconds))
	    return;
	/*
	 * verbose mode?
	 */
	printf("\nVerbose [n]: ");
	if (rdtty(buff, MAXLEN) && toupper(*buff) == 'Y') 
	    verbose = TRUE;
	/*
	 * "sweep" mode?
	 */
	printf("\nSweep range of sizes [n]: ");
	if (rdtty(buff, MAXLEN) && toupper(*buff) == 'Y') {
	    sweep = TRUE;
	    if (!atalk_get_sweep(&sweepmin,&sweepmax,&sweepint))
		return;
	    size = sweepmin;
	    sweeprepeat = ((sweepmax - sweepmin) / sweepint) + 1;
	}
    }
    
    /*
     * send some packets
     */
    psion(stdio);		/* allow user to interrupt us */
    node = atalk_address(net, host);
    printf(type_escape_seq_abort_msg);
    if (sweep == TRUE)
	printf("\nSending %d, [%d..%d]-byte AppleTalk Echos to %a, timeout is %d seconds:",
	       repeat*sweeprepeat, sweepmin, sweepmax, node, seconds);
    else
	printf("\nSending %d, %d-byte AppleTalk Echos to %a, timeout is %d seconds:",
	       repeat, size, node, seconds);
    flush();
    data.seqnum = atalk_random(MAXINT);
    for (i = 0, hits = 0; (i < repeat*sweeprepeat) &&
	 (psipending() == FALSE); i++) {
	if ((i % 70) == 0 && !verbose) {
	    printc('\n');	/* wrap line if needed */
	    flush();
	}
	TIMER_START(data.timeout, (ulong)seconds * ONESEC);
	data.responder = 0;
	data.hops = 0;
	data.seqnum++;
	send_atalkecho(node, &data, size);
	switch(data.how) {
	case '!':		/* good reply seen */
	    hits++;
	    if (data.deltatime == 0L)
		data.deltatime = 1L;
	    tsum += data.deltatime;
	    if (data.deltatime < tmin)
		tmin = data.deltatime;
	    if (data.deltatime > tmax)
		tmax = data.deltatime;
	    if (verbose) {
		if (sweep == TRUE)
		    printf("\nsize %d in %d ms", size,
			   data.deltatime);
		else
		    printf("\n%d in %d ms", i, data.deltatime);
		printf(" from %a via %d hop%s", data.responder,
		       data.hops,
		       (data.hops!=1)?"s":"");
	    }
	    break;
	case 'B':
	    if (verbose)
		printf("\nbad echo received from %a", 
		       data.responder);
	    break;
	case 'C':
	    if (verbose)
		printf("\nbad checksum received from %a",
		       data.responder);
	    break;
	case 'E':
	    if (verbose)
		if (sweep == TRUE)
		    printf("\nsize %d send failed", size);
		else
		    printf("\n%d send failed", i);
	    break;
	case 'R':
	    if (verbose)
		if (sweep == TRUE)
		    printf("\nsize %d send failed, no route", size);
		else
		    printf("\n%d send failed, no route", i);
	    break;
	case '.':
	    if (verbose)
		if (sweep == TRUE)
		    printf("\nsize %d timed out", size);
		else
		    printf("\n%d timed out", i);
	    break;
	default:
	    if (verbose)
		printf("\nunknown echo status %#x", data.how);
	    else
		data.how = '?';
	    break;
	}
	if (!verbose)
	    printc(data.how);
	flush();
	(void)process_may_suspend();
	if (sweep == TRUE) {
	    size += sweepint;
	    if (size > sweepmax)
		size = sweepmin;
	}
	/*
	 *
	 * We will only see PSIPENDING if we do this
	 */
	while (getc(stdio) != -1) {
	  /* Nothing */
	}
    }
    if (i > 0) {
	printf("\nSuccess rate is %d percent (%d/%d)",
	    (hits*100)/i, hits, i);
	if (hits) {
	    if (tmin == 0)
		tmin = 1;
	    printf(", round-trip min/avg/max = %ld/%ld/%ld ms",
		   tmin, tsum/hits, tmax);
	}
    }
    psioff(stdio);
}

/*
 * send_atalkecho
 * Send an Appletalk echo to a host and block for reply.
 * Return TRUE if reply received, FALSE otherwise.
 */
static boolean 
send_atalkecho (ataddrtype host, atalkechotype* data, int bytes)
{
    paktype *pak;
    ddpecho *echo;
    atalk_rdb_t* r;
    sys_timestamp clock;
    rbTree	*tree;

    tree = atroute_GetTree();
    r = atroute_FindRoute(tree,atalk_netonly(host), FALSE);
    if (r == NULL || (r && r->rpath.idb == NULL)) {
	data->how = 'R';
	atalk_stat[ATALK_ECHO_FAILED]++;
	return (FALSE);
    }
    pak = atalk_pak_create(r->rpath.idb,host,ATALK_SOCK_PING,
			   ATALK_SOCK_ECHO,
			   DDP_TYPE_ECHO,bytes-DDP_OVERHEADBYTES);
    ReleaseRoute(r);
    if (!pak) {
	data->how = 'E';
	atalk_stat[ATALK_NO_BUFFERS]++;
	return (FALSE);
    }
    memset(pak->transport_start, 0, pak->atalk_datalen);
    atalk_pak_hearself(pak);
    echo = (ddpecho *)pak->transport_start;
    echo->echofunc = DDP_ECHO_REQUEST;
    PUTLONG(&echo->echoseq,data->seqnum);
    if (bytes >= ATALK_MINECHOSIZE) {
	GET_TIMESTAMP(clock);
	bcopy((uchar*)&clock,echo->data, sizeof(clock));
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
    data->next = NULL;
    data->active = TRUE;
    data->tty = stdio;
    enqueue(&atalkechoQ, data);

    /* Suspend until reply received, user interrupt or timeout */
    for(;;) {
	if (!data->active || psipending2(data->tty) || AWAKE(data->timeout))
	    break;
	process_suspend();
    }
    if (data->active)
	p_unqueue(&atalkechoQ, data);
    return (data->active ? FALSE : TRUE);
}

/*
 * atalk_echo
 * answer echo request by changing type to Echo Reply
 * and sending it back to the original sender
 */
void
atalk_echo (paktype *pak)
{
    ddpecho *ep;
    ushort dnet = atalk_pak_dnet(pak);
    uchar mysock = atalk_pak_dsock(pak);
    atalkidbtype* idb = atalk_findIDB(dnet);
    if (idb == NULL) 
	idb = atalk_getidb(pak->if_input);

    atalk_pak_setdnode(pak,atalk_pak_snode(pak));
    atalk_pak_setdnet(pak,atalk_pak_snet(pak));
    atalk_pak_setsnode(pak,atalkif_MyNode(idb));
    atalk_pak_setsnet(pak,atalkif_MyNet(idb));
    atalk_pak_setdsock(pak,atalk_pak_ssock(pak));
    atalk_pak_setssock(pak,mysock);
    ep = (ddpecho *)pak->transport_start;
    ep->echofunc = DDP_ECHO_REPLY; 
    atalk_pak_setlength(pak,pak->atalk_datalen);
    atalk_pak_hearself(pak);

    atalk_DDPcksum(pak);
    if (atalk_send(pak))
	atalk_stat[ATALK_ECHO_REPLIES]++;
    else
	atalk_stat[ATALK_ECHO_FAILED]++;
}  

/*
 * atalk_echo_rcvd
 * handle receipt of a reply to our echo request
 */
void
atalk_echo_rcvd (paktype *pak)
{
    atalkechotype *ep;
    sys_timestamp clock;
    ddpecho* echo;

    for (ep = (atalkechotype *)atalkechoQ.qhead; ep;
	 ep = ep->next) {
	echo = (ddpecho*) pak->transport_start;
	if ((atalk_pak_dsock(pak) == ATALK_SOCK_PING) &&
	    ((ep->seqnum) == GETLONG(&echo->echoseq)) &&
	    (ep->active)) {
	    if (pak->atalk_datalen - ATALK_MINECHOSIZE >= sizeof(clock)) {
		bcopy(echo->data,(uchar*)&clock, sizeof(clock));
		clock = h2nl(clock);
		ep->deltatime = ELAPSED_TIME(clock);
	    } else
		ep->deltatime = 0;
	    ep->responder = atalk_pak_srcaddr(pak);
	    ep->hops = atalk_pak_hops(pak) + 1;
	    ep->how = '!';
	    ep->active = FALSE;
	    p_unqueue(&atalkechoQ, ep);
	    break;
	} else {
	    atalk_stat[ATALK_ECHO_DROPPED]++;
	}
    }
    datagram_done(pak);
}

/*
 * blast
 * Like ping, only nastier.  For debugging only.
 *
 */
#ifdef NOTDEF
atalkblast (ushort *innet, uchar inhost)
{
    long size, repeat, seconds, hits;
    ushort net;
    char buff[MAXLEN];
    int i;
    uchar host;
    paktype *pak;
    atalk_rdb_t* r;
    ddpecho *echo;

    size = 100;			/* default datagram size */
    repeat = 5;			/* default repeat count */
    net = 0L;			/* no default network */
    seconds = 2;		/* default timeout */
    /*
     * get Appletalk address
     */

    if (innet == 0) {
	printf("Target Appletalk address: ");
	if (!rdtty(buff, MAXLEN)) {
	    printf("\n%% Bad Appletalk address");
	    return;
	}
	else
	if (!strcasecmp(buff, "nbp")) {
		/* RT: undocumented entry to NBP test facility */
		nbp_test();
		return;
	}
	else
	if (!atalk_parse_address(buff, &net, &host)) {
	    printf("\n%% Bad Appletalk address");
	    return;
	}

    } else {
	net = innet;
	host = *inhost;
    }
    /*
     * get repeat count, size, timeout
     */
    if (!ping_common(&repeat, &size, ATALK_MINECHOSIZE + DDP_OVERHEADBYTES, 
		     ATALK_MAXECHOSIZE + DDP_OVERHEADBYTES, &seconds))
	return;

    r = atalk_find_rt(net);
    if (!r) {
	printf("\nAT: Cannot route to net %d",net);
	return;
    }
    pak = atalk_pak_create(p->idb,atalk_address(net,host),ATALK_SOCK_ECHO,
			   ATALK_SOCK_ECHO,
			   DDP_TYPE_ECHO,
			   size-DDP_OVERHEADBYTES);
    if (!pak)
	return;
    size = pak->atalk_datalen;
    memset(pak->transport_start, 0, size);
    echo = (ddpecho *)pak->transport_start;
    echo->echofunc = DDP_ECHO_REQUEST;
    atalk_DDPcksum(pak); 

    /*
     * Make sure ARP cache is non-empty
     */
    pak_lock(pak);			/* Place a lock on the packet */
    if (!atalk_send(pak)) {
	printf("\nAT: cannot send Echo pkt");
	return;
    }
    psion(stdio);
    i = 0;
    while (i++ < repeat && !psiset(stdio)) {
	pak_lock(pak);			/* Place a lock on the packet */
	(void)atalk_send1(pak, pak->dhost);
	{
	    int j;
	    for (j = 0; j < seconds; j++)
		;

	}
    }
    psioff(stdio);
    datagram_done(pak);
    return;
}

#endif

/*
 * atalk_EchoHandler
 */
void
atalk_EchoHandler (paktype* pak)
{
    ddpecho* ep;

    atalk_pak_printprocess(pak, atalktrue_debug, "echo packet");
    ep = (ddpecho *)pak->transport_start;
    atalk_stat[ATALK_ECHO]++;

    if (atalk_ValidPacket(pak,1,FALSE) != DDP_TYPE_ECHO) {
	atalk_pak_printfail(pak, atalktrue_debug, "invalid echo packet");
	atalk_dispose(ATALK_UNKNOWN, pak);
	return;
    }

    switch (ep->echofunc) {    
    case DDP_ECHO_REQUEST:
	atalk_echo(pak);
	break;
    case DDP_ECHO_REPLY:
	atalk_pak_printfail(pak, atalktrue_debug, "unsolicited AEP echo reply");
	atalk_dispose(ATALK_ECHO_DROPPED,pak);
	break;
    default:
	atalk_pak_printfail(pak, atalktrue_debug, "unknown echo function");
	atalk_dispose(ATALK_ECHO_ILL, pak);
	break;
    }
}	

/*
 * atalk_PingHandler
 * Handle packets returned for our ping server.
 */
void
atalk_PingHandler (paktype* pak)
{
    ddpecho* ep;

    atalk_pak_printprocess(pak, atalktrue_debug , "ping packet");
    ep = (ddpecho *)pak->transport_start;
    atalk_stat[ATALK_ECHO]++;

    if (atalk_ValidPacket(pak,ATALK_MINECHOSIZE,FALSE) != DDP_TYPE_ECHO) {
	atalk_pak_printfail(pak, atalktrue_debug, "invalid ping packet");
	atalk_dispose(ATALK_UNKNOWN, pak);
	return;
    }

    switch (ep->echofunc) {    
    case DDP_ECHO_REQUEST:
	atalk_echo(pak);	/* handle echo request packet */
	break;
    case DDP_ECHO_REPLY:
	atalk_echo_rcvd(pak);   /* handle echo reply packet */
	break;
    default:
	atalk_pak_printfail(pak, atalktrue_debug, "unknown ping function");
	atalk_dispose(ATALK_ECHO_ILL, pak);
	break;
    }
}	

/*
 * at_responder - implement Macintosh Responder Protocol for
 *		the cisco router.
 *
 * October 1991, Russ Tremain, Tremain Laboratories, under contract
 *  for cisco Systems, Inc.
 *	
 */

typedef struct responderUbytes {
    uchar	op;			/* responder opcode */
    uchar	unused1;		/* UNUSED */
    uchar	vers;			/* responder version */
    uchar	subvers;		/* responder sub-version */
} responderUbytes;

#define	AT_RESPONDER_REQUEST	0
#define	AT_RESPONDER_REPLY	1

/*
 * note - unclear how long the version strings can be,
 * there is clearly a limit to the number of bytes displayed
 * by the Inter*Poll application on the Mac (about 64).
 */
typedef struct responder_ {
	ushort		at_vers;	/* appletalk driver version */
	ushort		unused1;	/* UNUSED */
	ushort		unused2;	/* UNUSED */
	ushort		unused3;	/* UNUSED */
	ushort		unused4;	/* UNUSED */
	str160		prom_vers;	/* prom version */
	str160		ram_vers;	/* download version */
	ushort		unused5;	/* UNUSED */
	str32		misc_vers;	/* for future use */
} responder;

/* macro defining the size of a variable length responder record: */
#define	AT_RESPONDER_LEN(RR)\
	(6*sizeof(ushort) + 1+PSTRLEN(&(RR)->prom_vers)\
		+ 1+PSTRLEN(&(RR)->ram_vers) + 1+PSTRLEN(&(RR)->misc_vers))

/* value of Macintosh ATp2 drivers */
#define AT_ATP2_VERS 0x0038

/*
 * vers_filter -
 *	delete first newline or return, convert others to blanks,
 *	compress multiple spaces, shorten copyright notice.
 *	
 *	do not exceed declared length of dst.
 *	
 */
static void vers_filter (uchar *dst, char *src, int dcllen)
{
#define	COPYRIGHT "Copyright (c)"
#define	COPYRIGHT_CHAR 0xa9		/* from apple char set */
    register int ii, jj;

    /* copy src to dst until done or out of room */
    for (ii=0, jj=0; src[ii] != '\0' && jj < dcllen-1; ii++) {
	if (src[ii] == '\n' || src[ii] == '\r' || src[ii] == ' ') {
	    /* compress blanks */
	    if (ii > 0 && (jj > 0 && dst[jj-1] != ' '))
		    dst[jj++] = ' ';
	}
	else
	if (src[ii] == 'C') {
	    /* replace copyright string with apple copyright char: */
	    if (strncmp(&src[ii], COPYRIGHT, strlen(COPYRIGHT)) == 0) {
		dst[jj++] = COPYRIGHT_CHAR;
		ii += strlen(COPYRIGHT) -1;
	    } else {
		dst[jj++] = src[ii];
	    }
	} else {
	    dst[jj++] = src[ii];
	}
	
    }

    dst[jj] = '\0';
}

/*
 * atalk_Responder -
 *	
 *	respond to a Apple "system-request" packet ala Inter*Poll
 *	
 */
void
atalk_Responder (paktype *pak)
{
    static int		response_len = 0;
    static responder	ciscoResponderDat;
    responder 		*mr = &ciscoResponderDat;
    atptype		*atpin;
    atptype		*newatp;
    responderUbytes	*rub;
    uchar		*pp;
    paktype		*newpak = NULL;

    if (pak == NULL || pak->if_input == NULL)
	return;

    if (at_debug(atalkrsp_debug, ATALK_INTERFACE_MASK, pak->if_input))
	buginf("\nAT: Responder:  packet , response_len=%d\n",
		pak, response_len);

    /* if cisco responder info not yet initialized... */
    if (response_len == 0) {
	char *tmp;
	
	tmp = malloc(sizeof(char) * 160);
	if (tmp == NULL) {
	    errmsg(AT_ERR_NOMEM, "responder buffer", __LINE__, __FILE__);
	    datagram_done(pak);
	    return;
	    
	}
		
	/* then set it (see host_banner())... */
	vers_filter(tmp, mon_vstring(), 160);
	str160init(&mr->prom_vers, tmp);

	vers_filter(tmp, version, 160);
	str160init(&mr->ram_vers, tmp);

	str32init(&mr->misc_vers, "");	 /* for future use */

	mr->at_vers = AT_ATP2_VERS;

	/* note - remaining fields are zero */

	/* calculate length of response data: */
	response_len = ATP_OVERHEADBYTES + AT_RESPONDER_LEN(mr);
	if (at_debug(atalkrsp_debug, ATALK_NULL_MASK)) {
	    buginf("\nAT: responder, init response w/ versions of"
		   "\n   PROM='%s', RAM= '%s' MISC='%s' AT= '%d'",
		   PSTRPTR(&mr->prom_vers), PSTRPTR(&mr->ram_vers),
		   PSTRPTR(&mr->misc_vers), mr->at_vers);
	}
	free(tmp);
    }

    /* make sure we have an ATP packet: */
    if (atalk_ValidPacket(pak, 0, FALSE) != DDP_TYPE_ATP) {
	if (at_debug(atalkrsp_debug, ATALK_INTERFACE_MASK, pak->if_input)) {
	    atalk_pak_printfail(pak, atalkrsp_debug,
				"bad responder packet type");
	}
	atalk_dispose(ATALK_UNKNOWN, pak);
	return;
    }

    ++atalk_stat[AT_RESPONDER_IN];

    /* Review: an atp pkt:
     * atptype:
     *  byte ccf        command/control field
     *  byte bitmap     bitmap/sequence no
     *  byte tidhi      transaction identifier, msb
     *  byte tidlo      transaction identifier, lsb
     *  byte userbytes[4]   user bytes
     */

    atpin = (atptype *)pak->transport_start;

    if (at_debug(atalkrsp_debug, ATALK_INTERFACE_MASK, pak->if_input)) {
        long ub = *((long *) &atpin->userbytes[0]);
        buginf("\nAT: responder;  ATP request in ubytes=%08lx\n", ub);
    }

    /* do we have a request? */
    if (ATP_GET_FUNC(atpin->ccf) != ATP_TREQ) {
	atalk_dispose(AT_RESPONDER_BAD_ATP, pak);
        return;
    }

    rub = (responderUbytes *) &atpin->userbytes[0];
    switch (rub->op) {
    case AT_RESPONDER_REQUEST:
	break;
    default:
	atalk_dispose(AT_RESPONDER_BAD_OPCODE, pak);
        return;
    }

    /* need a new pkt in case incoming is too small: */
    newpak = atalk_pak_reply(pak, DDP_TYPE_ATP, response_len);

    if (newpak == NULL) {
	atalk_dispose(AT_RESPONDER_CREATE_ERROR, pak);
        return;
    }

    /* copy ATP data, free input packet: */
    newatp = (atptype *)newpak->transport_start;

    /* copy transaction id: */
    newatp->tidhi = atpin->tidhi;
    newatp->tidlo = atpin->tidlo;

    /* now finished with input pkt: */
    datagram_done(pak);
    pak=NULL;

    /* set reply command/control field: */
    newatp->ccf = 0;
    ATP_SET_FUNC(newatp->ccf, ATP_TRESP);  /* this is a response */
    ATP_SET_EOM(newatp->ccf);          /* set end-of-msg bit */

    /* bitmap: */
    newatp->bitmap = 0;

    /* user bytes: */
    rub = (responderUbytes *) &newatp->userbytes[0];
    rub->op	= AT_RESPONDER_REPLY;
    rub->unused1= 0;
    rub->vers	= 1;	/* cisco responder version 1.0 */
    rub->subvers= 0;

    pp = (uchar *) newatp;
    pp += ATP_OVERHEADBYTES;

    /* pack our responder data: */
    PUTSHORT(pp, mr->at_vers); pp += sizeof(mr->at_vers);
    PUTSHORT(pp, mr->unused1); pp += sizeof(mr->unused1);
    PUTSHORT(pp, mr->unused2); pp += sizeof(mr->unused2);
    PUTSHORT(pp, mr->unused3); pp += sizeof(mr->unused3);
    PUTSHORT(pp, mr->unused4); pp += sizeof(mr->unused4);

    PSTRCPY((str160 *)pp, &mr->prom_vers);
    pp += 1+PSTRLEN(&mr->prom_vers);

    PSTRCPY((str160 *)pp, &mr->ram_vers);
    pp += 1+PSTRLEN(&mr->ram_vers);

    PUTSHORT(pp, mr->unused5); pp += sizeof(mr->unused5);

    PSTRCPY((str32 *)pp, &mr->misc_vers);
    pp += 1+PSTRLEN(&mr->misc_vers);

    atalk_DDPcksum(newpak);
    if (atalk_send(newpak))
        ++atalk_stat[AT_RESPONDER_OUT];
    else
        ++atalk_stat[AT_RESPONDER_SEND_FAIL];
}	


/*
 * snmp_atalk_echo_validate
 * Validate a ATALK echo for SNMP
 * Called via the snmp_ping_validate registry
 */
static int snmp_atalk_echo_validate (ciscoPingEntry_t *entry)
{
    /*
     * there is a real problem if the protocol isn't set, or if it
     * isn't set to the same value as the case registry switch value
     */
    if (!VALID(I_ciscoPingProtocol, entry->valid) ||
        (entry->ciscoPingProtocol != D_ciscoPingProtocol_appletalk))
        return (SNMP_PING_INVALID_PROTOCOL);

    /*
     * If the address isn't set, we simply continue
     * (the activate routine won't be called by snmp until an address has
     * been set and validated) DDPNETBYTES + DDPNODEBYTES = ADDRLEN_APPLE
     */
    if (VALID(I_ciscoPingAddress, entry->valid) &&
        (entry->ciscoPingAddress->length != DDPNETBYTES+DDPNODEBYTES))
        return (SNMP_PING_INVALID_ADDRESS);

    /*
     * if the packet size is set, it better be within the legal range.
     * if the packet size is not set, we simply continue (the activation
     * routine won't be called by snmp until a packet size has been set
     * and validated)
     */
    if (VALID(I_ciscoPingPacketSize, entry->valid) &&
        ((entry->ciscoPingPacketSize < ATALK_MINECHOSIZE) ||
        (entry->ciscoPingPacketSize > ATALK_MAXECHOSIZE)))
        return (SNMP_PING_INVALID_SIZE);

    return (SNMP_PING_VALID);
}


/*
 e snmp_atalk_echo_activate
 * Send a IPX echo for SNMP
 * Called via the snmp_ping_activate registry
 */
static void snmp_atalk_echo_activate (ciscoPingEntry_t *entry)
{
    ushort net;
    uchar host;
    ataddrtype node;
    atalkechotype data;
    ulong total_rtt, idx;
    
    /*
     * Recover the network and node from the OctetString.
     * The address is in format DDPNETBYTES.DDPNODEBYTES, net.host.
     * Remember to ping 4165.18 user must use '10 45 12' octet string.
     */
    net = ((entry->ciscoPingAddress->octet_ptr[0] << 8) + 
	entry->ciscoPingAddress->octet_ptr[1]);
    host = entry->ciscoPingAddress->octet_ptr[2];
    node = atalk_address(net, host);
 
    entry->ciscoPingSentPackets = 0;
    SET_VALID(I_ciscoPingSentPackets, entry->valid);
    entry->ciscoPingReceivedPackets = 0;
    SET_VALID(I_ciscoPingReceivedPackets, entry->valid);
    entry->ciscoPingCompleted = D_ciscoPingCompleted_false;
    SET_VALID(I_ciscoPingCompleted, entry->valid);
    total_rtt = 0;

    data.seqnum = atalk_random(MAXINT);
    for (idx = 1; idx <= entry->ciscoPingPacketCount; idx++) {
	entry->ciscoPingSentPackets++;
	TIMER_START(data.timeout, entry->ciscoPingPacketTimeout);
	data.responder = 0;
	data.hops = 0;
	data.seqnum++;
	send_atalkecho(node, &data, entry->ciscoPingPacketSize);
	if (data.how == '!') {
	    entry->ciscoPingReceivedPackets++;
	    if (data.deltatime == 0L)
		data.deltatime = 1L;
	    total_rtt += data.deltatime;
	    if ((!VALID(I_ciscoPingMinRtt, entry->valid)) ||
		(data.deltatime < entry->ciscoPingMinRtt)) {
		entry->ciscoPingMinRtt = data.deltatime;
		SET_VALID(I_ciscoPingMinRtt, entry->valid);
	    }
	    if ((!VALID(I_ciscoPingMaxRtt, entry->valid)) ||
		(data.deltatime > entry->ciscoPingMaxRtt)) {
		entry->ciscoPingMaxRtt = data.deltatime;
		SET_VALID(I_ciscoPingMaxRtt, entry->valid);
	    }
	    entry->ciscoPingAvgRtt =
		total_rtt / entry->ciscoPingReceivedPackets;
		SET_VALID(I_ciscoPingAvgRtt, entry->valid);
	}
	(void)process_may_suspend();
	if (idx < entry->ciscoPingPacketCount) {
	    process_sleep_for(entry->ciscoPingDelay);
	}
    }
    entry->ciscoPingCompleted = D_ciscoPingCompleted_true;
}



void register_snmp_atalk_echo(void)
{
    reg_add_snmp_ping_validate(D_ciscoPingProtocol_appletalk,
		snmp_atalk_echo_validate,
		"snmp_atalk_echo_validate");

    reg_add_snmp_ping_activate(D_ciscoPingProtocol_appletalk,
		snmp_atalk_echo_activate,
		"snmp_atalk_echo_activate");
}

