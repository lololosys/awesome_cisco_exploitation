/* $Id: if_c4000_bt8360.c,v 3.4.44.4 1996/08/28 12:57:44 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_bt8360.c,v $
 *------------------------------------------------------------------
 * if_c4000_bt8360.h - C4XXX T1 framer modules. 
 *
 * Nov 27 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_bt8360.c,v $
 * Revision 3.4.44.4  1996/08/28  12:57:44  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.44.3  1996/06/19  21:53:07  wmay
 * CSCdi57946:  agent returns incorrect value for dsx1LineType mib
 * attribute - use mib setting from configuration.
 * Branch: California_branch
 *
 * Revision 3.4.44.2  1996/06/16  21:15:13  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.4.44.1  1996/03/18  20:41:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.1  1996/01/24  22:23:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/06  03:22:27  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  17:38:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/08  05:44:25  jliu
 * CSCdi39941:  loopback LED on CT1 nim always on under normal operation
 *
 * Revision 2.5  1995/06/23  18:48:17  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.4  1995/06/21  08:55:17  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/19  06:47:12  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/17  00:54:44  jliu
 * CSCdi35899:  Controller didnt come back up after shut/no shut
 *
 * Revision 2.1  1995/06/07  21:33:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "if_les.h"
#include "../if/if_controller.h"
#include "les_controller.h"
#include "../if/dsx1.h"
#include "if_c4000_mcn.h"
#include "if_c4000_m32.h"
#include "if_c4000_dsx1.h"
#include "if_c4000_dsx1_framer.h"
#include "if_c4000_bt8360.h"
#include "../if/if_msg_controller.c"	/* Not a typo, see logger.h */

