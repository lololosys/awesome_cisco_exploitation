/* $Id: remote_filesys.h,v 3.3 1995/12/18 07:09:06 schaefer Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/filesys/remote_filesys.h,v $
 *------------------------------------------------------------------
 * Include file used for remote file system master-slave interface
 *
 * June 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: remote_filesys.h,v $
 * Revision 3.3  1995/12/18  07:09:06  schaefer
 * CSCdi45771: include slave devices in 75xx flash mib
 *
 * Revision 3.2  1995/11/17  09:08:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  17:04:27  yanke
 * Placeholders
 *
 * CSCdi39151:  modulized local/remote file systems
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#if !defined(__remote_filesys_h__)
#define __remote_filesys_h__

#include "filesys.h"

/*----------------------------------------------------------------------------*
 * Return Codes for the remote file system
 *----------------------------------------------------------------------------*/
#define RFS_OK                                   0
#define RFS_ERROR_UNABLE_TO_GET_IPC_MSG_BUFFER  -1
#define RFS_ERROR_SENDING_IPC_REQUEST           -2
#define RFS_ERROR_BUFFER_OVERFLOW               -3

/*----------------------------------------------------------------------------*
 * A bunch of useful defines that are needed for the client/server stuff
 *----------------------------------------------------------------------------*/
#define RFS_FILE_NAME_MAX           FS_FILE_NAME_MAX
#define RFS_DEV_ID_MAX              FS_DEV_ID_MAX
/* need one extra byte for path separator character */
#define RFS_FILE_ID_MAX            (RFS_DEV_ID_MAX + 1 + RFS_FILE_NAME_MAX)
#define RFS_DATA_BUFFER_SIZE        2048
#define RFS_DIRENT_BUFFER_MAX       RFS_DATA_BUFFER_SIZE
#define RFS_DEVICE_NAMES_BUFFER_MAX RFS_DATA_BUFFER_SIZE
#define RFS_ERROR_TEXT_MAX          RFS_DATA_BUFFER_SIZE

#define RFS_SERVER_MASTER_PORT_NAME "Remote File System Server Port"
#define RFS_SERVER_SLAVE_PORT_NAME  "Slave RSP Remote File System Server Port"

/*----------------------------------------------------------------------------*
 * Definitions of the message type for the remote file system
 * client/server interface.
 *----------------------------------------------------------------------------*/
typedef enum rfs_msg_type_ {
    RFS_OPEN_REQ,
    RFS_CLOSE_REQ,
    RFS_READ_REQ,
    RFS_WRITE_REQ,
    RFS_LSEEK_REQ,
    RFS_STAT_REQ,
    RFS_FSTAT_REQ,
    RFS_ISTAT_REQ,
    RFS_FSSTAT_REQ,
    RFS_DELETE_REQ,
    RFS_UNDELETE_REQ,
    RFS_SQUEEZE_REQ,
    RFS_SETTYPE_REQ,
    RFS_GETDENTS_REQ,
    RFS_FORMAT_REQ,
    RFS_GETERRORTEXT_REQ,
    RFS_GETDEVICENAMES_REQ,
    RFS_IOCTL_REQ,

    RFS_OPEN_RSP,
    RFS_CLOSE_RSP,
    RFS_READ_RSP,
    RFS_WRITE_RSP,
    RFS_LSEEK_RSP,
    RFS_STAT_RSP,
    RFS_FSTAT_RSP,
    RFS_ISTAT_RSP,
    RFS_FSSTAT_RSP,
    RFS_DELETE_RSP,
    RFS_UNDELETE_RSP,
    RFS_SQUEEZE_RSP,
    RFS_SETTYPE_RSP,
    RFS_GETDENTS_RSP,
    RFS_FORMAT_RSP,
    RFS_GETERRORTEXT_RSP,
    RFS_GETDEVICENAMES_RSP,
    RFS_IOCTL_RSP,
} rfs_msg_type;


/*----------------------------------------------------------------------------*
 * Definitions for IPC messages used in remote file system
 *
 * These are paired sets of request/response message types.  The
 * requests end with "_req", and the responses end with "_rsp".
 *
 *----------------------------------------------------------------------------*/
typedef struct rfs_open_req_ {
    char  file_id[RFS_FILE_NAME_MAX + 1];
    int   flags;
    ulong mode;
} rfs_open_req_t;

typedef struct rfs_open_rsp_ {
    int   fd;
} rfs_open_rsp_t;


typedef struct rfs_close_req_ {
    int   fd;
} rfs_close_req_t;

typedef struct rfs_close_rsp_ {
    int   return_code;
} rfs_close_rsp_t;


typedef struct rfs_read_req_ {
    int   fd;
    int   byte_count;
} rfs_read_req_t;

