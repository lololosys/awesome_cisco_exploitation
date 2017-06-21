/* $Id: nv_filesys.c,v 3.2.60.1 1996/03/18 19:35:54 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/nv_filesys.c,v $
 *------------------------------------------------------------------
 * NVRAM file system sub-system and initialization code
 *
 * July 1995, Yan Ke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nv_filesys.c,v $
 * Revision 3.2.60.1  1996/03/18  19:35:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:36:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:50:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:08:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  01:23:28  yanke
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
#include <ciscolib.h>
#include "subsys.h"
#include "config.h"
#include "../os/config_compress.h"
#include "../os/nv.h"

#include "filesys.h"
#include "filesys_registry.h"
#include "fs_rec.h"

/*
 * XXX - to be defined in h/globs.h and os/nv.h
 */
extern nvtype *nv_review_buffer_raw(void);
extern boolean nv_write_open_raw(void);
extern int nv_write_close_raw(boolean);
extern void nv_done_raw(nvtype *ptr);
extern int nv_erase_raw(boolean);

#define FS_NV_FILE_NAME_LEN	FS_FILE_NAME_MAX
#define	NV_DEV_NAME	"nvram"	/* nvram device name string */

#define NV_FILE_ID	"nvram:"
#define NV_PRIVATE_FILE_ID	"nvram:private"
#define NV_MATCH_PRIVATE_FILEID(ptr) (!strcmp(ptr, NV_PRIVATE_FILE_ID))

static boolean nv_print_enable = TRUE;
static boolean nv_private_config_access_enable = FALSE;
static boolean nv_writing_private_config_file = FALSE;

static fs_dev_t nv_dev_rec =
{
    IOCTL_DEV_TYPE_NV,			/* device type */
    FS_NV_FILE_NAME_LEN,		/* max file name len */
    1,					/* max open files */
    0,					/* current opened file cnt */
    NV_DEV_NAME,			/* pointer to device name string */
    NULL				/* device specific structure */
};

/*
 * Verify if the flag is valid.
 */
