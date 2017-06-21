/* $Id: flash_aout.h,v 3.2.60.1 1996/09/11 23:06:43 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_aout.h,v $
 *------------------------------------------------------------------
 * flash_aout.h - Platform specific checks for SUN a.out execs
 *
 * Oct 1994, Eric Fronberg
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_aout.h,v $
 * Revision 3.2.60.1  1996/09/11  23:06:43  snyder
 * CSCdi68880:  more const stuff
 *              120 out of image, 144 data
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:00:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/06  18:12:35  enf
 * CSCdi40422:  Cant netboot router with boothelper image
 *
 * Revision 2.2  1995/09/23  05:57:05  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.1  1995/06/07  20:24:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FLASH_AOUT_H__
#define __FLASH_AOUT_H__


/********************************************************************
 * This file contains code for validating SUN a.out executables.
 * It is included within a platform specific flash file (such as
 * src-68-c3000/c3000_flash.c and src-68-c4000/xx_flash.c).
 *******************************************************************/


#include "../os/reloc.h"	/* get relocatable header info */
#include "image_classify.h" /* get image types */
#include "../os/sum_magic_number.h"
#include "../os/region.h"

/*
 * low end centric thinking here....
 * RXBOOT_DATA_BSS_SIZE is size of bss+data on rxboot 
 * image - 9.14(6) This number will grow with time.
 * For reference 10.X rxboot bss+data today = 274000+ bytes
 * In any event at least 150K or so will be sucked up by the data
 * and bss space in the rxboot image.
 */
#define RXBOOT_DATA_BSS_SIZE (150 * ONE_K)

const char error_prefix[] =
    "\n%Error: ";
const char warn_prefix[] =
    "\n%Warning: ";
const char msg_warn_invalid_exec[] =
    "%sFile not a valid executable for this system";
const char msg_non_recommended_image[] =
    "%sFile not a recommended image for this system";
const char msg_insufficient_DRAM[] =
    "%sSystem DRAM %sinsufficient to run this image";
const char msg_image_corruption[] = 
    "%sRelocatable image corrupted";


/* Does this image run from flash, specifically this file system 
 * in flash ? */
static boolean aout_image_RFF_check (SUN *aout_hdr,
                                     ulong fs_start, ulong fs_size)
{
    if ((aout_hdr->a_entry >= fs_start) &&
        (aout_hdr->a_entry <= (fs_start + fs_size)))
        return (TRUE);
    return (FALSE);
}

/* Can we write this image into flash ?
 * - Is there sufficient space to fit the file in ?
 * Do this only if we can read the file a.out header.
 */
static boolean aout_image_flash_write_checks (ulong *image_ptr,
                                              fsstat_t *fs_info,
                                              ulong verbose,
                                              boolean erase)
{
    SUN *aout_hdr;
    ulong size;

    aout_hdr = (SUN *)image_ptr;
    /*
     * Calculate size of file and see if it will fit into flash.
     */
    size = aout_hdr->a_text + aout_hdr->a_data + sizeof(SUN);
    if (size > (fs_info->size - FLASH_FHDR_SIZE)) {
        /* file exceeds filesys size */
        if (verbose)
            printf("%sImage size exceeds %s", error_prefix, "flash size");
        return (FALSE);
    }
    if (!erase) {  /* Not erasing. See if file will fit */
        if (size > (fs_info->free - FLASH_FHDR_SIZE)) {
            if (verbose)
                printf("%sImage size exceeds %s", error_prefix, "free space");
            return (FALSE);
        }
    }
    return (TRUE);
}

/* Is the RFF image loaded in flash at the correct location
 * for execution ?
 */
static boolean aout_image_RFF_exec_checks (uchar *image_ptr, ulong phy_start)
{
    SUN *aout_hdr;

    /* For run-from-flash, see that image is loaded at the right place */
    aout_hdr = (SUN *)image_ptr;
    if (aout_hdr->a_entry != (phy_start + sizeof(SUN))) {
            return (FALSE);
    }
    return (TRUE);
}

/* Can this RFR image execute ? Do we have sufficient DRAM ? */
static int aout_image_RFR_exec_checks (ulong image_size)
{
    ulong memsize;

    /*
     * If CPU memory size is less than image size signal loud
     * warning. If CPU memory size is less than image size + 250K,
     * signal warning. If CPU memory exceeds image size + 250K,
     * say its fine.
     */
    memsize = region_get_size_by_class(REGION_CLASS_LOCAL);
    memsize -= RXBOOT_DATA_BSS_SIZE;
    if (memsize <= image_size)
        return (-1);  /* error */
    if (memsize <= (image_size + TWO_FIFTY_K))
        return (0);
    return (1);
}

