/* $Id: pci.c,v 3.3.52.2 1996/03/21 22:49:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/if/pci.c,v $
 *------------------------------------------------------------------
 * pci.c - Generic functions for PCI configuration.
 *         No platform dependancies.
 *
 * Aug 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pci.c,v $
 * Revision 3.3.52.2  1996/03/21  22:49:12  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.3.52.1  1996/03/18  20:14:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.3  1995/12/11  18:49:52  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.2.2.2  1995/11/30  01:07:10  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  20:30:48  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.3  1995/12/01  01:59:35  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.2  1995/11/17  09:30:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:51:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:03:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:54:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "pci.h"

/*
 * Currently, there is only one routine to discover PCI devices on a
 * single PCI bus. In case of multiple buses in hierarchy, another
 * function can be written to use this routine for individual bus
 * configurations.
 */

/*
 * =====================================================================
 * pci_discover_devices -
 *
 * Description:
 * Discover devices on a PCI bus and configure them.
 *
 * Parameters:
 * bus_no               - bus # in PCI hierarchy.
 * pci_max_devices      - max # of PCI devices possible in this bus.
 * context              - The equivalent of a slot in C7000/C4X00
 *                        platforms.
 *        
 * Returns:
 * int if_num		- number of devices found on this bus 
 *
 * =====================================================================
 */

int pci_discover_devices (uchar bus_no,
			  uchar pci_max_devices,
			  uint  context)
{
    ulong vendor_device_id = 0;
    uchar device_no = 0;
    int if_num = 0;

    /*
     * Loop till pci_max_devices on this bus, find out if device exists,
     * and if so  call the appropriate registry based on the
     * VENDOR_DEVICE_ID.
     */
    for (device_no = 0; device_no < pci_max_devices; device_no++) {
	/*
	 * Read the vendor/device ID and call the registry if
	 * we get a valid code.
	 */
	vendor_device_id = pci_config_read(bus_no, device_no, 0, 
					   PCI_VENDOR_ID_OFFSET);
	if ((vendor_device_id & PCI_DEVICE_NOT_PRESENT) == 
             PCI_DEVICE_NOT_PRESENT)
	    continue;

	/*
	 * Now configure the individual device.
	 */
        if_num++;
        if(!reg_invoke_pci_device_init(vendor_device_id, context, device_no)) {
	    if_num--;
	    break;
	}
    }
    return (if_num);
}	

/* end of file */
