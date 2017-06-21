/* $Id: if_pas_mif68840.c,v 3.1.60.33 1996/09/15 00:52:26 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_mif68840.c,v $
 *------------------------------------------------------------------
 * if_pas_mif68840.c - 1F MIF68840 based PCI port adaptor support
 *                  (Platform independent components)
 *
 * August 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_mif68840.c,v $
 * Revision 3.1.60.33  1996/09/15  00:52:26  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   Add srb_create_sb_for_vip_inline() so that SRB subblock can be
 *     created for VIP when swidb is initialized
 *
 * Revision 3.1.60.32  1996/09/14  01:04:35  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   Previous commit breaks build for c7200- images
 *     Backout previous changes
 *     Create SRB_SB subsystem to init SRB vector table
 *       rather than init in  VIP reset routines
 *
 * Revision 3.1.60.31  1996/09/13  19:48:02  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   - Add SRB subblock object to VIP ucode (needs to be its own subsys)
 *   - Create SRB subblock at same time tokenRing/FDDI swidb created
 *   - Init SRB vector table when interface is reset
 *
 * Revision 3.1.60.30  1996/08/28  13:06:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.60.29  1996/08/26  15:11:04  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1.60.28  1996/08/22  18:47:16  dujihara
 * CSCdi65826:  Error messages do not specify slot/subunit
 * Branch: California_branch
 * Added slot (bay) and unit to error messages.
 *
 * Revision 3.1.60.27  1996/07/17  01:45:03  fsunaval
 * CSCdi59824:  RIF classification done on the VIP for the 1F.
 * Branch: California_branch
 * 1.Off-load RIF classification from the RSP to the VIP for the 1F.
 *   What's next, fragmentation?
 *
 * Revision 3.1.60.26  1996/07/09  06:04:06  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.1.60.25  1996/06/28  23:25:20  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.60.24  1996/06/18  20:05:40  fsunaval
 * CSCdi59153:  Provide support for cmt con and cmt dis.
 * Branch: California_branch
 * 1."cmt con" and "cmt dis" put into platform specific files.
 *
 * Revision 3.1.60.23  1996/06/10  22:21:45  mbeesley
 * CSCdi59552:  software forced reload due to memory corruption in
 * 2FPA-2FPA test. Use 32 byte burst, Memory Read Line on Predator. Also
 * remove debugging buginfs.
 * Branch: California_branch
 *
 * Revision 3.1.60.22  1996/06/10  03:09:29  fsunaval
 * CSCdi58917:  alignment problems with fddi_reset
 * Branch: California_branch
 *
 * Revision 3.1.60.21  1996/06/07  06:59:09  fsunaval
 * CSCdi54755:  Fixes for SMT conformance tests.
 * Branch: California_branch
 * 1.Fix trace test and other minor MIB values.
 *
 * Revision 3.1.60.20  1996/05/31  01:26:20  fsunaval
 * CSCdi57339:  Implement throttling on the 1F.
 * Branch: California_branch
 * 1.Throttle the interface when no more particles available.  The
 *   interface is unthrottled when the cache has a certain threshold of
 *   particles in it.
 *
 * Revision 3.1.60.19  1996/05/28  07:06:51  fsunaval
 * CSCdi58107:  Long stress run in DFS gets malloc error in FDDI PA.
 * Branch: California_branch
 * 1.If RX particle has error, enqueue it before calling cleanup routine.
 *
 * Revision 3.1.60.18  1996/05/21  10:00:05  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.60.17  1996/05/19  03:18:45  fsunaval
 * CSCdi55239:  FDDI driver needs to correctly mimic AF status bits
 * settings
 * Branch: California_branch
 *
 * Revision 3.1.60.16  1996/05/18  08:54:45  fsunaval
 * CSCdi57640:  1F - Software tweaks to augment new EPLD.
 * Branch: California_branch
 * 1.Remove 1F specific latency timer changes.
 *
 * Revision 3.1.60.15  1996/05/18  08:34:15  fsunaval
 * CSCdi57554:  FDDI PA needs OIR support
 * Branch: California_branch
 * 1.1F OIR support and patch for CSCdi57554.
 *
 * Revision 3.1.60.14  1996/05/17  11:40:08  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.46.3  1996/05/05  23:43:03  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.1.46.2  1996/04/26  15:22:43  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.46.1  1996/04/03  21:14:30  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1.60.13  1996/05/09  14:40:31  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.60.12  1996/05/07  00:38:55  mbeesley
 * CSCdi56232:  Enable IP Flow/Optimum on fddi
 * Branch: California_branch
 *
 * Revision 3.1.60.11  1996/04/30  07:01:07  fsunaval
 * CSCdi51250:  SW-VIP1: Host MIB updating needs to be completed
 * Branch: California_branch
 * 1.Code changes for formation and transfer of SMT Messages to RSP
 *   in form of love letters.
 *
 * Revision 3.1.60.10  1996/04/29  17:57:24  ssangiah
 * CSCdi54472: Inadequate information in the output of "sh controllers"
 *             command
 *   o Add a "show diag" command for the c7200 platform.
 *   o Make the output from "show controllers" consistent.
 * Branch: California_branch
 *
 * Revision 3.1.60.9.6.1  1996/04/27  08:00:56  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.60.9  1996/04/13  04:39:02  fsunaval
 * CSCdi54379:  Optical bypass not working on new port adapter.
 * Branch: California_branch
 * 1.Pri. and Sec. relays do not have to be energized after configuration
 *  change.
 *
 * Revision 3.1.60.8  1996/04/11  04:39:41  fsunaval
 * CSCdi53600:  SW-SVIP: SVIP Fddi optimizations.
 * Branch: California_branch
 * 1.Take advantage of posted read hardware.
 * 2.Take advantage of Management interrupts.
 *
 * Revision 3.1.60.7  1996/04/08  19:00:57  dcarroll
 * CSCdi53796:  Remove extra idb type assignments
 * Branch: California_branch
 * Remove the unneeded early setting of idb type.
 *
 * Revision 3.1.60.6  1996/04/05  19:56:26  dcarroll
 * CSCdi53769:  Return bool from pas init interface routines
 * Branch: California_branch
 * Pass boolean up the analyze chain, not idbs.
 *
 * Revision 3.1.60.5  1996/04/05  05:32:21  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.1.60.4  1996/04/04  20:04:21  getchell
 * CSCdi50943:  SVIP PA reset broken
 * Branch: California_branch
 *
 * Revision 3.1.60.3  1996/04/02  03:44:10  fsunaval
 * CSCdi53238:  Remove IDB_CBUS from the status field of new port adapter.
 * Branch: California_branch
 * 1.Remove IDB_CBUS from the idb->status field of port adapters.
 *
 * Revision 3.1.60.2  1996/03/28  02:58:13  fsunaval
 * CSCdi50890:  SW-VIP1: FDDI PA gets RX_Ovr_Par Error
 * Branch: California_branch
 *
 * Revision 3.1.4.9  1996/03/22  19:19:43  fsunaval
 * CSCdi50890:  SW-VIP1: FDDI PA gets RX_Ovr_Par Error
 * Branch: ELC_branch
 *
 * Revision 3.1.4.8  1996/03/22  18:48:03  getchell
 * CSCdi50943:  SVIP PA reset broken
 * Branch: ELC_branch
 *
 * Revision 3.1.4.7  1996/02/26  18:43:45  fsunaval
 * CSCdi49792:  Provide bridging support for fddi on Predator
 * Branch: ELC_branch
 *
 * Revision 3.1.4.6  1996/02/26  04:39:21  fsunaval
 * CSCdi49791:  Incorporate code-review comments for Fddi port adapter
 * Branch: ELC_branch
 *
 * Revision 3.1.4.5  1996/02/05  07:00:10  fsunaval
 * Branch: ELC_branch
 * 1.Particle based fast-switching Fddi support and code cleanup.
 *
 * Revision 3.1.4.4  1996/01/25  12:10:07  fsunaval
 * Branch: ELC_branch
 * 1.Predator FDDI support.
 * 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
 *
 * Revision 3.1.4.3  1996/01/16  01:43:10  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.1.4.2  1995/12/26  19:56:42  fsunaval
 * CSCdi46111:  Add parser support for FDDI PA and other changes
 * Branch: ELC_branch
 * 1.Add missing diffs from previous commit for init_pas.c
 *   (NMI interrupts).
 * 2.Add FDDI PA parser support (sh cont, test pas fddi).
 * 3.Do a little code cleanup of hes_fddi.c and rsp_fip.c specifically
 *   in regards to the way trt and tvx are passed down.
 *
 * Revision 3.1.4.1  1995/12/23  04:41:26  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/11/09  12:57:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/24  19:46:27  fsunaval
 * Placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper            /* use library function call */
#include <ciscolib.h>
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"   
#include "ttysrv.h" 
#include "logger.h"
#include "subsys.h"
#include "sched.h"
#include "parser.h"
#include "mgd_timers.h"
 
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
 
#include "../if/fddi.h"
#include "../if/fddi_private.h"
#include "../if/pci.h"
#include "../if/network.h"
#include "../ip/ip.h"
 
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../if/parser_defs_fddi.h"

#include "../smf/smf_public.h"

#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"

#include "../srt/srb_sb_private.h"

#include "../xdi/xdihdrsif.h" 
#include "../xdi/fddihdr.h"
#include "../xdi/smtmsg.h"
#include "../xdi/csphdr.h"
#include "../xdi/cspglbl.h"
#include "../xdi/cspproto.h"
#include "../xdi/hw_csp.h"
#include "../xdi/hw_phy.h"
#include "../xdi/motomac.h"
#include "../xdi/mibdefs.h"
#include "../xdi/mibglbl.h"
#include "../xdi/mibproto.h"
#include "../xdi/fbmframe.h"
#include "../xdi/fbmhdr.h"
#include "../xdi/fbmglbl.h"
#include "../xdi/fbmproto.h"

#include "../if/if_fddi.h"
#include "../if/if_fddi_public.h"
#include "../if/if_xdi_public.h"

#include "if_pas.h"
#include "if_pas_mif68840.h"
#include "msg_mif68840.c"		/* Not a typo, see logger.h */
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "pas_id.h"
#include "access.h"
#include "../hes/dgbus.h"

/*
 * Defintions
 */
#define BUFLEN          80

/* globals */
int smt_process_id = 0;
int mif68840_CSPTimer_process_id;

static boolean test_in_progress;

extern mgd_timer      mif68840_CSPTimer;

/************************************************ 
 * show routines  (all the "sh cont fddi" stuff) 
 ************************************************/

/*
 * mif68840_print_pci_regs:
 * Print out the PCI registers for a MIF68840 interface
 */
static void mif68840_print_pci_regs (hwidbtype *idb)
{
    mif68840_instance *ds = INSTANCE(idb);
    uint cfid, cfcs, cfrv, cflt, cbmem, cbio, cblocal, cfit, pa_bay;
    uchar bus_no;
    plx9060_local_regs *plxptr;

    plxptr = (plx9060_local_regs *) ds->mif68840_regaddr;
     
    /* 
     * Get bus number
     */
    pa_bay = ds->pacb.pa_bay;   
    bus_no = get_pci_bus_no(pa_bay); 
       
    printf("\n MIF68840 PCI registers:");
    
    printf("\n  bus_no=%d, device_no=%d", bus_no, ds->pacb.pa_device);

    cfid = pci_config_read(bus_no, ds->pacb.pa_device, 0,
               MIF68840_PCI_CFID_REG_OFFSET );  
    cfcs = pci_config_read(bus_no, ds->pacb.pa_device, 0,
               MIF68840_PCI_CFCS_REG_OFFSET);
    cfrv = pci_config_read(bus_no, ds->pacb.pa_device, 0,
               MIF68840_PCI_CFRV_REG_OFFSET);
    cflt = pci_config_read(bus_no, ds->pacb.pa_device, 0,
               MIF68840_PCI_CFLT_REG_OFFSET);
    cbmem = pci_config_read(bus_no, ds->pacb.pa_device, 0,
                          MIF68840_PCI_CBMEM_REG_OFFSET);
    cbio = pci_config_read(bus_no, ds->pacb.pa_device, 0,
                          MIF68840_PCI_CBIO_REG_OFFSET);
    cblocal = pci_config_read(bus_no, ds->pacb.pa_device, 0,
                          MIF68840_PCI_CBLOCAL_REG_OFFSET);
    cfit = pci_config_read(bus_no, ds->pacb.pa_device, 0,
                          MIF68840_PCI_CFIT_REG_OFFSET);

    /* Print it all out */
    printf("\n  CFID =0x%08x, CFCS=0x%08x, CFRV=0x%08x, CFLT=0x%08x",
       cfid, cfcs, cfrv, cflt);
    printf("\n  CBMEM=0x%08x, CBIO=0x%08x, CFIT=0x%08x",
       cbmem, cbio, cfit);

    /* And finally print out the local registers */
    printf("\n  range_pci_local=0x%08x, remap_pci_local=0x%08x",
        plxptr->range_pci_local, plxptr->remap_pci_local);
    printf("\n  local arb. reg =0x%08x", plxptr->local_arb_reg);
    printf("\n  bus_descriptors=0x%08x, range_local_pci=0x%08x",
        plxptr->bus_descriptors_pci_local, plxptr->range_local_pci);
    printf("\n  remap_local_pci=0x%08x, cfglocal_pci_io=0x%08x",
        plxptr->remap_local_pci, plxptr->cfg_addr_reg_local_pci_io);
    printf("\n  plx_int_status =0x%08x, plx_eeprom_ctrl=0x%08x",
            plxptr->interrupt_control_status, plxptr->eeprom_control);
}


