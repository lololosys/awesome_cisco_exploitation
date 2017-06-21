/* $Id: macip_server.c,v 3.5.42.6 1996/08/28 12:36:53 thille Exp $
 * $Source: /release/112/cvs/Xsys/atalk/macip_server.c,v $
 *------------------------------------------------------------------
 * macip_server.c   - implement MacIP server architecture.
 *
 * September 1991, Russ Tremain, Tremain Laboratories, under contract
 *  for cisco Systems, Inc.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: macip_server.c,v $
 * Revision 3.5.42.6  1996/08/28  12:36:53  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.42.5  1996/07/10  22:19:36  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.5.42.4  1996/06/29  00:07:26  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.5.42.3  1996/04/15  21:12:40  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.5.42.2  1996/03/23  01:28:37  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.5.42.1  1996/03/18  18:53:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.1  1996/03/07  08:29:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/01/06  01:30:03  tkolar
 * CSCdi46520:  MacIP never leaves initial state
 * Finish making MacIP conform to the new world order.
 *
 * Revision 3.4  1995/11/23  06:50:24  hampton
 * Reduce the memory bloat from 64 bit timers.  [CSCdi44415]
 *
 * Revision 3.3  1995/11/17  08:45:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:29  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:59:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:51:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/10/18  21:47:11  spreissm
 * CSCdi41076:  Software forced crash, PC 0x10BD58 - memory corruption
 *         - macip_atp_service was reusing a buffer w/o first checking
 *           size.
 *
 * Revision 2.2  1995/07/13  06:50:28  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.1  1995/06/07 20:10:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include <mgd_timers.h>
#include <interface_private.h>
#include "packet.h"
#include "address.h"
#include "name.h"			/* use domain-name info */
#include "../ip/ip.h"
#include "../ip/ipaddress.h"
#include "atalk_private.h"
#include "../atalk/atip_debug.h"
#include "macip_util.h"
#include "atalk_nbp.h"
#include "macip_db.h"
#include "macip_fsm.h"
#include "macip_route.h"
#include "macip_server.h"
#include "../ip/ip_registry.h"

/* definitions for at_err(): */
#define DEBUGSWITCH (at_debug(appletalk_debug, ATALK_NULL_MASK))

/* timer to delay executing macip sched check routine: */
static mgd_timer macip_master_timer;
static mgd_timer macip_sched_time;

/* timer types */
#define MACIP_TIMER_SCHED	1


/* used for lookups, declared here to save stack space: */
static key keystor;
static nbpEntity global_nve;

static int macip_up_servers;	/* number of running servers: */
static boolean macip_running;	/* true if macip running */

/* this queue used receives all IP & DDP pkts for macip */
static watched_queue *macipRouteQ;

/* free list for server events, to avoid malloc(): */
static queuetype mse_freeQ;
gccontext_t *macip_gccontext;


/*************************  PRIVATE FUNCTIONS  ************************/

/*
 * macip_timers -
 */
static void
macip_timers (server_rec *ss)
{
    /* by def., a delay of 0 means no MACIP_FSM_TIMEOUT
     * events for this state:
     */
    if (ss->delay == 0)
        return;

    /* if delay expired... */
    if (MACIP_DELAY_UP(ss))
        macip_add_event(ss, MACIP_FSM_TIMEOUT, 0);
}

/*
 * macip_needs_service -
 *	
 *	loop through server relation, & check for servers needing attention.
 *	if <callservice> then call the service routine for each server
 *	and return false.  Otherwise, return true if ANY server needs
 *	attention, returning the server needing attention in <who>.
 *	
 */
static boolean
macip_needs_service (boolean callservice, server_rec **who)
{
    key *kk = &keystor;
    server_rec *ss;
    int sid;

    /* start with result of previous call if provided: */
    ss = *who;
    if (ss == NULL)
        sid = id_first(srvr_idh);
    else {
        if (callservice)
            macip_service(ss);

        /* continue with next server: */
        sid = id_next(srvr_idh, ss->serverid);
    }

    /* foreach (remaining) server.... */
    for (sid = sid; sid != NILID; sid = id_next(srvr_idh, sid))
    {
        /*
         * servers: (short serverid, server_rec *ss)
         */

        db_mkkey(kk, 2, servers, sid);
        if (!DB_RETRIEVE((byteptr) &ss, kk, sizeof(ss))) {
            /* error! */
            continue;
        }

        /* check timers: */
        macip_timers(ss);

        /* if server requires service.... */
        if (process_queue_size(ss->reqQ) > 0
                || process_queue_size(ss->confirmQ) > 0
                || QUEUESIZE(&ss->stateQ) > 0)
        {
            if (callservice)
                macip_service(ss);
            else {
                *who = ss;
                return(TRUE);   /* somebody needs attention */
            }
        }
    }

    /* all servers serviced or none needing attention */
    *who = NULL;
    return(FALSE);
}

/*
 * macip_send_confirm -
 *	
 *	send a confirm request to a client
 *	
 */
