/* $Id: if_vip_s4t68360.c,v 3.13.6.15 1996/08/28 13:06:58 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_s4t68360.c,v $
 *------------------------------------------------------------------
 * if_vip_s4t68360.c - VIP PCI Adaptor s4t media driver
 *
 * 3/02/95   H.M. Robison
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_s4t68360.c,v $
 * Revision 3.13.6.15  1996/08/28  13:06:58  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.6.14  1996/08/23  04:47:32  xtang
 * CSCdi59553:  clockrate 250000 is not configurable on FSIP in 75xx
 * Branch: California_branch
 *              added 250000 Hz clockrate to s4t68360 4T
 *              port adaptor. The corresponding microcode change are in
 *              cvs -d /micro s4t68360
 *
 * Revision 3.13.6.13  1996/08/13  02:21:41  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.13.6.12  1996/08/06  02:59:17  rramacha
 * CSCdi64889:  4T interface erroneously shows WFQ at vip console
 * Branch: California_branch
 *
 * Revision 3.13.6.11  1996/06/18  19:25:59  gstovall
 * CSCdi58572:  Serial line comes back from the dead
 * Branch: California_branch
 * Check physical line state before deciding if the line is up or not.
 *
 * Revision 3.13.6.10  1996/06/11  13:43:47  dcarroll
 * CSCdi59788:  Router crashes doing OIR on 4E an 4R port adapters
 * Branch: California_branch
 * Use pas_allocate_fspak() for rx_pak.
 *
 * Revision 3.13.6.9  1996/06/07  05:10:40  getchell
 * CSCdi59206:  EMT_RELOAD, EMT_CPU_EXCEPTION does call svip_reload()
 * Branch: California_branch
 *
 * Revision 3.13.6.8  1996/06/07  04:08:21  amcrae
 * CSCdi56758:  4T does not recover after power surge - should be restarted
 * Branch: California_branch
 * Added structure to carry 4T PA data. Added periodic watchdog check
 * and restart handling. Nuked some unused counters.
 *
 * Revision 3.13.6.7  1996/05/22  23:28:18  rramacha
 * CSCdi58249:  Enable IP DFS on 4T
 * Branch: California_branch
 *  o receive serial encap cmd
 *  o turn-on DFS on 4T
 *  o decode HDLC, PPP encapsulation on the received packet
 *
 * Revision 3.13.6.6  1996/05/06  16:43:15  gstovall
 * CSCdi54863:  Priority queuing not working on new interface
 * Branch: California_branch
 * Allow the various queueing options to be enabled on the 4T in a
 * predator.
 *
 * Revision 3.13.6.5  1996/04/30  16:50:07  gstovall
 * CSCdi55513:  Fix OIR problems
 * Branch: California_branch
 * Fix oir memory handling on 4t.
 *
 * Revision 3.13.6.4  1996/04/10  02:43:28  rramacha
 * CSCdi52294:  Add 4T support for VIP2
 * Branch: California_branch
 * VIP2 support for 4T
 *
 * Revision 3.13.6.3  1996/04/05  05:32:35  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.13.6.2  1996/03/21  23:24:03  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.13.6.1  1996/03/18  21:34:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.10  1996/03/18  23:29:11  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.9  1996/03/08  22:10:06  gstovall
 * CSCdi51111:  Need new 4T firmware
 * Branch: ELC_branch
 * Suck in some of Andrews magic firmware, plus update the driver to deal
 * with it.
 *
 * Revision 3.3.2.8  1996/02/14  01:58:15  gstovall
 * CSCdi48946:  Pull in 4T bugfixes from mainline
 * Branch: ELC_branch
 * Bring the ELC 4T driver up to date.
 *
 * Revision 3.3.2.7  1996/02/10  00:30:59  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3.2.6  1996/02/08  08:48:18  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.5  1996/01/26  02:50:15  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.3.2.4  1996/01/24  04:16:50  gstovall
 * Branch: ELC_branch
 * Pull in some bug fixes from the 11.1 mainline, make fastswitching work
 * AND count correctly, notice if a cable gets pulled.
 *
 * Revision 3.3.2.3  1996/01/18  22:11:20  gstovall
 * Branch: ELC_branch
 * Preadator now knows the joy and splendor of serial interfaces.
 *
 * Revision 3.3.2.2  1996/01/16  01:43:51  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/11  19:13:59  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.13  1996/03/05  07:28:38  amcrae
 * CSCdi50610:  4T interfaces die at low temperatures
 * Moved transmit start to after ring update.
 * Rearranged firmware counters.
 *
 * Revision 3.12  1996/02/11  20:24:15  rharagan
 * CSCdi48311:  4T PA will hang rvip if download fails
 * Read back cksm after '360 down load, purge magic numbers from init block.
 *
 * Revision 3.11.2.1  1996/01/24  22:48:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.11  1996/01/20  04:59:30  amcrae
 * CSCdi47215:  4T exception processing doesnt signal host
 * CSCdi44907:  Fixed macros overlooked in previous commit
 *
 * Revision 3.10  1996/01/13  03:24:13  amcrae
 * CSCdi45724:  4T f/w is not answering mbx msgs during an interface reset
 * CSCdi43260:  VIP 4T has no implementation for transmit-delay
 * New interrupt scheme added. Firmware counter update added.
 *
 * Revision 3.9  1995/12/23  01:14:35  rharagan
 * CSCdi45811:  4T interface linestate does not refletct modem control
 * lines
 * Straighten out changes in linestate, continue to bring in code review
 * comments
 *
 * Revision 3.8  1995/12/19  23:20:57  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Add code review comments.
 *
 * Revision 3.7  1995/12/19  20:24:38  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.6  1995/12/14  07:41:09  rharagan
 * CSCdi42045:  tx0 output hung (800E - queue full) when pinging on serial
 * 4T
 * Strip out now unneeded tx pkt fragmentation, lose unwieldy ddelays on
 * mbx reads.
 *
 * Revision 3.5  1995/12/10  04:01:31  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.4  1995/12/02  05:50:28  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.3  1995/11/17  18:55:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:58:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "sys_registry.h"
#include "packet.h"
#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../hes/if_fci.h"
#include "../hes/if_mci.h"
#include "../hes/ccb_cmd.h"
#include "../hes/if_vip.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../if/network.h"

#include "ieee.h"
#include "../wan/ppp.h"
#include "../wan/smds.h"
#include "../wan/atm_dxi.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "../if/serial_inline.h"

#include "if_pas.h" 
#include "if_pas_s4t68360.h"
#include "msg_pas.c"			/* Not a typo, see logger.h */