uchar read_fcr (mif68840_instance *ds, ulong value)
{
    mif68840_regs *ptr = ds->mif68840_regaddr;
    FCR_IS_FREE;
    ptr->ifddi[FCR] = value;
    FCR_IS_FREE;
    return((ptr->ifddi[FCR] & 0x00ff0000) >> 16);
}



/*
 * ifddi_show_regs:
 * Print out the MIF68840 registers.
 */
static void ifddi_show_regs (hwidbtype *idb)
{
    mif68840_instance *ds=INSTANCE(idb);
    mif68840_regs *ptr; 
    ulong   regval;
    
    ptr = ds->mif68840_regaddr;

    printf("\n");
    printf("\n MIF68840 internal registers");
    printf("\n  IFDDI registers");
    printf("\n    SR1  =0x%08x, IMR1 =0x%08x, PSR  =0x%08x",
    ptr->ifddi[SR1], ptr->ifddi[IMR1], ptr->ifddi[PSR]);
    printf("\n  IFDDI FCR registers");

    printf("\n    MTR  =0x%02x, MRR  =0x%02x", 
        read_fcr(ds, READ_MACIF_TRANSMIT_REG),
        read_fcr(ds, READ_MACIF_RECEIVE_REG));

    printf("\n    ICR  =0x%02x", read_fcr(ds, READ_ICR));

    printf("\n    RFR4 =0x%02x, RFR5 =0x%02x", 
        read_fcr(ds, READ_RFR_4),
        read_fcr(ds, READ_RFR_5));

    printf("\n    BLRA =0x%02x, BLRB =0x%02x",
        read_fcr(ds,READ_BURST_LIMIT_REG_A),
        read_fcr(ds,READ_BURST_LIMIT_REG_B));

    printf("\n    RBLR4=0x%02x, RBLR5=0x%02x",
        read_fcr(ds,READ_RECEIVE_LENGTH_REG_4),
        read_fcr(ds,READ_RECEIVE_LENGTH_REG_5));
        
    printf("\n    FWR0 =0x%02x, FWR3 =0x%02x, FWR4 =0x%02x, FWR5 =0x%02x",
        read_fcr(ds, READ_FIFO_WATERMARK_REG_0),
        read_fcr(ds, READ_FIFO_WATERMARK_REG_3),
        read_fcr(ds, READ_FIFO_WATERMARK_REG_4),
        read_fcr(ds, READ_FIFO_WATERMARK_REG_5));

    printf("\n    RPR0 =0x%02x, RPR3 =0x%02x, RPR4 =0x%02x, RPR5 =0x%02x",
        read_fcr(ds, READ_RING_PARAMETER_REG_0),
        read_fcr(ds, READ_RING_PARAMETER_REG_3),
        read_fcr(ds, READ_RING_PARAMETER_REG_4),
        read_fcr(ds, READ_RING_PARAMETER_REG_5));

    printf("\n    PMPA =0x%02x, PMPB =0x%02x",
        read_fcr(ds, READ_PORT_MEMORY_PAGE_REG_A),
        read_fcr(ds, READ_PORT_MEMORY_PAGE_REG_B));

    printf("\n    PCRA =0x%02x, PCRB =0x%02x",
        read_fcr(ds, READ_PORT_CONTROL_REG_A),
        read_fcr(ds, READ_PORT_CONTROL_REG_B));

    printf("\n    RMR4 =0x%02x, RMR5 =0x%02x",
        read_fcr(ds, READ_MAXIMUM_RECEIVE_MEMORY_SPACE_REG_4),
        read_fcr(ds, READ_MAXIMUM_RECEIVE_MEMORY_SPACE_REG_5));

    printf("\n    IER  =0x%02x",read_fcr(ds, READ_INTERNAL_ERROR_STATUS_REG));
    printf("\n    REV  =0x%02x",read_fcr(ds, READ_FSI_REV_REG));
    printf("\n    IREV =0x%02x",read_fcr(ds, READ_IFDDI_REV_REG));

    printf("\n    RSR0 =0x%02x, RSR3 =0x%02x, RSR4 =0x%02x, RSR5 =0x%02x",
        read_fcr(ds, READ_RING_STATE_REG_0),
        read_fcr(ds, READ_RING_STATE_REG_3),
        read_fcr(ds, READ_RING_STATE_REG_4),
        read_fcr(ds, READ_RING_STATE_REG_5));

    /* Read the ring parameters for ring 0 */
    CMR_IS_FREE;
    ptr->ifddi[CER] = 0;
    CMR_IS_FREE;
    ptr->ifddi[CMR] = 0xbe800000;
    CMR_IS_FREE;
    regval = ptr->ifddi[CMR];
    printf("\n CMR value read for ring 0 = 0x%08x", regval);
    CMR_IS_FREE;
    regval = ptr->ifddi[CER];
    printf("\n CER value read for ring 0 = 0x%08x", regval);

    /* Read the ring parameters for ring 3 */
    CMR_IS_FREE;
    ptr->ifddi[CER] = 0;
    CMR_IS_FREE;
    ptr->ifddi[CMR] = 0xbe830000;
    CMR_IS_FREE;
    regval = ptr->ifddi[CMR];
    printf("\n CMR value read for ring 3 = 0x%08x", regval);
    CMR_IS_FREE;
    regval = ptr->ifddi[CER];
    printf("\n CER value read for ring 3 = 0x%08x", regval);

    /* Read the ring parameters for ring 4 */
    CMR_IS_FREE;
    ptr->ifddi[CER] = 0;
    CMR_IS_FREE;
    ptr->ifddi[CMR] = 0xbe840000;
    CMR_IS_FREE;
    regval = ptr->ifddi[CMR];
    printf("\n CMR value read for ring 4 = 0x%08x", regval);
    CMR_IS_FREE;
    regval = ptr->ifddi[CER];
    printf("\n CER value read for ring 4 = 0x%08x", regval);

    /* Read the ring parameters for ring 5 */
    CMR_IS_FREE;
    ptr->ifddi[CER] = 0;
    CMR_IS_FREE;
    ptr->ifddi[CMR] = 0xbe850000;
    CMR_IS_FREE;
    regval = ptr->ifddi[CMR];
    printf("\n CMR value read for ring 5 = 0x%08x", regval);
    CMR_IS_FREE;
    regval = ptr->ifddi[CER];
    printf("\n CER value read for ring 5 = 0x%08x", regval);
}

static void camel_show_regs (hwidbtype *idb)
{
    mif68840_instance *ds=INSTANCE(idb);
    volatile ulong *mac = ds->mif68840_regaddr->mac;
    volatile ulong *elma= ds->mif68840_regaddr->elma;
    volatile ulong *elmb= ds->mif68840_regaddr->elmb;
    ulong regval, trt, tvx, tmax;

    printf("\n MIF68840 internal registers");
    printf("\n  CAMEL registers");
    printf("\n    CAMEL_CNTRL=0x%04x", 
        ds->mif68840_regaddr->camel[CAMEL_CONTROL_REG] & 0xfff);
    printf("\n");
    printf("\n  MAC registers");
    printf("\n    MAC_CNTRL_A =0x%04x, MAC_CNTRL_B =0x%04x",
        mac[CONTROL_REG_A] & 0xffff, mac[CONTROL_REG_B] & 0xffff);
    printf("\n    MAC_MASK_A  =0x%04x, MAC_MASK_B =0x%04x, MAC_MASK_C =0x%04x",
        mac[INTERRUPT_MASK_REG_A] & 0xffff, 
        mac[INTERRUPT_MASK_REG_B] & 0xffff,
        mac[INTERRUPT_MASK_REG_C] & 0xfff);
    printf("\n    MAC_INTR_A  =0x%04x, MAC_INTR_B =0x%04x, MAC_INTR_C =0x%04x", 
        mac[INTERRUPT_EVENT_REG_A] & 0xffff,
        mac[INTERRUPT_EVENT_REG_B] & 0xffff,
        mac[INTERRUPT_EVENT_REG_C] & 0xffff);
    printf("\n    My Long address = 0x%04x%04x%04x", 
        mac[MY_LONG_ADDRESS_REG_C] & 0xffff,
        mac[MY_LONG_ADDRESS_REG_B] & 0xffff,
        mac[MY_LONG_ADDRESS_REG_A] & 0xffff);
    regval = mac[TREQ_REG] & 0xffff;
    trt    = regval | 0xffff0000;
    trt    = 1 + ~trt;
    trt    =  (trt << 11) / 100;
    printf("\n    TREQ        =0x%04x %u us.", regval, trt);

    regval = mac[TVX_VALUE_AND_TMAX_REG];
    tvx    = (regval & 0xff00) >> 8;
    tvx   |= 0xffffff00;
    tvx    = 1 + ~tvx;
    tvx    = (tvx << 11) / 100;
    tmax   = regval &  0xff;
    tmax  |= 0xffffff00;
    tmax   = 1 + ~tmax;
    tmax   = tmax * 5243;
    printf("\n    TVX_AND_TMAX=0x%04x   %u us. and %u us.", regval, tvx, tmax);

    printf("\n    MAC_REV_NO  =0x%04x", mac[REV_NO_REG]);
    printf("\n    T_NEG_A     =0x%04x", mac[NEGOTIATED_TTRT_A_REG]& 0xffff);
    printf("\n    T_NEG_B     =0x%04x", mac[NEGOTIATED_TTRT_B_REG]& 0xffff);
    printf("\n    Frame_Count =0x%04x,  Lost_error_count =0x%04x",
        mac[FRAME_COUNT_REG] & 0xffff,  mac[LOST_AND_ERROR_COUNT_REG]& 0xffff);
    printf("\n    Token_Count =0x%04x", mac[TOKEN_COUNT_REG] & 0xffff);
    printf("\n    MAC_RX_STS  =0x%04x,  MAC_TX_STS_REG   =0x%04x",
        mac[RECEIVE_STATUS_REG] & 0xffff, mac[TRANSMIT_STATUS_REG] & 0xffff);

    printf("\n");
    printf("\n  ELMB registers");
    printf("\n   CNTRL_A_REG  =0x%04x,  CNTRL_B_REG      =0x%04x",
        elmb[CNTRL_A_REG] & 0xffff,     elmb[CNTRL_B_REG] & 0xffff);
    printf("\n   STATUS_A_REG =0x%04x,  STATUS_B_REG     =0x%04x",
        elmb[STATUS_A_REG] & 0xffff,    elmb[STATUS_B_REG] & 0xffff);  
    printf("\n   INTR_MASK_REG=0x%04x,  INTR_EVENT_REG   =0x%04x",
        elmb[INTR_MASK_REG] & 0xffff,   elmb[INTR_EVENT_REG] & 0xffff);  
    printf("\n   XMIT_VECR_REG=0x%04x,  RCV_VECTOR_REG   =0x%04x",
        elmb[XMIT_VECTOR_REG] & 0xffff, elmb[RCV_VECTOR_REG] & 0xffff);  
    printf("\n   VEC_LEN_REG  =0x%04x", elmb[VECTOR_LENGTH_REG] & 0xffff);   
    printf("\n   CMIN_REG     =0x%04x,  TL_MIN_REG       =0x%04x",
        elmb[C_MIN_REG] & 0xffff,       elmb[TL_MIN_REG] & 0xffff);  
    printf("\n   TB_MIN_REG   =0x%04x,  T_OUT_REG        =0x%04x",
        elmb[TB_MIN_REG] & 0xffff,      elmb[T_OUT_REG] & 0xffff);  
    printf("\n   VIOL_SYM_REG =0x%04x,  LINK_ERR_CTR_REG =0x%04x",
        elmb[VIOL_SYM_CTR_REG] & 0xffff,elmb[LINK_ERR_CTR_REG] & 0xffff);  

    printf("\n");
    printf("\n  ELMA registers");
    printf("\n   CNTRL_A_REG  =0x%04x,  CNTRL_B_REG      =0x%04x",
        elma[CNTRL_A_REG] & 0xffff,     elma[CNTRL_B_REG] & 0xffff);
    printf("\n   STATUS_A_REG =0x%04x,  STATUS_B_REG     =0x%04x",
        elma[STATUS_A_REG] & 0xffff,    elma[STATUS_B_REG] & 0xffff);  
    printf("\n   INTR_MASK_REG=0x%04x,  INTR_EVENT_REG   =0x%04x",
        elma[INTR_MASK_REG] & 0xffff,   elma[INTR_EVENT_REG] & 0xffff);  
    printf("\n   XMIT_VECR_REG=0x%04x,  RCV_VECTOR_REG   =0x%04x",
        elma[XMIT_VECTOR_REG] & 0xffff, elma[RCV_VECTOR_REG] & 0xffff);  
    printf("\n   VEC_LEN_REG  =0x%04x", elma[VECTOR_LENGTH_REG] & 0xffff);   
    printf("\n   CMIN_REG     =0x%04x,  TL_MIN_REG       =0x%04x",
        elma[C_MIN_REG] & 0xffff,       elma[TL_MIN_REG] & 0xffff);  
    printf("\n   TB_MIN_REG   =0x%04x,  T_OUT_REG        =0x%04x",
        elma[TB_MIN_REG] & 0xffff,      elma[T_OUT_REG] & 0xffff);  
    printf("\n   VIOL_SYM_REG =0x%04x,  LINK_ERR_CTR_REG =0x%04x",
        elma[VIOL_SYM_CTR_REG] & 0xffff,elma[LINK_ERR_CTR_REG] & 0xffff); 
}
char *show_cam_header = "\nIndex  MAC Address      Canonical Form";
static int find_cam_entry (mif68840_regs *rp, ushort hi, ushort mid, short lo)
{
    ushort status_lo = 0;
    ushort status_hi = 0;

    CAM_DATA_WR_DIRECT (hi)
    CAM_DATA_WR_DIRECT (mid)
    CAM_DATA_WR_DIRECT (lo)

    status_lo = rp->cam[DCR];
    status_hi = rp->cam[DCR];

    if (status_lo & CAM_STATUS_MATCH_FLAG)
        return (FALSE);
    return (TRUE);
}

