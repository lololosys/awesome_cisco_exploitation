/* $Id: ncp.c,v 3.11.20.10 1996/08/30 08:40:41 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/ncp.c,v $
 *------------------------------------------------------------------
 * ncp.c - generic Control Protocol for multiprotocols over PPP.
 *
 * 05 Feb 1992, Sanjeev Newarikar
 *
 * Copyright (c) 1992-1997, by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncp.c,v $
 * Revision 3.11.20.10  1996/08/30  08:40:41  fox
 * CSCdi66686:  Shivas NBFCP protocol (0x8fec) is not properly rejected
 * Branch: California_branch
 * Protocol reject all unknown protocols and unsupported NCPs.  Reject
 * broken Shiva protocol #s using the full protocol number.  Fix
 * support for protocol rejecting packets with compressed protocol #s.
 *
 * Revision 3.11.20.9  1996/08/28  13:22:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.20.8  1996/08/18  09:08:22  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.11.20.7  1996/08/03  23:41:57  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.11.20.6  1996/06/28  20:09:04  fox
 * CSCdi61488:  Interface input queue reaches max and PPP fails
 * Branch: California_branch
 * ncp_input() handling of unsupported protocols was broken by CSCdi59550.
 *
 * Revision 3.11.20.5  1996/06/24  09:27:48  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.11.20.4  1996/05/10  01:45:10  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.11.20.3  1996/04/29  09:45:57  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.11.20.2  1996/04/27  06:37:07  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.6.8.3  1996/04/25  23:59:30  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.6.8.2  1996/04/17  00:01:33  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.8.1  1996/02/23  21:31:47  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.11.20.1  1996/03/18  22:48:09  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.2.2  1996/03/07  11:07:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.8.2.1  1996/02/20  21:52:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/02/04  15:37:44  fox
 * CSCdi48127:  First option in packet should start on its own debug line
 * Put each option of a Configuration packet on its own line
 * Fix offset to avoid bogus options being displayed
 *
 * Revision 3.10  1996/01/29  23:31:32  vandys
 * CSCdi47776:  ncp_protocols table has missing comma
 *
 * Revision 3.9  1996/01/23  18:22:35  fox
 * CSCdi47345:  Unused functions are unneeded functions
 * ncp_closed is never referenced
 * seen_addr and seen_pool are redundant
 *
 * Revision 3.8  1996/01/17  09:05:24  dmccowan
 * CSCdi47015:  crash in ncp_activeopen
 * fix NULL ptr dereference.
 *
 * Revision 3.7  1996/01/16  09:03:06  misko
 * CSCdi46888:  PPP doesnt allow a protocol to be added later
 * Fix the case where we've received a protocol reject, but then the
 * other side starts sending what they once rejected.  We should then
 * be allowed to send those packets also.  At the same time, apply
 * CSCdi29247 to 11.0.
 *
 * Revision 3.6  1995/12/21  19:24:52  dblair
 * CSCdi45604:  Crashed later on after changing the BRI encaps to PPP on
 * other side.
 *
 * Revision 3.5  1995/12/07  22:34:16  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/11/27  18:30:03  pst
 * CSCdi44460:  cosmetic cleanup of ppp / multilink information
 *
 * Revision 3.3  1995/11/17  18:03:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:11  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:50:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:36:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/23  13:45:11  dblair
 * CSCdi42550:  Add multilink status and clean up lcp/ncp stats in show
 * interface
 *
 * Revision 2.1  1995/06/07  23:18:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




/*
 * includes
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "../iprouting/route.h"
#include "logger.h"
#include "../wan/msg_ppp.c"		/* Not a typo, see logger.h */
#include "../les/isdn_bri.h"
#include "../ip/ip.h"
#include "../clns/clns_externs.h"
#include "../decnet/decnet.h"
#include "ppp.h"
#include "ncp.h"
#include "ipcp.h"
#include "atcp.h"
#include "ccp.h"
#include "nbfcp.h"
#include "../xns/novell.h"
#include "ipxcp.h"
#include "ppp_public.h"
#include "cdp_ncp.h"
#include "dialer.h"
#include "dialer_registry.h"

/*
 * externs and vars
 */

ncpconst ncptbl[MAX_NCP];

uchar link2ncpidxtbl[LINK_MAXLINKTYPE];