/*
 * s4t_get_particle:
 * Get an rx particle for the device
 */
particletype *s4t_get_particle (hwidbtype *idb)
{
    return(s4t_get_particle_inline(idb, TRUE));
}

/*
 * s4t_update_port_status
 * Fill in port_status field in love-letter bound for RP/RSP.
 *
 */
static void s4t_update_port_status (hwidbtype *hwidb, ulong *port_status)
{
    s4t_instance_t *ds;
    ds = INSTANCE(hwidb);
    /* using fsip modem control status above for proper display */
    *port_status = (ds->port_status & S4T_STATUS_RTS_CTS) ?
	FSIP_MASK_CTS : 0;
    *port_status |= (ds->port_status & S4T_STATUS_DTR_DSR) ?
	FSIP_MASK_DSR : 0;
    *port_status |= (ds->port_status & S4T_STATUS_DCD) ?
	FSIP_MASK_DCD : 0;
    if (hwidb->state == IDBS_UP) {
	*port_status |= (FSIP_MASK_RTS|FSIP_MASK_DTR);
    } else {
	*port_status &= ~(FSIP_MASK_RTS|FSIP_MASK_DTR);
    }
}

/*
 * s4t_get_applique_id
 * Return hes-friendly applique_id.
 *
 */
static ulong s4t_get_applique_id (hwidbtype *hwidb)
{
    s4t_instance_t *ds = INSTANCE(hwidb);
    
    /*
     * Let's convert for the master's (RP/RSP's) sake.
     */
    switch (ds->applique_id) {

      case S4T_PA_NO_CABLE:
	return MCI_UNIVERSE_NO_CABLE;

      case S4T_PA_RS232_DTE:
	return MCI_DTE_RS232;

      case S4T_PA_RS232_DCE:
	return MCI_DCE_RS232;

      case S4T_PA_V35_DTE:
	return MCI_DTE_V35;

      case S4T_PA_V35_DCE:
	return MCI_DCE_V35;

      case S4T_PA_RS449_DTE:
	return MCI_DTE_RS449;

      case S4T_PA_RS449_DCE:
	return MCI_DCE_RS449;

      case S4T_PA_X21_DTE:
	return MCI_DTE_X21;

      case S4T_PA_X21_DCE:
	return MCI_DCE_X21;

      case S4T_PA_RS530_DTE:
	return MCI_DTE_RS530;

      case S4T_PA_RS530_DCE:
	return MCI_DCE_RS530;

      case S4T_PA_NONE:
      default:
        return MCI_APP_UNKNOWN;
    }
}

