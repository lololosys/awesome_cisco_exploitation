/* $Id: radius.c,v 3.24.4.27 1996/09/12 04:02:07 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/servers/radius.c,v $
 *------------------------------------------------------------------
 * RADIUS - Support for Livingston's "Radius" network autentication protocol
 *
 * July 1995, Bill Westfield
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: radius.c,v $
 * Revision 3.24.4.27  1996/09/12  04:02:07  tkolar
 * CSCdi62277:  L2F tunnel authentication over Radius fails
 * Branch: California_branch
 * Allow cleartext passwords to be passed from Radius
 *
 * Revision 3.24.4.26  1996/09/11  05:01:19  snyder
 * CSCdi68758:  spellink errors
 *               procesing -> processing
 * Branch: California_branch
 *
 * Revision 3.24.4.25  1996/08/28  13:08:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.24.4.24  1996/08/12  16:07:13  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.24.4.23  1996/08/09  20:21:50  billw
 * CSCdi62518:  password length limited to 16bytes in radius packets
 * Branch: California_branch
 *
 * Revision 3.24.4.22  1996/08/07  09:02:19  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.24.4.21  1996/08/02  23:44:21  billw
 * CSCdi60940:  RADIUS Accounting ID fields not globally unique
 * Branch: California_branch
 *
 * Revision 3.24.4.20  1996/07/20  00:30:13  che
 * CSCdi58216:  autocmd constructed from RADIUS includes invalid /compress
 * switch
 * Branch: California_branch
 * Save switches for Frame-mode and Login-mode in different buffers.
 * Use only appropriate buffer when generating the autocommand.
 *
 * Revision 3.24.4.19  1996/07/16  08:01:32  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Inserted patches which were missing and update dirresp code as well.
 *
 * Revision 3.24.4.18  1996/07/06  05:56:19  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.24.4.17  1996/06/25  01:26:38  billw
 * CSCdi60011:  RADIUS should mark servers un-dead if they respond
 * Branch: California_branch
 *
 * Revision 3.24.4.16  1996/06/25  00:42:46  billw
 * CSCdi59887:  Undebug all doesnt turn off radius debug
 * Branch: California_branch
 *
 * Revision 3.24.4.15  1996/06/25  00:06:46  billw
 * CSCdi58045:  existing AAA remote address info not included in RADIUS
 * packets
 * Branch: California_branch
 *
 * Revision 3.24.4.14  1996/06/24  23:29:36  billw
 * CSCdi54704:  RADIUS should be able to talk to different UDP ports
 * Branch: California_branch
 *
 * Revision 3.24.4.13  1996/06/04  01:09:53  snyder
 * CSCdi59389:  spellink errors
 * Branch: California_branch
 *
 * connectionfor -> connection for
 * notconnected -> not connected
 *
 * Revision 3.24.4.12  1996/05/23  20:47:08  billw
 * CSCdi56667:  RADIUS Accounting fixes
 * Branch: California_branch
 *
 * Revision 3.24.4.11  1996/05/21  10:01:32  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.24.4.10  1996/05/07  00:39:30  billw
 * CSCdi56854:  RADIUS should not accept null usernames
 * Branch: California_branch
 *
 * Revision 3.24.4.9  1996/04/24  07:57:14  billw
 * CSCdi55467:  RADIUS use of Access-challenge cause crashes, doesnt work
 * Branch: California_branch
 *
 * Revision 3.24.4.8  1996/04/24  07:18:42  billw
 * CSCdi52594:  RADIUS does not support framed-route attribute
 * Branch: California_branch
 *
 * Revision 3.24.4.7  1996/04/24  02:41:45  billw
 * CSCdi53009:  RADIUS code doesnt handle lack of framed-ip-address
 * correctly
 * Branch: California_branch
 *
 * Revision 3.24.4.6  1996/04/24  02:06:20  billw
 * CSCdi51316:  RADIUS: implement better fallover behavior for down servers
 * Branch: California_branch
 *
 * Revision 3.24.4.5  1996/04/24  01:48:32  billw
 * CSCdi48644:  RADIUS passes addr attribute to AAA LCP authorization
 * Branch: California_branch
 *
 * Revision 3.24.4.4  1996/04/19  15:29:24  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.7  1996/04/09  21:45:12  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.6  1996/03/19  08:33:02  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.5  1996/02/28  06:15:00  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 3.1.2.4  1996/02/21  03:59:05  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 3.1.2.3  1996/01/23  06:32:35  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.1.2.2  1995/12/19  04:13:02  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 3.1.2.1  1995/12/17  02:16:14  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.24.4.3  1996/03/27  09:41:03  billw
 * CSCdi50704:  Additional RADIUS support needed
 * Branch: California_branch
 *
 * Revision 3.24.4.2  1996/03/27  07:59:35  billw
 * CSCdi50545:  RADIUS doesnt fall over to multiple servers
 * Branch: California_branch
 *
 * Revision 3.24.4.1  1996/03/18  21:47:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.13.2.3  1996/03/16  07:28:43  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.13.2.2  1996/03/07  10:42:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.13.2.1  1996/02/20  17:09:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.24  1996/03/07  22:28:37  billw
 * CSCdi50948:  RADIUS can fail to include a meaningful NAS-PORT attribute
 * If stdio is the console, base nas-port on AAA derived "port name"
 *
 * Revision 3.23  1996/02/22  14:37:41  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.22  1996/02/15  10:16:35  billw
 * CSCdi49072:  RADIUS packets fill up ethernet input queue.
 * handle some error cases as well as the mainline cases!
 *
 * Revision 3.21  1996/02/15  09:30:59  billw
 * CSCdi49072:  RADIUS packets fill up ethernet input queue.
 * Someone has to free packets for "background" accounting as well.
 *
 * Revision 3.20  1996/02/15  07:42:18  billw
 * CSCdi49072:  RADIUS packets fill up ethernet input queue.
 * Damn Damn Damn.  So much for the value of beta testing!
 *
 * Revision 3.19  1996/02/10  10:18:13  billw
 * CSCdi48621:  processes waiting for radius can be woken up by other
 * events
 *
 * Revision 3.18  1996/02/10  01:09:24  billw
 * CSCdi48611:  radius doesnt ignore some attributes that it should
 * Also fix a typo (no bad effects) elsewhere.
 *
 * Revision 3.17  1996/02/09  09:54:31  billw
 * CSCdi48528:  radius process shouldnt run if radius isnt configured
 *
 * Revision 3.16  1996/02/09  09:03:10  billw
 * CSCdi43680:  RADIUS doesnt support magic number ip address values
 * also add authstruct->auth_idb, and change auth->tty to auth->auth_tty
 *
 * Revision 3.15  1996/02/07  16:15:01  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.14  1996/01/23  22:39:48  billw
 * CSCdi43679:  RADIUS support does not include CHAP
 * Do it in a way that will be obsolete in California
 *
 * Revision 3.13  1996/01/22  06:59:12  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.12  1996/01/11  02:30:25  jenny
 * CSCdi46513:  TCP/IP need code clean up (part 2)
 * . merged registry functions ip_tuba_best_local_address and
 *     ip_choose_saddr to function ip_best_local_address().
 *
 * Revision 3.11  1996/01/09  10:47:25  billw
 * CSCdi46170:  the box crashed in radius while executing write terminal
 * initialize the radiusserver host queue
 *
 * Revision 3.10  1996/01/09  09:50:43  billw
 * CSCdi46598:  DEBUG RADIUS Command Does Not Confirm Status (debug
 * printer too)
 *
 * Revision 3.9  1996/01/08  09:47:12  billw
 * CSCdi46010:  RADIUS passes addr attribute to AAA LCP authorization
 * temporary fix - undo the patch that caused the problem!
 *
 * Revision 3.8  1996/01/03  19:57:32  irfan
 * CSCdi46304:  radius packets could use a predicatable source IP address
 * allow radius to use a specific (up) interface's IP address.
 *
 * Revision 3.7  1995/12/23  00:02:54  smackie
 * Convert subsystem name to lowercase. (CSCdi46085)
 *
 * Revision 3.6  1995/12/17  18:36:44  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/16  01:09:12  billw
 * CSCdi43681:  Some RADIUS code review comments not incorporated
 *
 * Revision 3.4  1995/11/21  19:25:02  snyder
 * CSCdi44272:  more spellink errors
 *              Recieved -> Received
 *
 * Revision 3.3  1995/11/20  22:36:56  snyder
 * CSCdi44202:  spellink errors
 *              Access-Challange -> Access-Challenge
 *              non-existant -> non-existent
 *
 * Revision 3.2  1995/11/17  18:51:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define VAR_PASSWORD 0

#include "master.h"
#include "../ui/common_strings.h"
#include <stdlib.h>		/* for atoi */
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "../h/cs_registry.regh"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "aaa.h"
#include "access.h"
#include "../servers/radius.h"
#include "../util/md5.h"
#include "../tcp/tcp.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../h/mgd_timers.h"
#include "../util/random.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../if/network.h"
#include "../wan/ppp_auth.h"
#include "../ui/command.h"
#include "../servers/msg_radius.c"

/*
 * Forward references
 */
static void radius_timers(void);
static void radius_queues(int);
static void radius_messages(void);
static void radius_encrypt (radius_ctx *, radiustype *, radius_attrtype *);
static boolean radius_decrypt (radius_ctx *, radiustype *);
static ipaddrtype pick_radius_source(void);
static void radius_cont_login (boolean *, userstruct *, authenstruct *);
static radius_attrtype *radius_std_attributes (radius_attrtype *, userstruct *);
static boolean radius_pick_next_server(radius_ctx *);
static void radius_server_up (radius_ctx *, boolean);
static radius_attrtype *radius_get_attr (userstruct *, int);
static process radius_saveident (void);

#define RADIUS_IPCLIMIT 10

#define RADIUS_MAX_VAL_LEN 253


/*
 * Local data
 */
enum queues {
    inQ,
    doneQ
};

/*
 * Radius variables
 */
static watched_queue *radius_inQ; /* queue of requests from AAA */
static queuetype radius_waitQ;	/* requests waiting for server to reply */
static watched_queue *radius_doneQ; /* requests that have received replies */
static ulong radius_highident;	/* high byte of accounting ident */

static queuetype radius_hostQ;

static mgd_timer radius_timer;	/* retransmit/etc timer */

static int radius_fork;
static int radius_idents;

static char radius_secret[RADIUS_SECRETLENGTH];
static int radius_secret_length;	/* Why re-calculate this every time? */

static boolean radius_debug = 0;
static boolean radius_debug_inited = FALSE;

static ipaddrtype radius_srcaddr;

static idbtype *radius_source_idb;  /* Use IP-addr of this idb for packs */

static ulong radius_retrans = RADIUS_RETRANS_DEFAULT;
static ulong radius_timeout = RADIUS_TIMEOUT_DEFAULT;
static ulong radius_acct_timeout = RADIUS_TIMEOUT_DEFAULT;
static ulong radius_acct_retrans = RADIUS_RETRANS_DEFAULT;
static ulong radius_deadtime = RADIUS_DEADTIME_DEFAULT;

static boolean radius_pw_optional = FALSE;
static boolean radius_directed_request = FALSE;
static boolean radius_extendedports = FALSE;

static boolean radius_server_all_dead = FALSE;


static char *
radius_type_string(int type)
{
    switch(type) {
      case RAD_CODE_ACC_REQUEST:
	return("Access-Request");
      case RAD_CODE_ACC_ACCEPT:
	return("Access-Accept");
      case RAD_CODE_ACC_REJECT:
	return("Access-Reject");
      case RAD_CODE_ACCT_REQUEST:
	return("Accounting-Request");
      case RAD_CODE_ACCT_RESPOND:
	return("Accounting-response");
      case RAD_CODE_ACC_CHALLENGE:
	return("Access-Challenge");
      case RAD_CODE_STAT_SERVER:
	return("Status-server");
      case RAD_CODE_STAT_CLIENT:
	return("Status-client");
      default: 
	return("UNKNOWN");
    }
}

static void
radius_dumppak (char *text, ipaddrtype ip, int port, radiustype *rad)
{
    radius_attrtype *attr;
    char *end;

    buginf("\nRadius: %s id %d %i:%d, %s, len %d", text, 
	   rad->rad_ident, ip, port, radius_type_string(rad->rad_code),
	   rad->rad_length);

    end = rad->rad_length + (char *)rad;
    attr = (radius_attrtype *)rad->rad_attributes;
    while ((char *)attr < end) {
	buginf("\n        Attribute %d %d %08x",
	       attr->type, attr->length, GETLONG(attr->val.str));
	if (attr->type == RAD_ATTR_VS1)
	    buginf("%08x", GETLONG(&attr->val.str[4]));
	attr = (radius_attrtype *) (attr->length + (char *) attr);
    }
}

/*
 * parse_ipandmask
 * parse an ip address and mask in either the "addr/bits" or "addr mask"
 * formats.  Do not be confused by two consecutive addresses where the
 * second one is NOT a mask.
 */
