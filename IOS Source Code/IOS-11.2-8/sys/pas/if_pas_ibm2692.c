/* $Id: if_pas_ibm2692.c,v 3.17.10.35 1996/09/15 00:52:21 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_pas_ibm2692.c,v $
 *------------------------------------------------------------------
 * if_pas_ibm2692.c - Platform independent code for 4R
 *		      IBM Token Ring PCI port adaptor
 *
 * May 1995, HC Wang
 * 
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_pas_ibm2692.c,v $
 * Revision 3.17.10.35  1996/09/15  00:52:21  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   Add srb_create_sb_for_vip_inline() so that SRB subblock can be
 *     created for VIP when swidb is initialized
 *
 * Revision 3.17.10.34  1996/09/14  01:04:28  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   Previous commit breaks build for c7200- images
 *     Backout previous changes
 *     Create SRB_SB subsystem to init SRB vector table
 *       rather than init in  VIP reset routines
 *
 * Revision 3.17.10.33  1996/09/13  19:47:56  ppearce
 * CSCdi68885:  SRB subblock not setup properly for VIP interfaces
 * Branch: California_branch
 *   - Add SRB subblock object to VIP ucode (needs to be its own subsys)
 *   - Create SRB subblock at same time tokenRing/FDDI swidb created
 *   - Init SRB vector table when interface is reset
 *
 * Revision 3.17.10.32  1996/08/28  13:06:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.17.10.31  1996/08/26  15:10:59  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.17.10.30  1996/08/22  18:42:37  dujihara
 * CSCdi65823:  Error messages dont specify slot/subunit
 * Branch: California_branch
 * Added slot (bay) and unit to error messages.
 *
 * Revision 3.17.10.29  1996/08/13  02:21:29  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.17.10.28  1996/07/25  21:05:03  sthormod
 * CSCdi63866:  C7200: rif entries not marked as local interface
 * Branch: California_branch
 *
 * Revision 3.17.10.27  1996/07/18  22:07:41  sthormod
 * CSCdi61600:  multi-port srb is process-switched on c7200
 * Branch: California_branch
 *
 * Revision 3.17.10.26  1996/07/16  09:36:40  hampton
 * Centralize knowledge of Decnet/Token Ring interaction.  [CSCdi63066]
 * Branch: California_branch
 *
 * Revision 3.17.10.25  1996/07/09  06:04:02  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.17.10.24  1996/06/26  23:44:48  hwang
 * CSCdi57723:  VIP1 tokenring interfaces wont come up
 * Branch: California_branch
 *
 * Revision 3.17.10.23  1996/06/12  19:22:52  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xxbit_word (final part)
 *
 * All good things must come to an end.  ntohl() and friends finally have
 * UNIX semantics in California releases and later.
 *
 * Branch: California_branch
 *
 * Revision 3.17.10.22  1996/06/07  23:52:23  sthormod
 * CSCdi57928:  OIR of 4R Wedges Console (No Throttling Code)
 * Branch: California_branch
 *
 * Revision 3.17.10.21  1996/06/07  00:17:43  hwang
 * CSCdi59533:  4r driver need to implement srb command queue
 * Branch: California_branch
 *
 * Revision 3.17.10.20  1996/05/28  17:41:49  hwang
 * CSCdi55620:  Missing idb_release_hwsb_inline in 4R driver
 * Add missing idb_release_hwsb_inline, if we are not changing data in
 * sub block don't use "idb_use_hwsb_inline" use "idb_get_hwsb_inline"
 * instead.
 * Branch: California_branch
 *
 * Revision 3.17.10.19  1996/05/24  20:00:11  hwang
 * CSCdi58573:  4R receiver become deaf
 * Enable Hermon timerinterrupt to sample pci busmaster status
 * register every 500 ms, if it indicates receiver channel is
 * not enabled or disabled reenable receive channel.
 * Branch: California_branch
 *
 * Revision 3.17.10.18  1996/05/23  00:53:00  sthormod
 * CSCdi57953:  Enable IP Flow/Optimum on token ring
 * Branch: California_branch
 *
 * Revision 3.17.10.17  1996/05/23  00:11:31  sthormod
 * CSCdi55611:  4r port adapter output queue wedges under stress...
 * Branch: California_branch
 *
 * Revision 3.17.10.16  1996/05/21  10:00:02  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.17.10.15  1996/05/17  11:40:05  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.17.2.3  1996/05/05  23:42:58  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.17.10.14  1996/05/07  00:48:39  sthormod
 * CSCdi55832:  4r PA driver needs to clean-up after OIR
 * Branch: California_branch
 *
 * Revision 3.17.10.13  1996/05/01  03:39:36  tboyle
 * CSCdi54205: Reduce buffer space reqd by 4R
 * Branch: California_branch
 *
 * Revision 3.17.10.12  1996/04/29  17:57:22  ssangiah
 * CSCdi54472: Inadequate information in the output of "sh controllers"
 *             command
 *   o Add a "show diag" command for the c7200 platform.
 *   o Make the output from "show controllers" consistent.
 * Branch: California_branch
 *
 * Revision 3.17.2.2  1996/04/26  15:22:40  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.17.10.11  1996/04/24  00:20:45  sthormod
 * CSCdi53847:  XNS fails to append the MAC address to the net number on
 * Token Ring
 * Branch: California_branch
 * Move the mac address assignment to before the check for
 * !system_configured.
 *
 * Revision 3.17.10.10  1996/04/23  02:23:54  sthormod
 * CSCdi54994:  4R uses cpu to byte-swap
 * Branch: California_branch
 *
 * Revision 3.17.10.9  1996/04/08  19:00:55  dcarroll
 * CSCdi53796:  Remove extra idb type assignments
 * Branch: California_branch
 * Remove the unneeded early setting of idb type.
 *
 * Revision 3.17.10.8  1996/04/05  19:56:20  dcarroll
 * CSCdi53769:  Return bool from pas init interface routines
 * Branch: California_branch
 * Pass boolean up the analyze chain, not idbs.
 *
 * Revision 3.17.10.7  1996/04/05  05:32:19  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.17.10.6  1996/04/04  20:04:17  getchell
 * CSCdi50943:  SVIP PA reset broken
 * Branch: California_branch
 *
 * Revision 3.17.10.5  1996/03/30  01:30:36  hwang
 * CSCdi51725:  4r driver need to provide lan manager support
 * Branch: California_branch
 * Set up proper address match bit when processing mac frame, note that
 * we have two different path to receive a mac frame, either via normal
 * LLC channel or using ARB.
 *
 * Revision 3.17.10.4  1996/03/28  02:00:12  sthormod
 * CSCdi52811:  4R SMT stats not displayed
 * Branch: California_branch
 *
 * Revision 3.17.10.3  1996/03/27  19:46:50  hwang
 * CSCdi52699:  pma pci 16 bit read work around
 * Branch: California_branch
 *
 * Revision 3.17.10.2  1996/03/21  23:23:31  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.17.10.1  1996/03/18  21:34:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.14  1996/03/18  23:28:55  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.13  1996/03/13  16:53:09  sthormod
 * CSCdi51335:  SegV exception while configuring decnet for tokenring
 * Branch: ELC_branch
 * Don't forget to bit swap those mac addresses.
 *
 * Revision 3.3.2.12  1996/03/10  02:49:53  sthormod
 * CSCdi51196:  add tbridge switching path for predator 4R
 * Branch: ELC_branch
 *
 * Revision 3.3.2.11  1996/03/06  19:51:35  sthormod
 * CSCdi50859:  add srb switching path for predator 4R
 * Branch: ELC_branch
 * o  process & fast switching path for srb & explorers.
 * o  routing with rif field.
 * o  use dma to coalesce particle based packet.
 * o  process mac frames.
 *
 * Revision 3.3.2.10  1996/02/22  00:00:02  sthormod
 * CSCdi49158:  predator/4R keep alives dont work properly
 * Branch: ELC_branch
 * Need to stop tr_lineuptimer on init so that periodic doesn't prematurely
 * shut down the interface.
 *
 * Revision 3.3.2.9  1996/02/21  22:45:58  sthormod
 * CSCdi49158:  predator/4R keep alives dont work properly
 * Branch: ELC_branch
 * - add token-ring error support
 * - add keep alive code
 * - update lastoutput
 * - add ibm2692_version_string()
 * - remove ibm2692_linestate(), use default tring_linestate
 * - move ibm2692_periodic() to idb->device_periodic vector
 * - add idb->system_configured vector
 *
 * Revision 3.3.2.8  1996/02/08  08:48:00  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.7  1996/01/31  01:37:28  sthormod
 * Branch: ELC_branch
 * Add Predator 4R support.
 *
 * Revision 3.3.2.6  1996/01/16  01:43:01  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.5  1995/12/19  23:32:33  sthormod
 * Branch: ELC_branch
 * Initial structure changes for porting 4R driver to predator.
 *
 * Revision 3.3.2.4  1995/12/11  19:13:50  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.3.2.3  1995/12/06  04:08:50  sthormod
 * Branch: ELC_branch
 * Bring in HC's code review changes.
 *
 * Revision 3.3.2.2  1995/11/30  02:05:51  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.3.2.1  1995/11/22  21:42:02  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.17  1996/02/22  02:29:14  hwang
 * CSCdi49454:  4R driver need to use new ibm mpc ucode feature in tb mode
 * Set disable ASA copy option if configured in tb mode, this bit is
 * not documented in the ibm spec.
 *
 * Revision 3.16  1996/02/14  22:02:10  hwang
 * CSCdi48583:  vines packets not forwarded in 4r-4r transparent bridge
 * Reduce the size of reject address table, if tb is configured xmt one
 * packet at a time (no chained packet to avoid rejection addr tbl
 * overflow). When access the packet memory use cpu view address instead
 * of pci byte swap address.
 *
 * Revision 3.15  1996/02/12  23:06:51  hwang
 * CSCdi46480:  4R failed hot standby test
 * Add configuration mechanism to allow 4r driver to configure
 * new functional address.
 *
 * Revision 3.14  1996/02/05  03:37:09  hwang
 * CSCdi47198:  router with rvip (4R & 4T) hang after config SRT
 * ibm token ring chip if configured to transparent bridge mode it will
 * copy broad cast frame it transmitted, which causing the network
 * flooding.
 * Implement software filter to drop such frame received.
 *
 * Revision 3.13  1996/01/23  23:50:49  hwang
 * CSCdi46141:  rvip 4r/4r crashes with ipx traffic
 * Hermon tx bug work around, don't give hermon any packet to xmt before
 * end of chain tx interrupt.
 *
 * Revision 3.12.4.1  1996/01/24  22:47:52  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.12  1996/01/13  02:51:42  rharagan
 * CSCdi46869:  Enable memory mapping for VIP
 * Change the default PCI access mode.
 *
 * Revision 3.11  1995/12/21  17:49:18  hwang
 * CSCdi45680:  appletalk over SRB using RVIP fails
 * Enable limited RI broadcast copy, only screening all
 * bridge multicast packet.
 *
 * Revision 3.10  1995/12/17  18:35:57  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.9  1995/12/14  19:42:09  hwang
 * Modify packet classify function to provide correct tb/srb type packet,
 * use latest ibm info to set the hidden copy_all_options in modify_receive
 * option srb command.
 * CSCdi43509:  srb bridging over rvip fails
 *
 * Revision 3.8  1995/12/12  04:45:47  hwang
 * CSCdi45276:  4R driver should use new idbtype instead of IDBTYPE_CTR
 *
 * Revision 3.7  1995/12/06  18:31:43  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.6  1995/12/03  06:39:40  hwang
 * CSCdi44890:  incorporate code review comments into 4r driver
 *
 * Revision 3.5  1995/12/01  21:37:01  hwang
 * Disable all receive options when open adapter to avoid mpc run out
 * of receive buffer and cause adapter check error.
 * CSCdi43619:  adapter check error (0001 0002 954A 8C0D)
 *
 * Revision 3.4  1995/12/01  02:00:05  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.3  1995/11/17  18:54:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:57:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:33:03  hampton
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
#include "sys_registry.h"
#include "media_registry.h"
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

#include "../if/tring.h"
#include "../if/tring_private.h"
#include "../if/tring_common.h"
#include "../if/pci.h"
#include "../if/network.h"
#include "../if/linkdown_event.h"
#include "../if/msg_tr.c"		/* Not a typo, see logger.h */
#include "../if/rif_inline.h"

