/* $Id: if_as5200_e1.c,v 3.1.20.2 1996/08/28 12:57:05 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_e1.c,v $
 *------------------------------------------------------------------
 * if_as5200_e1.c - AS5200 E1 framer module
 *
 * March 1996, Simon Chen
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_e1.c,v $
 * Revision 3.1.20.2  1996/08/28  12:57:05  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.20.1  1996/06/16  21:14:44  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/03/13  23:53:46  sichen
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../if/if_msg_controller.c"	/* Not a typo, see logger.h */
#include "if_les.h"
#include "../if/if_controller.h"
#include "les_controller.h"
#include "../if/dsx1.h"
#include "if_as5200_mcn.h"
#include "if_c4000_m32.h"
#include "if_c4000_dsx1.h"
#include "if_as5200_dsx1_framer.h"
#include "if_as5200_dsx1.h"
#include "if_as5200_e1.h"
#include "if_as5200_dsx1_ipc.h"
#include "../os/timer_generic.h"
#include "packet.h"
#include "if_as5200_dummy.h"

queuetype e1_sec_updateQ;
static ipc_section *ipc;
short e1_version = 4;
/*
 * =====================================================================
 * e1_rd_neat_mem -
 *
 * Description:
 * Read word data from e1 NEAT memory space
 *
 * Parameters:
 * none
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static short e1_rd_neat_mem (uchar slot, uint mail_addr)
{
    int            temp;
    short          data;
    leveltype      SR;       /* interrupt level */
    ushort 	       old_page;


    data = 0x0;

    temp = mail_addr;
    temp &= PAGE_ADDR;
    temp >>= 12;
    mail_addr &= ~PAGE_ADDR;
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
 * e1_wr_neat_mem -
 *
 * Description:
 * Write word data to e1 NEAT memory space
 *
 * Parameters:
 * none
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void e1_wr_neat_mem (uchar slot, uint mail_addr, short data)
{
        int            temp;
	leveltype      SR;       /* interrupt level */
	ushort 	       old_page;


        temp = mail_addr;
        temp &= PAGE_ADDR;
        temp >>= 12;
        mail_addr &= ~PAGE_ADDR;
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
 * e1_mail_to_np -
 *
 * Description:
 * Common routine to send command to NEAT processor
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void e1_mail_to_np (uchar slot, ushort command, boolean set, int mail_addr)
{
  	short    current_cmd;
	leveltype      SR;       /* interrupt level */

	SR = raise_interrupt_level(ALL_DISABLE);  
	current_cmd = e1_rd_neat_mem(slot, mail_addr);

        if (set)
	    current_cmd |= command;
        else 
	    current_cmd &= ~command;

        e1_wr_neat_mem(slot, mail_addr, current_cmd);
	reset_interrupt_level(SR);
}

/*
 * =====================================================================
 * e1_framing -
 *
 * Description:
 * Sets the frame type for E1 controller
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void e1_framing (uint slot, uint subcont, short frametype)
{
    boolean         set;
    dsx1_instance_t *e1;
   
    e1 = DSX1_INSTANCE(slot, subcont);

    if (frametype != DSX1_FRM_E1 && frametype != DSX1_FRM_E1_CRC)
            return;

    if (e1->frame_type != frametype) {
        e1->frame_type = frametype;
        
        set = (frametype == DSX1_FRM_E1) ? TRUE : FALSE;
        e1_mail_to_np(slot, MPU_MSG_FRAME, set, DSX1_MAIL_TO_C(subcont));
    }
}

/*
 * =====================================================================
 * e1_linecode -
 *
 * Description:
 * Sets the line code for E1 controller
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void e1_linecode ( uint slot, uint subcont, short codetype )
{
    boolean         set;
    dsx1_instance_t *e1;
   
    e1 = DSX1_INSTANCE(slot, subcont);

    if (codetype != DSX1_LC_AMI && codetype != DSX1_LC_HDB3)
            return;

    set = (codetype == DSX1_LC_AMI) ? TRUE : FALSE;

    e1_mail_to_np(slot, MPU_MSG_LINECODE, set, DSX1_MAIL_TO_C(subcont));
}

/*
 * =====================================================================
 * e1_clk_sel0 -
 *
 * Description:
 * Sets the TDM clock source for E1 controller 0
 *
 * Parameters:
 * clocksource type
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void e1_clk_sel0 (dsx1info *info, mcn_instance_t *ds_mcn, uchar slot)
{
         short          temp;

         if ( info->clocksource == DSX1_PRIMARY ) {
              temp = ds_mcn->mcn_regs->pri_control2;
              temp |= (DSX1_PRI_CLK_SEL | DSX1_PRI_CLK_EN);
              ds_mcn->mcn_regs->pri_control2 = temp;
         } else if ( info->clocksource == DSX1_SECONDARY ) {
              temp = ds_mcn->mcn_regs->pri_control2;
              temp |= (DSX1_SEC_CLK_SEL | DSX1_SEC_CLK_EN);
              ds_mcn->mcn_regs->pri_control2 = temp;
         }
}

/*
 * =====================================================================
 * e1_clk_sel1 -
 *
 * Description:
 * Sets the TDM clock source for E1 controller 1
 *
 * Parameters:
 * clocksource type
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void e1_clk_sel1 (dsx1info *info, mcn_instance_t *ds_mcn, uchar slot)
{
         short       temp;

         if ( info->clocksource == DSX1_PRIMARY ) {
              temp = ds_mcn->mcn_regs->pri_control2;
              temp |= DSX1_PRI_CLK_EN;
              temp &= ~DSX1_PRI_CLK_SEL;
              ds_mcn->mcn_regs->pri_control2 = temp;
         } else if ( info->clocksource == DSX1_SECONDARY ) {
              temp = ds_mcn->mcn_regs->pri_control2;
              temp |= DSX1_SEC_CLK_EN;
              temp &= ~DSX1_SEC_CLK_SEL;
              ds_mcn->mcn_regs->pri_control2 = temp;
         }
}

/*
 * =====================================================================
 * e1_clocksource -
 *
 * Description:
 * Sets the clock source for E1 controller
 *
 * Parameters:
 * dsx1info and clocktype.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void e1_clocksource (dsx1info *info, uint subcont, short clocktype)
{
    uint            slot;
    dsx1_instance_t *e1;
    mcn_instance_t *ds_mcn;

    slot = info->cdb->slot;
    ds_mcn = MCN_INSTANCE(slot);
    e1 = DSX1_INSTANCE(slot, subcont);
    
    e1->clock_type = clocktype;

    if (clocktype == DSX1_CLK_INTERNAL) {
        e1_mail_to_np(slot, MPU_MSG_CLK_SOURCE, TRUE, DSX1_MAIL_TO_C(subcont));
    } else if (clocktype == DSX1_CLK_FREE) {
        *(ushort *)ADRSPC_BRASIL_TDM_PLL = 0x25;
        e1_mail_to_np(slot, MPU_MSG_CLK_SOURCE, TRUE, DSX1_MAIL_TO_C(subcont));
    } else {
        *(ushort *)ADRSPC_BRASIL_TDM_PLL = 0x35;
        e1_mail_to_np(slot, MPU_MSG_CLK_SOURCE, FALSE, DSX1_MAIL_TO_C(subcont));
        if ( subcont == 0 ) {
			if (info->clocksource == 0) {
				info->clocksource = DSX1_PRIMARY;
			} 
			e1_clk_sel0(info, ds_mcn, slot);
        } else {
			if (info->clocksource == 0) {
				info->clocksource = DSX1_SECONDARY;
				info->clocking = DSX1_CLK_THRU;
			}
			e1_clk_sel1(info, ds_mcn, slot);
		}
    }
}

/*
 * =====================================================================
 * e1_loopback_clk_switch -
 *
 * Description:
 * Statemachine for controlling the E1 line clock source selection
 * during automatic loopback (caused by excessive errors).
 *   We switch line clock source, from one E1 to the other E1, in either of
 *   the following two cases:
 *   1) If the primary line clock source is automatically put into 
 *      local loopback.
 *   2) If the secondary line clock source is automatically put into 
 *      local loopback AND the user didn't configure a primary source.
 *
 *   And we restore the original clock selection again in either of the 
 *   three cases:
 *   a) If the E1 that satisfied case 1) above is automatically taken out 
 *      of local loopback.
 *   b) If the E1 that satisfied case 2) above is automatically taken out 
 *      of local loopback.
 *   c) If the user decides to make a change to the clock source configuration.
 *
 * Event types:
 * DSX1_INTO_LOOP_EVENT    : a E1 has automatically been put into loopback.
 * DSX1_AUTORECOVERED_EVENT: a E1 has automatically recovered from loopback.
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
void e1_loopback_clk_switch(dsx1info *info, uint subcont, uchar event)
{
    	uint            slot;
    	dsx1_instance_t *e1;
    	mcn_instance_t  *ds_mcn;
        short		temp;
	static short	saved_reg;

    	slot = info->cdb->slot;
    	ds_mcn = MCN_INSTANCE(slot);
    	e1 = DSX1_INSTANCE(slot, subcont);
	temp = ds_mcn->mcn_regs->pri_control2;

	switch (event) {
	case DSX1_INTO_LOOP_EVENT:
	  if (ds_mcn->loopback_state == MCN_IDLE_STATE) {
	    if ((e1->unit == DSX1_PRIMARY_CLK_UNIT(temp)) &&
		(temp & DSX1_PRI_CLK_EN)) {
	      /* This loop backed E1 is primary clock source.
	       * Disable this E1's clocking, and temporary select + enable 
	       * the other E1 as secondary source.
	       */
	      saved_reg = temp;
	      temp &= ~DSX1_PRI_CLK_EN;
	      if (e1->unit == 0) {
		temp |= DSX1_SEC_CLK_EN;
		temp &= ~DSX1_SEC_CLK_SEL; 
	      } else {
		temp |= (DSX1_SEC_CLK_SEL | DSX1_SEC_CLK_EN); 
	      }
	      ds_mcn->mcn_regs->pri_control2 = temp;
	      ds_mcn->loopback_state = MCN_PRI_IN_LOOP_STATE;
	    } else if ((e1->unit == DSX1_SECONDARY_CLK_UNIT(temp)) &&
		       !(temp & DSX1_PRI_CLK_EN) && (temp & DSX1_SEC_CLK_EN)) {
	      /* This loop backed E1 is secondary clock source AND the
	       * user didn't configure a primary clock source.
	       * Disable this E1's clocking, and temporary select + enable 
	       * the other E1 as primary source.
	       */
	      saved_reg = temp;
	      temp &= ~DSX1_SEC_CLK_EN;
	      if (e1->unit == 0) {
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
	    if (e1->unit == DSX1_PRIMARY_CLK_UNIT(temp)) {
	      ds_mcn->mcn_regs->pri_control2 = saved_reg;
	      ds_mcn->loopback_state = MCN_IDLE_STATE;
	    }
	    break;
	    
	  case MCN_SEC_IN_LOOP_STATE:
	    if (e1->unit == DSX1_SECONDARY_CLK_UNIT(temp)) {
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
 * e1_local_up -
 *
 * Description:
 * Enable local loopback for E1 controller
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void e1_local_up ( uint slot, uint subcont )
{
    dsx1_instance_t *e1;

    e1 = DSX1_INSTANCE(slot, subcont);

    e1->loopback_type = LOCAL_LOOP;
    e1_mail_to_np(slot, MPU_MSG_LPBK_LOC, TRUE, DSX1_MAIL_TO_C(subcont));

    /*
     * Turn on remote loopback LED since it does loopback at framer
     * and transceiver chip
     */

	if (!subcont) {
    	e1_mail_to_np(slot, LOOPBACK_0, TRUE, NEAT_LEDS);
	} else {
    	e1_mail_to_np(slot, LOOPBACK_1, TRUE, NEAT_LEDS);
	}
}

/*
 * =====================================================================
 * e1_no_loop -
 *
 * Description:
 * Disable local loopback for E1 controller
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void e1_no_loop ( uint slot, uint subcont )
{
    dsx1_instance_t *e1;

    e1 = DSX1_INSTANCE(slot, subcont);

    e1->loopback_type = 0;
    e1_mail_to_np(slot, MPU_MSG_LPBK_LOC, FALSE, DSX1_MAIL_TO_C(subcont));

    /*Turn off local and remote loopback LED*/
	if (!subcont) {
    	e1_mail_to_np(slot, LOOPBACK_0, FALSE, NEAT_LEDS);
	} else {
    	e1_mail_to_np(slot, LOOPBACK_1, FALSE, NEAT_LEDS);
	}
}

/*
 * =====================================================================
 * e1_txAIS -
 *
 * Description:
 * Send AIS out to indicate E1 controller has been shutdown
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void e1_txAIS ( uint slot, uint subcont, boolean set )
{

    e1_mail_to_np(slot, MPU_MSG_DISABLED, set, DSX1_MAIL_TO_C(subcont));

}

/*
* e1_force_remote_alarm
* Forces an RAI signal out the transmitter.  For Australian Homologation
*/
static void e1_force_remote_alarm (uint slot, uint subcont, boolean set)
{
	e1_mail_to_np(slot, MPU_MSG_SEND_RAI, set, DSX1_MAIL_TO_C(subcont));
}

/*
 * e1_homologation_type
 * Will indicate the homologation type (right now, just Australia has
 * differences).  It will also turn on the RAI signal if we are
 * unavailable if we are in Australia (as per the spec)
 */
void e1_homologation_type (uint slot, uint subcont, enum dsx1_homologation_type type)
{
	dsx1_instance_t *e1;

	e1 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
	e1_mail_to_np(slot, MPU_MSG_HOMOL_TYPE, (type == DSX1_HOMOL_AUSTRALIA),
		 DSX1_MAIL_TO_C(subcont));
	if ((type == DSX1_HOMOL_AUSTRALIA) &&
		(e1->unavailable_state == TRUE)) {
		e1_force_remote_alarm(slot, subcont, TRUE);
	} else {
		e1_force_remote_alarm(slot, subcont, FALSE);
	}
}

/*
 * =====================================================================
 * e1_l6_interrupt -
 *
 * Description:
 * E1 level 6 interrupt service routine
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void e1_l6_interrupt (uint slot)
{
    short       cause, src, temp;
    int         ix;
    mcn_instance_t *ds_mcn;

    ds_mcn = MCN_INSTANCE(slot);    /* get the mcn instance */

    cause = ds_mcn->mcn_regs->pri_status;

    if (cause & DSX1_NEAT_INT_L6)  {
        dsx1_instance_t     *e1;
        dsx1info            *info;
        dsx1MIBDataTable    *ee;

        /* clear the level 6 interrupt by writing 1 to Neat INT register */
        e1_wr_neat_mem(slot, DSX1_NEAT_INT, CLR_LEVEL6);

        for (ix = 0; ix < MAX_DSX1_PER_SLOT ; ix++) {
             e1 = DSX1_INSTANCE(slot, ix);
             info = e1->dsx1_ds;
             ee = &info->MIB;
             /*
              * clear carrier status after read it, so make it available for NEAT
              * processor to update the carrier status if changed
              */
              src = e1_rd_neat_mem(slot, DSX1_CARRIER_AVAIL(e1->unit));
              e1->carrier_status = src;
              e1_wr_neat_mem(slot, DSX1_CARRIER_AVAIL(e1->unit), 0);
            /*
             * read board status and update it to dsx1LineStatus to reflect
             * the latest one.
             */  
             src = e1_rd_neat_mem(slot, DSX1_CARRIER_AVAIL(e1->unit)+2);
             ee->C.dsx1LineStatus = src;

            /*
             * if receive remote alarm then turn on remote alarm LED
             */  
             if (src & DSX1_RX_REM_ALARM) {
				if (!ix) {
    				e1_mail_to_np(slot, REM_ALARM_0, FALSE, NEAT_LEDS);
				} else {
    				e1_mail_to_np(slot, REM_ALARM_1, FALSE, NEAT_LEDS);
				}
             } else {
				if (!ix) {
					e1_mail_to_np(slot, REM_ALARM_0, TRUE, NEAT_LEDS);
				} else {
				e1_mail_to_np(slot, REM_ALARM_1, TRUE, NEAT_LEDS);
				}
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
 * e1_carrier_update -
 *
 * Description:
 * E1 carrier state update
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void e1_carrier_update ( dsx1_instance_t *e1, uint subcont )
{
    dsx1info    *info;
    cdbtype     *cdb_d;

         info = e1->dsx1_ds;
         cdb_d = info->cdb;


         if (e1->carrier_status == CARRIER_UP) {
         /*
          * Before we bring up controller, make sure it isn't in shutdown
          * or unavailable state
          */
              if ((e1->unavailable_state == FALSE) && 
                  (cdb_d->cdb_state != IDBS_ADMINDOWN)) {
                    if (cdb_d->cdb_state != IDBS_UP) { 
                        errmsg(&msgsym(UPDOWN, CONTROLLER),
                               cdb_d->cdb_namestring, print_cdbstate(IDBS_UP));

                        cdb_d->cdb_state = IDBS_UP;
                        dsx1_create_idle_channel(info);
                        dsx1_change_all_idbs_state(cdb_d, IDBS_UP);
						if (!subcont) {
    						e1_mail_to_np(e1->slot, LOCAL_ALARM_0, TRUE, NEAT_LEDS);
						} else {
    						e1_mail_to_np(e1->slot, LOCAL_ALARM_1, TRUE, NEAT_LEDS);
						}
                    }
              }
         } else if (e1->carrier_status == CARRIER_DOWN) {
              if (cdb_d->cdb_state == IDBS_UP) {
                  errmsg(&msgsym(UPDOWN, CONTROLLER),
                      cdb_d->cdb_namestring, print_cdbstate(IDBS_DOWN));

                  cdb_d->cdb_state = IDBS_DOWN;
                  dsx1_change_all_idbs_state(cdb_d, IDBS_DOWN);
					if (!subcont) {
    					e1_mail_to_np(e1->slot, LOCAL_ALARM_0, FALSE, NEAT_LEDS);
					} else {
    					e1_mail_to_np(e1->slot, LOCAL_ALARM_1, FALSE, NEAT_LEDS);
					}
              }
         }
}

/*
 * =====================================================================
 * e1_get_digits -
 *
 * Description:
 * Copy the complete phone digits set from NEAT's memory to system memory
 *
 * Parameters:
 * E1 and DTMF data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void e1_get_digits (dsx1_instance_t *e1, dtmf_instance_t *dtmf, 
                         int fromdtmf_addr)
{
    	ushort		tempread, ix, total_digits;
        uchar           temp;
        uint            slot;

        slot = e1->slot;
        total_digits = e1_rd_neat_mem(slot, fromdtmf_addr + TOTAL_NUM_OFFSET);
		ix = 0;
        while (ix < total_digits) {
            tempread = e1_rd_neat_mem(slot, (fromdtmf_addr+PHONE_NUM_OFFSET+ix));
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
 * e1_reply_to_csm -
 *
 * Description:
 * E1 level 4 interrupt service routine
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void e1_reply_to_csm (dsx1_instance_t *e1, dtmf_instance_t *dtmf, 
                      int from_dtmf_addr, int to_dtmf_addr)
{
	ushort from_dtmf, to_dtmf, temp_msg;
        uint   slot;

        slot = e1->slot;

         from_dtmf = e1_rd_neat_mem(slot, from_dtmf_addr);

         if (from_dtmf & MSG_RDY_DIGCOL) {

             to_dtmf = e1_rd_neat_mem(slot, to_dtmf_addr);
             temp_msg = to_dtmf & MODEM_ID_GET;
             dsx1_mail_from_neat(READY_COL, temp_msg, e1->slot, dtmf->unit);

         } else if (from_dtmf & MSG_1STDIG_TO) {

             to_dtmf = e1_rd_neat_mem(slot, to_dtmf_addr);
             temp_msg = to_dtmf & MODEM_ID_GET;
             dsx1_mail_from_neat(FIRST_DIGIT_TO, temp_msg, e1->slot,
                                 dtmf->unit);

         } else if (from_dtmf & MSG_1STDIG_RCV) {

             to_dtmf = e1_rd_neat_mem(slot, to_dtmf_addr);
             temp_msg = to_dtmf & MODEM_ID_GET;
             dsx1_mail_from_neat(FIRST_DIGIT_RCV, temp_msg, e1->slot,
                                 dtmf->unit);

         } else if (from_dtmf & MSG_DIG_DONE) {

             e1_get_digits(e1, dtmf, from_dtmf_addr);
             to_dtmf = e1_rd_neat_mem(slot, to_dtmf_addr);
             temp_msg = to_dtmf & MODEM_ID_GET;
             dsx1_mail_from_neat(ALL_DIGIT_RCV, temp_msg, e1->slot, dtmf->unit);
         }
}

/*
 * =====================================================================
 * e1_l4_interrupt -
 *
 * Description:
 * E1 level 4 interrupt service routine
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void e1_l4_interrupt (uint slot)
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
        dsx1_instance_t     *e1;
        dsx1info            *info;
        dsx1MIBDataTable    *ee;

        /* clear the level 4 interrupt by writing 1 to Neat INT register */
        e1_wr_neat_mem(slot, DSX1_NEAT_INT, CLR_LEVEL4);

        for (ix = 0; ix < MAX_DSX1_PER_SLOT ; ix++) {
             e1 = DSX1_INSTANCE(slot, ix);
             info = e1->dsx1_ds;
             ee = &info->MIB;

             /*
              * Read message byte from NEAT processor's ram to determinate 
              * which message type
              */
             mail_addr = DSX1_MAIL_FROM_C(ix);

	     if (systeminit_flag) {
                 src = e1_rd_neat_mem(slot, mail_addr);
	         /*
	          * clear carrier status after read it, so make it available for NEAT
	          * processor to update the carrier status if changed
	          */
	         switch (src) {
	     	    case MSG_TYPE_FULL:
                /*
                 * Receive full message from NIM processor, so need to read all
                 * message and update those message accordingly.The code rely
                 * on this interrupt to figure out one second interval, so it
                 * can update MIB table
                 */  
              
                /*
                 * clear alive count when receive full message from NIM processor
                 * This count is used to monitor NIM processor is still alive or
                 * not. If this count is equal to or more than 10, then there
                 * must be something wrong on NIM processor
                 */
                 e1->alive_count = 0;
               
                /*
                 * Update carrier state according to carrier available byte from
                 * NIM processor's ram. Clear those two bytes after read them.
                 */
			     /* E1PRI
                 tempread = e1_rd_neat_mem(slot, mail_addr + BOARD_STATUS_OFFSET);
                 e1->carrier_status = tempread;
                 e1_wr_neat_mem(slot, CE1_CARRIER_AVAIL, 0);
			     */
 
                /*
                 * Read board status and update them to DSX1 line status in
                 * configuration table
                 */
                 tempread = e1_rd_neat_mem(slot, mail_addr + BOARD_STATUS_OFFSET);
                 ee->C.dsx1LineStatus = tempread;
 
                /*
                 * if receive remote alarm then turn on remote alarm LED
                 */
                 if (tempread & DSX1_RX_REM_ALARM) {
				    if (!ix) {
    				    e1_mail_to_np(slot, REM_ALARM_0, FALSE, NEAT_LEDS);
				    } else {
    				    e1_mail_to_np(slot, REM_ALARM_1, FALSE, NEAT_LEDS);
				    }
                 } else {
				    if (!ix) {
    				    e1_mail_to_np(slot, REM_ALARM_0, TRUE, NEAT_LEDS);
				    } else {
    				    e1_mail_to_np(slot, REM_ALARM_1, TRUE, NEAT_LEDS);
				    }
                 }
                /*
                 * Read MIB error messages from NIM process and update later
                 * we allocate a smallest buffer from public pool to store the
                 * full message from NIM processor since the message is only
                 * 9 bytes
                 */
                 if ((pak = getbuffer(MAX_UPDATE_SIZE)) == NULL)
                      break;
 
                 mib_info = pak->datagramstart;
 
                 *mib_info++ = e1_rd_neat_mem(slot, mail_addr + OTHER_STATUS_OFFSET);

                 tempread = e1_rd_neat_mem(slot,mail_addr + BPV_COUNT_OFFSET);
                 *mib_info++ = tempread >> 8; /* get upper byte of BPV count */
                 *mib_info++ = tempread & 0xff; /* get lower byte of BPV count */

                 tempread = e1_rd_neat_mem(slot,mail_addr + PCV_COUNT_OFFSET);
                 *mib_info++ = tempread >> 8; /* get upper byte of PCV_COUNT_OFFSET count */
                 *mib_info++ = tempread & 0xff; /* get lower byte of PCV_COUNT_OFFSET count */

                 tempread = e1_rd_neat_mem(slot,mail_addr + CRC_COUNT_OFFSET);
                 *mib_info++ = tempread >> 8; /* get upper byte of CRC_COUNT_OFFSET count */
                 *mib_info++ = tempread & 0xff; /* get lower byte of RC_COUNT_OFFSET count */

                 tempread = e1_rd_neat_mem(slot,mail_addr + FBE_COUNT_OFFSET);
                 *mib_info++ = tempread >> 8; /* get upper byte of FBE_COUNT_OFFSET count */
                 *mib_info++ = tempread & 0xff; /* get lower byte of FBE_COUNT_OFFSET count */
 
                /*
                 * Store dsx1 data structure pointer to rx_descriptor. so when
                 * one second update background process is called, the code
                 * will pass the right pointer if more than one CE1 nim are
                 * put in the system
                 */
                 pak->rx_descriptor = e1;
 
                 if (pak_enqueue(&e1_sec_updateQ, pak) == NULL) {
                     retbuffer(pak);
                    }
 
                 break;
         
		    case MSG_ERROR:
                   /*
                    * The errors word from NEAT processor can be used to report
                    * some errors occurs on E1 card. Right now only one error
                    * report, it could report 16 different errors in the future
                    */
                   tempread = e1_rd_neat_mem(slot, mail_addr + ERRORS_OFFSET);
                   if (tempread & MSG_DIG_MAX_ERR) {
                      buginf("*** collect too many phone digits number ***\n");

                 } 
			     break;
		    case  MSG_DTMF0:
				      /*
				      E1PRI: we need to change this function name to more general
				      */
                      e1_reply_to_csm(e1, DTMF_INSTANCE(e1->slot, 0), 
                                       DSX1_FROM_DTMF(0), DSX1_TO_DTMF(0));
			     break;
		    case MSG_DTMF1:
                      e1_reply_to_csm(e1, DTMF_INSTANCE(e1->slot, 1), 
                                       DSX1_FROM_DTMF(1), DSX1_TO_DTMF(1));
			    break;
		    default:
			    break;
		    }
	     }

           /*
            * clear message byte after read it, so make it available for NEAT
            * processor to send next message
            */
           e1_wr_neat_mem(slot, mail_addr, 0);
       }

    }
}

/*
 * =====================================================================
 * e1_sec_update -
 *
 * Description:
 * E1 receive errors second update
 *
 * Parameters:
 * Bt8360 data structure instance.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void e1_sec_update ( dsx1_instance_t *e1, uint subcont, paktype *pak )
{
	int  cnt, interval;
	dsx1info  *info;
	ushort  other_status; 
	ushort  bpv_count, pcv_count, crc_count, fbe_count;
	dsx1TableType  *errptr, *old, *new;
	dsx1MIBDataTable  *mib;
	cdbtype  *cdb_d;
	uchar  *mib_info;

	info = e1->dsx1_ds;
	cdb_d = info->cdb;
	mib = &info->MIB;
	errptr = &mib->CurrentData;


	/*
	* get the pointer which points to the message we want to process
	*/
	mib_info = pak->datagramstart;
	/*
	* retrieve message from e1_sec_updateQ
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
	if ( e1->unavailable_state ) {
		/*
		* Unavailable - we must have 10 non-SES seconds to become
		* available again
		*/
		errptr->dsx1UASs++;
		if (!(other_status & MSG_SES_OCCURRED)) {
			e1->ses_count++;
			if ( e1->ses_count >= 10 ) {
				e1->ses_count = 0;
				e1->unavailable_state = FALSE;
				if (!subcont) {
					e1_mail_to_np(e1->slot, LOCAL_ALARM_0, TRUE, NEAT_LEDS);
				} else {
					e1_mail_to_np(e1->slot, LOCAL_ALARM_1, TRUE, NEAT_LEDS);
				}
	
				if (info->homologation_type == DSX1_HOMOL_AUSTRALIA) {
					e1_force_remote_alarm(e1->slot, subcont, FALSE);
				}
	
				if (cdb_d->cdb_state != IDBS_ADMINDOWN) {
					errmsg(&msgsym(UPDOWN, CONTROLLER),
						cdb_d->cdb_namestring, print_cdbstate(IDBS_UP));
					cdb_d->cdb_state = IDBS_UP;
					dsx1_change_all_idbs_state(cdb_d, IDBS_UP);
				}
		
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
					old->dsx1ESs += e1->es_last_interval;
					old->dsx1BESs += e1->bes_last_interval;
					errptr->dsx1UASs = 0;
				}
				errptr->dsx1ESs += e1->es_in_failed;
				errptr->dsx1BESs += e1->bes_in_failed;
			} else {
				if ( other_status & MSG_ES_OCCURRED ) 
					e1->es_in_failed++;
				if ( other_status & MSG_BES_OCCURRED ) 
					e1->bes_in_failed++;
			}
		} else {
			e1->ses_count = 0;
			e1->bes_in_failed = 0;
			e1->es_in_failed = 0;
			e1->es_last_interval = 0;
			e1->bes_last_interval = 0;
		}
	} else {
		/*
		* Available - 10 SES in a row, we become unavailable
		*/
		if (!(other_status & MSG_SES_OCCURRED)) {
			e1->ses_count = 0;
			if ( other_status & MSG_ES_OCCURRED ) {
				errptr->dsx1ESs++;
			}
			if ( other_status & MSG_BES_OCCURRED ) {
				errptr->dsx1BESs++;
			}
			/*
			* Now see if we have a degraded minute
			*/
			e1->secs_for_min++;
			ADD_TO_GAUGE( e1->bpv_in_min, bpv_count );
			ADD_TO_GAUGE( e1->pcv_in_min, pcv_count );
			bpv_count = 0;
			pcv_count = 0;
			if ( e1->secs_for_min >= 60 ) {
				if (( e1->pcv_in_min > e1->pcvneeded_in_min )
					|| ( e1->bpv_in_min > e1->bpvneeded_in_min )) {
					errptr->dsx1DMs++;
				}
				e1->secs_for_min = 0;
				e1->pcv_in_min = 0;
				e1->bpv_in_min = 0;
			}
		} else {
			errptr->dsx1SESs++;
			e1->ses_count++;
			if ( e1->ses_count >= 10 ) {
				e1->ses_count = 0;
				/* We haven't lost frame - but have too many errors -
				* disable the M32 channels
				*/
				if (info->homologation_type == DSX1_HOMOL_AUSTRALIA) {
					/* In Australia, force the RAI signal */
					e1_force_remote_alarm(e1->slot, subcont, TRUE);
				}
	
				if (!subcont) {
					e1_mail_to_np(e1->slot, LOCAL_ALARM_0, FALSE, NEAT_LEDS);
				} else {
					e1_mail_to_np(e1->slot, LOCAL_ALARM_1, FALSE, NEAT_LEDS);
				}
				if (cdb_d->cdb_state == IDBS_UP) {
					errmsg(&msgsym(UPDOWN, CONTROLLER),
					cdb_d->cdb_namestring, print_cdbstate(IDBS_DOWN));
					cdb_d->cdb_state = IDBS_DOWN;
					dsx1_change_all_idbs_state(cdb_d, IDBS_DOWN);
				}
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
				e1->unavailable_state = TRUE;
				e1->es_in_failed = 0;
				e1->es_last_interval = 0;
				e1->bes_in_failed = 0;
				e1->bes_last_interval = 0;
			}
		}
	}
	
	mib->C.dsx1TimeElapsed++;
	/*
	* Check if 15 minutes passed then we need to update total MIB table
	*/
	if ( mib->C.dsx1TimeElapsed >= 900 ) {
		if (e1->unavailable_state) {
			e1->bes_last_interval = e1->bes_in_failed;
			e1->bes_in_failed = 0;
			e1->es_last_interval = e1->es_in_failed;
			e1->es_in_failed = 0;
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
 * e1_download_data -
 *
 * Description:
 * This function is called for downloading E1 fireware/data
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
static void e1_download_data (uint slot, short *data_ptr,
                                         short total_words, uint addr)
{
    int   index;

    for (index=0; index < total_words; index++) {
         e1_wr_neat_mem(slot, addr, *data_ptr++);
         addr += 2;
    }

}

/*
 * =====================================================================
 * e1_init -
 *
 * Description:
 * Initialize e1 data structure
 *
 * Parameters:
 * slot:                The slot #.
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
static void e1_init (dsx1_instance_t *e1, uint subcont, int slot)
{
    dsx1MIBDataTable *mib;
    mcn_instance_t *ds_mcn;


    /* Setup ipc address */
    ipc = (ipc_section *)IPC_AREA;

    ds_mcn = MCN_INSTANCE(slot);        /* get the mcn instance */
    mib = &e1->dsx1_ds->MIB;
    
    /*
     * The below are for a 1 x 10-6 error rate for 1 minute
     */
    e1->bpvneeded_in_min = (E1_FREQ_HZ * MIN_IN_SECS) / MEGA_UNIT;
    e1->pcvneeded_in_min = (E1_FREQ_HZ * 6 * MIN_IN_SECS) / MEGA_UNIT;
    
    mib->C.dsx1LineType = DSX1_OTHER_FRAME;     /* until found or framed */
    mib->C.dsx1LineCoding = DSX1_LC_HDB3;        /* until B8ZS found */
    mib->C.dsx1LoopbackConfig = DSX1_NO_LOOP;

    /*
     * The e1_sec_updateQ is used to store one second full message from
     * Neat processor. When main processor is too busy under heavy loading
     * it probably won't be able to do one second update background process
     * on schedule. In order to avoid losting full message from Neat
     * processor, put it on process queue for process later is one way to
     * solve this problem.
     */
    queue_init(&e1_sec_updateQ, 0);

    /*
     * Turn on local alarm but turn off remote alarm LED
     */
	if (!subcont) {
    	e1_mail_to_np(slot, REM_ALARM_0, TRUE, NEAT_LEDS);
    	e1_mail_to_np(slot, LOCAL_ALARM_0, FALSE, NEAT_LEDS);
	} else {
    	e1_mail_to_np(slot, REM_ALARM_1, TRUE, NEAT_LEDS);
    	e1_mail_to_np(slot, LOCAL_ALARM_1, FALSE, NEAT_LEDS);
	}
 
    /*Turn off local and remote loopback LED*/
	if (!subcont) {
    	e1_mail_to_np(slot, LOOPBACK_0, TRUE, NEAT_LEDS);
	} else {
    	e1_mail_to_np(slot, LOOPBACK_1, TRUE, NEAT_LEDS);
	}
 
    framer_set_MIB_ls( &mib->C.dsx1LineStatus, DSX1_RX_LOF );
    /*
     * Bring the NIM processor out of the reset state and enable force MPU
     * interrupt from NIM processor.
     */
	/* E1PRI: Does this in if_as5200_mcn.c
    *ce1->mpu->np_control &= ~(CE1_NP_RESET);
    *ce1->mpu->np_control |= CE1_NP_INT_ENABLE;
	*/
}

/*
 * =====================================================================
 * e1_dtmf_startup -
 *
 * Description:
 * This function is called by analyze_e1 as a one time initialization
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
void e1_dtmf_startup (mcn_instance_t *ds_mcn, uint subcont)
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
 * e1_startup -
 *
 * Description:
 * This function is called by analyze_e1 as a one time initialization
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
static void e1_startup (uint slot, uint type, uint subcont)
{
    dsx1_instance_t *e1;
    mcn_instance_t *ds_mcn;
    
    ds_mcn = MCN_INSTANCE(slot);        /* get the mcn instance */
    
    /*
     * Allocate memory for the channelized E1 instance.
     */
    e1 = malloc_fast(sizeof(dsx1_instance_t));
    if (!e1) {
        crashdump(0);
    }

    /*
     * Store the E1 instance pointer in the MCN instance.
     * Henceforth, access the instance using DSX1_INSTANCE(slot, subcont).
     */
    ds_mcn->mcn_subcont[subcont].info.dsx1 = e1;
    e1->appliquetype = type;
    e1->slot = slot;
    e1->unit = subcont;

    nim_register_hi_irq_handler((ushort) slot, framer_interrupt,
                                 NIM_HI_IRQ_T1, (ushort) slot);
	/* E1PRI */
    t1_int_array[0].function = (ulong)carrier_interrupt;
}

/*
 * =====================================================================
 * ce1_analyze_mcn_port_adaptor -
 *
 * Description:
 * Initialize the E1 port adaptor in a given slot. 
 * Since Brasil E1 code share if_c4000_dsx1.c file with midrange E1 nim
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
void ce1_analyze_mcn_port_adaptor (int slot, int subcont, 
                                         int total_ctrlr_unit)
{
	uint 	      type, slot_addr;
	dsx1info          *e1_info;
	dsx1_instance_t   *e1;
	mcn_instance_t    *ds_mcn;
	short temp;

	ds_mcn = MCN_INSTANCE(slot);        /* get the mcn instance */
	type = mcn_get_adapter_type(slot);
	slot_addr = (uint)BRASIL_GET_SLOT_BASE(slot);

	if (type == MCN_PA_E1) {
		/*
		* Initialize the E1 instance.
		*/
		e1_startup( slot, type, subcont );
		e1 = DSX1_INSTANCE(slot, subcont);
		/*
		* Init Dsx1. The dsx1_init_context is expected to return DSX1 info
		* structure pointer
		*/
#if defined(E1_PRI)
		temp = e1_rd_neat_mem(slot, LIU_STATUS);
#else
		temp = (UNBALANCE_0 | UNBALANCE_1);
#endif
		/*
		* Init Dsx1.
		*/
		if (((temp & UNBALANCE_0) && !subcont) || 
					((temp & UNBALANCE_1) && subcont)) {
			e1_info = dsx1_init_context(MCN_PA_E1, PIFT_E1,
						APPL_E1_UNBAL, slot, subcont, total_ctrlr_unit);
		} else {
			e1_info = dsx1_init_context(MCN_PA_E1, PIFT_E1,
						APPL_E1_BAL, slot, subcont, total_ctrlr_unit);
		}
		e1->dsx1_ds = e1_info;
		/*
		* Add the E1 framer Dsx1 registries and init Bt8510 framer chip
		*/
		e1_init(e1, subcont, slot);
	}
}


