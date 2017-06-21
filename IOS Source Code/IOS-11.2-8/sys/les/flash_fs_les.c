/* $Id: flash_fs_les.c,v 3.3.48.3 1996/08/07 09:01:14 snyder Exp $
 * $Source: /release/112/cvs/Xsys/les/flash_fs_les.c,v $
 *------------------------------------------------------------------
 * flash_fs_les.c - Flash file system for low end systems.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: flash_fs_les.c,v $
 * Revision 3.3.48.3  1996/08/07  09:01:14  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.48.2  1996/06/28  23:23:34  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.48.1  1996/03/18  20:40:40  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.2  1996/03/07  10:05:10  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.16.1  1996/01/24  22:22:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/07  22:30:17  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  17:37:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:13:48  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/28  09:28:44  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/09/24  19:16:08  vnguyen
 * CSCdi37804:  flash simms based on different chips can not be mixed
 * Changes made to support the mix of simms made from different chips.
 *
 * Revision 2.2  1995/06/28  09:28:44  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:31:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "../dev/flash_defines.h"
#include "../dev/dev_io.h"
#include "../dev/file_io.h"
#include "flash_fs_les.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "subsys.h"
#include "../dev/sr_flashmibdefs.h"

static char *const fs_states[] = {
     "No FS",
     "FS not initialized",
     "FS not programmable",
     "Read ONLY",
     "Read/Write",
};

static char *const fs_copy_states[] = {
     "RXBOOT-FLH ",
     "RXBOOT-manual",
     "Direct",
     "None",
};
/***********************************************************************
 *  Flash File system routines for Low-end systems.
 *  All functions in this file are local functions and never
 *  called directly from outside. Some are registered as callbacks
 *  and called indirectly from the generic file-io and device-io modules.
 ***********************************************************************/

static inline ulong les_flash_fs_long_align (ulong addr)
{
    /* align to long word boundary */
    return ((addr + FLASH_ALIGN_FACTOR) & ~FLASH_ALIGN_FACTOR);
}

static ulong les_flash_fs_file_header_read (int dh, flash_fhdr_t *f_fhdr,
                                            int offset)
{
    dev_seek(dh, DEV_START, offset);
    return (dev_read(dh, (uchar *)f_fhdr, sizeof(flash_fhdr_t)));
}

/*
 * Function needed to convert the copy mode value to a value that is
 * specified by the flash MIB. If the enumerated values for the copy
 * mode in the flash MIB change, or if the possible values for the
 * copy mode for the device change, then this function will have to be
 * updated to reflect the correct mapping between the copy mode values
 * for the device and those specified by the flash MIB.
 */
static long les_flash_fs_map_copy_mode (long mode)
{
    switch(mode) {
    case FLASH_FS_COPY_FLH: /* FLH copy mode state */
        mode = FS_COPY_FLH; /* Equivalent state value for snmp */
        break;
    case FLASH_FS_COPY_DIRECT: /* Direct copy mode state */
        mode = FS_COPY_DIRECT; /* Equivalent state value for snmp */
        break;
    case FLASH_FS_COPY_MANUAL: /* Manual copy mode state */
    case FLASH_FS_COPY_NONE:   /* Copy mode not-known state */
    default:
        mode = FS_COPY_UNKNOWN; /* Equivalent state value for snmp */
        break;
    }
    return(mode);
}

/*
 * Search for a file in the file system.
 */
static int les_flash_fs_file_search (flash_t *fsp, file_open_instance_t *fp)
{
    flash_fhdr_t *fhptr;
    const char *flnm;
    int i, match_file;

    flnm = fp->fname;
    if (!flnm || (strlen(flnm) == 0))  /* No file name. Give up */
        return (-1);

    match_file = -1;  /* no file matched */
    if (fsp) {
        if (fsp->f_filecnt == 0)  /* No files in file system */
            return (-1);

        /*
         * Search file system for undeleted file with matching name.
         * Don't be case sensitive.
         * If checksum is to be verified, do so for undeleted files
         * with matching name.
         */
        fhptr = (flash_fhdr_t *)&fsp->f_fhdrs[0];
        for (i = 0; i < fsp->f_filecnt; i++, fhptr++) {
            if (!(fhptr->ffh_flags & FFH_FLAGS_DUMMY)) /* A black hole */
                continue;

            if (strcasecmp(flnm, &fhptr->ffh_name[0]))
                continue;               /* Name does not match */

            if (!(fhptr->ffh_flags & FFH_FLAGS_DELETED)) { /* Deleted */
                if (!(fp->flags & FILE_FLAGS_NO_DEL_CHK))
                    continue;
            }

            /*
             * If checksum is to be checked, ignore files with
             * bad checksum.
             */
            if ((fp->flags & FILE_FLAGS_CHECKSUM) &&
                (fsp->f_cksum[i] != fhptr->ffh_checksum))
                    continue;

            /*
             * If match not recorded, record it. Don't record a match
             * if one's already been recorded.
             */
            if (match_file < 0)
                match_file = i;

            return(i);
        }  /* end for */

        if (match_file >= 0)  /* a match was found */
            return (match_file);

    }  /* end if(fsp) */
    return (-1);
}

/*
 * Add a file to the file system directory.
 * Return index of file in directory; -1 on error.
 */
static int les_flash_fs_file_add (flash_t *fsp, const char *flnm)
{
    flash_fhdr_t *fhptr;
    int i;
    ulong data_start;

    /*
     * See if file system full. If yes, then return invalid
     * file index.
     */
    i = fsp->f_filecnt;
    if (i >= FLASH_FILE_CNT) { /* file system full */
        fsp->f_erase_needed = TRUE;
        return (-1);
    }

    /*
     * Check if enough space left on device to atleast create
     * file header and add a little bit of data.
     */
    data_start = fsp->f_free_offset + sizeof(flash_fhdr_t);
    if ((data_start + FLASH_MIN_FREE_SPACE) >= fsp->fs_size) { /* device full */
        fsp->f_erase_needed = TRUE;
        return (-1);
    }

    fhptr = (flash_fhdr_t *)&fsp->f_fhdrs[i];
    /*
     * Now initialize the header fields
     */
    fhptr->ffh_magic    = FLASH_FHDR_MAGIC;
    fhptr->ffh_len      = 0;
    fhptr->ffh_checksum = 0;
    fhptr->ffh_flags    = (short) -1;
    fhptr->ffh_addr     = (uchar *)fsp->f_free_offset;
    strcpy(&fhptr->ffh_name[0],flnm);

    fsp->f_cksum[i]     = 0;
    fsp->f_addr[i]      = (uchar *)data_start;
    fsp->f_filecnt++;
    return(i);
}

