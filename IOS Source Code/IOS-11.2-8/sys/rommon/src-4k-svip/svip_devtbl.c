/* $Id: svip_devtbl.c,v 1.1.6.1 1996/07/08 22:43:05 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-4k-svip/svip_devtbl.c,v $
 *------------------------------------------------------------------
 * svip_devtbl.c 
 *
 * Mar, 96. Paul Greenfield
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_devtbl.c,v $
 * Revision 1.1.6.1  1996/07/08  22:43:05  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 1.1  1996/03/23  01:22:59  pgreenfi
 * Placeholder files for src-4k-svip.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** The device table.
*/
#include "dev.h"
#include "monitor.h"
#include "mon_boot.h"

extern long etext;
extern int getflashsiz(), bflash_siz(), promscan(), sizemainmem();
extern char *banner_string;  /* the last label in this image */
extern int fastflashsiz(), fastbflashsiz();
extern int slot0_size();
extern int slot1_size();
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
#ifdef BITSWAPPEDFLASH
    default_bootdev_id, "boot flash", (char *)KSEG1_ADRSPC_RXFLASH, 0,
         (char *)0, (unsigned)DEV_IS_BITSWAPPED | DEV_FS_CAPABLE, 0,
         FILE_ACCESS_FLASH, 
#else
    default_bootdev_id, "boot flash", (char *)KSEG1_ADRSPC_RXFLASH, 0,
         (char *)0, (unsigned)DEV_FS_CAPABLE, 0,
         FILE_ACCESS_FLASH, 
#endif
    DEV_EPROM_ID, "eprom", (char *)&banner_string, 0,
         (char *)(KSEG1_ADRSPC_PROM_END), (unsigned)DEV_FS_CAPABLE, promscan,
         FILE_ACCESS_ROM, 
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
