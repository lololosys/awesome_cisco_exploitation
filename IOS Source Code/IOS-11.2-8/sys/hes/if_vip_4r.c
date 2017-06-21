/* $Id: if_vip_4r.c,v 3.13.12.14 1996/09/13 22:17:03 klambert Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_vip_4r.c,v $
 *------------------------------------------------------------------
 * if_vip_4r.c -- cBus driver for 4R Token Ring port adapter
 *
 * July 1995 - Stig Thormodsrud
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_vip_4r.c,v $
 * Revision 3.13.12.14  1996/09/13  22:17:03  klambert
 * CSCdi62974:  multi-port srb is process-switched on VIP/4R
 * Needed to setup idb->rng_br_rng_[fb]. Similar to CSCdi61600
 * Branch: California_branch
 *
 * Revision 3.13.12.13  1996/08/28  12:45:22  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.12.12  1996/08/26  15:07:28  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.13.12.11  1996/07/16  09:36:27  hampton
 * Centralize knowledge of Decnet/Token Ring interaction.  [CSCdi63066]
 * Branch: California_branch
 *
 * Revision 3.13.12.10  1996/07/09  05:51:29  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.13.12.9  1996/06/07  00:33:56  kao
 * CSCdi59481:  4R does not work when hot swapped
 * CSCdi59000:  FDDI PA line status is always down after an eoir event
 * Branch: California_branch
 *
 * Revision 3.13.12.8  1996/06/01  07:02:51  hwang
 * CSCdi59222:  4R rp/rsp driver issue cbus command without select
 * interface
 * Branch: California_branch
 *
 * Revision 3.13.12.7  1996/06/01  06:15:34  hampton
 * Remove obsolete media_tring_supports_etr registry call.  [CSCdi59254]
 * Branch: California_branch
 *
 * Revision 3.13.12.6  1996/05/22  13:51:15  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.13.12.5  1996/05/17  11:18:47  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.13.2.3  1996/05/05  23:07:28  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.13.2.2  1996/04/26  15:19:05  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.13.2.1  1996/04/03  14:30:53  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.13.12.4  1996/05/09  14:29:04  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.13.12.3  1996/04/30  17:21:29  hwang
 * CSCdi48080:  4R does not recover from beaconing on the ring
 * Set mci_lineup count to MCI_KEEP_COUNT after receive 4R open
 * related love letter.
 * Branch: California_branch
 *
 * Revision 3.13.12.2.14.1  1996/04/27  07:02:35  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.13.12.2.4.1  1996/04/08  01:47:00  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.13.12.2  1996/03/30  01:28:32  hwang
 * CSCdi51725:  4r driver need to provide lan manager support
 * Branch: California_branch
 * Enable mac frame receive if srb configured, don't directly
 * call ctr_modify use idb->modify_srb vector instead, now we
 * have 2 cbus token ring driver (trip and vip_4r).
 *
 * Revision 3.13.12.1  1996/03/18  19:42:36  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.13.16.1  1996/03/22  09:36:41  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.13  1996/02/12  23:05:39  hwang
 * CSCdi46480:  4R failed hot standby test
 * Allow vip 4r driver to set new functional address used by
 * hsrp protocol.
 *
 * Revision 3.12.8.1  1996/03/05  06:11:38  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.12.4.1  1996/02/27  20:42:18  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.12  1996/02/01  06:02:50  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.11.2.1  1996/01/24  21:47:28  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.11  1996/01/18  22:47:29  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.10  1996/01/11  03:50:33  kao
 * CSCdi43878:  IS_VIP() macro should be used
 *
 * Revision 3.9.2.1  1996/02/15  19:13:19  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.9  1995/12/19  00:40:28  hampton
 * Obsolete the GET_NONZERO_TIMESTAMP routine.  It is no longer necessary
 * with 64bit timestamps.  [CSCdi45208]
 *
 * Revision 3.8  1995/12/14  19:38:32  hwang
 * Reset RINGMODE_BRIDGE, TRFUNC_BRIDGE, and RINGMODE_TBRIDGE if interface
 * is not configured for srb or tb mode.
 * CSCdi43509:  srb bridging over rvip fails
 *
 * Revision 3.7  1995/12/07  22:28:44  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.6  1995/11/29  02:47:54  sthormod
 * CSCdi42503:  4r needs modularity work
 * Clean up #ifdef C7000 kludge.
 *
 * Revision 3.5  1995/11/21  19:09:23  snyder
 * CSCdi44272:  more spellink errors
 *              recieved -> received
 *
 * Revision 3.4  1995/11/20  22:24:27  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.3  1995/11/17  09:19:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:29:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:41:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/17  05:35:28  sthormod
 * Placeholders for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "logger.h"
#include "../if/msg_tr.c"		/* Not a typo, see logger.h */
#include "msg_cbus.c"			/* Not a typo, see logger.h */
#include <string.h>
#include <ciscolib.h>
#include "subsys.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "cbus_registry.h"
#include "../srt/srt_registry.h"
#include "interface_private.h"

#include "../if/tring.h"
#include "../if/tring_private.h"
#include "../if/tring_debug.h"
#include "../if/network.h"
#include "../if/linkdown_event.h"

#include "../snmp/ifmib_registry.h"  

#include "../dev/flash_dvr_spec.h"        /* needed for slots.h */

#include "ucode.h"			  /* needed for slots.h */

#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"   

#include "if_mci.h"
#include "if_fci.h"
#include "if_cmd.h"
#include "if_hes_common.h"
#include "if_vip.h"
#include "if_vip_4r.h"
#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"
#include "../smf/smf_public.h"
#include "../tbridge/tbridge_debug.h"

/*
 * Protoypes
 */
static void cbus_vip_4r_get_mib_stats(hwidbtype *);


/*
 * send_vip_4r_block
 */
