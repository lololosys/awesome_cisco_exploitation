/* $Id: flash_rp1.c,v 3.5.42.3 1996/08/07 08:58:43 snyder Exp $
 * $Source: /release/112/cvs/Xsys/hes/flash_rp1.c,v $
 *------------------------------------------------------------------
 * flash_rp1.c - clean specific flash driver functions
 *
 * February 1993, Dit Morse
 *       - extracted from flash_rp1drv.c
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_rp1.c,v $
 * Revision 3.5.42.3  1996/08/07  08:58:43  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.42.2  1996/08/02  22:53:28  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.5.42.1  1996/03/18  19:40:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.2  1996/03/09  05:05:38  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.12.1  1996/01/24  21:45:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/01/06  03:22:20  hampton
 * Remove extraneous includes of if_timer.h.  Rename if_timer.h and move
 * it to the 'os' directory.  [CSCdi46482]
 *
 * Revision 3.4  1995/12/21  16:56:31  hampton
 * Remove old timer related declarations or obsolete conditionally
 * compiled code.  [CSCdi45965]
 *
 * Revision 3.3  1995/12/01  14:45:30  doner
 * CSCdi44708:  CIP loader requires imbedded flash odd boundary read
 *              Support added to handle read from non-word boundary
 *              withing the flash.
 *
 * Revision 3.2  1995/11/17  09:16:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:39:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:39:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *------------------------------------------------------------------
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "../os/timer_generic.h"
#include "../../boot/src-68-c7000/pcmap.h"
#include "../dev/g_dev.h"
#include "flash_rp1.h"
#include "../dev/flash_dvr_spec.h"

static int rp1_flash_init(void);
static int rp1_flash_open(int flags);
static int rp1_flash_close(flash_dev_t *dev);
static int rp1_flash_ioctl(flash_dev_t *dev, int cmd, uchar *buf);
static ulong rp1_flash_read(flash_dev_t *dev, uchar *buf, uchar *addr, uint len);
static ulong rp1_flash_write(flash_dev_t *dev, uchar *buf, uchar *addr, ulong len);
static ulong flash_query_size(flash_dev_t *dev);

static devfun_t rp1_flash_fun = {
    rp1_flash_init,    /* initialization function address */
    rp1_flash_open,    /* open function address */
    rp1_flash_close,   /* close function address */
    NULL,              /* no interrupt routine */
    rp1_flash_ioctl,   /* ioctl function address */
    rp1_flash_read,    /* read function address */
    rp1_flash_write,   /* write function address */
};

static flash_dev_t  rp1_flash_dev = 
{"rp1-flash",     0,   0,    0,     0,   -1, &rp1_flash_fun};

/*  Refer to ../../boot/cpu.h
 *  The CPU types are defined there.
 *  CPU_UNKNOWN is the last value 
 */
