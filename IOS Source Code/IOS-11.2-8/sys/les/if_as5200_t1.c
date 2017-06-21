/* $Id: if_as5200_t1.c,v 3.1.68.7 1996/09/09 20:56:47 jchlin Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_t1.c,v $
 *------------------------------------------------------------------
 * if_as5200_t1.c - AS5200 T1 framer module.
 *
 * Nov 1995, Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_t1.c,v $
 * Revision 3.1.68.7  1996/09/09  20:56:47  jchlin
 * CSCdi66525:  ct1/rbs cannot busy back the CO trunk when theres no free
 * modem
 * Branch: California_branch
 * o       The bugfix will busy dsx0s back to CO thru RBS when there's
 *         no free modem to accept incoming calls.
 * o       The modem pool is changed to have multiple pools for different
 *         types of modems.
 * o       Add hidden debug commands.
 *
 * Revision 3.1.68.6  1996/09/07  22:37:02  jliu
 * CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
 * Branch: California_branch
 *     o The fix included display warning message to console port and used
 *       SNMP notification to send the message to network management
 *       station
 *
 * Revision 3.1.68.5  1996/08/28  12:57:14  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.68.4  1996/07/15  20:31:24  towu
 * CSCdi62430:  CT1 RBS is not enabled
 * Branch: California_branch
 *         o Enabled CT1 RBS in the neat processor.
 *         o Added handler routine for Transmit RBS status back to
 *         MPU.(CSCdi61208)
 *
 * Revision 3.1.68.3  1996/07/11  22:36:59  jliu
 * CSCdi62357:  need to support AT&T and ANSI facility data link standard
 * Branch: California_branch
 *           o Added new fdl command for t1 controller in order to support
 *             CSU function
 *
 * Revision 3.1.68.2  1996/06/19  21:53:04  wmay
 * CSCdi57946:  agent returns incorrect value for dsx1LineType mib
 * attribute - use mib setting from configuration.
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/06/16  21:14:56  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/09  23:55:56  jliu
 * Create placeholder for new files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "logger.h"
#include "../if/if_msg_controller.c"	/* Not a typo, see logger.h */
#include "if_les.h"
#include "../if/if_controller.h"
#include "les_controller.h"
#include "../if/dsx1.h"
#include "if_as5200_dsx1_ipc.h"
#include "if_as5200_mcn.h"
#include "if_c4000_dsx1.h"
#include "if_as5200_dsx1_framer.h"
#include "if_as5200_dsx1.h"
#include "if_as5200_t1.h"
#include "../os/timer_generic.h"
#include "packet.h"
#include "../if/dsx1_registry.h"
#include "if_as5200_tdm.h"
#include "../dev/flash_defines.h"

extern message msgsym(FIRMWARE, CONTROLLER);
queuetype t1_sec_updateQ;

/*
 * =====================================================================
 * t1_rd_neat_mem -
 *
 * Description:
 * Read word data from t1 NEAT memory space
 *
 * Parameters:
 * none
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
short t1_rd_neat_mem (uchar slot, uint mail_addr)
{
	int            temp;
        short          data;
        leveltype      SR;       /* interrupt level */
	ushort 	       old_page;

        data = 0x0;

    	temp = mail_addr;
	temp = (temp & PAGE_ADDR_MASK) >> 12;
        mail_addr &= ~PAGE_ADDR_MASK;
	mail_addr += NEAT_SPACE;

        SR = raise_interrupt_level(ALL_DISABLE);

	old_page = dsx1_set_neat_page_reg(slot, temp);

        data = *(short *)((uint)GET_SLOT_BASE(slot) + mail_addr);

	/* Restore the old page reg value */
	dsx1_set_neat_page_reg(slot, old_page);
        reset_interrupt_level(SR);

   	return (data);
}

/*
 * =====================================================================
 * t1_wr_neat_mem -
 *
 * Description:
 * Write word data to t1 NEAT memory space
 *
 * Parameters:
 * none
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void t1_wr_neat_mem (uchar slot, uint mail_addr, short data)
{
        int            temp;
        leveltype      SR;       /* interrupt level */
	ushort 	       old_page;

        temp = mail_addr;
        temp = (temp & PAGE_ADDR_MASK) >> 12;
        mail_addr &= ~PAGE_ADDR_MASK;
        mail_addr += NEAT_SPACE;

        SR = raise_interrupt_level(ALL_DISABLE);
	old_page = dsx1_set_neat_page_reg(slot, temp);

        *(short *)((uint)GET_SLOT_BASE(slot) + mail_addr) = data;

	/* Restore the old page reg value */
	dsx1_set_neat_page_reg(slot, old_page);
        reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * t1_neat_int -
 *
 * Description:
 * Initiate NEAT interrupt at level 2.
 *
 * Parameters: slot
 *
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void t1_neat_int ( uchar slot)
{
	ushort tempread;
        leveltype      SR;       /* interrupt level */

        /*
         * Need to send new request to NEAT processor by interrupt
	 */
		 
        SR = raise_interrupt_level(ALL_DISABLE);
        tempread = t1_rd_neat_mem(slot, DSX1_NEAT_INT);
        tempread |= NEAT_INT_GEN;
        t1_wr_neat_mem(slot, DSX1_NEAT_INT, tempread);
        reset_interrupt_level(SR);
}
/*
 * =====================================================================
 * t1_mail_to_neat -
 *
 * Description:
 * Send phone digits collection request to NEAT processor
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void t1_mail_to_neat (ushort event, uchar mid, uchar slot, uchar subcont,
                             uchar *phonenum)
{
        dtmf_instance_t *dtmf;
        ushort          cmd;

        dtmf = DTMF_INSTANCE(slot, subcont);

        /*
         * store the pointer for copying phone numbers later from
         * the corresponding DTMF circuitry
         */
        dtmf->phone_digits = phonenum;
        cmd = mid | NEW_REQ_CMD;

        t1_wr_neat_mem(slot, DSX1_TO_DTMF(subcont) + NEW_REQ_OFFSET, cmd);

        /*
         * Need to send new request to NEAT processor by interrupt
         */
        t1_wr_neat_mem(slot, DSX1_NEAT_INT, NEAT_INT_GEN);
}

/*
 * =====================================================================
 * t1_mail_to_np -
 *
 * Description:
 * Common routine to send t1 configuration commands to NEAT processor
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void t1_mail_to_np (uchar slot, ushort command, boolean set, int mail_addr)
{
  	short    current_cmd;
	leveltype      SR;       /* interrupt level */

	SR = raise_interrupt_level(ALL_DISABLE);  
	current_cmd = t1_rd_neat_mem(slot, mail_addr);

        if (set) {
           if ((!command) || (command & MPU_MSG_FDL)) {
              current_cmd &= ~MPU_MSG_FDL;
           }
	      current_cmd |= command;
        } else 
              current_cmd &= ~command;

        t1_wr_neat_mem(slot, mail_addr, current_cmd);
	reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * t1_framing -
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
void t1_framing (uint slot, uint subcont, short frametype)
{
    	dsx1_instance_t *t1;
    	uchar           set;
    
    	t1 = DSX1_INSTANCE(slot, subcont);

    	if (frametype != DSX1_FRM_ESF && frametype != DSX1_FRM_SF)
       	     return;

    	if (t1->frame_type != frametype)
       	   t1->frame_type = frametype;

    	set = (frametype == DSX1_FRM_ESF) ? TRUE : FALSE;

    	t1_mail_to_np(slot, MPU_MSG_FRAME, set, DSX1_MAIL_TO_C(subcont));

}

