/* $Id: if_vip_ibm2692.c,v 3.10.12.22 1996/08/28 13:06:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_ibm2692.c,v $
 *------------------------------------------------------------------
 * if_vip_ibm2692.c - Checkers/Viper specific code for 4R
 *		      IBM Token Ring PCI port adaptor
 *
 * May 1995, HC Wang
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_ibm2692.c,v $
 * Revision 3.10.12.22  1996/08/28  13:06:52  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.12.21  1996/08/26  15:11:08  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.10.12.20  1996/08/13  02:21:38  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.10.12.19  1996/07/09  06:04:10  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.10.12.18  1996/06/11  13:43:44  dcarroll
 * CSCdi59788:  Router crashes doing OIR on 4E an 4R port adapters
 * Branch: California_branch
 * Use pas_allocate_fspak() for rx_pak.
 *
 * Revision 3.10.12.17  1996/06/11  02:55:55  hwang
 * CSCdi59504:  srb packets go to process level when vip1/4r or vip2/4r is
 * input int
 * Set multicast type only if functional or group address match, if frame
 * has rif field and we are configured for srb just set bridge bit and
 * return. We have to do exactly what trip does.
 * Branch: California_branch
 *
 * Revision 3.10.12.16  1996/06/07  02:18:56  sthormod
 * CSCdi59800:  c7200 4r needs to recover from adapter check
 * Branch: California_branch
 *
 * Revision 3.10.12.15  1996/06/06  12:03:58  dlobete
 * CSCdi57944:  DFS confused about rx ignores and tx drops
 * Branch: California_branch
 *
 * Revision 3.10.12.14  1996/06/04  05:01:04  hwang
 * CSCdi59342:  VIP2 interface gets into reset always
 * Branch: California_branch
 *
 * Revision 3.10.12.13  1996/05/28  17:41:55  hwang
 * CSCdi55620:  Missing idb_release_hwsb_inline in 4R driver
 * Add missing idb_release_hwsb_inline, if we are not changing data in
 * sub block don't use "idb_use_hwsb_inline" use "idb_get_hwsb_inline"
 * instead.
 * Branch: California_branch
 *
 * Revision 3.10.12.12  1996/05/26  00:18:18  hwang
 * CSCdi58206:  srb testing locks up svip
 * Use particle based pak to xfr MAC frame passed by Hermon/MPC using arb
 * command.
 * Branch: California_branch
 *
 * Revision 3.10.12.11  1996/05/24  21:55:03  hwang
 * CSCdi57571:  4R failed concurrent routing/bridging
 * Classify packet as "bridge type" only with transparent bridge copied
 * or source route match copied LLC frame.
 * Branch: California_branch
 *
 * Revision 3.10.12.10  1996/05/21  10:00:08  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.12.9  1996/05/17  11:40:12  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.10.2.3  1996/05/05  23:43:24  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.10.2.2  1996/04/26  15:22:45  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.10.2.1  1996/04/03  21:14:47  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.10.12.8  1996/05/13  04:58:12  hwang
 * CSCdi57003:  4R DFS support
 * Branch: California_branch
 *
 * Revision 3.10.12.7  1996/04/23  02:23:59  sthormod
 * CSCdi54994:  4R uses cpu to byte-swap
 * Branch: California_branch
 *
 * Revision 3.10.12.6  1996/04/05  05:32:32  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.10.12.5  1996/03/30  01:30:47  hwang
 * CSCdi51725:  4r driver need to provide lan manager support
 * Branch: California_branch
 * Set up proper address match bit when processing mac frame, note that
 * we have two different path to receive a mac frame, either via normal
 * LLC channel or using ARB.
 *
 * Revision 3.10.12.4  1996/03/28  02:00:22  sthormod
 * CSCdi52811:  4R SMT stats not displayed
 * Branch: California_branch
 *
 * Revision 3.10.12.3  1996/03/27  19:47:07  hwang
 * CSCdi52699:  pma pci 16 bit read work around
 * Branch: California_branch
 *
 * Revision 3.10.12.2  1996/03/21  23:23:56  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.10.12.1  1996/03/18  21:34:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.10  1996/03/18  23:29:09  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.9  1996/03/13  16:53:12  sthormod
 * CSCdi51335:  SegV exception while configuring decnet for tokenring
 * Branch: ELC_branch
 * Don't forget to bit swap those mac addresses.
 *
 * Revision 3.2.2.8  1996/03/10  02:49:59  sthormod
 * CSCdi51196:  add tbridge switching path for predator 4R
 * Branch: ELC_branch
 *
 * Revision 3.2.2.7  1996/03/06  19:51:51  sthormod
 * CSCdi50859:  add srb switching path for predator 4R
 * Branch: ELC_branch
 * o  process & fast switching path for srb & explorers.
 * o  routing with rif field.
 * o  use dma to coalesce particle based packet.
 * o  process mac frames.
 *
 * Revision 3.2.2.6  1996/02/10  00:30:57  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.5  1996/02/08  08:48:13  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.4  1996/01/31  01:37:37  sthormod
 * Branch: ELC_branch
 * Add Predator 4R support.
 *
 * Revision 3.2.2.3  1996/01/16  01:43:40  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.2  1995/12/19  23:32:39  sthormod
 * Branch: ELC_branch
 * Initial structure changes for porting 4R driver to predator.
 *
 * Revision 3.2.2.1  1995/12/06  04:08:54  sthormod
 * Branch: ELC_branch
 * Bring in HC's code review changes.
 * Revision 3.10  1996/02/14  22:02:14  hwang
 * CSCdi48583:  vines packets not forwarded in 4r-4r transparent bridge
 * Reduce the size of reject address table, if tb is configured xmt one
 * packet at a time (no chained packet to avoid rejection addr tbl
 * overflow). When access the packet memory use cpu view address instead
 * of pci byte swap address.
 *
 * Revision 3.9  1996/02/05  03:37:12  hwang
 * CSCdi47198:  router with rvip (4R & 4T) hang after config SRT
 * ibm token ring chip if configured to transparent bridge mode it will
 * copy broad cast frame it transmitted, which causing the network
 * flooding.
 * Implement software filter to drop such frame received.
 *
 * Revision 3.8  1996/01/11  19:39:35  hwang
 * CSCdi46480:  4R failed hot standby test
 * vip_token_classify function to return device stat in trip style.
 * Remove some unused error messages.
 *
 * Revision 3.7  1995/12/21  17:49:20  hwang
 * CSCdi45680:  appletalk over SRB using RVIP fails
 * Enable limited RI broadcast copy, only screening all
 * bridge multicast packet.
 *
 * Revision 3.6  1995/12/17  18:36:02  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.5  1995/12/14  19:42:10  hwang
 * Modify packet classify function to provide correct tb/srb type packet,
 * use latest ibm info to set the hidden copy_all_options in modify_receive
 * option srb command.
 * CSCdi43509:  srb bridging over rvip fails
 *
 * Revision 3.4  1995/12/10  04:01:29  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.3  1995/12/03  06:39:42  hwang
 * CSCdi44890:  incorporate code review comments into 4r driver
 *
 * Revision 3.2  1995/11/17  18:54:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "subsys.h"
#include "parser.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../h/ieee.h"

#include "../if/ether.h"
#include "../if/pci.h"
#include "../if/network.h"
#include "../if/tring_common.h"
#include "../if/tring.h"
#include "../if/tring_private.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "../hes/ccb_cmd.h"
#include "../hes/if_fci.h"
#include "../hes/if_vip.h"  /* for loveletter codes */

