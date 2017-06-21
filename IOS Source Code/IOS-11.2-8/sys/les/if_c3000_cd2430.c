/* $Id: if_c3000_cd2430.c,v 3.1.2.5 1996/08/28 12:57:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_cd2430.c,v $
 *------------------------------------------------------------------
 * if_c3000_cd2430.c - Common code for the synchronous and
 *                     asynchronous interface drivers for the Cirrus
 *                     CL-CD2430 controller (for c3000 only)
 *
 * May 1996, Jose Hernandez
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_cd2430.c,v $
 * Revision 3.1.2.5  1996/08/28  12:57:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.4  1996/07/05  19:31:40  jhernand
 * CSCdi61429:  Modify Cirrus Logic CD2430 initialization by adding Timeout
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/07/04  02:07:03  jhernand
 * CSCdi61640:  Cobra code does not conform to Cisco guidelines
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/26  19:45:59  etrehus
 * CSCdi58973:  2430 Subsystem initialized on inappropriate hardware
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/06/17  08:33:32  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/05/13  18:56:35  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "logger.h"
#include "../les/msg_cd2430.c"		/* Not a typo, see logger.h */
#include "stacks.h"
#include "subsys.h"
#include "../if/network.h"
#include "../les/if_les.h"
#include "../les/if_les_common_cd2430.h"
#include "../dev/file_io.h"
#include "../../boot/cpu.h"

int cd2430_controllers;         /* Number of CD2430 chips in system */
int cd2430ports;                /* Number of CD2430 ports in system */
int cd2430_istate;              /* Non-zero if in interrupt state */
int cd2430Base;                 /* Starting line # for CD2430's */
uint cd2430clock;                /* Clock applied to chips */
ushort lss_misc;                 /* Misc bits describing async options */
uchar initial_gfrcr[CD2430MAX];  /* Keep chip rev level */

/*
 * The cd2430_platform_type variable is used to keep track of the platform
 * on which this driver is running.
 */
int cd2430_platform_type;

/*
 * Set the Channel Access Register to point to a specific register.
 * This function assumes CD2430_CHIPCHAN is a power of 2 which should
 * always be true.
 */
inline void setchan_async (cd2430_quadart *reg_table, tt_soc *tty)
{
    uchar chan;

    if (reg_table->stk) {
        errmsg(&msgsym(SETCHAN,CIRRUS), tty->ttynum);
    }
    chan = (tty->ttynum - cd2430Base) & (CD2430_CHIPCHAN-1);
    /*
     * Set the CAR to appropriate channel number.
     */
    reg_table->car = chan;
}

/*
 * Set the Channel Access Register to point to a specific register.
 * This function assumes CD2430_CHIPCHAN is a power of 2 which should
 * always be true.
 */
inline void setchan_sync (cd2430_quadart *reg_table, hwidbtype *idb)
{
    uchar chan;

    if (reg_table->stk) {
        errmsg(&msgsym(SETCHAN,CIRRUS), idb->unit);
    }
    chan = (idb->unit - cd2430Base) & (CD2430_CHIPCHAN-1);
    /*
     * Set the CAR to appropriate channel number.
     */
    reg_table->car = chan;
}

/*
 * cd2430_wait_reset_complete()
 *      Wait until hardware reset is complete.
 * This function is called to wait for hardware reset completion.
 * This is determined by a non-zero value in the Global Firmware
 * Revision Code Register (GFRCR). A timeout value is also set
 * upon entry and if the CD2430 fails to reset before the timer
 * expires, an error message is posted.
 */
void cd2430_wait_reset_complete(cd2430_quadart *reg_table, int ctrl)
{
    sys_timestamp reset_timer;
    boolean reset_flag;

    TIMER_START(reset_timer, CD2430_RESET_TIMEOUT);
    reset_flag = FALSE;
    while (!reset_flag)
      {    reset_flag = (reg_table->gfrcr != 0);
         if (!reset_flag) {
           if (AWAKE(reset_timer)) {
             reset_flag = TRUE;
             errmsg(&msgsym(RESETERR,CIRRUS), ctrl);
           }
         }
      }
}

/*
 * cd2430_controller_init()
 * This function initializes the global context of the controller.
 * Depending on the platform (Copan or Cobra), this function is
 * called from the async initialization code or the sync initialization
 * code.
 */
