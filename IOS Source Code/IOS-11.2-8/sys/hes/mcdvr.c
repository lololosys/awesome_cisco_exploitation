/* $Id: mcdvr.c,v 3.4.42.2 1996/08/28 12:45:26 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/mcdvr.c,v $
 *------------------------------------------------------------------
 * mcdvr.c - MC and MC+ card Flash and NVRAM driver support
 *
 * July 1991, Howard Straus
 * Aug 1991, Darren Kerr
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * MCI-specific flash routines pulled from Chris Shaker
 * and Steve Elias' "mc.c" (now "mc.c" contains only
 * high-level flash routines, while "mcdvr.c" and "ec.c"
 * contain low-level flash drivers).
 *------------------------------------------------------------------
 * $Log: mcdvr.c,v $
 * Revision 3.4.42.2  1996/08/28  12:45:26  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.42.1  1996/05/17  11:18:50  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.30.2  1996/05/02  22:04:01  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.30.1  1996/04/03  14:31:01  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4  1995/12/21  16:56:35  hampton
 * Remove old timer related declarations or obsolete conditionally
 * compiled code.  [CSCdi45965]
 *
 * Revision 3.3  1995/11/17  09:19:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:29:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:41:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  13:04:47  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:42:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "../dev/flash_private.h"
#include "../dev/dev.h"
#include "../hes/if_mci.h"
#include "logger.h"
#include "../dev/msgtxt_flash.h"
#include "../dev/msg_flash.c"		/* Not a typo, see logger.h */

extern void (*usec_delay)(unsigned);

/*
 * Temporary grody hack:  take a copy of this from dev/flash_dvr.c
 * because we don't link flash_dvr.c 
 */
const char flash_writefail_msg[] = 
    "failed at location 0x%08x, wrote 0x%08x, read 0x%08x";

const char flash_erasefail_msg[] = "failed at bank %d";


hwidbtype *flash_nv_idbp;
extern flash_t flash;

static int mci_flash_init(void);
static int mci_flash_open(int);
static int mci_flash_close(void);
static int mci_flash_ioctl(ulong, int, void *);
static int mci_flash_read(ulong , char *, int );
static int mci_flash_write(ulong , char *, int );
static int mc_flash_command(uchar );
static int mc_erase_rom_mci(void);
static int mc_zero_rom_mci(void);
static int mc_erase_rom(void);
static int mc_zero_rom(void);
static int mc_flash_vpp(int );

devfun_t mci_flash_fun = {
    mci_flash_init,    /* initialization function address */
    mci_flash_open,    /* open function address */
    mci_flash_close,   /* close function address */
    NULL,              /* no interrupt routine */
    mci_flash_ioctl,   /* ioctl function address */
    mci_flash_read,    /* read function address */
    mci_flash_write    /* write function address */
    };


/**************************************************************************
 *
 *               FLASH DRIVER ROUTINES
 *
 *************************************************************************/


/*
 * mci_flash_init
 */
static int mci_flash_init (void)
{
    if (!flash_nv_idbp || !(flash_nv_idbp->status & IDB_MCI))
	return DEVERR_NODEV;
    return DEV_OK;
}


/*
 * mci_flash_open
 */
static int mci_flash_open (int flags)
{
    if (!flash_nv_idbp)
	return DEVERR_NODEV;
    return DEV_OK;
}


/* 
 * mci_flash_close
 */

static int mci_flash_close (void)
{   
    mc_flash_vpp(VPP_OFF);        /* for safety's sake */
    return(DEV_OK);
}


/*
 * mci_flash_ioctl
 * handles the IOCTL functions of the MC flash ROM driver 
 */

