/* $Id: if_svip.c,v 3.1.60.1 1996/03/21 22:40:48 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/if_svip.c,v $
 *------------------------------------------------------------------
 * if_svip.c - Stuff for the Versatile Interface Processor (VIP) 
 *             SVIP version
 *
 * November 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_svip.c,v $
 * Revision 3.1.60.1  1996/03/21  22:40:48  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.1  1996/02/10  00:04:13  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/28  05:01:41  getchell
 * Placeholder for VIPER development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#include "master.h"
#include "subsys.h"
#include "registry.h"
#include "cbus_registry.h"
#include "if_fci.h"
#include "if_svip.h"

/*
 * svip_init
 */
static void svip_init (subsystype *subsys)
{
    svip_platform_init(subsys);

    svip_debug_init();

    reg_add_show_controller_specific(FCI_SVIP_CONTROLLER, svip_show_cbus, 
				     "svip_show_cbus");
}

/*
 * VIP subsystem header
 */

#define SVIP_MAJVERSION   1
#define SVIP_MINVERSION   0
#define SVIP_EDITVERSION  1

SUBSYS_HEADER(svip, SVIP_MAJVERSION, SVIP_MINVERSION, SVIP_EDITVERSION,
	      svip_init, SUBSYS_CLASS_DRIVER,
	      "seq: dbus, cbus_mci",
	      "req: dbus, cbus_mci");

/* end of file */
