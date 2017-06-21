/* $Id: flash_dvr.c,v 3.4.4.3 1996/08/28 12:40:25 thille Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_dvr.c,v $
 *------------------------------------------------------------------
 * flash_dvr.c - flash common driver support 
 *
 * July 1991, Howard Straus
 * Aug 1991, Darren Kerr
 * Mar 1992, Dit Morse
 * February 1993, Dit Morse
 *        - extracted common parts from flash_lesdvr.c
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_dvr.c,v $
 * Revision 3.4.4.3  1996/08/28  12:40:25  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.4.2  1996/08/02  22:52:50  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.4.4.1  1996/03/18  19:12:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/16  06:34:15  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.2  1996/03/07  08:43:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/01/24  21:18:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/03/11  11:45:58  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Deterge the code.
 *
 * Revision 3.3  1996/01/18  21:52:29  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  09:00:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/09  12:59:59  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:24:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *------------------------------------------------------------------
 */

/* #define DEBUG */

#include "master.h"
#include "logger.h"
#include "../dev/msg_flash.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "g_dev.h"

#if defined (RP1)
#include "../hes/flash_rp1.h"
#endif /* RP1 */

#include "../dev/flash_dvr_spec.h"
#include "../dev/flash_dvr_private.h"
#include "../dev/msgtxt_flash.h"

/*
 * Most flash error messages are in response to a user command, so
 * they are generally not sent to the logger.  If an error occurs
 * while programming or erasing flash, it probably indicates a 
 * hardware problem (e.g. flash been erased to many times).  So in
 * those cases we will both print a message to the user and send
 * a message to the logger.  Since the message is the same, the
 * strings below have been defined to be used in both cases. 
 */

const char flash_writefail_msg[] = 
    "failed at location 0x%08x, wrote 0x%08x, read 0x%08x";

const char flash_erasefail_msg[] = "failed at bank %d";

/* some run-time option control */
#define BUG(a)  if (a & bugflags) {
#define BUGELSE } else {
#define BUGEND  }

/* control flags for use in above macros */
#define BF_CACHE   0x0001   /* disable data cache */
#define BF_MASK    0x0002   /* use command masking */ 
#define BF_WRTRIES 0x0004
#define BF_BOOT    0x0008
#define BF_PROBE   0x0010
#define BF_ERASE   0x0020
#define BF_VERIFY  0x0040

ulong bugflags = BF_CACHE | BF_MASK;


flash_geom fg = {0};
flash_header flash_hdr = {0};



ulong   wp[MAX_ROMS] = {  0  }; /* number of write retries */
ulong   ep[MAX_ROMS] = {  0  }; /* number of erase pulses */



/**************************************************************************
 *
 *               FLASH les Initialization
 *
 * flash_dev_init       - returns pointer to device block
 *************************************************************************/
flash_dev_t *flash_dvr_init (void)
{   
  flash_dev_t *dev;
  int i;

    fg.fg_status       = FR_NOT_QUERIED;     /* not yet queried */
    fg.fg_chipcount    = 4;
    fg.fg_chipsize     = fg.fg_size / fg.fg_chipcount;
    fg.fg_banks        = 1;
    fg.fg_banksize     = fg.fg_size;
    fg.fg_erase_sector = fg.fg_chipsize; /* initialise to chipsize (default) */
    fg.fg_sectors      = 1;
    fg.fg_cmnd_set     = 0; /* initialize to unknown command set (default) */
    fg.fg_width        = FLASH_WIDTH;
    fg.fg_card         = "embedded flash";
    
    /* clear write pulse and erase pulse counts/chip */
    for ( i = 0; i < MAX_ROMS; i++) {
	fg.fg_rom_id[i] = 0;
        wp[i] = 0;
        ep[i] = 0;
    }

    dev = flash_dev_init();
    if (dev != NULL)
	fg.fg_devname = dev->dev_name;
    else
	fg.fg_devname = NULL;

    return(dev);
}

ulong flash_align (ulong addr)
{ 
    return((addr + (FLASH_WIDTH-1)) & ~(FLASH_WIDTH-1));
}

static inline void pz (char c, ulong *addr)
{
    if (((ulong)addr % 0x10000) == FLASH_WIDTH) { 
        printc(c); 
        flush(); 
    }
}

boolean run_from_flash(void) 
{
    if ( (ulong)(void *)flash_query_roms > fg.fg_flashbase )
	return(TRUE);
    else
	return(FALSE);
}

/***************************************************************************
 *
 *               query flash eproms
 * flash_query_roms
 * query_bank
 ***************************************************************************/

/*
 * flash_query_roms
 * Read the manufacturer and device codes from each device.
 * Compute the total amount of ROM.
 * This routine must use spin wait delays so that
 * it can be called from the scheduler context.
 */

