/* $Id: if_c4000_ce1.c,v 3.3.62.4 1996/09/05 17:57:29 wmay Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c4000_ce1.c,v $
 *------------------------------------------------------------------
 * if_c4000_ce1.c - C4XXX E1 framer modules. 
 *
 * Nov 27 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c4000_ce1.c,v $
 * Revision 3.3.62.4  1996/09/05  17:57:29  wmay
 * CSCdi68168:  ce1 firmware data is in data segment - move to text
 * Branch: California_branch
 *
 * Revision 3.3.62.3  1996/08/28  12:57:48  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.2  1996/06/16  21:15:20  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.62.1  1996/03/18  20:41:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:23:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  17:39:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:58:11  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/07/07  22:58:21  wmay
 * CSCdi36810:  CE1 nim does not pass Austalian Homologation
 *
 * Revision 2.7  1995/06/27  00:11:06  wmay
 * CSCdi36414:  CE1 framer wont frame on 0s in timeslot 16 - remove unused
 * command
 *
 * Revision 2.6  1995/06/23  18:48:19  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.5  1995/06/21  08:55:19  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.4  1995/06/19  06:47:15  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/09  17:43:48  jliu
 * CSCdi35546:  One second update message may lose under heavy loading
 *
 * Revision 2.2  1995/06/08  22:05:40  jliu
 * CSCdi35359:  Rename files for new hardware changes
 *
 * Revision 2.1  1995/06/07  21:33:58  hampton
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
#include "../if/dsx1.h"
#include "if_c4000_mcn.h"
#include "if_c4000_m32.h"
#include "if_c4000_dsx1.h"
#include "if_c4000_dsx1_framer.h"
#include "if_c4000_ce1.h"
#include "packet.h"
#include "../if/if_msg_controller.c"	/* Not a typo, see logger.h */

queuetype ce1_sec_updateQ;

/*
 * =====================================================================
 * ce1_wr_nymph_ram -
 *
 * Description:
 * Write one byte data to NIM processor's ram memory
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void ce1_wr_nymph_ram (dsx1_instance_t *ce1, int mail_addr, short data)
{
        short page, index;
        mpu_instance_t *mpu_info;

        mpu_info = ce1->mpu;
        index = (ushort) 0xff & mail_addr;
        page = (ushort) 0xff & (mail_addr >> 8);
        *mpu_info->page_ram = page;
        *mpu_info->index_ram = index;
        *mpu_info->data_ram = data;
}

/*
 * =====================================================================
 * ce1_rd_nymph_ram -
 *
 * Description:
 * Read one byte data from NIM processor's ram memory
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static short ce1_rd_nymph_ram (dsx1_instance_t *ce1, int mail_addr)
{
        short page, index, data = 0;
        mpu_instance_t *mpu_info;

        mpu_info = ce1->mpu;
        index = (ushort) 0xff & mail_addr;
        page = (ushort) 0xff & (mail_addr >> 8);
        *mpu_info->page_ram = page;
        *mpu_info->index_ram = index;
        data = *mpu_info->data_ram;
        return(data);
}

/*
 * =====================================================================
 * ce1_startup -
 *
 * Description:
 * This function is called by analyze_ce1 as a one time initialization
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
static void ce1_startup (uint slot, uint type, uint subcont)
{
    dsx1_instance_t *ce1;
    mcn_instance_t *ds_mcn;

    ds_mcn = MCN_INSTANCE(slot);        /* get the mcn instance */

    /*
     * Allocate  memory for the channelized E1 instance.
     */
    ce1 = malloc_fast(sizeof(dsx1_instance_t));
    if (!ce1) {
        crashdump(0);
    }

    /*
     * Store the ce1 instance pointer in the MCN instance.
     * Henceforth, access the instance using DSX1_INSTANCE(slot, subcont).
     */
    ds_mcn->mcn_subcont[subcont].info.dsx1 = ce1;

    /*
     * register level 4 interrupt for this slot, to handle NIM
     * processor interrupts
     */
    nim_register_hi_irq_handler((ushort) slot, framer_interrupt,\
                             (ushort) IDBTYPE_E1, (ushort) slot);
 
    ce1->appliquetype = type;
}