/*
 * =====================================================================
 * t1_linecode -
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
void t1_linecode (uint slot, uint subcont, short codetype)
{
    	uchar           set;
    	dsx1_instance_t *t1;
    
    	t1 = DSX1_INSTANCE(slot, subcont);

    	if (codetype != DSX1_LC_AMI && codetype != DSX1_LC_B8ZS)
       	     return;

    	set = (codetype == DSX1_LC_B8ZS) ? TRUE : FALSE;

    	t1_mail_to_np(slot, MPU_MSG_LINECODE, set, DSX1_MAIL_TO_C(subcont));
}

/*
 * =====================================================================
 * t1_fdl -
 *
 * Description:
 * Sets the fdl standard for t1 data link
 *
 * Parameters:
 * slot # and fdltype.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void t1_fdl (uint slot, uint subcont, short fdltype)
{
        dsx1_instance_t *t1;

        t1 = DSX1_INSTANCE(slot, subcont);

        if (fdltype == DSX1_FDL_ATT) {
            t1_mail_to_np(slot, MPU_MSG_FDL_ATT, TRUE, DSX1_MAIL_TO_C(subcont));
        } else if (fdltype == DSX1_FDL_ANSI) {
            t1_mail_to_np(slot, MPU_MSG_FDL_ANSI, TRUE, DSX1_MAIL_TO_C(subcont));
        } else if (fdltype == DSX1_FDL_NONE) {
            t1_mail_to_np(slot, MPU_MSG_FDL_NONE, TRUE, DSX1_MAIL_TO_C(subcont));
        }
}

/*
 * =====================================================================
 * t1_set_equalizer -
 *
 * Description:
 * Routine  to access LIU control registers on T1 card
 *
 * Parameters:
 * dsx1_instance_t
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void t1_set_equalizer (uchar slot, uint subcont,  ushort control)
{
    	ushort      tempread;
        leveltype      SR;       /* interrupt level */        

    	tempread = 0x0;

        SR = raise_interrupt_level(ALL_DISABLE);
    	if (subcont == 0) {
       	  tempread = t1_rd_neat_mem(slot, LIU0_CONTROL);
       	  tempread &= ~(EC4_1);
       	  tempread |= control;
       	  t1_wr_neat_mem(slot, LIU0_CONTROL, tempread);
    	} else {
       	  tempread = t1_rd_neat_mem(slot, LIU1_CONTROL);
       	  tempread &= ~(EC4_1);
       	  tempread |= control;
       	  t1_wr_neat_mem(slot, LIU1_CONTROL, tempread);
    	}
        reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * t1_cablelength -
 *
 * Description:
 * Sets the transmit attenuation and receive equalizer control for LBO unit
 *
 * Parameters:
 * dsx1info and lengthtype.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void t1_cablelength (dsx1info *info, uint subcont, short lengthtype)
{
    	uint slot;

    	slot = info->cdb->slot;
  
    	if (info->length == DSX1_SHORT) {
       	 t1_set_equalizer(slot, subcont, SHORT_06DB);
    	} else {
       	 if (info->gain == DSX1_GAIN36) {
       	     switch (info->pulse) {
       	     case DSX1_0DB:
       	         t1_set_equalizer(slot, subcont, L36_0DB);
       	         break;
       	     case DSX1_75DB:
       	         t1_set_equalizer(slot, subcont, L36_75DB);
       	         break;
       	     case DSX1_15DB:
       	         t1_set_equalizer(slot, subcont, L36_15DB);
       	         break;
       	     case DSX1_225DB:
       	         t1_set_equalizer(slot, subcont, L36_225DB);
       	         break;
       	     }
          } else {
              switch (info->pulse) {
              case DSX1_0DB:
                  t1_set_equalizer(slot, subcont, L26_0DB);
                  break;
              case DSX1_75DB:
                  t1_set_equalizer(slot, subcont, L26_75DB);
                  break;
              case DSX1_15DB:
                  t1_set_equalizer(slot, subcont, L26_15DB);
                  break;
              case DSX1_225DB:
                  t1_set_equalizer(slot, subcont, L26_225DB);
                  break;
              }
          }
      }
}

/*
 * =====================================================================
 * t1_clk_sel0 -
 *
 * Description:
 * Select T1 0 received recovery clock as TDM bus clock source
 *
 * Parameters:
 * clocksource type
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void t1_clk_sel0 (dsx1info *info, mcn_instance_t *ds_mcn, 
                                                    uchar slot, uchar conflict)
{
         short          temp;

         if ( info->clocksource == DSX1_PRIMARY ) {
              temp = ds_mcn->mcn_regs->pri_control2;
              temp |= (DSX1_PRI_CLK_SEL | DSX1_PRI_CLK_EN);
              /*
               * If conflict is TRUE, that means the previous clock selection
               * for t1 0 could be secondary, so need to disable it. Even it
               * is not secondary, this disable won't hurt the clock selection
               */
              if (conflict)
                  temp &= ~DSX1_SEC_CLK_EN;
              ds_mcn->mcn_regs->pri_control2 = temp;
         } else if ( info->clocksource == DSX1_SECONDARY ) {
              info->clocking = DSX1_CLK_THRU;
              temp = ds_mcn->mcn_regs->pri_control2;
              temp |= (DSX1_SEC_CLK_SEL | DSX1_SEC_CLK_EN);
              if (conflict) 
                  temp &= ~DSX1_PRI_CLK_EN;
              ds_mcn->mcn_regs->pri_control2 = temp;
         }
}

