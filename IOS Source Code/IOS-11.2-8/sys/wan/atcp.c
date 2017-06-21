/* $Id: atcp.c,v 3.3.62.6 1996/09/12 13:31:51 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/atcp.c,v $
 *------------------------------------------------------------------
 * atcp.c - NCP routines for Appletalk over PPP.
 *
 * January 1995, Tim Kolar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *
 * This implemements all the options in RFC 1378.
 *------------------------------------------------------------------
 * $Log: atcp.c,v $
 * Revision 3.3.62.6  1996/09/12  13:31:51  fox
 * CSCdi68759:  AAA per-user routes are not removed when connection drops
 * Branch: California_branch
 * Separate parallel connection support for per-user routes from the
 * support for neighbor routes and dynamic dialer maps.  Transfer
 * ownership of the static route to the last idb to add it (applies only
 * to per-user routes).
 *
 * Revision 3.3.62.5  1996/08/18  09:07:25  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.3.62.4  1996/06/24  09:26:43  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.3.62.3  1996/05/10  01:44:12  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.3.62.2  1996/04/29  09:45:02  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.3.62.1  1996/03/18  22:46:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  11:06:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  21:49:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:00:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:48:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:47:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/22  06:36:55  tkolar
 * CSCdi38902:  ATCP client mode wont work with ISDN
 * Change method of handing out virtual-net node addresses to accomodate
 * arbitrary interfaces, and prevent non P2P interfaces from being
 * configured for Appletalk client mode.
 *
 * Revision 2.1  1995/06/07  23:16:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ppp.h"
#include "ieee.h"
#include "atcp.h"
#include "ppp_public.h"
#include "logger.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_ppp.h"
#include "ppp_registry.h"

static void atcp_resetci(fsm *);	/* Reset our Configuration Information */
static int atcp_cilen(fsm *);		/* Return length of our CI */
static void atcp_addci(fsm *, uchar *);	/* Add our CIs */
static int atcp_ackci(fsm *, uchar *, int);	/* Ack some CIs */
static void atcp_nakci(fsm *, uchar *, int);	/* Nak some CIs */
static void atcp_rejci(fsm *, uchar *, int);	/* Reject some CIs */
static uchar atcp_reqci(fsm *, uchar *, short *);	/* Check the requested CIs */
static void atcp_up(fsm *);		/* Link came up */
static void atcp_closed(fsm *);		/* Link came up */

static fsm_callbacks atcp_callbacks = { /* ATCP callback routines */
    atcp_resetci,		/* Reset our Configuration Information */
    atcp_cilen,			/* Length of our Configuration Information */
    atcp_addci,			/* Add our Configuration Information */
    atcp_ackci,			/* ACK our Configuration Information */
    atcp_nakci,			/* NAK our Configuration Information */
    atcp_rejci,			/* Reject our Configuration Information */
    atcp_reqci,			/* Request peer's Configuration Information */
    atcp_up,			/* Called when fsm reaches OPEN state */
    NULL,			/* Called when fsm leaves OPEN state */
    atcp_closed,		/* Called when fsm reaches CLOSED state */
    NULL,			/* Called when Protocol-Reject received */
    NULL,			/* Retransmission is necessary */
	NULL,			/* Reset Request */
	NULL,			/* Rest Acknowledge */
	NULL			/* Code Rejected */
};

/*
 * atcp_init - Initialize ATCP.
 */
