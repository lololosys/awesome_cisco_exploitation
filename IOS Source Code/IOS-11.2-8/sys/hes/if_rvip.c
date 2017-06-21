/* $Id: if_rvip.c,v 3.2.60.1 1996/03/18 19:42:12 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/if_rvip.c,v $
 *------------------------------------------------------------------
 * Stuff for the Versatile Interface Processor (VIP) - RVIP version
 *
 * July 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_rvip.c,v $
 * Revision 3.2.60.1  1996/03/18  19:42:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  21:47:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:18:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:40:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:00:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/19  19:18:22  getchell
 * Placeholders for VIP development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "cbus_registry.h"
#include "if_fci.h"
#include "if_rvip.h"

/*
 * rvip_init
 */
static void rvip_init (subsystype *subsys)
{
    rvip_platform_init(subsys);

    rvip_debug_init();

    reg_add_show_controller_specific(FCI_RVIP_CONTROLLER, rvip_show_cbus, 
				     "rvip_show_cbus");
}

/*
 * VIP subsystem header
 */

#define RVIP_MAJVERSION   1
#define RVIP_MINVERSION   0
#define RVIP_EDITVERSION  1

SUBSYS_HEADER(rvip, RVIP_MAJVERSION, RVIP_MINVERSION, RVIP_EDITVERSION,
	      rvip_init, SUBSYS_CLASS_DRIVER,
	      "seq: dbus, cbus_mci",
	      "req: dbus, cbus_mci");