static int mci_flash_ioctl (ulong ind, int cmd, void *buf)
{
    long param = *(long *)buf;
    
    switch (cmd) {

    case IOC_FLASH_VPP:
	/* turn on/off programming voltage */
	return mc_flash_vpp(param);
	
    case IOC_FLASH_INFO:
	return mc_flash_command(FLASH_INFO_CMD);
	
    case IOC_FLASH_RESET:
	mc_flash_command(FLASH_RESET_CMD);
	return mc_flash_command(FLASH_RESET_CMD);
	
    case IOC_FLASH_READ:
	return mc_flash_command(FLASH_READ_CMD);

    case IOC_FLASH_ZERO:
	/* zero bank at current offset */
	if (flash_nv_idbp->status & IDB_MCI)
	    return mc_zero_rom_mci();
	else
	    return mc_zero_rom();
	
    case IOC_FLASH_ERASE:
	/* erase bank at current offset */
	if (flash_nv_idbp->status & IDB_MCI)
	    return mc_erase_rom_mci();
	else
	    return mc_erase_rom();
	
    case IOC_FLASH_SEEK:          /* position current pointer */
	if (param <= flash.f_maxseek) {
	    flash.f_cur = param;
	    return DEV_OK;
	} else {
	    flash.f_cur = 0;
	    return DEVERR_SEEK_PAST_EOM;
	}
	break;
	
    default:
	return DEVERR_INVALID_OP;
    }
}


/* 
 * mci_flash_read
 * read 'len' bytes from the flash ROM, starting from the current BYTE
 * position flash.f_cur.  
 * parameters:   ulong ind;  flash ROM handle (unused)
 *               char *buf;  buffer for the read data
 *               int len;  maximum read length (byutes)
 * returns:      int;      number of bytes actually read
 */

static int mci_flash_read (ulong ind, char *buf, int len)
{
    ulong flash_addr;
    ushort sval;
    int rem;
    
    flash_addr = MC_ROM_ADDRESS + flash.f_cur / 2;

    rem = len = MIN(len, flash.f_maxseek - flash.f_cur);
    
    while (rem > 0) {
	sval = mc_read_short(flash_addr++);
	if (flash.f_cur & 1) {
	    /* First byte on odd boundary, so high byte */
	    *buf++ = (char) (sval >> 8);
	    flash.f_cur++;
	    rem--;
	} else if (rem == 1) {
	    /* Last byte (get low byte in short) */
	    *buf = (char) (sval & 0xff);
	    flash.f_cur++;
	    rem = 0;
	} else {
	    /* At least 2 bytes on even boundary */
	    *(ushort *)buf = sval;
	    flash.f_cur += 2;
	    buf += 2;
	    rem -= 2;
	}
        if (systeminit_flag && !schedflag && ((flash.f_cur & 0xff) == 0xfe))
            process_suspend(); /* So flash doesn't starve others */
    }
    return len;
}


/*
 * mci_flash_write
 * program flash with the contents of the given buffer
 * parameters: char *buf;  buffer containing the data to program into flash
 *             uint len;  number of bytes in buffer
 * returns: int n;  >= 0 the number of characters actually written to flash 
 *                  <0   error code
 */

static int mci_flash_write (ulong ind, char *buf, int len)
{
    long flash_addr, rem;
    ushort sval;
    
    flash_addr = MC_ROM_ADDRESS + (flash.f_cur / 2);
    
    rem = len = MIN(len, flash.f_maxseek - flash.f_cur);
    
    while (rem > 0) {
	if (flash.f_cur & 1) {
	    /* First byte on odd boundary */
	    sval = mc_read_short(flash_addr) & 0x00FF;
	    sval |= (*buf++ << 8);
	    flash.f_cur++;
	    rem--;
	} else if (rem == 1) {
	    /* Last byte */
	    sval = mc_read_short(flash_addr) & 0xFF00;
	    sval |= *buf;
	    flash.f_cur++;
	    rem = 0;
	} else {
	    /* At least 2 bytes on even boundary */
	    sval = *(ushort *) buf;
	    flash.f_cur += 2;
	    buf += 2;
	    rem -= 2;
	}
	if (!mc_program_rom(sval, flash_addr++))
	    return DEVERR_WRITE_FLASH;
	
	if ((flash.f_cur & 0xff) == 0xfe)
	    process_suspend();
    }
    return len;
}


/***************************************************************************
 *
 *                Flash Support Routines
 *
 ***************************************************************************/

/*
 * Issue flash part command to currently seeked address.
 */
