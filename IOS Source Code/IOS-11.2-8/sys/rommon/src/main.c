/* $Id: main.c,v 3.3.28.2 1996/07/08 22:42:43 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/main.c,v $
 *------------------------------------------------------------------
 * main.c
 *
 * August 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The obligatory main routine.
 *------------------------------------------------------------------
 * $Log: main.c,v $
 * Revision 3.3.28.2  1996/07/08  22:42:43  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/03/21  23:27:25  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/08  08:58:05  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:31  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.3  1996/01/17  23:31:19  mansonw
 * CSCdi47058:  Fix the broken ROMMON build
 *
 * Revision 3.2  1995/11/17  18:42:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "signal_4k.h"
#include "monlib.h"

#include "dev.h"
#include "cpu_mips.h"
#include "queue.h"
 
#define DBUS_IMAGE_FILENAME "dbus_image"

struct mon_iface *mifp;  /* our global pointer */

vc_t *vcp;

/* dummy for the new compilers */
__main()
{
}

main()
{
    char buffer[128];

    initdevtbl();
    init_ml_iface();  /* init the monlib interface */

#if defined(RSP2)
    load_dbus_handler();
#endif
    
#if defined(SVIP)
    sprintf(buffer, "%s%s", DEV_EPROM_ID, DBUS_IMAGE_FILENAME);
    /* load and execute the dbus handler */
    if((vcp = (vc_t *)loadprog(buffer, 0, 0, (char **)0)) == NULL)
        {
        }
    setcp0_sreg((getcp0_sreg() & ~SREG_BEV));
#endif

    monitor(1);
}