/*
 * Delete a file in the file system.
 * This consists of setting a bit in the flags field of the file
 * header to 0. We cannot physically remove the file and reclaim
 * any space without erasing the device.
 */
static int les_flash_fs_file_delete (flash_t *fsp, int fno)
{
    flash_fhdr_t *fhptr;

    /*
     * First see if we have a valid file index. If not, bail out.
     */
    if ((fno >= fsp->f_filecnt) || (fno < 0))
        return (0);

    /*
     * Reset the flags bit and return a value so that the 'close'
     * function knows that the header needs to be updated.
     */
    fhptr = (flash_fhdr_t *)&fsp->f_fhdrs[fno];
    fhptr->ffh_flags &= ~FFH_FLAGS_DELETED;
    return (FILE_UPDATE_DELFLAG);
}

/* 
 * This function is used to skip a file system when it is found in flash.
 * It also checks to see if any files in the file system end at bank
 * boundaries. If they do, it means that we could potentially allow partition
 * boundaries to fall at that bank even though ot falls within the span of
 * a file system. 
 * The function returns the offset of the address just past the end of
 * this file system.
 */
static ulong les_flash_fs_skip (int dh, flash_fhdr_t *fhptr, int offset,
                                 fs_bounds_t *fsbounds, ulong banksize)
{
    int i, len, indx, excp_bank;
    for (i=0; i < FLASH_FILE_CNT; i++) {
        offset += (fhptr->ffh_len + sizeof(flash_fhdr_t));
        offset = les_flash_fs_long_align(offset);
        
        /* Determine if this file ended on a bank boundary */
        if (offset % banksize == 0) { /* yes, it does end on a bank boundary */
            /* Add to the list of exception banks */
            excp_bank = offset / banksize;
            indx = 0;
            while (fsbounds->exception_bank[indx] != -1)
                indx++;
            fsbounds->exception_bank[indx] = excp_bank;
        }
        
        len = les_flash_fs_file_header_read(dh, fhptr, offset);
        if (len != sizeof(flash_fhdr_t))
            return(offset);
        if (fhptr->ffh_magic != FLASH_FHDR_MAGIC)
            return(offset);
    }
    return(-1); /* Needs more thought */
}

/*
 * les_fs_cksum_add_short(new_word, old_cksum);
 * Accumulates a checksum by adding the new word the old checksum, with
 * end-around carry.
 * Returns the new checksum value.
 */
static ushort les_flash_fs_cksum_add_short (ushort new_word, ushort old_cksum)
{
    ulong work;                       /* work value for end-around add */
    work = old_cksum + new_word;      /* add new word to checksum */
    if (work & ~0xFFFF)
        work++;                       /* end-around carry */
    return(work & 0xFFFF);
}
/*  calculates checksum for memory buffer.  Optimized by performing
 *  checksum here as well as les_flash_fs_cksum_add_short.
 */

static ushort les_flash_fs_checksum (ushort cksum, char *buf, ulong len)
{
    ulong work;
    ushort *p;
    
    if ( (ulong)buf & 0x1 ) {
        cksum = les_flash_fs_cksum_add_short(*buf++, cksum);
        len--;
    }

    work=cksum;
    p = (ushort*)buf;
    
    for ( ; len > 1; len -= sizeof(ushort)) {
        work += *p++;
    }
    cksum = work & 0xffff;
    cksum = les_flash_fs_cksum_add_short((work >> 16), cksum);

    if (len == 1)  {
        buf = (char*)p;
        cksum = les_flash_fs_cksum_add_short(*buf << 8, cksum);
    }
    return(cksum);
}

static ushort les_flash_fs_checksum_wrapper (int dh, flash_t *ffs, int fidx,
                                             ulong len, boolean delay_flag)
{
    ulong delay = 0;
    ushort cksum = 0;
    ulong  len_rd=0;
    
    dev_seek(dh, DEV_START, (int)ffs->f_addr[fidx]);
    while (len >= FS_MALLOC_BLOCK) { /* read in 2K blocks */
        len_rd =dev_read(dh, (uchar *)ffs->mem_block, FS_MALLOC_BLOCK);
        if (len_rd != FS_MALLOC_BLOCK) {
            break;
        }
        cksum = les_flash_fs_checksum(cksum, ffs->mem_block, FS_MALLOC_BLOCK);
        len -= FS_MALLOC_BLOCK;
        if (delay_flag) {
            if ((++delay % 64) == 0)  /* dismiss every  128K */
                dev_delay(dh, 10);
        }
    }
    if (len) {
        len_rd =dev_read(dh, (uchar *)ffs->mem_block, len);
        if (len_rd == len) {
            cksum = les_flash_fs_checksum(cksum, ffs->mem_block, len);
        }
    }
    return (cksum);
}

/*
 * Go through the file system, building up the directory.
 * Return after the last file or on error. Return number of
 * files seen.
 */
static int les_flash_fs_read (int dh, flash_t *ffs, flash_fhdr_t *fhptr,
                              boolean cksum_flag)
{
    int i;
    ulong len, fsz;

    fsz = ffs->fs_size;  /* file system size */

    for (i = ffs->f_filecnt; i < FLASH_FILE_CNT;) {
        /*
         * Is the length field ok ?
         */
        if (fhptr->ffh_len > fsz) { /* hosed length ? */
            ffs->f_erase_needed = TRUE;
            return (i);
        }
        /*
         * See if file length exceeds free space available.
         */
        if (fhptr->ffh_len > (fsz - ffs->f_free_offset - sizeof(flash_fhdr_t)))
{
            ffs->f_erase_needed = TRUE;
            return (i);
        }

        /*
         * Store file header start, file data start, and checksum.
         */
        fhptr->ffh_addr = (uchar *)ffs->f_free_offset;
        ffs->f_addr[i]  = (uchar *)ffs->f_free_offset + sizeof(flash_fhdr_t);
        /*
         * Generate the checksum for the file if requested via the
         * cksum_flag boolean.
         */
        if (cksum_flag)
            ffs->f_cksum[i] = ~les_flash_fs_checksum_wrapper(dh, ffs, i,
                                                         fhptr->ffh_len, TRUE);

        /*
         * Increment next free offset by size of this file's header
         * and data length.
         */
        ffs->f_free_offset += (fhptr->ffh_len + sizeof(flash_fhdr_t));

        /*
         * Long-word align offset and increment file count.
         * If next file offset goes beyond device after long-word
         * alignment, adjust the offset, note that erasure is needed
         * to add another file, and return.
         */
        ffs->f_free_offset = les_flash_fs_long_align(ffs->f_free_offset);

        ffs->f_erase_needed = FALSE;
        if (ffs->f_free_offset > fsz) {
            ffs->f_free_offset  = fsz;
            ffs->f_erase_needed = TRUE;
            return (i);
        }

        /*
         * Everything under control.
         * Read next file header portion and see if file exists.
         */
        fhptr++;  /* On to next file entry */
        i++;      /* bump up file count */
        len = les_flash_fs_file_header_read(dh, fhptr, ffs->f_free_offset);
        if (len != sizeof(flash_fhdr_t))
            return (i);

        if (fhptr->ffh_magic != FLASH_FHDR_MAGIC)
            return (i);
    }
    return (i);
}