ushort flash_query_roms (flash_dev_t *dev)
{
    uchar manu_buf[MAX_ROMS];
    uchar device_buf[MAX_ROMS];
    uchar id_buf[MAX_ROMS];
    uint b, j;
    boolean flash_ok = TRUE;    /* unless we discover otherwise */
    int id;
    ulong code;


    /* 
     * check to see if we need to query or not
     *
     * FR_NOT_QUERIED:  eproms not queried yet
     * FR_NOT_VALID:    no files, cannot write (no valid id's)
     * FR_READ_ONLY:    can always read if we got here.
     * FR_READ_WRITE:   valid to write
     */

    switch (fg.fg_status) {
      case FR_NOT_QUERIED:
	fg.fg_status = FR_NOT_VALID;   /* assume worst */
	break;
      case FR_NOT_VALID:
      case FR_READ_ONLY:
      case FR_READ_WRITE:
	return(fg.fg_status);
      default:
	return(FR_NOT_VALID);
    }

    /* check to see if run from flash - READ_ONLY */
    if (run_from_flash()) {
	printf("Flash: running from Flash, READ-ONLY.\n");
	return(fg.fg_status = FR_READ_ONLY);
    }

    if (!flash_assert_vpp()) {
	return(fg.fg_status);
    }
    
    /*
     *  Query each bank for manuf and device codes.
     *  For each chip in a bank, look up info table. 
     *  If chip not identifiable, quit with error
     *  else, if first chip in first bank, set up flash geometry
     *  else check chip size compatibility.
     */
    
    fg.fg_chipcount = 0;
    fg.fg_banks     = 0;
    
    /* query first bank and configure */
    query_bank(0, (ulong *)&manu_buf[0], (ulong *)&device_buf[0]);
    if ( 0 == (id = flash_identify(manu_buf[0] << 8 | device_buf[0])))
	{
	    flash_deassert_vpp();
	    printf("Flash: invalid chip id 0x%4x for flash\n", 
		   manu_buf[0] << 8 | device_buf[0]);
	    printf("       unable to configure flash for erase or write.\n");
	    flash_print_chips_known();
	    return(fg.fg_status);
	}

/* configure the geometry */

    fg.fg_erase_sector = flash_sector_size(id);
    fg.fg_chipsize     = flash_size(id);
    fg.fg_banksize     = fg.fg_chipsize * fg.fg_width;
    fg.fg_banks        = fg.fg_size / (fg.fg_width * fg.fg_chipsize);
    fg.fg_chipcount    = fg.fg_size / fg.fg_chipsize;
    fg.fg_sectors      = fg.fg_chipsize / fg.fg_erase_sector;
    fg.fg_cmnd_set     = flash_cmnd_set(id);
    fg.fg_size_cnfrm   = fg.fg_banksize * fg.fg_banks; /* set correct size */ 

    switch (fg.fg_cmnd_set) {
      case INTEL_010:
	fg.fg_program_rom  = prom_010;
	fg.fg_program_quad = prom_quad_010;
	fg.fg_erase_bank   = flash_erase_bank_010;
	break;
      case INTEL_008:
	fg.fg_program_rom  = prom_008;
	fg.fg_program_quad = prom_quad_008;
	fg.fg_erase_bank   = flash_erase_bank_008;
	break;
      case INVALID_SET:
      default:
	fg.fg_program_rom 
	    = (boolean (*)(ulong, volatile ulong *, ulong, int))prom_err;
	fg.fg_program_quad 
	    = (boolean (*)(ulong, volatile ulong *))prom_err;
	fg.fg_erase_bank   
	    = (boolean (*)(flash_dev_t *, int))prom_err;
	flash_ok = FALSE;
	break;
    }

    /*  query the remaining banks */
    for (b = 1; b < fg.fg_banks; b++) {
        query_bank(b*fg.fg_banksize, 
		   (ulong *)&manu_buf[b*fg.fg_width], 
		   (ulong *)&device_buf[b*fg.fg_width]);
    }
    flash_deassert_vpp();

    /* for each chip:
     *   - verify the manu/size code is in the table
     *   - check that the manu's are the same, and report warnings
     *   - check that sizes are the same, and report errors
     */
    for (j = 0; j < fg.fg_chipcount; j++) {
	code = fg.fg_rom_id[j] = (manu_buf[j] << 8) | device_buf[j];
	id = id_buf[j] = flash_identify(fg.fg_rom_id[j]);

	/* check for know chip id - error */
	if (id == 0) {		
	    printf("Flash: chip no %d, socket %s\n", 
		   j, flash_socket(j));
	    printf("       Invalid chip id 0x%4x\n", fg.fg_rom_id[j]);
	    flash_ok = FALSE;
	} else {
	    /* check for consistent id (warning) */
	    if ( flash_identify(fg.fg_rom_id[j]) != id_buf[0]) {
		printf("Flash: chip no %d, socket %s\n",
		       j, flash_socket(j));
                printf("       mismatched types: expected %04x: got %04x\n",
                       fg.fg_rom_id[0], fg.fg_rom_id[j]);
	    }
	    /* check for mismatched sizes - error */
	    if (fg.fg_chipsize != flash_size(id)) {
		printf("Flash: chip no %d, socket %s, ", 
		       j, flash_socket(j));
		printf("       mismatched sizes: expected 0x%x got: 0x%x\n",
		       fg.fg_chipsize, flash_size(id));
		flash_ok = FALSE;
	    }
	}
    }       /* end of loop for each chip */

    /* end game: */

    /* failed - id's not valid */    
    if (!flash_ok) { 
	printf("Flash: unable to configure flash for erase or write.\n");
	return(fg.fg_status = FR_NOT_VALID);
    }

    return(fg.fg_status = FR_READ_WRITE);
}


