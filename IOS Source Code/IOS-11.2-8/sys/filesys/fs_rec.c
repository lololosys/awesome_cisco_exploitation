/* $Id: fs_rec.c,v 3.2.60.3 1996/09/12 18:25:20 dtaubert Exp $
 * $Source: /release/112/cvs/Xsys/filesys/fs_rec.c,v $
 *------------------------------------------------------------------
 * File record and common utilities for tftp/rcp/nvram filesys
 *
 * July 1995, Yan Ke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fs_rec.c,v $
 * Revision 3.2.60.3  1996/09/12  18:25:20  dtaubert
 * CSCdi50888:  Simultaenous copy tftp to slot0: and bootflash: crashes
 * router
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/09/07  02:07:34  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:35:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:35:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:49:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:07:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  01:31:32  yanke
 * Placeholder file.
 *
 * CSCdi39151:  modulized local/remote file systems
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>

#include "filesys.h"
#include "fs_rec.h"

static fs_file_rec_t *frec_link = NULL;
static int FSDEV_file_descriptor = 0;

/*
 * Find the file record with given file descriptor.
 */
fs_file_rec_t *get_file_rec_by_fd (fs_dev_t *devp, int fd)
{
    fs_file_rec_t *frecp;

    for (frecp = frec_link; frecp; frecp = frecp->next) {
	if (frecp->dev == devp && frecp->fd == fd)
	    return frecp;
    }
    return NULL;
}

/*
 * Find the file record with given file name.
 */
fs_file_rec_t *get_file_rec_by_name (fs_dev_t *devp, const char *file_name)
{
    fs_file_rec_t *frecp;

    for (frecp = frec_link; frecp; frecp = frecp->next) {
	if (frecp->dev == devp && strncmp(frecp->name, file_name,
	    FS_FILE_NAME_MAX) == 0)
	    return frecp;
    }
    return NULL;
}

/*
 * Allocate a new file record for the device.
 */
fs_file_rec_t *alloc_local_file_rec (fs_dev_t *devp, const char *file_name)
{
    fs_file_rec_t *frecp;

    frecp = malloc(sizeof(fs_file_rec_t));
    if (frecp) {
	memset(frecp, 0, sizeof(fs_file_rec_t));
	frecp->next = frec_link;
	frecp->dev = devp;
	frecp->fd = FSDEV_file_descriptor++;
	strncpy(frecp->name, file_name, FS_FILE_NAME_MAX);
	/* frecp->name[FS_FILE_NAME_MAX] = 0; by memset */
	frec_link = frecp;
    }
    return frecp;
}

/*
 * Free an allocated file record.
 */
void free_local_file_rec (fs_file_rec_t *frecp)
{
    fs_file_rec_t *frp;
    fs_file_rec_t *fpp = NULL;

    for (frp = frec_link; frp; frp = frp->next) {
	if (frp == frecp) {
	    if (fpp) {
		fpp->next = frp->next;
	    } else
		frec_link = frp->next;
	    free(frecp);
	    return;
	}
	fpp = frp;
    }
}

/*
 * Create a new file record for the caller, perform
 * necessary common validity chechings.
 */
fs_file_rec_t *common_create_file_rec (const char *file_id, fs_dev_t *devp,
	int *error)
{
    char file_name[FS_FILE_NAME_MAX + 1];
    fs_file_rec_t *frecp = NULL;

    fileid2filename(file_id, file_name, FS_FILE_NAME_MAX);

    if (devp->file_open_cnt >= devp->file_open_max) {
	*error = FS_DEV_OPEN_MANY;
	return NULL;
    }

    frecp = get_file_rec_by_name(devp, file_name);
    if (frecp) {
	*error = FS_DEV_OPEN_ALRDY;
	return NULL;
    }

    frecp = alloc_local_file_rec(devp, file_name);
    if (frecp == NULL) {
	*error = FS_DEV_NO_FS_REC;
	return NULL;
    }

    *error = 0;
    return frecp;
}

/*
 * TFTP device error text table.
 */
static char *const common_err_table[] =
{
    "Dummy error message",
    "Invalid flag",
    "Invalid file id",
    "No such device",
    "No more memory",
    "Invalid host address",
    "TFTP read error",
    "TFTP open socket error",
    "TFTP write error",
    "RCP read error",
    "NVRAM not available",
    "NVRAM can not open for write",
    "Invalid file descriptor",
    "File not opened",
    "TFTP internal error",
    "RCP internal error",
    "Can not create tasks",
    "File size invalid",
    "Task terminates with error",
    "RCP write error",
    "RCP close error",
    "File is being used",
    "NVRAM erase error",
    "Service not supported",
    "Invalid device id"
};

const char *common_geterrortext (int err_code)
{
    if (err_code < 0)
	err_code = -err_code;
    /*
     * Make sure it's not out of range.
     */
    if (0 <= err_code && err_code <= COMMON_ERR_TBL_MAX)
	return common_err_table[err_code];
    else
	return "Unknown error";
}
