/* $Id: dnfast.c,v 3.5.12.5 1996/08/07 08:57:51 snyder Exp $
 * $Source: /release/112/cvs/Xsys/decnet/dnfast.c,v $
 *------------------------------------------------------------------
 * dnfast.c -- Generic support for DECnet fast switching support.
 *
 * 25-December-1988, Kirk Lougheed
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dnfast.c,v $
 * Revision 3.5.12.5  1996/08/07  08:57:51  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.12.4  1996/06/17  08:30:40  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.5.12.3  1996/05/09  14:12:25  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.12.2  1996/04/27  05:46:27  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.5.12.1.10.1  1996/04/27  06:34:38  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.12.1  1996/04/03  02:57:37  mbeesley
 * CSCdi53137:  Decnet Fastswitching not supported on new port adapter(s)
 * Branch: California_branch
 *
 * Revision 3.5.28.1  1996/04/08  01:45:13  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.18.1  1996/03/22  09:35:24  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5  1996/02/13  08:11:23  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.4  1995/11/27  21:32:19  rbadri
 * CSCdi44300:  DECnet fastswitching relies on all sub-interfaces
 * While enabling DECnet fastswitching on an interface, don't loop
 * through all the sub-interfaces to set the dnfast flag.
 * Revision 3.4.42.1  1996/03/05  05:49:17  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.4.36.1  1996/02/27  20:40:58  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4.12.1  1996/02/13  00:17:22  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.3  1995/11/17  08:59:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:15:00  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:16:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:53:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/10  05:32:27  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.2  1995/06/21  02:52:46  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  20:23:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "decnet.h"
#include "decnet_externs.h"
#include "access.h"
#include "../wan/dialer.h"

/*
 * External(s)
 */
extern const uchar nsptype[];


/*
 * dn_accesscheck
 * This function is a rip-off of *decnet_accesscheck*, but with performance
 * enchancements.
 */
boolean dn_accesscheck (dndatapacket *dnptr, dnaccitemtype *item)
{
    ushort source, destination, mask;
    
    /*
     * Swap the two (2) least significant bytes of the DEC addresses
     * to get the source area/node and destination area/node.
     */
    source = (dnptr->source[1] << 8);
    source |= dnptr->source[0];
    destination = (dnptr->dest[1] << 8);
    destination |= dnptr->dest[0];

    /*
     * Scan access items looking for a match -- if no match, return FALSE.
     */
    for (; item; item = item->next) {
	mask = ~item->sourcemask;
	if ((source & mask) == item->source) {
	    if (item->second) {
		mask = ~item->destmask;
		if ((destination & mask) != item->dest)
		    continue;
	    }
 	    return(item->grant);
	}
    }
    return(FALSE);
}

/*
 * dn_fastsetup
 * Turn on use of fast switching.  Logically AND configuration option
 * "idb->dn_routecache" with whether or not this interface can support
 * fast switching.  Fast switching code checks "idb->dn_fastokay".
 * Call when:
 *    - DECnet initialization.
 *    - changing serial encapsulation.
 *    - "[no] decnet route-cache" configuration command given.
 */

void dn_fastsetup (hwidbtype *idb)
{
    boolean fastflag;
    hwidbtype *tmpidb;

    fastflag = reg_invoke_fast_supported(LINK_DECNET, idb);
    if ((idb->status & (IDB_MCI | IDB_CBUS | IDB_FDDI | IDB_ATM)) ||
	(idb->type == IDBTYPE_HD64570) || (idb->type == IDBTYPE_E1) ||
	(idb->type == IDBTYPE_T1) || (idb->type == IDBTYPE_MK5025) ||
	(idb->type == IDBTYPE_LANCE) || (idb->type == IDBTYPE_QUICC_ETHER) ||
	(idb->type == IDBTYPE_AMDP2) ||
	(idb->type == IDBTYPE_S4T68360) ||
	(idb->type == IDBTYPE_FEIP) ||
        (idb->type == IDBTYPE_CD2430)) {
	if ((idb->status & IDB_ETHER) && (idb->enctype == ET_ARPA))
	    fastflag = TRUE;
	if ((idb->status & IDB_SERIAL) && (idb->enctype == ET_HDLC) &&
	          !is_ddr(idb))
	    fastflag = TRUE;
	if ((idb->status & IDB_FDDI) && (idb->enctype == ET_SNAP))
	    fastflag = TRUE;
        if (idb->status & IDB_ATM)
            fastflag = TRUE;
    }

    if ((idb->comp_db && (idb->enctype != ET_FRAME_RELAY))
      || idb->ppp_compression)
      fastflag = FALSE;

    if (!dn_running)
	fastflag = FALSE;
    idb->dn_fastokay = (fastflag && idb->dn_routecache);
    if (idb->dn_fastokay) {
	idb->fast_switch_flag |= FSF_DECNET;
	reg_invoke_register_drivers(FSF_DECNET, idb);
    } else {
	idb->fast_switch_flag &= ~FSF_DECNET;
	FOR_ALL_HWIDBS(tmpidb)
	    if (tmpidb->dn_fastokay)
		break;
	if (!tmpidb)
            reg_invoke_register_drivers(FSF_DECNET, idb);
    }
}