static int les_flash_fs_erase_query (file_open_instance_t *fp, flash_t *fsp,
                                     boolean *erase, int alt_fno)
{
    *erase = FALSE;  /* Assume we don't need to erase */

    if (fp->flags & FILE_FLAGS_PROMPT) {
        if (fsp->f_erase_needed)  /* Device needs erasure */
            printf("\nDevice needs erasure before copying new file");
        if (confirm("\nErase flash device before writing? ")) {
            if (fsp->f_filecnt) {  /* Reconfirm if Flash has files */
                if (confirm("\nFlash contains files. Are you sure you want to erase? "))
                    *erase = TRUE;
            }
            else
                *erase = TRUE;
        }
    }
    else {
        if (fsp->f_filecnt == 0)  /* Flash is empty, so erase it */
            *erase = TRUE;
    }
    if (*erase)
        return (FOPEN_POSITIVE);
    else {  /* don't erase */
        if (fsp->f_erase_needed) { /* erase needed; so bail out */
            if (fp->flags & FILE_FLAGS_VERBOSE)
                printf("\nCopy aborted - device needs erasure");
            return (FOPEN_NEED_ERASE);
        }
    }

    if (fsp->f_filecnt >= FLASH_FILE_CNT)  {  /* file system full */
        /*
         * If file system is full, nothing needs to be done to any
         * duplicate file. Either the flash will be erased, in
         * which case the file will be gone, or else the copy
         * will be aborted.
         */
        if (fp->flags & FILE_FLAGS_PROMPT) {
            printf("\nFlash File system full. Erasure needed");
            if (confirm("\nErase flash device before writing? ")) {
                if (confirm("\nFlash contains files. Are you sure you want to erase? ")) {
                    *erase = TRUE;
                    return (FOPEN_POSITIVE);
                }
            }
            printf("\nCopy aborted - erase NOT confirmed");
            return (FOPEN_USRABORT);
        }
        else {
            if (fp->flags & FILE_FLAGS_VERBOSE)
                printf("\nCopy aborted - file system full");
            return (FOPEN_NEED_ERASE);
        }
    }   /* if file system full */

    if (alt_fno >= 0) { /* file with same name already exists */
        if (fp->flags & FILE_FLAGS_PROMPT) {
            printf("\nFile '%s' already exists; it will be invalidated!",
                   fp->fname);
            if (alt_fno == 0) /* First file duplicated */
                printf("\nWARN: '%s' is the FIRST file in Flash", fp->fname);
            printf("\nInvalidate existing copy of '%s'? ", fp->fname);
            if (!confirm(NULL)) {
                printf("\nCopy aborted - user request");
                return (FOPEN_USRABORT);
            }
        }
    }
    return (FOPEN_POSITIVE);
}

/*********************************************************************
 *           Functions called by generic file i-o functions
 ********************************************************************/
static void les_flash_fs_init (int dh, ulong flags)
{
    ulong elen;
    flash_t *fsp;

    fsp = dev_get_fs_spec_info(dh);
    if (fsp) {
        if (fsp->fs_status != DEV_STATE_RW) {
            if (flags & FILE_FLAGS_VERBOSE) {
                printf("\nERR: Flash cannot be erased");
                flush();
            }
            return;
        }
        if (flags & FILE_FLAGS_VERBOSE) {
            printf("\nErasing device... ");
            flush();
        }
        /*
         * Position to start of file system, then erase.
         * If erase done, initialize file system info.
         * Then seek to start of file system.
         */
        dev_seek(dh, DEV_START, fsp->fs_start);
        elen = dev_erase(dh, 0, fsp->fs_size);
        if (elen) { /* something was erased */
            fsp->f_filecnt = 0;
            fsp->f_free_offset = 0;
            fsp->f_erase_needed = (elen == fsp->fs_size) ? FALSE : TRUE;
            if (flags & FILE_FLAGS_VERBOSE)
                printf(" ...%serased", fsp->f_erase_needed ? "NOT ":"");
        }
        else
            if (flags & FILE_FLAGS_VERBOSE)
                printf(" ...%serased", "NOT ");

        if (flags & FILE_FLAGS_VERBOSE)
            flush();

        /*
         * Now seek to start of file system.
         */
        dev_seek(dh, DEV_START, fsp->fs_start);
    }
}

