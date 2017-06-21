/* $Id: remote_filesys_server.c,v 3.4.42.2 1996/08/15 02:04:28 banderse Exp $
 * $Source: /release/112/cvs/Xsys/filesys/remote_filesys_server.c,v $
 *------------------------------------------------------------------
 * Remote file system server side -- runs on the slave RSP image
 *                                   Sep. 95 - also on the RP image
 *
 * June 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: remote_filesys_server.c,v $
 * Revision 3.4.42.2  1996/08/15  02:04:28  banderse
 * CSCdi64706:  Memory Allocation failure on RSP during IPC process
 * Branch: California_branch
 * Removed ONE_SEC sleep from RFSS_server_action. Multiple instances hog
 * stack space
 *
 * Revision 3.4.42.1  1996/03/18  19:35:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/07  09:36:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  13:50:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/18  17:07:40  schaefer
 * CSCdi45771: include slave devices in 75xx flash mib
 *
 * Revision 3.3  1995/12/01  04:01:38  yanke
 * Use strdup instead of malloc.
 *
 * CSCdi44796:  need to use strpdup for trailing null
 *
 * Revision 3.2  1995/11/17  09:08:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:25  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  17:04:29  yanke
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
#include "fs_rec.h"
#include "remote_filesys.h"
#include "../h/ttysrv.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ipc/ipc_private.h"
#include "../ipc/ipc_message.h"
#include "../ipc/ipc_seats.h"
#include "../ipc/ipc_ports.h"
#include "../ipc/remote_tty.h"

#include "sys_registry.h"
#include "../dev/flash_registry.h"

extern const uchar monlib_bundle[];
extern int monlib_bundle_size;

const char slave_prefix[] = "slave";

#define RFS_STRIP_PREFIX(x) ((x) + RFS_file_dev_prefix)
static int RFS_file_dev_prefix;
static char *RFS_ipc_port_name;


/******************************************************************************
 * Name: RFSS_server
 *
 * Description: 
 *
 * Input: 
 *
 * Return Value:
 *
 *******************************************************************************/
static process RFSS_server_action (void)
{
    ipc_message *req_msg;
    ipc_message *rsp_msg;
    ipc_message_header *req_hdr;
    char *file_id;
    char *dev_id;

    if (!process_get_arg_ptr((void**)&req_msg))
	process_kill(THIS_PROCESS);

    req_hdr = req_msg->header;
    switch (req_hdr->type) {
    /*
     *
     */
    case RFS_OPEN_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_open_rsp_t),0,RFS_OPEN_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_open_req_t *req;
	    rfs_open_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;
	    file_id = RFS_STRIP_PREFIX(req->file_id);
	    rsp->fd = FS_open(file_id,req->flags,req->mode);
	    reg_invoke_RFSS_watchdog_start(file_id, rsp->fd);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_CLOSE_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_close_rsp_t),0,RFS_CLOSE_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_close_req_t *req;
	    rfs_close_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;

	    reg_invoke_RFSS_watchdog_stop(req->fd);

	    rsp->return_code = FS_close(req->fd);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_READ_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_read_rsp_t),0,RFS_READ_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_read_req_t *req;
	    rfs_read_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;

	    rsp->actual = FS_read(req->fd,rsp->data,req->byte_count);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_WRITE_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_write_rsp_t),0,RFS_WRITE_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_write_req_t *req;
	    rfs_write_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;

	    rsp->actual = FS_write(req->fd,req->data,req->byte_count);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_LSEEK_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_lseek_rsp_t),0,RFS_LSEEK_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_lseek_req_t *req;
	    rfs_lseek_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;

	    rsp->curr_ptr = FS_lseek(req->fd,req->offset,req->whence);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_STAT_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_stat_rsp_t),0,RFS_STAT_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_stat_req_t *req;
	    rfs_stat_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;
	    file_id = RFS_STRIP_PREFIX(req->file_id);
	    rsp->return_code = FS_stat(file_id,&rsp->status);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_FSTAT_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_fstat_rsp_t),0,RFS_FSTAT_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_fstat_req_t *req;
	    rfs_fstat_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;

	    rsp->return_code = FS_fstat(req->fd,&rsp->status);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_ISTAT_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_istat_rsp_t),0,RFS_ISTAT_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_istat_req_t *req;
	    rfs_istat_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;
	    dev_id = RFS_STRIP_PREFIX(req->dev_id);
	    rsp->return_code = FS_istat(dev_id,req->index,&rsp->status);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_FSSTAT_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_fsstat_rsp_t),0,RFS_FSSTAT_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_fsstat_req_t *req;
	    rfs_fsstat_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;
	    dev_id = RFS_STRIP_PREFIX(req->dev_id);
	    rsp->return_code = FS_fsstat(dev_id,&rsp->status);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_DELETE_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_delete_rsp_t),0,RFS_DELETE_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_delete_req_t *req;
	    rfs_delete_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;
	    file_id = RFS_STRIP_PREFIX(req->file_id);
	    rsp->return_code = FS_delete(file_id);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_UNDELETE_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_undelete_rsp_t),0,RFS_UNDELETE_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_undelete_req_t *req;
	    rfs_undelete_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;
	    dev_id = RFS_STRIP_PREFIX(req->dev_id);
	    rsp->return_code = FS_undelete(dev_id,req->index,req->mode);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_SQUEEZE_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_squeeze_rsp_t),0,RFS_SQUEEZE_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_squeeze_req_t *req;
	    rfs_squeeze_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;
	    dev_id = RFS_STRIP_PREFIX(req->dev_id);
	    rsp->return_code = FS_squeeze(dev_id,req->flag);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_SETTYPE_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_settype_rsp_t),0,RFS_SETTYPE_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_settype_req_t *req;
	    rfs_settype_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;
	    file_id = RFS_STRIP_PREFIX(req->file_id);
	    rsp->return_code = FS_settype(file_id,req->file_type);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_GETDENTS_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_getdents_rsp_t),0,RFS_GETDENTS_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_getdents_req_t *req;
	    rfs_getdents_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;

	    rsp->byte_count = FS_getdents(req->fd,rsp->buffer,req->buffer_length);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    case RFS_FORMAT_REQ:
	/* not sure how this will work yet */
	break;
    /*
     *
     */
    case RFS_GETERRORTEXT_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_geterrortext_rsp_t),0,RFS_GETERRORTEXT_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_geterrortext_req_t *req;
	    rfs_geterrortext_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;

	    FS_geterrortext(req->error_code,rsp->buffer,req->buffer_len);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_GETDEVICENAMES_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_getdevicenames_rsp_t),0,RFS_GETDEVICENAMES_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_getdevicenames_req_t *req;
	    rfs_getdevicenames_rsp_t *rsp;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;

	    rsp->return_code = FS_getdevicenames(rsp->buffer,req->buffer_len);
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    /*
     *
     */
    case RFS_IOCTL_REQ:
	rsp_msg = ipc_get_message(sizeof(rfs_ioctl_rsp_t),0,RFS_IOCTL_RSP);
	if (rsp_msg == NULL) {
	    /* post an error */
	} else {
	    rfs_ioctl_req_t *req;
	    rfs_ioctl_rsp_t *rsp;
	    void *arg;
	    
	    req = req_msg->data;
	    rsp = rsp_msg->data;

	    switch (req->func) {
		case IOCTL_FORMAT_FLASH:
		    /*
		     * If format is specified, we need to
		     * use the monlib bundled in the slave image.
		     * In case monlib is specified as file id,
		     * then they are assumed to be locally on slave.
		     */
		    req->arg.format_parm.monlib_bndl=(char *)&monlib_bundle[0];
		    req->arg.format_parm.monlib_bndl_size = monlib_bundle_size;
	    	    memcpy(&rsp->arg, &req->arg, sizeof(format_parm_t));
		    arg = (void *)&rsp->arg;
		    break;

		case IOCTL_SET_VERBOSE:
		    arg = (void *)req->arg.verbose_flag;
		    break;

		case IOCTL_GET_BUSY_INFO:
		    arg = (void *)&rsp->arg.busy_info;
		    break;

		default:
		    arg = (void *)&rsp->arg;
		    break;
	    }
	    if (req->file_id[0]) {
		file_id = RFS_STRIP_PREFIX(req->file_id);
	    }
	    else
		file_id = NULL;
	    RTTYC_reassign_tty(req->server_ttynum);
	    rsp->return_code = FS_ioctl(file_id, req->func, arg);
	    RTTYC_restore_tty();
	    ipc_send_rpc_reply(req_msg,rsp_msg);
	}
	break;
    }

    process_kill(THIS_PROCESS);
}

