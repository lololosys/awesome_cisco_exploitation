/* $Id: tftp_rcp_filesys.c,v 3.4.22.3 1996/09/06 21:00:48 snyder Exp $
 * $Source: /release/112/cvs/Xsys/filesys/tftp_rcp_filesys.c,v $
 *------------------------------------------------------------------
 * TFTP/RCP file system sub-system and initialization code
 *
 * July 1995, Yan Ke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tftp_rcp_filesys.c,v $
 * Revision 3.4.22.3  1996/09/06  21:00:48  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.4.22.2  1996/06/28  23:06:02  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.22.1  1996/03/18  19:36:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  09:36:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  00:48:01  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/01/24  20:13:22  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.3  1995/11/17  09:09:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:23:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:33:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:37  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  01:28:34  yanke
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
#include "interface.h"
#include "subsys.h"
#include "address.h"
#include "ttysrv.h"
#include "name.h"
#include "../os/boot.h"
#include "file.h"
#include "packet.h"
#include "../ip/ip.h"
#include "interface_private.h"
#include "../ip/ip_registry.h"
#include "../h/sys_registry.h"

#include "filesys.h"
#include "filesys_registry.h"
#include "fs_rec.h"

/****************************************************************************
 *
 * Shared between tftp and rcp.
 *
 ****************************************************************************/

#define	REMOTE_SERVER_ACCESS_TIMEOUT	10*ONESEC

#define	TFTP_RCP_DEV_GO		1
#define	TFTP_RCP_DEV_DONE	2
#define	TFTP_RCP_DEV_ERROR	3
#define	TFTP_RCP_DEV_TIMEOUT	4

typedef struct remote_file_blk_
{
    uint control_flag;	/* sync between user task and tftp task */
    char *buffer;	/* pointer to buffer that holds tftp data */
    int size;		/* size of buffer */
    int len;		/* length of the valid data in buffer */
    int fs_offset;	/* offset in the file of the valid data */
    char *rtbuf;	/* pointer to user provided buffer */
    int count;		/* the number of available bytes in rtbuf */
    int request;	/* how many bytes requested by user */
    int pid;		/* pid of the tftp process */
    sys_timestamp time; /* record timeout */
    int blknum;		/* block number for write */
    int file_size;	/* byte count of the file */
    ipsocktype *soc;	/* tftp socket for write */
    addrtype addr;	/* address of the tftp server */
} remote_file_blk_t;

/*
 * Ask user for tftp server address.
 */
static boolean dev_get_server (addrtype *host, char *buffer,
    int len, boolean bcast_ok)
{
    nametype *ptr;
    int dummy;
    boolean retcode;

    if (bcast_ok == FALSE && host->ip_addr == -1)
	printf("\nAddress or name of remote host []? ");
    else
	printf("\nAddress or name of remote host [%s]? ",
	    name_addr2string(host));
    retcode = rdtty(buffer, len);
    if (retcode == FALSE) {
	printf("\n?Bad address or host name");
	return FALSE;
    }
    flush();

    if (!null(buffer)) {
	if (numeric_lookup(ADDR_IP, buffer, host) == 0) {
	    ptr = name_lookup(buffer, &dummy, NAM_IP);
	    if (ptr)
		address_copy(host, &ptr->v.ip.adr[0]);
	    else {
		printf("\nERR: Invalid host address or name");
		return FALSE;
	    }
	}
    } else {
	if (bcast_ok == FALSE && host->ip_addr == -1) {
	    printf("\n?Bad address or host name");
	    return FALSE;
	}
    }
    return TRUE;
}

/*
 * Routine to decide if we can pull tftp/rcp process
 * out of suspend mode. Return TRUE to continue suspend.
 */
static boolean server_dev_sleep (void *arg)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *fbp;

    frecp = (fs_file_rec_t *)arg;
    fbp = (remote_file_blk_t *)frecp->specific;

    if (fbp->control_flag == TFTP_RCP_DEV_GO) {
	/*
	 * Wake up to get more data. Refresh
	 * the timeout timer.
	 */
	GET_TIMESTAMP(fbp->time);
	return FALSE;
    }

    if (TIMER_RUNNING(fbp->time)) {
	if (ELAPSED_TIME(fbp->time) >= REMOTE_SERVER_ACCESS_TIMEOUT) {
	    /*
	     * We have timeout, stop waiting
	     * and kill this process. If we don't,
	     * server will timeout anyway.
	     */
	    fbp->control_flag = TFTP_RCP_DEV_TIMEOUT;
	    return FALSE;
	}
    } else {
	/*
	 * First time call, click the time.
	 */
	GET_TIMESTAMP(fbp->time);
    }
    /*
     * More wait.
     */
    return TRUE;
}

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
    const char *file_name;
    fs_file_rec_t *frecp;

    file_name = get_file_name_by_file_id(file_id);
    if (file_name == NULL || strcmp(file_name, ".") == 0) {
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

/****************************************************************************
 *
 * Tftp device interface routines and utilities
 *
 ****************************************************************************/

/*
 * file id/device id maximum length.
 */
#define	FS_TFTP_FILE_NAME_LEN	FS_FILE_NAME_MAX
#define	FS_TFTP_DEV_NAME_LEN	FS_DEV_ID_MAX

#define	FS_TFTP_OPEN_FILE_NUM	16	/* max number of opened files */
#define	TFTP_SERVER_NAME_LEN	64	/* tftp server name len */
#define	TFTP_DATA_BUFFER_SIZE	2048	/* tftp download data buf size */

#define	TFTP_DEV_NAME		"tftp"	/* tftp device name string */

static fs_dev_t tftp_dev_rec =
{
    IOCTL_DEV_TYPE_TFTP,		/* device type */
    FS_TFTP_FILE_NAME_LEN,		/* max file name len */
    FS_TFTP_OPEN_FILE_NUM,		/* max open files */
    0,					/* current opened file cnt */
    TFTP_DEV_NAME,			/* pointer to device name string */
    NULL				/* device specific structure */
};

static addrtype tftp_server_addr;	/* default tftp server address */

/*
 * Callback routine from tftp_read. This one
 * is for testing only.
 */
static boolean tftp_callback_test (uchar **buf, long *left, long len, void *arg)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *tfbp;

    /*
     *	Adjust buffer/size, accumulate byte count,
     *	and let tftp procedure to finish.
     */
    frecp = (fs_file_rec_t *)arg;
    tfbp = (remote_file_blk_t *)frecp->specific;
    *buf = tfbp->buffer;
    *left = tfbp->size;
    /*
     * TFTP may receive an error packet. If we are
     * called back in this case, it would mean
     * failure (and 'len' would be 0).
     */
    if (len == 0)
	tfbp->control_flag = TFTP_RCP_DEV_ERROR;
    return FALSE;
}

