/* $Id: remote_filesys_client.c,v 3.4.6.1 1996/03/18 19:35:56 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/remote_filesys_client.c,v $
 *------------------------------------------------------------------
 * Remote file system client side -- runs on the slave RSP image
 *
 * June 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: remote_filesys_client.c,v $
 * Revision 3.4.6.1  1996/03/18  19:35:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.3  1996/03/16  06:42:28  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.12.2  1996/03/07  09:36:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  13:50:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/06  06:59:11  dbath
 * CSCdi50822:  IOCTL_DEV_TYPE_LOCAL is set, even for remote files
 * Zero out IOCTL_DEV_TYPE_LOCAL, since if we're in here, we know
 * it's not a local device.  While the bonnet is up, clean up some
 * nasty whitespace... Jack might have 300 column emacs windows
 * but the rest of us don't.
 *
 * Revision 3.3  1995/12/18  07:09:07  schaefer
 * CSCdi45771: include slave devices in 75xx flash mib
 *
 * Revision 3.2  1995/11/17  09:08:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  17:04:28  yanke
 * Placeholders
 *
 * CSCdi39151:  modulized local/remote file systems
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "logger.h"
#include "subsys.h"
#include "filesys.h"
#include "filesys_registry.h"
#include "../h/ttysrv.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipc/ipc_message.h"
#include "../ipc/ipc_seats.h"
#include "../ipc/ipc_ports.h"
#include "remote_filesys.h"

/*----------------------------------------------------------------------------*
 *
 *----------------------------------------------------------------------------*/
static ipc_port_id rfsc_server_port = 0;

static void RFSC_locate_server_port (void)
{
    /*
     * Find the server port and save the port ID for use by all the
     * client routines. We delay this operation because we need
     * to wait for the slave to initialize itself.
     */
    if (rfsc_server_port == 0)
	rfsc_server_port = ipc_locate_port(RFS_SERVER_SLAVE_PORT_NAME);
}


/******************************************************************************
 * Name: RFSC_open
 *
 * Description: opens a file
 *
 * Input: 
 *
 * Return Value:
 *
 *******************************************************************************/
static int RFSC_open (const char *file_id, int flags, ulong mode)
{
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_open_req_t *req;
    rfs_open_rsp_t *rsp;
    int fd;

    RFSC_locate_server_port();
    if (!rfsc_server_port)
	return RFS_ERROR_SENDING_IPC_REQUEST;

    req_msg = ipc_get_message(sizeof(rfs_open_req_t),rfsc_server_port,
			      RFS_OPEN_REQ);
    if (req_msg == NULL) {
	/* post an error */
	fd = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	strncpy(req->file_id,file_id,sizeof(req->file_id));
	req->flags = flags;
	req->mode = mode;
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    fd = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    fd = rsp->fd;
	    ipc_return_message(rsp_msg);
	}
    }
    return(fd);
}


/******************************************************************************
 * Name: RFSC_close
 *
 * Description: closes an open file
 *
 * Input: 
 *   fd -- the file descriptor for the open file (returned from open())
 *
 * Return Value:
 *   error_code -- 0 if successful, othewise an error code.
 *
 *******************************************************************************/
static int RFSC_close (int fd)
{
    int rc;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_close_req_t *req;
    rfs_close_rsp_t *rsp;

    req_msg = ipc_get_message(sizeof(rfs_close_req_t),rfsc_server_port,
			      RFS_CLOSE_REQ);
    if (req_msg == NULL) {
	/* post an error */
	rc = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	req->fd = fd;
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    rc = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    rc = rsp->return_code;
	    ipc_return_message(rsp_msg);
	}
    }
    return(rc);
}


/******************************************************************************
 * Name: RFSC_read
 *
 * Description: reads from an open file
 *
 * Input: 
 *   fd -- the file descriptor for the open file (returned from open())
 *   buffer -- a pointer to the buffer inwhich to put the data
 *   byte_count -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually read.  If there was an error while
 *     reading the file, an error code is returned.  All error codes are
 *     negative, and all valid values for actual are positive.  Note that
 *     reading from a file that has no more bytes left in it is NOT an error.
 *     In this case a 0 is returned.
 *
 *******************************************************************************/