/* Open a file in the file system */
static int les_flash_fs_fopen (int dh, file_open_instance_t *fp)
{
    int fno, alt = -1;
    int updt = 0;
    flash_t *fsp;
    fs_open_instance_t *fsoi;
    boolean erase;

    /*
     * validate file name length
     */
    if (strlen(fp->fname) > (FLASH_NAME_LEN - 1))
        return (FOPEN_FNM_ERR);

    /*
     * See if device has specified file. Depending on the file
     * open mode, decide what action to take.
     * If we are opening in RD or DEL mode, file must be present.
     * If we are opening in WR mode, file with same name may or
     * may not be present.
     * Now, get pointer to the file system info.
     */
    fsp = dev_get_fs_spec_info(dh);
    if (!fsp)
        return (FOPEN_NOFILE);

    /*
     * Search file system directory for file. If not found, return
     * value will be < 0.
     */
    fno = les_flash_fs_file_search(fsp, fp);

    switch (fp->flags & FILE_FLAGS_OPENMASK) { /* switch on open mode */

    case FILE_FLAGS_RDONLY:
        if (fno < 0) {  /* file doesn't exist; report error */
            return (FOPEN_NOFILE);
        }
        if (fp->flags & FILE_FLAGS_CHECKSUM) { /* See if checksum ok */
            if (fp->flags & FILE_FLAGS_VERBOSE) {
                printf("\nVerifying checksum for '%s' (file # %d)... ",
                        fp->fname, fno+1);
                flush();
            }
            /*
             * Validate file length. Don't calculate checksum if length
             * takes file beyond file system end.
             */
            if (((ulong)fsp->f_addr[fno] + fsp->f_fhdrs[fno].ffh_len) > fsp->fs_size)
                fsp->f_cksum[fno] = ~fsp->f_fhdrs[fno].ffh_checksum;
            else
                fsp->f_cksum[fno] = ~les_flash_fs_checksum_wrapper(dh, fsp, fno,
                                         fsp->f_fhdrs[fno].ffh_len, TRUE);

            if (fsp->f_cksum[fno] != fsp->f_fhdrs[fno].ffh_checksum) {
                if (fp->flags & FILE_FLAGS_VERBOSE)
                    printf(" incorrect");
                return (FOPEN_CKSUM_ERR);
            }
            if (fp->flags & FILE_FLAGS_VERBOSE)
                printf(" OK");
        }
        break;

    case FILE_FLAGS_RW:
    case (FILE_FLAGS_RW | FILE_FLAGS_ERASE):

        /* When opening a file in r-w mode, if erase has been explicitly
         * specified, leave the actual erase and addition of new file to
         * the reopen.
         * If an explicit erase has not been specified, query the user
         * depending on the prompt flag. If no prompting is to be done,
         * take an intelligent decision. If user specifies erase, set
         * a bit in the flags field and leave the actual erase and
         * file addition to the reopen.
         * All this has been done to delink the user queries from the
         * actual erase and file addition. This allows us to do some file
         * validations w.r.t. the flash state prior to erasing flash but
         * after getting all user options.
         */
        if (!(fp->flags & FILE_FLAGS_REOPEN)) { /* First time open */
            if (fp->flags & FILE_FLAGS_ERASE) {   /* explicit erase */
                fp->flags |= FILE_FLAGS_REOPEN_ERASE; /* a reminder */
                return (FOPEN_OK);  /* return for now. Let reopen handle rest */
            }
            else {
                int query_sts;

                query_sts = les_flash_fs_erase_query(fp, fsp, &erase, fno);
                if (query_sts < 0)  /* Error */
                    return (query_sts);
                if (erase) {
                    fp->flags |= FILE_FLAGS_REOPEN_ERASE; /* a reminder */
                    return (FOPEN_OK);  /* return for now. Let reopen handle rest */
                }
            }
        }
        if (fp->fs_info) /* file already opened */
            return (FOPEN_OK);

        if (fp->flags & FILE_FLAGS_REOPEN_ERASE) { /* reopening for erase */
            /*
             * Purge file system without any prompting.
             */
            les_flash_fs_init(dh, fp->flags);
            /*
             * Was file system really fully erased ?  Bail out if not.
             */
            if (fsp->f_erase_needed) {  /* file system was not erased */
                if (fp->flags & FILE_FLAGS_VERBOSE)
                    printf("\nCopy aborted - erase failed");
                return (FOPEN_NEED_ERASE);
            }
            fno = -1; /* since we've erased, there's no duplicate */
        }
        if (fno >= 0)  /* duplicate file */
            alt = fno; /* save duplicate file index */

        fno = les_flash_fs_file_add(fsp, fp->fname);
        if (fno < 0)  /* Error. Could not add file. */
            return (FOPEN_FSFULL);
        break;

    case FILE_FLAGS_DEL:
        if (fno < 0) {  /* file doesn't exist or is already deleted; error */
            return (FOPEN_NOFILE);
        }
        updt = les_flash_fs_file_delete(fsp, fno);
        break;

    default:
        return (FOPEN_INV_MODE);
    }

    fsoi = malloc(sizeof(fs_open_instance_t));
    if (!fsoi)
        return (FOPEN_NOMEM);

    fsoi->updt = updt; /* whether any update required during 'close' */
    fsoi->alt = alt;   /* index of alternate (duplicate) file, if any */
    fsoi->hdr_start = (ulong)fsp->f_fhdrs[fno].ffh_addr;
    fsoi->data_start = (ulong)fsp->f_addr[fno];
    fsoi->cksum = 0; /* temporary checksum store */

    fp->len = fsp->f_fhdrs[fno].ffh_len;
    fp->findx = fno;
    fp->fname = &(fsp->f_fhdrs[fno].ffh_name[0]);
    fp->fs_info = fsoi;
    return (FOPEN_OK);
}

static int les_flash_fs_fiopen (int dh, file_open_instance_t *fp)
{
    int updt = 0, idx;
    flash_t *fsp;
    fs_open_instance_t *fsoi;

    /*
     * See if device has specified file. Depending on the file
     * open mode, decide what action to take.
     */
    idx = fp->findx;
    if ((idx < 0) || (idx >= FLASH_FILE_CNT))
        return (FOPEN_NOFILE);

    /*
     * Get pointer to file system info from the device structure.
     */
    fsp = dev_get_fs_spec_info(dh);
    if (!fsp)
        return (FOPEN_NOFILE);

    if (idx >= fsp->f_filecnt)  /* Invalid file index */
        return (FOPEN_NOFILE);

    /*
     * See if file is deleted.
     */
    if (!(fsp->f_fhdrs[idx].ffh_flags & FFH_FLAGS_DUMMY)) /* black hole */
        return (FOPEN_DEL);
    if (!(fsp->f_fhdrs[idx].ffh_flags & FFH_FLAGS_DELETED)) /* deleted */
        if (!(fp->flags & FILE_FLAGS_NO_DEL_CHK)) /* Ignore deleted file */
            return (FOPEN_DEL);

    if (fp->flags & FILE_FLAGS_CHECKSUM)
        if (fsp->f_cksum[idx] != fsp->f_fhdrs[idx].ffh_checksum)
            return (FOPEN_CKSUM_ERR);

    /*
     * Since a file name has not been specified, only allow a
     * read or a delete.
     */
    switch (fp->flags & FILE_FLAGS_OPENMASK) {

    case FILE_FLAGS_RDONLY:
        if (fp->flags & FILE_FLAGS_CHECKSUM) { /* See if checksum ok */
            /*
             * Validate file length. Don't calculate checksum if length
             * takes file beyond file system end.
             */
            if (((ulong)fsp->f_addr[idx] + fsp->f_fhdrs[idx].ffh_len) > fsp->fs_size)
                fsp->f_cksum[idx] = ~fsp->f_fhdrs[idx].ffh_checksum;
            else
                fsp->f_cksum[idx] = ~les_flash_fs_checksum_wrapper(dh, fsp, idx,
                                         fsp->f_fhdrs[idx].ffh_len, TRUE);

            if (fsp->f_cksum[idx] != fsp->f_fhdrs[idx].ffh_checksum)
                return (FOPEN_CKSUM_ERR);
        }
        break;

    case FILE_FLAGS_DEL:
        updt = les_flash_fs_file_delete(fsp, idx);
        break;

    case FILE_FLAGS_RW:
    default:
        /*
         * Do not allow file creation without a file name.
         */
        return (FOPEN_INV_MODE);
    }

    fsoi = malloc(sizeof(fs_open_instance_t));
    if (!fsoi)
        return (FOPEN_NOMEM);

    fsoi->updt = updt; /* whether any update required during 'close' */
    fsoi->alt = -1;
    fsoi->hdr_start = (ulong)fsp->f_fhdrs[idx].ffh_addr;
    fsoi->data_start = (ulong)fsp->f_addr[idx];
    fsoi->cksum = 0; /* temporary checksum store */

    fp->len = fsp->f_fhdrs[idx].ffh_len;
    fp->fname = &(fsp->f_fhdrs[idx].ffh_name[0]);
    fp->fs_info = fsoi;
    return (FOPEN_OK);
}

