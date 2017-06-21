/* $Id: rsp_init.c,v 3.15.4.13 1996/08/30 06:58:55 mcieslak Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_init.c,v $
 *------------------------------------------------------------------
 * RSP initialization
 *
 * March 1994, Chris Shaker
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_init.c,v $
 * Revision 3.15.4.13  1996/08/30  06:58:55  mcieslak
 * CSCdi67677:  Need to enhance RSP cache and watch test commands
 * Branch: California_branch
 *
 * Revision 3.15.4.12  1996/08/28  13:14:07  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.15.4.11  1996/07/25  00:32:53  ganesh
 * CSCdi63845:  Slave OIR reload fix in CSCdi57076 fails on rsp1, rsp7000.
 * Branch: California_branch
 * Added rsp_dual_capable() routine, that returns TRUE if the system
 * is dual capable, FALSE otherwise.
 *
 * Revision 3.15.4.10  1996/07/17  18:15:28  ganesh
 * CSCdi57076:  HSA Slave excision/removal crashes 7513--Router
 * RELOADS-OIR Fails
 * Branch: California_branch
 * During slave OIR, peer bits in ms_status register may tell us that
 * slave is present eventhough slave is actually removed!! Software
 * workaround is to do a touch_device of slave, and if it fails assume
 * slave is unplugged and proceed.
 *
 * Revision 3.15.4.9  1996/07/09  21:15:18  getchell
 * Modify read bus exception handler and DCL interrrupt handler to deal
 * with dbus accesses to non-existent card.
 * CSCdi58555:  bus exception caused by dbus access to non-existant card
 * Branch: California_branch
 *
 * Revision 3.15.4.8  1996/06/21  18:21:05  kmitchel
 * CSCdi47923:  DBUS-3-NORESP DBUS-3-BADCTRLRTYPE msgs when booting on RSP
 * Branch: California_branch
 *
 * Revision 3.15.4.7  1996/05/21  10:04:38  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.15.4.6  1996/05/21  06:40:28  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.15.4.5  1996/05/18  17:53:56  kao
 * CSCdi54117:  vip2 looses it with serial encaps other than HDLC
 * reserve the last memd bufhdr for a special s/w work-around on vip2 H/W
 * Branch: California_branch
 *
 * Revision 3.15.4.4  1996/04/24  00:07:02  ahh
 * CSCdi55483:  NAT: code review comments, various bug fixes
 * Fast-path fixup, cruft removal.
 * Branch: California_branch
 *
 * Revision 3.15.4.3  1996/04/19  15:31:32  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.9  1996/04/09  21:58:19  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.8  1996/03/19  08:44:12  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 2.1.6.7  1996/02/21  04:08:56  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.6  1996/01/23  06:50:02  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.5  1996/01/09  17:14:22  xliu
 * Branch: IosSec_branch
 * Moved crypto_engine_rsp_init.
 *
 * Revision 2.1.6.4  1995/12/19  04:17:06  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.3  1995/12/17  02:21:33  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.2  1995/11/30  08:18:55  xliu
 * Branch: IosSec_branch
 *
 * Add crypto engine rsp init.
 *
 * Revision 2.1.6.1  1995/11/17  18:16:58  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.15.4.2  1996/03/27  07:47:51  dbath
 * CSCdi51848:  Pulled slave remains in chassis MIB.
 * Branch: California_branch
 * Rip the slave out of the chassis MIB once we hear
 * the user has ripped it out of the chassis.
 *
 * Revision 3.15.4.1  1996/03/18  22:02:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.2.4  1996/03/16  07:36:38  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.2.3  1996/03/09  05:13:06  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.11.2.2  1996/03/07  10:45:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.2.1  1996/02/20  18:01:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.15  1996/03/09  05:01:23  foster
 * CSCdi50349:  HSA flashmib is not happy after slave reload
 * flashmid needs to wait until IPC is up before ask slave anything
 *
 * Revision 3.14  1996/03/09  01:18:59  hampton
 * Migrate processor specific files to src-xxx directories.  [CSCdi51128]
 *   Step 2: Update all files which reference the moved files to point to
 *   their new locations.
 *
 * Revision 3.13  1996/02/15  20:04:12  dbath
 * CSCdi49109:  HSA bootldr is noisy about slave state changes.
 * Don't whinge about slave state changes while we're netbooting.
 *
 * Revision 3.12  1996/02/04  00:49:15  foster
 * CSCdi47925:  HSA write mem cannot update slaves config file.
 *
 * Revision 3.11  1996/01/18  07:08:13  schaefer
 * CSCdi42858: %RSP-3-ERROR CyBus0 master error/04 page map parity error
 * disable read-around-write when initializing pagemap entries
 *
 * Revision 3.10  1996/01/18  06:48:10  foster
 * CSCdi47104:  HSA proms need some timing changes in system code
 * have the slave set software ready after it comes up.
 *
 * Revision 3.9  1996/01/18  00:21:32  dbath
 * CSCdi42471:  rsp2 in 7505 offers false hope of dual operation.
 * Don't chain in HSA stuff if we're an rsp2 running in a 7505 chassis.
 * While the bonnet's up, fix the setting of rsp_self_slot, to prevent
 * dbus bus errors when an rsp2 is installed in a 7505 chassis.
 *
 * Revision 3.8  1996/01/15  18:05:15  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.7  1996/01/06  03:23:22  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.6  1995/12/18  07:09:12  schaefer
 * CSCdi45771: include slave devices in 75xx flash mib
 *
 * Revision 3.5  1995/12/18  05:50:55  schaefer
 * CSCdi42858: %RSP-3-ERROR CyBus0 master error/04 page map parity error
 *
 * Revision 3.4  1995/12/17  18:38:23  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/27  21:34:09  yanke
 * Use cbus protection mechanism to guard slave rsp from crashing
 * during cbus stalled condition.
 * Recover ipc operation on master rsp when stall condition goes away.
 *
 * CSCdi44478:  slave rsp crashes or resets during ips eoir events
 *
 * Revision 3.2  1995/11/17  18:48:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/11/08  21:28:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.11  1995/09/26  21:33:01  motto
 * Decouple IPC from RSP subsystem.
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.10  1995/09/23  07:11:51  dbath
 * CSCdi40941:  Only 1/4 of MEMD initialised.
 * Now initialise all of MEMD, and all of the buffer header area, and
 * slam it in 64 bits at a time, while we're at it.
 *
 * Revision 2.9  1995/09/17  20:55:34  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.8  1995/08/10  19:12:04  schaefer
 * CSCdi38464: need branch patch in 11.0
 * don't crash if two RSPs are installed
 *
 * Revision 2.7  1995/08/05  01:57:29  mcieslak
 * Remove unused code and adjust MC ASIC definitions.
 * CSCdi38262:  Need to clean-up code review issues found in alaska merge
 *
 * Revision 2.6  1995/08/04  02:47:55  mcieslak
 * Fixed the initialization of the MEMD control register to
 * not make any assumptions of its initial state. Removed
 * unused #define relating to this register.
 * CSCdi37853:  CBUS-3-CCBPTIMEOUT with tracebacks
 *
 * Revision 2.5  1995/08/03  21:53:14  mcieslak
 * The DCL ASIC was not getting configured correctly to use the write
 * buffering and other features.
 * CSCdi35816:  RSP2 image vs RSP1 image performance disparity on RSP1 and
 * RSP2 hw
 *
 * Revision 2.4  1995/07/18  18:01:36  mcieslak
 * Added some additional Master/Slave support for Dual RSP2's.
 * Ensure that the Master/Slave interrupts are masked during
 * initialization.
 * CSCdi37206:  Dual processor support needs work
 *
 * Revision 2.3  1995/07/16  22:31:04  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.2  1995/06/23  18:48:40  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.1  1995/06/07  22:58:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include "config.h"
#include "interface_private.h"
#include "subsys.h"
#include "logger.h"
#include "../src-rsp/msg_rsp.c"		/* Not a typo, see logger.h */
#include "../src-rsp/msg_chasint.c"	/* Not a typo, see logger.h */
#include "stacks.h"
#include "ttysrv.h"
#include "sys_registry.h"
#include "../dev/monitor1.h"
#include "../os/free.h"
#include "../src-4k/gdb_4k.h"
#include "../src-4k/alignment.h"

#include "rsp_macaddr.h"
#include "rsp_bufhdr.h"
#include "rsp_if.h"
#include "rsp_qasic.h"
#include "qa_hw.h"
#include "../os/timer_generic.h"
#include "../hes/ccb_cmd.h"
#include "../hes/if_hes_common.h"

#include "config.h"
#include "../os/nv.h"
#include "../filesys/filesys.h"
#include "../filesys/flash_config.h"
#include "../filesys/remote_filesys.h"
#include "../dev/flash_registry.h"