/*
 * The quick lookup table is designed to map the most frequent PPP packet
 * types to the ncp index.  This provides a very fast lookup that can be
 * dynamically extended with minimal code changes.  Protocols not found in
 * the quick lookup table can be found by scanning the ncp table.
 */
uchar ppp_quick_lookup[PPP_QUICK_LOOKUP_SIZE];

int max_ncp = 0;		/* number of valid entries in ncptbl[] */
int first_ncp = 0;		/* index of first real NCP registered */
static boolean ncptbl_closed = FALSE;	/* flag set in ncp_init() */

static fsm_callbacks ncp_callbacks = {
    NULL,
    NULL,
    NULL,
    ncp_ackci,
    NULL,
    NULL,
    NULL,
    ncp_up,
    NULL,
    NULL,
    NULL,
    NULL,
	NULL,
	NULL,
	NULL
};

/*
 *code begins here
 */


static boolean ppp_protocheck (hwidbtype *netidb,
			       hwidbtype *ignore,
			       int linktype)
{
    return(reg_invoke_proto_on_swidb(linktype, netidb->firstsw));
}

/*
 * ncp_register
 *
 * Register new NCP.  Add information to ncptbl[].
 */

void
ncp_register (int linktype,
	      ushort protocol,
	      void (*init)(fsm *),
	      boolean (*protocheck)(hwidbtype *, hwidbtype *, int),
	      const char *name)
{
    int ncpidx;

    /*
     * Sanity check the linktype to see that it is within range.
     */
    if ((linktype <= LINK_ILLEGAL) || (linktype >= LINK_MAXLINKTYPE)) {
	errmsg(&msgsym(NOREGISTER, PPP), protocol);
	return;
    }

    /*
     * Sanity check protocol.  Just check for being non-zero at this time,
     * but this could be improved to validate correctness, etc.  Probably
     * not necessary for an internal function.  Do check during search for
     * open slot to make sure we are registered once and only once.
     */
    if (!protocol) {
	errmsg(&msgsym(NOREGISTER, PPP), protocol);
	return;
    }

    /*
     * Search for open slot and simultaneously verify that this entry has
     * not been done yet.  Obviously, this could be done with an incrementing
     * index, but we should still search the table to avoid the possibility
     * of duplicates.
     */
    FOR_ALL_NCPS(ncpidx) {
	if (ncptbl[ncpidx].ncp_linktype == linktype) {
	    errmsg(&msgsym(NOREGISTER, PPP), protocol);
	    return;
	}
    }

    /*
     * Sanity check that there are enough slots in the ncp table.
     */
    if (max_ncp == MAX_NCP) {
	errmsg(&msgsym(NOREGISTER, PPP), protocol);
	return;
    }

    /*
     * Sanity check that no PPP interfaces have been initialized.  I am
     * assuming that all NCPs will register at subsystem initialization.
     * If they try to register after an interface has been initialized,
     * we would have to go back to all PPP interfaces and adjust the
     * ncpfsmidx table.  If the table were statically configured to its
     * maximum possible size this would be trivial but then we would lose
     * some possible space savings in non-Enterprise images and face a
     * further complication with downstream assumptions that everything is
     * in place and stable.  Better to require the early registration than
     * to debug the possible problems resulting from late registration.
     * Note that one nasty possibility is a subsystem that registers early
     * MOST of the time.... :-(
     */
    if (ncptbl_closed) {
	errmsg(&msgsym(NOREGISTER, PPP), protocol);
	return;
    }

    /*
     * Treat Multilink PPP as a pseudo NCP.  But since we do not want any
     * NCP processing to apply to Multilink, we will hide it at the beginning
     * of the NCP tables.
     */
    if (linktype == LINK_MULTILINK) {
	if (max_ncp > 0) {
	    errmsg(&msgsym(NOREGISTER, PPP), protocol);
	    return;
	}
	first_ncp = 1;
    }

    /*
     * Fill in the next entry and adjust the counter.  Note that entries
     * cannot be removed once added.  If code is ever added to do so, either
     * this code should search the entire table (similar changes to similar
     * functions) or the removal code should move the last entry to where
     * the removed entry was located and the max_ncp counter should be
     * decremented.
     */
    ncptbl[max_ncp].ncp_linktype = linktype;
    ncptbl[max_ncp].ncp_protocol = protocol;
    ncptbl[max_ncp].ncp_init = init;
    ncptbl[max_ncp].protocheck =
	(protocheck == NULL) ? ppp_protocheck : protocheck;
    ncptbl[max_ncp].ncp_name = name;
    max_ncp++;

    /*
     * Add to link-to-index table.  We use the ncp counter after it is
     * incremented so that it will always be non-zero as the array is
     * initialized to zero.  Mustn't forget to decrement before using the
     * indexes in this array.
     */
    link2ncpidxtbl[linktype] = max_ncp;

    /*
     * Add to quick lookup table if applicable.  Note that the value stored
     * is the linktype.  The NCP protocol is USUALLY the data protocol OR'd
     * with 0x8000.  So strip off bit 15 and see if it fits into the quick
     * index table.  Since all PPP protocol types are * 'odd', we can save
     * a small amount of space by shifting over by one and using a table that
     * is half-size.
     */
    protocol &= NCP_PROTOCOL_MASK;
    if (protocol < PPP_QUICK_LOOKUP_MAX) {
	ppp_quick_lookup[protocol >> 1] = linktype;
    }
}


