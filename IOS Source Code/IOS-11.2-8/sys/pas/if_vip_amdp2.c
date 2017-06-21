/* $Id: if_vip_amdp2.c,v 3.5.44.9 1996/08/28 13:06:43 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_amdp2.c,v $
 *------------------------------------------------------------------
 * if_vip_amdp2.c - 8e Presidio2 based PCI port adaptor support
 *		    (VIP specific components)
 *
 * April 1995, Dave Turner
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_amdp2.c,v $
 * Revision 3.5.44.9  1996/08/28  13:06:43  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.44.8  1996/07/22  23:16:35  getchell
 * Reduce the number of buffers allocated for receive pool
 * CSCdi62831:  missing the last inteface of a port adapter
 * Branch: California_branch
 *
 * Revision 3.5.44.7  1996/07/09  18:42:18  feisal
 * CSCdi61457:  Two VIP2 connected back to back wont when both have
 * mac-addr config
 * Branch: California_branch
 *
 * Revision 3.5.44.6  1996/05/07  01:22:39  mbeesley
 * CSCdi56710:  Correct AMDp2 fifo setting, update error messages
 * Branch: California_branch
 *
 * Revision 3.5.44.5  1996/04/23  17:13:45  gshen
 * CSCdi55375:  Need to have different address separation for IO and
 * memory based IO
 * Branch: California_branch
 *
 * Two new I/O address separation definitions:
 *
 * #define PCI_DEV_IO_MAPPED_IO_SEPARATION  0x0400 / * 1k * /
 * #define PCI_DEV_MEM_MAPPED_IO_SEPARATION 0x4000 / * 16k * /
 *
 * These changes have been code reviewed.
 *
 * Revision 3.5.44.4  1996/04/23  16:28:53  feisal
 * CSCdi52937:  loss of carrier detected on 5EF/VIP2
 * Branch: California_branch
 *
 * Revision 3.5.44.3  1996/04/05  05:32:27  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.5.44.2  1996/03/29  04:05:46  sjacobso
 * CSCdi52266:  4E/8E receive promiscuously, hurting performance
 * Branch: California_branch
 * Change write_address_filter code to examine address filter
 * entries for important status, and to store that status in the
 * software address filter.  The correct common routines for
 * adding MAC addresses are called.  Also, fix Predator logical
 * address filter code to work correctly with HSRP addresses.
 *
 * Revision 3.5.44.1  1996/03/21  23:23:47  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.5  1996/02/19  05:25:34  dcarroll
 * CSCdi49299:  Fix 4e frame miss count handling
 * Branch: ELC_branch
 * Disable miss frame counter overflow interrupt.
 *
 * Revision 3.2.2.4  1996/02/10  00:30:49  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.3  1996/01/16  01:43:25  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.2  1995/12/05  06:36:05  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.2.2.1  1995/11/30  02:06:35  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.5  1995/12/19  20:24:35  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.4  1995/12/17  18:36:00  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/12/10  04:01:24  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.2  1995/11/17  18:54:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:10  hampton
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
#include "packet.h"
#include "logger.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/ether.h"
#include "../if/network.h"

#include "../hes/ccb_cmd.h"
#include "../hes/if_fci.h"

#include "if_pas.h"
#include "if_pas_amdp2.h"
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "pas_id.h"

/*
 * amdp2_get_particle:
 * Get an rx particle for the device
 */
particletype *amdp2_get_particle (hwidbtype *idb)
{
    return(amdp2_get_particle_inline(idb, TRUE));
}

/*
 * amdp2_address_filter_write:
 * Perform the driver end of the address filter write command.
 *
 * Write a 16 bit word into the software address filter.  Once
 * a complete entry is received, record information in the software
 * address filter status word; then, update the amdp2 hardware logical
 * address filter (LADRF).
 */
