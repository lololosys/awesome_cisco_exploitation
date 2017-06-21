/* $Id: if_vip_mif68840.c,v 3.1.58.18 1996/09/15 17:18:57 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_vip_mif68840.c,v $
 *------------------------------------------------------------------
 * if_vip_mif68840.c - 1F MIF68840 based PCI port adaptor support
 *                  (VIP specific components)
 *
 * August 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vip_mif68840.c,v $
 * Revision 3.1.58.18  1996/09/15  17:18:57  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   Backout previous changes that were not required after all
 *
 * Revision 3.1.58.17  1996/09/13  19:48:06  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   - Add SRB subblock object to VIP ucode (needs to be its own subsys)
 *   - Create SRB subblock at same time tokenRing/FDDI swidb created
 *   - Init SRB vector table when interface is reset
 *
 * Revision 3.1.58.16  1996/08/28  13:06:55  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.58.15  1996/08/26  15:11:11  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1.58.14  1996/07/23  13:27:39  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.58.13  1996/07/17  01:45:06  fsunaval
 * CSCdi59824:  RIF classification done on the VIP for the 1F.
 * Branch: California_branch
 * 1.Off-load RIF classification from the RSP to the VIP for the 1F.
 *   What's next, fragmentation?
 *
 * Revision 3.1.58.12  1996/06/18  20:05:42  fsunaval
 * CSCdi59153:  Provide support for cmt con and cmt dis.
 * Branch: California_branch
 * 1."cmt con" and "cmt dis" put into platform specific files.
 *
 * Revision 3.1.58.11  1996/06/11  13:43:45  dcarroll
 * CSCdi59788:  Router crashes doing OIR on 4E an 4R port adapters
 * Branch: California_branch
 * Use pas_allocate_fspak() for rx_pak.
 *
 * Revision 3.1.58.10  1996/06/10  02:01:12  fsunaval
 * CSCdi57738:  VIP2 SW: FDDI PA doesnt show claim, beacon counter at RSP
 * level.
 * Branch: California_branch
 * 1.Fix the claim, beacon and trace counters.
 * 2.Make the MIB in the RSP aware about single/multimode interface.
 * 3.Remove excess debugging stuff.
 *
 * Revision 3.1.58.9  1996/06/07  06:59:12  fsunaval
 * CSCdi54755:  Fixes for SMT conformance tests.
 * Branch: California_branch
 * 1.Fix trace test and other minor MIB values.
 *
 * Revision 3.1.58.8  1996/05/21  10:00:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.58.7  1996/05/19  03:18:49  fsunaval
 * CSCdi55239:  FDDI driver needs to correctly mimic AF status bits
 * settings
 * Branch: California_branch
 *
 * Revision 3.1.58.6  1996/05/06  19:27:37  fsunaval
 * CSCdi56768:  Update trt_neg
 * Branch: California_branch
 * 1.Pass one more parameter throught the MIB loveletter.
 *
 * Revision 3.1.58.5  1996/04/30  07:01:12  fsunaval
 * CSCdi51250:  SW-VIP1: Host MIB updating needs to be completed
 * Branch: California_branch
 * 1.Code changes for formation and transfer of SMT Messages to RSP
 *   in form of love letters.
 *
 * Revision 3.1.58.4  1996/04/13  04:39:06  fsunaval
 * CSCdi54379:  Optical bypass not working on new port adapter.
 * Branch: California_branch
 * 1.Pri. and Sec. relays do not have to be energized after configuration
 *  change.
 *
 * Revision 3.1.58.3  1996/04/11  04:39:44  fsunaval
 * CSCdi53600:  SW-SVIP: SVIP Fddi optimizations.
 * Branch: California_branch
 * 1.Take advantage of posted read hardware.
 * 2.Take advantage of Management interrupts.
 *
 * Revision 3.1.58.2  1996/04/05  05:32:34  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.1.58.1  1996/03/21  23:24:00  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.8  1996/03/06  20:50:29  fsunaval
 * CSCdi50792:  Distributing switch not working for fddi-pa.
 * Branch: ELC_branch
 *
 * Revision 3.1.4.7  1996/02/21  22:49:28  mbeesley
 * CSCdi49496:  FDDI PA needs to set up txring_cnt_ptr
 * Branch: ELC_branch
 *
 * Revision 3.1.4.6  1996/02/08  08:48:16  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.1.4.5  1996/02/05  07:00:13  fsunaval
 * Branch: ELC_branch
 * 1.Particle based fast-switching Fddi support and code cleanup.
 *
 * Revision 3.1.4.4  1996/01/25  12:10:09  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.1.4.3  1996/01/16  01:43:45  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.4.2  1995/12/26  19:56:45  fsunaval
 * CSCdi46111:  Add parser support for FDDI PA and other changes
 * Branch: ELC_branch
 * 1.Add missing diffs from previous commit for init_pas.c
 *   (NMI interrupts).
 * 2.Add FDDI PA parser support (sh cont, test pas fddi).
 * 3.Do a little code cleanup of hes_fddi.c and rsp_fip.c specifically
 *   in regards to the way trt and tvx are passed down.
 *
 * Revision 3.1.4.1  1995/12/23  04:41:28  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/11/09  12:58:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/24  19:46:28  fsunaval
 * Placeholder files for new development.
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
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"
#include "subsys.h"
#include "sched.h"
#include "parser.h"

 #include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"  
#include "../os/free.h"
   
#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "../if/pci.h"
#include "../if/network.h"

#include "mgd_timers.h"
     
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
 
#include "../hes/ccb_cmd.h"
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
#include "../xdi/motomac.h"
#include "../if/parser_defs_fddi.h"

#include "../if/if_fddi.h"
#include "../if/if_fddi_public.h"
#include "../if/if_xdi_public.h"

#include "if_pas.h"
#include "if_pas_mif68840.h"
#include "../hes/if_vip.h"

#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"

static  watched_queue *fddi_watch_q;
pid_t   mif68840_loveletter_pid = 0;
static  process fddiCSP_process (void);

/*
 * vip_send_1f_mib_info
 */