#include "rsp_pcmap.h"
#include "rsp_chasint.h"
#include "../asm-mips/asm_4k_rsp.h"
#include "../src-4k/signal_4k.h"
#include "../hes/dgbus.h"
#include "../hes/dbus.h"
#include "../hes/if_fci.h"
#include "../hes/cbus_registry.h"
#include "../os/sum.h"
#include "sys_registry.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "rsp_init.h"
#include "ms_xchng.h"
#include "rsp_flashmib.h"
#include "../snmp/chassismib_registry.h"
#include "rsp_cbus_monitor.h"

/*
 * Private data
 */
static uchar rsp_peer_slot;		/* slot number for peer processor */
static uchar rsp_self_slot;		/* slot number for this processor */

static ulong pmem_size;
static uchar reset_reason;
static tinybool rsp7000, i_am_master;

static volatile ushort peer_hw_status;
static slave_state_t slave_state;

/*
 * Globals
 */

qa_asic_t	*qasic_base;		/* Pointer to QASIC in uncached MEMD
					   address space */
qa_diag_t	*qasic_diag_base;	/* Pointer to QASIC Diag space in
					   uncached MEMD address space */

bufhdr_t	*bufhdr_base;		/* Start of buffer headers */
uint		bufhdr_count;		/* number of buffer headers */

ccbtype		*ccb_base;		/* array of CCBs */

uchar		*memd_base;		/* Base address of MEMD */

uint		memd_size;		/* size of MEMD in bytes */

boolean		watch_action_crash = FALSE; /* Action to take on a watch
					       exception. */

boolean         memd_parity_error = FALSE; /* MEMD write parity error, 
					      TRUE means recarve MEMD, 
					      restart interfaces */

/*
 * Private definitions
 */
typedef struct {
    uchar cyerr;
    uchar cycmd;
    uchar cymisc;
    uint phy_addr;
    uint virt_addr;
} cyerr_bus;
typedef struct {
    cyerr_bus bus[2];

    /* RSP1 only. */
    ushort cydatparerr;

    /* RSP2 only. */
    uchar mastererr;
    uchar intstatus;
    uchar intmask;
    union {
	ushort reg[4];
	u64 all;
    } MD_error_log;
} cyerr_all;

/* cyerr_bus_report print output flags. */
#define	CBR_APBYTE	0x0001
#define CBR_CMD		0x0002
#define CBR_DINPBYTE	0x0004
#define CBR_DOUTPBYTE	0x0008
#define CBR_OFFSET	0x0010
#define CBR_PADDR	0x0020
#define CBR_VADDR	0x0040


static char*
cycmd_string (uchar cycmd)
{
    char *str;

    switch (cycmd) {
      case CYCMD_READ_8B:	str = "read 8bytes";	break;
      case CYCMD_READ_4B:	str = "read 4bytes";	break;
      case CYCMD_WRITE_8B:	str = "write 8bytes";	break;
      case CYCMD_WRITE_4B:	str = "write 4bytes";	break;
      case CYCMD_WRITE_2B:	str = "write 2bytes";	break;
      case CYCMD_WRITE_1B:	str = "write 1byte";	break;
      default:			str = "invalid";	break;	
    }
    return str;
}

static void
print_cybus_details (cyerr_all* pcyerr_all, uint bus, ushort printout)
{
    cyerr_bus *pcyerr_bus = &pcyerr_all->bus[bus];

    /*
     * Set the memd_parity_error to indicate that memd may
     * be corrupt with bad parity data.  This flag is checked
     * in a variety of places to prevent access to corrupt
     * data. This case handles RSP1 MEMD parity errors and some
     * RSP2 MEMD parity errors.
     */

    memd_parity_error = TRUE;

    if (printout & CBR_APBYTE) {
	errvarmsg(&msgsym(ERROR, RSP),
		  "  address parity error 23:16 %u, 15:8 %u, 7:0 %u",
		  (pcyerr_bus->cymisc & CYMISC_CYADDRPAR23_16) != 0,
		  (pcyerr_bus->cymisc & CYMISC_CYADDRPAR15_8) != 0,
		  (pcyerr_bus->cymisc & CYMISC_CYADDRPAR7_0) != 0);
    }

    if (printout & CBR_CMD) {
	errvarmsg(&msgsym(ERROR, RSP), "  bus command %s (%#x)",
		  cycmd_string(pcyerr_bus->cycmd), pcyerr_bus->cycmd);
    }

    if ((printout & CBR_DINPBYTE) && !rsp2) {
	errvarmsg(&msgsym(ERROR, RSP), "  write parity error (bytes 0:7) %02x",
		  CYDATPAR_WRITE(pcyerr_all->cydatparerr));
    }

    if ((printout & CBR_DOUTPBYTE) && !rsp2) {
	errvarmsg(&msgsym(ERROR, RSP), "  read parity error (bytes 0:7) %02x",
		  CYDATPAR_READ(pcyerr_all->cydatparerr));
    }

    if (printout & CBR_OFFSET) {
	if (!rsp2) {
	    errvarmsg(&msgsym(ERROR, RSP), "  address offset (bits 2:0) %u",
		      CYMISC_PA2_0(pcyerr_bus->cymisc));
	} else {
	    errvarmsg(&msgsym(ERROR, RSP), "  address offset (bits 3:1) %u",
		      CYMISC_PA3_1(pcyerr_bus->cymisc) << 1);
	}
    }

    if (printout & CBR_PADDR) {
	errvarmsg(&msgsym(ERROR, RSP),
	    (!rsp2) ? "  physical address (bits 20:15) %06x" :
	    "  physical address (bits 20:12) %06x", pcyerr_bus->phy_addr);
    }

    if (printout & CBR_VADDR) {
	errvarmsg(&msgsym(ERROR, RSP), "  virtual address (bits 23:17) %06x",
		  pcyerr_bus->virt_addr);
    }
}

static int
mderr_report (cyerr_all* pcyerr_all)
{
    uint badbytes, badhigh, badlow;

    /* Determine if there are MD errors. */
    if (!pcyerr_all->MD_error_log.all)
	return 0;			/* crash not necessary */

    /* Indicate the MD errors we are dealing with. */
    errvarmsg(&msgsym(ERROR, RSP), "MD error %04x%04x%04x%04x", 
	pcyerr_all->MD_error_log.reg[0], pcyerr_all->MD_error_log.reg[1],
	pcyerr_all->MD_error_log.reg[2], pcyerr_all->MD_error_log.reg[3]);

    badbytes = (MDERR_MC_CTRL(pcyerr_all->MD_error_log.reg[0]) << 3) |
	MDERR_MC_CTRL(pcyerr_all->MD_error_log.reg[1]);
    if (badbytes) {
	errvarmsg(&msgsym(ERROR, RSP), "  MC Control parity error %02x",
		  badbytes);
    }

    badhigh = MDERR_RP_ERRS(pcyerr_all->MD_error_log.reg[0]);
    badlow = MDERR_RP_ERRS(pcyerr_all->MD_error_log.reg[1]);
    badbytes = MDERR_MERGE_HIGH_LOW(badhigh, badlow);
    if (badbytes) {
	errvarmsg(&msgsym(ERROR, RSP), "  RP parity error (bytes 0:7) %02x",
		  badbytes);
    }

    badhigh = MDERR_SRAM_ERRS(pcyerr_all->MD_error_log.reg[2]);
    badlow = MDERR_SRAM_ERRS(pcyerr_all->MD_error_log.reg[3]);
    badbytes = MDERR_MERGE_HIGH_LOW(badhigh, badlow);
    if (badbytes) {
	errvarmsg(&msgsym(ERROR, RSP), "  SRAM parity error (bytes 0:7) %02x",
		  badbytes);

	/*
	 * Set the memd_parity_error to indicate that
	 * memd is corrupt with bad parity data.  This flag is
	 * checked in a variety of places to prevent access to corrupt
	 * data. This case handles RSP2 SRAM write parity errors.
	 */

	memd_parity_error = TRUE;
    }

    badhigh = MDERR_QA_ERRS(pcyerr_all->MD_error_log.reg[2]);
    badlow = MDERR_QA_ERRS(pcyerr_all->MD_error_log.reg[3]);
    badbytes = MDERR_MERGE_HIGH_LOW(badhigh, badlow);
    if (badbytes) {
	errvarmsg(&msgsym(ERROR, RSP), "  QA parity error (bytes 0:3) %02x",
		  badbytes);

	/*
	 * Set the memd_parity_error to indicate that
	 * memd is corrupt with bad parity data.  This flag is
	 * checked in a variety of places to prevent access to corrupt
	 * data. This case handles RSP2 QA write parity errors.
	 */

	memd_parity_error = TRUE;	
    }

    badhigh = MDERR_CY0_ERRS(pcyerr_all->MD_error_log.reg[2]);
    badlow = MDERR_CY0_ERRS(pcyerr_all->MD_error_log.reg[3]);
    badbytes = MDERR_MERGE_HIGH_LOW(badhigh, badlow);
    if (badbytes) {
	errvarmsg(&msgsym(ERROR, RSP),
		  "  Cybus0 parity error (bytes 0:7) %02x", badbytes);
	print_cybus_details(pcyerr_all, 0, CBR_CMD | CBR_VADDR | CBR_PADDR);
    }

    badhigh = MDERR_CY1_ERRS(pcyerr_all->MD_error_log.reg[2]);
    badlow = MDERR_CY1_ERRS(pcyerr_all->MD_error_log.reg[3]);
    badbytes = MDERR_MERGE_HIGH_LOW(badhigh, badlow);
    if (badbytes) {
	errvarmsg(&msgsym(ERROR, RSP),
		  "  Cybus1 parity error (bytes 0:7) %02x", badbytes);
	print_cybus_details(pcyerr_all, 1, CBR_CMD | CBR_VADDR | CBR_PADDR);
    }

    return 0;			/* crash not necessary */
}

