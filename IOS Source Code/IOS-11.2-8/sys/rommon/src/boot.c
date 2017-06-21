/* $Id: boot.c,v 3.2.58.1 1996/03/21 23:26:36 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/boot.c,v $
 *------------------------------------------------------------------
 * boot.c
 *
 * February 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * System boot routines.
 *------------------------------------------------------------------
 * $Log: boot.c,v $
 * Revision 3.2.58.1  1996/03/21  23:26:36  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:19:45  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/08  00:46:30  smaser
 * CSCdi38454:  boot system rom fails
 *
 * Revision 2.2  1995/07/28  20:59:53  clev
 * CSCdi37562:  Bus error in ROM Mon during autobooting
 *
 * Protect against bogus pointer by sanity checking it before use.
 *
 * Revision 2.1  1995/06/07  22:36:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "signal_4k.h"
#include "mon_boot.h"
#include "getdents.h"
#include "dev.h"
#include "fsio.h"
#include "strings.h"
#include "monlib.h"
#include "confreg.h"
#include "fstat.h"
#include "simpfsio.h"
#include "sh.h"
#include "extern.h"

extern int optind;
extern char *optarg;
char boot_satisfied;
char sys_dirty;
/*
 * This flag hsa been added to provide a mechanism for bootload() to inform
 * the C1000 rom monitor that it has detected the "boot system rom" request.
 * and that the rom monitor should go ahead and boot up the
 * rboot image as the running system image. This addresses the issue that the
 * return code ROMBOOT_REQUEST is used by bootload() for two purposes.
 * The first is to communicate that the command "boot system rom" has been has
 * been detected in the system boot command list and the second is to 
 * communicate that none of the specified boot files in the boot command list
 * (either network or flash) could be loaded. In the first case the behaviour
 * should always be to boot the rom resident rboot image. In the second case
 * it might be appropriate to boot either the rom resident rboot image or default
 * to the first boot file in flash depending upon the platform. Since the value
 * of this return code cannot be changed for backward compatability reasons, the
 * flag is used instead.
 */
char boot_sys_rom_flag;

/*
** Call boot through the monlib interface
*/
int
mon_boot(int argc, char *argv[])
{
    return(ml_iface.boot(argc, argv));
}


/*
** The boot loader.
** Prepare a bootstring to be used for the netbooting case.  This
** gets passed to the boot helper program.  It must contain the
** arguments to the boot command minus the boot command itself.
*/
char bootstring[BOOTSTRSIZ];
char boot_verbose;
int boothelper_launchcode, image_launchcode;