static void atcp_init (fsm *atcp_fsm)
{
    atcp_options *wo;
    atcp_options *ao;

    if (ppp_negotiation_debug)
	buginf("\nInitializing ATCP");
    if (!atcp_fsm->wantoptions)
    	atcp_fsm->wantoptions = malloc(sizeof(atcp_options));
    else
	memset(atcp_fsm->wantoptions, 0, sizeof(atcp_options));
    if (!atcp_fsm->wantoptions)
	return;
    if (!atcp_fsm->allowoptions)
    	atcp_fsm->allowoptions = malloc(sizeof(atcp_options));
    else
	memset(atcp_fsm->allowoptions, 0, sizeof(atcp_options));
    if (!atcp_fsm->allowoptions)
	return;
    if (!atcp_fsm->heroptions)
    	atcp_fsm->heroptions = malloc(sizeof(atcp_options));
    else
	memset(atcp_fsm->heroptions, 0, sizeof(atcp_options));
    if (!atcp_fsm->heroptions)
	return;
    atcp_fsm->ouroptions = NULL;

    wo = (atcp_options *) atcp_fsm->wantoptions;
    ao = (atcp_options *) atcp_fsm->allowoptions;

    atcp_fsm->retransmits = 0;
    TIMER_STOP(atcp_fsm->timeouttime);
    atcp_fsm->failures = 0;
    atcp_fsm->callbacks = &atcp_callbacks;
    atcp_fsm->flags |= AOPENDING;           /* Active Open */

}


/*
 * atcp_resetci - Reset our CI.
 */

static void atcp_resetci (fsm *f)
{
    if (ppp_negotiation_debug)
	buginf("\nResetting ATCP");

    at_atcp_info(f->wantoptions, f->allowoptions, f->idb);

    f->gotoptions = f->wantoptions;
}


/*
 * atcp_cilen - Return length of our CI.
 */
static int atcp_cilen (fsm *f)
{
    atcp_options *go;

    go = (atcp_options *)f->gotoptions;

    return (LENCIATADDR(go->neg_address) +
	    LENCIATRPROT(go->neg_routeprot) +
	    LENCIATSUPR(go->neg_supbroad, go->num_suppressed_types) +
	    LENCIATCOMP(go->neg_compprot) +
	    LENCIATSERV(go->neg_server, go->server_name_len) +
	    LENCIATZONE(go->neg_zone, go->zone_name_len) +
	    LENCIATDEFROUT(go->neg_defrouter));
}


/*
 * atcp_addci - Add our desired CIs to a packet.
 */
static void atcp_addci (fsm *f, uchar *ucp)
{
    atcp_options *go;

    go = (atcp_options *)f->gotoptions;

    ADDCIATADDR(CI_AT_ADDRESS, go->neg_address,
		go->default_router_net, go->default_router_node);
    ADDCIATRPROT(CI_AT_ROUTING_PROTOCOL, go->neg_routeprot, 
		go->routing_protocol);
    ADDCIATSUPR(CI_AT_SUPRESSBROADCAST, go->neg_supbroad,
		go->suppressed_types, go->num_suppressed_types);
    ADDCIATCOMP(CI_AT_COMPRESSION, go->neg_compprot, 
		go->compression_protocol);
    ADDCIATSERV(CI_AT_SERVERINFO, go->neg_server,
		go->server_name, go->server_name_len,
		go->server_class, go->implementation_id);
    ADDCIATZONE(CI_AT_ZONEINFO, go->neg_zone,
		go->zone_name, go->zone_name_len);
    ADDCIATDEFROUT(CI_AT_DEFAULT_ROUTER, go->neg_defrouter,
		go->default_router_net, go->default_router_node);
}


/*
 * atcp_ackci - Ack our CIs.
 *
 * Returns:
 *	0 - Ack was bad.
 *	1 - Ack was good.
 *
 * This routine ensures that a received ACK message is valid. An ACK
 * message must contain all of the options that we specified, in the order
 * that we specified.
 */

