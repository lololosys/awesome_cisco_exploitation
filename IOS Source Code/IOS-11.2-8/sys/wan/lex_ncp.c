/* $Id: lex_ncp.c,v 3.3.62.8 1996/09/12 13:32:08 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/lex_ncp.c,v $
 *------------------------------------------------------------------
 * lex_ncp.c - LEX Control Protocol.
 *
 * 01-March-1994, David Coli
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lex_ncp.c,v $
 * Revision 3.3.62.8  1996/09/12  13:32:08  fox
 * CSCdi68759:  AAA per-user routes are not removed when connection drops
 * Branch: California_branch
 * Separate parallel connection support for per-user routes from the
 * support for neighbor routes and dynamic dialer maps.  Transfer
 * ownership of the static route to the last idb to add it (applies only
 * to per-user routes).
 *
 * Revision 3.3.62.7  1996/08/28  13:21:55  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.6  1996/08/18  09:08:13  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.3.62.5  1996/06/24  09:27:32  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.3.62.4  1996/05/11  06:10:52  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Attach lex_ncp subsystem to lex subsystem as lex requires PPP.
 * Add registry calls to eliminate PPP's dependancy on CCP/Compress.
 *
 * Revision 3.3.62.3  1996/05/10  01:45:06  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.3.62.2  1996/04/29  09:45:40  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.3.62.1  1996/03/18  22:48:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  11:07:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  21:51:57  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:03:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:49:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:18:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "logger.h"
#include "../wan/msg_ppp.c"		/* Not a typo, see logger.h */
#include "config.h"
#include "ppp.h"
#include "ncp.h"
#include "lex_ncp.h"
#include "ppp_public.h"
#include "ppp_registry.h"
#include "ieee.h"
#include "../tbridge/tbridge_public.h"	/* Only needed for ISMULTICAST which */
					/* doesn't belong there */

/*
 * lex_resetci - Reset our CI.
 */

static void lex_resetci (fsm *f)
{
    lex_options *wo;
    lex_options *ao;

    wo = f->wantoptions;
    ao = f->allowoptions;

    /*
     * According to the PPP Bridging draft, this option is advisory only.
     * This option MUST NOT be included in a Configure-NAK, thus always
     * allow it.
     */
    wo->neg_mac_support = FALSE;
    ao->neg_mac_support = TRUE;
    wo->mac_support_count = 0;

    wo->neg_tinygram_comp = FALSE;
    wo->tinygram_state = CI_TINYGRAM_DISABLED;
    ao->neg_tinygram_comp = TRUE;

    wo->neg_mac_address = FALSE;
    ao->neg_mac_address = TRUE;

    wo->neg_spanning_tree = FALSE;
    ao->neg_spanning_tree = TRUE;
    wo->span_proto_count = 0;

    wo->neg_mac_relay = FALSE;
    ao->neg_mac_relay = TRUE;

    bcopy(f->wantoptions, f->gotoptions, sizeof(lex_options));

}


/*
 * lex_cilen - Return length of our CI.
 */
static int lex_cilen (fsm *f)
{
    lex_options *go;
    int cilen;

    go = f->gotoptions;
    cilen = 0;

    if (go->neg_mac_support)
        cilen += ((go->mac_support_count) * LENCI_MAC_SUPPORT);
    if (go->neg_tinygram_comp)
        cilen += LENCI_TINYGRAM;
    if (go->neg_mac_address)
        cilen += LENCI_MAC_ADDRESS;
    if (go->neg_spanning_tree)
        cilen += PPP_TYPE_LEN_SIZE + go->span_proto_count;
    if (go->neg_mac_relay)
        cilen += LENCI_MAC_RELAY;

    if (ppp_negotiation_debug)
        buginf("\nppp: lex_ncp: cilen = %d", cilen);
    return(cilen);
}


/*
 * lex_addci - Add our desired CIs to a packet.
 */