/*
 * =====================================================================
 * t1_clk_sel1 -
 *
 * Description:
 * Select T1 1 received recovery clock as TDM bus clock source
 *
 * Parameters:
 * clocksource type
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void t1_clk_sel1 (dsx1info *info, mcn_instance_t *ds_mcn, 
                                                    uchar slot, uchar conflict)
{
         short       temp;

         if ( info->clocksource == DSX1_PRIMARY ) {
              temp = ds_mcn->mcn_regs->pri_control2;
              temp |= DSX1_PRI_CLK_EN;
              temp &= ~DSX1_PRI_CLK_SEL;
              if (conflict) 
                  temp &= ~DSX1_SEC_CLK_EN;
              ds_mcn->mcn_regs->pri_control2 = temp;
         } else if ( info->clocksource == DSX1_SECONDARY ) {
              info->clocking = DSX1_CLK_THRU;
              temp = ds_mcn->mcn_regs->pri_control2;
              temp |= DSX1_SEC_CLK_EN;
              temp &= ~DSX1_SEC_CLK_SEL;
              if (conflict) 
                  temp &= ~DSX1_PRI_CLK_EN;
              ds_mcn->mcn_regs->pri_control2 = temp;
         }
}

/*
 * =====================================================================
 * t1_clocksource -
 *
 * Description:
 * Selects the clock source for T1 framer trasnmit clock and TDM bus
 * Note: 1.Although Brasil shares the same parser command code with 4000,
 *         there is some difference on clock circuitry. The brasil box
 *         has TDM bus on the backplane which use 2.048Mhz clock as
 *         master clock for M32 & framer transmit buffer clock. So the
 *         default value for framer transmit buffer clock always come
 *         from internal in normal condition. The way it works for internal
 *         setup is to configure ADRSPC_BRASIL_TDM_PLL to 0x35 which selects
 *         automatic mode. The clock could be from free running, primary or
 *         secondary line recovered clock.
 *       2.If line primary or secondary commands exist, the TDM bus clock
 *         come from this source. if none of those command exist then free
 *         running clock is selected.
 *       3.If both T1 line use external loopback plug, the free running clock
 *         can be used by typing "clock source free" which is a hidden command
 *         and only be used for this condition.
 *       4.The only time to select line clock for transmit is when external
 *         network loopback request is between detected and released.
 *
 * Parameters:
 * dsx1info and clocktype.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void t1_clocksource (dsx1info *info, uint subcont, short clocktype)
{
        uchar           conflict;
    	uint            slot;
    	dsx1_instance_t *t1_0, *t1_1;
    	mcn_instance_t  *ds_mcn;

        conflict = FALSE;
    	slot = info->cdb->slot;
    	ds_mcn = MCN_INSTANCE(slot);
    	t1_0 = DSX1_INSTANCE(slot, 0);
    	t1_1 = DSX1_INSTANCE(slot, 1);
    
        if (subcont == 0)
    	    t1_0->clock_type = clocktype;
        else
            t1_1->clock_type = clocktype;

    	if (clocktype == DSX1_CLK_INTERNAL) {
       	   t1_mail_to_np(slot, MPU_MSG_CLK_SOURCE, TRUE, DSX1_MAIL_TO_C(subcont));
    	} else if (clocktype == DSX1_CLK_FREE) {
       	   *(ushort *)ADRSPC_BRASIL_TDM_PLL = 0x25;
       	   t1_mail_to_np(slot, MPU_MSG_CLK_SOURCE, TRUE, DSX1_MAIL_TO_C(subcont));
    	} else {
       	   *(ushort *)ADRSPC_BRASIL_TDM_PLL = 0x35;
       	   t1_mail_to_np(slot, MPU_MSG_CLK_SOURCE, FALSE, DSX1_MAIL_TO_C(subcont));
       	   if (subcont == 0) {
               /*
                * if no clock source was entered for t1 0 then check if t1 1
                * has no clock source either or be configured as secondary. If
                * any of above is true, then configure t1 0 as primary clock
                * source
                */
               if (info->clocksource == 0) {
                   if ((t1_1->dsx1_ds->clocksource == 0) ||
                            (t1_1->dsx1_ds->clocksource == DSX1_SECONDARY)) {
                        info->clocksource = DSX1_PRIMARY;
                   }
               } else 
                     /*
                      * if both t1 have the same clock source, print out warnning
                      * messages on the screen then use this new selection
                      * note: when this happened, set conflict TRUE, so later we
                      *       can disable the other clock source mux ENABLE on T1
                      *       card
                      */
                     if (info->clocksource == t1_1->dsx1_ds->clocksource) {
                         conflict = TRUE;
                         printf("\nSelect the same clock source for both T1 is not allowed!!!\n");
                         printf("\nThis new selection will be chosen over the other one!!!\n");
                     }
               t1_clk_sel0(info, ds_mcn, slot, conflict);
       	   } else {
               if (info->clocksource == 0) {
                  if ((t1_0->dsx1_ds->clocksource == 0) ||
                           (t1_0->dsx1_ds->clocksource == DSX1_PRIMARY)) {
                       info->clocksource = DSX1_SECONDARY;
                  }
               } else
                     if (info->clocksource == t1_0->dsx1_ds->clocksource) {
                         conflict = TRUE;
                         printf("\nSelect the same clock source for both T1 is not allowed!!!\n");
                         printf("\nThis new selection will be chosen over the other one!!!\n");
                     }
               t1_clk_sel1(info, ds_mcn, slot, conflict);
           }
       }
}

/*
 * =====================================================================
 * t1_loopback_clk_switch -
 *
 * Description:
 * Statemachine for controlling the T1 line clock source selection
 * during automatic loopback (caused by excessive errors).
 *   We switch line clock source, from one T1 to the other T1, in either of
 *   the following two cases:
 *   1) If the primary line clock source is automatically put into 
 *      local loopback.
 *   2) If the secondary line clock source is automatically put into 
 *      local loopback AND the user didn't configure a primary source.
 *
 *   And we restore the original clock selection again in either of the 
 *   three cases:
 *   a) If the T1 that satisfied case 1) above is automatically taken out 
 *      of local loopback.
 *   b) If the T1 that satisfied case 2) above is automatically taken out 
 *      of local loopback.
 *   c) If the user decides to make a change to the clock source configuration.
 *
 * Event types:
 * DSX1_INTO_LOOP_EVENT    : a T1 has automatically been put into loopback.
 * DSX1_AUTORECOVERED_EVENT: a T1 has automatically recovered from loopback.
 * DSX1_USER_CONFIG_EVENT  : user has decided to change the clock source
 *                           configuration.
 *
 * Parameters:
 * dsx1info ptr, sub-controller number, event type.
 * 
 * Returns:
 * none.
 *
 * =====================================================================
 */
void t1_loopback_clk_switch(dsx1info *info, uint subcont, uchar event)
{
    	uint            slot;
    	dsx1_instance_t *t1;
    	mcn_instance_t  *ds_mcn;
        short		temp;
	static short	saved_reg;

    	slot = info->cdb->slot;
    	ds_mcn = MCN_INSTANCE(slot);
    	t1 = DSX1_INSTANCE(slot, subcont);
	temp = ds_mcn->mcn_regs->pri_control2;

	switch (event) {
	case DSX1_INTO_LOOP_EVENT:
	  if (ds_mcn->loopback_state == MCN_IDLE_STATE) {
	    if ((t1->unit == DSX1_PRIMARY_CLK_UNIT(temp)) &&
		(temp & DSX1_PRI_CLK_EN)) {
	      /* This loop backed T1 is primary clock source.
	       * Disable this T1's clocking, and temporary select + enable 
	       * the other T1 as secondary source.
	       */
	      saved_reg = temp;
	      temp &= ~DSX1_PRI_CLK_EN;
	      if (t1->unit == 0) {
		temp |= DSX1_SEC_CLK_EN;
		temp &= ~DSX1_SEC_CLK_SEL; 
	      } else {
		temp |= (DSX1_SEC_CLK_SEL | DSX1_SEC_CLK_EN); 
	      }
	      ds_mcn->mcn_regs->pri_control2 = temp;
	      ds_mcn->loopback_state = MCN_PRI_IN_LOOP_STATE;
	    } else if ((t1->unit == DSX1_SECONDARY_CLK_UNIT(temp)) &&
		       !(temp & DSX1_PRI_CLK_EN) && (temp & DSX1_SEC_CLK_EN)) {
	      /* This loop backed T1 is secondary clock source AND the
	       * user didn't configure a primary clock source.
	       * Disable this T1's clocking, and temporary select + enable 
	       * the other T1 as primary source.
	       */
	      saved_reg = temp;
	      temp &= ~DSX1_SEC_CLK_EN;
	      if (t1->unit == 0) {
		temp |= DSX1_PRI_CLK_EN;
		temp &= ~DSX1_PRI_CLK_SEL; 
	      } else {
		temp |= (DSX1_PRI_CLK_SEL | DSX1_PRI_CLK_EN); 
	      }
	      ds_mcn->mcn_regs->pri_control2 = temp;
	      ds_mcn->loopback_state = MCN_SEC_IN_LOOP_STATE;
	    }
	  } 
	  break;
    
	case DSX1_AUTORECOVERED_EVENT:
	  switch (ds_mcn->loopback_state) {
	  case MCN_PRI_IN_LOOP_STATE:
	    if (t1->unit == DSX1_PRIMARY_CLK_UNIT(temp)) {
	      ds_mcn->mcn_regs->pri_control2 = saved_reg;
	      ds_mcn->loopback_state = MCN_IDLE_STATE;
	    }
	    break;
	    
	  case MCN_SEC_IN_LOOP_STATE:
	    if (t1->unit == DSX1_SECONDARY_CLK_UNIT(temp)) {
	      ds_mcn->mcn_regs->pri_control2 = saved_reg;
	      ds_mcn->loopback_state = MCN_IDLE_STATE;
	    }
	    break;
	    
	  default:
	    break;
	  }
	  break;
    
	case DSX1_USER_CONFIG_EVENT: 
	  if (ds_mcn->loopback_state > MCN_IDLE_STATE) {
	    ds_mcn->mcn_regs->pri_control2 = saved_reg;
	    ds_mcn->loopback_state = MCN_IDLE_STATE;
	  }
	  break;
    
	default:
	  break;
	}
}

