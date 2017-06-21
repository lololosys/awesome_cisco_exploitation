/* $Id: flash_dvr_les.c,v 3.3.12.3 1996/08/07 08:58:00 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_dvr_les.c,v $
 *------------------------------------------------------------------
 * flash_dvr_les.c - Low end systems Flash driver.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: flash_dvr_les.c,v $
 * Revision 3.3.12.3  1996/08/07  08:58:00  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.12.2  1996/06/16  21:11:19  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.12.1  1996/03/18  19:12:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  06:34:20  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/03/07  08:43:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:45:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/20  07:04:11  abaindur
 * CSCdi43852:  Crash during bootup with bad Flash SIMM
 *
 * Revision 3.2  1995/11/17  09:00:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/10/25  21:40:09  sagarwal
 * CSCdi41798:  Flash mib problems on the 4500
 * o Remove check for device subunit to prevent passing of corrupt data
 * o Init chipinfo array from beginning
 * o Make SNMP chip info static (and reupdate on partitioning or insertion
 *   of PCMCIA card) to make getnext retrievals faster
 *
 * Revision 2.7  1995/09/24  19:15:36  vnguyen
 * CSCdi37804:  flash simms based on different chips can not be mixed
 * Changes made to support the mix of simms made from different chips.
 *
 * Revision 2.6  1995/08/08  16:45:59  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.5  1995/07/29 00:27:29  enf
 * CSCdi37707:  CPUHOG message while booting from flash
 * make flash read more efficient and suspend reads every 512K bytes
 *
 * Revision 2.4  1995/06/28  09:23:17  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/26  20:29:36  henniger
 * CSCdi36403:  Flash system (re)init takes too long (CPUHOG)
 * Flash system re/init takes too long
 * cache write protect status
 *
 * Revision 2.2  1995/06/09  13:00:02  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:24:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************
 *  NOTE : All functions and data structures in this module are local.
 *         Do not add any globals.
 ********************************************************************/

#if defined(XX) || defined(PAN) || defined(BRASIL)
    #define DATA_BUS_REVERSED 	1
#else
    #undef DATA_BUS_REVERSED
#endif

#include "../dev/flash_codes.h"


#include "master.h"
#include <ciscolib.h>
#include "logger.h"
#include "../dev/flash_defines.h"
#include "../dev/dev_io.h"
#include "../dev/flash_dvr_les.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "subsys.h"
#include "../src-36/quicc.h"
#include "../src-36/quicc_lib.h"


#define UNKNOWN_CHIP_STR "Unknown Chip"
#define INIT_FAILED_MSG  "\n%%Error: %s initialization failed"

static ushort usec6 = 0, usec10 = 0;
static char *const dev_states[] = {
     "No device",
     "Device not initialized",
     "Device not programmable",
     "Read ONLY",
     "Read/Write",
     "Removed",
};

const flash_info_t flash_info_tbl[] = {
  /* Chip-id      Rev. Chip-id    man-dev  Chip-sz    Erase-sect  Cmnd-set  
     Name  */
  {(ushort)0x0   , (ushort)0x0   , "0000", 0x000000L, 0x1L    , INVALID_SET, 
   0, UNKNOWN_CHIP_STR},
  {(ushort)0x89B4, (ushort)0x912D, "89B4", 0x020000L, 0x20000L, INTEL_010, 
   1, "INTEL 28F010"},
  {(ushort)0x89BD, (ushort)0x91BD, "89BD", 0x040000L, 0x40000L, INTEL_010, 
   1, "INTEL 28F020"},
  {(ushort)0x01A7, (ushort)0x80E5, "01A7", 0x020000L, 0x20000L, INTEL_010, 
   1, "AMD   28F010"},
  {(ushort)0x01A2, (ushort)0x8045, "01A2", 0x020000L, 0x20000L, INTEL_010, 
   1, "AMD   28F010"},
  {(ushort)0x012A, (ushort)0x8054, "012A", 0x040000L, 0x40000L, INTEL_010, 
   1, "AMD   28F020"},
  {(ushort)0x012A, (ushort)0x5480, "012A", 0x040000L, 0x40000L, INTEL_010,
   0, "AMD   28F020"},  /* ^#@! DEC's kludge - A2 inverted */
  {(ushort)0x1CD0, (ushort)0x380D, "1CD0", 0x040000L, 0x40000L, INTEL_010, 
   1, "M5M   28F101P"},
  {(ushort)0x89A2, (ushort)0x9145, "89A2", 0x100000L, 0x10000L, INTEL_008, 
   1, "INTEL 28F008SA"},
  {(ushort)0x89A0, (ushort)0x9105, "89A0", 0x200000L, 0x10000L, INTEL_008,
   1, "INTEL 28F016SA"},
  {(ushort)0x01A4, (ushort)0x8025, "01A4", 0x080000L, 0x10000L, AMD_040,
   1, "AMD   29F040"},
  {(ushort)0x01D5, (ushort)0x80AB, "01D5", 0x0100000L, 0x10000L, AMD_040,
   1, "AMD   29F080"},
  {(ushort)0x01AD, (ushort)0x80B5, "01AD", 0x0200000L, 0x10000L, AMD_040,
   1, "AMD   29F016"},
  /* This must be the last entry in the table. Used to detect end of table. */
  {(ushort)0xFFFF  , (ushort)0xFFFF, "0000", 0x000000L, 0x1L    , INVALID_SET, 
   0, UNKNOWN_CHIP_STR},
 };

/***************************************************************************
 *                     flash chip code, size, name functions
 ***************************************************************************/
/*
 * Given the index, return its manu_code, as seen on the chip.
 */
static char *flash_manu_code (int i)
{
    return ((flash_info_tbl[i].chip_code != FLASH_TABLE_END) ?
            (flash_info_tbl[i].manu_chip_code) : "    ");
}

/*
 * return command set for chips (the write-erase algorithm # to be used)
 * 0 is an invalid command set.
 */
static int flash_cmnd_set (int i)
{
    return ((flash_info_tbl[i].chip_code != FLASH_TABLE_END) ?
            (flash_info_tbl[i].cmnd_set) : INVALID_SET);
}

/*
 * Given a manufacturer/device code, return the chip name.
 * Return "Unknown Chip" for unknown devices.
 */
static char *flash_name (int i)
{
    return ((flash_info_tbl[i].chip_code != FLASH_TABLE_END) ?
            (flash_info_tbl[i].chip_name) : UNKNOWN_CHIP_STR);
}

/*
 * Given a manufacturer/device code, return the manuf. name.
 * Return NULL for unknown devices.
 */
ulong flash_chip_size (int i)
{
    return ((flash_info_tbl[i].chip_code != FLASH_TABLE_END) ?
            (flash_info_tbl[i].chip_size) : 0);
}

/*
 * Return the erase sector size of the flash chip.
 * Return a 1 and not 0 by default so as to not cause a divide-by-zero
 * error.
 */
static ulong flash_sector_size(int i)
{
    return ((flash_info_tbl[i].chip_code != FLASH_TABLE_END) ?
            (flash_info_tbl[i].sector_size) : 1);
}

/***************************************************************************
 * Error management/reporting for flash programming.
 * Compare and mask bytes to help with re-writes
 * and with reporting by chip.
 ***************************************************************************/
static inline ulong flash_got_exp (ulong got, ulong exp, ulong mask)
{
    ulong res = (got ^ exp) & mask;
    ulong cmask = 0;

    /*
     * Compare what was actually read with what was expected and
     * return a mask that indicates which bytes are wrong.
     */
    if (res & 0xff) cmask |= 0xff;
    if (res & 0xff00) cmask |= 0xff00;
    if (res & 0xff0000) cmask |= 0xff0000;
    if (res & 0xff000000) cmask |= 0xff000000;
    return (cmask);
}

/*
 * Increment erase or write retries for each chip.
 */
static void flash_inc_retries (flash_geom_t *fg, uchar *addr,
                                ulong mask, ulong *cntr)
{
    uchar *mp = (uchar *)&mask;
    int i;

    if (mask == 0) return;
    for (i = 0; i < fg->fg_chips; i++)
        if (*mp++) cntr[i]++;
}

static boolean flash_fast_prog_err (dev_info_t *dev, ulong data,
                                     volatile ulong *ptr)
{
    if (dev->dev_flags & DEV_FLAGS_VERBOSE)
        printf("\n%%Error: %s not programmable\n", dev->dev_name);
    return (FALSE);
}
static boolean flash_prog_err (dev_info_t *dev, ulong data,
                                volatile ulong *ptr, ulong mask)
{
    return (flash_fast_prog_err(dev, data, ptr));
}
static ulong flash_erase_err (dev_info_t *dev, uchar *addr, ulong len)
{
    if (dev->dev_flags & DEV_FLAGS_VERBOSE)
        printf("\n%%Error: %s not programmable\n", dev->dev_name);
    return (0);
}

/*
 *   Called to wait out the long delays, eg., during VPP ON and OFF
 *   and during flash erase on sector erasable parts.
 *   Done out of kindness to others.
 */
static inline void flash_delay (ulong delta)
{
    process_sleep_for(delta); /* delay which permits scheduling */
}

/*
 * prepares individual chip commands
 */
static inline ulong fl_cmd (ulong a, ulong b, ulong m)
{
    return ((a & m) | (b & (~m)));
}

/*
 * Determine if we are executing this image from the same file
 * system in which this file resides.
 */
static boolean run_from_flash (dev_info_t *dev)
{
    ulong addr;
    /*
     * If address of this function lies in the logical device space
     * (actually file system space if we consider one file system
     * per logical device), then return TRUE.
     */
    addr = (ulong)(void *)run_from_flash;
    return (((addr >= dev->dev_addr) && (addr < dev->dev_end)) ? TRUE : FALSE);
}

/* Warning: beesley requires these (read32,write32) be inline: */

static inline ulong write32v(volatile ulong *dest, ulong data)
{
    ushort *p;
    p = (ushort*)dest;
    *p++ = data & 0xffff;
    *p   = (data>>16) & 0xffff;
    return(data);
}

static inline ulong read32v(volatile ulong *src)
{
    ushort *p;
    ulong data;
    p = (ushort*)src;
    data = (*p++)|((*p)<<16);
    return(data);
}


/*********************************************************************
 *                   flash read, write and erase entry routines
 *
 * parameters:   dev;        flash handle
 *               char *buf;  buffer for the read-write data
 *               char *addr; relative start addr in flash for erase
 *               len;        read, write or erase length (bytes)
 * returns:      ulong;      number of bytes actually read, written
 *                           or erased
 *********************************************************************/
static ulong les_flash_erase (dev_info_t *dev, uchar *addr, ulong len)
{
    flash_geom_t *fg;
    ulong min_erase_size, erased_len, ret_len;

    fg = dev->dev_geom;
    if (fg == NULL)  /* No geom; bye bye */
        return (0);

    min_erase_size = fg->fg_sectorsize * fg->fg_chips;

    if (min_erase_size == 0) 
        return (0);
    if ((ulong)addr >= dev->dev_size)  /* invalid address */
        return (0);

    if ((ulong)addr % min_erase_size)  /* start addr not aligned to a sector */
        return (0);

    if (len % min_erase_size)  /* length not a multiple of a sector */
        return (0);

    if (len > dev->dev_size)  /* correct length if it exceeds device */
        len = dev->dev_size;
    if (((ulong)addr + len) > dev->dev_size)
        len = dev->dev_size - (ulong)addr;

    /*
     * Skip to the right bank based on start address.
     */
    while (fg && ((ulong)addr >= fg->fg_bank_end))
        fg = fg->next_geom;

    erased_len = 0;
    ret_len = 0;
    while (fg && (erased_len < len)) {
        ret_len += (*fg->fg_erase)(dev, addr, min_erase_size);
        erased_len += min_erase_size;
        addr += min_erase_size;

	/* if we've reached the bank boundary, switch to the erase algorithm
	 * that the next bank uses
	 */
	if ((ulong)addr >= fg->fg_bank_end)
	    fg = fg->next_geom;

	if (dev->dev_flags & DEV_FLAGS_VERBOSE) {
	    printc('e');
	    flush();
	}
        if (dev->devcons->dev_delay_fn)
            (*dev->devcons->dev_delay_fn)(1);  /* be kind to others */
    }
    return (ret_len);
}

static ulong les_flash_read (dev_info_t *dev, uchar *buf,
                             uchar *addr, ulong len)
    /*
     * buf is the address of the buffer to read into.
     * addr is the relative address from where to read.
     * len is the number of bytes to read.
     * returns number of bytes read.
     */
{
    uchar *q = addr + dev->dev_addr;
    ushort *src,*dest;
    ulong l = len;
    ulong mod_len;

    if (dev->dev_width == BW_32BITS) {
        bcopy(addr + dev->dev_addr, buf, len);
    } else {
        if ((ulong)q & 0x01)
        {
          *buf++ = *q++;
	  l--;
        }
	dest = (ushort*)buf;
	src  = (ushort*)q;
	mod_len = l % 2;

        for (l -= mod_len; l; l-=2)
        {
	    *dest++ = *src++;
	}

	if (mod_len)
	{
	    *((uchar*)dest) = *((uchar*)src);
	}
    }
    return(len);
}