/*
 * Open indicated file on tftp device.
 */
static int TFTP_open (const char *file_id, int flags, ulong mode)
{
    fs_file_rec_t *frecp;
    int retcode;
    char *bufp;
    remote_file_blk_t *tfbp;
    ipsocktype *soc;
    boolean verbose_flag;
    filetype confg;
    int bufsize;
    char srv_name[TFTP_SERVER_NAME_LEN + 1];

    frecp = create_file_rec(file_id, flags, &tftp_dev_rec, &retcode);
    if (frecp == NULL)
	return retcode;

    /*
     * Allocate tftp download block.
     */
    tfbp = (remote_file_blk_t *)frecp->specific;
    if (tfbp == NULL) {
	tfbp = malloc(sizeof(remote_file_blk_t));
	if (tfbp == NULL) {
	    free_local_file_rec(frecp);
	    return FS_DEV_NO_MEM;
	}
	memset(tfbp, 0, sizeof(remote_file_blk_t));
	frecp->specific = (void *)tfbp;
    }

    /*
     * Allocate the data buffer used for down/up/loading.
     */
    bufp = tfbp->buffer;
    if (bufp == NULL) {
	bufp = malloc(TFTP_DATA_BUFFER_SIZE);
	if (bufp == NULL) {
	    free(tfbp);
	    free_local_file_rec(frecp);
	    return FS_DEV_NO_MEM;
	}
	tfbp->buffer = bufp;
	tfbp->size = TFTP_DATA_BUFFER_SIZE;
    }

    /*
     * Get the tftp server address from user.
     * and save it as new default.
     */
    address_copy(&tfbp->addr, &tftp_server_addr);
    if (dev_get_server(&tfbp->addr, srv_name, TFTP_SERVER_NAME_LEN,
	frecp->flag == OPEN_FOR_READ) == FALSE)
    {
	free(bufp);
	free(tfbp);
	free_local_file_rec(frecp);
	return FS_DEV_INV_ADDR;
    }
    address_copy(&tftp_server_addr, &tfbp->addr);

    memset(&confg, 0, sizeof(filetype));

    if (frecp->flag == OPEN_FOR_READ) {

	/*
	 * Download the file to check if it's
	 * a valid server and file.
	 * Turn off verbose if not in debug mode.
	 */
	tfbp->file_size = 0;
	verbose_flag = FALSE;

	confg.filename = &frecp->name[0];
	address_copy(&confg.ps.tuba.boothost, &tfbp->addr);
	confg.proto = FILE_ACCESS_TFTP;

	printf("\nAccessing file \"%s\" on %s ...",
	    confg.filename, name_addr2string(&tfbp->addr));
	flush();

	bufsize = tfbp->size;
	tfbp->control_flag = 0;
	retcode = reg_invoke_file_read(confg.proto, &confg,
	    (uchar *)tfbp->buffer, (ulong *)&bufsize,
	    verbose_flag, tftp_callback_test, (void *)frecp);

	if ((retcode != ERROR_OK && retcode != ERROR_ABORT) ||
	    tfbp->control_flag == TFTP_RCP_DEV_ERROR)
	{
	    free(bufp);
	    free(tfbp);
	    free_local_file_rec(frecp);
	    printf("NOT FOUND");
	    flush();
	    return FS_DEV_TFTP_READ;
	}

        printf("FOUND");
        flush();

	/*
	 * Initialize records. If there is a process
	 * hanging, kill it.
	 */
	frecp->base = 0;
	frecp->offset = 0;
	tfbp->pid = -1;

    } else if (frecp->flag == OPEN_FOR_WRITE) {
	soc = reg_invoke_ip_tftp_open_socket(&tfbp->addr, SOCK_ANYSRC);
	if (soc == 0) {
	    free(bufp);
	    free(tfbp);
	    free_local_file_rec(frecp);
	    return FS_DEV_TFTP_SOCK;
	}

	retcode = reg_invoke_ip_tftp_WRQ(soc, frecp->name, "octet", FALSE);
	if (retcode != TRUE) {
	    reg_invoke_ip_close_socket(soc);
	    free(bufp);
	    free(tfbp);
	    free_local_file_rec(frecp);
	    return FS_DEV_TFTP_WRITE;
	}

	tfbp->soc = soc;
	tfbp->blknum = 1;
	tfbp->pid = -1;

    } else {
	free(bufp);
	free(tfbp);
	free_local_file_rec(frecp);
	return FS_DEV_INV_FLAG;
    }

    return frecp->fd;
}

/*
 *	Callback routine to process the received tftp data.
 */
static boolean tftp_callback_proc_data (uchar **buf,
    long *left, long len, void *arg)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *tfbp;

    frecp = (fs_file_rec_t *)arg;
    tfbp = (remote_file_blk_t *)frecp->specific;
    tfbp->len = len;

    /*
     * Tell user process we've got a data buffer.
     */
    tfbp->control_flag = TFTP_RCP_DEV_DONE;

    if (len < TFTP_MAXDATA)
	return TRUE;	/* let tftp terminate */

    /*
     *	Pause the tftp progress and wait for
     *	instruction from user process, or timeout.
     */
    edisms((blockproc *)server_dev_sleep, (ulong)arg);

    if (tfbp->control_flag == TFTP_RCP_DEV_TIMEOUT) {
	/*
	 * Timeout, abort tftp procdure.
	 */
	return FALSE;
    }

    /*
     * Go back to tftp to fetch next data buffer.
     */
    tfbp->fs_offset += len;
    *buf = tfbp->buffer;
    *left = tfbp->size;
    return TRUE;
}

/*
 * Tftp process driven by the user process.
 */
