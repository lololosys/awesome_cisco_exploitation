/* $Id: if_as5200_dsx1_framer.c,v 3.1.68.6 1996/09/07 22:36:57 jliu Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_dsx1_framer.c,v $
 *------------------------------------------------------------------
 * if_as5200_dsx1_framer.c - Common framer functions for any E1/T1 interface.
 *
 * Nov 1995, Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_dsx1_framer.c,v $
 * Revision 3.1.68.6  1996/09/07  22:36:57  jliu
 * CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
 * Branch: California_branch
 *     o The fix included display warning message to console port and used
 *       SNMP notification to send the message to network management
 *       station
 *
 * Revision 3.1.68.5  1996/09/02  10:36:10  styang
 * CSCdi61183:  Cannot tell hardware revision of cards from command line
 * Branch: California_branch
 * Modify the printf's format for the E1 card.
 *
 * Revision 3.1.68.4  1996/09/02  08:37:03  styang
 * CSCdi61183:  Cannot tell hardware revision of cards from command line
 * Branch: California_branch
 * To show hardware version info for the modem card and T1/E1 card.
 *
 * Revision 3.1.68.3  1996/08/28  12:57:02  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.68.2  1996/07/11  22:36:56  jliu
 * CSCdi62357:  need to support AT&T and ANSI facility data link standard
 * Branch: California_branch
 *           o Added new fdl command for t1 controller in order to support
 *             CSU function
 *
 * Revision 3.1.68.1  1996/06/16  21:14:34  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/09  23:55:54  jliu
 * Create placeholder for new files
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
#include "../if/network.h"
#include "if_as5200_mcn.h"
#include "../if/dsx1.h"
#include "if_as5200_dsx1_framer.h"
#include "if_as5200_dsx1.h"
#include "if_as5200_t1.h"
#include "if_as5200_e1.h"
#include "if_as5200_dsx1_ipc.h"
#include "if_c4000_dsx1.h"
#include "../if/dsx1_registry.h"
#include "packet.h"
#include "../src-68-as5200/as5200.h"
#include "../src-68-as5200/as5200_pcmap.h"
#include "if_c4000_m32.h"

/*
 * =====================================================================
 * framer_background_process -
 *
 * Description:
 * Run frame process algorithm for every 250ms
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
forktype framer_background_process (void)
{
    cdbtype            *cdb;
    dsx1_instance_t    *dsx1;
    int                slot;
    uint               type;
    paktype            *pak;
    dsx1info           *info;
    mcn_instance_t     *ds_mcn;
    short              temp;
    static             firsttime = TRUE;

    process_wait_on_system_init();

    while (TRUE) {
	process_sleep_periodic(250L);
	
	if (systeminit_flag) {
	    FOR_ALL_CDBS(cdb) {
		slot = cdb->slot;
		dsx1 = DSX1_INSTANCE(slot, cdb->subunit);
                ds_mcn = MCN_INSTANCE(slot);
                info = dsx1->dsx1_ds;
		type = dsx1->appliquetype;

		if (type == MCN_PA_T1) {
                   if (firsttime) {
                      t1_wr_neat_mem(slot, DSX1_RELOAD_DONE, 0xabcd);
                      firsttime = FALSE;
                   }
		    /*
                     * We do T1 carrier status update every 250ms and
                     * T1 error statistics and performance every second.
                     * The one second update messages from Neat processor
                     * initially are queued to smallest packet buffers and
                     * are processed here all at one time.
		     */
                    t1_carrier_update(dsx1);
                    t1_lp_print(dsx1);
		    FORWARD_COUNTER(dsx1->one_sec_timer, FRAMER_TIME);
		    if (COUNTER_EXPIRED(dsx1->one_sec_timer,
					FRAMER_TIMES_PER_SEC)){
			CLEAR_COUNTER(dsx1->one_sec_timer);

/* This is ugly part for level 6 interrupt re-enable problem. Need to
   remove this tempory fix after the permant solution is found */

     temp = *((ushort *)ADRSPC_BRASIL_MB_CTRL1_REG);
     temp |=AS5200_LEVEL6_EN;
     *((ushort *)ADRSPC_BRASIL_MB_CTRL1_REG) = temp;

                       /*
                        * Increment one second and check if alive_count is
                        * equal to or more than 10 seconds. This way we can
                        * monitor NEAT processor is running or not
                        */
                        dsx1->alive_count +=1;
                           if (dsx1->alive_count >= 10) {
                               dsx1->alive_count = 0;
                               if (serial_debug) {
                                   errmsg(&msgsym(FIRMWARE, CONTROLLER),
                                       info->cdb->cdb_namestring,
                                       "firmware is not running");
                                   buginf("\nThe pri_status is %x\n",
                                          ds_mcn->mcn_regs->pri_status);
                                }
                            }
                        while ((pak = pak_dequeue(&t1_sec_updateQ)) != NULL) {
                                dsx1 = (dsx1_instance_t *)pak->rx_descriptor;
                                if (dsx1 != NULL)
			            t1_sec_update(dsx1, pak);
                                retbuffer(pak);
                         }
                     }
             } else if (type == MCN_PA_E1) {
                 if (firsttime) {
                    e1_wr_neat_mem(slot, DSX1_RELOAD_DONE, 0xabcd);
                    firsttime = FALSE;
                 }
	       /*
                * Start frame process when 250ms expired. The frame
                * process algorithm will take care of any error occurred
                * after power up or during running time
	        */
                e1_carrier_update(dsx1, cdb->subunit);
	        FORWARD_COUNTER(dsx1->one_sec_timer, FRAMER_TIME);
	        if (COUNTER_EXPIRED(dsx1->one_sec_timer,
	        			FRAMER_TIMES_PER_SEC)){
	   	   CLEAR_COUNTER(dsx1->one_sec_timer);

                   /*
		    * This is ugly part for level 6 interrupt re-enable problem. 
                    * Need to remove this tempory fix after the permant solution 
                    * is found 
                    */
     		   temp = *((ushort *)ADRSPC_BRASIL_MB_CTRL1_REG);
     	      	   temp |=AS5200_LEVEL6_EN;
     		   *((ushort *)ADRSPC_BRASIL_MB_CTRL1_REG) = temp;

                  /*
            	   * Increment one second and check if alive_count is
            	   * equal to or more than 10 seconds. This way we can
               	   * monitor NIM processor is running or not
                   */
                   dsx1->alive_count +=1;
                   if (dsx1->alive_count >= 10) {
                       dsx1->alive_count = 0;
                       if (serial_debug)
                       errmsg(&msgsym(FIRMWARE, CONTROLLER),
                                       info->cdb->cdb_namestring,
                                       "firmware is not running");
                   }
                   while ((pak = pak_dequeue(&e1_sec_updateQ)) != NULL) {
                 	 dsx1 = (dsx1_instance_t *)pak->rx_descriptor;
                 	if (dsx1 != NULL)
			   e1_sec_update(dsx1, cdb->subunit, pak);
                 	retbuffer(pak);
                   }
                }
	      }
	   }
	}
    }
}

