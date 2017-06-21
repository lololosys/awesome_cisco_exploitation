/* $Id: if_trip.c,v 3.10.4.13 1996/09/10 19:30:48 cmurphy Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_trip.c,v $
 *------------------------------------------------------------------
 * if_trip.c -- driver for the cBus Token Ring Interface Card
 *
 * August 1991 - David R. Hampton
 * 
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_trip.c,v $
 * Revision 3.10.4.13  1996/09/10  19:30:48  cmurphy
 * CSCdi63700:  Token-ring int inserts into ring with bad
 * ring#, no warning.  Spyglass parameter structure is being
 * padded for alignment on RSP, causing us to read the
 * wrong values.  Fixed the alignment problem.
 *
 * Branch: California_branch
 *
 * Revision 3.10.4.12  1996/09/07  02:07:44  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.10.4.11  1996/08/28  12:45:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.4.10  1996/08/26  15:07:24  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.10.4.9  1996/08/07  08:59:11  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.10.4.8  1996/07/16  09:36:24  hampton
 * Centralize knowledge of Decnet/Token Ring interaction.  [CSCdi63066]
 * Branch: California_branch
 *
 * Revision 3.10.4.7  1996/07/09  05:51:26  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.10.4.6  1996/06/01  06:15:24  hampton
 * Remove obsolete media_tring_supports_etr registry call.  [CSCdi59254]
 * Branch: California_branch
 *
 * Revision 3.10.4.5  1996/06/01  01:31:24  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.10.4.4  1996/05/29  03:46:06  skufer
 * CSCdi55758:  800E on TRIP steals MEMD from global queue, tql > tx queue
 * limit
 * Branch: California_branch
 *         TRIP on C7000 may advance tql when noisy ring causes
 *         frequent beacons.  Result is performance degradation.  When
 *         resetting the trip, new code prevents advancing the tql.  It
 *         is only a problem on c7000, not c7500 series.
 *
 * Revision 3.10.4.3  1996/05/17  11:18:43  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.14.3  1996/04/26  15:19:02  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.14.2  1996/04/03  14:30:44  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.14.1  1996/03/17  17:49:31  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.10.4.2  1996/05/09  14:29:02  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.10.4.1.18.1  1996/04/27  07:02:33  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.10.4.1.8.1  1996/04/08  01:46:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.10.4.1  1996/03/18  19:42:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.10.8.1  1996/03/22  09:36:38  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.10  1996/03/11  09:55:13  ogrady
 * CSCdi50668:  TRIP and FIP address filter should disable protocol bits
 *              for SRB.  Fix some AF bit masks shift values which are not
 *              the same for TRIP and FIP.  Disable IP bit only on packets
 *              with rif instead of for multiring configuration.
 *
 * Revision 3.9  1996/03/07  11:05:08  dkerr
 * CSCdi50623:  IP flow cache needs better accounting granularity
 * Don't allow optimum/flowswitching on tokenring or fddi when
 * an SRB RIF is present; otherwise it screws up cache invalidation.
 *
 * Revision 3.8.18.1  1996/03/05  06:11:34  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.8.12.1  1996/02/27  20:42:15  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.8  1996/01/18  22:47:27  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.7  1996/01/11  03:50:27  kao
 * CSCdi43878:  IS_VIP() macro should be used
 *
 * Revision 3.6.2.1  1996/02/15  19:13:16  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.6  1995/12/19  00:40:26  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.5  1995/11/29  02:47:52  sthormod
 * CSCdi42503:  4r needs modularity work
 * Clean up #ifdef C7000 kludge.
 *
 * Revision 3.4  1995/11/20  22:24:25  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  09:18:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:41:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:01:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/22  10:20:00  richl
 * CSCdi39768:  SUPNONE for SRT registry
 *
 * Revision 2.5  1995/08/30  18:02:49  ogrady
 * CSCdi39559:  Token ring tbridging loses broadcast packets.  Change
 * token ring interrupt handler so that broadcast packets are fast
 * tbridged when its enabled.
 *
 * Revision 2.4  1995/08/07  05:34:12  richl
 * CSCdi38315:  makefile rework
 *              move code to tring_inline and to rif_util in libutil.a
 *
 * Revision 2.3  1995/07/17  08:21:18  schaefer
 * CSCdi36181: more robust microcode management (on RSP)
 *
 * Revision 2.2  1995/07/05  20:48:37  hampton
 * Consolidate common token ring variables into the idb, instead of having
 * them in the private data structure for each type of token ring
 * interface.  Remove overloading of the delay variable by token rings.
 * Eliminate direct references to the system clock.  [CSCdi36721]
 *
 * Revision 2.1  1995/06/07 20:42:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../if/msg_tr.c"		/* Not a typo, see logger.h */
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "../srt/srt_registry.h"
#include "../hes/cbus_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../if/tring.h"
#include "../if/tring_private.h"
#include "../if/rif_inline.h"
#include "../if/tring_common.h"
#include "../if/tring_debug.h"
#include "../if/rif.h"
#include "../if/network.h"
#include "if_hes_common.h"
#include "if_fci.h"
#include "if_mci.h"
#include "if_cmd.h"
#include "if_trip.h"
#include "if_trip_inline.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "../if/linkdown_event.h"
#include "../smf/smf_public.h"
#include "../tbridge/tbridge_debug.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"


static char *const spyg_ramtest_errors[] = {
    "success",
    "Invalid Address Requested",
    "Addressing Error occurred",
    "Parity Error occurred",
    };

static char *const spyg_init_errors[] = {
    "Parity Error on Diagnostic Initialization",
    "0x21",
    "ROS Test Failed",
    "0x23",
    "RAM Test Failed",
    "0x25",
    "TRC uP Instruction Test Failed",
    "0x27",
    "TRC uP Interrupt Test Failed",
    "0x29",
    "0x2A",
    "0x2B",
    "Protocol Handler Test Failed"
};

static const char *const spyg_cmd_errors[] = {
    "Success",
    "Invalid Command",
    "Not Running",
    "Invalid when Closed",
    "Invalid when Open",
    "5",
    "6",
    "Open Failed",
    "Invalid MAC Address",
    "Invalid Group Address",
    "Invalid Bridge Number",
    "Invalid Target Ring Number",
    "Invalid Source Ring Number",

};

static ushort ctr_memb_read (hwidbtype *idb)
{
    ushort sresult, sdata;

    sresult = send_trip_memb_cont_read_cmd(idb, &sdata);
    if (sresult != FCI_RSP_OKAY) {
        errmsg(&msgsym(CTRCMDFAILED2, CBUS), idb->hw_namestring,
               "FCI_CTR_MEMB_C_READ", sresult);
	return(-1);
    }
    return(sdata);
}


/**********************************************************************
 *
 *		       Address Filter routines
 *
 * All of these routines must be called with interrupts disabled.
 *
 **********************************************************************/

static void cbus_token_address (hwidbtype *idb, ushort data, uchar const *ptr)
{
    int i;
    
    /*
     * Check for multicasts:
     */
    if (*ptr & 0x80)			/* check for multicast */
	idb->multibits |= (data<<8);	/* multicast mask for trip status */

    for (i = 2; i < IEEEBYTES+2; i++, ptr++) {
	cbus_af_or(idb, (i << 8) | *ptr, data);
    }
}

static void cbus_token_func_address (hwidbtype *hwidb, ushort data)
{
    int i, j, index;
    ulong functional;
    uchar *funcp;
    tokensb *tsb;

    tsb = idb_get_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
    
    /*
     * C000.xxxx.yyyy where the high order bit of xxxx is zero
     */
    cbus_af_or(hwidb, (2 << 8) | 0xC0, data);
    cbus_af_or(hwidb, (3 << 8) | 0x00, data);
    /*
     * Out of the 31 possible functional addresses, determine which
     * are set, and revise pages 4-7 of the address filter
     * (low order 4 bytes of dst MAC addr) to indicate which should be set,
     * and which should be clear, for the given FSM value.
     * Note that each of these 31 functional addresses are exclusive of
     * one another, so the address filter does not need to provide for
     * combinations of the bits.  Address filter winds up doing some unwanted
     * aliasing between bytes when multiple functional addresses in use...
     */
    functional = (tsb->tr_functional & ~(TR_FAI << 24));
    funcp = (uchar *)&functional;
    for (i = 0; i < 4; i++) {
	index = 256 * (4 + i);
	for (j = 1; j < 256; j *= 2) {
	    if (funcp[i] & j)
		cbus_af_or(hwidb, index + j, data);
	    else
		cbus_af_and(hwidb, index + j, ~data);
	}
	/*
	 * Figure out which bytes of MAC address may have value 0 when
	 * matching against a functional address.
	 */
	if (functional & ~(0xff000000 >> (8*i)))
	    cbus_af_or(hwidb, index, data);
	else
	    cbus_af_and(hwidb, index, ~data);
    }
}

static void cbus_token_group_address (hwidbtype *hwidb, ushort data)
{
    tokensb *tsb;

    tsb = idb_get_hwsb_inline(hwidb, HWIDB_SB_TOKEN);

    /*
     * C000.xxxx.yyyy where the high order bit of xxxx is non-zero
     */
    cbus_af_or(hwidb, (2 << 8) | 0xC0, data);
    cbus_af_or(hwidb, (3 << 8) | 0x00, data);
    cbus_af_or(hwidb, (4 << 8) | ((tsb->tr_group >> 24) & 0xff), data);
    cbus_af_or(hwidb, (5 << 8) | ((tsb->tr_group >> 16) & 0xff), data);
    cbus_af_or(hwidb, (6 << 8) | ((tsb->tr_group >>  8) & 0xff), data);
    cbus_af_or(hwidb, (7 << 8) | ((tsb->tr_group >>  0) & 0xff), data);
}

/*
 * cbus_token_ext_address
 * Handle the extended address filter for CTR style interfaces.
 */
