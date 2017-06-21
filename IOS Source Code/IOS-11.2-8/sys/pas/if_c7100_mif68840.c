/* $Id: if_c7100_mif68840.c,v 3.1.36.18 1996/08/28 13:05:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_mif68840.c,v $
 *------------------------------------------------------------------
 * if_c7100_mif68840.c - 1F MIF68840 based PCI port adapter support 
 *                   (Predator specific components)
 *
 * Jan 1996, Farad Sunavala
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_mif68840.c,v $
 * Revision 3.1.36.18  1996/08/28  13:05:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.36.17  1996/08/07  23:35:28  sthormod
 * CSCdi63522:  C7200: RSRB gets process switched on ether, serial, fddi
 * Branch: California_branch
 *
 * Revision 3.1.36.16  1996/07/23  18:47:58  ppearce
 * CSCdi63747:  Move srb_multiring out of SRB subblock
 * Branch: California_branch
 *   (1) srb_multiring doesn't really belong exclusively to SRB
 *         move srb_multiring out of SRB subblock and into swidb
 *         provide srb_multiring accessor routines
 *   (2) determine_rif() should take swidb as argument rather than hwidb
 *
 * Revision 3.1.36.15  1996/07/11  14:20:57  fbordona
 * CSCdi62521:  DLSw circuit does not connect using FST over ATM peer
 *              Remove DLSw FST hooks from platform/media specific drivers.
 * Branch: California_branch
 *
 * Revision 3.1.36.14  1996/07/09  06:03:58  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.1.36.13  1996/06/10  22:21:38  mbeesley
 * CSCdi59552:  software forced reload due to memory corruption in
 * 2FPA-2FPA test. Use 32 byte burst, Memory Read Line on Predator. Also
 * remove debugging buginfs.
 * Branch: California_branch
 *
 * Revision 3.1.36.12  1996/06/07  06:59:07  fsunaval
 * CSCdi54755:  Fixes for SMT conformance tests.
 * Branch: California_branch
 * 1.Fix trace test and other minor MIB values.
 *
 * Revision 3.1.36.11  1996/05/31  01:26:15  fsunaval
 * CSCdi57339:  Implement throttling on the 1F.
 * Branch: California_branch
 * 1.Throttle the interface when no more particles available.  The
 *   interface is unthrottled when the cache has a certain threshold of
 *   particles in it.
 *
 * Revision 3.1.36.10  1996/05/21  09:59:58  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.36.9  1996/05/18  08:34:13  fsunaval
 * CSCdi57554:  FDDI PA needs OIR support
 * Branch: California_branch
 * 1.1F OIR support and patch for CSCdi57554.
 *
 * Revision 3.1.36.8  1996/05/17  11:40:01  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.22.3  1996/05/05  23:42:24  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.1.22.2  1996/04/26  15:22:37  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.22.1  1996/04/03  21:13:49  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1.36.7  1996/05/10  22:11:40  fsunaval
 * CSCdi56239:  Fddi PA fails to come up for netbooting
 * Branch: California_branch
 *
 * Revision 3.1.36.6  1996/04/30  07:05:19  fsunaval
 * CSCdi54755:  Fixes for SMT conformance tests.
 * Branch: California_branch
 *
 * Revision 3.1.36.5  1996/04/24  02:13:12  fsunaval
 * CSCdi55257:  Need to speed up fddi process level
 * Branch: California_branch
 * 1.Use the DMA coalescer.
 *
 * Revision 3.1.36.4  1996/04/11  04:42:46  fsunaval
 * CSCdi53237:  alignment error 32 , Memory allocation failed from pool PCI
 * Branch: California_branch
 *
 * Revision 3.1.36.3  1996/04/11  02:03:07  izhu
 * CSCdi53807:  Parser support for new port adapter on predator
 * Also includes optical bypass change
 * Branch: California_branch
 *
 * Revision 3.1.36.2  1996/04/08  19:00:44  dcarroll
 * CSCdi53796:  Remove extra idb type assignments
 * Branch: California_branch
 * Remove the unneeded early setting of idb type.
 *
 * Revision 3.1.36.1  1996/03/21  23:22:51  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.6  1996/03/18  23:28:34  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.5  1996/02/17  05:54:27  fsunaval
 * CSCdi49008:  Fddi output counters are not reported and other fixes.
 * Branch: ELC_branch
 * 1.Output counts not increasing.
 * 2.Make FDDI on VIP memory-mapped.
 * 3.Distributed switching support.
 * 4.Reduce burst-size from 64 to 32.
 *
 * Revision 3.1.2.4  1996/02/13  01:43:04  mbeesley
 * CSCdi48778:  IO DRAM gets exhausted
 *         o Reduce AMDp2 and FDDI rx particle pool
 *         o Allocate more DRAM for IO on small images
 *         o Add cached view of packet DRAM
 *         o Use packet sram as a fast heap on rev 1 boxes
 * Branch: ELC_branch
 *
 * Revision 3.1.2.3  1996/02/08  08:47:41  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1.2.2  1996/02/05  07:00:07  fsunaval
 * Branch: ELC_branch
 * 1.Particle based fast-switching Fddi support and code cleanup.
 *
 * Revision 3.1.2.1  1996/01/25  12:10:02  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.1  1996/01/25  07:37:15  fsunaval
 * 1.Placeholder files for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "logger.h"
#include "msg_mif68840.c"		/* Not a typo, see logger.h */
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "../les/msg_fddi.c"		/* Not a typo, see logger.h */
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"
#include "subsys.h"
#include "sched.h"
#include "parser.h"
#include "mgd_timers.h"
#include "stacks.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"  