static char *const flash_sockets [CPU_UNKNOWN] [MAX_ROMS] =  {
  { "   ", "   ", "   ", "   ",   /* bank 0 */    /* CPU_SMI8  */
    "   ", "   ", "   ", "   ",   /* bank 1 */
    "   ", "   ", "   ", "   ",   /* bank 2 */
    "   ", "   ", "   ", "   " }, /* bank 3 */
  
  { "   ", "   ", "   ", "   ",   /* bank 0 */    /* CPU_CADLINC  */
    "   ", "   ", "   ", "   ",   /* bank 1 */
    "   ", "   ", "   ", "   ",   /* bank 2 */
    "   ", "   ", "   ", "   " }, /* bank 3 */

  { "   ", "   ", "   ", "   ",   /* bank 0 */    /* CPU_SMI10  */
    "   ", "   ", "   ", "   ",   /* bank 1 */
    "   ", "   ", "   ", "   ",   /* bank 2 */
    "   ", "   ", "   ", "   " }, /* bank 3 */
  
  { "   ", "   ", "   ", "   ",   /* bank 0 */    /* CPU_FTI10  */
    "   ", "   ", "   ", "   ",   /* bank 1 */
    "   ", "   ", "   ", "   ",   /* bank 2 */
    "   ", "   ", "   ", "   " }, /* bank 3 */

  { "U42","U44","U46","U48",      /* bank 0 */    /* CPU_CSC1   */
    "U41","U43","U45","U47",      /* bank 1 */
    "U142","U144","U146","U148",  /* bank 3 */
    "U141","U143","U145","U147" },/* bank 4 */

  { "U42","U44","U46","U48",      /* bank 0 */    /* CPU_UNIX   */
    "U41","U43","U45","U47",      /* bank 1 */
    "U142","U144","U146","U148",  /* bank 3 */
    "U141","U143","U145","U147" },/* bank 4 */

  { "U42","U44","U46","U48",      /* bank 0 */    /* CPU_CSC2   */
    "U41","U43","U45","U47",      /* bank 1 */
    "U142","U144","U146","U148",  /* bank 3 */
    "U141","U143","U145","U147" },/* bank 4 */

  { "U42","U44","U46","U48",      /* bank 0 */    /* CPU_MPU12   */
    "U41","U43","U45","U47",      /* bank 1 */
    "U142","U144","U146","U148",  /* bank 3 */
    "U141","U143","U145","U147" },/* bank 4 */

  { "U42","U44","U46","U48",      /* bank 0 */    /* CPU_CSC3   */
    "U41","U43","U45","U47",      /* bank 1 */
    "U142","U144","U146","U148",  /* bank 3 */
    "U141","U143","U145","U147" },/* bank 4 */

  { "U42","U44","U46","U48",      /* bank 0 */    /* CPU_STS1   */
    "U41","U43","U45","U47",      /* bank 1 */
    "U142","U144","U146","U148",  /* bank 3 */
    "U141","U143","U145","U147" },/* bank 4 */

  { "U42","U44","U46","U48",      /* bank 0 */    /* CPU_PAN   */
    "U41","U43","U45","U47",      /* bank 1 */
    "U142","U144","U146","U148",  /* bank 3 */
    "U141","U143","U145","U147" },/* bank 4 */

  { "U42","U44","U46","U48",      /* bank 0 */    /* CPU_MERLOT   */
    "U41","U43","U45","U47",      /* bank 1 */
    "U142","U144","U146","U148",  /* bank 3 */
    "U141","U143","U145","U147" },/* bank 4 */

  { "U42","U44","U46","U48",      /* bank 0 */    /* CPU_LEMONADE   */
    "U41","U43","U45","U47",      /* bank 1 */
    "U142","U144","U146","U148",  /* bank 3 */
    "U141","U143","U145","U147" },/* bank 4 */

  { "U42","U44","U46","U48",      /* bank 0 */    /* CPU_CSC4   */
    "U41","U43","U45","U47",      /* bank 1 */
    "U142","U144","U146","U148",  /* bank 3 */
    "U141","U143","U145","U147" },/* bank 4 */

  { "U42","U44","U46","U48",      /* bank 0 */    /* CPU_XX   */
    "U41","U43","U45","U47",      /* bank 1 */
    "U142","U144","U146","U148",  /* bank 3 */
    "U141","U143","U145","U147" },/* bank 4 */

  { "E9","E20","E13","E25",       /* bank 0 */    /* CPU_BRUT   */
    "   ","   ","   ","   ",      /* bank 1 */
    "   ","   ","   ","   ",      /* bank 2 */
    "   ","   ","   ","   " },    /* bank 3 */ 

  { "U63","U62","U61","U60",	  /* bank 0 */    /* CPU_RP1 */
    "U48","U47","U46","U45",      /* bank 1 */
    "U30","U29","U28","U27",      /* bank 2 */
    "U17","U16","U15","U14" },    /* bank 3 */

  { "   ", "   ", "   ", "   ",   /* bank 0 */    /* CPU_BASS  */
    "   ", "   ", "   ", "   ",   /* bank 1 */
    "   ", "   ", "   ", "   ",   /* bank 2 */
    "   ", "   ", "   ", "   " }, /* bank 3 */

  { "   ", "   ", "   ", "   ",   /* bank 0 */    /* CPU_CRISTAL  */
    "   ", "   ", "   ", "   ",   /* bank 1 */
    "   ", "   ", "   ", "   ",   /* bank 2 */
    "   ", "   ", "   ", "   " }, /* bank 3 */

  { "   ", "   ", "   ", "   ",   /* bank 0 */    /* CPU_CANCUN  */
    "   ", "   ", "   ", "   ",   /* bank 1 */
    "   ", "   ", "   ", "   ",   /* bank 2 */
    "   ", "   ", "   ", "   " }, /* bank 3 */

  { "   ", "   ", "   ", "   ",   /* bank 0 */    /* CPU_SIERRA  */
    "   ", "   ", "   ", "   ",   /* bank 1 */
    "   ", "   ", "   ", "   ",   /* bank 2 */
    "   ", "   ", "   ", "   " }, /* bank 3 */

  { "   ", "   ", "   ", "   ",   /* bank 0 */    /* CPU_RSP  */
    "   ", "   ", "   ", "   ",   /* bank 1 */
    "   ", "   ", "   ", "   ",   /* bank 2 */
    "   ", "   ", "   ", "   " }, /* bank 3 */

  { "   ", "   ", "   ", "   ",   /* bank 0 */    /* CPU_SAPPHIRE  */
    "   ", "   ", "   ", "   ",   /* bank 1 */
    "   ", "   ", "   ", "   ",   /* bank 2 */
    "   ", "   ", "   ", "   " }, /* bank 3 */

  { "   ", "   ", "   ", "   ",   /* bank 0 */    /* CPU_SYNALC  */
    "   ", "   ", "   ", "   ",   /* bank 1 */
    "   ", "   ", "   ", "   ",   /* bank 2 */
    "   ", "   ", "   ", "   " }, /* bank 3 */
};