static int atcp_ackci (fsm *f, uchar *p, int len)
{
    ushort cilen, citype;
    atcp_options *go;

    go = (atcp_options *)f->gotoptions;

    /*
     * CIs must be in exactly the same order that we sent...
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */

    ACKCIATADDR(CI_AT_ADDRESS, go->neg_address,
		go->default_router_net, go->default_router_node);
    ACKCIATRPROT(CI_AT_ROUTING_PROTOCOL, go->neg_routeprot, 
		go->routing_protocol);
    ACKCIATSUPR(CI_AT_SUPRESSBROADCAST, go->neg_supbroad,
		go->suppressed_types, go->num_suppressed_types);
    ACKCIATCOMP(CI_AT_COMPRESSION, go->neg_compprot, 
		go->compression_protocol);
    ACKCIATSERV(CI_AT_SERVERINFO, go->neg_server,
		go->server_name, go->server_name_len,
		go->server_class, go->implementation_id);
    ACKCIATZONE(CI_AT_ZONEINFO, go->neg_zone,
		go->zone_name, go->zone_name_len);
    ACKCIATDEFROUT(CI_AT_DEFAULT_ROUTER, go->neg_defrouter,
		go->default_router_net, go->default_router_node);

    /*
     * If there are any remaining CIs, then this packet is bad.
     */
    if (len != 0)
	goto bad;
    return (1);
bad:
    if (ppp_error_debug)
	buginf("\nppp: atcp_ackci: received bad Ack!");
    return (0);
}


/*
 * atcp_nakci - NAK some of our CIs.
 *
 * This routine handles a received NAK message. The NAK message will
 * list all of the options that we specified, the peer supports, but
 * the peer would prefer a different value.
 */

static void atcp_nakci (fsm *f, uchar *p, int len)
{
    ushort cinet;
    uchar cinode;
    ushort cishort, ciclass;
    ulong ciimplementation = 0;
    uchar ciarray[MAX_CIARRAY_LENGTH];
    atcp_options *go;

    go = (atcp_options *)f->gotoptions;

    /*
     * Any Nak'd CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */

    NAKCIATADDR(CI_AT_ADDRESS, go->neg_address,
		if (ppp_negotiation_debug)
		     buginf("\nppp: %s ATCP NAK of address, peer wants %d.%d",
			    f->idb->hw_namestring, cinet, cinode);
		go->neg_address = 0; /* Don't try again */
		);

    NAKCIATRPROT(CI_AT_ROUTING_PROTOCOL, go->neg_routeprot, 
		if (ppp_negotiation_debug)
		     buginf("\nppp: %s ATCP NAK of routing protocol, peer wants %d",
			    f->idb->hw_namestring, cishort);
		go->neg_routeprot = 0; /* Don't try again */
		);

    NAKCIATSUPR(CI_AT_SUPRESSBROADCAST, go->neg_supbroad,
		if (ppp_negotiation_debug) {
		     buginf("\nppp: %s ATCP NAK of suppression, peer wants",
			    f->idb->hw_namestring);
		     for (count =0; count < cishort; count++) {
			 buginf(" %d", ciarray[count]);
		     }
		}
		go->neg_supbroad = 0; /* Don't try again */
		);

    NAKCIATCOMP(CI_AT_COMPRESSION, go->neg_compprot, 
		if (ppp_negotiation_debug)
		     buginf("\nppp: %s ATCP NAK of compression, peer wants %d",
			    f->idb->hw_namestring, cishort);
		go->neg_compprot = 0; /* Don't try again */
		);

    NAKCIATSERV(CI_AT_SERVERINFO, go->neg_server,
		if (ppp_negotiation_debug) {
		     buginf("\nppp: %s ATCP NAK of server info, peer wants %x, %x: ",
			    f->idb->hw_namestring, ciclass, ciimplementation);
		     for (count =0; count < cishort; count++) {
			 buginf(" %d", ciarray[count]);
		     }
		}
		go->neg_server = 0; /* Don't try again */
		);


    NAKCIATZONE(CI_AT_ZONEINFO, go->neg_zone,
		if (ppp_negotiation_debug) {
		     buginf("\nppp: %s ATCP NAK of zone, peer wants",
			    f->idb->hw_namestring);
		     for (count =0; count < cishort; count++) {
			 buginf(" %d", ciarray[count]);
		     }
		}
		go->neg_zone = 0; /* Don't try again */
		);

    NAKCIATDEFROUT(CI_AT_DEFAULT_ROUTER, go->neg_defrouter,
		if (ppp_negotiation_debug)
		     buginf("\nppp: %s ATCP NAK of default router, peer wants %d.%d",
			    f->idb->hw_namestring, cinet, cinode);
		go->neg_defrouter = 0; /* Don't try again */
		);

    /*
     * This is where we check for additional options specified.
     */
    while (len >= 2) {

	/*
	 * Well, by default we don't negotiate our AT address but some
	 * implementations are lame enough to ask us for it.  So enable
	 * the negotiation of our address and it will go out in the next
	 * ATCP Config Request.
	 */
	NAKCIATADDR(CI_AT_ADDRESS, !go->neg_address,
		    if (ppp_negotiation_debug)
			buginf("\nppp: %s ATCP NAK for address",
			    f->idb->hw_namestring);
		go->neg_address = 1; /* Ok, send them something */
		)

	else {
	    int optlen;

	    if (ppp_negotiation_debug || ppp_error_debug)
		buginf("\nATCP %s: ignoring NAK for type = %d len = %d",
		       f->idb->hw_namestring, p[0], p[1]);
	    optlen = p[1];
	    if ((optlen > len) || (optlen < 2))
		break;	/* we should end up going thru the buginf below */
	    p += optlen;
	    len -= optlen;
	}
    }

    if (len) {
	if (ppp_negotiation_debug || ppp_error_debug)
	    buginf("\nppp: atcp_nakci: %s: received bad configuration NAK!",
		    f->idb->hw_namestring);
    }
}