static void cbus_token_ext_address (hwidbtype *idb)
{
    boolean bridging;
    boolean srb_bridging;
    ushort data, mask, smask, rif_mask, norif_mask;
    int indx;
    idbtype *swidb;
    srb_vector_table_t *srbV_p;
    

    /*
     * This code assumes that the bytes 2-7 of the address filter
     * has aleady been set up. Setup octets 0-1 and 8-31.
     */
    mask = 0;
    bridging = FALSE;
    srb_bridging = FALSE;

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srb_bridging = srbV_p->srb_sb_get_bridging_enabled(idb->firstsw);
    }
    if (bridge_enable && (idb->tbridge_on_hwidb || srb_bridging)) {
	bridging = TRUE;
	mask = FCI_BRIDGING_MASK;  	/* bridging mask */
	
	/*
	 * Notify the cBus controller of the bridging bit.
	 */
	(void)send_bridge_cmd(idb, (mask << 8));
    }
    mask |= (FCI_STATION_MASK | FCI_MULTICAST_MASK | FCI_MAC_MASK);
    smask = mask;

    /*
     * Set up the protocol portion of the address mask.
     * Do NOT program the AF in this way if CRB is enabled
     * and the interface is configured for transparent
     * bridging.
     */
    if (!(global_crb_enable && idb->tbridge_on_hwidb)) {
	FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	    if (ROUTING_IF(swidb, LINK_IP))
		mask |= FCI_DOD_IP_PROTOCOL_MASK;
	    if (ROUTING_IF(swidb, LINK_DECNET))
		mask |= FCI_DECNET_PROTOCOL_MASK;
	    if (ROUTING_IF(swidb, LINK_CLNS))
		mask |= FCI_CLNS_PROTOCOL_MASK;
	    if (ROUTING_IF(swidb, LINK_XNS))
		mask |= FCI_XNS_PROTOCOL_MASK;
	}
    }

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s MCI_CMD_AF_WRITE %04x", idb->hw_namestring, mask);
#endif  /* TBRIDGE_DEBUG */
    
    /*
     * Proceed to write the address filter.
     */

    /*
     * Any AC byte.
     */
    for (indx = 0; indx < 256; indx++) {
	cbus_af_write(idb, indx, mask);
    }

    /*
     * Only FC bytes having Type MAC or LLC.
     */
    for (indx = (256 * 1); indx < (256 * 1) + 0x40; indx++) {
	cbus_af_write(idb, indx, mask);
    }
    for (indx = (256 * 1) + 0x40; indx < (256 * 1) + 0x80; indx++) {
	cbus_af_write(idb, indx, (mask & ~FCI_MAC_MASK));
    }
    
    /*
     * Do special things for the Destination Address
     */
    for (indx = (256 * 2); indx < (256 * 8); indx++) {
	data = cbus_af_read(idb, indx);
	if (data & FCI_STATION_MASK) {
	    if (data & FCI_MULTICAST_MASK)
		data |= mask;
	    else
		data |= (mask & (~FCI_MULTICAST_MASK));
	} else if (bridging)
	    data |= FCI_BRIDGING_MASK;
	cbus_af_write(idb, indx, data);
    }
    
    /*
     * Set SA with mask
     *
     * if we are doing srb accept frames with RII bit set in src addr
     * if we are doing tbridge accept frames without RII bit set
     */
    if (srb_bridging) {
        rif_mask = mask;
    } else {
        rif_mask = (mask & ~(FCI_BRIDGING_MASK));
    }
    /*
     * Don't set IP bit if there's a RIF!
     */
    rif_mask &= ~FCI_DOD_IP_PROTOCOL_MASK;

    if (idb->tbridge_on_hwidb) {
        norif_mask = mask;
    } else {
        norif_mask = (mask & ~(FCI_BRIDGING_MASK));
    }

    for (indx = (256 * 8); indx < (256 * 9); indx++) {
        if (indx & TR_RII) {
            cbus_af_write(idb, indx, rif_mask);
        } else {
            cbus_af_write(idb, indx, norif_mask);
        }
    }
    for (indx = (256 * 9); indx < (256 * 14); indx++) {
	cbus_af_write(idb, indx, mask);
    }

    /*
     * 802.2 header.  Routing protocol bits get set differently for each
     * protocol.  Remember that the low order bit is the command/response
     * or group/individual bit, so we write everything in pairs.  Make
     * sure that station, broadcast, and bridging bits are set for all
     * bytes.
     */
    for (indx = (256 * 14); indx < (256 * 16); indx += 2) {
	data = smask;
	if ((indx == (256 * 14) + SAP_CLNS) || (indx == (256 * 15) + SAP_CLNS))
	    data |= (mask & FCI_CLNS_PROTOCOL_MASK);
	if ((indx == (256 * 14) + SAP_SNAP) ||
	    (indx == (256 * 15) + SAP_SNAP)) {
	    data |= (mask & FCI_DOD_IP_PROTOCOL_MASK);
	    data |= (mask & FCI_DECNET_PROTOCOL_MASK);
	    data |= (mask & FCI_XNS_PROTOCOL_MASK);
	}
	cbus_af_write(idb, indx, data);
	cbus_af_write(idb, indx+1, data);
    }

    for (indx = (256 * 16); indx < (256 * 17); indx++) {
	data = smask;
	if (indx == (256 * 16) + 0x03) {	/* UI frame */
	    data |= (mask & FCI_CLNS_PROTOCOL_MASK);
	    data |= (mask & FCI_DOD_IP_PROTOCOL_MASK);
	    data |= (mask & FCI_DECNET_PROTOCOL_MASK);
	    data |= (mask & FCI_XNS_PROTOCOL_MASK);
	}
	cbus_af_write(idb, indx, data);
    }

    /*
     * Snap OUI field.  This is a don't care for anything.
     */
    for (indx = (256 * 17); indx < (256 * 20); indx++) {
	cbus_af_write(idb, indx, mask);
    }

    /*
     * Snap Type field.  Make sure that station, broadcast,
     * and bridging bits are set for all bytes.
     */
    for (indx = (256 * 20); indx < (256 * 22); indx++) {
	cbus_af_write(idb, indx, smask);
    }

    /*
     * Finish remainder of the filter RAM
     */
    for (indx = (256 * 22); indx < (256 * 32); indx++) {
	cbus_af_write(idb, indx, mask);
    }

    /*
     * The address filter has been set up -- except for the SNAP type field.
     */
    if (mask & FCI_DOD_IP_PROTOCOL_MASK) {
	cbus_af_or(idb, ((256 * 20) + 0x08), FCI_DOD_IP_PROTOCOL_MASK);
	cbus_af_or(idb, ((256 * 21) + 0x00), FCI_DOD_IP_PROTOCOL_MASK);

	/*
	 * Clear IP protocol bit in the 22nd octet except for 0x45.
	 */
	for (indx = (256 * 22); indx < (256 * 23); indx++) {
	    if (indx == (256 * 22) + 0x45)
		continue;
	    cbus_af_and(idb, indx, ~FCI_DOD_IP_PROTOCOL_MASK);

	}
        /*
         * Clear IP protocol bit in the 30th octet (TTL) for 00 and 01.
         * Only want IP bit set if TTL > 1.
         */
        for (indx = (256 * 30); indx < (256 * 30) + 2; indx++) {
            cbus_af_and(idb, indx, ~FCI_DOD_IP_PROTOCOL_MASK);
        }

    }
    if (mask & FCI_DECNET_PROTOCOL_MASK) {
	cbus_af_or(idb, ((256 * 20) + 0x60), FCI_DECNET_PROTOCOL_MASK);
	cbus_af_or(idb, ((256 * 21) + 0x03), FCI_DECNET_PROTOCOL_MASK);
	
	/*
	 * Clear DECnet protocol bit -- if padding with not one 1 byte.
	 */
	for (indx = ((256 * 24) + 0x80); indx < (256 * 25); indx++) {
	    if (indx == ((256 * 24) + 0x81))
		continue;
	    cbus_af_and(idb, indx, ~FCI_DECNET_PROTOCOL_MASK);
	}
    }

    /*
     * If routing XNS set type 0600 in octets 20 and 21.
     */
    if (mask & FCI_XNS_PROTOCOL_MASK) {
	cbus_af_or(idb, ((256 * 20) + 0x06), FCI_XNS_PROTOCOL_MASK);
	cbus_af_or(idb, ((256 * 21) + 0x00), FCI_XNS_PROTOCOL_MASK);
	/*
	 * In case of XNS 26th octet contains hope count
	 * Exclude packets with hope count 15 and greater.
	 */
	for (indx = (256 * 26) + 0x0F; indx < (256 * 27); indx++) {
	    cbus_af_and(idb, indx, ~FCI_XNS_PROTOCOL_MASK);
	}
    }
}

/**********************************************************************
 *
 *			 Initialization Routines
 *
 **********************************************************************/

/*
 * ctr_read_pointers
 *
 * This routine assumes that the interface has already been selected.  It
 * also assumes that it is being called with interrupts disabled.
 */
static void ctr_read_pointers (hwidbtype *idb, spyglass_data_t *ctr)
{
    uchar version_string[MAC_VERS_LEN+1];
    int i;
    leveltype level;

    /*
     * Read pointers in init block
     */
    level = raise_interrupt_level(NETS_DISABLE);
    ctr_memb_setup_inline(idb, TRC_TO_CUE_ADDRESS + 8);
    ctr->srb_addr      = ctr_memb_read(idb);
    ctr->arb_addr      = ctr_memb_read(idb);
    ctr->expressb_addr = ctr_memb_read(idb);
    ctr->mac_tcb_addr  = ctr_memb_read(idb);
    reset_interrupt_level(level);
    
    /*
     * Read pointers in SRB
     */
    level = raise_interrupt_level(NETS_DISABLE);
    ctr_memb_setup_inline(idb, ctr->srb_addr + 4);
    ctr->revision_addr = ctr_memb_read(idb);
    ctr->adapter_addr  = ctr_memb_read(idb);
    ctr->parms_addr    = ctr_memb_read(idb);
    reset_interrupt_level(level);
  
    /*
     * Read revision level
     */
    level = raise_interrupt_level(NETS_DISABLE);
    ctr_memb_setup_inline(idb, ctr->revision_addr);
    for (i = 0; i < SPYG_VERS_LEN; i+=2) {
	*(ushort *)&ctr->spyglass_rev[i] = ctr_memb_read(idb);
    }
    bcopy(ctr->spyglass_rev+3, version_string, SPYG_VERS_LEN-3);
    reset_interrupt_level(level);
    version_string[SPYG_VERS_LEN-3] = '\0';
    sys_ebcdic_to_ascii(version_string, SPYG_VERS_LEN-3);
}

static void ctr_issue_open (hwidbtype *idb, spyglass_data_t *ctr)
{
    leveltype level;
    ushort open_options;
    int i;

    (void)send_trip_set_mac_hi_cmd(idb, *((ushort *)(idb->hardware+0)));
    (void)send_trip_set_mac_mid_cmd(idb, *((ushort *)(idb->hardware+2)));
    (void)send_trip_set_mac_lo_cmd(idb, *((ushort *)(idb->hardware+4)));

    open_options = SPYG_OPEN_OPTIONS;
    if (idb->tr_ring_mode & RINGMODE_LOOPBACK)
	open_options |= SPYG_OPEN_WRAP;
    if (idb->tr_ring_mode & RINGMODE_CONTENDER)	/* currently no-op */
	open_options |= (SPYG_OPEN_CONTENDER);
    if (idb->tr_ring_mode & RINGMODE_NO_RINGSTAT)
	open_options |= (SPYG_OPEN_NOHARD | SPYG_OPEN_NOSOFT);
    if (!(idb->tr_ring_mode & RINGMODE_ETR))
	open_options |= (SPYG_OPEN_NO_ETR);
    ctr->last_open_options = open_options;

    /*
     * Kick off the Spyglass open command.
     */
    if (tokenevent_debug)
        buginf("\n%s: Issuing TRIP spyglass open, options=0x%04x",
              idb->hw_namestring, open_options);
    lock_ctr(idb, SPY_SRB_MAX_OPEN_TIME);
    level = raise_interrupt_level(NETS_DISABLE);
    ctr_memb_setup_inline(idb, ctr->srb_addr);
    ctr_memb_write_inline(idb, SPYG_SRB_OPEN_ADAPTER << 8);
    ctr_memb_write_inline(idb, SPYG_SRB_RETURN  << 8);
    ctr_memb_write_inline(idb, open_options);
    ctr_memb_write_inline(idb, *(ushort *)&idb->hardware[0]);
    ctr_memb_write_inline(idb, *(ushort *)&idb->hardware[2]);
    ctr_memb_write_inline(idb, *(ushort *)&idb->hardware[4]);
    for (i = 0; i < SPYG_PRODUCTID_LEN/2; i++) {
	ctr_memb_write_inline(idb, 0);
    }
    issue_ctr_srb_command_inline(idb);
    reset_interrupt_level(level);
}

