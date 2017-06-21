/* $Id: rp_flash_filesys.c,v 3.2.60.1 1996/03/18 19:36:01 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/rp_flash_filesys.c,v $
 *------------------------------------------------------------------
 * C7000 embedded flash file system
 *
 * August 1995, Yan Ke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rp_flash_filesys.c,v $
 * Revision 3.2.60.1  1996/03/18  19:36:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:36:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/01/24  21:43:16  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:08:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/30  00:53:06  yanke
 * Place holder for further development.
 *
 * CSCdi39528:  create c7000 file subsystem
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "subsys.h"
#include "config.h"
#include "../os/boot.h"
#include "../dev/g_dev.h"
#include "../dev/flash_spec.h"
#include "../dev/flash_dvr_spec.h"
#include "../dev/flash_fmgr_private.h"

#include "filesys.h"
#include "filesys_registry.h"
#include "flh_config.h"
#include "fs_rec.h"

#include "ralib.h"
#include "fslib_internals.h"
#include "fslib_dev_intel_series_2plus.h"

#include "rp_fs_dev.h"
#include "rp_fc.h"

int monlib_bundle_size;
char *monlib_bundle;

#ifdef DEBUG
boolean rp_fs_debug = FALSE;
#define rp_fs_debug_print        if (rp_fs_debug) printf
#else
#define rp_fs_debug_print(params...)
#endif

static rp_flash_rec_t rp_flash_rec_table[FLASH_FILE_CNT];

static fs_dev_t rp_flash_dev_rec =
{
    IOCTL_DEV_TYPE_FLASH,		/* device type */
    FS_RP_FLASH_FILE_NAME_LEN,		/* max file name len */
    FS_RP_FLASH_OPEN_FILE_NUM,		/* max open files */
    0,					/* current opened file cnt */
    RP_FLASH_DEV_NAME,			/* pointer to device name string */
    NULL				/* device specific structure */
};

/**************************************************************************
 *  emulate FS_open on embedded flash
 */
int rp_fs_open_flash ( fs_file_rec_t *frecp, int flags, uint mode)
{
    rp_flash_rec_t *ffrp;
    flash_fhdr *ffh;

    rp_fs_debug_print("\nrp_fs_open_flash(%x, %x, %x) ",
                           frecp, flags, mode);
    
    /* frecp shouldn't be NULL, it is checked by fs_open_dev() 
     * before invoking us 
     */
    if ( (ffrp = rp_alloc_flash_rec()) == NULL) {
        printf("\nFlash open: can't get RP flash record");
        return FSLIB_ERROR;
    }
    flash_query_roms(flash.f_dev);
    if (flash_open( flash.f_dev, FLASH_OPEN) == NULL) {
        rp_free_flash_rec( ffrp);
        printf("\nFlash: error opening embedded flash");
        return FSLIB_ERROR;
    }
    switch ( frecp->flag) {
    case OPEN_FOR_READ:
	if (strcmp(frecp->name, ".") == 0)
	    ffh = &flash.f_fhdrs[0];
	else
	    ffh = flash_get_fhdr( flash.f_dev, frecp->name);
        if ( ffh != NULL) {
            frecp->offset = 0;
            frecp->size = ffh->ffh_len;
            ffrp->ffh = ffh;
            frecp->specific = (void *)ffrp;
            return frecp->fd;
        }
        printf("\nFile '%s' not found in embedded flash", frecp->name);
        break;

    case OPEN_FOR_WRITE:    
        /* check for write protection */
        if ( fg.fg_status == FR_READ_ONLY) {
            printf("\nFlash is READ-ONLY, possibly due to vpp (12V) jumper is not installed");
            break;
        }
        /* check file count and free flash space available to write */
        if (flash.f_filecnt >= FLASH_FILE_CNT) {
            printf("\nFlash file system full. Maximum file count %d reached.", 
                      FLASH_FILE_CNT);
            break;
        }
        ffh = flash_get_fhdr( flash.f_dev, frecp->name);
        if ( ffh != NULL) {
            printf("\nFile '%s' already exist in flash memory", frecp->name);
            printf("\nInvalidate existing copy of '%s'? ", frecp->name);
            if (!confirm(NULL)) {
                printf("\nCopy aborted - user request");
                break;
            }
            /* user wants to overwrite the current file 
             * the ffh will be saved in the "ffrp->dup_ffh" field
             */
        }
        if ((fg.fg_size - flash.f_free_offset) >= MIN_FILE_SIZE) {
            rp_fs_debug_print("\n%d bytes available for writing without erasure.",
                      fg.fg_size - flash.f_free_offset - sizeof(flash_fhdr));

            ffrp->dup_ffh = ffh;
            ffh = flash_init_fhdr(); 
            strcpy( ffh->ffh_name, frecp->name);
            ffh->ffh_addr = (uchar *)(flash.f_free_offset + sizeof(flash_fhdr));
            ffh->ffh_len = 0;
            flash.f_addr[flash.f_filecnt] = flash_address( ffh->ffh_addr);

            frecp->offset = 0;
            frecp->size = 0;
            ffrp->ffh = ffh;
            frecp->specific = (void *)ffrp;
            flash_assert_vpp();
            return(frecp->fd);
        }
        printf("\nFlash is filled to capacity.");
        break;

    default:
        printf("\nUnknown rp fs open flash flag %d", frecp->flag);
        break;
    }
    rp_free_flash_rec( ffrp);
    flash_close();
    return FSLIB_ERROR;
}