static ulong les_flash_write (dev_info_t *dev, uchar *buf,
                              uchar *addr, ulong len)
    /*
     * buf is the address of the buffer to write from.
     * addr is the Flash relative address to write to.
     * len is the number of bytes to write.
     * returns number of bytes written.
     */
{
    flash_geom_t *fg;
    ulong  len_to_write = len;                   /* bytes to write*/
    ulong bank_end;

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

    addr  = addr + dev->dev_addr;  /* set to physical address */

    /* check limits here: 0 < addr:addr+len < max */

    if ((ulong)addr >= dev->dev_end) {
        if (dev->dev_flags & DEV_FLAGS_VERBOSE)
            printf("\n%%Error: initial write address (0x%x) beyond %s end (0x%x)\n",
                   addr, dev->dev_name, dev->dev_end - 1);
        return(0);
    }
    if ( ((ulong)addr + len) > dev->dev_end) {
        if (dev->dev_flags & DEV_FLAGS_VERBOSE)
            printf("\n%%Error: final write address (0x%x) beyond %s end (0x%x)\n",
                   addr + len, dev->dev_name, dev->dev_end - 1);
        return(0);
    }

    fg = dev->dev_geom;
    if (fg == NULL)  /* No geom; bye bye */
        return (0);
    bank_end = fg->fg_bank_end + dev->dev_addr;

    /* find out which bank we want to start with */
    while (fg && ((ulong)addr >= bank_end)) {
      fg = fg->next_geom;
      if (fg)
          bank_end = fg->fg_bank_end + dev->dev_addr;
    }
    if (fg == NULL)  /* No geom; bye bye */
        return (0);

    switch (fg->fg_width) {
    case BW_8BITS:
        while (fg && len) {
            if (!(*fg->fg_program)(dev, (ulong)*buf, (ulong *)addr, 0xff))
                return (len_to_write - len);
            addr++;
            buf++;
            len--;

            /* if we've reached the bank boundary, switch to the write
             * algorithm that the next bank uses
             */
            if ((ulong)addr >= bank_end) {
                fg = fg->next_geom;
                if (fg)
                    bank_end = fg->fg_bank_end + dev->dev_addr;
            }
        }
        return (len_to_write - len);

    case BW_16BITS: {
        ushort *sbuf;

        if ((ulong)addr & 1) {  /* write to odd address */
            addr = (uchar *)((ulong)addr & ~1);
            if (!(*fg->fg_program)(dev, (ulong)*buf, (ulong *)addr, 0xff))
                return (len_to_write - len);
            addr += 2;  /* since we aligned it to even boundary */
            buf++;
            len--;

	    if ((ulong)addr >= bank_end) { /* bank boundary crossover ? */
		fg = fg->next_geom;
		if (fg)
		    bank_end = fg->fg_bank_end + dev->dev_addr;
	    }
        }
        /* Writes now aligned to short word boundary. */
        sbuf = (ushort *)buf;
        while (fg && (len > 1)) {

            if (!(*fg->fg_program)(dev, (ulong)*sbuf, (ulong *)addr, 0xffff))
	        return (len_to_write - len);
	    sbuf++;
	    addr += 2;
	    len -= 2;

	    if ((ulong)addr >= bank_end) { /* bank boundary crossover ? */
		fg = fg->next_geom;
		if (fg)
		    bank_end = fg->fg_bank_end + dev->dev_addr;
	    }
        }
        buf = (uchar *)sbuf;
        if (fg && (len > 0)) {  /* odd byte left to write */

            if (!(*fg->fg_program)(dev, (ulong)(*buf<<8), (ulong *)addr, 0xff00))
                return (len_to_write - len);
            len--;
        }
        return (len_to_write - len);
    }

    case BW_32BITS: {
        ulong data, blen, *faddr, *baddr, wmask;
        /*
         * Check for and write unaligned head; ie., head does not start
         * on a long word boundary.
         * When writing unaligned head, we could also have the case
         * where the number of bytes to be written does not cross that
         * long word (eg., we are writing just one or two bytes).
         * Some specific examples are :
         *     Start address      Length
         *     xxxxxxx1           1
         *     xxxxxxx1           2
         *     xxxxxxx1           3
         *     xxxxxxx2           1
         *     xxxxxxx2           2
         *     xxxxxxx3           1
	 *
	 *             +-----------+-----------+
	 *             |  |  |  |  |  |  |  |  |
	 *             +-----------+-----------+
	 *
	 *     blen =  0  1  2  3  0  1  2  3  0
         */

        data = wmask = 0;
        faddr = (ulong *)addr;
        if ((blen = ((ulong)addr & 3)) != 0) {
            switch(blen) {
            case 1:  /* upto three bytes */
                data = *buf++;
		wmask |= 0xFF;
		if (--len == 0) {  /* just one byte to write */
		    data <<= 16;
		    wmask <<= 16;
		    break;
		}
            case 2: /* upto two bytes */
                data = (data << 8) | *buf++;
		wmask = (wmask << 8) | 0xFF;
		if (--len == 0) {  
		    data <<= 8;
		    wmask <<= 8;
		    break;
		}
            case 3:  /* one byte */
                data = (data << 8) | *buf++;
		wmask = (wmask << 8) | 0xFF;
		len--;
                break;
            }

            faddr = (ulong *)(((ulong)addr & ~3));
            if (!(*fg->fg_program)(dev, data, faddr, wmask))
                return (len_to_write - len);

            faddr++;
	    if ((ulong)faddr >= bank_end) { /* bank boundary crossover ? */
		fg = fg->next_geom;
		if (fg)
		    bank_end = fg->fg_bank_end + dev->dev_addr;
	    }
        }

        /*
         * write aligned block:
         * fg->fg_program_quad - is fast long-word aligned write.
         * If it fails, the whole cheesy enchilada is eaten.
         */
        baddr = (ulong *)buf;
        while (fg && (len > 3)) {
            if (!(*fg->fg_fast_program)(dev, *baddr, faddr)) {
                data = *faddr;                 /* quad write failed */
                wmask = flash_got_exp(data, *baddr, 0xffffffff);
                flash_inc_retries(fg, (uchar *)faddr, wmask,
				  &fg->fg_write_errors[0]);
                if (!(*fg->fg_program)(dev, *baddr, faddr, wmask))
                    return (len_to_write - len);
            }
            baddr++; faddr++;
            len -= 4;

	    if ((ulong)faddr >= bank_end) { /* bank boundary crossover ? */
		fg = fg->next_geom;
		if (fg)
		    bank_end = fg->fg_bank_end + dev->dev_addr;
	    }
        }

        /*
         * check for and write tail of less than FLASH_WIDTH bytes
         */
        if (fg && (len > 0)) {
            if (!(*fg->fg_program)(dev, *baddr, faddr, endmask[len]))
                return (len_to_write - len);
            len = 0;
        }
        return (len_to_write - len);    /* return bytes written */
    }  /* end 32 bit case */
    default:
        return (0);
    }  /* end switch */
}

/*-----------------------------------------------------------------------*/
/*
 * Precondition flash chips before erase. This involves zeroing the
 * contents. Required on older technology chips.
 */
static boolean flash_zero (dev_info_t *dev, uchar *addr, ulong len)
{
    flash_geom_t *fg;
    register ulong i;
    register uint  width;
    ulong wmask, data;

    fg = dev->dev_geom;
    if (fg==NULL)
        return (FALSE);
    width = fg->fg_width;
    for (i = 0; i < len; i += width, addr += width) {
        switch (width) {
        case BW_8BITS:
            if (!(*fg->fg_program)(dev, 0, (ulong *)addr, 0xff)) {
                data = *addr;
                flash_inc_retries(fg, addr, 0xff,
                                &fg->fg_write_errors[0]);
                return(FALSE);
            }
            break;
        case BW_16BITS:
            if (!(*fg->fg_program)(dev, 0, (ulong *)addr, 0xffff)) {
                data = *(ushort *)addr;
                wmask = flash_got_exp(data, 0, 0xffff);
                flash_inc_retries(fg, addr, wmask, 
				  &fg->fg_write_errors[0]);
                return(FALSE);
            }
            break;

        case BW_32BITS:
            if (!(*fg->fg_program)(dev, 0, (ulong *)addr, 0xffffffff)) {
                data = *(ulong *)addr;
                wmask = flash_got_exp(data, 0, 0xffffffff);
                flash_inc_retries(fg, addr, wmask, 
				  &fg->fg_write_errors[0]);
                return(FALSE);
            }
            break;
        default:
            return (FALSE);
        }

        /* check suspend every 256 writes */
        if ((i & 0xff) == 0)
            process_may_suspend();
    }
    return(TRUE);
}

/*
 * Program RAM if we have that in place of flash. NCR went and did this
 * during the flash shortage.
 */
static boolean flash_prog_quad_ram (dev_info_t *dev, ulong data,
                                    volatile ulong *ptr)
{
    flash_geom_t *fg;

    fg = dev->dev_geom;
    if (fg == NULL)
        return (FALSE);
    switch (fg->fg_width) {
    case BW_32BITS:
        *ptr = data;
        return (data == *ptr);   /* read value at location */
    default:
        return (FALSE);
    }
}
/*--------------------------------------------------------------------------*/
/*  8 to 32 bits of data to program come in 'data'.                         */
/*  ptr is the address in the device we wish to place this data.            */
/*  if bus width is  8 bits, then data will be 000000xx                     */
/*                          and mask will be  000000ff.                     */
/*        this places the data byte 'xx' at *ptr.                           */
/*  if bus width is 16 bits, then data will be 0000abcd                     */
/*     only byte locations *ptr and *(ptr+1) can be affected.               */
/*     if mask is 000000ff   only *ptr is written with value cd.            */
/*     if mask is 0000ffff   *ptr gets ab, and *(ptr+1) gets cd.            */
/*     if mask is 0000ff00   *ptr gets ab, and *(ptr+1) is unchanged.       */
/*  if bus width is 32 bits, then data contains upto 32 bits of data,       */
/*     and the mask determines which if *ptr thru *(ptr+3) gets affected.   */
/*--------------------------------------------------------------------------*/
static boolean flash_prog_ram (dev_info_t *dev, ulong data,
                               volatile ulong *ptr, ulong mask)
{
    flash_geom_t *fg;
    ulong val;
    ushort short_val;
    ushort short_data;
    ushort *shortptr;
    uchar *byteptr;
    ushort short_mask;

    short_val = (*shortptr & ~short_mask) | short_data;
    shortptr = (ushort*)ptr;

    fg = (flash_geom_t *)dev->dev_geom;
    if (fg == NULL)
	return (FALSE);
    switch (fg->fg_width) {
    case BW_32BITS:
        val = (*ptr & ~mask) | data;
        *ptr = val;
        return (TRUE);
    case BW_8BITS:
        byteptr = (uchar *)ptr;
	if (mask == 0xff) {
	    *byteptr = data;
        }
        return (TRUE);
    case BW_16BITS:
        shortptr = (ushort *)ptr;
	short_data = (*shortptr & ~mask) | (data & mask);
	*shortptr = short_data;
        return (TRUE);
    default:
        return (FALSE);
    }
}

static ulong flash_erase_ram (dev_info_t *dev, uchar *addr, ulong len)
{
    ulong l = len;
    len = 0;
    if (dev) {
        addr += dev->dev_addr;
        if (((ulong)addr+l) <= dev->dev_end) {
            len = l;
            while (l--) {
                *addr++ = 0xff;
            }
        }
    }
    return (len);
}

/***************************************************************************
 * erase and programming algorithms for AMD/INTEL 008 chips
 ***************************************************************************//*
 * A fast write routine that does flash-width writes at flash-width
 * aligned boundaries.
 */
static boolean flash_008_prog_quad (dev_info_t *dev, ulong data,
                                    volatile ulong *ptr)
{
    int j;
    flash_geom_t *fg;

    fg = dev->dev_geom;
    switch (fg->fg_width) {
    case BW_8BITS: {
        volatile uchar *cptr = (volatile uchar *)ptr;
        *cptr = (uchar)(program_mem_008);
        *cptr = (uchar)data;
        *cptr = (uchar)(read_status_008);
        WASTETIME(fg->fg_usec10);

        for ( j = 0; j < READY_LOOP_COUNT_008; j ++ ) {
            if ( (*cptr & FLASH_READY) == (FLASH_READY & 0xff))
                break;
        }
        *cptr = (uchar)clear_status_008; /* clear status regsiter */
        *cptr = (uchar)read_mem_008;     /* set read mode */
        return ((uchar)data == *cptr);          /* read value at location */
    }

    case BW_16BITS: {
        volatile ushort *sptr = (volatile ushort *)ptr;
        *sptr = (ushort)(program_mem_008);
        *sptr = (ushort)data;
        *sptr = (ushort)(read_status_008);
        WASTETIME(fg->fg_usec10);

        for ( j = 0; j < READY_LOOP_COUNT_008; j ++ ) {
            if ( (*sptr & FLASH_READY) == (FLASH_READY & 0xffff))
                break;
        }
        *sptr = (ushort)(clear_status_008); /* clear status regsiter */
        *sptr = (ushort)read_mem_008;     /* set read mode */
        return ((ushort)data == *sptr);          /* read value at location */
    }

    case BW_32BITS:
        *ptr = program_mem_008;
        *ptr = data;
        *ptr = read_status_008;
        WASTETIME(fg->fg_usec10);

        for ( j = 0; j < READY_LOOP_COUNT_008; j ++ ) {
            if ( (*ptr & FLASH_READY) == FLASH_READY)
                break;
        }
        *ptr = clear_status_008; /* clear status regsiter */
        *ptr = read_mem_008;     /* set read mode */
        return (data == *ptr);   /* read value at location */
    default:
        return (FALSE);
    }
}

/*
 * A normal write routine that does writes of less than flash-width
 * bytes. Writes are always aligned to flash-width boundary, but all
 * chips within that width may not be written to. Usually called
 * during write retries (to program specific chips), or while writing
 * unaligned heads and tails.
 */