static void lex_addci (fsm *f, uchar *ucp)
{
    int i;
    lex_options *go;
    lex_options *ho;

    go = f->gotoptions;
    ho = f->heroptions;

    if (ppp_negotiation_debug)
        buginf("\nppp:lex_addci: sending CONFREQ, types => ");

    if (go->neg_mac_support) {
        for (i = 0; i < go->mac_support_count; i++) {
            PUTCHAR(CI_MAC_SUPPORT, ucp);
            PUTCHAR(LENCI_MAC_SUPPORT, ucp);
            PUTCHAR(go->mac_types[i], ucp);
            if (ppp_negotiation_debug)
                buginf("\nppp: lex_addci: CI_MAC_SUPPORT= %d", go->mac_types[i]);
        }
    }
    if (go->neg_tinygram_comp) {
        PUTCHAR(CI_TINYGRAM, ucp);
        PUTCHAR(LENCI_TINYGRAM, ucp);
        PUTCHAR(go->tinygram_state, ucp);
        if (ppp_negotiation_debug)
            buginf("\nppp: lex_addci: CI_TINYGRAM= %d", go->tinygram_state);
    }
    if (go->neg_mac_address) {
        PUTCHAR(CI_MAC_ADDRESS, ucp);
        PUTCHAR(LENCI_MAC_ADDRESS, ucp);
        for (i = 0; i < IEEEBYTES; i++)
            PUTCHAR(go->mac_address[i], ucp);
        if (ppp_negotiation_debug)
            buginf("\nppp: lex_addci: CI_MAC_ADDRESS= %e", go->mac_address);
    }
    if (go->neg_spanning_tree) {
        PUTCHAR(CI_SPANNING_TREE, ucp);
        PUTCHAR(LENCI_SPANNING_TREE + go->span_proto_count, ucp);
        for (i = 0; i < go->span_proto_count; i++)
            PUTCHAR(go->span_protos[i], ucp);
        if (ppp_negotiation_debug)
            buginf("\nppp: lex_addci: CI_SPANNING_TREE= %d", go->span_protos[0]);
    }
    if (go->neg_mac_relay) {
        PUTCHAR(CI_MAC_RELAY, ucp);
        PUTCHAR(LENCI_MAC_RELAY, ucp);
        PUTCHAR(go->mac_relay_version, ucp);
        if (ppp_negotiation_debug)
            buginf("\nppp: lex_addci: CI_MAC_RELAY= %d", go->mac_relay_version);
    }
}


/*
 * lex_ackci - Ack our CIs.
 *
 * Returns:
 *	0 - Ack was bad.
 *	1 - Ack was good.
 *
 * This routine ensures that a received ACK message is valid. An ACK
 * message must contain all of the options that we specified, in the order
 * that we specified.
 */

static int lex_ackci (fsm *f, uchar *p, int len)
{
    int i;
    uchar cilen, citype, mactype, st_proto, opt_value;
    lex_options *go;
    lex_options *ho;
    uchar macaddr[IEEEBYTES];

    go = f->gotoptions;
    ho = f->heroptions;

    /*
     * CIs must be in exactly the same order that we sent...
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
    if (go->neg_mac_support) {
        for (i = 0; i < go->mac_support_count; i++) {
            len -= LENCI_MAC_SUPPORT;
            if (len < 0)
                goto bad;
            GETCHAR(citype, p);
            GETCHAR(cilen, p);
            if ((cilen != LENCI_MAC_SUPPORT) || (citype != CI_MAC_SUPPORT)) {
                if (ppp_error_debug)
                    buginf("\nppp: lex_ackci: neg_mac_support- bad cilen or citype");
                goto bad;
            }
            GETCHAR(mactype, p);
            if (mactype != go->mac_types[i]) {
                if (ppp_error_debug)
                    buginf("\nppp: lex_ackci: bad mactype");
                goto bad;
            }
        }
    }
    if (go->neg_tinygram_comp) {
        len -= LENCI_TINYGRAM;
        if (len < 0)
            goto bad;
        GETCHAR(citype, p);
        GETCHAR(cilen, p);
        if ((cilen != LENCI_TINYGRAM) || (citype != CI_TINYGRAM)) {
            if (ppp_error_debug)
                buginf("\nppp: lex_ackci: neg_tinygram_comp- bad cilen or citype");
            goto bad;
        }
        GETCHAR(opt_value, p);
        if (opt_value != go->tinygram_state) {
            if (ppp_error_debug)
                buginf("\nppp: lex_ackci: bad tinygram_state");
            goto bad;
        }
    }
    if (go->neg_mac_address) {
        len -= LENCI_MAC_ADDRESS;
        if (len < 0)
            goto bad;
        GETCHAR(citype, p);
        GETCHAR(cilen, p);
        if ((cilen != LENCI_MAC_ADDRESS) || (citype != CI_MAC_ADDRESS)) {
            if (ppp_error_debug)
                buginf("\nppp: lex_ackci: neg_mac_address- bad cilen or citype");
            goto bad;
        }
        for (i = 0; i < IEEEBYTES; i++)
            GETCHAR(macaddr[i], p);
        if (!ieee_equal(go->mac_address, macaddr)) {
            if (ppp_error_debug)
                buginf("\nppp: lex_ackci: neg_mac_address- bad mac address");
            goto bad;
        }
    }
    if (go->neg_spanning_tree) {
        len -= (LENCI_SPANNING_TREE + go->span_proto_count);
        if (len < 0)
            goto bad;
        GETCHAR(citype, p);
        GETCHAR(cilen, p);
        if ((cilen != LENCI_SPANNING_TREE + go->span_proto_count) || 
            (citype != CI_SPANNING_TREE)) {
            if (ppp_error_debug)
                buginf("\nppp: lex_ackci: neg_spanning_tree- bad cilen or citype");
            goto bad;
        }
        for (i = 0; i < go->span_proto_count; i++) {
            GETCHAR(st_proto, p);
            if (st_proto != go->span_protos[i]) {
                if (ppp_error_debug)
                    buginf("\nppp: lex_ackci: neg_mac_address- bad mac address");
                goto bad;
            }
        }
    }

    if (go->neg_mac_relay) {
        len -= LENCI_MAC_RELAY;
        if (len < 0)
            goto bad;
        GETCHAR(citype, p);
        GETCHAR(cilen, p);
        if ((cilen != LENCI_MAC_RELAY) || (citype != CI_MAC_RELAY)) {
            if (ppp_error_debug)
                buginf("\nppp: lex_ackci: neg_mac_relay- bad cilen or citype");
            goto bad;
        }
        GETCHAR(opt_value, p);
        if (opt_value != go->mac_relay_version) {
            if (ppp_error_debug)
                buginf("\nppp: lex_ackci: bad mac_relay_version");
            goto bad;
        }
    }
    /*
     * If there are any remaining CIs, then this packet is bad.
     */
    if (len != 0) {
        if (ppp_error_debug)
            buginf("\nppp: lex_ackci: length is wrong- more CIs remain");
	goto bad;
    }
    if (ppp_negotiation_debug)
        buginf("\nppp: lex_ackci: ACK-ed all options");
    return (1);
bad:
    if (ppp_error_debug)
	buginf ( "\nppp: lex_ackci: received bad Ack!");
    return (0);
}


