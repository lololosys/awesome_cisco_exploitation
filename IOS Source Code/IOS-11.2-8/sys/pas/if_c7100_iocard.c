/* $Id: if_c7100_iocard.c,v 3.1.66.4 1996/08/28 13:05:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_c7100_iocard.c,v $
 *------------------------------------------------------------------
 * if_c7100_iocard.c - Support for Predator IO card 
 *		       Presently 1fe media only (MII)
 *
 * November 1995, Michael Beesley
 * 
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c7100_iocard.c,v $
 * Revision 3.1.66.4  1996/08/28  13:05:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.66.3  1996/07/18  20:04:40  mbeesley
 * CSCdi63360:  Not all IO cards have fastether, make it optional
 * Branch: California_branch
 *
 * Revision 3.1.66.2  1996/04/06  02:05:31  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/03/21  23:22:50  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.4  1996/02/15  19:48:40  dcarroll
 * CSCdi49080:  Add slots support to pas drivers
 * Branch: ELC_branch
 * Add missing slots initialization to pas drives,
 * for oir and snmp support.
 *
 * Revision 3.1.2.3  1995/12/12  04:53:55  mbeesley
 * Branch: ELC_branch
 * Allow "show pci bridge" to display all bridges. Change the 1fe GPC
 * register settings for Predator IO card 1fe. Fix some amdp2 error
 * messages. And turn power off on unrecognized port adaptors.
 *
 * Revision 3.1.2.2  1995/12/08  05:33:26  mbeesley
 * Branch: ELC_branch
 * Mini-sync ... get correct ID fields, and parse chains.
 *
 * Revision 3.1.2.1  1995/11/22  21:41:40  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/20  22:07:22  mbeesley
 * Add placeholder files for future development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "subsys.h"
#include "sched.h"
#include "parser.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"

#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_inline.h"
#include "../if/pci.h"
#include "../ip/ip.h"

#include "../snmp/snmp_api.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../snmp/snmp_registry.h"

#include "if_pas.h"
#include "msg_pas.c"			/* Not a typo, see logger.h */
#include "msg_dec21140.c"		/* Not a typo, see logger.h */
#include "pas_id.h"
#include "if_pas_dec21140.h"

#include "../src-4k-c7100/msg_c7100.c"	/* Not a typo, see logger.h */

/*
 * iocard_fe_analyze:
 * Discover the devices on the card (fastether version)
 */
static void iocard_fe_analyze (int pa_bay)
{
    uchar bus_no;

    /*
     * Make sure this is pa_bay 0
     */
    if (pa_bay != 0) {
	pas_deactivate_bay(pa_bay);
	errmsg(&msgsym(IONOT0, C7200), pa_bay);
	return;
    }

    /* 
     * Get the bus number
     */
    bus_no = get_pci_bus_no(pa_bay);
    
    /*
     * Initialize the device. The FE is device six on PCI bus 0.
     */
    if(!dec21140_analyze_device(pa_bay, PCI_IO1FE_DEV_NUM)) {
	pas_deactivate_bay(pa_bay);
	errmsg(&msgsym(DISCOVER, DEC21140), 0, pa_bay);
	return;
    }

    /*
     * Update slots state.
     */
    pas_activate_bay(pa_bay);

    /*
     * Install PA wrapper
     */
    pas_install_wrapper(pa_bay, dec21140_eim_wrapper);
}

/*
 * iocard_analyze:
 * Discover the devices on the card (non-fastether version)
 */
static void iocard_analyze (int pa_bay)
{
    /*
     * Make sure this is pa_bay 0
     */
    if (pa_bay != 0) {
	pas_deactivate_bay(pa_bay);
	errmsg(&msgsym(IONOT0, C7200), pa_bay);
	return;
    }

    /*
     * Update slots state.
     */
    pas_activate_bay(pa_bay);
}

/*
 * iocard_subsys_init:
 * Initialize the IOCARD subsystem
 */
static void iocard_subsys_init (subsystype *subsys)
{
    /*
     * Register the IO cards (both FE and non-FE)
     */
    reg_add_analyze_interface(PA_HARDWARE_C7100_IO1FE,
                              iocard_fe_analyze, "iocard_fe_analyze");
    reg_add_analyze_interface(PA_HARDWARE_C7100_IO,
                              iocard_analyze, "iocard_analyze");
}

/*
 * IO Card subsystem header
 */
#define IOCARD_MAJVERSION   1
#define IOCARD_MINVERSION   0
#define IOCARD_EDITVERSION  1

SUBSYS_HEADER(iocard, IOCARD_MAJVERSION, IOCARD_MINVERSION,
	      IOCARD_EDITVERSION,
	      iocard_subsys_init, SUBSYS_CLASS_DRIVER,
	      "seq: dec21140",
	      "req: dec21140");

/* end of file */
      
