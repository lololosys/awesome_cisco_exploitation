/* $Id: as5200_xmodem.c,v 1.1.14.1 1996/06/16 21:18:43 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src-68-as5200/as5200_xmodem.c,v $
 *------------------------------------------------------------------
 * AS5200_xmodem.c - AS5200 xmodem platform specific code
 *
 * May 1996, Padmaja Gogineni
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: as5200_xmodem.c,v $
 * Revision 1.1.14.1  1996/06/16  21:18:43  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1996/05/10  17:48:56  pgoginen
 * Placeholder for new development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "types.h"
#include "xmodem_generic.h"
#include "flash.h"
#include "../src/fsio.h"
#include "flash_fs_les.h"
#include "pcmap.h"
#include "mon_boot.h"
#include "as5200_pcmap.h"

extern int flashcopy;

ulong xmodem_flash_size ()
{
    return (bflash_siz());
}

ulong xmodem_freemem ()
{
    return ((sizemainmem() + ADRSPC_RAM) - MONITOR_FREE_MEM_BASE);
}

uchar *xmodem_download_start_address ()
{
    return ((uchar *) (MONITOR_FREE_MEM_BASE + FLASH_FHDR_SIZE));
}

int xmodem_flash_erase ()
{
     return (erase_flash_array(ADRSPC_BOOT_FLASH_BASE));
}

int xmodem_load_flash (ulong source, int size) 
{
     return (copy_to_flash(source, ADRSPC_BOOT_FLASH_BASE, size));
}

int xmodem_chk_image_size (long file_size) 
{
     /*
      * Peform DRAM and flash checks
      */
    if (file_size > xmodem_freemem()) {
        error_msg = INSUFFICIENT_DRAM;
        return (-1);
    }

    if (flashcopy && (file_size > xmodem_flash_size())) {
        error_msg = INSUFFICIENT_FLASH;
        return (-1);
    }

    return (1);
}

/*
 * Validates an image and returns it's size
 * retcode = -1		Image is bad
 * retcode = 0		Need bigger block of data to analyze image
 * retcode = 1		Image is cool
 */
int xmodem_valid_image (uchar *header_ptr, long *file_size,
			long block_size)
{
    ulong *fmagic;
    struct cisco_exec *aout_hdr;
    long flash_file_size;

    *file_size = 0;
    if (block_size == 0) {
	error_msg = INVALID_IMAGE;
        return (-1);
    }

    fmagic = (ulong *) header_ptr;

    if (*fmagic == AOUT_MAGIC) {
      aout_hdr = (struct cisco_exec *) header_ptr;
      *file_size = aout_hdr->a_text + aout_hdr->a_data +
	           CISCOEXEC_HDRSIZ;
      flash_file_size = *file_size + FLASH_FHDR_SIZE;
      return (xmodem_chk_image_size(flash_file_size));
    }
    else if (*fmagic == COMPRESSMAGIC) {
      if (ymodem_filesize) {
	*file_size = ymodem_filesize;
	flash_file_size = *file_size + FLASH_FHDR_SIZE;
	if ((xmodem_chk_image_size(flash_file_size) < 0) ||
	    (flash_file_size > (xmodem_freemem() - UNCOMPRESS_DATASIZE))) {
	  error_msg = INSUFFICIENT_DRAM;
	  return (-1);
	}
      }
      return (1);
    }
    else {
	error_msg = INVALID_IMAGE;
	return (-1);
    }
}