#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "../if/pci.h"
#include "../if/network.h"
#include "../if/parser_defs_fddi.h"

#include "../ip/ip.h"
     
#include "../snmp/snmp_api.h"
#include "../snmp/ifmib_registry.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
 
#include "../hes/if_fci.h"

#include "../xdi/xdihdrsif.h"
#include "../xdi/fddihdr.h"
#include "../xdi/smtmsg.h"
#include "../xdi/csphdr.h"
#include "../xdi/cspglbl.h"
#include "../xdi/cspproto.h"
#include "../xdi/hw_csp.h"
#include "../xdi/hw_phy.h"
#include "../xdi/mibdefs.h"
#include "../xdi/mibglbl.h"
#include "../xdi/mibproto.h"
#include "../xdi/mibproto.h"
#include "../xdi/fbmframe.h"
#include "../xdi/fbmhdr.h"
#include "../xdi/fbmglbl.h"
#include "../xdi/fbmproto.h"

#include "../if/if_fddi.h"
#include "../if/if_fddi_public.h"
#include "../if/if_xdi_public.h"

#include "../srt/srb_multiring.h"

#include "if_pas.h"
#include "if_pas_mif68840.h"
#include "pas_id.h"

#include "../hes/parser_defs_ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/ucode.h"
#include "../hes/slot.h"

#include "../src-4k-c7100/c7100_macaddr.h"

/* 
 * mif68840_if_final_init
 *
 * Called by if_final_init() - starts FDDI up if required
 * FIXME
 */
void mif68840_if_final_init (void)
{
}  

/*
 * mif68840_idb_create:
 * Create an idb for a MIF68840 FDDI interface. Try to get
 * it from the list of previously removed interfaces first.
 */
hwidbtype *mif68840_idb_create (uint slot, uint port)
{
    hwidbtype *idb;
        
    idb = c7100_get_matching_idb(slot, port, slots[slot].pa_type,
            IDBTYPE_FDDIT);
 
    if (idb) {
        slots[slot].interface_list[port] = idb;
        slots[slot].interface_count++;
    }
    return(idb);
}

/*   
 * mif68840_deallocate: 
 * Clean up all resourcse for this interface when the port adapter is 
 * removed.
 */
