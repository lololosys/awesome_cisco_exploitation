/* $Id: file_io.c,v 3.2.60.2 1996/05/22 04:45:52 snijsure Exp $
 * $Source: /release/112/cvs/Xsys/dev/file_io.c,v $
 *------------------------------------------------------------------
 * file_io.c - Generic file i-o routines.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: file_io.c,v $
 * Revision 3.2.60.2  1996/05/22  04:45:52  snijsure
 * CSCdi58178:  HTTP problem with flash access
 * Branch: California_branch
 * CSCdi58178: HTTP problem with flash access
 *
 * Revision 3.2.60.1  1996/03/18  19:12:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:42:57  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:45:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:00:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:54:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/18  19:40:37  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:24:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "../dev/flash_defines.h"
#include "../dev/file_io.h"
#include "../dev/dev_io.h"
#include "../filesys/rommon_filesys.h"
#include "../filesys/filesys.h"
#include "ttysrv.h"


static file_open_instance_t *fopen_table[MAX_FILE_OPEN_ENTRIES];

/*************************** Local functions *************************/

/*
 * Validate file handle and, if valid, return pointer to
 * the file open structure.
 */
static file_open_instance_t *file_open_struct (int fh)
{
    file_open_instance_t *foi;

    if ((fh < 0) || (fh >= MAX_FILE_OPEN_ENTRIES))  /* Invalid */
        return (NULL);

    return (foi = (file_open_instance_t *)fopen_table[fh]);
}

/*
 * Function to parse out the device name and return the pointer
 * to the start of the file name. It will skip the delimiter
 * between the two names, and any leading spaces before the file name.
 */
static const char *flash_open_parse_name (const char *nmptr, char *devnm, int len)
{
    char *tmptr, *strptr;
    char *first_colon, *sec_colon;

    *devnm = 0;  /* assume the worst - no device name */

    /* Remove leading and trailing whitespace from input string */
    strptr = deblank(nmptr);
    if (strptr) {
        tmptr = strptr + strlen(strptr) - 1;  /* pt to last char of string */
        /* skip any trailing white spaces */
        while ((tmptr >= strptr) && ((*tmptr == ' ') || (*tmptr == '\t')))
            *tmptr-- = 0;
    }
    else
        return (nmptr);

    first_colon = strchr(strptr, FLASH_PATH_SEPARATOR);
    if (first_colon)  /* We found a ':' */
        sec_colon = strchr(first_colon + 1, FLASH_PATH_SEPARATOR);
    else
        sec_colon = NULL;

    if (sec_colon) {  /* <dev>:<part>:<file> case */
        *sec_colon++ = 0;  /* term "<dev>:<part>" string; point to <file> */
        strcpy(devnm, strptr);  /* copy <dev>:<part> to devnm */
        return (sec_colon);  /* file name */
    }
    else {
        if (first_colon) {  /* <dev>:<file> or <part>:<file> case */
            *first_colon++ = 0;  /* term "<dev>" string; */
            strcpy(devnm, strptr);
            return (first_colon); /* <file> */
        }
        else  /* <file> case */
            return (strptr);
    }
}

/* Common function for file open.
 * Open device. If success, get file system callback functions,
 * malloc the file open structure and find a slot in the file
 * open table for it, initialize the structure, call the file
 * system specific open function, and finally seek to the start of
 * the file.
 */
