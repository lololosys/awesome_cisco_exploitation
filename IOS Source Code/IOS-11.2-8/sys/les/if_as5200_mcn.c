/* $Id: if_as5200_mcn.c,v 3.1.64.3 1996/09/07 22:37:00 jliu Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_mcn.c,v $
 *------------------------------------------------------------------
 * if_as5200_mcn.c -- The Multi channel NIM specific code for the as52xx.
 *
 * December, 1995 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_mcn.c,v $
 * Revision 3.1.64.3  1996/09/07  22:37:00  jliu
 * CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
 * Branch: California_branch
 *     o The fix included display warning message to console port and used
 *       SNMP notification to send the message to network management
 *       station
 *
 * Revision 3.1.64.2  1996/08/28  12:57:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.64.1  1996/06/16  21:14:48  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/19  23:47:59  jchlin
 * Placeholders for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "subsys.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "if_les.h"
#include "if_as5200_mcn.h"
#include "if_c4000_m32.h"
#include "../os/timer_generic.h"

/*
 * Global declarations
 */

/*
 * array of pointers to mcn instance structures.
 */
mcn_instance_t *mcn_ds[AS5200_MAX_SLOTS];


/*
 * =====================================================================
 * mcn_get_adapter_type -
 *
 * Description:
 * Get the port adapter type.
 *
 * Parameters:
 * slot:	slot # of 4000
 *
 * Returns:
 * adapter_type
 * =====================================================================
 */
uint mcn_get_adapter_type (uint slot)
{
    switch(BRASIL_BOARD_ID(slot)) {
    case BRASIL_T1_DPRI_PLUS_CSU:
         return MCN_PA_T1;
 
    case BRASIL_E1_DPRI_PLUS_MODEMS:
         return MCN_PA_E1;
 
    default:                /* empty or we don't know about it */
         return 0;
    }
}

static ushort mcn_get_total_subcont (int slot) 
{
    switch(BRASIL_BOARD_ID(slot)) {
    case BRASIL_T1_DPRI_PLUS_CSU:
    case BRASIL_E1_DPRI_PLUS_MODEMS:
         return 2;          /* dual T1/E1 board */

    default:                /* empty or we don't know about it */
         return 0;
    }
}

static int mcn_get_digit_collector_num (int slot)
{
    switch(BRASIL_BOARD_ID(slot)) {
    case BRASIL_T1_DPRI_PLUS_CSU:
    case BRASIL_E1_DPRI_PLUS_MODEMS:
         return 2;          /* dual T1 or E1 board has two digit collectors */
 
    default:                /* empty */
         return 0;
    }
}


/*
 * =====================================================================
 * mcn_analyze_interface -
 *
 * Description:
 * The general MCN startup routine which updates the
 * mcn_instance and set appropriate ptr to nim regs and counter regs.
 * called one time per slot.
 *
 * Parameters:
 * slot	: slot # for XX.
 *
 * Returns:
 * none
 * =====================================================================
 */