static forktype tftp_dev_proc (fs_file_rec_t* frecp)
{
    remote_file_blk_t *tfbp;
    int retcode;
    filetype confg;

    tfbp = (remote_file_blk_t *)frecp->specific;

    memset(&confg, 0, sizeof(filetype));
    confg.filename = &frecp->name[0];
    confg.persistence = TRUE;
    address_copy(&confg.ps.tuba.boothost, &tfbp->addr);
    confg.proto = FILE_ACCESS_TFTP;

    retcode = reg_invoke_file_read(confg.proto, &confg,
	(uchar *)tfbp->buffer, (ulong *)&tfbp->size,
	TRUE, tftp_callback_proc_data, (void *)frecp);

    if (retcode == ERROR_OK)
	tfbp->control_flag = TFTP_RCP_DEV_DONE;
    else {
	/*
	 * It's not stopped by us and it's not last pkt.
	 * Something is wrong.
	 */
	printf("\nTFTP failed: code = %d", retcode);
	tfbp->control_flag = TFTP_RCP_DEV_ERROR;
    }
    tfbp->pid = -2;
    process_kill(-1);
}

/*
 * Examin the data in the current tftp buffer and see
 * if it's what user wants, return the number of bytes
 * handed to user. Return 0 for more data to be tftp downloaded.
 * If error occurs, return -1.
 */
static int get_server_data (fs_file_rec_t *frecp, remote_file_blk_t *fbp)
{
    int len;
    char *ptr;
    char *bufp;
    int cnt;
    int m;

    if (fbp->buffer == NULL)
	return -1;

    if (frecp->offset < fbp->fs_offset) {
	/*
	 * buffer passed the offset, should
	 * not happen.
	 */
	return -1;
    }

    if (frecp->offset >= fbp->fs_offset &&
	frecp->offset < fbp->fs_offset + fbp->len) {

	bufp = fbp->rtbuf + fbp->count;	/* start loc to fill new data */

	len = frecp->offset - fbp->fs_offset;
	ptr = fbp->buffer + len;	/* data to fetch next */

	cnt = fbp->request;		/* bytes left unfilled */
	len = fbp->len - len;		/* bytes left unfetched */

	m = (cnt <= len) ? cnt : len;	/* how many to fetch */

	memcpy(bufp, ptr, m);		/* copy data and update */
	frecp->offset += m;
	fbp->count += m;
	fbp->request -= m;

	if (fbp->request == 0) {
	    /*
	     * All we need is in buffer.
	     */
	    return cnt;
	}
    }
    /*
     * All data in current buffer are fetched.
     * And we need more.
     */
    return 0;
}

/*
 * Read up to given number of bytes in the buffer from
 * the indicated file.
 */
static int TFTP_read (int fd, void *buffer, int cnt)
{
    fs_file_rec_t *frecp;
    int pid;
    remote_file_blk_t *tfbp;
    int retcode;

    frecp = get_file_rec_by_fd(&tftp_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    if (frecp->flag != OPEN_FOR_READ)
	return FS_DEV_NOT_OPEN;

    tfbp = (remote_file_blk_t *)frecp->specific;
    if (tfbp == NULL)
	return FS_DEV_TFTP_INT;
    tfbp->rtbuf = buffer;
    tfbp->request = cnt;
    tfbp->count = 0;

    if (tfbp->pid == -1) {
	/*
	 * Process is not created, or has terminated.
	 * Create it with default stack size, no terminal.
	 */
	tfbp->control_flag = TFTP_RCP_DEV_GO;
	pid = cfork((forkproc *)tftp_dev_proc, (long)frecp, 0,
	    "tftp dev", stdio->ttynum);
	if (pid < 0)
	    return FS_DEV_FORK_ERR;
	tfbp->pid = pid;
    }

    /*
     *	Loop until we get enough data as requested,
     *	or tftp proc has terminated.
     */
    for (;;) {

	while (tfbp->control_flag == TFTP_RCP_DEV_GO) {
	    process_suspend();
	}

	if (tfbp->control_flag != TFTP_RCP_DEV_DONE) {
	    printf("\nTFTP end abnormally (%d)", tfbp->control_flag);
	    return FS_DEV_TFTP_READ;
	}

	retcode = get_server_data(frecp, tfbp);
	if (retcode < 0) {
	    /*
	     * An error has occured.
	     */
	    printf("\nGet server data error (%d)", retcode);
	    return FS_DEV_TFTP_READ;
	}
	else if (retcode > 0) {
	    /*
	     * Got what we need.
	     */
	    return tfbp->count;
	}

	/*
	 * Need to start tftp (if alive) to get more data.
	 * Let go the tftp task.
	 */
	if (tfbp->pid >= 0) {
	    tfbp->control_flag = TFTP_RCP_DEV_GO;
	} else {
	    /*
	     * tftp process has stopped either because this
	     * is last data buffer, or an error occured.
	     */
	    return tfbp->count;
	}
    }
}

/*
 * Close the opened file.
 */
static int TFTP_close (int fd)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *tfbp;

    frecp = get_file_rec_by_fd(&tftp_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    tfbp = (remote_file_blk_t *)frecp->specific;
    if (tfbp) {
	if (frecp->flag == OPEN_FOR_WRITE) {
	    /*
	     * We need to finish off the leftovers
	     * even if it's none.
	     */
	    reg_invoke_ip_tftp_sendblock(tfbp->soc,
		tfbp->blknum, tfbp->buffer, tfbp->len, TRUE);
	}
	/*
	 * Now free everything and cleanup.
	 */
	if (tfbp->soc)
	    reg_invoke_ip_close_socket(tfbp->soc);

	if (tfbp->buffer)
	    free(tfbp->buffer);

	/*
	 * Instead of release the suspended process
	 * and let it close the socket and terminates,
	 * we simply kill the process and close the
	 * socket ourselves.
	 */
	if (tfbp->pid >= 0)
	    process_kill(tfbp->pid);

	free(tfbp);
	frecp->specific = NULL;
    }

    free_local_file_rec(frecp);
    return 0;
}

/*
 * Write bytes in the buffer to the indicated file.
 */
static int TFTP_write (int fd, void *buffer, int cnt)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *tfbp;
    boolean result;
    char *ptr;
    int left;
    int total = 0;

    frecp = get_file_rec_by_fd(&tftp_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    if (frecp->flag != OPEN_FOR_WRITE)
	return FS_DEV_NOT_OPEN;

    tfbp = (remote_file_blk_t *)frecp->specific;
    /*
     * We may have leftover data from last time's write.
     */
    ptr = tfbp->buffer + tfbp->len;	/* start of new data */
    left = TFTP_MAXDATA - tfbp->len;	/* need this much to send */

    /*
     *	Keep send data to tftp server until we don't
     *	have enough data for a max size packet.
     */
    while (cnt >= left) {
	/*
	 * Build a max size data buffer then
	 * send it to tftp.
	 */
	memcpy(ptr, buffer, left);
	result = reg_invoke_ip_tftp_sendblock(tfbp->soc,
	    tfbp->blknum, tfbp->buffer, TFTP_MAXDATA, TRUE);
	if (result != TRUE)
	    return FS_DEV_TFTP_WRITE;

	buffer = (void *)((char *)buffer + left);
	cnt -= left;
	total += left;

	ptr = tfbp->buffer;
	tfbp->len = 0;
	left = TFTP_MAXDATA;
	tfbp->blknum++;
    }

    /*
     * Save the leftover for next time.
     */
    if (cnt) {
	memcpy(ptr, buffer, cnt);
	tfbp->len += cnt;
	total += cnt;
    } else
	tfbp->len = 0;

    return total;
}

/*
 * Return information of a given file.
 */
static int TFTP_fstat (int fd, stat_t *status)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *tfbp;

    frecp = get_file_rec_by_fd(&tftp_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FILE_ID;

    tfbp = (remote_file_blk_t *)frecp->specific;
    if (tfbp == NULL)
	return FS_DEV_TFTP_INT;

    status->st_size = tfbp->file_size;
    status->server_addr = &tfbp->addr;
    return 0;
}

/*
 * Return all devices in this class
 */
static int TFTP_getdevicenames(char *buffer, int buffer_len)
{
    if (buffer_len > strlen(TFTP_DEV_NAME)) {
	strcpy(buffer, TFTP_DEV_NAME);
	return 0;
    }
    return FS_DEV_TFTP_INT;
}

/*
 * Return error text string by code.
 */
static void TFTP_geterrortext (int err_code, char *buffer, int blen)
{
    const char *msg;

    msg = common_geterrortext(err_code);
    strcpy(buffer, (char *)msg);
}

/*
 * Generic routine to set/get file attribes by file id.
 */
static int TFTP_ioctl (const char *file_id, int func, void *arg)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *tfbp;
    int retcode = 0;
    const char *file_name;

    switch (func) {
	case IOCTL_GET_DEV_TYPE:
	    *(uint *)arg = IOCTL_DEV_TYPE_TFTP;
	    break;

	case IOCTL_SET_FILE_SIZE:
	    file_name = get_file_name_by_file_id(file_id);
	    if (file_name) {
		frecp = get_file_rec_by_name(&tftp_dev_rec, file_name);
		if (frecp) {
		    tfbp = (remote_file_blk_t *)frecp->specific;
		    if (tfbp) {
			tfbp->file_size = (int)arg;
			break;
		    }
		}
	    }
	    retcode = FS_DEV_INV_FILE_ID;
	    break;

	default:
	    retcode = FS_DEV_INV_OP;
    }
    return retcode;
}

