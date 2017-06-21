/* $Id: rsp_flashmib.c,v 3.4.8.4 1996/09/10 03:22:46 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_flashmib.c,v $
 * ------------------------------------------------------------------
 * Flash MIB support code
 *
 * October 1995, Carl Schaefer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * in place of file_io.c & flash_services_les.c
 * ------------------------------------------------------------------
 * $Log: rsp_flashmib.c,v $
 * Revision 3.4.8.4  1996/09/10  03:22:46  snyder
 * CSCdi68568:  more constant opportuniites
 *              216 out of data
 *               52 out of image
 * Branch: California_branch
 *
 * Revision 3.4.8.3  1996/08/28  13:13:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.8.2  1996/04/22  18:59:01  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.4.8.1  1996/03/18  22:02:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.3  1996/03/16  07:36:26  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.12.2  1996/03/07  10:44:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  18:00:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/29  05:22:36  foster
 * CSCdi47230:  HSA slave should come up after crash
 *
 * Revision 3.3  1995/12/18  07:09:10  schaefer
 * CSCdi45771: include slave devices in 75xx flash mib
 *
 * Revision 3.2  1995/11/17  18:48:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/18  01:22:47  schaefer
 * placeholders for 7500 flash MIB
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../src-rsp/rsp_flashmib.h"
#include "../dev/flash_defines.h"
#include "../dev/flash_services.h"
#include "../dev/dev_io.h"

void
rsp_flashmib_init (void)
{
    /*
     * Build the table for flash device names for the flash
     * MIB agent.
     */
    dev_get_flash_devices(phy_dev_database);
}
