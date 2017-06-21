/* $Id: xtacacs.c,v 3.10.6.8 1996/08/28 13:09:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/servers/xtacacs.c,v $
 *------------------------------------------------------------------
 * xtacacs.c - Extended TAC access protocol support
 *
 * July 1990, Bill Westfield
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xtacacs.c,v $
 * Revision 3.10.6.8  1996/08/28  13:09:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.6.7  1996/07/30  04:22:29  che
 * CSCdi62108:  authentication pitfall with XTACACS/PAP
 * Branch: California_branch
 * Check access lists returned from xtacacs.  Check return value of
 * reg_invoke_login_slipon() in ipcp_up().
 *
 * Revision 3.10.6.6  1996/06/28  23:25:49  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.10.6.5  1996/06/01  01:08:12  carrel
 * CSCdi51262:  xslipoff not sent when the line is renegociated for ppp
 * Branch: California_branch
 * Make SLIPON and SLIPOFF message always come from the same subsystem.
 *
 * Revision 3.10.6.4  1996/05/21  10:01:42  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.6.3  1996/05/04  01:37:56  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.10.6.2  1996/04/19  15:29:44  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.8  1996/04/09  21:45:30  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.7  1996/03/19  08:33:12  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.6  1996/02/28  06:15:15  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 2.1.6.5  1996/02/21  03:59:18  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.4  1996/01/23  06:32:49  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.3  1995/12/19  04:13:15  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/12/17  02:16:29  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  17:39:47  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.10.6.1  1996/03/18  21:47:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.2  1996/03/16  07:28:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.2.1  1996/03/07  10:42:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.10  1996/03/05  01:21:28  billw
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Additional commenting...
 *
 * Revision 3.9  1996/02/23  10:38:14  billw
 * CSCdi49461:  autoselect during-login causes premature authorization
 * Define a new flag AUTOSELECT_AUTHORIZE that survives across death().
 * Set it at the same time AUTOSELECTED is set, but only clear it when
 * a new autoselect starts.
 *
 * Revision 3.8  1996/01/22  06:59:17  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1995/12/19  00:40:58  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.6  1995/12/17  18:36:50  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/13  22:57:53  lol
 * CSCdi39791:  access-lists applied by tacacs+ dont work when
 * fast-switched
 *
 * Revision 3.4  1995/11/29  01:14:31  lol
 * CSCdi42554:  SLIPOFF is missing information when accounting vty-async
 *
 * Revision 3.3  1995/11/22  22:38:30  che
 * CSCdi42049:  TACACS failures when using enable password privledge levels
 * Added new parameter to xtacacs_enable() to specify whether we're
 * doing an "ask" or a "notify".
 *
 * Revision 3.2  1995/11/17  18:52:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/10/19  21:17:26  che
 * CSCdi36726:  tacacs login causes crash
 * Lock the memory location of the Tacacs server we're trying to contact
 * so it doesn't get freed up while we're waiting for a response.
 *
 * Revision 2.7  1995/09/11  19:00:15  che
 * CSCdi38921:  Further TACACS sanity checks
 * Checks to make sure that the TACACS response packet we received
 * originated from a TACACS server in our server list.
 *
 * Revision 2.6  1995/08/23  18:49:30  eschaffe
 * CSCdi37678:  cant use per user access lists on vtyasync translate w/o
 * login
 *
 * Revision 2.5  1995/08/11  06:46:16  billw
 * CSCdi35591:  spurious xtacacs packets are sent with aaa new-model
 *
 * Revision 2.4  1995/08/08  16:48:44  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.3  1995/07/08 02:21:35  irfan
 * CSCdi28744:  memory corruption crashes while using xtacacs
 * Uninitialized ta_blocktypes entries in tacacs queues cause
 * overwrites to unallocated and low memory. While at it, catch
 * and return some allocated packet buffers too.
 *
 * Revision 2.2  1995/06/20  16:11:33  lol
 * CSCdi34401: Tacacs notif.type 6-ena and 10-slipoff not sent to
 *             nondefault server
 *
 * Revision 2.1  1995/06/07  22:44:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *                               Action
 *                               ------
 *              login   login@lhost     login           login@lhost
 *              slip    slip            slip@shost      slip@shost
 * Paktype
 * ------
 * login        def     lhost           def             lhost
 * logout       def     lhost           def             lhost
 * slipon       def     lhost           shost           shost
 * slipoff      def     lhost           shost           shost
 * slipaddr     def     lhost           shost           shost
 * enable       def     def             def             def
 *
 * Lol -- 6/16/95
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/udp_debug.h"
#include "../os/os_debug_flags.h"
#include "ttysrv.h"
#include "aaa.h"
#include "tacacs_plus.h"
#include "tacacs.h"
#include "logger.h"
#include "../servers/msg_tacacs.c"	/* Not a typo, see logger.h */
#include "connect.h"
#include "login.h"
#include "../wan/ppp.h"
#include "../parser/parser_actions.h"
#include "../parser/parser_defs_autoselect.h"
#include "../util/random.h"
#include "../ip/ip_registry.h"

