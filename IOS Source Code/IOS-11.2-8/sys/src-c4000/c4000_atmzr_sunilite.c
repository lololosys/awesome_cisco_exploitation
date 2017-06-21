/* $Id: c4000_atmzr_sunilite.c,v 3.6.12.1 1996/03/18 22:00:42 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-c4000/c4000_atmzr_sunilite.c,v $
 *------------------------------------------------------------------
 * C4000 router family ATMizer SUNI-LITE PLIM (STS-3c) driver.
 *
 * October, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: c4000_atmzr_sunilite.c,v $
 * Revision 3.6.12.1  1996/03/18  22:00:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/13  02:00:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.12.2  1996/03/07  10:43:50  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  17:57:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/13  08:15:53  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.5  1996/02/01  06:09:18  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/06  03:23:11  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  18:45:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:32:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:26:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/27  00:31:01  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.1  1995/06/07  22:56:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*******************************************************************
			Include Files
 *******************************************************************/

#include "master.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../ui/parse_util.h"
#include "../atm/parser_defs_atm.h"
#include "address.h"
#include "logger.h"
#include "../os/buffers.h"
#include "../if/network.h"
#include "../if/atm_debug.h"
#include "../if/atm.h"
#include "../les/if_les.h"
#include "c4000_atmzr.h"


/*******************************************************************
			Definitions
 *******************************************************************/



/*******************************************************************
			Variables
 *******************************************************************/

regs_block_db_t atmzr_sunilite_regs[] = {
    { "mr",		0x0000 },/* Master Rst and Ident/Load Meters Register */
    { "mcfgr",		0x0001 },/* Master Configuration Register */
    { "misr",		0x0002 },/* Master Interrupt Status Register */
    { "mcmr",		0x0004 },/* Master Clock Monitor Register */
    { "mctlr",		0x0005 },/* Master Control Register */
    { "cscsr",		0x0006 },/* Clock Synthesis Ctrl and Status Register */
    { "crcsr",		0x0007 },/* Clock Recovery Ctrl and Status Register */
    { "rsop_cier",	0x0010 },/* RSOP Control/Interrupt Enable Register */
    { "rsop_sisr",	0x0011 },/* RSOP Status/Interrupt Status Register */
    { "rsop_bip80r",	0x0012 },/* RSOP Section BIP-8 LSB Register */
    { "rsop_bip81r",	0x0013 },/* RSOP Section BIP-8 MSB Register */
    { "tsop_ctlr",	0x0014 },/* TSOP Control Register */
    { "tsop_diagr",	0x0015 },/* TSOP Diagnostic Register */
    { "rlop_csr",	0x0018 },/* RLOP Control/Status Register */
    { "rlop_ieisr",	0x0019 },/* RLOP Intr Enable/Intr Status Register */
    { "rlop_bip8_240r",	0x001A },/* RLOP Line BIP-8/24 LSB Register */
    { "rlop_bip8_241r",	0x001B },/* RLOP Line BIP-8/24 Register */
    { "rlop_bip8_242r",	0x001C },/* RLOP Line BIP-8/24 MSB Register */
    { "rlop_febe0r",	0x001D },/* RLOP Line FEBE LSB Register */
    { "rlop_febe1r",	0x001E },/* RLOP Line FEBE Register */
    { "rlop_febe2r",	0x001F },/* RLOP Line FEBE MSB Register */
    { "tlop_ctlr",	0x0020 },/* TLOP Control Register */
    { "tlop_diagr",	0x0021 },/* TLOP Diagnostic Register */
    { "rpop_scr",	0x0030 },/* RPOP Status/Control Register */
    { "rpop_isr",	0x0031 },/* RPOP Interrupt Status Register */
    { "rpop_ier",	0x0032 },/* RPOP Interrupt Enable Register */
    { "rpop_pslr",	0x0037 },/* RPOP Path Signal Label Register */
    { "rpop_pbip80r",	0x0038 },/* RPOP Path BIP-8 LSB Register */
    { "rpop_pbip81r",	0x0039 },/* RPOP Path BIP-8 MSB Register */
    { "rpop_pfebe0r",	0x003A },/* RPOP Path FEBE LSB Register */
    { "rpop_pfebe1r",	0x003B },/* RPOP Path FEBE MSB Register */
    { "tpop_cdr",	0x0040 },/* TPOP Control/Diagnostic Register */
    { "tpop_pcr",	0x0041 },/* TPOP Pointer Control Register */
    { "tpop_ap0r",	0x0045 },/* TPOP Arbitrary Pointer LSB Register */
    { "tpop_ap1r",	0x0046 },/* TPOP Arbitrary Pointer MSB Register */
    { "tpop_pslr",	0x0048 },/* TPOP Path Signal Label Register */
    { "tpop_psr",	0x0049 },/* TPOP Path Status Register */
    { "racp_csr",	0x0050 },/* RACP Control/Status Register */
    { "racp_iesr",	0x0051 },/* RACP Interrupt Enable/Status Register */
    { "racp_mhpr",	0x0052 },/* RACP Match Header Pattern Register */
    { "racp_mhmr",	0x0053 },/* RACP Match Header Mask Register */
    { "racp_checr",	0x0054 },/* RACP Correctable HCS Error Count Register */
    { "racp_uhecr",	0x0055 },/* RACP Uncorrectable HCS Error Cnt Register */
    { "racp_rcc0r",	0x0056 },/* RACP Receive Cell Counter LSB Register */
    { "racp_rcc1r",	0x0057 },/* RACP Receive Cell Counter Register */
    { "racp_rcc2r",	0x0058 },/* RACP Receive Cell Counter MSB Register */
    { "racp_cfgr",	0x0059 },/* RACP Configuration Register */
    { "tacp_csr",	0x0060 },/* TACP Control/Status Register */
    { "tacp_iuchpr",	0x0061 },/* TACP Idle/Unassign Cell Hdr Pattern Reg */
    { "tacp_iucpopr",	0x0062 },/* TACP Idle/Unas Cell Payload Octet Pat Reg */
    { "tacp_fctlr",	0x0063 },/* TACP FIFO Control Register */
    { "tacp_tcc0r",	0x0064 },/* TACP Transmit Cell Counter LSB Register */
    { "tacp_tcc1r",	0x0065 },/* TACP Transmit Cell Counter Register */
    { "tacp_tcc2r",	0x0066 },/* TACP Transmit Cell Counter MSB Register */
    { "tacp_cfgr",	0x0067 },/* TACP Configuration Register */
    { NULL,		0x0000 }
};

 
/*******************************************************************
			Functions
 *******************************************************************/