/*
 * =====================================================================
 * bt8360_rd_framer_reg -
 *
 * Description:
 * Read data from bt8360 framer register
 *
 * Parameters:
 * none
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static short bt8360_rd_framer_reg (volatile short *framer, int index)
{
    short temp;

    temp = framer[index];
    usecdelay(1);
    return(temp);
}

/*
 * =====================================================================
 * bt8360_wr_framer_reg -
 *
 * Description:
 * Write data to bt8360 framer register
 *
 * Parameters:
 * none
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void bt8360_wr_framer_reg (volatile short *framer, int index,
				  short value)
{
    short temp;

    framer[index] = value;
    usecdelay(1);
    temp = bt8360_rd_framer_reg(framer, index);
}

/*
 * =====================================================================
 * bt8360_rd_framer_ram -
 *
 * Description:
 * Read data from bt8360 framer ram
 *
 * Parameters:
 * none
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static short bt8360_rd_framer_ram (volatile short *framer,
				   volatile short *ram, int index)
{
    short tempread;
    int jndx;

    tempread = ram[index];
    usecdelay(1);

    jndx = 0;
    do {
        tempread = (bt8360_rd_framer_reg(framer, T1_MISCSTAT_REG))
            & T1_MISCSTAT_RAM;
        jndx++;
    } while ((tempread != 0) && (jndx < 100000));
    
    tempread = ram[index];
    usecdelay(1);
    return(tempread);
}

/*
 * =====================================================================
 * bt8360_wr_framer_ram -
 *
 * Description:
 * Write data to bt8360 framer ram
 *
 * Parameters:
 * none
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void bt8360_wr_framer_ram (volatile short *framer, volatile short *ram,
				  int index, short value)
{
    short tempread;
    int jndx;

    ram[index] = value;
    usecdelay(1);

    jndx = 0;
    do {
        tempread = (bt8360_rd_framer_reg(framer, T1_MISCSTAT_REG))
            & T1_MISCSTAT_RAM;
        jndx++;
    } while ((tempread != 0) && (jndx < 100000));
}

/*
 * =====================================================================
 * bt8360_framing -
 *
 * Description:
 * Sets the frame type for T1 controller
 *
 * Parameters:
 * slot # and frametype.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void bt8360_framing (uint slot, short frametype)
{
    short           old, delay;
    dsx1_instance_t *bt8360;
    volatile short  *bt8360_regs;
    leveltype SR;       /* interrupt level */
    
    bt8360 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    bt8360_regs = bt8360->framer;
    
    SR = raise_interrupt_level(HIRQ_INTLEVEL);
    if ( frametype == DSX1_FRM_ESF ) {
        /*
         * Enable ESF mode
         */
        old = bt8360_rd_framer_reg(bt8360_regs,T1_CFG_REG);
        old |= T1_CFG_ESF;
        bt8360_wr_framer_reg(bt8360_regs,T1_CFG_REG, old);
	
        for (delay = 0; delay <T1_FRAMING_TIMER; delay++); 
        old = bt8360_rd_framer_reg(bt8360_regs,T1_FRAMCTRL_REG);
        old |= T1_FRAMCTRL_CRCCHK;
        bt8360_wr_framer_reg(bt8360_regs,T1_FRAMCTRL_REG, old);
	
        for (delay = 0; delay <T1_FRAMING_TIMER; delay++); 
        old = bt8360_rd_framer_reg(bt8360_regs,T1_ALARM_REG);
        old &= ~T1_ALARM_SFYELLOW;
        bt8360_wr_framer_reg(bt8360_regs,T1_ALARM_REG, old);
    } else {
        /*
         * Enable SF mode
         */
        old = bt8360_rd_framer_reg(bt8360_regs,T1_CFG_REG);
        old &= ~T1_CFG_ESF;
        bt8360_wr_framer_reg(bt8360_regs, T1_CFG_REG, old);
	
        for (delay = 0; delay <T1_FRAMING_TIMER; delay++); 
        old = bt8360_rd_framer_reg(bt8360_regs, T1_FRAMCTRL_REG);
        old &= ~T1_FRAMCTRL_CRCCHK;
        bt8360_wr_framer_reg(bt8360_regs, T1_FRAMCTRL_REG, old);
	
        /*
         * This will put a timer on the received yellow alarms
         */
        for (delay = 0; delay <T1_FRAMING_TIMER; delay++); 
        old = bt8360_rd_framer_reg(bt8360_regs,T1_ALARM_REG);
        old |= T1_ALARM_SFYELLOW;
        bt8360_wr_framer_reg(bt8360_regs,T1_ALARM_REG, old);
    }
    
    /*
     * This will force re-framing
     */
    for (delay = 0; delay <T1_FRAMING_TIMER; delay++); 
    old = bt8360_rd_framer_reg(bt8360_regs,T1_FRAMCTRL_REG);
    old |= T1_FRAMCTRL_FORCE;
    bt8360_wr_framer_reg(bt8360_regs,T1_FRAMCTRL_REG, old);
    reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * bt8360_linecode -
 *
 * Description:
 * Sets the line code for T1 controller
 *
 * Parameters:
 * slot # and codetype.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void bt8360_linecode (uint slot, short codetype)
{
    short           old;
    dsx1_instance_t *bt8360;
    leveltype SR;       /* interrupt level */
    
    bt8360 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    
    SR = raise_interrupt_level(HIRQ_INTLEVEL);
    old = bt8360_rd_framer_reg(bt8360->framer, T1_CFG_REG);
    old &= ~(T1_CFG_ZBTSI | T1_CFG_B8ZS_TX | T1_CFG_B8ZS_RX );

    if ( codetype == DSX1_LC_B8ZS ) {
        old |= (T1_CFG_B8ZS_TX | T1_CFG_B8ZS_RX);
    }
    
    bt8360_wr_framer_reg(bt8360->framer, T1_CFG_REG, old);
    
    reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * bt8360_clock_source -
 *
 * Description:
 * Sets the clock source for T1 controller
 *
 * Parameters:
 * Clocktype and pointer of DSX1 instance data structure.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void bt8360_clock_source (dsx1_instance_t *bt8360, short clocktype)
{
    leveltype SR;       /* interrupt level */
    
    SR = raise_interrupt_level(HIRQ_INTLEVEL);

    if ( clocktype == DSX1_CLK_INTERNAL ) {
         *bt8360->ext_ctl |= T1_CLOCK_SOURCE;
    } else {
        *bt8360->ext_ctl &= ~T1_CLOCK_SOURCE;
    }
    
    reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * bt8360_clocksource -
 *
 * Description:
 * Sets the clock source for T1 controller
 *
 * Parameters:
 * slot # and clocktype.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void bt8360_clocksource (uint slot, short clocktype)
{
    dsx1_instance_t *bt8360;
    
    bt8360 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    
    bt8360->clock_type = clocktype;
    bt8360_clock_source(bt8360, clocktype);
}

/*
 * =====================================================================
 * bt8360_txAIS -
 *
 * Description:
 * Send AIS out to indicate T1 controller has been shutdown
 *
 * Parameters:
 * slot #.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */

void bt8360_txAIS (uint slot, boolean set)
{
    short read;
    dsx1_instance_t *bt8360;
    leveltype SR;       /* interrupt level */
    
    bt8360 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    
    SR = raise_interrupt_level(HIRQ_INTLEVEL);
    if ( set ) {
        if ( bt8360->txing_ais == FALSE ) {
            /*
             * Sending AIS
             */
            bt8360->txing_ais = TRUE;
            read = (bt8360_rd_framer_reg(bt8360->framer,T1_ALARM_REG))
                | T1_ALARM_TXAIS;
            bt8360_wr_framer_reg(bt8360->framer,T1_ALARM_REG, read);
        }
    } else {
        if ( bt8360->txing_ais ) {
            /*
             * Stop sending AIS
             */
            bt8360->txing_ais = FALSE;
            read = bt8360_rd_framer_reg(bt8360->framer,T1_ALARM_REG);
            read = read & ~T1_ALARM_TXAIS;
            usecdelay(1);
            bt8360_wr_framer_reg(bt8360->framer,T1_ALARM_REG, read);
        }
    }
    reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * bt8360_local_up -
 *
 * Description:
 * Enable local loopback for T1 controller
 *
 * Parameters:
 * slot #.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void bt8360_local_up (uint slot)
{
    dsx1_instance_t *bt8360;
    leveltype SR;       /* interrupt level */
    short tempread, old, delay;
    dsx1info *info;
    
    bt8360 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    info = bt8360->dsx1_ds;
    
    /*
     * For loopback, want to do a line loop at the transceiver, and a
     * equipment loopback at the framer chip.
     */
    bt8360_clock_source(bt8360, DSX1_CLK_INTERNAL);

    SR = raise_interrupt_level(HIRQ_INTLEVEL);
    
    tempread = bt8360_rd_framer_reg(bt8360->framer, T1_FRAME_REG);
    bt8360_wr_framer_reg(bt8360->framer, T1_FRAME_REG, 
                         (tempread | T1_FRAME_NETLOOP));
    
    for (delay = 0; delay <T1_FRAMING_TIMER; delay++);
    old = bt8360_rd_framer_reg(bt8360->framer,T1_FRAMCTRL_REG);
    old |= T1_FRAMCTRL_FORCE;
    bt8360_wr_framer_reg(bt8360->framer,T1_FRAMCTRL_REG, old);
    
    reset_interrupt_level(SR);
    
    /*
     * Turn on remote loopback LED since it does loopback at framer
     * and transceiver chip
     */
    *bt8360->ext_ctl |= T1_CTRL_REMLOOP;
    *bt8360->ext_led &= ~DSX1_LED_RLOOP;
}

/*
 * =====================================================================
 * bt8360_loopup_code -
 *
 * Description:
 * Send and enable detecting loop up code
 *
 * Parameters:
 * slot #.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void bt8360_loopup_code (uint slot)
{
    short read;
    dsx1_instance_t *bt8360;
    
    bt8360 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    
    /*
     * enable the interrupt for detecting loop up codes
     */
    read = bt8360_rd_framer_reg(bt8360->framer,T1_LOOPCODE_ACT_REG);
    read |= T1_LOOPCODE_ACT_INT;
    bt8360_wr_framer_reg(bt8360->framer, T1_LOOPCODE_ACT_REG, read);
    
    /*
     * Now send the loopback code
     */
    bt8360_wr_framer_reg(bt8360->framer,T1_LOOPCG_REG, 0x80);
    read = bt8360_rd_framer_reg(bt8360->framer,T1_LOOPGD_REG);
    read &= ~T1_LOOPGD_CODELEN;
    read |= (T1_LOOPGD_CODELEN_5 | T1_LOOPGD_CODEGEN);
    bt8360_wr_framer_reg(bt8360->framer, T1_LOOPGD_REG, read);
    
    /*
     * Indicate that we are searching for a loop-up code
     */
    bt8360->rem_loop_state = REM_LOOP_ON_SRCH_CODE;
    bt8360->rem_loop_count = 0;
    *bt8360->ext_led &= ~DSX1_LED_RLOOP;
}

/*
 * =====================================================================
 * bt8360_loopdown_code -
 *
 * Description:
 * Send and enable detecting loop down code
 *
 * Parameters:
 * slot #.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void bt8360_loopdown_code (uint slot)
{
    short read;
    dsx1_instance_t *bt8360;
    
    bt8360 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    
    /*
     * enable the interrupt for detecting loop down codes
     */
    read = bt8360_rd_framer_reg(bt8360->framer, T1_LOOPCODE_DEACT_REG);
    read |= T1_LOOPCODE_DEACT_INT;
    bt8360_wr_framer_reg(bt8360->framer, T1_LOOPCODE_DEACT_REG, read);
    
    /*
     * Now send the loop down code
     */
    bt8360_wr_framer_reg(bt8360->framer, T1_LOOPCG_REG, 0x90);
    read = bt8360_rd_framer_reg(bt8360->framer, T1_LOOPGD_REG);
    read &= ~T1_LOOPGD_CODELEN;
    read |= (T1_LOOPGD_CODELEN_6 | T1_LOOPGD_CODEGEN);
    bt8360_wr_framer_reg(bt8360->framer, T1_LOOPGD_REG, read);
    
    /*
     * Indicate that we are searching for a loop-down code
     */
    bt8360->rem_loop_state = REM_LOOP_OFF_SRCH_CODE;
    bt8360->rem_loop_count = 0;
    *bt8360->ext_led |= DSX1_LED_RLOOP;
}

/*
 * =====================================================================
 * bt8360_stoploop_code -
 *
 * Description:
 * Stop sending and disable detecting loop code
 *
 * Parameters:
 * slot #.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void bt8360_stoploop_code (uint slot)
{
    short read;
    dsx1_instance_t *bt8360;
    
    bt8360 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    
    /* Stop sending the code */
    read = bt8360_rd_framer_reg(bt8360->framer, T1_LOOPGD_REG);
    read &= ~T1_LOOPGD_CODEGEN;
    bt8360_wr_framer_reg(bt8360->framer, T1_LOOPGD_REG, read);
    
    /* Stop interrupting on both the loop-up and loop-down detections */
    read = bt8360_rd_framer_reg(bt8360->framer,T1_LOOPCODE_DEACT_REG);
    read &= ~T1_LOOPCODE_DEACT_INT;
    bt8360_wr_framer_reg(bt8360->framer, T1_LOOPCODE_DEACT_REG, read);
    
    read = bt8360_rd_framer_reg(bt8360->framer,T1_LOOPCODE_ACT_REG);
    read &= ~T1_LOOPCODE_ACT_INT;
    bt8360_wr_framer_reg(bt8360->framer,T1_LOOPCODE_ACT_REG,read);
}

