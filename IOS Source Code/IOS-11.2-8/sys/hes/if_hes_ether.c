/* $Id: if_hes_ether.c,v 3.13.10.11 1996/08/28 12:44:57 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_hes_ether.c,v $
 *------------------------------------------------------------------
 * hes_ether.c -- high end systems ethernet code (EIP, MEC, MCI ethernet)
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_hes_ether.c,v $
 * Revision 3.13.10.11  1996/08/28  12:44:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.10.10  1996/07/26  01:09:58  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.13.10.9  1996/07/17  21:34:56  wilber
 * CSCdi62840:  line protocol goes down on FE when irb is configured
 * Should populate every subinterface SMF, not just the main interface
 * Branch: California_branch
 *
 * Revision 3.13.10.8  1996/07/08  22:25:29  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.13.10.7  1996/06/07  19:03:28  kramling
 * CSCdi55236:  Problems with FEIP OIR
 * Branch: California_branch
 *
 * Revision 3.13.10.6  1996/06/05  05:08:17  kao
 * CSCdi53846:  EOIR of EIP wipes route cache of entries for that interface
 * No cache invalidation if reset during EOIR
 * Branch: California_branch
 *
 * Revision 3.13.10.5  1996/05/22  13:51:11  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.13.10.4  1996/05/21  09:47:53  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.13.10.3  1996/05/14  14:40:03  kramling
 * CSCdi56688:  Netbios multicast frame is not forwarding on Fast Ethernet.
 * Branch: California_branch
 *
 * Revision 3.13.10.2  1996/05/09  14:28:51  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.13.10.1.18.1  1996/04/27  07:02:23  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.13.10.1.8.1  1996/04/08  01:46:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.13.10.1  1996/03/18  19:41:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.13.12.1  1996/03/22  09:36:30  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.13  1996/02/22  16:58:35  kramling
 * CSCdi48646:  System restarted by error - a SegV exception, PC
 * 0x6049F09C
 * Transparent bridging and HSRP are incompatible over FastEthernet
 * interfaces
 *
 * Revision 3.12.8.1  1996/03/05  06:11:23  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.12.4.1  1996/02/27  20:42:05  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.12  1996/02/01  06:02:37  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.11.6.1  1996/01/24  21:46:42  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.11  1996/01/11  03:50:22  kao
 * CSCdi43878:  IS_VIP() macro should be used
 *
 * Revision 3.10.2.1  1996/02/15  19:13:05  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.10  1995/12/19  20:41:15  kramling
 * CSCdi45854:  High end FE and ETH interfaces not selected properly for
 * EOIR
 *
 * Revision 3.9  1995/12/17  18:27:22  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.8  1995/12/17  06:02:56  mmcneali
 * CSCdi43825:  No fastswitching for 802.10 vLAN packets.
 *
 * Revision 3.7  1995/12/15  09:18:44  ogrady
 * CSCdi39140:  Multicast fastswitching doesnt work on an RSP box.
 * Initialize FEIP to not turn on the DOD_IP_PROTOCOL address filter bit
 * for multicast frames.
 *
 * Revision 3.6  1995/12/02  06:55:28  tgrennan
 * CSCdi42449:  4/8e address filter improperly set by RP
 *
 * 4/8e address filter works like FE and FEIP instead of EIP/MCI.
 * Also, split large switch in cbus_mci_idb_init() into sub-functions.
 *
 * Revision 3.5  1995/11/30  07:16:37  tboyle
 * CSCdi44067:  No 4E/FE line up or protocol up after booting/or flaky
 *
 * Revision 3.4  1995/11/20  22:24:22  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  09:18:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:00:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/14  16:01:41  kramling
 * CSCdi38439:  Error in porting feip support to 11.0
 * Call cbus_fast_ethernet_address() even if bridging isn't configured.
 *
 * Revision 2.3  1995/08/09  00:19:51  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.2  1995/07/11  00:51:25  ogrady
 * CSCdi34908:  Process level R/SRB hangs interface (orig AT over SRB
 * fails).  Don't make fastout vector return_false to disable srb fast
 * switching since its called from srb_common() path.  Set AC byte to
 * AC_PRI4 for all srb output paths.  Don't count ignored MAC packets as
 * drops.  Ongoing SRB counter clean up.
 *
 * Revision 2.1  1995/06/07  20:41:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#include "master.h"
#include "../ui/common_strings.h"
#include "config.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../hes/cbus_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "../if/ether.h"
#include "../if/ether_private.h"
#include "../if/network.h"
#include "if_hes_common.h"
#include "if_fci.h"
#include "logger.h"
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"
#include "if_cmd.h"
#include "if_hes_ether.h"
#include "if_feip.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../if/linkdown_event.h"
#include "if_mci.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "msg_cbus.c"			/* Not a typo, see logger.h */

#include "../smf/smf_public.h"
#include "../ui/debug.h"
#include "../tbridge/tbridge_debug.h"

#include "../vlan/vlan.h"



/*
 * cbus_ether_address
 * Handle the extended address filter for MEC style interfaces.
 */
static void cbus_ether_address (hwidbtype *idb)
{
    ushort data, mask, mac_mask;
    int indx;
    idbtype *swidb;

    /*
     * Bail out quickly if not a cBus ethernet interface.
     */
    if (!(idb->status & IDB_CBUS))
	return;

    /*
     * This code assumes that the first 6 bytes of the address filter
     * has aleady been set up. Setup  octets 6-31.
     */
    mask = mac_mask = 0;

    if (bridge_enable && idb->tbridge_on_hwidb) {
	mac_mask = (FCI_BRIDGING_MASK << 8);  	/* bridging mask */

	/*
	 * Notify the cBus controller of the bridging bit.
	 */
	(void)send_bridge_cmd(idb, mac_mask);
    }

    /*
     * Mask of the FSMs pertaining to MAC address filtering (as opposed to
     * protocol filtering).
     */
    mac_mask |= ((FCI_STATION_MASK | FCI_MULTICAST_MASK |
		  FCI_HSRP_STATION_MASK) << 8);

    /*
     * Set up the protocol portion of the address mask.
     * Do NOT program the AF in this way if CRB is enabled
     * and the interface is configured for transparent
     * bridging.
     */
    if (!(global_crb_enable && idb->tbridge_on_hwidb)) {
	FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	    if (RUNNING_IF(LINK_IP, swidb))
		mask |= (FCI_DOD_IP_PROTOCOL_MASK << 8);
	    if (RUNNING_IF(LINK_DECNET, swidb))
		mask |= (FCI_DECNET_PROTOCOL_MASK << 8);
	    if (RUNNING_IF(LINK_CLNS, swidb))
		mask |= (FCI_CLNS_PROTOCOL_MASK << 8);
	    if (RUNNING_IF(LINK_XNS, swidb))
		mask |= (FCI_XNS_PROTOCOL_MASK << 8);
	}
    }

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
    if (tbridge_debug_crb)
	buginf("\nTB-CRB %s MCI_CMD_AF_WRITE %04x", idb->hw_namestring, mask);