/**************************************************************************
 *  emulate FS_read on embedded flash
 */
int rp_fs_read_flash ( fs_file_rec_t *frecp, void *buffer, int count)
{
    rp_flash_rec_t *ffrp;
    flash_fhdr *ffh;
    uchar  *dev_addr;
    int remain, rlen, cnt;

    rp_fs_debug_print("\nrp_fs_read_flash(%x, %x, %d) ",
                           frecp, buffer, count);

    /* frecp shouldn't be NULL, it is checked by fs_read_dev() 
     * before invoking us 
     */
    ffrp = (rp_flash_rec_t *)(frecp->specific);
    ffh  = (flash_fhdr *)(ffrp->ffh);
    if ( ffh->ffh_magic != FLASH_FHDR_MAGIC) { 
        printf("\nInvalid flash file header in rp_fs_read_flash()");
        return FSLIB_ERROR;
    }
    dev_addr = ffh->ffh_addr + frecp->offset;
    remain = frecp->size - frecp->offset;
    rlen = min( remain, count);
    if ((cnt=dev_read( flash.f_dev, (uchar *)buffer,dev_addr,rlen)) != rlen) {
        printf("\nError reading flash device( rlen %d, cnt %d)\n", rlen ,cnt);
    }
    if ( cnt >= 0)
        frecp->offset += cnt;
    return( cnt);
}

/**************************************************************************
 *  Set embedded flash file flag
 */
static void rp_write_ffh_flags (flash_fhdr *ffh)
{
    flash_fhdr *fhdr_addr;

    rp_fs_debug_print("\nrp_write_ffh_flags(%08x,%s,%x) ", ffh, ffh->ffh_name,ffh->ffh_addr);
    fhdr_addr = (flash_fhdr *)(ffh->ffh_addr - sizeof(flash_fhdr));
    rp_fs_debug_print("\n     fhdr_addr %x, buf_addr %x, dev_addr %x, size %d\n",
             fhdr_addr, &(ffh->ffh_flags), &(fhdr_addr->ffh_flags), sizeof(ffh->ffh_flags));
    flush();

    flash_assert_vpp();
    flash_write( (uchar *)&(ffh->ffh_flags), 
                          (uchar *)&(fhdr_addr->ffh_flags), 
                          sizeof(ffh->ffh_flags), TRUE);
    flash_deassert_vpp();

}

/**************************************************************************
 *  emulate FS_close on embedded flash
 */
int rp_fs_close_flash ( fs_file_rec_t *frecp)
{
    rp_flash_rec_t *ffrp;
    flash_fhdr *ffh, *dup_ffh;
    uchar *buff;
 
    rp_fs_debug_print("\nrp_fs_close_flash(%x) ", frecp);

    /* frecp shouldn't be NULL, it is checked by fs_close_dev() 
     * before invoking us 
     */
    ffrp = (rp_flash_rec_t *)(frecp->specific);
    /*
     * If the file was opened for write, we need to do some housekeeping
     *     1. verify the checksum, read back from flash and compare with
     *        the checksum calculated during writing to flash 
     *        If checksum verification failed, don't delete the dup file
     *     2. write the flag of the dup_ffh to flash 
     *     3. write the new ffh to flash
     */
    if (frecp->flag == OPEN_FOR_WRITE) {
        ffh  = (flash_fhdr *)(ffrp->ffh);
        dup_ffh=(flash_fhdr *)(ffrp->dup_ffh);
        if ( ffh->ffh_magic == FLASH_FHDR_MAGIC) {
            ffh->ffh_len = frecp->size;
            if (ffh->ffh_len > 0) {
                ffh->ffh_checksum = ~ffh->ffh_checksum;
                rp_fs_debug_print("\nflash file \"%s\" len %x (%d), cksum %x",
                ffh->ffh_name, ffh->ffh_len, ffh->ffh_len, ffh->ffh_checksum);
                /* 
                 * flash_verify() needs a buffer of size TFTP_MAXDATA
                 */
                if ((buff = malloc(TFTP_MAXDATA)) != NULL) {
                    flash.f_cp_cksum[flash.f_filecnt] =
                    flash_verify(flash.f_dev, ffh, buff, ffh->ffh_addr, TRUE);
                    free(buff);
                } else {
                    printf("\nMemory allocation error");
                    printf("Can't verify flash file checksum, file deleted.");
                    flash.f_cp_cksum[flash.f_filecnt] = ~ffh->ffh_checksum;
                }
                if (flash.f_cp_cksum[flash.f_filecnt] == ffh->ffh_checksum) {
                    if ( dup_ffh) {
                        if (dup_ffh->ffh_magic == FLASH_FHDR_MAGIC) { 
                            dup_ffh->ffh_flags &= ~FFH_FLAGS_DELETED;
                            rp_write_ffh_flags( dup_ffh);
                        } else {
                            printf("\nInvalid dup flash file header");
                        }
                    } /* no dup file */
                } else {  /* checksum error */
                    ffh->ffh_flags &= ~FFH_FLAGS_DELETED;
                }
	        /*
                 * write the new file header to flash 
                 */
                flash_assert_vpp();
                flash_write((uchar *)ffh, (uchar *)flash.f_free_offset,
                            sizeof(flash_fhdr), TRUE);
                /*
                 * update flash specific data 
                 */
                flash.f_free_offset =
                    flash_align(flash.f_free_offset + sizeof(flash_fhdr) +
                                ffh->ffh_len);
                flash.f_filecnt++;
            } /* if (ffh->ffh_len > 0) */
        } else {
            printf("\nInvalid flash file header in rp_fs_close_flash()");
        }
        flash_deassert_vpp();
    }
    rp_free_flash_rec( ffrp);
    flash_close();
    return 0;
}