#include "../hes/if_fci.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "../h/ieee.h"

#include "../smf/smf_public.h"

#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"

#include "../srt/srb_sb_private.h"

#include "if_pas.h"
#include "if_pas_ibm2692.h"
#include "msg_ibm2692.c"		/* Not a typo, see logger.h */
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "pas_id.h"

/*
 * ibm2692_reset : Issue software reset to Hermon/MPC, this will destroy
 *                 all configurations of the chip set.
 */
void ibm2692_reset (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    hermon_reg_map *hermon_ptr;

    hermon_ptr = ds->ibm2692_ptr;

    /*
     * issue software reset to Hermon
     */
    hermon_ptr->bctl |= HERMON_SOFT_RESET;

    /*
     * this bit has to be on for minimum 16 PCI cycle and 10 ICDB cycle 
     */
    usecdelay(HERMON_MPC_RESET_DELAY);
    hermon_ptr->bctl &= ~HERMON_SOFT_RESET;	/* remove soft reset */
}


/*
 * ibm2692_lap_read
 */
void ibm2692_lap_read (hermon_reg_map *hermon_ptr, ushort access_type,
		       ushort addr, ushort *buf, ushort length) 
{
    int i;

    hermon_ptr->lape = access_type;
    hermon_ptr->lapa = addr;
    for (i = 0; i < btow(length); i++) {
	buf[i] = swap_16bit_word(hermon_ptr->lapd);
	addr += 2;
	hermon_ptr->lapa = addr;
    }
}

/*
 * ibm2692_lap_write
 */
void ibm2692_lap_write (hermon_reg_map *hermon_ptr, ushort access_type, 
			ushort addr, ushort *buf, ushort length )  
{
    int i;

    hermon_ptr->lape = access_type;
    hermon_ptr->lapa = addr;
    for (i = 0; i < btow(length); i++) {
	hermon_ptr->lapd = swap_16bit_word(buf[i]);
	addr += 2;
	hermon_ptr->lapa = addr;
    }
}

/*
 * ibm2692_tx_start : starting output
 */
void ibm2692_tx_start (hwidbtype *idb)
{
    leveltype level;    

    level = raise_interrupt_level(NETS_DISABLE);
    ibm2692_safe_start(idb);
    reset_interrupt_level(level);
}

/*
 * ibm2692_safe_start : starting output
 */
void ibm2692_safe_start (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    paktype *pak;

    while ((ds->tx_outstanding < IBM2692_TX_RING_ENTRIES) &&
	   (idb->output_qcount)) {
	/*
	 * dequeue a packet from output hold queue
	 */
	if ((pak = (*idb->oqueue_dequeue)(idb))) {
	    /*
  	     * process level doesn't necessarily change AC on srb
  	     * input frame - input AC may not be suitable for output
  	     * on 4R port adapter.
  	     */
  	    if (pak->flags & (PAK_SRB_OUT|PAK_SRB_EXPLORER))
  		*pak->datagramstart = AC_PRI4;
  	 
	    if (pak->datagramsize > idb->max_pak_size) {
	        idb->counters.output_total_drops++;
	        datagram_done(pak);
	    } else {
		/*
		 * Do packet accounting
		 */
		if (IBM2692_PKT_STATS) {
		    idb->counters.tx_cumbytes += pak->datagramsize;
		    idb->counters.outputs += 1;
		    if (*(pak->datagramstart) & 0x01) {
			idb->counters.output_broad++;
		    }
		    GET_TIMESTAMP(idb->lastoutput);
		}
		
	        ibm2692_transmit_inline(idb, pak, TRUE);
	    }
	} else {
	    /*
	     * output hold queue empty
	     */
	    break;
	}
    }
}

/*
 * ibm2692_version_string
 */
char *ibm2692_version_string (uchar *ucode_ver)
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
 * ibm2692_alive
 */
static tinybool ibm2692_alive (hwidbtype *hwidb)
{
    ibm2692_instance_t *ds;
    leveltype level;
    hermon_reg_map	*hermon_ptr;
    ushort	r1;

    ds = INSTANCE(hwidb);
    hermon_ptr = ds->ibm2692_ptr;

    level = raise_interrupt_level(NETS_DISABLE);
    r1 = hermon_ptr->bmctl_sum;
    reset_interrupt_level(level);

    if ((r1 & TX2_DISABLED) || (r1 & RX_DISABLED)) {
	return FALSE;
    } else {
	/*
	 * This does not guarantee that the chip is functioning, but
	 * it does at least give us a "warm-fuzzy" that the
	 * transmitter and reciever are not disabled in the chip.
	 */
	return TRUE;
    }
}

/*
 * ibm2692_show_controller
 */
static boolean ibm2692_show_controller (hwidbtype *hwidb, parseinfo *csb)
{
    ibm2692_instance_t *ds;
    vip4r_data_t *pa4r;
    uchar *version;
    smt_parms_table *smt;
    addr_table *addr_tbl; 
    mpc_errors *mpc;
    tx_errors *tx, *tx_prev;
    rx_errors *rx, *rx_prev;
    ulong func_addr;
    tokensb	*tsb;
    tinybool alive;

    ds = INSTANCE(hwidb);
    pa4r = INSTANCE_4R_STATS(hwidb);

    if (!pa4r) 
	return automore_quit();

    tsb = idb_get_hwsb(hwidb, HWIDB_SB_TOKEN);
    /*
     * Interface name/status/mac address
     */
    printf("\nInterface %s: state %s", hwidb->hw_namestring, 
                                       print_hwidbstate(hwidb));
    printf("\n  current address: %e, burned in address: %e",
	   hwidb->hardware, hwidb->bia);

    if (tokenevent_debug) {
	/*
	 * This info is not really useful for customers, but is very
	 * useful for debugging, so for now we'll hide this under a
	 * debug flag.
	 */
	leveltype level;
	hermon_reg_map	*hermon_ptr;
	ushort	r1, r2, r3, r4;
	volatile rx_bfr_desc_t	*bda, *lbda;
	volatile tx_bfr_desc_t	*fda, *lfda;
	uchar bus_no;
	uint  pci_status;

	bus_no = get_pci_bus_no(ds->pacb.pa_bay);
	hermon_ptr = ds->ibm2692_ptr;
	level = raise_interrupt_level(NETS_DISABLE);
	/*
	 * read in hermon registers
	 */
	r1 = hermon_ptr->bmctl_sum;
	r2 = hermon_ptr->misr;
	r3 = hermon_ptr->sisr;
	r4 = hermon_ptr->lisr;

	pci_status = pci_config_read(bus_no, ds->pacb.pa_device, 0, 
				     PCI_STATUS_REG_OFFSET);
	reset_interrupt_level(level);

	printf("\n  Device values:");
	printf("\n    dev_base %x, ds %x", hermon_ptr, ds);
	printf("\n    bmctl %x, misr %x, sisr %x, lisr %x", r1, r2, r3, r4);
	printf("\n    rx recovery %d, tx recovery %d",
	       ds->rx_chan_disabled, ds->tx_chan_disabled); 


	/*
	 * read in ring pointers
	 */ 
	level = raise_interrupt_level(NETS_DISABLE);
	bda = hermon_ptr->rxbda;
	lbda = hermon_ptr->rxlbda;
	fda = hermon_ptr->tx2fda;
	lfda = hermon_ptr->tx2lfda;
	reset_interrupt_level(level);
	printf("\n    rxbda %x, rxlbda %x, txfda %x, txlfda %x", 
	       bda, lbda, fda, lfda);
	printf("\n    tx_pci_stat_fail %x, pci_status %x",
	       ds->tx_pci_stat_fail, pci_status);
    }

    /*
     * Ring Status
     */
    print_ring_status(hwidb);

    ibm2692_get_smt_stats(hwidb);

    alive = ibm2692_alive(hwidb);
    printf("\n  Monitor state: (%sactive)", (alive ? "" : "not " ));
    version = ibm2692_version_string(pa4r->ucode_version);
    printf(", chip f/w: '%s', [bridge capable]", version);
    printf("\n    ring mode: %x", hwidb->tr_ring_mode);
    addr_tbl = &pa4r->addr_tbl;
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
	if (func_addr & TRFUNC_CDP)
	    printf(" CDP");
    }
    printf("\n    internal functional: %08x (%08x), group: N/A (%08x)",
	   func_addr, tsb->tr_functional,
	   tsb->tr_group & ~0x80000000);

    /*
     *  MPC specific error counters
     */
    mpc = &ds->mpc_err;
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

    /*
     * 4R PCI errors counts
     */
    tx = &ds->tx_err;
    tx_prev = &pa4r->tx_prev;
    printf("\n    Microcode TX errors:");
    printf("\n      pci read %d/%d, icdb read parity %d/%d",
	   tx->tx_pci_read_error, tx_prev->tx_pci_read_error,
	   tx->tx_icd_read_parity_error, tx_prev->tx_icd_read_parity_error);
    printf("\n      icdb write parity %d/%d, icdb address parity %d/%d",
	   tx->tx_icd_write_parity_error, tx_prev->tx_icd_write_parity_error,
	   tx->tx_icd_addr_parity_error, tx_prev->tx_icd_addr_parity_error);
    printf("\n      status write %d/%d, fifo underrun %d/%d",
	   tx->tx_stat_write_error, tx_prev->tx_stat_write_error,
	   tx->tx_fifo_underrun, tx_prev->tx_fifo_underrun);
    printf("\n      fifo parity %d/%d",
	   tx->tx_fifo_parity_error, tx_prev->tx_fifo_parity_error);

    rx = &ds->rx_err;
    rx_prev = &pa4r->rx_prev;
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
	   rx->rx_imp_abrt_frm, rx_prev->rx_imp_abrt_frm,rx->rx_exp_abrt_frm, rx_prev->rx_exp_abrt_frm);


    ibm2692_add_counters(hwidb);

    /*
     * SMT stuff
     */
    smt = &pa4r->smt;
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

    printf("\n    Ring sampling: %d/1000\n", pa4r->ring_util);

    idb_release_hwsb_inline(hwidb, HWIDB_SB_TOKEN);
    return automore_quit();
}