static void ctr_open_failed (
    hwidbtype *idb,
    spyglass_data_t *ctr,
    ushort status)
{
    int phase, error, resultcode;
    leveltype level;
    boolean display;
    tokensb *tsb;

    level = raise_interrupt_level(NETS_DISABLE);
    (void)select_if(idb);
    (void)send_status_cmd(idb, MCI_IF_DISABLE);
    reset_interrupt_level(level);
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    resultcode = ((status & 0xFF00) >> 8);
    phase = ((status & 0x00F0) >> 4);
    error = (status & 0x000F);
    if (resultcode == SPYG_ERR_OPEN_FAILED && 
	(phase < TR_OPN_PHASES) &&
	(error < TR_OPN_ERRORS)) {
	/*
	 * The token ring adapter complained about a failure while it was
	 * testing the cable or inserting into the ring.  Log an error
	 * mesage indicating where the failure occurred.
	 */
	display = TRUE;
	if (ctr->last_cmd == SPYG_SRB_OPEN_ADAPTER
	    && ctr->last_rslt == status
	    && SLEEPING(tsb->tr_next_buginf_time))
	    display = FALSE;
        tsb->ringOpenStatus = error;

        /*SZ*/
        TR_OpenError(idb, error, phase, display);

	TIMER_START(tsb->tr_next_buginf_time, TR_BUGINF_INTERVAL);
	ctr->last_cmd = SPYG_SRB_OPEN_ADAPTER;
	ctr->last_rslt = status;
    } else if (resultcode < SPYG_ERR_LAST) {
	errmsg(&msgsym(CTRCMDFAILED, CBUS), idb->hw_namestring, 
	       "SPYG_SRB_OPEN_ADAPTER", spyg_cmd_errors[resultcode],
	       status);
    } else {
	errmsg(&msgsym(CTRCMDFAILED2, CBUS), idb->hw_namestring, 
	       "SPYG_SRB_OPEN_ADAPTER", status);
    }

    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}

/*
 *
 */
static void ctr_issue_set_functional (
    hwidbtype *idb,
    spyglass_data_t *ctr)
{
    leveltype level;
    ushort command;
    tokensb *tsb;
    boolean srb_bridging;
    srb_vector_table_t *srbV_p;
    boolean tbridge_on;
    idbtype *swidb;

    /*
     * Check interlock
     */
    if (ctr_locked(idb)) {
	level = raise_interrupt_level(NETS_DISABLE);
	ctr_memb_setup_inline(idb, ctr->srb_addr+2);
	command = ctr_memb_read(idb);
	reset_interrupt_level(level);
	errmsg(&msgsym(CTRCMDFAILED, CBUS), idb->hw_namestring,
              "ctr_issue_set_functional", "Command already in progress",
              command >> 8);
	return;
    }
    lock_ctr(idb, SPY_MAX_CMD_TIME);

    /*
     * Initialize functional addresses for routed protocols
     */
    tr_functional_init(idb);

    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    /*
     * Setup for source route bridging
     */
    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
       srb_bridging = srbV_p->srb_sb_get_bridging_enabled(idb->firstsw);
    } else {
       srb_bridging = FALSE;
    }
    if (srb_bridging) {
	idb->tr_ring_mode |= RINGMODE_BRIDGE;
	tsb->tr_functional |= TRFUNC_BRIDGE;
    } else
	idb->tr_ring_mode &= ~RINGMODE_BRIDGE;

    /*
     * Set up for transparent bridging
     */
    tbridge_on = FALSE;
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
      if (RUNNING_IF(LINK_BRIDGE,swidb) && bridge_enable &&
        !(system_loading | bootstrap_enable)) {
        tbridge_on = TRUE ;
        break ;
      }
    }
 
    if (tbridge_on) {
        idb->tr_ring_mode |= RINGMODE_TBRIDGE;
        tsb->tr_functional |= TRFUNC_BRIDGE;
        cbus_mci_rxtypes_update(idb);
    }
 

    /*
     * Set the Spyglass functional address.
     */
    if (tokenevent_debug)
        buginf("\n%s: Issuing TRIP spyglass set_functional, functional=0x%08x",
               idb->hw_namestring, tsb->tr_functional);
    level = raise_interrupt_level(NETS_DISABLE);
    ctr_memb_setup_inline(idb, ctr->srb_addr);
    ctr_memb_write_inline(idb, SPYG_SRB_SET_FUNCTIONAL << 8);
    ctr_memb_write_inline(idb, SPYG_SRB_RETURN  << 8);
    ctr_memb_write_inline(idb, tsb->tr_functional >> 16);
    ctr_memb_write_inline(idb, tsb->tr_functional & 0xffff);
    issue_ctr_srb_command_inline(idb);
    /*
     * Revise address filter
     */
    cbus_token_func_address(idb, FCI_MAC_MASK);
    cbus_token_func_address(idb, FCI_MULTICAST_MASK);
    cbus_token_group_address(idb, FCI_MULTICAST_MASK);
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
    reset_interrupt_level(level);
}

/*
 *
 */
static void ctr_issue_set_group (
    hwidbtype *idb,
    spyglass_data_t *ctr)
{
    leveltype level;
    ushort command;
    tokensb *tsb;

    /*
     * Check interlock
     */
    if (ctr_locked(idb)) {
	level = raise_interrupt_level(NETS_DISABLE);
	ctr_memb_setup_inline(idb, ctr->srb_addr+2);
	command = ctr_memb_read(idb);
	reset_interrupt_level(level);
	errmsg(&msgsym(CTRCMDFAILED, CBUS), idb->hw_namestring,
              "ctr_issue_set_group", "Command already in progress",
              command >> 8);
	return;
    }
    lock_ctr(idb, SPY_MAX_CMD_TIME);

    /*
     * Set the Spyglass group address.
     */
    tsb = idb_get_hwsb_inline(idb, HWIDB_SB_TOKEN);
    if (tokenevent_debug)
        buginf("\n%s: Issuing TRIP spyglass set_group, group=0x%08x",
               idb->hw_namestring, tsb->tr_group);
    level = raise_interrupt_level(NETS_DISABLE);
    ctr_memb_setup_inline(idb, ctr->srb_addr);
    ctr_memb_write_inline(idb, SPYG_SRB_SET_GROUP << 8);
    ctr_memb_write_inline(idb, SPYG_SRB_RETURN  << 8);
    ctr_memb_write_inline(idb, 0xC000);
    ctr_memb_write_inline(idb, tsb->tr_group >> 16);
    ctr_memb_write_inline(idb, tsb->tr_group & 0xffff);
    issue_ctr_srb_command_inline(idb);
    reset_interrupt_level(level);
}

static void ctr_issue_start_bridge (
    hwidbtype *idb,
    spyglass_data_t *ctr)
{
    leveltype level;
    idbtype   *swidb = firstsw_or_null(idb);
    srb_triplet_t srb_triplet;
    srb_vector_table_t *srbV_p;

    srbV_p = reg_invoke_srb_get_vector_table();
    if (!srbV_p) {
        return;
    }

    srbV_p->srb_sb_get_triplet(swidb, &srb_triplet);
    if (srb_triplet.bridgeNum > SRB_MAX_BRIDGE_NUM) {
        return;
    }

    /*
     * Check interlock
     */
    if (ctr_locked(idb)) {
	errmsg(&msgsym(CTRCMDFAILED, CBUS), idb->hw_namestring,
              "ctr_issue_start_bridge", "Command already in progress", 0);
        return;
    }
    lock_ctr(idb, SPY_MAX_CMD_TIME);

    /*
     * Set the Spyglass bridging parameters.
     */
    if (tokenevent_debug)
        buginf("\n%s: Issuing TRIP spyglass set_bridge, local_rng=0x%04x, br=%04x, target=0x%04x",
              idb->hw_namestring, srb_triplet.localRing, srb_triplet.bridgeNum,
              srb_triplet.remoteRing);
    level = raise_interrupt_level(NETS_DISABLE);
    ctr_memb_setup_inline(idb, ctr->srb_addr);
    ctr_memb_write_inline(idb, SPYG_SRB_SET_BRIDGE << 8);
    ctr_memb_write_inline(idb, SPYG_SRB_RETURN  << 8);
    ctr_memb_write_inline(idb, srb_triplet.bridgeNum);
    ctr_memb_write_inline(idb, srb_triplet.localRing);
    ctr_memb_write_inline(idb, srb_triplet.remoteRing);
    srbV_p->srb_sb_init_rng_br_rng_mask(swidb);
    issue_ctr_srb_command_inline(idb);

    /*
     * Go out an torque the bridging bit in the address filter.  This
     * needs to be be done because we do not want to bounce the token
     * ring when bridging is enable or disabled.
     */
    cbus_token_ext_address(idb);

    /*
     * Now set up the other PH registers to make things work correctly.
     */
    (void)send_trip_reset_ph_bit_cmd(idb, 
				    (SPYG_RCR_IGNCTL | SPYG_RCR_IGN_MASK));
    (void)send_trip_set_ph_bit_cmd(idb, 0x0004);
    (void)send_trip_set_ph_bit_cmd(idb, SPYG_RCR_ACCRI);
    if (srbV_p->srb_sb_get_spanning_explorer(swidb))
        (void)send_trip_set_ph_bit_cmd(idb, SPYG_RCR_ACCLB);
    reset_interrupt_level(level);
}

