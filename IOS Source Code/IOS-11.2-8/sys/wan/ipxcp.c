/* $Id: ipxcp.c,v 3.6.18.7 1996/09/12 13:32:05 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/ipxcp.c,v $
 *------------------------------------------------------------------
 * ipxcp.c - NCP routines for IPX over PPP.
 *
 * March 1994, William Miskovetz
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipxcp.c,v $
 * Revision 3.6.18.7  1996/09/12  13:32:05  fox
 * CSCdi68759:  AAA per-user routes are not removed when connection drops
 * Branch: California_branch
 * Separate parallel connection support for per-user routes from the
 * support for neighbor routes and dynamic dialer maps.  Transfer
 * ownership of the static route to the last idb to add it (applies only
 * to per-user routes).
 *
 * Revision 3.6.18.6  1996/08/28  13:21:47  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.18.5  1996/08/18  09:08:01  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.6.18.4  1996/06/24  09:27:24  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.6.18.3  1996/05/10  01:44:54  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.6.18.2  1996/04/29  09:45:35  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.6.18.1  1996/03/18  22:47:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.3  1996/03/13  02:12:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.2.2  1996/03/07  11:07:29  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  21:51:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/08  09:29:14  fox
 * CSCdi48417:  IPXCP Compression option can cause a misparsing of a
 * ConfReq
 * Move pointer to end of option if exiting early.
 *
 * Revision 3.5  1996/02/08  07:59:42  fox
 * CSCdi48135:  IPXCP Configuration Complete option is not supported
 * Enable support of IPXCP Configuration Complete option
 *
 * Revision 3.4  1996/01/22  07:45:49  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  18:02:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:58  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:49:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:35:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:18:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <stdlib.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "media_registry.h"
#include "dialer_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ppp.h"
#include "ppp_registry.h"
#include "ieee.h"
#include "../xns/novell.h"
#include "ipxcp.h"
#include "ppp_public.h"
#include "logger.h"
#include "../wan/msg_ppp.c"		/* Not a typo, see logger.h */
#include "dialer.h"
#include "aaa.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../xns/xns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/isis_ipx.h"
#include "../xns/ipx_registry.h"
#include "../xns/novfast.h"

static void ipxcp_resetci(fsm *);	/* Reset our Configuration Information */
static int ipxcp_cilen(fsm *);		/* Return length of our CI */
static void ipxcp_addci(fsm *, uchar *);	/* Add our CIs */
static int ipxcp_ackci(fsm *, uchar *, int);	/* Ack some CIs */
static void ipxcp_nakci(fsm *, uchar *, int);	/* Nak some CIs */
static void ipxcp_rejci(fsm *, uchar *, int);	/* Reject some CIs */
static uchar ipxcp_reqci(fsm *, uchar *, short *);	/* Check the requested CIs */
static void ipxcp_up(fsm *);		/* Link came up */
static void ipxcp_down(fsm *);		/* Link went down */
static boolean ipxcp_do_author(fsm *); /* tacacs+ authorization */

static fsm_callbacks ipxcp_callbacks = { /* IPCP callback routines */
    ipxcp_resetci,		/* Reset our Configuration Information */
    ipxcp_cilen,			/* Length of our Configuration Information */
    ipxcp_addci,			/* Add our Configuration Information */
    ipxcp_ackci,			/* ACK our Configuration Information */
    ipxcp_nakci,			/* NAK our Configuration Information */
    ipxcp_rejci,			/* Reject our Configuration Information */
    ipxcp_reqci,			/* Request peer's Configuration Information */
    ipxcp_up,			/* Called when fsm reaches OPEN state */
    NULL,			/* Called when fsm leaves OPEN state */
    ipxcp_down,			/* Called when fsm reaches CLOSED state */
    NULL,			/* Called when Protocol-Reject received */
    NULL,			/* Retransmission is necessary */
	NULL,			/* Reset Request */
	NULL,			/* Rest Acknowledge */
	NULL			/* Code Rejected */
};

/*
 * ipxcp_init - Initialize IPXCP.
 */