#endif  /* TBRIDGE_DEBUG */

    /*
     * Proceed to write the address filter.
     */
    for (indx = 0; indx < (256 * 32); indx++) {
	(void)send_af_select_cmd(idb, indx);
	(void)send_af_read_cmd(idb, &data);
	if (indx < (256 * 6)) {
	    if (data & ((FCI_STATION_MASK | FCI_HSRP_STATION_MASK) << 8)) {
		data |= mask;
	    } else if (idb->tbridge_on_hwidb)
		data |= (FCI_BRIDGING_MASK << 8);
	} else if (indx >= (256 * 12) && (indx < (256 * 14))) {
	    data = mac_mask;
	} else {
	    data = mask | mac_mask;
	}
	(void)send_af_select_cmd(idb, indx);
	(void)send_af_write_cmd(idb, data);
    }

    /*
     * The address filter has been set up -- except for the Ethertype field.
     */
    if (mask & (FCI_DOD_IP_PROTOCOL_MASK << 8)) {
	(void)send_af_select_cmd(idb, ((256 * 12) + 0x08));
	(void)send_af_read_cmd(idb, &data);
	data |= (FCI_DOD_IP_PROTOCOL_MASK << 8);
	(void)send_af_select_cmd(idb, ((256 * 12) + 0x08));
	(void)send_af_write_cmd(idb, data);
	(void)send_af_select_cmd(idb, ((256 * 13) + 0x00));
	(void)send_af_read_cmd(idb, &data);
	data |= (FCI_DOD_IP_PROTOCOL_MASK << 8);
	(void)send_af_select_cmd(idb, ((256 * 13) + 0x00));
	(void)send_af_write_cmd(idb, data);

	/*
	 * Clear IP protocol bit in the 14th octet except for 14-45.
	 */
	for (indx = (256 * 14); indx < (256 * 15); indx++) {
	    if (indx == (256 * 14) + 0x45)
		continue;
	    (void)send_af_select_cmd(idb, indx);
	    (void)send_af_read_cmd(idb, &data);
	    data &= ~(FCI_DOD_IP_PROTOCOL_MASK << 8);
	    (void)send_af_select_cmd(idb, indx);
	    (void)send_af_write_cmd(idb, data);
	}
	/*
	 * Clear IP protocol bit in the 22nd octet (TTL) for 00 and 01.
	 */
	for (indx = (256 * 22); indx < (256 * 22) + 2; indx++) {
	    (void)send_af_select_cmd(idb, indx);
	    (void)send_af_read_cmd(idb, &data);
	    data &= ~(FCI_DOD_IP_PROTOCOL_MASK << 8);
	    (void)send_af_select_cmd(idb, indx);
	    (void)send_af_write_cmd(idb, data);
	}

	/*
	 * Do not recognize IP destination address 255.xx.xx.xx.
	 */
	(void)send_af_select_cmd(idb, ((256 * 30) + 0xff));
	(void)send_af_read_cmd(idb, &data);
	data &= ~(FCI_DOD_IP_PROTOCOL_MASK << 8);
	(void)send_af_select_cmd(idb, ((256 * 30) + 0xff));
	(void)send_af_write_cmd(idb, data);
    }
    if (mask & (FCI_DECNET_PROTOCOL_MASK << 8)) {
	(void)send_af_select_cmd(idb, ((256 * 12) + 0x60));
	(void)send_af_read_cmd(idb, &data);
	data |= (FCI_DECNET_PROTOCOL_MASK << 8);
	(void)send_af_select_cmd(idb, ((256 * 12) + 0x60));
	(void)send_af_write_cmd(idb, data);
	(void)send_af_select_cmd(idb, ((256 * 13) + 0x03));
	(void)send_af_read_cmd(idb, &data);
	data |= (FCI_DECNET_PROTOCOL_MASK << 8);
	(void)send_af_select_cmd(idb, ((256 * 13) + 0x03));
	(void)send_af_write_cmd(idb, data);

	/*
	 * Clear DECnet protocol bit -- if padding with not one 1 byte.
	 */
	for (indx = ((256 * 16) + 0x80); indx < (256 * 17); indx++) {
	    if (indx == ((256 * 16) + 0x81))
		continue;
	    (void)send_af_select_cmd(idb, indx);
	    (void)send_af_read_cmd(idb, &data);
	    data &= ~(FCI_DECNET_PROTOCOL_MASK << 8);
	    (void)send_af_select_cmd(idb, indx);
	    (void)send_af_write_cmd(idb, data);
	}
    }
    /*
     * If routing CLNS set dsap/lsap to FEFE in octets 14 and 15.
     */
    if (mask & (FCI_CLNS_PROTOCOL_MASK << 8)) {
	(void)send_af_select_cmd(idb, ((256 * 14) + 0xFE));
	(void)send_af_read_cmd(idb, &data);
	data |= (FCI_CLNS_PROTOCOL_MASK << 8);
	(void)send_af_select_cmd(idb, ((256 * 14) + 0xFE));
	(void)send_af_write_cmd(idb, data);
	(void)send_af_select_cmd(idb, ((256 * 15) + 0xFE));
	(void)send_af_read_cmd(idb, &data);
	data |= (FCI_CLNS_PROTOCOL_MASK << 8);
	(void)send_af_select_cmd(idb, ((256 * 15) + 0xFE));
	(void)send_af_write_cmd(idb, data);

	/*
	 * Need to setup the length field i.e. octets 12 and 13.
	 */
    }

    /*
     * If routing XNS set type 0600 in octets 12 and 13.
     */
    if (mask & (FCI_XNS_PROTOCOL_MASK << 8)) {
 	(void)send_af_select_cmd(idb, ((256 * 12) + 0x06));
 	(void)send_af_read_cmd(idb, &data);
	data |= (FCI_XNS_PROTOCOL_MASK << 8);
 	(void)send_af_select_cmd(idb, ((256 * 12) + 0x06));
 	(void)send_af_write_cmd(idb, data);
 	(void)send_af_select_cmd(idb, ((256 * 13) + 0x00));
 	(void)send_af_read_cmd(idb, &data);
	data |= (FCI_XNS_PROTOCOL_MASK << 8);
 	(void)send_af_select_cmd(idb, ((256 * 13) + 0x00));
 	(void)send_af_write_cmd(idb, data);

	/*
	 * In case of XNS 18th octet contains hope count
	 * Exclude packets with hope count 15 and greater.
	 */
	for (indx = (256 * 18) + 0x0F; indx < (256 * 19); indx++) {
  	    (void)send_af_select_cmd(idb, indx);
  	    (void)send_af_read_cmd(idb, &data);
  	    data &= ~(FCI_XNS_PROTOCOL_MASK << 8);
  	    (void)send_af_select_cmd(idb, indx);
 	    (void)send_af_write_cmd(idb, data);
	}

	/*
	 * Octets 24 to 29 in XNS packet represent destnation address
	 * Exclude xns broadcast address (FF.xx.xx.xx.xx.xx)
	 */
  	send_af_select_cmd(idb, ((256 * 24) + 0xff));
  	(void)send_af_read_cmd(idb, &data);
  	data &= ~(FCI_XNS_PROTOCOL_MASK << 8);
  	send_af_select_cmd(idb, ((256 * 24) + 0xff));
 	(void)send_af_write_cmd(idb, data);
    }
}


