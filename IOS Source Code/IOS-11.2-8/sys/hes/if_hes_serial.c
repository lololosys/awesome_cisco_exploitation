/* $Id: if_hes_serial.c,v 3.9.12.10 1996/09/11 04:59:49 snyder Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_hes_serial.c,v $
 *------------------------------------------------------------------
 * if_hes_serial.c -- Common RSP, cBus, MCI interface code
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_hes_serial.c,v $
 * Revision 3.9.12.10  1996/09/11  04:59:49  snyder
 * CSCdi68758:  spellink errors
 *              acceptible -> acceptable
 * Branch: California_branch
 *
 * Revision 3.9.12.9  1996/08/29  02:24:02  xtang
 * CSCdi60447:  FSIP max MTU=14816 not 18000
 * Branch: California_branch
 *              Implemented new FSIP command 93 to pass the available
 *              buffer size in the controller to the system.
 *              The corresponding microcode change are in
 *              cvs -d /micro fsip_q (fsip187-0)
 *              and cvs -d /micro fsip_i (rsp_fsip204-0).
 *
 * Revision 3.9.12.8  1996/08/28  12:45:01  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.12.7  1996/08/23  04:46:23  xtang
 * CSCdi59553:  clockrate 250000 is not configurable on FSIP in 75xx
 * Branch: California_branch
 *              added 250000 Hz to the FSIP interface card. Kept
 *              the AGS+ mciclock[] and mciclock_rev2[] untouched.
 *
 * Revision 3.9.12.6  1996/08/06  02:58:37  rramacha
 * CSCdi64889:  4T interface erroneously shows WFQ at vip console
 * Branch: California_branch
 *
 * Revision 3.9.12.5  1996/07/08  22:25:32  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.9.12.4  1996/05/28  20:36:47  shaker
 * CSCdi58549:  Notify users of non EOIR capable hardware
 * Branch: California_branch
 * Let users know why we're resetting the interface.
 *
 * Revision 3.9.12.3  1996/05/22  13:51:13  getchell
 * CSCdi57387:  corrupt pool pointer, VIP2 dies, box not healthy;VIP2
 * removed
 * Branch: California_branch
 *
 * Revision 3.9.12.2  1996/05/09  14:28:54  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.9.12.1.18.1  1996/04/27  07:02:26  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.9.12.1.8.1  1996/04/08  01:46:51  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.9.12.1  1996/03/18  19:41:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.16.1  1996/03/22  09:36:32  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.9  1996/02/13  21:26:45  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.8.8.1  1996/03/05  06:11:26  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.8.4.1  1996/02/27  20:42:08  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.8  1996/01/31  23:05:49  rharagan
 * CSCdi47670:  NRZI support does not work on 4T
 * Issue interface reset consistently after interface specific subcommand.
 *
 * Revision 3.7  1996/01/25  23:10:41  gstovall
 * CSCdi47390:  down-when-looped command not working (regression)
 * Remember to reset the itemlist before tossing it up to the next level.
 *
 * Revision 3.6.6.1  1996/01/24  21:46:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/01/11  03:50:24  kao
 * CSCdi43878:  IS_VIP() macro should be used
 *
 * Revision 3.5.2.1  1996/02/15  19:13:09  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.5  1995/12/23  01:13:19  rharagan
 * CSCdi45811:  4T interface linestate does not refletct modem control
 * lines
 * Enable the 'ignore-dcd' command for 4t, issue interface reset when
 * parsing crc 32 command.
 *
 * Revision 3.4  1995/12/02  05:50:12  rharagan
 * CSCdi41966:  Messages between 4T PA & driver should be per interface
 * Move to single interface commands, incorporate code review comments.
 *
 * Revision 3.3  1995/11/17  09:18:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:28:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:40:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:00:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/06  02:26:06  lbustini
 * CSCdi39576:  dialer dtr does not leave DTR low permanently after idle
 * timeout
 * Raise DTR only when a call is placed.
 *
 * Revision 2.5  1995/08/31  22:38:30  izhu
 * CSCdi32813:  SDLC Multidrops need router to ignore DCD for High-End
 * Platform
 * and
 * CSCdi38317  STUN: cannot IPL a 3725 using FSIP
 *
 * Convert to midrange signal model for line state (RTS/CTS state not
 * used),
 * permit DCD to be ignored.
 *
 * Revision 2.4  1995/08/17  07:44:36  sma
 * CSCdi37141:  FSIP port2 does not accept clockrate, if port 0,1 w/ G703
 * appliques
 *
 * Revision 2.3  1995/07/28  01:31:15  rharagan
 * CSCdi37725:  G.703-E1 LOF, AIS, LOS, RAI not reported on c7000
 * Shovel G.703 error indicators up to the RP.
 *
 * Revision 2.2  1995/06/21  03:27:09  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  20:41:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "subsys.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "cbus_registry.h"
#include "interface_private.h"
#include "../ibm/sdlc.h"
#include "../if/network.h"
#include "if_hes_common.h"
#include "if_mci.h"
#include "if_fci.h"
#include "logger.h"
#include "dbus.h"
#include "dgbus.h"
#include "if_cmd.h"
#include "if_hes_serial.h"
#include "if_vip.h"
#include "if_vip_4t.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "if_mci.h"
#include "../util/itemlist.h"
#include "../wan/dialer_registry.h"
#include "msg_cbus.c"			/* Not a typo, see logger.h */
#include "msg_oir.c"			/* Not a typo, see logger.h */


