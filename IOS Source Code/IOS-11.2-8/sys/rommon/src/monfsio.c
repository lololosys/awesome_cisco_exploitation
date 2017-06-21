/* $Id: monfsio.c,v 3.2.58.4 1996/07/08 22:42:47 mwu Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/monfsio.c,v $
 *------------------------------------------------------------------
 * monfsio.c
 *
 * November 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Monitor file system I/O routines.
 *------------------------------------------------------------------
 * $Log: monfsio.c,v $
 * Revision 3.2.58.4  1996/07/08  22:42:47  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/06/16  21:18:22  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.58.2  1996/04/09  23:09:26  szhang
 * CSCdi53689:  Open() failed in PCMCIA monlib
 * Branch: California_branch
 * Monitor shouldn't reset the PCMCIA controller after the MONLIB
 * is launched.
 *
 * Revision 3.2.58.1  1996/03/21  23:27:32  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:37  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:42:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Support for file systems.
** This is what the programmer sees at the API.
** This code resides in the monitor only.
*/
#include "monitor.h"
#include "getdents.h"
#include "fsio.h"
#include "fstat.h"
#include "dev.h"
#include "mon_boot.h"
#include "sh.h"
#include "signal_4k.h"
#include "proto.h"
#include "dib.h"
#include "monlib.h"
#include "monfsio.h"
#include "simpfsio.h"
#include "extern.h"
#include "mon_boot.h"

/*
** The file open routine.
** This is made to resemble as close as possible the Un*x open
** routine.  This module is made to be re-entrant.
*/

struct mon_fsio openfiles[MAXOPENFILE];
struct fs_device_info_block_t dib;
int memdevnum;  /* must be global for raw_read */
long bootipaddr;
unsigned long monlib_upper_bound;
unsigned long monlib_launch;

/* forward declaration */
extern int o_read(int devnum, char *src, char *bufptr, int numbytes);
extern long memsize;
extern char *etext;
/*
 * The Brasil montior uses imageaddr to determine the
 * correct load point for netbooting a relocatable image.
 */
extern ulong imageaddr;

static char *bootldrstr  = "BOOTLDR";  /* monitor variable that specifies the boot loader */
static char *monlibstr   = "MONLIB";   /* monitor variable that specifies the Monitor Lib */
#define SIMPFILECHAR  '%'
#define MONLIB_NAME_LEN  32