static int les_flash_fs_fread (file_open_instance_t *foi, char *buf, ulong len)
{
    fs_open_instance_t *fsoi;

    if (len) { /* got to read something */

        if (psipending()) { /* any user interrupt ? */
            if (foi->flags & FILE_FLAGS_VERBOSE)
                printf("\nUser interrupt - '%s' read aborted", foi->fname);
            return(0);
        }

        fsoi = foi->fs_info;
        len = dev_read(foi->devh, (uchar *)buf, len);
        if ((foi->flags & FILE_FLAGS_CHECKSUM) && len)
            fsoi->cksum = les_flash_fs_checksum(fsoi->cksum, buf, len);
    }
    return (len);
}

static int les_flash_fs_fwrite (file_open_instance_t *foi, char *buf, ulong len)
{
    fs_open_instance_t *fsoi;
    ulong wlen = 0;

    if (len) {  /* got to write something */
        fsoi = foi->fs_info;
        /*
         * Compute checksum. Always do this on a write.
         */
        fsoi->cksum = les_flash_fs_checksum(fsoi->cksum, buf, len);
        if (psipending()) { /* user interrupt ? */
            if (foi->flags & FILE_FLAGS_VERBOSE)
                printf("\nUser interrupt - '%s' write aborted", foi->fname);
            return(0);
        }

        wlen = dev_write(foi->devh, (uchar *)buf, len);
        if (wlen != len) { /* there must have been a write problem */
            fsoi->cksum = ~fsoi->cksum;  /* deliberately corrupt cksum */
            return (wlen); /* return length actually written */
        }
    }
    return (len);
}

static void les_flash_fs_fclose (file_open_instance_t *foi)
{
    fs_open_instance_t *fsoi;
    flash_t *fsp;
    flash_fhdr_t *fhp, *fhp_alt, *tmpfhp;
    uchar *addr;
    int idx;
    ulong wrlen=0;

    idx = foi->findx;
    switch (foi->flags & FILE_FLAGS_OPENMASK) {
    case FILE_FLAGS_RDONLY:
        /*
         * If file was opened in read mode, nothing needs to be done.
         */
        break;
    case FILE_FLAGS_RW:
    case (FILE_FLAGS_RW | FILE_FLAGS_ERASE):
        if (foi->fs_info)
            fsoi = foi->fs_info;
        else
            return;  /* file wasn't opened successfully. */
        fsp = dev_get_fs_spec_info(foi->devh);

        if (foi->wr_done) {   /* Some file data was actually written */
            /*
             * First, validate the length. Then save the checksum and length.
             */
            if (((ulong)fsp->f_addr[idx] + foi->len) > fsp->fs_size)
                foi->len = fsp->fs_size - (ulong)fsp->f_addr[idx];
                if ((long)foi->len <0)
                    foi->len = 0;

            fhp = (flash_fhdr_t *)&fsp->f_fhdrs[idx];
            fhp->ffh_len      = foi->len;
            fhp->ffh_checksum = ~fsoi->cksum;
            if (foi->flags & FILE_FLAGS_VERBOSE) {
                printf("\nVerifying checksum... ");
                flush();
            }
            fsp->f_cksum[idx] = ~les_flash_fs_checksum_wrapper(foi->devh, fsp, idx,
                                     fhp->ffh_len, TRUE);
            if (foi->flags & FILE_FLAGS_VERBOSE) {
                if (fsp->f_cksum[idx] == fhp->ffh_checksum)
                    printf(" OK (0x%x)", fhp->ffh_checksum);
                else
                    printf(" invalid (expected 0x%x, computed 0x%x)",
                           fhp->ffh_checksum, fsp->f_cksum[idx]);
            }

            /*
             * Now go update the header of the new file copied in.
             * Then delete any duplicate file if present.
             * Then update the next file offset we keep in memory.
             */
            addr = (uchar *)fhp->ffh_addr;
            dev_seek(foi->devh, DEV_START, (int)addr);
            wrlen = dev_write(foi->devh, (uchar *)fhp, sizeof(flash_fhdr_t));
            if (wrlen != sizeof(flash_fhdr_t)) {
                break;
            }
            /*
             * See if there was a duplicate file case. If so, go delete
             * the earlier file if the new file has been written with a
             * good checksum.
             */
            if ((fsoi->alt >= 0) && (fsp->f_cksum[idx] == fhp->ffh_checksum)) {
                fhp_alt = (flash_fhdr_t *)&fsp->f_fhdrs[fsoi->alt];
                fhp_alt->ffh_flags &= ~FFH_FLAGS_DELETED;
                tmpfhp = (flash_fhdr_t *)fhp_alt->ffh_addr;
                /*
                 * To avoid misalignments on sierra, seek to the checksum
                 * field (which is aligned on a long word boundary), and
                 * write two short words - the checksum and the flags field.
                 * Earlier, we were updating only the flags field (a short).
                 */
                addr = (uchar *)&tmpfhp->ffh_checksum;
                dev_seek(foi->devh, DEV_START, (int)addr);
                wrlen = dev_write(foi->devh, (uchar *)&fhp_alt->ffh_checksum,
                          2*sizeof(short));
                if (wrlen !=  2*sizeof(short) ){
                    break;
                }
            }

            fsp->f_free_offset += (fhp->ffh_len + sizeof(flash_fhdr_t));
            fsp->f_free_offset = les_flash_fs_long_align(fsp->f_free_offset);
            /*
             * No space left on device. Note that we need erasure
             * to add another file.
             */
            if (fsp->f_free_offset >= fsp->fs_size) {
                fsp->f_free_offset = fsp->fs_size;
                fsp->f_erase_needed = TRUE;
            }
        }
        else {  /* File was not written. Readjust filecnt & next file offset */
            fsp->f_filecnt--;
        }
        break;
    case FILE_FLAGS_DEL:
        /*
         * Seek to header of file. Update 'DELETED' flag bit.
         */
        fsoi = foi->fs_info;
        if (fsoi->updt & FILE_UPDATE_DELFLAG) {
            fsp = dev_get_fs_spec_info(foi->devh);
            fhp = (flash_fhdr_t *)&(fsp->f_fhdrs[idx]);
            tmpfhp = (flash_fhdr_t *)fhp->ffh_addr;
            /*
             * To avoid misalignments on sierra, seek to the checksum
             * field (which is aligned on a long word boundary), and
             * write two short words - the checksum and the flags field.
             * Earlier, we were updating only the flags field (a short).
             */
            addr = (uchar *)&tmpfhp->ffh_checksum;
            dev_seek(foi->devh, DEV_START, (int)addr);
            dev_write(foi->devh, (uchar *)&fhp->ffh_checksum, 2*sizeof(short));
        }
        break;
    default:
        break;
    }
    if (foi->fs_info)
        free(foi->fs_info);
    foi->fs_info = NULL;
}