const mciclocktype fciclock[FCI_NCLOCKS] = {
 {0,        0, MCI_CLK_EXT},            /* external source */
 {1200,  2240, MCI_CLK_GEN3},           /*  1200.000 */
 {2400,  1120, MCI_CLK_GEN3},           /*  2400.000 */
 {4800,   560, MCI_CLK_GEN3},           /*  4800.000 */
 {9600,   280, MCI_CLK_GEN3},           /*  9600.000 */
 {19200,  140, MCI_CLK_GEN3},           /* 19200.000 */
 {38400,   70, MCI_CLK_GEN3},           /* 38400.000 */
 {56000,   48, MCI_CLK_GEN3},           /* 56000.000 */
 {64000,   42, MCI_CLK_GEN3},           /* 64000.000 */
 {72000,   56, MCI_CLK_GEN2},           /* 72000.000 */
 {125000,  65, MCI_CLK_GEN1},           /* 124061.540 */
 {148000,  54, MCI_CLK_GEN1},           /* 149333.333 */
 {250000,  32, MCI_CLK_GEN1},           /* 252000.000 */
 {500000,  16, MCI_CLK_GEN1},           /* 504000.000 */
 {800000,  10, MCI_CLK_GEN1},           /* 806400.000 */
 {1000000,  8, MCI_CLK_GEN1},           /* 1008000.000 */
 {1300000,  6, MCI_CLK_GEN1},           /* 1344000.000 */
 {2000000,  4, MCI_CLK_GEN1},           /* 2016000.000 */
 {4000000,  2, MCI_CLK_GEN1},           /* 4032000.000 */
 {8000000,  1, MCI_CLK_GEN1}            /* 8064000.000 */
};

const mciclocktype mciclock[NCLOCKS] = {
 {0,        0, MCI_CLK_EXT},            /* external source */
 {1200,  2222, MCI_CLK_GEN3},           /*  1200.120 */
 {2400,  1111, MCI_CLK_GEN3},           /*  2400.240 */
 {4800,   833, MCI_CLK_GEN2},           /*  4801.921 */
 {9600,   417, MCI_CLK_GEN2},           /*  9592.326 */
 {19200,  139, MCI_CLK_GEN3},           /* 19184.652 */
 {38400,  104, MCI_CLK_GEN2},           /* 38461.538 */
 {56000,   71, MCI_CLK_GEN2},           /* 56338.028 */
 {64000,   62, MCI_CLK_GEN2},           /* 64516.129 */
 {72000,   37, MCI_CLK_GEN3},           /* 72000.000 */
 {125000,  32, MCI_CLK_GEN2},           /* 125000.000 */
 {148000,  27, MCI_CLK_GEN2},           /* 148000.000 */
 {500000,   8, MCI_CLK_GEN2},           /* 500000.000 */
 {800000,   5, MCI_CLK_GEN2},           /* 800000.000 */
 {1000000,  4, MCI_CLK_GEN2},           /* 1000000.000 */
 {1300000,  3, MCI_CLK_GEN2},           /* 1300000.000 */
 {2000000,  2, MCI_CLK_GEN2},           /* 2000000.000 */
 {4000000,  1, MCI_CLK_GEN2},           /* 4000000.000 */
 {8000000,  1, MCI_CLK_GEN1}            /* 8000000.000 */
};

const mciclocktype mciclock_rev2[NCLOCKS] = {
 {0,        0, MCI_CLK_EXT},            /* external source */
 {1200,  2240, MCI_CLK_GEN3},           /*  1200 */
 {2400,  1680, MCI_CLK_GEN2},           /*  2400 */
 {4800,   560, MCI_CLK_GEN3},           /*  4800 */
 {9600,   280, MCI_CLK_GEN3},           /*  9600 */
 {19200,  140, MCI_CLK_GEN3},           /* 19200 */
 {38400,  105, MCI_CLK_GEN2},           /* 38400 */
 {56000,   72, MCI_CLK_GEN2},           /* 56000 */
 {64000,   63, MCI_CLK_GEN2},           /* 64000 */
 {72000,   56, MCI_CLK_GEN2},           /* 72000.000 */
 {125000,  32, MCI_CLK_GEN2},           /* 125000.000 */
 {148000,  27, MCI_CLK_GEN2},           /* 148000.000 */
 {500000,   8, MCI_CLK_GEN2},           /* 500000.000 */
 {800000,   5, MCI_CLK_GEN2},           /* 800000.000 */
 {1000000,  4, MCI_CLK_GEN2},           /* 1000000.000 */
 {1300000,  3, MCI_CLK_GEN2},           /* 1300000.000 */
 {2000000,  2, MCI_CLK_GEN2},           /* 2000000.000 */
 {4000000,  1, MCI_CLK_GEN2},           /* 4000000.000 */
};


/*
 * hssi_loop_request
 * Set the loop request flag in the idb and reset the interface.
 *
 * CAUTION: this routine must have interrupts disabled before calling.
 */
void hssi_loop_request (hwidbtype *idb)
{
    ushort mode;
    serialsb *ssb;

    select_if(idb);

    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);

    (void)send_hip_read_loop_cmd(idb, &mode);
    if (mode != ssb->hssi_loop_req) {
	ssb->hssi_loop_req = mode;
	(*idb->reset)(idb);
	if (idb->counters.resets > 0)
	    idb->counters.resets--;
    }

    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);
}


/*
 * Print out DTR, CTS, etc. on FSIP card
 */
