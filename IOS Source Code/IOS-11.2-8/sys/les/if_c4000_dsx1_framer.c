/* $Id: if_c4000_dsx1_framer.c,v 3.3.62.4 1996/09/07 22:37:11 jliu Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_dsx1_framer.c,v $
 *------------------------------------------------------------------
 * if_c4000_dsx1_framer.c - Common framer functions for any E1/T1 interface. 
 *
 * Nov 27 1994, Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_dsx1_framer.c,v $
 * Revision 3.3.62.4  1996/09/07  22:37:11  jliu
 * CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
 * Branch: California_branch
 *     o The fix included display warning message to console port and used
 *       SNMP notification to send the message to network management
 *       station
 *
 * Revision 3.3.62.3  1996/08/28  12:57:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.2  1996/06/16  21:15:33  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.62.1  1996/03/18  20:41:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:23:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  17:39:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:23  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/09/09  01:36:53  jliu
 * CSCdi39941:  loopback LED on CT1 nim always on under normal operation
 * Put extern queuetype to .h file instead
 *
 * Revision 2.8  1995/09/08  05:44:27  jliu
 * CSCdi39941:  loopback LED on CT1 nim always on under normal operation
 *
 * Revision 2.7  1995/07/07  22:58:23  wmay
 * CSCdi36810:  CE1 nim does not pass Austalian Homologation
 *
 * Revision 2.6  1995/06/17  00:54:48  jliu
 * CSCdi35899:  Controller didnt come back up after shut/no shut
 *
 * Revision 2.5  1995/06/09  21:47:22  jliu
 * CSCdi35546:  One second update message may lose under heavy loading
 *
 * Revision 2.4  1995/06/09  17:43:52  jliu
 * CSCdi35546:  One second update message may lose under heavy loading
 *
 * Revision 2.3  1995/06/09  13:11:24  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.2  1995/06/08 22:05:46  jliu
 * CSCdi35359:  Rename files for new hardware changes
 *
 * Revision 2.1  1995/06/07  21:34:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../if/if_controller.h"
#include "les_controller.h"
#include "if_c4000_mcn.h"
#include "../if/dsx1.h"
#include "if_c4000_dsx1_framer.h"
#include "if_c4000_ce1.h"
#include "if_c4000_bt8360.h"
#include "if_c4000_dsx1.h"
#include "../if/dsx1_registry.h"
#include "packet.h"
#include "../if/if_msg_controller.c"	/* Not a typo, see logger.h */


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
    

    while (TRUE) {
	process_sleep_periodic(250L);
	
	if (systeminit_flag) {
	    FOR_ALL_CDBS(cdb) {
		slot = cdb->slot;
		dsx1 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
                info = dsx1->dsx1_ds;
		type = dsx1->appliquetype;
		if (type == MCN_PA_E1) {
		    /*
                     * Start frame process when 250ms expired. The frame
                     * process algorithm will take care of any error occurred
                     * after power up or during running time
		     */
                    ce1_carrier_update(dsx1);
		    FORWARD_COUNTER(dsx1->one_sec_timer, FRAMER_TIME);
		    if (COUNTER_EXPIRED(dsx1->one_sec_timer,
					FRAMER_TIMES_PER_SEC)){
			CLEAR_COUNTER(dsx1->one_sec_timer);

                       /*
                        * Increment one second and check if alive_count is
                        * equal to or more than 10 seconds. This way we can
                        * monitor NIM processor is running or not
                        */
                        dsx1->mpu->alive_count +=1;
                           if (dsx1->mpu->alive_count >= 10) {
                               dsx1->mpu->alive_count = 0;
                               if (serial_debug)
                                   errmsg(&msgsym(FIRMWARE, CONTROLLER),
                                       info->cdb->cdb_namestring,
                                       "firmware is not running");
                                }
                        while ((pak = pak_dequeue(&ce1_sec_updateQ)) != NULL) {
                                dsx1 = (dsx1_instance_t *)pak->rx_descriptor;
                                if (dsx1 != NULL)
			            ce1_sec_update(dsx1, pak);
                                retbuffer(pak);
                         }
                     }
		} else if (type == MCN_PA_T1) {
		    
		    /*
                     * Start frame process when 250ms expired. The frame
                     * process algorithm will take care of any error occurred
                     * after power up or during running time
		     */
		    bt8360_frame_process(dsx1);
		    FORWARD_COUNTER(dsx1->one_sec_timer, FRAMER_TIME);
		    if (COUNTER_EXPIRED(dsx1->one_sec_timer,
					FRAMER_TIMES_PER_SEC)){
			CLEAR_COUNTER(dsx1->one_sec_timer);
			bt8360_sec_update(dsx1);
		    }
		}
	    }
	}
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
    int slot;
    uint type;
    dsx1_instance_t  *dsx1;
    dsx1MIBDataTable *mib;
    dsx1info         *info;

    
    slot = cdb->slot;
    dsx1 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    type = dsx1->appliquetype;
    info = dsx1->dsx1_ds;
    mib = &info->MIB;

    if (flag == TRUE) {
        framer_set_MIB_ls(&mib->C.dsx1LineStatus, DSX1_TX_AIS);
    } else {
        framer_clr_MIB_ls(&mib->C.dsx1LineStatus, DSX1_TX_AIS); 
    }
    if (type == MCN_PA_E1) {
        ce1_txAIS(slot, flag);
    } else if (type == MCN_PA_T1) {
        bt8360_txAIS(slot, flag);
    }
}