/*
 * lex_nakci - NAK some of our CIs.
 *
 * Returns:
 *	0 - Nak was bad.
 *	1 - Nak was good.
 *
 * This routine handles a received NAK message. The NAK message will
 * list all of the options that we specified, the peer supports, but
 * the peer would prefer a different value.
 */

static void lex_nakci (fsm *f, uchar *p, int len)
{
    lex_options *go;
    lex_options *ho;
    uchar ci_mactype, st_proto, opt_value;
    uchar macaddr[IEEEBYTES];
    int i;

    go = f->gotoptions;
    ho = f->heroptions;

    /*
     * Any Nak'd CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
    
    /*
     * According to section 5.3 (page 21) of the PPP bridging draft,
     * the MAC SUPPORT option MUST NOT be included in a Configure-NAK.
     */
    if (go->neg_mac_support &&
        len >= LENCI_MAC_SUPPORT &&
        p[0] == CI_MAC_SUPPORT &&
        p[1] == LENCI_MAC_SUPPORT) {
            len -= LENCI_MAC_SUPPORT;
            INCPTR(PPP_TYPE_LEN_SIZE, p);
            GETCHAR(ci_mactype, p);
            if (ppp_negotiation_debug)
                buginf("\nppp: lex_nakci: config NAK received, type = CI_MAC_SUPPORT");
    }

    if (go->tinygram_state &&
        len >= LENCI_TINYGRAM &&
        p[0] == CI_TINYGRAM &&
        p[1] == LENCI_TINYGRAM) {
            len -= LENCI_TINYGRAM;
            INCPTR(PPP_TYPE_LEN_SIZE, p);
            GETCHAR(opt_value, p);
            if (ppp_negotiation_debug)
                buginf("\nppp: lex_nakci: config NAK received, type = CI_TINYGRAM");
    }

    /*
     * According to section 5.6 (page 24) of the PPP bridging draft,
     * when specifying a non-zero MAC address in a Configure-Request,
     * any inclusion of this option in a Configure-NAK MUST be ignored.
     * Systems that have no mechanism for address assignment will
     * Configure-Reject this option if a MAC address of 0 was sent.
     */
    if (go->neg_mac_address &&
        len >= LENCI_MAC_ADDRESS &&
        p[0] == CI_MAC_ADDRESS &&
        p[1] == LENCI_MAC_ADDRESS) {
        len -= LENCI_MAC_ADDRESS;
        INCPTR(PPP_TYPE_LEN_SIZE, p);
        for (i = 0; i < IEEEBYTES; i++)
            GETCHAR(macaddr[i], p);
        if (!ieee_equal(go->mac_address, macaddr))
            go->neg_mac_address = FALSE;
        if (ppp_negotiation_debug)
            buginf("\nppp: lex_nakci: CI_MAC_ADDRESS nakd, addr = %e", macaddr);
    }

    if (go->neg_spanning_tree &&
        len >= (LENCI_SPANNING_TREE + go->span_proto_count) &&
        p[0] == CI_SPANNING_TREE &&
        p[1] == (LENCI_SPANNING_TREE + go->span_proto_count)) {
            INCPTR(PPP_TYPE_LEN_SIZE, p);
            for (i = 0; i < go->span_proto_count; i++) {
                GETCHAR(st_proto, p);
                if (st_proto != go->span_protos[i]) {
                    go->neg_spanning_tree = FALSE;
                    if (ppp_error_debug)
                        buginf("\nppp: lex_ackci: neg_spanning tree-bad proto");
                }
            }
    }

    if (go->neg_mac_relay &&
        len >= LENCI_MAC_RELAY &&
        p[0] == CI_MAC_RELAY &&
        p[1] == LENCI_MAC_RELAY) {
            len -= LENCI_MAC_RELAY;
            INCPTR(PPP_TYPE_LEN_SIZE, p);
            GETCHAR(opt_value, p);
            if (ppp_negotiation_debug)
                buginf("\nppp: lex_nakci: config NAK received, type = CI_MAC_RELAY");
    }

    /*
     * If there are any remaining CIs, then this packet is bad.
     * At this point, we could check for any appended options.
     */
    if (len == 0)
	return;

    if (ppp_error_debug)
	buginf ("\nppp: lex_nakci: ignoring unrequested options!");
}