static void vip_send_1f_mib_info (hwidbtype *idb)
{
    uInt16          stnID = idb->fddi_stn_id;
    mif68840_instance *ds = INSTANCE(idb);
    vip_1f_mib_info ll;
    fddisb          *fsb  = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);

    ll.ll_type = VIP_LL_1F_MIBS;

    ll.fddi_trt_neg = fsb->fddi_trt_neg;
    ll.multimode = (ds->csr & MULTIMODE_PHYB_PHYA ?  1 : 0);
    ll.ECMState= MIBDATA(stnID, mib->smtData.ECMState);
    ll.CF_State= MIBDATA(stnID, mib->smtData.CF_State);
    ll.RMTState= MIBDATA(stnID, mib->macBase[0].hwData.RMTState);
    ll.NeighborType[0] = MIBDATA(stnID,mib->portBase[0].hwData.NeighborType);
    ll.NeighborType[1] = MIBDATA(stnID,mib->portBase[1].hwData.NeighborType);
    ll.PMDClass[0] = MIBDATA(stnID,mib->portBase[0].hwData.PMDClass);
    ll.PMDClass[1] = MIBDATA(stnID,mib->portBase[1].hwData.PMDClass);
    ll.ConnectState[0] = MIBDATA(stnID,mib->portBase[0].hwData.ConnectState);
    ll.ConnectState[1] = MIBDATA(stnID,mib->portBase[1].hwData.ConnectState);
    ll.PCMState[0] = MIBDATA(stnID,mib->portBase[0].hwData.PCMState);
    ll.PCMState[1] = MIBDATA(stnID,mib->portBase[1].hwData.PCMState);
    ll.PC_LS[0] = ReadLineStateMulti (stnID, 0);
    ll.PC_LS[1] = ReadLineStateMulti (stnID, 1);
    ll.Ler_Estimate[0] = MIBDATA(stnID,mib->portBase[0].hwData.Ler_Estimate);
    ll.Ler_Estimate[1] = MIBDATA(stnID,mib->portBase[1].hwData.Ler_Estimate);
    ll.SM_MA_Avail = (idb->state) ? TRUE : FALSE;

    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);

    if (!vip_send_loveletter(&ds->pacb, (uchar *)&ll,
                sizeof(vip_1f_mib_info))) {
        if (fddi_debug)
            buginf("\nvip_send_1f_mib_info: vip_send_loveletter() failed");
    }
}