static void
macip_send_confirm (server_rec *ss, client_rec *cptr)
{
    nbp_result	err;
    ataddress	src;
    nbpEntity	*nve = &global_nve;

    /*
     * got the client record, send the confirm:
     * note:  we must use zone "*" here, as we don't
     * know the zone of the client.
     */
    atnbp_mkname(nve, ipaddrStr(cptr->ipaddr), "IPADDRESS", "*");

    /* use server ddp interface address, but direct replies to NBP: */
    src.net  = ss->reqaddr.net;
    src.node = ss->reqaddr.node;
    src.spec = ATALK_SOCK_NBP;

    if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
	buginf("\nAT: macip: Sending MacIP NBP Confirm from server %d.%d\n"
	       "      macip: zone '%s' to client %s with address of %d.%d", 
	       ss->reqaddr.net, ss->reqaddr.node,
	       at_hex_display(SY_STRING(ss->zone)),
	       ipaddrStr(cptr->ipaddr), cptr->ddpaddr.net,
	       cptr->ddpaddr.node);

    /* post confirm: */
    err = atnbp_confirm(nve, &cptr->ddpaddr, &src, NULL, ss->confirmQ);
    if (err != NBP_OK)
        at_err(MODID,"macip_send_confirm: send failed, %s",
	       atnbp_resultStr(err));
}

/*
 * macip_nbp_service -
 *	
 *	handle a reply to a nbp confirm or lookup.
 *	enter new clients in database.
 *	
 */
static void
macip_nbp_service (server_rec *ss, nbpQelem *ee)
{
    key		*kk = &keystor;
    client_rec	*cptr;
    ipaddrtype	cip;
    ataddress	ourmacip;
    
    if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
        buginf("\nAT: macip: macip_nbp_service %s: (%s): '%s'",
	       atnbp_msgStr(ee->msg),
	       ddpAddrStr(&ee->srvr_addr),
	       atnbp_entityStr(&ee->nve));
    
    if (ee->msg == NBP_TIMEOUT)
        goto freeit;

    ourmacip.net  = ss->reqaddr.net;
    ourmacip.node = ss->reqaddr.node;
    ourmacip.spec = ATALK_SOCK_MACIP;

    /* if from us... */
    if (DDPEQUAL(&ourmacip, &ee->srvr_addr)) {
        /* ...then ignore: */

	if (at_debug(atalkmacip_debug,0))
            buginf("\nAT-MacIP: macip_nbp_service: from us");
        goto freeit;
    }

    if (!parse_ip_address(STR32PTR(&ee->nve.srvr_name), &cip)) {
	/* bad NPB reply - skip it */
        goto freeit;
    }

    /* if someone stole our IP address... */
    if (cip == ss->ipaddr) {
        /* ...then error: */
        at_err(MODID,"MACIP server, zone %s: IP address IN USE, %i@[%s]",
	       at_hex_display(SY_STRING(ss->zone)),
	       ss->ipaddr, ddpAddrStr(&ee->srvr_addr));
        goto freeit;
    }

    /*
     * lookup client record, based on IP:
     * client_ip:  (ipaddr_hi, ipaddr_lo, client_rec *cptr)
     */
    db_mkkey(kk, 3, client_ip, HI_16(cip), LO_16(cip));
    /* if found... */
    if (DB_RETRIEVE((byteptr) &cptr, kk, sizeof(cptr))) {
	  if (at_debug(atalkmacip_debug,0))
            buginf("\nAT-MacIP: macip_nbp_service: found client");
        /* ...then update timestamp: */
	  
	  GET_TIMESTAMP(cptr->timer);
	  goto freeit;
    }


    if (at_debug(atalkmacip_debug,0))
        buginf("\nmacip_nbp_service: client not found");

    /* if no client, then add: */
    if (mdb_app_client(ss, cip, &ee->srvr_addr) != MDB_OK) {
	/* client address out of range or no memory */
        goto freeit;
    }

freeit:
    atnbp_event_free(ee);
}

/*
 * ipgp_server -
 *	
 *	fill in server info
 *	
 */
static boolean
ipgp_server (server_rec *ss, IPGP *ig)
{
    idbtype *idb = ss->ipidb;

    PUTLONG(&ig->ipaddress, ss->ipaddr);       /* my IP address */
    PUTLONG(&ig->ipbroad, idb->ip_broadcast);    /* IP broadcast address */
    PUTLONG(&ig->ipsubnet, idb->ip_nets_mask);    /* subnet mask */

    PUTLONG(&ig->ipname, domain_server[0]); /* first name server */
    if (default_domain != NULL) {
	STRCPY(ig->msg, default_domain);    /* default domain name */	
    }
    else
	memset(ig->msg, 0,128);

    return(TRUE);
}

/*
 * ipgp_assign -
 *	
 *	assign a dynamic IP address to a new client, append client record
 *	if no resources, then set error op code
 *	if client already has an address, then return the address.
 *	(multiple IP addresses not allowed to the same DDP address)
 *	
 */
static boolean
ipgp_assign (server_rec *ss, ataddress *cliddp, IPGP *ig)
{
    /* fill in server info: */
    if (!ipgp_server(ss, ig)) {
	return(FALSE);
    }

    PUTLONG(&ig->ipaddress, mdb_clientIPbyDDP(ss, cliddp));
    if (GETLONG(&ig->ipaddress) != 0) {
        /* address already assigned for this client.  */
        return (TRUE);
    }

    /* o'wise, allocate a new ip and client record: */
    PUTLONG(&ig->ipaddress, mdb_next_ip(ss));
    if (GETLONG(&ig->ipaddress) == 0) {
        PUTLONG(&ig->op, ipgpError);
        STRCPY(ig->msg, IPGP_MSG_NO_RESOURCES);
        return (FALSE);
    }

    /* o'wise, add client: */
    if (mdb_app_client(ss, GETLONG(&ig->ipaddress), cliddp) != MDB_OK) {
        PUTLONG(&ig->op, ipgpError);
        STRCPY(ig->msg, IPGP_MSG_NO_RESOURCES);
        /* back out: */
        mdb_free_ip(ss, GETLONG(&ig->ipaddress));
        return (FALSE);
    }

    /* success! */
    return (TRUE);
}

