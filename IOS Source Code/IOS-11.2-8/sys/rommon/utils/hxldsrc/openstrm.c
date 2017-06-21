/* $Id: openstrm.c,v 3.2 1995/11/17 18:49:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/utils/hxldsrc/openstrm.c,v $
 *------------------------------------------------------------------
 * openstrm.c
 *
 * June 1993, Rob Clevenger
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Open the output stream.
 *------------------------------------------------------------------
 * $Log: openstrm.c,v $
 * Revision 3.2  1995/11/17  18:49:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:10:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:43:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <termio.h>
#include <fcntl.h>
#include <sys/types.h>

struct termio saveterm;
int fd;
extern char *progname, shocount;
extern FILE *outfile;

openstrm(strm_name)
    char *strm_name;
{
    struct termio termbuf;
  
    if((fd = open(strm_name, O_WRONLY|O_CREAT|O_TRUNC, 0666)) < 0) {
	fprintf(stderr,"%s: cannot open %s\n",progname,strm_name);
	exit(1);
    }
    outfile = fdopen(fd,"w");

#ifdef NOTYET

    if(isatty(fd)) {
	ioctl(fd, TCGETA, &saveterm); /* save io control block */
	ioctl(fd, TCGETA, &termbuf); /* get io control block */
	termbuf.c_iflag = IGNBRK|IGNPAR|IXON; /* change it */
	termbuf.c_oflag = OPOST | OLCUC; /* map lower case to upper */
	termbuf.c_cflag = B9600|CS8;
	termbuf.c_lflag = 0;
	termbuf.c_line = 1;
	termbuf.c_cc[VMIN] = 1;
	termbuf.c_cc[VTIME] = 0;
	ioctl(fd, TCSETAF, &termbuf); /* put io control block */
	shocount = 1;  /* show activity on console by displaying block count */
    }
#endif

}

closestrm()
{
#ifdef NOTYET
    if(isatty(fd)) { /* restore original io control block */
	ioctl(fd, TCSETAW, &saveterm);
    }
#endif
    close(fd);
}