static void dump_cam (hwidbtype *idb)
{
    mif68840_instance *ds = INSTANCE(idb);
    mif68840_regs     *rp = ds->mif68840_regaddr;
    mac_address_type cam_data;
    ushort addr, match_address;
    boolean found = FALSE;

    if (!ds->cam_present)
        return; 
    printf("\n======= MU9C1480 CAM Information =======");
    printf("\nCAM CSR register: %x", rp->cam[CSR]);
    printf(show_cam_header);
    for (addr=0; addr < MAX_CAM_ENTRIES; addr++)
    {
        rp->cam[CSR] &= (ushort)~CAM_CSR_CAM_COMPARE_ENABLE;
        while (rp->cam[CSR] & CAM_CSR_CAM_COMPARE)
                ;

        SPS_M_DIRECT(addr);

        cam_data.u.sword[0] = rp->cam[DDR];
        cam_data.u.sword[1] = rp->cam[DDR];
        cam_data.u.sword[2] = rp->cam[DDR];
        if (find_cam_entry(rp, cam_data.u.sword[0],
            cam_data.u.sword[1], cam_data.u.sword[2])) {
            /*
             * Check if the match is at correct index.
             */
            match_address = (rp->cam[DCR] >> 1) & 0x3ff;
            if (match_address == addr)
                found = TRUE;
        } else
            break; 

        SPS_CR_DIRECT
        rp->cam[CSR] |= CAM_CSR_CAM_COMPARE_ENABLE;

        if (found) {
            printf("\n%-04d   %e", addr, &cam_data.u.sword[0]);
            ieee_swap(cam_data.u.byte, cam_data.u.byte);
            printf("   %e", &cam_data.u.sword[0]);
            found = FALSE;
         }
    }
}

static void dump_internal_cam (hwidbtype *idb)
{
    mif68840_instance *ds = INSTANCE(idb);
    mif68840_regs     *reg = ds->mif68840_regaddr;
    leveltype level;
    uint i;
    ulong regval0, regval1;
    uchar fddi[8];
    uchar ether[8];

    automore_enable(NULL);
    printf("\n\n======== Internal CAM Information ==========");
    printf("\nId  Validity      Fddi          Canonical ");
    printf("\n=============================================");

    /* Disable CAM comparison, maybe?? */

    for (i=0; i <= 0x3f; i++) {

        level = raise_interrupt_level(NETS_DISABLE);

        /* Write to generate read indication */
        CMR_IS_FREE;
        reg->ifddi[CMR] = CAM_READ_PREFIX | (i << 16);

        /* Read indication generated by write command */
        CMR_IS_FREE;
        regval0 = reg->ifddi[CMR];
        CMR_IS_FREE;
        regval1 = reg->ifddi[CER];

        reset_interrupt_level(level);

        printf("\nCMR read = 0x%08x, CER read = 0x%08x", regval0, regval1);
        bcopy((uchar *) &regval0, (uchar *)fddi, 4);
        bcopy((uchar *) &regval1, (uchar *)fddi + 4, 4);
        ieee_swap(&fddi[2], &ether[2]);

        printf("\n%2d   %s  %e  %e",  i, ((regval0 & 
            MIF68840_INTERNAL_CAM_VALID) ? "Valid" : "Invalid"),
            &fddi[2], &ether[2]);
    }
    automore_disable();

    /* Enable CAM comparison, maybe? */
}

/* 
 * mif68840_show_controller:
 * Print out info about this MIF68840 interface. Return TRUE if the
 * output terminated abnormally.
 */
static boolean mif68840_show_controller (hwidbtype *idb, parseinfo *csb)
{
    mif68840_instance *ds = INSTANCE(idb);
    mif68840_instance *ds_snap;
    leveltype level;

    printf("\nInterface %s", idb->hw_namestring);
    if (!ds) { 
        printf("\nNot initialized");
        return(FALSE);
    }
    printf("Hardware is MIF68840. Version %d.%10d", ds->hwver_hi,
            ds->hwver_lo);

    /*
     * Take a snapshot of the rx ring, rx shadow ring, tx ring, tx shadow
     */
    ds_snap = malloc(sizeof(mif68840_instance));
    if (!ds_snap) {
    return(TRUE);
    }

    /*
     * Lock out interrupts and copy the data structures. Then re-enable
     * network interrupts.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    bcopy(ds, ds_snap, sizeof(mif68840_instance));
    reset_interrupt_level(level);

    /*
     * Print out the addresses of some stuff
     */
    printf("\n idb=0x%08x, ds=0x%08x, device_base=0x%08x", 
       idb, ds, ds->mif68840_regaddr);
    printf("\n rx_llc=%d rx_smt=%d, tx=%d, llc_inputs=%d, smt_inputs=%d",
        MIF68840_RX_LLC_RING_ENTRIES, MIF68840_RX_SMT_RING_ENTRIES,
        MIF68840_TX_RING_ENTRIES, ds->llc_inputs, ds->smt_inputs);
    printf("\n");
    printf("\n rxr_llc_malloc=0x%08x, rxr_llc=0x%08x, rxr_llc_shadow=0x%08x",
        ds_snap->rxr_llc_malloc, ds_snap->rxr_llc, ds_snap->rxr_llc_shadow);
    printf("\n rx_llc_head=%d, rx_llc_tail=%d", ds_snap->rx_llc_head, ds_snap->rx_llc_tail);
    printf("\n rxr_smt_malloc=0x%08x, rxr_smt=0x%08x, rxr_smt_shadow=0x%08x",
        ds_snap->rxr_smt_malloc, ds_snap->rxr_smt, ds_snap->rxr_smt_shadow);
    printf("\n rx_smt_head=%d, rx_smt_tail=%d", ds_snap->rx_smt_head, ds_snap->rx_smt_tail);
    printf("\n txr_malloc=0x%08x, txr=0x%08x, txr_shadow=0x%08x",
       ds_snap->txr_malloc, ds_snap->txr, ds_snap->txr_shadow);
    printf("\n tx_head=%d, tx_tail=%d, tx_count=%d",
        ds_snap->tx_head, ds_snap->tx_tail, ds_snap->tx_count);
    printf("\n bcn_rd_ptr   = 0x%08x", ds->bcn_rd_ptr);
    printf("\n bcn_wr_ptr   = 0x%08x", ds->bcn_wr_ptr);
    printf("\n bcn_ptr      = 0x%08x", ds->bcn_ptr);
    printf("\n");
    printf("\n overruns=%d, rx_macif_parity_errors=%d, rx_port_errors=%d",
        ds->rx_overrun_errors,ds->rx_macif_parity_errors,ds->rx_port_errors);
    printf("\n np_errs=%d, ebuf_errs=%d, throttled=%d",
        ds->np_err, ds->ebuf_err, ds->throttled);
    printf("\n rx_no_llc_ep=%d, rx_no_smt_ep=%d, rxovr_llc=%d, rxovr_smt=%d",
        ds->rx_no_llc_ep,ds->rx_no_smt_ep,ds->rxovr_llc_cnt,ds->rxovr_smt_cnt);
    printf("\n LateTokens = %d, rx_llc_mis = %d, rx_smt_mis = %d",
        MIBDATA(idb->fddi_stn_id, mib->macBase[0].hwData.Late_Ct),
        ds->rx_llc_mis, ds->rx_smt_mis);

    /*
     * Print the ifddi registers
     */
    ifddi_show_regs(idb);
    printf("\n");

    /* Print the MAC/ELM registers  */
    camel_show_regs(idb);
    printf("\n");

    /*
     * Print out pci registers
     */
    mif68840_print_pci_regs(idb);

    /*
     * Print out all the valid entires in external CAM
     */
    dump_cam(idb);

    /*
     * Print out internal CAM contents 
     */
    dump_internal_cam(idb);

    /*
     * Release all memory
     */
    free(ds_snap);   
    return(FALSE);   
}

/*
 * Send Connect action message to PCM.
 */
void cmt_connect (hwidbtype *idb, int which)
{ 
    leveltype SR;

    if (which & FDDI_PHY_A) {
        SendSignalMulti (idb->fddi_stn_id, SIG_PC_Enable, 0, (uInt32) 0);
        SendSignalMulti (idb->fddi_stn_id, SIG_PC_Start, 0, (uInt32) 0);
    }
    if (which & FDDI_PHY_B) {
        SendSignalMulti (idb->fddi_stn_id, SIG_PC_Enable, 1, (uInt32) 0);
        SendSignalMulti (idb->fddi_stn_id, SIG_PC_Start, 1, (uInt32) 0);
    }
    
    /* Process signals */
    if (!EmptySignalQueue ()) {
	MDisableCSPInterrupts(&SR);
	ProcessCSP (&SR);
	MRestoreCSPInterrupts(&SR);
    }
}

/*
 * Send Disconnect action message to ECM.
 */
void cmt_disconnect (hwidbtype *idb, int which)
{
    leveltype	SR;

    if (which & FDDI_PHY_A) {
        SendSignalMulti(idb->fddi_stn_id, SIG_PC_Disable, 0, (uInt32) 0);
    }
    if (which & FDDI_PHY_B) {
        SendSignalMulti(idb->fddi_stn_id, SIG_PC_Disable, 1, (uInt32) 0);
    }

    /* Process signals */
    if (!EmptySignalQueue()) {
	MDisableCSPInterrupts(&SR);
	ProcessCSP (&SR);
	MRestoreCSPInterrupts(&SR);
    }
}

/********************************************************************/
/* Core routines begin */

/*
 * mif68840_throttle:
 * The interface is throttled by stopping the Receive side.
 * Whenever the reason for throttling is the lack of receive
 * minibuffers, the pool threshold callback function is responsible
 * for re-enabling the interface, so that it doesn't have to
 * wait for the background process to wake it up after 10 seconds.
 * CALLBACK FUNCTION TO BE IMPLEMENTED
 */
/*
 * mif68840_throttle:
 * Throttle the MIF68840 interface
 */
void mif68840_throttle (hwidbtype *idb)
{
    mif68840_instance *ds = INSTANCE(idb);
    
    /*
     * Mark the interface as throttled
     */
    idb_mark_throttled(idb);
    ds->throttled++;
        
    /*
     * Stop the Rx process
     */
    ds->mif68840_regaddr->ifddi[FCR] = WRITE_MACIF_RECEIVE_EN_RE5;
}