/*
 * Static local functions
 */
static boolean skip_slipon_slipoff(boolean, tt_soc *, hwidbtype *);
hwidbtype *authenticatingidb;

void xtacacs_enable_ext (void)
{
    if (!(tacacs_extended & TACACS_EXTEND)) {
	/* Only send RELOAD if extended mode WASN'T previously set. */
	tacacs_extended |= TACACS_EXTEND;
	xtacacs_reload();
    }
}

void xtacacs_setask (parseinfo *csb, int mask, boolean always, boolean get_acl)
{
    if (csb->sense) {
        tacacs_extended |= mask;
	if (always)
	    tacacs_always |= mask;
	else
	    tacacs_always &= ~mask;
	if (get_acl)
	    tacacs_extended |= TACACS_GETSLIPACL;
	else
	    tacacs_extended &= ~TACACS_GETSLIPACL;
    } else {
        tacacs_extended &= ~mask;
	tacacs_always &= ~mask;
	tacacs_extended &= ~TACACS_GETSLIPACL;
    }
    if (mask == TACACS_ASKENA)		/* set enable use-tacacs properly */
	enable_use_login_server = csb->sense;
}

void xtacacs_setnote (parseinfo *csb, int mask, boolean always)
{
    if (csb->sense) {
        tacacs_extended |= mask;
	if (always)
	    tacacs_always |= mask;
	else
	    tacacs_always &= ~mask;
    } else {
        tacacs_extended &= ~mask;
	tacacs_always &= ~mask;
    }
}

static char *
xtacacs_type_string(int type)
{
    switch(type) {
    case XTA_LOGIN: 
	return("LOGIN");
    case XTA_ANSWER: 
	return("ANSWER");
    case XTA_CHANGE: 
	return("CHANGE");
    case XTA_FOLLOW: 
	return("FOLLOW");
    case XTA_CONNECT: 
	return("CONNECT");
    case XTA_ENABLE: 
	return("ENABLE");
    case XTA_LOGOUT: 
	return("LOGOUT");
    case XTA_RELOAD: 
	return("RELOAD");
    case XTA_SLIPON: 
	return("SLIPON");
    case XTA_SLIPOFF: 
	return("SLIPOFF");
    case XTA_SLIPADDR: 
	return("SLIPADDR");
    case XTA_ARAP_AUTH: 
	return("ARAP_AUTH");
    case XTA_CHAP_AUTH: 
	return("CHAP_AUTH");
    default: 
	return("UNKNOWN");
    }
}


/*
 * xtacacs_server
 * server for extended tacacs packets
 */

void xtacacs_server (paktype *pak)
{
    iphdrtype *ip = (iphdrtype *)ipheadstart(pak);
    udptype *udp = (udptype *)((char *)ip + (ip->ihl << 2));
    xtacacstype *tac = (xtacacstype *)((uchar *)udp + UDPHEADERBYTES);
    ta_blocktype *block;
    servtype *lp, *lp2;

    if (udp_debug || tacacs_debug)
	buginf("\nTAC: received extended %s (%d) (%d) from %i, via %s  Id %d",
	       xtacacs_type_string(tac->type),
	       tac->type, tac->response,
	       ip->srcadr, pak->if_input->namestring, tac->trans);

    switch (tac->type) {
      case XTA_ANSWER:
	/*
	 * Try to find the transaction on the request queue
	 */
	for (block = (ta_blocktype *)tacacsreqQ.qhead; block;
	    block = block->next)
	    if (tac->trans == block->trans)
		break;
	/* 
	 * If transaction not on request queue, give it to the logger 
	 */
	if (block == NULL) {
	    tacacs_acknowledgement(pak, tac);
	    break;
	}
	/*
	 * If it is on the request queue, check and make sure that an
	 * answer has not already been received.  If we already have an 
	 * answer, then bail.
	 */
	if (block->done)
	    break;

	/*
	 * If not already answered, check to make sure the packet
	 * came from a valid TACACS server.  If it didn't, then bail.
	 */
	lp2 = (servtype *)tacacshostQ.qhead;
	if (ip->srcadr) {
	    while(lp2) {
		if (lp2->addr == ip->srcadr)
		    break;
		else
		    lp2 = lp2->next;
	    }
	} else {
	    lp2 = NULL;
	}
	if (lp2 == NULL) {
	    if (udp_debug || tacacs_debug) {
		buginf("\nTAC: Received bogus response from %i.  " 
		       "Discarding...",ip->srcadr);
	    }
	    break;
	}

	/*
	 * If the address is valid, put pointer to packet in the 
	 * block on queue, lock the packet, set the tac pointer, mark 
	 * the block as done and bail.
	 */
	block->pak = pak;
	pak_lock(pak);
	pak->unspecified[0] = (ulong) tac;
	pak->unspecified[1] = (ulong) (XTACACS_SIZE + tac->namelen +
				       tac->pwlen);
	block->done = TRUE;
	break;
    case XTA_CHANGE:
    case XTA_FOLLOW:
	for (block = (ta_blocktype *)tacacsreqQ.qhead; block;
	    block = block->next)
	    if (tac->trans == block->trans)
		break;
	if (block == NULL)
	    break;
	GET_TIMESTAMP(block->timeout);  /* Set to timeout */
	for (lp = (servtype *)tacacshostQ.qhead; lp; lp = lp->next)
	    if (ip->srcadr == lp->addr)
		break;
	if (lp == NULL)
	    break;
	/*
	 * Add it to head or end
	 */
	unqueue(&tacacshostQ, lp);
	if (tac->type == TA_CHANGE)
	    enqueue(&tacacshostQ, lp);
	else
	    requeue(&tacacshostQ, lp);
	break;

      default:
	break;
    }
}