/*
 * cbus_mci_ether_range
 * Set filter for a particular Ethernet address range.
 *
 * Warning: does not call self_station, an explicit check in bridge
 *          receive, or a manual call to self_station is necessary.
 */
static void cbus_mci_ether_range (hwidbtype *idb, int fsm,
				  uchar *addr, uchar *mask)
{
    int		i, j;
    ushort	data, newdata;
    
    newdata = ((1 << fsm) << 8);		/* compose FSM bit to set */

    /*
     * Check for multicasts:
     */
    if ((*addr & ~*mask) & 0x01)		/* check for multicast */
	idb->multibits |= newdata;		/* remember multicast */

    for (i = 0; i < IEEEBYTES; i++, addr++, mask++) {
	if (*mask) { 	/* sigh, gotta loop through ... */
	    for (j = 0; j < 255; j++)
		if ((j & ~(*mask)) == *addr) {
		    (void)send_af_select_cmd(idb, ((i << 8) | j));
		    (void)send_af_read_cmd(idb, &data); /* read data */
		    data |= newdata;			/* set bit for FSM */
		    (void)send_af_select_cmd(idb, ((i << 8) | j));
		    (void)send_af_write_cmd(idb, data); /* write new */
		}
	} else {		/* shortcut */
	    (void)send_af_select_cmd(idb, ((i << 8) | *addr));
	    (void)send_af_read_cmd(idb, &data); /* read data */
	    data |= newdata;			/* set bit for FSM */
	    (void)send_af_select_cmd(idb, ((i << 8) | *addr));
	    (void)send_af_write_cmd(idb, data); /* write new */
	}
    }
}


/*
 * cbus_pas_ethernet_address
 * Set filter for a particular (Fast) Ethernet address or range.
 *
 * Nota Bene: explicitly check for the case of the ISL vLAN multicast since
 *            the ucode identifies these packets via the bottom 2 bits of the
 *            2 byte protocol bits being set (i.e. FCI_ISL_VLAN_MASK)!
 *            Necessary so that the ucode can do it's CRC dance, ugly ...
 */

static void cbus_pas_ethernet_address (hwidbtype *idb, int fsm,
				       const uchar *af_address, uchar *mask)
{
    ushort newdata;
    ushort *af_addr;
    ushort *lcl_mask;
    ushort status;
    idbtype *swidb;
    
    if (ieee_equal(af_address, isl_vlan_multicast)) {

	newdata = FCI_ISL_VLAN_MASK;

    } else {

        newdata = ((1 << fsm) << 8);		/* compose FSM bit to set */

	/*
	 * Check for bridging
	 */
	if (bridge_enable && idb->tbridge_on_hwidb) {
	    newdata |= (FCI_BRIDGING_MASK << 8);  	/* bridging mask */
	}

        /*
         * Check for multicasts:
         */

        if ((*af_address & ~*mask) & 0x01) {      /* check for multicast */
  	    idb->multibits |= newdata;		/* remember multicast */
	} else {

            /*
             * Set up the protocol portion of the address mask for
	     * unicast packets only (don't want multicasts to
	     * fall into optimum switching path)
             */

            FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	        if (RUNNING_IF(LINK_IP, swidb))
	            newdata |= (FCI_DOD_IP_PROTOCOL_MASK << 8);
   	        if (RUNNING_IF(LINK_DECNET, swidb))
	            newdata |= (FCI_DECNET_PROTOCOL_MASK << 8);
	        if (RUNNING_IF(LINK_CLNS, swidb))
	            newdata |= (FCI_CLNS_PROTOCOL_MASK << 8);
	        if (RUNNING_IF(LINK_XNS, swidb))
	            newdata |= (FCI_XNS_PROTOCOL_MASK << 8);
            }
	}
    }

    /* 
     * send the new mac addr
     */
    af_addr = (ushort *) af_address;
    status = send_af_write_cmd(idb, *af_addr);
    status = send_af_write_cmd(idb, *(af_addr + 1));
    status = send_af_write_cmd(idb, *(af_addr + 2));
    /* 
     * send the mask
     */
    lcl_mask = (ushort *) mask;
    status = send_af_write_cmd(idb, *lcl_mask);
    status = send_af_write_cmd(idb, *(lcl_mask + 1));
    status = send_af_write_cmd(idb, *(lcl_mask + 2));
    /* 
     * send the mac and protocol bits
     */
    status = send_af_write_cmd(idb, newdata);
}

/*
 * cbus_mci_record_ether_range
 */