/*
 * Generic routine to set/get file attribes by file id.
 */
static int TFTP_fioctl (int fd, int func, void *arg)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *tfbp;
    int retcode = 0;

    switch (func) {
	case IOCTL_GET_DEV_TYPE:
	    *(uint *)arg = IOCTL_DEV_TYPE_TFTP;
	    break;

	case IOCTL_SET_FILE_SIZE:
	    frecp = get_file_rec_by_fd(&tftp_dev_rec, fd);
	    if (frecp) {
		tfbp = (remote_file_blk_t *)frecp->specific;
		if (tfbp) {
		    tfbp->file_size = (int)arg;
		    break;
		}
	    }
	    retcode = FS_DEV_INV_FD;
	    break;

	default:
	    retcode = FS_DEV_INV_OP;
    }
    return retcode;
}

/*
 * The following interface routines are not supported.
 */
static int TFTP_delete (const char *file_id)
{
    return FS_DEV_NOT_SUP;
}

static int TFTP_fsstat (const char *dev_id, file_sys_stat_t *status)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Seek to position in remote file.
 * Requirements:
 *     File must be opened READ only
 *     The resultant offset must be within the current remote read buffer.
 */
static int TFTP_lseek (int fd, int offset, int whence)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *tfbp;

    int curr_ptr;

    frecp = get_file_rec_by_fd(&tftp_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    if (frecp->flag != OPEN_FOR_READ)
	return FS_DEV_NOT_OPEN;

    tfbp = (remote_file_blk_t *)frecp->specific;
    if (tfbp) {
	switch (whence) {
	  case SEEK_SET:
	    curr_ptr = offset;
	    break;
	  case SEEK_CUR:
	    curr_ptr = frecp->offset + offset;
	    break;
	  case SEEK_END:
	    curr_ptr = tfbp->file_size + offset;
	    break;
	  default:
	    return FS_DEV_INV_WHENCE;
	}

	/* 
	 * now make sure that we have a reasonable curr_ptr
	 */
	if ( (curr_ptr < tfbp->fs_offset) || 
	     (curr_ptr >= tfbp->fs_offset + tfbp->len))
	     return FS_DEV_INV_LSEEK;

	frecp->offset = curr_ptr;;
	return 0;
    }
    else
	return FS_DEV_TFTP_INT;

}

/*
 * Return information of a given file.
 */
static int TFTP_stat (const char *file_id, stat_t *status)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Return information of a given file.
 */
static int TFTP_istat (const char *dev_id, int index, stat_t *status)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Undelete a file.
 */
static int TFTP_undelete (const char *dev_id, int index, int mode)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Squeeze the device.
 */
static int TFTP_squeeze (const char *dev_id, int flag)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Set file type.
 */
static int TFTP_settype (const char *file_id, ulong file_type)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Return file diretory entry.
 */
static int TFTP_getdents (int fd, char *buffer, int buffer_length)
{
    return FS_DEV_NOT_SUP;
}

/****************************************************************************
 *
 * RCP device interface routines and utilities
 *
 ****************************************************************************/