int
boot(int argc, char *argv[])
{
    int i, retval;
    static char optstr[] = "nraxv";
    char *cptr, *progname;
    long loadonly = 0;  /* init */
    char loadname[BOOTSTRSIZ];

    boot_verbose = 0;
    image_launchcode = boothelper_launchcode = PROM2_BOOTLOAD;
    while((i = getopt(argc,argv,optstr)) >= 0) {
	switch(i) {
	case 'a':  /* break on any image */
	    boothelper_launchcode = PROM2_BOOTDEBUG;
	    /* fall through */
	case 'n':
	    image_launchcode = PROM2_BOOTDEBUG;
	    break;
	case 'r':
	    boothelper_launchcode = PROM2_BOOTDEBUG;
	    break;
	case 'x':  /* do not execute */
	    loadonly = 1;
	    break;
	case 'v':  /* verbose */
	    boot_verbose = 1;
	    break;
	default:
usage:
	    printf("usage: %s [-%s] [filename] [ipaddr] [args]\n",argv[0],optstr);
	    printf("-x  load but do not execute\n"
		   "-v  verbose\n"
		   "-r  debugger break in boot helper image\n"
		   "-n  debugger break in system image\n"
		   "-a  debugger break in both images\n");
	    return(1);
	}
    }
#ifdef DEBUG
    printf("boot: argc = %d, optind after getopt = %d, argv[optind] = %s\n",
	   argc, optind, argv[optind]);
#endif

    progname = argv[0];  /* save this for later */
    argc = shift(optind, argv);  /* shift out progname and options */

#ifdef DEBUG
    printf("boot: argc = %d, optind after shift = %d, argv[optind] = %s\n",
	   argc, optind, argv[optind]);
#endif

    /*
    ** If the machine is not in a clean [reset] state
    ** tell the user.  Note: sys_dirty gets set on any
    ** operation that may leave the system in a non-bootable
    ** state.
    */
    if(sys_dirty && !loadonly) {
	printf("Please reset before booting\n");
	return(1);
    }

    /*
    ** If no name or device is specified, boot the
    ** default image.
    */
    if(!(argc - optind)) {
	argv[optind] = default_bootdev_id;
	argc++;
    }

    /*
    ** Construct a bootstring for the system image by stringing all args
    ** together (unparse them)
    */
    cptr = bootstring;
    for(i=0; i<argc; i++) cptr += sprintf(cptr,"%s ",argv[i]);
    *(--cptr) = '\0';  /* clobber the space at the end of the line */

    if(boot_verbose)
	printf("boot: bootstring is: \"%s\"\n", bootstring);

    /* make sure these are done */
    sizemainmem();
    sizeshmem();

    /* find the first occurance of a device id in the filename */
    cptr = strchr(argv[optind], DEVSEPARATOR);

    /*
    ** If no filename is specified, boot the first file on device.
    ** This does not account for arguments being passed to the image.
    */
    if(cptr && (*(cptr+1) == '\0' || *(cptr+1) == ' ')) {
	/* no filename specified, only a device id */
	strcpy(loadname, argv[optind]);  /* move to larger buffer */
	if(getfirstfilename(loadname) < 0) {
	    printf("boot: cannot determine first file name on device \"%s\"\n",
		   loadname);
	    return(1);
	}
	argv[optind] = loadname;
    }

    /*
    ** Record boot info for later use by system image.
    ** This info will be updated by a boothelper image, if one is used
    ** in the boot process.
    */
    i = getdevnum(argv[optind]);
    if(i < 0) {
	bootinfo.protocol = FILE_ACCESS_UNKNOWN;
    } else {
	bootinfo.protocol = devtbl[i].devfap;
    }
    strncpy(bootinfo.filename, argv[optind], BINFO_FLEN);

    boot_satisfied = 0;  /* init */

    /*
    ** Load the program.
    */
    retval = loadprog(argv[optind], loadonly, argc, argv);
    boot_verbose = 0;
    if(retval < 0) {
	printf("boot: cannot load \"%s\"\n", bootstring);
	return(1);
    }
    return(retval);
}

/*
** Get the name of the first file on specified device.
** Caller passes pointer to buffer which must contain
** the device ID with null termination only.  There must be
** sufficient room in the buffer to hold the file name.
** Return -1 if the operation is unsuccessful.
*/
int
getfirstfilename(char *string)
{
    int fd;
    int devchars;
    struct dirent_ dir;

    devchars = strlen(string);  /* get number of chars in device id */
    strcat(string, ".");  /* append a period to read directory */
    fd = open(string, O_RDONLY);
    string[devchars] = '\0';  /* null terminate the device id again */
    if(fd < 0) {
	printf("boot: cannot open \"%s\"\n", string);
	return(-1);
    }
    while(1) {
	if(!getdents(fd, &dir, sizeof(dir))) {
	    close(fd);
	    return(-1);
	}
	/*
	** make sure the file is not deleted and
	** has a good checksum
	*/
	if(dir.flags & FFH_FLAGS_DELETED && /* inverse logic */
	   dir.crc == dir.achecksum)
	    break;  /* found good file */
    }
    /* combine device and file name */
    strcat(string, dir.d_name);
    close(fd);
    return(0);
}