/*
 * ibm2692_4r_analyze_pa
 * Init the pci to pci bridge above the port adaptor, then discover devices
 * under the port adaptor.
 */
static void ibm2692_4r_analyze_pa (int pa_bay)
{
    uchar bus_no;
    int num_if_found;
    uchar expected_count;

    if (!ibm2692_rev_okay(pa_bay)) {
	pas_deactivate_bay(pa_bay);
	return;
    }

    bus_no = get_pci_bus_no(pa_bay);

    /*
     * Discover and initialize devices on the PA
     * For now, avoid the problem where config reads find devices for every
     * device number regardless of there being a real hardware interface or
     * not (6E seems to have the same problem) by passing the exact number
     * of expected devices to the discovery function.
     */
    expected_count = 4;
    num_if_found = pci_discover_devices(bus_no, expected_count, pa_bay);

    if (num_if_found != PCI_4R_NUM_INTERFACES) {
	errmsg(&msgsym(DISCOVER, IBM2692), num_if_found, pa_bay);
	pas_deactivate_bay(pa_bay);
	return;
    }

    /*
     * Update slots state
     */
    pas_activate_bay(pa_bay);

    /*
     * Install PA wrapper
     */
    pas_install_wrapper(pa_bay, ibm2692_eim_wrapper);
}

/*
 * build_tx_desc_ring : alloc PCI memory to use as Hermon transmit descriptor
 */
static boolean build_tx_desc_ring (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    int count, i;
    tx_bfr_desc_t	*ptr, *desc;
    tx_desc_shadow_t	*shadow_ptr;

    /*
     * allocate PCI memory for transmit descriptors
     */
    ptr = malloc_pcimem(sizeof(tx_bfr_desc_t)* IBM2692_TX_RING_ENTRIES);
    if (ptr) {
	ds->txr_malloc = ptr;
	for (count = 0; count < IBM2692_TX_RING_ENTRIES; count++) {
	    shadow_ptr = &ds->tx_desc_shadow[count];
	    shadow_ptr->cpu_view_addr = (tx_bfr_desc_t *)CPU_VIEW_ADDR(ptr);
	    shadow_ptr->pci_view_addr = (tx_bfr_desc_t *)PCI_VIEW_ADDR(ptr);
	    desc = (tx_bfr_desc_t *)CPU_VIEW_ADDR(ptr);
	    desc->tx_status = 0x0;
	    desc->bfr_count = 0;
	    desc->frame_length = 0;
	    for (i = 0; i < MAX_TX_BFR_SIZE; i++) {
		desc->bfr[i].bptr = NULL;
		desc->bfr[i].b_count = 0;
	    }
	    ptr++;
	}
	for (i = 0; i < IBM2692_TX_RING_ENTRIES; i++) {
	    ptr = ds->tx_desc_shadow[i].cpu_view_addr;
	    ptr->next = ds->tx_desc_shadow[i + 1].pci_view_addr;
	}
	ptr->next = ds->tx_desc_shadow[0].pci_view_addr;
	ds->tx_inptr = ds->tx_outptr = 0;
	return(TRUE);
    }
    else {
	errmsg(&msgsym(NOPCIMEMORY, IBM2692), idb->slot, idb->subunit);
	return(FALSE);
    }
}

/*
 * build_rx_desc_ring : alloc PCI memory to use as Hermon receive descriptor
 */
static boolean build_rx_desc_ring (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    int	count;
    rx_bfr_desc_t	*ptr, *desc;

    ptr = malloc_pcimem(sizeof(rx_bfr_desc_t) * IBM2692_RX_RING_ENTRIES);
    if (ptr) {
	ds->rxr_malloc = ptr;
        for (count = 0; count < IBM2692_RX_RING_ENTRIES; count++) {
            ds->rx_desc_shadow[count].pci_view_addr = (rx_bfr_desc_t *)PCI_VIEW_ADDR(ptr);
            ds->rx_desc_shadow[count].cpu_view_addr = (rx_bfr_desc_t *)CPU_VIEW_ADDR(ptr);
            desc = (rx_bfr_desc_t *)CPU_VIEW_ADDR(ptr);
            desc->buf_length = 0x0;
	    desc->frame_length = 0x0;
            desc->bptr = NULL;
            desc->rx_status = 0x0;
            ptr++;
        } 
	for (count = 0; count < IBM2692_RX_RING_ENTRIES; count++) {
            ds->rx_desc_shadow[count].cpu_view_addr->next = 
                            ds->rx_desc_shadow[count+1].pci_view_addr;
	}
        ds->rx_desc_shadow[count-1].cpu_view_addr->next =
                            ds->rx_desc_shadow[0].pci_view_addr;
	return(TRUE);
    } else {
	errmsg(&msgsym(NOPCIMEMORY, IBM2692), idb->slot, idb->subunit);
	return(FALSE);
    }
}

/*
 * build_rx_bfr : allocate receive mini buffers and attach it to the receive
 *                descriptors, build the link list structure.
 */
static void build_rx_bfr (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    rx_bfr_desc_t	*temp;
    particletype	*mp;
    int	i;

    for (i = 0; i < IBM2692_RX_RING_ENTRIES; i++) {
        temp = ds->rx_desc_shadow[i].cpu_view_addr;
        temp->rx_status = 0x00000000;	/* initial status field */
	temp->buf_length = IBM2692_PARTICLE_SIZE; 
	temp->frame_length = 0x0;
        if (!temp->bptr) {
            mp = ibm2692_get_particle_inline(idb, TRUE);  /* get particles */
            if (mp) {
                temp->bptr = (uchar *)IBM2692_BUF_ADDR(mp->data_start);
                ds->rx_desc_shadow[i].particle = mp;
            } else {
                temp->bptr = NULL;
                ds->rx_desc_shadow[i].particle = NULL;
		/* 
		 * can't get particles, shut down this interface
		 */ 
                break;
            }
        } else {
	    /*
	     * error condition, should never happen
	     */
	    if (tokenevent_debug) {
                buginf("\n%s: Not empty rx descriptor ring",idb->hw_namestring);
	    }
        }
    }

    ds->rx_start = 0;
    ds->queued_rx_desc = NULL;
    ds->ready_rx_desc = NULL;
}

/*
 * clear_and_free_ibm2692_descriptors : remove all mini buffer
 *                                      attached to tx and rx
 *                                      descriptors.  
 */
static void clear_and_free_ibm2692_descriptors (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    tx_bfr_desc_t	*tp;
    tx_desc_shadow_t	*tds;
    rx_bfr_desc_t *rp;
    rx_desc_shadow_t	*rds;
    int i;

    /*
     * if there are any pending transmit packet in the ring
     * free the mini buffers and the descriptor
     */

    if (tokenevent_debug) {
	buginf("\n%s: Tx outstanding count %d", 
	         idb->hw_namestring, ds->tx_outstanding);
    }

    while (ds->tx_outstanding) {
	tds = &(ds->tx_desc_shadow[ds->tx_outptr]);
        tp = tds->cpu_view_addr;
	if (tokenevent_debug) {
	    buginf("\nTx index %d, status %x", ds->tx_outptr, tp->tx_status);
	}
	tp->tx_status = 0x0;
	if (tokenevent_debug) {
	    buginf("\nparticle count %d", tp->bfr_count);
	}

	/*
	 * check for contigous buffer packet
	 */
	if (tds->tx_pak) {
	    /*
	     * reset buffer pointer and length field
	     */
	    for (i = 0; i < tp->bfr_count-1; i++) {
		tp->bfr[i].bptr = NULL;
		tp->bfr[i].b_count = 0;
	    }
	    /*
	     * return particles to pool
	     */
	    datagram_done(tds->tx_pak);
	    tds->tx_pak = NULL;
        } else {
	    /*
	     * segmented packet, need to return all particles
	     */
            for (i = 0; i < tp->bfr_count-1; i++) {
		if (tds->particle[i]) {
		    retparticle(tds->particle[i]);
		    tds->particle[i] = NULL;
		}
		tp->bfr[i].bptr = NULL;
		tp->bfr[i].b_count = 0;
	    }
	    tp->bfr[i].bptr = NULL;	/* this is the dummy bfr */
	}
	tp->bfr_count = 0;
	tp->frame_length = 0;
	ds->tx_outstanding -= 1;
	ds->tx_outptr++;
	ds->tx_outptr &= IBM2692_TX_RING_INDEX_MASK;
    }
    ds->tx_inptr = ds->tx_outptr = 0;
    ds->tx_outstanding = 0;

    /*
     * free all particles attached to receive descriptors
     */
    ibm2692_cleanup_pak(ds->rx_pak);

    for (i = 0; i < IBM2692_RX_RING_ENTRIES; i++) {
	rds = &(ds->rx_desc_shadow[i]);
        rp = rds->cpu_view_addr;
	if (rds->particle) {
	    retparticle(rds->particle);
	    rds->particle = NULL;
        }
	rp->rx_status = 0x0;
        rp->bptr = NULL;
	rp->frame_length = 0;
	rp->buf_length = 0;
    }
    ds->rx_start = 0;

    /*
     * clear transparent bridging reject address table
     */
    ds->rt_index = 0;
    for (i = 0; i < REJECT_TBL_SIZE; i++) {
	ieee_zero((uchar *)&ds->reject[i]);
    }
}

/*
 * init_hermon_mpc : Initialize a tokenring interface
 */
