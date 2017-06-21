/* $Id: unix_file_io.c,v 3.1.2.2 1996/06/01 01:43:49 ahh Exp $
 * $Source: /release/112/cvs/Xsys/unix/unix_file_io.c,v $
 *------------------------------------------------------------------
 * Unix file I/O wrappers. 
 *
 * May  1996, Subodh Nijsure 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_file_io.c,v $
 * Revision 3.1.2.2  1996/06/01  01:43:49  ahh
 * CSCdi59219:  IP modularity broke UNIX builds
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/31  21:59:20  snijsure
 * CSCdi58178:  HTTP problem with flash access
 * Branch: California_branch
 * Implement FS_open, FS_close, FS_read using open,close, read for UNIX
 * image.
 *
 * Revision 3.1  1996/05/31  21:53:55  snijsure
 * Place holder file
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "ttysrv.h"
#include "../filesys/rommon_filesys.h"

extern int open(char *,int, ulong);
extern int close(int);
extern int read(int, void *,ulong);
/*
 * These following (FS_open, FS_read...) routines allow one to open, read 
 * flash files on all other platforms except UNIX and AGS. The following 
 * implementation is introduced in case of UNIX, so that one can still build 
 * the UNIX image.
 * 
 * These is an intermediate measure till full POSIX like "portable" 
 * file system API is implemented in future (colorado) for cisco platforms.
 * At which point use of FS_open.. will be replaced by open,close 
 */
int FS_open (const char *nm, int flag, ulong mode)
{
    return open(nm,flag,mode);     
}

int FS_close (int fd)
{
    return close(fd); 
}

int FS_read (int fd, void *buffer, int byte_count)
{
    return read(fd,buffer,byte_count); 
}


int FS_stat (const char *name, stat_t *buf)
{
    /* Since we can not include straight unix headers to get definition
     * of structure stat we will return -1.
     * Only other (kludgy) alternative is to copy 'struct stat' into a header
     * unix_file_io.h
     */
    return -1;
}

/*
 * Another stub
 */

void flash_copy_command (void)
{
    printf("\ncopy to or from flash not implemented");
}