static int mc_flash_command (uchar cmd)
{
    uint mc_address;
    leveltype SR;
    
    mc_address = MC_ROM_ADDRESS + flash.f_cur/2;
    
    SR = raise_interrupt_level(ETHER_DISABLE);
    mc_write_short((cmd | (cmd << 8)), mc_address);
    reset_interrupt_level(SR);
    return 0;
}

/*
 * mc_erase_roms_mci
 * Erase the flash EPROMs.
 * Return TRUE if EPROMs are successfully erased, else, return FALSE.
 * Optimized to speed up mci flash erase
 */
static int mc_erase_rom_mci (void)
{
    int  pulsecount, i;
    uint flash_addr;
    leveltype SR;
    ushort data, fcmd;
    mcitype *mci = flash_nv_idbp->devctl;
    register ulong addr, high_addr, low_addr;
    ushort fever = (FLASH_ERASE_VER | (FLASH_ERASE_VER << 8));
    
    flash_addr =  MC_ROM_ADDRESS + flash.f_cur / 2;

    /* I know its ugly, but inlining all those mc_write_short()'s
     * improved the erase time by > 30%.  Even so, it's slow!
     */
    SR = raise_interrupt_level(ETHER_DISABLE);
    for (i=0, pulsecount=0; i<flash.f_chipsize; ) {
	addr = flash_addr + i;
	high_addr = (addr >> 16);
	low_addr = (addr & 0xFFFF);

	mci->regptr->argreg = high_addr;
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	mci->regptr->argreg = low_addr;
	mci->regptr->argreg = MCI_CMD_MEMX_SELECT_LOW;
	*((volatile ushort *)&mci->regptr->memxreg) = fever;
        (void) (*usec_delay)(6);

	mci->regptr->argreg = high_addr;
	mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	mci->regptr->argreg = low_addr;
	mci->regptr->argreg = MCI_CMD_MEMX_SELECT_LOW;
	data = *((volatile ushort *)&mci->regptr->memxreg);

	if (data == (ushort) 0xffff) {
	    i++;
	    if ((i & 0xFF) == 0xFF) {
		reset_interrupt_level(SR);
		process_suspend();
		SR = raise_interrupt_level(ETHER_DISABLE);
	    }
	} else {

	    if (pulsecount++ > 1000) {
		reset_interrupt_level(SR);
		errmsg(&msgsym(ERASEFAIL, FLASH), i);
		return -1;
	    }
	    if ((data & 0x00ff) == 0xff) 
		fcmd = FLASH_READ_CMD;
	    else 
		fcmd = FLASH_ERASE_CMD;
	    if ((data & 0xff00) == 0xff00)
		fcmd |= (FLASH_READ_CMD << 8);
	    else
		fcmd |= (FLASH_ERASE_CMD << 8);
	    
	    mci->regptr->argreg = high_addr;
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	    mci->regptr->argreg = low_addr;
	    mci->regptr->argreg = MCI_CMD_MEMX_SELECT_LOW;
	    *((volatile ushort *)&mci->regptr->memxreg) = fcmd;

	    mci->regptr->argreg = high_addr;
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	    mci->regptr->argreg = low_addr;
	    mci->regptr->argreg = MCI_CMD_MEMX_SELECT_LOW;
	    *((volatile ushort *)&mci->regptr->memxreg) = fcmd;

	    DELAY(10); /* DON'T process_sleep_for - must be within 500 us */

	    mci->regptr->argreg = high_addr;
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	    mci->regptr->argreg = low_addr;
	    mci->regptr->argreg = MCI_CMD_MEMX_SELECT_LOW;
	    *((volatile ushort *)&mci->regptr->memxreg) = fever;

	    reset_interrupt_level(SR);
	    process_suspend();
	    SR = raise_interrupt_level(ETHER_DISABLE);
	}
    }
    reset_interrupt_level(SR);
    flash.f_cur += flash.f_chipsize * 2;
    return 0;
}

/* 
 * mc_zero_rom_mci
 * Zero the flash EPROMs at both banks from the current seek.
 * Optimized to speed up mci flash zero
 */