static boolean flash_008_prog (dev_info_t *dev, ulong data,
                              volatile ulong *ptr, ulong mask)
{
    int j;
    ulong readback = 0;
    ulong wmask = mask;
    ulong smask;
    flash_geom_t *fg;

    fg = dev->dev_geom;
    j = 0;
    smask = FLASH_READY & mask;

    /*  Set write command for chips into which write is
     *  to be done.  Set read command for the other
     *  chips in the bank.
     */
    switch (fg->fg_width) {
    case BW_8BITS: {
        volatile uchar *cptr = (volatile uchar *)ptr;
        *cptr = (uchar)fl_cmd(program_mem_008, read_mem_008, wmask);
        *cptr = (uchar)fl_cmd(data, 0xffffffff, wmask);
        *cptr = (uchar)(read_status_008);
        do {
            WASTETIME(fg->fg_usec10);
            j++;
            readback = (uchar)(*cptr & mask);
        }
        while (((readback & smask) != smask) && 
	       (j < READY_LOOP_COUNT_008));

        if (readback & FLASH_VPP_LOW) {            /* Vpp low detected */
            if (dev->dev_flags & DEV_FLAGS_VERBOSE)
                printf("\n%%Error: %s write protected\n", dev->dev_name);
            /* clear status register */
            *cptr = (uchar)(clear_status_008);
            /* set read mode */
            *cptr = (uchar)read_mem_008;
            /* read value at location */
            readback = *cptr;
        }
        else  if (readback & FLASH_WRITE_ERR) {  /* write error in chip(s) */            /* clear status regsiter */
            *cptr = (uchar)(clear_status_008);
            /* set read mode */
            *cptr = (uchar)read_mem_008;
            /* read value at location */
            readback = *cptr;
            wmask = flash_got_exp(readback, data, mask);
            flash_inc_retries(fg, (uchar *)cptr, wmask, 
			      &fg->fg_write_errors[0]);
        }
        else  {
            /* set read mode */
            *cptr = read_mem_008;
            return (TRUE);
        }
        break;
    }

    case BW_16BITS: {
        volatile ushort *sptr = (volatile ushort *)ptr;
        *sptr = (ushort)fl_cmd(program_mem_008, read_mem_008, wmask);
        *sptr = (ushort)fl_cmd(data, 0xffffffff, wmask);
        *sptr = (ushort)(read_status_008);
        do {
            WASTETIME(fg->fg_usec10);
            j++;
            readback = (ushort)(*sptr & mask);
        }
        while (((readback & smask) != smask) && 
	       (j < READY_LOOP_COUNT_008));

        if (readback & FLASH_VPP_LOW) {            /* Vpp low detected */
            if (dev->dev_flags & DEV_FLAGS_VERBOSE)
                printf("\n%%Error: %s write protected\n", dev->dev_name);
            /* clear status register */
            *sptr = (ushort)(clear_status_008);
            /* set read mode */
            *sptr = (ushort)read_mem_008;
            /* read value at location */
            readback = *sptr;
        }
        else  if (readback & FLASH_WRITE_ERR) {  /* write error in chip(s) */            /* clear status regsiter */
            *sptr = (ushort)(clear_status_008);
            /* set read mode */
            *sptr = (ushort)read_mem_008;
            /* read value at location */
            readback = *sptr;
            wmask = flash_got_exp(readback, data, mask);
            flash_inc_retries(fg, (uchar *)sptr, wmask, 
			      &fg->fg_write_errors[0]);
        }
        else  {
            /* set read mode */
            *sptr = read_mem_008;
            return (TRUE);
        }
        break;
    }

    case BW_32BITS:
        *ptr = fl_cmd(program_mem_008, read_mem_008, wmask);
        *ptr = fl_cmd(data, 0xffffffff, wmask);
        *ptr = read_status_008;
        do {
            WASTETIME(fg->fg_usec10);
            j++;
            readback = (*ptr & mask);
        }
        while (((readback & smask) != smask) && 
	       (j < READY_LOOP_COUNT_008));

        if (readback & FLASH_VPP_LOW) {            /* Vpp low detected */
            if (dev->dev_flags & DEV_FLAGS_VERBOSE)
                printf("\n%%Error: %s write protected\n", dev->dev_name);
            /* clear status regsiter */
            *ptr = clear_status_008;
            /* set read mode */
            *ptr = read_mem_008;
            /* read value at location */
            readback = *ptr;
        }
        else  if (readback & FLASH_WRITE_ERR) {  /* write error in chip(s) */            /* clear status regsiter */
            *ptr = clear_status_008;
            /* set read mode */
            *ptr = read_mem_008;
            /* read value at location */
            readback = *ptr;
            wmask = flash_got_exp(readback, data, mask);
            flash_inc_retries(fg, (uchar *)ptr, wmask, 
			      &fg->fg_write_errors[0]);
        }
        else  {
            /* set read mode */
            *ptr = read_mem_008;
            return (TRUE);
        }
        break;
    default:
        return (FALSE);
    }
    if (dev->dev_flags & DEV_FLAGS_VERBOSE)
        printf("\n%%Error: %s location 0x%8x - wrote 0x%8x, read 0x%8x\n",
               dev->dev_name, ptr, data, readback);
    return (FALSE);
}

/*
 * Verify flash data. May be called after erase or zeroing to
 * verify that flash was actually erased or zeroed. Just reads
 * and compares with the supplied value.
 */
static boolean flash_data_verify (uchar *addr, ulong len,
                                  ushort value, boolean verb)
{
    volatile ushort *flash_addr;
    ushort data;
    ulong  i;

    flash_addr = (ushort *)addr;

    for (i = 0; i < len; i +=sizeof(ushort), flash_addr++ ) {
        data = *flash_addr;
        if (data != value) {
            if (verb) {
                printf("\n%%Error: Flash verify at 0x%8x - expected 0x%8x, read 0x%8x\n",
                       flash_addr, value, data);
            }
            return (FALSE);
        }

        /* check suspend every 1K verifies */
        if ((i & 0x3ff) == 0)
            process_may_suspend();
    }
    return (TRUE);
}

/*
 * Lowest level write routine for erasing the 008 chips.
 */
static boolean flash_008_erase_op (dev_info_t *dev, uchar *addr)
{
    flash_geom_t *fg;
    dev_cons_info_t *devcons;
    ulong data, mask;
    ulong smask;
    int i, j;

    /*
     * Set up address to start of sector
     */
    fg = dev->dev_geom;
    devcons = dev->devcons;
    mask = fg->fg_wmask;

    switch (fg->fg_width) {
    case BW_8BITS: {
        volatile uchar *caddr = (volatile uchar *)addr;
        for (i = 0; i < fg->fg_etries; i++)  {   /* retry loop for errors */
            /* clear status for safety sake */
            *caddr = (uchar)(clear_status_008);

            /* give erase commands */
            *caddr = (uchar)fl_cmd(erase_mem_008, read_mem_008, mask);
            *caddr = (uchar)fl_cmd(erase_cnf_008, read_mem_008, mask);
            *caddr = (uchar)(read_status_008);   /* set read status */

            /*
             * Loop while flash busy with erase.
             */
            smask = FLASH_READY & mask;
            for (j = 0; j < READY_LOOP_COUNT_008; j++) {
                (*devcons->dev_delay_fn)(2);
                data = *caddr & mask;
                if ((data & smask) == smask)
                    break;
            }

            if (data & FLASH_VPP_LOW) {  /* Vpp low detected */
                if (dev->dev_flags & DEV_FLAGS_VERBOSE)
		    printf("\n%%Error: %s write protected\n", dev->dev_name);
                /* clear status register */
                *caddr = (uchar)(clear_status_008);
                /* revert flash to read mode */
                *caddr = (uchar)read_mem_008;
                return (FALSE);
            } else {
                if (data & FLASH_ERASE_ERR)     /* flash erase error */
                    /* clear the error */
                    *caddr = (uchar)(clear_status_008);
                else
                    break;   /* erase a success */
            }

            /*
             *  Prepare for retries. Check each chip for error.
             *  If error reported, set erase commands, else,
             *  set read command for that chip
             */

            mask = flash_got_exp(data, FLASH_ERASE_ERR, mask);
            flash_inc_retries(fg, (uchar *)caddr, mask, 
			      &fg->fg_erase_errors[0]);
        }         /* end of retry for-loop */

        *caddr = (uchar)read_mem_008;  /* revert flash to read mode */
        break;
    }

    case BW_16BITS: {
        volatile ushort *saddr = (volatile ushort *)addr;
        for (i = 0; i < fg->fg_etries; i++)  {   /* retry loop for errors */
            /* clear status for safety sake */
            *saddr = (ushort)(clear_status_008);

            /* give erase commands */
            *saddr = (ushort)fl_cmd(erase_mem_008, read_mem_008, mask);
            *saddr = (ushort)fl_cmd(erase_cnf_008, read_mem_008, mask);
            *saddr = (ushort)(read_status_008);   /* set read status */

            /*
             * Loop while flash busy with erase.
             */
            smask = FLASH_READY & mask;
            for (j = 0; j < READY_LOOP_COUNT_008; j++) {
                (*devcons->dev_delay_fn)(2);
                data = *saddr & mask;
                if ((data & smask) == smask)
                    break;
            }

            if (data & FLASH_VPP_LOW) {  /* Vpp low detected */
                if (dev->dev_flags & DEV_FLAGS_VERBOSE)
		    printf("\n%%Error: %s write protected\n", dev->dev_name);
                /* clear status register */
                *saddr = (ushort)(clear_status_008);
                *saddr = (ushort)read_mem_008;
                return (FALSE);
            } else {
                if (data & FLASH_ERASE_ERR)     /* flash erase error */
                    /* clear the error */
                    *saddr = (ushort)(clear_status_008);
                else
                    break;   /* erase a success */
            }

            /*
             *  Prepare for retries. Check each chip for error.
             *  If error reported, set erase commands, else,
             *  set read command for that chip
             */

            mask = flash_got_exp(data, FLASH_ERASE_ERR, mask);
            flash_inc_retries(fg, (uchar *)saddr, mask, 
			      &fg->fg_erase_errors[0]);
        }         /* end of retry for-loop */

        /* revert flash to read mode */
        *saddr = (ushort)read_mem_008;
        break;
    }

    case BW_32BITS: {
        volatile ulong *laddr = (volatile ulong *)addr;
        for (i = 0; i < fg->fg_etries; i++)  {   /* retry loop for errors */
            /* clear status for safety sake */
            *laddr = clear_status_008;

            /* give erase commands */
            *laddr = fl_cmd(erase_mem_008, read_mem_008, mask);
            *laddr = fl_cmd(erase_cnf_008, read_mem_008, mask);
            *laddr = read_status_008;   /* set read status */

            /*
             * Loop while flash busy with erase.
             */
            smask = FLASH_READY & mask;
            for (j = 0; j < READY_LOOP_COUNT_008; j++) {
                (*devcons->dev_delay_fn)(2);
                data = *laddr & mask;
                if ((data & smask) == smask)
                    break;
            }

            if (data & FLASH_VPP_LOW) {  /* Vpp low detected */
                if (dev->dev_flags & DEV_FLAGS_VERBOSE)
                    printf("\n%%Error: %s write protected\n", dev->dev_name);
                *laddr = clear_status_008; /* clear status register */
                *laddr = read_mem_008;  /* revert flash to read mode */
                return (FALSE);
            } else {
                if (data & FLASH_ERASE_ERR)     /* flash erase error */
                    *laddr = clear_status_008;  /* clear the error */
                else
                    break;   /* erase a success */
            }

            /*
             *  Prepare for retries. Check each chip for error.
             *  If error reported, set erase commands, else,
             *  set read command for that chip
             */

            mask = flash_got_exp(data, FLASH_ERASE_ERR, mask);
            flash_inc_retries(fg, (uchar *)addr, mask, 
			      &fg->fg_erase_errors[0]);
        }         /* end of retry for-loop */

        *laddr = read_mem_008;  /* revert flash to read mode */
        break;
    }
    default:
        return (FALSE);
    }  /* end switch */
    /*
     * Return TRUE or FALSE depending on whether the erase was a
     * success or not.
     */
    return ((i == fg->fg_etries)  ? FALSE : TRUE);
}

static ulong flash_008_erase (dev_info_t *dev, uchar *addr, ulong len)
{
    ulong    erase_len;
    flash_geom_t *fg;
    boolean status = TRUE; /* lets be optimistic */

    /*
     *  This case does not require any bank preconditioning
     *  or verify. They are done by the chip.
     */
    fg = dev->dev_geom;
    addr += dev->dev_addr;  /* get physical address */
    erase_len = 0;
    switch (fg->fg_width) {
    case BW_8BITS :
        *(uchar *)addr = read_mem_008; /* set flash in read mode */
        break;

    case BW_16BITS :
        *(ushort *)addr = read_mem_008; /* set flash in read mode */
        break;

    case BW_32BITS :
        *(ulong *)addr = read_mem_008; /* set flash in read mode */
        break;

    default :
        return (0);
    }
    WASTETIME(fg->fg_usec6);
    /*
     * Before starting erase, check if Flash is already erased.
     */
    if (!flash_data_verify(addr, len, 0xffff, FALSE)) {
        status = flash_008_erase_op(dev, addr);
    }
    return (status ? len : 0);
}

/***************************************************************************
 * end of INTEL 008 specific code
 ***************************************************************************/

/***************************************************************************
 * erase and programming algorithms for AMD/INTEL 010, 020 chips
 ***************************************************************************/
/*
 * A fast write routine that does flash-width writes at flash-width
 * aligned boundaries.
 */