static void ctr_issue_start_tbridge (
    hwidbtype *idb,
    spyglass_data_t *ctr)
{
    leveltype level;
    idbtype *swidb ;
    boolean tbridge_on;

    tbridge_on = FALSE ;
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {    
      if (RUNNING_IF(LINK_BRIDGE,swidb) && bridge_enable &&
	!(system_loading | bootstrap_enable)) {
        tbridge_on = TRUE ;
        break ;
      }
    }
    
    /* any successful sub interface would enable the start */
    if (!tbridge_on)
      return ;

    /*
     * Check interlock
     */
    if (ctr_locked(idb)) {
	errmsg(&msgsym(CTRCMDFAILED, CBUS), idb->hw_namestring,
              "ctr_issue_set_group", "Command already in progress", 0);
	return;
    }
    lock_ctr(idb, SPY_MAX_CMD_TIME);

    /*
     * Set the Spyglass into promiscuous mode.
     */
    if (tokenevent_debug)
        buginf("\n%s: Issuing TRIP spyglass start_tbridge command",
               idb->hw_namestring);
    level = raise_interrupt_level(NETS_DISABLE);
    ctr_memb_setup_inline(idb, ctr->srb_addr);
    ctr_memb_write_inline(idb, SPYG_SRB_START_TBRIDGE << 8);
    ctr_memb_write_inline(idb, SPYG_SRB_RETURN  << 8);
    issue_ctr_srb_command_inline(idb);

    /*
     * Go out an torque the bridging bit in the address filter.  This
     * needs to be be done because we do not want to bounce the token
     * ring when bridging is enable or disabled.
     */
    cbus_token_ext_address(idb);
    reset_interrupt_level(level);
}

void ctr_modify (hwidbtype *idb, ulong type, ulong unused2)
{
    spyglass_data_t *ctr;
    leveltype level;

    if (idb->state != IDBS_UP)
        return;

    if (type == TR_MODIFY_SPAN) {
        /*
         * Change the spanning state in the Spyglass
         */
        ctr = idb->ctr_private;
        level = raise_interrupt_level(NETS_DISABLE);
        ctr_issue_start_bridge(idb, ctr);
        reset_interrupt_level(level);
    } else 
        cbus_token_reset(idb);
}

void ctr_setup (hwidbtype *idb)
{
    if (idb->state == IDBS_UP)
        cbus_token_reset(idb);
}


/*
 * ctr_set_addresses
 *
 * This routines sets the new group and functional addresses into the
 * spyglass chip.  It also needs to set the address filter up
 * appropriately for these addresses.  Furthermore, it needs to do the
 * equivalent of "set options" in the TI processor.  Since the spyglass
 * has no similar command, we tweak the address filter to achieve the
 * same filtering functionality.  !!DRH!!
 */
boolean ctr_set_addresses (hwidbtype *idb)
{
    spyglass_data_t *ctr;

    ctr = idb->ctr_private;

    if ((idb->state != IDBS_INIT) && (idb->state != IDBS_UP))
	return(FALSE);
    if ( ctr->init_phase != INIT_OPEN_COMPLETE)
        return(FALSE);

    ctr->set_functional_only = TRUE;
    if (ctr_locked(idb))
	ctr->set_functional_needed = TRUE;
    else
	ctr_issue_set_functional(idb, ctr);
    return(TRUE);
}

/**********************************************************************
 *
 *		       Receive Support Routines
 *
 **********************************************************************/


/**********************************************************************
 *
 *			 Utility Routines
 *
 **********************************************************************/

static void ctr_read_spyglass_errors (hwidbtype *idb)
{
    int i;
    spyglass_data_t *ctr;
    leveltype level;

    if (!(idb->state == IDBS_UP))
	return;
    /*
     * Check interlock
     */
    if (ctr_locked(idb)) {
        return;
    }
    lock_ctr(idb, SPY_MAX_CMD_TIME);

    /*
     * Kick off the Spyglass Read Error Log command.
     */
    ctr = idb->ctr_private;
    level = raise_interrupt_level(NETS_DISABLE);
    ctr_memb_setup_inline(idb, ctr->srb_addr);
    ctr_memb_write_inline(idb, SPYG_SRB_READ_ERROR_LOG << 8);
    ctr_memb_write_inline(idb, SPYG_SRB_RETURN  << 8);
    for (i = 0; i < sizeof(spyg_error_log_t)/2; i++) {
	ctr_memb_write_inline(idb, 0);
    }
    issue_ctr_srb_command_inline(idb);
    reset_interrupt_level(level);
}

static void ctr_read_spyglass_smt (hwidbtype *idb)
{
    spyglass_data_t *ctr;
    ushort *p;
    int i;
    leveltype level;

    ctr = idb->ctr_private;

    /*
     * Read the Current SpyglassAdreses.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    ctr_memb_setup_inline(idb, ctr->adapter_addr);
    p = (ushort *)&ctr->addrs;
    for (i = 0; i < sizeof(spyg_addrs_t)/2; i++) {
	*p++ = ctr_memb_read(idb);
    }
    reset_interrupt_level(level);

    /*
     * Read the Spyglass SMT Parameters.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    ctr_memb_setup_inline(idb, ctr->parms_addr);
    p = (ushort *)&ctr->smt;
    for (i = 0; i < sizeof(spyg_parms_t)/2; i++) {
	*p++ = ctr_memb_read(idb);
    }
    reset_interrupt_level(level);
}

/*
 * ctr_update_prev_stats: update our accumulation of previous stats
 * 	from the card with their current values.
 *
 * This gives us running totals and is useful for debugging.  This also
 * nails the cells we use for keeping track of ring status messages.
 */

void ctr_update_prev_stats (hwidbtype *idb)
{
    spyglass_data_t *ctr;
    spyg_parms_t *smt;
    tokensb *tsb;

    ctr = idb->ctr_private;
    smt = (spyg_parms_t *)&ctr->smt;
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    ieee_copy(&(smt->una[0]), &(tsb->naun[0]));
    ieee_copy(&(smt->beacon_una[0]), &(tsb->beacon_naun[0]));
    tsb->last_beacon_type = smt->last_beacon_type;

    /*
     * Add ring status errors
     */
    tring_update_counters(tsb);

    /*
     * Save the old value
     */
    ctr = idb->ctr_private;
    ctr->prev_runts = idb->counters.runts;
    ctr->prev_giants = idb->counters.giants;
    ctr->prev_input_overrun = idb->counters.input_overrun;
    ctr->prev_input_resource = idb->counters.input_resource;
    ctr->prev_parity = idb->counters.parity;
    ctr->prev_mac_giants = ctr->mac_giants;
    ctr->prev_mac_ignored = ctr->mac_ignored;
    ctr->prev_rx_rfed = ctr->rx_rfed;
    ctr->prev_rx_redi = ctr->rx_redi;
    ctr->prev_rx_null_rcp = ctr->rx_null_rcp;
    ctr->prev_rx_recover_rcp = ctr->rx_recover_rcp;
    ctr->prev_rx_explicit_abort = ctr->rx_explicit_abort;
    ctr->prev_rx_implicit_abort = ctr->rx_implicit_abort;
    ctr->prev_tx_sfed = ctr->tx_sfed;
    ctr->prev_tx_sedi = ctr->tx_sedi;
    ctr->prev_tx_false_token = ctr->tx_false_token;
    ctr->prev_tx_dma_parity = ctr->tx_dma_parity;
    ctr->prev_tx_next_buffer = ctr->tx_next_buffer;
    ctr->prev_tx_strip_abort = ctr->tx_strip_abort;
    ctr->prev_tx_ptt_expired = ctr->tx_ptt_expired;
    ctr->prev_tx_cumbytes = idb->counters.tx_cumbytes;
    ctr->prev_rx_cumbytes = idb->counters.rx_cumbytes;
    
    /*
     * Add Spyglass errors log
     */
    tsb->prev_err_line		+= ctr->err_line;
    tsb->prev_err_internal	+= ctr->err_internal;
    tsb->prev_err_burst		+= ctr->err_burst;
    tsb->prev_err_arifci	+= ctr->err_arifci;
    tsb->prev_err_abort		+= ctr->err_abort;
    
    tsb->prev_err_lostframe 	+= ctr->err_lostframe;
    tsb->prev_err_rcvcongestion += ctr->err_rcvcongestion;
    tsb->prev_err_copy		+= ctr->err_copy;
    tsb->prev_err_freq		+= ctr->err_freq;
    tsb->prev_err_token		+= ctr->err_token;         

    ctr->err_line = ctr->err_internal = ctr->err_burst =
	ctr->err_arifci = ctr->err_abort = ctr->err_lostframe =
	ctr->err_rcvcongestion = ctr->err_copy =
	ctr->err_freq = ctr->err_token = 0;

    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}


/**********************************************************************
 *
 *	       Upcalls from the cBus Interrupt routine
 *
 **********************************************************************/

/*
 *
 */
static void ctr_read_ringstat (hwidbtype *idb)
{
    spyglass_data_t *ctr;
    ushort ring_status;
    boolean display;
    tokensb *tsb;

    ctr = idb->ctr_private;
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    (void)send_trip_read_ring_stat_cmd(idb, &ring_status);
    tsb->tr_ring_status = ring_status;
    GET_TIMESTAMP(tsb->tr_status_time);

    /*
     * Should we display a message for this ring status.
     */
    display = FALSE;
    if (ring_status & RNG_WIRE_FAULT) {
	if ((ctr->last_cmd != CB_T2M_RING_STAT) ||
	    ( !(ctr->last_p1 & RNG_WIRE_FAULT)))
	    display = TRUE;
	else if (AWAKE(tsb->tr_next_buginf_time))
	    display = TRUE;
    }
    
    /*
     * Call lanmgr_ringstat() here, because the interface may not be UP yet
     * The tr_ringstat_common() does not call it.
     */    
    if (idb->state == IDBS_UP || ctr->init_phase == INIT_OPEN_COMPLETE)
        reg_invoke_media_tring_ring_status(idb, ring_status);
    if (tr_ringstat_common(idb, ring_status, display)) {
	TIMER_START(tsb->tr_next_buginf_time, TR_BUGINF_INTERVAL);
    }
    ctr->last_cmd = SPYG_ARB_RING_STATUS;
    ctr->last_rslt = SPYG_ERR_SUCCESS;
    ctr->last_p1 = ring_status;
    
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}

static void ctr_adapter_check (hwidbtype *idb)
{
    ushort check_ptr, a, b, c, d;

    net_cstate(idb, IDBS_RESET);
    (void)send_trip_get_check_ptr_cmd(idb, &check_ptr);

    ctr_memb_setup_inline(idb, check_ptr);
    a = ctr_memb_read(idb);
    b = ctr_memb_read(idb);
    c = ctr_memb_read(idb);
    d = ctr_memb_read(idb);
    errmsg(&msgsym(CTRACHECK, CBUS), idb->hw_namestring, a, b, c, d);
}

static void ctr_ucode_check (
    hwidbtype *idb)
{
    net_cstate(idb, IDBS_RESET);
    errmsg(&msgsym(CTRUCHECK, CBUS), idb->hw_namestring);
}

/*
 * ctr_deadman_disable
 * Turn off Spyglass deadman timer
 * Used on the 7000 only, for EOIR.  Long stall times and the 
 * architecture of the CTR/TRIP card can cause us to fail to 
 * stroke the timer frequently enough and the Spyglass will 
 * crash.  To avoid this, we disable the timer and use other
 * means to verify that the interfaces are still functional at
 * all times.
 *
 * Must be called after open completes.  Must be called
 * with interrupts disabled.
 */
static void ctr_deadman_disable (hwidbtype *idb)
{    
    ushort data;

    (void)select_if(idb);

    /*
     * Select the address to read
     */
    (void)send_trip_spyg_mem_select_cmd(idb, SPYGLASS_PH_INTCTL);

    (void)send_trip_spyg_mem_read_cmd(idb); /* issue read request cmd */

    DELAY(1);			/* allow time for command to complete */

    /*
     * Get the result
     */
    (void)send_trip_spyg_mem_read_rslt_cmd(idb, &data);
    data &= ~SPYGLASS_DEADMAN_TIMER_ERROR_ENABLE; /* clear timer enable */

    /*
     * Reselect the address
     */
    (void)send_trip_spyg_mem_select_cmd(idb, SPYGLASS_PH_INTCTL);

    /*
     * Rewrite the register 
     */
    (void)send_trip_spyg_mem_write_cmd(idb, data);
}    

static void ctr_init_complete (
    hwidbtype *idb,
    spyglass_data_t *ctr)
{
    ushort error;
    ushort status;
    char *result, dummy[20];

    if (tokenevent_debug)
        buginf("\n%s: TRIP spyglass init command completed", idb->hw_namestring);

    /*
     * Did the Initialization Test pass?  If not, print the
     * failure code and the extended error code.
     */
    select_if(idb);
    status = send_trip_init_complete_cmd(idb, &error);
    if (status != FCI_RSP_OKAY) {
	errmsg(&msgsym(DAUGHTER_NO_RSP, CBUS), idb->unit,
	       idb->cbus_ctrlr_index, status);
        cbus_restarts_error++;
	platform_demand_complex_restart(idb);
	return;
    }
    if (error) {
	/*
	 * Ram test or Initialize error
	 */
	(void)sprintf(dummy, "%#04x", error);
	if (error & FCI_INIT_RAM_ERR) {
	    error &= ~FCI_INIT_RAM_ERR;
	    if (error <= SPYG_RAMTEST_LAST) {
		result = spyg_ramtest_errors[error];
	    } else {
		result = dummy;
	    }
	    errmsg(&msgsym(CTRRAMTEST, CBUS), idb->hw_namestring, result);
	} else {
	    if ((error >= SPYG_INIT_FIRST) && (error <= SPYG_INIT_LAST)) {
		result = spyg_init_errors[error - SPYG_INIT_FIRST];
	    } else {
		result = dummy;
	    }
	    errmsg(&msgsym(CTRINIT, CBUS), idb->hw_namestring, result);
	}
	net_cstate(idb, IDBS_RESET);
	return;
    }
    
    /*
     * Read the pointers from the spyglass.
     */
    ctr_read_pointers(idb, ctr);
    
    /*
     * Enable reception.  It is possible for this command to
     * fail if there are insufficient receive buffers allocated.
     */
    if (tokenevent_debug)
        buginf("\n%s: Issuing CBUS enable", idb->hw_namestring);
    status = send_if_enable_cmd(idb);
    if (status != MCI_RSP_OKAY) {
        errmsg(&msgsym(RSETFAIL, CBUS), idb->hw_namestring,
                "cbus_token_reset()", status);
        net_cstate(idb, IDBS_RESET);
        return;
    }

    ctr->init_phase = INIT_ENABLE;
}

static void ctr_enable_complete (
    hwidbtype *idb,
    spyglass_data_t *ctr)
{
    if (tokenevent_debug)
        buginf("\n%s: CBUS enable completed", idb->hw_namestring);
    ctr_issue_open(idb, ctr);
    ctr->init_phase = INIT_OPEN;
}

static boolean  ctr_open_complete (
    hwidbtype *idb,
    spyglass_data_t *ctr)
{
    ushort status, actualring, srb_thisring;
    idbtype *swidb;
    tokensb *tsb;
    srb_vector_table_t *srbV_p;

    idb->mci_lineup = MCI_KEEP_COUNT;
    GET_TIMESTAMP(idb->mci_lineuptimer);
 
    if (tokenevent_debug)
        buginf("\n%s: TRIP spyglass open command completed", idb->hw_namestring);

    /*
     * Open was successful.  Do housekeeping.
     * If not print error message and leave.
     */
    ctr->init_phase = INIT_NONE;
    ctr_memb_setup_inline(idb, ctr->srb_addr+2);
    status = ctr_memb_read(idb);
    if (status) {
	net_cstate(idb, IDBS_DOWN);
	ctr_open_failed(idb, ctr, status);
	return(FALSE);
    }

    /*
     * Disable the Spyglass watchdog timer
     */
    ctr_deadman_disable(idb);
	    
    /*
     * Check the actual ring number.
     */
    ctr_read_spyglass_smt(idb);
    actualring = ctr->smt.local_ring;
    if (tokenevent_debug)
        buginf("\n%s: Actual ring number from RPS=0x%04x %s",
               idb->hw_namestring, actualring,
               ((actualring == 0) ? "(no RPS found)" : ""));
    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srb_thisring = srbV_p->srb_sb_get_thisring(idb->firstsw);
    } else {
        srb_thisring = SRB_INVALID_RING_NUM;
    }
    if (srb_thisring && actualring && (actualring != srb_thisring))
	errmsg(&msgsym(BADRNGNUM, TR),
	       idb->unit, srb_thisring, ctr->smt.local_ring);
    reg_invoke_media_rps_startup(idb, actualring);

    /*
     * A little housekeeping.  The XNS and Novell protocols rely on the
     * hardware address for their node addresses.
     */
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	reg_invoke_media_hwaddr_notify(swidb, idb->hardware, IEEEBYTES);
    }
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    rif_update(idb, 0, idb->hardware, NULL, NULL, RIF_INTERFACE);
    tsb->tr_ring_status = 0;
    GET_TIMESTAMP(tsb->tr_status_time);
    idb->counters.transitions++;
    ctr->init_phase = INIT_OPEN_COMPLETE;
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
    return(TRUE);
}