/*
 * ipgp_static -
 *	
 *	mark a static address as assigned.
 *	error if already assigned to different DDP address
 *	
 */
static boolean
ipgp_static (server_rec *ss, ataddress *cliddp, IPGP *ig)
{
    client_rec *cptr;
    ipaddrtype cip = GETLONG(&ig->ipaddress);     /* save ip address */

    /* fill in server info (default): */
    if (!ipgp_server(ss, ig)) {
	return (FALSE);
    }
    PUTLONG(&ig->ipaddress, cip);    /* restore static address */

    cptr =  mdb_clientbyIP(cip);
    /* if record already entered... */
    if (cptr != (client_rec *) NULL) {
        /* if assigned to different client... */
        if (!DDPEQUAL(cliddp, &cptr->ddpaddr)) {
            PUTLONG(&ig->op, ipgpError);
            sprintf(ig->msg, "%s @ [%s]",
		    IPGP_MSG_INUSE, ddpAddrStr(&cptr->ddpaddr));
            return (FALSE);
        }

        /* o'wise, client already entered */
        return (TRUE);
    }

    /* not entered, do it: */
    if (mdb_app_client(ss, cip, cliddp) != MDB_OK) {
        PUTLONG(&ig->op, ipgpError);
        STRCPY(ig->msg, IPGP_MSG_INUSE);
        return (FALSE);
    }

    /* success */
    return (TRUE);
}

/*
 * ipgp_release -
 *	
 *	release an IP address, delete client record.
 *	
 */
static boolean
ipgp_release (server_rec *ss, ataddress *cliddp, IPGP *ig)
{
    client_rec *cptr;
    
    cptr =  mdb_clientbyIP(GETLONG(&ig->ipaddress));
    /* if no record... */
    if (cptr == (client_rec *) NULL)
        return (FALSE);

    mdb_purge_client(ss, cptr->ncli);
    return (TRUE);
}

/*
 * macip_atp_service -
 *	
 *	service a macip allocation request packet
 *	TODO: add check to be sure we don't exceed max datagram size
 *	
 */