#include "../srt/srb_core.h"

#include "if_pas.h"
#include "if_pas_ibm2692.h"
#include "msg_ibm2692.c"		/* Not a typo, see logger.h */
#include "msg_pas.c"			/* Not a typo, see logger.h */


/*
 * ibm2692_idb_create 
 */
hwidbtype *ibm2692_idb_create (uint slot, uint subunit)
{
    hwidbtype           *idb;

    idb = idb_create();
    if (idb) {
	idb->oir_flags |= (IDB_FIRST_INIT | IDB_FINAL_INIT);
    }
    return (idb);
}

/*
 * ibm2692_deallocate
 */
void ibm2692_deallocate (hwidbtype *idb)
{
}

/*
 * ibm2692_show_instance
 */
void ibm2692_show_instance (ibm2692_instance_t *ds)
{
}

/*
 * ibm2692_snmp_link_trap 
 */
void ibm2692_snmp_link_trap (int trap, hwidbtype *idb)
{
}

/*
 * ibm2692_lineproto_change
 */
void ibm2692_lineproto_change (hwidbtype *idb, boolean state)
{
    vip_lineproto_change(idb, state);
}

/*
 * ibm2692_get_default_hw_address
 */
void ibm2692_get_default_hw_address (hwidbtype *idb, uchar addr[IEEEBYTES])
{
}