/*
 * atcp_rejci - Reject some of our CIs.
 */
static void atcp_rejci (fsm *f, uchar *p, int len)
{
    uchar cichar;
    ushort cishort;
    atcp_options *go;

    go = (atcp_options *)f->gotoptions;

    /*
     * Any Rejected CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */

    REJCIATADDR(CI_AT_ADDRESS, go->neg_address,
		go->default_router_net, go->default_router_node);
    REJCIATRPROT(CI_AT_ROUTING_PROTOCOL, go->neg_routeprot, 
		go->routing_protocol);
    REJCIATSUPR(CI_AT_SUPRESSBROADCAST, go->neg_supbroad,
		go->suppressed_types, go->num_suppressed_types);
    REJCIATCOMP(CI_AT_COMPRESSION, go->neg_compprot, 
		go->compression_protocol);
    REJCIATSERV(CI_AT_SERVERINFO, go->neg_server,
		go->server_name, go->server_name_len,
		go->server_class, go->implementation_id);
    REJCIATZONE(CI_AT_ZONEINFO, go->neg_zone,
		go->zone_name, go->zone_name_len);
    REJCIATDEFROUT(CI_AT_DEFAULT_ROUTER, go->neg_defrouter,
		go->default_router_net, go->default_router_node);

    /*
     * If there are any remaining CIs, then this packet is bad.
     */

    if (len == 0)
	return;
bad:
    if (ppp_error_debug)
	buginf("\nppp: atcp_rejci: received bad Reject!");
}


/*
 * atcp_reqci - Check the peer's requested CIs and send appropriate response.
 *
 * Returns: CONFACK, CONFNAK or CONFREJ and input packet modified
 * appropriately.
 */