static char *
parse_ipandmask (char *buf, ipaddrtype *ip, ipaddrtype *mask)
{
    char *ptr, *save;
    ipaddrtype address;
    int bits;

    if (null(buf))
	return(buf);
    ptr = deblank(buf);	/* flush leading blanks */
    if ((ptr = octet(ptr,&address,'.')) &&
	(ptr = octet(ptr,&address,'.')) &&
	(ptr = octet(ptr,&address,'.'))) {
	/*
	 * We have the first three octets.  The last octet can have different
	 * terminators depending on the format use for specifying the mask.
	 */
	save = ptr;
	if ((ptr = octet(save,&address,'/'))) {
	    /*
	     * /bits format
	     */
	    bits = parse_unsigned(ptr, (boolean *)mask);
	    if (bits > 0 && bits < 32) {
		*mask = ip_bitcount2mask(bits);
		*ip = address;
		while (!term_char(*ptr++)) /* Skip to next terminator */
		    ;
		return(ptr);	/* Success! */
	    } /* else the format is bad! */
	} else if ((ptr = octet(save,&address,' '))) {
	    /*
	     * first address terminated by space.  See if we have a mask.
	     */
	    char msktxt[17], *mskptr = msktxt;

	    ptr = deblank(ptr);
	    save = ptr;
	    while (!term_char(*ptr)) /* copy the next token */
		*mskptr++ = *ptr++;
	    *mskptr = '\0';
	    
	    if (parse_ip_address(msktxt, mask)) { /* good address */
		if (good_route_mask(address, *mask, subnetzero_enable)) {
		    /*
		     * address is good, mask/address pair looks good.
		     * (do this here so we can tell the difference (maybe)
		     *  between a mask and a next-hop.)
		     */
		    *ip = address;
		    ptr = deblank(ptr);
		    return(ptr);	/* Success! */
		} else {	/* OK address, but doesn't look like mask. */
		    *mask = get_majormask(address);
		    *ip = address;
		    return(save); /* Success of a sort */
		}
	    } else {
		/*
		 * No second address at all
		 */
		*mask = get_majormask(address);
		*ip = address;
		return(save); /* Success of a sort */
	    }
	} else if ((ptr = octet(save,&address,'\0'))) {
	    /*
	     * No second address at all
	     */
	    *mask = get_majormask(address);
	    *ip = address;
	    return(deblank(ptr)); /* Success of a sort */
	}
    }
    /*
     * Failure to parse any form of ip address and mask
     */
    *mask = *ip = 0;
    return(buf);
}

/*
 * radius_sanitycheck
 * Check whether a radius packet appears to contain properly formatted data.
 */
static boolean
radius_sanitycheck (paktype *pak, udptype *udp, radiustype *rad)
{
    radius_attrtype *attr;
    char *end;

    if (udp->length + MINIPHEADERBYTES > pak->datagramsize ||
	udp->length < UDPHEADERBYTES + 20 )
	return(FALSE);		/* UDP not sane */
    if (rad->rad_length < 20 ||
	rad->rad_length != udp->length - UDPHEADERBYTES ||
	rad->rad_length > 4096)
	return(FALSE);

    end = rad->rad_length + (char *)rad;
    attr = (radius_attrtype *)rad->rad_attributes;
    while ((char *)attr < end) {
	if (attr->length < 3 || ((char *)attr + attr->length) > end)
	    return(FALSE);
	attr = (radius_attrtype *) (attr->length + (char *) attr);
    }
    return(TRUE);
}

/*
 * radius_server
 * server for radius packets
 */

static boolean radius_server (paktype *pak, udptype *udp)
{
    iphdrtype *ip = (iphdrtype *)ipheadstart(pak);
    radiustype *rad = (radiustype *)((uchar *)udp + UDPHEADERBYTES);
    radius_ctx *block;

    if (udp->dport != RADIUS_UDP_PORT && udp->dport != RADIUSACCT_UDP_PORT)
	return(FALSE);

    pak->application_start = (uchar *)rad;

    switch (rad->rad_code) {
	case RAD_CODE_ACCT_RESPOND:
	case RAD_CODE_ACC_REJECT:
	case RAD_CODE_ACC_ACCEPT:
	case RAD_CODE_ACC_CHALLENGE:
	if (!radius_sanitycheck(pak, udp, rad)) {
	    if (udp_debug || radius_debug)
		buginf("RADIUS: Received from %i - un-sane packet",
		       ip->srcadr);
	    break;
	}

	if (udp_debug || radius_debug)
	    radius_dumppak("Received from",ip->srcadr, udp->sport, rad);
	/*
	 * Try to find the transaction on the request queue
	 */
	for (block = (radius_ctx *)radius_waitQ.qhead; block;
	    block = block->next) {
	    if (rad->rad_ident == block->ident)
		break;
	}
	/* 
	 * If transaction not on request queue, complain
	 */
	if (block == NULL) {
	    if (radius_debug)
		buginf("\nRADIUS: Response for non-existent request ident");
	    break;
	}
	/*
	 * If not already answered, put pointer to packet in the 
	 * block on queue, lock the packet, and unqueue the block to
	 * signal a response has been received
	 */
	mgd_timer_stop(&block->rxtimer);
	datagram_done(block->pak);	/* Delete held query packet */
	block->pak = pak;		/* Replace with the response */
	pak_lock(pak);
	pak->application_start = (uchar *) rad;
	unqueue(&radius_waitQ, block);
	process_enqueue(radius_doneQ, block);
	if (radius_server_all_dead) {
	    radius_server_all_dead = FALSE;
	    errmsg(&msgsym(SERVERALIVE, RADIUS), block->server);
	}
	break;
      case RAD_CODE_ACC_REQUEST:
      case RAD_CODE_ACCT_REQUEST:
      case RAD_CODE_STAT_SERVER:
      case RAD_CODE_STAT_CLIENT:
	if (radius_debug)
	    buginf("\nRadius: Unsupported packet type");
	break;
      default:
	break;
    }
    datagram_done(pak);
    return(TRUE);
}
 
/*
 * radius_timers
 * Run this when a timer expiration happens.  Handle retransmission of
 * requests, timeouts due to non-response, and so on.
 */

static void
radius_timers (void)
{
    ulong newtime = radius_timeout;

    while (mgd_timer_expired(&radius_timer)) {
	mgd_timer *expired_timer;
	radius_ctx *rctx;

	expired_timer = mgd_timer_first_expired(&radius_timer);
	rctx = mgd_timer_context(expired_timer);
	/*
	 * check whether the line/interface that is trying to authenticate
	 * has given up and gone away while radius is still trying to
	 * retransmit the request.  If so, fail right away.
	 */
	if ( (rctx->tty && modemcheck(rctx->tty)) ||
	     (rctx->idb && (rctx->idb->hwptr->state != IDBS_UP)) ) {
	    if (radius_debug)
		buginf("\nRADIUS: id %d, requester hung up.", rctx->ident);
	    TIMER_START(rctx->expiretime, 0); /* Cause us to fail now! */
	} else if (AWAKE(rctx->expiretime)) {
	    /*
	     * If we've reached our retransmit limit on this server, try
	     * stepping to the next server (which will be treated as continued
	     * retransmissions.
	     */
	    radius_server_up(rctx, FALSE); /* Mark current choice down */
	    if (radius_pick_next_server(rctx)) {
		/*
		 * if there was another server to try, reset our parameters
		 * and try sending to it instead.
		 */
		TIMER_START(rctx->expiretime,
			    100 + radius_timeout * radius_retrans);
		rctx->pak->if_output = NULL;
		if (radius_debug)
		    buginf("\nRadius: Trying next server (%i) for id %d",
			   rctx->server, rctx->ident);
	    }
	}
	if (SLEEPING(rctx->expiretime)) {
	    /*
	     * Still have some time to go.  Retransmit the radius request.
	     */
	    if (radius_debug)
		buginf("\nRadius: Retransmit id %d", rctx->ident);
	    /*
	     * Try to guess whether the last transmission is sitting on
	     * a queue somewhere due to severe network congestion.  If
	     * so, avoid putting another copy on the same queue.
	     */
	    if (rctx->pak->refcount <= 1) {
		/*
		 * reset the packet length and resend the datagram
		 */
		rctx->pak->datagramsize = rctx->pak->length;
		pak_lock(rctx->pak);
		if (rctx->type == RAD_CODE_ACCT_REQUEST) {

                    radius_attrtype *attr;
                    radiustype  *rad;

                    /*
                     * Add the retransmission timeout value
                     * to the Acct-delay-time attribute at
                     * the end of the packet.
                     */
                    attr = (radius_attrtype *)(rctx->pak->application_start +
                                        rctx->pak->length - 6);
                    PUTLONG(&attr->val.num,
			    GETLONG(&attr->val.num) + (newtime/ONESEC));
                    rad = (radiustype *)(rctx->pak->application_start);
                    rad->rad_ident = rctx->ident = radius_idents++;
		    if (radius_debug)
			buginf("\nRadius: acct-delay-time for %lx (at %lx) now %d",
			       rad, attr, GETLONG(&attr->val.num));
                    radius_encrypt(rctx, rad, NULL);
		    ip_udp_send(rctx->pak, pick_radius_source(),
				rctx->server,
				RADIUSACCT_UDP_PORT, rctx->udpport);
		    newtime = radius_acct_timeout;
		} else {
		    ip_udp_send(rctx->pak, pick_radius_source(),
				rctx->server,
				RADIUS_UDP_PORT, rctx->udpport);
		}
	    } else
		if (radius_debug)
		    buginf(" (avoided)");
	    mgd_timer_start(&rctx->rxtimer, newtime);
	} else {
	    /*
	     * request timeout
	     */
	    if (radius_debug)
		buginf("\nRadius: No response for id %d", rctx->ident);
	    datagram_done(rctx->pak);
	    rctx->pak = NULL;
	    unqueue(&radius_waitQ, rctx);
	    process_enqueue(radius_doneQ, rctx);	    
	    mgd_timer_stop(&rctx->rxtimer);
	}
    } /* while */
}

static void
radius_messages (void)
{
    ulong message, value;
    void *pointer;
    
    while (process_get_message(&message, &pointer, &value)) {
	errmsg(&msgsym(UNEXPECTEDMESSAGE,SCHED), message, pointer, value);
    }
}

/*
 * Find the next server AFTER the current one in our list of servers.
 * if needup is TRUE, only look for servers that are believed up.
 */
static radius_servertype *radius_pick_server (radius_ctx *rctx, boolean needup)
{
    radius_servertype *lp;
    int nullcount = 0;

    if (radius_hostQ.qhead == NULL)
	return(NULL);
    if (rctx->server) {
	for (lp=radius_hostQ.qhead; lp; lp= lp->next) {
	    if (lp->addr == rctx->server) /* find where we are now */
		break;
	}
	/*
	 * Check to make sure we FOUND the old server!
	 */
	if (lp != NULL)
	    lp = lp->next;		/* Next server */
	if (lp == NULL) {
	    lp = radius_hostQ.qhead; /* loop around */
	    nullcount++;	/* Seen end of list once... */
	}
    } else {
	lp = radius_hostQ.qhead; /* First server if we have none */
	nullcount++;		/* alread seen "end" once */
    }
    
    while (nullcount <= 1) {	/* Don't loop too many times! */
	/*
	 * If the server currently under scrutiny is the one we started on,
	 * the we have come full cirle without finding a server that is
	 * alive. Print an error, set the flag, and exit
	 */
	if (lp->addr == rctx->firstserver) {
	    if (!radius_server_all_dead) {
		radius_server_all_dead = TRUE;
		errmsg(&msgsym(ALLDEADSERVER, RADIUS));
	    }
	    return(NULL);
	}
	
	/*
	 * check whether the destination port for this type of request has
	 * been configured at 0, which means that this server doesn't support
	 * that type.  (This is our mechanism for running accounting and
	 * authentication on separate machines.)
	 */
	if ((rctx->type == RAD_CODE_ACCT_REQUEST && lp->acctport == 0) ||
	    (rctx->type == RAD_CODE_ACC_REQUEST && lp->authport == 0)) {
	    if (radius_debug)
		buginf("\nRADIUS: server %i doesn't support type %d",
		       lp->addr, rctx->type);
	    lp = lp->next;		/* Next server */
	    if (lp == NULL) {
		lp = radius_hostQ.qhead; /* loop around */
		nullcount++;	/* Seen end of list once... */
	    }
	    continue;
	}

	if (!needup)		/* If the server need not be up, we're done */
	    break;
	if (TIMER_RUNNING_AND_SLEEPING(lp->deaduntil)) {
	    if (radius_debug)
		buginf("\nRADIUS: server %i marked dead, skipping", lp->addr);
	    lp = lp->next;		/* Next server */
	    if (lp == NULL) {
		lp = radius_hostQ.qhead; /* loop around */
		nullcount++;	/* Seen end of list once... */
	    }
	} else {
	    TIMER_STOP(lp->deaduntil);
	    break;		/* Found one that's up! */
	}
    }	    
    if (lp)
	rctx->udpport = (rctx->type == RAD_CODE_ACCT_REQUEST) ? 
	    lp->acctport : lp->authport;
    return(lp);
}


static boolean radius_pick_next_server (radius_ctx *rctx)
{
    radius_servertype *lp;

    lp = radius_pick_server(rctx, TRUE);
    if (!lp)
	lp = radius_pick_server(rctx, FALSE);
    if (!lp)
	return(FALSE);
    if (rctx->firstserver == 0)
	rctx->firstserver = lp->addr;
    rctx->server = lp->addr;
    return(TRUE);
}

static void
radius_server_up (radius_ctx *rctx, boolean up)
{
    radius_servertype *lp;

    for (lp=radius_hostQ.qhead; lp; lp=lp->next)
	if (lp->addr == rctx->server)
	    break;
    if (lp) { 			/* Server not found (!?) */
	if (up) {
	    TIMER_STOP(lp->deaduntil);
	} else {
	    TIMER_START(lp->deaduntil, radius_deadtime*ONEMIN);
	}
    }
}

/*
 * pick_radius_source
 *
 * returns source address to use for radius IP packets.
 */
static ipaddrtype pick_radius_source (void)
{

    /*
     * Pick the local address to use for radius ip packet
     *   This is different from source-addr attribute
     *   in radius data. Returns 0 if "best" address desired.
     */
    if (radius_source_idb && radius_source_idb->ip_address &&
	interface_up(radius_source_idb)) {
	return (radius_source_idb->ip_address);
    }
    return ((ipaddrtype) 0);
}