/*
 * s4t_send_status
 * Create a love letter, fill it with 4t-specific info and send it.
 *
 */
static void s4t_send_status (hwidbtype *hwidb)
{
    pas_cbtype *pascb;
    s4t_pa *pa;
    vip_ll love_letter;
    s4t_instance_t *ds;
    
    pascb = INSTANCE(hwidb);
    if (!pascb) {
        return;
    }
    
    ds = INSTANCE(hwidb);
    pa = ds->pa_info;
    
    /*
     * Fill in 4T-specific info.
     */
    love_letter.data.s4t_pa_hw_version = pa->versions.hw_version;
    love_letter.data.s4t_pa_sw_version = pa->versions.sw_version;
    s4t_update_port_status(hwidb, &(love_letter.data.s4t_sig_status));
    love_letter.data.s4t_pa_is_dead_jim = 0;
    love_letter.data.s4t_applique_id = s4t_get_applique_id(hwidb);
    love_letter.ll_type = VIP_LL_4T_STATUS;
    vip_send_loveletter(pascb, (uchar *)&love_letter,
			sizeof(vip_ll)); 
    
    /* 
     *  do this until we can reliably field and clear interrupts from
     *  s4t indicating ctl line state changes.
     */
    s4t_linestate(hwidb);
    s4t_lineproto_change(hwidb, ds->s4t_lineup);
    
}

/*
 * s4t_clockrate
 * take the value handed down by the system and change it to what the
 * s4t68360 needs.
 *
 */
static void s4t_clockrate (hwidbtype *idb, ushort clockrate)
{
    uchar new_value;
    serialsb *ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);
    
    /* 
     * use ../hes/if_hes_serial.c : fciclock table 
     * for clockrate parameter.
     */
    switch(clockrate) {
    case 0: 
	new_value =  BPS_NONE;
	break;
    case 2240: 
	new_value =  BPS_1200;
	break;
    case 1120: 
	new_value =  BPS_2400;
	break;
    case 560: 
	new_value =  BPS_4800;
	break;
    case 280: 
	new_value =  BPS_9600;
	break;
    case 140: 
	new_value =  BPS_19200;
	break;
    case 70: 
	new_value =  BPS_38400;
	break;
    case 48: 
	new_value =  BPS_56K;
	break;
    case 42: 
	new_value =  BPS_64K;
	break;
    case 56: 
	new_value =  BPS_72K;
	break;
    case 65: 
	new_value =  BPS_125K;
	break;
    case 54: 
	new_value =  BPS_148K;
	break;
    case 32: 
	new_value =  BPS_250K;
	break;
    case 16: 
	new_value =  BPS_500K;
	break;
    case 10: 
	new_value =  BPS_800K;
	break;
    case 8: 
	new_value =  BPS_1000K;
	break;
    case 6: 
	new_value =  BPS_1300K;
	break;
    case 4: 
	new_value =  BPS_2M;
	break;
    case 2: 
	new_value =  BPS_4M;
	break;
    case 1: 
	new_value =  BPS_8M;
	break;
    default: 
	new_value = BPS_NONE;
	break;
    }
    ssb->serial_clockindex = new_value; 
      
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
}