/*
 * =====================================================================
 * t1_local_up -
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
void t1_local_up (uint slot, uint subcont)
{
    	dsx1_instance_t *t1;
        dsx1info        *info;
    
    	t1 = DSX1_INSTANCE(slot, subcont);
        info = t1->dsx1_ds;
    	t1->loopback_type = LOCAL_LOOP;
    	t1_mail_to_np(slot, MPU_MSG_LPBK_LOC, TRUE, DSX1_MAIL_TO_C(subcont));
}

/*
 * =====================================================================
 * t1_loopup_code -
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
void t1_loopup_code (uint slot, uint subcont)
{
    	dsx1_instance_t *t1;
    
    	t1 = DSX1_INSTANCE(slot, subcont);
    	t1->loopback_type = REMOTE_LOOP;
    	t1_mail_to_np(slot, MPU_MSG_LPBK_REM, TRUE, DSX1_MAIL_TO_C(subcont));
}

/*
 * =====================================================================
 * t1_no_loop -
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
void t1_no_loop (uint slot, uint subcont)
{
    	dsx1_instance_t *t1;
    
    	t1 = DSX1_INSTANCE(slot, subcont);

    	if (t1->loopback_type == LOCAL_LOOP)
       	   t1_mail_to_np(slot, MPU_MSG_LPBK_LOC, FALSE, DSX1_MAIL_TO_C(subcont));
        else
       	   t1_mail_to_np(slot, MPU_MSG_LPBK_REM, FALSE, DSX1_MAIL_TO_C(subcont));
}

/*
 * =====================================================================
 * t1_txAIS -
 *
 * Description:
 * Send AIS out to indicate T1 controller has been shutdown
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void t1_txAIS ( uint slot, uint subcont, boolean set )
{

    	t1_mail_to_np(slot, MPU_MSG_DISABLED, set, DSX1_MAIL_TO_C(subcont));
}

/*
 * =====================================================================
 * t1_l6_interrupt -
 *
 * Description:
 * T1 level 6 interrupt service routine
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void t1_l6_interrupt (uint slot)
{
    short          cause, temp;
    int            ix, src;
    mcn_instance_t *ds_mcn;

    ds_mcn = MCN_INSTANCE(slot);    /* get the mcn instance */


    cause = ds_mcn->mcn_regs->pri_status;

    if (cause & DSX1_NEAT_INT_L6)  {
        dsx1_instance_t     *t1;

        /* clear the level 6 interrupt by writing 1 to Neat INT register */
        t1_wr_neat_mem(slot, DSX1_NEAT_INT, CLR_LEVEL6);

        for (ix = 0; ix < MAX_DSX1_PER_SLOT ; ix++) {
             t1 = DSX1_INSTANCE(slot, ix);
             /*
              * clear carrier status after read it, so make it available for NEAT
              * processor to update the carrier status if changed
              */
             temp = t1_rd_neat_mem(slot, DSX1_CARRIER_AVAIL(t1->unit));
             t1->carrier_status = temp & CARRIER_MASK;
             src = (t1_rd_neat_mem(slot, DSX1_CARRIER_AVAIL(t1->unit)+0x06)) & 0xffff;
             src |= t1_rd_neat_mem(slot, DSX1_CARRIER_AVAIL(t1->unit)+0x04) << 16;
             if (src && BUS_ERROR) {
                buginf("\n!!! Bus error occurred on NEAT processor !!!\n");
                buginf("\nThe carrier struct pointer is %x\n",
                                          DSX1_CARRIER_AVAIL(t1->unit));
                buginf("The Stack Pointer is %x\n", (src & NEAT_ERR_MASK));
                buginf("The faulted address high is %x\n",
                                   t1_rd_neat_mem(slot, src + 8));
                buginf("The faulted address low is %x\n",
                                   t1_rd_neat_mem(slot, src + 0xa));
                buginf("The current instruction program counter high is %x\n",
                                   t1_rd_neat_mem(slot, src + 0x10));
                buginf("The current instruction program counter low is %x\n",
                                   t1_rd_neat_mem(slot, src + 0x12));
             }

             if (src && ILLEGAL) {
                buginf("\n!!! Illegal Instruction occurred on NEAT processor !!!\n");
                buginf("\nThe carrier struct pointer is %x\n",
                                          DSX1_CARRIER_AVAIL(t1->unit));
                buginf("The Stack Pointer is %x\n", (src & NEAT_ERR_MASK));
             }
        }
     /*
      * When every time level 6 interrupt occurs, we need to re-enable it
      * before exit this interrupt routine. Don't ask me why, it just the
      * way mother board hardware works
      */
     temp = *((ushort *)ADRSPC_BRASIL_MB_CTRL1_REG);
     temp |=AS5200_LEVEL6_EN;
     *((ushort *)ADRSPC_BRASIL_MB_CTRL1_REG) = temp;
     }
}

/*
 * =====================================================================
 * t1_carrier_update -
 *
 * Description:
 * T1 carrier status update
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void t1_carrier_update (dsx1_instance_t *t1)
{
    dsx1info    *info;
    cdbtype     *cdb_d;

    info = t1->dsx1_ds;
    cdb_d = info->cdb;

       if (t1->carrier_status == CARRIER_UP) {
         /*
          * Before we bring up controller, make sure it isn't in shutdown
          * or unavailable state
          */
              if ((t1->unavailable_state == FALSE) &&
                  (cdb_d->cdb_state != IDBS_ADMINDOWN)) {
                    if (cdb_d->cdb_state != IDBS_UP) {
                        errmsg(&msgsym(UPDOWN, CONTROLLER),
                               cdb_d->cdb_namestring, print_cdbstate(IDBS_UP));

                        cdb_d->cdb_state = IDBS_UP;
                        dsx1_create_idle_channel(info);
                        dsx1_change_all_idbs_state(cdb_d, IDBS_UP);
                    }
              }
       } else if (t1->carrier_status == CARRIER_DOWN) {
              if (cdb_d->cdb_state == IDBS_UP) {
                  errmsg(&msgsym(UPDOWN, CONTROLLER),
                      cdb_d->cdb_namestring, print_cdbstate(IDBS_DOWN));

                  cdb_d->cdb_state = IDBS_DOWN;
                  dsx1_change_all_idbs_state(cdb_d, IDBS_DOWN);
              }
       }
}