static int
cyerr_bus_report (cyerr_all* pcyerr_all, uint bus)
{
    cyerr_bus *pcyerr_bus = &pcyerr_all->bus[bus];
    ushort printout = 0;
    uchar cyerr = pcyerr_bus->cyerr;
    uchar mastererr = pcyerr_all->mastererr;

    /* Determine if there are any errors on the bus. */
    mastererr &= (bus == 0) ? CYMERR0_MASK : CYMERR1_MASK;
    if (!cyerr && !mastererr)
	return 0;			/* crash not necessary */

    /* Indicate the Cybus errors we are dealing with. */
    if (cyerr) {
	if (rsp2) {
	    errvarmsg(&msgsym(ERROR, RSP), "CyBus%d error %02x", bus, cyerr);
	} else {
	    errvarmsg(&msgsym(ERROR, RSP), "CyBus error %02x", cyerr);
	}

	/* Decode the CyErr register. */
	if (cyerr & CYERR_DATPAR_MEMDRD) {
	    errvarmsg(&msgsym(ERROR, RSP), "    read data parity");
	    printout |= CBR_DOUTPBYTE | CBR_PADDR;
	}
	if (cyerr & CYERR_BADWRITEDAT_PM) {
	    errvarmsg(&msgsym(ERROR, RSP), "    bad page map write data");
	    printout |= CBR_VADDR;
	}
	if (cyerr & CYERR_PM_PARITY) {
	    errvarmsg(&msgsym(ERROR, RSP), "    page map parity error");
	    printout |= CBR_CMD | CBR_VADDR;
	}
	if (cyerr & CYERR_PM_INVALIDREG) {
	    errvarmsg(&msgsym(ERROR, RSP), "    invalid page map register");
	    printout |= CBR_CMD | CBR_VADDR;
	}
	if (cyerr & CYERR_CMDADDR) {
	    errvarmsg(&msgsym(ERROR, RSP), "    command/address mismatch");
	    printout |= CBR_CMD | CBR_OFFSET | CBR_VADDR;
	}
	if (cyerr & CYERR_INVALIDCMD) {
	    errvarmsg(&msgsym(ERROR, RSP), "    invalid command");
	    printout |= CBR_CMD | CBR_VADDR;
	}
	if (cyerr & CYERR_ADDRPARITY) {
	    errvarmsg(&msgsym(ERROR, RSP), "    address parity error");
	    printout |= CBR_APBYTE | CBR_CMD | CBR_VADDR;
	}
	if (cyerr & CYERR_DATAPARITY) {
	    errvarmsg(&msgsym(ERROR, RSP), "    receive data parity error");
	    printout |= CBR_DINPBYTE | CBR_PADDR | CBR_VADDR;
	}
    }

    /* Indicate the Cybus Master errors for the bus. */
    if (mastererr) {
	errvarmsg(&msgsym(ERROR, RSP), "CyBus%d master error", bus);
	printout |= CBR_CMD | CBR_PADDR;

	/* Decode the CyBus Master register. */
	if (mastererr & (CYMERR0_NO_ACK_NACK | CYMERR1_NO_ACK_NACK)) {
	    errvarmsg(&msgsym(ERROR, RSP), "    no ACK/NACK or both");
	}
	if (mastererr & (CYMERR0_NACK | CYMERR1_NACK)) {
	    errvarmsg(&msgsym(ERROR, RSP), "    NACK response");
	}
	if (mastererr & (CYMERR0_INVALIDCMD | CYMERR1_INVALIDCMD)) {
	    errvarmsg(&msgsym(ERROR, RSP), "    invalid command");
	}
	if (mastererr & (CYMERR0_ARB_TIMEOUT | CYMERR1_ARB_TIMEOUT)) {
	    errvarmsg(&msgsym(ERROR, RSP), "    arbitration timeout");
	}
    }

    /* Print out appropriate information for the error(s). */
    print_cybus_details(pcyerr_all, bus, printout);

    return 0;				/* crash not necessary */
}

static int
cyerr_report (uchar* memderr)
{
    cyerr_all cyerr_data;
    volatile uchar* reg;
    volatile ushort* sreg;
    volatile u64* u64reg;
    volatile uchar *netmask;
    qa_hwq_t *temp_RawQ;
    int      qindex;

    /*
     * Collect the cyerr data for appropriate type of RSP.
     * Need to read these first, since reading ADRSPC_MEMD_ERRSTATUS
     * clears the latched error condition and allows information about
     * subsequent cycles to be saved in these registers.
     */       
    memset(&cyerr_data, 0, sizeof(cyerr_all));
    if (rsp2) {
	/* Collect CyBus0 */
	reg = (uchar*) ADRSPC_CYVA23_17;
	cyerr_data.bus[0].virt_addr = CYVIRTADDR23_17(*reg) << 17;

	reg = (uchar*) ADRSPC_CYPHYSADDR;
	cyerr_data.bus[0].phy_addr = CYPHYSADDR20_13(*reg) << 13;

	reg = (uchar*) ADRSPC_CYMISC;
	cyerr_data.bus[0].cymisc = *reg;
	cyerr_data.bus[0].phy_addr |=
	    CYMISC_PA12(cyerr_data.bus[0].cymisc) << 12;

	reg = (uchar*) ADRSPC_CYERRCMD;
	cyerr_data.bus[0].cycmd = CYERRCMD_CMD(*reg);

	reg = (uchar*) ADRSPC_CYERR;
	cyerr_data.bus[0].cyerr = *reg;

	/* Collect CyBus1 */
	reg = (uchar*) ADRSPC_CY1VA23_17;
	cyerr_data.bus[1].virt_addr = CYVIRTADDR23_17(*reg) << 17;

	reg = (uchar*) ADRSPC_CY1PHYSADDR;
	cyerr_data.bus[1].phy_addr = CYPHYSADDR20_13(*reg) << 13;

	reg = (uchar*) ADRSPC_CY1MISC;
	cyerr_data.bus[1].cymisc = *reg;
	cyerr_data.bus[1].phy_addr |=
	    CYMISC_PA12(cyerr_data.bus[1].cymisc) << 12;

	reg = (uchar*) ADRSPC_CY1ERRCMD;
	cyerr_data.bus[1].cycmd = CYERRCMD_CMD(*reg);

	reg = (uchar*) ADRSPC_CY1ERR;
	cyerr_data.bus[1].cyerr = *reg;

	/* Collect MD error info. */
	u64reg = (u64 *) ADRSPC_MDERRINFO;
	move64((u64 *) u64reg, &cyerr_data.MD_error_log.all);

	/* Collect Cybus common data. */
	reg = (uchar*) ADRSPC_CYMASTER_ERR;
    	cyerr_data.mastererr = *reg;

	reg = (uchar*) ADRSPC_MC_INTR_STATUS;
	cyerr_data.intstatus = *reg;

	reg = (uchar*) ADRSPC_MC_INTR_MASK;
	cyerr_data.intmask = *reg;
    } else {
	/* Only a single CyBus on RSP1. */
	reg = (uchar*) ADRSPC_CYVA23_17;
	cyerr_data.bus[0].virt_addr = CYVIRTADDR23_17(*reg) << 17;

	reg = (uchar*) ADRSPC_CYPHYSADDR;
	cyerr_data.bus[0].phy_addr = CYPHYSADDR20_16(*reg) << 16;

	reg = (uchar*) ADRSPC_CYMISC;
	cyerr_data.bus[0].cymisc = *reg;
	cyerr_data.bus[0].phy_addr |=
	    CYMISC_PA15(cyerr_data.bus[0].cymisc) << 15;

	reg = (uchar*) ADRSPC_CYERRCMD;
	cyerr_data.bus[0].cycmd = CYERRCMD_CMD(*reg);

	sreg = (ushort*) ADRSPC_CYDATPARERR;
	cyerr_data.cydatparerr = *sreg;

	reg = (uchar*) ADRSPC_CYERR;
	cyerr_data.bus[0].cyerr = *reg;
    }

    reg = (uchar*) ADRSPC_MEMD_ERRSTATUS;
    *memderr = *reg;
    if ((*memderr & MEMDERR_CYMEMD) == 0)
	return 0;		/* no error, no need to crash */

    if (rsp2) {
	mderr_report(&cyerr_data);
    }
    cyerr_bus_report(&cyerr_data, 0);
    if (rsp2) {
	cyerr_bus_report(&cyerr_data, 1);
    }

    /*
     * If a memd write parity error has occurred, memd may be
     * corrupt.  We must disable Level 1/2 interrupts until a
     * cybus complex restart is complete.  This will prevent
     * access to corrupt memd data for subsequent Level 1/2
     * interrupts.  These two registers are reset during
     * a cbus complex restart.  In addition, write a 0 to
     * the RawQ to cause a QA error and clear out the queues.
     * This will remove the links to the corrupt data.
     */

    if (memd_parity_error) {
      qindex = get_hwq(QT_RAW, 1);
      temp_RawQ = queue_ptr(qindex);
      hwq_enqueue(temp_RawQ, (bufhdr_t*)0x0);
      netmask = (uchar*) ADRSPC_QA_NETINT_MASK;
      *netmask = QA_INT_DISABLE;
      netmask = (uchar*) ADRSPC_QA_CMDINT_MASK;
      *netmask = QA_INT_DISABLE;
      errvarmsg(&msgsym(ERROR, RSP), "MEMD parity error condition");
    }

    platform_req_complex_restart(NULL);

    return 0;				/* crash not necessary */
}