/*
 * s4t_rx_sync
 *
 * Not presently used.
 *
 */
void s4t_rx_sync (hwidbtype *idb)
{
    s4t_instance_t *ds; 
    
    ds = INSTANCE(idb);
    
    if (!s4t_chg_init_param(idb, RX_SYNC_OFFSET, 
			    sizeof(ds->init_block->rx_sync_disable), 
			  ds->init_block->rx_sync_disable)) {
	buginf("\n4T(%d): s4t_rx_sync()- failed"
	       " for port %d",idb->unit, idb->subunit);
    }
    
}
/*
 * s4t_cmd_dispatch
 */ 
static void s4t_cmd_dispatch (malu_cmd_t *cmd)
{
    pas_cbtype *pascb = cmd->pascb;
    hwidbtype *idb;
    serialsb	*ssb;

    if (!pascb) {
	errmsg(&msgsym(BADPASCB, PA), pascb);
	return;
    }
    idb = pascb->idb;
    if (!idb) {
	errmsg(&msgsym(BADIDB, PA), idb);
	return;
    }

    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);
    switch (cmd->cmd) {
    case CCB_CMD_RESET:
	s4t_reset(idb);
	break;
    case CCB_CMD_STATUS:       
	switch (cmd->arg0) {
	case CCB_STATUS_IF_ENABLE:
	    vip_control_interface(idb, cmd);
	    break;
	case CCB_STATUS_IF_DISABLE:
	    vip_control_interface(idb, cmd);
	    /* update state of modem control lines */
	    s4t_send_status(idb);  
	    break;
	case CCB_STATUS_IF_LOOPLOCAL:
	    idb->loopback = TRUE;
 	    break;
	case CCB_STATUS_IF_NOLOOP:
	    idb->loopback = FALSE;
	    break;
	case CCB_STATUS_IF_QUIESCE:
	    vip_control_interface(idb, cmd);
	    break;
	case CCB_STATUS_IF_UNQUIESCE:
	    vip_control_interface(idb, cmd);
	    break;
	case CCB_STATUS_IF_THROTTLE:
	    s4t_throttle(idb);
	    break;
	case CCB_STATUS_IF_UNTHROTTLE:
            /* Enable the receiver*/
	    s4t_enable(idb);
	    break;
	}
	break;
    case CCB_CMD_APPLIQUE: 
        /* 
         * handled via loveletter
	 */
        break;
    case CCB_CMD_CARRIER: 
        /* handled in assembler.
         */
        break;
    case CCB_CMD_TXQLENGTH: 
        /* s4t doesn't do this.
         * 
         */
        break;
    case CCB_CMD_ERRORS:
        vip_send_interface_stats(idb, cmd);
        s4t_send_status(idb);
        break;
    case CCB_CMD_FSIP_NRZI:
        ssb->nrzi_enable = cmd->arg0;
	break;
    case CCB_CMD_FSIP_TX_INVERT_CLK:
        ssb->invert_txc = cmd->arg0;  
	break;
    case CCB_CMD_CLOCKRATE:
	s4t_clockrate(idb, cmd->arg0);
	break;
    case CCB_CMD_CLOCKTYPE:
        /* presently unused by 4T */
	break;
    case CCB_CMD_BUFFER_PARK:
        /* unused by 4T */
	break;
    case CCB_CMD_FSIP_SIGNAL_STS:
        /* unused by 4T, blocked on RP side */
	break;
    case CCB_CMD_SERIAL_CRC_32:
	ssb->crc_32_enabled = cmd->arg0;
	break;
    case CCB_CMD_FSIP_START_SLOT_G703:
        /* unused by 4T, get blocked on RP side */
	break;
    case CCB_CMD_FSIP_STOP_SLOT_G703:
        /* unused by 4T, get blocked on RP side */
	break;
    case CCB_CMD_FSIP_CRC4:
        /* unused by 4T, get blocked on RP side */
	break;
    case CCB_CMD_FSIP_TS16:
        /* unused by 4T, get blocked on RP side */
	break;
    case CCB_CMD_FSIP_CLOCKSOURCE:
        /* unused by 4T, get blocked on RP side */
	break;
    case CCB_CMD_RXBYTES:
	if_maxdgram_adjust(idb, cmd->arg0);
	break;
    case CCB_CMD_DELAY:
        ssb->hdx_transmit_delay = idb->xmitdelay = cmd->arg0 << 1;
	break;
    case CCB_CMD_ALT_IDLE:             /* 83 */
        if (cmd->arg0)
          idb->serial_flags |= SERIAL_FLAGS_DO_ALT_FLAG;
	else
          idb->serial_flags &= ~SERIAL_FLAGS_DO_ALT_FLAG;
	break;
    case CCB_CMD_SHORT_TXQ_LOCALBUF:   /* 85 */
	/*
	 * If queueing is used, then set the VIP polling limits
	 * so that we don't get too many packets.
	 */
	if (cmd->arg0) {
	    pascb->tx_polling_high = 2;
	    pascb->tx_polling_low = 1;
	} else {
	    pascb->tx_polling_high = S4T_TXPOLL_HIGH;
	    pascb->tx_polling_low = S4T_TXPOLL_LOW;
	}
	break;
    case CCB_CMD_FSIP_IGNORE_DCD:   /* 86 */
	ssb->ignore_dcd = cmd->arg0;
	break;
    case CCB_CMD_SERIAL_ENCAP:      /* 87 */
	idb->enctype = cmd->arg0 | (cmd->arg1 << 16);
 	break;
    default:
        vip_unsupported_cmd(cmd);
        break;
	
    }
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
}
/*
 * s4t_show_instance:
 * Platform specific printout of the instance structure
 */