void ipxcp_init (fsm *ipxcp_fsm)
{
    ipxcp_options *wo;
    ipxcp_options *ao;

    if (!ipxcp_fsm->wantoptions)
    	ipxcp_fsm->wantoptions = malloc(sizeof(ipxcp_options));
    else
	memset(ipxcp_fsm->wantoptions, 0, sizeof(ipxcp_options));
    if (!ipxcp_fsm->wantoptions)
	return;
    if (!ipxcp_fsm->allowoptions)
    	ipxcp_fsm->allowoptions = malloc(sizeof(ipxcp_options));
    else
	memset(ipxcp_fsm->allowoptions, 0, sizeof(ipxcp_options));
    if (!ipxcp_fsm->allowoptions)
	return;
    if (!ipxcp_fsm->heroptions)
    	ipxcp_fsm->heroptions = malloc(sizeof(ipxcp_options));
    else
	memset(ipxcp_fsm->heroptions, 0, sizeof(ipxcp_options));
    if (!ipxcp_fsm->heroptions)
	return;
    ipxcp_fsm->ouroptions = NULL;

    wo = ipxcp_fsm->wantoptions;
    ao = ipxcp_fsm->allowoptions;

    ipxcp_fsm->retransmits = 0;
    TIMER_STOP(ipxcp_fsm->timeouttime);
    ipxcp_fsm->failures = 0;
    ipxcp_fsm->callbacks = &ipxcp_callbacks;
    ipxcp_fsm->flags |= AOPENDING;           /* Active Open */

}


/*
 * ipxcp_resetci - Reset our CI.
 */

static void ipxcp_resetci (fsm *f)
{
    ipxcp_options *wo;
    ipxcp_options *ao;
    ipxcp_options *ho;
    hwidbtype *idb;
    hwidbtype *tempidb;
    nidbtype *nidb;
    addrtype addr;
    boolean compression_enabled;
    uchar compression_type, compression_options, compression_slots;


    wo = f->wantoptions;
    ao = f->allowoptions;
    ho = f->heroptions;

    memset(&addr, 0, sizeof(addrtype));
    idb = f->idb;
/*    reg_invoke_proto_address(LINK_NOVELL, idb->firstsw, &addr); */
    wo->neg_net = 1;		/* Negotiate network number */
    wo->address = addr;
    /* 
     * kludge alert.  This has to go.  We should be using the reg_invoke
     * above, but currently Novell has this requirement that you pass in
     * a pak that Novell has given you to get the network number.  Ugh.
     * This is temporary until I win a battle with the Novell folks...
     */
    tempidb = get_netidb(idb);

    if (tempidb->firstsw->owner_nidb)
        wo->address.nov_addr.net = tempidb->firstsw->owner_nidb->novellnet;
      else 
    if (tempidb->firstsw->primary_nidb)
	wo->address.nov_addr.net = tempidb->firstsw->primary_nidb->novellnet;
      else
        wo->address.nov_addr.net = 0;

    /*
     * If no network number is assigned, don't attempt to negotiate or
     * we will ask the other side to assign one to us and we can't deal
     * with that.
     */

    if (wo->address.nov_addr.net == 0)
        wo->neg_net = 0;

    wo->neg_node = 1;		/* negotiate node number */
    if (tempidb->firstsw->owner_nidb)
    	ieee_copy(tempidb->firstsw->owner_nidb->idb->novell_node, 
			wo->address.nov_addr.host);
      else
        ieee_copy(tempidb->firstsw->novell_node, wo->address.nov_addr.host);

    wo->neg_compress = 0;	/* Don't negotiate compression */
    ao->neg_compress = 0;	/* Don't negotiate compression */

    if ((reg_invoke_ipx_compression_exists() == TRUE) &&
	tempidb->firstsw) {

	nidb = IPX_PRIMARY_NIDB(tempidb->firstsw);
	if (nidb && nidb->compression_enabled) {
	    reg_invoke_ipx_compression_options_provide(nidb,
		    &compression_enabled, &compression_type,
		    &compression_options, &compression_slots);

	    if (compression_enabled &&
		(IPX_COMPRESSION_TYPE_TELEBIT == compression_type)) {
		wo->neg_compress = ao->neg_compress = 1;
		wo->comp_slots = ao->comp_slots = compression_slots;
		wo->comp_flags = ao->comp_flags = compression_options;
		reg_invoke_ipx_compression_reset(nidb);
	    }
	}
    }

    wo->neg_rprot = 0;		/* Don't negotiate routing protocol */
    wo->neg_rname = 0;		/* don't negotiate router name */
    wo->neg_conf_comp = 0;		/* no config complete */
    ao->neg_net = 1;
    ao->neg_node = 1;
    ao->neg_rprot = 0;		/* Don't negotiate routing protocol */
    ao->neg_rname = 0;		/* don't negotiate router name */
    /*
     * Allow peer to send Configuration Complete option.  It is advisory
     * only.  Accepting it means nothing.  Rejecting it has been known
     * to not be acceptable to one client (see DDTS that made this change).
     */
    ao->neg_conf_comp = 1;		/* accept config complete */
    f->gotoptions = f->wantoptions;
}


