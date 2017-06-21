/* $Id: rsp_srb.c,v 3.5.10.4 1996/07/25 23:53:52 motto Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_srb.c,v $
 *------------------------------------------------------------------
 * RSP specific fast switching routines for srb and rsrb
 *
 * February 1995, Bob O'Grady
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_srb.c,v $
 * Revision 3.5.10.4  1996/07/25  23:53:52  motto
 * CSCdi46834:  SRB explorer counters are incr. on CIP I/F for non CIP
 * destined expl
 * Branch: California_branch
 *
 * Revision 3.5.10.3  1996/04/17  13:48:21  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.5.10.2  1996/04/02  17:18:44  psandova
 * CSCdi53357:  RSP resorts to default explorer maxrate on reload.
 * Branch: California_branch
 *
 * The maxrate setting should not be set by the registered function.
 *
 * Revision 3.5.10.1  1996/03/18  22:02:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.4  1996/03/16  07:36:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.3  1996/03/13  02:01:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.14.2  1996/03/07  10:45:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  18:01:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/21  02:43:33  hampton
 * Clean up platform specific SRB defines in commonly included header
 * files.  [CSCdi48844]
 *
 * Revision 3.4  1995/12/13  04:23:43  ahh
 * CSCdi44230:  Remove dead code
 * Bring out your dead... *clang* (RSRB/TCP fast-switching)
 *
 * Revision 3.3  1995/11/17  18:49:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:33:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:28:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:28:46  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/09  21:33:38  ppearce
 * CSCdi38300:  Add CSNA packet switching to 75xx (RSP)
 *
 * Revision 2.3  1995/08/07  05:42:40  richl
 * CSCdi38315:  makefile rework
 *              header changes for rif_util.c and tring_inline.h
 *
 * Revision 2.2  1995/07/11  00:52:15  ogrady
 * CSCdi34908:  Process level R/SRB hangs interface (orig AT over SRB
 * fails).  Don't make fastout vector return_false to disable srb fast
 * switching since its called from srb_common() path.  Set AC byte to
 * AC_PRI4 for all srb output paths.  Don't count ignored MAC packets as
 * drops.  Ongoing SRB counter clean up.
 *
 * Revision 2.1  1995/06/07  22:58:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "../if/rif_util.h"
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "sys_registry.h"
#include "../ip/ip_registry.h"
#include "subsys.h"
#include "packet.h"
#include "config.h"
#include "../ui/debug.h"
#include "../h/ttysrv.h"
#include "../if/tring_common.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../if/channel_debug.h"
#include "../srt/trsrb.h"
#include "../if/rif_inline.h"
#include "../srt/srb.h"
#include "../srt/rsrb.h"
#include "../srt/srt_debug.h"
#include "../srt/srt_registry.h"
#include "../tbridge/tbridge_sde.h"
#include "../if/tring_inline.h"
#include "../srt/fastsrb.h"
#include "../src-rsp/rsp_bufhdr.h"
#include "../src-rsp/rsp_if.h"


/*
 * output "generic" srb packet on a trip.
 * since TRIP always regenerates FCS currently we can't use it.
 */
static int trip_srb_fastout (hwidbtype *oidb, paktype *pak)
{
    int drops;

    drops = oidb->counters.output_total_drops; /* at intr level this shouldn't change */

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
 * output "generic" srb packet on a cip.
 * 
 */
static int cip_srb_fastout (hwidbtype *oidb, paktype *pak)
{
    int drops;
    hwidbtype *vidb = oidb->output_if_hw;
    
    if (vidb) {
        pak->linktype = LINK_SRB;
        /*
	 * Add the channel encaps for a CIP internal LAN frame
	 */
        if (oidb->vencap(pak,-1)) {
            /* 
             * At interrupt level here - so the counter shouldn't change 
             * due to another packet being dropped.
             */
            drops = vidb->counters.output_total_drops;

            /*
             * Send to the CIP using the CIP virtual interface hwidb.
             */
            pak->if_output = vidb->firstsw;
            (*vidb->fastsend)(vidb, pak);

            /*
             * check if fastsend succeeded
             * kludge alert: see trip_srb_fastout comments
             */
            if (vidb->counters.output_total_drops == drops) {
                return(FS_SRB_SENT);
            } else {
                return(FS_SRB_DROPPED);
            }
        }
    } else {
        if (channel_debug_ilan) {
            buginf("\nCIP ILAN(%s) not registered with virtual interface"
                   " - cip_srb_fastout", oidb->hw_namestring);
        }
    }

    /* 
     * If packet not sent, make sure to free buffer
     */
    if (pak->buffer_header) {
        rsp_fs_free_memd_pak(pak);
    } else {
	datagram_done(pak);
    }

    return(FS_SRB_DROPPED);
}

/*
 * output "generic" srb packet on a fip.
 */
static int fip_srb_fastout (hwidbtype *oidb, paktype *pak)
{
    int drops;
    fddi_hdr *fddi;

    pak->datagramstart++;     /* chop off token ring AC byte */
    pak->datagramsize--;

    fddi = (fddi_hdr *)pak->datagramstart;
    fddi->fc = FDDI_LLC_FC_BYTE;
    ieee_swap( fddi->daddr, fddi->daddr);  /* bit swap mac addrs */
    ieee_swap( fddi->saddr, fddi->saddr);

    drops = oidb->counters.output_total_drops;        /* at intr level this shouldn't change */

    (*oidb->fastsend)(oidb, pak);

    /*
     * check if fastsend succeeded
     * kludge alert: see trip_srb_fastout comments
     */
    if (oidb->counters.output_total_drops == drops) {
        oidb->counters.tx_cumbytes_fast[ACCT_PROTO_SRB] += FDDI_FCS;
        return(FS_SRB_SENT);
    }
    return(FS_SRB_DROPPED);
}




/*
 * return trip specific srb output function pointer
 */
if_srb_fastout_type
get_srb_fastout (hwidbtype *hwidb)
{
    switch (hwidb->type) {
        case IDBTYPE_CTR:
            return(trip_srb_fastout);
	case IDBTYPE_CIP_LAN_ADAPTER:
	    return(cip_srb_fastout);
        case IDBTYPE_FDDIT:
            return(fip_srb_fastout);
        default:
            return(no_srb_fastout);
    }
}


void fs_expl_vectors (hwidbtype *idb)
{
    idb->fs_expl_func = fs_explorer_out;
}