static int mc_zero_rom_mci (void)
{
    ulong flash_addr;
    int i, pulsecount;
    leveltype SR;
    ushort value;
    ushort fpcmd = (FLASH_PROG_CMD | (FLASH_PROG_CMD << 8)); 
    ushort fpver = (FLASH_PROG_VER | (FLASH_PROG_VER << 8)); 
    ushort frcmd = (FLASH_READ_CMD | (FLASH_READ_CMD << 8)); 
    mcitype *mci = flash_nv_idbp->devctl;
    register ulong addr, high_addr, low_addr; 
    
    flash_addr = MC_ROM_ADDRESS + (flash.f_cur / 2);
    
    SR = raise_interrupt_level(ETHER_DISABLE);    
    for (i=0; i<flash.f_chipsize; i++) {
	addr = flash_addr+i;
	high_addr = addr >> 16;
	low_addr = addr & 0xFFFF; 
	for (pulsecount=0; pulsecount<25; pulsecount++) {
	    mci->regptr->argreg = high_addr;
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	    mci->regptr->argreg = low_addr;
	    mci->regptr->argreg = MCI_CMD_MEMX_SELECT_LOW;
	    *((volatile ushort *)&mci->regptr->memxreg) = fpcmd;
	    
	    mci->regptr->argreg = high_addr;
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	    mci->regptr->argreg = low_addr;
	    mci->regptr->argreg = MCI_CMD_MEMX_SELECT_LOW;
	    *((volatile ushort *)&mci->regptr->memxreg) = (ushort) 0x0;
	    (*usec_delay)(10);
	    
	    mci->regptr->argreg = high_addr;
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	    mci->regptr->argreg = low_addr;
	    mci->regptr->argreg = MCI_CMD_MEMX_SELECT_LOW;
	    *((volatile ushort *)&mci->regptr->memxreg) = fpver;
	    (*usec_delay)(6);
	    
	    mci->regptr->argreg = high_addr;
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	    mci->regptr->argreg = low_addr;
	    mci->regptr->argreg = MCI_CMD_MEMX_SELECT_LOW;
	    value = *((volatile ushort *)&mci->regptr->memxreg);
	    
	    mci->regptr->argreg = high_addr;
	    mci->regptr->cmdreg = MCI_CMD_MEMX_SELECT_HIGH;
	    mci->regptr->argreg = low_addr;
	    mci->regptr->argreg = MCI_CMD_MEMX_SELECT_LOW;
	    *((volatile ushort *)&mci->regptr->memxreg) = frcmd;
	    
	    if (value == 0x0) {
		break;
	    } 
	}
	if (value != 0x0) {
	    reset_interrupt_level(SR);
	    printf("\nFlash write: ");
 	    printf(flash_writefail_msg, addr, 0x0, value);
 	    errmsg(&msgsym(WRITEFAIL, FLASH), addr, 0x0, value);
	    return -1;
	}
	if ((i & 0xFF) == 0xFF) {
	    reset_interrupt_level(SR);
	    process_suspend(); 
	    SR = raise_interrupt_level(ETHER_DISABLE);    
	}
    }
    reset_interrupt_level(SR);
    flash.f_cur += flash.f_chipsize * 2;
    return 0;
}

/*
 * mc_erase_roms
 * Erase the flash EPROMs.
 * Return TRUE if EPROMs are successfully erased, else, return FALSE.
 */

