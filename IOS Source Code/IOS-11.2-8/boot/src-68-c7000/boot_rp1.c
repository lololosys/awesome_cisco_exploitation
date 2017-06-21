/* $Id: boot_rp1.c,v 3.1.58.1 1996/03/18 18:34:44 gstovall Exp $
 * $Source: /release/111/cvs/Xboot/src-68-c7000/boot_rp1.c,v $
 *------------------------------------------------------------------
 * Support for RP1 specific booting 
 *
 * January 1995, Stig Thormodsrud
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: boot_rp1.c,v $
 * Revision 3.1.58.1  1996/03/18  18:34:44  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.26.1  1996/03/16  06:10:28  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1  1995/11/09  09:09:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:02  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.1  1995/05/02  23:22:18  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:26  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "defs.h"
#include "prom2.h"
#include "stdio.h"
#include "ctype.h"
#include "reason.h"
#include "extern.h"
#include "cpu.h"
#include "dev_slot0.h"  
#include "nvmonvars.h"

#define BINFO_FLEN     64
#define BINFO_MOPADDR   6
#define BINFO_MOPIF    20
#define BINFO_MOPHOST  16
/* 
 * (stolen from sys/os/boot.h)
 */
struct bootinfo_t_ {
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

/*
 * Boot type: direct indicates that the rom monitor can boot the image
 * without help from an rxboot image (i.e. flash & slot0).
 */
#define INDIRECT_BOOT  1       /* need an rxboot image to help out */
#define DIRECT_BOOT    2
#define      DIRECT_INVALID  0
#define      DIRECT_ROM      1
#define      DIRECT_FLASH    2
#define      DIRECT_SLOT0    3

/*
 * strings indexed by defines DIRECT_[ROM|FLASH|SLOT0]
 */
char *method_str[] = {"invalid",
		      "rom",
		      "flash",
		      "slot0"};
/* 
 * Forward reference
 */
int boot_type(char *);
int boot_method(char *);

/*
 * bootload_rp1
 *
 * called from the common bootload() to deal with the RP1s requirement
 * to be able to load different rxboot images.
 *
 * note: the use of the 'recurse' parameter is silly hack to get
 * around the fact that we can't use initialized static variables
 * because they get put in ROM and the processor gets uptight about
 * trying to write a new value to a ROM variable.  
 */
long bootload_rp1 (char *filename, int flags, int entry_code, struct
		   plb_t *p, boolean recurse)
{
    ulong size;
    int device, result, type, method;
    char *dirent, *name;
    struct bootinfo_t_ *bootinfo;
    char *first = "first file";
    boolean debug_boot;

    debug_boot = debug(DEBUG_BOOT);

    if (debug_boot) {
	printf("boot: %sbootload_rp1(%s, %x, %x, %x)\n", 
	       recurse ? "recursing " : "",
	       *filename ? filename : "NULL", 
	       flags, 
	       entry_code, 
	       p);
    }

    size = 0;    
    name = NULL;
    type = boot_type(filename);
    if (debug_boot)
	printf("boot: type %sdirect", type == DIRECT_BOOT ? "": "in");
    
    switch (type) {
    case INDIRECT_BOOT:
	/* 
	 * At this point we know that we need the help of a rxboot
	 * image.  First check if the BOOTLDR environment variable is
	 * set, if not try the ROM image.  To load the image we'll
	 * recursively call ourselves with the name of a rxboot image
	 * that can be directly booted.
	 */
	name = (char *)getvar(nvram->evartbl,"BOOTLDR");
	if (!name || !*name) 
	    name = "rom";
	if (debug_boot)
	    printf(", booting [%s] via [%s] rxboot\n", filename,
		   name);
	/* 
	 * need to check that "name" isn't of type DIRECT_BOOT or else
	 * we have a nasty infinite recursive routine.
	 */
	if (boot_type(name) == INDIRECT_BOOT) {
	    printf("Invalid rxboot image, using ROM rxboot\n");
	    name = "rom";
	}
	size = bootload_rp1(name, 0, 0, NULL, TRUE);
	if (!size) {
	    /* 
	     * Specified rxboot failed, so lets try rom.  I suppose we
	     * could check that the specified rxboot wasn't ROM so
	     * that we don't try it again, but think of this a second
	     * chance. 
	     */
	    printf("Failed to load [%s] rxboot, "
		   "attempting to load ROM rxBoot\n", name); 
	    size = (ulong)bootload_rp1("rom", 0, 0, NULL, TRUE);
	    if (!size) 
		return 0;
	}
	/* set the file to be booted by the rxboot image boot */
	p->plb_str = filename;
	if (~config.fastboot & 1) {
	    /*
	     * Force the rxboot image to ignore the config and do as I
	     * say in "plb_str".
	     */
	    if (debug_boot)
		printf("Setting plb_option to BOOTIGNORECONFIGBITS\n");
	    p->plb_option = BOOTIGNORECONFIGBITS;
	}
	result = 1;
	break;
    case DIRECT_BOOT:
	method = boot_method(filename);
	name = (char *)strchr(filename, ':');
	if (name) {
	    name++; /* skip over ':' */
	    /* 
	     * If we have just a device without a filename, then we
	     * should set 'name' to NULL so that we try the first file
	     * on the device.
	     */
	    if (*name == '\0' || next_char(name) == '\0') 
		name = NULL;
	} 

	if (debug_boot)
	    printf(", method %s\n", method_str[method]);

	if (recurse) {
	    if (method != DIRECT_ROM) {
		printf("Loading rxboot [%s] via %s\n", 
		       name ? name : "first file", 
		       method_str[method]);
	    } 
	}
	
	switch (method) {
	case DIRECT_ROM:
	    p = NULL;
	    if (debug_boot) printf("boot: booting ROM image\n");
	    /*
	     * Run from ROM (MAGIC1), uncompress to RAM (MAGIC2), or
 	     * copy to RAM (MAGIC3), then netboot a file if (p != NULL).
	     */
	    switch (*prom2) {
	    case PROM2_MAGIC1:
		printf("Unsupported run from ROM\n");
		return 0;
		break;
	    case PROM2_MAGIC2:
		if (!(size = uncompress(prom2, baseaddr)))
		    return(0);
		if (debug_boot)
		    printf("boot: uncompressing rom - %d\n", size);
		break;
	    case PROM2_MAGIC3:
		if (!(size = unpack(prom2, baseaddr)))
		    return(0);
		if (debug_boot)
		    printf("boot: unpacking rom - %d\n", size);
		break;
	    default:
		printf("\nBad prom2 magic number - %x\n", *prom2);
		return(0);
	    }
	    if (recurse) {
		return size;
	    }
	    result = 1;
	    break;
	case DIRECT_FLASH:
	    if (debug_boot)
		printf("boot: booting [%s] via internal flash\n", 
		       name ? name : first);
	    dirent = (char *)flash_find_file(name);
	    if (!dirent) {
		printf("Couldn't open [%s] on flash:\n", 
		       name ? name : first);	    
		return 0;
	    }
	    size = flash_file_size(dirent);
	    if (!recurse) {
		/*
		 * This is "normally" set by the rxboot image after it
 		 * reads the to-be-booted image out of flash.  Since
		 * we are direct booting here, we have to set the
		 * bootinfo stuff too.
		 */
		bootinfo = (struct bootinfo_t_ *)boot_information;
		bootinfo->protocol = FILE_ACCESS_FLASH;
		strncpy(bootinfo->filename, 
			flash_file_name(dirent), BINFO_FLEN);
	    }
	    if (!flash_copy_to_ram(dirent, baseaddr, size)) 
		return 0;
	    printf("\n");
	    if (!recurse) {
		p = NULL;
	    } else {
		/* 
		 * return from recursive call
		 */ 
		return size;
	    }
	    result = 1;
	    break;
	case DIRECT_SLOT0:
	    if (debug_boot)
		printf("boot: booting [%s] via slot0:\n", 
		       name ? name : first);
	    dirent = (char *)flash_card_find_file(name);
	    if (!dirent)  {
		printf("Couldn't open [%s] on slot0:\n", 
		       name ? name : first);
		return 0;
	    }
	    size = flash_card_file_size(dirent);
	    if (!recurse) {
		/*
		 * This is "normally" set by the rxboot image after it
		 * reads the to-be-booted image out of flash.  Since
		 * we are direct booting here, we have to set the
		 * bootinfo stuff too.
		 */
		bootinfo = (struct bootinfo_t_ *)boot_information;
		bootinfo->protocol = FILE_ACCESS_SLOT0;
		strncpy(bootinfo->filename,
			flash_card_file_name(dirent), BINFO_FLEN);
	    }
	    if (!flash_card_copy_to_ram(dirent, (ulong)baseaddr, size))
		return 0;
	    printf("\n");
	    if (!recurse) {
		p = NULL; 
	    } else {
		/* 
		 * return from recursive call
		 */ 
		return size;
	    }
	    result = 1;
	    break;
	}
	break;
    }

    monlib_initialized = FALSE;
    sys_dirty = TRUE;

    if (!setup(baseaddr, size)) {
	/* 
	 * If we get here, then either our direct booted image or our
         * rxboot image is an unknown format.
	 */
	if (debug_boot)
	    printf("boot: setup(%x, %d) == NULL\n", baseaddr, size);
	return(0);
    }

    system_state = p ? SYSTATE_BOOTING : SYSTATE_RUNNING;
    if (debug_boot)
	printf("boot: system_state [%s], ", 
	       p ? "SYSTATE_BOOTING" : "SYSTATE_RUNNING"); 
    if (system_state == SYSTATE_BOOTING || flags & BOOTRUNIMAGE) {
	restore_vectors();
	if (debug_boot)
	    printf("ram_dispatch(%d, 0x%x, 0x%x)\n",
		   entry_code, p, baseaddr); 
	result = ram_dispatch(entry_code, p, baseaddr);
	/*
	 * Since we returned from ram_dispatch(), we were probably
	 * trying to netboot some image.
	 */
	if (debug_boot) {
	    printf("boot: ram_dispatch() = %d\n",result);
	    if (p) {
		printf("boot:\tstr [%s]\n", p->plb_str);
		printf("boot:\taddress %x\n",p->plb_addr);
		printf("boot:\tsize %d\n",p->plb_size);
		printf("boot:\tipaddr %x\n",p->plb_ipaddr);
		printf("boot:\toptions %x\n",p->plb_option);
		printf("boot:\tresult %x\n",p->plb_result);
	    }
	}
	return result;
    } else {
	if (debug_boot)
	    printf("no dispatch(), returning 0x%x\n", baseaddr);
	return (long)baseaddr;
    }
}

/*
 * boot_type
 *
 * Based on the filename, try to determine if the image can be
 * directly booted or if it will require a rxboot to do the dirty
 * work. 
 */
int boot_type (char *filename)
{
    char *ch;
    char *tmp;

    if (!filename || *filename == '\0')   /* rom */
	return DIRECT_BOOT;   
    
    ch = "rom";
    if (strncmp(filename,ch,strlen(ch)) == 0)
	return DIRECT_BOOT;

    /*
     * look for device separator
     */
    ch = (char *)strchr(filename,':'); 
    if (!ch)
	return INDIRECT_BOOT;

    tmp = "flash:";
    if (strncmp(filename, tmp, strlen(tmp)) == 0) 
	return DIRECT_BOOT;
    tmp = "slot0:";
    if (strncmp(filename, tmp, strlen(tmp)) == 0) 
	return DIRECT_BOOT;
    tmp = "tftp:";
    if (strncmp(filename, tmp, strlen(tmp)) == 0) 
	return INDIRECT_BOOT;
   
    /*
     * check for valid device
     */
    return INDIRECT_BOOT;
}

/*
 * boot_method
 *
 * Determine if "filename" can be direct booted or if it requires a
 * rxboot image.
 */
int boot_method (char *filename)
{
    char *tmp;

    if (!filename || *filename == '\0')
	return DIRECT_ROM;

    tmp = "rom";
    if (strncmp(filename,tmp,strlen(tmp)) == 0) 
	return DIRECT_ROM;

    tmp = "flash:";
    if (strncmp(filename,tmp,strlen(tmp)) == 0) 
	return DIRECT_FLASH;

    tmp = "slot0:";
    if (strncmp(filename,tmp,strlen(tmp)) == 0) 
	return DIRECT_SLOT0;

    return DIRECT_INVALID;
}

#define DEVSEP   ':'
#define FILESEP  ';'
#define PARMSEP  ','

char *autoboot (void)
{
    char *bootvar, *filename;
    char *tmp, *start, *end, *bootsptr;
    short bootstate;
    boolean debug_boot, fastboot;

    debug_boot = debug(DEBUG_BOOT);
    fastboot = ~config.fastboot & 1;
    if (debug_boot)
	printf("boot: FASTBOOT %sabled\n", fastboot ? "en" : "dis");

    /*
     * If "fastboot" is NOT defined, then return NULL and boot the old
     * way with an rxboot image.
     */
    if (!fastboot)
	return NULL;

    bootvar = (char *)getvar(nvram->evartbl, "BOOT");
    bootstate = nv_read_short(&nvram->autobootstate);
    bootsptr = (char *)nv_read_long(&nvram->autobootsptr);

    if (!bootvar || bootvar[0] == 0) {
	if (debug_boot)
	    printf("boot: no BOOT var defined\n");
	bootstate = 2;
    } 

 begin_switch:

    switch (bootstate) {
    case 0:
	nv_write_short((ushort *)&nvram->autobootstate, 1);
	break;
    case 1:
	/* 
	 * continue trying images from the BOOT variable
	 */
	if (bootsptr) 
	    bootvar = bootsptr;
	if (*bootvar == FILESEP)
	    bootvar++; 
	break;
    case 2:
	/*
	 * try booting 1st file in flash
	 */
	nv_write_short((ushort *)&nvram->autobootstate, 3);
	strcpy(bootfilename, "flash:");
	return bootfilename;
    case 3:
	/*
	 * try booting 1st file on flash credit card
	 */
	nv_write_short((ushort *)&nvram->autobootstate, 4);
	strcpy(bootfilename, "slot0:");
	return bootfilename;
    case 4:
	/*
	 * try booting rom image
	 */
	nv_write_short((ushort *)&nvram->autobootstate, 5);
	strcpy(bootfilename, "rom");
	return bootfilename;
    case 5:
    default:
	return NULL;
    }
    
    end = (char *)strchr(bootvar, FILESEP);

    if (!end || end == bootvar) {
	if (debug_boot)
	    printf("boot: end of BOOT string\n");
	bootstate = 2;
	goto begin_switch;
    }
	
    if (end) {
	*end = '\0';
	switch (boot_type(bootvar)) {
	case DIRECT_BOOT:
	    strncpy(bootfilename, bootvar, FNAME_LEN);
	    *end = FILESEP; /* repair nvram */	    
	    nv_write_long((ulong *)&nvram->autobootsptr, (ulong)end);
	    tmp = "rom:";
	    if (strncmp(bootfilename, tmp, strlen(tmp)) == 0) {
		strcpy(bootfilename,"rom");
	    }
	    if ((end=(char *)strchr(bootfilename, PARMSEP)))
		*end = '\0';
	    if (debug_boot)
		printf("boot: autoboot trying [%s]\n", bootfilename);
	    return(bootfilename);
	case INDIRECT_BOOT:
	default:
	    start = bootvar;
	    tmp = "tftp:";
	    if (strncmp(tmp, bootvar, strlen(tmp)) == 0) {
		start = (char *)strchr(bootvar, DEVSEP);
		start++;
	    } 
	    strncpy(bootfilename, start, FNAME_LEN);
	    *end = FILESEP;  /* repair nvram */
	    nv_write_long((ulong *)&nvram->autobootsptr, (ulong)end);
	    end = (char *)strchr(bootfilename, PARMSEP);
	    if (end)
		*end = '\0';
	    if (debug_boot)
		printf("boot: autoboot() [%s] requires rxboot\n", 
		       bootfilename);
	    return bootfilename;
	}
    } else {
	printf("Incorrect format [%s]\n", bootvar);
	return NULL;
    }
}

void reset_autoboot (void)
{
    if (debug(DEBUG_BOOT))
	printf("boot: reset autoboot\n");

    nv_write_short(&nvram->autobootstate, 0);
    nv_write_short(&nvram->autobootcount, 0);
    nv_write_long((ulong *)&nvram->autobootsptr, 0);

}

void default_rxboot (void)
{
    if (debug(DEBUG_BOOT))
	printf("boot: changing BOOTLDR from [%s] to default\n",
	       getvar(nvram->evartbl, "BOOTLDR"));
    unsetvar(nvram->evartbl,"BOOTLDR");
    savenv();
}

void show_autoboot (void)
{
    short bootstate, bootcount;
    char *bootsptr;

    bootstate = nv_read_short(&nvram->autobootstate);
    bootcount = nv_read_short(&nvram->autobootcount);
    bootsptr = (char *)nv_read_long(&nvram->autobootsptr);

    printf("BOOT = [%s]\n", getvar(nvram->evartbl,"BOOT"));
    printf("bootstate = %d\n", bootstate);
    printf("bootcount = %d\n", bootcount);
    printf("bootsptr  = [%s]\n", bootsptr);
}