/*
 * dn_get_route
 * Generic function to return a pointer to a pointer to the correct
 * DECnet routing table.
 */
dnroutetype **dn_get_route (dndatapacket *dndata, dnnet *net)
{
    ulong area;
    
    area = dn_area(dndata->dest);
    if (area == net->my_dn_area) {
	return(&net->dnroutes[dn_node(dndata->dest)]);
    } else{
	if (net->doareas)
	    return(&net->dnareas[area]);
	else
	    return(&net->dnroutes[0]);
    }
}


/*
 * dn_address_translation
 * Generic function to do the Address Translation for DECnet. Re-writes
 * the DECnet source and destination addresses and returns the dnnet
 * pointer if successful.
 */
dnnet *dn_address_translation (dndatapacket *dndata, dnnet *net)
{
    ushort srcadr, dstadr;
    dnnet *newnet;
    dnmap *map, *rmap;
    
    dstadr = dndata->dest[0] | (dndata->dest[1] << 8);
    map = net->nethash[dnhash(dstadr)];
    srcadr = dndata->source[0] | (dndata->source[1] << 8);
    while (map) {
	if (map->fromaddr == dstadr) {
	    newnet = dn_nets[map->tonetn];
	    if (newnet) {
		rmap = newnet->reverse[dnhash(srcadr)];
		while (rmap) {
		    if ((rmap->toaddr == srcadr) && 
			(rmap->tonetn == net->netn)) {
			
			/*
			 * Re-write the DECnet source and destination address.
			 */
			dndata->source[0] = rmap->fromaddr;
			dndata->source[1] = (rmap->fromaddr >> 8);
			dndata->dest[0] = map->toaddr;
			dndata->dest[1] = (map->toaddr >> 8);
			map->use++;
			rmap->use++;
			
			/*
			 * Return the new pointer.
 			 */
 			return(newnet);
 		    }
 		    rmap = rmap->reverse;
 		}
 	    }
 	    return(NULL);
 	}
 	map = map->next;
    }				
    return(NULL);
}

/*
 * dn_multipath
 * Generic function to perform DECnet multipath support.
 */
dnroutetype *dn_multipath (
    dndatapacket *dndata,
    dnnet *net,
    dnroutetype *dnroute)
{
    dnnspdispkt *nsp;
    dnroutetype *nextdnroute, *returndnroute;
    ulong temp, indx;
    
    if (!net->dn_interim_mode) {
	nextdnroute = dnroute->nextsplit->nextadj;
	if (!nextdnroute || (nextdnroute->flags & (DN_RT_HELLO | DN_RT_NET)))
	    nextdnroute = dnroute;
	returndnroute = dnroute->nextsplit;
	/*
	 * If the interface we're switching to is not enabled for fast
	 * switching, return NULL here to force an immediate return
	 * to process level.  In this case we do not want to update
	 * the next route to split since we are not going to
	 * use it.
	 */
	if (!returndnroute->interface|| 
	    !returndnroute->interface->hwptr->dn_fastokay)
	    return NULL;
 	dnroute->nextsplit = nextdnroute;
 	return(returndnroute);
    } else {
	nsp = (dnnspdispkt *) &dndata->databyte;
 	if (nsptype[nsp->msgflg])
 	    temp = dn_node(dndata->dest) ^ dn_node(dndata->source);
 	else
 	    temp = dn_id(nsp->dstaddr) ^ dn_id(nsp->srcaddr);
	
 	/*
 	 * Uuuh -- *divul* while at interrupt level!!!
 	 */
 	temp = temp % dnroute->numroutes;
 	nextdnroute = dnroute;
	
 	/*
 	 * I wonder if this REALLY belongs at interrupt level!!
 	 */
 	for (indx = 0; indx < temp; indx++) {
 	    nextdnroute = nextdnroute->nextadj;
 	    if (!nextdnroute ||
		(nextdnroute->flags & (DN_RT_ROUTER | DN_RT_NET)))
 		return(dnroute);
	}
	return(nextdnroute);
    }
}