static ushort send_vip_4r_block (hwidbtype *hwidb, char *block, int bytes)
{
    ushort status, *ptr;
    int i;

    status = send_vip_4r_start_block_cmd(hwidb);
    if (status != FCI_RSP_OKAY) {
	return status;
    }

    ptr = (ushort *)block;
    for (i = 0; i < bytes/sizeof(ushort); i++) {
	status = send_vip_4r_write_block_cmd(hwidb, *ptr);
	if (status != FCI_RSP_OKAY) {
	    return status;
	}
	ptr++;
    }

    status = send_vip_4r_end_block_cmd(hwidb);
    if (status != FCI_RSP_OKAY) {
	return status;
    }

    return status;
}


/*
 * send_vip_4r_init
 */
static void send_vip_4r_init (hwidbtype *hwidb, ulong cmd)
{
    ibmtr_cfg_data_t    cfg;
    tokensb	       *tsb;
    srb_vector_table_t *srbV_p;

    tsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
    memset(&cfg, 0, sizeof(cfg));
    cfg.config_type = cmd;
    ieee_copy(hwidb->hardware, &cfg.hardware[0]);
    cfg.tr_functional = tsb->tr_functional;
    cfg.tr_ring_mode = hwidb->tr_ring_mode;
    cfg.tr_group = tsb->tr_group;
    cfg.tr_ring_speed = hwidb->tr_ring_speed;

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srb_triplet_t srb_triplet;
        srbV_p->srb_sb_get_triplet(hwidb->firstsw, &srb_triplet);
        cfg.srb_thisring   = srb_triplet.localRing;
        cfg.srb_bridge_num = srb_triplet.bridgeNum;
        cfg.srb_targetring = srb_triplet.remoteRing;
    } else {
        cfg.srb_thisring   = SRB_INVALID_RING_NUM;
        cfg.srb_bridge_num = SRB_INVALID_BRIDGE_NUM;
        cfg.srb_targetring = SRB_INVALID_RING_NUM;
    }

    /*
     * recalc srb params
     */
    if (srbV_p) {
	srbV_p->srb_sb_init_rng_br_rng_mask(hwidb->firstsw);
    }
    
    /* 
     * The interface should already be selected for the reset routine. 
     */
    send_vip_4r_block(hwidb, (char *)&cfg, sizeof(ibmtr_cfg_data_t));
    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
}


/* 
 * cbus_vip_4r_set_functional
 */

static boolean cbus_vip_4r_set_functional (hwidbtype *idb)
{
    leveltype	level;

    if (idb->state != IDBS_UP) {
	return(FALSE);
    }
    tr_functional_init(idb);
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(idb);
    send_vip_4r_init(idb, FUNCTIONAL_ADDR_CHANGE);
    reset_interrupt_level(level);
    return(TRUE);
}


/*
 * cbus_vip_4r_reset
 */
boolean cbus_vip_4r_reset (hwidbtype *hwidb)
{
    leveltype level;
    int i;
    boolean eoir_reset;
    tokensb *tsb;
    boolean srb_bridging; 
    srb_vector_table_t *srbV_p;
    uchar save_bia[IEEEBYTES];
    uchar save_hardware[IEEEBYTES];
    boolean tbridge_on ;
    idbtype *swidb ;
    uchar swapped_address[IEEEBYTES];

    if (hwidb->status & IDB_NOHARDWARE) {
	errmsg(&msgsym(RESETNXI, CBUS), hwidb, hwidb->hw_namestring);
	return(TRUE);
    }

    /*
     * Are resets allowed yet?
     */
    if (nvsize && !system_configured) {
	return(TRUE);
    }

    if (hwidb->oir_flags & EOIR_RESET)
	eoir_reset = TRUE;
    else
	eoir_reset = FALSE;

    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);
    hwidb->counters.resets++;

    if (!eoir_reset) {
	TIMER_STOP(hwidb->mci_lineuptimer);
	hwidb->mci_lineup = 0;
	hwidb->status &= ~IDB_DISABLED;
	reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
	send_if_disable_cmd(hwidb); 
 	
	/*
	 * Flush hold queue
	 */
	holdq_reset(hwidb);
	
	/*
	 * Initialize the burned in MAC address and the working
	 * MAC address for the interface.
	 */
	if (is_ieee_zero(hwidb->bia)) {

            /*
             * Burned in MAC addr not setup yet - go initialize it
             */
	    (void) reg_invoke_assign_mac_addr(hwidb);

	    /*
             * Platform assigned MAC address is in ethernet format.
             * So, we have to bit flip it.
             */
            for (i = 0; i < IEEEBYTES; i++)
	        hwidb->bia[i] = BITSWAP(hwidb->bia[i]);

            /*
             * Don't disturb working MAC address if user has configured a LAA
             */
	    if (is_ieee_zero(hwidb->hardware))
	        ieee_copy(hwidb->bia, hwidb->hardware);

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
            ieee_copy(hwidb->bia, save_bia);
            ieee_copy(hwidb->hardware, save_hardware);
            ieee_zero(hwidb->bia);     /* Must be zero for assign_mac_addr to work */
            (void) reg_invoke_assign_mac_addr(hwidb);
            if (ieee_equal(save_bia, hwidb->bia)) {
                /*
                 * Yep, we got the standard e-net address for our slot/port - flip it
                 */
                for (i = 0; i < IEEEBYTES; i++)
                   hwidb->bia[i] = BITSWAP(hwidb->bia[i]);
                /*
                 * Check working MAC address to see if it was also pre-assigned
                 * an ethernet address.  Also, make sure it's not zero.
                 */
                if (ieee_equal(save_hardware, save_bia) || is_ieee_zero(hwidb->hardware))
                   ieee_copy(hwidb->bia, hwidb->hardware);
                else
                   ieee_copy(save_hardware, hwidb->hardware);
            } else {
                ieee_copy(save_bia, hwidb->bia);
                ieee_copy(save_hardware, hwidb->hardware);
	    }
        }
    }

    vip_4r_restart_init(hwidb);

    /*
     * If interface is administratively down, don't re-enable
     */
    if (hwidb->state == IDBS_ADMINDOWN) {
	if (eoir_reset) {
	    send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);
	}
	reset_interrupt_level(level);
	return(TRUE);
    }

    if (!hwidb->tr_ring_speed) {
	reset_interrupt_level(level);
	return(TRUE);
    }

    cbus_mci_txlimit(hwidb);

    if (eoir_reset) {
	send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);
	TIMER_STOP(hwidb->xmittimer);
	reset_interrupt_level(level);
	return(TRUE);
    }

    tsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_TOKEN);

    /*
     * Set transmitter delay counter
     */
    reg_invoke_media_transmit_delay(hwidb->type, hwidb, hwidb->xmitdelay);

    /*
     * Validate the hardware address.  If it is invalid, use the
     * universally administered address for the interface.
     */
    if (hwidb->hardware[0] & TR_GROUP) {
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
	errmsg(&msgsym(BADMUL, TR), hwidb->unit, hwidb->hardware);
	ieee_copy(hwidb->bia, hwidb->hardware);
    }

    /*
     * Initialize functional addresses for routed protocols
     */
    tr_functional_init(hwidb);

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) { 
        srb_bridging = srbV_p->srb_sb_get_bridging_enabled(hwidb->firstsw);
    } else {
        srb_bridging = FALSE;
    }
    if (srb_bridging) {
	hwidb->tr_ring_mode |= RINGMODE_BRIDGE | RINGMODE_ALL_MAC;
	tsb->tr_functional |= TRFUNC_BRIDGE;
    } else {
	hwidb->tr_ring_mode &= ~(RINGMODE_BRIDGE | RINGMODE_ALL_MAC);
	tsb->tr_functional &= ~TRFUNC_BRIDGE;
    }

    /*
     * Set up for transparent bridging
     */
    tbridge_on = FALSE ;
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
      if (RUNNING_IF(LINK_BRIDGE,swidb) && bridge_enable &&
	!(system_loading | bootstrap_enable)) {
        tbridge_on = TRUE ;
        break ;
      }
    }

    if (tbridge_on) {
	hwidb->tr_ring_mode |= RINGMODE_TBRIDGE;
	tsb->tr_functional |= TRFUNC_BRIDGE;
        cbus_mci_rxtypes_update(hwidb);
    } else {
	hwidb->tr_ring_mode &= ~RINGMODE_TBRIDGE;
    }

    /* 
     * Insert/remove MAC addresses of interest in the SMF.
     */
    ieee_swap(hwidb->hardware, swapped_address);
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
        reg_invoke_tbridge_smf_update(swapped_address,
				  swidb,
				  SMF_MACTYP_OWN_UNICAST |
				  SMF_ACTION_RECEIVE);


        reg_invoke_media_interesting_addresses(swidb);
    }

    reg_invoke_media_rif_reset(hwidb);

    /*
     * Now we're ready to send this interfaces configuration
     * parameters to the 4R.  The enable command signals the 4R to
     * start bringing itself up.  The period routine will check if the
     * interface has been IDBS_INIT for longer than
     * VIP_4R_MAX_INIT_TIME and take some action.
     */
    net_cstate(hwidb, IDBS_INIT);
    TIMER_START(hwidb->vip4r_info->init_timer, VIP_4R_MAX_INIT_TIME);
    send_vip_4r_init(hwidb, INITIAL_CONFIG);
    send_if_enable_cmd(hwidb); 

    reset_interrupt_level(level);
    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
    return(TRUE);		/* Always a success */
}