void cbus_mci_record_ether_range (idbtype *idb, uchar *address, uchar *mask)
{
    hwidbtype	*hwidb;
    boolean	if_enabled;
    ushort	status;
    leveltype	level;

    hwidb = idb->hwptr;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Note whether or not the interface was enabled when
     * we're called, so that we can preserve this state.
     */
    if_enabled = hwidb->hw_enabled;

    /*
     * Select the interface to get the address from
     */
    (void)select_if(hwidb);

    /*
     * if the interface is enabled, disable it before setting the
     * ether address
     */
    if (if_enabled) {
	status = send_if_disable_cmd_delay(hwidb, IF_DISABLE_TIME);
	if (status != MCI_RSP_OKAY)
	    errmsg(&msgsym(RSETFAIL, CBUS), idb->namestring,
		   "cbus_mci_record_ether_range()", status);
    }

    cbus_mci_ether_range(hwidb, FSM_BROADCAST, address, mask);

    /*
     * if the interface was enabled when we started, we need to
     * re-enable it now
     */
    if (if_enabled) {
	status = send_if_enable_cmd_delay(hwidb, 10);
	if (status != MCI_RSP_OKAY)
	    errmsg(&msgsym(RSETFAIL, CBUS), idb->namestring, 
		   "cbus_mci_record_ether_range()", status);
    }
    reset_interrupt_level(level);
}

/*
 * cbus_record_pas_ethernet_range
 */
void cbus_record_pas_ethernet_range (idbtype *idb, uchar *address, 
				      uchar *mask)
{
    hwidbtype	*hwidb;
    leveltype	level;

    hwidb = idb->hwptr;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Select the interface to get the address from
     */
    (void)select_if(hwidb);

    /*
     * set the address as a broadcast
     */
    cbus_pas_ethernet_address(hwidb, FSM_BROADCAST, address, mask);

    reset_interrupt_level(level);
}

/*****************************************************************************
 * Name: get_ether_address
 *
 * Description: gets the ether address from the indicated interface
 *   If there is an error while getting the address, a partial address
 *   (i.e. only some of the bytes) may be returned.
 *
 * Assumptions:
 *   This function assumes that the caller has
 *     1. disabled interrupts
 *     2. selected the interface
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   address -- pointer to the area to put the address
 *
 * Return Value: None
 *
 *****************************************************************************/
static void get_ether_address (hwidbtype *hwidb, char *address)
{
    ushort i;
    ushort status;

    for (i = 0; i < IEEEBYTES; i++) {
	status = send_station_cmd(hwidb,i,address + i);
	if (status != FCI_RSP_OKAY) {
	    errmsg(&msgsym(RSETFAIL, CBUS), hwidb->hw_namestring, 
		   "get_ether_address()", status);
	    break;
	}
    }
}



/*****************************************************************************
 * Name: get_ethernet_cbus_errors
 *
 * Description: Accumulates cBus errors from ethernet interfaces
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
static void get_ethernet_cbus_errors (hwidbtype *hwidb)
{
    long counts;

    counts = send_errors_cmd(hwidb, FCI_ERR_NOBUFFERS);
    /*
     * The FEIP or any VIP style ethernet interface (100mb/10mb) 
     * get their error statistics via love letters.  A single
     * send_errors_cmd triggers the IP to send the love letter.  Use
     * the above single cbus command as that trigger.
     */
    if ((hwidb->type == IDBTYPE_FEIP) || is_vip(hwidb))
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
    
    counts = send_errors_cmd(hwidb, FCI_ERR_ACQUIRE);
    hwidb->counters.output_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_COLLISIONS);
    hwidb->counters.output_collision += counts;
}

/*
 * Parse chains for serial interface commands
 */
#define ALTERNATE       NONE
#include "cfg_int_tx-burst-deferral.h"
LINK_POINT(cbus_ether_interface_commands, ALTERNATE);
#undef  ALTERNATE


static void eip_subsys_init (subsystype* subsys)
{
    reg_add_cbus_get_ip_errors(FCI_TYPE_ETHER, get_ethernet_cbus_errors,
			       "get_ethernet_cbus_errors");
    reg_add_cbus_get_ip_errors(FCI_TYPE_VIP_FETHER, get_ethernet_cbus_errors,
			       "get_ethernet_cbus_errors");
    reg_add_cbus_get_ip_errors(FCI_TYPE_VIP_FETHER_TX_NISL, get_ethernet_cbus_errors,
			       "get_ethernet_cbus_errors");
    reg_add_cbus_get_ip_errors(FCI_TYPE_VIP_FETHER_TX_ISL, get_ethernet_cbus_errors,
			       "get_ethernet_cbus_errors");
    reg_add_cbus_get_ip_errors(FCI_TYPE_VIP_FETHER_FX_ISL, get_ethernet_cbus_errors,
			       "get_ethernet_cbus_errors");
    reg_add_cbus_get_ip_errors(FCI_TYPE_VIP_ETHER8, get_ethernet_cbus_errors,
			       "get_ethernet_cbus_errors");
    eip_platform_init(subsys);

    /*
     * Initialize hes ether parser support
     */
    parser_add_commands(PARSE_ADD_CFG_INT_AS_CMD,
                  &pname(cbus_ether_interface_commands), "hes_ether");

    return;
}

/*---------------------------------------------------------------------------
 * Subsystem Header for EIP Driver
 *---------------------------------------------------------------------------*/
SUBSYS_HEADER(eip,			/* name */
	      1,			/* major version */
	      0,			/* minor version */
	      1,			/* edit version */
	      eip_subsys_init,		/* init function */
	      SUBSYS_CLASS_DRIVER,	/* subsystem class */
	      "seq: cbus_mci",		/* sequence list */
	      "req: cbus_mci"		/* prerequisite list */
	      );


/*****************************************************************************
 * Name: cbus_mci_ether_address
 *
 * Description: Set filter for a particular Ethernet address
 *
 * Assumptions:
 *   This function assumes that the caller has
 *     1. disabled interrupts
 *     2. selected the interface
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   fsm -- finite state machine constant (defined in if_mci.h).
 *     This is the bit number that is set in the address filter.
 *   af_address -- a pointer to the ethernet address to set
 *
 * Return Value: None
 *
 *****************************************************************************/