/*
 * =====================================================================
 * t1_get_digits -
 *
 * Description:
 * Copy the complete phone digits set from NEAT's memory to system memory
 *
 * Parameters:
 * T1 and DTMF data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void t1_get_digits (dsx1_instance_t *t1, dtmf_instance_t *dtmf, 
                         int fromdtmf_addr)
{
    	ushort		tempread, ix;
        uchar           temp, total_digits;
        uint            slot;

        slot = t1->slot;
        total_digits = (uchar) t1_rd_neat_mem(slot, 
                                      fromdtmf_addr + TOTAL_NUM_OFFSET);
        ix = 0;

        while (ix < total_digits) {
            tempread = t1_rd_neat_mem(slot, fromdtmf_addr + PHONE_NUM_OFFSET +ix);
            temp = (tempread & 0xff00) >> 8;
            if (temp == 0x0) {
                *dtmf->phone_digits = 0x0;
                break;
            } else  {
                *dtmf->phone_digits++ = temp;
            }
            
            temp = tempread & 0xff;
            if (temp == 0x0) {
                *dtmf->phone_digits = 0x0;
                break;
            } else {
                *dtmf->phone_digits++ = temp;
            }
            ix += 0x2;
        }
}

/*
 * =====================================================================
 * t1_reply_to_csm -
 *
 * Description:
 * Reply to CSM request for phone digits collection for outgoing calls
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void t1_reply_to_csm (dsx1_instance_t *t1, dtmf_instance_t *dtmf, 
                      int from_dtmf_addr, int to_dtmf_addr)
{
	ushort from_dtmf, to_dtmf, temp_msg;
        uint   slot;

        slot = t1->slot;

         from_dtmf = t1_rd_neat_mem(slot, from_dtmf_addr);

         if (from_dtmf & MSG_RDY_DIGCOL) {

             to_dtmf = t1_rd_neat_mem(slot, to_dtmf_addr);
             temp_msg = to_dtmf & MODEM_ID_GET;
             dsx1_mail_from_neat(READY_COL, temp_msg, t1->slot, dtmf->unit);

         } else if (from_dtmf & MSG_1STDIG_TO) {

             to_dtmf = t1_rd_neat_mem(slot, to_dtmf_addr);
             temp_msg = to_dtmf & MODEM_ID_GET;
             dsx1_mail_from_neat(FIRST_DIGIT_TO, temp_msg, t1->slot,
                                 dtmf->unit);

         } else if (from_dtmf & MSG_1STDIG_RCV) {

             to_dtmf = t1_rd_neat_mem(slot, to_dtmf_addr);
             temp_msg = to_dtmf & MODEM_ID_GET;
             dsx1_mail_from_neat(FIRST_DIGIT_RCV, temp_msg, t1->slot,
                                 dtmf->unit);

         } else if (from_dtmf & MSG_DIG_DONE) {

             t1_get_digits(t1, dtmf, from_dtmf_addr);
             to_dtmf = t1_rd_neat_mem(slot, to_dtmf_addr);
             temp_msg = to_dtmf & MODEM_ID_GET;
             dsx1_mail_from_neat(ALL_DIGIT_RCV, temp_msg, t1->slot, dtmf->unit);
         }
}

/*
 * =====================================================================
 * t1_l4_interrupt -
 *
 * Description:
 * T1 level 4 interrupt service routine
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void t1_l4_interrupt (uint slot)
{
    ushort              src, cause, tempread;
    paktype             *pak;
    uchar               *mib_info;
    int                 mail_addr, ix;
    mcn_instance_t      *ds_mcn;

    ds_mcn = MCN_INSTANCE(slot);    /* get the mcn instance */

    cause = ds_mcn->mcn_regs->pri_status;

    mail_addr = 0x0;

    if (cause & DSX1_NEAT_INT_L4)  {
        dsx1_instance_t     *t1;
        dsx1info            *info;
        dsx1MIBDataTable    *ee;

        /* clear the level 4 interrupt by writing 1 to Neat INT register */
        t1_wr_neat_mem(slot, DSX1_NEAT_INT, CLR_LEVEL4);

        for (ix = 0; ix < MAX_DSX1_PER_SLOT ; ix++) {
             t1 = DSX1_INSTANCE(slot, ix);
             info = t1->dsx1_ds;
             ee = &info->MIB;

             /*
              * Read message byte from NEAT processor's ram to determinate 
              * which message type
              */
             mail_addr = DSX1_MAIL_FROM_C(ix);

	     if (systeminit_flag) {
                 src = t1_rd_neat_mem(slot, mail_addr);
	         /*
	          * clear carrier status after read it, so make it available for NEAT
	          * processor to update the carrier status if changed
	          */
	         if (src & MSG_TYPE_FULL) {
                     /*
                      * Receive full message from NEAT processor, so need to read all
                      * message and update those message accordingly.The code rely
                      * on this interrupt to figure out one second interval, so it
                      * can update MIB table.
                      *
                      * Clear alive count when receive full message from NEAT processor
                      * This count is used to monitor NEAT processor is still alive or
                      * not. If this count is equal to or more than 10, then there
                      * must be something wrong on NEAT processor.
                      */
                      t1->alive_count = 0;

                     /*
                      * Read board status and update them to DSX1 line status in
                      * configuration table
                      */
                      tempread = t1_rd_neat_mem(slot, mail_addr + BOARD_STATUS_OFFSET);
                      ee->C.dsx1LineStatus = tempread;

                     /*
                      * Read MIB error messages from NEAT processor and update later
                      * we allocate a smallest buffer from public pool to store the
                      * full message from NEAT processor since the message is only
                      * 9 bytes
                      */
                      if ((pak = getbuffer(MAX_UPDATE_SIZE)) == NULL)
                           return;

                      mib_info = pak->datagramstart;

                      tempread = t1_rd_neat_mem(slot, mail_addr + OTHER_STATUS_OFFSET);
                      *mib_info++ = tempread & 0xff; /* get other_status byte */
     
                      tempread = t1_rd_neat_mem(slot, mail_addr + BPV_COUNT_OFFSET);
                      *mib_info++ = tempread >> 8; /* get upper byte of BPV count */
                      *mib_info++ = tempread & 0xff; /* get lower byte of BPV count */

                      tempread = t1_rd_neat_mem(slot, mail_addr + PCV_COUNT_OFFSET);
                      *mib_info++ = tempread >> 8; /* get upper byte of PCV count */
                      *mib_info++ = tempread & 0xff; /* get lower byte of PCV count */

                      tempread = t1_rd_neat_mem(slot, mail_addr + CRC_COUNT_OFFSET);
                      *mib_info++ = tempread >> 8; /* get upper byte of CRC count */
                      *mib_info++ = tempread & 0xff; /* get lower byte of CRC count */

                      tempread = t1_rd_neat_mem(slot, mail_addr + FBE_COUNT_OFFSET);
                      *mib_info++ = tempread >> 8; /* get upper byte of FBE count */
                      *mib_info++ = tempread & 0xff; /* get lower byte of FBE count */

                     /*
                      * Store dsx1 data structure pointer to rx_descriptor. so when
                      * one second update background process is called, the code
                      * will pass the right pointer since more than one T1 interface 
                      * are put in the system
                      */
                      pak->rx_descriptor = t1;

                      if (pak_enqueue(&t1_sec_updateQ, pak) == NULL) {
                          retbuffer(pak);
                         }
                  } 

                  if (src & MSG_LOOPBACK) {
                     /*
                      * NEAT processor pass the remote loopback test result back
                      * to core CPU, so the result can be displayed to user
                      */
                     t1->loopback_act = t1_rd_neat_mem(slot,
                                                   mail_addr + LOOPBACK_OFFSET);
                     t1_wr_neat_mem(slot, mail_addr + LOOPBACK_OFFSET, 0);
                 } 

                 if (src & MSG_ERROR) {
                   /*
                    * The errors word from NEAT processor can be used to report
                    * some errors occurs on T1 card. Right now only one error
                    * report, it could report 16 different errors in the future
                    */
                   tempread = t1_rd_neat_mem(slot, mail_addr + ERRORS_OFFSET);
                   t1_wr_neat_mem(slot, mail_addr + ERRORS_OFFSET, 0);

                   if (tempread & MSG_DIG_MAX_ERR)
                      buginf("*** collect too many phone digits number ***\n");
                 } 
   
                if (src & MSG_DTMF0) {
                      t1_reply_to_csm(t1, DTMF_INSTANCE(t1->slot, 0), 
                                       DSX1_FROM_DTMF(0), DSX1_TO_DTMF(0));
                 } 

                 if (src & MSG_DTMF1) {
                      t1_reply_to_csm(t1, DTMF_INSTANCE(t1->slot, 1), 
                                       DSX1_FROM_DTMF(1), DSX1_TO_DTMF(1));
                 }
	     }

           /*
            * clear message byte after read it, so make it available for NEAT
            * processor to send next message
            */
           t1_wr_neat_mem(slot, mail_addr, 0);
       }
       /* process message queue */
       neat_msgrcv_process(slot);  

    }
}