/*
 * =====================================================================
 * bt8360_no_loop -
 *
 * Description:
 * Disable local loopback for T1 controller
 *
 * Parameters:
 * slot #.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void bt8360_no_loop (uint slot)
{
    short tempread;
    dsx1_instance_t *bt8360;
    leveltype SR;       /* interrupt level */
    dsx1info *info;
    
    bt8360 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    info = bt8360->dsx1_ds;
    
    if (bt8360->rem_loop_state != REM_LOOP_NOT_SRCH) {
        bt8360_loopdown_code(slot);
        return;
    }
    bt8360_clock_source(bt8360, bt8360->clock_type);
    
    SR = raise_interrupt_level(HIRQ_INTLEVEL);
    
    tempread = bt8360_rd_framer_reg(bt8360->framer, T1_FRAME_REG);
    bt8360_wr_framer_reg(bt8360->framer,T1_FRAME_REG,
                         (tempread & ~T1_FRAME_NETLOOP));
    
    reset_interrupt_level(SR);
    
    /*Turn off local and remote loopback LED*/
    *bt8360->ext_ctl &= ~(T1_CTRL_REMLOOP);
    *bt8360->ext_led |= DSX1_LED_RLOOP;
}

/*
 * =====================================================================
 * bt8360_tx_remotealarm -
 *
 * Description:
 * Transmit remote alarm indication.
 *
 * Parameters:
 * Pointer to Bt8360 framer registers.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void bt8360_tx_remotealarm (volatile short *framer, boolean set)
{
    short old;
    leveltype SR;       /* interrupt level */
    
    SR = raise_interrupt_level(HIRQ_INTLEVEL);
    
    old = bt8360_rd_framer_reg(framer,T1_ALARM_REG);
    
    if ( set == FALSE ) {
        old &= ~T1_ALARM_TXYELLOW;
    } else {
        old |= T1_ALARM_TXYELLOW;
    }
    bt8360_wr_framer_reg(framer,T1_ALARM_REG,old);
    reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * bt8360_interrupt -
 *
 * Description:
 * T1 framer interrupt routine
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void bt8360_interrupt (dsx1_instance_t *bt8360)
{
    volatile short *bt8360_regs;
    short          src, cause, temp;
    dsx1info       *info;
    uint           slot;


    
    bt8360_regs = bt8360->framer;
    info = bt8360->dsx1_ds;
    slot = info->cdb->slot;
    cause = 0;
    
    src = bt8360_rd_framer_reg(bt8360_regs, T1_INTSRC_REG);
    if ( src ) {
	if (src & T1_INTSRC_ALS) {
	    /* alarm & loopback & signaling */
	    cause = bt8360_rd_framer_reg(bt8360_regs, T1_ALSSTAT_REG);
	    if (cause & T1_ALSSTAT_LOS) {
		bt8360->intrpt_los = TRUE;
		/*
		 * Disable this interrupt.  There's a bug in the
		 * Bt8360 chip that will give a constant interrupt.
		 * Try to avoid that here
		 */
		temp = bt8360_rd_framer_reg(bt8360_regs, T1_ALRMINT_REG);
		temp &= ~T1_ALRMINT_LOS;
		bt8360_wr_framer_reg(bt8360_regs, T1_ALRMINT_REG, temp);
	    }
	    if (cause & T1_ALSSTAT_OOF) {
		bt8360->intrpt_oof = TRUE;
		/*
		 * Disable this interrupt.  There's a bug in the
		 * Bt8360 chip that will give a constant interrupt.
		 * Try to avoid that here
		 */
		temp = bt8360_rd_framer_reg(bt8360_regs, T1_ALRMINT_REG);
		temp &= ~T1_ALRMINT_OOF;
		bt8360_wr_framer_reg(bt8360_regs, T1_ALRMINT_REG, temp);
	    }
	    /*
	     * Do we have a loop-up ?  If we were searching for
	     * it, stop sending the loop code, and indicate
	     * that we have sucessfully looped the remote unit
	     */
	    if (( cause & T1_ALSSTAT_LOOPACT )
		&& ( bt8360->rem_loop_state == REM_LOOP_ON_SRCH_CODE)) {
                errmsg(&msgsym(REMLOOP, CONTROLLER),
                        info->cdb->cdb_namestring,
                        info->loop == DSX1_LOOP_REMOTE ? "up" : "down",
                        "passed");

		bt8360_stoploop_code(slot);
		bt8360->rem_loop_state = REM_LOOP_IN_LOOP;
		bt8360->in_rem_loop = TRUE;
	    }
	}
	/*
	 * Do we have a loop-down ?  If we were searching for
	 * it, indicate that we have received it.  We will
	 * then wait until we stop receiving it.
	 */
	if (( cause & T1_ALSSTAT_LOOPDEACT )
	    && (bt8360->rem_loop_state == REM_LOOP_OFF_SRCH_CODE )) {
	    bt8360->rem_loop_state = REM_LOOP_OFF_WAIT_NO_CODE;
	}
    }
    if(src & T1_INTSRC_RXDATA) {
	cause = bt8360_rd_framer_reg(bt8360_regs, T1_RXSTATUS_REG);
    }
    
    if(src & T1_INTSRC_TXDATA) {
	/* we need to write this reg to clear the interrupt */
	bt8360_wr_framer_reg(bt8360_regs, T1_TXCTRL_REG, T1_TXCTRL_IDLE);
	/* send idle */
    }
}