/*  
 * mif68840_enable:
 * Enable a MIF68880 interface. This function "unthrottles"
 * the interface.
 */
static void mif68840_enable (hwidbtype *idb)
{
    mif68840_instance *ds = INSTANCE(idb);
 
    /*
     * Mark the interface as enabled (NOT throttled)
     */
    idb_mark_unthrottled(idb);
  
    /*
     * Reenable the Rx side
     */
    ds->mif68840_regaddr->ifddi[FCR] = WRITE_MACIF_RECEIVE_EN_ALL;
}  
 
/*   
 * mif68840_cache_threshold
 *
 * Callback routine from the buffer management code. We get a callback
 * when the interface private pool reaches our threshold value.
 * This function will reenable the receive side if it has been disabled.
 */  
void mif68840_cache_threshold (pooltype *pool)
{
    hwidbtype *idb = (hwidbtype *)pool->hwidb;
 
    if (idb_is_throttled(idb))
        mif68840_enable(idb);
}
 
/*
 * mif68840_allocate_rings:  
 * Create and initialize the transmit and LLC and SMT receive ring structures. 
 */
     
static boolean mif68840_allocate_rings (mif68840_instance  *ds)  
{
    ulong sizeof_ring, tmp;

    /* In the stupid Motorola chipset, the region of the address space
     * for the ring does not necessarily begin with the ring pointer.
     * Rather the region is defined such that the most significant bits
     * of the pointer remain constant. eg. ring pointer = 3445 4ff8 and
     * and a RML = 128 bytes (0100) provides an address range of 
     * 3445 4f80 to 3445 4ffc 
     * To circumvent this problem, I do the following.
     * pci_malloc twice the memory.  Let the pointer be x.
     * I need to find the smallest number above x that is a multiple of
     * 256 * 8 (0x800) for rx_llc.  (int)(x/0x800) * 0x800 + 0x800 is
     * answer.
     */
    
    /*
     * Allocate rx llc ring and rx llc ring shadow
     */
    sizeof_ring = sizeof(mif68840_rx_desc) * MIF68840_RX_LLC_RING_ENTRIES;

    ds->rxr_llc_malloc = malloc_pcimem(sizeof_ring * 2);
    if (!ds->rxr_llc_malloc) {
    return(FALSE);
    }
    tmp = (ulong)ds->rxr_llc_malloc / sizeof_ring;
    tmp = tmp * sizeof_ring + sizeof_ring;
    ds->rxr_llc = (mif68840_rx_desc *) tmp;
    ds->rxr_llc  = pci_memaddr_convert(ds->rxr_llc, MIF68840_RX_RING_ATTRIBUTE);
    ds->rxr_llc_shadow = malloc_fast(sizeof(particletype *) *
                 MIF68840_RX_LLC_RING_ENTRIES);
    if (!ds->rxr_llc_shadow) {
    return(FALSE);
    }
    mif68840_rxring_flush(ds);

    /*
     * Allocate rx smt ring and rx smt ring shadow
     */
    sizeof_ring = sizeof(mif68840_rx_desc) * MIF68840_RX_SMT_RING_ENTRIES;
    ds->rxr_smt_malloc = malloc_pcimem(sizeof_ring * 2);
    if (!ds->rxr_smt_malloc) {
    return(FALSE);
    }
    tmp = (ulong)ds->rxr_smt_malloc / sizeof_ring;
    tmp = tmp * sizeof_ring + sizeof_ring;
    ds->rxr_smt = (mif68840_rx_desc *) tmp;
    ds->rxr_smt  = pci_memaddr_convert(ds->rxr_smt, MIF68840_RX_RING_ATTRIBUTE);
    ds->rxr_smt_shadow = malloc_fast(sizeof(particletype *) *
        MIF68840_RX_SMT_RING_ENTRIES);
    if (!ds->rxr_smt_shadow) {
    return(FALSE);
    }
    mif68840_rxring_flush(ds);

    /*
     * Allocate tx ring and tx ring shadow
     */
    sizeof_ring = sizeof(mif68840_tx_desc) * MIF68840_TX_RING_ENTRIES;
    ds->txr_malloc = malloc_pcimem(sizeof_ring * 2);
    if (!ds->txr_malloc) {
    return(FALSE);
    }
    tmp = (ulong)ds->txr_malloc / sizeof_ring;
    tmp = tmp * sizeof_ring + sizeof_ring;
    ds->txr = (mif68840_tx_desc *) tmp;
    ds->txr_shadow = malloc_fast(sizeof(mif68840_tx_shadow) *
                 MIF68840_TX_RING_ENTRIES);
    if (!ds->txr_shadow) {
    return(FALSE);
    }

    /* 
     * Allocate beacon ring pointers (read and write).
     */
    sizeof_ring = sizeof(mif68840_tx_desc);
    ds->bcn_malloc = malloc_pcimem(sizeof_ring * 3);
    if (!ds->bcn_malloc) {
    return(FALSE);
    }
    tmp = (ulong) ds->bcn_malloc / sizeof_ring;
    tmp = tmp * sizeof_ring + sizeof_ring;
    ds->bcn_rd_ptr= (mif68840_tx_desc *) tmp;
    ds->bcn_wr_ptr= ds->bcn_rd_ptr + 1;

    return(TRUE);
}

void fddi_interface (hwidbtype *idb)
{
    uchar una[6],dna[6];  /* bit swapped upstream & downstream nayba */ 
    char *pcstate, *pcnbr;
    uInt16 ls;
    uInt16 stnID = idb->fddi_stn_id;
    fddisb             *fsb;
    srb_triplet_t       srb_triplet;
    srb_vector_table_t *srbV_p;

    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srbV_p->srb_sb_get_triplet(idb->firstsw, &srb_triplet);
    } else {
        srb_triplet.localRing  = SRB_INVALID_RING_NUM;
        srb_triplet.bridgeNum  = SRB_INVALID_BRIDGE_NUM;
        srb_triplet.remoteRing = SRB_INVALID_RING_NUM;
    }
    if (srb_triplet.bridgeNum <= SRB_MAX_BRIDGE_NUM) {
        printf("\n  Source bridging enabled, srn %d bn %d trn %d %s",
           srb_triplet.localRing,
           srb_triplet.bridgeNum,
           srb_triplet.remoteRing,
           (idb->tr_vring_blk ? "(ring group)" : ""));
        printf("\n    spanning explorer %sabled",
            (srbV_p && srbV_p->srb_sb_get_spanning_explorer(idb->firstsw))
                ? "en" : "dis");
    }
    
    pcstate = get_pcm_state_string (CSPDATA (stnID, portData[0].pcState));
    pcnbr = get_phy_type_string (CSPDATA (stnID, portData[0].PC_Neighbor));
    ls =  ReadLineStateMulti(stnID, 0);
    printf("\n  Phy-A state is %s, neighbor is %s, status %3s",
       pcstate, pcnbr, get_ls_string(ls));

    pcstate = get_pcm_state_string (CSPDATA (stnID, portData[1].pcState));
    pcnbr = get_phy_type_string (CSPDATA (stnID, portData[1].PC_Neighbor));
    ls =  ReadLineStateMulti(stnID, 1);
    printf("\n  Phy-B state is %s, neighbor is %s, status %3s",
       pcstate, pcnbr, get_ls_string(ls));
       
    printf("\n  ECM is %s, CFM is %s, RMT is %s,",
           get_smt_string(&SMTMsg.ec_state,
              MIBDATA (stnID, mib->smtData.ECMState)),
           get_smt_string(&SMTMsg.cf_state,
              MIBDATA (stnID, mib->smtData.CF_State)),
           get_smt_string(&SMTMsg.rm_state,
              MIBDATA (stnID, mib->macBase[0].hwData.RMTState)));

    printf("\n  Requested token rotation %u usec, negotiated %u usec",
       fsb->fddi_trt, fsb->fddi_trt_neg);
    printf("\n  Configured tvx is %u usec", fsb->fddi_tvx);

    if (!TIMER_RUNNING(fsb->fddi_operational))
    printf(" ring not operational");
    else {
    printf(" ring operational ");
    print_dhms(fsb->fddi_operational);
    }
    ieee_swap(MIBDATA (stnID, mib->macBase[0].hwData.UpstreamNbr),una);
    ieee_swap(MIBDATA (stnID, mib->macBase[0].hwData.DownstreamNbr),dna);
    printf("\n  Upstream neighbor %e, downstream neighbor %e", una, dna);
    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);
}


static void mif68840_stop (hwidbtype *idb)
{
    mif68840_instance *ds=INSTANCE(idb);

    if (fddi_debug | fddi_cmt_events_debug)
        buginf("\nIn mif68840_stop");

    /* Reset the IFDDI */
    FCR_IS_FREE;
    ds->mif68840_regaddr->ifddi[FCR] = SOFTWARE_RESET;
    DELAY(50);
}

static void mif68840_config_cam (mif68840_regs *rp)
{
    rp->cam[CSR] = 0; /* CAM_CSR_FIFO_CLEAR; */

    /* Power-up anomalie */
    SPS_CR_DIRECT

    /* Device select */
    TCO_DS_DIRECT
    CAM_COMMAND_DIRECT(0)

    /* Reset the CAM */
    TCO_CT_DIRECT
    CAM_COMMAND_DIRECT(0)

    /* Reset the FIFO */
    rp->cam[CSR] = CAM_CSR_FIFO_CLEAR;

    TCO_PA_DIRECT
    CAM_COMMAND_DIRECT(0)

    TCO_CT_DIRECT
    CAM_COMMAND_DIRECT(CAM_48_RAM_16 &
            CAM_ENABLE_MATCH_FLAG &
            CAM_ENABLE_FULL_FLAG &
            CAM_INPUT_NOT_SCRAMBLED &
            CAM_COMPARE_MASK_DISABLE &
            CAM_ENABLE_ADDRESS_INC);

    TCO_SC_DIRECT
    CAM_COMMAND_DIRECT(CAM_DST_CNTR_START01_END11 &
            CAM_SRC_CNTR_START01_END11 &
            CAM_LOAD_DST_SEG_CNTR_01 &
            CAM_LOAD_SRC_SEG_CNTR_01);

    rp->cam[CSR] |= CAM_CSR_CAM_COMPARE_ENABLE;
}

static boolean check_hw_cam (hwidbtype *idb)
{
    dgbus_eeprom_t *eptr;
    mif68840_instance *ds = INSTANCE(idb);
    int bay = idb->slot;
    uchar hwver_hi, hwver_lo;
    boolean support = FALSE;

    eptr = malloc(sizeof(dgbus_eeprom_t));
    if (!eptr) {
        printf("%%Error: No memory to allocate dgbus_eeprom_t\n");
        return(FALSE);
    }
    pas_get_eeprom_data(bay, eptr, 16);
    hwver_hi = eptr->hardware_version_high;
    hwver_lo = eptr->hardware_version_low;
    ds->hwver_hi = hwver_hi;
    ds->hwver_lo = hwver_lo;
    support = (((hwver_lo >= FAB03_WITH_CAM_SUPPORT) ||
              (hwver_lo >= FAB02_WITH_CAM_SUPPORT &&
               hwver_lo < FAB03_BASE)) ? TRUE : FALSE);
    free(eptr);
    return(support);
}

/*
 * mif68840_chip_startup:
 * Move the mif68840 from stopped to running state
 */