void dsx1_set_a_law (boolean sense, dsx1info *info)
{
	dsx1_instance_t    *dsx1;
	int                slot;
	uint               type;

    slot = info->cdb->slot;
    dsx1 = DSX1_INSTANCE(slot, info->cdb->subunit);
	type = dsx1->appliquetype;
	if (type == MCN_PA_T1) {
		if (sense)
    		t1_wr_neat_mem(slot, NEAT_LEDS, BOARD_OK);
		else
    		t1_wr_neat_mem(slot, NEAT_LEDS, (BOARD_OK | COMPAND));
	}
}

/*
 * =====================================================================
 * framer_send_ais -
 *
 * Description:
 * Send AIS depends on DSX1 controller "shut down" state
 *
 * Parameters:
 * cdbtype
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void framer_send_ais (cdbtype *cdb, boolean flag)
{
    int              slot;
	uint 			type;
    dsx1_instance_t  *dsx1;
    dsx1MIBDataTable *mib;
    dsx1info         *info;

    
    slot = cdb->slot;
    dsx1 = DSX1_INSTANCE(slot, cdb->subunit);
	type = dsx1->appliquetype;

    info = dsx1->dsx1_ds;
    mib = &info->MIB;

    if (flag == TRUE) {
        framer_set_MIB_ls(&mib->C.dsx1LineStatus, DSX1_TX_AIS);
    } else {
        framer_clr_MIB_ls(&mib->C.dsx1LineStatus, DSX1_TX_AIS); 
    }

	if (type == MCN_PA_E1) {
		e1_txAIS(slot, cdb->subunit, flag);
	} else if (type == MCN_PA_T1) {
    	t1_txAIS(slot, cdb->subunit, flag);
	}
}

/*
 * =====================================================================
 * framer_set_MIB_ls -
 *
 * Description:
 * Set dsx1LineStatus in MIB data structure
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void framer_set_MIB_ls (int *stptr, ushort mask)
{
    *stptr |= mask;
    *stptr &= ~DSX1_NO_ALARM;
}

/*
 * =====================================================================
 * framer_clr_MIB_ls -
 *
 * Description:
 * Reset dsx1LineStatus in MIB data structure
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void framer_clr_MIB_ls (int *stptr, ushort mask)
{
    *stptr &= ~mask;
    
    if (( *stptr & DSX1_HAVE_ERRORS ) == 0 )
        *stptr |= DSX1_NO_ALARM;
}

/*
 * =====================================================================
 * framer_clr_dsx1table -
 *
 * Description:
 * Clear MIB table
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void framer_clr_dsx1table ( dsx1TableType *p )
{
    p->dsx1ESs = 0;
    p->dsx1SESs = 0;
    p->dsx1SEFSs = 0;
    p->dsx1UASs = 0;
    p->dsx1CSSs = 0;
    p->dsx1PCVs = 0;
    p->dsx1LESs = 0;
    p->dsx1BESs = 0;
    p->dsx1DMs = 0;
    p->dsx1LCVs = 0;
}
/*
 * =====================================================================
 * carrier_interrupt -
 *
 * Description:
 * Level 6 interrupt from NEAT processor for carrier state changed. 
 * Since there is only one level 6 interrupt line in backplane shared by 
 * all three slots, the interrupt routine need to figure out which slot the 
 * T1 card is in, so the code can process the interrupt. 
 *
 * Parameters:
 * slot #.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void carrier_interrupt (void)
{
    mcn_instance_t     *ds;
    uint               slot;

    slot = get_slot_number();
    ds = MCN_INSTANCE(slot);

    /*
     * The level 6 interrupt from Neat processor includes carrier state
     * changed
     */
    switch( ds->mcn_cardtype ) {

      case MCN_PA_T1:
        t1_l6_interrupt( slot );
        break;
      case MCN_PA_E1:
        e1_l6_interrupt( slot );
        break;

      default:                /* empty or we don't know about it */
        break;
    }
}