static void
macip_atp_service (server_rec *ss, paktype *pak)
{
    atptype	*atp;
    IPGP	*ig;
    uchar	*pp;
    ataddress	cliddp;         /* client ddp address */
    int		len;
    paktype	*newpak;
    /* Obtain the length as specified in the header */
    ushort size = atalk_pak_rawlength(pak);
    /* hsize will be the header size for this linktype */
    ddptype*	ddp;

    ++atalk_stat[MACIP_SERVER_IN];

    /* Review: an atp pkt:
     * atptype:
     *  byte ccf        command/control field
     *  byte bitmap     bitmap/sequence no
     *  byte tidhi      transaction identifier, msb
     *  byte tidlo      transaction identifier, lsb
     *  byte userbytes[4]   user bytes
     */

    /* Make sure there is enough room in current packet for max reply */
    len = ATP_OVERHEADBYTES + ipgpMinSize + IPGPMSGSIZE + DDP_OVERHEADBYTES;
    newpak = pak_grow(pak, pak->datagramsize, len);

    /* If we couldn't get a new packet, toss the old one and scram */
    if (newpak == NULL) {
        datagram_done(pak);
        return;
    }

    ddp = (ddptype*)newpak->network_start;
    newpak->transport_start = ((uchar*) ddp) + DDP_OVERHEADBYTES;
    newpak->atalk_datalen = size - DDP_OVERHEADBYTES;

    atp = (atptype *)newpak->transport_start;

    /* do we have a request? */
    if (ATP_GET_FUNC(atp->ccf) != ATP_TREQ) {
        ++atalk_stat[MACIP_SERVER_BAD_ATP];
        datagram_done(newpak);
        return;
    }

    pp = (uchar *) atp;
    pp += ATP_OVERHEADBYTES;
    ig = (IPGP *) pp;

    if (at_debug(atalkmacip_debug, ATALK_INTERFACE_MASK, pak->if_input)) {
        long *pp = (long *) ig;
        if (at_debug(atalkmacip_debug,0)) {
	    buginf("\nAT-MacIP: atp_service:  ATP request in pak=%lx atp=%lx ig=%lx",
		   newpak, atp, ig);
	    buginf("\nAT-MacIP: %08lx.%08lx.%08lx.%08lx",
		   pp[0], pp[1], pp[2], pp[3]);
	}
    }

    cliddp.net  = atalk_pak_snet(newpak);
    cliddp.node   = atalk_pak_snode(newpak);
    cliddp.spec   = atalk_pak_ssock(newpak);

    /*
     * service the request:
     */
    switch (GETLONG(&ig->op)) {
      case ipgpAssign:
        ++atalk_stat[MACIP_SERVER_ASSIGN_IN];
        if (!ipgp_assign(ss, &cliddp, ig)) {
            ++atalk_stat[MACIP_SERVER_ASSIGN_ERROR];
	    if (at_debug(atalkmacip_debug,0)) 
                buginf("\nAT-MacIP: ipgpAssign error");
        } else {
            ++atalk_stat[MACIP_SERVER_ASSIGN_OUT];
	}
        break;
      case ipgpServer:
        ++atalk_stat[MACIP_SERVER_INFO_IN];
        if (!ipgp_server(ss, ig)) {
            ++atalk_stat[MACIP_SERVER_INFO_ERROR];
	    if (at_debug(atalkmacip_debug,0)) 
	        buginf("\nipgpAssign error");
        } else {
            ++atalk_stat[MACIP_SERVER_INFO_OUT];
	}
        break;
      case ipgpStatic:
        ++atalk_stat[MACIP_SERVER_STATIC_IN];
        if (!ipgp_static(ss, &cliddp, ig)) {
            ++atalk_stat[MACIP_SERVER_STATIC_ERROR];
            if (at_debug(atalkmacip_debug,0)) 
                buginf("\nAT-MacIP: ipgpAssign error");
        } else {
            ++atalk_stat[MACIP_SERVER_STATIC_OUT];
	}
        break;
      case ipgpRelease:
        ++atalk_stat[MACIP_SERVER_RELEASE_IN];
        /* no reply necessary: */
        if (!ipgp_release(ss, &cliddp, ig)) {
            ++atalk_stat[MACIP_SERVER_RELEASE_ERROR];
	    if (at_debug(atalkmacip_debug,0)) 
		buginf("\nAT-MacIP: ipgpRelease error");
        } else {
            ++atalk_stat[MACIP_SERVER_RELEASE_OUT];
	}
        ++atalk_stat[MACIP_SERVER_OUT];
        datagram_done(newpak);
        return;
      default:
        ++atalk_stat[MACIP_SERVER_BAD_OPCODE];
        if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
            buginf("\nAT-MacIP: bad IPGP op code, %d, from [%s]",
		   GETLONG(&ig->op), ddpAddrStr(&cliddp));
        PUTLONG(&ig->op, ipgpError);
        strcpy(ig->msg, IPGP_MSG_BAD_OP);
        break;
    }

    /* zero out unused fields: */
    PUTLONG(&ig->unused1, 0);
    PUTLONG(&ig->unused2, 0);
    PUTLONG(&ig->unused3, 0);
    PUTLONG(&ig->unused4, 0);

    /* set reply command/control field: */
    atp->ccf = 0;
    ATP_SET_FUNC(atp->ccf, ATP_TRESP);  /* this is a response */
    ATP_SET_EOM(atp->ccf);		/* set end-of-msg bit */

    /* bitmap: */
    atp->bitmap = 0;

    /* user bytes: */
    PUTSHORT(&atp->userbytes[0], MACIP_VERSION);    /* version number */
    atp->userbytes[2] = 0;
    atp->userbytes[3] = 0;

    atalk_pak_setdnet(newpak, cliddp.net);
    atalk_pak_setdnode(newpak, cliddp.node);
    atalk_pak_setdsock(newpak, cliddp.spec);

    atalk_pak_setsnet(newpak,ss->reqaddr.net);
    atalk_pak_setsnode(newpak,ss->reqaddr.node);
    atalk_pak_setssock(newpak,ss->reqaddr.spec);

    /* set packet length, checksum: */
    len = ATP_OVERHEADBYTES + ipgpMinSize + strlen(ig->msg) + 1;
    atalk_pak_setlength(newpak,len);
    atalk_pak_sethops(newpak, 0);		/* zero hop count */
    atalk_DDPcksum(newpak);

    /* force route lookup on output: */
    newpak->if_output = NULL;

    /* send the reply: */
    if (atalk_send(newpak)) {
        ++atalk_stat[MACIP_SERVER_OUT];
        if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
            buginf("\nAT: macip: atp_service: send OK");
    } else {
        ++atalk_stat[MACIP_SERVER_SEND_FAIL];
        if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
  	    buginf("\nAT: macip: atp_service: send failed to (%s) len=%d",
		   ddpAddrStr(&cliddp), len);
    }
}

/*
 * macip_empty_trash -
 *	
 *	maintenance routine to return excess unused storage.
 *	called by at_maint(), thru garbage collection table.
 *	
 */
boolean
macip_empty_trash (void)
{
    macipServerEvent *mse;

    while (QUEUESIZE(&mse_freeQ) >= MAX_MSE_FREELIST) {
	mse = dequeue(&mse_freeQ);
	free(mse);
    }
    return (TRUE);
}


/*************************  EXPORTED FUNCTIONS  ***********************/


/*
 * macip_process -
 *	this process routes all macip packets, and
 *	responds to Service requests for all macip servers.
 *	process forked from atalk_start() in atalk_input.c.
 *	
 */