void cd2430_controller_init (int ctrl)
{
    int i, line, vector;
    cd2430_quadart *reg_table = NULL;
    uchar *cd2402_flash_firmware = NULL;
    uchar *firmware;

    /*
     * Fill in the interrupt service vectors
     */
    for (i = 0; i < CD2430VectorCount; i++) {
        vector = i + (ctrl * CD2430VectorCount);
        if (cd2430_vectors[vector])
          exceptionHandler(CD2430Vector + vector, cd2430_vectors[vector]);
    }

    /*
     * Get the address of the register table for this controller.
     */
    switch (cd2430_platform_type) {
    case PLATFORM_COPAN:
        reg_table = CD2430ADDRESS(ctrl);
        break;
    case PLATFORM_COBRA:
        reg_table = CD2430ADDRESS_COBRA(ctrl);
        break;
    default:
        crashdump(0);
    }

    cd2430_reg_table[ctrl] = reg_table;
    cd2430_wait_reset_complete(reg_table, ctrl);
    
    /*
     * The initial value of the gfrcr is the chip rev level.
     * Save it to be printed in show controller. 
     */
    initial_gfrcr[ctrl] = reg_table->gfrcr;

    /*
     * If it's a 2402, download it now
     */
    if ((lss_misc & LSS_CHIPVER) == LSS_2402) {
        /*
         * Check to see if new firmware file is present in flash. If it
         * does, then that firmware file will be used and the firmware
         * linked in with the system image will be overridden.
         */
        cd2402_flash_firmware = flash_get_firmware();

        if (cd2402_flash_firmware) /* Firmware file was present in flash */
            firmware = cd2402_flash_firmware;
        else                       /* Linked in firmware will be used */
            firmware = cd2402_firmware;

        /*
         * Disable the bit engines
         */
        for (line = 0; line < CD2430_CHIPCHAN; line++) {
            reg_table->car = line;
            reg_table->tbpr = 0;
            reg_table->rbpr = 0;
        }

        /*
         * Dump new code to it
         */
        cd2402_download(reg_table, firmware);

        /*
         * Wait a bit for gfrcr to clear (reset started on the chip).
         * Max delay 1/8 sec, in case we missed the entire reset.
         */
        for (i = 0; i < 25; i++) {
            if (reg_table->gfrcr == 0) {
                break;
            }
            DELAY(5);
        }
    }

    /*
     * Wait for chip to be ready; reset it, and wait for reset
     * to complete by checking for a non-zero firmware revision
     */
    cd2430_wait_reset_complete(reg_table, ctrl);
    sendccr(reg_table, CCR_RESET);
    cd2430_wait_reset_complete(reg_table, ctrl);

    /*
     * If firmware file from flash was used, free the memory malloced to
     * store the flash firmware array.
     */
    if (cd2402_flash_firmware)
        free(cd2402_flash_firmware);

    /*
     * Set priority levels
     */
    reg_table->mpilr = CD2430PILR(CD2430M_DISABLE);
    reg_table->tpilr = CD2430PILR(CD2430T_DISABLE);
    reg_table->rpilr = CD2430PILR(CD2430R_DISABLE);
}


/*
 * Subsystem Initialization
 */
static void cd2430_subsys_init (subsystype *subsys)
{
    /*
     * If not a Cancun, can't have Copan asynch, eh?
     */
    if (cpu_type != CPU_CANCUN) {
	return;
    }

    /*
     * Determine if we are a COBRA or COPAN, otherwise do not initialize
     * this subsystem.
     */

    switch (cookie->interfaces) {
    case INTERFACE_1E2T8A:
    case INTERFACE_1R2T8A:
    case INTERFACE_1E2T:
    case INTERFACE_1E1T:
    case INTERFACE_1R2T: 
        if ( !touch_device((void *)ADRSPC_LSS_MISC) ||
             (cookie->vendor == VENDOR_CHIPCOM) ) {
	  return;
        }
	cd2430_platform_type = PLATFORM_COPAN;
        break;
    case INTERFACE_1E2T1B2AS:
    case INTERFACE_1R2T1B2AS: 
        cd2430_platform_type = PLATFORM_COBRA;
        break;
    default:
	return;
    }

    /*
     * Do a software initiated hard reset of the CD2430 controller.
     * This is needed because the rxboot still does not have the
     * code to do this for us automatically.
     */
    if (cd2430_platform_type == PLATFORM_COBRA) {
        (*(ushort *)ADRSPC_LSS_RESET_ASEL) &= DEVICE_LSS_RESET_MASK;
        DELAY(5);
        (*(ushort *)ADRSPC_LSS_RESET_ASEL) |= ~DEVICE_LSS_RESET_MASK;
    }

    /*
     * Create interrupt stacks.  Modem stack is usually created by the
     * sync driver, whose level we share.
     */
    createlevel(CD2430T_DISABLE, NULL, "CL-CD2430 transmit interrupts");
    createlevel(CD2430R_DISABLE, NULL, "CL-CD2430 receive interrupts");
    if (!level_exists(CD2430M_DISABLE)) {
        createlevel(CD2430M_DISABLE, NULL, "CL-CD2430 modem interrupts");
    }
}

/*
 * CD2430 subsystem header
 * Note the sequence requirement which requires that the Hitachi subsystem
 * and the flash subsystem be initialized before this subsystem is 
 * initialized. This is needed because we want the Hitachi serial interfaces
 * to be the lower ordinal serial interfaces than the Cirrus serial 
 * interfaces. The requirement with the flash device subsystem is present
 * because the init code searches the flash for the possible presence of
 * a CD2402 firmware file and this requires that flash be initialized.
 */
#define CD2430_MAJVERSION   1
#define CD2430_MINVERSION   0
#define CD2430_EDITVERSION  1
SUBSYS_HEADER(cd2430, CD2430_MAJVERSION, CD2430_MINVERSION,
        CD2430_EDITVERSION, cd2430_subsys_init, SUBSYS_CLASS_DRIVER,
        "seq: hd64570, flash_devices",
        NULL);