static int
memderr_report (r4k_context_t *cp)
{
    int crash;
    uchar memderr;

    crash = 0;

    crash |= cyerr_report(&memderr);

    if (memderr & MEMDERR_CHASINT)
	crash |= chasint_interrupt();

    if (memderr & MEMDERR_QA)
	crash |= rsp_qa_error_interrupt(cp);

    return crash;
}

/*
 * Handle interrupts from the dbus fpga.  Currently, only master/slave
 * related interrupts are handled.  MC ASIC errors are already taken
 * care of in cyerr_report().  This routine will only be called on
 * rsp2 processors, so it's not necessary for this routine to do
 * further checks of processor type.
 */
static int
fpgaerr_report (void)
{
    static char* const status_string[] = {
	"Non-participant",
	"Slave",
	"Master",
	"Unplugged",
    };
    int crash;
    volatile ushort* sreg;
    ushort fpgaerr, ms_status;

    sreg = (ushort *)ADRSPC_FPGA_INTR_STATUS;
    fpgaerr = *sreg;

    crash = 0;

    /*
     * Determine if our own timer has timed out
     */
    if (fpgaerr & FPGA_MS_TIMEOUT) {
	/*
	 * Clear the interrupt
	 */
	sreg = (ushort *)ADRSPC_MASTER_TIMER;
	*sreg = MASTER_TIMER_10MSEC;

	/*
	 * Disable the timer and the interrupt, one is enough for 
	 * us to be out of here
	 */
	sreg = (ushort *) ADRSPC_MASTER_TIMER_ENABLE;
	*sreg = ~MASTER_TIMER_ENABLE;
	sreg = (ushort *) ADRSPC_FPGA_INTR_MASK;
	*sreg &= ACTIVE_LOW(FPGA_MS_TIMEOUT);

	/*
	 * Exit ASAP.  We've already lost use of the cbus and
	 * dbus, and even as we speak, our peer is racing to
	 * come up, so let's join him in the ROM monitor. We
	 * won't bother executing the rest of this routine.
         */
	errmsg(&msgsym(MSDOG, RSP));
	return 1;
    }

    /*
     * Determine if there was a master/slave transition.
     */
    if (fpgaerr & FPGA_MS_CHANGE) {
	ushort new_peer_status;

	sreg = (ushort *)ADRSPC_MASTER_STATUS;
	ms_status = *sreg;

	new_peer_status = ms_status & MASTER_STATUS_OTHER_MASK;

	/*
	 * We're only interested in changes in our peer's state.
	 */
	if (new_peer_status != peer_hw_status) {
	    /*
	     * Our peer has changed state.  We need to decide
	     * how severe this is, based on whether or not we're
	     * the master, and what sort of state transition our
	     * peer underwent.
	     */
	    if (i_am_master) {
		ushort old_peer_index, new_peer_index;
		old_peer_index = peer_hw_status >> MASTER_STATUS_OTHER_SHIFT;
		new_peer_index = new_peer_status >> MASTER_STATUS_OTHER_SHIFT;

		/*
		 * We don't much care what the slave's up to.
		 * We'll log it and keep trucking for now.
		 * XXX revisit when dual-MEMD is implemented,
		 * reset the bus if mapped in, otherwise ignore.
		 */
		if (!system_loading) {
		    errmsg(&msgsym(SLAVECHANGE, RSP), 
			   status_string[old_peer_index], 
			   status_string[new_peer_index]);
		}

		/*
		 * Now update the static records we keep on the slave
		 */
		switch (new_peer_status) {

		case MASTER_STATUS_OTHER_NONPART:
		    slave_state = SLAVE_CRASHED;
		    break;

		case MASTER_STATUS_OTHER_SLAVE:
		    /* 
		     * Due to a FPGA bug these bits may incorrectly indicate
		     * that a slave is installed even after it was
		     * removed, so verify that a card is present.
		     */
		    if ( touch_device(pslot_to_dbus(get_rsp_peer_slot())) ) {
		        /*
		         * Now determine whether or not the slave has crashed
		         * recently.
		         */
		        if (board_has_instr_error(get_rsp_peer_slot()))
			    slave_state = SLAVE_CRASHED;
		        else
			    slave_state = SLAVE_NEEDS_BOOT;
		        break;
		    } else {
			/* Fall through to "slave unplugged" case */
		    }

		case MASTER_STATUS_OTHER_NOTHING:
		    slave_state = SLAVE_UNPLUGGED;
		    reg_invoke_chassismib_remove_card_entry(
					      get_slotp(get_rsp_peer_slot()));
		    break;

		case MASTER_STATUS_OTHER_MASTER:
		    /*
		     * The revolution... slave has just stolen m'ship.
		     * This should never happen.
		     */
		    errmsg(&msgsym(SLAVEMASTER, RSP));
		    crash = 1;
		    break;
		}

	    } else {
		/*
		 * We're the slave and the master has just changed state.
		 * This could be serious.  If the master is now anything
		 * but master, we need to reload so that we can participate
		 * in the next master/slave dance.  We musn't crash here,
		 * but rather must gracefully reload, but urgently.
		 */
		if (new_peer_status != MASTER_STATUS_OTHER_MASTER) {
		    reload_system(RELOAD_QUICK);
		}
	    }
	    peer_hw_status = new_peer_status;
	}
    }

    return crash;
}

static inline ulong
exceptaddr (ulong hi, ulong lo)
{
    return ((*(ushort*) hi) << 16) | *(ushort*) lo;
}

static char*
watchexcept (void)
{
    char *str;
    uchar stat;
    volatile uchar *reg;

    reg = (uchar*) ADRSPC_WATCH_STATUS;
    stat = *reg;
    switch (stat) {
      case DCL_WATCH_READ_BURST:	str = "burst read";	break;
      case DCL_WATCH_READ_1B:		str = "1b read";	break;
      case DCL_WATCH_READ_2B:		str = "2b read";	break;
      case DCL_WATCH_READ_3B:		str = "3b read";	break;
      case DCL_WATCH_READ_4B:		str = "4b read";	break;
      case DCL_WATCH_READ_5B:		str = "5b read";	break;
      case DCL_WATCH_READ_6B:		str = "6b read";	break;
      case DCL_WATCH_READ_7B:		str = "7b read";	break;
      case DCL_WATCH_READ_8B:		str = "8b read";	break;
      case DCL_WATCH_WRITE_BURST:	str = "burst write";	break;
      case DCL_WATCH_WRITE_1B:		str = "1b write";	break;
      case DCL_WATCH_WRITE_2B:		str = "2b write";	break;
      case DCL_WATCH_WRITE_3B:		str = "3b write";	break;
      case DCL_WATCH_WRITE_4B:		str = "4b write";	break;
      case DCL_WATCH_WRITE_5B:		str = "5b write";	break;
      case DCL_WATCH_WRITE_6B:		str = "6b write";	break;
      case DCL_WATCH_WRITE_7B:		str = "7b write";	break;
      case DCL_WATCH_WRITE_8B:		str = "8b write";	break;
      default: {
	  static char buf[sizeof("0xff")];

	  sprintf(buf, "0x%02x", stat);
	  return buf;
      }
    }
    return str;
}