static void cbus_mci_ether_address (hwidbtype *hwidb, int fsm,
				    uchar const *af_address)
{
    int i;
    ushort data;
    ushort newdata;
    ushort af_addr;
    ushort status;

    /*
     * compose the FSM (finite state machine) bit to set in the
     * address filter.
     */
    newdata = ((1 << fsm) << 8);

    /*
     * check for multicast address:
     */
    if (*af_address & 0x01) {
	hwidb->multibits |= newdata;
    }

    /*
     * for each byte in the ether address we update the address filter.
     * The operation is basically read/modify/write.
     */
    for (i = 0; i < IEEEBYTES; i++, af_address++) {
	/* 
	 * select the address
         */
	af_addr = (i << 8) | *af_address;
	status = send_af_select_cmd(hwidb,af_addr);
	if (status != FCI_RSP_OKAY)
	    break;

	/* 
	 * read the address filter data
         */
	status = send_af_read_cmd(hwidb,&data);
	if (status != FCI_RSP_OKAY)
	    break;

	/* 
	 * modify the address filter data (add new bit to data)
         */
	data |= newdata;

	/* 
	 * select the address (again because AF_WRITE is auto increment)
         */
	status = send_af_select_cmd(hwidb,af_addr);
	if (status != FCI_RSP_OKAY)
	    break;

	/* 
	 * write the new value back to the address filter
         */
	status = send_af_write_cmd(hwidb,data);
	if (status != FCI_RSP_OKAY)
	    break;
    }
}


/*****************************************************************************
 * Name: cbus_mci_record_ether_address
 *
 * Description: Sets the interface to listen to the address specified.
 *   This function is used in the "hwidb->listen" vector in the IDB for
 *    ethernet interfaces
 *
 * Input: 
 *   idb -- pointer to the software IDB for the interface
 *   address -- pointer to the ethernet address to "record"
 *   fsm     -- FSM mask to be used
 *
 * Return Value: None
 *
 *****************************************************************************/
void cbus_mci_record_ether_address (idbtype *idb, uchar *address, uchar fsm)
{
    hwidbtype	*hwidb;
    boolean	if_enabled;
    ushort	status;
    leveltype	level;

    hwidb = idb->hwptr;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Note whether or not the interface was enabled when
     * we're called, so that we can preserve this state.
     */
    if_enabled = hwidb->hw_enabled;

    /*
     * Select the interface to get the address from
     */
    select_if(hwidb);

    /*
     * if the interface is enabled, disable it before setting the
     * ether address
     */
    if (if_enabled) {
	status = send_if_disable_cmd_delay(hwidb, IF_DISABLE_TIME);
	if (status != MCI_RSP_OKAY)
	    errmsg(&msgsym(RSETFAIL, CBUS), idb->namestring,
		   "cbus_mci_record_ether_address()", status);
    }

    /*
     * set the ether address as a broadcast
     */
    cbus_mci_ether_address(hwidb, fsm, address);

    /*
     * if the interface was enabled when we started, we need to
     * re-enable it now
     */
    if (if_enabled) {
	status = send_if_enable_cmd_delay(hwidb, 10);
	if (status != MCI_RSP_OKAY)
	    errmsg(&msgsym(RSETFAIL, CBUS), idb->namestring, 
		   "cbus_mci_record_ether_address()", status);
    }
    reset_interrupt_level(level);
}


/*****************************************************************************
 * Name: pas_ethernet_reset_bridging
 *
 * Description: resets bridging in an ordered/sorted (fast) ethernet
 *              address filter
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 * Assumptions:
 *   1. the interface has been selected
 *   2. the interrupt level has been raised
 *
 *****************************************************************************/
static void pas_ethernet_reset_bridging (hwidbtype *hwidb)
{
    ushort filter;
    ushort status;

    if (hwidb->tbridge_on_hwidb) {
	if (bridge_enable) {
	    filter = (FCI_BRIDGING_MASK << 8);
	    status = send_bridge_cmd(hwidb,filter);
	    /* 
	     * send the new mac addr
	     */
	    status = send_af_write_cmd(hwidb,0xffff);
	    status = send_af_write_cmd(hwidb,0xffff);
	    status = send_af_write_cmd(hwidb,0xffff);
	    /* 
	     * send the mask; all 0xff's for bridging
	     */
	    status = send_af_write_cmd(hwidb,0xffff);
	    status = send_af_write_cmd(hwidb,0xffff);
	    status = send_af_write_cmd(hwidb,0xffff);
	    /* 
	     * set bridging
	     */
	    status = send_af_write_cmd(hwidb,filter);
	} else {
	    errmsg(&msgsym(NOBRIDGE, SYS));
	}
    }
}

/*****************************************************************************
 * Name: pas_ethernet_reset_afilter
 *
 * Description: resets an ordered/sorted (fast) ethernet address filter
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 * Assumptions:
 *   1. the interface has been selected
 *   2. the interrupt level has been raised
 *
 *****************************************************************************/
static void pas_ethernet_reset_afilter (hwidbtype *hwidb)
{
    ushort filter;
    idbtype *swidb;
    /* Filter for particular address, not range. */
    uchar       mask[IEEEBYTES] = { 0, 0, 0, 0, 0, 0};

    /*
     * Initialize address filter.  If spanning tree index is set, we
     * set up the HyBridge FSM, otherwise we clear all FSM's.  Note that
     * we refuse to set the HyBridge FSM if the MCI controller is not setup
     * for bridging support.  This prevents a performance disaster.    
     * Clear mask of FSM's that accept multicast addresses.
     */
    hwidb->multibits = 0;
    filter = 0;
    
    send_specific_cmd_1arg(hwidb, FCI_FEIP_SPECIFIC, FEIP_INIT_AF);
    
    /*
     * Enable Reception for appropriate addresses and address groups.
     * Determine our station address based on level 3 protocol support.
     * Also set broadcast address.
     */
    cbus_pas_ethernet_address(hwidb, FSM_STATION,
			       hwidb->hardware, (uchar *) mask);
    cbus_pas_ethernet_address(hwidb, FSM_BROADCAST, 
			       baddr, (uchar *) mask);
    
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	/* 
	 * should the bcast addr be put in the smf at this point 
	 * as well? 
	 */
	reg_invoke_tbridge_smf_update(hwidb->hardware,
				      swidb,
				      SMF_MACTYP_OWN_UNICAST |
				      SMF_ACTION_RECEIVE);
	reg_invoke_media_interesting_addresses(swidb);
    }
}