static int RFSC_read (int fd, void *buffer, int byte_count)
{
    int actual;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_read_req_t *req;
    rfs_read_rsp_t *rsp;

    actual = 0;
    while ((byte_count > 0) && (actual >= 0)) {
	/*
	 * Send off a request to read some bytes.  Up to the max size
	 * that we can ask for.
	 */
	/*
	 * Get a message buffer
	 */
	req_msg = ipc_get_message(sizeof(rfs_read_req_t),rfsc_server_port,
				  RFS_READ_REQ);
	if (req_msg == NULL) {
	    /* post an error */
	    actual = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
	} else {
	    /*
	     * got a buffer, fill it out and send it off
	     */
	    req = req_msg->data;
	    req->fd = fd;
	    if (byte_count > RFS_DATA_BUFFER_SIZE)
		req->byte_count = RFS_DATA_BUFFER_SIZE;
	    else
		req->byte_count = byte_count;
	    rsp_msg = ipc_send_rpc(req_msg,&ec);
	    if (ec != IPC_OK) {
		/* post an error */
		actual = RFS_ERROR_SENDING_IPC_REQUEST;
	    } else {
		/*
		 * check to see how it went on the remote side
		 */
		rsp = rsp_msg->data;
		if (rsp->actual < 0) {
		    actual = rsp->actual;
		} else if (rsp->actual == 0) {
		    ipc_return_message(rsp_msg);
		    break;	/* we are done */
		} else {
		    /*
		     * Everything went OK on the remote side.
		     * Copy the data and update the counts and pointers.
		     */
		    memcpy(buffer,rsp->data,rsp->actual);
		    byte_count -= rsp->actual;
		    (ulong)buffer += rsp->actual;
		    actual += rsp->actual;
		}
		ipc_return_message(rsp_msg);
	    }
	}
    } /* end while */
    return(actual);
}


/******************************************************************************
 * Name: RFSC_write
 *
 * Description: writes data to a file that has been opened for writing
 *
 * Input: 
 *   fd -- the file descriptor for the open file (returned from open())
 *   buffer -- a pointer to the data to write
 *   byte_count -- the number of bytes to write
 *
 * Return Value:
 *   actual -- the number of bytes actually written.  If there was an error,
 *     an error code is returned.  All error codes are negative.
 *
 *******************************************************************************/
static int RFSC_write (int fd, void *buffer, int byte_count)
{
    int actual;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_write_req_t *req;
    rfs_write_rsp_t *rsp;

    actual = 0;
    while ((byte_count > 0) && (actual >= 0)) {
	/*
	 * Send off a request to read some bytes.  Up to the max size
	 * that we can ask for.
	 */
	/*
	 * Get a message buffer
	 */
	req_msg = ipc_get_message(sizeof(rfs_write_req_t),rfsc_server_port,
				  RFS_WRITE_REQ);
	if (req_msg == NULL) {
	    /* post an error */
	    actual = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
	} else {
	    /*
	     * got a buffer, fill it out, copy the data and send it off
	     */
	    req = req_msg->data;
	    req->fd = fd;
	    if (byte_count > RFS_DATA_BUFFER_SIZE)
		req->byte_count = RFS_DATA_BUFFER_SIZE;
	    else
		req->byte_count = byte_count;
	    memcpy(req->data,buffer,req->byte_count);
	    rsp_msg = ipc_send_rpc(req_msg,&ec);
	    if (ec != IPC_OK) {
		/* post an error */
		actual = RFS_ERROR_SENDING_IPC_REQUEST;
	    } else {
		/*
		 * check to see how it went on the remote side
		 */
		rsp = rsp_msg->data;
		if (rsp->actual < 0) {
		    actual = rsp->actual;
		} else {
		    /*
		     * Everything went OK on the remote side.
		     * Update the counts and pointers.
		     */
		    byte_count -= rsp->actual;
		    (ulong)buffer += rsp->actual;
		    actual += rsp->actual;
		}
		ipc_return_message(rsp_msg);
	    }
	}
    } /* end while */
    return(actual);
}