static void mif68840_chip_startup (hwidbtype *idb)
{
    mif68840_instance *ds = INSTANCE(idb);
    mif68840_regs *reg;
    ulong regval;
    ushort *sptr;

    reg = ds->mif68840_regaddr;   

    /* CAMEL initialization */
    reg->camel[CAMEL_CONTROL_REG] = BYPASS_CTRL_ON;

    /* MAC initialization */
    /* In addition to the stuff that the XDI code does */
    regval = reg->mac[INTERRUPT_EVENT_REG_A];
    regval = reg->mac[INTERRUPT_EVENT_REG_B];
    regval = reg->mac[INTERRUPT_EVENT_REG_C];
    
    sptr = (ushort *) idb->bit_swapped_hardware;
    /* write the station address */
    reg->mac[MY_LONG_ADDRESS_REG_C] = GETSHORT(sptr++);
    reg->mac[MY_LONG_ADDRESS_REG_B] = GETSHORT(sptr++);
    reg->mac[MY_LONG_ADDRESS_REG_A] = GETSHORT(sptr);

    reg->mac[CONTROL_REG_A] = CANONICAL_FORM | 
        (ds->bridge ? COPY_IND_LLC : 0) | 
        COPY_GRP_LLC | DISABLE_BRDCST;

    reg->mac[CONTROL_REG_B] =  BRIDGE_STRIP;

    /* FSI Initialization */
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_PORT_CONTROL_EN_REG_A;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_PORT_MEMORY_PAGE_REG_A;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_RING_PARAMETER_REG_0;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_RING_PARAMETER_REG_3;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_RING_PARAMETER_REG_4;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_RING_PARAMETER_REG_5;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_FIFO_WATERMARK_REG_0;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_FIFO_WATERMARK_REG_3;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_FIFO_WATERMARK_REG_4;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_FIFO_WATERMARK_REG_5;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_RFR_4;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_RFR_5;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_MAXIMUM_RECEIVE_MEMORY_SPACE_REG_4;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_MAXIMUM_RECEIVE_MEMORY_SPACE_REG_5;
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_BURST_LIMIT_REG_A;

    /* Define receive ring 4 (LLC) */
    CMR_IS_FREE;
    reg->ifddi[CER] = (ulong) ds->rxr_llc;
    CMR_IS_FREE;
    regval = (ulong)ds->rxr_llc & 0xffff;
    reg->ifddi[CMR] = 0xbe0c0000 | regval;

    /* Define receive ring 5 (SMT) */
    CMR_IS_FREE;
    reg->ifddi[CER] = (ulong) ds->rxr_smt;
    CMR_IS_FREE;
    regval = (ulong) ds->rxr_smt & 0xffff;
    reg->ifddi[CMR] = 0xbe0d0000 | regval;

    /* Enable reception for receive channels 4 and 5 */
    FCR_IS_FREE;
    reg->ifddi[FCR] = WRITE_MACIF_RECEIVE_EN_ALL;

    /* Define transmit ring (beacon) 0 */
    CMR_IS_FREE;
    reg->ifddi[CER] = (ulong) ds->bcn_wr_ptr;
    regval = (ulong)ds->bcn_rd_ptr & 0xffff;
    reg->ifddi[CMR] = 0xbe000000 | regval;

    /* Define transmit ring 3 */
    CMR_IS_FREE;
    reg->ifddi[CER] = (ulong) ds->txr;
    CMR_IS_FREE;
    regval = (ulong) ds->txr & 0xffff;
    reg->ifddi[CMR] = 0xbe030000 | regval;

    FCR_IS_FREE;
    reg->ifddi[FCR] =  WRITE_MACIF_TRANSMIT_EN_3;

    /*
     * Enable interrupts for LLC ring(4), SMT ring(5), 
     * Transmit ring(3) and ErrorInterrupts.
     */
    reg->ifddi[IMR1] = RECEIVE_COMPLETE_5 | RECEIVE_COMPLETE_4 |
        RING_CMD_COMPLETE_3 | ERROR_NO_SMT_FRAME;
    /*
     * Initialize the MU9C1480 LANCAM
     */
    if (ds->cam_present)
        mif68840_config_cam(ds->mif68840_regaddr);

    return;
}

static boolean mif68840_init_rx_llc_ring (hwidbtype *idb)
{
    int i;
    mif68840_instance *ds=INSTANCE(idb);
    particletype *mp;
    mif68840_rx_desc *rmd;

    /* Initialize variables */
    ds->rx_llc_head = ds->rx_llc_tail = 0;  /* Rx head, tail indexes */

    /* 
     * Init the rx llc ring with particles owned by the MIF68840
     */
    for (i=0; i<MIF68840_RX_LLC_RING_ENTRIES; i++) {

        rmd = &ds->rxr_llc[i];

        if (ds->rxr_llc_shadow[i]) {

            /*
             * There is already a particle on the shadow rx ring.
             * Give it to the MIF68840.
             */
            mp = ds->rxr_llc_shadow[i];
		    rmd->rx_status_length = MIF68840_RX_STATUS_LENGTH_DEFAULT;
            rmd->rx_buffer_addr = MIF68840_BUF_ADDR(mp->data_start);
        } else {
            /*
             * There is no particle on the shadow rx ring, so
             * allocate it and give it to the MIF68840
             */
            mp = mif68840_get_particle_inline(idb, TRUE);
            if (mp) {
                ds->rxr_llc_shadow[i] = mp;
		        rmd->rx_status_length = MIF68840_RX_STATUS_LENGTH_DEFAULT;
                rmd->rx_buffer_addr = MIF68840_BUF_ADDR(mp->data_start);
            } else {
                net_cstate(idb, IDBS_DOWN);
                mif68840_lineproto_change(idb, FALSE);
                return(FALSE);
            }
        }
    }
    mif68840_rxring_flush(ds);
    return(TRUE);
}

static boolean mif68840_init_rx_smt_ring (hwidbtype *idb)
{
    int i;
    mif68840_instance *ds=INSTANCE(idb);
    particletype *mp;
    mif68840_rx_desc *rmd;

    /* Initialize variables */
    ds->rx_smt_head = ds->rx_smt_tail = 0;  /* Rx head, tail indexes */

    /* 
     * Init the rx smt ring with particles owned by the MIF68840
     */
    for (i=0; i < MIF68840_RX_SMT_RING_ENTRIES; i++) {
 
        rmd = &ds->rxr_smt[i];

        if (ds->rxr_smt_shadow[i]) {
            /*
             * There is already a particle on the shadow rx ring.
             * Give it to the MIF68840.
             */
            mp = ds->rxr_smt_shadow[i];
		    rmd->rx_status_length = MIF68840_RX_STATUS_LENGTH_DEFAULT;
            rmd->rx_buffer_addr = MIF68840_BUF_ADDR(mp->data_start);
        } else {

            /*
             * There is no particle on the shadow rx ring, so
             * allocate it and give it to the MIF68840
             */
            mp = mif68840_get_particle_inline(idb, TRUE);
            if (mp) {
                ds->rxr_smt_shadow[i] = mp;
		        rmd->rx_status_length = MIF68840_RX_STATUS_LENGTH_DEFAULT;
                rmd->rx_buffer_addr = MIF68840_BUF_ADDR(mp->data_start);
            } else {
                net_cstate(idb, IDBS_DOWN);
                mif68840_lineproto_change(idb, FALSE);
                return(FALSE);
            }
        }
    }
    mif68840_rxring_flush(ds);
    return(TRUE);
}

/*
 * mif68840_reset:
 */
static void mif68840_reset (hwidbtype *idb)
{
    mif68840_instance *ds=INSTANCE(idb);
    mif68840_tx_desc *tmd;
    leveltype level;
    int i;
    fddisb             *fsb;
    srb_vector_table_t *srbV_p;

    if (fddi_debug | fddi_cmt_events_debug) {
        buginf("\n%s: In mif68840_reset", idb->hw_namestring);
    }

    idb->counters.resets++;
    idb->status &= ~IDB_DISABLED;

    reg_invoke_bump_cache_version(idb, NULL, TRUE);
    reg_invoke_media_rif_reset(idb);

    fddi_disconnect(idb->fddi_stn_id);

    /* Shut down the chip  */
    mif68840_stop(idb);

    level = raise_interrupt_level(NETS_DISABLE);

    /* 
     * Flush hold queue -- ring down -- no Upstream or Downstream neighbors.
     */
    holdq_reset(idb);

    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);
    TIMER_STOP(fsb->fddi_operational);
    TIMER_STOP(idb->xmittimer);
    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);

    /*
     * Initialize the hardware addresses. 
     */
    mif68840_get_default_hw_address(idb,idb->bia);
    if (is_ieee_zero(idb->hardware)) {
    mif68840_get_default_hw_address(idb,idb->hardware);
    }

    /* Bit swap our canonical address to form the actual address. */
    ieee_swap(idb->hardware, idb->bit_swapped_hardware);

    /*
     * Adjust number of SMT buffers to respond to possible card insertion
     */
    adjust_smt_bufs();
    
    /*
     * Report new address to the FDDI SMT MIB
     */
    {
    SMTMessage msg;
 
    msg.destination = MIB_MSG_ID;
    msg.source = MAP_MSG_ID;
    msg.type = MIB_ACTION_CHANGE_MACADDR;
    msg.stnID = idb->fddi_stn_id;
    msg.entity = 0;
    MCopyAddress (msg.data.addr, &idb->bit_swapped_hardware);
 
    SendMAPMessage(&msg);
    }

    /* Bare minimum port operation is required even in down condition */
    FCR_IS_FREE;
    ds->mif68840_regaddr->ifddi[FCR] = WRITE_PORT_CONTROL_EN_REG_A;
     
   /*
    * If interface is administratively down, don't re-enable
    */
    if (idb->state == IDBS_ADMINDOWN) {
        BypassRequestMulti (idb->fddi_stn_id, DEINSERTED);
        mif68840_lineproto_change(idb, TRUE);
        reset_interrupt_level(level);
        return;
    }

    ds->tx_limited = hwidb_use_short_tx_q_inline(idb);
    if (ds->tx_limited) {
        idb->mci_txcount = 1;
    } else {
        idb->mci_txcount = MIF68840_TX_RING_ENTRIES;
    }

    /* Initialize variables */
    ds->rx_llc_head = ds->rx_llc_tail = 0;  /* Rx head, tail indexes */
    ds->rx_smt_head = ds->rx_smt_tail = 0;  /* Rx head, tail indexes */
    ds->tx_head = ds->tx_tail = 0;          /* Tx head, tail indexes */
    ds->tx_count = 0;                       /* no particles on Tx ring */
    ds->llc_inputs = ds->smt_inputs = 0;    /* no llc/smt inputs */

    /* 
     * Init the rx llc ring with particles owned by the MIF68840
     */
    if (!mif68840_init_rx_llc_ring(idb)) {
        reset_interrupt_level(level);
        return;
    }

    /* 
     * Init the rx smt ring with particles owned by the MIF68840
     */
    if (!mif68840_init_rx_smt_ring(idb)) {
        reset_interrupt_level(level);
        return;
    }

    /* 
     * Setup the tx ring and ensure that the CPU owns
     * all the buffers and/or particles
     */
    for (i=0; i < MIF68840_TX_RING_ENTRIES; i++) {
    
        tmd = &ds->txr[i];
    
        tmd->tx_status_length = 0;
        tmd->tx_buffer_addr = 0;
        if (ds->txr_shadow[i].mp) {
            retparticle_inline(ds->txr_shadow[i].mp);
        }
        ds->txr_shadow[i].pak = NULL;
        ds->txr_shadow[i].mp = NULL;
    }

    /* Setup the beacon ring */
    ds->bcn_rd_ptr->tx_status_length = 0;
    ds->bcn_rd_ptr->tx_buffer_addr = 0;

    /* FIXME */
    /* Bridging shit */
    /*
     * Turn on bridging on hwidb if any swidb has tbridging
     * or SRB configured. In other words go promiscuous. 
     */   
    if (MIF68840_BRIDGING_CHECK) {
        idbtype *swidb;
        boolean srbridge_on_hwidb = FALSE;

        /*
         * Determine whether SRB is configured on the interface.
         */
        FOR_ALL_SWIDBS_ON_HW(idb, swidb)
        if (RUNNING_IF(LINK_SRB, swidb)) {
            srbridge_on_hwidb = TRUE;
            break;
        }
        if (bridge_enable && (idb->tbridge_on_hwidb || srbridge_on_hwidb))
            ds->bridge = TRUE;
        else 
            ds->bridge = FALSE;

        /*
         * recalc srb params
         */
        srbV_p = reg_invoke_srb_get_vector_table();
        if (srbV_p) {
            srbV_p->srb_sb_init_rng_br_rng_mask(idb->firstsw);
        }
    }

    reg_invoke_tbridge_smf_update(idb->hardware,
                  idb->firstsw,
                  SMF_MACTYP_OWN_UNICAST |
                  SMF_ACTION_RECEIVE);
    reg_invoke_media_interesting_addresses(idb->firstsw);

    mif68840_chip_startup(idb); 

    reset_interrupt_level(level);


    BypassRequestMulti (idb->fddi_stn_id, INSERTED);

    init_cmt (idb);
}

static void mif68840_shutdown (hwidbtype *idb, int newstate)
{
    if (fddi_debug) {
        buginf("\nIn mif68840_shutdown");
    }
    idb->state = newstate;
    (*idb->reset)(idb);
}

/*   
 * mif68840_add_multicast_address
 *
 * Adds a multicast address to the enabled table for this interface.
 * Calls the necessary routines to manipulate the internal and external
 * cams for this address.
 * Algorithm: 
 * We always operate in promiscuous multicast mode.
 * Only address we'll need to add is the HSRP address.
 */
