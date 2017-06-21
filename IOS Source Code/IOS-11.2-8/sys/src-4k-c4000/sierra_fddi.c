/* $Id: sierra_fddi.c,v 3.3.62.2 1996/08/28 13:11:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c4000/sierra_fddi.c,v $
 *------------------------------------------------------------------
 * Sierra platform specific FDDI code.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sierra_fddi.c,v $
 * Revision 3.3.62.2  1996/08/28  13:11:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/03/18  21:58:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:50:31  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:41:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:22:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***********************************************************************
                                Includes
*********************************************************************/
/* cisco includes */
#include "../xdi/cschdrsif.h"
#include "stacks.h"
#include "logger.h"
#include "../les/msg_nim.c"		/* Not a typo, see logger.h */

/* XDI includes */
#include        "../xdi/xdihdrsif.h"
#include        "../xdi/csphdr.h"
#include        "../xdi/smtmsg.h"
#include        "../xdi/cspproto.h"

#include "../if/if_fddi.h"


/*********************************************************************
	Variables
*********************************************************************/
/* external */
extern int nfddinets;

/* internal */
ulong orig_nmi_wrapper = 0;


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
    changelevel(NMI_LEVEL, r4k_rt_timer_interrupt,
		"FDDI NMI Interrupt Handler");
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
Return:         void.
*********************************************************************/
{
    ushort		i;

    /* Report CSP System Timer event */
    ServiceCSPTimer();

    /*
     * If a timer expired, use icr, control bus write command
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

boolean init_bsi_dev (hwidbtype *idb)
/*********************************************************************
Function:       Set up the saved idb variable, the interrupt vector
                and initialize the vector register.
Parameters:     idb:    hardware idb of MAC being initialized.
Return:         TRUE if no problem, and FALSE otherwise.
*********************************************************************/
{
    bsi_instance_t *ds;

    /*
     * Set up the level 2 jumptable
     */
    if (idb->slot >= C4000_NUM_SLOTS)
        return(FALSE);
    set_slot_idb(idb);
    set_slot_vector(idb->slot, fddi_wrapper);
   
    /*
     * Make sure interrupt vector in csr1 is valid. Note
     * we only care about bit 0 on Sierra.
     */
    ds = INSTANCE;
    ds->module_regs->csr1 = 0;
    return(TRUE);
}


void bsi_check_version (int slot)
/*********************************************************************
Function:       Check for minimum NIM version level
Parameters:     slot: slot number of the NIM to check
Return:         void
*********************************************************************/
{
    ushort type;
    charlong version;
    xx_fddi_regs_t *nim;
    int i;
    
    type = nim_get_type(slot);

    nim = (xx_fddi_regs_t *)GET_SLOT_BASE(slot);

    switch (type) {
      case XX_FDDI_SINGLE:
      case XX_FDDI_DUAL:
      case XX_FDDI_SBRIDGE:
      case XX_FDDI_DBRIDGE:
	/*
	 * Get the BSI Rev level
	 */
	nim->bsi.pcar = 0;
	for (i = 0; i < 4; i++) {
	    version.d.byte[i] = nim->bsi.mbar;
	}
	if (version.d.lword < SIERRA_FDDI_MIN_VERSION)
	    errmsg(&msgsym(BADNIMREV, NIM), "FDDI", slot, version);
	break;

      default:
	break;
    }
}