/*
 * ipxcp_cilen - Return length of our CI.
 */
static int ipxcp_cilen (fsm *f)
{
    ipxcp_options *go;

    go = f->gotoptions;

    return (LENCINET(go->neg_net) +
	    LENCINODE(go->neg_node) +
 	    LENCICOMPRESS(go->neg_compress) +
	    LENCIRPROT(go->neg_rprot) +
	    LENCINAME(go->neg_rname, strlen(go->router_name)) +
	    LENCICONFCOMP(go->neg_conf_comp));
}


/*
 * ipxcp_addci - Add our desired CIs to a packet.
 */
static void ipxcp_addci (fsm *f, uchar *ucp)
{
    ipxcp_options *go;

    go = f->gotoptions;

    ADDCINET(CI_IPX_NET, go->neg_net, go->address.nov_addr.net);
    ADDCINODE(CI_IPX_NODE, go->neg_node, go->address.nov_addr.host);
    ADDCICOMPRESS(CI_IPX_COMPRESS, go->neg_compress, go->comp_slots,
		  go->comp_flags);
}


/*
 * ipxcp_ackci - Ack our CIs.
 *
 * Returns:
 *	0 - Ack was bad.
 *	1 - Ack was good.
 *
 * This routine ensures that a received ACK message is valid. An ACK
 * message must contain all of the options that we specified, in the order
 * that we specified.
 */

static int ipxcp_ackci (fsm *f, uchar *p, int len)
{
    ushort cilen, citype;
    ulong cilong;
    ipxcp_options *go;

    go = f->gotoptions;

    /*
     * CIs must be in exactly the same order that we sent...
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
    ACKCINET(CI_IPX_NET, go->neg_net, go->address.nov_addr.net);
    ACKCINODE(CI_IPX_NODE, go->neg_node, &go->address.nov_addr.host);
    ACKCICOMPRESS(CI_IPX_COMPRESS, go->neg_compress, go->comp_slots,
		  go->comp_flags);

    /*
     * If there are any remaining CIs, then this packet is bad.
     */
    if (len != 0)
	goto bad;
    return (1);
bad:
    if (ppp_error_debug)
	buginf("\nppp: ipxcp_ackci: received bad Ack!");
    return (0);
}


/*
 * ipxcp_nakci - NAK some of our CIs.
 *
 * Returns:
 *	0 - Nak was bad.
 *	1 - Nak was good.
 *
 * This routine handles a received NAK message. The NAK message will
 * list all of the options that we specified, the peer supports, but
 * the peer would prefer a different value.
 */