/**************************************************************************
 *
 *               FLASH dev DRIVER ROUTINES
 *
 * flash_dev_init
 * rp1_flash_open
 * rp1_flash_close
 * rp1_flash_ioctl
 *
 *************************************************************************/
flash_dev_t *flash_dev_init (void)
{
    flash_dev_t *dev = &rp1_flash_dev;

    if (DEV_OK == dev_init(dev))
	return(dev);
    else
	return(NULL);
}

static int rp1_flash_init (void)
{
    flash_dev_t *dev = &rp1_flash_dev;
    flash_header tmp_flash_hdr;
    
    fg.fg_size = 0;
    fg.fg_flashbase    = FLASH_BASE;
    /* calibrate timer */
    flash_timer(&fg.fg_usec6, &fg.fg_usec10, FALSE);

    if (!(fg.fg_size = flash_query_size(dev))) {
        if(rp1_flash_read(dev, (uchar *)&tmp_flash_hdr, (uchar *)0,
                      sizeof(flash_header)) != sizeof(flash_header))
            return(DEVERR_INIT);    /* no flash accessible or configured */
        switch(tmp_flash_hdr.fh_magic) {
          case FLASH_HDR_MAGIC:
            fg.fg_size = tmp_flash_hdr.fhx_size;
            fg.fg_status = FR_READ_ONLY;  /* No Vpp but valid data */
            break;
          default:
            return(DEVERR_INIT);    /* no flash accessible or configured */
            break;
        }
    }
    fg.fg_size_cnfrm = fg.fg_size; /* size confirmed by probe sizing */

    return(DEV_OK);           /* returns true if addressable */
}

static int rp1_flash_open (int flags)
{
    return(DEV_OK);
}

static int rp1_flash_close (flash_dev_t *dev)
{   
    flash_deassert_vpp();        /* for safety's sake */
    return(DEV_OK);
}

static int rp1_flash_ioctl (flash_dev_t *dev, int cmd, uchar *buf)
{
    switch (cmd) {
	
      default:
	return(DEVERR_INVALID_OP);
    }
}

/*
 * long_flash_copy
 * On the RP, only longword flash accesses are allowed.  For this reason, 
 * bcopy() cannot be used to read a file from flash unless we know the file 
 * size is a multiple of four.  Because we never know this for sure, this 
 * routine takes the place of bcopy and appropriately handles the stray bytes.
 */
static void long_flash_copy (uchar *flash_src, uchar *buf, ulong len)
{
  volatile ulong *long_src;
  ulong *long_buf, *long_buf_tmp;
  ulong leftover, num_longs, temp_long_area;
  uint i, j, shift;

  if (len <= 0)
    return;

  long_src = (volatile ulong *) flash_src;

  /* Number of bytes before FLASH_WIDTH boundary */
  leftover = FLASH_WIDTH - ((uint)long_src % FLASH_WIDTH);
  if (leftover < FLASH_WIDTH) {           /* Yes we pick them up separately */
      /*Back up to proper boundary*/
      long_src = (ulong *)(((uint)long_src/FLASH_WIDTH) * FLASH_WIDTH);
      temp_long_area = *long_src;         /* Pick up longword */
    for (j=leftover, shift=leftover-1; j > 0; j--, shift--) {
      *buf++ = ((temp_long_area >> (shift*8)) & 0xff);
    }
    long_src++;
    len -= leftover;
  }

  long_buf = (ulong *) buf;
  num_longs = len / FLASH_WIDTH;
  leftover = len % FLASH_WIDTH;

  if ( ((uint)long_buf % FLASH_WIDTH) == 0) {  
    /*Destination is on a proper boundary*/
    for(i=0; i < num_longs; i++) {
      *long_buf++ = *long_src++;
    }
  }
  else {
      /*
       * move differently (machine dependent), since destination is not on a
       * proper boundary
       */
      for(i=0; i < num_longs; i++, long_buf++) {
	  temp_long_area = *long_src++;     /* Pick up longword from flash */
	  PUTLONG(long_buf,temp_long_area); /* Move longword               */
      }
  }
  buf = (uchar *) long_buf;
  long_buf_tmp = long_buf;

  if (leftover) {
    *long_buf_tmp = *long_src;
    for (j=leftover, shift=FLASH_WIDTH-1; j > 0; j--, shift--) {
       *buf++ = ((*long_buf_tmp >> (shift*8)) & 0xff);
    }
  }
}

