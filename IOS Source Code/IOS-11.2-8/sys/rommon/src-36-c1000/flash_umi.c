/* $Id: flash_umi.c,v 3.2.10.1 1996/03/21 23:29:00 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/flash_umi.c,v $
 *------------------------------------------------------------------
 * Driver for UMI platforms flash
 *
 * Sept 1995, Eric Fronberg
 *
 * Copyright (c) 1995 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_umi.c,v $
 * Revision 3.2.10.1  1996/03/21  23:29:00  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.4.2  1996/03/19  01:01:12  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.4.1  1995/12/01  04:21:56  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1996/02/23  17:15:51  cshen
 * CSCdi48454:  Need to support on-board flash on c1005 derivative
 *
 * Revision 3.1  1995/11/09  13:07:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/22  04:36:18  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.1  1995/09/22  00:50:59  enf
 * Placeholders for frankenstorm/umi
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** Universal flash driver.
** This code will handle all of the different flash devices and geometry
** currently in cisco products.
*/
#include "monitor.h"
#include "mon_defs.h"
#include "flash.h"
#include "error.h"
#include "signal_4k.h"

#define MAXFLASHERASE 3000

#define ERASECMD \
	(F_ERASE | (F_ERASE << 8) | (F_ERASE << 16) | (F_ERASE << 24))
#define ERASECONFIRM \
	(F_ERASE_CONFIRM | (F_ERASE_CONFIRM << 8) | \
	(F_ERASE_CONFIRM << 16) | (F_ERASE_CONFIRM << 24))
#define ERASESTAT \
	(F_ERASE_STAT | (F_ERASE_STAT << 8) | (F_ERASE_STAT << 16) | \
	(F_ERASE_STAT << 24))
#define ERASEVER \
	(F_ERASE_VERIFY | (F_ERASE_VERIFY << 8) | (F_ERASE_VERIFY << 16) | \
	(F_ERASE_VERIFY << 24))
#define WRSTATRDY \
	(F_WR_STAT_RDY | (F_WR_STAT_RDY << 8) | (F_WR_STAT_RDY << 16) | \
	(F_WR_STAT_RDY << 24))
#define BYTEWRSTAT \
	(F_BYTE_WR_STAT | (F_BYTE_WR_STAT << 8) | (F_BYTE_WR_STAT << 16) | \
	(F_BYTE_WR_STAT << 24))
#define CLRSTATREG \
	(F_CLEAR_STAT_REG | (F_CLEAR_STAT_REG << 8) | \
	(F_CLEAR_STAT_REG << 16) | (F_CLEAR_STAT_REG << 24))
#define READMEM \
	(F_READ_MEMORY | (F_READ_MEMORY << 8) | (F_READ_MEMORY << 16) | \
	(F_READ_MEMORY << 24))
#define RDSTATCMD \
	(F_READ_STAT_REG | (F_READ_STAT_REG << 8) | (F_READ_STAT_REG << 16) | \
	(F_READ_STAT_REG << 24))
#define PROGCMD \
	(F_PROGRAM | (F_PROGRAM << 8) | (F_PROGRAM << 16) | (F_PROGRAM << 24))
#define PROGVER \
	(F_PROGRAM_VERIFY | (F_PROGRAM_VERIFY << 8) | \
	(F_PROGRAM_VERIFY << 16) | (F_PROGRAM_VERIFY << 24))
#define RESETCMD \
	(F_RESET | (F_RESET << 8) | (F_RESET << 16) | (F_RESET << 24))
#define VPPSTAT \
	(F_VPP_STAT | (F_VPP_STAT << 8) | (F_VPP_STAT << 16) | \
	(F_VPP_STAT << 24))

long tenmsecs, tenusecs, sixusecs;
long total_flash;
extern int netflashbooted;

/*
** The flash_id structure.
**
** This structure holds all of the information for the flash devices
** supported by the flash driver routines herein.
*/

