/* $Id: if_vip_dec21140.c,v 3.6.44.9 1996/08/28 13:06:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_dec21140.c,v $
 *------------------------------------------------------------------
 * if_vip_dec21140.c - DEC21140 100 Mbit Ethernet PCI PA
 *		       (VIP dependent parts)
 *
 * February 1995 - Steve Jacobson
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_dec21140.c,v $
 * Revision 3.6.44.9  1996/08/28  13:06:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.44.8  1996/07/26  01:10:11  feisal
 * CSCdi61016:  Need to add CardType MIB definition for different flavors
 * of 1FE PA
 * Branch: California_branch
 *
 * Revision 3.6.44.7  1996/07/09  18:42:21  feisal
 * CSCdi61457:  Two VIP2 connected back to back wont when both have
 * mac-addr config
 * Branch: California_branch
 *
 * Revision 3.6.44.6  1996/05/31  22:33:44  feisal
 * CSCdi56695:  ISL Broken on certain card
 * Branch: California_branch
 *
 * Revision 3.6.44.5  1996/04/05  05:32:30  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.6.44.4  1996/04/05  04:17:39  sjacobso
 * CSCdi52859:  New AF and new ISL code not fully integrated
 * Branch: California_branch
 * Cause dec21140 address filter write code to set the correct
 * ISL enable flags on receipt of an ISL entry.
 *
 * Revision 3.6.44.3  1996/03/29  00:45:26  sjacobso
 * CSCdi45152:  PERFORMANCE: 1FE runs in promiscuous mode instead of
 * filtering
 * Branch: California_branch
 * Add code to allow dec21140 chip to run in perfect
 * address filtering mode whenever possible.  Modify
 * address filter write code to record information
 * about address filter entries that may affect perfect
 * filter operation.  Interface VIP-specific software
 * address filter to RVIP/SVIP/Predator common routines.
 *
 * Revision 3.6.44.2  1996/03/21  23:23:51  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.6.44.1  1996/03/18  21:34:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.4  1996/02/10  00:30:53  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3.2.3  1996/01/29  19:04:37  wfried
 * Branch: ELC_branch
 * Use HW address filtering, disable rx side for better input throttling,
 * add loopback code, correct ignored frames counting, allow only MII media
 * on the Predator I/O card FE interface.
 *
 * Revision 3.3.2.2  1996/01/16  01:43:33  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/05  06:36:09  mbeesley
 * Branch: ELC_branch
 * Safeguard static paktype headers from being freed.
 *
 * Revision 3.6  1995/12/19  20:24:36  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.5  1995/12/17  18:36:01  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/10  04:01:26  smackie
 * Add support for public particle pools. Make particle chunk growth less
 * aggressive for dynamic pools. (CSCdi45336)
 *
 * Revision 3.3  1995/11/17  18:54:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:58:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:35  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:12  hampton
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
#include "../if/ether_private.h"
#include "../if/network.h"

#include "../hes/ccb_cmd.h"
#include "../hes/if_fci.h"

#include "../src-vip/vip_isl.h"

#include "if_pas.h"
#include "if_pas_dec21140.h"
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "pas_id.h"

/*
 * dec21140_get_particle:
 * Get an rx particle for the device
 */
particletype *dec21140_get_particle (hwidbtype *idb)
{
    return(dec21140_get_particle_inline(idb, TRUE));
}

/*
 * dec21140_address_filter_write:
 * Perform the driver end of the address filter write command.
 *
 * Write a 16 bit word into the software address filter.  Once
 * a complete entry is received, record information in the software
 * address filter status word; then, update the dec21140 hardware
 * address filter. 
 */