static boolean flash_010_prog_quad (dev_info_t *dev, ulong data,
                                    volatile ulong *ptr)
{
    flash_geom_t *fg;

    fg = dev->dev_geom;
    switch (fg->fg_width) {
    case BW_8BITS: {
        volatile uchar *cptr = (volatile uchar *)ptr;
        *cptr = (uchar)(program_mem_010);
        *cptr = (uchar)data;
        WASTETIME(fg->fg_usec10);
        *cptr = (uchar)(program_verify_010);
        WASTETIME(fg->fg_usec6);
        *cptr = (uchar)reset_cmd_010;
        if ((uchar)data == *cptr) {
            return(TRUE);
        }
        return(FALSE);
    }

    case BW_16BITS: {
        volatile ushort *sptr = (volatile ushort *)ptr;
        *sptr = (ushort)(program_mem_010);
        *sptr = (ushort)data;
        WASTETIME(fg->fg_usec10);
        *sptr = (ushort)(program_verify_010);
        WASTETIME(fg->fg_usec6);
        *sptr = (ushort)reset_cmd_010;
        if ((ushort)data == *sptr) {
            return(TRUE);
        }
        return(FALSE);
    }

    case BW_32BITS:
        *ptr = program_mem_010;
        *ptr = data;
        WASTETIME(fg->fg_usec10);
        *ptr = program_verify_010;
        WASTETIME(fg->fg_usec6);
        *ptr = reset_cmd_010;
        if (data == *ptr) {
            return(TRUE);
        }
        return(FALSE);

    default:
        return(FALSE);
    }
}

/*
 * A normal write routine that does writes of less than flash-width
 * bytes. Writes are always aligned to flash-width boundary, but all
 * chips within that width may not be written to. Usually called
 * during write retries (to program specific chips), or while writing
 * unaligned heads and tails.
 */
static boolean flash_010_prog (dev_info_t *dev, ulong data,
                               volatile ulong *ptr, ulong mask)
{
    int writepulse;
    ulong readback = 0;
    ulong wmask = mask;
    flash_geom_t *fg;

    fg = dev->dev_geom;
    switch (fg->fg_width) {

    case BW_8BITS: {
        volatile uchar *cptr = (volatile uchar *)ptr;
        for( writepulse = 0; writepulse < fg->fg_wtries; writepulse++) {
            /* Give write command followed by data */
            *cptr = (uchar)fl_cmd(program_mem_010, reset_cmd_010, wmask);
            *cptr = (uchar)fl_cmd(data, 0xff, wmask);
            WASTETIME(fg->fg_usec10 + fg->fg_usec6);
            /* Now give verify command */
            *cptr = (uchar)fl_cmd(program_verify_010, reset_cmd_010, wmask);
            WASTETIME(fg->fg_usec10);

            /* Now read data written to flash */
            readback = *cptr;
            /* Match data read back versus data written */
            wmask = flash_got_exp(readback, data, wmask);
            /* Reset chips */
            *cptr = (uchar)reset_cmd_010;
            if (wmask == 0) {
                return(TRUE);
            }

            /* Be persistent. Give one more try */
            readback = *cptr;
            wmask = flash_got_exp(readback, data, wmask);
            *cptr = (uchar)reset_cmd_010;
            if (wmask == 0) {
                return(TRUE);
            }

            /* Nogo. Write failed. Increment write retries */
            flash_inc_retries(fg, (uchar *)cptr, wmask, 
			      &fg->fg_write_errors[0]);
        }
        break;
    }

    case BW_16BITS: {
        volatile ushort *sptr = (volatile ushort *)ptr;
        for( writepulse = 0; writepulse < fg->fg_wtries; writepulse++) {
            /* Give write command followed by data */
            *sptr = (ushort)fl_cmd(program_mem_010, reset_cmd_010, wmask);
            *sptr = (ushort)fl_cmd(data, 0xffff, wmask);
            WASTETIME(fg->fg_usec10 + fg->fg_usec6);
            /* Now give verify command */
            *sptr = (ushort)fl_cmd(program_verify_010, reset_cmd_010, wmask);
            WASTETIME(fg->fg_usec10);

            /* Now read data written to flash */
            readback = *sptr;
            /* Match data read back versus data written */
            wmask = flash_got_exp(readback, data, wmask);
            /* Reset chips */
            *sptr = (ushort)reset_cmd_010;
            if (wmask == 0) {
                return(TRUE);
            }

            /* Be persistent. Give one more try */
            readback = *sptr;
            wmask = flash_got_exp(readback, data, wmask);
            *sptr = (ushort)reset_cmd_010;
            if (wmask == 0) {
                return(TRUE);
            }

            /* Nogo. Write failed. Increment write retries */
              flash_inc_retries(fg, (uchar *)sptr, wmask, 
				&fg->fg_write_errors[0]);
        }
        break;
    }

    case BW_32BITS:
        for( writepulse = 0; writepulse < fg->fg_wtries; writepulse++) {
            /* Give write command followed by data */
            *ptr = fl_cmd(program_mem_010, reset_cmd_010, wmask);
            *ptr = fl_cmd(data, 0xffffffff, wmask);
            WASTETIME(fg->fg_usec10 + fg->fg_usec6);
            /* Now give verify command */
            *ptr = fl_cmd(program_verify_010, reset_cmd_010, wmask);
            WASTETIME(fg->fg_usec10);

            /* Now read data written to flash */
            readback = *ptr;
            /* Match data read back versus data written */
            wmask = flash_got_exp(readback, data, wmask);
            /* Reset chips */
            *ptr = reset_cmd_010;
            if (wmask == 0) {
                return(TRUE);
            }

            /* Be persistent. Give one more try */
            readback = *ptr;
            wmask = flash_got_exp(readback, data, wmask);
            *ptr = reset_cmd_010;
            if (wmask == 0) {
                return(TRUE);
            }

            /* Nogo. Write failed. Increment write retries */
            flash_inc_retries(fg, (uchar *)ptr, wmask, 
			      &fg->fg_write_errors[0]);
        }
        break;

    default:
        return (FALSE);
    }

    if (wmask == 0) return(TRUE);
    if (dev->dev_flags & DEV_FLAGS_VERBOSE)
        printf("\n%%Error: %s location 0x%8x - wrote 0x%8x, read 0x%8x\n",
               dev->dev_name, ptr, data, readback);
    return (FALSE);
}

/*
 * Lowest level erase routine for 010 and 020 chips.
 */
static boolean flash_010_erase_op (dev_info_t *dev, uchar *addr, ulong len)
{
    uchar *flash_addr;
    int    i, erasepulse;
    ulong  data, erasecmd, emask, delay_cnt;
    flash_geom_t *fg;
    dev_cons_info_t *devcons;

    /* Program all locations to zero before erase */
    if (!flash_zero(dev, addr, len))
        return(FALSE);
    if (dev->dev_flags & DEV_FLAGS_VERBOSE) {
        printc('e');
        flush();
    }


    fg = dev->dev_geom;
    devcons = dev->devcons;
    flash_addr = addr;

    /* perform initial erase */
    switch (fg->fg_width) {
    case BW_8BITS: {
        volatile uchar *caddr = (volatile uchar *)addr;
        *caddr = (uchar)(erase_cmd_010);
        *caddr = (uchar)(erase_cmd_010);
        break;
    }

    case BW_16BITS: {
        volatile ushort *saddr = (volatile ushort *)addr;
        *saddr = (ushort)(erase_cmd_010);
        *saddr = (ushort)(erase_cmd_010);
        break;
    }

    case BW_32BITS: {
        volatile ulong *laddr = (volatile ulong *)addr;
        *laddr = (ulong)erase_cmd_010;
        *laddr = (ulong)erase_cmd_010;
        break;
    }

    default:
        return (FALSE);
    }  /* end case */
    (*devcons->dev_delay_fn)(10);

    /* now verify each address, re-erasing any byte! which fails to verify  */
    i = len;           /* loop control */
    erasepulse = fg->fg_etries;
    delay_cnt = 0;

            /*
             * Important: We must re-erase only the byte(s) that
             * needs re-erasing.  If you re-erase a location
             * that is already erased the part may be destroyed.
             * This code sets up the erasecmd for the next erasepulse
             * substituting the READ_MEMORY command for the bytes
             * that do not need re-erasing.
             */

    switch (fg->fg_width) {
    case BW_8BITS: {
        volatile uchar *caddr = (volatile uchar *)addr;
        do {
            *caddr = (uchar)(erase_verify_010);
            WASTETIME(fg->fg_usec6);
            data = (ulong)*caddr;

            if(data == 0xff) {   /* byte successfully erased */
                caddr++;
                i--;
                if ((++delay_cnt % 0x3FF) == 0)
                    (*devcons->dev_delay_fn)(10); /* dismiss periodically */
            } else { /* erase not successful at this location */
                *caddr = (uchar)(erase_cmd_010);
                *caddr = (uchar)(erase_cmd_010);
                flash_inc_retries(fg, (uchar *)caddr, 0xff, 
				  &fg->fg_erase_errors[0]);
                (*devcons->dev_delay_fn)(10);
                erasepulse--;
            }
        }  while ((i > 0) && (erasepulse > 0));
        break;
    }

    case BW_16BITS: {
        volatile ushort *saddr = (volatile ushort *)addr;
        do {
            *saddr = (ushort)(erase_verify_010);
            WASTETIME(fg->fg_usec6);
            data = (ulong)*saddr;

            if(data == 0xffff) {   /* short word successfully erased */
                saddr++;
                i -= sizeof(ushort);
                if ((++delay_cnt % 0x3FF) == 0)
                    (*devcons->dev_delay_fn)(10); /* dismiss periodically */
            } else { /* erase not successful at this location */
                emask = flash_got_exp(data, 0xffff, 0xffff);
                erasecmd = fl_cmd(erase_cmd_010, reset_cmd_010, emask);
                *saddr = (ushort)erasecmd;
                *saddr = (ushort)erasecmd;
                flash_inc_retries(fg, (uchar *)saddr, emask, 
				  &fg->fg_erase_errors[0]);
                (*devcons->dev_delay_fn)(10);
                erasepulse--;
            }
        }  while ((i > 0) && (erasepulse > 0));
        break;
    }

    case BW_32BITS: {
        volatile ulong *laddr = (volatile ulong *)addr;
        do {
            *laddr = (ulong)erase_verify_010;
            WASTETIME(fg->fg_usec6);
            data = (ulong)*laddr;

            if(data == 0xffffffff) {   /* long word successfully erased */
                laddr++;
                i -= sizeof(ulong);
                if ((++delay_cnt % 0x3FF) == 0) /* dismiss periodically */
                    (*devcons->dev_delay_fn)(10);

            } else { /* erase not successful at this location */
                emask = flash_got_exp(data, 0xffffffff, 0xffffffff);
                erasecmd = fl_cmd(erase_cmd_010, reset_cmd_010, emask);
                *laddr = erasecmd;
                *laddr = erasecmd;
                flash_inc_retries(fg, (uchar *)laddr, emask, 
				  &fg->fg_erase_errors[0]);
                (*devcons->dev_delay_fn)(10);
                erasepulse--;
            }
        }  while ((i > 0) && (erasepulse > 0));
        break;
    }

    default:
        return (FALSE);
    } /* end switch */

    switch (fg->fg_width) {
    case BW_8BITS:
        *flash_addr = (uchar)reset_cmd_010;
        *flash_addr = (uchar)reset_cmd_010;
        break;
    case BW_16BITS:
        *(ushort *)flash_addr = (ushort)reset_cmd_010;
        *(ushort *)flash_addr = (ushort)reset_cmd_010;
        break;
    case BW_32BITS:
        *(ulong *)flash_addr = (ulong)reset_cmd_010;
        *(ulong *)flash_addr = (ulong)reset_cmd_010;
        break;
    default:
        return (FALSE);
    }

    /* done, but was it successful? */
    return ((erasepulse <= 0) ? FALSE : TRUE);
}

static ulong flash_010_erase (dev_info_t *dev, uchar *addr, ulong len)
{
    flash_geom_t *fg;
    boolean status = TRUE; /* lets be optimistic */

    fg = dev->dev_geom;
    addr += dev->dev_addr;  /* get physical address */
    switch (fg->fg_width) {
    case BW_8BITS :
        *(uchar *)addr = reset_cmd_010; /* set flash in read mode */
        *(uchar *)addr = reset_cmd_010; /* set flash in read mode */
        break;

    case BW_16BITS :
        *(ushort *)addr = reset_cmd_010; /* set flash in read mode */
        *(ushort *)addr = reset_cmd_010; /* set flash in read mode */
        break;

    case BW_32BITS :
        *(ulong *)addr = reset_cmd_010; /* set flash in read mode */
        *(ulong *)addr = reset_cmd_010; /* set flash in read mode */
        break;

    default:
        return (0);
    }
    WASTETIME(fg->fg_usec6);
    /*
     * Before starting erase, check if Flash is already erased.
     */
    if (!flash_data_verify(addr, len, 0xffff, FALSE)) {
        status = flash_010_erase_op(dev, addr, len);
    }
    return (status ? len : 0);
}

/***************************************************************************
 * end of INTEL/AMD 010, 020 specific code
 ***************************************************************************/
/***************************************************************************
 * Erase and write routines for the AMD 040 chips.
 ***************************************************************************/
/*
 * A fast write routine that does flash-width writes at flash-width
 * aligned boundaries.
 */