/*
 * cbus_vip_4r_shutdown
 */
static void cbus_vip_4r_shutdown (hwidbtype *hwidb, int newstate)
{
    leveltype level;

    hwidb->counters.transitions++;
    level = raise_interrupt_level(NETS_DISABLE);

    holdq_reset(hwidb);

    select_if(hwidb);
    send_if_disable_cmd(hwidb);

    if (hwidb->state != IDBS_ADMINDOWN) {
	if (newstate != IDBS_ADMINDOWN)
	    net_cstate(hwidb, IDBS_RESET);
	else 
	    net_cstate(hwidb, IDBS_ADMINDOWN);
    }
    reset_interrupt_level(level);
    return;
}


/*
 * cbus_vip_4r_periodic
 */
void cbus_vip_4r_periodic (hwidbtype *hwidb)
{
    boolean newstate, oldstate;

    if (hwidb->state == IDBS_ADMINDOWN || hwidb->state == IDBS_RESET)
	return;

    /* 
     * If initializing give the interface 90 seconds to respond else shut
     * it down.  The user will have to do a "no shut" to get the system to
     * try again.  
     */
    if (hwidb->state == IDBS_INIT) {
	vip4r_data_t *vip4r;
	
	vip4r = INSTANCE_VIP_4R;
	if (!vip4r)
	    return;
	if (AWAKE(vip4r->init_timer)) {
	    if (tokenevent_debug)
		buginf("\n%s: Failed to initialize, shutting down",
		       hwidb->hw_namestring);
            TIMER_STOP(vip4r->init_timer);
            (*hwidb->shutdown)(hwidb, IDBS_RESET);
	}
        return;
    }   

    if (hwidb->nokeepalive)
        return;

    tr_send_loop(hwidb, hwidb->hardware);
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
	    if (tokenring_debug)
	      buginf("\n%s: Keepalive OK", hwidb->hw_namestring);
	} else if (--hwidb->mci_lineup <= 0) {
	    if (hwidb->status & IDB_MCI) {
		cbus_restarts_lineup++;

		/*
		 * When a port dies, restart entire switching complex (Ugh!)
		 */
		platform_demand_complex_restart(hwidb);
	    }
            else {  /* for interface type other then MCI */
                if (tokenevent_debug || tokenring_debug)
                      buginf("\n%s: Keepalive failed, resetting", 
			     hwidb->hw_namestring);
                (*hwidb->reset)(hwidb);
            }
            if (hwidb->failCause == 0)  /* Keep alive shouldn't overwrite real
					   cause */
	        hwidb->failCause = TR_KeepAliveFailure;
	}
    } else if (newstate)
        hwidb->mci_lineup = MCI_KEEP_COUNT;	/* Up, and stayed up */
}


/*
 * cbus_vip_4r_modify_srb
 */
static void cbus_vip_4r_modify_srb (hwidbtype *hwidb, ulong unused1,
                                    ulong unused2)
{
    leveltype level;
 
    level = raise_interrupt_level(NETS_DISABLE);    
 
    select_if(hwidb);
    send_vip_4r_init(hwidb, INITIAL_CONFIG);
 
    reset_interrupt_level(level);
}
 
