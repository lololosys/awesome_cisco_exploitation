/* $Id: rp_fs_dev.h,v 3.2 1995/11/17 09:08:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/rp_fs_dev.h,v $
 *------------------------------------------------------------------
 * Constants, Structures and Functions for RP embedded flash 
 *
 * April 1995, Szewei Ju
 *
 * Copyright (c) 1995, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rp_fs_dev.h,v $
 * Revision 3.2  1995/11/17  09:08:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:57:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/27  21:52:55  izhu
 * CSCdi41172:  7000 dir/show flash command not consistant w/ 7500
 *
 * Revision 2.1  1995/06/07  20:33:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#undef FS_MAX_DEV
#define FS_MAX_DEV 4

struct rp_flash_rec_ {
    int         flags;
    flash_fhdr *ffh;
    flash_fhdr *dup_ffh;
};

typedef struct rp_flash_rec_ rp_flash_rec_t;

#define	RP_FLASH_DEV_NAME		"flash"
#define	FS_RP_FLASH_FILE_NAME_LEN	FS_FILE_NAME_MAX
#define	FS_RP_FLASH_DEV_NAME_LEN	FS_DEV_ID_MAX
#define	FS_RP_FLASH_OPEN_FILE_NUM	16 /* max number of opened files */
#define FLASH_DEV_NAME			RP_FLASH_DEV_NAME

#define RP_FS_REC_VALID		1
#define RP_FS_REC_INVALID	-1
#define FS_MAX_FILE_OPEN_FLASH   	FLASH_FILE_CNT
#define FS_MAX_FILE_NAME_LEN_FLASH	FLASH_NAME_LEN
#define FLASH_SLOT0_NAME_STR            "slot0 flash memory"

/* The following FFH_FLAGS_* should be in dev/flash_dvr_spec.h */
#define FFH_FLAGS_TYPE_UNKNOWN          0x0000
#define FFH_FLAGS_SYS_CONFIG            0x0002
#define FFH_FLAGS_SYS_IMAGE             0x0004
#define FFH_FLAGS_FILE_TYPE_FIELD \
		        (FFH_FLAGS_SYS_CONFIG | FFH_FLAGS_SYS_IMAGE)
#define NOT_RP_FLASH_DEVICE		1

int rp_fs_open_flash( fs_file_rec_t *frecp, int flags, uint mode);
int rp_fs_read_flash( fs_file_rec_t *frecp, void *buffer, int count);
int rp_fs_close_flash( fs_file_rec_t *frecp);
int rp_fs_write_flash( fs_file_rec_t *frecp, void *buffer, int count);
int rp_fs_delete_flash(fs_dev_t *devp, const char *file_name);
int rp_fs_fstat_flash (fs_file_rec_t *frecp, stat_t *status);

int rp_fs_fsstat_flash(fs_file_rec_t *frecp, file_sys_stat_t *status);
int rp_fs_fsstat_flash_dev(const char *dev_id, file_sys_stat_t *status);

int rp_fs_lseek_flash(int fd, int offset, int whence);
int rp_fs_stat_flash(const char *file_id, stat_t *status);
int rp_fs_undelete_flash(const char *dev_id, int index, int mode);
int rp_fs_squeeze_flash(const char *dev_id, int flag);
int rp_fs_settype_flash(const char *file_id, ulong file_type);
int rp_fs_getdents_flash(int fd, char *buffer, int buffer_length);

void rp_fs_init_flash(void);
ulong rp_calc_crc( uchar *addr, ulong length, boolean yield);

rp_flash_rec_t *rp_alloc_flash_rec(void);
void rp_free_flash_rec( rp_flash_rec_t *rpfsfp);
void rp_dir_flash(void);