void fsip_show_serial_state (hwidbtype* idb, char* leader)
{
    const char *fsip_line_state_fmt_str0 = "\n     RTS %s, CTS %s, DTR %s, DCD %s, DSR %s"; 
    const char *fsip_line_state_fmt_str1 = "\n     BER %s, NELR %s, FELR %s"; 

    ushort signal_status;
    leveltype level;
    vip4t_data_t *vip4t_data;

    level = raise_interrupt_level(NETS_DISABLE);

    if (!is_vip(idb)) {
	select_if(idb);
	if (send_fsip_signal_sts_cmd(idb, &signal_status) != MCI_RSP_OKAY) {
	    reset_interrupt_level(level);
	    return;
	}
    } else {
	vip4t_data = idb->vip4t_data;
	if (!vip4t_data) {
	    reset_interrupt_level(level);
	    return;
	}
	signal_status = vip4t_data->signal_status;
    }
    reset_interrupt_level(level);

    if (idb->dte_interface) {
        printf(fsip_line_state_fmt_str0,
               (signal_status & FSIP_MASK_RTS) ? "up" : "down",
               (signal_status & FSIP_MASK_CTS) ? "up" : "down",
               (signal_status & FSIP_MASK_DTR) ? "up" : "down",
               (signal_status & FSIP_MASK_DCD) ? "up" : "down",
               (signal_status & FSIP_MASK_DSR) ? "up" : "down");
        if (is_g703(idb)) {
	    /* G.703 applique type presently identified as DTE's */
	    printf(fsip_line_state_fmt_str1,
		   (signal_status & FSIP_MASK_BER) ? "active" : "inactive",
		   (signal_status & FSIP_MASK_NELR) ? "active" : "inactive",
		   (signal_status & FSIP_MASK_FELR) ? "active" : "inactive");
	} 
    } else {
        /* The rest of the DCE types */
        printf(fsip_line_state_fmt_str0, 
               (signal_status & FSIP_MASK_CTS) ? "up" : "down",
               (signal_status & FSIP_MASK_RTS) ? "up" : "down",
               (signal_status & FSIP_MASK_DCD) ? "up" : "down",
               (signal_status & FSIP_MASK_DTR) ? "up" : "down",
               (signal_status & FSIP_MASK_DTR) ? "up" : "down");
    }
}


/*
 * ignore_dcd_control
 * disable dcd checking
 */
static void ignore_dcd_control (hwidbtype* idb, boolean enable)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(idb);
    (void)send_fsip_ignore_dcd_cmd(idb, enable);

    reset_interrupt_level(level);

}


/**************************************************************************
 * Name: fsip_check_mtu
 *
 * Description: check the size of the available rx and tx buffer
 *   in the daughter controller
 *   
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *   mtu   -- mtu size to be set
 *
 * Return Value: 
 *   boolean -- TRUE = size allright, 
 *              FALSE = mtu size too large
 *
 ***********************************************************************/
boolean fsip_check_mtu (hwidbtype *hwidb, ushort mtu)
{
    ushort max_mtu_supported = 0;
    leveltype level;

    int buffersize_delta = hwidb->buffersize - hwidb->maxmtu;

    if (IS_VIP(hwidb)) return (TRUE);

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);
    send_fsip_max_mtu_supported_cmd(hwidb, &max_mtu_supported);

    reset_interrupt_level(level);

    if ((mtu + buffersize_delta) > max_mtu_supported) {
        printf("\n%%Mtu of size %d is too large for this interface %s",
             mtu, hwidb->hw_namestring);
	printf("\n%%The maximum mtu acceptable is %d.",
             (max_mtu_supported - buffersize_delta));
	return (FALSE);
    }

    return (TRUE);
}

/*
 * crc4_control
 * Enable/disable CRC4 checking on G.703 serial interfaces.
 */
static void crc4_control (hwidbtype* idb, boolean enable)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(idb);
    (void)send_fsip_crc4_cmd(idb, enable);

    reset_interrupt_level(level);


}

/*
 * ts16_control
 * Enable/disable CRC4 checking on G.703 serial interfaces.
 */
static void ts16_control (hwidbtype* idb, boolean enable)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(idb);
    (void)send_fsip_ts16_cmd(idb, enable);

    reset_interrupt_level(level);

}

/*
 * clocksource_control
 * Choose clock source on G.703 serial interfaces.
 */
static void clocksource_control (hwidbtype* idb, boolean enable)
{
    leveltype level;


    level = raise_interrupt_level(NETS_DISABLE);

    select_if(idb);
    (void)send_fsip_clocksource_cmd(idb, enable);

    reset_interrupt_level(level);

}

/*
 * timeslot_control
 * Set timeslot range on G.703 serial interfaces.
 */
static void timeslot_control (hwidbtype* idb)
{
    leveltype level;
    serialsb *ssb;


    level = raise_interrupt_level(NETS_DISABLE);
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    select_if(idb);
    (void)send_fsip_start_slot_g703_cmd(idb, ssb->start_slot);
    (void)send_fsip_stop_slot_g703_cmd(idb, ssb->stop_slot);

    reset_interrupt_level(level);

}

/*
 * Get txqlength on FSIP card
 */
int fsip_get_txqlength (hwidbtype* idb)
{
    ushort result;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    (void)select_if(idb);
    (void)send_txqlength_cmd(idb, &result);
    reset_interrupt_level(level);
    return(result);
}
/*
 * short_txq_localbuf
 * pass cp/q setting to the interface.
 */
static void short_txq_localbuf (hwidbtype* idb, ushort enable)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);

    select_if(idb);
    (void)send_short_txq_localbuf_cmd(idb, enable);

    reset_interrupt_level(level);
}


/*
 * mci_applique
 * Set name, if any, of interface applique.
 * N.B.: For this function to work correctly, the caller MUST be
 *       be running at interrupt level of have interrupts disabled.
 */

const ushort app_cmds[APP_COUNT] = {0, 1, 2, 3, 0, 4, 5, 6, 7, 4, 0};
const uchar app_uses[APP_COUNT] =  {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0};