/*
 * =====================================================================
 * framer_interrupt -
 *
 * Description:
 * Level 4 interrupt from NEAT processor
 *
 * Parameters:
 * slot #.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void framer_interrupt (uint slot)
{
    mcn_instance_t     *ds;

    ds = MCN_INSTANCE(slot);
    /*
     * The level 4 interrupt from Neat processor includes one second 
     * update message, remote loopback test result and DTMF phone
     * digits collection status.
     */
    
    switch( ds->mcn_cardtype ) {
	
      case MCN_PA_T1:
	t1_l4_interrupt( slot );
	break;
      case MCN_PA_E1:
	e1_l4_interrupt( slot );
	break;
	
      default:                /* empty or we don't know about it */
	break;
    }
}

/*
 * =====================================================================
 * framer_set_framing -
 *
 * Description:
 * Set the frame type for the applique
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void framer_set_framing (dsx1info *info)
{
    short            frametype;
    uint             slot, subunit;
    dsx1MIBDataTable *mib;
    
    mib = &info->MIB;
    slot = info->cdb->slot;
    subunit = info->cdb->subunit;
    frametype = info->framing;
    
    switch (info->applique_type) {
	case APPL_T1_CSU:
        if ( frametype != DSX1_FRM_ESF && frametype != DSX1_FRM_SF )
            return;
        subunit = info->cdb->subunit;
		t1_framing(slot, subunit, frametype);
		break;
	case APPL_E1_UNBAL:
	case APPL_E1_BAL:
		if (frametype != DSX1_FRM_E1 && frametype != DSX1_FRM_E1_CRC)
			return;
		e1_framing(slot, subunit, frametype);
		e1_homologation_type(slot, subunit, info->homologation_type);
		break;
	default:
		break;
	}

    mib->C.dsx1LineType = frametype;
}

/*
 * =====================================================================
 * framer_set_linecode -
 *
 * Description:
 * Set the line code depending on the type of applique
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void framer_set_linecode (dsx1info *info)
{
    short            codetype;
    uint             slot, subunit;
    dsx1MIBDataTable *mib;
    
    mib = &info->MIB;
    slot = info->cdb->slot;
    subunit = info->cdb->subunit;
    codetype = info->linecode;
    
    switch (info->applique_type) {
	case APPL_T1_CSU:
        if ( codetype != DSX1_LC_B8ZS && codetype != DSX1_LC_AMI )
            return;
        t1_linecode(slot, subunit, codetype);
		break;
	case APPL_E1_UNBAL:
	case APPL_E1_BAL:
		if (codetype != DSX1_LC_AMI && codetype != DSX1_LC_HDB3)
			return;
		e1_linecode(slot, subunit, codetype);
		break;
	default:
		break;
	}
    mib->C.dsx1LineCoding = codetype;
}
/*
 * =====================================================================
 * show_as5200_nim_hw_ver -
 *
 * Description:
 * Show as5200 t1/e1 card hardware version numbers
 *
 * Parameters:
 * cdbtype *cdb
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */

static void show_as5200_nim_hw_ver (cdbtype *cdb)
{
    ushort slot;
    short neat_pld_reg_data;
    short nr_bus_reg_data;
    ushort type;

    slot = cdb->slot;
    type = mcn_get_adapter_type(cdb->slot);

    if (type == MCN_PA_T1) {
        /* neat_pld rev number */
        neat_pld_reg_data = t1_rd_neat_mem(slot, NEAT_PLD_REV);

        /* nr_bus_pld rev number */
        nr_bus_reg_data = BRASIL_T1_BOARD_NR_BUS_PLD_REV(slot);

        printf("\n  Version info of slot %d:  HW: %d, Firmware: %d, "
               "NEAT PLD: %d, NR Bus PLD: %d", slot, nim_get_version(slot),
               t1_version, neat_pld_reg_data, nr_bus_reg_data);
    } else if (type == MCN_PA_E1) {
        /* neat_pld rev number */
        neat_pld_reg_data = t1_rd_neat_mem(slot, NEAT_PLD_REV);

        printf("\n  Version info of Slot %d:  HW: %d, Firmware: %d, "
               "NEAT PLD: %d", slot, 
               nim_get_version(slot), e1_version, neat_pld_reg_data);
    } else {
        printf("\n Wrong board type for T1/E1 version info.");
    }

}

