/* $Id: clnsfast.c,v 3.4.12.5 1996/06/17 08:30:29 jhernand Exp $
 * $Source: /release/112/cvs/Xsys/clns/clnsfast.c,v $
 *------------------------------------------------------------------
 * clnsfast.c -- Device independent CLNS Fast Switching support
 *
 * 12-Aug-1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clnsfast.c,v $
 * Revision 3.4.12.5  1996/06/17  08:30:29  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.4.12.4  1996/05/04  20:04:10  asastry
 * CSCdi56559:  CLNS cache invalidations are too frequent, causing high
 * CPU utilization.
 * Branch: California_branch
 *
 * Revision 3.4.12.3  1996/04/27  05:45:53  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.20.3  1996/04/26  00:06:42  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.3.20.2  1996/04/17  00:38:18  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.3.20.1  1996/02/23  20:12:13  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.4.12.2  1996/04/03  03:04:12  mbeesley
 * CSCdi53161:  CLNS fast switching not supported on new PA(s).
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/03/18  19:03:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  01:13:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  08:40:25  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  00:21:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  08:11:14  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.3  1995/11/17  08:52:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:12:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:08:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:52:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/07/10  05:32:13  fred
 * CSCdi36850:  commit fast path queuing for appletalk, decnet, etc.
 *
 * Revision 2.2  1995/06/21  02:52:22  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  20:17:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Description of Routines Exported from this module. 
 *
 * clns_fastsetup: enables or disables an interface for CLNS fastswitching.
 *
 */

#include "master.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "access.h"
#include "../h/address.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "config.h"
#include "../clns/clns_externs.h"
#include "../wan/dialer.h"


/* clns_fastsetup
 * Turn on use of fast switching.  Logically AND configuration option
 * "idb->clns_routecache" with whether or not this interface can support
 * fast switching. If fastswitching is allowed, set  "idb->clns_fastokay" to
 * TRUE and the appropriate bit in the "fast_switch_flag".
 */

void clns_fastsetup (hwidbtype *idb)
{
    hwidbtype *tmpidb;
    boolean fastflag;
    
    fastflag = reg_invoke_fast_supported(LINK_CLNS, idb);
    if ((idb->status & (IDB_MCI | IDB_CBUS)) ||
	(idb->type == IDBTYPE_HD64570) || (idb->type == IDBTYPE_E1) ||
	(idb->type == IDBTYPE_T1) || (idb->type == IDBTYPE_BSI) ||
	(idb->type == IDBTYPE_MK5025) || (idb->type == IDBTYPE_LANCE) ||
	(idb->type == IDBTYPE_QUICC_ETHER) ||
	(idb->type == IDBTYPE_AMDP2) ||
	(idb->type == IDBTYPE_S4T68360) ||
	(idb->type == IDBTYPE_FEIP) ||
	(idb->type == IDBTYPE_FDDIT) ||
	(idb->type == IDBTYPE_CD2430)) {
	if ((idb->status & IDB_ETHER) && ((idb->enctype == ET_ARPA)))
	    fastflag = TRUE;
	else if ((idb->status & IDB_SERIAL) && (idb->enctype == ET_HDLC) &&
		 !is_ddr(idb))
	    fastflag = TRUE;
	else if (idb->status & IDB_FDDI)
	    fastflag = TRUE;
    }

    if (is_atm(idb)) {
	fastflag = TRUE;
    }
    if (!router_enable 
	|| (idb->comp_db && (idb->enctype != ET_FRAME_RELAY))
	|| idb->ppp_compression)
	fastflag = FALSE;
    idb->clns_fastokay = (fastflag && idb->clns_routecache && clns_running);
    if (idb->clns_fastokay) {
	idb->fast_switch_flag |= FSF_CLNS;
	reg_invoke_register_drivers(FSF_CLNS, idb);
    } else {
	idb->fast_switch_flag &= ~FSF_CLNS;
	FOR_ALL_HWIDBS(tmpidb)
	    if (tmpidb->clns_fastokay)
		break;
	if (!tmpidb)
	    reg_invoke_register_drivers(FSF_CLNS, idb);
    }
    clns_invalidate_cache("clns_fastsetup", TRUE);
} 					/* end of clns_fastsetup */

/* end of clnsfast.c */