/*
** Support autobooting for Sierra as per Ashwin's document.
*/
int
autoboot(void)
{
    int i;
    short state;
    char *cptr, *bptr, c;
    long numtries;
    jmp_buf jmpbuf;
    char cmdbuf[BOOTSTRSIZ + sizeof(MON_BOOT_CMD) + 2];
    struct shstuff shstuff;
    int bconf = (NVRAM)->confreg & BOOT_FIELD;

#ifdef DEBUG
    printf("ROMMON AUTOBOOT: bootconf: %#x, autobootstate: %d, autobootcount: %d\n",
       bconf,(NVRAM_SAV)->autobootstate,(NVRAM_SAV)->autobootcount);
#endif
    if(bconf == 0) return(1);  /* nothing to do */
    if(bconf == 1) {
	state = 3;     /* fudge this to make the right thing happen */
    } else {
	state = (NVRAM_SAV)->autobootstate;
	(NVRAM)->autobootstate = state + 1;
	/* write incremented value back to NVRAM */
	nvwrite(&(NVRAM)->autobootstate, &(NVRAM_SAV)->autobootstate,
		sizeof((NVRAM)->autobootstate));
    }
    /* set up for the boot process to handle fatal exceptions */
    if(!setjmp(jmpbuf)) {
	monjmpptr = &jmpbuf;
	i = sprintf(cmdbuf, "%s ", MON_BOOT_CMD);
	switch(state) {
	case 0:
	    /* first pass the default image name - no retries */
	    sprintf(&cmdbuf[i], CISCO_FORMAT, bconf, platform_string);
	    break;
	case 1:
	    /*
	    ** Next parse the BOOT environment variable for all flash commands
	    ** no retries
	    ** sanity check autobootsptr first - it should be in evartbl
	    ** There have been cases on catwoman where this pointer has been
	    ** found set to -1.  The root cause is still under investigation.
	    ** When found this code should print an error message if
	    ** autobootsptr is not sane.
	    */
	    if((NVRAM_SAV)->autobootsptr >= (NVRAM)->evartbl &&
	       (NVRAM_SAV)->autobootsptr < &(NVRAM)->evartbl[EVTSIZ])
		cptr = (NVRAM_SAV)->autobootsptr;
	    else cptr = getvar((NVRAM)->evartbl, AUTOBOOTVAR);
#ifdef DEBUG
	    printf("Autoboot state 1 BOOT variable returns: \"%s\"\n",cptr);
#endif
	    bptr = bootstring;  /* use our bootstring buffer temporarily */
	    while(cptr) {  /* valid AUTOBOOTVAR */
		c = *cptr++;
		switch(c) {
		case ',':
		    /* gobble up the number even though we ignore it */
		    cptr += getnum(cptr, 10, &numtries);
		    /* cptr should now point to either a ';' or a '\0' */
		    continue;
		case ';':
		    /* cptr is already bumped past the ';' */
		    /* fall through */
		case '\0':
		    *bptr = '\0';  /* terminate the line */
		    if(!c) cptr = (char *)0;  /* break the loop */
		    break;
		default:
		    *bptr++ = c;  /* copy character to buffer */
		    continue;  /* copy entire string segment */
		}
		if(strncmp(bootstring, DEV_FLASH_ID, strlen(DEV_FLASH_ID)) != 0) {
		    /* not a flash command */
		    bptr = bootstring; /* start fresh for next try */
		    *bptr = '\0';
		    continue;          /* try the next one */
		}
		/*
		** We now have a flash command in the bootstring buffer.
		** Try to boot using this info.
		** Ignore the numtries when booting from flash.
		*/
		strcat(cmdbuf, bootstring);  /* add to command buffer */
		break;
	    }
#ifdef DEBUG
	    printf("Autoboot state 1 command buffer is: \"%s\"\n",cmdbuf);
#endif
	    if(cmdbuf[i] != '\0') {  /* break if we have a valid command */
		(NVRAM)->autobootsptr = cptr;
		(NVRAM)->autobootstate = 1;  /* compensate for above increment */
		/* write back out to NVRAM */
		nvwrite(&(NVRAM)->autobootstate, &(NVRAM_SAV)->autobootstate,
			sizeof((NVRAM)->autobootstate + (NVRAM)->autobootsptr +
			       (NVRAM)->autobootcount));
		break;
	    }
	    clr_autoboot_state();
	    (NVRAM)->autobootstate = 3;
	    /* write back out to NVRAM */
	    nvwrite(&(NVRAM)->autobootstate, &(NVRAM_SAV)->autobootstate,
		    sizeof((NVRAM)->autobootstate));
	    /* fall through */
	case 2:
	    /* boot first file in flash - no retries */
	    strcat(cmdbuf, DEV_FLASH_ID);
	    break;
	case 3:
	    /*
	    ** default action is to boot from the default bootdev
	    ** try only once
	    */
	    if((NVRAM_SAV)->autobootcount < 1) {
		strcat(cmdbuf, default_bootdev_id);
		break;
	    }
	    /* fall through */
	default:
	    clr_autoboot_state();
	    return(1);
	}
	shstuff.cmdptr = cmdbuf;
	shcmdline(&shstuff);             /* execute it */
    }
    /*
    ** If the boot fails due to the system image returning
    ** or an exception we will end up here.
    ** If the boot was aborted due to user interrupt return to
    ** caller, else issue a reset to the platform which will
    ** cause autoboot to try the next state or the next retry.
    */
    if(hkeepflags & H_USRINT) {
	clr_autoboot_state();
	return(1);
    }
    /*
     * Check to see if the boot helper returned with a message to
     * "boot system rom".
     */
    if  (boot_sys_rom_flag){
#ifdef DEBUG
	printf("\n ROMMON AUTOBOOT: setting autoboot state to boot of rboot image. ");
#endif
        (NVRAM)->autobootstate = 3;
        nvwrite(&(NVRAM)->autobootstate, &(NVRAM_SAV)->autobootstate,
	    sizeof((NVRAM)->autobootstate));
	boot_sys_rom_flag = 0;
  
    }
    if(state >= 3) {
	(NVRAM)->autobootcount = (NVRAM_SAV)->autobootcount + 1;
	/* write back out to NVRAM */
	nvwrite(&(NVRAM)->autobootcount, &(NVRAM_SAV)->autobootcount,
		    sizeof((NVRAM)->autobootcount));
    }
    do_reset();
}