static boolean flash_040_prog_quad (dev_info_t *dev, ulong data,
        volatile ulong *ptr)
{
    int j;
    flash_geom_t *fg;
    ulong data_poll;

    fg = dev->dev_geom;

    switch (fg->fg_width) {
    case BW_32BITS: {
        volatile ulong *fptr = (ulong *)dev->dev_addr;

        /*
         * Set fptr to start of flash. Then bump it up until it reaches
         * start of the bank we are interested in. Since fptr is a ulong
         * pointer, bump it up by chipsize every time. This assumes that
         * flash width is a ulong.
         * Then issue the unlock cycles followed by the program setup
         * command. Note that the program setup command is not written
         * at the location to be programmed but at a fixed offset (0x5555)
         * within the chip. After this the data is written at the actual
         * location to be programmed.
         *
         */
        while((ulong)ptr >= (ulong)(fptr + fg->fg_chipsize))
            fptr += fg->fg_chipsize;

        /*  Set write command for chips into which write is
         *  to be done.  Set read command for the other
         *  chips in the bank.
         */
        fptr[ADR_040_5555] = ADR_040_5555_VAL;
        fptr[ADR_040_2AAA] = ADR_040_2AAA_VAL;
        fptr[ADR_040_5555] = program_mem_040;
        *ptr = data;
        
        data_poll = data & FLASH_READY;
        for (j = 0; j < READY_LOOP_COUNT_040; j++) {
            /*
             * While a chip is being programmed, the poll bit becomes the
             * complement of the bit value being written. After the write
             * is complete, the poll bit reflects the actual value written.
             */
             if ((*ptr & FLASH_READY) == data_poll)  /* All chips programmed */
                break;
            /*
             * If all chips have timed out, exit from loop. The timeout may
             * be due to either success or failure.
             */
            if ((*ptr & FLASH_TIMEOUT) == FLASH_TIMEOUT) /* Write timeout */
                break;
        }
        return (data == *ptr);           /* read value at location */
    }  /* end case 32BITS */
    default:
        return (FALSE);
    } /* end switch */
}

/*
 * A normal write routine that does writes of less than flash-width
 * bytes. Writes are always aligned to flash-width boundary, but all
 * chips within that width may not be written to. Usually called
 * during write retries (to program specific chips), or while writing
 * unaligned heads and tails.
 */
static boolean flash_040_prog (dev_info_t *dev, ulong data,
                  volatile ulong *ptr,ulong mask)
{
    int j;
    ulong readback = 0;
    ulong wmask = mask;
    ulong smask;
    flash_geom_t *fg;

    fg = dev->dev_geom;
    /*
     * Mask FLASH_READY to correspond only to the chips being programmed.
     */
    smask = FLASH_READY & mask;
    /*
     * Generate mask for checking data polling bit.
     */
    wmask = data & smask;

    switch (fg->fg_width) {
    case BW_16BITS: {
        volatile ushort *fptr = (ushort *)dev->dev_addr;
        volatile ushort *sptr = (ushort *)ptr;
        
	while((ulong)ptr >= (ulong)(fptr + fg->fg_chipsize))
            fptr += fg->fg_chipsize;

        /*  Set write command for chips into which write is
         *  to be done.  Set read command for the other
         *  chips in the bank.
         */

        fptr[ADR_040_5555] = fl_cmd(ADR_040_5555_VAL, read_mem_040, mask);
        fptr[ADR_040_2AAA] = fl_cmd(ADR_040_2AAA_VAL, read_mem_040, mask);
        fptr[ADR_040_5555] = fl_cmd(program_mem_040, read_mem_040, mask);
        *sptr = fl_cmd(data, read_mem_040, mask);

        for (j = 0; j < READY_LOOP_COUNT_040; j++) {
            WASTETIME(fg->fg_usec10);
            /*
             * While a chip is being programmed, the poll bit becomes the
             * complement of the bit value being written. After the write
             * is complete, the poll bit reflects the actual value written.
             */
            if (wmask == (*sptr & smask))  /* Data poll success */
                break;
            /*
             * If all chips have timed out, exit from loop. The timeout may
             * be due to either success or failure.
             */
            if ((*sptr & FLASH_TIMEOUT & mask) == (FLASH_TIMEOUT & mask))
                break;
        }

        if ((data & mask) == (*sptr & mask))  /* success */
            return (TRUE);
        else {
            readback = *sptr & mask;
            if (dev->dev_flags & DEV_FLAGS_VERBOSE)
                printf("\n%%Error: %s location 0x%8x - wrote 0x%8x, read 0x%8x\n
",
                       dev->dev_name, ptr, data & mask, readback);
            return (FALSE);
        }
    }  /* end case 16BITS */
    case BW_32BITS: {
        volatile ulong *fptr = (ulong *)dev->dev_addr;

        /*
         * Set fptr to start of flash. Then bump it up until it reaches
         * start of the bank we are interested in. Since fptr is a ulong
         * pointer, bump it up by chipsize every time. This assumes that
         * flash width is a ulong.
         * Then issue the unlock cycles followed by the program setup
         * command. Note that the program setup command is not written
         * at the location to be programmed but at a fixed offset (0x5555)
         * within the chip. After this the data is written at the actual
         * location to be programmed.
         *
         */
        while((ulong)ptr >= (ulong)(fptr + fg->fg_chipsize))
            fptr += fg->fg_chipsize;

        /*  Set write command for chips into which write is
         *  to be done.  Set read command for the other
         *  chips in the bank.
         */

        fptr[ADR_040_5555] = fl_cmd(ADR_040_5555_VAL, read_mem_040, mask);
        fptr[ADR_040_2AAA] = fl_cmd(ADR_040_2AAA_VAL, read_mem_040, mask);
        fptr[ADR_040_5555] = fl_cmd(program_mem_040, read_mem_040, mask);
        *ptr = fl_cmd(data, read_mem_040, mask);

        for (j = 0; j < READY_LOOP_COUNT_040; j++) {
            WASTETIME(fg->fg_usec10);
            /*
             * While a chip is being programmed, the poll bit becomes the
             * complement of the bit value being written. After the write
             * is complete, the poll bit reflects the actual value written.
             */
            if (wmask == (*ptr & smask))  /* Data poll success */
                break;
            /*
             * If all chips have timed out, exit from loop. The timeout may
             * be due to either success or failure.
             */
            if ((*ptr & FLASH_TIMEOUT & mask) == (FLASH_TIMEOUT & mask))
                break;
        }

        if ((data & mask) == (*ptr & mask))  /* success */
            return (TRUE);
        else {
            readback = *ptr & mask;
            if (dev->dev_flags & DEV_FLAGS_VERBOSE)
                printf("\n%%Error: %s location 0x%8x - wrote 0x%8x, read 0x%8x\n",
                       dev->dev_name, ptr, data & mask, readback);
            return (FALSE);
        }
    }  /* end case 32BITS */
    default:
        return (FALSE);
    } /* end switch */
}

/*
 * Lowest level erase routine for the 040 chips.
 */
static boolean flash_040_erase_op (dev_info_t *dev, uchar *addr)
{
    flash_geom_t *fg;
    dev_cons_info_t *devcons;
    ulong mask;
    int i, j;

    fg = dev->dev_geom;
    devcons = dev->devcons;
    mask = fg->fg_wmask;

    switch (fg->fg_width) {
    case BW_16BITS: {
        volatile ushort *caddr = (ushort *)addr;

        for (i = 0; i < fg->fg_etries; i++)  {   /* retry loop for errors */

            mask = 0xffffL;

            if ( psipending() ) {
                if (dev->dev_flags & DEV_FLAGS_VERBOSE)
                    printf("\n%%%s erase aborted on user request\n",
                           dev->dev_name);
                return (FALSE);
            }

            /* Give erase commands. First comes the erase setup, then the
             * erase confirmation. Each erase command is preceded by two
             * 'unlock' commands. This gives a total of 6 write cycles for
             * the erase operation.
             */

            caddr[ADR_040_5555] = ADR_040_5555_VAL   &   mask;
            caddr[ADR_040_2AAA] = ADR_040_2AAA_VAL   &   mask;
            caddr[ADR_040_5555] = erase_sect_040     &   mask;
            caddr[ADR_040_5555] = ADR_040_5555_VAL   &   mask;
            caddr[ADR_040_2AAA] = ADR_040_2AAA_VAL   &   mask;
            *caddr              = erase_sect_cnf_040 &   mask;

            /*
             * Loop while flash busy with erase. Check DQ7 (bit 0 on low-end
             * because of bit reversal). During erase, bit DQ7 will go low.
             */
            for (j = 0; j < READY_LOOP_COUNT_040; j++) {
                (*devcons->dev_delay_fn)(2);
                /*
                 * Check data polling bit. During erase this bit will go low.
                 * After erase it will go high. All chips must be erased before
                 * we exit out of the loop.
                 */
                if ((*caddr & FLASH_READY) == (FLASH_READY &   mask))
                    break;
                /*
                 * Check if all chips have timed out.
                 * If all chips have timed out, exit from loop. The timeout ma y
                 * be due to either success or failure.
                 */
                if ((*caddr & FLASH_TIMEOUT) == (FLASH_TIMEOUT &   mask))
                    break;
            }

            /*
             * Check if erase was a success or a failure.
             */
            if ((*caddr & FLASH_READY) == (FLASH_READY &   mask)) {/* success */
                /*
                 * Revert flash to read mode.
                 */
                caddr[ADR_040_5555] = ADR_040_5555_VAL &   mask;
                caddr[ADR_040_2AAA] = ADR_040_2AAA_VAL &   mask;
                *caddr              = read_mem_040     &   mask;
                return (TRUE);
            }
            /*
             * Erase error. Record error.
             */
            mask = flash_got_exp(*caddr, FLASH_READY, mask);
            flash_inc_retries(fg, (uchar *)caddr, mask, 
			      &fg->fg_erase_errors[0]);
        }  /* end of retry loop */

        /*
         * Revert flash to read mode.
         */
        mask = 0xffffL;
        caddr[ADR_040_5555] = ADR_040_5555_VAL &   mask;
        caddr[ADR_040_2AAA] = ADR_040_2AAA_VAL &   mask;
        *caddr              = read_mem_040     &   mask;
        return (FALSE);
    }
    case BW_32BITS: {
        volatile ulong *caddr = (ulong *)addr;

        for (i = 0; i < fg->fg_etries; i++)  {   /* retry loop for errors */

            mask = 0xffffffffL;

            if ( psipending() ) {
                if (dev->dev_flags & DEV_FLAGS_VERBOSE)
                    printf("\n%% %s: Erase aborted on user request",
                           dev->dev_name);
                return (FALSE);
            }

            /* Give erase commands. First comes the erase setup, then the
             * erase confirmation. Each erase command is preceded by two
             * 'unlock' commands. This gives a total of 6 write cycles for
             * the erase operation.
             */

            caddr[ADR_040_5555] = ADR_040_5555_VAL;
            caddr[ADR_040_2AAA] = ADR_040_2AAA_VAL;
            caddr[ADR_040_5555] = erase_sect_040;
            caddr[ADR_040_5555] = ADR_040_5555_VAL;
            caddr[ADR_040_2AAA] = ADR_040_2AAA_VAL;
            *caddr = erase_sect_cnf_040;

            /*
             * Loop while flash busy with erase. Check DQ7 (bit 0 on low-end
             * because of bit reversal). During erase, bit DQ7 will go low.
             */
            for (j = 0; j < READY_LOOP_COUNT_040; j++) {
                (*devcons->dev_delay_fn)(2);
                /*
                 * Check data polling bit. During erase this bit will go low.
                 * After erase it will go high. All chips must be erased before
                 * we exit out of the loop.
                 */
                if ((*caddr & FLASH_READY) == FLASH_READY)
                    break;
                /*
                 * Check if all chips have timed out.
                 * If all chips have timed out, exit from loop. The timeout may
                 * be due to either success or failure.
                 */
                if ((*caddr & FLASH_TIMEOUT) == FLASH_TIMEOUT)
                    break;
            }

            /*
             * Check if erase was a success or a failure.
             */
            if ((*caddr & FLASH_READY) == FLASH_READY) { /* success */
                /*
                 * Revert flash to read mode.
                 */
                caddr[ADR_040_5555] = ADR_040_5555_VAL;
                caddr[ADR_040_2AAA] = ADR_040_2AAA_VAL;
                *caddr = read_mem_040;
                return (TRUE);
            }
            /*
             * Erase error. Record error.
             */
            mask = flash_got_exp(*caddr, FLASH_READY, mask);
            flash_inc_retries(fg, (uchar *)caddr, mask,
                              &fg->fg_erase_errors[0]);

        }  /* end of retry loop */

        /*
         * Revert flash to read mode.
         */
        caddr[ADR_040_5555] = ADR_040_5555_VAL;
        caddr[ADR_040_2AAA] = ADR_040_2AAA_VAL;
        *caddr = read_mem_040;
        return (FALSE);
    }
    default:
        return (FALSE);
    } /* end switch */
}

static ulong flash_erase_040 (dev_info_t *dev, uchar *addr, ulong len)
{
    ulong erase_len;
    flash_geom_t *fg;
    boolean status = TRUE;  /* being optimistic */

    /*
     *  This case does not require any bank preconditioning
     *  or verify. They are done by the chip.
     */
    fg = dev->dev_geom;
    addr += dev->dev_addr;  /* get physical address */
    erase_len = 0;

    switch (fg->fg_width) {
  case BW_16BITS :
        *(ushort *)addr = reset_cmd_040 & 0xffff; /* flash in read mode */
        break;

    case BW_32BITS :
        *(ulong *)addr = reset_cmd_040; /* set flash in read mode */
        break;
    default :
        return (0);
    }

    WASTETIME(fg->fg_usec6);
    /*
     * Before starting erase, check if Flash is already erased.
     */
    if (!flash_data_verify(addr, len, 0xffff, FALSE)) {
        status = flash_040_erase_op(dev, addr);
    }
    return (status ? len : 0);
}

/*************************************************************************
 *  flash info functions
 *************************************************************************/

/*
 *  Display list of flash chips supported (as defined in
 *  the flash_info_table)
 */