static process
macip_process (void)
{
    server_rec	*ss = NULL;
    paktype	*pak;
    int		ii;

    /* wait for system initialization: */
    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    macipRouteQ = create_watched_queue("MacIP Route Queue", 0, 0);
    mgd_timer_init_parent(&macip_master_timer, NULL);
    mgd_timer_init_leaf(&macip_sched_time, &macip_master_timer,
			MACIP_TIMER_SCHED, NULL, FALSE);

    process_watch_queue(macipRouteQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&macip_master_timer, ENABLE);
    
    /* initialize schedule timer: */
    mgd_timer_start(&macip_sched_time, ONESEC);

    /* initialize server event free list queue: */
    queue_init(&mse_freeQ, 0);

    /*
     * SERVER LOOP
     */
    while (TRUE) {
	ulong major, minor;
	
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case TIMER_EVENT:
		mgd_timer_start(&macip_sched_time, ONESEC);
		if (!macip_initialized || !macip_configured)
		    break;
		if (macip_needs_service(FALSE, &ss))
		    if (ss != NULL) {
			/* ...call service routine for each server needing 
			 * service starting at <ss>, and reset <ss> to NULL:
			 */
			if (macip_needs_service(TRUE, &ss)) {
			    ;
			}
		    }
		break;
	    case QUEUE_EVENT:
		/* for each macip packet from DDP... */
		for (ii=0; ii < PAK_SWITCHCOUNT &&
		     !process_is_queue_empty(macipRouteQ); ii++) {
		    pak = process_dequeue(macipRouteQ);

		    /* if from IP... */
		    if (pak->linktype == LINK_IP)
			macip_to_DDP(pak);
		    else {
			/* from DDP - perform atalk dequeue checks: */
			if (pak && atalk_check(pak, DDP_TYPE_MACIP,
					       MACIP_DDP_BAD_DDP_TYPE, FALSE))
			    macip_to_IP(pak);
		    }
		}
		break;
	    default:
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
		break;
	    }
	}
    }
    /* NOTREACHED */
}
/*
 * macip_start -
 *	start the macip process if it's not already running
 */
void
macip_start (void)
{
    static boolean macip_started = FALSE;

    /* if process already running or macip not yet configured... */
    if (macip_started || !macip_configured)
	return;

    macip_started = TRUE;
    process_create(macip_process, "MacIP Service", NORMAL_STACK, PRIO_NORMAL);
}

/*
 * macip_enqueue
 *
 *      Enqueue a packet for macip
 *
 */
boolean macip_enqueue(paktype *pak)
{
    iphdrtype *ip;
    int port;

    ip = (iphdrtype *)ipheadstart(pak);
    port = (ALIAS_FLAGMASK & reg_invoke_ipalias_port(GETLONG(&ip->dstadr),0));
    
    if (port == MACIP_ALIASPORT) {
	/* a MacIP packet, enqueue on MacIP routing queue */
	if (macip_running) {
	    process_enqueue(macipRouteQ, pak);
	    return TRUE;
	}
    }
    return FALSE;
}

/*
 * macip_service -
 *	handle a service request for <ss>.
 *	process pending state machine events until exhausted.
 *	
 */
void
macip_service (server_rec *ss)
{
    PFV prevstate;
    macipServerEvent *ee;
    macipFsmEvent evt;

    if (!mdb_check_server(ss, "macip_service entry"))
        return;

    /* if NOT state machine event... */
    if (QUEUESIZE(&ss->stateQ) <= 0) {
        /* then just execute current state... */
        (*ss->stateFunc)(ss);
        return;
    }

    /* while state machine events... */
    while (QUEUESIZE(&ss->stateQ) > 0) {
        if (!mdb_check_server(ss, "macip_service loop"))
            break;

        ee = dequeue(&ss->stateQ);
        evt = ee->event;
	macip_free_server_event(ee);

        prevstate = ss->stateFunc;

        /* lookup new state: */
        ss->stateFunc = macip_next_state(ss, evt);

        /* let recursive states know about timeout events: */
        ss->timeout = (evt == MACIP_FSM_TIMEOUT);

        /* if new state or MACIP_FSM_TIMEOUT... */
        if (ss->stateFunc != prevstate || ss->timeout)
            TIMER_STOP(ss->timer);  /* ... then reset state timer: */

        if (at_debug(atalkmacip_debug,0))  {
            /* print state transition: */
            buginf("\nAT-MacIP:  STATE TRANSITION %lx:  %s(%s)==>%s",
                ss,
                macip_stateStr(prevstate),
                macip_eventStr(evt),
                macip_stateStr(ss->stateFunc));
        }

        /* process event: */
        if (macip_halt_state(ss->stateFunc)) {
            (*ss->stateFunc)(ss);
            break;      /* HALT state machine */
        }
        else
            (*ss->stateFunc)(ss);
    }
}

/*
 * macip_clearQs -
 *	clear the server request/confirm queues
 */
void
macip_clearQs (server_rec *ss)
{
    /* delete any pending request pkts: */
    if (process_queue_size(ss->reqQ) > 0 ) {
        paktype *pak;
        while ((pak  = process_dequeue(ss->reqQ)) != NULL)
            datagram_done(pak);
    }

    /* delete any pending confirm pkts: */
    if (process_queue_size(ss->confirmQ) > 0) {
        nbpQelem *ee;
        while ((ee = process_dequeue(ss->confirmQ)) != NULL)
            atnbp_event_free(ee);
    }

    /* just in case, delete pkts pending on macip routing Q: */
    if (process_queue_size(macipRouteQ) > 0 ) {
        paktype *pak;
        while ((pak  = process_dequeue(macipRouteQ)) != NULL)
            datagram_done(pak);
    }
}

/*
 * macip_clear_stateQ -
 *	clear the state queue
 */