/*
 * =====================================================================
 * framer_led_on -
 *
 * Description:
 * Turn on LED depends on mask parameter
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void framer_led_on (dsx1_instance_t *dsx1, ushort mask)
{
    *dsx1->ext_led &= (~mask);
}

/*
 * =====================================================================
 * framer_led_off -
 *
 * Description:
 * Turn off LED depends on mask parameter
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void framer_led_off (dsx1_instance_t *dsx1, ushort mask)
{
    *dsx1->ext_led |= mask;
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
 * framer_send_remalarm -
 *
 * Description:
 * Set/clear the remote alarm, and all the flags referring to it
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void framer_send_remalarm (dsx1_instance_t *dsx1, 
			   boolean         flag)
{
    volatile short   *framer;
    dsx1MIBDataTable *mib;
    dsx1info         *info;
    
    info = dsx1->dsx1_ds;
    mib = &info->MIB;
    framer = dsx1->framer;
    if (flag == FALSE) {
        if (dsx1->txing_rem_alarm) {
            /* Turn off Remote alarm */
            framer_clr_MIB_ls(&mib->C.dsx1LineStatus, DSX1_TX_REM_ALARM);
            dsx1->txing_rem_alarm = FALSE;
            if (dsx1->framer_remote_alarm) {
                (*dsx1->framer_remote_alarm)(framer, flag);
            }
        }
    } else {
        if (!(dsx1->txing_rem_alarm)) {
            /* Turn on remote alarm */
            framer_set_MIB_ls(&mib->C.dsx1LineStatus, DSX1_TX_REM_ALARM);
            dsx1->txing_rem_alarm = TRUE;
            if (dsx1->framer_remote_alarm) {
                (*dsx1->framer_remote_alarm)(framer, flag);
            }
        }
    }
}

/*
 * =====================================================================
 * framer_frame_loss -
 *
 * Description:
 * Indicates that we have lost frame
 *
 * Parameters:
 * Pointers of the DSX1 data structure instance and info.
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void framer_frame_loss (dsx1_instance_t *dsx1, dsx1info *info) 
{
    dsx1MIBDataTable *mib;
    cdbtype          *cdb;
    
    mib = &info->MIB;
    cdb = info->cdb;
    
    if ( dsx1->frame_acquired ) {
        dsx1->frame_loss_occurred = TRUE;
    }
    /*
     * Note: with channels defined, putting the send remote-alarm
     * here might screw up the time for the E1 to come back
     * in 500 usec.  Might want to move it after DisableAllM32Channels
     */
    framer_send_remalarm(dsx1, TRUE);
    if ( dsx1->has_frame ) {
        dsx1->has_frame = FALSE;
	
        CLEAR_COUNTER(dsx1->count_until_frame);
        CLEAR_COUNTER(dsx1->count_until_loss_clear);
        if (dsx1->rxing_rem_alarm) {
        dsx1->rxing_rem_alarm = FALSE;
        framer_led_off( dsx1, DSX1_LED_REMALARM );
        framer_clr_MIB_ls( &mib->C.dsx1LineStatus, DSX1_RX_REM_ALARM );
        }
        framer_led_on( dsx1, DSX1_LED_LOCALARM );
        framer_set_MIB_ls( &mib->C.dsx1LineStatus, DSX1_RX_LOF );
	
        if ( dsx1->available ) {
            dsx1->available = FALSE;
	    
            if (cdb->cdb_state != IDBS_ADMINDOWN) {
                controller_state_change(cdb, IDBS_DOWN);
            }
        }
    }
} 