/******************************************************************************
 * Name: RFSC_lseek
 *
 * Description: sets the current pointer associated with the open file
 *
 * Input: 
 *   fd -- the file descriptor for the open file (returned from open())
 *   offset -- the offset to seek in the file 
 *   whence -- determines from whence the seek originates. Possible values:
 *     SEEK_SET -- set the pointer to the offset.  If the offset is longer
 *       than the file, the pointer is set one byte past the end of the file.
 *     SEEK_CUR -- increment the pointer by offset.  If pointer + offset is
 *       longer than the file, the pointer is set one byte past the end of
 *       the file.
 *     SEEK_END -- set the pointer to the size of the file plus offset. 
 *       If offset is positive, the pointer is set to the size of the file
 *       (i.e. one byte past the end of the file).  If size + offset is 
 *       negative, the pointer is set to 0 (i.e. the beginning of the file).
 *
 * Return Value:
 *   curr_ptr -- the new value of the file pointer.  If an error occurred, an
 *     error code is returned.  All error codes are negative, and all valid 
 *     file pointers are positive.
 *
 *******************************************************************************/
static int RFSC_lseek (int fd, int offset, int whence)
{
    int curr_ptr;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_lseek_req_t *req;
    rfs_lseek_rsp_t *rsp;

    req_msg = ipc_get_message(sizeof(rfs_lseek_req_t),rfsc_server_port,
			      RFS_LSEEK_REQ);
    if (req_msg == NULL) {
	/* post an error */
	curr_ptr = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	req->fd = fd;
	req->offset = offset;
	req->whence = whence;
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    curr_ptr = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    curr_ptr = rsp->curr_ptr;
	    ipc_return_message(rsp_msg);
	}
    }
    return(curr_ptr);
}


/******************************************************************************
 * Name: RFSC_stat
 *
 * Description: returns information about a file (by file name)
 *
 * Input: 
 *   file_id -- the file ID of the file (including device ID and file name)
 *     for which to return status
 *   status -- a pointer to a buffer inwhich to write the stat_t structure.
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
static int RFSC_stat (const char *file_id, stat_t *status)
{
    int rc;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_stat_req_t *req;
    rfs_stat_rsp_t *rsp;

    RFSC_locate_server_port();
    if (!rfsc_server_port)
	return RFS_ERROR_SENDING_IPC_REQUEST;

    req_msg = ipc_get_message(sizeof(rfs_stat_req_t),rfsc_server_port,
			      RFS_STAT_REQ);
    if (req_msg == NULL) {
	/* post an error */
	rc = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	strncpy(req->file_id,file_id,sizeof(req->file_id));
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    rc = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    rc = rsp->return_code;
	    *status = rsp->status;
	    ipc_return_message(rsp_msg);
	}
    }
    return(rc);
}


/******************************************************************************
 * Name: RFSC_fstat
 *
 * Description: returns information about an open file (by file descriptor)
 *
 * Input: 
 *   fd -- the file descriptor for the open file (returned from open())
 *   status -- a pointer to a buffer inwhich to write the stat_t structure.
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
static int RFSC_fstat (int fd, stat_t *status)
{
    int rc;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_fstat_req_t *req;
    rfs_fstat_rsp_t *rsp;

    req_msg = ipc_get_message(sizeof(rfs_fstat_req_t),rfsc_server_port,
			      RFS_FSTAT_REQ);
    if (req_msg == NULL) {
	/* post an error */
	rc = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	req->fd = fd;
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    rc = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    rc = rsp->return_code;
	    *status = rsp->status;
	    ipc_return_message(rsp_msg);
	}
    }
    return(rc);
}