static void dec21140_address_filter_write (hwidbtype *idb, uint larg)
{
    pas_cbtype *pascb = INSTANCE(idb);
    dec21140_instance_t *ds = INSTANCE(idb);
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
     * address filter structure for the dec21140 chip.
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
             * address filter by dec21140_init().
             * Place bit in AF status word for information only.
             */
            first_ent_ptr->status_or_pad |= AF_STAT_STATION_CONFIGURED;
            ieee_copy((uchar *)curr_ent_ptr.match, idb->hardware);
            dec21140_set_hw_address(idb, (uchar *)curr_ent_ptr.match);
 
        } else if (larg & AF_HSRP_STATION_ADDR) {
            /*
             * HSRP unicast address.  Store it into the hardware address filter.
             * Bit in AF status word is for information only.
             */
            first_ent_ptr->status_or_pad |= AF_STAT_HSRP_CONFIGURED;
            dec21140_add_hw_address(idb->firstsw,
                                           (uchar *)curr_ent_ptr.match, 0);
 
        } else if (larg & AF_MULTICAST_ADDRESS) {
            /*
             * Multicast or broadcast address.  If multicast, store it into the
             * hardware address filter.  The broadcast address is already there
             * by default.  The multicast and broadcast bits in the AF status
             * word are for information only.
             *
             * Multicast range addresses and multicast address filter overflow
             * cause the AF status word to be marked for multicast range.  The
             * dec chip will be configured to accept all multicast addresses
             * from the wire in that case.  At this time, multicast range
             * causes the filter code to put all multicasts through the
             * software address filter.
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
                    dec21140_add_multicast_range(idb->firstsw,
                                                  (uchar *)curr_ent_ptr.match,
                                                  (uchar *)curr_ent_ptr.mask);
                } else {
                    dec21140_add_hw_address(idb->firstsw,
                                                   (uchar *)curr_ent_ptr.match,
                                                   0);
                }
                /*
                 * If one of the add multicast routines caused the HW
                 * address filter table to overflow, set multicast range
                 * so that multicasts will pass through the SW address filter.
                 */
                if (ds->addr_table_overflow) {
                    first_ent_ptr->status_or_pad |= AF_STAT_MULTICAST_RANGE;
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

        } else if (larg & AF_ISL) {
            /*
             * ISL is being configured.
             *
             * If the PA hardware supports ISL, set special handling
             * so the hardware address filter will be used.  Set
             * various software ISL flags.
             */
 
            if (pascb->isl_supported) {
                first_ent_ptr->status_or_pad |= AF_STAT_SPECIAL_HANDLING
                                                 | AF_STAT_ISL_CONFIGURED;
                dec21140_add_hw_address(idb->firstsw,
                                               (uchar *)curr_ent_ptr.match,
                                               0);
                pascb->isl_enabled = TRUE;
                idb->isl_vlan_on_hwidb = TRUE;
            }
        }
 
        /*
         * Update software address filter variables.
         */
        pascb->vip_af_index += 1;               /* skip the pad */
        pascb->vip_af_entries += 1;             /* increment entry count */
    }
}

/*
 * dec21140_cmd_dispatch:
 * Handle deferred malu attention comands
 */
static void dec21140_cmd_dispatch (malu_cmd_t *cmd)
{
    pas_cbtype	*pascb = cmd->pascb;
    hwidbtype	*idb;
    ethersb	*esb;
    dec21140_instance_t *ds;


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
	dec21140_address_filter_write(idb, cmd->arg0);
	break;

    case CCB_CMD_FEIP_SPECIFIC:
	esb = idb_use_hwsb_inline(idb, HWIDB_SB_ETHER);
	switch (cmd->arg0) {

	case FEIP_LOOP_NONE:
	case FEIP_LOOP_MOTOROLA:
	case FEIP_LOOP_DEC:
	case FEIP_LOOP_MICRO_LINEAR:
	    idb->ext_loop_type = cmd->arg0;
	    break;

	case FEIP_RJ45:
	    esb->media_type = ETHER_MEDIA_TYPE_FEIP_RJ45;
	    break;

	case FEIP_MII:
	    esb->media_type = ETHER_MEDIA_TYPE_FEIP_MII;
	    break;

	case FEIP_FULL_DUPLEX:
	    esb->feip_fullduplex = TRUE;
	    break;

	case FEIP_HALF_DUPLEX:
	    esb->feip_fullduplex = FALSE;
	    break;

	case FEIP_INIT_AF:
	    vip_address_filter_init(idb);
            dec21140_hwaf_clear(idb);
            idb->tbridge_on_hwidb = FALSE;
	    break;

	default:
	    vip_unsupported_cmd(cmd);
	    break;
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_ETHER);
	break;

    default:
	vip_unsupported_cmd(cmd);
	break;
    }
}