/*
 * tacacs_resend
 * Do retransmission of tacacs messages
 * Called by SERVICE_ONESEC
 */

void tacacs_resend (void)
{
    servtype *lp;
    uint start, len;
    ta_blocktype *block, *bp;
    paktype *pak;
    iphdrtype *ip;

    for (bp = (ta_blocktype *)tacacslogQ.qhead; bp; ) {
	block = bp;
	bp = bp->next;
	if (!AWAKE(block->nexttime))
	    continue;	/* not ready, try next one */
	ip  = (iphdrtype *)ipheadstart(block->pak); 

	if (AWAKE(block->timeout)) {
	    errmsg(&msgsym(SENDTMO, TAC),
		   ((xtacacstype *)block->pak->unspecified[0])->type,
		   ip->dstadr);
	    unqueue(&tacacslogQ, block);
	    datagram_done(block->pak);
	    free(block);
	} else {
	    for (lp = (servtype *)tacacshostQ.qhead; lp; lp = lp->next) {
		if (ip->dstadr == lp->addr)
		    break;
	    }
	    if (lp) lp = lp->next;   /* try next server */
	    if (!lp) lp = (servtype *) tacacshostQ.qhead;
	    len = block->pak->unspecified[1];
	    if (lp && (pak = setup_ipsocket(lp->sock, len, &start))) {
		pak_lock(pak);
		pak->unspecified[0] = (ulong)(pak->data_area + start);
		pak->unspecified[1] = len;
		bcopy((uchar *)block->pak->unspecified[0],
		      (uchar *)pak->unspecified[0], len); 
		datagram_done(block->pak);
		block->pak = pak;
		/*
		 * For backwards compatibility, if the tacacs_timeout is set
		 * to 5 seconds (which is the default) then we will use the
		 * old default value of 30 seconds for this timer.
		 */
		TIMER_START(block->nexttime, (tacacs_timeout == 5*ONESEC) ?
			    30*ONESEC : tacacs_timeout);
		write_ipsocket(lp->sock, pak, len);
		if (udp_debug || tacacs_debug) {
		    int type;
		    type = ((xtacacstype *) block->pak->unspecified[0])->type;
		    buginf("\nTAC: retransmit type %s (%d) to %i, Id %d",
			   xtacacs_type_string(type),
			   type, lp->addr, block->trans);
		}
	    } else {
		/* short wait for resources */
		TIMER_START(block->nexttime, 2 * ONESEC);
	    }
	}
    }
}

/*
 * tacacs_acknowledgement
 * Given an incoming reply, see if it is for a request handled by the
 * tacacs_resend().  Only responses where we don't care about
 * the actual return value are handled by tacacs_resend().
 */
void tacacs_acknowledgement (paktype *pak, xtacacstype *tac)
{
    iphdrtype *ip;
    ta_blocktype *block;

    ip = (iphdrtype *)ipheadstart(pak);

    for (block = (ta_blocktype *)tacacslogQ.qhead; block;
	 block = block->next)
	if (tac->trans == block->trans)
	    break;
    if (block == NULL) {
	errmsg(&msgsym(UNEXREP, TAC), tacacslogQ.count);
	return;
    }

    unqueue(&tacacslogQ, block);
    if (udp_debug || tacacs_debug) {
 	int type = ((xtacacstype *)block->pak->unspecified[0])->type;
 	buginf("\nTAC: ack received for type (%s) %d to %i, Id %d",
 	       xtacacs_type_string(type), type, ip->dstadr, block->trans);
    }

    if (block->pak)
      datagram_done(block->pak);
    free(block);
}

/*
 * xtacacs_data_setup
 * Set up an extended tacacs packet - fill in most fields.
 */