struct flash_id flash_ids[] = {
    0x89B4, 0x912d, 0x20000, 0x20000, "INTEL 28F010",   0,
    0x89BD, 0x91BD, 0x40000, 0x40000, "INTEL 28F020",   0,
    0x01A7, 0x80E5, 0x20000, 0x20000, "AMD 28F010",     0,
    0x012A, 0x8054, 0x40000, 0x40000, "AMD 28F020",     0,
    0x1CD0, 0x380D, 0x40000, 0x40000, "M5M28F101P",     0,
    0x89A2, 0x9145, 0x100000,0x10000, "INTEL 28F008SA", FI_STATREG,
    0x89A0, 0x9105, 0x200000,0x10000, "INTEL 28F016SA", FI_STATREG,
};

#define MAXFLIDS (sizeof(flash_ids)/sizeof(struct flash_id))

static int
check_flash_end(struct flash_dvr_info *fdiptr,unsigned int check_base)
{
    unsigned char manu, device;
    volatile unsigned char *addr;
 
    addr = (volatile unsigned char *)fdiptr->flash_base;
    *addr = F_RESET;
    addr = (volatile unsigned char *)check_base;
    *addr = F_RESET;

    wastetime(sixusecs);
    *addr = F_READ_ID_CODES;
    wastetime(sixusecs);
    addr = (volatile unsigned char *)fdiptr->flash_base;
    manu = *addr;
    device = *(addr + fdiptr->flash_width);
    *addr = F_RESET;
    addr = (volatile unsigned char *)check_base;
    *addr = F_RESET;
 
#ifdef DEBUG
    printf("Manu code = %#2x, device code = %#2x\n",manu,device);
#endif
    return((manu << 8) | device);
}
 

static int
get_man_dev(struct flash_dvr_info *fdiptr)
{
    unsigned char manu, device;
    volatile unsigned char *addr;

    addr = (volatile unsigned char *)fdiptr->flash_base;
    *addr = F_RESET;
    wastetime(sixusecs);
    *addr = F_RESET;
    wastetime(sixusecs);
    *addr = F_READ_ID_CODES;
    wastetime(sixusecs);
    manu = *addr;
    device = *(addr + fdiptr->flash_width);
    *addr = F_RESET;
#ifdef DEBUG
    printf("Manu code = %#2x, device code = %#2x\n",manu,device);
#endif
    return((manu << 8) | device);
}