static void ipxcp_nakci (fsm *f, uchar *p, int len)
{
    ulong ciaddr1;
    uchar cinode[IEEEBYTES];
    ipxcp_options *go, *ao, *ho;


    ao = f->allowoptions;
    ho = f->heroptions;
    go = f->gotoptions;

    /*
     * Any Nak'd CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */

    NAKCINET(CI_IPX_NET, go->neg_net,
	       if (!go->address.nov_addr.net)	/* Didn't know our address? */
		   go->address.nov_addr.net = ciaddr1;
	       else if (go->address.nov_addr.net != ciaddr1) {

		   /*
		    * At this point, we could check to see if this is an
		    * acceptable local address.
		    */
		   if (go->address.nov_addr.net > ciaddr1)
		       if (ppp_error_debug)
		           buginf("\n%s: IPXCP error.  NAK with lower net number",
			           f->idb->hw_namestring);

		   if (ppp_negotiation_debug)
		     buginf("\nppp: %s IPXCP NAK of net number, peer wants %#x",
			    f->idb->hw_namestring,
			    ciaddr1);
          	   errmsg(&msgsym(IPXNET, PPP), go->address.nov_addr.net,
		         ciaddr1);
		   go->neg_net = 0; /* Don't try again */
	       }	       
	       );

    NAKCINODE(CI_IPX_NODE, go->neg_node,
		   if (ppp_negotiation_debug)
		     buginf("\nppp: %s IPXCP NAK of node number, peer wants %e",
			    f->idb->hw_namestring,
			    cinode);
		   go->neg_node = 0; /* Don't try again */
	       );

    NAKCICOMPRESS(CI_IPX_COMPRESS, go->neg_compress,
		  if ((flags != go->comp_flags) ||
		      (slots > go->comp_slots)) {
		      go->neg_compress = 0;
		      ao->neg_compress = ho->neg_compress = 0;
		  } else {
		      go->comp_slots = slots;
		  }
		  );

    /*
     * If there are any remaining CIs, then this packet is bad.
     * At this point, we could check for any appended options.
     */
    if (len == 0)
	return;

    if (ppp_error_debug)
	buginf("\nppp: ipxcp_nakci: ignoring unrequested options!");
}


/*
 * ipxcp_rejci - Reject some of our CIs.
 */
static void ipxcp_rejci (fsm *f, uchar *p, int len)
{
    ulong cilong;
    ipxcp_options *go, *ho, *ao;

    go = f->gotoptions;
    ho = f->heroptions;
    ao = f->allowoptions;

    /*
     * Any Rejected CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */

    REJCINET(CI_IPX_NET, go->neg_net, go->address.nov_addr.net);
    REJCINODE(CI_IPX_NODE, go->neg_node, go->address.nov_addr.host);
    REJCICOMPRESS(CI_IPX_COMPRESS, go->neg_compress, go->comp_slots,
		  go->comp_flags);

    /*
     * If there are any remaining CIs, then this packet is bad.
     */

    if (len == 0)
	return;
bad:
    if (ppp_error_debug)
	buginf("\nppp: ipxcp_rejci: received bad Reject!");
}


/*
 * ipxcp_reqci - Check the peer's requested CIs and send appropriate response.
 *
 * Returns: CONFACK, CONFNAK or CONFREJ and input packet modified
 * appropriately.
 *
 * Returns TERMREQ if AAA authorization failed.
 */