static uchar atcp_reqci (fsm *f, uchar *inp, short *len)
{
    atcp_options *wo;
    atcp_options *ho;
    atcp_options *ao;
    atcp_options *go;
    uchar *cip;		/* Pointer to Current CI */
    ushort cilen, citype;	/* Parsed len, type */
    int rc = CONFACK;		/* Final packet return code */
    int orc;			/* Individual option return code */
    uchar *p = inp;		/* Pointer to next char to parse */
    uchar *ucp = inp;		/* Pointer to current output char */
    int l = *len;		/* Length left */
    int count;
    uchar cichar;
    ushort cishort, cishort1;

    wo = (atcp_options *)f->wantoptions;
    ao = (atcp_options *)f->allowoptions;
    ho = (atcp_options *)f->heroptions;
    go = (atcp_options *)f->gotoptions;

    /*
     * Reset all her options.
     */

    ho->neg_address = 0;        /* don't negotiate atalk address */
    ho->neg_routeprot = 0;      /* don't negotiate routing protocol */
    ho->neg_supbroad = 0;       /* don't negotiate broadcast suppression */
    ho->neg_compprot = 0;       /* don't negotiate compression*/
    ho->neg_server = 0;         /* don't negotiate server info */
    ho->neg_zone = 0;           /* don't negotiate zone info */
    ho->neg_defrouter = 0;      /* don't negotiate a default router */

    /*
     * Process all her options.
     */
    while (l) {
	orc = CONFACK;			/* Assume success */
	cip = p;			/* Remember begining of CI */
	if (l < 2 ||			/* Not enough data for CI header or */
	    p[1] < 2 ||			/*  CI length too small or */
	    p[1] > l) {			/*  CI length too big? */
	    if (ppp_error_debug)
		buginf("\nppp: atcp_reqci: bad CI length!");
	    orc = CONFREJ;		/* Reject bad CI */
	    cilen = l;			/* Reject till end of packet */
	    l = 0;			/* Don't loop again */
	    goto endswitch;
	}
	GETCHAR(citype, p);		/* Parse CI type */
	GETCHAR(cilen, p);		/* Parse CI length */
	l -= cilen;			/* Adjust remaining length */
	cilen -= 2;			/* Adjust cilen to just data */

	switch (citype) {		/* Check CI type */
	  case CI_AT_ADDRESS:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate AT address",
		     f->idb->hw_namestring);
	    if (!ao->neg_address) {
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }
	    GETCHAR(cichar, p);
	    if (cichar != 0) {
		INCPTR(cilen-1, p);   /* Skip rest of CI */
		orc = CONFREJ;	      /* Reject that puppy. */
		break;
	    }

	    PPP_GETSHORT(cishort, p);
	    GETCHAR(cichar, p);


	    if ((cishort != wo->her_address_net) || (cichar != wo->her_address_node)) {
		/* NAK with the address we want them to have */
		orc = CONFNAK;
		DECPTR(3, p);
		PPP_PUTSHORT(wo->her_address_net, p);
		PUTCHAR(wo->her_address_node, p);
		if (ppp_negotiation_debug)
		  buginf("\natcp %s: NAKing with our address",
			 f->idb->hw_namestring);
		break;
	    }
	    break;

	  case CI_AT_ROUTING_PROTOCOL:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate AT routing protocol",
		     f->idb->hw_namestring);
	    if (!ao->neg_routeprot) {
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }
	    break;

	  case CI_AT_SUPRESSBROADCAST:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate AT broadcast suppression",
		     f->idb->hw_namestring);
	    if (!ao->neg_supbroad) {
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }
	    break;
	  case CI_AT_COMPRESSION:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate AT compression protocol",
		     f->idb->hw_namestring);
	    if (!ao->neg_compprot) {
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }
	    break;
	  case CI_AT_RESERVED:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate AT RESERVED",
		     f->idb->hw_namestring);
	    if (TRUE) { /* always reject a RESERVED */
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }
	    break;
	  case CI_AT_SERVERINFO:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate AT server information",
		     f->idb->hw_namestring);
	    if (!ao->neg_server) {
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }
	    /* stuff our information in place of the requested and NAK */

	    PPP_GETSHORT(cishort, p);
	    PPP_GETLONG(cishort1,p);

	    if (cishort1 != wo->implementation_id) {
		orc = CONFNAK;
		DECPTR(7, p);  /*point to length byte */
		PUTCHAR(cilen+2, p);
		PPP_PUTSHORT(wo->server_class, p);
		PPP_PUTLONG(wo->implementation_id, p);
		for (count = 0; count < wo->server_name_len; count++) {
		    PUTCHAR(wo->server_name[count], p);
		}

		if (ppp_negotiation_debug)
		  buginf("\natcp %s: Sending our Server information in a NAK",
			    f->idb->hw_namestring);
		break;
	    }
	    break;
	  case CI_AT_ZONEINFO:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate AT zone information",
		     f->idb->hw_namestring);
	    if (!ao->neg_zone) {
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }
	    break;
	  case CI_AT_DEFAULT_ROUTER:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate AT default router",
		     f->idb->hw_namestring);
	    if (!ao->neg_defrouter) {
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }
	    break;
	  default:
	    if (ppp_error_debug)
	      buginf("\nppp %s: Rejecting unknown ATCP option %d",
		     f->idb->hw_namestring, citype);
	    INCPTR(cilen, p);
	    orc = CONFREJ;
	    break;
	}
	cilen += 2;			/* Adjust cilen whole CI */

