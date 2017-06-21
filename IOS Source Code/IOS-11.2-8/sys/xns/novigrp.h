/* $Id: novigrp.h,v 3.3.10.8 1996/09/10 18:11:02 sluong Exp $
 * $Source: /release/112/cvs/Xsys/xns/novigrp.h,v $
 *------------------------------------------------------------------
 * Novell IGRP definitions
 *
 * November 1992, Joanne Boyle
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novigrp.h,v $
 * Revision 3.3.10.8  1996/09/10  18:11:02  sluong
 * CSCdi65486:  IPX EIGRP Updates not Propogated with MTU < EIGRP packet
 * Size
 * Branch: California_branch
 *
 * Revision 3.3.10.7  1996/05/03  02:19:38  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.3.10.6  1996/04/29  16:46:04  sluong
 * CSCdi55576:  Change ipx eigrp-sap-split-horizon to off
 * Branch: California_branch
 *
 * Revision 3.3.10.5  1996/04/19  19:45:30  sluong
 * CSCdi54690:  turn ipx eigrp sap split horizon on by default
 * Branch: California_branch
 *
 * Revision 3.3.10.4  1996/04/02  07:32:56  dkatz
 * CSCdi52398:  EIGRP SAP defers too much to DUAL
 * Branch: California_branch
 * Interleave SAP and DUAL packets.  While we're in there, default SAP
 * split-horizon off again;  it's still messed up.
 *
 * Revision 3.3.10.3  1996/04/02  07:23:55  dkatz
 * CSCdi51966:  EIGRP SAP split horizon causes problems
 * Branch: California_branch
 * Fix a typo that left mondo weird stuff in the backup tables.  Get rid
 * of SAP CPUHOGs.  Turn split horizon back on by default.
 *
 * Revision 3.3.10.2  1996/04/02  07:18:35  dkatz
 * CSCdi51966:  EIGRP SAP split horizon causes problems
 * Branch: California_branch
 * Default split horizon off until we get to the bottom of this.
 *
 * Revision 3.3.10.1  1996/04/02  05:38:25  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.3  1996/02/21  22:23:27  sluong
 * CSCdi49360:  IPX EIGRP : bad tic metric redistributed into RIP
 *
 * Revision 3.2  1995/11/17  19:24:06  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:39:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/07  01:46:08  mikel
 * CSCdi38859:  When serial encap changes, eigrp hold and hello times dont
 * change
 * Make sure to change the eigrp hold and hello timers to the proper NBMA
 * or non-NBMA defaults if we have a encap change.  This change only takes
 * place if have the default timers set
 *
 * Revision 2.2  1995/08/04  03:08:37  dstine
 * CSCdi37681:  Need event logging for EIGRP
 *
 * Revision 2.1  1995/06/07  23:29:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NOVIGRP_H__
#define __NOVIGRP_H__

#include "../iprouting/dual.h"
#include "novroute.h"

/*
 * Default setting for EIGRP SAP split horizon
 */
#define IPX_EIGRP_SAP_SPLIT_HORIZON_DEFAULT (FALSE) 

/*
 * Number of DUAL packets to defer to before sending a SAP packet
 */
#define IPX_EIGRP_SAP_DEFER_COUNT 1	/* Send one route for each SAP */

/*
 * Constant definitions.
 */
#define NOVIGRP_PROT      0		/* Unknown packet type */

#define NOVIGRP_REQUEST   0x0301
#define NOVIGRP_METRIC    0x0302
#define NOVIGRP_EXTERNAL  0x0303

/*
 * External TLV value definitions.
 */
enum NOVIGRP_PROT_ID {
    NULL_PROTID,
    IGRP_PROTID,
    STATIC_PROTID,
    RIP_PROTID,
    CONNECTED_PROTID,
    ISIS_PROTID,          /* %%% New ... check out usage */
    NLSP_PROTID,          /* %%% New ... check out usage */
    INTERNAL_PROTID       /* %%% New ... check out usage */
};


/*
 * Used for decoding multiple destinations in an novigrp_metrictype
 */
#define NOVIGRPEXTPAKMACRO \
    uchar routerid[IEEEBYTES]; /* Router ID of injecting router */        \
    ulong PACKED(asystem);	/* Originating autonomous system number */ \
    ulong PACKED(tag);			/* Arbitrary tag */                \
    uchar protocol;			/* External prot */		   \
    uchar flag;				/* Internal external flag */	   \
    ushort PACKED(ext_metric);  	/* External metric  */             \
    ushort PACKED(ext_delay);		/* External delay */

typedef struct novigrp_metrictype_ {
    IGRP2TLMACRO
    ulong PACKED(nexthop_net);
    uchar nexthop_host[IEEEBYTES];
    IGRP2METRICPAKMACRO
    ulong PACKED(dest);   /* At least one of these per tlv */
} novigrp_metrictype;

typedef struct novigrp_externtype_ {
    IGRP2TLMACRO
    ulong PACKED(nexthop_net);
    uchar nexthop_host[IEEEBYTES];
    NOVIGRPEXTPAKMACRO
    IGRP2METRICPAKMACRO
    ulong PACKED(dest);   /* At least one of these per tlv */
} novigrp_externtype;