/*
 * dec21140_idb_create:
 * Create an idb for an DEC21140 ethernet interface. VIP platforms
 * do not support hot swap, so just create it.
 */
hwidbtype *dec21140_idb_create (uint slot, uint port)
{
    hwidbtype *idb;

    idb = idb_create();
    if (idb) {
	idb->oir_flags |= (IDB_FIRST_INIT | IDB_FINAL_INIT);
    }
    return(idb);
}

/*
 * dec21140_deallocate:
 * Clean up all resourcse for this interface. VIP does not support
 * hotswap, so there is no need to cleanup.
 */
void dec21140_deallocate (hwidbtype *hwidb)
{
}

/*
 * dec21140_platform_init:
 * Platform dependant initialization for dec21140 port adaptor interface.
 */
boolean dec21140_platform_init (hwidbtype *idb)
{
    dec21140_instance_t *ds = INSTANCE(idb);
    pas_cbtype *pascb = INSTANCE(idb);
    dec21140_regs_t *reg;

    /*
     * Setup fci_type
     */
    switch (ds->pa_type) {
      case PA_HARDWARE_1FE_TX_NISL:
          idb->fci_type = FCI_TYPE_VIP_FETHER_TX_NISL;
          break;
      case PA_HARDWARE_1FE_TX_ISL:
          idb->fci_type = FCI_TYPE_VIP_FETHER_TX_ISL;
          break;
      case PA_HARDWARE_1FE_FX_ISL:
          idb->fci_type = FCI_TYPE_VIP_FETHER_FX_ISL;
          break;
      case PA_HARDWARE_1FE:
      default:
          idb->fci_type = FCI_TYPE_VIP_FETHER;
          break;
    }


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
     * Init the software address filter.
     */
    vip_address_filter_init(idb);

    /*
     * Install the command dispatch routine
     */
    install_cmd_dispatch(idb, dec21140_cmd_dispatch);

    /*
     * Configure the control registers
     */
    reg = ds->dec21140_regaddr;
    if (DEC21140_DMA_BIGENDIAN) {
	/*
	 * Setup global bus parameters with the DMA engine in big-endian
	 */
	reg->csr0  = (DEC21140_CSR0_TX_AUTO_POLL_200_US
		      | DEC21140_CSR0_CACHE_ALIGN
		      | DEC21140_CSR0_PROG_BURST_LEN
		      | DEC21140_CSR0_BIG_ENDIAN
		      | DEC21140_CSR0_DESCRPT_SKIP_LENGTH);
    } else {
	/*
	 * Setup global bus parameters with the DMA engine in little-endian
	 */
	reg->csr0  = (DEC21140_CSR0_TX_AUTO_POLL_200_US
		      | DEC21140_CSR0_CACHE_ALIGN
		      | DEC21140_CSR0_PROG_BURST_LEN
		      | DEC21140_CSR0_DESCRPT_SKIP_LENGTH);
    }

    /* 
     * Disable all interrupts and stop rx, tx processes
     */
    reg->csr7  = DEC21140_CSR7_DISABLE_ALL_INTERRUPTS;
    reg->csr6  = (DEC21140_CSR6_INITIAL_VALUE
		  | DEC21140_CSR6_RECEIVE_STOP
		  | DEC21140_CSR6_TRANSMIT_STOP);

    /* 
     * Configure these pins on the general purpose port as outputs
     */
    reg->csr12 = (DEC21140_CSR12_GPC
		  | DEC21140_CSR12_OUTPUT_MASK);
    reg->csr12 = (DEC21140_CSR12_INITIAL_VALUE
		  & ~DEC21140_CSR12_MICRO_TX_ENABLE);

    /* 
     * Setup the tx poll limit and a pointer to the drivers
     * tx ring counter.
     */
    pascb->tx_polling_high = DEC21140_TXPOLL_HIGH;
    pascb->tx_polling_low = DEC21140_TXPOLL_LOW;
    pascb->txring_cnt_ptr = &ds->tx_count;

    /*
     * Install the status register for use by the posted read hw.
     */
    pas_install_status_reg(idb, (void *)&reg->csr5, DEC21140_MEMMAPPED_IO);

    /*
     * Install this interface into the pa_jumptable.
     */
    pas_install_port_instance(idb, TRUE);
    return(TRUE);
}