static int fopen_common (const char *devnm, const char *flnm, int fno, int flags)
{
    int dh, fh, level, sts;
    file_open_instance_t *foi;
    fs_fns_t *fs_fns;

    if (flags & FILE_FLAGS_REOPEN)  /* wrong open flag */
        return (FOPEN_INV_MODE);

    if (strlen(devnm) == 0) { /* no device specified */
        dh = dev_iopen(0, 0, flags);  /* try first device initialized */
        if (dh < 0)
            return (FOPEN_INV_DEV);
    }
    else {
        dh = dev_open(devnm, flags);
        if (dh < 0)
            return (FOPEN_DEVERR);
    }
    /*
     * Get pointer to file system specific file functions
     */
    fs_fns = dev_get_fs_gen_info(dh);
    if (!fs_fns) {
        dev_close(dh);
        return (FOPEN_FSERR);
    }

    foi = malloc(sizeof(file_open_instance_t));
    if (!foi) {
        dev_close(dh);
        return (FOPEN_NOMEM);
    }
    /*
     * Disable interrupts and look for a spare slot in the
     * file_open table. If not found, close device and return an
     * error code.
     */
    level = raise_interrupt_level(ALL_DISABLE);
    for (fh = 0; fh < MAX_FILE_OPEN_ENTRIES; fh++) {
        if (fopen_table[fh] == 0) {
            fopen_table[fh] = foi;
            break;
        }
    }
    reset_interrupt_level(level);

    if (fh >= MAX_FILE_OPEN_ENTRIES) { /* file_open table full */
        dev_close(dh);
        free(foi);
        return (FOPEN_TBLOVER);
    }

    /*
     * Initialize data structure for opened file.
     */
    foi->devh    = dh;   /* device handle */
    foi->tty     = stdio->ttynum;  /* user's tty */
    foi->flags   = flags;  /* open mode */
    foi->cur_ptr = 0;    /* current position within file */
    foi->len     = 0;    /* file length */
    foi->fs_info = NULL; /* file system specific info pointer */
    foi->fname   = flnm; /* file name - may not be known yet */
    foi->findx   = fno;  /* index within file system, may not be known yet */
    foi->wr_done = FALSE;  /* no data written to file */

    /*
     * Use any device partition number specified as a file system tag.
     * We do this to identify the file system we should search in case
     * file systems have been merged together because we don't recognize
     * device partitions.
     */
    foi->fs_tag  = dev_subunit(dh); /* get device partition no */

    /*
     * file system specific functions for all file i-o calls
     */
    foi->fs_fread_fn  = fs_fns->fs_fread_fn;
    foi->fs_fwrite_fn = fs_fns->fs_fwrite_fn;
    foi->fs_fclose_fn = fs_fns->fs_fclose_fn;
    foi->fs_fseek_fn  = fs_fns->fs_fseek_fn;
    foi->fs_fstat_fn  = fs_fns->fs_fstat_fn;

    /*
     * Call the file system specific function that handles a file open.
     */
    if (fno < 0)  /* file index not known */
        sts = (*fs_fns->fs_fopen_fn)(dh, foi); /* open with name */
    else
        sts = (*fs_fns->fs_fiopen_fn)(dh, foi); /* open with index */
    if (sts < 0) {
        dev_close(dh);
        fopen_table[fh] = 0;
        free(foi);
        return (sts);
    }

    if (foi->fs_info)  /* file fully opened by file system module */
        (*foi->fs_fseek_fn)(foi);  /* set pointer to start of file */
    return (fh);   /* return file handle */
}

/***************************************************************************
 *                    Global File-io Routines
 **************************************************************************/
/*
 * file_io_init
 * Initialize this module.
 */

void file_io_init (void)
{
    ushort i;

    /*
     * Initialize our table for open files.
     */
    for (i = 0; i < MAX_FILE_OPEN_ENTRIES; i++)
        fopen_table[i] = 0;
}

/*
 * file_open
 * Open a file by name.
 */
int file_open (const char *nm, int flags)
{
    char devnm[FLASH_DEV_NAME_LEN];
    const char *sptr;

    /*
     * The name passed in will consist of a device name and a file
     * name - "<dev>:[<part>:]<file>". Parse this name and separate
     * out the device and file names. The colon (:) is assumed to
     * be the delimiter between the device and file names.
     * <part> is the device partition number.
     */
    sptr = flash_open_parse_name(nm, devnm, FLASH_DEV_NAME_LEN);

    return (fopen_common(devnm, sptr, -1, flags));
}

/*
 * file_dev_open
 * Open a file with an internal device name.  Actually just a wrapper
 * for fopen_common.
 */