static void flash_print_chips_known (void)
{
    short i;
    printf("\n\nFlash chips supported by system :");
    printf("\n   Code   Rev-cd   Chip-Sz     Cmd-grp   Chip-name");
    for (i = 0; flash_info_tbl[i].chip_code != FLASH_TABLE_END; i++)
        if (flash_info_tbl[i].disp_flag) {
            printf("\n   %4s   %04x     %04dKB         %1d      %s",
                   flash_manu_code(i),
                   flash_info_tbl[i].rev_chip_code,
                   flash_chip_size(i)/1024,
                   flash_cmnd_set(i),
                   flash_name(i));
        }
}

/*
 * Display device level information -
 * total size, location, chip types, retry counts.
 */
static void les_flash_info (dev_info_t *dev, int flag)
{
    flash_geom_t *fg;
    boolean chips_recognized = TRUE;  /* be optimistic!! */
    ushort i;

    if (!(flag & SHOW_DEV_INFO_MASK))
        return;
    fg = dev->dev_geom;
    if (fg == NULL) {
        printf("\n%%Error: No %s chip information available\n", dev->dev_name);
        return;
    }

    /* Display partition number for "sh flash chips" only */
    if ((flag & SHOW_FLASH_CHIPS) && (!(flag & SHOW_FILES_DETAILED))) {
      if (dev->devcons->dev_part_cnt > 1)
          printf("\n\n%s partition %d:", dev->dev_name, dev->dev_subunit);
    }

    if (flag & SHOW_VER_FLASH_SIZE) {
      if (dev->devcons->dev_part_cnt == 1)
          printf("%dK bytes of %s %s (%s)\n",
                  dev->dev_size/1024,
                  fg->fg_card,
                  dev->dev_name,
                  dev_states[dev->dev_state]);
      else if (dev->devcons->dev_part_cnt > 1)
          printf("%dK bytes of %s %s partition %d (%s)\n",
                  dev->dev_size/1024,
                  fg->fg_card,
                  dev->dev_name,
                  dev->dev_subunit,
                  dev_states[dev->dev_state]);
    }

    if (flag & SHOW_FLASH_SIZE) {
        printf("\n%dK bytes of %s %s (%s)\n",
               dev->dev_size/1024,
               fg->fg_card,
               dev->dev_name,
               dev_states[dev->dev_state]);
    }

    if (fg->fg_status == FR_RAM)
        return;
    if (fg->fg_status == FR_REMOVED)
        return;

    if (flag & (SHOW_FLASH_CHIPS | SHOW_FLASH_ERR)) {
        printf("\n   Chip    Bank    Code      Size      Name");
        if (flag & SHOW_FLASH_ERR)
            printf("                erase  write");
        while (fg != NULL) {
            for (i = 0; i < fg->fg_chips; i++) {
                printf("\n    %2d     %d       %04x      %04dKB    %s",
                       i+1,
                       fg->fg_bank_no+1,
                       fg->fg_rom_id[i],
                       fg->fg_chipsize/1024,
                       fg->fg_chip_name[i]);
                if (flag & SHOW_FLASH_ERR) {
                      printf("      %5d  %5d",
                          fg->fg_erase_errors[i],
                          fg->fg_write_errors[i]);
                }
                if (fg->fg_chip_code[i] == 0)
                    chips_recognized = FALSE;
            }
            fg = fg->next_geom;
        }   /* end while */
        if (dev->dev_state == DEV_STATE_RFF) {
            if (dev->devcons->dev_part_cnt > 1)
                printf("\nExecuting current image from %s partition %d",
                       dev->dev_name, dev->dev_subunit);
            else
                printf("\nExecuting current image from %s", dev->dev_name);
        }
        if (!chips_recognized)
            if (dev->devcons->dev_wr_prot == FALSE)
                flash_print_chips_known();
            else 
                printf("\n%s possibly write protected\n", dev->dev_name);
    }
}

/*
 * Initialize chip level information for device.
 */
void les_flash_chipinfo_init (flash_geom_t *fg, dev_chipstat_t *dev_chipinfo)
{
    int i;
    int j;
 
    if ((fg == NULL) || (dev_chipinfo == NULL))
        return;

    /* First initialize */
    dev_chipinfo->dev_chips = 0;
    dev_chipinfo->dev_start_chip = fg->fg_start_chip;
    dev_chipinfo->dev_chip_wtries = fg->fg_wtries;
    dev_chipinfo->dev_chip_etries = fg->fg_etries;
    for (i = 0; i < MAX_CHIPS; i++) {
        dev_chipinfo->dev_chip_name[i] = UNKNOWN_CHIP_STR;
        dev_chipinfo->dev_rom_id[i]    = 0;
        dev_chipinfo->dev_chip_wp[i]   = 0;
        dev_chipinfo->dev_chip_ep[i]   = 0;
    }
    j = 0;
    while (fg != NULL) {
        dev_chipinfo->dev_chips += fg->fg_chips;
        for (i = 0; i < fg->fg_chips; i++, j++) {
            dev_chipinfo->dev_chip_name[j] = fg->fg_chip_name[i];
            dev_chipinfo->dev_rom_id[j]    = fg->fg_rom_id[i];
            dev_chipinfo->dev_chip_wp[j]   = fg->fg_write_errors[i];
            dev_chipinfo->dev_chip_ep[j]   = fg->fg_erase_errors[i];
        }
        fg = fg->next_geom;
    }
    return;
}
 

/**********************************************************************
 * Function to relink the dev_geoms for all partitions
 **********************************************************************/
/* This will adjust the next_geom pointer of the flash geometry
 * structure as we form many partitions from one, or one to partition
 * from many.  part_bounds will be NULL when the device is re-created.
 * It's used to store the partition boundary when we relink one partition
 * to many partitions.
 */
static void flash_geom_relink (dev_info_t *dev, int *part_bounds, ushort flag)
{
    flash_geom_t *fg, *last_fg = NULL;
    ulong bank_end, part_size;
    dev_info_t *tmp_dev;
    int i, j, k;
    int *partn_chip_bounds;
    int accumulated_chip_cnt = 0;
    ushort parts;
 
    fg = (flash_geom_t *)dev->dev_geom;
    if (fg == NULL) {  /* Uh oh */
        dev->dev_state = DEV_STATE_NODEV;
        return;
    }
 
    /* find out how many partitions we have */
    parts = dev->devcons->dev_part_cnt;
 
    /* array that maintains chip boundaries for each partition */
    partn_chip_bounds = dev->devcons->dev_chipinfo->dev_partn_chip_bounds;
    *partn_chip_bounds ++ = 0;

    /* see if we want to relink the device from 1 partition */
    if (flag & DEV_CLONE) {
        for (i = 0; dev && (i < parts); i++) {
            bank_end = 0;
            /* find out the size of the partition */
            part_size = (part_bounds[i+2] - part_bounds[i+1]) *
                        dev->devcons->dev_banksize;
 
            /* check the state of the device */
            if (run_from_flash(dev))
                dev->dev_state = DEV_STATE_RFF;
            else if (dev->devcons->dev_wr_prot) 
                dev->dev_state = DEV_STATE_RDONLY;
            else   
                dev->dev_state = DEV_STATE_RW;
 
            for (j = 0; fg && (bank_end < part_size); j++) {
 
                bank_end += fg->fg_banksize;
		accumulated_chip_cnt += fg->fg_chips;
                fg->fg_bank_end = bank_end;
                fg->fg_bank_no = j;
                switch (fg->fg_status) {
                    case FR_NOT_VALID:
                        dev->dev_state = DEV_STATE_RDONLY;
                        break;
                    case FR_READ_WRITE:
                        break;
                    default:
                        if (dev->dev_state != DEV_STATE_RFF)
                            dev->dev_state = DEV_STATE_RDONLY;
                        break;
                }
                last_fg = fg;
                fg = fg->next_geom;
            }
            if (last_fg)
                last_fg->next_geom = NULL;  /* break the chain */

            /* Maintain cumulative chip boundary counts for each new partition */
            *partn_chip_bounds++ = accumulated_chip_cnt;
            dev = dev->next_partition;  /* go to the next partition */
            if (dev != NULL)
                dev->dev_geom = (void *)fg; /* attach geom chain */
        }
      }
      /* we are re-creating the device to 1 partition */
      else if (flag & DEV_RECREATE) {
        k = 0;
        bank_end = 0;
        tmp_dev = dev;  /* store first dev */
        /* check the state of the device */
        if (run_from_flash(tmp_dev))
            tmp_dev->dev_state = DEV_STATE_RFF;
        else if (tmp_dev->devcons->dev_wr_prot) 
                tmp_dev->dev_state = DEV_STATE_RDONLY;
        else   
            tmp_dev->dev_state = DEV_STATE_RW;
 
        for (i = 0; dev; i++) {
            for (j = 0; fg; j++, k++) {
                bank_end += fg->fg_banksize;
		accumulated_chip_cnt += fg->fg_chips;
                fg->fg_bank_end = bank_end;
                fg->fg_bank_no = k;
                switch (fg->fg_status) {
                    case FR_NOT_VALID:
                        tmp_dev->dev_state = DEV_STATE_RDONLY;
                        break;
                    case FR_READ_WRITE:
                        break;
                    default:
                        if (tmp_dev->dev_state != DEV_STATE_RFF)
                            tmp_dev->dev_state = DEV_STATE_RDONLY;
                        break;
                }
                last_fg = fg;
                fg = fg->next_geom;
            }
 
            dev = dev->next_partition;
            if (dev != NULL) {
                fg = (flash_geom_t *)dev->dev_geom;
                last_fg->next_geom = fg;
            }
        }
        /* relinking device to single partition - store total num of chips */
        *partn_chip_bounds++ = accumulated_chip_cnt;
        last_fg->next_geom = NULL;
    }
}

/**********************************************************************
 *   Flash identification functions
 *********************************************************************/
/*
 * flash_chip_identify
 * Given a manufacturer/device code,
 *   return the index into the flash device table.
 * Return 0 for unknown devices.
 * NOTE:  The Hw is wired wrong on PAN boxes.
 * flash_chip_id_short_reversal() is for bit reversal of chip codes.
 */
static uchar flash_chip_id_byte_reversal (ushort code)
{
    uchar input_bit = 0x80;  /* start checking from the MSbit */
    uchar output_bit = 0x01;
    uchar out_code = 0;
    uchar inp_code;
    uchar cnt;

    inp_code = code & 0xff; /* chop off short into char */
    for (cnt = 0; cnt < 8; cnt++) { /* repeat for each bit in byte */
        if (inp_code & input_bit)
            out_code |= output_bit;
        input_bit >>= 1;
        output_bit <<= 1;
    }
    return out_code;
}
static ushort flash_chip_id_short_reversal (ushort code)
{
    ushort result = 0;

     /*
      * Bit reverse upper byte, then lower byte.
      * Return back complete reversed short value
      */
    result = flash_chip_id_byte_reversal(code >> 8) << 8;
    result |= flash_chip_id_byte_reversal(code);
    return (result);
}

int flash_chip_identify (ushort code)
{
    int i;

    for (i = 0; flash_info_tbl[i].chip_code != FLASH_TABLE_END; i++) {
        /*
         * Compare normal chip code. If that doesn't match, try the
         * bit-reversed chip code.
         */
        if (flash_info_tbl[i].chip_code == code)
            return (i);
        if (flash_info_tbl[i].rev_chip_code == code)
            return (i);
    }
    return (0);
}
/*
 * Probe and see if we have RAM or flash/ROM.
 */
ushort flash_probe_memory_type (dev_info_t *dev)
{
    volatile ulong val1;
    volatile ulong  *probe_ptr;

    probe_ptr = (volatile ulong *)dev->dev_addr;  /* physical start of device */
    val1 = read32v(probe_ptr);
    write32v(probe_ptr,0xc3c3c3c3);
    
    if (val1 == read32v(probe_ptr)) {  /* Flash or ROM !! */
	write32v(probe_ptr,reset_cmd);
	write32v(probe_ptr,reset_cmd);
        return(CHIP_TYPE_FLASH_ROM);
    }
    else {  /* must be RAM !! */
	write32v(probe_ptr,val1);   /* restore back original value */
        return(CHIP_TYPE_RAM);
    }
}


/***************************************************************************
 * flash_bank_query: read chip ids for a flash bank.
 ***************************************************************************/
/*
 * WARNING!!
 * flash_query_8_bit_040_end() is a dummy function that must
 * follow flash_query_8_bit_040(). 
 */
static void flash_query_8_bit_040 (volatile uchar *flashptr,
                                   uchar *manu, uchar *device,
                                   ushort delay)
{
        *(flashptr+ADR_040_5555) = (uchar)ADR_040_5555_VAL; /* unlock */
        *(flashptr+ADR_040_2AAA) = (uchar)ADR_040_2AAA_VAL;
        *(flashptr+ADR_040_5555) = (uchar)(read_id);  /* Read chip IDs */
        for ( ; delay > 0; delay--)
            ;
        *manu   = *flashptr++; /* get manuf code */
        *device = *flashptr;   /* get device code */
        *flashptr = (uchar)(reset_cmd_040);
        *flashptr = (uchar)(reset_cmd_040);
}
static void flash_query_8_bit_040_end (void)
{
    return;
}

static void flash_query_16_bit_040 (volatile uchar *flashptr,
                                    uchar *manu, uchar *device,
                                    ushort delay)
{
        *((volatile ushort *)flashptr+ADR_040_5555) = (ushort)ADR_040_5555_VAL;
        *((volatile ushort *)flashptr+ADR_040_2AAA) = (ushort)ADR_040_2AAA_VAL;
        *((volatile ushort *)flashptr+ADR_040_5555) = (ushort)(read_id);  
        for ( ; delay > 0; delay--)
            ;
	/* get manuf code */
        *(ushort *)manu   = *(volatile ushort *)flashptr;   
        flashptr += sizeof(short);
	/* get device code */
        *(ushort *)device = *(volatile ushort *)flashptr;   
        *(volatile ushort *)flashptr = (ushort)(reset_cmd_040);
        *(volatile ushort *)flashptr = (ushort)(reset_cmd_040);
}
static void flash_query_16_bit_040_end (void)
{
    return;
}