/*
 * ncp_register_link
 *
 * Register extra data protocol/link type to existing NCP.  This is called
 * immediately after ncp_register().
 */

void
ncp_register_link (int linktype, ushort protocol, int ncp_linktype)
{
    /*
     * Sanity check the linktype to see that it is within range.
     */
    if ((linktype <= LINK_ILLEGAL) || (linktype >= LINK_MAXLINKTYPE)) {
	errmsg(&msgsym(NOREGISTER, PPP), protocol);
	return;
    }

    /*
     * Sanity check the NCP linktype to see that it is within range and
     * has been initialized already.
     */
    if ((ncp_linktype <= LINK_ILLEGAL) ||
	(ncp_linktype >= LINK_MAXLINKTYPE) ||
	!link2ncpidxtbl[ncp_linktype]) {
	errmsg(&msgsym(NOREGISTER, PPP), protocol);
	return;
    }

    /*
     * Add to link-to-index table.
     */
    link2ncpidxtbl[linktype] = link2ncpidxtbl[ncp_linktype];

    /*
     * Add to quick lookup table if applicable (protocol must be odd (thus
     * non-zero) and less than 0x100).  Note that the NCP linktype is stored
     * of the data protocol linktype.
     */
    if ((protocol & 1) && (protocol < PPP_QUICK_LOOKUP_MAX)) {
	ppp_quick_lookup[protocol >> 1] = linktype;
    }
}


/*
 * ncp_init()
 *
 * Initialize all PPP NCP's which we are supporting. If any special init
 * is required then corresponding routine is called otherwise do the
 * default init here
 */

void ncp_init (hwidbtype *idb)
{
    fsm *f;
    int ncpidx;
    ncp_options *wo;
    ncp_options *ao;

    ncptbl_closed = TRUE;	/* Block any further registrations */

    if (!idb->ncpfsmidx) {
    	idb->ncpfsmidx = malloc_named(sizeof(fsm *) * max_ncp,
				      "IDB: NCP FSM Index Table");
	if (!idb->ncpfsmidx) {
	    return;
	}
    }

    /* now initialize all the protocols & their fsms */

    FOR_ALL_NCPS(ncpidx) {

	f = idb->ncpfsmidx[ncpidx];
	if (!f) {
	    f = malloc_named(sizeof(fsm), "IDB: NCP FSM");
	    if (!f) {
		return;
	    }
	    idb->ncpfsmidx[ncpidx] = f;
	}

	f->idb = idb;
	f->protocol = ncptbl[ncpidx].ncp_protocol;

	if (ncptbl[ncpidx].ncp_init) {
	    (*ncptbl[ncpidx].ncp_init)(f);
	} else {
	    if (!f->wantoptions) {
		f->wantoptions =
		  malloc_named(sizeof(ncp_options), "NCP Options");
	    } else {
		memset(f->wantoptions, 0, sizeof(ncp_options));
	    }

	    if (!f->wantoptions) {
		return;
	    }
	    if (!f->allowoptions) {
		f->allowoptions =
		  malloc_named(sizeof(ncp_options), "NCP Options");
	    } else {
		memset(f->allowoptions, 0, sizeof(ncp_options));
	    }
	    if (!f->allowoptions) {
		return;
	    }
	    if (!f->heroptions) {
		f->heroptions =
		  malloc_named(sizeof(ncp_options), "NCP Options");
	    } else {
		memset(f->heroptions, 0, sizeof(ncp_options));
	    }
	    if (!f->heroptions) {
		return;
	    }
	    f->ouroptions = NULL;

	    wo = f->wantoptions;
	    ao = f->allowoptions;

	    f->retransmits = 0;
	    TIMER_STOP(f->timeouttime);
	    f->failures = 0;
	    f->callbacks = &ncp_callbacks;
	    f->flags |= AOPENDING;

	    wo->junk = 0;
	    ao->junk = 0;
	}
	fsm_init(f);
    }
}