static void init_hermon_mpc (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    hermon_reg_map *hermon_ptr;
    rx_bfr_desc_t *p;

    hermon_ptr = ds->ibm2692_ptr;
    hermon_ptr->bctl = HERMON_SOFT_RESET;
    usecdelay(HERMON_MPC_RESET_DELAY);
    hermon_ptr->bctl = PCI_FIFO_SIZE; 

    /* 
     * MPC's init buffer and descriptor initialization
     */

    p = (rx_bfr_desc_t *)CPU_VIEW_ADDR(ds->mpc_init_rxdesc);
    p->next = NULL;
    p->rx_status = 0;
    p->bptr = (uchar *)IBM2692_BUF_ADDR(ds->mpc_init_bptr);
    p->buf_length = sizeof(ibm2692_init_buffer_t);

    /*
     * attach buffer descriptor to hermon
     */
    hermon_ptr->rxbda = (rx_bfr_desc_t *)PCI_VIEW_ADDR(ds->mpc_init_rxdesc);

    /*
     * set up interrupt mask regs, when set misr mask register bit 15
     * this will trigger MPC to start initialization sequence
     */
    hermon_ptr->lisr = LOCAL_INT_ENA | LOCAL_INT_MASK | ICDB_PERR_MASK;

    hermon_ptr->sisrm =  SYS_INT_ENA | LAP_PERR_MASK | LAP_ACC_ERR_M | 
			 TRB_RESP | PCI_PERR_MASK | ADP_CHK | SRB_RESP | 
			 ASB_FREE | ARB_CMD | TMR_INT_MASK;

    hermon_ptr->misrm_sum = SYS_INT_ENA | TX2_INT_MASK | RX_EOF_INT |
			    RX_HALT_INT | RX_NOSTA_INT;

    /*
     * enable hermon timer interrupt, with interval set to 500 ms
     */

    hermon_ptr->timer = TIMER_ENABLE | MS_500;

    /*
     * set up proper control flags to track Hermon interrupts
     */
    ds->mpc_init_flag = TRUE;
    ds->srb_in_use = TRUE;

    /*
     * change idb state to init
     */
    net_cstate(idb, IDBS_INIT);
}

/*
 * ibm2692_stop:  Stop the IBM Token Ring chip set and reinitialize it.
 *
 * Must be called with network interfaces disabled.
 */
static void ibm2692_stop (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    hermon_reg_map *hermon_ptr;

    hermon_ptr = ds->ibm2692_ptr;

    /*
     * issue disable request to tx/rx channel
     */
    hermon_ptr->bmctl_sum = TX1_DISABLE_REQ | TX2_DISABLE_REQ | RX_DISABLE_REQ;
}

/*
 * ibm2692_init:  Initialize the interface for an IBM Token Ring Interface.
 */
static void ibm2692_init (hwidbtype *idb)
{
    ibm2692_instance_t *ds;
    tokensb  *tsb;
    boolean tbridge_on ;
    idbtype *swidb ;
    uchar bitswapped_address[IEEEBYTES];

    /*
     * stop the device and count the reset:
     */
    ds = INSTANCE(idb);

    ibm2692_stop(idb);	/* disable transmit/receive channel */

    ibm2692_reset(idb); /* activate icdb reset to reset mpc */

    clear_and_free_ibm2692_descriptors(idb);

    /*
     * reset asb and srb queue control data structure
     */

    ds->asb_inptr = ds->asb_outptr = ds->asb_queue_count = 0;
    ds->srb_inptr = ds->srb_outptr = ds->srb_queue_count = 0;

    idb->counters.resets++;
    idb_mark_unthrottled(idb);
    if (idb->state != IDBS_ADMINDOWN) {
        /*
         * Show that we are being reset:
         */
        net_cstate(idb, IDBS_RESET);
    }

    if (tokenevent_debug) {
        buginf("\n%s: Reset", idb->hw_namestring);
    }

    /*
     * Flush the hold queue:
     */
    holdq_reset(idb);

    /*
     * If interface is administratively down, don't re-enable it.
     * Note that we do this check after shutting down the device
     */
    if (idb->state == IDBS_ADMINDOWN) {
        return;
    }

    /*
     * Validate the hardware address.  If it is invaild, use the
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

    reg_invoke_media_rif_reset(idb);

    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);

    if (IBM2692_LOCAL_BRIDGE) {
        srb_vector_table_t *srbV_p;

	/*
	 * Initialize functional addresses for routed protocols
	 */
	tr_functional_init(idb);
	
        srbV_p = reg_invoke_srb_get_vector_table();
        if (srbV_p && srbV_p->srb_sb_get_bridging_enabled(idb->firstsw)) {
	    idb->tr_ring_mode |= RINGMODE_BRIDGE;
	    tsb->tr_functional |= TRFUNC_BRIDGE;
	} else {
	    idb->tr_ring_mode &= ~RINGMODE_BRIDGE;
	    tsb->tr_functional &= ~TRFUNC_BRIDGE;
	}

	/*
	 * recalc srb params
	 */
        if (srbV_p) {
	    srbV_p->srb_sb_init_rng_br_rng_mask(idb->firstsw);
        }

	/*
	 * Set up for transparent bridging
	 */
        tbridge_on = FALSE ;
        FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
          if (RUNNING_IF(LINK_BRIDGE, swidb) && bridge_enable &&
    	    !(system_loading | bootstrap_enable)) {
              tbridge_on = TRUE ;
              break ;
          }
        }
        if (tbridge_on) {
  	    idb->tr_ring_mode |= RINGMODE_TBRIDGE;
    	    tsb->tr_functional |= TRFUNC_BRIDGE;
        } else {
  	    idb->tr_ring_mode &= ~RINGMODE_TBRIDGE;
        }

	ieee_swap(idb->hardware, bitswapped_address);
        FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
          /*
           * Insert/remove MAC addresses of interest in the SMF.
           */
          reg_invoke_tbridge_smf_update(bitswapped_address,
				      swidb,
				      SMF_MACTYP_OWN_UNICAST |
				      SMF_ACTION_RECEIVE);

          reg_invoke_media_interesting_addresses(swidb);
        }
    }

    /*
     * start initialize token ring interface
     */
    init_hermon_mpc(idb);
    tsb->tr_lineup = TR_KEEP_COUNT;
    TIMER_STOP(tsb->tr_lineuptimer);
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);
}

/*
 * ibm2692_restart : idb->reset vector
 *
 */
static void ibm2692_restart (hwidbtype *idb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * Initialize the hardware addresses.
     */
    ibm2692_get_default_hw_address(idb, idb->bia);
    if (is_ieee_zero(idb->hardware)) {
	ieee_copy(idb->bia, idb->hardware);
    }

    /*
     * We want to make sure the hwidb has a mac address before the
     * system parses through the nvram config, so assign it before
     * the check for "!system_configured" below.  One example of the
     * need for this is the command "xns network <XX>" uses the mac
     * address to derive the XNS address.  Other stuff we'll do after
     * we the config has been parsed.
     */
    if (nvsize && !system_configured) {
	reset_interrupt_level(level);
	return;
    }

    /*
     * flush input and output queues
     */
    holdq_reset(idb);

    if (idb->state != IDBS_ADMINDOWN) {
	/*
	 * Reset interface
	 */
	ibm2692_init(idb);
    } 

    reset_interrupt_level(level);
}

/*
 * ibm2692_shutdown
 * Shut down an interface
 */
static void ibm2692_shutdown (hwidbtype *idb, int state)
{
    leveltype level;
    ibm2692_instance_t *ds = INSTANCE(idb);

    /*
     * change idb state to new state
     */
    net_cstate(idb, state);

    level = raise_interrupt_level(NETS_DISABLE);

    ibm2692_stop(idb);
    ibm2692_reset(idb);
    holdq_reset(idb);
    /*
     * clear bridging flag
     */
    ds->bridge_mode = FALSE;
    ds->tb_mode = FALSE;

    reset_interrupt_level(level);

    clear_and_free_ibm2692_descriptors(idb);
}

/*
 * ibm2692_chip_startup: enable interface to receive & transmit
 */
static void ibm2692_chip_startup (hwidbtype *idb)
{
    ibm2692_instance_t *ds;
    ushort status, control;
    hermon_reg_map *hermon_ptr;
    tokensb	*tsb;

    if (idb->state == IDBS_UP)
        return;

    ds = (ibm2692_instance_t *)INSTANCE(idb);
    hermon_ptr = ds->ibm2692_ptr;
    ds->tx_in_progress = FALSE;
    ds->tx_hw_queue_count = 0;
    ds->rt_index = 0;

    ds->enabled++;
    build_rx_bfr(idb);		/* build receive descriptors */

    /*
     * read bus master control register 
     */
    status = hermon_ptr->bmctl_sum;

    /*
     * if transmit or receive channel are disabled, enable them
     */
    control = 0xffff;
    if (status & RX_DISABLED)
        control &= ~RX_DISABLED;
    if (status & TX2_DISABLED)
        control &= ~TX2_DISABLED;

    /*
     * clear disable bit 
     */
    if (control != 0xffff) {
        hermon_ptr->bmctl_rum = control;
	usecdelay(HERMON_MPC_ENABLE_DELAY);
    }

    status = hermon_ptr->bmctl_sum;
    if (status & (RX_DISABLED | TX2_DISABLED)) {
	/* need to change interface state to disabled */
	if (tokenevent_debug) {
	    buginf("\n%s: BBCtl = %x Can't enable busmaster channel", 
		    idb->hw_namestring, status); 
	}
	return;
    }

    /*
     * attatch receive descriptor link list to Hermon
     */
    hermon_ptr->rxbda = ds->rx_desc_shadow[0].pci_view_addr;
    hermon_ptr->rxlbda = 
        ds->rx_desc_shadow[IBM2692_RX_RING_ENTRIES-1].pci_view_addr;

    tsb = idb_use_hwsb_inline(idb, HWIDB_SB_TOKEN);
    tsb->tr_lineup = TR_KEEP_COUNT;
    idb->keep_count = 0;  /* force periodic to run next time */
    GET_TIMESTAMP(tsb->tr_lineuptimer);
    tsb->tr_ring_status = 0;    
    GET_TIMESTAMP(tsb->tr_status_time);
    idb_release_hwsb_inline(idb, HWIDB_SB_TOKEN);

    idb->counters.transitions++;
}

/*
 * queue_mpc_srb_command: queue a srb command for later processing
 */
static void queue_mpc_srb_command (hwidbtype *idb, srb_cmd_block_t *srb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);

    if (ds->srb_queue_count < MAX_SRB_QUEUE_SIZE) {
	bcopy((uchar *)srb, (uchar *)&ds->srb_queue[ds->srb_inptr],
	      sizeof(srb_cmd_block_t));
	ds->srb_queue_count += 1;
	ds->srb_inptr += 1;
	ds->srb_inptr &= MAX_SRB_QUEUE_MASK;
    } else {
	errmsg(&msgsym(SRBQ_OVERFLOW, IBM2692),  idb->slot, idb->subunit);
    }
}