static int
traperr_report (uchar trapstat)
{
    char *str;
    ulong val;

    if (trapstat & TRAP_WATCH) {
	ulong mask;
	volatile uchar *reg;

	reg = (uchar*) ADRSPC_WATCH_CONTROL;
	*reg = DCL_WATCH_INACTIVE;	/* disable watching */
	str = watchexcept();
	val = exceptaddr(ADRSPC_WATCH_HI, ADRSPC_WATCH_LO);
	mask = exceptaddr(ADRSPC_WATCH_MASK_HI, ADRSPC_WATCH_MASK_LO);
	errvarmsg(&msgsym(ERROR, RSP), "watch %s, addr %08x, mask %08x",
		  str, val, ~mask);
	return (watch_action_crash ? 1 : 0); /* follow instructions */
    }

    if (trapstat & TRAP_IOEXCEPT) {
	val = exceptaddr(ADRSPC_IO_WREXCEPT_HI, ADRSPC_IO_WREXCEPT_LO);
	/* 
	 * If this is a dbus address, then assume the card has been 
	 * pulled and quietly continue on.
	*/
	if ((val >= PHY_ADRSPC_DIAGBUS) && 
	    (val < (PHY_ADRSPC_DIAGBUS+DIAGBUS_ADRSPC_LEN))) {
	    return 0;
	}
	str = "I/O write";
    } else if (trapstat & TRAP_MEMDEXCEPT) {
	val = exceptaddr(ADRSPC_MEMD_WREXCEPT_HI, ADRSPC_MEMD_WREXCEPT_LO);
	str = "memd write";
    } else if (trapstat & TRAP_DRAMEXCEPT) {
	val = exceptaddr(ADRSPC_DRAM_WREXCEPT_HI, ADRSPC_DRAM_WREXCEPT_LO);
	str = "dram write";
    } else {
	str = "unknown";
	val = 0;
    }
    errvarmsg(&msgsym(ERROR, RSP), "%s exception, addr %08x", str, val);
    return 1;				/* system s/w error, time to die */
}

static int
rperrstat_report (uchar rperrstat)
{
    char *error_type;
    char *access_mode;
    char *memory_region;

    error_type = "alignment";
    access_mode = "access";
    memory_region = "MEMD";
    if (rperrstat & RPERR_CY_TIMEOUT) {
	error_type = "timeout";
	access_mode = "slave access";
	memory_region = "CY bus";
	if (i_am_master == FALSE) {
	    /*
	     * If we are the slave image, then
	     * this could be caused by reload/crash
	     * of the master. Therefore we simply
	     * do a quick escape to the rom monitor.
	     */
	    cbus_timeout_handler();
	    return 0;
	}
    } else if (rperrstat & RPERR_CY_ALIGN) {
	access_mode = "slave access";
	memory_region = "CY bus";
    } else if (rperrstat & RPERR_MEMD_DIAG) {
	memory_region = "MEMD diag";
    } else if (rperrstat & RPERR_QA_WRITE) {
	access_mode = "write";
	memory_region = "QA";
    } else if (rperrstat & RPERR_QA_READ) {
	access_mode = "read";
	memory_region = "QA";
    } else if (rperrstat & RPERR_SRAM_WRITE) {
	access_mode = "write";
    } else if (rperrstat & RPERR_SRAM_READ) {
	access_mode = "read";
    } else if (rperrstat & RPERR_ADDRESS) {
	error_type = "address";
    } else {
	error_type = "(unknown)";
    }
    errvarmsg(&msgsym(ERROR, RSP), "  RSP %s error on %s to %s, addr %08x",
	      error_type, access_mode, memory_region,
	      exceptaddr(ADRSPC_MEMD_WREXCEPT_HI, ADRSPC_MEMD_WREXCEPT_LO));
    return 1;			/* system s/w error, crash */
}

/*
 * handle the error interrupt
 */
static void
rsp_error_interrupt (r4k_context_t *ptr)
{
    int crash;
    volatile uchar* reg;
    uchar trapstat, rperrstat;

    reg = (uchar*) ADRSPC_TRAP_STATUS;
    trapstat = *reg;
    reg = (uchar*) ADRSPC_RPERRSTAT;
    rperrstat = *reg;

    crash = 0;

    crash |= memderr_report(ptr);

    if (rsp2)
	crash |= fpgaerr_report();

    if (trapstat)
	crash |= traperr_report(trapstat);

    if (rperrstat)
	crash |= rperrstat_report(rperrstat);

    if (!crash)
	return;

    /*
     * we're killing the image, flush accumulated messages
     */
    logger_flush();

    /*
     * Return to the monitor
     */
    r4k_return_to_monitor(SIGKILL, 0, ptr);
}

#define DBUS_PROTECT_CBUS0	0x01
#define DBUS_PROTECT_CBUS1	0x02

/*
 * save & disable the grant state of cbus 0 & 1
 */
void
rsp_dbus_protect (dbus_protect_t* state)
{
    volatile uchar *cbus0, *cbus1;

    cbus0 = (uchar*) ADRSPC_CBUS0REG;
    cbus1 = (uchar*) ADRSPC_CBUS1REG;

    state->cbus_grant = 0;
    if (*cbus0 & CBUSX_GRANT_ENABLE)
	state->cbus_grant |= DBUS_PROTECT_CBUS0;
    if (*cbus1 & CBUSX_GRANT_ENABLE)
	state->cbus_grant |= DBUS_PROTECT_CBUS1;

    GLOBAL_INTERRUPT_BLOCK(state->sreg);

    *cbus0 &= ~CBUSX_GRANT_ENABLE;
    *cbus1 &= ~CBUSX_GRANT_ENABLE;

    return;
}

/*
 * enable grant on cbus 0 & 1 if indicated by "state"
 */
void
rsp_dbus_restore (dbus_protect_t* state)
{
    volatile uchar* cbus;

    if (state->cbus_grant & DBUS_PROTECT_CBUS0) {
	cbus = (uchar*) ADRSPC_CBUS0REG;
	*cbus |= CBUSX_GRANT_ENABLE;
    }
    if (state->cbus_grant & DBUS_PROTECT_CBUS1) {
	cbus = (uchar*) ADRSPC_CBUS1REG;
	*cbus |= CBUSX_GRANT_ENABLE;
    }

    GLOBAL_INTERRUPT_UNBLOCK(state->sreg);

    return;
}

/*
 * enable grant on cbus 0 & 1
 * Warning: NOT nestable.
 */
void
enable_cbus_grant (void)
{
    volatile uchar* reg;

    reg = (uchar*) ADRSPC_CBUS0REG;
    *reg |= CBUSX_GRANT_ENABLE;

    reg = (uchar*) ADRSPC_CBUS1REG;
    *reg |= CBUSX_GRANT_ENABLE;

    return;
}

/*
 * disable grant on cbus 0 & 1
 * Warning: NOT nestable.
 */
void
disable_cbus_grant (void)
{
    volatile uchar* reg;

    reg = (uchar*) ADRSPC_CBUS0REG;
    *reg &= ~CBUSX_GRANT_ENABLE;

    reg = (uchar*) ADRSPC_CBUS1REG;
    *reg &= ~CBUSX_GRANT_ENABLE;

    return;
}

static void
pcmcia_reset (void)
{
    volatile uchar* reg;

    reg = (uchar*) ADRSPC_RESET_PERIPH;
    *reg = ACTIVE_LOW(PERIPH_PCMCIA);
    *reg;			/* flush write buffer */
    usecdelay(1);		/* delay 80ns to set-up reset, plus
				 * 80ns for all peripheral resets, except
				 * PCMCIA needs 500ns to reset
				 */
    return;
}

/*
 * fill a range with a 64bit value
 */
static inline void
mem_fill (u64* addr, uint count, u64* value)
{
    while (count-- > 0)
	move64(value, addr++);
    return;
}

/*
 * verify that a range contains a 64bit value throughout
 */
static inline boolean
mem_verify (u64* addr, uint count, u64* value)
{
    u64 readval;

    while (count-- > 0) {
	move64(addr, &readval);
	if (readval != *value) {
	    errvarmsg(&msgsym(ERROR, RSP),
		      "MEMD failure, addr %x, read %08x%08x, not %08x%08x",
		      (uint) addr,
		      ((ulong*) &readval)[0], ((ulong*) &readval)[1],
		      ((ulong*)  value)[0],   ((ulong*)  value)[1]);
	    return FALSE;
       }
	++addr;
    }
    return TRUE;
}