static void les_flash_fs_fseek (file_open_instance_t *foi)
{
    fs_open_instance_t *fsoi;

    fsoi = foi->fs_info;
    dev_seek(foi->devh, DEV_START, fsoi->data_start + foi->cur_ptr);
    return;
}

/*
 * Display file system info - basically file directory.
 */
static void les_flash_fs_info (flash_t *fsp, int info)
{
    int i;
    flash_fhdr_t *fhp;
    flash_t *fp;

    if (!(info & SHOW_FS_INFO_MASK))    /* nothing to show */
        return;
    if (!fsp)                           /* Ignore nonexistant devices */
        return;

    fp = (flash_t *)fsp;
    fhp = &fp->f_fhdrs[0]; /* point to entry for first file in directory */
    i = 0;

    if (info & SHOW_FILES_SUMMARY) {	/* sh flash summary */
	printf("\n  %d        %-5dK  %-5dK    %-5dK    %-5dK"
	       "      %s \t  %s",
		fp->dev_subunit,
		fp->fs_size/1024,
		fp->f_free_offset/1024,
		(fp->fs_size - fp->f_free_offset)/1024,
		fp->dev_banksize/1024,
		fs_states[fp->fs_status],
		fs_copy_states[fp->fs_copy_mode]);
	return;
    }
    if (fp->dev_part_cnt == 1)
	printf("\n\n%s directory:", fp->dev_name);
    else 
	printf("\n\n%s directory, partition %d:", fp->dev_name, 
						  fp->dev_subunit);

    if (fp->f_filecnt) {  /* we have atleast one file */
        printf("\nFile  Length   Name/status");
        if (info & SHOW_FILES_DETAILED)
            printf("\n        addr      fcksum  ccksum");

	for (; i<fp->f_filecnt; i++, fhp++) { /* loop for each file */
	    printf("\n %-2d   %7d  %s %s %s",
		    i+1,
		    fp->f_fhdrs[i].ffh_len,
		    &fp->f_fhdrs[i].ffh_name[0],
		    (fhp->ffh_flags & FFH_FLAGS_DELETED) ? "" : "[deleted]",
		    (fhp->ffh_checksum ^ fp->f_cksum[i])
		    ? "[invalid checksum]" : "");

	    if (info & SHOW_FILES_DETAILED)
		printf("\n        0x%6x  0x%4x  0x%4x",
			fp->f_addr[i],
                        fp->f_fhdrs[i].ffh_checksum, /* original checksum */
                        fp->f_cksum[i]);  /* recomputed checksum */
	}
    } else {
        printf("\nNo files in %s", fp->dev_name);
    }
    printf("\n[%d bytes used, %d available, %d total]",
           fp->f_free_offset, fp->fs_size - fp->f_free_offset, fp->fs_size);
}

/*
 * Provide some generic info about the file system.
 */
static int les_flash_fs_stat (flash_t *fsp, fsstat_t *ptr)
{
    flash_t *fs;
    fsstat_t *fst;
    ushort i;

    fst = (fsstat_t *)ptr;
    fs = (flash_t *)fsp;
    if (fs) {
        fst->totcnt  = fs->f_filecnt;        /* total number of files */
        fst->next    = fs->f_free_offset;    /* offset of next file */
        fst->ersf    = fs->f_erase_needed;   /* whether erase needed */
        fst->start   = fs->fs_start;         /* start of FS within device */
        fst->phy_start = fs->fs_start + fs->dev_addr; /* phy start of FS */
        fst->free    = fs->fs_size - fs->f_free_offset; /* free space left */
        fst->size    = fs->fs_size;          /* file system size */
        fst->status  = fs->fs_status;        /* file system read-write status */
        /* Copy mode; Needs conversion to value specified in flash MIB */
        fst->copy_mode = les_flash_fs_map_copy_mode(fs->fs_copy_mode); 
        fst->delcnt  = 0;                    /* count of deleted files */
        fst->delsz   = 0;                    /* size of deleted files */
        for (i = 0; i < fs->f_filecnt; i++) {
            if (!(fs->f_fhdrs[i].ffh_flags & FFH_FLAGS_DUMMY))
                continue;
            if (!(fs->f_fhdrs[i].ffh_flags & FFH_FLAGS_DELETED)) {
                fst->delcnt++;
                fst->delsz += fs->f_fhdrs[i].ffh_len;
            }
	    fst->chksum[i] = fs->f_fhdrs[i].ffh_checksum;
        }
        fst->fcnt    = fst->totcnt - fst->delcnt; /* good file count */
        fst->filenm_length = FLASH_FILE_NAME_LEN;
        fst->chksum_algo =
	    D_ciscoFlashPartitionChecksumAlgorithm_simpleChecksum;
        return (1);  /* some positive number to indicate success */
    }
    return (DEVERR_INIT);
}

/*
 * Provide file info.
 * Return some generic info about a file maintained by the file system.
 */
static void les_flash_fs_fstat (file_open_instance_t *foi, fstat_t *fs)
{
    flash_t *fp;
    fs_open_instance_t *fsoi;
    flash_fhdr_t *fhdr;

    fp = dev_get_fs_spec_info(foi->devh); /* file system ptr */
    fsoi = foi->fs_info;

    fs->data_start = fsoi->data_start;  /* Start of file data within filesys */
    fs->hdr_start  = fsoi->hdr_start;   /* Start of file hdr within filesys */
    fs->cksf       = 1;                 /* Checksum ok - be optimistic */
    fs->delf       = 0;                 /* Be optimistic - file undeleted */

    if (fp) {
        fhdr = &fp->f_fhdrs[foi->findx];
        fs->cksf = (fp->f_cksum[foi->findx] == fhdr->ffh_checksum) ? 1 : 0;
        fs->delf = (fhdr->ffh_flags & FFH_FLAGS_DELETED) ? 0 : 1;
    }
}