/*
 * lex_rejci - Reject some of our CIs.
 */
static void lex_rejci (fsm *f, uchar *p, int len)
{
    int i;
    uchar macaddr[IEEEBYTES];
    uchar mactype, st_proto, opt_value;
    lex_options *go;
    lex_options *ho;

    go = f->gotoptions;
    ho = f->heroptions;

    /*
     * Any Rejected CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */

    if (go->neg_mac_support)
        for (i = 0; i < go->mac_support_count; i++) {
            if ((len >= LENCI_MAC_SUPPORT) &&
               (p[0] == CI_MAC_SUPPORT) && (p[1] == LENCI_MAC_SUPPORT)) {
                len -= LENCI_MAC_SUPPORT;
                INCPTR(PPP_TYPE_LEN_SIZE, p);
                GETCHAR(mactype, p);
                if (mactype != go->mac_types[i])
                    goto bad;
                if (ppp_negotiation_debug)
                    buginf("\nppp: lex_nakci: config NAK received, type = CI_MAC_SUPPORT"
);
        }
    }

    if (go->neg_tinygram_comp && (len >= LENCI_TINYGRAM) &&
        (p[0] == CI_TINYGRAM) && (p[1] == LENCI_TINYGRAM)) {
            len -= LENCI_TINYGRAM;
            INCPTR(PPP_TYPE_LEN_SIZE, p);
            GETCHAR(opt_value, p);
            if (opt_value != go->tinygram_state)
                goto bad;
            if (ppp_negotiation_debug)
                buginf("\nppp: lex_nakci: config NAK received, type = CI_TINYGRAM");
    }

    if (go->neg_mac_address && (len >= LENCI_MAC_ADDRESS) && 
        (p[0] == CI_MAC_ADDRESS) && (p[1] == LENCI_MAC_ADDRESS)) {
        len -= LENCI_MAC_ADDRESS;
        INCPTR(PPP_TYPE_LEN_SIZE, p);
        for (i = 0; i < IEEEBYTES; i++)
            GETCHAR(macaddr[i], p);
        if (!ieee_equal(go->mac_address, macaddr))
            goto bad;
        go->neg_mac_address = FALSE;
        if (ppp_negotiation_debug)
            buginf("\nppp: lex_rejci: CI_MAC_ADDRESS rejected, addr = %e", macaddr);
    }

    if (go->neg_spanning_tree &&
        len >= (LENCI_SPANNING_TREE + go->span_proto_count) &&
        p[0] == CI_SPANNING_TREE &&
        p[1] == (LENCI_SPANNING_TREE + go->span_proto_count)) {
            INCPTR(PPP_TYPE_LEN_SIZE, p);
            for (i = 0; i < go->span_proto_count; i++) {
                GETCHAR(st_proto, p);
                if (st_proto != go->span_protos[i])
                    goto bad;
            }
            go->neg_spanning_tree = FALSE;
            if (ppp_negotiation_debug)
                buginf("\nppp: lex_rejci: CI_SPANNING_TREE rejected");
    }

    if (go->neg_mac_relay && (len >= LENCI_MAC_RELAY) &&
        (p[0] == CI_MAC_RELAY) && (p[1] == LENCI_MAC_RELAY)) {
            len -= LENCI_MAC_RELAY;
            INCPTR(PPP_TYPE_LEN_SIZE, p);
            GETCHAR(opt_value, p);
            if (opt_value != go->mac_relay_version)
                goto bad;
            if (ppp_negotiation_debug)
                buginf("\nppp: lex_nakci: config NAK received, type = CI_MAC_RELAY");
    }

    /*
     * If there are any remaining CIs, then this packet is bad.
     */

    if (len == 0)
	return;
bad:
    if (ppp_error_debug)
	buginf ( "\nppp: lex_rejci: received bad Reject!");
}