/*****************************************************************************
 * Name: cbus_record_pas_ethernet_address
 *
 * Description: Sets the interface to listen to the address specified.
 *   This function is used in the "hwidb->listen" vector in the IDB
 *   for (fast) ethernet interfaces 
 *
 * Input: 
 *   idb -- pointer to the software IDB for the interface
 *   address -- pointer to the ethernet address to "record"
 *   fsm     -- FSM mask to be used
 *
 * Return Value: None
 *
 *****************************************************************************/
void cbus_record_pas_ethernet_address (idbtype *idb, uchar *address,
					uchar fsm)
{
    hwidbtype	*hwidb;
    leveltype	level;
    /* Filter for particular address, not range. */
    uchar       mask[IEEEBYTES] = { 0, 0, 0, 0, 0, 0};

    hwidb = idb->hwptr;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Select the interface to get the address from
     */
    select_if(hwidb);

    /*
     * set the address
     */
    cbus_pas_ethernet_address(hwidb, fsm, address, (uchar *) mask);

    reset_interrupt_level(level);
}


/*****************************************************************************
 * Name: pas_ethernet_af_init
 *
 * Description: resets an entire ordered/sorted (fast) ethernet address filter
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void pas_ethernet_af_init (hwidbtype *hwidb)
{
    leveltype level;
 
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);
 
    /*
     * Reset address filter except bridging
     */
    pas_ethernet_reset_afilter(hwidb);
    
    /*
     * Setup bridging in address filter after everything else
     */
    pas_ethernet_reset_bridging(hwidb);
    reset_interrupt_level(level);
}

/*****************************************************************************
 * Name: cbus_mci_ether_reset
 *
 * Description: resets a cBus or MCI Ethernet Interface
 *   This function is used in the "hwidb->reset" vector in the IDB.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/

void cbus_mci_ether_reset (hwidbtype *hwidb)
{
    ushort filter;
    leveltype level;
    int i;
    idbtype *swidb;
    ushort status;
    boolean eoir_reset;

    if (hwidb->status & IDB_NOHARDWARE) {
	errmsg(&msgsym(RESETNXI, CBUS), hwidb, hwidb->hw_namestring);
	return;
    }

    if (hwidb->oir_flags & EOIR_RESET)
	eoir_reset = TRUE;
    else
	eoir_reset = FALSE;

    hwidb->counters.resets++;
    hwidb->status &= ~IDB_DISABLED;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Update the route cache:
     */
    if (!eoir_reset)
	reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
	
    select_if(hwidb);
    if (idb_is_throttled(hwidb) && (hwidb->status & IDB_MCI)) {
	;
    } else {
	send_if_disable_cmd_delay(hwidb, IF_DISABLE_TIME);
    }
    /*
     * Flush hold queue
     */
    if (!eoir_reset)
	holdq_reset(hwidb);
    
    /*
     * Get default Ethernet address.  
     */
    if (is_ieee_zero(&hwidb->bia[0])) {
	if (!reg_invoke_assign_mac_addr(hwidb)) {
	    get_ether_address(hwidb,hwidb->bia);
	}
	/*
	 * Fill in the hardware address if it is unknown
	 */
	if (is_ieee_zero(&hwidb->hardware[0])) {
	    ieee_copy(hwidb->bia, hwidb->hardware);
	}
    }

    /*
     * If interface is administratively down, don't re-enable
     */
    if (hwidb->state == IDBS_ADMINDOWN) {
	reset_interrupt_level(level);
	return;
    }

    if (!eoir_reset) {

	/*
	 * Initialize address filter.  If spanning tree index is set, we
	 * set up the HyBridge FSM, otherwise we clear all FSM's.  Note 
	 * that we refuse to set the HyBridge FSM if the MCI controller
	 * is not setup for bridging support.  This prevents a performance
	 * disaster.     
	 * Clear mask of FSM's that accept multicast addresses.
	 */
	hwidb->multibits = 0;
	filter = 0;

	/* Normal Ethernet address filter support */
	
	if (hwidb->tbridge_on_hwidb) {
	    if (bridge_enable) {
		mci_warning();
		if (hwidb->status & IDB_CBUS) {	/* MEC ether i/f's */
		    filter = (FCI_BRIDGING_MASK << 8);
		    status = send_bridge_cmd(hwidb,filter);
		} else if (hwidb->status & IDB_MCI) { /* MCI ether i/f's */
		    filter = (MCI_BRIDGING_MASK << 8);
		    send_mci_mema_select_cmd(hwidb,060);
		    send_mci_mema_write_cmd(hwidb,filter);

#ifdef TBRIDGE_DEBUG	/* see the monument in tbridge_debug_flags.h !!! */
		    if (tbridge_debug_crb)
		      buginf("\nTB-CRB %s MCI_CMD_MEMA_WRITE %04x",
			     hwidb->hw_namestring, filter);
#endif /* TBRIDGE_DEBUG */

		}
	    } else {
		errmsg(&msgsym(NOBRIDGE, SYS));
		filter = 0;
	    }
	}
	
	/*
	 * now set all the address filter bytes to the filter value
	 * the address filter write command is auto increment
	 */
	status = send_af_select_cmd(hwidb,0);
	if (status == FCI_RSP_OKAY) {
	    for (i = 0; i < AF_WORDS; i++) {
		status = send_af_write_cmd(hwidb,filter);
		if (status != FCI_RSP_OKAY)
		  break;
	    }
	}
	
	/*
	 * Enable Reception for appropriate addresses and address groups.
	 * Determine our station address based on level 3 protocol support.
	 * Also set broadcast address.
	 */
	cbus_mci_ether_address(hwidb, FSM_STATION, hwidb->hardware);
	cbus_mci_ether_address(hwidb, FSM_BROADCAST, baddr);
	
	/* should the bcast addr be put in the smf at this point as well? */
	reg_invoke_tbridge_smf_update(hwidb->hardware,
				      hwidb->firstsw,
				      SMF_MACTYP_OWN_UNICAST |
				      SMF_ACTION_RECEIVE);
	
	FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	    reg_invoke_media_interesting_addresses(swidb);
	}
	
	/*
	 * Setup the extended address filter for MEC devices -- this 
	 * function must called after all address are set i.e. no calls to
	 * the function cbus_mci_ether_address after this function call.
	 */
	cbus_ether_address(hwidb);
    }	

    /*
     * Set transmitter delay counter
     */
    reg_invoke_media_transmit_delay(hwidb->type, hwidb, hwidb->xmitdelay);

    /*
     * Set loopback state.  The hardware comes up normally in local loopback.
     */
    if (hwidb->loopback) {
	send_if_loopback_cmd(hwidb);
    } else {
	send_if_noloopback_cmd(hwidb);
    }

    /*
     * Enable reception, if the MCI interface is NOT throttled.
     * It is possible for this command to fail if there
     * insufficient receive buffers allocated.  Delay a bit after enabling
     * since some pseudo-Ethernets will keep the MCI bitslice busy for more
     * than the multibus timeout interval.
     */
    if (idb_is_throttled(hwidb) && (hwidb->status & IDB_MCI))  {
	;
    } else {
	cbus_mci_txlimit(hwidb);
	status = send_if_enable_cmd_delay(hwidb, 10);
	if (status != MCI_RSP_OKAY) {
	    errmsg(&msgsym(RSETFAIL, CBUS), hwidb->hw_namestring, 
		   "cbus_mci_ether_reset()", status);
	    reset_interrupt_level(level);
	    return;
	}
    }
    reset_interrupt_level(level);
    return;
}