void s4t_show_hw_instance (s4t_instance_t *ds)
{
    /*
     * Display VIP specific stuff
     */
    printf("\npak_to_host=%#x, vip_memd_ifcntl=%#x\n",
	   ds->pacb.pak_to_host, ds->pacb.vip_memd_ifcntl);
    printf("txring_cnt_ptr=%#x, polling high=%d, polling low=%d\n",
	   ds->pacb.txring_cnt_ptr, ds->pacb.tx_polling_high,
	   ds->pacb.tx_polling_low);
}


/*
 * s4t_snmp_link_trap:
 * SNMP trap event support. Nothing on VIP.
 */
void s4t_snmp_link_trap (int trap, hwidbtype *idb)
{
}

/*
 * s4t_lineproto_change:
 * Line protocol change notification.
 */
void s4t_lineproto_change (hwidbtype *idb, boolean state)
{

    /*
     * Call the VIP kernel, which will send indication
     * to the master wrt the line state change.
     */
    vip_lineproto_change(idb, state);

}
/*
 * s4t_init_buffer_pools:
 * Initialize buffer pools for this interface
 */
boolean s4t_init_buffer_pools (hwidbtype *idb)
{
    s4t_instance_t *ds;
    pooltype *pool;
    int particles_in_pool;
    mempool *buffer_mempool;

    /*
     * Init the particle pool
     */
    if (!idb->pool) {
	/*
	 * See which memory pool this interfaces buffers should use
	 * and create the buffer pool.
	 */
	buffer_mempool = pas_buffer_mempool(idb);

	if (idb->pool_group == POOL_GROUP_INVALID)
            idb->pool_group = pool_create_group();

	pool = particle_pool_create(idb->hw_namestring,
				    idb->pool_group,
				    S4T_PARTICLE_SIZE, 
				    POOL_SANITY,
				    32,
				    buffer_mempool);
	if (!pool) {
	    return(FALSE);
	}
	
	/*
	 * Add a fallback particle pool
	 */
	pool->fallback = pas_interface_fallback_pool(idb);
	
	/*
	 * Populate the pool and add a cache
	 */
	particles_in_pool = S4T_RX_RING_ENTRIES * 3;
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
	s4t_cleanup_pak(ds->rx_pak);
    }
    return(TRUE);
}