/*
 * ncp_enable()
 *
 * Initialize and enable a specific pseudo NCP.  These NCPs are not negotiated,
 * but exist to speed the datapath lookup of internal and external types.
 * This reduces the special case code in the lookup path.
 */

void ncp_enable (hwidbtype *idb, int linktype)
{
    fsm *f;
    int ncpidx;

    /*
     * The NCP FSM table should have been constructed by now!
     */
    if (!idb->ncpfsmidx) {
	return;
    }

    /*
     * Find and initialize the protocol's fsm.  If no pseudo NCPs have
     * been registered, then the loop below will be a NOP.  Notice the
     * lack of diagnostic messages.  This should be fixed.
     */
    for (ncpidx = 0; ncpidx < first_ncp; ncpidx++) {
	if (linktype == ncptbl[ncpidx].ncp_linktype) {
	    f = idb->ncpfsmidx[ncpidx];
	    if (!f) {
		f = malloc_named(sizeof(fsm), "Multilink FSM");
		if (!f) {
		    return;
		}
		idb->ncpfsmidx[ncpidx] = f;
	    }

	    f->idb = idb;
	    f->callbacks = &ncp_callbacks;
	    f->protocol = ncptbl[ncpidx].ncp_protocol;
	    f->state = OPEN;
	    break;
	}
    }
}

/*
 * ncp_disable()
 *
 * Disable a specific pseudo NCP.  These NCPs are not negotiated,
 * but exist to speed the datapath lookup of internal and external types.
 * This reduces the special case code in the lookup path.
 */

void ncp_disable (hwidbtype *idb, int linktype)
{
    fsm *f;
    int ncpidx;

    /*
     * The NCP FSM table should have been constructed by now!
     */
    if (!idb->ncpfsmidx) {
	return;
    }

    /*
     * Find and initialize the protocol's fsm.  If no pseudo NCPs have
     * been registered, then the loop below will be a NOP.  Notice the
     * lack of diagnostic messages.  This should be fixed.
     */
    for (ncpidx = 0; ncpidx < first_ncp; ncpidx++) {
	if (linktype == ncptbl[ncpidx].ncp_linktype) {
	    f = idb->ncpfsmidx[ncpidx];
	    if (!f) {
		return;
	    }
	    f->state = CLOSED;
	    break;
	}
    }
}

/*
 * ncp_input()
 *
 * receive input for a particular NCP pkt
 */