/*
 * =====================================================================
 * bt8360_frame_process -
 *
 * Description:
 * Run T1 frame process algorithm for every 250ms
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void bt8360_frame_process (dsx1_instance_t *bt8360)
{
    short            almstat, temp;
    uchar            HaveReceiveError;
    dsx1MIBDataTable *ee;
    dsx1info         *info;
    int              slot;
    leveltype        SR;       /* interrupt level */
    cdbtype          *cdb;
    
    info = bt8360->dsx1_ds;
    ee = &info->MIB;
    cdb = info->cdb;
    slot = info->cdb->slot;
    
    almstat = bt8360_rd_framer_reg(bt8360->framer, T1_ALSSTAT_REG);
    HaveReceiveError = FALSE;
    
    if ((almstat & T1_ALSSTAT_LOS) || bt8360->intrpt_los) {
        bt8360->intrpt_los = FALSE;
        /* Detected LOS - if we didn't have it before, set it */
        if ( bt8360->has_los == FALSE ) {
            bt8360->has_los = TRUE;
            framer_set_MIB_ls(&ee->C.dsx1LineStatus, DSX1_RX_LOS);
        }
        HaveReceiveError = TRUE;
    } else {
        /* no LOS - check framing */
        if (bt8360->has_los != FALSE) {
            bt8360->has_los = FALSE;
            framer_clr_MIB_ls(&ee->C.dsx1LineStatus, DSX1_RX_LOS);

            SR = raise_interrupt_level(HIRQ_INTLEVEL);
            /* Re-enable interrupt we turned off in interrupt routine */
            temp = bt8360_rd_framer_reg(bt8360->framer, T1_ALRMINT_REG);
            temp |= T1_ALRMINT_LOS;
            bt8360_wr_framer_reg(bt8360->framer, T1_ALRMINT_REG, temp);
            reset_interrupt_level(SR);
        }
	
        if ((almstat & T1_ALSSTAT_OOF) || bt8360->intrpt_oof) {
            /* Bad framing - process down below */
            bt8360->intrpt_oof = FALSE;
            HaveReceiveError = TRUE;
        } else {
            /* Framing is okay */
            if (bt8360->has_frame == FALSE) {
		
                SR = raise_interrupt_level(HIRQ_INTLEVEL);
                /* Re-enable interrupt turned off in interrupt routine */
                temp = bt8360_rd_framer_reg(bt8360->framer, T1_ALRMINT_REG);
                bt8360_wr_framer_reg(bt8360->framer, T1_ALRMINT_REG,
                                (temp | T1_ALRMINT_OOF));
                reset_interrupt_level(SR);
                /*
                 * We are Out of frame - but we have good frame
                 * Increment timer until it expires
                 */
                FORWARD_COUNTER(bt8360->count_until_frame, FRAMER_TIME);
                if (COUNTER_EXPIRED(bt8360->count_until_frame,
				    bt8360->secs_for_frame)) {
                    /*
                     * We now have frame  - indicate this
                     * Alert the CPU that we can now transmit - only if we
                     * are still in the available state.
                     */
                    bt8360->has_frame = TRUE;
                    bt8360->frame_acquired = TRUE;
                    framer_clr_MIB_ls(&ee->C.dsx1LineStatus,
                                       DSX1_RX_LOF );
                    ee->C.dsx1LineType = bt8360->frame_type;
		    
                    /* Turn off Yellow alarm if we are available */
                    if (!( bt8360->unavailable_state )) {
                        framer_send_remalarm( bt8360, FALSE );
                        framer_led_off( bt8360, DSX1_LED_LOCALARM );
                    }
                    framer_check_available(bt8360);
                }
            } else {
                /*
                 * This is the ugly part of the remote loopback algorithm
                 * If we had stopped sending the pattern, we will start
                 * sending it again.  We retain the time count.
                 */
                if (bt8360->rem_loop_state == REM_LOOP_ON_SRCH_BUT_STOPPED) {
                    temp = bt8360->rem_loop_count;
                    bt8360_loopup_code(slot);
                    bt8360->rem_loop_count = temp;
                }
                /*
                 * We are in frame and received a good frame.
                 * We will check to see if we have any frame loss events
                 * in the last few seconds.
                 * We keep track of the number of periods with frame
                 * loss (that's the CountsTowardLoss counter).  If we have any
                 * periods with frame loss, we will decrement that counter
                 * by 1 for every 5 periods we have without a frame loss.
                 * This allows us to handle intermittent frame loss.
                 */
                if (HAVE_COUNTER(bt8360->count_toward_loss)) {
                    FORWARD_COUNTER(bt8360->count_until_loss_clear,
				    FRAMER_TIME);
                    if (COUNTER_EXPIRED(bt8360->count_until_loss_clear,
					5 * FRAMER_TIME)) {
                        BACKWARD_COUNTER(bt8360->count_toward_loss,
                                         FRAMER_TIME);
                        CLEAR_COUNTER(bt8360->count_until_loss_clear);
                    }
                }
                /*
                 * We have frame, so see if we are getting in remote
                 * alarm.  If we are receiving it, it will take 2 seconds
                 * for us to stop receiving it before we declare we are up.
                 */
                if (almstat & T1_ALSSTAT_YELLOW) {
                    /* Detected yellow alarm */
                    if (bt8360->rxing_rem_alarm == FALSE) {
                        if (bt8360->available) {
                            bt8360->available = FALSE;
                            if (cdb->cdb_state != IDBS_ADMINDOWN) {
                                controller_state_change(cdb, IDBS_DOWN);
                            }
                        }
                        bt8360->rxing_rem_alarm = TRUE;
                        framer_set_MIB_ls(&ee->C.dsx1LineStatus,
                                         DSX1_RX_REM_ALARM );
                        framer_led_on(bt8360, DSX1_LED_REMALARM);
                    }
                    CLEAR_COUNTER(bt8360->count_no_remalarm);
                } else {
                    /* No yellow alarm */
                    if ( bt8360->rxing_rem_alarm ) {
                        FORWARD_COUNTER(bt8360->count_no_remalarm,
					FRAMER_TIME);
                        if (COUNTER_EXPIRED(bt8360->count_no_remalarm,
					    T1_REM_ALARM_TIME)) {
                            bt8360->rxing_rem_alarm = FALSE;
                            framer_clr_MIB_ls(&ee->C.dsx1LineStatus,
                                               DSX1_RX_REM_ALARM );
                            framer_led_off(bt8360, DSX1_LED_REMALARM );
                            framer_check_available(bt8360);
                        }
                    }
                } /* end else remote alarm */
            } /* end else we have frame */
	    
        } /* end else we have frame from framer */
    }
    
    if (HaveReceiveError) {
        bt8360->frame_loss_occurred = TRUE;
        CLEAR_COUNTER(bt8360->count_until_frame);
        CLEAR_COUNTER(bt8360->count_until_loss_clear);
        /*
         * If we are looking for the code to come back, and we get a frame
         * loss, we will stop sending the code, and change state
         */
        if (bt8360->rem_loop_state == REM_LOOP_ON_SRCH_CODE) {
            bt8360_stoploop_code(slot);
            bt8360->rem_loop_state = REM_LOOP_ON_SRCH_BUT_STOPPED;
        }
        /*
         * Receiver error (LOF or LOS), but still available
         * Must wait 3 seconds
         */
        if (bt8360->has_frame) {
            FORWARD_COUNTER(bt8360->count_toward_loss, FRAMER_TIME);
            if (COUNTER_EXPIRED(bt8360->count_toward_loss,
				T1_FRAME_LOSS_TIME)) {
                framer_frame_loss(bt8360, info);
                CLEAR_COUNTER(bt8360->count_toward_loss);
            }
        }
    }
    
    if ( almstat & T1_ALSSTAT_AIS ) {
        framer_set_MIB_ls(&ee->C.dsx1LineStatus, DSX1_RX_AIS);
    } else {
        framer_clr_MIB_ls(&ee->C.dsx1LineStatus, DSX1_RX_AIS);
    }
}          