/*
 * mif68840_cmd_dispatch:
 * Handle deferred malu attention comands
 */
static void mif68840_cmd_dispatch (malu_cmd_t *cmd)
{
    pas_cbtype *pascb = cmd->pascb;
    hwidbtype *idb;
    srb_vector_table_t *srbV_p;
    mif68840_instance *ds;
    fddisb *fsb;
    uInt16 stnID;
    leveltype SR;
    uchar mac_address[IEEEBYTES];
    
    if (!pascb) {
        errmsg(&msgsym(BADPASCB, PA), pascb);
    return;
    }
    idb = pascb->idb;
    if (!idb) {
        errmsg(&msgsym(BADIDB, PA), idb);
    return;
    }
    ds=INSTANCE(idb);
    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);
    stnID = idb->fddi_stn_id;

    /*
     * Process the command
     */
    switch (cmd->cmd) {
 
    case CCB_CMD_VIP_1F_BRIDGE:
        switch(cmd->arg0){
        case 0:
            ds->bridge = 0;
            break;
        case 1:
            ds->bridge = AF_BRIDGING_ADDRESS;
            break;
        }
        break;

    case CCB_CMD_VIP_1F_TVX:
        fsb->fddi_tvx = cmd->arg0;
        break;

    case CCB_CMD_VIP_1F_TRT:
        fsb->fddi_trt = cmd->arg0 * 1000;
        break;

    case CCB_CMD_VIP_1F_STN_ADD:
        bcopy(&cmd->arg0, &idb->hardware[0], 2);
        bcopy(&cmd->arg1, &idb->hardware[2], 2);
        bcopy(&cmd->arg2, &idb->hardware[4], 2);
        break;

    case CCB_CMD_VIP_1F_INTERESTING_ADD:
        bcopy(&cmd->arg0, &ds->interesting_add[0], 2);
        bcopy(&cmd->arg1, &ds->interesting_add[2], 2);
        bcopy(&cmd->arg2, &ds->interesting_add[4], 2);
        (*idb->listen)(idb->firstsw, ds->interesting_add, 1);
        break;

    case CCB_CMD_VIP_1F_ALL_MULTICAST_ADD:
        ds->promiscuous_llc = TRUE;
        break;

    case CCB_CMD_ERRORS:

        idb->counters.input_frame = 
            MIBDATA(stnID, mib->macBase[0].hwData.Lost_Ct);
        idb->counters.input_crc   = 
            MIBDATA(stnID, mib->macBase[0].hwData.Error_Ct);
        vip_send_interface_stats(idb, cmd);
        break;

    case CCB_CMD_VIP_1F_GET_MIB_INFO:
        vip_send_1f_mib_info(idb);
        break;

    case CCB_CMD_VIP_1F_SRB:
        switch(cmd->arg0) {
        case 0: 
            ds->srbridge_on_idb = FALSE;
            break;
        case 1:
            ds->srbridge_on_idb = TRUE;
            break;
        case 2:
            srbV_p = reg_invoke_srb_get_vector_table();
            if (srbV_p) {
              srbV_p->srb_sb_set_rng_br_rng_f(idb->firstsw, 
                                          (cmd->arg1 << 16) | (cmd->arg2));
            }
            break;
        case 3:
            srbV_p = reg_invoke_srb_get_vector_table();
            if (srbV_p) {
              srbV_p->srb_sb_set_rng_br_rng_f(idb->firstsw, 
                                          (cmd->arg1 << 16) | (cmd->arg2));
            }
            break;
        }
        break;

    case CCB_CMD_FIP_GET_STATE:
        break;

    case CCB_CMD_DELAY: 
        break;
 
    case CCB_CMD_RXBYTES:
        break;

    case CCB_CMD_STATUS:
        vip_control_interface(idb, cmd);
        vip_send_1f_mib_info(idb);
        break;

    case CCB_CMD_FIP_KICK_PHY:
        if (cmd->arg0 & FCI_FDDI_PHY_B) {
            if (cmd->arg0 & FCI_FDDI_START_PHY) {
                SendSignalMulti (stnID, SIG_PC_Enable, 1, (uInt32) 0);
                SendSignalMulti (stnID, SIG_PC_Start, 1, (uInt32) 0);
            } else {
                SendSignalMulti (stnID, SIG_PC_Disable, 1, (uInt32) 0);
            }
        } else {
            if (cmd->arg0 & FCI_FDDI_START_PHY) {
                SendSignalMulti (stnID, SIG_PC_Enable, 0, (uInt32) 0);
                SendSignalMulti (stnID, SIG_PC_Start, 0, (uInt32) 0);
            } else {
                SendSignalMulti (stnID, SIG_PC_Disable, 0, (uInt32) 0);
            }
        }

        /* Process signals */
        if (!EmptySignalQueue ()) {
            MDisableCSPInterrupts(&SR);
            ProcessCSP (&SR);
            MRestoreCSPInterrupts(&SR);
        }
        break;
    case CCB_CMD_VIP_CAM_POPULATE:
        bcopy(&cmd->arg0, &mac_address[0], 2);
        bcopy(&cmd->arg1, &mac_address[2], 2);
        bcopy(&cmd->arg2, &mac_address[4], 2);
        mif68840_fddi_cam_populate(idb, mac_address);
        break;
    case CCB_CMD_VIP_CAM_DEPOPULATE:
        bcopy(&cmd->arg0, &mac_address[0], 2);
        bcopy(&cmd->arg1, &mac_address[2], 2);
        bcopy(&cmd->arg2, &mac_address[4], 2);
        mif68840_fddi_cam_depopulate(idb, mac_address, NULL);
        break;

    default:
        vip_unsupported_cmd(cmd);
        break;
    }
    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);  
}
    