int
open(char *path, int mode)
{
    int fd, monlib_offset, simpfile, devnum, retcode, tmp;
    char *filename;
    char *cptr;
    unsigned long magic;
    struct mon_fsio *ofptr;
    struct devstruct *devptr;
    SIG_PF savfcn;
    struct shstuff shstuff;
    struct plb plb;
    struct pib pib;
    struct fs_device_info_block_t *dibptr;

    monlib_upper_bound = 0;

    init_ml_iface();
    clr_dev_lib();  /* clear flags in other device structures */    

#ifdef DEBUG
    printf("entering open: path = \"%s\"   mode = %d\n", path, mode);
#endif

    /*
    ** If no device is specified or it's not found
    ** assume the first device
    */
    if((devnum = getdevnum(path)) < 0) {
	filename = path;
	devptr = devtbl;
	devnum = 0;
    } else {
	filename = path + isdevname(path);  /* bump past device specifier */
	devptr = &devtbl[devnum];
    }

#ifdef DEBUG
    printf("open: got devnum %d, devname is %s\n", devnum, devptr->devname);
#endif

    /*
    ** Get a free mon_fsio struct.
    */
    savfcn = signal(SIGINT, SIG_IGN);  /* ignore user interrupts */
    ofptr = (struct mon_fsio *)0;      /* init */
    for(fd=0; fd < MAXOPENFILE; fd++) {
	if(!openfiles[fd].flags) {
	    ofptr = &openfiles[fd];    /* found open slot */
	    ofptr->flags = MFS_RAW;     /* initially */
	    break;
	}
    }
    signal(SIGINT, savfcn);            /* set it back */
    if(!ofptr) {
	printf("open failed due to full file table\n");
	return(-1);
    }

#ifdef DEBUG
    printf("open: got fd %d\n", fd);
#endif

    /* 
    ** Programs are loaded into main memory for execution
    */
    if(!memdevp) {
	if((memdevnum = getdevnum(mon_main_mem_id)) < 0) {
	    printf("cannot find main memory in device table\n");
nomem:
	    printf("main memory required for this command\n");
openerr:
	    ofptr->flags = 0;  /* free this entry */
	    return(-1);
	}
	memdevp = &devtbl[memdevnum];
	if(memdevp->devsize == 0) goto nomem;
    }

    ofptr->devnum = devnum;
    ofptr->usrfd = fd;
    ofptr->rfptr = (char *)0;  /* this is a logical file pointer */
    ofptr->start = ofptr->rfptr;
    ofptr->size = devptr->devsize;

#ifdef DEBUG
printf("device is devnum: %d is: \"%s\"\n", devnum, devptr->devname);
printf("device starting address is %#x\n",devptr->devstart);
printf("device ending address is %#x\n",devptr->devend);
printf("filename is: %s\n", filename);
printf("mem device is: %s\n", memdevp->devname);
printf("mem device starting address is %#x\n",memdevp->devstart);
printf("mem device ending address is %#x\n",memdevp->devend);
#endif

    if(*filename == '\0') {  /* no filename specified - raw device */
	if(!devptr->devsizroutine) {  /* network device */
	    printf("a file must be specified on device: %s\n", devptr->devname);
	    goto openerr;
	} else {  /* user wishes to open the raw device */
	    if(mode != O_RDONLY) {
		printf("open: raw device can only be opened for reads\n");
		goto openerr;
	    }
	}
#ifdef BRASIL
    /*
     * Setup imageaddr to default point to the start of the raw device.
     */
    imageaddr = (ulong)devptr->devstart;
#ifdef DEBUG
printf("\nraw device imageaddr: 0x%x\n", imageaddr);
#endif
#endif

    } else {  /* filename specified */
	pib.mifp = mifp;   /* initialize for later use */
	pib.argc = 0;
	pib.argv = (char **)0;
	if(devptr->flags & DEV_IS_MEMORY) {
	    /*
	    ** Filenames are not legal in main memory as there will never be
	    ** a file system there.
	    ** The filename here is expected to be a start address (actually
	    ** an offset from the start of memory) in hex.
	    ** This case is used for compressed files.
	    */
	    cptr = filename;
	    tmp = getnum(cptr, 16, &magic);
#ifdef BRASIL
    /*
     * Setup imageaddr to point to the image in memory.
     */
    imageaddr = (ulong)magic;
#ifdef DEBUG
printf("\nmemory imageaddr: 0x%x\n", imageaddr);
#endif DEBUG
#endif
	    if(!tmp) {
		printf("open: illegal characters after device specifier\n");
		goto openerr;
	    }

	    /*
	    ** Note that rfptr is an offset from the start of the physical
	    ** device
	    ** Physical device base addresses are dictated by the devtbl
	    ** entry
	    */
	    ofptr->rfptr = (char *)((long)magic & phymemmask);
	    ofptr->start = ofptr->rfptr;

	    cptr += tmp;
	    if(*cptr == ',') {  /* size is specified too */
		cptr++;
		tmp = getnum(cptr, 16, &magic);
	    }

	    if(tmp) ofptr->size = magic;   /* valid size specified */
	    else ofptr->size = devptr->devsize - (int)ofptr->start;
	    ofptr->flags = MFS_NET;  /* handle just like net file */
	} else if(!devptr->devsizroutine) {  /* network device */
	    /*
	    ** If this is a netbooted image, there is an interim step.  We must load the
	    ** boot helper image and execute it in order to obtain the netbooted file.
	    */
	    if((shstuff.cmdptr = getvar((NVRAM)->evartbl,bootldrstr)) != (char *)0 &&
	       *shstuff.cmdptr != '\0') {
		/* don't want to clobber the environment variable table */
		strcpy(shstuff.argvbuf, shstuff.cmdptr);
	    } else {
		/* try the first file in default_boothelper_id */
		sprintf(shstuff.argvbuf, default_boothelper_id);
		if(getfirstfilename(shstuff.argvbuf) < 0) {
		    printf("an alternate boot helper program is not specified\n"
			   "(monitor variable \"%s\" is not set)\n"
			   "and unable to determine first file in bootflash\n",
			   bootldrstr);
		    goto openerr;
		}
	    }
	    shstuff.argv[0] = shstuff.argvbuf;
	    shstuff.cmdptr = shstuff.argv[0];
	    shstuff.argc = 1;

	    /*
	    ** Hopefully we don't need to fully parse everything here.
            ** The boot helper program should never have any arguments.
	    */

	    if(loadprog(shstuff.cmdptr, 1, 0, (char **)0) < 0) {  /* load only */
		printf("open: failed to find and/or load the bootloader: \"%s\"\n",
		       shstuff.cmdptr);
		goto openerr;
	    }

	    /*
	    ** We now have the boothelper image ready to execute so
	    ** launch it with the name of the program we wish to netboot.
	    ** A bootstring has been specially prepared by boot() to be passed
	    ** to the boothelper.
	    */
	    plb.plb_str = bootstring;  /* point to the name of image to boot */
	    plb.plb_addr = (char *)0;
	    plb.plb_size = 0;
	    plb.plb_ipaddr = 0;

	    retcode = launch(boothelper_launchcode, &plb, &pib, entrypt);

#ifdef DEBUG
printf("open: returned from boothelper, retcode %#x, plb_str: %s, plb_size: %#x\n",
            retcode, plb.plb_str ? plb.plb_str : "NULL", plb.plb_size);
if(retcode && plb.plb_size) {
    printf("checksumming image loaded by boothelper image (location %#x, size %#x) ...",
	   plb.plb_addr, plb.plb_size);
    printf("\nchecksum is %#x\n",
	   u_chksum((unsigned char *)plb.plb_addr, plb.plb_size));
}
#endif

	    if(!retcode || !plb.plb_size) {
		/* unsuccessful (net)boot */
		printf("boot of \"%s\" using boot helper \"%s\" failed\n"
                       "error returned: %s\n",
		       bootstring, shstuff.cmdptr,
                       plb.plb_str ? plb.plb_str : "unknown");
		goto openerr;
	    }

	    /*
	    ** We now have our image in RAM.
	    ** Set up the appropriate stuff.
	    ** Note that rfptr is an offset from the start of the physical
	    ** device
	    ** Physical device base addresses are dictated by the devtbl
	    ** entry
	    */
	    ofptr->rfptr = (char *)((long)plb.plb_addr & phymemmask);
	    ofptr->start = ofptr->rfptr;
	    ofptr->size = plb.plb_size;
	    ofptr->flags = MFS_NET;
	    bootipaddr = plb.plb_ipaddr;  /* set this for EMT_BOOTIPADDR */

#ifdef DEBUG
printf("open: address of code is %#x, size of code is %#x, returned ip addr is %#x\n",
       plb.plb_addr, plb.plb_size, plb.plb_ipaddr);
#endif

/*
** FIXME: The following code was added for BRASIL which is RFF only.
** This code added to support images that run from devices other than
** main memory.
*/
#ifdef BRASIL
	    /* point imageaddr at the code buffer */
	    imageaddr = (ulong)plb.plb_addr;
#ifdef DEBUG
printf("\nnetboot imageaddr: 0x%x\n", imageaddr);
#endif
            ofptr->devnum = addr2devnum(plb.plb_addr);
#else
            ofptr->devnum = memdevnum;
#endif
	} else {  /* not a netbooted image */
	    if(*filename == SIMPFILECHAR) {
		filename++;  /* bump past this character */
		simpfile = 1;
	    } else simpfile = 0;
	    if(!(devptr->flags & DEV_LIB_LOADED) || simpfile) {
		/* 
		** Read the device info block to determine if there is a
	        ** monlib to load from the device
		*/
		dibptr = &dib;
		/* read in the device magic number (first 4 bytes) */
		if(o_read(devnum, (char *)0, (char *)&magic, sizeof(magic)) < 0) {
		    puts("trouble reading device magic number\n");
		    goto openerr;
		}

#ifdef DEBUG
		printf("device magic number is %x\n", magic);
#endif
#ifdef BRASIL
		    /* point imageaddr at the start of the device */
		    imageaddr = (ulong)devptr->devstart;
#ifdef DEBUG
printf("\nnot netbooted imageaddr: 0x%x\n", imageaddr);
#endif
#endif

		monlib_offset = 0;  /* init */
		switch(magic) {
		case FS_DEVICE_INFO_BLOCK_MAGIC:
		    /* read in entire device info block */
		    if(o_read(devnum, (char *)0, (char *)dibptr,
			      sizeof(struct fs_device_info_block_t)) < 0) {
			puts("trouble reading the device info block\n");
			goto openerr;
		    }
		    /* and pick up the start of monlib fs */
		    monlib_offset = dibptr->monlib_offset;
		    if(!monlib_offset) {
			puts("NULL monlib offset in device info block\n");
			goto openerr;
		    }
		    /* read magic number */
		    if(o_read(devnum, (char *)monlib_offset,
			      (char *)&magic, sizeof(magic)) < 0) {
			puts("trouble reading flash file header on monlib\n");
			goto openerr;
		    }
		    if(magic != FLASH_FHDR_MAGIC) {
			puts("monlib does not contain a valid magic number\n");
			goto openerr;
		    }
		    break;
		case FLASH_FHDR_MAGIC:
		    break;
		default:
		    puts("device does not contain a valid magic number\n");
		    goto openerr;
		}

		ofptr->rfptr = (char *)monlib_offset;
		ofptr->start = ofptr->rfptr;
		if(monlib_offset) {
		    ofptr->size = dibptr->monlib_length;
		/*
		 * The loadprog.c use monlib_upper_bound to check
                 * if the system code override the monlib code
		 */
		    monlib_upper_bound = dibptr->monlib_length;
		}
		else
		    ofptr->size -= (long)ofptr->start;

		if(!simpfile && monlib_offset) {
		    /* load and initialize monlib from this device */
		    if((shstuff.cmdptr = getvar((NVRAM)->evartbl, monlibstr)) == (char *)0) {
			/* use default name or first file in bootflash */
			sprintf(shstuff.argvbuf, "%s%s", devptr->devid, default_monlib_string);
		    } 
		    else {
			sprintf(shstuff.argvbuf, "%s%s", devptr->devid, shstuff.cmdptr);
		    }
		    /* don't want to clobber the environment variable table */
		    shstuff.argv[0] = shstuff.argvbuf;
		    shstuff.cmdptr = shstuff.argv[0];
		    shstuff.argc = 1;

		    /*
		    ** if we are to support arguments to monlib, then we must fully
		    ** parse this command line
		    */

		    clr_dev_lib();  /* clear flags in other device structures */

		    if(loadprog(shstuff.cmdptr, 1, 0, (char **)0) < 0) {  /* load only */
			printf("cannot load the monitor library \"%s\" from device: %s",
			       shstuff.cmdptr, devptr->devname);
			goto openerr;
		    }

		    monlib_upper_bound += entrypt;

		    /*
                    ** update the m_iface for ralib function vector
		    ** launch the monitor library to allow it to initialize
		    ** itself
		    ** pass it the address of our monlib interface structure
		    ** in the process load block
		    */
		    /* monlib_vector_init(); */
		    plb.plb_str = shstuff.cmdptr;  /* point to the name */
		    plb.plb_addr = (char *)&ml_iface;
		    plb.plb_size = sizeof(ml_iface);
		    plb.plb_ipaddr = 0;
		    monlib_launch = 1;    /* prevent PCMCIA card reset */
		    retcode = launch(PROM2_BOOTLOAD, &plb, &pib, entrypt);
		    sys_dirty = 0;   /* launch monlib is not system image */
		    monlib_launch = 0;

		    if(!retcode) {
			/* unsuccessful launch */
			printf("unable to initialize the monitor library \"%s\"\n",
			       shstuff.cmdptr);
			goto openerr;
		    }
		    
		    devptr->flags |= DEV_LIB_LOADED;
		    /*
		    ** monlib is loaded and initialized at this point
		    ** adjust our start, etc. to reflect the file system area
		    ** if valid - if not, leave pointing to monlib fs area
		    */
		    if(dibptr->file_system_offset) {
			ofptr->rfptr = (char *)dibptr->file_system_offset;
			ofptr->start = ofptr->rfptr;
			ofptr->size = dibptr->file_system_length;
		    }
		}
		/*
		** call the open routine in the monlib interface structure
		*/
		if ((devptr->flags & DEV_LIB_LOADED) != 0)
		    filename = path;


		if((ofptr->fsfd = ml_iface.open(filename, mode,
						devnum,
						ofptr->rfptr,
						ofptr->size)) < 0) {
		    printf("open(): Open Error = %d\n", ofptr->fsfd);
		    goto openerr;
		}
		ofptr->flags = MFS_FS;
	    }
	}
    }
    ofptr->mode = mode;
    return(fd);
}