/* Provide file system copy status.
 * Return one of the following 
 *    COPY-FLH
 *    COPY-MANUAL
 *    COPY-DIRECT
 *    NONE
 */
static ulong les_flash_fs_copy_state (ulong dev_state, int part_cnt)
{
    int flh_version;

    switch (dev_state) {
    case DEV_STATE_RFF:
    case DEV_STATE_RDONLY:
	flh_version = reg_invoke_flh_version();
	if (flh_version < FLASH_BASIC_FLH_SUPPORT)
	    /* FLH not supported */
	    return FLASH_FS_COPY_MANUAL;
	else if ((flh_version >= FLASH_FLH_SUPPORT_WITH_DFB) ||
		 ((part_cnt == 1) && (flh_version >= FLASH_BASIC_FLH_SUPPORT)))
	    return FLASH_FS_COPY_FLH;
	else 
	    return FLASH_FS_COPY_NONE;

    case DEV_STATE_RW:
	return FLASH_FS_COPY_DIRECT;

    case DEV_STATE_NODEV:
    case DEV_STATE_INIT:
    default:
	return FLASH_FS_COPY_NONE;
    }
}

/*
 * Sniff the filesystems existing in the flash device, and fill up
 * the flash_fs_bounds_t structure with information.
 */
static void les_flash_fs_sniff (char *nm, void *bounds)
{
    int bank = 0, max_bank, dh, sts, len;
    ulong offset;
    flash_fhdr_t f_fhdr;
    devstat_t devinfo;
    fs_bounds_t *fsbounds = (fs_bounds_t *)bounds;
    
    sts = dev_stat(nm, &devinfo);
    if (sts < 0) 
        return;
    if (devinfo.dev_banksize) {
        max_bank = (devinfo.dev_size)/(devinfo.dev_banksize) - 1;
    } else {
        max_bank = 0;
    }
    dh = dev_open(nm, DEV_FLAGS_RDONLY | DEV_FLAGS_EXCL);
    if (dh < 0)
        return;
    
    while (bank <= max_bank) {
        offset = bank * devinfo.dev_banksize;
        len = les_flash_fs_file_header_read(dh, &f_fhdr, offset); 
        if (len != sizeof(flash_fhdr_t)) /* Error */
            return;
        if (f_fhdr.ffh_magic == FLASH_FHDR_MAGIC) { /* File system found */
            fsbounds->start_bank = bank; /* Set start bank for filesystem */
           /*
            * Now, jump over this file system. In the process, also make
            * a note of any files in the file system which may end right
            * at the end of a bank boundary. We can allow partition 
            * boundaries to fall there.
            */
            offset = les_flash_fs_skip(dh, &f_fhdr, offset, fsbounds,
                                       devinfo.dev_banksize);
            while (bank * devinfo.dev_banksize <= offset)
                bank++;
            fsbounds->end_bank = bank - 1; /* Set last bank for file system */
            fsbounds++;
         }
         else    /* No file system here */
             bank++;
    }
    dev_close(dh);
    return;
}

/* 
 * Link the passed in file system structure into the linked 
 * list of file system structures for the device on which
 * this file system resides.
 */
static boolean les_flash_link_fs_struct (flash_t *ffs)
{
    char devnm[FLASH_LOG_DEV_NAME_LEN];
    flash_t *ffs_prev;
    int dh_prev;
    
    if (ffs->dev_subunit <= 1) /* Nothing to be done; first filesystem */
        return (TRUE);
    
    /* Get pointer to previous file system */
    strcpy(devnm, ffs->dev_alias);
    strcat(devnm, ":");
    sprintf(devnm + strlen(devnm), "%d", (ffs->dev_subunit - 1));
    dh_prev = dev_open(devnm, DEV_FLAGS_RDONLY | DEV_FLAGS_EXCL);
    if (dh_prev < 0)
        return (FALSE);
    ffs_prev = (flash_t *)dev_get_fs_spec_info(dh_prev);
    if (!ffs_prev) {
        dev_close(dh_prev);
        return (FALSE);
    }

    /* Link this file system structure into linked list */
    ffs_prev->next_fs = ffs;
    ffs->prev_fs = ffs_prev;

    dev_close(dh_prev);
    return (TRUE);
}

/*
 * Open low-end specific file system initialization and setup.
 */