/**************************************************************************
 * This is a special checksum routine used to accumulate the checksum
 * of the source during copy into the destination when the source
 * and the destination are on different odd/even byte boundary.
 */
static ushort update_odd_cksum(ushort cksum, uchar *wp, ulong i)
{
    ushort data;

    if ( ((ulong)wp & 1) == 0) {
        data = *wp++ & 0x00FF;
        i--;
        cksum = add_to_cksum(data, cksum);
    }
    while ( i > 1) {
        data = (*wp++ << 8) & 0xFF00;
        data |= *wp++;
        i -= 2;
        cksum = add_to_cksum(data, cksum);
    }
    if ( i) {
        data = (*wp << 8) & 0xFF00;
        cksum = add_to_cksum(data, cksum);
    }
    return(cksum);
}

/**************************************************************************
 *   emulate FS_write on embedded flash
 */
int rp_fs_write_flash ( fs_file_rec_t *frecp, void *buffer, int size)
{
    rp_flash_rec_t *ffrp;
    flash_fhdr *ffh;
    uchar  *dev_addr, *dev_begin_addr, *buf_addr;
    int  total, nbytes, wlen;

    rp_fs_debug_print("\nrp_fs_write_flash(%x, %x, %d) ", 
                           frecp, buffer, size);
    /*
     * frecp shouldn't be NULL, it is checked by fs_write_dev() 
     * before invoking us 
     */
    ffrp = (rp_flash_rec_t *)(frecp->specific);
    ffh  = (flash_fhdr *)(ffrp->ffh);
    if ( ffh->ffh_magic != FLASH_FHDR_MAGIC) { 
        printf("\nInvalid flash file header in rp_fs_write_flash()");
        return FSLIB_ERROR;
    }
    dev_begin_addr = ffh->ffh_addr + frecp->offset;
    dev_addr = dev_begin_addr;
    buf_addr = (uchar *)buffer;
    total = 0;
    while ( size > 0) {
        wlen = min( 2048, size);
        rp_fs_debug_print("\n    dev_write(buf_addr %x,dev_addr %x,wlen %d) ",
                         buf_addr, dev_addr, wlen);
        nbytes=dev_write( flash.f_dev, buf_addr, dev_addr, wlen);
        rp_fs_debug_print(" = %d", nbytes);
        if ( nbytes != wlen) {
            printf("\nError programming flash memory (%d,%d).\n", nbytes, wlen);
            total += nbytes;
            break;
        }
        flash_delay(10);
        dev_addr += wlen;
        buf_addr += wlen;
        size -= wlen;
        total += wlen;
    }
    rp_fs_debug_print("\n         update_cksum(%x,%x,%d) [%s]",
                      ffh->ffh_checksum, (ulong)buffer, total, buffer);
    if ( ((ulong)buffer & 1) ^ ((ulong)dev_begin_addr & 1))
        ffh->ffh_checksum = update_odd_cksum(ffh->ffh_checksum, (uchar *)buffer, total);
    else
        ffh->ffh_checksum = update_cksum(ffh->ffh_checksum, (uchar *)buffer, total);
    rp_fs_debug_print(" = %x", ffh->ffh_checksum);
    frecp->offset += total;
    frecp->size += total;
    return( total);
}


/**************************************************************************
 *   emulate FS_delete on embedded flash
 */