void mif68840_deallocate (hwidbtype *hwidb)
{
    mif68840_instance *ds;
    pooltype* pool;
    int ix, i;
    FBMMACInfoType  *macPtr;
 
    ds = INSTANCE(hwidb);
    pool = hwidb->pool;

    nfddinets -= 1;

    /* Clear the following value to mark the station removed */
    CSPDATA(hwidb->fddi_stn_id, stationData.CSPInitialized) = CLEAR;
    FBMDATA(hwidb->fddi_stn_id, fbmStationData.FBMInitialized) = CLEAR;
    STADATA (hwidb->fddi_stn_id, smtUp) = FALSE;
    STADATA (hwidb->fddi_stn_id, hwPresent) = FALSE;

    for (i=0, macPtr = &(FBMDATA (hwidb->fddi_stn_id, fbmMACData[0]));
	 i < MAX_MAC_COUNT; i++, macPtr++)
	macPtr->SM_MA_Avail = FALSE;

    /*
     * Determine if an instance exists. If so, go through
     * and clean up.
     */
    if (ds) {
	/*
	 * Free rx particles.
	 */
	if (ds->rxr_llc_shadow) {
	    for (ix = 0; ix < MIF68840_RX_LLC_RING_ENTRIES; ix++) {
		particletype *mp;
		mp = ds->rxr_llc_shadow[ix];
		if (mp) {
		    retparticle(mp);
		}
	    }
	}
	if (ds->rxr_smt_shadow) {
	    for (ix = 0; ix < MIF68840_RX_SMT_RING_ENTRIES; ix++) {
		particletype *mp;
		mp = ds->rxr_smt_shadow[ix];
		if (mp) {
		    retparticle(mp);
		}
	    }
	}
	/*
	 *  Free tx particles or paks.
	 */
	if (ds->txr_shadow) {
	    for (ix = 0; ix < MIF68840_TX_RING_ENTRIES; ix++) {
		particletype *mp;
		paktype *pak;
		mp = ds->txr_shadow[ix].mp;
		pak = ds->txr_shadow[ix].pak;
		if (mp) {
		    retparticle(mp);
		}
		if (pak && (pak != MIF68840_MAGIC_PAK_PTR)) {
		    datagram_done(pak);
		}
	    }
	}
	/*
	 * Free the blocks hanging off the instance.
	 */
	if (ds->rxr_llc_malloc)
	    free(ds->rxr_llc_malloc);
	if (ds->rxr_smt_malloc)
	    free(ds->rxr_smt_malloc);
	if (ds->txr_malloc)
	    free(ds->txr_malloc);
	if (ds->rxr_llc_shadow)
	    free(ds->rxr_llc_shadow);
	if (ds->rxr_smt_shadow)
	    free(ds->rxr_smt_shadow);
	if (ds->txr_shadow)
	    free(ds->txr_shadow);

	if (ds->bcn_malloc)
	    free(ds->bcn_malloc);

	if (ds->rx_pak_llc)
	    free(ds->rx_pak_llc);
	if (ds->rx_pak_smt)
	    free(ds->rx_pak_smt);

	if (ds->bcn_ptr) {
	    free(ds->bcn_ptr);
	}
	

	/*
	 * Now free the instance.
	 */
	free(ds);
	INSTANCE(hwidb) = NULL;
    }

    /*
     * Now destroy the pool.
     */
    if (pool) {
	pool_destroy(pool);
    }
    hwidb->pool = NULL;
    hwidb->pool_group = POOL_GROUP_INVALID;

    if(process_exists(smt_process_id) && (nfddinets == 0)) {
	process_kill(smt_process_id);
	smt_process_id = 0;
    }

    /* return buffers to smtbufQ */
    adjust_smt_bufs();

    /* Stop the FBM leaf timer.
     * Kill the FBM process and stop the parent timer
     * if this is the only station left.
     */
    mgd_timer_stop(&(STADATA(hwidb->fddi_stn_id, fbmTimer)));

    if(process_exists(ServiceFBMTimer_process_id) && (nfddinets == 0)) {
	process_kill(ServiceFBMTimer_process_id);
	ServiceFBMTimer_process_id = 0;
	mgd_timer_stop(&fbmTimer_parent);
    }

    /* free the memory allocated for this station's mib data */
    if(MIBDATA(hwidb->fddi_stn_id, mib)) 
	FREEMEMORY(MIBDATA(hwidb->fddi_stn_id, mib));
}

/*   
 * mif68840_platform_init:
 * Platform dependant initialization for MIF68840 port adaptor interface.
 */
boolean mif68840_platform_init (hwidbtype *idb)
{
    /*
     * No keepalives on FDDI
     */
    idb->nokeepalive = TRUE;
 
    /*
     * Install this interface into the pa_jumptable
     */
    pas_install_port_instance(idb, TRUE);

    /* Initialize the station's prevCFState for eoir cases */
    STADATA(idb->fddi_stn_id, prevCFState) =  CF_isolated;

    /* Initialize XDI SMT interface code */
    if (MIBMainMulti(idb->fddi_stn_id)) {
        errmsg(&msgsym(NOMEMORY, FDDI), idb->fddi_stn_id, "MIBMainMulti");
        return(FALSE);
    }

    if (FBMMainMulti(idb->fddi_stn_id)) {
        errmsg(&msgsym(NOMEMORY, FDDI), idb->fddi_stn_id, "FBMMainMulti");
        STADATA (idb->fddi_stn_id, smtUp) = FALSE;
        return(FALSE);
    }

    /*
     * If we haven't fired up the SMT input process yet, then
     * do so.
     */
    if (smt_process_id == 0) {
    smt_process_id = cfork(fddi_smt_input, 0, STACK_SIZE_DEFAULT,
                   "SMT input", 0);
    }

    /* Indicate that SMT was initialized successfuly */
    STADATA (idb->fddi_stn_id, smtUp) = TRUE;

    return(TRUE);
}