void
macip_clear_stateQ (server_rec *ss)
{
    macipServerEvent *ee;

    if (at_debug(atalkmacip_debug,0)) { 
        if (QUEUESIZE(&ss->stateQ) > 0) {
buginf("\nAT-MacIP:>>UNPROCESSED state events, server %lx, curr=%s, prev=%s, n=%d",
            ss,
            macip_stateStr(ss->stateFunc),
            macip_stateStr(ss->prevStateFunc),
            QUEUESIZE(&ss->stateQ));
        }
    }

    /* delete remaining states: */
    while (QUEUESIZE(&ss->stateQ) > 0) {
        ee = dequeue(&ss->stateQ);

	if (at_debug(atalkmacip_debug,0))
            buginf("\nAT-MacIP: \tCLEAR EVENT<%s>, server %lx",
                macip_eventStr(ee->event), ss);

	macip_free_server_event(ee);
    }
}

/*
 * macip_shutdown -
 *	
 *	generate events to shutdown all macip state machines,
 *	and delete from database
 *	
 */
void
macip_shutdown (void)
{
    key *kk = &keystor;
    server_rec *ss;
    int sid;

    /* foreach server.... */
    for (sid = id_first(srvr_idh); sid != NILID;
        sid = id_next(srvr_idh, sid))
    {
        /*
         * servers: (short serverid, server_rec *ss)
         */

        db_mkkey(kk, 2, servers, sid);
        if (!DB_RETRIEVE((byteptr) &ss, kk, sizeof(ss))) {
            continue;
        }

        /* ...kill it dead: */
        macip_jam_event(ss, MACIP_FSM_DEL_SERVER, 0);
        macip_service(ss);  /* do it now! */
    }
}

/*
 * macip_zone_seeded -
 *	
 *	events to shutdown all macip state machines,
 *	delete from database
 *	
 */
boolean
macip_zone_seeded (server_rec *ss)
{
    str32 zz;

    str32init(&zz, SY_STRING(ss->zone));

    return(pickIDBinZone(&zz, NULL) != NULL); /* from nbpapi.h */
}

/*
 * macip_start_server -
 *	
 *	Begin server operations:
 *	- alias server IP address if necessary
 *	- register server IP address <iaddr>:IPADDRESS@<zone> on MacIP socket
 *	- open socket to listen for ATP requests
 *	- register server as <iaddr>:IPGATEWAY@<zone> on this socket
 *	
 */
boolean
macip_start_server (server_rec *ss)
{
    static nbpEntity nve1, nve2;	/* save stack space */
    nbpEntity	*addrnve = &nve1, *gwnve = &nve2;
    nbp_result	err;
    ataddress	src;
    str32	zz;

    str32init(&zz, SY_STRING(ss->zone));

    if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
        buginf("\nAT: macip_start_server, server %i, zone '%s'",
	       ss->ipaddr, at_hex_display(SY_STRING(ss->zone)));

    ss->ipidb = ipaddr_findidb(ss->ipaddr);
    if (ss->ipidb == NULL) {
	errmsg(AT_ERR_MACIPNOCABLE,
		at_hex_display(SY_STRING(ss->zone)), ss->ipaddr);
        return (FALSE);
    }

    if (macip_ip_down(ss)) {
	if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: macip: %s: IP interface down, %i",
		   at_hex_display(SY_STRING(ss->zone)), ss->ipaddr);
        return (FALSE);
    }

    /* select our ddp interface to be same as IP if possible: */
    ss->ddpidb = pickIDBinZone(&zz, atalk_getidb(ss->ipidb));
    if (ss->ddpidb == NULL) {
	if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: macip: macip_start_server:  no DDP interface, %s!",
		   at_hex_display(SY_STRING(ss->zone)));
	ss->ipidb = NULL;
        return (FALSE);
    }

    /* add alias for server address if necessary: */
    if (ss->ipaddr != idb_sw_get_ipaddress(ss->ipidb)) {
        if (!reg_invoke_ipalias_insert(ss->ipaddr, MACIP_ALIASPORT, NULL)) {
	    if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
		buginf("\nAT: macip_start_server:  can't alias server IP, %i",
		       ss->ipaddr);
	    ss->ipidb = NULL;
	    ss->ddpidb = NULL;
            return (FALSE);
        }
    }

    /*
     * register IPADDRESS on MacIP routing socket:
     *
     * NOTE:  we don't care if another macip server is already
     * bound to this socket, as "routing queue" is not used.
     * However, NBP code requires a binding, and who knows,
     * maybe we will use it someday...
     *
     * Also note that we clear this queue periodically...
     */
    src.node = atalkif_MyNode(ss->ddpidb);
    src.net  = atalkif_MyNet(ss->ddpidb);
    src.spec = ATALK_SOCK_MACIP;
    /* if magic macip skt not already bound.... */
    if (!atalk_IsBoundSocket(src.spec)) {
        /* ...then bind it now: */
        if (!atalk_BindSocket(macipRouteQ, "MacIP Routing", src.spec, NULL)) {
            at_err(MODID,"macip_start_server:  can't bind skt %d", src.spec);
	    ss->ipidb = NULL;
	    ss->ddpidb = NULL;
            return (FALSE);
        }
    }

    /* create name, type, zone: */
    atnbp_mkname(addrnve,
		 ipaddrStr(ss->ipaddr), "IPADDRESS", SY_STRING(ss->zone));
    err = atnbp_register(addrnve, &src, TRUE);   /* async */
    if (err != NBP_REG_PENDING) {
	if (at_debug(atalkmacip_debug, ATALK_NULL_MASK))
	    buginf("\nAT: macip:  couldn't register '%s':  %s",
		   atnbp_entityStr(addrnve), atnbp_resultStr(err));
	ss->ipidb = NULL;
	ss->ddpidb = NULL;
        return (FALSE);
    } else {
	if (at_debug(atalk_events, ATALK_NULL_MASK))
	    errmsg(AT_ERR_REGPENDING, atnbp_entityStr(addrnve),
		   ddpAddrStr(&src));
    }
    /* IPADDRESS registration started */

    /*
     * Register MacIP IPGATEWAY Service:
     */

    /* if no sockets available in dynamic range... */
    if ((ss->reqaddr.spec = atalk_SelectSocket()) == 0) {
        at_err(MODID,"macip_start_server: no dynamic sockets available!");
        atnbp_unregister(addrnve);
	ss->ipidb = NULL;
	ss->ddpidb = NULL;
        return (FALSE);
    }
    if (!atalk_BindSocket(ss->reqQ, "MacIP Service", ss->reqaddr.spec, NULL)){
        at_err(MODID,"macip_start_server: couldn't bind skt %d!",
                ss->reqaddr.spec);
        atnbp_unregister(addrnve);
	ss->ipidb = NULL;
	ss->ddpidb = NULL;
        return (FALSE);
    }

    /* set ddp interface address: */
    ss->reqaddr.node = atalkif_MyNode(ss->ddpidb);
    ss->reqaddr.net = atalkif_MyNet(ss->ddpidb);

    /* create name, type, zone: */
    atnbp_mkname(gwnve, ipaddrStr(ss->ipaddr), "IPGATEWAY",
		 SY_STRING(ss->zone));

    err = atnbp_register(gwnve, &ss->reqaddr, TRUE);
    if (err != NBP_REG_PENDING) {
        at_err(MODID,"MacIP:  couldn't register '%s':  %s",
	       atnbp_entityStr(gwnve), atnbp_resultStr(err));
        /* back out: */
        reg_invoke_ipalias_delete(ss->ipaddr);
        atnbp_unregister(addrnve);
        atalk_UnBindSocket(ss->reqaddr.spec);
        macip_clearQs(ss);
	ss->ipidb = NULL;
	ss->ddpidb = NULL;

        return (FALSE);
    } else {
	if (at_debug(atalk_events, ATALK_NULL_MASK))
	    errmsg(AT_ERR_REGPENDING, atnbp_entityStr(gwnve),
		   ddpAddrStr(&ss->reqaddr));
    }
    /* IPGATEWAY registration started */

    return (TRUE);
}