static void ctr_error_log_complete (
    hwidbtype *idb,
    spyglass_data_t *ctr)
{
    spyg_error_log_t counters;
    ushort *p;
    int i;

    /*
     * Read the counter values back in.
     */
    ctr_memb_setup_inline(idb, ctr->srb_addr + 4);
    p = (ushort *)&counters;
    for (i = 0; i < sizeof(spyg_error_log_t)/2; i++) {
	*p++ = ctr_memb_read(idb);
    }

    /*
     * Now add them into the working 32 bit counters.
     */
    ctr->err_line          += counters.line;
    ctr->err_internal	   += counters.internal;
    ctr->err_burst	   += counters.burst;
    ctr->err_arifci	   += counters.arifci;
    ctr->err_abort	   += counters.abort;
    ctr->err_reserved6	   += counters.reserved6;
    ctr->err_lostframe	   += counters.lostframe;
    ctr->err_rcvcongestion += counters.rcvcongestion;
    ctr->err_copy	   += counters.copy;
    ctr->err_freq	   += counters.freq;
    ctr->err_token	   += counters.token;
    ctr->err_reserved11	   += counters.reserved11;
    ctr->err_reserved12	   += counters.reserved12;
    ctr->err_reserved13	   += counters.reserved13;
}


static void ctr_state_machine (hwidbtype *idb, ushort status)
{
    spyglass_data_t *ctr;
    ushort command, result;
    tokensb *tsb;

    ctr = idb->ctr_private;

    /*
     * O.K.  Now, why are we here?  Check for the simple cases first:
     * initialization complete and enable complete.
     */
    select_if(idb);
    if (status & CTR_COND_INIT_DONE) {
	ctr_init_complete(idb, ctr);
    }
    if (status & CTR_COND_ENABLE_COMP) {
	ctr_enable_complete(idb, ctr);
    }

    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    /*
     * Now look to see if a command completed.  If so, then we will have
     * to see WHICH command completed and do the appropriate thing for
     * that command.
     */
    if (status & CTR_COND_SRB_COMPLETE) {
	ctr_memb_setup_inline(idb, ctr->srb_addr);
	command = ctr_memb_read(idb);
	result = ctr_memb_read(idb);
	if (!ctr_locked(idb)) {
            if (idb->state == IDBS_UP || ctr->init_phase == INIT_OPEN_COMPLETE) {
	        errmsg(&msgsym(CTRBADLOVE2, CBUS), idb->hw_namestring, command, result);
            }
	    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	    return;
	}
	unlock_ctr(idb);
	switch (command >> 8) {
            case SPYG_SRB_READ_ERROR_LOG:
	        if (result) {
		    errmsg(&msgsym(CTRCMDFAILED2, CBUS), idb->hw_namestring, 
			   "SPYG_SRB_READ_ERROR_LOG", result);
		    if (result == FCI_RSP_ERR_TMOUT) {
                        if (tsb->tr_polling_state == TR_POLLING_OK) {
			    tsb->tr_polling_state = TR_POLLING_TMOUT;
			    tsb->tr_polling_failure = 0;
                        }
		    }
		    break;
		}
		ctr_error_log_complete(idb, ctr);
		break;
		
	    case SPYG_SRB_OPEN_ADAPTER:
		if (!ctr_open_complete(idb, ctr))
		    break;
		if (registry_populated(REG_SRT))
		    reg_invoke_srt_setup_auto_sw_srb(idb);
		ctr->set_functional_needed = FALSE;
		ctr_issue_set_functional(idb, ctr);
		break;
		
	    case SPYG_SRB_CLOSE_ADAPTER:
		break;
		
	    case SPYG_SRB_SET_FUNCTIONAL:
		if (result)
		    errmsg(&msgsym(SETFUNFAIL, TR), idb->unit, result);
		if (idb->tr_ring_mode & RINGMODE_NEED_MAC)
		    (void)send_trip_accept_mac_cmd(idb, MAC_ACCEPT);
		else
		    (void)send_trip_accept_mac_cmd(idb, MAC_PITCH);
		if ((idb->state == IDBS_UP) && ctr->set_functional_only) {
		    ctr->set_functional_only = FALSE;
		    break;
		}
		ctr_issue_set_group(idb, ctr);
		break;
		
	    case SPYG_SRB_SET_GROUP:
		if (result)
		    errmsg(&msgsym(SETGRPFAIL, TR), idb->unit, result);
		ctr_issue_start_bridge(idb, ctr);
		if (!ctr_locked(idb))
		    ctr_issue_start_tbridge(idb, ctr);
		net_cstate(idb, IDBS_UP);
                if (tsb->tr_ring_status == 0)
                    reg_invoke_media_tring_ring_status(idb, 0);
		break;
		
	    case SPYG_SRB_SET_BRIDGE:
		if (result)
		    errmsg(&msgsym(SETBRIDGEFAIL, TR), idb->unit, result);
		ctr_issue_start_tbridge(idb, ctr);
		break;
		
	    case SPYG_SRB_START_TBRIDGE:
		if (result)
		    errmsg(&msgsym(SETBRIDGEFAIL, TR), idb->unit, result);
		break;
		
	    default:
		errmsg(&msgsym(CTRBADLOVE1, CBUS), idb->hw_namestring, command >> 8);
		{
		    ulong a, b, c, d, e, f, g, h;
		    ctr_memb_setup_inline(idb, ctr->srb_addr);
		    a = ctr_memb_read(idb);
		    b = ctr_memb_read(idb);
		    c = ctr_memb_read(idb);
		    d = ctr_memb_read(idb);
		    e = ctr_memb_read(idb);
		    f = ctr_memb_read(idb);
                    g = ctr_memb_read(idb);
                    h = ctr_memb_read(idb);
                    errmsg(&msgsym(CTRBADLOVE1_DUMP, CBUS), a, b, c, d, e, f, g, h);
		}
		break;
        }
	if (ctr->set_functional_needed && !ctr_locked(idb)) {
	    ctr->set_functional_needed = FALSE;
	    ctr_issue_set_functional(idb, ctr);
	}
    }

    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}