/******************************************************************************
 * Name: RFSC_istat
 *
 * Description: returns information about a file (by file index)
 *
 * Input: 
 *   dev_id -- a pointer to a character string that is the device ID for the
 *     device containing the file.  The device ID can be terminated with a 
 *     null or a colon (:).
 *   index -- the index of the file for which to return status
 *   status -- a pointer to a buffer inwhich to write the stat_t structure.
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
static int RFSC_istat (const char *dev_id, int index, stat_t *status)
{
    int rc;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_istat_req_t *req;
    rfs_istat_rsp_t *rsp;

    RFSC_locate_server_port();
    if (!rfsc_server_port)
	return RFS_ERROR_SENDING_IPC_REQUEST;

    req_msg = ipc_get_message(sizeof(rfs_istat_req_t),rfsc_server_port,
			      RFS_ISTAT_REQ);
    if (req_msg == NULL) {
	/* post an error */
	rc = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	strncpy(req->dev_id,dev_id,sizeof(req->dev_id));
	req->index = index;
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    rc = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    rc = rsp->return_code;
	    *status = rsp->status;
	    ipc_return_message(rsp_msg);
	}
    }
    return(rc);
}


/******************************************************************************
 * Name: RFSC_fsstat
 *
 * Description: returns information about a file system
 *
 * Input: 
 *   dev_id -- a pointer to a character string that is the device ID for the
 *     device for which to return status information
 *   status -- a pointer to a buffer inwhich to write the file_sys_stat_t structure.
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
static int RFSC_fsstat (const char *dev_id, file_sys_stat_t *status)
{
    int rc;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_fsstat_req_t *req;
    rfs_fsstat_rsp_t *rsp;

    RFSC_locate_server_port();
    if (!rfsc_server_port)
	return RFS_ERROR_SENDING_IPC_REQUEST;

    req_msg = ipc_get_message(sizeof(rfs_fsstat_req_t),rfsc_server_port,
			      RFS_FSSTAT_REQ);
    if (req_msg == NULL) {
	/* post an error */
	rc = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	strncpy(req->dev_id,dev_id,sizeof(req->dev_id));
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    rc = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    rc = rsp->return_code;
	    *status = rsp->status;
	    ipc_return_message(rsp_msg);
	}
    }
    return(rc);
}


/******************************************************************************
 * Name: RFSC_delete
 *
 * Description: deletes a file by file name
 *
 * Input: 
 *   file_id -- the file ID (including the device ID and the file name) of the
 *     file to delete
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
static int RFSC_delete (const char *file_id)
{
    int rc;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_delete_req_t *req;
    rfs_delete_rsp_t *rsp;

    RFSC_locate_server_port();
    if (!rfsc_server_port)
	return RFS_ERROR_SENDING_IPC_REQUEST;

    req_msg = ipc_get_message(sizeof(rfs_delete_req_t),rfsc_server_port,
			      RFS_DELETE_REQ);
    if (req_msg == NULL) {
	/* post an error */
	rc = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	strncpy(req->file_id,file_id,sizeof(req->file_id));
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    rc = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    rc = rsp->return_code;
	    ipc_return_message(rsp_msg);
	}
    }
    return(rc);
}


/******************************************************************************
 * Name: RFSC_undelete
 *
 * Description: undeletes a file by device ID and file index
 *
 * Input: 
 *   dev_id -- the device ID of the device containing the file to undelete
 *   index -- the index of the file to undelete
 *   mode -- indicates what to do if the file to undelete has the same name
 *     as a file that already exists.  
 *     FSLIB_UNDELETE_AUTO -- automatically delete another file of the same name
 *     FSLIB_UNDELETE_UNIQUE -- only undelete the file if the file name in unique
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
static int RFSC_undelete (const char *dev_id, int index, int mode)
{
    int rc;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_undelete_req_t *req;
    rfs_undelete_rsp_t *rsp;

    RFSC_locate_server_port();
    if (!rfsc_server_port)
	return RFS_ERROR_SENDING_IPC_REQUEST;

    req_msg = ipc_get_message(sizeof(rfs_undelete_req_t),rfsc_server_port,
			      RFS_UNDELETE_REQ);
    if (req_msg == NULL) {
	/* post an error */
	rc = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	strncpy(req->dev_id,dev_id,sizeof(req->dev_id));
	req->index = index;
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    rc = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    rc = rsp->return_code;
	    ipc_return_message(rsp_msg);
	}
    }
    return(rc);
}