/*
 * ibm2692_init_buffer_pools
 */
boolean ibm2692_init_buffer_pools (hwidbtype *idb)
{
    ibm2692_instance_t *ds;
    pooltype *pool;
    int particles_in_pool;
    mempool *buffer_mempool;

    /*
     * init the particle pool
     */
    if (!idb->pool) {
	/*
	 * see  which memory pool this interfaces buffers should use
	 * and create the buffer pool.
	 */
	buffer_mempool = pas_buffer_mempool(idb);

	if (idb->pool_group == POOL_GROUP_INVALID)
            idb->pool_group = pool_create_group();

	pool = particle_pool_create(idb->hw_namestring,
				    idb->pool_group,
				    IBM2692_PARTICLE_SIZE,
				    POOL_SANITY,
				    32,
				    buffer_mempool);
        if (!pool) {
	    return(FALSE);
	}
	/*
	 * Add a fallback pool
	 */
        pool->fallback = pas_interface_fallback_pool(idb);

	/*
	 * Populate the pool and add a cache
	 */
        particles_in_pool = IBM2692_RX_RING_ENTRIES * 2;
	pool_adjust(pool, 0, particles_in_pool, particles_in_pool, TRUE);
	particle_pool_create_cache(pool, particles_in_pool);
	pool_adjust_cache(pool, particles_in_pool);
	idb->pool = pool;
    }
    /*
     * Init the header pool
     */
    if (!idb->headerpool) {
	idb->headerpool = pas_interface_header_pool(idb);
    }
    /*
     * Init the static rx_pak
     */
    ds = INSTANCE(idb);
    if (!ds->rx_pak) {
	ds->rx_pak = pas_allocate_fspak();
	if (!ds->rx_pak) {
	    return(FALSE);
	}
	ibm2692_cleanup_pak(ds->rx_pak);
    }
    return(TRUE);
}

/*
 * send_ibm2692_smt_stats
 */
static void send_ibm2692_smt_stats (hwidbtype *hwidb)
{
    ibm2692_instance_t *ds;
    vip_4r_smt_ll ll;

    ds = (ibm2692_instance_t *)INSTANCE(hwidb);
    ll.ll_type = VIP_LL_4R_SMT_STATS;
    bcopy(&ds->mpc_addr_table, &ll.addr_tbl, sizeof(addr_table));
    bcopy(&ds->mpc_parms_table, &ll.smt, sizeof(smt_parms_table));
    bcopy(&ds->mpc_ucode_level, &ll.ucode_version, MAC_VERS_LEN);
    PUTSHORT(&ll.ring_util, ds->mpc_utilization);
    if (!vip_send_loveletter(&ds->pacb, (uchar *)&ll,
				sizeof(vip_4r_smt_ll))) {
	if (tokenevent_debug)
	    buginf("\n%s: send_ibm2692_smt_stats: vip_send_loveletter() failed",
		     hwidb->hw_namestring);
    }
}

/*
 * send_ibm2692_errors
 */
static void send_ibm2692_errors (hwidbtype *hwidb)
{
    ibm2692_instance_t *ds;
    vip_4r_errors_ll ll;

    ds = (ibm2692_instance_t *)INSTANCE(hwidb);
    ll.ll_type = VIP_LL_4R_ERROR_STATS;
    bcopy(&ds->mpc_err, &ll.mpc_err, sizeof(mpc_errors));
    memset(&ds->mpc_err, 0, sizeof(mpc_errors));
    bcopy(&ds->tx_err, &ll.tx_err, sizeof(tx_errors));
    memset(&ds->tx_err, 0, sizeof(tx_errors));
    bcopy(&ds->rx_err, &ll.rx_err, sizeof(rx_errors));
    memset(&ds->rx_err, 0, sizeof(rx_errors));
    if (!vip_send_loveletter(&ds->pacb, (uchar *)&ll,
				sizeof(vip_4r_errors_ll))) {
	if (tokenevent_debug)
	    buginf("\n%s: send_ibm2692_errors: vip_send_loveletter() failed",
		     hwidb->hw_namestring);
    }
}

/* 
 * set_config_info : store configuration info into rvip idb
 */