/*
 * cbus_vip_4r_setup_srb
 */
static void cbus_vip_4r_setup_srb (hwidbtype *hwidb)
{
    cbus_vip_4r_modify_srb(hwidb, 0, 0);
}
 
/*
 *vip_4r_version_string
 */
static char *vip_4r_version_string (uchar *ucode_ver)
{
    static char ver[16];  /* careful, it's reused */

    /*
     *  10 byte microcode level from the IBM chipset.
     *  The first 3 bytes should be interpreted as hexadecimal, and
     *  the remaining 7 should be interpreted as an EBCDIC character
     *  string. 
     */
    sprintf(ver, "%02x%02x%02x.", ucode_ver[0], ucode_ver[1],
	    ucode_ver[2]);
    bcopy(&ucode_ver[3], &ver[7], 7);
    ver[15] = '\0';
    sys_ebcdic_to_ascii(&ver[7], 7);
    return ver;
}


/*
 * parse_vip
 */
void parse_vip_4r_err_ll (hwidbtype *hwidb, char *buf)
{
    vip_4r_err_ll *ll = (vip_4r_err_ll *)buf;
    char *ver = NULL;
    leveltype level;
    ushort status;
    int phase, error;
    boolean display;
    tokensb	*tsb;

    if (tokenevent_debug)
	buginf("\nll received by %s, ", hwidb->hw_namestring);

    tsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
    switch (ll->err_type) {
    case VIP_4R_INIT_FAIL:
	/*
	 *  Init diagnostics for the interface have failed.  Not much
 	 *  we can do at this point.
	 * 
	 *  Code  Meaning
	 *  ====  =======
	 *  0000  All diags completed successfully
	 *  0020  Processor initialization failed
	 *  0022  Microcode program storage diagnostics failed
	 *  0024  Adapter RAM diagnostics failed
	 *  0026  Adapter microprocessor instruction test failed
	 *  0028  Adapter microprocessor interrupt test failed
	 *  002A  System interface hardware test failed
	 *  002C  Token-ring protocol handler test failed (digital wrap)
	 *  002E  Busmaster function test failed
	 *  0030  Address match RAM test failed
	 *  0032  Address match Control Addressable Memory test failed
	 *  003C  Token-ring protocol handler test failed (analog wrap)
	 *
	 * LAN Technical Reference 7-6
         */
	ver = vip_4r_version_string(&ll->type.init.version[0]);
	if (tokenevent_debug)
	    buginf("init fail %x, version %s",
		   ll->type.init.bring_up_code, ver); 
	errmsg(&msgsym(DIAGFAIL, TR), hwidb->hw_namestring,
	       ll->type.init.bring_up_code, ver);
	net_cstate(hwidb, IDBS_RESET);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
	return;

    case VIP_4R_OPEN:
	/*
	 * Result of Adapter Open Command
	 * 
	 *  Return Code 
	 *  ===========
	 *  00  Operation completed successfully
	 *  01  Unrecognized command code
	 *  03  Adapter opened but must be closed for this command
	 *  07  Command cancelled -- unrecoverable failure
	 *  32  Node address not valid
	 *  34  Defined TRB length not valid
	 *  3C  Group/Functional address indicator bits not set correctly
	 *  62  Configuration not valid
	 *
	 * LAN Technical Reference 8-19
	 */
	ver = vip_4r_version_string(&ll->type.open.version[0]);
	hwidb->mci_lineup = MCI_KEEP_COUNT;
	switch (ll->type.open.ret_code) {
	case VIP_4R_OPEN_OK:
	    {
	    vip4r_data_t *vip4r;
	    
	    if (tokenevent_debug) 
		buginf("open OK, version %s", ver);
	    net_cstate(hwidb, IDBS_UP);
	    GET_TIMESTAMP(hwidb->mci_lineuptimer);
	    tsb->tr_ring_status = 0;
	    GET_TIMESTAMP(tsb->tr_status_time);
	    hwidb->counters.transitions++;
	    vip4r = INSTANCE_VIP_4R;
	    if (!vip4r) 
		return;
	    bcopy(&ll->type.open.version[0], &vip4r->ucode_version[0],
		  MAC_VERS_LEN);
	    cbus_vip_4r_get_mib_stats(hwidb);
	    if (registry_populated(REG_SRT))
		reg_invoke_srt_setup_auto_sw_srb(hwidb);
	    tr_functional_init(hwidb);
	    cbus_vip_4r_set_functional(hwidb);
	    if (tsb->tr_ring_status == 0)
		reg_invoke_media_tring_ring_status(hwidb, 0);
	    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
	    return;
 	    }
	case VIP_4R_OPEN_CANCELLED:
	    /*
	     * If the return code is set to 0x07, then check the open
	     * error code for more detail.
	     *
	     * LAN Technical Reference 8-20
	     */
	    if (tokenevent_debug) 
		buginf("open failed, ret_code %04x, error_code %04x, version %s", 
		       ll->type.open.ret_code,
		       ll->type.open.open_error_code, ver);

	    level = raise_interrupt_level(NETS_DISABLE);
	    select_if(hwidb);
	    send_if_disable_cmd(hwidb);
	    reset_interrupt_level(level);

	    status = ll->type.open.open_error_code;
	    phase = ((status & 0x00F0) >> 4);
	    error = (status & 0x000F);
	    tsb->ringOpenStatus = error;
	    TR_OpenError(hwidb, error, phase, TRUE);
	    net_cstate(hwidb, IDBS_DOWN);
	    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
	    return;
	default:
	    if (tokenevent_debug)
		buginf("\nopen failed, return code 0x%x", ll->type.open.ret_code);
	    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
	    return;
	}
	break;  /* end case VIP_4R_OPEN */

    case VIP_4R_ADP_CHK:
	/*
	 * An adapter check indicates that a serious problem has been
	 * detected by the adapter.  The adapter must be reset
	 * following an adapter check.  The first 2 bytes of the
	 * adapter check data are the Adapter Check Reason Codes and
	 * are defined as: 
	 * 
	 *  Code  Description
	 *  ====  ===========
	 *  8000  Reserved for device driver use
	 *  4000  (Reserved)
	 *  2000  Adapter internal cache parity error
	 *  1000  Adapter illegal operation
	 *  0800  Adapter bus parity error
	 *  0400  (Reserved)
	 *  0200  (Reserved)
	 *  0100  Parity error -- protocol handler local bus
	 *  0080  Parity error -- protocol handler transmit FIFO
	 *  0040  Parity error -- protocol handler receive FIFO
	 *  0020  Protocol handler transmit FIFO underrun
	 *  0010  Protocol handler receive FIFO overrun
	 *  0008  Unrecognized interrupt
	 *  0004  Unrecognized error interrupt
	 *  0003  (Reserved)
	 *  0002  Unrecognized supervisory request
	 *  0001  Program detected error
	 *
	 * LAN Technical Reference 5-7
	 */
	errmsg(&msgsym(ADPCHK, TR), hwidb->hw_namestring, 
	       ll->type.adp_chk.data[0],
	       ll->type.adp_chk.data[1],
	       ll->type.adp_chk.data[2],
	       ll->type.adp_chk.data[3]);
	net_cstate(hwidb, IDBS_RESET);
	break;

    case VIP_4R_LAN_STATUS:
	/*
	 * Change in lan status
	 *
	 * LAN Technical Reference 10-12
	 */
	if (tokenevent_debug)
	    buginf("ring status change ll %x",
		   ll->type.lan_status_change.lan_status); 
	tsb->tr_ring_status = ll->type.lan_status_change.lan_status;
	GET_TIMESTAMP(tsb->tr_status_time);
	/*
	 * Should we display a message for this ring status.
	 */
	display = FALSE;
	if (tsb->tr_ring_status & RNG_WIRE_FAULT) {
	    if (AWAKE(tsb->tr_next_buginf_time))
		display = TRUE;
	}
    
	/*
	 * Call lanmgr_ringstat() here, because the interface may not be UP yet
	 * The tr_ringstat_common() does not call it.
	 */    
	if (hwidb->state == IDBS_UP)
	    reg_invoke_media_tring_ring_status(hwidb, tsb->tr_ring_status);
	if (tr_ringstat_common(hwidb, tsb->tr_ring_status, display)) {
	    TIMER_START(tsb->tr_next_buginf_time, TR_BUGINF_INTERVAL);
	}
	break;

    default:
	if (tokenevent_debug)
	    buginf("unknow type %d", ll->err_type);
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
}


/*
 * parse_vip_4r_smt_ll
 */
void parse_vip_4r_smt_ll (hwidbtype *hwidb, char *buf)
{
    vip4r_data_t  *vip4r;
    vip_4r_smt_ll *love;

    if (tokenevent_debug)
	buginf("\n%s received smt ll", hwidb->hw_namestring);

    vip4r = INSTANCE_VIP_4R;
    if (!vip4r) 
	return;
    love = (vip_4r_smt_ll *)buf;
    bcopy(&love->smt, &vip4r->smt, sizeof(smt_parms_table));
    bcopy(&love->addr_tbl, &vip4r->addr_tbl, sizeof(addr_table));
    bcopy(&love->ucode_version, &vip4r->ucode_version, MAC_VERS_LEN);
    vip4r->ring_util = GETSHORT(&love->ring_util);
}


/*
 * MPC_ERR_UPDATE
 * 
 * mpc->foo += mpc_new->foo;
 */
#define MPC_ERR_UPDATE(foo)               \
{                                         \
   mpc-> ## foo += mpc_new-> ## foo;      \
}


/*
 * TX_ERR_UPDATE
 * 
 * tx->foo += tx_new->foo;
 */
#define TX_ERR_UPDATE(foo)                \
{                                         \
   tx-> ## foo += tx_new-> ## foo;        \
}


/*
 * RX_ERR_UPDATE
 *
 * rx->foo += rx_new->foo;
 */
#define RX_ERR_UPDATE(foo)                \
{                                         \
   rx-> ## foo += rx_new-> ## foo;        \
}


/*
 * parse_vip_4r_error_ll
 */
void parse_vip_4r_error_ll (hwidbtype *hwidb, char *buf)
{
    vip4r_data_t  *vip4r;
    vip_4r_errors_ll *love;
    mpc_errors *mpc, *mpc_new;
    tx_errors *tx, *tx_new;
    rx_errors *rx, *rx_new;

    if (tokenevent_debug)
	buginf("\n%s received errors ll", hwidb->hw_namestring);

    vip4r = INSTANCE_VIP_4R;
    if (!vip4r) 
	return;

    love = (vip_4r_errors_ll *)buf;

    mpc = &vip4r->mpc_err;
    mpc_new = &love->mpc_err;
    MPC_ERR_UPDATE(err_line);
    MPC_ERR_UPDATE(err_internal);
    MPC_ERR_UPDATE(err_burst);
    MPC_ERR_UPDATE(err_arifci);
    MPC_ERR_UPDATE(err_abort);
    MPC_ERR_UPDATE(err_lostframe);
    MPC_ERR_UPDATE(err_rcvcongestion);
    MPC_ERR_UPDATE(err_copy);
    MPC_ERR_UPDATE(err_token);
    
    tx = &vip4r->tx_err;
    tx_new = &love->tx_err;
    TX_ERR_UPDATE(tx_pci_read_error);
    TX_ERR_UPDATE(tx_icd_read_parity_error);
    TX_ERR_UPDATE(tx_icd_write_parity_error);
    TX_ERR_UPDATE(tx_icd_addr_parity_error);
    TX_ERR_UPDATE(tx_stat_write_error);
    TX_ERR_UPDATE(tx_fifo_underrun);
    TX_ERR_UPDATE(tx_fifo_parity_error);

    rx = &vip4r->rx_err;
    rx_new = &love->rx_err;
    RX_ERR_UPDATE(rx_pci_read_parity_error);
    RX_ERR_UPDATE(rx_int_read_parity_error);
    RX_ERR_UPDATE(rx_int_write_parity_error);
    RX_ERR_UPDATE(rx_int_pci_address_bfr);
    RX_ERR_UPDATE(rx_fifo_parity_error);
    RX_ERR_UPDATE(rx_no_data_bfr);
    RX_ERR_UPDATE(rx_pci_write_error);
    RX_ERR_UPDATE(rx_fifo_overrun);
    RX_ERR_UPDATE(rx_parity_error);
    RX_ERR_UPDATE(rx_mpc_error);
    RX_ERR_UPDATE(rx_imp_abrt_frm);
    RX_ERR_UPDATE(rx_exp_abrt_frm);
}


/*
 * cbus_vip_4r_get_mib_stats
 */
static void cbus_vip_4r_get_mib_stats (hwidbtype *hwidb)
{
    select_if(hwidb);
    send_vip_4r_stat_request_cmd(hwidb, VIP_4R_SMT_STATS);
    send_vip_4r_stat_request_cmd(hwidb, VIP_4R_ERROR_STATS);
}


/* 
 * IDB_TR_SB_ERR_UPDATE
 *
 * hwidb->prev_foo += hwidb->foo; hwidb->foo = 0;
 */
#define IDB_TR_SB_ERR_UPDATE(foo)          \
{                                          \
   tsb->prev_ ## foo += tsb-> ## foo;      \
   tsb-> ## foo = 0;                       \
}


/*
 * MPC_ERR_UPDATE_SB2
 *
 * hwidb->prev_foo += mpc->foo; mpc->foo = 0;
 */
#define MPC_ERR_UPDATE_SB2(foo)            \
{                                          \
    tsb->prev_ ## foo += mpc-> ## foo;   \
    mpc-> ## foo = 0;                      \
}


/*
 * TX_ERR_UPDATE2
 *
 * tx_prev->foo += tx->foo; tx->foo = 0;
 */
#define TX_ERR_UPDATE2(foo)                \
{                                          \
    tx_prev-> ## foo += tx-> ## foo;       \
    tx-> ## foo = 0;                       \
}    


/*
 * RX_ERR_UPDATE2
 *
 * rx_prev->foo += rx->foo; rx->foo = 0;
 */
#define RX_ERR_UPDATE2(foo)                \
{                                          \
    rx_prev-> ## foo += rx-> ## foo;       \
    rx-> ## foo = 0;                       \
}    


/*
 * cbus_vip_4r_update_prev_stats
 */
static void cbus_vip_4r_update_prev_stats (hwidbtype *hwidb)
{
    vip4r_data_t *vip4r;    
    mpc_errors *mpc;
    tx_errors *tx, *tx_prev;
    rx_errors *rx, *rx_prev;
    tokensb	*tsb;

    tsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
    /*
     * Add ring status errors
     */
    IDB_TR_SB_ERR_UPDATE(signal_loss);
    IDB_TR_SB_ERR_UPDATE(soft_errs);
    IDB_TR_SB_ERR_UPDATE(hard_errs);
    IDB_TR_SB_ERR_UPDATE(tx_beacon);
    IDB_TR_SB_ERR_UPDATE(wire_fault);
    IDB_TR_SB_ERR_UPDATE(rmt_removal);
    IDB_TR_SB_ERR_UPDATE(single_station);
    IDB_TR_SB_ERR_UPDATE(ring_recovery);

    vip4r = INSTANCE_VIP_4R;
    if (!vip4r) 
	return;

    mpc = &vip4r->mpc_err;
    MPC_ERR_UPDATE_SB2(err_line);
    MPC_ERR_UPDATE_SB2(err_internal);
    MPC_ERR_UPDATE_SB2(err_burst);
    MPC_ERR_UPDATE_SB2(err_arifci);
    MPC_ERR_UPDATE_SB2(err_abort);
    MPC_ERR_UPDATE_SB2(err_lostframe);
    MPC_ERR_UPDATE_SB2(err_copy);
    MPC_ERR_UPDATE_SB2(err_rcvcongestion);
    MPC_ERR_UPDATE_SB2(err_token);

    tx = &vip4r->tx_err;
    tx_prev = &vip4r->tx_prev;
    TX_ERR_UPDATE2(tx_pci_read_error);
    TX_ERR_UPDATE2(tx_icd_read_parity_error);
    TX_ERR_UPDATE2(tx_icd_write_parity_error);
    TX_ERR_UPDATE2(tx_icd_addr_parity_error);
    TX_ERR_UPDATE2(tx_stat_write_error);
    TX_ERR_UPDATE2(tx_fifo_underrun);
    TX_ERR_UPDATE2(tx_fifo_parity_error);

    rx = &vip4r->rx_err;
    rx_prev = &vip4r->rx_prev;
    RX_ERR_UPDATE2(rx_pci_read_parity_error);
    RX_ERR_UPDATE2(rx_int_read_parity_error);
    RX_ERR_UPDATE2(rx_int_write_parity_error);
    RX_ERR_UPDATE2(rx_int_pci_address_bfr);
    RX_ERR_UPDATE2(rx_fifo_parity_error);
    RX_ERR_UPDATE2(rx_no_data_bfr);
    RX_ERR_UPDATE2(rx_pci_write_error);
    RX_ERR_UPDATE2(rx_fifo_overrun);
    RX_ERR_UPDATE2(rx_parity_error);
    RX_ERR_UPDATE2(rx_mpc_error);
    RX_ERR_UPDATE2(rx_imp_abrt_frm);
    RX_ERR_UPDATE2(rx_exp_abrt_frm);

    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
}


/*
 * cbus_vip_4r_show_controller
 */
static boolean cbus_vip_4r_show_controller (hwidbtype *hwidb, parseinfo *csb)
{
    vip4r_data_t *vip4r;
    boolean alive;
    leveltype level;
    uchar *version;
    smt_parms_table *smt;
    addr_table *addr_tbl; 
    mpc_errors *mpc;
    tx_errors *tx, *tx_prev;
    rx_errors *rx, *rx_prev;
    ulong func_addr;
    tokensb *tsb;
    srb_vector_table_t *srbV_p;

    vip4r = INSTANCE_VIP_4R;
    if (!vip4r) 
	return automore_quit();

    tsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
    /*
     * Interface name/status/mac address
     */
    printf("\n%s: state %s", hwidb->hw_namestring, print_hwidbstate(hwidb));
    printf("\n  current address: %e, burned in address: %e",
	   hwidb->hardware, hwidb->bia);

    if (hwidb->state == IDBS_UP) {
	level = raise_interrupt_level(NETS_DISABLE); 
	select_if(hwidb);
	send_vip_4r_stat_request_cmd(hwidb, VIP_4R_SMT_STATS);
	send_vip_4r_stat_request_cmd(hwidb, VIP_4R_ERROR_STATS);
	reset_interrupt_level(level);
    }

    /*
     * Ring Status
     */
    printf("\n  Last Ring Status: ");
    if (tsb->tr_ring_status) {
	decode_ring_status(ring_status_text, tsb->tr_ring_status);
	print_dhms(tsb->tr_status_time);
	printf(" %s (%#x)", ring_status_text, tsb->tr_ring_status);
    } 
    else 
	printf("none");
    printf("\n    Stats: soft: %d/%d, hard: %d/%d, sig loss: %d/%d",
	   tsb->soft_errs, tsb->prev_soft_errs,
	   tsb->hard_errs, tsb->prev_hard_errs,
	   tsb->signal_loss, tsb->prev_signal_loss);
    printf("\n           tx beacon: %d/%d, wire fault %d/%d, recovery: %d/%d",
	   tsb->tx_beacon, tsb->prev_tx_beacon,
	   tsb->wire_fault, tsb->prev_wire_fault,
	   tsb->ring_recovery, tsb->prev_ring_recovery);
    printf("\n           only station: %d/%d, remote removal: %d/%d",
	   tsb->single_station, tsb->prev_single_station,
	   tsb->rmt_removal, tsb->prev_rmt_removal);

    /*
     * Monitor state.
     */
    level = raise_interrupt_level(NETS_DISABLE); 
    select_if(hwidb);
    alive = (platform_vip_4r_anybody_out_there(hwidb) == FCI_RSP_OKAY);
    reset_interrupt_level(level);
    printf("\n\n  Monitor state: (%sactive)",(alive ? "" : "not "));
    version = vip_4r_version_string(vip4r->ucode_version);
    printf(", chip f/w: '%s', [bridge capable]", version);
    printf("\n    ring mode: %x", hwidb->tr_ring_mode);
    addr_tbl = &vip4r->addr_tbl;
    func_addr = GETLONG(&addr_tbl->functional_addr);
    if (func_addr) {
	printf(", internal enables: ");
	if (func_addr & TRFUNC_MON)
	    printf(" MON");
	if (func_addr & TRFUNC_BRIDGE)
	    printf(" SRB");
	if (func_addr & TRFUNC_REM)
	    printf(" REM");
	if (func_addr & TRFUNC_RPS)
	    printf(" RPS");
	if (func_addr & TRFUNC_CRS)
	    printf(" CRS/NetMgr");
    }
    printf("\n    internal functional: %08x (%08x), group: N/A (%08x)",
	   func_addr, tsb->tr_functional,
	   tsb->tr_group & ~0x80000000);

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        idbtype *swidb = firstsw_or_null(hwidb);
        srb_triplet_t srb_triplet;
        srbV_p->srb_sb_get_triplet(swidb, &srb_triplet);
        if (srb_triplet.bridgeNum <= SRB_MAX_BRIDGE_NUM) {
	    printf("\n  Bridge: local %d, bnum %d, target %d",
	           srb_triplet.localRing,
                   srb_triplet.bridgeNum,
	           srb_triplet.remoteRing);
	    printf("\n    max_hops %d, target idb: 0x%x, %s",
	       srbV_p->srb_sb_get_max_hops(swidb), hwidb->tr_bridge_idb,
	       (hwidb->tr_bridge_idb ? hwidb->tr_bridge_idb->hw_namestring
	                             : "not local"));
        }
    }

    /*
     *  MPC specific error counters
     */
    mpc = &vip4r->mpc_err;
    printf("\n    Internal controller counts:");
    printf("\n      line errors: %d/%d, internal errors: %d/%d",
	   mpc->err_line, tsb->prev_err_line,
	   mpc->err_internal, tsb->prev_err_internal);
    printf("\n      burst errors: %d/%d, ari/fci errors: %d/%d",
	   mpc->err_burst, tsb->prev_err_burst,
	   mpc->err_arifci, tsb->prev_err_arifci);
    printf("\n      abort errors: %d/%d, lost frame: %d/%d",
	   mpc->err_abort, tsb->prev_err_abort,
	   mpc->err_lostframe, tsb->prev_err_lostframe);
    printf("\n      copy errors: %d/%d, rcvr congestion: %d/%d",
	   mpc->err_copy,  tsb->prev_err_copy,
	   mpc->err_rcvcongestion, tsb->prev_err_rcvcongestion);
    printf("\n      token errors: %d/%d",
	   mpc->err_token,  tsb->prev_err_token);

    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);

    /*
     * 4R PCI errors counts
     */
    tx = &vip4r->tx_err;
    tx_prev = &vip4r->tx_prev;
    printf("\n    Microcode TX errors:");
    printf("\n      pci read %d/%d, icdb read parity %d/%d",
	   tx->tx_pci_read_error, tx_prev->tx_pci_read_error,
	   tx->tx_icd_read_parity_error, tx_prev->tx_icd_read_parity_error);
    printf("\n      icdb write parity %d/%d, icdb address parity %d/%d",
	   tx->tx_icd_write_parity_error, tx_prev->tx_icd_write_parity_error,
	   tx->tx_icd_addr_parity_error, tx_prev->tx_icd_addr_parity_error);
    printf("\n      status write %d/%d, fifo underrun %d/%d",
	   tx->tx_stat_write_error, tx_prev->tx_stat_write_error,
	   tx->tx_fifo_parity_error, tx_prev->tx_fifo_parity_error);
    printf("\n      fifo parity %d/%d",
	   tx->tx_fifo_parity_error, tx_prev->tx_fifo_parity_error);

    rx = &vip4r->rx_err;
    rx_prev = &vip4r->rx_prev;
    printf("\n     Microcode RX errors:");
    printf("\n      pci read parity %d/%d, internal read parity %d/%d",
	   rx->rx_pci_read_parity_error, rx_prev->rx_pci_read_parity_error,
	   rx->rx_int_read_parity_error, rx_prev->rx_int_read_parity_error);
    printf("\n      internal write parity %d/%d, internal address bfr parity %d/%d",
	   rx->rx_int_write_parity_error, rx_prev->rx_int_write_parity_error,
	   rx->rx_int_pci_address_bfr, rx_prev->rx_int_pci_address_bfr);
    printf("\n      fifo parity %d/%d, no buffer %d/%d",
	   rx->rx_fifo_parity_error, rx_prev->rx_fifo_parity_error,
	   rx->rx_no_data_bfr, rx_prev->rx_no_data_bfr);
    printf("\n      pci write %d/%d, fifo overrun %d/%d",
	   rx->rx_pci_write_error, rx_prev->rx_pci_write_error,
	   rx->rx_fifo_overrun, rx_prev->rx_fifo_overrun);
    printf("\n      receive parity error %d/%d",
	   rx->rx_parity_error, rx_prev->rx_parity_error);
    printf("\n      receive mpc error %d/%d",
	   rx->rx_mpc_error, rx_prev->rx_mpc_error);
    printf("\n      received implicit aborted frame %d/%d, explicit aborted frame %d/%d",
	   rx->rx_imp_abrt_frm, rx_prev->rx_imp_abrt_frm, rx->rx_exp_abrt_frm, rx_prev->rx_exp_abrt_frm);

    cbus_vip_4r_update_prev_stats(hwidb);

    /*
     * SMT stuff
     */
    smt = &vip4r->smt;
    printf("\n    Internal controller smt state:"
           "\n      Adapter MAC:     %e, Physical drop:     %08x"
	   "\n      NAUN Address:    %e, NAUN drop:         %08x"
	   "\n      Last Source      %e, Last poll:         %e",
	   addr_tbl->node_addr, smt->phys_addr, 
	   smt->up_node_addr, smt->up_phys_addr,
	   smt->source_addr, smt->poll_addr);
    printf("\n      Last MVID:       %04x,           Last attn code:    %04x"
	   "\n      Txmit priority:  %04x,           Auth Class:        %04x"
	   "\n      Monitor Error:   %04x,           Interface errors   %04x",
	   smt->major_vector, smt->att_code, 
	   smt->acc_priority, smt->auth_source_class, 
	   smt->mon_error, smt->fe_error);
    printf("\n      Correlator:      %04x,           Soft Error Timer:  %04x"
	   "\n      Local Ring:      %04x,           Ring Status:       %04x"
	   "\n      Beacon rcv type: %04x,           Beacon txmit type: %04x",
	   smt->frame_correl, smt->soft_error_time, 
	   smt->local_ring, smt->lan_status, 
	   smt->beacon_receive, smt->beacon_transmit);
    printf("\n      Beacon type:     %04x,           Beacon NAUN:       %e"
	   "\n      Beacon drop:     %08x,       Reserved:          %04x"
	   "\n      Reserved2:       %04x",
	   smt->beacon_type, smt->beacon_naun, 
	   smt->beacon_phys, *(ushort *)smt->reserve1, 
           *(ushort *)smt->reserve2);

    printf("\n    Ring sampling: %d/1000\n", vip4r->ring_util, 1000);

    return automore_quit();
}