static void mif68840_add_multicast_address (idbtype *swidb, uchar *address, 
                                                uchar fsm)
{ 
    hwidbtype           *idb = swidb->hwptr;
    mif68840_instance   *ds = INSTANCE(idb);
    leveltype           level;
    ulong               cam_input_low=0, cam_input_high=0, regval;
    uint                i;
    uchar               fddi[6];

    /* 
     * Since we always operate in promiscuous multicast, we check and 
     * remove multicast/broadcast addresses here. 
     */
    if (*address & 0x01)
        return;

    if (fddi_debug) {
        buginf("\nIn mif68840_add_multicast_address, %s", idb->hw_namestring);
        buginf("\nAddress to be added is %e", address);
    }

    ieee_swap(address, fddi);
    bcopy(&fddi[2], (uchar *)&cam_input_low, 4);
    bcopy(&fddi[0], (uchar *)&cam_input_high + 2, 2);

    level = raise_interrupt_level(NETS_DISABLE);

    /* Write Compare CAM entry */
    CMR_IS_FREE;
    ds->mif68840_regaddr->ifddi[CER] = (ulong)cam_input_low;
    CMR_IS_FREE;
    ds->mif68840_regaddr->ifddi[CMR] = (ulong)cam_input_high | CAM_CMP_PREFIX;

    /* Read Compare CAM indication */
    CMR_IS_FREE;
    regval = ds->mif68840_regaddr->ifddi[CMR];

    /* Entry is already in CAM, return */
    if (regval & MIF68840_INTERNAL_CAM_MATCH) {
        buginf("\n Entry already in CAM. Return.");
        reset_interrupt_level(level);
        return;
    }

    /* Find first available CAM location */
    for (i=0 ; i <= 0x3f; i++) {

        /* Write READ CAM entry */
        CMR_IS_FREE;
        ds->mif68840_regaddr->ifddi[CMR] = CAM_READ_PREFIX | (i << 16);

        /* READ READ_CAM Indication */
        CMR_IS_FREE;
        regval = ds->mif68840_regaddr->ifddi[CMR];

        if (regval & MIF68840_INTERNAL_CAM_VALID)
            continue;

        buginf("\nEntry not in CAM. Adding entry to location %d", i);

        /* Add CAM entry */
        CMR_IS_FREE;
        ds->mif68840_regaddr->ifddi[CER] = (ulong)cam_input_low;

        buginf("\nCER written = 0x%08x", (ulong)cam_input_low);

        CMR_IS_FREE;
        ds->mif68840_regaddr->ifddi[CMR] = (ulong)cam_input_high | 
            (CAM_ADD_PREFIX | (i << 16));

        buginf("\nCMR written = 0x%08x", (ulong)cam_input_high |
            (CAM_ADD_PREFIX | (i << 16)));

        break;
    }
    reset_interrupt_level(level);

    if (i == 0x40)
        buginf("\nInternal CAM Full. Address %e not added", address);
}

/*
 * mif68840_add_multicast_range
 * Adds a multicast address range to the enabled table for this interface.
 *
 * This call will kick the CAM into a promiscuous multicast mode, which means
 * that'll see most multicasts on the network.
 */

static void mif68840_add_multicast_range (idbtype *swidb, uchar *address, uchar *mask)
{
    hwidbtype           *idb = swidb->hwptr;
    mif68840_instance   *ds = INSTANCE(idb);
    leveltype           level;
    register  ulong     regval;
    
    level = raise_interrupt_level(NETS_DISABLE);

    if (fddi_debug) {
        buginf("\nmif68840_add_multicast_range(): idb=%#x\n", idb);
        buginf("\nAddress added is %e", address);
    }

    regval = ds->mif68840_regaddr->mac[CONTROL_REG_A];
    regval |= COPY_GRP_LLC;
    ds->mif68840_regaddr->mac[CONTROL_REG_A] = regval;

    reset_interrupt_level(level);
}

static void mif68840_periodic (hwidbtype *idb)
{
    mif68840_instance *ds = INSTANCE(idb);
    mif68840_regs  *ptr = ds->mif68840_regaddr;
    ulong regval, sr4;
    leveltype         level;

    /*
     * If Overrun flag for ring4 is set, unset it and enable the ignore
     * interrupt.
     */
    if (ds->rxovr_4) {
        ds->rxovr_4 = FALSE;
        ds->mif68840_regaddr->mac[INTERRUPT_MASK_REG_B] = NOT_COPIED;
    }

    if (idb->state != IDBS_ADMINDOWN) {
        sr4 = read_fcr(ds, READ_RING_STATE_REG_4);
        if (sr4 == 0x83) {
            level = raise_interrupt_level(NETS_DISABLE);
 
            /* Initialize ring 4 (LLC) from processor's view */
            mif68840_init_rx_llc_ring(idb);
 
            /* Initialize ring 4 (LLC) from chips view. */
            CMR_IS_FREE;
            ptr->ifddi[CER] = (ulong) ds->rxr_llc;
            CMR_IS_FREE;
            regval = (ulong)ds->rxr_llc & 0xffff;
            ptr->ifddi[CMR] = 0xbe0c0000 | regval;
 
            /* Increment rxovr_llc_cnt counter */
            ds->rxovr_llc_cnt++;
            reset_interrupt_level(level);
        }
    }
}

static boolean mif68840_linestate (hwidbtype *hwidb)
{
    return(TRUE);
}

static int mif68840_txqlength (hwidbtype *idb)
{
   mif68840_instance *ds = INSTANCE(idb);

    /*
     * Note: we are not able to support this exactly, as the tx_count
     * field is the number of particle buffers queued on the tx ring rather
     * than the number of packets. But this should be close enough.
     */
    return(ds->tx_count);
}

/* 
 * mif68840_fddi_cam_depopulate:
 * Remove mac address from external CAM.
 */
void mif68840_fddi_cam_depopulate (hwidbtype *hwidb,
					  uchar *addr,
					  hwidbtype *input_hwidb)
{
    mif68840_instance *ds = INSTANCE(hwidb);
    mif68840_regs     *rp = ds->mif68840_regaddr;
    uchar baddr[IEEEBYTES];

    if (!ds->cam_present)
        return;

    rp->cam[CSR] &= ~CAM_CSR_SA_MISS_INTR_EN;
    if (rp->cam[CSR] & CAM_CSR_FIFO_GO) {
        usecdelay(2);
    }
    if (rp->cam[CSR] & CAM_CSR_FIFO_GO) {
        buginf ("\nDepopulate : CAM entry access timed out\n");
        rp->cam[CSR] |= CAM_CSR_SA_MISS_INTR_EN;
        return;
    }

    ieee_swap(addr, baddr);
    SPD_CR
    CAM_DATA_WR(GETSHORT(baddr))
    CAM_DATA_WR(GETSHORT(baddr+2))
    CAM_DATA_WR(GETSHORT(baddr+4))
    VBC_M_ALM_E
    /* Set FIFO GO bit */
    rp->cam[CSR] |= CAM_CSR_FIFO_GO;
    while (rp->cam[CSR] & CAM_CSR_FIFO_GO)
        ;
    rp->cam[CSR] |= CAM_CSR_SA_MISS_INTR_EN;
}

/*
 * mif68840_fddi_cam_populate:
 * Add MAC address to external CAM.
 */
void mif68840_fddi_cam_populate (hwidbtype *hwidb, uchar *address)
{
    mif68840_instance *ds = INSTANCE(hwidb);
    mif68840_regs     *rp = ds->mif68840_regaddr;
    uchar mac_address[IEEEBYTES];

    if (!ds->cam_present)
        return;
    if (rp->cam[CSR] & CAM_CSR_FIFO_GO) {
        usecdelay(2);
        if (rp->cam[CSR] & CAM_CSR_FIFO_GO) {
            buginf("\nCAM: attempt to add CAM entry timed out,%x",
                rp->cam[CSR]);
            return;
        }
    }

    ieee_swap(address, mac_address);

    rp->cam[CSR]  &= (ushort)~CAM_CSR_CAM_COMPARE_ENABLE;
    while (rp->cam[CSR] & CAM_CSR_CAM_COMPARE)
        ;
    if (find_cam_entry(rp, GETSHORT(mac_address), GETSHORT(mac_address+2),
                GETSHORT(mac_address+4))) {
        rp->cam[CSR] |= CAM_CSR_CAM_COMPARE_ENABLE;
        return;
    }
    rp->cam[CSR] |= CAM_CSR_CAM_COMPARE_ENABLE;
    SPD_CR
    CAM_DATA_WR (GETSHORT(mac_address))
    CAM_DATA_WR (GETSHORT(mac_address+2))
    CAM_DATA_WR (GETSHORT(mac_address+4))
    MOV_M_NF_CR_V
    /* Set FIFO GO bit */
    rp->cam[CSR] |= CAM_CSR_FIFO_GO;
}

/*
 * mif68840_init_idb:
 * IDB init routine for MIF68840 interfaces
 */
static boolean mif68840_init_idb (hwidbtype *idb, mif68840_regs *base_addr)
{
    mif68840_instance  *ds;
    fddisb             *fsb;
    
    /*
     * If this is the first time through this routine
     * for this idb, then initialize the following.
     * For idbs off the removed queue, this initialization
     * should not be repeated.
     */
    if (idb->oir_flags & IDB_FIRST_INIT) {
    idb->type = IDBTYPE_FDDIT; 
    idb->typestring = "MIF68840";
    idb->status = (IDB_FDDI | IDB_BCAST | IDB_BRIDGE); 

    fddi_idb_init(idb);

    if (pas_oir_active()) {
        idb->state = IDBS_ADMINDOWN; 
    } else {
        idb->state = IDBS_INIT;
    }

    idb->buffersize = MIF68840_PARTICLE_SIZE;

    /*
     * Default paramters all in microseconds
     */
    fddi_smtqsize = nfddinets * 2;
    idb->fddi_options |= FDDI_SMT_FRAMES;
    idb->fddi_options |= FDDI_LEM_SUPPORTED;
    idb->fddi_options |= FDDI_CMT_ASSIST_SUPPORTED;
    idb->fddi_options |= FDDI_FAST_CMT;
    idb->fddi_options |= FDDI_BRIDGE_CAM_SUPPORTED;

    idb->counters.resets = 0;
 
    idb->span_encapsize = FDDI_FC;
 
    idb->error_threshold = 5000;
    idb->error_count = 0;

    idb->fast_switch_flag = 0;
    idb->pool = NULL;
    idb->pool_group = POOL_GROUP_INVALID;

    /*
     * Set up function calls
     */
    idb->soutput = mif68840_tx_start;
    idb->reset = mif68840_reset;
    idb->system_configured = mif68840_system_configured;
    idb->periodic = NULL;
    idb->shutdown = mif68840_shutdown;
    idb->enable = mif68840_enable;
    idb->listen = mif68840_add_multicast_address;
    idb->listen_range = mif68840_add_multicast_range;
    idb->firstsw->tbridge_media = TBR_FDDI;
    idb->device_periodic = mif68840_periodic;
    idb->show_controller = mif68840_show_controller;
    idb->fastsend = mif68840_fastsend;
    idb->linestate = mif68840_linestate;
    idb->txqlength = mif68840_txqlength;

    reg_add_media_delete_hardware_address_filter(mif68840_fddi_cam_depopulate,
                             "mif68840_fddi_cam_depopulate");
    reg_add_media_add_hardware_address_filter(mif68840_fddi_cam_populate,
                          "mif68840_fddi_cam_populate");

    TIMER_STOP(idb->lastinput);
    TIMER_STOP(idb->lastoutput);
    TIMER_STOP(idb->lastreset);

    idb->oir_flags &= ~IDB_FIRST_INIT;
    }

    media_idb_enqueue(idb);

    /*
     * Setup encsize
     */
    idb->encsize = FDDI_SNAP_ENCAPBYTES;

    fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);
    TIMER_STOP(fsb->fddi_operational);
    fsb->fddi_trt = 5000;
    fsb->fddi_tvx = 2500;
    idb_release_hwsb_inline(idb, HWIDB_SB_FDDI);

    /*
     * Allocate an instance structure for this device
     */
    ds = malloc_fast(sizeof(mif68840_instance));
    if (!ds) {
    return(FALSE);
    } 

    /* 
     * Allocate the SRB subblock here because VIP cannot allocate 
     *   from interrupt level later when needed
     *
     */
    srb_create_sb_for_vip_inline(idb->firstsw);

    INSTANCE(idb) = (void *)ds;
    ds->mif68840_regaddr = (mif68840_regs *) base_addr;

    pas_instance_init_common(&ds->pacb, idb);

    ds->csr = ds->mif68840_regaddr->csr;

    /* Bare minimum port operation is required even in down condition */
    FCR_IS_FREE;
    ds->mif68840_regaddr->ifddi[FCR] = WRITE_PORT_CONTROL_EN_REG_A;
	
    STADATA (idb->fddi_stn_id, smtCfg) = &DASSMTCfg;
    STADATA (idb->fddi_stn_id, macCfg) = &SingleFORMACMACCfg;
    STADATA (idb->fddi_stn_id, pathCfg) = &DASPATHCfg; 
    if (ds->csr & MULTIMODE_PHYB_PHYA) {
        STADATA (idb->fddi_stn_id, portCfg) = &MMDASPORTCfg;
        idb->typestring = "MIF68840_MM";
    } else { 
        STADATA (idb->fddi_stn_id, portCfg) = &SSDASPORTCfg;
        idb->typestring = "MIF68840_SS";
    }
    STADATA (idb->fddi_stn_id, xferRegs[0]) = (void *)ds->mif68840_regaddr->ifddi;
    STADATA (idb->fddi_stn_id, macRegs[0]) = (void *)ds->mif68840_regaddr->mac;
    STADATA (idb->fddi_stn_id, portRegs[0]) = (void *)ds->mif68840_regaddr->elma;
    STADATA (idb->fddi_stn_id, portRegs[1]) = (void *)ds->mif68840_regaddr->elmb;
    STADATA (idb->fddi_stn_id, hwIDB[0]) = (hwidbtype *)idb;
    STADATA (idb->fddi_stn_id, xferData[0]) = (void *)HWIDBDATA (idb->fddi_stn_id, 0, devctl);
    STADATA (idb->fddi_stn_id, macData[0]) = (void *)HWIDBDATA (idb->fddi_stn_id, 0, devctl);
    STADATA (idb->fddi_stn_id, portData[0]) = (void *)HWIDBDATA (idb->fddi_stn_id, 0, devctl);
    STADATA (idb->fddi_stn_id, portData[1]) = (void *)HWIDBDATA (idb->fddi_stn_id, 0, devctl);
    STADATA (idb->fddi_stn_id, hwPresent) = TRUE;
    STADATA (idb->fddi_stn_id, TLMin) = TL_MIN;

    /*
     * Setup the removal vector here so that if the init process
     * fails from here on in, there will be a device specific vector
     * defined to clean up resources
     */
    ds->pacb.pas_deallocate = mif68840_deallocate;

    if (!mif68840_allocate_rings(ds))
        return(FALSE);

    /*
     * Do platform dependent initialization
     */
    if (!mif68840_platform_init(idb)) {
    return(FALSE);
    }

    /* Create Beacon Frame */
    ds->bcn_ptr = malloc_pcimem(128);

    ds->cam_present = check_hw_cam(idb);
    /*
     * Successfully got all resources
     */
    return(TRUE);
}