paktype *xtacacs_data_setup (
    char *name,
    void *pw,
    int pw_len,
    int type,
    int ttynum,
    int minor_ver)
{
    paktype *pak;
    servtype *lp;
    xtacacstype *tac;
    uint namelen, pwlen, start, count;
    uchar *p;

    if (!login_have_servers || ((lp = (servtype *)tacacshostQ.qhead) == NULL))
	return(NULL);
    if (!old_access_control)
	return(NULL);
    namelen = strlen(name);
    pwlen = pw_len;
    count = 0;
    pak = setup_ipsocket(lp->sock, XTACACS_SIZE+namelen+pwlen, &start);
    if (pak) {
	tac = (xtacacstype *)(pak->data_area + start);
	pak->unspecified[0] = (ulong) tac;
	pak->unspecified[1] = XTACACS_SIZE + namelen + pwlen;
	tac->version = XTA_VERSION | (minor_ver & XTA_MINOR_VERSION_MASK);
	tac->type = type;
	tac->namelen = namelen;
	tac->pwlen = pwlen;
	if (ttynum == -1) {
	    /*
	     * No tty - see if the global "authenticating interface" is set
	     */
	    if (authenticatingidb)
		tac->lport = 
		    idbname2portval(authenticatingidb->hw_short_namestring);
	} else {
	    tac->lport = ttynum;
	}
	if (stdio->conntop)
	  tac->dhost = stdio->conntop->address.ip_addr;
	else
	  tac->dhost = 0;
	    
	p = (uchar *)tac + XTACACS_SIZE;
	if (name) {
	    bcopy(name, p, namelen);
	    p += namelen;
	}
	if (pw)
	    bcopy(pw, p, pwlen);
    }
    return(pak);
}

/*
 * xtacacs_ask
 * Send packet to tacacs server, and wait for a reply.
 * If addr is set, ask the server at that address.
 */

boolean xtacacs_data_ask_addr (
    paktype *pak,
    char **reason,
    ipaddrtype addr,
    void *data,
    int *recv_len)
{
    paktype *nip;
    iphdrtype *ip;
    servtype *lp;
    xtacacstype *tac = NULL;
    uint start, count;
    ta_blocktype block;
    boolean login = FALSE;
    uchar saved_type = 0;

    /*
     * If TAC service not available, then claim okay.
     */
    if (!login_have_servers) {	/* If ip, free it before bailing */
	if (pak)
	    datagram_done(pak);
	return(FALSE);
    }
    /*
     * If not hosts in queue then refuse access
     */
    lp = (servtype *)tacacshostQ.qhead;
    if (addr) {
        while (lp) {
	    if (lp->addr == addr)
	        break;
	    else lp = lp->next;
	}
    }
    if (lp == NULL || pak == NULL) {
	if (pak)
	    datagram_done(pak);
	return(FALSE);
    }
/*
 * Zero retrans counter, setup block, and stick block on the request queue.
 */
    count = 0;
    block.trans = random_gen() & 0xFFFF;
    block.pak = NULL; block.next = NULL;
    block.response = TA_A_NONE;
    block.done = FALSE;
    enqueue(&tacacsreqQ, &block);

    do {
	nip = setup_ipsocket(lp->sock, pak->unspecified[1],
			     &start);		/* Get packet to setup send to server */
	if (nip == NULL) 
	    break;
	tac = (xtacacstype *)(nip->data_area + start); /* Point to tacacs area */
	/* 
	 * copy.  pak->unspec[0] is pointer, unspec[1] is count 
	 */
	bcopy((uchar *)pak->unspecified[0], tac, pak->unspecified[1]);
	nip->unspecified[0] = (ulong) tac;
	nip->unspecified[1] = pak->unspecified[1];
	saved_type = tac->type;
	/*
	 * Free the original packet passed in, then point ip at the copy.
	 */
	datagram_done(pak);
	pak = nip;
	ip = (iphdrtype *)ipheadstart(pak);
	login = tac->type == XTA_LOGIN;	/* Are we doing a login?? */
	/*
	 * Set transaction ID, set block not done, set block timeout.
	 */
	tac->trans = block.trans;
	block.done = FALSE;
	TIMER_START(block.timeout, tacacs_timeout);
	/*
	 * Increment refcount on packet, so it isn't lost, then write it.
	 */
	pak_lock(pak);
	mem_lock(lp);
	write_ipsocket(lp->sock, pak, pak->unspecified[1]);
	if (udp_debug || tacacs_debug) {
	    int type = ((xtacacstype *)pak->unspecified[0])->type;
	    buginf("\nTAC: Send query type %s (%d) to %i, Id %d, ver 0x%X, port %d",
		   xtacacs_type_string(type), type,
		   ip->dstadr, block.trans,
		   (int)((xtacacstype *)pak->unspecified[0])->version, tac->lport);
	}
 	/*
 	 * Sleep until we get reply or timeout.
 	 */
	edisms((blockproc *)tacacsBLOCK, (ulong)&block);
	if (addr) { /* If specific server, bail if retrans limit hit */
	    if (count++ >= tacacs_retrans) {
		free(lp);
	        break;
	    }
	} else {
	    /*
	     * Point to next tacacs server.  If no more servers, then bail 
	     * if hit retrans limit, else go back to head of server list.
	     * If the current server was freed from the server list while
	     * we were sending/receiving IP packets, go back to head of
	     * server list.
	     */
	    if (lp->addr != (ipaddrtype) 0) {
		/* All free() is doing is decrementing the ref count. */
		free(lp);
		lp = lp->next;
	    } else {
		free(lp);
		lp = NULL;
	    }

	    if (lp == NULL) {
		if ((count++ >= tacacs_retrans) || (!login_have_servers)) {
		    break;
		} else {
		    lp = (servtype *)tacacshostQ.qhead;
		}
	    }
	}
    } while (!block.done);
    /*
     * Either we got a response, or we timed out.  If we have a packet,
     * free it, then point to the packet attached to that block.
     * First unqueue the block, so that replies can't come in after
     * we've decided there is no reply!
     */
    unqueue(&tacacsreqQ, &block);

    if (pak)
	datagram_done(pak);
    pak = block.pak;
    if (pak) {
	
	tac = (xtacacstype *) pak->unspecified[0]; /* set local tac ptr */
	/*
	 * If the local port is a valid line and we have a tt_soc ptr
	 * for it, then if the response was positive, then set the user
	 * ID, and if an access list was returned and we are doing a
	 * login, then set the outgoing access class for this line.
	 */
	if ((tac->lport < MAXLINES) && (MODEMS[tac->lport])) {
	    if (tac->response == TA_A_ACCEPTED) {
		MODEMS[tac->lport]->uuid = tac->uuid;
		if ((tac->accesslist) && (login))
		    MODEMS[tac->lport]->accessout = tac->accesslist;
	    }
	}
	/* 
	 * Set response and reason (if available), then free the packet.
	 */
	block.response = tac->response;
	if (reason)
	    *reason = tacacs_whytext(tac);
	if (data && recv_len) {		/* Are we expecting data back */
	    if ((tac->version & XTA_MINOR_VERSION_MASK) == 0) {
		if (tacacs_debug)
		    buginf("\nTAC: Received packet w/o minor number.");
		*recv_len = 0;		/* but sender is old version */
	    } else if (tac->namelen != 0) {
		if (tacacs_debug)
		    buginf("\nTAC: Received invalid sized packet. (name)");
		*recv_len = 0;
	    } else if(tac->pwlen != *recv_len) {
		if (tacacs_debug)
		    buginf("\nTAC: Received unexpected sized packet."
			   "\n        expected len = %d, received len = %d",
			   XTACACS_SIZE + *recv_len,
			   XTACACS_SIZE + tac->pwlen);
		*recv_len = 0;
	    } else {
		bcopy(((char *)tac) + XTACACS_SIZE, data, *recv_len);
	    }
	}
	datagram_done(pak);
    } else /* No response packet, must have timed out.  Use last resort */
	block.response = tacacs_fallback(saved_type, reason) ? TA_A_ACCEPTED:0;

    /* 
     * Return true if accepted, false otherwise.
     */
    return (block.response == TA_A_ACCEPTED ? TRUE : FALSE);
}

