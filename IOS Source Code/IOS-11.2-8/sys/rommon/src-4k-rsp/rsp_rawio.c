/* $Id: rsp_rawio.c,v 3.2.58.1 1996/03/21 23:32:29 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-4k-rsp/rsp_rawio.c,v $
 *------------------------------------------------------------------
 * rsp_rawio.c 
 *
 * Mar 95, Steve J. Zhang
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * A RSP ROM Monitor version of rawio.c in sys/rommon/src
 *------------------------------------------------------------------
 * $Log: rsp_rawio.c,v $
 * Revision 3.2.58.1  1996/03/21  23:32:29  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:24:21  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:46:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:09:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:41:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** The raw access routines (aka. RALIB) needed for file system I/O.
** These routines live in the Monitor (EPROM) only.
*/

#include "monitor.h"
#include "signal_4k.h"
#include "dev.h"
#include "monlib.h"
#include "fsio.h"
#include "ralib.h"

/*
** Get the physical file pointer
trouble reading device magic number
** Returns size (adjusted such that it does not run past the end
** of the physical device) on success or -1 on failure
*/
int
raw_getphyfptr(int devnum, char *logfptr, int size, char **retval)
{
    char *phys_fptr;
    struct devstruct *devptr = &devtbl[devnum];

    /*
    ** The file pointer must be converted to a physical file
    ** pointer
    */
    phys_fptr = logfptr + (int)devptr->devstart;

#ifdef DEBUG
printf("entering raw_getphyfptr: devnum: %d, logfptr: %#x, size: %d\n",
       devnum, logfptr, size);
printf("physical file pointer = %#x\n", phys_fptr);
printf("physical device start = %#x\n", devptr->devstart);
printf("physical device end = %#x\n", devptr->devend);
#endif
    /*
    ** Check to make sure the phys_fptr is within the
    ** device boundaries
    */
    if(phys_fptr < devptr->devstart ||
       phys_fptr >= devptr->devend) return(-1);

    *retval = phys_fptr;

    /*
    ** Check to make sure the user is not trying to
    ** read past the end of the physical device
    ** Adjust if necessary
    */
    if(phys_fptr + size > devptr->devend)
	size = devptr->devend - phys_fptr;
    return(size);
}

/*
** The raw read routine
*/
int
raw_read(int devnum, char *src, char *bufptr, int numbytes)
{
    char *phys_fptr;
    struct devstruct *devptr = &devtbl[devnum];
    int ralib_devnum;
    int status;

    if (strcmp(devptr->devid, DEV_SLOT0) == 0) {
	ralib_devnum = ralib_get_dev_num("slot0");
	if (ralib_devnum == -1) {
	    printf("Device %s Doesn't Exit\n", DEV_SLOT0);
	    return(0);
	} 
	status = ralib_device_init(ralib_devnum);
	if (status == RALIB_DEVICE_FIRST_OK ||
	    status == RALIB_DEVICE_STILL_OK) {
	    return(ralib_read(ralib_devnum, (ulong)src, bufptr, numbytes));
	} else {
	    printf("PCMCIA slot0 device is not initialized\n");
	    return(0);
	}
    } else if (strcmp(devptr->devid, DEV_SLOT1) == 0) {
	ralib_devnum = ralib_get_dev_num("slot1");
	if (ralib_devnum == -1) {
	    printf("Device %s Doesn't Exit\n", DEV_SLOT1);
	    return(0);
	}
	status = ralib_device_init(ralib_devnum);
	if (status == RALIB_DEVICE_FIRST_OK ||
	    status == RALIB_DEVICE_STILL_OK) {
	    return(ralib_read(ralib_devnum, (ulong)src, bufptr, numbytes));
	} else {
	    printf("PCMCIA slot1 device is not initialized\n");
	    return(0);
	}
    } else if (strcmp(devptr->devid, DEV_FLASH_ID) == 0) {
	ralib_devnum = ralib_get_dev_num("bootflash");
	if (ralib_devnum == -1) {
	    printf("Device bootflash Doesn't Exit\n");
	    return(0);
	} else {
	    return(ralib_read(ralib_devnum, (ulong)src, bufptr, numbytes));
	}
    } else {
	numbytes = raw_getphyfptr(devnum, src, numbytes, &phys_fptr);

#ifdef DEBUG
	printf("raw_read after calc: numbytes = %d\n", numbytes);
#endif

	/*
	 ** Move the data only if bufptr is non-zero,
	 ** numbytes is greater than zero and the source
	 ** and destination differ
	 */

	if(bufptr && numbytes > 0 && phys_fptr != bufptr) {
#ifdef BITSWAPPEDFLASH
	    if(devptr->flags & DEV_IS_BITSWAPPED)
		bs_bcopy(phys_fptr, bufptr, numbytes);
	    else
#endif

		ml_iface.bcopy(phys_fptr, bufptr, numbytes);

	} /* if */

	return(numbytes);
    }
}

/*
** The raw write routine
*/
int
raw_write(int devnum, char *bufptr, char *dest, int numbytes)
{
    return(-1);  /* not supported at this time */
}


/*
** The raw device status routine
** This is just a stub for now, until I get a chance to code it up
*/
int
raw_stat(int devnum)
{
    return(0);
}

/*
** The raw device control routine
** This is just a stub for now, until I get a chance to code it up
*/
int
raw_ctl(int devnum)
{
    return(0);
}

/*
** The raw device yield routine
** This is just a stub for now, until I get a chance to code it up
*/
int
raw_yield(int devnum)
{
    return(0);
}

/*
** The raw device time routine
** This is just a stub for now, until I get a chance to code it up
*/
int
raw_time(int devnum)
{
    return(0);
}








