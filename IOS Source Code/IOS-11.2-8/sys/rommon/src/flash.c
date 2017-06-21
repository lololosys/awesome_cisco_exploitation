/* $Id: flash.c,v 3.2.58.2 1996/06/01 02:45:35 clev Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/flash.c,v $
 *------------------------------------------------------------------
 * flash.c
 *
 * January 1993, Rob Clevenger
 *
 * Copyright (c) 1995 - 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Universal flash routines.
 *------------------------------------------------------------------
 * $Log: flash.c,v $
 * Revision 3.2.58.2  1996/06/01  02:45:35  clev
 * CSCdi55487:  Add support for AMD 008 and 016 flash SIMMs
 * Branch: California_branch
 *
 * Fujitsu chips too.
 *
 * Revision 3.2.58.1  1996/03/21  23:27:12  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/02/21  00:56:50  szhang
 * CSCdi49417:  Flash Manufacturing Code Read Error
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/12/01  04:20:17  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Universal flash routines.
** This code will handle all of the different flash devices and geometry
** currently in cisco products (1/93).
*/
#include "proto.h"
#include "monitor.h"
#include "mon_defs.h"
#include "flash.h"
#include "signal_4k.h"
#include "error.h"
#include "bitdef.h"
#include "dev.h"
#include "monlib.h"
#include "dib.h"

#define MAXFLASHERASE 3000

#define ERASECMD \
	(C_ERASE | (C_ERASE << 8) | (C_ERASE << 16) | (C_ERASE << 24))
#define ERASECONFIRM \
	(C_ERASE_CONFIRM | (C_ERASE_CONFIRM << 8) | \
	(C_ERASE_CONFIRM << 16) | (C_ERASE_CONFIRM << 24))
#define ERASESTAT \
	(C_ERASE_STAT | (C_ERASE_STAT << 8) | (C_ERASE_STAT << 16) | \
	(C_ERASE_STAT << 24))
#define ERASEVER \
	(C_ERASE_VERIFY | (C_ERASE_VERIFY << 8) | (C_ERASE_VERIFY << 16) | \
	(C_ERASE_VERIFY << 24))
#define WRSTATRDY \
	(C_WR_STAT_RDY | (C_WR_STAT_RDY << 8) | (C_WR_STAT_RDY << 16) | \
	(C_WR_STAT_RDY << 24))
#define BYTEWRITE \
	(C_BYTE_WRITE | (C_BYTE_WRITE << 8) | (C_BYTE_WRITE << 16) | \
	(C_BYTE_WRITE << 24))
#define BYTEWRSTAT \
	(C_BYTE_WR_STAT | (C_BYTE_WR_STAT << 8) | (C_BYTE_WR_STAT << 16) | \
	(C_BYTE_WR_STAT << 24))
#define CLRSTATREG \
	(C_CLEAR_STAT_REG | (C_CLEAR_STAT_REG << 8) | \
	(C_CLEAR_STAT_REG << 16) | (C_CLEAR_STAT_REG << 24))
#define READMEM \
	(C_READ_MEMORY | (C_READ_MEMORY << 8) | (C_READ_MEMORY << 16) | \
	(C_READ_MEMORY << 24))
#define RDSTATCMD \
	(C_READ_STAT_REG | (C_READ_STAT_REG << 8) | (C_READ_STAT_REG << 16) | \
	(C_READ_STAT_REG << 24))
#define PROGCMD \
	(C_PROGRAM | (C_PROGRAM << 8) | (C_PROGRAM << 16) | (C_PROGRAM << 24))
#define PROGVER \
	(C_PROGRAM_VERIFY | (C_PROGRAM_VERIFY << 8) | \
	(C_PROGRAM_VERIFY << 16) | (C_PROGRAM_VERIFY << 24))
#define RESETCMD \
	(C_RESET | (C_RESET << 8) | (C_RESET << 16) | (C_RESET << 24))
#define VPPSTAT \
	(C_VPP_STAT | (C_VPP_STAT << 8) | (C_VPP_STAT << 16) | \
	(C_VPP_STAT << 24))
#define READID \
        (C_READ_ID_CODES | (C_READ_ID_CODES << 8) | (C_READ_ID_CODES << 16) | \
	(C_READ_ID_CODES << 24));

long tenmsecs, tenusecs, sixusecs;
long total_flash;
extern int netflashbooted;
struct flash_dvr_info *g_fdiptr;  /* global ptr for flash cleanup routine */

/*
** The flash_id structure.
**
** This structure holds all of the information for the flash devices
** supported by the flash driver routines herein.
*/

struct flash_id dib_set;

struct flash_id flash_ids[] = {
    0x89B4, 0x912d, 0x10000,  0x10000, "INTEL 28F010",   0, 
    0x89BD, 0x91BD, 0x20000,  0x20000, "INTEL 28F020",   0, 
    0x01A7, 0x80E5, 0x10000,  0x10000, "AMD 28F010",     0, 
    0x012A, 0x8054, 0x20000,  0x20000, "AMD 28F020",     0, 
    0x1CD0, 0x380D, 0x20000,  0x20000, "M5M28F101P",     0, 
    0x89A2, 0x9145, 0x40000,  0x10000, "INTEL 28F008SA", FI_STATREG,
    0x89A0, 0x9105, 0x200000, 0x10000, "INTEL 28F016SA", FI_STATREG,
    0x01D5, 0x80AB, 0x100000,0x10000, "AMD 29F080",     AMD_STATREG,
    0x01AD, 0x80B5, 0x200000,0x10000, "AMD 29F016",     AMD_STATREG,
};

#define MAXFLIDS (sizeof(flash_ids)/sizeof(struct flash_id))


struct bitdef flash_stat[] = {
    1," bit:7 unsued                          \n",
    1," bit:6 unsued                          \n",
    1," bit:5 unsued                          \n",
    1," bit:4 Vpp low detected                \n",
    1," bit:3 error in byte write             \n",
    1," bit:2 error in block erase            \n",
    1," bit:1 error erase suspended           \n",
    1," bit:0 write state machine status ready\n",
};