static ulong rp1_flash_read (flash_dev_t *dev, uchar *buf, uchar *addr, uint len)
{ 
    (void) long_flash_copy(FLASH_BASE + addr, buf, len);
    return(len);
}

static ulong rp1_flash_write (flash_dev_t *dev, uchar *buf, uchar *addr, ulong len)
{
    return(flash_write(buf, addr, len, FALSE));
}

char *flash_socket (int i)
{
    return(flash_sockets[cpu_type][i]);
}

void flash_timer (ushort *v6, ushort *v10, boolean print_timer)
{
    *v6  = timer_calibrate(6, print_timer);
    *v10 = timer_calibrate(10, print_timer);
}

boolean flash_assert_vpp (void)
{
    *(ushort *)(ADRSPC_CONTROL)  |= CONTROL_FLASH_VPP;
    DELAY(100);
    return(*(ushort *)(ADRSPC_CONTROL) & CONTROL_FLASH_VPP ? 1 : 0 );
}

static boolean flash_check_vpp (void)
{
    return((*(ushort *)(ADRSPC_CONTROL) & CONTROL_FLASH_VPP) ? TRUE : FALSE );
}

boolean flash_deassert_vpp (void)
{
    if (flash_check_vpp()) {
	*(ushort *)(ADRSPC_CONTROL) &= ~CONTROL_FLASH_VPP;
        DELAY(100);
    }
    return(*(ushort *)(ADRSPC_CONTROL) & CONTROL_FLASH_VPP ? 1 : 0 );
}

/*
 * flash_query_size
 * On the RP, there is no way to size flash by doing a touch_device()
 * looking for buserror.  This is because we won't buserror in the flash
 * address space on the RP when no flash parts are installed.  For this
 * reason, we must size based on flash device codes read from the flash
 * devices themselves.  This is the purpose of flash_query_size().
 */
static ulong flash_query_size (flash_dev_t *dev)
{
    uchar manu_buf[MAX_ROMS];
    uchar device_buf[MAX_ROMS];
    int b;
    int id;

    if (!flash_assert_vpp())
        return(FALSE);

    /*
     *  Query first part in each bank for manuf and device codes.
     *  Assume that if first chip in bank is ok, then the remaining
     *  chips in the bank are also ok.  This is ok to do seeing
     *  flash_query_roms will be called later and query all the parts.
     */

    /* query first bank and configure */
    query_bank(0, (ulong *)&manu_buf[0], (ulong *)&device_buf[0]);
    id = flash_identify((manu_buf[0] << 8) | device_buf[0]);
    if (!id) {
	flash_deassert_vpp();
	return(FALSE);
    }

    /* configure the geometry */
    fg.fg_chipsize     = flash_size(id);
    fg.fg_banksize     = fg.fg_chipsize * fg.fg_width;
    fg.fg_chipcount    = fg.fg_width; /* we support minimum of 1 bank */ 
    fg.fg_banks        = 4;  /* Assumption based on current RP */

    /*  query the remaining banks */
    for (b = 1; b < fg.fg_banks; b++) {
        query_bank(b*fg.fg_banksize,
                   (ulong *)&manu_buf[b*fg.fg_width],
                   (ulong *)&device_buf[b*fg.fg_width]);
	id = flash_identify((manu_buf[b*fg.fg_width] << 8) |
			    device_buf[b*fg.fg_width]);
        if (id) {
            fg.fg_chipcount += fg.fg_width;
        } else {
            break;
	}
    }
    flash_deassert_vpp();
    return(fg.fg_chipcount * fg.fg_chipsize);
}

boolean sync_flash_header (flash_dev_t *dev)
{
    flash_header tmp_flash_hdr;

    flash_assert_vpp();

    tmp_flash_hdr.fh_magic = FLASH_HDR_MAGIC;
    tmp_flash_hdr.fhx_size = fg.fg_size;
    tmp_flash_hdr.fhx_banks = fg.fg_banks;
    tmp_flash_hdr.fhx_width = fg.fg_width;

    rp1_flash_write(dev, (uchar *)&tmp_flash_hdr, (uchar *)0, 
                    sizeof(flash_header));
    flash_deassert_vpp();
    return(TRUE);  /* Return TRUE;  header supported on this platform */
}