/*
 * mif68840_idb_create:
 * Create an idb for a MIF68840 fddi interface. VIP platforms
 * do not support hot swap, so just create it.
 */
hwidbtype *mif68840_idb_create (uint slot, uint port)
{
    hwidbtype *idb;
 
    idb = idb_create();
    if (idb) {
        idb->oir_flags |= (IDB_FIRST_INIT | IDB_FINAL_INIT);
    }
    return(idb);
}

/*   
 * mif68840_deallocate: 
 * Clean up all resourcse for this interface. VIP does not support
 * hotswap, so there is no need to cleanup.
 */
void mif68840_deallocate (hwidbtype *hwidb)
{
}

/*   
 * mif68840_platform_init:
 * Platform dependant initialization for MIF68840 port adaptor interface.
 */
boolean mif68840_platform_init (hwidbtype *idb)
{
    mif68840_instance *ds = INSTANCE(idb);
    pas_cbtype *pascb = INSTANCE(idb);

    if(! mif68840_loveletter_pid)
        mif68840_loveletter_pid = process_create (fddiCSP_process,
                            "CSP Love letter", NORMAL_STACK, PRIO_NORMAL);

    /*
     * Setup fci_type
     */
    idb->fci_type = FCI_TYPE_VIP_FDDI;
    
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
    install_cmd_dispatch(idb, mif68840_cmd_dispatch);

    /* 
     * Setup the tx poll limit and a pointer to the drivers
     * tx ring counter.
     */
    pascb->tx_polling_high = MIF68840_TXPOLL_HIGH;
    pascb->tx_polling_low = MIF68840_TXPOLL_LOW;
    pascb->txring_cnt_ptr = &ds->tx_count;
   
    /*
     * Install the status register for use by the posted read hw.
     */
    pas_install_status_reg(idb, (void *)&ds->mif68840_regaddr->ifddi[SR1],
            MIF68840_MEMMAPPED_IO);
 
    /*
     * Install this interface into the pa_jumptable
     */
    pas_install_port_instance(idb, TRUE);

    /* Initialize XDI SMT interface code */
    if (MIBMainMulti(idb->fddi_stn_id)) {
        errmsg(&msgsym(NOMEMORY, FDDI), idb->fddi_stn_id, "MIBMainMulti");
        return(FALSE);
    }

    return(TRUE);
}