/*
 * =====================================================================
 * e1_download -
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
void e1_download (int slot)
{
    mcn_instance_t    *ds_mcn;
    short             old_fw_version;

    ds_mcn = MCN_INSTANCE(slot);

	framer_reg_add();
    /*
     * read old version from NEAT's SRAM space.
     */
    old_fw_version = e1_rd_neat_mem(slot, DSX1_FW_VERSION);

    /*
     * If new firmware version is different from the old one which was stored
     * in NEAT's SRAM, the code will do reset on NEAT and non NEAT circuitry.
     * This includes power-up condition.
     */
/*    if (e1_version != old_fw_version) { */
      if (TRUE) {
        /* write the new version back to NEAT's SRAM space */
        e1_wr_neat_mem(slot, DSX1_FW_VERSION, e1_version);

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
        ds_mcn->mcn_regs->pri_control1 = ~(DSX1_NEAT_RST);

        /*
         * Download firmware and tone data to NEAT's SRAM space
         */
/*
        e1_download_data(slot, (ushort *)e1_firmware,
                                     (short)e1_firmware_words, 0);
*/
        e1_download_data(slot, (ushort *)t1_busytone,
                                     (short)t1_busytone_words, (uint)BUSYTONE_ADDR);
        e1_download_data(slot, (ushort *)t1_dialtone,
                                     (short)t1_dialtone_words, (uint)DIALTONE_ADDR);
        /*
         * Bring the Neat processor out of the reset state and enable level 4
         * and level 6 interrupt from Neat processor.
         */
        ds_mcn->mcn_regs->pri_control1 |= ( DSX1_NEAT_RST
                                         | DSX1_L4_INT_EN | DSX1_L6_INT_EN);
    }
/*  e1_wr_neat_mem(slot, DSX1_RELOAD_DONE, 0x01);*/
}