/*
 * =====================================================================
 * bt8360_sec_update -
 *
 * Description:
 * T1 receive errors second update
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void bt8360_sec_update (dsx1_instance_t *bt8360)
{
    volatile short   *framer;
    dsx1info         *info;
    ushort           tmpBPV, tmpFBE, tmpCRC, read, tmpCOFA;
    boolean          EsOccurred, BESOccurred, SESOccurred;
    dsx1TableType    *errptr;
    dsx1MIBDataTable *ee;
    int              slot;
    
    info = bt8360->dsx1_ds;
    ee = &info->MIB;
    framer = bt8360->framer;
    errptr = &ee->CurrentData;
    slot = info->cdb->slot;
    
    EsOccurred = FALSE;
    BESOccurred = FALSE;
    SESOccurred = FALSE;
    
    /* Read all the counters */
    tmpBPV = bt8360_rd_framer_reg(framer,T1_BPV_LSB);
    tmpBPV += (bt8360_rd_framer_reg(framer,T1_BPV_MSB) << 8);
    
    tmpFBE = bt8360_rd_framer_reg(framer,T1_FBE_CTR);
    
    tmpCRC = bt8360_rd_framer_reg(framer,T1_CRC_CTR);
    
    read = bt8360_rd_framer_reg(framer,T1_COFA_CTR);
    
    if (( read & T1_COFA_CRC ) != 0 )
        tmpCRC += 0x100;
    
    tmpCOFA = read & T1_COFA_MASK;
    
    if ( tmpBPV > 0 ) {
        ADD_TO_GAUGE( errptr->dsx1LCVs, tmpBPV );
        errptr->dsx1LESs++;
        EsOccurred = TRUE;
        if ( tmpBPV >= 1544 )
            SESOccurred = TRUE;
    }
    
    if ( bt8360->has_frame ) {
        if ( tmpFBE > 0 ) {
            EsOccurred = TRUE;
            if ( info->framing == DSX1_FRM_SF ) {
                ADD_TO_GAUGE( errptr->dsx1PCVs, tmpFBE );
                if ( tmpFBE > 1 )
                    SESOccurred = TRUE;
            }
        }
	
        if ( info->framing == DSX1_FRM_ESF ) {
            tmpCRC += tmpFBE;
            if ( tmpCRC > 0 ) {
                EsOccurred = TRUE;
                if ( tmpCRC >= 320 )
                    SESOccurred = TRUE;
                else if ( tmpCRC > 1 )
                    BESOccurred = TRUE;
                ADD_TO_GAUGE( errptr->dsx1PCVs, tmpCRC );
            }
        }
    }
    
    if (( bt8360->frame_acquired ) && (bt8360->frame_loss_occurred)) {
        errptr->dsx1SEFSs++;
        SESOccurred = TRUE;
        bt8360->frame_loss_occurred = FALSE;
    }
    
    /*
     * Check for severe errored frame, or controlled slips
     */
    read = bt8360_rd_framer_reg(bt8360->framer,T1_ERRSTAT_REG);
    if ( read & T1_ERRSTAT_SEVERR )
        SESOccurred = TRUE;
    
    if ( read & T1_ERRSTAT_SLIPEV ) {
        /* Have a controlled slip event - add one, and set ES */
        errptr->dsx1CSSs++;
        EsOccurred = TRUE;
    }
    
    /*
     * Now check for remote loopback for the T1
     */
    if (( bt8360->rem_loop_state != REM_LOOP_NOT_SRCH )
        && ( bt8360->rem_loop_state != REM_LOOP_IN_LOOP )) {
        switch ( bt8360->rem_loop_state ) {
	  case REM_LOOP_NOT_SRCH:
	  case REM_LOOP_IN_LOOP:
            break;
	  case REM_LOOP_OFF_WAIT_NO_CODE:
            /*
             * check if we have stopped getting the loop code yet.
             * If so, indicate this and stop sending it.
             */
            read = bt8360_rd_framer_reg(framer,T1_ALSSTAT_REG);
            if (( read & T1_ALSSTAT_LOOPDEACT ) == 0 ) {
                errmsg(&msgsym(REMLOOP, CONTROLLER),
                        info->cdb->cdb_namestring,
                        info->loop == DSX1_LOOP_REMOTE ? "up" : "down",
                        "passed");
                bt8360_stoploop_code(slot);
                bt8360->rem_loop_state = REM_LOOP_NOT_SRCH;
                bt8360->in_rem_loop = FALSE;
		
                break;
            }
            /* else fall through */
	  case REM_LOOP_ON_SRCH_CODE:
	  case REM_LOOP_OFF_SRCH_CODE:
	  case REM_LOOP_ON_SRCH_BUT_STOPPED:

            /*
             * Here we do a 10 second count to see if we
             * have received the loop code response yet.  If we
             * haven't, indicate this and stop sending it.
             */
            FORWARD_COUNTER(bt8360->rem_loop_count, 1);
            if (COUNTER_EXPIRED(bt8360->rem_loop_count,10)) {
                /* no response in 10 seconds.  Forget it */
                bt8360_stoploop_code(slot);
                bt8360->rem_loop_state = bt8360->in_rem_loop == FALSE ?
                    REM_LOOP_NOT_SRCH : REM_LOOP_IN_LOOP;
                errmsg(&msgsym(REMLOOP, CONTROLLER),
                        info->cdb->cdb_namestring,
                        info->loop == DSX1_LOOP_REMOTE ? "up" : "down",
                        "failed");
            }
            break;
        } /* end switch */
    } /* end if looking for remote loop */
    
    /*
     * Now we check for unavailable / available states
     * Note: may want to break out later - should be the same for E1
     */
    framer_uas(bt8360, info, tmpBPV, tmpCRC, EsOccurred, BESOccurred,
	       SESOccurred, errptr);
    
}

