/* $Id: file_io_msgs.c,v 3.2 1995/11/17 09:00:22 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dev/file_io_msgs.c,v $
 * ------------------------------------------------------------------
 * error message strings for file I/O routines
 *
 * October 1995, Carl Schaefer
 *
 * Copyright (c) 1995, 1997 by cisco Systems, Inc.
 * All rights reserved.
 * ------------------------------------------------------------------
 * $Log: file_io_msgs.c,v $
 * Revision 3.2  1995/11/17  09:00:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:54:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/18  01:22:32  schaefer
 * placeholders for 7500 flash MIB
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../dev/file_io.h"

/*
 * Various file error messages.
 * NOTE: these are indexed through error codes in file_io.h
 */
static const char* const file_errmsg[] = {
    "file open successful",       /* FOPEN_OK                       */
    "invalid device",             /* FOPEN_INV_DEV                  */
    "device error",               /* FOPEN_DEVERR                   */
    "no memory",                  /* FOPEN_NOMEM                    */
    "file open table overflow",   /* FOPEN_TBLOVER                  */
    "file system error",          /* FOPEN_FSERR                    */
    "file not found",             /* FOPEN_NOFILE                   */
    "user abort",                 /* FOPEN_USRABORT                 */
    "device needs erasure",       /* FOPEN_NEED_ERASE               */
    "duplicate file",             /* FOPEN_FILEDUP                  */
    "file system full",           /* FOPEN_FSFULL                   */
    "invalid file open mode",     /* FOPEN_INV_MODE                 */
    "opening deleted file",       /* FOPEN_DEL                      */
    "file checksum error",        /* FOPEN_CKSUM_ERR                */
    "file name exceeds 47 chars", /* FOPEN_FNM_ERR                  */
    "unknown",                    /* FOPEN_LAST                     */
};
#define FILE_ERR_LAST (sizeof(file_errmsg) / sizeof(file_errmsg[0]))

const char *file_error_msg (int err)
{
    err = 0 - err;  /* make error code a positive number */
    return (((uint) err >= FILE_ERR_LAST) ? file_errmsg[FILE_ERR_LAST - 1] :
                                     file_errmsg[err]);
}

