/* $Id: fs_internals.h,v 3.2 1995/11/17 09:07:49 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/fs_internals.h,v $
 *------------------------------------------------------------------
 * File descriptor management table.
 *
 * February 1995, Yan Ke
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fs_internals.h,v $
 * Revision 3.2  1995/11/17  09:07:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:57:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/27  21:52:50  izhu
 * CSCdi41172:  7000 dir/show flash command not consistant w/ 7500
 *
 * Revision 2.1  1995/06/07  20:32:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	FS_FILE_DESP_MAX	32

#define	FS_DESP_ERR_CODE	-500
#define	FS_DESP_NO_FD		(FS_DESP_ERR_CODE - 0)	/* no such fd */
#define	FS_DESP_NO_TYPE		(FS_DESP_ERR_CODE - 1)	/* no such type */
#define	FS_DESP_NO_SPACE	(FS_DESP_ERR_CODE - 2)	/* table is full */
#define	FS_DESP_NO_MEM		(FS_DESP_ERR_CODE - 3)	/* no memory */
#define	FS_DESP_CIS_RD_ERR	(FS_DESP_ERR_CODE - 4)	/* cis read error */
#define	FS_DESP_UNK_MFG_ID	(FS_DESP_ERR_CODE - 5)	/* unknown mfg id */
#define	FS_DESP_FLSH_OP_ERR	(FS_DESP_ERR_CODE - 6)	/* flash card op err */
#define	FS_DESP_INV_DEV_ID	(FS_DESP_ERR_CODE - 7)	/* invalid dev id */
#define	FS_DESP_INT_ERR		(FS_DESP_ERR_CODE - 8)	/* internal error */
#define	FS_DESP_TBL_MAX		9

#define	FS_TYPE_FLASH	1
#define	FS_TYPE_NVRAM	2
#define	FS_TYPE_TFTP	3
#define	FS_TYPE_RCP	4
#define	FS_TYPE_MOP	5
#define	FS_TYPE_OTHERS	6
#define FS_TYPE_EMBEDDED_FLASH	7	/* for 7000 embedded flash */

typedef struct file_desp_table_
{
	int state;		/* for future use */
	int fd;			/* globally unique file descriptor */
	int type;		/* type of device */
	int native_fd;		/* raw file descriptor by each device */
} file_desp_table_t;