/*
 * file id/device id maximum length.
 */
#define	FS_RCP_FILE_NAME_LEN	FS_FILE_NAME_MAX
#define	FS_RCP_DEV_NAME_LEN	FS_DEV_ID_MAX

#define	FS_RCP_OPEN_FILE_NUM	16	/* max number of opened files */
#define	RCP_SERVER_NAME_LEN	64	/* rcp server name len */
#define	RCP_DATA_BUFFER_SIZE	1024	/* rcp data buffer size */

#define	RCP_DEV_NAME		"rcp"	/* rcp device name string */

static fs_dev_t rcp_dev_rec = {
    IOCTL_DEV_TYPE_RCP,			/* device type */
    FS_RCP_FILE_NAME_LEN,		/* max file name len */
    FS_RCP_OPEN_FILE_NUM,		/* max open files */
    0,					/* current opened file cnt */
    RCP_DEV_NAME,			/* device name string */
    NULL				/* device specific structure */
};

/*
 * Store default rcp server address.
 * Updated each time when rcp device is opened.
 */
static addrtype rcp_server_addr;

/*
 * Callback routine from rcp_read. This one
 * is for testing only.
 */
static boolean rcp_callback_test (uchar **buf, long *left, long len, void *arg)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *rcbp;

    /*
     * Set the flag, save the size, then return FALSE
     * to abort rcp procedure.
     */
    frecp = (fs_file_rec_t *)arg;
    rcbp = (remote_file_blk_t *)frecp->specific;
    rcbp->control_flag = 1;
    if (rcbp->file_size < 0)
	rcbp->file_size = *left;	/* file size */

    *buf = rcbp->buffer;
    return FALSE;
}

/*
 * Callback routine to fetch more data to write to
 * remote file on rcp server.
 */
static long rcp_callback_fetch_data (uchar **buf, long len, void *arg)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *rcbp;
    int chunk;

    frecp = (fs_file_rec_t *)arg;
    rcbp = (remote_file_blk_t *)frecp->specific;
    if (rcbp) {
	if (rcbp->size == 0) {
	    /*
	     * No more data availabel to fetch, indicate that
	     * we are done with current buffer, block
	     * the process, wake up on next write or timeout.
	     */
	    rcbp->control_flag = TFTP_RCP_DEV_DONE;
	    edisms((blockproc *)server_dev_sleep, (ulong)arg);

	    if (rcbp->control_flag != TFTP_RCP_DEV_GO)
		return 0;	/* timeout */
	}
	/*
	 * Fetch data and return.
	 */
	*buf = rcbp->rtbuf;
	chunk = (len <= rcbp->size) ? len : rcbp->size;
	rcbp->size -= chunk;
	rcbp->rtbuf += chunk;
	return chunk;
    }
    return 0;
}

/*
 * Process that controles rcp upload procedure.
 */
static forktype rcp_dev_proc_write (fs_file_rec_t* frecp)
{
    remote_file_blk_t *rcbp;
    boolean retcode;

    rcbp = (remote_file_blk_t *)frecp->specific;
    if (rcbp == NULL) {
	rcbp->control_flag = TFTP_RCP_DEV_ERROR;
	rcbp->pid = -1;
	process_kill(-1);
    }

    retcode = reg_invoke_rcp_write(frecp->name, &rcbp->addr, rcbp->buffer,
	rcbp->file_size, NULL, rcp_callback_fetch_data, (void *)frecp);

    /*
     * Update the flag to indicate the cause of termination.
     */
    if (retcode == TRUE)
	rcbp->control_flag = TFTP_RCP_DEV_DONE;
    else
	rcbp->control_flag = TFTP_RCP_DEV_ERROR;

    rcbp->pid = -2;
    process_kill(-1);
}

/*
 * Open file on RCP device.
 */
static int RCP_open (const char *file_id, int flags, ulong mode)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *rcbp;
    int retcode;
    char *bufp;
    filetype confg;
    int size;
    char srv_name[RCP_SERVER_NAME_LEN + 1];

    frecp = create_file_rec(file_id, flags, &rcp_dev_rec, &retcode);
    if (frecp == NULL)
	return retcode;

    /*
     * Allocate rcp download block.
     */
    rcbp = (remote_file_blk_t *)frecp->specific;
    if (rcbp == NULL) {
	rcbp = malloc(sizeof(remote_file_blk_t));
	if (rcbp == NULL) {
	    free_local_file_rec(frecp);
	    return FS_DEV_NO_MEM;
	}
	memset(rcbp, 0, sizeof(remote_file_blk_t));
	frecp->specific = (void *)rcbp;
    }

    /*
     * Get the rcp server address from user.
     * and save it as new default.
     */
    address_copy(&rcbp->addr, &rcp_server_addr);
    if (dev_get_server(&rcbp->addr, srv_name, RCP_SERVER_NAME_LEN,
	frecp->flag == OPEN_FOR_READ) == FALSE)
    {
	free(rcbp);
	free_local_file_rec(frecp);
	return FS_DEV_INV_ADDR;
    }
    address_copy(&rcp_server_addr, &rcbp->addr);

    memset(&confg, 0, sizeof(filetype));

    if (frecp->flag == OPEN_FOR_READ) {
	/*
	 * Allocate the buffer used for downloading.
	 */
	bufp = rcbp->buffer;
	if (bufp == NULL) {
	    bufp = malloc(RCP_DATA_BUFFER_SIZE);
	    if (bufp == NULL) {
		free(rcbp);
		free_local_file_rec(frecp);
		return FS_DEV_NO_MEM;
	    }
	    rcbp->buffer = bufp;
	    rcbp->size = RCP_DATA_BUFFER_SIZE;
	}

	/*
	 * Download the first block, this is to make sure
	 * the file is there, and get its size.
	 */
	confg.filename = &frecp->name[0];
	address_copy(&confg.ps.tuba.boothost, &rcbp->addr);
	confg.proto = FILE_ACCESS_RCP;

	rcbp->file_size = -1;
	size = rcbp->size;
	retcode = reg_invoke_file_read(confg.proto, &confg,
	    (uchar *)rcbp->buffer, (ulong *)&size,
	    TRUE, rcp_callback_test, (void *)frecp);

	if (retcode != ERROR_OK &&
	    (retcode != ERROR_WRITE || rcbp->control_flag != 1))
	{
	    free(bufp);
	    free(rcbp);
	    free_local_file_rec(frecp);
	    return FS_DEV_RCP_READ;
	}

	/*
	 * Initialize records. If there is a process
	 * hanging, kill it.
	 */
	frecp->base = 0;	/* we don't need this */
	frecp->offset = 0;
	rcbp->fs_offset = 0;
	rcbp->pid = -1;

    } else if (frecp->flag == OPEN_FOR_WRITE) {
	rcbp->pid = -1;		/* no process */
	rcbp->file_size = -1;	/* indicate no size set yet */
    } else {
	free(rcbp);
	free_local_file_rec(frecp);
	return FS_DEV_INV_FLAG;
    }
    return frecp->fd;
}

