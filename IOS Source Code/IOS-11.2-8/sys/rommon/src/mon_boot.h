/* $Id: mon_boot.h,v 3.2.62.1 1996/06/16 21:18:19 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/mon_boot.h,v $
 *------------------------------------------------------------------
 * mon_boot.h
 *
 * April 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Monitor boot defines.
 *------------------------------------------------------------------
 * $Log: mon_boot.h,v $
 * Revision 3.2.62.1  1996/06/16  21:18:19  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2  1995/11/17  18:42:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/08  00:46:32  smaser
 * CSCdi38454:  boot system rom fails
 *
 * Revision 2.1  1995/06/07  22:37:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __MON_BOOT_H__
#define __MON_BOOT_H__

#ifndef ASMINCLUDE

/*
** Format of a process load block (plb)
*/

struct plb {
	char *plb_str;			/* filename or error string */
	char *plb_addr;			/* starting address */
	long plb_size;			/* size in bytes */
	long plb_ipaddr;		/* IP address of host */
};

/* backwards compatible with original ROM Mon (taken from prom2.h) */
#define	PROM2_VERSION	3		/* request boot helper version */
#define	PROM2_BOOTLOAD	2		/* boot  */
#define PROM2_BOOTDEBUG 4		/* boot and enter debug mode */
#define PROM2_DEBUG	5		/* set up debugger and return */

/*
** The process interface block (pib).
**
** This structure is accessed from both C and assembler (scrt0.S).
** If the structure is changed, the defines must be changed accordingly.
*/
struct pib {
    struct mon_iface *mifp;   /* pointer to monitor interface structure */
    int argc;                 /* for argc,argv mechanism */
    char **argv;
};
#endif /* ASMINCLUDE */

#define PIB_MIFP   0
#define PIB_ARGC   4
#define PIB_ARGV   8
#define BOOTSTRSIZ 128

/* define a environment variable for autobooting */
#define AUTOBOOTVAR "BOOT"

/* define the monitor boot command */
#define MON_BOOT_CMD "boot"

/* define the default filename for booting */
#define CISCO_FORMAT		"cisco%o-%s"

/* defines for uncompressing files */
#define UNCOMPRESS_DATASIZE 220000

/*
** Same as define in sys/os/boot.h.  Could not include that file
** due to system prototypes.
*/
#ifndef ASMINCLUDE

#define BINFO_FLEN     64
#define BINFO_MOPADDR   6
#define BINFO_MOPIF    20
#define BINFO_MOPHOST  16
/* 
 * (stolen from sys/os/boot.h)
 */
struct bootinfo {
    int	protocol;	          /* Protocol identifier */
    char filename[BINFO_FLEN];	  /* Boot filename */
    long ipaddr;		  /* If booted from IP, host address */
    char mopaddr[BINFO_MOPADDR];  /* If booted from MOP, boot address */
    char mopif[BINFO_MOPIF];	  /* If booted from MOP, interface name */
    char mophost[BINFO_MOPHOST];  /* If booted from MOP, host name */
};

/*
 * A list of file access methods.
 * (stolen from sys/h/file.h )
 */
typedef enum {
    FILE_ACCESS_UNKNOWN = 0,
    FILE_ACCESS_TFTP = 1,
    FILE_ACCESS_MOP = 2,
    FILE_ACCESS_FLASH = 3,
    FILE_ACCESS_ROM = 4,
    FILE_ACCESS_RCP = 5,
    FILE_ACCESS_SLOT0 = 6,
    FILE_ACCESS_SLOT1 = 7
} file_access_protocol;


/* related prototypes */
extern int mon_boot(int argc, char *argv[]);
extern int boot(int argc, char *argv[]);
extern int getfirstfilename(char *string);
extern int autoboot(void);
extern void clr_autoboot_state(void);
extern int loadprog(char *loadname, int loadonly, int argc, char *argv[]);
extern unsigned long entrypt;
extern long loadsize;
extern long bootipaddr;
extern char bootstring[BOOTSTRSIZ];
extern char boot_satisfied, sys_dirty;
extern char boot_verbose;
extern char boot_sys_rom_flag;
extern int boothelper_launchcode, image_launchcode;
extern unsigned long imageaddr;
extern struct bootinfo bootinfo;
#endif /* ASMINCLUDE */

#endif  /* __MON_BOOT_H__ */