/*
 * hermon_sreqb_command : Send a command from system to MPC, this is
 *                        async operation.  When MPC completes the
 *                        command it will issue srb_completion interrupt. 
 */
void hermon_sreqb_command (hwidbtype *idb, srb_cmd_block_t *srb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    uchar	cmd;
    boolean	invalid_cmd;
    uint	length, i;
    hermon_reg_map *hermon_ptr;

    hermon_ptr = ds->ibm2692_ptr;

    if (ds->srb_in_use) {
        queue_mpc_srb_command(idb, srb);
        return;
    }

    invalid_cmd = FALSE;
    cmd = srb->nop.command;

    switch (cmd) {
        case CLOSE_ADAPTER :
	    if (tokenevent_debug) {
		buginf("\n%s: CLOSE_ADAPTER srb command issued", 
			  idb->hw_namestring);
	    }
            length = CLOSE_ADAPTER_SIZE;
            break;
        case CONFIG_BRIDGE :
	    if (tokenevent_debug) {
		buginf("\n%s: CONFIG_BRIDGE srb command issued", 
			idb->hw_namestring);
	    }
            length = CONFIGURE_BRIDGE_SIZE;
            break;
        case CFG_HP_CHANNEL :
	    if (tokenevent_debug) {
		buginf("\n%s: CFG_HP_CHANNEL srb command issued", 
			idb->hw_namestring);
	    }
            length = CONFIGURE_HP_CHANNEL_SIZE;
            break;
        case MODIFY_BG_PARM :
	    if (tokenevent_debug) {
		buginf("\n%s: MODIFY_BG_PARM srb command issued", 
			idb->hw_namestring);
	    }
            length = MODIFY_BRIDGE_PARMS_SIZE;
            break;
        case MODIFY_OPEN_OPTION :
	    if (tokenevent_debug) {
		buginf("\n%s: MODIFY_OPEN_OPTION srb command issued", 
			idb->hw_namestring);
	    }
            length = MODIFY_OPEN_OPTIONS_SIZE;
            break;
        case MODIFY_RCV_OPTION :
	    if (tokenevent_debug) {
		buginf("\n%s: MODIFY_RCV_OPTION srb command issued", 
			idb->hw_namestring);
	    }
            length = MODIFY_RECEIVE_OPTIONS_SIZE;
            break;
        case MODIFY_TB_OPTION :
	    if (tokenevent_debug) {
		buginf("\n%s: MODIFY_TB_OPTION srb command issued", 
			idb->hw_namestring);
	    }
            length = MODIFY_TB_OPTIONS_SIZE;
            break;
        case NO_OP :
	    if (tokenevent_debug) {
		buginf("\n%s: NO_OP srb command issued", idb->hw_namestring);
	    }
            length = NO_OPERATION_SIZE;
            break;
        case OPEN_ADAPTER :
	    if (tokenevent_debug) {
		buginf("\n%s: OPEN_ADAPTER srb command issued", 
			idb->hw_namestring);
	    }
            length = OPEN_ADAPTER_SIZE;
            break;
        case READ_LOG :
	    if (PAS_DEBUG_BOOL && tokenevent_debug) {
		buginf("\n%s: READ_LOG srb command issued", idb->hw_namestring);
	    }
            length = READ_LOG_SIZE;
            break;
        case READ_SR_COUNT :
	    if (PAS_DEBUG_BOOL && tokenevent_debug) {
		buginf("\n%s: READ_SR_COUNT srb command issued", 
			idb->hw_namestring);
	    }
            length = READ_SR_COUNTERS_SIZE;
            break;
        case RESET_GROUP_ADDR :
	    if (tokenevent_debug) {
		buginf("\n%s: RESET_GROUP_ADDR srb command issued", 
			idb->hw_namestring);
	    }
            length = RESET_GROUP_ADDRESS_SIZE;
            break;
        case RESET_TARGET_SEG :
	    if (tokenevent_debug) {
		buginf("\n%s: RESET_TARGET_SEG srb command issued", 
			idb->hw_namestring);
	    }
            length = RESET_TARGET_SEGMENT_SIZE;
            break;
        case SAVE_CFG :
	    if (tokenevent_debug) {
		buginf("\n%s: SAVE_CFG srb command issued", idb->hw_namestring);
	    }
            length = SAVE_CONFIGURATION_SIZE;
            break;
        case SET_BRDGE_PARM :
	    if (tokenevent_debug) {
		buginf("\n%s: SET_BRDGE_PARM srb command issued", 
			idb->hw_namestring);
	    }
            length = SET_BRIDGE_PARMS_SIZE;
            break;
        case SET_FUNC_ADDR :
	    if (tokenevent_debug) {
		buginf("\n%s: SET_FUNC_ADDR srb command issued", 
			idb->hw_namestring);
	    }
            length = SET_FUNCTIONAL_ADDRESS_SIZE;
            break;
        case SET_GROUP_ADDR :
	    if (tokenevent_debug) {
		buginf("\n%s: SET_GROUP_ADDR srb command issued", 
			idb->hw_namestring);
	    }
            length = SET_GROUP_ADDRESS_SIZE;
            break;
        case SET_TARGET_SEG :
	    if (tokenevent_debug) {
		buginf("\n%s: SET_TARGET_SEG srb command issued", 
			idb->hw_namestring);
	    }
            length = SET_TARGET_SEGMENT_SIZE;
            break;
        case TB_REJET_ADDR :
	    if (tokenevent_debug) {
		buginf("\n%s: TB_REJET_ADDR srb command issued", 
			idb->hw_namestring);
	    }
            length = TB_REJECT_ADDRESS_SIZE;
            break;
        default:
            length = 0;
            invalid_cmd = TRUE;
	    if (tokenevent_debug) {
		buginf("\n%s: Unknown srb command %x issued", 
			idb->hw_namestring);
	    }
            break;
    }

    /*
     * if valid command send it to MPC through the local access port
     */
    if (!invalid_cmd) {	
	ushort *cmd_buf;
	if (PAS_DEBUG_BOOL && tokenevent_debug) {
	    buginf("\n%s: sending srb cmd to mpc, srb @%x  length %d\n",
		     idb->hw_namestring, ds->srb_loc, length);
	    for (i = 0; i < btow(length); i++) {
	        buginf("%x ",srb->srb_shadow.data[i]);
	    }
	}
	cmd_buf = (ushort *)&srb->srb_shadow.data;
	ibm2692_lap_write(hermon_ptr, MPC_RAM, ds->srb_loc, cmd_buf, length);
        hermon_ptr->lisr_sum = SIF_SRB_CMD;
        ds->srb_in_use = TRUE;
    }
}

/*
 * ibm2692_periodic
 */
static void ibm2692_periodic (hwidbtype *idb)
{
    ibm2692_instance_t *ds;
    srb_cmd_block_t srb_cmd;
    leveltype level;
    init_resp_t	*irtp;
    int length;
    ushort sptr, *dptr;
    hermon_reg_map *hermon_ptr;

    if (idb->state != IDBS_UP)
        return;

    ds = INSTANCE(idb);
    hermon_ptr = ds->ibm2692_ptr;

    /*
     * copy in address table information
     */
    irtp = &(ds->mpc_init_response);
    level = raise_interrupt_level(NETS_DISABLE);
    length = ADDR_TABLE_SIZE;
    sptr = irtp->init_result.addr_table_addr;
    dptr = (ushort *)&ds->mpc_addr_table;
    ibm2692_lap_read(hermon_ptr, MPC_RAM, sptr, dptr, length);
    reset_interrupt_level(level);

    /*
     * update smt parms
     */
    level = raise_interrupt_level(NETS_DISABLE);
    length = PARMS_TABLE_SIZE;
    sptr = irtp->init_result.parms_addr;
    dptr = (ushort *)&ds->mpc_parms_table;
    ibm2692_lap_read(hermon_ptr, MPC_RAM, sptr, dptr, length);
    reset_interrupt_level(level);   

    /*
     * copy in token ring utilization
     */
    level = raise_interrupt_level(NETS_DISABLE);
    sptr = irtp->init_result.ru_count_addr;
    dptr = (ushort *)&ds->mpc_utilization;
    length = 2;
    ibm2692_lap_read(hermon_ptr, MPC_RAM, sptr, dptr, length);
    reset_interrupt_level(level);   

    level = raise_interrupt_level(NETS_DISABLE);
    srb_cmd.read_log.command = READ_LOG;
    srb_cmd.read_log.ret_code = MPC_INITIAL_STAT;
    hermon_sreqb_command(idb, &srb_cmd);
    reset_interrupt_level(level);
}

/*
 * ibm2692_set_maxdgram
 */
static boolean ibm2692_set_maxdgram (hwidbtype *hwidb, int buffersize,
				    int maxdgram)
{
    if_maxdgram_adjust(hwidb, maxdgram);

    return(TRUE);
}

/*
 * enable_mpc_receive : issue modify receive option command
 */
static void enable_mpc_receive (hwidbtype *idb)
{
    ibm2692_instance_t	*ds = INSTANCE(idb);
    srb_cmd_block_t	*ptr;

    ptr = &(ds->mpc_srb);
    ptr->modify_receive_options.command = MODIFY_RCV_OPTION;
    ptr->modify_receive_options.ret_code = MPC_INITIAL_STAT;
    ptr->modify_receive_options.receive_options = ENABLE_LLC_COPY;
    if (idb->tr_ring_mode & RINGMODE_RS) {
	ptr->modify_receive_options.receive_options |= DA_MAC_COPY;
    }
    if (idb->tr_ring_mode & RINGMODE_ALL_MAC) {
	ptr->modify_receive_options.receive_options |= ALL_NODE_MAC_COPY;
    }
    if (idb->tr_ring_mode & RINGMODE_BRIDGE) {
	ptr->modify_receive_options.receive_options &= ~NO_TX_RI_COPY;
	ptr->modify_receive_options.receive_options |= ALL_NODE_MAC_COPY;
    }
    /*
     * if we are doing transparent bridging
     */
    if (idb->tr_ring_mode & RINGMODE_TBRIDGE) {
	/*
	 * disable ASA match copy in tb mode only
	 */
	ptr->modify_receive_options.receive_options |= DISABLE_ASA_LLC_COPY;
        ptr->modify_receive_options.copy_all_options = ENA_TB_COPY;
        ptr->modify_receive_options.password[0] = MPC_PWD1;
        ptr->modify_receive_options.password[1] = MPC_PWD2;
        ptr->modify_receive_options.password[2] = MPC_PWD3;
        ptr->modify_receive_options.password[3] = MPC_PWD4;
    } else {
	ptr->modify_receive_options.copy_all_options = 0x0;
    }
    hermon_sreqb_command(idb, &ds->mpc_srb);
}