static void flash_query_32_bit_040 (volatile uchar *flashptr,
                                    uchar *manu, uchar *device,
                                    ushort delay)
{
        /* unlock */
        *((volatile ulong *)flashptr+ADR_040_5555) = ADR_040_5555_VAL; 
        *((volatile ulong *)flashptr+ADR_040_2AAA) = ADR_040_2AAA_VAL;
	/* Read chip IDs */
        *((volatile ulong *)flashptr+ADR_040_5555) = read_id;  
        for ( ; delay > 0; delay--)
            ;
	/* get manuf code */
        *(ulong *)manu   = *(volatile ulong *)flashptr;   
        flashptr += sizeof(long);
	/* get device code */
        *(ulong *)device = *(volatile ulong *)flashptr;   
        *(volatile ulong *)flashptr = reset_cmd_040;
        *(volatile ulong *)flashptr = reset_cmd_040;
}

static void flash_query_32_bit_040_end (void)
{
    return;
}

static void flash_query_8_bit_oth (volatile uchar *flashptr,
                                   uchar *manu, uchar *device,
                                   ushort delay)
{
    ulong cnt = delay * 2;
        *flashptr = (uchar)reset_cmd;
        *flashptr = (uchar)reset_cmd;  /* reset chips */
        for ( ; delay > 0; delay--)    /* 6 usec delay */
            ;
        *flashptr = (uchar)read_id;    /* Read chip IDs */
        for ( ; cnt > 0; cnt--)        /* 12 usec delay */
            ;
        *manu   = *flashptr++; /* get manuf code */
        *device = *flashptr;   /* get device code */
        *flashptr = (uchar)reset_cmd;
        *flashptr = (uchar)reset_cmd;  /* reset chips */
}
static void flash_query_8_bit_oth_end (void)
{
    return;
}

static void flash_query_16_bit_oth (volatile uchar *flashptr,
                                    uchar *manu, uchar *device,
                                    ushort delay)
{
    ulong cnt = delay * 2;
        *(volatile ushort *)flashptr = (ushort)reset_cmd;
        *(volatile ushort *)flashptr = (ushort)reset_cmd;  /* reset chips */
        for ( ; delay > 0; delay--)    /* 6 usec delay */
            ;
        *(volatile ushort *)flashptr = (ushort)read_id;    /* Read chip IDs */
        for ( ; cnt > 0; cnt--)        /* 12 usec delay */
            ;
        /* get manuf code */
        *(ushort *)manu   = *(volatile ushort *)flashptr;   
        flashptr += sizeof(short);
        /* get device code */
        *(ushort *)device = *(volatile ushort *)flashptr;   
        *(volatile ushort *)flashptr = (ushort)reset_cmd;
        *(volatile ushort *)flashptr = (ushort)reset_cmd;  /* reset chips */
}
static void flash_query_16_bit_oth_end (void)
{
    return;
}

/*
 * WARNING!!
 * flash_query_32_bit_oth_end() is a dummy function that must
 * follow flash_query_32_bit_oth(). 
 */
static void flash_query_32_bit_oth (volatile uchar *flashptr,
                                    uchar *manu, uchar *device,
                                    ushort delay)
{
    ulong cnt;
    uint i;

        for (i = 0; i < sizeof(ulong) ; i++) {
            *flashptr = (reset_cmd) & 0xff;
            *flashptr = (reset_cmd) & 0xff;
	    for (cnt = delay; cnt > 0; cnt--)     /* 6 usec delay          */
                ;
            *flashptr = (read_id) & 0xff;         /* read the chip ID's    */
	    for (cnt = delay*2; cnt > 0; cnt--)   /* 12 usec delay         */
                ;
            *manu++   = *flashptr;
            *device++ = *(flashptr + sizeof(long));
            *flashptr = (reset_cmd) & 0xff;
            *flashptr = (reset_cmd) & 0xff;
            flashptr++;
        }
}
static void flash_query_32_bit_oth_end (void)
{
    return;
}

void flash_bank_query_wrapper (dev_info_t *dev, ulong bankaddr,
                                      ulong *manu, ulong *device)
{
    ushort chip_code;
    volatile uchar *flashaddr;
    flash_geom_t *fg;
    char *code_buff;
    sys_timestamp time_now, time_prev;
    void (*func)(volatile uchar *, uchar *, uchar *, ushort);
    int fn_size, copy_size;
    leveltype sts;

    /* Chip query. 
     * Try the query that will succeed with most chips we
     * support. If that fails, try the 040 specific query.
     */

    flashaddr = (volatile uchar *)(dev->dev_addr + bankaddr);
    fg = dev->dev_geom;


    /* Check to see if the caller already knows what the chiptype is.
     * On some media, the write protect may be enabled, but the 
     * platform specific code may still know what the chip type is.
     * if this is the case, the fg->fg_chip_code will be non-zero,
     * and it should be used instead of attempting and failing to
     * read the chip type from the flash media.
     */

    if ((dev->devcons->dev_wr_prot) && (fg->fg_chip_code[0])) {
	char *mfg = (char *)manu;
	char *nfo = (char *)device;
	int i;
        for (i = fg->fg_chips ; i ; i--) {
	    *mfg++ = fg->fg_chip_code[0] >> 8;
	    *nfo++ = fg->fg_chip_code[0] & 0xff;
        }
	return;
    }

    /* Kludge !! Warning !! Alert !!!
     * Use of the beesley technique (Tm)
     * We are doing all this because we want to query the flash chips
     * even when we are running out of flash.
     * The six functions above are all using stack variables.
     * (They have intentionally been written thataway so they can 
     * be executed out of anywhere).
     * Therefore what we do is malloc a buffer large enough to hold 
     * any of those functions, then copy the desired function into
     * the buffer and pass control to it. The function will run
     * out of DRAM. Before calling the function, we ensure that we
     * won't get interrupted.
     */
    fn_size = (int)&flash_query_32_bit_oth_end - (int)&flash_query_32_bit_oth;
    code_buff = malloc(fn_size * 4);
    func = (void *)code_buff;

    switch (dev->dev_width) {
    case BW_8BITS: {
        if (dev->dev_state == DEV_STATE_RFF) {
            copy_size = (int)&flash_query_8_bit_oth_end - 
                        (int)&flash_query_8_bit_oth;
            bcopy((char *)&flash_query_8_bit_oth, code_buff, copy_size);
        }
        else
            func = (void *)flash_query_8_bit_oth;
        break;
        }
    case BW_16BITS: {
        if (dev->dev_state == DEV_STATE_RFF) {
            copy_size = (int)&flash_query_16_bit_oth_end - 
                        (int)&flash_query_16_bit_oth;
            bcopy((char *)&flash_query_16_bit_oth, code_buff, copy_size);
        }
        else
            func = (void *)flash_query_16_bit_oth;
        break;
        }
    case BW_32BITS: {
        if (dev->dev_state == DEV_STATE_RFF) {
            copy_size = (int)&flash_query_32_bit_oth_end - 
                        (int)&flash_query_32_bit_oth;
            bcopy((char *)&flash_query_32_bit_oth, code_buff, copy_size);
        }
        else
            func = (void *)flash_query_32_bit_oth;
        break;
        }
    }
    cache_flush();

    /* Disable all interrupts except NMI */
    sts = raise_interrupt_level(ALL_DISABLE); 

    /*
     * Wait out till an NMI has occurred. Then go in and do our
     * query. The query shouldn't take more than some tens of usecs.
     * So we should be long done before the next NMI.
     */
    GET_TIMESTAMP(time_prev);
    GET_TIMESTAMP(time_now);
    while (TIMERS_EQUAL(time_now, time_prev))
        GET_TIMESTAMP(time_now);

    /*
     * Now call the query function.
     */
    switch (dev->dev_width) {
    case BW_8BITS:
        (*func)((volatile uchar *)flashaddr, (uchar *)manu,
                (uchar *)device, fg->fg_usec6);
        break;
    case BW_16BITS:
        (*func)((volatile uchar *)flashaddr, (uchar *)manu,
                (uchar *)device, fg->fg_usec6);
        break;
    case BW_32BITS:
        (*func)((volatile uchar *)flashaddr, (uchar *)manu,
                (uchar *)device, usec6);
        break;
    }

    reset_interrupt_level(sts); /* Restore interrupt level */

    chip_code = ((*(uchar *)manu << 8) | *(uchar *)device);
    if (flash_chip_identify(chip_code) == 0) {
        /* identification failed; try the 040's now */

        switch (dev->dev_width) {
        case BW_8BITS:
            if (dev->dev_state == DEV_STATE_RFF) {
                copy_size = (int)&flash_query_8_bit_040_end - 
                            (int)&flash_query_8_bit_040;
                bcopy(flash_query_8_bit_040, code_buff, copy_size);
            }
            else
                func = (void *)flash_query_8_bit_040;
            break;
        case BW_16BITS:
            if (dev->dev_state == DEV_STATE_RFF) {
                copy_size = (int)&flash_query_16_bit_040_end - 
                            (int)&flash_query_16_bit_040;
                bcopy(flash_query_16_bit_040, code_buff, copy_size);
            }
            else
                func = (void *)flash_query_16_bit_040;
            break;
        case BW_32BITS:
            if (dev->dev_state == DEV_STATE_RFF) {
                copy_size = (int)&flash_query_32_bit_040_end - 
                            (int)&flash_query_32_bit_040;
                bcopy(flash_query_32_bit_040, code_buff, copy_size);
            }
            else
                func = (void *)flash_query_32_bit_040;
            break;
        }
        cache_flush();

        sts = raise_interrupt_level(ALL_DISABLE); /* disable interrupts */

        GET_TIMESTAMP(time_prev);
        GET_TIMESTAMP(time_now);
        while (TIMERS_EQUAL(time_now, time_prev))
            GET_TIMESTAMP(time_now);

        switch (dev->dev_width) {
        case BW_8BITS:
            (*func)((volatile uchar *)flashaddr, (uchar *)manu,
                    (uchar *)device, fg->fg_usec10);
            break;
        case BW_16BITS:
            (*func)((volatile uchar *)flashaddr, (uchar *)manu,
                    (uchar *)device, fg->fg_usec10);
            break;
        case BW_32BITS:
            (*func)((volatile uchar *)flashaddr, (uchar *)manu,
                    (uchar *)device, usec10);
            break;
        }
        reset_interrupt_level(sts);
    }

    free(code_buff);  /* avoid memory leaks */
}


static flash_geom_t *flash_geom_create (dev_info_t *dev)
{
    flash_geom_t *fg;
    int chip;

    /* malloc flash_geom_t structure */
    fg = malloc(sizeof(flash_geom_t));
    if (!fg)    /* malloc failure */
	return (NULL);

    fg->fg_status = FR_NOT_QUERIED;
    fg->fg_chips = dev->dev_width;  /* assume each chip is byte wide */
    fg->fg_chipsize = dev->dev_size / fg->fg_chips;
    fg->fg_banksize = dev->dev_size;
    fg->fg_bank_end = fg->fg_banksize;
    fg->fg_sectors = 1;
    fg->fg_sectorsize = fg->fg_chipsize / fg->fg_sectors;
    fg->fg_cmnd_set = INVALID_SET;  /* unknown */
    fg->fg_width = dev->dev_width;
    fg->fg_chip_width = fg->fg_width / fg->fg_chips;
    fg->fg_wmask = dev->dev_wmask;
    fg->fg_usec6 = usec6;
    fg->fg_usec10 = usec10;
    fg->fg_wtries = WRITE_TRIES;
    fg->fg_etries = ERASE_TRIES;
    fg->fg_start_chip = 0;
    for (chip = 0; chip < MAX_CHIPS_IN_BANK; chip++) {
	fg->fg_tbl_index[chip] = 0;
        fg->fg_write_errors[chip] = 0;
        fg->fg_erase_errors[chip] = 0;
	fg->fg_chip_code[chip] = 0;
	fg->fg_rom_id[chip] = 0;
	fg->fg_chip_id[chip] = "    ";
	fg->fg_chip_name[chip] = UNKNOWN_CHIP_STR;
    }
    fg->fg_bank_no = 0; 
    fg->fg_card = "";
    fg->fg_ctlr = "";
    if (dev->dev_geom == NULL)
	dev->dev_geom = fg;
    return (fg);
}

/***************************************************************************
 * flash_query_roms() : find out which flash chips we have.
 * Read the manufacturer and device codes from each device.
 * Compute the total amount of Flash.
 * This routine must use spin wait delays so that
 * it can be called from the scheduler context.
 */