int sizeflash(void)
{
      struct flash_dvr_info fdi[MAXFLASHBANKS*2];
 
      fdi[0].flash_base = ADRSPC_FLASH_BASE;
      if(!get_flash_info(fdi))
           return(0);
      else return(total_flash);
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
    struct flash_dvr_info *init_fdiptr = fdiptr;

    tenmsecs = timer_calibrate(10000); /* 10ms delay */
    tenusecs = timer_calibrate(10);
    sixusecs = timer_calibrate(6);
    total_flash = 0;

    for(j=0; j<MAXFLASHSETS; j++) {
	fdiptr->fiptr = (struct flash_id *)0;  /* init */
	fdiptr->next = (struct flash_dvr_info *)0;

	if(chkberr(fdiptr->flash_base, BW_32BITS, 1)) {
	    if(j==0) {  /* first time thru loop? */
		printf("bus error while trying to access flash - addr = %#x\n",
		      fdiptr->flash_base);
		return(0);
	    }
badflashset:
	    fdiptr--;
	    fdiptr->next = (struct flash_dvr_info *)0;  /* NULL this out */
	    break;  /* all done */
	}

	flash_prog_en(fdiptr);  /* enable programming mode */
	fdiptr->flash_width = 
	    get_flash_width((volatile unsigned long *)fdiptr->flash_base);    
	fdiptr->flash_mask = 0;
	for(i=0; i<fdiptr->flash_width; i++) 
	    fdiptr->flash_mask = (fdiptr->flash_mask << 8) | 0xff;
	mandev = get_man_dev(fdiptr);
	flash_prog_dis(fdiptr);  /* disable programming mode */
	/*
	** Look for the device in the flash_id table.
	*/
	for(i=0; i<MAXFLIDS; i++) {

	if(mandev == flash_ids[i].dev_code)
 	    fdiptr->fiptr = &flash_ids[i];
	}
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
	
	/*
	** Figure out how many banks there are.
        */
	fdiptr->flash_banks = 1;  /* init */
	bankstart = fdiptr->flash_base;  /* init */
	for(i=0; i< (MAXFLASHBANKS-1); i++) {
	    bankstart += banksiz;  /* bump to next bankstart */
	    /* 
	     *stop sizing  the flash above ADRSPC_FLASH_END boundary. 
	     */
	    if(bankstart >= ADRSPC_FLASH_END)
		break;

	    /* check for a bus error when we read */
	    if(chkberr(bankstart, fdiptr->flash_width, 1)) {
#ifdef DEBUG
		printf("get_flash_info: bus error at %#x\n",bankstart);
#endif
		break;
	    }

            flash_prog_en(fdiptr);   /* enable programming mode */
            /* Check if we are reading the same chip again */
            if (mandev == check_flash_end(init_fdiptr,bankstart)) {
                flash_prog_dis(fdiptr);
                break;
            }
	    mandev = get_man_dev(fdiptr);
	    flash_prog_dis(fdiptr);  /* disable programming mode */
#ifdef DEBUG
	    printf("get_flash_info: got man_dev of %#x\n",mandev);
#endif

	    if(mandev != fdiptr->fiptr->dev_code) {  /* different mandev */
		fdiptr->next = fdiptr + 1;   /* set the next pointer */
		fdiptr->next->flash_base = bankstart;
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
#ifdef DEBUG
    fdiptr = init_fdiptr;
    i=0;
    while(fdiptr) {
	printf("\nflash driver info structure # %d\n\
flash base: %#x    flash width: %d     flash set size: %#x    num banks: %d\n",
	       i++, fdiptr->flash_base, fdiptr->flash_width, fdiptr->flash_size,
	       fdiptr->flash_banks);
	fdiptr = fdiptr->next;
    }
    printf("total flash is %#x\n",total_flash);
#endif
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
	*(unsigned long *)addr = rdstatcmd;
	break;
    case BW_16BITS:
	*(unsigned short *)addr = rdstatcmd;
	break;
    default:
	*(unsigned char *)addr = rdstatcmd;
	break;
    }
    wastetime(sixusecs);
    if(!pstat(addr, size, 1, stat, stat, 4000, &stat)) {
	printf("timeout on flash read status command\n");
	return(0);
    }
    return(stat);
}


/*
** program a location in flash
** it is assumed that flash programming is enabled
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

    if(!fdiptr->fiptr) {
	printf("%s - illegal flash driver info pointer\n", prog_err_msg);
	return(0);
    }

    /* does device have a status register? */
    statreg = fdiptr->fiptr->flags & FI_STATREG;

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
	case BW_32BITS:  /* dlword */
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
	*(volatile unsigned long *)location = rstcmd;
	wastetime(sixusecs);
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
** Return the appropriate fdiptr for the start address suplied.
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
*/

int
flash_cleanup()
{
    flash_prog_dis();
    return((int)SIG_DFL);  /* do the default action too */
}


/*
** erase_flash
**
** erase the specified flash EPROMs or sectors
** if size is 0, flash is erased from specified starting address
** to end
** return 1 if EPROMs are successfully erased, 0 otherwise
*/
int
erase_flash(struct flash_dvr_info *fdiptr, unsigned start, int size)
{
    int erasepulse, i;
    volatile unsigned char *sptr, *sectend;
    unsigned long data, erasecmd, readval, mask;
    int width;
    SIG_PF savsig;
    int retval = 1;  /* optimism */
    char eraseall = 0;  /* init */

    /*
    ** Make sure starting and ending addresses are on sector
    ** boundaries.
    ** First find the appropriate fdi structure.
    */
    if(size == 0) eraseall = 1;
    if(!(fdiptr = get_fdi_struct(fdiptr, start))) {
	printf("erase_flash: illegal start address\n");
	return(0);
    }
    if((start - fdiptr->flash_base) % fdiptr->flash_sector) {
	printf("erase_flash: start address not on sector boundary\n");
	return(0);
    }

    savsig = signal(SIGINT, (SIG_PF)flash_cleanup);
    flash_prog_en();
    while(fdiptr && (size || eraseall)) {
	if(size && size < fdiptr->flash_sector) {
	    printf("erase_flash: erase size is less than sector size of %#x\n",
		   fdiptr->flash_sector);
	    goto badreturn;
	}
	sptr = (volatile unsigned char *)start;
	sectend = (volatile unsigned char *)(start + fdiptr->flash_sector);
	mask = fdiptr->flash_mask;
	width = fdiptr->flash_width;

	if(fdiptr->fiptr->flags & FI_STATREG) {
	    /* 
	    ** We are using technology with a status register.
	    ** Erase the entire sector.
	    */
#ifdef DEBUG
	    printf("\rErasing flash EPROM sector at %#x...",sptr);
#endif
	    switch(width) {
	    case BW_32BITS:  /* lword */
		*(volatile unsigned long *)sptr = ERASECMD; 
		*(volatile unsigned long *)sptr = ERASECONFIRM;
		poll_flash_stat(sptr, WRSTATRDY, width, mask);
		if ( *(volatile unsigned long *)sptr & ERASESTAT) {  /* erase error? */
		    *(volatile unsigned long *)sptr = CLRSTATREG;  /* clear error */
staterr:
		    printf("sector erase failed at location %#x\n", sptr);
		    goto badreturn;
		}
		break;
	    case BW_16BITS:  /* word */
		*(volatile unsigned short *)sptr = (ERASECMD & mask); 
		*(volatile unsigned short *)sptr = (ERASECONFIRM & mask);
		poll_flash_stat(sptr, WRSTATRDY, width, mask);
		if ( *(volatile unsigned short *)sptr & (ERASESTAT & mask)) {  /* erase error? */
		    *(volatile unsigned short *)sptr = (CLRSTATREG & mask);  /* clear error */
		    goto staterr;
		}
		break;
	    default:  /* byte */
		*sptr = (ERASECMD & mask); 
		*sptr = (ERASECONFIRM & mask);
		poll_flash_stat(sptr, WRSTATRDY, width, mask);
		if ( *sptr & (ERASESTAT & mask)) {  /* erase error? */
		    *sptr = (CLRSTATREG & mask);  /* clear error */
		    goto staterr;
		}
		break;
	    }
	    sptr += fdiptr->flash_sector;
	} else {
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
				erasecmd |= ( F_ERASE << i );
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
    flash_prog_dis();
    signal(SIGINT, savsig);
    return(retval);
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
    extern setcacr();
    extern boolean ftest();
    boolean retval = 1;  /* pessimistic? */
    int mandev;
    struct flash_dvr_info fdi[MAXFLASHBANKS*2];
    char buffer [5];

    testname("flash memory");

    fdi[0].flash_base = ADRSPC_FLASH_BASE;
    if(!get_flash_info(fdi)) {
	cterr('w',0,"flash not available...aborting test");
	goto ret;
    }

    if(!ftest(fdi, (unsigned long *)0, 0, "data equals address" ) ) {
	goto ret;
    }
    /*
    ** Test for VPP off.
    **
    ** On devices with a status register, read status and make sure the VPP
    ** bit is 1 (VPP low).
    ** Other devices, make sure we cannot read the man/device code.
    */
    if(fdi[0].fiptr->flags & FI_STATREG) {
	*(unsigned char *)fdi[0].flash_base = F_READ_STAT_REG;
	mandev = *(volatile unsigned char *)fdi[0].flash_base;
	if(mandev & F_VPP_STAT == 0) {
vppfail:
	    cterr('f', 0, "VPP does not turn off (statreg = %#2x)", mandev);
	    goto ret;
	}
    } else {

	if((mandev = get_man_dev(fdi)) == fdi[0].fiptr->dev_code) {
	    cterr('f', 0, "VPP does not turn off (man_dev = %#4x)", mandev);
	    goto ret;
	}
    }

    if(testlength > 0) {
	if(!ftest(fdi, testarray1, 2, "0x55, 0xAA" ) ||
	   !ftest(fdi, testarray2, 2, "0xAA, 0x55" ) )
	    goto ret;
    }
	
    prcomplete(testpass, errcount, 0);
    retval = 0;
ret:
    flash_cleanup();
    return(retval);
}

int
ftest(struct flash_dvr_info *fdiptr, unsigned long *valptr, int numvals, 
      char *nameptr)
{
    volatile unsigned char *start,*end;
    volatile unsigned long  *ptr;
    unsigned  char *vptr, *vend,*flashptr;
    unsigned long rval, value ,flashval;

    prpass(testpass, "erase phase, size %#x,", total_flash);
    if ( !erase_flash(fdiptr, fdiptr->flash_base, 0) ) {
	cterr('f',0,"flash EPROMs will NOT erase...aborting\n");
	return(0);
    }
    prpass(testpass, "write phase, size %#x,", total_flash);
    flash_prog_en();

    start = (volatile unsigned char *)fdiptr->flash_base;
    end = (volatile unsigned char *)((long)start + fdiptr->flash_size);
    if( numvals ) vend = (unsigned char *)(valptr + numvals);
    else {
	valptr = &value;
	vend = (unsigned char *)(valptr + 1);
    }
    for ( ptr = (unsigned long *)start; ptr < (unsigned long *)end; ) {
	if( !numvals ) value  = (long)ptr;  /* data equals address test */

        vptr = (unsigned char *)valptr;                 /* value set */
	for(flashptr = (unsigned char *)ptr; 
            vptr < vend && ptr < (volatile unsigned long *)end; 
            vptr += fdiptr->flash_width, flashptr += fdiptr->flash_width) {
            switch(fdiptr->flash_width) {
            case BW_32BITS:  /* lword */
                rval = *(volatile unsigned long *)vptr;
                break;
            case BW_16BITS:  /* word */
                rval = *(volatile unsigned short *)vptr;
               break;
            default:  /* byte */
                rval = *vptr;
                break;
            }

            ptr = (unsigned long *)((unsigned long) ptr + fdiptr->flash_width);
	    if( !program_flash(flashptr, rval, fdiptr ) ) {
		cterr('f',0,"write failure at location %8x", ptr);
		flash_prog_dis();
		return(0);
	    }
#ifdef DEBUG
	    if( !((unsigned long)ptr % 0x1000)) putchar('w');
#endif
	}
    }
    flash_prog_dis();

    prpass(testpass, "verify phase, size %#x,", total_flash);

    for ( ptr =  (unsigned long *)start; ptr <  (unsigned long *)end; ) {
        if( !numvals ) value  = (long)ptr;  /* data equals address test */
        vptr =  (unsigned char *)valptr;                 /* value test */
	for(flashptr = (unsigned char *)ptr; 
            vptr < vend && ptr < (unsigned long *)end; 
            vptr += fdiptr->flash_width, flashptr += fdiptr->flash_width) {
	    switch(fdiptr->flash_width) {
	    case BW_32BITS:  /* lword */
		rval = *(volatile unsigned long *)vptr;
		flashval = *(volatile unsigned long *)flashptr;
		break;
	    case BW_16BITS:  /* word */
		rval = *(volatile unsigned short *)vptr;
		flashval = *(volatile unsigned short *)flashptr;
		break;
	    default:  /* byte */
		rval = *vptr;
		flashval = *flashptr;
		break;
	    }
            ptr = (unsigned long *)((unsigned long) ptr + fdiptr->flash_width);
	    if( rval !=  flashval) {
		cterr('f',0,"verification failure at location %8x: expected %8x, is %8x",
		       ptr, *vptr, rval);
		return(0);
	    }
#ifdef DEBUG
	    if( !((unsigned long)ptr % 0x1000)) putchar('v');
#endif
	}
    }
    return(1);
}

flash(argc, argv)
    char *argv[];
{
    int i, bootflash, size, retval=1, width, fcc_install, start;
    int debug, info, erase, prog;
    static char optstr[] = "bd";
    extern int optind;
    struct flash_dvr_info fdi;
    unsigned long source, dest, source_ptr, dest_ptr, source_end;
    unsigned long data;


    fcc_install = 0; /* init */
    bootflash = 0;
    debug = 0; info = 0;
    erase = 0; prog = 0;
    fdi.flash_base = ADRSPC_FLASH_BASE; /* address of system flash base */

     while((i = getopt(argc,argv,optstr)) >= 0) {
	switch(i) {
	case 'b':  /* boot flash */
	    bootflash = 1;
	    bootflash_addr(&fdi.flash_base,&fcc_install); /* address of bootflash base */
	    break;
	case 'd':  /* debug */
	    debug = 1;
	    break;
	default:
usage:
	    printf("usage: %s [-%s] info \n",argv[0],optstr);
	    printf("usage: %s [-%s] erase\n",argv[0],optstr);
	    printf("usage: %s [-%s] prog <source> <destination> <size>\n",argv[0],optstr);
	    puts("\
-b  boot flash\n\
-d  debug\n");
	    goto ret;
	}
    }

    if(fdi.flash_base == 0xffffffff){
	printf("\nThis hardware platform does not support boot flash\n");
	goto ret;
    }

    if (optind>=argc)
	goto usage;

    if(strcmp(argv[optind],"info") == 0) info = 1;
    if(strcmp(argv[optind],"erase") == 0) erase = 1;
    if(strcmp(argv[optind],"prog") == 0) prog = 1;

#ifdef DEBUG
    printf("\nargc = %#x, optind = %#x", argc, optind);
    printf("\ninfo = %#x, erase = %#x, prog = %#x\n",info,erase,prog);
#endif

    if(!(((info || erase)&&((argc - optind) == 1)&&(argc > 1)) || (prog &&((argc - optind) == 4)&&(argc > 4))))
	goto usage;

    if(debug)printf("\nnetboot = %#x, fcc_install = %#x, bootflash = %#x",netflashbooted, fcc_install, bootflash);
    if(!netflashbooted && !fcc_install && bootflash){
	printf("\nYou are executing out of boot flash, commands to the boot flash are not allowed\n");
	goto ret;
    }

    printf("\n%s flash info\n", bootflash ? "Boot" : "System");
    if(!get_flash_info(&fdi)) {
	printf("flash not available...aborting test\n");
	goto ret;
    } 

    if(info){
	printf("flash width  = %#x", fdi.flash_width);
	printf("\nflash sector = %#x", fdi.flash_sector);
	printf("\nflash size   = %#x", fdi.flash_size);
	printf("\nflash base   = %#x", fdi.flash_base);
	printf("\n");
	retval = 0;
	goto ret; /* done with info */
    }

    if(erase){
	for(start=fdi.flash_base;start<(fdi.flash_base+fdi.flash_size);start+=fdi.flash_sector){
	    printf("\rErasing flash at %#x",start);
	    if ( !erase_flash(&fdi, start, fdi.flash_sector) ) {
		printf("flash EPROMs will NOT erase...aborting\n");
		goto ret;
	    }
	}
	printf("\n");
	retval = 0;
	goto ret; /* done with erase */
    }

    getnum(argv[++optind],16,(long *)&source);
    getnum(argv[++optind],16,(long *)&dest);  
    getnum(argv[++optind],16,(long *)&size);
    source_end = source + size;

    if(debug) printf("\nsource = %#x, dest = %#x, size = %#x\n", source, dest, size);

    if((dest<fdi.flash_base) || (dest>(fdi.flash_base + fdi.flash_size)) || ((dest+size)>(fdi.flash_base + fdi.flash_size))) {
	printf("\nThe destination address or the size does not fit in flash");
	printf("\nPlease read flash_info first\n");
	goto ret;
    }
	
    width = fdi.flash_width; /* get the width of the data bus */
    flash_prog_en();
    for(source_ptr=source, dest_ptr=dest; source_ptr<source_end; source_ptr += width, dest_ptr += width){
	switch (width) {
	case BW_8BITS : /* 8 bit data bus*/
	    (unsigned char)data = *(unsigned char *)source_ptr; /* read the data */
	    break;

	case BW_16BITS : /* 16 bit data bus*/
	    (unsigned short)data = *(unsigned short *)source_ptr; /* read the data */
	    break;

	case BW_32BITS : /* 32 bit data bus*/
	    (unsigned long)data = *(unsigned long *)source_ptr; /* read the data */
	    break;
	}
	
	if(!(dest_ptr % 0x10000))printf("\rprogram flash location %#x", dest_ptr);
	if(!program_flash((unsigned char *)dest_ptr, (long)data, &fdi)){   /* burn data into flash */
	    printf("program flash failure at location %#x\n",dest_ptr);
	    goto ret;  
	}
    }
    printf("\n");
    flash_prog_dis();
    retval = 0;
ret:
    flash_cleanup();
    return(retval);
}





  