/*
 * ibm2692_throttle:
 *
 * The interface is throttled by stopping the Receive side.
 * Whenever the reason for throttling is the lack of receive
 * minibuffers, the pool threshold callback function is responsible
 * for re-enabling the interface, so that it doesn't have to
 * wait for the background process to wake it up after 10 seconds.
 *
 * c7200 will also throttle the interface if the input queue to
 * process level is full.
 * 
 * This function assumes that it's called from interrupt level, so it
 * doesn't disable interrupts.
 */
void ibm2692_throttle (hwidbtype *idb)
{
    srb_cmd_block_t	srb;

    if (idb_is_throttled(idb))
	return;

    /*
     * Mark the interface as throttled
     */
    idb_mark_throttled(idb);

    /*
     * Stop the Rx process.
     *
     * Ideally we would like to just do:
     *
     *    ds->ibm2692_ptr->bmctl_sum = RX_DISABLE_REQ; 
     *
     * But the chipset doesn't seem to respond to that request when it
     * is very busy and as a result we end up with an adapter check.
     * So instead we will issue a system request block to modify the
     * receive options do not receive any frames.   
     */
    srb.modify_receive_options.command = MODIFY_RCV_OPTION;
    srb.modify_receive_options.ret_code = MPC_INITIAL_STAT;
    srb.modify_receive_options.receive_options = DISABLE_ALL_RCV_OPTION;
    hermon_sreqb_command(idb, &srb); 
}

/*
 * ibm2692_enable
 * Enable a IBM2692 interface.  This function "unthrottles" the
 * interface. 
 */
static void ibm2692_enable (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb); 

    if (!idb_is_throttled(idb))
	return;

    /*
     * Mark the interface as enabled (NOT throttled)
     */
    idb_mark_unthrottled(idb);

    /*
     * Reenable the Rx side by issuing a system request block to
     * enabling the receive options that we were using.
     */
    ds->ibm2692_ptr->bmctl_rum = ~RX_DISABLED; 
    enable_mpc_receive(idb);  
}

/*   
 * ibm2692_cache_threshold
 *
 * Callback routine from the buffer management code. We get a callback
 * when the interface private pool reaches our threshold value.
 * This function will reenable the receive side if it has been disabled.
 */  
void ibm2692_cache_threshold (pooltype *pool)
{
    hwidbtype *idb = (hwidbtype *)pool->hwidb;
 
    if (idb_is_throttled(idb))
        ibm2692_enable(idb);
}

/*
 * ibm2692_init_idb - Allocate and initialize an HWIDB and an SWIDB for
 *                  an IBM token ring interface.
 */
static boolean ibm2692_init_idb (hwidbtype *idb, hermon_reg_map *base_addr)
{
    idbtype *swidb = idb->firstsw;
    ibm2692_instance_t *ds;
    int speed;

    /*
     * If this is the first time through this routine
     * for this idb, then initialize the following.
     * For idbs off the removed queue, this initialization
     * should not be repeated.
     */
    if (idb->oir_flags & IDB_FIRST_INIT) {
	speed = 16;
	tring_idb_init(idb, speed);
	idb->status = IDB_TR | IDB_BCAST | IDB_HEARSELF | IDB_BRIDGE;
	idb->type = IDBTYPE_IBM2692;
	idb->typestring = "IBM2692";
	if (pas_oir_active()) {
	    idb->state = IDBS_ADMINDOWN; 
	} else {
	    idb->state = IDBS_RESET; 
	}
	idb->error_count = 0;
	idb->fast_switch_flag = 0;
	idb->rxload = 1;
	idb->span_encapsize = 0;
	idb->pool = NULL;
	idb->pool_group = POOL_GROUP_INVALID;
	swidb->tbridge_media = TBR_CTR;
 	idb->encsize = TRING_SNAP_ENCAPBYTES;

	/*
	 * set up function calls
	 */
	idb->soutput = ibm2692_tx_start;
	idb->reset = ibm2692_restart;
	idb->shutdown = ibm2692_shutdown;
	idb->enable = ibm2692_enable;
	swidb->span_disabled = FALSE;
	idb->show_controller = ibm2692_show_controller;
	idb->fastsend = ibm2692_fastsend;

	if (IBM2692_LOCAL_KEEPALIVE) {
	    /*
	     * The call to tring_idb_init() above will set idb->periodic
	     * equal to tr_periodic().  So if we want keepalives we leave
	     * that set.  For vip we undefine it so that it doesn't do
	     * local keepalives.  The call to periodic in network.c does
	     * check for NULL before diving into it.
	     */
	} else {
	    idb->periodic = NULL;
	}

	idb->device_periodic = ibm2692_periodic; 
	idb->system_configured = ibm2692_restart;
	idb->set_maxdgram = ibm2692_set_maxdgram;

	idb->maxmtu = idb->maxmtu_def = TR_MTU;
	if_maxdgram_adjust(idb, TR_MAXDATA);

        /* 
         * Allocate the SRB subblock here because VIP cannot allocate 
         *   from interrupt level later when needed
         *
         */
        srb_create_sb_for_vip_inline(swidb);

	if (IBM2692_LOCAL_BRIDGE) {

            reg_add_vip_setup_srb(idb->type, ibm2692_restart, 
					    "ibm2692_restart");

            reg_add_vip_modify_srb(idb->type, 
				(srbcore_modify_srb_t *) ibm2692_modify_srb, 
							"ibm2692_modify_srb");

	    idb->setup_bridging = ibm2692_set_bridge_filters;
	    idb->srb_routecache = TRUE;
	    idb->set_extra_addrs = ibm2692_set_addresses;
	    idb->fs_expl_allQ  = (fs_expl_que *) NULL;
	    idb->fs_expl_spanQ = (fs_expl_que *) NULL;
	    idb->fs_expl_input = (fs_expl_que *) NULL;	
	    idb->dot5_get_smt_stats = ibm2692_get_smt_stats;
	    idb->dot5_add_counters = ibm2692_add_counters;
	    idb->dot5_soft_reset = (void *)ibm2692_restart;
	    idb->firstsw->tbridge_media = TBR_TR;
	}
	idb->oir_flags &= ~IDB_FIRST_INIT;
    }

    media_idb_enqueue(idb);

    TIMER_STOP(idb->lastinput);
    TIMER_STOP(idb->lastoutput);
    TIMER_STOP(idb->lastreset);

    /*
     * allocate an instance structure for this device
     */
    ds = malloc_fast(sizeof(ibm2692_instance_t));
    if (!ds) {
	return(FALSE);
    }
    INSTANCE(idb) = (void *)ds;
    ds->ibm2692_ptr = base_addr;
    pas_instance_init_common(&ds->pacb, idb);

    /* 
     * setup the removal vector here so that if the init process
     * fails from here on in, there will be a device specific vector
     * defined to clean up resources
     */
    ds->pacb.pas_deallocate = ibm2692_deallocate;

    /*
     * allocate init descriptor and data buffer
     */
    ds->mpc_init_rxdesc = malloc_pcimem(sizeof(rx_bfr_desc_t));
    if (!ds->mpc_init_rxdesc) {
	return(FALSE);
    }
    ds->mpc_init_bptr = malloc_pcimem(sizeof(ibm2692_init_buffer_t));
    if (!ds->mpc_init_bptr) {
	free(ds->mpc_init_rxdesc);
	return(FALSE);
    }

    /*
     * allocate 4r counters subblock
     */
    idb->vip4r_info = malloc_named(sizeof(vip4r_data_t), "4R stats");
    if (!idb->vip4r_info) {
	return(FALSE);
    }

    /*
     * allocate rx/tx shadow ring
     */
    if (!(build_rx_desc_ring(idb))) {
	return(FALSE);
    }
    if (!(build_tx_desc_ring(idb))) {
	return(FALSE);
    }

    /*
     * do platform dependent initialization
     */
    if (!ibm2692_platform_init(idb)) {
	return(FALSE);
    }


    /* 
     * successfully got all resource
     */
    return(TRUE);
}

/*
 * ibm2692_init_interface
 */
static boolean ibm2692_init_interface (uint slot, uint unit, 
					  uint subunit, hermon_reg_map *base_addr)
{
    hwidbtype *idb;

    /*
     * set up storage add idb to interface queue
     */
    idb = ibm2692_idb_create(slot, subunit);
    if (!idb) {
	return(FALSE);
    }

    /*
     * save unit and slot number
     */
    idb->slot = slot;	/* corresponds to IP */
    idb->unit = unit;	/* nth token ring interface */
    idb->subunit = subunit;	/* viper concept of interface # */
    idb->slotunit = subunit;	/* for slotted interface names */
    pas_set_fci_type(idb, slot); /* Need fci type for OIR support */
    idb->status &= ~IDB_NOHARDWARE;

    /*
     * initialize the idb
     */
    if (!ibm2692_init_idb(idb, base_addr)) {
	return(FALSE);
    }
    if (!idb->hw_namestring) {
	idb_init_names(idb->firstsw, FALSE);
    }

    if (!ibm2692_init_buffer_pools(idb)) {
	if (tokenevent_debug) {
	    buginf("\n%s: slot %d unit %d failed mempool init",
		    idb->hw_namestring, idb->slot, idb->unit);
	}
	return(FALSE);
    }
    return(TRUE);
}

/* 
 * ibm2692_pci_init : perform PCI configuration specific to the IBM
 *                    Lan Streamer chip set
 */
static hermon_reg_map *ibm2692_pci_init (uint pa_bay, uint device_no)
{
    hermon_reg_map *device_base;
    ulong	value;
    uchar bus_no;

    device_base = (hermon_reg_map *)get_pci_device_base(pa_bay, device_no);

    bus_no = get_pci_bus_no(pa_bay);

    /*
     * write to PCI configuration command register, enable busmaster mode
     * IO cycle or memory mapped enable.
     */
    value = get_pci_device_base_offset(pa_bay, device_no);
    if (HERMON_MEMMAPPED_IO) {
	pci_config_write(bus_no, device_no, 0, PCI_MMIO_BASE_OFFSET, value);
	pci_config_write(bus_no, device_no, 0, PCI_COMMAND_REG_OFFSET,
			 PCI_COMMAND_BMEN | PCI_COMMAND_MEMEN |
			 FBB_ENABLE | SERR_ENABLE | PARITY_ENABLE |
			 MWI_ENABLE);
    } else {
	pci_config_write(bus_no, device_no, 0, PCI_IO_BASE_OFFSET, 
			 value & 0xffff);
	pci_config_write(bus_no, device_no, 0, PCI_COMMAND_REG_OFFSET,
			 PCI_COMMAND_BMEN | PCI_COMMAND_IOEN);
    }
    return (device_base);
}

