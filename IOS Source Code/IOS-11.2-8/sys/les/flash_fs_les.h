/* $Id: flash_fs_les.h,v 3.2 1995/11/17 17:37:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/les/flash_fs_les.h,v $
 *------------------------------------------------------------------
 * flash_fs_les.h - Flash file system for low end systems.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: flash_fs_les.h,v $
 * Revision 3.2  1995/11/17  17:37:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:30:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:31:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FLASH_FS_LES_H__
#define __FLASH_FS_LES_H__


#define FLASH_FHDR_MAGIC	0xbad00b1e /* file header magic */
#define FLASH_FHDR_SIZE 	64
#define FLASH_NAME_LEN 		(FLASH_FHDR_SIZE - (2 * sizeof(ulong)) - \
                        	(2 * sizeof(ushort)) - sizeof(uchar *))
#define FLASH_ALIGN_FACTOR      3
#define FLASH_FS_END_MARGIN     16
#define FFH_FLAGS_DELETED       0x0001
#define FFH_FLAGS_DUMMY         (0x0100 | FFH_FLAGS_DELETED)

/*
 * File header. This struct prepends each file written to flash.
 */
typedef struct flash_file_hdr {
    ulong   ffh_magic;
    ulong   ffh_len;
    ushort  ffh_checksum;
    short   ffh_flags;
    uchar  *ffh_addr;   /* start of file hdr within flash */
    char    ffh_name[FLASH_NAME_LEN];
} flash_fhdr_t;

#define FLASH_FILE_CNT 32    /* maximum files we'll support - includes aborts */

/*
 * File system structure information.
 */
struct flash_t_ {
    char     *dev_name;      /* Device name or description */
    char     *dev_alias;     /* Device alias used to refer to device */
    char     *dev_sub_alias; /* Device alias used to refer to device */
    int      dev_subunit;    /* Sub-device number, starting from 1 */
    int      dev_part_cnt;   /* Number of partitions */
    ulong    dev_addr;       /* Physical start of device */
    ulong    dev_size;       /* Device total size */
    ulong    dev_banksize;   /* Device bank size */
    ulong    fs_start;       /* File system start in device */
    ulong    fs_size;        /* File system size */
    ulong    fs_status;      /* File system status - RDONLY, RW etc */
    ulong    fs_copy_mode;   /* File system copy mode - Rxboot-FLH etc */
    ulong    f_free_offset;  /* current write pointer offset */
    uchar    f_filecnt;      /* Number of files written  */
    boolean  f_erase_needed; /* Is erasure needed in order to write another*/
    flash_fhdr_t f_fhdrs[FLASH_FILE_CNT];  /* Last read file headers */
    uchar   *f_addr[FLASH_FILE_CNT]; /* start of file data within flash */
    ushort  f_cksum[FLASH_FILE_CNT]; /* computed as file header is read */
    char    *mem_block;      /* a general purpose malloced block */
    struct flash_t_ *next_fs;   /* Pointer to next filesystem on this device */
    struct flash_t_ *prev_fs;   /* Pointer to prev filesystem on this device */
};

/*
 * Data structure maintained by file system for every opened file.
 */
struct fs_open_instance_t_ {
    int updt;            /* whether file header update required */
    int alt;             /* index of duplicate file, if any */
    ulong hdr_start;     /* header offset within device */
    ulong data_start;    /* data start within device */
    ushort cksum;        /* temporary checksum calculation & store */
};

/* Possible values for fs_copy_mode field */
enum FLASH_FS_COPY {
    FLASH_FS_COPY_FLH       =  0,     /* copy to flash via FLH 		 */
    FLASH_FS_COPY_MANUAL    =  1,     /* copy to flash via manual operation */
    FLASH_FS_COPY_DIRECT    =  2,     /* copy to flash directly 	 */
    FLASH_FS_COPY_NONE      =  3,     /* can not copy to flash		 */
};

/*
 * Data structure to hold information about sniffed filesysstems.
 */
typedef struct {
    char start_bank;      /* first bank in this filesystem */
    char end_bank;        /* last bank in this filesystem  */
    char partnum;         /* partition number for this file system */
    char exception_bank[FLASH_FILE_CNT]; /* the exception banks for this fs */
} fs_bounds_t;

#endif /* __FLASH_FS_LES_H__ */