/*
 * test read & write to MEMD, crash on failure
 */
static void
memd_memtest (void* start, uint bytes)
{
    u64 value;
    uint size, ix;
    static uchar testvals[] = {
	0, 0x5a, 0xa5, 0xff,
    };

    size = bytes / sizeof(value);
    for (ix = 0; ix < sizeof(testvals)/sizeof(testvals[0]); ++ix) {
	for (bytes = 0; bytes < sizeof(value); ++bytes)
	    ((uchar*) &value)[bytes] = testvals[ix];
	mem_fill(start, size, &value);
	cache_flush();
	if (!mem_verify(start, size, &value))
	    crashdump(0);
    }
    return;
}

#define RSP_MIN_BYTES_DRAM_11_2  32*1024*1024
/*
 * rsp_minimum_dram_check().  see CSCdi70557.
 * this process is started at the end of init.
 * it sleeps for a minute and then 
 * it checks for minimum dram and reports error if the amount of 
 * dram is less than RSP_MIN_BYTES_DRAM_11_2 (32 Megabytes as of 11.2(6)).
 */
#define DELAY_BEFORE_INSUFFICIENT_MEMORY_CHECK 60
static forktype
rsp_minimum_dram_check (void)
{
    process_sleep_for(DELAY_BEFORE_INSUFFICIENT_MEMORY_CHECK*ONESEC);
    if (mon_getmemsize() < RSP_MIN_BYTES_DRAM_11_2) {
        errmsg(&msgsym(RSPDRAM, RSP), RSP_MIN_BYTES_DRAM_11_2/1024/1024);
    }
    process_kill(THIS_PROCESS);
}

static void
rsp_init (subsystype* subsys)
{
    dgbus_eeprom_t *idprom;

    memd_size = mon_get_iosize();
    pmem_size = (ulong) mon_getmemsize();

    /* reserve some "buffer headers" at the top of the segment for CCBs */
    bufhdr_count = (MEMD_BUFHDR_SIZE -
		    MAX_SLOTS * sizeof(ccbtype)) / sizeof(bufhdr_t);

    /*
     * Reserve last bufhdr for a special VIP2 work-around
     * ccb_base is one bufhdr down.
     */
    bufhdr_count--;
    idprom = (dgbus_eeprom_t*) ADRSPC_IDPROM;
    if (idprom->eeprom_version == IDPROM_VERSION) {
	rsp7000 = FALSE;
	switch (idprom->controller_type) {
	  case FCI_RSP1_CONTROLLER:			break;
	  case FCI_RSP2_CONTROLLER:			break;
	  case FCI_RSP7000_CONTROLLER: rsp7000 = TRUE;	break;

	  default:
	    errmsg(&msgsym(IDPROM, RSP), idprom->controller_type);
	    break;
	}
    } else {
	errmsg(&msgsym(IDPROM, RSP), idprom->controller_type);
    }

    memd_base = (uchar*) ADRSPC_MEMD;			    /* write back */

    memd_memtest(memd_base + MEMD_BUFHDR_OFFSET,
		 memd_size - MEMD_BUFHDR_OFFSET);

    bufhdr_base = (bufhdr_t*) ADRSPC_MEMD_BUFHDR;	    /* write-through */

    ccb_base = ADDR_MEMD_TO_UNCACHED(&bufhdr_base[bufhdr_count]);

    qasic_base = (qa_asic_t*) ADRSPC_MEMD_QAREGS;	    /* uncached */

    createlevel(LEVEL_NETWORK, rsp_process_rawq, "Network Interrupt");
    createlevel(LEVEL_NETWORK_HI, rsp_process_eventq,
		"Network Status Interrupt");

    /*
     * Initialize memd buffer header memory and memd buffer data memory
     * cache mode is write-thru, but since we don't invalidate before reading
     * a new packet, lets invalidate now since we don't really know
     * whether or not any of this is in the cache already.  For
     * example, it looks like the call to mem_verify() above, leaves things 
     * in the cache.  Also, we do the two 'sections' seperately so as to
     * use their own virtual address spaces.
     */
    {
	uint quads;
	u64* addr;
	u64 data64;

	/*
	 * Set up so we can do it in 64-bit writes
	 */
	data64 = ((u64)INVALID_ADDR << 32) | (u64)INVALID_ADDR;

	/*
	 * First do the buffer header area.
	 */
	addr = (u64 *)bufhdr_base;
	quads = (MEMD_BUFDATA_OFFSET - MEMD_BUFHDR_OFFSET) / sizeof(u64);
	mem_fill(addr, quads, &data64); 

	/*
	 * Next, fill the buffer area.
	 */
	addr = (u64 *)&memd_base[MEMD_BUFDATA_OFFSET];
	quads = (memd_size - MEMD_BUFDATA_OFFSET) / sizeof(u64);
	mem_fill(addr, quads, &data64);
    }

    cache_flush();			/* Write everything out to MEMD */

    rsp_fs_init();			/* Initialize RSP specific info */

    /*
     * In case of slave, map everything to master's MEMD for now.
     * Notice this must be done after the above memd initializations.
     * Having mapped our MEMD out of the picture, it is now safe to go
     * ahead and enable the cbus as IPs will no longer be tempted to
     * access our slave MEMD.
     * Also initialize file system server.
     */
    if (i_am_master == FALSE) {
	volatile uchar* reg;

	*(uchar *)ADRSPC_QA_STATUS_RANGE = RANGE(0,0xf);
	*(uchar *)ADRSPC_QA_RANGE = RANGE(0,0xf);
	*(uchar *)ADRSPC_BUFHDR_RANGE = RANGE(0,0xf);
	*(uchar *)ADRSPC_BUFFER_RANGE = RANGE(0,0xf);
	reg = (uchar*) ADRSPC_SYSCONTROL;
	*reg = ACTIVE_LOW(CONTROL_DBUS_ENABLE | CONTROL_CBUS_ENABLE);

	rsp_init_cbus_monitor();
    }

    RFSS_init(i_am_master);

    /*
     * Both master and slave provide their own routine to set things up
     * with regards master/slave transactions, parse chains etc.
     * To avoid getting the hopes up of rsp2-in-7505 customers, we
     * won't chain the parse stuff in unless we're in a dual-capable
     * chassis (or test chassis, or 7506, but they don't matter
     * either way).  We also won't chain it in for rsp1 systems.
     */
    if(rsp2 && (get_rsp_peer_slot() != RSP_NO_SLOT)) {
	rsp_slave_init();
    }

    /*
     * register a test for slave.
     */
    reg_add_slave_test(rsp_is_slave, "rsp_is_slave");    
    reg_add_hardware_shutdown(pcmcia_reset, "pcmcia_reset");

    (void)cfork(rsp_minimum_dram_check,0,0,"RSP memory size check",stdio->ttynum);

    defeat_sum();			/* no need to spend time on this... */

    return;
}

SUBSYS_HEADER(rsp,			/* name */
	      1,			/* major version */
	      0,			/* minor version */
	      1,			/* edit version */
	      rsp_init,			/* init function */
	      SUBSYS_CLASS_DRIVER,	/* subsystem class */
	      "seq: dbus",		/* sequence list */
	      "req: dbus"		/* prerequisite list */
	      );

/*
 * original system bus error handler
 */
static SIG_PF buserr_handler;

/*
 * special handling for bus errors to certain address ranges
 */
static int
rsp_buserr (int sig, int code, r4k_context_t *sc)
{
    int cause;
    ulong addr;
    int reg;

    cause = (sc->cause & CAUSE_MASK) >> CAUSE_SHIFT;
    addr = (ulong)r4k_get_cpu_addr(sc);
    if (cause == CAUSE_BUS_DATA) {
	if ((addr >= ADRSPC_DIAGBUS) && 
	    (addr < (ADRSPC_DIAGBUS+DIAGBUS_ADRSPC_LEN))) {

	    /* Get register involved in read instruction */
	    reg = check_null_read(sc);

	    if (reg > 0) {
		/* Check and see if this is a read of dbus->diag_status */
		if ((addr & (DGBUSBYTES - 1)) == offsetof(dgtype, diag_status)) {
		    /*  return value that indicats no status */
		    r4k_set_cpu_reg(sc, reg, DBUS_STATUS_TIMEOUT, FALSE);
		} else {
		    /* Other dbus register read */
		    r4k_set_cpu_reg(sc, reg, DBUS_REG_TIMEOUT, FALSE);
		}
		/* move pc past instruction that caused exception */
		if (!branch_emulate(sc)) {
		    /* We've successfully incremented pc */
		    return(0);
		}
    }
	}
    }
    
    return (*buserr_handler)(sig, code, sc);
}