static ushort flash_query_roms (dev_info_t *dev, ushort dev_type)
{
    flash_geom_t *fg, *prev_geom;
    dev_cons_info_t *devcons;
    ulong addr;
    int b, j;
    boolean flash_ok = TRUE;    /* unless we discover otherwise */
    boolean wr_enab;

    fg = dev->dev_geom;
    prev_geom = NULL;
    devcons = dev->devcons;
    dev->dev_state = DEV_STATE_RDONLY; /* assume the worst */
    dev->dev_banksize = dev->dev_size;
    devcons->dev_banksize = dev->dev_banksize;
    if (devcons->dev_simm_size == 0) /* If default, set to full size */
        devcons->dev_simm_size = devcons->dev_size;

    /* See if running from device - if yes, set to READ_ONLY RFF state */
    if (run_from_flash(dev)) {
        dev->dev_state = DEV_STATE_RFF;
    }
 
    /* Calibrate our timer counts */
    if (devcons->dev_timer_fn)
        (*devcons->dev_timer_fn)(&usec6, &usec10, FALSE);

    if (dev_type == CHIP_TYPE_RAM) { /* If RAM, don't query it */
        /*
         * Malloc a flash_geom_t structure and initialize it.
         * Attach it to the dev_info_t structure.
         */
        if (fg == NULL)
            fg = flash_geom_create(dev);
        if (!fg) {    /* malloc failure */
            dev->dev_state = DEV_STATE_NODEV;
            return (FR_RAM);
        }

        fg->fg_cmnd_set     = ANY_TYPE_OF_RAM;
        fg->fg_card         = "processor board";
        fg->fg_status       = FR_RAM;
        fg->fg_program      = flash_prog_ram;
        fg->fg_fast_program = flash_prog_quad_ram;
        fg->fg_erase        = flash_erase_ram;
        fg->fg_wtries       = 1;
        fg->fg_etries       = 1;
 
        for (b = 0; b < MAX_CHIPS_IN_BANK; b++)
            fg->fg_chip_name[b] = "SRAM";
 
        dev->dev_state = DEV_STATE_RW;
        return (fg->fg_status);
    }

    /*
     * Turn ON Vpp. Some chips require Vpp for querying.
     * (Don't abort if chip id passed in thru dev's geom)
     */
    wr_enab=(*devcons->dev_vpp_fn)(VPP_ON);
    if (!wr_enab) {
       if (fg && !fg->fg_chip_code[0]) {
           (*devcons->dev_vpp_fn)(VPP_OFF);
           return(fg->fg_status = FR_READ_ONLY);
       }
    }
    
    /*
     *  Query each bank for manuf and device codes to identify chips.
     *  If chip(s) not identifiable, quit with error,
     *  If chip(s) identified, set up flash geometry and check for
     *  consistency and compatibility in chips.
     *  A mix of chips with different sizes or programming algorithms
     *  is not supported.
     */
    for (b = 0, addr = 0; (addr < dev->dev_size) && flash_ok; b++) {
 
        int id = 0;
        uchar manu_buf[8];  /* for upto 4 flash chips */
        uchar device_buf[8];

        /* malloc flash_geom_t structure */
        if (fg == NULL)
            fg = flash_geom_create(dev);
        if (!fg) {   /* malloc failure */
            flash_ok = FALSE;
            break;  /* exit loop */
        }
  
        fg->fg_bank_no = b;  /* store bank number */
        fg->fg_status = FR_READ_ONLY;
        flash_bank_query_wrapper(dev, addr,
                                 (ulong *)&manu_buf[0],
                                 (ulong *)&device_buf[0]);
        for (j = 0; j < fg->fg_chips; j++) {
            fg->fg_chip_code[j] = ((manu_buf[j] << 8) | device_buf[j]);
            id = flash_chip_identify(fg->fg_chip_code[j]);
            if (id == 0) {  /* Unknown chips; bail out */
                if (dev->dev_flags & DEV_FLAGS_VERBOSE)
                    printf("\n%%Error: %s bank %d chip %d unknown, "
                           "chip id 0x%4x (reversed = 0x%4x)\n",
                           dev->dev_name, b, j,
                           fg->fg_chip_code[j],
                           flash_chip_id_short_reversal(fg->fg_chip_code[j]));
                flash_ok = FALSE;
            }
            else {
                /*
                 * Leave chip id read in fg_chip_code. This may be bit
                 * reversed. Store normalized code in fg_rom_id. This
                 * matches data sheets.
                 */
                fg->fg_tbl_index[j] = id;
                fg->fg_rom_id[j]    = flash_info_tbl[id].chip_code;
                fg->fg_chip_id[j]   = flash_manu_code(id);
                fg->fg_chip_name[j] = flash_name(id);
            }
        }
        /*
         * Now ensure that all chips in bank are of same type.
         */
        for (j = 1; flash_ok && (j < fg->fg_chips); j++) {
            if (flash_cmnd_set(fg->fg_tbl_index[0]) !=
                flash_cmnd_set(fg->fg_tbl_index[j])) {
                if (dev->dev_flags & DEV_FLAGS_VERBOSE) {
                    printf("\n%%Error: %s bank %d chips mismatch\n",
                           dev->dev_name, b);
                    printf("\n       chip#1 = %s, chip#%d = %s",
                           fg->fg_chip_name[0], j, fg->fg_chip_name[j]);
                }
                fg->fg_status = FR_NOT_VALID;
                dev->dev_state = DEV_STATE_RDONLY;
                flash_ok = FALSE;
            }
        }
 
        /*
         * Configure the geometry if identification OK.
         */
        if (flash_ok) {
            if (wr_enab)
                fg->fg_status       = FR_READ_WRITE;
            fg->fg_sectorsize   = flash_sector_size(id);
            fg->fg_chipsize     = flash_chip_size(id);
            fg->fg_wmask        = dev->dev_wmask;
            fg->fg_banksize     = fg->fg_chipsize * fg->fg_chips;
            fg->fg_sectors      = (fg->fg_sectorsize ? 
                                   fg->fg_chipsize / fg->fg_sectorsize:0);
            fg->fg_cmnd_set     = flash_cmnd_set(id);
            fg->fg_card         = "processor board";
            fg->fg_ctlr         = "";  /* NULL string - no separate cntrlr */
            fg->fg_wtries       = WRITE_TRIES;
            fg->fg_etries       = ERASE_TRIES;
            fg->next_geom       = NULL;
            if (prev_geom != NULL) {
                fg->fg_start_chip = prev_geom->fg_start_chip + prev_geom->fg_chips;
                prev_geom->next_geom = fg;
            }
            prev_geom = fg;

            /*
             * Set write and erase callback functions.
             */
            switch (fg->fg_cmnd_set) {
            case INTEL_010:
                fg->fg_program      = flash_010_prog;
                fg->fg_fast_program = flash_010_prog_quad;
                fg->fg_erase        = flash_010_erase;
                break;
            case INTEL_008:
                fg->fg_program      = flash_008_prog;
                fg->fg_fast_program = flash_008_prog_quad;
                fg->fg_erase        = flash_008_erase;
                break;
            case AMD_040:
                fg->fg_program      = flash_040_prog;
                fg->fg_fast_program = flash_040_prog_quad;
                fg->fg_erase        = flash_erase_040;
                break;
            case INVALID_SET:
            default:
                fg->fg_program      = flash_prog_err;
                fg->fg_fast_program = flash_fast_prog_err;
                fg->fg_erase        = flash_erase_err;
		fg->fg_status       = FR_READ_ONLY;
                flash_ok = FALSE;
                break;
            }

	    addr += fg->fg_banksize; /* bump address to next bank */
	    fg->fg_bank_end = addr;
	    fg = fg->next_geom;
        }
    }
    /*
     * Turn Vpp OFF.
     */
    (*devcons->dev_vpp_fn)(VPP_OFF);
 
    if (!flash_ok) {  /* failed - something not valid */
        if (dev->dev_geom == NULL) {  /* Geometry couldn't be created */
            if (dev->dev_flags & DEV_FLAGS_VERBOSE)
                printf(INIT_FAILED_MSG
                       ", no memory\n", dev->dev_name);
            dev->dev_state = DEV_STATE_NODEV;
            return (FR_NOT_VALID);
        }
        else {  /* Chip query failed or chip mismatch */
            if (dev->dev_flags & DEV_FLAGS_VERBOSE)
                printf(INIT_FAILED_MSG
                        "\n", dev->dev_name);
            return (fg ? fg->fg_status : FR_READ_ONLY);
        }
    }

    /*
     * Go thru the geoms and figure out the largest bank size.
     * Store it in the devcons structure.
     */
    dev->dev_banksize = 0;
    for (fg = (flash_geom_t *)dev->dev_geom; fg; fg = fg->next_geom) {
        if (fg->fg_banksize > dev->dev_banksize)
            dev->dev_banksize = fg->fg_banksize;
    }
    devcons->dev_banksize = dev->dev_banksize;

    /*
     * Chip query was successful. Now look for consistency in banks.
     * We can't support banks with different sizes.
     */

    {
    ulong dev_total, simm_total;

    dev_total = 0;
    fg = (flash_geom_t *)dev->dev_geom;
    while (fg && (dev_total < devcons->dev_size)) {
        prev_geom = fg;
        simm_total = 0;
        while (fg && (simm_total < devcons->dev_simm_size)) {
            simm_total += fg->fg_banksize;
            fg = fg->next_geom;
        }
        if (simm_total != devcons->dev_simm_size) {
            if (dev->dev_flags & DEV_FLAGS_VERBOSE) {
                printf("\n%%Error: %s sizing mismatch", dev->dev_name);
                printf("\n        Size reported = %dKB, calculated = %dKB\n",
                       devcons->dev_simm_size/ONE_K, simm_total/ONE_K);
            }
            for (fg = (flash_geom_t *)dev->dev_geom; fg; fg = fg->next_geom)
                fg->fg_status = FR_NOT_VALID;
            dev->dev_state = DEV_STATE_RDONLY;
            return (FR_READ_ONLY);
        }
        dev_total += simm_total;
    }
    if (dev_total != devcons->dev_size) {
        if (dev->dev_flags & DEV_FLAGS_VERBOSE) {
            printf("\n%%Error: %s sizing mismatch", dev->dev_name);
            printf("\n        Size reported = %dKB, calculated = %dKB\n",
                 devcons->dev_size/ONE_K, dev_total/ONE_K);
        }
        for (fg = (flash_geom_t *)dev->dev_geom; fg; fg = fg->next_geom)
            fg->fg_status = FR_NOT_VALID;
        dev->dev_state = DEV_STATE_RDONLY;
        return (FR_READ_ONLY);
    }
    }
 
    for (fg = (flash_geom_t *)dev->dev_geom; fg; fg = fg->next_geom) {
        if (fg->fg_status != FR_READ_WRITE)
          return (FR_READ_ONLY);
    }
    if (dev->dev_state == DEV_STATE_RFF)
        return(FR_READ_ONLY);
 
    dev->dev_state = DEV_STATE_RW;
    return(FR_READ_WRITE);
}

/**************************************************************************
 *               FLASH driver Initialization for LES
 *
 * les_flash_dvr_init       - creates and initializes flash chip related
 *                            data structure in the dev_info_t structure
 *                            for the flash driver to use.
 *
 * Notes on initialization, flash states:
 *
 * 1. The existence of flash is determined either by sizing, using bus error, *    or through register bits. This is platform dependent stuff and is done
 *    in platform specific flash code.
 *
 * 2. Validity of flash is determined by reading the chip ids, and ensuring
 *    they are consistent.
 *    Invalid means:
 *    a. id not known (unsupported chips)
 *    b. not all the same size and maufacturer.
 *    c. unable to read id's (vpp not enabled on merlot/moosehead).
 *
 * 3. Writable if :
 *     a. vpp can be enabled
 *     b. flash is valid (chips recognized)
 *     c. not run from flash
 *
 * 4. status :
 *      case FR_NOT_VALID:   / * could not read ids, or ids not consistent * /
 *      case FR_READ_ONLY:   / * ids ok, but cannot write flash * /
 *      case FR_READ_WRITE:  / * ids ok, can write flash * /
 *************************************************************************/
void les_flash_dvr_init (void *devinfo)
{
  dev_info_t *dev = (dev_info_t *)devinfo;
  dev_cons_info_t *devcons = dev->devcons;
  ushort type;

    /*
     * Set up the basic call-back functions.
     */
    devcons->dev_read_fn  = (void *)les_flash_read;
    devcons->dev_write_fn = (void *)les_flash_write;
    devcons->dev_erase_fn = (void *)les_flash_erase;
    devcons->dev_info_fn  = (void *)les_flash_info;
    devcons->dev_delay_fn = (void *)flash_delay;
    devcons->dev_geom_relink_fn = (void *)flash_geom_relink;

    dev->dev_state = DEV_STATE_INIT;

    /*
     * Turn CACHE OFF, then initialize timer variables.
     * Then find out what type of chips we have.
     * Turn cache back ON.
     */
    if (devcons->dev_cache_fn)
        (*devcons->dev_cache_fn)(dev, CACHE_OFF);

    /* Find out if we have flash, ROM or RAM*/
    if (dev_chk(dev)) {
	type = flash_probe_memory_type(dev); 
        flash_query_roms(dev,type);        /* find out what chips we have */
    }
    devcons->dev_banksize = dev->dev_banksize;

    /* Initialize chip info for the flash device */
    les_flash_chipinfo_init(dev->dev_geom, devcons->dev_chipinfo);

    if (devcons->dev_cache_fn)
        (*devcons->dev_cache_fn)(dev, CACHE_ON);  /* reenable cache */
}

/*
 * Init function for this subsystem.
 */
static void les_flash_driver_init (subsystype *subsys)
{
/*
 * register function for initialization of driver.
 * This function is multiplexed on the platform and device type.
 */
    reg_add_flash_device_driver_init(DEVICE_LES_FLASH, les_flash_dvr_init,
                                     "les_flash_dvr_init");
}

/*
 * Subsystem header for low-end system Flash low-level driver
 * This susbsystem must be initialized first amongst all Flash 
 * subsystems.
 */

#define LESFLASH_MAJVERSION 1
#define LESFLASH_MINVERSION 0
#define LESFLASH_EDITVERSION  1

SUBSYS_HEADER(les_flash_driver,LESFLASH_MAJVERSION, LESFLASH_MINVERSION, 
              LESFLASH_EDITVERSION, les_flash_driver_init, 
              SUBSYS_CLASS_DRIVER, NULL, NULL);