static void set_config_info (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    ibmtr_cfg_data_t  *cfg;
    tokensb           *tsb;
    srb_triplet_t     srb_triplet;

    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    cfg = & ds->config_data;
    ieee_copy(cfg->hardware, idb->bia);
    ieee_copy(idb->bia, idb->hardware);
    tsb->tr_functional = cfg->tr_functional;
    if (tokenevent_debug) {
	buginf("\n%s: ring mode %x", idb->hw_namestring, cfg->tr_ring_mode);
    }
    idb->tr_ring_mode = cfg->tr_ring_mode;
    tsb->tr_group = cfg->tr_group;
    idb->tr_ring_speed = cfg->tr_ring_speed;
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);

    srb_triplet.localRing  = cfg->srb_thisring;
    srb_triplet.bridgeNum  = cfg->srb_bridge_num;
    srb_triplet.remoteRing = cfg->srb_targetring;
    srbV.srb_sb_set_triplet(idb->firstsw, &srb_triplet);

    switch (cfg->config_type) {
    case INITIAL_CONFIG :
	break;
    case FUNCTIONAL_ADDR_CHANGE :
	set_ibm2692_functional_address(idb);
	break;
    default :
	if (tokenevent_debug) {
	    buginf("\n%s unknown config block received", idb->hw_namestring);
	}
	break;
    }
}

/*
 * ibm2692_cmd_dispatch
 */
static void ibm2692_cmd_dispatch (malu_cmd_t *cmd)
{
    pas_cbtype *pascb = cmd->pascb;
    hwidbtype *idb;
    ibm2692_instance_t *ds;

    if (!pascb) {
	errmsg(&msgsym(BADPASCB, PA), pascb);
	return;
    }

    idb = pascb->idb;
    if (!idb) {
	errmsg(&msgsym(BADIDB, PA), idb);
	return;
    }

    ds = INSTANCE(idb);
    switch (cmd->cmd) {
        case CCB_CMD_VIP_4R_START_BLOCK:   /* 11 */
	    if (!ds->cfg_in_progress) {
	        ds->cfg_write_ptr = (ushort *)&ds->config_data;
	        ds->cfg_in_progress = TRUE;
	        ds->cfg_write_count = 0;
	    } else if (tokenevent_debug) {
	        buginf("\n %s: CFG start cmd received with cfg_in_progress set",
	            idb->hw_namestring);
	    }
	    break;
        case CCB_CMD_VIP_4R_WRITE_BLOCK:   /* 12 */
	    if ((ds->cfg_in_progress) && 
		(ds->cfg_write_count < CFG_DATA_SIZE)) {
	        *ds->cfg_write_ptr++ = cmd->arg0;
	        ds->cfg_write_count++;
	    } else if (tokenevent_debug) {
	        if (!ds->cfg_in_progress) {
		    buginf("\n%s: CFG write cmd but cfg_in_progress not set",
			    idb->hw_namestring);
		}
	        if (ds->cfg_write_count >= CFG_DATA_SIZE) {
		    buginf("\n%s: receive more cfg data than expected",
			    idb->hw_namestring);
		}
	    }
	    break;
        case CCB_CMD_VIP_4R_END_BLOCK:     /* 13 */
	    if (ds->cfg_in_progress) {
	        ds->cfg_in_progress = FALSE;
	        set_config_info(idb);
	    } else if (tokenevent_debug) {
	        buginf("\n%s: CFG_DONE cmd received but cfg_in_progress not set",
		        idb->hw_namestring);
	    }
	    break;
        case CCB_CMD_VIP_4R_REQUEST_STATS:  /* 14 */
	    switch (cmd->arg0) {
	        case VIP_4R_SMT_STATS:
	            send_ibm2692_smt_stats(idb);
	            break;
	        case VIP_4R_ERROR_STATS:
	            send_ibm2692_errors(idb);
	            break;
	    }
	    break;
        case CCB_CMD_STATUS:  /* 32 */
	    vip_control_interface(idb, cmd);
	    break;
        case CCB_CMD_ERRORS:  /* 38 */
	    vip_send_interface_stats(idb, cmd);
	    break;
        case CCB_CMD_DELAY:   /* 45 */
	    break;
        case CCB_CMD_RXBYTES: /* 48 */
	    if_maxdgram_adjust(idb, cmd->arg0);
	    break;
        default:
	    if (tokenevent_debug) {
	        buginf("\n%s: unsupported malu command %d arg %d received", 
		        idb->hw_namestring, cmd->cmd, cmd->arg0);
	    }
	    break;
    }
}