boolean xtacacs_ask (paktype *pak, char **reason)
{
  return xtacacs_ask_addr (pak, reason, 0);
}


/*
 * xtacacs_note
 * Send packet to server, and set up for retransmission by logger process.
 * Always return (more or less) immediately.
 */
boolean xtacacs_note (paktype *pak, ipaddrtype addr)
{
    paktype *nip;
    iphdrtype *ip;
    servtype *lp;
    xtacacstype *tac = NULL;
    uint start, count;
    ta_blocktype *block;

    /*
     * If TAC service not available, then claim okay.
     */
    if (!login_have_servers) {
	if (pak)
	    datagram_done(pak);
	return(TRUE);
    }

    /*
     * If not hosts in queue then refuse access
     */
    lp = (servtype *)tacacshostQ.qhead;
    if (addr) {
        while (lp) {
	    if (lp->addr == addr)
	        break;
	    else lp = lp->next;
	}
    }
    if (lp == NULL || pak == NULL) {
	if (pak)
	    datagram_done(pak);
	return(FALSE);
    }

    block = malloc(sizeof(ta_blocktype));
    if (!block) {
	if (pak)
	    datagram_done(pak);
	return(FALSE);
    }

    nip = setup_ipsocket(lp->sock, pak->unspecified[1], &start);
    if (nip == NULL) {
	if (pak)
	    datagram_done(pak);
	free(block);
	return(FALSE);
    }

    block->trans = random_gen() & 0xFFFF;
    count = 0;
    block->next = NULL;
    enqueue(&tacacslogQ, block);

    tac = (xtacacstype *)(nip->data_area + start);
    bcopy((uchar *)pak->unspecified[0], tac, pak->unspecified[1]);
    nip->unspecified[0] = (ulong) tac;
    nip->unspecified[1] = pak->unspecified[1];
    datagram_done(pak);
    pak = nip;
    ip = (iphdrtype *)ipheadstart(pak);
    tac->trans = block->trans;
    block->done = FALSE;
    pak_lock(pak);
    block->pak = pak;
    TIMER_START(block->nexttime, tacacs_timeout);
    TIMER_START(block->timeout, 5 * ONEMIN);
    write_ipsocket(lp->sock, pak, pak->unspecified[1]);
    if (udp_debug || tacacs_debug) {
	int type = ((xtacacstype *)pak->unspecified[0])->type;
	buginf("\nTAC: Sent notification type %s (%d) to %i, Id %d",
	       xtacacs_type_string(type), type, ip->dstadr, block->trans);
    }
    return (TRUE);
}