/*
 * platform_shutdown_ips
 *
 * Invoked via the hardware_shutdown registry when the system is
 * terminating.
 *
 * Reset all interface processor cards and disable the cbus grant.
 */
static void
platform_shutdown_ips (void)
{
    dbus_reset_IP_slots();
    disable_cbus_grant();
}

/*
 * rsp_bus_init
 *
 * Initializes basic high-end system/interface support
 * (or used to, anyway)
 * The drivers are now initialized as subsystems
 */
void rsp_bus_init (void)
{
    uchar val;
    uint count;
    volatile uchar* reg;
    volatile ushort* sreg;
    volatile u64* u64reg;

    buserr_handler = signal(SIGBUS, (SIG_PF) rsp_buserr);

    reset_reason = ACTIVE_LOW(mon_reset_reason()) & REASON_ALL;
    if (reset_reason == REASON_ALL)
	reset_reason = 0;

    reg_add_hardware_shutdown(platform_shutdown_ips, "platform_shutdown_ips");

    /*
     * check each cbus for stall condition
     */
    reg = (uchar*) ADRSPC_CBUS0REG;
    val = *reg;
    if (val & CBUSX_UNLATCHED_STALL) {
	errmsg(&msgsym(STALL, RSP), 0);
	crashdump(0);
    }
    if (val & CBUSX_LATCHED_STALL)		/* clear leftover stall */
	*reg = (*reg & ~CBUSX_LATCHED_STALL) | CBUSX_STALL_UPDATE;
    reg = (uchar*) ADRSPC_CBUS1REG;
    val = *reg;
    if (val & CBUSX_UNLATCHED_STALL) {
	errmsg(&msgsym(STALL, RSP), 1);
	crashdump(0);
    }
    if (val & CBUSX_LATCHED_STALL)		/* clear leftover stall */
	*reg = (*reg & ~CBUSX_LATCHED_STALL) | CBUSX_STALL_UPDATE;

    sreg = (ushort*) ADRSPC_DCL;
    *sreg = (DCL_MEMD_RDBYPASS | DCL_DRAM_RDBYPASS |
	     DCL_MEMD_WRBUF | DCL_DRAM_WRBUF | DCL_IO_WRBUF |
	     ((rsp2) ? DCL_MEMD_DELAY_RSP2 : DCL_MEMD_DELAY_RSP1));

    reg = (uchar*) ADRSPC_SYSCONTROL;
    if (i_am_master) {
	/*
	 * If we're the master (includes any rsp1, and an rsp2 master), enable
	 * both the cbus and the dbus.
	 */
	*reg = ACTIVE_LOW(CONTROL_DBUS_ENABLE | CONTROL_CBUS_ENABLE);
    } else {
	/*
	 * We're an rsp2 slave.  Go ahead and enable the dbus, but don't enable
	 * the cbus until we re-map our MEMD out of the picture in rsp_init().
	 * If we enable it here, both RSPs will respond to MEMD cycles from IPs
	 * which wreaks havoc on the bus.
	 */
	*reg = ACTIVE_LOW(CONTROL_DBUS_ENABLE);
    }

    if (rsp2) {
	u64 MD_error_info;
	
	/* CyBus ParityValid HW Problem workaround. */
	reg = (uchar*) ADRSPC_CBUS0REG;
	*reg &= ~CBUSX_PARITY_DISABLE;

	/* Adjust for differences in MEMD Control Register. */
	reg = (uchar*) ADRSPC_MEMD_DIAG;
	*reg = (DIAG_PARITY_EVEN | DIAG_CY_CYCLESTEAL | DIAG_CY1IO_ENABLE |
	    DIAG_CY0IO_ENABLE | DIAG_ERRCK_ENABLE);

	/*
	 * Work around MD error info problem by clearing out any
	 * problems.
	 */
	u64reg = (u64 *) ADRSPC_MDERRINFO;
	move64((u64 *) u64reg, &MD_error_info);
	reg = (uchar*) ADRSPC_MC_INTR_STATUS;
	val = *reg;
	reg = (uchar*) ADRSPC_MEMD_ERRSTATUS;
	val = *reg;

	/* Enable MC asic interrupts through the DBUS FPGA. */
	sreg = (ushort *) ADRSPC_FPGA_INTR_MASK;
	*sreg &= (*sreg & ~(FPGA_MC_ASIC));

    } else {
	/*
	 * this register is flaky on some RSP1s (hw fix in progress)
	 */
	reg = (uchar*) ADRSPC_MEMD_DIAG;
	for (count = 0; count < 10000; ++count) {
	    *reg = DIAG_ERRCK_ENABLE;
	    val = *reg;
	    if (val == DIAG_ERRCK_ENABLE)
		break;
	}
	if ((count > 0) || (val != DIAG_ERRCK_ENABLE)) {
	    errvarmsg(&msgsym(ERROR, RSP),
		      "MEMD Diag register failure, count %u, value %02x",
		      count, val);
	    if (val != DIAG_ERRCK_ENABLE)
		crashdump(0);
	}
    }

    macaddr_init();

    return;
}

/*
 * what version RSP is this?
 */
rsp_flavor_t rsp_flavor (void)
{
    if (rsp7000)
	return RSP_RSP7000;
    if (rsp2)
	return RSP_RSP2;
    return RSP_RSP1;
}

/*
 * Returns TRUE if this system(rsp, chassis) is dual capable,
 *         FALSE otherwise.
 */

boolean rsp_dual_capable (void)
{
    rsp_flavor_t rsp_type;

    rsp_type = rsp_flavor();

    if ((rsp_type == RSP_RSP2 || 
         rsp_type == RSP_RSP4) && 
        (get_rsp_peer_slot() != RSP_NO_SLOT)) {
	return TRUE;
    }
    else
	return FALSE;
}

/*
 * rsp_is_master
 *
 * Returns TRUE if this RSP is the master
 * A lonely RSP1 is also considered a master.
 */

boolean rsp_is_master (void)
{
    return(i_am_master);
}

/*
 * rsp_is_slave
 *
 * Returns TRUE if this RSP is a slave RSP in a dual system.
 */

boolean rsp_is_slave (void)
{
    return(!i_am_master);
}

/*
 * promote_slave_state
 *
 * Promotes the slave state to SLAVE_RUNNING.  Called by the slave signin 
 * handler.
 * XXX handle dual MEMD case, recarve, pass "enabled" to errmsg etc. and
 * change the SLAVEUP message to display the "enabled"/"disabled" state
 * of buffer memory.  For now, it ignores this parameter so as not to
 * get their hopes up until we've implemented dual MEMD.
 */
void promote_slave_state(void)
{
    slave_state = SLAVE_RUNNING;
    errmsg(&msgsym(SLAVEUP, RSP), "disabled");
    rsp_flashmib_init();
}

/*
 * rsp_slave_state
 *
 * Returns one of the following:
 *
 *    SLAVE_UNPLUGGED if no slave, or RSP1, sole RSP2  or  we are the slave
 *    SLAVE_CRASHED if slave is installed, but WCS_CNTRL_INSTR_ERROR is set
 *    SLAVE_NEEDS_BOOT if installed, above bit not set, but hasn't signed in
 *    SLAVE_RUNNING if installed, bit not set, and fully signed in.
 */
slave_state_t rsp_slave_state (void)
{
    return(slave_state);
}


/*
 * public access routines for peer_slot, and self_slot variables.
 */
uchar
get_rsp_peer_slot (void)
{
    return rsp_peer_slot;
}

/*
 * WARNING - it is not always possible to know the current slot,
 * so this function can return MAX_SLOTS (an illegal value)
 */
uchar
get_rsp_self_slot (void)
{
    return rsp_self_slot;
}

/*
 * Figure out our slot and the slave RSP's slot (if any)
 */
static void
determine_rsp_slots (boolean rsp2)
{
    rsp_peer_slot = RSP_NO_SLOT;
    if (rsp2) {
	uchar slot;
	volatile ushort* dbus_slot;

	dbus_slot = (volatile ushort*) ADRSPC_DBUS_SLOT;
	slot = *dbus_slot;
	if ((slot >> 4) != (~slot & 0x0f)) {
	    errmsg(&msgsym(SLOTCOMP, CI), "RSP", slot);
	} else {
	    rsp_self_slot = (slot >> 4);
	    rsp_self_slot = adjust_rsp_slot(rsp_self_slot);
	    switch (rsp_self_slot) {
	      case RSPSLOT_7505:
		break;				/* slave not possible */

	      case RSPSLOT_7507_PROC1:
		rsp_peer_slot = RSPSLOT_7507_PROC2;
		break;

	      case RSPSLOT_7507_PROC2:
		rsp_peer_slot = RSPSLOT_7507_PROC1;
		break;

	      case RSPSLOT_7513_PROC1:		/* or 7506 or test chassis */
		rsp_peer_slot = RSPSLOT_7513_PROC2;
		break;

	      case RSPSLOT_7513_PROC2:		/* or test chassis */
		rsp_peer_slot = RSPSLOT_7513_PROC1;
		break;
	    }
	    return;
	}
    }
    rsp_self_slot = compute_rsp_slot();
    return;
}