/*
 * =====================================================================
 * ce1_download_firmware -
 *
 * Description:
 * This function is called for downloading ce1 firmware
 *
 * Parameters:
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
static void ce1_download_firmware (dsx1_instance_t *ce1, 
				   char *data_ptr,
				   short total_bytes)
{
    int addr=0, index;

    for (index=0; index < total_bytes; index++) {
         ce1_wr_nymph_ram(ce1, addr, (short) *data_ptr++);
         addr +=1;
    }
}

/*
 * =====================================================================
 * ce1_init -
 *
 * Description:
 * Initialize ce1 data structure
 *
 * Parameters:
 * slot:                The slot #.
 *
 * Returns:
 * nothing.
 *
 * =====================================================================
 */
static void ce1_init (dsx1_instance_t *ce1, int slot)
{
    dsx1MIBDataTable *mib;

    mib = &ce1->dsx1_ds->MIB;

    /*
     * the calculation below is for 1 x 10-6 errors in 60 seconds.  A
     * minute is declared a "degraded minute" if we get this many of
     * the given type of error in it.
     */
    ce1->bpvneeded_in_min = (2048000 * 60) / 1000000;
    ce1->pcvneeded_in_min = (2048000 * 60) / 1000000;

    /* until found or framed */
    mib->C.dsx1LineType = DSX1_OTHER_FRAME;
    mib->C.dsx1LineCoding = DSX1_LC_HDB3;
    mib->C.dsx1LoopbackConfig = DSX1_LOOP_NONE;

    /*
     * The ce1_sec_updateQ is used to store one second full message from
     * NIM processor. When main processor is too busy under heavy loading
     * it probably won't be able to do one second update background process
     * on schedule. In order to avoid losting full message from NIM 
     * processor, put it on process queue for process later is one way to
     * solve this problem.
     */
    queue_init(&ce1_sec_updateQ, 0);

    /*
     * Turn on local alarm but turn off remote alarm LED
     */
    *ce1->mpu->ext_led |= DSX1_LED_REMALARM;
    *ce1->mpu->ext_led &= ~(DSX1_LED_LOCALARM);

    /*Turn off local and remote loopback LED*/
    *ce1->mpu->ext_led |= DSX1_LED_RLOOP;

    framer_set_MIB_ls(&mib->C.dsx1LineStatus, DSX1_RX_LOF);
    /*
     * Bring the NIM processor out of the reset state and enable force MPU
     * interrupt from NIM processor.
     */
    *ce1->mpu->np_control &= ~(CE1_NP_RESET);
    *ce1->mpu->np_control |= CE1_NP_INT_ENABLE;
}

/*
 * =====================================================================
 * ce1_analyze_mcn_port_adaptor -
 *
 * Description:
 * Initialize the E1  port adaptor in a given slot
 *
 * Parameters:
 * slot.
 *
 * Returns:
 * none.
 * 
 * =====================================================================
 */