/*
 * ibm2692_platform_init
 * platform dependant initialization
 */
boolean ibm2692_platform_init (hwidbtype *idb)
{
    pas_cbtype *pascb = INSTANCE(idb);
    ibm2692_instance_t *ds = INSTANCE(idb);

    /*
     * setup fci_type
     */
    idb->fci_type = FCI_TYPE_VIP_TRING4;

    /*
     * perform common vip interface initialization
     */
    vip_interface_init(idb);

    /*
     * 4R driver use keepalive to collect stat from mpc
     */
    idb->nokeepalive = FALSE;

    /*
     * install malu command dispatch routine
     */
    install_cmd_dispatch(idb, ibm2692_cmd_dispatch);

    /*
     * Install the polling limit and counter pointer.
     */
    pascb->tx_polling_high = IBM2692_TXPOLL_HIGH;
    pascb->tx_polling_low = IBM2692_TXPOLL_LOW;
    pascb->txring_cnt_ptr = &ds->tx_outstanding;

    /*
     * install this interface into pa_jumptable
     */
    pas_install_port_instance(idb, TRUE);
    return(TRUE);
}

/*
 * vip_token_classify
 */
static ushort vip_token_classify (ulong stat, uchar *ptr, 
				  hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    llc1_hdr_t  *llc1;
    tring_hdr	*mac;
    ushort pkt_status = 0x0;
    int len;

    /*
     * rx_status bit 0-7 should contain (trip style) chip specific info
     * bit 8-15 contain address filter info
     */
    pkt_status |= (ushort)(stat & ADDR_MATCH_MASK);

    /*
     * check for MAC frame first
     */
    mac = (tring_hdr *)ptr;
    if (!(mac->fc & FRAME_TYPE_MASK)) { /* this is MAC frame */
	pkt_status |= MAC_TYPE | RXSTAT_MAC;
	return(pkt_status);
    }

    /*
     * Set frame type based on address match bits from the MPC
     */
    if (stat & RX_IMAT) {
	pkt_status |= STATION_TYPE;
    } else if (stat & (RX_GMAT | RX_FMAT)) {
        pkt_status |= MULTICAST_TYPE;
    }

    /*
     * if we are in tb or srb mode
     */
    if (ds->bridge_mode) {

        /*
         * MPC does not set status bits correctly for broadcast frames
         * received while in bridging mode.  Since we are mirroring 
         * TRIP "bf_rx_status", we must set those bits here so the
         * RSP fast switching logic will work correctly.
         */
        if (is_ieee_bcast(mac->daddr)) 
            pkt_status |= (MULTICAST_TYPE | DEST_ADDR_MATCH);

	/*
	 * if we are in tb mode and received multicast, broadcast type packet
	 */
	if ((stat & TRANB_ADDR_MATCH) && (pkt_status & MULTICAST_TYPE)) {
	    /*
	     * need to check for packet forwarded by this station
	     */
	    if (in_reject_list(idb, &mac->saddr[0])) {
		pkt_status = 0x0;
		return(pkt_status);
	    }
	    pkt_status |= BRIDGING_TYPE;
	    return(pkt_status);
	} else if (stat & ROUTE_ADDR_MATCH) {
	    pkt_status |= BRIDGING_TYPE;
	    return(pkt_status);
	}
    } 

    /*
     * check for source route info present
     */
    if (mac->saddr[0] & RI_INDICATION) {
	/*
	 * it has routing info
	 */
        len = mac->data[0] & 0x1f;      /* length is 5 bit wide */
	llc1 = (llc1_hdr_t *)(mac + len);
    } else {
	llc1 = (llc1_hdr_t *)&mac->data[0];
    }

    /*
     * llc1 pointed to start of llc header
     */
    if (llc1->dsap == SAP_SNAP) {
	switch (llc1->type) {
	    case TYPE_IP10MB:
	    case TYPE_RFC826_ARP:
		pkt_status |= DOD_IP_TYPE;
		break;
	    case TYPE_DECNET:
		pkt_status |= DECNET_TYPE;
		break;
	    case TYPE_XNS:
		 pkt_status |= XNS_TYPE;
		 break;
	    default:
		 break;
	}
    }
    return(pkt_status);
}