static uchar ipxcp_reqci (fsm *f, uchar *inp, short *len)
{
    ipxcp_options *wo;
    ipxcp_options *ho;
    ipxcp_options *ao;
    ipxcp_options *go;
    uchar *cip;		/* Pointer to Current CI */
    ushort cilen, citype;	/* Parsed len, type */
    ushort cishort;		/* Parsed short value */
    uchar cichar;		/* Parsed char value */
    uchar cinode[IEEEBYTES];
    ulong tl, ciaddr1;	/* Parsed address values */
    int rc = CONFACK;		/* Final packet return code */
    int orc;			/* Individual option return code */
    uchar *p = inp;		/* Pointer to next char to parse */
    uchar *ucp = inp;		/* Pointer to current output char */
    int l = *len;		/* Length left */

    wo = f->wantoptions;
    ao = f->allowoptions;
    ho = f->heroptions;
    go = f->gotoptions;

    /*
     * Reset all her options.
     */

    ho->neg_net = 0;
    ho->neg_node = 0;
    ho->neg_compress = 0;	/* Don't negotiate compression */
    ho->neg_rprot = 0;		/* Don't negotiate routing protocol */
    ho->neg_rname = 0;		/* don't negotiate router name */
    ho->neg_conf_comp = 0;	/* no config complete */
    ho->comp_slots = 0;         /* Compression protocol details */
    ho->comp_flags = 0;

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
		buginf("\nppp: ipxcp_reqci: bad CI length!");
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
	  case CI_IPX_NET:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate IPX net number",
		     f->idb->hw_namestring);
	    if (!ao->neg_net ||
		cilen != sizeof(long)) { /* Check CI length */
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		break;
	    }

	    /*
	     * Handle the net number
	     */

	    PPP_GETLONG(tl, p);		/* Parse source address (her) */
	    ciaddr1 = h2nl(tl);
	    if (ppp_negotiation_debug)
		buginf(": their net %#x", ciaddr1);

	    /*
	     * If the remote does not know its net, send ours
	     */

	    if (!ciaddr1) {
		orc = CONFNAK;
		ciaddr1 = go->address.nov_addr.net;
	    }

	    /*
	     * If the remote address is different, NAK with the correct
	     * address.
	     */

	    if (go->address.nov_addr.net && ciaddr1 < go->address.nov_addr.net) {
		orc = CONFNAK;	/* Send a NAK */
		/*
		 * If the two differ, send the highest of the two 
		 */
		ciaddr1 = go->address.nov_addr.net; /* Send our net */
	    }


	    /*
	     * Their net number is greater than ours.  Reject it and issue
	     * a warning message.
	     */
	    if (go->address.nov_addr.net && ciaddr1 > go->address.nov_addr.net) {
		orc = CONFREJ;	/* Send a REJECT */
          	errmsg(&msgsym(IPXNET, PPP), go->address.nov_addr.net, ciaddr1);
	    }

	    /*
	     * If we are sending a NAK, update the parameters
	     */

	    if (orc == CONFNAK) {
		DECPTR(sizeof(long), p);
		PPP_PUTLONG(ciaddr1, p); /* Send net number */
		if (ppp_negotiation_debug)
		  buginf("\nppp %s: Sending IPXCP net number NAK, "
			 "net = %#x",
			 f->idb->hw_namestring, ciaddr1);
		break;
	    }

	    /*
	     * Remember the addresses from this message
	     */

	    ho->neg_net = 1;
	    ho->address.nov_addr.net = ciaddr1;
	    break;

	  case CI_IPX_NODE:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate IPX node number",
		     f->idb->hw_namestring);
	    if (!ao->neg_node ||
		cilen != IEEEBYTES) { /* Check CI length */
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		break;
	    }

	    ieee_copy(p, cinode);
	    INCPTR(IEEEBYTES, p);
	    if (ppp_negotiation_debug)
	        buginf(": their node %e", cinode);

	    /*
	     * Pretty much we accept anything here.  Correctly we should
	     * nak them if they are identical to us, but will that ever 
	     * happen?
	     */
	    
	    ho->neg_node = 1;
	    ieee_copy(cinode, ho->address.nov_addr.host);
	    break;

	  case CI_IPX_COMPRESS:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate IPX header compression",
		     f->idb->hw_namestring);
	    if (!ao->neg_compress) {
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		break;
	    }

	    PPP_GETSHORT(cishort, p);   /* Pick Compress Proto type */

	    /*
	     * Compresstype must be IPXCP_COMP_CIPX (aka Telebit)
	     */
	    if (cishort != IPXCP_COMP_CIPX) {
		DECPTR(sizeof(short), p);
		orc = CONFNAK;
		PPP_PUTSHORT(IPXCP_COMP_CIPX, p);
		/*
		 * Now we need to skip over the last two characters in the
		 * field since we are exiting early.
		 */
		INCPTR(2*sizeof(char), p);
		break;
	    }

	    GETCHAR(cichar, p);	/* Get the number of slots */
	    if (cichar > ao->comp_slots) { /* More slots than we can handle? */
		DECPTR(sizeof(char), p); /* Point back to slots */
		PUTCHAR(ao->comp_slots, p); /* Set our slot maximum */
		orc = CONFNAK;	/* Send a NAK */
		if (ppp_negotiation_debug)
		  buginf(" (negotiated from %d to %d slots)",
			 cichar, ao->comp_slots);
		/*
		 * Now we need to skip over the last character in the
		 * field since we are exiting early.
		 */
		INCPTR(sizeof(char), p);
		break;
	    }
	    ho->comp_slots = cichar;

	    GETCHAR(cichar, p);	/* Get the compression flags */
	    if (cichar != ao->comp_flags) { /* Wrong flags ? */
		DECPTR(sizeof(char), p); /* Point back to slots */
		PUTCHAR(ao->comp_flags, p); /* Put wanted flags */
		orc = CONFNAK;	/* Send a NAK */
		if (ppp_negotiation_debug)
		    buginf(" (Nak'ing requested compression flags %x to %x)",
			   cichar, ao->comp_flags);
		break;
	    }
	    ho->neg_compress = 1;
	    break;

	  case CI_IPX_ROUTING:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate IPX routing protocol",
		     f->idb->hw_namestring);
	    if (!ao->neg_rprot) {
		orc = CONFREJ;		/* Reject CI */
	    }
	    INCPTR(cilen, p);		/* Skip rest of CI */
	    break;

	  case CI_IPX_NAME:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Negotiate IPX router name",
		     f->idb->hw_namestring);
	    if (!ao->neg_rname) {
		orc = CONFREJ;		/* Reject CI */
	    }
	    INCPTR(cilen, p);		/* Skip rest of CI */
	    break;

	  case CI_IPX_CONFCOMP:
	    if (ppp_negotiation_debug)
	      buginf("\nppp %s: Received IPX configuration complete",
		     f->idb->hw_namestring);
	    if (!ao->neg_conf_comp || cilen) {
		orc = CONFREJ;		/* Reject CI */
	    }
	    INCPTR(cilen, p);		/* Skip rest of CI */
	    break;

	  default:
	    if (ppp_error_debug || ppp_negotiation_debug)
	      buginf("\nppp %s: Received unknown IPXCP option %d",
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

    if (!old_access_control) {
	if (!ipxcp_do_author(f)) {
	    rc = TERMREQ;
	}
    }

    if (ppp_negotiation_debug) {
	char *rc_str;

	switch (rc) {
	case CONFACK:
	    rc_str = "CONFAK";
	    break;
	case CONFNAK:
	    rc_str = "CONFNAK";
	    break;
	case TERMREQ: 
	    rc_str = "TERMREQ";
	    break;
	case CONFREJ: 
	    rc_str = "CONFREJ";
	    break;
	default: 
	    rc_str = "UNKNOWN";
	    break;
	}

	buginf("\nppp: ipxcp_reqci: returning %s.", rc_str);
    }
    return (rc);			/* Return final code */
}