int rp_fs_delete_flash (fs_dev_t *devp, const char *file_name)
{
    flash_fhdr *ffh;
    int rc;

    rp_fs_debug_print("\nrp_fs_delete_flash(%08x,%s) ", devp, file_name);

    flash_query_roms(flash.f_dev);
    if (flash_open( flash.f_dev, FLASH_OPEN) != NULL) {
        ffh = (flash_fhdr *)flash_get_fhdr( flash.f_dev, file_name);
        if ( ffh != NULL) {
            if ( ffh->ffh_magic == FLASH_FHDR_MAGIC) { 
                ffh->ffh_flags &= ~FFH_FLAGS_DELETED;
                rp_write_ffh_flags( ffh);
                rc = FSLIB_OK;
            } else {
                printf("\nError: Invalid flash file header");
                rc = FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR;
            }
        } else {
            printf("\nFile %s not found", file_name);
            rc = FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR;
        }
        flash_close();
    } else {
        printf("\nError: opening flash");
        rc = FSLIB_ERROR_DEVICE_NOT_READY;
    }
    return(rc);
}


/**************************************************************************
 * called by fs_fsstat_dev()
 */
int rp_fs_fsstat_flash (fs_file_rec_t *frecp, file_sys_stat_t *status)
{
    rp_flash_rec_t *ffrp;
    flash_fhdr *ffh;

    /* frecp shouldn't be NULL, it is checked by fs_fsstat_dev() 
     * before invoking us 
     */
    ffrp = (rp_flash_rec_t *)(frecp->specific);
    ffh  = (flash_fhdr *)(ffrp->ffh);
    if ( ffh->ffh_magic != FLASH_FHDR_MAGIC) { 
        printf("\nInvalid flash file header in rp_fs_fsstat_flash()");
        return FSLIB_ERROR;
    }
    /*
     * This function is currently not implementled because
     * FS_fsstat() does not call fs_fsstat_dev() 
     */
    return FSLIB_OK;
}


/***************************************************************************
 *
 *       Utility routines.
 *
 ***************************************************************************
 */

rp_flash_rec_t *rp_alloc_flash_rec (void)
{
    int i;

    for ( i=0; i< FLASH_FILE_CNT; i++) {
        if ( rp_flash_rec_table[i].flags == RP_FS_REC_INVALID) {
            rp_flash_rec_table[i].flags = RP_FS_REC_VALID;
            return( &rp_flash_rec_table[i]);
        }
    }
    return NULL;
}


void rp_free_flash_rec (rp_flash_rec_t *rpfsfp)
{
    if (rpfsfp)
        rpfsfp->flags = RP_FS_REC_INVALID;
}


/**************************************************************************
 * Check if given file id contains a device id support
 * the new flash file system
 */
boolean is_new_flash_file (const char *file_id)
{
    const char *dev_id;

    dev_id = get_dev_id_by_file_id(file_id);
    if (strcmp(dev_id, "slot0") == 0)
        return TRUE;
    return FALSE;
}


/**************************************************************************
 *  Check if given device id is a flash device id
 */
boolean is_flash_device (const char *dev_id)
{
    if ((strcmp(dev_id, "flash") == 0) ||
        (strcmp(dev_id, "slot0") == 0))
        return TRUE;
    return FALSE;
}

/**************************************************************************
 *  Check if given file id contains a flash device id.
 */
boolean is_flash_file (const char *file_id)
{
    const char *dev_id;

    dev_id = get_dev_id_by_file_id(file_id);
    return is_flash_device(dev_id);
}


/***************************************************************************
 *
 *       Emulate the FS_stat and FS_fstat of the new flash file system
 *
 ***************************************************************************
 */
static ulong rp_convert_ffh_flags_to_fs_type ( short ffh_flags)
{
    short file_type_bits;
    ulong fs_type;

    /*
     * bits in the file type field should be mutual exclusive 
     */
    file_type_bits = ~(ffh_flags | ~FFH_FLAGS_FILE_TYPE_FIELD);

    switch ( file_type_bits) {
    case FFH_FLAGS_SYS_CONFIG:
        fs_type = FILE_TYPE_SYS_CONFIG;
        break;

    case FFH_FLAGS_SYS_IMAGE:
        fs_type = FILE_TYPE_SYS_IMAGE;
        break;

    case FFH_FLAGS_TYPE_UNKNOWN:
        fs_type = FSLIB_ERROR;
        break;

    default:   /* any other conbination is error */ 
        fs_type = FSLIB_ERROR;
        break;
    }
    return( fs_type);
}

/**************************************************************************
 * Calculate CRC in RAM buffer
 */
ulong rp_calc_crc ( uchar *addr, ulong length, boolean yield)
{
    volatile uchar *cp;
    volatile ulong *buffer;
    ulong crc, last_lword;
    int   i;

    crc = 0UL;
    last_lword = 0UL;
    cp = (uchar *)&last_lword;
    for ( i = 0; i < (length & 3); i++)
        cp[i] = addr[length/sizeof(ulong) * sizeof(ulong) + i];
    /*
     * accumulate the CRC on the bytes read in the buffer
     * The algorithm is to rotate the current sum and then add the
     * next 32-bit word (one's complement).  This is slightly better
     * than a simple one's complement sum.
     */
    buffer = (volatile ulong *)addr;
    for (i = 0; i < (length/sizeof(ulong)); i++) {
        /*
         * rotate the crc
         */
        if (crc & 0x80000000) {
            crc <<= 1;
            crc++;
        } else {
            crc <<= 1;
        }
        /*
         * add in the next 32-bit word
         */
        crc += buffer[i];
        if (crc < buffer[i]) {
            crc++;
        }
        if ( yield && ((i % 40960) == 0))
            ralib_yield(10000);       /* don't hog cpu for too long */
    }

    if ( length & 3) {
        /*
         * rotate the crc
         */
        if (crc & 0x80000000) {
            crc <<= 1;
            crc++;
        } else {
            crc <<= 1;
        }
        /*
         * add in the last 32-bit word
         */
        crc += last_lword;
        if (crc < last_lword) {
            crc++;
        }
    }

    /*
     * We don't allow
     * crc's of 0 or 0xffffffff -- we map them to other numbers
     */
    if (crc == 0)
        crc = 1;
    else if (crc == 0xffffffff)
        crc = 0xfffffffe;

    return(crc);
}