/*
** Special command sequence for AMD 29F016 parts.
** fdiptr->flash_base must be valid on entry.
** Returns -1 on failure.
** THIS ROUTINE CURRENTLY ONLY WORKS WITH 32 BIT WIDE FLASH!
*/
unsigned long
amd_flash_cmd(struct flash_dvr_info *fdiptr, unsigned long command,
	      volatile unsigned long *addr, unsigned long data)
{
    unsigned long manu, device, temp;
    unsigned long retcode = -1;
    volatile unsigned long *baseaddr;

    baseaddr = (unsigned long *)fdiptr->flash_base;
#ifdef XDEBUG
printf("\namd_flash_cmd: command = %#x, at baseaddr %#x , addr %#x\n",
       command, baseaddr, addr);
#endif
    *(baseaddr + AMD_FLASH_CMD_ADDR1) = AMD_FLASH_CMD_CODE1;
    *(baseaddr + AMD_FLASH_CMD_ADDR2) = AMD_FLASH_CMD_CODE2;
    if(command != AMD_FLASH_ERASE_SCTR)
	*(baseaddr + AMD_FLASH_CMD_ADDR1) = command;
    switch(command) {
    case AMD_FLASH_ERASE_SCTR:
	*addr = command;
    case AMD_FLASH_ERASE_CHIP:
	data = AMD_DQ7_MASK;
	goto poll_dq7;
    case AMD_FLASH_PROGRAM:
	*addr = data;
	data &= AMD_DQ7_MASK;
poll_dq7:
	if(!pstat((unsigned long *)addr, 4, 1, AMD_DQ7_MASK, data, 5000, &temp)) {
	    printf("\namd_flash_cmd: timeout on AMD flash command\n");
#ifdef XXDEBUG
	    printf("expctd value: %#x, got: %#x\n", data, temp & AMD_DQ7_MASK);
#endif
	    return(retcode);
	}
	return(0);
    case AMD_FLASH_ERASE1:
    case AMD_FLASH_RESET:
	return(0);
    case AMD_FLASH_AUTOSELECT:
	manu = *addr;
	device = *(addr + 1);
#ifdef DEBUG
printf("amd_flash_cmd: manu = %#x, device = %#x, at baseaddr %#x , addr %#x\n",
       manu, device, baseaddr, addr);
#endif
	/* sanity check to make sure we are 32 bits wide */
	if(manu == 0xffffffff || device == 0xffffffff ||
	   ((manu & 0xff) != ((manu >> 24) & 0xff))  ||
	   ((device & 0xff) != ((device >> 24) & 0xff)) ||
	   ((manu & 0xffff) != ((manu >> 16) & 0xffff))  ||
	   ((device & 0xffff) != ((device >> 16) & 0xffff)))
	    return(retcode);
	fdiptr->flash_width = 4;
	retcode = (((manu & 0xff) << 8) | (device & 0xff));
	break;
    default:
	return(-1);
    }
    amd_flash_cmd(fdiptr, AMD_FLASH_RESET, addr, data);
    return(retcode);
}
    
/*
** Get the manufacturer and device information from the flash chips.
** fdiptr->base must be valid for this to work.
** This routine will actually fill in the flash_width field and the mask.
** Returns -1 on failure.
*/
static int
get_man_dev(struct flash_dvr_info *fdiptr)
{
    unsigned long temp, temp1;
    unsigned short mandev;
    int i, j, len;
    volatile unsigned long *addr;

    boolean found;
    char *phys_fptr;
    struct devstruct *devptr;    /* to be set to the device table */
    fs_device_info_block_t dev_info_blk;

    fdiptr->flash_mask = 0;

    addr = (volatile unsigned long *)fdiptr->flash_base;
    *addr = RESETCMD;
    wastetime(sixusecs);
    *addr = RESETCMD;
    wastetime(sixusecs);
    *addr = READID;
    wastetime(sixusecs);
    temp = *addr;
    temp1 = *(addr+1);
    *addr = RESETCMD;
#ifdef DEBUG
    printf("get_man_dev: read temp %#4x & temp1 %#4x from addr %#x\n",
	   temp, temp1, addr);
#endif
    mandev = ((temp & 0xff) << 8);  /* pick up man portion */
    if((temp >> 16) == (temp & 0xffff)) {
	fdiptr->flash_width = BW_32BITS;    /* 32 bit flash bus (4 byte) */
	mandev |= (temp1 & 0xff);           /* pick up dev portion */
    } else if((temp >> 24) == ((temp >> 16) & 0xff)) {
	fdiptr->flash_width = BW_16BITS;    /* 16 bit bus */
	mandev |= ((temp >> 16) & 0xff);    /* pick up dev portion */
    } else {
	fdiptr->flash_width = BW_8BITS;     /* eight bit bus */
	mandev |= ((temp >> 8) & 0xff);     /* pick up dev portion */
    }

    /*
    ** Plan A
    ** Obtain device information from the device information block
    ** This will only work if the device is formated.
    */
    /* first query the device table for physical address to boot flash.*/
    devptr = devtbl;
    found = FALSE;
     for(i=0; i<numdevstructs; i++) {  
       if (devptr == 0) break;
       len = strlen(devptr->devid);
#ifdef DEBUG
      printf ("get_man_dev: devtbl - %s\n",devptr->devid);
#endif
      if( (strncmp(devptr->devid, "boot flash", len) == 0 ) ||
	  (strncmp(devptr->devid, "bootflash:", len) == 0 ) ) { 
        /*extracting physical base address for flash*/
        phys_fptr = devptr->devstart; 
        found = TRUE;
        break; 
      }
      else devptr++;
    }	
    if (found) { /*make sure physical address is set */
      ml_iface.bcopy ( phys_fptr, (char*)&dev_info_blk, 
		       sizeof(fs_device_info_block_t) );
      if (dev_info_blk.magic != FS_DEVICE_INFO_BLOCK_MAGIC) { 
	found=FALSE; /* device is not formated - no DIB located*/
	printf("get_man_dev:Warning! Device is not formatted. Invalid DIB.\n"); 
      } 
    } else {
      /*boot flash really should not be missing from the device table*/
      printf("get_man_dev: No entry in device table for boot flash!\n"); 
    }
    /* all possible preparations for plan A completed.*/

    if (found) { /*DIB located - only possible if physical address was set.*/
      /*Do plan A.*/ 
#ifdef DEBUG
      printf("get_man_dev: Doing Plan A.\n");
#endif
      fdiptr->fiptr = &dib_set; 
      fdiptr->fiptr->dev_size = dev_info_blk.length/fdiptr->flash_width;
      fdiptr->fiptr->sector_size = dev_info_blk.sector_size;
      /* Plan A Completed. */
    } else { 
    /*
    ** Plan B - fallback Plan
    ** Look for the device in the flash_id table.
    */
#ifdef DEBUG
      printf("get_man_dev: Doing Plan B.\n");
#endif
      fdiptr->fiptr = (struct flash_id *)0;
      for(j=0; j<2; j++) {
	for(i=0; i<MAXFLIDS; i++) {
#ifndef  __mips
	    if(mandev == flash_ids[i].r_dev_code) {
		fdiptr->fiptr = &flash_ids[i];
		break;
	    }
#else	/* __mips */
	    if(mandev == flash_ids[i].dev_code) {
		fdiptr->fiptr = &flash_ids[i];
		break;
	    }
#endif	/* __mips */
	}
	if(!fdiptr->fiptr && j==0) {  /* mandev not found in table */
	    /* handle the new AMD algorithm */
	    mandev = amd_flash_cmd(fdiptr, AMD_FLASH_AUTOSELECT, 
				   (unsigned long *)fdiptr->flash_base, 0);
	}
      }
    } /*Plan B completed.*/

    /* 
    ** Do not set mandev to -1 if fdiptr->fiptr is invalid (i.e. 
    ** fdiptr->fiptr=0)  because the error message in get_flash_info 
    ** would not show the useful actual manufacturer device id but rather 
    ** only -1 (i.e. 0xFFFF). 
    */
    if(fdiptr->fiptr != 0) { 
	for(i=0; i<fdiptr->flash_width; i++) 
	    fdiptr->flash_mask = (fdiptr->flash_mask << 8) | 0xff;
    }
    return(mandev);
}