void mci_applique (hwidbtype *idb)
{
    ushort result, short_code;
    int i, code, shift;
    char *str;
    vip4t_data_t *vip4t_info = idb->vip4t_data;    

    idb->applique_name = NULL;

    if (idb->status & IDB_POS) {
	idb->applique_name = "SONET";
	return;
    }

    /*
     * If HSSI interface, no need to read the "applique".
     */
    if (idb->status & IDB_HSSI) {
	idb->applique_name = "Hssi DTE";
	return;
    }

    /*
     * MIP interfaces have no need to read the applique either
     */
    switch (idb->type) {
    case IDBTYPE_T1:
    case IDBTYPE_E1:
	idb->applique_name = reg_invoke_dsx1_applique_name(idb->mci_index);
	return;

    default:
	break;		/* make the compiler shut up */
    }

    /*
     * If not a Cbus or MCI serial interface, exit.
     */
    if (!(idb->status & IDB_SERIAL))
	return;
    
    /*
     * This code assumes we have been called with interrupts disabled.
     */
    (void)select_if(idb);

    if (idb->status & IDB_CBUS) {
	/*
	 * The cbus serial controller (FSIP) returns the
	 * applique type directly.
	 */
 	if (is_vip(idb)) {
 	    code = (int) vip4t_info->applique_id;
 	} else {
 	    short_code = 0;
 	    send_applique_cmd(idb, 0, &short_code);
 	    code = short_code;
 	}
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

    idb->dte_interface = TRUE;
    idb->app_type = APPTYPE_5IN1; /* default port adapter */
    idb->serial_flags &= ~SERIAL_FLAGS_NO_CLOCKRATE; /* default for clockrate */

    switch (code) {
    case MCI_UNIVERSE_NO_CABLE:
        str = "Universal (cable unattached)";
        idb->dte_interface = FALSE;
        break;
    case MCI_DTE_RS232: 
	str = "RS-232 DTE";
	idb->dte_interface = TRUE;
	break;
    case MCI_DCE_RS232:
	str = "RS-232 DCE";
	idb->dte_interface = FALSE;
	break;
    case MCI_DTE_V35:
	str = "V.35 DTE"; 
	idb->dte_interface = TRUE;
	break; 
    case MCI_DCE_V35:
	str = "V.35 DCE";
	idb->dte_interface = FALSE;
	break;
    case MCI_DTE_RS449:
	str = "RS-449 DTE";
	idb->dte_interface = TRUE;
	break;
    case MCI_DCE_RS449:
	str = "RS-449 DCE";
	idb->dte_interface = FALSE;
	break;
    case MCI_DTE_X21:
	str = "X.21 DTE";
	idb->dte_interface = TRUE;
	break;
    case MCI_DCE_X21:
	str = "X.21 DCE";
	idb->dte_interface = FALSE;
	break;
    case MCI_DTE_RS232_NRZI:
	str = "RS-232 NRZI DTE";
	idb->dte_interface = TRUE;
	break;
    case MCI_DCE_RS232_NRZI:
	str = "RS-232 NRZI DCE";
	idb->dte_interface = FALSE;
	break;
    case MCI_DXE_G703_UNBAL:
	str = "G.703 Unbalanced";
	idb->dte_interface = TRUE;
	idb->app_type = APPTYPE_G703_UNBAL;
	idb->serial_flags |= SERIAL_FLAGS_NO_CLOCKRATE;
 	break;
    case MCI_DXE_G703_BAL:
	str = "G.703 Balanced";
	idb->dte_interface = TRUE;
	idb->app_type = APPTYPE_G703_BAL; 
	idb->serial_flags |= SERIAL_FLAGS_NO_CLOCKRATE;
	break;
    case MCI_DTE_RS530:
	str = "RS-530 DTE";
	idb->dte_interface = TRUE;
	break;
    case MCI_DCE_RS530:
	str = "RS-530 DCE";
        idb->dte_interface = FALSE;
	break;
    case MCI_DTE_V35_NRZI:
	str = "V.35 NRZI DTE";
	idb->dte_interface = TRUE;
	break;
    case MCI_DCE_V35_NRZI:
	str = "V.35 NRZI DCE";
	idb->dte_interface = FALSE;
	break;
    default: 
	str = "unknown";
	break;
    }

    /*
     * Save descriptive string in mci structure, for show controller mci.
     */
    idb->applique_name = str;
}	


/*
 * Read half duplex related error counts
 * Call this only when (idb->sdlc_flags & SDLC_FLAG_HDX)
 */

static void get_half_duplex_errors (hwidbtype *idb)
{
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    select_if(idb);
    idb->rts_timeout_count += send_errors_cmd(idb, MCI_ERR_RTS_TIMEOUT);
    idb->half_duplex_violation +=
	send_errors_cmd(idb, MCI_ERR_HALF_DUPLEX_VIOLATION);
    reset_interrupt_level(level);
}


/*****************************************************************************
 * Name: get_serial_cbus_errors
 *
 * Description: Accumulates cBus errors from serial interfaces (including HSSI)
 *   Called by get_cbus_errors
 *
 * Assumptions:
 *   1. interrupts are disabled
 *   2. interface has been selected
 *
 * Input: 
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 * 
 *****************************************************************************/
static void get_serial_cbus_errors (hwidbtype *hwidb)
{
    long counts;
    serialsb *ssb;

    counts = send_errors_cmd(hwidb, FCI_ERR_NOBUFFERS);

    /*
     * The FEIP or any VIP interface  
     * get their error statistics via love letters or IPC.  A single
     * send_errors_cmd triggers the IP to send the msg to the RP.  Use
     * the above single cbus command as that trigger.
     */
    if ((hwidb->status & IDB_POS) || IS_VIP(hwidb))
      return;
 
    hwidb->counters.input_resource += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_CRC);
    hwidb->counters.input_crc += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_FRAME);
    hwidb->counters.input_frame += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_GIANTS);
    hwidb->counters.giants += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_RX_OVERRUN);
    hwidb->counters.input_overrun += counts;
    hwidb->counters.input_error += counts;
    
    counts = send_errors_cmd(hwidb, FCI_ERR_TX_UNDERFLOW);
    hwidb->counters.output_underrun += counts;

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    
    if (hwidb->status & IDB_HSSI) {
	/*
	 * HSSI (link controller -- DS3) ONLY
	 */
	counts = send_errors_cmd(hwidb, FCI_ERR_APPLIQUE);
	ssb->applique += counts;
	
	counts = send_errors_cmd(hwidb, FCI_ERR_PARITY);
	hwidb->counters.parity += counts;
	hwidb->counters.input_error += counts;
	
	counts = send_errors_cmd(hwidb,FCI_ERR_RX_THROTTLE);
	ssb->rx_disabled += counts;
    } else {
	/*
	 * NON-HSSI serial lines only
	 */
	counts = send_errors_cmd(hwidb, FCI_ERR_RX_EXPLICIT_ABORT);
	hwidb->counters.input_abort += counts;
	hwidb->counters.input_error += counts;

	if (is_g703(hwidb)) {
	    counts = send_errors_cmd(hwidb, FCI_ERR_RX_AIS);
	    ssb->rx_ais += counts;
	    counts = send_errors_cmd(hwidb, FCI_ERR_RX_RAI);
	    ssb->rx_rai += counts;
	    counts = send_errors_cmd(hwidb, FCI_ERR_RX_LOS);
	    ssb->rx_los += counts;
	    counts = send_errors_cmd(hwidb, FCI_ERR_RX_LOF);
	    ssb->rx_lof += counts;
        }
    }

    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
}