/***************************************************************************
 *
 * query_bank:
 *     read chip ids for bank
 *
 ***************************************************************************/
void query_bank (int bankaddr, 
		ulong *manu, 
		ulong *device)
{
    volatile ulong *ptr = (volatile ulong *)(fg.fg_flashbase + bankaddr);
    DCACHESTATUS;

    DCACHEOFF;
    *ptr = reset_cmd;
    *ptr = reset_cmd;
    WASTETIME(fg.fg_usec6);
    *ptr = read_id;
    WASTETIME(fg.fg_usec10); 
    *manu = *ptr++;
    *device = *ptr;
    *ptr = reset_cmd;
    DCACHEON;
}

/***************************************************************************
 *
 * flash_erase_bank
 * Erase the flash EPROMs.
 * Return TRUE if EPROMs are successfully erased, else, return FALSE.
 *
 ***************************************************************************/
boolean flash_erase_bank (flash_dev_t *dev, int bank)
{
    switch (fg.fg_cmnd_set) {
      case INTEL_010:
	return(flash_erase_bank_010(dev, bank));
      case INTEL_008:
	return(flash_erase_bank_008(dev, bank));
      default:
	return(FALSE);
    }
}

/********************************************************************* 
 *
 *                   flash read and write routines
 *
 * parameters:   dev;        flash ROM handle (unused)
 *               char *buf;  buffer for the read data
 *               char *addr; first addr in flash        
 *               len;        maximum read length (byutes)
 * returns:      ulong;      number of bytes actually read
 *
 *********************************************************************/

/********************************************************************* 
 *
 * pan_flash_read 
 *
 *********************************************************************/


ulong flash_read (uchar *buf, uchar *addr, uint len)
{ 
    bcopy(fg.fg_flashbase + addr, buf, len);
    return(len);
}

/***************************************************************************
 *
 *  pan_flash_write:
 *
 ***************************************************************************/

ulong flash_write (uchar *buf, uchar *addr, ulong len, boolean verbose)
{
    ulong *bufaddr     = (ulong *)buf;
    ulong *flash_addr  = (ulong *)((ulong)addr + fg.fg_flashbase);
    ulong  data = 0;
    ulong  mask;
    ulong  len_to_write = len;                   /* bytes to write*/
    int    blen;
    DCACHESTATUS
    
    /* note on counters:
     * we start len_to_write=len bytes to write,
     * len is the running count, which gets decremented by
     * bytes written.
     * we return (len_to_write-len) = bytes actually written
     */

    ulong endmask[4] = {
	0x0,
	0xff000000,
	0xffff0000,
	0xffffff00,
    };
    ulong beginmask;
    
    /* check limits here: 0 < addr:addr+len < max */
    
    if ( (ulong)addr > fg.fg_size) {
	if (verbose)
	    printf("Flash: initial address 0x%x beyond flash range of 0x%x.\n",
		   addr, fg.fg_size);
	return(0);
    }
    if ( ((ulong)addr + len) > fg.fg_size) {
	if (verbose)
	    printf("Flash: final address 0x%x beyond flash range of 0x%x.\n",
		   addr + len, fg.fg_size);
	return(0);
    }

    DCACHEOFF;
    
    /* 
     * check for and write unaligned head 
     */

    beginmask = 0;
    if ( (blen = (ulong)addr & (FLASH_WIDTH - 1)) != 0) {
	switch(blen) {
	  case 1:  /* three bytes ?? not necessary !!! */
	    data = data << 8 | *buf++;
	    beginmask = beginmask << 8 | 0xFF;
            if ( --len == 0) {
               data = data << 16;
	       beginmask = beginmask << 16;
               break;
            }
	  case 2:
	    data = data << 8 | *buf++;
	    beginmask = beginmask << 8 | 0xFF;
            if ( --len == 0) {
               data = data << 8;
	       beginmask = beginmask << 8;
               break;
            }
	  case 3:
	    data = data << 8 | *buf++;
	    beginmask = beginmask << 8 | 0xFF;
            len--;
	}
	flash_addr = (ulong *)(((ulong)flash_addr & ~(FLASH_WIDTH - 1)));
	if (!(*fg.fg_program_rom)(data, flash_addr, 
				   beginmask, WRITE_TRIES)) 
	    goto fl_wrt_exit;
	flash_addr++;
    }
    
    /* 
     * write aligned block:
     * fg.fg_program_quad - is fast aligned write
     * if it fails, the whole enchilada is eaten.
     */

    bufaddr = (ulong *)buf;
    while (len > (FLASH_WIDTH - 1)) {
	if (!(*fg.fg_program_quad)(*bufaddr, flash_addr)) {
	    data = *flash_addr;                 /* quad write failed */
	    mask = got_exp(data, *bufaddr, 0xffffffff);
	    inc_wp(mask, flash_addr);
	    if (!(*fg.fg_program_rom)(*bufaddr, flash_addr, 
				       mask, WRITE_TRIES)) 
		goto fl_wrt_exit;
	}
	bufaddr++; flash_addr++;
	len -= FLASH_WIDTH; 
    }
    
    /* 
     * check for and write tail of less than FLASH_WIDTH bytes
     */

    if (len > 0) {
        if (!(*fg.fg_program_rom)(*bufaddr, flash_addr, 
				   endmask[len], WRITE_TRIES))
            goto fl_wrt_exit;
	len -= len;
    }
  fl_wrt_exit:
    DCACHEON;
    return(len_to_write - len);  	/* return bytes written	*/
}