/*
 * mif68840_init_interface:
 * Initialize a FDDI PA interface
 */
static boolean mif68840_init_interface (uint slot, uint unit, uint subunit,
                    mif68840_regs *base_addr)
{
    hwidbtype *idb;

    /*
     * Set up storage, add IDB to interface queue
     */
    idb = mif68840_idb_create(slot, subunit);
    if (!idb) {
    return(FALSE);
    }

    /*
     * Save our unit number and PA slot number:
     */
    idb->slot = slot;       /* Corresponds to IP */

    idb->subunit = subunit; /* Corresponds to viper concept of interface # */
    idb->slotunit = subunit;/* For slotted interface names */
    pas_set_fci_type(idb, slot); /* Need fci type for OIR support */
    idb->status &= ~IDB_NOHARDWARE;   
           
    FIND_FDDI_STATION_ID(idb);

    /* make idb->unit unique */
    idb->unit = idb->fddi_stn_id;      

    /*
     * Initialize the idb
     */
    if (!mif68840_init_idb(idb, base_addr)) {
    return(FALSE);
    }

    /*
     * Initialize idb namestrings
     */
    if (!idb->hw_namestring) {
    idb_init_names(idb->firstsw, FALSE);
    }

    idb->counters.resets = 0;

    /*
     * Initialize private buffer pools
     */
    if (!mif68840_init_buffer_pools(idb)) {
        printf("\n mif68840_init_interface(%s), failed mempool init",
            idb->hw_namestring);
        STADATA (idb->fddi_stn_id, hwPresent) = FALSE;
        return(FALSE);
    }

    return (TRUE);
}

/*
 * mif68840_pci_init: 
 * Do PCI specific initialization for the MIF68840
 */
static mif68840_regs *mif68840_pci_init (uint pa_bay, uint device_no)
{
    uint  value, id;
    mif68840_regs  *device_base;
    plx9060_local_regs *plx9060_local_cfg_regs_ptr;
    uint  bus_no;

    /*
     * Get our bus no
     */
    bus_no = get_pci_bus_no(pa_bay);        

    /*
     * Read the device/vendor registers to verify that this
     * is a MIF68840 Fddi controller.
     */
    id = pci_config_read(bus_no, device_no, 0, MIF68840_PCI_CFID_REG_OFFSET);
    if (id != MIF68840_DEVICE_VENDOR_ID) {
        errmsg(&msgsym(NOTMIF68840, MIF68840), pa_bay, id);
        return(NULL);
    }

    /*
     * Get device base address
     */
    device_base = get_pci_device_base(pa_bay, device_no);

    /*
     * Configure CFCS, the command and status configuration register.
     */
    if (MIF68840_MEMMAPPED_IO) {
        value= MIF68840_CFCS_MEM_SPACE_ACCESS | MIF68840_CFCS_MASTER_OPERATION;
    } else {
        value = MIF68840_CFCS_IO_SPACE_ACCESS | MIF68840_CFCS_MASTER_OPERATION;
    }
    pci_config_write(bus_no, device_no, 0,
                     MIF68840_PCI_CFCS_REG_OFFSET, value);

    /*
     * Configure CFLT, the configuration latency timer register.
     */
    pci_config_write(bus_no, device_no, 0, MIF68840_PCI_CFLT_REG_OFFSET,
                     MIF68840_CFLT_LATENCY_TIMER);

    /*
     * Configure the devices address space defintion
     */
    value = get_pci_device_base_offset(pa_bay, device_no);
    pci_config_write(bus_no, device_no, 0,
              MIF68840_PCI_CBMEM_REG_OFFSET, value);
    pci_config_write(bus_no, device_no, 0,
              MIF68840_PCI_CBIO_REG_OFFSET, value);
    pci_config_write(bus_no, device_no, 0,
              MIF68840_PCI_CBLOCAL_REG_OFFSET, value);

    /* Get device base for programming the local registers */
    plx9060_local_cfg_regs_ptr = (plx9060_local_regs *) device_base;

    /* Program the local registers for slave accesses */
    if (MIF68840_MEMMAPPED_IO) {
        plx9060_local_cfg_regs_ptr->range_pci_local = 0xffff8000;
    } else {
        plx9060_local_cfg_regs_ptr->range_pci_local = 0xffff8001;
    }
    plx9060_local_cfg_regs_ptr->remap_pci_local = 0x00000001;

    /* Program the local arb. register */
    plx9060_local_cfg_regs_ptr->local_arb_reg = 0x010302ff;

    /* Program the local bus region desriptor for PCI to local Acc reg */
    plx9060_local_cfg_regs_ptr->bus_descriptors_pci_local = 0x48030343;

    /* Program PCI Base Address (Re-map) reg for Direct Master to PCI*/
    plx9060_local_cfg_regs_ptr->remap_local_pci = 0x00000011;

    /* Program the EEPROM control, PCI Command Codes */
    plx9060_local_cfg_regs_ptr->eeprom_control = MIF68840_PLX_EEPROM_CONTROL;

    /*
     * Return the device base address
     */
    return(device_base);
}

/*
 * mif68840_analyze_device:  
 * Analyze a MIF68840 interface 
 */
static boolean mif68840_analyze_device (uint pa_bay, uint device_no)
{
    mif68840_regs *device_base;

    /*
     * Don't accept more than the maximum number of
     * supported FDDI interfaces.
     */
    if (nfddinets >= MAX_STATION_COUNT)
        return(FALSE);

    /*
     * Perform PCI configuration specific to this device.
     */
    device_base = (mif68840_regs *)mif68840_pci_init(pa_bay, device_no);
    if (!device_base) {
    return(FALSE);
    }

    if (test_in_progress)
        return(TRUE);

    nfddinets += 1;
    /*
     * Initialize this device
     */
    if (!mif68840_init_interface(pa_bay, nfddinets, device_no, device_base)) {
        nfddinets--;
        return(FALSE);
    }

    return(TRUE);
}

/*
 * mif68840_analyze_pa:
 * Init the pci to pci bridge above the port adaptor, then discover devices
 * behind the bridge on the port adapter.
 *
 */
static void mif68840_analyze_pa (int pa_bay)
{
    ulong count;
    uchar bus_no;

    bus_no = get_pci_bus_no(pa_bay);

    /* Discover and initialize devices on the PA */
    count = pci_discover_devices(bus_no, MAX_PCI_DEVICES, pa_bay);

    if (count != 1) {
        errmsg(&msgsym(DISCOVER, MIF68840), count, pa_bay);
        pas_deactivate_bay(pa_bay);
        return;
    }

    /*
     * Update slots state.
     */
    pas_activate_bay(pa_bay);

    /* Install PA wrapper ../asm-mips/asm_4k_vip_mif68840.S */
    pas_install_wrapper(pa_bay, mif68840_eim_wrapper);
}

/*
 * mif68840_rx_error:
 * Process a receive error, *FIXME* This all needs work.
 */
void mif68840_rx_error (ulong rmd_status_length,hwidbtype *idb,particletype *mp){
    mif68840_instance *ds = INSTANCE(idb);


    /* Check if RX frame has Port error */
    if (rmd_status_length & MIF68840_RX_PORT) {
        buginf("\nRX_Port Error, Error indication=0x%08x", rmd_status_length);
        ds->rx_port_errors++;
    }
    
    /* Check if RX frame has overrun or MACIF error */
    if (rmd_status_length & MIF68840_RX_OVR_PAR) {

        /* This will happen when the fragment of aborted frame exceeds
         * the receive watermark. Two instances are rx_overrun and
         * rx_parity_error 
         * Overrun occurs when the receiver is
         * accumulating data sent from the MAC in much less time than 
         * it takes the FSI to acquire the system bus.
         * RX_PARITY_ERROR occurs when there is a parity error detected by
         * the MACIF. 
         */

        /* Overrun error */
        if (rmd_status_length & MIF68840_RX_OVR) {
            idb->counters.input_overrun++;
            ds->rx_overrun_errors++;

            /*An overrun error has been detected. As a result,
             *the receiver for the ring has been turned off.
             *We need to enable the bit in MACIF receive 
             *control register (MRR).  I don't think this is necessary.
             */

        } else if (rmd_status_length & MIF68840_RX_PAR) {

            /*  
             * MACIF errors. ie. A parity error has been detected by 
             * the MACIF 
             */
            buginf("\nRX_MACIF Parity Error, Error indication=0x%08x", 
                rmd_status_length);
            ds->rx_macif_parity_errors++;

        } else {

            /* Could be another error based on STT */
            return;
        }
    }
    
    /* Check if RX frame has CRC error */
    if ((rmd_status_length & MIF68840_RX_ERROR) == MIF68840_RX_CRC) {
        idb->counters.input_crc++;
    }

    /*
     * Account for the error
     */
    idb->counters.input_error++;
}

/*
 * mif68840_tx_error:
 * Process a transmit error, *FIXME* This all needs work.
 */
void mif68840_tx_error (ulong tmd_status, hwidbtype *idb)
{
    mif68840_instance *ds = INSTANCE(idb);

    /* Check if frame has a Port error */
    if (tmd_status & MIF68840_TX_PORT) {
        ds->tx_port_errors++;
        buginf("\nTX_Port Error: Code = 0x%01x",
            (tmd_status & MIF68840_TX_PORT) >> 19);

    } else if (tmd_status & MIF68840_TX_UNDERRUN) {

        /* check for underrun error */
        buginf("\nTX: underrun error");
        idb->counters.output_underrun++;

    } else if (tmd_status & MIF68840_TX_PARITY) {

        /* Check for transmit parity errors */
        buginf("\nTX: parity error");
        ds->tx_parity_errors++;

    } else  {

        /* There was a transmit error but neither a port error, an
         * underrun erro or a parity error. Therefore, the frame must
         * been aborted by the assertion of TABORT input signal
         */

        buginf("\nTX: Frame aborted by TABORT input signal, 0x%08x",
            tmd_status);
    }

    /*
     * Count the error
     */  
    idb->counters.output_error++;
}


/*
 * mif68840_smt_init:
 * Fire up XDI processes and timers.
 */