static void
radius_queues (int which)
{
    int count;
    radius_ctx *rctx;

    switch (which) {
      case inQ:
	/*
	 * Requests from client processes (users, etc.)
	 */
	count = RADIUS_IPCLIMIT;
	while (count-- >0) {
	    rctx = process_dequeue(radius_inQ);
	    if (!rctx)
		break;
	    (void) radius_pick_next_server(rctx); /* Pick initial server */
	    pak_lock(rctx->pak);
	    if (radius_debug)
		radius_dumppak("Initial Transmit",
			       rctx->server, rctx->udpport,
			       (radiustype *)rctx->pak->application_start);
	    /*
	     * ip_udp_send changes datagramsize, so we have to put it back
	     * to the pre-encapsulated size.
	     */
	    rctx->pak->datagramsize = rctx->pak->length;
	    if (rctx->type == RAD_CODE_ACCT_REQUEST)
		(void) ip_udp_send(rctx->pak, pick_radius_source(),
				   rctx->server,
				   RADIUSACCT_UDP_PORT, rctx->udpport);
	    else
		(void) ip_udp_send(rctx->pak, pick_radius_source(),
				   rctx->server,
				   RADIUS_UDP_PORT, rctx->udpport);

	    TIMER_START(rctx->expiretime, 
			100 + radius_timeout * radius_retrans);
	    rctx->pak->datagramsize = rctx->pak->length;
	    enqueue(&radius_waitQ, rctx);
	    mgd_timer_init_leaf(&rctx->rxtimer, &radius_timer, 0, rctx, FALSE);
	    mgd_timer_start(&rctx->rxtimer, radius_timeout);
	}
	break;
      case doneQ:
	/*
	 * replies received from the server
	 */
	rctx = process_dequeue(radius_doneQ);
	mgd_timer_stop(&rctx->rxtimer);
	if (rctx->done) {
	    /*
	     * If there is a process waiting for this repsonse, wake it up.
	     */
	    process_set_boolean(rctx->done, TRUE);
	} else {
	    /*
	     * If not process waiting, WE clean up data structures
	     */
	    datagram_done(rctx->pak);
	    free(rctx);
	}
	break;

      default:
	errmsg(&msgsym(UNEXPECTEDQUEUE,SCHED), which);
	break;
    }
}

/*
 * radius_saveident
 * gross hack to save new unqiue acocunting IDENT in nvram
 */
static process radius_saveident ()
{
    process_sleep_for(10*ONEMIN);
    if (reg_invoke_is_config_dirty() == TRUE) {
	buginf("\nRADIUS: Save of unique accounting ident aborted."
	       "\n        Configuration has been modified.");
    } else if (radius_highident) {
	if (radius_debug)
	    buginf("\nRADIUS: Saving config with new acct ident in nvram");
	write_memory(NULL);
    }
    process_kill(THIS_PROCESS);
}


/*
 * radius_startup, radius
 * initialize various queues and start the radius process
 */