void ce1_analyze_mcn_port_adaptor (int slot, int subcont, int total_ctrlr_unit)
{
    uint        type, slot_addr;
    ushort      temp;
    dsx1info    *ce1_info;
    dsx1_instance_t *ce1;
    mpu_instance_t *mpu_t;

    type = mcn_get_adapter_type(slot);
    slot_addr = (uint)GET_SLOT_BASE(slot);
    /*
     * Initialize the E1 instance.
     */
    ce1_startup( slot, type , subcont);

    ce1 = DSX1_INSTANCE(slot, subcont);

    mpu_t = malloc_fast(sizeof(mpu_instance_t));
    if (!mpu_t) {
	crashdump(0);
    }

    ce1->mpu = mpu_t;

    /*
     * set the base of the MPU structure for this slot
     * to get to the adapter registers.
     */
    ce1->mpu->np_control = (volatile short *)(slot_addr +
					      (uint)CE1_NP_CONTROL);
    ce1->mpu->page_ram = (volatile short *)(slot_addr + (uint)CE1_PAGE_OFFSET);
    ce1->mpu->index_ram = (volatile short *)(slot_addr +
					     (uint)CE1_INDEX_OFFSET);
    ce1->mpu->data_ram = (volatile ushort *)(slot_addr +
					     (uint)CE1_DATA_OFFSET);
    ce1->mpu->ext_led = (volatile ushort *)(slot_addr +
					    (uint)CE1_LED_OFFSET);

    temp = ce1_rd_nymph_ram(ce1, CE1_CABLE_TYPE);
    /*
     * Init Dsx1.
     */
    if (temp & UNBALANCE) {
        ce1_info = dsx1_init_context(MCN_PA_E1, PIFT_E1, 
                                    APPL_E1_UNBAL, slot, subcont, total_ctrlr_unit);
    } else {
        ce1_info = dsx1_init_context(MCN_PA_E1, PIFT_E1,
                                    APPL_E1_BAL, slot, subcont, total_ctrlr_unit);
    }
    ce1->dsx1_ds = ce1_info;

    /*
     * Add the E1 framer Dsx1 registries
     */
    framer_reg_add();	
    ce1_download_firmware(ce1, (char *)ce1_firmware,
                                     (short)ce1_firmware_bytes);
    ce1_init(ce1, slot);
}

/*
 * =====================================================================
 * ce1_mail_to_np -
 *
 * Description:
 * Common routine to send command to NIM processor
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
static void ce1_mail_to_np (dsx1_instance_t *ce1, ushort command, boolean set)
{
    ushort      value;

    value =  ce1_rd_nymph_ram(ce1, CE1_MAIL_TO_NP);

    if (set) {
        value |= command;
        ce1_wr_nymph_ram(ce1, CE1_MAIL_TO_NP, value);
    } else {
        value &= ~(command);
        ce1_wr_nymph_ram(ce1, CE1_MAIL_TO_NP, value);
    }
}

/*
 * =====================================================================
 * ce1_framing -
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
void ce1_framing (uint slot, short frametype)
{
    boolean         set;
    dsx1_instance_t *ce1;
   
    ce1 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);

    if (frametype != DSX1_FRM_E1 && frametype != DSX1_FRM_E1_CRC)
            return;

    if (ce1->frame_type != frametype) {
        ce1->frame_type = frametype;
        
        set = (frametype == DSX1_FRM_E1) ? TRUE : FALSE;
        ce1_mail_to_np(ce1, MPU_MSG_FRAME, set);
    }
}

/*
 * =====================================================================
 * ce1_linecode -
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
void ce1_linecode ( uint slot, short codetype )
{
    boolean         set;
    dsx1_instance_t *ce1;
   
    ce1 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);

    if (codetype != DSX1_LC_AMI && codetype != DSX1_LC_HDB3)
            return;

    set = (codetype == DSX1_LC_AMI) ? TRUE : FALSE;

    ce1_mail_to_np(ce1, MPU_MSG_LINECODE, set);
}


/*
 * =====================================================================
 * ce1_local_up -
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
void ce1_local_up ( uint slot )
{
    dsx1_instance_t *ce1;

    ce1 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);

    ce1_mail_to_np(ce1, MPU_MSG_LOOPBACK, TRUE);

    /*
     * Turn on remote loopback LED since it does loopback at framer
     * and transceiver chip
     */
    *ce1->mpu->ext_led &= ~DSX1_LED_RLOOP;
}

/*
 * =====================================================================
 * ce1_no_loop -
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
void ce1_no_loop ( uint slot )
{
    dsx1_instance_t *ce1;

    ce1 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);

    ce1_mail_to_np(ce1, MPU_MSG_LOOPBACK, FALSE);

    /*Turn off local and remote loopback LED*/
    *ce1->mpu->ext_led |= DSX1_LED_RLOOP;
}