/*
 * ibm2692_analyze_device
 */
static boolean ibm2692_analyze_device (uint pa_bay, uint interface)
{
    hermon_reg_map *device_base;

    /*
     * perform PCI configuration specific to this device.
     * obtain pointer to PCI device base address.
     */
    device_base = (hermon_reg_map *)ibm2692_pci_init(pa_bay, interface);
    if (!device_base) {
	return(FALSE);
    }

    /*
     * initialize the device
     */
    if (!ibm2692_init_interface(pa_bay, nTRs, interface, device_base)) {
	return(FALSE);
    } else {
	nTRs += 1;
	return(TRUE);
    }
}

/*
 * get_init_resp - copy init_resp srb from MPC RAM into system memory
 */
void get_init_resp (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    ushort  length, *ptr, loc;
    hermon_reg_map *hermon_ptr;
    init_resp_t *irp;

    hermon_ptr = ds->ibm2692_ptr;

    length = INIT_RESP_SIZE;
    loc = ds->srb_loc;
    irp = &ds->mpc_init_response;
    ptr = (ushort *)&irp->data[0];
    ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, ptr, length);
    if (irp->init_result.command != INIT_RESP) { /* is this init response? */
	if (tokenevent_debug) {
	    buginf("\n%s: Init error", idb->hw_namestring);
	}
        return;
    } else {	/* MPC completed initialization, get all response code */
	/*
         * copy in mpc micro code version info
         */
        length = UCODE_LEVEL_SIZE;
        loc = irp->init_result.ibm_ucode_level_addr;
        ptr = (ushort *)&ds->mpc_ucode_level; 
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, ptr, length);

        /*
         * check init status, if mpc failed return to caller
         */ 
        if (irp->init_result.bring_up_code) {
	    if (tokenevent_debug) {
		buginf("\n%s: Init bring up code %x", idb->hw_namestring, 
			irp->init_result.bring_up_code);
	    }
            return;
        }

        /*
         * copy in address table information
         */
        length = ADDR_TABLE_SIZE;
        loc = irp->init_result.addr_table_addr;
        ptr = (ushort *)&ds->mpc_addr_table;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, ptr, length);

        /*
         * copy in parameters table
         */
        length = PARMS_TABLE_SIZE;
        loc = irp->init_result.parms_addr;
        ptr = (ushort *)&ds->mpc_parms_table;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, ptr, length);

        /*
         * copy in universal administered address
         */
        length = UAA_ADDR_SIZE;
        loc = irp->init_result.uaa_addr;
        ptr = (ushort *)&ds->mpc_uaa_addr;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, ptr, length);
     }
}

/*
 * do_tx_accounting : book keeping of all error encountered
 */
void do_tx_accounting (ulong stat, ibm2692_instance_t *ds)
{
    tx_errors *tx_err;

    if (stat & TX_ERROR_MASK) { /* transmit error */
	tx_err = &ds->tx_err;
	if (stat & TX_PCPE)
            tx_err->tx_pci_read_error++;	 /* pci read error */
        if (stat & TX_IPRE)
            tx_err->tx_icd_read_parity_error++;	 /* ICDB read parity error */
        if (stat & TX_IWBE)
            tx_err->tx_icd_write_parity_error++; /* ICDB write parity error */
        if (stat & TX_IABE)
            tx_err->tx_icd_addr_parity_error++;	 /* ICDB address parity error */
        if (stat & TX_WER)
            tx_err->tx_stat_write_error++;	 /* status write error */
        if (stat & TX_TFUR)
            tx_err->tx_fifo_underrun++;		 /* tx fifo underrun error */
        if (stat & TX_TPE)
            tx_err->tx_fifo_parity_error++;	 /* tx fifo parity error */
    }
}

/*
 *  set_ibm2692_group_address : set group address
 */

void set_ibm2692_group_address (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    tokensb     *tsb;
    srb_cmd_block_t *ptr;

    tsb = idb_get_hwsb_inline(idb, HWIDB_SB_TOKEN);
    ptr = &(ds->mpc_srb);
    ptr->set_group_address.command = SET_GROUP_ADDR;
    ptr->set_group_address.ret_code = MPC_INITIAL_STAT;
    ptr->set_group_address.group_addr_01 = 0xc000;
    ptr->set_group_address.group_addr_2345 = tsb->tr_group;
    ptr->set_group_address.num_address = 1;
    ptr->set_group_address.group_type = 0x0;
    hermon_sreqb_command(idb, &ds->mpc_srb);
}

/*
 * config_bridge : issue srb command to mpc for bridging operation
 */
void config_bridge (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    srb_cmd_block_t *ptr;

    ptr = &(ds->mpc_srb);
    ptr->configure_bridge.command = CONFIG_BRIDGE;
    ptr->configure_bridge.ret_code = MPC_INITIAL_STAT;
    ptr->configure_bridge.access_priority = BRIDGE_ACCESS_PRIORITY;
    hermon_sreqb_command(idb, &ds->mpc_srb);
}

/*
 * set ibm2692_functional_address : set new functional address 
 */
void set_ibm2692_functional_address (hwidbtype *idb)
{
    srb_cmd_block_t srb, *srbp;
    tokensb		*tsb;
    uchar		*ptr;
    int			i;
    leveltype		level;

    tsb = idb_get_hwsb_inline(idb, HWIDB_SB_TOKEN);
    srbp = &srb;
    srbp->set_functional_address.command = SET_FUNC_ADDR;
    srbp->set_functional_address.ret_code = MPC_INITIAL_STAT;
    ptr = (uchar *)&tsb->tr_functional;
    for (i = 0; i < 4; i++) {
        srbp->set_functional_address.functional_address[i] = *ptr++;
    }
    level = raise_interrupt_level(NETS_DISABLE);
    hermon_sreqb_command(idb, srbp);
    reset_interrupt_level(level);
}

/*
 * do_mpc_open : issue open command
 */
void do_mpc_open (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    int	i;
    srb_cmd_block_t	*ptr;
    tokensb           *tsb;
    uchar             *gptr = NULL;

    tsb = idb_get_hwsb_inline(idb, HWIDB_SB_TOKEN);
    ptr = &(ds->mpc_srb);
    ptr->open_adapter.command = OPEN_ADAPTER;
    if (idb->tr_ring_speed == 16) {
        ptr->open_adapter.ext_open_options = EXPLICIT_CONFIG_PARM |
	    RJ45 | RS16;
    } else {
        ptr->open_adapter.ext_open_options = EXPLICIT_CONFIG_PARM |
	    RJ45;
    }
    if (idb->tr_ring_mode & RINGMODE_ETR) {
	ptr->open_adapter.open_options = 0x0;
    } else {
	ptr->open_adapter.open_options = ETR;
    }
    ieee_copy(idb->hardware, ds->mpc_srb.open_adapter.node_address);

    ptr->open_adapter.group_address[0] = 0xc0;
    ptr->open_adapter.group_address[1] = 0x0;
    gptr = (uchar *)&tsb->tr_group;
    for (i = 2; i < IEEEBYTES; i++) 
	ptr->open_adapter.group_address[i] = *gptr++;
    
    ptr->open_adapter.functional_address = tsb->tr_functional;
    
    ptr->open_adapter.trb_buf_length = 0x0;
    ptr->open_adapter.receive_options = DISABLE_ALL_RCV_OPTION;
    ptr->open_adapter.number_local_addr = 0x1;
    for (i = 0; i < PRODUCT_ID_LEN; i++)
	ptr->open_adapter.product_id[i] = 0x0;

    hermon_sreqb_command(idb, &ds->mpc_srb);
}

/*
 * set_bridge_parameter : set source route bridge parameters
 */
static void set_bridge_parameter (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    srb_cmd_block_t *p;
    srb_triplet_t srb_triplet;
    srb_vector_table_t *srbV_p;

    p = &(ds->mpc_srb);
    ds->bridge_mode = TRUE;
    p->set_bridge_parms.command = SET_BRDGE_PARM;
    p->set_bridge_parms.ret_code = MPC_INITIAL_STAT;

    srbV_p = reg_invoke_srb_get_vector_table();
    if (srbV_p) {
        srbV_p->srb_sb_get_triplet(idb->firstsw, &srb_triplet);
    }
    p->set_bridge_parms.source_ring   = srb_triplet.localRing;
    p->set_bridge_parms.bridge_number = srb_triplet.bridgeNum;
    p->set_bridge_parms.target_ring   = srb_triplet.remoteRing;

    p->set_bridge_parms.multi_port_bridge = 0;
    p->set_bridge_parms.limited_broadcast = ENA_LIMITED_BROADCAST;
    p->set_bridge_parms.inhibit_sr_copy = 0;
    if (idb->tr_ring_mode & RINGMODE_RPS) {
	p->set_bridge_parms.on_board_rps = 1;
    } else {
	p->set_bridge_parms.on_board_rps = 0;
    }
    hermon_sreqb_command(idb, &ds->mpc_srb);
}

/*
 *  modify_ibm2692_bridge_parameters : change one or more bridg options
 */

void modify_ibm2692_bridge_parameters (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    srb_cmd_block_t *p;
    tokensb   *tsb;

    tsb = idb_get_hwsb_inline(idb, HWIDB_SB_TOKEN);
    p = &(ds->mpc_srb);
    p->modify_bridge_parms.command = MODIFY_BG_PARM;
    p->modify_bridge_parms.ret_code = MPC_INITIAL_STAT;
    p->modify_bridge_parms.limited_broadcast = ENA_LIMITED_BROADCAST;
    p->modify_bridge_parms.inhibit_sr_copy = 0;
    if (tsb->tr_functional & TRFUNC_RPS) {
	p->modify_bridge_parms.on_board_rps = 0x1;
    } else {
	p->modify_bridge_parms.on_board_rps = 0;
    }

    hermon_sreqb_command(idb, &ds->mpc_srb);
}

/*
 * enable_tr_bridge : enable transparent bridge mode
 */
static void enable_tr_bridge (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    srb_cmd_block_t *p;

    ds->tb_mode = TRUE;
    p = &(ds->mpc_srb);
    p->modify_tb_option.command = MODIFY_TB_OPTION;
    p->modify_tb_option.ret_code = MPC_INITIAL_STAT;
    p->modify_tb_option.enable_tb_copy = 0x01;
    hermon_sreqb_command(idb, &ds->mpc_srb);
}