boolean zero_bank (int bank)
{
    ulong *flash_addr = (ulong *)(fg.fg_flashbase + fg.fg_banksize * bank);
    ulong  i; 
    ulong wmask, data;
    
    for (i=0; i<fg.fg_chipsize; i++, flash_addr++) {
	if (!(*fg.fg_program_quad)(0x0, flash_addr)) {
	    data = *flash_addr;
	    wmask = got_exp(data, 0x0, 0xffffffff);
	    inc_wp(wmask, flash_addr);  /* count this failure */
	    if (!(*fg.fg_program_rom)(0x0, flash_addr, 
				       wmask, ZERO_WRITE_TRIES))
		return(FALSE);
	}
	
	/* check suspend every 0x1000 writes, approx 10 ms or so */
	if ((i & 0xfff) == 0xfff) 
	    process_may_suspend();  
    }
    return(TRUE);
}


boolean verify_zeroed (int bank)
{
    volatile ulong *flash_addr = (ulong *)(fg.fg_flashbase + fg.fg_banksize * bank);
    ulong data;
    ulong i, j; 
    ulong wmask;
    
    for (i=0; i<fg.fg_chipsize; i++, flash_addr++ ) {
	if(*flash_addr) {         /* cheap first shot */
	    for ( j = 0; j < 10; j++ ) { /* 10 read retries */
		if ( (data = *flash_addr) == 0 ) {
		    break;
		}
	    }
	    if (data != 0 ) {
		printf("\nflash: zero verify failed in bank %d at 0x%x\n",
		       bank, flash_addr);
		wmask = got_exp(data, 0, 0xffffffff);
		prt_gotexp(data, 0, wmask, (ulong *)flash_addr);
		return(FALSE);
	    }
	}
	
	/* check suspend every 0x1000 verifies, approx 4 ms or so */
	if ((i & 0xfff) == 0xfff) 
	    process_may_suspend();  
	
    }
    return(TRUE);
}
/***************************************************************************
 *
 * error management/reporting for flash programming
 * compare and mask bytes to help with re-writes
 * and with reporting by chip.
 *
 ***************************************************************************/

void prt_gotexp (ulong got, ulong exp, ulong mask, ulong *addr)
{
    ulong res = (got ^ exp) & mask;
    uchar *rs = (uchar *)&res;
    uchar *gt = (uchar *)&got;
    uchar *ex = (uchar *)&exp;
    uint bank = ((ulong)addr - fg.fg_flashbase) / fg.fg_banksize;
    int i, chip;	     

    printf("\n");
    chip = bank * FLASH_WIDTH;
    for (i = 0; i < FLASH_WIDTH; i++, chip++, gt++, ex++) {
	if ( *rs++ != 0 )
	    printf("socket %s: got 0x%2x expected 0x%2x write tries %d\n",
		   flash_socket(chip), 
		   *gt, *ex,
		   wp[chip]);
    }
}

ulong got_exp (ulong got, ulong exp, ulong mask)
{
    ulong res = (got ^ exp) & mask;
    ulong cmask = 0;

    if (res & 0xff) cmask |= 0xff;
    if (res & 0xff00) cmask |= 0xff00;
    if (res & 0xff0000) cmask |= 0xff0000;
    if (res & 0xff000000) cmask |= 0xff000000;
    return (cmask);
}