/*
 * serial_eoir_capable_hw_version
 * Return true if this serial interface hardware is EOIR capable
 */
#if defined(C7000) || defined(RSP)
static boolean serial_eoir_capable_hw_version (hwidbtype *hwidb)
{
    if (hwidb->status & IDB_HSSI)
	return(TRUE);

    if (hwidb->type == IDBTYPE_CBUSSERIAL) {
	ushort hw_version;

	hw_version = dbus_hw_version(pslot_to_dbus(hwidb->slot));
	if (hw_version == FSIP_NO_EOIR_VERSION)
	    return(FALSE);
    }
    return(TRUE);
}
#endif

/*****************************************************************************
 * Name: cbus_mci_serial_reset
 *
 * Description: Reset a cBus or MCI Serial interface.
 *   This function is used in the "hwidb->reset" vector in the IDB.
 *   Note: changes you make here might have to be made also in
 *         mip_interface_reset for the MIP card.
 *
 * Input:
 *   hwidb -- pointer to the hardware IDB for the interface
 *
 * Return Value: None
 *
 *****************************************************************************/
void cbus_mci_serial_reset (hwidbtype *hwidb)
{
    ushort status;
    boolean req;
    leveltype level;
    boolean eoir_reset;
    ushort ta;
    ushort iclk;
    ushort lc;
    ushort flags, short_txq;
    serialsb *ssb;

    SAVE_CALLER();

    eoir_reset = FALSE;
    short_txq = FALSE;

#if defined(C7000) || defined(RSP)
    if (hwidb->status & IDB_NOHARDWARE) {
	errmsg(&msgsym(RESETNXI, CBUS), hwidb, hwidb->hw_namestring);
	return;
    }
    
    /*
     * Don't loop forever in reset when dbus detects board failure
     * and disables the controller. 
     */
    if (!reg_invoke_slot_enabled(hwidb->cbus_ctrlr_index)) {
	/*
	 * Card has been disabled due to some failure.
	 * Make sure state gets moved to down.
	 */
	hwidb->state = IDBS_DOWN;
	return;
    }

    if (hwidb->oir_flags & EOIR_RESET) {
	if (serial_eoir_capable_hw_version(hwidb)) {
	    eoir_reset = TRUE;
	} else {
	    ushort hw_version =
	      dbus_hw_version(pslot_to_dbus(hwidb->slot));
	    eoir_reset = FALSE;
	    errmsg(&msgsym(NOEOIR, OIR), hwidb->hw_namestring,
		   "hardware", (hw_version >> 8), (hw_version & 0xFF));
	}
    }
#endif

    hwidb->counters.resets++;
    level = raise_interrupt_level(NETS_DISABLE);
    if (!eoir_reset) {
	hwidb->status &= ~IDB_DISABLED;
	reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
    }

    select_if(hwidb);
    status = send_if_noloopback_cmd(hwidb);
    if (!eoir_reset) {
	send_if_disable_cmd(hwidb);
    }

    /*
     * Note, that the units (bytes Vs words) used in the rxoffset
     * field is platform dependant.  On the RSP, the value represents
     * bytes (on other platforms, like the 7000, this value
     * represents words).  The IP ucode on each platform has been
     * modified to expect the correct units for that platform.
     */
    platform_serial_reset_encsize(hwidb);

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (hwidb->status & IDB_CBUS) {
	if (!eoir_reset) {
	    /*
	     * The delays are reportedly to give time for buffers
	     * to return, and are not related to how long it takes
	     * for the IPs to respond with the DONE bit in the CCB.
	     */
	    if (hwidb->status & IDB_HSSI) {
		/* HSSI takes a while to disable */
		status = send_if_disable_cmd_delay(hwidb, HSSI_DISABLE_TIME);
	    } else {
		/* FSIP just needs a couple millisecs */
		status = send_if_disable_cmd_delay(hwidb, 2);
	    }

	    if (status != FCI_RSP_OKAY) {
		if (hwidb->status & IDB_HSSI)
		    errmsg(&msgsym(HSSIRSET, CBUS), hwidb->hw_namestring,
			   status, "disable");
		else
		    errmsg(&msgsym(FSIPRSET, CBUS), hwidb->hw_namestring,
			   status, "disable");

		platform_req_card_restart(hwidb);

		reset_interrupt_level(level);
		idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
		return;
	    }
	
	    if (serial_debug)
		buginf("\n%s: Reset from PC %#lx", hwidb->hw_namestring,
		       caller());

	    /*
	     * Perform the appropriate platform specific reset action for
	     * the interface.
	     */
	    platform_serial_reset_action(hwidb);
	}

	/*
	 * Perform the appropriate platform specific initialization for
	 * the interface.
	 */
	platform_serial_reset_init(hwidb);

	if (!eoir_reset) {
	    reg_invoke_crc_control(hwidb->type, hwidb, ssb->crc_32_enabled);

	    reg_invoke_crc4_control(hwidb->type, hwidb, ssb->crc4_enabled);
	    reg_invoke_ts16_control(hwidb->type, hwidb, 
				    ssb->timeslot16_enabled);
	    reg_invoke_timeslot_control(hwidb->type, hwidb);
	    reg_invoke_clocksource_control(hwidb->type, hwidb,
					   ssb->clocksource_enabled);
	    reg_invoke_ignore_dcd_control(hwidb->type, hwidb, ssb->ignore_dcd);
 
	}
    }

    /*
     * Flush hold queue
     */
    if (!eoir_reset)
	holdq_reset(hwidb);

    /*
     * If tql is tuned down, then turn-off tx local buffering.
     */
    if (hwidb_use_short_tx_q_inline(hwidb))
 	short_txq = TRUE;

    if (!eoir_reset) {
	if (!(hwidb->status & IDB_HSSI)) {
	    if (hwidb->type == IDBTYPE_CBUSSERIAL) {
		select_if(hwidb);
		send_alt_idle_cmd(hwidb, (ushort) ((hwidb->serial_flags &
		    SERIAL_FLAGS_DO_ALT_FLAG) ? TRUE : FALSE));
		send_clockrate_cmd(hwidb,
		    fciclock[ssb->serial_clockindex].cookie);
		send_clocktype_cmd(hwidb,
		    fciclock[ssb->serial_clockindex].prescalar);
	    } else { 
		/*
		 * Perform the appropriate platform specific clockrate
		 * initialization for the interface.
		 */
		platform_serial_reset_clockrate(hwidb);
	    }

	    /* 
	     * set various options
	     */

	    reg_invoke_internal_txclock_control(hwidb->type, hwidb,
					ssb->dce_terminal_timing_enable);
	    reg_invoke_nrzi_control(hwidb->type, hwidb, ssb->nrzi_enable);
	    reg_invoke_invert_txclock_control(hwidb->type, hwidb,
					      ssb->invert_txc);
	    reg_invoke_short_txq_localbuf(hwidb->type, hwidb, short_txq);
	}
	/*
	 * Set transmitter delay counter
	 */
	reg_invoke_media_transmit_delay(hwidb->type, hwidb, hwidb->xmitdelay);
    }

    /*
     * If we pulse DTR on a reset, make sure we start the pulse timer.
     * Clear timer and finish reset if pulse timer has expired.
     * This code is useful for encryption boxes that need DTR pulses
     * to re-synchronize.
     */
    if (!eoir_reset) {
	if (!serial_pulse_DTR(hwidb)) {
	    reset_interrupt_level(level);
	    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
	    return;
	}
	/*
	 * If a DTR dialer is configured on this interface we may not want
	 * to raise DTR, so return here if this is the case.
	 */
	if (is_ddr(hwidb)) {
	    if (reg_invoke_dialer_keep_dtr_low(hwidb)) {
		reset_interrupt_level(level);
		return;
	    }
	}
    }

    /*
     * Re-enable interface.
     * This clears loopback and lights green applique light.
     */
    cbus_mci_txlimit(hwidb);
    if (!eoir_reset)
	mci_applique(hwidb);

    /*
     * If interface is administratively down, don't re-enable
     */
    if (hwidb->state == IDBS_ADMINDOWN) {
	if (eoir_reset)
	    send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);
        reset_interrupt_level(level);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
        return;
    }

    req = (ssb->hssi_loop_req && !ssb->hssi_lc_inhibit);
    if (hwidb->status & IDB_HSSI) {
	select_if(hwidb);

	/*
	 * Set the TA bit on the hssi interface appropriately.
	 */
	ta = (req || (!req && !hwidb->loopback)) ? 1 : 0;
	send_hip_ta_cmd(hwidb, ta);

	/*
	 * Set internal or external clocking.  The rate is determined
	 * by hardware rather than software as in the mci dce applique.
	 */
	iclk = ssb->hssi_iclk ? HSSI_CLK_INTERNAL : HSSI_CLK_EXTERNAL;
	send_hip_iclk_cmd(hwidb, iclk);

	/*
	 * Set the LC inhibit appropriately.
	 */
	lc = ssb->hssi_lc_inhibit ? HSSI_LCI_ON : HSSI_LCI_OFF;
	send_hip_lc_inhibit_cmd(hwidb, lc);
    }

    /*
     *  Add SDLC HDX support.
     */
    if (!eoir_reset) {
        if ((hwidb->status & (IDB_SERIAL|IDB_MCI)) == (IDB_SERIAL|IDB_MCI)) {
            if (hwidb->hdx_flags & HDX_FLAG_CAPABLE) {
		select_if(hwidb);
                if (hwidb->hdx_flags & HDX_FLAG_ON)
                    flags = MCI_IF_HDX | MCI_SDLC_ON_MASK;
                else
                    flags = MCI_IF_HDX & ~MCI_SDLC_ON_MASK;
		send_status_cmd(hwidb, flags);
		send_cts_delay_cmd(hwidb, ssb->hdx_cts_delay * 
                                   MCI_COUNTS_PER_MSEC + HDX_CTS_DELAY_OFFSET); 
		send_cts_watchdog_cmd(hwidb, ssb->hdx_rts_timeout * MCI_COUNTS_PER_MSEC);
		if (ssb->serial_clockindex) {
		    send_status_cmd(hwidb, MCI_IF_DCE | MCI_SDLC_ON_MASK);
		}
            }
        }
    }

    /*
     * Set loopback state.  This sets a flag in the FSIP that is checked
     * when the interface is enabled with the MCI_IF_ENABLE command below.
     */
    if ((hwidb->status & IDB_CBUS) && (!(hwidb->status & IDB_HSSI))) {
	if (hwidb->loopback || req) {
	    send_if_loopback_cmd(hwidb);
	} else {
	    send_if_noloopback_cmd(hwidb);
	}
    }

    if (hwidb->status & IDB_POS) {
	select_if(hwidb);
	send_specific_cmd_1arg(hwidb,FCI_POSIP_SPECIFIC, ssb->posi_clock ?
			       POSIP_CLOCK_INTERNAL : POSIP_CLOCK_EXTERNAL);
	if (hwidb->loopback)
	    send_specific_cmd_1arg(hwidb, FCI_POSIP_SPECIFIC, 
				   hwidb->ext_loop_type);
	else
	    send_specific_cmd_1arg(hwidb, FCI_POSIP_SPECIFIC, POSIP_LOOP_NONE);
    }

    if (eoir_reset) {
	TIMER_STOP(hwidb->xmittimer);
	status = send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);
    } else {
	status = send_if_enable_cmd(hwidb);
    }
    if (status != MCI_RSP_OKAY) {
	errmsg(&msgsym(RSETFAIL, CBUS), hwidb->hw_namestring, 
	       "cbus_mci_serial_reset()", status);
	reset_interrupt_level(level);
	idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
	return;
    }
    
    /*
     * Set loopback state.  Loopback is cleared by MCI_IF_ENABLE function.
     */
    if (hwidb->loopback || req) {
	send_if_loopback_cmd(hwidb);
    } else {
	send_if_noloopback_cmd(hwidb);
    }

    if (hwidb->status & IDB_HSSI) {
	select_if(hwidb);

	/*
	 * Diddle the loopback mode on the HSSI
	 */
	send_hip_loopback_cmd(hwidb, hwidb->ext_loop_type);
    }

    reset_interrupt_level(level);
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
}

