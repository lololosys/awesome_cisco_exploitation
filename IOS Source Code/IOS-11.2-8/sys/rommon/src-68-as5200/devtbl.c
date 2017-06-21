/* $Id: devtbl.c,v 1.1.68.1 1996/06/16 21:18:44 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/devtbl.c,v $
 *------------------------------------------------------------------
 * devtbl.c -- The AS5200 device table for the monitor.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: devtbl.c,v $
 * Revision 1.1.68.1  1996/06/16  21:18:44  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:16:23  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "dev.h"
#include "pcmap.h"
#include "as5200_pcmap.h"
#include "mon_boot.h"

extern long etext;
extern int getflashsiz(), bflash_siz(), promscan(), sizemainmem();
extern int fastflashsiz(), fastbflashsiz();
extern char *banner_string;  /* the last label in this image */

char mon_main_mem_id[] = DEV_MEM_ID;
char default_boothelper_id[] = DEV_BOOTFLASH_ID;
char default_bootdev_id[] = DEV_BOOTFLASH_ID;

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
    DEV_FLASH_ID, "flash", (char *)ADRSPC_BRASIL_FLASH_BASE, 0,
         (char *)0, (unsigned)DEV_FS_CAPABLE, getflashsiz,
         FILE_ACCESS_FLASH,
    default_bootdev_id, "boot flash", (char *)ADRSPC_BOOT_FLASH_BASE, 0,
         (char *)0, (unsigned)DEV_FS_CAPABLE, bflash_siz,
         FILE_ACCESS_FLASH,
   DEV_EPROM_ID, "eprom", (char *)&banner_string, 0,
         (char *)(ADRSPC_PROM_END), (unsigned)DEV_FS_CAPABLE,
         promscan, FILE_ACCESS_ROM,
    mon_main_mem_id, "main memory", (char *)ADRSPC_RAM, 0,
         (char *)0, (unsigned)DEV_IS_MEMORY, sizemainmem,
         FILE_ACCESS_UNKNOWN,
};

#define NUMDEVSTRUCTS (sizeof(devtblinit)/sizeof(struct devstruct))

/* mask to turn main memory address into an offset */
unsigned long phymemmask = 0x1ffffff;

/* our working copy of the device table in RAM */
struct devstruct devtbl[NUMDEVSTRUCTS];
struct devstruct *memdevp;
int numdevstructs = NUMDEVSTRUCTS;

/*
** Find the address space that contains the passed address
** and return the device number (index into devtbl).
** Return -1 if not found.
*/
int
addr2devnum(unsigned long addr)
{
    int i;
    struct devstruct *dsp = devtbl;
 
    for(i=0; i < NUMDEVSTRUCTS; i++, dsp++) {
        if(dsp->devsizroutine) {  /* must have this */
            if(!(dsp->flags & DEV_IS_SIZED))
                getdevnum(dsp->devid);
            if(!dsp->devsize) continue;
            if((char *)addr >= dsp->devstart &&
               (char *)addr < dsp->devend)
                return(i);
        }
    }
    return(-1);
}
/* end of module */