static int mc_erase_rom (void)
{
    int  pulsecount, i;
    uint flash_addr;
    leveltype SR;
    ushort data, fcmd;
    
    flash_addr =  MC_ROM_ADDRESS + flash.f_cur / 2;

    for (i=0, pulsecount=0; i<flash.f_chipsize; ) {
	mc_write_short((FLASH_ERASE_VER | (FLASH_ERASE_VER << 8)),
		       flash_addr + i);
        (void) (*usec_delay)(6);

	data = mc_read_short(flash_addr + i);
	if (data == (ushort) 0xffff) {
	    i++;
	    if ((i & 0xFF) == 0xFF)
		process_suspend();
	} else {
	    if (pulsecount++ > 1000) {
		errmsg(&msgsym(ERASEFAIL, FLASH), i);
		return -1;
	    }
	    if ((data & 0x00ff) == 0x00ff)
		fcmd = FLASH_READ_CMD;
            else
                fcmd = FLASH_ERASE_CMD;
	    if ((data & 0xff00) == 0xff00)
		fcmd |= (FLASH_READ_CMD << 8);
	    else
		fcmd |= (FLASH_ERASE_CMD << 8);
	    SR = raise_interrupt_level(ALL_DISABLE);
	    mc_write_short(fcmd,flash_addr + i);
	    mc_write_short(fcmd,flash_addr + i);
	    DELAY(10); /* DON'T process_sleep_for - must be within 500 us */
	    mc_write_short((FLASH_ERASE_VER | (FLASH_ERASE_VER << 8)),
			   flash_addr + i);
	    reset_interrupt_level(SR);
	    process_suspend();
	}
    }
    return 0;
} 


/*
 * mc_zero_rom
 * Zero the flash EPROMs at both banks from the current seek.
 */

static int mc_zero_rom (void)
{
    ulong flash_addr;
    int   i; 
    
    flash_addr = MC_ROM_ADDRESS + (flash.f_cur / 2);

    for (i=0; i<flash.f_chipsize; i++) {
	if (!mc_program_rom(0x0, flash_addr+i))
	    return -1;
	if ((i & 0xFF) == 0xFF)
	    process_suspend();
    }
    flash.f_cur += flash.f_chipsize * 2;
    return 0;
}


/*
 * mc_program_rom
 * Write the specified value to flash EPROM at the specified address
 * vpp must be asserted before calling mc_program_rom()
 */
boolean mc_program_rom (ushort data, uint address)
{
    int pulsecount;
    ushort readback;
    leveltype SR;
    
    SR = raise_interrupt_level(ALL_DISABLE);
    for (pulsecount=0; pulsecount<25; pulsecount++) {
	mc_write_short((FLASH_PROG_CMD | (FLASH_PROG_CMD << 8)), address);
	mc_write_short(data, address);
        (*usec_delay)(10);
	mc_write_short((FLASH_PROG_VER | (FLASH_PROG_VER << 8)), address);
        (*usec_delay)(6);
	readback = mc_read_short(address);
	if ( readback == data) {
	    mc_write_short((FLASH_READ_CMD | (FLASH_READ_CMD << 8)), address);
	    reset_interrupt_level(SR);
	    return(TRUE);
	}
    }
    reset_interrupt_level(SR);
    mc_write_short((FLASH_READ_CMD | (FLASH_READ_CMD << 8)), address);  
    printf("\nFlash write: ");
    printf(flash_writefail_msg, address, data, readback);
    errmsg(&msgsym(WRITEFAIL, FLASH), address, data, readback);
    return(FALSE);    
}


/*
 * Set VPP ON or OFF, and verify that the VPP control register
 * responded back (this is used to probe for MC+ presence).
 */
static int mc_flash_vpp (int on_off)
{    
    mc_write_short(on_off, MC_VPP_CONTROL);
    flash.f_vpp_on = on_off;
    DELAY(1);
    if ((VPP_MASK & mc_read_short(MC_VPP_CONTROL)) != on_off)
	return -1;
    flash.f_vpp_on = on_off;
    return 0;
}

/***************************************************************************
 *
 *                MC,MC+ MCI Access Routines
 *
 ***************************************************************************/


/*
 * mc_read_short
 * Read one sixteen bit value from the MC card at the specified address:
 */
ushort mc_read_short(uint MC_addr)
{
    ushort value;

    value = mci_mc_read_short(flash_nv_idbp, MC_addr);
    return(value);
}

/*
 * mc_write_short
 * Write one sixteen bit value to the MC card at the specified address:
 */
void mc_write_short(ushort value, uint MC_addr)
{
    mci_mc_write_short(flash_nv_idbp, value, MC_addr);
}

/**************************************************************************
 *
 *               NVRAM DRIVER ROUTINES
 *
 *************************************************************************/

#ifdef NVRAM_DRIVER