/*
 * =====================================================================
 * ce1_txAIS -
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
void ce1_txAIS ( uint slot, boolean set )
{
    dsx1_instance_t *ce1;

    ce1 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);

    ce1_mail_to_np(ce1, MPU_MSG_DISABLED, set);

}

/*
 * ce1_force_remote_alarm
 * Forces an RAI signal out the transmitter.  For Australian Homologation
 */
static void ce1_force_remote_alarm (dsx1_instance_t *ce1, boolean set)
{
    ce1_mail_to_np(ce1, MPU_MSG_SEND_RAI, set);
}

/*
 * ce1_homologation_type
 * Will indicate the homologation type (right now, just Australia has
 * differences).  It will also turn on the RAI signal if we are
 * unavailable if we are in Australia (as per the spec)
 */
void ce1_homologation_type (uint slot, enum dsx1_homologation_type type)
{
    dsx1_instance_t *ce1;

    ce1 = DSX1_INSTANCE(slot, SINGLE_SUBCONT);
    ce1_mail_to_np(ce1, MPU_MSG_HOMOL_TYPE, (type == DSX1_HOMOL_AUSTRALIA));
    if ((type == DSX1_HOMOL_AUSTRALIA) &&
	(ce1->unavailable_state == TRUE)) {
	ce1_force_remote_alarm(ce1, TRUE);
    } else {
	ce1_force_remote_alarm(ce1, FALSE);
    }
}

/*
 * =====================================================================
 * ce1_carrier_update -
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
void ce1_carrier_update ( dsx1_instance_t *ce1 )
{
    dsx1info    *info;
    cdbtype     *cdb_d;

         info = ce1->dsx1_ds;
         cdb_d = info->cdb;


         if (ce1->mpu->carrier_status) {
         /*
          * Before we bring up controller, make sure it isn't in shutdown
          * or unavailable state
          */
              if ((ce1->unavailable_state == FALSE) && 
                  (cdb_d->cdb_state != IDBS_ADMINDOWN)) {
                    if (cdb_d->cdb_state != IDBS_UP) { 
                        errmsg(&msgsym(UPDOWN, CONTROLLER),
                               cdb_d->cdb_namestring, print_cdbstate(IDBS_UP));

                        cdb_d->cdb_state = IDBS_UP;
                        dsx1_create_idle_channel(info);
                        dsx1_change_all_idbs_state(cdb_d, IDBS_UP);
                        *ce1->mpu->ext_led |= DSX1_LED_LOCALARM;
                    }
              }
         } else {
              if (cdb_d->cdb_state == IDBS_UP) {
                  errmsg(&msgsym(UPDOWN, CONTROLLER),
                      cdb_d->cdb_namestring, print_cdbstate(IDBS_DOWN));

                  cdb_d->cdb_state = IDBS_DOWN;
                  dsx1_change_all_idbs_state(cdb_d, IDBS_DOWN);
                  *ce1->mpu->ext_led &= ~(DSX1_LED_LOCALARM);
              }
         }
}