boolean xtacacs_login (
    int ttynum,
    char *name,
    char *pw,
    char **reason,
    ipaddrtype addr)
{
    paktype *pak;

    pak = xtacacs_setup(name, pw, XTA_LOGIN, ttynum);
    return(xtacacs_ask_addr(pak, reason, addr));
}

boolean xtacacs_connect (ipaddrtype dhost, ushort dport)
{
    paktype *pak;
    xtacacstype *tac;

    if (!stdio->user && 
	!(tacacs_always & (TACACS_NOTECONNECT | TACACS_ASKCONNECT)))
	return(TRUE);
    pak = xtacacs_setup(stdio->user, NULL, XTA_CONNECT, stdio->ttynum);
    if (pak) {
	tac = (xtacacstype *)pak->unspecified[0];
	tac->uuid = stdio->uuid;
	tac->dhost = dhost;
	tac->dport = dport;
    }
    if (stdio->user) {
	if (tacacs_extended & TACACS_ASKCONNECT)
	    return(xtacacs_ask_addr(pak, NULL, stdio->tty_user_server));
	else
	    return(xtacacs_note(pak, stdio->tty_user_server));
    } else {
	if (tacacs_always & TACACS_ASKCONNECT)
	    return(xtacacs_ask_addr(pak, NULL, stdio->tty_user_server));
	else
	    return(xtacacs_note(pak, stdio->tty_user_server));
    }
}

boolean xtacacs_enable (char *name, char *pw, char **reason, uchar level,
			boolean sendask)
{
    paktype *pak;
    xtacacstype *tac;

    if ((tacacs_extended & (TACACS_ASKENA|TACACS_NOTEENA)) == 0)
	return(FALSE);
    if (level == PRIV_ROOT) {
	pak = xtacacs_setup(name, pw, XTA_ENABLE, stdio->ttynum);
    } else {
	pak = xtacacs_data_setup(name, pw, strlen(pw), XTA_ENABLE,
				 stdio->ttynum, XTA_MINOR_VER_1);
    }
    if (pak) {
	tac = (xtacacstype *)pak->unspecified[0];
	tac->uuid = stdio->uuid;
	tac->accesslist = level;
    }
    /*
     * "sendask" is set to TRUE if we are doing an authenticate(called from
     * tacacs_enable()) and set to FALSE if called from xtacacs_enable_notify().
     */
    if (sendask)
	return(xtacacs_ask(pak, reason));
    else
	return(xtacacs_note(pak, 0));
    return(FALSE);
}
void xtacacs_set_enable (parseinfo *csb)
{
    if (tacacs_extended & TACACS_EXTEND) {
	if (csb->sense)
	    tacacs_extended |= TACACS_ASKENA;
	else 
	    tacacs_extended &= ~TACACS_ASKENA;    
    }
}
void xtacacs_enable_notify (tt_soc *tty, uchar level)
{
    if (tacacs_extended & TACACS_NOTEENA)
	xtacacs_enable(tty->user, NULL, NULL, level, FALSE);
}

void xtacacs_logout (tt_soc *tty)
{
    paktype *pak;
    xtacacstype *tac;

    if (tty && !tty->user && !(tacacs_always & TACACS_NOTELOGOUT))
	return;
    pak = xtacacs_setup(tty ? tty->user : NULL, NULL, XTA_LOGOUT, 
			tty ? tty->ttynum : -1);
    if (pak) {
	tac = (xtacacstype *)pak->unspecified[0];
	tac->uuid = tty ? tty->uuid : -1;
	tac->reason = tty ? tacacs_reason(tty) : 0;
    }
    (void) xtacacs_note(pak, tty ? tty->tty_user_server : 0);

    /* Note that we don't clear tty_user_server so we can use it for
       slipoff notifications which are sent after logout has taken
       place. It will be cleared next time someone logs in to this
       line */
}

void xtacacs_reload (void)
{
    paktype *pak;

    if (system_loading)	    /* If reloading to netboot, then don't bother */
	return;

    pak = xtacacs_setup(NULL, NULL, XTA_RELOAD, 0);
    (void) xtacacs_note(pak, 0);
}

/* Decide which server to send a slipon/slipoff/slipaddr request
 * to. A return of zero means use the "default" server.
 *
 * If the user did a slip address@host, that host is recorded in
 * tty_slip_server.
 *
 * If the user did login@host, that host is recorded in tty_user_server.
 *
 * Otherwise, these are NULL.
 */

static 
ipaddrtype 
choose_slip_server(tt_soc *tty)
{
    if (tty->tty_slip_server) {
	/* the user did slip@sliphost. use sliphost */
	return(tty->tty_slip_server);
    }

    if (tty->tty_user_server) {
	/* no sliphost, but we did login@loginhost. Use loginhost */
	return(tty->tty_user_server);
    }

    /* no sliphost or loginhost. Use default server */
    return(0);
}