/*
 * crc_control
 * Enable 32 bit Ethernet CRC's on FSIP or HIP serial interfaces.
 */
static void crc_control (hwidbtype* idb, boolean enable)
{
    leveltype level;
    serialsb *ssb;

    if (idb->status & IDB_CBUS) {
	ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
	level = raise_interrupt_level(NETS_DISABLE);
	select_if(idb);
	(void)send_serial_crc_32_cmd(idb, ssb->crc_32_enabled);
	reset_interrupt_level(level);
    }
}

static void fsip_nrzi_control (hwidbtype* idb, boolean enable)
{
    leveltype level;
    serialsb *ssb;

    if (idb->status & IDB_CBUS) {
	ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
	level = raise_interrupt_level(NETS_DISABLE);
	select_if(idb);
	(void)send_fsip_nrzi_cmd(idb, ssb->nrzi_enable);
	reset_interrupt_level(level);
    }
}

static void fsip_invert_txc_control (hwidbtype* idb, boolean enable)
{
    leveltype level;
    serialsb *ssb;

    if (idb->status & IDB_CBUS) {
	level = raise_interrupt_level(NETS_DISABLE);
	select_if(idb);
	/* 
	 * Actually, for DTE & DCE 
	 */
	ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
	(void)send_fsip_tx_invert_clk_cmd(idb, ssb->invert_txc);
	reset_interrupt_level(level);
    }
}