int file_dev_open (const char *dev, const char *nm, int flags)
{
    return (fopen_common(dev, nm, -1, flags));
}

/*
 * Open a file by its index within the file system. Such a file
 * is assumed to exist. Allow only read operations on the file.
 * Don't allow a file to be created without a name.
 */
int file_iopen (const char *devnm, int fno, int flags)
{

    if (fno < 0) /* invalid file index */
        return (FOPEN_NOFILE);

    return (fopen_common(devnm, "", fno, flags));
}

/*
 * Reopen a file already opened earlier. We use this when writing
 * a file. The reopen goes and completes any file system specific
 * function that was pending, eg., file system erase.
 * We need to do this because we need to squeeze in some validations
 * between opening a file and actually creating the directory entry
 * for it. The initial open gets all the user options (erase, invalidate
 * duplicate...). The reopen does the actual erase and directory
 * entry creation.
 */
int file_reopen (int fh, int flags)
{
    file_open_instance_t *foi;
    fs_fns_t *fs_fns;
    int sts;

    if ((foi = file_open_struct(fh)) == NULL)
        return (FOPEN_NOFILE);

    /*
     * Get pointer to file system specific file functions
     */
    fs_fns = dev_get_fs_gen_info(foi->devh);
    if (!fs_fns) {
        dev_close(foi->devh);
        return (FOPEN_FSERR);
    }

    foi->flags |= (flags | FILE_FLAGS_REOPEN);  /* fold in new flag value */
    sts = (*fs_fns->fs_fopen_fn)(foi->devh, foi);
    if (sts < 0) {
        dev_close(foi->devh);
        fopen_table[fh] = 0;
        free(foi);
        return (sts);
    }
    if (foi->fs_info)  /* file fully opened by file system module */
        (*foi->fs_fseek_fn)(foi);  /* set pointer to start of file */

    return (sts);
}
/*
 * Generic file read based on file handle, buffer, and length.
 */
ulong file_read (int fh, char *buf, ulong len)
{
    file_open_instance_t *foi;

    if ((foi = file_open_struct(fh)) == NULL)
        return (0);

    if ((foi->cur_ptr + len) > foi->len)  /* length exceeds file limit */
        len = foi->len - foi->cur_ptr;    /* adjust length */

    if (len) {  /* we need to read something */
        /*
         * Call file system specific read function.
         */
        len = (*foi->fs_fread_fn)(foi, buf, len);
        foi->cur_ptr += len;  /* update our position within the file */
    }
    return (len);
}

/*
 * Generic file write based on file handle, buffer, and length.
 */
ulong file_write (int fh, char *buf, ulong len)
{
    file_open_instance_t *foi;

    if ((foi = file_open_struct(fh)) == NULL)
        return (0);

    if (!(foi->flags & FILE_FLAGS_RW))  /* file not opened for write */
        return (0);

    if (len) { /* something to write */
        /*
         * Call file system specific write function.
         */
        len = (*foi->fs_fwrite_fn)(foi, buf, len);
	/* Update file length so that amendments to the file within the   */
	/* current body of the file do not produce file length extensions */
	if ((len + foi->cur_ptr)>foi->len)
		foi->len = (len + foi->cur_ptr);
        foi->cur_ptr += len; /* update our position within the file */
        if (len)
            foi->wr_done = TRUE;  /* we did write something */
    }
    return (len);
}

/*
 * Seek to a position within the file. Validate position and
 * ignore it if invalid. Return current position within file.
 * The offset may be relative to the start or end of the file,
 * or the current position in the file.
 */