/*
 * =====================================================================
 * t1_lp_print -
 *
 * Description:
 * Send SNMP or SYSLOG messages for any loopback activity
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void t1_lp_print (dsx1_instance_t *dsx1)
{
     dsx1info         *info;
     dsx1MIBDataTable *mib;
     leveltype        SR;       /* interrupt level */

     info = dsx1->dsx1_ds;
     mib = &info->MIB;

     SR = raise_interrupt_level(NETS_DISABLE);

     if (dsx1->loopback_act & REM_LPBK_PASS) {
        errmsg(&msgsym(REMLOOP, CONTROLLER),
               info->cdb->cdb_namestring,
               info->loop == DSX1_LOOP_REMOTE ? "up" : "down",
               "passed");
     } else if (dsx1->loopback_act & REM_LPBK_FAIL) {
        errmsg(&msgsym(REMLOOP, CONTROLLER),
               info->cdb->cdb_namestring,
               info->loop == DSX1_LOOP_REMOTE ? "up" : "down",
               "failed");
    } else if (dsx1->loopback_act & LINE_LP_NET) {
        mib->C.dsx1LoopbackConfig = DSX1_LINE_LOOP;
        framer_set_MIB_ls(&mib->C.dsx1LineStatus, DSX1_LOOPED);
        errmsg(&msgsym(FIRMWARE, CONTROLLER),
               info->cdb->cdb_namestring,
               "LINE loopback request from external network");
    } else if (dsx1->loopback_act & NO_LINE_LP_NET) {
        mib->C.dsx1LoopbackConfig = DSX1_NO_LOOP;
        framer_clr_MIB_ls(&mib->C.dsx1LineStatus, DSX1_LOOPED);
        errmsg(&msgsym(FIRMWARE, CONTROLLER),
               info->cdb->cdb_namestring,
               "LINE loopback release from external network");
    } else if (dsx1->loopback_act & PAYLD_LP_NET) {
        mib->C.dsx1LoopbackConfig = DSX1_PAYLOAD_LOOP;
        framer_set_MIB_ls(&mib->C.dsx1LineStatus, DSX1_LOOPED);
        errmsg(&msgsym(FIRMWARE, CONTROLLER),
               info->cdb->cdb_namestring,
               "PAYLOAD loopback request from external network");
    } else if (dsx1->loopback_act & NO_PAYLD_LP_NET) {
        mib->C.dsx1LoopbackConfig = DSX1_NO_LOOP;
        framer_clr_MIB_ls(&mib->C.dsx1LineStatus, DSX1_LOOPED);
        errmsg(&msgsym(FIRMWARE, CONTROLLER),
               info->cdb->cdb_namestring,
               "PAYLOAD loopback release from external network");
   }
   dsx1->loopback_act = 0x0;

   reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * t1_sec_update -
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
void t1_sec_update ( dsx1_instance_t *t1, paktype *pak )
{
    int              cnt, interval;
    dsx1info         *info;
    ushort           other_status; 
    ushort           bpv_count, pcv_count, crc_count, fbe_count;
    dsx1TableType    *errptr, *old, *new;
    dsx1MIBDataTable *mib;
    cdbtype          *cdb_d;
    uchar            *mib_info;

    info = t1->dsx1_ds;
    cdb_d = info->cdb;
    mib = &info->MIB;
    errptr = &mib->CurrentData;


    /*
     * get the pointer which points to the message we want to process
     */
    mib_info = pak->datagramstart;
    /*
     * retrieve message from t1_sec_updateQ
     */
    other_status = (ushort)*mib_info++;
    bpv_count = (ushort)*mib_info++;
    bpv_count = (bpv_count << 8) + (ushort)*mib_info++;
    pcv_count = (ushort)*mib_info++;
    pcv_count = (pcv_count << 8) + (ushort)*mib_info++;
    crc_count = (ushort)*mib_info++;
    crc_count = (crc_count << 8) + (ushort)*mib_info++;
    fbe_count = (ushort)*mib_info++;
    fbe_count = (fbe_count << 8) + (ushort)*mib_info++;
 
    /*
     * Update BPV and PCV error counters 
     */
    if (bpv_count > 0) {
       errptr->dsx1LESs++;
       ADD_TO_GAUGE( errptr->dsx1LCVs, bpv_count );
    }

    if (pcv_count > 0) {
       ADD_TO_GAUGE( errptr->dsx1PCVs, pcv_count );
    }

    if (other_status & MSG_CSS_OCCURRED)
       errptr->dsx1CSSs++;
    
    if (other_status & MSG_FR_LOSS_OCCURRED)
       errptr->dsx1SEFSs++;

    /*
     * Now we check for unavailable / available states
     */
    if ( t1->unavailable_state ) {
        /*
         * Unavailable - we must have 10 non-SES seconds to become
         * available again
         */
        errptr->dsx1UASs++;
        if (!(other_status & MSG_SES_OCCURRED)) {
            t1->ses_count++;
            if ( t1->ses_count >= 10 ) {
                t1->ses_count = 0;
                t1->unavailable_state = FALSE;

		if (mib->C.dsx1TimeElapsed >= 9) {
		    errptr->dsx1UASs -= 10;
		} else {
                    /*
                     * Less than 10 seconds in this time period -
                     * subtract/add from the previous time period
                     */
                    if (mib->CurrentIndex != 0)
                        interval = mib->CurrentIndex - 1;
                    else
                        interval = QUARTER_HOURS_IN_DAY - 1;
                    old = &mib->Intervals[interval];
                    old->dsx1UASs -= 9 - mib->C.dsx1TimeElapsed;
                    old->dsx1ESs += t1->es_last_interval;
                    old->dsx1BESs += t1->bes_last_interval;
                    errptr->dsx1UASs = 0;
                }
                errptr->dsx1ESs += t1->es_in_failed;
                errptr->dsx1BESs += t1->bes_in_failed;

            } else {
                if ( other_status & MSG_ES_OCCURRED ) 
                     t1->es_in_failed++;
                if ( other_status & MSG_BES_OCCURRED ) 
                     t1->bes_in_failed++;
            }
        } else {
            t1->ses_count = 0;
            t1->bes_in_failed = 0;
            t1->es_in_failed = 0;
            t1->es_last_interval = 0;
            t1->bes_last_interval = 0;
        }
    } else {
        /*
         * Available - 10 SES in a row, we become unavailable
         */
            if ( other_status & MSG_ES_OCCURRED ) {
                errptr->dsx1ESs++;
            }
            /*
             * Now see if we have a degraded minute
             */
            t1->secs_for_min++;
            ADD_TO_GAUGE( t1->bpv_in_min, bpv_count );
            ADD_TO_GAUGE( t1->pcv_in_min, pcv_count );
            bpv_count = 0;
            pcv_count = 0;
            if ( t1->secs_for_min >= 60 ) {
                if (( t1->pcv_in_min > t1->pcvneeded_in_min )
                    || ( t1->bpv_in_min > t1->bpvneeded_in_min )) {
                    errptr->dsx1DMs++;
                }
                t1->secs_for_min = 0;
                t1->pcv_in_min = 0;
                t1->bpv_in_min = 0;
            }
        if (!(other_status & MSG_SES_OCCURRED)) {
            t1->ses_count = 0;
            if ( other_status & MSG_BES_OCCURRED ) {
                errptr->dsx1BESs++;
            }
        } else {
            errptr->dsx1SESs++;
            t1->ses_count++;
            if ( t1->ses_count >= 10 ) {
                 t1->ses_count = 0;

                if (mib->C.dsx1TimeElapsed >= 9) {
		    errptr->dsx1UASs += 10;
		    errptr->dsx1SESs -= 10;
                } else {
                    /*
                     * Less than 10 seconds in this time period -
                     * subtract/add from the previous time period
                     */
                    if (mib->CurrentIndex != 0)
                        interval = mib->CurrentIndex - 1;
                    else
                        interval = QUARTER_HOURS_IN_DAY - 1;
                    old = &mib->Intervals[interval];
                    old->dsx1UASs += 9 - mib->C.dsx1TimeElapsed;
                    old->dsx1SESs -= 9 - mib->C.dsx1TimeElapsed;
                    errptr->dsx1UASs = mib->C.dsx1TimeElapsed + 1;
                    errptr->dsx1SESs = 0;
                }
                t1->unavailable_state = TRUE;
                t1->es_in_failed = 0;
                t1->es_last_interval = 0;
                t1->bes_in_failed = 0;
                t1->bes_last_interval = 0;
            }
        }
    }

    mib->C.dsx1TimeElapsed++;
    /*
     * Check if 15 minutes passed then we need to update total MIB table
     */
    if ( mib->C.dsx1TimeElapsed >= 900 ) {
        if (t1->unavailable_state) {
            t1->bes_last_interval = t1->bes_in_failed;
            t1->bes_in_failed = 0;
            t1->es_last_interval = t1->es_in_failed;
            t1->es_in_failed = 0;
        }
         mib->C.dsx1TimeElapsed = 0;
         mib->C.dsx1ValidIntervals++;
         if (mib->C.dsx1ValidIntervals > QUARTER_HOURS_IN_DAY)
             mib->C.dsx1ValidIntervals = QUARTER_HOURS_IN_DAY;

         mib->CurrentData.dsx1Interval++;
         if (mib->CurrentData.dsx1Interval >= QUARTER_HOURS_IN_DAY)
             mib->CurrentData.dsx1Interval = 0;

         interval = mib->CurrentIndex;
         old = &mib->Intervals[interval];
         new = &mib->CurrentData;
        /*
         * save the next index
         */
        mib->CurrentIndex = interval + 1; /* for next time */
        if (mib->CurrentIndex >= QUARTER_HOURS_IN_DAY)
            mib->CurrentIndex = 0;

        mib->TotalData.dsx1ESs -= old->dsx1ESs ;
        ADD_TO_GAUGE(mib->TotalData.dsx1ESs, new->dsx1ESs);
        old->dsx1ESs = new->dsx1ESs ;

        mib->TotalData.dsx1SESs -= old->dsx1SESs;
        ADD_TO_GAUGE(mib->TotalData.dsx1SESs, new->dsx1SESs);
        old->dsx1SESs = new->dsx1SESs ;

        mib->TotalData.dsx1SEFSs -= old->dsx1SEFSs;
        ADD_TO_GAUGE(mib->TotalData.dsx1SEFSs, new->dsx1SEFSs);
        old->dsx1SEFSs = new->dsx1SEFSs ;

        mib->TotalData.dsx1UASs -= old->dsx1UASs;
        ADD_TO_GAUGE(mib->TotalData.dsx1UASs, new->dsx1UASs);
        old->dsx1UASs = new->dsx1UASs ;

        mib->TotalData.dsx1CSSs -= old->dsx1CSSs;
        ADD_TO_GAUGE(mib->TotalData.dsx1CSSs, new->dsx1CSSs);
        old->dsx1CSSs = new->dsx1CSSs ;

        mib->TotalData.dsx1PCVs -= old->dsx1PCVs;
        ADD_TO_GAUGE(mib->TotalData.dsx1PCVs, new->dsx1PCVs);
        old->dsx1PCVs = new->dsx1PCVs ;

        mib->TotalData.dsx1LESs -= old->dsx1LESs;
        ADD_TO_GAUGE(mib->TotalData.dsx1LESs, new->dsx1LESs);
        old->dsx1LESs = new->dsx1LESs ;

        mib->TotalData.dsx1BESs -= old->dsx1BESs;
        ADD_TO_GAUGE(mib->TotalData.dsx1BESs, new->dsx1BESs);
        old->dsx1BESs = new->dsx1BESs ;

        mib->TotalData.dsx1DMs -= old->dsx1DMs;
        ADD_TO_GAUGE(mib->TotalData.dsx1DMs, new->dsx1DMs);

        mib->TotalData.dsx1LCVs -= old->dsx1LCVs;
        ADD_TO_GAUGE(mib->TotalData.dsx1LCVs, new->dsx1LCVs);
        old->dsx1LCVs = new->dsx1LCVs ;

        framer_clr_dsx1table(new);

        /*
         * Go through and adjust the interval numbers to make the
         * latest set of data interval #1 (A pain, but it must be done)
         */
        for ( cnt = 1; cnt <= mib->C.dsx1ValidIntervals; cnt++ ) {
            mib->Intervals[interval].dsx1Interval = cnt;
            if ( interval == 0 ) interval = QUARTER_HOURS_IN_DAY - 1;
            else interval--;
        }
    }
}