/*
 * Formerly cbus_set_rxoffset but only used by the this module. Other
 * IP's perform this function differently.
 */ 
static void cbus_set_serial_rxoffset (hwidbtype *hwidb)
{
    hwidb->reset(hwidb);

}

/*
 * hes_serial_max_transmit_delay - return the maximum value
 * for the transmitter-delay config command.
 */
static ulong hes_serial_max_transmit_delay (hwidbtype *hwidb)
{
    return ((hwidb->fci_type == FCI_TYPE_VIP_SERIAL4) ? 15 : 0x1FFFF);
}


/*
 * hes_g703_print_version - print out the version of G.703 software that
 * we are running.  This is added to the print_copyright registration 
 * point and invoked by Print_Hardware().
 */
static void hes_g703_print_version (void)
{
    /*
     *	DO NOT CHANGE THE FOLLOWING G.703 VERSION IDENTIFICATION
     *  Put in place for BABT certification, others to follow perhaps.
     *  our certifications are keyed to this identification and any change
     *  will mean large commitments of time and money for recertification
     */

    if ((cpu_type == CPU_RP1) || (cpu_type == CPU_RSP)) {
	printf("G.703/E1 software, Version 1.0.\n");
    }
}

static boolean hes_serial_hw_set_config (hwidbtype *hwidb, itemlist *ilist,
					 serialsb *ssb)
{
    itemdesc *item;
    boolean any_unhandled = FALSE;   /* assume we've handled all items */
    tinybool this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;

	switch (item->item) {

	  case SERIAL_HW_DCE_TERMINAL_TIMING_ENABLE:
	    ssb->dce_terminal_timing_enable = item->u.l_value;
	    reg_invoke_internal_txclock_control(hwidb->type, hwidb,
						ssb->dce_terminal_timing_enable);
	    break;

	  case SERIAL_HW_INVERT_TXC:
	    ssb->invert_txc = item->u.l_value;
	    (*(hwidb->reset))(hwidb);
	    break;

	  case SERIAL_HW_NRZI_ENABLE:
	    ssb->nrzi_enable = item->u.l_value;
	    (*(hwidb->reset))(hwidb);
	    break;

	  case SERIAL_HW_CRC4_ENABLED:
	    ssb->crc4_enabled = item->u.l_value;
	    (*(hwidb->reset))(hwidb);
	    break;

	  case SERIAL_HW_TIMESLOT16_ENABLED:
	    ssb->timeslot16_enabled = item->u.l_value;
	    (*(hwidb->reset))(hwidb);
	    break;

	  case SERIAL_HW_IGNORE_DCD:
	    ssb->ignore_dcd = item->u.l_value;
	    (*(hwidb->reset))(hwidb);
	    break;

	  case SERIAL_HW_SET_CLOCKRATE:
	    clockrate_command(item->u.buffer);
	    break;

	  case SERIAL_HW_RTS_TIMEOUT:
	    ssb->hdx_rts_timeout = item->u.l_value;
	    (*(hwidb->reset))(hwidb);
	    break;

	  case SERIAL_HW_CTS_DELAY:
	    ssb->hdx_cts_delay = item->u.l_value;
	    (*(hwidb->reset))(hwidb);
	    break;

	  default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}

	if (!this_item_unhandled)
	    item->handled = TRUE;
    }

    return(!any_unhandled);
}

