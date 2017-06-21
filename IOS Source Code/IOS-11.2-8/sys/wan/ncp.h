/* $Id: ncp.h,v 3.3.22.6 1996/08/03 23:42:01 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/ncp.h,v $
 *------------------------------------------------------------------
 * ncp.h - generic control protcol defs.
 *
 * 05-Feb 1992, Sanjeev Newarikar
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncp.h,v $
 * Revision 3.3.22.6  1996/08/03  23:42:01  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.3.22.5  1996/07/01  20:47:21  fox
 * CSCdi61571:  Local Timeout at _ppp_getlink(0x303d730+0x1fff82)+0x160
 * Branch: California_branch
 * Virtual PPP interfaces may not be complete.  Sanity check existence
 * of NCP FSM pointer table.
 *
 * Revision 3.3.22.4  1996/06/28  20:09:07  fox
 * CSCdi61488:  Interface input queue reaches max and PPP fails
 * Branch: California_branch
 * ncp_input() handling of unsupported protocols was broken by CSCdi59550.
 *
 * Revision 3.3.22.3  1996/06/24  09:27:58  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.3.22.2  1996/05/10  01:45:12  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.3.22.1  1996/04/29  09:46:03  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.3  1996/01/23  18:22:38  fox
 * CSCdi47345:  Unused functions are unneeded functions
 * ncp_closed is never referenced
 * seen_addr and seen_pool are redundant
 *
 * Revision 3.2  1995/11/17  18:03:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:36:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  23:18:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__NCP_H__
#define __NCP_H__

typedef struct {
    char ncp_linktype;
    ushort ncp_protocol;
    void (*ncp_init)(fsm *);
    boolean (*protocheck)(hwidbtype *, hwidbtype *, int);
    const char *ncp_name;
} ncpconst;

#define MAX_NCP 32	/* change this value to add new protocols over ppp */

extern ncpconst ncptbl[MAX_NCP];

extern uchar link2ncpidxtbl[LINK_MAXLINKTYPE];

extern int max_ncp;	/* count of registered NCPs */

/*
 * The following kludge is designed to hide LINK_MULTILINK as a pseudo
 * NCP at the front of the NCP index and fsm pointer tables.  In order
 * to do so, we have to get several things right:
 *  a) we must add Multilink as the first entry before any real NCP
 *  b) we must always start 'FOR_ALL_' loops with the first real NCP
 *  c) we must handle several exception cases:
 *    1) fsm must be created by hand
 *    2) fsm state and other variables must be set by hand
 *    3) fsm must be removed by hand
 */
extern int first_ncp;	/* index of first real NCP in in NCP tables */

#define FOR_ALL_NCPS(ncpidx) \
    for (ncpidx = first_ncp; ncpidx < max_ncp; ncpidx++)

#define FOR_ALL_FSMS(nf, f, idx) \
    if ((nf)) \
	for (idx = first_ncp, (f) = (nf)[idx]; \
	     idx < max_ncp; \
	     idx++, (f) = (nf)[idx]) \
	    if (f)

/*
 * The quick lookup table is designed to map the most frequent PPP packet
 * types to the ncp index.  This provides a very fast lookup that can be
 * dynamically extended with minimal code changes.  Protocols not found in
 * the quick lookup table can be found by scanning the ncp table.
 *
 * Note that the table maps the protocol types less than or equal to 0xff
 * but since PPP protocols are odd numbers, we can compress the table in
 * half.  We derive the lookup value by shifting the protocol type right
 * one bit (or dividing by two).
 */
#define PPP_QUICK_LOOKUP_MAX 0x100
#define PPP_QUICK_LOOKUP_SIZE (PPP_QUICK_LOOKUP_MAX/2)
extern uchar ppp_quick_lookup[PPP_QUICK_LOOKUP_SIZE];

/*
 * NCPs (Network Control Protocols) usually start at 0x8000 and usually
 * are the network protocol number OR'd with 0x8000.
 */
#define NCP_PROTOCOL_BASE 0x8000
#define NCP_PROTOCOL_MASK 0x7fff


/*
 * ppp_ncpidx2fsm_inline
 * given an index into the fsm ptr list and an IDB ptr, return a pointer
 * to the fsm table.  Legal index values are 0 to (max_ncp - 1).
 */

static inline fsm *ppp_ncpidx2fsm_inline (int index, hwidbtype *idb)
{
    if (((uint)index >= max_ncp) || !idb->ncpfsmidx) {
	return(NULL);
    } else {
	return(idb->ncpfsmidx[index]);
    }
}

/*
 * ppp_link2fsm_inline
 * given a link type get ncp fsm pointer
 */

static inline fsm *ppp_link2fsm_inline (long link, hwidbtype *idb)
{
    int ncpidx;

    if ((ulong)link >= LINK_MAXLINKTYPE) {
	return(NULL);
    }
    /*
     * Load index into fsm pointers and ncp table.  This index is dynamic
     * based on initialization order of the network subsystems.
     */
    ncpidx = link2ncpidxtbl[link];

    if (!ncpidx || !idb->ncpfsmidx) {
	return(NULL);
    } else {
	return(idb->ncpfsmidx[ncpidx - 1]);
    }
}

/*
 * ppp_link2serial_inline
 * given a link type, return a serial type code
 */