/*
 * =====================================================================
 * t1_clear_neatmem -
 *
 * Description:
 * This function is called for clear all neat's memory space before
 * it download firmware and tonedata
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
static void t1_clear_neatmem (uint slot, uint total_words)
{
        int   addr, index;

        addr = 0;

        for (index=0; index <= total_words; index++) {
            t1_wr_neat_mem(slot, addr, 0);
            addr += 2;
        }
}

/*
 * =====================================================================
 * t1_download_data -
 *
 * Description:
 * This function is called for downloading data to NEAT's memory space
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
static void t1_download_data (uint slot, short *data_ptr,
                                         short total_words, uint addr)
{
    	int   index;

    	for (index=0; index < total_words; index++) {
            t1_wr_neat_mem(slot, addr, *data_ptr++);
            addr += 2;
    	}

}

/*
 * =====================================================================
 * t1_init -
 *
 * Description:
 * Initialize t1 data structure
 *
 * Parameters:
 * slot:                The slot #.
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
static void t1_init (dsx1_instance_t *t1, int slot)
{
    	dsx1MIBDataTable *mib;

	dsx1_cas_supp = TRUE;    	/* turn on cas support */

    	mib = &t1->dsx1_ds->MIB;
    
       /*
     	* The below are for a 1 x 10-6 error rate for 1 minute
     	*/
    	t1->bpvneeded_in_min = (1544000 * 60) / 1000000;
    	t1->crcneeded_in_min = (1544000 * 60) / 1000000;
    
       /*
     	* The t1_sec_updateQ is used to store one second full message from
     	* Neat processor. When main processor is too busy under heavy loading
     	* it probably won't be able to do one second update background process
     	* on schedule. In order to avoid losting full message from Neat
     	* processor, put it on process queue for process later is one way to
     	* solve this problem.
     	*/
    	queue_init(&t1_sec_updateQ, 0);

    	neat_ipc_init(slot);
    	framer_set_MIB_ls( &mib->C.dsx1LineStatus, DSX1_RX_LOF );
}

/*
 * =====================================================================
 * t1_dtmf_startup -
 *
 * Description:
 * This function is called by analyze_t1 as a one time initialization
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
void t1_dtmf_startup (mcn_instance_t *ds_mcn, uint subcont)
{
    	dtmf_instance_t *dtmf;

       /*
     	* Allocate memory for the Dual Tone Multi_frequency instance.
     	*/
    	dtmf = malloc_fast(sizeof(dtmf_instance_t));
    	if (!dtmf) {
       	 crashdump(0);
    	}

    	dtmf->unit = subcont;

       /*
     	* Store the DTMF instance pointer in the MCN instance.
     	* Henceforth, access the instance using DTMF_INSTANCE(slot, subcont).
     	*/
    	ds_mcn->dtmf_unit[subcont] = dtmf;

}