void inc_ep (ulong mask, int bank)
{
    uchar *mp = (uchar *)&mask;
    int i, chip;
    
    if (mask == 0) return;
    chip = bank * FLASH_WIDTH;
    for (i = 0; i < FLASH_WIDTH; i++, chip++)
        if (*mp++) ep[chip]++;
}

void inc_wp (ulong mask, ulong *addr)
{
    uint bank = ((ulong)addr - fg.fg_flashbase) / fg.fg_banksize;
    uchar *mp = (uchar *)&mask;
    int i, chip;
    
    if (mask == 0) return;
    chip = bank * FLASH_WIDTH;
    for (i = 0; i < FLASH_WIDTH; i++, chip++)
        if (*mp++) wp[chip]++;
}


void prt_errors (int bank)
{
    int j, chip;

    errmsg(&msgsym(ERASEFAIL, FLASH), bank);
    printf("Flash erase: ");
    printf(flash_erasefail_msg, bank);
    printf("\n             retry counts\n     socket   retries\n");
    chip = bank * FLASH_WIDTH;
    for ( j = 0; j < FLASH_WIDTH; j++, chip++)
	printf("     %6s   %4d\n",flash_socket(chip), ep[chip]);
}

boolean prom_err (void)
{
    printf("\nFlash: flash not programmable\n");
    return(FALSE);
}

/****************************************** 
 * flash_delay:
 *   called to wait out the 10 millisecs needed
 *   for 010/020 chips, also during 008 erase.
 *   not used for vpp - use spin waits.
 * call scheduler out of kindness to others.
 ******************************************/

void flash_delay (ulong delta) {  /* delay, which permits scheduling */
    process_sleep_for(delta);
}

/*
 *  Display list of flash chips known (as defined in
 *  the flash_info_table)
 */

void flash_print_chips_known (void)
{
    uint i;

    printf("Flash chips recognized by system :\n");
    printf("   Code   Chip-Sz     Cmd-grp   Chip-name\n"); 
    for (i = 0; i < MAX_ROM_MFGS; i++) 
	if (flash_size(i)) { 
	    printf("   %4s   0x%7x      %1d      %s\n",
		   flash_manu_code(i),
		   flash_size(i),
		   flash_cmnd_set(i),
		   flash_name(i));
	}
}

/* 
 * fl_cmd: for individual chip commands and data
 */
static inline ulong fl_cmd(ulong a, ulong b, ulong m) {
    return( (a&m) | (b&(~m)));
}

/***************************************************************************
 *
 * erase and programming algorithms for
 * AMD/INTEL 008 chips
 *
 ***************************************************************************/

boolean prom_quad_008 (ulong data, volatile ulong *ptr)
{
    int j;

    *ptr = program_mem_008;
    *ptr = data;
    *ptr = read_status_008;
    WASTETIME(fg.fg_usec10);	

    for ( j = 0; j < 10000; j ++ ) {
	if ( (*ptr & FLASH_READY) == FLASH_READY )
	    break;
    }
    *ptr = clear_status_008;         /* clear status regsiter */
    *ptr = read_mem_008;             /* set read mode */
    return (data == *ptr);           /* read value at location */
}

boolean prom_008 (ulong data, volatile ulong *ptr, ulong mask, int write_tries)
{
    int j;
    ulong readback = 0;
    ulong wmask = mask;
    ulong smask;
    
    /*  Set write command for chips into which write is 
     *  to be done.  Set read command for the other
     *  chips in the bank.
     */
    *ptr = fl_cmd(program_mem_008, read_mem_008, wmask);
    *ptr = fl_cmd(data, 0xffffffff, wmask);
    smask = FLASH_READY & mask;
    *ptr = read_status_008;
    j = 0;
    do { 
	WASTETIME(fg.fg_usec10);	
	j++; 
	readback = *ptr & mask;
    }
    while (((readback & smask) != smask) && (j < 10000));
    
    if (readback & FLASH_VPP_LOW) {	           /* Vpp low detected */
	printf("Flash: ** Vpp not enabled !! **\n");
	*ptr = clear_status_008;              /* clear status regsiter */
	*ptr = read_mem_008;                          /* set read mode */
	readback = *ptr;                     /* read value at location */
    }
    else  if (readback & FLASH_WRITE_ERR) {  /* write error in chip(s) */
	*ptr = clear_status_008;              /* clear status regsiter */
	*ptr = read_mem_008;                          /* set read mode */
	readback = *ptr;                     /* read value at location */
	wmask = got_exp(readback, data, mask);
	inc_wp(wmask, (ulong *)ptr);
    }
    else  {
	*ptr = read_mem_008;                          /* set read mode */
	return (TRUE);
    }
    printf("\nFlash write: ");
    printf(flash_writefail_msg, ptr, data, readback);
    errmsg(&msgsym(WRITEFAIL, FLASH), ptr, data, readback);
    prt_gotexp(readback, data, mask, (ulong *)ptr);
    return (FALSE);
}