static inline int ppp_link2serial_inline (long link, hwidbtype *idb)
{
    fsm *f;
    int type;

    /*
     * Look up the fsm struct for this protocol on this interface
     */
    f = ppp_link2fsm_inline(link, idb);

    /*
     * Create data type from the NCP protocol.  This works for most cases.
     * The exceptions are below.  We have to check for the existence of the
     * fsm here AND below because some of the exceptions don't care whether
     * the fsm exists or not. :-(
     */
    if (f) {
	type = f->protocol & NCP_PROTOCOL_MASK;
    } else {
	type = 0;
    }

    /*
     * Special case handling because not everything plays by the rules.
     */
    switch (link) {

    case LINK_COMPRESSED_TCP:
	type = TYPE_PPP_VJCOMP;
	break;

    case LINK_UNCOMPRESSED_TCP:
	type = TYPE_PPP_VJUNCOMP;
	break;
		  
    case LINK_DEC_SPANNING:
    case LINK_IEEE_SPANNING:
	type = TYPE_PPP_SPANNING;
	break;
  		  
    case LINK_BRIDGE:
        /*
         * If LAT is enabled, regardless of whether bridging is enabled 
         * on the interface, return the bridge type anyway. This allows
	 * bridge-encapsulated LAT traffic between two end points of a
	 * PPP link (without actually having BRIDGECP negotiated).
         */
        if (idb->firstsw->lat_running) {
            return(TYPE_PPP_BRIDGE);
	}
	break;

    default:
	break;

    }

    /*
     * Special cases are done.  Return the computed type if the appropriate
     * fsm exists and its state is OPEN.
     */
    if (f && (f->state == OPEN)) {
	return(type);
    } else {
	return(0);
    }
}

/*
 * ppp_serial2link_inline
 * given a PPP protocol, return an appropriate link type
 */

static inline int ppp_serial2link_inline (long type)
{
    /*
     * Let's assume for the moment that all of the drivers have Done the
     * Right Thing and sanity checked the format of the protocol so that
     * the protocol is an odd number.
     */
    if ((type & NCP_PROTOCOL_MASK) < PPP_QUICK_LOOKUP_MAX) {
	/*
	 * High frequency data protocols are supposed to have a protocol
	 * type that is less than 0x100 so that it can be easily compressed.
	 * Their matching NCP is usually the same value OR'd with 0x8000.
	 * So use this information to do a quick lookup of the link type
	 * in the most common cases.  Note that unregistered protocols
	 * will return a zero, which is LINK_ILLEGAL.  Note that we compress
	 * the lookup table because all PPP protocols are supposed to be
	 * odd.
	 */
	return(ppp_quick_lookup[(type & NCP_PROTOCOL_MASK) >> 1]);
    } else {
	int link;
	/*
	 * High frequency data packets are supposed to have single byte
	 * protocol numbers (less than 0x100).  We have done those and the
	 * equivalent NCPs.  What is left are low-frequency protocols and
	 * LCP and its extensions.  Putting these into a switch statement
	 * will be as fast as other lookup methods at this point but will
	 * require that new protocols (greater than 0x100) will have to be
	 * added here and elsewhere.  :-(  C'est la vie.
	 */
	switch (type) {

	case TYPE_PPP_LCP:
	case TYPE_PPP_UPAP:
	case TYPE_PPP_LQR:
	case TYPE_PPP_CHAP:
	    return(LINK_PPP);

	case TYPE_PPP_SPANNING:
	    link = LINK_IEEE_SPANNING;
	    break;

	case TYPE_PPP_CDP:
	case TYPE_PPP_CDPCP:
	    link = LINK_CDP;
	    break;

	default:
	    return(LINK_ILLEGAL);
	}

	/*
	 * Sure we have a link type, but is it a registered link type?
	 * We do this because we want to be consistent (ppp_quick_lookup[]
	 * of an unregistered protocol will return LINK_ILLEGAL).
	 */
	if (link2ncpidxtbl[link]) {
	    return(link);
	} else {
	    return(LINK_ILLEGAL);
	}
    }
}
/*
 * ppp_serial2ncpidx_inline
 * given a PPP protocol, return an index into the NCP and fsm pointer tables
 */

static inline int ppp_serial2ncpidx_inline (long type)
{
    return(link2ncpidxtbl[ppp_serial2link_inline(type)] - 1);
}

/*
 * ppp_serial2fsm_inline
 * given a PPP protocol and an IDB ptr, return a pointer to the fsm table
 */

static inline fsm *ppp_serial2fsm_inline (long type, hwidbtype *idb)
{
    return(ppp_ncpidx2fsm_inline(ppp_serial2ncpidx_inline(type), idb));
}

/*if NULL is specified then ncp_init,input or protrej will take
 *appropriate actions otherwise will invoke the func to do special
 *processing if any. If init is not specified then init to defaults
 */

typedef struct ncp_options {
    int junk;                 /* dummy field tested ALWAYS to 0 */
} ncp_options;

extern void ncp_print(paktype *);
extern void ncp_register(int, ushort, void (*)(fsm *),
			 boolean (*)(hwidbtype *, hwidbtype *, int),
			 const char *);
extern void ncp_register_link(int, ushort, int);
extern void ncp_enable(hwidbtype *, int);
extern void ncp_disable(hwidbtype *, int);
extern void ncp_init(hwidbtype *);
extern void ncp_input(paktype *, hwidbtype *);
extern void ncp_protrej(hwidbtype *, ushort);
extern void ncp_lowerup(fsm **);
extern void ncp_lowerdown(fsm **);
extern void ncp_close(fsm **, fsm *);
extern void ncp_activeopen(fsm **, fsm *);
extern void check_ncp_status(hwidbtype *);
extern void show_ncp_on_idb(hwidbtype *);
extern int ncp_ackci(fsm *, u_char *, int);
extern void ncp_up(fsm *);

#endif	__NCP_H__