/*
 * mif68840_get_default_hw_address:
 * Get a default mac address for this interface
 */
void mif68840_get_default_hw_address (hwidbtype *idb, uchar addr[IEEEBYTES])
{
    c7100_assign_mac_addr(idb, addr);  
}

/*
 * mif68840_init_buffer_pools:
 * Initialize buffer pools for this interface
 */   
boolean mif68840_init_buffer_pools (hwidbtype *idb)
{

    mif68840_instance *ds;
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
                    MIF68840_PARTICLE_SIZE,
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
        particles_in_pool = MIF68840_RX_LLC_RING_ENTRIES * 3;
        pool_adjust(pool, 0, particles_in_pool, particles_in_pool, TRUE);
        particle_pool_create_cache(pool, particles_in_pool);
        pool_adjust_cache(pool, particles_in_pool);
        idb->pool = pool;
        pool->hwidb = idb;
        pool_set_cache_threshold(pool,
                                    MIF68840_BUFFER_THRESHOLD,
                                    mif68840_cache_threshold);
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

    if (!ds->rx_pak_llc) {
        ds->rx_pak_llc = pas_allocate_fspak();
        if (!ds->rx_pak_llc) {
            return(FALSE);
        }
        mif68840_cleanup_pak(ds->rx_pak_llc);
    }

    if (!ds->rx_pak_smt) {
        ds->rx_pak_smt = pas_allocate_fspak();
        if (!ds->rx_pak_smt) {
            return(FALSE);
        }
        mif68840_cleanup_pak(ds->rx_pak_smt);
    }
    
    return(TRUE);
}

/*
 * mif68840_lineproto_change:
 * Line protocol change notification.
 */   
void mif68840_lineproto_change (hwidbtype *idb, boolean state)
{
    if (state) {
        reg_invoke_ifmib_link_trap(LINK_UP_TRAP, idb);
    } else {
        reg_invoke_ifmib_link_trap(LINK_DOWN_TRAP, idb);
    }
}

void smt_threshold_command (parseinfo *csb)
{
}