boolean flash_erase_008 (flash_dev_t *dev, int bank)
{
    uint    sector; 
    
    boolean status = TRUE; /* lets be optimistic */
    
    /* we should check for device open here */
    
    if (!flash_assert_vpp()) {
	printf("\nFlash erase: VPP not enabled.\n");
	return(FALSE);
    }
    
    /*
     *  This case does not require any bank preconditioning
     *  or verify. They are done by the chip.
     */
    for (sector = 0; sector < fg.fg_sectors; sector++)  {
	if (!verify_flash_sector_008(bank, sector, 0xffffffffL, FALSE))
	    if (!(status = flash_erase_sector_008(0, bank, sector)))
		break;   /* erase failure */
        printc('e');
        flush();
    } /* end of for-loop */
    flash_deassert_vpp();
    printc('\n');
    return (status);
}

boolean verify_erased_008 (int bank)
{
    printf("Verify if flash bank %d erased ... ",bank);
    return (verify_flash_bank_008(bank, 0xffffffffL));
}

boolean verify_flash_bank_008 (int bank, ulong value)
{
    uint   sector;
    boolean status = TRUE;
    
    for (sector = 0; sector < fg.fg_sectors; sector++)
        if (!(status = verify_flash_sector_008(bank, sector, value, TRUE)))
            return (FALSE);                         /* verify failure */
    printf("\n");
    return (TRUE);
}

boolean verify_flash_sector_008 (int bank, int sector, 
				 ulong value, boolean verb)
{
    volatile ulong *flash_addr 
	= (volatile ulong *)(fg.fg_flashbase + (fg.fg_banksize * bank));
    ulong data;
    ulong i, j;
    ulong wmask = 0;

    flash_addr += (sector * fg.fg_erase_sector);
    *flash_addr = read_mem_008; /* set flash in read mode */
    WASTETIME(fg.fg_usec6);
    
    for (i=0; i<fg.fg_erase_sector; i++, flash_addr++ ) {
        for ( j = 0; j < 10; j++ ) { /* 10 read retries */
            data = *flash_addr;
            if ( data == value ) 
	        break;
        }
	
        if ( data != value) {
	    BUG(BF_VERIFY)
                wmask = got_exp(data, value, 0xffffffff);
	    printf("\nFlash: verify failed in bank %d sector %d at 0x%x\n",
		   bank, sector, flash_addr);
	    BUGEND
		prt_gotexp(data, value, wmask, (ulong *)flash_addr);
	    return (FALSE);
	}
	
	/* check suspend every 0x1000 verifies, approx 4 ms or so */
	if ((i & 0xfff) == 0xfff) 
	    process_may_suspend();  
    }
    return (TRUE);
}


boolean flash_erase_bank_008 (flash_dev_t *dev, int bank)
{
    uint    sector; 
  
    boolean status = TRUE; /* lets be optimistic */

  /* we should check for device open here */
  
    if (!flash_assert_vpp()) {
        printf("\nFlash erase: VPP not enabled.\n");
        return (FALSE);
    }
    /*
     *  This case does not require any bank preconditioning
     *  or verify. They are done by the chip.
     */
    for (sector = 0; sector < fg.fg_sectors; sector++)  {
	if (!verify_flash_sector_008(bank, sector, 0xffffffffL, FALSE))
	    if (!(status = flash_erase_sector_008(0, bank, sector)))
		break; /* erase failure	*/
        printc('e');
        flush();
        flash_delay(10);
    } /* end of for-loop */
    flash_deassert_vpp();
    printc('\n');
    return (status);
}