/*
 * config_tr_bridge : enable transparent bridging
 */
static void config_tr_bridge (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);

    ds->bridge_mode = TRUE;
    if (idb->tr_ring_mode & RINGMODE_BRIDGE) {
	set_bridge_parameter(idb);
    } else {
	enable_tr_bridge(idb);
    }
}

/*
 * clear_tb_reject_address
 */
static void clear_tb_reject_address (hwidbtype *idb)
{
    srb_cmd_block_t	srb, *srbp;

    srbp = &srb;
    srbp->tb_reject_address.command = TB_REJET_ADDR;
    srbp->tb_reject_address.ret_code = MPC_INITIAL_STAT;
    srbp->tb_reject_address.set_reset_flag = RESET_ALL;
    hermon_sreqb_command(idb, srbp);
}

/*
 * read_sreqb_response : MPC srb command completion interrupt handler,
 *                     copy all information associated with this sreqb
 *                     command from MPC RAM into device instance  
 */
void read_sreqb_response (hwidbtype *idb)
{
    ibm2692_instance_t *ds = INSTANCE(idb);
    uchar	cmd;
    ushort	i, length, loc;
    hermon_reg_map *hermon_ptr;
    mpc_errors *mpc;
    srb_cmd_block_t *p;
    ushort *dest;

    hermon_ptr = ds->ibm2692_ptr;
    p = &(ds->mpc_srb);

    loc = hermon_ptr->lapa = ds->srb_loc;
    dest = (ushort *)&(p->srb_shadow.data[0]);
    /* srb command send to MPC */
    cmd = (swap_16bit_word(hermon_ptr->lapd) >> 8);
 
    switch (cmd) {
    case OPEN_ADAPTER :
	if (tokenevent_debug) {
	    buginf("\n%s: OPEN_ADAPTER srb resp", idb->hw_namestring);
	}
	
	/*
	 * get open response from MPC
	 */
	length = OPEN_RESP_SIZE;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, dest, length);
	
	/*
	 * check for return code, if ok update control data structure
	 */
	if (p->open_response.ret_code == MPC_OK_STAT) {
	    /*
	     * save address of control data structure in ds
	     */
	    ds->asb_loc = p->open_response.asb_addr;
	    ds->srb_loc = p->open_response.srb_addr;
	    ds->arb_loc = p->open_response.arb_addr;
	    ds->trb_loc = p->open_response.trb_addr;
	    /*
	     * check for bridging flag, if transparent bridging set
	     * modify bridging operation 
	     */
	    if (idb->tr_ring_mode & (RINGMODE_TBRIDGE | RINGMODE_BRIDGE)) {
		config_tr_bridge(idb);
	    } else {
		ibm2692_chip_startup(idb);	/* attach bfrs to hermon */
		enable_mpc_receive(idb);
	    }
	} else {
	    if (tokenevent_debug) {
		buginf("\n%s: Open fail return code %x", idb->hw_namestring,
		       ds->mpc_srb.open_response.ret_code);
	    }
	    ibm2692_open_fail(idb);
	    net_cstate(idb, IDBS_DOWN);
	    ibm2692_reset(idb);
	}
	break;
    case READ_LOG :
	if (PAS_DEBUG_BOOL && tokenevent_debug) {
	    buginf("\n%s: READ_LOG srb resp", idb->hw_namestring);
	}
	mpc = &ds->mpc_err;
	length = READ_LOG_SIZE;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, dest, length);
	if (p->read_log.ret_code == MPC_OK_STAT) {
	    mpc->err_line += p->read_log.line_error;
	    mpc->err_internal += p->read_log.internal_error;
	    mpc->err_burst += p->read_log.burst_error;
	    mpc->err_arifci += p->read_log.ari_fci_error;
	    mpc->err_abort += p->read_log.abort_error;
	    mpc->err_lostframe += p->read_log.lost_frame_error;
	    mpc->err_rcvcongestion += p->read_log.receive_congestion;
	    mpc->err_copy += p->read_log.frame_copy_error;
	    mpc->err_token += p->read_log.token_error;
	}
	break;
    case READ_SR_COUNT :
	if (PAS_DEBUG_BOOL && tokenevent_debug) {
	    buginf("\n%s: READ_SR_COUNT srb resp", idb->hw_namestring);
	}
	length = READ_SR_COUNTERS_SIZE;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, dest, length);
	break;
    case TB_REJET_ADDR :
	if (tokenevent_debug) {
	    buginf("\n%s: TB_REJET_ADDR srb resp", idb->hw_namestring);
	}
	length = TB_REJECT_ADDRESS_SIZE;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, dest, length);
	if (p->tb_reject_address.ret_code) {
	    if (tokenevent_debug) {
	        buginf("\n%s: TB_REJET_ADDR failed", idb->hw_namestring);
	    }
	} else {
	    ibm2692_chip_startup(idb);
	    enable_mpc_receive(idb);
	}
	break;
    case CONFIG_BRIDGE:
	if (tokenevent_debug) {
	    buginf("\n%s: CONFIG_BRIDGE srb resp", idb->hw_namestring);
	}
	length = CONFIGURE_BRIDGE_SIZE;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, dest, length);
	if (p->configure_bridge.ret_code) { /* fails */
	    if (tokenevent_debug) {
		buginf("\n%s: failed config bridge srb cmd", idb->hw_namestring);
	    }
	    ibm2692_cfg_brg_fail(idb);
	    net_cstate(idb, IDBS_RESET);
	    ibm2692_reset(idb);
	} else {
	    do_mpc_open(idb);
	}
	break;
    case MODIFY_TB_OPTION:
	if (tokenevent_debug) {
	    buginf("\n%s MODIFY_TB_OPTION srb resp", idb->hw_namestring);
	}
	length = MODIFY_TB_OPTIONS_SIZE;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, dest, length);
	if (p->modify_tb_option.ret_code) {
	    ibm2692_init_fail(idb, p->modify_tb_option.ret_code);
	    net_cstate(idb, IDBS_RESET);
	    ibm2692_reset(idb);
	} else {
	    clear_tb_reject_address(idb);
	}
	break;
    case MODIFY_BG_PARM:
	if (tokenevent_debug) {
	    buginf("\n%s: MODIFY_BG_PARM srb resp", idb->hw_namestring);
	}
	break;
    case MODIFY_RCV_OPTION:
	if (tokenevent_debug) {
	    buginf("\n%s: MODIFY_RCV_OPTION srb resp", idb->hw_namestring);
	}
	length = MODIFY_RECEIVE_OPTIONS_SIZE;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, dest, length);
	if (p->modify_receive_options.ret_code) {
	    if (tokenevent_debug) {
		buginf("\n%s: modify receive option srb ret code %x",
		       idb->hw_namestring,
		       p->modify_receive_options.ret_code);
	    }
	    ibm2692_open_fail(idb); 
	    net_cstate(idb, IDBS_DOWN);
	    ibm2692_reset(idb);
	} else {
	    /*
	     * We can get here either from startup or from re-enabling
	     * a throttled interface.  For the later case we don't
	     * need to call open_okay().
	     */
	    if (idb->state == IDBS_INIT) {
		net_cstate(idb, IDBS_UP);
		rif_update(idb, 0, idb->hardware, NULL, NULL, RIF_INTERFACE);
		ibm2692_open_okay(idb);
	    }
	}
	break;
    case SET_BRDGE_PARM:
	if (tokenevent_debug) {
	    buginf("\n%s: SET_BRDGE_PARM srb resp", idb->hw_namestring);
	}
	length = SET_BRIDGE_PARMS_SIZE;
	ibm2692_lap_read(hermon_ptr, MPC_RAM, loc, dest, length);
	if (p->open_response.ret_code == MPC_OK_STAT) {
	    if (idb->tr_ring_mode & RINGMODE_TBRIDGE) {
		enable_tr_bridge(idb);
	    } else {
		ibm2692_chip_startup(idb);
		enable_mpc_receive(idb);
	    }
	} else {
	    ibm2692_open_fail(idb); 
	    net_cstate(idb, IDBS_DOWN);
	}
	break;
    case SET_FUNC_ADDR:
	if (tokenevent_debug) {
	    buginf("\n%s: SET_FUNC_ADDR srb resp", idb->hw_namestring);
	}
	length = SET_FUNCTIONAL_ADDRESS_SIZE;
	length = length >> 1;
	for (i = 0; i < length; i++) {
	    p->srb_shadow.data[i] = swap_16bit_word(hermon_ptr->lapd_inc);
	}
	if (p->set_functional_address.ret_code != MPC_OK_STAT) {
	    if (tokenevent_debug) {
	        buginf("\n%s: failed set functional address", 
				idb->hw_namestring);
	    }
	}
        break;
    case SET_GROUP_ADDR:
	if (tokenevent_debug) {
	    buginf("\n%s: SET_GROUP_ADDR srb resp", idb->hw_namestring);
	}
	length = SET_GROUP_ADDRESS_SIZE;
	length = length >> 1;
	for (i = 0; i < length; i++) {
	    p->srb_shadow.data[i] = swap_16bit_word(hermon_ptr->lapd_inc);
	}
	if (p->set_group_address.ret_code != MPC_OK_STAT) {
	    if (tokenevent_debug) {
		buginf("\n%s: failed set group address",
			idb->hw_namestring);
	    }
	}
        break;
    default:
	/*
	 * Error! unknown srb command, get command back from MPC
	 * compare with what saved in ds 
	 */
	i = swap_16bit_word(hermon_ptr->lapd); /* read command code */
	if (tokenevent_debug) {
	    buginf("\n%s: Unknown srb resp %x", idb->hw_namestring, i);
	}
	break;
    }  
}

/*
 * ibm2692_subsys_init 
 *
 * Description: ibm2692 subsystem initialisation entry point
 *
 */
static void ibm2692_subsys_init (subsystype *subsys)
{
    /*
     * Register the PCI Token Ring PA
     */
    reg_add_analyze_interface(PA_HARDWARE_4R,
                              ibm2692_4r_analyze_pa,
                              "ibm2692_4r_analyze_pa");

    reg_add_pci_device_init(IBMTR_VENDOR_DEVICE_ID,
                            ibm2692_analyze_device,
                            "ibm2692_analyze_device");
}

/*
 * ibm2692 subsystem header
 */

#define IBM2692_MAJVERSION   1
#define IBM2692_MINVERSION   0
#define IBM2692_EDITVERSION  1

SUBSYS_HEADER(ibm2692, IBM2692_MAJVERSION, IBM2692_MINVERSION,
              IBM2692_EDITVERSION,
              ibm2692_subsys_init, SUBSYS_CLASS_DRIVER,
              NULL,
              NULL);

/* end of file */
