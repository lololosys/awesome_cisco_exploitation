/* $Id: unix_io.c,v 3.2.62.3 1996/09/13 04:15:03 smackie Exp $
 * $Source: /release/112/cvs/Xsys/unix/unix_io.c,v $
 *------------------------------------------------------------------
 * unix_io.c -- cisco under UNIX -- calls to UNIX standard I/O library
 *
 * 12-December-1991, Kirk Lougheed
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_io.c,v $
 * Revision 3.2.62.3  1996/09/13  04:15:03  smackie
 * Fix some of the major unix image unhappiness. (CSCdi69048)
 *
 *    o Remove problematic unix NIT support
 *    o Change default memory pool alignment to align 64 bit integers
 *    o Fix message compilation errors
 *
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/06/28  23:31:22  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:30:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/09  05:14:33  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.2  1996/03/07  10:59:09  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:27:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:55:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/13  04:07:30  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.2  1995/09/01 06:55:08  hampton
 * Make the unix images compile again.  [CSCdi39708]
 *
 * Revision 2.1  1995/06/07 23:12:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We compile this module using the standard UNIX include paths
 */
#include "unix.h"
#include <stdio.h>
#ifdef __svr4__
#undef boolean		/* "boolean" is an enum in sys/types.h on Solaris */
#endif /* svr4 */
#include <sys/types.h>
#ifdef __svr4__
#define boolean int	/* ... but we still need our definition from unix.h */
#endif /* svr4 */
#include <sys/ioctl.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <string.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <net/if.h>
#ifndef __svr4__
#include <net/nit_if.h>
#else /* __svr4__ */
#include <sys/sockio.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/dlpi.h>
#endif /* __svr4__ */

#ifndef __svr4__
static struct sgttyb ttymode;
#else /* __svr4__ */
static struct termios ttymode;
#endif /* __svr4__ */


/*
 * unix_stdinBLOCK
 * Wait for user typin to appear on the standard input
 */

int unix_stdinBLOCK ()
{
    fd_set nullfile, readfile;
    struct timeval timeout;
    int nready;

    FD_ZERO(&nullfile);
    FD_ZERO(&readfile);
    FD_SET(0, &readfile);
    memset(&timeout, 0, sizeof(struct timeval));
    nready = select(8*sizeof(int), &readfile, NULL, NULL, &timeout);
    return((nready == 0) ? TRUE : FALSE);
}

/*
 * unix_stdinread
 * Read some data from stdin.  A jacket for the read() call.
 */

int unix_stdinread (input, size)
    char *input;
    int size;
{
    return(read(0, input, size));
}

/*
 * unix_putc
 * Write a character to UNIX (not cisco) standard output
 */

void unix_putc (ch)
    char ch;
{
    write(1, &ch, 1);
}

/*
 * unix_read()
 * cisco wrapper for read(2)
 */

int unix_read (fd, buf, size)
     int fd;
     char* buf;
     u_int size;
{
    return read(fd, buf, size);
}

/*
 * unix_read_file
 * Read a UNIX configuration file into a buffer, returning byte count
 */

boolean unix_read_file (name, buffer, inbytes, maxbytes)
    char *name;
    unsigned char *buffer;
    long *inbytes;
    long maxbytes;
{
    int fid;

    fid = open(name, O_RDONLY);
    if (fid == -1)
	return(FALSE);
    *inbytes = read(fid, buffer, maxbytes);
    close(fid);
    return((*inbytes > 0) ? TRUE : FALSE);
}

/*
 * unix_write_file
 * Write a buffer of configuration commands to the specified UNIX file
 */

boolean unix_write_file (name, buffer, bytes)
    char *name;
    unsigned char *buffer;
    long bytes;
{
    int fid, result;

    fid = open(name, (O_WRONLY|O_CREAT), (S_IRWXU|S_IRWXG));
    if (fid == -1)
	return(FALSE);
    result = write(fid, buffer, bytes);
    close(fid);
    return((result == bytes) ? TRUE : FALSE);
}

/*
 * unix_remove_file
 */
void unix_remove_file (char *name)
{
    unlink(name);
}


/*
 * gdb_command()
 * pass control to gdb (router process dies if gdb isn't present)
 */
void gdb_command ()
{
    kill(0, SIGINT);
    return;
}