/*
 * Callback routine to process the received rcp data.
 * Return TRUE to continue rcp process.
 * Return FALSE to abort it, in this case, set flag in control_flag
 * to indicate to the caller.
 */
static boolean rcp_callback_proc_data (uchar **buf,
    long *left, long len, void *arg)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *rcbp;

    frecp = (fs_file_rec_t *)arg;
    rcbp = (remote_file_blk_t *)frecp->specific;
    rcbp->len = len;

    /*
     * Tell user process we've got a data buffer.
     */
    rcbp->control_flag = TFTP_RCP_DEV_DONE;

    if (rcbp->fs_offset + len >= rcbp->file_size)
	return TRUE;	/* this is last data, return to terminate rcp */

    /*
     * Pause the rcp progress and wait for
     * instruction from user process, or timeout.
     */
    edisms((blockproc *)server_dev_sleep, (ulong)arg);

    if (rcbp->control_flag == TFTP_RCP_DEV_TIMEOUT)
	return FALSE; /* Timeout, abort rcp procdure */

    /*
     * Go back to fetch next data buffer.
     */
    rcbp->fs_offset += len;
    *buf = rcbp->buffer;
    return TRUE;
}

/*
 * RCP read process driven by the user process.
 */
static forktype rcp_dev_proc (fs_file_rec_t* frecp)
{
    remote_file_blk_t *rcbp;
    int retcode;
    filetype confg;
    int size;

    rcbp = (remote_file_blk_t *)frecp->specific;

    memset(&confg, 0, sizeof(filetype));
    confg.filename = &frecp->name[0];
    address_copy(&confg.ps.tuba.boothost, &rcbp->addr);
    confg.proto = FILE_ACCESS_RCP;

    size = rcbp->size;
    retcode = reg_invoke_file_read(confg.proto, &confg,
	(uchar *)rcbp->buffer, (ulong *)&size,
	TRUE, rcp_callback_proc_data, (void *)frecp);

    if (retcode == ERROR_OK)
	rcbp->control_flag = TFTP_RCP_DEV_DONE;
    else {
	/*
	 * It's not stopped by us and it's not last pkt.
	 * Something is wrong.
	 */
	rcbp->control_flag = TFTP_RCP_DEV_ERROR;
    }

    rcbp->pid = -2;
    process_kill(-1);
}

/*
 * Read bytes up to specified count into buffer from file.
 */