boolean flash_erase_sector_008 (flash_dev_t *dev, int bank, int sector)
{
    volatile ulong *flash_addr;
    ulong data, mask;
    ulong smask;
    int i, j;
    
    /* 
     * Assert Vpp only if dev specified. Else assume Vpp has
     * already been asserted.
     */
    if (dev)                                    /* non-NULL device pointer */
        if (!flash_assert_vpp()) {
	    printf("\nFlash erase: VPP not enabled.\n");
	    return (FALSE);
	}
    
    /*  
     * Set up address to start of sector 
     */
    mask = 0xffffffffL;
    flash_addr = (volatile ulong *)(fg.fg_flashbase + (fg.fg_banksize * bank));
    flash_addr += (sector * fg.fg_erase_sector);	
    *flash_addr = clear_status_008;	   /* clear status for safety sake */
    
    for (i = 0; i < MAX_ERASE_TRIES; i++)  {      /* retry loop for errors */
	/* give erase commands */
        *flash_addr = fl_cmd(erase_mem_008, read_mem_008, mask);  
        *flash_addr = fl_cmd(erase_cnf_008, read_mem_008, mask); 
        *flash_addr = read_status_008;                  /* set read status */
	
	/*  
	 * Loop while flash busy with erase
	 */
        smask = FLASH_READY & mask;
	for (j = 0; j < 1000; j++) {
            flash_delay(2);            
            data = *flash_addr & mask;
	    if ((data & smask) == smask)
		break;
	}

        if (data & FLASH_VPP_LOW) {                    /* Vpp low detected */
            printf("\nFlash erase_sector: ** Vpp not enabled !! **\n");
            *flash_addr = clear_status_008;	  /* clear status regsiter */
            *flash_addr = read_mem_008;       /* revert flash to read mode */
            if (dev)
                flash_deassert_vpp();  /* deassert Vpp if dev specified */
            return (FALSE);
        } else {
            if (data & FLASH_ERASE_ERR)	              /* flash erase error */
                *flash_addr = clear_status_008;         /* clear the error */
            else
                break;                                  /* erase a success */
	}
	
	/*
	 *  Prepare for retries. Check each chip for error. 
	 *  If error reported, set erase commands, else,
	 *  set read command for that chip
	 */

        mask = got_exp(data, FLASH_ERASE_ERR, mask);
        inc_ep(mask, bank);
    }                                             /* end of retry for-loop */
    
    *flash_addr = read_mem_008;               /* revert flash to read mode */
    if (dev)
        flash_deassert_vpp();          /* deassert Vpp if dev specified */
    
    /*  
     * Check if erase was a success or a failure
     * after the retries (if any)
     */
    if (i == MAX_ERASE_TRIES)  {                    /* erase was a FAILURE */
        printc('\n');
        prt_errors(bank); 
        return (FALSE);
    }
    return (TRUE);                                  /* erase was a SUCCESS */
}
/***************************************************************************
 *
 * end of INTEL 008 specific code
 *
 *
 ***************************************************************************/


/***************************************************************************
 *
 * erase and programming algorithms for
 * AMD/INTEL 010, 020 chips
 *
 ***************************************************************************/
boolean prom_quad_010 (ulong data, volatile ulong *ptr)
{
    *ptr = program_mem_010;
    *ptr = data;
    WASTETIME(fg.fg_usec10); 
    *ptr = program_verify_010;
    WASTETIME(fg.fg_usec6);
    *ptr = reset_cmd_010;
    if (data == *ptr) {
	return(TRUE);
    }
    return(FALSE);
}

boolean prom_010 (ulong data, volatile ulong *ptr, ulong mask, int write_tries)
{
    int writepulse;
    ulong readback = 0;
    ulong wmask = mask;
    
    for( writepulse = 0; writepulse < write_tries; writepulse++) {
        /* Give write command followed by data */
	*ptr = fl_cmd(program_mem_010, reset_cmd_010, wmask);
	*ptr = fl_cmd(data, 0xffffffff, wmask);
	WASTETIME(fg.fg_usec10 + fg.fg_usec6); 
        /* Now give verify command */
	*ptr = fl_cmd(program_verify_010, reset_cmd_010, wmask);
	WASTETIME(fg.fg_usec10);

        /* Now read data written to flash */
	readback = *ptr;
        /* Match data read back versus data written */
	wmask = got_exp(readback, data, wmask);
         /* Reset chips */
	*ptr = reset_cmd_010;
	if (wmask == 0) {  /* Data read matches data written */
	    return(TRUE);
	} 

        /* Be persistent. Give one more try */
	readback = *ptr;
	wmask = got_exp(readback, data, wmask);
	if (wmask == 0) {
	    return(TRUE);
	} 

        /* Nogo. Write failed. Increment write retries */
	inc_wp(wmask, (ulong *)ptr);
    }
    if (wmask == 0) return(TRUE);
    printf("\nFlash write: ");
    printf(flash_writefail_msg, ptr, data, readback);
    errmsg(&msgsym(WRITEFAIL, FLASH), ptr, data, readback);
    prt_gotexp(readback, data, mask, (ulong *)ptr);
    return (FALSE);
}