/**************************************************************************
 * This function calculates the CRC of the embedded flash file
 * to support the FS_stat() and FS_fstat() emulation for the 
 * embedded flash (the file status structure has a CRC field
 */
static ulong rp_calc_flash_crc ( flash_fhdr *ffh)
{
    uchar *addr;

    addr = flash_address( ffh->ffh_addr);
    *(volatile ulong *)addr = 0xFFFFFFFF;
    return( rp_calc_crc( addr, ffh->ffh_len, TRUE));
}

/**************************************************************************
 * convert embedded flash file flag to file system file status
 */
static void rp_convert_ffh_to_stat( flash_fhdr *ffh, stat_t *status)
{
    /* standard Unix fields */
    status->st_ino = 0;
    status->st_mode = 0;
    status->st_nlink = 1;
    status->st_uid = 0;
    status->st_gid = 0;
    status->st_size = ffh->ffh_len;
    status->st_atime = 0;
    status->st_mtime = 0;
    status->st_ctime = 0;
    status->st_blksize = 0;
    status->st_blocks = 0;

    /* special cisco fields */
    status->type    = rp_convert_ffh_flags_to_fs_type(ffh->ffh_flags);
    status->flags   = 0;                /* not ffh->ffh_flags ??? */
    status->crc     = rp_calc_flash_crc( ffh);
    status->valid   = TRUE;
    status->deleted = (ffh->ffh_flags & FFH_FLAGS_DELETED) ? FALSE:TRUE;
    status->namlen  = strlen(ffh->ffh_name);
    sstrncpy(status->name,ffh->ffh_name,status->namlen);

}

/**************************************************************************
 * Called by FS_stat() to intercept embedded flash on 7000
 */
int rp_fs_stat_flash (const char *file_id, stat_t *status) 
{
    const char *dev_id;
    const char *file_name;
    flash_fhdr *ffh;
    int rc;

    dev_id = get_dev_id_by_file_id(file_id);
    if (dev_id) {
        if (strcmp(dev_id, FLASH_DEV_NAME) != 0)
            return NOT_RP_FLASH_DEVICE;
    } else {
        return NOT_RP_FLASH_DEVICE;
    }
    file_name = get_file_name_by_file_id(file_id);
    if (file_name == NULL) {
        printf("\nFile name missing");
        return( FSLIB_ERROR_NULL_FILE_NAME_PTR);
    } 
    if (flash_open( flash.f_dev, FLASH_OPEN)) {
        ffh = flash_get_fhdr( flash.f_dev, file_name);
        if ( ffh != NULL) {
            rp_convert_ffh_to_stat( ffh, status);
            rc = FSLIB_OK;
        } else {
            status->valid = FALSE;
            status->st_size = 0;
            status->type = 0;
            rc = FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR;
        }
        flash_close();
    } else {
        printf("\nFlash: error opening flash");
        rc = FSLIB_ERROR_DEVICE_NOT_READY;
    }
    return(rc);
}

/**************************************************************************
 * Convert file system type to embedded flash file flag
 */
static void rp_convert_fs_type_to_ffh_flags ( ulong type, short *ffh_flags)
{
    switch ( type) {
    case FILE_TYPE_SYS_CONFIG:
        *ffh_flags &= ~FFH_FLAGS_SYS_CONFIG;
        break;
    case FILE_TYPE_SYS_IMAGE:
        *ffh_flags &= ~FFH_FLAGS_SYS_IMAGE;
        break;
    default:
        break;
    }
}


/**************************************************************************
 * called by fs_fstat_dev() to get file status by fd (after open)
 */
int rp_fs_fstat_flash (fs_file_rec_t *frecp, stat_t *status)
{
    rp_flash_rec_t *ffrp;
    flash_fhdr *ffh;

    ffrp = (rp_flash_rec_t *)(frecp->specific);
    ffh  = (flash_fhdr *)(ffrp->ffh);
    if ( ffh->ffh_magic != FLASH_FHDR_MAGIC) { 
        printf("\nInvalid flash file header in rp_fs_fstat_flash()");
        return FSLIB_ERROR;
    }
    rp_convert_ffh_to_stat( ffh, status);

    return(FSLIB_OK);
}