static int RCP_read (int fd, void *buffer, int cnt)
{
    fs_file_rec_t *frecp;
    int pid;
    remote_file_blk_t *rcbp;
    int retcode;

    frecp = get_file_rec_by_fd(&rcp_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    rcbp = (remote_file_blk_t *)frecp->specific;
    if (rcbp == NULL)
	return FS_DEV_RCP_INT;

    rcbp->rtbuf = buffer;
    rcbp->request = cnt;
    rcbp->count = 0;

    if (rcbp->pid == -1) {
	/*
	 * Process is not created, or has terminated.
	 * Create it with default stack size, then let go.
	 */
	rcbp->control_flag = TFTP_RCP_DEV_GO;
	pid = cfork((forkproc *)rcp_dev_proc, (long)frecp, 0,
	    "rcp dev", stdio->ttynum);
	if (pid < 0)
	    return FS_DEV_FORK_ERR;
	rcbp->pid = pid;
    }

    /*
     * Loop until we get enough data as requested.
     */
    for (;;) {

	while (rcbp->control_flag == TFTP_RCP_DEV_GO)
	    process_suspend();

	if (rcbp->control_flag != TFTP_RCP_DEV_DONE)
	    return FS_DEV_RCP_READ;

	retcode = get_server_data(frecp, rcbp);
	if (retcode < 0)
	    return FS_DEV_RCP_READ; /* An error has occured */
	else if (retcode > 0)
	    return rcbp->count; /* Got what we need */

	/*
	 * Need to start rcp to get more data.
	 * Update file offset for the new data.
	 * Then let go the rcp task.
	 */
	if (rcbp->pid >= 0) {
	    rcbp->control_flag = TFTP_RCP_DEV_GO;
	} else {
	    /*
	     * RCP process has stopped either because this
	     * is last data buffer, or an error occured.
	     */
	    return rcbp->count;
	}
    }
}

/*
 * Close rcp device.
 */
static int RCP_close (int fd)
{
    fs_file_rec_t *frecp;
    int retcode = 0;
    remote_file_blk_t *rcbp;

    frecp = get_file_rec_by_fd(&rcp_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    rcbp = (remote_file_blk_t *)frecp->specific;
    if (rcbp) {
	if (rcbp->pid >= 0) {
	    process_kill(rcbp->pid);
	    rcbp->pid = -1;
	    if (frecp->flag == OPEN_FOR_WRITE)
		/*
		 * We haven't finish writing yet.
		 */
	    	retcode = FS_DEV_RCP_CLOSE;
	}

	if (frecp->flag == OPEN_FOR_READ) {
	    /*
	     * This buffer is allocated only in read operation.
	     */
	    if (rcbp->buffer) {
		free(rcbp->buffer);
		rcbp->buffer = NULL;
	    }
	}
	free(rcbp);
    }
    free_local_file_rec(frecp);
    return retcode;
}

/*
 * Write a buffer of data to rcp file.
 */
static int RCP_write (int fd, void *buffer, int cnt)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *rcbp;
    int pid;

    frecp = get_file_rec_by_fd(&rcp_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    if (frecp->flag != OPEN_FOR_WRITE)
	return FS_DEV_NOT_OPEN;

    /*
     * Make sure control block has been allocated.
     */
    rcbp = (remote_file_blk_t *)frecp->specific;
    if (rcbp == NULL)
	return FS_DEV_RCP_INT;

    /*
     * Make sure file size has been set.
     */
    if (rcbp->file_size < 0)
	return FS_DEV_FSIZE_ERR;

    /*
     * Process has terminated for
     * whatever reason (finish, timeout, error).
     */
    if (rcbp->pid == -2)
	return FS_DEV_TASK_TERM;

    /*
     * For first time write, set up buffer and create the process.
     * Otherwise, make sure process is in ready state.
     */
    if (rcbp->pid == -1) {

	rcbp->buffer = buffer;	/* the write buffer */
	rcbp->size = cnt;	/* and number of bytes in it */
	rcbp->rtbuf = rcbp->buffer;

	/*
	 * Create the process to upload the buffer.
	 */
	rcbp->control_flag = TFTP_RCP_DEV_GO;
	pid = cfork((forkproc *)rcp_dev_proc_write, (long)frecp,
	    0, "rcp dev write", 0);
	if (pid < 0)
	    return FS_DEV_FORK_ERR;
	rcbp->pid = pid;
    } else {
	/*
	 * This is a subsequent write, make sure process
	 * is ready for this write.
	 */
	if (rcbp->control_flag != TFTP_RCP_DEV_DONE) {
	    /*
	     * Shouldn't happen.
	     */
	    process_kill(rcbp->pid);
	    rcbp->pid = -1;
	    return FS_DEV_RCP_INT;
	}

	rcbp->buffer = buffer;		/* the write buffer */
	rcbp->size = cnt;		/* and number of bytes in it */
	rcbp->rtbuf = rcbp->buffer;

	rcbp->control_flag = TFTP_RCP_DEV_GO;
    }

    /*
     * Wait here until write is completed.
     */
    while (rcbp->control_flag == TFTP_RCP_DEV_GO)
	process_suspend();

    if (rcbp->control_flag != TFTP_RCP_DEV_DONE) {
	/*
	 * Either timeout or an error has occured.
	 * Kill the process and return.
	 */
	if (rcbp->pid >= 0) {
	    process_kill(rcbp->pid);
	    rcbp->pid = -1;
	}
	return FS_DEV_RCP_WRITE;
    }

    return (cnt - rcbp->size);
}

/*
 * Return information of a given file.
 */
static int RCP_fstat (int fd, stat_t *status)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *rcbp;

    frecp = get_file_rec_by_fd(&rcp_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FILE_ID;

    rcbp = (remote_file_blk_t *)frecp->specific;
    if (rcbp == NULL)
	return FS_DEV_RCP_INT;

    status->st_size = rcbp->file_size;
    status->server_addr = &rcbp->addr;
    return 0;
}

/*
 * Return all devices in this class
 */
static int RCP_getdevicenames(char *buffer, int buffer_len)
{
    if (buffer_len > strlen(RCP_DEV_NAME)) {
	strcpy(buffer, RCP_DEV_NAME);
	return 0;
    }
    return FS_DEV_TFTP_INT;
}

/*
 * Return error text string by code.
 */
static void RCP_geterrortext (int err_code, char *buffer, int blen)
{
    const char *msg;

    msg = common_geterrortext(err_code);
    strcpy(buffer, (char *)msg);
}

/*
 * Generic routine to set/get file attribes by file id.
 */
static int RCP_ioctl (const char *file_id, int func, void *arg)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *rcbp;
    int retcode = 0;
    const char *file_name;

    switch (func) {
	case IOCTL_GET_DEV_TYPE:
	    *(uint *)arg = IOCTL_DEV_TYPE_RCP;
	    break;

	case IOCTL_SET_FILE_SIZE:
	    file_name = get_file_name_by_file_id(file_id);
	    if (file_name) {
		frecp = get_file_rec_by_name(&rcp_dev_rec, file_name);
		if (frecp) {
		    rcbp = (remote_file_blk_t *)frecp->specific;
		    if (rcbp) {
			rcbp->file_size = (int)arg;
			break;
		    }
		}
	    }
	    retcode = FS_DEV_INV_FILE_ID;
	    break;

	default:
	    retcode = FS_DEV_INV_OP;
    }
    return retcode;
}

/*
 * Generic routine to set/get file attribes by file id.
 */
static int RCP_fioctl (int fd, int func, void *arg)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *rcbp;
    int retcode = 0;

    switch (func) {
	case IOCTL_GET_DEV_TYPE:
	    *(uint *)arg = IOCTL_DEV_TYPE_RCP;
	    break;

	case IOCTL_SET_FILE_SIZE:
	    frecp = get_file_rec_by_fd(&rcp_dev_rec, fd);
	    if (frecp) {
		rcbp = (remote_file_blk_t *)frecp->specific;
		if (rcbp) {
		    rcbp->file_size = (int)arg;
		    break;
		}
	    }
	    retcode = FS_DEV_INV_FD;
	    break;

	default:
	    retcode = FS_DEV_INV_OP;
    }
    return retcode;
}

/*
 * The following interface routines are not supported.
 */
static int RCP_delete (const char *file_id)
{
    return FS_DEV_NOT_SUP;
}

static int RCP_fsstat (const char *dev_id, file_sys_stat_t *status)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Seek to position in remote file.
 * Requirements:
 *     File must be opened READ only
 *     The resultant offset must be within the current remote read buffer.
 */
static int RCP_lseek (int fd, int offset, int whence)
{
    fs_file_rec_t *frecp;
    remote_file_blk_t *rcbp;

    int curr_ptr;

    frecp = get_file_rec_by_fd(&rcp_dev_rec, fd);
    if (frecp == NULL)
	return FS_DEV_INV_FD;

    if (frecp->flag != OPEN_FOR_READ)
	return FS_DEV_NOT_OPEN;

    rcbp = (remote_file_blk_t *)frecp->specific;
    if (rcbp) {
	switch (whence) {
	  case SEEK_SET:
	    curr_ptr = offset;
	    break;
	  case SEEK_CUR:
	    curr_ptr = frecp->offset + offset;
	    break;
	  case SEEK_END:
	    curr_ptr = rcbp->file_size + offset;
	    break;
	  default:
	    return FS_DEV_INV_WHENCE;
	}

	/* 
	 * now make sure that we have a reasonable curr_ptr
	 */
	if ( (curr_ptr < rcbp->fs_offset) || 
	     (curr_ptr >= rcbp->fs_offset + rcbp->len))
	     return FS_DEV_INV_LSEEK;

	frecp->offset = curr_ptr;;
	return 0;
    }
    else
	return FS_DEV_RCP_INT;
}