/*
 * =====================================================================
 * bt8360_init_framer -
 *
 * Description:
 * Initialize the bt8360 framer chip and transceiver
 *
 * Parameters:
 * slot:                The slot #.
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
static void bt8360_init_framer (dsx1_instance_t *bt8360, int slot)
{
    volatile short *bt8360_regs, *ram, tempread;
    int indx;
    
    bt8360_regs = bt8360->framer;
    ram = bt8360->ram;

    /* Since after power up, these three LEDs are in "unknown" condition. so*/
    /* need to turn off loopback LED and leave remote and local alarm LEDs on*/
    *bt8360->ext_led |= DSX1_LED_RLOOP;
    *bt8360->ext_led &= ~(DSX1_LED_REMALARM | DSX1_LED_LOCALARM);
    *bt8360->ext_ctl &= ~(T1_CTRL_LLOOP | T1_CTRL_RLOOP);
    
    bt8360_wr_framer_reg(bt8360_regs,T1_CFG_REG, 0);
    bt8360_wr_framer_reg(bt8360_regs,T1_CTRL_REG,
                    (T1_CTRL_BMC_193 | T1_CTRL_INFREQ_24 | T1_CTRL_OUTFREQ_1));
    bt8360_wr_framer_reg(bt8360_regs,T1_FRAME_REG, 0);
    bt8360_wr_framer_reg(bt8360_regs,T1_SIGCTRL_REG, 0);
    bt8360_wr_framer_reg(bt8360_regs,T1_FRAMCTRL_REG, T1_FRAMCTRL_2OF5);
    bt8360_wr_framer_reg(bt8360_regs,T1_SLIP_REG, 0);
    bt8360_wr_framer_reg(bt8360_regs,T1_ALARM_REG, T1_ALARM_ZERO);
    /*
     * No interrupts allowed at this moment until we are in signal/frame
     */
    bt8360_wr_framer_reg(bt8360_regs, T1_ALRMINT_REG, 0);
    
    
    /*
     * Don't interrupt on any overflow errors -
     */
    bt8360_wr_framer_reg(bt8360_regs,T1_ERRINT_REG, 0);
    
    /* the next two deal with the Facilities Data Link */
    /* send idle */
    bt8360_wr_framer_reg(bt8360_regs,T1_TXCTRL_REG, T1_TXCTRL_IDLE);
    bt8360_wr_framer_reg(bt8360_regs,T1_RXCTRL_REG, 0);
    
    
    /* loop codes for remote CSU */
    bt8360_wr_framer_reg(bt8360_regs, T1_LOOPGD_REG,
                    (T1_LOOPGD_DEACTDET_6 | T1_LOOPGD_ACTDET_5));
    bt8360_wr_framer_reg(bt8360_regs, T1_LOOPCG_REG, 0);

    /* 5 bit code 10000 */
    bt8360_wr_framer_reg(bt8360_regs, T1_LOOPCODE_ACT_REG, 0x80);
    bt8360_wr_framer_reg(bt8360_regs, T1_LOOPCODE_DEACT_REG, 0x90);
    
    /*
     * the below is to make sure that the transmit frame synch aligns
     * okay.
     */
    for ( indx = 1; indx <= 23; indx++ ) {
        bt8360_wr_framer_ram(bt8360_regs, ram, (T1_RATE_CONTROL + indx),
			T1_RATE_TS_ACTIVE);
        tempread = bt8360_rd_framer_ram(bt8360_regs, ram,
                                         (T1_RATE_CONTROL + indx));
    }
    bt8360_wr_framer_ram(bt8360_regs, ram,(T1_RATE_CONTROL + 24),
		    (T1_RATE_TS_ACTIVE | T1_RATE_TX_TS_INDICATE));
    tempread = bt8360_rd_framer_ram(bt8360_regs, ram,(T1_RATE_CONTROL + 24));
    
    /*
     * Also init the TX PCM control registers - for signalling
     */
    for ( indx = 0; indx <= 23; indx++ ) {
        bt8360_wr_framer_ram(bt8360_regs, ram,T1_TX_PCM_CONTROL + indx, 0x0);
    }
    
    /*
     * Dummy read of all counters - should get out initial shock
     */
    tempread = bt8360_rd_framer_reg(bt8360_regs,T1_BPV_LSB);
    tempread = bt8360_rd_framer_reg(bt8360_regs,T1_BPV_MSB);
    tempread = bt8360_rd_framer_reg(bt8360_regs,T1_FBE_CTR);
    tempread = bt8360_rd_framer_reg(bt8360_regs,T1_CRC_CTR);
    tempread = bt8360_rd_framer_reg(bt8360_regs,T1_COFA_CTR);
    
}