static void amdp2_address_filter_write (hwidbtype *idb, uint larg)
{
    pas_cbtype *pascb = INSTANCE(idb);
    ushort *af_entry;
    vip_af_entry_t *first_ent_ptr, curr_ent_ptr;
    vip_sw_af_t *af_ptr;
 
    af_entry = pascb->vip_af;
    if (!af_entry || (pascb->vip_af_entries >= VIP_MAX_AF_ENTRIES)) {
        return;
    }
 
    /*
     * Write the address filter if the index is within bounds
     */
    af_entry[pascb->vip_af_index] = (ushort)larg;
    pascb->vip_af_index += 1;
 
    /*
     * Process newly completed software address filter entries.
     *
     * If we just received the last entry of a filter entry,
     * analyze it.  Determine the characteristics of the entry
     * and record them in the software address filter status word.
     * When appropriate, record the new entry into the hardware
     * address filter structure and load it into the dec21140 chip.
     *
     * The software address filter status word is the eighth ushort
     * of the first entry (only) in the software address filter.
     */
    if ((pascb->vip_af_index & 0x07) == 0x07) {
 
        first_ent_ptr = pascb->vip_af;
 
        af_ptr = pascb->vip_af;
        curr_ent_ptr = af_ptr->af_entry[pascb->vip_af_entries];
 
        if (larg & AF_STATION_ADDRESS) {
            /*
             * Station unicast address.  This will be placed into the hardware
             * address filter by amdp2_init().
             * Place bit in AF status word for information only.
             */
            first_ent_ptr->status_or_pad |= AF_STAT_STATION_CONFIGURED;
            ieee_copy((uchar *)curr_ent_ptr.match, idb->hardware);
 
        } else if (larg & AF_HSRP_STATION_ADDR) {
            /*
             * HSRP unicast address.  Mark for special handling in the AF
             * status word because we need to use the software address filter.
             * The amdp2 hardware address filtering logic can handle only one
             * unicast address in hardware, so the chip must be placed in
             * promiscuous mode.
             */
            first_ent_ptr->status_or_pad |= AF_STAT_SPECIAL_HANDLING
                                             | AF_STAT_HSRP_CONFIGURED_SPL;
            amdp2_add_multicast_address(idb->firstsw,
                                        (uchar *)curr_ent_ptr.match, 0);
            amdp2_init(idb, FALSE);
            if (idb->counters.resets > 0) {
                idb->counters.resets--;
            }
 
        } else if (larg & AF_MULTICAST_ADDRESS) {
            /*
             * Multicast or broadcast address.  If multicast, store it into the
             * logical address filter.  The broadcast address is implicitly
             * accepted by the hardware.  The multicast and broadcast bits in
             * the AF status word are for information only.
             *
             * The hardware logical address filter (LADRF) in the amdp2 chip
             * is an imperfect multicast address filter.  At this time, we
             * allow higher-level software to filter the leakage through the
             * LADRF if multicast ranges are not configured.
             *
             * If multicast range addresses are configured, the software
             * address filter will be configured to filter all multicasts.
             */
            if ((curr_ent_ptr.match[0] == 0xFFFF)
                 && (curr_ent_ptr.match[1] == 0xFFFF)
                 && (curr_ent_ptr.match[2] == 0xFFFF)) {
                first_ent_ptr->status_or_pad |= AF_STAT_BROADCAST_CONFIGURED;
            } else {
                first_ent_ptr->status_or_pad |= AF_STAT_MULTICAST_CONFIGURED;
 
                if ((curr_ent_ptr.mask[0] != 0x0000)
                     || (curr_ent_ptr.mask[1] != 0x0000)
                     || (curr_ent_ptr.mask[2] != 0x0000)) {
                    first_ent_ptr->status_or_pad |= AF_STAT_MULTICAST_RANGE;
                    amdp2_add_multicast_range(idb->firstsw,
                                              (uchar *)curr_ent_ptr.match,
                                              (uchar *)curr_ent_ptr.mask);
                } else {
                    amdp2_add_multicast_address(idb->firstsw,
                                                (uchar *)curr_ent_ptr.match, 0);
                }
            }
 
        } else if (larg & AF_BRIDGING_ADDRESS) {
            /*
             * Bridging.
             *
             * Set spacial handling in the AF status word, as we need to use
             * the software address filter.  Set idb->tbridge_on_hwidb so
             * the chip will be placed in promiscuous mode.
             */
            first_ent_ptr->status_or_pad |= AF_STAT_SPECIAL_HANDLING
                                             | AF_STAT_BRIDGING_CONFIGURED;
 
            idb->tbridge_on_hwidb = TRUE;
        }
 
        /*
         * Update software address filter variables.
         */
        pascb->vip_af_index += 1;               /* skip the pad */
        pascb->vip_af_entries += 1;             /* increment entry count */
    }
}

/*
 * amdp2_cmd_dispatch:
 * Handle deferred malu attention comands
 */
