/* $Id: qem.c,v 3.3.20.1.36.1 1996/08/28 13:13:08 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/qem.c,v $
 *------------------------------------------------------------------
 * Interface to SP emulation of hardware queues and accumulators
 *
 * April 1994, Carl Schaefer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qem.c,v $
 * Revision 3.3.20.1.36.1  1996/08/28  13:13:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.20.1  1996/06/22  23:46:51  snyder
 * CSCdi60918:  empty seq: and req: fields should be NULL
 * Branch: California_branch
 *              and eliminate some unneccessary req: and seq:
 *              entries.  Hey 200 bytes is 200 bytes
 *
 * Revision 3.3  1996/02/01  06:09:29  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  18:46:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:57:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "logger.h"
#include "registry.h"
#include "interface_private.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/dbus.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "stacks.h"
#include "rsp_if.h"
#include "rsp_bufhdr.h"
#include "rsp_qasic.h"

/* 
 * QEM_mci_controller
 * Determine if a MCI controller lives at the passed multibus address.
 * copied from hes/if_mci.c
 */

static boolean
QEM_mci_controller (mciregtype *regptr)
{
    ushort hw_version;
    
    regptr->argreg = MCI_HW_VERSION;
    regptr->cmdreg = MCI_CMD_VERSION;
    hw_version = regptr->argreg;
    hw_version >>= 8;
    return(hw_version == MCI_HARDWARE || hw_version == SCI_HARDWARE);
}

static mcitype* find_sp (void)
{
    int ix;
    mciregtype* probe;
    static mcitype sp;

    probe = (mciregtype*) FCISTART;
    for (ix = 0; ix < FCI_MAX_CTRLR_NUMBER; ++ix, ++probe) {
	ushort hwver;

	if (!touch_device(probe))
	    continue;
				/* and copied to rsp_mcistuff.c */
#define	GETSPVERSION	0x8036	/* copied from qem_spif.c */
	probe->cmdreg = GETSPVERSION;
	if ((probe->cmdreg & 0xff00) == 0xf000)
	    goto found_it;

	if (QEM_mci_controller(probe))
	    continue;

	probe->argreg = FCI_HW_VERSION;
	probe->cmdreg = FCI_CMD_VERSION;
	hwver = probe->argreg;
	if ((hwver >> 8) == CBUSII_HARDWARE) {
found_it:    if (sp.unit > 0) {
		errmsg(&msgsym(DISCOVER, QEM), "multiple");
		return NULL;
	    }
	    sp.unit = ix;
	    sp.regptr = probe;
	}
    }
    if (sp.unit == 0) {
	errmsg(&msgsym(DISCOVER, QEM), "no");
	return NULL;
    }

    probe = sp.regptr;

    /*
     * addr of last word is returned, e.g. 0x3ffff for 512Kb
     */
    probe->argreg = FCI_MEMSIZE_MEMD;
    probe->cmdreg = FCI_CMD_MEMSIZE;
    sp.memd_size = probe->largreg;

    return &sp;
}

static void
emulator_rawq_eventq (void)
{
#ifdef MCISTUFF
    extern void MCI_mci_interrupt(void);

    MCI_mci_interrupt();
#endif
    if (RawQ && EventQ)
	rsp_process_rawq();
}

/*
 * rsp_emulation_event_error_interrupt
 * Handle the IRSP emulation interrupt for events and qa errors.
 * Decide which kind of interrupt this is, and then call the appropriate
 * routine to handle that kind of interrupt:
 */

void rsp_emulation_error_interrupt (void)
{
    /*
     * For the emulation only!
     * Check for QA errors:
     */
    rsp_qa_error_interrupt();
}


#define EMULATED_RAWQ_INTLEVEL		3
#define EMULATED_EVENT_ERROR_INTLEVEL	4

/*
 * equivalent to cbus_analyze()
 * except without code to support OIR
 */

static void qem_init (subsystype *subsys)
{
    mcitype* sp;

    /*
     * Set up for our CyBus interrupts from the SP hacked for QA emulation:
     */
    createlevel(EMULATED_RAWQ_INTLEVEL, emulator_rawq_eventq,
		"Emulated RAWQ interupt");
    createlevel(EMULATED_EVENT_ERROR_INTLEVEL,
		rsp_emulation_error_interrupt,
		"Emulated event or error interrupt");

    sp = find_sp();
    if (sp == NULL)
	return;

    emulator_init(sp->regptr);

    {
#define ADRSPC_IDPROM	  0x11131000	/* ID PROM */
	uchar* eeprom = (uchar*) ADRSPC_IDPROM;

	system_mac_addr[0] = 0x00;
	system_mac_addr[1] = 0x00;
	system_mac_addr[2] = 0x0c;
	system_mac_addr[3] = *eeprom++;
	system_mac_addr[4] = *eeprom++;
	system_mac_addr[5] = *eeprom++;
    }    
    return;
}

SUBSYS_HEADER(qem_cbus,			/* name */
	      1,			/* major version */
	      0,			/* minor version */
	      1,			/* edit version */
	      qem_init,			/* init function */
	      SUBSYS_CLASS_KERNEL,	/* subsystem class */
	      "seq: slot",		/* sequence list */
	      NULL   			/* prerequisite list */
	      );

/*
 * assign_mac_addr
 * Allocate a MAC address for an interface based on slot and port.
 */

boolean
rsp_assign_mac_addr (hwidbtype* idb)
{
    ulong newaddr = 0;
    int offset = idb->slot*CBUS2_MAX_CTRLR_INTERFACES+idb->slotunit;

#if 0 /* not needed on RSP emulator */
    /* 
     * if we weren't able to get base address out of EEPROM, fail now
     * and drop back to cookies
     */
    if (!system_mac_addr_avail)
	return (FALSE);
#endif

    /* check for illegal configuration attempts */
    if ((idb->slot >= MAX_CONTROLLERS) ||
	(idb->slotunit > MAX_CTRLR_INTERFACES) ||
	(offset > MAX_CBUS_INTERFACES)) {
	return (FALSE);
    }
    bcopy(system_mac_addr, idb->bia, 3);
    newaddr = (system_mac_addr[3] << 16) |
	(system_mac_addr[4] << 8)  | system_mac_addr[5];
    newaddr += offset;
    idb->bia[5] = newaddr & 0xff; newaddr >>= 8;
    idb->bia[4] = newaddr & 0xff; newaddr >>= 8;
    idb->bia[3] = newaddr & 0xff;
    return(TRUE);
}