boolean xtacacs_slipon (
    char *name, char *pw,
    tt_soc *tty,
    ipaddrtype remaddr,
    char **reason,
    boolean routing,
    boolean from_exec_cmd)
{
    paktype *pak;
    xtacacstype *tac;
    idbtype *swidb;
    xta_slip_acl slip_acl, *acl_p;
    int acl_len, retval;
    ipaddrtype server;
    int ttynum;
    ulong uuid;
    boolean have_user;

    if (tty == NULL) {
	/*
	 * If tty is NULL, then this is a slipon on a non-async PPP line.
	 * We need to use the tty number of -1, and no special server (for
	 * now anyway). 
	 */
	ttynum = -1;
	uuid = -1;
	server = 0;
	have_user = TRUE;
    } else {
	ttynum = tty->ttynum;
	uuid = tty->uuid;
	server = choose_slip_server(tty);
	have_user = tty->user ? TRUE : FALSE;

	/*
	 * Due to autoselect and other stuff, see if we should actually do
	 * this now or just return.
	 */
	if (skip_slipon_slipoff(!from_exec_cmd, tty, tty->tty_idb)) {
	    return(TRUE);
        }

        /*
         * If we are autoselected, then routing should be FALSE.  The PPP
         * code sets routing to TRUE (correct for sync lines), so clear it
         * for async lines.
         */
        if (!from_exec_cmd) {
	    routing = FALSE;
        }

	if (!tty->user && !(tacacs_always & (TACACS_NOTESLIP | TACACS_ASKSLIP)))
	    return(TRUE);

	if ((tty != stdio) && from_exec_cmd) {
	    /* allow lines to be turned on by modem process */
	    name = NULL;
	    pw = NULL;
	}
    }

    if (tacacs_extended & TACACS_GETSLIPACL) {
	pak = xtacacs_data_setup(name, pw, strlen(pw), XTA_SLIPON,
				 ttynum, XTA_MINOR_VER_1);
	acl_len = sizeof(slip_acl);
	acl_p = &slip_acl;
    } else {
	pak = xtacacs_setup(name, pw, XTA_SLIPON, ttynum);
	acl_len = 0;
	acl_p = NULL;
    }
    if (pak) {
	tac = (xtacacstype *)pak->unspecified[0];
	tac->uuid = uuid;
	tac->dhost = remaddr;
	if (routing) {
	    tac->flags = XTA_F_ROUTING;
	} else {
	    tac->flags = 0;
	}
    }

    if ((have_user && (tacacs_extended & TACACS_ASKSLIP)) ||
	(!have_user && (tacacs_always & TACACS_ASKSLIP))) {
	retval = xtacacs_data_ask_addr(pak, reason, server, acl_p, &acl_len);
    } else {
	return(xtacacs_note(pak, server));
    }

    if ((retval == TRUE) && (acl_len == sizeof(slip_acl)) && tty) {
	FOR_ALL_SWIDBS_ON_HW(tty->tty_idb, swidb) {
	    reg_invoke_ip_set_per_user_acl(swidb, OUT, n2hl(slip_acl.out));
	    reg_invoke_ip_set_per_user_acl(swidb, IN, n2hl(slip_acl.in));
	}
	if (tacacs_debug) {
	    buginf("\nTAC: Installing received access lists: in=%d, out=%d",
		   n2hl(slip_acl.in), n2hl(slip_acl.out));
	}
    }

    return(retval);
}

void xtacacs_slipoff (tt_soc *tty, ipaddrtype address, hwidbtype *idb,
		      boolean from_ipcp)
{
    paktype *pak;
    xtacacstype *tac;
    idbtype *swidb;
    ipaddrtype host;

    /* First off, always reset the access lists here, since
     * xtacacs_slipon() might have changed them.
     */
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	reg_invoke_ip_set_per_user_acl(swidb, IN, 
				       IP_ACL_USER_RESET);
	reg_invoke_ip_set_per_user_acl(swidb, OUT, 
				       IP_ACL_USER_RESET);
    }

    /*
     * Due to autoselect and other stuff, see if we should actually do
     * this now or just return.
     */
    if (skip_slipon_slipoff(from_ipcp, tty, idb)) {
	return;
    }

    /*
     * If tty is NULL, then this is a slipoff on a non-async PPP line.  We
     * need to use the tty number of -1, and no username, password, or
     * special server (for now anyway), then bail outta here.
     */
    if (tty == NULL) {
	char *username;
	if (idb && idb->lcpstate)
	    username = idb->lcpstate->user;
	else
	    username = NULL;
	pak = xtacacs_setup(username, NULL, XTA_SLIPOFF, -1);
	if (pak) {
	    tac = (xtacacstype *)pak->unspecified[0];
	    tac->uuid =  -1;
	    tac->dhost = address;
	    tac->reason = XTA_R_QUIT;
	}
	(void) xtacacs_note(pak, 0);
	return;
    }

    if (!tty->user && !(tacacs_always & (TACACS_NOTESLIP | TACACS_ASKSLIP)))
	return;
    pak = xtacacs_setup(tty->user, NULL, XTA_SLIPOFF, tty->ttynum);
    if (pak) {
	tac = (xtacacstype *)pak->unspecified[0];
	tac->uuid = tty->uuid;
	tac->dhost = address;
	tac->reason = tacacs_reason(tty);
    }

    host = choose_slip_server(tty);
    (void) xtacacs_note(pak, host);

    /* After we send a slipoff to sliphost, forget we ever knew about
       this server */
    
    tty->tty_slip_server = 0;
}