static void mcn_analyze_interface (int slot)
{
    mcn_instance_t *ds;
    ulong          pcm_format, subcont, total_subcont;
    ushort         ix;
    int            max_digit_collector_num;

    /*
     * allocate the mcn instance for this subcont.
     * mcn_ds is a global array.
     */
    ds = malloc_fast(sizeof(mcn_instance_t));
    if (!ds) {
        crashdump(0);
    }
    mcn_ds[slot] = ds;

    /*
     * Fill in the generic mcn info in the instance.
     */
    ds->mcn_regs = (mcn_block_t *)((uint) BRASIL_GET_SLOT_BASE(slot) +
                                   (uint) MCN_REG_OFFSET);
    ds->mcn_cardtype = mcn_get_adapter_type(slot);
    
    /*
     * For AS5200, the pcm_format should always be set to 2MB.
     */
    pcm_format = M32_AR_PCM_E1;
    
    /*
     * Take the munich32 and adaptor cards out of reset. Wait
     * for approximately 1s to enable M32 TSP rising pulse.
     */
    ds->mcn_regs->pri_control1 |= MCN_ENABLE_MASK;  /* go out of reset */
    for (ix= 0; ix< 200; ix++)
        usecdelay(5000);
    ds->mcn_regs->pri_control1 &= ~MCN_FRAMING_ENABLE_MASK; /* enable the TSP/RSP */
    
    dsx1_pro_add(slot);
    /*FIXME: put subsys init sequece in properly to fix the following hack.
     */
    *(volatile ushort *) (((char*)ds->mcn_regs)+0x180) = 0x1;
    ds->total_subcont = total_subcont = mcn_get_total_subcont(slot);
    for (subcont=0; subcont < total_subcont; subcont++) {
        /*
         * initialize the munich 32 hdlc controller.
         */
        ds->mcn_subcont[subcont].m32_ds = m32_startup(slot, subcont, pcm_format);
        if (ds->mcn_subcont[subcont].m32_ds == NULL) {
            char tmp[20];
            sprintf(tmp, "m32 startup at slot %d, subcontroller ", slot);
            errmsg(&msgsym(NOMEMORY,SYS), tmp, subcont);
            crashdump(0);
        }
        ds->mcn_subcont[subcont].m32_ds->runts_handle = dsx1_set_runts_handle;
        /*
         * Start the Port Adapter up for this slot
         */
        reg_invoke_analyze_mcn_port_adaptor(ds->mcn_cardtype, slot, subcont,
                                            AS5200_MCN_UNITS);
    }

    if (ds->mcn_cardtype == MCN_PA_T1) {

        t1_download(slot); 

        ds->total_dtmf = MAX_DTMF_UNIT_PER_SLOT;
        /*
         * initialize the DTMF data structure
         */
        for (subcont=0; subcont < MAX_DTMF_UNIT_PER_SLOT; subcont++) {
             t1_dtmf_startup(ds, subcont);
        }
    } else if (ds->mcn_cardtype == MCN_PA_E1) {
		/* FIXME: What to do with E1 Here */
        e1_download(slot); 

        ds->total_dtmf = MAX_DTMF_UNIT_PER_SLOT;
        /*
         * initialize the DTMF data structure
         */
        for (subcont=0; subcont < MAX_DTMF_UNIT_PER_SLOT; subcont++) {
             e1_dtmf_startup(ds, subcont);
        }
	}

    /*
     * Initialize statemachine for clock selection during auto-loopback
     */
    ds->loopback_state = MCN_IDLE_STATE;
    
    /*
     * This must be called after reg_invoke_analyze_mcn_port_adaptor() is 
     * executed which will initialize the dsx1 registries used in the
     * reg_invoke_csm_interface_init()...
     */
    max_digit_collector_num = mcn_get_digit_collector_num(slot);
    reg_invoke_csm_interface_init(CSM_ADD_DIGIT_COLLECTOR, slot, 
                                  max_digit_collector_num, 0);
    reg_invoke_modem_set_codec_type(ds->mcn_cardtype == MCN_PA_E1);
}


/*
 * =====================================================================
 * mcn_subsys_init -
 *
 * Description:	MCN subsystem initialisation entry point
 *
 * Parameters:
 * subsys:	subsystype ptr.
 *
 * Returns:
 * Nothing.
 * 
 * =====================================================================
 */
static void mcn_subsys_init (subsystype *subsys)
{
    /*
     * Register the NIM ID for the MCN base card
     */
    reg_add_analyze_interface(BRASIL_NIM_MCN,
			      mcn_analyze_interface,
			      "mcn_analyze_interface");
}

/*
 * MCN subsystem header
 */

#define MCN_MAJVERSION   1
#define MCN_MINVERSION   0
#define MCN_EDITVERSION  1

SUBSYS_HEADER(mcn, MCN_MAJVERSION, MCN_MINVERSION, MCN_EDITVERSION,
	      mcn_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);





