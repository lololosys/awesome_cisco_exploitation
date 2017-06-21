/* $Id: if_hes_serial_parser.c,v 3.6.18.6 1996/09/09 01:34:22 xtang Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_hes_serial_parser.c,v $
 *------------------------------------------------------------------
 * if_hes_serial_parser.c -- Common RSP, cBus, MCI interface code parser
 *                           commands.
 *
 * October 1995, Greg Stovall
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_hes_serial_parser.c,v $
 * Revision 3.6.18.6  1996/09/09  01:34:22  xtang
 * CSCdi56632:  unknown clock rates on 4T PA, FSIP
 * Branch: California_branch
 *         add ALT_HELPS to use csb->which in different
 *         driver procedures, and print the
 *         driver specific info. In this case, the clockrate
 *         option table, and csb->which = CFG_INT_CLOCKRATE
 *
 * Revision 3.6.18.5  1996/09/07  18:57:05  xtang
 * CSCdi67401:  ALIGN-3-SPURIOUS: Spurious memory access made at
 * 0x60191478 reading
 * Branch: California_branch
 *         Turned off hwidb->serial_flags for HSSI and POSIP interfaces,
 *         therefore disallow clock rate (clockrate) command on these.
 *         Any serial interfaces that do not have clockrate command
 *         implemented should act similarly.
 *
 * Revision 3.6.18.4  1996/08/23  04:46:27  xtang
 * CSCdi59553:  clockrate 250000 is not configurable on FSIP in 75xx
 * Branch: California_branch
 *              added 250000 Hz to the FSIP interface card. Kept
 *              the AGS+ mciclock[] and mciclock_rev2[] untouched.
 *
 * Revision 3.6.18.3  1996/05/21  09:47:54  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.18.2  1996/03/21  22:40:46  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.6.18.1  1996/03/18  19:41:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.4  1996/03/18  23:19:29  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.3  1996/02/08  07:22:03  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.2  1996/01/26  02:47:42  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.2.2.1  1996/01/15  10:19:40  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.6  1996/02/07  16:11:35  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5  1996/02/01  06:02:39  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4.12.1  1996/01/24  21:46:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/23  01:13:21  rharagan
 * CSCdi45811:  4T interface linestate does not refletct modem control
 * lines
 * Enable the 'ignore-dcd' command for 4t, issue interface reset when
 * parsing crc 32 command.
 *
 * Revision 3.3  1995/12/17  18:27:23  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/16  23:28:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/10/17  23:05:45  gstovall
 * Placeholders for Talisker_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include "cbus_registry.h"
#include "interface_private.h"
#include "config.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "parser.h"
#include "parser_defs_fci.h"
#include "if_hes_serial.h"
#include "if_cmd.h"
#include "if_mci.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"


/*
 * hssi_command
 * Deal with hssi specific commands.
 * Synopsis:
 *           [no] hssi internal-clock
 * Provide clocking on the transmitter clock line.
 * There is no provision for controlling the clockrate.
 *
 *           [no] hssi external-loop-request
 * Disable LC input on hssi interface.
 */