boolean xtacacs_slipaddr (
    tt_soc *tty,
    char *name,
    char *pw,
    ipaddrtype remaddr,
    char **reason,
    boolean routing)
{
    paktype *pak;
    xtacacstype *tac;
    ipaddrtype server;

    pak = xtacacs_setup(name, pw, XTA_SLIPADDR, stdio->ttynum);
    if (pak) {
	tac = (xtacacstype *)pak->unspecified[0];
	tac->uuid = stdio->uuid;
	tac->dhost = remaddr;
	if (routing) {
	    tac->flags = XTA_F_ROUTING;
	} else
	    tac->flags = 0;
    }
    server = choose_slip_server(tty);
    return(xtacacs_ask_addr(pak, reason, server));
}

/*
 * xtacacs_authen()	Does non-login authentication.
 * The passed "data" is sent to the TACACS server to be manipulated according
 * to the "type" argument.  The server sends back a result which is returned
 * in "data".  "data" must point to memory large enough to hold the return
 * response.  The result is NOT a pass/fail type of result, but rather
 * manipulated values that can used to determine a pass/fail answer locally.
 * Called by SERVICE_LOGIN_AUTHEN
 */
boolean
xtacacs_authen(
    char *user,
    void *data,
    int data_len,
    int recv_len,
    char **reason,
    int type,
    tt_soc *tty)
{
    char *host;
    ipaddrtype addr;
    paktype *pak;
    int ttynum, minor, length, retval;

    if(!tacacs_extended) {
	printf("%% xtacacs_authen(): Extended tacacs not configured.\n");
	return(FALSE);
    }

    /*
     * Did the user type "user@somehost"?
     */
    for (host = user; *host; host++) {
	if (*host == '@')
	    break;
    }
    if (*host) {
	*host = 0;
	host++;
	if (!parse_hostname(host, &addr)) {
	    printf("%% No such host: %s\n", host);
	    return(FALSE);
	}
    } else
	addr = 0;

    ttynum = tty ? tty->ttynum : -1;

    switch (type) {
    case XTA_ARAP_AUTH:
    case XTA_CHAP_AUTH:
	minor = XTA_MINOR_VER_1;
	break;
    default:
	minor = XTA_MINOR_VER_0;
	break;
    }

    pak = xtacacs_data_setup(user, data, data_len, type, ttynum, minor);
    length = recv_len;
    retval = xtacacs_data_ask_addr(pak, reason, addr, data, &length);

    if((retval == TRUE) && (length != recv_len))
	retval = FALSE;
    if ((retval == TRUE) && tty) {
	if (addr)
	    tty->tty_user_server = addr;
	else
	    tty->tty_user_server = 0;
	setstring(&tty->user, user);
    }
    return(retval);
}

static boolean
skip_slipon_slipoff (boolean from_ipcp, tt_soc *tty, hwidbtype *idb)
{
    /*
     * Okay, we have a valid tty.  We need to get around a couple of
     * situations:  
     *    1) we want to "skip" if not called from IPCP
     *       and it was an autoselect, and the encapsulation type is PPP,
     *       because we won't have a username yet and ipcp_up will call this
     *       function again.  AUTOSELECT_AUTHORIZE is set by a previous
     *       autoselect process is sticky past death, just so we can notice
     *       it here.
     *    2) we want to "skip" if called from IPCP if it
     *       wasn't an autoselect, because then this function was already
     *       called once by slip_exec_command().
     *  2.5) we don't want to "skip" if called from IPCP and the 
     *       interface is dedicated to protocol mode, because then we never
     *       went through slip_exec_command().
     *    3) we want to "skip" for vty lines when called from
     *       IPCP IF we are NOT a one-step.  If we are a two-step,
     *       then this function was already called from slip_exec_command().
     * In we match these conditions, we will just return TRUE,
     * allowing the slipon() or slipoff() call to succeed without sending
     * a TACACS packet.
     */

    if (!idb || !tty) {
	/* No idb shouldn't happen so be overly cautious and don't skip. */
	/* No tty is normal but also means don't skip. */
	return(FALSE);
    }

    if (idb->enctype != ET_PPP) {
	/* If it's not PPP, then never skip. */
	return(FALSE);
    }

    switch (tty->type) {
    case TTY_LINE:
	if (!from_ipcp && ((tty->autoselectmask & AUTOSELECT_AUTHORIZE) ||
			   (tty->capabilities & MUSTSLIP))) {
	    return(TRUE);
	}
	if (from_ipcp && (!(tty->autoselectmask & AUTOSELECT_AUTHORIZE) &&
			  !(tty->capabilities & MUSTSLIP))) {
	    return(TRUE);
	}
	break;
    case VTY_LINE:
	if (from_ipcp && !(tty->autoselectmask & PT_ONESTEP_CONNECTION)) {
	    return(TRUE);
	}
	break;
    default:
	break;
    }

    return(FALSE);
}