static struct mci_nvram {
    ulong n_curr;
    ulong n_stat;
    nvtype *n_nvp;
    boolean n_writ;
} mciNvram;

devfun_t nvram_driver = {
    nv_dvr_init,    /* initialization function address */ 
    nv_dvr_open,    /* open function address */
    nv_dvr_close,   /* close function address */
    NULL,           /* no interrupt routine */
    nv_dvr_ioctl,   /* ioctl function address */
    nv_dvr_read,    /* read function address */
    nv_dvr_write    /* write function address */
    };

/* 
 * nv_dvr_init
 */

int nv_dvr_init (void)
{
    mciNvram.n_curr = 0;
    mciNvram.n_stat = DEV_CLOSED;
    mciNvram.n_writ = FALSE;
    mciNvram.n_nvp = NULL;
    
    if (!flash_nv_idbp)
	return DEVERR_NODEV;
    nvsize = mc_nvsize(); 
    return(DEV_OK);
}


/*
 * support for NVRAM driver routines: open, close, ioctl, read, write
 * no support for interrupts, since NVRAM operates with no interrupts
 */

int nv_dvr_open (int flags)
{
    if (nvsize == 0)                  /* if no NVRAM available */
	return(DEVERR_NO_NVRAM);      /* return error code */
    mciNvram.n_stat = DEV_OPEN;            /* open the device */
    if ((mciNvram.n_nvp = (nvtype *)nv_getptr()) == NULL) {
	mciNvram.n_stat = DEV_CLOSED;
	return(DEVERR_INSUFF_MEMORY); /* get NVRAM to RAM buffer */
    }
    mciNvram.n_curr = 0;                   /* starting point */
    mciNvram.n_writ = FALSE;               /* true if data changed */
    return(DEV_OK);                 /* return success */
}


/* 
 * nv_dvr_close
 * close NVRAM for any operations, insuring that all changes made in the
 * NVRAM shadow buffer (if any) have been replicated into the NVRAM itself.
 * parameters: none.
 * returns: DEV_OK - successful completion.
 */

int nv_dvr_close (void)
{   
    if (mciNvram.n_nvp) {                  /* if a pointer exists */
	(char *)mciNvram.n_nvp->textptr = (char *)mciNvram.n_nvp->textbase = 
	    (char *)mciNvram.n_nvp + sizeof(nvtype); /* adjust pointers */
	if (mciNvram.n_writ)               /* if any data changed */
	    nv_writeenable(mciNvram.n_nvp); /* write out the NVRAM */
	nv_done(mciNvram.n_nvp);           /* release the buffer */
        mciNvram.n_nvp = NULL;             /* clear the pointer */
    }
    mciNvram.n_stat = DEV_CLOSED;
    return(DEV_OK);
}


/*
 * nv_dvr_ioctl
 * handles the IOCTL functions of the MC NVRAM driver 
 * parameters: int type; type of operation desired
 *                        0: erase NVRAM completely
 *                        1: set current position pointer 
 *             int param; desired position pointer for type == 2
 *             char *buf; unused for NVRAM.
 * returns: int er; DEV_OK: successful operation
 *                  DEVERR_SEEK_PAST_EOM: attempt to seek past end of NVRAM
 *                  DEVERR_INVALID_OP: invalid operation
 */

int nv_dvr_ioctl (int ind, int cmd, char *buf)
{
    int r;
    ulong *lp;              /* pointer for the clear */
    long i;
    
    r = DEV_OK;             /* preset return */
    
    switch (cmd) {
	
    case IOC_ERASE:
	if (!mciNvram.n_nvp)           /* any pointer present */
	    break;                /* no, don't try to store at it */
	lp = (ulong *)mciNvram.n_nvp;  /* completely erase NVRAM */
	for (i = 0; i < nvsize / sizeof(long); i++) /* for all NVRAM */
	    *lp++ = 0L;           /* clear the next long word */
	printf("\n%d bytes of NVRAM cleared.", nvsize);
	mciNvram.n_writ = TRUE;        /* force write of buffer */
	mciNvram.n_curr = 0;           /* set current pointer to beginning */
	break;
	
    case IOC_SEEK:          /* position current pointer */
	if (*((ulong *)buf) >= nvsize) {
	    r = DEVERR_SEEK_PAST_EOM;
	    mciNvram.n_curr = nvsize;
	}
	else {
	    mciNvram.n_curr = *((ulong *)buf);
	}
	break;
	
    default:
	r = DEVERR_INVALID_OP;
	break;
    }
    return(r);
}