/*
 * =====================================================================
 * ce1_interrupt -
 *
 * Description:
 * E1 NIM processor interrupt routine
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void ce1_interrupt ( dsx1_instance_t *ce1 )
{
    ushort src, cause, tempread;
    dsx1info            *info;
    dsx1MIBDataTable    *ee;
    paktype             *pak;
    uchar               *mib_info;
    
    info = ce1->dsx1_ds;
    ee = &info->MIB;

    /*
     * Read NIM processor control register to clear interrupt from NIM
     * processor
     */
    cause = *ce1->mpu->np_control; 

    if (cause & CE1_MPU_INT)  {
    /*
     * Read message byte from NIM processor's ram to determinate which message 
     * type
     */
       src = ce1_rd_nymph_ram(ce1, CE1_MAIL_FROM_NP);

       switch (src) {
          case MSG_TYPE_CARRIER:
            /*
             * Update carrier state according to carrier available byte from 
             * NIM processor's ram. Clear those two bytes after read them.
             */
             tempread = ce1_rd_nymph_ram(ce1, CE1_CARRIER_AVAIL);
             ce1->mpu->carrier_status = tempread;
             ce1_wr_nymph_ram(ce1, CE1_CARRIER_AVAIL, 0);

            /*
             * read board status and update it to dsx1LineStatus to reflect
             * the latest one.
             */
             tempread = ce1_rd_nymph_ram(ce1, CE1_BOARD_STATUS);
             ee->C.dsx1LineStatus = tempread;

            /*
             * if receive remote alarm then turn on remote alarm LED
             */
             if (tempread & DSX1_RX_REM_ALARM) {
                 *ce1->mpu->ext_led &= ~(DSX1_LED_REMALARM);
             } else {
                 *ce1->mpu->ext_led |= DSX1_LED_REMALARM;
             }

             break;


          case MSG_TYPE_FULL_MESSAGE:
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
             ce1->mpu->alive_count = 0;

            /*
             * Update carrier state according to carrier available byte from 
             * NIM processor's ram. Clear those two bytes after read them.
             */
             tempread = ce1_rd_nymph_ram(ce1, CE1_CARRIER_AVAIL);
             ce1->mpu->carrier_status = tempread;
             ce1_wr_nymph_ram(ce1, CE1_CARRIER_AVAIL, 0);

            /*
             * Read board status and update them to DSX1 line status in 
             * configuration table
             */
             tempread = ce1_rd_nymph_ram(ce1, CE1_BOARD_STATUS);
             ee->C.dsx1LineStatus = tempread;

            /*
             * if receive remote alarm then turn on remote alarm LED
             */
             if (tempread & DSX1_RX_REM_ALARM) {
                 *ce1->mpu->ext_led &= ~(DSX1_LED_REMALARM);
             } else {
                 *ce1->mpu->ext_led |= DSX1_LED_REMALARM;
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

             *mib_info++ = ce1_rd_nymph_ram(ce1, CE1_OTHER_STATUS);
             *mib_info++ = ce1_rd_nymph_ram(ce1,BPV_COUNT);
             *mib_info++ = ce1_rd_nymph_ram(ce1,BPV_COUNT+1);
             *mib_info++ = ce1_rd_nymph_ram(ce1,PCV_COUNT);
             *mib_info++ = ce1_rd_nymph_ram(ce1,PCV_COUNT+1);
             *mib_info++ = ce1_rd_nymph_ram(ce1,CRC_COUNT);
             *mib_info++ = ce1_rd_nymph_ram(ce1,CRC_COUNT+1);
             *mib_info++ = ce1_rd_nymph_ram(ce1,FBE_COUNT);
             *mib_info++ = ce1_rd_nymph_ram(ce1,FBE_COUNT+1);

            /*
             * Store dsx1 data structure pointer to rx_descriptor. so when
             * one second update background process is called, the code
             * will pass the right pointer if more than one CE1 nim are
             * put in the system
             */
             pak->rx_descriptor = ce1;

             if (pak_enqueue(&ce1_sec_updateQ, pak) == NULL) {
                 retbuffer(pak);
                }

             break;
         
        default:
        }
        /*
         * clear message byte after read it, so make it available for NIM 
         * processor to send next message 
         */
        ce1_wr_nymph_ram(ce1, CE1_MAIL_FROM_NP, 0);
    }
}

/*
 * =====================================================================
 * ce1_sec_update -
 *
 * Description:
 * E1 receive errors second update
 *
 * Parameters:
 * none.
 *
 * Returns:
 * none.
 *
 * =====================================================================
 */