static void hssi_command (parseinfo *csb)
{
    hwidbtype *idb;
    boolean mode;
    leveltype level;
    serialsb *ssb;

    idb = csb->interface->hwptr;

    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);

    if (csb->nvgen) {
	if (idb->status & IDB_HSSI) {
	    if (csb->which == HSSI_INTERNAL_CLOCK) {
		nv_write(ssb->hssi_iclk, csb->nv_command);
	    } else {
		nv_write(!ssb->hssi_lc_inhibit, csb->nv_command);
	    }
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
	return;
    }

    if (!(idb->status & IDB_HSSI)) {
	printf("\n%%Interface %s not an hssi.", idb->hw_namestring);
	idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
	return;
    }

    switch (csb->which) {
      case HSSI_INTERNAL_CLOCK:
	ssb->hssi_iclk = csb->sense;
	break;
      case HSSI_LC_INHIBIT:
	ssb->hssi_lc_inhibit = ! csb->sense;
	if (!csb->sense){
	    /*
	     * Force off dce's ability to loop us.
	     */
	    ssb->hssi_loop_req = FALSE;
	}else{
	    /*
	     * Make sure we are in sync with the LC bit.  We do this by
	     * faking an interrupt from the cbus so we check the status
	     * of the bit according to the microcode.  Only return after
	     * this call if hssi_loop_request() already does the
	     * call to *idb->reset() for us.
	     *
	     * hssi_loop_request() must have interrupts disabled in order
	     * to work correctly.
	     */
	    mode = ssb->hssi_loop_req;
	    level = raise_interrupt_level(NETS_DISABLE);
	    hssi_loop_request(idb);
	    reset_interrupt_level(level);
	    if (mode != ssb->hssi_loop_req){
		idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
		return;
	    }
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
	return;
    }

    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
    (*idb->reset)(idb);
    if (idb->counters.resets > 0)
	idb->counters.resets--;
}


/*
 * clockrate_command
 * Set the clock source and rate for an MCI serial interface
 */
void clockrate_command (parseinfo *csb)
{	
    hwidbtype	*idb;
    int		i, code, shift;
    ushort	result, short_code;
    leveltype	level;
    int		rate = 0;
    serialsb    *ssb;
    
    int         nclocks;
    const mciclocktype *clockrate_table;

    idb = csb->interface->hwptr;
    
    if (!(idb->status & IDB_SERIAL)) {
        return;
    }
      
    /*
     * nclocks will be either NCLOCKS or
     * FCI_NCLOCKS
     */
    if (idb->type == IDBTYPE_CBUSSERIAL) {
        nclocks = FCI_NCLOCKS;
        clockrate_table = fciclock;
    }
    else if (idb->type == IDBTYPE_MCISERIAL) {
        if (((mcitype *)idb->devctl)->sw_version > 0x0500) {
            nclocks = NCLOCKS;
	    clockrate_table = mciclock_rev2;
	} else {
            nclocks = NCLOCKS;
	    clockrate_table = mciclock;
	}
    }
    else {
        printf("\n%%This interface %s is not fci/mci.",
             idb->hw_namestring);
	return;
    }

    if (csb->sense) {
        rate = GETOBJ(int,1);
    } else {
        rate = 0;
    }

    if ((csb->which == CFG_INT_CLOCKRATE) && (rate == 0)) {
          printf("\n  \t%s\n", "Speed (bits per second)");

	  for (i = 1; i < nclocks; i++) {
	      printf("  %d\n", clockrate_table[i].rate);
	  }
	  return;
    }

    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);

    if (csb->nvgen) {
	if ((idb->status & (IDB_MCI | IDB_CBUS))) {
	    nv_write((ssb->serial_clockindex),
		     "%s %d", csb->nv_command,
		     clockrate_table[ssb->serial_clockindex].rate);
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
	return;
    }
    level = raise_interrupt_level(NETS_DISABLE);
    (void)select_if(idb);
    if (idb->status & IDB_CBUS) {
	short_code = 0;
	send_applique_cmd(idb, 0, &short_code);
	code = short_code;
    } else {
        for (i = 0, code = 0, shift = 0; i < APP_COUNT; i++) {
	    send_applique_cmd(idb, app_cmds[i], &result);
            if (result & MCI_APP_SENSE0) {
                code = 0;       /* not a valid cisco applique */
                break;
            }
            if (app_uses[i]) {
                if (result & MCI_APP_SENSE1)
                    code |= (1 << shift);
                shift++;
            }
        }
    }
    reset_interrupt_level(level);

    switch (code) {
	case MCI_DTE_RS232:
	case MCI_DTE_V35: 
	case MCI_DTE_RS449: 
	case MCI_DTE_X21:
	case MCI_DTE_RS232_NRZI:
	case MCI_DXE_G703_BAL:
	case MCI_DXE_G703_UNBAL:
        case MCI_DTE_RS530:
        case MCI_DTE_V35_NRZI:
	if (idb->type == IDBTYPE_CBUSSERIAL) {
  	    /*
  	     * Allow configuration of clockrate on interfaces
  	     * with DTE cables, but make sure to warn user that
  	     * it's only useful for DCE cables.
  	     */
	    printf("\nClock rate is ignored on DTE interfaces");
	} else if (csb->sense) {
	    printf("\nSetting clockrate for DTE's is not allowed");
	    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
	    return;
	}
    }

    /* 
     * Index through clockrate table to validate baud rate 
     * Index is saved; the different clockrate dividers are
     * obtained from the tables after reset. 
     */
    for (i = 0; i < nclocks; i++) {
	if (rate == clockrate_table[i].rate) {
	    ssb->serial_clockindex = i;
	    (void) (*idb->reset)(idb);
	    if (idb->counters.resets > 0)
		idb->counters.resets--;
	    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
	    return;
	}
    }
    printf("%%Unknown clock rate");
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
}


/*
 * Parse chains for serial interface commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_txclockint.h"
#include "cfg_int_txclockinvert.h"
#include "cfg_int_hssi.h"
LINK_POINT(cbus_serial_interface_commands, ALTERNATE);
#undef	ALTERNATE

void hes_serial_parser_init (void)
{
    parser_add_commands(PARSE_ADD_CFG_INT_AS_CMD,
			&pname(cbus_serial_interface_commands), "serial");
}