/*
 * ipxcp_up - FSM has reached up state
 */

static void ipxcp_up (fsm *f)
{
    hwidbtype *idb;
    idbtype *swidb;
    nidbtype *nidb;
    ipxcp_options *go;
    ipxcp_options *wo;
    ipxcp_options *ho;
    addrtype addr;


    idb = f->idb;		/* Get the IDB */

    go = f->gotoptions;
    wo = f->wantoptions;
    ho = f->heroptions;

    /*  
     *    If this is a client hung off of an owner nidb, then
     *	we want to adopt the remote node's address.  This is
     *	because the owner uses the client idb addresses to 
     *	determine which interface to send packets out 
     */ 
    swidb = firstsw_or_null(idb);
    if ((swidb) && (swidb->owner_nidb))
	ieee_copy(ho->address.nov_addr.host, swidb->novell_node);

    /*
     * Attempt to create a dynamic dialer map
     * if we got a node number from the remote.
     * Note: By rights we should check if we got a net and a node
     * number, but it appears some clients don't negotiate the
     * net number.  They must just assume ours if we send it.
     * So if the net number hasn't been provided, use ours.
     */

    if (ho->neg_node) {
        bzero(&addr, sizeof(addrtype));
        addr.type = ADDR_NOVELL;
        addr.length = ADDRLEN_NOVELL;
        ieee_copy(ho->address.nov_addr.host, addr.nov_addr.host);
	if (ho->neg_net)
            addr.nov_addr.net = ho->address.nov_addr.net;
	else
	    addr.nov_addr.net = go->address.nov_addr.net;
        reg_invoke_dialer_create_dynamic_map(LINK_NOVELL, idb, &addr);
    }

    /*
     * we need to make sure that the IPX negotiations, correctly
     * negotiated (or did not negotiate) compression for both
     * directions of the link, at the same time.
     * Note: CIPX will not work in only one direction of the link.
     */
    if (ho->neg_compress ^ go->neg_compress) {
	if (f->callbacks->down)
	    (*f->callbacks->down)(f); /* inform upper layers */
	f->state = CLOSED;
	if (f->callbacks->closed)
	    (*f->callbacks->closed)(f);	/* exit/restart/etc. */
	return;

    } else if (swidb && ho->neg_compress && go->neg_compress) {
	nidb = IPX_PRIMARY_NIDB(swidb);
	if (nidb && nidb->compression_enabled) {

	    reg_invoke_ipx_compression_options_receive(nidb,
		    ho->neg_compress, IPX_COMPRESSION_TYPE_TELEBIT,
		    IPX_COMPRESSION_SUPPORTED_OPTIONS,
		    min(ho->comp_slots, go->comp_slots));
	}
    }
    reg_invoke_media_protocol_up(idb, LINK_NOVELL);
}