/*
 * lex_reqci - Check the peer's requested CIs and send appropriate response.
 *
 * Returns: CONFACK, CONFNAK or CONFREJ and input packet modified
 * appropriately.
 */
static uchar lex_reqci (fsm *f, uchar *inp, short *len)
{
    lex_options *wo;
    lex_options *ho;
    lex_options *ao;
    lex_options *go;
    uchar *cip;		/* Pointer to Current CI */
    ushort cilen, citype;	/* Parsed len, type */
    int rc = CONFACK;		/* Final packet return code */
    int orc;			/* Individual option return code */
    uchar *p = inp;		/* Pointer to next char to parse */
    uchar *ucp = inp;		/* Pointer to current output char */
    int len_left = *len;	/* Length left */
    uchar mactype, st_proto, opt_value;
    uchar macaddr[IEEEBYTES];
    int i;

    wo = f->wantoptions;
    ao = f->allowoptions;
    ho = f->heroptions;
    go = f->gotoptions;

    /*
     * Reset all her options.
     */
    ho->neg_mac_support = FALSE;
    ho->neg_tinygram_comp = FALSE;
    ho->neg_mac_address = FALSE;
    ho->neg_spanning_tree = FALSE;
    ho->neg_mac_relay = FALSE;
    ho->mac_support_count = 0;
    for (i = 0; i < CI_MAX_MACTYPE; i++)
        ho->mac_types[i] = 0;
    ho->span_proto_count = 0;
    for (i = 0; i < CI_MAX_ST; i++)
        ho->span_protos[i] = 0;
    ho->tinygram_state = 0;
    ho->mac_relay_version = 0;
    
    /*
     * Process all her options.
     */
    while (len_left) {
	orc = CONFACK;			/* Assume success */
	cip = p;			/* Remember begining of CI */
	if (len_left < PPP_TYPE_LEN_SIZE || /* Not enough data for CI header */
	    p[1] < PPP_TYPE_LEN_SIZE ||		/*  CI length too small or */
	    p[1] > len_left) {		/*  CI length too big? */
	    if (ppp_error_debug)
		buginf ( "\nppp: lex_reqci: bad CI length!");
	    orc = CONFREJ;		/* Reject bad CI */
	    cilen = len_left;		/* Reject till end of packet */
	    len_left = 0;		/* Don't loop again */
	    goto endswitch;
	}
	GETCHAR(citype, p);		/* Parse CI type */
	GETCHAR(cilen, p);		/* Parse CI length */
	len_left -= cilen;		/* Adjust remaining length */
	cilen -= BCP_TYP_LEN_FLD_SIZE;	/* Adjust cilen to just data */

	switch (citype) {		/* Check CI type */

	  case CI_MAC_SUPPORT:

	    if (ppp_negotiation_debug)
	      buginf("\nppp: lex_reqci: Negotiate MAC Support");

	    if (!ao->neg_mac_support ||
		cilen != MAC_SUPPORT_VAL_SIZE) { /* Check CI length */ 
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }

            GETCHAR(mactype, p); /* Get supported MAC type */
	    if (ppp_negotiation_debug)
		buginf(": her MAC type is %i", mactype);

            /*
             * Check validity of specified MAC type
             */
            if (((mactype >= CI_IEEE_8023_CAN) && (mactype <= CI_FDDI_NCAN)) ||
                ((mactype >= CI_IEEE_8024_CAN) && (mactype <= CI_FDDI_CAN))) {
                
                ho->neg_mac_support = TRUE;
                ho->mac_support_count += 1;
                ho->mac_types[ho->mac_support_count - 1] = mactype;
            }
            else {
                orc = CONFREJ;          /* Reject CI */
                if (ppp_negotiation_debug)
                    buginf("\nppp: lex_reqci: invalid MACTYPE- rejected");
            }
	    break;

          case CI_TINYGRAM:

	    if (ppp_negotiation_debug)
	      buginf("\nppp: lex_reqci: Negotiate Tinygram compression");

	    if (!ao->neg_tinygram_comp ||
		cilen != TINYGRAM_VAL_SIZE) { /* Check CI length */ 
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }

            GETCHAR(opt_value, p); /* Get tinygram comp state */
	    if (ppp_negotiation_debug)
		buginf(": her tinygram compression state is %i", opt_value);

            /*
             * Check validity of specified tinygram state
             */
            if ((opt_value == CI_TINYGRAM_ENABLED) || 
                (opt_value == CI_TINYGRAM_DISABLED)) {
                
                ho->neg_tinygram_comp = TRUE;
                ho->tinygram_state = opt_value;
            }
            else {
                orc = CONFREJ;          /* Reject CI */
                if (ppp_negotiation_debug)
                    buginf("\nppp: lex_reqci: invalid tinygram state - rejected");
            }
	    break;

          case CI_MAC_ADDRESS:

            if (ppp_negotiation_debug)
              buginf("\nppp: lex_reqci: Negotiate MAC address");

            if ((!ao->neg_mac_address ||
		/* Check CI length */
                cilen != LENCI_MAC_ADDRESS - PPP_TYPE_LEN_SIZE)) { 
                INCPTR(cilen, p);       /* Skip rest of CI */
                orc = CONFREJ;          /* Reject CI */
                if (ppp_negotiation_debug)
                    buginf(" (rejected)");
                break;
            }

            for (i = 0; i < IEEEBYTES; i++)
                GETCHAR(macaddr[i], p);

            if ((is_ieee_zero(macaddr)) || (ISMULTICAST(macaddr))) {
                orc = CONFREJ;          /* Reject CI */
                if (ppp_negotiation_debug)
                    buginf(" (invalid MAC address rejected)");
                break;
            }
            if (!reg_invoke_lex_find_free_bymac(macaddr)) {
                errmsg(&msgsym(NOMAC, PPP), macaddr);
                orc = CONFREJ;          /* Reject CI */
                if (ppp_negotiation_debug)
                    buginf(" (invalid MAC address rejected)");
	    }
            else {
                ieee_copy(macaddr, ho->mac_address);
                ho->neg_mac_address = TRUE;
            }
	    break;

          case CI_SPANNING_TREE:

            if (!ao->neg_spanning_tree ||
                cilen > MAX_SPANNING_TREE_VAL_SIZE) { 
                INCPTR(cilen, p);       /* Skip rest of CI */
                orc = CONFREJ;          /* Reject CI */
                if (ppp_negotiation_debug)
                    buginf(" (rejected)");
                break;
            }

            for (i = 0; i < cilen; i++) {
                GETCHAR(st_proto, p);
                if (st_proto <= CI_SPAN_DEC) {
                    ho->span_protos[i] = st_proto;
                    ho->span_proto_count += 1;
                }
                else {
                    INCPTR(cilen - (i + 1), p);       /* Skip rest of CI */
                    orc = CONFREJ;          /* Reject CI */
                    if (ppp_negotiation_debug)
                        buginf(" (rejected)");
                    break;
                }
                ho->neg_spanning_tree = TRUE;
            }

            break;

          case CI_MAC_RELAY:

	    if (ppp_negotiation_debug)
	      buginf("\nppp: lex_reqci: Negotiate MAC relay");

	    if (!ao->neg_mac_relay ||
		cilen != MAC_RELAY_VAL_SIZE) { /* Check CI length */ 
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }

            GETCHAR(opt_value, p); /* Get MAC relay protocol version # */
	    if (ppp_negotiation_debug)
		buginf(": her MAC relay protocol version is %i", opt_value);

            /*
             * Check validity of specified MAC relay protocol version #
             */
            if (opt_value == CI_MACRELAY_PROTO_SUP) {
                ho->neg_mac_relay = TRUE;
                ho->mac_relay_version = opt_value;
            }
            else {
                orc = CONFREJ;          /* Reject CI */
                if (ppp_negotiation_debug)
                    buginf("\nppp: lex_reqci: invalid MAC relay protocol version # - rejected");
            }
	    break;
	  default:
	    if (ppp_error_debug)
	      buginf("\nppp %s: Rejecting unknown option %d",
		     f->idb->hw_namestring, citype);
	    INCPTR(cilen, p);
	    orc = CONFREJ;
	    break;
	}
	cilen += PPP_TYPE_LEN_SIZE;		/* Adjust cilen whole CI */

endswitch:
    if (ppp_negotiation_debug)
	buginf ( " (%s)\n",
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
     * If we wanted to send additional NAKs (for unsent CIs), the
     * code would go here.  This must be done with care since it might
     * require a longer packet than we received.
     */

    *len = ucp - inp;			/* Compute output length */
    if (ppp_negotiation_debug)
	buginf ( "\nppp: lex_reqci: returning %s.",
	      rc == CONFACK ? "CONFACK" :
	      rc == CONFNAK ? "CONFNAK" : "CONFREJ");
    return (rc);			/* Return final code */
}

/*
 * lex_up - FSM has reached up state
 */

static void lex_up (fsm *f)
{
    int i;
    lex_options *go;
    lex_options *ho;
    lex_options *wo;

    ho = f->heroptions;
    go = f->gotoptions;
    wo = f->wantoptions;

    if (ppp_negotiation_debug) {
        buginf("\nppp: LEX reached the OPEN state");
        if (go->neg_mac_support) {
            buginf("\nppp: lex_up: go->neg_mac_support");
            buginf("\nppp: lex_up: my mac support types = ");
            for (i = 0; i < go->mac_support_count; i++) 
                buginf(" %d ", go->mac_types[i]);

        }
        if (ho->neg_mac_support) {
            buginf("\nppp: lex_up: ho->neg_mac_support");
            buginf("\nppp: lex_up: peer's mac_types = ");
            for (i = 0; i < ho->mac_support_count; i++) 
                buginf(" %d ", ho->mac_types[i]);
            
        }
        if (go->neg_tinygram_comp) {
            buginf("\nppp: lex_up: go->neg_tinygram_comp");
            buginf("\nppp: lex_up: my tinygram state = %d", go->tinygram_state);
        }
        if (ho->neg_tinygram_comp) {
            buginf("\nppp: lex_up: ho->neg_tinygram_comp");
            buginf("\nppp: lex_up: peer's tinygram state = %d", ho->tinygram_state);
        }
        if (go->neg_mac_address) {
            buginf("\nppp: lex_up: go->neg_mac_address");
            buginf("\nppp: lex_up: my MAC address = %e", wo->mac_address);
        }
        if (ho->neg_mac_address) {
            buginf("\nppp: lex_up: ho->neg_mac_address");
            buginf("\nppp: lex_up: peer's MAC address = %e", ho->mac_address);
        }

        if (go->neg_spanning_tree) {
            buginf("\nppp: lex_up: go->neg_spanning_tree");
            buginf("\nppp: lex_up: my span_proto_count = %d", go->span_proto_count);
            buginf("\nppp: lex_up: my span_protos = %d", go->span_protos[0]);
        }
        if (ho->neg_spanning_tree) {
            buginf("\nppp: lex_up: ho->neg_spanning_tree");
            buginf("\nppp: lex_up: peer's span_proto_count = %d", ho->span_proto_count);
            buginf("\nppp: lex_up: peer's span_protos = %d", ho->span_protos[0]);
        }
        if (go->neg_mac_relay) {
            buginf("\nppp: lex_up: go->neg_mac_relay");
            buginf("\nppp: lex_up: my MAC relay = %d", go->mac_relay_version);
        }
        if (ho->neg_mac_relay) {
            buginf("\nppp: lex_up: ho->neg_mac_relay");
            buginf("\nppp: lex_up: her MAC relay = %d", ho->mac_relay_version);
        }
    }
    if (ho->neg_mac_address) {
        reg_invoke_lex_bind_bymac(f->idb->firstsw, ho->mac_address);
    }

}

static void lex_down (fsm *f)
{
    if (f->idb->firstsw->lex_swidb != NULL)
        reg_invoke_lex_unbind_byidb(f->idb->firstsw->lex_swidb);
}

void close_a_lex_ncp (hwidbtype *hwidb)
{
    fsm *finitesm;

    /*
     * Close the lex ncp and change the fsm state to NOT NEGOTIATED
     * (PPP_UNKNOWN) so that the flex box will try to re-negotiate.
     */
    finitesm = reg_invoke_ppp_link2fsm(LINK_LEX, hwidb);
    if (finitesm && ((finitesm->state == OPEN) ||
		     (finitesm->state == CLOSED))) {
	ncp_close(NULL, finitesm);
	finitesm->state = PPP_UNKNOWN;
    }
}   

static fsm_callbacks lex_callbacks = { /* LEX callback routines */
    lex_resetci,		/* Reset our Configuration Information */
    lex_cilen,			/* Length of our Configuration Information */
    lex_addci,			/* Add our Configuration Information */
    lex_ackci,			/* ACK our Configuration Information */
    lex_nakci,			/* NAK our Configuration Information */
    lex_rejci,			/* Reject our Configuration Information */
    lex_reqci,			/* Request peer's Configuration Information */
    lex_up,			/* Called when fsm reaches OPEN state */
    lex_down,			/* Called when fsm leaves OPEN state */
    lex_down,			/* Called when fsm reaches CLOSED state */
    NULL,			/* Called when Protocol-Reject received */
    NULL,			/* Retransmission is necessary */
    NULL,			/* Reset Request */
    NULL,			/* Rest Acknowledge */
    NULL			/* Code Rejected */
};

/*
 * lex_init - Initialize LEX.
 */
static void lex_ncp_init (fsm *lex_fsm)
{
    hwidbtype *idb;

    idb = lex_fsm->idb;

    if (!lex_fsm->wantoptions)
	lex_fsm->wantoptions = malloc(sizeof(lex_options));
    else
	memset(lex_fsm->wantoptions, 0, sizeof(lex_options));
    if (!lex_fsm->wantoptions)
	return;

    if (!lex_fsm->gotoptions)
	lex_fsm->gotoptions = malloc(sizeof(lex_options));
    else
	memset(lex_fsm->gotoptions, 0, sizeof(lex_options));
    if (!lex_fsm->gotoptions) {
	free(lex_fsm->wantoptions);
	return;
    }

    if (!lex_fsm->allowoptions)
	lex_fsm->allowoptions = malloc(sizeof(lex_options));
    else
	memset(lex_fsm->allowoptions, 0, sizeof(lex_options));
    if (!lex_fsm->allowoptions) {
	free(lex_fsm->wantoptions);
	free(lex_fsm->gotoptions);
	return;
    }

    if (!lex_fsm->heroptions)
	lex_fsm->heroptions = malloc(sizeof(lex_options));
    else
	memset(lex_fsm->heroptions, 0, sizeof(lex_options));
    if (!lex_fsm->heroptions) {
	free(lex_fsm->wantoptions);
	free(lex_fsm->gotoptions);
	free(lex_fsm->allowoptions);
	return;
    }

    lex_fsm->ouroptions = NULL;

    lex_fsm->retransmits = 0;
    TIMER_STOP(lex_fsm->timeouttime);
    lex_fsm->failures = 0;
    lex_fsm->restarttime = DEFTIMEOUT;
    lex_fsm->maxconfigure = DEFMAXCONFIGURE;
    lex_fsm->maxtermtransmits = DEFMAXTERMTRANSMITS;
    lex_fsm->maxnakloops = DEFMAXNAKLOOPS;
    lex_fsm->maxfailure = DEFMAXFAILURE;
    lex_fsm->callbacks = &lex_callbacks;
    lex_fsm->flags |= AOPENDING;           /* Active Open */
}


/*
 * lexcp_init_subsys
 * Initialization of LEXCP subsystem
 */

static void lexcp_init_subsys (subsystype *subsys)
{
    reg_invoke_ncp_register(LINK_LEX, TYPE_PPP_LEXCP,
			    lex_ncp_init, NULL, "LEX");
    /*
     * The following call is only half of a kludge but the significance of
     * the second parameter can not be overlooked.  ncp_register_link() will
     * only use the second parameter if it is <= 0xff, so that part of the
     * call is a NOP.  The only goal is to to link LINK_LEX_RCMD to the
     * FSM of LINK_LEX.  An alternative to TYPE_PPP_LEXCP would be to send
     * 0 which is an illegal type code but unfortunately there was no check
     * to catch this case.
     */
    reg_invoke_ncp_register_link(LINK_LEX_RCMD, TYPE_PPP_LEXCP, LINK_LEX);
}

/*
 * The LEXCP subsystem header
 */

#define LEXCP_MAJVERSION 1
#define LEXCP_MINVERSION 0
#define LEXCP_EDITVERSION  1

SUBSYS_HEADER(lexcp, LEXCP_MAJVERSION, LEXCP_MINVERSION, LEXCP_EDITVERSION,
	      lexcp_init_subsys, SUBSYS_CLASS_PROTOCOL,
	      "seq: ppp",
	      "req: ppp");
