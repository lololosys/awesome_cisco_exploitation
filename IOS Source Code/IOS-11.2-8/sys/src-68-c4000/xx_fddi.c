/* $Id: xx_fddi.c,v 3.3 1995/11/17 18:43:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-68-c4000/xx_fddi.c,v $
 *------------------------------------------------------------------
 * XX platform specific FDDI code.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: xx_fddi.c,v $
 * Revision 3.3  1995/11/17  18:43:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:55:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************
	Includes
*********************************************************************/

/* cisco includes */
#include "../xdi/cschdrsif.h"

/* XDI includes */
#include        "../xdi/xdihdrsif.h"
#include        "../xdi/fddihdr.h"
#include        "../xdi/csphdr.h"
#include        "../xdi/cspglbl.h"
#include        "../xdi/smtmsg.h"
#include        "../xdi/cspproto.h"

#include "../if/if_fddi.h"


/*********************************************************************
	Variables
*********************************************************************/
extern int nfddinets;

void (*orig_nmi_wrapper)(void);

static ushort leds_config[] = { /* 3809 LEDs configurations f(CF_State) */
    0,                                                  /* isolated     */
    0,                                                  /* local_a (NA) */
    0,                                                  /* local_b (NA) */
    0,                                                  /* local_ab (NA)*/
    0,                                                  /* local_s (NA) */
    CSR0_WRAP_A_LED | CSR0_ACTV_A_LED,                  /* wrap_a       */
    CSR0_WRAP_B_LED | CSR0_ACTV_B_LED,                  /* wrap_b       */
    0,                                                  /* wrap_ab (NA) */
    CSR0_WRAP_A_LED | CSR0_ACTV_A_LED,                  /* wrap_s       */
    CSR0_WRAP_A_LED | CSR0_ACTV_A_LED,                  /* c_wrap_a     */
    CSR0_WRAP_B_LED | CSR0_ACTV_B_LED,                  /* c_wrap_b     */
    CSR0_WRAP_A_LED | CSR0_ACTV_A_LED,                  /* c_wrap_s     */
    CSR0_THRU_LED | CSR0_ACTV_A_LED | CSR0_ACTV_B_LED   /* thru         */
};



/*********************************************************************
	Interface Functions
*********************************************************************/


/*
 * init_bsi_dev:
 * Set up the saved idb variable, the interrupt vector
 * and initialize the vector register
 */
boolean init_bsi_dev (hwidbtype *idb)
/*********************************************************************
Function:	Set up the saved idb variable, the interrupt vector
		and initialize the vector register.
Parameters:	idb:	hardware idb of MAC being initialized.
Return:		TRUE if no problem, and FALSE otherwise.
*********************************************************************/
{
    bsi_instance_t *ds;

    switch (idb->slot){
    case 0:
        set_slot_vector(idb->slot, 0, (uint)fim_slot1);
        set_slot_vector(idb->slot, 1, (uint)cam_slot1);
        add_idb_to_ichain(idb->slot, 0, idb, (uint)fim_slot1_jump_entry);
        break;
    case 1:
        set_slot_vector(idb->slot, 0, (uint)fim_slot2);
        set_slot_vector(idb->slot, 1, (uint)cam_slot2);
        add_idb_to_ichain(idb->slot, 0, idb, (uint)fim_slot2_jump_entry);
        break;
    case 2:
        set_slot_vector(idb->slot, 0, (uint)fim_slot3);
        set_slot_vector(idb->slot, 1, (uint)cam_slot3);
        add_idb_to_ichain(idb->slot, 0, idb, (uint)fim_slot3_jump_entry);
        break;
    default:
        return(FALSE);
    }

    set_slot_idb(idb);

    /*
     * Make sure interrupt vector in csr1 is valid:
     */
    ds = INSTANCE;
    ds->module_regs->csr1 = get_slot_vector(idb->slot, 0);
    return(TRUE);
}


void set_synoptics_leds (ushort cf_state)
/*********************************************************************
Function:	Set the FDDI LEDs on the Synoptics 3809 Hub XX NIM.
Parameters:	cf_state:	XDI CFM state index.
Return:		void
*********************************************************************/
{
    ushort              leds_shadow;
    xx_fddi_regs_t      *nim;

    nim = ((bsi_instance_t *)HWIDBDATA (0, 0, devctl))->module_regs;

    /* Set LEDs shadow */
    leds_shadow = leds_config[cf_state];
    if (CSPDATA (0, portData[PHY_A].PC_Mode) == PC_Mode_Tree)
        leds_shadow |= CSR0_TREE_A_LED;
    if (CSPDATA (0, portData[PHY_B].PC_Mode) == PC_Mode_Tree)
        leds_shadow |= CSR0_TREE_B_LED;

    /* Mask write CSR0 LED bits (0=On, 1=Off) */
    nim->csr0 = (nim->csr0 & ~CSR0_LEDS_MASK)|(~leds_shadow & CSR0_LEDS_MASK);
}


/*********************************************************************
	Timer Functions
*********************************************************************/

void rt_timer_start (void)
/*********************************************************************
Function:	This routine installs the real timer interrupt handler
			and enables the timer tick.
Parameters:	None.
Return:		Void.
*********************************************************************/
{
    orig_nmi_wrapper = exceptionHandler(NMIVector, rt_timer_nmi_wrapper);
}

void csp_rt_timer_int_hand (uchar *nullptr0,  uchar *nullptr1, uchar *nullptr2)
/*********************************************************************
Function:       Handle RT timer tick.
		Since this routine will be called in NMI Interrupt
			Level, minimum processing is required, which
			consists of checking for CSP time-out events
			and waking up the CSP process at a lower
			interrupt level (CMT level).
Parameters:     None.
Input:          None.
Output:         None.
Return:         void.
*********************************************************************/
{
    ushort		i;

    /* Report CSP System Timer event */
    ServiceCSPTimer();

    /*
     * If a timer expired, use p_icr, control bus write command
     * reject bit CCR to force a level 4 interrupt by writing to
     * a read only register.
     *
     * This has to be done on all XX FDDI NIMs for the case of
     * any being down.
     */
    if (!EmptySignalQueue()) {
	for (i=0; i<nfddinets; i++)
	    /* Force level 4 intr through an illegal write */
	    PLAYERREG (i,0,idr) = 1;
    }
}


void bsi_check_version (int slot)
/*********************************************************************
Function:       Check for minimum NIM version level
Parameters:     slot: slot number of the NIM to check
Return:         void
*********************************************************************/
{
    /*
     * No FDDI NIM version problems on XX
     */
}