/**************************************************************************
 * call by FS_fsstat() to get flash system status by dev_id 
 */
int rp_fs_fsstat_flash_dev (const char *dev_id, file_sys_stat_t *status)
{
    if (dev_id) {
        if (strcmp(dev_id, FLASH_DEV_NAME) != 0)
            return NOT_RP_FLASH_DEVICE;
    } else
        return NOT_RP_FLASH_DEVICE;

    status->bytes_available = fg.fg_size - flash.f_free_offset;
    status->bytes_used = flash.f_free_offset;
    status->file_hdr_size = sizeof(flash_fhdr);
    return(FSLIB_OK);
}


/**************************************************************************
 * called by FS_settype() to set file type on 7000 embedded flash 
 */
int rp_fs_settype_flash (const char *file_id, ulong type)
{
    const char *dev_id;
    const char *file_name;
    flash_fhdr *ffh;
    int rc;

    dev_id = get_dev_id_by_file_id(file_id);
    if (dev_id) {
        if (strcmp(dev_id, FLASH_DEV_NAME) != 0)
            return NOT_RP_FLASH_DEVICE;
    } else
        return NOT_RP_FLASH_DEVICE;

    file_name = get_file_name_by_file_id(file_id);
    if (file_name == NULL) {
        printf("\nFile name missing");
        return( FSLIB_ERROR_NULL_FILE_NAME_PTR);
    }
    flash_query_roms(flash.f_dev);
    if (flash_open( flash.f_dev, FLASH_OPEN)) {
        ffh = flash_get_fhdr( flash.f_dev, file_name);
        if ( ffh != NULL) {
            rp_convert_fs_type_to_ffh_flags( type, &(ffh->ffh_flags));
            rp_write_ffh_flags( ffh);
            rc = FSLIB_OK;
        } else {
            rc = FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR;
        }
        flash_close();
    } else {
        printf("\nFlash: error opening flash");
        rc = FSLIB_ERROR_DEVICE_NOT_READY;
    }
    return(rc);
}


/**************************************************************************
 *   emulate FS_undelete on embedded flash
 */
int rp_fs_undelete_flash (const char *dev_id, int index, int mode)
{
    if (dev_id) {
        if (strcmp(dev_id, FLASH_DEV_NAME) != 0)
            return NOT_RP_FLASH_DEVICE;
    } else
        return NOT_RP_FLASH_DEVICE;

    printf("\nFiles on 7000 embedded flash memory cannot be undeleted.\n");
    return(FSLIB_OK);
}

/**************************************************************************
 *   emulate FS_squeeze on embedded flash
 */
int rp_fs_squeeze_flash (const char *dev_id, int flag)
{
    if (dev_id) {
        if (strcmp(dev_id, FLASH_DEV_NAME) != 0)
            return NOT_RP_FLASH_DEVICE;
    } else
        return NOT_RP_FLASH_DEVICE;

    if ((flag & SQUEEZE_QUIET) == 0)
	printf("\nThe 7000 embedded flash memory cannot be squeezed.\n");
    return(FSLIB_OK);
}

/**************************************************************************
 *   emulate FS_getdents on embedded flash 
 */
static void rp_convert_ffh_to_dirent(flash_fhdr *ffh, dirent_t *dirp, int i)
{
    dirp->d_reclen  = sizeof(dirent_t);
    dirp->d_off     = 0;
    dirp->d_fileno  = i+1;
    dirp->d_namlen = strlen(ffh->ffh_name);
    sstrncpy(dirp->d_name, ffh->ffh_name, dirp->d_namlen+1);
    rp_fs_debug_print("\nffh_name:%s\n", ffh->ffh_name);
    rp_fs_debug_print("\nd_name:%s\n", dirp->d_name);
    dirp->size	    = ffh->ffh_len;
    dirp->type      = rp_convert_ffh_flags_to_fs_type(ffh->ffh_flags);
    dirp->time       = 0;
    dirp->crc       = rp_calc_flash_crc(ffh);
    dirp->valid     = TRUE;
    dirp->deleted   = (ffh->ffh_flags & FFH_FLAGS_DELETED) ? FALSE:TRUE;
    dirp->filetype  = 0;
}
int rp_fs_getdents_flash (int fd, char *buffer, int buffer_length)
{
    flash_fhdr *ffh;
    static unsigned char i=0;
    if ( flash.f_filecnt ) {
	if ( i < flash.f_filecnt ) {
	    ffh =  &flash.f_fhdrs[i];
	    rp_convert_ffh_to_dirent(ffh, (dirent_t *)buffer, i);
	    i++;
	    return sizeof(dirent_t);
	}else {
	    i=0;
	    return FSLIB_OK;
	}
    } else {
	printf("Flash: No files present.\n");
	return FSLIB_OK;
    }
}


/**************************************************************************
 * quick and dirty way to do "dir flash:", will be changed later
 */
void rp_dir_flash (void)
{
    flash_print_hardware();
    flash_query_roms(flash.f_dev);
    flash_print_files( TRUE);   /* modified to display type */
}    