/*
 * dec21140_show_instance:
 * Platform specific printout of the instance structure
 */
void dec21140_show_instance (dec21140_instance_t *ds)
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
    printf(" filtered_pak=%d, throttled=%d, enabled=%d, disabled=%d\n",
	   ds->filtered_pak, ds->throttled, ds->enabled, ds->disabled);
    printf(" rx_fifo_overflow=%d, rx_no_enp=%d, rx_discard=%d\n",
	   ds->rx_fifo_overflow, ds->rx_no_enp, ds->rx_discard);
    printf(" tx_underrun_err=%d, tx_jabber_timeout=%d, tx_carrier_loss=%d\n",
	   ds->tx_underrun_err, ds->tx_jabber_timeout, ds->tx_carrier_loss);
    printf(" tx_no_carrier=%d, tx_late_collision=%d, tx_excess_coll=%d\n",
	   ds->tx_no_carrier, ds->tx_late_collision, ds->tx_excess_coll);
    printf(" tx_collision_cnt=%d, tx_deferred=%d, fatal_tx_err=%d,"
           " tbl_overflow=%d\n",
	   ds->tx_collision_count, ds->tx_deferred, ds->fatal_tx_err,
           ds->addr_table_overflow);
}

/*
 * dec21140_snmp_link_trap:
 * SNMP trap event support. Nothing on VIP.
 */
void dec21140_snmp_link_trap (int trap, hwidbtype *idb)
{
}

/*
 * dec21140_lineproto_change:
 * Line protocol change notification.
 */
void dec21140_lineproto_change (hwidbtype *idb, boolean state)
{
    /*
     * Call the VIP kernel, which will send indication
     * to the master wrt to the line state change.
     */
    vip_lineproto_change(idb, state);
}

/*
 * dec21140_get_default_hw_address:
 * Get a default mac address for this interface
 */
void dec21140_get_default_hw_address (hwidbtype *idb, uchar addr[IEEEBYTES])
{
    pas_cbtype *pascb = INSTANCE(idb);

    /*
     * If the SW address filter has been written, then our station
     * address is the first entry.
     */
    if (pascb->vip_af && pascb->vip_af_entries) {
	ieee_copy((uchar *)pascb->vip_af, addr);
    }
}

/*
 * dec21140_init_buffer_pools:
 * Initialize buffer pools for this interface
 */
boolean dec21140_init_buffer_pools (hwidbtype *idb)
{
    dec21140_instance_t *ds;
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
				    DEC21140_PARTICLE_SIZE, 
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
	particles_in_pool = DEC21140_RX_RING_ENTRIES * 3;
	pool_adjust(pool, 0, particles_in_pool, particles_in_pool, TRUE);
	particle_pool_create_cache(pool, particles_in_pool);
	pool_adjust_cache(pool, particles_in_pool);
	pool->hwidb = idb;
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
	dec21140_cleanup_pak(ds->rx_pak);
    }
    return(TRUE);
}

/*
 * dec21140_default_media:
 * Return the default media type, MII or RJ45. On VIP, this is always RJ45.
 */
int dec21140_default_media (hwidbtype *idb)
{
    return(ETHER_MEDIA_TYPE_FEIP_RJ45);
}

/*
 * dec21140_default_duplex:
 * Return the default half/full duplex setting. On VIP, always half duplex.
 */
boolean dec21140_default_duplex (hwidbtype *idb)
{
    return(FALSE);
}

/* end of file */