/*
 * ipxcp_remove_per_user_access_lists
 *
 * Remove any per-user authorisaton applied to the specified interface
 */
static void ipxcp_remove_per_user_access_lists (hwidbtype *idb)
{
    idbtype *swidb;

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	swidb->novell_accesslist = swidb->novell_accesslist_def;
    }
}

/*
 * ipxcp_down - FSM has reached closed state
 */
static void ipxcp_down (fsm *f)
{
    hwidbtype *idb;
    ipxcp_options *ho;
    ipxcp_options *go;
    addrtype addr;

    idb = f->idb;			/* Get the IDB */

    if (!old_access_control) {
	/* Undo any per user authorization settings */
	ipxcp_remove_per_user_access_lists(idb);

	/* invalidate the fs cache */
	reg_invoke_ipx_novcache_increment_version(NULL,
						  NOVCACHE_NULL_REASON, 
						  0, NOV_NETMASK_ALL);
    }

    ho = f->heroptions;
    go = f->gotoptions;
    /*
     * deleted any dynamic dialer map in case one was created.
     */
    if (ho->neg_node) {
        bzero(&addr, sizeof(addrtype));
        addr.type = ADDR_NOVELL;
        addr.length = ADDRLEN_NOVELL;
        ieee_copy(ho->address.nov_addr.host, addr.nov_addr.host);
	if (ho->neg_net)
            addr.nov_addr.net = ho->address.nov_addr.net;
	else
	    addr.nov_addr.net = go->address.nov_addr.net;
        reg_invoke_dialer_delete_dynamic_map(LINK_NOVELL, idb, &addr);
    }
}

/* Process AV pairs cached by NCP startup authorization */