void fddi_config_commands (parseinfo *csb)
{
    hwidbtype   *hwidb;
    leveltype   level;
    fddisb *fsb;
    boolean cmt_config_changed = FALSE;
    ulong oldtrt,oldtvx;

    hwidb = hwidb_or_null(csb->interface);
    if (!hwidb)
        return;

    if (!(hwidb->status & IDB_FDDI)) {
        if (!csb->nvgen)
            printf("\n%%Only allowed on Fddi interfaces");
        return;
    }

    fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    if ( csb->nvgen ) {
        switch (csb->which) {
            case FDDI_INTFC_TRT:
                nv_write((fsb->fddi_trt != DEFAULT_TRT_TIME),
                    "%s %u", csb->nv_command, fsb->fddi_trt);
                break;
            case FDDI_INTFC_TVX:
                nv_write((fsb->fddi_tvx != DEFAULT_T_TVX_TIME),
                    "%s %u", csb->nv_command, fsb->fddi_tvx);
                break;
	        case FDDI_INTFC_TL_MIN:
	            nv_write((STADATA (hwidb->fddi_stn_id, TLMin) != TL_MIN),
		            "%s %u", csb->nv_command,
		        STADATA (hwidb->fddi_stn_id, TLMin));
	            break;
            case FDDI_INTFC_ENCAP:
                if (hwidb->fddi_options & FDDI_ENCAPSULATE)
                    nv_write(TRUE, csb->nv_command);
                break;

            default:
                bad_parser_subcommand(csb, csb->which);
                break;
        }
        idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
        return;
    }

    switch (csb->which) {
        case FDDI_INTFC_ENCAP:
            /*
             * Succeeding in changing the encapsulation mode flushes
             * this ports bridging entries from the bridge cache if
             * any.  We also reset after all of this to clear the CAM
             * and reset the AF to be appropriate for the current
             * bridging state.
             */
            level = raise_interrupt_level(NETS_DISABLE);
            if (!csb->sense) {
                hwidb->fddi_options &= ~FDDI_ENCAPSULATE;
                hwidb->span_encapsize = FDDI_FC;
                hwidb->bridge_transit = FALSE;
                hwidb->firstsw->tbridge_media = TBR_FDDI;
            } else {
                hwidb->fddi_options |= FDDI_ENCAPSULATE;
                hwidb->span_encapsize = FDDI_SNAP_ENCAPBYTES;   /* transit */
                if (hwidb->tbridge_on_hwidb)
                    hwidb->bridge_transit = TRUE;
                hwidb->firstsw->tbridge_media = TBR_FDDI_TRANSIT;
            }
            /*
             * If in a bridging group, clear the bridge cache of any entries.
             */
            reg_invoke_bridge_clear_interface(hwidb->firstsw);
            (*hwidb->reset)(hwidb);
            if (hwidb->counters.resets > 0)
                hwidb->counters.resets--;
            reset_interrupt_level(level);
            idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
            return;

        case FDDI_INTFC_TL_MIN:
            if (!csb->sense || !STADATA (hwidb->fddi_stn_id, TLMin))
                STADATA (hwidb->fddi_stn_id, TLMin) = TL_MIN;
            else
                STADATA (hwidb->fddi_stn_id, TLMin) = GETOBJ(int,1);
            idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
            return;

        case FDDI_INTFC_TRT:
            oldtrt = fsb->fddi_trt;
            if (csb->sense) {
                fsb->fddi_trt = GETOBJ(int,1);
            } else {
                fsb->fddi_trt = DEFAULT_TRT_TIME;
            }
            if (oldtrt != fsb->fddi_trt)
                cmt_config_changed = TRUE;
            break;

        case FDDI_INTFC_TVX:
            oldtvx  = fsb->fddi_tvx;
            if (csb->sense) {
                fsb->fddi_tvx = GETOBJ(int,1);
            } else {
                fsb->fddi_tvx = DEFAULT_T_TVX_TIME;
            }
            if (oldtvx != fsb->fddi_tvx)
                cmt_config_changed = TRUE;

            /*
             *   If smt is initialized, MIB is also initialized so we can
             *   update it, otherwise don't do nothing with it. MIB values
             *   will be copied later automaticaly, after smt is initialized.
             */

            if (STADATA (hwidb->fddi_stn_id, smtUp) == TRUE) {
                MIBDATA (hwidb->fddi_stn_id, mib->macBase[0].hwData.TvxValue) =
                    -MUsecs2Fddi(fsb->fddi_tvx);
            }
            break;

        default:
            bad_parser_subcommand(csb, csb->which);
            idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
            return;
    }
    if ( system_configured && cmt_config_changed ) {
        fddi_disconnect(hwidb->fddi_stn_id);
        init_cmt(hwidb);
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);
}

void mif68840_system_configured (hwidbtype *hwidb)
{
    (*hwidb->reset)(hwidb);
}

/* mif68840_dump_packet: 
 * for fddi debug - dump the iso2 header and some data.
 */
static void mif68840_dump_packet (char *str, uchar *address, int size)
{
    int i;
    buginf("%s: size = %d\n", str, size);
    buginf("data        : ");

    buginf("%2x dst %e src %e  ", address[0], &address[1], &address[7]);
    buginf("\n");
    address += 13;
    for (i=0; i <= (size - 13)/16; i++ ) {
        buginf("%3d: %08x %08x %08x %08x", i,
            GETLONG(address), GETLONG(address+sizeof(long)),
            GETLONG(address+2*sizeof(long)), GETLONG(address+3*sizeof(long)));
            address += 4 * sizeof(long);
        buginf("\n");
    }
}

/*
 * mif68840_post_coalesce_rx_llc:
 * Perform post-coalescing processing on a recieved frame
 */
