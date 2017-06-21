/* $Id: file_io.h,v 3.2 1995/11/17 09:00:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dev/file_io.h,v $
 *------------------------------------------------------------------
 * file_io.h - Generic file i-o routines.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1995, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: file_io.h,v $
 * Revision 3.2  1995/11/17  09:00:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:54:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FILE_IO_H__
#define __FILE_IO_H__

#define MAX_FILE_OPEN_ENTRIES  32

/*
 * File info structure
 */
typedef struct {
    int indx;                  /* file index within file system */
    ulong flags;               /* file open flags */
    const char *name;          /* file name */
    ulong len;                 /* file length */
    ulong data_start;          /* start of file data within device */
    ulong hdr_start;           /* start of file header within device */
    boolean cksf;              /* Whether checksum valid */
    boolean delf;              /* Whether file deleted */
} fstat_t;

/*
 * File system info structure
 */
struct fsstat_t_ {
    ulong next;                /* Next file offset */
    ulong start;               /* Start of file system */
    ulong phy_start;           /* Physical start of file system */
    ulong size;                /* File system size */
    ulong free;                /* free space left in file system */
    ulong status;              /* File system status - RDONLY, RW etc */
    ulong copy_mode;           /* Copy mode for this file system */
    ulong delsz;               /* space taken up by deleted files */
    ushort delcnt;             /* count of deleted files */
    ushort fcnt;               /* Number of good files in file system */
    ushort totcnt;             /* Number of total files in file system */
    boolean ersf;              /* erase-needed flag */
    ulong filenm_length;       /* Max filename length for this file system */
    ushort  chksum_algo;       /* Checksum algo used on this file system */
    ushort chksum[MAX_FILE_OPEN_ENTRIES]; /* file checksum */

};

/*
 * Structure for every open file
 */
#define FILE_OI struct file_open_instance
typedef FILE_OI {
    int         devh;          /* device handle */
    ulong       tty;           /* user's tty */
    ulong       flags;         /* File open flags */
    ulong       cur_ptr;       /* Current position in file */
    int         findx;         /* Index to file (file seq no) */
    const char *fname;        /* File name */
    ulong       len;           /* File data length */
    int         fs_tag;        /* File system tag */
    fs_open_instance_t *fs_info;      /* File system specific info for open file */

    /*
     * Callback functions to the file system, duped here for convenience
     */
    int         (*fs_fread_fn)(FILE_OI *, char *, ulong);
    int         (*fs_fwrite_fn)(FILE_OI *, char *, ulong);
    void        (*fs_fclose_fn)(FILE_OI *);
    void        (*fs_fseek_fn)(FILE_OI *);
    void        (*fs_fstat_fn)(FILE_OI *, fstat_t *);

    boolean     wr_done;       /* Whether any data was actually written */
} file_open_instance_t;

/*
 * File system specific callback functions for various file i-o
 * operations. This structure is held in the dev_info struct.
 * When a file is opened, they are copied into the file_open struct
 * to avoid going through the dev_io struct.
 */
typedef struct {
    int         (*fs_fopen_fn)(int, FILE_OI *);
    int         (*fs_fiopen_fn)(int, FILE_OI *);
    int         (*fs_fread_fn)(FILE_OI *, char *, ulong);
    int         (*fs_fwrite_fn)(FILE_OI *, char *, ulong);
    void        (*fs_fclose_fn)(FILE_OI *);
    void        (*fs_fseek_fn)(FILE_OI *);
    void        (*fs_fstat_fn)(FILE_OI *, fstat_t *);
} fs_fns_t;

/*
 * File open error code
 */
#define FOPEN_POSITIVE          1
#define FOPEN_OK                0
#define FOPEN_INV_DEV          -1
#define FOPEN_DEVERR           -2
#define FOPEN_NOMEM            -3
#define FOPEN_TBLOVER          -4
#define FOPEN_FSERR            -5
#define FOPEN_NOFILE           -6
#define FOPEN_USRABORT         -7
#define FOPEN_NEED_ERASE       -8
#define FOPEN_FILEDUP          -9
#define FOPEN_FSFULL           -10
#define FOPEN_INV_MODE         -11
#define FOPEN_DEL              -12
#define FOPEN_CKSUM_ERR        -13
#define FOPEN_FNM_ERR          -14

/*
 * File seek positions
 */
#define FILE_START   1
#define FILE_CURR    2
#define FILE_END     3

/*
 * Possible values for file open flags field.
 * These by and large correspond to the device open flag bits.
 * BE SURE TO CHECK AND UPDATE THE DEVICE OPEN FLAGS IN dev_io.h.
 */
#define FILE_FLAGS_EXCL        1    /* Exclusive open access       */
#define FILE_FLAGS_NO_DEL_CHK  2    /* Don't check if file deleted */
#define FILE_FLAGS_CHECKSUM    4    /* Generate checksum           */
#define FILE_FLAGS_VERBOSE     8    /* Be verbose                  */
#define FILE_FLAGS_PROMPT      0x10 /* Prompt if necessary; implies verbose */
#define FILE_FLAGS_FORCE       0x20 /* Force open file even if read mode */
#define FILE_FLAGS_REOPEN      0x40 /* Reopen to purge device &| add file */
#define FILE_FLAGS_REOPEN_ERASE 0x80  /* Erase on reopening */
#define FILE_FLAGS_RDONLY      0x1000
#define FILE_FLAGS_RW          0x2000
#define FILE_FLAGS_ERASE       0x4000
#define FILE_FLAGS_DEL         FILE_FLAGS_ERASE
#define FILE_FLAGS_OPENMASK    0x7000

#define FILE_UPDATE_DELFLAG     1

/*
 * prototypes
 */
extern void file_io_init(void);
extern int file_open(const char *, int);
extern int file_reopen(int, int);
extern int file_dev_open(const char *dev, const char *file, int flags);
extern int file_iopen(const char *, int, int);
extern ulong file_read(int, char *, ulong);
extern ulong file_write(int, char *, ulong);
extern ulong file_seek(int, int, int);
extern void file_close(int);
extern ulong file_flags(int);
extern int file_stat(int, fstat_t *);
extern int fsstat(const char *, fsstat_t *);
extern const char *file_error_msg(int);

#endif /* __FILE_IO_H__ */