/*
 * =====================================================================
 * framer_check_available -
 *
 * Description:
 * The DSX1 controller may become available so enable all M32 channels
 *
 * Parameters:
 * pointer of DSX1 data structure instance.
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
void framer_check_available (dsx1_instance_t *dsx1)
{
    dsx1info    *info;
    cdbtype     *cdb;
    dsx1MIBDataTable *ee;
    
    info = dsx1->dsx1_ds;
    cdb = info->cdb;
    ee = &info->MIB;
    
    if (( dsx1->unavailable_state == FALSE )
        && ( dsx1->rxing_rem_alarm == FALSE )) {
	dsx1->available = TRUE;
	
        if (cdb->cdb_state == IDBS_DOWN) {
            controller_state_change(cdb, IDBS_UP);
            dsx1_create_idle_channel(info);
	    framer_clr_MIB_ls(&ee->C.dsx1LineStatus, DSX1_RX_LOF);
        }
    }
}

/*
 * =====================================================================
 * framer_interrupt -
 *
 * Description:
 * Level 4 interrupt either from E1/T1 framer or 125us tick timer
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
    dsx1_instance_t *dsx1;
    
    dsx1 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    
    /*
     * Run fast update only if it is E1 applique and frame loss
     * In normal condition, the framer_fast_update should be null
     * Since we re-design new hardware to have a dedicate microprocessor
     * to handle all E1 frame process issues, so framer_fast_update is
     * not needed any more
     */
    
    switch( dsx1->appliquetype ) {
	
      case MCN_PA_T1:
	bt8360_interrupt( dsx1 );
	break;
	
      case MCN_PA_E1:
	ce1_interrupt( dsx1 );
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
    short frametype;
    uint slot;
    dsx1MIBDataTable *mib;
    
    mib = &info->MIB;
    slot = info->cdb->slot;
    frametype = info->framing;
    
    switch (info->applique_type) {
    case APPL_T1:
        if ( frametype != DSX1_FRM_ESF && frametype != DSX1_FRM_SF )
            return;
	bt8360_framing(slot, frametype);
        break;
    case APPL_E1_UNBAL:
    case APPL_E1_BAL:
        if (frametype != DSX1_FRM_E1 && frametype != DSX1_FRM_E1_CRC)
            return;
	ce1_framing(slot, frametype);
	ce1_homologation_type(slot, info->homologation_type);
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
    short codetype;
    uint slot;
    dsx1MIBDataTable *mib;
    
    mib = &info->MIB;
    slot = info->cdb->slot;
    codetype = info->linecode;
    
    switch (info->applique_type) {
    case APPL_T1:
        if ( codetype != DSX1_LC_B8ZS && codetype != DSX1_LC_AMI )
            return;
        bt8360_linecode(slot, codetype);
        break;
    case APPL_E1_UNBAL:
    case APPL_E1_BAL:
        if (codetype != DSX1_LC_AMI && codetype != DSX1_LC_HDB3)
            return;
        ce1_linecode(slot, codetype);
        break;
      default:
        break;
    }
    mib->C.dsx1LineCoding = codetype;
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
    uint slot;
    
    slot = info->cdb->slot;
    clocktype = info->clocking;
    
    switch (info->applique_type) {
    case APPL_T1:
        if (( clocktype >= DSX1_CLK_LINE )
            && ( clocktype <= DSX1_CLK_INTERNAL )) {
            bt8360_clocksource( slot, clocktype );
        } else
            return;
        break;
      case APPL_E1_UNBAL:
      case APPL_E1_BAL:
        /*
         * We don't need to set a clock source for the channelized E1
         * applique - the transceiver does it for us
         */
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
    uint slot;
    dsx1MIBDataTable *mib;
    
    mib = &info->MIB;
    slot = info->cdb->slot;
    
    switch (info->applique_type) {
      case APPL_T1:
        switch (info->loop) {
        case DSX1_LOOP_LOCAL:
           bt8360_local_up(slot);
           mib->C.dsx1LoopbackConfig = DSX1_LOOP_LOCAL;
              break;
        case DSX1_LOOP_REMOTE:
           bt8360_loopup_code(slot);
           mib->C.dsx1LoopbackConfig = DSX1_LOOP_REMOTE;
              break;
        default:
           bt8360_no_loop(slot);
           mib->C.dsx1LoopbackConfig = DSX1_LOOP_NONE;
              break;
           }
        break;
      case APPL_E1_UNBAL:
      case APPL_E1_BAL:
        switch (info->loop) {
        case DSX1_LOOP_LOCAL:
           ce1_local_up(slot);
           mib->C.dsx1LoopbackConfig = DSX1_LOOP_LOCAL;
              break;
        default:
           ce1_no_loop(slot);
           mib->C.dsx1LoopbackConfig = DSX1_LOOP_NONE;
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
              bt8360_local_up(idb->slot);
              break;
        case APPL_E1_UNBAL:
        case APPL_E1_BAL:
              ce1_local_up(idb->slot);
              break;
        default:
              break;
        }
    info->loop = DSX1_LOOP_LOCAL;
    mib->C.dsx1LoopbackConfig = DSX1_LOOP_LOCAL;
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
              bt8360_no_loop(idb->slot);
              break;
        case APPL_E1_UNBAL:
        case APPL_E1_BAL:
              ce1_no_loop(idb->slot);
              break;
        default:
              break;
        }

    info->loop = DSX1_LOOP_NONE;
    mib->C.dsx1LoopbackConfig = DSX1_LOOP_NONE;
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
                bt8360_local_up(idb->slot);
                break;
        case APPL_E1_UNBAL:
        case APPL_E1_BAL:
                ce1_local_up(idb->slot);
                break;
        default:
                break;
        }
    info->loop = DSX1_LOOP_LOCAL;
    mib->C.dsx1LoopbackConfig = DSX1_LOOP_LOCAL;
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
    reg_add_dsx1_noloop(dsx1_set_noloop, "dsx1_set_noloop");
    reg_add_dsx1_loop(dsx1_set_loop, "dsx1_set_loop");
}

/*
 * =====================================================================
 * framer_uas -
 *
 * Description:
 * Uas will handle the MIB interpretation of the unavailable state.
 * It will handle all the processing of the data into available and
 * unavailable seconds.  It will also disable the applique if we
 * go into unavailable state.
 * It will also handle processing the data into the correct 15 minute bucket.
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void framer_uas (dsx1_instance_t *dsx1,
		 dsx1info *info,
		 ushort tmpBPV,
		 ushort tmpCRC,
		 boolean EsOccurred,
		 boolean BESOccurred,
		 boolean SESOccurred,
		 dsx1TableType *errptr)
{
    int cnt, interval;
    dsx1TableType *old, *new;
    dsx1MIBDataTable *mib;
    cdbtype          *cdb;
    
    cdb = info->cdb;
    mib = &info->MIB;
    /*
     * Now we check for unavailable / available states 
     */
    if ( dsx1->unavailable_state ) {
	/*
	 * Unavailable - we must have 10 non-SES seconds to become
	 * available again
	 */
	errptr->dsx1UASs++;
	if ( SESOccurred == FALSE ) {
	    dsx1->ses_count++;
	    if ( dsx1->ses_count >= 10 ) {
		dsx1->ses_count = 0;
		dsx1->unavailable_state = FALSE;
		
		framer_send_remalarm(dsx1, FALSE);
		framer_led_off(dsx1, DSX1_LED_LOCALARM);
		framer_check_available (dsx1);
		
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
                    old->dsx1ESs += dsx1->es_last_interval;
                    old->dsx1BESs += dsx1->bes_last_interval;
                    errptr->dsx1UASs = 0;
                }
                errptr->dsx1ESs += dsx1->es_in_failed;
                errptr->dsx1BESs += dsx1->bes_in_failed;
		
	    } else {
		if ( EsOccurred ) dsx1->es_in_failed++;
		if ( BESOccurred ) dsx1->bes_in_failed++;
	    }
	} else {
	    dsx1->ses_count = 0;
	    dsx1->bes_in_failed = 0;
	    dsx1->es_in_failed = 0;
	    dsx1->es_last_interval = 0;
	    dsx1->bes_last_interval = 0;
	}
    } else {
	/*
	 * Available - 10 SES in a row, we become unavailable
	 */
	if ( SESOccurred == FALSE ) {
	    dsx1->ses_count = 0;
	    if ( EsOccurred ) {
		errptr->dsx1ESs++;
	    }
	    if ( BESOccurred ) {
		errptr->dsx1BESs++;
	    }
	    /*
	     * Now see if we have a degraded minute
	     */
	    dsx1->secs_for_min++;
	    ADD_TO_GAUGE( dsx1->bpv_in_min, tmpBPV );
	    ADD_TO_GAUGE( dsx1->crc_in_min, tmpCRC );
	    if ( dsx1->secs_for_min >= 60 ) {
		if (( dsx1->crc_in_min > dsx1->crcneeded_in_min )
		    || ( dsx1->bpv_in_min > dsx1->bpvneeded_in_min )) {
		    errptr->dsx1DMs++;
		}
		dsx1->secs_for_min = 0;
		dsx1->crc_in_min = 0;
		dsx1->bpv_in_min = 0;
	    }
            if ((cdb->cdb_state == IDBS_DOWN)
                 && ( dsx1->rxing_rem_alarm == FALSE )) {
                 controller_state_change(cdb, IDBS_UP);
                 dsx1_create_idle_channel(info);
             }

	} else {
	    errptr->dsx1SESs++;
	    dsx1->ses_count++;
	    if ( dsx1->ses_count >= 10 ) {
		dsx1->ses_count = 0;
		if ( dsx1->available ) {
		    /* We haven't lost frame - but have too many errors - 
		     * disable the M32 channels
		     */
		    dsx1->available = FALSE;
                    /* Turn on remote alarm */
		    framer_send_remalarm(dsx1, TRUE );
		    framer_led_on( dsx1, DSX1_LED_LOCALARM );
                    if (cdb->cdb_state == IDBS_UP) {
                        controller_state_change(cdb, IDBS_DOWN);
                    }
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
		dsx1->unavailable_state = TRUE;
		dsx1->es_in_failed = 0;
		dsx1->es_last_interval = 0;
		dsx1->bes_in_failed = 0;
		dsx1->bes_last_interval = 0;
	    } 
	}
    }
    
    mib->C.dsx1TimeElapsed++;
    if ( mib->C.dsx1TimeElapsed >= 900 ) {
        if (dsx1->unavailable_state) {
            dsx1->bes_last_interval = dsx1->bes_in_failed;
            dsx1->bes_in_failed = 0;
            dsx1->es_last_interval = dsx1->es_in_failed;
            dsx1->es_in_failed = 0;
        }
	mib->C.dsx1TimeElapsed = 0;
	mib->C.dsx1ValidIntervals++;
	if (mib->C.dsx1ValidIntervals > QUARTER_HOURS_IN_DAY)
	    mib->C.dsx1ValidIntervals = QUARTER_HOURS_IN_DAY;
	
	mib->CurrentData.dsx1Interval++;
	if (mib->CurrentData.dsx1Interval >= 96)
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
        old->dsx1DMs = new->dsx1DMs ;
	
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