/*
 * ibm2692_process_receive_packet
 */
void ibm2692_process_receive_packet (hwidbtype *idb, ulong stat, 
					paktype *pak)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    int pak_size;
    int fcs_fragment_size;
    particletype *new_particle;
    particletype *last_particle;

    pak_size = pak->datagramsize;
    pak->mci_status = vip_token_classify (stat, pak->datagramstart, idb);
    if (tokenring_debug) {
	buginf("\n%s: pkt size %d  stat %x status %x\n", idb->hw_namestring,
		pak_size, stat, pak->mci_status);
    }
    if (!pak->mci_status) {
        ibm2692_cleanup_pak(pak);
	idb->counters.input_drops++;
	return;
    }
    /*
     * check if DFS support is enabled
     */
    if (ibm2692_dfs_enabled(idb) &&
	((pak->mci_status & CLASSIFY_MASK) == (DOD_IP_TYPE | STATION_TYPE))) {
	pak->network_start = pak->datagramstart + TRING_SNAP_ENCAPBYTES;
	if (!ibm2692_ip_dfs(idb, pak)) {
	    ibm2692_cleanup_pak(pak);
	}
	ibm2692_flush_pak(pak);
	return;
    }
    /*
     * Either DFS is turned off or the packet was not a supported protocol
     * or this packet is destined to an interface not on this vip, punt it
     * to the host.
     * Note: the host expects the token ring FCS to be in the packet data buffer.
     * So, we must add it back on here.  Switching logic in the host will
     * do the right thing if the FCS is eventually not needed.
     */
    last_particle = pak->particlequeue.qtail;
    if (last_particle->data_bytes <= (IBM2692_PARTICLE_SIZE - IBM2692_CRC_SIZE)) {
        /*
         * Entire FCS is still left in the particle.  Just adjust the
         * particle's data length to pick up the FCS.
         */
        last_particle->data_bytes += IBM2692_CRC_SIZE;
    } else {
        /*
         * Only a fragment (or none) of the FCS still left in the particle.  Get the
         * remainder from the FCS save area and add it on in a new particle.
         */
        fcs_fragment_size = IBM2692_CRC_SIZE - 
            (IBM2692_PARTICLE_SIZE - last_particle->data_bytes);
        new_particle = ibm2692_get_particle_inline(idb, TRUE);
        if (new_particle) {
            ibm2692_particle_enqueue_inline(pak, new_particle);
            last_particle->data_bytes = IBM2692_PARTICLE_SIZE;
            new_particle->data_bytes = fcs_fragment_size;
            *new_particle->data_start = ds->fcs_save;
        } else {
            idb->counters.input_resource++;
            ibm2692_cleanup_pak(pak);
            return;
        }
    }
    pak->datagramsize += IBM2692_CRC_SIZE;
        
    if (!(*ds->pacb.pak_to_host)(idb, &ds->pacb, pak)) {
        idb->counters.input_drops++;
        ibm2692_cleanup_pak(pak);
    } else {
        idb->counters.rx_cumbytes += pak_size;
        idb->counters.inputs += 1;
    }
    ibm2692_flush_pak(pak);
}

/* 
 * ibm2692_process_mac_frame
 */
void ibm2692_process_mac_frame (hwidbtype *idb, paktype *pak)
{
    paktype *mac_pak;

    /*
     * convert contiguous pak to scatter pak
     */

    mac_pak = vip_message2buffer(idb, pak->datagramstart, pak->datagramsize);
    datagram_done(pak);
    if (!mac_pak) {
	return;
    }
    ibm2692_process_receive_packet(idb, 1, mac_pak);
    datagram_done(mac_pak);
}

/*
 * ibm2692_get_smt_stats 
 */
void ibm2692_get_smt_stats (hwidbtype *hwidb)
{
    /* NA for VIP */
}

/*
 * ibm2692_add_counters
 */
void ibm2692_add_counters (hwidbtype *hwidb)
{
    /* NA for VIP */
}