void ctr_conditions (hwidbtype *idb, ushort status)
{
    spyglass_data_t *ctr;

    if (is_vip(idb)) {
	/*
	 * VIP-4R uses loveletters instead of lovenotes, so we
	 * shouldn't have gotten here.
	 */
	if (tokenevent_debug)
	    buginf("\n%s: Unexpected lovenote code 0x%04x rec'd - wrong idb type",
            idb->hw_namestring, status);
	return;
    }

    ctr = idb->ctr_private;
    ctr->conditions |= status;

    if (idb->state == IDBS_ADMINDOWN || idb->state == IDBS_RESET) {
        if (tokenevent_debug) {
            buginf("\n%s: Unexpected lovenote code 0x%04x rec'd in state %s",
                   idb->hw_namestring, status,
                   ((idb->state == IDBS_ADMINDOWN) ? "ADMINDOWN" : "RESET"));
        }
	return;
    }

    (void)select_if(idb);
    if (status & CTR_COND_ADAPTER_CHECK)
	ctr_adapter_check(idb);
    if (status & CTR_COND_UCODE_CHECK)
	ctr_ucode_check(idb);
    ctr_state_machine(idb, status);
    if (status & CTR_COND_RING_STAT)
	ctr_read_ringstat(idb);
}


/**********************************************************************
 *
 *		   Calls from the system level code
 *
 **********************************************************************/



/*
 * cbus_token_setup_af
 *
 * Set up the address filter for a cbus token ring interface.
 */
static void cbus_token_setup_af (hwidbtype *idb)
{
    ushort sresult, filter;
    ushort actualsize;
    int i;
    uchar swapped_addr[IEEEBYTES];
    leveltype level;
    idbtype	*swidb;

    /*
     * Initialize address filter.  If spanning tree index is set, we
     * set up the HyBridge FSM, otherwise we clear all FSM's.  Note that
     * we refuse to set the HyBridge FSM if the MCI controller is not setup
     * for bridging support.  This prevents a performance disaster.    
     * Clear mask of FSM's that accept multicast addresses.
     */
    idb->multibits = 0;
    filter = 0;
    level = raise_interrupt_level(NETS_DISABLE);
    (void)select_if(idb);
    if (bridge_enable && idb->tbridge_on_hwidb) {
	filter = (FCI_BRIDGING_MASK << 8);
	sresult = send_bridge_cmd(idb, filter);
	if (sresult != FCI_RSP_OKAY)
	    errmsg(&msgsym(CTRRSET, CBUS), idb->hw_namestring,
                   "send_bridge_cmd()", sresult);
    }

    /*
     * Insert our address into the bridging database.
     */
    ieee_swap(idb->hardware, swapped_addr);

    /*
     * for sub interface, the first swidb may not have the bridge configured, 
     * but rest swidb could have.  Make sure the interest addr is on
     * if configuration.
     */
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
        reg_invoke_tbridge_smf_update(swapped_addr,
				  swidb,
				  SMF_MACTYP_OWN_UNICAST |
				  SMF_ACTION_RECEIVE);


        reg_invoke_media_interesting_addresses(swidb);
    }
    /*
     * Find out the actual size of the Address Filter RAM.  If it is
     * larger than this routine was originally coded for, set the new
     * locations to 0xFF so that nothing breaks.
     */
    (void)send_af_size_cmd(idb, &actualsize);
    if (actualsize > CTR_AF_WORDS) {
	(void)send_af_select_cmd(idb, CTR_AF_WORDS);
	for (i = CTR_AF_WORDS; i < actualsize; i++) {
	    (void)send_af_write_cmd(idb, 0xFF);
	}
    }

    /*
     * Zero out the entire address filter memory -- receive nothing.
     */
    (void)send_af_select_cmd(idb, 0);
    for (i = 0; i < CTR_AF_WORDS; i++) {
	(void)send_af_write_cmd(idb, 0);
    }
    
    /*
     * Enable Reception for appropriate addresses and address groups.
     * Determine our station address based on level 3 protocol support.
     * Also set broadcast address.
     */
    cbus_token_address(idb, FCI_STATION_MASK, idb->hardware);
    cbus_token_address(idb, FCI_MULTICAST_MASK, baddr);
    cbus_token_func_address(idb, FCI_MAC_MASK);
    cbus_token_func_address(idb, FCI_MULTICAST_MASK);
    cbus_token_group_address(idb, FCI_MULTICAST_MASK);

    /*
     * Setup the extended address filter for CTR devices -- this function 
     * must called after all address are set i.e. no calls to the function
     * cbus_token_address after this function call.
     */
    cbus_token_ext_address(idb);
    reset_interrupt_level(level);
}

/*
 * cbus_token_reset
 *
 * Reset a cBus Token Ring interface.  This routine (instead of the open
 * routine) must check the hardware address for a decnet style address
 * and do the conversion, because this is where the address filter gets
 * initialized.
 */