static boolean hes_serial_hwidb_state_config (hwidbtype *hwidb, ulong opcode,
					      itemlist *ilist)
{
    boolean ret;
    serialsb *ssb;

    if ((hwidb == NULL) && (ilist == NULL))
	return (FALSE);

    if (ilist->item_count == -1)
	ilist->item_count = itemlist_count(ilist);

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    if (ssb == NULL)
	return (FALSE);

    switch (opcode) {
      case IDB_CONTROL_SET_CONFIG:
	ret = hes_serial_hw_set_config(hwidb, ilist, ssb);
	break;

#if 0
      case IDB_CONTROL_READ_CONFIG:
	ret = serial_hw_read_config(hwidb, ilist, ssb);
	break;
	
      case IDB_CONTROL_SET_STATE:
	ret = serial_hw_set_state(hwidb, ilist, ssb);
	break;
	
      case IDB_CONTROL_READ_STATE:
	ret = serial_hw_read_state(hwidb, ilist, ssb);
	break;

#endif
      default:
	ret = FALSE;
	break;
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (!ret) {
	itemlist_reset(ilist);
	ret = (*ssb->hw_super_state_config)(hwidb, opcode, ilist);
    }
    return (ret);

}

void hes_serial_idb_init (hwidbtype *hwidb)
{
    serialsb *ssb;

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    if (hwidb->state_config != hes_serial_hwidb_state_config) {
	ssb->hw_super_state_config = hwidb->state_config;
	hwidb->state_config = hes_serial_hwidb_state_config;
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
}


static void hes_serial_subsys_init (subsystype* subsys)
{
    /*
     * All the serial controllers share the same get_errors routine,
     * so we register it for both controllers.
     */
    reg_add_cbus_get_ip_errors(FCI_TYPE_SERIAL, get_serial_cbus_errors,
			       "get_serial_cbus_errors");
    reg_add_cbus_get_ip_errors(FCI_TYPE_HSSI, get_serial_cbus_errors,
			       "get_serial_cbus_errors");
    reg_add_cbus_get_ip_errors(FCI_TYPE_VIP_SERIAL4, get_serial_cbus_errors,
			       "get_serial_cbus_errors");

    reg_add_media_check_mtu(IDBTYPE_CBUSSERIAL, 
			    fsip_check_mtu, 
			    "fsip_check_mtu");
    reg_add_crc_control(IDBTYPE_CBUSSERIAL, crc_control,
			 "crc_control");
    reg_add_crc4_control(IDBTYPE_CBUSSERIAL, crc4_control,
                           "crc4_control");
    reg_add_ts16_control(IDBTYPE_CBUSSERIAL, ts16_control,
                           "ts16_control");
    reg_add_clocksource_control(IDBTYPE_CBUSSERIAL, clocksource_control,
                           "clocksource_control");
    reg_add_timeslot_control(IDBTYPE_CBUSSERIAL, timeslot_control,
                           "timeslot_control");
    reg_add_nrzi_control(IDBTYPE_CBUSSERIAL, fsip_nrzi_control,
			 "fsip_nrzi_control");
    reg_add_invert_txclock_control(IDBTYPE_CBUSSERIAL, fsip_invert_txc_control,
				   "fsip_invert_txc_control");
    reg_add_short_txq_localbuf(IDBTYPE_CBUSSERIAL, short_txq_localbuf,
                           "short_txq_localbuf");
    reg_add_print_copyright(hes_g703_print_version, "hes_g703_print_version");


    reg_add_media_max_transmit_delay(hes_serial_max_transmit_delay,
				     "hes_serial_max_transmit_delay");
    reg_add_crc_control(IDBTYPE_HSSI, crc_control,
			 "crc_control");

    reg_add_get_hdpx_errors(get_half_duplex_errors, "get_half_duplex_errors");

    /*
     * Add parser callbacks
     */
    reg_add_media_set_rxoffset(IDBTYPE_CBUSSERIAL, cbus_set_serial_rxoffset,
                                "cbus_set_serial_rxoffset");
    reg_add_media_set_rxoffset(IDBTYPE_HSSI, cbus_set_serial_rxoffset,
                                "cbus_set_serial_rxoffset");
    reg_add_ignore_dcd_control(IDBTYPE_CBUSSERIAL, ignore_dcd_control,
			       "ignore_dcd_control");
    /*
     * Initialize serial parser support
     */
    hes_serial_parser_init();

    serial_platform_init(subsys);
}

/*---------------------------------------------------------------------------
 * Subsystem Header for Serial Driver
 *---------------------------------------------------------------------------*/
SUBSYS_HEADER(hes_serial,		/* name */
	      1,			/* major version */
	      0,			/* minor version */
	      1,			/* edit version */
	      hes_serial_subsys_init,	/* init function */
	      SUBSYS_CLASS_DRIVER,	/* subsystem class */
	      "seq: cbus_mci",		/* sequence list */
	      "req: cbus_mci"		/* prerequisite list */
	      );