static process radius (void)
{
    ulong major, minor;

    radius_inQ = create_watched_queue("Radius client reqs", 0, inQ);
    radius_doneQ = create_watched_queue("Radius Net done", 0, doneQ);
    process_watch_queue(radius_inQ, ENABLE, RECURRING);
    process_watch_queue(radius_doneQ, ENABLE, RECURRING);
    mgd_timer_init_parent(&radius_timer, NULL);
    process_watch_mgd_timer(&radius_timer, ENABLE);
    reg_add_ip_udp_input(radius_server, "radius_server");

    while (TRUE) {
	/*
	 * Wait for us to be needed
	 */
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
		radius_timers();
		break;

	      case QUEUE_EVENT:
		radius_queues(minor);
		break;

	      case MESSAGE_EVENT:
		radius_messages();
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

static void
radius_startup (void)
{
    if (radius_fork == 0) {
	radius_fork = process_create(radius, "RADIUS", NORMAL_STACK,
				     PRIO_NORMAL);
    }
}

static void
radius_chap_login (boolean *success, userstruct *u, authenstruct *a)
{
    radius_ctx rctx;
    radiustype *rad;
    radius_attrtype *attr;
    paktype *pak;
    uchar response[MD5_LEN], challenge[CHAP_CHALLENGE_SIZE];
    uchar *data;
    int id;

    *success = FALSE;		/* default is failure */

    if (radius_fork == 0) {
	a->status = AAA_AUTHEN_STATUS_ERROR;
	setstring_named(&a->server_msg, "No radius server configured",
			"RADIUS MSG 1");
	return;
    }

    data = a->user_data;
    id = data[0];
    bcopy(&data[1], challenge, CHAP_CHALLENGE_SIZE);
    bcopy(&data[1 + CHAP_CHALLENGE_SIZE], response, MD5_LEN);

    bzero(&rctx, sizeof(radius_ctx)); /* Clear it out! */
    a->server_data_len = 0;
    rctx.pak = pak = getbuffer(MEDDATA); 
    if (pak == NULL) {
	return;
    }
    rctx.done = create_watched_boolean("Radius IPC", 0);
    if (rctx.done == NULL)
	return;
    process_set_boolean(rctx.done, FALSE);
    rctx.ident = radius_idents++;
    rctx.user = u;			/* Save userstruct */
    rctx.authen = a;			/* Save authent struct */
    if (stdio && stdio->ttynum != 0)
	rctx.tty = stdio;
    parse_interface(u->port, &rctx.idb);

    rad = (radiustype *)(pak->datagramstart + IPHEADERBYTES(NOPT) +
			 UDPHEADERBYTES);
    pak->application_start = (char *)rad;
    rad->rad_code = RAD_CODE_ACC_REQUEST;
    rctx.type = RAD_CODE_ACC_REQUEST;
    rad->rad_ident = rctx.ident;

    attr = (radius_attrtype *) &rad->rad_attributes[0];
    attr = radius_std_attributes(attr, u);
    if (!attr) {
	a->status = AAA_AUTHEN_STATUS_ERROR;
	return;
    }	

    attr->type = RAD_ATTR_CPASS;
    attr->length = 19;
    attr->val.str[0] = id;
    bcopy(response,  &attr->val.str[1], 16);
    attr = (radius_attrtype *) (attr->length + (char *) attr);

    attr->type = RAD_ATTR_SERVICE;
    attr->length = 6;
    PUTLONG(&attr->val.num, RAD_SERV_FRAMED);
    attr = (radius_attrtype *) (attr->length + (char *) attr);

    attr->type = RAD_ATTR_FPROT;
    attr->length = 6;
    PUTLONG(&attr->val.num, RAD_FPROT_PPP);
    attr = (radius_attrtype *) (attr->length + (char *) attr);

    pak->length = rad->rad_length = ((char *)attr) - ((char *)rad);

    /*
     * Use CHAP challenge as authenticator
     */
    bcopy(challenge, rad->rad_authent, MD5_LEN);
    bcopy(challenge, &rctx.authent[0], MD5_LEN);

    process_enqueue(radius_inQ, &rctx);
    process_push_event_list(NULL);
    process_watch_boolean(rctx.done, ENABLE, TRUE);
    process_wait_for_event();
    delete_watched_boolean(&rctx.done);		/* Delete references */
    process_pop_event_list(NULL);
    if (pak == rctx.pak) {
	/*
	 * normally, if there isn't a repsonse from the server, rctx.pak will
	 * be null.  To be safe, check whether the pak is still our request.
	 */
	datagram_done(pak);
	rctx.pak = NULL;
    }
    if (rctx.pak == NULL) {
	if (radius_debug)
	    buginf("\nRadius: No response from server");
	a->status = AAA_AUTHEN_STATUS_ERROR;
	setstring_named(&a->server_msg, "No response from server",
			"RADIUS MSG 2");
	return;
    }

    /*
     * We have a response to our request
     */
    pak = rctx.pak;
    rad = (radiustype *)pak->application_start;
    if ( (rctx.tty && modemcheck(rctx.tty)) ||
	(rctx.idb && (rctx.idb->hwptr->state != IDBS_UP)) ) {
	/*
	 * check whether the line/interface that is trying to authenticate
	 * has given up and gone away while radius was still trying to
	 * retransmit the request.  If so, treat the accept like a reject
	 * so that we don't generate false accounting records.
	 */
	a->status = AAA_AUTHEN_STATUS_FAIL;
	if (radius_debug)
	    buginf("\nRADIUS: id %d, requester hung up.", rctx.ident);
    } else if (radius_decrypt(&rctx, rad)) {
	if (iphdrtype_start(pak)->srcadr == rctx.server)
	    radius_server_up(&rctx, TRUE);
	switch (rad->rad_code) {
	  case RAD_CODE_ACC_ACCEPT:
	    a->status = AAA_AUTHEN_STATUS_PASS;
	    *success = TRUE;
	    /* Reply-message ???? */
	    /*
	     * Save radius data for possible future authorization
	     */
	    u->savedauthor = malloc_named(rad->rad_length, "Author info");
	    if (u->savedauthor)
		bcopy(rad, (void *)u->savedauthor, rad->rad_length);
	    break;
	  case RAD_CODE_ACC_REJECT:
	    a->status = AAA_AUTHEN_STATUS_FAIL;
	    *success = TRUE;
	    /*
	     * Note any appropriate message is passed to user during
	     * attribute processing below
	     */
	    break;
	  case RAD_CODE_ACC_CHALLENGE:
	    /* don't accept challenges to CHAP requests */
	  default:
	    a->status = AAA_AUTHEN_STATUS_ERROR;
	    break;
	} /* case */
    } else {
	if (radius_debug)
	    buginf("\nRadius: Reply for %d fails decrypt", rad->rad_ident);
	a->status = AAA_AUTHEN_STATUS_ERROR;
    }
    datagram_done(pak);
}

/*
 * radius_start_login
 * Start a radius login.  In AAA, this is how the username and password are
 * collected.  Since radius expects these to be collected locally, this
 * routine never actually sends any packets - that will be done by the
 * continue_login routine.
 */
static void radius_start_login (boolean *result, userstruct *u,
			 authenstruct *a, int action)
{
    *result = FALSE;		/* Assume failure */
    a->status = AAA_AUTHEN_STATUS_ERROR;

    if (radius_fork == 0) {
	setstring_named(&a->server_msg,"No radius server configured",
			"RADIUS MSG 3");
	return;
    }

    setstring(&a->server_msg, NULL);
    setstring(&a->server_data, NULL);
    a->server_data_len = 0;

    if (action != AAA_AUTHEN_ACT_LOGIN)
	return;			/* RADIUS has no support for SENDPASS/etc */

    switch (action) {
    default:
 	a->status = AAA_AUTHEN_STATUS_ERROR;
 	/* RADIUS has no support for SENDAUTH/etc */
	return;    
 
    case AAA_AUTHEN_ACT_LOGIN:
 	switch (u->authen_type) {
 	case AAA_AUTHEN_TYPE_NONE:
 	case AAA_AUTHEN_TYPE_ARAP:
 	    /* Radius can't handle these. */
	    a->status = AAA_AUTHEN_STATUS_ERROR;
 	    return;
 
	case AAA_AUTHEN_TYPE_CHAP:
 	    /* 
 	     * Id, challenge and response are in the data field of the
 	     * start packet. Process and set status appropriately. 
 	     */
 	    radius_chap_login(result, u, a);
 	    return;

 	case AAA_AUTHEN_TYPE_PAP:
	    /* Username and password should be in the start packet. */
 	    if (u->user && a->user_data_len && a->user_data) {
		u->password = strdup_named(a->user_data, "RADIUS PAP");
 		/* sets status and result appropriately */
 		radius_cont_login(result, u, a);
		return;
 	    }
 	    a->status = AAA_AUTHEN_STATUS_ERROR;
	    return;
 	case AAA_AUTHEN_TYPE_ASCII:
	    break;
 	}
    }
    /*
     * Radius doesn't support null usernames, so if we don't have one yet,
     * have AAA read it locally by faking a server response.
     */
    if (!u->user) {
	a->status = AAA_AUTHEN_STATUS_GETUSER;
	setstring_named(&a->server_msg, username_prompt, "RADIUS PROMPT U1");
	if (!a->server_msg)
	    return;
	*result = TRUE;		/* success, so to speak. */
	return;
    }
    /*
     * Already know user so get password if needed
     */
    a->status = AAA_AUTHEN_STATUS_GETPASS;
    setstring_named(&a->server_msg, password_prompt, "RADIUS PROMPT P1");
    if (!a->server_msg)
	return;
    *result = TRUE;
    a->flags = AAA_RESP_FLAG_NOECHO;
    return;
}

/*
 * radius_std_attributes
 * Fill in the "standard" attributes common to all the radius packets
 * that we originate.  This includes our ip address, the port number,
 * and the username.
 *
 * Returns NULL on error
 */
static radius_attrtype *
radius_std_attributes (radius_attrtype *attr, userstruct *u)
{
    ulong port;
    ipaddrtype nasip;
    int len;
    int porttype = RAD_PTYPE_ASYNC;

    nasip = pick_radius_source();
    if (nasip == 0)
	nasip = radius_srcaddr;
    attr->type = RAD_ATTR_NASIP;
    attr->length = 2 + sizeof(ipaddrtype);
    PUTLONG(&attr->val.addr, nasip);
    attr = (radius_attrtype *) (attr->length + (char *) attr);

    if (stdio && stdio->ttynum != 0) {
	port = stdio->ttynum;
	if (stdio->type == VTY_LINE)
	    porttype = RAD_PTYPE_VTY;
    } else {
	/*
	 * Use AAA to encode u->port into 16 bits of info for non-ttys.
	 * We'll also need to decode ustruct->port if we're called by
	 * something like modem_scanner or ppp_process, in which case
	 * stdio will not reflect port we're doing stuff for!
	 */
	if (bcmp(u->port, "tty", 3) == 0) {
	    /*
	     * a tty style port, use only the numeric part as NAS-PORT
	     */
	    port = atoi(&u->port[3]);
	    if (MODEMS[port]->type == VTY_LINE)
		porttype = RAD_PTYPE_VTY;
	} else if (bcmp(u->port, "Async", 5) == 0) {
	    /*
	     * Async interface, user unit number only
	     */
	    port = atoi(&u->port[5]);
	} else if (bcmp(u->port, "VTY-Async", 9) == 0) {
	    /*
	     * virtual Async interface, user unit number only
	     */
	    port = atoi(&u->port[9]);
		porttype = RAD_PTYPE_VTY;
	} else {
	    port = idbname2portval(u->port);
	    switch (port/AAA_IFVAL_DIVISOR) {
	      case AAA_IFVAL_ASYNC:
		porttype = RAD_PTYPE_ASYNC;
		break;
	      case AAA_IFVAL_SYNC:
		porttype = RAD_PTYPE_SYNC;
		break;
	      case AAA_IFVAL_BRI:
	      case AAA_IFVAL_PRI:
		porttype = RAD_PTYPE_SISDN;
		break;
	      default:
		porttype = RAD_PTYPE_ILLEGAL;
		break;
	    }
	}
    }
    if (port < 1000 && radius_extendedports) {
	hwidbtype *idb = NULL;
	if (reg_invoke_channel_hwidb(MODEMS[port], &idb)) {
	    if (idb) { 
		port |= idbname2portval(idb->hw_short_namestring)<<16;
		if (radius_debug)
		    buginf("\nRADIUS: Computed extended port value %d:%d:",
			   port>>16, port & 0xFFFF);
	    }
	}
    }
    attr->type = RAD_ATTR_NASPORT;
    attr->length = 2 + sizeof(int);
    PUTLONG(&attr->val.num, port);
    attr = (radius_attrtype *) (attr->length + (char *) attr);

      
    if (porttype != RAD_PTYPE_ILLEGAL) {
	attr->type = RAD_ATTR_NASPTYPE;
	attr->length = 2 + sizeof(int);
	PUTLONG(&attr->val.num, porttype);
	attr = (radius_attrtype *) (attr->length + (char *) attr);
    }

    attr->type = RAD_ATTR_USER;
    len = strlen(u->user);
    if (len > RADIUS_MAX_VAL_LEN) {
	if (radius_debug) {
	    buginf("\nRADIUS: username too long (%d)", len);
	}
	return(NULL);
    }
    memcpy(attr->val.str, u->user, len);
    attr->length = 2 + len;
    attr = (radius_attrtype *) (attr->length + (char *) attr);

    if (!null(u->rem_addr)) {
	char *caller, *called;
	int callerlen;

	caller = u->rem_addr;
	called = strchr(u->rem_addr, '/');
	if (called) {
	    attr->type = RAD_ATTR_CDNIS;
	    len = strlen(called+1);
	    if (len > RADIUS_MAX_VAL_LEN) {
		if (radius_debug) {
		    buginf("\nRADIUS: DNIS too long (%d)", len);
		}
		return(NULL);
	    }
	    memcpy(attr->val.str, called+1, len);
	    attr->length = 2 + len;
	    attr = (radius_attrtype *) (attr->length + (char *) attr);
	    callerlen = called - caller;
	} else {
	    callerlen = strlen(caller);
	    if (callerlen > RADIUS_MAX_VAL_LEN) {
		if (radius_debug) {
		    buginf("\nRADIUS: CALLER-ID too long (%d)", len);
		}
		return(NULL);
	    }
	}

	if (strstr(u->rem_addr, "async") != u->rem_addr) {
	    attr->type = RAD_ATTR_CALLID;
	    attr->length = 2 + callerlen;
	    bcopy(caller, attr->val.str, callerlen);
	    attr = (radius_attrtype *) (attr->length + (char *) attr);
	}
    }

    return(attr);
}

/*
 * radius_add_attributes
 * Add login request attributes to a radius packet
 *
 * Returns NULL on error
 */
static radius_attrtype *
radius_add_attributes (radiustype *rad, userstruct *u, authenstruct *a,
		       radius_attrtype **password)
{
    radius_attrtype *attr;
    char *userpass;

    attr = (radius_attrtype *) &rad->rad_attributes[0];
    attr = radius_std_attributes(attr, u);
    if (!attr) {
	return(NULL);
    }

    *password = attr;
    attr->type = RAD_ATTR_UPASS;
#if VAR_PASSWORD
    attr->length = 2 + strlen(u->password) + 1;	/* Send the null too */
    if (a->status == AAA_AUTHEN_STATUS_GETDATA)
	strcpy(attr->val.str, a->user_reply);
    else
	strcpy(attr->val.str, u->password);
#else
    /*
     * round up length to nearest multiple of 16 bytes
     * The password is currently null terminated, even if exactly a
     * multiple of 16 bytes long (ie always round length UP.)
     */
    if (a->status == AAA_AUTHEN_STATUS_GETDATA)
	userpass = a->user_reply;
    else
	userpass = u->password;
    attr->length = ((strlen(userpass) + 16) & 0xFFFFFFF0);
    sstrncpy(attr->val.str, userpass ? userpass : "", attr->length); 
    attr->length += 2;		/* type and length fields */
#endif
    attr = (radius_attrtype *) (attr->length + (char *) attr);

    if (u->authen_config) {
	/* This is an authentication attempt made solely to download
	 * some configuration information. e.g. ip pool definitions.
	 * The radius server will only accept this ersatz login if it
	 * claims to be an outbound call, so that no-one can use this
	 * entry to get a login. 
	 */
	attr->type = RAD_ATTR_SERVICE;
	attr->length = 6;
	PUTLONG(&attr->val.num, RAD_SERV_OUTB);
	attr = (radius_attrtype *) (attr->length + (char *) attr);
    }

    switch (u->service) {
	/*
	 * Add additional information about the type of login requested 
	 */
      case AAA_SVC_ENABLE:
	attr->type = RAD_ATTR_SERVICE;
	attr->length = 6;
	PUTLONG(&attr->val.num, RAD_SERV_ADMIN);
	attr = (radius_attrtype *) (attr->length + (char *) attr);
	break;
      case AAA_SVC_PPP:
	attr->type = RAD_ATTR_SERVICE;
	attr->length = 6;
	PUTLONG(&attr->val.num, RAD_SERV_FRAMED);
	attr = (radius_attrtype *) (attr->length + (char *) attr);
	attr->type = RAD_ATTR_FPROT;
	attr->length = 6;
	PUTLONG(&attr->val.num, RAD_FPROT_PPP);
	attr = (radius_attrtype *) (attr->length + (char *) attr);
	break;
      default:
	break;
    }
    if (a && a->opaque) {		/* Add saved state */
	attr->type = RAD_ATTR_STATE;
	attr->length = strlen(a->opaque);
	bcopy(a->opaque, attr->val.str, attr->length);
	attr->length += 2;
	attr = (radius_attrtype *) (attr->length + (char *) attr);
    }
    return(attr);
}

/*
 * radius_cont_login
 * "Continue" a radius login.  Send the username/password that we've collected,
 * and wait for a response from the server.  If the response wants us to 
 * collect additional challange information, continue with that as well.
 * Otherwise, report success or failure to caller.
 */
static void 
radius_cont_login (boolean *result, userstruct *u, authenstruct *a)
{
    radius_ctx rctx;
    radiustype *rad;
    radius_attrtype *attr, *password;
    paktype *pak;
    char tmp;

    *result = FALSE;		/* default is failure */
    setstring(&a->server_msg, NULL);
    setstring(&a->server_data, NULL);

    if (null(u->user)) {
	a->status = AAA_AUTHEN_STATUS_GETUSER;
	setstring_named(&a->server_msg, username_prompt, "RADIUS PROMPT U2");
	if (!a->server_msg) {
	    a->status = AAA_AUTHEN_STATUS_ERROR;
	    return;
	}
	*result = TRUE;		/* success, so to speak. */
	return;
    }
    if (u->password == NULL && radius_pw_optional == FALSE) {
	/* Already know user so get password if needed */
	a->status = AAA_AUTHEN_STATUS_GETPASS;
	setstring_named(&a->server_msg, password_prompt, "RADIUS PROMPT P2");
	if (!a->server_msg) {
	    a->status = AAA_AUTHEN_STATUS_ERROR;
	    return;
	}
	*result = TRUE;
	a->flags = AAA_RESP_FLAG_NOECHO;
	return;
    }

    memset(&rctx, 0, sizeof(radius_ctx)); /* Clear it out! */
    a->server_data_len = 0;
    rctx.pak = pak = getbuffer(MEDDATA); 
    if (pak == NULL) {
	return;
    }
    rctx.done = create_watched_boolean("Radius IPC", 0);
    if (rctx.done == NULL)
	return;
    process_set_boolean(rctx.done, FALSE);
    rctx.ident = radius_idents++;
    rctx.user = u;			/* Save userstruct */
    rctx.authen = a;			/* Save authent struct */

    rad = (radiustype *)(pak->datagramstart + IPHEADERBYTES(NOPT) +
			 UDPHEADERBYTES);
    pak->application_start = (char *)rad;
    rad->rad_code = RAD_CODE_ACC_REQUEST;
    rctx.type = RAD_CODE_ACC_REQUEST;
    rad->rad_ident = rctx.ident;

    attr = radius_add_attributes(rad, u, a, &password);
    if (!attr) {
	a->status = AAA_AUTHEN_STATUS_ERROR;
	return;
    }

    pak->length = rad->rad_length = ((char *)attr) - ((char *)rad);
    radius_encrypt(&rctx, rad, password);
    process_enqueue(radius_inQ, &rctx);
    process_push_event_list(NULL);
    process_watch_boolean(rctx.done, ENABLE, TRUE);
    process_wait_for_event();
    delete_watched_boolean(&rctx.done);		/* Delete references */
    process_pop_event_list(NULL);
    if (pak == rctx.pak) {
	/*
	 * normally, if there isn't a repsonse from the server, rctx.pak will
	 * be null.  To be safe, check whether the pak is still our request.
	 */
	datagram_done(pak);
	rctx.pak = NULL;
    }
    if (rctx.pak == NULL) {
	if (radius_debug)
	    buginf("\nRadius: No response from server");
	a->status = AAA_AUTHEN_STATUS_ERROR;
	setstring_named(&a->server_msg, "No response from server",
			"RADIUS MSG 4");
	return;
    }

    /*
     * We have a response to our request
     */
    pak = rctx.pak;
    rad = (radiustype *)pak->application_start;
    if (radius_decrypt(&rctx, rad)) {
	if (iphdrtype_start(pak)->srcadr == rctx.server)
	    radius_server_up(&rctx, TRUE);
	switch (rad->rad_code) {
	  case RAD_CODE_ACC_ACCEPT:
	    a->status = AAA_AUTHEN_STATUS_PASS;
	    *result = TRUE;
	    /*
	     * Save radius data for possible future authorization
	     */
	    u->savedauthor = malloc_named(rad->rad_length, "Author info");
	    if (u->savedauthor)
		if (radius_debug)
		    buginf("\nRADIUS: saved authorization data for user %lx at %lx",
			   u, u->savedauthor);
		bcopy(rad, (void *)u->savedauthor, rad->rad_length);
	    /*
	     * Arrange for any reply message to be shown to the user
	     */
	    attr = radius_get_attr(u, RAD_ATTR_RMSG);
	    if (attr) {
		tmp = attr->val.str[attr->length - 2];
		attr->val.str[attr->length - 2] = '\0';
		setstring_named(&a->server_msg, &attr->val.str[0],
				"RADIUS MSG 5");
		attr->val.str[attr->length - 2] = tmp;
	    }		    
	    break;
	  case RAD_CODE_ACC_REJECT:
	    if (u->password == NULL && radius_pw_optional) {
	      /*
	       * we failed with no password when passwords were optional.
	       * Go back and ask the user for a password and try again.
	       */
	      a->status = AAA_AUTHEN_STATUS_GETPASS;
	      setstring_named(&a->server_msg, password_prompt, "RADIUS MSG 6");
	      if (!a->server_msg) {
		a->status = AAA_AUTHEN_STATUS_ERROR;
		break;
	      }
	      *result = TRUE;
	      a->flags = AAA_RESP_FLAG_NOECHO;
	      break;
	    }
	    a->status = AAA_AUTHEN_STATUS_FAIL;
	    *result = TRUE;
	    /*
	     * look for message in the packet instead of in ustruct.
	     * Note that we can null terminate the data in the packet,
	     * because we're done with it by now.
	     */
	    attr = (radius_attrtype *)rad->rad_attributes;
	    while ((char *)attr < (rad->rad_length + (char *)rad)) {
		if (attr->type == RAD_ATTR_RMSG) {
		    attr->val.str[attr->length - 2] = '\0';
		    setstring_named(&a->server_msg, &attr->val.str[0],
				    "RADIUS MSG 7");
		    break;
		}
		attr = (radius_attrtype *) (attr->length + (char *) attr);
	    }
	    break;
	  case RAD_CODE_ACC_CHALLENGE:
	    attr = (radius_attrtype *) &rad->rad_attributes[0];
	    while ((char *)attr < (rad->rad_length + (char *)rad)) {
		switch (attr->type) {
		  case RAD_ATTR_RMSG:
		    if ((((char *)attr) + attr->length) <=
			(rad->rad_length + (char *)rad)) {  /* Sanity check length */
			tmp = attr->val.str[attr->length-2];
			attr->val.str[attr->length-2] = '\0';
			setstring_named(&a->server_msg, &attr->val.str[0],
					"RADIUS MSG 8");
			a->status = AAA_AUTHEN_STATUS_GETDATA;
			*result = TRUE;
			attr->val.str[attr->length-2] = tmp;
		    }
		    break;
		  case RAD_ATTR_STATE:
		    if ( ((char *)attr + attr->length) <= 
			(rad->rad_length + (char *)rad)) {  /* Sanity check length */
			tmp = attr->val.str[attr->length-2];
			attr->val.str[attr->length-2] = '\0';
			if (a->opaque)
			    free(a->opaque); /* Free previous state */
			a->opaque = strdup_named(&attr->val.str[0],
						 "RADIUS opaque");
			attr->val.str[attr->length-2] = tmp;
		    }
		    break;
		  default:
		    /*
		     * we should have already complained about actual errors.
		     */
		    break;
		} /* case */
		attr =(radius_attrtype *) (attr->length + (char *)attr);
	    } /* While */
	    break;
	  default:
	    a->status = AAA_AUTHEN_STATUS_ERROR;
	    break;
	} /* case */
    } else {
	if (radius_debug)
	    buginf("\nRadius: Reply for %d fails decrypt", rad->rad_ident);
	a->status = AAA_AUTHEN_STATUS_ERROR;
    }
    datagram_done(pak);
}

static void
radius_bad_attr (userstruct *u, radius_attrtype *attr, char *msg)
{
    if (radius_debug)
	buginf("\nRadius: Bad attribute (%s): type %d len %d data %#x",
	       msg, attr->type, attr->length, GETLONG(&attr->val.num));
}

static boolean radius_useripaddr (char *prompt, ipaddrtype *ip)
{
    jmp_buf catch;
    char buf[100];
    char *p;

    *ip = 0L;
    while (0L == *ip) {
	if (setjmp(&catch)) {
	    return(FALSE);
	} else {
	    p = read_string2(&catch, buf, prompt, NULL);
	    if (parse_hostname(p, ip))
		return(TRUE);
	}
    }
    return(TRUE);
}

/*
 * radius_handle_froute
 * Try to handle a per-user route in some format.  Handle the cisco format,
 * the ascend format, and the format recomended in the specification.  Sigh.
 * cisco format:   network mask [router]
 * spec format:	   network/bits router metrics
 * ascend format:  network/bits router metrics [router==0 --> interface]
 */
static boolean
radius_handle_froute (char *proto, radius_attrtype *radattr,
			     char ***in_args, int* avail) 
{
    ipaddrtype network, mask, gateway = 0;
    char tmp= *(((char*)radattr)+radattr->length);
    char *ptr = radattr->val.str;

    /*
     * we support route options where the route is a text string containing
     * a network with optional mask and gateway.  Lack of a gateway (or
     * gateway 0.0.0.0) indicates that we should use a route via this user's
     * interface.
     */
    *(((char*)radattr)+radattr->length) = '\0'; /* null terminate */

    ptr = parse_ipandmask(ptr, &network, &mask);
    if (!network) {
	if (radius_debug)
	    buginf("\nRADIUS: unrecognized format for framed-route");
	*(((char*)radattr)+radattr->length) = tmp; /* restore */
	return(FALSE);
    } else if (radius_debug)
	buginf("\nRADIUS: route for %i %i", network, mask, gateway);
    if (good_route_mask(network, mask, subnetzero_enable)) {
	if (*(ptr-1) && (ptr = octet(ptr,&gateway,'.')) && 
	    (ptr = octet(ptr,&gateway,'.')) &&
	    (ptr = octet(ptr,&gateway,'.'))) {
	    char *t = ptr;
	    if ((ptr = octet(ptr,&gateway,'\0')) == NULL) {
		ptr = t;
		ptr = octet(ptr, &gateway, ' ');
	    }
	    if (ptr) {
		if (radius_debug)
		    buginf(" via %i", gateway);
		if (*(ptr-1) == ' ')  { /* Check for additional argument */
		    if (radius_debug)
			buginf("\nRadius: Additional route metrics ignored");
		}
		ptr = string_getnext();
		sprintf(ptr, gateway ? "%i %i %i" : "%i %i",
			network, mask, gateway);
	    } else {
		ptr = string_getnext();
		sprintf(ptr, "%i %i", network, mask);
	    }
	} else {
	    ptr = string_getnext();
	    sprintf(ptr, "%i %i", network, mask);
	}
	if (aaa_add_AV(in_args, avail, AAA_ATTR_route, ptr, AAA_AV_MAND)) {
	    *(((char*)radattr)+radattr->length) = tmp; /* restore */
	    return(TRUE);
	} else if (radius_debug)
		buginf("\nRADIUS: can't add route AVPair");
    } else {
	if (radius_debug)
	    buginf("\nRADIUS: bad mask in framed-route");
    }
    *(((char*)radattr)+radattr->length) = tmp; /* restore */
    return(FALSE);
}

/*
 * radius_cisco_vs
 * Process cisco-style AVpairs including in a cisco vendor specific option
 * Only add the avpairs to the authorization data if the protocol matches,
 * or if no specific protocol was mentioned.
 */
static boolean radius_cisco_vs(char *proto, radius_attrtype *radattr, char ***in_args,
			int* avail) 
{
    radius_attrtype *attr;
    char *data = string_getnext();
    int i;
    char *p, *a, *v;		/* Protocol, attribute, and value */
    char mand;			/* mandatory or optional? */
    
    /*
     * Point attr pointer inside the vendor specific attibutes
     */
    attr =(radius_attrtype *) &radattr->val.str[4];
    if (attr->length + 6 != (radattr->length) ||
	attr->length > 80) {
	buginf("\nRadius: inappropriate cisco attr length %d", attr->length);
	return(FALSE);
    }
    if (attr->type == RAD_CISCO_AVPAIR) {
	/*
	 * break the string "protocol:attrbute=value" into it's pieces
	 * and store pointers to each piece.
	 */
	p = a = v = NULL;
	mand = AAA_AV_OPT;
	for (i=0; i< attr->length-2; i++) {
	    switch (attr->val.str[i]) {
	      default:
		data[i] = attr->val.str[i];
		break;
	      case ':':		/* colon indicats end of protocol */
		data[i]= 0;
		if (a || v) {
		    buginf("\nRadius: invalid format in cisco "
			   "avpair at \"%s\"", data);
		    return(FALSE);
		}
		p = data;
		a = &data[i+1];
		break;
	      case '=':
		mand = AAA_AV_MAND;
		/* Fall through */
	      case '*':
		if (!a)		/* If Attr is not set because there is no */
		    a = data;	/* protocol specified, set it now. */
		data[i] = 0;
		v = &data[i+1];
		break;
	    }
	} /* for */
	data[i] = 0;
	if (!(a && v)) {
	    buginf("\nRadius: invalid format in cisco "
		   "avpair at \"%s\"", data);
	    return(FALSE);
	}

	if (!p || (strcmp(proto, p) == 0)) {
	    /*
	     * This avpair should be applied to this protocol,
	     * so go and do the actuall AAA call to do so.
	     */
	    if (radius_debug)
		buginf("\nRadius: cisco AVPair \"%s:%s%c%s\"", p, a,
		       mand == AAA_AV_MAND ? '=' :'*', v);
	    if (aaa_add_AV(in_args, avail, a, v, mand))
		return(TRUE);	/* Success! */
	} else {
	    if (radius_debug)
		buginf("\nRadius: cisco AVPair \"%s:%s%c%s\" "
		       "not applied for %s",
		       p, a, (mand==AAA_AV_MAND) ? '=' : '*', v, proto);
	    return(TRUE);
	}
    } else
	if (radius_debug)
	    buginf("\nRadius: unrecognized cisco VS option %d", attr->type);

    return(FALSE);
}

/*
 * radius_shell_author
 * extract radius authorization data for a shell (exec)
 * possibly generate an autocommand.
 */
static void
radius_shell_author (boolean *result, userstruct *u, authorstruct *author)
{
    const radiustype *rad = u->savedauthor;
    radius_attrtype *attr;
    char *dataend;
    boolean error = FALSE;	/* unrecoverable error? */
    char *command = NULL;
    char args[20];		/* room for ip address, etc */
    char args2[20];		/* room for port, etc */
    char f_switches[40];	/* room for frame switches */
    char l_switches[40];	/* room for login switches */
    char autocommand[100];	/* constructed command */
    char *add_frame_switch = f_switches;
    char *add_login_switch = l_switches;
    ipaddrtype ip;
    int avail = 0;
    int type = -1;			/* Type of service */

    if (rad == NULL)		/* No saved data is an error */
	return;

    f_switches[0] = l_switches[0] = args[0] = autocommand[0] = args2[0] = 0;
    attr = (radius_attrtype *) rad->rad_attributes;
    dataend = rad->rad_length + (char *)rad;
    while ((char *)attr < dataend) {
	switch (attr->type) {
	  default:
	    break;
	  case RAD_ATTR_SERVICE:
	    if (type != -1) {
		/*
		 * There had already been one service seen, so we want to 
		 * drop out before our parameters are overwritten by extra
		 * data in the packet
		 */
		attr = (radius_attrtype *)dataend;
		continue;
	    }
	    type = GETLONG(&attr->val.num);
	    switch (type) {
	      case RAD_SERV_FRAMED:
		command = "ppp";
		sstrncpy(args, "negotiate", 20);
		break;
	      case RAD_SERV_LOGIN:
		break;
	      case RAD_SERV_ADMIN:
	      case RAD_SERV_EXEC:
		command = NULL;	/* exec user, don't use a command */
		break;
	    }

	    break;
	    
	  case RAD_ATTR_FCOMP:
	    if (GETLONG(&attr->val.num) == RAD_FCOMP_VJTCP)
		add_frame_switch += sprintf(add_frame_switch, " /compress");
	    else if (radius_debug)
		radius_bad_attr(u, attr, "Unsupported value");
	    break;
	  case RAD_ATTR_FPROT:
	    if (GETLONG(&attr->val.num) == RAD_FPROT_SLIP) {
		command = "slip";
		sstrncpy(args, "default", 20);
	    }
	    else if (GETLONG(&attr->val.num) == RAD_FPROT_PPP) {
		command = "ppp";
		sstrncpy(args, "negotiate", 20);
	    }
	    else if (radius_debug)
		radius_bad_attr(u, attr, "Unsupported value");
	    break;
	  case RAD_ATTR_FILTER:
	    if (type != RAD_SERV_FRAMED) {
		char tmp = attr->val.str[attr->length-2];
		char *suffix;

		attr->val.str[attr->length-2] = 0;
		suffix = strchr(attr->val.str, '.');
		
		if (suffix) {
		  *suffix = 0;
		}
		if ((suffix == 0) || (strcasecmp(suffix+1, "out") == 0)) {
		  
		    if (!aaa_add_AV(&author->in_args, &avail, AAA_ATTR_acl,
				    attr->val.str, AAA_AV_MAND)) {
			attr->val.str[attr->length-2] = tmp;
			if (suffix) *suffix = '.';
			return;
		    }
		}
		
		if (suffix) {
		  *suffix = '.';
		}
		attr->val.str[attr->length-2] = tmp;
	    }
	    break;
	  case RAD_ATTR_FIP:
	    ip = GETLONG(&attr->val.addr);
	    if (ip == RAD_FIP_NASPICK) {
		sstrncpy(args, "default", 20);
	    } else if (ip == RAD_FIP_USERPICK &&
		       (strcmp("slip", command) == 0)) {
		/*
		 * If SLIP is in use, and the user is supposed to pick an
		 * address, prompt for it in text mode. If PPP is in use,
		 * we can get by with the "negotiate" default value.
		 */
		(void) radius_useripaddr("SLIP Address", &ip);
		sprintf(args, "%i", ip); /* Could be 0, which is OK */
	    } else {
		sprintf(args, "%i", ip);
	    }
	    break;
	  case RAD_ATTR_FMASK:
	  case RAD_ATTR_FMTU:
	  case RAD_ATTR_FROUTE:
	  case RAD_ATTR_FIPXN:
	  case RAD_ATTR_FROUTING:
	  case RAD_ATTR_FATNET:
	  case RAD_ATTR_FATZONE:
	    /*
	     * Packet mode stuff
	     * This is mostly handled in the network authorization phase,
	     * and ignored during exec authorization.
	     */
	    break;

	  case RAD_ATTR_LHOST:
	    ip = GETLONG(&attr->val.addr);
	    if (ip == RAD_LHOST_NASPICK) {
		sstrncpy(args, "radius-default", sizeof args);
	    } else if (ip == RAD_LHOST_USERPICK) {
		if (radius_useripaddr("Login Host", &ip))
		    sprintf(args, "%i", ip);
	    } else {
		sprintf(args, "%i", ip);
	    }
	    break;
	  case RAD_ATTR_LSERV:
	    switch (GETLONG(&attr->val.num)) {
	      case RAD_LSERV_TCP:
		add_login_switch += sprintf(add_login_switch, " /stream");
		/* Fall through */
	      case RAD_LSERV_TN:
		command = "telnet";
		break;
	      case RAD_LSERV_RLOG:
		command = "rlogin";
		break;
	      case RAD_LSERV_LAT:
		command = "lat";
		break;
	      default:
		radius_bad_attr(u, attr, "bad value");
		break;
	    }
	    break;
	  case RAD_ATTR_LPORT:
	    sprintf(args2, "%d", GETLONG(&attr->val.num));
	    break;
	  case RAD_ATTR_ATIMOUT:
	    /*
	     * note that radius timeouts are in seconds, but AAA wants a
	     * value in minutes.  Do appropriate fiddling, and round...
	     * (note that 0 means no timeout...)
	     */
	    sprintf(autocommand, "%d", (GETLONG(&attr->val.num) + 59)/60);
	    if (!aaa_add_AV(&author->in_args, &avail, AAA_ATTR_timeout,
			    autocommand, AAA_AV_OPT))
		return;
	    autocommand[0] = 0;
	    break;

	  case RAD_ATTR_ITIMOUT:
	    /*
	     * note that radius timeouts are in seconds, but AAA wants a
	     * value in minutes.  Do appropriate fiddling, and round...
	     */
	    sprintf(autocommand, "%d", (GETLONG(&attr->val.num) + 59)/60);
	    if (!aaa_add_AV(&author->in_args, &avail, AAA_ATTR_idletime,
			    autocommand, AAA_AV_OPT))
		return;
	    autocommand[0] = 0;
	    break;	    

	  case RAD_ATTR_LLATS:
	    if (attr->length - 2 < 20)
		sstrncpy(args, attr->val.str, 20);
	    else {
		radius_bad_attr(u, attr, "too long");
		error = TRUE;
	    }
	    break;
	  case RAD_ATTR_VS1:
	    if (GETLONG(&attr->val.num) == OID_CISCO) {
		if (attr->length + (char *)attr > dataend) /* Sanity check */
		    break;
		if (radius_cisco_vs("shell", attr, &author->in_args, &avail))
		    break;
		/* else fall through */
	    } else {
		if (radius_debug)
		    buginf("\nRadius: unrecognized Vendor code %d",
			   GETLONG(&attr->val.num));
		break;
	    }
	    /* Else fall through */

	  case RAD_ATTR_LLATN:
	  case RAD_ATTR_LLATG:
	  case RAD_ATTR_UNA1:
	  case RAD_ATTR_UNA2:
	  case RAD_ATTR_CBNUM:
	  case RAD_ATTR_CBNAM:
	    radius_bad_attr(u, attr, "unsupported attribute");
	  case RAD_ATTR_CLASS:
	    break;
	}
	if (attr->length + (char *)attr <= dataend)	/* Sanity check */
	    attr =(radius_attrtype *) (attr->length + (char *)attr);
	else {
	    radius_bad_attr(u, attr, "Bad length");
	    error = TRUE;
	    attr = (radius_attrtype *)dataend;	/* Abort the scan */
	}
    }
    if (error) {
	return;
    }
    /*
     * Now contruct our output AV pairs
     * Since this is for an exec authorization, this usually an autocommand
     */
    switch (type) {
      case RAD_SERV_FRAMED:
	if (command)
	    sprintf(autocommand, "%s %s %s", command, f_switches, args);
	command = NULL;
	/* Fall through */
      case RAD_SERV_LOGIN:
	if (command)
	    sprintf(autocommand, "%s %s %s %s", command, args, args2, l_switches);
	if (autocommand[0] == 0)
	    break;
	if (radius_debug)
	    buginf("\nRadius: Constructed \"%s\"", autocommand);
	if (!aaa_add_AV(&author->in_args, &avail, AAA_ATTR_noescape,
			AAA_VAL_true, AAA_AV_MAND))
	    return;
	if (!aaa_add_AV(&author->in_args, &avail, AAA_ATTR_autocmd,
			autocommand, AAA_AV_MAND))
	    return;		/* Error */
	/*
	 * Add our autocommands to existing line config
	 */
	author->status = AAA_AUTHOR_STATUS_PASS_ADD;
      case RAD_SERV_EXEC:
      case RAD_SERV_ADMIN:
	/*
	 * Allow exec access by not specifying any autocommand
	 */
	author->status = AAA_AUTHOR_STATUS_PASS_ADD;
	break;
    }
    *result = TRUE;
}

/*
 * radius_config_author
 * extract radius authorization data for a configuration event.
 */
static void
radius_config_author (boolean *result, userstruct *u, authorstruct *author)
{
    const radiustype *rad = u->savedauthor;
    radius_attrtype *attr;
    char *dataend;
    boolean error = FALSE;	/* unrecoverable error? */
    int avail = 0;
    char *proto;

    if (rad == NULL)		/* No saved data is an error */
	return;

    proto = aaa_get_value(author->out_args, AAA_ATTR_protocol);
    if (!proto) {
	proto = AAA_VAL_PROT_ip;
    }

    attr = (radius_attrtype *) rad->rad_attributes;
    dataend = rad->rad_length + (char *)rad;
    while ((char *)attr < dataend) {
	switch (attr->type) {

	  case RAD_ATTR_SERVICE:
	      if (GETLONG(&attr->val.num) == RAD_SERV_OUTB) {
		if (attr->length + (char *)attr > dataend) { /* Sanity check */
		    error = TRUE;
		}
	      } else {
		radius_bad_attr(u, attr, "unsupported attribute");
		error = TRUE;
	      }
	      break;
	  case RAD_ATTR_VS1:
	    if (GETLONG(&attr->val.num) == OID_CISCO) {
		if (attr->length + (char *)attr > dataend) { /* Sanity check */
		    error = TRUE;
		    break;
		}
		if (radius_cisco_vs(proto, attr, &author->in_args, &avail))
		    break;
		/* else fall through */
	    } else {
		if (radius_debug)
		    buginf("\nRadius: unrecognized Vendor code %d",
		       GETLONG(&attr->val.num));
		break;
	    }
	default:
	    /* Else fall through */
	    radius_bad_attr(u, attr, "unsupported attribute");
	    error = TRUE;
	    break;
	}
	if (attr->length + (char *)attr <= dataend)	/* Sanity check */
	    attr =(radius_attrtype *) (attr->length + (char *)attr);
	else {
	    radius_bad_attr(u, attr, "Bad length");
	    error = TRUE;
	    attr = (radius_attrtype *)dataend;	/* Abort the scan */
	}
    }
    if (error) {
	return;
    }
    /*
     * from here on is the success case
     */

    *result = TRUE;
    if (avail) {
	/*
	 * We came up with some attributes to set
	 */
	author->status = AAA_AUTHOR_STATUS_PASS_REPL;
    } else {
	author->status = AAA_AUTHOR_STATUS_PASS_ADD;
    }
}

/*
 * valid_proto_acl
 * Checks the range of the acl against the valid range for
 * the given protocol
 *
 */
static boolean
valid_proto_acl (char *proto, char *val)
{
    int num;

    num = atoi(val);
    
    if (STREQ(proto, AAA_VAL_PROT_ip)) {
	if ((num < MINFASTACC) || (num > MAXSLOWACC)) {
	    return(FALSE);
	}
    } else if (STREQ(proto, AAA_VAL_PROT_ipx)) {
	if ((num < MINNOVACC) || (num > MAXENOVACC)) {
	    return(FALSE);
	}
    } else if (STREQ(proto, AAA_VAL_PROT_atalk)) {
	if ((num < MINATALKACC) || (num > MAXATALKACC)) {
	    return(FALSE);
	}
    } else if (STREQ(proto, AAA_VAL_PROT_deccp)) {
	if ((num < MINDECACC) || (num > MAXDECACC)) {
	    return(FALSE);
	}
    } else {
	if (radius_debug) {
	    buginf("\nRadius: unknown proto \"%s\" in acl-check",
		   proto);
	}
	return(FALSE);
    }

    return(TRUE);
}

/*
 * radius_net_author
 * extract radius authorization data for a network event.
 * this handles address assigmnet for PPP (including ISDN)
 */
static void
radius_net_author (boolean *result, userstruct *u, authorstruct *author)
{
    const radiustype *rad = u->savedauthor;
    radius_attrtype *attr;
    char *dataend;
    boolean error = TRUE;	/* unrecoverable error? */
    int avail = 0;
    char args[20];
    ipaddrtype ip = 0, mask;
    char *proto, *addr;

    proto = aaa_get_value(author->out_args, AAA_ATTR_protocol);
    if (!proto)
	proto = "ppp";		/* hopefully a meaningful default? */

    if (rad == NULL)		/* No saved data is an error */
	return;

    attr = (radius_attrtype *) rad->rad_attributes;
    dataend = rad->rad_length + (char *)rad;
    while ((char *)attr < dataend) {
	switch (attr->type) {
	  default:
	    break;
	  case RAD_ATTR_SERVICE:
	    if (GETLONG(&attr->val.num) == RAD_SERV_FRAMED) 
		error = FALSE;
	    /*
	     * check what these mean ???
	     */
	    break;
	    
	  case RAD_ATTR_FIP:
	    if (bcmp(proto, AAA_VAL_PROT_ip, sizeof AAA_VAL_PROT_ip) != 0)
		break;	/* Only provide IP address for IP authorization */
	    ip = GETLONG(&attr->val.addr);
	    if (ip == RAD_FIP_USERPICK) {
		/*
		 * In the case of userpick, we're going to accept the
		 * address that is already in the avpair list without change.
		 */
		addr = aaa_get_value(author->out_args, AAA_ATTR_addr);

		if (radius_debug)
		    buginf("\nRadius: allowing negotiated framed address %s",
			   addr ? addr : "");

		if (addr) {
		    if (!aaa_add_AV(&author->in_args, &avail, 
				    AAA_ATTR_addr, addr, AAA_AV_MAND)) {
			return;		/* Fatal error */
		    }
		}
	    } else {
		if (ip == RAD_FIP_NASPICK) {
		    /*
		     * In the case of naspick, we'll already have loaded up 
		     * default value as the peer address.
		     */
		    ip = reg_invoke_ip_peer_addr(author->auth_idb->firstsw);
		    if (radius_debug)
			buginf("\nRadius: Using NAS default peer");
		}
		/*
		 * We only need to be here if we had a PPP login that is
		 * now getting a numeric address from the radius dialog.
		 */
		sprintf(args, "%i", ip);
		if (!aaa_add_AV(&author->in_args, &avail, AAA_ATTR_addr,
				args, AAA_AV_MAND))
		    return;		/* Fatal error */
		if (radius_debug)
		    buginf("\nRadius: Authorize IP address %i", ip);
	    }
	    break;
	  case RAD_ATTR_FROUTING:
	    if (!aaa_add_AV(&author->in_args, &avail, AAA_ATTR_routing,
			AAA_VAL_true, AAA_AV_OPT))
		return;		/* Fatal error */
	    break;
	  case RAD_ATTR_FCOMP:
	    break;
	  case RAD_ATTR_FILTER:
	    {
		char tmp = attr->val.str[attr->length-2];
		char *suffix;

		
		if (bcmp(proto, AAA_VAL_PROT_lcp,sizeof AAA_VAL_PROT_lcp) == 0)
		    break;	/* don't provide acls to lcp */
		attr->val.str[attr->length-2] = 0;
		suffix = strchr(attr->val.str, '.');
		if (suffix) *suffix = 0;

		/* check the acl for validity in the context of the proto */
		if (!valid_proto_acl(proto, attr->val.str)) {
		    if (radius_debug) {
			buginf("\nRadius: Filter-Id %s out of range for "
			       "protocol %s. Ignoring.",
			       attr->val.str, proto);
		    }
		    attr->val.str[attr->length-2] = tmp;
		    if (suffix) *suffix = '.';
		    break;  /* the access-list is not for this protocol
			     * ignore it */
		}
		
		if ((suffix == 0) || (strcasecmp(suffix+1, "out") == 0)) {
		    if (!aaa_add_AV(&author->in_args, &avail, AAA_ATTR_outacl,
				    attr->val.str, AAA_AV_MAND)) {
			attr->val.str[attr->length-2] = tmp;
			*suffix = '.';
			return;
		    }
		} else if (strcasecmp(suffix+1, "in") == 0) {
		    if (!aaa_add_AV(&author->in_args, &avail, AAA_ATTR_inacl,
				    attr->val.str, AAA_AV_MAND)) {
			attr->val.str[attr->length-2] = tmp;
			*suffix = '.';
			return;
		    }
		}
		attr->val.str[attr->length-2] = tmp;
		if (suffix) *suffix = '.';
	    }
	    break;

	  case RAD_ATTR_FPROT:
	  case RAD_ATTR_FMTU:
	    /*
	     * Packet mode stuff
	     */
	    break;

	  case RAD_ATTR_LHOST:
	  case RAD_ATTR_LSERV:
	  case RAD_ATTR_LPORT:
	    break;

	  case RAD_ATTR_FMASK:
	    if (bcmp(proto, AAA_VAL_PROT_ip, sizeof AAA_VAL_PROT_ip) != 0)
		break;
	    /*
	     * handle a Framed-mask by generating the appropriate route
	     * text, and using the code built to handle framed-routes.
	     */
	    if (ip) {
		radius_attrtype *fake = (radius_attrtype *)string_getnext();
		mask = GETLONG(&attr->val.num);
		if (radius_debug)
		    buginf("\nRADIUS: Framed-IP-Netmask %i", mask);
		if (mask == MASK_HOST)
		    break;	/* per-user host mask assumed */
		fake->length = 2 + sprintf(fake->val.str, "%i %i",
					   ip & mask, mask);
		fake->type = RAD_ATTR_FROUTE;
		radius_handle_froute(proto, fake, &author->in_args, &avail);
	    } else {
		if (radius_debug)
		    buginf("\nRADIUS: Framed-IP-Netmask without Framed-IP-Address");
	    }
	    break;
	  case RAD_ATTR_FROUTE:
	    /*
	     * try to handle a per-user route.  This is complex, and
	     * has its own routine.  Currently, only IP routes are supported.
	     */
	    if (bcmp(proto, AAA_VAL_PROT_ip, sizeof AAA_VAL_PROT_ip) != 0)
		break;
	    if (attr->length + (char *)attr > dataend) /* Sanity check */
		break;
	    radius_handle_froute(proto, attr, &author->in_args, &avail);
	    break;

	  case RAD_ATTR_VS1:
	    if (GETLONG(&attr->val.num) == OID_CISCO) {
		if (attr->length + (char *)attr > dataend) /* Sanity check */
		    break;
		if (radius_cisco_vs(proto, attr, &author->in_args, &avail))
		    break;
		/* else fall through */
	    } else {
		if (radius_debug)
		    buginf("\nRadius: unrecognized Vendor code %d",
		       GETLONG(&attr->val.num));
		break;
	    }
	    /* Else fall through */

	  case RAD_ATTR_CBNUM:
	  case RAD_ATTR_CBNAM:
	  case RAD_ATTR_FIPXN:
	  case RAD_ATTR_ATIMOUT:
	  case RAD_ATTR_ITIMOUT:
	  case RAD_ATTR_LLATS:
	  case RAD_ATTR_LLATN:
	  case RAD_ATTR_LLATG:
	  case RAD_ATTR_FATNET:
	  case RAD_ATTR_FATZONE:
	  case RAD_ATTR_UNA1:
	  case RAD_ATTR_UNA2:
	    radius_bad_attr(u, attr, "unsupported attribute");
	  case RAD_ATTR_CLASS:
	    break;
	}
	if (attr->length + (char *)attr <= dataend)	/* Sanity check */
	    attr =(radius_attrtype *) (attr->length + (char *)attr);
	else {
	    radius_bad_attr(u, attr, "Bad length");
	    error = TRUE;
	    attr = (radius_attrtype *)dataend;	/* Abort the scan */
	}
    }
    if (error) {
	return;
    }
    /*
     * from here on is the success case
     */

    *result = TRUE;
    author->status = AAA_AUTHOR_STATUS_PASS_REPL;
}

/* 
 * If an authorization call is made without radius having made an
 * authentication call, do the authentication first. The username and
 * password should already be available in the userstruct for this to
 * succeed. 
 */
static void
radius_do_authen (boolean *result, userstruct *u, authorstruct *author)
{
    authenstruct authen;

    memset(&authen, 0, sizeof(authen));

    if (radius_debug) {
	buginf("\nRadius: authenticating to get author data");
    }
    radius_cont_login(result, u, &authen);
    if (radius_debug) {
	buginf("\nRadius: authen request status = %d", authen.status);
    }
}


/*
 * radius_do_author
 * do radius authorization.  Process the attributes that we saved from the
 * authentication for those services that are appropriate.  Otherwise return
 * an error condition so that AAA will step to another method. 
 */
static void
radius_do_author (boolean *result, userstruct *u, authorstruct *author)
{
    *result = FALSE;		/* Default */
    author->status = AAA_AUTHOR_STATUS_ERROR;
    setstring(&author->server_msg, NULL);
    setstring(&author->server_admin_msg, NULL);

    if (u && author) {
	switch (author->type) {
	  case AAA_AUTHOR_TYPE_SHELL:
	    if (!u->savedauthor && u->acc_method == AAA_ACC_METH_NONE) {
		radius_do_authen(result, u, author);
	    }
	    radius_shell_author(result, u, author);
	    break;
	  case AAA_AUTHOR_TYPE_NET:
	    if (!u->savedauthor && u->acc_method == AAA_ACC_METH_NONE) {
		radius_do_authen(result, u, author);
	    }
	    if (u->authen_config) {
		radius_config_author(result, u, author);
	    } else {
		radius_net_author(result, u, author);
	    }
	    break;
	  case AAA_AUTHOR_TYPE_CONN:
	  case AAA_AUTHOR_TYPE_CMD:
	  default:
	    break;
	}
    }
    return;
}

/*
 * radius_convert_acctinfo
 * Convert protocol independent accounting info into radius attributes.
 */
static const uchar attr_ids[AAA_ATTR_MAX] = {
    0,			/* unknown */
    RAD_ATTR_FPROT,	/* "service" */
    0,			/* "start_time" */
    0,			/* "port" */
    RAD_ACCT_STIME,	/* "elapsed_time" */
    0,			/* "status" */
    0,			/* "priv_level" */
    0,			/* "cmd" */
    -1,			/* "protocol" */
    0,			/* "cmdarg" */
    RAD_ACCT_INBYTES,	/* "bytes_in" */
    RAD_ACCT_OUTBYTES,	/* "bytes_out" */
    RAD_ACCT_INPAKS,	/* "paks_in", */
    RAD_ACCT_OUTPAKS,	/*"paks_out" */
    -1,			/* "address", */
    RAD_ACCT_SESSID,	/* "task_id" */
    0,			/* "callback-dialstring", */
    0,			/* "nocallback-verify", */
    0,			/* "callback-line" */
    0,			/* "callback-rotary" */
    0,			/* "reason" */
    -1,			/* "event" */
    0,			/* "timezone"*/
};


static radius_attrtype 
*radius_convert_acctinfo (radius_attrtype *rad, userstruct *u, acctstruct *a)
{
    uchar c_attr;
    char c_val[30];
    int length;
    struct acct_attr_ *attr;
    uchar *start, *end;
    boolean ipprot = TRUE;
    boolean ipaddrseen = FALSE;

    start = a->info->acct_data;
    /*
     * For start packets, make sure we only send as much as was
     * indicated by the sending process
     */
    if (a->flags & ACCT_FLAG_START) {
	end = a->info->acct_end_write ? a->info->acct_end_write :
	    a->info->acct_dptr;
    } else {
	end = a->info->acct_dptr;
    }

    while (start < end) {
	attr = (struct acct_attr_ *) start;
	c_attr = attr_ids[attr->attr_ident];
	length = attr->value_length;
	if (c_attr) {
	    switch (attr->value_fmt) {
	      case AAA_TYPE_BYTE:
	      case AAA_TYPE_INT:
	      case AAA_TYPE_IP:
		PUTLONG(c_val,GETLONG(attr->attr_value));
		break;
	      case AAA_TYPE_STR:
		sprintf(c_val, "%s",attr->attr_value);
		break;
	      default:
		bcopy("BADF", c_val, 4);
		length = 4;
		break;
	    }
	    switch (c_attr) {
		/*
		 * If this attribute exists within the radius protocol,
		 * proceed with protocol conversion
		 */
	      case RAD_ATTR_FPROT:
		if (a->info->type == ACCT_TYPE_NET) {
		    if (strcmp(AAA_VAL_SVC_slip, c_val) == 0)
			PUTLONG(c_val, RAD_FPROT_SLIP);
		    else if (strcmp(AAA_VAL_SVC_ppp, c_val) == 0)
			PUTLONG(c_val, RAD_FPROT_PPP);
		    length = 4;
		} else
		    length = 0;
		break;
	      case RAD_ACCT_SESSID:
		if (radius_highident)
		    length = sprintf(c_val, "%02x%06x", radius_highident,
				     GETLONG(c_val) & 0x00FFFFFF);
		else
		    length = sprintf(c_val, "%08x", GETLONG(c_val));
		break;
	      case RAD_ACCT_STIME:
	      case RAD_ACCT_INBYTES:
	      case RAD_ACCT_OUTBYTES:
	      case RAD_ACCT_INPAKS:
	      case RAD_ACCT_OUTPAKS:
		break;
	      case (uchar)-1:
		/*
		 * assorted special cases
		 */
		switch (attr->attr_ident) {
		  case AAA_ATTR_PROTO:
		    if (a->info->type == ACCT_TYPE_NET) {
			/*
			 * radius only knows about ip addresses. if we have
			 * another type of protocol address coming, we won't
			 * know what do do with it.  
			 * Remember this so we don't burp.
			 */
			if (strcmp(AAA_VAL_PROT_ip, c_val) != 0)
			    ipprot = FALSE;
			length = 0;
		    } else if (a->info->type == ACCT_TYPE_CONN) {
			c_attr = RAD_ATTR_LSERV;
			length = 4;
			if (strcmp(AAA_VAL_PROT_telnet, c_val) == 0) {
			    PUTLONG(c_val, RAD_LSERV_TN);
			} else if (strcmp(AAA_VAL_PROT_rlogin, c_val) == 0) {
			    PUTLONG(c_val, RAD_LSERV_RLOG);
			} else if (strcmp(AAA_VAL_PROT_lat, c_val) == 0) {
			    PUTLONG(c_val, RAD_LSERV_LAT);
			    ipprot = FALSE;
			} else if (strcmp(AAA_VAL_PROT_tn3270, c_val) == 0) {
			    PUTLONG(c_val, RAD_LSERV_TN);
			} else {
			    length = 0;
			    ipprot = FALSE;
			}
		    }
		    break;
		  case AAA_ATTR_ADDR:
		    if (ipprot) {
			if (ipaddrseen) {
			    if (radius_debug)
				buginf("\nRadius: multiple Framed-IP-Address omitted");
			    length = 0;
			    break;
			}
			ipaddrseen = TRUE;
			if (a->info->type == ACCT_TYPE_CONN)
			    c_attr = RAD_ATTR_LHOST;
			else if (a->info->type == ACCT_TYPE_NET)
			    c_attr = RAD_ATTR_FIP;
			break;
		    } else if (a->info->type == ACCT_TYPE_CONN) {
			c_attr = RAD_ATTR_LLATS;
			/* note length is already set! */
			break;
		    }

		    /* Else fall through */
		  case AAA_ATTR_EVENT:
		    length = 0;
		    break;
		}
		break;
	      default:
		if (radius_debug)
		    buginf("\nRadius: unknown acct attribute conversion"
			   " for \"%s\"", aaa_attr_names[attr->attr_ident]);
	    }
	    if (length > 0) {
		rad->type = c_attr;
		rad->length = length + 2;
		bcopy(c_val, rad->val.str, length);
		rad = (radius_attrtype *) (rad->length + (char *) rad);
	    }
	}
	start = (start + sizeof(struct acct_attr_) + attr->value_length)-1;
    }
    return(rad);
}

/*
 * radius_get_attr
 * try to get a pointer to a specific radius attribute in the saved
 * authorization data for a user.  Makes sure that the user was authenticated
 * using radius.
 */
static radius_attrtype *
radius_get_attr (userstruct *u, int type)
{
    radius_attrtype *attr;
    const radiustype *rad;
    char *end;

    if (!u || u->acc_method != AAA_ACC_METH_RADIUS)
	return(NULL);
    rad = u->savedauthor;
    if (rad) {
	end = rad->rad_length + (char *)rad;
	attr = (radius_attrtype *)rad->rad_attributes;
	while ((char *)attr < end) {
	    if (attr->type == type)
		return(attr);
	    attr = (radius_attrtype *) (attr->length + (char *) attr);
	}
    }
    return(NULL);
}

/*
 * radius_do_accounting
 * Do accounting records via radius
 */
static void 
radius_do_accounting (boolean *result, userstruct *u, acctstruct *a)
{
    radius_ctx *rctx;
    radiustype *rad;
    radius_attrtype *attr, *class;
    paktype *pak;
    int service;
    int authentic;

    *result = FALSE;		/* default is failure */

    if (radius_fork == 0) {
	if (radius_debug || aaa_acct_debug) {
	    buginf("\nRadius:  Accounting requested, but no radius host");
	}
	a->status = AAA_ACCT_STATUS_ERROR;
	return;
    }
    
    if (aaa_acct_suppress_null_username && null(u->user)) {
	if (radius_debug || aaa_acct_debug) {
	    buginf("\nRADIUS:  Ignoring NULL-USER accounting record");
	}
	*result = TRUE;
	a->status = AAA_ACCT_STATUS_SUCCESS;
	return;
    }

    setstring(&a->server_msg, NULL);
    setstring(&a->server_admin_msg, NULL);

    /*
     * Based on type of accounting event, determin whether or not we can
     * do it using radius, and what sort of data will be supported
     */
    switch(a->info->type) {
      case ACCT_TYPE_EXEC:
	service = RAD_SERV_EXEC;
	break;
      case ACCT_TYPE_CONN:
	service = RAD_SERV_LOGIN;
	break;
      case ACCT_TYPE_NET:
	service = RAD_SERV_FRAMED;
	break;
      case ACCT_TYPE_CMD:
      case ACCT_TYPE_SYS:
      default:
	if (radius_debug)
	    buginf("\nRADIUS: unsupported accounting type %d for user \"%s\"", 
		   a->info->type, u->user);
	a->status = AAA_ACCT_STATUS_ERROR;
	return;
    }
    if (a->flags & ACCT_FLAG_WATCHDOG) {
        /*
         * The RADIUS support for "update" records was removed from the
	 * specification, so we shouldn't send any.  Note that code below
	 * this still knows how to send such records, in case it goes back
	 * into the specification at a later time.
	 */
	if (radius_debug)
	    buginf("\nRADIUS: ACCT Watchdog type %d for user \"%s\" deprecated", 
		   a->info->type, u->user);
	a->status = AAA_ACCT_STATUS_ERROR;
	return;
    }


    rctx = malloc_named(sizeof(radius_ctx), "RADIUS ctx");
    if (!rctx)
	return;

    rctx->pak = pak = getbuffer(MEDDATA); 
    if (pak == NULL) {
	free(rctx);
	return;
    }
    rctx->ident = radius_idents++;
    rctx->user = u;

    rad = (radiustype *)(pak->datagramstart + IPHEADERBYTES(NOPT) +
			 UDPHEADERBYTES);
    pak->application_start = (char *)rad;
    rad->rad_code = RAD_CODE_ACCT_REQUEST;
    rctx->type = RAD_CODE_ACCT_REQUEST;
    rad->rad_ident = rctx->ident;
    attr = (radius_attrtype *) &rad->rad_attributes[0];
    attr = radius_std_attributes(attr, u);
    if (!attr) {
	a->status = AAA_ACCT_STATUS_ERROR;
	return;
    }

    attr->type = RAD_ACCT_STATTYPE;
    attr->length = 6;
    switch (a->flags & ~ACCT_FLAG_MORE) {
      case ACCT_FLAG_START:
	PUTLONG(&attr->val.num, RADACCT_STAT_START);
	break;
      case ACCT_FLAG_STOP:
	PUTLONG(&attr->val.num, RADACCT_STAT_STOP);
	break;
      case ACCT_FLAG_WATCHDOG:
	  /*
	   * note that watchdog (or "update") records were deprecated in
	   * the radius specfication, so they are filtered out before we
	   * get here.  The code stays cause they might come back someday.
	   */
	PUTLONG(&attr->val.num, RADACCT_STAT_WATCH);
	break;
      default:
	datagram_done(rctx->pak);
	free(rctx);
	return;
    }
    attr = (radius_attrtype *) (attr->length + (char *) attr);

    class = radius_get_attr(u, RAD_ATTR_CLASS);
    if (class) {
	if (radius_debug)
	    buginf("\nRADIUS: Sent class \"%*s\" at %lx from user %lx",
		   class->length-2, class->val.str, class, u);
	bcopy(class, attr, class->length);
	attr = (radius_attrtype *) (attr->length + (char *) attr);
    }
    switch (u->acc_method) {
      case AAA_ACC_METH_RADIUS:
	authentic = RADACCT_AUTHEN_RADIUS;
	break;
      case AAA_ACC_METH_LINE:
      case AAA_ACC_METH_ENABLE:
      case AAA_ACC_METH_LOCAL:
      case AAA_ACC_METH_IFNEEDED:
      case AAA_ACC_METH_RCMD:
	authentic = RADACCT_AUTHEN_LOCAL;
	break;
      case AAA_ACC_METH_TACACSPLUS:
      case AAA_ACC_METH_KRB5:
	authentic = RADACCT_AUTHEN_REMOTE;
	break;
      case AAA_ACC_METH_GUEST:
      case AAA_ACC_METH_AUTHGUEST:
      case AAA_ACC_METH_NOT_SET:
      case AAA_ACC_METH_NONE:
      default:
	authentic = RADACCT_AUTHEN_NONE;
	break;
    }
    if (authentic != RADACCT_AUTHEN_NONE) {
	attr->type = RAD_ACCT_AUTHEN;
	attr->length = 6;
	PUTLONG(&attr->val.num, authentic);
	attr = (radius_attrtype *) (attr->length + (char *) attr);
    }
    attr->type = RAD_ATTR_SERVICE;
    attr->length = 6;
    PUTLONG(&attr->val.num, service);
    attr = (radius_attrtype *) (attr->length + (char *) attr);

    attr = radius_convert_acctinfo(attr, u, a);


    /*
     * Add an Acct-delay-time attribute as the last field
     * in the packet. Initially the delay time will be 0,
     * but if there are retransmissions then having the
     * attribute already in place makes handling of
     * this attribute easy.
     */
    attr->type = RAD_ACCT_DELAY;
    attr->length = 6;
    attr->val.num = 0;
    attr = (radius_attrtype *) (attr->length + (char *) attr);

    pak->length = rad->rad_length = ((char *)attr) - ((char *)rad);
    radius_encrypt(rctx, rad, NULL);
    if (a->act == AAA_ACCT_ACT_WAIT_START) {
	rctx->done = create_watched_boolean("Radius IPC", 0);
	process_set_boolean(rctx->done, FALSE);
	if (rctx->done == NULL) {
	    datagram_done(rctx->pak);
	    free(rctx);
	    return;
	}
	process_enqueue(radius_inQ, rctx);
	process_push_event_list(NULL);
	process_watch_boolean(rctx->done, ENABLE, TRUE);
	process_wait_for_event();
	delete_watched_boolean(&rctx->done);		/* Delete references */
	process_pop_event_list(NULL);
    } else {
	/*
	 * Normal accounting doesn't stop to wait for responses from the
	 * server, just assume success and return to caller.
	 */
	rctx->done = NULL;	/* No watched boolean! */
	process_enqueue(radius_inQ, rctx);
	a->status = AAA_ACCT_STATUS_SUCCESS;
	return;
    }
    /*
     * rctx->pak, which used to contain our request, should now contain
     * the reply from the radius server instead.
     */
    if (pak == rctx->pak) {
	/*
	 * We don't have a reply packet.  This means that we didn't get
	 * any response from the server.  Return this result.
	 */
	datagram_done(pak);
	if (radius_debug)
	    buginf("\nRadius: No accounting response received");
	a->status = AAA_ACCT_STATUS_ERROR;
	free(rctx);
	return;
    }
    /*
     * We have a response to our request
     */
    if (rctx->pak == NULL) {
	a->status = AAA_ACCT_STATUS_ERROR;
	free(rctx);
	return;
    }

    pak = rctx->pak;
    rad = (radiustype *)pak->application_start;
    if (radius_decrypt(rctx, rad)) {
	switch (rad->rad_code) {
	  case RAD_CODE_ACCT_RESPOND:
	    a->status = AAA_ACCT_STATUS_SUCCESS;
	    *result = TRUE;
	    break;
	  default:
	    a->status = AAA_ACCT_STATUS_ERROR;
	    break;
	} /* case */
    } else {
	if (radius_debug)
	    buginf("\nRadius: Reply for %d fails decrypt", rad->rad_ident);
	a->status = AAA_ACCT_STATUS_ERROR;
    }
    datagram_done(pak);
    free(rctx);
}

/*
 * radius_encrypt
 * Do radius "encryption".  This involves building the authenticator string,
 * hashing it through MD5 and xoring the password with the result.
 */
static void
radius_encrypt (radius_ctx *ctx, radiustype *rad, radius_attrtype *password)
{
    int i;
    uchar output[MD5_LEN];
    MD5_CTX mdcontext;

    if (rad->rad_code == RAD_CODE_ACCT_REQUEST) {
	/*
	 * "Encryption" of accounting packets completely different!
	 */
	memset(rad->rad_authent, 0, sizeof rad->rad_authent);
	MD5Init(&mdcontext);
	MD5Update(&mdcontext, (uchar *) rad, rad->rad_length);
	MD5Update(&mdcontext, radius_secret, radius_secret_length);
	MD5Final(&rad->rad_authent[0], &mdcontext);
	return;
    }
    /*
     * Build "authenticator" - random 16 bytes.  Show not be repeated
     * or predictable.
     */
    random_fill(rad->rad_authent, 16, TRUE);
    bcopy(rad->rad_authent, ctx->authent, 16);

    /*
     * Build MD5 hash of authenticator and our secret.
     */
    MD5Init(&mdcontext);
    MD5Update(&mdcontext, radius_secret, radius_secret_length);
    MD5Update(&mdcontext, rad->rad_authent, sizeof (rad->rad_authent));
    MD5Final(&output[0], &mdcontext);
    /*
     * Encrypt the user's password with the MD5 hash
     */
    for (i=0; i < (password->length - 2); i++) {
	if (i > 0 && (i % MD5_LEN) == 0) {
	    /*
	     * After we've hidden 16 bytes of password, we have to
	     * compute a new MD5 hash, using our secret + prev result.
	     */
	    if (radius_debug)
		buginf("RADIUS: Long password processing");
	    MD5Init(&mdcontext);
	    MD5Update(&mdcontext, radius_secret, radius_secret_length);
	    MD5Update(&mdcontext, &password->val.str[i-MD5_LEN], MD5_LEN);
	    MD5Final(&output[0], &mdcontext);
	}
	password->val.str[i] ^= output[i%MD5_LEN];
    }
}

static boolean radius_decrypt (radius_ctx *ctx, radiustype *rad)
{
    char savedauth[16], output[MD5_LEN];
    char *input;
    MD5_CTX mdcontext;
    boolean result;

    /*
     * Save packet headers before the radius packet itself (yes, we're
     * pretty sure that between udp, ip, and encapsulation, there is little
     * danger of running off the beginning of the data.
     * Also save the current authenticator for latter comparison.
     */
    bcopy(rad->rad_authent, savedauth, 16);
    bcopy(ctx->authent, rad->rad_authent, 16);

    input = (char *)rad;
    bcopy(radius_secret, input + rad->rad_length, radius_secret_length);
    /*
     * "input" now points to the radius packet concatted with our secret
     * with the original authenticator.  Calculate an MD5 hash of this,
     * and that should equal the authenticator that WAS in the packet
     */
    MD5Init(&mdcontext);
    MD5Update(&mdcontext, input, rad->rad_length + radius_secret_length);
    MD5Final(&output[0], &mdcontext);
    result = !bcmp(output, savedauth, 16);
    return(result);
}

/*
 * radius_abort_login
 * Abort a login in the middle.  Clean up any intermediate data structures
 * that we may have created.
 */
static void
radius_abort_login (userstruct *u, authenstruct *a, char *reason)
{
    setstring(&a->server_msg, NULL);
    setstring(&a->server_data, NULL);
    /*
     * Saved authorization stuff ????
     */
}


/*
 * radius_cfgcmd
 * handle the configuration options for radius.  This is mostly copied
 * from tacacs, but a lot of options are handled by AAA instead, so
 * radius is simpler.  (ie "tacacs last-resort")
 *
 * Also handles "ip radius source-interface <subint>" command
 */

void radius_cfgcmd (parseinfo *csb)
{
    radius_servertype *lp;

    if (csb->nvgen) {
	if (!radius_hostQ.qhead) {
	    return;
	}
	switch (csb->which) {
	  case RADIUS_SECRET:
	    nv_write(radius_secret_length, "%s %s", csb->nv_command,
		     radius_secret);
	    break;
	  case RADIUS_HOST:
	    for (lp = (radius_servertype *)radius_hostQ.qhead;
		 lp; lp = lp->next) {
		nv_write(TRUE, "%s %i", csb->nv_command, lp->addr);
		nv_add(TRUE, " auth-port %d", lp->authport);
		nv_add(TRUE, " acct-port %d", lp->acctport);
	    }
	    break;
	  case RADIUS_RETRANSMIT:
	    nv_write(radius_retrans != RADIUS_RETRANS_DEFAULT, "%s %d",
		     csb->nv_command, radius_retrans);
	    break;
	  case RADIUS_TIMEOUTVALUE:
	    nv_write(radius_timeout != RADIUS_TIMEOUT_DEFAULT, "%s %d",
		     csb->nv_command, radius_timeout/ONESEC);
	    break;
	  case RADIUS_DEADTVALUE:
	    nv_write(radius_deadtime != RADIUS_DEADTIME_DEFAULT, "%s %d",
		     csb->nv_command, radius_deadtime);
	    break;
	  case RADIUS_PASSWORDOPT:
	    nv_write(radius_pw_optional, csb->nv_command);
	    break;
	  case RADIUS_DIRECTED_REQUEST:
	    nv_write(radius_directed_request, "%s", csb->nv_command);
	    break;
	  case RADIUS_SOURCE_INTERFACE:
	    if (radius_source_idb) {
		nv_write(TRUE, "%s %s", csb->nv_command,
			 radius_source_idb->namestring);
	    }
	    break;
	  case RADIUS_UNIQUEID:
	    nv_write(radius_highident, "%s %d", csb->nv_command,
		     radius_highident);
	    break;
	  case RADIUS_EXTENDED_PORTNAMES:
	    nv_write(radius_extendedports, csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case RADIUS_HOST:
	for (lp = (radius_servertype *)radius_hostQ.qhead; lp; lp = lp->next) {
	    if (lp->addr == GETOBJ(paddr,1)->ip_addr) {
                if (csb->sense) {
		    unqueue(&radius_hostQ, lp);
                    break;    	/* If adding and already there, modify it */
		}
		/* Remove the entry and free the item. */
                unqueue(&radius_hostQ, lp);
                free(lp);
                return;
            }
	}
	if (!csb->sense) {
	    return;              /* If not found and we are removing, bail */
	}
	/* create or reuse an entry and add it to the list */
	if (!lp)
	    lp = malloc_named(sizeof(radius_servertype), "RADIUS server info");
	if (lp) {
	    lp->addr = GETOBJ(paddr,1)->ip_addr;
	    if (radius_hostQ.qhead == NULL)
		radius_srcaddr = ip_best_local_address(lp->addr, FALSE);
	    lp->authport = GETOBJ(int, 2); /* Defaults handled in parser */
	    lp->acctport = GETOBJ(int, 3);
	    enqueue(&radius_hostQ, lp);
	    radius_startup();
	}
	break;
      case RADIUS_SECRET:
	if (csb->sense) {
	    radius_secret_length = 
		min(RADIUS_SECRETLENGTH, strlen(GETOBJ(string,1)));
	    sstrncpy(radius_secret, GETOBJ(string,1), RADIUS_SECRETLENGTH);
	} else {
	    radius_secret_length = 0;
	    *radius_secret = '\0';
	}
	break;
      case RADIUS_RETRANSMIT:
	if (csb->sense) {
	    radius_retrans = radius_acct_retrans = GETOBJ(int,1);
	} else {
	    radius_retrans = radius_acct_retrans = RADIUS_RETRANS_DEFAULT;
	}
	break;
      case RADIUS_DEADTVALUE:
	if (csb->sense) {
	    radius_deadtime = GETOBJ(int,1);
	} else {
	    radius_deadtime = RADIUS_DEADTIME_DEFAULT;
	}
	break;
      case RADIUS_TIMEOUTVALUE:
	if (csb->sense) {
	    radius_timeout = radius_acct_timeout = GETOBJ(int,1) * ONESEC;
	} else {
	    radius_timeout = radius_acct_timeout = RADIUS_TIMEOUT_DEFAULT;
	}
	break;
      case RADIUS_PASSWORDOPT:
	radius_pw_optional = csb->sense;
	break;
      case RADIUS_DIRECTED_REQUEST:
	radius_directed_request = csb->sense;
	break;
      case RADIUS_SOURCE_INTERFACE:
      if (csb->sense) {
              radius_source_idb = GETOBJ(idb,1);
      } else {   /*   csb->sense */
          radius_source_idb = NULL;
      }
      break;
      case RADIUS_UNIQUEID:
	radius_highident = 1 + GETOBJ(int,1);
	radius_fork = process_create(radius_saveident, "radius nvwrite",
				     NORMAL_STACK, PRIO_NORMAL);
	break;
      case RADIUS_EXTENDED_PORTNAMES:
	radius_extendedports = csb->sense;
	break;
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * radius_debug_init
 * Initialize radius debugging
 */


static void radius_debug_show (void)
{
    debug_show_flag(radius_debug, FALSE, "Radius protocol");
}

static void radius_debug_all (boolean sense)
{
    /*
     * Radius debugging is NOT turned on by "debug all", but we
     * still want it to be turned off by "undebug all".
     */
    if (!sense)
	radius_debug = FALSE;
}

static void radius_debug_init (void)
{
    /* Repeated calls to this routine do nothing. */
    if (radius_debug_inited)
	return;
    (void) reg_add_debug_show(radius_debug_show, "radius_debug_show");
    (void) reg_add_debug_all(radius_debug_all, "radius_debug_all");

    radius_debug_inited = TRUE;
}

static void radius_debug_cmd (parseinfo *csb)
{
    radius_debug = csb->sense;
    debug_show_flag(radius_debug, TRUE, "Radius protocol");
}

/*
 * Parser chains and definitions for radius subsystem
 */

/* * Parse chains for RADIUS config commands */
#define	ALTERNATE NONE
#include "cfg_radius.h"
LINK_POINT(radius_cfg_commands, ALTERNATE);
 #undef	ALTERNATE

/*
 * Parse chains for top-level IP radius configuration commands
 */
#define ALTERNATE     NONE
#include "cfg_ip_radius.h"
LINK_POINT(radius_cfg_ip_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Parse chains for DEBUG commands
 */

EOLNS	(debug_radius_eol, radius_debug_cmd);
KEYWORD	(debug_radius, debug_radius_eol, NONE,
	 "radius", "RADIUS protocol", PRIV_OPR);
LINK_POINT(radius_debug_commands, debug_radius);

static parser_extension_request radius_chain_init_table[] = {
    { PARSE_ADD_CFG_ROUTING_CMD, &pname(radius_cfg_commands) },
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(radius_cfg_ip_commands) },    
    { PARSE_ADD_DEBUG_CMD, &pname(radius_debug_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * RADIUS Initialization and subsystem header
 */
static void 
radius_init (subsystype *subsys)
{
    radius_is_installed = TRUE;

    *radius_secret = 0;
    radius_secret_length = 0;
    radius_source_idb = NULL;
    radius_highident = 0;
    queue_init(&radius_hostQ, 0);

    reg_add_start_login(AAA_ACC_METH_RADIUS, radius_start_login,
			       "radius_start_login");
    reg_add_cont_login(AAA_ACC_METH_RADIUS, radius_cont_login,
			       "radius_cont_login");
    reg_add_abort_login(AAA_ACC_METH_RADIUS, radius_abort_login,
			       "radius_abort_login");
    reg_add_do_author(AAA_AUTHOR_METH_RADIUS, radius_do_author,
		      "radius_do_author");
    reg_add_do_acct(AAA_AUTHOR_METH_RADIUS, radius_do_accounting,
		    "radius_do_acct");
    parser_add_command_list(radius_chain_init_table, "radius");
    radius_debug_init();
}

SUBSYS_HEADER(radius, RADIUS_SUBSYS_MAJVERSION, RADIUS_SUBSYS_MINVERSION,
	      RADIUS_SUBSYS_EDITVERSION,
	      radius_init, SUBSYS_CLASS_LIBRARY,
	      NULL,
	      subsys_req_iphost);