static void amdp2_cmd_dispatch (malu_cmd_t *cmd)
{
    pas_cbtype *pascb = cmd->pascb;
    hwidbtype *idb;
    amdp2_instance_t *ds;

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

    /*
     * Process the command
     */
    switch (cmd->cmd) {

    case CCB_CMD_RXBYTES:
	break;

    case CCB_CMD_ERRORS:
	vip_send_interface_stats(idb, cmd);
	break;

    case CCB_CMD_DELAY:
	break;

    case CCB_CMD_STATUS:
	vip_control_interface(idb, cmd);
	break;

    case CCB_CMD_AF_WRITE:
	amdp2_address_filter_write(idb, cmd->arg0);
	break;

    case CCB_CMD_FEIP_SPECIFIC:
	
	switch (cmd->arg0) {

	case FEIP_INIT_AF:
	    vip_address_filter_init(idb);
            amdp2_ladrf_clear(idb);
            idb->tbridge_on_hwidb = FALSE;
	    break;

	default:
	    vip_unsupported_cmd(cmd);
	    break;
	}
	break;

    default:
	vip_unsupported_cmd(cmd);
	break;
    }
}

/*
 * amdp2_idb_create:
 * Create an idb for an AMDP2 ethernet interface. VIP platforms
 * do not support hot swap, so just create it.
 */
hwidbtype *amdp2_idb_create (uint slot, uint port)
{
    hwidbtype *idb;

    idb = idb_create();
    if (idb) {
	idb->oir_flags |= (IDB_FIRST_INIT | IDB_FINAL_INIT);
    }
    return(idb);
}

/*
 * amdp2_deallocate:
 * Clean up all resourcse for this interface. VIP does not support
 * hotswap, so there is no need to cleanup.
 */
void amdp2_deallocate (hwidbtype *hwidb)
{
}

/*
 * amdp2_platform_init:
 * Platform dependant initialization for Presidio2 port adaptor interface.
 */
boolean amdp2_platform_init (hwidbtype *idb)
{
    pas_cbtype *pascb = INSTANCE(idb);
    amdp2_regs_t *reg;
    amdp2_instance_t *ds;

    /*
     * Setup fci_type
     */
    idb->fci_type = FCI_TYPE_VIP_ETHER8;

    /*
     * Perform common VIP interface initialization
     */
    vip_interface_init(idb);

    /*
     * VIP interfaces do not generate keepalives (the host does)
     */
    idb->nokeepalive = TRUE;

    /*
     * Allocate an address filter
     */
    if (!vip_address_filter_allocate(idb)) {
	return(FALSE);
    }

    /*
     * Init the address filter
     */
    vip_address_filter_init(idb);

    /*
     * Install the command dispatch routine
     */
    install_cmd_dispatch(idb, amdp2_cmd_dispatch);

    /*
     * Configure BCRs. All these settings are one shots, and do
     * not get effected by resets.
     */
    ds = INSTANCE(idb);
    reg = ds->amdp2_regaddr;

    reg->rap = AMDP2_CSR0;
    reg->rdp = AMDP2_CSR0_STOP;
    DELAY(5);
    
    reg->rap = AMDP2_BCR18;
    reg->bdp = AMDP2_BCR18_BREADE | AMDP2_BCR18_BWRITE;
    reg->rap = AMDP2_BCR20;
    reg->bdp = AMDP2_BCR20_SSIZE32 | AMDP2_BCR20_SW_STYLE3;
    
    /* 
     * Link Status Enable & Pulse Stretcher on by default
     */
    reg->rap = AMDP2_BCR4;
    reg->bdp = AMDP2_BCR4_LNKSTE | AMDP2_BCR4_PSE;   

    /*
     * Disable pin outputs for LED1, LED2, LED3.
     * Add specific pattern to low-order nibble to avoid logic error
     * in silicon of rev B0 chip.  Otherwise, the error can cause
     * the chip to enter STOP state or fail to init.
     */
    reg->rap = AMDP2_BCR5;
    reg->bdp = AMDP2_BCR5_LEDDIS | 0x7;
    reg->rap = AMDP2_BCR6;
    if (get_pa_type(ds->pacb.pa_bay) == PA_HARDWARE_5E) 
	reg->bdp = AMDP2_BCR6_LEDPOL | AMDP2_BCR6_FDLSE;
    else
	reg->bdp = AMDP2_BCR6_LEDDIS;
    reg->rap = AMDP2_BCR7;
    reg->bdp = AMDP2_BCR7_LEDDIS;

    /*
     * Install the polling limit and counter pointer.
     */
    pascb->tx_polling_high = AMDP2_POLL_HIGH;
    pascb->tx_polling_low = AMDP2_POLL_LOW;
    pascb->txring_cnt_ptr = &ds->tx_count;

    /*
     * Install the status register for use by the posted read
     * hardware.
     */
    pas_install_status_reg(idb, (void *)&reg->rdp, TRUE);

    /*
     * Install this interface into the pa_jumptable
     */
    pas_install_port_instance(idb, TRUE);
    return(TRUE);
}

