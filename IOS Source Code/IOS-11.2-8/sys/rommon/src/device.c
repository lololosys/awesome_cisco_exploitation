/* $Id: device.c,v 3.2.58.1 1996/03/21 23:27:02 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/device.c,v $
 *------------------------------------------------------------------
 * device.c
 *
 * February 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Support routines for the device concept.
 *------------------------------------------------------------------
 * $Log: device.c,v $
 * Revision 3.2.58.1  1996/03/21  23:27:02  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:10  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "dev.h"
#include "mon_defs.h"
#include "monfsio.h"

/*
** See if a device is specified in name
** and return the index into the device table.
** If device has not been sized, size it.
*/
int
getdevnum(char *name)
{
    int i, len;
    struct devstruct *devptr = devtbl;

    for(i=0; i<numdevstructs; i++) {
	len = strlen(devptr->devid);
	if(strncmp(devptr->devid, name, len) == 0) {
	    /* found our device */
	    if(devptr->devsizroutine &&    /* not a network device */ 
	       !(devptr->flags & DEV_IS_SIZED)) {
		devptr->devsize = (*devptr->devsizroutine)(devptr);
		devptr->devend = devptr->devstart + devptr->devsize;
		if(!devptr->devsize) {
    printf("getdevnum warning: device \"%s\" has size of zero\n", devptr->devname);
		} else devptr->flags |= DEV_IS_SIZED;
	    }
	    return(i);
	}
	devptr++;
    }
    return(-1);
}

/*
** See if a device is specified in name.
** Return the number of characters in the dev name and separator
** Return 0 if name does not contain a device specifier
** Return -1 if name contains a bad device specifier
*/
int
isdevname(char *name)
{
    int devnum;

    if(!strchr(name, DEVSEPARATOR)) return(0);
    if((devnum = getdevnum(name)) >= 0)
	return(strlen(devtbl[devnum].devid));  /* DEVSEPARATOR is included */
    return(-1);
}

/*
** Clear the DEV_LIB_LOADED flag in all of the
** dev structs in devtbl
*/
void
clr_dev_lib()
{
    int i;
    struct devstruct *devptr = devtbl;

    monlib_upper_bound = 0;

    for(i=0; i<numdevstructs; i++) {
	devptr->flags &= ~DEV_LIB_LOADED;
	devptr++;
    }
}

/*
** Copy devtblinit into our working devtable in RAM and
** initialize memdevp.
*/

void
initdevtbl()
{
    bcopy(devtblinit, devtbl, sizeof(struct devstruct) * numdevstructs);
    memdevp = (struct devstruct *)0;
}

/*
** Scan the EPROM for possible image
*/
int
promscan(struct devstruct *devptr)
{
    long *ptr, *end;

    ptr = (long *)devptr->devstart;  /* point to the end of the monitor code */
    ptr = (long *)(((long)ptr + 3) & ~3);  /* longword align up */
    end = ptr + 0x10000;                   /* check 64k */
    while(ptr < end) {
	/* check to see if this location bus errors */
	if(chkberr(ptr, BW_32BITS, 1) > 0) break;
	if(ismagic(ptr)) {
	    devptr->devstart = (char *)ptr;  /* fix up the start of device */
	    /* return the size of the device */
	    return((int)devptr->devend - (int)devptr->devstart);
	}
	ptr++;
    }
    return(0);
}

/* end of module */