/******************************************************************************
 * Name: RFSC_squeeze
 *
 * Description: squeezes al the unused space out of a file system
 *   This function may take a long time to execute (perhaps minutes)
 *   since it may have to erase all of flash.  The process will leave a file
 *   system with all of the files that are valid at the beginning of the file
 *   system memory, and all of the unused space at the end of the file system
 *   memory will be erased.  After this function is executed, none of the
 *   deleted files can be successfully "undeleted".  Before a squeeze, files
 *   that have been deleted remain in the file system and are just marked
 *   deleted.  When the squeeze is executed, the deleted files are actually
 *   erased. 
 *
 * Input: 
 *   dev_id -- the device ID of the device containing the file to undelete
 *   flag -- flag that indicates whether the squeeze log should be used or not.
 *     Not using the squeeze log could speed up the squeeze in some cases, but
 *     it means that if power is lost or the card is removed before the
 *     squeeze completes, the data on the flash will be lost, and the device
 *     will have to be reformatted.
 *     SQUEEZE_NORMAL -- squeeze log *WILL* be used
 *     SQUEEZE_NO_LOG -- squeeze log *WILL NOT* be used
 *
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
static int RFSC_squeeze (const char *dev_id, int flag)
{
    int rc;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_squeeze_req_t *req;
    rfs_squeeze_rsp_t *rsp;

    RFSC_locate_server_port();
    if (!rfsc_server_port)
	return RFS_ERROR_SENDING_IPC_REQUEST;

    req_msg = ipc_get_message(sizeof(rfs_squeeze_req_t),rfsc_server_port,
			      RFS_SQUEEZE_REQ);
    if (req_msg == NULL) {
	/* post an error */
	rc = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	strncpy(req->dev_id,dev_id,sizeof(req->dev_id));
	req->flag = flag;
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    rc = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    rc = rsp->return_code;
	    ipc_return_message(rsp_msg);
	}
    }
    return(rc);
}


/******************************************************************************
 * Name: RFSC_getdents
 *
 * Description: gets directory entries in a file system independent format.
 *   The directory entries returned include all files in the file system
 *   including files that have been deleted.
 *
 * Input: 
 *   fd -- the file descriptor for an open directory (the directory file to
 *     open for the directory is <device ID>:. (where the file name is ".")
 *   buffer -- a pointer to a buffer to hold the directory entries.
 *   buffer_length -- the length of the buffer pointed to by "buffer"
 *
 * Return Value:
 *   byte_count -- the number of bytes written to the buffer.  If there was 
 *     an error an error code is returned.  All error codes are negative.
 *
 *******************************************************************************/
static int RFSC_getdents (int fd, char *buffer, int buffer_length)
{
    int byte_count;
    int last_time;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_getdents_req_t *req;
    rfs_getdents_rsp_t *rsp;

    byte_count = 0;
    last_time = FALSE;
    while ((last_time == FALSE) && (byte_count >= 0)) {
	/*
	 * Send off a request to read as many dirents as will fit in
	 * the buffer we were given or the largest buffer we can use
	 * remotely (whichever is less)
	 */
	/*
	 * Get a message buffer
	 */
	req_msg = ipc_get_message(sizeof(rfs_getdents_req_t),rfsc_server_port,
				  RFS_GETDENTS_REQ);
	if (req_msg == NULL) {
	    /* post an error */
	    byte_count = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
	} else {
	    /*
	     * got a buffer, fill it out and send it off
	     */
	    req = req_msg->data;
	    req->fd = fd;
	    if (buffer_length > RFS_DIRENT_BUFFER_MAX) {
		req->buffer_length = RFS_DIRENT_BUFFER_MAX;
	    } else {
		req->buffer_length = buffer_length;
		last_time = TRUE;
	    }
	    rsp_msg = ipc_send_rpc(req_msg,&ec);
	    if (ec != IPC_OK) {
		/* post an error */
		byte_count = RFS_ERROR_SENDING_IPC_REQUEST;
	    } else {
		/*
		 * check to see how it went on the remote side
		 */
		rsp = rsp_msg->data;
		if (rsp->byte_count < 0) {
		    byte_count = rsp->byte_count;
		} else if (rsp->byte_count == 0) {
		    ipc_return_message(rsp_msg);
		    break;	/* we are done */
		} else {
		    /*
		     * Everything went OK on the remote side.
		     * Copy the data and update the counts and pointers.
		     */
		    memcpy(buffer,rsp->buffer,rsp->byte_count);
		    buffer_length -= rsp->byte_count;
		    buffer += rsp->byte_count;
		    byte_count += rsp->byte_count;
		}
		ipc_return_message(rsp_msg);
	    }
	}
    } /* end while */
    return(byte_count);
}