boolean cbus_token_reset (hwidbtype *idb)
{
    leveltype level;
    int i;
    spyglass_data_t *ctr;
    boolean eoir_reset;
    tokensb *tsb;
    uchar save_bia[IEEEBYTES];
    uchar save_hardware[IEEEBYTES];

    if (tokenevent_debug)
        buginf("\n%s: Interface reset", idb->hw_namestring);

    if (idb->status & IDB_NOHARDWARE) {
	errmsg(&msgsym(RESETNXI, CBUS), idb, idb->hw_namestring);
	return(TRUE);
    }

    if (idb->oir_flags & EOIR_RESET)
	eoir_reset = TRUE;
    else
	eoir_reset = FALSE;

    ctr = idb->ctr_private;

    level = raise_interrupt_level(NETS_DISABLE);
    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    (void)select_if(idb);
    idb->counters.resets++;

    if (!eoir_reset) {
	TIMER_STOP(idb->mci_lineuptimer);
	idb->mci_lineup = 0;
	idb->status &= ~IDB_DISABLED;
	tsb->tr_polling_count = TR_POLLING_PERIOD;
	tsb->tr_polling_state = TR_POLLING_OK;
	ctr->conditions = 0;
	ctr->init_phase = INIT_NONE;
	reg_invoke_bump_cache_version(idb, NULL, TRUE);
	(void)send_if_disable_cmd(idb); 
	
	/*
	 * Flush hold queue
	 */
	holdq_reset(idb);
	
	/*
	 * Initialize the burned in MAC address and the working
	 * MAC address for the interface.
	 */
	if (is_ieee_zero(idb->bia)) {

            /*
             * Burned in MAC addr not setup yet - go initialize it
             */
	    (void) reg_invoke_assign_mac_addr(idb);

	    /*
             * Platform assigned MAC address is in ethernet format.
             * So, we have to bit flip it.
             */
            for (i = 0; i < IEEEBYTES; i++)
	        idb->bia[i] = BITSWAP(idb->bia[i]);

            /*
             * Don't disturb working MAC address if user has configured a LAA
             */
	    if (is_ieee_zero(idb->hardware))
	        ieee_copy(idb->bia, idb->hardware);

	} else {

            /*
             * Burned in MAC address already setup.  
             *
             * Some platforms pre-initialize all interfaces with an
             * ethernet format MAC address before giving us a chance
             * to set our own MAC address in token ring format.
             * Here we check our burned in MAC against the ethernet 
             * format MAC addr that would be assigned to us.  If
             * they're equal then we've been pre-assigned and we
             * must bit flip the existing burned-in MAC address to token ring format.
             */
            ieee_copy(idb->bia, save_bia);
            ieee_copy(idb->hardware, save_hardware);
            ieee_zero(idb->bia);     /* Must be zero for assign_mac_addr to work */
            (void) reg_invoke_assign_mac_addr(idb);
            if (ieee_equal(save_bia, idb->bia)) {
                /*
                 * Yep, we got the standard e-net address for our slot/port - flip it
                 */
                for (i = 0; i < IEEEBYTES; i++)
                   idb->bia[i] = BITSWAP(idb->bia[i]);
                /*
                 * Check working MAC address to see if it was also pre-assigned
                 * an ethernet address.  Also, make sure it's not zero.
                 */
                if (ieee_equal(save_hardware, save_bia) || is_ieee_zero(idb->hardware))
                   ieee_copy(idb->bia, idb->hardware);
                else
                   ieee_copy(save_hardware, idb->hardware);
            } else {
                ieee_copy(save_bia, idb->bia);
                ieee_copy(save_hardware, idb->hardware);
	    }
        }

    }
    /*
     * If interface is administratively down, don't re-enable
     */
    if (idb->state == IDBS_ADMINDOWN) {
	if (eoir_reset) {
	    send_status_cmd(idb, FCI_STATUS_UNQUIESCE);
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	reset_interrupt_level(level);
	return(TRUE);
    }

    /*
     * Are resets allowed yet?
     */
    if (nvsize && !system_configured) {
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	reset_interrupt_level(level);
	return(TRUE);
    }

    if (!idb->tr_ring_speed) {
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	reset_interrupt_level(level);
	return(TRUE);
    }

    /*
     * Assign the interface to a buffer pool, set max datagramsize,
     * set receive offset and love note pool.
     */
    if (!eoir_reset)
	ctr->init_phase = INIT_DISABLE;

    /*
     * Call a platform specific routine to pass down platform
     * specific information for this interface (for example,
     * buffer pools on the c7000/ags and hardware queues on 
     * the RSP.
     */
    trip_restart_init(idb);

    trip_cbus_mci_txlimit(idb);

    if (eoir_reset) {
	send_status_cmd(idb, FCI_STATUS_UNQUIESCE);
	TIMER_STOP(idb->xmittimer);
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	reset_interrupt_level(level);
	return(TRUE);
    }
    /*
     * Set transmitter delay counter
     */
    reg_invoke_media_transmit_delay(idb->type, idb, idb->xmitdelay);

    /*
     * Validate the hardware address.  If it is invalid, use the
     * universally administered address for the interface.
     */
    if (idb->hardware[0] & TR_GROUP) {
	/*
	 * Some protocols like to change the MAC addresses of all the
	 * interfaces.  Unfortunately, the bit indicating a multicast is
	 * different on token ring than on ethernet, so setting a token ring
	 * and an ethernet to the same address can be dangerous.  Here, we
	 * make sure that we don't set the address to a multicast, printing a
	 * nasty message (which includes the process running) instead.  There
	 * is a potential problem with people manually setting a funny XNS or
	 * Novell adddress.
	 */
	errmsg(&msgsym(BADMUL, TR), idb->unit, idb->hardware);
	ieee_copy(idb->bia, idb->hardware);
    }

    /*
     * Initialize address filter.  If spanning tree index is set, we
     * set up the HyBridge FSM, otherwise we clear all FSM's.  Note that
     * we refuse to set the HyBridge FSM if the MCI controller is not setup
     * for bridging support.  This prevents a performance disaster.    
     * Clear mask of FSM's that accept multicast addresses.
     */
    cbus_token_setup_af(idb);
    reg_invoke_media_rif_reset(idb);
    
    /*
     * Kick off the Spyglass initialization command.  This causes both a
     * Spyglass "RAM TEST" and an "INITIALIZE" command to happen.
     */
    if (tokenevent_debug)
        buginf("\n%s: Issuing TRIP spyglass init command", idb->hw_namestring);
    ctr->conditions = 0;
    unlock_ctr(idb);
    ctr->set_functional_needed = FALSE;
    ctr->set_functional_only = FALSE;
    net_cstate(idb, IDBS_INIT);
    TIMER_START(ctr->init_timer, SPY_MAX_INIT_TIME);
    (void)send_trip_init_cmd(idb, (idb->tr_ring_speed == 16 ? 1 : 0));
    ctr->init_phase = INIT_INIT;
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
    reset_interrupt_level(level);
    return(TRUE);		/* Always a success */
}

/*
 * cbus_token_shutdown
 * 
 * Cause the interface to completely shut up.
 * Newstate is advisory information on what state to mark the idb.
 *
 * On exit the only allowable state is either ADMINDOWN or RESET.
 * Interrupts are disabled because we do the hard reset prior to
 * closing the gate.  Has to be this way.
 */

void cbus_token_shutdown (hwidbtype *idb, int newstate)
{
    leveltype level;

    idb->counters.transitions++;
    level = raise_interrupt_level(NETS_DISABLE);

    holdq_reset(idb);

    (void)select_if(idb);
    (void)send_if_disable_cmd(idb);

    if (idb->state != IDBS_ADMINDOWN) {
	if (newstate != IDBS_ADMINDOWN)
	    net_cstate(idb, IDBS_RESET);
	else net_cstate(idb,IDBS_ADMINDOWN);
    }
    reset_interrupt_level(level);
    return;
}

/*
 * cbus_token_periodic
 *
 * Called from periodic background task to send a Token Ring loopback
 * packet.  Check if we've been able to receive our own loopback packets
 * recently and adjust line protocol appropriately.  Note that we can't
 * just fiddle idb->state since it could be mistakenly reset by interrupt
 * level hardware.
 */
void cbus_token_periodic (hwidbtype *idb)
{
    boolean newstate, oldstate;
    spyglass_data_t *ctr;
    leveltype level;
    
    if (idb->state == IDBS_ADMINDOWN || idb->state == IDBS_RESET)
	return;
 
    /* 
     * Poll the interface for any errors.
     */
    if (idb->state == IDBS_UP) {
	level = raise_interrupt_level(NETS_DISABLE);
	select_if(idb);
	ctr_read_spyglass_errors(idb);
	reset_interrupt_level(level);
    }

    /* If initializing give the interface 90 seconds to respond else shut
     * it down.  The user will have to do a "no shut" to get the system to
     * try again.  */

    if (idb->state == IDBS_INIT) {
        ctr = (spyglass_data_t *)idb->ctr_private;
        if (AWAKE(ctr->init_timer)) {
            errmsg(&msgsym(CTRRSET, CBUS), idb->hw_namestring);
            TIMER_STOP(ctr->init_timer);
            (*idb->shutdown)(idb, IDBS_RESET);
        }
        return;
    }   

    if (idb->nokeepalive)
        return;

    tr_send_loop(idb, idb->hardware);
    if (TIMER_RUNNING(idb->mci_lineuptimer)) {
	newstate = CLOCK_IN_INTERVAL(idb->mci_lineuptimer,
				     2*ONESEC*idb->keep_period);
	if (!newstate)
	    TIMER_STOP(idb->mci_lineuptimer);
    } else {
	newstate = FALSE;
    }
    oldstate = idb->mci_lineup ? TRUE : FALSE;
    if (newstate != oldstate) {
	if (newstate) {
	    idb->mci_lineup = MCI_KEEP_COUNT;	/* Just came up */
	    idb->reason = "Keepalive OK";
	    reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
	    if (tokenring_debug)
	      buginf("\n%s: Keepalive OK", idb->hw_namestring);
	} else if (--idb->mci_lineup <= 0) {
	    if (idb->status & IDB_MCI) {
		cbus_restarts_lineup++;

		/*
		 * When a port dies, restart entire switching complex (Ugh!)
		 */
		platform_demand_complex_restart(idb);
	    }
            else {  /* for interface type other then MCI */
                if (tokenevent_debug || tokenring_debug)
                      buginf("\nTR%d: Keepalive failed, resetting", idb->unit);
                (*idb->reset)(idb);
            }
            if (idb->failCause == 0)  /* Keep alive shouldn't overwrite real
					 cause */
	        idb->failCause = TR_KeepAliveFailure;
	}
    } else if (newstate)
        idb->mci_lineup = MCI_KEEP_COUNT;	/* Up, and stayed up */
}



/*
 * Used by the tr mib to collect stats.
 */
void ctr_get_mib_stats (hwidbtype *idb)
{

    spyglass_data_t *ctr;
    spyg_parms_t *smt;
    leveltype level;

    ctr = idb->ctr_private;
    smt = (spyg_parms_t *)&ctr->smt;

    if (idb->state == IDBS_UP) {
        level = raise_interrupt_level(NETS_DISABLE);
        select_if(idb);
        ctr_read_spyglass_errors(idb);
        ctr_read_spyglass_smt(idb);
        reset_interrupt_level(level);
    }

}



/*************************************************************************
 *
 * ctr_show_controller ()
 * 
 * support for show controller.
 *
 *************************************************************************/

static char *const init_state_strings[] = {
    "", "resetting board", "testing hardware", "enabling board", "opening", ""
    };

boolean ctr_show_controller (hwidbtype *idb, parseinfo *csb)
{
    uchar version_string[MAC_VERS_LEN+1];
    spyglass_data_t *ctr;
    int version_number;
    spyg_parms_t *smt;
    leveltype level;
    boolean alive;
    tokensb *tsb;

    ctr = idb->ctr_private;
    smt = (spyg_parms_t *)&ctr->smt;
    tsb = idb_get_hwsb_inline(idb, HWIDB_SB_TOKEN);
    if (idb->state == IDBS_UP) {
	level = raise_interrupt_level(NETS_DISABLE); 
	select_if(idb);
	ctr_read_spyglass_errors(idb);
	ctr_read_spyglass_smt(idb);
	reset_interrupt_level(level);
    }

    printf("\n%s: state %s", idb->hw_namestring, print_hwidbstate(idb));
    if (idb->state == IDBS_INIT)
	printf(", %s", init_state_strings[ctr->init_phase]);
    printf("\n  current address: %e, burned in address: %e",
	   idb->hardware, idb->bia);

    print_ring_status(idb);

    /*
     * Print out monitor state.  This includes both static state which
     * is gotten on the way up and dynamic state which is gotten from a
     * running controller.  If the controller isn't running the dynamic
     * information we would have gotten is not interesting.  This structure
     * lends itself to the get_status (dynamic) and get_config (static)
     * scheme of things but alas get_config wasn't built early enough
     * so for backward compatitbility reasons the static stuff that is
     * already on the status block will stay there.
     */
    level = raise_interrupt_level(NETS_DISABLE); 
    select_if(idb);
    alive = (platform_trip_anybody_out_there(idb) == FCI_RSP_OKAY);
    reset_interrupt_level(level);

    printf("\n\n  Monitor state: (%sactive)",(alive ? "" : "not "));
    version_number = GETLONG((ulong *)ctr->spyglass_rev) >> 8;
    bcopy(ctr->spyglass_rev+3, version_string, MAC_VERS_LEN-3);
    version_string[MAC_VERS_LEN-3] = '\0';
    sys_ebcdic_to_ascii(version_string, MAC_VERS_LEN-3);
    printf(", chip f/w: '%06x.%s', [bridge capable]", version_number,
	   version_string);
    printf("\n    ring mode: %x", idb->tr_ring_mode);
    if (ctr->addrs.functional) {
	printf(", internal enables: ");
	if (ctr->addrs.functional & TRFUNC_BRIDGE)
	  printf(" SRB");
	if (ctr->addrs.functional & TRFUNC_REM)
	  printf(" REM");
	if (ctr->addrs.functional & TRFUNC_RPS)
	  printf(" RPS");
	if (ctr->addrs.functional & TRFUNC_CRS)
	  printf(" CRS/NetMgr");
    }
    printf("\n    internal functional: %08x (%08x), group: %08x (%08x)",
	   ctr->addrs.functional, tsb->tr_functional,
	   *(ulong *)(&ctr->addrs.group[2]) & ~0x80000000,
	   tsb->tr_group & ~0x80000000);
    printf("\n    internal addrs: SRB: %04x, ARB: %04x, EXB %04x, MFB: %04x",
	   ctr->srb_addr, ctr->arb_addr, ctr->expressb_addr,
	   ctr->mac_tcb_addr);
    printf("\n                    Rev: %04x, Adapter: %04x, Parms %04x",
	   ctr->revision_addr, ctr->adapter_addr, ctr->parms_addr);

    /*
     * All the subtractions are because these counters are shared with
     * show interface.  We can't just zero the numbers when we drop them
     * into the previous cells like all the other statistics because that
     * screws up the "show interface" output.
     */
    printf("\n    Microcode counters:");
    printf("\n      MAC giants %d/%d, MAC ignored %d/%d",
	   ctr->mac_giants - ctr->prev_mac_giants, ctr->prev_mac_giants,
	   ctr->mac_ignored - ctr->prev_mac_ignored, ctr->prev_mac_ignored);
    printf("\n      Input runts %d/%d, giants %d/%d, overrun %d/%d",
	   idb->counters.runts - ctr->prev_runts, ctr->prev_runts,
	   idb->counters.giants - ctr->prev_giants, ctr->prev_giants,
	   idb->counters.input_overrun - ctr->prev_input_overrun,
	   ctr->prev_input_overrun);
    printf("\n      Input ignored %d/%d, parity %d/%d, RFED %d/%d",
	   idb->counters.input_resource - ctr->prev_input_resource,
	   ctr->prev_input_resource,
	   idb->counters.parity - ctr->prev_parity, ctr->prev_parity,
	   ctr->rx_rfed - ctr->prev_rx_rfed, ctr->prev_rx_rfed);
    printf("\n      Input REDI %d/%d, null rcp %d/%d, recovered rcp %d/%d",
	   ctr->rx_redi - ctr->prev_rx_redi, ctr->prev_rx_redi,
	   ctr->rx_null_rcp - ctr->prev_rx_null_rcp, ctr->prev_rx_null_rcp,
	   ctr->rx_recover_rcp - ctr->prev_rx_recover_rcp,
	   ctr->prev_rx_recover_rcp);
    printf("\n      Input implicit abort %d/%d, explicit abort %d/%d",
	   ctr->rx_implicit_abort - ctr->prev_rx_implicit_abort,
	   ctr->prev_rx_implicit_abort,
	   ctr->rx_explicit_abort - ctr->prev_rx_explicit_abort,
	   ctr->prev_rx_explicit_abort);
    printf("\n      Output underrun %d/%d, tx parity %d/%d, null tcp %d/%d",
	   idb->counters.output_underrun - ctr->prev_output_underrun,
	   ctr->prev_output_underrun,
	   ctr->tx_dma_parity - ctr->prev_tx_dma_parity,
	   ctr->prev_tx_dma_parity,
	   ctr->tx_next_buffer - ctr->prev_tx_next_buffer,
	   ctr->prev_tx_next_buffer);
    printf("\n      Output SFED %d/%d, SEDI %d/%d, abort %d/%d",
	   ctr->tx_sfed - ctr->prev_tx_sfed, ctr->prev_tx_sfed,
	   ctr->tx_sedi - ctr->prev_tx_sedi, ctr->prev_tx_sedi,
	   ctr->tx_strip_abort - ctr->prev_tx_strip_abort,
	   ctr->prev_tx_strip_abort);
    printf("\n      Output False Token %d/%d, PTT Expired %d/%d",
	   ctr->tx_false_token - ctr->prev_tx_false_token,
	   ctr->prev_tx_false_token,
	   ctr->tx_ptt_expired - ctr->prev_tx_ptt_expired,
	   ctr->prev_tx_ptt_expired);

    printf("\n    Internal controller counts:");
    printf("\n      line errors: %d/%d,  internal errors: %d/%d",
	   ctr->err_line, tsb->prev_err_line,
	   ctr->err_internal, tsb->prev_err_internal);
    printf("\n      burst errors: %d/%d,  ari/fci errors: %d/%d",
	   ctr->err_burst, tsb->prev_err_burst,
	   ctr->err_arifci, tsb->prev_err_arifci);
    printf("\n      abort errors: %d/%d, lost frame: %d/%d",
	   ctr->err_abort, tsb->prev_err_abort,
	   ctr->err_lostframe, tsb->prev_err_lostframe);
    printf("\n      copy errors: %d/%d, rcvr congestion: %d/%d",
	   ctr->err_copy, tsb->prev_err_copy,
	   ctr->err_rcvcongestion, tsb->prev_err_rcvcongestion);
    printf("\n      token errors: %d/%d, frequency errors: %d/%d",
	   ctr->err_token, tsb->prev_err_token,
	   ctr->err_freq, tsb->prev_err_freq);
    ctr_update_prev_stats(idb);

    printf("\n    Internal controller smt state:"
	   "\n      Adapter MAC:     %e, Physical drop:     %08x"
	   "\n      NAUN Address:    %e, NAUN drop:         %08x"
	   "\n      Last source:     %e, Last poll:         %e",
	   ctr->addrs.mac, smt->physdrop, smt->una, GETLONG(&smt->una_physdrop),
	   smt->last_src, smt->last_poll);
    printf("\n      Last MVID:       %04x,           Last attn code:    %04x"
	   "\n      Txmit priority:  %04x,           Auth Class:        %04x"
	   "\n      Monitor Error:   %04x,           Interface Errors:  %04x",
	   smt->last_major_vector, smt->last_attn, smt->txmt_pri,
	   smt->auth_class, smt->monitor_error_code,
	   smt->ring_xfc_errors);
    printf("\n      Correlator:      %04x,           Soft Error Timer:  %04x"
	   "\n      Local Ring:      %04x,           Ring Status:       %04x"
	   "\n      Beacon rcv type: %04x,           Beacon txmit type: %04x",
	   smt->frame_corr, smt->soft_err_timer, smt->local_ring,
	   smt->ring_status, smt->beacon_rcv_type,
	   smt->beacon_txmt_type);
    printf("\n      Beacon type:     %04x,           Beacon NAUN:       %e"
	   "\n      Beacon drop:     %08x,       Reserved:          %04x"
	   "\n      Reserved2:       %04x", smt->last_beacon_type,
	   smt->beacon_una, GETLONG(&smt->beacon_una_physdrop),
	   *(ushort *)smt->rsrvd, *(ushort *)smt->rsrvd2);

    return automore_quit();
}

/*
 * ctr_check_memb
 * Worker routine for deciding if a TRIP interface is still alive.  
 * Must be called with network interrupts disabled!
 */
static boolean ctr_check_memb (hwidbtype *idb)
{
    spyglass_data_t *ctr;
    int i;
    ushort result;
    ushort dont_care;

    ctr = idb->ctr_private;

    select_if(idb);
    send_trip_memb_select_cmd(idb, ctr->srb_addr);

    for (i = 0; i < 20; i++) {
	result = send_trip_memb_cont_read_cmd(idb, &dont_care);
        if (result == FCI_RSP_OKAY) 
            return(TRUE);
    }
    return(FALSE);
}

/*
 * ctr_deadman_poll
 * Ersatz keepalive timer for making sure the microcode or hardware hasn't
 * hung.  
 */
void ctr_deadman_poll (hwidbtype *idb)
{
    leveltype level;
    ushort sresult;
    tokensb *tsb;

    if ((idb->type == IDBTYPE_CTR) && !is_vip(idb) &&
	(idb->state == IDBS_UP)) { 
	level = raise_interrupt_level(NETS_DISABLE);
	tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
        if (tsb->tr_polling_state == TR_POLLING_OK) {
            if (--tsb->tr_polling_count <= 0) {
                tsb->tr_polling_count = TR_POLLING_PERIOD;
		select_if(idb);
		ctr_read_spyglass_errors(idb);
		sresult = platform_trip_anybody_out_there(idb);
		if (sresult != FCI_RSP_OKAY) {
		    if (sresult == FCI_RSP_ERR_TMOUT) { 
			if (tokenevent_debug)
			    buginf("\n%s encountered initial polling timeout",
				   idb->hw_namestring);
			tsb->tr_polling_state = TR_POLLING_TMOUT;
		    }
		}
            }
        } else if (tsb->tr_polling_state == TR_POLLING_TMOUT) {
            if (ctr_check_memb(idb)) {
		if (tokenevent_debug)
		    buginf("\n%s resumed normal polling", idb->hw_namestring);
                tsb->tr_polling_count = TR_POLLING_PERIOD;
                tsb->tr_polling_state = TR_POLLING_OK;
                tsb->tr_polling_failure = 0;
            } else if (++tsb->tr_polling_failure >= 3) {
                ctr_polling_failed(idb);
            }
        }
	idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
	reset_interrupt_level(level);
    }
}

/*****************************************************************************
 * Name: get_tokenring_cbus_errors
 *
 * Description: Accumulates cBus errors from tokenring interfaces
 *   Called by get_cbus_errors
 *
 * Assumptions:
 *   1. interrupts are disabled
 *   2. interface has been selected
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void get_tokenring_cbus_errors (hwidbtype *hwidb)
{
    long counts;
    spyglass_data_t *ctr;

    ctr = hwidb->ctr_private;

    counts = send_errors_cmd(hwidb, FCI_ERR_NOBUFFERS);

    if (is_vip(hwidb)) 
	return;

    hwidb->counters.input_resource += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_CRC);
    hwidb->counters.input_crc += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_FRAME);
    hwidb->counters.input_frame += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_GIANTS);
    hwidb->counters.giants += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_RUNT);
    hwidb->counters.runts += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_RX_OVERRUN);
    hwidb->counters.input_overrun += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_TX_UNDERFLOW);
    hwidb->counters.output_underrun += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_MAC_GIANTS);
    ctr->mac_giants += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_MAC_IGNORED);
    ctr->mac_ignored += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_PARITY);
    hwidb->counters.parity += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_RX_RFED);
    ctr->rx_rfed += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_RX_REDI);
    ctr->rx_redi += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_RX_NULL_RCP);
    ctr->rx_null_rcp += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_RX_RECOVER_RCP);
    ctr->rx_recover_rcp += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_RX_EXPLICIT_ABORT);
    ctr->rx_explicit_abort += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_RX_IMPLICIT_ABORT);
    ctr->rx_implicit_abort += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_TX_SFED);
    ctr->tx_sfed += counts;
    hwidb->counters.output_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_TX_SEDI);
    ctr->tx_sedi += counts;
    hwidb->counters.output_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_TX_FALSE_TOKEN);
    ctr->tx_false_token += counts;
    hwidb->counters.output_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_TX_DMA_PARITY);
    ctr->tx_dma_parity += counts;
    hwidb->counters.output_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_TX_NEXT_BUFFER);
    ctr->tx_next_buffer += counts;
    hwidb->counters.output_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_TX_STRIP_ABORT);
    ctr->tx_strip_abort += counts;
    hwidb->counters.output_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_TX_PTT_EXPIRED);
    ctr->tx_ptt_expired += counts;
    hwidb->counters.output_error += counts;
}


static void trip_subsys_init (subsystype* subsys)
{
    reg_add_cbus_get_ip_errors(FCI_TYPE_CTR, get_tokenring_cbus_errors,
			       "get_tokenring_cbus_errors");
    trip_platform_init(subsys);
}

/*---------------------------------------------------------------------------
 * Subsystem Header for TRIP Driver
 *---------------------------------------------------------------------------*/
SUBSYS_HEADER(trip,			/* name */
	      1,			/* major version */
	      0,			/* minor version */
	      1,			/* edit version */
	      trip_subsys_init,		/* init function */
	      SUBSYS_CLASS_DRIVER,	/* subsystem class */
	      "seq: cbus_mci",  	/* sequence list */
	      "req: cbus_mci"   	/* prerequisite list */
	      );



      