void ncp_input (paktype *pak, hwidbtype *idb)
{
    fsm *f;
    int ncpidx;
    ushort protocol;
    hwidbtype *tempidb;
    ppp_hdr *ppp_hdrp;
    ncpconst *ncp;

    if (ppp_packet_debug) {
	ncp_print(pak);
    }

    tempidb = idb;
    protocol = ppp_get_protocol_type(pak);
    ppp_hdrp = (ppp_hdr *)pak->network_start;

    /*
     * I'm not happy about having to put this here, but it is the
     * easiest way at this time.  The "running" routine checks to
     * see if an upper level protocol (IP, Novell, etc.) is in use
     * on the idb.  The problem is for ISDN interfaces and for
     * dialer rotary groups, the upper level protocols only know
     * about the hunt group leader or the ISDN D channel idb.  So,
     * we need to make idb point to the hunt group leader or the
     * D channel idb before we call the "running" routine.  It's a
     * shame to have to do this here, but it is really the only place
     * it can be done.
     */

    tempidb = get_netidb(idb); /* Use the hunt group master idb */

    ncpidx = ppp_serial2ncpidx_inline(protocol);
    f = ppp_ncpidx2fsm_inline(ncpidx, idb);
    ncp = &ncptbl[ncpidx];

    /*
     * The following check confused me so let me try to explain.  Before
     * passing this packet to fsm_input(), we want to ensure that:
     * -we know about and support this protocol (f is non-NULL) AND
     * -the packet is an NCP packet and not a data packet (which fall into
     *  here if unknown or the FSM is not yet Open) AND
     * -the packet is Configure Request and we support this protocol on this
     *  line OR packet is not a Configure Request, which means that we are
     *  already verified that this protocol is supported on this link.
     *
     * If you noticed that we call the protocheck vector everytime we receive
     * a Configure Request, you are not alone.  This would be candidate for
     * improvement IFF we received a noticification call when a upper-level
     * protocol was enabled on the interface.
     */
    if (f &&
	(protocol > NCP_PROTOCOL_BASE) &&
	((ppp_hdrp->code != CONFREQ) ||
	 (ncp->protocheck)(tempidb, idb, ncp->ncp_linktype))) {
	fsm_input(f, pak);
    } else {
	uchar *proto_ptr;

	proto_ptr = ppp_get_protocol_type_ptr(pak);

	if (proto_ptr) {
	    /*
	     * NOTE: Early in the implementation of PPP, IANA assigned four PPP
	     *       protocol numbers to Shiva that violated the fundamental
	     *       rules of PPP Protocol numbers, ie the first byte must be
	     *       even and the second odd.  Shiva shipped product using these
	     *       numbers and continues to support them in current products.
	     *       IANA is supposed to reserve the four protocol numbers that
	     *       the broken protocols resemble (0x0F, 0x81, 0x8f and 0xc1).
	     *       We want to protocol reject these NCP packets using the
	     *       protocol types that the peer used.  CSCdi66686
	     */
	    if (protocol < PPP_QUICK_LOOKUP_MAX) {
		ushort broken_protocol;

		PPP_GETSHORT(broken_protocol, proto_ptr);
		switch (broken_protocol) {
		case BAD_TYPE_PPP_SNBF:
		case BAD_TYPE_PPP_SIPXCP:
		case BAD_TYPE_PPP_SNBFCP:
		case BAD_TYPE_PPP_OLDSPAP:
		    protocol = broken_protocol;
		    pak->network_start++; /* first char pkt is part of proto! */
		    pak->encsize++;
		}
	    }

	    /*
	     * We choose here to not reject any packets whose protocol might
	     * be enabled.  So we will reject a packet if its protocol is
	     * completely unsupported or if it is an NCP packet whose
	     * protocol is not supported on this interface.  If the packet's
	     * protocol is a supported 'data' protocol we will silently drop
	     * it.  This might occur if the NCP is already up on the remote
	     * end, but (for packet loss or whatever) is not up on the local
	     * side.
	     */
	    if (!f || (protocol > NCP_PROTOCOL_BASE)) {
		fsm_sprotrej(protocol, pak->network_start,
			     pak->datagramsize - pak->encsize, idb->lcpfsm);
	    }
	}
	datagram_done(pak);
    }
}

/*
 * ncp_activeopen()
 *
 * Do an active open for a relevant ncp
 */

void ncp_activeopen (fsm **nf, fsm *f)
{
    /*
     * Note, f and nf should never be non-NULL together. Might want to
     * consider adding a check here.  May not matter.
     */

    /*
     * We are forbidden to send frames for a particular NCP
     * once that protocol value has been rejected.  The reject
     * will have closed the state machine.  So what we want to
     * do is a passive open, so that should someone enable this
     * protocol on the other side of this connection, they will
     * send is a packet for this protocol, and then we will
     * start up.
     */
    if (nf) {
	int ncpidx;

	FOR_ALL_FSMS(nf, f, ncpidx) {
	    if (!(f->flags & WASREJECTED)) {
		fsm_activeopen(f);
	    } else {
		fsm_passiveopen(f);
	    }
	}
    } else {
        if (f) {
	    f->flags |= LOWERUP;  /* set lower up for confreq to go out */
	    f->flags |= AOPENDING;
            if (!(f->flags & WASREJECTED)) {
	        fsm_activeopen(f);
	    } else {
	        fsm_passiveopen(f);
	    }
	}
    }
}

/*
 * ncp_close()
 *
 * close a particular or all NCP's.
 */