/*
 * determine_rsp_specifics
 * Determine the type of RSP we are and slot information for both ourself
 * and any peer that may exist in a dual-rsp configuration.
 */
static void
determine_rsp_specifics (void)
{
    dgbus_eeprom_t *idprom;
    volatile uchar* reg;
    volatile ushort* sreg;

    /*
     * What type of RSP are we?
     */
    idprom = (dgbus_eeprom_t*) ADRSPC_IDPROM;
    rsp2 = ((idprom->eeprom_version == IDPROM_VERSION) &&
	    (idprom->controller_type == FCI_RSP2_CONTROLLER));

    /*
     * turn on dbus so we can look for a slave RSP
     * note: this also disables the cbus drivers
     * On the RSP2, the DBUS_ENABLE bit is ignored, so
     * we can execute the following code regardless of what
     * sort of RSP we are.  This is the first place we initialise
     * this register, and we want to do that for both RSP1 and RSP2.
     */
    reg = (uchar*) ADRSPC_SYSCONTROL;
    *reg = ACTIVE_LOW(CONTROL_DBUS_ENABLE);

    determine_rsp_slots(rsp2);

    /*
     * Determine whether or not we're the master, and determine the
     * state of the slave.  Note, only the master is interested in
     * the state of the slave.  The master uses this to determine
     * whether or not it should download the slave.  Hence, if we're
     * the slave, we will claim that the slave is UNPLUGGED, that way
     * any common code running on the slave won't be tempted to download the
     * slave (and needn't explicitly check if it's the master).
     */
    if (rsp2) {
	ushort peer_status;

	sreg = (ushort*)ADRSPC_MASTER_STATUS;
	peer_hw_status = peer_status = *sreg & MASTER_STATUS_OTHER_MASK;
	switch (peer_status) {

	case MASTER_STATUS_OTHER_NOTHING:
	    i_am_master = TRUE;
	    slave_state = SLAVE_UNPLUGGED;
	    break;

	case MASTER_STATUS_OTHER_MASTER:
	    i_am_master = FALSE;
	    slave_state = SLAVE_UNPLUGGED;      /* see contract above */
	    break;

	case MASTER_STATUS_OTHER_NONPART:
	    i_am_master = TRUE;
	    slave_state = SLAVE_CRASHED;
	    break;

	case MASTER_STATUS_OTHER_SLAVE:
	    i_am_master = TRUE;

	    /*
	     * Now determine whether or not the slave has crashed
	     * recently.
	     */
	    if (board_has_instr_error(get_rsp_peer_slot()))
		slave_state = SLAVE_CRASHED;
	    else
		slave_state = SLAVE_NEEDS_BOOT;
	    break;
	}
    } else {
	i_am_master = TRUE;
	slave_state = SLAVE_UNPLUGGED;
    }


    /*
     * On RSP2's clear down our diag status_register.  The ROM monitor
     * writes WCS_CNTRL_INSTR_ERROR into this register whenever we
     * return to it via a CPU_EXCEPTION (except for console breaks).
     * This indicates to the other RSP that we've crashed, thereby
     * preventing it from automatically resetting us when it comes up
     * as master.  Now that we've been manually reset, and we're up
     * and running again, we need to clear it for next time.
     */
    if (rsp2) {
	sreg = (ushort*)ADRSPC_DBUS_STATUS;
	*sreg = 0;
	*sreg;                                    /* Flush the write */
    }

    /*
     * If we're an RSP2 master, allow CBUS IO cycles.  Currently, the
     * slave doesn't need to send attentions to the IPs, so we'll
     * give the full bandwidth to the master.  RSP1 doesn't have
     * such a concept.
     */
    if (rsp2 && i_am_master) {
	reg = (uchar*) ADRSPC_CBUS_LOCK;
	*reg = ACTIVE_LOW(CBUS_IOBUS_REQUEST);
    }
}

/*
 * Called by platform_exception_init() to set up hardware specific
 * interrupt vectors also clear interrupt sources we're not ready for
 */
void
rsp_exception_init (void)
{
    volatile uchar* reg;
    volatile ushort* sreg;
    uint slot, page;
    boolean valid, rdbypass;

    /*
     * This is a good place to determine what type of RSP we are
     * before interrupts are enabled and any handlers need to know.
     */
    determine_rsp_specifics();

    /*
     * initialize the page map table (clear out bad parity)
     * Only needs to be done at power-on
     */
    reg = (volatile uchar*) ADRSPC_MEMD_DIAG;
    *reg = DIAG_PARITY_EVEN;
    if (i_am_master)
	valid = FALSE;		/* master complains about bad accesses... */
    else
	valid = TRUE;		/* ...but slave won't. */
    reg = (volatile uchar*) ADRSPC_DCL;
    rdbypass = *reg & DCL_MEMD_RDBYPASS;
    if (rdbypass)
	*reg &= ~DCL_MEMD_RDBYPASS;
    for (slot = 0; slot < PAGEMAP_SLOTS; ++slot) {
	for (page = 0; page < PAGEMAPS_PER_SLOT; ++page) {
	    PAGEMAP_HI(PMINDEX(slot, page)) = PMVALUE_HI(0, valid);
	    PAGEMAP_LO(PMINDEX(slot, page)) = PMVALUE_LO(0);
	}
    }
    if (rdbypass) {
	/*
	 * flush last pagemap write
	 */
	(void) PAGEMAP_LO(PMINDEX(0, 0));
	*reg |= DCL_MEMD_RDBYPASS;
    }

    /*
     * levels 1 & 2: QA
     * reset IPs here because rsp_qa_init() disables grant for long
     * enough that rev 1 MIPs will take a bus error, resulting in an
     * invalid bus cycle when grant is enabled
     */
    
    /*
     * Turn on the dbus.
     * Initialize the dbus and assure all
     * interface processor cards are reset.
     */
    reg = (uchar*) ADRSPC_SYSCONTROL;
    *reg = ACTIVE_LOW(CONTROL_DBUS_ENABLE);
    dbus_startup();
    
    disable_cbus_grant();
    qasic_diag_base = (qa_diag_t *) ADRSPC_MEMD_CONTROLSPC;
    rsp_qa_init();
    
    /*
     * level 3: timers (ignore stall; crash on a boot-time OIR)
     */
    reg = (uchar*) ADRSPC_COUNTCTL;
    *reg &= COUNTCTL_C0ENABLE;        /* leave only watchdog running */
    
    /*
     * level 4: PCMCIA
     */
    pcmcia_reset();
    
    /*
     * level 5: console (will be initialized soon enough)
     */
    
    /*
     * level 6: errors
     */
    createlevel(LEVEL_ERROR,
		(void(*)(void)) rsp_error_interrupt, "Error Interrupt");

    /*
     * Enable Master/Slave Interrupts for RSP2.  This is also where
     * we initialise this register for the first time, so we don't
     * OR in our setting, we just whack it in hard.
     * Start the timer here too.
     */
    if (rsp2) {
	r4k_second_wdog = TRUE;
	sreg = (ushort *) ADRSPC_FPGA_INTR_MASK;
	*sreg = ACTIVE_LOW(FPGA_MS_CHANGE | FPGA_MS_TIMEOUT);

	sreg = (ushort *)ADRSPC_MASTER_TIMER;
	*sreg = MASTER_TIMER_10MSEC;
	sreg = (ushort *) ADRSPC_MASTER_TIMER_ENABLE;
	*sreg = MASTER_TIMER_ENABLE;
    }

    
    /*
     * level 7: there is no level 7
     */

    /*
     * set bus software ready on the slave
     */
    if (rsp_is_slave()) {
	sreg  = (ushort *) ADRSPC_DBUS_STATUS;
	*sreg = DBUS_SW_READY;
    }
    
    return;
}

/*
 * Test IO memory (packet memory) parity errors
 */
void
test_parity_io (void)
{
    printf("\n%%not supported");
    return;
}

int
flash_device_slot (char* devnm)
{
    uchar slot;

    if (strncmp(devnm, slave_prefix, strlen(slave_prefix)) == 0) {
	slot = get_rsp_peer_slot();
    } else {
	slot = get_rsp_self_slot();
    }
    if (slot == RSP_NO_SLOT)
	return NO_SLOTNUMBER;
    return slot;
}