ulong file_seek (int fh, int pos, int offset)
{
    file_open_instance_t *foi;
    int new_off;

    if ((foi = file_open_struct(fh)) == NULL)
        return (0);

    switch (pos) {
    case FILE_START:
        if ((ulong) offset <= foi->len) {  /* valid offset */
            foi->cur_ptr = offset;
            (*foi->fs_fseek_fn)(foi);
        }
        return (foi->cur_ptr);

    case FILE_CURR:
        new_off = foi->cur_ptr + offset;
        if (((ulong) new_off <= foi->len) && (new_off >= 0)) { /* valid offset */
            foi->cur_ptr = new_off;
            (*foi->fs_fseek_fn)(foi);
        }
        return (foi->cur_ptr);

    case FILE_END:
        new_off = foi->len + offset;
        if (((ulong) new_off <= foi->len) && (new_off >= 0)) { /* valid offset */
            foi->cur_ptr = new_off;
            (*foi->fs_fseek_fn)(foi);
        }
        return (foi->cur_ptr);

    default: /* invalid case. Do nothing but return current position */
        return (foi->cur_ptr);
    }
    return (foi->cur_ptr);
}

/*
 * Generic file close based on file handle.
 */
void file_close (int fh)
{
    file_open_instance_t *foi;

    if ((foi = file_open_struct(fh)) == NULL)
        return;

    (*foi->fs_fclose_fn)(foi); /* call fs specific close function */

    dev_close(foi->devh);

    fopen_table[fh] = 0;

    free(foi);
    return;
}

/* return file open flags */
ulong file_flags (int fh)
{
    file_open_instance_t *foi;

    if ((foi = file_open_struct(fh)) == NULL)
        return (0);
    return (foi->flags);
}
/*
 * Provide some generic info for a specified file.
 */
int file_stat (int fh, fstat_t *fs)
{
    file_open_instance_t *foi;

    if (!fs)  /* No structure to pass back the info */
        return (-1);

    if ((foi = file_open_struct(fh)) == NULL)
        return (-1);

    fs->indx  = foi->findx;     /* file index within file system */
    fs->flags = foi->flags;    /* file open flags */
    fs->name  = foi->fname;     /* file name */
    fs->len   = foi->len;       /* file length */
    (*foi->fs_fstat_fn)(foi, fs); /* now get some info from the file system */
    return (1);  /* ok */
}

/*
 * File system generic info.
 */
int fsstat (const char *devnm, fsstat_t *fs)
{
    if (!fs)  /* No structure to pass back the info */
        return (-1);
    /*
     * Go through the device. It will call the file system specific
     * function that will provide the info.
     */
    return (dev_fs_stat(devnm, fs));
}


/*
 * Implementaion of FS_open, FS_close, FS_stat interms of file_open,
 * file_close, file_stat resp. A intemediate measure till full POSIX
 * like "portable" file system API is implemented.
 * 
 */

int FS_open (const char *nm, int input_flag, ulong mode)
{
    int option=0;
    /* Common operations between all file sys available through this
     * API are
     * Open file to read, write or both
     * Currentlly the implementation to create, create and truncate
     * etc. is not present in file_open, so if those flags are specified
     * return error, -1;
     */
    if (input_flag == O_RDONLY)
        option = FILE_FLAGS_RDONLY; 
    else if (input_flag == O_WRONLY)
        option = FILE_FLAGS_RW;
    else if (input_flag == O_RDWR)
        option = FILE_FLAGS_RW;
    else
        return -1;
    return file_open(nm,option);
}

int FS_close (int fd)
{
    file_close(fd);
    return 0;
}

int FS_read (int fd, void *buffer, int byte_count)
{
    return file_read(fd,buffer,byte_count);
}


/* 
 * Note, only the common elements between structure stat_t and fstat_t
 * are st_size/len, and the filename. This function can be used to find 
 * length of the file, given its name.
*/

int FS_stat (const char *name, stat_t *buf)
{
    int fd,ret;
    fstat_t stat_buf;
    fd = file_open(name, FILE_FLAGS_RDONLY);
    if ( fd < 0 )
	return -1;
    ret = file_stat(fd,&stat_buf);

    buf->namlen = strlen(name); /* file name length */
    strcpy(buf->name,name);      /* file name */
    buf->st_size = stat_buf.len; /* file length */

    file_close(fd);
    return ret;
}