void ncp_close (fsm **nf, fsm *f)
{
    if (nf) {
	int ncpidx;

	FOR_ALL_FSMS(nf, f, ncpidx) {
	    fsm_close(f);
	}
    } else {
	if (f) {
	    fsm_close(f);
	}
    }
}

/*
 * ncp_lowerup()
 *
 * set the lower layer up for a particular or all NCP's.
 */

void ncp_lowerup (fsm **nf)
{
    fsm *f;
    hwidbtype *idb;
    boolean proto_running;
    int ncpidx;

    FOR_ALL_FSMS(nf, f, ncpidx) {
	/*
	 * I'm not happy about having to put this here, but it is the
	 * easiest way at this time.  The "running" routine checks to
	 * see if an upper level protocol (IP, Novell, etc.) is in use
	 * on the idb.  The problem is for ISDN interfaces and for
	 * dialer rotary groups, the upper level protocols only know
	 * about the hunt group leader or the ISDN D channel idb.  So,
	 * we need to make idb point to the hunt group leader or the
	 * D channel idb before we call the "running" routine.  It's a
	 * shame to have to do this here, but it is really the only place
	 * it can be done.
	 */
	idb = get_netidb(f->idb); /* Use the hunt group master idb */

	proto_running = (*ncptbl[ncpidx].protocheck)
			    (idb, f->idb, ncptbl[ncpidx].ncp_linktype);

	if (proto_running) {
	    f->flags |= AOPENDING;
	    fsm_lowerup(f);
	} else {
	    f->state = PPP_UNKNOWN;
	}
    }
}

/*
 * ncp_lowerdown()
 *
 * set the lower layer down for all unclosed NCPs.
 */
void ncp_lowerdown (fsm **nf)
{
    fsm *f;
    int ncpidx;

    FOR_ALL_FSMS(nf, f, ncpidx) {
	if ((f->state != CLOSED) && (f->state != PPP_UNKNOWN)) {
	    fsm_lowerdown(f);
	} else {
	    f->state = CLOSED;
	}
	/*
	 * New LCP, allow all protos again
	 */
	f->flags &= ~(WASREJECTED|LOWERUP|AOPENDING|POPENDING);
    }
}

/*
 * ncp_protrej()
 *
 */

void ncp_protrej (hwidbtype *idb, ushort protocol)
{
    fsm *f;

    f = ppp_serial2fsm(protocol, idb);
    if (f) {
	f->flags |= WASREJECTED;
	fsm_close(f);
    }
}

/*
 * ncp_ackci()
 *
 * dummy routine returning true to satisfy condition in ppp.c to
 * change the state.
 */

int ncp_ackci (fsm *f, uchar *p, int len)
{
    return(TRUE);
}


/*
 * check_ncp_status
 *
 * check to see if the NCP needs to be closed or not
 */

void check_ncp_status (hwidbtype *idb)
{
    hwidbtype *master_idb;
    boolean proto_running;
    fsm *f, **nf;
    int ncpidx;

    nf = idb->ncpfsmidx;
    if (!nf) {
	return;
    }

    /*
     * I'm not happy about having to put this here, but it is the
     * easiest way at this time.  The "running" routine checks to
     * see if an upper level protocol (IP, Novell, etc.) is in use
     * on the idb.  The problem is for ISDN interfaces and for
     * dialer rotary groups, the upper level protocols only know
     * about the hunt group leader or the ISDN D channel idb.  So,
     * we need to make idb point to the hunt group leader or the
     * D channel idb before we call the "running" routine.  It's a
     * shame to have to do this here, but it is really the only place
     * it can be done.
     */

    master_idb = get_netidb(idb); /* Use the hunt group master idb */

    FOR_ALL_NCPS(ncpidx) {
	f = nf[ncpidx];
	if (!f) {
	    continue;
	}

	proto_running = (*ncptbl[ncpidx].protocheck)
			    (master_idb, idb, ncptbl[ncpidx].ncp_linktype);

	if (!proto_running) {
	    if (f->state == OPEN) {
		ncp_close(NULL, f);
	    }
	    if ((f->state == CLOSED) || (f->state == LISTEN)) {
		f->state = PPP_UNKNOWN;
	    }
	} else {
	    fsm *lcpfsm;

	    lcpfsm = idb->lcpfsm;

	    /*
	     * Normally an NCP is reactivated by the upper layer sending
	     * outgoing data to the vencapsulate function. It checks and calls
	     * ncp_activeopen if the ncp for this upper layer hasn't been
	     * negotiated. However in the case of CCP, it has no upper layer
	     * and in this respect must be handled differently from the
	     * other NCPs hence the initialisation of it here.
	     */
	    if ((f->protocol == TYPE_PPP_CCP) && (f->state == PPP_UNKNOWN)) {
		if (lcpfsm->state == OPEN) {
		    ncp_activeopen(NULL, f);
		} else {
		    f->state = CLOSED;
		}
	    }

	    /*
	     * Keeping CCP in a LISTEN state instead of a CLOSED state
	     * allows compression to be switched back on without having
	     * to bring down the link and then bring it up again.
	     */
	    if ((f->protocol == TYPE_PPP_CCP) && (f->state == CLOSED) &&
		(lcpfsm->state == OPEN)) {
		f->flags |= LOWERUP;
		f->state = LISTEN;
	    }
	}
    }
}