typedef struct novigrp_extdatatype_ {
    NOVIGRPEXTPAKMACRO
} novigrp_extdatatype;

typedef struct novigrp_worktype_ {
    struct novigrp_worktype_ *next;
    igrp2_eventtype type;
    union c_ {
	struct {
	    ulong net;
	    int event;
	} red;
	struct {
	    int pdbindex;
	    boolean sense;
	} rea;
	struct {
	    ulong net;
	} d;
	struct {
	    idbtype *interface;
	    ulong net;
	    boolean sense;
	} ifc;
	struct {
	    idbtype *interface;
	} ifd;
	struct {
	    idbtype *idb;
	} send_sap;
    } c;
} novigrp_worktype;

/* Queue entry for novigrp_sapQ */

typedef struct novigrp_sapq_entry_ {
    struct novigrp_sapq_entry_ *next;	/* Next in queue */
    igrp2type *sappak;			/* SAP packet to send */
    ulong length;			/* Length of packet */
    enum IPX_DESTINATION destination_type; /* Who to send it to */
    igrp2addrtype dest;			/* Destination address */
} novigrp_sapq_entry;

/* Slag these in 2K chunks. */

#define NOVIGRP_SAPQ_CHUNK_COUNT (2000 / sizeof(novigrp_sapq_entry))


/* Return TRUE if doing incremental SAP udpates on this nidb */
#define NOVIGRP_DOING_INCREMENTAL_SAP(n) \
  (((n)->igrp_sap_updates) && reg_invoke_novigrp_have_peer((n)))

/*
 * Externs for functions in novigrp.c
 */
extern void novigrp_process(paktype *);
extern void free_novpdbindex (novpdbtype *);
extern boolean novigrp_send_sap(paktype *, enum IPX_DESTINATION, nidbtype *);
extern void novigrp_network_command(novpdbtype *, ulong, boolean);
extern void novigrp_route_adjust(novpdbtype *, idbtype *);
extern void novigrp_mtu_change(hwidbtype *, idbtype *, int);
extern void nov_set_ndb_flags(novndbtype *, novpdbtype *, boolean);
extern void nov_router_erase(novpdbtype *);
extern void novigrp_if_commands(parseinfo *);
extern void novigrp_address_command(idbtype *, ulong, boolean);
extern boolean novigrp_have_peer(nidbtype *);
extern void novigrp_sap_access_list_work(ulong, idbtype *);
extern void novigrp_sap_access_list_change(ulong);
extern void novigrp_rt_access_list_change(ulong);
extern void novigrp_test_command(parseinfo *);
extern void novigrp_clear_neighbors(parseinfo *);
extern void novigrp_timers(parseinfo *);
extern void novigrp_event_logging(parseinfo *);
extern void novigrp_take_neighbors_down(novpdbtype *, char *);
extern void novigrp_encap_fixup(idbtype *, int);
extern novpdbtype *novigrp_start(ushort);
extern void novigrp_reload_proto_table(novpdbtype *pdb);
extern void nov_redistribute (novpdbtype *, novpdbtype *, boolean);

#define NOVIGRP_DTARGET(target,dd) \
	((novelligrp_debug || novelligrp_event_debug)  ? \
	(dd->debug_target_set ? \
	target == dd->debug_target.nov_network : TRUE) : FALSE)

#define NOVIGRP_DETARGET(target,dd) \
	(novelligrp_event_debug ? \
	(dd->debug_target_set ? \
	target == dd->debug_target.nov_network : TRUE) : FALSE)

/*
 * find_novigrp_pdb
 * Given an IPX packet, return the IGRP pdb or NULL
 */
static inline novpdbtype *find_novigrp_pdb (int asystem)
{
    novpdbtype *pdb;

    for (pdb = (novpdbtype *) novpdbQ.qhead; pdb; pdb = pdb->next) {
	if ((pdb->proctype == NET_IGRP) && (asystem == pdb->asystem))
	    return(pdb);
    }
    return(NULL);
}


/*
 * ipx_igrp2_take_nbrs_down
 * Wrapper function used to invoke igrp2_take_nbrs_down from novell.c
 * A direct call to this function 'igrp2_take_nbrs_down' from novell.c fail
 * when the ipx_eigrp subsystem is not included. Providing it as a service
 * helps fix the problem.
 */
static inline void ipx_igrp2_take_nbrs_down (ddbtype *ddb, idbtype *idb, 
			       boolean destroy_now, char *reason)	
{
    igrp2_take_nbrs_down(ddb,idb,destroy_now, reason);
}	


/*
 * ipx_igrp2_exists_iidb
 * Tests whether igrp2_find_iidb returns a non-NULL pointer.
 * A direct call to this function 'igrp2_find_iidb' from novell.c 
 * will fail when the ipx_eigrp subsystem is not included. Providing it 
 * as a service helps fix the problem.
 */

static inline boolean ipx_igrp2_exists_iidb (ddbtype *ddb, idbtype *idb)
{	
    return (igrp2_find_iidb(ddb,idb) != NULL); 
}


#endif __NOVIGRP_H__