/*
 * mif68840_get_default_hw_address:
 * Get a default mac address for this interface
 */
void mif68840_get_default_hw_address (hwidbtype *idb, uchar *addr)
{
}


/*
 * mif68840_init_buffer_pools:
 * Initialize buffer pools for this interface
 */   
boolean mif68840_init_buffer_pools (hwidbtype *idb)
{

    mif68840_instance *ds;
    pooltype *pool, *ll_pool_ptr;
    int particles_in_pool;
    mempool *buffer_mempool;
    mempool *ll_mempool = mempool_find_by_class(MEMPOOL_CLASS_LOCAL);
    leveltype level;

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
    }
       
    /*
     * Init the header pool
     */
    if (!idb->headerpool) {
        idb->headerpool = pas_interface_header_pool(idb);
    }
    
    /*
     * Init the static rx_paks
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

    level = raise_interrupt_level(ALL_DISABLE);
    ds->waitq_pak   = NULL;
    ds->waitmsg_count = 0;
    ds->pak_on_watchq = FALSE;
    ll_pool_ptr = pak_pool_create("Loveletter_pool",
                POOL_GROUP_PUBLIC, MSGS_PER_LOVELETTER * SIZE_SMTMessage,
                POOL_DYNAMIC, ll_mempool);
    reset_interrupt_level(level);

    if(ll_pool_ptr) {
        pool_adjust(ll_pool_ptr, 2, MAX_LOVE_LETTERS,
                    MAX_LOVE_LETTERS, TRUE);
        ds->ll_pool_ptr = ll_pool_ptr;
    } else {
        return(FALSE);
    }

    return(TRUE);
}

/*
 * mif68840_lineproto_change:
 * Line protocol change notification.
 */   
void mif68840_lineproto_change (hwidbtype *idb, boolean state)
{
    /*
     * Call the VIP kernel, which will send indication
     * to the master wrt to the line state change.
     */
    vip_lineproto_change(idb, state);

    /* 
     * Update the MIB Info in the RSP
     * vip_send_1f_mib_info(idb); 
     * No longer necessary since the RSP will send the command upon 
     * getting a lovenote. This is necessary due to the NMI shit
     */
}

void smt_threshold_command (parseinfo *csb)
{
}

void fddi_config_commands (parseinfo *csb)
{
}

void mif68840_system_configured (hwidbtype *hwidb)
{
}

void fddiSendCSP_loveletter(void)
{
    uInt16 stnID;
    paktype *pak;
    SMTMessage  *msg;
    mif68840_instance *ds;
    leveltype   level;
    ulong ll_type;

    while((pak = process_dequeue(fddi_watch_q)) != NULL) {
    msg   = (SMTMessage *)(pak->datagramstart + sizeof(ll_type));
    stnID = msg->stnID;
    ds    = (mif68840_instance *)STADATA (stnID, xferData[0]);
    /*
     * Disable interrupts and make sure the int handler doesn't see this
     * packet anymore. MAKE SURE pak is the same as ds->pak. Guess why?
     */
    level = raise_interrupt_level(ALL_DISABLE);
    if(pak == ds->waitq_pak) {
        ds->waitq_pak     = NULL;
        ds->waitmsg_count = 0;
        ds->pak_on_watchq = FALSE;
    }
    reset_interrupt_level(level);
    if (!vip_send_loveletter(&ds->pacb, pak->datagramstart, pak->datagramsize))
    {
        process_sleep_for(PROC_SLEEP_TIME);
        /* Could not send the love letter. Put the msg back in front of
         * process-q and break. Try again when this process is invoked by
         * queue event or timer event.
         */
        process_requeue (fddi_watch_q, pak);
        break;
    } else {
        /* The job is over. Return buffer to its pool. No body else
         * is expected to use this buffer simultaneously. Hence the
         * reference count shd be one. Use retbuffer with ints disabled.
         */
        retbuffer(pak);
    }
    }
    return;
}

