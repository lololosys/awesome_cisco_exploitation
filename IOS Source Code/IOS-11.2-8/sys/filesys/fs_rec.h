/* $Id: fs_rec.h,v 3.2.60.1 1996/09/12 18:25:22 dtaubert Exp $
 * $Source: /release/112/cvs/Xsys/filesys/fs_rec.h,v $
 *------------------------------------------------------------------
 * File record management header file
 *
 * July 1995, Yan Ke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fs_rec.h,v $
 * Revision 3.2.60.1  1996/09/12  18:25:22  dtaubert
 * CSCdi50888:  Simultaenous copy tftp to slot0: and bootflash: crashes
 * router
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:07:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:13  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  01:39:22  yanke
 * Placeholder file.
 *
 * CSCdi39151:  modulized local/remote file systems
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define OPEN_FOR_READ		1	/* file opened for read */
#define OPEN_FOR_WRITE		2	/* file opened for write */

/*
 * error code defined here for tftp/rcp/nvram?
 */
#define FS_DEV_INV_FLAG		-1	/* invalid flag */
#define FS_DEV_INV_FILE_ID	-2	/* invalid file id */
#define FS_DEV_NO_SUCH_DEV	-3	/* no such dev */
#define FS_DEV_NO_MEM		-4	/* no more memory */
#define FS_DEV_INV_ADDR		-5	/* invalid (srv) addr */
#define FS_DEV_TFTP_READ	-6	/* tftp read error */
#define FS_DEV_TFTP_SOCK	-7	/* tftp open socket error */
#define FS_DEV_TFTP_WRITE	-8	/* tftp write error */
#define FS_DEV_RCP_READ		-9	/* rcp read error */
#define FS_DEV_NO_NV		-10	/* nvram not available */
#define FS_DEV_NV_WRITE		-11	/* nvram write open faile */
#define FS_DEV_INV_FD		-12	/* invalid file handle */
#define FS_DEV_NOT_OPEN		-13	/* file not opened */
#define FS_DEV_TFTP_INT		-14	/* tftp internal error */
#define FS_DEV_RCP_INT		-15	/* rcp internal error */
#define FS_DEV_FORK_ERR		-16	/* cfork error */
#define FS_DEV_FSIZE_ERR	-17	/* bad/null file size  */
#define FS_DEV_TASK_TERM	-18	/* task end abnormally */
#define FS_DEV_RCP_WRITE	-19	/* rcp write error */
#define FS_DEV_RCP_CLOSE	-20	/* rcp close error */
#define	FS_DEV_FILE_IN_USE	-21	/* file is in use */
#define	FS_DEV_DEL_NV		-22	/* nv erase error */
#define	FS_DEV_NOT_SUP		-23	/* service not supported */
#define	FS_DEV_INV_DEV		-24	/* invalid device id */
#define FS_DEV_INV_OP		-25	/* invalid op code */
#define FS_DEV_NV_INT		-26	/* nv filesys internal error */
#define FS_DEV_INV_WHENCE       -27     /* invalid whence option */
#define FS_DEV_INV_LSEEK        -28     /* lseek out of range */

#define	COMMON_ERR_TBL_MAX	25

/*
 * Device structure
 */
typedef struct fs_dev_ {
    uchar class;
    uchar file_len_max;
    uchar file_open_max;
    uchar file_open_cnt;
    char *name;
    void *dev_specific;
} fs_dev_t;

/*
 * Structure for file record
 */
typedef struct fs_file_rec_ {
    struct fs_file_rec_ *next;	/* used to link records */
    fs_dev_t *dev;		/* ptr to device block */
    uint flag;			/* control flag */
    int fd;			/* file descriptor */
    char *base;			/* used by nv filesys */
    int size;			/* used by nv filesys */
    int offset;			/* used by nv filesys */
    void *specific;		/* used by tftp/rcp file sys */
    char name[FS_FILE_NAME_MAX + 1];	/* file name */
} fs_file_rec_t;

#define FS_DEV_OPEN_MANY	-1	/* two many files opened */
#define FS_DEV_OPEN_ALRDY	-2	/* file opened already */
#define FS_DEV_NO_FS_REC	-3	/* no more file rec */

/*
 * Function prototypes
 */
extern fs_file_rec_t *get_file_rec_by_fd (fs_dev_t *devp, int fd);
extern fs_file_rec_t *get_file_rec_by_name(fs_dev_t *devp, const char *file_name);
extern fs_file_rec_t *alloc_local_file_rec(fs_dev_t *devp, const char *file_name);
extern void free_local_file_rec (fs_file_rec_t *frecp);
extern fs_file_rec_t *common_create_file_rec(const char *file_id,
    fs_dev_t *devp, int *error);
extern const char *common_geterrortext (int err_code);
extern boolean is_new_flash_file(const char *);
extern boolean is_flash_file(const char *file_id);