/* 
 * aout_image_check_rel_image()
 * look into the buffer handed to this routine and as constrained by the 
 * size of the buffer, do validation checks on the buffer to determine
 * if it is a valid relocatable image.
 * The following checks are performed (if possible):
 * Special header checksum
 * Image checksum
 * Text checksum
 * This routine is expected to grow as more checks for the relocatable image
 * type are created.  The routine returns TRUE for a good image and False
 * otherwise.  If no checks can be made because of buffersize limitations, 
 * the routine will return TRUE.  This routine is assumed to have 
 * been called with a image that is inface a relocatable image.
 */
 
static boolean aout_image_check_rel_image (void *ptr, ulong bufsize) 
{
    SUN *mptr  = ptr;
    struct special_header *shptr; /* pointer to the start of the rel */
				/* image special header*/
    uchar *bufptr = (uchar *)mptr;
    uint retvalue = 0;

    /* first a sanity check on bufsize */
    if (bufsize <= 0 || bufsize > (16 * ONE_MB)) {
	return(FALSE);
    }
    if (bufsize < (SUN_AOUT_HDR_SIZE + RELOC_HEADER_LEN)) {
	/* 
	 * buffer not large enough to do any valid checks so
	 * just assume alls ok
	 */
	return(TRUE);
    }
    /* 
     * so we have enough space to at least check the relocatable header
     */
    shptr = (struct special_header *)(mptr+1); /* notice the structure */
				             /* arith here */

    /* do a bounds check on r_hdr_cksum_algor */

#define CKSUM_ALGOR_LIMIT 0x10

    if(shptr->r_version < MIN_RELOC_IMAGE_VER) {
	return(FALSE);
    }
    if ((shptr->r_version >> 16) > RELOC_IMAGE_MAJOR_VER) {
	/* 
	 * Old versions cannot support new relocatable image formats that they
	 * can't recognize. 
	 */
	return(FALSE);
    }
    if ((!shptr->r_hdr_cksum_algor) || 
	shptr->r_hdr_cksum_algor > CKSUM_ALGOR_LIMIT) {
	return(FALSE);
    }
    if (shptr->r_hdr_len != RELOC_HEADER_LEN) {
	return(FALSE);
    }
    retvalue = reg_invoke_sum_region(shptr->r_hdr_cksum_algor,(uchar *)shptr,
				     shptr->r_hdr_len,FALSE);
    if (retvalue && retvalue != 0xffffffff) {
	/* bad header checksum return false */
	return(FALSE);
    }
    /* do sanity check on relocatable header */
    /* test image length */
    if ((mptr->a_text + mptr->a_data) != (shptr->r_hdr_len + 
					  shptr->r_pgmsize + 
					  shptr->r_rsize + 
					  shptr->r_tsize +  
					  shptr->r_dsize)) {
	return(FALSE);
    }
    /* test text_offset pointer */
    if (shptr->r_text_offset != (sizeof(SUN) + shptr->r_hdr_len + 
				 shptr->r_pgmsize + shptr->r_rsize)) {
	return(FALSE);
    }
    /* test data_offset offset */
    if (shptr->r_text_offset + shptr->r_tsize != shptr->r_data_offset) {
	return(FALSE);
    }
    /* test data checksum -- test1 */
    if (shptr->r_text_cksum_offset < shptr->r_data_offset) {
	return(FALSE);
    }
    /* test data checksum -- test2 */
    if (shptr->r_text_cksum_offset != (shptr->r_data_offset + 
				       shptr->r_text_cksum_offset_in_data)) {
	return(FALSE);
    }
    /* now see if have enough buffer to verify anything more */
    if (bufsize >= (shptr->r_text_cksum_offset + sizeof(long))) {
	ulong tmptextcksum;
	/* 
	 * we have a buffer that includes the text segment checksum
	 * so let's calculate the checksum and compare to what's held in
	 * the data segment.
	 */
	
	tmptextcksum = *(ulong*)(bufptr + shptr->r_text_cksum_offset);
	if (tmptextcksum != MAGIC_VALUE) { 
	    /* text checksum is valid, let's test it */
	    retvalue = reg_invoke_sum_region(shptr->r_text_cksum_algor,
					     (uchar *)(bufptr + 
						       shptr->r_text_offset),
					     shptr->r_tsize,TRUE);
	    if (retvalue && (retvalue != tmptextcksum)) {
		return(FALSE);
	    }
	}
    } 
    if (bufsize >= (shptr->r_dsize + shptr->r_data_offset)) {
	/* 
	 * We have a complete image so we can checksum the image
	 * from the PIC through the data section
	 */
	retvalue = reg_invoke_sum_region(shptr->r_image_cksum_algor,
					 (uchar *)(bufptr + shptr->r_hdr_len + 
						   SUN_AOUT_HDR_SIZE),
					 shptr->r_dsize + 
					 shptr->r_data_offset - 
					 (SUN_AOUT_HDR_SIZE + 
					  shptr->r_hdr_len),
					 TRUE);
	if (retvalue && (retvalue != shptr->r_image_cksum)) {
	    /* bad header checksum return false */
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * Sniff the first block of the file and try to determine what sort
 * of file it is - 68K executable or not, compressed executable or
 * uncompressed, etc.  If image is relocatable image and we have all of it
 * in the buffer, then do some image verification to make sure it's ok
 */
static ulong *sniff_image_type (uchar *buff, ulong *itype,
                                void *info)
{
    ulong *mptr;
    fsstat_t *fs_info = (fsstat_t *)info;
    mptr = (ulong *)buff;
    switch (*itype) {
    case  UNKNOWN_IMAGE_OR_DATA_FILE:
    default:
        if (*mptr == AOUT_MAGIC) {
            *itype = UNCOMPRESSED_68K_RFR_IMAGE;
            if (fs_info && aout_image_RFF_check((SUN *)mptr,
                                     fs_info->phy_start, fs_info->size))
                *itype =  UNCOMPRESSED_68K_RFF_IMAGE;
	    /* check now for Relocatable image */
	    if (aout_image_reloc_test((SUN *)mptr)) {
		*itype = UNCOMPRESSED_68K_REL_IMAGE;
	    }
	    return (mptr);
        }
        if (*mptr == MOP_MAGIC) {
            mptr = (ulong *)(buff + TFTP_MAXDATA);
            if (*mptr == AOUT_MAGIC) {
                *itype = UNCOMPRESSED_MOP_RFR_IMAGE;
                if (fs_info && aout_image_RFF_check((SUN *)mptr,
                                         fs_info->phy_start, fs_info->size))
                    *itype =  UNCOMPRESSED_MOP_RFF_IMAGE;
                return (mptr);
            }
        }
        if ((*(ushort *)mptr == COMPRESSED_MAGIC_1) ||
            (*(ushort *)mptr == COMPRESSED_MAGIC_2)) {
            *itype = COMPRESSED_68K_IMAGE;
            return (mptr);
        }
        break;
    case  COMPRESSED_68K_IMAGE:
        if (*mptr == AOUT_MAGIC) {
            *itype = COMPRESSED_68K_RFR_IMAGE;
            if (fs_info && aout_image_RFF_check((SUN *)mptr,
                                     fs_info->phy_start, fs_info->size))
                *itype =  COMPRESSED_68K_RFF_IMAGE;
            return (mptr);
        }
        if (*mptr == MOP_MAGIC) {
            mptr = (ulong *)(buff + TFTP_MAXDATA);
            if (*mptr == AOUT_MAGIC) {
                *itype = COMPRESSED_MOP_RFR_IMAGE;
                if (fs_info && aout_image_RFF_check((SUN *)mptr,
                                         fs_info->phy_start, fs_info->size))
                    *itype =  COMPRESSED_MOP_RFF_IMAGE;
                return (mptr);
            }
        }
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
    SUN *aout_hdr;
    ulong *magic_ptr;
    ulong itype, image_size, bss_size;
    int chk_sts;
    struct special_header *shptr;

    itype =  UNKNOWN_IMAGE_OR_DATA_FILE;
    magic_ptr = sniff_image_type(fparms->buff, &itype, (void *)fs_info);
    aout_hdr = (SUN *)magic_ptr;
    image_size = bss_size = 0;

    switch (itype) {
      case UNCOMPRESSED_68K_RFF_IMAGE:
      case UNCOMPRESSED_MOP_RFF_IMAGE:
        /* Will image fit in flash ? */
        if (!aout_image_flash_write_checks(magic_ptr, fs_info, 
                                           fparms->verbose, erase)) {
	    fparms->snmp_sts = SNMP_DEVICE_FULL;
            return (-1);
	}

	if (reg_invoke_image_family_check(fparms->buff)) {
	    if (fparms->prompt) {
                printf(msg_warn_invalid_exec, warn_prefix);
                if (confirm("\nAbort Copy? "))
                    return (-1);
	    }
	}
        /* Will image be correctly downloaded to execute from flash ? */
        if (!aout_image_RFF_exec_checks((uchar *)magic_ptr,
                                        fs_info->phy_start + FLASH_FHDR_SIZE +
                                        (erase ? 0 : fs_info->next))) {
            if (fparms->prompt) {
                printf("%sImage executes in flash.", warn_prefix);
                printf("\n      Load address not = execution address (0x%x)",
                       aout_hdr->a_entry);
                if (!confirm("\nCopy file? "))
                    return (-1);
            }
        }
        break;

      case UNCOMPRESSED_68K_REL_IMAGE:
        /* Will image fit in flash ? */
        if (!aout_image_flash_write_checks(magic_ptr, fs_info, 
                                           fparms->verbose, erase)) {
	    fparms->snmp_sts = SNMP_DEVICE_FULL;
            return (-1);
	}

	/* Do validation check on relocatable image */
	if (!aout_image_check_rel_image((void *)aout_hdr, 
					(ulong) fparms->buflen)) {
	    /* bad so just dump here */
	    if (fparms->verbose) {
		printf(msg_image_corruption, error_prefix);
	    }
	    /*
	     * flh should continue with copy
	     */
	     if (!bootstrap_enable) {
		fparms->snmp_sts = SNMP_FILE_TRANSFER_ERR;
		return(-1);
	    }
	}
	
	/* 
	 * we do not need to check to see if the image is is
	 * properly located in flash here as the download code
	 * will relocate this image for us 
	 */
	
	shptr = (struct special_header *) (aout_hdr + 1); /* struct arith! */

	/* 
	 * check to see if the image is correct for this type box
	 */
	if (reg_invoke_image_family_check(fparms->buff)) {
	    if (fparms->prompt) {
                printf(msg_warn_invalid_exec, warn_prefix);
                if (confirm("\nAbort Copy? "))
                    return (-1);
            }
	}
	break;

      case UNCOMPRESSED_68K_RFR_IMAGE:
      case UNCOMPRESSED_MOP_RFR_IMAGE:
        /* Will image fit in flash ? */
        if (!aout_image_flash_write_checks(magic_ptr, fs_info, 
                                           fparms->verbose, erase)) {
	    fparms->snmp_sts = SNMP_DEVICE_FULL;
            return (-1);
        }
	if (reg_invoke_image_family_check(fparms->buff)) {
	    if (fparms->prompt) {
                printf(msg_warn_invalid_exec, warn_prefix);
                if (confirm("\nAbort Copy? "))
                    return (-1);
            }
	}
        /* drop down */
      case COMPRESSED_68K_RFR_IMAGE:
      case COMPRESSED_MOP_RFR_IMAGE:
        if (!erase && fs_info->fcnt) { /* Not erasing and flash has files */
	    /* give warning if known RFF system */
	    if (fparms->verbose && RFF_recommended_system())
		printf(msg_non_recommended_image, warn_prefix);
	    return (1);  /* This file will not be the first, so OK */
        }

        /*
         * File being downloaded will be the first one in flash.
         * Take extra care.
         */
        /* Will image be able to run from DRAM ? */
        image_size = aout_hdr->a_text + aout_hdr->a_data +
                     sizeof(SUN);
        bss_size   = aout_hdr->a_bss;

        chk_sts = aout_image_RFR_exec_checks(image_size + aout_hdr->a_bss);
        if (chk_sts < 0) {  /* DRAM insufficient */
            if (fparms->prompt) {
                printf(msg_insufficient_DRAM, warn_prefix, "");
		if (!confirm("\nCopy file? ")) {
                    return (-1);
		}
	    }
        }

        /* give warning if known RFF system */
        if (fparms->verbose && RFF_recommended_system())
            printf(msg_non_recommended_image, warn_prefix);
        if (chk_sts == 0) {  /* DRAM may be just sufficient */
            if (fparms->prompt) {
                printf(msg_insufficient_DRAM, warn_prefix, "may be ");
                if (!confirm("\nCopy file? "))
                    return (-1);
            }
        }
        break;

      case COMPRESSED_68K_RFF_IMAGE:
      case COMPRESSED_MOP_RFF_IMAGE:
      case COMPRESSED_68K_IMAGE:
        if (!erase && fs_info->fcnt) { /* Not erasing and flash has files */
            /* give warning if known RFF system */
            if (fparms->verbose && RFF_recommended_system())
                printf(msg_non_recommended_image, warn_prefix);
            return (1);  /* This file will not be the first, so OK */
        }
        /*
         * File being downloaded will be the first one in flash.
         * Take extra care.
         */
        if (fparms->verbose && RFF_recommended_system()) 
            printf(msg_non_recommended_image, warn_prefix);
        if (fparms->prompt) {
            if (!confirm("\nCopy file? "))
                return (-1);
        }
        break;

      case UNKNOWN_IMAGE_OR_DATA_FILE:
      default:
        if (fparms->verbose)
            printf(msg_warn_invalid_exec, warn_prefix);
        if (fparms->prompt) {
            if (!confirm("\nCopy file? "))
                return (-1);
        }
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
    fsstat_t *fs_info = info;
    SUN *aout_hdr;
    ulong *magic_ptr;
    ulong itype, image_size, bss_size;
    int chk_sts;
    struct special_header *shptr;

    itype =  UNKNOWN_IMAGE_OR_DATA_FILE;
    magic_ptr = sniff_image_type(image_ptr, &itype, (void *)fs_info);
    aout_hdr = (SUN *)magic_ptr;
    image_size = bss_size = 0;

    switch (itype) {
      case UNCOMPRESSED_68K_RFF_IMAGE:
      case UNCOMPRESSED_MOP_RFF_IMAGE:
        if (!aout_image_RFF_exec_checks((uchar *)magic_ptr, (ulong)image_ptr))
            return (TRUE);
	if (reg_invoke_image_family_check((char*)magic_ptr)) {
	    printf(msg_warn_invalid_exec, error_prefix);
	}
        break;

      case UNCOMPRESSED_68K_RFR_IMAGE:
      case UNCOMPRESSED_MOP_RFR_IMAGE:
      case COMPRESSED_68K_RFR_IMAGE:
      case COMPRESSED_MOP_RFR_IMAGE:
        /* Will image be able to run from DRAM ? */
        image_size =  aout_hdr->a_text + aout_hdr->a_data + sizeof(SUN);
        chk_sts = aout_image_RFR_exec_checks(image_size + aout_hdr->a_bss);
        if (chk_sts < 0) /* DRAM insufficient */
            return (TRUE);
	if (reg_invoke_image_family_check(image_ptr)) {
	    printf(msg_warn_invalid_exec, error_prefix);
	}
        break;

      case UNCOMPRESSED_68K_REL_IMAGE:
	/* 
	 * Don't need to  validate relocatable image. This was done at
	 * download time.  If the image has been corrupted in flash then
	 * the flash checksum will fail and the image won't be loaded.
	 */

	shptr = (struct special_header *) (aout_hdr + 1); /* struct arith! */
	/* 
	 * check to see if the image is correct for this type box
	 */
	if (cpu_family != shptr->r_platform_family) {
	    return (TRUE);
	}
        if (!aout_image_RFF_exec_checks((uchar *)magic_ptr, 
					(ulong)image_ptr)) {
	    /* 
	     * image not relocated so we'll see if we can  run from ram 
	     */
	    image_size =  aout_hdr->a_text + aout_hdr->a_data + sizeof(SUN);
	    chk_sts = aout_image_RFR_exec_checks(image_size + aout_hdr->a_bss);
	    if (chk_sts < 0) /* DRAM insufficient  to load image */
		return (TRUE);
	}

	/* eventually put checks for run from ram option here */
	break;

      case COMPRESSED_68K_IMAGE:
        /* Normal compression on our 68K images is 34 to 35%.
         * We will therefore consider 33% as a cut-off point. This
         * is conservative since we are not considering bss size.
         */
        image_size = image_len + image_len/3;  /* Assume 33% compression */
        chk_sts = aout_image_RFR_exec_checks(image_size);
        if (chk_sts < 0)  /* DRAM insufficient for uncompressed image */
            return (TRUE);
        break;

      case COMPRESSED_68K_RFF_IMAGE:
      case COMPRESSED_MOP_RFF_IMAGE:
      case UNKNOWN_IMAGE_OR_DATA_FILE:
      default:
        return (TRUE);
    }
    return (FALSE);
}
#endif  /* __FLASH_AOUT_H__ */