void
clr_autoboot_state()
{
    (NVRAM)->autobootstate = 0;  /* reset these */
    (NVRAM)->autobootcount = 0;
    (NVRAM)->autobootsptr = 0;
    /* write back out to NVRAM */
    nvwrite(&(NVRAM)->autobootstate, &(NVRAM_SAV)->autobootstate,
	    sizeof((NVRAM)->autobootstate + (NVRAM)->autobootsptr +
		   (NVRAM)->autobootcount));
}

static char *reason_str[] = {
    "reset",
    "user break",
    "breakpoint",
    "",  /* exit - will never happen here */
    "",  /* trace - will never happen here */
    "",  /* emt - will never happen here */
    "bus error",
    "",  /* address error - will never happen here */
    "watchdog timeout",
    "user reload",
    "parity error",
    "shared memory parity error",
    "unexpected interrupt",
};

/*
** Dump out the sys_ret_info for the user
*/
int
sysretdump(int argc, char *argv[])
{
    int i;

    printf("System Return Info:\n"
	   "count: %d,  reason: %s\n"
	   "pc:%#x,  error address: %#x\n"
	   "Stack Trace:\n",
	   (NVRAM)->sys_ret_info[BOOT_COUNT],
	   (*reason_str[(NVRAM)->sys_ret_info[BOOT_REASON]] == '\0') ?
	   "unknown" : reason_str[(NVRAM)->sys_ret_info[BOOT_REASON]],
	   (NVRAM)->sys_ret_info[BOOT_PC],
	   (NVRAM)->sys_ret_info[BOOT_ADDRESS]);

    for(i=0; i<(BOOT_STACK_FRAMES * 2); i+=2) {
	printf("FP: %#.8x, PC: %#.8x\n",
	       (NVRAM)->sys_ret_info[BOOT_STACK_TRACE + i],
	       (NVRAM)->sys_ret_info[BOOT_STACK_TRACE + 1 + i]);
    }
}