static boolean
ipxcp_do_author(fsm* f)
{
    int i;
    char *attr, *val;
    enum AV_TYPE type;
    int list;
    idbtype *swidb;
    boolean ret = TRUE;
    char **args;
    hwidbtype *idb;
    hwidbtype *netidb;

    idb = f->idb;
    f->ncp_did_author = TRUE;
    args = f->cached_author;

    if (!aaa_author_needed(AAA_AUTHOR_TYPE_NET, 0, f->idb,
			   f->idb->idb_tty)) {
	/* Authorization is not required. */
	return(TRUE);
    }
    
    /* NCP startup authorization should have populated the cache for us */
    if (!args) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/IPXCP: %s: denied: cache is empty",
		   idb->hw_namestring);
	}
	return(FALSE);
    }

    for (i = 0; ret && args[i]; i++) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/IPXCP: %s: Processing AV %s", 
		   idb->hw_namestring, args[i]);
	}
	if (aaa_split_AV(args[i], &attr, &val, &type)) {
	    if (!strcmp(attr, AAA_ATTR_service)) {
		if (strcmp(val, AAA_VAL_SVC_ppp)) {
		    if (type == AAA_AV_MAND)
			ret = FALSE;
		    else {
			if (aaa_author_debug) {
			    buginf("\nAAA/AUTHOR/IPXCP: "
				   "tried to change \"service\".  Ignoring.");
			}
		    }
		}
		continue;
	    }
	    if (!strcmp(attr, AAA_ATTR_protocol)) {
		if (strcmp(val, AAA_VAL_PROT_ipx)) {
		    if (type == AAA_AV_MAND)
			ret = FALSE;
		    else {
			if (aaa_author_debug) {
			    buginf("\nAAA/AUTHOR/IPXCP: "
				   "tried to change \"protocol\".  Ignoring.");
			}
		    }
		}
		continue;
	    }

	    if (!strcmp(attr, AAA_ATTR_outacl)) {
		list = atoi(val);

		if ((list < MINNOVACC) || (list > MAXENOVACC)) {
		    if (aaa_author_debug)
			buginf("\nAAA/AUTHOR/IPXCP: "
			       "outacl access list value %d out of range %d-%d",
			       list, MINNOVACC, MAXENOVACC);

		    if (type == AAA_AV_MAND) {
			ret = FALSE;
		    }
		    continue;
		}
		
		netidb = reg_invoke_dialer_pool_acl_idb(idb);
		if (aaa_author_debug) {
		    buginf("\nAAA/AUTHOR/IPXCP: %s: install outacl=%d on %s",
			idb->hw_namestring, list, netidb->hw_namestring);
		}
		FOR_ALL_SWIDBS_ON_HW(netidb, swidb) {
		    swidb->novell_accesslist = list;
		}

		reg_invoke_ipx_novcache_increment_version(NULL,
							  NOVCACHE_NULL_REASON, 
							  0, NOV_NETMASK_ALL);
		continue;
	    }

	    /* If unknown and mandatory, then bail */
	    if (type == AAA_AV_MAND) {
		if (aaa_author_debug)
		    buginf("\nAAA/AUTHOR/IPXCP: Unknown mandatory AV %s",
			   args[i]);
		ret = FALSE;
	    }
	}
    }

    if (aaa_author_debug) {
	buginf("\nAAA/AUTHOR/IPXCP: %s: authorization %s", 
	       idb->hw_namestring,
	       ret ? "succeeded" : "denied");
    }

    return(ret);
}

static boolean ppp_ipx_protocheck (hwidbtype *hwidb,
				   hwidbtype *ignore,
				   int linktype)
{
    /* Don't open NCP for ipx if we want to psuedo-bridge it */
    if (hwidb->ppp_bridge_ipx)
	return(FALSE);
    return(reg_invoke_proto_on_swidb(linktype, hwidb->firstsw));
}

/*
 * ipxcp_init_subsys
 * Initialization of IPXCP subsystem
 */

static void ipxcp_init_subsys (subsystype *subsys)
{
    reg_add_author_remove_per_user(ipxcp_remove_per_user_access_lists,
				    "ipxcp_remove_per_user_access_lists");
    reg_invoke_ncp_register(LINK_NOVELL, TYPE_PPP_IPXCP,
			    ipxcp_init, ppp_ipx_protocheck, "IPXCP");
}

/*
 * The IPXCP subsystem header
 */

#define IPXCP_MAJVERSION 1
#define IPXCP_MINVERSION 0
#define IPXCP_EDITVERSION  1

SUBSYS_HEADER(ipxcp, IPXCP_MAJVERSION, IPXCP_MINVERSION, IPXCP_EDITVERSION,
	      ipxcp_init_subsys, SUBSYS_CLASS_PROTOCOL,
	      "seq: ppp",
	      "req: ipx, ppp");
