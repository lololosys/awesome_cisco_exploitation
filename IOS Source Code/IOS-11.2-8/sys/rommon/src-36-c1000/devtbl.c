/* $Id: devtbl.c,v 3.2.58.1 1996/03/21 23:28:51 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/devtbl.c,v $
 *------------------------------------------------------------------
 * devtbl.c
 *
 * October 1994, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The device table for the monitor.
 *------------------------------------------------------------------
 * $Log: devtbl.c,v $
 * Revision 3.2.58.1  1996/03/21  23:28:51  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:46  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:43:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "dev.h"
#include "mon_boot.h"

extern long etext;
extern int promscan(), sizemainmem(), sizepcmcia();
extern char *banner_string;  /* the last label in this image */

char mon_main_mem_id[] = DEV_MEM_ID;
char default_boothelper_id[] = DEV_EPROM_ID;
char default_bootdev_id[] = DEV_EPROM_ID;

/*
** The first item in the devstruct table is the default
** search area when a device specifier is not used or known.
** The network device(s) must be the only ones with a NULL
** devsizroutine
** This is the initial structure in EPROM, it is copied into
** a RAM structure at runtime so fields can be updated
*/

struct devstruct devtblinit[] = {
    DEV_NETWORK_ID, "network", (char *)0, 0,
         (char *)0, (unsigned)0, (int (*)())0,
         FILE_ACCESS_UNKNOWN,
    default_boothelper_id, "eprom", (char *)&banner_string, 0,
         (char *)(ADRSPC_PROM + ADRSPC_PROM_SPC_SIZ),
         (unsigned)DEV_FS_CAPABLE, promscan,
         FILE_ACCESS_ROM,
    mon_main_mem_id, "main memory", (char *)ADRSPC_RAM, 0,
         (char *)0, (unsigned)DEV_IS_MEMORY, sizemainmem,
         FILE_ACCESS_UNKNOWN, 
    DEV_FLASH_ID, "PCMCIA slot 1", (char *)ADRSPC_PCMCIA_MEM, 0,
         (char *)0, (unsigned)DEV_FS_CAPABLE, sizepcmcia,
         FILE_ACCESS_FLASH,
};

#define NUMDEVSTRUCTS (sizeof(devtblinit)/sizeof(struct devstruct))

/* mask to turn main memory address into an offset */
unsigned long phymemmask = 0x1ffffff;

/* our working copy of the device table in RAM */
struct devstruct devtbl[NUMDEVSTRUCTS];
struct devstruct *memdevp;
int numdevstructs = NUMDEVSTRUCTS;
    
/* end of module */