endswitch:
    if (ppp_negotiation_debug)
	buginf(" (%s)",
		   orc == CONFACK ? "ACK" : (orc == CONFNAK ? "NAK" : "REJ"));
	if (orc == CONFACK &&		/* Good CI */
	    rc != CONFACK)		/*  but prior CI wasnt? */
	    continue;			/* Don't send ther one */

	if (orc == CONFNAK) {		/* Nak this CI? */
	    if (rc == CONFREJ)		/* Rejecting prior CI? */
		continue;		/* Don't send this one */
	    if (rc == CONFACK) {	/* Ack'd all prior CIs? */
		rc = CONFNAK;		/* Not anymore... */
		ucp = inp;		/* Backup */
	    }
	}
	if (orc == CONFREJ &&		/* Reject this CI */
	    rc != CONFREJ) {		/*  but no prior ones? */
	    rc = CONFREJ;
	    ucp = inp;			/* Backup */
	}
	if (ucp != cip)			/* Need to move CI? */
	    bcopy(cip, ucp, cilen);	/* Move it */
	INCPTR(cilen, ucp);		/* Update output pointer */
    }

    /*
     * XXX If we wanted to send additional NAKs (for unsent CIs), the
     * code would go here.  This must be done with care since it might
     * require a longer packet than we received.
     */

    *len = ucp - inp;			/* Compute output length */

    if (ppp_negotiation_debug)
	buginf("\nppp: atcp_reqci: returning %s.",
	      rc == CONFACK ? "CONFACK" :
	      rc == CONFNAK ? "CONFNAK" : "CONFREJ");
    return (rc);			/* Return final code */
}

/*
 * atcp_up - FSM has reached up state
 */

static void atcp_up (fsm *f)
{
    reg_invoke_media_protocol_up(f->idb, LINK_APPLETALK);
}

/*
 * atcp_closed - FSM has reached CLOSED state
 */

static void atcp_closed (fsm *f)
{
    reg_invoke_media_protocol_down(f->idb, LINK_APPLETALK);
}

static boolean ppp_atalk_protocheck (hwidbtype *hwidb,
				     hwidbtype *ignore,
				     int linktype)
{
    if (hwidb->ppp_bridge_atalk)
	return(FALSE);
    return(reg_invoke_proto_on_swidb(linktype, hwidb->firstsw));
}

/*
 * atcp_init_subsys
 * Initialization of ATCP subsystem
 */

static void atcp_init_subsys (subsystype *subsys)
{
    init_atalk_ppp();
    reg_invoke_ncp_register(LINK_APPLETALK, TYPE_PPP_ETHERTALKCP,
			    atcp_init, ppp_atalk_protocheck, "ATALKCP");
}

/*
 * The ATCP subsystem header
 */

#define ATCP_MAJVERSION 1
#define ATCP_MINVERSION 0
#define ATCP_EDITVERSION  1

SUBSYS_HEADER(atcp, ATCP_MAJVERSION, ATCP_MINVERSION, ATCP_EDITVERSION,
	      atcp_init_subsys, SUBSYS_CLASS_PROTOCOL,
	      "seq: atalk, ppp",
	      "req: atalk, ppp");