static void RFSS_server (ipc_message *req_msg, void *context, ipc_error_code ec)
{
    int result;

    /*
     * Create a process to do the work, so we can return
     * to handle other ipc request messages.
     */
    result = process_create(RFSS_server_action,
	"RFSS_server_action", LARGE_STACK, PRIO_NORMAL);
    if (result != NO_PROCESS) {
	process_set_arg_ptr(result, req_msg);
    }
}

static void RFSS_reg_server_port (void)
{
    ipc_port_id server_port;
    ipc_error_code retcode;

    /*
     * By now the port may have been renumber,
     * so we need to get it again by the name.
     */
    server_port = ipc_locate_port(RFS_ipc_port_name);
    
    retcode = ipc_register_port(server_port);
    if (retcode != IPC_OK) {
	/* post an error */
	;
    }
}


/******************************************************************************
 * Name: RFSS_init
 *
 * Description: initialization for the remote file system server
 *
 * Input: 
 *
 * Return Value:
 *
 *******************************************************************************/
int RFSS_init (boolean master)
{
    ipc_error_code ec;
    ipc_port_id server_port;

    RTTYS_init(master);

    /*
     * Create the server port and set the server call-back function to
     * handle messages that arrive.
     */

    if (master) {
	RFS_file_dev_prefix = 0;
	RFS_ipc_port_name = strdup(RFS_SERVER_MASTER_PORT_NAME);
    } else {
	RFS_file_dev_prefix = strlen(slave_prefix);
	RFS_ipc_port_name = strdup(RFS_SERVER_SLAVE_PORT_NAME);
    }
    if (RFS_ipc_port_name == NULL)
	return -1;
    ec = ipc_create_named_port(RFS_ipc_port_name, &server_port,0);
    if (ec != IPC_OK) {
	/* post an error */
	return -1;
    } else {
	ec = ipc_set_receive_callback(server_port,0,RFSS_server);
    }

    /*
     * Can not register our server port right now as IPC
     * may have not been initialized yet. Register an init
     * function for later invocation.
     */
    ipc_register_delayed_func(IPC_LEVEL_FILESYS, RFSS_reg_server_port);

    return 0;
}