/**************************************************************************
 * initialize the embedded flash as a device in the file system
 */
void fs_init_rp_flash (void)
{
    int i;
#ifdef NODEF
    fs_dev_t *devp;

    /*
     *	7000 rp1 flash device driver.
     */
    devp= &fs_dev_tbl[FS_DEV_CLASS_FLASH];
    devp->class = FS_DEV_CLASS_FLASH;
    devp->file_len_max = FS_MAX_FILE_NAME_LEN_FLASH;
    devp->file_open_max = FS_MAX_FILE_OPEN_FLASH;
    strcpy(devp->name, FLASH_DEV_NAME);
    devp->dev_specific = (void *)NULL;
    devp->fs_open_fn = rp_fs_open_flash;
    devp->fs_close_fn = rp_fs_close_flash;
    devp->fs_read_fn = rp_fs_read_flash;
    devp->fs_write_fn = rp_fs_write_flash;
    devp->fs_delete_fn = rp_fs_delete_flash;
    devp->fs_fstat_fn = rp_fs_fstat_flash;
    devp->fs_fsstat_fn = rp_fs_fsstat_flash;
#endif
    /*
     *	initialize the RP file system flash file record
     */
    for ( i=0; i< FLASH_FILE_CNT; i++)
        rp_flash_rec_table[i].flags = RP_FS_REC_INVALID;

    monlib_bundle_size = (int)&bndl_monlib_end - (int)&bndl_monlib_start;
    monlib_bundle = &bndl_monlib_start;
    bndl_monlib_crc = rp_calc_crc( &bndl_monlib_start, monlib_bundle_size, FALSE);
}

/*
 * Verify if the flag is valid.
 */
static boolean valid_file_flags (int flags)
{
    /*
     * check that the file mode is one of the valid modes
     */
    if (flags == O_RDONLY
	|| flags == (O_WRONLY | O_APPEND | O_TRUNC)
	|| flags == (O_WRONLY | O_APPEND | O_TRUNC | O_CREAT))
	return TRUE;
    else
	return FALSE;
}

/*
 * Create a new file record for rp flash file.
 * Perform device specific checking on file id.
 */
static fs_file_rec_t *create_file_rec (const char *file_id, int flags,
	fs_dev_t *devp, int *retcode)
{
    const char *file_name;
    fs_file_rec_t *frecp;

    file_name = get_file_name_by_file_id(file_id);
    if (file_name == NULL) {
	*retcode = FS_DEV_NOT_SUP;
	return NULL;
    }

    if (valid_file_flags(flags) == FALSE) {
	*retcode = FS_DEV_INV_FLAG;
	return NULL;
    }

    frecp = common_create_file_rec(file_id, devp, retcode);
    if (frecp == NULL)
	return NULL;

    if (flags == O_RDONLY)
	frecp->flag = OPEN_FOR_READ;
    else
	frecp->flag = OPEN_FOR_WRITE;
    return frecp;
}

static int RP_FLASH_open (const char *file_id, int flags, ulong mode)
{
    fs_file_rec_t *frecp;
    int retcode;
    int fd;

    frecp = create_file_rec(file_id, flags, &rp_flash_dev_rec, &retcode);
    if (frecp == NULL)
	return retcode;

    fd = rp_fs_open_flash(frecp, flags, mode);
    if (fd < 0)
	free_local_file_rec(frecp);
    return fd;
}