/*
 * =====================================================================
 * framer_set_fdl -
 *
 * Description:
 * Set the fdl standard for dsx1 data link
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void framer_set_fdl (dsx1info *info)
{
    short            fdltype;
    uint             slot, subunit;
    dsx1MIBDataTable *mib;
   
    mib = &info->MIB;
    slot = info->cdb->slot;
    subunit = info->cdb->subunit;
    fdltype = info->fdl;
   
    t1_fdl(slot, subunit, fdltype);

    mib->C.dsx1Fdl = fdltype;
}

/*
 * =====================================================================
 * framer_set_cablelength -
 *
 * Description:
 * Set the cable length for T1 applique but ignored for E1 aplique
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void framer_set_cablelength (dsx1info *info)
{
    short lengthtype;
    uint  subunit;

    lengthtype = info->length;

    if (info->applique_type == APPL_T1_CSU) {
        if ( lengthtype != DSX1_SHORT && lengthtype != DSX1_LONG )
            return;
        subunit = info->cdb->subunit;
        t1_cablelength(info, subunit, lengthtype);
    }
}

/*
 * =====================================================================
 * dsx1_loopback_clk_switch -
 *
 * Description:
 * Dispatch service; calling the proper E1/T1 statemachine for controlling 
 * the DSX1 line clock source selection during loopback.
 *
 * Parameters:
 * dsx1info ptr, event type.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void dsx1_loopback_clk_switch(dsx1info *info, uchar event)
{
    uint  subunit;
    
    subunit = info->cdb->subunit;

    switch (info->applique_type) {
        case APPL_T1_CSU:
                t1_loopback_clk_switch(info, subunit, event);
                break;
        case APPL_E1_UNBAL:
        case APPL_E1_BAL:
                e1_loopback_clk_switch(info, subunit, event);
                break;
        default:
                break;
    }

}

/*
 * =====================================================================
 * framer_set_clock_source -
 *
 * Description:
 * Set the clock source for T1 applique but ignored for E1 aplique
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void framer_set_clock_source (dsx1info *info)
{
    short clocktype;
    uint  subunit;
    
    clocktype = info->clocking;
    subunit = info->cdb->subunit;
    
	switch (info->applique_type) {
	case APPL_T1_CSU:
        if (( clocktype >= DSX1_CLK_LINE )
            && ( clocktype <= DSX1_CLK_FREE )) {
            dsx1_loopback_clk_switch(info, DSX1_USER_CONFIG_EVENT);
            t1_clocksource( info, subunit, clocktype );
        } else
            return;
		break;
	case APPL_E1_UNBAL:
	case APPL_E1_BAL:
        if (( clocktype >= DSX1_CLK_LINE )
            && ( clocktype <= DSX1_CLK_FREE )) {
            dsx1_loopback_clk_switch(info, DSX1_USER_CONFIG_EVENT);
            e1_clocksource( info, subunit, clocktype );
        } else
            return;
		break;
	default:
		break;
	}
}

/*
 * =====================================================================
 * framer_set_loopback -
 *
 * Description:
 * Set or reset loopback and indicate this information in dsx1ConfigTable 
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void framer_set_loopback (dsx1info *info)
{
    uint slot, subunit;
    dsx1MIBDataTable *mib;
    dsx1_instance_t *dsx1;
    m32_instance_t   *m32_ds;
    
    mib = &info->MIB;
    slot = info->cdb->slot;
    subunit = info->cdb->subunit;
    dsx1 = DSX1_INSTANCE(slot, subunit);
    m32_ds = M32_INSTANCE(slot, subunit);    
    dsx1 = DSX1_INSTANCE(slot, subunit);
    
	switch (info->applique_type) {
	case APPL_T1_CSU:
            switch (info->loop) {
            case DSX1_LOOP_LOCAL:
                reset_if(info->hwidb[T1_ISDN_D_CHANNEL]);
                t1_local_up(slot, subunit);
                errmsg(&msgsym(FIRMWARE, CONTROLLER),
                        info->cdb->cdb_namestring,
                        "LOCAL loopback request from user command");
                mib->C.dsx1LoopbackConfig = DSX1_OTHER_LOOP;
		framer_set_MIB_ls(&mib->C.dsx1LineStatus, DSX1_LOOPED);
                break;
            case DSX1_LOOP_REMOTE:
                t1_loopup_code(slot, subunit);
                mib->C.dsx1LoopbackConfig = DSX1_OTHER_LOOP;
                break;
            default:
                if (dsx1->runts_recovery) {
                    m32_ds->no_auto_check = TRUE;
                    m32_ds->en_runts_check = TRUE;
                    dsx1->runts_recovery = FALSE;
                    m32_ds->idb_runts =0x0;
                }
                t1_no_loop(slot, subunit);
                mib->C.dsx1LoopbackConfig = DSX1_NO_LOOP;
    		if (dsx1->loopback_type == LOCAL_LOOP) {
		   framer_clr_MIB_ls(&mib->C.dsx1LineStatus, DSX1_LOOPED);
                   errmsg(&msgsym(FIRMWARE, CONTROLLER),
                          info->cdb->cdb_namestring,
                          "LOCAL loopback release from user command");
                }
                break;
            }
            break;
	case APPL_E1_UNBAL:
	case APPL_E1_BAL:
	    switch (info->loop) {
	    case DSX1_LOOP_LOCAL:
	        reset_if(info->hwidb[E1_ISDN_D_CHANNEL]);
		e1_local_up(slot, subunit);
                errmsg(&msgsym(FIRMWARE, CONTROLLER),
                        info->cdb->cdb_namestring,
                        "LOCAL loopback request from user command");
		mib->C.dsx1LoopbackConfig = DSX1_OTHER_LOOP;
		framer_set_MIB_ls(&mib->C.dsx1LineStatus, DSX1_LOOPED);
		break;
            case DSX1_LOOP_REMOTE:
		/* Do nothing here */
		break;
	    default:
                if (dsx1->runts_recovery) {
                    m32_ds->no_auto_check = TRUE;
                    m32_ds->en_runts_check = TRUE;
                    dsx1->runts_recovery = FALSE;
                    m32_ds->idb_runts =0x0;
                }
		e1_no_loop(slot, subunit);
                errmsg(&msgsym(FIRMWARE, CONTROLLER),
                       info->cdb->cdb_namestring,
                       "LOCAL loopback release from user command");
		mib->C.dsx1LoopbackConfig = DSX1_NO_LOOP;
		framer_clr_MIB_ls(&mib->C.dsx1LineStatus, DSX1_LOOPED);
		break;
	    }
	    break;
	default:
	    break;
	}
}