/*
 * =====================================================================
 * t1_startup -
 *
 * Description:
 * This function is called by analyze_t1 as an one time initialization
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
static void t1_startup (uint slot, uint type, uint subcont)
{
    	dsx1_instance_t *t1;
    	mcn_instance_t *ds_mcn;
    
    	ds_mcn = MCN_INSTANCE(slot);        /* get the mcn instance */
    
    	/*
     	* Allocate memory for the channelized T1 instance.
     	*/
    	t1 = malloc_fast(sizeof(dsx1_instance_t));
    	if (!t1) {
       	   crashdump(0);
    	}

       /*
     	* Store the T1 instance pointer in the MCN instance.
     	* Henceforth, access the instance using DSX1_INSTANCE(slot, subcont).
     	*/
    	ds_mcn->mcn_subcont[subcont].info.dsx1 = t1;
    	t1->appliquetype = type;
    	t1->slot = slot;
    	t1->unit = subcont;

    	nim_register_hi_irq_handler((ushort) slot, framer_interrupt,
                                 NIM_HI_IRQ_T1, (ushort) slot);
    	t1_int_array[0].function = (ulong)carrier_interrupt;
}

/*
 * =====================================================================
 * bt8360_analyze_mcn_port_adaptor -
 *
 * Description:
 * Initialize the T1 port adaptor in a given slot. 
 * Since Brasil T1 code share if_c4000_dsx1.c file with midrange T1 nim
 * code, here we use the same name as 4000 code for MCN port analyze
 * code registry
 *
 * Parameters:
 * slot.
 *
 * Returns:
 * none.
 * 
 * =====================================================================
 */
void bt8360_analyze_mcn_port_adaptor (int slot, int subcont, 
                                         int total_ctrlr_unit)
{
    	uint 	          type, slot_addr;
    	dsx1info          *t1_info;
    	dsx1_instance_t   *t1;
    	mcn_instance_t    *ds_mcn;

    	ds_mcn = MCN_INSTANCE(slot);        /* get the mcn instance */
    	type = mcn_get_adapter_type(slot);
    	slot_addr = (uint)BRASIL_GET_SLOT_BASE(slot);

    	if (type == MCN_PA_T1) {
	
	    /*
	     * Initialize the T1 instance.
	     */
	    t1_startup( slot, type, subcont );
	
            t1 = DSX1_INSTANCE(slot, subcont);

	    /*
	     * Init Dsx1. The dsx1_init_context is expected to return DSX1 info
             * structure pointer
	     */
	    t1_info = dsx1_init_context(MCN_PA_T1, PIFT_T1, APPL_T1_CSU, slot,
                                           subcont, total_ctrlr_unit);
            t1->dsx1_ds = t1_info;
	
	    t1_init(t1, slot);
        }
}

/*
 * Enable/Disable cas group
 */
static void t1_enable_cas_group (dsx1info *dsx1)
{
	msg_ct1_channels cas;
	int ix;
	uchar signal;
	

	for ( ix = 1; ix <= dsx1->max_timeslots; ix++) {
            if (dsx1->timeslots[ix] != UNUSED_CHAN)
	        signal = dsx1->signal_type[dsx1->timeslots[ix]];
            else
                signal = DSX1_SIGTYPE_NOSIG;
            if (ix & 1) {
                signal <<= 4;
                cas.sig_type[(ix-1) >> 1] = 0;
            }
            cas.sig_type[(ix-1) >> 1] |= signal;
	}

	cas.unit = dsx1->cdb->subunit;

	neat_ipc_message_send(dsx1->cdb->slot, MSG_MPU_SET_CT1_CHANNELS,  
			(uchar *)&cas, sizeof (msg_ct1_channels));
}

/*
 * =====================================================================
 * t1_download -
 *
 * Description:
 * Download NEAT firmware, busy and dial tone data
 *
 * Parameters:
 * slot.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void t1_download (int slot)
{
    	mcn_instance_t    *ds_mcn;
	pid_t             pid;

    	ds_mcn = MCN_INSTANCE(slot);

       /*
     	* Add the T1 framer Dsx1 registries
     	*/
    	framer_reg_add();

        /*
         * Disable backplane reset after power up or firmware reload to prevent
         * NEAT processor been reset during system reload with same firmware
         * version.
         */
        ds_mcn->mcn_regs->pri_control2 |= DSX1_ALL_RESET_EN;
        /*
         * Do neat and non-neat circuitry reset if firmware version is
         * not the same.
         */
        ds_mcn->mcn_regs->pri_control1 &= ~(DSX1_NEAT_RST);

        t1_clear_neatmem(slot, NEATMEM_SIZE);
        /*
         * Download firmware and tone data to NEAT's SRAM space
         */
        t1_download_data(slot, (ushort *)t1_firmware,
                                     (short)t1_firmware_words, 0);
        t1_download_data(slot, (ushort *)t1_dialtone,
                                     (short)t1_dialtone_words, (uint)DIALTONE_ADDR);
        t1_download_data(slot, (ushort *)t1_busytone,
                                     (short)t1_busytone_words, (uint)BUSYTONE_ADDR);
        /*
         * Bring the Neat processor out of the reset state and enable level 4
         * and level 6 interrupt from Neat processor.
         */
        ds_mcn->mcn_regs->pri_control1 |= ( DSX1_NEAT_RST
                                         | DSX1_L4_INT_EN | DSX1_L6_INT_EN);
        if (dsx1_cas_supp == TRUE) {
        	/* reset ipc queue */
		pid = process_create(neat_ipc_reset, "Reset ipc queue ",
				NORMAL_STACK, PRIO_NORMAL);
		if (pid == NO_PROCESS) {
		      printf("\nas5200_t1: Unable to create neat_ipc_reset process");
		      crashdump(0);
		}
		process_set_arg_num(pid, (ulong)slot);
        }


    t1_wr_neat_mem(slot, NEAT_LEDS, (BOARD_OK | COMPAND));
}
/******* RBS support *******/
/*
 * t1_rx_rbs_data_handler -
 *  The handler for message type MSG_NEAT_RX_RBS_DATA.
 */
 
static void t1_rx_rbs_data_handler (uchar slot, void *data)
{
 
        msg_show_rbs_resp *rbs = data;
        dsx1_instance_t *t1;
 
        t1 = DSX1_INSTANCE(slot, rbs->unit);
        /* save in the dsx instance */
        bcopy(rbs->signals, t1->dsx1_ds->rx_rbs, DSX1_MAX_CHAN/2);
}
/*
 * t1_tx_rbs_data_handler -
 *  The handler for message type MSG_NEAT_TX_RBS_DATA.
 */
 
static void t1_tx_rbs_data_handler (uchar slot, void *data)
{
 
        msg_show_rbs_resp *rbs = data;
        dsx1_instance_t *t1;
 
        t1 = DSX1_INSTANCE(slot, rbs->unit);
        /* save in the dsx instance */
        bcopy(rbs->signals, t1->dsx1_ds->tx_rbs, DSX1_MAX_CHAN/2);
}

/*
 * t1_cas_init -
 *   register cas specific functions.
 */

void t1_cas_init (uchar slot)
{
	reg_add_dsx1_set_cas_group(t1_enable_cas_group, " t1_enable_cas_group");
        neat_ipc_message_register( MSG_NEAT_RX_RBS_DATA,
                t1_rx_rbs_data_handler, NEAT_IPC_MSG_PRI_LO);
        neat_ipc_message_register( MSG_NEAT_TX_RBS_DATA,
                t1_tx_rbs_data_handler, NEAT_IPC_MSG_PRI_LO);

}