/*
 * s4t_set_maxdgram
 * Set max rx buffer size for interface.
 *
 */
boolean s4t_set_maxdgram (hwidbtype *idb, int buffersize, int maxdgram)
{
    /* nop used on VIP platform, maxdgram passed through CCB */
    return(TRUE);

}

/*
 * s4t68360_idb_create:
 * Create an idb for an s4t68360 serial interface. VIP platforms
 * do not support hot swap, so just create it.
 */
hwidbtype *s4t68360_idb_create (s4t_pa *pa, uint port)
{
    hwidbtype *idb;

    idb = idb_create();
    if (idb) {
	idb->oir_flags |= (IDB_FIRST_INIT | IDB_FINAL_INIT);
    }
    return(idb);
}

/*
 * s4t_periodic
 * Called every 10 seconds. Check the health of the PA.
 */
void s4t_periodic (hwidbtype *idb)
{
    if (idb->subunit == 0) {
	s4t_instance_t *ds = INSTANCE(idb);

	s4t_check_pa_health(ds->pa_info);
    }
}

/*
 * s4t_deallocate:
 * Clean up all resources for this interface. VIP does not support
 * hotswap, so there is no need to cleanup.
 */

void s4t_deallocate (hwidbtype *hwidb)
{
}

/*
 * s4t_platform_init:
 * Platform dependant initialization for MC68360 4T port adaptor interface.
 */
boolean s4t_platform_init (hwidbtype *idb)
{
    pas_cbtype *pascb = INSTANCE(idb);
    s4t_instance_t *ds = INSTANCE(idb);

    /*
     * Setup fci_type
     */
    idb->fci_type = FCI_TYPE_VIP_SERIAL4;

    /*
     * Perform common VIP interface initialization
     */
    vip_interface_init(idb);

    /*
     * VIP interfaces do not generate keepalives (the host does)
     */
    idb->nokeepalive = TRUE;

    /*
     * Install the command dispatch routine
     */
    install_cmd_dispatch(idb, s4t_cmd_dispatch);

    /*
     * Install the polling limit and counter pointer.
     */
    pascb->tx_polling_high = S4T_TXPOLL_HIGH;
    pascb->tx_polling_low = S4T_TXPOLL_LOW;
    pascb->txring_cnt_ptr = &ds->tx_count;

    /*
     * Install this interface into the pa_jumptable
     */
    pas_install_port_instance(idb, TRUE);
    return(TRUE);
}


/*
 * s4t_clockrate()
 * Set the DCE clock rate. 
 */
void s4t_clockrate_command (hwidbtype *hwidb, parseinfo *csb)
{
    /*
     * Not used on VIP
     */
}

/*
 * s4t_status_int_handle()
 * Do status interrupt handling
 */
void s4t_status_int_handle (hwidbtype *hwidb)
{
    /*
     * Do nothing on VIP
     */

}

/*
 * Set up the clockrate
 */
void s4t_init_clockrate (hwidbtype *hwidb)
{
    /*
     * Do nothing on VIP
     */
}

/*
 *  s4t_is_really_dead_jim - 4T has no responded to messages.
 *  A little drastic, but this is a real corner case.
 */
void s4t_is_really_dead_jim (s4t_pa *pa)
{
    crashpoint();
}

/*
 * Report back if the line should be forced up or not
 */
boolean s4t_line_is_up (hwidbtype *hwidb)
{
    /*
     * Line is always up for VIP
     */
    return (TRUE);
}
/* end of file */