/*
 * =====================================================================
 * bt8360_init -
 *
 * Description:
 * Initialize bt8360 data structure and framer chip
 *
 * Parameters:
 * slot:                The slot #.
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
static void bt8360_init (dsx1_instance_t *bt8360, int slot)
{
    dsx1MIBDataTable *mib;
    volatile short *bt8360_regs;
    
    mib = &bt8360->dsx1_ds->MIB;
    bt8360_regs = bt8360->framer;
    
    bt8360_init_framer(bt8360, slot);        /* init the hardware */
    
    bt8360->framer_remote_alarm = bt8360_tx_remotealarm;
    bt8360->has_ais = FALSE;
    bt8360->has_frame = FALSE;
    bt8360->secs_for_frame = 5 * FRAMER_TIMES_PER_SEC;
    bt8360->one_sec_timer = 0;
    CLEAR_COUNTER(bt8360->count_until_frame);
    CLEAR_COUNTER(bt8360->count_until_loss_clear);
    bt8360->frame_loss_occurred = FALSE;
    bt8360->frame_acquired = FALSE;
    bt8360->unavailable_state = TRUE;
    bt8360->rxing_rem_alarm = FALSE;   /* will be set in Process below */
    bt8360->txing_rem_alarm = FALSE;
    bt8360->intrpt_los = FALSE;
    bt8360->intrpt_oof = FALSE;
    bt8360->clock_type = DSX1_CLK_LINE;

    /*
     * The below are for a 1 x 10-6 error rate for 1 minute
     */
    bt8360->bpvneeded_in_min = (1544000 * 60) / 1000000;
    bt8360->crcneeded_in_min = (1544000 * 60) / 1000000;
    
    bt8360->has_los = TRUE;
    mib->C.dsx1LineStatus = DSX1_RX_LOS;

    bt8360->rxing_rem_alarm = FALSE;
    framer_led_off( bt8360, DSX1_LED_REMALARM );
    framer_led_on( bt8360, DSX1_LED_LOCALARM );
    framer_set_MIB_ls( &mib->C.dsx1LineStatus, DSX1_RX_LOF );
    framer_send_remalarm( bt8360, TRUE );
    /*
     * Enable framer interrupt
     */
    *bt8360->ext_int_ctl |= DSX1_INT_ENABLE;

    bt8360_rd_framer_reg(bt8360_regs, T1_ALARM_REG);
}