void mif68840_post_coalesce_rx_llc(hwidbtype *idb,paktype *rx_pak,iqueue_t iq)
{
    fddi_hdr *fddi;
    ushort   rxtype = rx_pak->rxtype;

    fddi = (fddi_hdr *)rx_pak->datagramstart;

    if (PAS_DEBUG_BOOL && fddi_debug) {
        buginf("\n%s: ", idb->hw_namestring);
        mif68840_dump_packet("LLC post_coalesce", rx_pak->datagramstart,
                    rx_pak->datagramsize);
    }

    if (rxtype == RXTYPE_SRB) {   
        if (reg_invoke_fs_rsrb_direct_input(idb, rx_pak)) {
            return;
	}
    }

    /* 
     * If it is IP, check for tunneled stuff
     */
    if (ROUTING_RXTYPE(idb->firstsw, rxtype) && 
	rxtype == RXTYPE_DODIP) {
        iphdrtype *ip;
	
	ip = (iphdrtype *)ipheadstart(rx_pak);
	switch (ip->prot) {
	case GRE_PROT:
	case IPINIP_PROT:
	    if (reg_invoke_tunnel_ip_les_fs(rx_pak)) {
		return;
	    }
	    break;
	case FST_RSRB_PROT:
	    if (reg_invoke_fs_rsrb_fst_input(idb, rx_pak)) {
                return;
	    }
	    break;
	}
    }

    /*
     * Count input broadcasts:
     */
    if (fddi->daddr[0] & 1) {
        idb->counters.input_broad++;
    }

    /*   
     * We did not fast switch the datagram.
     * Pass it up to process level in an appropriate way
     */
    idb->counters.rx_cumbytes += rx_pak->datagramsize;
    idb->counters.inputs++;

    if (fddi->saddr[0] & 0x1)
        rx_pak->flags |= PAK_SRB_IN;
 
    if (srb_multiring_any_enabled(idb->firstsw) || (rx_pak->enctype == ET_LLC2))
        rif_update(idb, 0, fddi->saddr, NULL,
           (srbroute_t *)(idb->rxrifwordlen ? fddi->data:NULL), RIF_AGED);
    if (rx_pak->enctype == ET_LLC2) {
        fddi->saddr[0] &= ~0x1;
        llc_input(rx_pak);
    } else {
        (*(idb->iqueue)) (idb, rx_pak);
    }
}

/*
 * mif68840_post_coalesce_fs:   
 * Perform post-coalesce processing on a recieved frame which we have
 * not even tried to fastswitch yet.
 */
void mif68840_post_coalesce_fs (hwidbtype *idb, paktype *rx_pak, iqueue_t iq)
{
    /*
     * Attempt to fast switch this contigous packet.
     */
    if (reg_invoke_fddi_fs(rx_pak->rxtype, rx_pak)) {
        return;
    }
 
    /*
     * Do the remainder of the interrupt level processing
     */
    mif68840_post_coalesce_rx_llc(idb, rx_pak, iq);
}


/*
 * mif68840_post_coalesce_rx_smt:
 * Perform post-coalescing processing on a recieved frame
 */
void mif68840_post_coalesce_rx_smt (hwidbtype *idb, paktype *rx_pak)
{
    fddi_hdr *fddi;
    uchar *bufp;
    extern queuetype smtbufQ;

    if (fddi_smt_packet_debug) {
        buginf("\n%s: ", idb->hw_namestring);
        mif68840_dump_packet("SMT post_coalesce", rx_pak->datagramstart,
            rx_pak->datagramsize);
    }

    fddi = (fddi_hdr *)rx_pak->datagramstart;

    /*
     * Count input broadcasts:
     */
    if (fddi->daddr[0] & 1) {
        idb->counters.input_broad++;
    }

    idb->counters.rx_cumbytes += rx_pak->datagramsize;
    idb->counters.inputs++;

    bufp = p_dequeue(&smtbufQ);
    if (!bufp) {
        mif68840_instance *ds=INSTANCE(idb);

        fddi_cbuginf("noFBMbuf, ");
        ds->smt_bh_drops++;
        retbuffer(rx_pak);
        return;
    }

    bcopy(rx_pak->datagramstart - 3, bufp, rx_pak->datagramsize + 3);

    if (fddi_smt_packet_debug) {
        buginf("\n%s: bufp = 0x%08x, eac bits = 0x%04x", 
            idb->hw_namestring, bufp, rx_pak->mci_status);
        fddi_smt_debug(rx_pak,FALSE);
    }

    fddi_smt_frame(idb,bufp,rx_pak->datagramsize,(uchar)rx_pak->mci_status);
    retbuffer(rx_pak);
    rx_pak = NULL;
}

/* end of file */

      