/*
 * a t m z r _ s u n i l i t e _ i n i t  ( )
 *
 * SUNI-LITE PLIM control structures initialization.
 */
void
atmzr_sunilite_init (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    atmzr_instance_t            *ds;            /* ATMizer dabase pointer */

    /*
     * Initialize local variables and
     * register the PLIM functions
     */
    ds = (atmzr_instance_t *)INSTANCE;
    ds->plim_linerate = LITE_OC3_LINERATE;
    ds->plim_reset = atmzr_sunilite_reset;
    ds->plim_show_regs = atmzr_sunilite_show_regs;
    ds->plim_inthand = atmzr_sunilite_inthand;

    /*
     * Initialize global idb variables
     */
    set_default_interface_bandwidth(idb->firstsw, LITE_OC3_LINERATE);
}


/*
 * a t m z r _ s u n i l i t e _ s h o w _ r e g s  ( )
 *
 * SUNI-LITE PLIM show PLIM registers.
 */
void
atmzr_sunilite_show_regs (hwidbtype *idb)	/* Interface HW IDB ptr */
{
    atmzr_instance_t		*ds;            /* ATMizer dabase pointer */
    suni_lite_t			*rp;            /* SUNI-LITE registers ptr */
    vshort			*rbp;		/* Registers base pointer */
    regs_block_db_t		*dbp;		/* Registers database pointer */
    ushort			indx;		/* Auxiliar index */

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    rp = &ds->nim_regs->plim_sunilite;
    rbp = (vshort *)&((atmzr_instance_t *)INSTANCE)->nim_regs->plim;
    dbp = &atmzr_sunilite_regs[0];

    /*
     * Start writing to the configuration register
     * to force the counters to be reloaded so that
     * they will have meaningful values in them by
     * the time we read and display them. Wait some
     * time before actually dumping the register
     * values.
     */
    rp->mr = 0;
    WASTETIME(1);

    /*
     * Print all registers (4 per line)
     */
    while (dbp->reg_name) {
	indx = 4;
	printf ("\n  ");
	for (indx=0; indx<4 && dbp->reg_name; indx++) {
	    printf ("%s 0x%02X, ", dbp->reg_name,
				   (uchar)(*(rbp + dbp->reg_offset)));
	    dbp++;
	}
    }
}