/*
 * =====================================================================
 * bt8360_startup -
 *
 * Description:
 * This function is called by analyze_bt8360 as a one time initialization
 * per slot.
 *
 * Parameters:
 * slot:                The slot #.
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
static void bt8360_startup (uint slot, uint type, uint subcont)
{
    dsx1_instance_t *bt8360;
    mcn_instance_t *ds_mcn;
    
    ds_mcn = MCN_INSTANCE(slot);        /* get the mcn instance */
    
    /*
     * Allocate  memory for the channelized T1 instance.
     */
    bt8360 = malloc_fast(sizeof(dsx1_instance_t));
    if (!bt8360) {
        crashdump(0);
    }

    /*
     * set the base of the Bt8360 framer registers and ram area for this slot
     */
    bt8360->framer = (volatile short *) ((uint) GET_SLOT_BASE(slot) +
					 (uint) T1_FRAMER_OFFSET);
    bt8360->ram = (volatile short *) ((uint) GET_SLOT_BASE(slot) +
				      (uint) T1_RAM_OFFSET);
    
    /*
     * set the base of some external registers for this slot
     */
    bt8360->ext_ctl = (volatile ushort *)((uint) GET_SLOT_BASE(slot) +
					  (uint) T1_CONTROL_OFFSET);
    bt8360->ext_status = (volatile ushort *)((uint) GET_SLOT_BASE(slot) +
					     (uint) T1_STATUS_OFFSET);
    bt8360->ext_int_ctl = (volatile ushort *)((uint) GET_SLOT_BASE(slot) +
					      (uint) T1_INT_CONTROL_OFFSET);
    bt8360->ext_led = (volatile ushort *)((uint) GET_SLOT_BASE(slot) +
					  (uint) T1_LED_OFFSET);
    
    /*
     * Store the bt8360 instance pointer in the MCN instance.
     * Henceforth, access the instance using DSX1_INSTANCE(slot, subcont).
     */
    ds_mcn->mcn_subcont[subcont].info.dsx1 = bt8360;
    
    /*
     * register level 4 interrupt for this slot, to handle T1 framer
     * interrupts
     */
    nim_register_hi_irq_handler((ushort) slot, framer_interrupt,\
				(ushort) IDBTYPE_T1, (ushort) slot);
    
    bt8360->appliquetype = type;
}

/*
 * =====================================================================
 * bt8360_analyze_mcn_port_adaptor -
 *
 * Description:
 * Initialize the T1  port adaptor in a given slot
 *
 * Parameters:
 * slot.
 *
 * Returns:
 * none.
 * 
 * =====================================================================
 */
void bt8360_analyze_mcn_port_adaptor (int slot, int subcont, int total_ctrlr_unit)
{
    uint 	      type;
    dsx1info          *bt8360_info;
    dsx1_instance_t   *bt8360;

    type = mcn_get_adapter_type(slot);
    
    if (type == MCN_PA_T1) {
	
	/*
	 * Initialize the T1 instance.
	 */
	bt8360_startup( slot, type, subcont );
	
	/*
	 * Init Dsx1. The dsx1_init_context is expected to return DSX1 info
         * structure pointer
	 */
	bt8360_info = dsx1_init_context(MCN_PA_T1, PIFT_T1, APPL_T1, slot, subcont,
                                        total_ctrlr_unit);
        bt8360 = DSX1_INSTANCE(slot, subcont);
        bt8360->dsx1_ds = bt8360_info;
	
	/*
	 * Add the T1 framer Dsx1 registries and init Bt8360 framer chip
	 */
	framer_reg_add();
	bt8360_init(bt8360, slot);
    }
}