/*****************************************************************************
 * Name: cbus_feip_reset
 *
 * Description: resets a FEIP (NOT VIP FE) fast ethernet interface
 *   This function is used in the "hwidb->reset" vector in the IDB.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/

void cbus_feip_reset (hwidbtype *hwidb)
{
    leveltype level;
    ushort status;
    boolean eoir_reset;
    ethersb *esb;

    if (hwidb->status & IDB_NOHARDWARE) {
	errmsg(&msgsym(RESETNXI, CBUS), hwidb, hwidb->hw_namestring);
	return;
    }

    if (hwidb->oir_flags & EOIR_RESET)
	eoir_reset = TRUE;
    else
	eoir_reset = FALSE;

    hwidb->counters.resets++;
    hwidb->status &= ~IDB_DISABLED;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);
	
    if (!eoir_reset) {
	/*
	 * Update the route cache:
	 */
	reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
    }
	
    /*
     * Disable reception
     */
    send_if_disable_cmd_delay(hwidb, IF_DISABLE_TIME);

    if (!eoir_reset) {
	/*
	 * Flush hold queue
	 */
	holdq_reset(hwidb);
    }

    /*
     * Get default Ethernet address.  
     */
    if (is_ieee_zero(&hwidb->bia[0])) {
	if (!reg_invoke_assign_mac_addr(hwidb)) {
	    get_ether_address(hwidb,hwidb->bia);
	}
	/*
	 * Fill in the hardware address if it is unknown
	 */
	if (is_ieee_zero(&hwidb->hardware[0])) {
	    ieee_copy(hwidb->bia, hwidb->hardware);
	}
    }

    /*
     * If interface is administratively down, don't re-enable
     */
    if (hwidb->state == IDBS_ADMINDOWN) {
	reset_interrupt_level(level);
	return;
    }

    /*
     * Reset address filter 
     */
    pas_ethernet_af_init(hwidb);
    
    esb = idb_get_hwsb_inline(hwidb, HWIDB_SB_ETHER);
    
    /* Set 100BaseTX or MII */
    send_specific_cmd_1arg(hwidb, FCI_FEIP_SPECIFIC, esb->media_type);

    /* Set half or full duplex mode. */
    send_specific_cmd_1arg(hwidb, FCI_FEIP_SPECIFIC, esb->feip_fullduplex ?
			   FEIP_FULL_DUPLEX : FEIP_HALF_DUPLEX);

    /* Set loopback state.  */
    send_specific_cmd_1arg(hwidb, FCI_FEIP_SPECIFIC, hwidb->ext_loop_type);

    /* assume we're up with a reset until we hear otherwise */
    hwidb->state = IDBS_UP;
	
    /*
     * Enable reception
     */
    cbus_mci_txlimit(hwidb);
    status = send_if_enable_cmd_delay(hwidb, 10);
    if (status != MCI_RSP_OKAY) {
	errmsg(&msgsym(RSETFAIL, CBUS), hwidb->hw_namestring, 
	       "cbus_feip_reset()", status);
	reset_interrupt_level(level);
	return;
    }
    reset_interrupt_level(level);
    return;
}

/*****************************************************************************
 * Name: cbus_pas_ethernet_reset
 *
 * Description: resets a cbus (fast) ethernet interface (NOT FEIP!)
 *   This function is used in the "hwidb->reset" vector in the IDB.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/

void cbus_pas_ethernet_reset (hwidbtype *hwidb)
{
    leveltype level;
    ushort status;
    boolean eoir_reset;
    ethersb *esb;

    if (hwidb->status & IDB_NOHARDWARE) {
	errmsg(&msgsym(RESETNXI, CBUS), hwidb, hwidb->hw_namestring);
	return;
    }

    if (hwidb->oir_flags & EOIR_RESET)
	eoir_reset = TRUE;
    else
	eoir_reset = FALSE;

    hwidb->counters.resets++;
    hwidb->status &= ~IDB_DISABLED;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);
	
    if (!eoir_reset) {
	/*
	 * Update the route cache:
	 */
	reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
	
	/*
	 * Disable reception
	 */
	send_if_disable_cmd_delay(hwidb, IF_DISABLE_TIME);

	/*
	 * Flush hold queue
	 */
	holdq_reset(hwidb);
    }

    /*
     * Get default Ethernet address.  
     */
    if (is_ieee_zero(&hwidb->bia[0])) {
	if (!reg_invoke_assign_mac_addr(hwidb)) {
	    get_ether_address(hwidb,hwidb->bia);
	}
	/*
	 * Fill in the hardware address if it is unknown
	 */
	if (is_ieee_zero(&hwidb->hardware[0])) {
	    ieee_copy(hwidb->bia, hwidb->hardware);
	}
    }

    /*
     * If interface is administratively down, don't re-enable
     */
    if (hwidb->state == IDBS_ADMINDOWN) {
	if (eoir_reset) {
	    send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);
	}
	reset_interrupt_level(level);
	return;
    }

    if (!eoir_reset) {
	/*
	 * Reset address filter 
	 */
	pas_ethernet_af_init(hwidb);

	if (hwidb->type == IDBTYPE_FEIP) {
	    /* NOTE: someday the 4/8E will also support FDX and
	     * loopback commands */
	    esb = idb_get_hwsb_inline(hwidb, HWIDB_SB_ETHER);

	    /* Set 100BaseTX or MII */
	    send_specific_cmd_1arg(hwidb, FCI_FEIP_SPECIFIC, esb->media_type);

	    /* Set half or full duplex mode. */
	    send_specific_cmd_1arg(hwidb, FCI_FEIP_SPECIFIC, esb->feip_fullduplex ?
				   FEIP_FULL_DUPLEX : FEIP_HALF_DUPLEX);

	    /* Set loopback state.  */
	    send_specific_cmd_1arg(hwidb, FCI_FEIP_SPECIFIC, hwidb->ext_loop_type);
	}
	/* assume we're up with a reset until we hear otherwise */
	hwidb->state = IDBS_UP;
    }
	
    /*
     * Enable reception
     */
    cbus_mci_txlimit(hwidb);
    if (!eoir_reset) {
	status = send_if_enable_cmd_delay(hwidb, 10);
	if (status != MCI_RSP_OKAY) {
	    errmsg(&msgsym(RSETFAIL, CBUS), hwidb->hw_namestring, 
		   "cbus_pas_ethernet_reset()", status);
	    reset_interrupt_level(level);
	    return;
	}
    } else {
	send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);
    }
    reset_interrupt_level(level);
    return;
}