void ce1_sec_update ( dsx1_instance_t *ce1, paktype *pak )
{
    int              cnt, interval;
    dsx1info         *info;
    ushort           other_status; 
    ushort           bpv_count, pcv_count, crc_count, fbe_count;
    dsx1TableType    *errptr, *old, *new;
    dsx1MIBDataTable *mib;
    mpu_instance_t   *mpu_info;
    cdbtype          *cdb_d;
    uchar            *mib_info;

    info = ce1->dsx1_ds;
    cdb_d = info->cdb;
    mpu_info = ce1->mpu;
    mib = &info->MIB;
    errptr = &mib->CurrentData;


    /*
     * get the pointer which points to the message we want to process
     */
    mib_info = pak->datagramstart;
    /*
     * retrieve message from ce1_sec_updateQ
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
    if ( ce1->unavailable_state ) {
        /*
         * Unavailable - we must have 10 non-SES seconds to become
         * available again
         */
        errptr->dsx1UASs++;
        if (!(other_status & MSG_SES_OCCURRED)) {
            ce1->ses_count++;
            if ( ce1->ses_count >= 10 ) {
                ce1->ses_count = 0;
                ce1->unavailable_state = FALSE;
                *ce1->mpu->ext_led |= DSX1_LED_LOCALARM;

		if (info->homologation_type == DSX1_HOMOL_AUSTRALIA) {
		    ce1_force_remote_alarm(ce1, FALSE);
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
                    old->dsx1ESs += ce1->es_last_interval;
                    old->dsx1BESs += ce1->bes_last_interval;
                    errptr->dsx1UASs = 0;
                }
                errptr->dsx1ESs += ce1->es_in_failed;
                errptr->dsx1BESs += ce1->bes_in_failed;

            } else {
                if ( other_status & MSG_ES_OCCURRED ) 
                     ce1->es_in_failed++;
                if ( other_status & MSG_BES_OCCURRED ) 
                     ce1->bes_in_failed++;
            }
        } else {
            ce1->ses_count = 0;
            ce1->bes_in_failed = 0;
            ce1->es_in_failed = 0;
            ce1->es_last_interval = 0;
            ce1->bes_last_interval = 0;
        }
    } else {
        /*
         * Available - 10 SES in a row, we become unavailable
         */
        if (!(other_status & MSG_SES_OCCURRED)) {
            ce1->ses_count = 0;
            if ( other_status & MSG_ES_OCCURRED ) {
                errptr->dsx1ESs++;
            }
            if ( other_status & MSG_BES_OCCURRED ) {
                errptr->dsx1BESs++;
            }
            /*
             * Now see if we have a degraded minute
             */
            ce1->secs_for_min++;
            ADD_TO_GAUGE( ce1->bpv_in_min, bpv_count );
            ADD_TO_GAUGE( ce1->pcv_in_min, pcv_count );
            bpv_count = 0;
            pcv_count = 0;
            if ( ce1->secs_for_min >= 60 ) {
                if (( ce1->pcv_in_min > ce1->pcvneeded_in_min )
                    || ( ce1->bpv_in_min > ce1->bpvneeded_in_min )) {
                    errptr->dsx1DMs++;
                }
                ce1->secs_for_min = 0;
                ce1->pcv_in_min = 0;
                ce1->bpv_in_min = 0;
            }
        } else {
            errptr->dsx1SESs++;
            ce1->ses_count++;
            if ( ce1->ses_count >= 10 ) {
                ce1->ses_count = 0;
		/* We haven't lost frame - but have too many errors -
		 * disable the M32 channels
		 */

		if (info->homologation_type == DSX1_HOMOL_AUSTRALIA) {
		    /* In Australia, force the RAI signal */
		    ce1_force_remote_alarm(ce1, TRUE);
		}

		*ce1->mpu->ext_led &= ~(DSX1_LED_LOCALARM);
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
                ce1->unavailable_state = TRUE;
                ce1->es_in_failed = 0;
                ce1->es_last_interval = 0;
                ce1->bes_in_failed = 0;
                ce1->bes_last_interval = 0;
            }
        }
    }

    mib->C.dsx1TimeElapsed++;
    /*
     * Check if 15 minutes passed then we need to update total MIB table
     */
    if ( mib->C.dsx1TimeElapsed >= 900 ) {
        if (ce1->unavailable_state) {
            ce1->bes_last_interval = ce1->bes_in_failed;
            ce1->bes_in_failed = 0;
            ce1->es_last_interval = ce1->es_in_failed;
            ce1->es_in_failed = 0;
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