/******************************************************************************
 * Name: RFSC_settype
 *
 * Description: Sets the file type of the specified file.
 *   If the file type has already been set, it is not changed.  If the current 
 *   value of the file type is the same as the value being set, this program
 *   returns FSLIB_ERROR_FILE_TYPE_SET_THE_SAME.  If the current value of the
 *   file type is different than the one being set, this program returns
 *   FSLIB_ERROR_FILE_TYPE_ALREADY_SET.
 *
 * Input: 
 *   file_id -- the file ID of the file (including device ID and file name)
 *     for which to set the type
 *   file_type -- the value to set the file type
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, FSLIB_ERROR_FILE_TYPE_SET_THE_SAME
 *     if the file type has already been set and is being set to the same
 *     value again, FSLIB_ERROR_FILE_TYPE_ALREADY_SET if the file type has
 *     already been set and is being set to a different value.
 *
 *******************************************************************************/
static int RFSC_settype (const char *file_id, ulong file_type)
{
    int rc;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_settype_req_t *req;
    rfs_settype_rsp_t *rsp;

    RFSC_locate_server_port();
    if (!rfsc_server_port)
	return RFS_ERROR_SENDING_IPC_REQUEST;

    req_msg = ipc_get_message(sizeof(rfs_settype_req_t),rfsc_server_port,
			      RFS_SETTYPE_REQ);
    if (req_msg == NULL) {
	/* post an error */
	rc = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	strncpy(req->file_id,file_id,sizeof(req->file_id));
	req->file_type = file_type;
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    rc = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    rc = rsp->return_code;
	    ipc_return_message(rsp_msg);
	}
    }
    return(rc);
}


/******************************************************************************
 * Name: RFSC_geterrortext
 *
 * Description: returns a pointer to a text string for the give error code
 *
 * Input: 
 *   error -- error code
 *
 * Return Value:
 *   pointer to a text string describing the error
 *
 *******************************************************************************/
static void RFSC_geterrortext (int error_code, char *buffer, int buffer_len)
{
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_geterrortext_req_t *req;
    rfs_geterrortext_rsp_t *rsp;
    int len;

    RFSC_locate_server_port();
    if (!rfsc_server_port) {
	strncpy(buffer,"ERROR_FINDING_IPC_PORT", buffer_len);
	buffer[buffer_len - 1] = '\0';
	return;
    }

    req_msg = ipc_get_message(sizeof(rfs_geterrortext_req_t),rfsc_server_port,
			      RFS_GETERRORTEXT_REQ);
    if (req_msg == NULL) {
	/* post an error */
	strncpy(buffer,"ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER",buffer_len);
    } else {
	req = req_msg->data;
	req->error_code = error_code;
	req->buffer_len = RFS_ERROR_TEXT_MAX;
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    strncpy(buffer,"ERROR_SENDING_IPC_REQUEST",buffer_len);
	} else {
	    rsp = rsp_msg->data;
	    if (buffer_len > RFS_ERROR_TEXT_MAX)
		len = RFS_ERROR_TEXT_MAX;
	    else 
		len = buffer_len;
	    strncpy(buffer,rsp->buffer,len);
	    buffer[len - 1] = '\0';
	    ipc_return_message(rsp_msg);
	}
    }
    buffer[buffer_len - 1] = '\0';
}