/*
 * show_ncp_on_idb
 *
 * show states of NCP's on a particular interface
 */
void show_ncp_on_idb (hwidbtype *idb)
{
    fsm *f;
    int state, len, indent;
    boolean first;
    int ncpidx;

    if (reg_invoke_dialer_huntgroup_leader(idb)) {
        return;
    }

    if (!idb->ncpfsmidx) {
	return;
    }

    for (state = CLOSED; state < PPP_UNKNOWN; state++) {
	first = TRUE;
	len   = 0;
	indent= 0;

	FOR_ALL_FSMS(idb->ncpfsmidx, f, ncpidx) {
	    if (f->state == state) {
		if (first) {
		    len = printf("\n  %s: %s", ppp_linkstates[state],
					       ncptbl[ncpidx].ncp_name);
		    indent = strlen(ppp_linkstates[state]) + 4;
		    first = FALSE;
		} else {
		    if (len > 70) {
			printf("\n%*c%s", indent, ' ',
			       ncptbl[ncpidx].ncp_name);
			len = indent + strlen(ncptbl[ncpidx].ncp_name);
		    } else {
			len += printf(", %s", ncptbl[ncpidx].ncp_name);
		    }
		}
	    }
	}
    }
}


/*
 * ncp_print
 * buginf out an entire ncp packet.
 */
static const char *const ncp_ip_names[] = {
    "??",			/* 0 oops */
    "Addresses-Old",		/* 1 IP Addresses (old) */
    "Compresstype",		/* 2 Compression Type */
    "Address"			/* 3 IP Address */
    };
#define MAX_IPNAMES 3

void ncp_print (paktype *pak)
{
    char protocol[100];
    const char *pname;
    char pnamestr[16];
    uchar *p;
    idbtype *idb;
    int ncpidx, proto, option_num;
    const char *const *option_names;

    idb = pak->if_input;
    if (!idb) {
	idb = pak->if_output;
    }

    p = pak->network_start;

    proto = ppp_get_protocol_type(pak);
    ncpidx = ppp_serial2ncpidx_inline(proto);
    if (ncpidx < 0) {
	sprintf(pnamestr, "NCP%4x", proto);
	pname = pnamestr;
    } else {
	pname = ncptbl[ncpidx].ncp_name;
    }

    sprintf(protocol, "PPP %s: %c %s %s(%d) id %d len %d",
	    idb ? idb->namestring : "?", pak->if_input ? 'I' : 'O', pname,
	    *p > MAX_LCPTYPES ? "UNKNOWN" : ppp_packettypes[*p],
	    *p, *(p+1), *(short *) (p+2));

    if (proto == TYPE_PPP_IPCP) {
	option_names = ncp_ip_names;
	option_num = MAX_IPNAMES;
    } else {
	option_names = NULL;
	option_num = 0;
    }

    ppp_dataprint(protocol, ((uchar*) (p))+4,
		  (*(short *) (p+2)) - 4,
		  option_names, option_num);
}

/*
 * ncp_up - FSM has reached OPEN state
 */

void ncp_up (fsm *f)
{
    uint link;

    /*
     * Gee, you would think that we could fit a one byte index into the fsm
     * since we save the idb pointer there.  :-(
     */
    link = ppp_serial2link(f->protocol);

    if (f->idb && link) {
	reg_invoke_media_protocol_up(f->idb, link);
    }
}