/*
 * macip_client_search -
 *	
 *	initiate client search, finish startup tasks.
 *	
 */
boolean
macip_client_search (server_rec *ss)
{
    static nbpEntity	clinve;		/* save stack space */
    static nbptimer	tm;
    nbp_result		err;
    ataddress		src;

    /*
     * Try to find already registered clients in my address
     * assignment space.  replies are handled similarly to
     * replies from confirm requests, except unknown clients are
     * automatically entered in database.
     */

    atnbp_mkname(&clinve, "=", "IPADDRESS", SY_STRING(ss->zone));
    tm.maxrequests = MAX_CLIENT_REQUESTS;
    tm.maxreplies = MAX_CLIENT_REPLIES;
    tm.interval = DEF_INTERVAL;
    src.net  = ss->reqaddr.net;
    src.node = ss->reqaddr.node;
    src.spec = ATALK_SOCK_NBP;
    err = atnbp_find(&clinve, &src, &tm, ss->confirmQ);
    if (err != NBP_OK) {
        at_err(MODID,"macip_client_search failed: %s", atnbp_resultStr(err));
        return (FALSE);
    }

    /* mark this server as up: */
    ss->serverUP = TRUE;
    /* update global server-up count: */
    macip_up_servers = mdb_up_servers();
    macip_running = (macip_up_servers > 0);

    /* log message: */
    errmsg(AT_ERR_MACIPUP, at_hex_display(SY_STRING(ss->zone)), ss->ipaddr);

    return (TRUE);
}

/*
 * macip_stop_server -
 *	
 *	Halt server operations:
 *	- de-register server name
 *	- close ATP listen socket
 *	- close pending NBP requests
 *	- purge pending requests/confirm queues
 *	- delete IP alias for server
 *	- mark server as down
 *	
 */
void
macip_stop_server (server_rec *ss)
{
    nbpEntity *nve = &global_nve;

    if (at_debug(atalkmacip_debug,0))
        buginf("\nAT-MacIP: macip_stop_server, server %i, zone '%s'",
	       ss->ipaddr, at_hex_display(SY_STRING(ss->zone)));

    /* cancel any pending nbp requests: */
    atnbp_cancel(ss->confirmQ);

    /* unregister IPADDRESS: */
    atnbp_mkname(nve, ipaddrStr(ss->ipaddr), "IPADDRESS", SY_STRING(ss->zone));
    atnbp_unregister(nve);

    /* unregister IPGATEWAY: */
    atnbp_mkname(nve, ipaddrStr(ss->ipaddr), "IPGATEWAY", SY_STRING(ss->zone));
    atnbp_unregister(nve);

    /* shut down request socket: */
    atalk_UnBindSocket(ss->reqaddr.spec);

    /* clear anything pending on input queues: */
    macip_clearQs(ss);

    /* delete server alias if installed: */
    reg_invoke_ipalias_delete(ss->ipaddr);

    /* mark interfaces as down: */
    ss->ddpidb = NULL;
    ss->ipidb = NULL;

    /* if server was operational, log message: */
    if (ss->serverUP)
	errmsg(AT_ERR_MACIPDOWN,
	       at_hex_display(SY_STRING(ss->zone)), ss->ipaddr);

    /* mark this server as down: */
    ss->serverUP = FALSE;
    /* update global server-up count: */
    macip_up_servers = mdb_up_servers();
    macip_running = (macip_up_servers > 0);
}