/******************************************************************************
 * Name: RFSC_getdevicenames
 *
 * Description: gets a string that contains all the valid device names
 *   This function just calls ralib_get_device_names() and returns
 *   that value.
 *
 * Input: None
 *   buffer -- a pointer to the buffer inwhich to put the names of the
 *     devices supported.  The names do not contain the colon (:) and are
 *     concatenated with separating spaces and the whole string is
 *     null terminated.  The device names are in no specific order. 
 *
 *     Example of return value.
 *       "slot0 slot1 bootflash"
 * 
 *   buffer_len -- the length of the buffer pointed to by "buffer"
 *
 * Return Value:
 *   return_code -- RFS_OK if successful, otherwise an error code
 *
 *******************************************************************************/
static int RFSC_getdevicenames (char *buffer, int buffer_len)
{
    int rc;
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_getdevicenames_req_t *req;
    rfs_getdevicenames_rsp_t *rsp;
    const char *pp;
    char *np;
    char *bp;

    RFSC_locate_server_port();
    if (!rfsc_server_port)
	return RFS_OK;

    req_msg = ipc_get_message(sizeof(rfs_getdevicenames_req_t),
			      rfsc_server_port,RFS_GETDEVICENAMES_REQ);
    if (req_msg == NULL) {
	/* post an error */
	rc = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	req->buffer_len = RFS_DEVICE_NAMES_BUFFER_MAX;
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    rc = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    rc = rsp->return_code;
	    if (rc == RFS_OK) {
		/* 
		 * copy the string from the response IPC buffer to the
		 * buffer supplied by the caller and insert the
		 * slave prefix string at the beginning of each
		 * device.
		 */
		bp = buffer;
		np = rsp->buffer;
		while ((buffer_len > 0) && (*np != '\0')) {
		    /*
		     * copy the prefix to the buffer
		     */
		    pp = slave_prefix;
		    while ((buffer_len > 0) && (*pp != '\0')) {
			*bp++ = *pp++;
			buffer_len--;
		    }
		    /*
		     * copy the device name immediately after the prefix
		     */
		    while ((buffer_len > 0) && (*np != ' ') && (*np != '\0')) {
			*bp++ = *np++;
			buffer_len--;
		    }
		    /*
		     * copy the space ore terminating null
		     */
		    *bp++ = *np++;
		    buffer_len--;
		}
		if (buffer_len <= 0) {
		    rc = RFS_ERROR_BUFFER_OVERFLOW;
		}
	    }
	    ipc_return_message(rsp_msg);
	}
    }
    return(rc);
}

#define	FORMAT_TIME_PERIOD	600

/*
 * need to support format, print chip, get device.
 */
int RFSC_ioctl (const char *file_id, int func, void *arg)
{
    ipc_error_code ec;
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    rfs_ioctl_req_t *req;
    rfs_ioctl_rsp_t *rsp;
    int retcode = RFS_OK;

    RFSC_locate_server_port();
    if (!rfsc_server_port)
	return RFS_ERROR_SENDING_IPC_REQUEST;

    req_msg = ipc_get_message(sizeof(rfs_ioctl_req_t),rfsc_server_port,
			      RFS_IOCTL_REQ);
    if (req_msg == NULL) {
	/* post an error */
	retcode = RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER;
    } else {
	req = req_msg->data;
	if (file_id)
	    strncpy(req->file_id,file_id,sizeof(req->file_id));
	else
	    req->file_id[0] = 0;
	req->func = func;
	switch (func) {
	    case IOCTL_FORMAT_FLASH:
		memcpy(&req->arg, arg, sizeof(ioctl_arg_t));
		ipc_set_rpc_timeout(req_msg, FORMAT_TIME_PERIOD);
		break;

	    case IOCTL_SET_VERBOSE:
		req->arg.verbose_flag = (int)arg;
		break;

	    default:
		break;
	}
	req->server_ttynum = tt2number(stdio);
	rsp_msg = ipc_send_rpc(req_msg,&ec);
	if (ec != IPC_OK) {
	    /* post an error */
	    retcode = RFS_ERROR_SENDING_IPC_REQUEST;
	} else {
	    rsp = rsp_msg->data;
	    switch (func) {
		case IOCTL_GET_DEV_TYPE:
		    /*
		     * Remember to clear the TYPE_LOCAL bit, since this
		     * file can't possibly be local.
		     */
		    *(int *)arg = rsp->arg.dev_type & ~IOCTL_DEV_TYPE_LOCAL;
		    break;

		case IOCTL_GET_BUSY_INFO:
		    /*
		     * Get the return value from the response message and
		     * save it for the caller.
		     */
		    *(int *)arg = rsp->arg.busy_info;
		    break;

		default:
		    break;
	    }
	    retcode = rsp->return_code;
	    ipc_return_message(rsp_msg);
	}
    }
    return retcode;
}


