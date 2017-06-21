/* $Id: fsutils.c,v 3.2.58.2 1996/06/16 21:18:14 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/fsutils.c,v $
 *------------------------------------------------------------------
 * fsutils.c
 *
 * February 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * File system utility programs.
 *------------------------------------------------------------------
 * $Log: fsutils.c,v $
 * Revision 3.2.58.2  1996/06/16  21:18:14  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:27:14  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:20  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "monitor.h"
#include "getdents.h"
#include "fsio.h"
#include "fstat.h"
#include "simpfsio.h"
#include "dev.h"

/*
** Concatenate all files specified on the command line or dump them as
** ascii hex.  The action depends on argv[0] being cat or fdump.
*/
int
cat(int argc, char *argv[])
{
  register i, j, num, fd;
  char dump = 0;  /* dump flag */
  int daddr, retval = 0;
  char buffer[32];

  if(strcmp(argv[0],"fdump") == 0) dump = 1;
  if(argc < 2) {
usage:
    printf("usage: %s filename1 [filename2...]\n",argv[0]);
    return(1);
  }
  for(i=1; i<argc; i++) {
    daddr = 0;  /* reset for new file */
    if((fd = open(argv[i],O_RDONLY)) < 0) {
      printf("cannot open \"%s\"\n",argv[i]);
      return(1);
    }
    while(1) {
      filbyte(buffer,sizeof(buffer),0);
      if((num = read(fd,buffer,sizeof(buffer))) == 0) break;
      if(num < 0) {
	printf("%s: read error...aborting\n", argv[0]);
	retval = 1;
	break;
      }
      if(dump) {
	dismem(buffer,num,daddr,1);  /* display as bytes */
	daddr += num;
      } else {
	  for(j=0; j<num; j++) putchar(buffer[j]);
      }
      if(num != sizeof(buffer)) break;
    }
    close(fd);
  }
  return(retval);
}


/*
** List the files in a file system
*/
int
directory(int argc, char *argv[])
{
    int fd, i;
    char *bptr;
    char buffer[80];

    if(argc < 2) {
usage:
	printf("usage: %s <device>\n", argv[0]);
	return(1);
    }
    i = isdevname(argv[1]);
    if(i <= 0) {
	printf("bad device name\n");
	goto usage;
    }
    if(i && !(devtbl[getdevnum(argv[1])].flags & DEV_FS_CAPABLE)) {
	printf("not a filesystem device\n");
	goto usage;
    }

    sprintf(buffer, "%s.", argv[1]);  /* append a '.' to get to the filesystem */
    if((fd = open(buffer,O_RDONLY)) < 0) {
	printf("dir: cannot open device \"%s\"\n", argv[1]);
	return(1);
    }

    return(print_dir_info(fd));
}

/*
** Print the directory info for the file system
*/

int
print_dir_info(int fd)
{

    struct dirent_ dir;
    int i;

    puts("         File size           Checksum   File name\n");
    while(getdents(fd, &dir, sizeof(struct dirent_))) {
	i = printf("%10d bytes (%#x)", dir.size, dir.size);
	while(i++ < 30) putchar(' ');  /* pad with spaces */
	printf("%#.4x    ", dir.crc);
	printf("%s", dir.d_name);

	if (dir.filetype == SIMPLE_FS_TYPE) {
	    /* deal with simple file system */
	    if(!(dir.flags & FFH_FLAGS_DELETED)) printf(" (deleted)");
	    if(dir.crc != dir.achecksum)
		printf(" (bad checksum: %#.4x)",dir.achecksum);
	} else {
	    /* deal with new file system */
	    if (dir.deleted)
		printf(" (deleted)");
	    else if (!dir.valid)
		printf(" (invalid)");
	}

	putchar('\n');
    }
    close(fd);
    return(0);
}

/*
** List all of the devs in the device table
** for this platform.
*/
int
devs(int argc, char *argv[])
{
    int i;
    struct devstruct *devp = devtbl;

    printf("Devices in device table:\n        id  name\n");
    for(i=0; i<numdevstructs; i++, devp++) {
	if(!(devp->flags & DEV_FS_CAPABLE) &&
	   !(hkeepflags & H_PRIVILEGED)) continue;
	printf("%10s  %-25s  ", devp->devid, devp->devname);
	if(devp->flags & DEV_FS_CAPABLE &&
	   (hkeepflags & H_PRIVILEGED))
	    printf("(device is file system capable)");
	putchar('\n');
    }
    return(0);
}