/*
 * cbus_vip_4r_idb_init
 */
void cbus_vip_4r_idb_init (hwidbtype *hwidb)
{

    if (!is_valid_vip(hwidb))
	return;

    if (!hwidb->vip4r_info) {
	hwidb->vip4r_info = malloc_named(sizeof(vip4r_data_t), "VIP 4R Data");
    }

    hwidb->reset = (reset_t)cbus_vip_4r_reset;
    hwidb->system_configured = (system_configured_t)cbus_vip_4r_reset;
    hwidb->periodic = cbus_vip_4r_periodic;
    hwidb->shutdown = cbus_vip_4r_shutdown;
    hwidb->set_maxdgram = cbus_mci_set_maxdgram;
    hwidb->dot5_get_smt_stats = cbus_vip_4r_get_mib_stats; 
    hwidb->dot5_add_counters = cbus_vip_4r_update_prev_stats;
    hwidb->dot5_soft_reset = cbus_vip_4r_reset;
    hwidb->show_controller = cbus_vip_4r_show_controller;
    hwidb->set_extra_addrs = cbus_vip_4r_set_functional;

    reg_add_vip_setup_srb(hwidb->type, cbus_vip_4r_setup_srb, 
				      "cbus_vip_4r_setup_srb");

    reg_add_vip_modify_srb(hwidb->type, cbus_vip_4r_modify_srb, 
				       "cbus_vip_4r_modify_srb");
}