/*
 * nv_dvr_read
 * read the next len bytes from NVRAM to the specified buffer
 * parameters: int ind; unused index into driver table
 *             char *buf;  buffer address
 *             int len;  maximum transfer size
 * returns:    int result;  if positive, bytes actually transferred
 *                           else, error code.
 */

int nv_dvr_read (int ind, char *buf, int len)
{ 
    int actual;
    
    if (mciNvram.n_stat != DEV_OPEN) /* if device is not open */
	return(DEVERR_CLOSED);  /* return with an error */
    if (!mciNvram.n_nvp) {           /* if NVRAM not in RAM buffer */
	mciNvram.n_nvp = (nvtype *)nv_getptr(); /* fetch it */
	mciNvram.n_curr = 0;         /* set the current position */
    }
    if (mciNvram.n_curr >= nvsize)   /* if current is at EOM */
	return(DEVERR_EOF);     /* return EOF indication */
    actual = len;               /* attempt to read len bytes */       
    if (mciNvram.n_curr + actual > nvsize) 
	actual = nvsize - mciNvram.n_curr;
    memcpy(buf, (uchar *)mciNvram.n_nvp + mciNvram.n_curr, actual);
    mciNvram.n_curr += actual;       /* set the pointer for next operation */
    return(actual);
}



/*
 * nv_dvr_write(ind, buf, len)
 * write len bytes from the buffer to the NVRAM
 * parameters: int ind; unused index into driver table
 *             char *buf; buffer from which to get characters
 *             int len; number of characters to transfer
 * returns: int er; (>= 0) number of characters actually written 
 *                  (<0) - error code
 */

int nv_dvr_write (int ind, char *buf, int len)
{
    
    if (mciNvram.n_stat != DEV_OPEN)  /* is device available for writing */
	return(DEVERR_CLOSED);   /* no, return error */
    if (mciNvram.n_curr + len > nvsize) /* longer than NVRAM */
	return(DEVERR_WRITE_PAST_EOM); /* return error */
    if (!mciNvram.n_nvp)
	return(DEVERR_NO_NVRAM); /* no buffer pointer allocated */
    memcpy((uchar *)mciNvram.n_nvp + mciNvram.n_curr, buf, len); /* copy data into NVRAM */
    mciNvram.n_curr += len;
    mciNvram.n_writ = TRUE;
    return(len);
}

/***************************************************************************
 *
 *                      Misc. NVRAM Routines
 *
 ***************************************************************************/

/* 
 * mc_nvsize
 * Returns the size of NVRAM on the MC or MC+ card
 * method: for up to 256 Kbytes, checks each word of NVRAM for presence or
 * absence.  the current contents are saved, the location written, read then
 * restored.  
 */
ulong mc_nvsize (void)
{   ulong i;
    ulong n;
    ushort actual;
    ushort save;
    ulong addr;
    
    addr = MC_NVRAM_ADDRESS;  /* start at beginning of NVRAM */
    n = 0;                    /* total bytes of NVRAM available */
    for (i = 0; i < 0x20000L; addr++, i++) {  /* test up to 128K words (256K bytes) */
	save = mc_read_short(addr); /* get the word which was there */
	mc_write_short(0xDEAF, addr); /* write out a test pattern */
	actual = mc_read_short(addr); /* read it back in */
	mc_write_short(save, addr); /* restore the old word */
	if (actual != 0xDEAF) /* if the read was incorrect */
	    break;            /* stop the loop */
	n += sizeof(ushort);  /* increment the byte count */
    }
    return(n);
}

#endif /* NVRAM_DRIVER */


/* *** END-OF-FILE MCDVR.C *** */
