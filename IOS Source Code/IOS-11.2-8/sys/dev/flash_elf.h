/* $Id: flash_elf.h,v 3.2 1995/11/17 09:00:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dev/flash_elf.h,v $
 *------------------------------------------------------------------
 * flash_elf.h - Platform specific checks for ELF execs
 *
 * Aug 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: flash_elf.h,v $
 * Revision 3.2  1995/11/17  09:00:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/06  18:12:36  enf
 * CSCdi40422:  Cant netboot router with boothelper image
 *
 * Revision 2.1  1995/06/07  20:24:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FLASH_ELF_H__
#define __FLASH_ELF_H__

/********************************************************************
 * This file contains code for validating Elf executables.
 * It is included within a platform specific flash file (such as
 * src-4k-c4000/sierra_flash.c.
 *******************************************************************/

#include "../os/region.h"

/*
 * defines used for image validations, either during download to
 * flash, or while booting from flash.
 */

#define ELF char

#define UNKNOWN_IMAGE_OR_DATA_FILE 0x00
#define UNCOMPRESSED_68K_RFR_IMAGE 0x01
#define UNCOMPRESSED_68K_RFF_IMAGE 0x02
#define UNCOMPRESSED_MOP_RFR_IMAGE 0x03
#define UNCOMPRESSED_MOP_RFF_IMAGE 0x04
#define COMPRESSED_68K_IMAGE       0x05
#define COMPRESSED_68K_RFR_IMAGE   0x06
#define COMPRESSED_68K_RFF_IMAGE   0x07
#define COMPRESSED_MOP_RFR_IMAGE   0x08
#define COMPRESSED_MOP_RFF_IMAGE   0x09

char *msg_warn_invalid_exec =
    "\nWARN: File not a valid executable for this system";
char *msg_insufficient_DRAM =
    "\n%s: System DRAM %sinsufficient to run this image";


/* Does this image run from flash, specifically this file system 
 * in flash ? */
static inline boolean elf_image_RFF_check (ELF *elf_hdr,
					   ulong fs_start, ulong fs_size)
{
    return (FALSE);
}

/* Can we write this image into flash ?
 * - Is there sufficient space to fit the file in ?
 * Do this only if we can read the file a.out header.
 */
static inline boolean elf_image_flash_write_checks (ulong *image_ptr,
						    fsstat_t *fs_info,
						    ulong verbose,
						    boolean erase)
{
    return (TRUE);
}

/* Is the RFF image loaded in flash at the correct location
 * for execution ?
 */
static boolean elf_image_RFF_exec_checks (uchar *image_ptr, ulong phy_start)
{
    return (FALSE);
}

/* Can this RFR image execute ? Do we have sufficient DRAM ? */
static inline int elf_image_RFR_exec_checks (ulong image_size)
{
    ulong memsize;

    /*
     * If CPU memory size is less than image size signal loud
     * warning. If CPU memory size is less than image size + 250K,
     * signal warning. If CPU memory exceeds image size + 250K,
     * say its fine.
     */
    memsize = region_get_size_by_class(REGION_CLASS_LOCAL);
    if (memsize <= image_size)
        return (-1);  /* error */
    if (memsize <= (image_size + TWO_FIFTY_K))
        return (0);
    return (1);
}

/*
 * Sniff the first block of the file and try to determine what sort
 * of file it is - MIPS executable or not, compressed executable or
 * uncompressed, etc.
 */
static ulong *sniff_image_type (uchar *buff, ulong *itype,
                                fsstat_t *fs_info)
{
    ulong *mptr;

    mptr = (ulong *)buff;
    switch (*itype) {
    case  UNKNOWN_IMAGE_OR_DATA_FILE:
    default:
        break;
    }
    *itype = UNKNOWN_IMAGE_OR_DATA_FILE;
    return ((ulong *)buff);
}

/*
 * Platform specific code that checks an image being downloaded into
 * flash and says yay or nay.
 */
static int flash_download_verify (void *parms, void *info, boolean erase)
{
    flash_copy_parms_t *fparms = parms;
    fsstat_t *fs_info = info;
    ELF *elf_hdr;
    ulong *magic_ptr;
    ulong itype;

    itype =  UNKNOWN_IMAGE_OR_DATA_FILE;
    magic_ptr = sniff_image_type(fparms->buff, &itype, fs_info);
    elf_hdr = (ELF *)magic_ptr;

    switch (itype) {
    case UNKNOWN_IMAGE_OR_DATA_FILE:
    default:
        break;
    }
    return (1);
}

/* Try to do some sanity checks before we execute a file.
 * Called when booting a file from flash.
 * Return FALSE (0) with good image and 
 * TRUE (1) with invalid image.
 */
static boolean flash_boot_verify (uchar *image_ptr, ulong image_len, void *info)
{
    return (FALSE);
}

#endif  /* __FLASH_ELF_H__ */
