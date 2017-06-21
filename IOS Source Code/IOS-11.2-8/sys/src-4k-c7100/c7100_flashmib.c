/* $Id: c7100_flashmib.c,v 3.1.68.4 1996/09/10 03:22:36 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_flashmib.c,v $
 * ------------------------------------------------------------------
 * c7100_flashmib.c - Flash MIB support code
 *
 * October 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * ------------------------------------------------------------------
 * $Log: c7100_flashmib.c,v $
 * Revision 3.1.68.4  1996/09/10  03:22:36  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.1.68.3  1996/08/28  13:11:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.68.2  1996/04/22  18:58:45  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.1.68.1  1996/03/21  23:47:51  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.1  1995/11/22  21:43:08  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  01:37:41  mbeesley
 * Add placeholder files for new development.
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "../snmp/snmp_api.h"
#include "../dev/sr_flashmib.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../dev/flash_defines.h"
#include "../dev/file_io.h"
#include "../dev/dev_io.h"
#include "../dev/sr_flashmibdefs.h"
#include "../dev/sr_flashmibtype.h"
#include "../dev/snmp_flashmib_cmnds.h"
#include "../dev/flash_services.h"
#include "../filesys/ralib.h"
#include "../filesys/filesys.h"
#include "../filesys/fslib_internals.h"
#include "../filesys/flash_config.h"
#include "../src-4k-c7100/c7100_flashmib.h"
#include "../src-4k-c7100/msg_c7100.c"	/* Not a typo, see logger.h */
#include "../filesys/flash_services_hes.h"
#include "../filesys/msg_filesys.c"	/* Not a typo, see logger.h */

static watched_boolean *flashmib_process_wakeup;
static pid_t flashmib_process_pid;

forktype c7100_flashmib_update_process (void)
{
    process_watch_boolean(flashmib_process_wakeup, ENABLE, RECURRING);
 
    while (TRUE) {
        process_wait_for_event();
        process_set_boolean(flashmib_process_wakeup, FALSE);
 
        /*
         * Build the table for flash device names for the flash
         * MIB agent.
         */
        dev_get_flash_devices(phy_dev_database);
    }
}
 
/*
 * c7100_flashmib_adjust:
 * Trigger the adjustment of the settings in the flash MIB.
 */
void c7100_flashmib_adjust (void)
{
    process_set_boolean(flashmib_process_wakeup, TRUE);
}
 
/*
 * c7100_flashmib_init:
 * Init the flash MIB
 */
void c7100_flashmib_init (void)
{
    flashmib_process_wakeup = create_watched_boolean("Flashmib Process Wakeup",
                                                                            0);
    flashmib_process_pid = process_create(c7100_flashmib_update_process,
                                          "Flash MIB Update",
                                          NORMAL_STACK,
                                          PRIO_NORMAL);
    if (flashmib_process_pid == NO_PROCESS)
        return;
 
    c7100_flashmib_adjust();
}

      