/* 
 * vip_4r_subsys_init
 */
static void vip_4r_subsys_init (subsystype* subsys)
{

    reg_add_cbus_get_ip_errors(FCI_TYPE_VIP_TRING4, get_tokenring_cbus_errors,
			       "get_tokenring_cbus_errors");

    /*
     * This is kinda gross in that VIP-4R shares this IDBTYPE with
     * TRIP, but the TRIP doesn't call reg_invoke_hwidb_setup_routine().
     * It's starting to look like we should create our own IDBTYPE. :(
     */
    reg_add_hwidb_setup_routine(IDBTYPE_CTR, cbus_vip_4r_idb_init,
				"cbus_vip_4r_idb_init");

    vip_4r_platform_init(subsys);
}


/*---------------------------------------------------------------------------
 * Subsystem Header for VIP 4R token ring Driver
 *---------------------------------------------------------------------------*/
SUBSYS_HEADER(vip_4r,			/* name */
	      1,			/* major version */
	      0,			/* minor version */
	      1,			/* edit version */
	      vip_4r_subsys_init,	/* init function */
	      SUBSYS_CLASS_DRIVER,	/* subsystem class */
	      "seq: cbus_mci",  	/* sequence list */
	      "req: cbus_mci"   	/* prerequisite list */
	      );

      