static void  mif68840_smt_init (void)
{
    MIBStartup ();
    CSPStartup ();
    FBMStartup ();

    /* Create CSP process-level timer */
    mgd_timer_init_leaf(&mif68840_CSPTimer, NULL, 0, NULL, TRUE);

    mif68840_CSPTimer_process_id = process_create(mif68840_CSPTimer_process, 
        "CSP Timer", NORMAL_STACK, PRIO_HIGH); 
}


/*
 * mif68840_subsys_init -
 * mif68840 subsystem initialisation entry point
 */
static void mif68840_subsys_init (subsystype *subsys)
{
    int i, j;

    /* Register the PCI 1F Port Adapter  */

    reg_add_analyze_interface(PA_HARDWARE_FDDI_DAS_MM,
                              mif68840_analyze_pa,
                              "mif68840_analyze_pa");
    reg_add_analyze_interface(PA_HARDWARE_FDDI_DAS_SS,
                              mif68840_analyze_pa,
                              "mif68840_analyze_pa");

    reg_add_pci_device_init(MIF68840_DEVICE_VENDOR_ID,
                mif68840_analyze_device,
                "mif68840_analyze_device");

    /* Fire up the XDI smt processes required of each platform */
    mif68840_smt_init();

    /*
     * Reset FDDI SMT data structure
     */
    for (i = 0; i < MAX_STATION_COUNT; i++) {
        for (j=0; j<MAX_MAC_COUNT; j++)
            STADATA (i, hwIDB[j]) = 0;
        for (j=0; j<MAX_PORT_COUNT; j++)
            STADATA (i, portRegs[j]) = 0;
        STADATA (i, hwPresent) = FALSE;
        STADATA (i, prevCFState) = CF_isolated;
    }
}

/* 
 * mif68840 subsystem header
 */
#define MIF68840_MAJVERSION   1
#define MIF68840_MINVERSION   0
#define MIF68840_EDITVERSION  1
    
SUBSYS_HEADER(mif68840, MIF68840_MAJVERSION, MIF68840_MINVERSION,
          MIF68840_EDITVERSION,  
          mif68840_subsys_init, SUBSYS_CLASS_DRIVER,
          NULL,
          NULL);

/****************************************************************************/
/* Test Routines begin */

/* 
 * mif68840_powerup_reset:
 * Assumes graceful shutdown has taken place
 * returns with hardware ready to go
 */
static void mif68840_powerup_reset (hwidbtype *idb)
{
    /* Take power off */
    pas_control_power(idb->slot, FALSE);
    DELAY(100);

    /* Apply reset and power-on */
    pas_control_reset(idb->slot, TRUE);
    pas_control_power(idb->slot, TRUE);
    DELAY(600);

    /* Remove reset */
    pas_control_reset(idb->slot, FALSE);
    DELAY(100);
 
    /* need to get the pci interface as before */
    mif68840_analyze_pa(idb->slot);
}


static int testelmb (hwidbtype *idb)
{
    mif68840_instance *ds=INSTANCE(idb);
    ulong  violation, link_error, bist;

    ds->mif68840_regaddr->elmb[CNTRL_A_REG] = EB_LOC_LOOP_MASK;
    violation = ds->mif68840_regaddr->elmb[VIOL_SYM_CTR_REG];
    link_error= ds->mif68840_regaddr->elmb[LINK_ERR_CTR_REG];
    ds->mif68840_regaddr->elmb[CNTRL_A_REG] = RUN_BIST_MASK;

    DELAY(10);

    bist = ds->mif68840_regaddr->elmb[BIST_SIGNATURE_REG] & 0xffff;
    if (bist == 0x5b6b) {
        printf("\nPassed BIST elmb, BIST value = 0x%04x", bist);
        return(0);
    } else {
        printf("\nFailed BIST elmb, BIST value = 0x%04x", bist);
        return(1);
    }
}
    
static int testelma (hwidbtype *idb)
{
    mif68840_instance *ds=INSTANCE(idb);
    ulong violation, link_error, bist;
        
    ds->mif68840_regaddr->elma[CNTRL_A_REG] = EB_LOC_LOOP_MASK;
    violation = ds->mif68840_regaddr->elma[VIOL_SYM_CTR_REG];
    link_error= ds->mif68840_regaddr->elma[LINK_ERR_CTR_REG];
    ds->mif68840_regaddr->elma[CNTRL_A_REG] = RUN_BIST_MASK;

    DELAY(10);

    bist = ds->mif68840_regaddr->elma[BIST_SIGNATURE_REG] & 0xffff;
    if (bist  == 0x5b6b) {
        printf("\nPassed BIST elma, BIST value = 0x%04x", bist);
        return(0);
    } else {
        printf("\nFailed BIST elma, BIST value = 0x%04x", bist);
        return(1);
    }
}
    
static int testmac (hwidbtype *idb)
{
    mif68840_instance *ds=INSTANCE(idb);
    ulong bist;

    ds->mif68840_regaddr->mac[CONTROL_REG_B] = 0x200;
    ds->mif68840_regaddr->mac[MY_SHORT_ADDRESS_REG] = 0x7777;
    ds->mif68840_regaddr->mac[MY_LONG_ADDRESS_REG_A]= 0xaaaa;
    ds->mif68840_regaddr->mac[MY_LONG_ADDRESS_REG_B]= 0xbbbb;
    ds->mif68840_regaddr->mac[MY_LONG_ADDRESS_REG_C]= 0xcccc;
    ds->mif68840_regaddr->mac[TREQ_REG] = 0xff00;
    ds->mif68840_regaddr->mac[TVX_VALUE_AND_TMAX_REG] = 0xaacc;
    ds->mif68840_regaddr->mac[CONTROL_REG_A] = 0x88c4;
    DELAY(10);
    bist = ds->mif68840_regaddr->mac[MAC_BIST_SIGNATURE_REG] & 0xffff;
    if (bist != 0x9fa2)  {
        printf("\n\tFailed BIST_1 mac, BIST value = 0x%04x", bist);
        return(1);
    } else {
        printf("\n\tPassed BIST_1 mac, BIST value = 0x%04x", bist);
        mif68840_powerup_reset(idb);
        ds->mif68840_regaddr->mac[CONTROL_REG_B] = 0x82ff;
        ds->mif68840_regaddr->mac[MY_SHORT_ADDRESS_REG] = 0x8888;
        ds->mif68840_regaddr->mac[MY_LONG_ADDRESS_REG_A]= 0x5555;
        ds->mif68840_regaddr->mac[MY_LONG_ADDRESS_REG_B]= 0x4444;
        ds->mif68840_regaddr->mac[MY_LONG_ADDRESS_REG_C]= 0x3333;
        ds->mif68840_regaddr->mac[TREQ_REG] = 0x00ff;
        ds->mif68840_regaddr->mac[TVX_VALUE_AND_TMAX_REG] = 0x5533;
        ds->mif68840_regaddr->mac[CONTROL_REG_A] = 0xf127;

        DELAY(10);

        bist = ds->mif68840_regaddr->mac[MAC_BIST_SIGNATURE_REG] & 0xffff;
        if (bist  != 0x9324)  {
            printf("\n\tFailed BIST_2 mac, BIST value = 0x%04x", bist);
            return(1);
        } else {
            printf("\n\tPassed BIST_2 mac, BIST value = 0x%04x", bist);
            mif68840_powerup_reset(idb);
            ds->mif68840_regaddr->mac[CONTROL_REG_B] = 0x13ff;
            ds->mif68840_regaddr->mac[MY_SHORT_ADDRESS_REG] = 0x137f;
            ds->mif68840_regaddr->mac[MY_LONG_ADDRESS_REG_A]= 0x37f1;
            ds->mif68840_regaddr->mac[MY_LONG_ADDRESS_REG_B]= 0x7f13;
            ds->mif68840_regaddr->mac[MY_LONG_ADDRESS_REG_C]= 0xf137;
            ds->mif68840_regaddr->mac[TREQ_REG] = 0xfec8;
            ds->mif68840_regaddr->mac[TVX_VALUE_AND_TMAX_REG] = 0xf0fc;
            ds->mif68840_regaddr->mac[CONTROL_REG_A] = 0xa414;

            DELAY(10);
    
            bist = ds->mif68840_regaddr->mac[MAC_BIST_SIGNATURE_REG] & 0xffff;
            if (bist != 0x8445)  {
                printf("\n\tFailed BIST_3 mac, BIST value = 0x%04x", bist);
                return(1);
            } else {
                printf("\n\tPassed BIST_3 mac, BIST value = 0x%04x", bist);
            }
        }
    }
    return(0);
}

/* End of Test routines */

/************************************************************************/
/* Dump routines begin */

/* 
 * pas_fddi: 
 * Handle "test pas fddi"
 */
void pas_fddi (parseinfo *csb)
{
    char buff[BUFLEN];
    boolean parsed;
    char cmdchar = '?';
    int result = 0;
    hwidbtype *idb = NULL;
    mif68840_instance *ds;

    idb = GETOBJ(idb,1)->hwptr;
    ds  = INSTANCE(idb);

    if (ds->csr & MULTIMODE_PHYB_PHYA)
        printf("\n Port adapter %s is Multimode Fddi", idb->hw_namestring);
    else
        printf("\n Port adapter %s is Singlemode Fddi", idb->hw_namestring);

    /*
     * Loop until the user quits or there is an error
     */
    while (TRUE) {
    
        printf("\n Port %s (? for help)[%c]:  ", idb->hw_namestring, cmdchar);
        parsed = rdtty(buff, BUFLEN);
        if (!null(buff) && parsed) {
            cmdchar = toupper(buff[0]);
        }
    
    switch (cmdchar) {

        /* Print out a helpful menu */
       
    case 'H':
    case '?':
        printf("\n  r - Power-Up reset");
        printf("\n  m - run BIST for MAC.  4 seconds");
        printf("\n  a - run BIST for ELMA. 2 seconds");
        printf("\n  b - run BIST for ELMB. 2 seconds");
        printf("\n  d - dump internal CAM");
        printf("\n  q - exit eeprom test");
        printf("\n\n  'c' rules of radix type-in and display apply.\n");
        break;

    /* Power-Up Reset */
    case 'R':
        if (idb->state != IDBS_ADMINDOWN) {
            printf("\nPlease shut down the interface before this test.");
            return;
        }
        test_in_progress = TRUE;
        mif68840_powerup_reset(idb);
        test_in_progress = FALSE;
        printf("\n\tPower-Up reset complete");
        printf("\n");
        return;
    
    /* Run MAC BIST test*/
    case 'M':
        if (idb->state != IDBS_ADMINDOWN) {
            printf("\nPlease shut down the interface before this test.");
            return;
        }
        test_in_progress = TRUE;
        mif68840_powerup_reset(idb);
        result = testmac(idb);
        mif68840_powerup_reset(idb);
        if (result) {
            printf("\n\tMAC BIST test for fddi port adapter %s FAILS",
                idb->hw_namestring);
        } else {
            printf("\n\tMAC BIST test for fddi port adapter %s PASSES",
                idb->hw_namestring);
        }
        test_in_progress = FALSE;
        printf("\n\tMAC BIST test completed");
        printf("\n");
        return;

    /* Run ELMA BIST test */
    case 'A':
        if (idb->state != IDBS_ADMINDOWN) {
            printf("\nPlease shut down the interface before this test.");
            return;
        }
        test_in_progress = TRUE;
        mif68840_powerup_reset(idb);
        result = testelma(idb);
        mif68840_powerup_reset(idb);
        if (result) {
            printf("\n\tELMA BIST test for fddi port adapter %s FAILS", 
                idb->hw_namestring);
        } else {
            printf("\n\tELMA BIST test for fddi port adapter %s PASSES",
                idb->hw_namestring);
        }
        test_in_progress = FALSE;
        printf("\n\tELMA BIST test completed");
        printf("\n");
        return;

    /* Run ELMB BIST test */
    case 'B':
        if (idb->state != IDBS_ADMINDOWN) {
            printf("\nPlease shut down the interface before this test.");
            return;
        }
        test_in_progress = TRUE;
        mif68840_powerup_reset(idb);
        result = testelmb(idb);
        mif68840_powerup_reset(idb);
        if (result) {
            printf("\n\tELMB BIST test for fddi port adapter %s FAILS", 
                idb->hw_namestring);
        } else {
            printf("\n\tELMA BIST test for fddi port adapter %s PASSES",
                idb->hw_namestring);
        }
        test_in_progress = FALSE;
        printf("\n\tELMB BIST test completed");
        printf("\n");
        return;

    /* Dump contents of internal CAM */
    case 'D':
        dump_internal_cam(idb);
        printf("\n");
        printf("\n Internal CAM dump completed");
        break;

    /* Quit */
    case 'Q':
        return;

    /* Woops, user screwed up */

    default:
        printf("%%Invalid selection\n");
        break;
    }

    }
}
/* end of file */