/*****************************************************************************
 * Name: cbus_mci_ether_periodic
 *
 * Description: periodic background ethernet function
 *   This function is used in the "hwidb->periodic" vector in the IDB.
 *   Called from periodic background task to send an Ethernet loopback
 *   packet.  Check if we've been able to receive our own loopback packets
 *   recently and adjust line protocol appropriately.  Note that we can't
 *   just fiddle idb->state since it could be mistakenly reset by interrupt
 *   level hardware.
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/

void cbus_mci_ether_periodic (hwidbtype *hwidb)
{
    boolean newstate, oldstate;
    
    if (hwidb->nokeepalive || (hwidb->state == IDBS_ADMINDOWN)) {
	if (!hwidb->mci_lineup) {
	    hwidb->mci_lineup = MCI_KEEP_COUNT;
	    if (hwidb->state != IDBS_ADMINDOWN) {
		hwidb->reason = "Checking disabled";
		reg_invoke_ifmib_link_trap(LINK_UP_TRAP, hwidb);
	    }
	}
	return;
    }

    /* Don't do a keepalive packet if this is a transmit only interface.
     * This will be done on this interface when the receive only interface
     * is scaned by net_periodic().
     */
    if (!hwidb->xmitonly && !send_loop(hwidb, hwidb->hardware)) {
	buginf("\n%s: keepalive not sent", hwidb->hw_namestring);
	return;
    }

    /*
     * If the interface is a VIP style interface, we are done.
     * VIP style interfaces control the mci_lineup via love notes
     * coming back from the VIP kernel when carrier changes state
     */
    if (is_vip(hwidb)) {
	return;
    }

    if (TIMER_RUNNING(hwidb->mci_lineuptimer)) {
	newstate = CLOCK_IN_INTERVAL(hwidb->mci_lineuptimer,
				     2*ONESEC*hwidb->keep_period);
	if (!newstate)
	    TIMER_STOP(hwidb->mci_lineuptimer);
    } else {
	newstate = FALSE;
    }
    oldstate = hwidb->mci_lineup ? TRUE : FALSE;
    if (newstate != oldstate) {
	if (newstate) {
	    hwidb->mci_lineup = MCI_KEEP_COUNT;	/* Just came up */
	    hwidb->reason = "Keepalive OK";
	    reg_invoke_ifmib_link_trap(LINK_UP_TRAP, hwidb);
	} else if (--hwidb->mci_lineup <= 0) {
	    if (hwidb->status & IDB_MCI) {
		cbus_restarts_lineup++;
		platform_demand_complex_restart(hwidb);
	    }
            /* NmEvent Hook 5 */
            if (hwidb->failCause == 0)
                hwidb->failCause = Ether_KeepAliveFailure;
	}
    } else if (newstate)
        hwidb->mci_lineup = MCI_KEEP_COUNT;	/* Up, and stayed up */
    else
	if (hwidb->status & IDB_MCI) {
            /*
             *  One ms delay is needed to assure the keepalive has a chance
             *  of being received before the interface is reset.  When the
             *  keepalive is received, hwidb->mci_lineuptimer will be updated
             *  and verified during the next keep-period when cbus_mci_ether_
             *  ether_periodic is called again.  Note:  Without this delay,
             *  keepalives will never bring up an MCI ethernet interface
             *  with csc/4, because the port will always be reset before
             *  the keepalive is received.  This race-condition can also
             *  happen with a csc/3, however I haven't seen it...
             */
            DELAY(1);

	    (*hwidb->reset) (hwidb);		/* If it stays down, repeatedly
						   reset single interface */
        }
}

/*****************************************************************************
 * txburst_command
 * Set the transmit burst deferal on an interface.
 * Splice in the tx burst deferral value into the idb->xmitdelay value.
 *****************************************************************************/
void txburst_command (parseinfo *csb)
{
    hwidbtype *idb;
    int     ticks = 0;
 
    idb = csb->interface->hwptr;
 
    if (idb == NULL)
        return;
 
    /* Only act for c7000 EIP */
    if ((idb->status & (IDB_ETHER | IDB_CBUS)) != (IDB_ETHER | IDB_CBUS) )
        return;
 
    if (csb->nvgen) {
 
        if (idb->tx_burst_deferral) {
            nv_write(TRUE, "%s %d", csb->nv_command,
                        idb->tx_burst_deferral);
        }
        return;
    }
 
    /*
     * Load the transmitter-delay that may
     * be present (lower 17 bits for EIP)
     * before loading the tx-burst-deferral value
     * because both commands share the same registered function.
     */
    ticks = idb->xmitdelay & 0x1FF;
    if (csb->sense)
        ticks |=  GETOBJ(int,1) << 17;
    else
        idb->tx_burst_deferral = 0;
 
    reg_invoke_media_transmit_delay(idb->type, idb, ticks);
 
}
      