typedef struct rfs_read_rsp_ {
    int   actual;
    char  data[RFS_DATA_BUFFER_SIZE];
} rfs_read_rsp_t;


typedef struct rfs_write_req_ {
    int   fd;
    int   byte_count;
    char  data[RFS_DATA_BUFFER_SIZE];
} rfs_write_req_t;

typedef struct rfs_write_rsp_ {
    int   actual;
} rfs_write_rsp_t;


typedef struct rfs_lseek_req_ {
    int   fd;
    int   offset;
    int   whence;
} rfs_lseek_req_t;

typedef struct rfs_lseek_rsp_ {
    int   curr_ptr;
} rfs_lseek_rsp_t;


typedef struct rfs_stat_req_ {
    char  file_id[RFS_FILE_ID_MAX + 1];
} rfs_stat_req_t;

typedef struct rfs_stat_rsp_ {
    int   return_code;
    stat_t status;
} rfs_stat_rsp_t;


typedef struct rfs_fstat_req_ {
    int   fd;
} rfs_fstat_req_t;

typedef struct rfs_fstat_rsp_ {
    int   return_code;
    stat_t status;
} rfs_fstat_rsp_t;


typedef struct rfs_istat_req_ {
    char  dev_id[RFS_DEV_ID_MAX + 1];
    int   index;
} rfs_istat_req_t;

typedef struct rfs_istat_rsp_ {
    int   return_code;
    stat_t status;
} rfs_istat_rsp_t;


typedef struct rfs_fsstat_req_ {
    char  dev_id[RFS_DEV_ID_MAX + 1];
} rfs_fsstat_req_t;

typedef struct rfs_fsstat_rsp_ {
    int   return_code;
    file_sys_stat_t status;
} rfs_fsstat_rsp_t;


typedef struct rfs_delete_req_ {
    char  file_id[RFS_FILE_ID_MAX + 1];
} rfs_delete_req_t;

typedef struct rfs_delete_rsp_ {
    int   return_code;
} rfs_delete_rsp_t;


typedef struct rfs_undelete_req_ {
    char  dev_id[RFS_DEV_ID_MAX + 1];
    int   index;
    int   mode;
} rfs_undelete_req_t;

typedef struct rfs_undelete_rsp_ {
    int   return_code;
} rfs_undelete_rsp_t;


typedef struct rfs_squeeze_req_ {
    char  dev_id[RFS_DEV_ID_MAX + 1];
    int   flag;
} rfs_squeeze_req_t;

typedef struct rfs_squeeze_rsp_ {
    int   return_code;
} rfs_squeeze_rsp_t;


typedef struct rfs_settype_req_ {
    char  file_id[RFS_FILE_ID_MAX + 1];
    ulong file_type;
} rfs_settype_req_t;

typedef struct rfs_settype_rsp_ {
    int   return_code;
} rfs_settype_rsp_t;


typedef struct rfs_getdents_req_ {
    int   fd;
    int   buffer_length;
} rfs_getdents_req_t;

typedef struct rfs_getdents_rsp_ {
    int   byte_count;
    char  buffer[RFS_DIRENT_BUFFER_MAX];
} rfs_getdents_rsp_t;


typedef struct rfs_format_req_ {
    char  dev_id[RFS_DEV_ID_MAX + 1];
    int   options;
} rfs_format_req_t;

typedef struct rfs_format_rsp_ {
    int   return_code;
} rfs_format_rsp_t;


typedef struct rfs_geterrortext_req_ {
    int   error_code;
    int   buffer_len;
} rfs_geterrortext_req_t;

typedef struct rfs_geterrortext_rsp_ {
    char  buffer[RFS_ERROR_TEXT_MAX];
} rfs_geterrortext_rsp_t;


typedef struct rfs_getdevicenames_req_ {
    int   buffer_len;
} rfs_getdevicenames_req_t;

typedef struct rfs_getdevicenames_rsp_ {
    int   return_code;
    char  buffer[RFS_DEVICE_NAMES_BUFFER_MAX];
} rfs_getdevicenames_rsp_t;

typedef union ioctl_arg_ {
    format_parm_t format_parm;
    int verbose_flag;
    int dev_type;
    int busy_info;
} ioctl_arg_t;

typedef struct rfs_ioctl_req_ {
    char file_id[RFS_FILE_ID_MAX + 1];
    int func;
    ioctl_arg_t arg;
    int server_ttynum;
} rfs_ioctl_req_t;

typedef struct rfs_ioctl_rsp_ {
    int return_code;
    ioctl_arg_t arg;
} rfs_ioctl_rsp_t;

extern char const slave_prefix[];

#endif /* !defined(__remote_filesys_h__) */