int
o_read(int devnum, char *src, char *bufptr, int numbytes)
{
    int bytesread;
    
    if((bytesread = raw_read(devnum, src, bufptr, numbytes)) != numbytes) {
	printf("open: read error...requested 0x%x bytes, got 0x%x\n",
	       numbytes,bytesread);
	return(-1);
    }
    return(bytesread);
}


/*
** The read system call.
*/

int
read(int fd, char *bufptr, int numbytes)
{
    struct mon_fsio *ofptr = &openfiles[fd];
    int numread;

#ifdef DEBUG
printf("entering read: fd = %d   bufptr = %x   numbytes = %d, devnum = %d\n",
       fd, bufptr, numbytes, ofptr->devnum);
#endif
    if(ofptr->flags & MFS_RAW || ofptr->flags & MFS_NET) {
	numread = raw_read(ofptr->devnum, ofptr->rfptr, bufptr, numbytes);
	if(numread > 0) ofptr->rfptr += numread;
	return(numread);
    }
    return(ml_iface.read(ofptr->fsfd, bufptr, numbytes));
}


/*
** The write system call
*/

int
write(int fd, char *bufptr, int numbytes)
{
    struct mon_fsio *ofptr = &openfiles[fd];

#ifdef DEBUG
printf("entering write: fd = %d   bufptr = %x   numbytes = %d\n",
       fd, bufptr, numbytes);
#endif

    if(ofptr->flags & MFS_RAW || ofptr->flags & MFS_NET) return(-1);
    return(ml_iface.write(ofptr->fsfd, bufptr, numbytes));
}



