/* $Id: xx.c,v 3.3.62.2 1996/08/28 13:11:57 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c4000/xx.c,v $
 *------------------------------------------------------------------
 * general routines for DOSXX
 *
 * March, 1992  Marciano Pitargue
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xx.c,v $
 * Revision 3.3.62.2  1996/08/28  13:11:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/03/18  21:59:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/09  05:12:29  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.26.1  1996/02/20  17:56:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  18:43:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:12  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:25:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  13:21:12  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:54:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../les/if_les.h"


/*
 * Check for an XX overtemperature condition,
 * and print an alert message if so:
 */

void check_for_xx_overtemp (void)
{
    ulong control;

    if (cpu_type != CPU_XX)
	return;

    /*
     * Check for overtemperature:
     */
    control = *(volatile ulong *)ADRSPC_CONTROL;
    if (!(control & CONTROL_OVER_TEMP))
	errmsg(&msgsym(OVERTEMP, SYS));
}

void test_leds (parseinfo *csb)
{
    ulong adrspc_control;
    ulong adrspc_shmem_control, adrspc_shmem_copy;
    int i;

    adrspc_control       = *(volatile ulong *)ADRSPC_CONTROL;
    adrspc_shmem_control = *(volatile ulong *)ADRSPC_SHMEM_CONTROL;
    adrspc_shmem_copy    = adrspc_shmem_control;

    for (i = 0; i < 30; i++) {
	*(volatile ulong *)ADRSPC_CONTROL = adrspc_control &
	                                    (~CONTROL_RUNLAMP);
	*(volatile ulong *)ADRSPC_SHMEM_CONTROL = adrspc_shmem_control &
	                                          (~SHMEM_SLOT_LEDS);
	process_sleep_for(100);		/* Let other processes run again */

	*(volatile ulong *)ADRSPC_CONTROL = adrspc_control |
	                                    CONTROL_RUNLAMP;
	*(volatile ulong *)ADRSPC_SHMEM_CONTROL = adrspc_shmem_control |
	                                          SHMEM_SLOT_LEDS;
	process_sleep_for(100);		/* Let other processes run again */
    }

    *(volatile ulong *)ADRSPC_SHMEM_CONTROL = adrspc_shmem_copy;
}