/*
 * =====================================================================
 * dsx1_set_loop -
 *
 * Description:
 * After excessive errors went away, if there was a loopback command
 * pending, we need to finish the command
 *
 * Parameters:
 * hwidb ptr
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void dsx1_set_loop (hwidbtype *idb)
{
    dsx1info         *info;
    dsx1_instance_t  *dsx1;
    dsx1MIBDataTable *mib;

    dsx1 = DSX1_INSTANCE(idb->slot, idb->subcont);
    info = dsx1->dsx1_ds;
    mib = &info->MIB;
    switch (info->applique_type) {
        case APPL_T1_CSU:
              t1_local_up(idb->slot, idb->subcont);
              break;
        case APPL_E1_UNBAL:
        case APPL_E1_BAL:
              e1_local_up(idb->slot, idb->subcont);
              break;
        default:
              break;
        }
    info->loop = DSX1_LOOP_LOCAL;
    mib->C.dsx1LoopbackConfig = DSX1_OTHER_LOOP;
    framer_set_MIB_ls(&mib->C.dsx1LineStatus, DSX1_LOOPED);
}

/*
 * =====================================================================
 * dsx1_set_noloop -
 *
 * Description:
 * Background process in every minute to put no loopback on problematic
 * T1. So we can re-examine the excessive errors from M32 still exist
 * or not
 *
 * Parameters:
 * hwidb ptr
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void dsx1_set_noloop (hwidbtype *idb)
{
    dsx1info         *info;
    dsx1_instance_t  *dsx1;
    dsx1MIBDataTable *mib;


    dsx1 = DSX1_INSTANCE(idb->slot, idb->subcont);
    info = dsx1->dsx1_ds;
    mib = &info->MIB;

    switch (info->applique_type) {
        case APPL_T1_CSU:
              t1_no_loop(idb->slot, idb->subcont);
              break;
        case APPL_E1_UNBAL:
        case APPL_E1_BAL:
              e1_no_loop(idb->slot, idb->subcont);
              break;
        default:
              break;
        }

    info->loop = DSX1_LOOP_NONE;
    mib->C.dsx1LoopbackConfig = DSX1_NO_LOOP;
    framer_clr_MIB_ls(&mib->C.dsx1LineStatus, DSX1_LOOPED);
}

/*
 * =====================================================================
 * dsx1_set_runts_handle -
 *
 * Description:
 * The reason to do this is intense errors interrupt from Munich32 uses
 * a lot of of CPU bandwidth and cause all calls on the other DSX1 to drop.
 * In order to prevent this happened, the troubled DSX1 will be put in local
 * loopback mode until auto-recovery to correct the errors and pull the
 * DSX1 out of loopback mode
 *
 * Parameters:
 * hwidb ptr
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void dsx1_set_runts_handle (hwidbtype *idb)
{
    dsx1info         *info;
    dsx1_instance_t  *dsx1;
    dsx1MIBDataTable *mib;


    dsx1 = DSX1_INSTANCE(idb->slot, idb->subcont);
    info = dsx1->dsx1_ds;
    mib = &info->MIB;

    switch (info->applique_type) {
        case APPL_T1_CSU:
                t1_local_up(idb->slot, idb->subcont);
                break;
        case APPL_E1_UNBAL:
        case APPL_E1_BAL:
                e1_local_up(idb->slot, idb->subcont);
                break;
        default:
                break;
        }
    info->loop = DSX1_LOOP_LOCAL;
    mib->C.dsx1LoopbackConfig = DSX1_OTHER_LOOP;
    framer_set_MIB_ls(&mib->C.dsx1LineStatus, DSX1_LOOPED);
    if (!dsx1->runts_recovery) {
       dsx1->m32_errmsg_print = TRUE;
       dsx1->runts_recovery = TRUE;
    }
}

/*
 * =====================================================================
 * framer_reg_add -
 *
 * Description:
 * Add framer specific functions registry
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void framer_reg_add ()
{
    reg_add_dsx1_set_loopback(framer_set_loopback, "framer_set_loopback");
    reg_add_dsx1_set_clock_source(framer_set_clock_source,
				  "framer_set_clock_source");
    reg_add_dsx1_set_framing(framer_set_framing, "framer_set_framing");
    reg_add_dsx1_set_linecode(framer_set_linecode, "framer_set_linecode");
    reg_add_dsx1_set_fdl(framer_set_fdl, "framer_set_fdl");
    reg_add_dsx1_set_cablelength(framer_set_cablelength, 
                                    "framer_set_cablelength");
    reg_add_dsx1_set_a_law(dsx1_set_a_law, "dsx1_set_a_law");
    reg_add_show_nim_hw_ver(show_as5200_nim_hw_ver, 
                            "show_as5200_nim_hw_ver");
    reg_add_dsx1_noloop(dsx1_set_noloop, "dsx1_set_noloop");
    reg_add_dsx1_loop(dsx1_set_loop, "dsx1_set_loop");
    reg_add_dsx1_loopback_clk_switch(dsx1_loopback_clk_switch,
				     "dsx1_loopback_clk_switch");
}