void SendCSP_loveletter (SMTMessage * msg)
/* CAUTION: This function is always called from interrupt handler.
 * If future code changes cannot continue to do so, disable interrupts
 * at the begining and enable them before exiting from this function.
 */
{
    paktype *pak;
    ulong   ll_type, offset=0;
    uInt16 stnID   = msg->stnID;
    mif68840_instance *ds = STADATA (stnID, xferData[0]);

    pak = ds->waitq_pak;
    if(ds->waitq_pak) {
        if(ds->waitmsg_count < MSGS_PER_LOVELETTER)  {
            offset = sizeof(ll_type) + ((ds->waitmsg_count) * SIZE_SMTMessage);
        }
        if(ds->waitmsg_count == MSGS_PER_LOVELETTER) {
            if(! ds->pak_on_watchq) {
            /*
             * This means the pak could not be enqueued earlier.
             * Try queing it now. If we fail again, there is something
             * wrong. Stop accepting new SMTMessages in that case.
             */
                if(! process_enqueue (fddi_watch_q, pak)) {
                    if(fddi_debug)
                        buginf("SendCSP_loveletter: Failed to enqueue\n");
                        return;
                } 
            }
            ds->waitq_pak     = NULL;
            ds->waitmsg_count = 0;
            ds->pak_on_watchq = FALSE;
        }
    }
    /* We need the following check again as it may have become NULL
     * in the step above. 
     */
    if (!ds->waitq_pak) {
        pak = pool_getbuffer(ds->ll_pool_ptr);
        if (!pak) {
            return; /* Sorry, no buffers! */
        }
        offset = sizeof(ll_type);
        ds->waitmsg_count = 0;
        ds->pak_on_watchq = FALSE;
        *(ulong *)(pak->datagramstart) = VIP_LL_1F_LUVL;
        pak->datagramsize = sizeof(ll_type);
    }
    bcopy((uchar *)msg, (pak->datagramstart+offset), SIZE_SMTMessage);
    pak->datagramsize += SIZE_SMTMessage;
    ds->waitq_pak      = pak;
    ds->waitmsg_count++;
    /*
     * Put the pak on watch-q if it is not already on it.
     */
    if(! ds->pak_on_watchq) {
        if(process_enqueue (fddi_watch_q, pak)) {
            ds->pak_on_watchq = TRUE;
        } else {
            if (fddi_cmt_events_debug)
                buginf("Failed to q in fddi_watch_q. pak = %x\n", pak);
        }
    }  
    /*
     * If process_enqueue failed above, we can still try next time as
     * the pak is safe in ds->waitq_pak.
     */
    return;
}

process fddiCSP_process (void)
{
    ulong   major, minor;

    fddi_watch_q = create_watched_queue("FDDI VIP watched-q", 0, 0);
    process_watch_queue(fddi_watch_q, ENABLE, RECURRING);

    while (TRUE) {
        process_wait_for_event();
        while (process_get_wakeup(&major, &minor)) {
            switch (major) {
                case QUEUE_EVENT:
                case TIMER_EVENT:
                    fddiSendCSP_loveletter();
                    break;
                case MESSAGE_EVENT:
                default:
                    break;
            }
        }
    }
}