static void les_flash_fs_open (char *nm, boolean cksum_flag)
{
    ulong len;
    flash_fhdr_t *f_fhdr;
    flash_t *ffs;
    devstat_t devinfo;
    fs_fns_t *fs_fns;
    int sts, dh;

    /*
     * Check if device present. If so, we'll also get some
     * generic device related info.
     */
    sts = dev_stat(nm, &devinfo);
    
    if (sts < 0) /* error - device not found (shouldn't really happen) */
        return;

    /*
     * Now open device
     */
    dh = dev_open(nm, DEV_FLAGS_RDONLY | DEV_FLAGS_EXCL);
    if (dh < 0) {
        return;
    }

    /*
     * Malloc structure for file system callback functions
     */
    if ((fs_fns = (fs_fns_t*)dev_get_fs_gen_info(dh)) == NULL) {
        fs_fns = malloc_named(sizeof(fs_fns_t), "FileSys Callback");
        if (!fs_fns) {
            dev_close(dh);
            return;
        }
    }

    /*
     * Set up file system functions for callback.
     *
     */
    fs_fns->fs_fopen_fn  = les_flash_fs_fopen;
    fs_fns->fs_fiopen_fn = les_flash_fs_fiopen;
    fs_fns->fs_fread_fn  = les_flash_fs_fread;
    fs_fns->fs_fwrite_fn = les_flash_fs_fwrite;
    fs_fns->fs_fclose_fn = les_flash_fs_fclose;
    fs_fns->fs_fseek_fn  = les_flash_fs_fseek;
    fs_fns->fs_fstat_fn  = les_flash_fs_fstat;


    /*
     * malloc data structure for storing file system info.
     */

    if ((ffs = (flash_t *)dev_get_fs_spec_info(dh)) == NULL) {
        ffs = malloc_named(sizeof(flash_t), "FileSys Info");
        if (!ffs) {  /* Uh oh. No memory */
            free(fs_fns);
            dev_close(dh);
            return;
        }
        ffs->mem_block = NULL;
    }

    ffs->f_free_offset  = 0;  /* where next file is to go */
    ffs->fs_start       = 0;  /* file system start within device */
    ffs->f_filecnt      = 0;  /* number of files */
    ffs->f_erase_needed = TRUE;  /* assume the worst */

    ffs->dev_name  = devinfo.dev_name;  /* name of device on which FS resides */
    ffs->dev_alias = devinfo.dev_alias; /* alias used to refer to device */
    ffs->dev_sub_alias = devinfo.dev_sub_alias; /* alias for sub-device */
    ffs->dev_subunit = devinfo.dev_subunit; /* Partition number */
    ffs->dev_part_cnt = devinfo.dev_part_cnt;  /* Number of partitions */
    ffs->dev_addr  = devinfo.dev_addr;  /* physical start addr of device */
    ffs->dev_size  = devinfo.dev_size;  /* device size */
    ffs->dev_banksize = devinfo.dev_banksize;  /* device bank size */
    ffs->fs_size   = devinfo.dev_size;  /* file system size */
    ffs->fs_status = devinfo.dev_state; /* state of file system (or device) */
    ffs->fs_copy_mode = les_flash_fs_copy_state(devinfo.dev_state,
						devinfo.dev_part_cnt);
    if (ffs->mem_block == NULL) {
        ffs->mem_block = malloc_named(FS_MALLOC_BLOCK, "FileSys Scratch");
    }
    ffs->next_fs = NULL;
    ffs->prev_fs = NULL;

    /*
     * Store file system functions table pointer in the device structure.
     */
    if (!dev_set_fs_gen_info(dh, fs_fns)) {
        free(fs_fns);
        free(ffs);
        dev_close(dh);
        return;
    }

    /*
     * Store file system specific info pointer in the device structure.
     */
    if (!dev_set_fs_spec_info(dh, ffs)) {
        dev_set_fs_gen_info(dh, NULL);
        free(fs_fns);
        free(ffs);
        dev_close(dh);
        return;
    }

    /*
     * Link this filesystem structure into the linked list of 
     * filesystem structures for this device.
     */
    if (!les_flash_link_fs_struct(ffs)) {
        free(fs_fns);
        free(ffs);
        dev_close(dh);
        return;
    }

    /* Read in some data from flash to see if a file exists */
    f_fhdr = (flash_fhdr_t *)&ffs->f_fhdrs[ffs->f_filecnt];
    len = les_flash_fs_file_header_read(dh, f_fhdr, ffs->f_free_offset);
    if (len != sizeof(flash_fhdr_t))  {
        dev_close(dh);
        return;
    }
    if (f_fhdr->ffh_magic == FLASH_FHDR_MAGIC)  /* File header present */
        ffs->f_filecnt = les_flash_fs_read(dh, ffs, f_fhdr, cksum_flag);
 
    dev_close(dh);
}

/*
 * This is a registry function that is used to populate the array of
 * file checksums for files in the newly created partition set, using
 * checksum information from the old set of partitions.
 */
static void les_flash_fs_map_file_cksums (void *ffs_old, void *ffs_new)
{
    flash_t *src_ffs, *dest_ffs;
    int src_indx, dest_indx;

    src_ffs = (flash_t *)ffs_old;
    dest_ffs = (flash_t *)ffs_new;

    dest_indx = 0;
    
    while (src_ffs && dest_ffs) {
        for (src_indx = 0; src_indx < src_ffs->f_filecnt; ) {
            /* Check if we are over the edge of this dest file system */
            if (dest_indx >= dest_ffs->f_filecnt) { 
                dest_ffs = dest_ffs->next_fs;
                /* Point to next file system that has non-zero files */
                while (dest_ffs && (dest_ffs->f_filecnt == 0))
                    dest_ffs = dest_ffs->next_fs;
                dest_indx = 0;
            }
            /* Copy the checksum for this file */
            dest_ffs->f_cksum[dest_indx++] = src_ffs->f_cksum[src_indx++];
        }
        src_ffs = src_ffs->next_fs; /* Proceed to next source file system */
    }
    return;
}

/*
 * This is a registry function that gets called when device information
 * structures are being uncreated. 
 */
static void les_flash_fs_mem_free (void *info_ptr, int flags)
{
    flash_t  *fs_spec_info;
    fs_fns_t *fs_gen_info;

    if (flags & DEV_FS_SPEC_INFO) { /* Need to free up fs specific info */
        fs_spec_info = info_ptr;
        if (fs_spec_info) {
            if (fs_spec_info->mem_block)
                free(fs_spec_info->mem_block);
            free(fs_spec_info);
        }
    }
    if (flags & DEV_FS_GEN_INFO) {  /* Need to free up fs generic info */
        fs_gen_info = info_ptr;
        if (fs_gen_info)
            free(fs_gen_info);
    }
}

/*
 * Init function for this subsystem.
 */
static void les_ffs_init (subsystype *subsys)
{
/*
 * Register function to be called for opening file system.
 * Function will be indexed on platform and device type.
 */
    reg_add_flash_device_file_system(DEVICE_LES_FLASH, les_flash_fs_open,
                                  "les_flash_fs_open");
    reg_add_file_system_init(DEVICE_LES_FLASH, les_flash_fs_init,
                                  "les_flash_fs_init");
    reg_add_map_file_cksums(DEVICE_LES_FLASH, les_flash_fs_map_file_cksums,
                                  "les_flash_fs_map_file_cksums");
    reg_add_file_system_info(DEVICE_LES_FLASH, les_flash_fs_info,
                                  "les_flash_fs_info");
    reg_add_file_system_stat(DEVICE_LES_FLASH, les_flash_fs_stat,
                                  "les_flash_fs_stat");
    reg_add_file_system_sniff(DEVICE_LES_FLASH, les_flash_fs_sniff,
                                  "les_flash_fs_sniff");
    reg_add_file_system_mem_free(DEVICE_LES_FLASH, les_flash_fs_mem_free,
                                  "les_flash_fs_mem_free");
}

/*
 * Subsystem header for low-end system Flash filing system.  
 * This susbsystem must be initialized second amongst all Flash 
 * subsystems.
 */

#define LES_FFS_MAJVERSION 1
#define LES_FFS_MINVERSION 0
#define LES_FFS_EDITVERSION  1

SUBSYS_HEADER(les_flash_file_system,LES_FFS_MAJVERSION, LES_FFS_MINVERSION, 
              LES_FFS_EDITVERSION, les_ffs_init, 
              SUBSYS_CLASS_DRIVER, NULL, NULL);