static int RP_FLASH_close (int fd)
{
    fs_file_rec_t *frecp;
    int retcode;

    frecp = get_file_rec_by_fd(&rp_flash_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;
    retcode = rp_fs_close_flash(frecp);
    free_local_file_rec(frecp);
    return retcode;
}

static int RP_FLASH_read (int fd, void *buffer, int count)
{
    fs_file_rec_t *frecp;

    frecp = get_file_rec_by_fd(&rp_flash_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    if (frecp->flag != OPEN_FOR_READ)
	return FS_DEV_NOT_OPEN;

    return rp_fs_read_flash(frecp, buffer, count);
}

static int RP_FLASH_write (int fd, void *buffer, int cnt)
{
    fs_file_rec_t *frecp;

    frecp = get_file_rec_by_fd(&rp_flash_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    if (frecp->flag != OPEN_FOR_WRITE)
	return FS_DEV_NOT_OPEN;

    return rp_fs_write_flash(frecp, buffer, cnt);
}

static int RP_FLASH_stat (const char *file_id, stat_t *status)
{
    return rp_fs_stat_flash (file_id, status);
}

static int RP_FLASH_fstat (int fd, stat_t *status)
{
    fs_file_rec_t *frecp;

    frecp = get_file_rec_by_fd(&rp_flash_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    return rp_fs_fstat_flash(frecp, status);
}

static int RP_FLASH_fsstat (const char *dev_id, file_sys_stat_t *status)
{
    return rp_fs_fsstat_flash_dev(dev_id, status);
}

static int RP_FLASH_delete (const char *file_id)
{
    const char *file_name;

    file_name = get_file_name_by_file_id(file_id);
    return rp_fs_delete_flash(&rp_flash_dev_rec, file_name);
}

static int RP_FLASH_undelete (const char *dev_id, int index, int mode)
{
    return rp_fs_undelete_flash(dev_id, index, mode);
}

static int RP_FLASH_squeeze (const char *dev_id, int flag)
{
    return rp_fs_squeeze_flash(dev_id, flag);
}

static int RP_FLASH_getdents (int fd, char *buffer, int buffer_length)
{
    return rp_fs_getdents_flash(fd, buffer, buffer_length);
}

static int RP_FLASH_settype (const char *file_id, ulong file_type)
{
    return rp_fs_settype_flash(file_id, file_type);
}

static void RP_FLASH_geterrortext (int err_code, char *buffer, int blen)
{
    FSLIB_geterrortext(err_code, buffer, blen);
}

static int RP_FLASH_getdevicenames(char *buffer, int buffer_len)
{
    if (buffer_len > strlen(RP_FLASH_DEV_NAME)) {
	strcpy(buffer, RP_FLASH_DEV_NAME);
	return 0;
    }
    return FS_DEV_NV_INT;
}

static int RP_FLASH_ioctl (const char *file_id, int func, void *arg)
{
    int retcode = FSLIB_OK;
    uint dev_type = IOCTL_DEV_TYPE_LOCAL;

    switch (func) {
	case IOCTL_GET_DEV_TYPE:
	    dev_type |= IOCTL_DEV_TYPE_FLASH;
	    *(uint *)arg = dev_type;
	    break;

	default:
	    retcode = FSLIB_ERROR;
    }
    return retcode;
}

static int RP_FLASH_fioctl (int fd, int func, void *arg)
{
    return FS_DEV_NOT_SUP;
}

static int RP_FLASH_lseek (int fd, int offset, int whence)
{
    return FS_DEV_NOT_SUP;
}

static int RP_FLASH_istat (const char *dev_id, int index, stat_t *status)
{
    return FS_DEV_NOT_SUP;
}

/*
 * nvram subsystem init routine.
 */
static void rp_flash_subsys_init (subsystype* subsys)
{
    int file_sys_type;
    char device_name[FS_DEV_NAMES_MAX];
    int error;

    /*
     * register with the tftp file system layer
     * and get the file_sys_type to use.
     */
    error = (int)RP_FLASH_getdevicenames(device_name, FS_DEV_NAMES_MAX);
    file_sys_type = FS_register(device_name, FS_STD_ERROR_CODE_ALLOCATION);

    /*
     * add all the client file system routines to the registries
     */
    reg_add_open(file_sys_type, RP_FLASH_open, "RP_FLASH_open");
    reg_add_close(file_sys_type, RP_FLASH_close, "RP_FLASH_close");
    reg_add_read(file_sys_type, RP_FLASH_read, "RP_FLASH_read");
    reg_add_write(file_sys_type, RP_FLASH_write, "RP_FLASH_write");
    reg_add_lseek(file_sys_type, RP_FLASH_lseek, "RP_FLASH_lseek");
    reg_add_stat(file_sys_type, RP_FLASH_stat, "RP_FLASH_stat");
    reg_add_fstat(file_sys_type, RP_FLASH_fstat, "RP_FLASH_fstat");
    reg_add_istat(file_sys_type, RP_FLASH_istat, "RP_FLASH_istat");
    reg_add_fsstat(file_sys_type, RP_FLASH_fsstat, "RP_FLASH_fsstat");
    reg_add_delete(file_sys_type, RP_FLASH_delete, "RP_FLASH_delete");
    reg_add_undelete(file_sys_type, RP_FLASH_undelete, "RP_FLASH_undelete");
    reg_add_squeeze(file_sys_type, RP_FLASH_squeeze, "RP_FLASH_squeeze");
    reg_add_settype(file_sys_type, RP_FLASH_settype, "RP_FLASH_settype");
    reg_add_getdents(file_sys_type, RP_FLASH_getdents, "RP_FLASH_getdents");
    reg_add_geterrortext(file_sys_type,
	RP_FLASH_geterrortext, "RP_FLASH_geterrortext");
    reg_add_getdevicenames(file_sys_type,
	RP_FLASH_getdevicenames, "RP_FLASH_getdevicenames");
    reg_add_ioctl(file_sys_type, RP_FLASH_ioctl, "RP_FLASH_ioctl");
    reg_add_fioctl(file_sys_type, RP_FLASH_fioctl, "RP_FLASH_fioctl");
}

/*
 * Subsystem header for the c7000 embedded flash file system
 */
SUBSYS_HEADER(rp_flash_file_system,   /* name */
    1,				/* major version */
    0,				/* minor version */
    1,				/* edit version */
    rp_flash_subsys_init,	/* init function */
    SUBSYS_CLASS_DRIVER,	/* subsystem class */
    "seq: file_system",		/* sequence list */
    "req: file_system"		/* prerequisite list */
);
