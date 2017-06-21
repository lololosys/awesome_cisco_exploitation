/* $Id: c7100_srb.c,v 3.1.68.3 1996/07/12 01:36:51 sthormod Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_srb.c,v $
 *------------------------------------------------------------------
 * c7100_srb.c - Predator specific fs routines for srb and rsrb
 *
 * November 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_srb.c,v $
 * Revision 3.1.68.3  1996/07/12  01:36:51  sthormod
 * CSCdi62622:  fastswitched SRB gets counted as a drop
 * Branch: California_branch
 * Fix some damage that occurred when outputdrops was moved to a subblock.
 *
 * Revision 3.1.68.2  1996/04/02  17:18:25  psandova
 * CSCdi53357:  RSP resorts to default explorer maxrate on reload.
 * Branch: California_branch
 *
 * The maxrate setting should not be set by the registered function.
 *
 * Revision 3.1.68.1  1996/03/21  23:48:15  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.5  1996/03/18  23:33:14  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.4  1996/03/10  02:50:18  sthormod
 * CSCdi51196:  add tbridge switching path for predator 4R
 * Branch: ELC_branch
 *
 * Revision 3.1.2.3  1996/03/06  19:52:29  sthormod
 * CSCdi50859:  add srb switching path for predator 4R
 * Branch: ELC_branch
 * Add case for IBDTYPE_IBM2692.
 *
 * Revision 3.1.2.2  1996/01/16  02:16:55  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.2.1  1995/11/22  21:43:26  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  01:37:49  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "types.h"
#include "sched.h"
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "sys_registry.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"

#include "../ui/debug.h"

#include "../if/rif_util.h"
#include "../if/tring_common.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../srt/trsrb.h"
#include "../if/rif_inline.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../srt/srt_debug.h"
#include "../srt/srt_registry.h"
#include "../tbridge/tbridge_sde.h"
#include "../if/tring_inline.h"
#include "../srt/fastsrb.h"

/*
 * output "generic" srb packet on a 4R port adapter.
 */
static int ibm2692_srb_fastout (hwidbtype *oidb, paktype *pak)
{
    int drops;

    drops = oidb->counters.output_total_drops; /* at intr level this
						    shouldn't change */

    (*oidb->fastsend)(oidb, pak);

    /*
     * kludge alert:
     * using the fact that ohwidb->outputdrops increments to determine
     * if fastsend succeeded.  NOT a great way to determine if it
     * did but better than ASSuming it did -
     * which the old code did for rsrb accounting.
     * fastsend() routines should be modified to return a value.
     */

    if (oidb->counters.output_total_drops == drops) {
        oidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += TR_FCS_SIZE;
        return(FS_SRB_SENT);
    }
    return(FS_SRB_DROPPED);
}

/*
 * get_srb_fastout:
 * return specific srb output function pointer
 */
if_srb_fastout_type get_srb_fastout (hwidbtype *hwidb)
{
    switch (hwidb->type) {
    case IDBTYPE_IBM2692:
	return(ibm2692_srb_fastout);
    default:
	return(no_srb_fastout);
    }
}

/*
 * fs_rsrb_tcp_output_platform:
 *
 * output an SRB packet using tcp encaps rsrb. 
 * Called via tr_vpeertype->fastlanout() vector. We can call
 * fs_rsrb_tcp_output() directly as out packet will be contigous
 * at this point.
int fs_rsrb_tcp_output_platform (hwidbtype *ihwidb,
				 tr_vpeertype *vrp,
				 tr_ventrytype *vre,
				 ushort rn,
				 paktype *pak)
{
    int status;

    status = fs_rsrb_tcp_output(ihwidb, vrp, vre, rn, pak);
    return(status);
}
 */

/*
 * fs_rsrb_tcp_input_platform:
 *
 * input a TCP/RSRB packet and output SRB pak
 * Called via reg_invoke_fs_rsrb_tcp_input();
 * NOTE: pak->network_start should be valid on entry, and since
 * our packet is contigous already, we can call fs_rsrb_tcp_input() directly.
int fs_rsrb_tcp_input_platform (hwidbtype *ihwidb,
				paktype *pak)
{
    boolean status;

    status = fs_rsrb_tcp_input(ihwidb, pak);
    return(status);
}
 */

/*
 * fs_expl_vectors:
 * Setup explorer vectors
 */
void fs_expl_vectors (hwidbtype *idb)
{
    idb->fs_expl_func = fs_explorer_out;
}

/* end of file */