/*
 * a t m z r _ s u n i l i t e _ r e s e t  ( )
 *
 * SUNI-LITE PLIM chip reset and initialization.
 */
void
atmzr_sunilite_reset (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    atmzr_instance_t            *ds;            /* ATMizer dabase pointer */
    suni_lite_t			*rp;		/* SUNI-LITE registers ptr */
    vshort			mctlr;

    /*
     * Initialize local variables
     */
    ds = (atmzr_instance_t *)INSTANCE;
    rp = &ds->nim_regs->plim_sunilite;
    mctlr = 0;

    /*
     * Reset PLIM
     */
    rp->mr = LITE_MR_RESET;
    rp->mr = 0;

    /*
     * Initialize chip registers
     */

    /* Set master control register */
    if (idb->atm_db->txclock == ATM_CLOCK_LINE) {
	/* Set Tx Clock to Line */
	mctlr |= LITE_MCTL_LOOPT;
    }
    /* Select loopback mode (if any) */
    if (idb->loopback) {
        switch (idb->ext_loop_type) {
        case ATM_LOOP_LINE:
	    mctlr |= LITE_MCTL_LLE;
            idb->inloopback = TRUE;
            break;
        case ATM_LOOP_DIAG:
	    mctlr |= LITE_MCTL_DLE;
            idb->inloopback = TRUE;
            break;
        default:
            idb->inloopback = FALSE;
	}
    } else {
        idb->inloopback = FALSE;
    }
    rp->mctlr = mctlr;

    /* Enable HCS coset polynomial */
    rp->racp_csr = LITE_RAOP_CS_HCSADD;

    /* Tailor configuration for STM-1 if this is the case */
    if (idb->atm_db->sonetmode == ATM_SONET_STM1) {
	rp->tacp_iuchpr = LITE_TACP_IUCHP_CLP;
	rp->tpop_ap1r = LITE_TPOP_AP1_S_2;
    }
    /* Enable the generation of RDI interrupt */
    rp->rlop_ieisr |= LITE_RLOP_IEIS_RDIE;

    /* Enable line state change interrupts */
    rp->rsop_cier = LITE_RSOP_CIE_LOSE;
}


/*
 * a t m z r _ s u n i l i t e _ i n t h a n d  ( )
 *
 * SUNI-LITE PLIM interrupt handler.
 * The PLIM interrupt is one of the HI-IRQ interrupts
 * coming from the NIM.
 */
void
atmzr_sunilite_inthand (hwidbtype *idb)		/* Interface HW IDB ptr */
{
    atmzr_instance_t            *ds;            /* ATMizer dabase pointer */
    suni_lite_t			*rp;		/* SUNI-LITE registers ptr */
    enum IDB_STATES		newstate;	/* New interface state */
    ushort                      dummy;

    /*
     * Initialize local variables and
     * register the PLIM Interrupt Handler
     */
    ds = (atmzr_instance_t *)INSTANCE;
    rp = &ds->nim_regs->plim_sunilite;
    /*
     * Get new state and update state variable
     */
    if(rp->misr & LITE_MIS_RLOPI) {
      /* Check for the RDI state */
      dummy=rp->rlop_ieisr;
      newstate = (!(rp->rlop_csr & LITE_RLOP_CS_RDIV) && ds->nim_operational)
                 ? IDBS_UP : IDBS_DOWN;
    } else {
      newstate = (!(rp->rsop_sisr & LITE_RSOP_SIS_LOSV) && ds->nim_operational) 
                 ? IDBS_UP : IDBS_DOWN;
    }
    if ((idb->state != newstate) && (idb->state != IDBS_ADMINDOWN)) {
      net_cstate(idb, newstate);
      idb->counters.transitions++;
    }
}