/*
** Seek to a position within a file.
*/

int
lseek(int fd, long offset, int whence)
{
    struct mon_fsio *ofptr = &openfiles[fd];
    char *ptr, *end;

#ifdef DEBUG
    printf("entering lseek: fd = %d   offset = %d   whence = %d\n",
	   fd, offset, whence);
#endif
    if(ofptr->flags & MFS_RAW || ofptr->flags & MFS_NET) {
	end = ofptr->start + ofptr->size;
	switch(whence) {
	case SEEK_CUR:
	    ptr = ofptr->rfptr + offset;
	    break;
	case SEEK_SET:
	    ptr = ofptr->start + offset;
	    break;
	case SEEK_END:
	    ptr = end + offset;
	    break;
	default:
	    printf("lseek operation not supported\n");
	    return(-1);
	}
	if(ptr >= end || ptr < ofptr->start) {
	    printf("lseek out of bounds\n");
#ifdef DEBUG
printf("sfs_lseek: seeked file pointer = %#x, ", ptr);
printf("file start = %#x, file size = %#x\n", ofptr->start, ofptr->size);
#endif
	} else {
	    ofptr->rfptr = ptr;
	    /* return the current location in file */
	    return(ptr - ofptr->start);
	}
	return(-1);
    }
    return(ml_iface.lseek(ofptr->fsfd, offset, whence));
}