static boolean valid_file_flags (int flags)
{
    flags &= O_RW_MASK;
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
 * Create a new file record for tftp/rcp file.
 * Perform device specific checking on file id.
 */
static fs_file_rec_t *create_file_rec (const char *file_id, int flags,
	fs_dev_t *devp, int *retcode)
{
    fs_file_rec_t *frecp;

    /*
     * If the private configuration file was specified, verify that access
     * has been enabled first. If access had been enabled, continue but
     * first clear the enabled flag to minimize unexpected access.
     */
    if (NV_MATCH_PRIVATE_FILEID(file_id)) {
	if (!nv_private_config_access_enable) {
	    *retcode = FS_DEV_NOT_SUP;
	    return NULL;
	}
	nv_private_config_access_enable = FALSE;
    } else {
        if (get_file_name_by_file_id(file_id) != NULL) {
	    *retcode = FS_DEV_NOT_SUP;
	    return NULL;
    	}
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

/*
 * Open nvram.
 */
static int NV_open (const char *file_id, int flags, ulong mode)
{
    nvtype *nv;
    nvtype_private *priv_nv;
    fs_file_rec_t *frecp;
    int retcode;

    frecp = create_file_rec(file_id, flags, &nv_dev_rec, &retcode);
    if (frecp == NULL)
	return retcode;

    /*
     * The access check for the private config is done in create_file_rec()
     * above, so if we get here we're fine. But we need to mark whether or
     * not we're writing the config file.
     */
    if ((frecp->flag == OPEN_FOR_WRITE) && NV_MATCH_PRIVATE_FILEID(file_id)) {
	nv_writing_private_config_file = TRUE;
    } else {
	nv_writing_private_config_file = FALSE;
    }

    if ((frecp->flag == OPEN_FOR_READ) || nv_writing_private_config_file) {
	nv = nv_review_buffer_raw();
	if (nv == NULL) {
	    free_local_file_rec(frecp);
	    return FS_DEV_NO_NV;
	}
	frecp->size = nv->textsize;
    } else {
	if (nv_write_open_raw() == FALSE) {
	    free_local_file_rec(frecp);
	    return FS_DEV_NV_WRITE;
	}
	nv = nvptr;
	frecp->size = configbufsize;
    }

    frecp->base = nv->textbase;
    frecp->dev->dev_specific = (void *)nv;
    frecp->offset = 0;

    /*
     * If the filename is "private" then reset the base point to the private
     * configuration chain. Otherwise this is the public config which
     * starts at the beginning of NVRAM.
     */
    if (NV_MATCH_PRIVATE_FILEID(file_id)) {
	priv_nv = nv_private_getptr(nv);
	if (!priv_nv) {
	    free_local_file_rec(frecp);
	    return FS_DEV_NOT_OPEN;
	}
	if (nv_writing_private_config_file) {
	    /*
	     * Save a pointer to where the private config header should be
	     * written. It will be only be written in NV_write().
	     */
	    frecp->specific = (void *)priv_nv;
            frecp->base = (char *)priv_nv + sizeof(nvtype_private);
	    frecp->size = configbufsize -  ((char*)priv_nv - (char *)nv);
	} else {
	    /*
	     * If we are opening for reading and there is no private area,
	     * report it as an error.
	     */
	    if ((frecp->flag==OPEN_FOR_READ) && (priv_nv->magic!=NVMAGIC_PRIV)){
	        nv_done_raw(nv);
	        free_local_file_rec(frecp);
	        return FS_DEV_NOT_OPEN;
	    }
            frecp->base = priv_nv->priv_textbase;
	    frecp->size = priv_nv->priv_textsize;
	}
    } 

    return frecp->fd;
}

/*
 * Close the opened nvram.
 */
static int NV_close (int fd)
{
    fs_file_rec_t *frecp;
    nvtype *nv;
    nvtype_private *priv_nv;

    frecp = get_file_rec_by_fd(&nv_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    nv = (nvtype *)frecp->dev->dev_specific;
    if (nv_writing_private_config_file) {
	priv_nv = nv_private_getptr(nv);
	priv_nv->priv_textsize = frecp->offset;
	nv->checksum = 0;  
	nv->checksum = ipcrc((ushort *)nv,nvsize);
    }
    if ((frecp->flag == OPEN_FOR_READ) || nv_writing_private_config_file) {
	nv_done_raw(nv);
    } else {
	nv->textsize = frecp->offset;
	nv_write_close_raw(nv_print_enable);
    }
    free_local_file_rec(frecp);
    return 0;
}

/*
 * Read up to given number of bytes in the buffer from nvram.
 */
static int NV_read (int fd, void *buffer, int count)
{
    fs_file_rec_t *frecp;
    char *ptr;
    int offset;
    int remain;
    int cnt;

    frecp = get_file_rec_by_fd(&nv_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    offset = frecp->offset;
    remain = frecp->size - offset;

    if (remain == 0)
	return 0;	/* No more byte left to read */

    if (remain < count)
	cnt = remain;
    else
	cnt = count;

    ptr = frecp->base + frecp->offset;
    memcpy(buffer, ptr, cnt);
    frecp->offset += cnt;

    return cnt;
}

/*
 * Write bytes in the buffer to nvram.
 */
static int NV_write (int fd, void *buffer, int cnt)
{
    fs_file_rec_t *frecp;
    char *ptr;
    int offset;
    int remain;

    frecp = get_file_rec_by_fd(&nv_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    /*
     * If this is the first call when writing the private configuration first 
     * write the header structure. A pointer to where the header structure 
     * should start is in frecp->specific.
     */
    if (nv_writing_private_config_file && frecp->specific) {
	nv_build_private_config_header(frecp->specific, 
				       (nvtype_private *)frecp->specific);
	frecp->specific = NULL;
    }

    offset = frecp->offset;
    remain = frecp->size - offset;

    if (remain == 0)
	return 0;	/* No more room for further write */
    cnt = (remain < cnt) ? remain : cnt;

    ptr = frecp->base + frecp->offset;
    memcpy(ptr, buffer, cnt);
    frecp->offset += cnt;
    return cnt;
}

/*
 * Erase the nvram. Only allow a full erasure.
 */
static int NV_delete (const char *file_id)
{
    int retcode;

    if (strcmp(file_id, NV_FILE_ID)) {
	return FS_DEV_NOT_SUP;
    }
    retcode = nv_erase_raw(nv_print_enable);
    if (retcode < 0)
	return FS_DEV_DEL_NV;
    else
	return 0;
}

/*
 * For now, just return the size of the nvram content.
 */
static int NV_fstat (int fd, stat_t *status)
{
    fs_file_rec_t *frecp;

    frecp = get_file_rec_by_fd(&nv_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FILE_ID;

    status->st_size = frecp->size;
    return 0;
}

/*
 * Return all devices in this class
 */
static int NV_getdevicenames(char *buffer, int buffer_len)
{
    if (buffer_len > strlen(NV_DEV_NAME)) {
	strcpy(buffer, NV_DEV_NAME);
	return 0;
    }
    return FS_DEV_NV_INT;
}

/*
 * Return error text string by code.
 */
static void NV_geterrortext (int err_code, char *buffer, int blen)
{
    const char *msg;

    msg = common_geterrortext(err_code);
    strcpy(buffer, (char *)msg);
}

/*
 * Generic routine to set/get file attribes by file id.
 */
static int NV_ioctl (const char *file_id, int func, void *arg)
{
    int retcode = 0;
    uint dev_type = IOCTL_DEV_TYPE_LOCAL;

    switch (func) {
	case IOCTL_GET_DEV_TYPE:
	    dev_type |= IOCTL_DEV_TYPE_NV;
	    *(uint *)arg = dev_type;
	    break;

	case IOCTL_SET_VERBOSE:
	    nv_print_enable = (boolean)arg;
	    break;

	case IOCTL_SET_PRIVATE_RW:
	    if (NV_MATCH_PRIVATE_FILEID(file_id)) {
	    	nv_private_config_access_enable = (boolean)arg;
	    } else {
		retcode = FS_DEV_INV_FILE_ID;
	    }
	    break;

	default:
	    retcode = FS_DEV_INV_OP;
    }
    return retcode;
}

/*
 * Generic routine to set/get file attribes by file id.
 */
static int NV_fioctl (int fd, int func, void *arg)
{
    return 0;
}

/*
 * The following interface routines are not supported.
 */
static int NV_fsstat (const char *dev_id, file_sys_stat_t *status)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Seek to position in file.
 */
static int NV_lseek (int fd, int offset, int whence)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Return information of a given file.
 */
static int NV_stat (const char *file_id, stat_t *status)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Return information of a given file.
 */
static int NV_istat (const char *dev_id, int index, stat_t *status)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Undelete a file.
 */
static int NV_undelete (const char *dev_id, int index, int mode)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Squeeze the device.
 */
static int NV_squeeze (const char *dev_id, int flag)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Set file type.
 */
static int NV_settype (const char *file_id, ulong file_type)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Return file diretory entry.
 */
static int NV_getdents (int fd, char *buffer, int buffer_length)
{
    return FS_DEV_NOT_SUP;
}

/*
 * nvram subsystem init routine.
 */
static void nv_subsys_init (subsystype* subsys)
{
    int file_sys_type;
    char device_name[FS_DEV_NAMES_MAX];
    int error;

    /*
     * register with the tftp file system layer
     * and get the file_sys_type to use.
     */
    error = (int)NV_getdevicenames(device_name, FS_DEV_NAMES_MAX);
    file_sys_type = FS_register(device_name, FS_STD_ERROR_CODE_ALLOCATION);

    /*
     * add all the client file system routines to the registries
     */
    reg_add_open(file_sys_type, NV_open, "NV_open");
    reg_add_close(file_sys_type, NV_close, "NV_close");
    reg_add_read(file_sys_type, NV_read, "NV_read");
    reg_add_write(file_sys_type, NV_write, "NV_write");
    reg_add_lseek(file_sys_type, NV_lseek, "NV_lseek");
    reg_add_stat(file_sys_type, NV_stat, "NV_stat");
    reg_add_fstat(file_sys_type, NV_fstat, "NV_fstat");
    reg_add_istat(file_sys_type, NV_istat, "NV_istat");
    reg_add_fsstat(file_sys_type, NV_fsstat, "NV_fsstat");
    reg_add_delete(file_sys_type, NV_delete, "NV_delete");
    reg_add_undelete(file_sys_type, NV_undelete, "NV_undelete");
    reg_add_squeeze(file_sys_type, NV_squeeze, "NV_squeeze");
    reg_add_settype(file_sys_type, NV_settype, "NV_settype");
    reg_add_getdents(file_sys_type, NV_getdents, "NV_getdents");
    /*
    reg_add_format(file_sys_type, NV_format, "NV_format");
    */
    reg_add_geterrortext(file_sys_type, NV_geterrortext, "NV_geterrortext");
    reg_add_getdevicenames(file_sys_type,
	NV_getdevicenames, "NV_getdevicenames");
    reg_add_ioctl(file_sys_type, NV_ioctl, "NV_ioctl");
    reg_add_fioctl(file_sys_type, NV_fioctl, "NV_fioctl");
}

/*
 * Subsystem header for the nvram file system
 */
SUBSYS_HEADER(nv_file_system,   /* name */
    1,				/* major version */
    0,				/* minor version */
    1,				/* edit version */
    nv_subsys_init,		/* init function */
    SUBSYS_CLASS_DRIVER,	/* subsystem class */
    "seq: file_system",		/* sequence list */
    "req: file_system"		/* prerequisite list */
);
