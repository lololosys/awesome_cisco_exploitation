/* $Id: devtbl.c,v 3.2.58.1 1996/03/21 23:30:39 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-c4000/devtbl.c,v $
 *------------------------------------------------------------------
 * devtbl.c
 *
 * January 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The device table for the monitor.
 *------------------------------------------------------------------
 * $Log: devtbl.c,v $
 * Revision 3.2.58.1  1996/03/21  23:30:39  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:58  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:45:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** The device table.
*/
#include "monitor.h"
#include "dev.h"
#include "mon_boot.h"

extern long etext;
extern int getflashsiz(), bflash_siz(), promscan(), sizemainmem();
extern char *banner_string;  /* the last label in this image */
extern int fastflashsiz(), fastbflashsiz();
char mon_main_mem_id[] = DEV_KMEM1_ID;
char default_bootdev_id[] = DEV_BOOTFLASH_ID;
char default_boothelper_id[] = DEV_BOOTFLASH_ID;

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
    DEV_FLASH_ID, "flash", (char *)KSEG1_ADRSPC_FLASH_BASE, 0,
         (char *)0, (unsigned)DEV_FS_CAPABLE, getflashsiz,
         FILE_ACCESS_FLASH,
    default_bootdev_id, "boot flash", (char *)KSEG1_ADRSPC_BFLASH_BASE, 0,
         (char *)0, (unsigned)DEV_FS_CAPABLE, bflash_siz,
         FILE_ACCESS_ROM,
    DEV_EPROM_ID, "eprom", (char *)&banner_string, 0,
         (char *)(KSEG1_ADRSPC_PROM_END), (unsigned)DEV_FS_CAPABLE,
         promscan, FILE_ACCESS_ROM,
    mon_main_mem_id, "main memory (kseg1)", (char *)ADRSPC_K1BASE, 0,
         (char *)0, (unsigned)DEV_IS_MEMORY, sizemainmem,
         FILE_ACCESS_UNKNOWN,
    DEV_KMEM0_ID, "main memory (kseg0)", (char *)ADRSPC_K0BASE, 0,
         (char *)0, (unsigned)DEV_IS_MEMORY, sizemainmem,
         FILE_ACCESS_UNKNOWN,
    DEV_KMEMU_ID, "main memory (kuseg)", (char *)ADRSPC_KUBASE, 0,
         (char *)0, (unsigned)DEV_IS_MEMORY, sizemainmem,
         FILE_ACCESS_UNKNOWN,
};
#define NUMDEVSTRUCTS (sizeof(devtblinit)/sizeof(struct devstruct))

/* mask to turn main memory address into a physical address */
unsigned long phymemmask = 0x1fffffff;

/* our working copy of the device table in RAM */
struct devstruct devtbl[NUMDEVSTRUCTS];
struct devstruct *memdevp;
int numdevstructs = NUMDEVSTRUCTS;