/*
** Close a file.
** Free up the file descriptor.
*/

int
close(int fd)
{
    struct mon_fsio *ofptr = &openfiles[fd];
    int flags;

#ifdef DEBUG
    printf("entering close: fd = %d\n",fd);
#endif

    if(fd >= 0 && fd < MAXOPENFILE && ofptr->flags) {
	flags = ofptr->flags;
	ofptr->flags = 0;  /* close it out */
	if(flags & MFS_RAW || flags & MFS_NET) return(0);
	return(ml_iface.close(ofptr->fsfd));
    }
    return(-1);  /* close error */
}

void
closeall()
{
    int fd;

    for(fd=0; fd < MAXOPENFILE; fd++)
	close(fd);
}

/*
** File status routines stat and fstat
*/

int
stat(char *path, struct stat_ *statptr)
{
    int fd;

    if((fd = open(path, O_RDONLY)) < 0) {
	printf("open failed, cannot stat: %s\n", path);
	return(-1);
    }
    if(fstat(fd, statptr) < 0) {
	printf("fstat failed, cannot stat: %s\n", path);
	close(fd);
	return(-1);
    }
    close(fd);
    return(0);
}

int
fstat(int fd, struct stat_ *statptr)
{
    struct mon_fsio *ofptr = &openfiles[fd];

    if(ofptr->flags & MFS_RAW || ofptr->flags & MFS_NET) {
	statptr->devnum = ofptr->devnum;
	statptr->st_size = ofptr->size;
	statptr->crc = 0;  /* no checksum on raw devices */
	return(0);
    }
    return(ml_iface.fstat(ofptr->fsfd, statptr));
}


/*
** The getdents system call.
*/

int
getdents(int fd, struct dirent_ *dptr, int dirent_len)
{
    struct mon_fsio *ofptr = &openfiles[fd];

    if(ofptr->flags & MFS_RAW || ofptr->flags & MFS_NET)
	return(-1);

#ifdef DEBUG
printf("entering getdirent: fd = %d   dptr = %x\n", fd, dptr);
#endif

    return(ml_iface.getdents(ofptr->fsfd, dptr, dirent_len));
}