void ibm2692_adp_chk (hwidbtype *hwidb)
{
    vip_4r_err_ll ll;
    ibm2692_instance_t *ds = INSTANCE(hwidb);
    int i;
    
    ll.ll_type = VIP_LL_4R_ERR_MSG;
    ll.err_type = VIP_4R_ADP_CHK;
    for (i = 0; i < 4; i++) {
	ll.type.adp_chk.data[i] = ds->adp_chk_data[i];
    }
    if (!vip_send_loveletter(&ds->pacb, (uchar *)&ll,
			     sizeof(vip_4r_err_ll))) {
	buginf("\n%s: failed to send adapter check love letter",
	       hwidb->hw_namestring);
    }
    ibm2692_reset(hwidb);
    net_cstate(hwidb, IDBS_RESET);
}

void ibm2692_init_fail (hwidbtype *hwidb, ushort ret_code)
{
    vip_4r_err_ll ll;
    ibm2692_instance_t *ds = INSTANCE(hwidb);

    ll.ll_type = VIP_LL_4R_ERR_MSG;
    ll.err_type = VIP_4R_INIT_FAIL;
    ll.type.init.bring_up_code = ret_code;
    bcopy(&ds->mpc_ucode_level, &ll.type.init.version, 10);
    if (!vip_send_loveletter(&ds->pacb, (uchar *)&ll, sizeof(vip_4r_err_ll))) {
	if (tokenevent_debug) {
	    buginf("\n%s: failed to send init fail love letter",
		   hwidb->hw_namestring);
	}
    }
}

void ibm2692_lan_status_change (hwidbtype *hwidb, ushort status)
{
    vip_4r_err_ll ll;
    ibm2692_instance_t *ds = INSTANCE(hwidb);

    ll.ll_type = VIP_LL_4R_ERR_MSG;
    ll.err_type = VIP_4R_LAN_STATUS;
    ll.type.lan_status_change.lan_status = status;
    if (!vip_send_loveletter(&ds->pacb, (uchar *)&ll,
			     sizeof(vip_4r_err_ll))) {
	if (tokenevent_debug) {
	    buginf("\n%s: failed to send ring status love letter",
		   hwidb->hw_namestring);
	}
    }
}

void ibm2692_open_fail (hwidbtype *hwidb)
{
    vip_4r_err_ll ll;
    ibm2692_instance_t *ds = INSTANCE(hwidb);
    
    ll.ll_type = VIP_LL_4R_ERR_MSG;
    ll.err_type = VIP_4R_OPEN;
    ll.type.open.ret_code = ds->mpc_srb.open_response.ret_code;
    ll.type.open.open_error_code =
	ds->mpc_srb.open_response.open_error_code; 
    bcopy(&ds->mpc_ucode_level, &ll.type.open.version, MAC_VERS_LEN);
    if (!vip_send_loveletter(&ds->pacb, (uchar *)&ll, sizeof(vip_4r_err_ll))) {  
	buginf("\n%s: failed to send open fail love letter",
	       hwidb->hw_namestring);
    }
}

void ibm2692_open_okay (hwidbtype *hwidb)
{
    vip_4r_err_ll ll;
    ibm2692_instance_t *ds = INSTANCE(hwidb);

    ll.ll_type = VIP_LL_4R_ERR_MSG;
    ll.err_type = VIP_4R_OPEN;
    ll.type.open.ret_code = ds->mpc_srb.open_response.ret_code;
    ll.type.open.open_error_code = 0;
    bcopy(&ds->mpc_ucode_level, &ll.type.open.version, MAC_VERS_LEN);
    if (!vip_send_loveletter(&ds->pacb, (uchar *)&ll, sizeof(vip_4r_err_ll))) {
	buginf("\n%s: failed to send open fail love letter",
	       hwidb->hw_namestring);
    }
}

void ibm2692_cfg_brg_fail (hwidbtype *hwidb)
{
    vip_4r_err_ll ll;
    ibm2692_instance_t *ds = INSTANCE(hwidb);

    ll.ll_type = VIP_LL_4R_ERR_MSG;
    ll.err_type = VIP_4R_CFG_BRG_FAIL;
    ll.type.cfg_brg.ret_code = ds->mpc_srb.configure_bridge.ret_code;
    if (!vip_send_loveletter(&ds->pacb, (uchar *)&ll, sizeof(vip_4r_err_ll))) {
	buginf("\n%s: failed to send init fail love letter",
	       hwidb->hw_namestring);
    }
}

/*
 * ibm2692_rev_okay
 */
boolean ibm2692_rev_okay (int pa_bay)
{
    /*
     * This is currently always true for vip.
     */
    return TRUE;
}

/* end of file */