/*
** get_flash_info(fdiptr)
**
** Fill out the flash driver info structure pointed to by fdiptr.
** The base address field in the structure must be valid and the caller
** must make sure there are MAXFLASHSETS flash_dvr_info structs and they
** must be contiguous.
*/
int
get_flash_info(struct flash_dvr_info *fdiptr)
{
    int i, j, banksiz;
    unsigned bankstart;
    unsigned short mandev;
    struct flash_dvr_info *orig_fdiptr;

#ifdef DEBUG
    struct flash_dvr_info *init_fdiptr = fdiptr;
    printf("get_flash_info(fdiptr): entering with fdiptr = %#x\n",
	   fdiptr);
#endif

    tenmsecs = timer_calibrate(10000); /* 10ms delay */
    tenusecs = timer_calibrate(10);
    sixusecs = timer_calibrate(6);
    total_flash = 0;
    orig_fdiptr = fdiptr;

#ifdef DEBUG
printf("get_flash_info: done calibrating the timer\n");
#endif

    for(j=0; j<MAXFLASHSETS; j++) {
	fdiptr->fiptr = (struct flash_id *)0;  /* init */
	fdiptr->next = (struct flash_dvr_info *)0;

#ifdef DEBUG
printf("get_flash_info: calling chkberr with addr of %#x\n",
       fdiptr->flash_base);
#endif
	if(chkberr((unsigned long *)fdiptr->flash_base, BW_32BITS, 1)) {
#ifdef DEBUG
	    printf("Bus error at start of flash bank at %#x\n",
		   fdiptr->flash_base);
#endif
	    if(j==0) {  /* first time thru loop? */
		printf("bus error while trying to access flash - addr = %#x\n",
		      fdiptr->flash_base);
		return(0);
	    }
badflashset:
	    if(fdiptr != orig_fdiptr) {
		fdiptr--;
		fdiptr->next = (struct flash_dvr_info *)0;  /* NULL this out */
	    }
	    break;  /* all done */
	}

	flash_prog_en(fdiptr);  /* enable programming mode */
	mandev = get_man_dev(fdiptr);
	flash_prog_dis(fdiptr);  /* disable programming mode */

	if(!fdiptr->fiptr) {
	    if(j==0) {  /* first time through loop? */
		printf("unknown flash device - mandev code = %#x\n", mandev);
		return(0);
	    }
	    goto badflashset;
	}

	/*
	** Calculate a bank size.
	*/
	banksiz = fdiptr->fiptr->dev_size * fdiptr->flash_width;

#ifdef DEBUG
	printf("banksiz(%#x) = fdiptr->fiptr->dev_size(%#x) "
	       "* fdiptr->flash_width(%#x);", banksiz,
	       fdiptr->fiptr->dev_size,  fdiptr->flash_width);
#endif

	/*
	** Figure out how many banks there are.
        */
	fdiptr->flash_banks = 1;  /* init */
	bankstart = fdiptr->flash_base;  /* init */
	for(i=0; i< (MAXFLASHBANKS); i++) {
	    bankstart += banksiz;  /* bump to next bankstart */
	    if(chkberr((unsigned long *)bankstart, fdiptr->flash_width, 1)) {
#ifdef DEBUG
		printf("get_flash_info: bus error at %#x\n",bankstart);
#endif
		break;
	    }
	    fdiptr++;  /* temporarily point to the next struct */
	    fdiptr->flash_base = bankstart;
	    flash_prog_en(fdiptr);   /* enable programming mode */
	    mandev = get_man_dev(fdiptr);
	    flash_prog_dis(fdiptr);  /* disable programming mode */
	    fdiptr--;  /* go back to last struct */
#ifdef DEBUG
	    printf("get_flash_info: got man_dev of %#x\n",mandev);
#endif

	    /*
	    ** Each new kind of flash chip must have a new fdi structure.
	    ** Note that the new AMD_STATREG chips must have a new fdi struct
	    ** for each bank so each bank can be reset when necessary.
	    */
#ifndef __mips
	    if(mandev != fdiptr->fiptr->r_dev_code ||  /* different mandev */
	       fdiptr->fiptr->flags == AMD_STATREG) {
#else	/* __mips */
	    if(mandev != fdiptr->fiptr->dev_code ||  /* different mandev */
	       fdiptr->fiptr->flags == AMD_STATREG) {
#endif	/* __mips */
		fdiptr->next = fdiptr + 1;   /* set the next pointer */
		/* flash_base was filled in above */
		break;                       /* to go thru outer loop again */
	    }
	    fdiptr->flash_banks++;
	}
	fdiptr->flash_size = banksiz * fdiptr->flash_banks;
	fdiptr->flash_sector = fdiptr->flash_width * fdiptr->fiptr->sector_size;
	total_flash += fdiptr->flash_size;
	fdiptr = fdiptr->next;
	if(!fdiptr) break;  /* all done */
    }
    return(1);
}


/*
** The newer flash parts have status registers.  This routine polls
** the address passed until it sees the status passed or timeout is reached.
*/
unsigned long
poll_flash_stat(volatile unsigned char *addr, unsigned long stat,
		int size, unsigned long mask)
{
    unsigned long rdstatcmd;

    rdstatcmd = RDSTATCMD & mask;
    stat &= mask;

    switch(size) {
    case BW_32BITS:
	*(volatile unsigned long *)addr = rdstatcmd;
	break;
    case BW_16BITS:
	*(volatile unsigned short *)addr = rdstatcmd;
	break;
    default:
	*(volatile unsigned char *)addr = rdstatcmd;
	break;
    }
    wastetime(sixusecs);
    if(!pstat((unsigned long *)addr, size, 1, stat, stat, 100000, &stat)) {
	printf("timeout on flash read status command\n");
	return(0);
    }
    return(stat);
}


/*
** program a location in flash
** it is assumed that flash programming is enabled
** fdiptr must be valid for the given location
*/
static char *prog_err_msg = "flash program error";

int
program_flash(volatile unsigned char *location, long data,
	      struct flash_dvr_info *fdiptr)
{
    int writepulse;
    unsigned long readback, prgcmd, prgver, rstcmd;
    unsigned long mask, pollstat, wrstat, clrstat;
    unsigned statreg;
    unsigned char amdpart;

    if(!fdiptr->fiptr) {
	printf("%s - illegal flash driver info pointer\n", prog_err_msg);
	return(0);
    }

    /* does device have a status register? */
    statreg = fdiptr->fiptr->flags & FI_STATREG;
    amdpart = fdiptr->fiptr->flags & AMD_STATREG;

    /*
    ** prepare our commands and status
    */
    mask = fdiptr->flash_mask;
    prgcmd = PROGCMD & mask;
    prgver = PROGVER & mask;
    rstcmd = RESETCMD &	mask;

    if(statreg) {
	pollstat = WRSTATRDY & mask;
	wrstat   = BYTEWRSTAT & mask;
	clrstat  = CLRSTATREG & mask;
    }

    /*
    ** write the location
    */
    for( writepulse = 0; writepulse < MAXFLASHWRITE; writepulse++) {
	switch(fdiptr->flash_width) {
	case BW_32BITS:  /* lword */
	    if(amdpart) {
		amd_flash_cmd(fdiptr, AMD_FLASH_PROGRAM, 
			      (unsigned long *)location, data);
		goto chkwrite;
	    }
	    *(volatile unsigned long *)location = prgcmd;
	    *(volatile unsigned long *)location = data;
	    if(statreg) {  /* we are using technology with a status register */
		poll_flash_stat(location, pollstat, fdiptr->flash_width, mask);
		if ( *(volatile unsigned long *)location & wrstat ) {  /* erase error? */
		    *(volatile unsigned long *)location = clrstat;  /* clear error */
badwrite:
		    printf("write operation failed at location %#x\n", location);
		    return(0);
		}
		goto chkwrite;
	    }
	    wastetime(tenusecs); 
	    *(volatile unsigned long *)location = prgver;
	    wastetime(sixusecs);
	    readback = *(volatile unsigned long *)location;
	    break;
	case BW_16BITS:
	    if(amdpart) {
		printf("16 bit writes to AMD 016 parts is not supported\n");
		return(0);
	    }
	    *(volatile unsigned short *)location = prgcmd;
	    *(volatile unsigned short *)location = data;
	    if(statreg) {  /* we are using technology with a status register */
		poll_flash_stat(location, pollstat, fdiptr->flash_width, mask);
		if ( *(volatile unsigned short *)location & wrstat ) {  /* erase error? */
		    *(volatile unsigned short *)location = clrstat;  /* clear error */
		    goto badwrite;
		}
		goto chkwrite;
	    }
	    wastetime(tenusecs); 
	    *(volatile unsigned short *)location = prgver;
	    wastetime(sixusecs);
	    readback = *(volatile unsigned short *)location;
	    break;
	default:
	    if(amdpart) {
		printf("8 bit writes to AMD 016 parts is not supported\n");
		return(0);
	    }
	    *(volatile unsigned char *)location = prgcmd;
	    *(volatile unsigned char *)location = data;
	    if(statreg) {  /* we are using technology with a status register */
		poll_flash_stat(location, pollstat, fdiptr->flash_width, mask);
		if ( *(volatile unsigned char *)location & wrstat ) {  /* erase error? */
		    *(volatile unsigned char *)location = clrstat;  /* clear error */
		    goto badwrite;
		}
		goto chkwrite;
	    }
	    wastetime(tenusecs); 
	    *(volatile unsigned char *)location = prgver;
	    wastetime(sixusecs);
	    readback = *(volatile unsigned char *)location;
	    break;
	}
	if ( readback == data ) break;
    }
 chkwrite:
    switch(fdiptr->flash_width) {
    case BW_32BITS:  /* lword */
	if(!amdpart) {
	    *(volatile unsigned long *)location = rstcmd;
	    wastetime(sixusecs);
	}
	readback = *(volatile unsigned long *)location;
	break;
    case BW_16BITS:
	*(volatile unsigned short *)location = rstcmd;
	wastetime(sixusecs);
	readback = *(volatile unsigned short *)location;
	break;
    default:
	*(volatile unsigned char *)location = rstcmd;
	wastetime(sixusecs);
	readback = *(volatile unsigned char *)location;
	break;
    }
    if(readback == data) return(1);
    printf("\nprogram_flash: location %#x wrote %#8x, contains %#8x\n",
	   location, data, readback);
    return(0);

}

/*
** Return the appropriate fdiptr for the start address supplied.
*/
struct flash_dvr_info *
get_fdi_struct(struct flash_dvr_info *fdiptr, unsigned start)
{
    while(fdiptr) {
	if(start >= fdiptr->flash_base && 
	   start < (fdiptr->flash_base + fdiptr->flash_size))
	    return(fdiptr);
	fdiptr = fdiptr->next;
    }
    return((struct flash_dvr_info *)0);
}


/*
** Cleanup routine for our flash driver.
** Attempt to reset all chips.
*/
static int ocachevalid;
static int origcache;

int
flash_cleanup(void)
{
    struct flash_dvr_info *fdiptr;

    fdiptr = g_fdiptr;  /* pick up global fdi ptr */
#ifdef DEBUG
    printf("\nflash_cleanup: fdiptr = %#x\n", fdiptr);
#endif
    while(fdiptr) {
	if(fdiptr->flash_base && fdiptr->fiptr) {
	    if(fdiptr->fiptr->flags & AMD_STATREG) {
#ifdef DEBUG
printf("resetting new amd chip at %#x\n", fdiptr->flash_base);
#endif
		amd_flash_cmd(fdiptr, AMD_FLASH_RESET,
			      (unsigned long *)0, 0);
	    } else {
#ifdef DEBUG
printf("resetting normal chip at %#x\n", fdiptr->flash_base);
#endif
		flash_prog_en(fdiptr);
		*(volatile unsigned long *)fdiptr->flash_base = RESETCMD;
		wastetime(sixusecs);
		*(volatile unsigned long *)fdiptr->flash_base = RESETCMD;
		wastetime(sixusecs);
	    }
	}
	fdiptr = fdiptr->next;
    }
    flash_prog_dis(fdiptr);
    signal(SIGINT,SIG_DFL);
    if(ocachevalid) setcacr(origcache);
    return((int)SIG_DFL);  /* do the default action too */
}


/*
** erase_flash
**
** erase the specified flash EPROMs or sectors
** return 1 if EPROMs are successfully erased, 0 otherwise
*/
int
erase_flash(struct flash_dvr_info *fdiptr, unsigned start, int size)
{
    int erasepulse, i;
    volatile unsigned char *sptr, *sectend;
    unsigned long data, erasecmd, readval, mask, statreg;
    int width;
    SIG_PF savsig;
    int retval = 1;  /* optimism */

    /*
    ** Make sure starting and ending addresses are on sector
    ** boundaries.
    ** First find the appropriate fdi structure.
    */
    if(size == 0) return(0);
    if(!(fdiptr = get_fdi_struct(fdiptr, start))) {
	printf("erase_flash: illegal start address\n");
	return(0);
    }
    if((start - fdiptr->flash_base) % fdiptr->flash_sector) {
	printf("erase_flash: start address not on sector boundary\n");
	return(0);
    }

    savsig = (SIG_PF)signal(SIGINT, (SIG_PF)flash_cleanup);
    flash_prog_en();
    while(fdiptr && size) {
	if(size && size < fdiptr->flash_sector) {
	    printf("erase_flash: erase size is less than sector size of %#x\n",
		   fdiptr->flash_sector);
	    goto badreturn;
	}
	sptr = (volatile unsigned char *)start;
	sectend = (volatile unsigned char *)(start + fdiptr->flash_sector);
	mask = fdiptr->flash_mask;
	width = fdiptr->flash_width;

#ifdef DEBUG
	printf("\rErasing flash sector at %#x...flag is %#.2x",
	       sptr, fdiptr->fiptr->flags);
#endif
	switch(fdiptr->fiptr->flags & (AMD_STATREG | FI_STATREG)) {
	case AMD_STATREG:
	    amd_flash_cmd(fdiptr, AMD_FLASH_ERASE1, (unsigned long *)sptr, 0);
	    if(amd_flash_cmd(fdiptr, AMD_FLASH_ERASE_SCTR, 
			     (unsigned long *)sptr, 0) < 0)
		goto badreturn;
	    sptr += fdiptr->flash_sector;
	    break;
	case FI_STATREG:
	    /* 
	    ** We are using technology with a status register.
	    ** Erase the entire sector.
	    */
	    switch(width) {
	    case BW_32BITS:  /* lword */
		*(volatile unsigned long *)sptr = ERASECMD; 
		*(volatile unsigned long *)sptr = ERASECONFIRM;
		poll_flash_stat(sptr, WRSTATRDY, width, mask);
		statreg = ( *(volatile unsigned long *)sptr & ERASESTAT);
		if ( statreg ) {  /* erase error? */
		    *(volatile unsigned long *)sptr = CLRSTATREG;  /* clear error */
staterr:
		    printf("sector erase failed at location %#x, status %#x\n",
			   sptr, statreg);
		    goto badreturn;
		}
		break;
	    case BW_16BITS:  /* word */
		*(volatile unsigned short *)sptr = (ERASECMD & mask); 
		*(volatile unsigned short *)sptr = (ERASECONFIRM & mask);
		poll_flash_stat(sptr, WRSTATRDY, width, mask);
		statreg = ( *(volatile unsigned short *)sptr & (ERASESTAT & mask));
		if ( statreg ) {  /* erase error? */
		    *(volatile unsigned short *)sptr = (CLRSTATREG & mask);  /* clear error */
		    goto staterr;
		}
		break;
	    default:  /* byte */
		*sptr = (ERASECMD & mask); 
		*sptr = (ERASECONFIRM & mask);
		poll_flash_stat(sptr, WRSTATRDY, width, mask);
		statreg = *sptr & (ERASESTAT & mask);
		if ( statreg ) {  /* erase error? */
		    *sptr = (CLRSTATREG & mask);  /* clear error */
		    goto staterr;
		}
		break;
	    }
	    sptr += fdiptr->flash_sector;
	    break;
	default:
	    /*
	    ** Program all locations to zero before erase
	    */
#ifdef DEBUG
	    printf("\nZeroing flash EPROMs for erase at sector base %#x...", sptr);
#endif
	    for(sptr = (volatile unsigned char *)start; sptr < sectend;
		sptr += width ) {
		switch(width) {
		case BW_32BITS:
		    readval = *(volatile unsigned long *)sptr;
		    break;
		case BW_16BITS:
		    readval = *(volatile unsigned short *)sptr;
		    break;
		default:
		    readval = *sptr;
		    break;
		}

		if( readval != 0 ) {
		    if( !program_flash(sptr, 0, fdiptr) ) {
			printf("\nflash address %#x would not zero for erase",sptr);
			goto badreturn;
		    }
		}

#ifdef DEBUG
		if( !((ulong)sptr % 0x1000)) putchar('z');
#endif
	    }

	    /*
	    ** Now erase the flash.
	    */
	    erasepulse = 0;
	    erasecmd = ERASECMD & mask;
	    sptr = (volatile unsigned char *)start;
#ifdef DEBUG
	    printf("\nErasing flash EPROMs at sector base %#x...",sptr);
#endif
	    while( erasepulse++ < MAXFLASHERASE ) {  /* allowable erase pulses */
		erasecmd &= mask;
		switch(width) {
		case BW_32BITS:  /* lword */
		    /* write erase set-up command */
		    *(volatile unsigned long *)sptr = erasecmd;
		    /* write erase command */
		    *(volatile unsigned long *)sptr = erasecmd;
		    break;
		case BW_16BITS:  /* word */
		    /* write erase set-up command */
		    *(volatile unsigned short *)sptr = erasecmd;
		    /* write erase command */
		    *(volatile unsigned short *)sptr = erasecmd;
		    break;
		default:  /* byte*/
		    /* write erase set-up command */
		    *sptr = erasecmd;
		    /* write erase command */
		    *sptr = erasecmd;
		    break;
		}
		wastetime(tenmsecs);  /* 10ms delay */
		while( sptr < sectend ) {  /* verify location by location */
		    switch(width) {
		    case BW_32BITS:  /* lword */
			*(volatile unsigned long *)sptr = ERASEVER;
			wastetime(sixusecs);
			data = *(volatile unsigned long *)sptr;
			break;
		    case BW_16BITS:  /* word */
			*(volatile unsigned short *)sptr = (ERASEVER & mask);
			wastetime(sixusecs);
			data = *(volatile unsigned short *)sptr;
			break;
		    default:  /* byte */
			*sptr = (ERASEVER & mask);
			wastetime(sixusecs);
			data = *sptr;
			break;
		    }
		    if( data != mask ) {  /* verify failed */
			/*
			** Important: We must re-erase only the byte that
			** needs re-erasing.  If you re-erase a location
			** that is already erased the part may be destroyed.
			** This code sets up the erasecmd for the next erasepulse
			** substituting the READ_MEMORY command for the bytes
			** that do not need re-erasing.
			*/
			erasecmd = (READMEM & mask);
			for( i=0; i< (width * 8); i+=8 ) {
			    if((( data >> i ) & 0xff ) != 0xff )  /* location not erased */
				erasecmd |= ( C_ERASE << i );
			}
#ifdef XDEBUG
			printf("erase command = %#x, at location %#x\n",erasecmd,sptr);
#endif
			break;
		    }
#ifdef DEBUG
		    if(!((ulong)sptr % 0x1000)) putchar('e');
#endif
		    sptr += width;
		}
		*(volatile unsigned long *)start = RESETCMD;
		if(sptr == sectend) break;  /* this sector successfully erased */
	    }
	    if(sptr != sectend) goto erasedone;  /* we ran out of erase pulses */
	    break;
	}
	start += fdiptr->flash_sector;

	if(start >= (fdiptr->flash_base + fdiptr->flash_size)) {
	    /* we are at the end of this flash_dvr_info struct */
	    fdiptr = fdiptr->next;
	    if(fdiptr) start = fdiptr->flash_base;
	    else break;  /* all done */
	}
	if(size) size -= fdiptr->flash_sector;
    }
erasedone:
    if(sptr != sectend) {
	printf("\nerase failed at location %#x",sptr);
badreturn:
	retval = 0;
    }
returnok:
    flash_prog_dis(fdiptr);
    return(retval);
}

/*
** General routines for erasing a flash array and programming an
** image into flash
*/

/*
** Initialize the flash structures.
** fdiptr must point to an array of fdi structs that is large enough.
** Returns 1 on success and 0 on failure.
** SIGINT is left pointing to flash_cleanup and g_fdiptr is valid.
*/
int
get_flash_fdi(struct flash_dvr_info *fdiptr, unsigned long baseaddr)
{

    signal(SIGINT,SIG_IGN);  /* ignore while we get origcache */
    origcache = clearcaches();
    ocachevalid = 1;
    g_fdiptr = fdiptr;
    bzero(fdiptr, sizeof(struct flash_dvr_info));
    signal(SIGINT,(SIG_PF)flash_cleanup);

    fdiptr->flash_base = baseaddr;

    /* Fill in fdi structure */

    if(!get_flash_info(fdiptr)) {
        printf("unable to get flash info...aborting\n");
        flash_cleanup();
        g_fdiptr = (struct flash_dvr_info *)0;
        return(0);
    }
    return(1);
}

/*
** Erase all of flash as defined by fdi structs
** Returns 1 on success and 0 on failure.
*/
int
fdi_erase_flash(struct flash_dvr_info *fdiptr)
{

    int start;
    int retval = 1;  /* success */

    while(fdiptr) {     
        for(start=fdiptr->flash_base;
            start<(fdiptr->flash_base+fdiptr->flash_size);
            start+=fdiptr->flash_sector) {
            printf("\rErasing flash at %#x",start);
            if(!erase_flash(fdiptr, start, fdiptr->flash_sector)) {
                printf("flash sector will NOT erase...aborting\n");
		retval = 0;
		goto done;
            }
        }
      fdiptr = fdiptr->next;
    }
done:
    flash_cleanup();
    g_fdiptr = (struct flash_dvr_info *)0;
    return(retval);
}

/*
** Copy data to flash
** Returns 1 on success and 0 on failure.
*/
int
fdi_program_flash(struct flash_dvr_info *fdiptr, unsigned long source,
		  unsigned long dest, int size)
{

    int width;
    unsigned long source_ptr, dest_ptr, source_end;
    unsigned long data;
    int retval = 0;

    /* Program flash */
    source_end = source + size;
    width = fdiptr->flash_width; /* get the width of the data bus */
    flash_prog_en();
    for(source_ptr = source, dest_ptr = dest; source_ptr < source_end;
                          source_ptr += width, dest_ptr += width) {
        switch (width) {
        case BW_8BITS : /* 8 bit data bus*/
            /* read the data */
            (unsigned char)data = *(unsigned char *)source_ptr;
            break;

        case BW_16BITS : /* 16 bit data bus*/
            /* read the data */
            (unsigned short)data = *(unsigned short *)source_ptr;
            break;

        case BW_32BITS : /* 32 bit data bus*/
            /* read the data */
            (unsigned long)data = *(unsigned long *)source_ptr;
            break;
        }

        if(!(dest_ptr % 0x10000))
            printf("\rprogram flash location %#x", dest_ptr);

        if(!(fdiptr = get_fdi_struct(fdiptr, dest_ptr))) {
            printf("program flash: address %#x not valid - aborting\n",
                   dest_ptr);
	    goto done;
        }

        /* burn data into flash */
        if(!program_flash((unsigned char *)dest_ptr, (long)data, fdiptr)) {
            printf("program flash failure at location %#x - aborting\n",
		   dest_ptr);
	    goto done;
        }

    }
    retval = 1;  /* success */
done:
    flash_cleanup();
    g_fdiptr = (struct flash_dvr_info *)0;
    return(retval);
}

/*
** General routines for the outside world.
** Hides all of the fdi particulars.
** Just need base address, etc.
** Returns 1 on success and 0 on failure.
*/
int
erase_flash_array(unsigned long baseaddr)
{
    struct flash_dvr_info fdi[MAXFLASHBANKS*2];

    if(get_flash_fdi(fdi, baseaddr) == 0)
	return(0);

    return(fdi_erase_flash(fdi));
}

int
copy_to_flash(unsigned long sourceaddr, unsigned long flashaddr,
	      int size)
{
    struct flash_dvr_info fdi[MAXFLASHBANKS*2];

    if(get_flash_fdi(fdi, flashaddr) == 0)
	return(0);

    return(fdi_program_flash(fdi, sourceaddr, flashaddr, size));
}


/*
 * flash_test
 * Test the flash EPROMs. Do read/write test. Let user know stats on each
 * part.  Provide simple "data equals address" test routine (flash_test)
 * and an extended routine with all ones and all zeros tests (eflash_test).
 */

unsigned long testarray1[] = {
    0x55555555, 0xaaaaaaaa,
};

unsigned long testarray2[] = {
    0xaaaaaaaa, 0x55555555,
};

int
flash_test(int testlength)
{
    extern boolean ftest();
    boolean retval = 1;  /* pessimistic? */
    int mandev;
    struct flash_dvr_info fdi[MAXFLASHBANKS*2];

    testname("flash memory");
    bzero(fdi, sizeof(struct flash_dvr_info));
    fdi[0].flash_base = get_flash_base();

    signal(SIGINT,SIG_IGN);  /* ignore while we get origcache */
    origcache = clearcaches();
    ocachevalid = 1;
    g_fdiptr = fdi;  /* set this for cleanup routine */
    signal(SIGINT,(SIG_PF)flash_cleanup);

    if(!get_flash_info(fdi)) {
	cterr('w',0,"flash not available...aborting test");
	goto ret;
    }

    if(!ftest(fdi, (unsigned long *)0, 0, "data equals address" ) ) {
	goto ret;
    }

    /*
    ** Test for VPP off.
    */
    
    if (test_vpp_off(fdi))
	goto ret;

    if(testlength > 0) {
	if(!ftest(fdi, testarray1, 2, "0x55, 0xAA" ) ||
	   !ftest(fdi, testarray2, 2, "0xAA, 0x55" ) )
	    goto ret;
    }
	
    prcomplete(testpass, errcount, 0);
    retval = 0;
ret:
    flash_cleanup();
    g_fdiptr = (struct flash_dvr_info *)0;
    return(retval);
}

int
ftest(struct flash_dvr_info *fdiptr, unsigned long *valptr, int numvals, 
      char *nameptr)
{
    volatile unsigned char *start;
    volatile unsigned char *ptr, *end;
    unsigned long *vptr, *vend;
    unsigned long rval, value;
    struct flash_dvr_info *orig_fdiptr;

#ifdef DEBUG
    printf("\nentering ftest\n");
#endif
    orig_fdiptr = fdiptr;

    while(fdiptr) {     
        for(start = (volatile unsigned char *)fdiptr->flash_base;
            start < (volatile unsigned char *)(fdiptr->flash_base+fdiptr->flash_size);
            start += fdiptr->flash_sector) {
	    prpass(testpass,"erasing flash sector at %#x",start);
            if(!erase_flash(fdiptr, (int)start, fdiptr->flash_sector)) {
                cterr('f',0,"flash sector will NOT erase...aborting\n");
		return(0);
            }
        }
      fdiptr = fdiptr->next;
    }

    flash_prog_en();

    if( numvals ) vend = valptr + numvals;
    else {
	valptr = &value;
	vend = valptr + 1;
    }

    fdiptr = orig_fdiptr;
    while(fdiptr) {
	start = (volatile unsigned char *)fdiptr->flash_base;
	end = (volatile unsigned char *)((long)start + fdiptr->flash_size);
	if(fdiptr->fiptr->flags & AMD_STATREG) {
	    /* these chips require a reset here */
	    amd_flash_cmd(fdiptr, AMD_FLASH_RESET, (unsigned long *)0, 0);
	}
	for ( ptr = start; ptr < end; ) {
	    for(vptr = valptr; vptr < vend && ptr < end; vptr++, ptr += fdiptr->flash_width) {
		if( !((unsigned long)ptr % 0x10000))
		    prpass(testpass, "write phase, location %#x,", ptr);
		if( !numvals ) *vptr = (long)ptr;  /* data equals address test */
		if( !program_flash( ptr, *vptr, fdiptr ) ) {
		    cterr('f',0,"write failure at location %8x", ptr);
		    flash_prog_dis();
		    return(0);
		}
	    }
	}
	fdiptr = fdiptr->next;
    }
    flash_prog_dis();

    fdiptr = orig_fdiptr;
    while(fdiptr) {
	start = (volatile unsigned char *)fdiptr->flash_base;
	end = (volatile unsigned char *)((long)start + fdiptr->flash_size);
	for ( ptr = start; ptr < end; ) {
	    for(vptr = valptr; vptr < vend && ptr < end; vptr++, ptr += fdiptr->flash_width) {
		if( !((unsigned long)ptr % 0x10000))
		    prpass(testpass, "verify phase, location %#x,", ptr);
		if( !numvals ) *vptr = (long)ptr;  /* data equals address test */
		switch(fdiptr->flash_width) {
		case BW_32BITS:  /* lword */
		    rval = *(volatile unsigned long *)ptr;
		    break;
		case BW_16BITS:  /* word */
		    rval = *(volatile unsigned short *)ptr;
		    break;
		default:  /* byte */
		    rval = *ptr;
		    break;
		}
		if( rval != (*vptr & fdiptr->flash_mask)) {
		    cterr('f',0,"verification failure at location %8x: expected %8x, is %8x",
			  ptr, *vptr, rval);
		    return(0);
		}
	    }
	}
	fdiptr = fdiptr->next;
    }
#ifdef DEBUG
    printf("\nverify done\n");
#endif
    return(1);
}

char *no_flash_msg = "\nThis hardware platform does not support %s flash\n";

int
flash(int argc, char *argv[])
{
    int i, bootflash, size, retval=1, width, fcc_install, start;
    int debug, info, erase, prog;
    static char optstr[] = "bd";
    extern int optind;
    struct flash_dvr_info *fdiptr;
    unsigned long source, dest, source_ptr, dest_ptr, source_end;
    unsigned long base, data;
    struct flash_dvr_info fdi[MAXFLASHBANKS];

    fdiptr = fdi;
    fcc_install = 0; /* init */
    bootflash = 0;
    debug = 0; info = 0;
    erase = 0; prog = 0;
    base = get_flash_base(); /* address of system flash base */
    
    while((i = getopt(argc,argv,optstr)) >= 0) {
	switch(i) {
	case 'b':  /* boot flash */
	    bootflash = 1;
	    if(!bootflash_addr(&base, &fcc_install)) {
		printf(no_flash_msg, "boot");
		goto ret;
	    }
	    break;
	case 'd':  /* debug */
	    debug = 1;
	    break;
	default:
usage:
	    printf("usage: %s [-%s] info \n",argv[0],optstr);
	    printf("usage: %s [-%s] erase\n",argv[0],optstr);
	    printf("usage: %s [-%s] prog <source> <destination>"
		   " <size>\n",argv[0],optstr);
	    puts("-b  boot flash\n-d  debug\n");
	    goto ret;
	}
    }

    if (optind>=argc)
	goto usage;

    if(base == 0xffffffff) {
	printf(no_flash_msg, "system");
	goto ret;
    }
    if(strcmp(argv[optind],"info") == 0) info = 1;
    if(strcmp(argv[optind],"erase") == 0) erase = 1;
    if(strcmp(argv[optind],"prog") == 0) prog = 1;

#ifdef DEBUG
    printf("\nargc = %#x, optind = %#x", argc, optind);
    printf("\ninfo = %#x, erase = %#x, prog = %#x\n",info,erase,prog);
#endif

    if(!(((info || erase)&&((argc - optind) == 1)&&(argc > 1)) ||
	 (prog &&((argc - optind) == 4)&&(argc > 4))))
	goto usage;

    if(debug) printf("\nnetboot = %#x, fcc_install = %#x, bootflash = %#x",
		     netflashbooted, fcc_install, bootflash);
    if(!netflashbooted && !fcc_install && bootflash){
	printf("\nYou are executing out of boot flash,"
	       " commands to the boot flash are not allowed\n");
	goto ret;
    }

    if(!get_flash_fdi(fdiptr, base)) goto ret;

    if(info){
	printf("\n%s flash info\n", bootflash ? "Boot" : "System");
	i=0;
	while(fdiptr) {
	    printf("\nflash driver info structure # %d\n"
		   "flash base: %#x    flash width: %d\n"
		   "flash set size: %#x    num banks: %d    device: %s\n",
		   i++, fdiptr->flash_base, fdiptr->flash_width, fdiptr->flash_size,
		   fdiptr->flash_banks, fdiptr->fiptr->dev_string);
	    fdiptr = fdiptr->next;
	}
	printf("\ntotal flash is %#x\n",total_flash);
	retval = 0;
	goto ret; /* done with info */
    }

    if(erase){
	if(fdi_erase_flash(fdiptr))
	    retval = 0;
	goto ret; /* done with erase */
    }

    /*
    ** Flash Program
    */
    getnum(argv[++optind],16,(long *)&source);
    getnum(argv[++optind],16,(long *)&dest);  
    getnum(argv[++optind],16,(long *)&size);
    source_end = source + size;

    if(debug) printf("\nsource = %#x, dest = %#x, size = %#x\n", source, dest, size);

    if((dest<fdiptr->flash_base) ||
       (dest>(fdiptr->flash_base + total_flash)) ||
       ((dest+size)>(fdiptr->flash_base + total_flash))) {
	printf("\nThe destination address or the size does not fit in flash");
	printf("\nPlease read \"flash info\" first\n");
	goto ret;
    }

    if(fdi_program_flash(fdiptr, source, dest, size))
	retval = 0;

ret:
    flash_cleanup();
    g_fdiptr = (struct flash_dvr_info *)0;
    printf("\n");
    return(retval);
}

/*
** Test for VPP off.
**
** Vpp has been turned off before this function is called.
** 
** On devices with a status register, using byte write command then
** read status and make sure the VPP bit is 1 (VPP low).
**
**   ---------------------------------------------------------------
**  | bit 7 | bit 6 | bit 5 | bit 4 | bit 3 | bit 2 | bit 1 | bit 0 | 
**  ----------------------------------------------------------------
**  |unused |unused |unused |  Vpp  | write | erase |suspend|       |
**  ----------------------------------------------------------------
**
**    1," bit:4 Vpp low detected      
**    1," bit:3 error in byte write   
**    1," bit:2 error in block erase   
**    1," bit:1 error erase suspended  
**
** Other devices, make sure we cannot read the man/device code.
*/

int
test_vpp_off (struct flash_dvr_info *fdiptr)
{
    ulong fl_stat, status, mandev, width, temp, result;
    volatile uchar *sptr;
    ulong mask;

    status = 1;
    fl_stat = 0;
#ifdef DEBUG
    printf("\nTest for Vpp off\n");
    printf("\nflash_base:%lx flag:%x mandev:%x\n",
	   fdiptr->flash_base, fdiptr->fiptr->flags, fdiptr->fiptr->dev_code);
#endif
    sptr = (volatile unsigned char *)fdiptr->flash_base;
    mask = fdiptr->flash_mask;
    width = fdiptr->flash_width;

    if(fdiptr->fiptr->flags & FI_STATREG) {
	switch(width) {
	case BW_32BITS:
	    *(volatile ulong *)sptr = CLRSTATREG;  /* clear error */
	    *(volatile ulong *)sptr = BYTEWRITE; 
	    *(volatile ulong *)sptr = 0xaaaaaaaa;
	    poll_flash_stat(sptr, WRSTATRDY, width, mask);
	    *(volatile ulong *)sptr = RDSTATCMD;
	    fl_stat = *(volatile ulong *)sptr;
	    result = pstat((ulong *)sptr,BW_32BITS,1,WRSTATRDY,WRSTATRDY,200,0);
	    if (*(volatile ulong *)sptr & (VPPSTAT | ERASESTAT | BYTEWRSTAT))
		*(volatile ulong *)sptr = CLRSTATREG;  /* clear error */
	    break;
	case BW_16BITS:
	    *(volatile ushort *)sptr = (ushort)CLRSTATREG;  /* clear error */
	    *(volatile ushort *)sptr = (ushort)BYTEWRITE; 
	    *(volatile ushort *)sptr = 0xaaaa;
	    poll_flash_stat(sptr, WRSTATRDY, width, mask);
	    *(volatile ushort *)sptr = (ushort)RDSTATCMD;
	    fl_stat = *(volatile ushort *)sptr;
	    result = pstat((ulong *)sptr,BW_16BITS,1,WRSTATRDY,WRSTATRDY,200,0);
	    if (*(volatile ushort *)sptr & (VPPSTAT | ERASESTAT | BYTEWRSTAT))
		*(volatile ushort *)sptr = (ushort)CLRSTATREG;  /* clear error */
	    break;
	default:
	    *sptr = C_CLEAR_STAT_REG;  /* clear error */
	    *sptr = C_BYTE_WRITE; 
	    *sptr = 0xaa;
	    poll_flash_stat(sptr, WRSTATRDY, width, mask);
	    *sptr = C_READ_STAT_REG;
	    fl_stat = *sptr;
	    result = pstat((ulong *)sptr,BW_8BITS,1,C_WR_STAT_RDY,C_WR_STAT_RDY,200,0);
	    if (*sptr & (C_VPP_STAT | C_ERASE_STAT | C_BYTE_WR_STAT))
		*sptr = C_CLEAR_STAT_REG;  /* clear error */
	    break;
	}

#ifdef DEBUG
	printf("\nIt has stat reg fl_stat:%x\n", fl_stat); 
#endif
	if((fl_stat & VPPSTAT) == 0) {
	    cterr('f', 0, "VPP does not turn off (statreg = %#2x)"
		  "active bits are:\n %#.8b", fl_stat,flash_stat,fl_stat);
	    status = 0;
	}
    } else if(!(fdiptr->fiptr->flags & AMD_STATREG)) {

#ifndef  __mips
	if((mandev = get_man_dev(fdiptr)) == fdiptr->fiptr->r_dev_code) {
#else	/* __mips */
	if((mandev = get_man_dev(fdiptr)) == fdiptr->fiptr->dev_code) {
#endif 	/* __mips */
	    cterr('f', 0, "VPP does not turn off (man_dev = %#4x)", mandev);
	    status = 0;
	}
    }
 
    return (status);
}

/*
** convenient routines for getting the size of flash
*/
long
flash_siz(struct flash_dvr_info *fdip)
{
    if(!get_flash_info(fdip)) {
	printf("cannot read flash info\n");
	return(0);
    }
    return(total_flash);
}

/* end of module */