/*
 * amdp2_show_instance:
 * Platform specific printout of the instance structure
 */
void amdp2_show_instance (amdp2_instance_t *ds)
{
    /*
     * Display the address filter
     */
    vip_address_filter_print(&ds->pacb);

    /*
     * Display VIP specific stuff
     */
    printf(" pak_to_host=%#x, vip_memd_ifcntl=%#x\n",
	   ds->pacb.pak_to_host, ds->pacb.vip_memd_ifcntl);
    printf(" spurious_idon=%d, filtered_pak=%d, throttled=%d, enabled=%d, "
	   "disabled=%d\n",
	   ds->spurious_idon, ds->filtered_pak, ds->throttled,
	   ds->enabled, ds->disabled);
    printf(" rx_framing_err=%d, rx_overflow_err=%d, rx_buffer_err=%d, "
	   "rx_bpe_err=%d\n",
	   ds->rx_framing_err, ds->rx_overflow_err, ds->rx_buffer_err,
	   ds->rx_bpe_err);
    printf(" rx_soft_overflow_err=%d, rx_no_enp=%d, rx_discard=%d, "
	   "rx_miss_count=%d\n",
	   ds->rx_soft_overflow_err, ds->rx_no_enp, ds->rx_discard,
	   ds->rx_miss_count);
    printf(" tx_one_col_err=%d, tx_more_col_err=%d, tx_no_enp=%d, "
	   "tx_deferred_err=%d\n",
	   ds->tx_one_col_err, ds->tx_more_col_err, ds->tx_no_enp,
	   ds->tx_deferred_err);
    printf(" tx_underrun_err=%d, tx_late_collision_err=%d, "
	   "tx_loss_carrier_err=%d\n",
	   ds->tx_underrun_err, ds->tx_late_collision_err, 
	   ds->tx_loss_carrier_err);
    printf(" tx_exc_collision_err=%d, tx_buff_err=%d, fatal_tx_err=%d\n",
	   ds->tx_exc_collision_err, ds->tx_buff_err, ds->fatal_tx_err);
    printf(" hsrp_conf=%d\n", ds->hsrp_configured);
}

/*
 * amdp2_snmp_link_trap:
 * SNMP trap event support. Nothing on VIP.
 */
void amdp2_snmp_link_trap (int trap, hwidbtype *idb)
{
}

/*
 * amdp2_lineproto_change:
 * Line protocol change notification.
 */
void amdp2_lineproto_change (hwidbtype *idb, boolean state)
{
    /*
     * Call the VIP kernel, which will send indication
     * to the master wrt to the line state change.
     */
    vip_lineproto_change(idb, state);
}

/*
 * amdp2_get_default_hw_address:
 * Get a default mac address for this interface
 */
void amdp2_get_default_hw_address (hwidbtype *idb, uchar addr[IEEEBYTES])
{
    pas_cbtype *pascb = INSTANCE(idb);

    /*
     * If the address filter has been written, then our station
     * address is the first entry.
     */
    if (pascb->vip_af && pascb->vip_af_entries) {
	ieee_copy((uchar *)pascb->vip_af, addr);
    }
}

/*
 * amdp2_init_buffer_pools:
 * Initialize buffer pools for this interface
 */
boolean amdp2_init_buffer_pools (hwidbtype *idb)
{
    amdp2_instance_t *ds;
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
				    AMDP2_PARTICLE_SIZE, 
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
	particles_in_pool = AMDP2_RX_RING_ENTRIES * 2;
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
	amdp2_cleanup_pak(ds->rx_pak);
    }
    return(TRUE);
}