/******************************************************************************
 * Name: RFSC_subsys_init
 *
 * Description: initialization for the remote file system client
 *
 * Input: 
 *
 * Return Value:
 *
 *******************************************************************************/
static void RFSC_subsys_init (subsystype *subsys)
{
    int file_sys_type;

    /*------------------------------------------------------------------------*
     * Initialize all the upper layer stuff -- the file
     * system registration etc.
     *------------------------------------------------------------------------*/
    /*
     * register with the file system layer and get the file_sys_type
     * to use.
     */
    file_sys_type = FS_register("slaveslot0 slaveslot1 slavebootflash slavenvram", FS_STD_ERROR_CODE_ALLOCATION);

    /*
     * add all the client file system routines to the registries
     */
    reg_add_open(file_sys_type,RFSC_open,"RFSC_open");
    reg_add_close(file_sys_type,RFSC_close,"RFSC_close");
    reg_add_read(file_sys_type,RFSC_read,"RFSC_read");
    reg_add_write(file_sys_type,RFSC_write,"RFSC_write");
    reg_add_lseek(file_sys_type,RFSC_lseek,"RFSC_lseek");
    reg_add_stat(file_sys_type,RFSC_stat,"RFSC_stat");
    reg_add_fstat(file_sys_type,RFSC_fstat,"RFSC_fstat");
    reg_add_istat(file_sys_type,RFSC_istat,"RFSC_istat");
    reg_add_fsstat(file_sys_type,RFSC_fsstat,"RFSC_fsstat");
    reg_add_delete(file_sys_type,RFSC_delete,"RFSC_delete");
    reg_add_undelete(file_sys_type,RFSC_undelete,"RFSC_undelete");
    reg_add_squeeze(file_sys_type,RFSC_squeeze,"RFSC_squeeze");
    reg_add_settype(file_sys_type,RFSC_settype,"RFSC_settype");
    reg_add_getdents(file_sys_type,RFSC_getdents,"RFSC_getdents");
/* jack    reg_add_format(file_sys_type,RFSC_format,"RFSC_format"); */
    reg_add_geterrortext(file_sys_type,RFSC_geterrortext,"RFSC_geterrortext");
    reg_add_getdevicenames(file_sys_type,RFSC_getdevicenames,
			   "RFSC_getdevicenames");
    reg_add_ioctl(file_sys_type, RFSC_ioctl, "RFSC_ioctl");
    /*
    reg_add_fioctl(file_sys_type, RFSC_fioctl, "RFSC_fioctl");
    */
}

/*----------------------------------------------------------------------------*
 * Subsystem header for the remote file system client
 *----------------------------------------------------------------------------*/
SUBSYS_HEADER(remote_filesys_client,    /* name */
              1,                        /* major version */
              0,                        /* minor version */
              1,                        /* edit version */
              RFSC_subsys_init,         /* init function */
              SUBSYS_CLASS_DRIVER,      /* subsystem class */
              "seq: file_system",       /* sequence list */
              "req: ipc, file_system"   /* prerequisite list */
              );