boolean flash_erase_bank_010 (flash_dev_t *dev, int bank)
{
    volatile ulong *flash_addr;
    volatile ulong *flash_base 
	= (ulong *)(fg.fg_flashbase + fg.fg_banksize * bank);  
    int    i, erasepulse; 
    ulong  data, erasecmd, emask, delay_cnt;
    DCACHESTATUS;
    
    /* we should check for device open here */
    
    if (!flash_assert_vpp()) {
	printf("\nFlash erase: VPP not enabled.\n");
	return(FALSE);
    }
    DCACHEOFF; /* Disable cache */
    
    /* Program all locations to zero before erase */
    if (!zero_bank(bank) || !verify_zeroed(bank))
	{
	    printf("flash: failed to zero bank %d.\nErase aborted.\n", 
		   bank);
	    flash_deassert_vpp();
	    DCACHEON;
	    return(FALSE);
	}
    
    /* perform initial erase of entire bank */
    flash_base = flash_addr 
	= (volatile ulong *)(fg.fg_flashbase + fg.fg_banksize * bank);  
    *flash_base = erase_cmd_010;
    *flash_base = erase_cmd_010;
    flash_delay(10);
    
    /* now verify each address, re-erasing any byte! which fails to verify  */
    i = fg.fg_chipsize;           /* loop control */
    erasepulse = MAX_ERASE_TRIES;
    delay_cnt = 0;
    
    do { 
	*flash_addr = erase_verify_010;
	WASTETIME(fg.fg_usec6);
	data = *flash_addr;
	
	if(data == 0xffffffff)
	    {                     /* long word successfully erased */
		flash_addr++; 
		i--;
                if ((++delay_cnt % 0x3FF) == 0)  /* dismiss once in a while */
                    flash_delay(10);
	    } else { /* erase not successful at this location */
		
		/*
		 * Important: We must re-erase only the byte(s) that
		 * needs re-erasing.  If you re-erase a location
		 * that is already erased the part may be destroyed.
		 * This code sets up the erasecmd for the next erasepulse
		 * substituting the READ_MEMORY command for the bytes
		 * that do not need re-erasing.
		 */
		
		emask = got_exp(data, 0xffffffff, 0xffffffff);
		inc_ep(emask, bank);
		erasecmd = fl_cmd(erase_cmd_010, reset_cmd_010, emask);
		*flash_addr = erasecmd;
		*flash_addr = erasecmd;
		flash_delay(10);
		erasepulse--;
	    }
	pz('e',(ulong *)flash_addr);
    }  while ( (i > 0) & (erasepulse > 0) );
    
    /* done, but was it successful? */
    
    *flash_base = reset_cmd_010;
    *flash_base = reset_cmd_010;
    flash_deassert_vpp();
    DCACHEON;
    
    if (erasepulse <= 0 ) {	    /* erase failed */
	prt_errors(bank);
	return(FALSE);
    } else {
	return(TRUE);
    }
}

/***************************************************************************
 *
 * end of INTEL/AMD 010, 020 specific code
 *
 ***************************************************************************/



/***************************************************************************
 *
 *                     flash chip code, size, name functions
 *
 * flash_socket
 * flash_identify
 * flash_manu_code
 * flash_cmnd_set
 * flash_name
 * flash_manu
 * flash_size
 * eh?
 *
 ***************************************************************************/

/*
 * flash_identify
 * Given a manufacturer/device code,
 *   return the index into the flash device table.
 * Return 0 for unknown devices.
 * All other calls take the index as the parameter.
 * NOTE:  This probably shouldn't look at both dev_code and rev_dev_code
 * long term.  Some qualification should be done so we don't need to look
 * at both values for high and low-end systems.
 */
int flash_identify (ushort code)
{
    uint i;

    for (i = 0; i < MAX_ROM_MFGS; i++)
	if ( (flash_info_tbl[i].dev_code == code) ||
	    (flash_info_tbl[i].rev_dev_code == code) )
	    return (i);
    return (0);
}

/*
 * flash_manu_code
 * Given the index, return its manu_code, as seen on the chip.
 */

char *flash_manu_code (uint i)
{
    return ((i < MAX_ROM_MFGS) ? (flash_info_tbl[i].manu_dev_code) : 0);
}

/* 
 * flash_cmnd_set
 * return command set index
 */

int flash_cmnd_set (uint i)
{
    return ((i < MAX_ROM_MFGS) ? (flash_info_tbl[i].cmnd_set) : 0);
}

/*
 * flash_name
 * Given a manufacturer/device code, return the chip name.
 * Return NULL for unknown devices.
 */

char *flash_name (uint i)
{
    return ((i < MAX_ROM_MFGS) ? (flash_info_tbl[i].chip_name) : 0);
}

/*
 * flash_manu
 * Given a manufacturer/device code, return the manuf. name.
 * Return NULL for unknown devices.
 */

ulong flash_size (uint i)
{
    return ((i < MAX_ROM_MFGS) ? (flash_info_tbl[i].device_size) : 0);
}

/*
 *  Return no of erase sectors per chip
 */
int flash_sectors (void)
{
    return ((fg.fg_size_cnfrm == 0) ? 0 : fg.fg_sectors);
}

/*
 *  Return no of flash banks
 */
int flash_banks (void)
{
    return (fg.fg_banks);
}
ulong flash_sector_size (uint i)
{
    return ((i < MAX_ROM_MFGS) ? (flash_info_tbl[i].sector_size) : 1);
}

uchar *flash_address (uchar *addr)
{
    return(fg.fg_flashbase + addr);
}