/*
 * macip_ddp_down -
 *	check our DDP interface, zone configuration.  if zone or
 *	interface is down return false, o'wise true.
 */
boolean
macip_ddp_down (server_rec *ss)
{
    nbpEntity *nve = &global_nve;

    /* check to see if our server is still registered: */
    atnbp_mkname(nve, ipaddrStr(ss->ipaddr), "IPGATEWAY", SY_STRING(ss->zone));

    /*
     * NOTE:  this catches everything because all entities attached
     * to an interface are de-registered whenever the interface
     * flops due to any reason, including config changes or hardware
     * errors.
     */
    return (atnbp_isregistered(nve) != NBP_OK); /* unregistered? */
}

/*
 * macip_ip_down -
 *	true if IP interface is no longer available due to
 *	a config change or a down interface
 */
boolean
macip_ip_down (server_rec *ss)
{
    idbtype *idb = ss->ipidb;

    return(idb != ipaddr_findidb(ss->ipaddr) || !interface_up(idb));
}

/*
 * macip_confirm_clients -
 *	send confirms requests to current clients
 */
void
macip_confirm_clients (server_rec *ss)
{
    key		*kk = &keystor;
    ushort	cid;			/* client enum */
    client_rec	*cptr;
    sys_timestamp wakeup_time;

    /* check for confirm replies one last time before timing out clients: */
    macip_client_service(ss);

    /* foreach client of this server.... */
    for (cid = id_first(ss->ncli_idh); cid != NILID;
	 cid = id_next(ss->ncli_idh, cid)) {
        /* server_client:  (serverid, enum, client_rec *cptr) */
        db_mkkey(kk, 3, server_client,ss->serverid, cid);
        if (!DB_RETRIEVE((byteptr) &cptr, kk, sizeof(cptr))) {
            at_err(MODID,"macip_confirm: no client record, sid=%d #%d",
		   ss->serverid, cid);
            continue;			/* SKIP */
        }

        /* if timed-out... */
	wakeup_time = TIMER_ADD_DELTA(cptr->timer, CLIENT_TIMEOUT);
        if (!SLEEPING(wakeup_time)) {
            /* ...purge the client: */
            if (at_debug(atalkmacip_debug,0))
                buginf("\nAT-MacIP: CLIENT TIMEOUT %i@[%s]",
		       cptr->ipaddr,
		       ddpAddrStr(&cptr->ddpaddr));
            mdb_purge_client(ss, cptr->ncli);
        } else {
	    wakeup_time =
		TIMER_ADD_DELTA(cptr->timer, (NBP_CONFIRM_TIME/2));
	    if (!SLEEPING(wakeup_time)) {
		/* send a confirm unless recent packet activity */
		macip_send_confirm(ss, cptr);
	    }
	}
    }
}

/*
 * macip_client_service -
 *	service the client macip queues
 */
void
macip_client_service (server_rec *ss)
{
    nbpQelem	*ee;
    paktype	*pak;
    /*
     * service any pending confirm replies:
     * NOTE:  do this before atp, so we don't mis-allocate
     * existing clients on startup.
     */
    if (process_queue_size(ss->confirmQ) > 0) {
        while ((ee = process_dequeue(ss->confirmQ)) != NULL)
            macip_nbp_service(ss,ee);
    }

    /* service any pending atp request pkts: */
    if (process_queue_size(ss->reqQ) > 0 ) {
        while ((pak = process_dequeue(ss->reqQ)) != NULL)
            macip_atp_service(ss, pak);
    }
}

/*
 * macip_show_clients -
 *	the client macip queues
 */
void
macip_show_clients (void)
{
    mdb_show_clients();
}

/*
 * macip_show_servers -
 *	dump the current macip database
 */
void
macip_show_servers (void)
{
    mdb_show_servers();
}

/*
 * macip_show_traffic -
 *	show macip stats.  we assume the stats are contiguous
 */
void
macip_show_traffic (void)
{
    int		ii;
    const char	ban[] = " -- MACIP Statistics";

    automore_header(ban);
    for (ii = MACIP_DDP_IN; ii <= MACIP_SERVER_BAD_OPCODE ; ii++) {
        if (atalk_stat[ii] == 0)
            continue;			/* show only non-zero stats */
        /* NOTE: printf alignment opposite of K&R! */
        printf("\n%-30s: %-8ld", atalk_statStr(ii), atalk_stat[ii]);
    }
    automore_header(NULL);
}

macipServerEvent *
macip_get_server_event(void)
{
    macipServerEvent	*mse;

    if (QUEUEEMPTY(&mse_freeQ))
	mse = malloc(sizeof(macipServerEvent));
    else
	mse = dequeue(&mse_freeQ);

    return(mse);
}

/*
 * macip_free_server_event -
 *	return a macip server event record to free list, schedule
 *	garbage collection if needed.
 *	
 */
void
macip_free_server_event (macipServerEvent *mse)
{
    enqueue(&mse_freeQ, mse);

    if (QUEUESIZE(&mse_freeQ) > MAX_MSE_FREELIST)
	atmaint_ScheduleGCRun(macip_gccontext);
}