/*
 * Return information of a given file.
 */
static int RCP_stat (const char *file_id, stat_t *status)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Return information of a given file.
 */
static int RCP_istat (const char *dev_id, int index, stat_t *status)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Undelete a file.
 */
static int RCP_undelete (const char *dev_id, int index, int mode)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Squeeze the device.
 */
static int RCP_squeeze (const char *dev_id, int flag)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Set file type.
 */
static int RCP_settype (const char *file_id, ulong file_type)
{
    return FS_DEV_NOT_SUP;
}

/*
 * Return file diretory entry.
 */
static int RCP_getdents (int fd, char *buffer, int buffer_length)
{
    return FS_DEV_NOT_SUP;
}

/****************************************************************************
*
* Subsystem header and init routine.
*
*****************************************************************************/

/*
 * tftp/rcp subsystem init routine.
 */
static void tftp_rcp_subsys_init (subsystype* subsys)
{
    int file_sys_type;
    char device_name[FS_DEV_NAMES_MAX];
    int error;

    /*
     * register with the tftp file system layer
     * and get the file_sys_type to use.
     */
    error = (int)TFTP_getdevicenames(device_name, FS_DEV_NAMES_MAX);
    file_sys_type = FS_register(device_name, FS_STD_ERROR_CODE_ALLOCATION);

    /*
     * add all the client file system routines to the registries
     */
    reg_add_open(file_sys_type, TFTP_open, "TFTP_open");
    reg_add_close(file_sys_type, TFTP_close, "TFTP_close");
    reg_add_read(file_sys_type, TFTP_read, "TFTP_read");
    reg_add_write(file_sys_type, TFTP_write, "TFTP_write");
    reg_add_lseek(file_sys_type, TFTP_lseek, "TFTP_lseek");
    reg_add_stat(file_sys_type, TFTP_stat, "TFTP_stat");
    reg_add_fstat(file_sys_type, TFTP_fstat, "TFTP_fstat");
    reg_add_istat(file_sys_type, TFTP_istat, "TFTP_istat");
    reg_add_fsstat(file_sys_type, TFTP_fsstat, "TFTP_fsstat");
    reg_add_delete(file_sys_type, TFTP_delete, "TFTP_delete");
    reg_add_undelete(file_sys_type, TFTP_undelete, "TFTP_undelete");
    reg_add_squeeze(file_sys_type, TFTP_squeeze, "TFTP_squeeze");
    reg_add_settype(file_sys_type, TFTP_settype, "TFTP_settype");
    reg_add_getdents(file_sys_type, TFTP_getdents, "TFTP_getdents");
    /*
    reg_add_format(file_sys_type, TFTP_format, "TFTP_format");
    */
    reg_add_geterrortext(file_sys_type, TFTP_geterrortext, "TFTP_geterrortext");
    reg_add_getdevicenames(file_sys_type,
	TFTP_getdevicenames, "TFTP_getdevicenames");
    reg_add_ioctl(file_sys_type, TFTP_ioctl, "TFTP_ioctl");
    reg_add_fioctl(file_sys_type, TFTP_fioctl, "TFTP_fioctl");

    /*
     * register with the rcp file system layer
     * and get the file_sys_type to use.
     */
    error = (int)RCP_getdevicenames(device_name, FS_DEV_NAMES_MAX);
    file_sys_type = FS_register(device_name, FS_STD_ERROR_CODE_ALLOCATION);

    /*
     * add all the client file system routines to the registries
     */
    reg_add_open(file_sys_type, RCP_open, "RCP_open");
    reg_add_close(file_sys_type, RCP_close, "RCP_close");
    reg_add_read(file_sys_type, RCP_read, "RCP_read");
    reg_add_write(file_sys_type, RCP_write, "RCP_write");
    reg_add_lseek(file_sys_type, RCP_lseek, "RCP_lseek");
    reg_add_stat(file_sys_type, RCP_stat, "RCP_stat");
    reg_add_fstat(file_sys_type, RCP_fstat, "RCP_fstat");
    reg_add_istat(file_sys_type, RCP_istat, "RCP_istat");
    reg_add_fsstat(file_sys_type, RCP_fsstat, "RCP_fsstat");
    reg_add_delete(file_sys_type, RCP_delete, "RCP_delete");
    reg_add_undelete(file_sys_type, RCP_undelete, "RCP_undelete");
    reg_add_squeeze(file_sys_type, RCP_squeeze, "RCP_squeeze");
    reg_add_settype(file_sys_type, RCP_settype, "RCP_settype");
    reg_add_getdents(file_sys_type, RCP_getdents, "RCP_getdents");
    /*
    reg_add_format(file_sys_type, RCP_format, "RCP_format");
    */
    reg_add_geterrortext(file_sys_type, RCP_geterrortext, "RCP_geterrortext");
    reg_add_getdevicenames(file_sys_type,
	RCP_getdevicenames, "RCP_getdevicenames");
    reg_add_ioctl(file_sys_type, RCP_ioctl, "RCP_ioctl");
    reg_add_fioctl(file_sys_type, RCP_fioctl, "RCP_fioctl");

    /*
     *	Assign default tftp/rcp server address.
     */
    tftp_server_addr.type = ADDR_IP;
    tftp_server_addr.ip_addr = -1;

    rcp_server_addr.type = ADDR_IP;
    rcp_server_addr.ip_addr = -1;
}

/*
 * Subsystem header for the tftp/rcp file system
 */
SUBSYS_HEADER(tftp_rcp_file_system,     /* name */
    1,				/* major version */
    0,				/* minor version */
    1,				/* edit version */
    tftp_rcp_subsys_init,	/* init function */
    SUBSYS_CLASS_DRIVER,	/* subsystem class */
    "seq: file_system",		/* sequence list */
    "req: file_system"		/* prerequisite list */
);
